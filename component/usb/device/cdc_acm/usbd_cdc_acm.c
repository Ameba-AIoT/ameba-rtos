/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_cdc_acm.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int cdc_acm_set_config(usb_dev_t *dev, u8 config);
static int cdc_acm_clear_config(usb_dev_t *dev, u8 config);
static int cdc_acm_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 cdc_acm_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int cdc_acm_handle_ep0_data_out(usb_dev_t *dev);
static int cdc_acm_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int cdc_acm_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static void cdc_acm_status_changed(usb_dev_t *dev, u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "ACM";

/* USB Standard Device Descriptor */
static const u8 usbd_cdc_acm_dev_desc[USB_LEN_DEV_DESC] = {
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
static const u8 usbd_cdc_acm_lang_id_desc[USB_LEN_LANGID_STR_DESC] = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(CDC_ACM_LANGID_STRING),            /* wLANGID */
	USB_HIGH_BYTE(CDC_ACM_LANGID_STRING),
};  /* usbd_cdc_acm_lang_id_desc */

/* USB Standard Device Qualifier Descriptor */
static const u8 usbd_cdc_acm_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
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
static const u8 usbd_cdc_acm_hs_config_desc[] = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	0x00,                                           /* wTotalLength */
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
static const u8 usbd_cdc_acm_fs_config_desc[] = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	0x00,                                           /* wTotalLength */
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
static const usbd_class_driver_t usbd_cdc_driver = {
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
	int ret = HAL_OK;
	usbd_cdc_acm_dev_t *cdev = &usbd_cdc_acm_dev;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;
#if CONFIG_CDC_ACM_NOTIFY
	usbd_ep_t *ep_intr_in = &cdev->ep_intr_in;
#endif

	UNUSED(config);

	cdev->dev = dev;

	/* Init BULK IN EP */
	ep_bulk_in->xfer_state = 0U;
	ep_bulk_in->mps = (dev->dev_speed == USB_SPEED_HIGH) ? CDC_ACM_HS_BULK_IN_PACKET_SIZE : CDC_ACM_FS_BULK_IN_PACKET_SIZE;
	usbd_ep_init(dev, ep_bulk_in);

	/* Init BULK OUT EP */
	ep_bulk_out->mps = (dev->dev_speed == USB_SPEED_HIGH) ? CDC_ACM_HS_BULK_OUT_PACKET_SIZE : CDC_ACM_FS_BULK_OUT_PACKET_SIZE;
	usbd_ep_init(dev, ep_bulk_out);

#if CONFIG_CDC_ACM_NOTIFY
	/* Init INTR IN EP */
	ep_intr_in->xfer_state = 0U;
	ep_intr_in->mps = CDC_ACM_INTR_IN_PACKET_SIZE;
	usbd_ep_init(dev, ep_intr_in);
#endif

	if ((ep_bulk_out->skip_dcache_pre_clean) && (ep_bulk_out->xfer_buf != NULL) && (ep_bulk_out->xfer_len != 0)) {
		if (USB_IS_MEM_DMA_ALIGNED(ep_bulk_out->xfer_buf)) {
			DCache_Clean((u32)ep_bulk_out->xfer_buf, ep_bulk_out->xfer_len);
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "RX buf align err\n");
			return HAL_ERR_MEM;
		}
	}

	/* Prepare to receive next BULK OUT packet */
	usbd_ep_receive(dev, ep_bulk_out);

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
	usbd_cdc_acm_dev_t *cdev = &usbd_cdc_acm_dev;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;
#if CONFIG_CDC_ACM_NOTIFY
	usbd_ep_t *ep_intr_in = &cdev->ep_intr_in;
#endif

	UNUSED(config);

	/* DeInit BULK IN EP */
	usbd_ep_deinit(dev, ep_bulk_in);

	/* DeInit BULK OUT EP */
	usbd_ep_deinit(dev, ep_bulk_out);

#if CONFIG_CDC_ACM_NOTIFY
	/* DeInit INTR IN EP */
	usbd_ep_deinit(dev, ep_intr_in);
#endif

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
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	int ret = HAL_OK;

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
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_len = 1U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
			}

			break;

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
			ret = HAL_ERR_PARA;
			break;
		}
		break;
	case USB_REQ_TYPE_CLASS:
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
static int cdc_acm_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_cdc_acm_dev_t *cdev = &usbd_cdc_acm_dev;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
#if CONFIG_CDC_ACM_NOTIFY
	usbd_ep_t *ep_intr_in = &cdev->ep_intr_in;
#endif

	UNUSED(dev);

	if (status == HAL_OK) {
		if (ep_addr == CDC_ACM_BULK_IN_EP) {
			ep_bulk_in->xfer_state = 0U;
			if (cdev->cb->transmitted) {
				cdev->cb->transmitted(status);
			}
		}
#if CONFIG_CDC_ACM_NOTIFY
		else if (ep_addr == CDC_ACM_INTR_IN_EP) {
			ep_intr_in->xfer_state = 0U;
#if CONFIG_CDC_ACM_NOTIFY_LOOP_TEST
			usbd_cdc_acm_notify_serial_state(cdev->intr_notify_idx++);
#endif
		}
#endif
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x TX fail: %d\n", ep_addr, status);
		if (ep_addr == CDC_ACM_BULK_IN_EP) {
			ep_bulk_in->xfer_state = 0U;
			if (cdev->cb->transmitted) {
				cdev->cb->transmitted(status);
			}
		}
#if CONFIG_CDC_ACM_NOTIFY
		else if (ep_addr == CDC_ACM_INTR_IN_EP) {
			ep_intr_in->xfer_state = 0U;
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
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;

	UNUSED(dev);

	if ((ep_bulk_out->skip_dcache_post_invalidate) && (ep_bulk_out->xfer_buf != NULL) && (len != 0)) {
		DCache_Invalidate((u32)ep_bulk_out->xfer_buf, len);
	}

	if ((ep_addr == CDC_ACM_BULK_OUT_EP) && (len > 0)) {
		cdev->cb->received(ep_bulk_out->xfer_buf, len);
	}

	if ((ep_bulk_out->skip_dcache_pre_clean) && (ep_bulk_out->xfer_buf != NULL) && (ep_bulk_out->xfer_len != 0)) {
		if (USB_IS_MEM_DMA_ALIGNED(ep_bulk_out->xfer_buf)) {
			DCache_Clean((u32)ep_bulk_out->xfer_buf, ep_bulk_out->xfer_len);
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "RX buf align err\n");
			return HAL_ERR_MEM;
		}
	}

	usbd_ep_receive(cdev->dev, ep_bulk_out);

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
	usbd_ep_t *ep0_out = &dev->ep0_out;

	UNUSED(dev);

	if (cdev->ctrl_req.bRequest != 0xFFU) {
		cdev->cb->setup(&cdev->ctrl_req, ep0_out->xfer_buf);
		cdev->ctrl_req.bRequest = 0xFFU;

		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  Get descriptor callback
  * @param  dev: USB device instance
  * @param  req: Setup request handle
  * @param  buf: Poniter to Buffer
  * @retval Descriptor length
  */
static u16 cdc_acm_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	u8 *desc = NULL;
	u16 len = 0;
	usb_speed_type_t speed = dev->dev_speed;

	dev->self_powered = CDC_ACM_SELF_POWERED;
	dev->remote_wakeup_en = CDC_ACM_REMOTE_WAKEUP_EN;

	switch (USB_HIGH_BYTE(req->wValue)) {

	case USB_DESC_TYPE_DEVICE:
		len = sizeof(usbd_cdc_acm_dev_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_cdc_acm_dev_desc, len);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_cdc_acm_hs_config_desc;
			len = sizeof(usbd_cdc_acm_hs_config_desc);
		} else {
			desc = (u8 *)usbd_cdc_acm_fs_config_desc;
			len = sizeof(usbd_cdc_acm_fs_config_desc);
		}
		usb_os_memcpy((void *)buf, (void *)desc, len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		len = sizeof(usbd_cdc_acm_device_qualifier_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_cdc_acm_device_qualifier_desc, len);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_cdc_acm_fs_config_desc;
			len = sizeof(usbd_cdc_acm_fs_config_desc);
		} else {
			desc = (u8 *)usbd_cdc_acm_hs_config_desc;
			len = sizeof(usbd_cdc_acm_hs_config_desc);
		}

		usb_os_memcpy((void *)buf, (void *)desc, len);
		buf[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);

		break;

	case USB_DESC_TYPE_STRING:
		switch (USB_LOW_BYTE(req->wValue)) {
		case USBD_IDX_LANGID_STR:
			len = sizeof(usbd_cdc_acm_lang_id_desc);
			usb_os_memcpy((void *)buf, (void *)usbd_cdc_acm_lang_id_desc, len);
			break;
		case USBD_IDX_MFC_STR:
			len = usbd_get_str_desc(CDC_ACM_MFG_STRING, buf);
			break;
		case USBD_IDX_PRODUCT_STR:
			if (speed == USB_SPEED_HIGH) {
				len = usbd_get_str_desc(CDC_ACM_PROD_HS_STRING, buf);
			} else {
				len = usbd_get_str_desc(CDC_ACM_PROD_FS_STRING, buf);
			}
			break;
		case USBD_IDX_SERIAL_STR:
			len = usbd_get_str_desc(CDC_ACM_SN_STRING, buf);
			break;
		case USBD_IDX_MS_OS_STR:
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
	usbd_ep_t *ep_intr_in = &cdev->ep_intr_in;
	usbd_cdc_acm_ntf_t *ntf = (usbd_cdc_acm_ntf_t *)ep_intr_in->xfer_buf;

	if (!dev->is_ready) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x TX %d not ready\n", CDC_ACM_INTR_IN_EP, len);
		return ret;
	}

	if (len > CDC_ACM_INTR_IN_DATA_SIZE) {
		len = CDC_ACM_INTR_IN_DATA_SIZE;
	}

	if (ep_intr_in->xfer_state == 0U) {
		if (dev->is_ready) {
			ep_intr_in->is_busy = 1U;
			ep_intr_in->xfer_state = 1U;

			ntf->bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
			ntf->bNotificationType = type;
			ntf->wValue = value;
			ntf->wIndex = 0;
			ntf->wLength = len;

			usb_os_memcpy((void *)ntf->buf, (void *)data, len);

			if (dev->is_ready) {
				ep_intr_in->xfer_len = CDC_ACM_INTR_IN_REQUEST_SIZE + len;
				usbd_ep_transmit(dev, ep_intr_in);
				ret = HAL_OK;
			} else {
				ep_intr_in->xfer_state = 0U;
			}

			ep_intr_in->is_busy = 0U;
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
	usbd_ep_t *ep_bulk_in = &cdc->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdc->ep_bulk_out;
#if CONFIG_CDC_ACM_NOTIFY
	usbd_ep_t *ep_intr_in = &cdc->ep_intr_in;
#endif

	ep_bulk_out->addr = CDC_ACM_BULK_OUT_EP;
	ep_bulk_out->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_out->xfer_buf_len = bulk_out_xfer_size;
	ep_bulk_out->xfer_buf = (u8 *)usb_os_malloc(ep_bulk_out->xfer_buf_len);
	ep_bulk_out->xfer_len = ep_bulk_out->xfer_buf_len;
	if (ep_bulk_out->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto USBD_CDC_Init_exit;
	}

	ep_bulk_in->addr = CDC_ACM_BULK_IN_EP;
	ep_bulk_in->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_in->xfer_buf_len = bulk_in_xfer_size;
	ep_bulk_in->xfer_buf = (u8 *)usb_os_malloc(ep_bulk_in->xfer_buf_len);
	if (ep_bulk_in->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto USBD_CDC_Init_clean_bulk_out_buf_exit;
	}

#if CONFIG_CDC_ACM_NOTIFY
	ep_intr_in->addr = CDC_ACM_INTR_IN_EP;
	ep_intr_in->type = USB_CH_EP_TYPE_INTR;
	ep_intr_in->xfer_buf = (u8 *)usb_os_malloc(sizeof(usbd_cdc_acm_ntf_t));
	if (ep_intr_in->xfer_buf == NULL) {
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
	usb_os_mfree(ep_intr_in->xfer_buf);
	ep_intr_in->xfer_buf = NULL;

USBD_CDC_Init_clean_bulk_in_buf_exit:
#endif

	usb_os_mfree(ep_bulk_in->xfer_buf);
	ep_bulk_in->xfer_buf = NULL;

USBD_CDC_Init_clean_bulk_out_buf_exit:
	usb_os_mfree(ep_bulk_out->xfer_buf);
	ep_bulk_out->xfer_buf = NULL;

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
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;

#if CONFIG_CDC_ACM_NOTIFY
	usbd_ep_t *ep_intr_in = &cdev->ep_intr_in;
#endif

#if CONFIG_CDC_ACM_NOTIFY
	is_busy = ep_bulk_in->is_busy || ep_intr_in->is_busy;
#else
	is_busy = ep_bulk_in->is_busy;
#endif
	while (is_busy) {
		usb_os_delay_us(100);
	}

	usbd_unregister_class();

	if ((cdev->cb != NULL) && (cdev->cb->deinit != NULL)) {
		cdev->cb->deinit();
	}

#if CONFIG_CDC_ACM_NOTIFY
	if (ep_intr_in->xfer_buf != NULL) {
		usb_os_mfree(ep_intr_in->xfer_buf);
		ep_intr_in->xfer_buf = NULL;
	}
#endif

	if (ep_bulk_in->xfer_buf != NULL) {
		usb_os_mfree(ep_bulk_in->xfer_buf);
		ep_bulk_in->xfer_buf = NULL;
	}

	if (ep_bulk_out->xfer_buf != NULL) {
		usb_os_mfree(ep_bulk_out->xfer_buf);
		ep_bulk_out->xfer_buf = NULL;
	}

	return HAL_OK;
}

/**
  * @brief  Transmit BULK IN packet
  * @param  buf: data buffer
  * @param  len: data length
  * @retval Status
  */
int usbd_cdc_acm_transmit(u8 *buf, u16 len)
{
	int ret = HAL_ERR_HW;
	usbd_cdc_acm_dev_t *cdev = &usbd_cdc_acm_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;

	if (!dev->is_ready) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x TX %d not ready\n", CDC_ACM_BULK_IN_EP, len);
		return ret;
	}

	/* limited the transfer size to the length of the ep_bulk_in.buf(ep_bulk_in.buf_size) */
	if (len > ep_bulk_in->xfer_buf_len) {
		len = ep_bulk_in->xfer_buf_len;
	}

	if (ep_bulk_in->xfer_state == 0U) {
		if (dev->is_ready) {
			ep_bulk_in->is_busy = 1U;
			ep_bulk_in->xfer_state = 1U;
#if CONFIG_CDC_ACM_BULK_TX_SKIP_MEMCPY
			ep_bulk_in->xfer_buf = buf;
#else
			usb_os_memcpy((void *)ep_bulk_in->xfer_buf, (void *)buf, len);
#endif
			if ((ep_bulk_in->skip_dcache_pre_clean) && (buf != NULL) && (len != 0)) {
				if (USB_IS_MEM_DMA_ALIGNED(buf)) {
					DCache_Clean((u32)buf, len);
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "EP TX buf align err\n");
					return HAL_ERR_MEM;
				}
			}

			if (dev->is_ready) {
				ep_bulk_in->xfer_len = len;
				usbd_ep_transmit(dev, ep_bulk_in);
				ret = HAL_OK;
			} else {
				ep_bulk_in->xfer_state = 0U;
			}

			ep_bulk_in->is_busy = 0U;
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

