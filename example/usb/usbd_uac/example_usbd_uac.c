/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "usbd_uac.h"
#include "atcmd_service.h"
#include "usb_app.h"
#include "example_usbd_uac.h"

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

// Endpoint address
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
#define USBD_UAC_RECORD_EP                       0x84U
#define USBD_UAC_PLAY_EP                         0x02U
#else
#define USBD_UAC_RECORD_EP                       0x81U
#define USBD_UAC_PLAY_EP                         0x02U
#endif

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define USBD_UAC_HOTPLUG                          1

// This configuration is used to choose one channel to play
// for the audio does not support some channel, Such as 4 chs
// while enable this configuration,choose the first ch to play
#define USBD_UAC_DEMUX_CH_DEBUG                   1

// Thread priorities
#define USBD_UAC_INIT_THREAD_PRIORITY             6
#define USBD_UAC_HOTPLUG_THREAD_PRIORITY          8
#define USBD_UAC_PLAYER_THREAD_PRIORITY           6

// Thread stack sizes
#define USBD_UAC_INIT_THREAD_STACK_SIZE           1024U
#define USBD_UAC_HOTPLUG_THREAD_STACK_SIZE        1024U
#define USBD_UAC_PLAYER_THREAD_STACK_SIZE         2300U

// Microphone (USB IN / record) demo, built with the class record path: CONFIG USB > UAC > Enable record (microphone)
#ifdef CONFIG_USBD_UAC_RECORD
#define USBD_UAC_ENABLE_RECORD                    1
#else
#define USBD_UAC_ENABLE_RECORD                    0
#endif

#if USBD_UAC_ENABLE_RECORD
#define USBD_UAC_RECORD_THREAD_PRIORITY           4
#define USBD_UAC_RECORD_THREAD_STACK_SIZE         (1024U * 4)
/* 1 ms of the 16kHz/2ch/16bit record format. usbd_uac_transmit_data() cuts it into per-bInterval
 * packets itself, so this is only the feeding granularity */
#define USBD_UAC_RECORD_CHUNK_LEN                 64U
#define USBD_UAC_RECORD_CHUNK_DELAY_MS            1U
/* "Realtek" clip replayed as the mic input, refer to example_usbd_uac_record_thread() */
#include "example_usbd_uac_record_audio_data.h"

/* The clip and USBD_UAC_RECORD_CHUNK_LEN are 16kHz/2ch/16bit, the format the mic reports shall match */
#if (USBD_UAC_RECORD_SAMPLING_FREQ != USBD_UAC_SAMPLING_FREQ_16K) || (USBD_UAC_RECORD_CHANNEL_COUNT != USBD_UAC_CHANNEL_COUNT_2) || \
	(USBD_UAC_RECORD_BYTE_WIDTH != USBD_UAC_BYTE_WIDTH_2)
#error "The built-in record clip is 16 kHz, 2 channels, 16-bit: select this record format in menuconfig, or replace the clip"
#endif

/* The record thread reads the table in whole chunks */
_Static_assert((sizeof(usbd_uac_record_audio_data) % USBD_UAC_RECORD_CHUNK_LEN) == 0U, "UAC record data len");
#endif

/* Data worker threads joined by a stop: the player, plus the record thread */
#define USBD_UAC_WORKER_CNT                       (1U + USBD_UAC_ENABLE_RECORD)

#define AUDIO_BYTE_WIDTH_SIZE                   USBD_UAC_PLAY_BYTE_WIDTH
#define AUDIO_SAMPLING_FREQ                     USBD_UAC_SAMPLING_FREQ_48K
#define AUDIO_CHANNEL_NUM                       USBD_UAC_PLAY_MAX_CHANNEL_COUNT

/* ms */
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define USB_AUDIO_MS_BUF_SIZE               1023U
#else
/* the buffer size maybe need to be changed if format( AUDIO_BYTE_WIDTH_SIZE * AUDIO_CHANNEL_NUM num * AUDIO_SAMPLING_FREQ) changes */
//#define USB_AUDIO_MS_BUF_SIZE             ((AUDIO_BYTE_WIDTH_SIZE) * (AUDIO_CHANNEL_NUM) * (AUDIO_SAMPLING_FREQ) / 1000)
#define USB_AUDIO_MS_BUF_SIZE               4000U
#endif
#define USB_AUDIO_BUF_SIZE                  ((USB_AUDIO_MS_BUF_SIZE) * 3U)

/* Private types -------------------------------------------------------------*/

/* Playback format selected by the host, refer to uac_cb_format_changed() */
typedef struct {
	u32 sampling_freq;
	u8 byte_width;
	u8 channel_count;
} uac_play_format_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

#if USBD_UAC_ENABLE_RECORD
static int uac_cmd_record(u16 argc, char *argv[]);
static void uac_cmd_record_at(u16 argc, char **argv);
#endif

static int uac_cb_init(void);
static int uac_cb_deinit(void);
static int uac_cb_setup(usb_setup_req_t *req, u8 *buf);
static int uac_cb_set_config(void);

static void uac_cb_status_changed(u8 old_status, u8 status);
static void uac_cb_mute_changed(u8 mute);
static void uac_cb_volume_changed(u8 volume);
static void uac_cb_format_changed(u32 sampling_freq, u8 channel_count, u8 byte_width);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UAC";

#if USBD_UAC_HOTPLUG
static rtos_sema_t uac_attach_status_changed_sema;
/* Written by the ISR, read by the hotplug thread, possibly on another core */
static __IO u8 uac_attach_status;
/* Set by the hotplug thread while the stack is torn down, so that a stop racing a failed
   reinit does not deinit it twice */
static __IO u8 uac_stack_down;
#endif
/* Start/stop state: example_usbd_uac_stop() raises quit, wakes every worker and joins them,
   the workers leave their loops on quit */
static usb_app_ctx_t uac_app;
static rtos_sema_t uac_ready_sema;
static __IO u8 uac_task_exiting = 0;
static __IO u8 uac_player_stop = 0;

/* Serialize the USB stack bring up/tear down against the worker threads: the player
 * thread holds uac_play_lock for a playback session, the record thread holds
 * uac_record_lock for a record session, and the hotplug thread takes both before it
 * touches the stack. One lock per direction rather than a single one, so that
 * playback and record still run at the same time.
 * Created once and never deleted, like uac_record_start_sema: the example may be
 * started again, and uac_stack_ready is the gate that keeps callers off the stack. */
static rtos_mutex_t uac_play_lock = NULL;
#if USBD_UAC_ENABLE_RECORD
static rtos_mutex_t uac_record_lock = NULL;
#endif
/* 1: usbd_init() and usbd_uac_init() both done, the class APIs may be called. Cleared
 * before any tear down, so a non-zero value also implies the locks are valid.
 * usbd_uac_deinit() frees the isoc ring buffers and their semaphores, which
 * usbd_uac_read()/usbd_uac_transmit_data() dereference; the bounded wait this replaces
 * could time out and deinit under a worker still inside the stack, so under SMP the
 * workers must be excluded from the whole teardown, not just raced against. */
static __IO u8 uac_stack_ready = 0;

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
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

static const usbd_config_t uac_cfg = {
	.info = {
#ifdef CONFIG_USBD_UAC1
		.prod_str = "Realtek UAC1.0 Device",
#else
		.prod_str = "Realtek UAC2.0 Device",
#endif
		/* High Speed, clamped to Full Speed by the core on an FS-only SoC */
		.speed = USB_SPEED_HIGH,
	},
	.isr_priority = INT_PRI_MIDDLE,
#if defined(CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 724U,
	.ptx_fifo_depth = {0U, 0U, 0U, 256U, },
#elif defined(CONFIG_RLE1509)
	.rx_fifo_depth = 688U,
	.ptx_fifo_depth = {0U, 0U, 0U, 256U, },
#elif defined (CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1680U,
	.ptx_fifo_depth = {16U, 256U, 16U, },
#endif
};

/* Handed to the class, which keeps the pointer: never written afterwards, refer to uac_play_format */
static const usbd_uac_config_t uac_class_cfg = {
	.play_ep_addr = USBD_UAC_PLAY_EP,
	.record_ep_addr = USBD_UAC_RECORD_EP,
};

/* Current playback format: rewritten by format_changed() in ISR context, possibly on another
   core, and snapshot by the player under the same lock */
static uac_play_format_t uac_play_format = {
	.sampling_freq = AUDIO_SAMPLING_FREQ,
	.byte_width = AUDIO_BYTE_WIDTH_SIZE,
	.channel_count = AUDIO_CHANNEL_NUM,
};

static usbd_uac_cb_t uac_cb = {
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

#if USBD_UAC_ENABLE_RECORD
static rtos_sema_t uac_record_start_sema;
#endif

/* Exported variables --------------------------------------------------------*/

#if USBD_UAC_ENABLE_RECORD
/* AT commands "AT+<cmd>=<p1>,<p2>...": the command name is case-sensitive, the parameters are not */
ATCMD_TABLE_DATA_SECTION
const log_item_t usbd_uac_cmd_table[] = {
	{"+USBDUAC", uac_cmd_record_at},
};
#endif /* USBD_UAC_ENABLE_RECORD */

/* Private functions ---------------------------------------------------------*/

#if USBD_UAC_HOTPLUG
/**
  * @brief  Release the locks taken around the stack tear down/bring up
  * @retval None
  */
static void uac_state_unlock(void)
{
#if USBD_UAC_ENABLE_RECORD
	rtos_mutex_give(uac_record_lock);
#endif
	rtos_mutex_give(uac_play_lock);
}
#endif

/**
  * @brief  Free the objects shared by the example threads
  * @note   Only called by the stop owner, after every worker has left and after the USB stack
  *         is fully deinited, so that neither a worker nor an ISR callback can touch them
  * @retval None
  */
static void uac_free_resource(void)
{
	rtos_sema_delete(uac_ready_sema);
	uac_ready_sema = NULL;
	/* uac_record_start_sema is kept: the console command gives it from the shell thread,
	   which the example can not join, so it is never provably unreferenced */
#if USBD_UAC_HOTPLUG
	rtos_sema_delete(uac_attach_status_changed_sema);
	uac_attach_status_changed_sema = NULL;
#endif
}

/**
  * @brief  Wake the worker threads so that they see quit and leave
  * @note   Called with uac_app.quit already raised. A worker is never deleted from outside:
  *         it may be inside the class APIs or hold a lock, it is joined instead.
  * @retval None
  */
static void uac_wake_workers(void)
{
	uac_stack_ready = 0;
	uac_task_exiting = 1;
	usbd_uac_stop_play();
#if USBD_UAC_ENABLE_RECORD
	usbd_uac_stop_record();
	rtos_sema_give(uac_record_start_sema);
#endif
	rtos_sema_give(uac_ready_sema);
}

/**
  * @brief  Handle the uac class control requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
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

	/* No vendor request is implemented by this example, reject so that the class
	   driver makes the core STALL EP0 instead of ACKing an unsupported request */
	return -RTK_ERR_BADARG;
}

/**
  * @brief  Initializes uac application layer
  * @param  None
  * @retval Status
  */
static int uac_cb_init(void)
{
	return RTK_SUCCESS;
}

/**
  * @brief  DeInitializes uac application layer
  * @param  None
  * @retval Status
  */
static int uac_cb_deinit(void)
{
	return RTK_SUCCESS;
}

/**
  * @brief  Set config callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  None
  * @retval Status
  */
static int uac_cb_set_config(void)
{
	//usb ready
	return RTK_SUCCESS;
}

/**
  * @brief  Handle UAC attach status change notifications from the USB stack
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  old_status: Previous attach status
  * @param  status: New attach status
  * @retval None
  */
static void uac_cb_status_changed(u8 old_status, u8 status)
{
	UNUSED(old_status);

#if USBD_UAC_HOTPLUG
	uac_attach_status = status;
	rtos_sema_give(uac_attach_status_changed_sema);
#else
	UNUSED(status);
#endif
}

#if USBD_UAC_HOTPLUG
static void example_usbd_uac_hotplug_thread(void *param)
{
	int ret = 0;
	u8 fatal = 0U;

	UNUSED(param);

#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* C-2: the USB OTG ISR is delivered on CPU0 (GIC ITARGETSR pins every SPI to core 0).
	   Pinning the hotplug/deinit thread to CPU0 puts it on the same core as the ISR,
	   so deinit's local interrupt disable is meaningful again under SMP. The player /
	   record data threads stay unaffined: a pending deinit is excluded from them by
	   uac_play_lock / uac_record_lock, not by core.
	   Done by the thread itself, first: a created thread may already run on another core, and its
	   creator pinning it afterwards could come too late or reach a task that has deleted itself. */
	rtos_task_set_affinity(rtos_task_handle_get(), 0);
#endif

	for (;;) {
		if (rtos_sema_take(uac_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (uac_app.quit != 0U) {
				/* Stop requested: the stack is left as it is, the stop entry tears it down */
				break;
			}
			if (uac_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");
				/* Ask the workers to leave their loops first: stop_play()/stop_record()
				   unblock a pending usbd_uac_read(), uac_task_exiting ends the loops.
				   Both must run while the stack is still up, so before the lock. */
				usbd_uac_stop_play();
#if USBD_UAC_ENABLE_RECORD
				usbd_uac_stop_record();
#endif
				uac_task_exiting = 1;

				/* Each worker holds its lock for as long as it is inside the class
				   APIs, so taking both here waits until neither of them is: unlike a
				   bounded wait, this can not time out and deinit under a worker that is
				   still running, which matters on SMP where a worker keeps running on
				   the other core. */
				rtos_mutex_take(uac_play_lock, RTOS_MAX_TIMEOUT);
#if USBD_UAC_ENABLE_RECORD
				rtos_mutex_take(uac_record_lock, RTOS_MAX_TIMEOUT);
#endif
				uac_stack_ready = 0;

				usbd_uac_deinit();
				usbd_deinit();
				uac_stack_down = 1U;
				if (uac_app.quit != 0U) {
					/* Stop requested while the stack is down: skip the re-init, the stop sees uac_stack_down */
					uac_state_unlock();
					break;
				}
				uac_task_exiting = 0;
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				ret = usbd_init(&uac_cfg);
				if (ret != 0) {
					uac_state_unlock();
					fatal = 1U;
					break;
				}
				ret = usbd_uac_init(&uac_cb, &uac_class_cfg);
				if (ret != 0) {
					usbd_deinit();
					uac_state_unlock();
					fatal = 1U;
					break;
				}
				uac_stack_down = 0U;
				uac_stack_ready = 1;
				uac_state_unlock();
			} else if (uac_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}
	if (fatal != 0U) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Hotplug thread fail\n");
		/* The stack is fully deinited here, no more ISR callback */
		if (usb_app_ctx_claim_stop(&uac_app) != 0U) {
			/* No stop is in flight: stop the workers blocked on the semaphores, then free
			   the shared objects as the last thread standing */
			uac_wake_workers();
			if (usb_app_ctx_join(&uac_app, USBD_UAC_WORKER_CNT) != RTK_SUCCESS) {
				/* A worker may still touch the shared objects: leak them */
				RTK_LOGS(TAG, RTK_LOG_WARN, "Wait worker exit timeout\n");
				rtos_task_delete(NULL);
			}
			uac_free_resource();
			usb_app_ctx_end(&uac_app);
			rtos_task_delete(NULL);
		}
	}
	usb_app_ctx_leave(&uac_app);
	rtos_task_delete(NULL);
}
#endif // USBD_UAC_HOTPLUG

/**
  * @brief  Handle UAC mute control changes from the host
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  mute: New mute state
  * @retval None
  */
static void uac_cb_mute_changed(u8 mute)
{
	UNUSED(mute);
	// RTK_LOGS(TAG, RTK_LOG_INFO, "USBD set mute %d\n", mute);
}

/**
  * @brief  Handle UAC volume control changes from the host
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  volume: New volume value
  * @retval None
  */
static void uac_cb_volume_changed(u8 volume)
{
	UNUSED(volume);
	// RTK_LOGS(TAG, RTK_LOG_INFO, "USBD set volume %d\n", volume);
}

/**
  * @brief  Handle UAC stream format (sampling rate / channel count / sample width) changes from the host
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  sampling_freq: New sampling frequency in Hz (0 means unchanged)
  * @param  channel_count: New channel count (0 means unchanged)
  * @param  byte_width: New sample byte width (0 means unchanged)
  * @retval None
  */
static void uac_cb_format_changed(u32 sampling_freq, u8 channel_count, u8 byte_width)
{
	uac_play_format_t play_format;

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
	play_format = uac_play_format;
	rtos_critical_exit(RTOS_CRITICAL_USB);

	if ((play_format.sampling_freq != 0U) && (play_format.byte_width != 0U) && (play_format.channel_count != 0U)) {
		uac_player_stop = 1;
		usbd_uac_stop_play();
		rtos_sema_give(uac_ready_sema);
		//RTK_LOGS(TAG, RTK_LOG_INFO, "USBD set sampling_freq %d set channel_count %d\n", sampling_freq, channel_count);
	}
}
/* playback , USB OUT */
static void example_audio_track_play(void)
{
#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
	AudioTrackConfig track_config;
	struct AudioTrack *audio_track;
	u32 format;
	int track_buf_size;
	u32 track_rate;
	u32 track_channel;
	u32 track_format;
	u32 play_track_channel;
#if USBD_UAC_DEMUX_CH_DEBUG
	u32 idx = 0;
	u32 off = 0;
	u32 play_data_size;
	u32 audio_src_step;
	u32 audio_dst_step;
#endif
	uac_play_format_t play_format;
#else
	u32 total_len = 0;
	u32 read_cnt = 0;
#endif
	u32 read_dat_len = 0;

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB
	/* One consistent snapshot: format_changed() rewrites the fields from the ISR */
	rtos_critical_enter(RTOS_CRITICAL_USB);
	play_format = uac_play_format;
	rtos_critical_exit(RTOS_CRITICAL_USB);
#endif

	/* start_play() fails until the host has configured the device, and keeps failing once the
	   stack is torn down: leave on any stop request instead of retrying forever. Nothing to
	   unwind here, no AudioTrack has been created yet. */
	while (usbd_uac_start_play() != RTK_SUCCESS) {
		if ((uac_task_exiting != 0) || (uac_player_stop != 0) || (uac_app.quit != 0U)) {
			return;
		}
		rtos_time_delay_ms(5);
	}

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB

	/* audio trace config params */
	track_rate = play_format.sampling_freq;
	track_channel = play_format.channel_count;
	track_format = play_format.byte_width * 8;

	play_track_channel = track_channel;    //mix not support 4 channel

#if USBD_UAC_DEMUX_CH_DEBUG
	//force to get the 1st channel to play
	play_track_channel = 1;
	audio_src_step = track_channel * track_format / 8;
	audio_dst_step = play_track_channel * track_format / 8;
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "Audio track demo begin\n");

	//user should set sdk/component/soc/**/usrcfg/include/ameba_audio_hw_usrcfg.h's AUDIO_HW_AMPLIFIER_PIN to make sure amp is enabled.
	AudioService_Init();

	RTK_LOGS(TAG, RTK_LOG_INFO, "Audio ch:%d,rate:%d,bits=%d\n", track_channel, track_rate, track_format);

	switch (track_format) {
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
		usbd_uac_stop_play();
		return;
	}

	track_buf_size = AudioTrack_GetMinBufferBytes(audio_track, AUDIO_CATEGORY_MEDIA, track_rate, format, play_track_channel) * 4;
	if (track_buf_size == 0) {
		track_buf_size = track_rate * track_format / 8 * play_track_channel / 1000 * 100;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Track buf resize to %d\n", track_buf_size);
	}

	track_config.category_type = AUDIO_CATEGORY_MEDIA;
	track_config.sample_rate = track_rate;
	track_config.format = format;
	track_config.channel_count = play_track_channel;
	track_config.buffer_bytes = track_buf_size;
	if (AudioTrack_Init(audio_track, &track_config, AUDIO_OUTPUT_FLAG_NONE) != AUDIO_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Audio track init fail\n");
		usbd_uac_stop_play();
		AudioTrack_Destroy(audio_track);
		return;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Track buf size:%d\n", track_buf_size);

	/*for mixer version, this mean sw volume, for passthrough version, sw volume is not supported*/
	AudioTrack_SetVolume(audio_track, 1.0, 1.0);
	AudioTrack_SetStartThresholdBytes(audio_track, track_buf_size);

	if (AudioTrack_Start(audio_track) != AUDIO_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Audio track start fail\n");
		usbd_uac_stop_play();
		AudioTrack_Destroy(audio_track);
		return;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC stop %d\n", uac_player_stop);

	while ((uac_task_exiting != 1) && (uac_player_stop != 1) && (uac_app.quit == 0U)) {
		read_dat_len = usbd_uac_read(recv_buf, USB_AUDIO_BUF_SIZE * 2, 500, NULL);
		if (read_dat_len > 0) {
#if USBD_UAC_DEMUX_CH_DEBUG
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

			AudioTrack_Write(audio_track, (u8 *)play_buf, play_data_size, true);
			//RTK_LOGS(TAG, RTK_LOG_INFO, "Audio track start %d-%d\n",read_dat_len,play_data_size);
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
#else
	total_len = 0;
	read_cnt = 0;
	while ((uac_task_exiting != 1) && (uac_player_stop != 1) && (uac_app.quit == 0U)) {
		read_dat_len = usbd_uac_read(recv_buf, USB_AUDIO_BUF_SIZE * 2, 500, NULL);
		read_cnt ++;
		if (read_dat_len > 0) {
			total_len += read_dat_len;
			if (read_cnt % 200 == 0) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Audio track get %d %d\n", read_dat_len, total_len);
			}
		} else {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Audio Read Timeout\n");
			rtos_time_delay_ms(1);
		}
	}

	usbd_uac_stop_play();
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "Audio track demo stop\n\n\n");
}

static void example_usbd_uac_audio_track_thread(void *param)
{
	UNUSED(param);

	//should wait usb init success
	do {
		if (rtos_sema_take(uac_ready_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			break;
		}
		if (uac_app.quit != 0U) {
			break;
		}
		if (uac_task_exiting != 0) {
			break;
		}
		uac_player_stop = 0;
		/* Hold the lock for the whole playback session: it keeps a pending teardown
		   out of the class APIs this thread is inside. The hotplug thread sets
		   uac_task_exiting before it asks for the lock, so the loop below always
		   leaves and the wait is bounded. */
		rtos_mutex_take(uac_play_lock, RTOS_MAX_TIMEOUT);
		if ((uac_stack_ready != 0U) && (uac_app.quit == 0U)) {
			example_audio_track_play();
		}
		rtos_mutex_give(uac_play_lock);
	} while (1);

	/* Never deinits the stack: the stop owner does, once every worker is gone */
	usb_app_ctx_leave(&uac_app);
	rtos_task_delete(NULL);
}

#if USBD_UAC_ENABLE_RECORD
/* Replays a synthetic 1 kHz tone as the microphone (USB IN) source, once triggered
 * by the "USBDUAC record" console command. */
static void example_usbd_uac_record_thread(void *param)
{
	u32 offset;
	u32 tx_fail_cnt;

	UNUSED(param);

	while (1) {
		if (rtos_sema_take(uac_record_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			break;
		}
		if (uac_app.quit != 0U) {
			break;
		}

		/* Hold the lock for the whole record session: it keeps a pending teardown out
		   of the class APIs this thread is inside. The hotplug thread sets
		   uac_task_exiting before it asks for the lock, so the loop below always
		   leaves and the wait is bounded. */
		rtos_mutex_take(uac_record_lock, RTOS_MAX_TIMEOUT);
		if (uac_stack_ready == 0U) {
			rtos_mutex_give(uac_record_lock);
			continue;
		}

		/* Fails before the host has configured the device: the record ring would stay unsized */
		if (usbd_uac_start_record() != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC start record fail\n");
			rtos_mutex_give(uac_record_lock);
			continue;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "UAC record start\n");
		offset = 0U;
		tx_fail_cnt = 0U;
		while ((uac_task_exiting == 0) && (uac_app.quit == 0U)) {
			/* One ms of audio per call, the clip loops. A live stream tolerates a lost chunk, so
			   keep the 1 ms pace and only report the first failure of a run, to stay off the
			   console at 1 kHz. */
			if (usbd_uac_transmit_data(&usbd_uac_record_audio_data[offset], USBD_UAC_RECORD_CHUNK_LEN) != RTK_SUCCESS) {
				if (tx_fail_cnt == 0U) {
					RTK_LOGS(TAG, RTK_LOG_WARN, "UAC record tx fail\n");
				}
				tx_fail_cnt++;
			} else {
				tx_fail_cnt = 0U;
			}
			offset += USBD_UAC_RECORD_CHUNK_LEN;
			if (offset >= usbd_uac_record_data_len) {
				offset = 0U;
			}
			rtos_time_delay_ms(USBD_UAC_RECORD_CHUNK_DELAY_MS);
		}
		usbd_uac_stop_record();
		rtos_mutex_give(uac_record_lock);
	}

	/* Never deinits the stack: the stop owner does, once every worker is gone */
	usb_app_ctx_leave(&uac_app);
	rtos_task_delete(NULL);
}

static int uac_cmd_record(u16 argc, char *argv[])
{
	const char *cmd = (argc != 0U) ? argv[0] : NULL;

	if ((cmd == NULL) || (_stricmp(cmd, "record") != 0) || (argc != 1U)) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Usage: AT+USBDUAC=<cmd>\n");
		RTK_LOGS(TAG, RTK_LOG_INFO, "  record   Loop the built-in \"Realtek\" clip on the mic (USB IN) stream once the host\n");
		RTK_LOGS(TAG, RTK_LOG_INFO, "           configured it; each call queues one more session\n");
		RTK_LOGS(TAG, RTK_LOG_INFO, "  help     Print this help\n");
		return ((cmd != NULL) && ((_stricmp(cmd, "help") == 0) || (_stricmp(cmd, "?") == 0))) ? RTK_SUCCESS : -RTK_ERR_BADARG;
	}

	/* NULL only if the example never created it; once created it is never deleted, refer to
	   uac_free_resource(), so the handle cannot be freed under this shell thread. */
	if ((uac_record_start_sema == NULL) || (uac_stack_ready == 0U) || (uac_app.quit != 0U)) {
		/* Not running: a session queued now would only fire on the next start */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Not running\n");
		return -RTK_ERR_IO;
	}

	if (rtos_sema_give(uac_record_start_sema) != RTK_SUCCESS) {
		return -RTK_ERR_IO;
	}
	return RTK_SUCCESS;
}

/* AT entry: argv[0] is reserved by the AT parser and argv[1] holds the first parameter */
static void uac_cmd_record_at(u16 argc, char **argv)
{
	int ret = uac_cmd_record((argc != 0U) ? (u16)(argc - 1U) : 0U, &argv[1]);

	if (ret == RTK_SUCCESS) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, (int)ret);
	}
}

#endif /* USBD_UAC_ENABLE_RECORD */

/**
  * @brief  Bring the stack and the worker threads up
  * @retval RTK_SUCCESS, or the first error, with everything already created rolled back
  */
static int uac_start(void)
{
	int ret;
	u32 workers = 0U;
	rtos_task_t task;

	ret = usb_app_ctx_begin(&uac_app);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	/* Created before anything else and never deleted, see their declaration. */
	if (uac_play_lock == NULL) {
		ret = rtos_mutex_create(&uac_play_lock);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create play lock fail\n");
			goto exit;
		}
	}
#if USBD_UAC_ENABLE_RECORD
	if (uac_record_lock == NULL) {
		ret = rtos_mutex_create(&uac_record_lock);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create record lock fail\n");
			goto exit;
		}
	}
#endif

	ret = rtos_sema_create(&uac_ready_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create ready sema fail\n");
		goto exit;
	}

#if USBD_UAC_ENABLE_RECORD
	/* Created once and never deleted, refer to uac_free_resource() */
	if (uac_record_start_sema == NULL) {
		ret = rtos_sema_create(&uac_record_start_sema, 0U, RTOS_SEMA_MAX_COUNT);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create record sema fail\n");
			goto exit;
		}
	} else {
		/* Drop the sessions left queued by the previous run and its stop */
		while (rtos_sema_take(uac_record_start_sema, 0U) == RTK_SUCCESS) {
			/* drain */
		}
	}
#endif

#if USBD_UAC_HOTPLUG
	ret = rtos_sema_create(&uac_attach_status_changed_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create detach sema fail\n");
		goto exit;
	}
	uac_stack_down = 0U;
#endif

	/* A previous run ended in uac_wake_workers(), which leaves it raised */
	uac_task_exiting = 0;

	ret = usbd_init(&uac_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init fail\n");
		goto exit;
	}

	ret = usbd_uac_init(&uac_cb, &uac_class_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC init fail\n");
		goto clear_usb_driver_exit;
	}
	uac_stack_ready = 1;

	/* Workers first and the hotplug thread last: the hotplug thread is the only one that
	   tears the stack down on its own, so once it runs nothing is left to unwind here */
	ret = rtos_task_create(&task, "usbd_uac_audio_track_thread",
						   example_usbd_uac_audio_track_thread, NULL,
						   USBD_UAC_PLAYER_THREAD_STACK_SIZE, USBD_UAC_PLAYER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create audio track fail\n");
		goto clear_workers_exit;
	}
	workers++;

#if USBD_UAC_ENABLE_RECORD
	ret = rtos_task_create(&task, "usbd_uac_record_thread",
						   example_usbd_uac_record_thread, NULL,
						   USBD_UAC_RECORD_THREAD_STACK_SIZE, USBD_UAC_RECORD_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create record thread fail\n");
		goto clear_workers_exit;
	}
	workers++;
#endif

#if USBD_UAC_HOTPLUG
	ret = rtos_task_create(&task, "usbd_uac_hotplug_thread",
						   example_usbd_uac_hotplug_thread, NULL,
						   USBD_UAC_HOTPLUG_THREAD_STACK_SIZE, USBD_UAC_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug task fail\n");
		goto clear_workers_exit;
	}
#endif // USBD_UAC_HOTPLUG

	return RTK_SUCCESS;

	/* Failure unwind, strictly one-way: the workers leave first (they block on the
	   semaphores and call into the class), then the class, the core and the semaphores. */
clear_workers_exit:
	uac_app.quit = 1U;
	uac_wake_workers();
	if (usb_app_ctx_join(&uac_app, workers) != RTK_SUCCESS) {
		/* A worker may still run on the stack: keep it, the app stays not idle */
		uac_app.state = USB_APP_STATE_STOPPING;
		return -RTK_ERR_TIMEOUT;
	}
	usbd_uac_deinit();

clear_usb_driver_exit:
	usbd_deinit();

exit:
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBD UAC demo aborted\n");
	uac_free_resource();
	usb_app_ctx_end(&uac_app);

	return ret;
}

static void example_usbd_uac_thread(void *param)
{
	UNUSED(param);

#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* A start that fails rolls back through usbd_deinit(), which waits for no ISR on another
	   core: run it on CPU0, where the USB IRQ is delivered, as the hotplug thread does.
	   Done by the thread itself, first: a created thread may already run on another core, and its
	   creator pinning it afterwards could come too late or reach a task that has deleted itself. */
	rtos_task_set_affinity(rtos_task_handle_get(), 0);
#endif

	if (uac_start() != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Start fail\n");
	}
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Start the UAC device, used by the USB app manager
  * @note   Task context only
  * @retval RTK_SUCCESS, -RTK_ERR_BUSY if already running, or the init error
  */
int example_usbd_uac_start(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD UAC demo start\n");

	return uac_start();
}

/**
  * @brief  Stop the UAC device and release everything it owns
  * @note   Task context only. Safe to call when not running.
  * @retval RTK_SUCCESS once idle, -RTK_ERR_TIMEOUT if a worker did not leave in time
  */
int example_usbd_uac_stop(void)
{
	u32 workers = USBD_UAC_WORKER_CNT;

	if (usb_app_ctx_claim_stop(&uac_app) == 0U) {
		/* Idle, or the hotplug thread found the stack dead and finishes the teardown itself */
		return usb_app_ctx_wait_idle(&uac_app);
	}

	/* The playback loop needs the longest: usbd_uac_read() has a 500ms timeout and the
	   AudioTrack teardown follows */
	uac_wake_workers();
#if USBD_UAC_HOTPLUG
	rtos_sema_give(uac_attach_status_changed_sema);
	workers++;
#endif
	if (usb_app_ctx_join(&uac_app, workers) != RTK_SUCCESS) {
		/* A worker is still on the stack: keep everything, the app stays STOPPING */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stop timeout\n");
		return -RTK_ERR_TIMEOUT;
	}

	/* Every worker is gone: this caller is the only one left on the stack. A hotplug
	   reinit racing the stop may have reopened the gate, close it again. */
	uac_stack_ready = 0;
#if USBD_UAC_HOTPLUG
	/* The hotplug thread lost the claim after a failed reinit: the stack is already down */
	if (uac_stack_down == 0U)
#endif
	{
		usbd_uac_deinit();
		usbd_deinit();
	}
	uac_free_resource();
	usb_app_ctx_end(&uac_app);
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD UAC demo stop\n");

	return RTK_SUCCESS;
}

/**
  * @brief  Example entry: start the UAC device from its own thread
  * @retval None
  */
void example_usbd_uac(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD UAC demo start\n");

	ret = rtos_task_create(&task, "usbd_uac_thread", example_usbd_uac_thread, NULL,
						   USBD_UAC_INIT_THREAD_STACK_SIZE, USBD_UAC_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD UAC thread fail\n");
	}
}
