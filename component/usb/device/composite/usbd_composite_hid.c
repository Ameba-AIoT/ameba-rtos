/**
  ******************************************************************************
  * @file    usbd_cdc_acm.c
  * @author  Realsil WLAN5 Team
  * @brief   This file provides the functionalities of the USB CDC Class
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

#include "usbd_composite_hid.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int composite_hid_setup(usb_dev_t *dev, usb_setup_req_t *req);
static int composite_hid_set_config(usb_dev_t *dev, u8 config);
static int composite_hid_clear_config(usb_dev_t *dev, u8 config);
static int composite_hid_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static u8 *composite_hid_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len);
static int composite_hid_handle_ep0_data_out(usb_dev_t *dev);

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "COMP";

/* HID interface descriptor */
static u8 usbd_composite_hid_itf_desc[] USB_DMA_ALIGNED = {
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
	COMP_HID_REPORT_DESC_SIZE,			/*wItemLength*/
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
static u8 usbd_composite_hid_desc[] USB_DMA_ALIGNED = {
	0x09,								/*bLength: HID Descriptor size*/
	COMP_HID_DESC,						/*bDescriptorType: HID*/
	0x11,								/*bcdHID: HID Class Spec release number Byte0*/
	0x01,								/*bcdHID: HID Class Spec release number Byte1*/
	0x00,								/*bCountryCode: Hardware target country*/
	0x01,								/*bNumDescriptors: Number of HID class descriptors to follow*/
	COMP_HID_REPORT_DESC,				/*bDescriptorType*/
	COMP_HID_REPORT_DESC_SIZE,		    /*wItemLength: Total length of Report descriptor Byte0*/
	0x00,		    					/*wItemLength: Total length of Report descriptor Byte1*/
};

// HID mouse report descriptor
static u8 composite_hid_mouse_report_desc[COMP_HID_REPORT_DESC_SIZE] USB_DMA_ALIGNED = {
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

usbd_class_driver_t usbd_composite_hid_driver = {
	.get_descriptor = composite_hid_get_descriptor,
	.set_config = composite_hid_set_config,
	.clear_config = composite_hid_clear_config,
	.setup = composite_hid_setup,
	.ep0_data_out = composite_hid_handle_ep0_data_out,
	.ep_data_in = composite_hid_handle_ep_data_in,
};

static usbd_composite_hid_device_t composite_hid_device;

/* Private functions ---------------------------------------------------------*/

static int composite_hid_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	int ret = HAL_OK;
	u16 len = 0;
	u8 *buf = NULL;
	usbd_composite_hid_device_t *hid = &composite_hid_device;
	usbd_composite_dev_t *cdev = hid->cdev;

	//RTK_LOGD(TAG, "SETUP: bmRequestType=0x%02x bRequest=0x%02x wLength=0x%04x wValue=%x\n",
	//		 req->bmRequestType, req->bRequest, req->wLength, req->wValue);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_GET_DESCRIPTOR:
			if (req->wValue >> 8 == COMP_HID_REPORT_DESC) {
				len = MIN(COMP_HID_REPORT_DESC_SIZE, req->wLength);
				buf = composite_hid_mouse_report_desc;
				usbd_ep0_transmit(dev, buf, len);
			} else if (req->wValue >> 8 == COMP_HID_DESC) {
				len = MIN(sizeof(usbd_composite_hid_desc), req->wLength);
				buf = usbd_composite_hid_desc;
				usbd_ep0_transmit(dev, buf, len);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;
		default:
			RTK_LOGS(TAG, "[COMP] Invalid bRequest 0x%02x\n", req->bRequest);
			ret = HAL_ERR_PARA;
			break;
		}
		break;
	case USB_REQ_TYPE_CLASS:
		if (req->wLength) {
			if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
				ret = hid->cb->setup(req, cdev->ctrl_buf);
				if (ret == HAL_OK) {
					usbd_ep0_transmit(dev, cdev->ctrl_buf, req->wLength);
				}
			} else {
				usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
			}
		} else {
			ret = hid->cb->setup(req, cdev->ctrl_buf);
		}
		break;
	default:
		RTK_LOGS(TAG, "[COMP] Invalid bmRequestType 0x%02x\n", req->bmRequestType);
		ret = HAL_ERR_PARA;
		break;
	}

	return ret;
}

static int composite_hid_set_config(usb_dev_t *dev, u8 config)
{
	int ret = HAL_OK;
	usbd_composite_hid_device_t *hid = &composite_hid_device;

	UNUSED(config);

	hid->intr_in_state = 0U;

	/* Init INTR IN EP */
	usbd_ep_init(dev, USBD_COMP_HID_INTR_IN_EP, USB_CH_EP_TYPE_INTR, COMP_HID_INTR_IN_PACKET_SIZE);

	hid->is_ready = 1U;

	return ret;
}

static int composite_hid_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0;
	usbd_composite_hid_device_t *hid = &composite_hid_device;

	UNUSED(config);

	hid->is_ready = 0U;

	/* DeInit INTR IN EP */
	usbd_ep_deinit(dev, USBD_COMP_HID_INTR_IN_EP);

	return ret;
}

static int composite_hid_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_composite_hid_device_t *hid = &composite_hid_device;

	UNUSED(dev);

	if (status == HAL_OK) {
		/*TX done*/
	} else {
		RTK_LOGS(TAG, "[COMP] EP%02x TX err: %d\n", ep_addr, status);
	}

	if (hid->cb->transmitted) {
		hid->cb->transmitted(status);
	}

	hid->intr_in_state = 0U;

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

	UNUSED(dev);

	if (cdev->ctrl_req.bRequest != 0xFFU) {
		hid->cb->setup(&cdev->ctrl_req, cdev->ctrl_buf);
		cdev->ctrl_req.bRequest = 0xFFU;

		ret = HAL_OK;
	}

	return ret;
}

static u8 *composite_hid_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len)
{
	u8 *buf = NULL;

	UNUSED(dev);
	UNUSED(speed);

	switch ((req->wValue >> 8) & 0xFF) {

	case USB_DESC_TYPE_CONFIGURATION:
		buf = usbd_composite_hid_itf_desc;
		*len = sizeof(usbd_composite_hid_itf_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		buf = usbd_composite_hid_itf_desc;
		*len = sizeof(usbd_composite_hid_itf_desc);
		break;

	default:
		break;
	}

	return buf;
}

/* Exported functions --------------------------------------------------------*/

int usbd_composite_hid_init(usbd_composite_dev_t *cdev, u16 tx_buf_len, usbd_composite_hid_usr_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_composite_hid_device_t *hid = &composite_hid_device;

	hid->intr_in_buf_size = tx_buf_len;
	hid->intr_in_buf = (u8 *)usb_os_malloc(hid->intr_in_buf_size);
	if (hid->intr_in_buf == NULL) {
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
	if (hid->intr_in_buf) {
		usb_os_mfree(hid->intr_in_buf);
		hid->intr_in_buf = NULL;
	}

usbd_composite_hid_init_exit:
	return ret;
}

int usbd_composite_hid_deinit(void)
{
	usbd_composite_hid_device_t *hid = &composite_hid_device;

	hid->is_ready = 0U;

	while (hid->is_intr_in_busy) {
		usb_os_delay_us(100);
	}

	if ((hid->cb != NULL) && (hid->cb->deinit != NULL)) {
		hid->cb->deinit();
	}

	if (hid->intr_in_buf != NULL) {
		usb_os_mfree(hid->intr_in_buf);
		hid->intr_in_buf = NULL;
	}

	return HAL_OK;
}

int usbd_composite_hid_send_data(u8 *data, u16 len)
{
	int ret = HAL_ERR_HW;
	usbd_composite_hid_device_t *hid = &composite_hid_device;

	if (!hid->is_ready) {
		RTK_LOGS(TAG, "[COMP] EP%02x TX %d not ready\n", USBD_COMP_HID_INTR_IN_EP, len);
		return ret;
	}

	if (len > hid->intr_in_buf_size) {
		len = hid->intr_in_buf_size;
	}

	if (!hid->intr_in_state) {
		if (hid->is_ready) { // In case deinit when plug out
			hid->is_intr_in_busy = 1U;
			hid->intr_in_state = 1U;

			usb_os_memcpy((void *)hid->intr_in_buf, (void *)data, len);
			if (hid->is_ready) { // In case deinit when plug out
				usbd_ep_transmit(hid->cdev->dev, USBD_COMP_HID_INTR_IN_EP, hid->intr_in_buf, len);
				ret = HAL_OK;
			} else {
				hid->intr_in_state = 0U;
			}
			hid->is_intr_in_busy = 0U;
		} else {
			/*TX not ready*/
		}
	} else {
		/*TX busy*/
		ret = HAL_BUSY;
	}

	return ret;
}

