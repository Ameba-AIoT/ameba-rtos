/**
  ******************************************************************************
  * @file    usbd_vendor.c
  * @author  Realsil WLAN5 Team
  * @brief   This file provides the functionalities of the USB Vendor Class
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

#include "usbd_vendor.h"
#include "usbd.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_vendor_set_config(usb_dev_t *dev, u8 config);
static int usbd_vendor_clear_config(usb_dev_t *dev, u8 config);
static int usbd_vendor_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u8 *usbd_vendor_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len);
static int usbd_vendor_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_vendor_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static void usbd_vendor_status_changed(usb_dev_t *dev, u8 status);

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "VND";

/* USB Standard Device Descriptor */
static u8 usbd_vendor_dev_desc[USB_LEN_DEV_DESC] USB_DMA_ALIGNED = {
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
static u8 usbd_vendor_lang_id_desc[USB_LEN_LANGID_STR_DESC] USB_DMA_ALIGNED = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(USBD_VENDOR_LANGID_STRING),            /* wLANGID */
	USB_HIGH_BYTE(USBD_VENDOR_LANGID_STRING),
};  /* usbd_vendor_lang_id_desc */

/* USB Standard Device Qualifier Descriptor */
static u8 usbd_vendor_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] USB_DMA_ALIGNED = {
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
static u8 usbd_vendor_hs_config_desc[USBD_VENDOR_CONFIG_DESC_SIZE] USB_DMA_ALIGNED = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,								/* bLength */
	USB_DESC_TYPE_CONFIGURATION,					/* bDescriptorType */
	USBD_VENDOR_CONFIG_DESC_SIZE,					/* wTotalLength */
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
	0x04,                                           /* bNumEndpoints */
	USBD_VENDOR_CLASS_CODE,                         /* bInterfaceClass: Vendor */
	USBD_VENDOR_SUBCLASS_CODE,                      /* bInterfaceSubClass */
	USBD_VENDOR_PROTOCOL,                 			/* bInterfaceProtocol */
	0x00,                                           /* iInterface */

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

	/* Interface1 Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	0x00,                                           /* bInterfaceNumber */
	0x01,                                           /* bAlternateSetting */
	0x04,                                           /* bNumEndpoints */
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

	/* ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_VENDOR_ISOC_OUT_EP,						/* bEndpointAddress */
	USB_CH_EP_TYPE_ISOC,							/* bmAttributes: ISOC */
	USB_LOW_BYTE(USBD_VENDOR_HS_ISOC_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_HS_ISOC_MPS),
	USBD_VENDOR_HS_ISOC_OUT_INTERVAL,				/* bInterval */

	/* ISO IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_VENDOR_ISOC_IN_EP,							/* bEndpointAddress */
	USB_CH_EP_TYPE_ISOC,							/* bmAttributes: ISOC */
	USB_LOW_BYTE(USBD_VENDOR_HS_ISOC_MPS),			/* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_VENDOR_HS_ISOC_MPS),
	USBD_VENDOR_HS_ISOC_IN_INTERVAL,				/* bInterval */
};  /* usbd_vendor_hs_config_desc */

/* USB Vendor Device Full Speed Configuration Descriptor */
static u8 usbd_vendor_fs_config_desc[USBD_VENDOR_CONFIG_DESC_SIZE] USB_DMA_ALIGNED = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	USBD_VENDOR_CONFIG_DESC_SIZE,					/* wTotalLength */
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
	0x04,                                           /* bNumEndpoints */
	USBD_VENDOR_CLASS_CODE,                         /* bInterfaceClass: Vendor */
	USBD_VENDOR_SUBCLASS_CODE,                      /* bInterfaceSubClass*/
	USBD_VENDOR_PROTOCOL,                 			/* bInterfaceProtocol */
	0x00,                                           /* iInterface */

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

	/* Interface1 Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	0x00,                                           /* bInterfaceNumber */
	0x01,                                           /* bAlternateSetting */
	0x04,                                           /* bNumEndpoints */
	USBD_VENDOR_CLASS_CODE,                         /* bInterfaceClass: Vendor */
	USBD_VENDOR_SUBCLASS_CODE,                      /* bInterfaceSubClass */
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
usbd_class_driver_t usbd_vendor_driver = {
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

#if CONFIG_USBD_VENDOR_BULK_TEST

/**
  * @brief  Transmit BULK IN ZLP packet
  * @retval Status
  */
static int usbd_vendor_transmit_bulk_zlp(void)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;

	RTK_LOGS(TAG, "[VND] BULK TX ZLP\n");
	usbd_ep_transmit(cdev->dev, USBD_VENDOR_BULK_IN_EP, NULL, 0);

	return HAL_OK;
}

#endif

/**
  * @brief  Set Vendor class configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_vendor_set_config(usb_dev_t *dev, u8 config)
{
	u16 ep_mps;
	int ret = HAL_OK;
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;

	UNUSED(config);

	cdev->dev = dev;
	cdev->alt_setting = 0U;

#if CONFIG_USBD_VENDOR_INTR_TEST
	/* Init INTR IN EP */
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_INTR_MPS : USBD_VENDOR_FS_INTR_MPS;
	usbd_ep_init(dev, USBD_VENDOR_INTR_IN_EP, USB_CH_EP_TYPE_INTR, ep_mps);
	cdev->intr_in_state = 0;

	/* Init INTR OUT EP */
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_INTR_MPS : USBD_VENDOR_FS_INTR_MPS;
	usbd_ep_init(dev, USBD_VENDOR_INTR_OUT_EP, USB_CH_EP_TYPE_INTR, ep_mps);
	usbd_ep_receive(dev, USBD_VENDOR_INTR_OUT_EP, cdev->intr_out_buf, USBD_VENDOR_INTR_OUT_BUF_SIZE);
#endif

#if CONFIG_USBD_VENDOR_ISOC_TEST
	/* Init ISO IN EP */
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_ISOC_MPS : USBD_VENDOR_FS_ISOC_MPS;
	usbd_ep_init(dev, USBD_VENDOR_ISOC_IN_EP, USB_CH_EP_TYPE_ISOC, ep_mps);

	/* Init ISO OUT EP */
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_ISOC_MPS : USBD_VENDOR_FS_ISOC_MPS;
	usbd_ep_init(dev, USBD_VENDOR_ISOC_OUT_EP, USB_CH_EP_TYPE_ISOC, ep_mps);
	usbd_ep_receive(dev, USBD_VENDOR_ISOC_OUT_EP, cdev->isoc_out_buf, USBD_VENDOR_ISOC_OUT_BUF_SIZE);
#endif

	if (cdev->cb->set_config != NULL) {
		cdev->cb->set_config();
	}

	cdev->is_ready = 1U;

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

	UNUSED(config);

	cdev->is_ready = 0U;

	if (cdev->alt_setting == 0U) {
#if CONFIG_USBD_VENDOR_INTR_TEST
		/* DeInit INTR IN EP */
		usbd_ep_deinit(dev, USBD_VENDOR_INTR_IN_EP);

		/* DeInit INTR OUT EP */
		usbd_ep_deinit(dev, USBD_VENDOR_INTR_OUT_EP);
#endif
	} else {
#if CONFIG_USBD_VENDOR_BULK_TEST
		/* DeInit BULK IN EP */
		usbd_ep_deinit(dev, USBD_VENDOR_BULK_IN_EP);

		/* DeInit BULK OUT EP */
		usbd_ep_deinit(dev, USBD_VENDOR_BULK_OUT_EP);
#endif
	}

#if CONFIG_USBD_VENDOR_ISOC_TEST
	/* DeInit ISOC IN EP */
	usbd_ep_deinit(dev, USBD_VENDOR_ISOC_IN_EP);

	/* DeInit ISOC OUT EP */
	usbd_ep_deinit(dev, USBD_VENDOR_ISOC_OUT_EP);
#endif

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
	u16 ep_mps;
	int ret = HAL_OK;
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;

	//RTK_LOGD(TAG, "SETUP: bmRequestType=0x%02x bRequest=0x%02x wLength=0x%04x wValue=%x\n",
	//	 req->bmRequestType, req->bRequest, req->wLength, req->wValue);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				if (req->wIndex == 0U) {
					cdev->alt_setting = (u8)(req->wValue & 0xFF);
#if CONFIG_USBD_VENDOR_ISOC_TEST
					/* DeInit ISOC IN EP */
					usbd_ep_deinit(dev, USBD_VENDOR_ISOC_IN_EP);

					/* DeInit ISOC OUT EP */
					usbd_ep_deinit(dev, USBD_VENDOR_ISOC_OUT_EP);
#endif
					if (cdev->alt_setting == 0U) {
#if CONFIG_USBD_VENDOR_BULK_TEST
						/* DeInit BULK IN EP */
						usbd_ep_deinit(dev, USBD_VENDOR_BULK_IN_EP);

						/* DeInit BULK OUT EP */
						usbd_ep_deinit(dev, USBD_VENDOR_BULK_OUT_EP);
#endif
#if CONFIG_USBD_VENDOR_INTR_TEST
						/* Init INTR IN EP */
						ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_INTR_MPS : USBD_VENDOR_FS_INTR_MPS;
						usbd_ep_init(dev, USBD_VENDOR_INTR_IN_EP, USB_CH_EP_TYPE_INTR, ep_mps);
						cdev->intr_in_state = 0;

						/* Init INTR OUT EP */
						ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_INTR_MPS : USBD_VENDOR_FS_INTR_MPS;
						usbd_ep_init(dev, USBD_VENDOR_INTR_OUT_EP, USB_CH_EP_TYPE_INTR, ep_mps);
						usbd_ep_receive(dev, USBD_VENDOR_INTR_OUT_EP, cdev->intr_out_buf, USBD_VENDOR_INTR_OUT_BUF_SIZE);
#endif
					} else {
#if CONFIG_USBD_VENDOR_INTR_TEST
						/* DeInit INTR IN EP */
						usbd_ep_deinit(dev, USBD_VENDOR_INTR_IN_EP);

						/* DeInit INTR OUT EP */
						usbd_ep_deinit(dev, USBD_VENDOR_INTR_OUT_EP);
#endif
#if CONFIG_USBD_VENDOR_BULK_TEST
						/* Init BULK IN EP */
						ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_BULK_MPS : USBD_VENDOR_FS_BULK_MPS;
						usbd_ep_init(dev, USBD_VENDOR_BULK_IN_EP, USB_CH_EP_TYPE_BULK, ep_mps);
						cdev->bulk_in_state = 0;

						/* Init BULK OUT EP */
						ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_BULK_MPS : USBD_VENDOR_FS_BULK_MPS;
						usbd_ep_init(dev, USBD_VENDOR_BULK_OUT_EP, USB_CH_EP_TYPE_BULK, ep_mps);
						usbd_ep_receive(dev, USBD_VENDOR_BULK_OUT_EP, cdev->bulk_out_buf, USBD_VENDOR_BULK_OUT_BUF_SIZE);
#endif
					}
#if CONFIG_USBD_VENDOR_ISOC_TEST
					/* Init ISO IN EP */
					ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_ISOC_MPS : USBD_VENDOR_FS_ISOC_MPS;
					usbd_ep_init(dev, USBD_VENDOR_ISOC_IN_EP, USB_CH_EP_TYPE_ISOC, ep_mps);

					/* Init ISO OUT EP */
					ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_ISOC_MPS : USBD_VENDOR_FS_ISOC_MPS;
					usbd_ep_init(dev, USBD_VENDOR_ISOC_OUT_EP, USB_CH_EP_TYPE_ISOC, ep_mps);
					usbd_ep_receive(dev, USBD_VENDOR_ISOC_OUT_EP, cdev->isoc_out_buf, USBD_VENDOR_ISOC_OUT_BUF_SIZE);
#endif
				} else {
					ret = HAL_ERR_HW;
				}
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->ctrl_buf[0] = cdev->alt_setting;
				usbd_ep0_transmit(dev, cdev->ctrl_buf, 1U);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->ctrl_buf[0] = 0U;
				cdev->ctrl_buf[1] = 0U;
				usbd_ep0_transmit(dev, cdev->ctrl_buf, 2U);
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
			if (req->bmRequestType & 0x80U) {
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
  * @brief  Data sent on non-control IN endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_vendor_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usbd_vendor_cb_t *cb = cdev->cb;

	UNUSED(dev);

	if (cdev->alt_setting == 0U) {
#if CONFIG_USBD_VENDOR_INTR_TEST
		if (ep_addr == USBD_VENDOR_INTR_IN_EP) {
			cdev->intr_in_state = 0U;
			if (status == HAL_OK) {
				RTK_LOGS(TAG, "[VND] INTR TX done\n");
			} else {
				RTK_LOGS(TAG, "[VND] INTR TX err: %d\n", status);
			}
			if (cb->intr_transmitted != NULL) {
				cb->intr_transmitted(status);
			}
		}
#endif
	} else {
#if CONFIG_USBD_VENDOR_BULK_TEST
		if (ep_addr == USBD_VENDOR_BULK_IN_EP) {
			if (status == HAL_OK) {
				RTK_LOGS(TAG, "[VND] BULK TX done\n");
				if (cdev->bulk_in_zlp) {
					cdev->bulk_in_zlp = 0;
					usbd_vendor_transmit_bulk_zlp();
				} else {
					cdev->bulk_in_state = 0U;
				}
			} else {
				RTK_LOGS(TAG, "[VND] BULK TX err: %d\n", status);
				cdev->bulk_in_state = 0U;
			}
			if (cb->bulk_transmitted != NULL) {
				cb->bulk_transmitted(status);
			}
		}
#endif
	}

#if CONFIG_USBD_VENDOR_ISOC_TEST
	if (ep_addr == USBD_VENDOR_ISOC_IN_EP) {
		if (status == HAL_OK) {
			RTK_LOGS(TAG, "[VND] ISOC TX done\n");
		} else {
			RTK_LOGS(TAG, "[VND] ISOC TX err: %d\n", status);
		}
		if (cb->isoc_transmitted != NULL) {
			cb->isoc_transmitted(status);
		}
	}
#endif

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

	UNUSED(dev);

	if (len == 0) {
		/*RX ZLP*/
		return HAL_OK;
	}

	if (cdev->alt_setting == 0U) {
#if CONFIG_USBD_VENDOR_INTR_TEST
		if (ep_addr == USBD_VENDOR_INTR_OUT_EP) {
			cdev->cb->intr_received(cdev->intr_out_buf, len);
		}
#endif
	} else {
#if CONFIG_USBD_VENDOR_BULK_TEST
		if (ep_addr == USBD_VENDOR_BULK_OUT_EP) {
			cdev->cb->bulk_received(cdev->bulk_out_buf, len);
		}
#endif
	}

#if CONFIG_USBD_VENDOR_ISOC_TEST
	if (ep_addr == USBD_VENDOR_ISOC_OUT_EP) {
		cdev->cb->isoc_received(cdev->isoc_out_buf, len);
	}
#endif

	return HAL_OK;
}

/**
  * @brief  Get descriptor callback
  * @param  req: setup request
  * @param  speed: device speed
  * @param  len: descriptor length
  * @retval descriptor buffer
  */
static u8 *usbd_vendor_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len)
{
	u8 *buf = NULL;
	u8 *desc = usbd_vendor_dev.ctrl_buf;

	dev->self_powered = USBD_VENDOR_SELF_POWERED;

	switch ((req->wValue >> 8) & 0xFF) {

	case USB_DESC_TYPE_DEVICE:
		buf = usbd_vendor_dev_desc;
		*len = sizeof(usbd_vendor_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			buf = usbd_vendor_hs_config_desc;
			*len = sizeof(usbd_vendor_hs_config_desc);
		} else {
			buf = usbd_vendor_fs_config_desc;
			*len = sizeof(usbd_vendor_fs_config_desc);
		}
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		buf = usbd_vendor_device_qualifier_desc;
		*len = sizeof(usbd_vendor_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			buf = usbd_vendor_fs_config_desc;
			*len = sizeof(usbd_vendor_fs_config_desc);
		} else {
			buf = usbd_vendor_hs_config_desc;
			*len = sizeof(usbd_vendor_hs_config_desc);
		}
		usb_os_memcpy((void *)desc, (void *)buf, *len);
		desc[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		buf = desc;
		break;

	case USB_DESC_TYPE_STRING:
		switch (req->wValue & 0xFF) {
		case USBD_IDX_LANGID_STR:
			buf = usbd_vendor_lang_id_desc;
			*len = sizeof(usbd_vendor_lang_id_desc);
			break;
		case USBD_IDX_MFC_STR:
			usbd_get_str_desc(USBD_VENDOR_MFG_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_PRODUCT_STR:
			if (speed == USB_SPEED_HIGH) {
				usbd_get_str_desc(USBD_VENDOR_PROD_HS_STRING, desc, len);
			} else {
				usbd_get_str_desc(USBD_VENDOR_PROD_FS_STRING, desc, len);
			}
			buf = desc;
			break;
		case USBD_IDX_SERIAL_STR:
			usbd_get_str_desc(USBD_VENDOR_SN_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_MS_OS_STR:
			/*Not support*/
			break;
		/* Add customer string here */
		default:
			RTK_LOGS(TAG, "[VND] Invalid str idx %d\n", req->wValue & 0xFF);
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
  * @param  config: USB USB attach status
  * @retval void
  */
static void usbd_vendor_status_changed(usb_dev_t *dev, u8 status)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;

	UNUSED(dev);

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		cdev->is_ready = 0U;
	}

	if (cdev->cb->status_changed) {
		cdev->cb->status_changed(status);
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

	cdev->ctrl_buf = (u8 *)usb_os_malloc(USBD_VENDOR_CTRL_BUF_SIZE);
	if (cdev->ctrl_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_exit;
	}

	cdev->bulk_out_buf = (u8 *)usb_os_malloc(USBD_VENDOR_BULK_OUT_BUF_SIZE);
	if (cdev->bulk_out_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_ctrl_buf_exit;
	}

	cdev->bulk_in_buf = (u8 *)usb_os_malloc(USBD_VENDOR_BULK_IN_BUF_SIZE);
	if (cdev->bulk_in_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_bulk_out_buf_exit;
	}

	cdev->intr_out_buf = (u8 *)usb_os_malloc(USBD_VENDOR_INTR_OUT_BUF_SIZE);
	if (cdev->intr_out_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_bulk_in_buf_exit;
	}

	cdev->intr_in_buf = (u8 *)usb_os_malloc(USBD_VENDOR_INTR_IN_BUF_SIZE);
	if (cdev->intr_in_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_intr_out_buf_exit;
	}

	cdev->isoc_out_buf = (u8 *)usb_os_malloc(USBD_VENDOR_ISOC_OUT_BUF_SIZE);
	if (cdev->isoc_out_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_intr_in_buf_exit;
	}

	cdev->isoc_in_buf = (u8 *)usb_os_malloc(USBD_VENDOR_ISOC_IN_BUF_SIZE);
	if (cdev->isoc_in_buf == NULL) {
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
	usb_os_mfree(cdev->isoc_in_buf);
	cdev->isoc_in_buf = NULL;

init_clean_isoc_out_buf_exit:
	usb_os_mfree(cdev->isoc_out_buf);
	cdev->isoc_out_buf = NULL;

init_clean_intr_in_buf_exit:
	usb_os_mfree(cdev->intr_in_buf);
	cdev->intr_in_buf = NULL;

init_clean_intr_out_buf_exit:
	usb_os_mfree(cdev->intr_out_buf);
	cdev->intr_out_buf = NULL;

init_clean_bulk_in_buf_exit:
	usb_os_mfree(cdev->bulk_in_buf);
	cdev->bulk_in_buf = NULL;

init_clean_bulk_out_buf_exit:
	usb_os_mfree(cdev->bulk_out_buf);
	cdev->bulk_out_buf = NULL;

init_clean_ctrl_buf_exit:
	usb_os_mfree(cdev->ctrl_buf);
	cdev->ctrl_buf = NULL;

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

	cdev->is_ready = 0U;

	if (cdev->cb != NULL) {
		if (cdev->cb->deinit != NULL) {
			cdev->cb->deinit();
		}
		cdev->cb = NULL;
	}

	usbd_unregister_class();

	if (cdev->ctrl_buf != NULL) {
		usb_os_mfree(cdev->ctrl_buf);
		cdev->ctrl_buf = NULL;
	}

	if (cdev->bulk_in_buf != NULL) {
		usb_os_mfree(cdev->bulk_in_buf);
		cdev->bulk_in_buf = NULL;
	}

	if (cdev->bulk_out_buf != NULL) {
		usb_os_mfree(cdev->bulk_out_buf);
		cdev->bulk_out_buf = NULL;
	}

	if (cdev->intr_in_buf != NULL) {
		usb_os_mfree(cdev->intr_in_buf);
		cdev->intr_in_buf = NULL;
	}

	if (cdev->intr_out_buf != NULL) {
		usb_os_mfree(cdev->intr_out_buf);
		cdev->intr_out_buf = NULL;
	}

	if (cdev->isoc_in_buf != NULL) {
		usb_os_mfree(cdev->isoc_in_buf);
		cdev->isoc_in_buf = NULL;
	}

	if (cdev->isoc_out_buf != NULL) {
		usb_os_mfree(cdev->isoc_out_buf);
		cdev->isoc_out_buf = NULL;
	}

	return HAL_OK;
}

int usbd_vendor_transmit_ctrl_data(u8 *buf, u16 len)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usb_dev_t *dev = cdev->dev;
	u16 ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USB_HS_MAX_PACKET_SIZE : USB_FS_MAX_PACKET_SIZE;

	if (!cdev->is_ready) {
		return HAL_ERR_HW;
	}

	if (len > ep_mps) {
		len = ep_mps;
	}

	RTK_LOGS(TAG, "[VND] CTRL TX len=%d\n", len);

	usb_os_memcpy(cdev->ctrl_buf, buf, len);

	usbd_ep0_transmit(dev, cdev->ctrl_buf, len);

	return HAL_OK;
}

int usbd_vendor_transmit_bulk_data(u8 *buf, u16 len)
{
	int ret = HAL_OK;
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;
	usb_dev_t *dev = cdev->dev;
	u16 ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_VENDOR_HS_BULK_MPS : USBD_VENDOR_FS_BULK_MPS;

	if (!cdev->is_ready) {
		return HAL_ERR_HW;
	}

	if (len > USBD_VENDOR_BULK_IN_BUF_SIZE) {
		len = USBD_VENDOR_BULK_IN_BUF_SIZE;
	}

	if (len == ep_mps) {
		cdev->bulk_in_zlp = 1;
	} else {
		cdev->bulk_in_zlp = 0;
	}

	if (cdev->bulk_in_state == 0U) {
		RTK_LOGS(TAG, "[VND] BULK TX len=%d data=%d\n", len, buf[0]);
		cdev->bulk_in_state = 1U;
		usb_os_memcpy((void *)cdev->bulk_in_buf, (void *)buf, len);
		usbd_ep_transmit(cdev->dev, USBD_VENDOR_BULK_IN_EP, cdev->bulk_in_buf, len);
	} else {
		RTK_LOGS(TAG, "[VND] BULK TX len=%d data=%d: BUSY\n", len, buf[0]);
		ret = HAL_BUSY;
	}

	return ret;
}

int usbd_vendor_receive_bulk_data(void)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;

	if (!cdev->is_ready) {
		return HAL_ERR_HW;
	}

	usbd_ep_receive(cdev->dev, USBD_VENDOR_BULK_OUT_EP, cdev->bulk_out_buf, USBD_VENDOR_BULK_OUT_BUF_SIZE);

	return HAL_OK;
}

int usbd_vendor_transmit_intr_data(u8 *buf, u16 len)
{
	int ret = HAL_OK;
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;

	if (!cdev->is_ready) {
		return HAL_ERR_HW;
	}

	if (len > USBD_VENDOR_INTR_IN_BUF_SIZE) {
		len = USBD_VENDOR_INTR_IN_BUF_SIZE;
	}

	if (cdev->intr_in_state == 0U) {
		RTK_LOGS(TAG, "[VND] INTR TX len=%d data=%d\n", len, buf[0]);
		cdev->intr_in_state = 1U;
		usb_os_memcpy((void *)cdev->intr_in_buf, (void *)buf, len);
		usbd_ep_transmit(cdev->dev, USBD_VENDOR_INTR_IN_EP, cdev->intr_in_buf, len);
	} else {
		RTK_LOGS(TAG, "[VND] INTR TX len=%d data=%d: BUSY\n", len, buf[0]);
		ret = HAL_BUSY;
	}

	return ret;
}

int usbd_vendor_receive_intr_data(void)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;

	if (!cdev->is_ready) {
		return HAL_ERR_HW;
	}

	usbd_ep_receive(cdev->dev, USBD_VENDOR_INTR_OUT_EP, cdev->intr_out_buf, USBD_VENDOR_INTR_OUT_BUF_SIZE);

	return HAL_OK;
}

int usbd_vendor_transmit_isoc_data(u8 *buf, u16 len)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;

	if (!cdev->is_ready) {
		return HAL_ERR_HW;
	}

	if (len > USBD_VENDOR_ISOC_IN_BUF_SIZE) {
		len = USBD_VENDOR_ISOC_IN_BUF_SIZE;
	}

	RTK_LOGS(TAG, "[VND] ISOC TX len=%d data=%d\n", len, buf[0]);
	usb_os_memcpy(cdev->isoc_in_buf, buf, len);
	usbd_ep_transmit(cdev->dev, USBD_VENDOR_ISOC_IN_EP, cdev->isoc_in_buf, len);

	return HAL_OK;
}

int usbd_vendor_receive_isoc_data(void)
{
	usbd_vendor_dev_t *cdev = &usbd_vendor_dev;

	if (!cdev->is_ready) {
		return HAL_ERR_HW;
	}

	usbd_ep_receive(cdev->dev, USBD_VENDOR_ISOC_OUT_EP, cdev->isoc_out_buf, USBD_VENDOR_ISOC_OUT_BUF_SIZE);

	return HAL_OK;
}

