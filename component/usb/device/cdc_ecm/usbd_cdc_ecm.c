/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_cdc_ecm.h"
#ifdef CONFIG_USBD_COMPOSITE
#include "usbd_composite.h"
#endif

/* Private defines -----------------------------------------------------------*/

#define USBD_CDC_ECM_RX_SPEED_CHECK                       0                     /* CDC ECM rx speed test */
#define USBD_CDC_ECM_TX_SPEED_CHECK                       0                     /* CDC ECM tx speed test */

/* TX ring buffer capacity (usbd_cdc_ecm_config_t::bulk_tx_queue_depth) is the number of
 * Ethernet frames that can be queued while a DMA transfer is in progress; the default
 * tolerates bursts without blocking the caller.
 *
 * The matching blocking-transmit timeout (usbd_cdc_ecm_config_t::bulk_tx_timeout_ms) must be
 * long enough to cover the ring-buffer drain time under peak throughput:
 *   USB HS (480 Mbps bus, ~40 MB/s BULK effective): depth * 1514 B / 40 MB/s -> 0.4 ms
 *   USB FS ( 12 Mbps bus, ~1.5 MB/s BULK effective): depth * 1514 B / 1.5 MB/s -> 10 ms
 * The 10 ms default gives 2 ~ 6 * headroom for normal bursts without risking a noticeable
 * tcpip-task freeze when the host stops polling BULK IN (stuck-TX scenario). */

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
 * rx_xfer_idx tracks which of the two buffers is currently armed for USB OUT.
 *
 * The thread's stack size and priority come from usbd_cdc_ecm_config_t. */

/* Poll period (ms) of the RX thread's blocking wait: bounds how long the thread takes to
 * observe rx_thread_running == 0 at teardown, not a functional parameter. */
#define USBD_CDC_ECM_RX_SEMA_TAKE_TIMEOUT_MS          100U

/* Period of the warning deinit logs while a task does not leave usbd_cdc_ecm_transmit().  A
 * woken caller leaves within microseconds, only a starved one takes longer. */
#define USBD_CDC_ECM_TX_EXIT_WARN_MS                5000U

#if USBD_CDC_ECM_STATE_TRACE_ENABLE
#define USBD_CDC_ECM_TRACE_THREAD_STACK_SIZE         1024U
#define USBD_CDC_ECM_TRACE_THREAD_PRIORITY           1U
#define USBD_CDC_ECM_TRACE_INTERVAL_MS               1000U
#endif

/* Private types -------------------------------------------------------------*/

/**
 * @brief ECM notification state machine
 */
enum usbd_cdc_ecm_notify_state {
	USBD_ECM_NOTIFY_NONE,      /**< No notification pending */
	USBD_ECM_NOTIFY_CONNECT,   /**< Network connection notification */
	USBD_ECM_NOTIFY_SPEED,     /**< Speed change notification */
};

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_ecm_set_config(usb_dev_t *dev, u8 config);
static void usbd_ecm_clear_config(usb_dev_t *dev, u8 config);
static int usbd_ecm_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_ecm_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
static int usbd_ecm_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_ecm_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, int status);
static int usbd_ecm_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static void usbd_ecm_sof(usb_dev_t *dev);
static void usbd_ecm_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
#ifdef CONFIG_USBD_COMPOSITE
static void usbd_ecm_set_interface_base(u8 base);
#endif
static void usbd_ecm_bulk_tx_start_from_rb(void);
static int usbd_ecm_data_alt_start(usb_dev_t *dev);
static void usbd_ecm_data_alt_stop(usb_dev_t *dev);
static int usbd_ecm_intr_in_send(void *data, u16 len);
static int usbd_ecm_send_notification(void);
static inline u8 usbd_ecm_char_to_hex(u8 value);
static void usbd_ecm_mac_to_string(const u8 *mac, char *mac_str);
static void usbd_ecm_set_mac(const u8 *mac);
static int usbd_ecm_bulk_send(u8 *buf, u32 len);
static int usbd_ecm_tx_enqueue(const u8 *buf, u32 len, u8 block);
#if USBD_CDC_ECM_STATE_TRACE_ENABLE
static void usbd_ecm_trace_task_init(void);
static void usbd_ecm_trace_task_deinit(void);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "ECM";
static const u8 usbd_ecm_default_mac[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};  /**< MAC address */

/* USB Standard Device Descriptor */
static const u8 usbd_cdc_ecm_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,                               /* bLength */
	USB_DESC_TYPE_DEVICE,                           /* bDescriptorType */
	0x00, 0x02,                                     /* bcdUSB: 2.0 */
	USB_CDC_CLASS_CODE,                             /* bDeviceClass: CDC */
	0x00,                                           /* bDeviceSubClass: 0x00 (must be 0x00 per CDC 1.2 5.1.1) */
	0x00,                                           /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize0 */
	USB_LOW_BYTE(USB_VID),                          /* idVendor (patched by core from usbd_dev_info_t) */
	USB_HIGH_BYTE(USB_VID),
	USB_LOW_BYTE(USB_PID),                          /* idProduct (patched by core from usbd_dev_info_t) */
	USB_HIGH_BYTE(USB_PID),
	0x00, 0x02,                                     /* bcdDevice: 2.0 (patched by core from usbd_dev_info_t) */
	USBD_IDX_MFC_STR,                               /* iManufacturer (patched by core) */
	USBD_IDX_PRODUCT_STR,                           /* iProduct (patched by core) */
	USBD_IDX_SERIAL_STR,                            /* iSerialNumber (patched by core) */
	0x01                                            /* bNumConfigurations */
};

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
/* USB Device Qualifier Descriptor */
static const u8 usbd_cdc_ecm_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
	USB_LEN_DEV_QUALIFIER_DESC,                     /* bLength */
	USB_DESC_TYPE_DEVICE_QUALIFIER,                 /* bDescriptorType */
	0x00, 0x02,                                     /* bcdUSB: 2.0 */
	USB_CDC_CLASS_CODE,                             /* bDeviceClass: CDC, same as the device descriptor (USB 2.0 9.6.2) */
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
	0x80,                                           /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,                                           /* bMaxPower: 100mA */

	/* IAD for CDC ECM (Communication + Data) */
	USB_LEN_IAD_DESC,                               /* bLength */
	USB_DESC_TYPE_IAD,                              /* bDescriptorType */
	0x00,                                           /* bFirstInterface (patched by composite) */
	0x02,                                           /* bInterfaceCount */
	USB_CDC_COMM_INTERFACE_CLASS_CODE,              /* bFunctionClass: CDC */
	USB_CDC_ECM_SUBCLASS_ECM,                       /* bFunctionSubClass: ECM */
	USB_CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC,        /* bFunctionProtocol */
	0x00,                                           /* iFunction */

	/* Communication Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_ECM_COMM_INTERFACE_NUM,                /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x01,                                           /* bNumEndpoints */
	USB_CDC_COMM_INTERFACE_CLASS_CODE,              /* bInterfaceClass: CDC */
	USB_CDC_ECM_SUBCLASS_ECM,                           /* bInterfaceSubClass: ECM */
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
	USBD_CDC_ECM_COMM_INTERFACE_NUM,                /* bControlInterface */
	USBD_CDC_ECM_DATA_INTERFACE_NUM,                /* bSubordinateInterface0 */

	/* CDC Ethernet Networking Functional Descriptor */
	USB_CDC_ECM_ETHERNET_FUNC_DESC_SIZE,            /* bFunctionLength */
	USB_CDC_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_FUNC_DESC_ETHERNET_NETWORKING,          /* bDescriptorSubtype */
	0x00,                                           /* iMACAddress, runtime patched */
	0x00, 0x00, 0x00, 0x00,                         /* bmEthernetStatistics */
	USB_LOW_BYTE(USB_CDC_ECM_MAX_SEGMENT_SIZE),     /* wMaxSegmentSize */
	USB_HIGH_BYTE(USB_CDC_ECM_MAX_SEGMENT_SIZE),
	0x00, 0x00,                                     /* wNumberMCFilters */
	0x00,                                           /* bNumberPowerFilters */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_D2H,                                        /* bEndpointAddress: dir IN (placeholder) */
	USB_CH_EP_TYPE_INTR,                            /* bmAttributes */
	USB_LOW_BYTE(USB_CDC_ECM_INTR_IN_PACKET_SIZE),  /* wMaxPacketSize */
	USB_HIGH_BYTE(USB_CDC_ECM_INTR_IN_PACKET_SIZE),
	USBD_CDC_ECM_DEFAULT_INTR_IN_INTERVAL,          /* bInterval (patched at runtime) */

	/* Data Interface Descriptor (Alt 0 - No endpoints) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_ECM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x00,                                           /* bNumEndpoints */
	USB_CDC_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	USB_CDC_SUBCLASS_RESERVED,                      /* bInterfaceSubClass */
	USB_CDC_DATA_PROTOCOL_NO_CLASS_SPECIFIC,        /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* Data Interface Descriptor (Alt 1 - Active) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_ECM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x01,                                           /* bAlternateSetting */
	0x02,                                           /* bNumEndpoints */
	USB_CDC_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	USB_CDC_SUBCLASS_RESERVED,                      /* bInterfaceSubClass */
	USB_CDC_DATA_PROTOCOL_NO_CLASS_SPECIFIC,        /* bInterfaceProtocol */
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

/* USB CDC ECM Full Speed Configuration Descriptor */
static const u8 usbd_cdc_ecm_fs_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	0x00, 0x00,                                     /* wTotalLength (calculated later) */
	0x02,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
	0x80,                                           /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,                                           /* bMaxPower: 100mA */

	/* IAD for CDC ECM (Communication + Data) */
	USB_LEN_IAD_DESC,                               /* bLength */
	USB_DESC_TYPE_IAD,                              /* bDescriptorType */
	0x00,                                           /* bFirstInterface (patched by composite) */
	0x02,                                           /* bInterfaceCount */
	USB_CDC_COMM_INTERFACE_CLASS_CODE,              /* bFunctionClass: CDC */
	USB_CDC_ECM_SUBCLASS_ECM,                       /* bFunctionSubClass: ECM */
	USB_CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC,        /* bFunctionProtocol */
	0x00,                                           /* iFunction */

	/* Communication Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_ECM_COMM_INTERFACE_NUM,                /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x01,                                           /* bNumEndpoints */
	USB_CDC_COMM_INTERFACE_CLASS_CODE,              /* bInterfaceClass: CDC */
	USB_CDC_ECM_SUBCLASS_ECM,                           /* bInterfaceSubClass: ECM */
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
	USBD_CDC_ECM_COMM_INTERFACE_NUM,                /* bControlInterface */
	USBD_CDC_ECM_DATA_INTERFACE_NUM,                /* bSubordinateInterface0 */

	/* CDC Ethernet Networking Functional Descriptor */
	USB_CDC_ECM_ETHERNET_FUNC_DESC_SIZE,            /* bFunctionLength */
	USB_CDC_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_FUNC_DESC_ETHERNET_NETWORKING,          /* bDescriptorSubtype */
	0x00,                                           /* iMACAddress, runtime patched */
	0x00, 0x00, 0x00, 0x00,                         /* bmEthernetStatistics */
	USB_LOW_BYTE(USB_CDC_ECM_MAX_SEGMENT_SIZE),     /* wMaxSegmentSize */
	USB_HIGH_BYTE(USB_CDC_ECM_MAX_SEGMENT_SIZE),
	0x00, 0x00,                                     /* wNumberMCFilters */
	0x00,                                           /* bNumberPowerFilters */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_D2H,                                        /* bEndpointAddress: dir IN (placeholder) */
	USB_CH_EP_TYPE_INTR,                            /* bmAttributes */
	USB_LOW_BYTE(USB_CDC_ECM_INTR_IN_PACKET_SIZE),  /* wMaxPacketSize */
	USB_HIGH_BYTE(USB_CDC_ECM_INTR_IN_PACKET_SIZE),
	USBD_CDC_ECM_DEFAULT_INTR_IN_INTERVAL,          /* bInterval (patched at runtime) */

	/* Data Interface Descriptor (Alt 0 - No endpoints) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_ECM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x00,                                           /* bNumEndpoints */
	USB_CDC_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	USB_CDC_SUBCLASS_RESERVED,                      /* bInterfaceSubClass */
	USB_CDC_DATA_PROTOCOL_NO_CLASS_SPECIFIC,        /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* Data Interface Descriptor (Alt 1 - Active) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_ECM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x01,                                           /* bAlternateSetting */
	0x02,                                           /* bNumEndpoints */
	USB_CDC_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	USB_CDC_SUBCLASS_RESERVED,                      /* bInterfaceSubClass */
	USB_CDC_DATA_PROTOCOL_NO_CLASS_SPECIFIC,        /* bInterfaceProtocol */
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

/* CDC ECM Class Driver */
static const usbd_class_driver_t usbd_cdc_ecm_driver = {
	.get_descriptor = usbd_ecm_get_descriptor,
	.set_config = usbd_ecm_set_config,
	.clear_config = usbd_ecm_clear_config,
	.setup = usbd_ecm_setup,
	.ep0_data_out = usbd_ecm_handle_ep0_data_out,
	.ep_data_in = usbd_ecm_handle_ep_data_in,
	.ep_data_out = usbd_ecm_handle_ep_data_out,
	.sof = usbd_ecm_sof,
	.status_changed = usbd_ecm_status_changed,
#ifdef CONFIG_USBD_COMPOSITE
	.set_interface_base = usbd_ecm_set_interface_base,
#endif
};

/* CDC ECM Device Instance */
static usbd_cdc_ecm_dev_t usbd_cdc_ecm_dev;

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Convert a nibble (0-15) to its ASCII hex character.
 * @param value: Nibble value (0x0-0xF).
 * @retval ASCII character '0'-'9' or 'A'-'F'. Returns '0' for out-of-range input.
 */
static inline u8 usbd_ecm_char_to_hex(u8 value)
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
static void usbd_ecm_mac_to_string(const u8 *mac, char *mac_str)
{
	u8 str_index = 0;
	u8 i;

	for (i = 0; i < 6; i++) {
		/* high 4 bits */
		mac_str[str_index++] = usbd_ecm_char_to_hex((mac[i] >> 4) & 0x0F);

		/* low 4 bits */
		mac_str[str_index++] = usbd_ecm_char_to_hex(mac[i] & 0x0F);
	}

	mac_str[str_index] = '\0';
}

/**
 * @brief Set the device MAC address from an external source.
 * @param mac: Pointer to a 6-byte MAC address buffer. NULL is ignored.
 */
static void usbd_ecm_set_mac(const u8 *mac)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;

	if (NULL == mac) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "No Param\n");
		return;
	}

	usb_os_memcpy((void *) & (ecm->mac[0]), (const void *)mac, USBD_CDC_ECM_MAC_STR_LEN);

	/* Keep the string form in step: the core holds it by pointer for the iMACAddress
	   string descriptor, so it is formatted here rather than on each request */
	usbd_ecm_mac_to_string(ecm->mac, ecm->mac_str);
}

/**
 * @brief Start DMA for the head frame in the TX ring buffer.
 * @note  This function is called within an interrupt service routine (ISR) context only
 *        (SOF and BULK IN completion), which never run concurrently, so it is the single
 *        consumer of the TX ring buffer and needs no lock even on SMP.
 *        The head frame is copied out of the ring buffer into ecm->bulk_tx_dma_buf
 *        via the public usb_ringbuf_remove_head() API, which also advances the head
 *        and frees the slot.  The DMA then runs from bulk_tx_dma_buf, so it never
 *        references ring buffer node memory and we touch no ring buffer internals.
 *        Because the slot is released here (not in usbd_ecm_handle_ep_data_in), the
 *        free-slot semaphore is given here too.
 */
static void usbd_ecm_bulk_tx_start_from_rb(void)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usb_ringbuf_manager_t *rb = &ecm->bulk_tx_rb;
	u32 frame_len;
	int ret;

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

	/* A slot just freed up - wake any producer blocked in usbd_cdc_ecm_transmit().  A
	 * binary semaphore that is already given cannot be given again, which is harmless. */
	usb_os_sema_give(ecm->bulk_tx_slot_sema);

	/* The frame is already out of the ring, so a failed submit drops it.  The return
	 * value must not be discarded (MISRA-C:2012 Rule 17.7): account for the drop so a
	 * silently lost frame is visible instead of invisible.  No retry is attempted - the
	 * only reachable failure is a device that went not-ready (bus reset / detach), where
	 * the frame is undeliverable anyway, and the host retransmits at the IP layer. */
	ret = usbd_ecm_bulk_send(ecm->bulk_tx_dma_buf, frame_len);
	if (ret != RTK_SUCCESS) {
#if USBD_CDC_ECM_STATE_TRACE_ENABLE
		ecm->dbg_tx_drop_cnt++;
		RTK_LOGS(TAG, RTK_LOG_WARN, "TX submit fail %d drop(%u)\n", ret, frame_len);
#endif
	}
}

/**
 * @brief  Activate the data interface: bring up the BULK IN/OUT endpoints.
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 *         Ref CDC ECM 1.2 3.2.2 and USB 2.0 9.4.10: the BULK endpoints belong to
 *         alternate setting 1 of the data interface, so they may only exist while
 *         the host has selected that alternate setting.
 *         Idempotent: a repeated SET_INTERFACE for the alternate setting already in
 *         use does not re-initialise the endpoints, so an in-flight transfer is
 *         never disturbed.  The caller still clears any STALL condition, which is
 *         what USB 2.0 9.4.10 requires for a repeated request.
 *         On failure every step already taken is undone, so the interface stays in alt 0
 *         and the caller answers SET_INTERFACE with a request error.
 * @param  dev: USB device instance.
 * @retval RTK_SUCCESS on success, otherwise the error of the failing endpoint operation.
 */
static int usbd_ecm_data_alt_start(usb_dev_t *dev)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep_bulk_in = &ecm->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &ecm->ep_bulk_out;
	u16 mps;
	int ret;

	if (ecm->data_alt_setting != 0U) {
		return RTK_SUCCESS;
	}

	mps = (dev->dev_speed == USB_SPEED_HIGH) ? (u16)USB_BULK_HS_MAX_MPS : (u16)USB_BULK_FS_MAX_MPS;

	/* Initialize BULK IN endpoint */
	ep_bulk_in->xfer_state = 0U;
	ep_bulk_in->info.mps = mps;
	ret = usbd_ep_init(dev, ep_bulk_in);
	if (ret != RTK_SUCCESS) {
		usbd_ecm_data_alt_stop(dev);
		return ret;
	}

	/* Initialize BULK OUT endpoint */
	ep_bulk_out->info.mps = mps;
	ret = usbd_ep_init(dev, ep_bulk_out);
	if (ret != RTK_SUCCESS) {
		usbd_ecm_data_alt_stop(dev);
		return ret;
	}

	/* Publish the new alternate setting before arming the first OUT transfer, so
	 * a completion that fires immediately observes a consistent state.  This is
	 * also the only place that raises data_alt_setting to 1: keeping the write
	 * inside the helper is what guarantees the field can never disagree with the
	 * real endpoint state. */
	ecm->data_alt_setting = 1U;

	/* Start receiving */
	ret = usbd_ep_receive(dev, ep_bulk_out);
	if (ret != RTK_SUCCESS) {
		usbd_ecm_data_alt_stop(dev);
	}

	return ret;
}

/**
 * @brief  Deactivate the data interface: tear the BULK IN/OUT endpoints down.
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 *         Fully idempotent, so it is safe to call for an already-inactive data
 *         interface: usbd_ep_deinit() tolerates a never-initialised endpoint and
 *         every other step only resets state.  usbd_ecm_clear_config() reuses it
 *         for exactly that reason.
 * @param  dev: USB device instance.
 * @retval None
 */
static void usbd_ecm_data_alt_stop(usb_dev_t *dev)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;

	/* Drop back to the default alternate setting first: usbd_cdc_ecm_transmit()
	 * must not start a new DMA once the endpoints are being torn down.  As in
	 * _start(), this is the only place that lowers data_alt_setting to 0. */
	ecm->data_alt_setting = 0U;

	usbd_ep_deinit(dev, &ecm->ep_bulk_in);
	usbd_ep_deinit(dev, &ecm->ep_bulk_out);

	/* No OUT transfer can be outstanding any more; drop the deferred length so
	 * the SOF handler does not re-arm a de-initialised endpoint. */
	ecm->rx_pending_len = 0U;

	/* Discard any queued frames and reset the endpoint state so the next
	 * activation starts with a clean TX path.  Only the consumer index moves:
	 * transmit() may be adding a frame on another core and owns the tail. */
	usb_ringbuf_drop_all(&ecm->bulk_tx_rb);
	ecm->ep_bulk_in.xfer_state = 0U;

	/* Unblock any transmit() call that is waiting on a ring buffer slot. */
	if (ecm->bulk_tx_slot_sema != NULL) {
		usb_os_sema_give(ecm->bulk_tx_slot_sema);
	}
}

/**
 * @brief Deliver a received frame to the upper-layer callback.
 * @param buf:    Pointer to the received data buffer.
 * @param length: Length of the received frame in bytes.
 */
static int usbd_ecm_bulk_receive(u8 *buf, u32 length)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
#if USBD_CDC_ECM_RX_SPEED_CHECK
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
				 usb_os_get_free_heap_size(),
				 (u32)usb_rx_total_len, (u32)usb_rx_interval_time, (u32)((usb_rx_total_len * 8 * 1000) / (usb_rx_interval_time * 1024)));
		usb_rx_start_time = usb_rx_end_time;
		usb_rx_total_len = 0;
	}
#endif

	if ((ecm->cb != NULL) && (ecm->cb->received != NULL)) {
		/* The frame is consumed either way: there is nothing to retry on a callback error */
		ecm->cb->received(buf, length);
	}

	return RTK_SUCCESS;
}

/**
 * @brief  Send the next pending INTR IN notification and advance the state machine.
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @retval RTK_SUCCESS on success, -RTK_ERR_BADARG if no notification is pending,
 *         or a RTK error code if the transfer could not be submitted.
 */
static int usbd_ecm_send_notification(void)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usb_dev_t *dev = ecm->dev;
	usb_cdc_ecm_notify_t event;
	int status;
	u32 bitrate;
	u16 length;
	u8 next_state;

	event.bmRequestType = 0xA1;
	/* Class notifications are emitted on the communication (control) interface's
	 * notification endpoint, so wIndex carries the communication interface
	 * number (Ref CDC 1.2 6.3).  The BULK data interface is a different
	 * interface and must not be named here. */
	event.wIndex = (u16)(ecm->if_base + USBD_CDC_ECM_COMM_INTERFACE_NUM);

	switch (ecm->notify_state) {
	case USBD_ECM_NOTIFY_CONNECT:
		event.bNotificationCode = USB_CDC_NOTIFY_NETWORK_CONNECTION;
		event.wValue = ecm->connect_status;
		event.wLength = 0;
		length = USB_CDC_ECM_NETWORK_CONNECTION_SIZE;
		/* Follow a "connected" notification with a speed-change report; a
		 * "disconnected" notification stands alone (no trailing SPEED). */
		next_state = ecm->connect_status ? USBD_ECM_NOTIFY_SPEED : USBD_ECM_NOTIFY_NONE;
		break;

	case USBD_ECM_NOTIFY_SPEED:
		event.bNotificationCode = USB_CDC_NOTIFY_CONNECTION_SPEED_CHANGE;
		event.wValue = 0;
		event.wLength = 8;
		/* Ref CDC 1.2 6.3.3: DLBitRate/ULBitRate are the actual connection bit rates
		 * in bits/s, so report the enumerated bus rate rather than 0 (which would tell
		 * the host the link has no bandwidth at all).  The link is symmetric. */
		bitrate = (dev->dev_speed == USB_SPEED_HIGH) ? USB_CDC_CONNECTION_BITRATE_HS : USB_CDC_CONNECTION_BITRATE_FS;
		event.data.DLBitRate = bitrate; /* Downstream bits/sec */
		event.data.ULBitRate = bitrate; /* Upstream bits/sec */
		length = USB_CDC_ECM_CONNECTION_SPEED_CHANGE_SIZE;
		next_state = USBD_ECM_NOTIFY_NONE;
		break;

	case USBD_ECM_NOTIFY_NONE:
	default:
		return RTK_SUCCESS;
	}

	status = usbd_ecm_intr_in_send(&event, length);
	if (status == RTK_SUCCESS) {
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
static int usbd_ecm_intr_in_send(void *data, u16 len)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usb_dev_t *dev = ecm->dev;
	usbd_ep_t *ep_intr_in = &ecm->ep_intr_in;
	int ret = -RTK_ERR_IO;

	if (!dev->is_ready) {
		return ret;
	}

	if (len > USB_CDC_ECM_INTR_IN_PACKET_SIZE) {
		len = USB_CDC_ECM_INTR_IN_PACKET_SIZE;
	}

	if (ep_intr_in->xfer_state == 0U) {
		ep_intr_in->xfer_state = 1U;

		if (dev->is_ready) {
			usb_os_memcpy((void *)ep_intr_in->xfer_buf, (const void *)data, len);
			ep_intr_in->xfer_len = len;
			ret = usbd_ep_transmit(dev, ep_intr_in);
			if (ret != RTK_SUCCESS) {
				/* The transfer never started (e.g. buffer alignment / HW error),
				 * so no XFRC interrupt will fire to clear xfer_state.  Reset it
				 * here, otherwise the INTR IN path wedges permanently: the SOF
				 * retry is gated on xfer_state == 0, and usbd_ecm_send_notification()
				 * would advance notify_state on a bogus success, so every later
				 * link-state notification is lost until re-enumeration. */
				ep_intr_in->xfer_state = 0U;
			}
		} else {
			ep_intr_in->xfer_state = 0U;
		}
	} else {
		/*TX busy*/
		ret = -RTK_ERR_BUSY;
	}

	return ret;
}

/**
 * @brief Transmit data via BULK IN endpoint
 * @param buf: Data buffer
 * @param len: Data length
 * @retval Status
 */
static int usbd_ecm_bulk_send(u8 *buf, u32 len)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep_bulk_in = &ecm->ep_bulk_in;
	usb_dev_t *dev = ecm->dev;
	int ret = -RTK_ERR_IO;

	if (!dev->is_ready) {
		return ret;
	}

	if (buf == NULL || len == 0) {
		return -RTK_ERR_BADARG;
	}

	if (ep_bulk_in->xfer_state == 0U) {
		ep_bulk_in->xfer_state = 1U;

		ep_bulk_in->xfer_buf = buf;

		/* Transmit */
		if (dev->is_ready) {
			ep_bulk_in->xfer_len = len;
			ret = usbd_ep_transmit(dev, ep_bulk_in);
			if (ret != RTK_SUCCESS) {
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
	} else {
		ret = -RTK_ERR_BUSY;
	}

	return ret;
}

/**
  * @brief  Queue a frame into the TX ring buffer
  * @note   Task context only, called by usbd_cdc_ecm_transmit() between its gate updates.
  * @param  buf: Data buffer
  * @param  len: Data length
  * @param  block: See usbd_cdc_ecm_transmit()
  * @retval RTK_SUCCESS on success, -RTK_ERR_BUSY if the frame is dropped.
  */
static int usbd_ecm_tx_enqueue(const u8 *buf, u32 len, u8 block)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	u32 tx_timeout_ms;
	int ret;

#if USBD_CDC_ECM_TX_SPEED_CHECK
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
	 *             then retries.  Bounded by usbd_cdc_ecm_config_t::bulk_tx_timeout_ms.
	 * block == 0: non-blocking; drop immediately when full. */
	if (block) {
		tx_timeout_ms = (ecm->cfg->bulk_tx_timeout_ms != 0U) ? ecm->cfg->bulk_tx_timeout_ms : USBD_CDC_ECM_DEFAULT_BULK_TX_TIMEOUT_MS;
		do {
			ret = usb_ringbuf_add_tail(&ecm->bulk_tx_rb, buf, len, 1U);
			if (ret == RTK_SUCCESS) {
				break;
			}
			if (usb_os_sema_take(ecm->bulk_tx_slot_sema, tx_timeout_ms) != RTK_SUCCESS) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "TX timeout drop(%u)\n", len);
				return -RTK_ERR_BUSY;
			}
			/* Sema may have been fired by deinit/clear_config/SET_INTERFACE alt 0
			 * rather than a real XFRC.  Test data_alt_setting, the "BULK endpoints
			 * are alive" predicate: connect_status also depends on the uplink state,
			 * so it says nothing about the endpoints.  is_closing means deinit is
			 * waiting for this call to leave. */
			if ((ecm->data_alt_setting == 0U) || (ecm->is_closing != 0U)) {
				return -RTK_ERR_BUSY;
			}
		} while (1);
	} else {
		ret = usb_ringbuf_add_tail(&ecm->bulk_tx_rb, buf, len, 1U);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "TX drop(%u)\n", len);
			return -RTK_ERR_BUSY;
		}
	}

#if USBD_CDC_ECM_STATE_TRACE_ENABLE
	ecm->dbg_tx_bytes += len;
#endif

	/* The frame is only queued: the next SOF starts it when BULK IN is idle, and each
	 * BULK IN completion chains the next one.  Starting it here as well would make this
	 * task a second consumer of the ring buffer, racing the USB ISR on another SMP core. */

#if USBD_CDC_ECM_TX_SPEED_CHECK
	usb_tx_end_time = usb_os_get_timestamp_ms();
	usb_tx_interval_time = (usb_tx_end_time - usb_tx_start_time) * RTOS_TICK_RATE_MS;

	if (usb_tx_interval_time >= 3000) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Heap %d TX %dB in %d ms, %d Kbps\n",
				 usb_os_get_free_heap_size(),
				 (u32)usb_tx_total_len, (u32)usb_tx_interval_time,
				 (u32)((usb_tx_total_len * 8 * 1000) / (usb_tx_interval_time * 1024)));
		usb_tx_start_time = usb_tx_end_time;
		usb_tx_total_len = 0;
	}
#endif

	return RTK_SUCCESS;
}

/**
 * @brief Set CDC ECM configuration
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @param config: Configuration number
 * @retval Status
 */
static int usbd_ecm_set_config(usb_dev_t *dev, u8 config)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep_intr_in = &ecm->ep_intr_in;
	usb_ep_info_t *info;
	int ret;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if (config != 1U) {
		return -RTK_ERR_BADARG;
	}

	ecm->dev = dev;

	/* Initialize INTR IN endpoint.  It is the only endpoint that belongs to the
	 * configuration itself: the communication interface has a single alternate
	 * setting (alt 0), so its notification endpoint exists as soon as the device
	 * is configured.
	 *
	 * The BULK IN/OUT endpoints are deliberately NOT initialised here.  Ref CDC
	 * ECM 1.2 3.2.2: they belong to alternate setting 1 of the data interface,
	 * whose default setting (alt 0) has bNumEndpoints = 0.  They are brought up
	 * in usbd_ecm_data_alt_start() when the host actually selects alt 1. */
	ep_intr_in->xfer_state = 0U;
	info = &ep_intr_in->info;
	info->mps = USB_CDC_ECM_INTR_IN_PACKET_SIZE;
	ret = usbd_ep_init(dev, ep_intr_in);

	/* A fresh configuration always starts from the data interface default
	 * setting (USB 2.0 9.4.7: SET_CONFIGURATION selects alt 0 for every
	 * interface), so the data path starts inactive. */
	ecm->data_alt_setting = 0U;

	/* A failure makes the core reject SET_CONFIGURATION */
	return ret;
}

/**
 * @brief Clear CDC ECM configuration
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @param config: Configuration number
 * @retval None
 */
static void usbd_ecm_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;

	UNUSED(config);

	/* Tear the data interface down first.  The helper deinitialises the BULK
	 * endpoints, drops the deferred RX length, resets the TX ring buffer and
	 * releases a producer blocked on a slot - exactly the work this function
	 * used to do inline - and is safe even when the host never selected alt 1
	 * (usbd_ep_deinit() tolerates a never-initialised endpoint). */
	usbd_ecm_data_alt_stop(dev);

	/* The notification endpoint belongs to the configuration, so it is only
	 * released here. */
	usbd_ep_deinit(dev, &ecm->ep_intr_in);

	/* The data path is gone: clear the link state and abandon any in-flight
	 * notification sequence so the next SET_INTERFACE re-reports from scratch
	 * (mirrors the detach reset in usbd_ecm_status_changed). */
	ecm->connect_status = 0;
	ecm->notify_state = USBD_ECM_NOTIFY_NONE;
	ecm->notify_retry = 0U;
}

/**
 * @brief Handle CDC ECM SETUP requests
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @param req: SETUP request
 * @retval Status
 */
static int usbd_ecm_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	int ret = RTK_SUCCESS;
	u8 req_type = req->bmRequestType & USB_REQ_TYPE_MASK;

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
			   core turns into an EP0 STALL on a non-RTK_SUCCESS return.  The alternate
			   setting is validated BEFORE any state is modified, so a rejected
			   request leaves the interface exactly as it was. */
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = -RTK_ERR_BADARG;
			} else if ((USB_HIGH_BYTE(req->wValue) != 0U) || (req->wLength != 0U)) {
				/* Ref USB 2.0 Table 9-3: the whole wValue is the alternate setting and wLength is zero */
				ret = -RTK_ERR_BADARG;
			} else {
				u8 alt = USB_LOW_BYTE(req->wValue);

				if (req->wIndex == USBD_CDC_ECM_DATA_INTERFACE_NUM) {
					/* Data interface: alt 0 (no endpoints) and alt 1 (BULK IN/OUT) only */
					if (alt > 1U) {
						ret = -RTK_ERR_BADARG;
					} else if (alt == 1U) {
						/* Bring the BULK endpoints up before reporting the link, so the
						 * host finds a working data path as soon as it sees the
						 * notification.  A failure leaves alt 0 in place and STALLs the
						 * request, rather than reporting an alternate setting whose
						 * endpoints do not work. */
						ret = usbd_ecm_data_alt_start(dev);
						if (ret == RTK_SUCCESS) {
							/* USB 2.0 9.4.10: the endpoints of the selected interface return
							 * to their default state.  A freshly initialised endpoint is
							 * already unhalted, but a repeated request for the alternate
							 * setting already in use is short-circuited inside the helper,
							 * so clear the STALL condition explicitly here. */
							usbd_ep_clear_stall(dev, &ecm->ep_bulk_in);
							usbd_ep_clear_stall(dev, &ecm->ep_bulk_out);

							/* Report the link state once when the host activates the data
							 * interface.  It is the real uplink state, not a hard-coded "up":
							 * the data path being active says nothing about the network behind
							 * it.  If the INTR IN endpoint is momentarily busy the send fails
							 * here; notify_retry lets the SOF handler re-send so the initial
							 * notification is never silently lost. */
							ecm->notify_state = USBD_ECM_NOTIFY_CONNECT;
							ecm->connect_status = ecm->uplink_status;
							if (usbd_ecm_send_notification() != RTK_SUCCESS) {
								ecm->notify_retry = 1U;
							}
						}
					} else {
						/* alt 0: the host is tearing the data path down (this is what
						 * "ifdown" does on Linux).  Drop the endpoints so the upper layer
						 * cannot keep queueing frames the host will never collect.
						 *
						 * No link-down notification is emitted on purpose: the host asked
						 * for the teardown itself, and pushing a notification into an
						 * INTR IN endpoint it no longer drains would leave that endpoint
						 * busy forever and block the next link-up report. */
						usbd_ecm_data_alt_stop(dev);
						ecm->connect_status = 0;
						ecm->notify_state = USBD_ECM_NOTIFY_NONE;
						ecm->notify_retry = 0U;
					}
				} else if (req->wIndex == USBD_CDC_ECM_COMM_INTERFACE_NUM) {
					/* Communication interface: alt 0 is the only defined setting */
					if (alt != 0U) {
						ret = -RTK_ERR_BADARG;
					} else {
						usbd_ep_clear_stall(dev, &ecm->ep_intr_in);
					}
				} else {
					/* Foreign interface: not ours to configure */
					ret = -RTK_ERR_BADARG;
				}

				if ((ret == RTK_SUCCESS) && (ecm->cb != NULL) && (ecm->cb->setup != NULL)) {
					/* Notification only: the request has already been applied above */
					ecm->cb->setup(req, NULL);
				}
			}
			break;

		case USB_REQ_GET_INTERFACE:
			/* Ref USB 2.0 9.4.4: report the alternate setting of the interface
			 * addressed by wIndex.  The two interfaces must be answered separately -
			 * the communication interface only has alt 0, so reporting the data
			 * interface's setting for it would be wrong. */
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = -RTK_ERR_BADARG;
			} else if ((req->wValue != 0U) || (req->wLength != 1U)) {
				/* Ref USB 2.0 Table 9-3: wValue is zero and wLength is one */
				ret = -RTK_ERR_BADARG;
			} else if (req->wIndex == USBD_CDC_ECM_DATA_INTERFACE_NUM) {
				ep0_in->xfer_buf[0] = ecm->data_alt_setting;
				ep0_in->xfer_len = 1U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else if (req->wIndex == USBD_CDC_ECM_COMM_INTERFACE_NUM) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_len = 1U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else {
				/* Foreign interface: request error */
				ret = -RTK_ERR_BADARG;
			}
			break;

		case USB_REQ_GET_STATUS:
			/* Ref USB 2.0 9.4.5 and Table 9-3: an interface-recipient GET_STATUS returns
			   two reserved zero bytes and wLength is two. A request naming an interface
			   this function does not own is a request error, which the device core turns
			   into an EP0 STALL on a non-RTK_SUCCESS return. */
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = -RTK_ERR_BADARG;
			} else if ((req->wValue != 0U) || (req->wLength != 2U)) {
				ret = -RTK_ERR_BADARG;
			} else if ((req->wIndex == USBD_CDC_ECM_DATA_INTERFACE_NUM) || (req->wIndex == USBD_CDC_ECM_COMM_INTERFACE_NUM)) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else {
				/* Foreign interface: request error */
				ret = -RTK_ERR_BADARG;
			}
			break;

		default:
			ret = -RTK_ERR_BADARG;
			break;
		}
		break;

	case USB_REQ_TYPE_CLASS:
		if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) != USB_REQ_RECIPIENT_INTERFACE) {
			ret = -RTK_ERR_BADARG;
			break;
		}
		/* Ref ECM 1.2 6.2: every management element request is addressed to the Communication
		   Class interface. Reject any other interface so composite dispatch can continue. */
		if (req->wIndex != USBD_CDC_ECM_COMM_INTERFACE_NUM) {
			ret = -RTK_ERR_BADARG;
			break;
		}
		if (req->wLength > 0U) {
			if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
				/* Device-to-Host with data stage.  The response length is defined by the
				 * request (Ref CDC 1.2 6.2), not by the host: scheduling wLength bytes
				 * would transmit whatever the callback did not write. */
				u16 rsp_len;

				switch (req->bRequest) {
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

				if ((rsp_len == 0U) || (ecm->cb == NULL) || (ecm->cb->setup == NULL)) {
					/* Unsupported request or nobody can produce the data stage: report a
					 * request error instead of leaving the host waiting for a reply. */
					ret = -RTK_ERR_BADARG;
				} else {
					if (rsp_len > (u16)ep0_in->xfer_buf_len) {
						rsp_len = (u16)ep0_in->xfer_buf_len;
					}
					/* The EP0 buffer is shared with descriptor and OUT traffic, so clear the
					 * response window: a callback writing fewer bytes must not leak stale
					 * control-buffer contents to the host. */
					usb_os_memset((void *)ep0_in->xfer_buf, 0, rsp_len);
					ret = ecm->cb->setup(req, ep0_in->xfer_buf);
					if (ret == RTK_SUCCESS) {
						/* Ref USB 2.0 9.2.7: never return more than wLength bytes; the core
						 * STALLs EP0 on a non-RTK_SUCCESS return, so propagate a submit failure
						 * instead of leaving the host in a control-transfer timeout. */
						ep0_in->xfer_len = (req->wLength < rsp_len) ? req->wLength : rsp_len;
						ret = usbd_ep_transmit(dev, ep0_in);
					}
				}
			} else {
				/* Host-to-Device with data stage */
				usb_os_memcpy((void *)&ecm->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
				ecm->ctrl_req_pending = 1U;
				ep0_out->xfer_len = req->wLength;
				ret = usbd_ep_receive(dev, ep0_out);
				if (ret != RTK_SUCCESS) {
					/* The data stage never started, so no EP0 OUT completion will
					 * arrive to consume the stashed request.  Invalidate it, else the
					 * next unrelated request's data stage would be interpreted as this
					 * one's payload. */
					ecm->ctrl_req_pending = 0U;
				}
			}
		} else {
			/* No data stage */
			if ((ecm->cb != NULL) && (ecm->cb->setup != NULL)) {
				ret = ecm->cb->setup(req, NULL);
			}
		}
		break;

	default:
		ret = -RTK_ERR_IO;
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
static int usbd_ecm_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, int status)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep_bulk_in = &ecm->ep_bulk_in;
	usbd_ep_t *ep_intr_in = &ecm->ep_intr_in;

	UNUSED(dev);
	UNUSED(status);

	if (ep_addr == ecm->cfg->bulk_in_addr) {
		ep_bulk_in->xfer_state = 0U;
		/* The completed frame was already dequeued (and its slot freed) in
		 * usbd_ecm_bulk_tx_start_from_rb() before the DMA started, so there is
		 * nothing to retire here - just chain the next queued frame, if any.
		 * No ring buffer internals are touched.
		 *
		 * Do not chain when the data interface has just been torn down: a
		 * completion can still surface after SET_INTERFACE alt 0 / clear_config /
		 * detach, and the next frame must not be pushed into a dead endpoint. */
		if ((ecm->data_alt_setting != 0U) && (!usb_ringbuf_is_empty(&ecm->bulk_tx_rb))) {
			usbd_ecm_bulk_tx_start_from_rb();
		}
#if USBD_CDC_ECM_STATE_TRACE_ENABLE
		ecm->dbg_bulk_in_done_cnt++;
#endif
	} else if (ep_addr == ecm->cfg->intr_in_addr) {
		ep_intr_in->xfer_state = 0U;
		/* Defer the next notification transmission to the SOF handler so all
		 * retry paths converge in one place (usbd_ecm_sof).  SOF fires every
		 * 1 ms (FS) or 125 us (HS), so the delay is negligible. */
		ecm->notify_retry = 1U;
#if USBD_CDC_ECM_STATE_TRACE_ENABLE
		ecm->dbg_intr_in_done_cnt++;
#endif
	} else {
		/* Return non-zero so composite dispatcher continues iterating. */
		return -RTK_ERR_BADARG;
	}

	return RTK_SUCCESS;
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
		if (usb_os_sema_take(ecm->rx_data_ready_sema, USBD_CDC_ECM_RX_SEMA_TAKE_TIMEOUT_MS) != RTK_SUCCESS) {
			/* timeout - re-check running flag */
			continue;
		}

		if (!ecm->rx_thread_running) {
			break;
		}

		if ((ecm->rx_msg_buf != NULL) && (ecm->rx_msg_len > 0U)) {
			usbd_ecm_bulk_receive(ecm->rx_msg_buf, ecm->rx_msg_len);
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
static int usbd_ecm_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep_bulk_out = &ecm->ep_bulk_out;

	UNUSED(dev);

	if (ep_addr != ecm->cfg->bulk_out_addr) {
		return -RTK_ERR_BADARG;
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
			/* rx_buf_free gates the hand-off, so the semaphore is never pending here */
			usb_os_sema_give(ecm->rx_data_ready_sema);

			/* Flip to the other buffer for the next OUT transfer. */
			ecm->rx_xfer_idx ^= 1U;
			ep_bulk_out->xfer_buf = ecm->rx_buf[ecm->rx_xfer_idx];
		} else {
			/* RX thread still busy. Do NOT re-arm the endpoint; the host will
			 * see NAK on the bulk-out pipe. Mark as pending so the SOF handler
			 * re trigger the endpoint as soon as the thread frees the buffer. */
			ecm->rx_pending_len = len;
			return RTK_SUCCESS;
		}
	}

	/* Do not re-arm once the host has deselected data-interface alt 1: the
	 * endpoint no longer exists. */
	if (ecm->data_alt_setting == 0U) {
		return RTK_SUCCESS;
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
static void usbd_ecm_sof(usb_dev_t *dev)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep_bulk_out = &ecm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ecm->ep_intr_in;
	u8 link_up;

	UNUSED(dev);

	/* Report a link change.  The host sees the link up only while the data interface is
	 * active and the uplink is up.  usbd_cdc_ecm_set_link_status() only writes
	 * uplink_status, so connect_status/notify_state/notify_retry have a single writer -
	 * the USB ISR - and a change requested from another core can never be overwritten. */
	link_up = ((ecm->data_alt_setting != 0U) && (ecm->uplink_status != 0U)) ? 1U : 0U;
	if (link_up != ecm->connect_status) {
		ecm->connect_status = link_up;
		ecm->notify_state = USBD_ECM_NOTIFY_CONNECT;
		ecm->notify_retry = 1U;
	}

	/* Retry a previously-failed notification when the endpoint is free. */
	if (ecm->notify_retry && ep_intr_in->xfer_state == 0U) {
		if (usbd_ecm_send_notification() == RTK_SUCCESS) {
			ecm->notify_retry = 0U;
		}
	}

	/* The BULK OUT endpoint only exists while data-interface alt 1 is selected.
	 * Without this guard a frame deferred just before a SET_INTERFACE alt 0 would
	 * make the SOF handler re-arm a de-initialised endpoint. */
	if (ecm->data_alt_setting == 0U) {
		return;
	}

	/* TX consumer: start the head frame if BULK IN is idle.  Only the USB ISR starts a
	 * BULK IN transfer - here and in the completion chain - so the ring buffer has a
	 * single consumer and no lock is needed against usbd_cdc_ecm_transmit(). */
	if ((ecm->ep_bulk_in.xfer_state == 0U) && (!usb_ringbuf_is_empty(&ecm->bulk_tx_rb))) {
		usbd_ecm_bulk_tx_start_from_rb();
	}

	if (ecm->rx_pending_len == 0U) {
		return;
	}

	/* Thread still busy - wait for the next SOF. */
	if (ecm->rx_buf_free == 0U) {
		return;
	}

	/* Buffer is free: hand off the pending frame and re-arm the endpoint. */
	ecm->rx_buf_free = 0U;
	ecm->rx_msg_buf = ecm->rx_buf[ecm->rx_xfer_idx];
	ecm->rx_msg_len = ecm->rx_pending_len;
	usb_os_sema_give(ecm->rx_data_ready_sema);

	ecm->rx_xfer_idx ^= 1U;
	ep_bulk_out->xfer_buf = ecm->rx_buf[ecm->rx_xfer_idx];
	ecm->rx_pending_len = 0U;

	if (usbd_ep_receive(ecm->dev, ep_bulk_out) != RTK_SUCCESS) {
		/* Nobody to report to from the SOF ISR: record it, the host keeps seeing NAK */
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ecm->cfg->bulk_out_addr);
	}
}

/**
 * @brief Handle EP0 data OUT phase
 * @note  This function is called within an interrupt service routine (ISR) context;
 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param dev: USB device instance
 * @retval Status
 */
static int usbd_ecm_handle_ep0_data_out(usb_dev_t *dev)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	/* Ref USB 2.0 8.5.3.1: a non-zero value makes the core stall the status stage. Default to
	   success, an application without a setup handler simply ignores class-specific requests
	   (e.g. SET_ETHERNET_PACKET_FILTER) and the data stage itself was received correctly.
	   Stalling here would make the host give up on the interface. */
	int ret = RTK_SUCCESS;

	if (ecm->ctrl_req_pending != 0U) {
		/* Consume the pending request first: a single data stage belongs to exactly one setup
		   packet, so the saved request must not be replayed by a later EP0 OUT event. */
		ecm->ctrl_req_pending = 0U;

		/* cb is released by usbd_cdc_ecm_deinit(), which may run between the setup and the
		   data stage of an H2D request, so both the structure and the handler are checked. */
		if ((ecm->cb != NULL) && (ecm->cb->setup != NULL)) {
			ret = ecm->cb->setup(&ecm->ctrl_req, ep0_out->xfer_buf);
		}
	}

	return ret;
}

/**
 * @brief Patch the runtime-assigned fields in a configuration descriptor block
 * @note   Replaces direction-only EP placeholders (USB_D2H/USB_H2D) with actual
 *         EP addresses from the class configuration, applies the configured INTR IN
 *         interval, rewrites the iMACAddress and iInterface string indices with the
 *         indices the core assigned, and rebases the Union FD interface cross-references
 *         with the current interface base, which is the standalone default unless the
 *         composite framework rebased it.
 * @param  desc: Pointer to config descriptor body (starting after config header)
 * @param  len: Length of the descriptor block
 * @param  speed: Speed the descriptor describes, selects the INTR IN interval
 * @retval None
 */
static void usbd_cdc_ecm_patch_desc(u8 *desc, u16 len, usb_speed_type_t speed)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	const usbd_cdc_ecm_config_t *cfg = ecm->cfg;
	u8 interval;

	interval = (speed == USB_SPEED_HIGH) ? cfg->intr_in_interval_hs : cfg->intr_in_interval_fs;
	if (interval == 0U) {
		interval = USBD_CDC_ECM_DEFAULT_INTR_IN_INTERVAL;
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
			desc[i + 8] = ecm->itf_str_idx;         /* iInterface */
		} else if ((dtype == USB_CDC_CS_INTERFACE) && (dlen >= 4) &&
				   (desc[i + 2] == USB_CDC_FUNC_DESC_ETHERNET_NETWORKING)) {
			/* Ethernet Networking FD: iMACAddress at offset 3. Writes the standalone
			 * default (same as the static template) unless the composite framework
			 * rebased the class string window. */
			desc[i + 3] = ecm->mac_str_idx;
		} else if ((dtype == USB_CDC_CS_INTERFACE) && (dlen >= 5) &&
				   (desc[i + 2] == USB_CDC_FUNC_DESC_UNION)) {
			/* Union FD: bControlInterface at offset 3, bSubordinateInterface0 at offset 4
			 * (Ref CDC 1.2 5.2.3.8). The composite framework only rebases the standard
			 * Interface and IAD descriptors, so this cross-reference is ours to fix up. */
			desc[i + 3] = (u8)(ecm->if_base + USBD_CDC_ECM_COMM_INTERFACE_NUM);
			desc[i + 4] = (u8)(ecm->if_base + USBD_CDC_ECM_DATA_INTERFACE_NUM);
		}
		i += dlen;
	}
}

/**
 * @brief Get USB descriptor
 */
static u16 usbd_ecm_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len)
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
		desc = usbd_cdc_ecm_dev_desc;
		len = sizeof(usbd_cdc_ecm_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
#ifndef CONFIG_SUPPORT_USB_FS_ONLY
		if (speed == USB_SPEED_HIGH) {
			desc = usbd_cdc_ecm_hs_config_desc;
			len = sizeof(usbd_cdc_ecm_hs_config_desc);
		} else
#endif
		{
			desc = usbd_cdc_ecm_fs_config_desc;
			len = sizeof(usbd_cdc_ecm_fs_config_desc);
		}
		is_cfg = 1;
		break;

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		desc = usbd_cdc_ecm_device_qualifier_desc;
		len = sizeof(usbd_cdc_ecm_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		/* Ref USB 2.0 9.6.4: describes the configuration at the speed the device is not
		   running at, so its bInterval is patched for that speed, not the current one */
		desc = (speed == USB_SPEED_HIGH) ?
			   usbd_cdc_ecm_fs_config_desc : usbd_cdc_ecm_hs_config_desc;
		len = (speed == USB_SPEED_HIGH) ?
			  sizeof(usbd_cdc_ecm_fs_config_desc) : sizeof(usbd_cdc_ecm_hs_config_desc);
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
		usbd_cdc_ecm_patch_desc(buf + USB_LEN_CFG_DESC, len - USB_LEN_CFG_DESC, desc_speed);
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
static void usbd_ecm_set_interface_base(u8 base)
{
	usbd_cdc_ecm_dev.if_base = base;
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
static void usbd_ecm_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;

	UNUSED(dev);

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		/* Clear link state immediately so upper layers see link_is_up == 0
		 * without waiting for usbd_cdc_ecm_deinit(). */
		ecm->connect_status = 0;
		ecm->notify_state = USBD_ECM_NOTIFY_NONE;
		ecm->notify_retry = 0U;

		ecm->rx_pending_len = 0U;

		/* The bus is gone, so the endpoints are gone with it: fall back to the
		 * default alternate setting.  This is what stops a queued frame from being
		 * pushed into a dead endpoint after detach, and it matches what the host
		 * will assume after the next enumeration (USB 2.0 9.4.7).
		 * Written directly rather than through usbd_ecm_data_alt_stop(): the TX
		 * ring reset and the endpoint deinit are done by clear_config()/deinit(),
		 * and this handler must stay as short as possible. */
		ecm->data_alt_setting = 0U;
	}

	if ((ecm->cb != NULL) && (ecm->cb->status_changed != NULL)) {
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
static void usbd_ecm_trace_thread(void *param)
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
					 ecm->notify_state, ecm->notify_retry, ecm->data_alt_setting,
					 ep_bulk_in->xfer_state, ep_bulk_out->xfer_state, ep_intr_in->xfer_state,
					 ecm->rx_buf_free, ecm->rx_pending_len, ecm->rx_xfer_idx,
					 usb_ringbuf_get_count(&ecm->bulk_tx_rb));
			/* Line 2 - monotonic per-path counters; a value frozen across intervals
			 * pinpoints a wedged path:
			 *   tx   = usbd_cdc_ecm_transmit() calls   in   = BULK IN  completion ISRs
			 *   out  = BULK OUT completion ISRs        intr = INTR IN  completion ISRs
			 *   rxd  = frames delivered to upper layer rxl  = RX thread heartbeat
			 *                                                 (frozen => RX thread wedged)
			 *   drp  = dequeued frames whose BULK IN submit failed (lost) */
			RTK_LOGS(TAG, RTK_LOG_INFO,
					 "cnt tx%d in%d out%d intr%d/rxd%d rxl%d drp%d/Heap %d\n",
					 ecm->dbg_tx_cnt, ecm->dbg_bulk_in_done_cnt, ecm->dbg_bulk_out_done_cnt,
					 ecm->dbg_intr_in_done_cnt, ecm->dbg_rx_deliver_cnt, ecm->dbg_rx_loop_cnt,
					 ecm->dbg_tx_drop_cnt, usb_os_get_free_heap_size());
		}

		usb_os_sleep_ms(USBD_CDC_ECM_TRACE_INTERVAL_MS);
	}

	ecm->trace_task = NULL;
	rtos_task_delete(NULL);
}

/**
 * @brief Create the ECM state-trace thread (idempotent).
 */
static void usbd_ecm_trace_task_init(void)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	int status;

	if (ecm->trace_task_running) {
		return;
	}

	ecm->trace_task_running = 1;
	status = rtos_task_create(&ecm->trace_task, "usbd_cdc_ecm_trace_thread", usbd_ecm_trace_thread, NULL,
							  USBD_CDC_ECM_TRACE_THREAD_STACK_SIZE, USBD_CDC_ECM_TRACE_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create ECM trace task fail\n");
		ecm->trace_task_running = 0;
	}
}

/**
 * @brief Stop the ECM state-trace thread and wait for it to exit.
 */
static void usbd_ecm_trace_task_deinit(void)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	ecm->trace_task_running = 0;
	while (ecm->trace_task != NULL) {
		usb_os_sleep_ms(10);
	}
}
#endif /* USBD_CDC_ECM_STATE_TRACE_ENABLE */

/**
 * @brief Initialize CDC ECM class
 * @param cb: User callbacks
 * @param cfg: Class configuration
 * @retval Status
 */
static int usbd_cdc_ecm_private_init(const usbd_cdc_ecm_cb_t *cb, const usbd_cdc_ecm_config_t *cfg)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;

	usbd_ep_t *ep_bulk_in = &ecm->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &ecm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ecm->ep_intr_in;
	usb_ep_info_t *info;
	u8 tx_depth;
	int ret = RTK_SUCCESS;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return -RTK_ERR_BADARG;
	}

	if (cfg == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cfg\n");
		return -RTK_ERR_BADARG;
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
		return -RTK_ERR_BADARG;
	}

	usb_os_memset((void *)ecm, 0, sizeof(usbd_cdc_ecm_dev_t));

	/* Published before anything that reads it, so the descriptor patch path and the TX path
	   always see the configuration this init was given. */
	ecm->cfg = cfg;

	/* No H2D class request is waiting for its data stage yet (the memset above already
	   cleared it; kept explicit so the invariant is visible at init). */
	ecm->ctrl_req_pending = 0U;
	/* The core owns the string table, so the same call works in standalone and composite
	   mode. mac_str is registered by pointer, so it may still be empty here: set_mac()
	   formats it below, before enumeration can request it. */
	ecm->mac_str_idx = usbd_add_string(ecm->mac_str);
	ecm->itf_str_idx = usbd_add_string(cfg->itf_str);
	/* Standalone default; the composite framework rebases it via set_interface_base() */
	ecm->if_base = 0;

	tx_depth = (cfg->bulk_tx_queue_depth != 0U) ? cfg->bulk_tx_queue_depth : USBD_CDC_ECM_DEFAULT_BULK_TX_QUEUE_DEPTH;
	if (usb_ringbuf_manager_init(&ecm->bulk_tx_rb, tx_depth,
								 USBD_CDC_ECM_BULK_BUF_MAX_SIZE, 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init bulk_tx ring buffer fail\n");
		return -RTK_ERR_NOMEM;
	}

	/* Private DMA source buffer for BULK IN.  Each frame is copied here from the
	 * ring buffer head before transmission, so DMA never reads node memory. */
	ecm->bulk_tx_dma_buf = (u8 *)usb_os_malloc(USBD_CDC_ECM_BULK_BUF_MAX_SIZE);
	if (ecm->bulk_tx_dma_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc bulk_tx dma buf fail\n");
		ret = -RTK_ERR_NOMEM;
		goto exit;
	}

	/* BULK IN transmits from ecm->bulk_tx_dma_buf (filled per-frame at TX time) */
	info = &ep_bulk_in->info;
	info->addr = cfg->bulk_in_addr;
	info->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_in->xfer_buf_len = 0;
	ep_bulk_in->xfer_buf = NULL;

	/* Allocate BULK OUT ping-pong buffers */
	info = &ep_bulk_out->info;
	info->addr = cfg->bulk_out_addr;
	info->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_out->xfer_buf_len = USBD_CDC_ECM_BULK_BUF_MAX_SIZE;
	ep_bulk_out->xfer_len = ep_bulk_out->xfer_buf_len;

	ecm->rx_buf[0] = (u8 *)usb_os_malloc(USBD_CDC_ECM_BULK_BUF_MAX_SIZE);
	if (ecm->rx_buf[0] == NULL) {
		ret = -RTK_ERR_NOMEM;
		goto exit;
	}

	ecm->rx_buf[1] = (u8 *)usb_os_malloc(USBD_CDC_ECM_BULK_BUF_MAX_SIZE);
	if (ecm->rx_buf[1] == NULL) {
		ret = -RTK_ERR_NOMEM;
		goto cleanup_rx_buf0;
	}

	ecm->rx_xfer_idx = 0U;
	ep_bulk_out->xfer_buf = ecm->rx_buf[ecm->rx_xfer_idx];

	/* bulk_tx_slot_sema: ISR -> tcpip, given each time a TX ring buffer slot is freed. */
	if (usb_os_sema_create(&ecm->bulk_tx_slot_sema) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create TX slot sema fail\n");
		ret = -RTK_ERR_NOMEM;
		goto cleanup_rx_buf1;
	}

	/* rx_data_ready_sema: ISR -> thread, given when a buffer is filled. */
	if (usb_os_sema_create(&ecm->rx_data_ready_sema) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create RX data_ready sema fail\n");
		ret = -RTK_ERR_NOMEM;
		goto cleanup_tx_slot_sema;
	}

	ecm->rx_buf_free    = 1U;
	ecm->rx_pending_len = 0U;

	/* Start RX delivery thread */
	ecm->rx_thread_running = 1;
	ret = rtos_task_create(&ecm->rx_task, "usbd_cdc_ecm_rx_thread", usbd_cdc_ecm_rx_thread, NULL,
						   (cfg->rx_task_stack_size != 0U) ? cfg->rx_task_stack_size : USBD_CDC_ECM_DEFAULT_RX_TASK_STACK_SIZE,
						   (cfg->rx_task_priority != 0U) ? cfg->rx_task_priority : USBD_CDC_ECM_DEFAULT_RX_TASK_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create RX thread fail\n");
		ecm->rx_thread_running = 0;
		ret = -RTK_ERR_IO;
		goto cleanup_rx_data_ready_sema;
	}

	/* Allocate INTR IN buffer */
	info = &ep_intr_in->info;
	info->addr = cfg->intr_in_addr;
	info->type = USB_CH_EP_TYPE_INTR;
	ep_intr_in->xfer_buf_len = USB_CDC_ECM_INTR_IN_PACKET_SIZE;
	ep_intr_in->xfer_buf = (u8 *)usb_os_malloc(ep_intr_in->xfer_buf_len);
	if (ep_intr_in->xfer_buf == NULL) {
		ret = -RTK_ERR_NOMEM;
		goto cleanup_rx_thread;
	}

	usbd_ecm_set_mac((cfg->mac != NULL) ? cfg->mac : usbd_ecm_default_mac);

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "User init fail: %d\n", ret);
			goto cleanup_intr_in;
		}
	}

	/* Initialize user callbacks */
	ecm->cb = cb;

	/* Register CDC ECM class driver */

#if USBD_CDC_ECM_STATE_TRACE_ENABLE
	usbd_ecm_trace_task_init();
#endif

	/* Every TX resource exists from here on: let usbd_cdc_ecm_transmit() in.  The memset
	   above already cleared it, kept explicit as the counterpart of deinit. */
	ecm->is_closing = 0U;

	return RTK_SUCCESS;

cleanup_intr_in:
	usb_os_mfree((void *)ep_intr_in->xfer_buf);
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
	usb_os_mfree((void *)ecm->rx_buf[1]);
	ecm->rx_buf[1] = NULL;

cleanup_rx_buf0:
	usb_os_mfree((void *)ecm->rx_buf[0]);
	ecm->rx_buf[0] = NULL;
	ep_bulk_out->xfer_buf = NULL;

exit:
	usb_os_mfree((void *)ecm->bulk_tx_dma_buf);
	ecm->bulk_tx_dma_buf = NULL;
	usb_ringbuf_manager_deinit(&ecm->bulk_tx_rb);

	return ret;
}

/* Exported functions --------------------------------------------------------*/

int usbd_cdc_ecm_init(const usbd_cdc_ecm_cb_t *cb, const usbd_cdc_ecm_config_t *cfg)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	int ret;

	ret = usbd_cdc_ecm_private_init(cb, cfg);
	if (ret == RTK_SUCCESS) {
		ecm->from_composite = 0;
		ret = usbd_register_class(&usbd_cdc_ecm_driver);
		if (ret != RTK_SUCCESS) {
			/* private_init completed, so deinit is its exact reverse */
			usbd_cdc_ecm_deinit();
		}
	}
	return ret;
}

#ifdef CONFIG_USBD_COMPOSITE
int usbd_composite_cdc_ecm_init(const usbd_cdc_ecm_cb_t *cb, const usbd_cdc_ecm_config_t *cfg)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	int ret;

	ret = usbd_cdc_ecm_private_init(cb, cfg);
	if (ret == RTK_SUCCESS) {
		ecm->from_composite = 1;
		ret = usbd_composite_register_driver(&usbd_cdc_ecm_driver);
		if (ret != RTK_SUCCESS) {
			/* private_init completed, so deinit is its exact reverse. from_composite is
			   already 1, so the unregister inside is a no-op for an unregistered driver. */
			usbd_cdc_ecm_deinit();
		}
	}
	return ret;
}
#endif

/**
 * @brief Deinitialize CDC ECM class
 * @retval None
 */
void usbd_cdc_ecm_deinit(void)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usbd_ep_t *ep_bulk_in = &ecm->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &ecm->ep_bulk_out;
	usbd_ep_t *ep_intr_in = &ecm->ep_intr_in;
	u32 wait_cnt = 0U;
	int ret;
	u8 i;

	/* Leave the core before anything is torn down. Composite: refused until
	   usbd_composite_deinit() has blocked the ISR and cleared the configuration of every
	   function. Standalone: refused in interrupt context. Either way nothing is released when
	   refused, and once accepted no class callback runs any more and every endpoint of this
	   class is deinitialized: ref USB 2.0 9.4.7, the transfers still pending are aborted. */
#ifdef CONFIG_USBD_COMPOSITE
	if (ecm->from_composite != 0U) {
		ret = usbd_composite_unregister_driver(&usbd_cdc_ecm_driver);
	} else
#endif
	{
		ret = usbd_unregister_class();
	}
	if (ret != RTK_SUCCESS) {
		return;
	}

	/* No completion arrives any more to clear them */
	ep_bulk_in->xfer_state = 0U;
	ep_intr_in->xfer_state = 0U;

	/* Close the TX gate: from here on usbd_cdc_ecm_transmit() rejects new callers, and the
	 * ones already inside are waited for below before their resources go away. */
	usb_os_enter_critical();
	ecm->is_closing = 1U;
	usb_os_exit_critical();

	ecm->connect_status = 0;
	/* Fall back to the default alternate setting so every TX/RX submission path
	 * bails out before anything below is torn down. */
	ecm->data_alt_setting = 0U;

#if USBD_CDC_ECM_STATE_TRACE_ENABLE
	/* Stop the trace thread first so it does not read state being torn down. */
	usbd_ecm_trace_task_deinit();
#endif

	/* Wait for every task still inside usbd_cdc_ecm_transmit() to leave.  A caller blocked
	 * on a TX slot is woken on each round and leaves on is_closing; deleting the sema while
	 * it still waits on it, or freeing the ring it is writing to, would be a use after free,
	 * so the wait is not bounded and a starved caller is reported once per
	 * USBD_CDC_ECM_TX_EXIT_WARN_MS instead. */
	wait_cnt = 0U;
	while (ecm->xfer_users != 0U) {
		usb_os_sema_give(ecm->bulk_tx_slot_sema);
		usb_os_sleep_ms(1U);
		wait_cnt++;
		if (wait_cnt >= USBD_CDC_ECM_TX_EXIT_WARN_MS) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Wait TX exit\n");
			wait_cnt = 0U;
		}
	}

	/* No more USB ISR runs against this class.  Free the TX ring buffer and the
	 * private BULK IN DMA buffer. */
	usb_ringbuf_manager_deinit(&ecm->bulk_tx_rb);
	usb_os_mfree((void *)ecm->bulk_tx_dma_buf);
	ecm->bulk_tx_dma_buf = NULL;

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
	/* Unregistered above: no class callback can run afterwards, so dropping the pending
	 * control request here cannot race an EP0 OUT completion in ISR context. */
	ecm->ctrl_req_pending = 0U;

	/* Call user deinit */
	if ((ecm->cb != NULL) && (ecm->cb->deinit != NULL) && (ecm->cb->deinit() != RTK_SUCCESS)) {
		/* Teardown goes on regardless, the class resources are released below */
		RTK_LOGS(TAG, RTK_LOG_WARN, "CB deinit fail\n");
	}

	/* Free buffers */
	usb_os_mfree((void *)ep_intr_in->xfer_buf);
	ep_intr_in->xfer_buf = NULL;

	/* Free RX ping-pong buffers. */
	for (i = 0; i < USBD_CDC_ECM_RX_BUF_NUM; i++) {
		usb_os_mfree((void *)ecm->rx_buf[i]);
		ecm->rx_buf[i] = NULL;
	}
	ep_bulk_out->xfer_buf = NULL;

	/* Clear user callback pointer */
	ecm->cb = NULL;
}

/**
  * @brief  Start to transmit data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @param  block: When non-zero, block up to usbd_cdc_ecm_config_t::bulk_tx_timeout_ms ms
  *                waiting for a ring buffer slot before dropping.  Zero means
  *                non-blocking - drop immediately when the ring buffer is full.
  * @note   Task context only: the ring buffer has a single producer and a blocking call
  *         waits on a semaphore, so a call in interrupt context is rejected.
  * @retval RTK_SUCCESS on success, -RTK_ERR_BUSY if the ring buffer is full (frame dropped).
  */
int usbd_cdc_ecm_transmit(const u8 *buf, u32 len, u8 block)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	int ret;

	if (usb_os_is_in_interrupt() != 0U) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, 0U);
		return -RTK_ERR_BADARG;
	}

	/* Reject invalid frames up front.  A NULL buffer or zero length would be
	 * accepted by usb_ringbuf_add_tail() but always rejected by usbd_ecm_bulk_send()
	 * (it returns on buf==NULL / len==0 without starting a transfer), leaving the
	 * node stuck at the ring buffer head forever - head only advances on XFRC,
	 * which never fires for an un-started transfer. */
	if ((buf == NULL) || (len == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "TX bad param (len %u)\n", len);
		return -RTK_ERR_BADARG;
	}

	/* Ref CDC ECM 1.2 5.4: wMaxSegmentSize is the longest frame the host is told to expect,
	 * so never send more.  The ring node is larger only because the shared RX buffer size is
	 * rounded up to whole packets. */
	if (len > USB_CDC_ECM_MAX_SEGMENT_SIZE) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "TX frame too large: %u\n", len);
		return -RTK_ERR_BADARG;
	}

	/* The BULK IN endpoint only exists while the host has selected data-interface
	 * alt 1 (Ref CDC ECM 1.2 3.2.2).  Reject early rather than queueing into a
	 * ring buffer nothing can drain: without this, a host that deselected alt 1
	 * (Linux "ifdown") would let the upper layer keep filling the ring - and with
	 * block != 0 it would stall the caller for the full timeout on every frame. */
	usb_os_enter_critical();
	if ((ecm->is_closing != 0U) || (ecm->data_alt_setting == 0U)) {
		usb_os_exit_critical();
		return -RTK_ERR_BUSY;
	}
	/* Counted under the same lock that deinit closes the gate with, so deinit either sees
	 * this caller or this caller sees is_closing: never both missed */
	ecm->xfer_users++;
	usb_os_exit_critical();

	ret = usbd_ecm_tx_enqueue(buf, len, block);

	usb_os_enter_critical();
	ecm->xfer_users--;
	usb_os_exit_critical();

	return ret;
}

/**
  * @brief  Get current network link state (uplink status).
  * @retval 1: Network link is up (connected to uplink).
  *         0: Network link is down (disconnected from uplink).
  */
int usbd_cdc_ecm_get_link_status(void)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;

	return ecm->connect_status;
}

/**
  * @brief  Report the upper-layer network link state to the host.
  * @note   Only records the uplink state.  The SOF handler (ISR context) compares the
  *         resulting host-visible state with the one last reported and sends
  *         NETWORK_CONNECTION on a change, so repeated calls with the same value are
  *         no-ops and every notification field keeps the USB ISR as its only writer.
  *         Callable from any task context.
  * @param  link_up: Non-zero when the network link is up, zero when it is down.
  * @retval RTK_SUCCESS
  */
int usbd_cdc_ecm_set_link_status(u8 link_up)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;

	ecm->uplink_status = (link_up != 0U) ? 1U : 0U;

	return RTK_SUCCESS;
}

__attribute__((weak))
int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbd_cdc_ecm_transmit(buf, len, block);
}
