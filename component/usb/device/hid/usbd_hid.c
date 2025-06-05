/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_hid.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int hid_setup(usb_dev_t *dev, usb_setup_req_t *req);
static int hid_set_config(usb_dev_t *dev, u8 config);
static int hid_clear_config(usb_dev_t *dev, u8 config);
static int hid_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static u16 hid_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
static int hid_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static int hid_handle_ep0_data_out(usb_dev_t *dev);
#endif
static void hid_status_changed(usb_dev_t *dev, u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "HID";

/* USB Standard Device Descriptor */
static const u8 usbd_hid_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,								/* bLength */
	USB_DESC_TYPE_DEVICE,							/* bDescriptorType */
	0x00,											/* bcdUSB */
	0x02,
	0x00,											/* bDeviceClass */
	0x00,											/* bDeviceSubClass */
	0x00,											/* bDeviceProtocol */
	USB_MAX_EP0_SIZE,								/* bMaxPacketSize */
	USB_LOW_BYTE(USBD_HID_VID),						/* idVendor */
	USB_HIGH_BYTE(USBD_HID_VID),
	USB_LOW_BYTE(USBD_HID_PID),						/* idProduct */
	USB_HIGH_BYTE(USBD_HID_PID),
	0x00,											/* bcdDevice */
	0x02,
	USBD_HID_STRING_MANUFACTURER,					/* iManufacturer */
	USBD_HID_STRING_PRODUCT,						/* iProduct */
	USBD_HID_STRING_SERIALNUMBER,					/* iSerialNumber */
	0x01											/* bNumConfigurations */
};

/* USB Standard String Descriptor 0 */
static const u8 usbd_hid_lang_id_desc[USB_LEN_LANGID_STR_DESC] = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(USBD_HID_LANGID_STRING),           /* wLANGID */
	USB_HIGH_BYTE(USBD_HID_LANGID_STRING),
};

/* USB Standard Device Qualifier Descriptor */
static const u8 usbd_hid_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
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
};

/* USB HID device FS Configuration Descriptor */
static const u8 usbd_hid_fs_config_desc[] = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,								/*bLength*/
	USB_DESC_TYPE_CONFIGURATION, 					/*bDescriptorType: Configuration*/
	0x00,											/*wTotalLength, runtime assigned*/
	0x00,
	0x01,         									/*bNumInterfaces*/
	0x01,         									/*bConfigurationValue*/
	0x00,        									/*iConfiguration*/
	0x80,         									/*bmAttributes*/
	0x32,         									/*MaxPower 100 mA*/

	/* HID Interface Descriptor*/
	0x09,       									/*bLength*/
	USB_DESC_TYPE_INTERFACE,						/*bDescriptorType*/
	0x00,											/*bInterfaceNumber*/
	0x00,											/*bAlternateSetting*/
#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
	0x01,											/*bNumEndpoints*/
#else
	0x02,											/*bNumEndpoints*/
#endif
	0x03,											/*bInterfaceClass: HID*/
	0x01,											/*bInterfaceSubClass: 1=BOOT, 0=no boot*/
#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
	0x02,											/*nInterfaceProtocol: 0=none, 1=keyboard, 2=mouse*/
#else
	0x01,											/*nInterfaceProtocol: 0=none, 1=keyboard, 2=mouse*/
#endif
	0,												/*iInterface*/

	/*HID Descriptor*/
	0x09,											/*bLength*/
	USBD_HID_DESC, 									/*bDescriptorType*/
	0x11,											/*bcdHID*/
	0x01,
	0x00,											/*bCountryCode*/
	0x01,											/*bNumDescriptors*/
	USBD_HID_REPORT_DESC,							/*bDescriptorType*/
	0x00,											/*wItemLength, runtime assigned*/
	0x00,

	/* Endpoint Descriptor*/
	0x07,											/*bLength*/
	USB_DESC_TYPE_ENDPOINT,							/*bDescriptorType:*/
	USBD_HID_INTERRUPT_IN_EP_ADDRESS,				/*bEndpointAddress*/
	0x03,											/*bmAttributes*/
	USB_LOW_BYTE(USBD_HID_FS_INT_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_HID_FS_INT_MAX_PACKET_SIZE),
	0xA,											/*bInterval*/

#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	/* Endpoint Descriptor*/
	0x07,											/*bLength*/
	USB_DESC_TYPE_ENDPOINT,							/*bDescriptorType:*/
	USBD_HID_INTERRUPT_OUT_EP_ADDRESS,				/*bEndpointAddress*/
	0x03,											/*bmAttributes*/
	USB_LOW_BYTE(USBD_HID_FS_INT_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_HID_FS_INT_MAX_PACKET_SIZE),
	0xA,											/*bInterval*/
#endif
};

/* USB HID device HS Configuration Descriptor */
static const u8 usbd_hid_hs_config_desc[] = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,								/* bLength*/
	USB_DESC_TYPE_CONFIGURATION,					/* bDescriptorType*/
	0x00,											/* wTotalLength, runtime assigned*/
	0x00,
	0x01,											/*bNumInterfaces*/
	0x01,											/*bConfigurationValue*/
	0x00,											/*iConfiguration*/
	0x80,											/*bmAttributes*/
	0x32,											/*MaxPower*/

	/* HID Interface Descriptor*/
	0x09,											/*bLength*/
	USB_DESC_TYPE_INTERFACE,						/*bDescriptorType*/
	0x00,											/*bInterfaceNumber*/
	0x00,											/*bAlternateSetting*/
#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
	0x01,											/*bNumEndpoints*/
#else
	0x02,											/*bNumEndpoints*/
#endif
	0x03,											/*bInterfaceClass*/
	0x01,											/*bInterfaceSubClass: 1=BOOT, 0=no boot*/
#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
	0x02,											/*nInterfaceProtocol: 0=none, 1=keyboard, 2=mouse*/
#else
	0x01,											/*nInterfaceProtocol: 0=none, 1=keyboard, 2=mouse*/
#endif
	0,												/*iInterface*/

	/*HID Descriptor*/
	USBD_HID_DESC_SIZE,								/*bLength*/
	USBD_HID_DESC,									/*bDescriptorType*/
	0x11,											/*bcdHID*/
	0x01,
	0x00,											/*bCountryCode*/
	0x01,											/*bNumDescriptors*/
	USBD_HID_REPORT_DESC,							/*bDescriptorType*/
	0x00,											/*wItemLength, runtime assigned*/
	0x00,

	/* Endpoint Descriptor*/
	0x07,											/*bLength*/
	USB_DESC_TYPE_ENDPOINT,							/*bDescriptorType*/
	USBD_HID_INTERRUPT_IN_EP_ADDRESS,				/*bEndpointAddress*/
	0x03,											/*bmAttributest*/
	USB_LOW_BYTE(USBD_HID_HS_INT_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_HID_HS_INT_MAX_PACKET_SIZE),
	0xA,											/*bInterval*/

#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	/* Endpoint Descriptor*/
	0x07,											/*bLength*/
	USB_DESC_TYPE_ENDPOINT,							/*bDescriptorType:*/
	USBD_HID_INTERRUPT_OUT_EP_ADDRESS,				/*bEndpointAddress*/
	0x03,											/*bmAttributes*/
	USB_LOW_BYTE(USBD_HID_HS_INT_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_HID_HS_INT_MAX_PACKET_SIZE),
	0xA,											/*bInterval*/
#endif
};

/* USB HID Descriptor */
static const u8 usbd_hid_desc[USBD_HID_DESC_SIZE] = {
	USBD_HID_DESC_SIZE,								/*bLength: HID Descriptor size*/
	USBD_HID_DESC,									/*bDescriptorType: HID*/
	0x11,											/*bcdHID: HID Class Spec release number Byte0*/
	0x01,											/*bcdHID: HID Class Spec release number Byte1*/
	0x00,											/*bCountryCode: Hardware target country*/
	0x01,											/*bNumDescriptors: Number of HID class descriptors to follow*/
	USBD_HID_REPORT_DESC,							/*bDescriptorType*/
	0x00,											/*wItemLength: Total length of Report descriptor, runtime assigned*/
	0x00,
};

#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
/* HID Mouse Report Descriptor */
static const u8 hid_mouse_report_desc[] = {
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
#else
/* HID Keyboard Report Descriptor */
static const u8 hid_keyboard_report_desc[] = {
	0x05,	//bSize: 0x01, bType: Global, bTag: Usage Page
	0x01,	//Usage Page(Generic Desktop Controls )

	0x09,	//bSize: 0x01, bType: Local, bTag: Usage
	0x06,	//Usage(Keyboard)

	0xA1,	//bSize: 0x01, bType: Main, bTag: Collection
	0x01,	//Collection(Application )
	0x05,	//bSize: 0x01, bType: Global, bTag: Usage Page
	0x07,	//Usage Page(Key )
	0x19,	//bSize: 0x01, bType: Local, bTag: Usage Minimum
	0xE0,	//Usage Minimum(0xE0 )
	0x29,	//bSize: 0x01, bType: Local, bTag: Usage Maximum
	0xE7,	//Usage Maximum(0xE7 )
	0x15,	//bSize: 0x01, bType: Global, bTag: Logical Minimum
	0x00,	//Logical Minimum(0x0 )
	0x25,	//bSize: 0x01, bType: Global, bTag: Logical Maximum
	0x01,	//Logical Maximum(0x1 )
	0x95,	//bSize: 0x01, bType: Global, bTag: Report Count
	0x08,	//Report Count(0x8 )
	0x75,	//bSize: 0x01, bType: Global, bTag: Report Size
	0x01,	//Report Size(0x1 )
	0x81,	//bSize: 0x01, bType: Main, bTag: Input
	//left & right : alt win shift alt command
	0x02,	//Input(Data, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Bit Field)
	0x95,	//bSize: 0x01, bType: Global, bTag: Report Count
	0x01,	//Report Count(0x1 )
	0x75,	//bSize: 0x01, bType: Global, bTag: Report Size
	0x08,	//Report Size(0x8 )
	0x81,	//bSize: 0x01, bType: Main, bTag: Input
	//constant padding
	0x03,	//Input(Constant, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Bit Field)
	0x05,	//bSize: 0x01, bType: Global, bTag: Usage Page
	0x07,	//Usage Page(Key )
	0x95,	//bSize: 0x01, bType: Global, bTag: Report Count
	0x06,	//Report Count(0x6 )
	0x75,	//bSize: 0x01, bType: Global, bTag: Report Size
	0x08,	//Report Size(0x8 )
	0x15,	//bSize: 0x01, bType: Global, bTag: Logical Minimum
	0x00,	//Logical Minimum(0x0 )
	0x25,	//bSize: 0x01, bType: Global, bTag: Logical Maximum
	0xFF,	//Logical Maximum(0xFF )
	0x19,	//bSize: 0x01, bType: Local, bTag: Usage Minimum
	0x00,	//Usage Minimum(0x0 )
	0x29,	//bSize: 0x02, bType: Local, bTag: Usage Maximum
	0xFF,		//Usage Maximum(0xFF )
	0x81,	//bSize: 0x01, bType: Main, bTag: Input
	// 6 command key values
	0x00,	//Input(Data, Array, Absolute, No Wrap, Linear, Preferred State, No Null Position, Bit Field)


	0x05,	//bSize: 0x01, bType: Global, bTag: Usage Page
	0x08,	//Usage Page(LEDs )
	0x15,	//bSize: 0x01, bType: Global, bTag: Logical Minimum
	0x00,	//Logical Minimum(0x0 )
	0x25,	//bSize: 0x01, bType: Global, bTag: Logical Maximum
	0x01,	//Logical Maximum(0x1 )

	0x95,	//bSize: 0x01, bType: Global, bTag: Report Count
	0x05,	//Report Count(0x5 )
	0x75,	//bSize: 0x01, bType: Global, bTag: Report Size
	0x01,	//Report Size(0x1 )

	0x19,	//bSize: 0x01, bType: Local, bTag: Usage Minimum
	0x01,	//Usage Minimum(0x1 )
	0x29,	//bSize: 0x01, bType: Local, bTag: Usage Maximum
	0x05,	//Usage Maximum(0x5 )

	0x91,	//bSize: 0x01, bType: Main, bTag: Output
	0x02,	//Output(Data, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Non VolatileBit Field)
	0x95,	//bSize: 0x01, bType: Global, bTag: Report Count
	0x01,	//Report Count(0x1 )
	0x75,	//bSize: 0x01, bType: Global, bTag: Report Size
	0x03,	//Report Size(0x3 )
	0x91,	//bSize: 0x01, bType: Main, bTag: Output
	0x03,	//Output(Constant, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Non VolatileBit Field)

	0xC0	//bSize: 0x00, bType: Main, bTag: End Collection
};
#endif

static const usbd_class_driver_t usbd_hid_driver = {
	.get_descriptor = hid_get_descriptor,
	.set_config = hid_set_config,
	.clear_config = hid_clear_config,
	.setup = hid_setup,
	.ep_data_in = hid_handle_ep_data_in,
#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	.ep0_data_out = hid_handle_ep0_data_out,
	.ep_data_out = hid_handle_ep_data_out,
#endif
	.status_changed = hid_status_changed,
};

static usbd_hid_t hid_device;

/* Private functions ---------------------------------------------------------*/

#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE

/**
  * @brief  Prepare to receive INT OUT packet
  * @retval Status
  */
static int usbd_hid_receive(void)
{
	usbd_hid_t *hid = &hid_device;
	usbd_ep_receive(hid->dev, &hid->ep_intr_out);

	return HAL_OK;
}

static int hid_handle_ep0_data_out(usb_dev_t *dev)
{
	int ret = HAL_ERR_HW;
	usbd_hid_t *hid = &hid_device;

	UNUSED(dev);

	if (hid->ctrl_req.bRequest != 0xFFU) {
		hid->ctrl_req.bRequest = 0xFFU;
		ret = HAL_OK;
	}

	return ret;
}

static int hid_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len)
{
	usbd_hid_t *hid = &hid_device;
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;

	UNUSED(dev);

	if (hid->cb->received && (ep_addr == USBD_HID_INTERRUPT_OUT_EP_ADDRESS) && (len > 0)) {
		hid->cb->received(ep_intr_out->xfer_buf, len);
	}

	//continue
	usbd_hid_receive();

	return HAL_OK;
}

#endif // USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE

static int hid_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_hid_t *hid = &hid_device;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	int ret = HAL_OK;
	u16 len = 0;
	u16 report_len;
	u8 *buf = NULL;

#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
	report_len = sizeof(hid_mouse_report_desc);
#else
	report_len = sizeof(hid_keyboard_report_desc);
#endif

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
		case USB_REQ_GET_DESCRIPTOR:
			if (USB_HIGH_BYTE(req->wValue) == USBD_HID_REPORT_DESC) {
				/* HID Report Descriptor */
#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
				buf = (u8 *)hid_mouse_report_desc;
#else
				buf = (u8 *)hid_keyboard_report_desc;
#endif
				ep0_in->xfer_len = MIN(report_len, req->wLength);
				usb_os_memcpy((void *)ep0_in->xfer_buf, (void *)buf, report_len);
			} else if (USB_HIGH_BYTE(req->wValue) == USBD_HID_DESC) {
				/* HID Descriptor */
				len = USBD_HID_DESC_SIZE;
				buf = ep0_in->xfer_buf;
				usb_os_memcpy((void *)buf, (void *)usbd_hid_desc, len);
				buf[USBD_HID_DESC_ITEM_LENGTH_OFFSET] = USB_LOW_BYTE(report_len);
				buf[USBD_HID_DESC_ITEM_LENGTH_OFFSET + 1] = USB_HIGH_BYTE(report_len);
				ep0_in->xfer_len = MIN(len, req->wLength);
			} else {
				ret = HAL_ERR_PARA;
				break;
			}

			usbd_ep_transmit(dev, ep0_in);
			if (hid->cb->setup) {
				hid->cb->setup();
			}
			break;
		default:
			ret = HAL_ERR_PARA;
			break;
		}
		break;

	case USB_REQ_TYPE_CLASS :
		switch (req->bRequest) {
		case USBD_HID_SET_PROTOCOL:
			hid->protocol = USB_LOW_BYTE(req->wValue);
			break;
		case USBD_HID_GET_PROTOCOL:
			ep0_in->xfer_buf[0] = hid->protocol;
			ep0_in->xfer_len = 1U;
			usbd_ep_transmit(dev, ep0_in);
			break;
		case USBD_HID_SET_REPORT:
			if ((req->wLength) && (!(req->bmRequestType & 0x80U))) {
				usb_os_memcpy((void *)&hid->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				ep0_out->xfer_len = req->wLength;
				usbd_ep_receive(dev, ep0_out);
			}
			ret = HAL_OK;
			break;
		case USBD_HID_SET_IDLE:
			hid->idle_rate = USB_HIGH_BYTE(req->wValue);
			break;
		case USBD_HID_GET_IDLE:
			ep0_in->xfer_buf[0] = hid->idle_rate;
			ep0_in->xfer_len = 1U;
			usbd_ep_transmit(dev, ep0_in);
			break;
		default:
			ret = HAL_ERR_PARA;
			break;
		}
		break;
	default:
		ret = HAL_ERR_PARA;
		break;
	}

	return ret;
}

static int hid_set_config(usb_dev_t *dev, u8 config)
{
	int ret = HAL_OK;
	usbd_hid_t *hid = &hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;
#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;
#endif

	UNUSED(config);

	hid->dev = dev;

	/* Init INTR IN EP */
	ep_intr_in->xfer_state = 0;
	ep_intr_in->is_busy = 0U;
	ep_intr_in->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_HID_HS_INT_MAX_PACKET_SIZE : USBD_HID_FS_INT_MAX_PACKET_SIZE;
	usbd_ep_init(dev, ep_intr_in);

#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	/* Init INTR OUT EP */
	ep_intr_out->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_HID_HS_INT_MAX_PACKET_SIZE : USBD_HID_FS_INT_MAX_PACKET_SIZE;
	usbd_ep_init(dev, ep_intr_out);
	/* Prepare to receive next INTR OUT packet */
	usbd_hid_receive();
#endif

	return ret;
}

static int hid_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = HAL_OK;
	usbd_hid_t *hid = &hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;
#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;
#endif

	UNUSED(config);

	/* DeInit INTR IN EP */
	usbd_ep_deinit(dev, ep_intr_in);

#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	usbd_ep_deinit(dev, ep_intr_out);
#endif
	return ret;
}

static int hid_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_hid_t *hid = &hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;

	UNUSED(dev);

	if (status == HAL_OK) {
		/*TX done*/
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x TX err: %d\n", ep_addr, status);
	}

	hid->cb->transmitted(status);
	ep_intr_in->xfer_state = 0U;

	return HAL_OK;
}

/**
  * @brief  Get descriptor callback
  * @param  dev: USB device instance
  * @param  req: Setup request handle
  * @param  buf: Poniter to Buffer
  * @retval Descriptor length
  * @retval Status
  */
static u16 hid_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	u8 *desc = NULL;
	usb_speed_type_t speed = dev->dev_speed;
	u16 len = 0;
	u16 report_len;

#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
	report_len = sizeof(hid_mouse_report_desc);
#else
	report_len = sizeof(hid_keyboard_report_desc);
#endif

	dev->self_powered = USBD_HID_SELF_POWERED;

	switch (USB_HIGH_BYTE(req->wValue)) {

	case USB_DESC_TYPE_DEVICE:
		len = sizeof(usbd_hid_dev_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_hid_dev_desc, len);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_hid_hs_config_desc;
			len = sizeof(usbd_hid_hs_config_desc);
		} else {
			desc = (u8 *)usbd_hid_fs_config_desc;
			len = sizeof(usbd_hid_fs_config_desc);
		}
		usb_os_memcpy((void *)buf, (void *)desc, len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		buf[USBD_HID_CFG_DESC_ITEM_LENGTH_OFFSET] = USB_LOW_BYTE(report_len);
		buf[USBD_HID_CFG_DESC_ITEM_LENGTH_OFFSET + 1] = USB_HIGH_BYTE(report_len);
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		len = sizeof(usbd_hid_device_qualifier_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_hid_device_qualifier_desc, len);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_hid_fs_config_desc;
			len = sizeof(usbd_hid_fs_config_desc);
		} else {
			desc = (u8 *)usbd_hid_hs_config_desc;
			len = sizeof(usbd_hid_hs_config_desc);
		}
		usb_os_memcpy((void *)buf, (void *)desc, len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		buf[USBD_HID_CFG_DESC_ITEM_LENGTH_OFFSET] = USB_LOW_BYTE(report_len);
		buf[USBD_HID_CFG_DESC_ITEM_LENGTH_OFFSET + 1] = USB_HIGH_BYTE(report_len);
		break;

	case USB_DESC_TYPE_STRING:
		switch (USB_LOW_BYTE(req->wValue)) {
		case USBD_IDX_LANGID_STR:
			len = sizeof(usbd_hid_lang_id_desc);
			usb_os_memcpy((void *)buf, (void *)usbd_hid_lang_id_desc, len);
			break;
		case USBD_IDX_MFC_STR:
			len = usbd_get_str_desc(USBD_HID_MFG_STRING, buf);
			break;
		case USBD_IDX_PRODUCT_STR:
			if (speed == USB_SPEED_HIGH) {
				len = usbd_get_str_desc(USBD_HID_PROD_HS_STRING, buf);
			} else {
				len = usbd_get_str_desc(USBD_HID_PROD_FS_STRING, buf);
			}
			break;
		case USBD_IDX_SERIAL_STR:
			len = usbd_get_str_desc(USBD_HID_SN_STRING, buf);
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
  * @param  status: USB USB attach status
  * @retval void
  */
static void hid_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_hid_t *hid = &hid_device;

	UNUSED(dev);

	if (hid->cb->status_changed) {
		hid->cb->status_changed(old_status, status);
	}
}

/* Exported functions --------------------------------------------------------*/

int usbd_hid_init(u16 tx_buf_len, usbd_hid_usr_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_hid_t *hid = &hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;

#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;
	ep_intr_out->addr = USBD_HID_INTERRUPT_OUT_EP_ADDRESS;
	ep_intr_out->type = USB_CH_EP_TYPE_INTR;
	ep_intr_out->xfer_buf_len = USBD_HID_INTR_OUT_BUF_SIZE;
	ep_intr_out->xfer_buf = (u8 *)usb_os_malloc(ep_intr_out->xfer_buf_len);
	ep_intr_out->xfer_len = ep_intr_out->xfer_buf_len;
	if (ep_intr_out->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto usbd_hid_init_exit;
	}
#endif

	if (tx_buf_len == 0) {
		tx_buf_len = USBD_HID_INTR_IN_BUF_SIZE;
	}

	ep_intr_in->addr = USBD_HID_INTERRUPT_IN_EP_ADDRESS;
	ep_intr_in->type = USB_CH_EP_TYPE_INTR;
	ep_intr_in->xfer_buf_len = tx_buf_len;
	ep_intr_in->xfer_buf = (u8 *)usb_os_malloc(tx_buf_len);
	if (ep_intr_in->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto usbd_hid_init_clean_intr_out_buf_exit;
	}

	if (cb != NULL) {
		hid->cb = cb;
		if (cb->init != NULL) {
			cb->init();
		}
	}

	usbd_register_class(&usbd_hid_driver);

	return ret;

usbd_hid_init_clean_intr_out_buf_exit:
#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	if (ep_intr_out->xfer_buf) {
		usb_os_mfree(ep_intr_out->xfer_buf);
	}
	ep_intr_out->xfer_buf = NULL;
usbd_hid_init_exit:
#endif
	return ret;
}

int usbd_hid_deinit(void)
{
	usbd_hid_t *hid = &hid_device;

	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;
#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;
#endif

	while (ep_intr_in->is_busy) {
		usb_os_delay_us(100);
	}

	usbd_unregister_class();

	if ((hid->cb != NULL) && (hid->cb->deinit != NULL)) {
		hid->cb->deinit();
	}

	if (ep_intr_in->xfer_buf != NULL) {
		usb_os_mfree(ep_intr_in->xfer_buf);
		ep_intr_in->xfer_buf = NULL;
	}
#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	if (ep_intr_out->xfer_buf != NULL) {
		usb_os_mfree(ep_intr_out->xfer_buf);
		ep_intr_out->xfer_buf = NULL;
	}
#endif

	return HAL_OK;
}

int usbd_hid_send_data(u8 *data, u16 len)
{
	int ret = HAL_ERR_HW;
	usbd_hid_t *hid = &hid_device;
	usb_dev_t *dev = hid->dev;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;

	if (!dev->is_ready) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x TX %d not ready\n", USBD_HID_INTERRUPT_IN_EP_ADDRESS, len);
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
				usbd_ep_transmit(hid->dev, ep_intr_in);
				ret = HAL_OK;
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

