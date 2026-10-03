/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_whc.h"
#include "usbd_whc_otp.h"

/* Private defines -----------------------------------------------------------*/

/* String descriptor index placeholders. The real indices are handed out by usbd_add_string()
 * at init, so these values only mark the descriptor fields that usbd_whc_patch_str_idx()
 * rewrites at runtime. They must stay outside the range the core assigns. */
#define USBD_WHC_IDX_BT_STR              0xF0U
#ifdef CONFIG_WHC_ETH
#define USBD_WHC_IDX_ETH_STR             0xF1U
#endif

/*
 * Interface layout of normal mode
 *
 * CONFIG_WHC_ETH enabled : ITF0 = BT HCI, ITF1 = Ethernet, ITF2 = WiFi
 *     BT SCO shares EP3 with Ethernet, so the whole BT SCO interface, including all its
 *     alternate settings, is excluded from the descriptors and BT works in HCI only mode.
 *     Note: SCO based BT audio(e.g. HFP) is NOT supported in this mode.
 * CONFIG_WHC_ETH disabled: ITF0 = BT HCI, ITF1 = BT SCO, ITF2 = WiFi
 */
#define USBD_WHC_ITF_NUM                 3U
#define USBD_WHC_ITF_BT                  0U
#ifdef CONFIG_WHC_ETH
#define USBD_WHC_ITF_ETH                 1U
#else
#define USBD_WHC_ITF_BT_SCO              1U
#define USBD_WHC_BT_SCO_ALT_NUM          6U
#endif
#define USBD_WHC_ITF_WIFI                2U

/* Interface layout of the WiFi only mode, i.e. BT is disabled by eFuse
 *
 * CONFIG_WHC_ETH enabled : ITF0 = WiFi, ITF1 = Ethernet
 * CONFIG_WHC_ETH disabled: ITF0 = WiFi
 */
#ifdef CONFIG_WHC_ETH
#define USBD_WHC_WIFI_ONLY_ITF_NUM       2U
#define USBD_WHC_WIFI_ONLY_ITF_WIFI      0U
#define USBD_WHC_WIFI_ONLY_ITF_ETH       1U
#else
#define USBD_WHC_WIFI_ONLY_ITF_NUM       1U
#define USBD_WHC_WIFI_ONLY_ITF_WIFI      0U
#endif

/*
 * BT SCO is dropped when Ethernet is enabled, so the BT function owns a single interface
 * and the IAD descriptor is no longer needed: report a composite device(0x00/0x00/0x00)
 * instead of the IAD device class(0xEF/0x02/0x01).
 */
#ifdef CONFIG_WHC_ETH
#define USBD_WHC_DEV_CLASS               0x00U
#define USBD_WHC_DEV_SUBCLASS            0x00U
#define USBD_WHC_DEV_PROTOCOL            0x00U
#else
#define USBD_WHC_DEV_CLASS               0xEFU
#define USBD_WHC_DEV_SUBCLASS            0x02U
#define USBD_WHC_DEV_PROTOCOL            0x01U
#endif

#define USBD_WHC_EP_STATE_IDLE           0U
#define USBD_WHC_EP_STATE_BUSY           1U

#define USBD_WHC_RESET_THREAD_PRIORITY   6
#define USBD_WHC_RESET_THREAD_STACK_SIZE 512   /**< Thread tack size */

#define	USBD_WHC_QUERY_PACKET_SIZE       (sizeof(usbd_whc_query_packet_t))

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Number of interfaces reported in the active config descriptor */
#define USBD_WHC_ITF_CNT(idev)           (((idev)->otp.bt_en) ? USBD_WHC_ITF_NUM : USBD_WHC_WIFI_ONLY_ITF_NUM)

/* Private function prototypes -----------------------------------------------*/

static int usbd_whc_set_config(usb_dev_t *dev, u8 config);
static void usbd_whc_clear_config(usb_dev_t *dev, u8 config);
static int usbd_whc_set_wifi_config(usb_dev_t *dev, u8 config);
static int usbd_whc_clear_wifi_config(usb_dev_t *dev, u8 config);
static int usbd_whc_set_bt_config(usb_dev_t *dev, u8 config);
static int usbd_whc_clear_bt_config(usb_dev_t *dev, u8 config);
#ifdef CONFIG_WHC_ETH
static int usbd_whc_set_eth_config(usb_dev_t *dev, u8 config);
static int usbd_whc_clear_eth_config(usb_dev_t *dev, u8 config);
#endif
static int usbd_whc_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_whc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
static int usbd_whc_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_whc_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_whc_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static void usbd_whc_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
static void usbd_whc_wakeup(usb_dev_t *dev);
/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "WHC";

/* USB Standard Device Descriptor */
static const u8 usbd_whc_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,             // bLength
	USB_DESC_TYPE_DEVICE,         // bDescriptorType
	0x00,                         // bcdUSB
	0x02,
	USBD_WHC_DEV_CLASS,          // bDeviceClass
	USBD_WHC_DEV_SUBCLASS,       // bDeviceSubClass
	USBD_WHC_DEV_PROTOCOL,       // bDeviceProtocol
	USB_MAX_EP0_SIZE,             // bMaxPacketSize
	USB_LOW_BYTE(USBD_WHC_VID),  // idVendor
	USB_HIGH_BYTE(USBD_WHC_VID), // idVendor
	USB_LOW_BYTE(USBD_WHC_PID),  // idProduct
	USB_HIGH_BYTE(USBD_WHC_PID), // idProduct
	0x00,                         // bcdDevice
	0x00,
	USBD_IDX_MFC_STR,             // Index of manufacturer string
	USBD_IDX_PRODUCT_STR,         // Index of product string
	USBD_IDX_SERIAL_STR,          // Index of serial number string
	1                             // bNumConfigurations
}; // usbd_whc_dev_desc

static const u8 usbd_whc_wifi_only_mode_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,             // bLength
	USB_DESC_TYPE_DEVICE,         // bDescriptorType
	0x00,                         // bcdUSB
	0x02,
	0x00,                         // bDeviceClass
	0x00,                         // bDeviceSubClass
	0x00,                         // bDeviceProtocol
	USB_MAX_EP0_SIZE,             // bMaxPacketSize
	USB_LOW_BYTE(USBD_WHC_VID),  // idVendor
	USB_HIGH_BYTE(USBD_WHC_VID), // idVendor
	USB_LOW_BYTE(USBD_WHC_PID),  // idProduct
	USB_HIGH_BYTE(USBD_WHC_PID), // idProduct
	0x00,                         // bcdDevice
	0x00,
	USBD_IDX_MFC_STR,             // Index of manufacturer string
	USBD_IDX_PRODUCT_STR,         // Index of product string
	USBD_IDX_SERIAL_STR,          // Index of serial number string
	1                             // bNumConfigurations
}; // usbd_whc_wifi_only_mode_dev_desc

/* USB Standard Device Descriptor */
static const u8 usbd_whc_dev_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
	USB_LEN_DEV_QUALIFIER_DESC,
	USB_DESC_TYPE_DEVICE_QUALIFIER,
	0x00,
	0x02,
	0x00,
	0x00,
	0x00,
	USB_MAX_EP0_SIZE,
	0x01,
	0x00,
};

/* USB Configuration Descriptor */
static const u8 usbd_whc_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,						// bLength: Configuration Descriptor size
	USB_DESC_TYPE_CONFIGURATION,			// bDescriptorType: Configuration
	0x00,									// wTotalLength: number of returned bytes, runtime assigned
	0x00,
	USBD_WHC_ITF_NUM,						// bNumInterfaces
	0x01,									// bConfigurationValue
	0x00,									// iConfiguration
	0x80,									// bmAttributes: decided by eFuse
	0xFA,									// MaxPower 500 mA

	/*---------------------------------------------------------------------------*/

#ifndef CONFIG_WHC_ETH
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,						// bLength: IAD Descriptor size
	USB_DESC_TYPE_IAD,						// bDescriptorType: IAD
	USBD_WHC_ITF_BT,						// bFirstInterface
	0x02,									// bInterfaceCount
	USBD_WHC_BT_ITF_CLASS,					// bFunctionClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bFunctionSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bFunctionProtocol
	USBD_WHC_IDX_BT_STR,					// iFunction: USBD_WHC_BT_STRING
#endif

	/*---------------------------------------------------------------------------*/

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_BT,						// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x03,									// bNumEndpoints: 3 endpoints
	USBD_WHC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_WHC_IDX_BT_STR,					// iInterface: USBD_WHC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_INTR_IN,				// bEndpointAddress: BT INTR IN
	USB_CH_EP_TYPE_INTR,						// bmAttributes: INTR
	0x10,									// wMaxPacketSize: 16 bytes
	0x00,
	0x04,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_BULK_OUT,				// bEndpointAddress: BT BULK OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_WHC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_BULK_IN,				// bEndpointAddress: BT BULK IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_WHC_HS_BULK_MPS),
	0x00,									// bInterval

	/*---------------------------------------------------------------------------*/

#ifdef CONFIG_WHC_ETH
	/* WHC Ethernet Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_ETH,						// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_ETH_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_WHC_ETH_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_ETH_ITF_PROTOCOL,				// bInterfaceProtocol
	USBD_WHC_IDX_ETH_STR,					// iInterface: USBD_WHC_ETH_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP_BULK_OUT,				// bEndpointAddress: Ethernet BULK OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_WHC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP_BULK_IN,				// bEndpointAddress: Ethernet BULK IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_ETH_HS_IN_MPS),	// wMaxPacketSize
	USB_HIGH_BYTE(USBD_WHC_ETH_HS_IN_MPS),
	0x00,									// bInterval
#else
	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_WHC_IDX_BT_STR,					// iInterface: USBD_WHC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_OUT,				// bEndpointAddress: BT SCO ISOC OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x00,									// wMaxPacketSize: 0 bytes
	0x00,
	0x04,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_IN,				// bEndpointAddress: BT SCO ISOC IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x00,									// wMaxPacketSize: 0 bytes
	0x00,
	0x04,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x01,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_WHC_IDX_BT_STR,					// iInterface: USBD_WHC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_OUT,				// bEndpointAddress: BT SCO ISOC OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x09,									// wMaxPacketSize: 9 bytes
	0x00,
	0x04,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_IN,				// bEndpointAddress: BT SCO ISOC IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x09,									// wMaxPacketSize: 9 bytes
	0x00,
	0x04,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x02,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_WHC_IDX_BT_STR,					// iInterface: USBD_WHC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_OUT,				// bEndpointAddress: BT SCO ISOC OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x11,									// wMaxPacketSize: 17 bytes
	0x00,
	0x04,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_IN,				// bEndpointAddress: BT SCO ISOC IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x11,									// wMaxPacketSize: 17 bytes
	0x00,
	0x04,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x03,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_WHC_IDX_BT_STR,					// iInterface: USBD_WHC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_OUT,				// bEndpointAddress: BT SCO ISOC OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x19,									// wMaxPacketSize: 25 bytes
	0x00,
	0x04,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_IN,				// bEndpointAddress: BT SCO ISOC IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x19,									// wMaxPacketSize: 25 bytes
	0x00,
	0x04,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x04,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_WHC_IDX_BT_STR,					// iInterface: USBD_WHC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_OUT,				// bEndpointAddress: BT SCO ISOC OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x21,									// wMaxPacketSize: 33 bytes
	0x00,
	0x04,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_IN,				// bEndpointAddress: BT SCO ISOC IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x21,									// wMaxPacketSize: 33 bytes
	0x00,
	0x04,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x05,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_WHC_IDX_BT_STR,					// iInterface: USBD_WHC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_OUT,				// bEndpointAddress: BT SCO ISOC OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x31,									// wMaxPacketSize: 49 bytes
	0x00,
	0x04,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_IN,				// bEndpointAddress: BT SCO ISOC IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x31,									// wMaxPacketSize: 49 bytes
	0x00,
	0x04,									// bInterval

#endif

	/*---------------------------------------------------------------------------*/

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_WIFI,						// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x04,									// bNumEndpoints: 4 endpoints
	USBD_WHC_WIFI_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_WHC_WIFI_ITF_SUBCLASS,			// bInterfaceSubClass
	USBD_WHC_WIFI_ITF_PROTOCOL,			// bInterfaceProtocol
	USBD_IDX_PRODUCT_STR,					// iInterface: device product string

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_IN,				// bEndpointAddress: WiFi BULK IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_WHC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_OUT_1,				// bEndpointAddress: WiFi BULK OUT 1
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_WHC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_OUT_2,				// bEndpointAddress: WiFi BULK OUT 2
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_WHC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_OUT_3,				// bEndpointAddress: WiFi BULK OUT 3
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_WHC_HS_BULK_MPS),
	0x00,									// bInterval
};

/* USB Full Speed Configuration Descriptor */
static const u8 usbd_whc_full_speed_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,						// bLength: Configuration Descriptor size
	USB_DESC_TYPE_CONFIGURATION,			// bDescriptorType: Configuration
	0x00,									// wTotalLength: number of returned bytes, runtime assigned
	0x00,
	USBD_WHC_ITF_NUM,						// bNumInterfaces
	0x01,									// bConfigurationValue
	0x00,									// iConfiguration
	0x80,									// bmAttributes: decided by eFuse
	0xFA,									// MaxPower 500 mA

	/*---------------------------------------------------------------------------*/

#ifndef CONFIG_WHC_ETH
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,						// bLength: IAD Descriptor size
	USB_DESC_TYPE_IAD,						// bDescriptorType: IAD
	USBD_WHC_ITF_BT,						// bFirstInterface
	0x02,									// bInterfaceCount
	USBD_WHC_BT_ITF_CLASS,					// bFunctionClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bFunctionSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bFunctionProtocol
	USBD_WHC_IDX_BT_STR,					// iFunction: USBD_WHC_BT_STRING
#endif

	/*---------------------------------------------------------------------------*/

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_BT,						// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x03,									// bNumEndpoints: 3 endpoints
	USBD_WHC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_WHC_IDX_BT_STR,					// iInterface: USBD_WHC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_INTR_IN,				// bEndpointAddress: BT INTR IN
	USB_CH_EP_TYPE_INTR,					// bmAttributes: INTR
	0x10,									// wMaxPacketSize: 16 bytes
	0x00,
	0x01,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_BULK_OUT,				// bEndpointAddress: BT BULK OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_BULK_IN,				// bEndpointAddress: BT BULK IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval

	/*---------------------------------------------------------------------------*/

#ifdef CONFIG_WHC_ETH
	/* WHC Ethernet Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_ETH,						// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_ETH_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_WHC_ETH_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_ETH_ITF_PROTOCOL,				// bInterfaceProtocol
	USBD_WHC_IDX_ETH_STR,					// iInterface: USBD_WHC_ETH_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP_BULK_OUT,				// bEndpointAddress: Ethernet BULK OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP_BULK_IN,				// bEndpointAddress: Ethernet BULK IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval
#else
	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_WHC_IDX_BT_STR,					// iInterface: USBD_WHC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_OUT,				// bEndpointAddress: BT SCO ISOC OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x00,									// wMaxPacketSize: 0 bytes
	0x00,
	0x01,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_IN,				// bEndpointAddress: BT SCO ISOC IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x00,									// wMaxPacketSize: 0 bytes
	0x00,
	0x01,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x01,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_WHC_IDX_BT_STR,					// iInterface: USBD_WHC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_OUT,				// bEndpointAddress: BT SCO ISOC OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x09,									// wMaxPacketSize: 9 bytes
	0x00,
	0x01,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_IN,				// bEndpointAddress: BT SCO ISOC IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x09,									// wMaxPacketSize: 9 bytes
	0x00,
	0x01,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x02,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_WHC_IDX_BT_STR,					// iInterface: USBD_WHC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_OUT,				// bEndpointAddress: BT SCO ISOC OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x11,									// wMaxPacketSize: 17 bytes
	0x00,
	0x01,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_IN,				// bEndpointAddress: BT SCO ISOC IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x11,									// wMaxPacketSize: 17 bytes
	0x00,
	0x01,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x03,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_WHC_IDX_BT_STR,					// iInterface: USBD_WHC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_OUT,				// bEndpointAddress: BT SCO ISOC OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x19,									// wMaxPacketSize: 25 bytes
	0x00,
	0x01,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_IN,				// bEndpointAddress: BT SCO ISOC IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x19,									// wMaxPacketSize: 25 bytes
	0x00,
	0x01,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x04,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_WHC_IDX_BT_STR,					// iInterface: USBD_WHC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_OUT,				// bEndpointAddress: BT SCO ISOC OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x21,									// wMaxPacketSize: 33 bytes
	0x00,
	0x01,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_IN,				// bEndpointAddress: BT SCO ISOC IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x21,									// wMaxPacketSize: 33 bytes
	0x00,
	0x01,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x05,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_WHC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_WHC_IDX_BT_STR,					// iInterface: USBD_WHC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_OUT,				// bEndpointAddress: BT SCO ISOC OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x31,									// wMaxPacketSize: 49 bytes
	0x00,
	0x01,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_BT_EP_ISOC_IN,				// bEndpointAddress: BT SCO ISOC IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x31,									// wMaxPacketSize: 49 bytes
	0x00,
	0x01,									// bInterval

#endif

	/*---------------------------------------------------------------------------*/

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_ITF_WIFI,						// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x04,									// bNumEndpoints: 4 endpoints
	USBD_WHC_WIFI_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_WHC_WIFI_ITF_SUBCLASS,			// bInterfaceSubClass
	USBD_WHC_WIFI_ITF_PROTOCOL,			// bInterfaceProtocol
	USBD_IDX_PRODUCT_STR,					// iInterface: device product string

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_IN,				// bEndpointAddress: WiFi BULK IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_OUT_1,				// bEndpointAddress: WiFi BULK OUT 1
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_OUT_2,				// bEndpointAddress: WiFi BULK OUT 2
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_OUT_3,				// bEndpointAddress: WiFi BULK OUT 3
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval
};

/* USB Configuration Descriptor for WiFi-only mode */
static const u8 usbd_whc_single_wifi_mode_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,						// bLength: Configuration Descriptor size
	USB_DESC_TYPE_CONFIGURATION,			// bDescriptorType: Configuration
	0x00,									// wTotalLength: number of returned bytes
	0x00,
	USBD_WHC_WIFI_ONLY_ITF_NUM,			// bNumInterfaces
	0x01,									// bConfigurationValue
	0x00,									// iConfiguration
	0x80,									// bmAttributes: decided by eFuse
	0xFA,									// MaxPower 500 mA

	/*---------------------------------------------------------------------------*/

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_WIFI_ONLY_ITF_WIFI,			// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x04,									// bNumEndpoints: 4 endpoints
	USBD_WHC_WIFI_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_WHC_WIFI_ITF_SUBCLASS,			// bInterfaceSubClass
	USBD_WHC_WIFI_ITF_PROTOCOL,			// bInterfaceProtocol
	USBD_IDX_PRODUCT_STR,					// iInterface: device product string

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_IN,				// bEndpointAddress: WiFi BULK IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_WHC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_OUT_1,				// bEndpointAddress: WiFi BULK OUT 1
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_WHC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_OUT_2,				// bEndpointAddress: WiFi BULK OUT 2
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_WHC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_OUT_3,				// bEndpointAddress: WiFi BULK OUT 3
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_WHC_HS_BULK_MPS),
	0x00,									// bInterval

#ifdef CONFIG_WHC_ETH
	/* WHC Ethernet Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_WIFI_ONLY_ITF_ETH,			// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_ETH_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_WHC_ETH_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_ETH_ITF_PROTOCOL,				// bInterfaceProtocol
	USBD_WHC_IDX_ETH_STR,					// iInterface: USBD_WHC_ETH_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP_BULK_OUT,				// bEndpointAddress: Ethernet BULK OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_WHC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP_BULK_IN,				// bEndpointAddress: Ethernet BULK IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_ETH_HS_IN_MPS),	// wMaxPacketSize
	USB_HIGH_BYTE(USBD_WHC_ETH_HS_IN_MPS),
	0x00,									// bInterval
#endif
};

/* USB Full Speed Configuration Descriptor for WiFi-only mode */
static const u8 usbd_whc_wifi_only_mode_full_speed_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,						// bLength: Configuration Descriptor size
	USB_DESC_TYPE_CONFIGURATION,			// bDescriptorType: Configuration
	0x00,									// wTotalLength: number of returned bytes
	0x00,
	USBD_WHC_WIFI_ONLY_ITF_NUM,			// bNumInterfaces
	0x01,									// bConfigurationValue
	0x00,									// iConfiguration
	0x80,									// bmAttributes: decided by eFuse
	0xFA,									// MaxPower 500 mA

	/*---------------------------------------------------------------------------*/

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_WIFI_ONLY_ITF_WIFI,			// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x04,									// bNumEndpoints: 4 endpoints
	USBD_WHC_WIFI_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_WHC_WIFI_ITF_SUBCLASS,			// bInterfaceSubClass
	USBD_WHC_WIFI_ITF_PROTOCOL,			// bInterfaceProtocol
	USBD_IDX_PRODUCT_STR,					// iInterface: device product string

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_IN,				// bEndpointAddress: WiFi BULK IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_OUT_1,				// bEndpointAddress: WiFi BULK OUT 1
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_OUT_2,				// bEndpointAddress: WiFi BULK OUT 2
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP_BULK_OUT_3,				// bEndpointAddress: WiFi BULK OUT 3
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval

#ifdef CONFIG_WHC_ETH
	/* WHC Ethernet Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_WHC_WIFI_ONLY_ITF_ETH,			// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_WHC_ETH_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_WHC_ETH_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_WHC_ETH_ITF_PROTOCOL,				// bInterfaceProtocol
	USBD_WHC_IDX_ETH_STR,					// iInterface: USBD_WHC_ETH_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP_BULK_OUT,				// bEndpointAddress: Ethernet BULK OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP_BULK_IN,				// bEndpointAddress: Ethernet BULK IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_WHC_FS_BULK_MPS),
	0x00,									// bInterval
#endif
};

/* WHC Class Driver */
static const usbd_class_driver_t usbd_whc_driver = {
	.get_descriptor = usbd_whc_get_descriptor,
	.set_config = usbd_whc_set_config,
	.clear_config = usbd_whc_clear_config,
	.setup = usbd_whc_setup,
	.ep0_data_out = usbd_whc_handle_ep0_data_out,
	.ep_data_in = usbd_whc_handle_ep_data_in,
	.ep_data_out = usbd_whc_handle_ep_data_out,
	.status_changed = usbd_whc_status_changed,
	.wakeup = usbd_whc_wakeup,
};

/* WHC Device */
static usbd_whc_dev_t usbd_whc_dev;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Set class configuration for WiFi interface
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_whc_set_wifi_config(usb_dev_t *dev, u8 config)
{
	int ret;
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_ep_t *ep;
	usb_ep_info_t *info;

	UNUSED(config);

	/* Init WiFi BULK IN */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_WHC_HS_BULK_MPS : USBD_WHC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_whc_clear_wifi_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;

	/* Init WiFi BULK OUT 1 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_1)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_WHC_HS_BULK_MPS : USBD_WHC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_whc_clear_wifi_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;

	/* Init WiFi BULK OUT 2 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_WHC_HS_BULK_MPS : USBD_WHC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_whc_clear_wifi_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;

	/* Init WiFi BULK OUT 3 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_3)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_WHC_HS_BULK_MPS : USBD_WHC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_whc_clear_wifi_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;

	return HAL_OK;
}

#ifdef CONFIG_WHC_ETH
/**
  * @brief  Set class configuration for Ethernet interface
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_whc_set_eth_config(usb_dev_t *dev, u8 config)
{
	int ret;
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_ep_t *ep;
	usb_ep_info_t *info;

	UNUSED(config);

	/* Init Ethernet BULK OUT */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_ETH_EP_BULK_OUT)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_WHC_HS_BULK_MPS : USBD_WHC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_whc_clear_eth_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;

	/* Init Ethernet BULK IN */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_ETH_EP_BULK_IN)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_WHC_ETH_HS_IN_MPS : USBD_WHC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_whc_clear_eth_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;

	return HAL_OK;
}
#endif

/**
  * @brief  Set class configuration for BT interface
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_whc_set_bt_config(usb_dev_t *dev, u8 config)
{
	int ret;
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_ep_t *ep;
	usb_ep_info_t *info;

	UNUSED(config);

	/* Init BT INTR IN */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_BT_EP_INTR_IN)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_WHC_HS_INTR_MPS : USBD_WHC_FS_INTR_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_whc_clear_bt_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;

	/* Init BT BULK IN */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_BT_EP_BULK_IN)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_WHC_HS_BULK_MPS : USBD_WHC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_whc_clear_bt_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;

	/* Init BT BULK OUT */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_BT_EP_BULK_OUT)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_WHC_HS_BULK_MPS : USBD_WHC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_whc_clear_bt_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;

	return HAL_OK;
}

/**
  * @brief  Set class configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_whc_set_config(usb_dev_t *dev, u8 config)
{
	int ret;
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_otp_t *otp = &idev->otp;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if (config != 1U) {
		return HAL_ERR_PARA;
	}

	idev->dev = dev;

	ret = usbd_whc_set_wifi_config(dev, config);
	if (ret != HAL_OK) {
		return ret;
	}

	if (otp->bt_en) {
		ret = usbd_whc_set_bt_config(dev, config);
		if (ret != HAL_OK) {
			goto clean_wifi_config_exit;
		}
	}

#ifdef CONFIG_WHC_ETH
	ret = usbd_whc_set_eth_config(dev, config);
	if (ret != HAL_OK) {
		goto clean_bt_config_exit;
	}
#endif

	if ((idev->cb != NULL) && (idev->cb->set_config != NULL)) {
		ret = idev->cb->set_config();
		if (ret != HAL_OK) {
			/* Roll back all the interfaces inited above, as the application fails to start up */
			goto clean_config_exit;
		}
	}

	return HAL_OK;

clean_config_exit:
#ifdef CONFIG_WHC_ETH
	usbd_whc_clear_eth_config(dev, config);

clean_bt_config_exit:
#endif
	if (otp->bt_en) {
		usbd_whc_clear_bt_config(dev, config);
	}

clean_wifi_config_exit:
	usbd_whc_clear_wifi_config(dev, config);
	return ret;
}

/**
  * @brief  Clear class configuration for WiFi interface
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_whc_clear_wifi_config(usb_dev_t *dev, u8 config)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_ep_t *ep;

	UNUSED(config);

	/* DeInit WiFi BULK IN */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN)].ep;
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit WiFi BULK OUT 1 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_1)].ep;
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit WiFi BULK OUT 2 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2)].ep;
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit WiFi BULK OUT 3 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_3)].ep;
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	return HAL_OK;
}

#ifdef CONFIG_WHC_ETH
/**
  * @brief  Clear class configuration for Ethernet interface
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_whc_clear_eth_config(usb_dev_t *dev, u8 config)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_ep_t *ep;

	UNUSED(config);

	/* DeInit Ethernet BULK IN */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_ETH_EP_BULK_IN)].ep;
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit Ethernet BULK OUT */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_ETH_EP_BULK_OUT)].ep;
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	return HAL_OK;
}
#endif

/**
  * @brief  Clear class configuration for BT interface
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_whc_clear_bt_config(usb_dev_t *dev, u8 config)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_ep_t *ep;

	UNUSED(config);

	/* DeInit BT INTR IN */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_BT_EP_INTR_IN)].ep;
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit BT BULK IN */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_BT_EP_BULK_IN)].ep;
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit BT BULK OUT */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_BT_EP_BULK_OUT)].ep;
	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	return HAL_OK;
}

/**
  * @brief  Clear class configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval None
  */
static void usbd_whc_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_otp_t *otp = &idev->otp;

	UNUSED(config);

#ifdef CONFIG_WHC_ETH
	usbd_whc_clear_eth_config(dev, config);
#endif

	if (otp->bt_en) {
		usbd_whc_clear_bt_config(dev, config);
	}

	usbd_whc_clear_wifi_config(dev, config);

	if ((idev->cb != NULL) && (idev->cb->clear_config != NULL)) {
		idev->cb->clear_config();
	}
}

/**
  * @brief  Handle the whc class control requests
  * @param  req: USB CTRL requests
  * @param  buf: Buffer containing command data (request parameters), NULL for a no-data request
  * @param  buf_len: Usable length of buf in bytes, 0 if buf is NULL
  * @retval Status
  */
static int usbd_whc_handle_setup(usb_setup_req_t *req, u8 *buf, u16 buf_len)
{
	int ret = HAL_ERR_PARA;
	usbd_whc_query_packet_t *pkt;
	usbd_whc_dev_t *idev = &usbd_whc_dev;

	if (req->bRequest == USBD_WHC_VENDOR_REQ_FW_DOWNLOAD) {
		/* The ACK packet is built in place, reject a request which cannot carry it */
		if ((buf == NULL) || (buf_len < USBD_WHC_QUERY_PACKET_SIZE)) {
			return HAL_ERR_PARA;
		}

		pkt = (usbd_whc_query_packet_t *)buf;
		if (req->wIndex == USBD_WHC_VENDOR_QUERY_CMD) {
			pkt->data_len = 0;
			pkt->data_offset = USBD_WHC_QUERY_PACKET_SIZE;
			pkt->pkt_type = USBD_WHC_VENDOR_QUERY_ACK;
			pkt->xfer_status = HAL_OK;
			pkt->rl_version = (u8)(EFUSE_GetChipVersion() & 0xFF);
			pkt->dev_mode = USBD_WHC_FW_TYPE_APPLICATION;
			ret = HAL_OK;
		} else if (req->wIndex == USBD_WHC_VENDOR_RESET_CMD) {
			pkt->data_len = 0;
			pkt->data_offset = USBD_WHC_QUERY_PACKET_SIZE;
			pkt->pkt_type = USBD_WHC_VENDOR_RESET_ACK;
			pkt->xfer_status = HAL_OK;
			rtos_sema_give(idev->reset_sema);
			ret = HAL_OK;
		} else {
			/* Unsupported sub command, keep the request error */
		}
	} else {
		if ((idev->cb != NULL) && (idev->cb->setup != NULL)) {
			/* Propagate the class callback status so an unsupported request is
			 * STALLed by the core instead of being ACKed */
			ret = idev->cb->setup(req, buf);
		}
	}

	return ret;
}

/**
  * @brief  Return the endpoints of an interface to the not halted, DATA0 default state
  * @note   Ref USB 2.0 9.4.10: the endpoints of the interface selected by SET_INTERFACE
  *         return to their default state, not halted and data toggle DATA0. This holds
  *         even for an interface with the default alternate setting only, hosts do send
  *         the request in that case. The BT SCO interface is not handled here, it owns
  *         isochronous endpoints only which can neither be halted nor own a data toggle.
  * @param  dev: USB device instance
  * @param  itf: Interface number, already validated by the caller
  */
static void usbd_whc_itf_clear_stall(usb_dev_t *dev, u16 itf)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;

	if (itf == ((idev->otp.bt_en) ? USBD_WHC_ITF_WIFI : USBD_WHC_WIFI_ONLY_ITF_WIFI)) {
		usbd_ep_clear_stall(dev, &idev->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN)].ep);
		usbd_ep_clear_stall(dev, &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_1)].ep);
		usbd_ep_clear_stall(dev, &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2)].ep);
		usbd_ep_clear_stall(dev, &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_3)].ep);
#ifdef CONFIG_WHC_ETH
	} else if (itf == ((idev->otp.bt_en) ? USBD_WHC_ITF_ETH : USBD_WHC_WIFI_ONLY_ITF_ETH)) {
		usbd_ep_clear_stall(dev, &idev->in_ep[USB_EP_NUM(USBD_WHC_ETH_EP_BULK_IN)].ep);
		usbd_ep_clear_stall(dev, &idev->out_ep[USB_EP_NUM(USBD_WHC_ETH_EP_BULK_OUT)].ep);
#endif
	} else if ((idev->otp.bt_en) && (itf == USBD_WHC_ITF_BT)) {
		usbd_ep_clear_stall(dev, &idev->in_ep[USB_EP_NUM(USBD_WHC_BT_EP_INTR_IN)].ep);
		usbd_ep_clear_stall(dev, &idev->in_ep[USB_EP_NUM(USBD_WHC_BT_EP_BULK_IN)].ep);
		usbd_ep_clear_stall(dev, &idev->out_ep[USB_EP_NUM(USBD_WHC_BT_EP_BULK_OUT)].ep);
	} else {
		/* BT SCO, isochronous endpoints only */
	}
}

/**
  * @brief  Handle WHC specific CTRL requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_whc_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	u8 alt;
	int ret = HAL_OK;
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			/*
			 * BT SCO is the only interface which owns alternate settings, all the other
			 * interfaces have the default alternate setting(0) only: reject any non-existent
			 * interface or alternate setting with a request error as per USB spec 9.4.10.
			 */
#ifdef CONFIG_WHC_ETH
			if ((dev->dev_state != USBD_STATE_CONFIGURED) || (req->wIndex >= USBD_WHC_ITF_CNT(idev)) || (req->wValue != 0U)) {
				ret = HAL_ERR_HW;
			} else {
				usbd_whc_itf_clear_stall(dev, req->wIndex);
			}
#else
			if ((dev->dev_state == USBD_STATE_CONFIGURED) && (req->wIndex < USBD_WHC_ITF_CNT(idev))) {
				if ((idev->otp.bt_en) && (req->wIndex == USBD_WHC_ITF_BT_SCO)) {
					if (req->wValue < USBD_WHC_BT_SCO_ALT_NUM) {
						idev->bt_sco_alt = (u8)req->wValue;
					} else {
						ret = HAL_ERR_HW;
					}
				} else if (req->wValue != 0U) {
					ret = HAL_ERR_HW;
				}

				if (ret == HAL_OK) {
					usbd_whc_itf_clear_stall(dev, req->wIndex);
				}
			} else {
				ret = HAL_ERR_HW;
			}
#endif
			break;
		case USB_REQ_GET_INTERFACE:
			if ((dev->dev_state == USBD_STATE_CONFIGURED) && (req->wIndex < USBD_WHC_ITF_CNT(idev))) {
				alt = 0U;
#ifndef CONFIG_WHC_ETH
				if ((idev->otp.bt_en) && (req->wIndex == USBD_WHC_ITF_BT_SCO)) {
					alt = idev->bt_sco_alt;
				}
#endif
				ep0_in->xfer_buf[0] = alt;
				ep0_in->xfer_len = 1U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		default:
			ret = HAL_ERR_HW;
			break;
		}
		break;
	case USB_REQ_TYPE_CLASS:
	case USB_REQ_TYPE_VENDOR:
		/* The WHC class/vendor requests are all addressed to the device: the FW download
		 * requests use 0xC0/0x40 and the BT HCI command uses 0x20, so reject any other
		 * recipient with a request error as per USB 2.0 9.2.7 */
		if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) != USB_REQ_RECIPIENT_DEVICE) {
			ret = HAL_ERR_HW;
		} else if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
			if (req->wLength != 0U) {
				// SETUP + DATA IN + STATUS
				ret = usbd_whc_handle_setup(req, ep0_in->xfer_buf, (u16)ep0_in->xfer_buf_len);
				if (ret == HAL_OK) {
					/* Cap the IN data stage to the control buffer, a short response is legal
					 * as per USB 2.0 9.3.5 while an oversized xfer_len is rejected by the core
					 * and would leave the control transfer unanswered */
					ep0_in->xfer_len = (req->wLength < ep0_in->xfer_buf_len) ? req->wLength : (u16)ep0_in->xfer_buf_len;
					ret = usbd_ep_transmit(dev, ep0_in);
				}
			} else {
				// SETUP + ZLP IN + STATUS, invalid
				ret = HAL_ERR_PARA;
			}
		} else {
			if (req->wLength != 0U) {
				// SETUP + DATA OUT + STATUS, the DATA OUT phase is processed in ep0_data_out callback
				if (req->wLength > ep0_out->xfer_buf_len) {
					/* The OUT data stage cannot be buffered, the host sends wLength bytes
					 * and there is no way to shorten it: reject the request */
					ret = HAL_ERR_PARA;
				} else {
					usb_os_memcpy((void *)&idev->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
					idev->ctrl_req_pending = 1U;
					ep0_out->xfer_len = req->wLength;
					ret = usbd_ep_receive(dev, ep0_out);
					if (ret != HAL_OK) {
						/* EP0 OUT was not armed, so no data stage completion will ever arrive.
						 * Drop the pending request to keep the next transfer clean */
						idev->ctrl_req_pending = 0U;
					}
				}
			} else {
				// SETUP + STATUS
				ret = usbd_whc_handle_setup(req, NULL, 0U);
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
  * @brief  Handle EP0 Rx Ready event
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @retval Status
  */
static int usbd_whc_handle_ep0_data_out(usb_dev_t *dev)
{
	int ret = HAL_OK;
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	const usbd_whc_cb_t *cb = idev->cb;

	/* Only dispatch a data stage which belongs to a saved H2D request. A separate flag is
	 * needed because every bRequest code including 0xFF is a valid vendor request, so no
	 * value of ctrl_req can mark the slot as empty. Clear it first: a single data stage
	 * belongs to exactly one setup packet and must not be replayed by a later EP0 OUT event */
	if (idev->ctrl_req_pending != 0U) {
		idev->ctrl_req_pending = 0U;

		/* cb is released by usbd_whc_deinit(), which may run between the setup and the data
		 * stage of an H2D request, so both the structure and the handler are checked */
		if ((cb != NULL) && (cb->setup != NULL)) {
			ret = cb->setup(&idev->ctrl_req, dev->ep0_out.xfer_buf);
		}
	}

	return ret;
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_whc_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	const usbd_whc_cb_t *cb = idev->cb;
	u8 ep_num = USB_EP_NUM(ep_addr);
	usbd_whc_ep_t *in_ep;
	usbd_ep_t *ep;

	UNUSED(dev);

	/* USB_EP_NUM() keeps 7 bits of the address, so the index has to be range-checked before use */
	if (ep_num >= USB_MAX_ENDPOINTS) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
		return HAL_ERR_PARA;
	}

	in_ep = &idev->in_ep[ep_num];
	ep = &in_ep->ep;

	if (status != HAL_OK) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
	}

	ep->xfer_state = USBD_WHC_EP_STATE_IDLE;

	if ((cb != NULL) && (cb->transmitted != NULL)) {
		cb->transmitted(in_ep, status);
	}

	return HAL_OK;
}

/**
  * @brief  Data received on non-control Out endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint number
  * @retval Status
  */
static int usbd_whc_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	const usbd_whc_cb_t *cb = idev->cb;
	u8 ep_num = USB_EP_NUM(ep_addr);
	usbd_whc_ep_t *out_ep;
	usbd_ep_t *ep;
	void *userdata;
	/* Evaluated once, so that the RX callback and the EP re-arm policy below always agree */
	u8 rx_cb_valid = (((cb != NULL) && (cb->received != NULL)) ? 1U : 0U);

	UNUSED(dev);

	/* USB_EP_NUM() keeps 7 bits of the address, so the index has to be range-checked before use */
	if (ep_num >= USB_MAX_ENDPOINTS) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
		return HAL_ERR_PARA;
	}

	out_ep = &idev->out_ep[ep_num];
	ep = &out_ep->ep;
	userdata = out_ep->userdata;

	if ((ep->skip_dcache_post_invalidate) && (ep->xfer_buf != NULL) && (len != 0)) {
		DCache_Invalidate((u32)ep->xfer_buf, len);
	}

	if ((len > 0) && (rx_cb_valid != 0U)) {
		cb->received(out_ep, len);
	}

	/* The EPs listed below are re-armed by the application itself after the RX data is consumed */
	if ((len == 0) || (rx_cb_valid == 0U) ||
		((ep_addr != USBD_WHC_WIFI_EP_BULK_OUT_1) && (ep_addr != USBD_WHC_WIFI_EP_BULK_OUT_2) && (ep_addr != USBD_WHC_WIFI_EP_BULK_OUT_3)
#ifdef CONFIG_WHC_ETH
		 && (ep_addr != USBD_WHC_ETH_EP_BULK_OUT)
#endif
		)) {
		usbd_whc_receive_data(ep_addr, ep->xfer_buf, ep->xfer_len, userdata);
	}

	return HAL_OK;
}

/**
  * @brief  Get descriptor callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  req: Setup request handle
  * @param  len: Descriptor length
  * @retval Status
  */
/**
  * @brief  Publish the device identity to the USB device core
  * @note   This device is identified by the values latched in hardware, not by the ones the
  *         application configured: the host has already seen them while the ROM and the
  *         bootloader enumerated, so the identity shall stay the same along the whole boot
  *         chain. Every field is therefore assigned unconditionally, either from OTP when it
  *         is programmed or from this class's own defaults.
  *         The core reads these on every descriptor request, so assigning them once here is
  *         enough. The OTP strings remain valid for as long as the device is enumerable,
  *         because usbd_otp_deinit() runs after usbd_unregister_class().
  * @param  otp: OTP parameters, already loaded
  * @retval None
  */
static void usbd_whc_apply_dev_info(const usbd_otp_t *otp)
{
	usbd_dev_info_t *info = usbd_get_dev_info();
	u16 pid;

	if (otp->otp_param != 0U) {
		info->vid = otp->vid;
		info->pid = otp->pid;
		info->mfg_str = (const char *)otp->mfg_str;
		info->prod_str = (const char *)otp->prod_str;
	} else {
		/* NIC mode is told apart by its own product ID */
		pid = (SYSCFG_OTP_BOOTSEL() == BOOT_FROM_USB) ? USBD_NIC_VID : USBD_WHC_PID;

		info->vid = USBD_WHC_VID;
		info->pid = pid;
		info->mfg_str = USBD_WHC_MFG_STRING;
		info->prod_str = USBD_WHC_PROD_STRING;
	}

	info->sn_str = (otp->otp_sn != 0U) ? (const char *)otp->sn_str : USBD_WHC_SN_STRING;
	info->self_powered = otp->self_powered;
	info->remote_wakeup_en = otp->remote_wakeup_en;
}

/**
  * @brief  Replace the string index placeholders in a configuration descriptor block
  * @note   Only the iInterface field of an Interface descriptor and the iFunction field of an
  *         IAD are rewritten, so a placeholder value appearing in an unrelated byte, e.g. an
  *         endpoint address or an MPS, is left untouched. An unregistered string resolves to 0,
  *         which is the Chapter 9 encoding for "no string".
  * @param  desc: Pointer to the config descriptor body, starting after the config header
  * @param  len: Length of the descriptor block
  * @retval None
  */
static void usbd_whc_patch_str_idx(u8 *desc, u16 len)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	u16 i = 0U;
	u8 dlen;
	u8 dtype;
	u8 *p;

	while (i < len) {
		dlen = desc[i];
		dtype = desc[i + 1U];
		if (dlen == 0U) {
			break;
		}

		/* iInterface is the last byte of an Interface descriptor, iFunction the last byte
		   of an IAD, so both sit one byte before the end of their descriptor */
		if (((dtype == USB_DESC_TYPE_INTERFACE) && (dlen >= USB_LEN_IF_DESC)) ||
			((dtype == USB_DESC_TYPE_IAD) && (dlen >= USB_LEN_IAD_DESC))) {
			p = &desc[i + dlen - 1U];

			if (*p == USBD_WHC_IDX_BT_STR) {
				*p = idev->bt_str_idx;
#ifdef CONFIG_WHC_ETH
			} else if (*p == USBD_WHC_IDX_ETH_STR) {
				*p = idev->eth_str_idx;
#endif
			} else {
				/* Names no string of this class, or none at all */
			}
		}
		i += dlen;
	}
}

static u16 usbd_whc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len)
{
	usb_speed_type_t speed = dev->dev_speed;
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_otp_t *otp = &idev->otp;
	const u8 *desc = NULL;
	u32 len = 0;
	u8 type = USB_HIGH_BYTE(req->wValue);
	u8 is_cfg = 0;

	switch (type) {

	case USB_DESC_TYPE_DEVICE:
		if (otp->bt_en) {
			desc = usbd_whc_dev_desc;
		} else {
			desc = usbd_whc_wifi_only_mode_dev_desc;
		}

		len = USB_LEN_DEV_DESC;
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (otp->bt_en) {
			if (speed == USB_SPEED_HIGH) {
				desc = usbd_whc_config_desc;
				len = sizeof(usbd_whc_config_desc);
			} else {
				desc = usbd_whc_full_speed_config_desc;
				len = sizeof(usbd_whc_full_speed_config_desc);
			}
		} else {
			if (speed == USB_SPEED_HIGH) {
				desc = usbd_whc_single_wifi_mode_config_desc;
				len = sizeof(usbd_whc_single_wifi_mode_config_desc);
			} else {
				desc = usbd_whc_wifi_only_mode_full_speed_config_desc;
				len = sizeof(usbd_whc_wifi_only_mode_full_speed_config_desc);
			}
		}
		is_cfg = 1;
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		desc = usbd_whc_dev_qualifier_desc;
		len = USB_LEN_DEV_QUALIFIER_DESC;
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (otp->bt_en) {
			if (speed == USB_SPEED_HIGH) {
				desc = usbd_whc_full_speed_config_desc;
				len = sizeof(usbd_whc_full_speed_config_desc);
			} else {
				desc = usbd_whc_config_desc;
				len = sizeof(usbd_whc_config_desc);
			}
		} else {
			if (speed == USB_SPEED_HIGH) {
				desc = usbd_whc_wifi_only_mode_full_speed_config_desc;
				len = sizeof(usbd_whc_wifi_only_mode_full_speed_config_desc);
			} else {
				desc = usbd_whc_single_wifi_mode_config_desc;
				len = sizeof(usbd_whc_single_wifi_mode_config_desc);
			}
		}
		is_cfg = 1;
		break;

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

		/* Emit the runtime string indices the core assigned. bmAttributes, bMaxPower and the
		   device descriptor VID/PID/string indices are patched by the core itself. */
		usbd_whc_patch_str_idx(buf + USB_LEN_CFG_DESC, (u16)(len - USB_LEN_CFG_DESC));
	}

	return len;

}

static int usbd_whc_wifi_init(void)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_ep_t *ep;
	usb_ep_info_t *info;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN);
	ep = &idev->in_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_WIFI_EP_BULK_IN;
	info->type = USB_CH_EP_TYPE_BULK;
	ep->skip_dcache_pre_clean = 0;
	ep->skip_dcache_post_invalidate = 0;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_1);
	ep = &idev->out_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_WIFI_EP_BULK_OUT_1;
	info->type = USB_CH_EP_TYPE_BULK;
	ep->skip_dcache_pre_clean = 0;
	ep->skip_dcache_post_invalidate = 0;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2);
	ep = &idev->out_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_WIFI_EP_BULK_OUT_2;
	info->type = USB_CH_EP_TYPE_BULK;
	ep->skip_dcache_pre_clean = 0;
	ep->skip_dcache_post_invalidate = 0;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_3);
	ep = &idev->out_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_WIFI_EP_BULK_OUT_3;
	info->type = USB_CH_EP_TYPE_BULK;
	ep->skip_dcache_pre_clean = 0;
	ep->skip_dcache_post_invalidate = 0;

	return HAL_OK;

}

#ifdef CONFIG_WHC_ETH
static int usbd_whc_eth_init(void)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_ep_t *ep;
	usb_ep_info_t *info;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_ETH_EP_BULK_OUT);
	ep = &idev->out_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_ETH_EP_BULK_OUT;
	info->type = USB_CH_EP_TYPE_BULK;
	ep->skip_dcache_pre_clean = 0;
	ep->skip_dcache_post_invalidate = 0;

	ep_num = USB_EP_NUM(USBD_WHC_ETH_EP_BULK_IN);
	ep = &idev->in_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_ETH_EP_BULK_IN;
	info->type = USB_CH_EP_TYPE_BULK;
	ep->skip_dcache_pre_clean = 0;
	ep->skip_dcache_post_invalidate = 0;

	return HAL_OK;
}
#endif

static int usbd_whc_bt_init(void)
{
	int ret = HAL_OK;
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_ep_t *ep;
	usb_ep_info_t *info;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_BT_EP_INTR_IN);
	ep = &idev->in_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_BT_EP_INTR_IN;
	info->type = USB_CH_EP_TYPE_INTR;

	ep_num = USB_EP_NUM(USBD_WHC_BT_EP_BULK_IN);
	ep = &idev->in_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_BT_EP_BULK_IN;
	info->type = USB_CH_EP_TYPE_BULK;

	ep_num = USB_EP_NUM(USBD_WHC_BT_EP_BULK_OUT);
	ep = &idev->out_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_BT_EP_BULK_OUT;
	info->type = USB_CH_EP_TYPE_BULK;

	return ret;
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
static void usbd_whc_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;

	UNUSED(dev);

	if ((idev->cb != NULL) && (idev->cb->status_changed != NULL)) {
		idev->cb->status_changed(old_status, status);
	}
}

/**
  * @brief  Wakeup callback, called when the device resumes from suspend.
  * @param  dev: USB device instance
  * @retval void
  */
static void usbd_whc_wakeup(usb_dev_t *dev)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;

	UNUSED(dev);

	if ((idev->cb != NULL) && (idev->cb->wakeup != NULL)) {
		idev->cb->wakeup();
	}
}

static void usbd_whc_reset_thread(void *param)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(idev->reset_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			rtos_time_delay_ms(500); // Wait reset request done
			System_Reset();
		}
	}
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize whc device
  * @retval Status
  */
int usbd_whc_init(const usbd_whc_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_otp_t *otp = &idev->otp;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	if (usbd_otp_init(otp) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Fail to load OTP para\n");
	}

	usbd_whc_apply_dev_info(otp);

	/* Class-specific strings, whose indices the core hands out, so the descriptor fields
	   naming them are patched at runtime rather than carrying a fixed index */
	idev->bt_str_idx = (otp->bt_en != 0U) ? usbd_add_string(USBD_WHC_BT_STRING) : 0U;
#ifdef CONFIG_WHC_ETH
	idev->eth_str_idx = usbd_add_string(USBD_WHC_ETH_STRING);
#endif

	usbd_whc_wifi_init();
#ifdef CONFIG_WHC_ETH
	usbd_whc_eth_init();
#endif

	if (otp->bt_en) {
		usbd_whc_bt_init();
	}

	/* No H2D class/vendor request is waiting for its data stage yet */
	idev->ctrl_req_pending = 0U;

	idev->cb = cb;
	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			goto init_exit;
		}
	}

	if (rtos_sema_create(&idev->reset_sema, 0, 1) != RTK_SUCCESS) {
		ret = HAL_ERR_MEM;
		goto init_clean_cb;
	}

	ret = rtos_task_create(&idev->reset_task, "usbd_whc_reset_thread", usbd_whc_reset_thread, NULL, USBD_WHC_RESET_THREAD_STACK_SIZE,
						   USBD_WHC_RESET_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto init_clean_all;
	}

	usbd_register_class(&usbd_whc_driver);

	return ret;

init_clean_all:
	rtos_sema_delete(idev->reset_sema);

init_clean_cb:
	if (idev->cb != NULL) {
		if (idev->cb->deinit != NULL) {
			idev->cb->deinit();
		}
		idev->cb = NULL;
	}

init_exit:
	usbd_otp_deinit(otp);

	return ret;
}

/**
  * @brief  DeInitialize whc device
  * @param  void
  * @retval None
  */
void usbd_whc_deinit(void)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_otp_t *otp = &idev->otp;

	rtos_task_delete(idev->reset_task);
	rtos_sema_delete(idev->reset_sema);

	/* Unregister first: no class callback can run afterwards, so releasing cb and the pending
	 * control request below cannot race an EP0 OUT completion in ISR context */
	usbd_unregister_class();

	idev->ctrl_req_pending = 0U;

	if (idev->cb != NULL) {
		if (idev->cb->deinit != NULL) {
			idev->cb->deinit();
		}
		idev->cb = NULL;
	}

	usbd_otp_deinit(otp);
}

int usbd_whc_transmit_ctrl_data(u8 *buf, u16 len)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usb_dev_t *dev = idev->dev;
	usbd_ep_t *ep0_in;

	/* idev->dev is set on SET_CONFIGURATION only, it stays NULL until enumeration completes */
	if ((dev == NULL) || (dev->is_ready == 0U)) {
		return HAL_ERR_HW;
	}

	ep0_in = &dev->ep0_in;
	if (len > ep0_in->xfer_buf_len) {
		len = ep0_in->xfer_buf_len;
	}
	usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)buf, len);
	ep0_in->xfer_len = len;

	return usbd_ep_transmit(dev, ep0_in);
}

int usbd_whc_transmit_data(u8 ep_addr, u8 *buf, u32 len, void *userdata)
{
	int ret = HAL_OK;
	u8 num = USB_EP_NUM(ep_addr);
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usb_dev_t *dev = idev->dev;
	usbd_ep_t *ep;

	if (USB_EP_IS_OUT(ep_addr) || (num >= USB_MAX_ENDPOINTS)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP 0x%02x\n", ep_addr);
		return HAL_ERR_PARA;
	}

	/* idev->dev is set on SET_CONFIGURATION only, it stays NULL until enumeration completes */
	if ((dev == NULL) || (dev->is_ready == 0U)) {
		return HAL_ERR_HW;
	}

	ep = &idev->in_ep[num].ep;

	if (ep->xfer_state == USBD_WHC_EP_STATE_IDLE) {
		ep->xfer_state = USBD_WHC_EP_STATE_BUSY;
		idev->in_ep[num].userdata = userdata;
		ep->xfer_buf = buf;/*Application should free this txbuf only afer TX DONE*/
		ep->xfer_len = len;
		if ((ep->skip_dcache_pre_clean) && (buf != NULL) && (len != 0)) {
			if (USB_IS_MEM_DMA_ALIGNED(buf)) {
				DCache_Clean((u32)buf, len);
			} else {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "EP TX buf align err\n");
				return HAL_ERR_MEM;
			}
		}
		ret = usbd_ep_transmit(dev, ep);
	} else {
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x TX BUSY\n", ep_addr);
		ret = HAL_BUSY;
	}

	return ret;
}

int usbd_whc_receive_data(u8 ep_addr, u8 *buf, u32 len, void *userdata)
{
	u8 num = USB_EP_NUM(ep_addr);
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usb_dev_t *dev = idev->dev;
	usbd_ep_t *ep;

	if (USB_EP_IS_IN(ep_addr) || (num >= USB_MAX_ENDPOINTS)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP 0x%02x\n", ep_addr);
		return HAL_ERR_PARA;
	}

	/* idev->dev is set on SET_CONFIGURATION only, it stays NULL until enumeration completes */
	if ((dev == NULL) || (dev->is_ready == 0U)) {
		/*RX not ready*/
		return HAL_ERR_HW;
	}

	ep = &idev->out_ep[num].ep;

	ep->xfer_buf = buf;
	ep->xfer_len = len;
	idev->out_ep[num].userdata = userdata;

	if ((ep->skip_dcache_pre_clean) && (buf != NULL) && (len != 0)) {
		if (USB_IS_MEM_DMA_ALIGNED(buf)) {
			/* Clean the whole DMA window rather than the requested length, so that no dirty
			 * line inside the window can be written back over the received data. */
			DCache_Clean((u32)buf, usb_get_dma_len(len, ep->info.mps));
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "EP RX buf align err\n");
			return HAL_ERR_MEM;
		}
	}
	return usbd_ep_receive(dev, ep);
}

u8 usbd_whc_is_bt_en(void)
{
	usbd_whc_dev_t *idev = &usbd_whc_dev;
	usbd_otp_t *otp = &idev->otp;
	/*Call this after usbd_otp_init*/
	return  otp->bt_en;
}
