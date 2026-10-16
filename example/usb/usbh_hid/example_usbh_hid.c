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
static void usbh_hid_cb_report(usbh_hid_event_t *event);
static void usbh_hid_cb_attached(void);
static void usbh_hid_cb_detached(void);
static void usbh_hid_cb_setup(void);
static void usbh_hid_cb_process(usb_host_t *host, u8 msg);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "HID";

#if CONFIG_USBH_HID_HOTPLUG
/**
 * @brief Application-level context for the HID demo.
 * @note  Both members exist only to run the hot-plug worker, and every use of
 *        them is already inside a CONFIG_USBH_HID_HOTPLUG block, so the whole
 *        context is compiled out with it. Without this guard the instance is
 *        unreferenced when hot-plug is disabled, which the build treats as an
 *        error (-Werror=unused-variable).
 */
typedef struct {
	rtos_sema_t detach_sema;    /*!< Signaled by detach callback; consumed by hotplug thread */
	rtos_task_t hotplug_task;   /*!< Hot-plug monitoring thread handle */
} usbh_hid_ctx_t;

static usbh_hid_ctx_t usbh_hid_ctx;
#endif

/* Serializes the USB stack bring up / tear down against any caller of the HID
   class API. This example drives no transfers of its own - the class runs its
   own report parse thread and hands events up through the report callback - so
   today the only contender is the init thread's rollback path. The lock is kept
   anyway because usbh_hid_deinit() frees hid_ctrl_buf and the report ring buffer
   while only checking hid->host on entry: under SMP any future caller of
   usbh_hid_send_report() on another core would be inside that API while this
   thread frees what it dereferences. Excluding such a caller has to be done by
   lock, not by core.
   Created before the stack comes up and never deleted: a caller of the class API
   can not be joined by the example, so there is no point in time at which this
   mutex is provably unreferenced. usbh_hid_stack_ready is the gate that keeps
   callers off the stack instead. */
static rtos_mutex_t usbh_hid_state_mutex = NULL;

/* 1: usbh_init() and usbh_hid_init() are both done and the class API may be
   called. Cleared before any tear down, so a non-zero value also implies
   usbh_hid_state_mutex is valid. */
static volatile u8 usbh_hid_stack_ready = 0;

/* USB host stack configuration */
static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_FULL,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = USBH_HID_MAIN_TASK_STACK_SIZE,
	.main_task_priority = USBH_HID_MAIN_THREAD_PRIORITY,
	.tick_source = USBH_SOF_TICK,
#if defined(CONFIG_AMEBAGREEN2)
	/*FIFO total 1024 DWORD, resv 12 DWORD for DMA*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined(CONFIG_RLE1509)
	/*FIFO total 1024 DWORD, resv 48 DWORD */
	.rx_fifo_depth = 464,
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
	.attached = usbh_hid_cb_attached,
	.detached = usbh_hid_cb_detached,
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
  */
static void usbh_hid_cb_report(usbh_hid_event_t *event)
{
	if (!event) {
		return;
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
}

/**
  * @brief  HID attach callback, invoked when a HID device is enumerated.
  */
static void usbh_hid_cb_attached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "HID attach\n");
}

/**
  * @brief  HID detach callback, invoked when the HID device is removed.
  *         In hot-plug mode, signals the hotplug thread to reinitialize.
  */
static void usbh_hid_cb_detached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "HID detach\n");

#if CONFIG_USBH_HID_HOTPLUG
	rtos_sema_give(usbh_hid_ctx.detach_sema);
#endif
}

/**
  * @brief  HID setup-stage callback, invoked after class setup completes.
  */
static void usbh_hid_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "HID setup\n");
}

/**
  * @brief  General USB host process callback (attach/detach state machine).
  * @param  host: Host handle
  * @param  msg:  Host state message
  * @retval Status
  */
static void usbh_hid_cb_process(usb_host_t *host, u8 msg)
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

		/* Tear down. Close the gate under the lock: a class API call already in
		   flight finishes first, and any later one gives up on the cleared flag. */
		rtos_mutex_take(usbh_hid_state_mutex, RTOS_MAX_TIMEOUT);
		usbh_hid_stack_ready = 0;

		usbh_stop();
		usbh_hid_deinit();
		usbh_deinit();
		rtos_mutex_give(usbh_hid_state_mutex);

		rtos_time_delay_ms(100);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

		/* Re-initialize */
		RTK_LOGS(TAG, RTK_LOG_INFO, "Re-init USB host...\n");

		rtos_mutex_take(usbh_hid_state_mutex, RTOS_MAX_TIMEOUT);

		ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
		if (ret != HAL_OK) {
			rtos_mutex_give(usbh_hid_state_mutex);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "USB re-init fail: %d\n", ret);
			break;
		}

		ret = usbh_hid_init(&usbh_hid_cfg);
		if (ret != HAL_OK) {
			usbh_deinit();
			rtos_mutex_give(usbh_hid_state_mutex);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "HID re-init fail: %d\n", ret);
			break;
		}

		usbh_start();

		/* Open the gate as the very last step, so no caller runs against a
		   half-rebuilt stack. */
		usbh_hid_stack_ready = 1;
		rtos_mutex_give(usbh_hid_state_mutex);

		RTK_LOGS(TAG, RTK_LOG_INFO, "Re-init complete\n");
	}

	/* A re-init failed, so the stack is gone for good: keep every class API
	   caller out from here on. The detach sema is deliberately left alive - the
	   class callback may still be about to give it, and freeing it here would
	   pull it out from under that. */
	usbh_hid_stack_ready = 0;

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

	/* Created before the stack comes up and never deleted, see the declaration */
	if (usbh_hid_state_mutex == NULL) {
		ret = rtos_mutex_create(&usbh_hid_state_mutex);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create state mutex fail\n");
			goto example_exit;
		}
	}

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

	/* The stack is fully up: allow the class API to be called. */
	usbh_hid_stack_ready = 1;

#if CONFIG_USBH_HID_HOTPLUG
	ret = rtos_task_create(&usbh_hid_ctx.hotplug_task, "usbh_hid_hotplug_thread",
						   example_usbh_hid_hotplug_thread, NULL,
						   USBH_HID_HOTPLUG_THREAD_STACK_SIZE, USBH_HID_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
		goto usbh_hid_deinit_exit;
	}
#if defined(CONFIG_SMP)
	/* The USB OTG ISR is delivered on CPU0: the GIC pins every SPI to core 0
	   (ITARGETSR is written to 0x01010101 in arm_gic.c gic_dist_init). Pinning
	   the hotplug/deinit thread to CPU0 puts it on the same core as the ISR, so
	   the local interrupt disables inside the teardown are meaningful again. Any
	   class API caller stays unaffined: a pending teardown is excluded from it by
	   usbh_hid_state_mutex, not by core. */
	rtos_task_set_affinity(usbh_hid_ctx.hotplug_task, 0);
#endif
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH HID example running\n");
	goto example_exit;

#if CONFIG_USBH_HID_HOTPLUG
usbh_hid_deinit_exit:
	/* The gate is already open, so hold it shut under the lock for this rollback
	   too, exactly as the hotplug thread does. usbh_deinit() is called here rather
	   than by falling through to usb_deinit_exit, so the whole teardown happens
	   under one lock hold. */
	rtos_mutex_take(usbh_hid_state_mutex, RTOS_MAX_TIMEOUT);
	usbh_hid_stack_ready = 0;
	usbh_stop();
	usbh_hid_deinit();
	usbh_deinit();
	rtos_mutex_give(usbh_hid_state_mutex);
	goto free_sema_exit;
#endif

usb_deinit_exit:
	/* usbh_hid_init() failed, so only the host core is up and the gate was never
	   opened: no lock needed here. */
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

	ret = rtos_task_create(NULL, "usbh_hid_init_thread",
						   example_usbh_hid_init_thread, NULL,
						   USBH_HID_INIT_THREAD_STACK_SIZE, USBH_HID_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create init thread fail\n");
	}
}
