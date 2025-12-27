/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_vendor.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_vendor_set_config(usb_dev_t *dev, u8 config);
static int usbd_vendor_clear_config(usb_dev_t *dev, u8 config);
static int usbd_vendor_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_vendor_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int usbd_vendor_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_vendor_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static void usbd_vendor_status_changed(usb_dev_t *dev, u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "VND";

/* USB Standard Device Descriptor */
static const u8 usbd_vendor_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,                               /* bLength */
	USB_DESC_TYPE_DEVICE,                           /* bDescriptorType */
	0x00,                                           /* bcdUSB */
	0x02,
	0x00,                                           /* bDeviceClass */
	0x00,                                           /* bDeviceSubClass */
	0x00,                                           /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize */
	USB_LOW_BYTE(USBD_VENDOR_VID),                  /* idVendor */
	USB_HIGH_BYTE(USBD_VENDOR_VID),
	USB_LOW_BYTE(USBD_VENDOR_PID),                  /* idProduct */
	USB_HIGH_BYTE(USBD_VENDOR_PID),
	0x00,                                           /* bcdDevice */
	0x02,
	USBD_IDX_MFC_STR,                               /* iManufacturer */
	USBD_IDX_PRODUCT_STR,                           /* iProduct */
	USBD_IDX_SERIAL_STR,                            /* iSerialNumber */
	0x01                                            /* bNumConfigurations */
};  /* usbd_vendor_dev_desc */

/* USB Standard String Descriptor 0 */
static const u8 usbd_vendor_lang_id_desc[USB_LEN_LANGID_STR_DESC] = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(USBD_VENDOR_LANGID_STRING),            /* wLANGID */
	USB_HIGH_BYTE(USBD_VENDOR_LANGID_STRING),
};  /* usbd_vendor_lang_id_desc */

/* USB Standard Device Qualifier Descriptor */
static const u8 usbd_vendor_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
	USB_LEN_DEV_QUALIFIER_DESC,                     /* bLength */
	USB_DESC_TYPE_DEVICE_QUALIFIER,                 /* bDescriptorType */
	0x00,                                           /* bcdUSB */
	0x02,
	0x00,                                           /* bDeviceClass */
	0x00,                                           /* bDeviceSubClass */
	0x00,                                           /* bDeviceProtocol */
	0x40,                                           /* bMaxPacketSize */
	0x01,                                           /* bNumConfigurations */
	0x00,                                           /* Reserved */
};  /* usbd_vendor_device_qualifier_desc */

/* USB Vendor Device High Speed Configuration Descriptor */
static const u8 usbd_vendor_hs_config_desc[] = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,								/* bLength */
	USB_DESC_TYPE_CONFIGURATION,					/* bDescriptorType */
	0x00,											/* wTotalLength */
	0x00,
	0x01,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
#if USBD_VENDOR_SELF_POWERED
	0xC0,                                           /* bmAttributes: self powered */
#else
	0x80,                                           /* bmAttributes: bus powered */
#endif
	0x32,                                           /* bMaxPower */

	/* Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	0x00,                                           /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x06,                                           /* bNumEndpoints */
	USBD_VENDOR_CLASS_CODE,                         /* bInterfaceClass: Vendor */
	USBD_VENDOR_SUBCLASS_CODE,                      /* bInterfaceSubClass */
	USBD_VENDOR_PROTOCOL,                 			/* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,								/* bLength */
	USB_DESC_TYPE_ENDPOINT,							/* bDescriptorType */
	USBD_VENDOR_BULK_OUT_EP,						/* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,							/* bmAttributes: BULK */
	USB_LOW_BYTE(USBD_VENDOR_HS_BULK_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_HS_BULK_MPS),
	0x00,											/* bInterval */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_VENDOR_BULK_IN_EP,							/* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,							/* bmAttributes: BULK */
	USB_LOW_BYTE(USBD_VENDOR_HS_BULK_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_HS_BULK_MPS),
	0x00,											/* bInterval */

	/* INTR OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_VENDOR_INTR_OUT_EP,						/* bEndpointAddress */
	USB_CH_EP_TYPE_INTR,							/* bmAttributes: INTR */
	USB_LOW_BYTE(USBD_VENDOR_HS_INTR_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_HS_INTR_MPS),
	USBD_VENDOR_HS_INTR_OUT_INTERVAL,				/* bInterval */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_VENDOR_INTR_IN_EP,							/* bEndpointAddress */
	USB_CH_EP_TYPE_INTR,							/* bmAttributes: INTR */
	USB_LOW_BYTE(USBD_VENDOR_HS_INTR_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_HS_INTR_MPS),
	USBD_VENDOR_HS_INTR_IN_INTERVAL,				/* bInterval */

	/* ISOC OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_VENDOR_ISOC_OUT_EP,						/* bEndpointAddress */
	USB_CH_EP_TYPE_ISOC,							/* bmAttributes: ISOC */
	USB_LOW_BYTE(USBD_VENDOR_HS_ISOC_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_HS_ISOC_MPS),
	USBD_VENDOR_HS_ISOC_OUT_INTERVAL,				/* bInterval */

	/* ISOC IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_VENDOR_ISOC_IN_EP,							/* bEndpointAddress */
	USB_CH_EP_TYPE_ISOC,							/* bmAttributes: ISOC */
	USB_LOW_BYTE(USBD_VENDOR_HS_ISOC_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_HS_ISOC_MPS),
	USBD_VENDOR_HS_ISOC_IN_INTERVAL,				/* bInterval */
};  /* usbd_vendor_hs_config_desc */

/* USB Vendor Device Full Speed Configuration Descriptor */
static const u8 usbd_vendor_fs_config_desc[] = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	0x00,											/* wTotalLength */
	0x00,
	0x01,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
#if USBD_VENDOR_SELF_POWERED
	0xC0,                                           /* bmAttributes: self powered */
#else
	0x80,                                           /* bmAttributes: bus powered */
#endif
	0x32,                                           /* bMaxPower */

	/* Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	0x00,                                           /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x06,                                           /* bNumEndpoints */
	USBD_VENDOR_CLASS_CODE,                         /* bInterfaceClass: Vendor */
	USBD_VENDOR_SUBCLASS_CODE,                      /* bInterfaceSubClass*/
	USBD_VENDOR_PROTOCOL,                 			/* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,								/* bLength */
	USB_DESC_TYPE_ENDPOINT,							/* bDescriptorType */
	USBD_VENDOR_BULK_OUT_EP,						/* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,							/* bmAttributes: BULK */
	USB_LOW_BYTE(USBD_VENDOR_FS_BULK_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_FS_BULK_MPS),
	0x00,											/* bInterval */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,								/* bLength */
	USB_DESC_TYPE_ENDPOINT,							/* bDescriptorType */
	USBD_VENDOR_BULK_IN_EP,							/* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,							/* bmAttributes: BULK */
	USB_LOW_BYTE(USBD_VENDOR_FS_BULK_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_FS_BULK_MPS),
	0x00,											/* bInterval */

	/* INTR OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_VENDOR_INTR_OUT_EP,                        /* bEndpointAddress */
	USB_CH_EP_TYPE_INTR,							/* bmAttributes: INTR */
	USB_LOW_BYTE(USBD_VENDOR_FS_INTR_MPS),          /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_FS_INTR_MPS),
	USBD_VENDOR_FS_INTR_OUT_INTERVAL,				/* bInterval */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_VENDOR_INTR_IN_EP,                         /* bEndpointAddress */
	USB_CH_EP_TYPE_INTR,							/* bmAttributes: INTR */
	USB_LOW_BYTE(USBD_VENDOR_FS_INTR_MPS),          /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_FS_INTR_MPS),
	USBD_VENDOR_FS_INTR_IN_INTERVAL,				/* bInterval */

	/* ISOC OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_VENDOR_ISOC_OUT_EP,                        /* bEndpointAddress */
	USB_CH_EP_TYPE_ISOC,                            /* bmAttributes: ISOC */
	USB_LOW_BYTE(USBD_VENDOR_FS_ISOC_MPS),          /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_FS_ISOC_MPS),
	USBD_VENDOR_FS_ISOC_OUT_INTERVAL,               /* bInterval */

	/* ISOC IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_VENDOR_ISOC_IN_EP,                         /* bEndpointAddress */
	USB_CH_EP_TYPE_ISOC,                            /* bmAttributes: ISOC */
	USB_LOW_BYTE(USBD_VENDOR_FS_ISOC_MPS),          /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_FS_ISOC_MPS),
	USBD_VENDOR_FS_ISOC_IN_INTERVAL,                /* bInterval */
};  /* usbd_vendor_fs_config_desc */

/* Vendor Class Driver */
static const usbd_class_driver_t usbd_vendor_driver = {
	.get_descriptor = usbd_vendor_get_descriptor,
	.set_config = usbd_vendor_set_config,
	.clear_config = usbd_vendor_clear_config,
	.setup = usbd_vendor_setup,
	.ep_data_in = usbd_vendor_handle_ep_data_in,
	.ep_data_out = usbd_vendor_handle_ep_data_out,
	.status_changed = usbd_vendor_status_changed,
};

/* Vendor Device */
static usbd_vendor_dev_t usbd_vendor_dev;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Set Vendor class configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_vendor_set_config(usb_dev_t *dev, u8 config)
{
	int ret = HAL_OK;
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &cdev->ep_intr_in;
	usbd_ep_t *ep_intr_out = &cdev->ep_intr_out;
	u8 speed = dev->dev_speed;

	UNUSED(config);

	cdev->dev = dev;
	cdev->alt_setting = 0U;
	/* Init INTR IN EP */
	ep_intr_in->mps = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_INTR_MPS : USBD_VENDOR_FS_INTR_MPS;
	ep_intr_in->binterval = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_INTR_IN_INTERVAL : USBD_VENDOR_FS_INTR_IN_INTERVAL;
	usbd_ep_init(dev, ep_intr_in);
	ep_intr_in->xfer_state = 0;

	/* Init INTR OUT EP */
	ep_intr_out->mps = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_INTR_MPS : USBD_VENDOR_FS_INTR_MPS;
	ep_intr_out->binterval = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_INTR_OUT_INTERVAL : USBD_VENDOR_FS_INTR_OUT_INTERVAL;
	usbd_ep_init(dev, ep_intr_out);
	usbd_ep_receive(dev, ep_intr_out);

	/* Init BULK IN EP */
	ep_bulk_in->mps = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_BULK_MPS : USBD_VENDOR_FS_BULK_MPS;
	usbd_ep_init(dev, ep_bulk_in);
	ep_bulk_in->xfer_state = 0;

	/* Init BULK OUT EP */
	ep_bulk_out->mps = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_BULK_MPS : USBD_VENDOR_FS_BULK_MPS;
	usbd_ep_init(dev, ep_bulk_out);
	usbd_ep_receive(dev, ep_bulk_out);

	/* Init ISO IN EP */
	ep_isoc_in->mps = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_ISOC_MPS : USBD_VENDOR_FS_ISOC_MPS;
	ep_isoc_in->binterval = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_ISOC_IN_INTERVAL : USBD_VENDOR_FS_ISOC_IN_INTERVAL;
	usbd_ep_init(dev, ep_isoc_in);

	/* Init ISO OUT EP */
	ep_isoc_out->mps = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_ISOC_MPS : USBD_VENDOR_FS_ISOC_MPS;
	ep_isoc_out->binterval = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_ISOC_OUT_INTERVAL : USBD_VENDOR_FS_ISOC_OUT_INTERVAL;
	usbd_ep_init(dev, ep_isoc_out);
	usbd_ep_receive(dev, ep_isoc_out);

	if (cdev->cb->set_config != NULL) {
		cdev->cb->set_config();
	}

	return ret;
}

/**
  * @brief  Clear Vendor configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_vendor_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0U;
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &cdev->ep_intr_in;
	usbd_ep_t *ep_intr_out = &cdev->ep_intr_out;

	UNUSED(config);

	/* DeInit INTR IN EP */
	usbd_ep_deinit(dev, ep_intr_in);

	/* DeInit INTR OUT EP */
	usbd_ep_deinit(dev, ep_intr_out);

	/* DeInit BULK IN EP */
	usbd_ep_deinit(dev, ep_bulk_in);

	/* DeInit BULK OUT EP */
	usbd_ep_deinit(dev, ep_bulk_out);

	/* DeInit ISOC IN EP */
	usbd_ep_deinit(dev, ep_isoc_in);

	/* DeInit ISOC OUT EP */
	usbd_ep_deinit(dev, ep_isoc_out);

	return ret;
}

/**
  * @brief  Handle Vendor specific CTRL requests
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_vendor_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	int ret = HAL_OK;
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;

	//RTK_LOGD(TAG, "SETUP: bmRequestType=0x%02x bRequest=0x%02x wLength=0x%04x wValue=%x\n",
	//	 req->bmRequestType, req->bRequest, req->wLength, req->wValue);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				if (req->wIndex == 0U) {
					cdev->alt_setting = USB_LOW_BYTE(req->wValue);
				} else {
					ret = HAL_ERR_HW;
				}
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = cdev->alt_setting;
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
	case USB_REQ_TYPE_CLASS :
	case USB_REQ_TYPE_VENDOR:
		if (req->wLength) {
			if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
				ret = cdev->cb->setup(req, ep0_in->xfer_buf);
				if (ret == HAL_OK) {
					ep0_in->xfer_len = req->wLength;
					usbd_ep_transmit(dev, ep0_in);
				}
			} else {
				usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				ep0_out->xfer_len = req->wLength;
				usbd_ep_receive(dev, ep0_out);
			}
		} else {
			cdev->cb->setup(req, NULL);
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
static int usbd_vendor_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usbd_vendor_cb_t *cb = cdev->cb;
	usbd_ep_t *ep_intr_in = &cdev->ep_intr_in;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	UNUSED(dev);

	if (ep_addr == USBD_VENDOR_INTR_IN_EP) {
		ep_intr_in->xfer_state = 0U;
		if (status == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "INTR TX done\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "INTR TX err: %d\n", status);
		}
		if (cb->intr_transmitted != NULL) {
			cb->intr_transmitted(status);
		}
	}

	if (ep_addr == USBD_VENDOR_BULK_IN_EP) {
		ep_bulk_in->xfer_state = 0U;
		if (status == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "BULK TX done\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "BULK TX err: %d\n", status);
		}
		if (cb->bulk_transmitted != NULL) {
			cb->bulk_transmitted(status);
		}
	}

	if (ep_addr == USBD_VENDOR_ISOC_IN_EP) {
		if (status == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "ISOC TX done\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "ISOC TX err: %d\n", status);
		}
		if (cb->isoc_transmitted != NULL) {
			cb->isoc_transmitted(status);
		}
	}

	return HAL_OK;
}

/**
  * @brief  Data received on non-control Out endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_vendor_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;
	usbd_ep_t *ep_intr_out = &cdev->ep_intr_out;
	UNUSED(dev);

	if (ep_addr == USBD_VENDOR_INTR_OUT_EP) {
		if (len > 0) {
			cdev->cb->intr_received(ep_intr_out->xfer_buf, len);
		}
		usbd_ep_receive(cdev->dev, ep_intr_out);
	}

	if (ep_addr == USBD_VENDOR_BULK_OUT_EP) {
		if (len > 0) {
			cdev->cb->bulk_received(ep_bulk_out->xfer_buf, len);
		}
		usbd_ep_receive(cdev->dev, ep_bulk_out);
	}

	if (ep_addr == USBD_VENDOR_ISOC_OUT_EP) {
		if (len > 0) {
			cdev->cb->isoc_received(ep_isoc_out->xfer_buf, len);
		}
		usbd_ep_receive(cdev->dev, ep_isoc_out);
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
static u16 usbd_vendor_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	usb_speed_type_t speed = dev->dev_speed;
	u8 *desc = NULL;
	u16 len = 0;

	dev->self_powered = USBD_VENDOR_SELF_POWERED;

	switch (USB_HIGH_BYTE(req->wValue)) {

	case USB_DESC_TYPE_DEVICE:
		len = sizeof(usbd_vendor_dev_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_vendor_dev_desc, len);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_vendor_hs_config_desc;
			len = sizeof(usbd_vendor_hs_config_desc);
		} else {
			desc = (u8 *)usbd_vendor_fs_config_desc;
			len = sizeof(usbd_vendor_fs_config_desc);
		}
		usb_os_memcpy((void *)buf, (void *)desc, len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		len = sizeof(usbd_vendor_device_qualifier_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_vendor_device_qualifier_desc, len);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_vendor_fs_config_desc;
			len = sizeof(usbd_vendor_fs_config_desc);
		} else {
			desc = (u8 *)usbd_vendor_hs_config_desc;
			len = sizeof(usbd_vendor_hs_config_desc);
		}
		usb_os_memcpy((void *)buf, (void *)desc, len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		break;

	case USB_DESC_TYPE_STRING:
		switch (USB_LOW_BYTE(req->wValue)) {
		case USBD_IDX_LANGID_STR:
			len = sizeof(usbd_vendor_lang_id_desc);
			usb_os_memcpy((void *)buf, (void *)usbd_vendor_lang_id_desc, len);
			break;
		case USBD_IDX_MFC_STR:
			len = usbd_get_str_desc(USBD_VENDOR_MFG_STRING, buf);
			break;
		case USBD_IDX_PRODUCT_STR:
			if (speed == USB_SPEED_HIGH) {
				len = usbd_get_str_desc(USBD_VENDOR_PROD_HS_STRING, buf);
			} else {
				len = usbd_get_str_desc(USBD_VENDOR_PROD_FS_STRING, buf);
			}
			break;
		case USBD_IDX_SERIAL_STR:
			len = usbd_get_str_desc(USBD_VENDOR_SN_STRING, buf);
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

/**
  * @brief  USB attach status change
  * @param  dev: USB device instance
  * @param  old_status: USB old attach status
  * @param  status: USB USB attach status
  * @retval void
  */
static void usbd_vendor_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;

	UNUSED(dev);

	if (cdev->cb->status_changed) {
		cdev->cb->status_changed(old_status, status);
	}
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize vendor device
  * @retval Status
  */
int usbd_vendor_init(usbd_vendor_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &cdev->ep_intr_in;
	usbd_ep_t *ep_intr_out = &cdev->ep_intr_out;

	ep_bulk_out->addr = USBD_VENDOR_BULK_OUT_EP;
	ep_bulk_out->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_out->xfer_len = USBD_VENDOR_HS_BULK_MPS;
	ep_bulk_out->xfer_buf = (u8 *)usb_os_malloc(USBD_VENDOR_HS_BULK_MPS);
	if (ep_bulk_out->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_exit;
	}

	ep_bulk_in->addr = USBD_VENDOR_BULK_IN_EP;
	ep_bulk_in->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_in->xfer_buf = (u8 *)usb_os_malloc(USBD_VENDOR_HS_BULK_MPS);
	if (ep_bulk_in->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_bulk_out_buf_exit;
	}

	ep_intr_out->addr = USBD_VENDOR_INTR_OUT_EP;
	ep_intr_out->type = USB_CH_EP_TYPE_INTR;
	ep_intr_out->xfer_len = USBD_VENDOR_HS_INTR_MPS;
	ep_intr_out->xfer_buf = (u8 *)usb_os_malloc(USBD_VENDOR_HS_INTR_MPS);
	ep_intr_out->binterval = 1U;
	if (ep_intr_out->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_bulk_in_buf_exit;
	}

	ep_intr_in->addr = USBD_VENDOR_INTR_IN_EP;
	ep_intr_in->type = USB_CH_EP_TYPE_INTR;
	ep_intr_in->xfer_buf = (u8 *)usb_os_malloc(USBD_VENDOR_HS_INTR_MPS);
	ep_intr_in->binterval = 1U;
	if (ep_intr_in->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_intr_out_buf_exit;
	}

	ep_isoc_out->addr = USBD_VENDOR_ISOC_OUT_EP;
	ep_isoc_out->type = USB_CH_EP_TYPE_ISOC;
	ep_isoc_out->xfer_len = USBD_VENDOR_HS_ISOC_MPS;
	ep_isoc_out->xfer_buf = (u8 *)usb_os_malloc(USBD_VENDOR_HS_ISOC_MPS);
	ep_isoc_out->binterval = 1U;
	if (ep_isoc_out->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_intr_in_buf_exit;
	}

	ep_isoc_in->addr = USBD_VENDOR_ISOC_IN_EP;
	ep_isoc_in->type = USB_CH_EP_TYPE_ISOC;
	ep_isoc_in->xfer_buf = (u8 *)usb_os_malloc(USBD_VENDOR_HS_ISOC_MPS);
	ep_isoc_in->binterval = 1U;
	if (ep_isoc_in->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_isoc_out_buf_exit;
	}

	if (cb != NULL) {
		cdev->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				goto init_clean_isoc_in_buf_exit;
			}
		}
	}

	usbd_register_class(&usbd_vendor_driver);

	return ret;

init_clean_isoc_in_buf_exit:
	usb_os_mfree(ep_isoc_in->xfer_buf);
	ep_isoc_in->xfer_buf = NULL;

init_clean_isoc_out_buf_exit:
	usb_os_mfree(ep_isoc_out->xfer_buf);
	ep_isoc_out->xfer_buf = NULL;

init_clean_intr_in_buf_exit:
	usb_os_mfree(ep_intr_in->xfer_buf);
	ep_intr_in->xfer_buf = NULL;

init_clean_intr_out_buf_exit:
	usb_os_mfree(ep_intr_out->xfer_buf);
	ep_intr_out->xfer_buf = NULL;

init_clean_bulk_in_buf_exit:
	usb_os_mfree(ep_bulk_in->xfer_buf);
	ep_bulk_in->xfer_buf = NULL;

init_clean_bulk_out_buf_exit:
	usb_os_mfree(ep_bulk_out->xfer_buf);
	ep_bulk_out->xfer_buf = NULL;

init_exit:
	return ret;
}

/**
  * @brief  DeInitialize vendor device
  * @param  void
  * @retval Status
  */
int usbd_vendor_deinit(void)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &cdev->ep_intr_in;
	usbd_ep_t *ep_intr_out = &cdev->ep_intr_out;

	u8 is_busy = ep_intr_in->is_busy;

	while (is_busy) {
		usb_os_delay_us(100);
	}

	if (cdev->cb != NULL) {
		if (cdev->cb->deinit != NULL) {
			cdev->cb->deinit();
		}
		cdev->cb = NULL;
	}

	usbd_unregister_class();

	if (ep_bulk_in->xfer_buf != NULL) {
		usb_os_mfree(ep_bulk_in->xfer_buf);
		ep_bulk_in->xfer_buf = NULL;
	}

	if (ep_bulk_out->xfer_buf != NULL) {
		usb_os_mfree(ep_bulk_out->xfer_buf);
		ep_bulk_out->xfer_buf = NULL;
	}

	if (ep_intr_in->xfer_buf != NULL) {
		usb_os_mfree(ep_intr_in->xfer_buf);
		ep_intr_in->xfer_buf = NULL;
	}

	if (ep_intr_out->xfer_buf != NULL) {
		usb_os_mfree(ep_intr_out->xfer_buf);
		ep_intr_out->xfer_buf = NULL;
	}

	if (ep_isoc_in->xfer_buf != NULL) {
		usb_os_mfree(ep_isoc_in->xfer_buf);
		ep_isoc_in->xfer_buf = NULL;
	}

	if (ep_isoc_out->xfer_buf != NULL) {
		usb_os_mfree(ep_isoc_out->xfer_buf);
		ep_isoc_out->xfer_buf = NULL;
	}

	return HAL_OK;
}

int usbd_vendor_transmit_bulk_data(u8 *buf, u16 len)
{
	int ret = HAL_OK;
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;

	if (!dev->is_ready) {
		return HAL_ERR_HW;
	}

	if (len > ep_bulk_in->mps) {
		len = ep_bulk_in->mps;
	}

	if (ep_bulk_in->xfer_state == 0U) {
		ep_bulk_in->xfer_state = 1U;
		usb_os_memcpy((void *)ep_bulk_in->xfer_buf, (void *)buf, len);
		ep_bulk_in->xfer_len = len;
		usbd_ep_transmit(dev, ep_bulk_in);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "BULK TX len %d data %d \n", len, buf[0]);
	} else {
		ret = HAL_BUSY;
	}

	return ret;
}

int usbd_vendor_transmit_intr_data(u8 *buf, u16 len)
{
	int ret = HAL_OK;
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_ep_t *ep_intr_in = &cdev->ep_intr_in;

	if (!dev->is_ready) {
		return HAL_ERR_HW;
	}

	if (len > ep_intr_in->mps) {
		len = ep_intr_in->mps;
	}

	if (ep_intr_in->xfer_state == 0U) {
		ep_intr_in->xfer_state = 1U;
		usb_os_memcpy((void *)ep_intr_in->xfer_buf, (void *)buf, len);
		ep_intr_in->xfer_len = len;
		usbd_ep_transmit(dev, ep_intr_in);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "INTR TX len %d data %d \n", len, buf[0]);
	} else {
		ret = HAL_BUSY;
	}

	return ret;
}

int usbd_vendor_transmit_isoc_data(u8 *buf, u16 len)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;

	if (!dev->is_ready) {
		return HAL_ERR_HW;
	}

	if (len > ep_isoc_in->mps) {
		len = ep_isoc_in->mps;
	}

	usb_os_memcpy(ep_isoc_in->xfer_buf, buf, len);
	ep_isoc_in->xfer_len = len;
	usbd_ep_transmit(cdev->dev, ep_isoc_in);
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "ISOC TX len %d data %d \n", len, buf[0]);
	return HAL_OK;
}
