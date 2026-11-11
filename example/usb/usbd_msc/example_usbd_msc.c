/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "usbd_msc.h"
#include "os_wrapper.h"
#include "usb_app.h"
#include "example_usbd_msc.h"
#if defined(CONFIG_AMEBASMART)
#include "ameba_sd.h"
#endif
#ifdef CONFIG_USBD_MSC_SECOND_FLASH
#include "vfs_second_nor_flash.h"
#endif
#if defined(CONFIG_USBD_MSC_FLASH) || defined(CONFIG_USBD_MSC_RAM)
#include "vfs.h"
#define USBD_MSC_VFS_PREFIX  "usbmsc"
#endif
#ifdef CONFIG_USBD_MSC_FLASH
/* On-chip flash region for the MSC U-disk, overridable by editing this macro.
   Defaults to VFS2 because VFS1 is usually taken by the system LITTLEFS/KV. */
#define USBD_MSC_FLASH_VFS_TARGET VFS_TARGET_FLASH_VFS2
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

#if !defined(CONFIG_USBD_MSC_SD_CARD) && (MSC_SD_HOTPLUG == 1)
#error "Only SD card (SD mode) support SD hotplug"
#endif

#if !defined(CONFIG_AMEBASMART) && (MSC_SD_HOTPLUG == 1)
#error "SD hotplug is not supported"
#endif

// Thread priorities
#define MSC_INIT_THREAD_PRIORITY                  5
#define MSC_USB_HOTPLUG_THREAD_PRIORITY           8
#define MSC_SD_HOTPLUG_THREAD_PRIORITY            8

// Thread stack sizes
#define MSC_INIT_THREAD_STACK_SIZE                1400U
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
	.info = {
		.prod_str = "Realtek MSC Device",
	},
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

static const usbd_msc_config_t msc_class_cfg = {
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
static volatile SD_RESULT msc_sd_status;
static rtos_sema_t msc_sd_status_changed_sema;
#endif

#if (MSC_SD_HOTPLUG == 1) || (MSC_USB_HOTPLUG == 1)
/* Read from ISR/card-detect callback context, so it must not be cached. */
static volatile usbd_msc_hotplug_type_t msc_hotplug_ongoing_type;
#endif

#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
/* Serializes the two hotplug threads' deinit/reinit of the device stack. Each
   thread publishes msc_hotplug_ongoing_type (volatile) BEFORE taking the lock so
   the ISR/card-detect callback stops feeding the peer's semaphore while the stack
   is being rebuilt; the notification that slipped through earlier is drained by
   the peer once the lock is released. */
static rtos_mutex_t msc_stack_lock;
#endif

/* Whether usbd_init() and usbd_msc_init() are in effect, and whether the disk still needs a
   usbd_msc_disk_deinit(). Written by the start entry and the hotplug threads, read by the
   teardown once they are all gone. */
static u8 msc_stack_up;
static u8 msc_disk_up;

/* Start/stop state: the hotplug threads own the running stack, example_usbd_msc_stop() asks
   them to leave through quit and joins them */
static usb_app_ctx_t msc_app;

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Free the objects shared by the example threads
  * @note   Only called once every hotplug thread is gone and the USB stack is fully
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
  * @brief  Release everything the example owns: the stack and the disk if still up, the VFS volume and the objects
  * @note   Only called once every hotplug thread is gone
  * @retval None
  */
static void msc_teardown(void)
{
	if (msc_stack_up != 0U) {
		usbd_msc_deinit();
		usbd_deinit();
		msc_stack_up = 0U;
	}
	if (msc_disk_up != 0U) {
		usbd_msc_disk_deinit();
		msc_disk_up = 0U;
	}

#ifdef CONFIG_USBD_MSC_FLASH
	vfs_target_unregister(USBD_MSC_VFS_PREFIX);
#elif defined(CONFIG_USBD_MSC_RAM)
	vfs_target_unregister(USBD_MSC_VFS_PREFIX);
#endif

	msc_free_resource();
}

#if (MSC_SD_HOTPLUG == 1) || (MSC_USB_HOTPLUG == 1)
/**
  * @brief  Finish the teardown claimed by a hotplug thread that found the stack dead
  * @note   The stack is already down and the caller holds no lock. Does not return.
  * @retval None
  */
static void msc_hotplug_fatal_teardown(void)
{
#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
	/* Wake the peer thread, it observes quit and leaves */
	rtos_sema_give(msc_usb_status_changed_sema);
	rtos_sema_give(msc_sd_status_changed_sema);
	if (usb_app_ctx_join(&msc_app, 1U) != RTK_SUCCESS) {
		/* The peer may still touch the shared objects: leak them */
		rtos_task_delete(NULL);
	}
#endif
	msc_teardown();
	usb_app_ctx_end(&msc_app);
	rtos_task_delete(NULL);
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
	u8 fatal = 0U;
#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
	u8 lock_held = 0U;
#endif

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(msc_usb_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (msc_app.quit != 0U) {
				/* Stop requested, or the peer found the stack dead: the stop owner tears it down */
				break;
			}
			if (msc_usb_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				/* Publish before taking the lock so the peer's notifications are
				   gated even if it currently owns the stack. */
				msc_hotplug_ongoing_type = USBD_MSC_USB_HOTPLUG;
				MSC_STACK_LOCK();
#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
				if (msc_app.quit != 0U) {
					/* Stop requested while the peer owned the stack: leave it as it is */
					break;
				}
#endif
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");
				usbd_msc_deinit();
				usbd_deinit();
				msc_stack_up = 0U;
				usbd_msc_disk_deinit();
				msc_disk_up = 0U;
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				ret = usbd_msc_disk_init();
				if (ret != RTK_SUCCESS) {
					/* No medium to expose: leave the stack down rather than enumerate a disk every access fails */
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Init disk fail %d\n", ret);
					fatal = 1U;
					break;
				}
				msc_disk_up = 1U;
				ret = usbd_init(&msc_cfg);
				if (ret != 0) {
					fatal = 1U;
					break;
				}
				ret = usbd_msc_init(&msc_cb, &msc_class_cfg);
				if (ret != 0) {
					usbd_deinit();
					fatal = 1U;
					break;
				}
				msc_stack_up = 1U;
				msc_hotplug_ongoing_type = USBD_MSC_HOTPLUG_NONE;
				MSC_STACK_UNLOCK();
			} else if (msc_usb_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}

	if (fatal != 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\n");
		/* Claimed before the lock is released, so the peer sees quit as soon as it gets the lock */
		fatal = usb_app_ctx_claim_stop(&msc_app);
	}
#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
	if (lock_held != 0U) {
		MSC_STACK_UNLOCK();
	}
#endif
	if (fatal != 0U) {
		/* No stop is in flight: this thread finishes the teardown on its own */
		msc_hotplug_fatal_teardown();
	}
	usb_app_ctx_leave(&msc_app);
	rtos_task_delete(NULL);
}
#endif // MSC_USB_HOTPLUG

#if MSC_SD_HOTPLUG
static void example_usbd_msc_sd_hotplug_thread(void *param)
{
	int ret = 0;
	u8 fatal = 0U;
#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
	u8 lock_held = 0U;
#endif

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(msc_sd_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (msc_app.quit != 0U) {
				/* Stop requested, or the peer found the stack dead: the stop owner tears it down */
				break;
			}
			/* Publish before taking the lock so the peer's notifications are
			   gated even if it currently owns the stack. The lock is held from
			   card removal until the stack is rebuilt on the next insertion. */
			msc_hotplug_ongoing_type = USBD_MSC_SD_HOTPLUG;
#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
			if (lock_held == 0U) {
				MSC_STACK_LOCK();
				if (msc_app.quit != 0U) {
					/* Stop requested while the peer owned the stack: leave it as it is */
					break;
				}
			}
#endif
			if (msc_sd_status == SD_NODISK) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "SD card removed\n");
				usbd_msc_deinit();
				usbd_deinit();
				msc_stack_up = 0U;
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "SD card insert, re-init USB\n");
				SD_CardInit();
				ret = usbd_init(&msc_cfg);
				if (ret != 0) {
					fatal = 1U;
					break;
				}
				ret = usbd_msc_init(&msc_cb, &msc_class_cfg);
				if (ret != 0) {
					usbd_deinit();
					fatal = 1U;
					break;
				}
				msc_stack_up = 1U;
				msc_hotplug_ongoing_type = USBD_MSC_HOTPLUG_NONE;
				MSC_STACK_UNLOCK();
			}
		}
	}

	/* Same exit sequence as the USB hotplug thread */
	if (fatal != 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "SD card hotplug thread fail\n");
		fatal = usb_app_ctx_claim_stop(&msc_app);
	}
#if (MSC_SD_HOTPLUG == 1) && (MSC_USB_HOTPLUG == 1)
	if (lock_held != 0U) {
		MSC_STACK_UNLOCK();
	}
#endif
	if (fatal != 0U) {
		msc_hotplug_fatal_teardown();
	}
	usb_app_ctx_leave(&msc_app);
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

/**
  * @brief  Bring the disk, the stack and the hotplug threads up
  * @retval RTK_SUCCESS, or the first error, with everything already created rolled back
  */
static int msc_start(void)
{
	int ret;
#if MSC_USB_HOTPLUG
	rtos_task_t usb_task;
#endif
#if MSC_SD_HOTPLUG
	rtos_task_t sd_task;
#endif

	ret = usb_app_ctx_begin(&msc_app);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

#if (MSC_SD_HOTPLUG == 1) || (MSC_USB_HOTPLUG == 1)
	msc_hotplug_ongoing_type = USBD_MSC_HOTPLUG_NONE;
#endif

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
#ifndef CONFIG_SECOND_FLASH_NOR
	second_flash_spi_init();
	second_flash_get_id();
#endif
#endif

#ifdef CONFIG_USBD_MSC_FLASH
	/* Mount FATFS on the selected on-chip flash region. */
	ret = vfs_target_register(USBD_MSC_VFS_PREFIX, USBD_MSC_FLASH_VFS_TARGET, VFS_RW);
	if (ret != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "VFS register FATFS/FLASH fail: %d\n", ret);
		goto exit_usbd_msc_disk_init_fail;
	}
#elif defined(CONFIG_USBD_MSC_RAM)
	/* Mount FATFS on the VFS RAM disk (volatile, formatted fresh on every register). */
	ret = vfs_target_register(USBD_MSC_VFS_PREFIX, VFS_TARGET_RAM, VFS_RW);
	if (ret != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "VFS register FATFS/RAM fail: %d\n", ret);
		goto exit_usbd_msc_disk_init_fail;
	}
#endif

	/* Set before the init: a failed one is unwound by usbd_msc_disk_deinit() too */
	msc_disk_up = 1U;
	ret = usbd_msc_disk_init();
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init disk fail\n");
		/* VFS FATFS volume is already registered above; unwind it too. */
		goto exit_usbd_init_fail;
	}

	ret = usbd_init(&msc_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBD fail\n");
		goto exit_usbd_init_fail;
	}

	ret = usbd_msc_init(&msc_cb, &msc_class_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init MSC class fail\n");
		usbd_deinit();
		goto exit_usbd_init_fail;
	}
	msc_stack_up = 1U;

	/* The hotplug threads are created last: they own the stack once running */
#if MSC_USB_HOTPLUG
	ret = rtos_task_create(&usb_task, "usbd_msc_usb_hotplug_thread", example_usbd_msc_usb_hotplug_thread, NULL,
						   MSC_USB_HOTPLUG_THREAD_STACK_SIZE,
						   MSC_USB_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
		goto exit_usbd_init_fail;
	}
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
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
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* Same reason as the USB hotplug thread: this one deinits/reinits the device
	   stack too, so it must run on the core the OTG ISR is delivered on. */
	rtos_task_set_affinity(sd_task, 0);
#endif
#endif // MSC_SD_HOTPLUG

	return RTK_SUCCESS;

#if MSC_SD_HOTPLUG
exit_create_msc_sd_hotplug_fail:
#if MSC_USB_HOTPLUG
	if (usb_app_ctx_claim_stop(&msc_app) == 0U) {
		/* The USB hotplug thread found the stack dead and owns the teardown: it joins its
		   peer, so stand in for the SD hotplug thread that never ran */
		usb_app_ctx_leave(&msc_app);
		return ret;
	}
	/* Wake and join the USB hotplug thread before the stack goes away */
	rtos_sema_give(msc_usb_status_changed_sema);
	if (usb_app_ctx_join(&msc_app, 1U) != RTK_SUCCESS) {
		/* It may still run on the stack: keep it, the app stays STOPPING */
		return -RTK_ERR_TIMEOUT;
	}
#endif
#endif

exit_usbd_init_fail:
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBD MSC demo aborted\n");
	/* Deinits the stack only while msc_stack_up says it is up: a joined hotplug thread may have left it down */
	msc_teardown();
	usb_app_ctx_end(&msc_app);

	return ret;

exit_usbd_msc_disk_init_fail:
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBD MSC demo aborted\n");
	msc_free_resource();
	usb_app_ctx_end(&msc_app);

	return ret;
}

static void example_usbd_msc_thread(void *param)
{
	UNUSED(param);

	if (msc_start() != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Start fail\n");
	}
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Start the MSC device, used by the USB app manager
  * @note   Task context only
  * @retval RTK_SUCCESS, -RTK_ERR_BUSY if already running, or the init error
  */
int example_usbd_msc_start(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD MSC demo start\n");

	return msc_start();
}

/**
  * @brief  Stop the MSC device and release everything it owns
  * @note   Task context only. Safe to call when not running.
  * @retval RTK_SUCCESS once idle, -RTK_ERR_TIMEOUT if a hotplug thread did not leave in time
  */
int example_usbd_msc_stop(void)
{
	u32 workers = 0U;

	if (usb_app_ctx_claim_stop(&msc_app) == 0U) {
		/* Idle, or a hotplug thread found the stack dead and finishes the teardown itself */
		return usb_app_ctx_wait_idle(&msc_app);
	}

#if MSC_USB_HOTPLUG
	rtos_sema_give(msc_usb_status_changed_sema);
	workers++;
#endif
#if MSC_SD_HOTPLUG
	rtos_sema_give(msc_sd_status_changed_sema);
	workers++;
#endif
	if (usb_app_ctx_join(&msc_app, workers) != RTK_SUCCESS) {
		/* A hotplug thread is still on the stack: keep everything, the app stays STOPPING */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stop timeout\n");
		return -RTK_ERR_TIMEOUT;
	}

	/* Every hotplug thread is gone: this caller is the only one left on the stack */
	msc_teardown();
	usb_app_ctx_end(&msc_app);
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD MSC demo stop\n");

	return RTK_SUCCESS;
}

/**
  * @brief  Example entry: start the MSC device from its own thread
  * @retval None
  */
void example_usbd_msc(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD MSC demo start\n");

	ret = rtos_task_create(&task, "usbd_msc_thread", example_usbd_msc_thread, NULL,
						   MSC_INIT_THREAD_STACK_SIZE, MSC_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD MSC thread fail\n");
	}
}
