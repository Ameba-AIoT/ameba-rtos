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

/* Interface number, the second interface exists in the BIDIR mode only */
#define USBD_HID_PRIV_ITF_NUM                0x00U /**< Priv/data interface. */
#define USBD_HID_VEND_ITF_NUM                0x01U /**< Vend/Consumer Control interface. */

/* String index */
#define USBD_HID_STRING_MANUFACTURER         1 /**< Manufacture string index. */
#define USBD_HID_STRING_PRODUCT              2 /**< Product string index. */
#define USBD_HID_STRING_SERIALNUMBER         3 /**< Serial number string index. */

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
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

/* Private function prototypes -----------------------------------------------*/

static int hid_setup(usb_dev_t *dev, usb_setup_req_t *req);
static int hid_set_config(usb_dev_t *dev, u8 config);
static void hid_clear_config(usb_dev_t *dev, u8 config);
static int hid_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static u16 hid_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
static int hid_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static int hid_handle_ep0_data_out(usb_dev_t *dev);
#endif
#ifdef CONFIG_USBD_HID_BIDIR
static void hid_sof(usb_dev_t *dev);
static int usbd_hid_ring_buf_ctrl_init(usbd_hid_buf_ctrl_t *pbuf_ctrl);
static void usbd_hid_ring_buf_ctrl_deinit(usbd_hid_buf_ctrl_t *pbuf_ctrl);
static u32 usbd_hid_ring_buf_is_full_internal(const usbd_hid_buf_ctrl_t *pdata_ctrl);
static u32 usbd_hid_ring_buf_cnt_internal(const usbd_hid_buf_ctrl_t *pdata_ctrl);
#endif
static void hid_status_changed(usb_dev_t *dev, u8 old_status, u8 status);

#ifdef CONFIG_USBD_HID_BIDIR
/* Consumer Control command bits carried in the Volume/Power report payload. */
#define USBD_HID_CMD_VOLUME_UP                            BIT0
#define USBD_HID_CMD_VOLUME_DOWN                          BIT1
#define USBD_HID_CMD_POWER                                BIT2
#endif

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

#ifndef CONFIG_USB_FS
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

/* USB HID device FS Configuration Descriptor */
static const u8 usbd_hid_fs_config_desc[] = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,								/*bLength*/
	USB_DESC_TYPE_CONFIGURATION, 					/*bDescriptorType: Configuration*/
	0x00,											/*wTotalLength, runtime assigned*/
	0x00,
#ifdef CONFIG_USBD_HID_BIDIR
	0x02,											/*bNumInterfaces*/
#else
	0x01,											/*bNumInterfaces*/
#endif
	0x01,         									/*bConfigurationValue*/
	0x00,        									/*iConfiguration*/
	0x80,         									/* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,         									/*MaxPower 100 mA*/

	/* HID Interface Descriptor*/
	0x09,       									/*bLength*/
	USB_DESC_TYPE_INTERFACE,						/*bDescriptorType*/
	0x00,											/*bInterfaceNumber*/
	0x00,											/*bAlternateSetting*/
#ifdef CONFIG_USBD_HID_MOUSE
	0x01,											/*bNumEndpoints*/
#elif defined(CONFIG_USBD_HID_KEYBOARD)
	0x02,											/*bNumEndpoints*/
#else /* CONFIG_USBD_HID_BIDIR: priv interface = 2 EPs (IN + OUT) */
	0x02,											/*bNumEndpoints*/
#endif
	0x03,											/*bInterfaceClass: HID*/
#ifdef CONFIG_USBD_HID_BIDIR
	0x00,											/*bInterfaceSubClass: 0=no boot (vendor raw)*/
	0x00,											/*bInterfaceProtocol: 0=none*/
#else
	0x01,											/*bInterfaceSubClass: 1=BOOT*/
#ifdef CONFIG_USBD_HID_MOUSE
	USB_HID_PROTOCOL_MOUSE,							/*bInterfaceProtocol: 2=mouse*/
#else
	USB_HID_PROTOCOL_KEYBOARD,						/*bInterfaceProtocol: 1=keyboard*/
#endif
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
	USB_D2H,								/*bEndpointAddress*/
	0x03,											/*bmAttributes*/
	USB_LOW_BYTE(USBD_HID_FS_INT_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_HID_FS_INT_MAX_PACKET_SIZE),
#ifdef CONFIG_USBD_HID_BIDIR
	0x01,											/*bInterval: 1 ms (bidir raw)*/
#else
	0xA,											/*bInterval*/
#endif

#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
	/* Endpoint Descriptor*/
	0x07,											/*bLength*/
	USB_DESC_TYPE_ENDPOINT,							/*bDescriptorType:*/
	USB_H2D,								/*bEndpointAddress*/
	0x03,											/*bmAttributes*/
	USB_LOW_BYTE(USBD_HID_FS_INT_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_HID_FS_INT_MAX_PACKET_SIZE),
#ifdef CONFIG_USBD_HID_BIDIR
	0x01,											/*bInterval: 1 ms*/
#else
	0xA,											/*bInterval*/
#endif
#endif

#ifdef CONFIG_USBD_HID_BIDIR
	/* Consumer Control interface: 1 EP (Consumer INTR IN) */
	0x09,											/*bLength*/
	USB_DESC_TYPE_INTERFACE,						/*bDescriptorType*/
	0x01,											/*bInterfaceNumber*/
	0x00,											/*bAlternateSetting*/
	0x01,											/*bNumEndpoints*/
	0x03,											/*bInterfaceClass: HID*/
	0x00,											/*bInterfaceSubClass: 0=no boot*/
	0x00,											/*bInterfaceProtocol: 0=none*/
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

	/* Endpoint Descriptor (Consumer INTR IN) */
	0x07,											/*bLength*/
	USB_DESC_TYPE_ENDPOINT,							/*bDescriptorType:*/
	USB_D2H,										/*bEndpointAddress*/
	0x03,											/*bmAttributes*/
	USB_LOW_BYTE(USBD_HID_FS_INT_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_HID_FS_INT_MAX_PACKET_SIZE),
	0x01,											/*bInterval*/
#endif
};

#ifndef CONFIG_USB_FS
/* USB HID device HS Configuration Descriptor */
static const u8 usbd_hid_hs_config_desc[] = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,								/* bLength*/
	USB_DESC_TYPE_CONFIGURATION,					/* bDescriptorType*/
	0x00,											/* wTotalLength, runtime assigned*/
	0x00,
#ifdef CONFIG_USBD_HID_BIDIR
	0x02,											/*bNumInterfaces*/
#else
	0x01,											/*bNumInterfaces*/
#endif
	0x01,											/*bConfigurationValue*/
	0x00,											/*iConfiguration*/
	0x80,											/* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,											/*MaxPower*/

	/* HID Interface Descriptor*/
	0x09,											/*bLength*/
	USB_DESC_TYPE_INTERFACE,						/*bDescriptorType*/
	0x00,											/*bInterfaceNumber*/
	0x00,											/*bAlternateSetting*/
#ifdef CONFIG_USBD_HID_MOUSE
	0x01,											/*bNumEndpoints*/
#else /* keyboard or bidir: 2 EPs on the priv interface */
	0x02,											/*bNumEndpoints*/
#endif
	0x03,											/*bInterfaceClass*/
#ifdef CONFIG_USBD_HID_BIDIR
	0x00,											/*bInterfaceSubClass: 0=no boot*/
	0x00,											/*bInterfaceProtocol: 0=none*/
#else
	0x01,											/*bInterfaceSubClass: 1=BOOT*/
#ifdef CONFIG_USBD_HID_MOUSE
	USB_HID_PROTOCOL_MOUSE,							/*bInterfaceProtocol*/
#else
	USB_HID_PROTOCOL_KEYBOARD,						/*bInterfaceProtocol*/
#endif
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
	USB_D2H,								/*bEndpointAddress*/
	0x03,											/*bmAttributest*/
	USB_LOW_BYTE(USBD_HID_HS_INT_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_HID_HS_INT_MAX_PACKET_SIZE),
	0x04,											/*bInterval: HS 2^(4-1)=8 microframes = 1 ms*/

#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
	/* Endpoint Descriptor*/
	0x07,											/*bLength*/
	USB_DESC_TYPE_ENDPOINT,							/*bDescriptorType:*/
	USB_H2D,								/*bEndpointAddress*/
	0x03,											/*bmAttributes*/
	USB_LOW_BYTE(USBD_HID_HS_INT_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_HID_HS_INT_MAX_PACKET_SIZE),
	0x04,											/*bInterval*/
#endif

#ifdef CONFIG_USBD_HID_BIDIR
	/* Consumer Control interface: 1 EP (Consumer INTR IN) */
	0x09,											/*bLength*/
	USB_DESC_TYPE_INTERFACE,						/*bDescriptorType*/
	0x01,											/*bInterfaceNumber*/
	0x00,											/*bAlternateSetting*/
	0x01,											/*bNumEndpoints*/
	0x03,											/*bInterfaceClass: HID*/
	0x00,											/*bInterfaceSubClass: 0=no boot*/
	0x00,											/*bInterfaceProtocol: 0=none*/
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

	/* Endpoint Descriptor (Consumer INTR IN) */
	0x07,											/*bLength*/
	USB_DESC_TYPE_ENDPOINT,							/*bDescriptorType:*/
	USB_D2H,										/*bEndpointAddress*/
	0x03,											/*bmAttributes*/
	USB_LOW_BYTE(USBD_HID_HS_INT_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_HID_HS_INT_MAX_PACKET_SIZE),
	0x04,											/*bInterval*/
#endif
};
#endif

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

#if defined(CONFIG_USBD_HID_BIDIR)
/* Priv (vendor raw IN/OUT) Report Descriptor */
static const u8 hid_priv_report_desc[] = {
	0x05, 0x0C,                                  /* Usage Page (Consumer Devices) */
	0x09, 0x01,                                  /* Usage (Consumer Control) */
	0xA1, 0x01,                                  /* COLLECTION (Application) */

	0x85, USBD_HID_REPORT_ID,                    /* Report ID */
	0x09, 0x01,                                  /* Vendor Usage */
	0x15, 0x00,                                  /* Usage Minimum */
	0x25, 0xFF,                                  /* Usage Maximum */
	0x75, 0x08,                                  /* Report Size */
	0x95, USBD_HID_TX_PKT_SIZE,                  /* Report Count */
	0x81, 0x02,                                  /* Input (Data,Var,Abs) */

	0x85, USBD_HID_REPORT_ID,                    /* Report ID */
	0x09, 0x02,                                  /* Vendor Usage */
	0x15, 0x00,                                  /* Usage Minimum */
	0x25, 0xFF,                                  /* Usage Maximum */
	0x75, 0x08,                                  /* Report Size */
	0x95, USBD_HID_RX_PKT_SIZE,                  /* Report Count */
	0x91, 0x02,                                  /* Output (Data,Var,Abs) */

	0xC0                                         /* END_COLLECTION */
};

/* Consumer Control (Volume Up/Down, Power) Report Descriptor */
static const u8 hid_vend_report_desc[] = {
	0x05, 0x0C,                                  /* Usage Page (Consumer Devices) */
	0x09, 0x01,                                  /* Usage (Consumer Control) */
	0xA1, 0x01,                                  /* COLLECTION (Application) */

	0x85, USBD_HID_VOLUME_REPORT_ID,             /* Report ID */
	0x09, 0xE9,                                  /* Usage (Volume Up) */
	0x09, 0xEA,                                  /* Usage (Volume Down) */
	0x09, 0x30,                                  /* Usage (Power) */
	0x15, 0x00,                                  /* Logical Minimum (0) */
	0x25, 0x01,                                  /* Logical Maximum (1) */
	0x75, 0x01,                                  /* Report Size (1 bit) */
	0x95, 0x03,                                  /* Report Count (3) */
	0x81, 0x02,                                  /* Input (Data, Var, Abs) */
	0x75, 0x01,                                  /* Report Size (1 bit) */
	0x95, 0x05,                                  /* Report Count (5, padding) */
	0x81, 0x03,                                  /* Input (Cnst, Var, Abs) */

	0xC0                                         /* END_COLLECTION */
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
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
	.ep0_data_out = hid_handle_ep0_data_out,
	.ep_data_out = hid_handle_ep_data_out,
#endif
#ifdef CONFIG_USBD_HID_BIDIR
	.sof = hid_sof,
#endif
	.status_changed = hid_status_changed,
};

static usbd_hid_t hid_device;

/* Private functions ---------------------------------------------------------*/

#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)

#ifdef CONFIG_USBD_HID_BIDIR
static u32 usbd_hid_ring_buf_is_full_internal(const usbd_hid_buf_ctrl_t *pdata_ctrl)
{
	return (pdata_ctrl->write_idx + 1) % (pdata_ctrl->buf_array_cnt) == pdata_ctrl->read_idx;
}

static u32 usbd_hid_ring_buf_cnt_internal(const usbd_hid_buf_ctrl_t *pdata_ctrl)
{
	return (u32)(pdata_ctrl->write_idx + pdata_ctrl->buf_array_cnt - pdata_ctrl->read_idx) %
		   pdata_ctrl->buf_array_cnt;
}

static int usbd_hid_ring_buf_ctrl_init(usbd_hid_buf_ctrl_t *pbuf_ctrl)
{
	u16 i;

	pbuf_ctrl->hid_mps = USBD_HID_MAX_BUF_SIZE;
	pbuf_ctrl->buf_array_cnt = USBD_HID_BUF_MAX_CNT;
	pbuf_ctrl->read_idx = 0U;
	pbuf_ctrl->write_idx = 0U;
	pbuf_ctrl->is_intr_out_busy = 0U;
	pbuf_ctrl->read_wait_sema = 0U;
	pbuf_ctrl->hid_sema_valid = 0U;

	pbuf_ctrl->hid_buf = (u8 *)usb_os_malloc((u32)pbuf_ctrl->buf_array_cnt * pbuf_ctrl->hid_mps);
	if (pbuf_ctrl->hid_buf == NULL) {
		return HAL_ERR_MEM;
	}

	pbuf_ctrl->buf_array = (usbd_hid_buf_t *)usb_os_malloc((u32)pbuf_ctrl->buf_array_cnt * sizeof(usbd_hid_buf_t));
	if (pbuf_ctrl->buf_array == NULL) {
		usb_os_mfree(pbuf_ctrl->hid_buf);
		pbuf_ctrl->hid_buf = NULL;
		return HAL_ERR_MEM;
	}

	for (i = 0U; i < pbuf_ctrl->buf_array_cnt; i++) {
		pbuf_ctrl->buf_array[i].buf_raw = pbuf_ctrl->hid_buf + (u32)i * pbuf_ctrl->hid_mps;
		pbuf_ctrl->buf_array[i].buf_valid_len = 0U;
	}

	if (rtos_sema_create(&pbuf_ctrl->rx_sema, 0U, 0xFFFFU) != RTK_SUCCESS) {
		usb_os_mfree(pbuf_ctrl->buf_array);
		pbuf_ctrl->buf_array = NULL;
		usb_os_mfree(pbuf_ctrl->hid_buf);
		pbuf_ctrl->hid_buf = NULL;
		return HAL_ERR_MEM;
	}
	pbuf_ctrl->hid_sema_valid = 1U;

	return HAL_OK;
}

static void usbd_hid_ring_buf_ctrl_deinit(usbd_hid_buf_ctrl_t *pbuf_ctrl)
{
	if (pbuf_ctrl->hid_sema_valid) {
		pbuf_ctrl->hid_sema_valid = 0U;
		if (pbuf_ctrl->read_wait_sema) {
			/* Release any thread blocked in usbd_hid_read(). */
			rtos_sema_give(pbuf_ctrl->rx_sema);
		}
		/* Wait for usbd_hid_read() to observe hid_sema_valid == 0 before deleting
		 * rx_sema. Yield instead of busy-wait: a lower-priority reader would
		 * otherwise get starved of the CPU it needs to release the semaphore. */
		do {
			usb_os_sleep_ms(1U);
		} while (pbuf_ctrl->read_wait_sema);
		rtos_sema_delete(pbuf_ctrl->rx_sema);
	}
	if (pbuf_ctrl->buf_array != NULL) {
		usb_os_mfree(pbuf_ctrl->buf_array);
		pbuf_ctrl->buf_array = NULL;
	}
	if (pbuf_ctrl->hid_buf != NULL) {
		usb_os_mfree(pbuf_ctrl->hid_buf);
		pbuf_ctrl->hid_buf = NULL;
	}
}
#endif

/**
  * @brief  Prepare to receive one INTR OUT packet.
  *         Keyboard: uses the single ep_intr_out.xfer_buf (LED report).
  *         Bidir:    aims the xfer at the current ring-buffer write slot so that
  *                   the ISR can push the payload without an extra memcpy.
  * @retval Status
  */
static int usbd_hid_receive(void)
{
	usbd_hid_t *hid = &hid_device;
#ifdef CONFIG_USBD_HID_BIDIR
	usbd_hid_buf_ctrl_t *pbuf_ctrl = &hid->rx_ctrl;
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;
	usbd_hid_buf_t *p_buf = &pbuf_ctrl->buf_array[pbuf_ctrl->write_idx];

	ep_intr_out->xfer_buf = p_buf->buf_raw;
	ep_intr_out->xfer_len = pbuf_ctrl->hid_mps;
	return usbd_ep_receive(hid->dev, ep_intr_out);
#else
	return usbd_ep_receive(hid->dev, &hid->ep_intr_out);
#endif
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

	if (hid->ctrl_req.bRequest != 0xFFU) {
		/* Deliver SET_REPORT data (e.g., keyboard LED state, or bidir raw OUT report
		 * sent via EP0) to the application. Some hosts send SET_REPORT via the
		 * control endpoint instead of INTR OUT. */
		if (hid->cb->received) {
			hid->cb->received(dev->ep0_out.xfer_buf, hid->ctrl_req.wLength);
		}
		hid->ctrl_req.bRequest = 0xFFU;
	}

	/* No pending request means this data stage does not belong to HID, the composite dispatcher
	   already routed it by active_func. Ref USB 2.0 8.5.3.1: a non-zero value here makes the core
	   stall the status stage, so do not report a failure the host cannot act on. */
	return HAL_OK;
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
#ifndef CONFIG_USBD_HID_BIDIR
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;
#endif

	UNUSED(dev);

	/* Return non-zero if EP is not owned by HID, so composite
	 * dispatcher can continue iterating to the correct sub-function. */
	if (ep_addr != hid->ep_cfg->intr_out_addr) {
		return HAL_ERR_PARA;
	}

#ifdef CONFIG_USBD_HID_BIDIR
	{
		usbd_hid_buf_ctrl_t *pbuf_ctrl = &hid->rx_ctrl;
		if (len > 0) {
			/* Publish the payload to the write slot for readers. If the ring is full,
			 * leave the EP un-rearmed (is_intr_out_busy=0); hid_sof re-arms on a free slot. */
			pbuf_ctrl->buf_array[pbuf_ctrl->write_idx].buf_valid_len = (u16)len;

			if (hid->cb->received) {
				hid->cb->received(pbuf_ctrl->buf_array[pbuf_ctrl->write_idx].buf_raw, len);
			}

			if (!usbd_hid_ring_buf_is_full_internal(pbuf_ctrl)) {
				pbuf_ctrl->write_idx = (u8)((pbuf_ctrl->write_idx + 1U) % pbuf_ctrl->buf_array_cnt);
				pbuf_ctrl->is_intr_out_busy = 1;
				usbd_hid_receive();
			} else {
				pbuf_ctrl->is_intr_out_busy = 0;
			}

			rtos_sema_give(pbuf_ctrl->rx_sema);
		}
	}
#else
	if (hid->cb->received && (len > 0)) {
		hid->cb->received(ep_intr_out->xfer_buf, len);
	}
	/* Keyboard: single-shot buffer, re-arm immediately */
	usbd_hid_receive();
#endif

	return HAL_OK;
}

#endif // CONFIG_USBD_HID_KEYBOARD || CONFIG_USBD_HID_BIDIR

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
	int ret = HAL_OK;
	u16 len = 0;
	u16 report_len = 0;
	u8 *buf = NULL;

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = HAL_ERR_PARA;
			} else {
				/* Ref USB 2.0 9.4.10: the endpoints of the selected interface return to
				   their default state, not halted and data toggle DATA0. This holds even
				   for an interface with the default setting only, hosts do send the
				   request in that case. Only the endpoints of the interface addressed by
				   wIndex are touched, so the other interface keeps its data toggle.
				   Ref USB 2.0 Table 9-10: the whole wIndex is the interface number, so a
				   foreign interface or a non-zero high byte leaves the endpoints untouched. */
				if (req->wIndex == USBD_HID_PRIV_ITF_NUM) {
					usbd_ep_clear_stall(dev, &hid->ep_intr_in);
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
					usbd_ep_clear_stall(dev, &hid->ep_intr_out);
#endif
#ifdef CONFIG_USBD_HID_BIDIR
				} else if (req->wIndex == USBD_HID_VEND_ITF_NUM) {
					usbd_ep_clear_stall(dev, &hid->ep_consumer_intr_in);
#endif
				} else {
					/* Foreign interface */
				}
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
#ifdef CONFIG_USBD_HID_BIDIR
			/* wIndex = local HID interface (composite rebases to 0-based), the whole
			 * field is the interface number: 1 = Vend/Consumer, else Priv. */
			if (req->wIndex == USBD_HID_VEND_ITF_NUM) {
				report_len = sizeof(hid_vend_report_desc);
				buf = (u8 *)hid_vend_report_desc;
			} else {
				report_len = sizeof(hid_priv_report_desc);
				buf = (u8 *)hid_priv_report_desc;
			}
#elif defined(CONFIG_USBD_HID_MOUSE)
			report_len = sizeof(hid_mouse_report_desc);
			buf = (u8 *)hid_mouse_report_desc;
#else
			report_len = sizeof(hid_keyboard_report_desc);
			buf = (u8 *)hid_keyboard_report_desc;
#endif
			if (USB_HIGH_BYTE(req->wValue) == USBD_HID_REPORT_DESC) {
				/* HID Report Descriptor */
				ep0_in->xfer_len = MIN(report_len, req->wLength);
				if (ep0_in->xfer_len > ep0_in->xfer_buf_len) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Rpt desc OVSZ %d > %d\n", ep0_in->xfer_len, ep0_in->xfer_buf_len);
					ret = HAL_ERR_PARA;
					break;
				}
				usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)buf, ep0_in->xfer_len);
			} else if (USB_HIGH_BYTE(req->wValue) == USBD_HID_DESC) {
				/* HID Descriptor */
				len = USBD_HID_DESC_SIZE;
				buf = ep0_in->xfer_buf;
				if (len > ep0_in->xfer_buf_len) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "HID desc OVSZ %d > %d\n", len, ep0_in->xfer_buf_len);
					ret = HAL_ERR_PARA;
					break;
				}
				usb_os_memcpy((void *)buf, (const void *)usbd_hid_desc, len);
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
		/* All HID class requests (GET/SET_REPORT, GET/SET_IDLE, GET/SET_PROTOCOL)
		 * are interface-recipient. In a composite device the framework offers
		 * endpoint- and device-recipient class requests to every sub-function via
		 * a first-accept loop; e.g. UAC1 sampling-freq SET_CUR is endpoint-recipient
		 * and its bRequest (0x01) aliases HID GET_REPORT. Without this guard HID
		 * would wrongly claim and answer another function's request. Only handle
		 * requests addressed to a HID interface. */
		if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) != USB_REQ_RECIPIENT_INTERFACE) {
			ret = HAL_ERR_PARA;
			break;
		}
		switch (req->bRequest) {
		case USBD_HID_SET_PROTOCOL:
			hid->protocol = USB_LOW_BYTE(req->wValue);
			break;
		case USBD_HID_GET_PROTOCOL:
			ep0_in->xfer_buf[0] = hid->protocol;
			ep0_in->xfer_len = 1U;
			usbd_ep_transmit(dev, ep0_in);
			break;

		case USBD_HID_GET_REPORT:
			/* send an empty report */
			ep0_in->xfer_len = MIN(req->wLength, ep0_in->xfer_buf_len);
			usb_os_memset((void *)ep0_in->xfer_buf, 0x0, ep0_in->xfer_len);
			usbd_ep_transmit(dev, ep0_in);
			break;
		case USBD_HID_SET_REPORT:
			if ((req->wLength) && (!(req->bmRequestType & 0x80U))) {
				if (req->wLength > ep0_out->xfer_buf_len) {
					/* Cannot accept the data stage: stall so the host recovers promptly */
					ret = HAL_ERR_PARA;
					break;
				}
				usb_os_memcpy((void *)&hid->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
				ep0_out->xfer_len = req->wLength;
				ret = usbd_ep_receive(dev, ep0_out);
			}
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
	int ret = HAL_OK;
	usbd_hid_t *hid = &hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;
#endif
#ifdef CONFIG_USBD_HID_BIDIR
	usbd_ep_t *ep_consumer_intr_in = &hid->ep_consumer_intr_in;
#endif
	usb_ep_info_t *info;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if (config != 1U) {
		return HAL_ERR_PARA;
	}

	hid->dev = dev;

	if (!hid->from_composite) {
#ifdef CONFIG_USBD_SELF_POWERED
		dev->self_powered = 1;
#else
		dev->self_powered = 0;
#endif
#ifdef CONFIG_USBD_REMOTE_WAKEUP_EN
		dev->remote_wakeup_en = 1;
#else
		dev->remote_wakeup_en = 0;
#endif
	}

	/* Init INTR IN EP */
	info = &ep_intr_in->info;
	ep_intr_in->xfer_state = 0;
	ep_intr_in->is_busy = 0U;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_HID_HS_INT_MAX_PACKET_SIZE : USBD_HID_FS_INT_MAX_PACKET_SIZE;
	usbd_ep_init(dev, ep_intr_in);

#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
	/* Init INTR OUT EP */
	info = &ep_intr_out->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_HID_HS_INT_MAX_PACKET_SIZE : USBD_HID_FS_INT_MAX_PACKET_SIZE;
	usbd_ep_init(dev, ep_intr_out);
#endif

#ifdef CONFIG_USBD_HID_BIDIR
	/* Init Consumer INTR IN EP */
	info = &ep_consumer_intr_in->info;
	ep_consumer_intr_in->xfer_state = 0;
	ep_consumer_intr_in->is_busy = 0U;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_HID_HS_INT_MAX_PACKET_SIZE : USBD_HID_FS_INT_MAX_PACKET_SIZE;
	usbd_ep_init(dev, ep_consumer_intr_in);

	/* Bidir uses a ring buffer; arm the first OUT reception into the current write slot */
	hid->rx_ctrl.is_intr_out_busy = 1U;
	usbd_hid_receive();
#elif defined(CONFIG_USBD_HID_KEYBOARD)
	/* Prepare to receive next INTR OUT packet */
	usbd_hid_receive();
#endif

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
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;
#endif
#ifdef CONFIG_USBD_HID_BIDIR
	usbd_ep_t *ep_consumer_intr_in = &hid->ep_consumer_intr_in;
#endif

	UNUSED(config);

	/* DeInit INTR IN EP */
	usbd_ep_deinit(dev, ep_intr_in);

#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
	usbd_ep_deinit(dev, ep_intr_out);
#endif
#ifdef CONFIG_USBD_HID_BIDIR
	usbd_ep_deinit(dev, ep_consumer_intr_in);
#endif
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
static int hid_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_hid_t *hid = &hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;
#ifdef CONFIG_USBD_HID_BIDIR
	usbd_ep_t *ep_consumer_intr_in = &hid->ep_consumer_intr_in;
#endif

	UNUSED(dev);

	/* Check EP ownership before diagnosing, so a foreign EP (owned by another
	 * composite sub-function) never produces a misattributed USB_DIAG entry. */
	if (ep_addr == hid->ep_cfg->intr_in_addr) {
		if (status != HAL_OK) {
			USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
		}
		ep_intr_in->xfer_state = 0U;
		if (hid->cb->transmitted) {
			hid->cb->transmitted(status);
		}
		return HAL_OK;
	}

#ifdef CONFIG_USBD_HID_BIDIR
	if (ep_addr == hid->ep_cfg->consumer_intr_in_addr) {
		if (status != HAL_OK) {
			USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
		}
		/* Consumer control IN — just clear the busy flag, no user callback */
		ep_consumer_intr_in->xfer_state = 0U;
		return HAL_OK;
	}
#endif

	/* Return non-zero if EP is not owned by HID, so composite
	 * dispatcher can continue iterating to the correct sub-function. */
	return HAL_ERR_PARA;
}

/**
  * @brief  Patch endpoint addresses in a configuration descriptor to use runtime EP config
  * @param  desc: Pointer to the descriptor buffer
  * @param  len: Total length of descriptor data
  * @param  ep_cfg: Endpoint configuration with actual addresses to patch in
  * @retval void
  */
static void usbd_hid_patch_ep_addresses(u8 *desc, u16 len, const usbd_hid_ep_cfg_t *ep_cfg)
{
	u16 i;
#ifdef CONFIG_USBD_HID_BIDIR
	u8 d2h_intr_seen = 0U;
#endif

	for (i = 0; i < len;) {
		u8 dlen = desc[i];
		u8 dtype = desc[i + 1];
		if (dlen == 0) {
			break;
		}
		if ((dtype == USB_DESC_TYPE_ENDPOINT) && ((i + 3) <= len)) {
			u8 addr = desc[i + 2];
			u8 dir = addr & USB_REQ_DIR_MASK;
			u8 type = desc[i + 3] & 0x03;
			if ((dir == USB_D2H) && (type == USB_CH_EP_TYPE_INTR)) {
#ifdef CONFIG_USBD_HID_BIDIR
				/* Priv INTR IN comes first (Priv interface), Consumer INTR IN second. */
				if (d2h_intr_seen == 0U) {
					desc[i + 2] = ep_cfg->intr_in_addr;
				} else {
					desc[i + 2] = ep_cfg->consumer_intr_in_addr;
				}
				d2h_intr_seen++;
#else
				desc[i + 2] = ep_cfg->intr_in_addr;
#endif
			}
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
			else if ((dir == USB_H2D) && (type == USB_CH_EP_TYPE_INTR)) {
				desc[i + 2] = ep_cfg->intr_out_addr;
			}
#endif
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
	usbd_hid_t *hid = &hid_device;
	const u8 *desc = NULL;
	usb_speed_type_t speed = dev->dev_speed;
	u16 len = 0;
	u16 report_len;
	u8 type = USB_HIGH_BYTE(req->wValue);
	u8 is_cfg = 0;
	u8 attr = 0x80U;
#ifdef CONFIG_USBD_HID_BIDIR
	u16 vend_report_len = sizeof(hid_vend_report_desc);
#endif

#ifdef CONFIG_USBD_HID_MOUSE
	report_len = sizeof(hid_mouse_report_desc);
#elif defined(CONFIG_USBD_HID_BIDIR)
	report_len = sizeof(hid_priv_report_desc);
#else
	report_len = sizeof(hid_keyboard_report_desc);
#endif

	if (!hid->from_composite) {
#ifdef CONFIG_USBD_SELF_POWERED
		attr |= USB_CFG_DESC_OFFSET_ATTR_BIT_SELF_POWERED;
#endif
#ifdef CONFIG_USBD_REMOTE_WAKEUP_EN
		attr |= USB_CFG_DESC_OFFSET_ATTR_BIT_REMOTE_WAKEUP;
#endif
	}

	switch (type) {

	case USB_DESC_TYPE_DEVICE:
		desc = usbd_hid_dev_desc;
		len = sizeof(usbd_hid_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
#ifndef CONFIG_USB_FS
		if (speed == USB_SPEED_HIGH) {
			desc = usbd_hid_hs_config_desc;
			len = sizeof(usbd_hid_hs_config_desc);
		} else
#endif
		{
			desc = usbd_hid_fs_config_desc;
			len = sizeof(usbd_hid_fs_config_desc);
		}
		is_cfg = 1;
		break;

#ifndef CONFIG_USB_FS
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		desc = usbd_hid_device_qualifier_desc;
		len = sizeof(usbd_hid_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = usbd_hid_fs_config_desc;
			len = sizeof(usbd_hid_fs_config_desc);
		} else {
			desc = usbd_hid_hs_config_desc;
			len = sizeof(usbd_hid_hs_config_desc);
		}
		is_cfg = 1;
		break;
#endif

	case USB_DESC_TYPE_STRING:
		switch (USB_LOW_BYTE(req->wValue)) {
		case USBD_IDX_LANGID_STR:
			desc = usbd_hid_lang_id_desc;
			len = sizeof(usbd_hid_lang_id_desc);
			break;
		case USBD_IDX_MFC_STR:
			len = usbd_get_str_descriptor(USBD_HID_MFG_STRING, buf, buf_len);
			break;
		case USBD_IDX_PRODUCT_STR:
			if (speed == USB_SPEED_HIGH) {
				len = usbd_get_str_descriptor(USBD_HID_PROD_HS_STRING, buf, buf_len);
			} else {
				len = usbd_get_str_descriptor(USBD_HID_PROD_FS_STRING, buf, buf_len);
			}
			break;
		case USBD_IDX_SERIAL_STR:
			len = usbd_get_str_descriptor(USBD_HID_SN_STRING, buf, buf_len);
			break;
		case USBD_IDX_MS_OS_STR:
			break;
		/* Add customer string here */
		default:
			USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_GET_DESC, 0);
			break;
		}
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
#ifdef CONFIG_USBD_HID_BIDIR
		buf[USBD_HID_CFG_VEND_DESC_ITEM_LENGTH_OFFSET] = USB_LOW_BYTE(vend_report_len);
		buf[USBD_HID_CFG_VEND_DESC_ITEM_LENGTH_OFFSET + 1] = USB_HIGH_BYTE(vend_report_len);
#endif

		if (!hid->from_composite) {
			buf[USB_CFG_DESC_OFFSET_ATTR] = attr;
		}

		usbd_hid_patch_ep_addresses(buf + USB_LEN_CFG_DESC, len - USB_LEN_CFG_DESC, hid->ep_cfg);
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

	if (hid->cb->status_changed) {
		hid->cb->status_changed(old_status, status);
	}
}

#ifdef CONFIG_USBD_HID_BIDIR
/**
  * @brief  SOF handler for bidir.
  *         If the previous OUT frame stalled because the ring was full, the
  *         reader has since drained a slot: re-arm the INTR OUT xfer here.
  *         Also forward SOF to the user callback (e.g. for periodic reports).
  * @note   Called in ISR context; keep it short.
  */
static void hid_sof(usb_dev_t *dev)
{
	usbd_hid_t *hid = &hid_device;
	usbd_hid_buf_ctrl_t *ctrl = &hid->rx_ctrl;

	UNUSED(dev);

	if ((!ctrl->is_intr_out_busy) && (!usbd_hid_ring_buf_is_full_internal(ctrl))) {
		/* The ISR had left the current write slot un-advanced because the ring
		 * was full; a reader has drained one now, so advance and re-arm. */
		ctrl->write_idx = (u8)((ctrl->write_idx + 1U) % ctrl->buf_array_cnt);
		ctrl->is_intr_out_busy = 1U;
		usbd_hid_receive();
	}

	if (hid->cb->sof) {
		hid->cb->sof();
	}
}
#endif

static int usbd_hid_private_init(const usbd_hid_usr_cb_t *cb, const usbd_hid_ep_cfg_t *ep_cfg)
{
	int ret = HAL_OK;
	usbd_hid_t *hid = &hid_device;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;
	usb_ep_info_t *info;
	u32 tx_buf_len;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	if (ep_cfg == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP cfg\n");
		return HAL_ERR_PARA;
	}

	tx_buf_len = ep_cfg->intr_in_xfer_size;
	if (tx_buf_len == 0) {
		tx_buf_len = USBD_HID_INTR_IN_BUF_SIZE;
	}

#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;
	info = &ep_intr_out->info;
	info->addr = ep_cfg->intr_out_addr;
	info->type = USB_CH_EP_TYPE_INTR;
#ifdef CONFIG_USBD_HID_BIDIR
	/* Bidir uses the ring-buffer's DMA slot for xfer_buf; no separate allocation. */
	ep_intr_out->xfer_buf_len = USBD_HID_MAX_BUF_SIZE;
	ep_intr_out->xfer_buf = NULL;
	ep_intr_out->xfer_len = 0U;
#else
	ep_intr_out->xfer_buf_len = USBD_HID_INTR_OUT_BUF_SIZE;
	ep_intr_out->xfer_buf = (u8 *)usb_os_malloc(ep_intr_out->xfer_buf_len);
	ep_intr_out->xfer_len = ep_intr_out->xfer_buf_len;
	if (ep_intr_out->xfer_buf == NULL) {
		return HAL_ERR_MEM;
	}
#endif
#endif

#ifdef CONFIG_USBD_HID_BIDIR
	usbd_ep_t *ep_consumer_intr_in = &hid->ep_consumer_intr_in;
	info = &ep_consumer_intr_in->info;
	info->addr = ep_cfg->consumer_intr_in_addr;
	info->type = USB_CH_EP_TYPE_INTR;
	ep_consumer_intr_in->xfer_buf_len = USBD_HID_MAX_BUF_SIZE;
	ep_consumer_intr_in->xfer_buf = (u8 *)usb_os_malloc(ep_consumer_intr_in->xfer_buf_len);
	if (ep_consumer_intr_in->xfer_buf == NULL) {
		return HAL_ERR_MEM;
	}
#endif

	info = &ep_intr_in->info;
	info->addr = ep_cfg->intr_in_addr;
	info->type = USB_CH_EP_TYPE_INTR;
	ep_intr_in->xfer_buf_len = tx_buf_len;
	ep_intr_in->xfer_buf = (u8 *)usb_os_malloc(tx_buf_len);
	if (ep_intr_in->xfer_buf == NULL) {
#ifdef CONFIG_USBD_HID_BIDIR
		usb_os_mfree(ep_consumer_intr_in->xfer_buf);
		ep_consumer_intr_in->xfer_buf = NULL;
#elif defined(CONFIG_USBD_HID_KEYBOARD)
		usb_os_mfree(ep_intr_out->xfer_buf);
		ep_intr_out->xfer_buf = NULL;
#endif
		return HAL_ERR_MEM;
	}

#ifdef CONFIG_USBD_HID_BIDIR
	ret = usbd_hid_ring_buf_ctrl_init(&hid->rx_ctrl);
	if (ret != HAL_OK) {
		usb_os_mfree(ep_intr_in->xfer_buf);
		ep_intr_in->xfer_buf = NULL;
		usb_os_mfree(ep_consumer_intr_in->xfer_buf);
		ep_consumer_intr_in->xfer_buf = NULL;
		return ret;
	}
	hid->report_id_append = 0U;
#endif

	hid->cb = cb;
	hid->ep_cfg = ep_cfg;
	hid->protocol = 1U; /* HID 1.11 7.2.6: default to Report Protocol after enumeration */
	if (cb->init != NULL) {
		cb->init();
	}


	return ret;
}

/* Exported functions --------------------------------------------------------*/

int usbd_hid_init(const usbd_hid_usr_cb_t *cb, const usbd_hid_ep_cfg_t *ep_cfg)
{
	usbd_hid_t *hid = &hid_device;
	int ret;

	ret = usbd_hid_private_init(cb, ep_cfg);
	if (ret == HAL_OK) {
		hid->from_composite = 0;
		usbd_register_class(&usbd_hid_driver);
	}
	return ret;
}

#ifdef CONFIG_USBD_COMPOSITE
int usbd_composite_hid_init(const usbd_hid_usr_cb_t *cb, const usbd_hid_ep_cfg_t *ep_cfg)
{
	usbd_hid_t *hid = &hid_device;
	int ret;

	ret = usbd_hid_private_init(cb, ep_cfg);
	if (ret == HAL_OK) {
		hid->from_composite = 1;
		ret = usbd_composite_register_driver(&usbd_hid_driver);
	}
	return ret;
}
#endif

int usbd_hid_deinit(void)
{
	usbd_hid_t *hid = &hid_device;

	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
	usbd_ep_t *ep_intr_out = &hid->ep_intr_out;
#endif
#ifdef CONFIG_USBD_HID_BIDIR
	usbd_ep_t *ep_consumer_intr_in = &hid->ep_consumer_intr_in;
#endif

	/* Wait for in-flight transfers on all HID EPs to actually complete
	 * (xfer_state is cleared in the completion ISR) before freeing the DMA
	 * buffers, so the controller never DMAs from freed memory on hot-unplug.
	 * Bounded (~100 ms) to avoid a hang if a completion never arrives after
	 * detach. */
	u32 wait = 0U;
	while ((wait < 1000U) &&
		   (ep_intr_in->xfer_state
#ifdef CONFIG_USBD_HID_BIDIR
			|| ep_consumer_intr_in->xfer_state
#endif
		   )) {
		usb_os_delay_us(100);
		wait++;
	}

#ifdef CONFIG_USBD_COMPOSITE
	if (hid->from_composite) {
		usbd_composite_unregister_driver(&usbd_hid_driver);
	} else
#endif
	{
		usbd_unregister_class();
	}

	if ((hid->cb != NULL) && (hid->cb->deinit != NULL)) {
		hid->cb->deinit();
	}

	usb_os_mfree((void *)ep_intr_in->xfer_buf);
	ep_intr_in->xfer_buf = NULL;
#ifdef CONFIG_USBD_HID_BIDIR
	usb_os_mfree((void *)ep_consumer_intr_in->xfer_buf);
	ep_consumer_intr_in->xfer_buf = NULL;
	/* Bidir: intr_out.xfer_buf points into the ring buffer's DMA region; the
	 * ring-buffer teardown frees it. Only null the pointer. */
	ep_intr_out->xfer_buf = NULL;
	usbd_hid_ring_buf_ctrl_deinit(&hid->rx_ctrl);
#elif defined(CONFIG_USBD_HID_KEYBOARD)
	usb_os_mfree((void *)ep_intr_out->xfer_buf);
	ep_intr_out->xfer_buf = NULL;
#endif

	return HAL_OK;
}

int usbd_hid_send_data(const u8 *data, u32 len)
{
	int ret = HAL_ERR_HW;
	usbd_hid_t *hid = &hid_device;
	usb_dev_t *dev = hid->dev;
	usbd_ep_t *ep_intr_in = &hid->ep_intr_in;
#ifdef CONFIG_USBD_HID_BIDIR
	u32 hdr_len = hid->report_id_append ? 1U : 0U;
#endif

	if (!dev->is_ready) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x TX not ready\n", hid->ep_cfg->intr_in_addr);
		return ret;
	}

#ifdef CONFIG_USBD_HID_BIDIR
	if (len > ep_intr_in->xfer_buf_len - hdr_len) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "len %d > buf %u, truncated\n", (int)(len + hdr_len), ep_intr_in->xfer_buf_len);
		len = ep_intr_in->xfer_buf_len - hdr_len;
	}
#else
	if (len > ep_intr_in->xfer_buf_len) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "len %u > buf %u, truncated\n", len, ep_intr_in->xfer_buf_len);
		len = ep_intr_in->xfer_buf_len;
	}
#endif

	if (!ep_intr_in->xfer_state) {
		if (dev->is_ready) { // In case deinit when plug out
			ep_intr_in->is_busy = 1U;
			ep_intr_in->xfer_state = 1U;
#ifdef CONFIG_USBD_HID_BIDIR
			if (hdr_len) {
				ep_intr_in->xfer_buf[0] = USBD_HID_REPORT_ID;
				usb_os_memcpy((void *)(ep_intr_in->xfer_buf + 1), (const void *)data, len);
			} else {
				usb_os_memcpy((void *)ep_intr_in->xfer_buf, (const void *)data, len);
			}
			/* Pad short reports to the fixed Report Count the descriptor declares. */
			if (len < USBD_HID_TX_PKT_SIZE) {
				usb_os_memset((void *)(ep_intr_in->xfer_buf + hdr_len + len), 0, USBD_HID_TX_PKT_SIZE - len);
				len = USBD_HID_TX_PKT_SIZE;
			}
#else
			usb_os_memcpy((void *)ep_intr_in->xfer_buf, (const void *)data, len);
#endif
			if (dev->is_ready) { // In case deinit when plug out
#ifdef CONFIG_USBD_HID_BIDIR
				ep_intr_in->xfer_len = len + hdr_len;
#else
				ep_intr_in->xfer_len = len;
#endif
				ret = usbd_ep_transmit(hid->dev, ep_intr_in);
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

#ifdef CONFIG_USBD_HID_BIDIR
/**
  * @brief  Send a Consumer Control code through the Consumer INTR IN endpoint.
  * @param  cmd_bitmap: bitmap of USBD_HID_CMD_VOLUME_UP / _DOWN / _POWER
  * @retval Status
  */
static int usbd_hid_system_control(u8 cmd_bitmap)
{
	int ret = HAL_ERR_HW;
	usbd_hid_t *hid = &hid_device;
	usb_dev_t *dev = hid->dev;
	usbd_ep_t *ep_consumer = &hid->ep_consumer_intr_in;
	u8 press[2];
	u8 release[2] = {USBD_HID_VOLUME_REPORT_ID, 0x00U};

	if ((dev == NULL) || (!dev->is_ready)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Consumer TX not ready\n");
		return ret;
	}

	if (ep_consumer->xfer_state) {
		return HAL_BUSY;
	}

	press[0] = USBD_HID_VOLUME_REPORT_ID;
	press[1] = cmd_bitmap;

	/* Press */
	ep_consumer->is_busy = 1U;
	ep_consumer->xfer_state = 1U;
	usb_os_memcpy((void *)ep_consumer->xfer_buf, (void *)press, sizeof(press));
	ep_consumer->xfer_len = sizeof(press);
	ret = usbd_ep_transmit(dev, ep_consumer);
	ep_consumer->is_busy = 0U;
	if (ret != HAL_OK) {
		ep_consumer->xfer_state = 0U;
		return ret;
	}

	/* Wait for the press completion before issuing the release (bounded). */
	{
		u32 wait = 0U;
		while (ep_consumer->xfer_state && (wait < 100U) && dev->is_ready) {
			usb_os_delay_us(1000);
			wait++;
		}
	}

	if (!dev->is_ready) {
		return HAL_ERR_HW;
	}

	/* Release */
	ep_consumer->is_busy = 1U;
	ep_consumer->xfer_state = 1U;
	usb_os_memcpy((void *)ep_consumer->xfer_buf, (void *)release, sizeof(release));
	ep_consumer->xfer_len = sizeof(release);
	ret = usbd_ep_transmit(dev, ep_consumer);
	ep_consumer->is_busy = 0U;
	if (ret != HAL_OK) {
		ep_consumer->xfer_state = 0U;
	}

	return ret;
}

int usbd_hid_volume_ctrl(u8 vol_up)
{
	return usbd_hid_system_control(vol_up ? USBD_HID_CMD_VOLUME_UP : USBD_HID_CMD_VOLUME_DOWN);
}

int usbd_hid_power_ctrl(void)
{
	return usbd_hid_system_control(USBD_HID_CMD_POWER);
}

u32 usbd_hid_ring_buf_is_full(void)
{
	usbd_hid_t *hid = &hid_device;
	if (!hid->rx_ctrl.hid_sema_valid) {
		return 0U;
	}
	return usbd_hid_ring_buf_is_full_internal(&hid->rx_ctrl);
}

u32 usbd_hid_get_read_buf_cnt(void)
{
	usbd_hid_t *hid = &hid_device;
	if (!hid->rx_ctrl.hid_sema_valid) {
		return 0U;
	}
	return usbd_hid_ring_buf_cnt_internal(&hid->rx_ctrl);
}

u32 usbd_hid_read(u8 *buffer, u32 size, u32 time_out_ms)
{
	usbd_hid_t *hid = &hid_device;
	usbd_hid_buf_ctrl_t *ctrl = &hid->rx_ctrl;
	usbd_hid_buf_t *slot;
	u32 copied = 0U;
	u8 *src;
	u16 valid_len;

	if ((buffer == NULL) || (size == 0U)) {
		return 0U;
	}

	/* A reader spawned before ring init sees zeroed rx_ctrl; bail out before
	 * touching any field, else buf_array_cnt==0 faults as divide-by-zero. */
	if (!ctrl->hid_sema_valid) {
		usb_os_sleep_ms(time_out_ms);
		return 0U;
	}

	if (usbd_hid_ring_buf_cnt_internal(ctrl) == 0U) {
		if (time_out_ms == 0U) {
			return 0U;
		}
		ctrl->read_wait_sema = 1U;
		/* Re-check hid_sema_valid after publishing read_wait_sema: pairs with
		 * deinit's observe-read_wait_sema then give-then-delete ordering. */
		if (!ctrl->hid_sema_valid) {
			ctrl->read_wait_sema = 0U;
			return 0U;
		}
		if (rtos_sema_take(ctrl->rx_sema, time_out_ms) != RTK_SUCCESS) {
			ctrl->read_wait_sema = 0U;
			return 0U;
		}
		ctrl->read_wait_sema = 0U;
		if (!ctrl->hid_sema_valid || (usbd_hid_ring_buf_cnt_internal(ctrl) == 0U)) {
			return 0U;
		}
	}

	slot = &ctrl->buf_array[ctrl->read_idx];
	valid_len = slot->buf_valid_len;
	src = slot->buf_raw;

	/* Detect Report-ID-prepended frames so TX can mirror the same format. */
	if ((valid_len >= 1U) && (src[0] == USBD_HID_REPORT_ID)) {
		hid->report_id_append = 1U;
		src++;
		valid_len = (u16)(valid_len - 1U);
	} else {
		hid->report_id_append = 0U;
	}

	copied = MIN((u32)valid_len, size);
	if (copied > 0U) {
		usb_os_memcpy((void *)buffer, (void *)src, copied);
	}

	slot->buf_valid_len = 0U;
	ctrl->read_idx = (u8)((ctrl->read_idx + 1U) % ctrl->buf_array_cnt);

	return copied;
}
#endif /* CONFIG_USBD_HID_BIDIR */
