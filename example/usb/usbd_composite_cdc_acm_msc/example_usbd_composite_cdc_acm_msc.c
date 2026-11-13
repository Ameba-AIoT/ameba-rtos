/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "usbd_cdc_acm.h"
#include "usbd_msc.h"
#include "usbd_composite.h"
#include "os_wrapper.h"
#include "usb_app.h"
#include "example_usbd_composite_cdc_acm_msc.h"
#if defined(CONFIG_AMEBASMART)
#include "ameba_sd.h"
#endif
#ifdef CONFIG_USBD_MSC_SECOND_FLASH
#include "vfs_second_nor_flash.h"
#endif
#if defined(CONFIG_USBD_MSC_FLASH) || defined(CONFIG_USBD_MSC_RAM)
#include "vfs.h"
#define COMP_MSC_VFS_PREFIX  "usbmsc"
#endif
#ifdef CONFIG_USBD_MSC_FLASH
/* On-chip flash region for the MSC U-disk, overridable by editing this macro.
   Defaults to VFS2 because VFS1 is usually taken by the system LITTLEFS/KV. */
#define COMP_MSC_FLASH_VFS_TARGET VFS_TARGET_FLASH_VFS2
#endif

/* Private defines -----------------------------------------------------------*/

/* CDC ACM endpoint addresses */
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
#define COMP_CDC_BULK_IN_EP                           0x82U
#define COMP_CDC_BULK_OUT_EP                          0x02U
#define COMP_CDC_INTR_IN_EP                           0x83U
#else
#define COMP_CDC_BULK_IN_EP                           0x81U
#define COMP_CDC_BULK_OUT_EP                          0x02U
#define COMP_CDC_INTR_IN_EP                           0x83U
#endif

/* MSC endpoint addresses */
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
#define COMP_MSC_BULK_IN_EP                           0x84U
#define COMP_MSC_BULK_OUT_EP                          0x05U
#else
#define COMP_MSC_BULK_IN_EP                           0x85U
#define COMP_MSC_BULK_OUT_EP                          0x04U
#endif

#define CDC_BULK_IN_XFER_SIZE                         2048U
#define CDC_BULK_OUT_XFER_SIZE                        2048U

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define COMP_HOTPLUG                                  1
// Also tear down the stack on SD card removal, rebuild it on insertion.
#define MSC_SD_HOTPLUG                                0

#if !defined(CONFIG_USBD_MSC_SD_CARD) && (MSC_SD_HOTPLUG == 1)
#error "Only SD card (SD mode) support SD hotplug"
#endif

#if !defined(CONFIG_AMEBASMART) && (MSC_SD_HOTPLUG == 1)
#error "SD hotplug is not supported"
#endif

#if (COMP_HOTPLUG == 0) && (MSC_SD_HOTPLUG == 1)
#error "SD hotplug needs COMP_HOTPLUG"
#endif

// Thread priorities
#define COMP_INIT_THREAD_PRIORITY                     5
#define COMP_HOTPLUG_THREAD_PRIORITY                  8

// Thread stack sizes
#define COMP_INIT_THREAD_STACK_SIZE                   1400U
#define COMP_HOTPLUG_THREAD_STACK_SIZE                1024U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int composite_cdc_acm_cb_init(void);
static int composite_cdc_acm_cb_deinit(void);
static int composite_cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int composite_cdc_acm_cb_received(u8 *buf, u32 len);

static int composite_init_stack(void);
#if COMP_HOTPLUG
static void composite_cb_status_changed(u8 old_status, u8 status);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "COMP";

static const usbd_config_t composite_cfg = {
	.info = {
		.prod_str = "Realtek ACM+MSC Composite Device",
	},
	.isr_priority = INT_PRI_MIDDLE,
	/* Enlarge this value if composite configuration descriptor is larger than 512B */
	/* .ctrl_xfer_buf_len = 512U, */
#if defined(CONFIG_AMEBASMART)
	.nptx_max_epmis_cnt = 100U,
#elif defined(CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 436U,
	.ptx_fifo_depth = {0U, 256U, 32U, 256U, },
#elif defined(CONFIG_RLE1509)
	.rx_fifo_depth = 400U,
	.ptx_fifo_depth = {0U, 256U, 32U, 256U, },
#elif defined(CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1424U,
	.ptx_fifo_depth = {256U, 32U, 256U, },
#endif
};

/* CDC ACM endpoint configuration */
static const usbd_cdc_acm_config_t cdc_acm_class_cfg = {
	.bulk_in_addr  = COMP_CDC_BULK_IN_EP,
	.bulk_out_addr = COMP_CDC_BULK_OUT_EP,
	.intr_in_addr  = COMP_CDC_INTR_IN_EP,
	.bulk_in_xfer_size  = CDC_BULK_IN_XFER_SIZE,
	.bulk_out_xfer_size = CDC_BULK_OUT_XFER_SIZE,
	.notify_en = 1,
	/* Copy into the class TX buffer: the echo path transmits straight out of the BULK OUT
	   buffer, which is re-armed for the next packet before the IN transfer has completed */
	.bulk_in_zero_copy = 0,
};

/* MSC endpoint configuration */
static const usbd_msc_config_t msc_class_cfg = {
	.bulk_in_addr  = COMP_MSC_BULK_IN_EP,
	.bulk_out_addr = COMP_MSC_BULK_OUT_EP,
};

/* MSC user callbacks (status_changed is optional) */
static const usbd_msc_cb_t composite_msc_usr_cb = {
	.status_changed  = NULL,
};

/* CDC ACM user callbacks */
static const usbd_cdc_acm_cb_t composite_cdc_acm_usr_cb = {
	.init            = composite_cdc_acm_cb_init,
	.deinit          = composite_cdc_acm_cb_deinit,
	.setup           = composite_cdc_acm_cb_setup,
	.received        = composite_cdc_acm_cb_received,
	.transmitted     = NULL,
	.status_changed  = NULL,
	.wakeup          = NULL,
};


static usb_cdc_acm_line_coding_t composite_cdc_acm_line_coding;

#if COMP_HOTPLUG
static rtos_sema_t composite_attach_status_changed_sema;
/* Written by the ISR, read by the hotplug thread, possibly on another core */
static volatile u8 composite_attach_status;

/* Composite-level callback: forwarded the aggregated attach status by the
   composite framework, used to drive the hotplug thread. */
static const usbd_composite_cb_t composite_usr_cb = {
	.status_changed = composite_cb_status_changed,
};
#define COMP_CB (&composite_usr_cb)
#else
#define COMP_CB NULL
#endif

#if MSC_SD_HOTPLUG
/* Written by the card-detect callback, read by the hotplug thread, possibly on another core */
static volatile u8 composite_sd_status;
#endif

/* Whether composite_init_stack() is in effect. Written by the start entry and the hotplug
   thread, read by the teardown once the hotplug thread is gone. */
static u8 composite_stack_up;

/* Start/stop state: the hotplug thread (or, without hotplug, the start entry) owns the running
   stack, example_usbd_composite_cdc_acm_msc_stop() asks it to leave through quit and joins it */
static usb_app_ctx_t composite_app;

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CDC ACM media layer
  * @param  None
  * @retval Status
  */
static int composite_cdc_acm_cb_init(void)
{
	usb_cdc_acm_line_coding_t *lc = &composite_cdc_acm_line_coding;

	lc->b.dwDteRate = 150000;
	lc->b.bCharFormat = 0x00;
	lc->b.bParityType = 0x00;
	lc->b.bDataBits = 0x08;

	return 0;
}

/**
  * @brief  DeInitializes the CDC ACM media layer
  * @param  None
  * @retval Status
  */
static int composite_cdc_acm_cb_deinit(void)
{
	return 0;
}

/**
  * @brief  Handle the CDC class control (setup) requests for the composite device
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  req: USB setup request
  * @param  buf: Buffer containing the setup data payload
  * @retval Status
  */
static int composite_cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	usb_cdc_acm_line_coding_t *lc = &composite_cdc_acm_line_coding;
	/* Ref USB 2.0 9.2.7: anything not explicitly accepted below is a request error, so
	   the default status makes the core STALL EP0 instead of ACKing the status stage. */
	int ret = -RTK_ERR_BADARG;

	switch (req->bRequest) {
	case USB_CDC_ACM_SET_LINE_CODING:
		/* Ref CDC PSTN 1.2 Table 17: the Line Coding structure is exactly 7 bytes, any
		   other wLength must not update the cached line coding. */
		if (req->wLength == USB_CDC_ACM_LINE_CODING_SIZE) {
			lc->b.dwDteRate = (u32)buf[0] | ((u32)buf[1] << 8) | ((u32)buf[2] << 16) | ((u32)buf[3] << 24);
			lc->b.bCharFormat = buf[4];
			lc->b.bParityType = buf[5];
			lc->b.bDataBits = buf[6];
			ret = RTK_SUCCESS;
		}
		break;

	case USB_CDC_ACM_GET_LINE_CODING:
		buf[0] = (u8)(lc->b.dwDteRate & 0xFF);
		buf[1] = (u8)((lc->b.dwDteRate >> 8) & 0xFF);
		buf[2] = (u8)((lc->b.dwDteRate >> 16) & 0xFF);
		buf[3] = (u8)((lc->b.dwDteRate >> 24) & 0xFF);
		buf[4] = lc->b.bCharFormat;
		buf[5] = lc->b.bParityType;
		buf[6] = lc->b.bDataBits;
		ret = RTK_SUCCESS;
		break;

	case USB_CDC_ACM_SET_CONTROL_LINE_STATE:
		/*
		wValue:	wValue, Control Signal Bitmap
				D2-15:	Reserved, 0
				D1:	RTS, 0 - Deactivate, 1 - Activate
				D0:	DTR, 0 - Not Present, 1 - Present
		Ref CDC PSTN 1.2 6.3.12: no data stage, so wLength must be zero; the reserved bits
		are ignored rather than rejected.
		*/
		if (req->wLength == 0U) {
			if ((req->wValue & 0x01U) != 0U) {
				/* VCOM port activate */
				USB_DIAG(USB_LAYER_APP, USB_EVT_LINK, 0);
				usbd_cdc_acm_notify_serial_state(USB_CDC_ACM_CTRL_DSR | USB_CDC_ACM_CTRL_DCD);
			}
			ret = RTK_SUCCESS;
		}
		break;

	case USB_CDC_ACM_SEND_BREAK:
		/* Ref CDC PSTN 1.2 6.3.13: no data stage, so wLength must be zero. Nothing to do */
		if (req->wLength == 0U) {
			ret = RTK_SUCCESS;
		}
		break;

	default:
		/* Request error, keep the default status */
		break;
	}

	return ret;
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC ACM interface through this function.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  Buf: RX buffer
  * @param  Len: RX data length (in bytes)
  * @retval Status
  */
static int composite_cdc_acm_cb_received(u8 *buf, u32 len)
{
	/* Loopback; -RTK_ERR_BUSY means the previous echo is still in flight and this one is dropped */
	return usbd_cdc_acm_transmit(buf, len);
}

/**
  * @brief  Bring up the whole composite stack (disk + core + each class + framework).
  * @note   Reused by both the initial start-up and the hotplug re-init path.
  *         The storage disk must be initialised before usbd_init.
  * @retval RTK_SUCCESS on success, other HAL_Status code on failure (all partial resources rolled back)
  */
static int composite_init_stack(void)
{
	int ret;

	/* Step 1: Initialize storage disk (must be done before usbd_init) */
#ifdef CONFIG_USBD_MSC_SECOND_FLASH
#ifndef CONFIG_SECOND_FLASH_NOR
	second_flash_spi_init();
	second_flash_get_id();
#endif
#endif
#ifdef CONFIG_USBD_MSC_FLASH
	/* Mount FATFS on the selected on-chip flash region. */
	ret = vfs_target_register(COMP_MSC_VFS_PREFIX, COMP_MSC_FLASH_VFS_TARGET, VFS_RW);
	if (ret != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "VFS register FATFS/FLASH fail: %d\r\n", ret);
		return ret;
	}
#elif defined(CONFIG_USBD_MSC_RAM)
	/* Mount FATFS on the VFS RAM disk (volatile, formatted fresh on every register). */
	ret = vfs_target_register(COMP_MSC_VFS_PREFIX, VFS_TARGET_RAM, VFS_RW);
	if (ret != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "VFS register FATFS/RAM fail: %d\r\n", ret);
		return ret;
	}
#endif
	ret = usbd_msc_disk_init();
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "MSC disk init failed\r\n");
#ifdef CONFIG_USBD_MSC_FLASH
		vfs_target_unregister(COMP_MSC_VFS_PREFIX);
#elif defined(CONFIG_USBD_MSC_RAM)
		vfs_target_unregister(COMP_MSC_VFS_PREFIX);
#endif
		return ret;
	}

	/* Step 2: Start USB device stack */
	ret = usbd_init(&composite_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "usbd_init failed\r\n");
		goto exit_disk_init;
	}

	/* Step 3: Initialize each class driver with its EP config */
	ret = usbd_composite_cdc_acm_init(&composite_cdc_acm_usr_cb, &cdc_acm_class_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "CDC ACM init failed\r\n");
		goto exit_usbd_init;
	}

	ret = usbd_composite_msc_init(&composite_msc_usr_cb, &msc_class_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "MSC init failed\r\n");
		goto exit_cdc_acm_init;
	}

	/* Step 4: Start composite framework (drivers already registered via composite_init calls above) */
	ret = usbd_composite_init(COMP_CB);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Composite init failed\r\n");
		goto exit_msc_init;
	}

	return RTK_SUCCESS;

exit_msc_init:
	usbd_msc_deinit();
exit_cdc_acm_init:
	usbd_cdc_acm_deinit();
exit_usbd_init:
	usbd_deinit();
exit_disk_init:
	usbd_msc_disk_deinit();
#ifdef CONFIG_USBD_MSC_FLASH
	vfs_target_unregister(COMP_MSC_VFS_PREFIX);
#elif defined(CONFIG_USBD_MSC_RAM)
	vfs_target_unregister(COMP_MSC_VFS_PREFIX);
#endif
	return ret;
}

/**
  * @brief  Tear down the whole composite stack, in the reverse order of composite_init_stack():
  *         framework -> classes -> core -> disk.
  * @note   No return value: a teardown failure has no recoverable path, so the
  *         only sequence of release calls in this example lives here.
  * @retval None
  */
static void composite_deinit_stack(void)
{
	usbd_composite_deinit();
	usbd_msc_deinit();
	usbd_cdc_acm_deinit();
	usbd_deinit();
	usbd_msc_disk_deinit();
#ifdef CONFIG_USBD_MSC_FLASH
	vfs_target_unregister(COMP_MSC_VFS_PREFIX);
#elif defined(CONFIG_USBD_MSC_RAM)
	vfs_target_unregister(COMP_MSC_VFS_PREFIX);
#endif
}

/**
  * @brief  Release everything the example owns: the stack if still up and the objects
  * @note   Only called once the hotplug thread is gone, so that it is the only user left
  * @retval None
  */
static void composite_teardown(void)
{
	if (composite_stack_up != 0U) {
		composite_deinit_stack();
		composite_stack_up = 0U;
	}
#if COMP_HOTPLUG
#if MSC_SD_HOTPLUG
	/* Card detect stays enabled after SD_DeInit(), so unhook it before its sema goes away */
	SD_SetCdCallback(NULL);
#endif
	/* The stack is fully deinited, so no ISR can give the sema any more */
	rtos_sema_delete(composite_attach_status_changed_sema);
	composite_attach_status_changed_sema = NULL;
#endif
}

#if COMP_HOTPLUG
/**
  * @brief  Composite attach-status change notification (ISR context).
  * @note   time-consuming operations are not permitted here.
  */
static void composite_cb_status_changed(u8 old_status, u8 status)
{
	UNUSED(old_status);
	composite_attach_status = status;
	rtos_sema_give(composite_attach_status_changed_sema);
}

#if MSC_SD_HOTPLUG
/* SD card-detect notification, ISR context: no time-consuming calls here. */
static void composite_sd_cb_status_changed(SD_RESULT res)
{
	composite_sd_status = (u8)res;
	rtos_sema_give(composite_attach_status_changed_sema);
}
#endif

/* Tear down and re-init the whole composite stack on cable detach (avoids a leak
   across plug/unplug), and with MSC_SD_HOTPLUG also on SD card insert/removal. */
static void example_usbd_composite_hotplug_thread(void *param)
{
	u8 status;
#if MSC_SD_HOTPLUG
	/* usb_status is the attach status handled last, so a card-detect wake-up does
	   not replay it. composite_stack_up is 1 only while the card is present. */
	u8 sd_present;
	u8 usb_status = USBD_ATTACH_STATUS_INIT;
#endif

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(composite_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (composite_app.quit != 0U) {
				/* Stop requested: the stop owner tears the stack down */
				break;
			}
			status = composite_attach_status;
#if MSC_SD_HOTPLUG
			sd_present = (composite_sd_status != SD_NODISK) ? 1U : 0U;
			if (sd_present != composite_stack_up) {
				if (sd_present == 0U) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "SD card removed\n");
					composite_deinit_stack();
					composite_stack_up = 0U;
					RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
					/* deinit drops the core to DETACHED without a callback */
					usb_status = USBD_ATTACH_STATUS_DETACHED;
				} else {
					RTK_LOGS(TAG, RTK_LOG_INFO, "SD card insert, re-init USB\n");
					if (composite_init_stack() != RTK_SUCCESS) {
						/* rolled back; stay down and retry on the next insertion */
						continue;
					}
					composite_stack_up = 1U;
				}
				continue;
			}
			/* Skip a USB status queued before the teardown, or already handled */
			if ((composite_stack_up == 0U) || (status == usb_status)) {
				continue;
			}
			usb_status = status;
#endif
			if (status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\r\n");
				composite_deinit_stack();
				composite_stack_up = 0U;
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				if (composite_init_stack() != RTK_SUCCESS) {
					/* Rolled back: the stack is fully down */
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\r\n");
					if (usb_app_ctx_claim_stop(&composite_app) != 0U) {
						/* No stop is in flight: this thread finishes the teardown on its own */
						composite_teardown();
						usb_app_ctx_end(&composite_app);
						rtos_task_delete(NULL);
					}
					/* A stop is in flight and joins this thread */
					break;
				}
				composite_stack_up = 1U;
			} else if (status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\r\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\r\n");
			}
		}
	}

	usb_app_ctx_leave(&composite_app);
	rtos_task_delete(NULL);
}
#endif // COMP_HOTPLUG

/**
  * @brief  Bring the stack and the hotplug thread up
  * @retval RTK_SUCCESS, or the first error, with everything already created rolled back
  */
static int composite_start(void)
{
	int ret;
#if COMP_HOTPLUG
	rtos_task_t hotplug_task;
#endif

	ret = usb_app_ctx_begin(&composite_app);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

#if COMP_HOTPLUG
	ret = rtos_sema_create(&composite_attach_status_changed_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema failed\r\n");
		goto exit;
	}
#endif

	ret = composite_init_stack();
	if (ret != RTK_SUCCESS) {
		goto exit;
	}
	composite_stack_up = 1U;

#if COMP_HOTPLUG
#if MSC_SD_HOTPLUG
	SD_SetCdCallback(composite_sd_cb_status_changed);
#endif
	/* Created last: it owns the stack once running, so nothing after it can fail */
	ret = rtos_task_create(&hotplug_task, "usbd_composite_hotplug_thread",
						   example_usbd_composite_hotplug_thread, NULL,
						   COMP_HOTPLUG_THREAD_STACK_SIZE, COMP_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\r\n");
		goto exit;
	}
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* C-2: the USB OTG ISR is delivered on CPU0 (GIC ITARGETSR pins every SPI to
	   core 0). Pinning the hotplug thread to CPU0 makes its deinit/reinit
	   single-core against the ISR, so deinit's local interrupt disable is
	   meaningful under SMP. */
	rtos_task_set_affinity(hotplug_task, 0);
#endif
#endif

	return RTK_SUCCESS;

exit:
	/* No worker is running: deinits the stack only while composite_stack_up says it is up */
	composite_teardown();
	usb_app_ctx_end(&composite_app);
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBD ACM+MSC comp demo aborted\n");

	return ret;
}

static void example_usbd_composite_cdc_acm_msc_thread(void *param)
{
	UNUSED(param);

	if (composite_start() != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Start fail\n");
	}
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Start the ACM+MSC composite device, used by the USB app manager
  * @note   Task context only
  * @retval RTK_SUCCESS, -RTK_ERR_BUSY if already running, or the init error
  */
int example_usbd_composite_cdc_acm_msc_start(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD ACM+MSC comp demo start\n");

	return composite_start();
}

/**
  * @brief  Stop the ACM+MSC composite device and release everything it owns
  * @note   Task context only. Safe to call when not running.
  * @retval RTK_SUCCESS once idle, -RTK_ERR_TIMEOUT if the hotplug thread did not leave in time
  */
int example_usbd_composite_cdc_acm_msc_stop(void)
{
	u32 workers = 0U;

	if (usb_app_ctx_claim_stop(&composite_app) == 0U) {
		/* Idle, or the hotplug thread found the stack dead and finishes the teardown itself */
		return usb_app_ctx_wait_idle(&composite_app);
	}

#if COMP_HOTPLUG
	rtos_sema_give(composite_attach_status_changed_sema);
	workers++;
#endif
	if (usb_app_ctx_join(&composite_app, workers) != RTK_SUCCESS) {
		/* The hotplug thread is still on the stack: keep everything, the app stays STOPPING */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stop timeout\n");
		return -RTK_ERR_TIMEOUT;
	}

	/* The hotplug thread is gone: this caller is the only one left on the stack */
	composite_teardown();
	usb_app_ctx_end(&composite_app);
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD ACM+MSC comp demo stop\n");

	return RTK_SUCCESS;
}

/**
  * @brief  Example entry: start the ACM+MSC composite device from its own thread
  * @retval None
  */
void example_usbd_composite_cdc_acm_msc(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD ACM+MSC comp demo start\n");

	ret = rtos_task_create(&task, "usbd_composite_thread", example_usbd_composite_cdc_acm_msc_thread, NULL,
						   COMP_INIT_THREAD_STACK_SIZE, COMP_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD ACM+MSC comp thread fail\n");
	}
}
