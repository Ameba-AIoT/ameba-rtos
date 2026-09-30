/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "usbd_msc.h"
#include "os_wrapper.h"
#if defined(CONFIG_AMEBASMART)
#include "ameba_sd.h"
#endif
#ifdef CONFIG_USBD_MSC_SECOND_FLASH
#include "vfs_second_nor_flash.h"
#endif

/* Private defines -----------------------------------------------------------*/

// Endpoint address
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
#define MSC_BULK_IN_EP                            0x82U
#else
#define MSC_BULK_IN_EP                            0x81U
#endif
#define MSC_BULK_OUT_EP                           0x02U

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define MSC_USB_HOTPLUG                           1
#define MSC_SD_HOTPLUG                            0

#if !defined(CONFIG_USBD_MSC_SD_MODE) && (MSC_SD_HOTPLUG == 1)
#error "Only SD card (SD mode) support SD hotplug"
#endif

#if !defined(CONFIG_AMEBASMART) && (MSC_SD_HOTPLUG == 1)
#error "SD hotplug is not supported"
#endif

// USB speed
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define MSC_USB_SPEED                            USB_SPEED_FULL
#else
#define MSC_USB_SPEED                            USB_SPEED_HIGH
#endif

// Thread priorities
#define MSC_INIT_THREAD_PRIORITY                  5
#define MSC_USB_HOTPLUG_THREAD_PRIORITY           8
#define MSC_SD_HOTPLUG_THREAD_PRIORITY            8

// Thread stack sizes
#define MSC_INIT_THREAD_STACK_SIZE                1024U
#define MSC_USB_HOTPLUG_THREAD_STACK_SIZE         1024U
#define MSC_SD_HOTPLUG_THREAD_STACK_SIZE          1024U


/* Private types -------------------------------------------------------------*/
typedef enum {
	USBD_MSC_HOTPLUG_NONE = 0U,
	USBD_MSC_USB_HOTPLUG,
	USBD_MSC_SD_HOTPLUG
} usbd_msc_hotplug_type_t;

/* Private macros ------------------------------------------------------------*/

#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
/* Take/release the stack lock and track ownership in the caller's local
   lock_held, so every exit path (normal, fatal) releases it exactly once. */
#define MSC_STACK_LOCK()        do { rtos_mutex_take(msc_stack_lock, RTOS_SEMA_MAX_COUNT); lock_held = 1U; } while (0)
#define MSC_STACK_UNLOCK()      do { lock_held = 0U; rtos_mutex_give(msc_stack_lock); } while (0)
#else
#define MSC_STACK_LOCK()
#define MSC_STACK_UNLOCK()
#endif

/* Private function prototypes -----------------------------------------------*/

static void msc_cb_status_changed(u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "MSC";

static const usbd_config_t msc_cfg = {
	.speed = MSC_USB_SPEED,
	.isr_priority = INT_PRI_MIDDLE,
#if defined(CONFIG_AMEBASMART)
	.nptx_max_epmis_cnt = 100U,
#elif defined(CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 724U,
	.ptx_fifo_depth = {0U, 256U, 0U, 0U, 0U},
#elif defined(CONFIG_RLE1509)
	.rx_fifo_depth = 688U,
	.ptx_fifo_depth = {0U, 256U, 0U, 0U, 0U},
#elif defined (CONFIG_AMEBAL2)
	.rx_fifo_depth = 677U,
	.ptx_fifo_depth = {256U, 16U, 16U, 16U},
#elif defined (CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1680U,
	.ptx_fifo_depth = {256U, 16U, 16U, },
#endif
};

static const usbd_msc_ep_cfg_t msc_ep = {
	.bulk_in_addr  = MSC_BULK_IN_EP,
	.bulk_out_addr = MSC_BULK_OUT_EP,
};

static const usbd_msc_cb_t msc_cb = {
	.status_changed = msc_cb_status_changed
};

#if MSC_USB_HOTPLUG
/* Written by the ISR, read by the hotplug thread, possibly on another core */
static volatile u8 msc_usb_attach_status;
static rtos_sema_t msc_usb_status_changed_sema;
#endif

#if MSC_SD_HOTPLUG
/* Written by the SD card-detect callback, read by the SD thread, possibly on another core */
static volatile u8 msc_sd_status;
static rtos_sema_t msc_sd_status_changed_sema;
#endif

#if (MSC_SD_HOTPLUG == 1) || (MSC_USB_HOTPLUG == 1)
/* Read from ISR/card-detect callback context, so it must not be cached. */
static volatile usbd_msc_hotplug_type_t msc_hotplug_ongoing_type;
/* Set by whichever hotplug thread hits an unrecoverable error, tells the other
   one to quit so that the last thread standing frees the shared objects. */
static volatile u8 msc_stack_fatal;
#endif

#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
/* Serializes the two hotplug threads' deinit/reinit of the device stack. Each
   thread publishes msc_hotplug_ongoing_type (volatile) BEFORE taking the lock so
   the ISR/card-detect callback stops feeding the peer's semaphore while the stack
   is being rebuilt; the notification that slipped through earlier is drained by
   the peer once the lock is released. */
static rtos_mutex_t msc_stack_lock;
#endif

/* Private functions ---------------------------------------------------------*/

#if (MSC_SD_HOTPLUG == 1) || (MSC_USB_HOTPLUG == 1)
/**
  * @brief  Free the objects shared by the example threads
  * @note   Only called by the last running thread, after the USB stack is fully
  *         deinited, so that no ISR callback can touch these objects any more
  * @retval None
  */
static void msc_free_resource(void)
{
#if MSC_SD_HOTPLUG
	/* Unhook the card-detect callback before its semaphore goes away. */
	SD_SetCdCallback(NULL);
	rtos_sema_delete(msc_sd_status_changed_sema);
	msc_sd_status_changed_sema = NULL;
#endif
#if MSC_USB_HOTPLUG
	rtos_sema_delete(msc_usb_status_changed_sema);
	msc_usb_status_changed_sema = NULL;
#endif
#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
	rtos_mutex_delete(msc_stack_lock);
	msc_stack_lock = NULL;
#endif
}

/**
  * @brief  Leave the example: the first thread to fail tears the stack down and
  *         wakes its peer, the last one out frees the shared objects
  * @note   The stack is already deinited by the caller, so no ISR callback can
  *         give a semaphore any more
  * @retval None
  */
static void msc_hotplug_thread_exit(void)
{
	u8 first = (msc_stack_fatal == 0U) ? 1U : 0U;

	msc_stack_fatal = 1U;

#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
	if (first != 0U) {
		/* Wake the peer thread so it can observe the flag and quit; it frees the
		   shared objects as the last thread standing. */
		rtos_sema_give(msc_usb_status_changed_sema);
		rtos_sema_give(msc_sd_status_changed_sema);
	} else {
		msc_free_resource();
	}
#else
	UNUSED(first);
	msc_free_resource();
#endif
}
#endif // (MSC_SD_HOTPLUG == 1) || (MSC_USB_HOTPLUG == 1)

/**
  * @brief  Handle MSC attach status change notifications from the USB stack
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  old_status: Previous attach status
  * @param  status: New attach status
  * @retval None
  */
static void msc_cb_status_changed(u8 old_status, u8 status)
{
	UNUSED(old_status);

#if MSC_USB_HOTPLUG
	msc_usb_attach_status = status;
	if (msc_hotplug_ongoing_type != USBD_MSC_SD_HOTPLUG) {
		rtos_sema_give(msc_usb_status_changed_sema);
	}
#else
	UNUSED(status);
#endif
}

#if MSC_USB_HOTPLUG
static void example_usbd_msc_usb_hotplug_thread(void *param)
{
	int ret = 0;
#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
	u8 lock_held = 0U;
#endif

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(msc_usb_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (msc_stack_fatal != 0U) {
				break;
			}
			if (msc_usb_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				/* Publish before taking the lock so the peer's notifications are
				   gated even if it currently owns the stack. */
				msc_hotplug_ongoing_type = USBD_MSC_USB_HOTPLUG;
				MSC_STACK_LOCK();
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");
				usbd_msc_deinit();
				usbd_deinit();
				usbd_msc_disk_deinit();
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				usbd_msc_disk_init();
				ret = usbd_init(&msc_cfg);
				if (ret != 0) {
					break;
				}
				ret = usbd_msc_init(&msc_cb, &msc_ep);
				if (ret != 0) {
					usbd_deinit();
					break;
				}
				msc_hotplug_ongoing_type = USBD_MSC_HOTPLUG_NONE;
				MSC_STACK_UNLOCK();
			} else if (msc_usb_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\n");
	/* The stack is down for good here. Release the lock BEFORE the hand-off so
	   the peer never blocks on it and msc_free_resource() does not delete a mutex
	   that is still held. */
#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
	if (lock_held != 0U) {
		MSC_STACK_UNLOCK();
	}
#endif
	msc_hotplug_thread_exit();
	rtos_task_delete(NULL);
}
#endif // MSC_USB_HOTPLUG

#if MSC_SD_HOTPLUG
static void example_usbd_msc_sd_hotplug_thread(void *param)
{
	int ret = 0;
#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
	u8 lock_held = 0U;
#endif

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(msc_sd_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (msc_stack_fatal != 0U) {
				break;
			}
			/* Publish before taking the lock so the peer's notifications are
			   gated even if it currently owns the stack. The lock is held from
			   card removal until the stack is rebuilt on the next insertion. */
			msc_hotplug_ongoing_type = USBD_MSC_SD_HOTPLUG;
#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
			if (lock_held == 0U) {
				MSC_STACK_LOCK();
			}
#endif
			if (msc_sd_status == SD_NODISK) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "SD card removed\n");
				usbd_msc_deinit();
				usbd_deinit();
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "SD card insert, re-init USB\n");
				SD_CardInit();
				ret = usbd_init(&msc_cfg);
				if (ret != 0) {
					break;
				}
				ret = usbd_msc_init(&msc_cb, &msc_ep);
				if (ret != 0) {
					usbd_deinit();
					break;
				}
				msc_hotplug_ongoing_type = USBD_MSC_HOTPLUG_NONE;
				MSC_STACK_UNLOCK();
			}
		}
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "SD card hotplug thread fail\n");
	/* Release the lock before the hand-off, see the USB hotplug thread. */
#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
	if (lock_held != 0U) {
		MSC_STACK_UNLOCK();
	}
#endif
	msc_hotplug_thread_exit();
	rtos_task_delete(NULL);
}

static void sd_intr_cb(SD_RESULT res)
{

	RTK_LOGS(TAG, RTK_LOG_INFO, "SD callback status: %d\n", res);
	msc_sd_status = res;
	if (msc_hotplug_ongoing_type != USBD_MSC_USB_HOTPLUG) {
		rtos_sema_give(msc_sd_status_changed_sema);
	}
}
#endif // MSC_SD_HOTPLUG

static void example_usbd_msc_thread(void *param)
{
	int ret = 0;
#if MSC_USB_HOTPLUG
	rtos_task_t usb_task;
#endif
#if MSC_SD_HOTPLUG
	rtos_task_t sd_task;
#endif
	UNUSED(param);

#if MSC_USB_HOTPLUG
	ret = rtos_sema_create(&msc_usb_status_changed_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto exit_usbd_msc_disk_init_fail;
	}
#endif

#if MSC_SD_HOTPLUG
	ret = rtos_sema_create(&msc_sd_status_changed_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto exit_usbd_msc_disk_init_fail;
	}
#endif

#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
	ret = rtos_mutex_create(&msc_stack_lock);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create mutex fail\n");
		goto exit_usbd_msc_disk_init_fail;
	}
#endif

#ifdef CONFIG_USBD_MSC_SECOND_FLASH
	second_flash_spi_init();
	second_flash_get_id();
#endif

	ret = usbd_msc_disk_init();
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init disk fail\n");
		goto exit_usbd_msc_disk_init_fail;
	}

	ret = usbd_init(&msc_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBD fail\n");
		goto exit_usbd_init_fail;
	}

	ret = usbd_msc_init(&msc_cb, &msc_ep);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init MSC class fail\n");
		goto exit_usbd_msc_init_fail;
	}

#if MSC_USB_HOTPLUG
	ret = rtos_task_create(&usb_task, "usbd_msc_usb_hotplug_thread", example_usbd_msc_usb_hotplug_thread, NULL,
						   MSC_USB_HOTPLUG_THREAD_STACK_SIZE,
						   MSC_USB_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
		goto exit_create_hotplug_fail;
	}
#if defined(CONFIG_SMP)
	/* C-2: the USB OTG ISR is delivered on CPU0 (GIC ITARGETSR pins every SPI to core 0).
	   Pinning the threads that touch the device stack to CPU0 makes task<->ISR access
	   single-core, so deinit's local interrupt disable is meaningful again under SMP. */
	rtos_task_set_affinity(usb_task, 0);
#endif
#endif // MSC_USB_HOTPLUG

#if MSC_SD_HOTPLUG
	SD_SetCdCallback(sd_intr_cb);
	ret = rtos_task_create(&sd_task, "usbd_msc_sd_hotplug_thread", example_usbd_msc_sd_hotplug_thread, NULL,
						   MSC_SD_HOTPLUG_THREAD_STACK_SIZE,
						   MSC_SD_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create SD card hotplug thread fail\n");
		goto exit_create_msc_sd_hotplug_fail;
	}
#if defined(CONFIG_SMP)
	/* Same reason as the USB hotplug thread: this one deinits/reinits the device
	   stack too, so it must run on the core the OTG ISR is delivered on. */
	rtos_task_set_affinity(sd_task, 0);
#endif
#endif // MSC_SD_HOTPLUG

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD MSC demo start\n");

	rtos_task_delete(NULL);

	return;

#if MSC_SD_HOTPLUG
exit_create_msc_sd_hotplug_fail:
#if MSC_USB_HOTPLUG
	rtos_task_delete(usb_task);
#endif
#endif

#if MSC_USB_HOTPLUG
exit_create_hotplug_fail:
#endif
	usbd_msc_deinit();

exit_usbd_msc_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
	usbd_msc_disk_deinit();

exit_usbd_msc_disk_init_fail:
#if (MSC_SD_HOTPLUG == 1) || (MSC_USB_HOTPLUG == 1)
	msc_free_resource();
#endif

	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbd_msc(void)
{
	int ret;
	rtos_task_t task;

	ret = rtos_task_create(&task, "usbd_msc_thread", example_usbd_msc_thread, NULL,
						   MSC_INIT_THREAD_STACK_SIZE, MSC_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD MSC thread fail\n");
	}
}
