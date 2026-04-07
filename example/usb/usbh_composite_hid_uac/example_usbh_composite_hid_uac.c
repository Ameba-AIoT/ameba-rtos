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
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/

/**
 * @brief Enable hot-plug support for memory leak testing
 * When enabled, the system will automatically reinitialize after device removal
 */
#define USBH_UAC_HOT_PLUG_TEST        1

/**
 * @brief Continue loop count
 */
#define USBH_UAC_TEST_CNT             30

/**
 * @brief Number of frames for ISOC transfer buffer
 * Affects latency and buffer size for audio streaming
 */
#define USBH_UAC_FRAME_CNT            20

/* Audio format configuration */
#define USBH_UAC_CHANNELS             2       /* Stereo audio */
#define USBH_UAC_BITWIDTH             16      /* 16-bit audio samples */
#define USBH_UAC_SAMPLING_FREQ        48000   /* 48kHz sample rate */

/* Thread priority definitions (higher number = higher priority) */
#define USBH_UAC_ISOC_TEST_THREAD_PRIORITY    4   /* Audio playback thread */
#define USBH_UAC_MAIN_THREAD_PRIORITY         5   /* USB host main thread */
#define USBH_UAC_HOTPLUG_THREAD_PRIORITY      6   /* Hot-plug detection thread */

/**
 * @brief Enable data validation check
 * When enabled, fills audio buffer with test pattern for debugging
 */
#define USBH_UAC_XFER_CHECK           0

#if USBH_UAC_XFER_CHECK
#define  USBH_UAC_OUT_DATA                0x88
static u32 usbh_uac_data_len = 1200;
static unsigned char usbh_uac_audio_data[1200] = {0,};
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
 * @brief Semaphore signaled when device is attached
 * Triggers playback thread to start playing
 */
static rtos_sema_t usbh_uac_attach_sema;

/**
 * @brief Semaphore signaled when device setup is complete
 * Indicates audio playback can begin
 */
static rtos_sema_t usbh_uac_isoc_start_sema;

/**
 * @brief Flag indicating device is ready for audio transfer
 * Set after successful device enumeration and configuration
 * Reset when device is removed
 */
static __IO int usbh_uac_is_ready = 0;

/**
 * @brief Flag to request playback thread to exit completely
 * Only set when application is shutting down
 */
static __IO int usbh_uac_thread_exit = 0;

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
 * @brief Handle for audio playback thread (resident)
 * Created once at startup, never deleted
 */
static rtos_task_t usbh_uac_isoc_task = NULL;

/**
 * @brief Handle for hot-plug monitoring thread
 * Remains active for entire application lifetime
 */
#if USBH_UAC_HOT_PLUG_TEST
static rtos_task_t usbh_uac_hotplug_task = NULL;
#endif

/**
 * @brief USB host controller configuration
 * Defines hardware parameters and priorities
 */
static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_FULL,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
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
 * @brief HID report callback - handles button presses
 */
static int usbh_hid_cb_report(usbh_composite_hid_event_t *event)
{
	if (!event) {
		return HAL_OK;
	}

	RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "\nHID event rx\n");
	RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "Press: %d, Type: %d\n", event->is_press, event->type);

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
 * @brief UAC initialization callback
 */
static int usbh_uac_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Init\n");
	return HAL_OK;
}

/**
 * @brief UAC deinitialization callback
 */
static int usbh_uac_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Deinit\n");
	return HAL_OK;
}

/**
 * @brief UAC device attach callback
 */
static int usbh_uac_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Attach\n");

	/* Reset play counter for new connection */
	usbh_uac_play_count = 0;

	/* Signal playback thread that device is attached */
	rtos_sema_give(usbh_uac_attach_sema);
	return HAL_OK;
}

/**
 * @brief UAC device detach callback
 */
static int usbh_uac_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Detached, total played: %d times\n", usbh_uac_play_count);

	/* Clear ready flag - this will stop playback loop */
	usbh_uac_is_ready = 0;

	/* Immediately stop audio to prevent noise */
	usbh_composite_uac_stop_play();

#if USBH_UAC_HOT_PLUG_TEST
	/* Signal hot-plug thread to reinitialize */
	rtos_sema_give(usbh_uac_detach_sema);
#endif

	return HAL_OK;
}

/**
 * @brief UAC setup complete callback
 */
static int usbh_uac_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");

	/* Mark device as ready */
	usbh_uac_is_ready = 1;

	/* Signal playback thread that setup is complete */
	rtos_sema_give(usbh_uac_isoc_start_sema);
	return HAL_OK;
}

/**
 * @brief ISOC transfer completion callback
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
 * @brief USB host event processor callback
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
 * @brief Audio playback thread - resident mode with state control
 */
static void usbh_uac_isoc_test(void *param)
{
	/* Audio buffer and format variables */
	const unsigned char *usbh_uac_audio_data_handle = usbh_uac_audio_data;
	const usbh_audio_fmt_t *fmt_info = NULL;
	const usbh_audio_fmt_t *audio_fmt = NULL;
	u8 *buffer = NULL;
	u32 audio_total_data_len = usbh_uac_data_len;
	u32 play_loop_count = 0;
	u32 frame_size = 0;
	u32 total_len = 0;
	u32 send_len = 0;
	u32 offset = 0;
	u32 ret;
	u8 is_playing = 0;
	u8 fmt_cnt;
	u8 i = 0;

	UNUSED(param);

	while (!usbh_uac_thread_exit) {

		/*
		 * PHASE 1: WAIT FOR DEVICE ATTACHMENT
		 **/

		RTK_LOGS(TAG, RTK_LOG_INFO, "Wait attach\n");

		/* Block until device is attached */
		if (rtos_sema_take(usbh_uac_attach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}

		/* Check if thread should exit */
		if (usbh_uac_thread_exit) {
			break;
		}

		/*
		 * PHASE 2: WAIT FOR DEVICE SETUP COMPLETE
		 **/

		/* Block until device is configured and ready */
		if (rtos_sema_take(usbh_uac_isoc_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}

		/* Check if device was removed during setup */
		if (!usbh_uac_is_ready || usbh_uac_thread_exit) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Device removed\n");
			continue;
		}

		/*
		 * PHASE 3: AUDIO FORMAT CONFIGURATION
		 **/

		/* Query available audio formats from device */
		fmt_info = usbh_composite_uac_get_alt_setting(0, &fmt_cnt);

		if ((fmt_info == NULL) || (fmt_cnt == 0)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Failed to get audio format\n");
			usbh_uac_is_ready = 0;
			continue;
		}

		/* Log all available formats */
		RTK_LOGS(TAG, RTK_LOG_INFO, "Available audio formats:\n");
		for (i = 0; i < fmt_cnt; i++) {
			audio_fmt = &(fmt_info[i]);
			RTK_LOGS(TAG, RTK_LOG_INFO, "  [%d] CH=%d BW=%d FREQ=%d\n",
					 i, audio_fmt->ch_cnt, audio_fmt->bit_width, audio_fmt->sampling_freq);
		}

		/* Select matching audio format */
		if (usbh_composite_uac_set_alt_setting(0, USBH_UAC_CHANNELS, USBH_UAC_BITWIDTH, USBH_UAC_SAMPLING_FREQ) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "No suitable interface/altsetting found\n");
			usbh_uac_is_ready = 0;
			continue;
		}

		/* Get frame size for selected format */
		frame_size = usbh_composite_uac_get_frame_size(0);
		rtos_time_delay_ms(100);

		RTK_LOGS(TAG, RTK_LOG_INFO, "Start playback data size=%d, frame size=%d\n", audio_total_data_len, frame_size);

#if USBH_UAC_XFER_CHECK
		memset(usbh_uac_audio_data, USBH_UAC_OUT_DATA, usbh_uac_data_len);
#endif

		/*
		 * PHASE 4: START AUDIO PLAYBACK
		 **/

		/* Start the audio stream */
		if (usbh_uac_is_ready) {
			usbh_composite_uac_start_play();
			is_playing = 1;
			play_loop_count = 0;
		}

		total_len = audio_total_data_len;

		/*
		 * PHASE 5: CONTINUOUS PLAYBACK LOOP
		 **/

		while (usbh_uac_is_ready && !usbh_uac_thread_exit) {
			/* Reset for new loop */
			offset = 0;
			play_loop_count++;
			usbh_uac_play_count++;
			usbh_uac_err_count = 0;

			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Playing loop #%d (total: %d)\n", play_loop_count, usbh_uac_play_count);

			/*
			 * SEND AUDIO DATA
			 **/

			while (offset < total_len && usbh_uac_is_ready && !usbh_uac_thread_exit) {
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

				buffer = (u8 *)(usbh_uac_audio_data_handle + offset);

				/* Send audio data with timeout */
				ret = usbh_composite_uac_write(buffer, send_len, 10);

				if (ret != send_len) {
					usbh_uac_err_count++;

					if (!usbh_uac_is_ready || usbh_uac_err_count > 100) {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Errors %d\n", usbh_uac_err_count);
						goto stop_play;
					}
					continue;
				}

				offset += ret;
			}

			/* Periodic statistics output */
			if (play_loop_count % USBH_UAC_TEST_CNT == 0) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Stats: loops=%d, errors=%d, busy=%d\n", play_loop_count, usbh_uac_err_count, usbh_uac_busy_count);
			}
		}

		/*
		 * PHASE 6: STOP PLAYBACK AND CLEANUP FOR THIS SESSION
		 **/
stop_play:
		if (is_playing) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Stop playback (played %d loops)\n", play_loop_count);
			usbh_composite_uac_stop_play();
			is_playing = 0;
			rtos_time_delay_ms(50);
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Go back to idle\n\n");
	}

	/* Force stop if somehow still playing */
	if (is_playing) {
		usbh_composite_uac_stop_play();
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Playback thread exited\n");

	usbh_uac_isoc_task = NULL;
	rtos_task_delete(NULL);
}

#if USBH_UAC_HOT_PLUG_TEST
/**
 * @brief Hot-plug monitoring thread
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
			RTK_LOGS(TAG, RTK_LOG_INFO, "\n========== Hotplug event #%d ==========\n", hotplug_count);

			/* Wait for playback to stop */
			rtos_time_delay_ms(200);

			/* Cleanup USB resources */
			usbh_composite_deinit();
			usbh_deinit();

			rtos_time_delay_ms(100);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			/* Clear stale semaphores */
			while (rtos_sema_take(usbh_uac_isoc_start_sema, 0) == RTK_SUCCESS);
			while (rtos_sema_take(usbh_uac_attach_sema, 0) == RTK_SUCCESS);

			/* Reinitialize USB stack */
			RTK_LOGS(TAG, RTK_LOG_INFO, "Re-init USB host...\n");

			ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init failed: %d\n", ret);
				break;
			}

			ret = usbh_composite_init(&usbh_hid_cfg, &usbh_uac_cfg, USBH_UAC_FRAME_CNT);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Composite init failed: %d\n", ret);
				usbh_deinit();
				break;
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "Re-init complete, wait device connect\n");
		}
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread exited\n");
	usbh_uac_hotplug_task = NULL;
	rtos_task_delete(NULL);
}
#endif

/**
 * @brief Main initialization thread for UAC example
 */
static void example_usbh_uac_thread(void *param)
{
	int status;

	UNUSED(param);

	/* Create synchronization primitives */
	rtos_sema_create(&usbh_uac_detach_sema, 0U, 1U);
	rtos_sema_create(&usbh_uac_attach_sema, 0U, 1U);
	rtos_sema_create(&usbh_uac_isoc_start_sema, 0U, 1U);

	/* Create resident playback thread */
	status = rtos_task_create(&usbh_uac_isoc_task, "usbh_uac_player", usbh_uac_isoc_test, NULL, 1024U * 2, USBH_UAC_ISOC_TEST_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create play thread fail\n");
		goto free_sema_exit;
	}

	/* Initialize USB host stack */
	status = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH init failed: %d\n", status);
		goto delete_play_task_exit;
	}

	status = usbh_composite_init(&usbh_hid_cfg, &usbh_uac_cfg, USBH_UAC_FRAME_CNT);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Composite init failed: %d\n", status);
		goto usb_deinit_exit;
	}

	/* Create hot-plug monitor thread */
#if USBH_UAC_HOT_PLUG_TEST
	status = rtos_task_create(&usbh_uac_hotplug_task, "usbh_uac_hotplug", usbh_uac_hotplug_thread, NULL, 1024U * 2, USBH_UAC_HOTPLUG_THREAD_PRIORITY);
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

delete_play_task_exit:
	usbh_uac_thread_exit = 1;
	rtos_sema_give(usbh_uac_attach_sema);
	rtos_time_delay_ms(500);
	if (usbh_uac_isoc_task != NULL) {
		rtos_task_delete(usbh_uac_isoc_task);
		usbh_uac_isoc_task = NULL;
	}

free_sema_exit:
	rtos_sema_delete(usbh_uac_detach_sema);
	rtos_sema_delete(usbh_uac_attach_sema);
	rtos_sema_delete(usbh_uac_isoc_start_sema);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Demo stopped\n");

example_exit:
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/
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
