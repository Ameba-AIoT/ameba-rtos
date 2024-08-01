/**
  ******************************************************************************
  * @file    usbd_composite_cdc_acm.c
  * @author  Realsil WLAN5 Team
  * @brief   This file provides the functionalities of the USB CDC ACM Class
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

#include "usbd_composite_cdc_acm.h"
#include "usbd.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int composite_cdc_acm_set_config(usb_dev_t *dev, u8 config);
static int composite_cdc_acm_clear_config(usb_dev_t *dev, u8 config);
static int composite_cdc_acm_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u8 *composite_cdc_acm_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len);
static int composite_cdc_acm_handle_ep0_data_out(usb_dev_t *dev);
static int composite_cdc_acm_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int composite_cdc_acm_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "COMP";

/* USB CDC ACM Device High Speed Interface Descriptor */
static u8 usbd_composite_cdc_acm_hs_itf_desc[] USB_DMA_ALIGNED = {
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,								/* bLength: IAD Descriptor size */
	USB_DESC_TYPE_IAD,								/* bDescriptorType: IAD */
	USBD_COMP_CDC_COM_ITF,							/* bFirstInterface */
	0x02,											/* bInterfaceCount */
	0x02,											/* bFunctionClass: CDC */
	0x02,											/* bFunctionSubClass: Abstract Control Model */
	0x00,											/* bFunctionProtocol: 0x00: No class specific protocol required, 0x01: Common AT commands */
	USBD_IDX_CDC_ITF_STR,							/* iFunction */

	/* CDC Communication Interface Descriptor */
	USB_LEN_IF_DESC,								/* bLength */
	USB_DESC_TYPE_INTERFACE,						/* bDescriptorType */
	USBD_COMP_CDC_COM_ITF,							/* bInterfaceNumber */
	0x00,											/* bAlternateSetting */
	0x01,											/* bNumEndpoints */
	0x02,											/* bInterfaceClass: CDC */
	0x02,											/* bInterfaceSubClass: Abstract Control Model */
	0x00,											/* bInterfaceProtocol: Common AT commands */
	0x00,											/* iInterface */

	/* CDC Header Functional Descriptor */
	0x05,											/* bLength */
	0x24,											/* bDescriptorType: CS_INTERFACE */
	0x00,											/* bDescriptorSubtype: Header Functional Descriptor */
	0x10, 0x01,										/* bcdCDC */

	/* CDC Call Management Functional Descriptor */
	0x05,											/* bFunctionLength */
	0x24,											/* bDescriptorType: CS_INTERFACE */
	0x01,											/* bDescriptorSubtype: Call Management Functional Descriptor */
	0x00,											/* bmCapabilities: D0+D1 */
	0x01,											/* bDataInterface */

	/* CDC ACM Functional Descriptor */
	0x04,											/* bFunctionLength */
	0x24,											/* bDescriptorType: CS_INTERFACE */
	0x02,											/* bDescriptorSubtype: ACM Functional Descriptor */
	0x02,											/* bmCapabilities */

	/* CDC Union Functional Descriptor */
	0x05,											/* bFunctionLength */
	0x24,											/* bDescriptorType: CS_INTERFACE */
	0x06,											/* bDescriptorSubtype: Union Functional Descriptor */
	0x00,											/* bMasterInterface: Communication Class Interface */
	USBD_COMP_CDC_DAT_ITF,							/* bSlaveInterface0: Data Class Interface */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,								/* bLength */
	USB_DESC_TYPE_ENDPOINT,							/* bDescriptorType */
	USBD_COMP_CDC_INTR_IN_EP,						/* bEndpointAddress */
	USB_CH_EP_TYPE_INTR,							/* bmAttributes: INTR */
	USB_LOW_BYTE(COMP_CDC_ACM_INTR_IN_PACKET_SIZE), USB_HIGH_BYTE(COMP_CDC_ACM_INTR_IN_PACKET_SIZE),	/* wMaxPacketSize */
	COMP_CDC_ACM_HS_INTR_IN_INTERVAL,				/* bInterval */

	/* CDC Data Interface Descriptor */
	USB_LEN_IF_DESC,								/* bLength */
	USB_DESC_TYPE_INTERFACE,						/* bDescriptorType: */
	USBD_COMP_CDC_DAT_ITF,							/* bInterfaceNumber */
	0x00,											/* bAlternateSetting */
	0x02,											/* bNumEndpoints */
	0x0A,											/* bInterfaceClass: CDC */
	0x00,											/* bInterfaceSubClass */
	0x00,											/* bInterfaceProtocol */
	0x00,											/* iInterface */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,								/* bLength */
	USB_DESC_TYPE_ENDPOINT,							/* bDescriptorType */
	USBD_COMP_CDC_BULK_OUT_EP,						/* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,							/* bmAttributes: BULK */
	USB_LOW_BYTE(COMP_CDC_ACM_HS_BULK_MAX_PACKET_SIZE), USB_HIGH_BYTE(COMP_CDC_ACM_HS_BULK_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	0x00,											/* bInterval */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,								/* bLength */
	USB_DESC_TYPE_ENDPOINT,							/* bDescriptorType */
	USBD_COMP_CDC_BULK_IN_EP,						/* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,							/* bmAttributes: BULK */
	USB_LOW_BYTE(COMP_CDC_ACM_HS_BULK_MAX_PACKET_SIZE), USB_HIGH_BYTE(COMP_CDC_ACM_HS_BULK_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	0x00											/* bInterval */
};  /* usbd_composite_cdc_acm_hs_itf_desc */

/* USB CDC ACM Device Full Speed Interface Descriptor */
static u8 usbd_composite_cdc_acm_fs_itf_desc[] USB_DMA_ALIGNED = {
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,								/* bLength: IAD Descriptor size */
	USB_DESC_TYPE_IAD,								/* bDescriptorType: IAD */
	USBD_COMP_CDC_COM_ITF,							/* bFirstInterface */
	0x02,											/* bInterfaceCount */
	0x02,											/* bFunctionClass: CDC */
	0x02,											/* bFunctionSubClass: Abstract Control Model */
	0x00,											/* bFunctionProtocol: 0x00: No class specific protocol required, 0x01: Common AT commands */
	USBD_IDX_CDC_ITF_STR,							/* iFunction */

	/* CDC Communication Interface Descriptor */
	USB_LEN_IF_DESC,								/* bLength */
	USB_DESC_TYPE_INTERFACE,						/* bDescriptorType */
	USBD_COMP_CDC_COM_ITF,							/* bInterfaceNumber */
	0x00,											/* bAlternateSetting */
	0x01,											/* bNumEndpoints */
	0x02,											/* bInterfaceClass: CDC */
	0x02,											/* bInterfaceSubClass: Abstract Control Model */
	0x00,											/* bInterfaceProtocol: Common AT commands */
	0x00,											/* iInterface */

	/* CDC Header Functional Descriptor */
	0x05,											/* bLength */
	0x24,											/* bDescriptorType: CS_INTERFACE */
	0x00,											/* bDescriptorSubtype: Header Functional Descriptor */
	0x10, 0x01,										/* bcdCDC */

	/* CDC Call Management Functional Descriptor */
	0x05,											/* bFunctionLength */
	0x24,											/* bDescriptorType: CS_INTERFACE */
	0x01,											/* bDescriptorSubtype: Call Management Functional Descriptor */
	0x00,											/* bmCapabilities: D0+D1 */
	0x01,											/* bDataInterface */

	/* CDC ACM Functional Descriptor */
	0x04,											/* bFunctionLength */
	0x24,											/* bDescriptorType: CS_INTERFACE */
	0x02,											/* bDescriptorSubtype: ACM Functional Descriptor */
	0x02,											/* bmCapabilities */

	/* CDC Union Functional Descriptor */
	0x05,											/* bFunctionLength */
	0x24,											/* bDescriptorType: CS_INTERFACE */
	0x06,											/* bDescriptorSubtype: Union Functional Descriptor */
	0x00,											/* bMasterInterface: Communication Class Interface */
	USBD_COMP_CDC_DAT_ITF,							/* bSlaveInterface0: Data Class Interface */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,								/* bLength */
	USB_DESC_TYPE_ENDPOINT,							/* bDescriptorType */
	USBD_COMP_CDC_INTR_IN_EP,						/* bEndpointAddress */
	USB_CH_EP_TYPE_INTR,							/* bmAttributes: INTR */
	USB_LOW_BYTE(COMP_CDC_ACM_INTR_IN_PACKET_SIZE), USB_HIGH_BYTE(COMP_CDC_ACM_INTR_IN_PACKET_SIZE),	/* wMaxPacketSize */
	COMP_CDC_ACM_FS_INTR_IN_INTERVAL,				/* bInterval: */

	/* CDC Data Interface Descriptor */
	USB_LEN_IF_DESC,								/* bLength */
	USB_DESC_TYPE_INTERFACE,						/* bDescriptorType: */
	USBD_COMP_CDC_DAT_ITF,							/* bInterfaceNumber */
	0x00,											/* bAlternateSetting */
	0x02,											/* bNumEndpoints */
	0x0A,											/* bInterfaceClass: CDC */
	0x00,											/* bInterfaceSubClass */
	0x00,											/* bInterfaceProtocol */
	0x00,											/* iInterface */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,								/* bLength */
	USB_DESC_TYPE_ENDPOINT,							/* bDescriptorType */
	USBD_COMP_CDC_BULK_OUT_EP,						/* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,							/* bmAttributes: BULK */
	USB_LOW_BYTE(COMP_CDC_ACM_FS_BULK_MAX_PACKET_SIZE), USB_HIGH_BYTE(COMP_CDC_ACM_FS_BULK_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	0x00,											/* bInterval */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,								/* bLength */
	USB_DESC_TYPE_ENDPOINT,							/* bDescriptorType */
	USBD_COMP_CDC_BULK_IN_EP,						/* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,							/* bmAttributes: BULK */
	USB_LOW_BYTE(COMP_CDC_ACM_FS_BULK_MAX_PACKET_SIZE), USB_HIGH_BYTE(COMP_CDC_ACM_FS_BULK_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	0x00											/* bInterval */
};  /* usbd_composite_cdc_acm_fs_itf_desc */

/* CDC ACM Class Driver */
usbd_class_driver_t usbd_composite_cdc_acm_driver = {
	.get_descriptor = composite_cdc_acm_get_descriptor,
	.set_config = composite_cdc_acm_set_config,
	.clear_config = composite_cdc_acm_clear_config,
	.setup = composite_cdc_acm_setup,
	.ep_data_in = composite_cdc_acm_handle_ep_data_in,
	.ep_data_out = composite_cdc_acm_handle_ep_data_out,
	.ep0_data_out = composite_cdc_acm_handle_ep0_data_out,
};

/* CDC ACM Device */
static usbd_composite_cdc_acm_dev_t composite_cdc_acm_dev;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Set CDC class configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int composite_cdc_acm_set_config(usb_dev_t *dev, u8 config)
{
	u16 ep_mps;
	int ret = HAL_OK;
	usbd_composite_cdc_acm_dev_t *cdc = &composite_cdc_acm_dev;

	UNUSED(config);

	/* Init BULK IN state */
	cdc->bulk_in_state = 0U;
	cdc->bulk_out_zlp = 0U;

	/* Init BULK IN EP */
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? COMP_CDC_ACM_HS_BULK_IN_PACKET_SIZE : COMP_CDC_ACM_FS_BULK_IN_PACKET_SIZE;
	usbd_ep_init(dev, USBD_COMP_CDC_BULK_IN_EP, USB_CH_EP_TYPE_BULK, ep_mps);

	/* Init BULK OUT EP */
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? COMP_CDC_ACM_HS_BULK_OUT_PACKET_SIZE : COMP_CDC_ACM_FS_BULK_OUT_PACKET_SIZE;
	usbd_ep_init(dev, USBD_COMP_CDC_BULK_OUT_EP, USB_CH_EP_TYPE_BULK, ep_mps);

	/* Init INTR IN EP */
	usbd_ep_init(dev, USBD_COMP_CDC_INTR_IN_EP, USB_CH_EP_TYPE_INTR, COMP_CDC_ACM_INTR_IN_PACKET_SIZE);

	/* Prepare to receive next BULK OUT packet */
	usbd_ep_receive(dev, USBD_COMP_CDC_BULK_OUT_EP, cdc->bulk_out_buf, cdc->bulk_out_buf_size);

	cdc->is_ready = 1U;

	return ret;
}

/**
  * @brief  Clear CDC ACM configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int composite_cdc_acm_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0U;
	usbd_composite_cdc_acm_dev_t *cdc = &composite_cdc_acm_dev;

	UNUSED(config);

	cdc->is_ready = 0U;

	/* DeInit BULK IN EP */
	usbd_ep_deinit(dev, USBD_COMP_CDC_BULK_IN_EP);

	/* DeInit BULK OUT EP */
	usbd_ep_deinit(dev, USBD_COMP_CDC_BULK_OUT_EP);

	/* DeInit INTR IN EP */
	usbd_ep_deinit(dev, USBD_COMP_CDC_INTR_IN_EP);

	return ret;
}

/**
  * @brief  Handle CDC specific CTRL requests
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int composite_cdc_acm_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_composite_cdc_acm_dev_t *cdc = &composite_cdc_acm_dev;
	usbd_composite_dev_t *cdev = cdc->cdev;
	int ret = HAL_OK;

	//RTK_LOGD(TAG, "SETUP: bmRequestType=0x%02x bRequest=0x%02x wLength=0x%04x wValue=%x\n",
	//		 req->bmRequestType, req->bRequest, req->wLength, req->wValue);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_CLASS:
		if (req->wLength) {
			if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
				ret = cdc->cb->setup(req, cdev->ctrl_buf);
				if (ret == HAL_OK) {
					usbd_ep0_transmit(dev, cdev->ctrl_buf, req->wLength);
				}
			} else {
				usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
			}
		} else {
			ret = cdc->cb->setup(req, cdev->ctrl_buf);
		}
		break;
	default:
		RTK_LOGS(TAG, "[COMP] Invalid bRequest 0x%02x\n", req->bRequest);
		ret = HAL_ERR_HW;
		break;
	}

	return ret;
}

/**
  * @brief  Transmit BULK IN ZLP packet
  * @retval Status
  */
static int composite_cdc_acm_transmit_zlp(void)
{
	usbd_composite_cdc_acm_dev_t *cdc = &composite_cdc_acm_dev;
	usb_dev_t *dev = cdc->cdev->dev;

	usbd_ep_transmit(dev, USBD_COMP_CDC_BULK_IN_EP, NULL, 0);

	return HAL_OK;
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int composite_cdc_acm_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_composite_cdc_acm_dev_t *cdc = &composite_cdc_acm_dev;

	UNUSED(dev);

	if (status == HAL_OK) {
		/*TX done*/
		if (ep_addr == USBD_COMP_CDC_BULK_IN_EP) {
			if (cdc->bulk_out_zlp) {
				cdc->bulk_out_zlp = 0;
				composite_cdc_acm_transmit_zlp();
			} else {
				cdc->bulk_in_state = 0U;
			}
		}
#if CONFIG_COMP_CDC_ACM_NOTIFY
		else if (ep_addr == USBD_COMP_CDC_INTR_IN_EP) {
			cdc->intr_in_state = 0U;
		}
#endif
	} else {
		RTK_LOGS(TAG, "[COMP] EP%02x TX fail: %d\n", ep_addr, status);
		if (ep_addr == USBD_COMP_CDC_BULK_IN_EP) {
			cdc->bulk_in_state = 0U;
		}
#if CONFIG_COMP_CDC_ACM_NOTIFY
		else if (ep_addr == USBD_COMP_CDC_INTR_IN_EP) {
			cdc->intr_in_state = 0U;
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
static int composite_cdc_acm_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len)
{
	usbd_composite_cdc_acm_dev_t *cdc = &composite_cdc_acm_dev;

	UNUSED(dev);

	if (ep_addr == USBD_COMP_CDC_BULK_OUT_EP) {
		if ((cdc->cb->received) && (len > 0)) {
			cdc->cb->received(cdc->bulk_out_buf, len);
		}
	}

	usbd_ep_receive(cdc->cdev->dev, USBD_COMP_CDC_BULK_OUT_EP, cdc->bulk_out_buf, cdc->bulk_out_buf_size);

	return HAL_OK;
}

/**
  * @brief  Handle EP0 Rx Ready event
  * @param  dev: USB device instance
  * @retval Status
  */
static int composite_cdc_acm_handle_ep0_data_out(usb_dev_t *dev)
{
	int ret = HAL_ERR_HW;
	usbd_composite_cdc_acm_dev_t *cdc = &composite_cdc_acm_dev;
	usbd_composite_dev_t *cdev = cdc->cdev;

	UNUSED(dev);

	if (cdev->ctrl_req.bRequest != 0xFFU) {
		if (cdc->cb->setup) {
			cdc->cb->setup(&cdev->ctrl_req, cdev->ctrl_buf);
		}
		cdev->ctrl_req.bRequest = 0xFFU;

		ret = HAL_OK;
	}

	return ret;
}

static u8 *composite_cdc_acm_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len)
{
	u8 *buf = NULL;

	UNUSED(dev);

	switch ((req->wValue >> 8) & 0xFF) {

	case USB_DESC_TYPE_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			buf = usbd_composite_cdc_acm_hs_itf_desc;
			*len = sizeof(usbd_composite_cdc_acm_hs_itf_desc);
		} else {
			buf = usbd_composite_cdc_acm_fs_itf_desc;
			*len = sizeof(usbd_composite_cdc_acm_fs_itf_desc);
		}
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			buf = usbd_composite_cdc_acm_fs_itf_desc;
			*len = sizeof(usbd_composite_cdc_acm_fs_itf_desc);
		} else {
			buf = usbd_composite_cdc_acm_hs_itf_desc;
			*len = sizeof(usbd_composite_cdc_acm_hs_itf_desc);
		}
		break;

	default:
		break;
	}

	return buf;
}

#if CONFIG_COMP_CDC_ACM_NOTIFY
/**
  * @brief  Transmit INTR IN packet
  * @param  type: notification type
  * @param  value: notification value
  * @param  buf: data buffer
  * @param  len: data length
  * @retval Status
  */
static int composite_acm_cdc_notify(u8 type, u16 value, void *data, u16 len)
{
	int ret = HAL_ERR_HW;
	usbd_composite_cdc_acm_dev_t *cdc = &composite_cdc_acm_dev;
	usb_dev_t *dev = cdc->dev;
	usbd_composite_cdc_acm_ntf_t *ntf = cdc->intr_in_buf;

	if (!cdc->is_ready) {
		RTK_LOGS(TAG, "[COMP] EP%02x TX %d not ready\n", USBD_COMP_CDC_INTR_IN_EP, len);
		return ret;
	}

	if (len > COMP_CDC_ACM_INTR_IN_DATA_SIZE) {
		len = COMP_CDC_ACM_INTR_IN_DATA_SIZE;
	}

	if (cdc->intr_in_state == 0U) {
		if (cdc->is_ready) {
			cdc->is_intr_in_busy = 1U;
			cdc->intr_in_state = 1U;

			ntf->bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
			ntf->bNotificationType = type;
			ntf->wValue = value;
			ntf->wIndex = 0;
			ntf->wLength = len;

			usb_os_memcpy((void *)ntf->buf, (void *)data, len);

			if (cdc->is_ready) {
				usbd_ep_transmit(dev, USBD_COMP_CDC_INTR_IN_EP, (u8 *)ntf, COMP_CDC_ACM_INTR_IN_REQUEST_SIZE + len);
				ret = HAL_OK;
			} else {
				cdc->intr_in_state = 0U;
			}

			cdc->is_intr_in_busy = 0U;
		} else  {
			/* TX not ready*/
		}
	} else {
		RTK_LOGS(TAG, "[COMP] EP%02x TX %d busy\n", USBD_COMP_CDC_INTR_IN_EP, len);
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
int usbd_composite_cdc_acm_init(usbd_composite_dev_t *cdev, u16 bulk_out_xfer_size, u16 bulk_in_xfer_size, usbd_composite_cdc_acm_usr_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_composite_cdc_acm_dev_t *cdc = &composite_cdc_acm_dev;

	cdc->bulk_out_zlp = 0U;

	cdc->bulk_out_buf_size = bulk_out_xfer_size;
	cdc->bulk_out_buf = (u8 *)usb_os_malloc(cdc->bulk_out_buf_size);
	if (cdc->bulk_out_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto usbd_composite_cdc_acm_init_exit;
	}

	cdc->bulk_in_buf_size = bulk_in_xfer_size;
	cdc->bulk_in_buf = (u8 *)usb_os_malloc(cdc->bulk_in_buf_size);
	if (cdc->bulk_in_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto usbd_composite_cdc_acm_init_clean_bulk_out_buf_exit;
	}

#if CONFIG_COMP_CDC_ACM_NOTIFY
	cdc->intr_in_buf = (usbd_composite_cdc_acm_ntf_t *)usb_os_malloc(sizeof(usbd_composite_cdc_acm_ntf_t));
	if (cdc->intr_in_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto usbd_composite_cdc_acm_init_clean_bulk_in_buf_exit;
	}
#endif

	if (cb != NULL) {
		cdc->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				goto usbd_composite_cdc_acm_init_;
			}
		}
	}

	cdc->cdev = cdev;

	return ret;

usbd_composite_cdc_acm_init_:

#if CONFIG_COMP_CDC_ACM_NOTIFY
	usb_os_mfree(cdc->intr_in_buf);
	cdc->intr_in_buf = NULL;

usbd_composite_cdc_acm_init_clean_bulk_in_buf_exit:
#endif

	usb_os_mfree(cdc->bulk_in_buf);
	cdc->bulk_in_buf = NULL;

usbd_composite_cdc_acm_init_clean_bulk_out_buf_exit:
	usb_os_mfree(cdc->bulk_out_buf);
	cdc->bulk_out_buf = NULL;

usbd_composite_cdc_acm_init_exit:
	return ret;
}

/**
  * @brief  DeInit CDC ACM class
  * @param  void
  * @retval Status
  */
int usbd_composite_cdc_acm_deinit(void)
{
	u8 is_busy;
	usbd_composite_cdc_acm_dev_t *cdc = &composite_cdc_acm_dev;

	cdc->is_ready = 0U;

#if CONFIG_COMP_CDC_ACM_NOTIFY
	is_busy = cdc->is_bulk_in_busy || cdc->is_intr_in_busy;
#else
	is_busy = cdc->is_bulk_in_busy;
#endif
	while (is_busy) {
		usb_os_delay_us(100);
	}

	if ((cdc->cb != NULL) && (cdc->cb->deinit != NULL)) {
		cdc->cb->deinit();
	}

#if CONFIG_COMP_CDC_ACM_NOTIFY
	if (cdc->intr_in_buf != NULL) {
		usb_os_mfree(cdc->intr_in_buf);
		cdc->intr_in_buf = NULL;
	}
#endif

	if (cdc->bulk_in_buf != NULL) {
		usb_os_mfree(cdc->bulk_in_buf);
		cdc->bulk_in_buf = NULL;
	}

	if (cdc->bulk_out_buf != NULL) {
		usb_os_mfree(cdc->bulk_out_buf);
		cdc->bulk_out_buf = NULL;
	}

	return HAL_OK;
}

/**
  * @brief  Transmit BULK IN packet
  * @param  buf: data buffer
  * @param  len: data length
  * @retval Status
  */
int usbd_composite_cdc_acm_transmit(u8       *buf, u16 len)
{
	int ret = HAL_ERR_HW;
	usbd_composite_cdc_acm_dev_t *cdc = &composite_cdc_acm_dev;
	usb_dev_t *dev = cdc->cdev->dev;

	if (!cdc->is_ready) {
		RTK_LOGS(TAG, "[COMP] EP%02x TX %d not ready\n", USBD_COMP_CDC_BULK_IN_EP, len);
		return ret;
	}

	if (len > cdc->bulk_in_buf_size) {
		len = cdc->bulk_in_buf_size;
	}

	/* As per USB SPEC for bulk transfer, the transfer ends with a ZLP or a packet whose size
	 * is less than the endpoint max packet size.
	 */
	if (len == cdc->bulk_in_buf_size) {
		cdc->bulk_out_zlp = 1;
	} else {
		cdc->bulk_out_zlp = 0;
	}

	if (cdc->bulk_in_state == 0U) {
		if (cdc->is_ready) {
			cdc->is_bulk_in_busy = 1U;
			cdc->bulk_in_state = 1U;

			usb_os_memcpy((void *)cdc->bulk_in_buf, (void *)buf, len);

			if (cdc->is_ready) {
				usbd_ep_transmit(dev, USBD_COMP_CDC_BULK_IN_EP, cdc->bulk_in_buf, len);
				ret = HAL_OK;
			} else {
				cdc->bulk_in_state = 0U;
			}

			cdc->is_bulk_in_busy = 0U;
		} else  {
			/*TX not ready*/
		}
	} else {
		RTK_LOGS(TAG, "[COMP] EP%02x TX %d busy\n", USBD_COMP_CDC_BULK_IN_EP, len);
		ret = HAL_BUSY;
	}

	return ret;
}

#if CONFIG_COMP_CDC_ACM_NOTIFY
int usbd_composite_cdc_acm_notify_serial_state(u16 serial_state)
{
	return composite_acm_cdc_notify(COMP_CDC_NOTIFY_SERIAL_STATE, 0, &serial_state, sizeof(serial_state));
}
#endif

