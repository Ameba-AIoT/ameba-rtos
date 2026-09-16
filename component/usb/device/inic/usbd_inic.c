/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_inic.h"
#include "usbd_inic_otp.h"

/* Private defines -----------------------------------------------------------*/

/* String descriptor indexes, 0x00 ~ 0x03 are reserved by the USB device core */
#define USBD_INIC_IDX_BT_STR              0x04U
#ifdef CONFIG_WHC_ETH
#define USBD_INIC_IDX_ETH_STR             0x05U
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
#define USBD_INIC_ITF_NUM                 3U
#define USBD_INIC_ITF_BT                  0U
#ifdef CONFIG_WHC_ETH
#define USBD_INIC_ITF_ETH                 1U
#else
#define USBD_INIC_ITF_BT_SCO              1U
#define USBD_INIC_BT_SCO_ALT_NUM          6U
#endif
#define USBD_INIC_ITF_WIFI                2U

/* Interface layout of the WiFi only mode, i.e. BT is disabled by eFuse
 *
 * CONFIG_WHC_ETH enabled : ITF0 = WiFi, ITF1 = Ethernet
 * CONFIG_WHC_ETH disabled: ITF0 = WiFi
 */
#ifdef CONFIG_WHC_ETH
#define USBD_INIC_WIFI_ONLY_ITF_NUM       2U
#define USBD_INIC_WIFI_ONLY_ITF_WIFI      0U
#define USBD_INIC_WIFI_ONLY_ITF_ETH       1U
#else
#define USBD_INIC_WIFI_ONLY_ITF_NUM       1U
#define USBD_INIC_WIFI_ONLY_ITF_WIFI      0U
#endif

/*
 * BT SCO is dropped when Ethernet is enabled, so the BT function owns a single interface
 * and the IAD descriptor is no longer needed: report a composite device(0x00/0x00/0x00)
 * instead of the IAD device class(0xEF/0x02/0x01).
 */
#ifdef CONFIG_WHC_ETH
#define USBD_INIC_DEV_CLASS               0x00U
#define USBD_INIC_DEV_SUBCLASS            0x00U
#define USBD_INIC_DEV_PROTOCOL            0x00U
#else
#define USBD_INIC_DEV_CLASS               0xEFU
#define USBD_INIC_DEV_SUBCLASS            0x02U
#define USBD_INIC_DEV_PROTOCOL            0x01U
#endif

#define USBD_INIC_EP_STATE_IDLE           0U
#define USBD_INIC_EP_STATE_BUSY           1U

#define USBD_INIC_RESET_THREAD_PRIORITY   6
#define USBD_INIC_RESET_THREAD_STACK_SIZE 512   /**< Thread tack size */

#define	USBD_INIC_QUERY_PACKET_SIZE       (sizeof(usbd_inic_query_packet_t))

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Number of interfaces reported in the active config descriptor */
#define USBD_INIC_ITF_CNT(idev)           (((idev)->otp.bt_en) ? USBD_INIC_ITF_NUM : USBD_INIC_WIFI_ONLY_ITF_NUM)

/* Private function prototypes -----------------------------------------------*/

static int usbd_inic_set_config(usb_dev_t *dev, u8 config);
static void usbd_inic_clear_config(usb_dev_t *dev, u8 config);
static int usbd_inic_set_wifi_config(usb_dev_t *dev, u8 config);
static int usbd_inic_clear_wifi_config(usb_dev_t *dev, u8 config);
static int usbd_inic_set_bt_config(usb_dev_t *dev, u8 config);
static int usbd_inic_clear_bt_config(usb_dev_t *dev, u8 config);
#ifdef CONFIG_WHC_ETH
static int usbd_inic_set_eth_config(usb_dev_t *dev, u8 config);
static int usbd_inic_clear_eth_config(usb_dev_t *dev, u8 config);
#endif
static int usbd_inic_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_inic_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
static int usbd_inic_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_inic_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_inic_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static void usbd_inic_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
static void usbd_inic_wakeup(usb_dev_t *dev);
/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "INIC";

/* USB Standard Device Descriptor */
static const u8 usbd_inic_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,             // bLength
	USB_DESC_TYPE_DEVICE,         // bDescriptorType
	0x00,                         // bcdUSB
	0x02,
	USBD_INIC_DEV_CLASS,          // bDeviceClass
	USBD_INIC_DEV_SUBCLASS,       // bDeviceSubClass
	USBD_INIC_DEV_PROTOCOL,       // bDeviceProtocol
	USB_MAX_EP0_SIZE,             // bMaxPacketSize
	USB_LOW_BYTE(USBD_INIC_VID),  // idVendor
	USB_HIGH_BYTE(USBD_INIC_VID), // idVendor
	USB_LOW_BYTE(USBD_INIC_PID),  // idProduct
	USB_HIGH_BYTE(USBD_INIC_PID), // idProduct
	0x00,                         // bcdDevice
	0x00,
	USBD_IDX_MFC_STR,             // Index of manufacturer string
	USBD_IDX_PRODUCT_STR,         // Index of product string
	USBD_IDX_SERIAL_STR,          // Index of serial number string
	1                             // bNumConfigurations
}; // usbd_inic_dev_desc

static const u8 usbd_inic_wifi_only_mode_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,             // bLength
	USB_DESC_TYPE_DEVICE,         // bDescriptorType
	0x00,                         // bcdUSB
	0x02,
	0x00,                         // bDeviceClass
	0x00,                         // bDeviceSubClass
	0x00,                         // bDeviceProtocol
	USB_MAX_EP0_SIZE,             // bMaxPacketSize
	USB_LOW_BYTE(USBD_INIC_VID),  // idVendor
	USB_HIGH_BYTE(USBD_INIC_VID), // idVendor
	USB_LOW_BYTE(USBD_INIC_PID),  // idProduct
	USB_HIGH_BYTE(USBD_INIC_PID), // idProduct
	0x00,                         // bcdDevice
	0x00,
	USBD_IDX_MFC_STR,             // Index of manufacturer string
	USBD_IDX_PRODUCT_STR,         // Index of product string
	USBD_IDX_SERIAL_STR,          // Index of serial number string
	1                             // bNumConfigurations
}; // usbd_inic_wifi_only_mode_dev_desc

/* USB Standard Device Descriptor */
static const u8 usbd_inic_lang_id_desc[USB_LEN_LANGID_STR_DESC] = {
	USB_LEN_LANGID_STR_DESC,
	USB_DESC_TYPE_STRING,
	USB_LOW_BYTE(USBD_INIC_LANGID_STRING),
	USB_HIGH_BYTE(USBD_INIC_LANGID_STRING),
};

/* USB Standard Device Descriptor */
static const u8 usbd_inic_dev_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
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
static const u8 usbd_inic_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,						// bLength: Configuration Descriptor size
	USB_DESC_TYPE_CONFIGURATION,			// bDescriptorType: Configuration
	0x00,									// wTotalLength: number of returned bytes, runtime assigned
	0x00,
	USBD_INIC_ITF_NUM,						// bNumInterfaces
	0x01,									// bConfigurationValue
	0x00,									// iConfiguration
	0x80,									// bmAttributes: decided by eFuse
	0xFA,									// MaxPower 500 mA

	/*---------------------------------------------------------------------------*/

#ifndef CONFIG_WHC_ETH
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,						// bLength: IAD Descriptor size
	USB_DESC_TYPE_IAD,						// bDescriptorType: IAD
	USBD_INIC_ITF_BT,						// bFirstInterface
	0x02,									// bInterfaceCount
	USBD_INIC_BT_ITF_CLASS,					// bFunctionClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bFunctionSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bFunctionProtocol
	USBD_INIC_IDX_BT_STR,					// iFunction: USBD_INIC_BT_STRING
#endif

	/*---------------------------------------------------------------------------*/

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_BT,						// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x03,									// bNumEndpoints: 3 endpoints
	USBD_INIC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_INIC_IDX_BT_STR,					// iInterface: USBD_INIC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_INIC_BT_EP1_INTR_IN,				// bEndpointAddress: EP1 IN
	USB_CH_EP_TYPE_INTR,						// bmAttributes: INTR
	0x10,									// wMaxPacketSize: 16 bytes
	0x00,
	0x04,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_INIC_BT_EP2_BULK_OUT,				// bEndpointAddress: EP2 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_INIC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_INIC_BT_EP2_BULK_IN,				// bEndpointAddress: EP2 IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_INIC_HS_BULK_MPS),
	0x00,									// bInterval

	/*---------------------------------------------------------------------------*/

#ifdef CONFIG_WHC_ETH
	/* WHC Ethernet Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_ETH,						// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_ETH_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_INIC_ETH_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_ETH_ITF_PROTOCOL,				// bInterfaceProtocol
	USBD_INIC_IDX_ETH_STR,					// iInterface: USBD_INIC_ETH_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP3_BULK_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_INIC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP6_BULK_IN,				// bEndpointAddress: EP6 IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_ETH_HS_IN_MPS),	// wMaxPacketSize
	USB_HIGH_BYTE(USBD_WHC_ETH_HS_IN_MPS),
	0x00,									// bInterval
#else
	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_INIC_IDX_BT_STR,					// iInterface: USBD_INIC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x00,									// wMaxPacketSize: 0 bytes
	0x00,
	0x04,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_IN,				// bEndpointAddress: EP3 IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x00,									// wMaxPacketSize: 0 bytes
	0x00,
	0x04,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x01,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_INIC_IDX_BT_STR,					// iInterface: USBD_INIC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x09,									// wMaxPacketSize: 9 bytes
	0x00,
	0x04,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_IN,				// bEndpointAddress: EP3 IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x09,									// wMaxPacketSize: 9 bytes
	0x00,
	0x04,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x02,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_INIC_IDX_BT_STR,					// iInterface: USBD_INIC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x11,									// wMaxPacketSize: 17 bytes
	0x00,
	0x04,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_IN,				// bEndpointAddress: EP3 IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x11,									// wMaxPacketSize: 17 bytes
	0x00,
	0x04,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x03,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_INIC_IDX_BT_STR,					// iInterface: USBD_INIC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x19,									// wMaxPacketSize: 25 bytes
	0x00,
	0x04,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_IN,				// bEndpointAddress: EP3 IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x19,									// wMaxPacketSize: 25 bytes
	0x00,
	0x04,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x04,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_INIC_IDX_BT_STR,					// iInterface: USBD_INIC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x21,									// wMaxPacketSize: 33 bytes
	0x00,
	0x04,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_IN,				// bEndpointAddress: EP3 IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x21,									// wMaxPacketSize: 33 bytes
	0x00,
	0x04,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x05,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_INIC_IDX_BT_STR,					// iInterface: USBD_INIC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x31,									// wMaxPacketSize: 49 bytes
	0x00,
	0x04,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_IN,				// bEndpointAddress: EP3 IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x31,									// wMaxPacketSize: 49 bytes
	0x00,
	0x04,									// bInterval

#endif

	/*---------------------------------------------------------------------------*/

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_WIFI,						// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x04,									// bNumEndpoints: 4 endpoints
	USBD_INIC_WIFI_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_INIC_WIFI_ITF_SUBCLASS,			// bInterfaceSubClass
	USBD_INIC_WIFI_ITF_PROTOCOL,			// bInterfaceProtocol
	USBD_IDX_PRODUCT_STR,					// iInterface: USBD_INIC_PROD_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP4_BULK_IN,				// bEndpointAddress: EP4 IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_INIC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP5_BULK_OUT,				// bEndpointAddress: EP5 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_INIC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP6_BULK_OUT,				// bEndpointAddress: EP6 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_INIC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP7_BULK_OUT,				// bEndpointAddress: EP7 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_INIC_HS_BULK_MPS),
	0x00,									// bInterval
};

/* USB Full Speed Configuration Descriptor */
static const u8 usbd_inic_full_speed_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,						// bLength: Configuration Descriptor size
	USB_DESC_TYPE_CONFIGURATION,			// bDescriptorType: Configuration
	0x00,									// wTotalLength: number of returned bytes, runtime assigned
	0x00,
	USBD_INIC_ITF_NUM,						// bNumInterfaces
	0x01,									// bConfigurationValue
	0x00,									// iConfiguration
	0x80,									// bmAttributes: decided by eFuse
	0xFA,									// MaxPower 500 mA

	/*---------------------------------------------------------------------------*/

#ifndef CONFIG_WHC_ETH
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,						// bLength: IAD Descriptor size
	USB_DESC_TYPE_IAD,						// bDescriptorType: IAD
	USBD_INIC_ITF_BT,						// bFirstInterface
	0x02,									// bInterfaceCount
	USBD_INIC_BT_ITF_CLASS,					// bFunctionClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bFunctionSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bFunctionProtocol
	USBD_INIC_IDX_BT_STR,					// iFunction: USBD_INIC_BT_STRING
#endif

	/*---------------------------------------------------------------------------*/

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_BT,						// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x03,									// bNumEndpoints: 3 endpoints
	USBD_INIC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_INIC_IDX_BT_STR,					// iInterface: USBD_INIC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_INIC_BT_EP1_INTR_IN,				// bEndpointAddress: EP1 IN
	USB_CH_EP_TYPE_INTR,					// bmAttributes: INTR
	0x10,									// wMaxPacketSize: 16 bytes
	0x00,
	0x01,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_INIC_BT_EP2_BULK_OUT,				// bEndpointAddress: EP2 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_INIC_BT_EP2_BULK_IN,				// bEndpointAddress: EP2 IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval

	/*---------------------------------------------------------------------------*/

#ifdef CONFIG_WHC_ETH
	/* WHC Ethernet Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_ETH,						// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_ETH_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_INIC_ETH_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_ETH_ITF_PROTOCOL,				// bInterfaceProtocol
	USBD_INIC_IDX_ETH_STR,					// iInterface: USBD_INIC_ETH_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP3_BULK_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP6_BULK_IN,				// bEndpointAddress: EP6 IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval
#else
	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_INIC_IDX_BT_STR,					// iInterface: USBD_INIC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x00,									// wMaxPacketSize: 0 bytes
	0x00,
	0x01,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_IN,				// bEndpointAddress: EP3 IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x00,									// wMaxPacketSize: 0 bytes
	0x00,
	0x01,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x01,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_INIC_IDX_BT_STR,					// iInterface: USBD_INIC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x09,									// wMaxPacketSize: 9 bytes
	0x00,
	0x01,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_IN,				// bEndpointAddress: EP3 IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x09,									// wMaxPacketSize: 9 bytes
	0x00,
	0x01,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x02,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_INIC_IDX_BT_STR,					// iInterface: USBD_INIC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x11,									// wMaxPacketSize: 17 bytes
	0x00,
	0x01,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_IN,				// bEndpointAddress: EP3 IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x11,									// wMaxPacketSize: 17 bytes
	0x00,
	0x01,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x03,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_INIC_IDX_BT_STR,					// iInterface: USBD_INIC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x19,									// wMaxPacketSize: 25 bytes
	0x00,
	0x01,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_IN,				// bEndpointAddress: EP3 IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x19,									// wMaxPacketSize: 25 bytes
	0x00,
	0x01,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x04,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_INIC_IDX_BT_STR,					// iInterface: USBD_INIC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x21,									// wMaxPacketSize: 33 bytes
	0x00,
	0x01,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_IN,				// bEndpointAddress: EP3 IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x21,									// wMaxPacketSize: 33 bytes
	0x00,
	0x01,									// bInterval

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_BT_SCO,					// bInterfaceNumber: Number of Interface
	0x05,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_BT_ITF_CLASS,					// bInterfaceClass: Wireless Controller
	USBD_INIC_BT_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_BT_ITF_PROTOCOL,				// bInterfaceProtocol: Bluetooth Programming Interface
	USBD_INIC_IDX_BT_STR,					// iInterface: USBD_INIC_BT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x31,									// wMaxPacketSize: 49 bytes
	0x00,
	0x01,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_INIC_BT_EP3_ISOC_IN,				// bEndpointAddress: EP3 IN
	USB_CH_EP_TYPE_ISOC,					// bmAttributes: ISOC
	0x31,									// wMaxPacketSize: 49 bytes
	0x00,
	0x01,									// bInterval

#endif

	/*---------------------------------------------------------------------------*/

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_ITF_WIFI,						// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x04,									// bNumEndpoints: 4 endpoints
	USBD_INIC_WIFI_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_INIC_WIFI_ITF_SUBCLASS,			// bInterfaceSubClass
	USBD_INIC_WIFI_ITF_PROTOCOL,			// bInterfaceProtocol
	USBD_IDX_PRODUCT_STR,					// iInterface: USBD_INIC_PROD_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP4_BULK_IN,				// bEndpointAddress: EP4 IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP5_BULK_OUT,				// bEndpointAddress: EP5 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP6_BULK_OUT,				// bEndpointAddress: EP6 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP7_BULK_OUT,				// bEndpointAddress: EP7 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval
};

/* USB Configuration Descriptor for WiFi-only mode */
static const u8 usbd_inic_single_wifi_mode_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,						// bLength: Configuration Descriptor size
	USB_DESC_TYPE_CONFIGURATION,			// bDescriptorType: Configuration
	0x00,									// wTotalLength: number of returned bytes
	0x00,
	USBD_INIC_WIFI_ONLY_ITF_NUM,			// bNumInterfaces
	0x01,									// bConfigurationValue
	0x00,									// iConfiguration
	0x80,									// bmAttributes: decided by eFuse
	0xFA,									// MaxPower 500 mA

	/*---------------------------------------------------------------------------*/

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_WIFI_ONLY_ITF_WIFI,			// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x04,									// bNumEndpoints: 4 endpoints
	USBD_INIC_WIFI_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_INIC_WIFI_ITF_SUBCLASS,			// bInterfaceSubClass
	USBD_INIC_WIFI_ITF_PROTOCOL,			// bInterfaceProtocol
	USBD_IDX_PRODUCT_STR,					// iInterface: USBD_INIC_PROD_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP4_BULK_IN,				// bEndpointAddress: EP4 IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_INIC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP5_BULK_OUT,				// bEndpointAddress: EP5 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_INIC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP6_BULK_OUT,				// bEndpointAddress: EP6 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_INIC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP7_BULK_OUT,				// bEndpointAddress: EP7 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_INIC_HS_BULK_MPS),
	0x00,									// bInterval

#ifdef CONFIG_WHC_ETH
	/* WHC Ethernet Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_WIFI_ONLY_ITF_ETH,			// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_ETH_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_INIC_ETH_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_ETH_ITF_PROTOCOL,				// bInterfaceProtocol
	USBD_INIC_IDX_ETH_STR,					// iInterface: USBD_INIC_ETH_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP3_BULK_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_HS_BULK_MPS),	// wMaxPacketSize: 512 bytes
	USB_HIGH_BYTE(USBD_INIC_HS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP6_BULK_IN,				// bEndpointAddress: EP6 IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_WHC_ETH_HS_IN_MPS),	// wMaxPacketSize
	USB_HIGH_BYTE(USBD_WHC_ETH_HS_IN_MPS),
	0x00,									// bInterval
#endif
};

/* USB Full Speed Configuration Descriptor for WiFi-only mode */
static const u8 usbd_inic_wifi_only_mode_full_speed_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,						// bLength: Configuration Descriptor size
	USB_DESC_TYPE_CONFIGURATION,			// bDescriptorType: Configuration
	0x00,									// wTotalLength: number of returned bytes
	0x00,
	USBD_INIC_WIFI_ONLY_ITF_NUM,			// bNumInterfaces
	0x01,									// bConfigurationValue
	0x00,									// iConfiguration
	0x80,									// bmAttributes: decided by eFuse
	0xFA,									// MaxPower 500 mA

	/*---------------------------------------------------------------------------*/

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_WIFI_ONLY_ITF_WIFI,			// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x04,									// bNumEndpoints: 4 endpoints
	USBD_INIC_WIFI_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_INIC_WIFI_ITF_SUBCLASS,			// bInterfaceSubClass
	USBD_INIC_WIFI_ITF_PROTOCOL,			// bInterfaceProtocol
	USBD_IDX_PRODUCT_STR,					// iInterface: USBD_INIC_PROD_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP4_BULK_IN,				// bEndpointAddress: EP4 IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP5_BULK_OUT,				// bEndpointAddress: EP5 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP6_BULK_OUT,				// bEndpointAddress: EP6 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	USBD_WHC_WIFI_EP7_BULK_OUT,				// bEndpointAddress: EP7 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval

#ifdef CONFIG_WHC_ETH
	/* WHC Ethernet Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	USBD_INIC_WIFI_ONLY_ITF_ETH,			// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x02,									// bNumEndpoints: 2 endpoints
	USBD_INIC_ETH_ITF_CLASS,				// bInterfaceClass: Vendor Specific
	USBD_INIC_ETH_ITF_SUBCLASS,				// bInterfaceSubClass
	USBD_INIC_ETH_ITF_PROTOCOL,				// bInterfaceProtocol
	USBD_INIC_IDX_ETH_STR,					// iInterface: USBD_INIC_ETH_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP3_BULK_OUT,				// bEndpointAddress: EP3 OUT
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT,					// bDescriptorType: Endpoint
	USBD_WHC_ETH_EP6_BULK_IN,				// bEndpointAddress: EP6 IN
	USB_CH_EP_TYPE_BULK,					// bmAttributes: BULK
	USB_LOW_BYTE(USBD_INIC_FS_BULK_MPS),	// wMaxPacketSize: 64 bytes
	USB_HIGH_BYTE(USBD_INIC_FS_BULK_MPS),
	0x00,									// bInterval
#endif
};

/* INIC Class Driver */
static const usbd_class_driver_t usbd_inic_driver = {
	.get_descriptor = usbd_inic_get_descriptor,
	.set_config = usbd_inic_set_config,
	.clear_config = usbd_inic_clear_config,
	.setup = usbd_inic_setup,
	.ep0_data_out = usbd_inic_handle_ep0_data_out,
	.ep_data_in = usbd_inic_handle_ep_data_in,
	.ep_data_out = usbd_inic_handle_ep_data_out,
	.status_changed = usbd_inic_status_changed,
	.wakeup = usbd_inic_wakeup,
};

/* INIC Device */
static usbd_inic_dev_t usbd_inic_dev;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Set class configuration for WiFi interface
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_inic_set_wifi_config(usb_dev_t *dev, u8 config)
{
	int ret;
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_ep_t *ep;
	usb_ep_info_t *info;

	UNUSED(config);

	/* Init BULK IN EP4 */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP4_BULK_IN)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_INIC_HS_BULK_MPS : USBD_INIC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_inic_clear_wifi_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;

	/* Init BULK OUT EP5 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP5_BULK_OUT)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_INIC_HS_BULK_MPS : USBD_INIC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_inic_clear_wifi_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;

	/* Init BULK OUT EP6 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP6_BULK_OUT)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_INIC_HS_BULK_MPS : USBD_INIC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_inic_clear_wifi_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;

	/* Init BULK OUT EP7 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP7_BULK_OUT)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_INIC_HS_BULK_MPS : USBD_INIC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_inic_clear_wifi_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;

	return HAL_OK;
}

#ifdef CONFIG_WHC_ETH
/**
  * @brief  Set class configuration for Ethernet interface
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_inic_set_eth_config(usb_dev_t *dev, u8 config)
{
	int ret;
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_ep_t *ep;
	usb_ep_info_t *info;

	UNUSED(config);

	/* Init BULK OUT EP3 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_ETH_EP3_BULK_OUT)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_INIC_HS_BULK_MPS : USBD_INIC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_inic_clear_eth_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;

	/* Init BULK IN EP6 */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_ETH_EP6_BULK_IN)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_WHC_ETH_HS_IN_MPS : USBD_INIC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_inic_clear_eth_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;

	return HAL_OK;
}
#endif

/**
  * @brief  Set class configuration for BT interface
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_inic_set_bt_config(usb_dev_t *dev, u8 config)
{
	int ret;
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_ep_t *ep;
	usb_ep_info_t *info;

	UNUSED(config);

	/* Init INTR IN EP1 */
	ep = &idev->in_ep[USB_EP_NUM(USBD_INIC_BT_EP1_INTR_IN)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_INIC_HS_INTR_MPS : USBD_INIC_FS_INTR_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_inic_clear_bt_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;

	/* Init BULK IN EP2 */
	ep = &idev->in_ep[USB_EP_NUM(USBD_INIC_BT_EP2_BULK_IN)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_INIC_HS_BULK_MPS : USBD_INIC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_inic_clear_bt_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;

	/* Init BULK OUT EP2 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_INIC_BT_EP2_BULK_OUT)].ep;
	info = &ep->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_INIC_HS_BULK_MPS : USBD_INIC_FS_BULK_MPS;
	ret = usbd_ep_init(dev, ep);
	if (ret != HAL_OK) {
		usbd_inic_clear_bt_config(dev, config);
		return ret;
	}
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;

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
static int usbd_inic_set_config(usb_dev_t *dev, u8 config)
{
	int ret;
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_otp_t *otp = &idev->otp;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if (config != 1U) {
		return HAL_ERR_PARA;
	}

	idev->dev = dev;

	ret = usbd_inic_set_wifi_config(dev, config);
	if (ret != HAL_OK) {
		return ret;
	}

	if (otp->bt_en) {
		ret = usbd_inic_set_bt_config(dev, config);
		if (ret != HAL_OK) {
			goto clean_wifi_config_exit;
		}
	}

#ifdef CONFIG_WHC_ETH
	ret = usbd_inic_set_eth_config(dev, config);
	if (ret != HAL_OK) {
		goto clean_bt_config_exit;
	}
#endif

	if (idev->cb->set_config != NULL) {
		ret = idev->cb->set_config();
		if (ret != HAL_OK) {
			/* Roll back all the interfaces inited above, as the application fails to start up */
			goto clean_config_exit;
		}
	}

	return HAL_OK;

clean_config_exit:
#ifdef CONFIG_WHC_ETH
	usbd_inic_clear_eth_config(dev, config);

clean_bt_config_exit:
#endif
	if (otp->bt_en) {
		usbd_inic_clear_bt_config(dev, config);
	}

clean_wifi_config_exit:
	usbd_inic_clear_wifi_config(dev, config);
	return ret;
}

/**
  * @brief  Clear class configuration for WiFi interface
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_inic_clear_wifi_config(usb_dev_t *dev, u8 config)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_ep_t *ep;

	UNUSED(config);

	/* DeInit BULK IN EP4 */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP4_BULK_IN)].ep;
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit BULK OUT EP5 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP5_BULK_OUT)].ep;
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit BULK OUT EP6 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP6_BULK_OUT)].ep;
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit BULK OUT EP7 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP7_BULK_OUT)].ep;
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;
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
static int usbd_inic_clear_eth_config(usb_dev_t *dev, u8 config)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_ep_t *ep;

	UNUSED(config);

	/* DeInit BULK IN EP6 */
	ep = &idev->in_ep[USB_EP_NUM(USBD_WHC_ETH_EP6_BULK_IN)].ep;
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit BULK OUT EP3 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_WHC_ETH_EP3_BULK_OUT)].ep;
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;
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
static int usbd_inic_clear_bt_config(usb_dev_t *dev, u8 config)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_ep_t *ep;

	UNUSED(config);

	/* DeInit INTR IN EP1 */
	ep = &idev->in_ep[USB_EP_NUM(USBD_INIC_BT_EP1_INTR_IN)].ep;
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit BULK IN EP2 */
	ep = &idev->in_ep[USB_EP_NUM(USBD_INIC_BT_EP2_BULK_IN)].ep;
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;
	usbd_ep_deinit(dev, ep);

	/* DeInit BULK OUT EP2 */
	ep = &idev->out_ep[USB_EP_NUM(USBD_INIC_BT_EP2_BULK_OUT)].ep;
	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;
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
static void usbd_inic_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_otp_t *otp = &idev->otp;

	UNUSED(config);

#ifdef CONFIG_WHC_ETH
	usbd_inic_clear_eth_config(dev, config);
#endif

	if (otp->bt_en) {
		usbd_inic_clear_bt_config(dev, config);
	}

	usbd_inic_clear_wifi_config(dev, config);

	if (idev->cb->clear_config != NULL) {
		idev->cb->clear_config();
	}
}

/**
  * @brief  Handle the inic class control requests
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @param  value: Value for the command code
  * @retval Status
  */
static int usbd_inic_handle_setup(usb_setup_req_t *req, u8 *buf)
{
	int ret = HAL_ERR_PARA;
	usbd_inic_query_packet_t *pkt;
	usbd_inic_dev_t *idev = &usbd_inic_dev;

	if (req->bRequest == USBD_INIC_VENDOR_REQ_FW_DOWNLOAD) {
		if (req->wIndex == USBD_INIC_VENDOR_QUERY_CMD) {
			pkt = (usbd_inic_query_packet_t *)buf;
			pkt->data_len = 0;
			pkt->data_offset = USBD_INIC_QUERY_PACKET_SIZE;
			pkt->pkt_type = USBD_INIC_VENDOR_QUERY_ACK;
			pkt->xfer_status = HAL_OK;
			pkt->rl_version = (u8)(EFUSE_GetChipVersion() & 0xFF);
			pkt->dev_mode = USBD_INIC_FW_TYPE_APPLICATION;
			ret = HAL_OK;
		} else if (req->wIndex == USBD_INIC_VENDOR_RESET_CMD) {
			pkt = (usbd_inic_query_packet_t *)buf;
			pkt->data_len = 0;
			pkt->data_offset = USBD_INIC_QUERY_PACKET_SIZE;
			pkt->pkt_type = USBD_INIC_VENDOR_RESET_ACK;
			pkt->xfer_status = HAL_OK;
			rtos_sema_give(idev->reset_sema);
			ret = HAL_OK;
		}
	} else {
		if ((idev->cb != NULL) && (idev->cb->setup != NULL)) {
			idev->cb->setup(req, buf);
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
static void usbd_inic_itf_clear_stall(usb_dev_t *dev, u16 itf)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;

	if (itf == ((idev->otp.bt_en) ? USBD_INIC_ITF_WIFI : USBD_INIC_WIFI_ONLY_ITF_WIFI)) {
		usbd_ep_clear_stall(dev, &idev->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP4_BULK_IN)].ep);
		usbd_ep_clear_stall(dev, &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP5_BULK_OUT)].ep);
		usbd_ep_clear_stall(dev, &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP6_BULK_OUT)].ep);
		usbd_ep_clear_stall(dev, &idev->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP7_BULK_OUT)].ep);
#ifdef CONFIG_WHC_ETH
	} else if (itf == ((idev->otp.bt_en) ? USBD_INIC_ITF_ETH : USBD_INIC_WIFI_ONLY_ITF_ETH)) {
		usbd_ep_clear_stall(dev, &idev->in_ep[USB_EP_NUM(USBD_WHC_ETH_EP6_BULK_IN)].ep);
		usbd_ep_clear_stall(dev, &idev->out_ep[USB_EP_NUM(USBD_WHC_ETH_EP3_BULK_OUT)].ep);
#endif
	} else if ((idev->otp.bt_en) && (itf == USBD_INIC_ITF_BT)) {
		usbd_ep_clear_stall(dev, &idev->in_ep[USB_EP_NUM(USBD_INIC_BT_EP1_INTR_IN)].ep);
		usbd_ep_clear_stall(dev, &idev->in_ep[USB_EP_NUM(USBD_INIC_BT_EP2_BULK_IN)].ep);
		usbd_ep_clear_stall(dev, &idev->out_ep[USB_EP_NUM(USBD_INIC_BT_EP2_BULK_OUT)].ep);
	} else {
		/* BT SCO, isochronous endpoints only */
	}
}

/**
  * @brief  Handle INIC specific CTRL requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_inic_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	u8 alt;
	int ret = HAL_OK;
	usbd_inic_dev_t *idev = &usbd_inic_dev;
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
			if ((dev->dev_state != USBD_STATE_CONFIGURED) || (req->wIndex >= USBD_INIC_ITF_CNT(idev)) || (req->wValue != 0U)) {
				ret = HAL_ERR_HW;
			} else {
				usbd_inic_itf_clear_stall(dev, req->wIndex);
			}
#else
			if ((dev->dev_state == USBD_STATE_CONFIGURED) && (req->wIndex < USBD_INIC_ITF_CNT(idev))) {
				if ((idev->otp.bt_en) && (req->wIndex == USBD_INIC_ITF_BT_SCO)) {
					if (req->wValue < USBD_INIC_BT_SCO_ALT_NUM) {
						idev->bt_sco_alt = (u8)req->wValue;
					} else {
						ret = HAL_ERR_HW;
					}
				} else if (req->wValue != 0U) {
					ret = HAL_ERR_HW;
				}

				if (ret == HAL_OK) {
					usbd_inic_itf_clear_stall(dev, req->wIndex);
				}
			} else {
				ret = HAL_ERR_HW;
			}
#endif
			break;
		case USB_REQ_GET_INTERFACE:
			if ((dev->dev_state == USBD_STATE_CONFIGURED) && (req->wIndex < USBD_INIC_ITF_CNT(idev))) {
				alt = 0U;
#ifndef CONFIG_WHC_ETH
				if ((idev->otp.bt_en) && (req->wIndex == USBD_INIC_ITF_BT_SCO)) {
					alt = idev->bt_sco_alt;
				}
#endif
				ep0_in->xfer_buf[0] = alt;
				ep0_in->xfer_len = 1U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				usbd_ep_transmit(dev, ep0_in);
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
		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
			if (req->wLength) {
				// SETUP + DATA IN + STATUS
				usbd_inic_handle_setup(req, ep0_in->xfer_buf);
				ep0_in->xfer_len = req->wLength;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				// SETUP + ZLP IN + STATUS, invalid
			}
		} else {
			if (req->wLength) {
				// SETUP + DATA OUT + STATUS, the DATA OUT phase is processed in ep0_data_out callback
				usb_os_memcpy((void *)&idev->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
				ep0_out->xfer_len = req->wLength;
				usbd_ep_receive(dev, ep0_out);
			} else {
				// SETUP + STATUS
				usbd_inic_handle_setup(req, NULL);
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
static int usbd_inic_handle_ep0_data_out(usb_dev_t *dev)
{
	int ret = HAL_OK;
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	const usbd_inic_cb_t *cb = idev->cb;

	UNUSED(dev);

	if (cb != NULL) {
		ret = cb->setup(&idev->ctrl_req, dev->ep0_out.xfer_buf);
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
static int usbd_inic_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	const usbd_inic_cb_t *cb = idev->cb;
	usbd_inic_ep_t *in_ep = &idev->in_ep[USB_EP_NUM(ep_addr)];
	usbd_ep_t *ep = &in_ep->ep;

	UNUSED(dev);

	if (status != HAL_OK) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
	}

	ep->xfer_state = USBD_INIC_EP_STATE_IDLE;

	if (cb->transmitted != NULL) {
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
static int usbd_inic_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	const usbd_inic_cb_t *cb = idev->cb;
	usbd_inic_ep_t *out_ep = &idev->out_ep[USB_EP_NUM(ep_addr)];
	usbd_ep_t *ep = &out_ep->ep;
	void *userdata = out_ep->userdata;

	UNUSED(dev);

	if ((ep->skip_dcache_post_invalidate) && (ep->xfer_buf != NULL) && (len != 0)) {
		DCache_Invalidate((u32)ep->xfer_buf, len);
	}

	if ((len > 0) && (cb->received != NULL)) {
		cb->received(out_ep, len);
	}

	/* The EPs listed below are re-armed by the application itself after the RX data is consumed */
	if ((len == 0) || (cb->received == NULL) ||
		((ep_addr != USBD_WHC_WIFI_EP5_BULK_OUT) && (ep_addr != USBD_WHC_WIFI_EP6_BULK_OUT) && (ep_addr != USBD_WHC_WIFI_EP7_BULK_OUT)
#ifdef CONFIG_WHC_ETH
		 && (ep_addr != USBD_WHC_ETH_EP3_BULK_OUT)
#endif
		)) {
		usbd_inic_receive_data(ep_addr, ep->xfer_buf, ep->xfer_len, userdata);
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
static u16 usbd_inic_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len)
{
	usb_speed_type_t speed = dev->dev_speed;
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_otp_t *otp = &idev->otp;
	const u8 *desc = NULL;
	u32 len = 0;
	u8 type = USB_HIGH_BYTE(req->wValue);
	u8 is_cfg = 0;
	u8 is_dev = 0;

	dev->self_powered = otp->self_powered;
	dev->remote_wakeup_en = otp->remote_wakeup_en;

	switch (type) {

	case USB_DESC_TYPE_DEVICE:
		if (otp->bt_en) {
			desc = usbd_inic_dev_desc;
		} else {
			desc = usbd_inic_wifi_only_mode_dev_desc;
		}

		len = USB_LEN_DEV_DESC;
		is_dev = 1;
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (otp->bt_en) {
			if (speed == USB_SPEED_HIGH) {
				desc = usbd_inic_config_desc;
				len = sizeof(usbd_inic_config_desc);
			} else {
				desc = usbd_inic_full_speed_config_desc;
				len = sizeof(usbd_inic_full_speed_config_desc);
			}
		} else {
			if (speed == USB_SPEED_HIGH) {
				desc = usbd_inic_single_wifi_mode_config_desc;
				len = sizeof(usbd_inic_single_wifi_mode_config_desc);
			} else {
				desc = usbd_inic_wifi_only_mode_full_speed_config_desc;
				len = sizeof(usbd_inic_wifi_only_mode_full_speed_config_desc);
			}
		}
		is_cfg = 1;
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		desc = usbd_inic_dev_qualifier_desc;
		len = USB_LEN_DEV_QUALIFIER_DESC;
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (otp->bt_en) {
			if (speed == USB_SPEED_HIGH) {
				desc = usbd_inic_full_speed_config_desc;
				len = sizeof(usbd_inic_full_speed_config_desc);
			} else {
				desc = usbd_inic_config_desc;
				len = sizeof(usbd_inic_config_desc);
			}
		} else {
			if (speed == USB_SPEED_HIGH) {
				desc = usbd_inic_wifi_only_mode_full_speed_config_desc;
				len = sizeof(usbd_inic_wifi_only_mode_full_speed_config_desc);
			} else {
				desc = usbd_inic_single_wifi_mode_config_desc;
				len = sizeof(usbd_inic_single_wifi_mode_config_desc);
			}
		}
		is_cfg = 1;
		break;

	case USB_DESC_TYPE_STRING:
		switch (USB_LOW_BYTE(req->wValue)) {
		case USBD_IDX_LANGID_STR:
			desc = usbd_inic_lang_id_desc;
			len = USB_LEN_LANGID_STR_DESC;
			break;
		case USBD_IDX_MFC_STR:
			if (otp->otp_param) {
				desc = otp->mfg_str;
				len = otp->mfg_str_len;
			} else {
				len = usbd_get_str_descriptor(USBD_INIC_MFG_STRING, buf, buf_len);
			}
			break;
		case USBD_IDX_PRODUCT_STR:
			if (otp->otp_param) {
				desc = otp->prod_str;
				len = otp->prod_str_len;
			} else {
				len = usbd_get_str_descriptor(USBD_INIC_PROD_STRING, buf, buf_len);
			}
			break;
		case USBD_IDX_SERIAL_STR:
			if (otp->otp_sn) {
				desc = otp->sn_str;
				len = otp->sn_str_len;
			} else {
				len = usbd_get_str_descriptor(USBD_INIC_SN_STRING, buf, buf_len);
			}
			break;
		case USBD_INIC_IDX_BT_STR:
			len = usbd_get_str_descriptor(USBD_INIC_BT_STRING, buf, buf_len);
			break;
#ifdef CONFIG_WHC_ETH
		case USBD_INIC_IDX_ETH_STR:
			len = usbd_get_str_descriptor(USBD_INIC_ETH_STRING, buf, buf_len);
			break;
#endif
		case USBD_IDX_MS_OS_STR:
			/*Not support*/
			break;
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

	if (is_dev != 0) {
		if (otp->otp_param) {
			buf[USB_DEV_DESC_OFFSET_VID] = USB_LOW_BYTE(otp->vid);
			buf[USB_DEV_DESC_OFFSET_VID + 1] = USB_HIGH_BYTE(otp->vid);
			buf[USB_DEV_DESC_OFFSET_PID] = USB_LOW_BYTE(otp->pid);
			buf[USB_DEV_DESC_OFFSET_PID + 1] = USB_HIGH_BYTE(otp->pid);
		} else if (SYSCFG_OTP_BOOTSEL() == BOOT_FROM_USB) {
			buf[USB_DEV_DESC_OFFSET_PID] = USB_LOW_BYTE(USBD_NIC_VID);
			buf[USB_DEV_DESC_OFFSET_PID + 1] = USB_HIGH_BYTE(USBD_NIC_VID);
		}
	}

	if (is_cfg != 0) {
		buf[USB_CFG_DESC_OFFSET_TYPE] = type;
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_ATTR] &= ~(USB_CFG_DESC_OFFSET_ATTR_BIT_SELF_POWERED | USB_CFG_DESC_OFFSET_ATTR_BIT_REMOTE_WAKEUP);
		if (otp->self_powered) {
			buf[USB_CFG_DESC_OFFSET_ATTR] |= USB_CFG_DESC_OFFSET_ATTR_BIT_SELF_POWERED;
		}
		if (otp->remote_wakeup_en) {
			buf[USB_CFG_DESC_OFFSET_ATTR] |= USB_CFG_DESC_OFFSET_ATTR_BIT_REMOTE_WAKEUP;
		}
	}

	return len;

}

static int usbd_inic_wifi_init(void)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_ep_t *ep;
	usb_ep_info_t *info;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP4_BULK_IN);
	ep = &idev->in_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_WIFI_EP4_BULK_IN;
	info->type = USB_CH_EP_TYPE_BULK;
	ep->skip_dcache_pre_clean = 0;
	ep->skip_dcache_post_invalidate = 0;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP5_BULK_OUT);
	ep = &idev->out_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_WIFI_EP5_BULK_OUT;
	info->type = USB_CH_EP_TYPE_BULK;
	ep->skip_dcache_pre_clean = 0;
	ep->skip_dcache_post_invalidate = 0;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP6_BULK_OUT);
	ep = &idev->out_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_WIFI_EP6_BULK_OUT;
	info->type = USB_CH_EP_TYPE_BULK;
	ep->skip_dcache_pre_clean = 0;
	ep->skip_dcache_post_invalidate = 0;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP7_BULK_OUT);
	ep = &idev->out_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_WIFI_EP7_BULK_OUT;
	info->type = USB_CH_EP_TYPE_BULK;
	ep->skip_dcache_pre_clean = 0;
	ep->skip_dcache_post_invalidate = 0;

	return HAL_OK;

}

#ifdef CONFIG_WHC_ETH
static int usbd_inic_eth_init(void)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_ep_t *ep;
	usb_ep_info_t *info;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_ETH_EP3_BULK_OUT);
	ep = &idev->out_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_ETH_EP3_BULK_OUT;
	info->type = USB_CH_EP_TYPE_BULK;
	ep->skip_dcache_pre_clean = 0;
	ep->skip_dcache_post_invalidate = 0;

	ep_num = USB_EP_NUM(USBD_WHC_ETH_EP6_BULK_IN);
	ep = &idev->in_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_WHC_ETH_EP6_BULK_IN;
	info->type = USB_CH_EP_TYPE_BULK;
	ep->skip_dcache_pre_clean = 0;
	ep->skip_dcache_post_invalidate = 0;

	return HAL_OK;
}
#endif

static int usbd_inic_bt_init(void)
{
	int ret = HAL_OK;
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_ep_t *ep;
	usb_ep_info_t *info;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_INIC_BT_EP1_INTR_IN);
	ep = &idev->in_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_INIC_BT_EP1_INTR_IN;
	info->type = USB_CH_EP_TYPE_INTR;

	ep_num = USB_EP_NUM(USBD_INIC_BT_EP2_BULK_IN);
	ep = &idev->in_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_INIC_BT_EP2_BULK_IN;
	info->type = USB_CH_EP_TYPE_BULK;

	ep_num = USB_EP_NUM(USBD_INIC_BT_EP2_BULK_OUT);
	ep = &idev->out_ep[ep_num].ep;
	info = &ep->info;
	info->addr = USBD_INIC_BT_EP2_BULK_OUT;
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
static void usbd_inic_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;

	UNUSED(dev);

	if (idev->cb->status_changed) {
		idev->cb->status_changed(old_status, status);
	}
}

/**
  * @brief  Wakeup callback, called when the device resumes from suspend.
  * @param  dev: USB device instance
  * @retval void
  */
static void usbd_inic_wakeup(usb_dev_t *dev)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;

	UNUSED(dev);

	if (idev->cb->wakeup) {
		idev->cb->wakeup();
	}
}

static void usbd_inic_reset_thread(void *param)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;

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
  * @brief  Initialize inic device
  * @retval Status
  */
int usbd_inic_init(const usbd_inic_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_otp_t *otp = &idev->otp;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	if (usbd_otp_init(otp) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Fail to load OTP para\n");
	}

	usbd_inic_wifi_init();
#ifdef CONFIG_WHC_ETH
	usbd_inic_eth_init();
#endif

	if (otp->bt_en) {
		usbd_inic_bt_init();
	}

	idev->cb = cb;
	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			goto init_exit;
		}
	}

	rtos_sema_create(&idev->reset_sema, 0, 1);

	ret = rtos_task_create(&idev->reset_task, "usbd_inic_reset_thread", usbd_inic_reset_thread, NULL, USBD_INIC_RESET_THREAD_STACK_SIZE,
						   USBD_INIC_RESET_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto init_clean_all;
	}

	usbd_register_class(&usbd_inic_driver);

	return ret;

init_clean_all:
	rtos_sema_delete(idev->reset_sema);

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
  * @brief  DeInitialize inic device
  * @param  void
  * @retval Status
  */
int usbd_inic_deinit(void)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_otp_t *otp = &idev->otp;

	rtos_task_delete(idev->reset_task);
	rtos_sema_delete(idev->reset_sema);

	usbd_unregister_class();

	if (idev->cb != NULL) {
		if (idev->cb->deinit != NULL) {
			idev->cb->deinit();
		}
		idev->cb = NULL;
	}

	usbd_otp_deinit(otp);

	return HAL_OK;
}

int usbd_inic_transmit_ctrl_data(u8 *buf, u16 len)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usb_dev_t *dev = idev->dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;

	if (!dev->is_ready) {
		return HAL_ERR_HW;
	}

	if (len > ep0_in->xfer_buf_len) {
		len = ep0_in->xfer_buf_len;
	}
	usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)buf, len);
	ep0_in->xfer_len = len;
	usbd_ep_transmit(dev, ep0_in);

	return HAL_OK;
}

int usbd_inic_transmit_data(u8 ep_addr, u8 *buf, u32 len, void *userdata)
{
	int ret = HAL_OK;
	u8 num = USB_EP_NUM(ep_addr);
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usb_dev_t *dev = idev->dev;
	usbd_ep_t *ep;

	if (USB_EP_IS_OUT(ep_addr) || (num >= USB_MAX_ENDPOINTS)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP 0x%02x\n", ep_addr);
		return HAL_ERR_PARA;
	}

	if (!dev->is_ready) {
		return HAL_ERR_HW;
	}

	ep = &idev->in_ep[num].ep;

	if (ep->xfer_state == USBD_INIC_EP_STATE_IDLE) {
		ep->xfer_state = USBD_INIC_EP_STATE_BUSY;
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
		ret = usbd_ep_transmit(idev->dev, ep);
	} else {
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x TX BUSY\n", ep_addr);
		ret = HAL_BUSY;
	}

	return ret;
}

int usbd_inic_receive_data(u8 ep_addr, u8 *buf, u32 len, void *userdata)
{
	u8 num = USB_EP_NUM(ep_addr);
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usb_dev_t *dev = idev->dev;
	usbd_ep_t *ep;

	if (USB_EP_IS_IN(ep_addr) || (num >= USB_MAX_ENDPOINTS)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP 0x%02x\n", ep_addr);
		return HAL_ERR_PARA;
	}

	if (!dev->is_ready) {
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
	return usbd_ep_receive(idev->dev, ep);
}

u8 usbd_inic_is_bt_en(void)
{
	usbd_inic_dev_t *idev = &usbd_inic_dev;
	usbd_otp_t *otp = &idev->otp;
	/*Call this after usbd_otp_init*/
	return  otp->bt_en;
}
