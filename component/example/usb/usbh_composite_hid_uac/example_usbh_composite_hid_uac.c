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
#include "usbh_composite_hid_uac.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/
#define USBH_UAC_HOT_PLUG_TEST        1     /* Hot plug / memory leak test */

#define USBH_UAC_TEST_CNT             30
#define USBH_UAC_FRAME_CNT            20

#define USBH_UAC_CHANNELS             2
#define USBH_UAC_BITWIDTH             16
#define USBH_UAC_SAMPLING_FREQ        48000

#define USBH_UAC_ISOC_TEST_THREAD_PRIORITY    4
#define USBH_UAC_MAIN_THREAD_PRIORITY         5
#define USBH_UAC_ISR_THREAD_PRIORITY          6
#define USBH_UAC_HOTPLUG_THREAD_PRIORITY      7

#define USBH_UAC_XFER_CHECK           0   /* used to check the trx data valid */

#if USBH_UAC_XFER_CHECK
#define  USBH_UAC_OUT_DATA                0x88
static u32 usbh_uac_data_len = 1200;
static unsigned char usbh_uac_audio_data[1200] = {0,};
#else
#include "example_usbh_composite_hid_uac_audio_data.h"
#endif

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int usbh_hid_cb_report(usbh_composite_hid_event_t *event);
static int usbh_uac_cb_init(void);
static int usbh_uac_cb_deinit(void);
static int usbh_uac_cb_attach(void);
static int usbh_uac_cb_detach(void);
static int usbh_uac_cb_setup(void);
static int usbh_uac_cb_isoc_transmitted(usbh_urb_state_t state);
static int usbh_uac_cb_process(usb_host_t *host, u8 id);
/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "UAC";

static rtos_sema_t usbh_uac_detach_sema;
static rtos_sema_t usbh_uac_attach_sema;
static rtos_sema_t usbh_uac_isoc_start_sema;

static __IO int usbh_uac_is_ready = 0;
static int usbh_uac_busy_count;
static int usbh_uac_err_count;

static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_FULL,
	.dma_enable = 1U,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.isr_task_priority  = USBH_UAC_ISR_THREAD_PRIORITY,
	.main_task_priority = USBH_UAC_MAIN_THREAD_PRIORITY,
	.sof_tick_enable = 1U,
	.alt_max_cnt = 10U,
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

static usbh_composite_uac_usr_cb_t usbh_uac_cfg = {
	.init = usbh_uac_cb_init,
	.deinit = usbh_uac_cb_deinit,
	.attach = usbh_uac_cb_attach,
	.detach = usbh_uac_cb_detach,
	.setup = usbh_uac_cb_setup,
	.isoc_transmitted = usbh_uac_cb_isoc_transmitted,
};

static usbh_composite_hid_usr_cb_t usbh_hid_cfg = {
	.report = usbh_hid_cb_report,
};

static usbh_user_cb_t usbh_usr_cb = {
	.process = usbh_uac_cb_process
};

/* Private functions ---------------------------------------------------------*/

static int usbh_hid_cb_report(usbh_composite_hid_event_t *event)
{
	if (!event) {
		return HAL_OK;  //todo
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n=== Handling Event ===\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Press:%d\n", event->is_press);

	switch (event->type) {

	case VOLUME_EVENT_CONSUMER_UP:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "=== Executing Volume Up (Consumer) ===\n");
		break;
	case VOLUME_EVENT_CONSUMER_DOWN:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "=== Executing Volume Down (Consumer) ===\n");
		break;
	case VOLUME_EVENT_CONSUMER_MUTE:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "=== Executing Mute (Consumer) ===\n");
		break;

	case VOLUME_EVENT_CONSUMER_PLAY_PAUSE:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "=== Executing PLAY PAUSE (Consumer) ===\n");
		break;
	case VOLUME_EVENT_CONSUMER_STOP:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "=== Executing STOP (Consumer) ===\n");
		break;
	default:
		break;
	}

	return HAL_OK;
}

static int usbh_uac_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC INIT\n");
	return HAL_OK;
}

static int usbh_uac_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC DEINIT\n");
	return HAL_OK;
}

static int usbh_uac_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC ATTACH\n");
	rtos_sema_give(usbh_uac_attach_sema);
	return HAL_OK;
}

static int usbh_uac_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC DETACH\n");
#if USBH_UAC_HOT_PLUG_TEST
	rtos_sema_give(usbh_uac_detach_sema);
#endif
	usbh_uac_is_ready = 0;
	return HAL_OK;
}

static int usbh_uac_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC SETUP\n");
	usbh_uac_is_ready = 1;
	rtos_sema_give(usbh_uac_isoc_start_sema);
	return HAL_OK;
}

static int usbh_uac_cb_isoc_transmitted(usbh_urb_state_t state)
{
	if (state == USBH_URB_DONE) {
		/*TX ok*/
	} else if (state == USBH_URB_BUSY) {
		usbh_uac_busy_count++;
	} else {
		//RTK_LOGS(TAG, RTK_LOG_ERROR, "TX fail: %d\n", state);
	}

	return HAL_OK;
}

static int usbh_uac_cb_process(usb_host_t *host, u8 id)
{
	UNUSED(host);

	switch (id) {
	case USBH_MSG_DISCONNECTED:
		usbh_uac_is_ready = 0;
		break;

	case USBH_MSG_CONNECTED:
		break;

	default:
		break;
	}

	return HAL_OK;
}

static void usbh_uac_isoc_test(void *param)
{
	const unsigned char *usbh_uac_audio_data_handle = usbh_uac_audio_data;
	const usbh_audio_fmt_t *fmt_info = NULL;
	u32 audio_total_data_len = usbh_uac_data_len;
	const usbh_audio_fmt_t *audio_fmt = NULL;
	u8 *buffer = NULL;
	u32 frame_size;
	u32 total_len;
	u32 send_len;
	u32 offset;
	u32 ret;
	u32 i;
	u8 fmt_cnt;

	UNUSED(param);
	if (rtos_sema_take(usbh_uac_isoc_start_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {

		fmt_info = usbh_composite_uac_get_alt_setting(0, &fmt_cnt);

		if ((fmt_info == NULL) || (fmt_cnt == 0)) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Get audio fmt fail\n");
		} else {
			for (i = 0; i < fmt_cnt; i++) {
				audio_fmt = &(fmt_info[i]);
				RTK_LOGS(TAG, RTK_LOG_INFO, "Audio fmt %d: %d %d %d\n", i, audio_fmt->ch_cnt, audio_fmt->bit_width, audio_fmt->sampling_freq);
			}
		}

		// idx 0 means isoc out
		if (usbh_composite_uac_set_alt_setting(0, USBH_UAC_CHANNELS, USBH_UAC_BITWIDTH, USBH_UAC_SAMPLING_FREQ) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "No suitable interface/altsettinfg, return\n");
			rtos_task_delete(NULL);
			return;
		}
		//update frame size, idx 0 means isoc out
		frame_size = usbh_composite_uac_get_frame_size(0);
		rtos_time_delay_ms(100);

		RTK_LOGS(TAG, RTK_LOG_INFO, "ISOC uac test %d start, test_cnt %d, size %d, frame %d\n", i, USBH_UAC_TEST_CNT, audio_total_data_len, frame_size);

		for (i = 0; i < (u32)USBH_UAC_TEST_CNT; i++) {
			usbh_uac_busy_count = 0;
			usbh_uac_err_count = 0;
			if (!usbh_uac_is_ready) {
				rtos_time_delay_ms(500);
				RTK_LOGS(TAG, RTK_LOG_WARN, "Device disconnect, test abort\n");
				break;
			}
#if USBH_UAC_XFER_CHECK
			memset(usbh_uac_audio_data, USBH_UAC_OUT_DATA, usbh_uac_data_len);
#endif
			total_len = audio_total_data_len;
			offset = 0;
			usbh_composite_uac_start_play();
			while (offset < total_len) {
				TRNG_get_random_bytes(&send_len, 1);
				// RTK_LOGS(TAG, RTK_LOG_INFO, "xfer len %d\n",send_len);
				if (send_len == 0) {
					continue;
				}

				if (offset + send_len > total_len) {
					send_len = total_len - offset;
				}

				buffer = (u8 *)(usbh_uac_audio_data_handle + offset);
				ret = usbh_composite_uac_write(buffer, send_len, 10);
				if (ret != send_len) {
					usbh_uac_err_count++;
					continue;
				}

				offset += ret;
			}

			rtos_time_delay_ms(50);
			usbh_composite_uac_stop_play();
			RTK_LOGS(TAG, RTK_LOG_INFO, "%d Send finished. Total bytes sent: %u, err count %d, busy count %d\n", i, offset, usbh_uac_err_count, usbh_uac_busy_count);
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Test done\n");
	rtos_time_delay_ms(1000);

	rtos_task_delete(NULL);
}

#if USBH_UAC_HOT_PLUG_TEST
static void usbh_uac_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(usbh_uac_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			rtos_time_delay_ms(100);
			usbh_composite_deinit();
			usbh_deinit();
			rtos_time_delay_ms(10);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
			if (ret != HAL_OK) {
				break;
			}

			ret = usbh_composite_init(&usbh_hid_cfg, &usbh_uac_cfg, USBH_UAC_FRAME_CNT);
			if (ret < 0) {
				usbh_deinit();
				break;
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
#if USBH_UAC_HOT_PLUG_TEST
	rtos_task_t hotplug_task;
#endif

	UNUSED(param);

	rtos_sema_create(&usbh_uac_detach_sema, 0U, 1U);
	rtos_sema_create(&usbh_uac_attach_sema, 0U, 1U);
	rtos_sema_create(&usbh_uac_isoc_start_sema, 0U, 1U);

	status = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (status != HAL_OK) {
		goto free_sema_exit;
	}

	status = usbh_composite_init(&usbh_hid_cfg, &usbh_uac_cfg, USBH_UAC_FRAME_CNT);  /*0 means use default transfer size, and it can not exceed 65536*/
	if (status != HAL_OK) {
		goto usb_deinit_exit;
	}

#if USBH_UAC_HOT_PLUG_TEST
	status = rtos_task_create(&hotplug_task, "usbh_uac_hotplug_thread", usbh_uac_hotplug_thread, NULL, 1024U * 2, USBH_UAC_HOTPLUG_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		goto usbh_uac_deinit_exit;
	}
#endif

	if (rtos_sema_take(usbh_uac_attach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
		status = rtos_task_create(&isoc_task, "usbh_uac_isoc_test", usbh_uac_isoc_test, NULL, 1024U * 2, USBH_UAC_ISOC_TEST_THREAD_PRIORITY);
		if (status != RTK_SUCCESS) {
			goto delete_hotplug_task_exit;
		}
	}

	goto example_exit;

delete_hotplug_task_exit:
#if USBH_UAC_HOT_PLUG_TEST
	rtos_task_delete(hotplug_task);
#endif

#if USBH_UAC_HOT_PLUG_TEST
usbh_uac_deinit_exit:
#endif
	usbh_composite_deinit();

usb_deinit_exit:
	usbh_deinit();

free_sema_exit:
	rtos_sema_delete(usbh_uac_detach_sema);
	rtos_sema_delete(usbh_uac_attach_sema);
	rtos_sema_delete(usbh_uac_isoc_start_sema);

example_exit:
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH UAC demo stop\n");
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/
void example_usbh_composite_hid_uac(void)
{
	int status;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH UAC&HID composite demo start\n");

	status = rtos_task_create(&task, "example_usbh_uac_thread", example_usbh_uac_thread, NULL, 1024U * 2, 2U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}
}
