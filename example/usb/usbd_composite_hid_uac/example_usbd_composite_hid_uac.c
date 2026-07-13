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

#define CONFIG_USBD_COMP_HOTPLUG                             1

#define CONFIG_USBD_COMP_HID_TX_BUF_LEN                      1024

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
#define CONFIG_USBD_COMP_DEMUX_CH_DEBUG                      1
#else
#define CONFIG_USBD_COMP_DEMUX_CH_DEBUG                      0
#endif

/* Task exit wait timeout */
#define CONFIG_USBD_COMP_TASK_EXIT_TIMEOUT_MS                5000
#define CONFIG_USBD_COMP_TASK_POLL_INTERVAL_MS               100

#define CONFIG_USBD_COMP_DUMP_COUNT                          300

/* USB speed */
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define CONFIG_USBD_COMP_SPEED                               USB_SPEED_FULL
#elif defined(CONFIG_USBD_COMPOSITE_HID_UAC1)
/* UAC 1.0 spec supports only Full Speed. */
#define CONFIG_USBD_COMP_SPEED                               USB_SPEED_HIGH_IN_FULL
#else
#define CONFIG_USBD_COMP_SPEED                               USB_SPEED_HIGH
/* At present, audio recording functionality is only supported by the UAC 2.0 interface. */
#define CONFIG_USBD_COMP_ENABLE_RECORD                       1
#endif

/* Thread priorities */
#define CONFIG_USBD_COMP_INIT_THREAD_PRIORITY                7U
#define CONFIG_USBD_COMP_EVENT_TASK_PRIORITY                 8U
#define CONFIG_USBD_COMP_HID_THREAD_PRIORITY                 5U
#define CONFIG_USBD_COMP_THREAD_PRIORITY                     6U
#define CONFIG_USBD_COMP_STATE_THREAD_PRIORITY               1U

/* Thread stack sizes */
#define CONFIG_USBD_COMP_INIT_THREAD_STACK_SIZE              (1024U)
#define CONFIG_USBD_COMP_EVENT_TASK_STACK_SIZE               (900U)
#define CONFIG_USBD_COMP_HID_THREAD_STACK_SIZE               (1024U)
#define CONFIG_USBD_COMP_RECORD_THREAD_STACK_SIZE            (900U)
#define CONFIG_USBD_COMP_PLAY_THREAD_STACK_SIZE              (2800U)
#define CONFIG_USBD_COMP_STATE_THREAD_STACK_SIZE             (900U)

/* Buffer sizes */
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define CONFIG_USBD_COMP_AUDIO_MS_BUF_SIZE                   1023U
#else
#define CONFIG_USBD_COMP_AUDIO_MS_BUF_SIZE                   1024U
#endif
#define CONFIG_USBD_COMP_AUDIO_PLAYER_BUF_SIZE               ((CONFIG_USBD_COMP_AUDIO_MS_BUF_SIZE) * 5)

/* Private types -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void usbd_comp_cb_status_changed(u8 old_status, u8 status);
static int usbd_comp_cb_set_config(void);

/* HID */
static int usbd_comp_hid_cb_init(void);
static int usbd_comp_hid_cb_deinit(void);
static int usbd_comp_hid_cb_setup(usb_setup_req_t *req, u8 *buf);
static int usbd_comp_hid_cb_set_config(void);
static int usbd_comp_hid_cb_received(u8 *buf, u32 Len);
static int usbd_comp_hid_cb_transmitted(u8 status);

/* UAC */
static int usbd_comp_uac_cb_init(void);
static int usbd_comp_uac_cb_deinit(void);
static int usbd_comp_uac_cb_set_config(void);
static int usbd_comp_uac_cb_mute_changed(u8 mute);
static int usbd_comp_uac_cb_volume_changed(u8 volume);
static int usbd_comp_uac_cb_format_changed(u32 sampling_freq, u8 ch_cnt, u8 byte_width);

/* Private variables ---------------------------------------------------------*/
/* USB Common */
#if CONFIG_USBD_COMP_HOTPLUG
static __IO u8 usbd_comp_attach_status;
static __IO u8 usbd_comp_attach_old_status;
#endif

#if CONFIG_USBD_COMP_HOTPLUG
static rtos_sema_t usbd_comp_attach_sema = NULL;
static __IO u8 usbd_comp_hotplug_thread_running = 0;
#endif
static __IO u8 usbd_comp_usb_ready = 0;           /* USB ready */

/* HID */
static __IO u8 usbd_comp_hid_ready = 0;      /* HID ready */
static __IO u8 usbd_comp_hid_tx_done = 0;         /* HID tx xfer done */
static u8 usbd_comp_hid_tx_buf[CONFIG_USBD_COMP_HID_TX_BUF_LEN];    /* HID tx buffer */
static u8 usbd_comp_hid_rx_buf[USBD_COMP_HID_MAX_BUF_SIZE];             /* HID rx buffer */

/*  UAC */
static __IO u8 usbd_comp_audio_ready = 0;         /* UAC ready: playback & record */
/*  UAC play */
static __IO u8 usbd_comp_audio_play_stop = 0;     /* Audio play stop */
static __IO u8 usbd_comp_audio_play_task_alive = 0;    /* Audio play task */
static __IO u8 usbd_comp_hid_stop = 0;             /* HID RX thread stop */
static __IO u8 usbd_comp_hid_task_alive = 0;       /* HID RX task alive flag */

/* Audio state: updated by ISR callbacks, read by dump task */
static u8 usbd_comp_uac_mute = 0;                 /* Current mute state */
static u8 usbd_comp_uac_volume = 0;               /* Current volume value */
static rtos_sema_t usbd_comp_state_sema = NULL;         /* State thread sema: given by mute/volume ISR */

/* UAC record */
#if defined(CONFIG_USBD_COMP_ENABLE_RECORD)
static __IO u8 usbd_comp_user_enable_uac_record = 0;    /* User enable uac record function */
static __IO u8 usbd_comp_audio_record_stop = 0;         /* Audio record stop */
static __IO u8 usbd_comp_audio_record_task_alive = 0;   /* Audio record task */

static int usbd_comp_uac_busy_cnt;
static int usbd_comp_uac_err_cnt;
#endif

#if CONFIG_USBD_COMP_DEMUX_CH_DEBUG
/*
    the buffer which will write to the audio track
    the basic length is CONFIG_USBD_COMP_AUDIO_PLAYER_BUF_SIZE
*/
static u8 usbd_comp_uac_play_buf[CONFIG_USBD_COMP_AUDIO_PLAYER_BUF_SIZE];
#endif

/*
    uac isoc rx buffer, used to save the audio data that received from the host
    the recv_buffer length is 2*CONFIG_USBD_COMP_AUDIO_PLAYER_BUF_SIZE
    the play task will write the CONFIG_USBD_COMP_AUDIO_PLAYER_BUF_SIZE length data to audiotack once
*/
static u8 usbd_comp_uac_recv_buf[CONFIG_USBD_COMP_AUDIO_PLAYER_BUF_SIZE];

/* Callback structures */
static const usbd_composite_cb_t composite_usbd_cb = {
	.status_changed = usbd_comp_cb_status_changed,
	.set_config = usbd_comp_cb_set_config,
};

static const usbd_config_t composite_usbd_cfg = {
	.speed = CONFIG_USBD_COMP_SPEED,
	.isr_priority = INT_PRI_HIGHEST,
	.isr_in_critical = 0,
#if defined (CONFIG_AMEBASMART) || defined (CONFIG_AMEBADPLUS)
	.ext_intr_enable = USBD_SOF_INTR,
	.nptx_max_epmis_cnt = 100U,
#elif defined (CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 420U,
	.ptx_fifo_depth = {16U, 256U, 32U, 256U, },
	.ext_intr_enable = USBD_SOF_INTR,
#elif defined (CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1424U,
	.ptx_fifo_depth = {256U, 32U, 256U, },
	.ext_intr_enable = USBD_SOF_INTR,
#endif
};

/*  HID */
static const usbd_composite_hid_usr_cb_t composite_hid_usr_cb = {
	.init = usbd_comp_hid_cb_init,
	.deinit = usbd_comp_hid_cb_deinit,
	.set_config = usbd_comp_hid_cb_set_config,
	.setup = usbd_comp_hid_cb_setup,
	.transmitted = usbd_comp_hid_cb_transmitted,
};

/*  UAC */
static usbd_composite_uac_usr_cb_t composite_uac_usr_cb = {
	.audio_ctx = NULL,
	.in = {
#if defined(CONFIG_USBD_COMP_ENABLE_RECORD)
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
	.init = usbd_comp_uac_cb_init,
	.deinit = usbd_comp_uac_cb_deinit,
	.set_config = usbd_comp_uac_cb_set_config,
	.mute_changed = usbd_comp_uac_cb_mute_changed,
	.volume_changed = usbd_comp_uac_cb_volume_changed,
	.format_changed = usbd_comp_uac_cb_format_changed,
};

/* Private functions ---------------------------------------------------------*/
/**
 * @brief Wait for task to exit with timeout
 * @param task_alive_flag: Pointer to task alive flag
 * @param timeout_ms: Timeout in milliseconds
 * @return 0 if success, -1 if timeout
 */
static int usbd_comp_wait_task_exit(volatile u8 *task_alive_flag, u32 timeout_ms)
{
	u32 elapsed = 0;

	while (*task_alive_flag == 1 && elapsed < timeout_ms) {
		usb_os_sleep_ms(CONFIG_USBD_COMP_TASK_POLL_INTERVAL_MS);
		elapsed += CONFIG_USBD_COMP_TASK_POLL_INTERVAL_MS;
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
static void usbd_comp_stop_all_tasks(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Stop all tasks\n");

	/* Stop HID task */
	usbd_comp_hid_ready = 0;
	usbd_comp_hid_stop = 1;

	/* Stop UAC */
	usbd_comp_audio_ready = 0;

	/* Stop UAC play task */
	usbd_comp_audio_play_stop = 1;
	usbd_composite_uac_stop_play();

#if defined(CONFIG_USBD_COMP_ENABLE_RECORD)
	/* Stop UAC record task */
	usbd_comp_user_enable_uac_record = 0;
	usbd_comp_audio_record_stop = 1;
	usbd_composite_uac_stop_record();

	/* Wait for record task exit */
	if (usbd_comp_wait_task_exit(&usbd_comp_audio_record_task_alive, CONFIG_USBD_COMP_TASK_EXIT_TIMEOUT_MS) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Record task exit fail\n");
	}
#endif

	/* Wait for hid rx task exit. */
	if (usbd_comp_wait_task_exit(&usbd_comp_hid_task_alive, CONFIG_USBD_COMP_TASK_EXIT_TIMEOUT_MS) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "HID task exit fail\n");
	}
	/* Wait for play task exit */
	if (usbd_comp_wait_task_exit(&usbd_comp_audio_play_task_alive, CONFIG_USBD_COMP_TASK_EXIT_TIMEOUT_MS) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Play task exit fail\n");
	}
}

/**
 * @brief USB status change callback
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 */
static void usbd_comp_cb_status_changed(u8 old_status, u8 status)
{
#if CONFIG_USBD_COMP_HOTPLUG
	usbd_comp_attach_status = status;
	usbd_comp_attach_old_status = old_status;
	if (usbd_comp_attach_sema != NULL) {
		rtos_sema_give(usbd_comp_attach_sema);
	}
#else
	UNUSED(old_status);
	UNUSED(status);
#endif
}

/**
 * @brief USB set config callback
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 */
static int usbd_comp_cb_set_config(void)
{
	usbd_comp_usb_ready = 1;

	return HAL_OK;
}

/**
 * @brief Reinitialize USB device
 */
static void usbd_comp_do_reinit(void)
{
	int ret;

	usbd_comp_usb_ready = 0;
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

#if CONFIG_USBD_COMP_HOTPLUG
/**
 * @brief Hotplug detection and handling thread (semaphore-based, mirrors usbd_cdc_ecm)
 */
static void usbd_comp_hotplug_thread(void *param)
{
	u8 current_status;
	UNUSED(param);

	usbd_comp_hotplug_thread_running = 1;

	while (usbd_comp_hotplug_thread_running) {
		if (rtos_sema_take(usbd_comp_attach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Status change %d -> %d\n",
				 usbd_comp_attach_old_status, usbd_comp_attach_status);
		current_status = usbd_comp_attach_status;

		if (current_status == USBD_ATTACH_STATUS_DETACHED) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "USB detached\n");
			usbd_comp_stop_all_tasks();
			usbd_comp_do_reinit();
		} else if (current_status == USBD_ATTACH_STATUS_ATTACHED) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Attached\n");
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Hotplug thread exit\n");
	usbd_comp_hotplug_thread_running = 0;
	rtos_task_delete(NULL);
}
#endif /* CONFIG_USBD_COMP_HOTPLUG */

/**
  * @brief  Initializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int usbd_comp_hid_cb_init(void)
{
	return HAL_OK;
}

/**
  * @brief  DeInitializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int usbd_comp_hid_cb_deinit(void)
{
	return HAL_OK;
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface through this function.
  * @param  Buf: RX buffer
  * @param  Len: RX data length (in bytes)
  * @retval Status
  */
static int usbd_comp_hid_cb_received(u8 *buf, u32 len)
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

/**
  * @brief  HID transmit complete callback
  * @note  This function is called within an interrupt service routine (ISR) context;
  *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  status: TX status
  * @retval Status
  */
static int usbd_comp_hid_cb_transmitted(u8 status)
{
	UNUSED(status);

	usbd_comp_hid_tx_done = 1;

	return HAL_OK;
}

/**
  * @brief  Handle the CDC class control requests
  * @note  This function is called within an interrupt service routine (ISR) context;
  *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @retval Status
  */
static int usbd_comp_hid_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	UNUSED(req);
	UNUSED(buf);

	return HAL_OK;
}

/**
  * @brief  Set config callback
  * @note  This function is called within an interrupt service routine (ISR) context;
  *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  None
  * @retval Status
  */
static int usbd_comp_hid_cb_set_config(void)
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
static int usbd_comp_uac_cb_init(void)
{
	return HAL_OK;
}

/**
  * @brief  DeInitializes uac application layer
  * @param  None
  * @retval Status
  */
static int usbd_comp_uac_cb_deinit(void)
{
	return HAL_OK;
}

/**
  * @brief  Set config callback
  * @note  This function is called within an interrupt service routine (ISR) context;
  *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  None
  * @retval Status
  */
static int usbd_comp_uac_cb_set_config(void)
{
	usbd_comp_audio_ready = 1;

	return HAL_OK;
}

/**
  * @brief  Mute state change notification from the USB host
  * @note  This function is called within an interrupt service routine (ISR) context;
  *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  mute: New mute state
  * @retval Status
  */
static int usbd_comp_uac_cb_mute_changed(u8 mute)
{
	usbd_comp_uac_mute = mute;
	if (usbd_comp_state_sema) {
		rtos_sema_give(usbd_comp_state_sema);
	}
	return HAL_OK;
}

/**
  * @brief  Volume change notification from the USB host
  * @note  This function is called within an interrupt service routine (ISR) context;
  *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  volume: New volume value
  * @retval Status
  */
static int usbd_comp_uac_cb_volume_changed(u8 volume)
{
	usbd_comp_uac_volume = volume;
	if (usbd_comp_state_sema) {
		rtos_sema_give(usbd_comp_state_sema);
	}
	return HAL_OK;
}

/**
  * @brief  Audio format change notification from the USB host
  * @note  This function is called within an interrupt service routine (ISR) context;
  *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  sampling_freq: New sampling frequency
  * @param  ch_cnt: New channel count
  * @param  byte_width: New sample byte width
  * @retval Status
  */
static int usbd_comp_uac_cb_format_changed(u32 sampling_freq, u8 ch_cnt, u8 byte_width)
{
	usbd_audio_cfg_t *out = &(composite_uac_usr_cb.out);

	//RTK_LOGS(TAG, RTK_LOG_INFO, "UAC format rate %d ch %d\n", sampling_freq, ch_cnt);

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
static void usbd_comp_audio_track_play(void)
{
	u32 read_data_len = 0;
	u32 zero_pkt = 0;

	if ((composite_uac_usr_cb.out.sampling_freq == 0) ||
		(composite_uac_usr_cb.out.ch_cnt == 0) ||
		(composite_uac_usr_cb.out.byte_width == 0)) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Play skip: format not ready (%d-%d-%d)\n",
				 composite_uac_usr_cb.out.sampling_freq,
				 composite_uac_usr_cb.out.ch_cnt,
				 composite_uac_usr_cb.out.byte_width);
		return;
	}

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

#if CONFIG_USBD_COMP_DEMUX_CH_DEBUG
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
		read_data_len = usbd_composite_uac_read(usbd_comp_uac_recv_buf, CONFIG_USBD_COMP_AUDIO_PLAYER_BUF_SIZE, 500, &zero_pkt);
		if (zero_pkt) {
			zero_pkt = 0;
		}

		if (read_data_len > 0) {
#if CONFIG_USBD_COMP_DEMUX_CH_DEBUG
			play_data_size = 0;
			//get the 2 channel data from the 4 channel
			for (idx = 0, off = 0; idx < read_data_len; idx += audio_src_step, off += audio_dst_step) {
				// ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3
				// 24  24  24  24  24  24  24  24  24  24  24  24
				memcpy((void *)(usbd_comp_uac_play_buf + off), (void *)(usbd_comp_uac_recv_buf + idx), audio_dst_step);
				play_data_size += audio_dst_step;
			}

			AudioTrack_Write(audio_track, (u8 *)usbd_comp_uac_play_buf, play_data_size, true);
#else
			AudioTrack_Write(audio_track, (u8 *)usbd_comp_uac_recv_buf, read_data_len, true);
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

	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter read loop, waiting for host audio data...\n");

	while (!usbd_comp_audio_play_stop) {
		zero_pkt = 0;
		read_data_len = usbd_composite_uac_read(usbd_comp_uac_recv_buf, CONFIG_USBD_COMP_AUDIO_PLAYER_BUF_SIZE, 500, &zero_pkt);

		read_cnt++;
		if (read_data_len > 0) {
			if (total_len == 0) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "First audio pkt: len=%d\n", read_data_len);
			}
			total_len += read_data_len;
			if (read_cnt % CONFIG_USBD_COMP_DUMP_COUNT == 0) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Audio get %d %d\n", read_data_len, total_len);
			}
		} else {
			rtos_time_delay_ms(1);
		}
	}
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "Play exit\n");
}

static void usbd_comp_playback_thread(void *param)
{
	UNUSED(param);

	//wait usb init success
	do {
		/*wait uac class alive*/
		RTK_LOGS(TAG, RTK_LOG_INFO, "USBD wait playback init\n");
		usbd_comp_audio_play_task_alive = 0;

		/*
		 * Start playback only when BOTH conditions hold:
		 *   audio_ready == 1  : USB is configured (set by uac set_config)
		 *   play_stop  == 1   : a start/restart was requested
		 *
		 * play_stop is raised by two sources:
		 *   - usbd_comp_uac_cb_format_changed(): host changed the audio format,
		 *     USB stays configured so audio_ready is still 1  -> restart here.
		 *   - usbd_comp_stop_all_tasks() on detach: it ALSO clears audio_ready
		 *     to 0, so the combined guard stays false and we keep parking with
		 *     play_task_alive == 0, letting wait_task_exit() succeed.
		 * Using the combined guard removes the need for a separate
		 * hotplug-in-progress flag scattered across the loop.
		 */
		while (!((usbd_comp_audio_ready != 0) && (usbd_comp_audio_play_stop != 0))) {
			rtos_time_delay_ms(50);
		}
		usbd_comp_audio_play_stop = 0;

		usbd_comp_audio_play_task_alive = 1;
		usbd_comp_audio_track_play();
	} while (1);

	rtos_task_delete(NULL);
}

/* -------------------------------------------------------------------------- */
/**
 * @brief  Periodic dump of audio state (mute, volume).
 * @note   Runs once per second in its own thread.
 */
static void usbd_comp_state_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		/* Wait for mute/volume change notification */
		if (usbd_comp_state_sema) {
			rtos_sema_take(usbd_comp_state_sema, RTOS_SEMA_MAX_COUNT);
		} else {
			rtos_time_delay_ms(200);
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Mute:%d vol:%d\n",
				 usbd_comp_uac_mute, usbd_comp_uac_volume);
	}
}

/* Audio record functions ----------------------------------------------------*/
#if defined(CONFIG_USBD_COMP_ENABLE_RECORD)
/**
 * @brief Audio record implementation - continuous loop mode
 * @note Records audio continuously until stop signal received
 */
static void usbd_comp_audio_record(void)
{
	const unsigned char *usbh_uac_audio_data_handle = usbd_uac_record_audio_data;
	u32 audio_total_data_len = usbd_uac_record_data_len;
	u8 *buffer = NULL;
	u32 total_len;
	u32 send_len = 0;
	u32 offset;
	u32 ret;
	u32 loop_count = 0;
	u16 record_prompt_printed;

	/* Guard against an unconfigured IN format. Same divide-by-zero hazard as
	 * the playback path: if the host has not set a valid IN interface/format
	 * (all three fields 0), the endpoint MPS resolves to 0 and the record ring
	 * buffer stays uninitialised (capacity == 0), which faults inside
	 * usb_ringbuf_is_full/add_tail on the first transfer. Bail out early. */
	if ((composite_uac_usr_cb.in.sampling_freq == 0) ||
		(composite_uac_usr_cb.in.ch_cnt == 0) ||
		(composite_uac_usr_cb.in.byte_width == 0)) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Record skip: format not ready (%d-%d-%d)\n",
				 composite_uac_usr_cb.in.sampling_freq,
				 composite_uac_usr_cb.in.ch_cnt,
				 composite_uac_usr_cb.in.byte_width);
		return;
	}

	usbd_composite_uac_config(&(composite_uac_usr_cb.in), 1, 0);
	usbd_composite_uac_start_record();

	/* Wait for user to trigger recording, or stop on hotplug */
	record_prompt_printed = 0;
	do {
		if (record_prompt_printed >= 100) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Type \"usbd_uac_record\" to start record\n");
			record_prompt_printed = 0;
		} else {
			record_prompt_printed++;
		}
		rtos_time_delay_ms(50);
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

		usbd_comp_uac_busy_cnt = 0;
		usbd_comp_uac_err_cnt = 0;
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
					usbd_comp_uac_err_cnt++;
					send_len -= ret;

					if (usbd_comp_uac_err_cnt % 100 == 0) {
						RTK_LOGS(TAG, RTK_LOG_WARN, "Record error count: %d (sent %u, expected %u)\n", usbd_comp_uac_err_cnt, ret, send_len + ret);
					}
				} else {
					/* Transmission successful, exit retry loop */
					break;
				}
			} while (usbd_comp_audio_record_stop == 0);
		}

		/* One round of data transmission completed */
		if (loop_count % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Record loop #%u done: %u bytes, errors: %d\n", loop_count, offset, usbd_comp_uac_err_cnt);
		}

		/* Brief delay to avoid excessive CPU usage */
		rtos_time_delay_ms(10);
	}

exit_record:
	usbd_composite_uac_stop_record();

	RTK_LOGS(TAG, RTK_LOG_INFO, "Recording stopped after %u loops, total errors: %d\n", loop_count, usbd_comp_uac_err_cnt);
}

static void usbd_comp_record_thread(void *param)
{
	UNUSED(param);

	//wait usb init success
	do {
		/*wait uac class alive*/
		RTK_LOGS(TAG, RTK_LOG_INFO, "USBD wait record init\n");
		usbd_comp_audio_record_task_alive = 0;

		/*
		 * Start only when audio_ready == 1 (USB configured) AND
		 * record_stop == 0. usbd_comp_stop_all_tasks() on detach clears
		 * audio_ready and sets record_stop = 1, so the guard stays false and
		 * we keep parking with record_task_alive == 0, letting
		 * wait_task_exit() succeed. Note: record_stop is cleared only AFTER
		 * the guard passes (a real start), never at the top of the loop, so a
		 * detach that arrives mid-park cannot be lost.
		 */
		while (!((usbd_comp_audio_ready != 0) && (usbd_comp_audio_record_stop == 0))) {
			rtos_time_delay_ms(50);
		}

		usbd_comp_audio_record_task_alive = 1;
		usbd_comp_audio_record();

		/* record_stop is set by both detach (stop_all_tasks) and the normal end
		 * of a recording session. Clear it here, after the session returns, so
		 * the next loop iteration parks on the guard until a fresh start. */
		usbd_comp_audio_record_stop = 0;
	} while (1);

	rtos_task_delete(NULL);
}
#endif

static void example_usbd_comp_init_thread(void *param)
{
	UNUSED(param);
	int ret = 0;
	rtos_task_t task;

	usbd_comp_hid_tx_done = 1;
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD HID_UAC demo start\n");

#if CONFIG_USBD_COMP_HOTPLUG
	ret = rtos_sema_create(&usbd_comp_attach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create attach sema fail\n");
		goto exit_cleanup;
	}
#endif

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

#if CONFIG_USBD_COMP_HOTPLUG
	ret = rtos_task_create(&task, "usbd_comp_hotplug_thread",
						   usbd_comp_hotplug_thread, NULL,
						   CONFIG_USBD_COMP_EVENT_TASK_STACK_SIZE, CONFIG_USBD_COMP_EVENT_TASK_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_create_check_task_fail;
	}
#endif

	rtos_time_delay_ms(100);

	rtos_task_delete(NULL);

	return;

exit_create_check_task_fail:
	usbd_composite_deinit();

exit_usbd_composite_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
#if CONFIG_USBD_COMP_HOTPLUG
	if (usbd_comp_attach_sema != NULL) {
		rtos_sema_delete(usbd_comp_attach_sema);
		usbd_comp_attach_sema = NULL;
	}
exit_cleanup:
#endif
	rtos_task_delete(NULL);
}

static void usbd_comp_flag_check(__IO u8 *flag)
{
	if (flag && (*flag == 0)) {
		do {
			rtos_time_delay_ms(100);
		} while (*flag == 0);
		*flag = 0;
	}
}

static u32 usbd_comp_hid_tx(u16 argc, u8 *argv[])
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

	usbd_comp_flag_check(&usbd_comp_hid_tx_done);

	memset(&usbd_comp_hid_tx_buf[0], (u8)(size & 0xFF), size);
	ret = usbd_composite_hid_send_data((u8 *)&usbd_comp_hid_tx_buf[0], size);

	if (ret == HAL_OK) {
		usbd_comp_hid_tx_done = 0;
	}

	return HAL_OK;
}

static u32 usbd_comp_hid_volup(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	usbd_composite_hid_volume_ctrl(1);
	return HAL_OK;
}

static u32 usbd_comp_hid_voldown(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	usbd_composite_hid_volume_ctrl(0);
	return HAL_OK;
}

static u32 usbd_comp_record_start(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
#if defined(CONFIG_USBD_COMP_ENABLE_RECORD)
	usbd_comp_user_enable_uac_record = 1;
#endif
	return HAL_OK;
}

static void usbd_comp_hid_rx_thread(void *param)
{
	UNUSED(param);
	u32 read_data_len = 0;

	do {
		/*wait hid class alive*/
		usbd_comp_hid_task_alive = 0;
		RTK_LOGS(TAG, RTK_LOG_INFO, "USBD wait hid ready\n");

		/*
		 * Start only when hid_ready == 1 (USB configured) AND hid_stop == 0.
		 * usbd_comp_stop_all_tasks() on detach sets hid_ready = 0 and
		 * hid_stop = 1, so the guard stays false and we keep parking with
		 * hid_task_alive == 0, letting wait_task_exit() succeed. This single
		 * combined guard replaces the scattered hotplug-in-progress checks.
		 */
		while (!((usbd_comp_hid_ready != 0) && (usbd_comp_hid_stop == 0))) {
			rtos_time_delay_ms(50);
		}

		usbd_comp_hid_task_alive = 1;

		while ((usbd_comp_hid_ready != 0) && !usbd_comp_hid_stop) {
			read_data_len = usbd_composite_hid_read(usbd_comp_hid_rx_buf, USBD_COMP_HID_MAX_BUF_SIZE, 500);
			if (usbd_comp_hid_stop) {
				break;
			}
			if (read_data_len > 0) {
				usbd_comp_hid_cb_received(usbd_comp_hid_rx_buf, read_data_len);
			} else {
				rtos_time_delay_ms(1);
			}
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

	/* Create dump semaphore */
	rtos_sema_create(&usbd_comp_state_sema, 0U, 1U);

	ret = rtos_task_create(&task, "example_usbd_comp_init_thread",
						   example_usbd_comp_init_thread, NULL,
						   CONFIG_USBD_COMP_INIT_THREAD_STACK_SIZE, CONFIG_USBD_COMP_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD COMP thread fail\n");
		return;
	}

#if defined(CONFIG_USBD_COMP_ENABLE_RECORD)
	if (rtos_task_create(NULL, ((const char *)"usbd_comp_record_thread"),
						 usbd_comp_record_thread, NULL,
						 CONFIG_USBD_COMP_RECORD_THREAD_STACK_SIZE, CONFIG_USBD_COMP_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create audio record fail\n");
	}
#endif

	if (rtos_task_create(NULL, ((const char *)"usbd_comp_playback_thread"),
						 usbd_comp_playback_thread, NULL,
						 CONFIG_USBD_COMP_PLAY_THREAD_STACK_SIZE, CONFIG_USBD_COMP_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create audio track fail\n");
	}

	if (rtos_task_create(NULL, ((const char *)"usbd_comp_hid_rx_thread"),
						 usbd_comp_hid_rx_thread, NULL,
						 CONFIG_USBD_COMP_HID_THREAD_STACK_SIZE, CONFIG_USBD_COMP_HID_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create HID RX thread fail\n");
	}

	if (rtos_task_create(NULL, ((const char *)"usbd_comp_state_thread"),
						 usbd_comp_state_thread, NULL,
						 CONFIG_USBD_COMP_STATE_THREAD_STACK_SIZE,
						 CONFIG_USBD_COMP_STATE_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create dump thread fail\n");
	}
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE composite_usbd_hid_cmd_table[] = {
	{"usbd_hid_tx", usbd_comp_hid_tx},
	{"usbd_hid_volup", usbd_comp_hid_volup},
	{"usbd_hid_voldown", usbd_comp_hid_voldown},
	{"usbd_uac_record", usbd_comp_record_start},
};
