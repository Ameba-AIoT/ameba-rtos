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

/* This used to check the USB issue */
#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBADPLUS)
#define CONFIG_USBD_COMPOSITE_AUDIO_EN                          1
#else
#define CONFIG_USBD_COMPOSITE_AUDIO_EN                          0
#endif

#if  CONFIG_USBD_COMPOSITE_AUDIO_EN
#include "audio/audio_control.h"
#include "audio/audio_equalizer.h"
#include "audio/audio_track.h"
#include "audio/audio_service.h"
#include "common/audio_errnos.h"
#endif

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "COMP";

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define CONFIG_USBD_COMPOSITE_HOTPLUG                            1

#define CONFIG_USBD_HID_CONSTANT_LOOP                            2
#define CONFIG_USBD_COMP_HID_TX_BUG_LEN                          1024

#define CONFIG_USBD_UAC_DEMUX_CH_DEBUG                           1

#define CONFIG_COMP_USBD_MSG_QUEUE_DEPTH                         10

#define COMP_USBD_HOT_PLUG_STATUS                                1

// USB speed
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define CONFIG_USBD_COMPOSITE_SPEED USB_SPEED_FULL
#elif defined(CONFIG_USBD_COMPOSITE_HID_UAC1)
/* UAC 1.0 spec supports only Full Speed. */
#define CONFIG_USBD_COMPOSITE_SPEED USB_SPEED_HIGH_IN_FULL
#else
#define CONFIG_USBD_COMPOSITE_SPEED USB_SPEED_HIGH
#endif

// Thread priorities
#define CONFIG_USBD_COMPOSITE_HID_THREAD_PRIORITY                4U
#define CONFIG_USBD_COMPOSITE_UAC_THREAD_PRIORITY                4U
#define CONFIG_USBD_COMPOSITE_INIT_THREAD_PRIORITY               5U
#define CONFIG_USBD_COMPOSITE_ISR_THREAD_PRIORITY                7U
#define CONFIG_USBD_COMPOSITE_EVENT_TASK_PRIORITY                8U // Should be higher than CONFIG_USBD_COMPOSITE_ISR_THREAD_PRIORITY

/* ms */
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define COMP_USBD_AUDIO_MS_BUF_SIZE               1023U
#else
/* the buffer size maybe need to be changed if format( AUDIO_BYTE_WIDTH_SIZE * AUDIO_CHANNEL_NUM num * AUDIO_SAMPLING_RATE) changes */
//#define COMP_USBD_AUDIO_MS_BUF_SIZE             ((AUDIO_BYTE_WIDTH_SIZE) * (AUDIO_CHANNEL_NUM) * (AUDIO_SAMPLING_RATE) / 1000)
#define COMP_USBD_AUDIO_MS_BUF_SIZE               1024U
#endif
#define COMP_USBD_AUDIO_BUF_SIZE                  ((COMP_USBD_AUDIO_MS_BUF_SIZE) * 1)

/* Private types -------------------------------------------------------------*/
typedef struct __PACKED {
	u8 func_cmd;      //left & right : alt win shift alt command
	u8 padding;       //constant panding
	u8 cmd[30];       //6 command key values
} composite_usbd_hid_report_data_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void composite_cb_status_changed(u8 old_status, u8 status);;
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
/*
* USB Common
*/
#if CONFIG_USBD_COMPOSITE_HOTPLUG
static __IO u8 composite_attach_status;
#endif

static usb_os_queue_t msg_queue;

static __IO u8 usb_ready_flag = 0;
static __IO u8 uac_ready_flag = 0;
static __IO u8 hid_ready_flag = 0;

/*  HID */
static __IO u8 hid_rx_len = 0;
static __IO u8 hid_transmit_flag = 0;
static __IO u8 usbd_comp_hid_task_stop = 0;
static u8 hid_report_buf[CONFIG_USBD_COMP_HID_TX_BUG_LEN];
static u8 hid_rx_buf[USBD_COMP_HID_MAX_BUF_SIZE];

/*  UAC */
static __IO u8 uac_play_start_flag = 0;
static __IO u8 usbd_comp_audio_task_stop = 0;
static __IO u8 usbd_comp_uac_class_alive = 0;

#if  CONFIG_USBD_COMPOSITE_AUDIO_EN
/*
    the buffer which will write to the audio track
    the basic length is COMP_USBD_AUDIO_BUF_SIZE
*/
static u8 composite_usbd_uac_play_buf[COMP_USBD_AUDIO_BUF_SIZE];
#endif

/*
    uac isoc rx buffer, used to save the audio data that received from the host
    the recv_buffer length is 2*COMP_USBD_AUDIO_BUF_SIZE
    the play task will write the COMP_USBD_AUDIO_BUF_SIZE length data to audiotack once
*/
static u8 composite_usbd_uac_recv_buf[COMP_USBD_AUDIO_BUF_SIZE];

/* HID */
composite_usbd_hid_report_data_t composite_hid_report_data[] = {
	{1,   1,   {1,    1,    1,    1,    1,    1} },    //all released
	{2,   2,   {2,    2,    2,    2,    2,    2} },    //enter a pressed
	{3,   3,   {3,    3,    3,    3,    3,    3} },    //enter a released
};

static usbd_composite_cb_t composite_usbd_cb = {
	.status_changed = composite_cb_status_changed,
	.set_config = composite_cb_set_config,
};

static usbd_config_t composite_usbd_cfg = {
	.speed = CONFIG_USBD_COMPOSITE_SPEED,
	.dma_enable = 1U,
	.isr_priority = INT_PRI_HIGHEST,
	.isr_in_critical = 0,
	.intr_use_ptx_fifo = 0U,
	.ext_intr_en        = USBD_EPMIS_INTR | USBD_EOPF_INTR,
	.nptx_max_epmis_cnt = 100U,
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
	.in = {.enable = 0,}, /* current just support usb out,usb in TODO */
	.out = {.enable = 1, },
	.init = composite_uac_cb_init,
	.deinit = composite_uac_cb_deinit,
	.set_config = composite_uac_cb_set_config,
	.mute_changed = composite_uac_cb_mute_changed,
	.volume_changed = composite_uac_cb_volume_changed,
	.format_changed = composite_uac_cb_format_changed,
};

/* Private functions ---------------------------------------------------------*/
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

static int composite_cb_set_config(void)
{
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "USB Set Cfg\n");
	usb_ready_flag = 1;

	return HAL_OK;
}

static void composite_do_reinit(void)
{
	int ret;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "DETACHED\n");
	usb_ready_flag = 0;
	usbd_composite_deinit();
	ret = usbd_deinit();
	if (ret != HAL_OK) {
		return;
	}

	rtos_time_delay_ms(100);

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
	ret = usbd_init(&composite_usbd_cfg);
	if (ret != HAL_OK) {
		return ;
	}

	ret = usbd_composite_init(&composite_hid_usr_cb,
							  &composite_uac_usr_cb,
							  &composite_usbd_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "usbd_composite_init fail\n");
		usbd_deinit();
	}
}

static void composite_handle_event_task(void *param)
{
	UNUSED(param);
	u32 msg;

	for (;;) {
		int ret = usb_os_queue_receive(msg_queue, &msg, 0xFFFFFFFF);
		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "Get Event %d state %d\n", msg, ret);
		if (ret == HAL_OK) { /*handle the event*/
			switch (msg) {
#if CONFIG_USBD_COMPOSITE_HOTPLUG
			case COMP_USBD_HOT_PLUG_STATUS:
				if (composite_attach_status == USBD_ATTACH_STATUS_DETACHED) {
					usbd_comp_uac_class_alive = 0;
					usbd_composite_uac_stop_play();
					usbd_comp_audio_task_stop = 1;

					composite_do_reinit();
				}
				break;
#endif
			default:
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Can not handle type %d\n", msg);
				break;
			}
		}
	}

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Event thread fail\n");
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
	RTK_LOGS(TAG, RTK_LOG_INFO, "\n\n");

	return HAL_OK;
}

static int composite_hid_cb_transmitted(u8 status)
{
	UNUSED(status);
	DiagPrintf("HID TX\n");
	hid_transmit_flag = 1;

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
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "HID set config\n");
	hid_ready_flag = 1;

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
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "UAC set config\n");
	uac_ready_flag = 1;

	return HAL_OK;
}

static int composite_uac_cb_mute_changed(u8 mute)
{
	UNUSED(mute);
	// RTK_LOGS(TAG, RTK_LOG_DEBUG,"UAC set mute %d\n", mute);

	return HAL_OK;
}

static int composite_uac_cb_volume_changed(u8 volume)
{
	UNUSED(volume);
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "UAC set volume %d\n", volume);

	return HAL_OK;
}

static int composite_uac_cb_format_changed(u32 sampling_freq, u8 ch_cnt, u8 byte_width)
{
	usbd_composite_uac_stop_play();
	usbd_comp_audio_task_stop = 1;
	uac_play_start_flag = 1;

	if (sampling_freq != 0U) {
		composite_uac_usr_cb.out.sampling_freq = sampling_freq;
	}
	if (ch_cnt != 0U) {
		composite_uac_usr_cb.out.ch_cnt = ch_cnt;
	}
	if (byte_width != 0U) {
		composite_uac_usr_cb.out.byte_width = byte_width;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC set sampling_freq %d ch_cnt %d\n", sampling_freq, ch_cnt);

	return HAL_OK;
}


/* Private functions --------------------------------------------------------*/
static void composite_usbd_audio_track_play(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Audio track demo begin\n");
	u32 read_dat_len = 0;
	u32 zero_pkt = 0;

	usbd_composite_uac_config(&(composite_uac_usr_cb.out), 0, 0);
	do {
		if (usbd_composite_uac_start_play() == HAL_OK) {
			break;
		}
	} while (1);

#if  CONFIG_USBD_COMPOSITE_AUDIO_EN

	struct RTAudioTrack *audio_track;
	uint32_t format;
	int32_t track_buf_size;

	/* audio trace config params */
	// RTK_LOGS(TAG, RTK_LOG_DEBUG,"Audio info %d-%d-%d\n",composite_uac_usr_cb.out.byte_width,composite_uac_usr_cb.out.ch_cnt,composite_uac_usr_cb.out.sampling_freq);
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
	RTAudioService_Init();

	RTK_LOGS(TAG, RTK_LOG_INFO, "Audio ch:%d,rate:%d,bits=%d\n", g_track_channel, g_track_rate, g_track_format);

	switch (g_track_format) {
	case 16:
		format = RTAUDIO_FORMAT_PCM_16_BIT;
		break;
	case 24:
		format = RTAUDIO_FORMAT_PCM_24_BIT;
		break;
	case 32:
		format = RTAUDIO_FORMAT_PCM_32_BIT;
		break;
	default:
		format = RTAUDIO_FORMAT_PCM_16_BIT;
		break;
	}

	audio_track = RTAudioTrack_Create();
	if (!audio_track) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create AudioTrack fail\n");
		return;
	}

	track_buf_size = RTAudioTrack_GetMinBufferBytes(audio_track, RTAUDIO_CATEGORY_MEDIA, g_track_rate, format, play_track_channel) * 10;
	if (track_buf_size == 0) {
		track_buf_size = g_track_rate * g_track_format / 8 * play_track_channel / 1000 * 100;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Track buf resize to %d\n", track_buf_size);
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Track buf size:%d\n", track_buf_size);
	}
	RTAudioTrackConfig  track_config;
	track_config.category_type = RTAUDIO_CATEGORY_MEDIA;
	track_config.sample_rate = g_track_rate;
	track_config.format = format;
	track_config.channel_count = play_track_channel;
	track_config.buffer_bytes = track_buf_size;
	RTAudioTrack_Init(audio_track, &track_config, RTAUDIO_OUTPUT_FLAG_NONE);

	/*for mixer version, this mean sw volume, for passthrough version, sw volume is not supported*/
	RTAudioTrack_SetVolume(audio_track, 1.0, 1.0);
	RTAudioTrack_SetStartThresholdBytes(audio_track, track_buf_size);

	if (RTAudioTrack_Start(audio_track) != AUDIO_OK) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Audio track start fail\n");
		return;
	}

	// RTK_LOGS(TAG, RTK_LOG_DEBUG,"UAC stop %d\n", usbd_comp_audio_task_stop);
#if 0
	memset(composite_usbd_uac_play_buf, 0x00, COMP_USBD_AUDIO_BUF_SIZE);
	for (u8 i = 0 ; i < (2 * track_buf_size) / COMP_USBD_AUDIO_BUF_SIZE ; i ++) {
		RTAudioTrack_Write(audio_track, (u8 *)composite_usbd_uac_play_buf, COMP_USBD_AUDIO_BUF_SIZE, true);
	}
#endif
	RTK_LOGS(TAG, RTK_LOG_INFO, "Audio track will loop to write\n");
	while (!usbd_comp_audio_task_stop) {
		read_dat_len = usbd_composite_uac_read(composite_usbd_uac_recv_buf, COMP_USBD_AUDIO_BUF_SIZE, 500, &zero_pkt);
		if (zero_pkt) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Audio track start %d-0x%08x\n", read_dat_len, zero_pkt);
			zero_pkt = 0;
		}

		if (read_dat_len > 0) {
#if CONFIG_USBD_UAC_DEMUX_CH_DEBUG
			play_data_size = 0;
			//get the 2 channel data from the 4 channel
			for (idx = 0, off = 0; idx < read_dat_len; idx += audio_src_step, off += audio_dst_step) {
				// ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3
				// 24  24  24  24  24  24  24  24  24  24  24  24
				memcpy((void *)(composite_usbd_uac_play_buf + off), (void *)(composite_usbd_uac_recv_buf + idx), audio_dst_step);
				play_data_size += audio_dst_step;
			}

			RTAudioTrack_Write(audio_track, (u8 *)composite_usbd_uac_play_buf, play_data_size, true);
			//RTK_LOGS(TAG, RTK_LOG_DEBUG,"Audio track start %d-%d\n",read_dat_len,play_data_size);
#else
			RTAudioTrack_Write(audio_track, (u8 *)composite_usbd_uac_recv_buf, read_dat_len, true);
#endif
		}
	}
	// RTK_LOGS(TAG, RTK_LOG_DEBUG,"Audio track exit\n");
	usbd_composite_uac_stop_play();

	RTAudioTrack_Pause(audio_track);
	RTAudioTrack_Flush(audio_track);
	RTAudioTrack_Stop(audio_track);
	RTAudioTrack_Destroy(audio_track);

	audio_track = NULL;
#else
	u32 total_len = 0;
	u32 read_cnt = 0;
	while (!usbd_comp_audio_task_stop) {
		read_dat_len = usbd_composite_uac_read(composite_usbd_uac_recv_buf, COMP_USBD_AUDIO_BUF_SIZE, 500, &zero_pkt);
		if (zero_pkt) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Audio track start %d-0x%08x\n", read_dat_len, zero_pkt);
			zero_pkt = 0;
		}
		read_cnt ++;
		if (read_dat_len > 0) {
			total_len += read_dat_len;
			if (read_cnt % 200 == 0) {
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "Audio track get %d %d\n", read_dat_len, total_len);
			}
		} else {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Audio Read Timeout\n");
			rtos_time_delay_ms(1);
		}
	}
#endif

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Audio track demo stop\n\n\n");
}

static void composite_usbd_audio_track_thread(void *param)
{
	UNUSED(param);

	//wait usb init success
	do {
		/*wait uac class alive*/
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "USBD wait uac init\n");
		do {
			rtos_time_delay_ms(1000);
		} while (uac_ready_flag == 0);
		uac_ready_flag = 0;

		usbd_comp_uac_class_alive = 1;

		do {
			do {
				rtos_time_delay_ms(1000);
			} while (uac_play_start_flag == 0);
			uac_play_start_flag = 0;

			usbd_comp_audio_task_stop = 0;
			composite_usbd_audio_track_play();
		} while (usbd_comp_uac_class_alive);
	} while (1);

	rtos_task_delete(NULL);
}


static void composite_usbd_init_thread(void *param)
{
	UNUSED(param);
	int ret = 0;
	rtos_task_t task;

	hid_transmit_flag = 1;

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

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "USBD COMP demo start\n");

	rtos_task_delete(NULL);

	return;

exit_create_check_task_fail:
	usbd_composite_deinit();

exit_usbd_composite_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "USBD COMP demo stop\n");
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

static void composite_usbd_hid_test_send_data(void)
{
	u8 i;
	int loop = 0;
	u8 array_len = sizeof(composite_hid_report_data) / sizeof(composite_usbd_hid_report_data_t);

	do {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Test round %d/%d\n", loop + 1, CONFIG_USBD_HID_CONSTANT_LOOP);
		for (i = 0; i < array_len; i++) {
			composite_usbd_flag_check(&hid_transmit_flag);
			usbd_composite_hid_send_data((u8 *)&composite_hid_report_data[i], sizeof(composite_usbd_hid_report_data_t));
		}
		// rtos_time_delay_ms(500); //next loop
	} while (++loop < CONFIG_USBD_HID_CONSTANT_LOOP);

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Test done\n");
}

static u32 composite_usbd_comp_hid_test(u16 argc, u8 *argv[])
{
	int status = HAL_OK;
	const char *cmd;

	if (argc == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid argument\n");
		return HAL_ERR_PARA;
	}

	cmd = (const char *)argv[0];

	if (_stricmp(cmd, "tx") == 0) {
		u16 size = 10;
		if (argv[1]) {
			size = (u16)_strtoul((const char *)(argv[1]), (char **)NULL, 10);
		}
		if (size > 1024) {
			size = 1024;
		}
		composite_usbd_flag_check(&hid_transmit_flag);
		memset(&hid_report_buf[0], (u8)(size & 0xFF), size);
		usbd_composite_hid_send_data((u8 *)&hid_report_buf[0], size);
	} else if (_stricmp(cmd, "vol1") == 0) {
		usbd_composite_hid_volume_ctrl(1);
	} else if (_stricmp(cmd, "vol2") == 0) {
		usbd_composite_hid_volume_ctrl(0);
	} else if (_stricmp(cmd, "pow") == 0) {
		usbd_composite_hid_power_ctrl();
	} else if (_stricmp(cmd, "ver") == 0) {
		composite_usbd_hid_test_send_data();
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Input cmd err %s\n", cmd);
	}

	return status;
}

static void composite_usbd_hid_rx_thread(void *param)
{
	UNUSED(param);
	u32 read_dat_len = 0;

	/*wait hid class alive*/
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "USBD wait hid init\n");

	do {
		rtos_time_delay_ms(1000);
	} while (hid_ready_flag == 0);
	hid_ready_flag = 0;

	RTK_LOGS(TAG, RTK_LOG_INFO, "HID init success\n");

	//wait usb init success
	while (!usbd_comp_hid_task_stop) {
		read_dat_len = usbd_composite_hid_read(hid_rx_buf, USBD_COMP_HID_MAX_BUF_SIZE, 1000);
		if (read_dat_len > 0) {
			composite_hid_cb_received(hid_rx_buf, read_dat_len);
		} else {
			rtos_time_delay_ms(1);
		}
	}

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

	usbd_comp_uac_class_alive = 0;
	uac_ready_flag = 0;
	uac_play_start_flag = 0;

	ret = rtos_task_create(&task, "composite_usbd_init_thread", composite_usbd_init_thread, NULL, 1024, CONFIG_USBD_COMPOSITE_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD COMP thread fail\n");
	}

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
	{
		(const u8 *)"USBD", 3, composite_usbd_comp_hid_test, (const u8 *)"\tUSB device test cmd:\n"
		"\t\t usbd tx\n"
	}
};
