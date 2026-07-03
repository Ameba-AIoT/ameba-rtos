/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_cdc_ecm.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/**
 * @brief ECM notification state machine
 */
enum usbd_cdc_ecm_notify_state {
	ECM_NOTIFY_NONE,      /**< No notification pending */
	ECM_NOTIFY_CONNECT,   /**< Network connection notification */
	ECM_NOTIFY_SPEED,     /**< Speed change notification */
};

enum usbd_cdc_ecm_dongle_mac_type_t {
	CDC_ECM_MAC_UNINIT = 0U,
	CDC_ECM_MAC_UPPER_LAYER_SET,
	CDC_ECM_MAC_HOST_SET,
	CDC_ECM_MAC_TYPE_MAX,
};

/* Private macros ------------------------------------------------------------*/
#define USBD_ECM_RX_SPEED_CHECK                       0                     /* CDC ECM rx speed test */
#define USBD_ECM_TX_SPEED_CHECK                       0                     /* CDC ECM tx speed test */

/* TX ring buffer capacity: number of Ethernet frames that can be queued while a DMA
 * transfer is in progress.  USBD_CDC_ECM_BULK_TX_RB_SIZE slots tolerate bursts without blocking the caller. */
#define USBD_CDC_ECM_BULK_TX_RB_SIZE                  5U

/* Maximum time (ms) usbd_cdc_ecm_transmit() will block waiting for a ring buffer
 * slot when called with block != 0.
 *
 * Must be long enough to cover the ring-buffer drain time under peak throughput:
 *   USB HS (480 Mbps bus, ~40 MB/s BULK effective): BULK_TX_RB_SIZE - 1514 B / 40 MB/s -> 0.4 ms
 *   USB FS ( 12 Mbps bus, ~1.5 MB/s BULK effective): BULK_TX_RB_SIZE - 1514 B / 1.5 MB/s -> 10 ms
 * 10 ms gives 2¨C6 * headroom for normal bursts without risking a noticeable
 * tcpip-task freeze when the host stops polling BULK IN (stuck-TX scenario). */
#define USBD_CDC_ECM_BULK_TX_TIMEOUT_MS               10U

/* Device identification */
#define USBD_CDC_ECM_VID                              USB_VID               /**< Vendor ID */
#define USBD_CDC_ECM_PID                              USB_PID               /**< Product ID */
#define USBD_CDC_ECM_SELF_POWERED                     1U                    /**< Self-powered device */
#define USBD_CDC_ECM_REMOTE_WAKEUP_EN                 1U                    /**< Remote wakeup enabled */

/* String descriptors */
#define USBD_CDC_ECM_LANGID_STRING                    0x0409U               /**< Language ID (US English) */
#define USBD_CDC_ECM_MFG_STRING                       "Realtek"             /**< Manufacturer */
#define USBD_CDC_ECM_PROD_HS_STRING                   "Realtek CDC ECM (HS)"/**< Product (HS) */
#define USBD_CDC_ECM_PROD_FS_STRING                   "Realtek CDC ECM (FS)"/**< Product (FS) */
#define USBD_CDC_ECM_SN_STRING                        "1234567890"          /**< Serial number */
#define USBD_CDC_ECM_MAC_STRING_INDEX                 (USBD_IDX_SERIAL_STR + 1) /**< MAC string index */

/* Interrupt Endpoint Intervals */
#define USBD_CDC_ECM_HS_INTR_IN_INTERVAL              8U                    /**< HS INTR IN interval */
#define USBD_CDC_ECM_FS_INTR_IN_INTERVAL              8U                    /**< FS INTR IN interval */

/* Buffer sizes */
#define USBD_CDC_ECM_BULK_BUF_MAX_SIZE                ((USB_CDC_ECM_MAX_SEGMENT_SIZE + USB_BULK_HS_MAX_MPS - 1) / USB_BULK_HS_MAX_MPS * USB_BULK_HS_MAX_MPS)

/* RX decoupling: ping-pong buffers + dedicated thread to deliver received frames
 * out of USB context. State lives in usbd_cdc_ecm_dev_t; see usbd_cdc_ecm.h.
 *
 * Synchronization:
 *   - rx_data_ready_sema (ISR -> thread): given by ISR when a buffer holds a
 *     frame; taken (blocking) by the RX thread to start processing.
 *   - rx_buf_free (thread -> ISR): volatile flag. Set to 1 by the RX thread
 *     after processing; cleared to 0 by the ISR when it claims the buffer.
 *
 * Flow control via SOF interrupt (no drop):
 *   If rx_buf_free == 0 when ep_data_out fires, usbd_ep_receive is NOT trigger
 *   (bulk-out endpoint NAKs the host). rx_pending is set with the frame length.
 *   The SOF handler (fires every 1 ms FS / 125 us HS) polls the flag and
 *   trigger the endpoint as soon as the thread frees the buffer.
 *
 * rx_xfer_idx tracks which of the two buffers is currently armed for USB OUT. */
#define USBD_CDC_ECM_RX_THREAD_STACK_SIZE             1024U
#define USBD_CDC_ECM_RX_THREAD_PRIORITY               6U
#define USBD_CDC_ECM_RX_SEMA_TAKE_TIMEOUT_MS          100U

#if USBD_CDC_ECM_STATE_TRACE_ENABLE
#define USBD_CDC_ECM_TRACE_THREAD_STACK_SIZE         1024U
#define USBD_CDC_ECM_TRACE_THREAD_PRIORITY           1U
#define USBD_CDC_ECM_TRACE_INTERVAL_MS               1000U
#endif

/* Private function prototypes -----------------------------------------------*/

static int cdc_ecm_set_config(usb_dev_t *dev, u8 config);
static int cdc_ecm_clear_config(usb_dev_t *dev, u8 config);
static int cdc_ecm_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 cdc_ecm_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int cdc_ecm_handle_ep0_data_out(usb_dev_t *dev);
static int cdc_ecm_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int cdc_ecm_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static int cdc_ecm_sof(usb_dev_t *dev);
static void cdc_ecm_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
static void cdc_ecm_bulk_tx_start_from_rb(void);
static int cdc_ecm_intr_in_send(void *data, u16 len);
static int cdc_ecm_send_notification(void);
static inline u8 cdc_ecm_char_to_hex(u8 value);
static void cdc_ecm_mac_to_string(u8 *mac, char *mac_str);
static void cdc_ecm_set_mac(u8 *mac);
static int cdc_ecm_bulk_send(u8 *buf, u32 len);
#if USBD_CDC_ECM_STATE_TRACE_ENABLE
static void cdc_ecm_trace_task_init(void);
static void cdc_ecm_trace_task_deinit(void);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "ECM";
static const u8 ecm_mac[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};  /**< MAC address */

/* USB Standard Device Descriptor */
static const u8 usbd_cdc_ecm_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,                               /* bLength */
	USB_DESC_TYPE_DEVICE,                           /* bDescriptorType */
	0x00, 0x02,                                     /* bcdUSB: 2.0 */
	USB_CDC_ECM_COMM_INTERFACE_CLASS_CODE,              /* bDeviceClass: CDC */
	USB_CDC_ECM_SUBCLASS_ECM,                           /* bDeviceSubClass: ECM */
	0x00,                                           /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize0 */
	USB_LOW_BYTE(USBD_CDC_ECM_VID),                 /* idVendor */
	USB_HIGH_BYTE(USBD_CDC_ECM_VID),
	USB_LOW_BYTE(USBD_CDC_ECM_PID),                 /* idProduct */
	USB_HIGH_BYTE(USBD_CDC_ECM_PID),
	0x00, 0x02,                                     /* bcdDevice: 2.0 */
	USBD_IDX_MFC_STR,                               /* iManufacturer */
	USBD_IDX_PRODUCT_STR,                           /* iProduct */
	USBD_IDX_SERIAL_STR,                            /* iSerialNumber */
	0x01                                            /* bNumConfigurations */
};

/* USB Standard String Descriptor 0 (Language ID) */
static const u8 usbd_cdc_ecm_lang_id_desc[USB_LEN_LANGID_STR_DESC] = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(USBD_CDC_ECM_LANGID_STRING),       /* wLANGID */
	USB_HIGH_BYTE(USBD_CDC_ECM_LANGID_STRING),
};

#ifndef CONFIG_USB_FS
/* USB Device Qualifier Descriptor */
static const u8 usbd_cdc_ecm_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
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

/* USB CDC ECM High Speed Configuration Descriptor */
static const u8 usbd_cdc_ecm_hs_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	0x00, 0x00,                                     /* wTotalLength (calculated later) */
	0x02,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
#if USBD_CDC_ECM_SELF_POWERED
	0xE0,                                           /* bmAttributes: Self-powered, Remote wakeup */
#else
	0xA0,                                           /* bmAttributes: Bus-powered, Remote wakeup */
#endif
	0x32,                                           /* bMaxPower: 100mA */

	/* Communication Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_ECM_COMM_INTERFACE_NUM,                /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x01,                                           /* bNumEndpoints */
	USB_CDC_ECM_COMM_INTERFACE_CLASS_CODE,              /* bInterfaceClass: CDC */
	USB_CDC_ECM_SUBCLASS_ECM,                           /* bInterfaceSubClass: ECM */
	0x00,                                           /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* CDC Header Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	USB_CDC_ECM_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_ECM_FUNC_DESC_HEADER,                       /* bDescriptorSubtype */
	0x10, 0x01,                                     /* bcdCDC: 1.10 */

	/* CDC Union Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	USB_CDC_ECM_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_ECM_FUNC_DESC_UNION,                        /* bDescriptorSubtype */
	USBD_CDC_ECM_COMM_INTERFACE_NUM,                /* bControlInterface */
	USBD_CDC_ECM_DATA_INTERFACE_NUM,                /* bSubordinateInterface0 */

	/* CDC Ethernet Networking Functional Descriptor */
	USB_CDC_ECM_ETHERNET_FUNC_DESC_SIZE,            /* bFunctionLength */
	USB_CDC_ECM_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_ECM_FUNC_DESC_ETHERNET_NETWORKING,          /* bDescriptorSubtype */
	USBD_CDC_ECM_MAC_STRING_INDEX,                  /* iMACAddress */
	0x00, 0x00, 0x00, 0x00,                         /* bmEthernetStatistics */
	USB_LOW_BYTE(USB_CDC_ECM_MAX_SEGMENT_SIZE),     /* wMaxSegmentSize */
	USB_HIGH_BYTE(USB_CDC_ECM_MAX_SEGMENT_SIZE),
	0x00, 0x00,                                     /* wNumberMCFilters */
	0x00,                                           /* bNumberPowerFilters */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_CDC_ECM_INTR_IN_EP,                        /* bEndpointAddress */
	USB_CH_EP_TYPE_INTR,                            /* bmAttributes */
	USB_LOW_BYTE(USB_CDC_ECM_INTR_IN_PACKET_SIZE),  /* wMaxPacketSize */
	USB_HIGH_BYTE(USB_CDC_ECM_INTR_IN_PACKET_SIZE),
	USBD_CDC_ECM_HS_INTR_IN_INTERVAL,               /* bInterval */

	/* Data Interface Descriptor (Alt 0 - No endpoints) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_ECM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x00,                                           /* bNumEndpoints */
	USB_CDC_ECM_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	0x00,                                           /* bInterfaceSubClass */
	0x00,                                           /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* Data Interface Descriptor (Alt 1 - Active) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_ECM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x01,                                           /* bAlternateSetting */
	0x02,                                           /* bNumEndpoints */
	USB_CDC_ECM_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	0x00,                                           /* bInterfaceSubClass */
	0x00,                                           /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_CDC_ECM_BULK_IN_EP,                        /* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes */
	USB_LOW_BYTE(USB_BULK_HS_MAX_MPS),              /* wMaxPacketSize */
	USB_HIGH_BYTE(USB_BULK_HS_MAX_MPS),
	0x00,                                           /* bInterval */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_CDC_ECM_BULK_OUT_EP,                       /* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes */
	USB_LOW_BYTE(USB_BULK_HS_MAX_MPS),              /* wMaxPacketSize */
	USB_HIGH_BYTE(USB_BULK_HS_MAX_MPS),
	0x00                                            /* bInterval */
};
#endif

/* USB CDC ECM Full Speed Configuration Descriptor */
static const u8 usbd_cdc_ecm_fs_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	0x00, 0x00,                                     /* wTotalLength (calculated later) */
	0x02,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
#if USBD_CDC_ECM_SELF_POWERED
	0xE0,                                           /* bmAttributes: Self-powered, Remote wakeup */
#else
	0xA0,                                           /* bmAttributes: Bus-powered, Remote wakeup */
#endif
	0x32,                                           /* bMaxPower: 100mA */

	/* Communication Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_ECM_COMM_INTERFACE_NUM,                /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x01,                                           /* bNumEndpoints */
	USB_CDC_ECM_COMM_INTERFACE_CLASS_CODE,              /* bInterfaceClass: CDC */
	USB_CDC_ECM_SUBCLASS_ECM,                           /* bInterfaceSubClass: ECM */
	0x00,                                           /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* CDC Header Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	USB_CDC_ECM_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_ECM_FUNC_DESC_HEADER,                       /* bDescriptorSubtype */
	0x10, 0x01,                                     /* bcdCDC: 1.10 */

	/* CDC Union Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	USB_CDC_ECM_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_ECM_FUNC_DESC_UNION,                        /* bDescriptorSubtype */
	USBD_CDC_ECM_COMM_INTERFACE_NUM,                /* bControlInterface */
	USBD_CDC_ECM_DATA_INTERFACE_NUM,                /* bSubordinateInterface0 */

	/* CDC Ethernet Networking Functional Descriptor */
	USB_CDC_ECM_ETHERNET_FUNC_DESC_SIZE,            /* bFunctionLength */
	USB_CDC_ECM_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_ECM_FUNC_DESC_ETHERNET_NETWORKING,          /* bDescriptorSubtype */
	USBD_CDC_ECM_MAC_STRING_INDEX,                  /* iMACAddress */
	0x00, 0x00, 0x00, 0x00,                         /* bmEthernetStatistics */
	USB_LOW_BYTE(USB_CDC_ECM_MAX_SEGMENT_SIZE),     /* wMaxSegmentSize */
	USB_HIGH_BYTE(USB_CDC_ECM_MAX_SEGMENT_SIZE),
	0x00, 0x00,                                     /* wNumberMCFilters */
	0x00,                                           /* bNumberPowerFilters */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_CDC_ECM_INTR_IN_EP,                        /* bEndpointAddress */
	USB_CH_EP_TYPE_INTR,                            /* bmAttributes */
	USB_LOW_BYTE(USB_CDC_ECM_INTR_IN_PACKET_SIZE),  /* wMaxPacketSize */
	USB_HIGH_BYTE(USB_CDC_ECM_INTR_IN_PACKET_SIZE),
	USBD_CDC_ECM_FS_INTR_IN_INTERVAL,               /* bInterval */

	/* Data Interface Descriptor (Alt 0 - No endpoints) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_ECM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x00,                                           /* bNumEndpoints */
	USB_CDC_ECM_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	0x00,                                           /* bInterfaceSubClass */
	0x00,                                           /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* Data Interface Descriptor (Alt 1 - Active) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_ECM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x01,                                           /* bAlternateSetting */
	0x02,                                           /* bNumEndpoints */
	USB_CDC_ECM_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	0x00,                                           /* bInterfaceSubClass */
	0x00,                                           /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_CDC_ECM_BULK_IN_EP,                        /* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes */
	USB_LOW_BYTE(USB_BULK_FS_MAX_MPS),              /* wMaxPacketSize */
	USB_HIGH_BYTE(USB_BULK_FS_MAX_MPS),
	0x00,                                           /* bInterval */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_CDC_ECM_BULK_OUT_EP,                       /* bEndpointAddress */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes */
	USB_LOW_BYTE(USB_BULK_FS_MAX_MPS),              /* wMaxPacketSize */
	USB_HIGH_BYTE(USB_BULK_FS_MAX_MPS),
	0x00                                            /* bInterval */
};

/* CDC ECM Class Driver */
static const usbd_class_driver_t usbd_cdc_driver = {
	.get_descriptor = cdc_ecm_get_descriptor,
	.set_config = cdc_ecm_set_config,
	.clear_config = cdc_ecm_clear_config,
	.setup = cdc_ecm_setup,
	.ep0_data_out = cdc_ecm_handle_ep0_data_out,
	.ep_data_in = cdc_ecm_handle_ep_data_in,
	.ep_data_out = cdc_ecm_handle_ep_data_out,
	.sof = cdc_ecm_sof,
	.status_changed = cdc_ecm_status_changed,
};

/* CDC ECM Device Instance */
static usbd_cdc_ecm_dev_t usbd_cdc_ecm_dev;

/* Private functions ---------------------------------------------------------*/
/**
 * @brief Convert a nibble (0-15) to its ASCII hex character.
 * @param value: Nibble value (0x0-0xF).
 * @retval ASCII character '0'-'9' or 'A'-'F'. Returns '0' for out-of-range input.
 */
static inline u8 cdc_ecm_char_to_hex(u8 value)
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
static void cdc_ecm_mac_to_string(u8 *mac, char *mac_str)
{
	u8 str_index = 0;
	u8 i;

	for (i = 0; i < 6; i++) {
		/* high 4 bits */
		mac_str[str_index++] = cdc_ecm_char_to_hex((mac[i] >> 4) & 0x0F);

		/* low 4 bits */
		mac_str[str_index++] = cdc_ecm_char_to_hex(mac[i] & 0x0F);
	}

	mac_str[str_index] = '\0';
}

/**
 * @brief Set the device MAC address from an external source.
 * @param mac: Pointer to a 6-byte MAC address buffer. NULL is ignored.
 */
static void cdc_ecm_set_mac(u8 *mac)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;

	if (NULL == mac) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "No Param\n");
		return;
	}

	memcpy((void *) & (ecm->mac[0]), (void *)mac, USBD_CDC_ECM_MAC_STR_LEN);
	ecm->mac_src_type = CDC_ECM_MAC_UPPER_LAYER_SET;

	ecm->mac_valid = 1;
}

/**
 * @brief Start DMA for the head frame in the TX ring buffer.
 * @note  Called from both thread context (transmit path) and ISR context (XFRC chain).
 *        The head frame is copied out of the ring buffer into ecm->bulk_tx_dma_buf
 *        via the public usb_ringbuf_remove_head() API, which also advances the head
 *        and frees the slot.  The DMA then runs from bulk_tx_dma_buf, so it never
 *        references ring buffer node memory and we touch no ring buffer internals.
 *        Because the slot is released here (not in cdc_ecm_handle_ep_data_in), the
 *        free-slot semaphore is given here too.
 */
static void cdc_ecm_bulk_tx_start_from_rb(void)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usb_ringbuf_manager_t *rb = &ecm->bulk_tx_rb;
	u32 frame_len;

	if (usb_ringbuf_is_empty(rb)) {
		return;
	}

	/* Copy the head frame into our private DMA buffer and dequeue it.  This
	 * advances the ring buffer head, so the slot is immediately reusable by the
	 * producer; the DMA below reads only bulk_tx_dma_buf, never node memory. */
	frame_len = usb_ringbuf_remove_head(rb, ecm->bulk_tx_dma_buf,
										USBD_CDC_ECM_BULK_BUF_MAX_SIZE, NULL);
	if (frame_len == 0U) {
		return;
	}

	/* A slot just freed up - wake any producer blocked in usbd_cdc_ecm_transmit(). */
	usb_os_sema_give(ecm->bulk_tx_slot_sema);

	cdc_ecm_bulk_send(ecm->bulk_tx_dma_buf, frame_len);
}

/**
 * @brief Deliver a received frame to the upper-layer callback.
 * @param buf:    Pointer to the received data buffer.
 * @param length: Length of the received frame in bytes.
 */
static int cdc_ecm_bulk_receive(u8 *buf, u32 length)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
#if USBD_ECM_RX_SPEED_CHECK
	static u64 usb_rx_start_time = 0, usb_rx_end_time, usb_rx_interval_time; //ms
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

	if ((ecm->cb != NULL) && (ecm->cb->received != NULL)) {
		ecm->cb->received(buf, length);
	}

	return HAL_OK;
}

/**
 * @brief  Send the next pending INTR IN notification and advance the state machine.
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @retval HAL_OK on success, HAL_ERR_PARA if no notification is pending,
 *         or a HAL error code if the transfer could not be submitted.
 */
static int cdc_ecm_send_notification(void)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usb_cdc_ecm_notify_t event;
	int status;
	u16 length;
	u8 next_state;

	event.bmRequestType = 0xA1;
	event.wIndex = USBD_CDC_ECM_DATA_INTERFACE_NUM;

	switch (ecm->notify_state) {
	case ECM_NOTIFY_CONNECT:
		event.bNotificationCode = USB_CDC_ECM_NOTIFY_NETWORK_CONNECTION;
		event.wValue = ecm->connect_status;
		event.wLength = 0;
		length = USB_CDC_ECM_NETWORK_CONNECTION_SIZE;
		/* Follow a "connected" notification with a speed-change report; a
		 * "disconnected" notification stands alone (no trailing SPEED). */
		next_state = ecm->connect_status ? ECM_NOTIFY_SPEED : ECM_NOTIFY_NONE;
		break;

	case ECM_NOTIFY_SPEED:
		event.bNotificationCode = USB_CDC_ECM_NOTIFY_CONNECTION_SPEED_CHANGE;
		event.wValue = 0;
		event.wLength = 8;
		event.data.DLBitRate = 0; /* Downstream bits/sec */
		event.data.ULBitRate = 0; /* Upstream bits/sec */
		length = USB_CDC_ECM_CONNECTION_SPEED_CHANGE_SIZE;
		next_state = ECM_NOTIFY_NONE;
		break;

	case ECM_NOTIFY_NONE:
	default:
		return HAL_OK;
	}

	status = cdc_ecm_intr_in_send(&event, length);
	if (status == HAL_OK) {
		/* Advance state only on success; retry will re-send the same notification. */
		ecm->notify_state = next_state;
	}

	return status;
}

/**
 * @brief Transmit INTR IN notification
 * @param data: Notification data
 * @param len: Data length
 * @retval Status
 */
static int cdc_ecm_intr_in_send(void *data, u16 len)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usb_dev_t *dev = ecm->dev;
	usbd_ep_t *ep_intr_in = &ecm->ep_intr_in;
	int ret = HAL_ERR_HW;

	if (!dev->is_ready) {
		return ret;
	}

	if (len > USB_CDC_ECM_INTR_IN_PACKET_SIZE) {
		len = USB_CDC_ECM_INTR_IN_PACKET_SIZE;
	}

	if (ep_intr_in->xfer_state == 0U) {
		ep_intr_in->is_busy = 1U;
		ep_intr_in->xfer_state = 1U;

		if (dev->is_ready) {
			usb_os_memcpy((void *)ep_intr_in->xfer_buf, (void *)data, len);
			ep_intr_in->xfer_len = len;
			usbd_ep_transmit(dev, ep_intr_in);
			ret = HAL_OK;
		} else {
			ep_intr_in->xfer_state = 0U;
		}

		ep_intr_in->is_busy = 0U;
	} else {
		/*TX busy*/
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
static int cdc_ecm_bulk_send(u8 *buf, u32 len)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep_bulk_in = &ecm->ep_bulk_in;
	usb_dev_t *dev = ecm->dev;
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
				/* The transfer never started (e.g. buffer alignment / HW error),
				 * so no XFRC interrupt will fire to clear xfer_state.  Reset it
				 * here, otherwise the BULK IN path wedges permanently: the ISR
				 * chain stops, the TX ring buffer never drains and every later
				 * frame is dropped until the device is re-enumerated. */
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
 * @brief Set CDC ECM configuration
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @param config: Configuration number
 * @retval Status
 */
static int cdc_ecm_set_config(usb_dev_t *dev, u8 config)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep_bulk_in = &ecm->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &ecm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ecm->ep_intr_in;
	usb_ep_info_t *info;
	UNUSED(config);

	ecm->dev = dev;

	/* Initialize INTR IN endpoint */
	ep_intr_in->xfer_state = 0U;
	info = &ep_intr_in->info;
	info->mps = USB_CDC_ECM_INTR_IN_PACKET_SIZE;
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

	/* Start receiving */
	usbd_ep_receive(dev, ep_bulk_out);

	return HAL_OK;
}

/**
 * @brief Clear CDC ECM configuration
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @param config: Configuration number
 * @retval Status
 */
static int cdc_ecm_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;

	UNUSED(config);

	/* Deinitialize all endpoints */
	usbd_ep_deinit(dev, &ecm->ep_bulk_in);
	usbd_ep_deinit(dev, &ecm->ep_bulk_out);
	usbd_ep_deinit(dev, &ecm->ep_intr_in);

	/* Discard any queued frames and reset the endpoint state so the next
	 * set_config starts with a clean TX path. */
	usb_ringbuf_reset(&ecm->bulk_tx_rb);
	ecm->ep_bulk_in.xfer_state = 0U;
	/* Unblock any transmit() call that is waiting on a ring buffer slot. */
	usb_os_sema_give(ecm->bulk_tx_slot_sema);

	/* The data path is gone: clear the link state and abandon any in-flight
	 * notification sequence so the next SET_INTERFACE re-reports from scratch
	 * (mirrors the detach reset in cdc_ecm_status_changed). */
	ecm->connect_status = 0;
	ecm->notify_state = ECM_NOTIFY_NONE;
	ecm->notify_retry = 0U;

	return HAL_OK;
}

/**
 * @brief Handle CDC ECM SETUP requests
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @param req: SETUP request
 * @retval Status
 */
static int cdc_ecm_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	int ret = HAL_OK;
	u8 req_type = req->bmRequestType & USB_REQ_TYPE_MASK;
	u8 interface_id;

	switch (req_type) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				interface_id = USB_LOW_BYTE(req->wIndex);
				ecm->alt_setting = USB_LOW_BYTE(req->wValue);

				if (interface_id == USBD_CDC_ECM_DATA_INTERFACE_NUM) {
					/* Report the link state once when the host activates the data
					 * interface.  If the INTR IN endpoint is momentarily busy the
					 * send fails here; notify_retry lets the SOF handler re-send so
					 * the initial notification is never silently lost. */
					ecm->notify_state = ECM_NOTIFY_CONNECT;
					ecm->connect_status = 1;
					if (cdc_ecm_send_notification() != HAL_OK) {
						ecm->notify_retry = 1U;
					}
				}

				if (ecm->cb && ecm->cb->setup) {
					ecm->cb->setup(req, NULL);
				}
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = ecm->alt_setting;
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
		if (req->wLength > 0) {
			if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
				/* Device-to-Host with data stage */
				if (ecm->cb && ecm->cb->setup) {
					ret = ecm->cb->setup(req, ep0_in->xfer_buf);
					if (ret == HAL_OK) {
						ep0_in->xfer_len = req->wLength;
						usbd_ep_transmit(dev, ep0_in);
					}
				}
			} else {
				/* Host-to-Device with data stage */
				usb_os_memcpy((void *)&ecm->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				ep0_out->xfer_len = req->wLength;
				usbd_ep_receive(dev, ep0_out);
			}
		} else {
			/* No data stage */
			if (ecm->cb && ecm->cb->setup) {
				ret = ecm->cb->setup(req, NULL);
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
static int cdc_ecm_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep_bulk_in = &ecm->ep_bulk_in;
	usbd_ep_t *ep_intr_in = &ecm->ep_intr_in;

	UNUSED(dev);
	UNUSED(status);

	if (ep_addr == USBD_CDC_ECM_BULK_IN_EP) {
		ep_bulk_in->xfer_state = 0U;
		/* The completed frame was already dequeued (and its slot freed) in
		 * cdc_ecm_bulk_tx_start_from_rb() before the DMA started, so there is
		 * nothing to retire here - just chain the next queued frame, if any.
		 * No ring buffer internals are touched. */
		if (!usb_ringbuf_is_empty(&ecm->bulk_tx_rb)) {
			cdc_ecm_bulk_tx_start_from_rb();
		}
#if USBD_CDC_ECM_STATE_TRACE_ENABLE
		ecm->dbg_bulk_in_done_cnt++;
#endif
	} else if (ep_addr == USBD_CDC_ECM_INTR_IN_EP) {
		ep_intr_in->xfer_state = 0U;
		/* Defer the next notification transmission to the SOF handler so all
		 * retry paths converge in one place (cdc_ecm_sof).  SOF fires every
		 * 1 ms (FS) or 125 us (HS), so the delay is negligible. */
		ecm->notify_retry = 1U;
#if USBD_CDC_ECM_STATE_TRACE_ENABLE
		ecm->dbg_intr_in_done_cnt++;
#endif
	}

	return HAL_OK;
}

/**
 * @brief RX delivery thread
 * @details Waits on rx_data_ready_sema, forwards each frame to the user callback,
 *          then sets rx_buf_free so the USB OUT path can hand off again.
 *          Decouples upper-layer (lwIP) processing from USB completion context.
 */
static void usbd_cdc_ecm_rx_thread(void *param)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	UNUSED(param);

	while (ecm->rx_thread_running) {
#if USBD_CDC_ECM_STATE_TRACE_ENABLE
		/* Heartbeat counter: incremented every loop iteration.  A frozen value
		 * in the trace output means this thread is wedged (e.g. blocked inside
		 * the upper-layer received() callback). */
		ecm->dbg_rx_loop_cnt++;
#endif
		if (usb_os_sema_take(ecm->rx_data_ready_sema, USBD_CDC_ECM_RX_SEMA_TAKE_TIMEOUT_MS) != HAL_OK) {
			/* timeout - re-check running flag */
			continue;
		}

		if (!ecm->rx_thread_running) {
			break;
		}

		if ((ecm->rx_msg_buf != NULL) && (ecm->rx_msg_len > 0U)) {
			cdc_ecm_bulk_receive(ecm->rx_msg_buf, ecm->rx_msg_len);
#if USBD_CDC_ECM_STATE_TRACE_ENABLE
			ecm->dbg_rx_deliver_cnt++;
			ecm->dbg_rx_bytes += ecm->rx_msg_len;
#endif
		}

		/* Previous buffer consumed; let the OUT EP path hand off another. */
		ecm->rx_buf_free = 1U;
	}

	ecm->rx_task = NULL;
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
static int cdc_ecm_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep_bulk_out = &ecm->ep_bulk_out;

	UNUSED(dev);

	if (ep_addr != USBD_CDC_ECM_BULK_OUT_EP) {
		return HAL_ERR_PARA;
	}

	/* Invalidate cache if needed */
	if (ep_bulk_out->skip_dcache_post_invalidate && len > 0) {
		DCache_Invalidate((u32)ep_bulk_out->xfer_buf, len);
	}

	if (len > 0U) {
#if USBD_CDC_ECM_STATE_TRACE_ENABLE
		ecm->dbg_bulk_out_done_cnt++;
#endif
		/* Try to hand off the current buffer to the RX thread.
		 * Check the volatile flag - safe in ISR context, no semaphore needed. */
		if (ecm->rx_buf_free != 0U) {
			ecm->rx_buf_free = 0U;
			ecm->rx_msg_buf = ecm->rx_buf[ecm->rx_xfer_idx];
			ecm->rx_msg_len = len;
			usb_os_sema_give(ecm->rx_data_ready_sema);

			/* Flip to the other buffer for the next OUT transfer. */
			ecm->rx_xfer_idx ^= 1U;
			ep_bulk_out->xfer_buf = ecm->rx_buf[ecm->rx_xfer_idx];
		} else {
			/* RX thread still busy. Do NOT re-arm the endpoint; the host will
			 * see NAK on the bulk-out pipe. Mark as pending so the SOF handler
			 * re trigger the endpoint as soon as the thread frees the buffer. */
			ecm->rx_pending_len = len;
			return HAL_OK;
		}
	}

	/* Continue receiving */
	return usbd_ep_receive(ecm->dev, ep_bulk_out);
}

/**
 * @brief   SOF interrupt handler
 * @note    This function is called within an interrupt service routine (ISR) context;
 *          time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @details Re xfer the bulk-out endpoint when a previously-deferred frame can
 *          now be handed to the RX thread (i.e. rx_buf_free == 1). Called every
 *          SOF (1 ms FS / 125 us HS) so the endpoint stall is short-lived and
 *          no frames are dropped.
 */
static int cdc_ecm_sof(usb_dev_t *dev)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep_bulk_out = &ecm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ecm->ep_intr_in;

	UNUSED(dev);

	/* Retry a previously-failed notification when the endpoint is free. */
	if (ecm->notify_retry && ep_intr_in->xfer_state == 0U) {
		if (cdc_ecm_send_notification() == HAL_OK) {
			ecm->notify_retry = 0U;
		}
	}

	if (ecm->rx_pending_len == 0U) {
		return HAL_OK;
	}

	/* Thread still busy - wait for the next SOF. */
	if (ecm->rx_buf_free == 0U) {
		return HAL_OK;
	}

	/* Buffer is free: hand off the pending frame and re-arm the endpoint. */
	ecm->rx_buf_free = 0U;
	ecm->rx_msg_buf = ecm->rx_buf[ecm->rx_xfer_idx];
	ecm->rx_msg_len = ecm->rx_pending_len;
	usb_os_sema_give(ecm->rx_data_ready_sema);

	ecm->rx_xfer_idx ^= 1U;
	ep_bulk_out->xfer_buf = ecm->rx_buf[ecm->rx_xfer_idx];
	ecm->rx_pending_len = 0U;

	usbd_ep_receive(ecm->dev, ep_bulk_out);

	return HAL_OK;
}

/**
 * @brief Handle EP0 data OUT phase
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @retval Status
 */
static int cdc_ecm_handle_ep0_data_out(usb_dev_t *dev)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	int ret = HAL_ERR_HW;

	if (ecm->ctrl_req.bRequest != 0xFFU) {
		if (ecm->cb && ecm->cb->setup) {
			ret = ecm->cb->setup(&ecm->ctrl_req, ep0_out->xfer_buf);
		}
		ecm->ctrl_req.bRequest = 0xFFU; /* Mark as processed */
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
static u16 cdc_ecm_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usb_speed_type_t speed = dev->dev_speed;
	u8 desc_type = USB_HIGH_BYTE(req->wValue);
	u8 desc_idx = USB_LOW_BYTE(req->wValue);
	char mac_buf[32] = {0,};
	const u8 *desc = NULL;
	u16 len = 0;

	dev->self_powered = USBD_CDC_ECM_SELF_POWERED;
	dev->remote_wakeup_en = USBD_CDC_ECM_REMOTE_WAKEUP_EN;

	switch (desc_type) {
	case USB_DESC_TYPE_DEVICE:
		len = sizeof(usbd_cdc_ecm_dev_desc);
		usb_os_memcpy(buf, usbd_cdc_ecm_dev_desc, len);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
#ifndef CONFIG_USB_FS
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_cdc_ecm_hs_config_desc;
			len = sizeof(usbd_cdc_ecm_hs_config_desc);
		} else
#endif
		{
			desc = (u8 *)usbd_cdc_ecm_fs_config_desc;
			len = sizeof(usbd_cdc_ecm_fs_config_desc);
		}

		usb_os_memcpy((void *)buf, (void *)desc, len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		break;

#ifndef CONFIG_USB_FS
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		len = sizeof(usbd_cdc_ecm_device_qualifier_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_cdc_ecm_device_qualifier_desc, len);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		desc = (speed == USB_SPEED_HIGH) ?
			   usbd_cdc_ecm_fs_config_desc : usbd_cdc_ecm_hs_config_desc;
		len = (speed == USB_SPEED_HIGH) ?
			  sizeof(usbd_cdc_ecm_fs_config_desc) : sizeof(usbd_cdc_ecm_hs_config_desc);
		usb_os_memcpy(buf, desc, len);
		buf[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		break;
#endif

	case USB_DESC_TYPE_STRING:
		switch (desc_idx) {
		case USBD_IDX_LANGID_STR:
			len = sizeof(usbd_cdc_ecm_lang_id_desc);
			usb_os_memcpy(buf, usbd_cdc_ecm_lang_id_desc, len);
			break;
		case USBD_IDX_MFC_STR:
			len = usbd_get_str_desc(USBD_CDC_ECM_MFG_STRING, buf);
			break;
		case USBD_IDX_PRODUCT_STR:
			len = usbd_get_str_desc((speed == USB_SPEED_HIGH) ?
									USBD_CDC_ECM_PROD_HS_STRING : USBD_CDC_ECM_PROD_FS_STRING, buf);
			break;
		case USBD_IDX_SERIAL_STR:
			len = usbd_get_str_desc(USBD_CDC_ECM_SN_STRING, buf);
			break;
		case USBD_CDC_ECM_MAC_STRING_INDEX:
			cdc_ecm_mac_to_string((u8 *)(ecm->mac), mac_buf);
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
static void cdc_ecm_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;

	UNUSED(dev);

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		/* Clear link state immediately so upper layers see link_is_up == 0
		 * without waiting for usbd_cdc_ecm_deinit(). */
		ecm->connect_status = 0;
		ecm->notify_state = ECM_NOTIFY_NONE;
		ecm->notify_retry = 0U;

		ecm->rx_pending_len = 0U;
	}

	if (ecm->cb && ecm->cb->status_changed) {
		ecm->cb->status_changed(old_status, status);
	}
}

#if USBD_CDC_ECM_STATE_TRACE_ENABLE
/**
 * @brief Periodic state-trace thread.
 * @details Once every USBD_CDC_ECM_TRACE_INTERVAL_MS it prints three lines:
 *          (1) instantaneous link / endpoint / TX-ring-buffer state,
 *          (2) monotonic per-path completion counters (a frozen value pinpoints
 *              a wedged path without a debugger, including an RX thread heartbeat),
 *          (3) TX / RX throughput over the last interval.
 *          Each line's field legend is documented inline below.
 *          Compiled out when USBD_CDC_ECM_STATE_TRACE_ENABLE == 0.
 */
static void cdc_ecm_trace_thread(void *param)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep_bulk_in  = &ecm->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &ecm->ep_bulk_out;
	usbd_ep_t *ep_intr_in  = &ecm->ep_intr_in;
	usb_dev_t *dev;
	UNUSED(param);

	while (ecm->trace_task_running) {

		dev = ecm->dev;
		/* ecm->dev is assigned in set_config; NULL before enumeration. */
		if (dev != NULL) {
			/* Line 1 - instantaneous link / endpoint / ring-buffer state (not counters):
			 *   rdy = dev->is_ready (1=enumerated)   conn = network link reported up
			 *   ntf = notify_state / notify_retry    alt  = data-interface alt setting
			 *   ep i/o/t = xfer_state of BULK IN / BULK OUT / INTR IN (1=busy)
			 *   rx f/pend/idx = rx_buf_free / rx_pending_len / rx_xfer_idx
			 *   tx rb = frames currently queued in the TX ring buffer */
			RTK_LOGS(TAG, RTK_LOG_INFO,
					 "ready %d conn %d ntf %d/%d alt %d/ep i%d o%d t%d/rx f%d pend%d idx%d/tx rb%d\n",
					 dev->is_ready, ecm->connect_status,
					 ecm->notify_state, ecm->notify_retry, ecm->alt_setting,
					 ep_bulk_in->xfer_state, ep_bulk_out->xfer_state, ep_intr_in->xfer_state,
					 ecm->rx_buf_free, (u32)ecm->rx_pending_len, ecm->rx_xfer_idx,
					 usb_ringbuf_get_count(&ecm->bulk_tx_rb));
			/* Line 2 - monotonic per-path counters; a value frozen across intervals
			 * pinpoints a wedged path:
			 *   tx   = usbd_cdc_ecm_transmit() calls   in   = BULK IN  completion ISRs
			 *   out  = BULK OUT completion ISRs        intr = INTR IN  completion ISRs
			 *   rxd  = frames delivered to upper layer rxl  = RX thread heartbeat
			 *                                                 (frozen => RX thread wedged) */
			RTK_LOGS(TAG, RTK_LOG_INFO,
					 "cnt tx%d in%d out%d intr%d/rxd%d rxl%d/Heap %d\n",
					 ecm->dbg_tx_cnt, ecm->dbg_bulk_in_done_cnt, ecm->dbg_bulk_out_done_cnt,
					 ecm->dbg_intr_in_done_cnt, ecm->dbg_rx_deliver_cnt, ecm->dbg_rx_loop_cnt,
					 rtos_mem_get_free_heap_size());
		}

		usb_os_sleep_ms(USBD_CDC_ECM_TRACE_INTERVAL_MS);
	}

	ecm->trace_task = NULL;
	rtos_task_delete(NULL);
}

/**
 * @brief Create the ECM state-trace thread (idempotent).
 */
static void cdc_ecm_trace_task_init(void)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	int status;

	if (ecm->trace_task_running) {
		return;
	}

	ecm->trace_task_running = 1;
	status = rtos_task_create(&ecm->trace_task, "usbd_cdc_ecm_trace", cdc_ecm_trace_thread, NULL,
							  USBD_CDC_ECM_TRACE_THREAD_STACK_SIZE, USBD_CDC_ECM_TRACE_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create ECM trace task fail\n");
		ecm->trace_task_running = 0;
	}
}

/**
 * @brief Stop the ECM state-trace thread and wait for it to exit.
 */
static void cdc_ecm_trace_task_deinit(void)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	ecm->trace_task_running = 0;
	while (ecm->trace_task != NULL) {
		usb_os_sleep_ms(10);
	}
}
#endif /* USBD_CDC_ECM_STATE_TRACE_ENABLE */

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize CDC ECM class
 * @param cb: User callbacks
 * @retval Status
 */
int usbd_cdc_ecm_init(usbd_cdc_ecm_cb_t *cb)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep_bulk_in = &ecm->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &ecm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ecm->ep_intr_in;
	usb_ep_info_t *info;
	int ret = HAL_OK;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	ecm->ctrl_req.bRequest = 0xFFU;

	if (usb_ringbuf_manager_init(&ecm->bulk_tx_rb, USBD_CDC_ECM_BULK_TX_RB_SIZE,
								 USBD_CDC_ECM_BULK_BUF_MAX_SIZE, 1) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init bulk_tx ring buffer fail\n");
		return HAL_ERR_MEM;
	}

	/* Private DMA source buffer for BULK IN.  Each frame is copied here from the
	 * ring buffer head before transmission, so DMA never reads node memory. */
	ecm->bulk_tx_dma_buf = (u8 *)usb_os_malloc(USBD_CDC_ECM_BULK_BUF_MAX_SIZE);
	if (ecm->bulk_tx_dma_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc bulk_tx dma buf fail\n");
		ret = HAL_ERR_MEM;
		goto exit;
	}

	/* BULK IN transmits from ecm->bulk_tx_dma_buf (filled per-frame at TX time) */
	info = &ep_bulk_in->info;
	info->addr = USBD_CDC_ECM_BULK_IN_EP;
	info->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_in->xfer_buf_len = 0;
	ep_bulk_in->xfer_buf = NULL;

	/* Allocate BULK OUT ping-pong buffers */
	info = &ep_bulk_out->info;
	info->addr = USBD_CDC_ECM_BULK_OUT_EP;
	info->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_out->xfer_buf_len = USBD_CDC_ECM_BULK_BUF_MAX_SIZE;
	ep_bulk_out->xfer_len = ep_bulk_out->xfer_buf_len;

	ecm->rx_buf[0] = (u8 *)usb_os_malloc(USBD_CDC_ECM_BULK_BUF_MAX_SIZE);
	if (ecm->rx_buf[0] == NULL) {
		ret = HAL_ERR_MEM;
		goto exit;
	}

	ecm->rx_buf[1] = (u8 *)usb_os_malloc(USBD_CDC_ECM_BULK_BUF_MAX_SIZE);
	if (ecm->rx_buf[1] == NULL) {
		ret = HAL_ERR_MEM;
		goto cleanup_rx_buf0;
	}

	ecm->rx_xfer_idx = 0U;
	ep_bulk_out->xfer_buf = ecm->rx_buf[ecm->rx_xfer_idx];

	/* bulk_tx_slot_sema: ISR -> tcpip, given each time a TX ring buffer slot is freed. */
	if (usb_os_sema_create(&ecm->bulk_tx_slot_sema) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create TX slot sema fail\n");
		ret = HAL_ERR_MEM;
		goto cleanup_rx_buf1;
	}

	/* rx_data_ready_sema: ISR -> thread, given when a buffer is filled. */
	if (usb_os_sema_create(&ecm->rx_data_ready_sema) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create RX data_ready sema fail\n");
		ret = HAL_ERR_MEM;
		goto cleanup_tx_slot_sema;
	}

	ecm->rx_buf_free    = 1U;
	ecm->rx_pending_len = 0U;

	/* Start RX delivery thread */
	ecm->rx_thread_running = 1;
	ret = rtos_task_create(&ecm->rx_task, "usbd_cdc_ecm_rx_thread", usbd_cdc_ecm_rx_thread, NULL,
						   USBD_CDC_ECM_RX_THREAD_STACK_SIZE, USBD_CDC_ECM_RX_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create RX thread fail\n");
		ecm->rx_thread_running = 0;
		ret = HAL_ERR_HW;
		goto cleanup_rx_data_ready_sema;
	}

	/* Allocate INTR IN buffer */
	info = &ep_intr_in->info;
	info->addr = USBD_CDC_ECM_INTR_IN_EP;
	info->type = USB_CH_EP_TYPE_INTR;
	ep_intr_in->xfer_buf_len = USB_CDC_ECM_INTR_IN_PACKET_SIZE;
	ep_intr_in->xfer_buf = (u8 *)usb_os_malloc(ep_intr_in->xfer_buf_len);
	if (ep_intr_in->xfer_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto cleanup_rx_thread;
	}

	if ((cb != NULL) && (cb->priv != NULL) && (cb->priv->mac_value != NULL)) {
		cdc_ecm_set_mac(cb->priv->mac_value);
	} else {
		cdc_ecm_set_mac((u8 *)ecm_mac);
	}

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "User init fail: %d\n", ret);
			goto cleanup_intr_in;
		}
	}

	/* Initialize user callbacks */
	ecm->cb = cb;

	/* Register CDC ECM class driver */
	usbd_register_class(&usbd_cdc_driver);

#if USBD_CDC_ECM_STATE_TRACE_ENABLE
	cdc_ecm_trace_task_init();
#endif

	return HAL_OK;

cleanup_intr_in:
	usb_os_mfree(ep_intr_in->xfer_buf);
	ep_intr_in->xfer_buf = NULL;

cleanup_rx_thread:
	ecm->rx_thread_running = 0;
	/* Wake the thread so it can observe running=0 and exit. */
	usb_os_sema_give(ecm->rx_data_ready_sema);
	while (ecm->rx_task != NULL) {
		usb_os_sleep_ms(10);
	}

cleanup_rx_data_ready_sema:
	usb_os_sema_delete(ecm->rx_data_ready_sema);
	ecm->rx_data_ready_sema = NULL;

cleanup_tx_slot_sema:
	usb_os_sema_delete(ecm->bulk_tx_slot_sema);
	ecm->bulk_tx_slot_sema = NULL;

cleanup_rx_buf1:
	usb_os_mfree(ecm->rx_buf[1]);
	ecm->rx_buf[1] = NULL;

cleanup_rx_buf0:
	usb_os_mfree(ecm->rx_buf[0]);
	ecm->rx_buf[0] = NULL;
	ep_bulk_out->xfer_buf = NULL;

exit:
	if (ecm->bulk_tx_dma_buf != NULL) {
		usb_os_mfree(ecm->bulk_tx_dma_buf);
		ecm->bulk_tx_dma_buf = NULL;
	}
	usb_ringbuf_manager_deinit(&ecm->bulk_tx_rb);

	return ret;
}

/**
 * @brief Deinitialize CDC ECM class
 * @retval Status
 */
int usbd_cdc_ecm_deinit(void)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep_bulk_in = &ecm->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &ecm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ecm->ep_intr_in;
	int wait_cnt = 0;

	ecm->connect_status = 0;

#if USBD_CDC_ECM_STATE_TRACE_ENABLE
	/* Stop the trace thread first so it does not read state being torn down. */
	cdc_ecm_trace_task_deinit();
#endif

	/* Wait for ongoing BULK/INTR DMA transfers to complete.
	 * xfer_state is set to 1 before usbd_ep_transmit() and cleared to 0
	 * by the XFRC ISR, so it is the correct in-flight indicator.
	 * (is_busy is a function-call-level flag and clears too early.) */
	while ((ep_bulk_in->xfer_state || ep_intr_in->xfer_state) && wait_cnt < 10000) {
		usb_os_delay_us(100);
		wait_cnt++;
	}

	/* Unregister class driver */
	usbd_unregister_class();

	/* After unregister, no more USB ISRs will fire.  Free the TX ring buffer
	 * and the private BULK IN DMA buffer. */
	usb_ringbuf_manager_deinit(&ecm->bulk_tx_rb);
	if (ecm->bulk_tx_dma_buf != NULL) {
		usb_os_mfree(ecm->bulk_tx_dma_buf);
		ecm->bulk_tx_dma_buf = NULL;
	}

	/* Stop the RX delivery thread and wait for it to exit. */
	ecm->rx_thread_running = 0;
	if (ecm->rx_data_ready_sema != NULL) {
		usb_os_sema_give(ecm->rx_data_ready_sema);
	}
	while (ecm->rx_task != NULL) {
		usb_os_sleep_ms(10);
	}

	/* Unblock any transmit() still waiting for a TX slot, then free the sema. */
	if (ecm->bulk_tx_slot_sema != NULL) {
		usb_os_sema_give(ecm->bulk_tx_slot_sema);
		usb_os_sema_delete(ecm->bulk_tx_slot_sema);
		ecm->bulk_tx_slot_sema = NULL;
	}

	/* Tear down RX semaphores. */
	if (ecm->rx_data_ready_sema != NULL) {
		usb_os_sema_delete(ecm->rx_data_ready_sema);
		ecm->rx_data_ready_sema = NULL;
	}
	/* Call user deinit */
	if (ecm->cb && ecm->cb->deinit) {
		ecm->cb->deinit();
	}

	/* Free buffers */
	if (ep_intr_in->xfer_buf != NULL) {
		usb_os_mfree(ep_intr_in->xfer_buf);
		ep_intr_in->xfer_buf = NULL;
	}

	/* Free RX ping-pong buffers. */
	for (u8 i = 0; i < USBD_CDC_ECM_RX_BUF_NUM; i++) {
		if (ecm->rx_buf[i] != NULL) {
			usb_os_mfree(ecm->rx_buf[i]);
			ecm->rx_buf[i] = NULL;
		}
	}
	ep_bulk_out->xfer_buf = NULL;

	return HAL_OK;
}

/**
  * @brief  Start to transmit data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @param  block: When non-zero, block up to USBD_CDC_ECM_BULK_TX_TIMEOUT_MS ms
  *                waiting for a ring buffer slot before dropping.  Zero means
  *                non-blocking - drop immediately when the ring buffer is full.
  * @retval HAL_OK on success, HAL_BUSY if the ring buffer is full (frame dropped).
  */
int usbd_cdc_ecm_transmit(u8 *buf, u32 len, u8 block)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	int ret;

	/* Reject invalid frames up front.  A NULL buffer or zero length would be
	 * accepted by usb_ringbuf_add_tail() but always rejected by cdc_ecm_bulk_send()
	 * (it returns on buf==NULL / len==0 without starting a transfer), leaving the
	 * node stuck at the ring buffer head forever - head only advances on XFRC,
	 * which never fires for an un-started transfer. */
	if ((buf == NULL) || (len == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "TX bad param (len %u)\n", len);
		return HAL_ERR_PARA;
	}

#if USBD_ECM_TX_SPEED_CHECK
	static u64 usb_tx_start_time = 0, usb_tx_end_time, usb_tx_interval_time;
	static u64 usb_tx_total_len = 0;

	usb_tx_total_len += len;
	if (usb_tx_start_time == 0) {
		usb_tx_start_time = usb_os_get_timestamp_ms();
	}
#endif

#if USBD_CDC_ECM_STATE_TRACE_ENABLE
	ecm->dbg_tx_cnt++;
#endif

	/* Copy frame into the ring buffer.
	 * block != 0: when the ring buffer is full, wait for the ISR to signal a free
	 *             slot via bulk_tx_slot_sema rather than busy-sleeping.  Wakes up
	 *             as soon as a XFRC fires (typically within one USB microframe),
	 *             then retries.  Bounded by USBD_CDC_ECM_BULK_TX_TIMEOUT_MS.
	 * block == 0: non-blocking; drop immediately when full. */
	if (block) {
		do {
			ret = usb_ringbuf_add_tail(&ecm->bulk_tx_rb, buf, len, 1U);
			if (ret == HAL_OK) {
				break;
			}
			if (usb_os_sema_take(ecm->bulk_tx_slot_sema,
								 USBD_CDC_ECM_BULK_TX_TIMEOUT_MS) != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "TX timeout drop(%u)\n", len);
				return HAL_BUSY;
			}
			/* Sema may have been fired by deinit/clear_config rather than a real
			 * XFRC.  If the device is no longer connected the ring buffer and
			 * endpoint are being torn down - bail out before touching them. */
			if (ecm->connect_status == 0U) {
				return HAL_BUSY;
			}
		} while (1);
	} else {
		ret = usb_ringbuf_add_tail(&ecm->bulk_tx_rb, buf, len, 1U);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "TX drop(%u)\n", len);
			return HAL_BUSY;
		}
	}

#if USBD_CDC_ECM_STATE_TRACE_ENABLE
	ecm->dbg_tx_bytes += len;
#endif

	/* If the BULK IN endpoint is idle, kick the first DMA now.  Otherwise the
	 * ISR chains the next frame automatically after each XFRC interrupt.
	 * The connect_status guard mirrors the blocking path above: skip the kick
	 * when the device is disconnecting so cdc_ecm_bulk_tx_start_from_rb() does
	 * not dereference a ring buffer that deinit may be freeing.  This narrows
	 * the window but does not fully close it - the upper layer must still stop
	 * TX (netif down) before calling deinit. */
	if ((ecm->connect_status != 0U) && (ecm->ep_bulk_in.xfer_state == 0U)) {
		cdc_ecm_bulk_tx_start_from_rb();
	}

#if USBD_ECM_TX_SPEED_CHECK
	usb_tx_end_time = usb_os_get_timestamp_ms();
	usb_tx_interval_time = (usb_tx_end_time - usb_tx_start_time) * RTOS_TICK_RATE_MS;

	if (usb_tx_interval_time >= 3000) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Heap %d TX %dB in %d ms, %d Kbps\n",
				 rtos_mem_get_free_heap_size(),
				 (u32)usb_tx_total_len, (u32)usb_tx_interval_time,
				 (u32)((usb_tx_total_len * 8 * 1000) / (usb_tx_interval_time * 1024)));
		usb_tx_start_time = usb_tx_end_time;
		usb_tx_total_len = 0;
	}
#endif

	return HAL_OK;
}

/**
  * @brief  Get the mac str, if the dongle support the standard CDC ECM
  * @retval mac string,the length is 6 Bytes
  */
const u8 *usbd_cdc_ecm_get_mac_str(void)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	u8 i = 0;

	while (!ecm->mac_valid && i < 10) {
		usb_os_sleep_ms(1000);
		i++;
		RTK_LOGS(TAG, RTK_LOG_INFO,  "Wait MAC ready\n");
	}

	if (ecm->mac_valid == 0) {
		return NULL;
	}

	return ecm->mac;
}

/**
  * @brief  Get ecm device connect status
  * @retval device connect status
  */
int usbd_cdc_ecm_get_connect_status(void)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;

	return ecm->connect_status;
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
int usbd_cdc_ecm_set_link_status(u8 link_up)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;

	link_up = link_up ? 1U : 0U;

	/* Only act on an actual transition. */
	if (ecm->connect_status == link_up) {
		return HAL_OK;
	}
	ecm->connect_status = link_up;

	/* Queue a NETWORK_CONNECTION notification; the SOF handler performs the
	 * actual transmit so all INTR IN endpoint access stays in ISR context. */
	ecm->notify_state = ECM_NOTIFY_CONNECT;
	ecm->notify_retry = 1U;

	return HAL_OK;
}

__attribute__((weak))
int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbd_cdc_ecm_transmit(buf, len, block);
}
