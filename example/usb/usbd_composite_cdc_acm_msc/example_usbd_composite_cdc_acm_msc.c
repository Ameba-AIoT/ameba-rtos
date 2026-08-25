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

/* Private function prototypes -----------------------------------------------*/

static int composite_cdc_acm_cb_init(void);
static int composite_cdc_acm_cb_deinit(void);
static int composite_cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int composite_cdc_acm_cb_received(u8 *buf, u32 len);

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

/* Exported functions --------------------------------------------------------*/

void example_usbd_composite(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD COMP demo start\r\n");

	/* Step 1: Initialize storage disk (must be done before usbd_init) */
	if (usbd_msc_disk_init() != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "MSC disk init failed\r\n");
		return;
	}

	/* Step 2: Start USB device stack */
	if (usbd_init(&composite_cfg) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "usbd_init failed\r\n");
		goto exit_disk_init;
	}

	/* Step 3: Initialize each class driver with its EP config */
	if (usbd_composite_cdc_acm_init(&composite_cdc_acm_usr_cb, &cdc_acm_ep) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "CDC ACM init failed\r\n");
		goto exit_usbd_init;
	}

	if (usbd_composite_msc_init(&composite_msc_usr_cb, &msc_ep) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "MSC init failed\r\n");
		goto exit_cdc_acm_init;
	}

	/* Step 4: Start composite framework (drivers already registered via composite_init calls above) */
	if (usbd_composite_init(NULL) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Composite init failed\r\n");
		goto exit_msc_init;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD COMP demo ready\r\n");
	return;

exit_msc_init:
	usbd_msc_deinit();
exit_cdc_acm_init:
	usbd_cdc_acm_deinit();
exit_usbd_init:
	usbd_deinit();
exit_disk_init:
	usbd_msc_disk_deinit();
}
