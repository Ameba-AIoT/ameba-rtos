/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_composite_hid.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int composite_hid_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 composite_hid_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int composite_hid_set_config(usb_dev_t *dev, u8 config);
static int composite_hid_clear_config(usb_dev_t *dev, u8 config);
static int composite_hid_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int composite_hid_handle_ep0_data_out(usb_dev_t *dev);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "COMP";

/* HID interface descriptor */
static const u8 usbd_composite_hid_itf_desc[] = {
	/* HID Interface Descriptor */
	0x09,       						/*bLength*/
	USB_DESC_TYPE_INTERFACE,			/*bDescriptorType*/
	USBD_COMP_HID_ITF,					/*bInterfaceNumber*/
	0x00,								/*bAlternateSetting*/
	0x01,								/*bNumEndpoints*/
	0x03,								/*bInterfaceClass: HID*/
	0x01,								/*bInterfaceSubClass: 1=BOOT, 0=no boot*/
	0x02,								/*nInterfaceProtocol: 0=none, 1=keyboard, 2=mouse*/
	USBD_IDX_HID_ITF_STR,				/*iInterface*/

	/* HID Descriptor */
	0x09,								/*bLength*/
	COMP_HID_DESC, 						/*bDescriptorType*/
	0x11,								/*bcdHID*/
	0x01,
	0x00,								/*bCountryCode*/
	0x01,								/*bNumDescriptors*/
	COMP_HID_REPORT_DESC,				/*bDescriptorType*/
	0x00,								/*wItemLength, runtime assigned*/
	0x00,

	/* Endpoint Descriptor*/
	0x07,								/*bLength*/
	USB_DESC_TYPE_ENDPOINT,				/*bDescriptorType:*/
	USBD_COMP_HID_INTR_IN_EP,			/*bEndpointAddress*/
	0x03,								/*bmAttributes*/
	USB_LOW_BYTE(COMP_HID_INTR_IN_PACKET_SIZE), USB_HIGH_BYTE(COMP_HID_INTR_IN_PACKET_SIZE),	/*wMaxPacketSize*/
	0x0A,								/*bInterval*/
};

// HID descriptor
static const u8 usbd_composite_hid_desc[COMP_HID_DESC_SIZE] = {
	COMP_HID_DESC_SIZE,					/*bLength: HID Descriptor size*/
	COMP_HID_DESC,						/*bDescriptorType: HID*/
	0x11,								/*bcdHID: HID Class Spec release number Byte0*/
	0x01,								/*bcdHID: HID Class Spec release number Byte1*/
	0x00,								/*bCountryCode: Hardware target country*/
	0x01,								/*bNumDescriptors: Number of HID class descriptors to follow*/
	COMP_HID_REPORT_DESC,				/*bDescriptorType*/
	0x00,		    					/*wItemLength: Total length of Report descriptor, runtime assigned */
	0x00,
};

// HID mouse report descriptor
static const u8 composite_hid_mouse_report_desc[] = {
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x09, 0x02,                    // USAGE (Mouse)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x09, 0x01,                    //   USAGE (Pointer)
	0xa1, 0x00,                    //   COLLECTION (Physical)
	0x05, 0x09,                    //     USAGE_PAGE (Button)
	0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
	0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
	0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
	0x95, 0x03,                    //     REPORT_COUNT (3)
	0x75, 0x01,                    //     REPORT_SIZE (1)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x95, 0x01,                    //     REPORT_COUNT (1)
	0x75, 0x05,                    //     REPORT_SIZE (5)
	0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
	0x09, 0x30,                    //     USAGE (X)
	0x09, 0x31,                    //     USAGE (Y)
	0x09, 0x38,                    //     USAGE (Wheel)
	0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
	0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
	0x75, 0x08,                    //     REPORT_SIZE (8)
	0x95, 0x03,                    //     REPORT_COUNT (3)
	0x81, 0x06,                    //     INPUT (Data,Var,Rel)
	0xc0,                          //   END_COLLECTION
	0xc0                           // END_COLLECTION
};

static usbd_composite_hid_device_t composite_hid_device;

/* Exported variables --------------------------------------------------------*/

const usbd_class_driver_t usbd_composite_hid_driver = {
	.get_descriptor = composite_hid_get_descriptor,
	.set_config = composite_hid_set_config,
	.clear_config = composite_hid_clear_config,
	.setup = composite_hid_setup,
	.ep0_data_out = composite_hid_handle_ep0_data_out,
	.ep_data_in = composite_hid_handle_ep_data_in,
};

/* Private functions ---------------------------------------------------------*/

static int composite_hid_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	int ret = HAL_OK;
	u16 len = 0;
	u16 report_len = sizeof(composite_hid_mouse_report_desc);
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_composite_dev_t *cdev = hid->cdev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	u8 *buf = ep0_in->xfer_buf;

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_GET_DESCRIPTOR:
			if (USB_HIGH_BYTE(req->wValue) == COMP_HID_REPORT_DESC) {
				usb_os_memcpy((void *)buf, (void *)composite_hid_mouse_report_desc, report_len);
				ep0_in->xfer_len = MIN(report_len, req->wLength);
				usbd_ep_transmit(dev, ep0_in);
			} else if (USB_HIGH_BYTE(req->wValue) == COMP_HID_DESC) {
				len = COMP_HID_DESC_SIZE;
				usb_os_memcpy((void *)buf, (void *)usbd_composite_hid_desc, len);
				buf[COMP_HID_DESC_ITEM_LENGTH_OFFSET] = USB_LOW_BYTE(report_len);
				buf[COMP_HID_DESC_ITEM_LENGTH_OFFSET + 1] = USB_HIGH_BYTE(report_len);
				ep0_in->xfer_len = MIN(len, req->wLength);
				usbd_ep_transmit(dev, ep0_in);
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
	int ret = HAL_OK;
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;

	UNUSED(config);

	ep_intr_in->xfer_state = 0U;
	ep_intr_in->is_busy = 0U;
	/* Init INTR IN EP */
	ep_intr_in->mps = COMP_HID_INTR_IN_PACKET_SIZE;
	usbd_ep_init(dev, ep_intr_in);

	return ret;
}

static int composite_hid_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0;
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;

	UNUSED(config);

	/* DeInit INTR IN EP */
	usbd_ep_deinit(dev, ep_intr_in);

	return ret;
}

static int composite_hid_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;

	UNUSED(dev);

	if (status == HAL_OK) {
		/*TX done*/
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x TX err: %d\n", ep_addr, status);
	}

	if (hid->cb->transmitted) {
		hid->cb->transmitted(status);
	}

	ep_intr_in->xfer_state = 0U;

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
  * @param  len: Descriptor length
  * @retval descriptor buffer
  */
static u16 composite_hid_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	u16 len = 0;
	u16 report_len = sizeof(composite_hid_mouse_report_desc);

	UNUSED(dev);

	switch (USB_HIGH_BYTE(req->wValue)) {

	case USB_DESC_TYPE_CONFIGURATION:
	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		len = sizeof(usbd_composite_hid_itf_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_composite_hid_itf_desc, len);
		buf[COMP_HID_ITF_DESC_ITEM_LENGTH_OFFSET] = USB_LOW_BYTE(report_len);
		buf[COMP_HID_ITF_DESC_ITEM_LENGTH_OFFSET + 1] = USB_HIGH_BYTE(report_len);
		break;

	default:
		break;
	}

	return len;
}

/* Exported functions --------------------------------------------------------*/

int usbd_composite_hid_init(usbd_composite_dev_t *cdev, u16 tx_buf_len, usbd_composite_hid_usr_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;

	ep_intr_in->addr = USBD_COMP_HID_INTR_IN_EP;
	ep_intr_in->type = USB_CH_EP_TYPE_INTR;
	ep_intr_in->xfer_buf_len = tx_buf_len;
	ep_intr_in->xfer_buf = (u8 *)usb_os_malloc(tx_buf_len);

	if (ep_intr_in->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto usbd_composite_hid_init_exit;
	}

	if (cb != NULL) {
		hid->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				goto usbd_composite_hid_clean_all_exit;
			}
		}
	}

	hid->cdev = cdev;

	return ret;

usbd_composite_hid_clean_all_exit:
	if (ep_intr_in->xfer_buf) {
		usb_os_mfree(ep_intr_in->xfer_buf);
		ep_intr_in->xfer_buf = NULL;
	}

usbd_composite_hid_init_exit:
	return ret;
}

int usbd_composite_hid_deinit(void)
{
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;

	while (ep_intr_in->is_busy) {
		usb_os_delay_us(100);
	}

	if ((hid->cb != NULL) && (hid->cb->deinit != NULL)) {
		hid->cb->deinit();
	}

	if (ep_intr_in->xfer_buf != NULL) {
		usb_os_mfree(ep_intr_in->xfer_buf);
		ep_intr_in->xfer_buf = NULL;
	}

	return HAL_OK;
}

int usbd_composite_hid_send_data(u8 *data, u16 len)
{
	int ret = HAL_ERR_HW;
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usb_dev_t *dev = hid->cdev->dev;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;

	if (!dev->is_ready) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x TX %d not ready\n", USBD_COMP_HID_INTR_IN_EP, len);
		return ret;
	}

	if (len > ep_intr_in->xfer_buf_len) {
		len = ep_intr_in->xfer_buf_len;
	}

	if (!ep_intr_in->xfer_state) {
		if (dev->is_ready) { // In case deinit when plug out
			ep_intr_in->is_busy = 1U;
			ep_intr_in->xfer_state = 1U;

			usb_os_memcpy((void *)ep_intr_in->xfer_buf, (void *)data, len);
			if (dev->is_ready) { // In case deinit when plug out
				ep_intr_in->xfer_len = len;
				ret = usbd_ep_transmit(hid->cdev->dev, ep_intr_in);
			} else {
				ep_intr_in->xfer_state = 0U;
			}
			ep_intr_in->is_busy = 0U;
		} else {
			/*TX not ready*/
		}
	} else {
		/*TX busy*/
		ret = HAL_BUSY;
	}

	return ret;
}

