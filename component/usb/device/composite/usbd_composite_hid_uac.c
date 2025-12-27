/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_composite_hid_uac.h"

/* Private defines -----------------------------------------------------------*/
#define USBD_DEV_DESC_OFFSET_PID               10U


#define CONFIG_USBD_COMPOSITE_HID_CDC_ACM_BULK_IN_XFER_SIZE		2048U
#define CONFIG_USBD_COMPOSITE_HID_CDC_ACM_BULK_OUT_XFER_SIZE	2048U
/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int usbd_composite_set_config(usb_dev_t *dev, u8 config);
static int usbd_composite_clear_config(usb_dev_t *dev, u8 config);
static int usbd_composite_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_composite_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int usbd_composite_sof(usb_dev_t *dev);
static int usbd_composite_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_composite_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_composite_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static void usbd_composite_status_changed(usb_dev_t *dev, u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "COMP";

/* USB Standard Device Descriptor */
static const u8 usbd_composite_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,                                                /* bLength */
	USB_DESC_TYPE_DEVICE,                                            /* bDescriptorType */
	0x00, 0x02,                                                      /* bcdUSB */
	0x00,                                                            /* bDeviceClass: Miscellaneous */
	0x00,                                                            /* bDeviceSubClass: Common Class */
	0x00,                                                            /* bDeviceProtocol: Interface Association Descriptor */
	USB_MAX_EP0_SIZE,                                                /* bMaxPacketSize */
	USB_LOW_BYTE(USBD_COMP_VID), USB_HIGH_BYTE(USBD_COMP_VID),       /* idVendor */
	USB_LOW_BYTE(USBD_COMP_UAC_PID), USB_HIGH_BYTE(USBD_COMP_UAC_PID),       /* idProduct */
	0x00, 0x02,                                                      /* bcdDevice */
	USBD_IDX_MFC_STR,                                                /* iManufacturer */
	USBD_IDX_PRODUCT_STR,                                            /* iProduct */
	USBD_IDX_SERIAL_STR,                                             /* iSerialNumber */
	0x01                                                             /* bNumConfigurations */
};  /* usbd_composite_dev_desc */

/* USB Standard String Descriptor 0 */
static const u8 usbd_composite_lang_id_desc[USB_LEN_LANGID_STR_DESC] = {
	USB_LEN_LANGID_STR_DESC,                                         /* bLength */
	USB_DESC_TYPE_STRING,                                            /* bDescriptorType */
	USB_LOW_BYTE(USBD_COMP_LANGID), USB_HIGH_BYTE(USBD_COMP_LANGID)  /* wLANGID */
};  /* usbd_composite_lang_id_desc */

#if !defined(CONFIG_USBD_COMPOSITE_HID_UAC1)
/* USB Standard Device Qualifier Descriptor */
static const u8 usbd_composite_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
	USB_LEN_DEV_QUALIFIER_DESC,                                      /* bLength */
	USB_DESC_TYPE_DEVICE_QUALIFIER,                                  /* bDescriptorType */
	0x00, 0x02,                                                      /* bcdUSB */
	0x00,                                                            /* bDeviceClass: Miscellaneous */
	0x00,                                                            /* bDeviceSubClass: Common Class */
	0x00,                                                            /* bDeviceProtocol: Interface Association Descriptor */
	USB_MAX_EP0_SIZE,                                                /* bMaxPacketSize */
	0x01,                                                            /* bNumConfigurations */
	0x00,                                                            /* Reserved */
};  /* usbd_composite_device_qualifier_desc */
#endif

/* USB Hid ACM Device High Speed Configuration Descriptor */
static const u8 usbd_composite_config_desc[USB_LEN_CFG_DESC] = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                                                /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                                     /* bDescriptorType */
	0x00,
	0x00,                                                            /* wTotalLength: calculated at runtime */
	0x04,                                                            /* bNumInterfaces */
	0x01,                                                            /* bConfigurationValue */
	0x00,                                                            /* iConfiguration */
	0xC0,                                                            /* bmAttributes: self powered */
	0x32,                                                            /* bMaxPower */
};  /* usbd_composite_hs_config_desc */

/* Composite Class Driver */
static const usbd_class_driver_t usbd_composite_driver = {
	.get_descriptor = usbd_composite_get_descriptor,
	.set_config = usbd_composite_set_config,
	.clear_config = usbd_composite_clear_config,
	.setup = usbd_composite_setup,
	.sof = usbd_composite_sof,
	.ep0_data_out = usbd_composite_handle_ep0_data_out,
	.ep_data_in = usbd_composite_handle_ep_data_in,
	.ep_data_out = usbd_composite_handle_ep_data_out,
	.status_changed = usbd_composite_status_changed,
};

/* Composite Device */
static usbd_composite_dev_t usbd_composite_dev;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Handles the SOF event for the composite device
  * @param  dev: USB device instance
  * @retval Status
  */
static int usbd_composite_sof(usb_dev_t *dev)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	if ((cdev->hid != NULL) && (cdev->hid->sof != NULL)) {
		cdev->hid->sof(dev);
	}

	if ((cdev->uac != NULL) && (cdev->uac->sof)) {
		cdev->uac->sof(dev);
	}

	return HAL_OK;
}

/**
  * @brief  Set Hid class configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_composite_set_config(usb_dev_t *dev, u8 config)
{
	int ret = HAL_OK;
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	cdev->dev = dev;
	// RTK_LOGS(TAG, RTK_LOG_INFO, "Set cfg\n");

	if ((cdev->hid != NULL) && (cdev->hid->set_config != NULL)) {
		cdev->hid->set_config(dev, config);
	}

	if ((cdev->uac != NULL) && (cdev->uac->set_config)) {
		cdev->uac->set_config(dev, config);
	}

	if ((cdev->cb != NULL) && (cdev->cb->set_config)) {
		cdev->cb->set_config();
	}

	return ret;
}

/**
  * @brief  Clear Hid ACM configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_composite_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0U;
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	if ((cdev->hid != NULL) && (cdev->hid->clear_config)) {
		ret += cdev->hid->clear_config(dev, config);  //TODO
	}

	if ((cdev->uac != NULL) && (cdev->uac->clear_config)) {
		ret += cdev->uac->clear_config(dev, config);
	}

	return ret;
}

/**
  * @brief  Check USB Audio Class request version
  * @param  entityId: USB entity ID
  * @param  req: Pointer to the USB request structure
  * @retval Status
  */
static int usbd_composite_is_uac_class_request(int entityId, usb_setup_req_t *req)
{
	int ret = 0U;
#if defined(CONFIG_USBD_COMPOSITE_CDC_ACM_UAC1)
	ret = (entityId == USBD_COMP_UAC_AC_HEADSET) ||
		  (entityId == USBD_COMP_UAC_AS_HEADSET_HEADPHONES) ||
		  ((req->bmRequestType & 0x1FU) == USB_REQ_RECIPIENT_ENDPOINT);
#else/* CONFIG_USBD_COMPOSITE_CDC_ACM_UAC2 */
	UNUSED(req);
	ret = (entityId == USBD_COMP_UAC_AC_HEADSET) ||
		  (entityId == USBD_COMP_UAC_AS_HEADSET_HEADPHONES);
#endif
	return ret;
}

/**
  * @brief  Handle Hid specific CTRL requests
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_composite_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	int ret = HAL_OK;
	u8 entityId;

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		default:
			entityId = USB_LOW_BYTE(req->wIndex);
			if ((entityId == USBD_COMP_HID_ITF) || (entityId == USBD_COMP_HID_VEND_ITF)) {
				ret = cdev->hid->setup(dev, req);
			} else {
				if ((entityId == USBD_COMP_UAC_AC_HEADSET) || (entityId == USBD_COMP_UAC_AS_HEADSET_HEADPHONES)) {
					if ((cdev->uac != NULL) && (cdev->uac->setup != NULL)) {
						ret = cdev->uac->setup(dev, req);
					}
				}
			}
			break;
		}
		break;

	case USB_REQ_TYPE_CLASS:
		entityId = USB_LOW_BYTE(req->wIndex);
		if ((entityId == USBD_COMP_HID_ITF) || (entityId == USBD_COMP_HID_VEND_ITF)) {
			ret = cdev->hid->setup(dev, req);
		} else if (usbd_composite_is_uac_class_request(entityId, req)) {
			if ((cdev->uac != NULL) && (cdev->uac->setup != NULL)) {
				ret = cdev->uac->setup(dev, req);
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
  * @brief  Data sent on non-control IN endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_composite_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	int ret = HAL_OK;
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	if ((ep_addr == USBD_COMP_HID_INTR_IN_EP) || (ep_addr == USBD_COMP_HID_CONSUMER_INTR_IN_EP)) {
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

	if (ep_addr == USBD_COMP_HID_INTR_OUT_EP) {
		if ((cdev->hid != NULL) && (cdev->hid->ep_data_out != NULL)) {
			ret = cdev->hid->ep_data_out(dev, ep_addr, len);
		}
	} else if ((ep_addr == USBD_COMP_UAC_ISOC_OUT_EP) && (cdev->uac != NULL) &&
			   (cdev->uac->ep_data_out != NULL)) {
		ret = cdev->uac->ep_data_out(dev, ep_addr, len);
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

	// RTK_LOGS(TAG, RTK_LOG_DEBUG, "usbd_composite_handle_ep0_data_out\n");

	if ((cdev->hid != NULL) && (cdev->hid->ep0_data_out)) {
		cdev->hid->ep0_data_out(dev);
	}

	if ((cdev->uac != NULL) && (cdev->uac->ep0_data_out != NULL)) {
		ret = cdev->uac->ep0_data_out(dev);
	}

	return ret;
}

/**
  * @brief  USB attach status change
  * @param  dev: USB device instance
  * @param  old_status: USB old attach status
  * @param  status: USB attach status
  * @retval void
  */
static void usbd_composite_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	UNUSED(dev);

	if ((cdev->hid != NULL) && (cdev->hid->status_changed)) {
		cdev->hid->status_changed(dev, old_status, status);
	}

	if ((cdev->uac != NULL) && (cdev->uac->status_changed != NULL)) {
		cdev->uac->status_changed(dev, old_status, status);
	}

	if ((cdev->cb != NULL) && (cdev->cb->status_changed)) {
		cdev->cb->status_changed(old_status, status);
	}
}

/**
  * @brief  Get descriptor callback
  * @param  dev: USB device instance
  * @param  req: Setup request handle
  * @param  buf: Poniter to Buffer
  * @retval Descriptor length
  */
static u16 usbd_composite_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	usb_speed_type_t speed = dev->dev_speed;
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	u16 len = 0;
	u16 desc_len;
	u16 total_len = 0;

	dev->self_powered = USBD_COMP_SELF_POWERED;
	dev->remote_wakeup_en = USBD_COMP_REMOTE_WAKEUP_EN;

	switch (USB_HIGH_BYTE(req->wValue)) {

	case USB_DESC_TYPE_DEVICE:
		len = USB_LEN_DEV_DESC;
		usb_os_memcpy((void *)buf, (void *)usbd_composite_dev_desc, len);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
#if !defined(CONFIG_USBD_COMPOSITE_HID_UAC1)
	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
#endif
		usb_os_memcpy((void *)buf, (void *)usbd_composite_config_desc, USB_LEN_CFG_DESC);
		buf += USB_LEN_CFG_DESC;
		total_len += USB_LEN_CFG_DESC;

		desc_len = cdev->hid->get_descriptor(dev, req, buf);
		buf += desc_len;
		total_len += desc_len;

		desc_len = cdev->uac->get_descriptor(dev, req, buf);
		buf += desc_len;
		total_len += desc_len;

		buf = dev->ep0_in.xfer_buf;
#if !defined(CONFIG_USBD_COMPOSITE_HID_UAC1)
		if (USB_HIGH_BYTE(req->wValue) == USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION) {
			buf[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		}
#endif
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(total_len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(total_len);
		len = total_len;

		break;

#if !defined(CONFIG_USBD_COMPOSITE_HID_UAC1)
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		len = sizeof(usbd_composite_device_qualifier_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_composite_device_qualifier_desc, len);
		break;
#endif

	case USB_DESC_TYPE_STRING:
		switch (USB_LOW_BYTE(req->wValue)) {
		case USBD_IDX_LANGID_STR:
			len = sizeof(usbd_composite_lang_id_desc);
			usb_os_memcpy((void *)buf, (void *)usbd_composite_lang_id_desc, len);
			break;
		case USBD_IDX_MFC_STR:
			len = usbd_get_str_desc(USBD_COMP_MFG_STRING, buf);
			break;
		case USBD_IDX_PRODUCT_STR:
			len = usbd_get_str_desc(USBD_COMP_PROD_STRING, buf);
			break;
		case USBD_IDX_SERIAL_STR:
			len = usbd_get_str_desc(USBD_COMP_SN_STRING, buf);
			break;
		case USBD_IDX_HID_ITF_STR:
			if (speed == USB_SPEED_HIGH) {
				len = usbd_get_str_desc(USBD_COMP_HID_HS_ITF_STRING, buf);
			} else {
				len = usbd_get_str_desc(USBD_COMP_HID_FS_ITF_STRING, buf);
			}
			break;
		case USBD_IDX_HID_VEND_ITF_STR:
			if (speed == USB_SPEED_HIGH) {
				len = usbd_get_str_desc(USBD_COMP_HID_CONSUMER_HS_ITF_STRING, buf);
			} else {
				len = usbd_get_str_desc(USBD_COMP_HID_CONSUMER_FS_ITF_STRING, buf);
			}
			break;
		case USBD_IDX_UAC_ITF_STR:
			if (speed == USB_SPEED_HIGH) {
				len = usbd_get_str_desc(USBD_COMP_UAC_HS_ITF_STRING, buf);
			} else {
				len = usbd_get_str_desc(USBD_COMP_UAC_FS_ITF_STRING, buf);
			}
			break;
		case USBD_IDX_MS_OS_STR:
			/*Not support*/
			break;
		/* Add customer string here */
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

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init Hid Uac Composite class
  * @param  hid_cb: Hid user callback
  * @param  uac_cb: uac user callback
  * @param  cb:  composite callback
  * @retval Status
  */
int usbd_composite_init(usbd_composite_hid_usr_cb_t *hid_cb, usbd_composite_uac_usr_cb_t *uac_cb,
						usbd_composite_cb_t *cb)
{
	int ret;
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	if ((hid_cb == NULL) || (uac_cb == NULL)) {
		ret = HAL_ERR_PARA;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return ret;
	}

	if (cb != NULL) {
		cdev->cb = cb;
	}

	ret = usbd_composite_hid_init(cdev, hid_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init HID itf fail: %d\n", ret);
		return ret;
	}
	cdev->hid = (usbd_class_driver_t *)&usbd_composite_hid_driver;

	ret = usbd_composite_uac_init(cdev, uac_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init UAC itf fail: %d\n", ret);
		usbd_composite_hid_deinit();
		return ret;
	}
	cdev->uac = (usbd_class_driver_t *)&usbd_composite_uac_driver;

	usbd_register_class(&usbd_composite_driver);

	return ret;
}

/**
  * @brief  DeInit HID UAC Composite Class
  * @param  void
  * @retval Status
  */
void usbd_composite_deinit(void)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	usbd_unregister_class();

	if (cdev->uac) {
		usbd_composite_uac_deinit();
		cdev->uac = NULL;
	}

	if (cdev->hid) {
		usbd_composite_hid_deinit();
		cdev->hid = NULL;
	}

	cdev->cb = NULL;
	cdev->dev = NULL;
}
