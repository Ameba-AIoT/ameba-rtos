/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_vendor.h"
#ifdef CONFIG_USBD_COMPOSITE
#include "usbd_composite.h"
#endif

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static u8 usbd_vendor_get_interval(u8 interval_hs, u8 interval_fs, u8 is_hs);
static int usbd_vendor_set_config(usb_dev_t *dev, u8 config);
static void usbd_vendor_clear_config(usb_dev_t *dev, u8 config);
static int usbd_vendor_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_vendor_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
static int usbd_vendor_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_vendor_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static int usbd_vendor_handle_ep0_data_out(usb_dev_t *dev);
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
	USB_LOW_BYTE(USB_VID),                          /* idVendor (patched by core from usbd_dev_info_t) */
	USB_HIGH_BYTE(USB_VID),
	USB_LOW_BYTE(USB_PID),                          /* idProduct (patched by core from usbd_dev_info_t) */
	USB_HIGH_BYTE(USB_PID),
	0x00,                                           /* bcdDevice (patched by core from usbd_dev_info_t) */
	0x02,
	USBD_IDX_MFC_STR,                               /* iManufacturer (patched by core) */
	USBD_IDX_PRODUCT_STR,                           /* iProduct (patched by core) */
	USBD_IDX_SERIAL_STR,                            /* iSerialNumber (patched by core) */
	0x01                                            /* bNumConfigurations */
};  /* usbd_vendor_dev_desc */

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
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
	0x80,                                           /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
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
	USB_H2D,						/* bEndpointAddress: BULK OUT (placeholder) */
	USB_CH_EP_TYPE_BULK,							/* bmAttributes: BULK */
	USB_LOW_BYTE(USBD_VENDOR_HS_BULK_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_HS_BULK_MPS),
	0x00,											/* bInterval */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_D2H,							/* bEndpointAddress: BULK IN (placeholder) */
	USB_CH_EP_TYPE_BULK,							/* bmAttributes: BULK */
	USB_LOW_BYTE(USBD_VENDOR_HS_BULK_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_HS_BULK_MPS),
	0x00,											/* bInterval */

	/* INTR OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_H2D,						/* bEndpointAddress: INTR OUT (placeholder) */
	USB_CH_EP_TYPE_INTR,							/* bmAttributes: INTR */
	USB_LOW_BYTE(USBD_VENDOR_HS_INTR_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_HS_INTR_MPS),
	USBD_VENDOR_DEFAULT_EP_INTERVAL,				/* bInterval (patched at runtime) */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_D2H,							/* bEndpointAddress: INTR IN (placeholder) */
	USB_CH_EP_TYPE_INTR,							/* bmAttributes: INTR */
	USB_LOW_BYTE(USBD_VENDOR_HS_INTR_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_HS_INTR_MPS),
	USBD_VENDOR_DEFAULT_EP_INTERVAL,				/* bInterval (patched at runtime) */

	/* ISOC OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_H2D,						/* bEndpointAddress: ISOC OUT (placeholder) */
	USB_CH_EP_TYPE_ISOC,							/* bmAttributes: ISOC */
	USB_LOW_BYTE(USBD_VENDOR_HS_ISOC_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_HS_ISOC_MPS),
	USBD_VENDOR_DEFAULT_EP_INTERVAL,				/* bInterval (patched at runtime) */

	/* ISOC IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_D2H,							/* bEndpointAddress: ISOC IN (placeholder) */
	USB_CH_EP_TYPE_ISOC,							/* bmAttributes: ISOC */
	USB_LOW_BYTE(USBD_VENDOR_HS_ISOC_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_HS_ISOC_MPS),
	USBD_VENDOR_DEFAULT_EP_INTERVAL,				/* bInterval (patched at runtime) */
};  /* usbd_vendor_hs_config_desc */
#endif

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
	0x80,                                           /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
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
	USB_H2D,						/* bEndpointAddress: BULK OUT (placeholder) */
	USB_CH_EP_TYPE_BULK,							/* bmAttributes: BULK */
	USB_LOW_BYTE(USBD_VENDOR_FS_BULK_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_FS_BULK_MPS),
	0x00,											/* bInterval */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,								/* bLength */
	USB_DESC_TYPE_ENDPOINT,							/* bDescriptorType */
	USB_D2H,							/* bEndpointAddress: BULK IN (placeholder) */
	USB_CH_EP_TYPE_BULK,							/* bmAttributes: BULK */
	USB_LOW_BYTE(USBD_VENDOR_FS_BULK_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_FS_BULK_MPS),
	0x00,											/* bInterval */

	/* INTR OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_H2D,                        /* bEndpointAddress: INTR OUT (placeholder) */
	USB_CH_EP_TYPE_INTR,							/* bmAttributes: INTR */
	USB_LOW_BYTE(USBD_VENDOR_FS_INTR_MPS),          /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_FS_INTR_MPS),
	USBD_VENDOR_DEFAULT_EP_INTERVAL,				/* bInterval (patched at runtime) */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_D2H,                         /* bEndpointAddress: INTR IN (placeholder) */
	USB_CH_EP_TYPE_INTR,							/* bmAttributes: INTR */
	USB_LOW_BYTE(USBD_VENDOR_FS_INTR_MPS),          /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_FS_INTR_MPS),
	USBD_VENDOR_DEFAULT_EP_INTERVAL,				/* bInterval (patched at runtime) */

	/* ISOC OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_H2D,                        /* bEndpointAddress: ISOC OUT (placeholder) */
	USB_CH_EP_TYPE_ISOC,                            /* bmAttributes: ISOC */
	USB_LOW_BYTE(USBD_VENDOR_FS_ISOC_MPS),          /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_FS_ISOC_MPS),
	USBD_VENDOR_DEFAULT_EP_INTERVAL,               /* bInterval */

	/* ISOC IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_D2H,                         /* bEndpointAddress: ISOC IN (placeholder) */
	USB_CH_EP_TYPE_ISOC,                            /* bmAttributes: ISOC */
	USB_LOW_BYTE(USBD_VENDOR_FS_ISOC_MPS),          /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_FS_ISOC_MPS),
	USBD_VENDOR_DEFAULT_EP_INTERVAL,                /* bInterval */
};  /* usbd_vendor_fs_config_desc */

/* Vendor Class Driver */
static const usbd_class_driver_t usbd_vendor_driver = {
	.get_descriptor = usbd_vendor_get_descriptor,
	.set_config = usbd_vendor_set_config,
	.clear_config = usbd_vendor_clear_config,
	.setup = usbd_vendor_setup,
	.ep_data_in = usbd_vendor_handle_ep_data_in,
	.ep_data_out = usbd_vendor_handle_ep_data_out,
	.ep0_data_out = usbd_vendor_handle_ep0_data_out,
	.status_changed = usbd_vendor_status_changed,
};

/* Vendor Device */
static usbd_vendor_dev_t usbd_vendor_dev;

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Clamp the transfer length of an OUT endpoint to whole packets fitting in its buffer
  * @note   The controller always receives whole packets and each packet occupies a DWORD
  *         aligned slot in memory, so the DMA window is a multiple of the aligned MPS. As the
  *         transfer buffer is allocated from the application configuration, which may not be a
  *         multiple of the per-packet stride in use (e.g. the full speed ISOC MPS is 1023, whose
  *         stride is 1024), the length is clamped to @ref usb_get_max_payload_len so that the
  *         window never exceeds the buffer, refer to @ref usbd_ep_receive.
  *         Shall be called after usbd_ep_init(), which fills info.mps for the negotiated speed.
  * @param  ep: OUT endpoint
  * @retval Status
  */
static int usbd_vendor_config_out_xfer_len(usbd_ep_t *ep)
{
	u32 xfer_len = usb_get_max_payload_len(ep->xfer_buf_len, ep->info.mps);

	/* A buffer too small for a single padded packet leaves nothing to request */
	if ((ep->info.mps == 0U) || (xfer_len == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x buf_len %u < mps %u\n", ep->info.addr, ep->xfer_buf_len, ep->info.mps);
		return HAL_ERR_PARA;
	}

	ep->xfer_len = xfer_len;

	return HAL_OK;
}

/**
  * @brief  Get the bInterval of a periodic endpoint for a speed
  * @note   Shared by the descriptor patch and set_config, so the interval the endpoint is opened
  *         with is the one the host reads from the descriptor, ref USB 2.0 9.6.6.
  * @param  interval_hs: Configured High-Speed value, 0 for the class default
  * @param  interval_fs: Configured Full-Speed value, 0 for the class default
  * @param  is_hs: 1 for High Speed, 0 for Full Speed
  * @retval bInterval
  */
static u8 usbd_vendor_get_interval(u8 interval_hs, u8 interval_fs, u8 is_hs)
{
	u8 interval = (is_hs != 0U) ? interval_hs : interval_fs;

	return (interval != 0U) ? interval : USBD_VENDOR_DEFAULT_EP_INTERVAL;
}

/**
  * @brief  Set Vendor class configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
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
	const usbd_vendor_config_t *cfg = cdev->cfg;
	u8 speed = dev->dev_speed;
	u8 is_hs = (speed == USB_SPEED_HIGH) ? 1U : 0U;
	usb_ep_info_t *info;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if (config != 1U) {
		return HAL_ERR_PARA;
	}

	cdev->dev = dev;

	cdev->alt_setting = 0U;
	/* Init INTR IN EP */
	info = &ep_intr_in->info;
	info->mps = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_INTR_MPS : USBD_VENDOR_FS_INTR_MPS;
	info->binterval = usbd_vendor_get_interval(cfg->intr_in_interval_hs, cfg->intr_in_interval_fs, is_hs);
	ret = usbd_ep_init(dev, ep_intr_in);
	if (ret != HAL_OK) {
		goto exit_clear_config;
	}
	ep_intr_in->xfer_state = 0;

	/* Init INTR OUT EP */
	info = &ep_intr_out->info;
	info->mps = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_INTR_MPS : USBD_VENDOR_FS_INTR_MPS;
	info->binterval = usbd_vendor_get_interval(cfg->intr_out_interval_hs, cfg->intr_out_interval_fs, is_hs);
	ret = usbd_ep_init(dev, ep_intr_out);
	if (ret != HAL_OK) {
		goto exit_clear_config;
	}
	ret = usbd_vendor_config_out_xfer_len(ep_intr_out);
	if (ret != HAL_OK) {
		goto exit_clear_config;
	}
	ret = usbd_ep_receive(dev, ep_intr_out);
	if (ret != HAL_OK) {
		goto exit_clear_config;
	}

	/* Init BULK IN EP */
	info = &ep_bulk_in->info;
	info->mps = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_BULK_MPS : USBD_VENDOR_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep_bulk_in);
	if (ret != HAL_OK) {
		goto exit_clear_config;
	}
	ep_bulk_in->xfer_state = 0;

	/* Init BULK OUT EP */
	info = &ep_bulk_out->info;
	info->mps = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_BULK_MPS : USBD_VENDOR_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep_bulk_out);
	if (ret != HAL_OK) {
		goto exit_clear_config;
	}
	ret = usbd_vendor_config_out_xfer_len(ep_bulk_out);
	if (ret != HAL_OK) {
		goto exit_clear_config;
	}
	ret = usbd_ep_receive(dev, ep_bulk_out);
	if (ret != HAL_OK) {
		goto exit_clear_config;
	}

	/* Init ISO IN EP */
	info = &ep_isoc_in->info;
	info->mps = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_ISOC_MPS : USBD_VENDOR_FS_ISOC_MPS;
	info->binterval = usbd_vendor_get_interval(cfg->isoc_in_interval_hs, cfg->isoc_in_interval_fs, is_hs);
	ret = usbd_ep_init(dev, ep_isoc_in);
	if (ret != HAL_OK) {
		goto exit_clear_config;
	}

	/* Init ISO OUT EP */
	info = &ep_isoc_out->info;
	info->mps = (speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_ISOC_MPS : USBD_VENDOR_FS_ISOC_MPS;
	info->binterval = usbd_vendor_get_interval(cfg->isoc_out_interval_hs, cfg->isoc_out_interval_fs, is_hs);
	ret = usbd_ep_init(dev, ep_isoc_out);
	if (ret != HAL_OK) {
		goto exit_clear_config;
	}
	ret = usbd_vendor_config_out_xfer_len(ep_isoc_out);
	if (ret != HAL_OK) {
		goto exit_clear_config;
	}
	ret = usbd_ep_receive(dev, ep_isoc_out);
	if (ret != HAL_OK) {
		goto exit_clear_config;
	}

	if (cdev->cb->set_config != NULL) {
		cdev->cb->set_config();
	}

	return ret;

exit_clear_config:
	/* Release the endpoints initialized above */
	usbd_vendor_clear_config(dev, config);
	return ret;
}

/**
  * @brief  Clear Vendor configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval None
  */
static void usbd_vendor_clear_config(usb_dev_t *dev, u8 config)
{
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
}

/**
  * @brief  Handle Vendor specific CTRL requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
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

	//RTK_LOGS(TAG, RTK_LOG_DEBUG, "SETUP: bmRequestType=0x%02x bRequest=0x%02x wLength=0x%04x wValue=%x\n",
	//	 req->bmRequestType, req->bRequest, req->wLength, req->wValue);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				/* Ref USB 2.0 Table 9-10: the whole wIndex is the interface number, this
				   class owns the single interface 0 only */
				/* Ref USB 2.0 9.4.10: only the alternate settings described for the
				   interface are valid, interface 0 describes bAlternateSetting 0 only,
				   so any other wValue is a request error */
				if ((req->wIndex == 0U) && (req->wValue == 0U)) {
					cdev->alt_setting = 0U;

					/* Ref USB 2.0 9.4.10: the endpoints of the selected interface return
					   to their default state, not halted and data toggle DATA0. This holds
					   even for an interface with the default setting only, hosts do send
					   the request in that case. The ISOC endpoints are skipped, they can
					   neither be halted nor own a data toggle. */
					usbd_ep_clear_stall(dev, &cdev->ep_bulk_in);
					usbd_ep_clear_stall(dev, &cdev->ep_bulk_out);
					usbd_ep_clear_stall(dev, &cdev->ep_intr_in);
					usbd_ep_clear_stall(dev, &cdev->ep_intr_out);
				} else {
					ret = HAL_ERR_HW;
				}
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_INTERFACE:
			/* Ref USB 2.0 9.4.4 and Table 9-3: wLength is one, and a request for a
			   non-existent interface is a request error */
			if ((dev->dev_state == USBD_STATE_CONFIGURED) && (req->wIndex == 0U) && (req->wLength == 1U)) {
				ep0_in->xfer_buf[0] = cdev->alt_setting;
				ep0_in->xfer_len = 1U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_STATUS:
			/* Ref USB 2.0 9.4.5 and Table 9-3: an interface-recipient GET_STATUS returns two
			   reserved zero bytes, wLength is two, and a request for a non-existent interface
			   is a request error */
			if ((dev->dev_state == USBD_STATE_CONFIGURED) && (req->wIndex == 0U) && (req->wLength == 2U)) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				ret = usbd_ep_transmit(dev, ep0_in);
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
		if (cdev->cb->setup == NULL) {
			/* No handler for nonstandard requests, STALL so that the host recovers
			   promptly instead of waiting out the data stage */
			ret = HAL_ERR_PARA;
		} else if (req->wLength != 0U) {
			if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
				u16 rsp_len = req->wLength;

				/* Ref USB 2.0 9.3.4: wLength is the maximum the host accepts, never trust it
				   as a buffer size. Clamp to the EP0 buffer to avoid an over-read. */
				if (rsp_len > ep0_in->xfer_buf_len) {
					rsp_len = (u16)ep0_in->xfer_buf_len;
				}

				/* EP0 buffer is shared with descriptor and OUT traffic, clear the response
				   window so a callback writing fewer bytes cannot leak stale data. The
				   callback owns the whole window and shall reject any request it does not
				   support, which makes the core STALL EP0. */
				usb_os_memset((void *)ep0_in->xfer_buf, 0, rsp_len);
				ret = cdev->cb->setup(req, ep0_in->xfer_buf);
				if (ret == HAL_OK) {
					ep0_in->xfer_len = rsp_len;
					/* Propagate a submit failure so that the core stalls EP0 instead of
					   leaving the host waiting out the data stage, ref USB 2.0 8.5.3.4 */
					ret = usbd_ep_transmit(dev, ep0_in);
				}
			} else {
				/* Ref USB 2.0 8.5.3: an H2D control transfer with wLength > 0 carries the
				   payload in a following data stage, so the request cannot be handed to the
				   application yet. Save the setup packet and arm EP0 OUT; the payload is
				   delivered to cdev->cb->setup() from usbd_vendor_handle_ep0_data_out() once
				   the whole data stage has been received. */
				usb_os_memcpy((void *)&cdev->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
				cdev->ctrl_req_pending = 1U;
				ep0_out->xfer_len = req->wLength;
				ret = usbd_ep_receive(dev, ep0_out);
				if (ret != HAL_OK) {
					/* EP0 OUT was not armed (e.g. wLength exceeds the EP0 buffer), so no data
					   stage completion will ever arrive. Drop the pending request to keep the
					   next transfer clean and report the failure so that the core stalls EP0
					   instead of silently acknowledging a request that is never processed. */
					cdev->ctrl_req_pending = 0U;
				}
			}
		} else {
			/* No data stage: the setup packet is self-contained, dispatch it right away.
			   Propagate the callback status so that an unsupported request is STALLed by
			   the core instead of being ACKed. */
			ret = cdev->cb->setup(req, NULL);
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
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_vendor_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	const usbd_vendor_cb_t *cb = cdev->cb;
	usbd_ep_t *ep_intr_in = &cdev->ep_intr_in;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	UNUSED(dev);

	/* Ref USB 2.0 5.3.2: an endpoint is owned by exactly one function. Reject a foreign
	   address so that the composite dispatcher keeps iterating to the real owner. */
	if ((ep_addr != cdev->cfg->intr_in_addr) && (ep_addr != cdev->cfg->bulk_in_addr)
		&& (ep_addr != cdev->cfg->isoc_in_addr)) {
		return HAL_ERR_PARA;
	}

	if (status != HAL_OK) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
	}

	if (ep_addr == cdev->cfg->intr_in_addr) {
		ep_intr_in->xfer_state = 0U;
		if (cb->intr_transmitted != NULL) {
			cb->intr_transmitted(status);
		}
	}

	if (ep_addr == cdev->cfg->bulk_in_addr) {
		ep_bulk_in->xfer_state = 0U;
		if (cb->bulk_transmitted != NULL) {
			cb->bulk_transmitted(status);
		}
	}

	if (ep_addr == cdev->cfg->isoc_in_addr) {
		if (cb->isoc_transmitted != NULL) {
			cb->isoc_transmitted(status);
		}
	}

	return HAL_OK;
}

/**
  * @brief  Data received on non-control Out endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_vendor_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	const usbd_vendor_cb_t *cb = cdev->cb;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;
	usbd_ep_t *ep_intr_out = &cdev->ep_intr_out;
	UNUSED(dev);
	int ret = HAL_OK;

	/* Ref USB 2.0 5.3.2: an endpoint is owned by exactly one function. Reject a foreign
	   address so that the composite dispatcher keeps iterating to the real owner, which
	   also re-arms its own OUT endpoint. */
	if ((ep_addr != cdev->cfg->intr_out_addr) && (ep_addr != cdev->cfg->bulk_out_addr)
		&& (ep_addr != cdev->cfg->isoc_out_addr)) {
		return HAL_ERR_PARA;
	}

	if (ep_addr == cdev->cfg->intr_out_addr) {
		if ((len > 0) && (cb != NULL) && (cb->intr_received != NULL)) {
			cb->intr_received(ep_intr_out->xfer_buf, len);
		}
		ret = usbd_ep_receive(cdev->dev, ep_intr_out);
		if (ret != HAL_OK) {
			return ret;
		}
	}

	if (ep_addr == cdev->cfg->bulk_out_addr) {
		if ((len > 0) && (cb != NULL) && (cb->bulk_received != NULL)) {
			cb->bulk_received(ep_bulk_out->xfer_buf, len);
		}
		ret = usbd_ep_receive(cdev->dev, ep_bulk_out);
		if (ret != HAL_OK) {
			return ret;
		}
	}

	if (ep_addr == cdev->cfg->isoc_out_addr) {
		if ((len > 0) && (cb != NULL) && (cb->isoc_received != NULL)) {
			cb->isoc_received(ep_isoc_out->xfer_buf, len);
		}
		ret = usbd_ep_receive(cdev->dev, ep_isoc_out);
		if (ret != HAL_OK) {
			return ret;
		}
	}

	return ret;
}

/**
  * @brief  Data received on the control endpoint (EP0 OUT data stage completed)
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  *
  *         Ref USB 2.0 8.5.3: an H2D class/vendor request with wLength > 0 is split into
  *         SETUP + DATA OUT + STATUS. usbd_vendor_setup() only saves the setup packet and
  *         arms EP0 OUT; the core invokes this callback after the last data packet has been
  *         received, which is the point where the request may finally be executed with its
  *         complete payload.
  *
  *         Returning non-HAL_OK signals "this request is not mine", which the composite
  *         dispatcher and the core use to avoid treating a stale event as handled.
  * @param  dev: USB device instance
  * @retval Status
  */
static int usbd_vendor_handle_ep0_data_out(usb_dev_t *dev)
{
	/* Ref USB 2.0 8.5.3.1: a non-zero value makes the core stall the status stage. Default to
	   success, an application without a setup handler simply ignores vendor requests and the data
	   stage itself was received correctly. Real failures still propagate from cb->setup. */
	int ret = HAL_OK;
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usbd_ep_t *ep0_out = &dev->ep0_out;

	if (cdev->ctrl_req_pending != 0U) {
		/* Consume the pending request first: a single data stage belongs to exactly one setup
		   packet, so the saved request must not be replayed by a later EP0 OUT event. */
		cdev->ctrl_req_pending = 0U;

		/* cb is released by usbd_vendor_deinit(), which may run between the setup and the
		   data stage of an H2D request, so both the structure and the handler are checked. */
		if ((cdev->cb != NULL) && (cdev->cb->setup != NULL)) {
			ret = cdev->cb->setup(&cdev->ctrl_req, ep0_out->xfer_buf);
		}
	}

	return ret;
}

/**
  * @brief  Patch endpoint addresses in a configuration descriptor to use runtime EP config.
  * @note   Replaces direction-only placeholders (USB_D2H/USB_H2D) with actual
  *         EP addresses from the EP configuration structure.
  * @param  desc: Pointer to config descriptor body (starting after config header)
  * @param  len: Length of the descriptor block
  * @param  speed: Speed the descriptor describes, selects the INTR/ISOC intervals
  * @retval None
  */
static void usbd_vendor_patch_desc(u8 *desc, u16 len, usb_speed_type_t speed)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	const usbd_vendor_config_t *cfg = cdev->cfg;
	u8 is_hs = (speed == USB_SPEED_HIGH) ? 1U : 0U;

	for (u16 i = 0; i < len;) {
		u8 dlen = desc[i];
		u8 dtype = desc[i + 1];
		if (dlen == 0) {
			break;
		}

		if ((dtype == USB_DESC_TYPE_ENDPOINT) && (i + 3 <= len)) {
			u8 addr  = desc[i + 2];
			u8 dir   = addr & USB_REQ_DIR_MASK;
			u8 type  = desc[i + 3] & 0x03;

			u8 interval = USBD_VENDOR_DEFAULT_EP_INTERVAL;

			if ((dir == USB_D2H) && (type == USB_CH_EP_TYPE_BULK)) {
				desc[i + 2] = cfg->bulk_in_addr;
			} else if ((dir == USB_H2D) && (type == USB_CH_EP_TYPE_BULK)) {
				desc[i + 2] = cfg->bulk_out_addr;
			} else if ((dir == USB_D2H) && (type == USB_CH_EP_TYPE_INTR)) {
				desc[i + 2] = cfg->intr_in_addr;
				interval = usbd_vendor_get_interval(cfg->intr_in_interval_hs, cfg->intr_in_interval_fs, is_hs);
			} else if ((dir == USB_H2D) && (type == USB_CH_EP_TYPE_INTR)) {
				desc[i + 2] = cfg->intr_out_addr;
				interval = usbd_vendor_get_interval(cfg->intr_out_interval_hs, cfg->intr_out_interval_fs, is_hs);
			} else if ((dir == USB_D2H) && (type == USB_CH_EP_TYPE_ISOC)) {
				desc[i + 2] = cfg->isoc_in_addr;
				interval = usbd_vendor_get_interval(cfg->isoc_in_interval_hs, cfg->isoc_in_interval_fs, is_hs);
			} else if ((dir == USB_H2D) && (type == USB_CH_EP_TYPE_ISOC)) {
				desc[i + 2] = cfg->isoc_out_addr;
				interval = usbd_vendor_get_interval(cfg->isoc_out_interval_hs, cfg->isoc_out_interval_fs, is_hs);
			}

			/* BULK leaves bInterval 0 (ignored for BULK, Ref USB 2.0 9.6.6); the periodic
			   endpoints take the configured value, or the class default when left 0. */
			if ((type != USB_CH_EP_TYPE_BULK) && (dlen >= USB_LEN_EP_DESC)) {
				desc[i + 6] = interval;
			}
		} else if ((dtype == USB_DESC_TYPE_INTERFACE) && (dlen >= USB_LEN_IF_DESC)) {
			desc[i + 8] = cdev->itf_str_idx;         /* iInterface */
		}
		i += dlen;
	}
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
static u16 usbd_vendor_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len)
{
	usb_speed_type_t speed = dev->dev_speed;
	/* Speed the returned configuration describes, which selects the bInterval encoding */
	usb_speed_type_t desc_speed = speed;
	const u8 *desc = NULL;
	u16 len = 0;
	u8 type = USB_HIGH_BYTE(req->wValue);
	u8 is_cfg = 0;

	switch (type) {

	case USB_DESC_TYPE_DEVICE:
		desc = usbd_vendor_dev_desc;
		len = sizeof(usbd_vendor_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
#ifndef CONFIG_SUPPORT_USB_FS_ONLY
		if (speed == USB_SPEED_HIGH) {
			desc = usbd_vendor_hs_config_desc;
			len = sizeof(usbd_vendor_hs_config_desc);
		} else
#endif
		{
			desc = usbd_vendor_fs_config_desc;
			len = sizeof(usbd_vendor_fs_config_desc);
		}
		is_cfg = 1;
		break;

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		desc = usbd_vendor_device_qualifier_desc;
		len = sizeof(usbd_vendor_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		/* Ref USB 2.0 9.6.4: describes the configuration at the speed the device is not
		   running at, so its bInterval is patched for that speed, not the current one */
		if (speed == USB_SPEED_HIGH) {
			desc = usbd_vendor_fs_config_desc;
			len = sizeof(usbd_vendor_fs_config_desc);
			desc_speed = USB_SPEED_FULL;
		} else {
			desc = usbd_vendor_hs_config_desc;
			len = sizeof(usbd_vendor_hs_config_desc);
			desc_speed = USB_SPEED_HIGH;
		}
		is_cfg = 1;
		break;
#endif

	case USB_DESC_TYPE_STRING:
		/* Every string this class owns is registered with usbd_add_string() and answered by
		   the core, which only forwards an index it does not know, e.g. the MS OS string */
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_GET_DESC, 0);
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
		buf[USB_CFG_DESC_OFFSET_TYPE] = type;
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);

		/* Patch EP addresses, intervals and the class string index to actual values */
		usbd_vendor_patch_desc(buf + USB_LEN_CFG_DESC, len - USB_LEN_CFG_DESC, desc_speed);
	}

	return len;
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
static void usbd_vendor_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;

	UNUSED(dev);

	if (cdev->cb->status_changed) {
		cdev->cb->status_changed(old_status, status);
	}
}

/**
  * @brief  Initialize vendor device
  * @param[in] cb: Pointer to the user-defined callback structure.
  * @param[in] cfg: Pointer to the class configuration.
  * @retval Status
  */
static int usbd_vendor_private_init(const usbd_vendor_cb_t *cb, const usbd_vendor_config_t *cfg)
{
	int ret = HAL_OK;
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &cdev->ep_intr_in;
	usbd_ep_t *ep_intr_out = &cdev->ep_intr_out;
	usb_ep_info_t *info;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	if (cfg == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cfg\n");
		return HAL_ERR_PARA;
	}

	/* Every endpoint is mandatory for this class, and a wrong direction bit would make the
	   class claim an endpoint the controller drives the other way round. */
	if (((cfg->bulk_in_addr & USB_REQ_DIR_MASK) != USB_D2H) ||
		((cfg->isoc_in_addr & USB_REQ_DIR_MASK) != USB_D2H) ||
		((cfg->intr_in_addr & USB_REQ_DIR_MASK) != USB_D2H) ||
		((cfg->bulk_out_addr & USB_REQ_DIR_MASK) != USB_H2D) ||
		((cfg->isoc_out_addr & USB_REQ_DIR_MASK) != USB_H2D) ||
		((cfg->intr_out_addr & USB_REQ_DIR_MASK) != USB_H2D) ||
		((cfg->bulk_in_addr & (~USB_REQ_DIR_MASK)) == 0U) ||
		((cfg->bulk_out_addr & (~USB_REQ_DIR_MASK)) == 0U) ||
		((cfg->isoc_in_addr & (~USB_REQ_DIR_MASK)) == 0U) ||
		((cfg->isoc_out_addr & (~USB_REQ_DIR_MASK)) == 0U) ||
		((cfg->intr_in_addr & (~USB_REQ_DIR_MASK)) == 0U) ||
		((cfg->intr_out_addr & (~USB_REQ_DIR_MASK)) == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP addr\n");
		return HAL_ERR_PARA;
	}

	/* Published before anything that reads it: the descriptor patch path and every endpoint
	   setup below resolve their options through cdev->cfg. */
	cdev->cfg = cfg;

	/* The core owns the string table, so the same call works in standalone and composite
	   mode. 0 means no iInterface string. */
	cdev->itf_str_idx = usbd_add_string(cfg->itf_str);

	/* No H2D class/vendor request is waiting for its data stage yet. */
	cdev->ctrl_req_pending = 0U;

	info = &ep_bulk_out->info;
	info->addr = cdev->cfg->bulk_out_addr;
	info->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_out->xfer_len = cdev->cfg->bulk_out_xfer_size;
	ep_bulk_out->xfer_buf_len = cdev->cfg->bulk_out_xfer_size;
	ep_bulk_out->xfer_buf = (u8 *)usb_os_malloc(ep_bulk_out->xfer_buf_len);
	if (ep_bulk_out->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_exit;
	}

	info = &ep_bulk_in->info;
	info->addr = cdev->cfg->bulk_in_addr;
	info->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_in->xfer_buf_len = cdev->cfg->bulk_in_xfer_size;
	ep_bulk_in->xfer_buf = (u8 *)usb_os_malloc(ep_bulk_in->xfer_buf_len);
	if (ep_bulk_in->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_bulk_out_buf_exit;
	}

	info = &ep_intr_out->info;
	info->addr = cdev->cfg->intr_out_addr;
	info->type = USB_CH_EP_TYPE_INTR;
	info->binterval = 1U;
	ep_intr_out->xfer_len = cdev->cfg->intr_out_xfer_size;
	ep_intr_out->xfer_buf_len = cdev->cfg->intr_out_xfer_size;
	ep_intr_out->xfer_buf = (u8 *)usb_os_malloc(ep_intr_out->xfer_buf_len);
	if (ep_intr_out->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_bulk_in_buf_exit;
	}

	info = &ep_intr_in->info;
	info->addr = cdev->cfg->intr_in_addr;
	info->type = USB_CH_EP_TYPE_INTR;
	info->binterval = 1U;
	ep_intr_in->xfer_buf_len = cdev->cfg->intr_in_xfer_size;
	ep_intr_in->xfer_buf = (u8 *)usb_os_malloc(ep_intr_in->xfer_buf_len);
	if (ep_intr_in->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_intr_out_buf_exit;
	}

	info = &ep_isoc_out->info;
	info->addr = cdev->cfg->isoc_out_addr;
	info->type = USB_CH_EP_TYPE_ISOC;
	info->binterval = 1U;
	ep_isoc_out->xfer_len = cdev->cfg->isoc_out_xfer_size;
	ep_isoc_out->xfer_buf_len = cdev->cfg->isoc_out_xfer_size;
	ep_isoc_out->xfer_buf = (u8 *)usb_os_malloc(ep_isoc_out->xfer_buf_len);
	if (ep_isoc_out->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_intr_in_buf_exit;
	}

	info = &ep_isoc_in->info;
	info->addr = cdev->cfg->isoc_in_addr;
	info->type = USB_CH_EP_TYPE_ISOC;
	info->binterval = 1U;
	ep_isoc_in->xfer_buf_len = cdev->cfg->isoc_in_xfer_size;
	ep_isoc_in->xfer_buf = (u8 *)usb_os_malloc(ep_isoc_in->xfer_buf_len);
	if (ep_isoc_in->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_isoc_out_buf_exit;
	}

	cdev->cb = cb;
	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			goto init_clean_isoc_in_buf_exit;
		}
	}


	return ret;

init_clean_isoc_in_buf_exit:
	usb_os_mfree((void *)ep_isoc_in->xfer_buf);
	ep_isoc_in->xfer_buf = NULL;

init_clean_isoc_out_buf_exit:
	usb_os_mfree((void *)ep_isoc_out->xfer_buf);
	ep_isoc_out->xfer_buf = NULL;

init_clean_intr_in_buf_exit:
	usb_os_mfree((void *)ep_intr_in->xfer_buf);
	ep_intr_in->xfer_buf = NULL;

init_clean_intr_out_buf_exit:
	usb_os_mfree((void *)ep_intr_out->xfer_buf);
	ep_intr_out->xfer_buf = NULL;

init_clean_bulk_in_buf_exit:
	usb_os_mfree((void *)ep_bulk_in->xfer_buf);
	ep_bulk_in->xfer_buf = NULL;

init_clean_bulk_out_buf_exit:
	usb_os_mfree((void *)ep_bulk_out->xfer_buf);
	ep_bulk_out->xfer_buf = NULL;

init_exit:
	return ret;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize vendor device as a standalone device (from_composite = 0).
  * @param[in] cb: Pointer to the user-defined callback structure.
  * @param[in] cfg: Pointer to the class configuration.
  * @retval Status
  */
int usbd_vendor_init(const usbd_vendor_cb_t *cb, const usbd_vendor_config_t *cfg)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	int ret;

	ret = usbd_vendor_private_init(cb, cfg);
	if (ret == HAL_OK) {
		cdev->from_composite = 0;
		usbd_register_class(&usbd_vendor_driver);
	}
	return ret;
}

#ifdef CONFIG_USBD_COMPOSITE
/**
  * @brief  Initialize vendor device as part of a composite device (from_composite = 1).
  * @param[in] cb: Pointer to the user-defined callback structure.
  * @param[in] cfg: Pointer to the class configuration.
  * @retval Status
  */
int usbd_composite_vendor_init(const usbd_vendor_cb_t *cb, const usbd_vendor_config_t *cfg)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	int ret;

	ret = usbd_vendor_private_init(cb, cfg);
	if (ret == HAL_OK) {
		cdev->from_composite = 1;
		ret = usbd_composite_register_driver(&usbd_vendor_driver);
		if (ret != HAL_OK) {
			/* private_init completed, so deinit is its exact reverse. from_composite is
			   already 1, so the unregister inside is a no-op for an unregistered driver. */
			usbd_vendor_deinit();
		}
	}
	return ret;
}
#endif

/**
  * @brief  DeInitialize vendor device
  * @param  void
  * @retval None
  */
void usbd_vendor_deinit(void)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &cdev->ep_intr_in;
	usbd_ep_t *ep_intr_out = &cdev->ep_intr_out;
	int ret;

	/* Leave the core before anything is torn down. Composite: refused until
	   usbd_composite_deinit() has blocked the ISR and cleared the configuration of every
	   function. Standalone: refused in interrupt context. Either way nothing is released when
	   refused, and once accepted no class callback runs any more and every endpoint of this
	   class is deinitialized: ref USB 2.0 9.4.7, the transfers still pending are aborted. */
#ifdef CONFIG_USBD_COMPOSITE
	if (cdev->from_composite != 0U) {
		ret = usbd_composite_unregister_driver(&usbd_vendor_driver);
	} else
#endif
	{
		ret = usbd_unregister_class();
	}
	if (ret != HAL_OK) {
		return;
	}

	/* No completion arrives any more to clear them */
	ep_bulk_in->xfer_state = 0U;
	ep_intr_in->xfer_state = 0U;

	cdev->ctrl_req_pending = 0U;

	if (cdev->cb != NULL) {
		if (cdev->cb->deinit != NULL) {
			cdev->cb->deinit();
		}
		cdev->cb = NULL;
	}

	usb_os_mfree((void *)ep_bulk_in->xfer_buf);
	ep_bulk_in->xfer_buf = NULL;

	usb_os_mfree((void *)ep_bulk_out->xfer_buf);
	ep_bulk_out->xfer_buf = NULL;

	usb_os_mfree((void *)ep_intr_in->xfer_buf);
	ep_intr_in->xfer_buf = NULL;

	usb_os_mfree((void *)ep_intr_out->xfer_buf);
	ep_intr_out->xfer_buf = NULL;

	usb_os_mfree((void *)ep_isoc_in->xfer_buf);
	ep_isoc_in->xfer_buf = NULL;

	usb_os_mfree((void *)ep_isoc_out->xfer_buf);
	ep_isoc_out->xfer_buf = NULL;
}

int usbd_vendor_transmit_bulk_data(u8 *buf, u32 len)
{
	int ret = HAL_OK;
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;

	/* cdev->dev is assigned in set_config, so it is NULL until the host has
	   configured the device. */
	if ((dev == NULL) || (dev->is_ready == 0U)) {
		return HAL_ERR_HW;
	}

	if (len > ep_bulk_in->xfer_buf_len) {
		len = ep_bulk_in->xfer_buf_len;
	}

	if (ep_bulk_in->xfer_state == 0U) {
		ep_bulk_in->xfer_state = 1U;
		usb_os_memcpy((void *)ep_bulk_in->xfer_buf, (const void *)buf, len);
		ep_bulk_in->xfer_len = len;
		ret = usbd_ep_transmit(dev, ep_bulk_in);
		if (ret != HAL_OK) {
			/* The transfer was rejected before being armed on the controller (buffer not DMA
			   aligned, EP not opened, or xfer_len/packet count beyond the XFRSIZ/PKTCNT field
			   limits), so no ep_data_in completion will ever clear xfer_state. Roll it back
			   here, otherwise this EP would stay busy forever and every later send would
			   return HAL_BUSY until the next set_config. */
			ep_bulk_in->xfer_state = 0U;
		}
	} else {
		ret = HAL_BUSY;
	}

	return ret;
}

int usbd_vendor_transmit_intr_data(u8 *buf, u32 len)
{
	int ret = HAL_OK;
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_ep_t *ep_intr_in = &cdev->ep_intr_in;

	/* cdev->dev is assigned in set_config, so it is NULL until the host has
	   configured the device. */
	if ((dev == NULL) || (dev->is_ready == 0U)) {
		return HAL_ERR_HW;
	}

	if (len > ep_intr_in->xfer_buf_len) {
		len = ep_intr_in->xfer_buf_len;
	}

	if (ep_intr_in->xfer_state == 0U) {
		ep_intr_in->xfer_state = 1U;
		usb_os_memcpy((void *)ep_intr_in->xfer_buf, (const void *)buf, len);
		ep_intr_in->xfer_len = len;
		ret = usbd_ep_transmit(dev, ep_intr_in);
		if (ret != HAL_OK) {
			/* Same rollback as the bulk IN path: an immediate submit error produces no
			   completion callback, so the busy flag has to be released by the caller side. */
			ep_intr_in->xfer_state = 0U;
		}
	} else {
		ret = HAL_BUSY;
	}

	return ret;
}

int usbd_vendor_transmit_isoc_data(u8 *buf, u32 len)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;

	/* cdev->dev is assigned in set_config, so it is NULL until the host has
	   configured the device. */
	if ((dev == NULL) || (dev->is_ready == 0U)) {
		return HAL_ERR_HW;
	}

	if (len > ep_isoc_in->xfer_buf_len) {
		len = ep_isoc_in->xfer_buf_len;
	}

	/* Ref USB 2.0 5.6: ISOC is a timely, best-effort stream without retry, so no in-flight
	   state is kept. Each call arms the next packet right away, the caller accepts that a
	   packet still pending may be overwritten. */
	usb_os_memcpy((void *)ep_isoc_in->xfer_buf, (const void *)buf, len);
	ep_isoc_in->xfer_len = len;
	return usbd_ep_transmit(dev, ep_isoc_in);
}
