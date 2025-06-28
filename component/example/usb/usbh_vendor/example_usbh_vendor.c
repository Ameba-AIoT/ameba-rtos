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
#include "usbh_vendor.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "VND";
#define CONFIG_USBH_VENDOR_HOT_PLUG_TEST 1     /* Hot plug / memory leak test */

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int vendor_cb_detach(void);
static int vendor_cb_setup(void);
static int vendor_cb_process(usb_host_t *host, u8 id);

/* Private variables ---------------------------------------------------------*/

static rtos_sema_t vendor_detach_sema;
static __IO int vendor_is_ready = 0;

static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.dma_enable = FALSE,
	.ext_intr_en = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.isr_task_priority  = 4U,
	.main_task_priority = 3U,
	.sof_tick_en = 1U,

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

static usbh_vendor_cb_t vendor_usr_cb = {
	.detach = vendor_cb_detach,
	.setup = vendor_cb_setup,
};

static usbh_user_cb_t usbh_usr_cb = {
	.process = vendor_cb_process
};

/* Private functions ---------------------------------------------------------*/

static int vendor_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
#if CONFIG_USBH_VENDOR_HOT_PLUG_TEST
	rtos_sema_give(vendor_detach_sema);
#endif
	return HAL_OK;
}

static int vendor_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	vendor_is_ready = 1;
	return HAL_OK;
}

static int vendor_cb_process(usb_host_t *host, u8 id)
{
	UNUSED(host);

	switch (id) {
	case USBH_MSG_DISCONNECTED:
		vendor_is_ready = 0;
		break;

	case USBH_MSG_CONNECTED:
		break;

	default:
		break;
	}

	return HAL_OK;
}

#if CONFIG_USBH_VENDOR_HOT_PLUG_TEST
static void vendor_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(vendor_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			rtos_time_delay_ms(100);//make sure disconnect handle finish before deinit.
			usbh_vendor_deinit();
			usbh_deinit();

			rtos_time_delay_ms(10);

			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%08x\n", rtos_mem_get_free_heap_size());

			ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail: %d\n", ret);
				break;
			}

			ret = usbh_vendor_init(&vendor_usr_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init vendor fail: %d\n", ret);
				usbh_deinit();
				break;
			}
		}
	}

	rtos_task_delete(NULL);
}
#endif

void example_usbh_vendor_thread(void *param)
{
	int status;
#if CONFIG_USBH_VENDOR_HOT_PLUG_TEST
	rtos_task_t task;
#endif

	UNUSED(param);

	rtos_sema_create(&vendor_detach_sema, 0U, 1U);
	status = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (status != HAL_OK) {
		goto error_exit;
	}

	status = usbh_vendor_init(&vendor_usr_cb);
	if (status < 0) {
		usbh_deinit();
		goto error_exit;
	}

#if CONFIG_USBH_VENDOR_HOT_PLUG_TEST
	status = rtos_task_create(&task, "vendor_hotplug_thread", vendor_hotplug_thread, NULL, 1024U * 2, 2U);
	if (status != RTK_SUCCESS) {
		usbh_vendor_deinit();
		usbh_deinit();
		goto error_exit;
	}
#endif

	goto example_exit;

error_exit:
	rtos_sema_delete(vendor_detach_sema);
example_exit:
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH vendor demo stop\n");
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbh_vendor(void)
{
	int status;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH vendor demo start\n");
	status = rtos_task_create(&task, "example_usbh_vendor_thread", example_usbh_vendor_thread, NULL, 1024U * 2, 2U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}
}

