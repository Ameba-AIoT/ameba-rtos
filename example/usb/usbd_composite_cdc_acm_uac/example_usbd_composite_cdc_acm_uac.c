/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "usbd_cdc_acm.h"
#include "usbd_uac.h"
#include "usbd_composite.h"
#include "os_wrapper.h"
#include "ameba_soc.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "atcmd_service.h"
#include "usb_app.h"
#include "example_usbd_composite_cdc_acm_uac.h"

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
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
#define COMP_CDC_BULK_IN_EP                           0x82U
#define COMP_CDC_BULK_OUT_EP                          0x02U
#define COMP_CDC_INTR_IN_EP                           0x83U
#else
#define COMP_CDC_BULK_IN_EP                           0x81U
#define COMP_CDC_BULK_OUT_EP                          0x02U
#define COMP_CDC_INTR_IN_EP                           0x83U
#endif

/* UAC endpoint addresses */
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
#define COMP_UAC_RECORD_EP                            0x84U
#define COMP_UAC_PLAY_EP                              0x05U
#define COMP_UAC_PLAY_FEEDBACK_EP                     0x86U
#else
/* A shared DFIFO SoC builds the record or the playback feedback, never both, refer to usbd_uac.h:
   with the CDC ACM they take the last of the three IN endpoints EP1/EP3/EP5 */
#define COMP_UAC_RECORD_EP                            0x85U
#define COMP_UAC_PLAY_EP                              0x04U
#define COMP_UAC_PLAY_FEEDBACK_EP                     0x85U
#endif

#define CDC_BULK_IN_XFER_SIZE                         2048U
#define CDC_BULK_OUT_XFER_SIZE                        2048U

/* The board plays 2 channels: a host stream of more channels (4/6/8) is cut down to its front
   left and right channels, a stereo or mono stream is played as is */
#define COMP_UAC_DEMUX_CH_DEBUG                      1
#define COMP_UAC_PLAY_MAX_TRACK_CHANNEL               2U

/* Mic recording, built with the class record path: CONFIG USB > UAC > Enable record (microphone) */
#ifdef CONFIG_USBD_UAC_RECORD
#define COMP_UAC_ENABLE_RECORD                        1
#else
#define COMP_UAC_ENABLE_RECORD                        0
#endif

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define COMP_HOTPLUG                                  1

// Worker threads joined by a stop: playback, plus record and hotplug
#define COMP_WORKER_CNT                               (1U + COMP_UAC_ENABLE_RECORD + COMP_HOTPLUG)

// Thread priorities
#define COMP_INIT_THREAD_PRIORITY                     5U
#define COMP_UAC_THREAD_PRIORITY                      4U
#define COMP_HOTPLUG_THREAD_PRIORITY                  8U
// Thread stack sizes
#define COMP_INIT_THREAD_STACK_SIZE                   1024U
#define COMP_UAC_THREAD_STACK_SIZE                    (1024U * 16)
#define COMP_HOTPLUG_THREAD_STACK_SIZE                1024U

#if COMP_UAC_ENABLE_RECORD
/* Mic loopback record thread, triggered by the "AT+USBDACMUAC=record" command */
#define COMP_UAC_RECORD_THREAD_PRIORITY               4U
#define COMP_UAC_RECORD_THREAD_STACK_SIZE             (1024U * 4)
/* 1 ms of the 16kHz/2ch/16bit record format */
#define COMP_UAC_RECORD_CHUNK_LEN                     64U
#define COMP_UAC_RECORD_CHUNK_DELAY_MS                1U
#include "example_usbd_composite_cdc_acm_uac_record_audio_data.h"

/* The clip and COMP_UAC_RECORD_CHUNK_LEN are 16kHz/2ch/16bit, the format the mic reports shall match */
#if (USBD_UAC_RECORD_SAMPLING_FREQ != USBD_UAC_SAMPLING_FREQ_16K) || (USBD_UAC_RECORD_CHANNEL_COUNT != USBD_UAC_CHANNEL_COUNT_2) || \
	(USBD_UAC_RECORD_BYTE_WIDTH != USBD_UAC_BYTE_WIDTH_2)
#error "The built-in record clip is 16 kHz, 2 channels, 16-bit: select this record format in menuconfig, or replace the clip"
#endif

/* The record thread reads the table in whole chunks, refer to example_usbd_composite_acm_uac_record_thread() */
_Static_assert((sizeof(uac_record_audio_data) % COMP_UAC_RECORD_CHUNK_LEN) == 0U, "UAC record data len");
#endif

/* Private types -------------------------------------------------------------*/

/* Playback format selected by the host, refer to composite_uac_cb_format_changed() */
typedef struct {
	u32 sampling_freq;
	u8 byte_width;
	u8 channel_count;
} composite_uac_play_format_t;

#if defined(CONFIG_USBD_UAC_PLAY_FEEDBACK) && defined(CONFIG_SUPPORT_AUDIO_FOR_USB)
/* Audio held by the AudioTrack, the water level of the playback feedback: written by the player,
   read by the feedback callback in ISR context */
typedef struct {
	u32 written_frames;      /* Frames written to the track, wraps like the mixer position */
	u32 frame_bytes;
	u32 sampling_freq;
	u32 last_us;             /* Previous raw level, for the two sample average */
	__IO u32 queued_us;      /* Audio the track holds, averaged over two writes */
	__IO u32 target_us;      /* Half of the track buffer, 0 while no track runs */
} uac_play_level_t;
#endif

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int composite_cdc_acm_cb_init(void);
static int composite_cdc_acm_cb_deinit(void);
static int composite_cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int composite_cdc_acm_cb_received(u8 *buf, u32 Len);
static void composite_uac_cb_format_changed(u32 sampling_freq, u8 channel_count, u8 byte_width);
#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
static u32 composite_uac_cb_get_play_feedback(void);
#endif
#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
static void uac_track_write(struct AudioTrack *track, const u8 *buf, u32 len);
#endif
#if defined(CONFIG_USBD_UAC_PLAY_FEEDBACK) && defined(CONFIG_SUPPORT_AUDIO_FOR_USB)
static void uac_play_level_start(u32 frame_bytes, u32 sampling_freq, u32 buf_bytes);
#endif

static int composite_init_stack(void);
#if COMP_HOTPLUG
static void composite_cb_status_changed(u8 old_status, u8 status);
#endif
static void composite_teardown(void);
static void example_audio_track_play(void);
#if COMP_UAC_ENABLE_RECORD
static int composite_cmd(u16 argc, char *argv[]);
static void composite_cmd_at(u16 argc, char **argv);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "COMP";

static const usbd_config_t composite_cfg = {
	.info = {
		.prod_str = "Realtek ACM+UAC Composite Device",
	},
	.isr_priority = INT_PRI_MIDDLE,
	/* Enlarge this value if composite configuration descriptor is larger than 512B */
	/* .ctrl_xfer_buf_len = 512U, */
#if COMP_UAC_ENABLE_RECORD || defined(CONFIG_USBD_UAC_PLAY_FEEDBACK)
	/* The SOF paces the ISOC IN streams: the record packets and the playback feedback */
	.ext_intr_enable = USBD_SOF_INTR,
#endif
#if defined (CONFIG_AMEBASMART)
	.nptx_max_epmis_cnt = 100U,
#elif defined(CONFIG_AMEBAGREEN2) && defined(CONFIG_USBD_UAC_PLAY_FEEDBACK)
	/* TxFIFO5 (EP6) for the feedback EP */
	.rx_fifo_depth = 420U,
	.ptx_fifo_depth = {0U, 256U, 32U, 256U, 16U, },
#elif defined(CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 436U,
	.ptx_fifo_depth = {0U, 256U, 32U, 256U, },
#elif defined(CONFIG_RLE1509) && defined(CONFIG_USBD_UAC_PLAY_FEEDBACK)
	.rx_fifo_depth = 384U,
	.ptx_fifo_depth = {0U, 256U, 32U, 256U, 16U, },
#elif defined(CONFIG_RLE1509)
	.rx_fifo_depth = 400U,
	.ptx_fifo_depth = {0U, 256U, 32U, 256U, },
#elif defined (CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1424U,
	.ptx_fifo_depth = {256U, 32U, 256U, },
#endif
};

/* CDC ACM endpoint configuration */
static const usbd_cdc_acm_config_t cdc_acm_class_cfg = {
	.bulk_in_addr  = COMP_CDC_BULK_IN_EP,
	.bulk_out_addr = COMP_CDC_BULK_OUT_EP,
	.intr_in_addr  = COMP_CDC_INTR_IN_EP,
	.bulk_in_xfer_size  = CDC_BULK_IN_XFER_SIZE,
	.bulk_out_xfer_size = CDC_BULK_OUT_XFER_SIZE,
	.notify_en = 1,
	/* Copy into the class TX buffer: the echo path transmits straight out of the BULK OUT
	   buffer, which is re-armed for the next packet before the IN transfer has completed */
	.bulk_in_zero_copy = 0,
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
static const usbd_uac_config_t uac_class_cfg = {
	.play_ep_addr = COMP_UAC_PLAY_EP,
	.record_ep_addr = COMP_UAC_RECORD_EP,
	.play_feedback_ep_addr = COMP_UAC_PLAY_FEEDBACK_EP,
};

/* UAC user callbacks */
static usbd_uac_cb_t composite_uac_cb = {
	.init = NULL,
	.deinit = NULL,
	.setup = NULL,
	.set_config = NULL,
	.status_changed = NULL,
	.mute_changed = NULL,
	.volume_changed = NULL,
	.format_changed = composite_uac_cb_format_changed,
	.sof = NULL,
#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
	.get_play_feedback = composite_uac_cb_get_play_feedback,
#endif
};

static usb_cdc_acm_line_coding_t composite_cdc_acm_line_coding;

/* UAC audio data buffers and play control */
#define COMP_USBD_AUDIO_MS_BUF_SIZE               1024U
#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
static u8 play_buf[COMP_USBD_AUDIO_MS_BUF_SIZE];
#endif
static u8 recv_buf[COMP_USBD_AUDIO_MS_BUF_SIZE * 2];

static rtos_sema_t uac_ready_sema;
static volatile u8 audio_task_stop;
/* Raised by the hotplug thread for a detach and cleared once the stack is back up: unlike
   audio_task_stop the playback thread never clears it, so a session that starts after the
   detach still leaves and the hotplug thread gets the lock */
static volatile u8 composite_detaching;

/* Serializes the composite stack bring up/tear down against the playback thread,
 * taken by the hotplug thread and by the playback thread for a whole session.
 * Created once and never deleted: the example may be started again, and
 * composite_stack_ready is the gate that keeps callers off the stack. */
static rtos_mutex_t composite_state_lock = NULL;
#if COMP_UAC_ENABLE_RECORD
/* Held by the record thread for a record session, and taken by the hotplug thread after
   composite_state_lock: one lock per direction, so that playback and record run at the same time.
   Created once and never deleted, as composite_state_lock. */
static rtos_mutex_t composite_record_lock = NULL;
#endif
/* 1: the whole composite stack is up, the class APIs may be called. Cleared before any
 * tear down, so a non-zero value also implies composite_state_lock is valid.
 * usbd_uac_deinit() frees the ISOC ring buffer and its semaphores, which
 * usbd_uac_read() dereferences; the bounded wait this replaces could time out and
 * deinit under a playback thread still inside the stack, which is what happens on SMP
 * where that thread keeps running on the other core. */
static volatile u8 composite_stack_ready;

/* Start/stop state: the playback and hotplug threads leave their loops on quit and are
   joined by whoever claimed the stop, nothing ever deletes them from outside */
static usb_app_ctx_t composite_app;

#if COMP_HOTPLUG
static rtos_sema_t composite_attach_status_changed_sema;
/* Written by the ISR, read by the hotplug thread, possibly on another core */
static volatile u8 composite_attach_status;

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
static composite_uac_play_format_t uac_play_format = {
	.sampling_freq = USBD_UAC_SAMPLING_FREQ_48K,
	.byte_width = USBD_UAC_BYTE_WIDTH_2,
	.channel_count = USBD_UAC_CHANNEL_COUNT_2,
};

#if defined(CONFIG_USBD_UAC_PLAY_FEEDBACK) && defined(CONFIG_SUPPORT_AUDIO_FOR_USB)
static uac_play_level_t uac_play_level;
#endif

#if COMP_UAC_ENABLE_RECORD
/* Created once and never deleted: the record command gives it from the shell thread, which
   the example can not join, so it is never provably unreferenced */
static rtos_sema_t uac_record_start_sema;
#endif

/* Exported variables --------------------------------------------------------*/

#if COMP_UAC_ENABLE_RECORD
/* AT commands "AT+<cmd>=<p1>,<p2>...": the command name is case-sensitive, the parameters are not */
ATCMD_TABLE_DATA_SECTION
const log_item_t composite_acm_uac_cmd_table[] = {
	{"+USBDACMUAC", composite_cmd_at},
};
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
	/* Loopback; -RTK_ERR_BUSY means the previous echo is still in flight and this one is dropped */
	return usbd_cdc_acm_transmit(buf, len);
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
	/* Ref USB 2.0 9.2.7: anything not explicitly accepted below is a request error, so
	   the default status makes the core STALL EP0 instead of ACKing the status stage. */
	int ret = -RTK_ERR_BADARG;

	switch (req->bRequest) {
	case USB_CDC_ACM_SET_LINE_CODING:
		/* Ref CDC PSTN 1.2 Table 17: the Line Coding structure is exactly 7 bytes, any
		   other wLength must not update the cached line coding. */
		if (req->wLength == USB_CDC_ACM_LINE_CODING_SIZE) {
			lc->b.dwDteRate = (u32)buf[0] | ((u32)buf[1] << 8) | ((u32)buf[2] << 16) | ((u32)buf[3] << 24);
			lc->b.bCharFormat = buf[4];
			lc->b.bParityType = buf[5];
			lc->b.bDataBits = buf[6];
			ret = RTK_SUCCESS;
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
		ret = RTK_SUCCESS;
		break;

	case USB_CDC_ACM_SET_CONTROL_LINE_STATE:
		/*
		wValue:	wValue, Control Signal Bitmap
				D2-15:	Reserved, 0
				D1:	RTS, 0 - Deactivate, 1 - Activate
				D0:	DTR, 0 - Not Present, 1 - Present
		Ref CDC PSTN 1.2 6.3.12: no data stage, so wLength must be zero; the reserved bits
		are ignored rather than rejected.
		*/
		if (req->wLength == 0U) {
			if ((req->wValue & 0x01U) != 0U) {
				/* VCOM port activate */
				USB_DIAG(USB_LAYER_APP, USB_EVT_LINK, 0);
				usbd_cdc_acm_notify_serial_state(USB_CDC_ACM_CTRL_DSR | USB_CDC_ACM_CTRL_DCD);
			}
			ret = RTK_SUCCESS;
		}
		break;

	case USB_CDC_ACM_SEND_BREAK:
		/* Ref CDC PSTN 1.2 6.3.13: no data stage, so wLength must be zero. Nothing to do */
		if (req->wLength == 0U) {
			ret = RTK_SUCCESS;
		}
		break;

	default:
		/* Request error, keep the default status */
		break;
	}

	return ret;
}

/* playback , USB OUT */
static void example_audio_track_play(void)
{
#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
	composite_uac_play_format_t play_format;
#endif
	u32 read_dat_len = 0;
#ifndef CONFIG_SUPPORT_AUDIO_FOR_USB
	u32 total_len = 0;
	u32 read_cnt = 0;
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "Audio track demo begin\n");

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
	/* One consistent snapshot: format_changed() rewrites the fields from the ISR */
	rtos_critical_enter(RTOS_CRITICAL_USB);
	play_format = uac_play_format;
	rtos_critical_exit(RTOS_CRITICAL_USB);
#endif

	/* Sizes the ISOC OUT ring buffer and arms the endpoint. Required on every build: without
	   it no OUT packet is ever received, so it must not depend on the audio framework. */
	while (usbd_uac_start_playback() != RTK_SUCCESS) {
		if (audio_task_stop || composite_detaching || composite_app.quit) {
			return;
		}
		rtos_time_delay_ms(5);
	}

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
	{
		struct AudioTrack *audio_track;
		uint32_t format;
		int32_t track_buf_size;

		uint32_t g_track_rate = play_format.sampling_freq;
		uint32_t g_track_channel = play_format.channel_count;
		uint32_t g_track_format = play_format.byte_width * 8;

		uint32_t play_track_channel = g_track_channel;

#if COMP_UAC_DEMUX_CH_DEBUG
		uint32_t idx = 0, off = 0;
		/* Keep the front left and right channels of a multichannel stream */
		play_track_channel = MIN(g_track_channel, COMP_UAC_PLAY_MAX_TRACK_CHANNEL);
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

		/* Every failure below stops the ISOC OUT stream armed above and frees the track, so a
		   failed session leaves nothing running or allocated */
		audio_track = AudioTrack_Create();
		if (!audio_track) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create AudioTrack fail\n");
			usbd_uac_stop_playback();
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
		if (AudioTrack_Init(audio_track, &track_config, AUDIO_OUTPUT_FLAG_NONE) != AUDIO_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Audio track init fail\n");
			usbd_uac_stop_playback();
			AudioTrack_Destroy(audio_track);
			return;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Track buf size:%d\n", track_buf_size);

		/*for mixer version, this mean sw volume, for passthrough version, sw volume is not supported*/
		AudioTrack_SetVolume(audio_track, 1.0, 1.0);
		AudioTrack_SetStartThresholdBytes(audio_track, track_buf_size);

		if (AudioTrack_Start(audio_track) != AUDIO_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Audio track start fail\n");
			usbd_uac_stop_playback();
			AudioTrack_Destroy(audio_track);
			return;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "UAC stop %d\n", audio_task_stop);
#if defined(CONFIG_USBD_UAC_PLAY_FEEDBACK) && defined(CONFIG_SUPPORT_AUDIO_FOR_USB)
		uac_play_level_start(play_track_channel * g_track_format / 8U, g_track_rate, (u32)track_buf_size);
#endif

		/* quit is rechecked at least every read timeout, so a stop never waits on the host */
		while (!audio_task_stop && !composite_detaching && !composite_app.quit) {
			read_dat_len = usbd_uac_read(recv_buf, COMP_USBD_AUDIO_MS_BUF_SIZE * 2, 500, NULL);
			if (read_dat_len > 0) {
#if COMP_UAC_DEMUX_CH_DEBUG
				if (play_track_channel == g_track_channel) {
					uac_track_write(audio_track, recv_buf, read_dat_len);
					continue;
				}
				play_data_size = 0;
				//get the 2 channel data from the 4 channel
				/* Whole frames only, and never past play_buf: a short ISOC packet may end mid-frame */
				for (idx = 0, off = 0; ((idx + audio_src_step) <= read_dat_len) && ((off + audio_dst_step) <= sizeof(play_buf));
					 idx += audio_src_step, off += audio_dst_step) {
					// ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3
					// 24  24  24  24  24  24  24  24  24  24  24  24
					memcpy((void *)(play_buf + off), (const void *)(recv_buf + idx), audio_dst_step);
					play_data_size += audio_dst_step;
				}

				uac_track_write(audio_track, play_buf, play_data_size);
#else
				uac_track_write(audio_track, recv_buf, read_dat_len);
#endif
			}
		}

		usbd_uac_stop_playback();

#if defined(CONFIG_USBD_UAC_PLAY_FEEDBACK) && defined(CONFIG_SUPPORT_AUDIO_FOR_USB)
		/* Back to the OUT ring level */
		uac_play_level.target_us = 0U;
#endif
		AudioTrack_Pause(audio_track);
		AudioTrack_Flush(audio_track);
		AudioTrack_Stop(audio_track);
		AudioTrack_Destroy(audio_track);

		audio_track = NULL;
	}
#else
	/* No audio framework on this SoC: keep draining the ISOC OUT stream so the host
	   still sees a working speaker endpoint, and report the throughput instead. */
	while (!audio_task_stop && !composite_detaching && !composite_app.quit) {
		read_dat_len = usbd_uac_read(recv_buf, COMP_USBD_AUDIO_MS_BUF_SIZE * 2, 500, NULL);
		read_cnt++;
		if (read_dat_len > 0) {
			total_len += read_dat_len;
			if ((read_cnt % 200U) == 0U) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Audio track get %d %d\n", read_dat_len, total_len);
			}
		} else {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Audio read timeout\n");
			rtos_time_delay_ms(1);
		}
	}

	usbd_uac_stop_playback();
#endif
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Audio track demo stop\n\n\n");
}

#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
/**
  * @brief  Report the playback rate the device consumes, sent to the host once per ms
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  *         A water level control within 0.1% of the nominal rate: a fuller buffer asks the host
  *         for less, an emptier one for more. While a track runs the level is the audio the
  *         AudioTrack holds plus the OUT ring, kept at half the track buffer: the player drains
  *         the ring at once and blocks in AudioTrack_Write(). Without a track (no audio
  *         framework) the OUT ring alone is kept half full.
  *         A proportional control only: when the device clock differs from the nominal rate the
  *         level settles off the target, by the share of the 0.1% range the clock error takes
  *         (e.g. 30 ms above it for a clock 170 ppm slow). An application that needs the level
  *         on the target adds an integral term, the accumulated level error, to the formula.
  * @retval Frames per ms in 16.16 fixed point
  */
static u32 composite_uac_cb_get_play_feedback(void)
{
	/* 1/1024 of the nominal rate, about 0.1% */
	u32 nominal = (u32)(((u64)uac_play_format.sampling_freq << USBD_UAC_PLAY_FEEDBACK_FRACTION_BITS) / 1000U);
	s32 max_delta = (s32)(nominal >> 10);
	s32 target_us = (s32)(USBD_UAC_DEFAULT_PLAY_BUF_COUNT * 1000U / 2U);
	s32 level_us = (s32)usbd_uac_get_read_frame_time_us();
	s32 diff;

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
	if (uac_play_level.target_us != 0U) {
		target_us = (s32)uac_play_level.target_us;
		level_us += (s32)uac_play_level.queued_us;
	}
#endif

	diff = target_us - level_us;
	if (diff > target_us) {
		diff = target_us;
	} else if (diff < -target_us) {
		diff = -target_us;
	}

	return (u32)((s32)nominal + (s32)(((s64)max_delta * diff) / target_us));
}
#endif

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
/**
  * @brief  Write audio to the track, and with the playback feedback publish the audio it holds
  * @param  track: Audio track
  * @param  buf: Audio data
  * @param  len: Length in bytes
  * @retval None
  */
static void uac_track_write(struct AudioTrack *track, const u8 *buf, u32 len)
{
#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
	int32_t ret;
	uint64_t pos = 0;

	ret = AudioTrack_Write(track, buf, len, true);
	if (ret > 0) {
		uac_play_level.written_frames += (u32)ret / uac_play_level.frame_bytes;
	}
	/* The frames the track consumed, the rest is still queued */
	ret = AudioTrack_GetPosition(track, &pos);
	if (ret == AUDIO_OK) {
		/* The mixer position moves in blocks: average two samples to smooth the steps out */
		u32 us = (u32)(((u64)(uac_play_level.written_frames - (u32)pos) * 1000000U) / uac_play_level.sampling_freq);
		uac_play_level.queued_us = (us + uac_play_level.last_us) / 2U;
		uac_play_level.last_us = us;
	}
#else
	AudioTrack_Write(track, buf, len, true);
#endif
}
#endif

#if defined(CONFIG_USBD_UAC_PLAY_FEEDBACK) && defined(CONFIG_SUPPORT_AUDIO_FOR_USB)
/**
  * @brief  Start publishing the audio held by a new track for the playback feedback
  * @param  frame_bytes: Bytes per frame written to the track
  * @param  sampling_freq: Sampling frequency of the track
  * @param  buf_bytes: Size of the track buffer
  * @retval None
  */
static void uac_play_level_start(u32 frame_bytes, u32 sampling_freq, u32 buf_bytes)
{
	uac_play_level.written_frames = 0U;
	uac_play_level.frame_bytes = frame_bytes;
	uac_play_level.sampling_freq = sampling_freq;
	uac_play_level.last_us = 0U;
	uac_play_level.queued_us = 0U;
	uac_play_level.target_us = (u32)(((u64)(buf_bytes / frame_bytes) * 1000000U) / sampling_freq / 2U);
}
#endif

/**
  * @brief  Audio format change notification from the USB host
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  sampling_freq: New sampling frequency
  * @param  channel_count: New channel count
  * @param  byte_width: New sample byte width
  * @retval void
  */
static void composite_uac_cb_format_changed(u32 sampling_freq, u8 channel_count, u8 byte_width)
{
	/* The player thread snapshots the format under the same lock, refer to example_audio_track_play() */
	rtos_critical_enter(RTOS_CRITICAL_USB);
	if (sampling_freq != 0U) {
		uac_play_format.sampling_freq = sampling_freq;
	}
	if (channel_count != 0U) {
		uac_play_format.channel_count = channel_count;
	}
	if (byte_width != 0U) {
		uac_play_format.byte_width = byte_width;
	}
	rtos_critical_exit(RTOS_CRITICAL_USB);

	if (sampling_freq && channel_count && byte_width) {
		/* Stop the current stream so usbd_uac_read returns at once (next_xfer=0) and the
		 * player thread restarts with the new format, re-arming the ISOC OUT EP. Without
		 * this the read loop live-locks on stale data after a mid-stream rate change. */
		audio_task_stop = 1;
		usbd_uac_stop_playback();
		rtos_sema_give(uac_ready_sema);
	}
}

/**
  * @brief  Bring up the whole composite stack (core + each class + framework).
  * @note   Reused by both the initial start-up and the hotplug re-init path.
  * @retval RTK_SUCCESS on success, other HAL_Status code on failure (all partial resources rolled back)
  */
static int composite_init_stack(void)
{
	int ret;

	ret = usbd_init(&composite_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init fail\n");
		return ret;
	}

	ret = usbd_composite_cdc_acm_init(&composite_cdc_acm_usr_cb, &cdc_acm_class_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "CDC ACM init fail\n");
		goto exit_usbd_init;
	}

	ret = usbd_composite_uac_init(&composite_uac_cb, &uac_class_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC init fail\n");
		goto exit_cdc_acm_init;
	}

	ret = usbd_composite_init(COMP_CB);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Composite init fail\n");
		goto exit_uac_init;
	}

	return RTK_SUCCESS;

exit_uac_init:
	usbd_uac_deinit();
exit_cdc_acm_init:
	usbd_cdc_acm_deinit();
exit_usbd_init:
	usbd_deinit();
	return ret;
}

/**
  * @brief  Tear down the whole composite stack, in the reverse order of
  *         composite_init_stack(): framework -> classes -> core.
  * @note   The playback loop must already be stopped (the stop owner joins it, or the
  *         hotplug thread does that) so the UAC class can be torn down safely.
  *         No return value: a teardown failure has no recoverable path, so the
  *         only sequence of release calls in this example lives here.
  * @retval None
  */
static void composite_deinit_stack(void)
{
	usbd_composite_deinit();
	usbd_uac_deinit();
	usbd_cdc_acm_deinit();
	usbd_deinit();
}

/**
  * @brief  Wake the data threads of a run whose quit flag is already raised, so that they leave
  * @retval None
  */
static void composite_wake_workers(void)
{
	rtos_sema_give(uac_ready_sema);
#if COMP_UAC_ENABLE_RECORD
	rtos_sema_give(uac_record_start_sema);
#endif
}

#if COMP_HOTPLUG
/**
  * @brief  Take every lock that guards a call into the stack
  * @note   Always in this order, the only place more than one is held at a time
  * @retval None
  */
static void composite_lock_all(void)
{
	rtos_mutex_take(composite_state_lock, RTOS_MAX_TIMEOUT);
#if COMP_UAC_ENABLE_RECORD
	rtos_mutex_take(composite_record_lock, RTOS_MAX_TIMEOUT);
#endif
}

/**
  * @brief  Release the locks taken by composite_lock_all()
  * @retval None
  */
static void composite_unlock_all(void)
{
#if COMP_UAC_ENABLE_RECORD
	rtos_mutex_give(composite_record_lock);
#endif
	rtos_mutex_give(composite_state_lock);
}

/**
  * @brief  Composite attach-status change notification (ISR context).
  * @note   time-consuming operations are not permitted here.
  */
static void composite_cb_status_changed(u8 old_status, u8 status)
{
	UNUSED(old_status);
	composite_attach_status = status;
	rtos_sema_give(composite_attach_status_changed_sema);
}

/* Tear down and re-init the whole composite stack on cable detach, to avoid
   memory leak across repeated plug/unplug. The audio playback loop is stopped
   first so the UAC class can be torn down safely. */
static void example_usbd_composite_hotplug_thread(void *param)
{
	u8 fatal = 0U;

	UNUSED(param);

#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* C-2: the USB OTG ISR is delivered on CPU0 (GIC ITARGETSR pins every SPI to core 0).
	   Pinning the hotplug/deinit thread to CPU0 puts it on the same core as the ISR,
	   so deinit's local interrupt disable is meaningful again under SMP. The playback
	   data thread stays unaffined: a pending deinit is excluded from it by
	   composite_state_lock, not by core.
	   Done by the thread itself, first: a created thread may already run on another core, and its
	   creator pinning it afterwards could come too late or reach a task that has deleted itself. */
	rtos_task_set_affinity(rtos_task_handle_get(), 0);
#endif

	while (composite_app.quit == 0U) {
		if (rtos_sema_take(composite_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (composite_app.quit != 0U) {
				/* Stop requested: the stop owner tears the stack down */
				break;
			}
			if (composite_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");
				/* Ask the playback loop to leave first: usbd_uac_stop_playback() unblocks a pending
				   usbd_uac_read(), audio_task_stop ends the loop. Both must run while
				   the stack is still up, so before the lock. */
				composite_detaching = 1U;
				audio_task_stop = 1;
				usbd_uac_stop_playback();
				/* The playback thread holds this lock for as long as it is inside the
				   class APIs, so taking it here waits until it is not: unlike a bounded
				   wait, this can not time out and deinit under a thread that is still
				   running, which matters on SMP where it runs on the other core. */
				composite_lock_all();
				composite_stack_ready = 0;
				composite_deinit_stack();
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				if (composite_init_stack() != RTK_SUCCESS) {
					composite_unlock_all();
					fatal = 1U;
					break;
				}
				composite_stack_ready = 1;
				composite_detaching = 0U;
				composite_unlock_all();
			} else if (composite_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread exited\n");

	/* composite_init_stack() rolled back everything it had brought up and composite_stack_ready
	   stays 0, so whoever owns the teardown does not deinit the stack a second time */
	if ((fatal != 0U) && (usb_app_ctx_claim_stop(&composite_app) != 0U)) {
		/* No stop is in flight: stop the data threads, then finish the teardown here */
		composite_wake_workers();
		if (usb_app_ctx_join(&composite_app, COMP_WORKER_CNT - 1U) != RTK_SUCCESS) {
			/* A data thread is still running: keep everything, the app stays STOPPING */
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Wait worker exit timeout\n");
			rtos_task_delete(NULL);
		}
		composite_teardown();
		usb_app_ctx_end(&composite_app);
		rtos_task_delete(NULL);
	}

	usb_app_ctx_leave(&composite_app);
	rtos_task_delete(NULL);
}
#endif // COMP_HOTPLUG

static void example_usbd_composite_acm_uac_audio_track_thread(void *param)
{
	UNUSED(param);

	while (composite_app.quit == 0U) {
		if (rtos_sema_take(uac_ready_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			break;
		}
		if (composite_app.quit != 0U) {
			break;
		}
		audio_task_stop = 0;
		/* Hold the lock for the whole playback session: it keeps a pending teardown
		   out of the class APIs this thread is inside. The hotplug thread raises
		   audio_task_stop before it asks for the lock, so the loop inside always
		   leaves and the wait is bounded. */
		rtos_mutex_take(composite_state_lock, RTOS_MAX_TIMEOUT);
		if (composite_stack_ready != 0U) {
			example_audio_track_play();
		}
		rtos_mutex_give(composite_state_lock);
	}

	/* The AudioTrack of a session is already destroyed by example_audio_track_play().
	   uac_ready_sema is owned by the teardown: the format_changed callback gives it
	   from ISR context, so this thread must never free it. */
	usb_app_ctx_leave(&composite_app);
	rtos_task_delete(NULL);
}

#if COMP_UAC_ENABLE_RECORD
/* Mic loopback record: replays uac_record_audio_data[] as the mic input, once triggered */
static void example_usbd_composite_acm_uac_record_thread(void *param)
{
	u32 offset;
	u32 tx_fail_cnt;

	UNUSED(param);

	while (composite_app.quit == 0U) {
		if (rtos_sema_take(uac_record_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			break;
		}
		if (composite_app.quit != 0U) {
			break;
		}

		/* Hold the lock for the whole record session: it keeps a pending teardown out of the
		   class APIs this thread is inside. composite_detaching is raised before the lock is
		   asked for, so the loop below always leaves. */
		rtos_mutex_take(composite_record_lock, RTOS_MAX_TIMEOUT);
		if (composite_stack_ready == 0U) {
			rtos_mutex_give(composite_record_lock);
			continue;
		}

		/* Fails before the host has configured the device: the record ring would stay unsized */
		if (usbd_uac_start_record() != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC start record fail\n");
			rtos_mutex_give(composite_record_lock);
			continue;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "UAC record start\n");
		offset = 0U;
		tx_fail_cnt = 0U;
		while ((composite_app.quit == 0U) && (composite_detaching == 0U)) {
			/* One ms of audio per call, the class cuts it into per-(micro)frame packets. A live
			   stream tolerates a lost chunk, so keep the 1 ms pace and only report the first
			   failure of a run, to stay off the console at 1 kHz. */
			if (usbd_uac_write(&uac_record_audio_data[offset], COMP_UAC_RECORD_CHUNK_LEN) < 0) {
				if (tx_fail_cnt == 0U) {
					RTK_LOGS(TAG, RTK_LOG_WARN, "UAC record tx fail\n");
				}
				tx_fail_cnt++;
			} else {
				tx_fail_cnt = 0U;
			}
			offset += COMP_UAC_RECORD_CHUNK_LEN;
			if (offset >= uac_record_data_len) {
				offset = 0U;
			}
			rtos_time_delay_ms(COMP_UAC_RECORD_CHUNK_DELAY_MS);
		}
		usbd_uac_stop_record();
		rtos_mutex_give(composite_record_lock);
	}

	usb_app_ctx_leave(&composite_app);
	rtos_task_delete(NULL);
}

static int composite_cmd(u16 argc, char *argv[])
{
	const char *cmd = (argc != 0U) ? argv[0] : NULL;

	if ((cmd == NULL) || (_stricmp(cmd, "help") == 0) || (_stricmp(cmd, "?") == 0)) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Usage: AT+USBDACMUAC=<cmd>\n");
		RTK_LOGS(TAG, RTK_LOG_INFO, "  record   Loop the built-in \"Realtek\" clip on the mic (USB IN) stream once the host\n");
		RTK_LOGS(TAG, RTK_LOG_INFO, "           configured it; each call queues one more session\n");
		RTK_LOGS(TAG, RTK_LOG_INFO, "  help     Print this help\n");
		return (cmd == NULL) ? -RTK_ERR_BADARG : RTK_SUCCESS;
	}

	/* Refused while the app is not running, a hotplug reinit is fine */
	if (composite_app.state != USB_APP_STATE_RUNNING) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Not running\n");
		return -RTK_ERR_IO;
	}

	if (_stricmp(cmd, "record") == 0) {
		if (argc != 1U) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Usage: AT+USBDACMUAC=record\n");
			return -RTK_ERR_BADARG;
		}
		/* NULL only if the example never created it; once created it is never deleted, so the
		   handle cannot be freed under this shell thread */
		if ((uac_record_start_sema == NULL) || (rtos_sema_give(uac_record_start_sema) != RTK_SUCCESS)) {
			return -RTK_ERR_IO;
		}
		return RTK_SUCCESS;
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cmd: %s, try AT+USBDACMUAC=help\n", cmd);
	return -RTK_ERR_BADARG;
}

/* AT entry: argv[0] is reserved by the AT parser and argv[1] holds the first parameter */
static void composite_cmd_at(u16 argc, char **argv)
{
	int ret = composite_cmd((argc != 0U) ? (u16)(argc - 1U) : 0U, &argv[1]);

	if (ret == RTK_SUCCESS) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, (int)ret);
	}
}
#endif /* COMP_UAC_ENABLE_RECORD */

/**
  * @brief  Release the stack if still up and the objects of one run
  * @note   Only called once every worker is gone or was never created. A failed hotplug
  *         reinit already left the stack down with composite_stack_ready cleared, so it is
  *         not deinited twice. The semaphores are deleted here only: the format_changed /
  *         status_changed ISR callbacks give them while the stack is up.
  * @retval None
  */
static void composite_teardown(void)
{
	rtos_mutex_take(composite_state_lock, RTOS_MAX_TIMEOUT);
	if (composite_stack_ready != 0U) {
		composite_stack_ready = 0;
		composite_deinit_stack();
	}
	rtos_mutex_give(composite_state_lock);

#if COMP_HOTPLUG
	rtos_sema_delete(composite_attach_status_changed_sema);
	composite_attach_status_changed_sema = NULL;
#endif
	rtos_sema_delete(uac_ready_sema);
	uac_ready_sema = NULL;
}

/**
  * @brief  Bring the stack, the playback thread and the hotplug thread up
  * @retval RTK_SUCCESS, or the first error, with everything already created rolled back
  */
static int composite_start(void)
{
	int ret;
	rtos_task_t task;

	ret = usb_app_ctx_begin(&composite_app);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	/* Created once and never deleted, see their declaration. */
	if (composite_state_lock == NULL) {
		ret = rtos_mutex_create(&composite_state_lock);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create lock fail\n");
			goto exit;
		}
	}
#if COMP_UAC_ENABLE_RECORD
	if (composite_record_lock == NULL) {
		ret = rtos_mutex_create(&composite_record_lock);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create lock fail\n");
			goto exit;
		}
	}
	if (uac_record_start_sema == NULL) {
		ret = rtos_sema_create(&uac_record_start_sema, 0U, RTOS_SEMA_MAX_COUNT);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
			goto exit;
		}
	}
	/* Drop the wake up given by the stop of a previous run, it would start an unrequested session */
	while (rtos_sema_take(uac_record_start_sema, 0U) == RTK_SUCCESS) {
	}
#endif

	ret = rtos_sema_create(&uac_ready_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto exit;
	}
	/* Raised by the hotplug thread or the format_changed callback of a previous run */
	audio_task_stop = 0;
	composite_detaching = 0U;

#if COMP_HOTPLUG
	ret = rtos_sema_create(&composite_attach_status_changed_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto exit_teardown;
	}
#endif

	ret = composite_init_stack();
	if (ret != RTK_SUCCESS) {
		goto exit_teardown;
	}
	composite_stack_ready = 1;

	/* Created from the start context (the init thread in standalone, after its own
	 * CPU1-bring-up delay has already elapsed) rather than from the example entry,
	 * because a task's first blocking wait (sema_take/mutex/delay) hitting the same
	 * narrow boot window corrupts the FreeRTOS blocked-list bookkeeping. */
	ret = rtos_task_create(&task, "usbd_composite_playback_thread",
						   example_usbd_composite_acm_uac_audio_track_thread, NULL,
						   COMP_UAC_THREAD_STACK_SIZE,
						   COMP_UAC_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create audio track fail\n");
		goto exit_teardown;
	}

#if COMP_UAC_ENABLE_RECORD
	ret = rtos_task_create(&task, "usbd_composite_record_thread",
						   example_usbd_composite_acm_uac_record_thread, NULL,
						   COMP_UAC_RECORD_THREAD_STACK_SIZE,
						   COMP_UAC_RECORD_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create record thread fail\n");
		/* Stop the playback thread first. Claiming the stop always wins here (no worker can
		   claim yet) and raises quit. */
		usb_app_ctx_claim_stop(&composite_app);
		composite_wake_workers();
		if (usb_app_ctx_join(&composite_app, 1U) != RTK_SUCCESS) {
			/* Still running: keep everything, the app stays STOPPING */
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Wait playback exit timeout\n");
			return -RTK_ERR_TIMEOUT;
		}
		goto exit_teardown;
	}
#endif

#if COMP_HOTPLUG
	/* Created last: the hotplug thread is the only one that tears the stack down on its
	   own, so once it runs nothing is left to unwind here */
	ret = rtos_task_create(&task, "usbd_composite_hotplug_thread",
						   example_usbd_composite_hotplug_thread, NULL,
						   COMP_HOTPLUG_THREAD_STACK_SIZE, COMP_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
		/* Stop the data threads first: they block on their semaphores and touch the UAC class.
		   Claiming the stop always wins here (no worker can claim yet) and raises quit. */
		usb_app_ctx_claim_stop(&composite_app);
		composite_wake_workers();
		if (usb_app_ctx_join(&composite_app, COMP_WORKER_CNT - 1U) != RTK_SUCCESS) {
			/* Still running: keep everything, the app stays STOPPING */
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Wait worker exit timeout\n");
			return -RTK_ERR_TIMEOUT;
		}
		goto exit_teardown;
	}
#endif

	return RTK_SUCCESS;

exit_teardown:
	composite_teardown();
exit:
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBD ACM+UAC comp demo aborted\n");
	usb_app_ctx_end(&composite_app);

	return ret;
}

/* Init thread: bring up the composite stack and start the workers */
static void example_usbd_composite_acm_uac_init_thread(void *param)
{
	UNUSED(param);

#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* A start that fails rolls back through usbd_deinit(), which waits for no ISR on another
	   core: run it on CPU0, where the USB IRQ is delivered, as the hotplug thread does.
	   Done by the thread itself, first: a created thread may already run on another core, and its
	   creator pinning it afterwards could come too late or reach a task that has deleted itself. */
	rtos_task_set_affinity(rtos_task_handle_get(), 0);
#endif

	if (composite_start() != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Start fail\n");
	}
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Start the ACM+UAC composite device, used by the USB app manager
  * @note   Task context only
  * @retval RTK_SUCCESS, -RTK_ERR_BUSY if already running, or the init error
  */
int example_usbd_composite_cdc_acm_uac_start(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD ACM+UAC comp demo start\n");

	return composite_start();
}

/**
  * @brief  Stop the ACM+UAC composite device and release everything it owns
  * @note   Task context only. Safe to call when not running. The playback thread leaves
  *         within one usbd_uac_read() timeout and destroys its AudioTrack itself.
  * @retval RTK_SUCCESS once idle, -RTK_ERR_TIMEOUT if a worker did not leave in time
  */
int example_usbd_composite_cdc_acm_uac_stop(void)
{
	if (usb_app_ctx_claim_stop(&composite_app) == 0U) {
		/* Idle, or the hotplug thread found the stack dead and finishes the teardown itself */
		return usb_app_ctx_wait_idle(&composite_app);
	}

	composite_wake_workers();
#if COMP_HOTPLUG
	rtos_sema_give(composite_attach_status_changed_sema);
#endif
	if (usb_app_ctx_join(&composite_app, COMP_WORKER_CNT) != RTK_SUCCESS) {
		/* A worker is still on the stack: keep everything, the app stays STOPPING */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stop timeout\n");
		return -RTK_ERR_TIMEOUT;
	}

	composite_teardown();
	usb_app_ctx_end(&composite_app);
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD ACM+UAC comp demo stop\n");

	return RTK_SUCCESS;
}

/**
  * @brief  Example entry: composite CDC ACM + UAC device
  * @param  None
  * @retval None
  */
void example_usbd_composite_cdc_acm_uac(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD ACM+UAC comp demo start\n");

	ret = rtos_task_create(&task, "usbd_composite_init_thread",
						   example_usbd_composite_acm_uac_init_thread, NULL,
						   COMP_INIT_THREAD_STACK_SIZE,
						   COMP_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create init thread fail\n");
	}
}
