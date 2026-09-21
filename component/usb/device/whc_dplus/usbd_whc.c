/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_whc.h"

/* Private defines -----------------------------------------------------------*/

#define USBD_WHC_IDX_INTERFACE_STR 0x04U

#define USBD_WHC_ITF_NUM           1U

#define USBD_WHC_EP_STATE_IDLE     0U
#define USBD_WHC_EP_STATE_BUSY     1U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_whc_set_config(usb_dev_t *dev, u8 config);
static void usbd_whc_clear_config(usb_dev_t *dev, u8 config);
static int usbd_whc_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_whc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
static int usbd_whc_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_whc_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_whc_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static void usbd_whc_status_changed(usb_dev_t *dev, u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "WHC";

static const u8 usbd_whc_wifi_only_mode_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,             // bLength
	USB_DESC_TYPE_DEVICE,         // bDescriptorType
	0x00,                         // bcdUSB
	0x02,
	0x00,                         // bDeviceClass
	0x00,                         // bDeviceSubClass
	0x00,                         // bDeviceProtocol
	USB_MAX_EP0_SIZE,             // bMaxPacketSize
	USB_LOW_BYTE(USBD_WHC_VID),  // idVendor
	USB_HIGH_BYTE(USBD_WHC_VID), // idVendor
	USB_LOW_BYTE(USBD_WHC_PID),  // idProduct
	USB_HIGH_BYTE(USBD_WHC_PID), // idProduct
	0x00,                         // bcdDevice
	0x00,
	USBD_IDX_MFC_STR,             // Index of manufacturer string
	USBD_IDX_PRODUCT_STR,         // Index of product string
	USBD_IDX_SERIAL_STR,          // Index of serial number string
	1                             // bNumConfigurations
}; // usbd_whc_wifi_only_mode_dev_desc

/* USB Standard Device Descriptor */
static const u8 usbd_whc_lang_id_desc[USB_LEN_LANGID_STR_DESC] = {
	USB_LEN_LANGID_STR_DESC,
	USB_DESC_TYPE_STRING,
	USB_LOW_BYTE(USBD_WHC_LANGID_STRING),
	USB_HIGH_BYTE(USBD_WHC_LANGID_STRING),
};

/* USB Full Speed Configuration Descriptor for WiFi-only mode */
static const u8 usbd_whc_wifi_only_mode_full_speed_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,						// bLength: Configuration Descriptor size
	USB_DESC_TYPE_CONFIGURATION,			// bDescriptorType: Configuration
	0x00,									// wTotalLength: number of returned bytes, runtime assigned
	0x00,
	0x01,									// bNumInterfaces
	0x01,									// bConfigurationValue
	0x00,									// iConfiguration
	0xA0,									// bmAttributes: decided by eFuse
	0xFA,									// MaxPower 500 mA

	/*---------------------------------------------------------------------------*/

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	0x00,									// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x03,									// bNumEndpoints
	0xFF,									// bInterfaceClass: Vendor Specific
	0x00,									// bInterfaceSubClass: WHC function family
	0x01,									// bInterfaceProtocol: WiFi function
	USBD_IDX_PRODUCT_STR,					// iInterface: USBD_WHC_PROD_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_IN,				// bEndpointAddress
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_OUT_1,				// bEndpointAddress
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_OUT_2,				// bEndpointAddress
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval

};

/* WHC Class Driver */
static const usbd_class_driver_t usbd_whc_driver = {
	.get_descriptor = usbd_whc_get_descriptor,
	.set_config = usbd_whc_set_config,
	.clear_config = usbd_whc_clear_config,
	.setup = usbd_whc_setup,
	.ep0_data_out = usbd_whc_handle_ep0_data_out,
	.ep_data_in = usbd_whc_handle_ep_data_in,
	.ep_data_out = usbd_whc_handle_ep_data_out,
	.status_changed = usbd_whc_status_changed,
};

/* WHC Device */
static usbd_whc_dev_t usbd_whc_dev;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Set class configuration for WiFi interface
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_whc_set_wifi_config(usb_dev_t *dev, u8 config)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_ep_t *ep;

	UNUSED(config);

	/* Init WiFi BULK IN */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN)].ep;
	usbd_ep_init(dev, ep);
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;

	/* Init WiFi BULK OUT 2 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2)].ep;
	usbd_ep_init(dev, ep);
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;

	/* Init WiFi BULK OUT 1 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_1)].ep;
	usbd_ep_init(dev, ep);
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;

	return HAL_OK;
}

/**
  * @brief  Set class configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_whc_set_config(usb_dev_t *dev, u8 config)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if (config != 1U) {
		return HAL_ERR_PARA;
	}

	idev->dev = dev;

	usbd_whc_set_wifi_config(dev, config);

	if ((idev->cb != NULL) && (idev->cb->set_config != NULL)) {
		idev->cb->set_config();
	}

	return HAL_OK;
}

/**
  * @brief  Clear class configuration for WiFi interface
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_whc_clear_wifi_config(usb_dev_t *dev, u8 config)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_ep_t *ep;

	UNUSED(config);

	/* DeInit WiFi BULK IN */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN)].ep;
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit WiFi BULK OUT 2 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2)].ep;
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit WiFi BULK OUT 1 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_1)].ep;
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	return HAL_OK;
}

/**
  * @brief  Clear class configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval None
  */
static void usbd_whc_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;

	UNUSED(config);

	usbd_whc_clear_wifi_config(dev, config);

	if ((idev->cb != NULL) && (idev->cb->clear_config != NULL)) {
		idev->cb->clear_config();
	}
}

/**
  * @brief  Handle WHC specific CTRL requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_whc_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	int ret = HAL_OK;
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			/*
			 * The WiFi interface owns the default alternate setting(0) only: reject any
			 * non-existent interface or alternate setting with a request error as per
			 * USB spec 9.4.10.
			 */
			if ((dev->dev_state != USBD_STATE_CONFIGURED) || (req->wIndex >= USBD_WHC_ITF_NUM) || (req->wValue != 0U)) {
				ret = HAL_ERR_HW;
			} else {
				/* Ref USB 2.0 9.4.10: the endpoints of the selected interface return to
				   their default state, not halted and data toggle DATA0. This holds even
				   for an interface with the default setting only, hosts do send the
				   request in that case. */
				usbd_ep_clear_stall(dev, &idev->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN)].ep);
				usbd_ep_clear_stall(dev, &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2)].ep);
				usbd_ep_clear_stall(dev, &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_1)].ep);
			}
			break;
		case USB_REQ_GET_INTERFACE:
			if ((dev->dev_state == USBD_STATE_CONFIGURED) && (req->wIndex < USBD_WHC_ITF_NUM)) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_len = 1U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		default:
			ret = HAL_ERR_HW;
			break;
		}
		break;
	case USB_REQ_TYPE_CLASS:
	case USB_REQ_TYPE_VENDOR:
		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
			if (req->wLength != 0U) {
				// SETUP + DATA IN + STATUS
				ret = HAL_ERR_PARA;
				if ((idev->cb != NULL) && (idev->cb->setup != NULL)) {
					/* Propagate the class callback status so an unsupported request is
					 * STALLed by the core instead of being ACKed */
					ret = idev->cb->setup(req, ep0_in->xfer_buf);
				}
				if (ret == HAL_OK) {
					/* Cap the IN data stage to the control buffer, a short response is legal
					 * as per USB 2.0 9.3.5 while an oversized xfer_len is rejected by the core
					 * and would leave the control transfer unanswered */
					ep0_in->xfer_len = (req->wLength < ep0_in->xfer_buf_len) ? req->wLength : (u16)ep0_in->xfer_buf_len;
					ret = usbd_ep_transmit(dev, ep0_in);
				}
			} else {
				// SETUP + ZLP IN + STATUS, invalid
				ret = HAL_ERR_PARA;
			}
		} else {
			if (req->wLength != 0U) {
				// SETUP + DATA OUT + STATUS, the DATA OUT phase is processed in ep0_data_out callback
				if (req->wLength > ep0_out->xfer_buf_len) {
					/* The OUT data stage cannot be buffered, the host sends wLength bytes
					 * and there is no way to shorten it: reject the request */
					ret = HAL_ERR_PARA;
				} else {
					usb_os_memcpy((void *)&idev->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
					idev->ctrl_req_pending = 1U;
					ep0_out->xfer_len = req->wLength;
					ret = usbd_ep_receive(dev, ep0_out);
					if (ret != HAL_OK) {
						/* EP0 OUT was not armed, so no data stage completion will ever arrive.
						 * Drop the pending request to keep the next transfer clean */
						idev->ctrl_req_pending = 0U;
					}
				}
			} else {
				// SETUP + STATUS
				ret = HAL_ERR_PARA;
				if ((idev->cb != NULL) && (idev->cb->setup != NULL)) {
					ret = idev->cb->setup(req, NULL);
				}
			}
		}
		break;
	default:
		ret = HAL_ERR_HW;
		break;
	}

	return ret;
}

/**
  * @brief  Handle EP0 Rx Ready event
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @retval Status
  */
static int usbd_whc_handle_ep0_data_out(usb_dev_t *dev)
{
	int ret = HAL_OK;
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	const usbd_whc_cb_t *cb = idev->cb;

	/* Only dispatch a data stage which belongs to a saved H2D request. A separate flag is
	 * needed because every bRequest code including 0xFF is a valid vendor request, so no
	 * value of ctrl_req can mark the slot as empty. Clear it first: a single data stage
	 * belongs to exactly one setup packet and must not be replayed by a later EP0 OUT event */
	if (idev->ctrl_req_pending != 0U) {
		idev->ctrl_req_pending = 0U;

		/* cb is released by usbd_whc_deinit(), which may run between the setup and the data
		 * stage of an H2D request, so both the structure and the handler are checked */
		if ((cb != NULL) && (cb->setup != NULL)) {
			ret = cb->setup(&idev->ctrl_req, dev->ep0_out.xfer_buf);
		}
	}

	return ret;
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_whc_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	const usbd_whc_cb_t *cb = idev->cb;
	u8 ep_num = USB_EP_NUM(ep_addr);
	usbd_whc_ep_t *in_ep;
	usbd_ep_t *ep;
	UNUSED(dev);

	/* USB_EP_NUM() keeps 7 bits of the address, so the index has to be range-checked before use */
	if (ep_num >= USB_MAX_ENDPOINTS) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
		return HAL_ERR_PARA;
	}

	in_ep = &idev->in_ep[ep_num];
	ep = &in_ep->ep;

	if (status != HAL_OK) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
	}

	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;

	if ((cb != NULL) && (cb->transmitted != NULL)) {
		cb->transmitted(in_ep, status);
	}

	return HAL_OK;
}

/**
  * @brief  Data received on non-control Out endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint number
  * @retval Status
  */
static int usbd_whc_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	const usbd_whc_cb_t *cb = idev->cb;
	u8 ep_num = USB_EP_NUM(ep_addr);
	usbd_whc_ep_t *out_ep;
	usbd_ep_t *ep;
	void *userdata;
	/* Evaluated once, so that the RX callback and the EP re-arm policy below always agree */
	u8 rx_cb_valid = (((cb != NULL) && (cb->received != NULL)) ? 1U : 0U);

	UNUSED(dev);

	/* USB_EP_NUM() keeps 7 bits of the address, so the index has to be range-checked before use */
	if (ep_num >= USB_MAX_ENDPOINTS) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
		return HAL_ERR_PARA;
	}

	out_ep = &idev->out_ep[ep_num];
	ep = &out_ep->ep;
	userdata = out_ep->userdata;

	if ((ep->skip_dcache_post_invalidate) && (ep->xfer_buf != NULL) && (len != 0)) {
		DCache_Invalidate((u32)ep->xfer_buf, len);
	}

	if ((len > 0) && (rx_cb_valid != 0U)) {
		cb->received(out_ep, len);
	}

	if ((len == 0) || (rx_cb_valid == 0U) ||
		((ep_addr != USBD_WHC_WIFI_EP_BULK_OUT_1) && (ep_addr != USBD_WHC_WIFI_EP_BULK_OUT_2))) {
		usbd_whc_receive_data(ep_addr, ep->xfer_buf, ep->xfer_len, userdata);
	}

	return HAL_OK;
}

/**
  * @brief  Get descriptor callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  req: Setup request handle
  * @param  buf: Poniter to Buffer
  * @retval Descriptor length
  */
static u16 usbd_whc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len)
{
	const u8 *desc = NULL;
	u16 len = 0;
	u8 type = USB_HIGH_BYTE(req->wValue);
	u8 is_cfg = 0;
	UNUSED(dev);

	switch (type) {

	case USB_DESC_TYPE_DEVICE:
		desc = usbd_whc_wifi_only_mode_dev_desc;
		len = USB_LEN_DEV_DESC;
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		desc = usbd_whc_wifi_only_mode_full_speed_config_desc;
		len = sizeof(usbd_whc_wifi_only_mode_full_speed_config_desc);
		is_cfg = 1;
		break;

	case USB_DESC_TYPE_STRING:
		switch (USB_LOW_BYTE(req->wValue)) {
		case USBD_IDX_LANGID_STR:
			desc = usbd_whc_lang_id_desc;
			len = USB_LEN_LANGID_STR_DESC;
			break;
		case USBD_IDX_MFC_STR:
			len = usbd_get_str_descriptor(USBD_WHC_MFG_STRING, buf, buf_len);
			break;
		case USBD_IDX_PRODUCT_STR:
			len = usbd_get_str_descriptor(USBD_WHC_PROD_STRING, buf, buf_len);
			break;
		case USBD_IDX_SERIAL_STR:
			len = usbd_get_str_descriptor(USBD_WHC_SN_STRING, buf, buf_len);
			break;
		case USBD_IDX_MS_OS_STR:
			/*Not support*/
			break;
		default:
			USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_GET_DESC, 0);
			break;
		}
		break;

	default:
		break;
	}

	if (desc != NULL) {
		/* Truncation is not allowed: a short descriptor is illegal, so stall instead */
		if (len > buf_len) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Desc %d OVSZ %d > %d\n", type, len, buf_len);
			return 0;
		}

		usb_os_memcpy((void *)buf, (const void *)desc, len);
	}

	if (is_cfg != 0) {
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
	}

	return len;

}

static int usbd_whc_wifi_init(void)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_ep_t *ep;
	usb_ep_info_t *info;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN);
	ep = &idev->in_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_WIFI_EP_BULK_IN;
	info->mps = USBD_WHC_FS_BULK_MPS;
	info->type = USB_CH_EP_TYPE_BULK;
	ep->skip_dcache_pre_clean = 0;
	ep->skip_dcache_post_invalidate = 0;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2);
	ep = &idev->out_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_WIFI_EP_BULK_OUT_2;
	info->mps = USBD_WHC_FS_BULK_MPS;
	info->type = USB_CH_EP_TYPE_BULK;
	ep->skip_dcache_pre_clean = 0;
	ep->skip_dcache_post_invalidate = 0;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_1);
	ep = &idev->out_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_WIFI_EP_BULK_OUT_1;
	info->mps = USBD_WHC_FS_BULK_MPS;
	info->type = USB_CH_EP_TYPE_BULK;
	ep->skip_dcache_pre_clean = 0;
	ep->skip_dcache_post_invalidate = 0;

	return HAL_OK;
}

/**
  * @brief  USB attach status change
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  old_status: USB old attach status
  * @param  status: USB USB attach status
  * @retval void
  */
static void usbd_whc_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;

	UNUSED(dev);

	if ((idev->cb != NULL) && (idev->cb->status_changed != NULL)) {
		idev->cb->status_changed(old_status, status);
	}
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize whc device
  * @retval Status
  */
int usbd_whc_init(const usbd_whc_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_whc_dev_t *idev = &usbd_whc_dev;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	usbd_whc_wifi_init();

	/* No H2D class/vendor request is waiting for its data stage yet */
	idev->ctrl_req_pending = 0U;

	idev->cb = cb;
	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			goto init_exit;
		}
	}

	usbd_register_class(&usbd_whc_driver);

	return ret;

init_exit:
	return ret;
}

/**
  * @brief  DeInitialize whc device
  * @param  void
  * @retval Status
  */
int usbd_whc_deinit(void)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;

	/* Unregister first: no class callback can run afterwards, so releasing cb and the pending
	 * control request below cannot race an EP0 OUT completion in ISR context */
	usbd_unregister_class();

	idev->ctrl_req_pending = 0U;

	if (idev->cb != NULL) {
		if (idev->cb->deinit != NULL) {
			idev->cb->deinit();
		}
		idev->cb = NULL;
	}

	return HAL_OK;
}

int usbd_whc_transmit_ctrl_data(u8 *buf, u16 len)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usb_dev_t *dev = idev->dev;
	usbd_ep_t *ep0_in;

	/* idev->dev is set on SET_CONFIGURATION only, it stays NULL until enumeration completes */
	if ((dev == NULL) || (dev->is_ready == 0U)) {
		return HAL_ERR_HW;
	}

	ep0_in = &dev->ep0_in;
	if (len > ep0_in->xfer_buf_len) {
		len = ep0_in->xfer_buf_len;
	}
	usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)buf, len);
	ep0_in->xfer_len = len;
	return usbd_ep_transmit(dev, ep0_in);
}

int usbd_whc_transmit_data(u8 ep_addr, u8 *buf, u32 len, void *userdata)
{
	int ret = HAL_OK;
	u8 num = USB_EP_NUM(ep_addr);
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usb_dev_t *dev = idev->dev;
	usbd_ep_t *ep;

	if (USB_EP_IS_OUT(ep_addr) || (num >= USB_MAX_ENDPOINTS)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP 0x%02x\n", ep_addr);
		return HAL_ERR_PARA;
	}

	/* idev->dev is set on SET_CONFIGURATION only, it stays NULL until enumeration completes */
	if ((dev == NULL) || (dev->is_ready == 0U)) {
		return HAL_ERR_HW;
	}

	ep = &idev->in_ep[num].ep;

	if (ep->xfer_state == USBD_WHC_EP_STATE_IDLE) {
		ep->xfer_state = USBD_WHC_EP_STATE_BUSY;
		idev->in_ep[num].userdata = userdata;
		ep->xfer_buf = buf; /*Application should free this txbuf only afer TX DONE*/
		ep->xfer_len = len;
		if ((ep->skip_dcache_pre_clean) && (buf != NULL) && (len != 0)) {
			if (USB_IS_MEM_DMA_ALIGNED(buf)) {
				DCache_Clean((u32)buf, len);
			} else {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "EP TX buf align err\n");
				return HAL_ERR_MEM;
			}
		}
		ret = usbd_ep_transmit(dev, ep);
	} else {
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x TX BUSY\n", ep_addr);
		ret = HAL_BUSY;
	}

	return ret;
}

int usbd_whc_receive_data(u8 ep_addr, u8 *buf, u32 len, void *userdata)
{
	u8 num = USB_EP_NUM(ep_addr);
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usb_dev_t *dev = idev->dev;
	usbd_ep_t *ep;

	if (USB_EP_IS_IN(ep_addr) || (num >= USB_MAX_ENDPOINTS)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid OUT EP num: 0x%02x\n", ep_addr);
		return HAL_ERR_PARA;
	}

	/* idev->dev is set on SET_CONFIGURATION only, it stays NULL until enumeration completes */
	if ((dev == NULL) || (dev->is_ready == 0U)) {
		/*RX not ready*/
		return HAL_ERR_HW;
	}

	ep = &idev->out_ep[num].ep;

	ep->xfer_buf = buf;
	ep->xfer_len = len;
	idev->out_ep[num].userdata = userdata;
	if ((ep->skip_dcache_pre_clean) && (buf != NULL) && (len != 0)) {
		if (USB_IS_MEM_DMA_ALIGNED(buf)) {
			/* Clean the whole DMA window rather than the requested length, so that no dirty
			 * line inside the window can be written back over the received data. */
			DCache_Clean((u32)buf, usb_get_dma_len(len, ep->info.mps));
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "EP RX buf align err\n");
			return HAL_ERR_MEM;
		}
	}
	return usbd_ep_receive(dev, ep);
}
