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

#include "usbd_hid.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
#define USBD_HID_CONFIG_DESC_SIZ                         34
#define USBD_HID_DEV_REPORT_DESC_SIZE                    52
#else
#define USBD_HID_CONFIG_DESC_SIZ                         41
#define USBD_HID_DEV_REPORT_DESC_SIZE                    67
#endif

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int hid_setup(usb_dev_t *dev, usb_setup_req_t *req);
static int hid_set_config(usb_dev_t *dev, u8 config);
static int hid_clear_config(usb_dev_t *dev, u8 config);
static int hid_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static u8 *hid_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len);
#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
static int hid_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static int hid_handle_ep0_data_out(usb_dev_t *dev);
#endif
static void hid_status_changed(usb_dev_t *dev, u8 status);

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "HID";

static u8 *usbd_hid_report_desc = NULL;

/* USB Standard Device Descriptor */
static u8 usbd_hid_dev_desc[USB_LEN_DEV_DESC] USB_DMA_ALIGNED = {
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
static u8 usbd_hid_lang_id_desc[USB_LEN_LANGID_STR_DESC] USB_DMA_ALIGNED = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(USBD_HID_LANGID_STRING),           /* wLANGID */
	USB_HIGH_BYTE(USBD_HID_LANGID_STRING),
};

/* USB Standard Device Qualifier Descriptor */
static u8 usbd_hid_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] USB_DMA_ALIGNED = {
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
static u8 usbd_hid_fs_config_desc[USBD_HID_CONFIG_DESC_SIZ] USB_DMA_ALIGNED = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,								/*bLength*/
	USB_DESC_TYPE_CONFIGURATION, 					/*bDescriptorType: Configuration*/
	USBD_HID_CONFIG_DESC_SIZ,						/*wTotalLength*/
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
	USBD_HID_DEV_REPORT_DESC_SIZE,					/*wItemLength*/
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
static u8 usbd_hid_hs_config_desc[USBD_HID_CONFIG_DESC_SIZ] USB_DMA_ALIGNED = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,								/* bLength*/
	USB_DESC_TYPE_CONFIGURATION,					/* bDescriptorType*/
	USBD_HID_CONFIG_DESC_SIZ,						/* wTotalLength*/
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
	0x09,											/*bLength*/
	USBD_HID_DESC,									/*bDescriptorType*/
	0x11,											/*bcdHID*/
	0x01,
	0x00,											/*bCountryCode*/
	0x01,											/*bNumDescriptors*/
	USBD_HID_REPORT_DESC,							/*bDescriptorType*/
	USBD_HID_DEV_REPORT_DESC_SIZE,					/*wItemLength*/
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
static u8 usbd_hid_desc[USBD_HID_DESC_SIZE] USB_DMA_ALIGNED = {
	0x09,											/*bLength: HID Descriptor size*/
	USBD_HID_DESC,									/*bDescriptorType: HID*/
	0x11,											/*bcdHID: HID Class Spec release number Byte0*/
	0x01,											/*bcdHID: HID Class Spec release number Byte1*/
	0x00,											/*bCountryCode: Hardware target country*/
	0x01,											/*bNumDescriptors: Number of HID class descriptors to follow*/
	USBD_HID_REPORT_DESC,							/*bDescriptorType*/
	USBD_HID_DEV_REPORT_DESC_SIZE,					/*wItemLength: Total length of Report descriptor Byte0*/
	0x00,		    								/*wItemLength: Total length of Report descriptor Byte1*/
};

#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
/* HID Mouse Report Descriptor */
static u8 hid_mouse_report_desc[USBD_HID_DEV_REPORT_DESC_SIZE] USB_DMA_ALIGNED = {
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
static u8 hid_keyboard_report_desc[USBD_HID_DEV_REPORT_DESC_SIZE] USB_DMA_ALIGNED = {
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

static usbd_class_driver_t usbd_hid_driver = {
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

	usbd_ep_receive(hid->dev, USBD_HID_INTERRUPT_OUT_EP_ADDRESS, hid->intr_out_buf, hid->intr_out_buf_size);

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
	UNUSED(dev);

	if (hid->cb->received && (ep_addr == USBD_HID_INTERRUPT_OUT_EP_ADDRESS) && (len > 0)) {
		hid->cb->received(hid->intr_out_buf, len);
	}

	//continue
	usbd_hid_receive();

	return HAL_OK;
}

#endif // USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE

static int hid_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_hid_t *hid = &hid_device;
	int ret = HAL_OK;
	u16 len = 0;
	u8 *buf = NULL;

	//RTK_LOGD(TAG, "SETUP: bmRequestType=0x%02x bRequest=0x%02x wLength=0x%04x wValue=%x\n",
	//		 req->bmRequestType, req->bRequest, req->wLength, req->wValue);

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
				hid->ctrl_buf[0] = 0U;
				usbd_ep0_transmit(dev, hid->ctrl_buf, 1);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;
		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				hid->ctrl_buf[0] = 0U;
				hid->ctrl_buf[1] = 0U;
				usbd_ep0_transmit(dev, hid->ctrl_buf, 2);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;
		case USB_REQ_GET_DESCRIPTOR:
			if (req->wValue >> 8 == USBD_HID_REPORT_DESC) {
				len = MIN(USBD_HID_DEV_REPORT_DESC_SIZE, req->wLength);
				buf = usbd_hid_report_desc;
			} else if (req->wValue >> 8 == USBD_HID_DESC) {
				/* HID Descriptor */
				len = MIN(USBD_HID_DESC_SIZE, req->wLength);
				buf = usbd_hid_desc;
			} else {
				ret = HAL_ERR_PARA;
				break;
			}
			usbd_ep0_transmit(dev, buf, len);
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
			hid->protocol = (u8)(req->wValue & 0xFF);
			break;
		case USBD_HID_GET_PROTOCOL:
			hid->ctrl_buf[0] = hid->protocol;
			usbd_ep0_transmit(dev, hid->ctrl_buf, 1U);
			break;
		case USBD_HID_SET_REPORT:
			if ((req->wLength) && (!(req->bmRequestType & 0x80U))) {
				usb_os_memcpy((void *)&hid->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				usbd_ep0_receive(dev, dev->ctrl_buf, req->wLength);
			}
			ret = HAL_OK;
			break;
		case USBD_HID_SET_IDLE:
			hid->idle_rate = (u8)(req->wValue >> 8);
			break;
		case USBD_HID_GET_IDLE:
			hid->ctrl_buf[0] = hid->idle_rate;
			usbd_ep0_transmit(dev, hid->ctrl_buf, 1U);
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
	u16 ep_mps;

	UNUSED(config);

	hid->dev = dev;
	hid->intr_in_state = 0;

	/* Init INTR IN EP */
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_HID_HS_INT_MAX_PACKET_SIZE : USBD_HID_FS_INT_MAX_PACKET_SIZE;
	usbd_ep_init(dev, USBD_HID_INTERRUPT_IN_EP_ADDRESS, USB_CH_EP_TYPE_INTR, ep_mps);

#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	/* Init INTR OUT EP */
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_HID_HS_INT_MAX_PACKET_SIZE : USBD_HID_FS_INT_MAX_PACKET_SIZE;
	usbd_ep_init(dev, USBD_HID_INTERRUPT_OUT_EP_ADDRESS, USB_CH_EP_TYPE_INTR, ep_mps);
	/* Prepare to receive next INTR OUT packet */
	usbd_hid_receive();
#endif

	hid->is_ready = 1;

	return ret;
}

static int hid_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = HAL_OK;
	usbd_hid_t *hid = &hid_device;

	UNUSED(config);

	hid->is_ready = 0;

	/* DeInit INTR IN EP */
	usbd_ep_deinit(dev, USBD_HID_INTERRUPT_IN_EP_ADDRESS);

	return ret;
}

static int hid_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_hid_t *hid = &hid_device;

	UNUSED(dev);

	if (status == HAL_OK) {
		/*TX done*/
	} else {
		RTK_LOGS(TAG, "[HID] EP%02x TX err: %d\n", ep_addr, status);
	}

	hid->cb->transmitted(status);
	hid->intr_in_state = 0U;

	return HAL_OK;
}

static u8 *hid_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len)
{
	u8 *buf = NULL;
	u8 *desc = hid_device.ctrl_buf;

	dev->self_powered = USBD_HID_SELF_POWERED;
	//RTK_LOGD(TAG, "Get desc：speed=%d bmRequestType=0x%02x bRequest=0x%02x wLength=0x%04x wValue=%x\n",
	//		 speed, req->bmRequestType, req->bRequest, req->wLength, req->wValue);

	switch ((req->wValue >> 8) & 0xFF) {

	case USB_DESC_TYPE_DEVICE:
		buf = usbd_hid_dev_desc;
		*len = sizeof(usbd_hid_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			buf = usbd_hid_hs_config_desc;
			*len = sizeof(usbd_hid_hs_config_desc);
		} else {
			buf = usbd_hid_fs_config_desc;
			*len = sizeof(usbd_hid_fs_config_desc);
		}
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		buf = usbd_hid_device_qualifier_desc;
		*len = sizeof(usbd_hid_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			buf = usbd_hid_fs_config_desc;
			*len = sizeof(usbd_hid_fs_config_desc);
		} else {
			buf = usbd_hid_hs_config_desc;
			*len = sizeof(usbd_hid_hs_config_desc);
		}
		usb_os_memcpy((void *)desc, (void *)buf, *len);
		desc[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		buf = desc;
		break;

	case USB_DESC_TYPE_STRING:
		switch (req->wValue & 0xFF) {
		case USBD_IDX_LANGID_STR:
			buf = usbd_hid_lang_id_desc;
			*len = sizeof(usbd_hid_lang_id_desc);
			break;
		case USBD_IDX_MFC_STR:
			usbd_get_str_desc(USBD_HID_MFG_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_PRODUCT_STR:
			if (speed == USB_SPEED_HIGH) {
				usbd_get_str_desc(USBD_HID_PROD_HS_STRING, desc, len);
			} else {
				usbd_get_str_desc(USBD_HID_PROD_FS_STRING, desc, len);
			}
			buf = desc;
			break;
		case USBD_IDX_SERIAL_STR:
			usbd_get_str_desc(USBD_HID_SN_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_MS_OS_STR:
			break;
		/* Add customer string here */
		default:
			RTK_LOGS(TAG, "[HID] Invalid str idx %d\n", req->wValue & 0xFF);
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
  * @param  status: USB attach status
  * @retval void
  */
static void hid_status_changed(usb_dev_t *dev, u8 status)
{
	usbd_hid_t *hid = &hid_device;

	UNUSED(dev);

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		hid->is_ready = 0;
	}

	if (hid->cb->status_changed) {
		hid->cb->status_changed(status);
	}
}

/* Exported functions --------------------------------------------------------*/

int usbd_hid_init(u16 tx_buf_len, usbd_hid_usr_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_hid_t *hid = &hid_device;

#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	hid->intr_out_buf_size = USBD_HID_INTR_OUT_BUF_SIZE;
	hid->intr_out_buf = (u8 *)usb_os_malloc(hid->intr_out_buf_size);
	if (hid->intr_out_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto usbd_hid_init_exit;
	}
#endif

	hid->ctrl_buf = (u8 *)usb_os_malloc(USBD_HID_CTRL_BUF_SIZE);
	if (hid->ctrl_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto usbd_hid_init_clean_intr_out_buf_exit;
	}

	if (tx_buf_len == 0) {
		tx_buf_len = USBD_HID_INTR_IN_BUF_SIZE;
	}
	hid->intr_in_buf_len = tx_buf_len;
	hid->intr_in_buf = (u8 *)usb_os_malloc(tx_buf_len);
	if (hid->intr_in_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto usbd_hid_init_clean_ctrl_buf_exit;
	}

	if (cb != NULL) {
		hid->cb = cb;
		if (cb->init != NULL) {
			cb->init();
		}
	}

#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
	usbd_hid_report_desc = hid_mouse_report_desc;
#else
	usbd_hid_report_desc = hid_keyboard_report_desc;
#endif

	usbd_register_class(&usbd_hid_driver);

	return ret;

usbd_hid_init_clean_ctrl_buf_exit:
	if (hid->ctrl_buf) {
		usb_os_mfree(hid->ctrl_buf);
	}
	hid->ctrl_buf = NULL;

usbd_hid_init_clean_intr_out_buf_exit:
#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	if (hid->intr_out_buf) {
		usb_os_mfree(hid->intr_out_buf);
	}
	hid->intr_out_buf = NULL;
usbd_hid_init_exit:
#endif
	return ret;
}

int usbd_hid_deinit(void)
{
	usbd_hid_t *hid = &hid_device;

	hid->is_ready = 0;

	while (hid->is_tx_busy) {
		usb_os_delay_us(100);
	}

	usbd_unregister_class();

	if ((hid->cb != NULL) && (hid->cb->deinit != NULL)) {
		hid->cb->deinit();
	}

	if (hid->intr_in_buf != NULL) {
		usb_os_mfree(hid->intr_in_buf);
		hid->intr_in_buf = NULL;
	}
#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	if (hid->intr_out_buf != NULL) {
		usb_os_mfree(hid->intr_out_buf);
		hid->intr_out_buf = NULL;
	}
#endif

	if (hid->ctrl_buf != NULL) {
		usb_os_mfree(hid->ctrl_buf);
		hid->ctrl_buf = NULL;
	}

	return HAL_OK;
}

int usbd_hid_send_data(u8 *data, u16 len)
{
	int ret = HAL_ERR_HW;
	usbd_hid_t *hid = &hid_device;

	if (!hid->is_ready) {
		RTK_LOGS(TAG, "[HID] EP%02x TX %d not ready\n", USBD_HID_INTERRUPT_IN_EP_ADDRESS, len);
		return ret;
	}

	if (len > hid->intr_in_buf_len) {
		len = hid->intr_in_buf_len;
	}

	if (!hid->intr_in_state) {
		if (hid->is_ready) { // In case deinit when plug out
			hid->is_tx_busy = 1U;
			hid->intr_in_state = 1U;
			usb_os_memcpy((void *)hid->intr_in_buf, (void *)data, len);
			if (hid->is_ready) { // In case deinit when plug out
				usbd_ep_transmit(hid->dev, USBD_HID_INTERRUPT_IN_EP_ADDRESS, hid->intr_in_buf, len);
				ret = HAL_OK;
			} else {
				hid->intr_in_state = 0U;
			}
			hid->is_tx_busy = 0U;
		} else {
			/*TX not ready*/
		}
	} else {
		/*TX busy*/
		ret = HAL_BUSY;
	}

	return ret;
}

