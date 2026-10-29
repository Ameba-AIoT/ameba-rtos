/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "os_wrapper.h"
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
 * @brief Hot-plug / memory-leak stress-test enable.
 *        1 = soft re-init on every device detach, 0 = run once.
 */
#define USBH_UAC_HOT_PLUG_TEST                  1

/**
 * @brief Ring-buffer depth (USB frames) reserved for each ISOC path.
 *        Passed to usbh_uac_init via usbh_uac_cb_t::isoc_*_frm_cnt.
 *        At least one of OUT/IN must be non-zero.
 */
#define USBH_UAC_FRAME_CNT                      20

/**
 * @brief Number of full-buffer playback loops performed per test run.
 */
#define USBH_UAC_TEST_CNT                       60

/**
 * @brief Playback statistics print interval (ms).
 */
#define USBH_UAC_DEBUG_TRACE_STEP               1000

/**
 * @brief Record capture buffer size (bytes) per usbh_uac_read() call.
 */
#define USBH_UAC_RECORD_BUFFER_SIZE             1024

/* Playback audio format configuration ----------------------------------------
 * The PCM data in the audio header file uses this format.  The UAC device must
 * support the same format or the demo will fall back gracefully.
 */
#define USBH_UAC_PLAY_CHANNELS                  2       /* Stereo */
#define USBH_UAC_PLAY_BITWIDTH                  16      /* 16-bit samples */
#define USBH_UAC_PLAY_SAMPLING_FREQ             48000   /* 48 kHz */

/* Thread priorities (higher number = higher priority) */
#define USBH_UAC_INIT_THREAD_PRIORITY           5
#define USBH_UAC_MAIN_TASK_PRIORITY             5
#define USBH_UAC_HOTPLUG_THREAD_PRIORITY        6
#define USBH_UAC_PLAY_THREAD_PRIORITY           4
#define USBH_UAC_RECORD_THREAD_PRIORITY         4

/* Thread stack sizes */
#define USBH_UAC_INIT_THREAD_STACK_SIZE         (1024U * 2)
#define USBH_UAC_MAIN_TASK_STACK_SIZE           1024U
#define USBH_UAC_HOTPLUG_THREAD_STACK_SIZE      (1024U * 2)
#define USBH_UAC_PLAY_THREAD_STACK_SIZE         (1024U * 2)
#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
#define USBH_UAC_RECORD_THREAD_STACK_SIZE       (1024U * 3)
#else
#define USBH_UAC_RECORD_THREAD_STACK_SIZE       (1024U)
#endif

/**
 * @brief Fill playback buffer with a fixed pattern (0x88) for TX validation
 *        instead of real PCM audio.  Disable for normal operation.
 */
#define USBH_UAC_XFER_CHECK                     0

#if USBH_UAC_XFER_CHECK
#define USBH_UAC_OUT_DATA                       0x88
#else
#include "example_usbh_uac_audio_data.h"
#endif

/* Private types -------------------------------------------------------------*/

/**
 * @brief Per-channel (playback or record) runtime state.
 */
typedef struct {
	__IO int thread_exit;   /*!< Signal the worker thread to stop */
	rtos_task_t task;       /*!< Worker thread handle */
	int err_count;          /*!< Transfer error counter */
	u32 count;              /*!< Total successful loop count */
	u8 cur_volume;          /*!< Current volume percentage [0..100] */
} uac_chan_ctx_t;

/**
 * @brief Top-level application context - all UAC demo state lives here.
 */
typedef struct {
	/* ---- Playback state ---- */
	uac_chan_ctx_t play;
	/* ---- Record state ---- */
	uac_chan_ctx_t record;

	/* ---- Synchronisation ---- */
	rtos_sema_t detach_sema;        /*!< Given by cb_detached; taken by hotplug thread */
	rtos_sema_t play_start_sema;    /*!< Given when setup completes; wakes play thread */
	rtos_sema_t record_start_sema;  /*!< Given when setup completes; wakes record thread */

#if USBH_UAC_HOT_PLUG_TEST
	rtos_task_t hotplug_task;       /*!< Hot-plug monitoring thread handle */
#endif

	__IO u8 is_ready;               /*!< Device enumerated and configured */
} uac_ctx_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static u32 uach_mute(u16 argc, u8 *argv[]);
static u32 uach_vol(u16 argc, u8 *argv[]);
static u32 uach_voldown(u16 argc, u8 *argv[]);
static u32 uach_volup(u16 argc, u8 *argv[]);

static int usbh_uac_cb_init(void);
static void usbh_uac_cb_deinit(void);
static void usbh_uac_cb_attached(void);
static void usbh_uac_cb_detached(void);
static void usbh_uac_cb_setup(void);
static void usbh_uac_cb_transmitted(usbh_urb_state_t state);
static void usbh_uac_cb_process(usb_host_t *host, u8 msg);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UAC";

/** Example-layer playback volume tracker, used by the shell test commands. */
static u8 example_usbh_uac_volume = 50;

#if USBH_UAC_XFER_CHECK
/** Fixed-pattern playback buffer used only by the TX validation path. */
static u32 usbh_uac_data_len = 1200;
static unsigned char usbh_uac_audio_data[1200] = {0,};
#endif

/** Single application-level context instance. */
static uac_ctx_t usbh_uac_ctx;

/** Record capture destination buffer. */
static u8 usbh_uac_record_buffer[USBH_UAC_RECORD_BUFFER_SIZE];

/* Serializes the USB stack bring up / tear down against the class API callers.
   usbh_uac_deinit() frees the ISOC ring buffers and the per-direction format
   tables that usbh_uac_write()/usbh_uac_read()/set_alt_setting() dereference, and
   usbh_deinit() memset()s the whole host handle; the class API only checks its own
   ready state on entry, so under SMP a worker on core 1 can be past that check
   while core 0 frees what it walks. The workers must be excluded by lock, not by
   core.
   Split per direction, plus one for the control requests the shell commands issue,
   so full-duplex playback and capture still run concurrently - a single lock would
   serialize them against each other for no reason. The hotplug thread takes all
   three before tearing down.
   Created before the stack comes up and never deleted: the workers and the shell
   can not be joined by the example, so there is no point in time at which these
   mutexes are provably unreferenced. usbh_uac_stack_ready is the gate that keeps
   callers off the stack instead. */
static rtos_mutex_t usbh_uac_play_mutex = NULL;
static rtos_mutex_t usbh_uac_record_mutex = NULL;
static rtos_mutex_t usbh_uac_ctrl_mutex = NULL;

/* 1: usbh_init() and usbh_uac_init() are both done and the class API may be
   called. Cleared before any tear down, so a non-zero value also implies the three
   mutexes above are valid. */
static volatile u8 usbh_uac_stack_ready = 0;

/* Set by the hotplug thread BEFORE it takes the locks, so a worker inside a
   playback or capture loop gives up its round instead of re-taking a lock between
   its own buffers and starving the teardown out. */
static volatile u8 usbh_uac_teardown = 0;

/* Set by the hotplug thread when the stack can not be rebuilt. Tells the workers
   to stop for good rather than spin against a stack that is gone. */
static volatile u8 usbh_uac_stack_fatal = 0;

static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_FULL,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = USBH_UAC_MAIN_TASK_STACK_SIZE,
	.main_task_priority = USBH_UAC_MAIN_TASK_PRIORITY,
	.tick_source = USBH_SOF_TICK,
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

static const usbh_uac_cb_t usbh_uac_cfg = {
	.init             = usbh_uac_cb_init,
	.deinit           = usbh_uac_cb_deinit,
	.attached           = usbh_uac_cb_attached,
	.detached           = usbh_uac_cb_detached,
	.setup            = usbh_uac_cb_setup,
	.transmitted = usbh_uac_cb_transmitted,
};

static const usbh_uac_config_t uac_class_cfg = {
	/* Ring-buffer depth (frames) for OUT playback and IN record paths; 0 disables that
	 * direction, and usbh_uac_init() rejects the configuration when both are 0. */
	.isoc_out_frm_cnt = USBH_UAC_FRAME_CNT,
	.isoc_in_frm_cnt  = USBH_UAC_FRAME_CNT,
};

static const usbh_user_cb_t usbh_usr_cb = {
	.process = usbh_uac_cb_process
};

/* Exported variables --------------------------------------------------------*/

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbh_uac_test_md_table[] = {
	{"uach_mute",    uach_mute},
	{"uach_vol",     uach_vol},
	{"uach_volup",   uach_volup},
	{"uach_voldown", uach_voldown},
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Take one of the per-direction stack locks if the class API may be used
  * @note   Gates on usbh_uac_stack_ready BEFORE touching the mutex: the handle is
  *         still NULL if the example never started, and the flag is already cleared
  *         if a teardown is pending. A hotplug right after this check is harmless,
  *         the mutex stays valid and the re-check under it catches the loss.
  * @param  lock: One of usbh_uac_play_mutex / _record_mutex / _ctrl_mutex
  * @retval HAL_OK when the lock is held and the stack is usable, HAL_ERR_HW when
  *         the caller must give up without unlocking
  */
static int usbh_uac_lock(rtos_mutex_t lock)
{
	if ((usbh_uac_stack_ready == 0U) || (usbh_uac_teardown != 0U) || (lock == NULL)) {
		return HAL_ERR_HW;
	}

	if (rtos_mutex_take(lock, RTOS_MAX_TIMEOUT) != RTK_SUCCESS) {
		/* Not held: the caller gives up and must not unlock */
		return HAL_ERR_HW;
	}

	if ((usbh_uac_stack_ready == 0U) || (usbh_uac_teardown != 0U)) {
		/* Lost the race against a teardown */
		rtos_mutex_give(lock);
		return HAL_ERR_HW;
	}

	return HAL_OK;
}

/**
  * @brief  Release a lock taken by usbh_uac_lock()
  * @param  lock: The same handle that was passed to usbh_uac_lock()
  * @retval None
  */
static void usbh_uac_unlock(rtos_mutex_t lock)
{
	rtos_mutex_give(lock);
}

/**
  * @brief  UAC class init callback - invoked once when the driver is registered.
  * @retval HAL_OK
  */
static int usbh_uac_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
	return HAL_OK;
}

/**
  * @brief  UAC class deinit callback - invoked when the driver is unregistered.
  */
static void usbh_uac_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
}

/**
  * @brief  UAC attach callback - device plugged in and enumerated.
  */
static void usbh_uac_cb_attached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
}

/**
  * @brief  UAC detach callback - device removed.
  *         Clears is_ready, stops audio pipelines, signals hotplug thread.
  */
static void usbh_uac_cb_detached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH play=%u rec=%u\n",
			 usbh_uac_ctx.play.count, usbh_uac_ctx.record.count);

	usbh_uac_ctx.is_ready = 0;
	/* Deliberately NOT under the play/record locks: this callback runs on the USB
	   main task, and the hotplug thread calls usbh_uac_deinit() while holding them,
	   which reaches this same callback - taking them here would self-deadlock. The
	   workers are kept off the stack by is_ready and the gate instead. */
	usbh_uac_stop_play();
	usbh_uac_stop_capture();

#if USBH_UAC_HOT_PLUG_TEST
	rtos_sema_give(usbh_uac_ctx.detach_sema);
#endif
}

/**
  * @brief  UAC setup callback - device fully configured and ready.
  *         Marks is_ready and wakes play + record worker threads.
  */
static void usbh_uac_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	usbh_uac_ctx.is_ready = 1;
	rtos_sema_give(usbh_uac_ctx.play_start_sema);
	rtos_sema_give(usbh_uac_ctx.record_start_sema);
}

/**
  * @brief  ISOC OUT (playback) transfer complete callback.
  * @param  state: URB completion state
  */
static void usbh_uac_cb_transmitted(usbh_urb_state_t state)
{
	if (state == USBH_URB_DONE) {
		/* TX ok */
	} else if (state == USBH_URB_BUSY) {
		usbh_uac_ctx.play.err_count++;
	} else {
		usbh_uac_ctx.play.err_count++;
	}
}

/**
  * @brief  USB host top-level event callback (connect / disconnect / probe fail).
  * @param  host: USB host handle (unused)
  * @param  msg:  Event identifier
  * @retval HAL_OK
  */
static void usbh_uac_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);

	switch (msg) {
	case USBH_MSG_DISCONNECTED:
		usbh_uac_ctx.is_ready = 0;
		break;
	case USBH_MSG_CONNECTED:
		break;
	case USBH_MSG_PROBE_FAIL:
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Probe Fail\n");
		usbh_uac_ctx.is_ready = 0;
		break;
	default:
		break;
	}
}

/**
  * @brief  Audio playback worker thread.
  *
  *         Resident thread that lives for the duration of the demo.  Each
  *         iteration:
  *           1. Blocks on play_start_sema until the device is set up.
  *           2. Queries OUT alt-settings and selects the matching format.
  *           3. Streams the embedded PCM buffer in random-sized chunks for
  *              USBH_UAC_TEST_CNT iterations.
  *           4. Stops the OUT pipeline and loops back to step 1.
  * @param  param: Unused
  */
static void example_usbh_uac_play_thread(void *param)
{
	const unsigned char *usbh_uac_audio_data_handle = usbh_uac_audio_data;
	const usbh_uac_audio_fmt_t *fmt_info = NULL;
	const usbh_uac_audio_fmt_t *audio_fmt = NULL;
	u32 audio_total_data_len = usbh_uac_data_len;
	u32 play_loop_count = 0;
	u32 frame_size = 0;
	u32 total_len = 0;
	u32 send_len = 0;
	u32 offset = 0;
	u32 debug_time = 0;
	int ret;
	u8 playback_format_find = 0;
	u8 is_playing = 0;
	u8 fmt_cnt;
	u8 i = 0;

	UNUSED(param);

	while (usbh_uac_ctx.play.thread_exit == 0) {
		/*
		 * PHASE 1: WAIT FOR DEVICE
		 */
		RTK_LOGS(TAG, RTK_LOG_INFO, "Play wait setup\n");

		if (rtos_sema_take(usbh_uac_ctx.play_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}

		if (usbh_uac_ctx.play.thread_exit != 0) {
			break;
		}

		if (usbh_uac_ctx.is_ready == 0) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Play not ready\n");
			continue;
		}

		if (usbh_uac_teardown != 0U || usbh_uac_stack_fatal != 0U) {
			continue;
		}

		if (usbh_uac_lock(usbh_uac_play_mutex) != HAL_OK) {
			continue;
		}

		if (usbh_uac_support_playback() == 0) {
			usbh_uac_unlock(usbh_uac_play_mutex);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Play not support\n");
			continue;
		}

		/*
		 * PHASE 2: AUDIO FORMAT SELECTION
		 */
		fmt_info = usbh_uac_get_alt_setting(USBH_UAC_ISOC_OUT_DIR, &fmt_cnt);

		if ((fmt_info == NULL) || (fmt_cnt == 0)) {
			usbh_uac_unlock(usbh_uac_play_mutex);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Play get fmt fail\n");
			continue;
		}

		playback_format_find = 0;

		RTK_LOGS(TAG, RTK_LOG_INFO, "Play fmt list:\n");
		for (i = 0; i < fmt_cnt; i++) {
			audio_fmt = &(fmt_info[i]);
			RTK_LOGS(TAG, RTK_LOG_INFO, "[%d] CH=%d BW=%d FREQ=%d\n",
					 i, audio_fmt->ch_cnt, audio_fmt->bit_width, audio_fmt->sampling_freq);

			if ((playback_format_find == 0)
				&& (audio_fmt->sampling_freq == USBH_UAC_PLAY_SAMPLING_FREQ)
				&& (audio_fmt->bit_width    == USBH_UAC_PLAY_BITWIDTH)
				&& (audio_fmt->ch_cnt       == USBH_UAC_PLAY_CHANNELS)) {
				playback_format_find = 1;
			}
		}

		if (playback_format_find == 0) {
			usbh_uac_unlock(usbh_uac_play_mutex);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Play fmt mismatch\n");
			continue;
		}

		if (usbh_uac_set_alt_setting(USBH_UAC_ISOC_OUT_DIR,
									 USBH_UAC_PLAY_CHANNELS,
									 USBH_UAC_PLAY_BITWIDTH,
									 USBH_UAC_PLAY_SAMPLING_FREQ) != HAL_OK) {
			usbh_uac_unlock(usbh_uac_play_mutex);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Play set alt fail\n");
			continue;
		}

		frame_size = usbh_uac_get_frame_size(USBH_UAC_ISOC_OUT_DIR);
		/* Release before the sleep and the playback loop below: the loop re-takes
		   the lock per buffer, so a teardown only waits for one usbh_uac_write()
		   rather than for the whole playback session. */
		usbh_uac_unlock(usbh_uac_play_mutex);
		rtos_time_delay_ms(100);

		RTK_LOGS(TAG, RTK_LOG_INFO, "Play start, data=%d frame=%d test_cnt=%d\n",
				 audio_total_data_len, frame_size, USBH_UAC_TEST_CNT);

#if USBH_UAC_XFER_CHECK
		usb_os_memset((void *)usbh_uac_audio_data, USBH_UAC_OUT_DATA, usbh_uac_data_len);
#endif

		/*
		 * PHASE 3: CONTINUOUS PLAYBACK
		 */
		if (usbh_uac_ctx.is_ready != 0) {
			if (usbh_uac_lock(usbh_uac_play_mutex) != HAL_OK) {
				continue;
			}
			usbh_uac_start_play();
			usbh_uac_unlock(usbh_uac_play_mutex);
			is_playing = 1;
			play_loop_count = 0;
			usbh_uac_ctx.play.err_count = 0;
		}

		total_len = audio_total_data_len;

		while (usbh_uac_ctx.is_ready != 0 && usbh_uac_ctx.play.thread_exit == 0) {
			offset = 0;
			play_loop_count++;
			usbh_uac_ctx.play.count++;

			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Play loop %d total %d\n",
					 play_loop_count, usbh_uac_ctx.play.count);

			while (offset < total_len && usbh_uac_ctx.is_ready != 0
				   && usbh_uac_ctx.play.thread_exit == 0) {
				send_len = 0;
				if (TRNG_get_random_bytes(&send_len, 2) != RTK_SUCCESS) {
					send_len = 0;
				}
				send_len = send_len & 0x0FFFU;

				/* A failed or zero draw falls back to one packet, so every round moves data
				   instead of spinning without a delay */
				if (send_len == 0) {
					send_len = (frame_size != 0U) ? frame_size : 1U;
				}

				if (offset + send_len > total_len) {
					send_len = total_len - offset;
				}

				if (usbh_uac_lock(usbh_uac_play_mutex) != HAL_OK) {
					goto play_stop;
				}
				ret = usbh_uac_write((u8 *)(usbh_uac_audio_data_handle + offset), send_len, 10);
				usbh_uac_unlock(usbh_uac_play_mutex);

				if (ret < 0) {
					/* Stream stopped or device gone: end this session, the next setup restarts it */
					RTK_LOGS(TAG, RTK_LOG_WARN, "Play write err %d\n", -ret);
					goto play_stop;
				}

				/* Keep what was queued and retry only the rest: resending it would play it twice */
				offset += (u32)ret;
				if ((u32)ret != send_len) {
					/* Ring stayed full: the device consumes slower than expected, back off */
					usbh_uac_ctx.play.err_count++;
					rtos_time_delay_ms(2);
				}
			}

			if ((usb_os_get_timestamp_ms() - debug_time) >= USBH_UAC_DEBUG_TRACE_STEP) {
				debug_time = usb_os_get_timestamp_ms();
				RTK_LOGS(TAG, RTK_LOG_INFO, "Play status loop=%d err=%d\n",
						 play_loop_count, usbh_uac_ctx.play.err_count);
			}

			/* Honour test count limit when HOT_PLUG_TEST is disabled */
#if !USBH_UAC_HOT_PLUG_TEST
			if (play_loop_count >= (u32)USBH_UAC_TEST_CNT) {
				break;
			}
#endif
		}

play_stop:
		if (is_playing != 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Play stop loops=%d\n", play_loop_count);
			/* Best effort: on a closed gate the stack is already down and the
			   class has stopped the stream itself in the detach callback. */
			if (usbh_uac_lock(usbh_uac_play_mutex) == HAL_OK) {
				usbh_uac_stop_play();
				usbh_uac_unlock(usbh_uac_play_mutex);
			}
			is_playing = 0;
			rtos_time_delay_ms(50);
		}

		if (usbh_uac_stack_fatal != 0U) {
			break;
		}
	}

	if (is_playing != 0) {
		if (usbh_uac_lock(usbh_uac_play_mutex) == HAL_OK) {
			usbh_uac_stop_play();
			usbh_uac_unlock(usbh_uac_play_mutex);
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Play exit\n");
	usbh_uac_ctx.play.task = NULL;
	rtos_task_delete(NULL);
}

/**
  * @brief  Audio record worker thread.
  *
  *         Resident thread that lives for the duration of the demo.  Each
  *         iteration:
  *           1. Blocks on record_start_sema until the device is set up.
  *           2. Queries IN alt-settings and selects the first advertised one.
  *           3. (Optional) Creates an AudioTrack for local monitoring when
  *              CONFIG_SUPPORT_AUDIO_FOR_USB is enabled.
  *           4. Pulls ISOC IN data into usbh_uac_record_buffer until the
  *              device disappears or the demo is shutting down.
  *           5. Tears down the IN pipeline / AudioTrack and returns to step 1.
  * @param  param: Unused
  */
static void example_usbh_uac_record_thread(void *param)
{
	const usbh_uac_audio_fmt_t *fmt_info = NULL;
	const usbh_uac_audio_fmt_t *audio_fmt = NULL;
	u32 record_loop_count = 0;
	u32 frame_size = 0;
	u32 total_read = 0;
	u32 read_len = 0;
	u32 debug_time = 0;
	int ret;
	u8 is_recording = 0;
	u8 fmt_cnt;
	u8 i = 0;

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
	struct AudioTrack *audio_track = NULL;
	uint32_t format;
	int32_t track_buf_size;
	uint32_t g_track_rate = 0;
	uint32_t g_track_channel = 0;
	uint32_t g_track_format = 0;
#endif

	UNUSED(param);

	while (usbh_uac_ctx.record.thread_exit == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Rec wait setup\n");

		if (rtos_sema_take(usbh_uac_ctx.record_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}

		if (usbh_uac_ctx.record.thread_exit != 0) {
			break;
		}

		if (usbh_uac_ctx.is_ready == 0) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Rec not ready\n");
			continue;
		}

		if (usbh_uac_teardown != 0U || usbh_uac_stack_fatal != 0U) {
			continue;
		}

		if (usbh_uac_lock(usbh_uac_record_mutex) != HAL_OK) {
			continue;
		}

		if (usbh_uac_support_record() == 0) {
			usbh_uac_unlock(usbh_uac_record_mutex);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Rec not support\n");
			continue;
		}

		fmt_info = usbh_uac_get_alt_setting(USBH_UAC_ISOC_IN_DIR, &fmt_cnt);

		if ((fmt_info == NULL) || (fmt_cnt == 0)) {
			usbh_uac_unlock(usbh_uac_record_mutex);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Rec get fmt fail\n");
			continue;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Rec fmt list:\n");
		for (i = 0; i < fmt_cnt; i++) {
			audio_fmt = &(fmt_info[i]);
			RTK_LOGS(TAG, RTK_LOG_INFO, "[%d] CH=%d BW=%d FREQ=%d\n",
					 i, audio_fmt->ch_cnt, audio_fmt->bit_width, audio_fmt->sampling_freq);
		}

		/* Select the first advertised IN format */
		if (usbh_uac_set_alt_setting(USBH_UAC_ISOC_IN_DIR,
									 fmt_info[0].ch_cnt,
									 fmt_info[0].bit_width,
									 fmt_info[0].sampling_freq) != HAL_OK) {
			usbh_uac_unlock(usbh_uac_record_mutex);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Rec set alt fail\n");
			continue;
		}

		frame_size = usbh_uac_get_frame_size(USBH_UAC_ISOC_IN_DIR);
#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
		/* Copy the format fields out while still holding the lock: fmt_info points
		   into the class format table, which usbh_uac_deinit() frees once the lock
		   is released. Everything below uses only the copies. */
		g_track_rate    = fmt_info[0].sampling_freq;
		g_track_channel = fmt_info[0].ch_cnt;
		g_track_format  = fmt_info[0].bit_width;
#endif
		fmt_info = NULL;
		/* Release before the AudioTrack setup and the capture loop: both can run
		   for a long time, and the loop re-takes the lock per read so a teardown
		   only waits for one usbh_uac_read(). */
		usbh_uac_unlock(usbh_uac_record_mutex);
		rtos_time_delay_ms(100);

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
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

		/* Local monitoring output is optional. On any failure below, audio_track is left NULL
		   and capture still starts: the IN alt setting is already selected, and the data is
		   read and dropped instead. */
		audio_track = AudioTrack_Create();
		if (audio_track == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Rec track create fail, drain without output\n");
		} else {
			AudioTrackConfig track_config;

			track_buf_size = AudioTrack_GetMinBufferBytes(audio_track, AUDIO_CATEGORY_MEDIA,
							 g_track_rate, format, g_track_channel) * 10;
			if (track_buf_size == 0) {
				track_buf_size = (int32_t)(g_track_rate * g_track_format / 8
										   * g_track_channel / 1000 * 100);
				RTK_LOGS(TAG, RTK_LOG_INFO, "Rec track buf resize %d\n", track_buf_size);
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Rec track buf %d\n", track_buf_size);
			}

			track_config.category_type  = AUDIO_CATEGORY_MEDIA;
			track_config.sample_rate    = g_track_rate;
			track_config.format         = format;
			track_config.channel_count  = g_track_channel;
			track_config.buffer_bytes   = track_buf_size;
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
#endif

		RTK_LOGS(TAG, RTK_LOG_INFO, "Rec start, frame=%d\n", frame_size);

		if (usbh_uac_ctx.is_ready != 0) {
			if (usbh_uac_lock(usbh_uac_record_mutex) != HAL_OK) {
				goto record_stop;
			}
			usbh_uac_start_capture();
			usbh_uac_unlock(usbh_uac_record_mutex);
			is_recording = 1;
			record_loop_count = 0;
			total_read = 0;
		}

		while (usbh_uac_ctx.is_ready != 0 && usbh_uac_ctx.record.thread_exit == 0) {
			read_len = USBH_UAC_RECORD_BUFFER_SIZE;
			if (usbh_uac_lock(usbh_uac_record_mutex) != HAL_OK) {
				goto record_stop;
			}
			ret = usbh_uac_read(usbh_uac_record_buffer, read_len, 1000);
			usbh_uac_unlock(usbh_uac_record_mutex);

			if (ret < 0) {
				/* Stream stopped or device gone: end this session, the next setup restarts it */
				RTK_LOGS(TAG, RTK_LOG_WARN, "Rec read err %d\n", -ret);
				goto record_stop;
			}

			if (ret > 0) {
				record_loop_count++;
				usbh_uac_ctx.record.count++;
				total_read += (u32)ret;

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
				if (audio_track != NULL) {
					AudioTrack_Write(audio_track, (u8 *)usbh_uac_record_buffer, (u32)ret, true);
				}
#endif
			} else {
				/* Running stream, no data within the timeout: count it and keep waiting */
				usbh_uac_ctx.record.err_count++;
			}

			if ((usb_os_get_timestamp_ms() - debug_time) >= USBH_UAC_DEBUG_TRACE_STEP) {
				debug_time = usb_os_get_timestamp_ms();
				RTK_LOGS(TAG, RTK_LOG_INFO, "Rec status loop=%d bytes=%d empty=%d\n",
						 record_loop_count, total_read, usbh_uac_ctx.record.err_count);
			}
		}

record_stop:
		if (is_recording != 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Rec stop loops=%d bytes=%d\n",
					 record_loop_count, total_read);
			/* Best effort: on a closed gate the stack is already down and the class
			   has stopped the stream itself in the detach callback. */
			if (usbh_uac_lock(usbh_uac_record_mutex) == HAL_OK) {
				usbh_uac_stop_capture();
				usbh_uac_unlock(usbh_uac_record_mutex);
			}
			is_recording = 0;
			rtos_time_delay_ms(50);
		}

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

		if (usbh_uac_stack_fatal != 0U) {
			break;
		}
	}

	if (is_recording != 0) {
		if (usbh_uac_lock(usbh_uac_record_mutex) == HAL_OK) {
			usbh_uac_stop_capture();
			usbh_uac_unlock(usbh_uac_record_mutex);
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
	usbh_uac_ctx.record.task = NULL;
	rtos_task_delete(NULL);
}

#if USBH_UAC_HOT_PLUG_TEST
/**
  * @brief  Hot-plug worker thread.
  *         Blocks on detach_sema.  On each detach event it tears down UAC and
  *         the USB host, drains stale start semaphores so play/record threads
  *         do not re-enter with stale state, then re-initializes the host.
  * @param  param: Unused
  */
static void example_usbh_uac_hotplug_thread(void *param)
{
	int ret;
	u32 hotplug_count = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(usbh_uac_ctx.detach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}

		hotplug_count++;
		RTK_LOGS(TAG, RTK_LOG_INFO, "========== Hotplug #%u ==========\n", hotplug_count);

		rtos_time_delay_ms(200);

		/* Announce the teardown BEFORE contending for the locks, so a worker inside
		 * a playback or capture loop gives up its round instead of re-taking a lock
		 * between buffers and starving this thread out of a teardown it must do. */
		usbh_uac_teardown = 1U;

		/* All three locks, always in this order (play, record, ctrl). The workers
		 * and the shell each take exactly one, never two, so a fixed order here is
		 * enough to rule out a cycle. */
		rtos_mutex_take(usbh_uac_play_mutex, RTOS_MAX_TIMEOUT);
		rtos_mutex_take(usbh_uac_record_mutex, RTOS_MAX_TIMEOUT);
		rtos_mutex_take(usbh_uac_ctrl_mutex, RTOS_MAX_TIMEOUT);
		/* Close the gate under the locks: no class API call is in flight here, and
		 * any later one gives up on the cleared flag. */
		usbh_uac_stack_ready = 0U;

		usbh_stop();
		usbh_uac_deinit();
		usbh_deinit();

		rtos_mutex_give(usbh_uac_ctrl_mutex);
		rtos_mutex_give(usbh_uac_record_mutex);
		rtos_mutex_give(usbh_uac_play_mutex);

		rtos_time_delay_ms(100);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

		/* Drain stale semaphore counts so play/record threads don't unblock
		 * with is_ready == 0 on the next plug-in. */
		while (rtos_sema_take(usbh_uac_ctx.play_start_sema, 0) == RTK_SUCCESS) {}
		while (rtos_sema_take(usbh_uac_ctx.record_start_sema, 0) == RTK_SUCCESS) {}

		rtos_mutex_take(usbh_uac_play_mutex, RTOS_MAX_TIMEOUT);
		rtos_mutex_take(usbh_uac_record_mutex, RTOS_MAX_TIMEOUT);
		rtos_mutex_take(usbh_uac_ctrl_mutex, RTOS_MAX_TIMEOUT);

		ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
		if (ret != HAL_OK) {
			rtos_mutex_give(usbh_uac_ctrl_mutex);
			rtos_mutex_give(usbh_uac_record_mutex);
			rtos_mutex_give(usbh_uac_play_mutex);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail %d\n", ret);
			break;
		}

		ret = usbh_uac_init(&usbh_uac_cfg, &uac_class_cfg);
		if (ret != HAL_OK) {
			usbh_deinit();
			rtos_mutex_give(usbh_uac_ctrl_mutex);
			rtos_mutex_give(usbh_uac_record_mutex);
			rtos_mutex_give(usbh_uac_play_mutex);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail %d\n", ret);
			break;
		}

		usbh_start();

		/* Open the gate as the very last step, so no worker runs against a half
		 * rebuilt stack. Clear teardown only after ready is set, or a worker could
		 * slip through seeing neither flag. */
		usbh_uac_stack_ready = 1U;
		usbh_uac_teardown = 0U;

		rtos_mutex_give(usbh_uac_ctrl_mutex);
		rtos_mutex_give(usbh_uac_record_mutex);
		rtos_mutex_give(usbh_uac_play_mutex);

		RTK_LOGS(TAG, RTK_LOG_INFO, "Reinit done\n");
	}

	/* A re-init failed, so the stack is gone for good. Keep every caller out and
	 * wake the workers so they observe the flag and leave their loops. The semas are
	 * left alive on purpose: a worker may still be about to take one, and freeing
	 * them here would pull them out from under it. */
	usbh_uac_stack_fatal = 1U;
	usbh_uac_stack_ready = 0U;
	usbh_uac_teardown = 1U;
	rtos_sema_give(usbh_uac_ctx.play_start_sema);
	rtos_sema_give(usbh_uac_ctx.record_start_sema);

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread exit\n");
	rtos_task_delete(NULL);
}
#endif /* USBH_UAC_HOT_PLUG_TEST */

/**
  * @brief  Bootstrap thread: creates synchronisation objects, spawns resident
  *         play and record workers, brings up the USB host and UAC class driver,
  *         and (optionally) starts the hot-plug worker.  Self-deletes on
  *         completion or failure.
  * @param  param: Unused
  */
static void example_usbh_uac_thread(void *param)
{
	int ret;
#if USBH_UAC_HOT_PLUG_TEST
	rtos_task_t hotplug_task;
#endif

	UNUSED(param);

	/* Create synchronisation primitives; free_sema_exit only deletes the
	   handles already created (the others are still NULL). */
	/* Created before the stack comes up and never deleted, see the declarations */
	if (usbh_uac_play_mutex == NULL) {
		ret = rtos_mutex_create(&usbh_uac_play_mutex);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create play mutex fail\n");
			goto free_sema_exit;
		}
	}
	if (usbh_uac_record_mutex == NULL) {
		ret = rtos_mutex_create(&usbh_uac_record_mutex);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create record mutex fail\n");
			goto free_sema_exit;
		}
	}
	if (usbh_uac_ctrl_mutex == NULL) {
		ret = rtos_mutex_create(&usbh_uac_ctrl_mutex);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create ctrl mutex fail\n");
			goto free_sema_exit;
		}
	}

	ret = rtos_sema_create(&usbh_uac_ctx.detach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create detach sema fail\n");
		goto free_sema_exit;
	}

	ret = rtos_sema_create(&usbh_uac_ctx.play_start_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create play sema fail\n");
		goto free_sema_exit;
	}

	ret = rtos_sema_create(&usbh_uac_ctx.record_start_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create record sema fail\n");
		goto free_sema_exit;
	}

	/* Create resident play thread. */
	ret = rtos_task_create(&usbh_uac_ctx.play.task, "usbh_uac_play_thread",
						   example_usbh_uac_play_thread, NULL,
						   USBH_UAC_PLAY_THREAD_STACK_SIZE, USBH_UAC_PLAY_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create play thread fail\n");
		goto free_sema_exit;
	}

	/* Create resident record thread. */
	ret = rtos_task_create(&usbh_uac_ctx.record.task, "usbh_uac_record_thread",
						   example_usbh_uac_record_thread, NULL,
						   USBH_UAC_RECORD_THREAD_STACK_SIZE, USBH_UAC_RECORD_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create record thread fail\n");
		goto delete_play_task_exit;
	}

	/* Initialise USB host. */
	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH init fail %d\n", ret);
		goto delete_record_task_exit;
	}

	/* Register UAC class driver. */
	ret = usbh_uac_init(&usbh_uac_cfg, &uac_class_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC init fail %d\n", ret);
		goto usb_deinit_exit;
	}

	/* Start USB TRX so enumeration can begin. */
	usbh_start();

	/* The stack is fully up: allow the workers and the shell to call the class. */
	usbh_uac_stack_ready = 1U;

#if USBH_UAC_HOT_PLUG_TEST
	ret = rtos_task_create(&hotplug_task, "usbh_uac_hotplug_thread",
						   example_usbh_uac_hotplug_thread, NULL,
						   USBH_UAC_HOTPLUG_THREAD_STACK_SIZE, USBH_UAC_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
		goto usbh_uac_deinit_exit;
	}
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* The USB OTG ISR is delivered on CPU0: the GIC pins every SPI to core 0
	   (ITARGETSR is written to 0x01010101 in arm_gic.c gic_dist_init). Pinning the
	   hotplug/deinit thread to CPU0 puts it on the same core as the ISR, so the
	   local interrupt disables inside the teardown are meaningful again. The play
	   and record workers stay unaffined: a pending teardown is excluded from them by
	   the per-direction mutexes, not by core. */
	rtos_task_set_affinity(hotplug_task, 0);
#endif
#endif

	goto example_exit;

#if USBH_UAC_HOT_PLUG_TEST
usbh_uac_deinit_exit:
	usbh_stop();
#endif
	/* The gate is already open and both workers exist, so hold it shut under all
	   three locks for this rollback too, exactly as the hotplug thread does. */
	usbh_uac_stack_fatal = 1U;
	usbh_uac_teardown = 1U;
	rtos_mutex_take(usbh_uac_play_mutex, RTOS_MAX_TIMEOUT);
	rtos_mutex_take(usbh_uac_record_mutex, RTOS_MAX_TIMEOUT);
	rtos_mutex_take(usbh_uac_ctrl_mutex, RTOS_MAX_TIMEOUT);
	usbh_uac_stack_ready = 0U;
	usbh_uac_deinit();
	usbh_deinit();
	rtos_mutex_give(usbh_uac_ctrl_mutex);
	rtos_mutex_give(usbh_uac_record_mutex);
	rtos_mutex_give(usbh_uac_play_mutex);
	goto delete_record_task_exit;

usb_deinit_exit:
	/* usbh_uac_init() failed, so only the host core is up and the gate was never
	   opened: no lock needed here. */
	usbh_deinit();

delete_record_task_exit:
	/* Keep the workers off a stack that is going away for good, whichever path
	   got here. */
	usbh_uac_stack_fatal = 1U;
	usbh_uac_stack_ready = 0U;
	usbh_uac_teardown = 1U;
	usbh_uac_ctx.play.thread_exit   = 1;
	usbh_uac_ctx.record.thread_exit = 1;
	/* Wake threads blocked on sema so they can observe the exit flag. */
	rtos_sema_give(usbh_uac_ctx.play_start_sema);
	rtos_sema_give(usbh_uac_ctx.record_start_sema);
	rtos_time_delay_ms(500);
	if (usbh_uac_ctx.record.task != NULL) {
		rtos_task_delete(usbh_uac_ctx.record.task);
		usbh_uac_ctx.record.task = NULL;
	}

delete_play_task_exit:
	if (usbh_uac_ctx.play.task != NULL) {
		rtos_task_delete(usbh_uac_ctx.play.task);
		usbh_uac_ctx.play.task = NULL;
	}

free_sema_exit:
	rtos_sema_delete(usbh_uac_ctx.detach_sema);
	rtos_sema_delete(usbh_uac_ctx.play_start_sema);
	rtos_sema_delete(usbh_uac_ctx.record_start_sema);
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH UAC demo aborted\n");

example_exit:
	rtos_task_delete(NULL);
}

/**
  * @brief  Parse a decimal shell argument and check its range.
  * @param  arg: Argument string.
  * @param  max: Largest accepted value.
  * @param  val: Output value, written only on success.
  * @retval HAL_OK when the whole string is a number in [0, max], HAL_ERR_PARA otherwise.
  */
static int uach_parse_arg(const u8 *arg, u32 max, u8 *val)
{
	char *end = NULL;
	u32 num;

	if ((arg == NULL) || (arg[0] == 0U)) {
		return HAL_ERR_PARA;
	}

	num = (u32)_strtoul((const char *)arg, &end, 10);
	if ((end == NULL) || (*end != '\0') || (num > max)) {
		return HAL_ERR_PARA;
	}

	*val = (u8)num;
	return HAL_OK;
}

static u32 uach_mute(u16 argc, u8 *argv[])
{
	int ret;
	u8 mute;

	if ((argc == 0U) || (uach_parse_arg(argv[0], 1U, &mute) != HAL_OK)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Usage: uach_mute 0|1\n");
		return HAL_ERR_PARA;
	}

	/* Shell context, i.e. a thread of its own: it has to be excluded from a
	   concurrent teardown exactly like the workers are. */
	if (usbh_uac_lock(usbh_uac_ctrl_mutex) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stack not ready\n");
		return HAL_ERR_HW;
	}
	ret = usbh_uac_set_mute(mute, USBH_UAC_ISOC_OUT_DIR);
	usbh_uac_unlock(usbh_uac_ctrl_mutex);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Set fail %d\n", ret);
		return HAL_ERR_HW;
	}
	return HAL_OK;
}

static u32 uach_vol(u16 argc, u8 *argv[])
{
	int ret;
	u8 vol;

	if ((argc == 0U) || (uach_parse_arg(argv[0], 100U, &vol) != HAL_OK)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Usage: uach_vol 0..100\n");
		return HAL_ERR_PARA;
	}
	example_usbh_uac_volume = vol;
	/* See uach_mute() for why the shell path needs the lock. */
	if (usbh_uac_lock(usbh_uac_ctrl_mutex) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stack not ready\n");
		return HAL_ERR_HW;
	}
	ret = usbh_uac_set_volume(example_usbh_uac_volume, USBH_UAC_ISOC_OUT_DIR);
	usbh_uac_unlock(usbh_uac_ctrl_mutex);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Set fail %d\n", ret);
		return HAL_ERR_HW;
	}
	return HAL_OK;
}

static u32 uach_volup(u16 argc, u8 *argv[])
{
	int ret;

	UNUSED(argc);
	UNUSED(argv);

	/* Saturate before adding so the u8 can not wrap */
	if (example_usbh_uac_volume > 95U) {
		example_usbh_uac_volume = 100U;
	} else {
		example_usbh_uac_volume += 5U;
	}
	/* See uach_mute() for why the shell path needs the lock. */
	if (usbh_uac_lock(usbh_uac_ctrl_mutex) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stack not ready\n");
		return HAL_ERR_HW;
	}
	ret = usbh_uac_set_volume(example_usbh_uac_volume, USBH_UAC_ISOC_OUT_DIR);
	usbh_uac_unlock(usbh_uac_ctrl_mutex);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Set fail %d\n", ret);
		return HAL_ERR_HW;
	}
	return HAL_OK;
}

static u32 uach_voldown(u16 argc, u8 *argv[])
{
	int ret;

	UNUSED(argc);
	UNUSED(argv);

	if (example_usbh_uac_volume < 5U) {
		example_usbh_uac_volume = 0U;
	} else {
		example_usbh_uac_volume -= 5U;
	}
	/* See uach_mute() for why the shell path needs the lock. */
	if (usbh_uac_lock(usbh_uac_ctrl_mutex) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stack not ready\n");
		return HAL_ERR_HW;
	}
	ret = usbh_uac_set_volume(example_usbh_uac_volume, USBH_UAC_ISOC_OUT_DIR);
	usbh_uac_unlock(usbh_uac_ctrl_mutex);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Set fail %d\n", ret);
		return HAL_ERR_HW;
	}
	return HAL_OK;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Entry point for the USB host UAC demo.
  *         Spawns the bootstrap thread and returns immediately.
  *         Safe to call once during application startup.
  */
void example_usbh_uac(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH UAC demo start\n");

	ret = rtos_task_create(&task, "usbh_uac_thread",
						   example_usbh_uac_thread, NULL,
						   USBH_UAC_INIT_THREAD_STACK_SIZE, USBH_UAC_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBH UAC thread fail\n");
	}
}

