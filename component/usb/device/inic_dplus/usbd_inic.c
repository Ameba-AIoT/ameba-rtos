/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_inic.h"
#include "usbd.h"

/* Private defines -----------------------------------------------------------*/

#define USBD_INIC_MFG_STRING	"Realtek"
#define USBD_INIC_PROD_STRING	"802.11n  WLAN Adapter"
#define USBD_INIC_SN_STRING		"123456789ABC"

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_inic_set_config(usb_dev_t *dev, u8 config);
static int usbd_inic_clear_config(usb_dev_t *dev, u8 config);
static int usbd_inic_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_inic_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int usbd_inic_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_inic_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_inic_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static void usbd_inic_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
static int usbd_inic_suspend(usb_dev_t *dev);
static int usbd_inic_resume(usb_dev_t *dev);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "INIC";

static const u8 usbd_inic_wifi_only_mode_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,             // bLength
	USB_DESC_TYPE_DEVICE,         // bDescriptorType
	0x00,                         // bcdUSB
	0x02,
	0x00,                         // bDeviceClass
	0x00,                         // bDeviceSubClass
	0x00,                         // bDeviceProtocol
	USB_MAX_EP0_SIZE,             // bMaxPacketSize
	USB_LOW_BYTE(USBD_INIC_VID),  // idVendor
	USB_HIGH_BYTE(USBD_INIC_VID), // idVendor
	USB_LOW_BYTE(USBD_INIC_PID),  // idProduct
	USB_HIGH_BYTE(USBD_INIC_PID), // idProduct
	0x00,                         // bcdDevice
	0x00,
	USBD_IDX_MFC_STR,             // Index of manufacturer string
	USBD_IDX_PRODUCT_STR,         // Index of product string
	USBD_IDX_SERIAL_STR,          // Index of serial number string
	1                             // bNumConfigurations
}; // usbd_inic_wifi_only_mode_dev_desc

/* USB Standard Device Descriptor */
static const u8 usbd_inic_lang_id_desc[USB_LEN_LANGID_STR_DESC] = {
	USB_LEN_LANGID_STR_DESC,
	USB_DESC_TYPE_STRING,
	USB_LOW_BYTE(USBD_INIC_LANGID_STRING),
	USB_HIGH_BYTE(USBD_INIC_LANGID_STRING),
};

/* USB Full Speed Configuration Descriptor for WiFi-only mode */
static const u8 usbd_inic_wifi_only_mode_full_speed_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,						// bLength: Configuration Descriptor size
	USB_DESC_TYPE_CONFIGURATION,			// bDescriptorType: Configuration
	0x00,									// wTotalLength: number of returned bytes, runtime assigned
	0x00,
	0x01,									// bNumInterfaces: 1 interface
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
	0x03,									// bNumEndpoints: 4 endpoints
	0xFF,									// bInterfaceClass: Vendor Specific
	0xFF,									// bInterfaceSubClass
	0xFF,									// bInterfaceProtocol
	USBD_IDX_PRODUCT_STR,					// iInterface: USBD_INIC_PROD_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	0x83,									// bEndpointAddress: EP3 IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	0x02,									// bEndpointAddress: EP2 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	0x04,									// bEndpointAddress: EP4 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval

};

/* INIC Class Driver */
static const usbd_class_driver_t usbd_inic_driver = {
	.get_descriptor = usbd_inic_get_descriptor,
	.set_config = usbd_inic_set_config,
	.clear_config = usbd_inic_clear_config,
	.setup = usbd_inic_setup,
	.ep0_data_out = usbd_inic_handle_ep0_data_out,
	.ep_data_in = usbd_inic_handle_ep_data_in,
	.ep_data_out = usbd_inic_handle_ep_data_out,
	.status_changed = usbd_inic_status_changed,
	.suspend = usbd_inic_suspend,
	.resume = usbd_inic_resume,
};

/* INIC Device */
static usbd_inic_dev_t usbd_inic_dev;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Set class configuration for WiFi interface
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_inic_set_wifi_config(usb_dev_t *dev, u8 config)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_ep_t *ep;

	UNUSED(config);

	/* Init BULK IN EP3 */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP3_BULK_IN)].ep;
	usbd_ep_init(dev, ep);
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;

	/* Init BULK OUT EP4 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP4_BULK_OUT)].ep;
	usbd_ep_init(dev, ep);
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;

	/* Init BULK OUT EP2 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP2_BULK_OUT)].ep;
	usbd_ep_init(dev, ep);
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;

	return HAL_OK;
}

/**
  * @brief  Set class configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_inic_set_config(usb_dev_t *dev, u8 config)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;

	idev->dev = dev;

	usbd_inic_set_wifi_config(dev, config);

	if (idev->cb->set_config != NULL) {
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
static int usbd_inic_clear_wifi_config(usb_dev_t *dev, u8 config)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_ep_t *ep;

	UNUSED(config);

	/* DeInit BULK IN EP3 */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP3_BULK_IN)].ep;
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit BULK OUT EP4 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP4_BULK_OUT)].ep;
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit BULK OUT EP2 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP2_BULK_OUT)].ep;
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	return HAL_OK;
}

/**
  * @brief  Clear class configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_inic_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0U;
	usbd_inic_dev_t *idev = &usbd_inic_dev;

	UNUSED(config);

	usbd_inic_clear_wifi_config(dev, config);

	if (idev->cb->clear_config != NULL) {
		idev->cb->clear_config();
	}

	return ret;
}

/**
  * @brief  Handle INIC specific CTRL requests
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_inic_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	u8 alt;
	int ret = HAL_OK;
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				alt = USB_LOW_BYTE(req->wValue);
				switch (req->wIndex) {
				case USBD_INIC_ITF_WIFI:
					idev->wifi_alt = alt;
					break;
				default:
					break;
				}
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				switch (req->wIndex) {
				case USBD_INIC_ITF_WIFI:
					alt = idev->wifi_alt;
					break;
				default:
					ret = HAL_ERR_HW;
					break;
				}
				if (ret == HAL_OK) {
					ep0_in->xfer_buf[0] = alt;
					ep0_in->xfer_len = 1U;
					usbd_ep_transmit(dev, ep0_in);
				}
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
			if (req->wLength) {
				// SETUP + DATA IN + STATUS
				idev->cb->setup(req, ep0_in->xfer_buf);
				ep0_in->xfer_len = req->wLength;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				// SETUP + ZLP IN + STATUS, invalid
			}
		} else {
			if (req->wLength) {
				// SETUP + DATA OUT + STATUS, the DATA OUT phase is processed in ep0_data_out callback
				usb_os_memcpy((void *)&idev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				ep0_out->xfer_len = req->wLength;
				usbd_ep_receive(dev, ep0_out);
			} else {
				// SETUP + STATUS
				idev->cb->setup(req, NULL);
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
  * @param  dev: USB device instance
  * @retval Status
  */
static int usbd_inic_handle_ep0_data_out(usb_dev_t *dev)
{
	int ret = HAL_OK;
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_inic_cb_t *cb = idev->cb;

	UNUSED(dev);

	if (cb != NULL) {
		ret = cb->setup(&idev->ctrl_req, dev->ep0_out.xfer_buf);
	}

	return ret;
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_inic_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_inic_cb_t *cb = idev->cb;
	usbd_inic_ep_t *in_ep = &idev->in_ep[USB_EP_NUM(ep_addr)];
	usbd_ep_t *ep = &in_ep->ep;
	UNUSED(dev);

	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x TX err: %d\n", ep_addr, status);
	}

	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;

	if (cb->transmitted != NULL) {
		cb->transmitted(in_ep, status);
	}

	return HAL_OK;
}

/**
  * @brief  Data received on non-control Out endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint number
  * @retval Status
  */
static int usbd_inic_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_inic_cb_t *cb = idev->cb;
	usbd_inic_ep_t *out_ep = &idev->out_ep[USB_EP_NUM(ep_addr)];
	usbd_ep_t *ep = &out_ep->ep;
	void *userdata = out_ep->userdata;

	UNUSED(dev);

	if ((len > 0) && (cb->received != NULL)) {
		cb->received(out_ep, len);
	}

	if ((len == 0) || (cb->received == NULL) ||
		((ep_addr != USBD_WHC_WIFI_EP2_BULK_OUT) && (ep_addr != USBD_WHC_WIFI_EP4_BULK_OUT))) {
		usbd_inic_receive_data(ep_addr, ep->xfer_buf, ep->xfer_len, userdata);
	}

	return HAL_OK;
}

/**
  * @brief  Get descriptor callback
  * @param  dev: USB device instance
  * @param  req: Setup request handle
  * @param  buf: Poniter to Buffer
  * @retval Descriptor length
  */
static u16 usbd_inic_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	u16 len = 0;
	UNUSED(dev);

	switch (USB_HIGH_BYTE(req->wValue)) {

	case USB_DESC_TYPE_DEVICE:
		len = USB_LEN_DEV_DESC;
		usb_os_memcpy((void *)buf, (void *)usbd_inic_wifi_only_mode_dev_desc, len);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		len = sizeof(usbd_inic_wifi_only_mode_full_speed_config_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_inic_wifi_only_mode_full_speed_config_desc, len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		break;

	case USB_DESC_TYPE_STRING:
		switch (USB_LOW_BYTE(req->wValue)) {
		case USBD_IDX_LANGID_STR:
			len = USB_LEN_LANGID_STR_DESC;
			usb_os_memcpy((void *)buf, (void *)usbd_inic_lang_id_desc, len);
			break;
		case USBD_IDX_MFC_STR:
			len = usbd_get_str_desc(USBD_INIC_MFG_STRING, buf);
			break;
		case USBD_IDX_PRODUCT_STR:
			len = usbd_get_str_desc(USBD_INIC_PROD_STRING, buf);
			break;
		case USBD_IDX_SERIAL_STR:
			len = usbd_get_str_desc(USBD_INIC_SN_STRING, buf);
			break;
		case USBD_IDX_MS_OS_STR:
			/*Not support*/
			break;
		default:
			//RTK_LOGS(TAG, RTK_LOG_WARN, "Invalid str idx %d\n", USB_LOW_BYTE(req->wValue));
			break;
		}
		break;

	default:
		break;
	}

	return len;

}

static int usbd_inic_wifi_init(void)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_ep_t *ep;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP3_BULK_IN);
	ep = &idev->in_ep[ep_num].ep;
	ep->addr = USBD_WHC_WIFI_EP3_BULK_IN;
	ep->mps = USBD_INIC_FS_BULK_MPS;
	ep->type = USB_CH_EP_TYPE_BULK;


	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP4_BULK_OUT);
	ep = &idev->out_ep[ep_num].ep;
	ep->addr = USBD_WHC_WIFI_EP4_BULK_OUT;
	ep->mps = USBD_INIC_FS_BULK_MPS;
	ep->type = USB_CH_EP_TYPE_BULK;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP2_BULK_OUT);
	ep = &idev->out_ep[ep_num].ep;
	ep->mps = USBD_INIC_FS_BULK_MPS;
	ep->type = USB_CH_EP_TYPE_BULK;

	return HAL_OK;
}

/**
  * @brief  USB attach status change
  * @param  dev: USB device instance
  * @param  old_status: USB old attach status
  * @param  status: USB USB attach status
  * @retval void
  */
static void usbd_inic_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;

	UNUSED(dev);

	if (idev->cb->status_changed) {
		idev->cb->status_changed(old_status, status);
	}
}

static int usbd_inic_suspend(usb_dev_t *dev)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;

	UNUSED(dev);

	if (idev->cb->suspend) {
		idev->cb->suspend();
	}

	return HAL_OK;
}

static int usbd_inic_resume(usb_dev_t *dev)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;

	UNUSED(dev);

	if (idev->cb->resume) {
		idev->cb->resume();
	}

	return HAL_OK;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize inic device
  * @retval Status
  */
int usbd_inic_init(usbd_inic_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_inic_dev_t *idev = &usbd_inic_dev;

	usbd_inic_wifi_init();

	if (cb != NULL) {
		idev->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				goto init_exit;
			}
		}
	}

	usbd_register_class(&usbd_inic_driver);

	return ret;

init_exit:
	return ret;
}

/**
  * @brief  DeInitialize inic device
  * @param  void
  * @retval Status
  */
int usbd_inic_deinit(void)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;

	if (idev->cb != NULL) {
		if (idev->cb->deinit != NULL) {
			idev->cb->deinit();
		}
		idev->cb = NULL;
	}

	usbd_unregister_class();

	return HAL_OK;
}

int usbd_inic_transmit_ctrl_data(u8 *buf, u16 len)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usb_dev_t *dev = idev->dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;

	if (!dev->is_ready) {
		return HAL_ERR_HW;
	}

	if (len > ep0_in->xfer_buf_len) {
		len = ep0_in->xfer_buf_len;
	}
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "CTRL TX len=%d\n", len);
	usb_os_memcpy((void *)ep0_in->xfer_buf, (void *)buf, len);
	ep0_in->xfer_len = len;
	usbd_ep_transmit(dev, ep0_in);

	return HAL_OK;
}

int usbd_inic_transmit_data(u8 ep_addr, u8 *buf, u16 len, void *userdata)
{
	int ret = HAL_OK;
	u8 num = USB_EP_NUM(ep_addr);
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usb_dev_t *dev = idev->dev;
	usbd_ep_t *ep;

	if (USB_EP_IS_OUT(ep_addr) || (num >= USB_MAX_ENDPOINTS)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid IN EP num: 0x%02x\n", ep_addr);
		return HAL_ERR_PARA;
	}

	if (!dev->is_ready) {
		return HAL_ERR_HW;
	}

	ep = &idev->in_ep[num].ep;

	if (ep->xfer_state == USBD_INIC_EP_STATE_IDLE) {
		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "EP%02x TX len=%d data=%d\n", num, len, buf[0]);
		ep->xfer_state = USBD_INIC_EP_STATE_BUSY;
		idev->in_ep[num].userdata = userdata;
		usb_os_memcpy((void *)ep->xfer_buf, (void *)buf, len);
		ep->xfer_len = len;
		usbd_ep_transmit(idev->dev, ep);
	} else {
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x TX len=%d data=%d: BUSY\n", num, len, buf[0]);
		ret = HAL_BUSY;
	}

	return ret;
}

int usbd_inic_receive_data(u8 ep_addr, u8 *buf, u16 len, void *userdata)
{
	u8 num = USB_EP_NUM(ep_addr);
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usb_dev_t *dev = idev->dev;
	usbd_ep_t *ep;

	if (USB_EP_IS_IN(ep_addr) || (num >= USB_MAX_ENDPOINTS)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid OUT EP num: 0x%02x\n", ep_addr);
		return HAL_ERR_PARA;
	}

	if (!dev->is_ready) {
		/*RX not ready*/
		return HAL_ERR_HW;
	}

	ep = &idev->out_ep[num].ep;

	ep->xfer_buf = buf;
	ep->xfer_len = len;
	idev->out_ep[num].userdata = userdata;
	usbd_ep_receive(idev->dev, ep);

	return HAL_OK;
}

