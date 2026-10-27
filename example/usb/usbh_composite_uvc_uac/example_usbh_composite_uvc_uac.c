/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "os_wrapper.h"
#include "usbh_uvc.h"
#include "usbh_uac1.h"

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
#include "audio/audio_control.h"
#include "audio/audio_equalizer.h"
#include "audio/audio_track.h"
#include "audio/audio_service.h"
#include "common/audio_errnos.h"
#endif

/* Private defines -----------------------------------------------------------*/

/**
 * @brief Enable hot-plug support for memory leak testing
 * When enabled, the system will automatically reinitialize after device removal
 */
#define USBH_UVC_UAC_HOT_PLUG_TEST            1

#define USBH_UAC_VOLUME_STEP                  20

/**
 * @brief Statistics print interval in milliseconds
 * Play/record threads log their loop counters once per this period
 */
#define USBH_UAC_DEBUG_TRACE_STEP             1000

/**
 * @brief Time to wait after an async set_mute/set_volume before issuing the
 *        sync read-back. The set path is event-driven via SOF, so we give the
 *        state machine a few SOFs to flush before querying.
 */
#define USBH_UAC_VERIFY_SETTLE_MS             50

/**
 * @brief Number of frames for ISOC transfer buffer
 * Affects latency and buffer size for audio streaming
 */
#define USBH_UAC_FRAME_CNT                    20

/**
 * @brief Record buffer size for audio capture
 */
#define USBH_UAC_RECORD_BUFFER_SIZE           1024

/* UAC playback configuration: preferred channel count and sampling frequency.
 * The stream thread iterates the device's advertised format list and selects
 * the entry that matches these values; falls back to index 0 if not found. */
#define USBH_UAC_PLAY_CHANNELS                2
#define USBH_UAC_PLAY_BITWIDTH                16
#define USBH_UAC_PLAY_SAMPLING_FREQ           48000

/* UAC capture configuration: this camera is mono-only (CH=1); 48kHz is fmt[5]. */
#define USBH_UAC_REC_CHANNELS                 1
#define USBH_UAC_REC_BITWIDTH                 16
#define USBH_UAC_REC_SAMPLING_FREQ            48000

/* UVC stream configuration.
 * Must name a resolution the camera actually advertises for the requested
 * format, otherwise the device silently negotiates down in PROBE and the
 * returned bFrameIndex no longer matches what was asked for. */
#define USBH_UVC_STREAM_WIDTH                 1920
#define USBH_UVC_STREAM_HEIGHT                1080
#define USBH_UVC_STREAM_FRAMERATE             30
#define USBH_UVC_STREAM_INDEX                 0

/* Frame buffer size in bytes.
 * H264 @ 1080p 30 fps: I-frames peak well under 250 KB. */
#define USBH_UVC_FRAME_BUF_SIZE               (250 * 1024)

/* Ctrl setup timeouts: UVC Probe/Commit callback and UAC CLASS_REQUEST ready */
#define USBH_COMPOSITE_CTRL_TIMEOUT_MS        2000U

/* Silence buffer size for playback demo (bytes per usbh_uac_write call) */
#define USBH_UAC_SILENCE_BUF_SIZE             256U

/* Timeout passed to usbh_uac_write / usbh_uac_read, in milliseconds */
#define USBH_UAC_WRITE_TIMEOUT_MS             10U
#define USBH_UAC_READ_TIMEOUT_MS              1000U

/* Inter-iteration delay in the play data loop */
#define USBH_UAC_PLAY_LOOP_DELAY_MS           10U

/* Polling delay when usbh_uvc_get_frame returns NULL */
#define USBH_UVC_FRAME_POLL_DELAY_MS          1U

/* Polling delay used by the ctrl thread while waiting for the device to detach */
#define USBH_COMPOSITE_DETACH_POLL_MS         50U

/* Thread priority definitions (higher number = higher priority) */
#define USBH_UAC_PLAY_THREAD_PRIORITY         4       /* Audio playback thread */
#define USBH_UAC_RECORD_THREAD_PRIORITY       4       /* Audio record thread */
#define USBH_UVC_CTRL_THREAD_PRIORITY         3       /* UVC/UAC ctrl setup thread */
#define USBH_UVC_STREAM_THREAD_PRIORITY       3       /* UVC frame data thread */
#define USBH_UVC_UAC_MAIN_THREAD_PRIORITY     5       /* USB host main thread */
#define USBH_UVC_UAC_HOTPLUG_THREAD_PRIORITY  6       /* Hot-plug detection thread */

/* Thread stack sizes in bytes */
#define USBH_UVC_CTRL_THREAD_STACK_SIZE       704U    /* UVC/UAC ctrl setup thread, measured used=448B */
#define USBH_UVC_STREAM_THREAD_STACK_SIZE     512U    /* UVC frame data thread, measured used=264B */

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static u32 usbh_composite_cmd(u16 argc, u8 *argv[]);

static void example_usbh_uac_uvc_thread(void *param);
static void usbh_uvc_uac_ctrl_thread(void *param);
static void usbh_uvc_stream_thread(void *param);
static void usbh_uac_play_thread(void *param);
static void usbh_uac_record_thread(void *param);
#if USBH_UVC_UAC_HOT_PLUG_TEST
static void usbh_uvc_uac_hotplug_thread(void *param);
#endif
static int usbh_uvc_cb_init(void);
static void usbh_uvc_cb_deinit(void);
static void usbh_uvc_cb_attached(void);
static void usbh_uvc_cb_detached(void);
static void usbh_uvc_cb_setup(void);
static void usbh_uvc_cb_setparam(int status);
static int usbh_uac_cb_init(void);
static void usbh_uac_cb_deinit(void);
static void usbh_uac_cb_attached(void);
static void usbh_uac_cb_detached(void);
static void usbh_uac_cb_setup(void);
static void usbh_uac_cb_process(usb_host_t *host, u8 msg);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "COMP";

/**
 * @brief Semaphore signaled when device is detached
 * Used to trigger hot-plug reinitialization sequence
 */
static rtos_sema_t usbh_uvc_uac_detach_sema;

/**
 * @brief Semaphore signaled when playback device setup is complete
 * Indicates audio playback can begin
 */
static rtos_sema_t usbh_uac_play_start_sema;

/**
 * @brief Semaphore signaled when record device setup is complete
 * Indicates audio capture can begin
 */
static rtos_sema_t usbh_uac_record_start_sema;

/**
 * @brief Semaphore signaled when UVC setup is complete
 * Indicates video streaming can begin
 */
static rtos_sema_t usbh_uvc_stream_start_sema;

/**
 * @brief Semaphore signaled when UVC set_param (Probe/Commit/SET_INTERFACE) completes
 */
static rtos_sema_t usbh_uvc_setparam_sema;

/**
 * @brief Semaphore signaled when UAC CLASS_REQUEST setup completes.
 * The ctrl thread waits on this before issuing UAC set_alt_setting so
 * that it does not race with the UAC class driver's own setup transfers.
 */
static rtos_sema_t usbh_uac_ready_sema;

/**
 * @brief Semaphore signaled by the ctrl thread after full UVC+UAC setup.
 * Releases the UVC frame data thread to begin get/put frame loop.
 */
static rtos_sema_t usbh_uvc_data_start_sema;

/**
 * @brief Status reported by the UVC set_param callback (HAL_OK / HAL_ERR_HW)
 */
/* Written by usbh_uvc_cb_setparam (class/core context), read by the ctrl thread
   after it takes usbh_uvc_setparam_sema. Under SMP those are different cores, so
   volatile keeps the reader from using a value cached before the callback ran. */
static volatile u8 usbh_uvc_setparam_status;

/**
 * @brief UVC stream context for set_param / start
 */
static usbh_uvc_s_ctx_t usbh_uvc_s_ctx;

/**
 * @brief Flag indicating device is ready for data transfer
 * Set after successful device enumeration and configuration
 * Reset when device is removed
 */
static __IO int usbh_uvc_uac_is_ready = 0;

/* Serializes the USB stack bring up / tear down against the class API callers.
   usbh_uac_deinit() frees the ISOC ring buffers and the per-direction format
   tables, usbh_uvc_deinit() frees the video frame buffers that
   usbh_uvc_get_frame()/_put_frame() hand out, and usbh_deinit() memset()s the
   whole host handle. Each class API only checks its own ready state on entry, so
   under SMP a worker on core 1 can be past that check while core 0 frees what it
   walks. The workers must be excluded by lock, not by core.
   Split four ways - video, playback, capture and the control/setup path - so the
   streams still run concurrently; a single lock would serialize video against
   audio for no reason. The teardown takes all four, always in the order video,
   play, record, ctrl. Each worker takes exactly one at a time, never two, so a
   fixed order here is enough to rule out a cycle.
   Created before the stack comes up and never deleted: the workers can not be
   joined by the example, so there is no point in time at which these mutexes are
   provably unreferenced. usbh_comp_stack_ready is the gate that keeps callers off
   the stack instead. */
static rtos_mutex_t usbh_comp_video_mutex = NULL;
static rtos_mutex_t usbh_comp_play_mutex = NULL;
static rtos_mutex_t usbh_comp_record_mutex = NULL;
static rtos_mutex_t usbh_comp_ctrl_mutex = NULL;

/* 1: the whole stack is up and the class APIs may be called. Cleared before any
   tear down, so a non-zero value also implies the four mutexes are valid. */
static volatile u8 usbh_comp_stack_ready = 0;

/* Set by the teardown BEFORE it takes the locks, so a worker inside a streaming
   loop gives up its round instead of re-taking a lock between frames and starving
   the teardown out. */
static volatile u8 usbh_comp_teardown = 0;

/* Set by the hotplug thread when the stack can not be rebuilt. Tells the workers
   to stop for good rather than spin against a stack that is gone. */
static volatile u8 usbh_comp_stack_fatal = 0;

static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = 1024U,
	.main_task_priority = USBH_UVC_UAC_MAIN_THREAD_PRIORITY,
	.tick_source = USBH_SOF_TICK,
	.class_num = 2U,   /* UVC + UAC */
#if defined(CONFIG_AMEBAGREEN2)
	/*FIFO total 1024 DWORD, resv 12 DWORD for DMA*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined(CONFIG_RLE1509)
	/*FIFO total 1024 DWORD, resv 48 DWORD */
	.rx_fifo_depth = 464,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAL2)
	/*FIFO total depth is 1024 DWORD, reserve 11 DWORD for DMA addr*/
	.rx_fifo_depth = 501,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAPRO3)
	/*FIFO total depth is 2232 DWORD, resv 8 DWORD for DMA addr */
	.rx_fifo_depth = 1712,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

static const usbh_uvc_config_t usbh_uvc_cfg_ctx = {
	.frame_buf_size = USBH_UVC_FRAME_BUF_SIZE,
#if USBH_UVC_USE_HW
	.hw_isr_pri = INT_PRI_MIDDLE,
#endif
};

static const usbh_uvc_cb_t usbh_uvc_cb = {
	.init = usbh_uvc_cb_init,
	.deinit = usbh_uvc_cb_deinit,
	.attached = usbh_uvc_cb_attached,
	.detached = usbh_uvc_cb_detached,
	.setup = usbh_uvc_cb_setup,
	.set_param = usbh_uvc_cb_setparam,
};

static const usbh_uac_cb_t usbh_uac_cb = {
	.init = usbh_uac_cb_init,
	.deinit = usbh_uac_cb_deinit,
	.attached = usbh_uac_cb_attached,
	.detached = usbh_uac_cb_detached,
	.setup = usbh_uac_cb_setup,
};

static const usbh_uac_config_t uac_class_cfg = {
	.isoc_in_frm_cnt = USBH_UAC_FRAME_CNT,
	.isoc_out_frm_cnt = USBH_UAC_FRAME_CNT,
};

static const usbh_user_cb_t usbh_usr_cb = {
	.process = usbh_uac_cb_process
};

/**
 * @brief Flag to request playback thread to exit completely
 * Only set when application is shutting down
 */
static __IO int usbh_uac_play_thread_exit = 0;

/**
 * @brief Flag to request record thread to exit completely
 */
static __IO int usbh_uac_record_thread_exit = 0;

/**
 * @brief Flag to request UVC/UAC ctrl thread to exit completely
 */
static __IO u8 usbh_uvc_ctrl_thread_exit = 0U;

/**
 * @brief Flag to request UVC frame data thread to exit completely
 */
static __IO int usbh_uvc_stream_thread_exit = 0;

/**
 * @brief Global record buffer for audio capture
 */
static u8 usbh_uac_record_buffer[USBH_UAC_RECORD_BUFFER_SIZE];

/**
 * @brief Count of transfer errors
 */
static int usbh_uac_err_count;

/**
 * @brief Total number of complete audio playback loops
 */
static u32 usbh_uac_play_count = 0;

/**
 * @brief Total number of complete audio record loops
 */
static u32 usbh_uac_record_count = 0;

/**
 * @brief Total number of UVC frames received
 */
static u32 usbh_uvc_frame_count = 0;

/**
 * @brief Handle for audio playback thread (resident)
 */
static rtos_task_t usbh_uac_play_task = NULL;

/**
 * @brief Handle for audio record thread (resident)
 */
static rtos_task_t usbh_uac_record_task = NULL;

/**
 * @brief Handle for UVC/UAC ctrl setup thread (resident)
 */
static rtos_task_t usbh_uvc_ctrl_task = NULL;

/**
 * @brief Handle for UVC frame data thread (resident)
 */
static rtos_task_t usbh_uvc_stream_task = NULL;

static rtos_task_t usbh_uvc_uac_hotplug_task = NULL;

/* Audio control direction: USBH_UAC_ISOC_OUT_DIR for playback, USBH_UAC_ISOC_IN_DIR for record. */
static u8 uac_ctrl_dir = USBH_UAC_ISOC_OUT_DIR;

static u8 cur_playback_volume = 50;

static u8 cur_record_volume = 50;

/* Exported variables --------------------------------------------------------*/

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbh_composite_uvc_uac_test_cmd_table[] = {
	{"uach", usbh_composite_cmd},
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Take one of the per-path stack locks if the class API may be used
  * @note   Gates on usbh_comp_stack_ready BEFORE touching the mutex: the handle is
  *         still NULL if the example never started, and the flag is already cleared
  *         if a teardown is pending. A hotplug right after this check is harmless,
  *         the mutex stays valid and the re-check under it catches the loss.
  * @param  lock: One of usbh_comp_video_mutex / _play_mutex / _record_mutex / _ctrl_mutex
  * @retval HAL_OK when the lock is held and the stack is usable, HAL_ERR_HW when
  *         the caller must give up without unlocking
  */
static int usbh_comp_lock(rtos_mutex_t lock)
{
	if ((usbh_comp_stack_ready == 0U) || (usbh_comp_teardown != 0U) || (lock == NULL)) {
		return HAL_ERR_HW;
	}

	rtos_mutex_take(lock, RTOS_MAX_TIMEOUT);

	if ((usbh_comp_stack_ready == 0U) || (usbh_comp_teardown != 0U)) {
		/* Lost the race against a teardown */
		rtos_mutex_give(lock);
		return HAL_ERR_HW;
	}

	return HAL_OK;
}

/**
  * @brief  Release a lock taken by usbh_comp_lock()
  * @param  lock: The same handle that was passed to usbh_comp_lock()
  * @retval None
  */
static void usbh_comp_unlock(rtos_mutex_t lock)
{
	rtos_mutex_give(lock);
}

/**
 * @brief Handle for hot-plug monitoring thread
 */
#if USBH_UVC_UAC_HOT_PLUG_TEST
#endif

/**
 * @brief USB host controller configuration
 * Defines hardware parameters and priorities.
 * class_num = 2 because the example registers two independent standard class
 * drivers (UVC + UAC) to the host core - there is no composite class layer.
 */

/**
 * @brief UVC global context (frame buffer sizing at init time)
 */

/**
 * @brief UVC (USB Video Class) callback configuration
 */

/**
 * @brief UAC (USB Audio Class) callback configuration
 */

/**
 * @brief General USB host event callbacks
 */

/**
  * @brief  UVC class init callback, invoked once when the UVC class driver is
  *         registered to the USB host stack.
  * @retval Status
  */
static int usbh_uvc_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UVC INIT\n");
	return HAL_OK;
}

/**
  * @brief  UVC class deinit callback.
  */
static void usbh_uvc_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UVC DEINIT\n");
}

/**
  * @brief  UVC attach callback, fired when a UVC-capable device is plugged in.
  */
static void usbh_uvc_cb_attached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UVC ATTACH\n");
	usbh_uvc_frame_count = 0;
}

/**
  * @brief  UVC detach callback, fired when the device is unplugged.
  */
static void usbh_uvc_cb_detached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UVC DETACH frames=%d\n", usbh_uvc_frame_count);
	usbh_uvc_uac_is_ready = 0;
	/* Halt ISOC; frame buffers remain (freed only by usbh_uvc_deinit). */
	usbh_uvc_stop(USBH_UVC_STREAM_INDEX);
}

/**
  * @brief  UVC setup callback, fired after device enumeration and class
  *         setup completes. Marks the device ready and wakes the stream thread.
  */
static void usbh_uvc_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UVC SETUP\n");
	usbh_uvc_uac_is_ready = 1;
	rtos_sema_give(usbh_uvc_stream_start_sema);
}

/**
  * @brief  UVC set_param callback, fired after Probe/Commit/SET_INTERFACE
  *         negotiation completes.
  * @param  status: HAL_OK on success, HAL_ERR_MEM or HAL_ERR_HW on failure.
  * @retval Status
  */
static void usbh_uvc_cb_setparam(int status)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UVC setparam status=%d\n", status);
	usbh_uvc_setparam_status = (u8)status;
	rtos_sema_give(usbh_uvc_setparam_sema);
}

/**
  * @brief  UAC class init callback.
  * @retval Status
  */
static int usbh_uac_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC INIT\n");
	return HAL_OK;
}

/**
  * @brief  UAC class deinit callback.
  */
static void usbh_uac_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC DEINIT\n");
}

/**
  * @brief  UAC attach callback.
  */
static void usbh_uac_cb_attached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC ATTACH\n");
	usbh_uac_play_count = 0;
	usbh_uac_record_count = 0;
}

/**
  * @brief  UAC detach callback.
  */
static void usbh_uac_cb_detached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC DETACH played=%d rec=%d\n",
			 usbh_uac_play_count, usbh_uac_record_count);

	usbh_uvc_uac_is_ready = 0;

	usbh_uac_stop_play();
	usbh_uac_stop_capture();

	/* Unblock data threads if they are still waiting for ctrl setup to complete */
	rtos_sema_give(usbh_uac_play_start_sema);
	rtos_sema_give(usbh_uac_record_start_sema);

#if USBH_UVC_UAC_HOT_PLUG_TEST
	rtos_sema_give(usbh_uvc_uac_detach_sema);
#endif
}

/**
  * @brief  UAC setup callback, fired after device enumeration.
  */
static void usbh_uac_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC SETUP\n");

	usbh_uvc_uac_is_ready = 1;

	/* Signal the UVC stream thread that UAC CLASS_REQUEST setup is done. */
	rtos_sema_give(usbh_uac_ready_sema);
}

/**
  * @brief  Generic USB host process callback.
  * @param  host: USB host handle (unused).
  * @param  msg:  Event identifier dispatched by the host core.
  * @retval Status
  */
static void usbh_uac_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);

	switch (msg) {
	case USBH_MSG_DISCONNECTED:
		usbh_uvc_uac_is_ready = 0;
		break;

	case USBH_MSG_CONNECTED:
		break;

	case USBH_MSG_PROBE_FAIL:
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Probe Fail\n");
		usbh_uvc_uac_is_ready = 0;
		break;

	default:
		break;
	}
}

/**
  * @brief  Composite ctrl setup thread (UVC + UAC).
  *
  *         Handles all ep0 control transfers for each attach cycle:
  *           1. UVC Probe/Commit/SET_INTERFACE (usbh_uvc_set_param, async, cb-waited)
  *           2. UVC ISOC data start            (usbh_uvc_start)
  *           3. UAC OUT SET_INTERFACE          (usbh_uac_set_alt_setting, blocking)
  *           4. UAC IN  SET_INTERFACE          (usbh_uac_set_alt_setting, blocking)
  *           5. UAC playback data start        (usbh_uac_start_play)
  *           6. UAC capture data start         (usbh_uac_start_capture)
  *         After all ctrl work, releases the three data threads via semaphores and
  *         waits idle until the device detaches or app exits.
  * @param  param: Unused.
  * @retval None
  */
static void usbh_uvc_uac_ctrl_thread(void *param)
{
	const usbh_uac_audio_fmt_t *fmt_out = NULL;
	const usbh_uac_audio_fmt_t *fmt_in = NULL;
	u8 is_streaming = 0U;
	u8 is_playing = 0U;
	u8 is_recording = 0U;
	u8 do_ctrl_setup = 0U;
	u8 uac_ready;
	u8 fmt_cnt = 0U;
	u8 i = 0U;

	UNUSED(param);

	while (usbh_uvc_ctrl_thread_exit == 0U) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Ctrl wait setup\n");
		do_ctrl_setup = 0U;

		if (rtos_sema_take(usbh_uvc_stream_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}
		/* cppcheck-suppress oppositeInnerCondition ; volatile: the hotplug thread sets this from another core while we block on the sema above */
		if (usbh_uvc_ctrl_thread_exit != 0U) {
			break;
		}

		if (usbh_uvc_uac_is_ready != 0U) {
			do_ctrl_setup = 1U;
		} else {
			RTK_LOGS(TAG, RTK_LOG_WARN, "UVC not ready\n");
		}

		if ((usbh_comp_teardown != 0U) || (usbh_comp_stack_fatal != 0U)) {
			do_ctrl_setup = 0U;
		}

		/* Wait for UAC CLASS_REQUEST setup to finish (usually already done). Done before the
		   ctrl lock: the wait touches no stack state and usbh_comp_lock() re-checks
		   stack_ready/teardown afterwards, so a teardown is not held up by it. */
		uac_ready = 1U;
		if ((do_ctrl_setup != 0U) && (rtos_sema_take(usbh_uac_ready_sema, USBH_COMPOSITE_CTRL_TIMEOUT_MS) != RTK_SUCCESS)) {
			uac_ready = 0U;
		}

		if ((do_ctrl_setup != 0U) && (usbh_comp_lock(usbh_comp_ctrl_mutex) != HAL_OK)) {
			/* Stack went away between the ready check and here */
			do_ctrl_setup = 0U;
		}

		if (do_ctrl_setup != 0U) {
			/* The whole negotiation runs under one ctrl lock hold, so a teardown waits for
			   the complete sequence: up to USBH_COMPOSITE_CTRL_TIMEOUT_MS for the UVC
			   set_param result plus up to 6 s (the UAC driver's sync timeout) for each UAC
			   SET_INTERFACE, but never for a data stream. */
			if (uac_ready == 0U) {
				if ((usbh_uac_support_playback() == 0) && (usbh_uac_support_record() == 0)) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "UVC-only device (no UAC attached)\n");
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "UAC setup timeout, continuing anyway\n");
				}
			}

			/* 1. UVC set_param: Probe/Commit/SET_INTERFACE (async, result via cb) */
			RTK_LOGS(TAG, RTK_LOG_INFO, "UVC set param (%dx%d@%dfps)\n",
					 USBH_UVC_STREAM_WIDTH, USBH_UVC_STREAM_HEIGHT, USBH_UVC_STREAM_FRAMERATE);
			usbh_uvc_s_ctx.width = USBH_UVC_STREAM_WIDTH;
			usbh_uvc_s_ctx.height = USBH_UVC_STREAM_HEIGHT;
			usbh_uvc_s_ctx.frame_rate = USBH_UVC_STREAM_FRAMERATE;
			usbh_uvc_s_ctx.fmt_type = USBH_UVC_FORMAT_H264;
			usbh_uvc_s_ctx.frame_buf_size = USBH_UVC_FRAME_BUF_SIZE;
			if (usbh_uvc_set_param(&usbh_uvc_s_ctx, USBH_UVC_STREAM_INDEX) != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "UVC setparam req fail\n");
			} else if (rtos_sema_take(usbh_uvc_setparam_sema, USBH_COMPOSITE_CTRL_TIMEOUT_MS) != RTK_SUCCESS) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "UVC setparam timeout\n");
			} else if (usbh_uvc_setparam_status != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "UVC setparam fail=%d\n", usbh_uvc_setparam_status);
			} else {
				/* 2. UVC start: begin ISOC video data transfer. is_streaming gates the
				 * release of the frame data thread, so it must only be set when the
				 * stream really started - usbh_uvc_get_frame() is not safe before it. */
				if (usbh_uvc_start(USBH_UVC_STREAM_INDEX) != HAL_OK) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "UVC start fail\n");
				} else {
					is_streaming = 1U;
					RTK_LOGS(TAG, RTK_LOG_INFO, "UVC streaming started\n");
				}
			}

			/* 3. UAC OUT SET_INTERFACE (blocking ctrl) */
			if (usbh_uac_support_playback() != 0) {
				fmt_out = usbh_uac_get_alt_setting(USBH_UAC_ISOC_OUT_DIR, &fmt_cnt);
				if ((fmt_out != NULL) && (fmt_cnt > 0U)) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Play fmt list:\n");
					for (i = 0U; i < fmt_cnt; i++) {
						RTK_LOGS(TAG, RTK_LOG_INFO, "[%d] CH=%d BW=%d FREQ=%d\n",
								 i, fmt_out[i].ch_cnt, fmt_out[i].bit_width, fmt_out[i].sampling_freq);
					}
					for (i = 0U; i < fmt_cnt; i++) {
						if ((fmt_out[i].ch_cnt == USBH_UAC_PLAY_CHANNELS) &&
							(fmt_out[i].sampling_freq == USBH_UAC_PLAY_SAMPLING_FREQ)) {
							break;
						}
					}
					if (i >= fmt_cnt) {
						i = 0U;
						RTK_LOGS(TAG, RTK_LOG_WARN, "Play: %dch/%dHz not found, using fmt[0]\n",
								 USBH_UAC_PLAY_CHANNELS, USBH_UAC_PLAY_SAMPLING_FREQ);
					}
					if (usbh_uac_set_alt_setting(USBH_UAC_ISOC_OUT_DIR,
												 fmt_out[i].ch_cnt,
												 fmt_out[i].bit_width,
												 fmt_out[i].sampling_freq) == HAL_OK) {
						/* 5. UAC play start */
						usbh_uac_start_play();
						is_playing = 1U;
						RTK_LOGS(TAG, RTK_LOG_INFO, "UAC play started\n");
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Play set alt fail\n");
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Play get fmt fail\n");
				}
			}

			/* 4. UAC IN SET_INTERFACE (blocking ctrl) */
			if (usbh_uac_support_record() != 0) {
				fmt_in = usbh_uac_get_alt_setting(USBH_UAC_ISOC_IN_DIR, &fmt_cnt);
				if ((fmt_in != NULL) && (fmt_cnt > 0U)) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Rec fmt list:\n");
					for (i = 0U; i < fmt_cnt; i++) {
						RTK_LOGS(TAG, RTK_LOG_INFO, "[%d] CH=%d BW=%d FREQ=%d\n",
								 i, fmt_in[i].ch_cnt, fmt_in[i].bit_width, fmt_in[i].sampling_freq);
					}
					for (i = 0U; i < fmt_cnt; i++) {
						if ((fmt_in[i].ch_cnt == USBH_UAC_REC_CHANNELS) &&
							(fmt_in[i].sampling_freq == USBH_UAC_REC_SAMPLING_FREQ)) {
							break;
						}
					}
					if (i >= fmt_cnt) {
						i = 0U;
						RTK_LOGS(TAG, RTK_LOG_WARN, "Rec: %dch/%dHz not found, using fmt[0]\n",
								 USBH_UAC_REC_CHANNELS, USBH_UAC_REC_SAMPLING_FREQ);
					}
					if (usbh_uac_set_alt_setting(USBH_UAC_ISOC_IN_DIR,
												 fmt_in[i].ch_cnt,
												 fmt_in[i].bit_width,
												 fmt_in[i].sampling_freq) == HAL_OK) {
						/* 6. UAC capture start */
						usbh_uac_start_capture();
						is_recording = 1U;
						RTK_LOGS(TAG, RTK_LOG_INFO, "UAC rec started\n");
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Rec set alt fail\n");
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Rec get fmt fail\n");
				}
			}

			/* Negotiation done: release the ctrl lock before the long detach wait
			   below, so a teardown is not blocked for the whole session. */
			usbh_comp_unlock(usbh_comp_ctrl_mutex);
		}

		/* Release UAC data threads unconditionally (they guard via is_ready). */
		rtos_sema_give(usbh_uac_play_start_sema);
		rtos_sema_give(usbh_uac_record_start_sema);

		/* Only release UVC data thread if streaming was actually started.
		 * usbh_uvc_get_frame() will crash if called before usbh_uvc_start(). */
		if (is_streaming != 0U) {
			rtos_sema_give(usbh_uvc_data_start_sema);
		}

		/* Wait for device to disappear (detach callbacks set is_ready=0) */
		while ((usbh_uvc_uac_is_ready != 0U) && (usbh_uvc_ctrl_thread_exit == 0U)) {
			rtos_time_delay_ms(USBH_COMPOSITE_DETACH_POLL_MS);
		}

		/* Guard stops for app-exit path (detach callbacks already stopped on detach).
		   Each stop takes its own path lock, and all are best effort: on a closed
		   gate the stack is already down and the classes stopped their streams
		   themselves in the detach callbacks. */
		if (is_streaming != 0U) {
			if (usbh_comp_lock(usbh_comp_video_mutex) == HAL_OK) {
				usbh_uvc_stop(USBH_UVC_STREAM_INDEX);
				usbh_comp_unlock(usbh_comp_video_mutex);
			}
			is_streaming = 0U;
		}
		if (is_playing != 0U) {
			if (usbh_comp_lock(usbh_comp_play_mutex) == HAL_OK) {
				usbh_uac_stop_play();
				usbh_comp_unlock(usbh_comp_play_mutex);
			}
			is_playing = 0U;
		}
		if (is_recording != 0U) {
			if (usbh_comp_lock(usbh_comp_record_mutex) == HAL_OK) {
				usbh_uac_stop_capture();
				usbh_comp_unlock(usbh_comp_record_mutex);
			}
			is_recording = 0U;
		}

		if (usbh_comp_stack_fatal != 0U) {
			break;
		}
		rtos_time_delay_ms(USBH_UAC_VERIFY_SETTLE_MS);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Ctrl thread exit\n");

	usbh_uvc_ctrl_task = NULL;
	rtos_task_delete(NULL);
}

/**
  * @brief  UVC frame data thread.
  *
  *         Resident thread that only pulls and releases video frames. Each
  *         attach cycle: blocks on usbh_uvc_data_start_sema until the ctrl
  *         thread has completed full UVC+UAC setup, then loops on
  *         usbh_uvc_get_frame / usbh_uvc_put_frame until the device detaches.
  * @param  param: Unused.
  * @retval None
  */
static void usbh_uvc_stream_thread(void *param)
{
	usbh_uvc_frame_t *frame = NULL;
	u32 frame_count = 0U;
	u32 debug_time = 0U;

	UNUSED(param);

	while (usbh_uvc_stream_thread_exit == 0U) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "UVC stream wait\n");

		if (rtos_sema_take(usbh_uvc_data_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}
		/* cppcheck-suppress oppositeInnerCondition ; volatile: the hotplug thread sets this from another core while we block on the sema above */
		if (usbh_uvc_stream_thread_exit != 0U) {
			break;
		}

		frame_count = 0U;
		debug_time = usb_os_get_timestamp_ms();

		while ((usbh_uvc_uac_is_ready != 0U) && (usbh_uvc_stream_thread_exit == 0U)) {
			/* get_frame, the dereference and put_frame must sit in ONE lock hold:
			   the returned pointer points into the stream's frame buffers, which
			   usbh_uvc_deinit() frees. Releasing between get and put would leave a
			   borrowed frame across a teardown, and put_frame would then write to
			   freed memory. get_frame is internally bounded (it returns NULL rather
			   than blocking forever), so the hold stays short. */
			if (usbh_comp_lock(usbh_comp_video_mutex) != HAL_OK) {
				break;
			}
			frame = usbh_uvc_get_frame(USBH_UVC_STREAM_INDEX);
			if (frame != NULL) {
				frame_count++;
				usbh_uvc_frame_count++;
				if ((usb_os_get_timestamp_ms() - debug_time) >= USBH_UAC_DEBUG_TRACE_STEP) {
					debug_time = usb_os_get_timestamp_ms();
					RTK_LOGS(TAG, RTK_LOG_INFO, "UVC frames: %d (size=%d, ts=%d)\n",
							 frame_count, frame->byteused, frame->timestamp);
				}
				usbh_uvc_put_frame(frame, USBH_UVC_STREAM_INDEX);
				usbh_comp_unlock(usbh_comp_video_mutex);
			} else {
				usbh_comp_unlock(usbh_comp_video_mutex);
				rtos_time_delay_ms(USBH_UVC_FRAME_POLL_DELAY_MS);
			}
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "UVC stream stop, total frames=%d\n", frame_count);

		if (usbh_comp_stack_fatal != 0U) {
			break;
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "UVC stream exit\n");

	usbh_uvc_stream_task = NULL;
	rtos_task_delete(NULL);
}

/**
  * @brief  Audio playback worker thread.
  * @param  param: Unused.
  * @retval None
  */
static void usbh_uac_play_thread(void *param)
{
	static u8 silence[USBH_UAC_SILENCE_BUF_SIZE] = {0U};
	u32 play_loop_count = 0U;
	u32 frame_size = 0U;
	u32 debug_time = 0U;

	UNUSED(param);

	while (usbh_uac_play_thread_exit == 0U) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Play wait start\n");

		/* Waits until the setup thread completes all ctrl setup and calls start_play() */
		if (rtos_sema_take(usbh_uac_play_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}
		/* cppcheck-suppress oppositeInnerCondition ; volatile: the hotplug thread sets this from another core while we block on the sema above */
		if (usbh_uac_play_thread_exit != 0U) {
			break;
		}
		if (usbh_comp_lock(usbh_comp_play_mutex) != HAL_OK) {
			continue;
		}
		if ((usbh_uvc_uac_is_ready == 0U) || (usbh_uac_support_playback() == 0)) {
			usbh_comp_unlock(usbh_comp_play_mutex);
			RTK_LOGS(TAG, RTK_LOG_WARN, "Play not ready or not supported\n");
			continue;
		}

		frame_size = usbh_uac_get_frame_size(USBH_UAC_ISOC_OUT_DIR);
		usbh_comp_unlock(usbh_comp_play_mutex);
		if ((frame_size == 0U) || (frame_size > (u32)sizeof(silence))) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Play frame %d out of range, clamp to %d\n",
					 frame_size, (u32)sizeof(silence));
			frame_size = (u32)sizeof(silence);
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "Play data start, frame=%d\n", frame_size);

		play_loop_count = 0U;
		debug_time = usb_os_get_timestamp_ms();
		while ((usbh_uvc_uac_is_ready != 0U) && (usbh_uac_play_thread_exit == 0U)) {
			play_loop_count++;
			usbh_uac_play_count++;

			/* Send silence (zeros) as demo playback data. Write exactly one ISOC
			 * frame per call so the OUT buffer stays full. usbh_uac_write returns
			 * the byte count actually written, not a status code, so a short write
			 * means the ring buffer stayed full for the whole timeout. Back off in
			 * that case: the timeout alone does not throttle the loop when the
			 * write fails immediately. */
			if (usbh_comp_lock(usbh_comp_play_mutex) != HAL_OK) {
				break;
			}
			if (usbh_uac_write(silence, frame_size, USBH_UAC_WRITE_TIMEOUT_MS) != frame_size) {
				usbh_comp_unlock(usbh_comp_play_mutex);
				usbh_uac_err_count++;
				rtos_time_delay_ms(USBH_UAC_PLAY_LOOP_DELAY_MS);
			} else {
				usbh_comp_unlock(usbh_comp_play_mutex);
			}

			if ((usb_os_get_timestamp_ms() - debug_time) >= USBH_UAC_DEBUG_TRACE_STEP) {
				debug_time = usb_os_get_timestamp_ms();
				RTK_LOGS(TAG, RTK_LOG_INFO, "Play loop=%d err=%d\n",
						 play_loop_count, usbh_uac_err_count);
			}
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Play data stop, loops=%d\n", play_loop_count);

		if (usbh_comp_stack_fatal != 0U) {
			break;
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Play exit\n");

	usbh_uac_play_task = NULL;
	rtos_task_delete(NULL);
}

/**
  * @brief  Audio record worker thread.
  *
  *         Resident thread that lives for the entire demo. Each iteration:
  *           1. Blocks on `usbh_uac_record_start_sema` until a device is set up.
  *           2. Queries the IN alt-settings and selects the first one advertised.
  *           3. (Optional) Creates an `AudioTrack` so captured PCM is rendered
  *              locally for monitoring when `CONFIG_SUPPORT_AUDIO_FOR_USB` is on.
  *           4. Pulls isochronous IN data into `usbh_uac_record_buffer` until the
  *              device disappears or the demo is shutting down.
  *           5. Tears down the IN pipeline / AudioTrack and returns to step 1.
  * @param  param: Unused.
  * @retval None
  */
static void usbh_uac_record_thread(void *param)
{
	u32 record_loop_count = 0U;
	u32 frame_size = 0U;
	u32 total_read = 0U;
	u32 read_len = 0U;
	u32 debug_time = 0U;
	u32 ret = 0U;

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
	AudioTrackConfig track_config = {0};
	const usbh_uac_audio_fmt_t *fmt_info = NULL;
	struct AudioTrack *audio_track = NULL;
	u32 format = 0U;
	s32 track_buf_size = 0;
	u32 g_track_rate = 0U;
	u32 g_track_channel = 0U;
	u32 g_track_format = 0U;
	u8 fmt_cnt = 0U;
	u8 have_fmt = 0U;
#endif

	UNUSED(param);

	while (usbh_uac_record_thread_exit == 0U) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Rec wait start\n");

		/* Waits until the setup thread completes all ctrl setup and calls start_capture() */
		if (rtos_sema_take(usbh_uac_record_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}
		/* cppcheck-suppress oppositeInnerCondition ; volatile: the hotplug thread sets this from another core while we block on the sema above */
		if (usbh_uac_record_thread_exit != 0U) {
			break;
		}
		if (usbh_comp_lock(usbh_comp_record_mutex) != HAL_OK) {
			continue;
		}
		if ((usbh_uvc_uac_is_ready == 0U) || (usbh_uac_support_record() == 0)) {
			usbh_comp_unlock(usbh_comp_record_mutex);
			RTK_LOGS(TAG, RTK_LOG_WARN, "Rec not ready or not supported\n");
			continue;
		}

		frame_size = usbh_uac_get_frame_size(USBH_UAC_ISOC_IN_DIR);

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
		/* Copy the format fields out while still holding the lock: fmt_info points
		   into the class format table, which usbh_uac_deinit() frees. Everything
		   below uses only the copies, so the AudioTrack setup can run unlocked. */
		fmt_info = usbh_uac_get_alt_setting(USBH_UAC_ISOC_IN_DIR, &fmt_cnt);
		have_fmt = ((fmt_info != NULL) && (fmt_cnt > 0U)) ? 1U : 0U;
		if (have_fmt != 0U) {
			g_track_rate = fmt_info[0].sampling_freq;
			g_track_channel = fmt_info[0].ch_cnt;
			g_track_format = fmt_info[0].bit_width;
		}
		fmt_info = NULL;
#endif
		/* Done with the class queries: release before the AudioTrack setup and the
		   capture loop, which re-takes the lock per read. */
		usbh_comp_unlock(usbh_comp_record_mutex);

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
		if (have_fmt != 0U) {

			RTK_LOGS(TAG, RTK_LOG_INFO, "Rec init ch=%d rate=%d bits=%d\n",
					 g_track_channel, g_track_rate, g_track_format);

			AudioService_Init();

			switch (g_track_format) {
			case 16:
				format = AUDIO_FORMAT_PCM_16_BIT;
				break;
			case 24:
				format = AUDIO_FORMAT_PCM_24_BIT;
				break;
			case 32:
				format = AUDIO_FORMAT_PCM_32_BIT;
				break;
			default:
				format = AUDIO_FORMAT_PCM_16_BIT;
				break;
			}

			/* Local monitoring output is optional. On any failure below, audio_track is
			 * left NULL and the read loop still runs: capture is already started in the
			 * driver, so the IN buffer must be drained or the ISOC pipe stalls. */
			audio_track = AudioTrack_Create();
			if (audio_track == NULL) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Rec track create fail, drain without output\n");
			} else {
				track_buf_size = AudioTrack_GetMinBufferBytes(audio_track, AUDIO_CATEGORY_MEDIA,
								 g_track_rate, format, g_track_channel) * 10;
				if (track_buf_size == 0) {
					track_buf_size = g_track_rate * g_track_format / 8 * g_track_channel / 1000 * 100;
					RTK_LOGS(TAG, RTK_LOG_INFO, "Rec track buf resize %d\n", track_buf_size);
				} else {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Rec track buf %d\n", track_buf_size);
				}

				track_config.category_type = AUDIO_CATEGORY_MEDIA;
				track_config.sample_rate = g_track_rate;
				track_config.format = format;
				track_config.channel_count = g_track_channel;
				track_config.buffer_bytes = track_buf_size;
				AudioTrack_Init(audio_track, &track_config, AUDIO_OUTPUT_FLAG_NONE);

				AudioTrack_SetVolume(audio_track, 1.0, 1.0);
				AudioTrack_SetStartThresholdBytes(audio_track, track_buf_size);

				if (AudioTrack_Start(audio_track) != AUDIO_OK) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Rec track start fail, drain without output\n");
					AudioTrack_Destroy(audio_track);
					audio_track = NULL;
				} else {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Rec track started\n");
				}
			}
		}
#endif

		RTK_LOGS(TAG, RTK_LOG_INFO, "Rec data start, frame=%d\n", frame_size);

		record_loop_count = 0U;
		total_read = 0U;
		debug_time = usb_os_get_timestamp_ms();

		while ((usbh_uvc_uac_is_ready != 0U) && (usbh_uac_record_thread_exit == 0U)) {
			read_len = USBH_UAC_RECORD_BUFFER_SIZE;
			if (usbh_comp_lock(usbh_comp_record_mutex) != HAL_OK) {
				break;
			}
			ret = usbh_uac_read(usbh_uac_record_buffer, read_len, USBH_UAC_READ_TIMEOUT_MS);
			usbh_comp_unlock(usbh_comp_record_mutex);

			if (ret > 0U) {
				record_loop_count++;
				usbh_uac_record_count++;
				total_read += ret;

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
				if (audio_track != NULL) {
					AudioTrack_Write(audio_track, (u8 *)usbh_uac_record_buffer, ret, true);
				}
#endif
			}

			if ((usb_os_get_timestamp_ms() - debug_time) >= USBH_UAC_DEBUG_TRACE_STEP) {
				debug_time = usb_os_get_timestamp_ms();
				if (total_read > 0U) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Rec loop=%d bytes=%d\n",
							 record_loop_count, total_read);
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "Rec loop=%d no data\n", record_loop_count);
				}
			}
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Rec data stop, loops=%d bytes=%d\n",
				 record_loop_count, total_read);

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
		if (audio_track != NULL) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Rec track stop\n");
			AudioTrack_Pause(audio_track);
			AudioTrack_Flush(audio_track);
			AudioTrack_Stop(audio_track);
			AudioTrack_Destroy(audio_track);
			audio_track = NULL;
		}
#endif

		if (usbh_comp_stack_fatal != 0U) {
			break;
		}
	}

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
	if (audio_track != NULL) {
		AudioTrack_Pause(audio_track);
		AudioTrack_Flush(audio_track);
		AudioTrack_Stop(audio_track);
		AudioTrack_Destroy(audio_track);
		audio_track = NULL;
	}
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "Rec exit\n");

	usbh_uac_record_task = NULL;
	rtos_task_delete(NULL);
}

/**
  * @brief  Bring up the USB host stack: core -> each class -> TRX.
  * @note   Reused by both the initial start-up and the hotplug re-init path.
  *         usbh_start() is issued only after every class driver is registered,
  *         so the Phase-1 match sees both drivers and enumeration can proceed.
  * @retval HAL_OK on success, other HAL_Status code on failure (all partial resources rolled back)
  */
static int usbh_composite_init_stack(void)
{
	int ret;

	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH init fail: %d\n", ret);
		return ret;
	}

	ret = usbh_uvc_init(&usbh_uvc_cb, &usbh_uvc_cfg_ctx);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UVC init failed: %d\n", ret);
		goto exit_usbh_init;
	}

	ret = usbh_uac_init(&usbh_uac_cb, &uac_class_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC init fail: %d\n", ret);
		goto exit_uvc_init;
	}

	usbh_start();

	/* Open the gate as the very last step, so no worker runs against a half built
	   stack. This helper is reused by the hotplug thread, so the re-attach path
	   re-opens it here too; teardown is cleared alongside it because a worker that
	   saw neither flag would slip through. */
	usbh_comp_stack_ready = 1U;
	usbh_comp_teardown = 0U;

	return HAL_OK;

exit_uvc_init:
	usbh_uvc_deinit();
exit_usbh_init:
	usbh_deinit();
	return ret;
}

#if USBH_UVC_UAC_HOT_PLUG_TEST
/**
  * @brief  Tear down the USB host stack, in the reverse order of
  *         usbh_composite_init_stack(): TRX -> classes -> core.
  * @note   No return value: a teardown failure has no recoverable path, so the
  *         only sequence of release calls in this example lives here.
  * @retval None
  */
static void usbh_composite_deinit_stack(void)
{
	/* Announce the teardown BEFORE contending for the locks, so a worker inside a
	   streaming loop gives up its round instead of re-taking a lock between frames
	   and starving this caller out. */
	usbh_comp_teardown = 1U;

	/* All four locks, always in this order (video, play, record, ctrl). */
	rtos_mutex_take(usbh_comp_video_mutex, RTOS_MAX_TIMEOUT);
	rtos_mutex_take(usbh_comp_play_mutex, RTOS_MAX_TIMEOUT);
	rtos_mutex_take(usbh_comp_record_mutex, RTOS_MAX_TIMEOUT);
	rtos_mutex_take(usbh_comp_ctrl_mutex, RTOS_MAX_TIMEOUT);
	/* Close the gate under the locks: no class API call is in flight here, and any
	   later one gives up on the cleared flag. */
	usbh_comp_stack_ready = 0U;

	usbh_stop();
	usbh_uac_deinit();
	usbh_uvc_deinit();
	usbh_deinit();

	rtos_mutex_give(usbh_comp_ctrl_mutex);
	rtos_mutex_give(usbh_comp_record_mutex);
	rtos_mutex_give(usbh_comp_play_mutex);
	rtos_mutex_give(usbh_comp_video_mutex);
}

/**
  * @brief  Hot-plug worker thread for memory-leak / re-enumeration soak testing.
  *
  *         Sleeps on usbh_uvc_uac_detach_sema. On each detach event it tears
  *         down both class drivers and the host core, drains any pending start
  *         semaphores so the workers do not re-enter with stale state, then
  *         re-initializes the host stack so a freshly plugged device can be
  *         re-enumerated.
  * @param  param: Unused.
  * @retval None
  */
static void usbh_uvc_uac_hotplug_thread(void *param)
{
	u32 hotplug_count = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(usbh_uvc_uac_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			hotplug_count++;
			RTK_LOGS(TAG, RTK_LOG_INFO, "\n========== Hotplug #%d ==========\n", hotplug_count);

			rtos_time_delay_ms(200);

			/* Cleanup USB resources */
			usbh_composite_deinit_stack();

			rtos_time_delay_ms(100);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			/* Clear stale semaphores */
			while (rtos_sema_take(usbh_uac_play_start_sema, 0) == RTK_SUCCESS) {
			}
			while (rtos_sema_take(usbh_uac_record_start_sema, 0) == RTK_SUCCESS) {
			}
			while (rtos_sema_take(usbh_uvc_stream_start_sema, 0) == RTK_SUCCESS) {
			}
			while (rtos_sema_take(usbh_uac_ready_sema, 0) == RTK_SUCCESS) {
			}
			while (rtos_sema_take(usbh_uvc_data_start_sema, 0) == RTK_SUCCESS) {
			}
			while (rtos_sema_take(usbh_uvc_setparam_sema, 0) == RTK_SUCCESS) {
			}

			/* Reinitialize USB stack (usbh_composite_init_stack() re-arms USB TRX) */
			if (usbh_composite_init_stack() != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail\n");
				break;
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "Reinit done\n");
		}
	}

	/* A re-init failed, so the stack is gone for good. Keep every worker out and
	 * wake the ones parked on a start sema so they observe the flag and leave their
	 * loops. The semas are left alive on purpose: a worker may still be about to
	 * take one, and freeing them here would pull them out from under it. */
	usbh_comp_stack_fatal = 1U;
	usbh_comp_stack_ready = 0U;
	usbh_comp_teardown = 1U;
	rtos_sema_give(usbh_uac_play_start_sema);
	rtos_sema_give(usbh_uac_record_start_sema);
	rtos_sema_give(usbh_uvc_data_start_sema);
	rtos_sema_give(usbh_uvc_stream_start_sema);

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread exit\n");
	usbh_uvc_uac_hotplug_task = NULL;
	rtos_task_delete(NULL);
}
#endif

/**
  * @brief  Compute the expected raw dB value the driver would have written for a
  *         given percent, given the channel's own min/max range. Mirrors the
  *         logic in usbh_uac_volume_to_db() so each verified channel is compared
  *         against its own quantisation range.
  * @param  vol_min: Minimum raw dB value reported by the device for this channel.
  * @param  vol_max: Maximum raw dB value reported by the device for this channel.
  * @param  percent: Volume percentage (0-100).
  * @param  expect:  Output pointer for the computed dB value.
  * @retval void
  */
static void usbh_composite_compute_expected_db(s16 vol_min, s16 vol_max, u8 percent, s16 *expect)
{
	s32 range;
	s32 raw;

	if (percent == 0) {
		*expect = vol_min;
	} else if (percent >= 100) {
		*expect = vol_max;
	} else {
		/* Must match usbh_uac_volume_to_db() exactly (same integer
		   truncation and s64 intermediate) so verification never flags an
		   expected value as a mismatch. */
		range = (s32)vol_max - (s32)vol_min;
		raw = (s32)vol_min + (s32)((s64)percent * range / 100);
		if (raw < vol_min) {
			raw = vol_min;
		} else if (raw > vol_max) {
			raw = vol_max;
		}
		*expect = (s16)raw;
	}
}

/**
  * @brief  Verify the most recent set_mute() by walking every channel
  *         (master + per-channel) on the active Feature Unit whose support
  *         bitmap has USB_UAC1_CONTROL_MUTE set, querying each, and reporting
  *         OK / FAIL per channel plus a summary line. A mismatch is always an
  *         ERROR - mute has no quantisation, so the device must report exactly
  *         what was written.
  * @param  dir:           Direction the set_mute() was issued for.
  * @param  expected_mute: Mute byte the caller wrote (0 or 1).
  * @retval void
  */
static void usbh_composite_verify_mute(u8 dir, u8 expected_mute)
{
	const usbh_uac_fu_info_t *info;
	u8 verified = 0;
	u8 ok = 0;
	u8 fail = 0;
	u8 ch;

	/* Shell context: the reads are excluded from a teardown like the set is, and the
	   lock is held across the settle delay so the stack can not go away in between. */
	if (usbh_comp_lock(usbh_comp_ctrl_mutex) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stack not ready\n");
		return;
	}

	rtos_time_delay_ms(USBH_UAC_VERIFY_SETTLE_MS);

	info = usbh_uac_get_volume_ctrl_info(dir);
	if (info == NULL) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Verify mute(%d) no FU\n", dir);
		usbh_comp_unlock(usbh_comp_ctrl_mutex);
		return;
	}

	for (ch = 0; ch <= info->num_channels; ch++) {
		u8 read_mute = 0;
		int ret;

		if (!(info->bma_controls[ch] & USB_UAC1_CONTROL_MUTE)) {
			continue;
		}
		verified++;

		ret = usbh_uac_get_mute(dir, ch, &read_mute);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Verify mute(%d) ch=%d read err=%d\n", dir, ch, ret);
			fail++;
			continue;
		}

		if (read_mute == expected_mute) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Verify mute(%d) ch=%d ok val=%d\n", dir, ch, read_mute);
			ok++;
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Verify mute(%d) ch=%d FAIL expect=%d got=%d\n",
					 dir, ch, expected_mute, read_mute);
			fail++;
		}
	}

	usbh_comp_unlock(usbh_comp_ctrl_mutex);

	if (verified == 0) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Verify mute(%d) no ch supports MUTE\n", dir);
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Verify mute(%d) summary: %d ok, %d fail (total %d)\n",
				 dir, ok, fail, verified);
	}
}

/**
  * @brief  Verify the most recent set_volume() by walking every channel
  *         (master + per-channel) on the active Feature Unit whose support
  *         bitmap has USB_UAC1_CONTROL_VOLUME set, querying each, and reporting
  *         OK / WARN per channel plus a summary line. Each channel is compared
  *         against its own min/max range because the driver re-computes the dB
  *         target per channel. A mismatch is WARN rather than ERROR since
  *         devices commonly quantise to a coarser step than requested.
  * @param  dir:              Direction the set_volume() was issued for.
  * @param  expected_percent: Volume percentage the caller wrote.
  * @retval void
  */
static void usbh_composite_verify_volume(u8 dir, u8 expected_percent)
{
	const usbh_uac_fu_info_t *info;
	u8 verified = 0;
	u8 ok = 0;
	u8 warn = 0;
	u8 err = 0;
	u8 ch;

	/* Same locking as usbh_composite_verify_mute() */
	if (usbh_comp_lock(usbh_comp_ctrl_mutex) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stack not ready\n");
		return;
	}

	rtos_time_delay_ms(USBH_UAC_VERIFY_SETTLE_MS);

	info = usbh_uac_get_volume_ctrl_info(dir);
	if (info == NULL) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Verify vol(%d) no FU\n", dir);
		usbh_comp_unlock(usbh_comp_ctrl_mutex);
		return;
	}

	for (ch = 0; ch <= info->num_channels; ch++) {
		s16 read_vol = 0;
		s16 vol_min = 0;
		s16 vol_max = 0;
		s16 expect_db = 0;
		int ret;

		if (!(info->bma_controls[ch] & USB_UAC1_CONTROL_VOLUME)) {
			continue;
		}
		verified++;

		ret = usbh_uac_get_volume(dir, ch, &read_vol, &vol_min, &vol_max);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Verify vol(%d) ch=%d read err=%d\n", dir, ch, ret);
			err++;
			continue;
		}

		usbh_composite_compute_expected_db(vol_min, vol_max, expected_percent, &expect_db);

		if (read_vol == expect_db) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Verify vol(%d) ch=%d ok %d%% db=0x%04x\n",
					 dir, ch, expected_percent, (u16)read_vol);
			ok++;
		} else {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Verify vol(%d) ch=%d %d%% expect=0x%04x got=0x%04x [0x%04x..0x%04x]\n",
					 dir, ch, expected_percent, (u16)expect_db, (u16)read_vol, (u16)vol_min, (u16)vol_max);
			warn++;
		}
	}

	usbh_comp_unlock(usbh_comp_ctrl_mutex);

	if (verified == 0) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Verify vol(%d) no ch supports VOLUME\n", dir);
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Verify vol(%d) summary: %d ok, %d warn, %d err (total %d)\n",
				 dir, ok, warn, err, verified);
	}
}

/**
  * @brief  Increase the volume by USBH_UAC_VOLUME_STEP percent, clamped at 100%.
  * @param  vol: Pointer to the current volume value; updated in place.
  * @retval HAL_OK on success, HAL_ERR_PARA if vol is NULL.
  */
static u32 usbh_composite_volup(u8 *vol)
{
	int ret;
	u8 cur_vol;

	if (vol == NULL) {
		return HAL_ERR_PARA;
	}

	cur_vol = *vol;

	cur_vol += USBH_UAC_VOLUME_STEP;
	if (cur_vol > 100) {
		cur_vol = 100;
	}
	/* Shell context, i.e. a thread of its own: it has to be excluded from a
	   concurrent teardown exactly like the workers are. */
	if (usbh_comp_lock(usbh_comp_ctrl_mutex) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stack not ready\n");
		return HAL_ERR_HW;
	}
	ret = usbh_uac_set_volume(cur_vol, uac_ctrl_dir);
	usbh_comp_unlock(usbh_comp_ctrl_mutex);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Set vol fail: %d\n", ret);
		return HAL_ERR_HW;
	}
	*vol = cur_vol;

	return HAL_OK;
}

/**
  * @brief  Decrease the volume by USBH_UAC_VOLUME_STEP percent, clamped at 0%.
  * @param  vol: Pointer to the current volume value; updated in place.
  * @retval HAL_OK on success, HAL_ERR_PARA if vol is NULL.
  */
static u32 usbh_composite_voldown(u8 *vol)
{
	int ret;
	u8 cur_vol;

	if (vol == NULL) {
		return HAL_ERR_PARA;
	}

	cur_vol = *vol;

	if (cur_vol < USBH_UAC_VOLUME_STEP) {
		cur_vol = 0;
	} else {
		cur_vol -= USBH_UAC_VOLUME_STEP;
	}

	/* Shell context, i.e. a thread of its own: it has to be excluded from a
	   concurrent teardown exactly like the workers are. */
	if (usbh_comp_lock(usbh_comp_ctrl_mutex) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stack not ready\n");
		return HAL_ERR_HW;
	}
	ret = usbh_uac_set_volume(cur_vol, uac_ctrl_dir);
	usbh_comp_unlock(usbh_comp_ctrl_mutex);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Set vol fail: %d\n", ret);
		return HAL_ERR_HW;
	}
	*vol = cur_vol;

	return HAL_OK;
}

static u32 usbh_composite_cmd(u16 argc, u8 *argv[])
{
	const char *cmd;
	u8 *cur_vol;
	u32 vol_ret;
	int ret;
	u8 mute = 0;

	if (argc == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid argument\n");
		return HAL_ERR_PARA;
	}

	cmd = (const char *)argv[0];

	if (_stricmp(cmd, "init") == 0) {
		if ((argv[1] != NULL) && (_stricmp((const char *)argv[1], "record") == 0)) {
			uac_ctrl_dir = USBH_UAC_ISOC_IN_DIR;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Record Control\n");
		} else {
			uac_ctrl_dir = USBH_UAC_ISOC_OUT_DIR;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Playback Control\n");
		}
	} else if (_stricmp(cmd, "mute") == 0) {
		mute = 1;
		if (argv[1]) {
			mute = (u8)_strtoul((const char *)(argv[1]), (char **)NULL, 10);
		}

		/* See the volume commands for why the shell path needs the lock. */
		if (usbh_comp_lock(usbh_comp_ctrl_mutex) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Stack not ready\n");
			return HAL_ERR_HW;
		}
		ret = usbh_uac_set_mute(mute, uac_ctrl_dir);
		usbh_comp_unlock(usbh_comp_ctrl_mutex);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set mute fail: %d\n", ret);
			return HAL_ERR_HW;
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "%s\n", ((mute) ? ("Mute") : ("UnMute")));

		usbh_composite_verify_mute(uac_ctrl_dir, mute);
	} else if (_stricmp(cmd, "vol") == 0) {
		cur_vol = (uac_ctrl_dir == USBH_UAC_ISOC_IN_DIR) ? &cur_record_volume : &cur_playback_volume;

		if ((argv[1] != NULL) && (_stricmp((const char *)argv[1], "down") == 0)) {
			vol_ret = usbh_composite_voldown(cur_vol);
		} else {
			vol_ret = usbh_composite_volup(cur_vol);
		}
		if (vol_ret != HAL_OK) {
			return vol_ret;
		}

		usbh_composite_verify_volume(uac_ctrl_dir, *cur_vol);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Input cmd is err\n");
		return HAL_ERR_PARA;
	}

	return HAL_OK;
}

/**
  * @brief  Demo bootstrap thread.
  *
  *         Spawns the resident UVC stream, UAC playback and UAC record workers,
  *         brings up the USB host core and registers the two standard class
  *         drivers (UVC + UAC) - the composite behaviour comes purely from
  *         registering both, there is no composite class layer. On any setup
  *         failure it unwinds via labelled goto cleanup steps.
  * @param  param: Unused.
  * @retval None
  */
static void example_usbh_uac_uvc_thread(void *param)
{
	int status;

	UNUSED(param);

	/* Create synchronization primitives. On failure only the handles already
	   created are released; the rest are still NULL and skipped by
	   rtos_sema_delete(). */
	/* Created before the stack comes up and never deleted, see the declarations */
	if (usbh_comp_video_mutex == NULL) {
		status = rtos_mutex_create(&usbh_comp_video_mutex);
		if (status != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create video mutex fail\n");
			goto free_sema_exit;
		}
	}
	if (usbh_comp_play_mutex == NULL) {
		status = rtos_mutex_create(&usbh_comp_play_mutex);
		if (status != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create play mutex fail\n");
			goto free_sema_exit;
		}
	}
	if (usbh_comp_record_mutex == NULL) {
		status = rtos_mutex_create(&usbh_comp_record_mutex);
		if (status != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create record mutex fail\n");
			goto free_sema_exit;
		}
	}
	if (usbh_comp_ctrl_mutex == NULL) {
		status = rtos_mutex_create(&usbh_comp_ctrl_mutex);
		if (status != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create ctrl mutex fail\n");
			goto free_sema_exit;
		}
	}

	status = rtos_sema_create(&usbh_uvc_uac_detach_sema, 0U, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto free_sema_exit;
	}

	status = rtos_sema_create(&usbh_uac_play_start_sema, 0U, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto free_sema_exit;
	}

	status = rtos_sema_create(&usbh_uac_record_start_sema, 0U, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto free_sema_exit;
	}

	status = rtos_sema_create(&usbh_uvc_stream_start_sema, 0U, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto free_sema_exit;
	}

	status = rtos_sema_create(&usbh_uvc_setparam_sema, 0U, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto free_sema_exit;
	}

	status = rtos_sema_create(&usbh_uac_ready_sema, 0U, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto free_sema_exit;
	}

	status = rtos_sema_create(&usbh_uvc_data_start_sema, 0U, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto free_sema_exit;
	}

	/* Create UVC/UAC ctrl setup thread */
	status = rtos_task_create(&usbh_uvc_ctrl_task, "usbh_uvc_uac_ctrl", usbh_uvc_uac_ctrl_thread,
							  NULL, USBH_UVC_CTRL_THREAD_STACK_SIZE, USBH_UVC_CTRL_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create ctrl thread fail\n");
		goto free_sema_exit;
	}

	/* Create UVC frame data thread */
	status = rtos_task_create(&usbh_uvc_stream_task, "usbh_uvc_stream", usbh_uvc_stream_thread,
							  NULL, USBH_UVC_STREAM_THREAD_STACK_SIZE, USBH_UVC_STREAM_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create UVC stream thread fail\n");
		goto delete_ctrl_task_exit;
	}

	/* Create resident playback thread */
	status = rtos_task_create(&usbh_uac_play_task, "usbh_composite_uac_play_thread", usbh_uac_play_thread,
							  NULL, 1024, USBH_UAC_PLAY_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create play thread fail\n");
		goto delete_stream_task_exit;
	}

	/* Create resident record thread */
	/* 2048 words, not 1536: the static analyzer reports the record thread already
	   overflows a 1536-word stack by ~330 bytes before this change, and the added
	   lock bookkeeping makes it marginally worse. Raised to clear both. */
	status = rtos_task_create(&usbh_uac_record_task, "usbh_composite_uac_record_thread", usbh_uac_record_thread,
							  NULL, 2048, USBH_UAC_RECORD_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create record thread fail\n");
		goto delete_play_task_exit;
	}

	/* Initialize USB host stack */
	status = usbh_composite_init_stack();
	if (status != HAL_OK) {
		goto delete_record_task_exit;
	}

	/* Create hot-plug monitor thread */
#if USBH_UVC_UAC_HOT_PLUG_TEST
	status = rtos_task_create(&usbh_uvc_uac_hotplug_task, "usbh_composite_hotplug_thread", usbh_uvc_uac_hotplug_thread,
							  NULL, 768, USBH_UVC_UAC_HOTPLUG_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
		goto deinit_stack_exit;
	}
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* The USB OTG ISR is delivered on CPU0: the GIC pins every SPI to core 0
	   (ITARGETSR is written to 0x01010101 in arm_gic.c gic_dist_init). Pinning the
	   hotplug/deinit thread to CPU0 puts it on the same core as the ISR, so the
	   local interrupt disables inside the teardown are meaningful again. The four
	   workers stay unaffined: a pending teardown is excluded from them by the
	   per-path mutexes, not by core. */
	rtos_task_set_affinity(usbh_uvc_uac_hotplug_task, 0);
#endif
#endif

	goto example_exit;

#if USBH_UVC_UAC_HOT_PLUG_TEST
deinit_stack_exit:
	usbh_composite_deinit_stack();
#endif

delete_record_task_exit:
	usbh_uac_play_thread_exit = 1;
	usbh_uac_record_thread_exit = 1;
	usbh_uvc_stream_thread_exit = 1;
	usbh_uvc_ctrl_thread_exit = 1U;
	/* Wake all blocked threads so they can observe their exit flags. */
	rtos_sema_give(usbh_uac_play_start_sema);
	rtos_sema_give(usbh_uac_record_start_sema);
	rtos_sema_give(usbh_uvc_stream_start_sema);
	rtos_sema_give(usbh_uvc_data_start_sema);
	rtos_sema_give(usbh_uac_ready_sema);
	rtos_time_delay_ms(500);
	if (usbh_uac_record_task != NULL) {
		rtos_task_delete(usbh_uac_record_task);
		usbh_uac_record_task = NULL;
	}

delete_play_task_exit:
	if (usbh_uac_play_task != NULL) {
		rtos_task_delete(usbh_uac_play_task);
		usbh_uac_play_task = NULL;
	}

delete_stream_task_exit:
	if (usbh_uvc_stream_task != NULL) {
		rtos_task_delete(usbh_uvc_stream_task);
		usbh_uvc_stream_task = NULL;
	}

delete_ctrl_task_exit:
	if (usbh_uvc_ctrl_task != NULL) {
		rtos_task_delete(usbh_uvc_ctrl_task);
		usbh_uvc_ctrl_task = NULL;
	}

free_sema_exit:
	rtos_sema_delete(usbh_uvc_uac_detach_sema);
	rtos_sema_delete(usbh_uac_play_start_sema);
	rtos_sema_delete(usbh_uac_record_start_sema);
	rtos_sema_delete(usbh_uvc_stream_start_sema);
	rtos_sema_delete(usbh_uvc_setparam_sema);
	rtos_sema_delete(usbh_uac_ready_sema);
	rtos_sema_delete(usbh_uvc_data_start_sema);
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH UVC+UAC comp demo aborted\n");

example_exit:
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Entry point for the USB host composite UVC + UAC demo.
  * @retval None
  */
void example_usbh_composite_uvc_uac(void)
{
	int status;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH UVC+UAC comp demo start\n");

	status = rtos_task_create(&task, "usbh_composite_main_thread", example_usbh_uac_uvc_thread, NULL, 1024U * 2, 2U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBH UVC+UAC comp thread fail\n");
	}
}

/*
uach init record/playback
uach mute 1/0     # set mute, then sync GET_CUR(MUTE) to verify the device accepted it
uach vol down/up  # adjust volume, then sync GET_CUR(VOLUME) to verify
*/
