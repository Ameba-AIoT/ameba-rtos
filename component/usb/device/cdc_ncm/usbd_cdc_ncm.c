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

/* Private defines -----------------------------------------------------------*/

#define USBD_CDC_NCM_RX_SPEED_CHECK                       0                     /* CDC NCM rx speed test */
#define USBD_CDC_NCM_TX_SPEED_CHECK                       0                     /* CDC NCM tx speed test */

/* Aggregation NTB layout.
 *
 * Everything below is derived at init from usbd_cdc_ncm_config_t::ntb_in_max_datagrams and
 * cached in usbd_cdc_ncm_dev_t, because the datagram count is a configuration item:
 *   NDP directory  = header(8) + (max_datagrams + 1 terminator) * 4
 *   agg_data_offset = NTH16 + NDP directory, rounded up to the datagram alignment
 *   ntb_tx_buf_size = agg_data_offset + max_datagrams aligned max-size frames
 * The NDP directory always starts immediately after the NTH16, so that offset stays a
 * constant.  Getting agg_data_offset wrong would let the first datagram overwrite the NDP
 * directory, so the slot buffers are always allocated from the same cached values the
 * aggregation path writes with - see usbd_cdc_ncm_tx_layout_init(). */
#define USBD_CDC_NCM_AGG_NDP_OFFSET                   USB_CDC_NCM_NTH16_LENGTH

/* RX bulk working buffer.
 *
 * The host aggregates multiple egress datagrams into one OUT NTB up to the
 * dwNtbOutMaxSize we advertise in GET_NTB_PARAMETERS
 * (usbd_cdc_ncm_config_t::ntb_out_max_size).  The buffer must therefore be large enough
 * to hold a whole max-size OUT NTB, not just a single Ethernet frame, otherwise large
 * aggregated NTBs would be truncated and their trailing datagrams dropped.
 *
 * Take the larger of {one max Ethernet frame + NTB overhead, advertised OUT max size},
 * rounded up to a whole number of HS bulk max packets.  Computed at init into
 * usbd_cdc_ncm_dev_t::rx_buf_size. */
#define USBD_CDC_NCM_BULK_ONE_FRAME_SIZE             (USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE + USB_CDC_NCM_NTH16_LENGTH + USB_CDC_NCM_NDP16_MIN_LENGTH)

/* Interface numbers */
#define USBD_CDC_NCM_COMM_INTERFACE_NUM 0x00U  /**< Communication interface */
#define USBD_CDC_NCM_DATA_INTERFACE_NUM 0x01U  /**< Data interface */

/* Maximum time usbd_cdc_ncm_transmit() will block waiting for TX room when called with
 * block != 0, i.e. usbd_cdc_ncm_config_t::bulk_tx_timeout_ms.
 *
 * Ref USB 2.0 5.7.2 / 8.5: BULK has no guaranteed service interval, the host alone
 * decides when - or whether - it polls BULK IN.  A wait-forever here would let a host
 * that stops polling (S3 sleep, ifdown without SET_INTERFACE alt 0, wedged host
 * driver, dead hub with VBUS still present) freeze the single lwIP tcpip task
 * permanently, taking every other netif down with it, because none of those cases
 * produces the teardown that releases the semaphore.  Backpressure must therefore be
 * bounded: on timeout the frame is dropped (HAL_BUSY) and TCP retransmits, which is
 * the correct best-effort Ethernet semantics.
 *
 * Size it from the drain time of both TX stages at peak throughput.  At the defaults NCM
 * buffers tx_raw_queue_depth raw frames plus tx_ntb_depth NTBs of ntb_in_max_datagrams
 * frames each, i.e. ~13 frames:
 *   USB HS (480 Mbps bus, ~40 MB/s BULK effective): ~13 * 1514 B / 40  MB/s -> 0.5 ms
 *   USB FS ( 12 Mbps bus, ~1.5 MB/s BULK effective): ~13 * 1514 B / 1.5 MB/s ->  13 ms
 * The 20 ms default covers the full-pipe FS drain with headroom, so a normal burst never
 * loses a frame, while still bounding the stall a stuck host can impose.  Raising the queue
 * depths without raising this timeout re-introduces drops under a full-pipe FS burst. */

/* Period of the warning deinit logs while a task does not leave usbd_cdc_ncm_transmit().  A
 * woken caller leaves within microseconds, only a starved one takes longer. */
#define USBD_CDC_NCM_TX_EXIT_WARN_MS                5000U

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

/* Poll period (ms) of the RX thread's blocking wait: bounds how long the thread takes to
 * observe rx_thread_running == 0 at teardown, not a functional parameter.  The thread's
 * stack size and priority come from usbd_cdc_ncm_config_t. */
#define USBD_CDC_NCM_RX_SEMA_TAKE_TIMEOUT_MS          100U

/* Fixed NTB parameters advertised in GET_NTB_PARAMETERS; the negotiable sizes and datagram
 * counts live in usbd_cdc_ncm_config_t. */
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

/* Private macros ------------------------------------------------------------*/

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
static void usbd_cdc_ncm_set_interface_base(u8 base);
#endif
static int usbd_cdc_ncm_intr_in_send(void *data, u16 len);
static int usbd_cdc_ncm_send_notification(void);
static inline u8 usbd_cdc_ncm_char_to_hex(u8 value);
static void usbd_cdc_ncm_mac_to_string(const u8 *mac, char *mac_str);
static void usbd_cdc_ncm_set_mac(const u8 *mac);
static int usbd_cdc_ncm_bulk_send(u8 *buf, u32 len);

static void usbd_cdc_ncm_agg_begin(ncm_tx_ntb_t *slot, u16 sequence);
static int  usbd_cdc_ncm_agg_append(ncm_tx_ntb_t *slot, const u8 *frame, u32 frame_len);
static void usbd_cdc_ncm_agg_finalize(ncm_tx_ntb_t *slot);
static void usbd_cdc_ncm_tx_kick(usbd_cdc_ncm_dev_t *ncm);
static int usbd_cdc_ncm_tx_enqueue(const u8 *buf, u32 len, u8 block);
static void usbd_cdc_ncm_tx_wake_producer(usbd_cdc_ncm_dev_t *ncm);
static int usbd_cdc_ncm_data_alt_start(usb_dev_t *dev);
static void usbd_cdc_ncm_data_alt_stop(usb_dev_t *dev);
static void usbd_cdc_ncm_tx_reset(usbd_cdc_ncm_dev_t *ncm);
static void usbd_cdc_ncm_tx_layout_init(usbd_cdc_ncm_dev_t *ncm, const usbd_cdc_ncm_config_t *cfg);
#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
static void usbd_cdc_ncm_tx_task(void *param);
static int usbd_cdc_ncm_tx_task_init(void);
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
	USB_LOW_BYTE(USB_VID),                          /* idVendor (patched by core from usbd_dev_info_t) */
	USB_HIGH_BYTE(USB_VID),
	USB_LOW_BYTE(USB_PID),                          /* idProduct (patched by core from usbd_dev_info_t) */
	USB_HIGH_BYTE(USB_PID),
	0x00, 0x02,                                     /* bcdDevice: 2.0 */
	USBD_IDX_MFC_STR,                               /* iManufacturer */
	USBD_IDX_PRODUCT_STR,                           /* iProduct */
	USBD_IDX_SERIAL_STR,                            /* iSerialNumber */
	0x01                                            /* bNumConfigurations */
};

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
/* USB Device Qualifier Descriptor */
static const u8 usbd_cdc_ncm_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
	USB_LEN_DEV_QUALIFIER_DESC,                     /* bLength */
	USB_DESC_TYPE_DEVICE_QUALIFIER,                 /* bDescriptorType */
	0x00, 0x02,                                     /* bcdUSB: 2.0 */
	USB_CDC_COMM_INTERFACE_CLASS_CODE,              /* bDeviceClass: same as the device descriptor (USB 2.0 9.6.2) */
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
	0x00,                                           /* iMACAddress, runtime patched */
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
	USBD_CDC_NCM_DEFAULT_INTR_IN_INTERVAL,          /* bInterval (patched at runtime) */

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
	0x00,                                           /* iMACAddress, runtime patched */
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
	USBD_CDC_NCM_DEFAULT_INTR_IN_INTERVAL,          /* bInterval (patched at runtime) */

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
	.set_interface_base = usbd_cdc_ncm_set_interface_base,
#endif
};

/* CDC NCM Device Instance */
static usbd_cdc_ncm_dev_t usbd_cdc_ncm_dev;

/* Exported variables --------------------------------------------------------*/

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
 * @note   usbd_cdc_ncm_dev_t::ntb_tx_buf_size is the allocated capacity of a slot
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

	if ((limit == 0U) || (limit > ncm->ntb_tx_buf_size)) {
		limit = ncm->ntb_tx_buf_size;
	}

	return limit;
}

/**
 * @brief  Derive the TX NTB memory layout from the configured datagram count.
 * @note   Called once from the private init, before any slot buffer is allocated, so that the
 *         buffers and the offsets the aggregation path writes at always come from the same
 *         numbers.  A slot holds the NTH16, the NDP16 directory sized for the configured
 *         datagram count plus its terminator entry, and then that many aligned max-size
 *         frames; the first datagram starts at agg_data_offset.
 * @param  ncm: Device instance whose layout fields are filled in.
 * @param  cfg: Class configuration, already validated by the caller.
 */
static void usbd_cdc_ncm_tx_layout_init(usbd_cdc_ncm_dev_t *ncm, const usbd_cdc_ncm_config_t *cfg)
{
	u32 ndp_size;

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
	ncm->ntb_in_max_datagrams = (cfg->ntb_in_max_datagrams != 0U) ?
								cfg->ntb_in_max_datagrams : USBD_CDC_NCM_DEFAULT_NTB_IN_MAX_DATAGRAMS;
#else
	/* Without the aggregation path the lwIP task builds one-frame NTBs, so any other value
	   would only over-allocate every slot buffer. */
	UNUSED(cfg);
	ncm->ntb_in_max_datagrams = USBD_CDC_NCM_DEFAULT_NTB_IN_MAX_DATAGRAMS;
#endif

	ndp_size = USB_CDC_NCM_NDP16_HEADER_LENGTH +
			   (((u32)ncm->ntb_in_max_datagrams + 1U) * USB_CDC_NCM_NDP16_ENTRY_LENGTH);
	ncm->agg_data_offset = ((u32)USBD_CDC_NCM_AGG_NDP_OFFSET + ndp_size + USB_CDC_NCM_DATAGRAM_ALIGN - 1U)
						   & ~((u32)USB_CDC_NCM_DATAGRAM_ALIGN - 1U);
	ncm->ntb_tx_buf_size = ncm->agg_data_offset +
						   ((u32)ncm->ntb_in_max_datagrams *
							(USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE + USB_CDC_NCM_DATAGRAM_ALIGN));
}

/**
 * @brief Begin a new aggregated NTB16 in the given slot.
 * @param slot:     Ping-pong slot to initialise (caller has reserved it).
 * @param sequence: NTB sequence number for this block.
 */
static void usbd_cdc_ncm_agg_begin(ncm_tx_ntb_t *slot, u16 sequence)
{
	const usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usb_cdc_ncm_nth16_t *nth16 = (usb_cdc_ncm_nth16_t *)slot->buf;

	nth16->dwSignature = USB_CDC_NCM_NTH16_SIGNATURE;
	nth16->wHeaderLength = USB_CDC_NCM_NTH16_LENGTH;
	nth16->wSequence = sequence;
	nth16->wBlockLength = 0; /* Finalised by usbd_cdc_ncm_agg_finalize() */
	nth16->wFpIndex = (u16)USBD_CDC_NCM_AGG_NDP_OFFSET;

	slot->sequence = sequence;
	slot->frame_count = 0;
	slot->data_offset = ncm->agg_data_offset;
}

/**
 * @brief Append an ethernet frame to the slot's aggregated NTB.
 * @param slot:      Ping-pong slot currently being filled.
 * @param frame:     Ethernet frame data.
 * @param frame_len: Frame length.
 * @retval HAL_OK on success, HAL_ERR_PARA if buffer full (caller must flush first).
 */
static int usbd_cdc_ncm_agg_append(ncm_tx_ntb_t *slot, const u8 *frame, u32 frame_len)
{
	const usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usb_cdc_ncm_ndp16_entry_t *entry;
	u32 aligned_frame_len;

	if (slot->frame_count >= ncm->ntb_in_max_datagrams) {
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
	u8 next_wd = (u8)((ncm->tx_wd + 1U) % ncm->tx_ntb_depth);
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
	usb_os_mem_sync();

	if (usbd_cdc_ncm_bulk_send(slot->buf, slot->data_offset) != HAL_OK) {
		/* Submit failed before usbd_ep_transmit() succeeded: no XFRC will
		 * fire.  Drop this NTB, advance rd, clear inflight, and wake the
		 * producer in case the ring was full. */
		ncm->tx_inflight = 0U;
		ncm->tx_rd = (u8)((ncm->tx_rd + 1U) % ncm->tx_ntb_depth);
		usb_os_mem_sync();
		/* A binary semaphore that is already given cannot be given again, which is harmless */
		usb_os_sema_give(ncm->tx_buf_free_sema);
	}
}

/**
 * @brief  Reset the SPSC TX ring: drop every queued NTB.
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 *         Slot buffers are kept allocated - init/deinit owns their lifetime.
 *         Only the consumer side moves (tx_rd catches up with tx_wd): the producer may be
 *         building slot[tx_wd] on another SMP core and owns tx_wd and that slot, which
 *         agg_begin() re-initialises anyway.  A producer that passed its data_alt_setting
 *         check just before the caller cleared it may still publish one NTB afterwards;
 *         it is sent once the host selects alt 1 again, a late frame rather than a
 *         corrupted ring.
 * @param  ncm: Device instance.
 * @retval None
 */
static void usbd_cdc_ncm_tx_reset(usbd_cdc_ncm_dev_t *ncm)
{
	ncm->tx_rd = ncm->tx_wd;
	ncm->tx_inflight = 0;
	usb_os_mem_sync();
	ncm->ep_bulk_in.xfer_state = 0U;

	/* Unblock any producer waiting for TX room so it can observe the cleared
	 * data_alt_setting and bail out. */
	if (ncm->tx_buf_free_sema != NULL) {
		usb_os_sema_give(ncm->tx_buf_free_sema);
	}
#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
	/* Wake the TX task too: it drops the queued raw frames itself, being their only
	 * consumer, see usbd_cdc_ncm_tx_task() */
	if (ncm->tx_raw_sema != NULL) {
		usb_os_sema_give(ncm->tx_raw_sema);
	}
	if (ncm->tx_raw_free_sema != NULL) {
		usb_os_sema_give(ncm->tx_raw_free_sema);
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
 *         On failure every step already taken is undone, so the interface stays in alt 0
 *         and the caller answers SET_INTERFACE with a request error.
 * @param  dev: USB device instance.
 * @retval HAL_OK on success, otherwise the error of the failing endpoint operation.
 */
static int usbd_cdc_ncm_data_alt_start(usb_dev_t *dev)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usbd_ep_t *ep_bulk_in = &ncm->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &ncm->ep_bulk_out;
	u16 mps;
	int ret;

	if (ncm->data_alt_setting != 0U) {
		return HAL_OK;
	}

	mps = (dev->dev_speed == USB_SPEED_HIGH) ? (u16)USB_BULK_HS_MAX_MPS : (u16)USB_BULK_FS_MAX_MPS;

	/* Initialize BULK IN endpoint */
	ep_bulk_in->xfer_state = 0U;
	ep_bulk_in->info.mps = mps;
	ret = usbd_ep_init(dev, ep_bulk_in);
	if (ret != HAL_OK) {
		usbd_cdc_ncm_data_alt_stop(dev);
		return ret;
	}

	/* Initialize BULK OUT endpoint */
	ep_bulk_out->info.mps = mps;
	ret = usbd_ep_init(dev, ep_bulk_out);
	if (ret != HAL_OK) {
		usbd_cdc_ncm_data_alt_stop(dev);
		return ret;
	}

	/* Publish the new alternate setting before arming the first OUT transfer, so
	 * a completion that fires immediately observes a consistent state.  This is
	 * also the only place that raises data_alt_setting to 1: keeping the write
	 * inside the helper is what guarantees the field can never disagree with the
	 * real endpoint state. */
	ncm->data_alt_setting = 1U;
	usb_os_mem_sync();

	/* Start receiving */
	ret = usbd_ep_receive(dev, ep_bulk_out);
	if (ret != HAL_OK) {
		usbd_cdc_ncm_data_alt_stop(dev);
	}

	return ret;
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
	usb_os_mem_sync();

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
 * Consumes raw ethernet frames from tx_raw_rb in place, aggregates up to
 * usbd_cdc_ncm_config_t::ntb_in_max_datagrams frames into one NTB slot, then
 * publishes it to the SPSC ring (tx_wd++) for the USB ISR to transmit.
 *
 * This task is the sole writer of slot[tx_wd] -- no critical section needed
 * against the USB ISR because the ISR only reads slot[tx_rd].
 */
static void usbd_cdc_ncm_tx_task(void *param)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	const usbd_cdc_ncm_config_t *cfg = ncm->cfg;
	u32 tx_sema_timeout_ms = (cfg->tx_sema_timeout_ms != 0U) ?
							 cfg->tx_sema_timeout_ms : USBD_CDC_NCM_DEFAULT_TX_SEMA_TIMEOUT_MS;
	u32 tx_room_timeout_ms = (cfg->bulk_tx_timeout_ms != 0U) ?
							 cfg->bulk_tx_timeout_ms : USBD_CDC_NCM_DEFAULT_BULK_TX_TIMEOUT_MS;
	ncm_tx_ntb_t *slot;
	u8 *frame = NULL;
	u32 frame_len = 0U;

	UNUSED(param);

	while (ncm->tx_task_running) {
		/* Woken by every enqueue, so the timeout is only a safety net, not a poll period.
		   A timeout only re-runs the loop, so its result is not needed. */
		usb_os_sema_take(ncm->tx_raw_sema, tx_sema_timeout_ms);

		if (!ncm->tx_task_running) {
			break;
		}

		/* The data interface is down: drop what was queued for it.  This task is the only
		 * consumer of tx_raw_rb, so it does it rather than the ISR that saw alt 0. */
		if (ncm->data_alt_setting == 0U) {
			usb_ringbuf_drop_all(&ncm->tx_raw_rb);
			usb_os_sema_give(ncm->tx_raw_free_sema);
			continue;
		}

		/* Drain as many frames as we can from the raw ring buffer.  Gate on
		 * data_alt_setting ("the BULK endpoints exist"), not on connect_status
		 * (the link state reported to the host, which also depends on the
		 * uplink), so this task never publishes an NTB for a de-initialised
		 * endpoint. */
		while ((ncm->data_alt_setting != 0U) && !usb_ringbuf_is_empty(&ncm->tx_raw_rb)) {

			/* Wait for NTB ring to have room for a new slot.  Only a teardown ends the
			 * task: an alt 0 just stops this drain, the outer loop then drops the queued
			 * frames and the task keeps serving the next alt 1. */
			while (usbd_cdc_ncm_tx_ring_full(ncm)) {
				if (!ncm->tx_task_running) {
					goto task_exit;
				}
				if (ncm->data_alt_setting == 0U) {
					break;
				}
				usb_os_sema_take(ncm->tx_buf_free_sema, tx_room_timeout_ms);
			}
			if (ncm->data_alt_setting == 0U) {
				break;
			}

			/* Start a new NTB slot. */
			slot = &ncm->tx_slot[ncm->tx_wd];
			usbd_cdc_ncm_agg_begin(slot, ncm->sequence);

			/* Aggregate frames into this slot until full or raw rb empty.  Each frame is
			 * copied straight from its ring node, which stays owned by this task until
			 * popped: a frame the current NTB cannot hold is left queued for the next one. */
			while (slot->frame_count < ncm->ntb_in_max_datagrams) {
				frame = usb_ringbuf_peek_head(&ncm->tx_raw_rb, &frame_len);
				if (frame == NULL) {
					break;
				}

				if (usbd_cdc_ncm_agg_append(slot, frame, frame_len) != HAL_OK) {
					/* This NTB is full, the frame stays queued for the next one */
					break;
				}

				usb_ringbuf_pop_head(&ncm->tx_raw_rb);
				usb_os_sema_give(ncm->tx_raw_free_sema);
			}

			if (slot->frame_count == 0U) {
				/* Nothing fit into an empty NTB.  Cannot happen for a valid frame
				 * (transmit() caps len at 1514 and the negotiated NTB minimum of
				 * 2048 always holds one such datagram), but dropping it here keeps
				 * the loop bounded instead of spinning forever on the same frame. */
				if (frame != NULL) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "TX drop(%u): exceeds NTB limit %u\n",
							 frame_len, usbd_cdc_ncm_tx_ntb_limit());
					usb_ringbuf_pop_head(&ncm->tx_raw_rb);
					usb_os_sema_give(ncm->tx_raw_free_sema);
				}
				continue;
			}

			usbd_cdc_ncm_agg_finalize(slot);
			ncm->sequence++;
			usb_os_mem_sync();
			ncm->tx_wd = (u8)((ncm->tx_wd + 1U) % ncm->tx_ntb_depth);
			usb_os_mem_sync();
		}
	}

task_exit:
	ncm->tx_task = NULL;
	rtos_task_delete(NULL);
}

static int usbd_cdc_ncm_tx_task_init(void)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	const usbd_cdc_ncm_config_t *cfg = ncm->cfg;
	int ret;

	if (ncm->tx_task_running) {
		return HAL_OK;
	}
	ncm->tx_task_running = 1U;
	ret = rtos_task_create(&ncm->tx_task, "usbd_cdc_ncm_tx_thread",
						   usbd_cdc_ncm_tx_task, NULL,
						   (cfg->tx_task_stack_size != 0U) ? cfg->tx_task_stack_size : USBD_CDC_NCM_DEFAULT_TX_TASK_STACK_SIZE,
						   (cfg->tx_task_priority != 0U) ? cfg->tx_task_priority : USBD_CDC_NCM_DEFAULT_TX_TASK_PRIORITY);
	if (ret != RTK_SUCCESS) {
		ncm->tx_task_running = 0U;
		ncm->tx_task = NULL;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create TX task fail\n");
		return HAL_ERR_HW;
	}

	return HAL_OK;
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

	/* Keep the string form in step: the core holds it by pointer for the iMACAddress
	   string descriptor, so it is formatted here rather than on each request */
	usbd_cdc_ncm_mac_to_string(ncm->mac, ncm->mac_str);
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
				 usb_os_get_free_heap_size(),
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
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad NTH16 sig: 0x%08x\n", nth16->dwSignature);
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
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad NDP16 sig: 0x%08x\n", ndp16->dwSignature);
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

			/* Ref NCM 1.0 3.3.1: a datagram is one Ethernet frame without FCS, never longer than
			 * the wMaxSegmentSize this device advertises.  Drop an oversized one and go on. */
			if (datagram_length > USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Datagram %d too long: %d\n", i, datagram_length);
				continue;
			}

			if ((ncm->cb != NULL) && (ncm->cb->received != NULL)) {
				/* The datagram is consumed either way: there is nothing to retry on a callback error */
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
	u32 bitrate;
	u16 length;
	u8 next_state;

	event.bmRequestType = 0xA1;
	/* Class notifications are emitted on the communication (control) interface's
	 * notification endpoint, so wIndex carries the communication interface number. */
	event.wIndex = (u16)(ncm->if_base + USBD_CDC_NCM_COMM_INTERFACE_NUM);

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
		/* Ref CDC 1.2 6.3.3: DLBitRate/ULBitRate are the actual connection bit rates
		 * in bits/s, so report the enumerated bus rate rather than 0 (which would tell
		 * the host the link has no bandwidth at all).  The link is symmetric. */
		bitrate = (ncm->dev->dev_speed == USB_SPEED_HIGH) ? USB_CDC_CONNECTION_BITRATE_HS : USB_CDC_CONNECTION_BITRATE_FS;
		speed_data.DLBitRate = bitrate; /* Downstream bits/sec */
		speed_data.ULBitRate = bitrate; /* Upstream bits/sec */
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
	} else {
		ret = HAL_BUSY;
	}

	return ret;
}

/**
 * @brief  Queue an ethernet frame for transmission
 * @note   Task context only, called by usbd_cdc_ncm_transmit() between its gate updates
 *         with the frame already validated.
 * @param  buf: Ethernet frame data buffer
 * @param  len: Ethernet frame length
 * @param  block: See usbd_cdc_ncm_transmit()
 * @retval Status
 */
static int usbd_cdc_ncm_tx_enqueue(const u8 *buf, u32 len, u8 block)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	u32 tx_timeout_ms;
	int ret;

#if USBD_CDC_NCM_TX_SPEED_CHECK
	static u64 usb_tx_start_time = 0, usb_tx_end_time, usb_tx_interval_time;
	static u64 usb_tx_total_len = 0;

	usb_tx_total_len += len;
	if (usb_tx_start_time == 0) {
		usb_tx_start_time = usb_os_get_timestamp_ms();
	}
#endif

	/* The caller's gate checked data_alt_setting, which is only ever non-zero once init has
	   published ncm->cfg, so this cannot read through a NULL configuration. */
	tx_timeout_ms = (ncm->cfg->bulk_tx_timeout_ms != 0U) ?
					ncm->cfg->bulk_tx_timeout_ms : USBD_CDC_NCM_DEFAULT_BULK_TX_TIMEOUT_MS;

	/* SPSC producer side.  Single producer (lwIP tcpip_thread) -- the
	 * single consumer (USB ISR: SOF + XFRC, serialised) advances tx_rd
	 * after each XFRC.  Producer touches only slot[tx_wd]; consumer
	 * touches only slot[tx_rd].  wd/rd index isolation is the primary
	 * mutual-exclusion mechanism.
	 *
	 * Aggregation: the TX task appends frames into slot[tx_wd] until
	 * ntb_in_max_datagrams is reached or the raw ring runs empty, then finalizes and
	 * publishes immediately -- a partially filled NTB is never left pending,
	 * so no flush timeout is involved.
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
			if (usb_os_sema_take(ncm->tx_raw_free_sema, tx_timeout_ms) != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "TX timeout drop(%u)\n", len);
				return HAL_BUSY;
			}
			/* The sema may have been fired by a teardown (clear_config /
			 * SET_INTERFACE alt 0 / detach) rather than by a real drain.  Test
			 * data_alt_setting, the "BULK endpoints are alive" predicate:
			 * connect_status also depends on the uplink state, so it says nothing
			 * about whether the ring can drain.  is_closing means deinit is waiting
			 * for this call to leave. */
			if ((ncm->data_alt_setting == 0U) || (ncm->is_closing != 0U)) {
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
	 * ntb_in_max_datagrams == 1 means transmit() always finalize+advances wd
	 * before returning, so SOF never sees a partially filled slot[wd] and
	 * never writes slot[wd] -- no critical section needed. */
	{
		ncm_tx_ntb_t *slot = &ncm->tx_slot[ncm->tx_wd];

		while (usbd_cdc_ncm_tx_ring_full(ncm)) {
			if (block == 0U) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "TX drop(%u): ring full\n", len);
				return HAL_BUSY;
			}
			if (usb_os_sema_take(ncm->tx_buf_free_sema, tx_timeout_ms) != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "TX timeout drop(%u)\n", len);
				return HAL_BUSY;
			}
			/* The sema may have been fired by a teardown rather than by a real
			 * XFRC - see the aggregation path above for why this tests
			 * data_alt_setting instead of connect_status.  is_closing means deinit is
			 * waiting for this call to leave. */
			if ((ncm->data_alt_setting == 0U) || (ncm->is_closing != 0U)) {
				return HAL_BUSY;
			}
		}
		/* Unconditionally start a fresh NTB.  With ntb_in_max_datagrams == 1 slot[tx_wd] is
		 * never partially filled on entry -- every path out of transmit() either
		 * publishes the slot or leaves it untouched, and agg_begin() resets
		 * frame_count/data_offset itself.  A "frame_count == 0" guard here would
		 * therefore always be taken, while adding an unsynchronised read of a field
		 * the USB ISR clears on the other SMP core: on a stale non-zero value it
		 * would skip agg_begin(), agg_append() would then reject the frame, and
		 * nothing would ever clear frame_count again (only a completed transfer
		 * does) -- permanently retiring this slot from the ring. */
		usbd_cdc_ncm_agg_begin(slot, ncm->sequence);

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
		usb_os_mem_sync();
		ncm->tx_wd = (u8)((ncm->tx_wd + 1U) % ncm->tx_ntb_depth);
		usb_os_mem_sync();
		ret = HAL_OK;
	}

#endif /* CONFIG_USBD_CDC_NCM_TX_AGGREGATION */

#if USBD_CDC_NCM_TX_SPEED_CHECK
	usb_tx_end_time = usb_os_get_timestamp_ms();
	usb_tx_interval_time = (usb_tx_end_time - usb_tx_start_time) * RTOS_TICK_RATE_MS;

	if (usb_tx_interval_time >= 3000) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Heap %d TX %dB in %d ms, %d Kbps\n",
				 usb_os_get_free_heap_size(),
				 (u32)usb_tx_total_len, (u32)usb_tx_interval_time, (u32)((usb_tx_total_len * 8 * 1000) / (usb_tx_interval_time * 1024)));
		usb_tx_start_time = usb_tx_end_time;
		usb_tx_total_len = 0;
	}
#endif

	return ret;
}

/**
 * @brief  Wake a producer blocked in usbd_cdc_ncm_tx_enqueue() waiting for TX room
 * @param  ncm: Device instance
 * @retval None
 */
static void usbd_cdc_ncm_tx_wake_producer(usbd_cdc_ncm_dev_t *ncm)
{
#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
	if (ncm->tx_raw_free_sema != NULL) {
		usb_os_sema_give(ncm->tx_raw_free_sema);
	}
#else
	if (ncm->tx_buf_free_sema != NULL) {
		usb_os_sema_give(ncm->tx_buf_free_sema);
	}
#endif
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
	usbd_ep_t *ep_intr_in = &ncm->ep_intr_in;
	usb_ep_info_t *info;
	int ret;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if (config != 1U) {
		return HAL_ERR_PARA;
	}

	ncm->dev = dev;

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
	ret = usbd_ep_init(dev, ep_intr_in);

	/* Drop a frame deferred before the reset.  rx_buf_free/rx_xfer_idx are left alone: the RX
	 * thread may still be parsing the buffer it was handed, and only it may release that buffer,
	 * so the armed buffer stays the other one. */
	ncm->rx_pending_len = 0U;

	/* Ref NCM 1.0 6.2.7: dwNtbInMaxSize returns to its default when the device is
	 * re-configured, because the host renegotiates it per configuration. */
	ncm->ntb_in_max_size = ncm->ntb_in_max_size_cap;

	/* A fresh configuration always starts from the data interface default
	 * setting (USB 2.0 9.4.7: SET_CONFIGURATION selects alt 0 for every
	 * interface), so the data path starts inactive. */
	ncm->data_alt_setting = 0U;

	/* A failure makes the core reject SET_CONFIGURATION */
	return ret;
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
			} else if ((USB_HIGH_BYTE(req->wValue) != 0U) || (req->wLength != 0U)) {
				/* Ref USB 2.0 Table 9-3: the whole wValue is the alternate setting and wLength is zero */
				ret = HAL_ERR_PARA;
			} else {
				u8 alt = USB_LOW_BYTE(req->wValue);

				if (req->wIndex == USBD_CDC_NCM_DATA_INTERFACE_NUM) {
					/* The NCM data interface is dual-alt: alt 0 has no endpoints
					 * (idle), alt 1 carries the bulk IN/OUT endpoints (active).
					 * Only alt 1 means the host has armed the data path and will
					 * poll the bulk IN endpoint, so both the endpoints and
					 * connect_status are gated on it (connect_status additionally
					 * needs the uplink to be up).  Reporting "connected" on
					 * alt 0 would let the upper layer bring up the netif and issue
					 * a blocking IN transfer that the host never drains, wedging
					 * the network thread. */
					if (alt > 1U) {
						ret = HAL_ERR_PARA;
					} else if (alt == 1U) {
						/* Bring the BULK endpoints up before reporting the link, so the
						 * host finds a working data path as soon as it sees the
						 * notification.  A failure leaves alt 0 in place and STALLs the
						 * request, rather than reporting an alternate setting whose
						 * endpoints do not work. */
						ret = usbd_cdc_ncm_data_alt_start(dev);
						if (ret == HAL_OK) {
							/* USB 2.0 9.4.10: the endpoints of the selected interface return
							 * to their default state.  A freshly initialised endpoint is
							 * already unhalted, but a repeated request for the alternate
							 * setting already in use is short-circuited inside the helper,
							 * so clear the STALL condition explicitly here. */
							usbd_ep_clear_stall(dev, &ncm->ep_bulk_in);
							usbd_ep_clear_stall(dev, &ncm->ep_bulk_out);

							/* Data path active: report the real uplink state once, not a
							 * hard-coded "up".  If the INTR IN endpoint is momentarily busy the
							 * send fails here; notify_retry lets the SOF handler re-send so the
							 * notification is never lost. */
							ncm->notify_state = NCM_NOTIFY_CONNECT;
							ncm->connect_status = ncm->uplink_status;
							if (usbd_cdc_ncm_send_notification() != HAL_OK) {
								ncm->notify_retry = 1U;
							}
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
					/* Notification only: the request has already been applied above */
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
			} else if ((req->wValue != 0U) || (req->wLength != 1U)) {
				/* Ref USB 2.0 Table 9-3: wValue is zero and wLength is one */
				ret = HAL_ERR_PARA;
			} else if (req->wIndex == USBD_CDC_NCM_DATA_INTERFACE_NUM) {
				ep0_in->xfer_buf[0] = ncm->data_alt_setting;
				ep0_in->xfer_len = 1U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else if (req->wIndex == USBD_CDC_NCM_COMM_INTERFACE_NUM) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_len = 1U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else {
				/* Foreign interface: request error */
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_GET_STATUS:
			/* Ref USB 2.0 9.4.5 and Table 9-3: an interface-recipient GET_STATUS returns
			   two reserved zero bytes and wLength is two.  A request naming an interface
			   this function does not own is a request error (9.2.7), which the device core
			   turns into an EP0 STALL on a non-HAL_OK return.  The core only range-checks
			   wIndex against USBD_MAX_NUM_INTERFACES, so ownership is the class's own job,
			   exactly as for SET_INTERFACE / GET_INTERFACE above. */
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = HAL_ERR_PARA;
			} else if ((req->wValue != 0U) || (req->wLength != 2U)) {
				ret = HAL_ERR_PARA;
			} else if ((req->wIndex == USBD_CDC_NCM_COMM_INTERFACE_NUM) || (req->wIndex == USBD_CDC_NCM_DATA_INTERFACE_NUM)) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else {
				/* Foreign interface: request error */
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
		/* Ref NCM 1.0 6.2: every NCM class request is addressed to the communication
		 * interface, so a wrong wIndex cannot make the device answer a GET or apply a SET
		 * the host should not have sent.
		 *
		 * In composite mode this is already guaranteed by the framework, which routes
		 * interface-recipient requests only to the owning function and rebases wIndex to the
		 * local interface number - the value here is the local 0 either way, so the check is
		 * a no-op there.  Standalone mode (the example) is the one this actually protects. */
		if (req->wIndex != USBD_CDC_NCM_COMM_INTERFACE_NUM) {
			ret = HAL_ERR_PARA;
			break;
		}
		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
			/* Device-to-Host: prepare response data in EP0 buffer */
			switch (req->bRequest) {
			case USB_CDC_NCM_GET_NTB_PARAMETERS:
				ntb_params.wLength = sizeof(usb_cdc_ncm_ntb_parameters_t);
				ntb_params.bmNtbFormatsSupported = USB_CDC_NCM_NTB16_SUPPORTED;
				/* Ref NCM 1.0 6.2.1: the NTB Parameter Structure reports the device's
				 * fixed capability, not the value last negotiated by SET_NTB_INPUT_SIZE
				 * (Ref NCM 1.0 6.2.5).  Reporting ncm->ntb_in_max_size would tell a host
				 * that re-queries after shrinking the input size that the device can
				 * never accept more, permanently capping IN aggregation. */
				ntb_params.dwNtbInMaxSize = ncm->ntb_in_max_size_cap;
				ntb_params.wNdbInDivisor = USBD_CDC_NCM_NTB_DIVISOR;
				ntb_params.wNdbInPayloadRemainder = USBD_CDC_NCM_NTB_PAYLOAD_REMAINDER;
				ntb_params.wNdbInAlignment = USBD_CDC_NCM_NTB_ALIGNMENT;
				ntb_params.dwNtbOutMaxSize = ncm->ntb_out_max_size;
				ntb_params.wNdbOutDivisor = USBD_CDC_NCM_NTB_DIVISOR;
				ntb_params.wNdbOutPayloadRemainder = USBD_CDC_NCM_NTB_PAYLOAD_REMAINDER;
				ntb_params.wNdbOutAlignment = USBD_CDC_NCM_NTB_ALIGNMENT;
				ntb_params.wNtbOutMaxDatagrams = (ncm->cfg->ntb_out_max_datagrams != 0U) ?
												 ncm->cfg->ntb_out_max_datagrams : USBD_CDC_NCM_DEFAULT_NTB_OUT_MAX_DATAGRAMS;
				ntb_params.wReserved1 = 0;
				usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&ntb_params, sizeof(ntb_params));
				ep0_in->xfer_len = sizeof(ntb_params);
				ret = usbd_ep_transmit(dev, ep0_in);
				break;

			case USB_CDC_NCM_GET_NTB_FORMAT:
				/* wNtbFormatSelected: 16-bit value */
				ep0_in->xfer_buf[0] = ncm->ntb_format;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				ret = usbd_ep_transmit(dev, ep0_in);
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
				ret = usbd_ep_transmit(dev, ep0_in);
				break;


			case USB_CDC_NCM_GET_MAX_DATAGRAM_SIZE:
				ep0_in->xfer_buf[0] = USB_LOW_BYTE(USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE);
				ep0_in->xfer_buf[1] = USB_HIGH_BYTE(USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE);
				ep0_in->xfer_len = 2U;
				ret = usbd_ep_transmit(dev, ep0_in);
				break;

			default: {
				/* The response length is defined by the request (Ref NCM 1.0 6.2 and
				 * CDC 1.2 6.2), not by the host: scheduling wLength bytes would transmit
				 * whatever the callback did not write. */
				u16 rsp_len;

				switch (req->bRequest) {
				case USB_CDC_NCM_GET_NET_ADDRESS:
					/* Ref NCM 1.0 6.2.2: the 6-byte EUI-48 address */
					rsp_len = USB_CDC_NCM_NET_ADDRESS_RSP_LEN;
					break;
				case USB_CDC_NCM_GET_CRC_MODE:
					/* Ref NCM 1.0 6.2.10: a 16-bit bmCrcMode */
					rsp_len = USB_CDC_NCM_CRC_MODE_RSP_LEN;
					break;
				case USB_CDC_GET_ETHERNET_STATISTIC:
					/* Ref CDC 1.2 6.2.6: a single 32-bit counter */
					rsp_len = USB_CDC_ETHERNET_STATISTIC_RESPONSE_LEN;
					break;
				case USB_CDC_GET_ETHERNET_POWER_MANAGEMENT:
					/* Ref CDC 1.2 6.2.4: a 16-bit boolean */
					rsp_len = USB_CDC_ETHERNET_POWER_MANAGEMENT_RESPONSE_LEN;
					break;
				default:
					rsp_len = 0U;
					break;
				}

				if ((rsp_len == 0U) || (ncm->cb == NULL) || (ncm->cb->setup == NULL)) {
					/* Unsupported request or nobody can produce the data stage: report a
					 * request error instead of leaving the host waiting for a reply. */
					ret = HAL_ERR_PARA;
				} else {
					if (rsp_len > (u16)ep0_in->xfer_buf_len) {
						rsp_len = (u16)ep0_in->xfer_buf_len;
					}
					/* The EP0 buffer is shared with descriptor and OUT traffic, so clear the
					 * response window: a callback writing fewer bytes must not leak stale
					 * control-buffer contents to the host. */
					usb_os_memset((void *)ep0_in->xfer_buf, 0, rsp_len);
					ret = ncm->cb->setup(req, ep0_in->xfer_buf);
					if (ret == HAL_OK) {
						/* Ref USB 2.0 9.2.7: never return more than wLength bytes; the core
						 * STALLs EP0 on a non-HAL_OK return, so propagate a submit failure
						 * instead of leaving the host in a control-transfer timeout. */
						ep0_in->xfer_len = (req->wLength < rsp_len) ? req->wLength : rsp_len;
						ret = usbd_ep_transmit(dev, ep0_in);
					}
				}
				break;
			}
			}
		} else {
			/* Host-to-Device */
			wValue = req->wValue;

			switch (req->bRequest) {
			case USB_CDC_NCM_SET_NTB_FORMAT:
				/* wValue: 0=NTB16, 1=NTB32.  Ref NCM 1.0 6.2.8: no data stage, wLength is zero */
				if ((wValue == 0U) && (req->wLength == 0U)) {
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
					ncm->ctrl_req_pending = 1U;
					ep0_out->xfer_len = req->wLength;
					/* The core does not report a short data stage, so clear the payload
					 * window: missing bytes then read as 0 instead of the residue of the
					 * previous control transfer, and fail the range check below. */
					usb_os_memset((void *)ep0_out->xfer_buf, 0, req->wLength);
					ret = usbd_ep_receive(dev, ep0_out);
					if (ret != HAL_OK) {
						/* The data stage never started, so no EP0 OUT completion will
						 * arrive to consume the stashed request.  Invalidate it, else
						 * the next unrelated request's data stage would be applied as
						 * this one's payload. */
						ncm->ctrl_req_pending = 0U;
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
				/* Only no-CRC (mode 0) supported.  Ref NCM 1.0 6.2.11: no data stage, wLength is zero */
				if ((wValue == USB_CDC_NCM_CRC_MODE_NONE) && (req->wLength == 0U)) {
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
					ncm->ctrl_req_pending = 1U;
					ep0_out->xfer_len = req->wLength;
					ret = usbd_ep_receive(dev, ep0_out);
					if (ret != HAL_OK) {
						/* No data stage means no EP0 OUT completion: drop the stashed
						 * request so it cannot be applied to a later transfer. */
						ncm->ctrl_req_pending = 0U;
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

	if (ep_addr == ncm->cfg->bulk_in_addr) {
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
			usb_os_mem_sync();
			ncm->tx_rd = (u8)((ncm->tx_rd + 1U) % ncm->tx_ntb_depth);
			ncm->tx_inflight = 0U;
			usb_os_mem_sync();
			usb_os_sema_give(ncm->tx_buf_free_sema);
			usbd_cdc_ncm_tx_kick(ncm);
		}
#if USBD_CDC_NCM_STATE_TRACE_ENABLE
		ncm->dbg_bulk_in_done_cnt++;
#endif
	} else if (ep_addr == ncm->cfg->intr_in_addr) {
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

	if (ep_addr != ncm->cfg->bulk_out_addr) {
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
			usb_os_mem_sync();
			/* rx_buf_free gates the hand-off, so the semaphore is never pending here */
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
 * @note    SOF never flushes a half-filled NTB, because neither TX path can leave
 *          one: the non-aggregation producer finalizes and advances tx_wd before
 *          returning (NTB_IN_MAX_DATAGRAMS=1), and the aggregation TX task stops
 *          appending as soon as the raw ring runs empty.  slot[tx_wd] is therefore
 *          always empty when SOF fires, and SOF only kicks the consumer.
 */
static void usbd_cdc_ncm_sof(usb_dev_t *dev)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usbd_ep_t *ep_bulk_out = &ncm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ncm->ep_intr_in;
	u8 link_up;

	UNUSED(dev);

	/* Report a link change.  The host sees the link up only while the data interface is
	 * active and the uplink is up.  usbd_cdc_ncm_set_link_status() only writes
	 * uplink_status, so connect_status/notify_state/notify_retry have a single writer -
	 * the USB ISR - and a change requested from another core can never be overwritten. */
	link_up = ((ncm->data_alt_setting != 0U) && (ncm->uplink_status != 0U)) ? 1U : 0U;
	if (link_up != ncm->connect_status) {
		ncm->connect_status = link_up;
		ncm->notify_state = NCM_NOTIFY_CONNECT;
		ncm->notify_retry = 1U;
	}

	/* Retry a previously-failed notification when the endpoint is free. */
	if (ncm->notify_retry && ep_intr_in->xfer_state == 0U) {
		if (usbd_cdc_ncm_send_notification() == HAL_OK) {
			ncm->notify_retry = 0U;
		}
	}

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

	if (usbd_ep_receive(ncm->dev, ep_bulk_out) != HAL_OK) {
		/* Nobody to report to from the SOF ISR: record it, the host keeps seeing NAK */
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ncm->cfg->bulk_out_addr);
	}
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

	if (ncm->ctrl_req_pending != 0U) {
		/* Consume the pending request first: a single data stage belongs to exactly one setup
		   packet, so the saved request must not be replayed by a later EP0 OUT event. */
		ncm->ctrl_req_pending = 0U;

		if (ncm->ctrl_req.bRequest == USB_CDC_NCM_SET_NTB_INPUT_SIZE) {
			/* dwNtbInMaxSize: 4-byte little-endian payload from the DATA stage.
			 *
			 * The SETUP stage already rejected any wLength != 4 and cleared the four
			 * bytes, so a short data stage leaves zeros rather than residue from a
			 * previous control transfer.  Re-assert the length here: the core does
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
				 * The upper bound is exactly what GET_NTB_PARAMETERS advertised, so the
				 * device never accepts a size it did not offer.  It may exceed one slot
				 * buffer: the accepted value only lowers the cap, and every emitted NTB is
				 * clamped to the slot capacity by usbd_cdc_ncm_tx_ntb_limit(). */
				if ((ntb_in >= USB_CDC_NCM_MIN_NTB_INPUT_SIZE) &&
					(ntb_in <= ncm->ntb_in_max_size_cap)) {
					ncm->ntb_in_max_size = ntb_in;
					ret = HAL_OK;
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SET_NTB_INPUT_SIZE out of range: %u\n", ntb_in);
					ret = HAL_ERR_PARA;
				}
			}
		} else if ((ncm->cb != NULL) && (ncm->cb->setup != NULL)) {
			/* cb is released by usbd_cdc_ncm_deinit(), which may run between the setup and the
			   data stage of an H2D request, so both the structure and the handler are checked. */
			ret = ncm->cb->setup(&ncm->ctrl_req, ep0_out->xfer_buf);
		}
		/* An application without a setup handler simply ignores class-specific requests and the
		   data stage itself was received correctly, so ret stays HAL_OK: stalling would make the
		   host give up on the interface. */
	}

	return ret;
}

/**
 * @brief Patch the runtime-assigned fields in a configuration descriptor block
 * @note   Replaces direction-only EP placeholders (USB_D2H/USB_H2D) with actual
 *         EP addresses from the class configuration, applies the configured INTR IN
 *         interval, rewrites the iMACAddress and iInterface string indices with the indices
 *         the core assigned, and rebases the Union FD interface cross-references with the
 *         current interface base, which is the standalone default unless the composite
 *         framework rebased it.
 * @param  desc: Pointer to config descriptor body (starting after config header)
 * @param  len: Length of the descriptor block
 * @param  speed: Speed the descriptor describes, selects the INTR IN interval
 * @retval None
 */
static void usbd_cdc_ncm_patch_desc(u8 *desc, u16 len, usb_speed_type_t speed)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	const usbd_cdc_ncm_config_t *cfg = ncm->cfg;
	u8 interval;

	interval = (speed == USB_SPEED_HIGH) ? cfg->intr_in_interval_hs : cfg->intr_in_interval_fs;
	if (interval == 0U) {
		interval = USBD_CDC_NCM_DEFAULT_INTR_IN_INTERVAL;
	}

	for (u16 i = 0; i < len;) {
		u8 dlen = desc[i];
		u8 dtype;

		/* A descriptor occupies desc[i .. i + dlen - 1], so anything short of a
		 * complete 2-byte header (bLength + bDescriptorType), or a descriptor
		 * claiming to run past the end of the block, means the block is malformed:
		 * stop rather than index outside it. */
		if ((dlen == 0U) || ((u32)i + 2U > (u32)len) || ((u32)i + (u32)dlen > (u32)len)) {
			break;
		}
		dtype = desc[i + 1];

		if ((dtype == USB_DESC_TYPE_ENDPOINT) && (dlen >= USB_LEN_EP_DESC)) {
			u8 addr  = desc[i + 2];
			u8 dir   = addr & USB_REQ_DIR_MASK;
			u8 type  = desc[i + 3] & USB_EP_XFER_TYPE_MASK;

			if ((dir == USB_D2H) && (type == USB_CH_EP_TYPE_BULK)) {
				desc[i + 2] = cfg->bulk_in_addr;
			} else if ((dir == USB_H2D) && (type == USB_CH_EP_TYPE_BULK)) {
				desc[i + 2] = cfg->bulk_out_addr;
			} else if ((dir == USB_D2H) && (type == USB_CH_EP_TYPE_INTR)) {
				desc[i + 2] = cfg->intr_in_addr;
				desc[i + 6] = interval;             /* bInterval */
			}
		} else if ((dtype == USB_DESC_TYPE_INTERFACE) && (dlen >= USB_LEN_IF_DESC) &&
				   (desc[i + 5] == USB_CDC_COMM_INTERFACE_CLASS_CODE)) {
			/* The data interface keeps the 0x00 placeholder: only the communication
			 * interface is named, matching the CDC ACM class. */
			desc[i + 8] = ncm->itf_str_idx;         /* iInterface */
		} else if ((dtype == USB_CDC_CS_INTERFACE) && (dlen >= 4) &&
				   (desc[i + 2] == USB_CDC_FUNC_DESC_ETHERNET_NETWORKING)) {
			/* Ethernet Networking FD: iMACAddress at offset 3. Writes the standalone
			 * default (same as the static template) unless the composite framework
			 * rebased the class string window. */
			desc[i + 3] = ncm->mac_str_idx;
		} else if ((dtype == USB_CDC_CS_INTERFACE) && (dlen >= 5) &&
				   (desc[i + 2] == USB_CDC_FUNC_DESC_UNION)) {
			/* Union FD: bControlInterface at offset 3, bSubordinateInterface0 at offset 4
			 * (Ref CDC 1.2 5.2.3.8). The composite framework only rebases the standard
			 * Interface and IAD descriptors, so this cross-reference is ours to fix up. */
			desc[i + 3] = (u8)(ncm->if_base + USBD_CDC_NCM_COMM_INTERFACE_NUM);
			desc[i + 4] = (u8)(ncm->if_base + USBD_CDC_NCM_DATA_INTERFACE_NUM);
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
	usb_speed_type_t speed = dev->dev_speed;
	/* Speed the returned configuration describes, which selects the bInterval encoding */
	usb_speed_type_t desc_speed = speed;
	u8 desc_type = USB_HIGH_BYTE(req->wValue);
	const u8 *desc = NULL;
	u16 len = 0;
	u8 is_cfg = 0;

	/* Ref USB 2.0 9.4.3: the index only selects among string and configuration descriptors;
	   with a single configuration, a non-string index other than 0 names no descriptor. */
	if ((desc_type != USB_DESC_TYPE_STRING) && (USB_LOW_BYTE(req->wValue) != 0U)) {
		return 0;
	}

	switch (desc_type) {
	case USB_DESC_TYPE_DEVICE:
		desc = usbd_cdc_ncm_dev_desc;
		len = sizeof(usbd_cdc_ncm_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
#ifndef CONFIG_SUPPORT_USB_FS_ONLY
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

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		desc = usbd_cdc_ncm_device_qualifier_desc;
		len = sizeof(usbd_cdc_ncm_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		/* Ref USB 2.0 9.6.4: describes the configuration at the speed the device is not
		   running at, so its bInterval is patched for that speed, not the current one */
		desc = (speed == USB_SPEED_HIGH) ?
			   usbd_cdc_ncm_fs_config_desc : usbd_cdc_ncm_hs_config_desc;
		len = (speed == USB_SPEED_HIGH) ?
			  sizeof(usbd_cdc_ncm_fs_config_desc) : sizeof(usbd_cdc_ncm_hs_config_desc);
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
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Desc %d OVSZ %d > %d\n", desc_type, len, buf_len);
			return 0;
		}

		usb_os_memcpy((void *)buf, (const void *)desc, len);
	}

	if (is_cfg != 0) {
		buf[USB_CFG_DESC_OFFSET_TYPE] = desc_type;
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);

		/* Patch EP addresses, interval and the class string indices to actual values */
		usbd_cdc_ncm_patch_desc(buf + USB_LEN_CFG_DESC, len - USB_LEN_CFG_DESC, desc_speed);
	}

	return len;
}

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief Store the first interface number assigned to this class by the composite framework
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param base: First interface number of this class
 */
static void usbd_cdc_ncm_set_interface_base(u8 base)
{
	usbd_cdc_ncm_dev.if_base = base;
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
		usb_os_mem_sync();

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
					 "ring", (int)((ncm->tx_wd - ncm->tx_rd + ncm->tx_ntb_depth) % ncm->tx_ntb_depth),
					 ncm->sequence);
			RTK_LOGS(TAG, RTK_LOG_INFO,
					 "cnt tx%d in%d out%d intr%d rxd%d/Heap %d ntb %d/%d\n",
					 ncm->dbg_tx_cnt, ncm->dbg_bulk_in_done_cnt, ncm->dbg_bulk_out_done_cnt,
					 ncm->dbg_intr_in_done_cnt, ncm->dbg_rx_deliver_cnt,
					 usb_os_get_free_heap_size(), ncm->ntb_in_max_size, ncm->ntb_out_max_size);
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
 * @param cfg: Class configuration
 * @retval Status
 */
static int usbd_cdc_ncm_private_init(const usbd_cdc_ncm_cb_t *cb, const usbd_cdc_ncm_config_t *cfg)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usbd_ep_t *ep_bulk_in = &ncm->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &ncm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ncm->ep_intr_in;
	usb_ep_info_t *info;
	u32 rx_raw_size;
	int ret = HAL_OK;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	if (cfg == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cfg\n");
		return HAL_ERR_PARA;
	}

	/* Every endpoint is mandatory for this class, and a wrong direction bit would make the
	   class claim an endpoint the controller drives the other way round. */
	if (((cfg->bulk_in_addr & USB_REQ_DIR_MASK) != USB_D2H) ||
		((cfg->bulk_out_addr & USB_REQ_DIR_MASK) != USB_H2D) ||
		((cfg->intr_in_addr & USB_REQ_DIR_MASK) != USB_D2H) ||
		((cfg->bulk_in_addr & (~USB_REQ_DIR_MASK)) == 0U) ||
		((cfg->bulk_out_addr & (~USB_REQ_DIR_MASK)) == 0U) ||
		((cfg->intr_in_addr & (~USB_REQ_DIR_MASK)) == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP addr %02x/%02x/%02x\n",
				 cfg->bulk_in_addr, cfg->bulk_out_addr, cfg->intr_in_addr);
		return HAL_ERR_PARA;
	}

	/* Ref USB 2.0 9.6.6: the endpoint numbers must exist on the controller, and the two IN
	   endpoints must differ, otherwise two endpoints would share one hardware endpoint */
	if (((cfg->bulk_in_addr & (u8)(~USB_REQ_DIR_MASK)) >= USB_MAX_ENDPOINTS) ||
		((cfg->bulk_out_addr & (u8)(~USB_REQ_DIR_MASK)) >= USB_MAX_ENDPOINTS) ||
		((cfg->intr_in_addr & (u8)(~USB_REQ_DIR_MASK)) >= USB_MAX_ENDPOINTS) ||
		(cfg->bulk_in_addr == cfg->intr_in_addr)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP addr %02x/%02x/%02x\n",
				 cfg->bulk_in_addr, cfg->bulk_out_addr, cfg->intr_in_addr);
		return HAL_ERR_PARA;
	}

	/* Published before anything that reads it: the descriptor patch path, the TX path and
	   the aggregation task all resolve their options through ncm->cfg. */
	ncm->cfg = cfg;

	/* Resolve the TX NTB geometry first - every TX allocation below is sized from it. */
	usbd_cdc_ncm_tx_layout_init(ncm, cfg);

	ncm->tx_ntb_depth = (cfg->tx_ntb_depth != 0U) ? cfg->tx_ntb_depth : USBD_CDC_NCM_DEFAULT_TX_NTB_DEPTH;
	if (ncm->tx_ntb_depth < USBD_CDC_NCM_MIN_TX_NTB_DEPTH) {
		/* The SPSC ring keeps one empty gap, so a depth below 2 leaves no usable slot and
		   every transmit would report the ring full. */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "TX NTB depth %d < %d\n", ncm->tx_ntb_depth, USBD_CDC_NCM_MIN_TX_NTB_DEPTH);
		return HAL_ERR_PARA;
	}

	/* dwNtbInMaxSize is the upper bound SET_NTB_INPUT_SIZE may negotiate, so it has to be at
	   least the minimum every NCM host may ask for (Ref NCM 1.0 Table 6-3).
	   It is deliberately NOT capped at the slot buffer size: a device may always emit NTBs
	   smaller than the maximum it advertises, and usbd_cdc_ncm_tx_ntb_limit() clamps every
	   NTB to what a slot can actually hold.  The default 4096 therefore exceeds the 1546 B
	   (one datagram) / 3068 B (two) a slot holds, exactly as before this was configurable. */
	ncm->ntb_in_max_size_cap = (cfg->ntb_in_max_size != 0U) ? cfg->ntb_in_max_size : USBD_CDC_NCM_DEFAULT_NTB_IN_SIZE;
	/* NTB-16 carries 16-bit block lengths and offsets, so 65535 is the largest NTB it can describe */
	if ((ncm->ntb_in_max_size_cap < USB_CDC_NCM_MIN_NTB_INPUT_SIZE) || (ncm->ntb_in_max_size_cap > USB_CDC_NCM_MAX_NTB_INPUT_SIZE)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "NTB IN size %u out of range\n", ncm->ntb_in_max_size_cap);
		return HAL_ERR_PARA;
	}

	/* dwNtbOutMaxSize sizes the RX buffers, so it has to hold at least one full-size
	   Ethernet frame wrapped in a minimal NTB or the host could not send one at all. */
	ncm->ntb_out_max_size = (cfg->ntb_out_max_size != 0U) ? cfg->ntb_out_max_size : USBD_CDC_NCM_DEFAULT_NTB_OUT_SIZE;
	if ((ncm->ntb_out_max_size < USBD_CDC_NCM_BULK_ONE_FRAME_SIZE) || (ncm->ntb_out_max_size > USB_CDC_NCM_MAX_NTB_INPUT_SIZE)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "NTB OUT size %u out of range\n", ncm->ntb_out_max_size);
		return HAL_ERR_PARA;
	}

	/* Round the RX buffer up to a whole number of HS bulk max packets - see
	   USBD_CDC_NCM_BULK_ONE_FRAME_SIZE. */
	rx_raw_size = (ncm->ntb_out_max_size > USBD_CDC_NCM_BULK_ONE_FRAME_SIZE) ?
				  ncm->ntb_out_max_size : (u32)USBD_CDC_NCM_BULK_ONE_FRAME_SIZE;
	ncm->rx_buf_size = (rx_raw_size + USB_BULK_HS_MAX_MPS - 1U) / USB_BULK_HS_MAX_MPS * USB_BULK_HS_MAX_MPS;

	/* No H2D class request is waiting for its data stage yet. The device context is a static
	   object, so a re-init after deinit must not inherit a stale pending flag. */
	ncm->ctrl_req_pending = 0U;
	/* The core owns the string table, so the same call works in standalone and composite
	   mode. mac_str is registered by pointer, so it may still be empty here: set_mac()
	   formats it below, before enumeration can request it. */
	ncm->mac_str_idx = usbd_add_string(ncm->mac_str);
	ncm->itf_str_idx = usbd_add_string(cfg->itf_str);
	/* Standalone default; the composite framework rebases it via set_interface_base() */
	ncm->if_base = 0;

	/* Allocate the TX NTB ring and its per-slot DMA-aligned buffers.  Each slot keeps its
	 * buffer for the lifetime of the class instance; clear_config() only resets state,
	 * never frees these. */
	ncm->tx_slot = (ncm_tx_ntb_t *)usb_os_malloc((u32)ncm->tx_ntb_depth * sizeof(ncm_tx_ntb_t));
	if (ncm->tx_slot == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc TX ring fail\n");
		return HAL_ERR_MEM;
	}
	{
		u8 i;
		for (i = 0U; i < ncm->tx_ntb_depth; i++) {
			ncm->tx_slot[i].buf = (u8 *)usb_os_malloc(CACHE_LINE_ALIGNMENT(ncm->ntb_tx_buf_size));
			if (ncm->tx_slot[i].buf == NULL) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc TX pp slot %d fail\n", i);
				/* Roll back any earlier slots before bailing out. */
				while (i-- > 0U) {
					usb_os_mfree((void *)ncm->tx_slot[i].buf);
					ncm->tx_slot[i].buf = NULL;
				}
				usb_os_mfree((void *)ncm->tx_slot);
				ncm->tx_slot = NULL;
				return HAL_ERR_MEM;
			}
			ncm->tx_slot[i].frame_count = 0;
			ncm->tx_slot[i].data_offset = 0;
			ncm->tx_slot[i].sequence = 0;
		}
		ncm->tx_wd = 0;
		ncm->tx_rd = 0;
		ncm->tx_inflight = 0;
	}

	/* Initialize NTB parameters.  Ref NCM 1.0 6.2.7: the negotiated IN size starts at the
	   advertised capability until a host shrinks it with SET_NTB_INPUT_SIZE. */
	ncm->ntb_in_max_size = ncm->ntb_in_max_size_cap;
	ncm->sequence = 0;
	ncm->ntb_format = 0; /* NTB16 */
	ncm->crc_mode = 0;   /* No CRC */

	/* Start from the data interface default setting: the BULK endpoints only come
	 * up when the host selects alt 1.  Explicit here because this instance is
	 * re-initialised in place on a hotplug cycle. */
	ncm->data_alt_setting = 0U;

	/* The uplink starts down: the owner reports it with usbd_cdc_ncm_set_link_status() */
	ncm->uplink_status = 0U;
	ncm->connect_status = 0U;

	/* BULK IN use the caller buffer */
	info = &ep_bulk_in->info;
	info->addr = cfg->bulk_in_addr;
	info->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_in->xfer_buf_len = 0;
	ep_bulk_in->xfer_buf = NULL;

	/* Allocate BULK OUT ping-pong buffers */
	info = &ep_bulk_out->info;
	info->addr = cfg->bulk_out_addr;
	info->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_out->xfer_buf_len = ncm->rx_buf_size;
	ep_bulk_out->xfer_len = ep_bulk_out->xfer_buf_len;

	ncm->rx_buf[0] = (u8 *)usb_os_malloc(ncm->rx_buf_size);
	if (ncm->rx_buf[0] == NULL) {
		ret = HAL_ERR_MEM;
		goto exit;
	}

	ncm->rx_buf[1] = (u8 *)usb_os_malloc(ncm->rx_buf_size);
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
	if (usb_ringbuf_manager_init(&ncm->tx_raw_rb,
								 (cfg->tx_raw_queue_depth != 0U) ? cfg->tx_raw_queue_depth : USBD_CDC_NCM_DEFAULT_TX_RAW_QUEUE_DEPTH,
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
	/* Separate from tx_raw_sema so that a producer waiting for room can never take the
	 * "frame queued" signal meant for the TX task, and vice versa */
	if (usb_os_sema_create(&ncm->tx_raw_free_sema) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create TX raw free sema fail\n");
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
						   (cfg->rx_task_stack_size != 0U) ? cfg->rx_task_stack_size : USBD_CDC_NCM_DEFAULT_RX_TASK_STACK_SIZE,
						   (cfg->rx_task_priority != 0U) ? cfg->rx_task_priority : USBD_CDC_NCM_DEFAULT_RX_TASK_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create RX thread fail\n");
		ncm->rx_thread_running = 0;
		ret = HAL_ERR_HW;
		goto cleanup_rx_data_ready_sema;
	}

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
	/* The TX task is the only consumer of tx_raw_rb: without it transmit() would
	 * silently queue frames that are never turned into NTBs. */
	ret = usbd_cdc_ncm_tx_task_init();
	if (ret != HAL_OK) {
		goto cleanup_rx_thread;
	}
#endif

	/* Allocate INTR IN buffer */
	info = &ep_intr_in->info;
	info->addr = cfg->intr_in_addr;
	info->type = USB_CH_EP_TYPE_INTR;
	ep_intr_in->xfer_buf_len = USBD_CDC_NCM_INTR_IN_PACKET_SIZE;
	ep_intr_in->xfer_buf = (u8 *)usb_os_malloc(ep_intr_in->xfer_buf_len);
	if (ep_intr_in->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto cleanup_rx_thread;
	}

	usbd_cdc_ncm_set_mac((cfg->mac != NULL) ? cfg->mac : usbd_cdc_ncm_default_mac);

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

#if USBD_CDC_NCM_STATE_TRACE_ENABLE
	usbd_cdc_ncm_trace_task_init();
#endif

	/* Every TX resource exists from here on: let usbd_cdc_ncm_transmit() in.  The device
	   context is static, so this also clears the flag a previous deinit left set. */
	ncm->xfer_users = 0U;
	ncm->is_closing = 0U;

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

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
	/* Join the TX aggregation task before the semaphores and slot buffers it
	 * produces into are released below.  Idempotent when the task was never
	 * created (the earlier gotos land here with ncm->tx_task == NULL). */
	usbd_cdc_ncm_tx_task_deinit();
#endif

cleanup_rx_data_ready_sema:
	usb_os_sema_delete(ncm->rx_data_ready_sema);
	ncm->rx_data_ready_sema = NULL;

cleanup_bulk_tx_slot_sema:
	usb_os_sema_delete(ncm->tx_buf_free_sema);
	ncm->tx_buf_free_sema = NULL;

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
cleanup_tx_raw_rb:
	if (ncm->tx_raw_free_sema != NULL) {
		usb_os_sema_delete(ncm->tx_raw_free_sema);
		ncm->tx_raw_free_sema = NULL;
	}
	if (ncm->tx_raw_sema != NULL) {
		usb_os_sema_delete(ncm->tx_raw_sema);
		ncm->tx_raw_sema = NULL;
	}
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
		for (i = 0U; i < ncm->tx_ntb_depth; i++) {
			usb_os_mfree((void *)ncm->tx_slot[i].buf);
			ncm->tx_slot[i].buf = NULL;
		}
		usb_os_mfree((void *)ncm->tx_slot);
		ncm->tx_slot = NULL;
	}

	return ret;
}

/* Exported functions --------------------------------------------------------*/

int usbd_cdc_ncm_init(const usbd_cdc_ncm_cb_t *cb, const usbd_cdc_ncm_config_t *cfg)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	int ret;

	ret = usbd_cdc_ncm_private_init(cb, cfg);
	if (ret == HAL_OK) {
		ncm->from_composite = 0;
		ret = usbd_register_class(&usbd_cdc_ncm_driver);
		if (ret != HAL_OK) {
			/* private_init completed, so deinit is its exact reverse */
			usbd_cdc_ncm_deinit();
		}
	}
	return ret;
}

#ifdef CONFIG_USBD_COMPOSITE
int usbd_composite_cdc_ncm_init(const usbd_cdc_ncm_cb_t *cb, const usbd_cdc_ncm_config_t *cfg)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	int ret;

	ret = usbd_cdc_ncm_private_init(cb, cfg);
	if (ret == HAL_OK) {
		ncm->from_composite = 1;
		ret = usbd_composite_register_driver(&usbd_cdc_ncm_driver);
		if (ret != HAL_OK) {
			/* private_init completed, so deinit is its exact reverse. from_composite is
			   already 1, so the unregister inside is a no-op for an unregistered driver. */
			usbd_cdc_ncm_deinit();
		}
	}
	return ret;
}
#endif

/**
 * @brief Deinitialize CDC NCM class
 * @retval None
 */
void usbd_cdc_ncm_deinit(void)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	usbd_ep_t *ep_bulk_in = &ncm->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &ncm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ncm->ep_intr_in;
	u32 wait_cnt = 0U;
	int ret;

	/* Leave the core before anything is torn down. Composite: refused until
	   usbd_composite_deinit() has blocked the ISR and cleared the configuration of every
	   function. Standalone: refused in interrupt context. Either way nothing is released when
	   refused, and once accepted no class callback runs any more and every endpoint of this
	   class is deinitialized: ref USB 2.0 9.4.7, the transfers still pending are aborted. */
#ifdef CONFIG_USBD_COMPOSITE
	if (ncm->from_composite != 0U) {
		ret = usbd_composite_unregister_driver(&usbd_cdc_ncm_driver);
	} else
#endif
	{
		ret = usbd_unregister_class();
	}
	if (ret != HAL_OK) {
		return;
	}

	/* No completion arrives any more to clear them */
	ep_bulk_in->xfer_state = 0U;
	ep_intr_in->xfer_state = 0U;

	/* Close the TX gate: from here on usbd_cdc_ncm_transmit() rejects new callers, and the
	 * ones already inside are waited for below before their resources go away. */
	usb_os_enter_critical();
	ncm->is_closing = 1U;
	usb_os_exit_critical();

	ncm->connect_status = 0;
	/* Fall back to the default alternate setting so every TX/RX submission path
	 * bails out before anything below is torn down. */
	ncm->data_alt_setting = 0U;
	usb_os_mem_sync();

#if USBD_CDC_NCM_STATE_TRACE_ENABLE
	/* Stop the trace thread first so it does not read state being torn down. */
	usbd_cdc_ncm_trace_task_deinit();
#endif

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
	/* Stop the TX aggregation task before anything it produces into is released.
	 * data_alt_setting is already 0, but the innermost aggregation loop does not
	 * re-check it, so the task may still be inside usbd_cdc_ncm_agg_append()
	 * writing to tx_slot[].buf, or blocked on tx_buf_free_sema.  Joining it here
	 * is what makes the slot-buffer frees and the semaphore deletes below
	 * provably free of a live producer. */
	usbd_cdc_ncm_tx_task_deinit();
#endif

	/* Wait for every task still inside usbd_cdc_ncm_transmit() to leave.  A caller blocked
	 * on TX room is woken on each round and leaves on is_closing; deleting the sema while
	 * it still waits on it, or freeing the slots it is writing to, would be a use after free,
	 * so the wait is not bounded and a starved caller is reported once per
	 * USBD_CDC_NCM_TX_EXIT_WARN_MS instead. */
	wait_cnt = 0U;
	while (ncm->xfer_users != 0U) {
		usbd_cdc_ncm_tx_wake_producer(ncm);
		usb_os_sleep_ms(1U);
		wait_cnt++;
		if (wait_cnt >= USBD_CDC_NCM_TX_EXIT_WARN_MS) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Wait TX exit\n");
			wait_cnt = 0U;
		}
	}

	/* No more USB ISR runs against this class.  Tear down TX state. */
	{
		u8 i;
		for (i = 0U; i < ncm->tx_ntb_depth; i++) {
			ncm->tx_slot[i].frame_count = 0;
			ncm->tx_slot[i].data_offset = 0;
			usb_os_mfree((void *)ncm->tx_slot[i].buf);
			ncm->tx_slot[i].buf = NULL;
		}
		usb_os_mfree((void *)ncm->tx_slot);
		ncm->tx_slot = NULL;
		ncm->tx_wd = 0;
		ncm->tx_rd = 0;
		ncm->tx_inflight = 0;
	}
	/* Unblock any transmit() still waiting for a free slot, then free the sema. */
	if (ncm->tx_buf_free_sema != NULL) {
		usb_os_sema_give(ncm->tx_buf_free_sema);
		usb_os_sema_delete(ncm->tx_buf_free_sema);
		ncm->tx_buf_free_sema = NULL;
	}

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
	/* The aggregation task was joined at the top of this function, and every
	 * transmit() caller waited for above; only the buffers and semaphores are
	 * left to release here. */
	usb_ringbuf_manager_deinit(&ncm->tx_raw_rb);
	if (ncm->tx_raw_sema != NULL) {
		usb_os_sema_delete(ncm->tx_raw_sema);
		ncm->tx_raw_sema = NULL;
	}
	if (ncm->tx_raw_free_sema != NULL) {
		usb_os_sema_delete(ncm->tx_raw_free_sema);
		ncm->tx_raw_free_sema = NULL;
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
	/* Unregistered above: no class callback can run afterwards, so dropping the pending
	 * control request here cannot race an EP0 OUT completion in ISR context. */
	ncm->ctrl_req_pending = 0U;

	/* Call user deinit */
	if ((ncm->cb != NULL) && (ncm->cb->deinit != NULL) && (ncm->cb->deinit() != HAL_OK)) {
		/* Teardown goes on regardless, the class resources are released below */
		RTK_LOGS(TAG, RTK_LOG_WARN, "CB deinit fail\n");
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
}

/**
 * @brief Start to transmit an ethernet frame via NCM NTB16
 * @note  Task context only: the TX path has a single producer and a blocking call waits on
 *        a semaphore, so a call in interrupt context is rejected.
 * @param buf: Ethernet frame data buffer
 * @param len: Ethernet frame length
 * @param block: Whether block until xfer done
 * @retval Status
 */
int usbd_cdc_ncm_transmit(const u8 *buf, u32 len, u8 block)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;
	int ret;

#if USBD_CDC_NCM_STATE_TRACE_ENABLE
	ncm->dbg_tx_cnt++;
#endif

	if (usb_os_is_in_interrupt() != 0U) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, 0U);
		return HAL_ERR_PARA;
	}


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
	usb_os_enter_critical();
	if ((ncm->is_closing != 0U) || (ncm->data_alt_setting == 0U)) {
		usb_os_exit_critical();
		return HAL_BUSY;
	}
	/* Counted under the same lock that deinit closes the gate with, so deinit either sees
	 * this caller or this caller sees is_closing: never both missed */
	ncm->xfer_users++;
	usb_os_exit_critical();

	ret = usbd_cdc_ncm_tx_enqueue(buf, len, block);

	usb_os_enter_critical();
	ncm->xfer_users--;
	usb_os_exit_critical();

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
  * @note   Only records the uplink state.  The SOF handler (ISR context) compares the
  *         resulting host-visible state with the one last reported and sends
  *         NETWORK_CONNECTION on a change, so repeated calls with the same value are
  *         no-ops and every notification field keeps the USB ISR as its only writer.
  *         Callable from any task context.
  * @param  link_up: Non-zero when the network link is up, zero when it is down.
  * @retval HAL_OK
  */
int usbd_cdc_ncm_set_link_status(u8 link_up)
{
	usbd_cdc_ncm_dev_t *ncm = &usbd_cdc_ncm_dev;

	ncm->uplink_status = (link_up != 0U) ? 1U : 0U;

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
