/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

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
#define USBH_UAC_MAIN_TASK_STACK_SIZE           768U
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
static u32 usbh_uac_data_len = 1200;
static unsigned char usbh_uac_audio_data[1200] = {0,};
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
	rtos_sema_t detach_sema;        /*!< Given by cb_detach; taken by hotplug thread */
	rtos_sema_t play_start_sema;    /*!< Given when setup completes; wakes play thread */
	rtos_sema_t record_start_sema;  /*!< Given when setup completes; wakes record thread */

#if USBH_UAC_HOT_PLUG_TEST
	rtos_task_t hotplug_task;       /*!< Hot-plug monitoring thread handle */
#endif

	__IO u8 is_ready;               /*!< Device enumerated and configured */
} uac_ctx_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int usbh_uac_cb_init(void);
static int usbh_uac_cb_deinit(void);
static int usbh_uac_cb_attach(void);
static int usbh_uac_cb_detach(void);
static int usbh_uac_cb_setup(void);
static int usbh_uac_cb_isoc_transmitted(usbh_urb_state_t state);
static int usbh_uac_cb_process(usb_host_t *host, u8 msg);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "UAC";

/** Single application-level context instance. */
static uac_ctx_t usbh_uac_ctx;

/** Record capture destination buffer. */
static u8 usbh_uac_record_buffer[USBH_UAC_RECORD_BUFFER_SIZE];

static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_FULL,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = USBH_UAC_MAIN_TASK_STACK_SIZE,
	.main_task_priority = USBH_UAC_MAIN_TASK_PRIORITY,
	.tick_source = USBH_SOF_TICK,
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

static const usbh_uac_cb_t usbh_uac_cfg = {
	.init             = usbh_uac_cb_init,
	.deinit           = usbh_uac_cb_deinit,
	.attach           = usbh_uac_cb_attach,
	.detach           = usbh_uac_cb_detach,
	.setup            = usbh_uac_cb_setup,
	.isoc_transmitted = usbh_uac_cb_isoc_transmitted,

	/* Ring-buffer depth (frames) for OUT playback and IN record paths.
	 * usbh_uac_init() rejects the cb when both are 0. */
	.isoc_out_frm_cnt = USBH_UAC_FRAME_CNT,
	.isoc_in_frm_cnt  = USBH_UAC_FRAME_CNT,
};

static const usbh_user_cb_t usbh_usr_cb = {
	.process = usbh_uac_cb_process
};

/* Private functions ---------------------------------------------------------*/

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
  * @retval HAL_OK
  */
static int usbh_uac_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
	return HAL_OK;
}

/**
  * @brief  UAC attach callback - device plugged in and enumerated.
  * @retval HAL_OK
  */
static int usbh_uac_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	return HAL_OK;
}

/**
  * @brief  UAC detach callback - device removed.
  *         Clears is_ready, stops audio pipelines, signals hotplug thread.
  * @retval HAL_OK
  */
static int usbh_uac_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH play=%u rec=%u\n",
			 usbh_uac_ctx.play.count, usbh_uac_ctx.record.count);

	usbh_uac_ctx.is_ready = 0;
	usbh_uac_stop_play();
	usbh_uac_stop_capture();

#if USBH_UAC_HOT_PLUG_TEST
	rtos_sema_give(usbh_uac_ctx.detach_sema);
#endif
	return HAL_OK;
}

/**
  * @brief  UAC setup callback - device fully configured and ready.
  *         Marks is_ready and wakes play + record worker threads.
  * @retval HAL_OK
  */
static int usbh_uac_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	usbh_uac_ctx.is_ready = 1;
	rtos_sema_give(usbh_uac_ctx.play_start_sema);
	rtos_sema_give(usbh_uac_ctx.record_start_sema);
	return HAL_OK;
}

/**
  * @brief  ISOC OUT (playback) transfer complete callback.
  * @param  state: URB completion state
  * @retval HAL_OK
  */
static int usbh_uac_cb_isoc_transmitted(usbh_urb_state_t state)
{
	if (state == USBH_URB_DONE) {
		/* TX ok */
	} else if (state == USBH_URB_BUSY) {
		usbh_uac_ctx.play.err_count++;
	} else {
		usbh_uac_ctx.play.err_count++;
	}
	return HAL_OK;
}

/**
  * @brief  USB host top-level event callback (connect / disconnect / probe fail).
  * @param  host: USB host handle (unused)
  * @param  msg:  Event identifier
  * @retval HAL_OK
  */
static int usbh_uac_cb_process(usb_host_t *host, u8 msg)
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
	return HAL_OK;
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
	u32 ret;
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

		if (usbh_uac_support_playback() == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Play not support\n");
			continue;
		}

		/*
		 * PHASE 2: AUDIO FORMAT SELECTION
		 */
		fmt_info = usbh_uac_get_alt_setting(USBH_UAC_ISOC_OUT_DIR, &fmt_cnt);

		if ((fmt_info == NULL) || (fmt_cnt == 0)) {
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
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Play fmt mismatch\n");
			continue;
		}

		if (usbh_uac_set_alt_setting(USBH_UAC_ISOC_OUT_DIR,
									 USBH_UAC_PLAY_CHANNELS,
									 USBH_UAC_PLAY_BITWIDTH,
									 USBH_UAC_PLAY_SAMPLING_FREQ) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Play set alt fail\n");
			continue;
		}

		frame_size = usbh_uac_get_frame_size(USBH_UAC_ISOC_OUT_DIR);
		rtos_time_delay_ms(100);

		RTK_LOGS(TAG, RTK_LOG_INFO, "Play start, data=%d frame=%d test_cnt=%d\n",
				 audio_total_data_len, frame_size, USBH_UAC_TEST_CNT);

#if USBH_UAC_XFER_CHECK
		memset(usbh_uac_audio_data, USBH_UAC_OUT_DATA, usbh_uac_data_len);
#endif

		/*
		 * PHASE 3: CONTINUOUS PLAYBACK
		 */
		if (usbh_uac_ctx.is_ready != 0) {
			usbh_uac_start_play();
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
				TRNG_get_random_bytes(&send_len, 2);
				send_len = send_len & 0x0FFFU;

				if (send_len == 0) {
					continue;
				}

				if (offset + send_len > total_len) {
					send_len = total_len - offset;
				}

				ret = usbh_uac_write((u8 *)(usbh_uac_audio_data_handle + offset), send_len, 10);

				if (ret != send_len) {
					usbh_uac_ctx.play.err_count++;
					if (usbh_uac_ctx.is_ready == 0 || usbh_uac_ctx.play.err_count > 100) {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Play err %d\n",
								 usbh_uac_ctx.play.err_count);
						goto play_stop;
					}
					rtos_time_delay_ms(2);
					continue;
				}

				offset += ret;
				usbh_uac_ctx.play.err_count = 0;
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
			usbh_uac_stop_play();
			is_playing = 0;
			rtos_time_delay_ms(50);
		}
	}

	if (is_playing != 0) {
		usbh_uac_stop_play();
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
	u32 ret;
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

		if (usbh_uac_support_record() == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Rec not support\n");
			continue;
		}

		fmt_info = usbh_uac_get_alt_setting(USBH_UAC_ISOC_IN_DIR, &fmt_cnt);

		if ((fmt_info == NULL) || (fmt_cnt == 0)) {
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
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Rec set alt fail\n");
			continue;
		}

		frame_size = usbh_uac_get_frame_size(USBH_UAC_ISOC_IN_DIR);
		rtos_time_delay_ms(100);

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
		g_track_rate    = fmt_info[0].sampling_freq;
		g_track_channel = fmt_info[0].ch_cnt;
		g_track_format  = fmt_info[0].bit_width;

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
			track_buf_size = (int32_t)(g_track_rate * g_track_format / 8
									   * g_track_channel / 1000 * 100);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Rec track buf resize %d\n", track_buf_size);
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Rec track buf %d\n", track_buf_size);
		}

		{
			AudioTrackConfig track_config;
			track_config.category_type  = AUDIO_CATEGORY_MEDIA;
			track_config.sample_rate    = g_track_rate;
			track_config.format         = format;
			track_config.channel_count  = g_track_channel;
			track_config.buffer_bytes   = track_buf_size;
			AudioTrack_Init(audio_track, &track_config, AUDIO_OUTPUT_FLAG_NONE);
		}

		AudioTrack_SetVolume(audio_track, 1.0, 1.0);
		AudioTrack_SetStartThresholdBytes(audio_track, track_buf_size);

		if (AudioTrack_Start(audio_track) != AUDIO_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Rec track start fail\n");
			AudioTrack_Destroy(audio_track);
			audio_track = NULL;
			continue;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Rec track started\n");
#endif

		RTK_LOGS(TAG, RTK_LOG_INFO, "Rec start, frame=%d\n", frame_size);

		if (usbh_uac_ctx.is_ready != 0) {
			usbh_uac_start_capture();
			is_recording = 1;
			record_loop_count = 0;
			total_read = 0;
		}

		while (usbh_uac_ctx.is_ready != 0 && usbh_uac_ctx.record.thread_exit == 0) {
			read_len = USBH_UAC_RECORD_BUFFER_SIZE;
			ret = usbh_uac_read(usbh_uac_record_buffer, read_len, 1000);

			if (ret > 0) {
				record_loop_count++;
				usbh_uac_ctx.record.count++;
				total_read += ret;

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
				if (audio_track != NULL) {
					AudioTrack_Write(audio_track, (u8 *)usbh_uac_record_buffer, ret, true);
				}
#endif
				if ((usb_os_get_timestamp_ms() - debug_time) >= USBH_UAC_DEBUG_TRACE_STEP) {
					debug_time = usb_os_get_timestamp_ms();
					RTK_LOGS(TAG, RTK_LOG_INFO, "Rec status loop=%d bytes=%d err=%d\n",
							 record_loop_count, total_read, usbh_uac_ctx.record.err_count);
				}
			} else if (ret == 0) {
				/* timeout / no data - nothing to report */
			} else {
				usbh_uac_ctx.record.err_count++;
				if (usbh_uac_ctx.record.err_count > 100) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Rec err %d\n", usbh_uac_ctx.record.err_count);
					goto record_stop;
				}
			}
		}

record_stop:
		if (is_recording != 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Rec stop loops=%d bytes=%d\n",
					 record_loop_count, total_read);
			usbh_uac_stop_capture();
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
	}

	if (is_recording != 0) {
		usbh_uac_stop_capture();
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

		usbh_stop();
		usbh_uac_deinit();
		usbh_deinit();

		rtos_time_delay_ms(100);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

		/* Drain stale semaphore counts so play/record threads don't unblock
		 * with is_ready == 0 on the next plug-in. */
		while (rtos_sema_take(usbh_uac_ctx.play_start_sema, 0) == RTK_SUCCESS) {}
		while (rtos_sema_take(usbh_uac_ctx.record_start_sema, 0) == RTK_SUCCESS) {}

		ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "USB re-init fail %d\n", ret);
			break;
		}

		ret = usbh_uac_init(&usbh_uac_cfg);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC re-init fail %d\n", ret);
			usbh_deinit();
			break;
		}

		usbh_start();
		RTK_LOGS(TAG, RTK_LOG_INFO, "Re-init done\n");
	}

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

	/* Create synchronisation primitives; cascade failures. */
	if (rtos_sema_create(&usbh_uac_ctx.detach_sema, 0U, 1U) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create detach sema fail\n");
		goto example_exit;
	}
	if (rtos_sema_create(&usbh_uac_ctx.play_start_sema, 0U, 1U) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create play sema fail\n");
		rtos_sema_delete(usbh_uac_ctx.detach_sema);
		goto example_exit;
	}
	if (rtos_sema_create(&usbh_uac_ctx.record_start_sema, 0U, 1U) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create record sema fail\n");
		rtos_sema_delete(usbh_uac_ctx.detach_sema);
		rtos_sema_delete(usbh_uac_ctx.play_start_sema);
		goto example_exit;
	}

	/* Create resident play thread. */
	ret = rtos_task_create(&usbh_uac_ctx.play.task, "example_usbh_uac_play_thread",
						   example_usbh_uac_play_thread, NULL,
						   USBH_UAC_PLAY_THREAD_STACK_SIZE, USBH_UAC_PLAY_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create play thread fail\n");
		goto free_sema_exit;
	}

	/* Create resident record thread. */
	ret = rtos_task_create(&usbh_uac_ctx.record.task, "example_usbh_uac_record_thread",
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
	ret = usbh_uac_init(&usbh_uac_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC init fail %d\n", ret);
		goto usb_deinit_exit;
	}

	/* Start USB TRX so enumeration can begin. */
	usbh_start();

#if USBH_UAC_HOT_PLUG_TEST
	ret = rtos_task_create(&hotplug_task, "example_usbh_uac_hotplug_thread",
						   example_usbh_uac_hotplug_thread, NULL,
						   USBH_UAC_HOTPLUG_THREAD_STACK_SIZE, USBH_UAC_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
		goto usbh_uac_deinit_exit;
	}
#endif

	goto example_exit;

#if USBH_UAC_HOT_PLUG_TEST
usbh_uac_deinit_exit:
	usbh_stop();
#endif
	usbh_uac_deinit();

usb_deinit_exit:
	usbh_deinit();

delete_record_task_exit:
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
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH UAC demo stop\n");

example_exit:
	rtos_task_delete(NULL);
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

	ret = rtos_task_create(&task, "example_usbh_uac_thread",
						   example_usbh_uac_thread, NULL,
						   USBH_UAC_INIT_THREAD_STACK_SIZE, USBH_UAC_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}
}

/* Shell test commands ------------------------------------------------------- */

/** Example-layer playback volume tracker. */
static u8 example_usbh_uac_volume = 50;

static u32 uach_mute(u16 argc, u8 *argv[])
{
	u8 mute;

	if (argc == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid argument\n");
		return HAL_ERR_PARA;
	}

	mute = 1;
	if (argv[0] != NULL) {
		mute = (u8)_strtoul((const char *)(argv[0]), (char **)NULL, 10);
	}

	usbh_uac_set_mute(mute, USBH_UAC_ISOC_OUT_DIR);
	return HAL_OK;
}

static u32 uach_vol(u16 argc, u8 *argv[])
{
	u8 vol;

	if (argc == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid argument\n");
		return HAL_ERR_PARA;
	}

	vol = 50;
	if (argv[0] != NULL) {
		vol = (u8)_strtoul((const char *)(argv[0]), (char **)NULL, 10);
	}
	example_usbh_uac_volume = vol;
	usbh_uac_set_volume(example_usbh_uac_volume, USBH_UAC_ISOC_OUT_DIR);
	return HAL_OK;
}

static u32 uach_volup(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	example_usbh_uac_volume += 5U;
	if (example_usbh_uac_volume >= 100U) {
		example_usbh_uac_volume = 100U;
	}
	usbh_uac_set_volume(example_usbh_uac_volume, USBH_UAC_ISOC_OUT_DIR);
	return HAL_OK;
}

static u32 uach_voldown(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	if (example_usbh_uac_volume < 5U) {
		example_usbh_uac_volume = 0U;
	} else {
		example_usbh_uac_volume -= 5U;
	}
	usbh_uac_set_volume(example_usbh_uac_volume, USBH_UAC_ISOC_OUT_DIR);
	return HAL_OK;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbh_uac_test_md_table[] = {
	{"uach_mute",    uach_mute},
	{"uach_vol",     uach_vol},
	{"uach_volup",   uach_volup},
	{"uach_voldown", uach_voldown},
};
