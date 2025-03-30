/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */
/* This used to check the USB issue */
#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBADPLUS)
#define CONFIG_USBD_COMPOSITE_AUDIO_EN                          1
#else
#define CONFIG_USBD_COMPOSITE_AUDIO_EN                          0
#endif

#include <platform_autoconf.h>
#include "usbd_composite_cdc_acm_uac.h"
#include "os_wrapper.h"
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "platform_stdlib.h"
#include "basic_types.h"

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
#define CONFIG_USBD_COMPOSITE_HOTPLUG							0
/*
	This configuration is used to choose one channel to play
	for the audio does not support some channel, Such as 4 chs
	while enable this configuration,choose the first ch to play
*/
#define CONFIG_USBD_UAC_DEMUX_CH_DEBUG   1

// USB speed
#ifdef CONFIG_USB_FS
#define CONFIG_USBD_COMPOSITE_SPEED								USB_SPEED_FULL
#else
#define CONFIG_USBD_COMPOSITE_SPEED								USB_SPEED_HIGH
#endif

// Thread priorities
#define CONFIG_USBD_COMPOSITE_UAC_THREAD_PRIORITY               4U
#define CONFIG_USBD_COMPOSITE_INIT_THREAD_PRIORITY				5U
#define CONFIG_USBD_COMPOSITE_HOTPLUG_THREAD_PRIORITY			8U

#define CONFIG_USBD_COMPOSITE_UAC_ACM_BULK_IN_XFER_SIZE		2048U
#define CONFIG_USBD_COMPOSITE_UAC_ACM_BULK_OUT_XFER_SIZE	2048U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void composite_cb_status_changed(u8 status);
static int composite_cb_set_config(void);

static int composite_cdc_acm_cb_init(void);
static int composite_cdc_acm_cb_deinit(void);
static int composite_cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int composite_cdc_acm_cb_received(u8 *buf, u32 Len);

static int composite_uac_cb_init(void);
static int composite_uac_cb_deinit(void);
static int composite_uac_cb_setup(usb_setup_req_t *req, u8 *buf);
static int composite_uac_cb_set_config(void);

static int composite_uac_cb_mute_changed(u8 mute);
static int composite_uac_cb_volume_changed(u8 volume);
static int composite_uac_cb_format_changed(u32 freq, u8 ch_cnt, u8 byte_width);

/* Private variables ---------------------------------------------------------*/
static usbd_config_t composite_cfg = {
	.speed = CONFIG_USBD_COMPOSITE_SPEED,
	.dma_enable = 1U,
	.isr_priority = INT_PRI_MIDDLE,
	.intr_use_ptx_fifo = 0U,
	.nptx_max_epmis_cnt = 100U,
};

static usbd_composite_cb_t composite_cb = {
	.status_changed = composite_cb_status_changed,
	.set_config = composite_cb_set_config,
};

static usbd_composite_cdc_acm_usr_cb_t composite_cdc_acm_usr_cb = {
	.init = composite_cdc_acm_cb_init,
	.deinit = composite_cdc_acm_cb_deinit,
	.setup = composite_cdc_acm_cb_setup,
	.received = composite_cdc_acm_cb_received
};

static usbd_composite_cdc_acm_line_coding_t composite_cdc_acm_line_coding;

static u16 composite_cdc_acm_ctrl_line_state;

#ifdef CONFIG_USB_FS
#define COMP_USBD_AUDIO_MS_BUF_SIZE               1023U
#else
/* the buffer size maybe need to be changed if format( AUDIO_BYTE_WIDTH_SIZE * AUDIO_CHANNEL_NUM num * AUDIO_SAMPLING_RATE) changes */
//#define COMP_USBD_AUDIO_MS_BUF_SIZE             ((AUDIO_BYTE_WIDTH_SIZE) * (AUDIO_CHANNEL_NUM) * (AUDIO_SAMPLING_RATE) / 1000)
#define COMP_USBD_AUDIO_MS_BUF_SIZE               1024U
#endif
#define COMP_USBD_AUDIO_BUF_SIZE                  ((COMP_USBD_AUDIO_MS_BUF_SIZE) * 1)

/*  UAC */
static rtos_sema_t uac_ready_sema;

static volatile u8 audio_task_stop = 0;

#if  CONFIG_USBD_COMPOSITE_AUDIO_EN
/*
    the buffer which will write to the audio track
    the basic length is COMP_USBD_AUDIO_BUF_SIZE
*/
static u8 play_buf[COMP_USBD_AUDIO_BUF_SIZE];
#endif
/*
	uac isoc rx buffer, used to save the audio data that received from the host
	the recv_buffer length is 2*COMP_USBD_AUDIO_BUF_SIZE
	the play task will write the COMP_USBD_AUDIO_BUF_SIZE length data to audiotack once, it switchs like a ping pang ball
*/
static u8 recv_buf[COMP_USBD_AUDIO_BUF_SIZE * 2];

static usbd_composite_uac_usr_cb_t composite_uac_usr_cb = {
	.audio_ctx = NULL,
	.in = {.enable = 0,}, /* current just support usb out,usb in TODO */
	.out = {.enable = 1, },
	.init = composite_uac_cb_init,
	.deinit = composite_uac_cb_deinit,
	.setup = composite_uac_cb_setup,
	.set_config = composite_uac_cb_set_config,
	.mute_changed = composite_uac_cb_mute_changed,
	.volume_changed = composite_uac_cb_volume_changed,
	.format_changed = composite_uac_cb_format_changed,
};

#if CONFIG_USBD_COMPOSITE_HOTPLUG
static u8 composite_attach_status;
static rtos_sema_t composite_attach_status_changed_sema;
#endif

/* Private functions ---------------------------------------------------------*/
static void composite_cb_status_changed(u8 status)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Status change: %d\n", status);
#if CONFIG_USBD_COMPOSITE_HOTPLUG
	composite_attach_status = status;
	rtos_sema_give(composite_attach_status_changed_sema);
#endif
}

static int composite_cb_set_config(void)
{
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "USB Set Cfg\n");
	return HAL_OK;
}

#if CONFIG_USBD_COMPOSITE_HOTPLUG
static void composite_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(composite_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (composite_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");
				usbd_composite_deinit();
				ret = usbd_deinit();
				if (ret != 0) {
					break;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				ret = usbd_init(&composite_cfg);
				if (ret != 0) {
					break;
				}
				ret = usbd_composite_init(CONFIG_USBD_COMPOSITE_UAC_ACM_BULK_OUT_XFER_SIZE,
										  CONFIG_USBD_COMPOSITE_UAC_ACM_BULK_IN_XFER_SIZE,
										  &composite_cdc_acm_usr_cb,
										  &composite_uac_usr_cb,
										  &composite_cb);
				if (ret != 0) {
					usbd_deinit();
					break;
				}
			} else if (composite_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\n");
	rtos_task_delete(NULL);
}
#endif // CONFIG_USBD_COMPOSITE_HOTPLUG

/**
  * @brief  Initializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int composite_cdc_acm_cb_init(void)
{
	usbd_composite_cdc_acm_line_coding_t *lc = &composite_cdc_acm_line_coding;

	lc->bitrate = 150000;
	lc->format = 0x00;
	lc->parity_type = 0x00;
	lc->data_type = 0x08;

	return HAL_OK;
}

/**
  * @brief  DeInitializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int composite_cdc_acm_cb_deinit(void)
{
	return HAL_OK;
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface through this function.
  * @param  Buf: RX buffer
  * @param  Len: RX data length (in bytes)
  * @retval Status
  */
static int composite_cdc_acm_cb_received(u8 *buf, u32 len)
{
	return usbd_composite_cdc_acm_transmit(buf, len);
}

/**
  * @brief  Handle the CDC class control requests
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @retval Status
  */
static int composite_cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	int ret = HAL_OK;
	usbd_composite_cdc_acm_line_coding_t *lc = &composite_cdc_acm_line_coding;

	switch (req->bRequest) {
	case COMP_CDC_SEND_ENCAPSULATED_COMMAND:
		/* Do nothing */
		break;

	case COMP_CDC_GET_ENCAPSULATED_RESPONSE:
		/* Do nothing */
		break;

	case COMP_CDC_SET_COMM_FEATURE:
		/* Do nothing */
		break;

	case COMP_CDC_GET_COMM_FEATURE:
		/* Do nothing */
		break;

	case COMP_CDC_CLEAR_COMM_FEATURE:
		/* Do nothing */
		break;

	case COMP_CDC_SET_LINE_CODING:
		if (req->wLength == COMP_CDC_ACM_LINE_CODING_SIZE) {
			lc->bitrate = (u32)(buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));
			lc->format = buf[4];
			lc->parity_type = buf[5];
			lc->data_type = buf[6];
		}
		break;

	case COMP_CDC_GET_LINE_CODING:
		buf[0] = (u8)(lc->bitrate & 0xFF);
		buf[1] = (u8)((lc->bitrate >> 8) & 0xFF);
		buf[2] = (u8)((lc->bitrate >> 16) & 0xFF);
		buf[3] = (u8)((lc->bitrate >> 24) & 0xFF);
		buf[4] = lc->format;
		buf[5] = lc->parity_type;
		buf[6] = lc->data_type;
		break;

	case COMP_CDC_SET_CONTROL_LINE_STATE:
		/*
		wValue:	wValue, Control Signal Bitmap
				D2-15:	Reserved, 0
				D1:	RTS, 0 - Deactivate, 1 - Activate
				D0:	DTR, 0 - Not Present, 1 - Present
		*/
		composite_cdc_acm_ctrl_line_state = req->wValue;
		if (composite_cdc_acm_ctrl_line_state & 0x01) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "VCOM port activate\n");
		}
		break;

	case COMP_CDC_SEND_BREAK:
		/* Do nothing */
		break;

	default:
		RTK_LOGS(TAG, RTK_LOG_WARN, "Invalid CDC bRequest 0x%02x\n", req->bRequest);
		ret = HAL_ERR_PARA;
		break;
	}

	return ret;
}

/**
  * @brief  Handle the uac class control requests
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @param  value: Value for the command code
  * @retval Status
  */
static int composite_uac_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	UNUSED(req);
	UNUSED(buf);

	return HAL_OK;
}
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
	usbd_composite_uac_stop();
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
	return HAL_OK;
}

static int composite_uac_cb_mute_changed(u8 mute)
{
	UNUSED(mute);
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD set mute %d\n", mute);
	return HAL_OK;
}

static int composite_uac_cb_volume_changed(u8 volume)
{
	UNUSED(volume);
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD set volume %d\n", volume);
	return HAL_OK;
}

static int composite_uac_cb_format_changed(u32 freq, u8 ch_cnt, u8 byte_width)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD set freq %d set ch_cnt %d\n", freq, ch_cnt);
	composite_uac_usr_cb.out.freqence = freq;
	composite_uac_usr_cb.out.ch_cnt = ch_cnt;
	composite_uac_usr_cb.out.byte_width = byte_width;
	rtos_sema_give(uac_ready_sema);
	audio_task_stop = 1;

	return HAL_OK;
}

/* playback , USB OUT */
static void example_audio_track_play(void)
{
	u32 read_dat_len = 0;
	u32 zero_pkt = 0;

	RTK_LOGS(TAG, RTK_LOG_INFO, "Audio track demo begin\n");

	usbd_composite_uac_audio_cfg_init(&(composite_uac_usr_cb.out), 0, 0);
	do {
		if (usbd_composite_uac_start() == HAL_OK) {
			break;
		}
	} while (1);

#if  CONFIG_USBD_COMPOSITE_AUDIO_EN

	struct RTAudioTrack *audio_track;
	uint32_t format;
	int32_t track_buf_size;

	/* audio trace config params */
	uint32_t g_track_rate = composite_uac_usr_cb.out.freqence;
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

	track_buf_size = RTAudioTrack_GetMinBufferBytes(audio_track, RTAUDIO_CATEGORY_MEDIA, g_track_rate, format, play_track_channel) * 4;
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

	RTK_LOGS(TAG, RTK_LOG_INFO, "Track buf size:%d\n", track_buf_size);

	/*for mixer version, this mean sw volume, for passthrough version, sw volume is not supported*/
	RTAudioTrack_SetVolume(audio_track, 1.0, 1.0);
	RTAudioTrack_SetStartThresholdBytes(audio_track, track_buf_size);

	if (RTAudioTrack_Start(audio_track) != AUDIO_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Audio track start fail\n");
		return;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC stop %d\n", audio_task_stop);

	while (!audio_task_stop) {
		read_dat_len = usbd_composite_uac_read(recv_buf, COMP_USBD_AUDIO_BUF_SIZE * 2, 500, &zero_pkt);
		if (read_dat_len > 0) {
#if CONFIG_USBD_UAC_DEMUX_CH_DEBUG
			play_data_size = 0;
			//get the 2 channel data from the 4 channel
			for (idx = 0, off = 0; idx < read_dat_len; idx += audio_src_step, off += audio_dst_step) {
				// ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3
				// 24  24  24  24  24  24  24  24  24  24  24  24
				memcpy((void *)(play_buf + off), (void *)(recv_buf + idx), audio_dst_step);
				play_data_size += audio_dst_step;
			}

			RTAudioTrack_Write(audio_track, (u8 *)play_buf, play_data_size, true);
			//RTK_LOGS(TAG, RTK_LOG_INFO, "Audio track start %d-%d\n",read_dat_len,play_data_size);
#else
			RTAudioTrack_Write(audio_track, (u8 *)recv_buf, read_dat_len, true);
#endif
		}
	}

	usbd_composite_uac_stop();

	RTAudioTrack_Pause(audio_track);
	RTAudioTrack_Flush(audio_track);
	RTAudioTrack_Stop(audio_track);
	RTAudioTrack_Destroy(audio_track);

	audio_track = NULL;
#else
	u32 total_len = 0;
	u32 read_cnt = 0;
	while (!audio_task_stop) {
		read_dat_len = usbd_composite_uac_read(recv_buf, COMP_USBD_AUDIO_BUF_SIZE, 500, &zero_pkt);
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

static void example_audio_track_thread(void *param)
{
	UNUSED(param);

	//should wait usb init success
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

static void example_usbd_composite_thread(void *param)
{
	UNUSED(param);
	int ret = 0;
#if CONFIG_USBD_COMPOSITE_HOTPLUG
	rtos_task_t task;

	rtos_sema_create(&composite_attach_status_changed_sema, 0U, 1U);
#endif

	ret = usbd_init(&composite_cfg);
	if (ret != HAL_OK) {
		goto exit_usbd_init_fail;
	}

	ret = usbd_composite_init(CONFIG_USBD_COMPOSITE_UAC_ACM_BULK_OUT_XFER_SIZE,
							  CONFIG_USBD_COMPOSITE_UAC_ACM_BULK_IN_XFER_SIZE,
							  &composite_cdc_acm_usr_cb,
							  &composite_uac_usr_cb,
							  &composite_cb);
	if (ret != HAL_OK) {
		goto exit_usbd_composite_init_fail;
	}

#if CONFIG_USBD_COMPOSITE_HOTPLUG
	ret = rtos_task_create(&task, "composite_hotplug_thread", composite_hotplug_thread, NULL,
						   1024, CONFIG_USBD_COMPOSITE_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_create_check_task_fail;
	}
#endif

	rtos_time_delay_ms(100);

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD COMP demo start\n");

	rtos_task_delete(NULL);

	return;

#if CONFIG_USBD_COMPOSITE_HOTPLUG
exit_create_check_task_fail:
	usbd_composite_deinit();
#endif

exit_usbd_composite_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD COMP demo stop\n");
#if CONFIG_USBD_COMPOSITE_HOTPLUG
	rtos_sema_delete(composite_attach_status_changed_sema);
#endif
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

	rtos_sema_create(&uac_ready_sema, 0U, 1U);
	ret = rtos_task_create(&task, "example_usbd_composite_thread", example_usbd_composite_thread, NULL, 1024, CONFIG_USBD_COMPOSITE_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD COMP thread fail\n");
	}

	if (rtos_task_create(NULL, ((const char *)"example_audio_track_thread"), example_audio_track_thread, NULL, 1024 * 16,
						 CONFIG_USBD_COMPOSITE_UAC_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create audio track fail\n");
	}
}

