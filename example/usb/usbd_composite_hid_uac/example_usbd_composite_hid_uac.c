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
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
#define COMP_HID_INTR_IN_EP                           0x82U
#define COMP_HID_INTR_OUT_EP                          0x02U
#define COMP_HID_CONSUMER_INTR_IN_EP                  0x83U
#else
#define COMP_HID_INTR_IN_EP                           0x81U
#define COMP_HID_INTR_OUT_EP                          0x02U
#define COMP_HID_CONSUMER_INTR_IN_EP                  0x85U
#endif

/* UAC endpoint addresses */
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
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
#define COMP_UAC_ENABLE_RECORD                        0
#elif defined(CONFIG_USBD_UAC1)
/* UAC 1.0 spec supports only Full Speed. */
#define COMP_USB_SPEED                                USB_SPEED_HIGH_IN_FULL
/* Mic recording is only wired up in the UAC 2.0 class driver. */
#define COMP_UAC_ENABLE_RECORD                        0
#else
#define COMP_USB_SPEED                                USB_SPEED_HIGH
#define COMP_UAC_ENABLE_RECORD                        1
#endif

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define COMP_HOTPLUG                                  1

// Thread priorities
#define COMP_INIT_THREAD_PRIORITY                     5U
#define COMP_UAC_THREAD_PRIORITY                      4U
#define COMP_HOTPLUG_THREAD_PRIORITY                  8U
#define COMP_UAC_STATE_THREAD_PRIORITY                1U
// Thread stack sizes
#define COMP_INIT_THREAD_STACK_SIZE                   1024U
#define COMP_UAC_THREAD_STACK_SIZE                    (1024U * 16)
#define COMP_HOTPLUG_THREAD_STACK_SIZE                1024U
#define COMP_UAC_STATE_THREAD_STACK_SIZE              1024U

#ifdef CONFIG_USBD_HID_BIDIR
/* HID RX thread + console TX/volume commands */
#define COMP_HID_RX_THREAD_PRIORITY                   4U
#define COMP_HID_RX_THREAD_STACK_SIZE                 1024U
#define COMP_HID_TX_BUF_LEN                           1024U
#endif

#if COMP_UAC_ENABLE_RECORD
/* Mic loopback record thread, triggered by the "usbd_uac_record" console command */
#define COMP_UAC_RECORD_THREAD_PRIORITY               4U
#define COMP_UAC_RECORD_THREAD_STACK_SIZE             (1024U * 4)
#define COMP_UAC_RECORD_CHUNK_LEN                     64U
#define COMP_UAC_RECORD_CHUNK_DELAY_MS                1U
#include "example_usbd_composite_hid_uac_record_audio_data.h"
#endif

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int comp_init_stack(void);
#if COMP_HOTPLUG
static void composite_cb_status_changed(u8 old_status, u8 status);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "COMP";

static const usbd_config_t composite_cfg = {
	.speed = COMP_USB_SPEED,
	/* MIDDLE not HIGHEST: USB ISR at INT_PRI_HIGHEST would preempt audio
	 * hardware (sport/GDMA use INT_PRI_MIDDLE) — starving I2S DMA completion
	 * breaks UAC playback, and starving HID intr-out completion delays volup.
	 * Matches acm_uac / standalone hid / standalone uac which all use MIDDLE. */
	.isr_priority = INT_PRI_MIDDLE,
#if defined (CONFIG_AMEBASMART)
	/* SOF ISR only paces isoc-IN (recording); under UAC1 playback it instead
	 * runs the driver's handle_sof ZLP filler, which injects stale / drops real
	 * nodes in the isoc-OUT ring buffer and breaks playback. Enable it only when
	 * recording is on, matching cdc_acm_uac (no SOF -> UAC1 plays fine). */
#if COMP_UAC_ENABLE_RECORD
	.ext_intr_enable = USBD_SOF_INTR,
#endif
	.nptx_max_epmis_cnt = 100U,
#elif defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
	.rx_fifo_depth = 420U,
	.ptx_fifo_depth = {16U, 256U, 32U, 256U, },
#if COMP_UAC_ENABLE_RECORD
	.ext_intr_enable = USBD_SOF_INTR,
#endif
#elif defined (CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1424U,
	.ptx_fifo_depth = {256U, 32U, 256U, },
#if COMP_UAC_ENABLE_RECORD
	.ext_intr_enable = USBD_SOF_INTR,
#endif
#endif
};

/* HID endpoint configuration */
static const usbd_hid_ep_cfg_t hid_ep = {
	.intr_in_xfer_size = HID_INTR_IN_XFER_SIZE,
	.intr_in_addr  = COMP_HID_INTR_IN_EP,
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
	.intr_out_addr = COMP_HID_INTR_OUT_EP,
#endif
#ifdef CONFIG_USBD_HID_BIDIR
	.consumer_intr_in_addr = COMP_HID_CONSUMER_INTR_IN_EP,
#endif
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

#ifdef CONFIG_USBD_HID_BIDIR
static u8 hid_tx_buf[COMP_HID_TX_BUF_LEN];
static u8 hid_rx_buf[USBD_HID_MAX_BUF_SIZE];
#endif

/* UAC user callbacks */
static void composite_uac_cb_mute_changed(u8 mute);
static void composite_uac_cb_volume_changed(u8 volume);

static usbd_uac_cb_t composite_uac_cb = {
	/* in.enable must track record. UAC1's FS config desc declares no ISOC-IN EP
	 * (IAD bInterfaceCount=2, AS_OUT only); leaving in.enable=1 causes
	 * SET_CUR SAMPLING_FREQ (usbd_uac1.c ~1236) to ep_init a phantom IN EP
	 * the host never saw — on amebasmart shared DFIFO this perturbs the IN
	 * nextep_seq/EPMISCNT chain and breaks isoc-OUT playback in the
	 * HID+UAC1 combo (HID BIDIR periodic EPs amplify the pressure). */
#if COMP_UAC_ENABLE_RECORD
	.in = {
		.enable = 1,
		.sampling_freq = USBD_UAC_IN_DEFAULT_SAMPLING_FREQ,
		.byte_width = USBD_UAC_IN_DEFAULT_BYTE_WIDTH,
		.ch_cnt = USBD_UAC_IN_DEFAULT_CH_CNT
	},
#else
	.in = {.enable = 0, },
#endif
	.out = {.enable = 1, },
	.audio_ctx = NULL,
	.init = NULL,
	.deinit = NULL,
	.setup = NULL,
	.set_config = NULL,
	.status_changed = NULL,
	.mute_changed = composite_uac_cb_mute_changed,
	.volume_changed = composite_uac_cb_volume_changed,
	.format_changed = NULL, /* set to composite_uac_cb_format_changed in init thread */
	.sof = NULL,
};

/* UAC mute/volume state: updated by ISR callbacks, dumped by the state thread */
static u8 uac_cur_mute;
static u8 uac_cur_volume;
static rtos_sema_t uac_state_sema;

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
	.sampling_freq = USBD_UAC_SAMPLING_FREQ_48K,
	.byte_width = USBD_UAC_BYTE_WIDTH_2,
	.ch_cnt = USBD_UAC_CH_CNT_2,
	.enable = 1,
};

#if COMP_UAC_ENABLE_RECORD
/* Mic record format: fixed to match usbd_uac_record_audio_data[] (16bit/16000Hz/2ch) */
static const usbd_audio_cfg_t uac_record_cfg = {
	.sampling_freq = USBD_UAC_IN_DEFAULT_SAMPLING_FREQ,
	.byte_width = USBD_UAC_IN_DEFAULT_BYTE_WIDTH,
	.ch_cnt = USBD_UAC_IN_DEFAULT_CH_CNT,
	.enable = 1,
};
static rtos_sema_t uac_record_start_sema;
#endif

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
		/* Stop the current stream so usbd_uac_read returns at once (next_xfer=0) and the
		 * player thread restarts with the new format, re-arming the ISOC OUT EP. Without
		 * this the read loop live-locks on stale data after a mid-stream rate change. */
		audio_task_stop = 1;
		usbd_uac_stop_play();
		rtos_sema_give(uac_ready_sema);
	}
}

/**
  * @brief  Mute state change notification from the USB host
  * @note   This function is called within an ISR context; only lightweight state
  *         updates are permitted.
  * @param  mute: 1 if muted, 0 otherwise
  * @retval void
  */
static void composite_uac_cb_mute_changed(u8 mute)
{
	uac_cur_mute = mute;
	rtos_sema_give(uac_state_sema);
}

/**
  * @brief  Volume change notification from the USB host
  * @note   This function is called within an ISR context; only lightweight state
  *         updates are permitted.
  * @param  volume: New volume level
  * @retval void
  */
static void composite_uac_cb_volume_changed(u8 volume)
{
	uac_cur_volume = volume;
	rtos_sema_give(uac_state_sema);
}

/* UAC mute/volume state-dump thread */
static void example_usbd_composite_hid_uac_state_thread(void *param)
{
	UNUSED(param);

	while (1) {
		if (rtos_sema_take(uac_state_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			break;
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "Mute:%d vol:%d\n", uac_cur_mute, uac_cur_volume);
	}

	rtos_task_delete(NULL);
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
			read_dat_len = usbd_uac_read(recv_buf, COMP_USBD_AUDIO_MS_BUF_SIZE * 2, 500, NULL);
			if (read_dat_len > 0) {
#if COMP_UAC_DEMUX_CH_DEBUG
				play_data_size = 0;
				//get the 2 channel data from the 4 channel
				for (idx = 0, off = 0; idx < read_dat_len; idx += audio_src_step, off += audio_dst_step) {
					// ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3
					// 24  24  24  24  24  24  24  24  24  24  24  24
					usb_os_memcpy((void *)(play_buf + off), (const void *)(recv_buf + idx), audio_dst_step);
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

#ifdef CONFIG_USBD_HID_BIDIR
/* Blocking HID OUT reader: dispatches whatever the host sends on the vendor INTR OUT EP */
static void example_usbd_composite_hid_uac_hid_rx_thread(void *param)
{
	u32 rx_len;

	UNUSED(param);

	while (1) {
		rx_len = usbd_hid_read(hid_rx_buf, USBD_HID_MAX_BUF_SIZE, 500U);
		if (rx_len > 0U) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "HID RX %u bytes, first byte:%02x\n", rx_len, hid_rx_buf[0]);
		}
	}
}

static u32 composite_hid_cmd_tx(u16 argc, u8 *argv[])
{
	u16 size = 10U;
	int ret;

	if (argc == 0U) {
		size = 10U;
	} else {
		size = (u16)_strtoul((const char *)argv[0], (char **)NULL, 10);
		if (size > COMP_HID_TX_BUF_LEN) {
			size = COMP_HID_TX_BUF_LEN;
		}
	}

	memset(hid_tx_buf, (u8)(size & 0xFFU), size);
	ret = usbd_hid_send_data(hid_tx_buf, size);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "HID tx dropped, busy\n");
	}

	return HAL_OK;
}

static u32 composite_hid_cmd_volup(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	usbd_hid_volume_ctrl(1);
	return HAL_OK;
}

static u32 composite_hid_cmd_voldown(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	usbd_hid_volume_ctrl(0);
	return HAL_OK;
}
#endif /* CONFIG_USBD_HID_BIDIR */

#if COMP_UAC_ENABLE_RECORD
/* Mic loopback record: replays usbd_uac_record_audio_data[] as the mic input, once triggered */
static void example_usbd_composite_hid_uac_record_thread(void *param)
{
	u32 offset;
	u8 chunk[COMP_UAC_RECORD_CHUNK_LEN];

	UNUSED(param);

	while (1) {
		if (rtos_sema_take(uac_record_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			break;
		}

		usbd_uac_config(&uac_record_cfg, 1, 0);
		if (usbd_uac_start_record() != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC start record fail\n");
			continue;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "UAC record start\n");
		offset = 0U;
		while (1) {
			memcpy(chunk, &usbd_uac_record_audio_data[offset], COMP_UAC_RECORD_CHUNK_LEN);
			usbd_uac_transmit_data(chunk, COMP_UAC_RECORD_CHUNK_LEN);
			offset += COMP_UAC_RECORD_CHUNK_LEN;
			if (offset >= usbd_uac_record_data_len) {
				offset = 0U;
			}
			rtos_time_delay_ms(COMP_UAC_RECORD_CHUNK_DELAY_MS);
		}
	}

	rtos_task_delete(NULL);
}

static u32 composite_uac_cmd_record(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	rtos_sema_give(uac_record_start_sema);
	return HAL_OK;
}
#endif /* COMP_UAC_ENABLE_RECORD */

#if defined(CONFIG_USBD_HID_BIDIR) || COMP_UAC_ENABLE_RECORD
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE composite_hid_uac_cmd_table[] = {
#ifdef CONFIG_USBD_HID_BIDIR
	{"usbd_hid_tx", composite_hid_cmd_tx},
	{"usbd_hid_volup", composite_hid_cmd_volup},
	{"usbd_hid_voldown", composite_hid_cmd_voldown},
#endif
#if COMP_UAC_ENABLE_RECORD
	{"usbd_uac_record", composite_uac_cmd_record},
#endif
};
#endif

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

	ret = usbd_composite_hid_init(&composite_hid_usr_cb, &hid_ep);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "HID init fail\n");
		goto exit_usbd_init;
	}

	ret = usbd_composite_uac_init(&composite_uac_cb, &uac_ep);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC init fail\n");
		goto exit_hid_init;
	}

	ret = usbd_composite_init(COMP_CB);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Composite init fail\n");
		goto exit_uac_init;
	}

	return HAL_OK;

exit_uac_init:
	usbd_uac_deinit();
exit_hid_init:
	usbd_hid_deinit();
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
				usbd_hid_deinit();
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

/* Release the semaphores created by example_usbd_composite() on the failure/teardown path */
static void example_usbd_composite_hid_uac_release_semas(void)
{
#if COMP_HOTPLUG
	rtos_sema_delete(comp_attach_status_changed_sema);
#endif
#if COMP_UAC_ENABLE_RECORD
	rtos_sema_delete(uac_record_start_sema);
#endif
	rtos_sema_delete(uac_state_sema);
	rtos_sema_delete(uac_ready_sema);
}

/* Init thread: bring up the composite stack, then start the hotplug watcher */
static void example_usbd_composite_hid_uac_init_thread(void *param)
{
	UNUSED(param);

	composite_uac_cb.format_changed = composite_uac_cb_format_changed;

	if (comp_init_stack() != HAL_OK) {
		goto exit_release_sema;
	}

#if COMP_HOTPLUG
	if (rtos_task_create(&comp_hotplug_task, "usbd_comp_hotplug_thread",
						 example_usbd_composite_hotplug_thread, NULL,
						 COMP_HOTPLUG_THREAD_STACK_SIZE, COMP_HOTPLUG_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
	}
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD COMP demo start\n");

#ifdef CONFIG_USBD_HID_BIDIR
	/* Start the HID RX poll thread only after usbd core/class init has
	 * fully succeeded, instead of racing it against usb_chip_init(). */
	if (rtos_task_create(NULL, "usbd_comp_hid_rx_thread",
						 example_usbd_composite_hid_uac_hid_rx_thread, NULL,
						 COMP_HID_RX_THREAD_STACK_SIZE,
						 COMP_HID_RX_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create HID RX thread fail\n");
	}
#endif

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
	/* Created here (after this thread's own CPU1-bring-up delay has already
	 * elapsed) rather than from example_usbd_composite(), because a task's
	 * first blocking wait (sema_take/mutex/delay) hitting the same narrow
	 * boot window corrupts the FreeRTOS blocked-list bookkeeping. */
	if (rtos_task_create(NULL, "usbd_comp_playback_thread",
						 example_usbd_composite_hid_uac_audio_track_thread, NULL,
						 COMP_UAC_THREAD_STACK_SIZE,
						 COMP_UAC_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create audio track fail\n");
	}
#endif

	if (rtos_task_create(NULL, "usbd_comp_state_thread",
						 example_usbd_composite_hid_uac_state_thread, NULL,
						 COMP_UAC_STATE_THREAD_STACK_SIZE,
						 COMP_UAC_STATE_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create UAC state thread fail\n");
	}

#if COMP_UAC_ENABLE_RECORD
	if (rtos_task_create(NULL, "usbd_comp_record_thread",
						 example_usbd_composite_hid_uac_record_thread, NULL,
						 COMP_UAC_RECORD_THREAD_STACK_SIZE,
						 COMP_UAC_RECORD_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create UAC record thread fail\n");
	}
#endif

exit:
	rtos_task_delete(NULL);
	return;

exit_release_sema:
	example_usbd_composite_hid_uac_release_semas();
	goto exit;
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

	if (rtos_sema_create(&uac_ready_sema, 0U, 1U) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		return;
	}
	if (rtos_sema_create(&uac_state_sema, 0U, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto exit_release_sema;
	}
	audio_task_stop = 0;

#if COMP_UAC_ENABLE_RECORD
	if (rtos_sema_create(&uac_record_start_sema, 0U, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto exit_release_sema;
	}
#endif

#if COMP_HOTPLUG
	if (rtos_sema_create(&comp_attach_status_changed_sema, 0U, 1U) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto exit_release_sema;
	}
#endif

	if (rtos_task_create(&task, "usbd_comp_init_thread",
						 example_usbd_composite_hid_uac_init_thread, NULL,
						 COMP_INIT_THREAD_STACK_SIZE,
						 COMP_INIT_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create init thread fail\n");
		goto exit_release_sema;
	}

	return;

exit_release_sema:
	example_usbd_composite_hid_uac_release_semas();
}
