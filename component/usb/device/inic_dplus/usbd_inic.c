/**
  ******************************************************************************
  * @file    usbd_inic.c
  * @author  Realsil WLAN5 Team
  * @brief   This file provides the functionalities of the USB INIC Class
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
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
static u8 *usbd_inic_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len);
static int usbd_inic_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_inic_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_inic_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static void usbd_inic_status_changed(usb_dev_t *dev, u8 status);
static int usbd_inic_suspend(usb_dev_t *dev);
static int usbd_inic_resume(usb_dev_t *dev);

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "INIC";

static u8 usbd_inic_wifi_only_mode_dev_desc[USB_LEN_DEV_DESC] USB_DMA_ALIGNED = {
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
static u8 usbd_inic_lang_id_desc[USB_LEN_LANGID_STR_DESC] USB_DMA_ALIGNED = {
	USB_LEN_LANGID_STR_DESC,
	USB_DESC_TYPE_STRING,
	USB_LOW_BYTE(USBD_INIC_LANGID_STRING),
	USB_HIGH_BYTE(USBD_INIC_LANGID_STRING),
};

/* USB Full Speed Configuration Descriptor for WiFi-only mode */
static u8 usbd_inic_wifi_only_mode_full_speed_config_desc[USBD_INIC_WIFI_ONLY_MODE_CONFIG_DESC_SIZE] USB_DMA_ALIGNED = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,						// bLength: Configuration Descriptor size
	USB_DESC_TYPE_CONFIGURATION,			// bDescriptorType: Configuration
	USB_LOW_BYTE(USBD_INIC_WIFI_ONLY_MODE_CONFIG_DESC_SIZE),	// wTotalLength: number of returned bytes
	USB_HIGH_BYTE(USBD_INIC_WIFI_ONLY_MODE_CONFIG_DESC_SIZE),
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
usbd_class_driver_t usbd_inic_driver = {
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
  * @brief  Transmit BULK IN ZLP packet
  * @retval Status
  */
static int usbd_inic_transmit_zlp(u8 ep_addr)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;

	RTK_LOGS(TAG, "[INIC] IN EP%02X TX ZLP\n", ep_addr);
	usbd_ep_transmit(idev->dev, ep_addr, NULL, 0);

	return HAL_OK;
}

/**
  * @brief  Set class configuration for WiFi interface
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_inic_set_wifi_config(usb_dev_t *dev, u8 config)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_inic_ep_t *ep;

	UNUSED(config);

	/* Init BULK IN EP3 */
	ep = &idev->in_ep[USB_EP_NUM(USBD_INIC_WIFI_EP3_BULK_IN)];
	ep->mps = USBD_INIC_FS_BULK_MPS;
	ep->type = USB_CH_EP_TYPE_BULK;
	usbd_ep_init(dev, USBD_INIC_WIFI_EP3_BULK_IN, USB_CH_EP_TYPE_BULK, ep->mps);
	ep->state = USBD_INIC_EP_STATE_IDLE;

	/* Init BULK OUT EP4 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_INIC_WIFI_EP4_BULK_OUT)];
	ep->mps = USBD_INIC_FS_BULK_MPS;
	ep->type = USB_CH_EP_TYPE_BULK;
	usbd_ep_init(dev, USBD_INIC_WIFI_EP4_BULK_OUT, USB_CH_EP_TYPE_BULK, ep->mps);
	ep->state = USBD_INIC_EP_STATE_IDLE;

	/* Init BULK OUT EP2 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_INIC_WIFI_EP2_BULK_OUT)];
	ep->mps = USBD_INIC_FS_BULK_MPS;
	ep->type = USB_CH_EP_TYPE_BULK;
	usbd_ep_init(dev, USBD_INIC_WIFI_EP2_BULK_OUT, USB_CH_EP_TYPE_BULK, ep->mps);
	ep->state = USBD_INIC_EP_STATE_IDLE;

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
	idev->is_ready = 1U;

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
	usbd_inic_ep_t *ep;

	UNUSED(config);

	/* DeInit BULK IN EP3 */
	ep = &idev->in_ep[USB_EP_NUM(USBD_INIC_WIFI_EP3_BULK_IN)];
	ep->state = USBD_INIC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, USBD_INIC_WIFI_EP3_BULK_IN);

	/* DeInit BULK OUT EP4 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_INIC_WIFI_EP4_BULK_OUT)];
	ep->state = USBD_INIC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, USBD_INIC_WIFI_EP4_BULK_OUT);

	/* DeInit BULK OUT EP2 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_INIC_WIFI_EP2_BULK_OUT)];
	ep->state = USBD_INIC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, USBD_INIC_WIFI_EP2_BULK_OUT);

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

	idev->is_ready = 0U;

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

	//RTK_LOGD(TAG, "SETUP: bmRequestType=0x%02x bRequest=0x%02x wLength=0x%04x wValue=%x\n",
	//		 req->bmRequestType, req->bRequest, req->wLength, req->wValue);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				alt = (u8)(req->wValue & 0xFF);
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
					idev->ctrl_buf[0] = alt;
					usbd_ep0_transmit(dev, idev->ctrl_buf, 1U);
				}
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				idev->ctrl_buf[0] = 0U;
				idev->ctrl_buf[1] = 0U;
				usbd_ep0_transmit(dev, idev->ctrl_buf, 2U);
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
				idev->cb->setup(req, idev->ctrl_buf);
				usbd_ep0_transmit(dev, idev->ctrl_buf, req->wLength);
			} else {
				// SETUP + ZLP IN + STATUS, invalid
			}
		} else {
			if (req->wLength) {
				// SETUP + DATA OUT + STATUS, the DATA OUT phase is processed in ep0_data_out callback
				usb_os_memcpy((void *)&idev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				usbd_ep0_receive(dev, idev->ctrl_buf, req->wLength);
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
		ret = cb->setup(&idev->ctrl_req, idev->ctrl_buf);
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
	usbd_inic_ep_t *ep = &idev->in_ep[USB_EP_NUM(ep_addr)];
	UNUSED(dev);

	if (status == HAL_OK) {
		/*TX done*/
		if (ep->zlp) {
			ep->zlp = 0;
			usbd_inic_transmit_zlp(ep_addr);
		} else {
			ep->state = USBD_INIC_EP_STATE_IDLE;
		}
	} else {
		RTK_LOGS(TAG, "[INIC] EP%02x TX err: %d\n", ep_addr, status);
		ep->state = USBD_INIC_EP_STATE_IDLE;
	}

	if ((cb->transmitted != NULL) && (ep->state == USBD_INIC_EP_STATE_IDLE)) {
		cb->transmitted(ep, status);
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
	usbd_inic_ep_t *ep = &idev->out_ep[USB_EP_NUM(ep_addr)];

	UNUSED(dev);

	if ((len > 0) && (cb->received != NULL)) {
		cb->received(ep, len);
	}

	usbd_inic_receive_data(ep_addr, ep->buf, ep->buf_len);

	return HAL_OK;
}

/**
  * @brief  Get descriptor callback
  * @param  req: setup request
  * @param  speed: device speed
  * @param  len: descriptor length
  * @retval descriptor buffer
  */
static u8 *usbd_inic_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len)
{
	UNUSED(dev);
	UNUSED(speed);
	u8 *buf = NULL;
	u16 length;
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	u8 *desc = idev->ctrl_buf;

	switch ((req->wValue >> 8) & 0xFF) {

	case USB_DESC_TYPE_DEVICE:
		buf = usbd_inic_wifi_only_mode_dev_desc;
		length = USB_LEN_DEV_DESC;
		usb_os_memcpy((void *)desc, (void *)buf, length);
		buf = desc;
		*len = length;
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		buf = usbd_inic_wifi_only_mode_full_speed_config_desc;
		length = USBD_INIC_WIFI_ONLY_MODE_CONFIG_DESC_SIZE;
		usb_os_memcpy((void *)desc, (void *)buf, length);
		buf = desc;
		*len = length;
		break;

	case USB_DESC_TYPE_STRING:
		switch (req->wValue & 0xFF) {
		case USBD_IDX_LANGID_STR:
			buf = usbd_inic_lang_id_desc;
			*len = USB_LEN_LANGID_STR_DESC;
			break;
		case USBD_IDX_MFC_STR:
			usbd_get_str_desc(USBD_INIC_MFG_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_PRODUCT_STR:
			usbd_get_str_desc(USBD_INIC_PROD_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_SERIAL_STR:
			usbd_get_str_desc(USBD_INIC_SN_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_MS_OS_STR:
			/*Not support*/
			break;
		default:
			RTK_LOGS(TAG, "[INIC] Invalid str idx %d\n", req->wValue & 0xFF);
			break;
		}
		break;

	default:
		break;
	}

	return buf;

}

static int usbd_inic_wifi_init(void)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_inic_ep_t *ep;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_INIC_WIFI_EP3_BULK_IN);
	ep = &idev->in_ep[ep_num];
	ep->addr = USBD_INIC_WIFI_EP3_BULK_IN;
	ep->valid = 1U;

	ep_num = USB_EP_NUM(USBD_INIC_WIFI_EP4_BULK_OUT);
	ep = &idev->out_ep[ep_num];
	ep->addr = USBD_INIC_WIFI_EP4_BULK_OUT;
	ep->valid = 1U;

	ep_num = USB_EP_NUM(USBD_INIC_WIFI_EP2_BULK_OUT);
	ep = &idev->out_ep[ep_num];
	ep->addr = USBD_INIC_WIFI_EP2_BULK_OUT;
	ep->valid = 1U;

	return HAL_OK;
}

static void usbd_inic_wifi_deinit(void)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_inic_ep_t *ep;

	ep = &idev->in_ep[USB_EP_NUM(USBD_INIC_WIFI_EP3_BULK_IN)];
	ep->valid = 0U;

	ep = &idev->out_ep[USB_EP_NUM(USBD_INIC_WIFI_EP4_BULK_OUT)];
	ep->valid = 0U;

	ep = &idev->out_ep[USB_EP_NUM(USBD_INIC_WIFI_EP2_BULK_OUT)];
	ep->valid = 0U;

}

/**
  * @brief  USB attach status change
  * @param  dev: USB device instance
  * @param  config: USB USB attach status
  * @retval void
  */
static void usbd_inic_status_changed(usb_dev_t *dev, u8 status)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;

	UNUSED(dev);

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		idev->is_ready = 0U;
	}

	if (idev->cb->status_changed) {
		idev->cb->status_changed(status);
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
	idev->is_ready = 1U;
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

	idev->ctrl_buf_len = USBD_INIC_CTRL_BUF_SIZE;
	idev->ctrl_buf = (u8 *)usb_os_malloc(idev->ctrl_buf_len);
	if (idev->ctrl_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_exit;
	}

	ret = usbd_inic_wifi_init();
	if (ret != HAL_OK) {
		goto init_clean_ctrl_buf_exit;
	}

	if (cb != NULL) {
		idev->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				goto init_deinit_wifi_exit;
			}
		}
	}

	usbd_register_class(&usbd_inic_driver);

	return ret;

init_deinit_wifi_exit:
	usbd_inic_wifi_deinit();

init_clean_ctrl_buf_exit:
	usb_os_mfree(idev->ctrl_buf);
	idev->ctrl_buf = NULL;

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

	idev->is_ready = 0U;

	if (idev->cb != NULL) {
		if (idev->cb->deinit != NULL) {
			idev->cb->deinit();
		}
		idev->cb = NULL;
	}

	usbd_unregister_class();

	usbd_inic_wifi_deinit();

	if (idev->ctrl_buf != NULL) {
		usb_os_mfree(idev->ctrl_buf);
		idev->ctrl_buf = NULL;
	}

	return HAL_OK;
}

int usbd_inic_transmit_ctrl_data(u8 *buf, u16 len)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usb_dev_t *dev = idev->dev;

	if (!idev->is_ready) {
		return HAL_ERR_HW;
	}

	if (len > idev->ctrl_buf_len) {
		len = idev->ctrl_buf_len;
	}
	RTK_LOGS(TAG, "[INIC] CTRL TX len=%d\n", len);
	usb_os_memcpy(idev->ctrl_buf, buf, len);

	usbd_ep0_transmit(dev, idev->ctrl_buf, len);

	return HAL_OK;
}

int usbd_inic_transmit_data(u8 ep_addr, u8 *buf, u16 len, void *userdata)
{
	int ret = HAL_OK;
	u8 num = USB_EP_NUM(ep_addr);
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_inic_ep_t *ep;

	if (USB_EP_IS_OUT(ep_addr) || (num >= USB_MAX_ENDPOINTS)) {
		RTK_LOGS(TAG, "[INIC] Invalid IN EP num: 0x%02x\n", ep_addr);
		return HAL_ERR_PARA;
	}

	if (!idev->is_ready) {
		return HAL_ERR_HW;
	}

	ep = &idev->in_ep[num];

	if (len > ep->buf_len) {
		len = ep->buf_len;
	}

	if (((len % ep->mps) == 0) && (ep->type == USB_CH_EP_TYPE_BULK)) {
		ep->zlp = 1;
	} else {
		ep->zlp = 0;
	}

	if (ep->state == USBD_INIC_EP_STATE_IDLE) {
		RTK_LOGS(TAG, "[INIC] EP%02x TX len=%d data=%d\n", num, len, buf[0]);
		ep->state = USBD_INIC_EP_STATE_BUSY;
		ep->userdata = userdata;
		ep->buf = buf;
		usbd_ep_transmit(idev->dev, ep_addr, ep->buf, len);
	} else {
		RTK_LOGS(TAG, "[INIC] EP%02x TX len=%d data=%d: BUSY\n", num, len, buf[0]);
		ret = HAL_BUSY;
	}

	return ret;
}

int usbd_inic_receive_data(u8 ep_addr, u8 *buf, u16 len)
{
	u8 num = USB_EP_NUM(ep_addr);
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_inic_ep_t *ep;

	if (USB_EP_IS_IN(ep_addr) || (num >= USB_MAX_ENDPOINTS)) {
		RTK_LOGS(TAG, "[INIC] Invalid OUT EP num: 0x%02x\n", ep_addr);
		return HAL_ERR_PARA;
	}

	if (!idev->is_ready) {
		/*RX not ready*/
		return HAL_ERR_HW;
	}

	ep = &idev->out_ep[num];

	ep->buf = buf;
	ep->buf_len = len;
	usbd_ep_receive(idev->dev, ep_addr, ep->buf, ep->buf_len);

	return HAL_OK;
}

