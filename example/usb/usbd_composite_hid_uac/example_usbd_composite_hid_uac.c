/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */
#include <platform_autoconf.h>
#include "usbd_composite_hid_uac.h"
#include "os_wrapper.h"
#include "ameba_soc.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "example_usbd_composite_hid_uac_record_audio_data.h"

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
static const char *const TAG = "COMP";

#define CONFIG_USBD_COMPOSITE_HOTPLUG                            1

#define CONFIG_USBD_HID_CONSTANT_LOOP                            2
#define CONFIG_USBD_COMP_HID_TX_BUG_LEN                          1024

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
#define CONFIG_USBD_UAC_DEMUX_CH_DEBUG                           1
#else
#define CONFIG_USBD_UAC_DEMUX_CH_DEBUG                           0
#endif

#define CONFIG_COMP_USBD_MSG_QUEUE_DEPTH                         10
#define COMP_USBD_HOT_PLUG_STATUS                                1

#define CONFIG_COMP_USBD_UAC_RECORD_TEST_CNT                     6
#define CONFIG_COMP_USBD_UAC_RECORD_TEST_TIME                    10

/* Task exit wait timeout */
#define TASK_EXIT_WAIT_TIMEOUT_MS                                5000
#define TASK_EXIT_POLL_INTERVAL_MS                               100

/* USB speed */
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define CONFIG_USBD_COMPOSITE_SPEED USB_SPEED_FULL
#elif defined(CONFIG_USBD_COMPOSITE_HID_UAC1)
/* UAC 1.0 spec supports only Full Speed. */
#define CONFIG_USBD_COMPOSITE_SPEED USB_SPEED_HIGH_IN_FULL
#else
#define CONFIG_USBD_COMPOSITE_SPEED USB_SPEED_HIGH
/* At present, audio recording functionality is only supported by the UAC 2.0 interface. */
#define ENABLE_USB_RECORD_FEATURE                                1
#endif

/* Thread priorities */
#define CONFIG_USBD_COMPOSITE_HID_THREAD_PRIORITY                5U
#define CONFIG_USBD_COMPOSITE_UAC_THREAD_PRIORITY                6U
#define CONFIG_USBD_COMPOSITE_INIT_THREAD_PRIORITY               7U
#define CONFIG_USBD_COMPOSITE_EVENT_TASK_PRIORITY                8U

/* Buffer sizes */
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define COMP_USBD_AUDIO_MS_BUF_SIZE               1023U
#else
#define COMP_USBD_AUDIO_MS_BUF_SIZE               1024U
#endif
#define COMP_USBD_AUDIO_PLAYER_BUF_SIZE           ((COMP_USBD_AUDIO_MS_BUF_SIZE) * 5)

/* Private types -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void composite_cb_status_changed(u8 old_status, u8 status);
static int composite_cb_set_config(void);

/* HID */
static int composite_hid_cb_init(void);
static int composite_hid_cb_deinit(void);
static int composite_hid_cb_setup(usb_setup_req_t *req, u8 *buf);
static int composite_hid_cb_set_config(void);
static int composite_hid_cb_received(u8 *buf, u32 Len);
static int composite_hid_cb_transmitted(u8 status);

/* UAC */
static int composite_uac_cb_init(void);
static int composite_uac_cb_deinit(void);
static int composite_uac_cb_set_config(void);
static int composite_uac_cb_mute_changed(u8 mute);
static int composite_uac_cb_volume_changed(u8 volume);
static int composite_uac_cb_format_changed(u32 sampling_freq, u8 ch_cnt, u8 byte_width);

/* Private variables ---------------------------------------------------------*/
/* USB Common */
#if CONFIG_USBD_COMPOSITE_HOTPLUG
static __IO u8 composite_attach_status;
#endif

static usb_os_queue_t msg_queue;
static __IO u8 usb_ready_flag = 0;           /* USB ready */

/* HID */
static __IO u8 usbd_comp_hid_ready = 0;      /* HID ready */
static __IO u8 hid_tx_xfer_done = 0;         /* HID tx xfer done */
static u8 hid_report_buf[CONFIG_USBD_COMP_HID_TX_BUG_LEN];    /* HID tx buffer */
static u8 hid_rx_buf[USBD_COMP_HID_MAX_BUF_SIZE];             /* HID rx buffer */

/*  UAC */
static __IO u8 usbd_comp_audio_ready = 0;         /* UAC ready: playback & record */
/*  UAC play */
static __IO u8 usbd_comp_audio_play_stop = 0;     /* Audio play stop */
static __IO u8 usbd_comp_audio_play_task_alive = 0;    /* Audio play task */

/* UAC record */
#if defined(ENABLE_USB_RECORD_FEATURE)
static __IO u8 usbd_comp_user_enable_uac_record = 0;    /* User enable uac record function */
static __IO u8 usbd_comp_audio_record_stop = 0;         /* Audio record stop */
static __IO u8 usbd_comp_audio_record_task_alive = 0;   /* Audio record task */

static int usbh_uac_busy_count;
static int usbh_uac_err_count;
#endif

#if CONFIG_USBD_UAC_DEMUX_CH_DEBUG
/*
    the buffer which will write to the audio track
    the basic length is COMP_USBD_AUDIO_PLAYER_BUF_SIZE
*/
static u8 composite_usbd_uac_play_buf[COMP_USBD_AUDIO_PLAYER_BUF_SIZE];
#endif

/*
    uac isoc rx buffer, used to save the audio data that received from the host
    the recv_buffer length is 2*COMP_USBD_AUDIO_PLAYER_BUF_SIZE
    the play task will write the COMP_USBD_AUDIO_PLAYER_BUF_SIZE length data to audiotack once
*/
static u8 composite_usbd_uac_recv_buf[COMP_USBD_AUDIO_PLAYER_BUF_SIZE];

/* Callback structures */
static usbd_composite_cb_t composite_usbd_cb = {
	.status_changed = composite_cb_status_changed,
	.set_config = composite_cb_set_config,
};

static usbd_config_t composite_usbd_cfg = {
	.speed = CONFIG_USBD_COMPOSITE_SPEED,
	.isr_priority = INT_PRI_HIGHEST,
	.isr_in_critical = 0,
	.intr_use_ptx_fifo = 0U,
	.ext_intr_enable        = USBD_EPMIS_INTR | USBD_EOPF_INTR | USBD_SOF_INTR,
	.nptx_max_epmis_cnt = 100U,
#if defined (CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 420U,
	.ptx_fifo_depth = {16U, 256U, 32U, 256U, },
#elif defined (CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1424U,
	.ptx_fifo_depth = {256U, 32U, 256U, },
#endif
};

/*  HID */
static usbd_composite_hid_usr_cb_t composite_hid_usr_cb = {
	.init = composite_hid_cb_init,
	.deinit = composite_hid_cb_deinit,
	.set_config = composite_hid_cb_set_config,
	.setup = composite_hid_cb_setup,
	.transmitted = composite_hid_cb_transmitted,
};

/*  UAC */
static usbd_composite_uac_usr_cb_t composite_uac_usr_cb = {
	.audio_ctx = NULL,
	.in = {
#if defined(ENABLE_USB_RECORD_FEATURE)
		.enable = 1,
		.sampling_freq = USBD_UAC_IN_DEFAULT_SAMPLING_FREQ,
		.byte_width = USBD_UAC_BYTE_WIDTH_2,
		.ch_cnt = USBD_UAC_IN_DEFAULT_CH_CNT,
#else
		.enable = 0,
#endif
	},
	.out = {
		.enable = 1,
	},
	.init = composite_uac_cb_init,
	.deinit = composite_uac_cb_deinit,
	.set_config = composite_uac_cb_set_config,
	.mute_changed = composite_uac_cb_mute_changed,
	.volume_changed = composite_uac_cb_volume_changed,
	.format_changed = composite_uac_cb_format_changed,
};

/* Private functions ---------------------------------------------------------*/
/**
 * @brief Wait for task to exit with timeout
 * @param task_alive_flag: Pointer to task alive flag
 * @param timeout_ms: Timeout in milliseconds
 * @return 0 if success, -1 if timeout
 */
static int composite_wait_task_exit(volatile u8 *task_alive_flag, u32 timeout_ms)
{
	u32 elapsed = 0;

	while (*task_alive_flag == 1 && elapsed < timeout_ms) {
		usb_os_sleep_ms(TASK_EXIT_POLL_INTERVAL_MS);
		elapsed += TASK_EXIT_POLL_INTERVAL_MS;
	}

	if (*task_alive_flag == 1) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Task exit timeout\n");
		return -1;
	}

	return 0;
}

/**
 * @brief Stop all tasks for hotplug handling
 */
static void composite_stop_all_tasks(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Stop all tasks\n");

	/* Stop HID task */
	usbd_comp_hid_ready = 0;

	/* Stop UAC */
	usbd_comp_audio_ready = 0;

	/* Stop UAC play task */
	usbd_comp_audio_play_stop = 1;
	usbd_composite_uac_stop_play();

#if defined(ENABLE_USB_RECORD_FEATURE)
	/* Stop UAC record task */
	usbd_comp_user_enable_uac_record = 0;
	usbd_comp_audio_record_stop = 1;
	usbd_composite_uac_stop_record();

	/* Wait for record task exit */
	if (composite_wait_task_exit(&usbd_comp_audio_record_task_alive, TASK_EXIT_WAIT_TIMEOUT_MS) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Record task exit fail\n");
	}
#endif

	/* Wait for play task exit */
	if (composite_wait_task_exit(&usbd_comp_audio_play_task_alive, TASK_EXIT_WAIT_TIMEOUT_MS) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Play task exit fail\n");
	}
}

/**
 * @brief USB status change callback
 */
static void composite_cb_status_changed(u8 old_status, u8 status)
{
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Status change: %d-%d-%d\n", old_status, status, usb_ready_flag);
#if CONFIG_USBD_COMPOSITE_HOTPLUG
	composite_attach_status = status;
	if ((USBD_ATTACH_STATUS_DETACHED == status) && (usb_ready_flag)) {
		u32 msg = COMP_USBD_HOT_PLUG_STATUS;
		usb_os_queue_send(msg_queue, &msg, 0);
	}
#endif
}

/**
 * @brief USB set config callback
 */
static int composite_cb_set_config(void)
{
	usb_ready_flag = 1;

	return HAL_OK;
}

/**
 * @brief Reinitialize USB device
 */
static void composite_do_reinit(void)
{
	int ret;

	usb_ready_flag = 0;
	usbd_composite_deinit();
	ret = usbd_deinit();
	if (ret != HAL_OK) {
		return;
	}

	rtos_time_delay_ms(100);

	RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
	ret = usbd_init(&composite_usbd_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail\n");
		return ;
	}

	ret = usbd_composite_init(&composite_hid_usr_cb,
							  &composite_uac_usr_cb,
							  &composite_usbd_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Comp init fail\n");
		usbd_deinit();
	}
}

/**
 * @brief Event handling task
 */
static void composite_handle_event_task(void *param)
{
	UNUSED(param);
	u32 msg;
	int ret;

	for (;;) {
		ret = usb_os_queue_receive(msg_queue, &msg, 0xFFFFFFFF);
		if (ret != HAL_OK) {
			continue;
		}

		switch (msg) {
#if CONFIG_USBD_COMPOSITE_HOTPLUG
		case COMP_USBD_HOT_PLUG_STATUS:
			if (composite_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "USB detached\n");
				composite_stop_all_tasks();
				composite_do_reinit();
			}
			break;
#endif
		default:
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Unknown event %d\n", msg);
			break;
		}
	}

	rtos_task_delete(NULL);
}

/**
  * @brief  Initializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int composite_hid_cb_init(void)
{
	return HAL_OK;
}

/**
  * @brief  DeInitializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int composite_hid_cb_deinit(void)
{
	return HAL_OK;
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface through this function.
  * @param  Buf: RX buffer
  * @param  Len: RX data length (in bytes)
  * @retval Status
  */
static int composite_hid_cb_received(u8 *buf, u32 len)
{
	u32 i = 0;
	u8 dump_len = len;
	RTK_LOGS(TAG, RTK_LOG_INFO, "HID RX Len %d\n", len);
	if (dump_len > 10) {
		dump_len = 10;
	}
	for (i = 0; i < dump_len; i++) {
		RTK_LOGS(TAG, RTK_LOG_INFO, " 0x%02x", buf[i]);
		if ((i + 1) % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\n");
		}
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "\n");

	return HAL_OK;
}

static int composite_hid_cb_transmitted(u8 status)
{
	UNUSED(status);

	hid_tx_xfer_done = 1;

	return HAL_OK;
}

/**
  * @brief  Handle the CDC class control requests
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @retval Status
  */
static int composite_hid_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	UNUSED(req);
	UNUSED(buf);

	return HAL_OK;
}

/**
  * @brief  Set config callback
  * @param  None
  * @retval Status
  */
static int composite_hid_cb_set_config(void)
{
	usbd_comp_hid_ready = 1;

	return HAL_OK;
}

/* Private functions -UAC --------------------------------------------------------*/
/**
  * @brief  Initializes uac application layer
  * @param  None
  * @retval Status
  */
static int composite_uac_cb_init(void)
{
	return HAL_OK;
}

/**
  * @brief  DeInitializes uac application layer
  * @param  None
  * @retval Status
  */
static int composite_uac_cb_deinit(void)
{
	return HAL_OK;
}

/**
  * @brief  Set config callback
  * @param  None
  * @retval Status
  */
static int composite_uac_cb_set_config(void)
{
	usbd_comp_audio_ready = 1;

	return HAL_OK;
}

static int composite_uac_cb_mute_changed(u8 mute)
{
	UNUSED(mute);
	return HAL_OK;
}

static int composite_uac_cb_volume_changed(u8 volume)
{
	UNUSED(volume);

	return HAL_OK;
}

static int composite_uac_cb_format_changed(u32 sampling_freq, u8 ch_cnt, u8 byte_width)
{
	usbd_audio_cfg_t *out = &(composite_uac_usr_cb.out);

	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC format rate %d ch %d\n", sampling_freq, ch_cnt);

	if (sampling_freq != 0U) {
		out->sampling_freq = sampling_freq;
	}
	if (ch_cnt != 0U) {
		out->ch_cnt = ch_cnt;
	}
	if (byte_width != 0U) {
		out->byte_width = byte_width;
	}

	if (out->sampling_freq && out->ch_cnt && out->byte_width) {
		usbd_composite_uac_stop_play();
		usbd_comp_audio_play_stop = 1;
	}

	return HAL_OK;
}

/* Private functions --------------------------------------------------------*/
static void composite_usbd_audio_track_play(void)
{
	u32 read_data_len = 0;
	u32 zero_pkt = 0;

	usbd_composite_uac_config(&(composite_uac_usr_cb.out), 0, 0);
	do {
		if (usbd_composite_uac_start_play() == HAL_OK) {
			break;
		}
	} while (!usbd_comp_audio_play_stop);

	if (usbd_comp_audio_play_stop || (0 == usbd_comp_audio_ready)) {
		return;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Play begin\n");

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
	struct AudioTrack *audio_track;
	uint32_t format;
	int32_t track_buf_size;

	/* audio trace config params */
	uint32_t g_track_rate = composite_uac_usr_cb.out.sampling_freq;
	uint32_t g_track_channel = composite_uac_usr_cb.out.ch_cnt;
	uint32_t g_track_format = composite_uac_usr_cb.out.byte_width * 8;

	uint32_t play_track_channel = g_track_channel;    //mix not support 4 channel

#if CONFIG_USBD_UAC_DEMUX_CH_DEBUG
	uint32_t idx = 0, off = 0;
	//force to get the 1st channel to play
	play_track_channel = 1;
	uint32_t play_data_size;
	const uint32_t audio_src_step = g_track_channel * g_track_format / 8;
	const uint32_t audio_dst_step = play_track_channel * g_track_format / 8;
#endif

	//user should set sdk/component/soc/**/usrcfg/include/ameba_audio_hw_usrcfg.h's AUDIO_HW_AMPLIFIER_PIN to make sure amp is enabled.
	AudioService_Init();

	RTK_LOGS(TAG, RTK_LOG_INFO, "Audio ch:%d,rate:%d,bits=%d\n", g_track_channel, g_track_rate, g_track_format);

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

	track_buf_size = AudioTrack_GetMinBufferBytes(audio_track, AUDIO_CATEGORY_MEDIA, g_track_rate, format, play_track_channel) * 10;
	if (track_buf_size == 0) {
		track_buf_size = g_track_rate * g_track_format / 8 * play_track_channel / 1000 * 100;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Track buf resize to %d\n", track_buf_size);
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Track buf size %d\n", track_buf_size);
	}
	AudioTrackConfig  track_config;
	track_config.category_type = AUDIO_CATEGORY_MEDIA;
	track_config.sample_rate = g_track_rate;
	track_config.format = format;
	track_config.channel_count = play_track_channel;
	track_config.buffer_bytes = track_buf_size;
	AudioTrack_Init(audio_track, &track_config, AUDIO_OUTPUT_FLAG_NONE);

	/*for mixer version, this mean sw volume, for passthrough version, sw volume is not supported*/
	AudioTrack_SetVolume(audio_track, 1.0, 1.0);
	AudioTrack_SetStartThresholdBytes(audio_track, track_buf_size);

	if (AudioTrack_Start(audio_track) != AUDIO_OK) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Audio track start fail\n");
		return;
	}

	while (!usbd_comp_audio_play_stop) {
		read_data_len = usbd_composite_uac_read(composite_usbd_uac_recv_buf, COMP_USBD_AUDIO_PLAYER_BUF_SIZE, 500, &zero_pkt);
		if (zero_pkt) {
			zero_pkt = 0;
		}

		if (read_data_len > 0) {
#if CONFIG_USBD_UAC_DEMUX_CH_DEBUG
			play_data_size = 0;
			//get the 2 channel data from the 4 channel
			for (idx = 0, off = 0; idx < read_data_len; idx += audio_src_step, off += audio_dst_step) {
				// ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3
				// 24  24  24  24  24  24  24  24  24  24  24  24
				memcpy((void *)(composite_usbd_uac_play_buf + off), (void *)(composite_usbd_uac_recv_buf + idx), audio_dst_step);
				play_data_size += audio_dst_step;
			}

			AudioTrack_Write(audio_track, (u8 *)composite_usbd_uac_play_buf, play_data_size, true);
#else
			AudioTrack_Write(audio_track, (u8 *)composite_usbd_uac_recv_buf, read_data_len, true);
#endif
		}
	}

	usbd_composite_uac_stop_play();

	AudioTrack_Pause(audio_track);
	AudioTrack_Flush(audio_track);
	AudioTrack_Stop(audio_track);
	AudioTrack_Destroy(audio_track);

	audio_track = NULL;
#else
	u32 total_len = 0;
	u32 read_cnt = 0;
	while (!usbd_comp_audio_play_stop) {
		zero_pkt = 0;
		read_data_len = usbd_composite_uac_read(composite_usbd_uac_recv_buf, COMP_USBD_AUDIO_PLAYER_BUF_SIZE, 500, &zero_pkt);
		if (zero_pkt) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Audio data %d-0x%08x\n", read_data_len, zero_pkt);
		}
		read_cnt ++;
		if (read_data_len > 0) {
			total_len += read_data_len;
			if (read_cnt % 200 == 0) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Audio get %d %d\n", read_data_len, total_len);
			}
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Audio Read Timeout\n");
			rtos_time_delay_ms(1);
		}
	}
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "Play exit\n");
}

static void composite_usbd_audio_track_thread(void *param)
{
	UNUSED(param);

	//wait usb init success
	do {
		/*wait uac class alive*/
		RTK_LOGS(TAG, RTK_LOG_INFO, "USBD wait playback init\n");
		usbd_comp_audio_play_task_alive = 0;
		do {
			rtos_time_delay_ms(1000);
		} while (usbd_comp_audio_ready == 0); //wait uac ready

		do {
			rtos_time_delay_ms(1000);
		} while (usbd_comp_audio_play_stop == 0);  //wait set config
		usbd_comp_audio_play_stop = 0;

		usbd_comp_audio_play_task_alive = 1;
		composite_usbd_audio_track_play();
	} while (1);

	rtos_task_delete(NULL);
}

/* Audio record functions ----------------------------------------------------*/
#if defined(ENABLE_USB_RECORD_FEATURE)
/**
 * @brief Audio record implementation - continuous loop mode
 * @note Records audio continuously until stop signal received
 */
static void composite_usbd_audio_record(void)
{
	const unsigned char *usbh_uac_audio_data_handle = usbd_uac_record_audio_data;
	u32 audio_total_data_len = usbd_uac_record_data_len;
	u8 *buffer = NULL;
	u32 total_len;
	u32 send_len = 0;
	u32 offset;
	u32 ret;
	u32 loop_count = 0;

	usbd_composite_uac_config(&(composite_uac_usr_cb.in), 1, 0);
	usbd_composite_uac_start_record();

	/* Wait for user to trigger recording */
	do {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Type \"comp_record\" to start record\n");
		rtos_time_delay_ms(5000);
	} while ((usbd_comp_user_enable_uac_record == 0) && (usbd_comp_audio_record_stop == 0));

	if (usbd_comp_audio_record_stop) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Record cancelled before start\n");
		usbd_composite_uac_stop_record();
		return;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Record start\n");

	/* Continuous recording loop until stop signal received */
	while (usbd_comp_audio_record_stop == 0) {
		loop_count++;

		usbh_uac_busy_count = 0;
		usbh_uac_err_count = 0;
		total_len = audio_total_data_len;
		offset = 0;

		/* Send one complete round of audio data */
		while ((offset < total_len) && (usbd_comp_audio_record_stop == 0)) {
			/* Fixed send length of 64 bytes */
			send_len = 64;

			/* Retry loop until success or stop requested */
			do {
				/* Check if stop is requested */
				if (usbd_comp_audio_record_stop) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Record stopped at offset %u\n", offset);
					goto exit_record;
				}

				/* Adjust length for the last transmission */
				if (offset + send_len >= total_len) {
					send_len = total_len - offset;
				}

				buffer = (u8 *)(usbh_uac_audio_data_handle + offset);

				/* Send data */
				ret = usbd_composite_uac_write(buffer, send_len, 1000);
				offset += ret;

				if (ret != send_len) {
					/* Transmission failed, log error and retry */
					usbh_uac_err_count++;
					send_len -= ret;

					if (usbh_uac_err_count % 100 == 0) {
						RTK_LOGS(TAG, RTK_LOG_WARN, "Record error count: %d (sent %u, expected %u)\n", usbh_uac_err_count, ret, send_len + ret);
					}
				} else {
					/* Transmission successful, exit retry loop */
					break;
				}
			} while (usbd_comp_audio_record_stop == 0);
		}

		/* One round of data transmission completed */
		if (loop_count % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Record loop #%u done: %u bytes, errors: %d\n", loop_count, offset, usbh_uac_err_count);
		}

		/* Brief delay to avoid excessive CPU usage */
		rtos_time_delay_ms(10);
	}

exit_record:
	usbd_composite_uac_stop_record();

	RTK_LOGS(TAG, RTK_LOG_INFO, "Recording stopped after %u loops, total errors: %d\n", loop_count, usbh_uac_err_count);
}

static void composite_usbd_audio_record_thread(void *param)
{
	UNUSED(param);

	//wait usb init success
	do {
		/*wait uac class alive*/
		RTK_LOGS(TAG, RTK_LOG_INFO, "USBD wait record init\n");
		usbd_comp_audio_record_task_alive = 0;
		usbd_comp_audio_record_stop = 0;

		do {
			rtos_time_delay_ms(1000);
		} while (usbd_comp_audio_ready == 0);

		usbd_comp_audio_record_task_alive = 1;
		composite_usbd_audio_record();
	} while (1);

	rtos_task_delete(NULL);
}
#endif

static void composite_usbd_init_thread(void *param)
{
	UNUSED(param);
	int ret = 0;
	rtos_task_t task;

	hid_tx_xfer_done = 1;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD HID_UAC demo start\n");
	usb_os_queue_create(&msg_queue, CONFIG_COMP_USBD_MSG_QUEUE_DEPTH, sizeof(u32));

	ret = usbd_init(&composite_usbd_cfg);
	if (ret != HAL_OK) {
		goto exit_usbd_init_fail;
	}

	ret = usbd_composite_init(&composite_hid_usr_cb,
							  &composite_uac_usr_cb,
							  &composite_usbd_cb);
	if (ret != HAL_OK) {
		goto exit_usbd_composite_init_fail;
	}

	ret = rtos_task_create(&task, "composite_handle_event_task", composite_handle_event_task, NULL, 1024 * 2,  CONFIG_USBD_COMPOSITE_EVENT_TASK_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_create_check_task_fail;
	}

	rtos_time_delay_ms(100);

	rtos_task_delete(NULL);

	return;

exit_create_check_task_fail:
	usbd_composite_deinit();

exit_usbd_composite_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
	usb_os_queue_delete(msg_queue);
	rtos_task_delete(NULL);
}

static void composite_usbd_flag_check(__IO u8 *flag)
{
	if (flag && (*flag == 0)) {
		do {
			rtos_time_delay_ms(100);
		} while (*flag == 0);
		*flag = 0;
	}
}

static u32 hidd_comp_tx(u16 argc, u8 *argv[])
{
	u16 size = 10;
	int ret;

	if (argc == 0) {
		size = 10;
	} else {
		size = (u16)_strtoul((const char *)(argv[0]), (char **)NULL, 10);

		if (size > 1024) {
			size = 1024;
		}
	}

	composite_usbd_flag_check(&hid_tx_xfer_done);

	memset(&hid_report_buf[0], (u8)(size & 0xFF), size);
	ret = usbd_composite_hid_send_data((u8 *)&hid_report_buf[0], size);

	if (ret == HAL_OK) {
		hid_tx_xfer_done = 0;
	}

	return HAL_OK;
}

static u32 hidd_comp_volup(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	usbd_composite_hid_volume_ctrl(1);
	return HAL_OK;
}

static u32 hidd_comp_voldown(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	usbd_composite_hid_volume_ctrl(0);
	return HAL_OK;
}

static u32 comp_record_start(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
#if defined(ENABLE_USB_RECORD_FEATURE)
	usbd_comp_user_enable_uac_record = 1;
#endif
	return HAL_OK;
}

static void composite_usbd_hid_rx_thread(void *param)
{
	UNUSED(param);
	u32 read_data_len = 0;

	do {
		/*wait hid class alive*/
		while (usbd_comp_hid_ready == 0) {
			rtos_time_delay_ms(1000);
		}

		read_data_len = usbd_composite_hid_read(hid_rx_buf, USBD_COMP_HID_MAX_BUF_SIZE, 500);
		if (read_data_len > 0) {
			composite_hid_cb_received(hid_rx_buf, read_data_len);
		} else {
			rtos_time_delay_ms(1);
		}
	} while (1);

	rtos_task_delete(NULL);
}
/* Exported functions --------------------------------------------------------*/
/**
  * @brief  USB download de-initialize
  * @param  None
  * @retval Result of the operation: 0 if success else fail
  */
void example_usbd_composite(void)
{
	int ret;
	rtos_task_t task;

	usbd_comp_audio_ready = 0;
	usbd_comp_audio_play_stop = 0;

	ret = rtos_task_create(&task, "composite_usbd_init_thread", composite_usbd_init_thread, NULL, 1024, CONFIG_USBD_COMPOSITE_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD COMP thread fail\n");
		return;
	}

#if defined(ENABLE_USB_RECORD_FEATURE)
	if (rtos_task_create(NULL, ((const char *)"composite_usbd_audio_record_thread"), composite_usbd_audio_record_thread, NULL, 1024 * 16,
						 CONFIG_USBD_COMPOSITE_UAC_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create audio record fail\n");
	}
#endif

	if (rtos_task_create(NULL, ((const char *)"composite_usbd_audio_track_thread"), composite_usbd_audio_track_thread, NULL, 1024 * 16,
						 CONFIG_USBD_COMPOSITE_UAC_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create audio track fail\n");
	}

	if (rtos_task_create(NULL, ((const char *)"composite_usbd_hid_rx_thread"), composite_usbd_hid_rx_thread, NULL, 1024,
						 CONFIG_USBD_COMPOSITE_HID_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create HID RX thread fail\n");
	}
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE composite_usbd_hid_cmd_table[] = {
	{"hidd_comp_tx", hidd_comp_tx},
	{"hidd_comp_volup", hidd_comp_volup},
	{"hidd_comp_voldown", hidd_comp_voldown},
	{"comp_record", comp_record_start},
};
