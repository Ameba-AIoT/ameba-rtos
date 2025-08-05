/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "os_wrapper.h"
#include "usbh_uac1.h"
#include "usbh.h"

#include "audio/audio_service.h"
#include "media/rtplayer.h"


/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "UAC";
#define CONFIG_USBH_UAC_HOT_PLUG_TEST 0     /* Hot plug / memory leak test */

#define USBH_UAC_ISOC_BUF_SIZE        200    /* Buffer size for ISOC uac test, which should match with device ISOC uac buffer size */
#define USBH_UAC_ISOC_RX_PASS_CNT     3     /* Loopback ISOC test pass threshold */
#define USBH_UAC_TEST_CNT             3

#define USBH_UAC_CHANNELS             2
#define USBH_UAC_BITWIDTH             16
#define USBH_UAC_SAMPLING_FREQ        48000
/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int uac_cb_init(void);
static int uac_cb_deinit(void);
static int uac_cb_attach(void);
static int uac_cb_detach(void);
static int uac_cb_setup(void);
static int uac_cb_isoc_transmitted(usbh_urb_state_t state);
static int uac_cb_process(usb_host_t *host, u8 id);
void usbh_uac_stop_play(void);
/* Private variables ---------------------------------------------------------*/

#define  USB_OUT_DATA (0x88)
#define  USB_IN_DATA  (0x22)


static rtos_sema_t uac_detach_sema;
static rtos_sema_t uac_attach_sema;
static rtos_sema_t uac_isoc_start_sema;
static rtos_sema_t uac_isoc_send_done_sema;

static __IO int uac_is_ready = 0;
static volatile u8 audio_task_stop = 0;
static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_FULL,
	.dma_enable = 1U,
	.isr_priority = INT_PRI_MIDDLE,
	.isr_task_priority  = 4U,
	.main_task_priority = 3U,
	.sof_tick_en = 1U,
	.alt_max = 6,
#if defined (CONFIG_AMEBAGREEN2)
	/*FIFO total depth is 1024, reserve 12 for DMA addr*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBASMARTPLUS)
	/*FIFO total depth is 1280 DWORD, reserve 14 DWORD for DMA addr*/
	.rx_fifo_depth = 754,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAL2)
	/*FIFO total depth is 1024 DWORD, reserve 11 DWORD for DMA addr*/
	.rx_fifo_depth = 501,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

static usbh_uac_cb_t uac_usr_cb = {
	.init = uac_cb_init,
	.deinit = uac_cb_deinit,
	.attach = uac_cb_attach,
	.detach = uac_cb_detach,
	.setup = uac_cb_setup,
	.isoc_transmitted = uac_cb_isoc_transmitted,
};

static usbh_user_cb_t usbh_usr_cb = {
	.process = uac_cb_process
};

static int busy_count = 0;
static int busy_count2 = 0;
static int trans_error = 0;
static int time_out = 0;
/* Private functions ---------------------------------------------------------*/

static int uac_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
	return HAL_OK;
}

static int uac_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
	return HAL_OK;
}

static int uac_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	rtos_sema_give(uac_attach_sema);
	return HAL_OK;
}

static int uac_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
#if CONFIG_USBH_UAC_HOT_PLUG_TEST
	rtos_sema_give(uac_detach_sema);
#endif
	uac_is_ready = 0;
	return HAL_OK;
}

static int uac_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	uac_is_ready = 1;
	rtos_sema_give(uac_isoc_start_sema);
	return HAL_OK;
}

static int uac_cb_isoc_transmitted(usbh_urb_state_t state)
{
	if (state == USBH_URB_DONE) {
		/*TX ok*/
	} else if (state == USBH_URB_BUSY) {
		//printf("aaa busy now\n");
		busy_count++;
	} else {
		//RTK_LOGS(TAG, RTK_LOG_ERROR, "TX fail: %d\n", state);
	}
	rtos_sema_give(uac_isoc_send_done_sema);
	return HAL_OK;
}

static int uac_cb_process(usb_host_t *host, u8 id)
{
	UNUSED(host);

	switch (id) {
	case USBH_MSG_DISCONNECTED:
		uac_is_ready = 0;
		break;

	case USBH_MSG_CONNECTED:
		break;

	default:
		break;
	}

	return HAL_OK;
}

#define MAX_URL_SIZE 1024
static char g_url[MAX_URL_SIZE];

enum PlayingStatus {
	IDLE,
	PLAYING,
	PAUSED,
	PLAYING_COMPLETED,
	REWIND_COMPLETE,
	STOPPED,
	RESET,
};
int g_playing_status = IDLE;

struct RTPlayer *g_player;

static void OnStateChanged(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int state)
{
	printf("OnStateChanged(%p %p), (%d)\n", listener, player, state);

	switch (state) {
	case RTPLAYER_PREPARED: { //entered for async prepare
		break;
	}

	case RTPLAYER_PLAYBACK_COMPLETE: { //eos received, then stop
		g_playing_status = PLAYING_COMPLETED;
		break;
	}

	case RTPLAYER_STOPPED: { //stop received, then reset
		printf("start reset\n");
		g_playing_status = STOPPED;
		break;
	}

	case RTPLAYER_PAUSED: { //pause received when do pause or start rewinding
		printf("paused\n");
		g_playing_status = PAUSED;
		break;
	}

	case RTPLAYER_REWIND_COMPLETE: { //rewind done received, then start
		printf("rewind complete\n");
		g_playing_status = REWIND_COMPLETE;
		break;
	}
	}
}

static void OnInfo(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int info, int extra)
{
	//printf("OnInfo (%p %p), (%d, %d)\n", listener, player, info, extra);
	(void) listener;
	(void) player;
	(void) info;
	(void) extra;

	switch (info) {
	case RTPLAYER_INFO_BUFFERING_START: {
		//printf("RTPLAYER_INFO_BUFFERING_START\n");
		break;
	}

	case RTPLAYER_INFO_BUFFERING_END: {
		//printf("RTPLAYER_INFO_BUFFERING_END\n");
		break;
	}

	case RTPLAYER_INFO_BUFFERING_INFO_UPDATE: {
		//printf("RTPLAYER_INFO_BUFFERING_INFO_UPDATE %d\n", extra);
		break;
	}

	case RTPLAYER_INFO_NOT_REWINDABLE: {
		//printf("RTPLAYER_INFO_NOT_REWINDABLE\n");
		break;
	}
	}
}

static void OnError(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int error, int extra)
{
	printf("OnError (%p %p), (%d, %d)\n", player, listener, error, extra);
}

void StartPlay(struct RTPlayer *player, const char *url)
{
	if (player == NULL) {
		printf("start play fail, player is NULL!\n");
		return;
	}

	printf("start to play: %s\n", url);
	int32_t ret = 0;

	g_playing_status = PLAYING;

	printf("SetSource\n");
	ret = RTPlayer_SetSource(player, url);
	if (ret) {
		printf("SetDataSource fail:error=%d\n", (int)ret);
		return ;
	}

	printf("Prepare\n");
	ret = RTPlayer_Prepare(player);
	if (ret) {
		printf("prepare  fail:error=%d\n", (int)ret);
		return ;
	}

	printf("Start\n");
	ret = RTPlayer_Start(player);
	if (ret) {
		printf("start  fail:error=%d\n", (int)ret);
		return ;
	}

	int64_t duration = 0;
	RTPlayer_GetDuration(player, &duration);
	printf("duration is %lldms\n", duration);

	while (g_playing_status == PLAYING) {
		rtos_time_delay_ms(1000);
	}

	if (g_playing_status == PLAYING_COMPLETED) {
		printf("play complete, now stop.\n");
		usbh_uac_stop_play();
		RTPlayer_Stop(player);
	}

	while (g_playing_status == PLAYING_COMPLETED) {
		rtos_time_delay_ms(1000);
	}

	if (g_playing_status == STOPPED) {
		printf("play stopped, now reset.\n");
		RTPlayer_Reset(player);
	}

	printf("play %s done!!!!\n", url);
}

ssize_t RTPlayer_AudioSinkCallback(int16_t *data, size_t len)
{
	int ret = usbh_uac_isoc_transmit((uint8_t *)data, len);
	if (ret == HAL_OK) {
		if (rtos_sema_take(uac_isoc_send_done_sema, 1000U) == RTK_SUCCESS) {
			return len;
		} else {
			time_out++;
			printf("timeout now\n");
			return 0;
		}
	} else if (ret == HAL_BUSY) {
		busy_count2++;
		rtos_time_delay_ms(5);
		return 0;
	} else {
		trans_error++;
		rtos_time_delay_ms(5);
		return 0;
	}
}

static void uac_isoc_test(void *param)
{
	printf("uac_isoc_test start\n");
	(void) param;
	if (rtos_sema_take(uac_isoc_start_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
		// general default format: 2ch * 16bit * 48k
		printf("uac_isoc_start_sema give\n");
		if (usbh_uac_set_alt_setting(0, USBH_UAC_CHANNELS, USBH_UAC_BITWIDTH, USBH_UAC_SAMPLING_FREQ) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "No suitable interface/altsettinfg, return\n");
			rtos_task_delete(NULL);
			return;
		}
	}

	rtos_time_delay_ms(500);

	printf("rtplay callback start\n");
	RTPlayerCallback *callback = (RTPlayerCallback *)malloc(sizeof(RTPlayerCallback));
	if (!callback) {
		printf("Calloc RTPlayerCallback fail.\n");
		return;
	}

	callback->OnRTPlayerStateChanged = OnStateChanged;
	callback->OnRTPlayerInfo = OnInfo;
	callback->OnRTPlayerError = OnError;

	g_player = RTPlayer_CreateEx(AUDIO_SINK_TYPE_UAC);
	int32_t frame_size = usbh_uac_get_frame_size(0);
	printf("frame_size %ld\n", frame_size);

	// invoke bt audio params
	Parcel *request = Parcel_Create();
	Parcel_WriteInt32(request, 1111);
	Parcel_WriteInt32(request, USBH_UAC_SAMPLING_FREQ);
	Parcel_WriteInt32(request, USBH_UAC_CHANNELS);
	Parcel_WriteInt32(request, USBH_UAC_BITWIDTH);
	Parcel_WriteInt32(request, frame_size * 10);
	RTPlayer_Invoke(g_player, request, NULL);
	Parcel_Destroy(request);

	RTPlayer_SetCallback(g_player, callback);

	StartPlay(g_player, g_url);

	printf("trans_error %d busy_count %d busy_count2 %d time_out %d\n", trans_error, busy_count, busy_count2, time_out);

	free(callback);
	RTPlayer_Destory(g_player);
	g_player = NULL;

	rtos_time_delay_ms(1000);

	printf("exit\n");
	rtos_task_delete(NULL);
}


#if CONFIG_USBH_UAC_HOT_PLUG_TEST
static void uac_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(uac_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			rtos_time_delay_ms(100);
			usbh_uac_deinit();
			usbh_deinit();
			rtos_time_delay_ms(10);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
			if (ret != HAL_OK) {
				break;
			}

			ret = usbh_uac_init(&uac_usr_cb);
			if (ret < 0) {
				usbh_deinit();
				break;
			}

			if (rtos_sema_take(uac_attach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
				if (usbh_uac_setup_out(0) != HAL_OK) {
					break;
				}
			}
		}
	}
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\n");
	rtos_task_delete(NULL);
}
#endif

static void example_usbh_uac_thread(void *param)
{
	int status;
	rtos_task_t isoc_task;
#if CONFIG_USBH_UAC_HOT_PLUG_TEST
	rtos_task_t hotplug_task;
#endif

	UNUSED(param);

	rtos_sema_create(&uac_detach_sema, 0U, 1U);
	rtos_sema_create(&uac_attach_sema, 0U, 1U);
	rtos_sema_create(&uac_isoc_start_sema, 0U, 1U);
	rtos_sema_create(&uac_isoc_send_done_sema, 0U, 1U);

	status = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (status != HAL_OK) {
		goto free_sema_exit;
	}

	status = usbh_uac_init(&uac_usr_cb);  /*0 means use default transfer size, and it can not exceed 65536*/
	if (status != HAL_OK) {
		goto usb_deinit_exit;
	}

#if CONFIG_USBH_UAC_HOT_PLUG_TEST
	status = rtos_task_create(&hotplug_task, "uac_hotplug_thread", uac_hotplug_thread, NULL, 1024U * 2, 3U);
	if (status != RTK_SUCCESS) {
		goto usbh_uac_deinit_exit;
	}
#endif

	if (rtos_sema_take(uac_attach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
		status = usbh_uac_setup_out(0);
		if (status != HAL_OK) {
			goto delete_hotplug_task_exit;
		}

		status = rtos_task_create(&isoc_task, "uac_isoc_test", uac_isoc_test, NULL, 1024U * 2, 5U);
		if (status != RTK_SUCCESS) {
			goto delete_hotplug_task_exit;
		}
	}

	goto example_exit;

delete_hotplug_task_exit:
#if CONFIG_USBH_UAC_HOT_PLUG_TEST
	rtos_task_delete(hotplug_task);
#endif

#if CONFIG_USBH_UAC_HOT_PLUG_TEST
usbh_uac_deinit_exit:
#endif
	usbh_uac_deinit();

usb_deinit_exit:
	usbh_deinit();

free_sema_exit:
	rtos_sema_delete(uac_detach_sema);
	rtos_sema_delete(uac_attach_sema);
	rtos_sema_delete(uac_isoc_start_sema);
	rtos_sema_delete(uac_isoc_send_done_sema);

example_exit:
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH UAC demo stop\n");
	rtos_task_delete(NULL);
}


void example_usbh_uac_args_handle(char  *argv[])
{
	RTAudioService_Init();
	printf("AudioService_Init done\n");

	/* parse command line arguments */
	while (*argv) {
		if (strcmp(*argv, "-F") == 0) {
			argv++;
			if (*argv) {
				memset(g_url, 0, MAX_URL_SIZE);
				if (!strncasecmp("http://", *argv, 7) || !strncasecmp("https://", *argv, 8)) {
					snprintf(g_url, MAX_URL_SIZE, "%s", *argv);
				} else {
					snprintf(g_url, MAX_URL_SIZE, "%s", *argv);
				}
			}
		}
		if (*argv) {
			argv++;
		}
	}
	printf("Usage: url is %s", g_url);

	printf("player test start......\n");

	int status;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH UAC demo start\n");

	status = rtos_task_create(&task, "example_usbh_uac_thread", example_usbh_uac_thread, NULL, 1024U * 2, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}

	printf("player test done......\n");
	printf("\n\n");

	return;
}

u32 example_usbh_uac(u16 argc, u8 *argv[])
{
	(void) argc;
	example_usbh_uac_args_handle((char **)argv);
	return TRUE;
}


CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbh_uac_test_cmd_table[] = {
	{
		(const u8 *)"player_usb",  1, example_usbh_uac, (const u8 *)"\t player_usb\n"
	},
};



