/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "usbd_cdc_acm.h"
#include "usbd_uac.h"
#include "usbd_composite.h"
#include "os_wrapper.h"
#include "ameba_soc.h"
#include "platform_stdlib.h"
#include "basic_types.h"

/* This used to check the USB issue */
/*
	Note:
	If EVB is AMEBAGREEN2 and CONFIG_SUPPORT_AUDIO_FOR_USB=1, then OS needs to be configured as FREERTOS(default is FREERTOS_ROM)
*/

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
#include "audio/audio_control.h"
#include "audio/audio_equalizer.h"
#include "audio/audio_track.h"
#include "audio/audio_service.h"
#include "common/audio_errnos.h"
#endif

/* Private defines -----------------------------------------------------------*/

/* CDC ACM endpoint addresses */
#if defined (CONFIG_AMEBAGREEN2)
#define COMP_CDC_BULK_IN_EP                           0x82U
#define COMP_CDC_BULK_OUT_EP                          0x02U
#define COMP_CDC_INTR_IN_EP                           0x83U
#else
#define COMP_CDC_BULK_IN_EP                           0x81U
#define COMP_CDC_BULK_OUT_EP                          0x02U
#define COMP_CDC_INTR_IN_EP                           0x83U
#endif

/* UAC endpoint addresses */
#if defined (CONFIG_AMEBAGREEN2)
#define COMP_UAC_ISOC_IN_EP                           0x84U
#define COMP_UAC_ISOC_OUT_EP                          0x05U
#else
#define COMP_UAC_ISOC_IN_EP                           0x85U
#define COMP_UAC_ISOC_OUT_EP                          0x04U
#endif

#define CDC_BULK_IN_XFER_SIZE                         2048U
#define CDC_BULK_OUT_XFER_SIZE                        2048U

#define COMP_UAC_DEMUX_CH_DEBUG                      1

#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define COMP_USB_SPEED                                USB_SPEED_FULL
#elif defined(CONFIG_USBD_UAC1)
/* UAC 1.0 spec supports only Full Speed. */
#define COMP_USB_SPEED                                USB_SPEED_HIGH_IN_FULL
#else
#define COMP_USB_SPEED                                USB_SPEED_HIGH
#endif

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define COMP_HOTPLUG                                  1

// Thread priorities
#define COMP_INIT_THREAD_PRIORITY                     5U
#define COMP_UAC_THREAD_PRIORITY                      4U
#define COMP_HOTPLUG_THREAD_PRIORITY                  8U
// Thread stack sizes
#define COMP_INIT_THREAD_STACK_SIZE                   1024U
#define COMP_UAC_THREAD_STACK_SIZE                    (1024U * 16)
#define COMP_HOTPLUG_THREAD_STACK_SIZE                1024U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int composite_cdc_acm_cb_init(void);
static int composite_cdc_acm_cb_deinit(void);
static int composite_cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int composite_cdc_acm_cb_received(u8 *buf, u32 Len);

static int comp_init_stack(void);
#if COMP_HOTPLUG
static void composite_cb_status_changed(u8 old_status, u8 status);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "COMP";

static const usbd_config_t composite_cfg = {
	.speed = COMP_USB_SPEED,
	.isr_priority = INT_PRI_MIDDLE,
#if defined (CONFIG_AMEBASMART)
	.nptx_max_epmis_cnt = 100U,
#elif defined (CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 420U,
	.ptx_fifo_depth = {16U, 256U, 32U, 256U, },
#elif defined (CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1424U,
	.ptx_fifo_depth = {256U, 32U, 256U, },
#endif
};

/* CDC ACM endpoint configuration */
static const usbd_cdc_acm_ep_cfg_t cdc_acm_ep = {
	.bulk_in_addr  = COMP_CDC_BULK_IN_EP,
	.bulk_out_addr = COMP_CDC_BULK_OUT_EP,
	.intr_in_addr  = COMP_CDC_INTR_IN_EP,
	.bulk_in_xfer_size  = CDC_BULK_IN_XFER_SIZE,
	.bulk_out_xfer_size = CDC_BULK_OUT_XFER_SIZE,
};

/* CDC ACM user callbacks */
static const usbd_cdc_acm_cb_t composite_cdc_acm_usr_cb = {
	.init            = composite_cdc_acm_cb_init,
	.deinit          = composite_cdc_acm_cb_deinit,
	.setup           = composite_cdc_acm_cb_setup,
	.received        = composite_cdc_acm_cb_received,
	.transmitted     = NULL,
	.status_changed  = NULL,
	.wakeup          = NULL,
};

/* UAC endpoint configuration */
static const usbd_uac_ep_cfg_t uac_ep = {
	.isoc_in_addr  = COMP_UAC_ISOC_IN_EP,
	.isoc_out_addr = COMP_UAC_ISOC_OUT_EP,
};

/* UAC user callbacks */
static usbd_uac_cb_t composite_uac_cb = {
	.in = {.enable = 0, },
	.out = {.enable = 1, },
	.audio_ctx = NULL,
	.init = NULL,
	.deinit = NULL,
	.setup = NULL,
	.set_config = NULL,
	.status_changed = NULL,
	.mute_changed = NULL,
	.volume_changed = NULL,
	.format_changed = NULL,
	.sof = NULL,
};

static usb_cdc_acm_line_coding_t composite_cdc_acm_line_coding;

/* UAC audio data buffers and play control */
#define COMP_USBD_AUDIO_MS_BUF_SIZE               1024U
#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
static u8 play_buf[COMP_USBD_AUDIO_MS_BUF_SIZE];
static u8 recv_buf[COMP_USBD_AUDIO_MS_BUF_SIZE * 2];
#endif

static rtos_sema_t uac_ready_sema;
static volatile u8 audio_task_stop;
#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
static volatile u8 uac_playing;
#endif

#if COMP_HOTPLUG
static rtos_task_t comp_hotplug_task;
static rtos_sema_t comp_attach_status_changed_sema;
static u8 comp_attach_status;

/* Composite-level callback: forwarded the aggregated attach status by the
   composite framework, used to drive the hotplug thread. */
static const usbd_composite_cb_t composite_usr_cb = {
	.status_changed = composite_cb_status_changed,
};
#define COMP_CB (&composite_usr_cb)
#else
#define COMP_CB NULL
#endif

/* UAC format info updated by format_changed callback */
static usbd_audio_cfg_t uac_play_cfg = {
	.sampling_freq = 48000,
	.byte_width = 2,
	.ch_cnt = 2,
	.enable = 1,
};

/* Private function prototypes -----------------------------------------------*/
#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
static void example_audio_track_play(void);
#endif

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CDC ACM media layer
  * @param  None
  * @retval Status
  */
static int composite_cdc_acm_cb_init(void)
{
	usb_cdc_acm_line_coding_t *lc = &composite_cdc_acm_line_coding;

	lc->b.dwDteRate = 150000;
	lc->b.bCharFormat = 0x00;
	lc->b.bParityType = 0x00;
	lc->b.bDataBits = 0x08;

	return 0;
}

/**
  * @brief  DeInitializes the CDC ACM media layer
  * @param  None
  * @retval Status
  */
static int composite_cdc_acm_cb_deinit(void)
{
	return 0;
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC ACM interface through this function.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  Buf: RX buffer
  * @param  Len: RX data length (in bytes)
  * @retval Status
  */
static int composite_cdc_acm_cb_received(u8 *buf, u32 len)
{
	if (buf && len) {
		usbd_cdc_acm_transmit(buf, len);
	}
	return 0;
}

/**
  * @brief  Handle the CDC class control requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @retval Status
  */
static int composite_cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	usb_cdc_acm_line_coding_t *lc = &composite_cdc_acm_line_coding;

	switch (req->bRequest) {
	case USB_CDC_ACM_SET_LINE_CODING:
		if (req->wLength == USB_CDC_ACM_LINE_CODING_SIZE) {
			lc->b.dwDteRate = (u32)(buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));
			lc->b.bCharFormat = buf[4];
			lc->b.bParityType = buf[5];
			lc->b.bDataBits = buf[6];
		}
		break;

	case USB_CDC_ACM_GET_LINE_CODING:
		buf[0] = (u8)(lc->b.dwDteRate & 0xFF);
		buf[1] = (u8)((lc->b.dwDteRate >> 8) & 0xFF);
		buf[2] = (u8)((lc->b.dwDteRate >> 16) & 0xFF);
		buf[3] = (u8)((lc->b.dwDteRate >> 24) & 0xFF);
		buf[4] = lc->b.bCharFormat;
		buf[5] = lc->b.bParityType;
		buf[6] = lc->b.bDataBits;
		break;

	case USB_CDC_ACM_SET_CONTROL_LINE_STATE:
		/*
		wValue:	wValue, Control Signal Bitmap
				D2-15:	Reserved, 0
				D1:	RTS, 0 - Deactivate, 1 - Activate
				D0:	DTR, 0 - Not Present, 1 - Present
		*/
		if (req->wValue & 0x01) {
			/* VCOM port activate */
			USB_DIAG(USB_LAYER_APP, USB_EVT_LINK, 0);
			usbd_cdc_acm_notify_serial_state(USB_CDC_ACM_CTRL_DSR | USB_CDC_ACM_CTRL_DCD);
		}
		break;

	case USB_CDC_ACM_SEND_BREAK:
		/* Do nothing */
		break;

	default:
		break;
	}

	return 0;
}

/* playback , USB OUT */
#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
static void example_audio_track_play(void)
{
	u32 read_dat_len = 0;

	RTK_LOGS(TAG, RTK_LOG_INFO, "Audio track demo begin\n");

	usbd_uac_config(&uac_play_cfg, 0, 0);
	do {
		if (usbd_uac_start_play() == HAL_OK) {
			break;
		}
	} while (1);

	{
		struct AudioTrack *audio_track;
		uint32_t format;
		int32_t track_buf_size;

		uint32_t g_track_rate = uac_play_cfg.sampling_freq;
		uint32_t g_track_channel = uac_play_cfg.ch_cnt;
		uint32_t g_track_format = uac_play_cfg.byte_width * 8;

		uint32_t play_track_channel = g_track_channel;

#if COMP_UAC_DEMUX_CH_DEBUG
		uint32_t idx = 0, off = 0;
		//force to get the 1st channel to play
		play_track_channel = 1;
		uint32_t play_data_size;
		const uint32_t audio_src_step = g_track_channel * g_track_format / 8;
		const uint32_t audio_dst_step = play_track_channel * g_track_format / 8;
#endif

		//user should set sdk/component/soc/**/usrcfg/include/ameba_audio_hw_usrcfg.h's AUDIO_HW_AMPLIFIER_PIN to make sure amp is enabled.
		AudioService_Init();

		RTK_LOGS(TAG, RTK_LOG_INFO, "Audio ch:%u,rate:%u,bits=%u\n", g_track_channel, g_track_rate, g_track_format);

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
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create AudioTrack fail\n");
			return;
		}

		track_buf_size = AudioTrack_GetMinBufferBytes(audio_track, AUDIO_CATEGORY_MEDIA, g_track_rate, format, play_track_channel) * 4;
		if (track_buf_size == 0) {
			track_buf_size = g_track_rate * g_track_format / 8 * play_track_channel / 1000 * 100;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Track buf resize to %d\n", track_buf_size);
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Track buf size:%d\n", track_buf_size);
		}
		AudioTrackConfig  track_config;
		track_config.category_type = AUDIO_CATEGORY_MEDIA;
		track_config.sample_rate = g_track_rate;
		track_config.format = format;
		track_config.channel_count = play_track_channel;
		track_config.buffer_bytes = track_buf_size;
		AudioTrack_Init(audio_track, &track_config, AUDIO_OUTPUT_FLAG_NONE);

		RTK_LOGS(TAG, RTK_LOG_INFO, "Track buf size:%d\n", track_buf_size);

		/*for mixer version, this mean sw volume, for passthrough version, sw volume is not supported*/
		AudioTrack_SetVolume(audio_track, 1.0, 1.0);
		AudioTrack_SetStartThresholdBytes(audio_track, track_buf_size);

		if (AudioTrack_Start(audio_track) != AUDIO_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Audio track start fail\n");
			return;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "UAC stop %d\n", audio_task_stop);

		uac_playing = 1;
		while (!audio_task_stop) {
			read_dat_len = usbd_uac_read(recv_buf, COMP_USBD_AUDIO_MS_BUF_SIZE * 2, 500);
			if (read_dat_len > 0) {
#if COMP_UAC_DEMUX_CH_DEBUG
				play_data_size = 0;
				//get the 2 channel data from the 4 channel
				for (idx = 0, off = 0; idx < read_dat_len; idx += audio_src_step, off += audio_dst_step) {
					// ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3
					// 24  24  24  24  24  24  24  24  24  24  24  24
					memcpy((void *)(play_buf + off), (void *)(recv_buf + idx), audio_dst_step);
					play_data_size += audio_dst_step;
				}

				AudioTrack_Write(audio_track, (u8 *)play_buf, play_data_size, true);
#else
				AudioTrack_Write(audio_track, (u8 *)recv_buf, read_dat_len, true);
#endif
			}
		}

		usbd_uac_stop_play();

		AudioTrack_Pause(audio_track);
		AudioTrack_Flush(audio_track);
		AudioTrack_Stop(audio_track);
		AudioTrack_Destroy(audio_track);

		audio_track = NULL;
		uac_playing = 0;
	}
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Audio track demo stop\n\n\n");
}
#endif

/**
  * @brief  Audio format change notification from the USB host
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  sampling_freq: New sampling frequency
  * @param  ch_cnt: New channel count
  * @param  byte_width: New sample byte width
  * @retval void
  */
static void composite_uac_cb_format_changed(u32 sampling_freq, u8 ch_cnt, u8 byte_width)
{
	if (sampling_freq != 0U) {
		uac_play_cfg.sampling_freq = sampling_freq;
	}
	if (ch_cnt != 0U) {
		uac_play_cfg.ch_cnt = ch_cnt;
	}
	if (byte_width != 0U) {
		uac_play_cfg.byte_width = byte_width;
	}

	if (sampling_freq && ch_cnt && byte_width) {
		rtos_sema_give(uac_ready_sema);
		audio_task_stop = 1;
	}
}

/**
  * @brief  Bring up the whole composite stack (core + each class + framework).
  * @note   Reused by both the initial start-up and the hotplug re-init path.
  * @retval HAL_OK on success, other HAL_Status code on failure (all partial resources rolled back)
  */
static int comp_init_stack(void)
{
	int ret;

	ret = usbd_init(&composite_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init fail\n");
		return ret;
	}

	ret = usbd_composite_cdc_acm_init(&composite_cdc_acm_usr_cb, &cdc_acm_ep);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "CDC ACM init fail\n");
		goto exit_usbd_init;
	}

	ret = usbd_composite_uac_init(&composite_uac_cb, &uac_ep);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC init fail\n");
		goto exit_cdc_acm_init;
	}

	ret = usbd_composite_init(COMP_CB);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Composite init fail\n");
		goto exit_uac_init;
	}

	return HAL_OK;

exit_uac_init:
	usbd_uac_deinit();
exit_cdc_acm_init:
	usbd_cdc_acm_deinit();
exit_usbd_init:
	usbd_deinit();
	return ret;
}

#if COMP_HOTPLUG
/**
  * @brief  Composite attach-status change notification (ISR context).
  * @note   time-consuming operations are not permitted here.
  */
static void composite_cb_status_changed(u8 old_status, u8 status)
{
	UNUSED(old_status);
	comp_attach_status = status;
	rtos_sema_give(comp_attach_status_changed_sema);
}

/* Tear down and re-init the whole composite stack on cable detach, to avoid
   memory leak across repeated plug/unplug. The audio playback loop is stopped
   first so the UAC class can be torn down safely. Deinit order is the reverse
   of init: framework -> classes -> core. */
static void example_usbd_composite_hotplug_thread(void *param)
{
	UNUSED(param);
#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
	int wait_cnt;
#endif

	for (;;) {
		if (rtos_sema_take(comp_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (comp_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");
#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
				/* Stop the playback loop and wait for it to unwind before
				   tearing down the UAC class. */
				audio_task_stop = 1;
				usbd_uac_stop_play();
				rtos_time_delay_ms(200);
				wait_cnt = 0;
				while ((uac_playing != 0) && (wait_cnt < 25)) { /* max wait 500ms */
					rtos_time_delay_ms(20);
					wait_cnt++;
				}
#endif
				usbd_composite_deinit();
				usbd_uac_deinit();
				usbd_cdc_acm_deinit();
				if (usbd_deinit() != HAL_OK) {
					break;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				if (comp_init_stack() != HAL_OK) {
					break;
				}
			} else if (comp_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\n");
	rtos_task_delete(NULL);
}
#endif // COMP_HOTPLUG

/* Init thread: bring up the composite stack, then start the hotplug watcher */
static void example_usbd_composite_acm_uac_init_thread(void *param)
{
	UNUSED(param);

	composite_uac_cb.format_changed = composite_uac_cb_format_changed;

	if (comp_init_stack() != HAL_OK) {
		goto exit;
	}

#if COMP_HOTPLUG
	if (rtos_task_create(&comp_hotplug_task, "usbd_comp_hotplug_thread",
						 example_usbd_composite_hotplug_thread, NULL,
						 COMP_HOTPLUG_THREAD_STACK_SIZE, COMP_HOTPLUG_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
	}
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD COMP demo start\n");

exit:
	rtos_task_delete(NULL);
}

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
static void example_usbd_composite_acm_uac_audio_track_thread(void *param)
{
	UNUSED(param);

	do {
		if (rtos_sema_take(uac_ready_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			break;
		}
		audio_task_stop = 0;
		example_audio_track_play();
	} while (1);

	rtos_sema_delete(uac_ready_sema);
	rtos_task_delete(NULL);
}
#endif

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Example entry: composite CDC ACM + UAC device
  * @param  None
  * @retval None
  */
void example_usbd_composite(void)
{
	rtos_task_t task;

	if (rtos_sema_create(&uac_ready_sema, 0U, 1U) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		return;
	}
	audio_task_stop = 0;

#if COMP_HOTPLUG
	if (rtos_sema_create(&comp_attach_status_changed_sema, 0U, 1U) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		rtos_sema_delete(uac_ready_sema);
		return;
	}
#endif

	if (rtos_task_create(&task, "usbd_comp_init_thread",
						 example_usbd_composite_acm_uac_init_thread, NULL,
						 COMP_INIT_THREAD_STACK_SIZE,
						 COMP_INIT_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create init thread fail\n");
	}

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
	if (rtos_task_create(NULL, "usbd_comp_audio_thread",
						 example_usbd_composite_acm_uac_audio_track_thread, NULL,
						 COMP_UAC_THREAD_STACK_SIZE,
						 COMP_UAC_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create audio track fail\n");
	}
#endif
}
