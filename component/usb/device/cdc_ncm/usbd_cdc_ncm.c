/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_cdc_ncm.h"
#ifdef CONFIG_USBD_COMPOSITE
#include "usbd_composite.h"
#endif

#define USBD_CDC_NCM_RX_SPEED_CHECK                       0                     /* CDC NCM rx speed test */
#define USBD_CDC_NCM_TX_SPEED_CHECK                       0                     /* CDC NCM tx speed test */

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
/* Maximum ethernet frames this device aggregates into one device-to-host (IN)
 * NTB, i.e. the depth of the TX aggregation path.  "IN" is the USB direction
 * (Ref NCM 1.0: IN = device-to-host), so this is purely a property of what the
 * device emits - it must not be confused with, or reused as, the
 * wNtbOutMaxDatagrams limit advertised to the host, which constrains the
 * opposite direction.  See USBD_CDC_NCM_NTB_OUT_MAX_DATAGRAMS below.
 *
 * 2 frames per NTB balances throughput and memory: each NTB slot buf is
 * ~3 KB (vs ~6 KB for 4), and 4 slots total = ~12 KB instead of ~24 KB. */
#define USBD_CDC_NCM_NTB_IN_MAX_DATAGRAMS            2U

/* TX aggregation task: stack size, priority and sema timeout.
 * frame_buf is heap-allocated, so stack only needs call frames + RTOS overhead. */
#define USBD_CDC_NCM_TX_TASK_STACK_SIZE               1024U
#define USBD_CDC_NCM_TX_TASK_PRIORITY                 5U
#define USBD_CDC_NCM_TX_SEMA_TIMEOUT_MS               10U

#else
/* Non-aggregation: one frame per device-to-host (IN) NTB, no extra task. */
#define USBD_CDC_NCM_NTB_IN_MAX_DATAGRAMS            1U
#endif

/* wNtbOutMaxDatagrams: how many datagrams the HOST may pack into one
 * host-to-device (OUT) NTB, as advertised in GET_NTB_PARAMETERS
 * (Ref NCM 1.0 Table 6-3; 0 would mean "no limit").
 *
 * This is the OUT direction - the device's RX path - and is therefore a separate
 * knob from USBD_CDC_NCM_NTB_IN_MAX_DATAGRAMS above, which sizes the device's own
 * TX aggregation.  The two used to share one constant, so retuning TX aggregation
 * silently changed what the device promised the host about OUT traffic.
 *
 * The RX parser walks every NDP entry and bounds-checks each datagram against the
 * received length, so nothing in the device actually limits the count; the real
 * bound is how much fits in one OUT NTB, already advertised as dwNtbOutMaxSize
 * (USBD_CDC_NCM_DEFAULT_NTB_OUT_SIZE, 4096 B, which is also the RX buffer size).
 * That works out to 2 full-size 1514-byte datagrams, or ~59 at 64 bytes.
 *
 * 1 is kept deliberately: it is what the default (non-aggregation) build has
 * always advertised, and the bandwidth it costs is negligible - a lone 1514-byte
 * datagram carries 30 B of NTB overhead (1.98%) versus 36 B for two (1.17%). */
#define USBD_CDC_NCM_NTB_OUT_MAX_DATAGRAMS            1U

/* Aggregation NTB layout (all derived from the IN max-datagrams knob above):
 *   NDP area = header(8) + (max_datagrams + 1 terminator) * 4 */
#define USBD_CDC_NCM_AGG_NDP_SIZE                     (8U + (USBD_CDC_NCM_NTB_IN_MAX_DATAGRAMS + 1U) * 4U)
#define USBD_CDC_NCM_AGG_NDP_OFFSET                   USB_CDC_NCM_NTH16_LENGTH
#define USBD_CDC_NCM_AGG_DATA_OFFSET                  ((USBD_CDC_NCM_AGG_NDP_OFFSET + USBD_CDC_NCM_AGG_NDP_SIZE + USB_CDC_NCM_DATAGRAM_ALIGN - 1) & ~(USB_CDC_NCM_DATAGRAM_ALIGN - 1))

/* ncm_tx_ntb_t::frame_count is u8 in the slot typedef -- ensure the
 * configured limit cannot wrap the counter and silently overwrite the
 * NTH16/NDP area. */
_Static_assert(USBD_CDC_NCM_NTB_IN_MAX_DATAGRAMS <= 255U,
			   "USBD_CDC_NCM_NTB_IN_MAX_DATAGRAMS exceeds u8 frame_count capacity");

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
#define USBD_CDC_NCM_NTB_TX_BUF_SIZE                  (USBD_CDC_NCM_AGG_DATA_OFFSET + USBD_CDC_NCM_NTB_IN_MAX_DATAGRAMS * (USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE + USB_CDC_NCM_DATAGRAM_ALIGN))

/* Private defines -----------------------------------------------------------*/

/* Class-specific string descriptors: indices above USBD_IDX_SERIAL_STR, laid out as a
 * window whose base is the standalone default below, or the one assigned by the composite
 * framework via set_class_str_base(). */
#define USBD_CDC_NCM_STR_IDX_MAC                      0U                         /**< Ordinal of the MAC string inside the class string window */
#define USBD_CDC_NCM_CLASS_STR_COUNT                  1U                         /**< Class-specific string count: iMACAddress only */
#define USBD_CDC_NCM_CLASS_STR_BASE_DEFAULT           (USBD_IDX_SERIAL_STR + 1U) /**< Standalone base, right above the device-global strings */

/* Interface numbers */
#define USBD_CDC_NCM_COMM_INTERFACE_NUM 0x00U  /**< Communication interface */
#define USBD_CDC_NCM_DATA_INTERFACE_NUM 0x01U  /**< Data interface */

/* Private macros ------------------------------------------------------------*/

/* TX NTB slot depth: see USBD_CDC_NCM_TX_DEPTH in usbd_cdc_ncm.h. */

/* Maximum time (ms) usbd_cdc_ncm_transmit() will block waiting for a free
 * TX NTB buffer when called with block != 0.  Set to USB_OS_SEMA_TIMEOUT
 * (wait forever) by default: the caller asked for backpressure, so the
 * upper layer prefers to stall rather than drop a frame. */
#define USBD_CDC_NCM_BULK_TX_TIMEOUT_MS               USB_OS_SEMA_TIMEOUT

/* NCM Functional Descriptor size (6 bytes) */
#define USBD_CDC_NCM_FUNC_DESC_SIZE                   6U

/* bmNetworkCapabilities of the NCM Functional Descriptor (Ref NCM 1.0 Table 5-2).
 *
 * This is NOT the NTB-format bitmap: the supported NTB formats are reported in
 * GET_NTB_PARAMETERS.bmNtbFormatsSupported (Ref NCM 1.0 Table 6-3), which this
 * driver fills in separately.  The two fields have completely different bit
 * definitions, so the NTB-format macro must not be used here.
 *
 * Every capability in this field is optional and this device implements none of
 * them: GET/SET_NET_ADDRESS are not handled, there is no encapsulated-command
 * channel, SET_MAX_DATAGRAM_SIZE is not honoured (the maximum is fixed at one
 * Ethernet frame), only CRC mode 0 exists so there is nothing to negotiate, and
 * the 8-byte form of SET_NTB_INPUT_SIZE is not accepted.  Declaring 0 keeps a
 * compliant host from issuing requests the device cannot answer. */
#define USBD_CDC_NCM_NETWORK_CAPABILITIES             0x00U

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
static void usbd_cdc_ncm_clear_config(usb_dev_t *dev, u8 config);
static int usbd_cdc_ncm_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_cdc_ncm_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
static int usbd_cdc_ncm_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_cdc_ncm_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_cdc_ncm_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static void usbd_cdc_ncm_sof(usb_dev_t *dev);
static void usbd_cdc_ncm_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
#ifdef CONFIG_USBD_COMPOSITE
static u8 usbd_cdc_ncm_set_class_str_base(u8 base);
#endif
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
static void usbd_cdc_ncm_data_alt_start(usb_dev_t *dev);
static void usbd_cdc_ncm_data_alt_stop(usb_dev_t *dev);
static void usbd_cdc_ncm_tx_reset(usbd_cdc_ncm_dev_t *ncm);
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
	USB_CDC_COMM_INTERFACE_CLASS_CODE,              /* bDeviceClass: Communications */
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
	0x80,                                           /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,                                           /* bMaxPower: 100mA */

	/* IAD for CDC NCM (Communication + Data) */
	USB_LEN_IAD_DESC,                               /* bLength */
	USB_DESC_TYPE_IAD,                              /* bDescriptorType */
	0x00,                                           /* bFirstInterface (patched by composite) */
	0x02,                                           /* bInterfaceCount */
	USB_CDC_COMM_INTERFACE_CLASS_CODE,              /* bFunctionClass: CDC */
	USB_CDC_NCM_SUBCLASS_CODE,                      /* bFunctionSubClass: NCM */
	USB_CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC,        /* bFunctionProtocol */
	0x00,                                           /* iFunction */

	/* Communication Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_NCM_COMM_INTERFACE_NUM,                /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x01,                                           /* bNumEndpoints */
	USB_CDC_COMM_INTERFACE_CLASS_CODE,              /* bInterfaceClass: CDC */
	USB_CDC_NCM_SUBCLASS_CODE,                           /* bInterfaceSubClass: NCM */
	USB_CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC,        /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* CDC Header Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	USB_CDC_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_FUNC_DESC_HEADER,                       /* bDescriptorSubtype */
	0x10, 0x01,                                     /* bcdCDC: 1.10 */

	/* CDC Union Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	USB_CDC_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_FUNC_DESC_UNION,                        /* bDescriptorSubtype */
	USBD_CDC_NCM_COMM_INTERFACE_NUM,                /* bControlInterface */
	USBD_CDC_NCM_DATA_INTERFACE_NUM,                /* bSubordinateInterface0 */

	/* CDC Ethernet Networking Functional Descriptor */
	USBD_CDC_NCM_ETHERNET_FUNC_DESC_SIZE,           /* bFunctionLength */
	USB_CDC_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_FUNC_DESC_ETHERNET_NETWORKING,          /* bDescriptorSubtype */
	USBD_CDC_NCM_CLASS_STR_BASE_DEFAULT +
	USBD_CDC_NCM_STR_IDX_MAC,                       /* iMACAddress, runtime patched */
	0x00, 0x00, 0x00, 0x00,                         /* bmEthernetStatistics */
	USB_LOW_BYTE(USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE), /* wMaxSegmentSize */
	USB_HIGH_BYTE(USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE),
	0x00, 0x00,                                     /* wNumberMCFilters */
	0x00,                                           /* bNumberPowerFilters */

	/* CDC NCM Functional Descriptor */
	USBD_CDC_NCM_FUNC_DESC_SIZE,                    /* bFunctionLength: 6 */
	USB_CDC_CS_INTERFACE,                           /* bDescriptorType: CS_INTERFACE (0x24) */
	USB_CDC_NCM_FUNC_DESC,                          /* bDescriptorSubtype: NCM (0x1A) */
	0x00, 0x01,                                     /* bcdNcmVersion: 1.00 */
	USBD_CDC_NCM_NETWORK_CAPABILITIES,              /* bmNetworkCapabilities: no optional capability */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_D2H,                                        /* bEndpointAddress: dir IN (placeholder) */
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
	USB_CDC_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	USB_CDC_SUBCLASS_RESERVED,                      /* bInterfaceSubClass */
	USB_CDC_DATA_PROTOCOL_NETWORK_TRANSFER_BLOCK,   /* bInterfaceProtocol: NCM */
	0x00,                                           /* iInterface */

	/* Data Interface Descriptor (Alt 1 - Active) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_NCM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x01,                                           /* bAlternateSetting */
	0x02,                                           /* bNumEndpoints */
	USB_CDC_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	USB_CDC_SUBCLASS_RESERVED,                      /* bInterfaceSubClass */
	USB_CDC_DATA_PROTOCOL_NETWORK_TRANSFER_BLOCK,   /* bInterfaceProtocol: NCM */
	0x00,                                           /* iInterface */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_D2H,                                    /* bEndpointAddress: dir IN (placeholder) */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes */
	USB_LOW_BYTE(USB_BULK_HS_MAX_MPS),              /* wMaxPacketSize */
	USB_HIGH_BYTE(USB_BULK_HS_MAX_MPS),
	0x00,                                           /* bInterval */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_H2D,                                    /* bEndpointAddress: dir OUT (placeholder) */
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
	0x80,                                           /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,                                           /* bMaxPower: 100mA */

	/* IAD for CDC NCM (Communication + Data) */
	USB_LEN_IAD_DESC,                               /* bLength */
	USB_DESC_TYPE_IAD,                              /* bDescriptorType */
	0x00,                                           /* bFirstInterface (patched by composite) */
	0x02,                                           /* bInterfaceCount */
	USB_CDC_COMM_INTERFACE_CLASS_CODE,              /* bFunctionClass: CDC */
	USB_CDC_NCM_SUBCLASS_CODE,                      /* bFunctionSubClass: NCM */
	USB_CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC,        /* bFunctionProtocol */
	0x00,                                           /* iFunction */

	/* Communication Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_NCM_COMM_INTERFACE_NUM,                /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x01,                                           /* bNumEndpoints */
	USB_CDC_COMM_INTERFACE_CLASS_CODE,              /* bInterfaceClass: CDC */
	USB_CDC_NCM_SUBCLASS_CODE,                           /* bInterfaceSubClass: NCM */
	USB_CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC,        /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* CDC Header Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	USB_CDC_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_FUNC_DESC_HEADER,                       /* bDescriptorSubtype */
	0x10, 0x01,                                     /* bcdCDC: 1.10 */

	/* CDC Union Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	USB_CDC_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_FUNC_DESC_UNION,                        /* bDescriptorSubtype */
	USBD_CDC_NCM_COMM_INTERFACE_NUM,                /* bControlInterface */
	USBD_CDC_NCM_DATA_INTERFACE_NUM,                /* bSubordinateInterface0 */

	/* CDC Ethernet Networking Functional Descriptor */
	USBD_CDC_NCM_ETHERNET_FUNC_DESC_SIZE,           /* bFunctionLength */
	USB_CDC_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_FUNC_DESC_ETHERNET_NETWORKING,          /* bDescriptorSubtype */
	USBD_CDC_NCM_CLASS_STR_BASE_DEFAULT +
	USBD_CDC_NCM_STR_IDX_MAC,                       /* iMACAddress, runtime patched */
	0x00, 0x00, 0x00, 0x00,                         /* bmEthernetStatistics */
	USB_LOW_BYTE(USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE), /* wMaxSegmentSize */
	USB_HIGH_BYTE(USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE),
	0x00, 0x00,                                     /* wNumberMCFilters */
	0x00,                                           /* bNumberPowerFilters */

	/* CDC NCM Functional Descriptor */
	USBD_CDC_NCM_FUNC_DESC_SIZE,                    /* bFunctionLength: 6 */
	USB_CDC_CS_INTERFACE,                           /* bDescriptorType: CS_INTERFACE (0x24) */
	USB_CDC_NCM_FUNC_DESC,                          /* bDescriptorSubtype: NCM (0x1A) */
	0x00, 0x01,                                     /* bcdNcmVersion: 1.00 */
	USBD_CDC_NCM_NETWORK_CAPABILITIES,              /* bmNetworkCapabilities: no optional capability */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_D2H,                                        /* bEndpointAddress: dir IN (placeholder) */
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
	USB_CDC_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	USB_CDC_SUBCLASS_RESERVED,                      /* bInterfaceSubClass */
	USB_CDC_DATA_PROTOCOL_NETWORK_TRANSFER_BLOCK,   /* bInterfaceProtocol: NCM */
	0x00,                                           /* iInterface */

	/* Data Interface Descriptor (Alt 1 - Active) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_NCM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x01,                                           /* bAlternateSetting */
	0x02,                                           /* bNumEndpoints */
	USB_CDC_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	USB_CDC_SUBCLASS_RESERVED,                      /* bInterfaceSubClass */
	USB_CDC_DATA_PROTOCOL_NETWORK_TRANSFER_BLOCK,   /* bInterfaceProtocol: NCM */
	0x00,                                           /* iInterface */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_D2H,                                    /* bEndpointAddress: dir IN (placeholder) */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes */
	USB_LOW_BYTE(USB_BULK_FS_MAX_MPS),              /* wMaxPacketSize */
	USB_HIGH_BYTE(USB_BULK_FS_MAX_MPS),
	0x00,                                           /* bInterval */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_H2D,                                    /* bEndpointAddress: dir OUT (placeholder) */
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
#ifdef CONFIG_USBD_COMPOSITE
	.set_class_str_base = usbd_cdc_ncm_set_class_str_base,
#endif
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
 * @brief  Effective maximum length of a device-to-host (IN) NTB.
 * @note   USBD_CDC_NCM_NTB_TX_BUF_SIZE is the compile-time capacity of a slot
 *         buffer, while dwNtbInMaxSize is what the host has agreed to accept
 *         (Ref NCM 1.0 6.2.7 SET_NTB_INPUT_SIZE: "the host may use this request
 *         to reduce the maximum NTB size").  An NTB must respect BOTH bounds -
 *         without the runtime term the device would keep emitting over-sized NTBs
 *         after the host shrank the limit, which the host is entitled to drop.
 *
 *         The negotiated minimum (USB_CDC_NCM_MIN_NTB_INPUT_SIZE, 2048) always
 *         leaves room for one full 1514-byte datagram plus NTH16/NDP16 overhead,
 *         so a legal single-frame NTB can never be blocked by this cap.
 * @retval Maximum NTB length in bytes.
 */
static u32 usbd_cdc_ncm_tx_ntb_limit(void)
{
	const usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	u32 limit = ncm->ntb_in_max_size;

	if ((limit == 0U) || (limit > (u32)USBD_CDC_NCM_NTB_TX_BUF_SIZE)) {
		limit = (u32)USBD_CDC_NCM_NTB_TX_BUF_SIZE;
	}

	return limit;
}

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

	if (slot->frame_count >= USBD_CDC_NCM_NTB_IN_MAX_DATAGRAMS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "AGG: max frames reached\n");
		return HAL_ERR_PARA;
	}

	/* Align frame length to 4 bytes */
	aligned_frame_len = (frame_len + USB_CDC_NCM_DATAGRAM_ALIGN - 1) & ~(USB_CDC_NCM_DATAGRAM_ALIGN - 1);

	/* Check the frame against BOTH the slot capacity and the NTB size the host
	 * agreed to accept - see usbd_cdc_ncm_tx_ntb_limit(). */
	if ((slot->data_offset + aligned_frame_len) > usbd_cdc_ncm_tx_ntb_limit()) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "AGG: NTB full (off %u + %u > %u)\n",
				 slot->data_offset, aligned_frame_len, usbd_cdc_ncm_tx_ntb_limit());
		return HAL_ERR_PARA;
	}

	/* Write NDP entry at: NDP_header_start + NDP16 header + frame_count entries */
	entry = (usb_cdc_ncm_ndp16_entry_t *)
			(slot->buf + USBD_CDC_NCM_AGG_NDP_OFFSET + USB_CDC_NCM_NDP16_HEADER_LENGTH +
			 ((u32)slot->frame_count * USB_CDC_NCM_NDP16_ENTRY_LENGTH));
	entry->wDatagramIndex = (u16)slot->data_offset;
	entry->wDatagramLength = (u16)frame_len;

	/* Copy frame data */
	usb_os_memcpy((void *)(slot->buf + slot->data_offset), (const void *)frame, frame_len);

	/* Zero-pad to alignment boundary */
	if (aligned_frame_len > frame_len) {
		usb_os_memset((void *)(slot->buf + slot->data_offset + frame_len),
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
		   (slot->buf + USBD_CDC_NCM_AGG_NDP_OFFSET + USB_CDC_NCM_NDP16_HEADER_LENGTH +
			((u32)slot->frame_count * USB_CDC_NCM_NDP16_ENTRY_LENGTH));
	term->wDatagramIndex = 0;
	term->wDatagramLength = 0;

	/* Finalize NDP header */
	ndp16->dwSignature = USB_CDC_NCM_NDP16_NOCRC_SIGNATURE;
	ndp16->wLength = (u16)(USB_CDC_NCM_NDP16_HEADER_LENGTH +
						   (ndp_entry_count * USB_CDC_NCM_NDP16_ENTRY_LENGTH));
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

/**
 * @brief  Reset the SPSC TX ring: drop every queued NTB and reset the indices.
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 *         Slot buffers are kept allocated - init/deinit owns their lifetime.
 *         The caller must have cleared data_alt_setting first, so the producer
 *         cannot publish a new slot while the indices are being reset.
 * @param  ncm: Device instance.
 * @retval None
 */
static void usbd_cdc_ncm_tx_reset(usbd_cdc_ncm_dev_t *ncm)
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
	ncm->ep_bulk_in.xfer_state = 0U;

	/* Unblock any producer waiting for a free slot so it can observe the
	 * cleared data_alt_setting and bail out. */
	if (ncm->tx_buf_free_sema != NULL) {
		usb_os_sema_give(ncm->tx_buf_free_sema);
	}
#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
	if (ncm->tx_raw_sema != NULL) {
		usb_os_sema_give(ncm->tx_raw_sema);
	}
#endif
}

/**
 * @brief  Activate the data interface: bring up the BULK IN/OUT endpoints.
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 *         Ref NCM 1.0 3.1 and USB 2.0 9.4.10: the BULK endpoints belong to
 *         alternate setting 1 of the data interface, so they may only exist while
 *         the host has selected that alternate setting.
 *         Idempotent: a repeated SET_INTERFACE for the alternate setting already
 *         in use does not re-initialise the endpoints, so an in-flight transfer is
 *         never disturbed.
 * @param  dev: USB device instance.
 * @retval None
 */
static void usbd_cdc_ncm_data_alt_start(usb_dev_t *dev)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usbd_ep_t *ep_bulk_in = &ncm->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &ncm->ep_bulk_out;
	u16 mps;

	if (ncm->data_alt_setting != 0U) {
		return;
	}

	mps = (dev->dev_speed == USB_SPEED_HIGH) ? (u16)USB_BULK_HS_MAX_MPS : (u16)USB_BULK_FS_MAX_MPS;

	/* Initialize BULK IN endpoint */
	ep_bulk_in->xfer_state = 0U;
	ep_bulk_in->info.mps = mps;
	usbd_ep_init(dev, ep_bulk_in);

	/* Initialize BULK OUT endpoint */
	ep_bulk_out->info.mps = mps;
	usbd_ep_init(dev, ep_bulk_out);

	/* Publish the new alternate setting before arming the first OUT transfer, so
	 * a completion that fires immediately observes a consistent state.  This is
	 * also the only place that raises data_alt_setting to 1: keeping the write
	 * inside the helper is what guarantees the field can never disagree with the
	 * real endpoint state. */
	ncm->data_alt_setting = 1U;
	__sync_synchronize();

	/* Start receiving */
	usbd_ep_receive(dev, ep_bulk_out);
}

/**
 * @brief  Deactivate the data interface: tear the BULK IN/OUT endpoints down.
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 *         Fully idempotent, so it is safe to call for an already-inactive data
 *         interface: usbd_ep_deinit() tolerates a never-initialised endpoint and
 *         every other step only resets state.  usbd_cdc_ncm_clear_config() reuses
 *         it for exactly that reason.
 * @param  dev: USB device instance.
 * @retval None
 */
static void usbd_cdc_ncm_data_alt_stop(usb_dev_t *dev)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;

	/* Drop back to the default alternate setting first: no producer may publish a
	 * new NTB once the endpoints are being torn down.  As in _start(), this is the
	 * only place that lowers data_alt_setting to 0. */
	ncm->data_alt_setting = 0U;
	__sync_synchronize();

	usbd_ep_deinit(dev, &ncm->ep_bulk_in);
	usbd_ep_deinit(dev, &ncm->ep_bulk_out);

	/* No OUT transfer can be outstanding any more; drop the deferred length so
	 * the SOF handler does not re-arm a de-initialised endpoint. */
	ncm->rx_pending_len = 0U;

	/* Discard any queued NTBs so the next activation starts clean. */
	usbd_cdc_ncm_tx_reset(ncm);
}

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
/**
 * @brief TX aggregation task.
 *
 * Dequeues raw ethernet frames from tx_raw_rb, aggregates up to
 * USBD_CDC_NCM_NTB_IN_MAX_DATAGRAMS frames into one NTB slot, then
 * publishes it to the SPSC ring (tx_wd++) for the USB ISR to transmit.
 *
 * This task is the sole writer of slot[tx_wd] -- no critical section needed
 * against the USB ISR because the ISR only reads slot[tx_rd].
 */
static void usbd_cdc_ncm_tx_task(void *param)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	u8 *frame_buf;
	u32 frame_len = 0U;
	/* 1 = frame_buf holds a frame that has been dequeued but not yet appended to
	 * an NTB.  It is carried over to the next NTB instead of being discarded:
	 * usbd_cdc_ncm_agg_append() can now legitimately refuse a frame when the
	 * host-negotiated dwNtbInMaxSize is smaller than the slot buffer, and a
	 * dequeued frame must never be lost just because the current NTB is full. */
	u8 frame_pending = 0U;

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

		/* Drain as many frames as we can from the raw ring buffer.  Gate on
		 * data_alt_setting ("the BULK endpoints exist"), not on connect_status
		 * (the upper-layer link state), so a set_link_status() call can never
		 * make this task publish an NTB for a de-initialised endpoint. */
		while ((ncm->data_alt_setting != 0U) &&
			   ((frame_pending != 0U) || !usb_ringbuf_is_empty(&ncm->tx_raw_rb))) {

			/* Wait for NTB ring to have room for a new slot. */
			while (usbd_cdc_ncm_tx_ring_full(ncm)) {
				if (!ncm->tx_task_running || (ncm->data_alt_setting == 0U)) {
					goto task_exit;
				}
				usb_os_sema_take(ncm->tx_buf_free_sema,
								 USBD_CDC_NCM_BULK_TX_TIMEOUT_MS);
			}

			/* Start a new NTB slot. */
			ncm_tx_ntb_t *slot = &ncm->tx_slot[ncm->tx_wd];
			usbd_cdc_ncm_agg_begin(slot, ncm->sequence);

			/* Aggregate frames into this slot until full or raw rb empty. */
			while (slot->frame_count < USBD_CDC_NCM_NTB_IN_MAX_DATAGRAMS) {
				if (frame_pending == 0U) {
					frame_len = usb_ringbuf_remove_head(&ncm->tx_raw_rb,
														frame_buf,
														USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE,
														NULL);
					if (frame_len == 0U) {
						break;
					}
					frame_pending = 1U;
				}

				if (usbd_cdc_ncm_agg_append(slot, frame_buf, frame_len) != HAL_OK) {
					/* This NTB is full.  Keep frame_pending set so the frame goes
					 * into the next NTB rather than being dropped. */
					break;
				}
				frame_pending = 0U;

				usb_os_sema_give(ncm->tx_raw_sema);
			}

			if (slot->frame_count == 0U) {
				/* Nothing fit into an empty NTB.  Cannot happen for a valid frame
				 * (transmit() caps len at 1514 and the negotiated NTB minimum of
				 * 2048 always holds one such datagram), but dropping it here keeps
				 * the loop bounded instead of spinning forever on the same frame. */
				if (frame_pending != 0U) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "TX drop(%u): exceeds NTB limit %u\n",
							 frame_len, usbd_cdc_ncm_tx_ntb_limit());
					frame_pending = 0U;
					usb_os_sema_give(ncm->tx_raw_sema);
				}
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
	usb_os_mfree((void *)frame_buf);
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
	ret = rtos_task_create(&ncm->tx_task, "usbd_cdc_ncm_tx_thread",
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

	usb_os_memcpy((void *) & (ncm->mac[0]), (const void *)mac, USBD_CDC_NCM_MAC_STR_LEN);
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
	const usb_cdc_ncm_ndp16_t *ndp16;
	const usb_cdc_ncm_ndp16_entry_t *entries;
	u32 ndp_offset;
	u32 next_offset;
	u32 header_length;
	u32 entry_count;
	u32 hops = 0U;
	u32 max_hops;
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

	/* wHeaderLength is the offset of the first byte after the NTH16 (Ref NCM 1.0
	 * Table 3-1).  It is the lower bound for every NDP / datagram offset below,
	 * so reject a value that would place data inside the header itself. */
	header_length = nth16->wHeaderLength;
	if ((header_length < USB_CDC_NCM_NTH16_LENGTH) || (header_length > length)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad NTH16 hdr len: %d, rx=%d\n", header_length, length);
		return HAL_ERR_PARA;
	}

	/* wBlockLength is the total size of the NTB (Ref NCM 1.0 Table 3-1).  A value
	 * larger than what actually arrived means the NTB was truncated on the wire,
	 * so the offsets inside it cannot be trusted. */
	if ((nth16->wBlockLength != 0U) && ((u32)nth16->wBlockLength > length)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "NTB truncated: wBlockLength=%d, rx=%d\n",
				 nth16->wBlockLength, length);
		return HAL_ERR_PARA;
	}

	/* Walk the NDP chain (wFpIndex -> wNextFpIndex, 0 terminates).
	 *
	 * The chain comes straight off the wire, so it must be treated as hostile.
	 * Two independent guards bound this loop:
	 *   - forward progress: an NDP must start strictly after the previous one.
	 *     Ref NCM 1.0 3.3: the NDPs of an NTB appear in increasing offset order,
	 *     so this rejects a self-referencing (wNextFpIndex == own offset) or
	 *     otherwise cyclic chain, which would otherwise spin this loop forever
	 *     and wedge the RX thread - the BULK OUT endpoint would then NAK the host
	 *     indefinitely and deinit could never join the thread.
	 *   - hop limit: every NDP occupies at least USB_CDC_NCM_NDP16_MIN_LENGTH
	 *     bytes, so a valid NTB cannot contain more than that many NDPs.  This
	 *     also caps the work a single (still strictly increasing) NTB can cause.
	 * Multi-NDP NTBs stay supported: only malformed chains are rejected. */
	max_hops = length / USB_CDC_NCM_NDP16_MIN_LENGTH;
	ndp_offset = nth16->wFpIndex;
	while (ndp_offset != 0U) {
		if (hops >= max_hops) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "NDP chain too long: hops=%d, rx=%d\n", hops, length);
			break;
		}
		hops++;

		/* An NDP may not overlap the NTH16 it belongs to. */
		if (ndp_offset < header_length) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "NDP inside NTH16: off=%d, hdr=%d\n",
					 ndp_offset, header_length);
			break;
		}

		/* Check that this NDP header fits within the received data. */
		if ((ndp_offset + USB_CDC_NCM_NDP16_MIN_LENGTH) > length) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "NDP beyond RX len: off=%d, len=%d\n",
					 ndp_offset, length);
			break;
		}

		ndp16 = (const usb_cdc_ncm_ndp16_t *)(buf + ndp_offset);

		/* Verify NDP16 signature.  Only the no-CRC variant is accepted because
		 * SET_CRC_MODE rejects anything but mode 0 (Ref NCM 1.0 6.2.11). */
		if (ndp16->dwSignature != USB_CDC_NCM_NDP16_NOCRC_SIGNATURE) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad NDP16 sig: 0x%08X\n", ndp16->dwSignature);
			break;
		}

		/* Validate the NDP itself lies fully within the received buffer before
		 * trusting wLength. wLength must cover at least the NDP header plus one
		 * entry and the terminator, and the whole NDP (header + entry array) must
		 * fit within the received data, otherwise reading an entry below would
		 * read past the buffer. */
		if ((ndp16->wLength < USB_CDC_NCM_NDP16_MIN_LENGTH) ||
			(((u32)ndp_offset + (u32)ndp16->wLength) > length)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad NDP len: wLength=%d, off=%d, rx=%d\n",
					 ndp16->wLength, ndp_offset, length);
			break;
		}

		/* Calculate number of datagram entries
		 * = (wLength - NDP16 header) / bytes per entry */
		entry_count = ((u32)ndp16->wLength - USB_CDC_NCM_NDP16_HEADER_LENGTH) /
					  USB_CDC_NCM_NDP16_ENTRY_LENGTH;

		/* Address the entry array through a pointer rather than ndp16->aEntry[i]:
		 * aEntry is declared with 2 elements as a place-holder, while a real NDP
		 * carries entry_count of them (bounds-checked against the RX buffer just
		 * above), so indexing the declared array would be out of bounds. */
		entries = (const usb_cdc_ncm_ndp16_entry_t *)
				  (buf + ndp_offset + USB_CDC_NCM_NDP16_HEADER_LENGTH);

		/* Iterate over entries and forward each ethernet frame */
		for (i = 0U; i < entry_count; i++) {
			datagram_index = entries[i].wDatagramIndex;
			datagram_length = entries[i].wDatagramLength;

			/* Terminator entry (Ref NCM 1.0 3.3: a zero index or length ends the
			 * entry list). */
			if ((datagram_index == 0U) || (datagram_length == 0U)) {
				break;
			}

			/* Validate the datagram lies inside the received buffer and does not
			 * overlap the NTH16. */
			if ((datagram_index < header_length) ||
				((datagram_index + datagram_length) > length)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR,
						 "Datagram %d out of range: idx=%d+len=%d, hdr=%d, rx=%d\n",
						 i, datagram_index, datagram_length, header_length, length);
				break;
			}

			if ((ncm->cb != NULL) && (ncm->cb->received != NULL)) {
				ncm->cb->received(buf + datagram_index, datagram_length);
			}
		}

		/* Advance to the next NDP in the chain (0 = last), enforcing forward
		 * progress so a cyclic chain cannot loop back. */
		next_offset = ndp16->wNextFpIndex;
		if ((next_offset != 0U) && (next_offset <= ndp_offset)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "NDP chain not increasing: next=%d, cur=%d\n",
					 next_offset, ndp_offset);
			break;
		}
		ndp_offset = next_offset;
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
		event.bNotificationCode = USB_CDC_NOTIFY_NETWORK_CONNECTION;
		event.wValue = ncm->connect_status;
		event.wLength = 0;
		length = USBD_CDC_NCM_NETWORK_CONNECTION_SIZE;
		/* Follow a "connected" notification with a speed-change report; a
		 * "disconnected" notification stands alone (no trailing SPEED). */
		next_state = ncm->connect_status ? NCM_NOTIFY_SPEED : NCM_NOTIFY_NONE;
		usb_os_memcpy((void *)notify_buf, (const void *)&event, sizeof(event));
		break;

	case NCM_NOTIFY_SPEED:
		event.bNotificationCode = USB_CDC_NOTIFY_CONNECTION_SPEED_CHANGE;
		event.wValue = 0;
		event.wLength = 8;
		speed_data.DLBitRate = 0; /* Downstream bits/sec */
		speed_data.ULBitRate = 0; /* Upstream bits/sec */
		length = USBD_CDC_NCM_CONNECTION_SPEED_CHANGE_SIZE;
		next_state = NCM_NOTIFY_NONE;
		usb_os_memcpy((void *)notify_buf, (const void *)&event, sizeof(event));
		usb_os_memcpy((void *)(notify_buf + sizeof(event)), (const void *)&speed_data, sizeof(speed_data));
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
			usb_os_memcpy((void *)ep_intr_in->xfer_buf, (const void *)data, len);
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
	usbd_ep_t *ep_bulk_out = &ncm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ncm->ep_intr_in;
	usb_ep_info_t *info;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if (config != 1U) {
		return HAL_ERR_PARA;
	}

	ncm->dev = dev;

	if (!ncm->from_composite) {
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

	/* Initialize INTR IN endpoint.  It is the only endpoint that belongs to the
	 * configuration itself: the communication interface has a single alternate
	 * setting (alt 0), so its notification endpoint exists as soon as the device
	 * is configured.
	 *
	 * The BULK IN/OUT endpoints are deliberately NOT initialised here.  Ref NCM
	 * 1.0 3.1: they belong to alternate setting 1 of the data interface, whose
	 * default setting (alt 0) has bNumEndpoints = 0.  They are brought up in
	 * usbd_cdc_ncm_data_alt_start() when the host actually selects alt 1. */
	ep_intr_in->xfer_state = 0U;
	info = &ep_intr_in->info;
	info->mps = USBD_CDC_NCM_INTR_IN_PACKET_SIZE;
	usbd_ep_init(dev, ep_intr_in);

	/* Reset the RX state machine so a re-enumeration (clear->set config) starts
	 * from a clean buffer hand-off state rather than inheriting stale indices. */
	ncm->rx_buf_free = 1U;
	ncm->rx_pending_len = 0U;
	ncm->rx_xfer_idx = 0U;
	ep_bulk_out->xfer_buf = ncm->rx_buf[ncm->rx_xfer_idx];

	/* Ref NCM 1.0 6.2.7: dwNtbInMaxSize returns to its default when the device is
	 * re-configured, because the host renegotiates it per configuration. */
	ncm->ntb_in_max_size = USBD_CDC_NCM_DEFAULT_NTB_IN_SIZE;

	/* A fresh configuration always starts from the data interface default
	 * setting (USB 2.0 9.4.7: SET_CONFIGURATION selects alt 0 for every
	 * interface), so the data path starts inactive. */
	ncm->data_alt_setting = 0U;

	return HAL_OK;
}

/**
 * @brief Clear CDC NCM configuration
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @param config: Configuration number
 * @retval None
 */
static void usbd_cdc_ncm_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;

	UNUSED(config);

	/* Tear the data interface down first.  The helper deinitialises the BULK
	 * endpoints, drops the deferred RX length, resets the SPSC TX ring and
	 * releases any producer blocked on a slot - exactly the work this function
	 * used to do inline - and is safe even when the host never selected alt 1
	 * (usbd_ep_deinit() tolerates a never-initialised endpoint). */
	usbd_cdc_ncm_data_alt_stop(dev);

	/* The notification endpoint belongs to the configuration, so it is only
	 * released here. */
	usbd_ep_deinit(dev, &ncm->ep_intr_in);

	/* The data path is gone: clear the link state and abandon any in-flight
	 * notification sequence so the next SET_INTERFACE re-reports from scratch
	 * (mirrors the detach reset in usbd_cdc_ncm_status_changed). */
	ncm->connect_status = 0;
	ncm->notify_state = NCM_NOTIFY_NONE;
	ncm->notify_retry = 0U;
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
	u16 wValue;
	usb_cdc_ncm_ntb_parameters_t ntb_params;

	switch (req_type) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			/* Ref USB 2.0 9.4.10: the endpoints of the selected interface return to
			   their default state, not halted and data toggle DATA0. This holds even
			   for an interface with the default setting only, hosts do send the
			   request in that case. Only the endpoints of the interface addressed by
			   wIndex are touched, so the other interface keeps its data toggle.
			   Ref USB 2.0 Table 9-10: the whole wIndex is the interface number, so a
			   foreign interface or a non-zero high byte leaves the endpoints untouched.

			   Ref USB 2.0 9.4.10 request error: an alternate setting that is not
			   defined in the configuration descriptor - or an interface this function
			   does not own - must be answered with a request error, which the device
			   core turns into an EP0 STALL on a non-HAL_OK return.  The alternate
			   setting is validated BEFORE any state is modified, so a rejected
			   request leaves the interface exactly as it was. */
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = HAL_ERR_PARA;
			} else {
				u8 alt = USB_LOW_BYTE(req->wValue);

				if (req->wIndex == USBD_CDC_NCM_DATA_INTERFACE_NUM) {
					/* The NCM data interface is dual-alt: alt 0 has no endpoints
					 * (idle), alt 1 carries the bulk IN/OUT endpoints (active).
					 * Only alt 1 means the host has armed the data path and will
					 * poll the bulk IN endpoint, so both the endpoints and
					 * connect_status are gated on it.  Reporting "connected" on
					 * alt 0 would let the upper layer bring up the netif and issue
					 * a blocking IN transfer that the host never drains, wedging
					 * the network thread. */
					if (alt > 1U) {
						ret = HAL_ERR_PARA;
					} else if (alt == 1U) {
						/* Bring the BULK endpoints up before reporting the link, so the
						 * host finds a working data path as soon as it sees the
						 * notification. */
						usbd_cdc_ncm_data_alt_start(dev);

						/* USB 2.0 9.4.10: the endpoints of the selected interface return
						 * to their default state.  A freshly initialised endpoint is
						 * already unhalted, but a repeated request for the alternate
						 * setting already in use is short-circuited inside the helper,
						 * so clear the STALL condition explicitly here. */
						usbd_ep_clear_stall(dev, &ncm->ep_bulk_in);
						usbd_ep_clear_stall(dev, &ncm->ep_bulk_out);

						/* Data path active: report link state once.  If the INTR IN
						 * endpoint is momentarily busy the send fails here; notify_retry
						 * lets the SOF handler re-send so the notification is never lost. */
						ncm->notify_state = NCM_NOTIFY_CONNECT;
						ncm->connect_status = 1;
						if (usbd_cdc_ncm_send_notification() != HAL_OK) {
							ncm->notify_retry = 1U;
						}
					} else {
						/* alt 0: the host is tearing the data path down (this is what
						 * "ifdown" does on Linux).  Drop the endpoints so the upper layer
						 * cannot keep queueing NTBs the host will never collect.
						 *
						 * No link-down notification is emitted on purpose: the host asked
						 * for the teardown itself, and pushing a notification into an
						 * INTR IN endpoint it no longer drains would leave that endpoint
						 * busy forever and block the next link-up report. */
						usbd_cdc_ncm_data_alt_stop(dev);
						ncm->connect_status = 0;
						ncm->notify_state = NCM_NOTIFY_NONE;
						ncm->notify_retry = 0U;
					}
				} else if (req->wIndex == USBD_CDC_NCM_COMM_INTERFACE_NUM) {
					/* Communication interface: alt 0 is the only defined setting */
					if (alt != 0U) {
						ret = HAL_ERR_PARA;
					} else {
						usbd_ep_clear_stall(dev, &ncm->ep_intr_in);
					}
				} else {
					/* Foreign interface: not ours to configure */
					ret = HAL_ERR_PARA;
				}

				if ((ret == HAL_OK) && (ncm->cb != NULL) && (ncm->cb->setup != NULL)) {
					ncm->cb->setup(req, NULL);
				}
			}
			break;

		case USB_REQ_GET_INTERFACE:
			/* Ref USB 2.0 9.4.4: report the alternate setting of the interface
			 * addressed by wIndex.  The two interfaces must be answered separately -
			 * the communication interface only has alt 0, so reporting the data
			 * interface's setting for it would be wrong. */
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = HAL_ERR_PARA;
			} else if (req->wIndex == USBD_CDC_NCM_DATA_INTERFACE_NUM) {
				ep0_in->xfer_buf[0] = ncm->data_alt_setting;
				ep0_in->xfer_len = 1U;
				usbd_ep_transmit(dev, ep0_in);
			} else if (req->wIndex == USBD_CDC_NCM_COMM_INTERFACE_NUM) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_len = 1U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				/* Foreign interface: request error */
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
		if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) != USB_REQ_RECIPIENT_INTERFACE) {
			ret = HAL_ERR_PARA;
			break;
		}
		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
			/* Ref NCM 1.0 6.2: every NCM class request is addressed to the
			 * communication interface.  The H2D side already rejects a wrong
			 * interface on SET_NTB_INPUT_SIZE; this keeps the device-to-host
			 * GET requests symmetric with it, so a wrong wIndex cannot make the
			 * device answer a GET the host should not have sent.
			 *
			 * In composite mode this is already guaranteed by the framework, which
			 * routes interface-recipient requests only to the owning function and
			 * rebases wIndex to the local interface number - the value here is the
			 * local 0 either way, so the check is a no-op there.  Standalone mode
			 * (the example) is the one this actually protects. */
			if (req->wIndex != USBD_CDC_NCM_COMM_INTERFACE_NUM) {
				ret = HAL_ERR_PARA;
				break;
			}

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
				usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&ntb_params, sizeof(ntb_params));
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
				/* dwNtbInMaxSize: 32-bit little-endian value (Ref NCM 1.0 6.2.6).
				 * All four bytes are derived from the stored value - hard-coding the
				 * upper half to zero would silently misreport any size >= 64 KiB. */
				ep0_in->xfer_buf[0] = (u8)(ncm->ntb_in_max_size & 0xFFU);
				ep0_in->xfer_buf[1] = (u8)((ncm->ntb_in_max_size >> 8) & 0xFFU);
				ep0_in->xfer_buf[2] = (u8)((ncm->ntb_in_max_size >> 16) & 0xFFU);
				ep0_in->xfer_buf[3] = (u8)((ncm->ntb_in_max_size >> 24) & 0xFFU);
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
				/* Ref NCM 1.0 6.2.6: wValue must be 0, wIndex is the communication
				 * interface, and the 4-byte dwNtbInMaxSize travels in the DATA stage.
				 * The 8-byte form (dwNtbInMaxSize + wNtbInMaxDatagrams) is only legal
				 * when bmNetworkCapabilities advertises it, which this device does not,
				 * so exactly 4 bytes are accepted.
				 *
				 * Validating the request tuple here matters: the data stage handler
				 * unconditionally reads 4 bytes out of the EP0 buffer, so a request
				 * with a shorter wLength would previously have been parsed from
				 * whatever residue the previous control transfer left behind. */
				if ((req->wLength != 4U) || (req->wValue != 0U) ||
					(req->wIndex != USBD_CDC_NCM_COMM_INTERFACE_NUM)) {
					RTK_LOGS(TAG, RTK_LOG_ERROR,
							 "Bad SET_NTB_INPUT_SIZE: wLen=%d wVal=%d wIdx=%d\n",
							 req->wLength, req->wValue, req->wIndex);
					ret = HAL_ERR_PARA;
				} else {
					/* Stash the request and arm EP0 OUT; the payload is validated and
					 * applied in usbd_cdc_ncm_handle_ep0_data_out(). */
					usb_os_memcpy((void *)&ncm->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
					ep0_out->xfer_len = req->wLength;
					ret = usbd_ep_receive(dev, ep0_out);
					if (ret != HAL_OK) {
						/* The data stage never started, so no EP0 OUT completion will
						 * arrive to consume the stashed request.  Invalidate it, else
						 * the next unrelated request's data stage would be applied as
						 * this one's payload. */
						ncm->ctrl_req.bRequest = 0xFFU;
					}
				}
				break;


			case USB_CDC_NCM_SET_MAX_DATAGRAM_SIZE:
				/* Not supported, so answer with a request error (Ref USB 2.0 9.2.7).
				 *
				 * bmNetworkCapabilities advertises no MAX_DATAGRAM_SIZE capability
				 * (USBD_CDC_NCM_NETWORK_CAPABILITIES == 0), and the datagram size is
				 * hard-wired to one full Ethernet frame: the RX buffers, the TX slot
				 * layout and GET_MAX_DATAGRAM_SIZE are all derived from
				 * USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE at compile time, so a
				 * host-supplied value cannot take effect.
				 *
				 * This used to return HAL_OK and discard the value, which told the
				 * host a smaller limit had been accepted while the device kept
				 * sending full-size datagrams.  Rejecting is the honest answer, and
				 * a compliant host never issues the request in the first place
				 * because the capability is not advertised. */
				ret = HAL_ERR_PARA;
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
				if (req->wLength > 0U) {
					usb_os_memcpy((void *)&ncm->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
					ep0_out->xfer_len = req->wLength;
					ret = usbd_ep_receive(dev, ep0_out);
					if (ret != HAL_OK) {
						/* No data stage means no EP0 OUT completion: drop the stashed
						 * request so it cannot be applied to a later transfer. */
						ncm->ctrl_req.bRequest = 0xFFU;
					}
				} else {
					if ((ncm->cb != NULL) && (ncm->cb->setup != NULL)) {
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

	if (ep_addr == ncm->ep_cfg->bulk_in_addr) {
		ep_bulk_in->xfer_state = 0U;

		/* A completion may still surface right after the data interface was torn
		 * down (SET_INTERFACE alt 0 / clear_config / detach).  The ring indices
		 * have already been reset at that point, so retiring a slot here would
		 * desynchronise tx_rd from tx_wd and make the reset ring look non-empty.
		 * Just release the endpoint and stop. */
		if (ncm->data_alt_setting == 0U) {
			return HAL_OK;
		}

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
	} else if (ep_addr == ncm->ep_cfg->intr_in_addr) {
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
	} else {
		/* Return non-zero so composite dispatcher continues iterating. */
		return HAL_ERR_PARA;
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

	if (ep_addr != ncm->ep_cfg->bulk_out_addr) {
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

	/* Do not re-arm once the host has deselected data-interface alt 1: the
	 * endpoint no longer exists. */
	if (ncm->data_alt_setting == 0U) {
		return HAL_OK;
	}

	/* Continue receiving */
	return usbd_ep_receive(ncm->dev, ep_bulk_out);
}

/**
 * @brief   SOF interrupt handler
 * @note    This function is called within an interrupt service routine (ISR) context;
 *          time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 */
static void usbd_cdc_ncm_sof(usb_dev_t *dev)
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
	 * returning (NTB_IN_MAX_DATAGRAMS=1), so slot[wd].frame_count is always
	 * 0 when SOF fires.  No SOF flush needed -- SOF only kicks the consumer. */
#endif /* !CONFIG_USBD_CDC_NCM_TX_AGGREGATION */

	/* The BULK endpoints only exist while data-interface alt 1 is selected, so
	 * neither the TX consumer nor the RX re-arm may run outside of it. */
	if (ncm->data_alt_setting == 0U) {
		return;
	}

	/* Kick consumer: start DMA if idle and ring is non-empty.
	 * Aggregation path: TX task publishes wd; SOF just kicks.
	 * Non-aggregation path: SOF may have just published wd above. */
	usbd_cdc_ncm_tx_kick(ncm);

	if (ncm->rx_pending_len == 0U) {
		return;
	}

	/* Thread still busy - wait for the next SOF. */
	if (ncm->rx_buf_free == 0U) {
		return;
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
	/* Ref USB 2.0 8.5.3.1: a non-zero value makes the core stall the status stage. Default to
	   success, no pending request means this data stage does not belong to NCM (the composite
	   dispatcher already routed it by active_func). The genuine rejections below stay. */
	int ret = HAL_OK;

	if (ncm->ctrl_req.bRequest != 0xFFU) {
		if (ncm->ctrl_req.bRequest == USB_CDC_NCM_SET_NTB_INPUT_SIZE) {
			/* dwNtbInMaxSize: 4-byte little-endian payload from the DATA stage.
			 *
			 * The SETUP stage already rejected any wLength != 4, so the four bytes
			 * below are guaranteed to be host-supplied payload rather than residue
			 * from a previous control transfer.  Re-assert it here: the core does
			 * not hand the received length to this callback, so the stashed request
			 * is the only place that knowledge exists. */
			if (ncm->ctrl_req.wLength != 4U) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SET_NTB_INPUT_SIZE bad wLength %d\n",
						 ncm->ctrl_req.wLength);
				ret = HAL_ERR_PARA;
			} else {
				u32 ntb_in = ((u32)ep0_out->xfer_buf[0]) |
							 ((u32)ep0_out->xfer_buf[1] << 8) |
							 ((u32)ep0_out->xfer_buf[2] << 16) |
							 ((u32)ep0_out->xfer_buf[3] << 24);

				/* Accept the whole range the spec requires a device to support:
				 * from USB_CDC_NCM_MIN_NTB_INPUT_SIZE (2048 for NTB-16, Ref NCM 1.0
				 * Table 6-3) up to the dwNtbInMaxSize advertised in
				 * GET_NTB_PARAMETERS.  The old lower bound used
				 * USB_CDC_NCM_DEFAULT_NTB_INPUT_SIZE (4096), which equals the upper
				 * bound and therefore collapsed the window to the single value 4096
				 * - a host legitimately shrinking the NTB to 2048 (Linux cdc_ncm
				 * does) was rejected.
				 *
				 * The upper bound stays at what the device can actually produce; it
				 * must never be raised beyond USBD_CDC_NCM_NTB_TX_BUF_SIZE, or the
				 * device would promise NTBs it cannot build.  The accepted value is
				 * enforced on the TX path by usbd_cdc_ncm_tx_ntb_limit(). */
				if ((ntb_in >= USB_CDC_NCM_MIN_NTB_INPUT_SIZE) &&
					(ntb_in <= (u32)USBD_CDC_NCM_DEFAULT_NTB_IN_SIZE)) {
					ncm->ntb_in_max_size = ntb_in;
					ret = HAL_OK;
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SET_NTB_INPUT_SIZE out of range: %u\n", ntb_in);
					ret = HAL_ERR_PARA;
				}
			}
		} else if ((ncm->cb != NULL) && (ncm->cb->setup != NULL)) {
			ret = ncm->cb->setup(&ncm->ctrl_req, ep0_out->xfer_buf);
		}
		/* An application without a setup handler simply ignores class-specific requests and the
		   data stage itself was received correctly, so ret stays HAL_OK: stalling would make the
		   host give up on the interface. */
		ncm->ctrl_req.bRequest = 0xFFU; /* Mark as processed */
	}

	return ret;
}

/**
 * @brief Patch the runtime-assigned fields in a configuration descriptor block
 * @note   Replaces direction-only EP placeholders (USB_D2H/USB_H2D) with actual
 *         EP addresses from the EP configuration structure, and rewrites the
 *         iMACAddress string index with the current class string base.
 * @param  desc: Pointer to config descriptor body (starting after config header)
 * @param  len: Length of the descriptor block
 * @param  ep_cfg: EP configuration with actual endpoint addresses
 * @retval None
 */
static void usbd_cdc_ncm_patch_desc(u8 *desc, u16 len,
									const usbd_cdc_ncm_ep_cfg_t *ep_cfg)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;

	for (u16 i = 0; i < len;) {
		u8 dlen = desc[i];
		u8 dtype = desc[i + 1];
		if (dlen == 0) {
			break;
		}

		if ((dtype == USB_DESC_TYPE_ENDPOINT) && (i + 3 <= len)) {
			u8 addr  = desc[i + 2];
			u8 dir   = addr & USB_REQ_DIR_MASK;
			u8 type  = desc[i + 3] & 0x03;

			if ((dir == USB_D2H) && (type == USB_CH_EP_TYPE_BULK)) {
				desc[i + 2] = ep_cfg->bulk_in_addr;
			} else if ((dir == USB_H2D) && (type == USB_CH_EP_TYPE_BULK)) {
				desc[i + 2] = ep_cfg->bulk_out_addr;
			} else if ((dir == USB_D2H) && (type == USB_CH_EP_TYPE_INTR)) {
				desc[i + 2] = ep_cfg->intr_in_addr;
			}
		} else if ((dtype == USB_CDC_CS_INTERFACE) && (dlen >= 4) &&
				   (desc[i + 2] == USB_CDC_FUNC_DESC_ETHERNET_NETWORKING)) {
			/* Ethernet Networking FD: iMACAddress at offset 3. Writes the standalone
			 * default (same as the static template) unless the composite framework
			 * rebased the class string window. */
			desc[i + 3] = (u8)(ncm->cls_str_base + USBD_CDC_NCM_STR_IDX_MAC);
		}
		i += dlen;
	}
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
static u16 usbd_cdc_ncm_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usb_speed_type_t speed = dev->dev_speed;
	u8 desc_type = USB_HIGH_BYTE(req->wValue);
	u8 desc_idx = USB_LOW_BYTE(req->wValue);
	char mac_buf[32] = {0,};
	const u8 *desc = NULL;
	u16 len = 0;
	u8 is_cfg = 0;
	u8 attr = 0x80U;

	if (!ncm->from_composite) {
#ifdef CONFIG_USBD_SELF_POWERED
		attr |= USB_CFG_DESC_OFFSET_ATTR_BIT_SELF_POWERED;
#endif
#ifdef CONFIG_USBD_REMOTE_WAKEUP_EN
		attr |= USB_CFG_DESC_OFFSET_ATTR_BIT_REMOTE_WAKEUP;
#endif
	}

	switch (desc_type) {
	case USB_DESC_TYPE_DEVICE:
		desc = usbd_cdc_ncm_dev_desc;
		len = sizeof(usbd_cdc_ncm_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
#ifndef CONFIG_USB_FS
		if (speed == USB_SPEED_HIGH) {
			desc = usbd_cdc_ncm_hs_config_desc;
			len = sizeof(usbd_cdc_ncm_hs_config_desc);
		} else
#endif
		{
			desc = usbd_cdc_ncm_fs_config_desc;
			len = sizeof(usbd_cdc_ncm_fs_config_desc);
		}
		is_cfg = 1;
		break;

#ifndef CONFIG_USB_FS
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		desc = usbd_cdc_ncm_device_qualifier_desc;
		len = sizeof(usbd_cdc_ncm_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		desc = (speed == USB_SPEED_HIGH) ?
			   usbd_cdc_ncm_fs_config_desc : usbd_cdc_ncm_hs_config_desc;
		len = (speed == USB_SPEED_HIGH) ?
			  sizeof(usbd_cdc_ncm_fs_config_desc) : sizeof(usbd_cdc_ncm_hs_config_desc);
		is_cfg = 1;
		break;
#endif

	case USB_DESC_TYPE_STRING:
		switch (desc_idx) {
		case USBD_IDX_LANGID_STR:
			desc = usbd_cdc_ncm_lang_id_desc;
			len = sizeof(usbd_cdc_ncm_lang_id_desc);
			break;
		case USBD_IDX_MFC_STR:
			len = usbd_get_str_descriptor(USBD_CDC_NCM_MFG_STRING, buf, buf_len);
			break;
		case USBD_IDX_PRODUCT_STR:
			len = usbd_get_str_descriptor((speed == USB_SPEED_HIGH) ?
										  USBD_CDC_NCM_PROD_HS_STRING : USBD_CDC_NCM_PROD_FS_STRING, buf, buf_len);
			break;
		case USBD_IDX_SERIAL_STR:
			len = usbd_get_str_descriptor(USBD_CDC_NCM_SN_STRING, buf, buf_len);
			break;
		default:
			/* Class-specific indices are decided at runtime (rebased by the composite
			 * framework), so they cannot be case labels. Comparing them here also makes
			 * it impossible to shadow the device-global indices above. */
			if (desc_idx == (u8)(ncm->cls_str_base + USBD_CDC_NCM_STR_IDX_MAC)) {
				usbd_cdc_ncm_mac_to_string((const u8 *)(ncm->mac), mac_buf);
				len = usbd_get_str_descriptor(mac_buf, buf, buf_len);
			}
			break;
		}
		break;

	default:
		break;
	}

	if (desc != NULL) {
		/* Truncation is not allowed: a short descriptor is illegal, so stall instead */
		if (len > buf_len) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Desc %d OVSZ %d > %d\n", desc_type, len, buf_len);
			return 0;
		}

		usb_os_memcpy((void *)buf, (const void *)desc, len);
	}

	if (is_cfg != 0) {
		buf[USB_CFG_DESC_OFFSET_TYPE] = desc_type;
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);

		if (!ncm->from_composite) {
			buf[USB_CFG_DESC_OFFSET_ATTR] = attr;
		}

		/* Patch EP addresses and the class string index to actual values */
		usbd_cdc_ncm_patch_desc(buf + USB_LEN_CFG_DESC,
								len - USB_LEN_CFG_DESC,
								ncm->ep_cfg);
	}

	return len;
}

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief Assign the first class-specific string index of this class (composite mode only)
 * @note  This function is called by the composite framework before enumeration.
 * @param base: First class-specific string index for this class
 * @retval Number of class-specific string indices consumed
 */
static u8 usbd_cdc_ncm_set_class_str_base(u8 base)
{
	usbd_cdc_ncm_dev.cls_str_base = base;

	return USBD_CDC_NCM_CLASS_STR_COUNT;
}
#endif

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

		/* The bus is gone, so the endpoints are gone with it: fall back to the
		 * default alternate setting.  Doing this before resetting the ring is what
		 * makes the reset safe - the producer re-checks data_alt_setting on its next
		 * wake and bails out before touching the ring or an endpoint.  It also
		 * matches what the host assumes after the next enumeration (USB 2.0 9.4.7). */
		ncm->data_alt_setting = 0U;
		__sync_synchronize();

		/* Discard any buffered but unsent frames, and wake any producer blocked
		 * waiting for a slot so it can observe the cleared flag and bail out.
		 * NOTE: this runs in ISR context.  The producer (lwIP task or the TX
		 * aggregation task) only touches slot[tx_wd] and only reads tx_rd. */
		usbd_cdc_ncm_tx_reset(ncm);
	}

	if ((ncm->cb != NULL) && (ncm->cb->status_changed != NULL)) {
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
					 dev->is_ready, ncm->connect_status, ncm->notify_state, ncm->notify_retry, ncm->data_alt_setting,
					 ep_bulk_in->xfer_state, ep_bulk_out->xfer_state, ep_intr_in->xfer_state,
					 ncm->rx_buf_free, ncm->rx_pending_len, ncm->rx_xfer_idx,
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
	status = rtos_task_create(&ncm->trace_task, "usbd_cdc_ncm_trace_thread", usbd_cdc_ncm_trace_thread, NULL,
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

/**
 * @brief Initialize CDC NCM class
 * @param cb: User callbacks
 * @param ep_cfg: EP configuration
 * @retval Status
 */
static int usbd_cdc_ncm_private_init(const usbd_cdc_ncm_cb_t *cb, const usbd_cdc_ncm_ep_cfg_t *ep_cfg)
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

	if (ep_cfg == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP cfg\n");
		return HAL_ERR_PARA;
	}

	ncm->ctrl_req.bRequest = 0xFFU;
	/* Standalone default; the composite framework rebases it via set_class_str_base() */
	ncm->cls_str_base = USBD_CDC_NCM_CLASS_STR_BASE_DEFAULT;

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
					usb_os_mfree((void *)ncm->tx_slot[i].buf);
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

	/* Start from the data interface default setting: the BULK endpoints only come
	 * up when the host selects alt 1.  Explicit here because this instance is
	 * re-initialised in place on a hotplug cycle. */
	ncm->data_alt_setting = 0U;

	/* BULK IN use the caller buffer */
	info = &ep_bulk_in->info;
	info->addr = ep_cfg->bulk_in_addr;
	info->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_in->xfer_buf_len = 0;
	ep_bulk_in->xfer_buf = NULL;

	/* Allocate BULK OUT ping-pong buffers */
	info = &ep_bulk_out->info;
	info->addr = ep_cfg->bulk_out_addr;
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
	ret = rtos_task_create(&ncm->rx_task, "usbd_cdc_ncm_rx_thread", usbd_cdc_ncm_rx_thread, NULL,
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
	info->addr = ep_cfg->intr_in_addr;
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
	ncm->ep_cfg = ep_cfg;

	/* Register CDC NCM class driver */

#if USBD_CDC_NCM_STATE_TRACE_ENABLE
	usbd_cdc_ncm_trace_task_init();
#endif

	return HAL_OK;

cleanup_intr_in:
	usb_os_mfree((void *)ep_intr_in->xfer_buf);
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
	usb_os_mfree((void *)ncm->rx_buf[1]);
	ncm->rx_buf[1] = NULL;

cleanup_rx_buf0:
	usb_os_mfree((void *)ncm->rx_buf[0]);
	ncm->rx_buf[0] = NULL;
	ep_bulk_out->xfer_buf = NULL;

exit: {
		u8 i;
		for (i = 0U; i < USBD_CDC_NCM_TX_DEPTH; i++) {
			usb_os_mfree((void *)ncm->tx_slot[i].buf);
			ncm->tx_slot[i].buf = NULL;
		}
	}

	return ret;
}

/* Exported functions --------------------------------------------------------*/

int usbd_cdc_ncm_init(const usbd_cdc_ncm_cb_t *cb, const usbd_cdc_ncm_ep_cfg_t *ep_cfg)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	int ret;

	ret = usbd_cdc_ncm_private_init(cb, ep_cfg);
	if (ret == HAL_OK) {
		ncm->from_composite = 0;
		usbd_register_class(&usbd_cdc_ncm_driver);
	}
	return ret;
}

#ifdef CONFIG_USBD_COMPOSITE
int usbd_composite_cdc_ncm_init(const usbd_cdc_ncm_cb_t *cb, const usbd_cdc_ncm_ep_cfg_t *ep_cfg)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	int ret;

	ret = usbd_cdc_ncm_private_init(cb, ep_cfg);
	if (ret == HAL_OK) {
		ncm->from_composite = 1;
		ret = usbd_composite_register_driver(&usbd_cdc_ncm_driver);
	}
	return ret;
}
#endif

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
	/* Fall back to the default alternate setting so every TX/RX submission path
	 * bails out before anything below is torn down. */
	ncm->data_alt_setting = 0U;
	__sync_synchronize();

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
#ifdef CONFIG_USBD_COMPOSITE
	if (ncm->from_composite) {
		usbd_composite_unregister_driver(&usbd_cdc_ncm_driver);
	} else
#endif
	{
		usbd_unregister_class();
	}

	/* After unregister, no more USB ISRs will fire.  Tear down TX state. */
	{
		u8 i;
		for (i = 0U; i < USBD_CDC_NCM_TX_DEPTH; i++) {
			ncm->tx_slot[i].frame_count = 0;
			ncm->tx_slot[i].data_offset = 0;
			usb_os_mfree((void *)ncm->tx_slot[i].buf);
			ncm->tx_slot[i].buf = NULL;
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
	usb_os_mfree((void *)ep_intr_in->xfer_buf);
	ep_intr_in->xfer_buf = NULL;

	/* Free RX ping-pong buffers. */
	for (u8 i = 0; i < USBD_CDC_NCM_RX_BUF_NUM; i++) {
		usb_os_mfree((void *)ncm->rx_buf[i]);
		ncm->rx_buf[i] = NULL;
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

	/* The BULK IN endpoint only exists while the host has selected data-interface
	 * alt 1 (Ref NCM 1.0 3.1).  Reject early rather than queueing into a ring
	 * whose consumer cannot run: nothing would ever drain it, and the frame would
	 * be handed to a de-initialised endpoint if the guard were left to the ISR. */
	if (ncm->data_alt_setting == 0U) {
		return HAL_BUSY;
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
			/* The sema may have been fired by a teardown (clear_config /
			 * SET_INTERFACE alt 0 / detach) rather than by a real drain.  Test
			 * data_alt_setting, not connect_status: connect_status is the
			 * upper-layer link state and can be forced back to 1 by
			 * usbd_cdc_ncm_set_link_status(), which would keep this loop spinning
			 * on a ring nobody can drain. */
			if (ncm->data_alt_setting == 0U) {
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
	 * NTB_IN_MAX_DATAGRAMS=1 means transmit() always finalize+advances wd
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
				/* The sema may have been fired by a teardown rather than by a real
				 * XFRC - see the aggregation path above for why this tests
				 * data_alt_setting instead of connect_status. */
				if (ncm->data_alt_setting == 0U) {
					return HAL_BUSY;
				}
			}
			usbd_cdc_ncm_agg_begin(slot, ncm->sequence);
		}
		/* Re-check for the non-wait path: if the ring was not full the while-loop
		 * above was skipped entirely.  Without this, a clear_config /
		 * SET_INTERFACE alt 0 / status_changed ISR that fires after the loop but
		 * before finalize could leave a stale NTB queued after the reset. */
		if (ncm->data_alt_setting == 0U) {
			return HAL_BUSY;
		}

		if (usbd_cdc_ncm_agg_append(slot, buf, len) != HAL_OK) {
			/* Should not happen for a valid single-frame NTB: the buf is
			 * sized for MAX_DATAGRAMS=1 frame.  Drop the frame. */
			RTK_LOGS(TAG, RTK_LOG_ERROR, "TX frame too large: %u\n", len);
			return HAL_ERR_PARA;
		}

		/* NTB_IN_MAX_DATAGRAMS=1: always finalize and publish immediately. */
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
