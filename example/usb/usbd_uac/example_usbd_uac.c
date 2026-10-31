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
#define USBD_UAC_ISOC_IN_EP                      0x84U
#define USBD_UAC_ISOC_OUT_EP                     0x02U
#else
#define USBD_UAC_ISOC_IN_EP                      0x81U
#define USBD_UAC_ISOC_OUT_EP                     0x02U
#endif

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define USBD_UAC_HOTPLUG                          1

// This configuration is used to choose one channel to play
// for the audio does not support some channel, Such as 4 chs
// while enable this configuration,choose the first ch to play
#define USBD_UAC_DEMUX_CH_DEBUG                   1

// USB speed. Only UAC 1.0 needs to ask: its spec supports Full Speed only, so an
// HS-capable PHY has to be driven in Full-Speed mode. UAC 2.0 leaves the speed to
// the core, which clamps to Full Speed by itself on an FS-only SoC.
#ifdef CONFIG_USBD_UAC1
#define USBD_UAC_USB_SPEED                        USB_SPEED_HIGH_IN_FULL
#else
#define USBD_UAC_USB_SPEED                        USB_SPEED_HIGH
#endif

// Thread priorities
#define USBD_UAC_INIT_THREAD_PRIORITY             6
#define USBD_UAC_HOTPLUG_THREAD_PRIORITY          8
#define USBD_UAC_PLAYER_THREAD_PRIORITY           6

// Thread stack sizes
#define USBD_UAC_INIT_THREAD_STACK_SIZE           1024U
#define USBD_UAC_HOTPLUG_THREAD_STACK_SIZE        1024U
#define USBD_UAC_PLAYER_THREAD_STACK_SIZE         2300U

// Microphone (USB IN / record) demo. Recording is only wired up in the UAC 2.0
// class driver, so follow the same rule as the composite UAC example: enable it
// for UAC 2.0 (HS / HS-IN-FULL) and disable it for UAC 1.0 and FS-only builds.
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define USBD_UAC_ENABLE_RECORD                    0
#elif defined(CONFIG_USBD_UAC1)
#define USBD_UAC_ENABLE_RECORD                    0
#else
#define USBD_UAC_ENABLE_RECORD                    1
#endif

#if USBD_UAC_ENABLE_RECORD
#define USBD_UAC_RECORD_THREAD_PRIORITY           4
#define USBD_UAC_RECORD_THREAD_STACK_SIZE         (1024U * 4)
/* One USB IN chunk = one period of a 1 kHz sine at 16 kHz stereo 16-bit = 16 frames = 64 bytes */
#define USBD_UAC_RECORD_CHUNK_LEN                 64U
#define USBD_UAC_RECORD_CHUNK_DELAY_MS            1U
/* usbd_uac_transmit_data() accepts any length and cuts it into per-(micro)frame packets itself,
 * so the slices below are only a feeding granularity: one call per ms would do as well. */
#define USBD_UAC_RECORD_SLICES_PER_MS             8U
#define USBD_UAC_RECORD_SLICE_LEN                 (USBD_UAC_RECORD_CHUNK_LEN / USBD_UAC_RECORD_SLICES_PER_MS)
#endif

#define AUDIO_BYTE_WIDTH_SIZE                   USBD_UAC_DEFAULT_BYTE_WIDTH
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
#define USB_AUDIO_BUF_SIZE                  ((USB_AUDIO_MS_BUF_SIZE) * 3U)

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

#if USBD_UAC_ENABLE_RECORD
static int uac_cmd_record(u16 argc, u8 *argv[]);
static void uac_cmd_record_at(u16 argc, char **argv);
#endif

static int uac_cb_init(void);
static int uac_cb_deinit(void);
static int uac_cb_setup(usb_setup_req_t *req, u8 *buf);
static int uac_cb_set_config(void);

static void uac_cb_status_changed(u8 old_status, u8 status);
static void uac_cb_mute_changed(u8 mute);
static void uac_cb_volume_changed(u8 volume);
static void uac_cb_format_changed(u32 sampling_freq, u8 ch_cnt, u8 byte_width);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UAC";

#if USBD_UAC_HOTPLUG
static rtos_task_t check_status_task;
static rtos_sema_t uac_attach_status_changed_sema;
/* Written by the ISR, read by the hotplug thread, possibly on another core */
static __IO u8 uac_attach_status;
#endif
/* Raised by uac_stop_workers() when the stack is gone for good: the workers leave their
   loops and the caller frees the shared objects. */
static __IO u8 uac_stack_fatal = 0;
static rtos_task_t uac_player_task;
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
		.speed = USBD_UAC_USB_SPEED,
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

/* Handed to the class, which keeps the pointer: never written afterwards, refer to uac_play_cfg */
static const usbd_uac_config_t uac_class_cfg = {
#if USBD_UAC_ENABLE_RECORD
	.in = {
		.enable = 1,
		.sampling_freq = USBD_UAC_IN_DEFAULT_SAMPLING_FREQ,
		.byte_width = USBD_UAC_IN_DEFAULT_BYTE_WIDTH,
		.ch_cnt = USBD_UAC_IN_DEFAULT_CH_CNT
	},
#else
	.in = {.enable = 0,}, /* playback only (UAC 1.0 / FS-only) */
#endif
	.out = {.enable = 1, .sampling_freq = AUDIO_SAMPLING_FREQ, .byte_width = AUDIO_BYTE_WIDTH_SIZE, .ch_cnt = AUDIO_CHANNEL_NUM},
	.isoc_in_addr  = USBD_UAC_ISOC_IN_EP,
	.isoc_out_addr = USBD_UAC_ISOC_OUT_EP,
};

/* Current playback format: rewritten by format_changed() in ISR context, possibly on another
   core, and snapshot by the player under the same lock */
static usbd_audio_cfg_t uac_play_cfg = {
	.sampling_freq = AUDIO_SAMPLING_FREQ,
	.byte_width = AUDIO_BYTE_WIDTH_SIZE,
	.ch_cnt = AUDIO_CHANNEL_NUM,
	.enable = 1,
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
/* Mic record format: 16-bit / 16 kHz / 2ch (stereo), matches the synthetic tone below */
static const usbd_audio_cfg_t uac_record_cfg = {
	.sampling_freq = USBD_UAC_IN_DEFAULT_SAMPLING_FREQ,
	.byte_width = USBD_UAC_IN_DEFAULT_BYTE_WIDTH,
	.ch_cnt = USBD_UAC_IN_DEFAULT_CH_CNT,
	.enable = 1,
};
static rtos_sema_t uac_record_start_sema;
static rtos_task_t uac_record_task;
/* 16-point sine LUT (one period), amplitude ~10000; drives a 1 kHz tone at 16 kHz */
static const s16 uac_record_sine16[16] = {
	0, 3827, 7071, 9239, 10000, 9239, 7071, 3827,
	0, -3827, -7071, -9239, -10000, -9239, -7071, -3827
};
static u8 uac_record_chunk[USBD_UAC_RECORD_CHUNK_LEN] USB_DMA_ALIGNED;
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
  * @note   Only called by the last running thread, after uac_stop_workers() and after the
  *         USB stack is fully deinited, so that neither a worker nor an ISR callback can
  *         touch these objects any more
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
  * @brief  Name the first worker thread that has not left yet
  * @retval Thread name, NULL once every worker is gone
  */
static const char *uac_get_running_worker(void)
{
	if (uac_player_task != NULL) {
		return "player";
	}
#if USBD_UAC_ENABLE_RECORD
	if (uac_record_task != NULL) {
		return "record";
	}
#endif
	return NULL;
}

/**
  * @brief  Ask the worker threads to leave for good and wait until they have
  * @note   Each worker clears its handle as its last action and deletes itself. A worker is
  *         never deleted from outside: it may be inside the class APIs or hold a lock, so the
  *         wait is not bounded and a stuck worker is reported every 2s instead.
  * @retval None
  */
static void uac_stop_workers(void)
{
	const char *name;
	u32 wait_cnt = 0U;

	uac_stack_ready = 0;
	uac_stack_fatal = 1;
	uac_task_exiting = 1;
	usbd_uac_stop_play();
#if USBD_UAC_ENABLE_RECORD
	usbd_uac_stop_record();
	rtos_sema_give(uac_record_start_sema);
#endif
	rtos_sema_give(uac_ready_sema);

	/* The playback loop needs the longest: usbd_uac_read() has a 500ms timeout and the
	   AudioTrack teardown follows */
	name = uac_get_running_worker();
	while (name != NULL) {
		rtos_time_delay_ms(20);
		wait_cnt++;
		if (wait_cnt >= 100U) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Wait %s exit\n", name);
			wait_cnt = 0U;
		}
		name = uac_get_running_worker();
	}
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

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(uac_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
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
				uac_task_exiting = 0;
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				ret = usbd_init(&uac_cfg);
				if (ret != 0) {
					uac_state_unlock();
					break;
				}
				ret = usbd_uac_init(&uac_cb, &uac_class_cfg);
				if (ret != 0) {
					usbd_deinit();
					uac_state_unlock();
					break;
				}
				uac_stack_ready = 1;
				uac_state_unlock();
			} else if (uac_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Hotplug thread fail\n");

	/* The stack is fully deinited here, no more ISR callback: stop the workers
	   blocked on the semaphores, then free them as the last thread standing. */
	uac_stop_workers();
	uac_free_resource();
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
  * @param  ch_cnt: New channel count (0 means unchanged)
  * @param  byte_width: New sample byte width (0 means unchanged)
  * @retval None
  */
static void uac_cb_format_changed(u32 sampling_freq, u8 ch_cnt, u8 byte_width)
{
	usbd_audio_cfg_t play_cfg;

	/* The player thread snapshots the format under the same lock, refer to example_audio_track_play() */
	rtos_critical_enter(RTOS_CRITICAL_USB);
	if (sampling_freq != 0U) {
		uac_play_cfg.sampling_freq = sampling_freq;
	}
	if (ch_cnt != 0U) {
		uac_play_cfg.ch_cnt = ch_cnt;
	}
	if (byte_width != 0U) {
		uac_play_cfg.byte_width = byte_width;
	}
	play_cfg = uac_play_cfg;
	rtos_critical_exit(RTOS_CRITICAL_USB);

	if ((play_cfg.sampling_freq != 0U) && (play_cfg.byte_width != 0U) && (play_cfg.ch_cnt != 0U)) {
		uac_player_stop = 1;
		usbd_uac_stop_play();
		rtos_sema_give(uac_ready_sema);
		//RTK_LOGS(TAG, RTK_LOG_INFO, "USBD set sampling_freq %d set ch_cnt %d\n", sampling_freq, ch_cnt);
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
#else
	u32 total_len = 0;
	u32 read_cnt = 0;
#endif
	u32 read_dat_len = 0;
	usbd_audio_cfg_t play_cfg;

	/* One consistent snapshot: format_changed() rewrites the fields from the ISR */
	rtos_critical_enter(RTOS_CRITICAL_USB);
	play_cfg = uac_play_cfg;
	rtos_critical_exit(RTOS_CRITICAL_USB);

	/* Fails until the host has enumerated the device or for a format the class does not
	   declare: wait for the next format change or attach instead of streaming unsized */
	if (usbd_uac_config(&play_cfg, 0, 0) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC config fail\n");
		return;
	}
	/* isoc_mps is cleared on deinit, so start_play() keeps failing once the stack is
	   torn down: leave on any stop request instead of retrying forever. Nothing to
	   unwind here, no AudioTrack has been created yet. */
	while (usbd_uac_start_play() != RTK_SUCCESS) {
		if ((uac_task_exiting != 0) || (uac_player_stop != 0) || (uac_stack_fatal != 0)) {
			return;
		}
		rtos_time_delay_ms(5);
	}

#ifdef CONFIG_SUPPORT_AUDIO_FOR_USB

	/* audio trace config params */
	track_rate = play_cfg.sampling_freq;
	track_channel = play_cfg.ch_cnt;
	track_format = play_cfg.byte_width * 8;

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

	while ((uac_task_exiting != 1) && (uac_player_stop != 1)) {
		read_dat_len = usbd_uac_read(recv_buf, USB_AUDIO_BUF_SIZE * 2, 500, NULL);
		if (read_dat_len > 0) {
#if USBD_UAC_DEMUX_CH_DEBUG
			play_data_size = 0;
			//get the 2 channel data from the 4 channel
			for (idx = 0, off = 0; idx < read_dat_len; idx += audio_src_step, off += audio_dst_step) {
				// ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3 ch0 ch1 ch2 ch3
				// 24  24  24  24  24  24  24  24  24  24  24  24
				usb_os_memcpy((void *)(play_buf + off), (const void *)(recv_buf + idx), audio_dst_step);
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
	while ((uac_task_exiting != 1) && (uac_player_stop != 1)) {
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
		if (uac_stack_fatal != 0) {
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
		if (uac_stack_ready != 0U) {
			example_audio_track_play();
		}
		rtos_mutex_give(uac_play_lock);
	} while (1);

	/* Clear the handle last: uac_stop_workers() polls it to know this worker is
	   gone before the semaphores are freed. */
	uac_player_task = NULL;
	rtos_task_delete(NULL);
}

#if USBD_UAC_ENABLE_RECORD
/* Replays a synthetic 1 kHz tone as the microphone (USB IN) source, once triggered
 * by the "USBDUAC record" console command. */
static void example_usbd_uac_record_thread(void *param)
{
	u32 i;
	u32 tx_fail_cnt;
	u32 frames = USBD_UAC_RECORD_CHUNK_LEN / (USBD_UAC_IN_DEFAULT_CH_CNT * USBD_UAC_IN_DEFAULT_BYTE_WIDTH);
	s16 *frame = (s16 *)(void *)uac_record_chunk;

	UNUSED(param);

	/* Pre-fill one chunk: interleaved stereo frames, L = R = sine sample */
	for (i = 0; i < frames; i++) {
		frame[USBD_UAC_IN_DEFAULT_CH_CNT * i]     = uac_record_sine16[i % 16U];
		frame[USBD_UAC_IN_DEFAULT_CH_CNT * i + 1] = uac_record_sine16[i % 16U];
	}

	while (1) {
		if (rtos_sema_take(uac_record_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			break;
		}
		if (uac_stack_fatal != 0) {
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

		if (usbd_uac_config(&uac_record_cfg, 1, 0) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC config fail\n");
			rtos_mutex_give(uac_record_lock);
			continue;
		}
		if (usbd_uac_start_record() != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC start record fail\n");
			rtos_mutex_give(uac_record_lock);
			continue;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "UAC record start\n");
		tx_fail_cnt = 0U;
		while (uac_task_exiting == 0) {
			for (i = 0; i < USBD_UAC_RECORD_SLICES_PER_MS; i++) {
				/* A live stream tolerates a lost slice, so keep the pace and only report the first
				   failure of a run, to stay off the console at 8 kHz */
				if (usbd_uac_transmit_data(&uac_record_chunk[i * USBD_UAC_RECORD_SLICE_LEN], USBD_UAC_RECORD_SLICE_LEN) != RTK_SUCCESS) {
					if (tx_fail_cnt == 0U) {
						RTK_LOGS(TAG, RTK_LOG_WARN, "UAC record tx fail\n");
					}
					tx_fail_cnt++;
				} else {
					tx_fail_cnt = 0U;
				}
			}
			rtos_time_delay_ms(USBD_UAC_RECORD_CHUNK_DELAY_MS);
		}
		usbd_uac_stop_record();
		rtos_mutex_give(uac_record_lock);
	}

	/* Clear the handle last: uac_stop_workers() polls it to know this worker is
	   gone before the semaphores are freed. */
	uac_record_task = NULL;
	rtos_task_delete(NULL);
}

static int uac_cmd_record(u16 argc, u8 *argv[])
{
	const char *cmd = (argc != 0U) ? (const char *)argv[0] : NULL;

	if ((cmd == NULL) || (_stricmp(cmd, "record") != 0) || (argc != 1U)) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Usage: AT+USBDUAC=<cmd>\n");
		RTK_LOGS(TAG, RTK_LOG_INFO, "  record   Replay the built-in clip on the mic (USB IN) stream once the host\n");
		RTK_LOGS(TAG, RTK_LOG_INFO, "           configured it; each call queues one more session. UAC 2.0, non FS-only build only\n");
		RTK_LOGS(TAG, RTK_LOG_INFO, "  help     Print this help\n");
		return ((cmd != NULL) && ((_stricmp(cmd, "help") == 0) || (_stricmp(cmd, "?") == 0))) ? RTK_SUCCESS : -RTK_ERR_BADARG;
	}

	/* NULL only if the example never created it; once created it is never deleted, refer to
	   uac_free_resource(), so the handle cannot be freed under this shell thread. */
	if (uac_record_start_sema == NULL) {
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
	int ret = uac_cmd_record((argc != 0U) ? (argc - 1U) : 0U, (u8 **)&argv[1]);

	if (ret == RTK_SUCCESS) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, (int)ret);
	}
}

#endif /* USBD_UAC_ENABLE_RECORD */

static void example_usbd_uac_thread(void *param)
{
	UNUSED(param);
	int ret = 0;

	/* Created before anything else and never deleted, see their declaration. */
	if (uac_play_lock == NULL) {
		ret = rtos_mutex_create(&uac_play_lock);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create play lock fail\n");
			goto example_exit;
		}
	}
#if USBD_UAC_ENABLE_RECORD
	if (uac_record_lock == NULL) {
		ret = rtos_mutex_create(&uac_record_lock);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create record lock fail\n");
			goto example_exit;
		}
	}
#endif

	ret = rtos_sema_create(&uac_ready_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create ready sema fail\n");
		goto example_exit;
	}

#if USBD_UAC_ENABLE_RECORD
	/* Created once and never deleted, refer to uac_free_resource() */
	if (uac_record_start_sema == NULL) {
		ret = rtos_sema_create(&uac_record_start_sema, 0U, RTOS_SEMA_MAX_COUNT);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create record sema fail\n");
			goto exit;
		}
	}
#endif

#if USBD_UAC_HOTPLUG
	ret = rtos_sema_create(&uac_attach_status_changed_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create detach sema fail\n");
		goto exit;
	}
#endif

	/* A previous run ended in uac_stop_workers(), which leaves both raised */
	uac_stack_fatal = 0;
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
	ret = rtos_task_create(&uac_player_task, "usbd_uac_audio_track_thread",
						   example_usbd_uac_audio_track_thread, NULL,
						   USBD_UAC_PLAYER_THREAD_STACK_SIZE, USBD_UAC_PLAYER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create audio track fail\n");
		uac_player_task = NULL;
		goto clear_workers_exit;
	}

#if USBD_UAC_ENABLE_RECORD
	ret = rtos_task_create(&uac_record_task, "usbd_uac_record_thread",
						   example_usbd_uac_record_thread, NULL,
						   USBD_UAC_RECORD_THREAD_STACK_SIZE, USBD_UAC_RECORD_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create record thread fail\n");
		uac_record_task = NULL;
		goto clear_workers_exit;
	}
#endif

#if USBD_UAC_HOTPLUG
	ret = rtos_task_create(&check_status_task, "usbd_uac_hotplug_thread",
						   example_usbd_uac_hotplug_thread, NULL,
						   USBD_UAC_HOTPLUG_THREAD_STACK_SIZE, USBD_UAC_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug task fail\n");
		goto clear_workers_exit;
	}
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* C-2: the USB OTG ISR is delivered on CPU0 (GIC ITARGETSR pins every SPI to core 0).
	   Pinning the hotplug/deinit thread to CPU0 puts it on the same core as the ISR,
	   so deinit's local interrupt disable is meaningful again under SMP. The player /
	   record data threads stay unaffined: a pending deinit is excluded from them by
	   uac_play_lock / uac_record_lock, not by core. */
	rtos_task_set_affinity(check_status_task, 0);
#endif
#endif // USBD_UAC_HOTPLUG

	rtos_time_delay_ms(100);

	goto example_exit;

	/* Failure unwind, strictly one-way: the workers leave first (they block on the
	   semaphores and call into the class), then the class, the core and the semaphores. */
clear_workers_exit:
	uac_stop_workers();
	usbd_uac_deinit();

clear_usb_driver_exit:
	usbd_deinit();

exit:
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBD UAC demo aborted\n");
	uac_free_resource();

example_exit:
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

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
