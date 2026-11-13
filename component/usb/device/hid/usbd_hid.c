/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_hid.h"
#ifdef CONFIG_USBD_COMPOSITE
#include "usbd_composite.h"
#endif
#include "usb_hid.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

/* Interface number, every variant has a single HID interface */
#define USBD_HID_ITF_NUM                     0x00U

/* Ref HID 1.11 4.2 and 4.3: only the mouse and the keyboard support the boot protocol, the
   other variants declare no subclass and no protocol */
#if defined(CONFIG_USBD_HID_MOUSE)
#define USBD_HID_ITF_SUBCLASS                0x01U
#define USBD_HID_ITF_PROTOCOL                USB_HID_PROTOCOL_MOUSE
#elif defined(CONFIG_USBD_HID_KEYBOARD)
#define USBD_HID_ITF_SUBCLASS                0x01U
#define USBD_HID_ITF_PROTOCOL                USB_HID_PROTOCOL_KEYBOARD
#else
#define USBD_HID_ITF_SUBCLASS                USB_HID_SUBCLASS_NONE
#define USBD_HID_ITF_PROTOCOL                USB_HID_PROTOCOL_NONE
#endif

/* Mouse and headset report to the host only, keyboard and vendor own an INTR OUT endpoint too */
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
#define USBD_HID_ITF_EP_NUM                  0x02U
#else
#define USBD_HID_ITF_EP_NUM                  0x01U
#endif

// HID request
#define USBD_HID_GET_REPORT						0x01
#define USBD_HID_GET_IDLE						0x02
#define USBD_HID_GET_PROTOCOL					0x03
#define USBD_HID_SET_REPORT						0x09
#define USBD_HID_SET_IDLE						0x0a
#define USBD_HID_SET_PROTOCOL					0x0b

// wValue of HID report request
#define USBD_HID_DESC							0x21
#define USBD_HID_REPORT_DESC					0x22

/* Report type in the high byte of the GET_REPORT/SET_REPORT wValue, Ref HID 1.11 7.2.1 */
#define USBD_HID_REPORT_TYPE_INPUT           0x01U
#define USBD_HID_REPORT_TYPE_OUTPUT          0x02U

/* Length of the Input report the interface declares, Report ID included where one is declared */
#if defined(CONFIG_USBD_HID_VENDOR)
#define USBD_HID_INPUT_REPORT_LEN            USBD_HID_VENDOR_REPORT_LEN
#elif defined(CONFIG_USBD_HID_HEADSET)
#define USBD_HID_INPUT_REPORT_LEN            USBD_HID_HEADSET_REPORT_LEN
#elif defined(CONFIG_USBD_HID_MOUSE)
#define USBD_HID_INPUT_REPORT_LEN            4U
#else
#define USBD_HID_INPUT_REPORT_LEN            8U
#endif

/* Length of the Output report the interface declares, Report ID included where one is declared.
   The mouse and the headset declare none. */
#if defined(CONFIG_USBD_HID_VENDOR)
#define USBD_HID_OUTPUT_REPORT_LEN           USBD_HID_VENDOR_REPORT_LEN
#elif defined(CONFIG_USBD_HID_KEYBOARD)
#define USBD_HID_OUTPUT_REPORT_LEN           1U
#endif

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int hid_setup(usb_dev_t *dev, usb_setup_req_t *req);
static int hid_set_config(usb_dev_t *dev, u8 config);
static void hid_clear_config(usb_dev_t *dev, u8 config);
static int hid_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, int status);
static u16 hid_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
static int hid_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static int hid_handle_ep0_data_out(usb_dev_t *dev);
#endif
static void hid_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
static u16 usbd_hid_get_input_report_len(u16 itf, u16 value);
static u16 usbd_hid_get_output_report_len(u16 itf, u16 value);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "HID";

/* USB Standard Device Descriptor */
static const u8 usbd_hid_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,								/* bLength */
	USB_DESC_TYPE_DEVICE,							/* bDescriptorType */
	0x00,											/* bcdUSB */
	0x02,
	0x00,											/* bDeviceClass: set per interface */
	0x00,											/* bDeviceSubClass */
	0x00,											/* bDeviceProtocol */
	USB_MAX_EP0_SIZE,								/* bMaxPacketSize */
	USB_LOW_BYTE(USB_VID),							/* idVendor (patched by core from usbd_dev_info_t) */
	USB_HIGH_BYTE(USB_VID),
	USB_LOW_BYTE(USB_PID),							/* idProduct (patched by core from usbd_dev_info_t) */
	USB_HIGH_BYTE(USB_PID),
	0x00,											/* bcdDevice (patched by core from usbd_dev_info_t) */
	0x02,
	USBD_IDX_MFC_STR,								/* iManufacturer (patched by core) */
	USBD_IDX_PRODUCT_STR,							/* iProduct (patched by core) */
	USBD_IDX_SERIAL_STR,							/* iSerialNumber (patched by core) */
	0x01											/* bNumConfigurations */
};

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
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
#endif

/* USB HID device Configuration Descriptor, wMaxPacketSize and bInterval are patched for the
   speed it describes, refer to usbd_hid_patch_desc() */
static const u8 usbd_hid_config_desc[] = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,								/*bLength*/
	USB_DESC_TYPE_CONFIGURATION, 					/*bDescriptorType: Configuration*/
	0x00,											/*wTotalLength, runtime assigned*/
	0x00,
	0x01,											/*bNumInterfaces*/
	0x01,         									/*bConfigurationValue*/
	0x00,        									/*iConfiguration*/
	0x80,         									/* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,         									/*MaxPower 100 mA*/

	/* HID Interface Descriptor*/
	USB_LEN_IF_DESC,								/*bLength*/
	USB_DESC_TYPE_INTERFACE,						/*bDescriptorType*/
	USBD_HID_ITF_NUM,								/*bInterfaceNumber*/
	0x00,											/*bAlternateSetting*/
	USBD_HID_ITF_EP_NUM,							/*bNumEndpoints*/
	USB_HID_CLASS_CODE,								/*bInterfaceClass: HID*/
	USBD_HID_ITF_SUBCLASS,							/*bInterfaceSubClass: 1=BOOT, 0=no boot*/
	USBD_HID_ITF_PROTOCOL,							/*bInterfaceProtocol: 1=keyboard, 2=mouse, 0=none*/
	0,												/*iInterface*/

	/*HID Descriptor*/
	USBD_HID_DESC_SIZE,								/*bLength*/
	USBD_HID_DESC, 									/*bDescriptorType*/
	0x11,											/*bcdHID*/
	0x01,
	0x00,											/*bCountryCode*/
	0x01,											/*bNumDescriptors*/
	USBD_HID_REPORT_DESC,							/*bDescriptorType*/
	0x00,											/*wItemLength, runtime assigned*/
	0x00,

	/* Endpoint Descriptor*/
	USB_LEN_EP_DESC,								/*bLength*/
	USB_DESC_TYPE_ENDPOINT,							/*bDescriptorType:*/
	USB_D2H,										/*bEndpointAddress*/
	0x03,											/*bmAttributes*/
	USB_LOW_BYTE(USBD_HID_HS_INT_MAX_PACKET_SIZE),  /* wMaxPacketSize, patched at runtime */
	USB_HIGH_BYTE(USBD_HID_HS_INT_MAX_PACKET_SIZE),
	USBD_HID_DEFAULT_HS_INTR_INTERVAL,				/*bInterval, patched at runtime*/

#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	/* Endpoint Descriptor*/
	USB_LEN_EP_DESC,								/*bLength*/
	USB_DESC_TYPE_ENDPOINT,							/*bDescriptorType:*/
	USB_H2D,										/*bEndpointAddress*/
	0x03,											/*bmAttributes*/
	USB_LOW_BYTE(USBD_HID_HS_INT_MAX_PACKET_SIZE),  /* wMaxPacketSize, patched at runtime */
	USB_HIGH_BYTE(USBD_HID_HS_INT_MAX_PACKET_SIZE),
	USBD_HID_DEFAULT_HS_INTR_INTERVAL,				/*bInterval, patched at runtime*/
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

#if defined(CONFIG_USBD_HID_VENDOR)
/* Vendor Report Descriptor: one Input and one Output report of opaque bytes, each with its own
   Report ID. A customer protocol adds or resizes reports here, refer to usbd_hid.h. */
static const u8 hid_vendor_report_desc[] = {
	0x06, 0x00, 0xFF,                            /* Usage Page (Vendor Defined 0xFF00) */
	0x09, 0x01,                                  /* Usage (Vendor Usage 1) */
	0xA1, 0x01,                                  /* Collection (Application) */
	0x15, 0x00,                                  /*   Logical Minimum (0) */
	0x26, 0xFF, 0x00,                            /*   Logical Maximum (255) */
	0x75, 0x08,                                  /*   Report Size (8) */

	0x85, USBD_HID_VENDOR_IN_REPORT_ID,          /*   Report ID */
	0x95, USBD_HID_VENDOR_REPORT_PAYLOAD_LEN,    /*   Report Count */
	0x09, 0x02,                                  /*   Usage (Vendor Usage 2) */
	0x81, 0x02,                                  /*   Input (Data,Var,Abs) */

	0x85, USBD_HID_VENDOR_OUT_REPORT_ID,         /*   Report ID */
	0x95, USBD_HID_VENDOR_REPORT_PAYLOAD_LEN,    /*   Report Count */
	0x09, 0x03,                                  /*   Usage (Vendor Usage 3) */
	0x91, 0x02,                                  /*   Output (Data,Var,Abs) */

	0xC0                                         /* End Collection */
};
#elif defined(CONFIG_USBD_HID_HEADSET)
/* Headset Report Descriptor: the buttons of a USB Type-C headset, Ref Android USB headset
   specification. One bit per button in the order of USBD_HID_HEADSET_KEY_*, no Report ID. */
static const u8 hid_headset_report_desc[] = {
	0x05, 0x0C,                                  /* Usage Page (Consumer) */
	0x09, 0x01,                                  /* Usage (Consumer Control) */
	0xA1, 0x01,                                  /* Collection (Application) */
	0x15, 0x00,                                  /*   Logical Minimum (0) */
	0x25, 0x01,                                  /*   Logical Maximum (1) */
	0x75, 0x01,                                  /*   Report Size (1) */
	0x95, 0x05,                                  /*   Report Count (5) */
	0x09, 0xCD,                                  /*   Usage (Play/Pause) */
	0x09, 0xE9,                                  /*   Usage (Volume Increment) */
	0x09, 0xEA,                                  /*   Usage (Volume Decrement) */
	0x09, 0xCF,                                  /*   Usage (Voice Command) */
	0x09, 0xE2,                                  /*   Usage (Mute) */
	0x81, 0x02,                                  /*   Input (Data,Var,Abs) */
	0x95, 0x03,                                  /*   Report Count (3) */
	0x81, 0x03,                                  /*   Input (Cnst,Var,Abs): padding to a byte */
	0xC0                                         /* End Collection */
};
#elif defined(CONFIG_USBD_HID_MOUSE)
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
	0x26,	//bSize: 0x02, bType: Global, bTag: Logical Maximum (2-byte, signed-safe)
	0xFF,	//Logical Maximum low byte
	0x00,	//Logical Maximum high byte => 255
	0x19,	//bSize: 0x01, bType: Local, bTag: Usage Minimum
	0x00,	//Usage Minimum(0x0 )
	0x29,	//bSize: 0x01, bType: Local, bTag: Usage Maximum
	0xFF,	//Usage Maximum(0xFF )
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
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	.ep0_data_out = hid_handle_ep0_data_out,
	.ep_data_out = hid_handle_ep_data_out,
#endif
	.status_changed = hid_status_changed,
};

static usbd_hid_t hid_device;

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Get the Report descriptor of the variant built
  * @param  desc: Receives the Report descriptor
  * @retval Report descriptor length
  */
static u16 usbd_hid_get_report_desc(const u8 **desc)
{
#if defined(CONFIG_USBD_HID_VENDOR)
	*desc = hid_vendor_report_desc;
	return (u16)sizeof(hid_vendor_report_desc);
#elif defined(CONFIG_USBD_HID_HEADSET)
	*desc = hid_headset_report_desc;
	return (u16)sizeof(hid_headset_report_desc);
#elif defined(CONFIG_USBD_HID_MOUSE)
	*desc = hid_mouse_report_desc;
	return (u16)sizeof(hid_mouse_report_desc);
#else
	*desc = hid_keyboard_report_desc;
	return (u16)sizeof(hid_keyboard_report_desc);
#endif
}

#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)

/**
  * @brief  Prepare to receive one INTR OUT packet: the keyboard LED report or the vendor Output report
  * @retval Status
  */
static int usbd_hid_receive(void)
{
	usbd_hid_t *hid = &hid_device;

	return usbd_ep_receive(hid->dev, &hid->ep_intr_out);
}

/**
  * @brief  Handle EP0 Rx Ready event
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @retval Status
  */
static int hid_handle_ep0_data_out(usb_dev_t *dev)
{
	usbd_hid_t *hid = &hid_device;

	UNUSED(dev);

	if (hid->ctrl_req_pending != 0U) {
		/* Consume the pending request first: a single data stage belongs to exactly one setup
		 * packet, so the saved request must not be replayed by a later EP0 OUT event. */
		hid->ctrl_req_pending = 0U;

		/* Deliver SET_REPORT data (e.g., keyboard LED state, or vendor Output report
		 * sent via EP0, Report ID first) to the application. Some hosts send SET_REPORT via the
		 * control endpoint instead of INTR OUT.
		 * cb is released by usbd_hid_deinit(), which may run between the setup and the data
		 * stage of an H2D request, so both the structure and the handler are checked. */
		if ((hid->cb != NULL) && (hid->cb->received != NULL)) {
			hid->cb->received(dev->ep0_out.xfer_buf, hid->ctrl_req.wLength);
		}
	}

	/* No pending request means this data stage does not belong to HID, the composite dispatcher
	   already routed it by active_func. Ref USB 2.0 8.5.3.1: a non-zero value here makes the core
	   stall the status stage, so do not report a failure the host cannot act on. */
	return RTK_SUCCESS;
}

/**
  * @brief  Data received on non-control Out endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @param  len: received data length
  * @retval Status
  */
static int hid_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_hid_t *hid = &hid_device;
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;
	int ret;

	UNUSED(dev);

	/* Return non-zero if EP is not owned by HID, so composite
	 * dispatcher can continue iterating to the correct sub-function. */
	if (ep_addr != hid->cfg->intr_out_addr) {
		return -RTK_ERR_BADARG;
	}

	/* Ref USB 2.0 8.4.6: a zero-length OUT packet is a legal, empty transfer that carries no
	   report, it is only re-armed for */
	if ((hid->cb->received != NULL) && (len > 0U)) {
		hid->cb->received(ep_intr_out->xfer_buf, len);
	}
	/* Single-shot buffer, re-arm immediately: the callback has consumed the report */
	ret = usbd_hid_receive();

	if (ret != RTK_SUCCESS) {
		/* The OUT endpoint stays un-armed and NAKs the host from now on */
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
	}

	return RTK_SUCCESS;
}

#endif // CONFIG_USBD_HID_KEYBOARD || CONFIG_USBD_HID_VENDOR

/**
  * @brief  Get the length of the Input report a GET_REPORT names
  * @param  itf: Class-local interface number, the whole wIndex of the request
  * @param  value: wValue of the request, report type in the high byte and Report ID in the low
  * @retval Report length in bytes, 0 when the interface declares no such Input report
  */
static u16 usbd_hid_get_input_report_len(u16 itf, u16 value)
{
	u16 len = 0U;
	u8 id = USB_LOW_BYTE(value);

	if (USB_HIGH_BYTE(value) != USBD_HID_REPORT_TYPE_INPUT) {
		return 0U;
	}

#if defined(CONFIG_USBD_HID_VENDOR)
	if ((itf == USBD_HID_ITF_NUM) && (id == USBD_HID_VENDOR_IN_REPORT_ID)) {
		len = USBD_HID_INPUT_REPORT_LEN;
	}
#else
	/* No Report ID is declared, so the ID is 0 (Ref HID 1.11 7.2.1) */
	if ((itf == USBD_HID_ITF_NUM) && (id == 0U)) {
		len = USBD_HID_INPUT_REPORT_LEN;
	}
#endif

	return len;
}

/**
  * @brief  Get the length of the Output report a SET_REPORT names
  * @param  itf: Class-local interface number, the whole wIndex of the request
  * @param  value: wValue of the request, report type in the high byte and Report ID in the low
  * @retval Report length in bytes, 0 when the interface declares no such Output report
  */
static u16 usbd_hid_get_output_report_len(u16 itf, u16 value)
{
	u16 len = 0U;
	u8 id = USB_LOW_BYTE(value);

	/* Ref HID 1.11 7.2.2: SET_REPORT may name an Input or Feature report too, but this device
	   gives them no meaning, so only the Output report it declares is accepted */
	if (USB_HIGH_BYTE(value) != USBD_HID_REPORT_TYPE_OUTPUT) {
		return 0U;
	}

#if defined(CONFIG_USBD_HID_VENDOR)
	if ((itf == USBD_HID_ITF_NUM) && (id == USBD_HID_VENDOR_OUT_REPORT_ID)) {
		len = USBD_HID_OUTPUT_REPORT_LEN;
	}
#elif defined(CONFIG_USBD_HID_KEYBOARD)
	/* The LED report, no Report ID is declared so the ID is 0 (Ref HID 1.11 7.2.1) */
	if ((itf == USBD_HID_ITF_NUM) && (id == 0U)) {
		len = USBD_HID_OUTPUT_REPORT_LEN;
	}
#else
	UNUSED(itf);
	UNUSED(id);
#endif

	return len;
}

/**
  * @brief  Handle HID specific CTRL requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int hid_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_hid_t *hid = &hid_device;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	int ret = RTK_SUCCESS;
	u16 len = 0;
	u16 report_len = 0;
	/* Report descriptor tables live in rodata: keep them behind a const pointer so the
	   const is never cast away (MISRA-C:2012 Rule 11.8). buf is only ever the mutable
	   EP0 IN transfer buffer. */
	const u8 *report_desc = NULL;
	u8 *buf = NULL;

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = -RTK_ERR_BADARG;
			} else if (req->wValue != 0U) {
				/* Ref USB 2.0 9.4.10 and Table 9-10: the whole wValue is the alternate setting,
				   and every interface of this function declares setting 0 only, so any other
				   value is a request error. */
				ret = -RTK_ERR_BADARG;
			} else {
				/* Ref USB 2.0 9.4.10: the endpoints of the selected interface return to
				   their default state, not halted and data toggle DATA0. This holds even
				   for an interface with the default setting only, hosts do send the
				   request in that case. Only the endpoints of the interface addressed by
				   wIndex are touched, so the other interface keeps its data toggle.
				   Ref USB 2.0 Table 9-10: the whole wIndex is the interface number, so a
				   foreign interface or a non-zero high byte leaves the endpoints untouched. */
				if (req->wIndex == USBD_HID_ITF_NUM) {
					usbd_ep_clear_stall(dev, &hid->ep_intr_in);
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
					usbd_ep_clear_stall(dev, &hid->ep_intr_out);
#endif
				} else {
					/* Foreign interface: not ours to configure, request error */
					ret = -RTK_ERR_BADARG;
				}
			}
			break;
		case USB_REQ_GET_INTERFACE:
			/* Ref USB 2.0 9.4.4: report the alternate setting of the interface addressed by
			   wIndex; a non-existent interface is a request error. Ref USB 2.0 Table 9-3: wLength
			   is one. Every interface of this function has alternate setting 0 only. */
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = -RTK_ERR_BADARG;
			} else if (req->wLength != 1U) {
				ret = -RTK_ERR_BADARG;
			} else if (req->wIndex != USBD_HID_ITF_NUM) {
				/* Foreign interface: request error */
				ret = -RTK_ERR_BADARG;
			} else {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_len = 1U;
				ret = usbd_ep_transmit(dev, ep0_in);
			}
			break;
		case USB_REQ_GET_STATUS:
			/* Ref USB 2.0 9.4.5 and Table 9-3: an interface-recipient GET_STATUS returns two
			   reserved zero bytes, and a request for a non-existent interface is a request error */
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = -RTK_ERR_BADARG;
			} else if (req->wLength != 2U) {
				ret = -RTK_ERR_BADARG;
			} else if (req->wIndex != USBD_HID_ITF_NUM) {
				/* Foreign interface: request error */
				ret = -RTK_ERR_BADARG;
			} else {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				ret = usbd_ep_transmit(dev, ep0_in);
			}
			break;
		case USB_REQ_GET_DESCRIPTOR:
			/* Ref HID 1.11 7.1.1: wIndex of a HID class descriptor request is the interface
			   number, composite rebases it to the class-local one. Reject a foreign interface */
			if (req->wIndex != USBD_HID_ITF_NUM) {
				ret = -RTK_ERR_BADARG;
				break;
			}
			report_len = usbd_hid_get_report_desc(&report_desc);
			if (USB_HIGH_BYTE(req->wValue) == USBD_HID_REPORT_DESC) {
				/* HID Report Descriptor */
				/* Both descriptors are far shorter than the EP0 buffer, which is at least
				   USBD_MIN_CTRL_BUF_LEN, and usbd_ep_transmit() rejects an oversized EP0 IN */
				ep0_in->xfer_len = MIN(report_len, req->wLength);
				usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)report_desc, ep0_in->xfer_len);
			} else if (USB_HIGH_BYTE(req->wValue) == USBD_HID_DESC) {
				/* HID Descriptor */
				len = USBD_HID_DESC_SIZE;
				buf = ep0_in->xfer_buf;
				usb_os_memcpy((void *)buf, (const void *)usbd_hid_desc, len);
				buf[USBD_HID_DESC_ITEM_LENGTH_OFFSET] = USB_LOW_BYTE(report_len);
				buf[USBD_HID_DESC_ITEM_LENGTH_OFFSET + 1] = USB_HIGH_BYTE(report_len);
				ep0_in->xfer_len = MIN(len, req->wLength);
			} else {
				ret = -RTK_ERR_BADARG;
				break;
			}

			ret = usbd_ep_transmit(dev, ep0_in);
			if ((ret == RTK_SUCCESS) && (hid->cb->setup != NULL)) {
				hid->cb->setup();
			}
			break;
		default:
			ret = -RTK_ERR_BADARG;
			break;
		}
		break;

	case USB_REQ_TYPE_CLASS :
		/* All HID class requests (GET/SET_REPORT, GET/SET_IDLE, GET/SET_PROTOCOL)
		 * are interface-recipient. In a composite device the framework offers
		 * endpoint- and device-recipient class requests to every sub-function via
		 * a first-accept loop; e.g. UAC1 sampling-freq SET_CUR is endpoint-recipient
		 * and its bRequest (0x01) aliases HID GET_REPORT. Without this guard HID
		 * would wrongly claim and answer another function's request. Only handle
		 * requests addressed to a HID interface. */
		if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) != USB_REQ_RECIPIENT_INTERFACE) {
			ret = -RTK_ERR_BADARG;
			break;
		}
		/* Ref HID 1.11 7.2: wIndex of every HID class request is the interface number. */
		if (req->wIndex != USBD_HID_ITF_NUM) {
			ret = -RTK_ERR_BADARG;
			break;
		}
		switch (req->bRequest) {
		case USBD_HID_SET_PROTOCOL:
			/* Ref HID 1.11 7.2.6: Protocol is Boot(0) or Report(1) only, and the request
			 * carries no data stage (wLength == 0). Any other value/length is a request
			 * error, so reject it and let the core stall EP0 (USB 2.0 9.2.7) rather than
			 * storing and later echoing a protocol value outside the valid set. */
			if ((USB_LOW_BYTE(req->wValue) > 1U) || (req->wLength != 0U)) {
				ret = -RTK_ERR_BADARG;
				break;
			}
			hid->protocol = USB_LOW_BYTE(req->wValue);
			break;
		case USBD_HID_GET_PROTOCOL:
			/* Ref HID 1.11 7.2.5: wValue 0, a one-byte data stage */
			if ((req->wValue != 0U) || (req->wLength != 1U) || ((req->bmRequestType & USB_REQ_DIR_MASK) != USB_D2H)) {
				ret = -RTK_ERR_BADARG;
				break;
			}
			ep0_in->xfer_buf[0] = hid->protocol;
			ep0_in->xfer_len = 1U;
			ret = usbd_ep_transmit(dev, ep0_in);
			break;

		case USBD_HID_GET_REPORT:
			/* Ref HID 1.11 7.2.1: wValue carries the report type and ID. Only the Input report
			   each interface declares exists here, so any other type or ID is a request error */
			report_len = usbd_hid_get_input_report_len(req->wIndex, req->wValue);
			if ((report_len == 0U) || (req->wLength == 0U) || ((req->bmRequestType & USB_REQ_DIR_MASK) != USB_D2H)) {
				ret = -RTK_ERR_BADARG;
				break;
			}
			/* send an idle report, the Report ID first where the interface declares one */
			ep0_in->xfer_len = MIN(MIN(req->wLength, report_len), ep0_in->xfer_buf_len);
			usb_os_memset((void *)ep0_in->xfer_buf, 0x0, ep0_in->xfer_len);
			if (USB_LOW_BYTE(req->wValue) != 0U) {
				ep0_in->xfer_buf[0] = USB_LOW_BYTE(req->wValue);
			}
			ret = usbd_ep_transmit(dev, ep0_in);
			break;
		case USBD_HID_SET_REPORT:
			/* Ref HID 1.11 7.2.2: wValue carries the report type and ID. Only the Output report
			   each interface declares exists here, so any other type or ID is a request error.
			   A shorter report is accepted, hosts such as Linux hidraw may send one */
			report_len = usbd_hid_get_output_report_len(req->wIndex, req->wValue);
			if ((report_len != 0U) && (req->wLength != 0U) && (req->wLength <= report_len) &&
				((req->bmRequestType & USB_REQ_DIR_MASK) == USB_H2D)) {
				if (req->wLength > ep0_out->xfer_buf_len) {
					/* Cannot accept the data stage: stall so the host recovers promptly */
					ret = -RTK_ERR_BADARG;
					break;
				}
				/* Ref USB 2.0 8.5.3: an H2D control transfer with wLength > 0 carries the
				 * payload in a following data stage, the request cannot be dispatched yet. */
				usb_os_memcpy((void *)&hid->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
				hid->ctrl_req_pending = 1U;
				/* The EP0 buffer is shared with every control transfer: clear the window, so a
				   data stage that ends short leaves zeros, not an earlier transfer, behind */
				usb_os_memset((void *)ep0_out->xfer_buf, 0, req->wLength);
				ep0_out->xfer_len = req->wLength;
				ret = usbd_ep_receive(dev, ep0_out);
				if (ret != RTK_SUCCESS) {
					/* The data stage never started, so no EP0 OUT completion will arrive to
					 * consume the stashed request. Drop it, else the next unrelated request's
					 * data stage would be delivered as this one's payload. */
					hid->ctrl_req_pending = 0U;
				}
			} else {
				/* Ref HID 1.11 7.2.2: SET_REPORT is host-to-device with a report in the data
				 * stage. A D2H direction bit, wLength==0, a longer report or a report this
				 * interface does not declare is malformed, so report a request error and let
				 * the core stall EP0 (ref USB 2.0 9.2.7) instead of leaving it unanswered. */
				ret = -RTK_ERR_BADARG;
			}
			break;

		case USBD_HID_SET_IDLE:
			/* Ref HID 1.11 7.2.4: no data stage. The low byte of wValue names the report the
			   rate applies to, 0 meaning all of them; a single rate is kept for every report */
			if ((req->wLength != 0U) || ((req->bmRequestType & USB_REQ_DIR_MASK) != USB_H2D)) {
				ret = -RTK_ERR_BADARG;
				break;
			}
			hid->idle_rate = USB_HIGH_BYTE(req->wValue);
			/* HID 1.11 7.2.4 idle semantics are delivered by the application layer, not
			 * by this driver: each report here is pushed on every usbd_hid_send_data()
			 * call. The app owns the send cadence and knows whether the report actually
			 * changed, so it suppresses unchanged reports at the requested idle_rate
			 * itself. This driver only stores/echoes idle_rate for host compliance. */
			break;
		case USBD_HID_GET_IDLE:
			/* Ref HID 1.11 7.2.3: a one-byte data stage, wValue high byte reserved as 0 */
			if ((USB_HIGH_BYTE(req->wValue) != 0U) || (req->wLength != 1U) || ((req->bmRequestType & USB_REQ_DIR_MASK) != USB_D2H)) {
				ret = -RTK_ERR_BADARG;
				break;
			}
			ep0_in->xfer_buf[0] = hid->idle_rate;
			ep0_in->xfer_len = 1U;
			ret = usbd_ep_transmit(dev, ep0_in);
			break;
		default:
			ret = -RTK_ERR_BADARG;
			break;
		}
		break;
	default:
		ret = -RTK_ERR_BADARG;
		break;
	}

	return ret;
}

/**
  * @brief  Set HID class configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int hid_set_config(usb_dev_t *dev, u8 config)
{
	int ret = RTK_SUCCESS;
	usbd_hid_t *hid = &hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;
#endif
	usb_ep_info_t *info;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if (config != 1U) {
		return -RTK_ERR_BADARG;
	}

	hid->dev = dev;

	/* Init INTR IN EP */
	info = &ep_intr_in->info;
	ep_intr_in->xfer_state = 0;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_HID_HS_INT_MAX_PACKET_SIZE : USBD_HID_FS_INT_MAX_PACKET_SIZE;
	ret = usbd_ep_init(dev, ep_intr_in);
	if (ret != RTK_SUCCESS) {
		goto exit_clear_config;
	}

#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	/* Init INTR OUT EP */
	info = &ep_intr_out->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_HID_HS_INT_MAX_PACKET_SIZE : USBD_HID_FS_INT_MAX_PACKET_SIZE;
	/* The controller receives whole DWORD aligned packets: request no more than the buffer holds
	   at this MPS, so that usbd_ep_receive() accepts it. usbd_hid_private_init() made sure the
	   buffer holds one packet at least. */
	ep_intr_out->xfer_len = usb_get_max_payload_len(ep_intr_out->xfer_buf_len, info->mps);
	ret = usbd_ep_init(dev, ep_intr_out);
	if (ret != RTK_SUCCESS) {
		goto exit_clear_config;
	}

	/* Prepare to receive next INTR OUT packet */
	ret = usbd_hid_receive();
	if (ret != RTK_SUCCESS) {
		goto exit_clear_config;
	}
#endif

	return RTK_SUCCESS;

exit_clear_config:
	/* Release the endpoints initialized above */
	hid_clear_config(dev, config);
	return ret;
}

/**
  * @brief  Clear HID class configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval None
  */
static void hid_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_hid_t *hid = &hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;
#endif

	UNUSED(config);

	/* DeInit INTR IN EP */
	usbd_ep_deinit(dev, ep_intr_in);

#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	usbd_ep_deinit(dev, ep_intr_out);
#endif

	/* Ref HID 1.11 7.2.6: the protocol returns to Report after a reset, and the idle rate and a
	   control write waiting for its data stage belong to the configuration that is gone */
	hid->protocol = 1U;
	hid->idle_rate = 0U;
	hid->ctrl_req_pending = 0U;
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @param  status: transfer status
  * @retval Status
  */
static int hid_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, int status)
{
	usbd_hid_t *hid = &hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;

	UNUSED(dev);

	/* Check EP ownership before diagnosing, so a foreign EP (owned by another
	 * composite sub-function) never produces a misattributed USB_DIAG entry. */
	if (ep_addr == hid->cfg->intr_in_addr) {
		if (status != RTK_SUCCESS) {
			USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
		}
		ep_intr_in->xfer_state = 0U;
		if (hid->cb->transmitted != NULL) {
			hid->cb->transmitted(status);
		}
		return RTK_SUCCESS;
	}

	/* Return non-zero if EP is not owned by HID, so composite
	 * dispatcher can continue iterating to the correct sub-function. */
	return -RTK_ERR_BADARG;
}

/**
  * @brief  Patch endpoint addresses in a configuration descriptor to use runtime EP config
  * @param  desc: Pointer to the descriptor buffer
  * @param  len: Total length of descriptor data
  * @param  speed: Speed the descriptor describes, selects the INTR bInterval
  * @retval void
  */
static void usbd_hid_patch_desc(u8 *desc, u16 len, usb_speed_type_t speed)
{
	usbd_hid_t *hid = &hid_device;
	const usbd_hid_config_t *cfg = hid->cfg;
	u16 i;
	u16 mps = (speed == USB_SPEED_HIGH) ? USBD_HID_HS_INT_MAX_PACKET_SIZE : USBD_HID_FS_INT_MAX_PACKET_SIZE;
	u8 interval;

	interval = (speed == USB_SPEED_HIGH) ? cfg->intr_interval_hs : cfg->intr_interval_fs;
	if (interval == 0U) {
		interval = (speed == USB_SPEED_HIGH) ? USBD_HID_DEFAULT_HS_INTR_INTERVAL : USBD_HID_DEFAULT_FS_INTR_INTERVAL;
	}

	for (i = 0; i < len;) {
		u8 dlen;
		u8 dtype;

		/* Read a header only when it fits, and patch a descriptor only when its bLength fits
		   what is left of the block */
		if (((u32)len - i) < USB_LEN_DESC_HEADER) {
			break;
		}
		dlen = desc[i];
		dtype = desc[i + 1U];
		if ((dlen < USB_LEN_DESC_HEADER) || ((u32)dlen > ((u32)len - i))) {
			break;
		}
		if ((dtype == USB_DESC_TYPE_ENDPOINT) && (dlen >= USB_LEN_EP_DESC)) {
			u8 addr = desc[i + 2];
			u8 dir = addr & USB_REQ_DIR_MASK;
			u8 type = desc[i + 3] & USB_EP_XFER_TYPE_MASK;
			if ((dir == USB_D2H) && (type == USB_CH_EP_TYPE_INTR)) {
				desc[i + 2] = cfg->intr_in_addr;
			}
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
			else if ((dir == USB_H2D) && (type == USB_CH_EP_TYPE_INTR)) {
				desc[i + 2] = cfg->intr_out_addr;
			}
#endif
			if (type == USB_CH_EP_TYPE_INTR) {
				desc[i + 4] = USB_LOW_BYTE(mps);    /* wMaxPacketSize */
				desc[i + 5] = USB_HIGH_BYTE(mps);
				desc[i + 6] = interval;             /* bInterval */
			}
		} else if ((dtype == USB_DESC_TYPE_INTERFACE) && (dlen >= USB_LEN_IF_DESC)) {
			desc[i + 8] = hid->itf_str_idx;         /* iInterface */
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
  * @retval Status
  */
static u16 hid_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len)
{
	const u8 *desc = NULL;
	usb_speed_type_t speed = dev->dev_speed;
	/* Speed the returned configuration describes, which selects the bInterval encoding */
	usb_speed_type_t desc_speed = speed;
	u16 len = 0;
	const u8 *report_desc = NULL;
	u16 report_len = usbd_hid_get_report_desc(&report_desc);
	u8 type = USB_HIGH_BYTE(req->wValue);
	u8 is_cfg = 0;

	switch (type) {

	case USB_DESC_TYPE_DEVICE:
		desc = usbd_hid_dev_desc;
		len = sizeof(usbd_hid_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		desc = usbd_hid_config_desc;
		len = sizeof(usbd_hid_config_desc);
		is_cfg = 1;
		break;

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		desc = usbd_hid_device_qualifier_desc;
		len = sizeof(usbd_hid_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		/* Ref USB 2.0 9.6.4: describes the configuration at the speed the device is not
		   running at, so its bInterval is patched for that speed, not the current one */
		desc = usbd_hid_config_desc;
		len = sizeof(usbd_hid_config_desc);
		desc_speed = (speed == USB_SPEED_HIGH) ? USB_SPEED_FULL : USB_SPEED_HIGH;
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
		buf[USBD_HID_CFG_DESC_ITEM_LENGTH_OFFSET] = USB_LOW_BYTE(report_len);
		buf[USBD_HID_CFG_DESC_ITEM_LENGTH_OFFSET + 1] = USB_HIGH_BYTE(report_len);

		usbd_hid_patch_desc(buf + USB_LEN_CFG_DESC, len - USB_LEN_CFG_DESC, desc_speed);
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
static void hid_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_hid_t *hid = &hid_device;

	UNUSED(dev);

	if (hid->cb->status_changed != NULL) {
		hid->cb->status_changed(old_status, status);
	}
}

static int usbd_hid_private_init(const usbd_hid_usr_cb_t *cb, const usbd_hid_config_t *cfg)
{
	int ret = RTK_SUCCESS;
	usbd_hid_t *hid = &hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;
	usb_ep_info_t *info;
	u32 tx_buf_len;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return -RTK_ERR_BADARG;
	}

	if (cfg == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cfg\n");
		return -RTK_ERR_BADARG;
	}

	/* The INTR IN endpoint is mandatory for every HID variant, and a wrong direction bit would
	   make the class claim an endpoint the controller drives the other way round. */
	if (((cfg->intr_in_addr & USB_REQ_DIR_MASK) != USB_D2H) ||
		((cfg->intr_in_addr & (~USB_REQ_DIR_MASK)) == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid INTR IN addr %02x\n", cfg->intr_in_addr);
		return -RTK_ERR_BADARG;
	}
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	if (((cfg->intr_out_addr & USB_REQ_DIR_MASK) != USB_H2D) ||
		((cfg->intr_out_addr & (~USB_REQ_DIR_MASK)) == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid INTR OUT addr %02x\n", cfg->intr_out_addr);
		return -RTK_ERR_BADARG;
	}
#endif
	/* Published before anything that reads it: the descriptor patch path and every endpoint
	   setup below resolve their options through hid->cfg. */
	hid->cfg = cfg;

	/* The core owns the string table, so the same call works in standalone and composite
	   mode. 0 means no iInterface string. */
	hid->itf_str_idx = usbd_add_string(cfg->itf_str);

	tx_buf_len = cfg->intr_in_xfer_size;
	if (tx_buf_len == 0) {
		tx_buf_len = USBD_HID_INTR_IN_BUF_SIZE;
	}

#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;
	info = &ep_intr_out->info;
	info->addr = cfg->intr_out_addr;
	info->type = USB_CH_EP_TYPE_INTR;
	ep_intr_out->xfer_buf_len = (cfg->intr_out_xfer_size != 0U) ? cfg->intr_out_xfer_size : USBD_HID_INTR_OUT_BUF_SIZE;
	/* The INTR OUT buffer receives whole packets, so it shall hold one at every speed, otherwise
	   SET_CONFIGURATION fails, refer to hid_set_config() */
	if ((usb_get_max_payload_len(ep_intr_out->xfer_buf_len, USBD_HID_HS_INT_MAX_PACKET_SIZE) == 0U) ||
		(usb_get_max_payload_len(ep_intr_out->xfer_buf_len, USBD_HID_FS_INT_MAX_PACKET_SIZE) == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "OUT buf %u < mps\n", ep_intr_out->xfer_buf_len);
		return -RTK_ERR_BADARG;
	}
	ep_intr_out->xfer_buf = (u8 *)usb_os_malloc(ep_intr_out->xfer_buf_len);
	if (ep_intr_out->xfer_buf == NULL) {
		return -RTK_ERR_NOMEM;
	}
#endif

	info = &ep_intr_in->info;
	info->addr = cfg->intr_in_addr;
	info->type = USB_CH_EP_TYPE_INTR;
	ep_intr_in->xfer_buf_len = tx_buf_len;
	ep_intr_in->xfer_buf = (u8 *)usb_os_malloc(tx_buf_len);
	if (ep_intr_in->xfer_buf == NULL) {
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
		usb_os_mfree(ep_intr_out->xfer_buf);
		ep_intr_out->xfer_buf = NULL;
#endif
		return -RTK_ERR_NOMEM;
	}

	/* No H2D class request is waiting for its data stage yet. The device context is a static
	 * object, so a re-init after deinit must not inherit a stale pending flag. */
	hid->ctrl_req_pending = 0U;

	hid->cb = cb;
	hid->protocol = 1U; /* HID 1.11 7.2.6: default to Report Protocol after enumeration */
	if (cb->init != NULL) {
		cb->init();
	}

	return ret;
}

/* Exported functions --------------------------------------------------------*/

#ifdef CONFIG_USBD_COMPOSITE
int usbd_composite_hid_init(const usbd_hid_usr_cb_t *cb, const usbd_hid_config_t *cfg)
{
	usbd_hid_t *hid = &hid_device;
	int ret;

	ret = usbd_hid_private_init(cb, cfg);
	if (ret == RTK_SUCCESS) {
		hid->from_composite = 1;
		ret = usbd_composite_register_driver(&usbd_hid_driver);
		if (ret != RTK_SUCCESS) {
			/* private_init completed, so deinit is its exact reverse. from_composite is
			   already 1, so the unregister inside is a no-op for an unregistered driver. */
			usbd_hid_deinit();
		}
	}
	return ret;
}
#endif

int usbd_hid_init(const usbd_hid_usr_cb_t *cb, const usbd_hid_config_t *cfg)
{
	usbd_hid_t *hid = &hid_device;
	int ret;

	ret = usbd_hid_private_init(cb, cfg);
	if (ret == RTK_SUCCESS) {
		hid->from_composite = 0;
		ret = usbd_register_class(&usbd_hid_driver);
		if (ret != RTK_SUCCESS) {
			/* private_init completed, so deinit is its exact reverse */
			usbd_hid_deinit();
		}
	}
	return ret;
}
void usbd_hid_deinit(void)
{
	usbd_hid_t *hid = &hid_device;

	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;
#endif
	int ret;

	/* Leave the core before anything is torn down. Composite: refused until
	   usbd_composite_deinit() has blocked the ISR and cleared the configuration of every
	   function. Standalone: refused in interrupt context. Either way nothing is released when
	   refused, and once accepted no class callback runs any more and every endpoint of this
	   class is deinitialized: ref USB 2.0 9.4.7, the transfers still pending are aborted. */
#ifdef CONFIG_USBD_COMPOSITE
	if (hid->from_composite != 0U) {
		ret = usbd_composite_unregister_driver(&usbd_hid_driver);
	} else
#endif
	{
		ret = usbd_unregister_class();
	}
	if (ret != RTK_SUCCESS) {
		return;
	}

	/* No completion arrives any more to clear them */
	ep_intr_in->xfer_state = 0U;

	/* Unregistered above: no class callback can run afterwards, so dropping the pending
	 * control request here cannot race an EP0 OUT completion in ISR context. */
	hid->ctrl_req_pending = 0U;

	if ((hid->cb != NULL) && (hid->cb->deinit != NULL)) {
		hid->cb->deinit();
	}

	usb_os_mfree((void *)ep_intr_in->xfer_buf);
	ep_intr_in->xfer_buf = NULL;
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	usb_os_mfree((void *)ep_intr_out->xfer_buf);
	ep_intr_out->xfer_buf = NULL;
#endif
}
int usbd_hid_send_data(const u8 *data, u32 len)
{
	int ret = -RTK_ERR_IO;
	usbd_hid_t *hid = &hid_device;
	usb_dev_t *dev = hid->dev;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;
	u8 claimed;

	/* hid->dev is assigned in set_config, so it is NULL until the host has configured the device */
	if ((dev == NULL) || (dev->is_ready == 0U)) {
		/* hid->cfg is NULL before the first init: log the endpoint copy */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x TX not ready\n", ep_intr_in->info.addr);
		return ret;
	}

	if (len > ep_intr_in->xfer_buf_len) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "len %u > buf %u, truncated\n", len, ep_intr_in->xfer_buf_len);
		len = ep_intr_in->xfer_buf_len;
	}

	/* Claim the endpoint atomically: a task on another core, or a class callback in ISR context,
	   may test xfer_state at the same time, and both would otherwise fill the same buffer */
	usb_os_enter_critical();
	claimed = (ep_intr_in->xfer_state == 0U) ? 1U : 0U;
	ep_intr_in->xfer_state = 1U;
	usb_os_exit_critical();

	if (claimed == 0U) {
		return -RTK_ERR_BUSY;
	}

	usb_os_memcpy((void *)ep_intr_in->xfer_buf, (const void *)data, len);
	ep_intr_in->xfer_len = len;

	/* A disconnect after the is_ready check is caught by the core: the endpoint is closed and
	   the transmit fails, so the rollback below gives the endpoint back */
	ret = usbd_ep_transmit(hid->dev, ep_intr_in);
	if (ret != RTK_SUCCESS) {
		/* The transfer was not armed, so no ep_data_in completion will ever release
		   the EP: release it here or every later report stays -RTK_ERR_BUSY */
		ep_intr_in->xfer_state = 0U;
	}

	return ret;
}
