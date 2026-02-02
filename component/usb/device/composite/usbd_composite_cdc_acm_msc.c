/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_composite_cdc_acm_msc.h"
#include "usbd_composite_cdc_acm.h"
#include "usbd_composite_msc.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_composite_set_config(usb_dev_t *dev, u8 config);
static int usbd_composite_clear_config(usb_dev_t *dev, u8 config);
static int usbd_composite_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_composite_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int usbd_composite_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_composite_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_composite_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static void usbd_composite_status_changed(usb_dev_t *dev, u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "COMP";

/* USB Standard Device Descriptor */
static const u8 usbd_composite_dev_desc[USB_LEN_DEV_DESC] = {
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
static const u8 usbd_composite_lang_id_desc[USB_LEN_LANGID_STR_DESC] = {
	USB_LEN_LANGID_STR_DESC,										/* bLength */
	USB_DESC_TYPE_STRING,											/* bDescriptorType */
	USB_LOW_BYTE(USBD_COMP_LANGID), USB_HIGH_BYTE(USBD_COMP_LANGID)	/* wLANGID */
};  /* usbd_composite_lang_id_desc */

/* USB Standard Device Qualifier Descriptor */
static const u8 usbd_composite_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
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
static const u8 usbd_composite_config_desc[USB_LEN_CFG_DESC] = {
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
static const usbd_class_driver_t usbd_composite_driver = {
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
	cdev->msc->set_config(dev, config);

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
	cdev->msc->clear_config(dev, config);

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
	usbd_ep_t *ep0_in = &dev->ep0_in;
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
			if (req->wIndex == USBD_COMP_MSC_ITF) {
				// MSC CLEAR_FEATURE
				ret = cdev->msc->setup(dev, req);
			}
			break;
		}
		break;
	case USB_REQ_TYPE_CLASS:
		if ((req->wIndex == USBD_COMP_CDC_COM_ITF) || (req->wIndex == USBD_COMP_CDC_DAT_ITF)) {
			ret = cdev->cdc->setup(dev, req);
		} else if (req->wIndex == USBD_COMP_MSC_ITF) {
			ret = cdev->msc->setup(dev, req);
		} else {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Invalid class req\n");
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
	} else if (ep_addr == USBD_COMP_MSC_BULK_IN_EP) {
		if (cdev->msc->ep_data_in != NULL) {
			ret = cdev->msc->ep_data_in(dev, ep_addr, status);
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
static int usbd_composite_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	int ret = HAL_OK;
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	if (ep_addr == USBD_COMP_CDC_BULK_OUT_EP) {
		if (cdev->cdc->ep_data_out != NULL) {
			ret = cdev->cdc->ep_data_out(dev, ep_addr, len);
		}
	} else if (ep_addr == USBD_COMP_MSC_BULK_OUT_EP) {
		if (cdev->msc->ep_data_out != NULL) {
			ret = cdev->msc->ep_data_out(dev, ep_addr, len);
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

	return ret;
}

/**
  * @brief  USB attach status change
  * @param  dev: USB device instance
  * @param  status: USB attach status
  * @retval void
  */
static void usbd_composite_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	UNUSED(dev);

	if (cdev->cb->status_changed) {
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
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	usb_speed_type_t speed = dev->dev_speed;
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
	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		usb_os_memcpy((void *)buf, (void *)usbd_composite_config_desc, USB_LEN_CFG_DESC);
		buf += USB_LEN_CFG_DESC;
		total_len += USB_LEN_CFG_DESC;
		desc_len = cdev->cdc->get_descriptor(dev, req, buf);
		buf += desc_len;
		total_len += desc_len;
		desc_len = cdev->msc->get_descriptor(dev, req, buf);
		total_len += desc_len;
		buf = dev->ep0_in.xfer_buf;
		if (USB_HIGH_BYTE(req->wValue) == USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION) {
			buf[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		}
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(total_len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(total_len);
		len = total_len;
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		len = sizeof(usbd_composite_device_qualifier_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_composite_device_qualifier_desc, len);
		break;

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
		case USBD_IDX_CDC_ITF_STR:
			if (speed == USB_SPEED_HIGH) {
				len = usbd_get_str_desc(USBD_COMP_CDC_HS_ITF_STRING, buf);
			} else {
				len = usbd_get_str_desc(USBD_COMP_CDC_FS_ITF_STRING, buf);
			}
			break;
		case USBD_IDX_MSC_ITF_STR:
			if (speed == USB_SPEED_HIGH) {
				len = usbd_get_str_desc(USBD_COMP_MSC_HS_ITF_STRING, buf);
			} else {
				len = usbd_get_str_desc(USBD_COMP_MSC_FS_ITF_STRING, buf);
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
  * @brief  Init CDC ACM class
  * @param  cb: CDC ACM user callback
  * @retval Status
  */
int usbd_composite_init(u32 cdc_bulk_out_xfer_size, u32 cdc_bulk_in_xfer_size, usbd_composite_cdc_acm_usr_cb_t *cdc_cb, usbd_composite_cb_t *cb)
{
	int ret;
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	if (cdc_cb == NULL) {
		ret = HAL_ERR_PARA;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user cb\n");
		return ret;
	}

	if (cb != NULL) {
		cdev->cb = cb;
	}

	ret = usbd_composite_cdc_acm_init(cdev, cdc_bulk_out_xfer_size, cdc_bulk_in_xfer_size, cdc_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init CDC ACM itf fail: %d\n", ret);
		return ret;
	}

	ret = usbd_composite_msc_init(cdev);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init MSC itf fail: %d\n", ret);
		usbd_composite_cdc_acm_deinit();
		return ret;
	}

	cdev->cdc = (usbd_class_driver_t *)&usbd_composite_cdc_acm_driver;
	cdev->msc = (usbd_class_driver_t *)&usbd_composite_msc_driver;

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
	usbd_unregister_class();

	usbd_composite_msc_deinit();
	usbd_composite_cdc_acm_deinit();
}

