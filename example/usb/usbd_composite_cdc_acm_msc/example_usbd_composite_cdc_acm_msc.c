/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "usbd_cdc_acm.h"
#include "usbd_msc.h"
#include "usbd_composite.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

/* CDC ACM endpoint addresses */
#if defined (CONFIG_AMEBAGREEN2)
#define COMP_CDC_BULK_IN_EP                           0x82U
#define COMP_CDC_BULK_OUT_EP                          0x02U
#define COMP_CDC_INTR_IN_EP                           0x83U
#else
#define COMP_CDC_BULK_IN_EP                           0x81U
#define COMP_CDC_BULK_OUT_EP                          0x02U
#define COMP_CDC_INTR_IN_EP                           0x83U
#endif

/* MSC endpoint addresses */
#if defined (CONFIG_AMEBAGREEN2)
#define COMP_MSC_BULK_IN_EP                           0x84U
#define COMP_MSC_BULK_OUT_EP                          0x05U
#else
#define COMP_MSC_BULK_IN_EP                           0x85U
#define COMP_MSC_BULK_OUT_EP                          0x04U
#endif

#define CDC_BULK_IN_XFER_SIZE                         2048U
#define CDC_BULK_OUT_XFER_SIZE                        2048U

#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define COMP_USB_SPEED                                USB_SPEED_FULL
#else
#define COMP_USB_SPEED                                USB_SPEED_HIGH
#endif

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define COMP_HOTPLUG                                  1

// Hotplug thread
#define COMP_HOTPLUG_THREAD_PRIORITY                  8
#define COMP_HOTPLUG_THREAD_STACK_SIZE                1024U

/* Private function prototypes -----------------------------------------------*/

static int composite_cdc_acm_cb_init(void);
static int composite_cdc_acm_cb_deinit(void);
static int composite_cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int composite_cdc_acm_cb_received(u8 *buf, u32 len);

static int comp_init_stack(void);
#if COMP_HOTPLUG
static void composite_cb_status_changed(u8 old_status, u8 status);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "COMP";

static const usbd_config_t composite_cfg = {
	.speed = COMP_USB_SPEED,
	.isr_priority = INT_PRI_MIDDLE,
#if defined(CONFIG_AMEBASMART)
	.nptx_max_epmis_cnt = 100U,
#elif defined(CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 420U,
	.ptx_fifo_depth = {16U, 256U, 32U, 256U, },
#elif defined(CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1424U,
	.ptx_fifo_depth = {256U, 32U, 256U, },
#endif
};

/* CDC ACM endpoint configuration */
static const usbd_cdc_acm_ep_cfg_t cdc_acm_ep = {
	.bulk_in_addr  = COMP_CDC_BULK_IN_EP,
	.bulk_out_addr = COMP_CDC_BULK_OUT_EP,
	.intr_in_addr  = COMP_CDC_INTR_IN_EP,
	.bulk_in_xfer_size  = CDC_BULK_IN_XFER_SIZE,
	.bulk_out_xfer_size = CDC_BULK_OUT_XFER_SIZE,
};

/* MSC endpoint configuration */
static const usbd_msc_ep_cfg_t msc_ep = {
	.bulk_in_addr  = COMP_MSC_BULK_IN_EP,
	.bulk_out_addr = COMP_MSC_BULK_OUT_EP,
};

/* MSC user callbacks (status_changed is optional) */
static const usbd_msc_cb_t composite_msc_usr_cb = {
#if !defined(CONFIG_USBD_MSC_RAM_DISK)
	.status_changed  = NULL,
#endif
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
static rtos_task_t comp_hotplug_task;
static rtos_sema_t comp_attach_status_changed_sema;
static u8 comp_attach_status;

/* Composite-level callback: forwarded the aggregated attach status by the
   composite framework, used to drive the hotplug thread. */
static const usbd_composite_cb_t composite_usr_cb = {
	.status_changed = composite_cb_status_changed,
};
#define COMP_CB (&composite_usr_cb)
#else
#define COMP_CB NULL
#endif

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

	switch (req->bRequest) {
	case USB_CDC_ACM_SET_LINE_CODING:
		if (req->wLength == USB_CDC_ACM_LINE_CODING_SIZE) {
			lc->b.dwDteRate = (u32)(buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));
			lc->b.bCharFormat = buf[4];
			lc->b.bParityType = buf[5];
			lc->b.bDataBits = buf[6];
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
		break;

	case USB_CDC_ACM_SET_CONTROL_LINE_STATE:
		/*
		wValue:	wValue, Control Signal Bitmap
				D2-15:	Reserved, 0
				D1:	RTS, 0 - Deactivate, 1 - Activate
				D0:	DTR, 0 - Not Present, 1 - Present
		*/
		if (req->wValue & 0x01) {
			/* VCOM port activate */
			USB_DIAG(USB_LAYER_APP, USB_EVT_LINK, 0);
			usbd_cdc_acm_notify_serial_state(USB_CDC_ACM_CTRL_DSR | USB_CDC_ACM_CTRL_DCD);
		}
		break;

	case USB_CDC_ACM_SEND_BREAK:
		/* Do nothing */
		break;

	default:
		break;
	}

	return 0;
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
	if (buf && len) {
		usbd_cdc_acm_transmit(buf, len);
	}
	return 0;
}

/**
  * @brief  Bring up the whole composite stack (disk + core + each class + framework).
  * @note   Reused by both the initial start-up and the hotplug re-init path.
  *         The storage disk must be initialised before usbd_init.
  * @retval HAL_OK on success, other HAL_Status code on failure (all partial resources rolled back)
  */
static int comp_init_stack(void)
{
	int ret;

	/* Step 1: Initialize storage disk (must be done before usbd_init) */
	ret = usbd_msc_disk_init();
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "MSC disk init failed\r\n");
		return ret;
	}

	/* Step 2: Start USB device stack */
	ret = usbd_init(&composite_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "usbd_init failed\r\n");
		goto exit_disk_init;
	}

	/* Step 3: Initialize each class driver with its EP config */
	ret = usbd_composite_cdc_acm_init(&composite_cdc_acm_usr_cb, &cdc_acm_ep);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "CDC ACM init failed\r\n");
		goto exit_usbd_init;
	}

	ret = usbd_composite_msc_init(&composite_msc_usr_cb, &msc_ep);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "MSC init failed\r\n");
		goto exit_cdc_acm_init;
	}

	/* Step 4: Start composite framework (drivers already registered via composite_init calls above) */
	ret = usbd_composite_init(COMP_CB);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Composite init failed\r\n");
		goto exit_msc_init;
	}

	return HAL_OK;

exit_msc_init:
	usbd_msc_deinit();
exit_cdc_acm_init:
	usbd_cdc_acm_deinit();
exit_usbd_init:
	usbd_deinit();
exit_disk_init:
	usbd_msc_disk_deinit();
	return ret;
}

#if COMP_HOTPLUG
/**
  * @brief  Composite attach-status change notification (ISR context).
  * @note   time-consuming operations are not permitted here.
  */
static void composite_cb_status_changed(u8 old_status, u8 status)
{
	UNUSED(old_status);
	comp_attach_status = status;
	rtos_sema_give(comp_attach_status_changed_sema);
}

/* Tear down and re-init the whole composite stack on cable detach, to avoid
   memory leak across repeated plug/unplug. Deinit order is the reverse of
   init: framework -> classes -> core -> disk. */
static void example_usbd_composite_hotplug_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(comp_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (comp_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\r\n");
				usbd_composite_deinit();
				usbd_msc_deinit();
				usbd_cdc_acm_deinit();
				if (usbd_deinit() != HAL_OK) {
					break;
				}
				usbd_msc_disk_deinit();
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				if (comp_init_stack() != HAL_OK) {
					break;
				}
			} else if (comp_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\r\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\r\n");
			}
		}
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\r\n");
	rtos_task_delete(NULL);
}
#endif // COMP_HOTPLUG

/* Exported functions --------------------------------------------------------*/

void example_usbd_composite(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD COMP demo start\r\n");

#if COMP_HOTPLUG
	if (rtos_sema_create(&comp_attach_status_changed_sema, 0U, 1U) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema failed\r\n");
		return;
	}
#endif

	if (comp_init_stack() != HAL_OK) {
#if COMP_HOTPLUG
		rtos_sema_delete(comp_attach_status_changed_sema);
#endif
		return;
	}

#if COMP_HOTPLUG
	if (rtos_task_create(&comp_hotplug_task, "usbd_comp_hotplug_thread",
						 example_usbd_composite_hotplug_thread, NULL,
						 COMP_HOTPLUG_THREAD_STACK_SIZE, COMP_HOTPLUG_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\r\n");
	}
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD COMP demo ready\r\n");
}
