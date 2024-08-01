/**
  ******************************************************************************
  * @file    usbd_verify.c
  * @author  Realsil WLAN5 Team
  * @brief   This file provides the functionalities of the USB Verify Class
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_hal.h"
#include "ameba_usb.h"
#include "usbd_verify.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#define DUMP_DATA_MAX_LEN 			(10)

/* Private function prototypes -----------------------------------------------*/
/* common */
static int usbd_verify_set_config(usb_dev_t *dev, u8 config);
static int usbd_verify_clear_config(usb_dev_t *dev, u8 config);
static int usbd_verify_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u8 *usbd_verify_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len);
static int usbd_verify_handle_ep0_data_in(usb_dev_t *dev, u8 status);
static int usbd_verify_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_verify_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_verify_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "VRY";

static const char *TEXT_CTRL = "CTRL";
static const char *TEXT_BULK = "BULK";
static const char *TEXT_INTR = "INTR";
static const char *TEXT_ISOC = "ISOC";

/*
	common description
*/
//#define USB_DMA_ALIGNED
/* USB Standard Device Descriptor */
static u8 usbd_verify_dev_desc[USB_LEN_DEV_DESC] USB_DMA_ALIGNED = {
	USB_LEN_DEV_DESC,                               /* bLength */
	USB_DESC_TYPE_DEVICE,                           /* bDescriptorType */
	0x00,                                           /* bcdUSB */
	0x02,
	0x00,                                           /* bDeviceClass */
	0x00,                                           /* bDeviceSubClass */
	0x00,                                           /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize */
	USB_LOW_BYTE(USBD_VERIFY_DEFAULT_VID),          /* idVendor */
	USB_HIGH_BYTE(USBD_VERIFY_DEFAULT_VID),
	USB_LOW_BYTE(USBD_VERIFY_DEFAULT_PID),          /* idProduct */
	USB_HIGH_BYTE(USBD_VERIFY_DEFAULT_PID),
	0x00,                                           /* bcdDevice */
	0x02,
	USBD_IDX_MFC_STR,                               /* iManufacturer */
	USBD_IDX_PRODUCT_STR,                           /* iProduct */
	USBD_IDX_SERIAL_STR,                            /* iSerialNumber */
	0x01                                            /* bNumConfigurations */
};  /* usbd_verify_dev_desc */

/* USB Standard Device Qualifier Descriptor */
static u8 usbd_verify_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] USB_DMA_ALIGNED = {
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
};  /* usbd_verify_device_qualifier_desc */

/* USB Verify Device Full Speed Configuration Descriptor */
u8 usbd_verify_cfg_desc[USB_LEN_CFG_DESC] = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,				/* bLength */
	USB_DESC_TYPE_CONFIGURATION,			/* bDescriptorType */
	USB_LOW_BYTE(0x00),	/* wTotalLength */
	USB_HIGH_BYTE(0x00),
	0x01,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
	CONFIG_ATTRIBUTE,                             	/* bmAttributes: self powered */
	0x32,                                           /* bMaxPower */
};

u8 usbd_verify_if_desc[USB_LEN_IF_DESC] = {
	/* Verify Interface, Alternate Setting 0 */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	0x00,                                           /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x00,                                           /* bNumEndpoints */
	USBD_VERIFY_CLASS_CODE,                         /* bInterfaceClass: Verify */
	USBD_VERIFY_SUBCLASS_CODE,                      /* bInterfaceSubClass */
	USBD_VERIFY_PROTOCOL,                 		/* bInterfaceProtocol */
	0,                   /* iInterface */
};

u8 usbd_verify_ep_desc[USB_LEN_EP_DESC]  = { //USB_DMA_ALIGNED
	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	0x00,			/* bEndpointAddress */
	0x00,				/* bmAttributes: BULK */
	USB_LOW_BYTE(0x00),		/* wMaxPacketSize: */
	USB_HIGH_BYTE(0x00),
	0x00,			/* bInterval */
};

/* USB Standard String Descriptor 0 */
static u8 usbd_verify_lang_id_desc[USB_LEN_LANGID_STR_DESC] USB_DMA_ALIGNED = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(USBD_VENDOR_LANGID_STRING),            /* wLANGID */
	USB_HIGH_BYTE(USBD_VENDOR_LANGID_STRING),
};  /* usbd_verify_lang_id_desc */

/* Vendor Class Driver */
usbd_class_driver_t usbd_verify_driver = {
	.get_descriptor = usbd_verify_get_descriptor,
	.set_config = usbd_verify_set_config,
	.clear_config = usbd_verify_clear_config,
	.setup = usbd_verify_setup,

	.ep0_data_in = usbd_verify_handle_ep0_data_in,
	.ep0_data_out = usbd_verify_handle_ep0_data_out,
	.ep_data_in = usbd_verify_handle_ep_data_in,
	.ep_data_out = usbd_verify_handle_ep_data_out,
};

/* Verify Device */
static usbd_verify_device_t usbd_verify_dev;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Set Vendor class configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_verify_set_config(usb_dev_t *dev, u8 config)
{
	int ret = HAL_OK;
	usbd_verify_device_t *cdev = &usbd_verify_dev;

	UNUSED(config);

	cdev->dev = dev;
	cdev->alt_setting = 0U;

	if (cdev->cb->set_config != NULL) {
		ret = cdev->cb->set_config(dev);
	}

	return ret;
}

/**
  * @brief  Clear Vendor configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_verify_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0U;
	usbd_verify_device_t *cdev = &usbd_verify_dev;

	UNUSED(config);

	if (cdev->cb->clear_config != NULL) {
		ret = cdev->cb->clear_config(dev);
	}

	return ret;
}

/**
  * @brief  Handle Vendor specific CTRL requests
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_verify_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	int ret = HAL_OK;
	usbd_verify_device_t *cdev = &usbd_verify_dev;

	//RTK_LOGD(TAG, "SETUP: bmRequestType=0x%02x bRequest=0x%02x wLength=0x%04x wValue=%x\n",
	//	 req->bmRequestType, req->bRequest, req->wLength, req->wValue);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->alt_setting = (u8)(req->wValue & 0xFF);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->ctrl_ep0.buf[0] = cdev->alt_setting;
				usbd_ep0_transmit(dev, cdev->ctrl_ep0.buf, 1U);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->ctrl_ep0.buf[0] = 0U;
				cdev->ctrl_ep0.buf[1] = 0U;
				usbd_ep0_transmit(dev, cdev->ctrl_ep0.buf, 2U);
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
			if (req->bmRequestType & 0x80U) { //in
				if (cdev->cb->setup != NULL) {
					cdev->cb->setup(req->bRequest, cdev->ctrl_ep0.buf, req->wLength, req->wValue);
				}
				usbd_ep0_transmit(dev, cdev->ctrl_ep0.buf, req->wLength);
			} else {  //out
				cdev->ctrl_req = req->bRequest;
				//cdev->ctrl_data_len = req->wLength;
				usbd_ep0_receive(dev, cdev->ctrl_ep0.buf, req->wLength);
			}
		} else {
			if (cdev->cb->setup != NULL) {
				cdev->cb->setup(req->bRequest, cdev->ctrl_ep0.buf, 0, req->wValue);
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
  * @brief  Get descriptor callback
  * @param  req: setup request
  * @param  speed: device speed
  * @param  len: descriptor length
  * @retval descriptor buffer
  */
static u8 *usbd_verify_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len)
{
	u8 *buf = NULL;
	u8 *desc = usbd_verify_dev.ctrl_ep0.buf;

	dev->self_powered = USBD_VENDOR_SELF_POWERED;

	switch ((req->wValue >> 8) & 0xFF) {

	case USB_DESC_TYPE_DEVICE:
		*len = sizeof(usbd_verify_dev_desc);
		buf = usbd_verify_dev_desc;
		usb_os_memcpy((void *)desc, (void *)buf, *len);
		buf = desc;
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (usbd_verify_dev.cb->get_config_desc != NULL) {
			buf = usbd_verify_dev.cb->get_config_desc(len);
		}
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		*len = sizeof(usbd_verify_device_qualifier_desc);
		buf = usbd_verify_device_qualifier_desc;
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (usbd_verify_dev.cb->get_config_desc != NULL) {
			buf = usbd_verify_dev.cb->get_config_desc(len);
			usb_os_memcpy((void *)desc, (void *)buf, *len);
			desc[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
			buf = desc;
		}
		break;

	case USB_DESC_TYPE_STRING:
		switch (req->wValue & 0xFF) {
		case USBD_IDX_LANGID_STR:
			buf = usbd_verify_lang_id_desc;
			*len = sizeof(usbd_verify_lang_id_desc);
			break;
		case USBD_IDX_MFC_STR:
			usbd_get_str_desc(USBD_VERIFY_MFG_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_PRODUCT_STR:
			if (speed == USB_SPEED_HIGH) {
				usbd_get_str_desc(USBD_VERIFY_HS_STRING, desc, len);
			} else {
				usbd_get_str_desc(USBD_VERIFY_FS_STRING, desc, len);
			}
			buf = desc;
			break;
		case USBD_IDX_SERIAL_STR:
			usbd_get_str_desc(USBD_VERIFY_SN_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_MS_OS_STR:
			/*Not support*/
			break;
		/* Add customer string here */
		default:
			RTK_LOGS(TAG, "[VRY] Invalid str idx %d\n", req->wValue & 0xFF);
			break;
		}
		break;

	default:
		break;
	}

	return buf;
}

static int usbd_verify_handle_ep0_data_in(usb_dev_t *dev, u8 status)
{
	int ret = HAL_OK;
	usbd_verify_device_t *cdev = &usbd_verify_dev;

	if (status != HAL_OK) {
		return status;
	}

	if (cdev->cb->ep0_data_in != NULL) {
		ret = cdev->cb->ep0_data_in(dev, status);
	}

	return ret;
}

static int usbd_verify_handle_ep0_data_out(usb_dev_t *dev)
{
	int ret = HAL_OK;
	usbd_verify_device_t *cdev = &usbd_verify_dev;

	if (cdev->cb->ep0_data_out != NULL) {
		ret = cdev->cb->ep0_data_out(dev);
	}

	return ret;
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_verify_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	int ret = HAL_OK;
	usbd_verify_device_t *cdev = &usbd_verify_dev;

	if (status != HAL_OK) {
		return status;
	}

	if (cdev->cb->ep_data_in != NULL) {
		ret = cdev->cb->ep_data_in(dev, ep_addr, status);
	}

	return ret;
}

/**
  * @brief  Data received on non-control Out endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
int usbd_verify_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len)
{
	int ret = HAL_OK;
	usbd_verify_device_t *cdev = &usbd_verify_dev;

	if (cdev->cb->ep_data_out != NULL) {
		ret = cdev->cb->ep_data_out(dev, ep_addr, len);
	}

	return ret;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize verify device
  * @retval Status
  */
int usbd_verify_init(usbd_verify_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_verify_device_t *cdev = &usbd_verify_dev;
	usb_os_memset(cdev, 0, sizeof(usbd_verify_device_t));

	if (HAL_OK != usbd_verify_ep_init(&(cdev->ctrl_ep0), USB_CH_EP_TYPE_CTRL, 0x00,
									  0, USBD_VERIFY_CTRL_BUF_SIZE, USBD_VERIFY_CTRL_BUF_SIZE, 0x00)) {
		return ret;
	}

	if (cb != NULL) {
		cdev->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				goto init_exit;
			}
		}
		usbd_register_class(&usbd_verify_driver);
	}

	return ret;

init_exit:
	usbd_verify_ep_deinit(&(cdev->ctrl_ep0));
	return ret;
}

/**
  * @brief  DeInitialize verify device
  * @param  void
  * @retval Status
  */
int usbd_verify_deinit(void)
{
	usbd_verify_device_t *cdev = &usbd_verify_dev;
	usbd_verify_ep_deinit(&(cdev->ctrl_ep0));
	if (cdev->cb != NULL) {
		if (cdev->cb->deinit != NULL) {
			cdev->cb->deinit();
		}
		cdev->cb = NULL;
	}

	usbd_unregister_class();

	return HAL_OK;
}

int usbd_verify_ep_enable(usb_dev_t *dev, usbd_verify_ep_t *ep)
{
	usbd_verify_ep_basic_t *ep_infor = &(ep->ep_infor);
	if (ep->ep_init == 0) {
		usbd_ep_init(dev, ep_infor->ep_addr, ep_infor->ep_type, ep_infor->mps);
		ep->ep_init = 1;
	} else {
		/*EP has inited*/
	}
	return HAL_OK;
}

int usbd_verify_ep_disable(usb_dev_t *dev, usbd_verify_ep_t *ep)
{
	usbd_verify_ep_basic_t *ep_infor = &(ep->ep_infor);
	if (ep->ep_init == 1) {
		usbd_ep_deinit(dev, ep_infor->ep_addr);
		ep->ep_init = 0;
	} else {
		/*EP has dis*/
	}
	return HAL_OK;
}

int usbd_verify_ep_init(usbd_verify_ep_t *ep, u8 type, u8 epaddr, u8 intervalue, u16 mps, u16 trans_size, u8 matchep)
{
	usbd_verify_ep_basic_t *ep_infor = &(ep->ep_infor);
	usb_os_memset(ep, 0x00, sizeof(usbd_verify_ep_t));

	ep->buf = (u8 *)usb_os_malloc(trans_size);
	if (ep->buf == NULL) {
		return HAL_ERR_MEM;
	}

	ep_infor->mps = mps;
	ep_infor->trans_len = trans_size;
	ep_infor->ep_addr = epaddr;
	ep_infor->ep_type = type;
	ep_infor->ep_interval = intervalue;
	ep_infor->match_addr = matchep;

	ep->state = VERIFY_TRANSFER_STATE_TX;

	//usbd_verify_dump_ep(ep);
	return HAL_OK;
}

int usbd_verify_ep_deinit(usbd_verify_ep_t *ep)
{
	if (ep && ep->buf != NULL) {
		usb_os_mfree(ep->buf);
		ep->buf = NULL;
	}

	return HAL_OK;
}

int usbd_verify_receive_data(usbd_verify_ep_t *ep)
{
	usbd_verify_device_t *cdev = &usbd_verify_dev;
	usbd_verify_ep_basic_t *ep_infor = &(ep->ep_infor);
	usbd_ep_receive(cdev->dev, ep_infor->ep_addr, ep->buf, ep_infor->trans_len);

	return HAL_OK;
}

/**
  * @brief  Transmit USB IN ZLP packet
  * @retval Status
  */
int usbd_verify_transmit_zlp(u8 addr)
{
	usbd_verify_device_t *cdev = &usbd_verify_dev;

	usbd_ep_transmit(cdev->dev, addr, NULL, 0);

	return HAL_OK;
}

int usbd_verify_transmit_ctrl_data(u8 *buf, u16 len)
{
	usbd_verify_device_t *cdev = &usbd_verify_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_verify_ep_t *ep = &(cdev->ctrl_ep0);
	u16 ep_mps = USBD_VERIFY_CTRL_BUF_SIZE;

	if (len > ep_mps) {
		len = ep_mps;
	}
	if (ep->state == VERIFY_TRANSFER_STATE_IDLE) {
		usb_os_memcpy(ep->buf, buf, len);
		usbd_ep0_transmit(dev, ep->buf, len);
	} else {
		ep->drop_count ++;
	}
	return HAL_OK;
}

int usbd_verify_transmit_data(usbd_verify_ep_t *ep)
{
	int ret = HAL_OK;
	usbd_verify_device_t *cdev = &usbd_verify_dev;
	usbd_verify_ep_basic_t *ep_infor = &(ep->ep_infor);

	if (((USB_CH_EP_TYPE_BULK == ep_infor->ep_type) || (USB_CH_EP_TYPE_INTR == ep_infor->ep_type)) && ((ep_infor->trans_len % ep_infor->mps) == 0) &&
		(ep_infor->trans_len > 0)) {
		ep->zlp = 1;
	} else {
		ep->zlp = 0;
	}
	if (ep->state == VERIFY_TRANSFER_STATE_IDLE) {
		ep->state = VERIFY_TRANSFER_STATE_TX;
		if (ep_infor->trans_len > 0) {
			ep->send_count ++;
		}
		//usbd_verify_dump_ep(ep);
		usbd_ep_transmit(cdev->dev, ep_infor->ep_addr, ep->buf, ep_infor->trans_len);
	} else {
		ep->drop_count ++;
		ret = HAL_BUSY;
	}

	return ret;
}

void usbd_verify_enable_dump_debug(u8 flag)
{
	usbd_verify_device_t *cdev = &usbd_verify_dev;

	cdev->enable_dump = flag;
}

char *usbd_verify_get_xfer_type_text(u8 ep_type)
{
	char *text = NULL;

	switch (ep_type) {
	case USB_CH_EP_TYPE_CTRL:
		text = (char *)TEXT_CTRL;
		break;
	case USB_CH_EP_TYPE_BULK:
		text = (char *)TEXT_BULK;
		break;
	case USB_CH_EP_TYPE_INTR:
		text = (char *)TEXT_INTR;
		break;
	case USB_CH_EP_TYPE_ISOC:
		text = (char *)TEXT_ISOC;
		break;
	default:
		break;
	}

	return text;
}

void usbd_verify_dump_ep(usbd_verify_ep_t *ep)
{
	usbd_verify_device_t *cdev = &usbd_verify_dev;
	usbd_verify_ep_basic_t *ep_infor;
	if (ep && cdev->enable_dump) {
		ep_infor = &(ep->ep_infor);
		RTK_LOGS(TAG, "[VRY] [type=%s-%s]EP%02x/%02x:mps(%d)/xferlen%d/state(%d)zlp(%d)\n",
				 usbd_verify_get_xfer_type_text(ep_infor->ep_type),
				 ((USB_EP_IS_IN(ep_infor->ep_addr)) ? ("IN") : ("OUT")),
				 ep_infor->ep_addr, ep_infor->match_addr,
				 ep_infor->mps, ep_infor->trans_len, ep->state, ep->zlp);
	}
}

void usbd_verify_dump_buf(u8 *type, u8 *buf, u32 length, u32 count)
{
	u32 i;
	usbd_verify_device_t *cdev = &usbd_verify_dev;
	u32 len = ((length <= DUMP_DATA_MAX_LEN) ? (length) : (DUMP_DATA_MAX_LEN));

	if (cdev->enable_dump) {
		RTK_LOGS(TAG, "[VRY] type %s cnt=%d[buf=%02x]\n", type, count, buf[0]);
		for (i = 0; i < len;) {
			if (i + 10 <= len) {
				RTK_LOGS(TAG, "[VRY] %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n", buf[i], buf[i + 1], buf[i + 2], buf[i + 3], buf[i + 4], buf[i + 5],
						 buf[i + 6], buf[i + 7], buf[i + 8],
						 buf[i + 9]);
				i += 10;
			} else {
				for (; i < len; i++) {
					RTK_LOGS(TAG, "%3d ", buf[i]);
				}
				RTK_LOGS(TAG, "\n");
				break;
			}
		}
	}
}

