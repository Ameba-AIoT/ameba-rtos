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
#include "usbh_composite_hid_uac.h"

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
#define USBH_UAC_HOT_PLUG_TEST              1

#define USBH_UAC_VOLUME_STEP                20

/**
 * @brief Statistics print interval in milliseconds
 * Play/record threads log their loop counters once per this period
 */
#define USBH_UAC_DEBUG_TRACE_STEP           1000

/**
 * @brief Number of frames for ISOC transfer buffer
 * Affects latency and buffer size for audio streaming
 */
#define USBH_UAC_FRAME_CNT                  20

/**
 * @brief Record buffer size for audio capture
 */
#define USBH_UAC_RECORD_BUFFER_SIZE         1024

/*
	Audio format configuration - Playback
	The PCM data obtained from the header file has the following format.
	It is necessary to ensure that the current UAC device supports this format; otherwise, it will result in no sound.
*/
#define USBH_UAC_PLAY_CHANNELS              2       /* Stereo audio */
#define USBH_UAC_PLAY_BITWIDTH              16      /* 16-bit audio samples */
#define USBH_UAC_PLAY_SAMPLING_FREQ         48000   /* 48kHz sample rate */

/* Thread priority definitions (higher number = higher priority) */
#define USBH_UAC_PLAY_THREAD_PRIORITY       4       /* Audio playback thread */
#define USBH_UAC_RECORD_THREAD_PRIORITY     4       /* Audio record thread */
#define USBH_UAC_MAIN_THREAD_PRIORITY       5       /* USB host main thread */
#define USBH_UAC_HOTPLUG_THREAD_PRIORITY    6       /* Hot-plug detection thread */

/**
 * @brief Enable data validation check
 * When enabled, fills audio buffer with test pattern for debugging
 */
#define USBH_UAC_XFER_CHECK                 0

#if USBH_UAC_XFER_CHECK
#define USBH_UAC_OUT_DATA                   0x88
static u32 usbh_uac_audio_data_2ch_len = 1200;
static unsigned char usbh_uac_audio_data_2ch[1200] = {0,};
#else
#include "example_usbh_composite_hid_uac_audio_data.h"
#endif

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int usbh_hid_cb_report(usbh_composite_hid_event_t *event);
static int usbh_uac_cb_init(void);
static int usbh_uac_cb_deinit(void);
static int usbh_uac_cb_attach(void);
static int usbh_uac_cb_detach(void);
static int usbh_uac_cb_setup(void);
static int usbh_uac_cb_isoc_transmitted(usbh_urb_state_t state);
static int usbh_uac_cb_process(usb_host_t *host, u8 msg);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "COMP";

/**
 * @brief Semaphore signaled when device is detached
 * Used to trigger hot-plug reinitialization sequence
 */
static rtos_sema_t usbh_uac_detach_sema;

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
 * @brief Flag indicating device is ready for audio transfer
 * Set after successful device enumeration and configuration
 * Reset when device is removed
 */
static __IO int usbh_uac_is_ready = 0;

/**
 * @brief Flag to request playback/record threads to exit completely
 * Only set when application is shutting down
 */
static __IO int usbh_uac_play_thread_exit = 0;

/**
 * @brief Flag to request record thread to exit completely
 * Only set when application is shutting down
 */
static __IO int usbh_uac_record_thread_exit = 0;

/**
 * @brief Global record buffer for audio capture
 */
static u8 usbh_uac_record_buffer[USBH_UAC_RECORD_BUFFER_SIZE];

/**
 * @brief Count of URB busy responses
 * Indicates USB bandwidth congestion
 */
static int usbh_uac_busy_count;

/**
 * @brief Count of transfer errors
 * Used to detect device disconnection
 */
static int usbh_uac_err_count;

/**
 * @brief Total number of complete audio playback loops
 * Accumulated across all connections for statistics
 */
static u32 usbh_uac_play_count = 0;

/**
 * @brief Total number of complete audio record loops
 * Accumulated across all connections for statistics
 */
static u32 usbh_uac_record_count = 0;

/**
 * @brief Handle for audio playback thread (resident)
 * Created once at startup, never deleted
 */
static rtos_task_t usbh_uac_play_task = NULL;

/**
 * @brief Handle for audio record thread (resident)
 * Created once at startup, never deleted
 */
static rtos_task_t usbh_uac_record_task = NULL;

/**
 * @brief Handle for hot-plug monitoring thread
 * Remains active for entire application lifetime
 */
#if USBH_UAC_HOT_PLUG_TEST
static rtos_task_t usbh_uac_hotplug_task = NULL;
#endif

/* Audio control direction: USBH_UAC_ISOC_OUT_DIR for playback, USBH_UAC_ISOC_IN_DIR for record.
   Set by the "init" CLI command; applies to subsequent volume and mute operations. */
static u8 uac_ctrl_dir = USBH_UAC_ISOC_OUT_DIR;
static u8 cur_playback_volume = 50;
static u8 cur_record_volume = 50;

/**
 * @brief USB host controller configuration
 * Defines hardware parameters and priorities
 */
static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_FULL,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = 900U,
	.main_task_priority = USBH_UAC_MAIN_THREAD_PRIORITY,
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

/**
 * @brief UAC (USB Audio Class) callback configuration
 */
static usbh_composite_uac_usr_cb_t usbh_uac_cfg = {
	.init = usbh_uac_cb_init,
	.deinit = usbh_uac_cb_deinit,
	.attach = usbh_uac_cb_attach,
	.detach = usbh_uac_cb_detach,
	.setup = usbh_uac_cb_setup,
	.isoc_transmitted = usbh_uac_cb_isoc_transmitted,

	.isoc_in_frm_cnt = USBH_UAC_FRAME_CNT,
	.isoc_out_frm_cnt = USBH_UAC_FRAME_CNT,
};

/**
 * @brief HID (Human Interface Device) callback configuration
 */
static usbh_composite_hid_usr_cb_t usbh_hid_cfg = {
	.report = usbh_hid_cb_report,
};

/**
 * @brief General USB host event callbacks
 */
static usbh_user_cb_t usbh_usr_cb = {
	.process = usbh_uac_cb_process
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  HID report callback, dispatched on each consumer-control event
  *         received from the composite HID interface (volume / mute / play).
  * @param  event: Pointer to the HID event descriptor reported by the stack.
  * @retval Status
  */
static int usbh_hid_cb_report(usbh_composite_hid_event_t *event)
{
	if (!event) {
		return HAL_OK;
	}

	switch (event->type) {
	case VOLUME_EVENT_CONSUMER_UP:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Volume Up\n");
		break;
	case VOLUME_EVENT_CONSUMER_DOWN:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Volume Down\n");
		break;
	case VOLUME_EVENT_CONSUMER_MUTE:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Mute\n");
		break;
	case VOLUME_EVENT_CONSUMER_PLAY_PAUSE:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Play/Pause\n");
		break;
	case VOLUME_EVENT_CONSUMER_STOP:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Stop\n");
		break;
	default:
		break;
	}

	return HAL_OK;
}

/**
  * @brief  UAC class init callback, invoked once when the composite UAC
  *         class driver is registered to the USB host stack.
  * @retval Status
  */
static int usbh_uac_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Init\n");
	return HAL_OK;
}

/**
  * @brief  UAC class deinit callback, invoked when the class driver is
  *         unregistered (typically on `usbh_composite_deinit`).
  * @retval Status
  */
static int usbh_uac_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Deinit\n");
	return HAL_OK;
}

/**
  * @brief  UAC attach callback, fired when a UAC-capable device is plugged
  *         in. Resets the per-connection statistics counters.
  * @retval Status
  */
static int usbh_uac_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Attach\n");

	/* Reset play counter for new connection */
	usbh_uac_play_count = 0;
	usbh_uac_record_count = 0;

	return HAL_OK;
}

/**
  * @brief  UAC detach callback, fired when the device is unplugged. Stops
  *         playback / capture pipelines and (in hot-plug test mode) signals
  *         the hot-plug worker to re-initialize the host stack.
  * @retval Status
  */
static int usbh_uac_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Detached, played: %d, recorded: %d\n",
			 usbh_uac_play_count, usbh_uac_record_count);

	/* Clear ready flag - this will stop playback loop */
	usbh_uac_is_ready = 0;

	/* Immediately stop audio/record to prevent noise */
	usbh_composite_uac_stop_play();
	usbh_composite_uac_stop_capture();

#if USBH_UAC_HOT_PLUG_TEST
	/* Signal hot-plug thread to reinitialize */
	rtos_sema_give(usbh_uac_detach_sema);
#endif

	return HAL_OK;
}

/**
  * @brief  UAC setup callback, fired after device enumeration and class
  *         setup completes. Marks the device ready and wakes the playback
  *         and record worker threads.
  * @retval Status
  */
static int usbh_uac_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");

	/* Mark device as ready */
	usbh_uac_is_ready = 1;

	/* Signal playback thread that setup is complete */
	rtos_sema_give(usbh_uac_play_start_sema);
	rtos_sema_give(usbh_uac_record_start_sema);

	return HAL_OK;
}

/**
  * @brief  UAC isochronous transfer completion callback. Updates the busy /
  *         error counters used to monitor USB bandwidth and link health.
  * @param  state: Final URB state reported by the controller.
  * @retval Status
  */
static int usbh_uac_cb_isoc_transmitted(usbh_urb_state_t state)
{
	if (state == USBH_URB_DONE) {
		/* Transfer successful */
	} else if (state == USBH_URB_BUSY) {
		/* USB bandwidth congestion */
		usbh_uac_busy_count++;
	} else {
		/* Transfer error */
		usbh_uac_err_count++;
	}

	return HAL_OK;
}

/**
  * @brief  Generic USB host process callback, used to react to top-level
  *         events such as connect / disconnect / probe failure.
  * @param  host: USB host handle (unused).
  * @param  msg:  Event identifier dispatched by the host core.
  * @retval Status
  */
static int usbh_uac_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);

	switch (msg) {
	case USBH_MSG_DISCONNECTED:
		usbh_uac_is_ready = 0;
		break;

	case USBH_MSG_CONNECTED:
		break;

	case USBH_MSG_PROBE_FAIL:
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Probe Fail\n");
		usbh_uac_is_ready = 0;
		break;

	default:
		break;
	}

	return HAL_OK;
}

/**
  * @brief  Audio playback worker thread.
  *
  *         Resident thread that lives for the entire demo. Each iteration:
  *           1. Blocks on `usbh_uac_play_start_sema` until a device is set up.
  *           2. Queries and prints the OUT alt-settings advertised by the device.
  *           3. Selects the demo playback format (channels / bit-width / rate).
  *           4. Streams the embedded PCM buffer in randomly-sized chunks until
  *              the device disappears or the demo is shutting down.
  *           5. Stops the OUT pipeline and goes back to step 1.
  * @param  param: Unused.
  * @retval None
  */
static void usbh_uac_play_thread(void *param)
{
	const unsigned char *usbh_uac_audio_data_2ch_handle = usbh_uac_audio_data_2ch;
	const usbh_audio_fmt_t *fmt_info = NULL;
	const usbh_audio_fmt_t *audio_fmt = NULL;
	u8 *buffer = NULL;
	u32 audio_total_data_len = usbh_uac_audio_data_2ch_len;
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

	while (!usbh_uac_play_thread_exit) {
		/*
		 * PHASE 1: WAIT FOR DEVICE
		 **/
		RTK_LOGS(TAG, RTK_LOG_INFO, "Play wait setup\n");

		/* Block until device is attached */
		if (rtos_sema_take(usbh_uac_play_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}

		/* Check if thread should exit */
		if (usbh_uac_play_thread_exit) {
			break;
		}

		/* Check if device was removed during setup */
		if (!usbh_uac_is_ready) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Play not ready\n");
			continue;
		}

		if (!usbh_composite_uac_support_playback()) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Play not support\n");
			continue;
		}

		/*
		 * PHASE 2: AUDIO FORMAT CONFIGURATION
		 **/

		/* Query available audio formats from device */
		fmt_info = usbh_composite_uac_get_alt_setting(USBH_UAC_ISOC_OUT_DIR, &fmt_cnt);

		if ((fmt_info == NULL) || (fmt_cnt == 0)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Play get fmt fail\n");
			continue;
		}

		/* Reset per-connection flag before scanning */
		playback_format_find = 0;

		/* Log all available formats */
		RTK_LOGS(TAG, RTK_LOG_INFO, "Play fmt list:\n");
		for (i = 0; i < fmt_cnt; i++) {
			audio_fmt = &(fmt_info[i]);
			RTK_LOGS(TAG, RTK_LOG_INFO, "[%d] CH=%d BW=%d FREQ=%d\n",
					 i, audio_fmt->ch_cnt, audio_fmt->bit_width, audio_fmt->sampling_freq);

			if ((playback_format_find == 0)
				&& (audio_fmt->sampling_freq == USBH_UAC_PLAY_SAMPLING_FREQ)
				&& (audio_fmt->bit_width == USBH_UAC_PLAY_BITWIDTH)
				&& (audio_fmt->ch_cnt == USBH_UAC_PLAY_CHANNELS)) {
				playback_format_find = 1;
			}
		}

		if (playback_format_find == 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Play fmt mismatch\n");
			continue;
		}

		/* Select matching audio format */
		if (usbh_composite_uac_set_alt_setting(USBH_UAC_ISOC_OUT_DIR, USBH_UAC_PLAY_CHANNELS,
											   USBH_UAC_PLAY_BITWIDTH, USBH_UAC_PLAY_SAMPLING_FREQ) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Play set alt fail\n");
			continue;
		}

		/* Get frame size for selected format */
		frame_size = usbh_composite_uac_get_frame_size(USBH_UAC_ISOC_OUT_DIR);
		rtos_time_delay_ms(100);

		RTK_LOGS(TAG, RTK_LOG_INFO, "Play start, data=%d frame=%d\n", audio_total_data_len, frame_size);

#if USBH_UAC_XFER_CHECK
		memset(usbh_uac_audio_data_2ch, 0x00, usbh_uac_audio_data_2ch_len);
#endif

		/*
		 * PHASE 3: START AUDIO PLAYBACK
		 **/

		/* Start the audio stream */
		if (usbh_uac_is_ready) {
			usbh_composite_uac_start_play();
			is_playing = 1;
			play_loop_count = 0;
		}

		total_len = audio_total_data_len;

		/*
		 * PHASE 4: CONTINUOUS PLAYBACK LOOP
		 **/
		while (usbh_uac_is_ready && !usbh_uac_play_thread_exit) {
			/* Reset for new loop */
			offset = 0;
			play_loop_count++;
			usbh_uac_play_count++;
			usbh_uac_err_count = 0;

			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Play loop %d total %d\n", play_loop_count, usbh_uac_play_count);

			/*
			 * SEND AUDIO DATA
			 **/

			while (offset < total_len && usbh_uac_is_ready && !usbh_uac_play_thread_exit) {
				/* Generate random transfer length for testing */
				TRNG_get_random_bytes(&send_len, 1);
				send_len = send_len & 0xFFFF;

				if (send_len == 0) {
					continue;
				}

				/* Clamp to remaining data */
				if (offset + send_len > total_len) {
					send_len = total_len - offset;
				}

				buffer = (u8 *)(usbh_uac_audio_data_2ch_handle + offset);

				/* Send audio data with timeout */
				ret = usbh_composite_uac_write(buffer, send_len, 10);

				if (ret != send_len) {
					usbh_uac_err_count++;

					if (!usbh_uac_is_ready || usbh_uac_err_count > 100) {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Play err %d\n", usbh_uac_err_count);
						goto play_stop;
					}
					continue;
				}

				offset += ret;
			}

			/* Periodic statistics output */
			if ((usb_os_get_timestamp_ms() - debug_time) >= USBH_UAC_DEBUG_TRACE_STEP) {
				debug_time = usb_os_get_timestamp_ms();
				RTK_LOGS(TAG, RTK_LOG_INFO, "Play status loop=%d err=%d busy=%d\n",
						 play_loop_count, usbh_uac_err_count, usbh_uac_busy_count);
			}
		}

		/*
		 * PHASE 5: STOP PLAYBACK AND CLEANUP FOR THIS SESSION
		 **/
play_stop:
		if (is_playing) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Play stop, loops=%d\n", play_loop_count);
			usbh_composite_uac_stop_play();
			is_playing = 0;
			rtos_time_delay_ms(50);
		}
	}

	/* Force stop if somehow still playing */
	if (is_playing) {
		usbh_composite_uac_stop_play();
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
	const usbh_audio_fmt_t *fmt_info = NULL;
	const usbh_audio_fmt_t *audio_fmt = NULL;
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

	while (!usbh_uac_record_thread_exit) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Rec wait setup\n");

		if (rtos_sema_take(usbh_uac_record_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}

		if (usbh_uac_record_thread_exit) {
			break;
		}

		if (!usbh_uac_is_ready) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Rec not ready\n");
			continue;
		}

		if (!usbh_composite_uac_support_record()) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Rec not support\n");
			continue;
		}

		fmt_info = usbh_composite_uac_get_alt_setting(USBH_UAC_ISOC_IN_DIR, &fmt_cnt);

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

		if (usbh_composite_uac_set_alt_setting(USBH_UAC_ISOC_IN_DIR, fmt_info[0].ch_cnt,
											   fmt_info[0].bit_width, fmt_info[0].sampling_freq) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Rec set alt fail\n");
			continue;
		}

		frame_size = usbh_composite_uac_get_frame_size(USBH_UAC_ISOC_IN_DIR);
		rtos_time_delay_ms(100);

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
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
		if (!audio_track) {
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

		AudioTrackConfig track_config;
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
#endif

		RTK_LOGS(TAG, RTK_LOG_INFO, "Rec start, frame=%d\n", frame_size);

		if (usbh_uac_is_ready) {
			usbh_composite_uac_start_capture();
			is_recording = 1;
			record_loop_count = 0;
			total_read = 0;
		}

		while (usbh_uac_is_ready && !usbh_uac_record_thread_exit) {
			read_len = USBH_UAC_RECORD_BUFFER_SIZE;
			ret = usbh_composite_uac_read(usbh_uac_record_buffer, read_len, 1000);

			if (ret > 0) {
				record_loop_count++;
				usbh_uac_record_count++;
				total_read += ret;

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
				if (audio_track) {
					AudioTrack_Write(audio_track, (u8 *)usbh_uac_record_buffer, ret, true);
				}
#endif

				/* Periodic statistics output (only after at least one successful read) */
				if ((usb_os_get_timestamp_ms() - debug_time) >= USBH_UAC_DEBUG_TRACE_STEP) {
					debug_time = usb_os_get_timestamp_ms();
					RTK_LOGS(TAG, RTK_LOG_INFO, "Rec status loop=%d bytes=%d err=%d\n",
							 record_loop_count, total_read, usbh_uac_err_count);
				}
			} else if (ret == 0) {
				/* timeout / no data, nothing to report */
			} else {
				usbh_uac_err_count++;
				if (usbh_uac_err_count > 100) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Rec err %d\n", usbh_uac_err_count);
					goto record_stop;
				}
			}
		}

record_stop:
		if (is_recording) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Rec stop, loops=%d bytes=%d\n",
					 record_loop_count, total_read);
			usbh_composite_uac_stop_capture();
			is_recording = 0;
			rtos_time_delay_ms(50);
		}

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
		if (audio_track) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Rec track stop\n");
			AudioTrack_Pause(audio_track);
			AudioTrack_Flush(audio_track);
			AudioTrack_Stop(audio_track);
			AudioTrack_Destroy(audio_track);
			audio_track = NULL;
		}
#endif
	}

	if (is_recording) {
		usbh_composite_uac_stop_capture();
	}

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
	if (audio_track) {
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

#if USBH_UAC_HOT_PLUG_TEST
/**
  * @brief  Hot-plug worker thread, used for memory-leak / re-enumeration
  *         soak testing.
  *
  *         Sleeps on `usbh_uac_detach_sema`. On each detach event it tears
  *         down the composite class driver and the host core, drains any
  *         pending start semaphores so the play / record threads do not
  *         re-enter with stale state, then re-initializes the host stack so
  *         a freshly plugged device can be re-enumerated.
  * @param  param: Unused.
  * @retval None
  */
static void usbh_uac_hotplug_thread(void *param)
{
	int ret = 0;
	u32 hotplug_count = 0;

	UNUSED(param);

	for (;;) {
		/* Block until device is detached */
		if (rtos_sema_take(usbh_uac_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			hotplug_count++;
			RTK_LOGS(TAG, RTK_LOG_INFO, "\n========== Hotplug #%d ==========\n", hotplug_count);

			rtos_time_delay_ms(200);

			/* Cleanup USB resources */
			usbh_composite_deinit();
			usbh_deinit();

			rtos_time_delay_ms(100);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			/* Clear stale semaphores */
			while (rtos_sema_take(usbh_uac_play_start_sema, 0) == RTK_SUCCESS);
			while (rtos_sema_take(usbh_uac_record_start_sema, 0) == RTK_SUCCESS);

			/* Reinitialize USB stack */
			RTK_LOGS(TAG, RTK_LOG_INFO, "Re-init USB host...\n");

			ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init failed: %d\n", ret);
				break;
			}

			ret = usbh_composite_init(&usbh_hid_cfg, &usbh_uac_cfg);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Composite init failed: %d\n", ret);
				usbh_deinit();
				break;
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "Re-init complete\n");
		}
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread exited\n");
	usbh_uac_hotplug_task = NULL;
	rtos_task_delete(NULL);
}
#endif

/**
  * @brief  Demo bootstrap thread.
  *
  *         Allocates synchronization objects, spawns the resident playback
  *         and record workers, brings up the USB host core and the composite
  *         HID + UAC class driver, and (optionally) starts the hot-plug
  *         worker. On any setup failure it unwinds via labelled goto cleanup
  *         steps. The thread deletes itself once setup is complete; the
  *         spawned workers carry the demo from then on.
  * @param  param: Unused.
  * @retval None
  */
static void example_usbh_uac_thread(void *param)
{
	int status;

	UNUSED(param);

	/* Create synchronization primitives */
	rtos_sema_create(&usbh_uac_detach_sema, 0U, 1U);
	rtos_sema_create(&usbh_uac_play_start_sema, 0U, 1U);
	rtos_sema_create(&usbh_uac_record_start_sema, 0U, 1U);

	/* Create resident playback thread */
	status = rtos_task_create(&usbh_uac_play_task, "usbh_uac_play", usbh_uac_play_thread,
							  NULL, 1024, USBH_UAC_PLAY_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create play thread fail\n");
		goto free_sema_exit;
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

	status = usbh_composite_init(&usbh_hid_cfg, &usbh_uac_cfg);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Composite init failed: %d\n", status);
		goto usb_deinit_exit;
	}

	/* Create hot-plug monitor thread */
#if USBH_UAC_HOT_PLUG_TEST
	status = rtos_task_create(&usbh_uac_hotplug_task, "usbh_uac_hotplug", usbh_uac_hotplug_thread,
							  NULL, 768, USBH_UAC_HOTPLUG_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
		goto usbh_uac_deinit_exit;
	}
#endif

	goto example_exit;

#if USBH_UAC_HOT_PLUG_TEST
usbh_uac_deinit_exit:
#endif
	usbh_composite_deinit();

usb_deinit_exit:
	usbh_deinit();

delete_record_task_exit:
	usbh_uac_play_thread_exit = 1;
	usbh_uac_record_thread_exit = 1;
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

free_sema_exit:
	rtos_sema_delete(usbh_uac_detach_sema);
	rtos_sema_delete(usbh_uac_play_start_sema);
	rtos_sema_delete(usbh_uac_record_start_sema);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Demo stopped\n");

example_exit:
	rtos_task_delete(NULL);
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
	usbh_composite_uac_set_volume(cur_vol, uac_ctrl_dir);
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

	usbh_composite_uac_set_volume(cur_vol, uac_ctrl_dir);
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

		usbh_composite_uac_set_mute(mute, uac_ctrl_dir);
		RTK_LOGS(TAG, RTK_LOG_INFO, "%s\n", ((mute) ? ("Mute") : ("UnMute")));
	} else if (_stricmp(cmd, "vol") == 0) {
		cur_vol = (uac_ctrl_dir == USBH_UAC_ISOC_IN_DIR) ? &cur_record_volume : &cur_playback_volume;

		if ((argv[1] != NULL) && (_stricmp((const char *)argv[1], "down") == 0)) {
			uach_comp_voldown(cur_vol);
		} else {
			uach_comp_volup(cur_vol);
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Input cmd is err\n");
		return HAL_ERR_PARA;
	}

	return HAL_OK;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Entry point for the USB host composite HID + UAC demo.
  *
  *         Spawns the bootstrap thread that brings up the host stack and the
  *         worker threads. Safe to call once during application startup.
  * @retval None
  */
void example_usbh_composite_hid_uac(void)
{
	int status;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH UAC&HID composite demo start\n");

	status = rtos_task_create(&task, "usbh_uac_main", example_usbh_uac_thread, NULL, 1024U * 2, 2U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Failed to create main thread\n");
	}
}

/*
uach init record/playback
uach mute 1/0
uach vol down/up
 */
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbh_composite_uac_test_cmd_table[] = {
	{"uach", uach_comp_cmd},
};
