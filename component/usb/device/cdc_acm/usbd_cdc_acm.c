/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_cdc_acm.h"
#include "usbd.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int cdc_acm_set_config(usb_dev_t *dev, u8 config);
static int cdc_acm_clear_config(usb_dev_t *dev, u8 config);
static int cdc_acm_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u8 *cdc_acm_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len);
static int cdc_acm_handle_ep0_data_out(usb_dev_t *dev);
static int cdc_acm_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int cdc_acm_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static void cdc_acm_status_changed(usb_dev_t *dev, u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "ACM";

/* USB Standard Device Descriptor */
static u8 usbd_cdc_acm_dev_desc[USB_LEN_DEV_DESC] USB_DMA_ALIGNED = {
	USB_LEN_DEV_DESC,                               /* bLength */
	USB_DESC_TYPE_DEVICE,                           /* bDescriptorType */
	0x00,                                           /* bcdUSB */
	0x02,
	0x02,                                           /* bDeviceClass */
	0x02,                                           /* bDeviceSubClass */
	0x00,                                           /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize */
	USB_LOW_BYTE(CDC_ACM_VID),                      /* idVendor */
	USB_HIGH_BYTE(CDC_ACM_VID),
	USB_LOW_BYTE(CDC_ACM_PID),                      /* idProduct */
	USB_HIGH_BYTE(CDC_ACM_PID),
	0x00,                                           /* bcdDevice */
	0x02,
	USBD_IDX_MFC_STR,                               /* iManufacturer */
	USBD_IDX_PRODUCT_STR,                           /* iProduct */
	USBD_IDX_SERIAL_STR,                            /* iSerialNumber */
	0x01                                            /* bNumConfigurations */
};  /* usbd_cdc_acm_dev_desc */

/* USB Standard String Descriptor 0 */
static u8 usbd_cdc_acm_lang_id_desc[USB_LEN_LANGID_STR_DESC] USB_DMA_ALIGNED = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(CDC_ACM_LANGID_STRING),            /* wLANGID */
	USB_HIGH_BYTE(CDC_ACM_LANGID_STRING),
};  /* usbd_cdc_acm_lang_id_desc */

/* USB Standard Device Qualifier Descriptor */
static u8 usbd_cdc_acm_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] USB_DMA_ALIGNED = {
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
};  /* usbd_cdc_acm_device_qualifier_desc */

/* USB CDC ACM Device High Speed Configuration Descriptor */
static u8 usbd_cdc_acm_hs_config_desc[CDC_ACM_CONFIG_DESC_SIZE] USB_DMA_ALIGNED = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	CDC_ACM_CONFIG_DESC_SIZE,                       /* wTotalLength */
	0x00,
	0x02,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
#if CDC_ACM_SELF_POWERED
	0xE0,                                           /* bmAttributes: self powered Bit 7: Reserved (set to one) 1 Bit 6: Self-powered 1 Bit 5: Remote Wakeup 0 */
#else
	0xA0,                                           /* bmAttributes: bus powered */
#endif
	0x32,                                           /* bMaxPower */

	/* CDC Communication Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	0x00,                                           /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x01,                                           /* bNumEndpoints */
	0x02,                                           /* bInterfaceClass: CDC */
	0x02,                                           /* bInterfaceSubClass: Abstract Control Model */
	0x01,                                           /* bInterfaceProtocol: Common AT commands */
	0x00,                                           /* iInterface */

	/* CDC Header Functional Descriptor */
	0x05,                                           /* bLength */
	0x24,                                           /* bDescriptorType: CS_INTERFACE */
	0x00,                                           /* bDescriptorSubtype: Header Functional Descriptor */
	0x10,                                           /* bcdCDC */
	0x01,

	/* CDC Call Management Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	0x24,                                           /* bDescriptorType: CS_INTERFACE */
	0x01,                                           /* bDescriptorSubtype: Call Management Functional Descriptor */
	0x00,                                           /* bmCapabilities: D0+D1 */
	0x01,                                           /* bDataInterface */

	/* CDC ACM Functional Descriptor */
	0x04,                                           /* bFunctionLength */
	0x24,                                           /* bDescriptorType: CS_INTERFACE */
	0x02,                                           /* bDescriptorSubtype: ACM Functional Descriptor */
	0x02,                                           /* bmCapabilities */

	/* CDC Union Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	0x24,                                           /* bDescriptorType: CS_INTERFACE */
	0x06,                                           /* bDescriptorSubtype: Union Functional Descriptor */
	0x00,                                           /* bMasterInterface: Communication Class Interface */
	0x01,                                           /* bSlaveInterface0: Data Class Interface */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	CDC_ACM_INTR_IN_EP,                             /* bEndpointAddress */
	USB_CH_EP_TYPE_INTR,                            /* bmAttributes: INTR */
	USB_LOW_BYTE(CDC_ACM_INTR_IN_PACKET_SIZE),      /* wMaxPacketSize */
	USB_HIGH_BYTE(CDC_ACM_INTR_IN_PACKET_SIZE),
	CDC_ACM_HS_INTR_IN_INTERVAL,                    /* bInterval: */

	/* CDC Data Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType: */
	0x01,                                           /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x02,                                           /* bNumEndpoints */
	0x0A,                                           /* bInterfaceClass: CDC */
	0x00,                                           /* bInterfaceSubClass */
	0x00,                                           /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	CDC_ACM_BULK_OUT_EP,                            /* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes: BULK */
	USB_LOW_BYTE(CDC_ACM_HS_BULK_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(CDC_ACM_HS_BULK_MAX_PACKET_SIZE),
	0x00,                                           /* bInterval */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	CDC_ACM_BULK_IN_EP,                             /* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes: BULK */
	USB_LOW_BYTE(CDC_ACM_HS_BULK_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(CDC_ACM_HS_BULK_MAX_PACKET_SIZE),
	0x00                                            /* bInterval */
};  /* usbd_cdc_acm_hs_config_desc */

/* USB CDC ACM Device Full Speed Configuration Descriptor */
static u8 usbd_cdc_acm_fs_config_desc[CDC_ACM_CONFIG_DESC_SIZE] USB_DMA_ALIGNED = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	CDC_ACM_CONFIG_DESC_SIZE,                       /* wTotalLength */
	0x00,
	0x02,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
#if CDC_ACM_SELF_POWERED
	0xE0,                                           /* bmAttributes: self powered Bit 7: Reserved (set to one) 1 Bit 6: Self-powered 1 Bit 5: Remote Wakeup 0 */
#else
	0xA0,                                           /* bmAttributes: bus powered */
#endif
	0x32,                                           /* bMaxPower */

	/* CDC Communication Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	0x00,                                           /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x01,                                           /* bNumEndpoints */
	0x02,                                           /* bInterfaceClass: CDC */
	0x02,                                           /* bInterfaceSubClass: Abstract Control Model */
	0x01,                                           /* bInterfaceProtocol: Common AT commands */
	0x00,                                           /* iInterface */

	/* CDC Header Functional Descriptor */
	0x05,                                           /* bLength */
	0x24,                                           /* bDescriptorType: CS_INTERFACE */
	0x00,                                           /* bDescriptorSubtype: Header Functional Descriptor */
	0x10,                                           /* bcdCDC */
	0x01,

	/* CDC Call Management Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	0x24,                                           /* bDescriptorType: CS_INTERFACE */
	0x01,                                           /* bDescriptorSubtype: Call Management Functional Descriptor */
	0x00,                                           /* bmCapabilities: D0+D1 */
	0x01,                                           /* bDataInterface */

	/* CDC ACM Functional Descriptor */
	0x04,                                           /* bFunctionLength */
	0x24,                                           /* bDescriptorType: CS_INTERFACE */
	0x02,                                           /* bDescriptorSubtype: ACM Functional Descriptor */
	0x02,                                           /* bmCapabilities */

	/* CDC Union Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	0x24,                                           /* bDescriptorType: CS_INTERFACE */
	0x06,                                           /* bDescriptorSubtype: Union Functional Descriptor */
	0x00,                                           /* bMasterInterface: Communication Class Interface */
	0x01,                                           /* bSlaveInterface0: Data Class Interface */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	CDC_ACM_INTR_IN_EP,                             /* bEndpointAddress */
	USB_CH_EP_TYPE_INTR,                            /* bmAttributes: INTR */
	USB_LOW_BYTE(CDC_ACM_INTR_IN_PACKET_SIZE),      /* wMaxPacketSize */
	USB_HIGH_BYTE(CDC_ACM_INTR_IN_PACKET_SIZE),
	CDC_ACM_FS_INTR_IN_INTERVAL,                    /* bInterval: */

	/* CDC Data Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType: */
	0x01,                                           /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x02,                                           /* bNumEndpoints */
	0x0A,                                           /* bInterfaceClass: CDC */
	0x00,                                           /* bInterfaceSubClass */
	0x00,                                           /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	CDC_ACM_BULK_OUT_EP,                            /* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes: BULK */
	USB_LOW_BYTE(CDC_ACM_FS_BULK_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(CDC_ACM_FS_BULK_MAX_PACKET_SIZE),
	0x00,                                           /* bInterval */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	CDC_ACM_BULK_IN_EP,                             /* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes: BULK */
	USB_LOW_BYTE(CDC_ACM_FS_BULK_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(CDC_ACM_FS_BULK_MAX_PACKET_SIZE),
	0x00                                            /* bInterval */
};  /* usbd_cdc_acm_fs_config_desc */

/* CDC ACM Class Driver */
usbd_class_driver_t usbd_cdc_driver = {
	.get_descriptor = cdc_acm_get_descriptor,
	.set_config = cdc_acm_set_config,
	.clear_config = cdc_acm_clear_config,
	.setup = cdc_acm_setup,
	.ep0_data_out = cdc_acm_handle_ep0_data_out,
	.ep_data_in = cdc_acm_handle_ep_data_in,
	.ep_data_out = cdc_acm_handle_ep_data_out,
	.status_changed = cdc_acm_status_changed,
};

/* CDC ACM Device */
static usbd_cdc_acm_dev_t usbd_cdc_acm_dev;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Set CDC class configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int cdc_acm_set_config(usb_dev_t *dev, u8 config)
{
	u16 ep_mps;
	int ret = HAL_OK;
	usbd_cdc_acm_dev_t *cdev = &usbd_cdc_acm_dev;

	UNUSED(config);

	cdev->dev = dev;

	/* Init BULK IN state */
	cdev->bulk_in_state = 0U;
	cdev->bulk_out_zlp = 0U;

	/* Init BULK IN EP */
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? CDC_ACM_HS_BULK_IN_PACKET_SIZE : CDC_ACM_FS_BULK_IN_PACKET_SIZE;
	usbd_ep_init(dev, CDC_ACM_BULK_IN_EP, USB_CH_EP_TYPE_BULK, ep_mps);

	/* Init BULK OUT EP */
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? CDC_ACM_HS_BULK_OUT_PACKET_SIZE : CDC_ACM_FS_BULK_OUT_PACKET_SIZE;
	usbd_ep_init(dev, CDC_ACM_BULK_OUT_EP, USB_CH_EP_TYPE_BULK, ep_mps);

	/* Init INTR IN EP */
	usbd_ep_init(dev, CDC_ACM_INTR_IN_EP, USB_CH_EP_TYPE_INTR, CDC_ACM_INTR_IN_PACKET_SIZE);
#if CONFIG_CDC_ACM_NOTIFY
	cdev->intr_in_state = 0U;
#endif

	/* Prepare to receive next BULK OUT packet */
	usbd_ep_receive(dev, CDC_ACM_BULK_OUT_EP, cdev->bulk_out_buf, cdev->bulk_out_buf_size);

	return ret;
}

/**
  * @brief  Clear CDC ACM configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int cdc_acm_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0U;

	UNUSED(config);

	/* DeInit BULK IN EP */
	usbd_ep_deinit(dev, CDC_ACM_BULK_IN_EP);

	/* DeInit BULK OUT EP */
	usbd_ep_deinit(dev, CDC_ACM_BULK_OUT_EP);

	/* DeInit INTR IN EP */
	usbd_ep_deinit(dev, CDC_ACM_INTR_IN_EP);

	return ret;
}

/**
  * @brief  Handle CDC specific CTRL requests
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int cdc_acm_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_cdc_acm_dev_t *cdev = &usbd_cdc_acm_dev;
	int ret = HAL_OK;

	// RTK_LOGD(TAG, "SETUP: bmRequestType=0x%02x bRequest=0x%02x wLength=0x%04x wValue=%x\n",
	//       			req->bmRequestType, req->bRequest, req->wLength, req->wValue);

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
			ret = HAL_ERR_PARA;
			break;
		}
		break;
	case USB_REQ_TYPE_CLASS:
		if (req->wLength) {
			if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
				ret = cdev->cb->setup(req, cdev->ctrl_buf);
				if (ret == HAL_OK) {
					usbd_ep0_transmit(dev, cdev->ctrl_buf, req->wLength);
				}
			} else {
				usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
			}
		} else {
			cdev->cb->setup(req, cdev->ctrl_buf);
		}
		break;
	default:
		ret = HAL_ERR_HW;
		break;
	}

	return ret;
}

/**
  * @brief  Transmit BULK IN ZLP packet
  * @retval Status
  */
static int cdc_acm_transmit_zlp(void)
{
	usbd_cdc_acm_dev_t *cdev = &usbd_cdc_acm_dev;
	usb_dev_t *dev = cdev->dev;

	usbd_ep_transmit(dev, CDC_ACM_BULK_IN_EP, NULL, 0);

	return HAL_OK;
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int cdc_acm_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_cdc_acm_dev_t *cdev = &usbd_cdc_acm_dev;

	UNUSED(dev);

	if (status == HAL_OK) {
		if (ep_addr == CDC_ACM_BULK_IN_EP) {
			if (cdev->bulk_out_zlp) {
				cdev->bulk_out_zlp = 0;
				cdc_acm_transmit_zlp();
			} else {
				cdev->bulk_in_state = 0U;
				if (cdev->cb->transmitted) {
					cdev->cb->transmitted(status);
				}
			}
		}
#if CONFIG_CDC_ACM_NOTIFY
		else if (ep_addr == CDC_ACM_INTR_IN_EP) {
			cdev->intr_in_state = 0U;
#if CONFIG_CDC_ACM_NOTIFY_LOOP_TEST
			usbd_cdc_acm_notify_serial_state(cdev->intr_notify_idx++);
#endif
		}
#endif
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x TX fail: %d\n", ep_addr, status);
		if (ep_addr == CDC_ACM_BULK_IN_EP) {
			cdev->bulk_in_state = 0U;
			if (cdev->cb->transmitted) {
				cdev->cb->transmitted(status);
			}
		}
#if CONFIG_CDC_ACM_NOTIFY
		else if (ep_addr == CDC_ACM_INTR_IN_EP) {
			cdev->intr_in_state = 0U;
#if CONFIG_CDC_ACM_NOTIFY_LOOP_TEST
			usbd_cdc_acm_notify_serial_state(cdev->intr_notify_idx++);
#endif
		}
#endif
	}

	return HAL_OK;
}

/**
  * @brief  Data received on non-control Out endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int cdc_acm_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len)
{
	usbd_cdc_acm_dev_t *cdev = &usbd_cdc_acm_dev;

	UNUSED(dev);

	if ((ep_addr == CDC_ACM_BULK_OUT_EP) && (len > 0)) {
		cdev->cb->received(cdev->bulk_out_buf, len);
	}

	usbd_ep_receive(cdev->dev, CDC_ACM_BULK_OUT_EP, cdev->bulk_out_buf, cdev->bulk_out_buf_size);

	return HAL_OK;
}

/**
  * @brief  cdc_acm_handle_ep0_data_out
  *         Handle EP0 Rx Ready event
  * @param  dev: USB device instance
  * @retval Status
  */
static int cdc_acm_handle_ep0_data_out(usb_dev_t *dev)
{
	int ret = HAL_ERR_HW;
	usbd_cdc_acm_dev_t *cdev = &usbd_cdc_acm_dev;

	UNUSED(dev);

	if (cdev->ctrl_req.bRequest != 0xFFU) {
		cdev->cb->setup(&cdev->ctrl_req, cdev->ctrl_buf);
		cdev->ctrl_req.bRequest = 0xFFU;

		ret = HAL_OK;
	}

	return ret;
}

static u8 *cdc_acm_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len)
{
	u8 *buf = NULL;
	u8 *desc = usbd_cdc_acm_dev.ctrl_buf;

	dev->self_powered = CDC_ACM_SELF_POWERED;
	dev->remote_wakeup_en = CDC_ACM_REMOTE_WAKEUP_EN;

	switch ((req->wValue >> 8) & 0xFF) {

	case USB_DESC_TYPE_DEVICE:
		buf = usbd_cdc_acm_dev_desc;
		*len = sizeof(usbd_cdc_acm_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			buf = usbd_cdc_acm_hs_config_desc;
			*len = sizeof(usbd_cdc_acm_hs_config_desc);
		} else {
			buf = usbd_cdc_acm_fs_config_desc;
			*len = sizeof(usbd_cdc_acm_fs_config_desc);
		}
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		buf = usbd_cdc_acm_device_qualifier_desc;
		*len = sizeof(usbd_cdc_acm_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			buf = usbd_cdc_acm_fs_config_desc;
		} else {
			buf = usbd_cdc_acm_hs_config_desc;
		}

		*len = CDC_ACM_CONFIG_DESC_SIZE;

		usb_os_memcpy((void *)desc, (void *)buf, *len);
		desc[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		buf = desc;

		break;

	case USB_DESC_TYPE_STRING:
		switch (req->wValue & 0xFF) {
		case USBD_IDX_LANGID_STR:
			buf = usbd_cdc_acm_lang_id_desc;
			*len = sizeof(usbd_cdc_acm_lang_id_desc);
			break;
		case USBD_IDX_MFC_STR:
			usbd_get_str_desc(CDC_ACM_MFG_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_PRODUCT_STR:
			if (speed == USB_SPEED_HIGH) {
				usbd_get_str_desc(CDC_ACM_PROD_HS_STRING, desc, len);
			} else {
				usbd_get_str_desc(CDC_ACM_PROD_FS_STRING, desc, len);
			}
			buf = desc;
			break;
		case USBD_IDX_SERIAL_STR:
			usbd_get_str_desc(CDC_ACM_SN_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_MS_OS_STR:
			break;
		/* Add customer string here */
		default:
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid str idx %d\n", req->wValue & 0xFF);
			break;
		}
		break;

	default:
		break;
	}

	return buf;
}

/**
  * @brief  USB attach status change
  * @param  dev: USB device instance
  * @param  old_status: USB old attach status
  * @param  status: USB attach status
  * @retval void
  */
static void cdc_acm_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_cdc_acm_dev_t *cdev = &usbd_cdc_acm_dev;

	UNUSED(dev);

	if (cdev->cb->status_changed) {
		cdev->cb->status_changed(old_status, status);
	}
}

#if CONFIG_CDC_ACM_NOTIFY
/**
  * @brief  Transmit INTR IN packet
  * @param  type: notification type
  * @param  value: notification value
  * @param  buf: data buffer
  * @param  len: data length
  * @retval Status
  */
static int usbd_acm_cdc_notify(u8 type, u16 value, void *data, u16 len)
{
	u8 ret = HAL_ERR_HW;
	usbd_cdc_acm_dev_t *cdev = &usbd_cdc_acm_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_cdc_acm_ntf_t *ntf = cdev->intr_in_buf;

	if (!dev->is_ready) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x TX %d not ready\n", CDC_ACM_INTR_IN_EP, len);
		return ret;
	}

	if (len > CDC_ACM_INTR_IN_DATA_SIZE) {
		len = CDC_ACM_INTR_IN_DATA_SIZE;
	}

	if (cdev->intr_in_state == 0U) {
		if (dev->is_ready) {
			cdev->is_intr_in_busy = 1U;
			cdev->intr_in_state = 1U;

			ntf->bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
			ntf->bNotificationType = type;
			ntf->wValue = value;
			ntf->wIndex = 0;
			ntf->wLength = len;

			usb_os_memcpy((void *)ntf->buf, (void *)data, len);

			if (dev->is_ready) {
				usbd_ep_transmit(dev, CDC_ACM_INTR_IN_EP, (u8 *)ntf, CDC_ACM_INTR_IN_REQUEST_SIZE + len);
				ret = HAL_OK;
			} else {
				cdev->intr_in_state = 0U;
			}

			cdev->is_intr_in_busy = 0U;
		} else  {
			/*TX not ready*/
		}
	} else {
		/*TX busy*/
		ret = HAL_BUSY;
	}

	return ret;
}
#endif

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init CDC ACM class
  * @param  cb: CDC ACM user callback
  * @retval Status
  */
int usbd_cdc_acm_init(u16 bulk_out_xfer_size, u16 bulk_in_xfer_size, usbd_cdc_acm_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_cdc_acm_dev_t *cdc = &usbd_cdc_acm_dev;

	cdc->ctrl_buf = (u8 *)usb_os_malloc(CDC_ACM_CTRL_BUF_SIZE);
	if (cdc->ctrl_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto USBD_CDC_Init_exit;
	}

	cdc->bulk_out_zlp = 0U;

	cdc->bulk_out_buf_size = bulk_out_xfer_size;
	cdc->bulk_out_buf = (u8 *)usb_os_malloc(cdc->bulk_out_buf_size);
	if (cdc->bulk_out_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto USBD_CDC_Init_clean_ctrl_buf_exit;
	}

	cdc->bulk_in_buf_size = bulk_in_xfer_size;
	cdc->bulk_in_buf = (u8 *)usb_os_malloc(cdc->bulk_in_buf_size);
	if (cdc->bulk_in_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto USBD_CDC_Init_clean_bulk_out_buf_exit;
	}

#if CONFIG_CDC_ACM_NOTIFY
	cdc->intr_in_buf = (usbd_cdc_acm_ntf_t *)usb_os_malloc(sizeof(usbd_cdc_acm_ntf_t));
	if (cdc->intr_in_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto USBD_CDC_Init_clean_bulk_in_buf_exit;
	}
#endif

	if (cb != NULL) {
		cdc->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				goto USBD_CDC_Init_clean_cb_init_exit;
			}
		}
	}

	usbd_register_class(&usbd_cdc_driver);

	return ret;

USBD_CDC_Init_clean_cb_init_exit:

#if CONFIG_CDC_ACM_NOTIFY
	usb_os_mfree(cdc->intr_in_buf);
	cdc->intr_in_buf = NULL;

USBD_CDC_Init_clean_bulk_in_buf_exit:
#endif

	usb_os_mfree(cdc->bulk_in_buf);
	cdc->bulk_in_buf = NULL;

USBD_CDC_Init_clean_bulk_out_buf_exit:
	usb_os_mfree(cdc->bulk_out_buf);
	cdc->bulk_out_buf = NULL;

USBD_CDC_Init_clean_ctrl_buf_exit:
	usb_os_mfree(cdc->ctrl_buf);
	cdc->ctrl_buf = NULL;

USBD_CDC_Init_exit:
	return ret;
}

/**
  * @brief  DeInit CDC ACM class
  * @param  void
  * @retval Status
  */
int usbd_cdc_acm_deinit(void)
{
	u8 is_busy;
	usbd_cdc_acm_dev_t *cdev = &usbd_cdc_acm_dev;

#if CONFIG_CDC_ACM_NOTIFY
	is_busy = cdev->is_bulk_in_busy || cdev->is_intr_in_busy;
#else
	is_busy = cdev->is_bulk_in_busy;
#endif
	while (is_busy) {
		usb_os_delay_us(100);
	}

	usbd_unregister_class();

	if ((cdev->cb != NULL) && (cdev->cb->deinit != NULL)) {
		cdev->cb->deinit();
	}

#if CONFIG_CDC_ACM_NOTIFY
	if (cdev->intr_in_buf != NULL) {
		usb_os_mfree(cdev->intr_in_buf);
		cdev->intr_in_buf = NULL;
	}
#endif

	if (cdev->bulk_in_buf != NULL) {
		usb_os_mfree(cdev->bulk_in_buf);
		cdev->bulk_in_buf = NULL;
	}

	if (cdev->bulk_out_buf != NULL) {
		usb_os_mfree(cdev->bulk_out_buf);
		cdev->bulk_out_buf = NULL;
	}

	if (cdev->ctrl_buf != NULL) {
		usb_os_mfree(cdev->ctrl_buf);
		cdev->ctrl_buf = NULL;
	}

	return HAL_OK;
}

/**
  * @brief  Transmit BULK IN packet
  * @param  buf: data buffer
  * @param  len: data length
  * @retval Status
  */
int usbd_cdc_acm_transmit(u8       *buf, u16 len)
{
	int ret = HAL_ERR_HW;
	u16 ep_mps;
	usbd_cdc_acm_dev_t *cdev = &usbd_cdc_acm_dev;
	usb_dev_t *dev = cdev->dev;

	if (!dev->is_ready) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x TX %d not ready\n", CDC_ACM_BULK_IN_EP, len);
		return ret;
	}

	/* limited the transfer size to the length of the bulk_in_buf(bulk_in_buf_size) */
	if (len > cdev->bulk_in_buf_size) {
		len = cdev->bulk_in_buf_size;
	}

	/* As per USB SPEC for bulk transfer, the transfer ends with a ZLP or a packet whose size
	 * is less than the endpoint max packet size.
	 */
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? CDC_ACM_HS_BULK_IN_PACKET_SIZE : CDC_ACM_FS_BULK_IN_PACKET_SIZE;
	if ((len % ep_mps) == 0) {
		cdev->bulk_out_zlp = 1;
	} else {
		cdev->bulk_out_zlp = 0;
	}

	if (cdev->bulk_in_state == 0U) {
		if (dev->is_ready) {
			cdev->is_bulk_in_busy = 1U;
			cdev->bulk_in_state = 1U;

			usb_os_memcpy((void *)cdev->bulk_in_buf, (void *)buf, len);

			if (dev->is_ready) {
				usbd_ep_transmit(dev, CDC_ACM_BULK_IN_EP, cdev->bulk_in_buf, len);
				ret = HAL_OK;
			} else {
				cdev->bulk_in_state = 0U;
			}

			cdev->is_bulk_in_busy = 0U;
		} else  {
			/*TX not ready*/
		}
	} else {
		ret = HAL_BUSY;
	}

	return ret;
}

#if CONFIG_CDC_ACM_NOTIFY
int usbd_cdc_acm_notify_serial_state(u16 serial_state)
{
	int ret = 0;
	usbd_cdc_acm_dev_t *cdev = &usbd_cdc_acm_dev;
	usb_dev_t *dev = cdev->dev;

	if (dev->is_ready) {
		ret = usbd_acm_cdc_notify(CDC_NOTIFY_SERIAL_STATE, 0, &serial_state, sizeof(serial_state));
	}

	return ret;
}
#endif

