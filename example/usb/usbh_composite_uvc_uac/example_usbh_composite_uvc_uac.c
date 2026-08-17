/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

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

/* UVC stream configuration */
#define USBH_UVC_STREAM_WIDTH                 2560
#define USBH_UVC_STREAM_HEIGHT                1440
#define USBH_UVC_STREAM_FRAMERATE             30
#define USBH_UVC_STREAM_INDEX                 0

/* Frame buffer size in bytes.
 * H264 @ 2K 30 fps: I-frames can peak at 200-300 KB;
 * Reduce if heap is tight; increase if oversize errors occur. */
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

/* Thread priority definitions (higher number = higher priority) */
#define USBH_UAC_PLAY_THREAD_PRIORITY         4       /* Audio playback thread */
#define USBH_UAC_RECORD_THREAD_PRIORITY       4       /* Audio record thread */
#define USBH_UVC_STREAM_THREAD_PRIORITY       3       /* UVC stream thread */
#define USBH_UVC_UAC_MAIN_THREAD_PRIORITY     5       /* USB host main thread */
#define USBH_UVC_UAC_HOTPLUG_THREAD_PRIORITY  6       /* Hot-plug detection thread */

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void example_usbh_uac_uvc_thread(void *param);
static void usbh_uvc_stream_thread(void *param);
static void usbh_uac_play_thread(void *param);
static void usbh_uac_record_thread(void *param);
#if USBH_UVC_UAC_HOT_PLUG_TEST
static void usbh_uvc_uac_hotplug_thread(void *param);
#endif
static int usbh_uvc_cb_init(void);
static int usbh_uvc_cb_deinit(void);
static int usbh_uvc_cb_attach(void);
static int usbh_uvc_cb_detach(void);
static int usbh_uvc_cb_setup(void);
static int usbh_uvc_cb_setparam(int status);
static int usbh_uac_cb_init(void);
static int usbh_uac_cb_deinit(void);
static int usbh_uac_cb_attach(void);
static int usbh_uac_cb_detach(void);
static int usbh_uac_cb_setup(void);
static int usbh_uac_cb_process(usb_host_t *host, u8 msg);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "COMP_UVC_UAC";

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
 * The UVC stream thread waits on this before issuing UAC set_alt_setting so
 * that it does not race with the UAC class driver's own setup transfers.
 */
static rtos_sema_t usbh_uac_ready_sema;

/**
 * @brief Status reported by the UVC set_param callback (HAL_OK / HAL_ERR_HW)
 */
static u8 usbh_uvc_setparam_status;

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
 * @brief Flag to request UVC stream thread to exit completely
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
 * @brief Handle for UVC stream thread (resident)
 */
static rtos_task_t usbh_uvc_stream_task = NULL;

/**
 * @brief Handle for hot-plug monitoring thread
 */
#if USBH_UVC_UAC_HOT_PLUG_TEST
static rtos_task_t usbh_uvc_uac_hotplug_task = NULL;
#endif

/* Audio control direction: USBH_UAC_ISOC_OUT_DIR for playback, USBH_UAC_ISOC_IN_DIR for record. */
static u8 uac_ctrl_dir = USBH_UAC_ISOC_OUT_DIR;
static u8 cur_playback_volume = 50;
static u8 cur_record_volume = 50;

/**
 * @brief USB host controller configuration
 * Defines hardware parameters and priorities.
 * class_num = 2 because the example registers two independent standard class
 * drivers (UVC + UAC) to the host core - there is no composite class layer.
 */
static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = 1024U,
	.main_task_priority = USBH_UVC_UAC_MAIN_THREAD_PRIORITY,
	.tick_source = USBH_SOF_TICK,
	.class_num = 2U,   /* UVC + UAC */
#if defined (CONFIG_AMEBAGREEN2)
	/*FIFO total depth is 1024, reserve 12 for DMA addr*/
	.rx_fifo_depth = 500,
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

/**
 * @brief UVC global context (frame buffer sizing at init time)
 */
static const usbh_uvc_ctx_t usbh_uvc_cfg_ctx = {
	.frame_buf_size = USBH_UVC_FRAME_BUF_SIZE,
#if USBH_UVC_USE_HW
	.hw_isr_pri = INT_PRI_MIDDLE,
#endif
};

/**
 * @brief UVC (USB Video Class) callback configuration
 */
static const usbh_uvc_cb_t usbh_uvc_cb = {
	.init = usbh_uvc_cb_init,
	.deinit = usbh_uvc_cb_deinit,
	.attach = usbh_uvc_cb_attach,
	.detach = usbh_uvc_cb_detach,
	.setup = usbh_uvc_cb_setup,
	.set_param = usbh_uvc_cb_setparam,
};

/**
 * @brief UAC (USB Audio Class) callback configuration
 */
static const usbh_uac_cb_t usbh_uac_cb = {
	.init = usbh_uac_cb_init,
	.deinit = usbh_uac_cb_deinit,
	.attach = usbh_uac_cb_attach,
	.detach = usbh_uac_cb_detach,
	.setup = usbh_uac_cb_setup,

	.isoc_in_frm_cnt = USBH_UAC_FRAME_CNT,
	.isoc_out_frm_cnt = USBH_UAC_FRAME_CNT,
};

/**
 * @brief General USB host event callbacks
 */
static const usbh_user_cb_t usbh_usr_cb = {
	.process = usbh_uac_cb_process
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  UVC class init callback, invoked once when the UVC class driver is
  *         registered to the USB host stack.
  * @retval Status
  */
static int usbh_uvc_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UVC Init\n");
	return HAL_OK;
}

/**
  * @brief  UVC class deinit callback.
  * @retval Status
  */
static int usbh_uvc_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UVC Deinit\n");
	return HAL_OK;
}

/**
  * @brief  UVC attach callback, fired when a UVC-capable device is plugged in.
  * @retval Status
  */
static int usbh_uvc_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UVC Attach\n");
	usbh_uvc_frame_count = 0;
	return HAL_OK;
}

/**
  * @brief  UVC detach callback, fired when the device is unplugged.
  * @retval Status
  */
static int usbh_uvc_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UVC Detach, frames: %d\n", usbh_uvc_frame_count);
	usbh_uvc_uac_is_ready = 0;
	/* Halt ISOC; frame buffers remain (freed only by usbh_uvc_deinit). */
	usbh_uvc_stop(USBH_UVC_STREAM_INDEX);
	return HAL_OK;
}

/**
  * @brief  UVC setup callback, fired after device enumeration and class
  *         setup completes. Marks the device ready and wakes the stream thread.
  * @retval Status
  */
static int usbh_uvc_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UVC SETUP\n");
	usbh_uvc_uac_is_ready = 1;
	rtos_sema_give(usbh_uvc_stream_start_sema);
	return HAL_OK;
}

/**
  * @brief  UVC set_param callback, fired after Probe/Commit/SET_INTERFACE
  *         negotiation completes.
  * @param  status: HAL_OK on success, HAL_ERR_HW on failure.
  * @retval Status
  */
static int usbh_uvc_cb_setparam(int status)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UVC setparam status=%d\n", status);
	usbh_uvc_setparam_status = (u8)status;
	rtos_sema_give(usbh_uvc_setparam_sema);
	return HAL_OK;
}

/**
  * @brief  UAC class init callback.
  * @retval Status
  */
static int usbh_uac_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC Init\n");
	return HAL_OK;
}

/**
  * @brief  UAC class deinit callback.
  * @retval Status
  */
static int usbh_uac_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC Deinit\n");
	return HAL_OK;
}

/**
  * @brief  UAC attach callback.
  * @retval Status
  */
static int usbh_uac_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC Attach\n");
	usbh_uac_play_count = 0;
	usbh_uac_record_count = 0;
	return HAL_OK;
}

/**
  * @brief  UAC detach callback.
  * @retval Status
  */
static int usbh_uac_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC Detached, played: %d, recorded: %d\n",
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

	return HAL_OK;
}

/**
  * @brief  UAC setup callback, fired after device enumeration.
  * @retval Status
  */
static int usbh_uac_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC SETUP\n");

	usbh_uvc_uac_is_ready = 1;

	/* Signal the UVC stream thread that UAC CLASS_REQUEST setup is done. */
	rtos_sema_give(usbh_uac_ready_sema);

	return HAL_OK;
}

/**
  * @brief  Generic USB host process callback.
  * @param  host: USB host handle (unused).
  * @param  msg:  Event identifier dispatched by the host core.
  * @retval Status
  */
static int usbh_uac_cb_process(usb_host_t *host, u8 msg)
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

	return HAL_OK;
}

/**
  * @brief  Composite setup + UVC stream worker thread.
  *
  *         Sequential ctrl setup flow each attach cycle:
  *           1. UVC Probe/Commit/SET_INTERFACE (usbh_uvc_set_param, async, cb-waited)
  *           2. UVC ISOC data start          (usbh_uvc_start)
  *           3. UAC OUT SET_INTERFACE        (usbh_uac_set_alt_setting, blocking)
  *           4. UAC IN  SET_INTERFACE        (usbh_uac_set_alt_setting, blocking)
  *           5. UAC playback data start      (usbh_uac_start_play)
  *           6. UAC capture data start       (usbh_uac_start_capture)
  *         After 1.–6. the play/record data threads are released via their semas.
  *         Then enters the UVC frame-pull loop until the device detaches.
  * @param  param: Unused.
  * @retval None
  */
static void usbh_uvc_stream_thread(void *param)
{
	const usbh_uac_audio_fmt_t *fmt_out = NULL;
	const usbh_uac_audio_fmt_t *fmt_in = NULL;
	usbh_uvc_frame_t *frame = NULL;
	u32 frame_count = 0U;
	u32 debug_time = 0U;
	u8 is_streaming = 0U;
	u8 is_playing = 0U;
	u8 is_recording = 0U;
	u8 do_ctrl_setup = 0U;
	u8 fmt_cnt = 0U;
	u8 i = 0U;

	UNUSED(param);

	while (usbh_uvc_stream_thread_exit == 0U) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "UVC stream wait setup\n");
		do_ctrl_setup = 0U;

		if (rtos_sema_take(usbh_uvc_stream_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}
		if (usbh_uvc_stream_thread_exit != 0U) {
			break;
		}

		if (usbh_uvc_uac_is_ready != 0U) {
			do_ctrl_setup = 1U;
		} else {
			RTK_LOGS(TAG, RTK_LOG_WARN, "UVC not ready\n");
		}

		if (do_ctrl_setup != 0U) {
			/* Wait for UAC CLASS_REQUEST setup to finish (usually already done) */
			if (rtos_sema_take(usbh_uac_ready_sema, USBH_COMPOSITE_CTRL_TIMEOUT_MS) != RTK_SUCCESS) {
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
			usbh_uvc_set_param(&usbh_uvc_s_ctx, USBH_UVC_STREAM_INDEX);
			if (rtos_sema_take(usbh_uvc_setparam_sema, USBH_COMPOSITE_CTRL_TIMEOUT_MS) != RTK_SUCCESS) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "UVC setparam timeout\n");
			} else if (usbh_uvc_setparam_status != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "UVC setparam fail=%d\n", usbh_uvc_setparam_status);
			} else {
				/* 2. UVC start: begin ISOC video data transfer */
				usbh_uvc_start(USBH_UVC_STREAM_INDEX);
				is_streaming = 1U;
				frame_count = 0U;
				RTK_LOGS(TAG, RTK_LOG_INFO, "UVC streaming started\n");
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
					/* Find preferred CH/FREQ; fall back to index 0 */
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
						/* 5. UAC play start: begin ISOC audio OUT data transfer */
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
					/* Find preferred CH/FREQ; fall back to index 0 */
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
						/* 6. UAC capture start: begin ISOC audio IN data transfer */
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
		}

		/* All ctrl setups done; release play and record data threads */
		rtos_sema_give(usbh_uac_play_start_sema);
		rtos_sema_give(usbh_uac_record_start_sema);

		/* Video data loop */
		while ((usbh_uvc_uac_is_ready != 0U) && (usbh_uvc_stream_thread_exit == 0U)) {
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
			} else {
				rtos_time_delay_ms(USBH_UVC_FRAME_POLL_DELAY_MS);
			}
		}

		if (is_streaming != 0U) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "UVC stream stop, total frames=%d\n", frame_count);
			usbh_uvc_stop(USBH_UVC_STREAM_INDEX);
			is_streaming = 0U;
		}
		if (is_playing != 0U) {
			usbh_uac_stop_play();
			is_playing = 0U;
		}
		if (is_recording != 0U) {
			usbh_uac_stop_capture();
			is_recording = 0U;
		}
		rtos_time_delay_ms(USBH_UAC_VERIFY_SETTLE_MS);
	}

	if (is_streaming != 0U) {
		usbh_uvc_stop(USBH_UVC_STREAM_INDEX);
	}
	if (is_playing != 0U) {
		usbh_uac_stop_play();
	}
	if (is_recording != 0U) {
		usbh_uac_stop_capture();
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
		if (usbh_uac_play_thread_exit != 0U) {
			break;
		}
		if ((usbh_uvc_uac_is_ready == 0U) || (usbh_uac_support_playback() == 0)) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Play not ready or not supported\n");
			continue;
		}

		frame_size = usbh_uac_get_frame_size(USBH_UAC_ISOC_OUT_DIR);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Play data start, frame=%d\n", frame_size);

		play_loop_count = 0U;
		while ((usbh_uvc_uac_is_ready != 0U) && (usbh_uac_play_thread_exit == 0U)) {
			play_loop_count++;
			usbh_uac_play_count++;

			/* Send silence (zeros) as demo playback data */
			(void)usbh_uac_write(silence, (u32)sizeof(silence), USBH_UAC_WRITE_TIMEOUT_MS);

			if ((usb_os_get_timestamp_ms() - debug_time) >= USBH_UAC_DEBUG_TRACE_STEP) {
				debug_time = usb_os_get_timestamp_ms();
				RTK_LOGS(TAG, RTK_LOG_INFO, "Play loop=%d err=%d\n",
						 play_loop_count, usbh_uac_err_count);
			}

			rtos_time_delay_ms(USBH_UAC_PLAY_LOOP_DELAY_MS);
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Play data stop, loops=%d\n", play_loop_count);
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
#endif

	UNUSED(param);

	while (usbh_uac_record_thread_exit == 0U) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Rec wait start\n");

		/* Waits until the setup thread completes all ctrl setup and calls start_capture() */
		if (rtos_sema_take(usbh_uac_record_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}
		if (usbh_uac_record_thread_exit != 0U) {
			break;
		}
		if ((usbh_uvc_uac_is_ready == 0U) || (usbh_uac_support_record() == 0)) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Rec not ready or not supported\n");
			continue;
		}

		frame_size = usbh_uac_get_frame_size(USBH_UAC_ISOC_IN_DIR);

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
		fmt_info = usbh_uac_get_alt_setting(USBH_UAC_ISOC_IN_DIR, &fmt_cnt);
		if ((fmt_info != NULL) && (fmt_cnt > 0U)) {
			g_track_rate = fmt_info[0].sampling_freq;
			g_track_channel = fmt_info[0].ch_cnt;
			g_track_format = fmt_info[0].bit_width;

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

			audio_track = AudioTrack_Create();
			if (audio_track == NULL) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Rec track create fail\n");
				continue;
			}

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
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Rec track start fail\n");
				AudioTrack_Destroy(audio_track);
				audio_track = NULL;
				continue;
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "Rec track started\n");
		}
#endif

		RTK_LOGS(TAG, RTK_LOG_INFO, "Rec data start, frame=%d\n", frame_size);

		record_loop_count = 0U;
		total_read = 0U;

		while ((usbh_uvc_uac_is_ready != 0U) && (usbh_uac_record_thread_exit == 0U)) {
			read_len = USBH_UAC_RECORD_BUFFER_SIZE;
			ret = usbh_uac_read(usbh_uac_record_buffer, read_len, USBH_UAC_READ_TIMEOUT_MS);

			if (ret > 0U) {
				record_loop_count++;
				usbh_uac_record_count++;
				total_read += ret;

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
				if (audio_track != NULL) {
					AudioTrack_Write(audio_track, (u8 *)usbh_uac_record_buffer, ret, true);
				}
#endif

				if ((usb_os_get_timestamp_ms() - debug_time) >= USBH_UAC_DEBUG_TRACE_STEP) {
					debug_time = usb_os_get_timestamp_ms();
					RTK_LOGS(TAG, RTK_LOG_INFO, "Rec loop=%d bytes=%d\n",
							 record_loop_count, total_read);
				}
			}
			/* ret == 0U: read timeout / no data this interval */
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

#if USBH_UVC_UAC_HOT_PLUG_TEST
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
	int ret = 0;
	u32 hotplug_count = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(usbh_uvc_uac_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			hotplug_count++;
			RTK_LOGS(TAG, RTK_LOG_INFO, "\n========== Hotplug #%d ==========\n", hotplug_count);

			rtos_time_delay_ms(200);

			/* Cleanup USB resources */
			usbh_stop();
			usbh_uvc_deinit();
			usbh_uac_deinit();
			usbh_deinit();

			rtos_time_delay_ms(100);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			/* Clear stale semaphores */
			while (rtos_sema_take(usbh_uac_play_start_sema, 0) == RTK_SUCCESS);
			while (rtos_sema_take(usbh_uac_record_start_sema, 0) == RTK_SUCCESS);
			while (rtos_sema_take(usbh_uvc_stream_start_sema, 0) == RTK_SUCCESS);
			while (rtos_sema_take(usbh_uac_ready_sema, 0) == RTK_SUCCESS);

			/* Reinitialize USB stack */
			RTK_LOGS(TAG, RTK_LOG_INFO, "Re-init USB host...\n");

			ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init failed: %d\n", ret);
				break;
			}

			ret = usbh_uvc_init(&usbh_uvc_cfg_ctx, &usbh_uvc_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "UVC init failed: %d\n", ret);
				usbh_deinit();
				break;
			}

			ret = usbh_uac_init(&usbh_uac_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC init failed: %d\n", ret);
				usbh_uvc_deinit();
				usbh_deinit();
				break;
			}

			/* Re-arm USB TRX after the re-init. */
			usbh_start();

			RTK_LOGS(TAG, RTK_LOG_INFO, "Re-init complete\n");
		}
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread exited\n");
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
static void usbh_comp_compute_expected_db(s16 vol_min, s16 vol_max, u8 percent, s16 *expect)
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
static void usbh_comp_verify_mute(u8 dir, u8 expected_mute)
{
	const usbh_uac_fu_info_t *info;
	u8 verified = 0;
	u8 ok = 0;
	u8 fail = 0;
	u8 ch;

	rtos_time_delay_ms(USBH_UAC_VERIFY_SETTLE_MS);

	info = usbh_uac_get_volume_ctrl_info(dir);
	if (info == NULL) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Verify mute(%d) no FU\n", dir);
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
static void usbh_comp_verify_volume(u8 dir, u8 expected_percent)
{
	const usbh_uac_fu_info_t *info;
	u8 verified = 0;
	u8 ok = 0;
	u8 warn = 0;
	u8 err = 0;
	u8 ch;

	rtos_time_delay_ms(USBH_UAC_VERIFY_SETTLE_MS);

	info = usbh_uac_get_volume_ctrl_info(dir);
	if (info == NULL) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Verify vol(%d) no FU\n", dir);
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

		usbh_comp_compute_expected_db(vol_min, vol_max, expected_percent, &expect_db);

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
static u32 uach_comp_volup(u8 *vol)
{
	u8 cur_vol;

	if (vol == NULL) {
		return HAL_ERR_PARA;
	}

	cur_vol = *vol;

	cur_vol += USBH_UAC_VOLUME_STEP;
	if (cur_vol > 100) {
		cur_vol = 100;
	}
	usbh_uac_set_volume(cur_vol, uac_ctrl_dir);
	*vol = cur_vol;

	return HAL_OK;
}

/**
  * @brief  Decrease the volume by USBH_UAC_VOLUME_STEP percent, clamped at 0%.
  * @param  vol: Pointer to the current volume value; updated in place.
  * @retval HAL_OK on success, HAL_ERR_PARA if vol is NULL.
  */
static u32 uach_comp_voldown(u8 *vol)
{
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

	usbh_uac_set_volume(cur_vol, uac_ctrl_dir);
	*vol = cur_vol;

	return HAL_OK;
}

static u32 uach_comp_cmd(u16 argc, u8 *argv[])
{
	const char *cmd;
	u8 *cur_vol;
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

		usbh_uac_set_mute(mute, uac_ctrl_dir);
		RTK_LOGS(TAG, RTK_LOG_INFO, "%s\n", ((mute) ? ("Mute") : ("UnMute")));

		usbh_comp_verify_mute(uac_ctrl_dir, mute);
	} else if (_stricmp(cmd, "vol") == 0) {
		cur_vol = (uac_ctrl_dir == USBH_UAC_ISOC_IN_DIR) ? &cur_record_volume : &cur_playback_volume;

		if ((argv[1] != NULL) && (_stricmp((const char *)argv[1], "down") == 0)) {
			uach_comp_voldown(cur_vol);
		} else {
			uach_comp_volup(cur_vol);
		}

		usbh_comp_verify_volume(uac_ctrl_dir, *cur_vol);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Input cmd is err\n");
		return HAL_ERR_PARA;
	}

	return HAL_OK;
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

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH UVC&UAC composite demo start\n");

	status = rtos_task_create(&task, "usbh_uvc_uac_main", example_usbh_uac_uvc_thread, NULL, 1024U * 2, 2U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Failed to create main thread\n");
	}
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

	/* Create synchronization primitives */
	rtos_sema_create(&usbh_uvc_uac_detach_sema, 0U, 1U);
	rtos_sema_create(&usbh_uac_play_start_sema, 0U, 1U);
	rtos_sema_create(&usbh_uac_record_start_sema, 0U, 1U);
	rtos_sema_create(&usbh_uvc_stream_start_sema, 0U, 1U);
	rtos_sema_create(&usbh_uvc_setparam_sema, 0U, 1U);
	rtos_sema_create(&usbh_uac_ready_sema, 0U, 1U);

	/* Create UVC stream thread */
	status = rtos_task_create(&usbh_uvc_stream_task, "usbh_uvc_stream", usbh_uvc_stream_thread,
							  NULL, 2048, USBH_UVC_STREAM_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create UVC stream thread fail\n");
		goto free_sema_exit;
	}

	/* Create resident playback thread */
	status = rtos_task_create(&usbh_uac_play_task, "usbh_uac_play", usbh_uac_play_thread,
							  NULL, 1024, USBH_UAC_PLAY_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create play thread fail\n");
		goto delete_stream_task_exit;
	}

	/* Create resident record thread */
	status = rtos_task_create(&usbh_uac_record_task, "usbh_uac_record", usbh_uac_record_thread,
							  NULL, 1536, USBH_UAC_RECORD_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create record thread fail\n");
		goto delete_play_task_exit;
	}

	/* Initialize USB host stack */
	status = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH init failed: %d\n", status);
		goto delete_record_task_exit;
	}

	/* Register the UVC class driver */
	status = usbh_uvc_init(&usbh_uvc_cfg_ctx, &usbh_uvc_cb);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UVC init failed: %d\n", status);
		goto usb_deinit_exit;
	}

	/* Register the UAC class driver */
	status = usbh_uac_init(&usbh_uac_cb);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC init failed: %d\n", status);
		usbh_uvc_deinit();
		goto usb_deinit_exit;
	}

	/* Both class drivers (UVC + UAC) registered; start USB TRX so Phase-1
	 * match sees both drivers and enumeration can proceed. */
	usbh_start();

	/* Create hot-plug monitor thread */
#if USBH_UVC_UAC_HOT_PLUG_TEST
	status = rtos_task_create(&usbh_uvc_uac_hotplug_task, "usbh_uvc_uac_hotplug", usbh_uvc_uac_hotplug_thread,
							  NULL, 768, USBH_UVC_UAC_HOTPLUG_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
		goto class_deinit_exit;
	}
#endif

	goto example_exit;

#if USBH_UVC_UAC_HOT_PLUG_TEST
class_deinit_exit:
	usbh_stop();
	usbh_uac_deinit();
	usbh_uvc_deinit();
#endif

usb_deinit_exit:
	usbh_deinit();

delete_record_task_exit:
	usbh_uac_play_thread_exit = 1;
	usbh_uac_record_thread_exit = 1;
	usbh_uvc_stream_thread_exit = 1;
	/* Give the start semaphores so that threads blocked on rtos_sema_take()
	   can wake up and observe the exit flag instead of waiting indefinitely. */
	rtos_sema_give(usbh_uac_play_start_sema);
	rtos_sema_give(usbh_uac_record_start_sema);
	rtos_sema_give(usbh_uvc_stream_start_sema);
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

free_sema_exit:
	rtos_sema_delete(usbh_uvc_uac_detach_sema);
	rtos_sema_delete(usbh_uac_play_start_sema);
	rtos_sema_delete(usbh_uac_record_start_sema);
	rtos_sema_delete(usbh_uvc_stream_start_sema);
	rtos_sema_delete(usbh_uvc_setparam_sema);
	rtos_sema_delete(usbh_uac_ready_sema);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Demo stopped\n");

example_exit:
	rtos_task_delete(NULL);
}

/*
uach init record/playback
uach mute 1/0     # set mute, then sync GET_CUR(MUTE) to verify the device accepted it
uach vol down/up  # adjust volume, then sync GET_CUR(VOLUME) to verify
*/
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbh_composite_uvc_uac_test_cmd_table[] = {
	{"uach", uach_comp_cmd},
};
