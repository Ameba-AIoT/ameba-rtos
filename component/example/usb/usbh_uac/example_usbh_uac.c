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

static u8 uac_isoc_tx_buf[USBH_UAC_ISOC_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

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

static int busy_count;
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

static void uac_isoc_test(void *param)
{
	u32 i;
	int ret;
	u32 total_len;
	u32 offset;
	u32 send_len;
	int frame_size;
	int my_data_len = 600;
	char my_data[my_data_len];

	UNUSED(param);
	usb_os_memset((void *)my_data, USB_OUT_DATA, my_data_len);
	if (rtos_sema_take(uac_isoc_start_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
		// idx 0 means isoc out
		if (usbh_uac_set_alt_setting(0, USBH_UAC_CHANNELS, USBH_UAC_BITWIDTH, USBH_UAC_SAMPLING_FREQ) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "No suitable interface/altsettinfg, return\n");
			rtos_task_delete(NULL);
			return;
		}
		//update frame size, idx 0 means isoc out
		frame_size = usbh_uac_get_frame_size(0);
		rtos_time_delay_ms(100);

		RTK_LOGS(TAG, RTK_LOG_INFO, "ISOC uac test start, test_cnt %d, size %d, frame %d\n", USBH_UAC_TEST_CNT, my_data_len, frame_size);

		for (i = 0; i < USBH_UAC_TEST_CNT; i++) {
			busy_count = 0;
			if (!uac_is_ready) {
				rtos_time_delay_ms(500);
				RTK_LOGS(TAG, RTK_LOG_WARN, "Device disconnect, test abort\n");
				// stop play
				break;
			}

			total_len = my_data_len;
			offset = 0;
			while (offset < total_len) {
				send_len = frame_size;
				if (offset + send_len > total_len) {
					send_len = total_len - offset;
					usb_os_memcpy(uac_isoc_tx_buf, my_data + offset, send_len);
					usb_os_memset(uac_isoc_tx_buf + send_len, 0x0, frame_size - send_len);// not enough 1 frame, fill 0x0
				} else {
					usb_os_memcpy(uac_isoc_tx_buf, my_data + offset, send_len);
				}

				ret = usbh_uac_isoc_transmit(uac_isoc_tx_buf, send_len);
				if (ret == HAL_OK) {
					if (rtos_sema_take(uac_isoc_send_done_sema, 1000U) == RTK_SUCCESS) {
						offset += send_len;
						continue;
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "This ISOC packet not done!\n");
						break;
					}
				} else if (ret == HAL_BUSY) {
					rtos_time_delay_ms(1);
					continue;
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "TX start fail: %d\n", ret);
					break;
				}
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "%d Send finished. Total bytes sent: %u, busy count %d\n", i, offset, busy_count);
		}

		usbh_uac_stop_play();
	}

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

		status = rtos_task_create(&isoc_task, "uac_isoc_test", uac_isoc_test, NULL, 1024U * 2, 3U);
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

/* Exported functions --------------------------------------------------------*/

void example_usbh_uac(void)
{
	int status;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH UAC demo start\n");

	status = rtos_task_create(&task, "example_usbh_uac_thread", example_usbh_uac_thread, NULL, 1024U * 2, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}
}

