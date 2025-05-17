/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "usbd.h"
#if defined(CONFIG_USBD_UAC1)
#include "usbd_uac1.h"
#else
#include "usbd_uac2.h"
#endif
/* This used to check the USB issue */
#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBADPLUS)
#define CONFIG_USBD_AUDIO_EN                          1
#else
#define CONFIG_USBD_AUDIO_EN                          0
#endif

#if  CONFIG_USBD_AUDIO_EN
#include "audio/audio_control.h"
#include "audio/audio_equalizer.h"
#include "audio/audio_track.h"
#include "audio/audio_service.h"
#include "common/audio_errnos.h"
#endif

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "UAC";

/*
	This configuration is used to enable a thread to check hotplug event
	and reset USB stack to avoid memory leak, only for example.
*/
#define CONFIG_USBD_UAC_HOTPLUG    0
/*
	This configuration is used to choose one channel to play
	for the audio does not support some channel, Such as 4 chs
	while enable this configuration,choose the first ch to play
*/
#define CONFIG_USBD_UAC_DEMUX_CH_DEBUG   1

/* USB speed */
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define CONFIG_USBD_UAC_SPEED USB_SPEED_FULL
#elif defined(CONFIG_USBD_UAC1)
/* UAC 1.0 spec supports only Full Speed. */
#define CONFIG_USBD_UAC_SPEED USB_SPEED_HIGH_IN_FULL
#else
#define CONFIG_USBD_UAC_SPEED USB_SPEED_HIGH
#endif

/* Thread priorities */
#define CONFIG_USBD_UAC_PLAYER_THREAD_PRIORITY  6U
#define CONFIG_USBD_UAC_INIT_THREAD_PRIORITY    6U
#define CONFIG_USBD_UAC_HOTPLUG_THREAD_PRIORITY 8U

#define AUDIO_BYTE_WIDTH_SIZE                   0x02U
#define AUDIO_SAMPLING_FREQ                     USBD_UAC_SAMPLING_FREQ_48K
#define AUDIO_CHANNEL_NUM                       USBD_UAC_DEFAULT_CH_CNT

/* ms */
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define USB_AUDIO_MS_BUF_SIZE               1023U
#else
/* the buffer size maybe need to be changed if format( AUDIO_BYTE_WIDTH_SIZE * AUDIO_CHANNEL_NUM num * AUDIO_SAMPLING_FREQ) changes */
//#define USB_AUDIO_MS_BUF_SIZE             ((AUDIO_BYTE_WIDTH_SIZE) * (AUDIO_CHANNEL_NUM) * (AUDIO_SAMPLING_FREQ) / 1000)
#define USB_AUDIO_MS_BUF_SIZE               4000U
#endif
#define USB_AUDIO_BUF_SIZE                  ((USB_AUDIO_MS_BUF_SIZE) * 3)

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int uac_cb_init(void);
static int uac_cb_deinit(void);
static int uac_cb_setup(usb_setup_req_t *req, u8 *buf);
static int uac_cb_set_config(void);

static void uac_cb_status_changed(u8 old_status, u8 status);
static void uac_cb_mute_changed(u8 mute);
static void uac_cb_volume_changed(u8 volume);
static void uac_cb_format_changed(u32 sampling_freq, u8 ch_cnt, u8 byte_width);
/* Private variables ---------------------------------------------------------*/

#if CONFIG_USBD_UAC_HOTPLUG
static u8 uac_attach_status;
static rtos_sema_t uac_attach_status_changed_sema;
#endif

static rtos_sema_t uac_ready_sema;

static volatile u8 audio_task_stop = 0;

#if CONFIG_USBD_AUDIO_EN
/*
	the buffer which will write to the audio track
	the basic length is USB_AUDIO_BUF_SIZE
*/
static u8 play_buf[USB_AUDIO_BUF_SIZE];
#endif

/*
	uac isoc rx buffer, used to save the audio data that received from the host
	the recv_buffer length is 2*USB_AUDIO_BUF_SIZE
	the play task will write the USB_AUDIO_BUF_SIZE length data to audiotack once, it switchs like a ping pang ball
*/
static u8 recv_buf[USB_AUDIO_BUF_SIZE * 2];


static usbd_config_t uac_cfg = {
	.speed = CONFIG_USBD_UAC_SPEED,
	.dma_enable = 1U,
	.isr_priority = INT_PRI_MIDDLE,
	.ext_intr_en = 0,
	.intr_use_ptx_fifo = 0U,
};

static usbd_uac_cb_t uac_cb = {
	.audio_ctx = NULL,
	.in = {.enable = 0,}, /* current just support usb out,usb in TODO */
	.out = {.enable = 1, .sampling_freq = AUDIO_SAMPLING_FREQ, .byte_width = AUDIO_BYTE_WIDTH_SIZE, .ch_cnt = AUDIO_CHANNEL_NUM},
	.init = uac_cb_init,
	.deinit = uac_cb_deinit,
	.setup = uac_cb_setup,
	.set_config = uac_cb_set_config,
	.status_changed = uac_cb_status_changed,
	.mute_changed = uac_cb_mute_changed,
	.volume_changed = uac_cb_volume_changed,
	.format_changed = uac_cb_format_changed,
	.sof = NULL,
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Handle the uac class control requests
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @param  value: Value for the command code
  * @retval Status
  */
static int uac_cb_setup(usb_setup_req_t *req, u8 *buf)
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
static int uac_cb_init(void)
{
	return HAL_OK;
}

/**
  * @brief  DeInitializes uac application layer
  * @param  None
  * @retval Status
  */
static int uac_cb_deinit(void)
{
	usbd_uac_stop_play();
	return HAL_OK;
}

/**
  * @brief  Set config callback
  * @param  None
  * @retval Status
  */
static int uac_cb_set_config(void)
{
	//usb ready
	return HAL_OK;
}

static void uac_cb_status_changed(u8 old_status, u8 status)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Status change: %d -> %d \n", old_status, status);
#if CONFIG_USBD_UAC_HOTPLUG
	uac_attach_status = status;
	rtos_sema_give(uac_attach_status_changed_sema);
#endif
}

#if CONFIG_USBD_UAC_HOTPLUG
static void uac_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(uac_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (uac_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");
				usbd_uac_deinit();
				ret = usbd_deinit();
				if (ret != 0) {
					break;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				ret = usbd_init(&uac_cfg);
				if (ret != 0) {
					break;
				}
				ret = usbd_uac_init(&uac_cb);
				if (ret != 0) {
					usbd_deinit();
					break;
				}
			} else if (uac_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Hotplug thread fail\n");
	rtos_task_delete(NULL);
}
#endif // CONFIG_USBD_UAC_HOTPLUG

static void example_usbd_uac_thread(void *param)
{
	UNUSED(param);
	int ret = 0;

#if CONFIG_USBD_UAC_HOTPLUG
	rtos_task_t check_status_task;
	rtos_sema_create(&uac_attach_status_changed_sema, 0U, 1U);
#endif

	ret = usbd_init(&uac_cfg);
	if (ret != HAL_OK) {
		goto exit;
	}

	ret = usbd_uac_init(&uac_cb);
	if (ret != HAL_OK) {
		goto clear_usb_driver_exit;
	}

#if CONFIG_USBD_UAC_HOTPLUG
	ret = rtos_task_create(&check_status_task, "uac_hotplug_thread", uac_hotplug_thread, NULL, 1024U, CONFIG_USBD_UAC_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_usb_class_exit;
	}
#endif // CONFIG_USBD_UAC_HOTPLUG

	rtos_time_delay_ms(100);

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD uac demo start\n");

	rtos_task_delete(NULL);

	return;

#if CONFIG_USBD_UAC_HOTPLUG
	rtos_task_delete(check_status_task);

clear_usb_class_exit:
#endif
	usbd_uac_deinit();

clear_usb_driver_exit:
	usbd_deinit();

exit:
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD uac demo stop\n");
#if CONFIG_USBD_UAC_HOTPLUG
	rtos_sema_delete(uac_attach_status_changed_sema);
#endif
	rtos_task_delete(NULL);
}

static void uac_cb_mute_changed(u8 mute)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD set mute %d\n", mute);
}

static void uac_cb_volume_changed(u8 volume)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD set volume %d\n", volume);
}

static void uac_cb_format_changed(u32 sampling_freq, u8 ch_cnt, u8 byte_width)
{
	if (sampling_freq != 0U) {
		uac_cb.out.sampling_freq = sampling_freq;
	}
	if (ch_cnt != 0U) {
		uac_cb.out.ch_cnt = ch_cnt;
	}
	if (byte_width != 0U) {
		uac_cb.out.byte_width = byte_width;
	}
	rtos_sema_give(uac_ready_sema);
	audio_task_stop = 1;
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD set sampling_freq %d set ch_cnt %d\n", sampling_freq, ch_cnt);
}
/* playback , USB OUT */
static void example_audio_track_play(void)
{
	u32 read_dat_len = 0;
	RTK_LOGS(TAG, RTK_LOG_INFO, "Audio track demo begin\n");

	usbd_uac_config(&(uac_cb.out), 0, 0);
	do {
		if (usbd_uac_start_play() == HAL_OK) {
			break;
		}
	} while (1);

#if  CONFIG_USBD_AUDIO_EN
	struct RTAudioTrack *audio_track;
	uint32_t format;
	int32_t track_buf_size;

	/* audio trace config params */
	uint32_t g_track_rate = uac_cb.out.sampling_freq;
	uint32_t g_track_channel = uac_cb.out.ch_cnt;
	uint32_t g_track_format = uac_cb.out.byte_width * 8;

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
		read_dat_len = usbd_uac_read(recv_buf, USB_AUDIO_BUF_SIZE * 2, 500);
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

	usbd_uac_stop_play();

	RTAudioTrack_Pause(audio_track);
	RTAudioTrack_Flush(audio_track);
	RTAudioTrack_Stop(audio_track);
	RTAudioTrack_Destroy(audio_track);

	audio_track = NULL;
#else
	u32 total_len = 0;
	u32 read_cnt = 0;
	while (!audio_task_stop) {
		read_dat_len = usbd_uac_read(recv_buf, USB_AUDIO_BUF_SIZE * 2, 500);
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

	RTK_LOGS(TAG, RTK_LOG_INFO, "Audio track demo stop\n\n\n");
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

/* Exported functions --------------------------------------------------------*/

void example_usbd_uac(void)
{
	int status;
	rtos_task_t task;

	rtos_sema_create(&uac_ready_sema, 0U, 1U);
	status = rtos_task_create(&task, "example_usbd_uac_thread", example_usbd_uac_thread, NULL, 1024U, CONFIG_USBD_UAC_INIT_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD uac thread fail\n");
	}

	if (rtos_task_create(NULL, ((const char *)"example_audio_track_thread"), example_audio_track_thread, NULL, 1024 * 16,
						 CONFIG_USBD_UAC_PLAYER_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create audio track fail\n");
	}
}