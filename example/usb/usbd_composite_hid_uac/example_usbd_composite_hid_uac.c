/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "usbd_hid.h"
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

/* HID endpoint addresses */
#if defined (CONFIG_AMEBAGREEN2)
#define COMP_HID_INTR_IN_EP                           0x82U
#else
#define COMP_HID_INTR_IN_EP                           0x81U
#endif

/* UAC endpoint addresses */
#if defined (CONFIG_AMEBAGREEN2)
#define COMP_UAC_ISOC_IN_EP                           0x84U
#define COMP_UAC_ISOC_OUT_EP                          0x05U
#else
#define COMP_UAC_ISOC_IN_EP                           0x83U
#define COMP_UAC_ISOC_OUT_EP                          0x04U
#endif

/* HID EP tx buffer size */
#define HID_INTR_IN_XFER_SIZE                         64U

#define COMP_UAC_DEMUX_CH_DEBUG                      1

#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define COMP_USB_SPEED                                USB_SPEED_FULL
#elif defined(CONFIG_USBD_UAC1)
/* UAC 1.0 spec supports only Full Speed. */
#define COMP_USB_SPEED                                USB_SPEED_HIGH_IN_FULL
#else
#define COMP_USB_SPEED                                USB_SPEED_HIGH
#endif

// Thread priorities
#define COMP_INIT_THREAD_PRIORITY                     5U
#define COMP_UAC_THREAD_PRIORITY                      4U
// Thread stack sizes
#define COMP_INIT_THREAD_STACK_SIZE                   1024U
#define COMP_UAC_THREAD_STACK_SIZE                    (1024U * 16)

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

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

/* HID endpoint configuration */
static const usbd_hid_ep_cfg_t hid_ep = {
	.intr_in_xfer_size = HID_INTR_IN_XFER_SIZE,
	.intr_in_addr  = COMP_HID_INTR_IN_EP,
};

/* UAC endpoint configuration */
static const usbd_uac_ep_cfg_t uac_ep = {
	.isoc_in_addr  = COMP_UAC_ISOC_IN_EP,
	.isoc_out_addr = COMP_UAC_ISOC_OUT_EP,
};

/* HID user callbacks */
static void composite_hid_cb_init(void) { }
static void composite_hid_cb_deinit(void) { }
static const usbd_hid_usr_cb_t composite_hid_usr_cb = {
	.init = composite_hid_cb_init,
	.deinit = composite_hid_cb_deinit,
	.setup = NULL,
	.transmitted = NULL,
};

/* UAC user callbacks */
static usbd_uac_cb_t composite_uac_cb = {
	.in = {.enable = 1, },
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

/* UAC audio data buffers and play control */
#define COMP_USBD_AUDIO_MS_BUF_SIZE               1024U
#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
static u8 play_buf[COMP_USBD_AUDIO_MS_BUF_SIZE];
static u8 recv_buf[COMP_USBD_AUDIO_MS_BUF_SIZE * 2];
#endif

static rtos_sema_t uac_ready_sema;
static volatile u8 audio_task_stop;

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
	}
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Audio track demo stop\n\n\n");
}
#endif

/* Audio track thread */
#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
static void example_usbd_composite_hid_uac_audio_track_thread(void *param)
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

/* Init thread: 3-step init sequence */
static void example_usbd_composite_hid_uac_init_thread(void *param)
{
	UNUSED(param);
	int ret;

	composite_uac_cb.format_changed = composite_uac_cb_format_changed;

	ret = usbd_init(&composite_cfg);
	if (ret != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init fail\n");
		goto exit;
	}
	ret = usbd_composite_hid_init(&composite_hid_usr_cb, &hid_ep);
	if (ret != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "HID init fail\n");
		usbd_deinit();
		goto exit;
	}
	ret = usbd_composite_uac_init(&composite_uac_cb, &uac_ep);
	if (ret != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC init fail\n");
		usbd_hid_deinit();
		usbd_deinit();
		goto exit;
	}
	ret = usbd_composite_init(NULL);
	if (ret != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Composite init fail\n");
		usbd_uac_deinit();
		usbd_hid_deinit();
		usbd_deinit();
		goto exit;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD COMP demo start\n");

exit:
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Example entry: composite HID + UAC device
  * @param  None
  * @retval None
  */
void example_usbd_composite(void)
{
	rtos_task_t task;

	rtos_sema_create(&uac_ready_sema, 0U, 1U);
	audio_task_stop = 0;

	if (rtos_task_create(&task, "usbd_composite_hid_uac_init",
						 example_usbd_composite_hid_uac_init_thread, NULL,
						 COMP_INIT_THREAD_STACK_SIZE,
						 COMP_INIT_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create init thread fail\n");
	}

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
	if (rtos_task_create(NULL, "usbd_composite_hid_uac_audio",
						 example_usbd_composite_hid_uac_audio_track_thread, NULL,
						 COMP_UAC_THREAD_STACK_SIZE,
						 COMP_UAC_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create audio track fail\n");
	}
#endif
}
