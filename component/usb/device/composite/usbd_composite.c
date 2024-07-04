/**
  ******************************************************************************
  * @file    usbd_cdc_acm.c
  * @author  Realsil WLAN5 Team
  * @brief   This file provides the functionalities of the USB CDC Class
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_composite.h"
#include "usbd_composite_cdc_acm.h"
#include "usbd_composite_hid.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_composite_set_config(usb_dev_t *dev, u8 config);
static int usbd_composite_clear_config(usb_dev_t *dev, u8 config);
static int usbd_composite_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u8 *usbd_composite_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len);
static int usbd_composite_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_composite_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_composite_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static void usbd_composite_status_changed(usb_dev_t *dev, u8 status);

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "COMP";

/* USB Standard Device Descriptor */
static u8 usbd_composite_dev_desc[USB_LEN_DEV_DESC] USB_DMA_ALIGNED = {
	USB_LEN_DEV_DESC,												/* bLength */
	USB_DESC_TYPE_DEVICE,											/* bDescriptorType */
	0x00, 0x02,														/* bcdUSB */
	0xEF,															/* bDeviceClass: Miscellaneous */
	0x02,															/* bDeviceSubClass: Common Class */
	0x01,															/* bDeviceProtocol: Interface Association Descriptor */
	USB_MAX_EP0_SIZE,												/* bMaxPacketSize */
	USB_LOW_BYTE(USBD_COMP_VID), USB_HIGH_BYTE(USBD_COMP_VID),		/* idVendor */
	USB_LOW_BYTE(USBD_COMP_PID), USB_HIGH_BYTE(USBD_COMP_PID),		/* idProduct */
	0x00, 0x02,														/* bcdDevice */
	USBD_IDX_MFC_STR,												/* iManufacturer */
	USBD_IDX_PRODUCT_STR,											/* iProduct */
	USBD_IDX_SERIAL_STR,											/* iSerialNumber */
	0x01															/* bNumConfigurations */
};  /* usbd_composite_dev_desc */

/* USB Standard String Descriptor 0 */
static u8 usbd_composite_lang_id_desc[USB_LEN_LANGID_STR_DESC] USB_DMA_ALIGNED = {
	USB_LEN_LANGID_STR_DESC,										/* bLength */
	USB_DESC_TYPE_STRING,											/* bDescriptorType */
	USB_LOW_BYTE(USBD_COMP_LANGID), USB_HIGH_BYTE(USBD_COMP_LANGID)	/* wLANGID */
};  /* usbd_composite_lang_id_desc */

/* USB Standard Device Qualifier Descriptor */
static u8 usbd_composite_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] USB_DMA_ALIGNED = {
	USB_LEN_DEV_QUALIFIER_DESC,										/* bLength */
	USB_DESC_TYPE_DEVICE_QUALIFIER,									/* bDescriptorType */
	0x00, 0x02,														/* bcdUSB */
	0xEF,															/* bDeviceClass: Miscellaneous */
	0x02,															/* bDeviceSubClass: Common Class */
	0x01,															/* bDeviceProtocol: Interface Association Descriptor */
	USB_MAX_EP0_SIZE,												/* bMaxPacketSize */
	0x01,															/* bNumConfigurations */
	0x00,															/* Reserved */
};  /* usbd_composite_device_qualifier_desc */

/* USB CDC ACM Device High Speed Configuration Descriptor */
static u8 usbd_composite_config_desc[USB_LEN_CFG_DESC] USB_DMA_ALIGNED = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,												/* bLength */
	USB_DESC_TYPE_CONFIGURATION,									/* bDescriptorType */
	0x00, 0x00,														/* wTotalLength: calculated at runtime */
	0x03,															/* bNumInterfaces */
	0x01,															/* bConfigurationValue */
	0x00,															/* iConfiguration */
#if USBD_COMP_SELF_POWERED
	0xC0,															/* bmAttributes: self powered */
#else
	0x80,															/* bmAttributes: bus powered */
#endif
	0x32,															/* bMaxPower */
};  /* usbd_composite_hs_config_desc */

/* Composite Class Driver */
usbd_class_driver_t usbd_composite_driver = {
	.get_descriptor = usbd_composite_get_descriptor,
	.set_config = usbd_composite_set_config,
	.clear_config = usbd_composite_clear_config,
	.setup = usbd_composite_setup,
	.ep0_data_out = usbd_composite_handle_ep0_data_out,
	.ep_data_in = usbd_composite_handle_ep_data_in,
	.ep_data_out = usbd_composite_handle_ep_data_out,
	.status_changed = usbd_composite_status_changed,
};

/* Composite Device */
static usbd_composite_dev_t usbd_composite_dev;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Set CDC class configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_composite_set_config(usb_dev_t *dev, u8 config)
{
	int ret = HAL_OK;
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	cdev->dev = dev;

	cdev->cdc->set_config(dev, config);
	cdev->hid->set_config(dev, config);

	return ret;
}

/**
  * @brief  Clear CDC ACM configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_composite_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0U;
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	cdev->cdc->clear_config(dev, config);
	cdev->hid->clear_config(dev, config);

	return ret;
}

/**
  * @brief  Handle CDC specific CTRL requests
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_composite_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	int ret = HAL_OK;

	//RTK_LOGD(TAG, "SETUP: bmRequestType=0x%02x bRequest=0x%02x wLength=0x%04x wValue=%x\n",
	//		 req->bmRequestType, req->bRequest, req->wLength, req->wValue);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->ctrl_buf[0] = 0U;
				usbd_ep0_transmit(dev, cdev->ctrl_buf, 1U);
			} else {
				ret = HAL_ERR_PARA;
			}

			break;

		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->ctrl_buf[0] = 0U;
				cdev->ctrl_buf[1] = 0U;
				usbd_ep0_transmit(dev, cdev->ctrl_buf, 2U);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		default:
			if (req->wIndex == USBD_COMP_HID_ITF) {
				// Get HID report descriptor
				ret = cdev->hid->setup(dev, req);
			}
			break;
		}
		break;
	case USB_REQ_TYPE_CLASS:
		if ((req->wIndex == USBD_COMP_CDC_COM_ITF) || (req->wIndex == USBD_COMP_CDC_DAT_ITF)) {
			ret = cdev->cdc->setup(dev, req);
		} else if (req->wIndex == USBD_COMP_HID_ITF) {
			ret = cdev->hid->setup(dev, req);
		} else {
			RTK_LOGS(TAG, "[COMP] Invalid class req\n");
		}
		break;
	default:
		ret = HAL_ERR_HW;
		break;
	}

	return ret;
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_composite_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	int ret = HAL_OK;
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	if ((ep_addr == USBD_COMP_CDC_BULK_IN_EP) || (ep_addr == USBD_COMP_CDC_INTR_IN_EP)) {
		if (cdev->cdc->ep_data_in != NULL) {
			ret = cdev->cdc->ep_data_in(dev, ep_addr, status);
		}
	} else if (ep_addr == USBD_COMP_HID_INTR_IN_EP) {
		if (cdev->hid->ep_data_in != NULL) {
			ret = cdev->hid->ep_data_in(dev, ep_addr, status);
		}
	}

	return ret;
}

/**
  * @brief  Data received on non-control Out endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_composite_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len)
{
	int ret = HAL_OK;
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	if (ep_addr == USBD_COMP_CDC_BULK_OUT_EP) {
		if (cdev->cdc->ep_data_out != NULL) {
			ret = cdev->cdc->ep_data_out(dev, ep_addr, len);
		}
	} else if (ep_addr == USBD_COMP_HID_INTR_OUT_EP) {
		if (cdev->hid->ep_data_out != NULL) {
			ret = cdev->hid->ep_data_out(dev, ep_addr, len);
		}
	}

	return ret;
}

/**
  * @brief  usbd_composite_handle_ep0_data_out
  *         Handle EP0 Rx Ready event
  * @param  dev: USB device instance
  * @retval Status
  */
static int usbd_composite_handle_ep0_data_out(usb_dev_t *dev)
{
	int ret = HAL_OK;
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	cdev->cdc->ep0_data_out(dev);
	cdev->hid->ep0_data_out(dev);

	return ret;
}

/**
  * @brief  USB attach status change
  * @param  dev: USB device instance
  * @param  status: USB attach status
  * @retval void
  */
static void usbd_composite_status_changed(usb_dev_t *dev, u8 status)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	UNUSED(dev);

	if (cdev->cb->status_changed) {
		cdev->cb->status_changed(status);
	}
}

static u8 *usbd_composite_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len)
{
	u8 *buf = NULL;
	u16 desc_len;
	u16 total_len = 0;
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	u8 *desc = cdev->ctrl_buf;

	dev->self_powered = USBD_COMP_SELF_POWERED;
	dev->remote_wakeup_en = USBD_COMP_REMOTE_WAKEUP_EN;

	switch ((req->wValue >> 8) & 0xFF) {

	case USB_DESC_TYPE_DEVICE:
		buf = usbd_composite_dev_desc;
		*len = USB_LEN_DEV_DESC;
		break;

	case USB_DESC_TYPE_CONFIGURATION:
	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		usb_os_memcpy((void *)desc, (void *)usbd_composite_config_desc, USB_LEN_CFG_DESC);
		desc += USB_LEN_CFG_DESC;
		total_len += USB_LEN_CFG_DESC;
		buf = cdev->cdc->get_descriptor(dev, req, speed, &desc_len);
		usb_os_memcpy((void *)desc, (void *)buf, desc_len);
		desc += desc_len;
		total_len += desc_len;
		buf = cdev->hid->get_descriptor(dev, req, speed, &desc_len);
		usb_os_memcpy((void *)desc, (void *)buf, desc_len);
		total_len += desc_len;
		buf = cdev->ctrl_buf;
		if (((req->wValue >> 8) & 0xFF) == USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION) {
			buf[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		}
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(total_len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(total_len);
		*len = total_len;
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		buf = usbd_composite_device_qualifier_desc;
		*len = sizeof(usbd_composite_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_STRING:
		switch (req->wValue & 0xFF) {
		case USBD_IDX_LANGID_STR:
			buf = usbd_composite_lang_id_desc;
			*len = sizeof(usbd_composite_lang_id_desc);
			break;
		case USBD_IDX_MFC_STR:
			usbd_get_str_desc(USBD_COMP_MFG_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_PRODUCT_STR:
			usbd_get_str_desc(USBD_COMP_PROD_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_SERIAL_STR:
			usbd_get_str_desc(USBD_COMP_SN_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_CDC_ITF_STR:
			if (speed == USB_SPEED_HIGH) {
				usbd_get_str_desc(USBD_COMP_CDC_HS_ITF_STRING, desc, len);
			} else {
				usbd_get_str_desc(USBD_COMP_CDC_FS_ITF_STRING, desc, len);
			}
			buf = desc;
			break;
		case USBD_IDX_HID_ITF_STR:
			if (speed == USB_SPEED_HIGH) {
				usbd_get_str_desc(USBD_COMP_HID_HS_ITF_STRING, desc, len);
			} else {
				usbd_get_str_desc(USBD_COMP_HID_FS_ITF_STRING, desc, len);
			}
			buf = desc;
			break;
		case USBD_IDX_MS_OS_STR:
			/*Not support*/
			break;
		/* Add customer string here */
		default:
			RTK_LOGS(TAG, "[COMP] Invalid str idx %d\n", req->wValue & 0xFF);
			break;
		}
		break;

	default:
		break;
	}

	return buf;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init CDC ACM class
  * @param  cb: CDC ACM user callback
  * @retval Status
  */
int usbd_composite_init(u16 cdc_bulk_out_xfer_size, u16 cdc_bulk_in_xfer_size, usbd_composite_cdc_acm_usr_cb_t *cdc_cb,
						u16 hid_intr_in_xfer_size, usbd_composite_hid_usr_cb_t *hid_cb, usbd_composite_cb_t *cb)
{
	int ret;
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	if ((cdc_cb == NULL) || (hid_cb == NULL)) {
		ret = HAL_ERR_PARA;
		RTK_LOGS(TAG, "[COMP] Invalid user CB\n");
		return ret;
	}

	if (cb != NULL) {
		cdev->cb = cb;
	}

	cdev->ctrl_buf = (u8 *)usb_os_malloc(COMP_CDC_ACM_CTRL_BUF_SIZE);
	if (cdev->ctrl_buf == NULL) {
		ret = HAL_ERR_MEM;
		RTK_LOGS(TAG, "[COMP] Alloc ctrl buf fail\n");
		return ret;
	}

	ret = usbd_composite_cdc_acm_init(cdev, cdc_bulk_out_xfer_size, cdc_bulk_in_xfer_size, cdc_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, "[COMP] Init CDC ACM itf fail: %d\n", ret);
		usb_os_mfree(cdev->ctrl_buf);
		return ret;
	}

	ret = usbd_composite_hid_init(cdev, hid_intr_in_xfer_size, hid_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, "[COMP] Init HID itf fail: %d\n", ret);
		usbd_composite_cdc_acm_deinit();
		usb_os_mfree(cdev->ctrl_buf);
		return ret;
	}

	cdev->cdc = &usbd_composite_cdc_acm_driver;
	cdev->hid = &usbd_composite_hid_driver;

	usbd_register_class(&usbd_composite_driver);

	return ret;
}

/**
  * @brief  DeInit CDC ACM class
  * @param  void
  * @retval Status
  */
void usbd_composite_deinit(void)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	usbd_composite_hid_deinit();
	usbd_composite_cdc_acm_deinit();
	usbd_unregister_class();

	if (cdev->ctrl_buf != NULL) {
		usb_os_mfree(cdev->ctrl_buf);
		cdev->ctrl_buf = NULL;
	}
}

