/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "os_wrapper.h"
#include "usbh.h"
#include "usbh_hid.h"
#include "example_usbh_hid.h"

/* Private defines -----------------------------------------------------------*/

/**
 * @brief Enable hot-plug support.
 * When enabled, the hotplug thread waits on detach_sema; on each detach it
 * tears down the HID class driver and the USB host core, then reinitializes
 * so a freshly plugged device is re-enumerated automatically.
 */
#define CONFIG_USBH_HID_HOTPLUG                     1

/* Thread priorities */
#define USBH_HID_INIT_THREAD_PRIORITY               4U
#define USBH_HID_HOTPLUG_THREAD_PRIORITY            6U

/* Thread stack sizes */
#define USBH_HID_MAIN_TASK_STACK_SIZE               1024U
#define USBH_HID_MAIN_THREAD_PRIORITY               4U
#define USBH_HID_INIT_THREAD_STACK_SIZE             768U
#define USBH_HID_HOTPLUG_THREAD_STACK_SIZE          768U

/* Private function prototypes -----------------------------------------------*/
static int usbh_hid_cb_report(usbh_hid_event_t *event);
static int usbh_hid_cb_attach(void);
static int usbh_hid_cb_detach(void);
static int usbh_hid_cb_setup(void);
static int usbh_hid_cb_process(usb_host_t *host, u8 msg);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "HID";

/**
 * @brief Application-level context for the HID demo.
 */
typedef struct {
	rtos_sema_t detach_sema;    /*!< Signaled by detach callback; consumed by hotplug thread */
#if CONFIG_USBH_HID_HOTPLUG
	rtos_task_t hotplug_task;   /*!< Hot-plug monitoring thread handle */
#endif
} usbh_hid_ctx_t;

static usbh_hid_ctx_t usbh_hid_ctx;

/* USB host stack configuration */
static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_FULL,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = USBH_HID_MAIN_TASK_STACK_SIZE,
	.main_task_priority = USBH_HID_MAIN_THREAD_PRIORITY,
	.tick_source = USBH_SOF_TICK,
#if defined (CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAL2)
	.rx_fifo_depth = 501,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAPRO3)
	.rx_fifo_depth = 1712,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

/* HID user callback configuration */
static const usbh_hid_usr_cb_t usbh_hid_cfg = {
	.attach = usbh_hid_cb_attach,
	.detach = usbh_hid_cb_detach,
	.setup  = usbh_hid_cb_setup,
	.report = usbh_hid_cb_report,
};

/* General USB host event callbacks */
static const usbh_user_cb_t usbh_usr_cb = {
	.process = usbh_hid_cb_process,
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  HID report event callback. Invoked by the HID class driver when a
  *         parsed HID (consumer control) event is reported.
  * @param  event: Pointer to the HID event descriptor.
  * @retval Status
  */
static int usbh_hid_cb_report(usbh_hid_event_t *event)
{
	if (!event) {
		return HAL_OK;
	}

	switch (event->type) {
	case VOLUME_EVENT_CONSUMER_UP:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Volume Up\n");
		break;
	case VOLUME_EVENT_CONSUMER_DOWN:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Volume Down\n");
		break;
	case VOLUME_EVENT_CONSUMER_MUTE:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Mute\n");
		break;
	case VOLUME_EVENT_CONSUMER_PLAY_PAUSE:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Play/Pause\n");
		break;
	case VOLUME_EVENT_CONSUMER_STOP:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Stop\n");
		break;
	default:
		break;
	}

	return HAL_OK;
}

/**
  * @brief  HID attach callback, invoked when a HID device is enumerated.
  * @retval Status
  */
static int usbh_hid_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "HID attach\n");
	return HAL_OK;
}

/**
  * @brief  HID detach callback, invoked when the HID device is removed.
  *         In hot-plug mode, signals the hotplug thread to reinitialize.
  * @retval Status
  */
static int usbh_hid_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "HID detach\n");

#if CONFIG_USBH_HID_HOTPLUG
	rtos_sema_give(usbh_hid_ctx.detach_sema);
#endif

	return HAL_OK;
}

/**
  * @brief  HID setup-stage callback, invoked after class setup completes.
  * @retval Status
  */
static int usbh_hid_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "HID setup\n");
	return HAL_OK;
}

/**
  * @brief  General USB host process callback (attach/detach state machine).
  * @param  host: Host handle
  * @param  msg:  Host state message
  * @retval Status
  */
static int usbh_hid_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);

	switch (msg) {
	case USBH_MSG_DISCONNECTED:
		RTK_LOGS(TAG, RTK_LOG_INFO, "DISCONNECT\n");
		break;
	case USBH_MSG_CONNECTED:
		RTK_LOGS(TAG, RTK_LOG_INFO, "CONNECT\n");
		break;
	default:
		break;
	}

	return HAL_OK;
}

#if CONFIG_USBH_HID_HOTPLUG
/**
  * @brief  Hot-plug worker thread.
  *
  *         Sleeps on usbh_hid_ctx.detach_sema.  On each detach event it tears
  *         down the HID class driver and the USB host core, then reinitializes
  *         the stack so a freshly plugged device is re-enumerated.
  * @param  param: Unused.
  * @retval None
  */
static void example_usbh_hid_hotplug_thread(void *param)
{
	int ret;
	u32 hotplug_count = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(usbh_hid_ctx.detach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}

		hotplug_count++;
		RTK_LOGS(TAG, RTK_LOG_INFO, "\n========== Hotplug #%d ==========\n", hotplug_count);

		rtos_time_delay_ms(200);

		/* Tear down */
		usbh_stop();
		usbh_hid_deinit();
		usbh_deinit();

		rtos_time_delay_ms(100);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

		/* Re-initialize */
		RTK_LOGS(TAG, RTK_LOG_INFO, "Re-init USB host...\n");

		ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "USB re-init fail: %d\n", ret);
			break;
		}

		ret = usbh_hid_init(&usbh_hid_cfg);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "HID re-init fail: %d\n", ret);
			usbh_deinit();
			break;
		}

		usbh_start();

		RTK_LOGS(TAG, RTK_LOG_INFO, "Re-init complete\n");
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread exited\n");
	usbh_hid_ctx.hotplug_task = NULL;
	rtos_task_delete(NULL);
}
#endif /* CONFIG_USBH_HID_HOTPLUG */

/**
  * @brief  Bootstrap thread: creates synchronization primitives, brings up
  *         the USB host stack, starts the hotplug thread, then self-deletes.
  * @param  param: Unused.
  * @retval None
  */
static void example_usbh_hid_init_thread(void *param)
{
	int ret;

	UNUSED(param);

#if CONFIG_USBH_HID_HOTPLUG
	ret = rtos_sema_create(&usbh_hid_ctx.detach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create detach sema fail\n");
		goto example_exit;
	}
#endif

	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init fail: %d\n", ret);
		goto free_sema_exit;
	}

	ret = usbh_hid_init(&usbh_hid_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "HID init fail: %d\n", ret);
		goto usb_deinit_exit;
	}

	/* All class drivers registered; start USB TRX so enumeration can run. */
	usbh_start();

#if CONFIG_USBH_HID_HOTPLUG
	ret = rtos_task_create(&usbh_hid_ctx.hotplug_task, "example_usbh_hid_hotplug_thread",
						   example_usbh_hid_hotplug_thread, NULL,
						   USBH_HID_HOTPLUG_THREAD_STACK_SIZE, USBH_HID_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
		goto usbh_hid_deinit_exit;
	}
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH HID example running\n");
	goto example_exit;

#if CONFIG_USBH_HID_HOTPLUG
usbh_hid_deinit_exit:
	usbh_stop();
#endif
	usbh_hid_deinit();

usb_deinit_exit:
	usbh_deinit();

free_sema_exit:
#if CONFIG_USBH_HID_HOTPLUG
	rtos_sema_delete(usbh_hid_ctx.detach_sema);
#endif

example_exit:
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  USB Host HID example entry point.
  * @retval None
  */
void example_usbh_hid(void)
{
	int ret;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH HID demo start\n");

	ret = rtos_task_create(NULL, "example_usbh_hid_init_thread",
						   example_usbh_hid_init_thread, NULL,
						   USBH_HID_INIT_THREAD_STACK_SIZE, USBH_HID_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create init thread fail\n");
	}
}
