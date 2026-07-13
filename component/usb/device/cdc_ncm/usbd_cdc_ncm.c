/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_cdc_ncm.h"

#define USBD_CDC_NCM_RX_SPEED_CHECK                       0                     /* CDC NCM rx speed test */
#define USBD_CDC_NCM_TX_SPEED_CHECK                       0                     /* CDC NCM tx speed test */

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
/* Maximum ethernet frames aggregated into one NTB by the TX task.
 * 2 frames per NTB balances throughput and memory: each NTB slot buf is
 * ~3 KB (vs ~6 KB for 4), and 4 slots total = ~12 KB instead of ~24 KB. */
#define USBD_CDC_NCM_NTB_OUT_MAX_DATAGRAMS            2U

/* TX aggregation task: stack size, priority and sema timeout.
 * frame_buf is heap-allocated, so stack only needs call frames + RTOS overhead. */
#define USBD_CDC_NCM_TX_TASK_STACK_SIZE               1024U
#define USBD_CDC_NCM_TX_TASK_PRIORITY                 5U
#define USBD_CDC_NCM_TX_SEMA_TIMEOUT_MS               10U

#else
/* Non-aggregation: one frame per NTB, no extra task. */
#define USBD_CDC_NCM_NTB_OUT_MAX_DATAGRAMS            1U
#endif

/* Aggregation NTB layout (all derived from the max-datagrams knob above):
 *   NDP area = header(8) + (max_datagrams + 1 terminator) * 4 */
#define USBD_CDC_NCM_AGG_NDP_SIZE                     (8U + (USBD_CDC_NCM_NTB_OUT_MAX_DATAGRAMS + 1U) * 4U)
#define USBD_CDC_NCM_AGG_NDP_OFFSET                   USB_CDC_NCM_NTH16_LENGTH
#define USBD_CDC_NCM_AGG_DATA_OFFSET                  ((USBD_CDC_NCM_AGG_NDP_OFFSET + USBD_CDC_NCM_AGG_NDP_SIZE + USB_CDC_NCM_DATAGRAM_ALIGN - 1) & ~(USB_CDC_NCM_DATAGRAM_ALIGN - 1))

/* ncm_tx_ntb_t::frame_count is u8 in the slot typedef -- ensure the
 * configured limit cannot wrap the counter and silently overwrite the
 * NTH16/NDP area. */
_Static_assert(USBD_CDC_NCM_NTB_OUT_MAX_DATAGRAMS <= 255U,
			   "USBD_CDC_NCM_NTB_OUT_MAX_DATAGRAMS exceeds u8 frame_count capacity");

/* RX bulk working buffer.
 *
 * The host aggregates multiple egress datagrams into one OUT NTB up to the
 * dwNtbOutMaxSize we advertise in GET_NTB_PARAMETERS (USBD_CDC_NCM_DEFAULT_NTB_OUT_SIZE).
 * The buffer must therefore be large enough to hold a whole max-size OUT NTB,
 * not just a single Ethernet frame, otherwise large aggregated NTBs would be
 * truncated and their trailing datagrams dropped.
 *
 * Take the larger of {one max Ethernet frame + NTB overhead, advertised OUT
 * max size}, rounded up to a whole number of HS bulk max packets. */
#define USBD_CDC_NCM_BULK_ONE_FRAME_SIZE             (USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE + USB_CDC_NCM_NTH16_LENGTH + USB_CDC_NCM_NDP16_MIN_LENGTH)
#define USBD_CDC_NCM_BULK_BUF_RAW_SIZE               ((USBD_CDC_NCM_BULK_ONE_FRAME_SIZE > USBD_CDC_NCM_DEFAULT_NTB_OUT_SIZE) ? USBD_CDC_NCM_BULK_ONE_FRAME_SIZE : USBD_CDC_NCM_DEFAULT_NTB_OUT_SIZE)
#define USBD_CDC_NCM_BULK_BUF_MAX_SIZE                ((USBD_CDC_NCM_BULK_BUF_RAW_SIZE + USB_BULK_HS_MAX_MPS - 1) / USB_BULK_HS_MAX_MPS * USB_BULK_HS_MAX_MPS)

/* TX NTB working buffer: NTH16 + NDP16 + MAX_DATAGRAMS frames + alignment. */
#define USBD_CDC_NCM_NTB_TX_BUF_SIZE                  (USBD_CDC_NCM_AGG_DATA_OFFSET + USBD_CDC_NCM_NTB_OUT_MAX_DATAGRAMS * (USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE + USB_CDC_NCM_DATAGRAM_ALIGN))

/* Private defines -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* TX NTB slot depth: see USBD_CDC_NCM_TX_DEPTH in usbd_cdc_ncm.h. */

/* Maximum time (ms) usbd_cdc_ncm_transmit() will block waiting for a free
 * TX NTB buffer when called with block != 0.  Set to USB_OS_SEMA_TIMEOUT
 * (wait forever) by default: the caller asked for backpressure, so the
 * upper layer prefers to stall rather than drop a frame. */
#define USBD_CDC_NCM_BULK_TX_TIMEOUT_MS               USB_OS_SEMA_TIMEOUT

/* Device identification */
#define USBD_CDC_NCM_VID                              USB_VID               /**< Vendor ID */
#define USBD_CDC_NCM_PID                              USB_PID               /**< Product ID */
#define USBD_CDC_NCM_SELF_POWERED                     1U                    /**< Self-powered device */
#define USBD_CDC_NCM_REMOTE_WAKEUP_EN                 1U                    /**< Remote wakeup enabled */

/* String descriptors */
#define USBD_CDC_NCM_LANGID_STRING                    0x0409U               /**< Language ID (US English) */
#define USBD_CDC_NCM_MFG_STRING                       "Realtek"             /**< Manufacturer */
#define USBD_CDC_NCM_PROD_HS_STRING                   "Realtek CDC NCM (HS)"/**< Product (HS) */
#define USBD_CDC_NCM_PROD_FS_STRING                   "Realtek CDC NCM (FS)"/**< Product (FS) */
#define USBD_CDC_NCM_SN_STRING                        "1234567890"          /**< Serial number */
#define USBD_CDC_NCM_MAC_STRING_INDEX                 (USBD_IDX_SERIAL_STR + 1) /**< MAC string index */

/* Interrupt Endpoint Intervals */
#define USBD_CDC_NCM_HS_INTR_IN_INTERVAL              1U                    /**< HS INTR IN interval */
#define USBD_CDC_NCM_FS_INTR_IN_INTERVAL              1U                    /**< FS INTR IN interval */

/* NCM Functional Descriptor size (6 bytes) */
#define USBD_CDC_NCM_FUNC_DESC_SIZE                   6U

/* Ethernet Networking Functional Descriptor size (13 bytes) */
#define USBD_CDC_NCM_ETHERNET_FUNC_DESC_SIZE          13U

/* RX thread settings */
#define USBD_CDC_NCM_RX_THREAD_STACK_SIZE             1024U
#define USBD_CDC_NCM_RX_THREAD_PRIORITY               6U
#define USBD_CDC_NCM_RX_SEMA_TAKE_TIMEOUT_MS          100U

/* Default NTB parameters */
#define USBD_CDC_NCM_DEFAULT_NTB_IN_SIZE              4096U
#define USBD_CDC_NCM_DEFAULT_NTB_OUT_SIZE             4096U
#define USBD_CDC_NCM_NTB_DIVISOR                      4U
#define USBD_CDC_NCM_NTB_PAYLOAD_REMAINDER            0U
#define USBD_CDC_NCM_NTB_ALIGNMENT                    4U

/* NCM Interrupt IN endpoint packet size (same as ECM: header + optional 8 byte speed data) */
#define USBD_CDC_NCM_INTR_IN_PACKET_SIZE              16U

/* NCM notification sizes (header + optional data) */
#define USBD_CDC_NCM_NETWORK_CONNECTION_SIZE          8U       /* NCM Notification header only (8 bytes) */
#define USBD_CDC_NCM_CONNECTION_SPEED_CHANGE_SIZE     16U      /* 8 byte header + 8 byte speed data */

#if USBD_CDC_NCM_STATE_TRACE_ENABLE
#define USBD_CDC_NCM_TRACE_THREAD_STACK_SIZE         1024U
#define USBD_CDC_NCM_TRACE_THREAD_PRIORITY           1U
#define USBD_CDC_NCM_TRACE_INTERVAL_MS               1000U
#endif

/* Private types -------------------------------------------------------------*/

/**
 * @brief NCM notification state machine
 */
enum usbd_cdc_ncm_notify_state {
	NCM_NOTIFY_NONE,      /**< No notification pending */
	NCM_NOTIFY_CONNECT,   /**< Network connection notification */
	NCM_NOTIFY_SPEED,     /**< Speed change notification */
};

/* Private function prototypes -----------------------------------------------*/

static int usbd_cdc_ncm_set_config(usb_dev_t *dev, u8 config);
static int usbd_cdc_ncm_clear_config(usb_dev_t *dev, u8 config);
static int usbd_cdc_ncm_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_cdc_ncm_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int usbd_cdc_ncm_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_cdc_ncm_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_cdc_ncm_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static int usbd_cdc_ncm_sof(usb_dev_t *dev);
static void usbd_cdc_ncm_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
static int usbd_cdc_ncm_intr_in_send(void *data, u16 len);
static int usbd_cdc_ncm_send_notification(void);
static inline u8 usbd_cdc_ncm_char_to_hex(u8 value);
static void usbd_cdc_ncm_mac_to_string(const u8 *mac, char *mac_str);
static void usbd_cdc_ncm_set_mac(const u8 *mac);
static int usbd_cdc_ncm_bulk_send(u8 *buf, u32 len);

static void usbd_cdc_ncm_agg_begin(ncm_tx_ntb_t *slot, u16 sequence);
static int  usbd_cdc_ncm_agg_append(ncm_tx_ntb_t *slot, u8 *frame, u32 frame_len);
static void usbd_cdc_ncm_agg_finalize(ncm_tx_ntb_t *slot);
static void usbd_cdc_ncm_tx_kick(usbd_cdc_ncm_dev_t *ncm);
#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
static void usbd_cdc_ncm_tx_task(void *param);
static void usbd_cdc_ncm_tx_task_init(void);
static void usbd_cdc_ncm_tx_task_deinit(void);
#endif
#if USBD_CDC_NCM_STATE_TRACE_ENABLE
static void usbd_cdc_ncm_trace_task_init(void);
static void usbd_cdc_ncm_trace_task_deinit(void);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "NCM";
static u8 usbd_cdc_ncm_default_mac[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};  /**< Default MAC address */

/* USB Standard Device Descriptor */
static const u8 usbd_cdc_ncm_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,                               /* bLength */
	USB_DESC_TYPE_DEVICE,                           /* bDescriptorType */
	0x00, 0x02,                                     /* bcdUSB: 2.0 */
	USB_CDC_NCM_COMM_INTERFACE_CLASS_CODE,              /* bDeviceClass: Communications */
	0x00,                                           /* bDeviceSubClass: 0 at device level; NCM subclass lives on the comm interface descriptor */
	0x00,                                           /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize0 */
	USB_LOW_BYTE(USBD_CDC_NCM_VID),                 /* idVendor */
	USB_HIGH_BYTE(USBD_CDC_NCM_VID),
	USB_LOW_BYTE(USBD_CDC_NCM_PID),                 /* idProduct */
	USB_HIGH_BYTE(USBD_CDC_NCM_PID),
	0x00, 0x02,                                     /* bcdDevice: 2.0 */
	USBD_IDX_MFC_STR,                               /* iManufacturer */
	USBD_IDX_PRODUCT_STR,                           /* iProduct */
	USBD_IDX_SERIAL_STR,                            /* iSerialNumber */
	0x01                                            /* bNumConfigurations */
};

/* USB Standard String Descriptor 0 (Language ID) */
static const u8 usbd_cdc_ncm_lang_id_desc[USB_LEN_LANGID_STR_DESC] = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(USBD_CDC_NCM_LANGID_STRING),       /* wLANGID */
	USB_HIGH_BYTE(USBD_CDC_NCM_LANGID_STRING),
};

#ifndef CONFIG_USB_FS
/* USB Device Qualifier Descriptor */
static const u8 usbd_cdc_ncm_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
	USB_LEN_DEV_QUALIFIER_DESC,                     /* bLength */
	USB_DESC_TYPE_DEVICE_QUALIFIER,                 /* bDescriptorType */
	0x00, 0x02,                                     /* bcdUSB: 2.0 */
	0x00,                                           /* bDeviceClass */
	0x00,                                           /* bDeviceSubClass */
	0x00,                                           /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize0 */
	0x01,                                           /* bNumConfigurations */
	0x00,                                           /* Reserved */
};

/* USB CDC NCM High Speed Configuration Descriptor */
static const u8 usbd_cdc_ncm_hs_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	0x00, 0x00,                                     /* wTotalLength (calculated later) */
	0x02,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
#if USBD_CDC_NCM_SELF_POWERED
	0xE0,                                           /* bmAttributes: Self-powered, Remote wakeup */
#else
	0xA0,                                           /* bmAttributes: Bus-powered, Remote wakeup */
#endif
	0x32,                                           /* bMaxPower: 100mA */

	/* Communication Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_NCM_COMM_INTERFACE_NUM,                /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x01,                                           /* bNumEndpoints */
	USB_CDC_NCM_COMM_INTERFACE_CLASS_CODE,              /* bInterfaceClass: CDC */
	USB_CDC_NCM_SUBCLASS_CODE,                           /* bInterfaceSubClass: NCM */
	0x00,                                           /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* CDC Header Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	USB_CDC_NCM_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_NCM_FUNC_DESC_HEADER,                       /* bDescriptorSubtype */
	0x10, 0x01,                                     /* bcdCDC: 1.10 */

	/* CDC Union Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	USB_CDC_NCM_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_NCM_FUNC_DESC_UNION,                        /* bDescriptorSubtype */
	USBD_CDC_NCM_COMM_INTERFACE_NUM,                /* bControlInterface */
	USBD_CDC_NCM_DATA_INTERFACE_NUM,                /* bSubordinateInterface0 */

	/* CDC Ethernet Networking Functional Descriptor */
	USBD_CDC_NCM_ETHERNET_FUNC_DESC_SIZE,           /* bFunctionLength */
	USB_CDC_NCM_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_NCM_FUNC_DESC_ETHERNET_NETWORKING,          /* bDescriptorSubtype */
	USBD_CDC_NCM_MAC_STRING_INDEX,                  /* iMACAddress */
	0x00, 0x00, 0x00, 0x00,                         /* bmEthernetStatistics */
	USB_LOW_BYTE(USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE), /* wMaxSegmentSize */
	USB_HIGH_BYTE(USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE),
	0x00, 0x00,                                     /* wNumberMCFilters */
	0x00,                                           /* bNumberPowerFilters */

	/* CDC NCM Functional Descriptor */
	USBD_CDC_NCM_FUNC_DESC_SIZE,                    /* bFunctionLength: 6 */
	USB_CDC_NCM_CS_INTERFACE,                           /* bDescriptorType: CS_INTERFACE (0x24) */
	USB_CDC_NCM_FUNC_DESC,                          /* bDescriptorSubtype: NCM (0x1A) */
	0x00, 0x01,                                     /* bcdNcmVersion: 1.00 */
	USB_CDC_NCM_NTB16_SUPPORTED,                    /* bmNetworkCapabilities: NTB16 supported */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_CDC_NCM_INTR_IN_EP,                        /* bEndpointAddress */
	USB_CH_EP_TYPE_INTR,                            /* bmAttributes */
	USB_LOW_BYTE(USBD_CDC_NCM_INTR_IN_PACKET_SIZE),  /* wMaxPacketSize (reuse same size as ECM) */
	USB_HIGH_BYTE(USBD_CDC_NCM_INTR_IN_PACKET_SIZE),
	USBD_CDC_NCM_HS_INTR_IN_INTERVAL,               /* bInterval */

	/* Data Interface Descriptor (Alt 0 - No endpoints) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_NCM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x00,                                           /* bNumEndpoints */
	USB_CDC_NCM_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	0x00,                                           /* bInterfaceSubClass */
	USB_CDC_NCM_DATA_PROTOCOL_NETWORK_TRANSFER_BLOCK,   /* bInterfaceProtocol: NCM */
	0x00,                                           /* iInterface */

	/* Data Interface Descriptor (Alt 1 - Active) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_NCM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x01,                                           /* bAlternateSetting */
	0x02,                                           /* bNumEndpoints */
	USB_CDC_NCM_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	0x00,                                           /* bInterfaceSubClass */
	USB_CDC_NCM_DATA_PROTOCOL_NETWORK_TRANSFER_BLOCK,   /* bInterfaceProtocol: NCM */
	0x00,                                           /* iInterface */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_CDC_NCM_BULK_IN_EP,                        /* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes */
	USB_LOW_BYTE(USB_BULK_HS_MAX_MPS),              /* wMaxPacketSize */
	USB_HIGH_BYTE(USB_BULK_HS_MAX_MPS),
	0x00,                                           /* bInterval */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_CDC_NCM_BULK_OUT_EP,                       /* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes */
	USB_LOW_BYTE(USB_BULK_HS_MAX_MPS),              /* wMaxPacketSize */
	USB_HIGH_BYTE(USB_BULK_HS_MAX_MPS),
	0x00                                            /* bInterval */
};
#endif

/* USB CDC NCM Full Speed Configuration Descriptor */
static const u8 usbd_cdc_ncm_fs_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	0x00, 0x00,                                     /* wTotalLength (calculated later) */
	0x02,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
#if USBD_CDC_NCM_SELF_POWERED
	0xE0,                                           /* bmAttributes: Self-powered, Remote wakeup */
#else
	0xA0,                                           /* bmAttributes: Bus-powered, Remote wakeup */
#endif
	0x32,                                           /* bMaxPower: 100mA */

	/* Communication Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_NCM_COMM_INTERFACE_NUM,                /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x01,                                           /* bNumEndpoints */
	USB_CDC_NCM_COMM_INTERFACE_CLASS_CODE,              /* bInterfaceClass: CDC */
	USB_CDC_NCM_SUBCLASS_CODE,                           /* bInterfaceSubClass: NCM */
	0x00,                                           /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* CDC Header Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	USB_CDC_NCM_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_NCM_FUNC_DESC_HEADER,                       /* bDescriptorSubtype */
	0x10, 0x01,                                     /* bcdCDC: 1.10 */

	/* CDC Union Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	USB_CDC_NCM_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_NCM_FUNC_DESC_UNION,                        /* bDescriptorSubtype */
	USBD_CDC_NCM_COMM_INTERFACE_NUM,                /* bControlInterface */
	USBD_CDC_NCM_DATA_INTERFACE_NUM,                /* bSubordinateInterface0 */

	/* CDC Ethernet Networking Functional Descriptor */
	USBD_CDC_NCM_ETHERNET_FUNC_DESC_SIZE,           /* bFunctionLength */
	USB_CDC_NCM_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_NCM_FUNC_DESC_ETHERNET_NETWORKING,          /* bDescriptorSubtype */
	USBD_CDC_NCM_MAC_STRING_INDEX,                  /* iMACAddress */
	0x00, 0x00, 0x00, 0x00,                         /* bmEthernetStatistics */
	USB_LOW_BYTE(USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE), /* wMaxSegmentSize */
	USB_HIGH_BYTE(USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE),
	0x00, 0x00,                                     /* wNumberMCFilters */
	0x00,                                           /* bNumberPowerFilters */

	/* CDC NCM Functional Descriptor */
	USBD_CDC_NCM_FUNC_DESC_SIZE,                    /* bFunctionLength: 6 */
	USB_CDC_NCM_CS_INTERFACE,                           /* bDescriptorType: CS_INTERFACE (0x24) */
	USB_CDC_NCM_FUNC_DESC,                          /* bDescriptorSubtype: NCM (0x1A) */
	0x00, 0x01,                                     /* bcdNcmVersion: 1.00 */
	USB_CDC_NCM_NTB16_SUPPORTED,                    /* bmNetworkCapabilities: NTB16 supported */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_CDC_NCM_INTR_IN_EP,                        /* bEndpointAddress */
	USB_CH_EP_TYPE_INTR,                            /* bmAttributes */
	USB_LOW_BYTE(USBD_CDC_NCM_INTR_IN_PACKET_SIZE),  /* wMaxPacketSize */
	USB_HIGH_BYTE(USBD_CDC_NCM_INTR_IN_PACKET_SIZE),
	USBD_CDC_NCM_FS_INTR_IN_INTERVAL,               /* bInterval */

	/* Data Interface Descriptor (Alt 0 - No endpoints) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_NCM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x00,                                           /* bNumEndpoints */
	USB_CDC_NCM_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	0x00,                                           /* bInterfaceSubClass */
	USB_CDC_NCM_DATA_PROTOCOL_NETWORK_TRANSFER_BLOCK,   /* bInterfaceProtocol: NCM */
	0x00,                                           /* iInterface */

	/* Data Interface Descriptor (Alt 1 - Active) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_NCM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x01,                                           /* bAlternateSetting */
	0x02,                                           /* bNumEndpoints */
	USB_CDC_NCM_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	0x00,                                           /* bInterfaceSubClass */
	USB_CDC_NCM_DATA_PROTOCOL_NETWORK_TRANSFER_BLOCK,   /* bInterfaceProtocol: NCM */
	0x00,                                           /* iInterface */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_CDC_NCM_BULK_IN_EP,                        /* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes */
	USB_LOW_BYTE(USB_BULK_FS_MAX_MPS),              /* wMaxPacketSize */
	USB_HIGH_BYTE(USB_BULK_FS_MAX_MPS),
	0x00,                                           /* bInterval */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_CDC_NCM_BULK_OUT_EP,                       /* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes */
	USB_LOW_BYTE(USB_BULK_FS_MAX_MPS),              /* wMaxPacketSize */
	USB_HIGH_BYTE(USB_BULK_FS_MAX_MPS),
	0x00                                            /* bInterval */
};

/* CDC NCM Class Driver */
static const usbd_class_driver_t usbd_cdc_ncm_driver = {
	.get_descriptor = usbd_cdc_ncm_get_descriptor,
	.set_config = usbd_cdc_ncm_set_config,
	.clear_config = usbd_cdc_ncm_clear_config,
	.setup = usbd_cdc_ncm_setup,
	.ep0_data_out = usbd_cdc_ncm_handle_ep0_data_out,
	.ep_data_in = usbd_cdc_ncm_handle_ep_data_in,
	.ep_data_out = usbd_cdc_ncm_handle_ep_data_out,
	.sof = usbd_cdc_ncm_sof,
	.status_changed = usbd_cdc_ncm_status_changed,
};

/* CDC NCM Device Instance */
static usbd_cdc_ncm_dev_t usbd_cdc_ncm_dev;

/* Private functions ---------------------------------------------------------*/

/* ============================================================================
 * SPSC TX slot helpers
 * ----------------------------------------------------------------------------
 * Each ncm_tx_ntb_t slot owns its own DMA-aligned NTB scratch buffer.  The
 * single producer (lwIP task, or TX aggregation task) fills slot[tx_wd]
 * and advances tx_wd; the single consumer (USB ISR) sends slot[tx_rd] and
 * advances tx_rd after XFRC.
 *
 * tx_wd/tx_rd index isolation provides mutual exclusion between producer and
 * consumer.  usbd_cdc_ncm_tx_kick() is called only from USB ISR context (SOF and
 * XFRC, which are serialised), so no locking is needed anywhere.
 * ==========================================================================*/

/**
 * @brief Begin a new aggregated NTB16 in the given slot.
 * @param slot:     Ping-pong slot to initialise (caller has reserved it).
 * @param sequence: NTB sequence number for this block.
 */
static void usbd_cdc_ncm_agg_begin(ncm_tx_ntb_t *slot, u16 sequence)
{
	usb_cdc_ncm_nth16_t *nth16 = (usb_cdc_ncm_nth16_t *)slot->buf;

	nth16->dwSignature = USB_CDC_NCM_NTH16_SIGNATURE;
	nth16->wHeaderLength = USB_CDC_NCM_NTH16_LENGTH;
	nth16->wSequence = sequence;
	nth16->wBlockLength = 0; /* Finalised by usbd_cdc_ncm_agg_finalize() */
	nth16->wFpIndex = (u16)USBD_CDC_NCM_AGG_NDP_OFFSET;

	slot->sequence = sequence;
	slot->frame_count = 0;
	slot->data_offset = USBD_CDC_NCM_AGG_DATA_OFFSET;
}

/**
 * @brief Append an ethernet frame to the slot's aggregated NTB.
 * @param slot:      Ping-pong slot currently being filled.
 * @param frame:     Ethernet frame data.
 * @param frame_len: Frame length.
 * @retval HAL_OK on success, HAL_ERR_PARA if buffer full (caller must flush first).
 */
static int usbd_cdc_ncm_agg_append(ncm_tx_ntb_t *slot, u8 *frame, u32 frame_len)
{
	usb_cdc_ncm_ndp16_entry_t *entry;
	u32 aligned_frame_len;

	if (slot->frame_count >= USBD_CDC_NCM_NTB_OUT_MAX_DATAGRAMS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "AGG: max frames reached\n");
		return HAL_ERR_PARA;
	}

	/* Align frame length to 4 bytes */
	aligned_frame_len = (frame_len + USB_CDC_NCM_DATAGRAM_ALIGN - 1) & ~(USB_CDC_NCM_DATAGRAM_ALIGN - 1);

	/* Check if frame fits in remaining buffer */
	if (slot->data_offset + aligned_frame_len > USBD_CDC_NCM_NTB_TX_BUF_SIZE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "AGG: buffer full\n");
		return HAL_ERR_PARA;
	}

	/* Write NDP entry at: NDP_header_start + 8 + frame_count * 4 */
	entry = (usb_cdc_ncm_ndp16_entry_t *)
			(slot->buf + USBD_CDC_NCM_AGG_NDP_OFFSET + 8 + slot->frame_count * 4);
	entry->wDatagramIndex = (u16)slot->data_offset;
	entry->wDatagramLength = (u16)frame_len;

	/* Copy frame data */
	usb_os_memcpy(slot->buf + slot->data_offset, frame, frame_len);

	/* Zero-pad to alignment boundary */
	if (aligned_frame_len > frame_len) {
		usb_os_memset(slot->buf + slot->data_offset + frame_len,
					  0, aligned_frame_len - frame_len);
	}

	slot->frame_count++;
	slot->data_offset += aligned_frame_len;

	return HAL_OK;
}

/**
 * @brief Finalise the slot's NTB headers (terminator, NDP length, NTH wBlockLength).
 *        After this returns, slot->buf is ready to be handed to usbd_cdc_ncm_bulk_send().
 *        Caller must transition the slot state from FILLING to READY/IN_FLIGHT.
 * @param slot: Slot whose aggregation cursor is fully written.
 */
static void usbd_cdc_ncm_agg_finalize(ncm_tx_ntb_t *slot)
{
	usb_cdc_ncm_nth16_t *nth16 = (usb_cdc_ncm_nth16_t *)slot->buf;
	usb_cdc_ncm_ndp16_t *ndp16;
	usb_cdc_ncm_ndp16_entry_t *term;
	u32 ndp_entry_count;

	ndp16 = (usb_cdc_ncm_ndp16_t *)(slot->buf + USBD_CDC_NCM_AGG_NDP_OFFSET);
	ndp_entry_count = slot->frame_count + 1U; /* +1 for terminator */

	/* Write terminator entry immediately after the last real entry. */
	term = (usb_cdc_ncm_ndp16_entry_t *)
		   (slot->buf + USBD_CDC_NCM_AGG_NDP_OFFSET + 8 + slot->frame_count * 4);
	term->wDatagramIndex = 0;
	term->wDatagramLength = 0;

	/* Finalize NDP header */
	ndp16->dwSignature = USB_CDC_NCM_NDP16_NOCRC_SIGNATURE;
	ndp16->wLength = 8 + (u16)(ndp_entry_count * 4);
	ndp16->wNextFpIndex = 0;

	/* Finalize NTH16 block length */
	nth16->wBlockLength = (u16)slot->data_offset;
}

/**
 * @brief Return non-zero if the TX ring is full (producer cannot enqueue).
 *        SPSC: lwIP producer reads tx_rd (consumer-owned) only to test fullness.
 *        One empty gap is kept between wd and rd, so usable depth = DEPTH-1.
 * @param ncm: Device instance.
 */
static inline int usbd_cdc_ncm_tx_ring_full(usbd_cdc_ncm_dev_t *ncm)
{
	u8 next_wd = (u8)((ncm->tx_wd + 1U) % USBD_CDC_NCM_TX_DEPTH);
	return (next_wd == ncm->tx_rd) ? 1 : 0;
}

/**
 * @brief Return non-zero if the TX ring is empty (nothing to send).
 * @param ncm: Device instance.
 */
static inline int usbd_cdc_ncm_tx_ring_empty(usbd_cdc_ncm_dev_t *ncm)
{
	return (ncm->tx_wd == ncm->tx_rd) ? 1 : 0;
}

/**
 * @brief Consumer: kick the next queued NTB into the BULK IN endpoint.
 *
 *        SPSC consumer side.  Runs in USB ISR context only (XFRC chain) or in
 *        the producer's "endpoint was idle" fast-path -- never both at once
 *        because tx_inflight gates it: if a transfer is already in flight we
 *        return immediately and the XFRC ISR will chain the next one.
 *
 *        SOF and XFRC are both USB-interrupt context and run serially, so no
 *        lock is needed against each other.  Against the producer, tx_inflight
 *        + the wd/rd gap provide isolation: the consumer only ever reads
 *        slot[tx_rd], which the producer never touches once it has advanced
 *        tx_wd past it.
 *
 * @param ncm: Device instance.
 */
static void usbd_cdc_ncm_tx_kick(usbd_cdc_ncm_dev_t *ncm)
{
	ncm_tx_ntb_t *slot;

	/* Already transmitting, or nothing queued -> nothing to do.
	 * Called only from USB ISR context (SOF and XFRC), which are
	 * serialised -- no lock needed. */
	if (ncm->tx_inflight || usbd_cdc_ncm_tx_ring_empty(ncm)) {
		return;
	}

	slot = &ncm->tx_slot[ncm->tx_rd];
	ncm->tx_inflight = 1U;
	__sync_synchronize();

	if (usbd_cdc_ncm_bulk_send(slot->buf, slot->data_offset) != HAL_OK) {
		/* Submit failed before usbd_ep_transmit() succeeded: no XFRC will
		 * fire.  Drop this NTB, advance rd, clear inflight, and wake the
		 * producer in case the ring was full. */
		ncm->tx_inflight = 0U;
#ifndef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
		ncm->tx_filling_busy = 0U;
		ncm->tx_wd_tick = 0U;
#endif
		ncm->tx_rd = (u8)((ncm->tx_rd + 1U) % USBD_CDC_NCM_TX_DEPTH);
		__sync_synchronize();
		usb_os_sema_give(ncm->tx_buf_free_sema);
	}
}

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
/**
 * @brief TX aggregation task.
 *
 * Dequeues raw ethernet frames from tx_raw_rb, aggregates up to
 * USBD_CDC_NCM_NTB_OUT_MAX_DATAGRAMS frames into one NTB slot, then
 * publishes it to the SPSC ring (tx_wd++) for the USB ISR to transmit.
 *
 * This task is the sole writer of slot[tx_wd] -- no critical section needed
 * against the USB ISR because the ISR only reads slot[tx_rd].
 */
static void usbd_cdc_ncm_tx_task(void *param)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	u8 *frame_buf;
	u32 frame_len;

	UNUSED(param);

	/* Allocate frame buffer on heap so the task stack stays small.
	 * Free on task exit. */
	frame_buf = (u8 *)usb_os_malloc(USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE);
	if (frame_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "TX task: frame buf alloc fail\n");
		ncm->tx_task = NULL;
		rtos_task_delete(NULL);
		return;
	}

	while (ncm->tx_task_running) {
		/* Wait for at least one frame or timeout (to catch missed signals). */
		usb_os_sema_take(ncm->tx_raw_sema, USBD_CDC_NCM_TX_SEMA_TIMEOUT_MS);

		if (!ncm->tx_task_running) {
			break;
		}

		/* Drain as many frames as we can from the raw ring buffer. */
		while (ncm->connect_status &&
			   !usb_ringbuf_is_empty(&ncm->tx_raw_rb)) {

			/* Wait for NTB ring to have room for a new slot. */
			while (usbd_cdc_ncm_tx_ring_full(ncm)) {
				if (!ncm->tx_task_running || !ncm->connect_status) {
					goto task_exit;
				}
				usb_os_sema_take(ncm->tx_buf_free_sema,
								 USBD_CDC_NCM_BULK_TX_TIMEOUT_MS);
			}

			/* Start a new NTB slot. */
			ncm_tx_ntb_t *slot = &ncm->tx_slot[ncm->tx_wd];
			usbd_cdc_ncm_agg_begin(slot, ncm->sequence);

			/* Aggregate frames into this slot until full or raw rb empty. */
			while (slot->frame_count < USBD_CDC_NCM_NTB_OUT_MAX_DATAGRAMS) {
				frame_len = usb_ringbuf_remove_head(&ncm->tx_raw_rb,
													frame_buf,
													USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE,
													NULL);
				if (frame_len == 0U) {
					break;
				}

				if (usbd_cdc_ncm_agg_append(slot, frame_buf, frame_len) != HAL_OK) {
					break;
				}

				usb_os_sema_give(ncm->tx_raw_sema);
			}

			if (slot->frame_count == 0U) {
				continue;
			}

			usbd_cdc_ncm_agg_finalize(slot);
			ncm->sequence++;
			__sync_synchronize();
			ncm->tx_wd = (u8)((ncm->tx_wd + 1U) % USBD_CDC_NCM_TX_DEPTH);
			__sync_synchronize();
		}
	}

task_exit:
	usb_os_mfree(frame_buf);
	ncm->tx_task = NULL;
	rtos_task_delete(NULL);
}

static void usbd_cdc_ncm_tx_task_init(void)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	int ret;

	if (ncm->tx_task_running) {
		return;
	}
	ncm->tx_task_running = 1U;
	ret = rtos_task_create(&ncm->tx_task, "usbd_cdc_ncm_tx",
						   usbd_cdc_ncm_tx_task, NULL,
						   USBD_CDC_NCM_TX_TASK_STACK_SIZE,
						   USBD_CDC_NCM_TX_TASK_PRIORITY);
	if (ret != HAL_OK) {
		ncm->tx_task_running = 0U;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create TX task fail\n");
	}
}

static void usbd_cdc_ncm_tx_task_deinit(void)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;

	ncm->tx_task_running = 0U;
	/* Wake the task so it sees the flag and exits. */
	if (ncm->tx_raw_sema != NULL) {
		usb_os_sema_give(ncm->tx_raw_sema);
	}
	/* Also give tx_buf_free_sema in case it is blocked waiting for NTB room. */
	if (ncm->tx_buf_free_sema != NULL) {
		usb_os_sema_give(ncm->tx_buf_free_sema);
	}
	while (ncm->tx_task != NULL) {
		usb_os_sleep_ms(10);
	}
}
#endif /* CONFIG_USBD_CDC_NCM_TX_AGGREGATION */

/**
 * @brief Convert a nibble (0-15) to its ASCII hex character.
 * @param value: Nibble value (0x0-0xF).
 * @retval ASCII character '0'-'9' or 'A'-'F'. Returns '0' for out-of-range input.
 */
static inline u8 usbd_cdc_ncm_char_to_hex(u8 value)
{
	if (value <= 0x9) {
		return 0x30 + value;
	} else if (value <= 0xF) {
		return 0x41 + (value - 0xA);
	} else {
		return 0x30;
	}
}

/**
 * @brief Convert a 6-byte MAC address to a 12-character uppercase hex string.
 * @param mac:     Input MAC address buffer (6 bytes).
 * @param mac_str: Output null-terminated string (must be at least 13 bytes).
 */
static void usbd_cdc_ncm_mac_to_string(const u8 *mac, char *mac_str)
{
	u8 str_index = 0;
	u8 i;

	for (i = 0; i < 6; i++) {
		/* high 4 bits */
		mac_str[str_index++] = usbd_cdc_ncm_char_to_hex((mac[i] >> 4) & 0x0F);

		/* low 4 bits */
		mac_str[str_index++] = usbd_cdc_ncm_char_to_hex(mac[i] & 0x0F);
	}

	mac_str[str_index] = '\0';
}

/**
 * @brief Set the device MAC address from an external source.
 * @param mac: Pointer to a 6-byte MAC address buffer. NULL is ignored.
 */
static void usbd_cdc_ncm_set_mac(const u8 *mac)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;

	if (NULL == mac) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "No Param\n");
		return;
	}

	memcpy((void *) & (ncm->mac[0]), (void *)mac, USBD_CDC_NCM_MAC_STR_LEN);
}

/**
 * @brief Process a received NTB and extract ethernet frames to deliver to upper layer.
 * @param buf:    Pointer to the received data buffer containing an NTB.
 * @param length: Length of the received data in bytes.
 */
static int usbd_cdc_ncm_bulk_receive(u8 *buf, u32 length)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usb_cdc_ncm_nth16_t *nth16;
	usb_cdc_ncm_ndp16_t *ndp16;
	u32 ndp_offset;
	u32 entry_count;
	u32 i;
	u32 datagram_index;
	u32 datagram_length;
#if USBD_CDC_NCM_RX_SPEED_CHECK
	static u64 usb_rx_start_time = 0, usb_rx_end_time, usb_rx_interval_time;
	static u64 usb_rx_total_len = 0;

	usb_rx_total_len += length;
	if (usb_rx_start_time == 0) {
		usb_rx_start_time = usb_os_get_timestamp_ms();
	}
	usb_rx_end_time = usb_os_get_timestamp_ms();
	usb_rx_interval_time = (usb_rx_end_time - usb_rx_start_time) * RTOS_TICK_RATE_MS;

	if (usb_rx_interval_time >= 2000) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Heap %d RX %dB in %d ms, %d Kbps\n",
				 rtos_mem_get_free_heap_size(),
				 (u32)usb_rx_total_len, (u32)usb_rx_interval_time, (u32)((usb_rx_total_len * 8 * 1000) / (usb_rx_interval_time * 1024)));
		usb_rx_start_time = usb_rx_end_time;
		usb_rx_total_len = 0;
	}
#endif

	if (length < sizeof(usb_cdc_ncm_nth16_t)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "RX too short for NTH16: %d\n", length);
		return HAL_ERR_PARA;
	}

	nth16 = (usb_cdc_ncm_nth16_t *)buf;

	/* Verify NTH16 signature */
	if (nth16->dwSignature != USB_CDC_NCM_NTH16_SIGNATURE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad NTH16 sig: 0x%08X\n", nth16->dwSignature);
		return HAL_ERR_PARA;
	}

	/* Walk the NDP chain (wFpIndex -> wNextFpIndex, 0 terminates). */
	ndp_offset = nth16->wFpIndex;
	while (ndp_offset != 0U) {
		/* Check that this NDP header fits within the received data. */
		if (((u32)ndp_offset + sizeof(usb_cdc_ncm_ndp16_t)) > length) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "NDP beyond RX len: off=%d, len=%d\n",
					 ndp_offset, length);
			break;
		}

		ndp16 = (usb_cdc_ncm_ndp16_t *)(buf + ndp_offset);

		/* Verify NDP16 signature */
		if (ndp16->dwSignature != USB_CDC_NCM_NDP16_NOCRC_SIGNATURE) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad NDP16 sig: 0x%08X\n", ndp16->dwSignature);
			break;
		}

		/* Validate the NDP itself lies fully within the received buffer before
		 * trusting wLength. wLength must cover at least the 8-byte NDP header,
		 * and the whole NDP (header + entry array) must fit within the received
		 * data, otherwise reading aEntry[i] below would read past the buffer. */
		if ((ndp16->wLength < (USB_CDC_NCM_NDP16_MIN_LENGTH)) ||
			(((u32)ndp_offset + ndp16->wLength) > length)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad NDP len: wLength=%d, off=%d, rx=%d\n",
					 ndp16->wLength, ndp_offset, length);
			break;
		}

		/* Calculate number of datagram entries
		 * = (wLength - 8-byte header) / 4 bytes per entry */
		entry_count = (ndp16->wLength - 8U) / 4U;

		/* Iterate over entries and forward each ethernet frame */
		for (i = 0U; i < entry_count; i++) {
			datagram_index = ndp16->aEntry[i].wDatagramIndex;
			datagram_length = ndp16->aEntry[i].wDatagramLength;

			/* Terminator entry */
			if ((datagram_index == 0U) || (datagram_length == 0U)) {
				break;
			}

			/* Validate datagram is within the received buffer */
			if (((u32)datagram_index + (u32)datagram_length) > length) {
				RTK_LOGS(TAG, RTK_LOG_ERROR,
						 "Datagram %d beyond RX: idx=%d+len=%d > %d\n",
						 i, datagram_index, datagram_length, length);
				break;
			}

			if ((ncm->cb != NULL) && (ncm->cb->received != NULL)) {
				ncm->cb->received(buf + datagram_index, datagram_length);
			}
		}

		/* Advance to the next NDP in the chain (0 = last). */
		ndp_offset = ndp16->wNextFpIndex;
	}

	return HAL_OK;
}

/**
 * @brief Send the next pending INTR IN notification and advance the state machine.
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @retval HAL_OK on success, HAL_ERR_PARA if no notification is pending,
 *         or a HAL error code if the transfer could not be submitted.
 */
static int usbd_cdc_ncm_send_notification(void)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usb_cdc_ncm_notify_t event;
	usb_cdc_ncm_speed_change_data_t speed_data;
	u8 notify_buf[16];
	int status;
	u16 length;
	u8 next_state;

	event.bmRequestType = 0xA1;
	/* Class notifications are emitted on the communication (control) interface's
	 * notification endpoint, so wIndex carries the communication interface number. */
	event.wIndex = USBD_CDC_NCM_COMM_INTERFACE_NUM;

	switch (ncm->notify_state) {
	case NCM_NOTIFY_CONNECT:
		event.bNotificationCode = USB_CDC_NCM_NOTIFY_NETWORK_CONNECTION;
		event.wValue = ncm->connect_status;
		event.wLength = 0;
		length = USBD_CDC_NCM_NETWORK_CONNECTION_SIZE;
		/* Follow a "connected" notification with a speed-change report; a
		 * "disconnected" notification stands alone (no trailing SPEED). */
		next_state = ncm->connect_status ? NCM_NOTIFY_SPEED : NCM_NOTIFY_NONE;
		usb_os_memcpy(notify_buf, &event, sizeof(event));
		break;

	case NCM_NOTIFY_SPEED:
		event.bNotificationCode = USB_CDC_NCM_NOTIFY_CONNECTION_SPEED_CHANGE;
		event.wValue = 0;
		event.wLength = 8;
		speed_data.DLBitRate = 0; /* Downstream bits/sec */
		speed_data.ULBitRate = 0; /* Upstream bits/sec */
		length = USBD_CDC_NCM_CONNECTION_SPEED_CHANGE_SIZE;
		next_state = NCM_NOTIFY_NONE;
		usb_os_memcpy(notify_buf, &event, sizeof(event));
		usb_os_memcpy(notify_buf + sizeof(event), &speed_data, sizeof(speed_data));
		break;

	case NCM_NOTIFY_NONE:
	default:
		return HAL_OK;
	}

	status = usbd_cdc_ncm_intr_in_send(&notify_buf, length);
	if (status == HAL_OK) {
		/* Advance state only on success; retry will re-send the same notification. */
		ncm->notify_state = next_state;
	}

	return status;
}

/**
 * @brief Transmit INTR IN notification
 * @param data: Notification data
 * @param len: Data length
 * @retval Status
 */
static int usbd_cdc_ncm_intr_in_send(void *data, u16 len)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usb_dev_t *dev = ncm->dev;
	usbd_ep_t *ep_intr_in = &ncm->ep_intr_in;
	int ret = HAL_ERR_HW;

	if (!dev->is_ready) {
		return ret;
	}

	if (len > USBD_CDC_NCM_INTR_IN_PACKET_SIZE) {
		len = USBD_CDC_NCM_INTR_IN_PACKET_SIZE;
	}

	if (ep_intr_in->xfer_state == 0U) {
		ep_intr_in->is_busy = 1U;
		ep_intr_in->xfer_state = 1U;

		if (dev->is_ready) {
			usb_os_memcpy((void *)ep_intr_in->xfer_buf, (void *)data, len);
			ep_intr_in->xfer_len = len;
			ret = usbd_ep_transmit(dev, ep_intr_in);
			if (ret != HAL_OK) {
				/* Transfer never started - no XFRC will fire to clear xfer_state.
				 * Reset it here, otherwise the INTR IN path wedges permanently. */
				ep_intr_in->xfer_state = 0U;
			}
		} else {
			ep_intr_in->xfer_state = 0U;
		}

		ep_intr_in->is_busy = 0U;
	} else {
		/* TX busy */
		ret = HAL_BUSY;
	}

	return ret;
}

/**
 * @brief Transmit data via BULK IN endpoint
 * @param buf: Data buffer
 * @param len: Data length
 * @retval Status
 */
static int usbd_cdc_ncm_bulk_send(u8 *buf, u32 len)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usbd_ep_t *ep_bulk_in = &ncm->ep_bulk_in;
	usb_dev_t *dev = ncm->dev;
	int ret = HAL_ERR_HW;

	if (!dev->is_ready) {
		return ret;
	}

	if (buf == NULL || len == 0) {
		return HAL_ERR_PARA;
	}

	if (ep_bulk_in->xfer_state == 0U) {
		ep_bulk_in->is_busy = 1U;
		ep_bulk_in->xfer_state = 1U;

		ep_bulk_in->xfer_buf = buf;

		/* Transmit */
		if (dev->is_ready) {
			ep_bulk_in->xfer_len = len;
			ret = usbd_ep_transmit(dev, ep_bulk_in);
			if (ret != HAL_OK) {
				/* Transfer never started - no XFRC will fire to clear xfer_state.
				 * Reset it here, otherwise the BULK IN path wedges permanently. */
				ep_bulk_in->xfer_state = 0U;
			}
		} else {
			ep_bulk_in->xfer_state = 0U;
		}

		ep_bulk_in->is_busy = 0U;
	} else {
		ret = HAL_BUSY;
	}

	return ret;
}

/**
 * @brief Set CDC NCM configuration
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @param config: Configuration number
 * @retval Status
 */
static int usbd_cdc_ncm_set_config(usb_dev_t *dev, u8 config)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usbd_ep_t *ep_bulk_in = &ncm->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &ncm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ncm->ep_intr_in;
	usb_ep_info_t *info;
	UNUSED(config);

	ncm->dev = dev;

	/* Initialize INTR IN endpoint */
	ep_intr_in->xfer_state = 0U;
	info = &ep_intr_in->info;
	info->mps = USBD_CDC_NCM_INTR_IN_PACKET_SIZE;
	usbd_ep_init(dev, ep_intr_in);

	/* Initialize BULK IN endpoint */
	ep_bulk_in->xfer_state = 0U;
	info = &ep_bulk_in->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USB_BULK_HS_MAX_MPS : USB_BULK_FS_MAX_MPS;
	usbd_ep_init(dev, ep_bulk_in);

	/* Initialize BULK OUT endpoint */
	info = &ep_bulk_out->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USB_BULK_HS_MAX_MPS : USB_BULK_FS_MAX_MPS;
	usbd_ep_init(dev, ep_bulk_out);

	/* Reset the RX state machine so a re-enumeration (clear->set config) starts
	 * from a clean buffer hand-off state rather than inheriting stale indices. */
	ncm->rx_buf_free = 1U;
	ncm->rx_pending_len = 0U;
	ncm->rx_xfer_idx = 0U;
	ep_bulk_out->xfer_buf = ncm->rx_buf[ncm->rx_xfer_idx];

	/* Start receiving */
	usbd_ep_receive(dev, ep_bulk_out);

	return HAL_OK;
}

/**
 * @brief Clear CDC NCM configuration
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @param config: Configuration number
 * @retval Status
 */
static int usbd_cdc_ncm_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;

	UNUSED(config);

	/* Deinitialize all endpoints */
	usbd_ep_deinit(dev, &ncm->ep_bulk_in);
	usbd_ep_deinit(dev, &ncm->ep_bulk_out);
	usbd_ep_deinit(dev, &ncm->ep_intr_in);

	/* Discard any queued NTBs and reset the endpoint state so the next
	 * set_config starts with a clean TX path. */
	{
		u8 i;
		/* Reset the SPSC TX ring: drop any queued NTBs and reset indices.
		 * Buffers are kept allocated (init/deinit owns the lifetime). */
		for (i = 0U; i < USBD_CDC_NCM_TX_DEPTH; i++) {
			ncm->tx_slot[i].frame_count = 0;
			ncm->tx_slot[i].data_offset = 0;
		}
		ncm->tx_wd = 0;
		ncm->tx_rd = 0;
		ncm->tx_inflight = 0;
#ifndef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
		ncm->tx_filling_busy = 0U;
		ncm->tx_wd_tick = 0U;
#endif
		__sync_synchronize();
		ncm->ep_bulk_in.xfer_state = 0U;
		/* Unblock any transmit() call that is waiting on a free slot. */
		if (ncm->tx_buf_free_sema != NULL) {
			usb_os_sema_give(ncm->tx_buf_free_sema);
		}
	}

	/* The data path is gone: clear the link state and abandon any in-flight
	 * notification sequence so the next SET_INTERFACE re-reports from scratch
	 * (mirrors the detach reset in usbd_cdc_ncm_status_changed). */
	ncm->connect_status = 0;
	ncm->notify_state = NCM_NOTIFY_NONE;
	ncm->notify_retry = 0U;

	return HAL_OK;
}

/**
 * @brief Handle CDC NCM SETUP requests
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @param req: SETUP request
 * @retval Status
 */
static int usbd_cdc_ncm_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	int ret = HAL_OK;
	u8 req_type = req->bmRequestType & USB_REQ_TYPE_MASK;
	u8 interface_id;
	u16 wValue;
	usb_cdc_ncm_ntb_parameters_t ntb_params;

	switch (req_type) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				interface_id = USB_LOW_BYTE(req->wIndex);
				ncm->alt_setting = USB_LOW_BYTE(req->wValue);

				if (interface_id == USBD_CDC_NCM_DATA_INTERFACE_NUM) {
					/* The NCM data interface is dual-alt: alt 0 has no endpoints
					 * (idle), alt 1 carries the bulk IN/OUT endpoints (active).
					 * Only alt 1 means the host has armed the data path and will
					 * poll the bulk IN endpoint, so gate connect_status on it.
					 * Reporting "connected" on alt 0 would let the upper layer
					 * bring up the netif and issue a blocking IN transfer that
					 * the host never drains, wedging the network thread. */
					if (ncm->alt_setting != 0U) {
						/* Data path active: report link state once.  If the INTR IN
						 * endpoint is momentarily busy the send fails here; notify_retry
						 * lets the SOF handler re-send so the notification is never lost. */
						ncm->notify_state = NCM_NOTIFY_CONNECT;
						ncm->connect_status = 1;
						if (usbd_cdc_ncm_send_notification() != HAL_OK) {
							ncm->notify_retry = 1U;
						}
					} else {
						/* alt 0: data path torn down, endpoints gone */
						ncm->connect_status = 0;
					}
				}

				if (ncm->cb && ncm->cb->setup) {
					ncm->cb->setup(req, NULL);
				}
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = ncm->alt_setting;
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
			ret = HAL_ERR_PARA;
			break;
		}
		break;

	case USB_REQ_TYPE_CLASS:
		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
			/* Device-to-Host: prepare response data in EP0 buffer */
			switch (req->bRequest) {
			case USB_CDC_NCM_GET_NTB_PARAMETERS:
				ntb_params.wLength = sizeof(usb_cdc_ncm_ntb_parameters_t);
				ntb_params.bmNtbFormatsSupported = USB_CDC_NCM_NTB16_SUPPORTED;
				ntb_params.dwNtbInMaxSize = ncm->ntb_in_max_size;
				ntb_params.wNdbInDivisor = USBD_CDC_NCM_NTB_DIVISOR;
				ntb_params.wNdbInPayloadRemainder = USBD_CDC_NCM_NTB_PAYLOAD_REMAINDER;
				ntb_params.wNdbInAlignment = USBD_CDC_NCM_NTB_ALIGNMENT;
				ntb_params.dwNtbOutMaxSize = ncm->ntb_out_max_size;
				ntb_params.wNdbOutDivisor = USBD_CDC_NCM_NTB_DIVISOR;
				ntb_params.wNdbOutPayloadRemainder = USBD_CDC_NCM_NTB_PAYLOAD_REMAINDER;
				ntb_params.wNdbOutAlignment = USBD_CDC_NCM_NTB_ALIGNMENT;
				ntb_params.wNtbOutMaxDatagrams = USBD_CDC_NCM_NTB_OUT_MAX_DATAGRAMS;
				ntb_params.wReserved1 = 0;
				usb_os_memcpy(ep0_in->xfer_buf, &ntb_params, sizeof(ntb_params));
				ep0_in->xfer_len = sizeof(ntb_params);
				usbd_ep_transmit(dev, ep0_in);
				break;

			case USB_CDC_NCM_GET_NTB_FORMAT:
				/* wNtbFormatSelected: 16-bit value */
				ep0_in->xfer_buf[0] = ncm->ntb_format;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				usbd_ep_transmit(dev, ep0_in);
				break;

			case USB_CDC_NCM_GET_NTB_INPUT_SIZE:
				/* dwNtbInMaxSize: 32-bit value */
				ep0_in->xfer_buf[0] = USB_LOW_BYTE(ncm->ntb_in_max_size);
				ep0_in->xfer_buf[1] = USB_HIGH_BYTE(ncm->ntb_in_max_size);
				ep0_in->xfer_buf[2] = 0U;
				ep0_in->xfer_buf[3] = 0U;
				ep0_in->xfer_len = 4U;
				usbd_ep_transmit(dev, ep0_in);
				break;


			case USB_CDC_NCM_GET_MAX_DATAGRAM_SIZE:
				ep0_in->xfer_buf[0] = USB_LOW_BYTE(USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE);
				ep0_in->xfer_buf[1] = USB_HIGH_BYTE(USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE);
				ep0_in->xfer_len = 2U;
				usbd_ep_transmit(dev, ep0_in);
				break;

			default:
				if (ncm->cb && ncm->cb->setup) {
					ret = ncm->cb->setup(req, ep0_in->xfer_buf);
					if (ret == HAL_OK) {
						ep0_in->xfer_len = req->wLength;
						usbd_ep_transmit(dev, ep0_in);
					}
				}
				break;
			}
		} else {
			/* Host-to-Device */
			wValue = req->wValue;

			switch (req->bRequest) {
			case USB_CDC_NCM_SET_NTB_FORMAT:
				/* wValue: 0=NTB16, 1=NTB32 */
				if (wValue == 0) {
					ncm->ntb_format = 0;
					ret = HAL_OK;
				} else {
					/* Only NTB16 supported */
					ret = HAL_ERR_PARA;
				}
				break;

			case USB_CDC_NCM_SET_NTB_INPUT_SIZE:
				/* Per CDC-NCM, the 4-byte dwNtbInMaxSize is carried in the DATA
				 * stage (wValue must be 0), not in wValue. Stash the request and
				 * arm EP0 OUT; the payload is validated/applied in
				 * usbd_cdc_ncm_handle_ep0_data_out(). */
				if (req->wLength > 0U) {
					usb_os_memcpy((void *)&ncm->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
					ep0_out->xfer_len = req->wLength;
					usbd_ep_receive(dev, ep0_out);
				} else {
					ret = HAL_ERR_PARA;
				}
				break;


			case USB_CDC_NCM_SET_MAX_DATAGRAM_SIZE:
				/* Accept any reasonable datagram size */
				ret = HAL_OK;
				break;

			case USB_CDC_NCM_SET_CRC_MODE:
				/* Only no-CRC (mode 0) supported */
				if (wValue == USB_CDC_NCM_CRC_MODE_NONE) {
					ncm->crc_mode = 0;
					ret = HAL_OK;
				} else {
					ret = HAL_ERR_PARA;
				}
				break;

			default:
				/* Forward to upper layer with data stage if needed */
				if (req->wLength > 0) {
					usb_os_memcpy((void *)&ncm->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
					ep0_out->xfer_len = req->wLength;
					usbd_ep_receive(dev, ep0_out);
				} else {
					if (ncm->cb && ncm->cb->setup) {
						ret = ncm->cb->setup(req, NULL);
					}
				}
				break;
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
 * @brief Handle EP data IN completion
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @param ep_addr: Endpoint address
 * @param status: Transfer status
 * @retval Status
 */
static int usbd_cdc_ncm_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usbd_ep_t *ep_bulk_in = &ncm->ep_bulk_in;
	usbd_ep_t *ep_intr_in = &ncm->ep_intr_in;

	UNUSED(dev);
	UNUSED(status);

	if (ep_addr == USBD_CDC_NCM_BULK_IN_EP) {
		ep_bulk_in->xfer_state = 0U;
		/* SPSC consumer side: the slot at tx_rd just finished DMA.
		 * Advance rd to release it, clear inflight, wake any producer
		 * blocked because the ring was full, and chain the next NTB
		 * if one is queued. */
		{
			ncm_tx_ntb_t *done = &ncm->tx_slot[ncm->tx_rd];
			done->frame_count = 0;
			done->data_offset = 0;
			__sync_synchronize();
			ncm->tx_rd = (u8)((ncm->tx_rd + 1U) % USBD_CDC_NCM_TX_DEPTH);
			ncm->tx_inflight = 0U;
			__sync_synchronize();
			usb_os_sema_give(ncm->tx_buf_free_sema);
			usbd_cdc_ncm_tx_kick(ncm);
		}
#if USBD_CDC_NCM_STATE_TRACE_ENABLE
		ncm->dbg_bulk_in_done_cnt++;
#endif
	} else if (ep_addr == USBD_CDC_NCM_INTR_IN_EP) {
		ep_intr_in->xfer_state = 0U;
#if USBD_CDC_NCM_STATE_TRACE_ENABLE
		ncm->dbg_intr_in_done_cnt++;
#endif
		/* Chain the next notification in the sequence (CONNECT -> SPEED -> NONE).
		 * The endpoint was just freed above, so the send normally succeeds
		 * in-context; if it reports busy, notify_retry lets the SOF handler
		 * re-send (SOF fires every 1 ms FS / 125 us HS, so the delay is tiny). */
		if (ncm->notify_state != NCM_NOTIFY_NONE) {
			if (usbd_cdc_ncm_send_notification() != HAL_OK) {
				ncm->notify_retry = 1U;
			}
		}
	}

	return HAL_OK;
}

/**
 * @brief RX delivery thread
 * @details Waits on rx_data_ready_sema, forwards each received NTB to the parser,
 *          then sets rx_buf_free so the USB OUT path can hand off again.
 */
static void usbd_cdc_ncm_rx_thread(void *param)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	UNUSED(param);

	while (ncm->rx_thread_running) {
#if USBD_CDC_NCM_STATE_TRACE_ENABLE
		/* Heartbeat: advances every loop (incl. the 100ms sema timeout), so a
		 * frozen value in the trace output means the RX thread is wedged. */
		ncm->dbg_rx_loop_cnt++;
#endif
		if (usb_os_sema_take(ncm->rx_data_ready_sema, USBD_CDC_NCM_RX_SEMA_TAKE_TIMEOUT_MS) != HAL_OK) {
			/* timeout - re-check running flag */
			continue;
		}

		if (!ncm->rx_thread_running) {
			break;
		}

		if ((ncm->rx_msg_buf != NULL) && (ncm->rx_msg_len > 0U)) {
			usbd_cdc_ncm_bulk_receive(ncm->rx_msg_buf, ncm->rx_msg_len);
#if USBD_CDC_NCM_STATE_TRACE_ENABLE
			ncm->dbg_rx_deliver_cnt++;
#endif
		}

		/* Previous buffer consumed; let the OUT EP path hand off another. */
		ncm->rx_buf_free = 1U;
	}

	ncm->rx_task = NULL;
	rtos_task_delete(NULL);
}

/**
 * @brief Handle EP data OUT completion
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @param ep_addr: Endpoint address
 * @param len: Received data length
 * @retval Status
 */
static int usbd_cdc_ncm_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usbd_ep_t *ep_bulk_out = &ncm->ep_bulk_out;

	UNUSED(dev);

	if (ep_addr != USBD_CDC_NCM_BULK_OUT_EP) {
		return HAL_ERR_PARA;
	}

	/* Invalidate cache if needed */
	if (ep_bulk_out->skip_dcache_post_invalidate && len > 0) {
		DCache_Invalidate((u32)ep_bulk_out->xfer_buf, len);
	}

	if (len > 0U) {
#if USBD_CDC_NCM_STATE_TRACE_ENABLE
		ncm->dbg_bulk_out_done_cnt++;
#endif
		/* Try to hand off the current buffer to the RX thread. */
		if (ncm->rx_buf_free != 0U) {
			ncm->rx_buf_free = 0U;
			ncm->rx_msg_buf = ncm->rx_buf[ncm->rx_xfer_idx];
			ncm->rx_msg_len = len;
			/* Ensure buf/len writes are visible to the RX thread before
			 * the sema_give wakes it (acquire-release ordering). */
			__sync_synchronize();
			usb_os_sema_give(ncm->rx_data_ready_sema);

			/* Flip to the other buffer for the next OUT transfer. */
			ncm->rx_xfer_idx ^= 1U;
			ep_bulk_out->xfer_buf = ncm->rx_buf[ncm->rx_xfer_idx];
		} else {
			/* RX thread still busy. Do NOT re-arm the endpoint; the host will
			 * see NAK on the bulk-out pipe. Mark as pending so the SOF handler
			 * re-triggers the endpoint as soon as the thread frees the buffer. */
			ncm->rx_pending_len = len;
			return HAL_OK;
		}
	}

	/* Continue receiving */
	return usbd_ep_receive(ncm->dev, ep_bulk_out);
}

/**
 * @brief   SOF interrupt handler
 * @note    This function is called within an interrupt service routine (ISR) context;
 *          time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 */
static int usbd_cdc_ncm_sof(usb_dev_t *dev)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usbd_ep_t *ep_bulk_out = &ncm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ncm->ep_intr_in;

	UNUSED(dev);

	/* Retry a previously-failed notification when the endpoint is free. */
	if (ncm->notify_retry && ep_intr_in->xfer_state == 0U) {
		if (usbd_cdc_ncm_send_notification() == HAL_OK) {
			ncm->notify_retry = 0U;
		}
	}

#ifndef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
	/* Non-aggregation: transmit() always finalizes and advances wd before
	 * returning (NTB_OUT_MAX_DATAGRAMS=1), so slot[wd].frame_count is always
	 * 0 when SOF fires.  No SOF flush needed -- SOF only kicks the consumer. */
#endif /* !CONFIG_USBD_CDC_NCM_TX_AGGREGATION */

	/* Kick consumer: start DMA if idle and ring is non-empty.
	 * Aggregation path: TX task publishes wd; SOF just kicks.
	 * Non-aggregation path: SOF may have just published wd above. */
	usbd_cdc_ncm_tx_kick(ncm);

	if (ncm->rx_pending_len == 0U) {
		return HAL_OK;
	}

	/* Thread still busy - wait for the next SOF. */
	if (ncm->rx_buf_free == 0U) {
		return HAL_OK;
	}

	/* Buffer is free: hand off the pending data and re-arm the endpoint. */
	ncm->rx_buf_free = 0U;
	ncm->rx_msg_buf = ncm->rx_buf[ncm->rx_xfer_idx];
	ncm->rx_msg_len = ncm->rx_pending_len;
	usb_os_sema_give(ncm->rx_data_ready_sema);

	ncm->rx_xfer_idx ^= 1U;
	ep_bulk_out->xfer_buf = ncm->rx_buf[ncm->rx_xfer_idx];
	ncm->rx_pending_len = 0U;

	usbd_ep_receive(ncm->dev, ep_bulk_out);

	return HAL_OK;
}

/**
 * @brief Handle EP0 data OUT phase
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @retval Status
 */
static int usbd_cdc_ncm_handle_ep0_data_out(usb_dev_t *dev)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	int ret = HAL_ERR_HW;

	if (ncm->ctrl_req.bRequest != 0xFFU) {
		if (ncm->ctrl_req.bRequest == USB_CDC_NCM_SET_NTB_INPUT_SIZE) {
			/* dwNtbInMaxSize: 4-byte little-endian payload from the DATA stage.
			 * Clamp to the supported range: at least the NCM-mandated minimum and
			 * no larger than the dwNtbInMaxSize advertised in GET_NTB_PARAMETERS. */
			u32 ntb_in = ((u32)ep0_out->xfer_buf[0]) |
						 ((u32)ep0_out->xfer_buf[1] << 8) |
						 ((u32)ep0_out->xfer_buf[2] << 16) |
						 ((u32)ep0_out->xfer_buf[3] << 24);
			if ((ntb_in >= USB_CDC_NCM_DEFAULT_NTB_INPUT_SIZE) &&
				(ntb_in <= USBD_CDC_NCM_DEFAULT_NTB_IN_SIZE)) {
				ncm->ntb_in_max_size = ntb_in;
				ret = HAL_OK;
			} else {
				ret = HAL_ERR_PARA;
			}
		} else if (ncm->cb && ncm->cb->setup) {
			ret = ncm->cb->setup(&ncm->ctrl_req, ep0_out->xfer_buf);
		}
		ncm->ctrl_req.bRequest = 0xFFU; /* Mark as processed */
	}

	return ret;
}

/**
 * @brief Get USB descriptor
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @param req: SETUP request
 * @param buf: Buffer to fill descriptor
 * @retval Descriptor length
 */
static u16 usbd_cdc_ncm_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usb_speed_type_t speed = dev->dev_speed;
	u8 desc_type = USB_HIGH_BYTE(req->wValue);
	u8 desc_idx = USB_LOW_BYTE(req->wValue);
	char mac_buf[32] = {0,};
	const u8 *desc = NULL;
	u16 len = 0;

	dev->self_powered = USBD_CDC_NCM_SELF_POWERED;
	dev->remote_wakeup_en = USBD_CDC_NCM_REMOTE_WAKEUP_EN;

	switch (desc_type) {
	case USB_DESC_TYPE_DEVICE:
		len = sizeof(usbd_cdc_ncm_dev_desc);
		usb_os_memcpy(buf, usbd_cdc_ncm_dev_desc, len);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
#ifndef CONFIG_USB_FS
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_cdc_ncm_hs_config_desc;
			len = sizeof(usbd_cdc_ncm_hs_config_desc);
		} else
#endif
		{
			desc = (u8 *)usbd_cdc_ncm_fs_config_desc;
			len = sizeof(usbd_cdc_ncm_fs_config_desc);
		}

		usb_os_memcpy((void *)buf, (void *)desc, len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		break;

#ifndef CONFIG_USB_FS
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		len = sizeof(usbd_cdc_ncm_device_qualifier_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_cdc_ncm_device_qualifier_desc, len);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		desc = (speed == USB_SPEED_HIGH) ?
			   usbd_cdc_ncm_fs_config_desc : usbd_cdc_ncm_hs_config_desc;
		len = (speed == USB_SPEED_HIGH) ?
			  sizeof(usbd_cdc_ncm_fs_config_desc) : sizeof(usbd_cdc_ncm_hs_config_desc);
		usb_os_memcpy(buf, desc, len);
		buf[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		break;
#endif

	case USB_DESC_TYPE_STRING:
		switch (desc_idx) {
		case USBD_IDX_LANGID_STR:
			len = sizeof(usbd_cdc_ncm_lang_id_desc);
			usb_os_memcpy(buf, usbd_cdc_ncm_lang_id_desc, len);
			break;
		case USBD_IDX_MFC_STR:
			len = usbd_get_str_desc(USBD_CDC_NCM_MFG_STRING, buf);
			break;
		case USBD_IDX_PRODUCT_STR:
			len = usbd_get_str_desc((speed == USB_SPEED_HIGH) ?
									USBD_CDC_NCM_PROD_HS_STRING : USBD_CDC_NCM_PROD_FS_STRING, buf);
			break;
		case USBD_IDX_SERIAL_STR:
			len = usbd_get_str_desc(USBD_CDC_NCM_SN_STRING, buf);
			break;
		case USBD_CDC_NCM_MAC_STRING_INDEX:
			usbd_cdc_ncm_mac_to_string((const u8 *)(ncm->mac), mac_buf);
			len = usbd_get_str_desc(mac_buf, buf);
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}

	return len;
}

/**
 * @brief USB attach status changed callback
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @param old_status: Previous status
 * @param status: Current status
 */
static void usbd_cdc_ncm_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;

	UNUSED(dev);

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		/* Clear link state immediately */
		ncm->connect_status = 0;
		ncm->notify_state = NCM_NOTIFY_NONE;
		ncm->notify_retry = 0U;

		ncm->rx_pending_len = 0U;

		/* Discard any buffered but unsent frames.
		 * NOTE: this runs in ISR context.  The producer (lwIP task) only
		 * touches slot[tx_wd] and only reads tx_rd; resetting indices
		 * here is safe because the producer will see connect_status==0
		 * on its next sema_take wake and bail out before touching the ring. */
		{
			u8 i;
			for (i = 0U; i < USBD_CDC_NCM_TX_DEPTH; i++) {
				ncm->tx_slot[i].frame_count = 0;
				ncm->tx_slot[i].data_offset = 0;
			}
			ncm->tx_wd = 0;
			ncm->tx_rd = 0;
			ncm->tx_inflight = 0;
#ifndef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
			ncm->tx_filling_busy = 0U;
			ncm->tx_wd_tick = 0U;
#endif
			__sync_synchronize();
		}
		/* Wake any producer that was blocked waiting for a slot so it
		 * can observe connect_status==0 and bail out. */
		if (ncm->tx_buf_free_sema != NULL) {
			usb_os_sema_give(ncm->tx_buf_free_sema);
		}
	}

	if (ncm->cb && ncm->cb->status_changed) {
		ncm->cb->status_changed(old_status, status);
	}
}

#if USBD_CDC_NCM_STATE_TRACE_ENABLE
/**
 * @brief Periodic state-trace thread.
 * @details Mirrors the USBH CDC ECM trace task: every second it dumps the live
 *          state of the NCM device instance (link, notification machine, endpoint
 *          xfer states, RX ping-pong hand-off, TX block flag, NTB sizes and the
 *          per-path completion counters) so traffic stalls can be diagnosed
 *          without a debugger. Compiled out unless USBD_CDC_NCM_STATE_TRACE_ENABLE.
 */
static void usbd_cdc_ncm_trace_thread(void *param)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usbd_ep_t *ep_bulk_in = &ncm->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &ncm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ncm->ep_intr_in;
	usb_dev_t *dev;

	UNUSED(param);

	while (ncm->trace_task_running) {
		dev = ncm->dev;
		/* ncm->dev is assigned in set_config; before enumeration it is NULL. */
		if (dev != NULL) {
			RTK_LOGS(TAG, RTK_LOG_INFO,
					 "rdy %d conn %d ntf %d/%d alt %d/ep i%d o%d t%d/rx f%d pend%d idx%d/tx %s%d seq%d\n",
					 dev->is_ready, ncm->connect_status, ncm->notify_state, ncm->notify_retry, ncm->alt_setting,
					 ep_bulk_in->xfer_state, ep_bulk_out->xfer_state, ep_intr_in->xfer_state,
					 ncm->rx_buf_free, (u32)ncm->rx_pending_len, ncm->rx_xfer_idx,
					 "ring", (int)((ncm->tx_wd - ncm->tx_rd + USBD_CDC_NCM_TX_DEPTH) % USBD_CDC_NCM_TX_DEPTH),
					 ncm->sequence);
			RTK_LOGS(TAG, RTK_LOG_INFO,
					 "cnt tx%d in%d out%d intr%d rxd%d/Heap %d ntb %d/%d\n",
					 ncm->dbg_tx_cnt, ncm->dbg_bulk_in_done_cnt, ncm->dbg_bulk_out_done_cnt,
					 ncm->dbg_intr_in_done_cnt, ncm->dbg_rx_deliver_cnt,
					 rtos_mem_get_free_heap_size(), ncm->ntb_in_max_size, ncm->ntb_out_max_size);
			RTK_LOGS(TAG, RTK_LOG_INFO, "tx pp[0] frm%d off%d / pp[1] frm%d off%d / wd%d rd%d inflight%d\n",
					 ncm->tx_slot[0].frame_count, ncm->tx_slot[0].data_offset,
					 ncm->tx_slot[1].frame_count, ncm->tx_slot[1].data_offset,
					 ncm->tx_wd, ncm->tx_rd, ncm->tx_inflight);
		}

		/* Thread trace: rx[loop is a heartbeat - frozen => RX thread wedged]. */
		RTK_LOGS(TAG, RTK_LOG_INFO, "thr rx[run %d alive %d prio %d loop %d] trace[prio %d]\n",
				 ncm->rx_thread_running, (ncm->rx_task != NULL),
				 (ncm->rx_task != NULL) ? (int)rtos_task_priority_get(ncm->rx_task) : -1,
				 ncm->dbg_rx_loop_cnt,
				 (ncm->trace_task != NULL) ? (int)rtos_task_priority_get(ncm->trace_task) : -1);

		usb_os_sleep_ms(USBD_CDC_NCM_TRACE_INTERVAL_MS);
	}

	ncm->trace_task = NULL;
	rtos_task_delete(NULL);
}

/**
 * @brief Create the NCM state-trace thread (idempotent).
 */
static void usbd_cdc_ncm_trace_task_init(void)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	int status;

	if (ncm->trace_task_running) {
		return;
	}

	ncm->trace_task_running = 1;
	status = rtos_task_create(&ncm->trace_task, "usbd_cdc_ncm_trace", usbd_cdc_ncm_trace_thread, NULL,
							  USBD_CDC_NCM_TRACE_THREAD_STACK_SIZE, USBD_CDC_NCM_TRACE_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create NCM trace task fail\n");
		ncm->trace_task_running = 0;
	}
}

/**
 * @brief Stop the NCM state-trace thread and wait for it to exit.
 */
static void usbd_cdc_ncm_trace_task_deinit(void)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	ncm->trace_task_running = 0;
	while (ncm->trace_task != NULL) {
		usb_os_sleep_ms(10);
	}
}
#endif /* USBD_CDC_NCM_STATE_TRACE_ENABLE */

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize CDC NCM class
 * @param cb: User callbacks
 * @retval Status
 */
int usbd_cdc_ncm_init(const usbd_cdc_ncm_cb_t *cb)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usbd_ep_t *ep_bulk_in = &ncm->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &ncm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ncm->ep_intr_in;
	usb_ep_info_t *info;
	int ret = HAL_OK;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	ncm->ctrl_req.bRequest = 0xFFU;

	/* Allocate per-slot DMA-aligned NTB buffers for the ping-pong TX path.
	 * Each slot keeps its buffer for the lifetime of the class instance;
	 * clear_config() only resets state, never frees these. */
	{
		u8 i;
		for (i = 0U; i < USBD_CDC_NCM_TX_DEPTH; i++) {
			ncm->tx_slot[i].buf = (u8 *)usb_os_malloc(CACHE_LINE_ALIGNMENT(USBD_CDC_NCM_NTB_TX_BUF_SIZE));
			if (ncm->tx_slot[i].buf == NULL) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc TX pp slot %d fail\n", i);
				/* Roll back any earlier slots before bailing out. */
				while (i-- > 0U) {
					usb_os_mfree(ncm->tx_slot[i].buf);
					ncm->tx_slot[i].buf = NULL;
				}
				return HAL_ERR_MEM;
			}
			ncm->tx_slot[i].frame_count = 0;
			ncm->tx_slot[i].data_offset = 0;
			ncm->tx_slot[i].sequence = 0;
		}
		ncm->tx_wd = 0;
		ncm->tx_rd = 0;
		ncm->tx_inflight = 0;
#ifndef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
		ncm->tx_filling_busy = 0U;
		ncm->tx_wd_tick = 0U;
#endif
	}

	/* Initialize NTB parameters */
	ncm->ntb_in_max_size = USBD_CDC_NCM_DEFAULT_NTB_IN_SIZE;
	ncm->ntb_out_max_size = USBD_CDC_NCM_DEFAULT_NTB_OUT_SIZE;
	ncm->sequence = 0;
	ncm->ntb_format = 0; /* NTB16 */
	ncm->crc_mode = 0;   /* No CRC */

	/* BULK IN use the caller buffer */
	info = &ep_bulk_in->info;
	info->addr = USBD_CDC_NCM_BULK_IN_EP;
	info->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_in->xfer_buf_len = 0;
	ep_bulk_in->xfer_buf = NULL;

	/* Allocate BULK OUT ping-pong buffers */
	info = &ep_bulk_out->info;
	info->addr = USBD_CDC_NCM_BULK_OUT_EP;
	info->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_out->xfer_buf_len = USBD_CDC_NCM_BULK_BUF_MAX_SIZE;
	ep_bulk_out->xfer_len = ep_bulk_out->xfer_buf_len;

	ncm->rx_buf[0] = (u8 *)usb_os_malloc(USBD_CDC_NCM_BULK_BUF_MAX_SIZE);
	if (ncm->rx_buf[0] == NULL) {
		ret = HAL_ERR_MEM;
		goto exit;
	}

	ncm->rx_buf[1] = (u8 *)usb_os_malloc(USBD_CDC_NCM_BULK_BUF_MAX_SIZE);
	if (ncm->rx_buf[1] == NULL) {
		ret = HAL_ERR_MEM;
		goto cleanup_rx_buf0;
	}

	ncm->rx_xfer_idx = 0U;
	ep_bulk_out->xfer_buf = ncm->rx_buf[ncm->rx_xfer_idx];

	/* tx_buf_free_sema: ISR -> producer, given each time a slot returns to FREE. */
	if (usb_os_sema_create(&ncm->tx_buf_free_sema) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create TX free sema fail\n");
		ret = HAL_ERR_MEM;
		goto cleanup_rx_buf1;
	}

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
	/* Raw frame ring buffer: lwIP -> TX task. */
	if (usb_ringbuf_manager_init(&ncm->tx_raw_rb, USBD_CDC_NCM_TX_RB_DEPTH,
								 USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE, 1) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init TX raw ring buffer fail\n");
		ret = HAL_ERR_MEM;
		goto cleanup_bulk_tx_slot_sema;
	}
	if (usb_os_sema_create(&ncm->tx_raw_sema) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create TX raw sema fail\n");
		ret = HAL_ERR_MEM;
		goto cleanup_tx_raw_rb;
	}
#endif

	/* rx_data_ready_sema: ISR -> thread, given when a buffer is filled. */
	if (usb_os_sema_create(&ncm->rx_data_ready_sema) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create RX data_ready sema fail\n");
		ret = HAL_ERR_MEM;
		goto cleanup_bulk_tx_slot_sema;
	}

	ncm->rx_buf_free    = 1U;
	ncm->rx_pending_len = 0U;

	/* Start RX delivery thread */
	ncm->rx_thread_running = 1;
	ret = rtos_task_create(&ncm->rx_task, "usbd_cdc_ncm_rx", usbd_cdc_ncm_rx_thread, NULL,
						   USBD_CDC_NCM_RX_THREAD_STACK_SIZE, USBD_CDC_NCM_RX_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create RX thread fail\n");
		ncm->rx_thread_running = 0;
		ret = HAL_ERR_HW;
		goto cleanup_rx_data_ready_sema;
	}

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
	usbd_cdc_ncm_tx_task_init();
#endif

	/* Allocate INTR IN buffer */
	info = &ep_intr_in->info;
	info->addr = USBD_CDC_NCM_INTR_IN_EP;
	info->type = USB_CH_EP_TYPE_INTR;
	ep_intr_in->xfer_buf_len = USBD_CDC_NCM_INTR_IN_PACKET_SIZE;
	ep_intr_in->xfer_buf = (u8 *)usb_os_malloc(ep_intr_in->xfer_buf_len);
	if (ep_intr_in->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto cleanup_rx_thread;
	}

	if ((cb->priv != NULL) && (cb->priv->mac_value != NULL)) {
		usbd_cdc_ncm_set_mac(cb->priv->mac_value);
	} else {
		usbd_cdc_ncm_set_mac(usbd_cdc_ncm_default_mac);
	}

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "User init fail: %d\n", ret);
			goto cleanup_intr_in;
		}
	}

	/* Initialize user callbacks */
	ncm->cb = cb;

	/* Register CDC NCM class driver */
	usbd_register_class(&usbd_cdc_ncm_driver);

#if USBD_CDC_NCM_STATE_TRACE_ENABLE
	usbd_cdc_ncm_trace_task_init();
#endif

	return HAL_OK;

cleanup_intr_in:
	usb_os_mfree(ep_intr_in->xfer_buf);
	ep_intr_in->xfer_buf = NULL;

cleanup_rx_thread:
	ncm->rx_thread_running = 0;
	/* Wake the thread so it can observe running=0 and exit. */
	usb_os_sema_give(ncm->rx_data_ready_sema);
	while (ncm->rx_task != NULL) {
		usb_os_sleep_ms(10);
	}

cleanup_rx_data_ready_sema:
	usb_os_sema_delete(ncm->rx_data_ready_sema);
	ncm->rx_data_ready_sema = NULL;

cleanup_bulk_tx_slot_sema:
	usb_os_sema_delete(ncm->tx_buf_free_sema);
	ncm->tx_buf_free_sema = NULL;

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
cleanup_tx_raw_rb:
	usb_os_sema_delete(ncm->tx_raw_sema);
	ncm->tx_raw_sema = NULL;
	usb_ringbuf_manager_deinit(&ncm->tx_raw_rb);
#endif

cleanup_rx_buf1:
	usb_os_mfree(ncm->rx_buf[1]);
	ncm->rx_buf[1] = NULL;

cleanup_rx_buf0:
	usb_os_mfree(ncm->rx_buf[0]);
	ncm->rx_buf[0] = NULL;
	ep_bulk_out->xfer_buf = NULL;

exit: {
		u8 i;
		for (i = 0U; i < USBD_CDC_NCM_TX_DEPTH; i++) {
			if (ncm->tx_slot[i].buf != NULL) {
				usb_os_mfree(ncm->tx_slot[i].buf);
				ncm->tx_slot[i].buf = NULL;
			}
		}
	}

	return ret;
}

/**
 * @brief Deinitialize CDC NCM class
 * @retval Status
 */
int usbd_cdc_ncm_deinit(void)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usbd_ep_t *ep_bulk_in = &ncm->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &ncm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ncm->ep_intr_in;
	u32 wait_cnt = 0U;

	ncm->connect_status = 0;

#if USBD_CDC_NCM_STATE_TRACE_ENABLE
	/* Stop the trace thread first so it does not read state being torn down. */
	usbd_cdc_ncm_trace_task_deinit();
#endif

	/* Wait for any in-flight IN transfer to actually complete before tearing
	 * down. xfer_state (not is_busy) is the real in-flight flag: it is set when
	 * a transfer is submitted and cleared by the completion ISR. A bounded
	 * timeout avoids spinning forever if the device was detached and the
	 * completion interrupt will never arrive. This matters because the INTR IN
	 * xfer_buf is freed below and must not be read by a still-active DMA. */
	while ((ep_bulk_in->xfer_state || ep_intr_in->xfer_state) && (wait_cnt < 1000U)) {
		usb_os_delay_us(100);
		wait_cnt++;
	}

	/* Unregister class driver */
	usbd_unregister_class();

	/* After unregister, no more USB ISRs will fire.  Tear down TX state. */
	{
		u8 i;
		for (i = 0U; i < USBD_CDC_NCM_TX_DEPTH; i++) {
			ncm->tx_slot[i].frame_count = 0;
			ncm->tx_slot[i].data_offset = 0;
			if (ncm->tx_slot[i].buf != NULL) {
				usb_os_mfree(ncm->tx_slot[i].buf);
				ncm->tx_slot[i].buf = NULL;
			}
		}
		ncm->tx_wd = 0;
		ncm->tx_rd = 0;
		ncm->tx_inflight = 0;
#ifndef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
		ncm->tx_filling_busy = 0U;
		ncm->tx_wd_tick = 0U;
#endif
	}
	/* Unblock any transmit() still waiting for a free slot, then free the sema. */
	if (ncm->tx_buf_free_sema != NULL) {
		usb_os_sema_give(ncm->tx_buf_free_sema);
		usb_os_sema_delete(ncm->tx_buf_free_sema);
		ncm->tx_buf_free_sema = NULL;
	}

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
	/* Stop TX aggregation task and free raw ring buffer. */
	usbd_cdc_ncm_tx_task_deinit();
	usb_ringbuf_manager_deinit(&ncm->tx_raw_rb);
	if (ncm->tx_raw_sema != NULL) {
		usb_os_sema_delete(ncm->tx_raw_sema);
		ncm->tx_raw_sema = NULL;
	}
#endif

	/* Stop the RX delivery thread and wait for it to exit. */
	ncm->rx_thread_running = 0;
	if (ncm->rx_data_ready_sema != NULL) {
		usb_os_sema_give(ncm->rx_data_ready_sema);
	}
	while (ncm->rx_task != NULL) {
		usb_os_sleep_ms(10);
	}

	/* Tear down RX semaphores. */
	if (ncm->rx_data_ready_sema != NULL) {
		usb_os_sema_delete(ncm->rx_data_ready_sema);
		ncm->rx_data_ready_sema = NULL;
	}
	/* Call user deinit */
	if (ncm->cb && ncm->cb->deinit) {
		ncm->cb->deinit();
	}

	/* Free buffers */
	if (ep_intr_in->xfer_buf != NULL) {
		usb_os_mfree(ep_intr_in->xfer_buf);
		ep_intr_in->xfer_buf = NULL;
	}

	/* Free RX ping-pong buffers. */
	for (u8 i = 0; i < USBD_CDC_NCM_RX_BUF_NUM; i++) {
		if (ncm->rx_buf[i] != NULL) {
			usb_os_mfree(ncm->rx_buf[i]);
			ncm->rx_buf[i] = NULL;
		}
	}
	ep_bulk_out->xfer_buf = NULL;

	return HAL_OK;
}

/**
 * @brief Start to transmit an ethernet frame via NCM NTB16
 * @param buf: Ethernet frame data buffer
 * @param len: Ethernet frame length
 * @param block: Whether block until xfer done
 * @retval Status
 */
int usbd_cdc_ncm_transmit(u8 *buf, u32 len, u8 block)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	int ret;

#if USBD_CDC_NCM_STATE_TRACE_ENABLE
	ncm->dbg_tx_cnt++;
#endif

#if USBD_CDC_NCM_TX_SPEED_CHECK
	static u64 usb_tx_start_time = 0, usb_tx_end_time, usb_tx_interval_time;
	static u64 usb_tx_total_len = 0;

	usb_tx_total_len += len;
	if (usb_tx_start_time == 0) {
		usb_tx_start_time = usb_os_get_timestamp_ms();
	}
#endif

	/* ---- Aggregation TX path: buffer frames, flush on trigger ---- */

	/* Validate frame parameters BEFORE touching aggregation state, otherwise
	 * a bad call could corrupt a half-built NTB or NULL-deref inside
	 * usbd_cdc_ncm_agg_append's memcpy. */
	if ((buf == NULL) || (len == 0U) || (len > USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE)) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "TX bad param (len %u)\n", len);
		return HAL_ERR_PARA;
	}

	/* SPSC producer side.  Single producer (lwIP tcpip_thread) -- the
	 * single consumer (USB ISR: SOF + XFRC, serialised) advances tx_rd
	 * after each XFRC.  Producer touches only slot[tx_wd]; consumer
	 * touches only slot[tx_rd].  wd/rd index isolation is the primary
	 * mutual-exclusion mechanism.
	 *
	 * Aggregation: multiple frames are appended into slot[tx_wd] until
	 * MAX_DATAGRAMS is reached (size trigger) or the SOF timeout fires.
	 * tx_filling_busy=1 while the producer holds the slot so SOF backs off.
	 *
	 * The block parameter:
	 *   - block != 0: wait on tx_buf_free_sema for a slot to free up.
	 *   - block == 0: drop immediately (HAL_BUSY) when the ring is full. */

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
	/* Aggregation path: lwIP task just enqueues the raw frame into tx_raw_rb.
	 * The dedicated TX task (usbd_cdc_ncm_tx_task) dequeues frames, builds NTBs,
	 * and advances tx_wd.  No NTB construction or critical sections here. */
	if (block) {
		do {
			ret = usb_ringbuf_add_tail(&ncm->tx_raw_rb, buf, len, 1U);
			if (ret == HAL_OK) {
				break;
			}
			if (usb_os_sema_take(ncm->tx_raw_sema,
								 USBD_CDC_NCM_BULK_TX_TIMEOUT_MS) != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "TX timeout drop(%u)\n", len);
				return HAL_BUSY;
			}
			if (ncm->connect_status == 0U) {
				return HAL_BUSY;
			}
		} while (1);
	} else {
		ret = usb_ringbuf_add_tail(&ncm->tx_raw_rb, buf, len, 1U);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "TX drop(%u): raw rb full\n", len);
			return HAL_BUSY;
		}
	}
	/* Wake the TX task so it processes the new frame promptly. */
	usb_os_sema_give(ncm->tx_raw_sema);
	ret = HAL_OK;

#else /* !CONFIG_USBD_CDC_NCM_TX_AGGREGATION */

	/* Non-aggregation path: lwIP task directly builds one-frame NTBs and
	 * publishes them to the SPSC ring.  The SOF ISR kicks the consumer.
	 * wd/rd index isolation is the sole mutual-exclusion mechanism:
	 * producer touches only slot[tx_wd], consumer only slot[tx_rd].
	 * NTB_OUT_MAX_DATAGRAMS=1 means transmit() always finalize+advances wd
	 * before returning, so SOF never sees frame_count>0 on slot[wd] and
	 * never writes slot[wd] -- no critical section needed. */
	{
		ncm_tx_ntb_t *slot = &ncm->tx_slot[ncm->tx_wd];

		if (slot->frame_count == 0U) {
			while (usbd_cdc_ncm_tx_ring_full(ncm)) {
				if (block == 0U) {
					RTK_LOGS(TAG, RTK_LOG_WARN, "TX drop(%u): ring full\n", len);
					return HAL_BUSY;
				}
				if (usb_os_sema_take(ncm->tx_buf_free_sema,
									 USBD_CDC_NCM_BULK_TX_TIMEOUT_MS) != HAL_OK) {
					RTK_LOGS(TAG, RTK_LOG_WARN, "TX timeout drop(%u)\n", len);
					return HAL_BUSY;
				}
				if (ncm->connect_status == 0U) {
					return HAL_BUSY;
				}
			}
			usbd_cdc_ncm_agg_begin(slot, ncm->sequence);
		}
		/* Extra connect_status check for the non-wait path: if ring was not
		 * full the while-loop above was skipped entirely.  Without this, a
		 * clear_config/status_changed ISR that fires after the loop but
		 * before finalize could leave a stale NTB queued after reset. */
		if (ncm->connect_status == 0U) {
			return HAL_BUSY;
		}

		if (usbd_cdc_ncm_agg_append(slot, buf, len) != HAL_OK) {
			/* Should not happen for a valid single-frame NTB: the buf is
			 * sized for MAX_DATAGRAMS=1 frame.  Drop the frame. */
			RTK_LOGS(TAG, RTK_LOG_ERROR, "TX frame too large: %u\n", len);
			return HAL_ERR_PARA;
		}

		/* NTB_OUT_MAX_DATAGRAMS=1: always finalize and publish immediately. */
		usbd_cdc_ncm_agg_finalize(slot);
		ncm->sequence++;
		__sync_synchronize();
		ncm->tx_wd = (u8)((ncm->tx_wd + 1U) % USBD_CDC_NCM_TX_DEPTH);
		__sync_synchronize();
		ret = HAL_OK;
	}

#endif /* CONFIG_USBD_CDC_NCM_TX_AGGREGATION */

#if USBD_CDC_NCM_TX_SPEED_CHECK
	usb_tx_end_time = usb_os_get_timestamp_ms();
	usb_tx_interval_time = (usb_tx_end_time - usb_tx_start_time) * RTOS_TICK_RATE_MS;

	if (usb_tx_interval_time >= 3000) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Heap %d TX %dB in %d ms, %d Kbps\n",
				 rtos_mem_get_free_heap_size(),
				 (u32)usb_tx_total_len, (u32)usb_tx_interval_time, (u32)((usb_tx_total_len * 8 * 1000) / (usb_tx_interval_time * 1024)));
		usb_tx_start_time = usb_tx_end_time;
		usb_tx_total_len = 0;
	}
#endif

	return ret;
}

/**
 * @brief  Get the current network link state (uplink status).
 * @retval 1: Network link is up (connected to uplink). 0: Network link is down.
 */
int usbd_cdc_ncm_get_link_status(void)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;

	return ncm->connect_status;
}

/**
  * @brief  Report the upper-layer network link state to the host.
  * @note   Edge-triggered: a NETWORK_CONNECTION notification is queued only when
  *         the link state actually changes.  Repeated calls with the same value
  *         are no-ops, so the host is not spammed.  The notification is sent from
  *         the SOF handler (ISR context) to avoid a thread-vs-ISR race on the
  *         INTR IN endpoint; callable from any task context.
  * @param  link_up: Non-zero when the network link is up, zero when it is down.
  * @retval HAL_OK
  */
int usbd_cdc_ncm_set_link_status(u8 link_up)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;

	link_up = link_up ? 1U : 0U;

	/* Only act on an actual transition. */
	if (ncm->connect_status == link_up) {
		return HAL_OK;
	}
	ncm->connect_status = link_up;

	/* Queue a NETWORK_CONNECTION notification; the SOF handler performs the
	 * actual transmit so all INTR IN endpoint access stays in ISR context. */
	ncm->notify_state = NCM_NOTIFY_CONNECT;
	ncm->notify_retry = 1U;

	return HAL_OK;
}

/**
 * @brief Weak default usb_ethernet_transmit for compatibility.
 *        Upper layer (lwIP) calls this to send an ethernet frame.
 * @param buf: Ethernet frame buffer
 * @param len: Frame length
 * @param block: Blocking flag
 * @retval Status
 */
__attribute__((weak))
int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbd_cdc_ncm_transmit(buf, len, block);
}
