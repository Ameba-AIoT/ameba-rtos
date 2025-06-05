/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_composite_hid_bi_dir.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/
/* wValue of HID report request */
#define COMP_HID_CMD_VOLUME_UP                              BIT0
#define COMP_HID_CMD_VOLUME_DOWN                            BIT1
#define COMP_HID_CMD_POWER                                  BIT2

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int composite_hid_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 composite_hid_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int composite_hid_sof(usb_dev_t *dev);
static int composite_hid_set_config(usb_dev_t *dev, u8 config);
static int composite_hid_clear_config(usb_dev_t *dev, u8 config);
static int composite_hid_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int composite_hid_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static int composite_hid_handle_ep0_data_out(usb_dev_t *dev);
static void composite_hid_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "COMP";

/* HID interface descriptor */
static const u8 usbd_composite_hid_itf_desc[] = {
	/* HID Interface Descriptor */
	0x09,                                        /*bLength*/
	USB_DESC_TYPE_INTERFACE,                     /*bDescriptorType*/
	USBD_COMP_HID_ITF,                           /*bInterfaceNumber*/
	0x00,                                        /*bAlternateSetting*/
	0x02,                                        /*bNumEndpoints*/
	0x03,                                        /*bInterfaceClass: HID*/
	0x00,                                        /*bInterfaceSubClass: 1=BOOT, 0=no boot*/
	0x00,                                        /*nInterfaceProtocol: 0=none, 1=keyboard, 2=mouse*/
	USBD_IDX_HID_ITF_STR,                        /*iInterface*/

	/* HID Descriptor */
	0x09,                                        /*bLength*/
	COMP_HID_DESC,                               /*bDescriptorType*/
	0x11,                                        /*bcdHID*/
	0x01,
	0x00,                                        /*bCountryCode*/
	0x01,                                        /*bNumDescriptors*/
	COMP_HID_REPORT_DESC,                        /*bDescriptorType*/
	0x00,                                        /*wItemLength, runtime assigned*/
	0x00,

	/* Endpoint Descriptor*/
	0x07,                                        /*bLength*/
	USB_DESC_TYPE_ENDPOINT,                      /*bDescriptorType:*/
	USBD_COMP_HID_INTR_IN_EP,                    /*bEndpointAddress*/
	0x03,                                        /*bmAttributes*/
	USB_LOW_BYTE(USBD_COMP_HID_MAX_BUF_SIZE),    /*wMaxPacketSize*/
	USB_HIGH_BYTE(USBD_COMP_HID_MAX_BUF_SIZE),
	0x01,                                        /*bInterval*/

	/* Endpoint Descriptor*/
	0x07,                                        /*bLength*/
	USB_DESC_TYPE_ENDPOINT,                      /*bDescriptorType:*/
	USBD_COMP_HID_INTR_OUT_EP,                   /*bEndpointAddress*/
	0x03,                                        /*bmAttributes*/
	USB_LOW_BYTE(USBD_COMP_HID_MAX_BUF_SIZE),    /*wMaxPacketSize*/
	USB_HIGH_BYTE(USBD_COMP_HID_MAX_BUF_SIZE),
	0x01,                                        /*bInterval*/


	/* HID Interface Descriptor */
	0x09,                                        /*bLength*/
	USB_DESC_TYPE_INTERFACE,                     /*bDescriptorType*/
	USBD_COMP_HID_VEND_ITF,                      /*bInterfaceNumber*/
	0x00,                                        /*bAlternateSetting*/
	0x01,                                        /*bNumEndpoints*/
	0x03,                                        /*bInterfaceClass: HID*/
	0x00,                                        /*bInterfaceSubClass: 1=BOOT, 0=no boot*/
	0x00,                                        /*nInterfaceProtocol: 0=none, 1=keyboard, 2=mouse*/
	USBD_IDX_HID_VEND_ITF_STR,                   /*iInterface*/

	/* HID Descriptor */
	0x09,                                        /*bLength*/
	COMP_HID_DESC,                               /*bDescriptorType*/
	0x11,                                        /*bcdHID*/
	0x01,
	0x00,                                        /*bCountryCode*/
	0x01,                                        /*bNumDescriptors*/
	COMP_HID_REPORT_DESC,                        /*bDescriptorType*/
	0x00,                                        /*wItemLength, runtime assigned*/
	0x00,

	/* Endpoint Descriptor*/
	0x07,                                        /*bLength*/
	USB_DESC_TYPE_ENDPOINT,                      /*bDescriptorType:*/
	USBD_COMP_HID_CONSUMER_INTR_IN_EP,           /*bEndpointAddress*/
	0x03,                                        /*bmAttributes*/
	USB_LOW_BYTE(USBD_COMP_HID_MAX_BUF_SIZE),    /*wMaxPacketSize*/
	USB_HIGH_BYTE(USBD_COMP_HID_MAX_BUF_SIZE),
	0x01,                                        /*bInterval*/
};

// HID descriptor
static const u8 usbd_composite_hid_desc[] = {
	0x09,                                        /*bLength: HID Descriptor size*/
	COMP_HID_DESC,                               /*bDescriptorType: HID*/
	0x11,                                        /*bcdHID: HID Class Spec release number Byte0*/
	0x01,                                        /*bcdHID: HID Class Spec release number Byte1*/
	0x00,                                        /*bCountryCode: Hardware target country*/
	0x01,                                        /*bNumDescriptors: Number of HID class descriptors to follow*/
	COMP_HID_REPORT_DESC,                        /*bDescriptorType*/
	0x00,                                        /*wItemLength: Total length of Report descriptor, runtime assigned*/
	0x00,
};

// HID report descriptor
static const u8 usbd_comp_hid_report_desc[] = {
	0x05, 0x0C,                                  /* Usage Page (Consumer Devices) */
	0x09, 0x01,                                  /* Usage (Consumer Control) */
	0xA1, 0x01,                                  /* COLLECTION (Application) */

	0x85, USBD_COMP_HID_REPORT_ID,               /* Report ID */
	0x09, 0x01,                                  /* Vendor Usage */
	0x15, 0x00,                                  /* Usage Minimum */
	0x25, 0xFF,                                  /* Usage Maximum */
	0x75, 0x08,                                  /* Report Size */
	0x95, USBD_COMP_HID_TX_PKT_SIZE,             /* Report Count */
	0x81, 0x02,                                  /* Input (Data,Var,Abs) */

	0x85, USBD_COMP_HID_REPORT_ID,               /* Report ID */
	0x09, 0x02,                                  /* Vendor Usage */
	0x15, 0x00,                                  /* Usage Minimum */
	0x25, 0xFF,                                  /* Usage Maximum */
	0x75, 0x08,                                  /* Report Size */
	0x95, USBD_COMP_HID_RX_PKT_SIZE,             /* Report Count */
	0x91, 0x02,                                  /* Output (Data,Var,Abs) */

	0xC0                                         /* end Application Collection */
};

// HID descriptor
static const u8 usbd_composite_hid_vend_desc[] = {
	0x09,                                        /*bLength: HID Descriptor size*/
	COMP_HID_DESC,                               /*bDescriptorType: HID*/
	0x11,                                        /*bcdHID: HID Class Spec release number Byte0*/
	0x01,                                        /*bcdHID: HID Class Spec release number Byte1*/
	0x00,                                        /*bCountryCode: Hardware target country*/
	0x01,                                        /*bNumDescriptors: Number of HID class descriptors to follow*/
	COMP_HID_REPORT_DESC,                        /*bDescriptorType*/
	0x00,                                        /*wItemLength: Total length of Report descriptor, runtime assigned*/
	0x00,
};

// HID report descriptor
static const u8 usbd_comp_hid_vend_report_desc[] = {
	0x05, 0x0C,                                  /* Usage Page (Consumer Devices) */
	0x09, 0x01,                                  /* Usage (Consumer Control) */
	0xA1, 0x01,                                  /* COLLECTION (Application) */

	0x85, USBD_COMP_HID_VOLUME_REPORT_ID,        /* Report ID */
	0x09, 0xE9,                                  /* Usage (Volume Up) */
	0x09, 0xEA,                                  /* Usage (Volume Down) */
	0x09, 0x30,                                  /* Usage (Power) */
	0x15, 0x00,                                  /* Logical Minimum (0) */
	0x25, 0x01,                                  /* Logical Maximum (1) */
	0x75, 0x01,                                  /* Report Size (1 bit) */
	0x95, 0x03,                                  /* Report Count (3) */
	0x81, 0x02,                                  /* Input (Data, Variable, Absolute) */
	0x75, 0x01,                                  /* Report Size (1 bits) */
	0x95, 0x05,                                  /* Report Count (5) */
	0x81, 0x03,                                  /* Input (Constant, Variable, Absolute) - Padding for alignment */

	0xC0                                         /* end Application Collection */
};

static usbd_composite_hid_device_t composite_hid_device;

/* Exported variables --------------------------------------------------------*/

const usbd_class_driver_t usbd_composite_hid_driver = {
	.get_descriptor = composite_hid_get_descriptor,
	.set_config = composite_hid_set_config,
	.clear_config = composite_hid_clear_config,
	.setup = composite_hid_setup,
	.sof = composite_hid_sof,
	.ep0_data_out = composite_hid_handle_ep0_data_out,
	.ep_data_in = composite_hid_handle_ep_data_in,
	.ep_data_out = composite_hid_handle_ep_data_out,
	.status_changed = composite_hid_status_changed,
};

/* Private functions ---------------------------------------------------------*/
static int usbd_composite_hid_send_data_internal(u8 *data, u16 len)
{
	u16 valid_len;
	u8 offset = 0;
	int ret = HAL_ERR_HW;
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usb_dev_t *dev = hid->cdev->dev;
	usbd_ep_t *ep_hid_in = &(hid->ep_hid_in);

	if (!dev->is_ready) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x TX %d not ready\n", USBD_COMP_HID_CONSUMER_INTR_IN_EP, len);
		return ret;
	}

	valid_len = ep_hid_in->xfer_buf_len;

	if (len > valid_len) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Data len %d > max %d\n",  len, valid_len);
		len = valid_len;
	}

	if (!ep_hid_in->xfer_state) {
		if (dev->is_ready) { // In case deinit when plug out
			ep_hid_in->is_busy = 1U;
			ep_hid_in->xfer_state = 1U;

			usb_os_memcpy((void *) & (ep_hid_in->xfer_buf[offset]), (void *)data, len); //dma cache
			valid_len = len;

			if (dev->is_ready) { // In case deinit when plug out
				ep_hid_in->xfer_len = valid_len;
				usbd_ep_transmit(hid->cdev->dev, ep_hid_in);
				ret = HAL_OK;
			} else {
				ep_hid_in->xfer_state = 0U;
			}
			ep_hid_in->is_busy = 0U;
		} else {
			/*TX not ready*/
		}
	} else {
		/*TX busy*/
		ret = HAL_BUSY;
	}

	return ret;
}

static int usbd_composite_hid_priv_send_data_internal(u8 *data, u16 len, u8 reportid, u8 with_reportid)
{
	u16 valid_len;
	u8 offset = 0;
	int ret = HAL_ERR_HW;
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_ep_t *ep_hid_priv_in = &(hid->ep_hid_priv_in);
	usb_dev_t *dev = hid->cdev->dev;

	if (!dev->is_ready) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x TX %d not ready\n", USBD_COMP_HID_INTR_IN_EP, len);
		return ret;
	}

	if (hid->report_id_append && (with_reportid == 0)) {
		valid_len = ep_hid_priv_in->xfer_buf_len - 1;
	} else {
		valid_len = ep_hid_priv_in->xfer_buf_len;
	}

	if (len > valid_len) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Data len %d > max %d\n",  len, valid_len);
		len = valid_len;
	}

	if (!ep_hid_priv_in->xfer_state) {
		if (dev->is_ready) { // In case deinit when plug out
			ep_hid_priv_in->is_busy = 1U;
			ep_hid_priv_in->xfer_state = 1U;

			if (hid->report_id_append && (with_reportid == 0)) {
				ep_hid_priv_in->xfer_buf[0] = reportid;
				offset = 1;
			}

			usb_os_memcpy((void *) & (ep_hid_priv_in->xfer_buf[offset]), (void *)data, len); //dma cache
			if (hid->report_id_append && (with_reportid == 0)) {
				if (len < USBD_COMP_HID_TX_PKT_SIZE) {
					/* set padding to zero */
					usb_os_memset((void *) & (ep_hid_priv_in->xfer_buf[offset + len]), 0x00, USBD_COMP_HID_TX_PKT_SIZE - len);
				}
				valid_len = USBD_COMP_HID_TX_PKT_SIZE + offset;
			} else {
				valid_len = len;
			}

			if (dev->is_ready) { // In case deinit when plug out
				ep_hid_priv_in->xfer_len = valid_len;
				usbd_ep_transmit(hid->cdev->dev, ep_hid_priv_in);
				ret = HAL_OK;
			} else {
				ep_hid_priv_in->xfer_state = 0U;
			}
			ep_hid_priv_in->is_busy = 0U;
		} else {
			/*TX not ready*/
		}
	} else {
		/*TX busy*/
		ret = HAL_BUSY;
	}

	return ret;
}

static int usbd_hid_system_control(u8 cmd)
{
	u8 hid_report_buf[2];

	hid_report_buf[0] = USBD_COMP_HID_VOLUME_REPORT_ID;
	hid_report_buf[1] = cmd;
	usbd_composite_hid_send_data_internal(hid_report_buf, 2);

	rtos_time_delay_ms(40);

	// Reset to stop the ongoing effect.
	hid_report_buf[1] = 0x00;
	usbd_composite_hid_send_data_internal(hid_report_buf, 2);

	return HAL_OK;
}

static int usbd_hid_receive(void)
{
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_composite_dev_t *cdev = hid->cdev;
	usbd_composite_hid_buf_t *p_buf = NULL;
	usbd_ep_t *ep_intr_out = &(hid->ep_intr_out);
	usbd_composite_hid_buf_ctrl_t *pbuf_ctrl = &(hid->hid_priv_out);

	p_buf = &(pbuf_ctrl->buf_array[pbuf_ctrl->write_idx]);

	ep_intr_out->xfer_buf = p_buf->buf_raw;
	ep_intr_out->xfer_len = pbuf_ctrl->hid_mps;
	usbd_ep_receive(cdev->dev, ep_intr_out);

	return HAL_OK;
}

static int composite_hid_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	int ret = HAL_OK;
	u16 len = 0;
	u16 report_len;
	u8 entityId;
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_composite_dev_t *cdev = hid->cdev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	u8 *buf = ep0_in->xfer_buf;

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

		case USB_REQ_GET_DESCRIPTOR:
			entityId = USB_LOW_BYTE(req->wIndex);
			if (entityId == USBD_COMP_HID_ITF) {
				if (USB_HIGH_BYTE(req->wValue) == COMP_HID_REPORT_DESC) {
					report_len = sizeof(usbd_comp_hid_report_desc);
					usb_os_memcpy((void *)buf, (void *)usbd_comp_hid_report_desc, report_len);
					ep0_in->xfer_len = MIN(report_len, req->wLength);
					usbd_ep_transmit(dev, ep0_in);
				} else if (USB_HIGH_BYTE(req->wValue) == COMP_HID_DESC) {
					len = COMP_HID_DESC_SIZE;
					report_len = sizeof(usbd_comp_hid_report_desc);
					usb_os_memcpy((void *)buf, (void *)usbd_composite_hid_desc, len);
					buf[COMP_HID_DESC_ITEM_LENGTH_OFFSET] = USB_LOW_BYTE(report_len);
					buf[COMP_HID_DESC_ITEM_LENGTH_OFFSET + 1] = USB_HIGH_BYTE(report_len);
					ep0_in->xfer_len = MIN(len, req->wLength);
					usbd_ep_transmit(dev, ep0_in);
				} else {
					ret = HAL_ERR_PARA;
				}
			} else if (entityId == USBD_COMP_HID_VEND_ITF) {
				if (USB_HIGH_BYTE(req->wValue) == COMP_HID_REPORT_DESC) {
					report_len = sizeof(usbd_comp_hid_vend_report_desc);
					usb_os_memcpy((void *)buf, (void *)usbd_comp_hid_vend_report_desc, report_len);
					ep0_in->xfer_len = MIN(report_len, req->wLength);
					usbd_ep_transmit(dev, ep0_in);
				} else if (USB_HIGH_BYTE(req->wValue) == COMP_HID_DESC) {
					len = COMP_HID_DESC_SIZE;
					report_len = sizeof(usbd_comp_hid_vend_report_desc);
					usb_os_memcpy((void *)buf, (void *)usbd_composite_hid_vend_desc, len);
					buf[COMP_HID_DESC_ITEM_LENGTH_OFFSET] = USB_LOW_BYTE(report_len);
					buf[COMP_HID_DESC_ITEM_LENGTH_OFFSET + 1] = USB_HIGH_BYTE(report_len);
					ep0_in->xfer_len = MIN(len, req->wLength);
					usbd_ep_transmit(dev, ep0_in);
				} else {
					ret = HAL_ERR_PARA;
				}
			} else {
				ret = HAL_ERR_PARA;
			}
			break;
		default:
			RTK_LOGS(TAG, RTK_LOG_WARN, "Invalid bRequest 0x%02x\n", req->bRequest);
			ret = HAL_ERR_PARA;
			break;
		}
		break;
	case USB_REQ_TYPE_CLASS:
		if (req->wLength) {
			if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
				ret = hid->cb->setup(req, ep0_in->xfer_buf);
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
			hid->cb->setup(req, NULL);
		}
		break;
	default:
		RTK_LOGS(TAG, RTK_LOG_WARN, "Invalid bmRequestType 0x%02x\n", req->bmRequestType);
		ret = HAL_ERR_PARA;
		break;
	}

	return ret;
}

static int composite_hid_set_config(usb_dev_t *dev, u8 config)
{
	UNUSED(config);

	int ret = HAL_OK;
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_ep_t *ep_hid_priv_in = &(hid->ep_hid_priv_in);
	usbd_ep_t *ep_hid_in = &(hid->ep_hid_in);
	usbd_ep_t *ep_intr_out = &(hid->ep_intr_out);

	ep_hid_priv_in->xfer_state = 0U;
	ep_hid_in->xfer_state = 0U;

	/* Init INTR IN EP */
	ep_hid_in->mps = USBD_COMP_HID_MAX_BUF_SIZE;
	usbd_ep_init(dev, ep_hid_in);

	/* Init Priv Hid command EP */
	/* Init INTR IN EP */
	ep_hid_priv_in->mps = USBD_COMP_HID_MAX_BUF_SIZE;
	usbd_ep_init(dev, ep_hid_priv_in);

	/* Init INTR OUT EP */
	ep_intr_out->mps = USBD_COMP_HID_MAX_BUF_SIZE;
	usbd_ep_init(dev, ep_intr_out);
	/* Prepare to receive next INTR OUT packet */
	usbd_hid_receive();

	if (hid->cb->set_config != NULL) {
		hid->cb->set_config();
	}

	return ret;
}

static int composite_hid_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0;
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_ep_t *ep_hid_priv_in = &(hid->ep_hid_priv_in);
	usbd_ep_t *ep_hid_in = &(hid->ep_hid_in);
	usbd_ep_t *ep_intr_out = &(hid->ep_intr_out);

	UNUSED(config);

	/* DeInit INTR IN EP */
	usbd_ep_deinit(dev, ep_hid_priv_in);

	/* DeInit INTR OUT EP */
	usbd_ep_deinit(dev, ep_intr_out);

	/* DeInit INTR IN EP */
	usbd_ep_deinit(dev, ep_hid_in);

	return ret;
}

static int composite_hid_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_ep_t *ep_hid_priv_in = &(hid->ep_hid_priv_in);
	usbd_ep_t *ep_hid_in = &(hid->ep_hid_in);

	UNUSED(dev);

	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x TX err: %d\n", ep_addr, status);
	} else {
		if (ep_addr == USBD_COMP_HID_INTR_IN_EP) { // priv hid cmmand
			ep_hid_priv_in->xfer_state = 0U;
			if (hid->cb->transmitted) {
				hid->cb->transmitted(status);
			}
		} else { //default hid cmmand
			ep_hid_in->xfer_state = 0U;
		}
	}

	return HAL_OK;
}

static int composite_hid_sof(usb_dev_t *dev)
{
	UNUSED(dev);
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_composite_hid_buf_ctrl_t *pbuf_ctrl = &(hid->hid_priv_out);

	/*
	    the ring buf is not full and the hid rx not busy, means should trigger to hid rx
	*/
	if ((!pbuf_ctrl->is_intr_out_busy) && (!usbd_composite_hid_ring_buf_is_full())) {
		pbuf_ctrl->write_idx = (pbuf_ctrl->write_idx + 1) % (pbuf_ctrl->buf_array_cnt);
		pbuf_ctrl->is_intr_out_busy = 1;
		usbd_hid_receive();
	}

	return HAL_OK;
}

static int composite_hid_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len)
{
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_composite_hid_buf_ctrl_t *pbuf_ctrl;

	UNUSED(dev);

	if ((ep_addr == USBD_COMP_HID_INTR_OUT_EP) && (len > 0)) {
		/*
		    if the ring buf is full, should wait,and will trigger the rx in next sof
		*/
		pbuf_ctrl = &(hid->hid_priv_out);
		pbuf_ctrl->buf_array[pbuf_ctrl->write_idx].buf_valid_len = len;

		if (!usbd_composite_hid_ring_buf_is_full()) {
			pbuf_ctrl->write_idx = (pbuf_ctrl->write_idx + 1) % (pbuf_ctrl->buf_array_cnt);
			pbuf_ctrl->is_intr_out_busy = 1;
			usbd_hid_receive();
		} else {
			pbuf_ctrl->is_intr_out_busy = 0;
		}

		//used for usb read
		rtos_sema_give(pbuf_ctrl->uac_hid_sema);
	}

	return HAL_OK;
}

/**
  * @brief  Handle EP0 Rx Ready event
  * @param  dev: USB device instance
  * @retval Status
  */
static int composite_hid_handle_ep0_data_out(usb_dev_t *dev)
{
	int ret = HAL_ERR_HW;
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_composite_dev_t *cdev = hid->cdev;

	if (cdev->ctrl_req.bRequest != 0xFFU) {
		hid->cb->setup(&cdev->ctrl_req, dev->ep0_in.xfer_buf);
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
static u16 composite_hid_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	u16 len = 0;
	u16 report_len;

	UNUSED(dev);

	switch (USB_HIGH_BYTE(req->wValue)) {

	case USB_DESC_TYPE_CONFIGURATION:
	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		len = sizeof(usbd_composite_hid_itf_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_composite_hid_itf_desc, len);
		report_len = sizeof(usbd_comp_hid_report_desc);
		buf[COMP_HID_ITF_DESC_ITEM_LENGTH_OFFSET] = USB_LOW_BYTE(report_len);
		buf[COMP_HID_ITF_DESC_ITEM_LENGTH_OFFSET + 1] = USB_HIGH_BYTE(report_len);
		report_len = sizeof(usbd_comp_hid_vend_report_desc);
		buf[COMP_HID_ITF_VEND_DESC_ITEM_LENGTH_OFFSET] = USB_LOW_BYTE(report_len);
		buf[COMP_HID_ITF_VEND_DESC_ITEM_LENGTH_OFFSET + 1] = USB_HIGH_BYTE(report_len);
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
static void composite_hid_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	UNUSED(dev);
	UNUSED(old_status);
	UNUSED(status);
}

/**
  * @brief  Deinitialize HID EP buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @retval void
  */
static void usbd_composite_hid_ringbuf_ctrl_deinit(void)
{
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_composite_hid_buf_ctrl_t *buf_ctrl = &(hid->hid_priv_out);

	buf_ctrl->read_idx = 0;
	buf_ctrl->write_idx = 0;
	buf_ctrl->hid_mps = 0;
	buf_ctrl->buf_array_cnt = 0;

	// RTK_LOGS(TAG, RTK_LOG_DEBUG, "Buf 0x%08x-0x%08x sema %d\n",buf_ctrl->isoc_buf,buf_ctrl->buf_array,buf_ctrl->uac_sema_valid);

	if (buf_ctrl->hid_buf != NULL) {
		usb_os_mfree(buf_ctrl->hid_buf);
		buf_ctrl->hid_buf = NULL;
	}

	if (buf_ctrl->buf_array != NULL) {
		usb_os_mfree(buf_ctrl->buf_array);
		buf_ctrl->buf_array = NULL;
	}

	if (buf_ctrl->hid_sema_valid) {
		buf_ctrl->hid_sema_valid = 0;

		do {
			rtos_sema_give(buf_ctrl->uac_hid_sema);
			usb_os_delay_us(100U);
		} while (buf_ctrl->read_wait_sema);

		rtos_sema_delete(buf_ctrl->uac_hid_sema);
	}
}

/**
  * @brief  Initialize HID EP buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @param  params: Pointer to audio configuration parameters structure
  * @param  speed: USB connection speed
  * @retval Status
  */
static int usbd_composite_hid_ring_buf_ctrl_init(void)
{
	u8 idx;
	usbd_composite_hid_buf_t *pdata = NULL;
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_composite_hid_buf_ctrl_t *buf_ctrl = &(hid->hid_priv_out);

	buf_ctrl->hid_mps = USBD_COMP_HID_MAX_BUF_SIZE;
	if (buf_ctrl->hid_mps == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS check fail\n");
		return HAL_ERR_PARA;
	}

	buf_ctrl->buf_array_cnt = COMP_HID_BUF_MAX_CNT;
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Buf mps len %d, cnt %d\n", buf_ctrl->hid_mps, buf_ctrl->buf_array_cnt);

	buf_ctrl->hid_buf = (u8 *)usb_os_malloc(CACHE_LINE_ALIGMENT(buf_ctrl->hid_mps) * buf_ctrl->buf_array_cnt);
	if (buf_ctrl->hid_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Can not get hid buf mem\n");
		return HAL_ERR_MEM;
	}

	buf_ctrl->buf_array = (usbd_composite_hid_buf_t *)usb_os_malloc(sizeof(usbd_composite_hid_buf_t) * buf_ctrl->buf_array_cnt);
	if (buf_ctrl->buf_array == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Can not get hid buf array mem\n");
		return HAL_ERR_MEM;
	}
	for (idx = 0; idx < buf_ctrl->buf_array_cnt; idx ++) {
		pdata = &(buf_ctrl->buf_array[idx]);
		pdata->buf_valid_len = 0;
		pdata->buf_raw = buf_ctrl->hid_buf + CACHE_LINE_ALIGMENT(buf_ctrl->hid_mps) * idx ;

		// RTK_LOGS(TAG, RTK_LOG_DEBUG, "Buf %d-%d-%d-0x%08x\n",idx,buf_ctrl->hid_mps,pdata->buf_valid_len,pdata->buf_raw);
	}

	rtos_sema_create(&(buf_ctrl->uac_hid_sema), 0U, 1U);
	buf_ctrl->hid_sema_valid = 1;

	buf_ctrl->read_wait_sema = 0;
	buf_ctrl->read_idx = 0;
	buf_ctrl->write_idx = 0;

	return HAL_OK;
}

/* Exported functions --------------------------------------------------------*/

int usbd_composite_hid_init(usbd_composite_dev_t *cdev, usbd_composite_hid_usr_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_ep_t *ep_hid_priv_in = &(hid->ep_hid_priv_in);
	usbd_ep_t *ep_hid_in = &(hid->ep_hid_in);
	usbd_ep_t *ep_intr_out = &(hid->ep_intr_out);

	if (cb != NULL) {
		hid->cb = cb;
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "HID cb is NULL\n");
		return HAL_ERR_PARA;
	}

	if (usbd_composite_hid_ring_buf_ctrl_init() != HAL_OK) { //init fail
		ret = HAL_ERR_MEM;
		goto usbd_hid_init_exit;
	}

	ep_hid_priv_in->addr = USBD_COMP_HID_INTR_IN_EP;
	ep_hid_priv_in->type = USB_CH_EP_TYPE_INTR;
	ep_hid_priv_in->xfer_buf_len = USBD_COMP_HID_MAX_BUF_SIZE;
	ep_hid_priv_in->xfer_buf = (u8 *)usb_os_malloc(ep_hid_priv_in->xfer_buf_len);
	if (ep_hid_priv_in->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto usbd_composite_hid_priv_out_buf_exit;
	}

	ep_hid_in->addr = USBD_COMP_HID_CONSUMER_INTR_IN_EP;
	ep_hid_in->type = USB_CH_EP_TYPE_INTR;
	ep_hid_in->xfer_buf_len = USBD_COMP_HID_MAX_BUF_SIZE;
	ep_hid_in->xfer_buf = (u8 *)usb_os_malloc(ep_hid_in->xfer_buf_len);
	if (ep_hid_in->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto usbd_composite_hid_clean_priv_exit;
	}

	ep_intr_out->addr = USBD_COMP_HID_INTR_OUT_EP;
	ep_intr_out->type = USB_CH_EP_TYPE_INTR;

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			goto usbd_composite_hid_clean_all_exit;
		}
	}

	hid->cdev = cdev;

	return ret;

usbd_composite_hid_clean_all_exit:
	if (ep_hid_in->xfer_buf) {
		usb_os_mfree(ep_hid_priv_in->xfer_buf);
		ep_hid_priv_in->xfer_buf = NULL;
	}

usbd_composite_hid_clean_priv_exit:
	if (ep_hid_priv_in->xfer_buf) {
		usb_os_mfree(ep_hid_priv_in->xfer_buf);
		ep_hid_priv_in->xfer_buf = NULL;
	}

usbd_composite_hid_priv_out_buf_exit:
	usbd_composite_hid_ringbuf_ctrl_deinit();

usbd_hid_init_exit:
	return ret;
}

int usbd_composite_hid_deinit(void)
{
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_ep_t *ep_hid_priv_in = &(hid->ep_hid_priv_in);
	usbd_ep_t *ep_hid_in = &(hid->ep_hid_in);

	while (ep_hid_priv_in->is_busy || ep_hid_in->is_busy) {
		usb_os_delay_us(100);
	}

	if ((hid->cb != NULL) && (hid->cb->deinit != NULL)) {
		hid->cb->deinit();
	}

	if (ep_hid_priv_in->xfer_buf) {
		usb_os_mfree(ep_hid_priv_in->xfer_buf);
		ep_hid_priv_in->xfer_buf = NULL;
	}

	if (ep_hid_in->xfer_buf) {
		usb_os_mfree(ep_hid_in->xfer_buf);
		ep_hid_in->xfer_buf = NULL;
	}

	usbd_composite_hid_ringbuf_ctrl_deinit();

	return HAL_OK;
}

int usbd_composite_hid_send_data(u8 *data, u16 len)
{
	return usbd_composite_hid_priv_send_data_internal(data, len, USBD_COMP_HID_REPORT_ID, 0);
}

int usbd_composite_hid_volume_ctrl(u8 vol_up)
{
	u8 hid_cmd = COMP_HID_CMD_VOLUME_DOWN;
	if (vol_up) {
		hid_cmd = COMP_HID_CMD_VOLUME_UP;
	}

	return usbd_hid_system_control(hid_cmd);
}

int usbd_composite_hid_power_ctrl(void)
{
	return usbd_hid_system_control(COMP_HID_CMD_POWER);
}

/**
  * @brief  Read data from a USB HID ring buffer
  * @param  pdata_ctrl: Pointer to the buffer control structure
  * @param  buffer: Pointer to Data buffer
  * @param  size: Data length
  * @param  copy_len: Copy length
  * @retval Status
  */
static u32 usbd_composite_hid_read_ring_buf(usbd_composite_hid_buf_ctrl_t *pdata_ctrl, u8 *buffer, u32 size, u32 *copy_len)
{
	usbd_composite_hid_buf_t *p_buf = NULL;
	u8 rd_idx = pdata_ctrl->read_idx % (pdata_ctrl->buf_array_cnt);
	usbd_composite_hid_device_t *hid = &composite_hid_device;

	do {
		p_buf = &(pdata_ctrl->buf_array[rd_idx]);

		/* should exit : 1) Enough data has been obtained; 2) the next data cannot be saved completely */
		if ((*copy_len >= size) || (*copy_len + p_buf->buf_valid_len > size)) {
			pdata_ctrl->read_idx = rd_idx;
			return 0;
		}

		if (p_buf->buf_valid_len > 0) {
			if (p_buf->buf_raw[0] == USBD_COMP_HID_REPORT_ID) {
				hid->report_id_append = 1;
				usb_os_memcpy(buffer + *copy_len, p_buf->buf_raw + 1, p_buf->buf_valid_len - 1);
				*copy_len += p_buf->buf_valid_len - 1;
			} else {
				hid->report_id_append = 0;
				usb_os_memcpy(buffer + *copy_len, p_buf->buf_raw, p_buf->buf_valid_len);
				*copy_len += p_buf->buf_valid_len;
			}

		}

		rd_idx = (rd_idx + 1) % (pdata_ctrl->buf_array_cnt);

	} while (rd_idx != pdata_ctrl->write_idx);

	pdata_ctrl->read_idx = rd_idx;
	return 1;
}

/**
  * @brief  Read HID data
  * @param  buf: Pointer to data buffer
  * @param  size: Data length
  * @param  time_out_ms: Time out
  * @retval copy len
  */
u32 usbd_composite_hid_read(u8 *buffer, u32 size, u32 time_out_ms)
{
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usb_dev_t *dev = hid->cdev->dev;
	usbd_composite_hid_buf_ctrl_t *pdata_ctrl = &(hid->hid_priv_out);
	u32 copy_len = 0;

	if (dev->is_ready == 0) {
		return 0;
	}

	if (time_out_ms == 0) {
		if (pdata_ctrl->read_idx == pdata_ctrl->write_idx) { //no valid data
			return 0;
		}

		usbd_composite_hid_read_ring_buf(pdata_ctrl, buffer, size, &copy_len);
	} else {
		do {
			if (pdata_ctrl->read_idx == pdata_ctrl->write_idx) {
				//wait sema
				pdata_ctrl->read_wait_sema = 1;
				if (rtos_sema_take(pdata_ctrl->uac_hid_sema, time_out_ms) != RTK_SUCCESS) {
					pdata_ctrl->read_wait_sema = 0;
					break;
				}

				pdata_ctrl->read_wait_sema = 0;
			} else {
				/* if did not read any pkt, loop to check the wr/rd pos*/
				if (usbd_composite_hid_read_ring_buf(pdata_ctrl, buffer, size, &copy_len) == 0) {
					break;
				}
			}
		} while (dev->is_ready);
	}

	if (copy_len > size) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Pls check the hid cfg\n");
	}

	return copy_len;
}

/**
  * @brief  Get HID read buffer cnt
  * @param  void
  * @retval read buffer cnt
  */
u32 usbd_composite_hid_get_read_buf_cnt(void)
{
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_composite_hid_buf_ctrl_t *pdata_ctrl = &(hid->hid_priv_out);

	if (pdata_ctrl == NULL) {
		return 0;
	}

	return (pdata_ctrl->write_idx + pdata_ctrl->buf_array_cnt - pdata_ctrl->read_idx) % (pdata_ctrl->buf_array_cnt);
}

/**
  * @brief  check whether the ring buf is full
  * @param  void
  * @retval 1 if the ring buf is full, else 0
  */
u32 usbd_composite_hid_ring_buf_is_full(void)
{
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_composite_hid_buf_ctrl_t *pdata_ctrl = &(hid->hid_priv_out);

	if (pdata_ctrl == NULL) {
		return 0;
	}

	return (pdata_ctrl->write_idx + 1) % (pdata_ctrl->buf_array_cnt) ==  pdata_ctrl->read_idx;
}
