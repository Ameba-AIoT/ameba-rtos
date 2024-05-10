/**
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "usbd.h"
#include "usbd_msc.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define CONFIG_USBD_MSC_HOTPLUG						1

// USB speed
#ifdef CONFIG_USB_FS
#define CONFIG_USBD_MSC_SPEED						USB_SPEED_FULL
#else
#define CONFIG_USBD_MSC_SPEED						USB_SPEED_HIGH
#endif

// Thread priorities
#define CONFIG_USBD_MSC_INIT_THREAD_PRIORITY		5U
#define CONFIG_USBD_MSC_ISR_THREAD_PRIORITY			7U
#define CONFIG_USBD_MSC_HOTPLUG_THREAD_PRIORITY		8U // Should be higher than CONFIG_USBD_MSC_ISR_THREAD_PRIORITY

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void msc_cb_status_changed(u8 status);

/* Private variables ---------------------------------------------------------*/

static usbd_config_t msc_cfg = {
	.speed = CONFIG_USBD_MSC_SPEED,
	.dma_enable = 1U,
	.isr_priority = CONFIG_USBD_MSC_ISR_THREAD_PRIORITY,
	.nptx_max_err_cnt = {0U, 2000U, },
};

static usbd_msc_cb_t msc_cb = {
	.status_changed = msc_cb_status_changed
};

#if CONFIG_USBD_MSC_HOTPLUG
static u8 msc_attach_status;
static rtos_sema_t msc_attach_status_changed_sema;
#endif

/* Private functions ---------------------------------------------------------*/

static void msc_cb_status_changed(u8 status)
{
	printf("\n[MSC] USB status changed: %d\n", status);
#if CONFIG_USBD_MSC_HOTPLUG
	msc_attach_status = status;
	rtos_sema_give(msc_attach_status_changed_sema);
#endif
}

#if CONFIG_USBD_MSC_HOTPLUG
static void msc_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(msc_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == SUCCESS) {
			if (msc_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				printf("\n[MSC] USB DETACHED\n");
				usbd_msc_deinit();
				ret = usbd_deinit();
				if (ret != 0) {
					printf("\n[MSC] Fail to de-init USBD driver\n");
					break;
				}
				printf("\n[MSC] Free heap size: 0x%lx\n", rtos_mem_get_free_heap_size());
				ret = usbd_init(&msc_cfg);
				if (ret != 0) {
					printf("\n[MSC] Fail to re-init USBD driver\n");
					break;
				}
				ret = usbd_msc_init(&msc_cb);
				if (ret != 0) {
					printf("\n[MSC] Fail to re-init MSC class\n");
					usbd_deinit();
					break;
				}
			} else if (msc_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				printf("\n[MSC] USB ATTACHED\n");
			} else {
				printf("\n[MSC] USB INIT\n");
			}
		}
	}

	rtos_task_delete(NULL);
}
#endif // CONFIG_USBD_MSC_HOTPLUG

static void example_usbd_msc_thread(void *param)
{
	int status = 0;
#if CONFIG_USBD_MSC_HOTPLUG
	rtos_task_t task;
#endif

	UNUSED(param);

#if CONFIG_USBD_MSC_HOTPLUG
	rtos_sema_create(&msc_attach_status_changed_sema, 0U, 1U);
#endif

	status = usbd_init(&msc_cfg);
	if (status != HAL_OK) {
		printf("\n[MSC] USB device driver init fail\n");
		goto example_usbd_msc_thread_fail;
	}

	status = usbd_msc_init(&msc_cb);
	if (status != HAL_OK) {
		printf("\n[MSC] USB MSC init fail\n");
		usbd_deinit();
		goto example_usbd_msc_thread_fail;
	}

#if CONFIG_USBD_MSC_HOTPLUG
	status = rtos_task_create(&task, "msc_hotplug_thread", msc_hotplug_thread, NULL, 1024U, CONFIG_USBD_MSC_HOTPLUG_THREAD_PRIORITY);
	if (status != SUCCESS) {
		printf("\n[MSC] USB create hotplug thread fail\n");
		usbd_msc_deinit();
		usbd_deinit();
		goto example_usbd_msc_thread_fail;
	}
#endif // CONFIG_USBD_MSC_HOTPLUG

	printf("\n[MSC] USBD MSC demo started\n");

	rtos_task_delete(NULL);

	return;

example_usbd_msc_thread_fail:
#if CONFIG_USBD_MSC_HOTPLUG
	rtos_sema_delete(msc_attach_status_changed_sema);
#endif

	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbd_msc(void)
{
	int ret;
	rtos_task_t task;

	ret = rtos_task_create(&task, "example_usbd_msc_thread", example_usbd_msc_thread, NULL, 1024, CONFIG_USBD_MSC_INIT_THREAD_PRIORITY);
	if (ret != SUCCESS) {
		printf("\n[MSC] USBD MSC create thread fail\n");
	}
}

