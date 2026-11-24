/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_rndis.h"
#ifdef CONFIG_USBD_COMPOSITE
#include "usbd_composite.h"
#endif

/* Private defines -----------------------------------------------------------*/

/* One REMOTE_NDIS_PACKET_MSG per transfer: the 44-byte header and the largest frame */
#define USBD_RNDIS_TX_BUF_SIZE                        USB_RNDIS_MAX_PACKET_MSG_SIZE

/* RX window: the largest packet message plus the 1-byte pad a host may append instead of a ZLP
 * (Ref RNDIS USB 3.1), rounded up to whole HS packets */
#define USBD_RNDIS_RX_BUF_SIZE                        ((USB_RNDIS_MAX_PACKET_MSG_SIZE + 1U + USB_BULK_HS_MAX_MPS - 1U) / USB_BULK_HS_MAX_MPS * USB_BULK_HS_MAX_MPS)

/* Largest transfer the device accepts, reported in INITIALIZE_CMPLT */
#define USBD_RNDIS_MAX_TRANSFER_SIZE                  USB_RNDIS_MAX_PACKET_MSG_SIZE

/* Shortest encapsulated command: the message header */
#define USBD_RNDIS_MIN_CMD_LEN                        ((u16)sizeof(usb_rndis_msg_hdr_t))

/* INTR IN wMaxPacketSize: the 8-byte RESPONSE_AVAILABLE notification */
#define USBD_RNDIS_INTR_IN_PACKET_SIZE                USB_RNDIS_NOTIFY_SIZE

/* OID_GEN_VENDOR_ID: Realtek IEEE OUI in the low 3 bytes, vendor NIC code 0 */
#define USBD_RNDIS_VENDOR_ID                          0x00E04C00U

/* OID_GEN_LINK_SPEED is reported in units of 100 bit/s */
#define USBD_RNDIS_LINK_SPEED_UNIT                    100U

/* Poll period (ms) of the RX thread's blocking wait: bounds how long the thread takes to
 * observe rx_thread_running == 0 at teardown, not a functional parameter. */
#define USBD_RNDIS_RX_SEMA_TAKE_TIMEOUT_MS            100U

/* Period of the warning deinit logs while a task does not leave usbd_rndis_transmit() */
#define USBD_RNDIS_TX_EXIT_WARN_MS                    5000U

/* Private types -------------------------------------------------------------*/

/**
 * @brief RNDIS device state, Ref RNDIS 1.0 "Remote NDIS Device States"
 */
typedef enum {
	USBD_RNDIS_STATE_UNINITIALIZED = 0U,   /**< No INITIALIZE yet, or HALT received */
	USBD_RNDIS_STATE_INITIALIZED,          /**< INITIALIZE done, packet filter still 0 */
	USBD_RNDIS_STATE_DATA_INITIALIZED,     /**< Non-zero packet filter: data may flow */
} usbd_rndis_state_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_rndis_set_config(usb_dev_t *dev, u8 config);
static void usbd_rndis_clear_config(usb_dev_t *dev, u8 config);
static int usbd_rndis_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_rndis_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
static int usbd_rndis_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_rndis_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, int status);
static int usbd_rndis_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static void usbd_rndis_sof(usb_dev_t *dev);
static void usbd_rndis_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
#ifdef CONFIG_USBD_COMPOSITE
static void usbd_rndis_set_interface_base(u8 base);
#endif
static void usbd_rndis_data_stop(void);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "RNDIS";
static const u8 usbd_rndis_default_mac[USB_RNDIS_MAC_ADDR_LEN] = {0x02, 0xE0, 0x4C, 0x87, 0x00, 0x01};
static const char usbd_rndis_default_vendor_desc[] = "Realtek RNDIS";

/* OIDs answered by QUERY, reported by OID_GEN_SUPPORTED_LIST */
static const u32 usbd_rndis_supported_oids[] = {
	USB_RNDIS_OID_GEN_SUPPORTED_LIST,
	USB_RNDIS_OID_GEN_HARDWARE_STATUS,
	USB_RNDIS_OID_GEN_MEDIA_SUPPORTED,
	USB_RNDIS_OID_GEN_MEDIA_IN_USE,
	USB_RNDIS_OID_GEN_MAXIMUM_FRAME_SIZE,
	USB_RNDIS_OID_GEN_LINK_SPEED,
	USB_RNDIS_OID_GEN_TRANSMIT_BLOCK_SIZE,
	USB_RNDIS_OID_GEN_RECEIVE_BLOCK_SIZE,
	USB_RNDIS_OID_GEN_VENDOR_ID,
	USB_RNDIS_OID_GEN_VENDOR_DESCRIPTION,
	USB_RNDIS_OID_GEN_CURRENT_PACKET_FILTER,
	USB_RNDIS_OID_GEN_MAXIMUM_TOTAL_SIZE,
	USB_RNDIS_OID_GEN_MEDIA_CONNECT_STATUS,
	USB_RNDIS_OID_GEN_PHYSICAL_MEDIUM,
	USB_RNDIS_OID_GEN_XMIT_OK,
	USB_RNDIS_OID_GEN_RCV_OK,
	USB_RNDIS_OID_GEN_XMIT_ERROR,
	USB_RNDIS_OID_GEN_RCV_ERROR,
	USB_RNDIS_OID_GEN_RCV_NO_BUFFER,
	USB_RNDIS_OID_802_3_PERMANENT_ADDRESS,
	USB_RNDIS_OID_802_3_CURRENT_ADDRESS,
	USB_RNDIS_OID_802_3_MULTICAST_LIST,
	USB_RNDIS_OID_802_3_MAXIMUM_LIST_SIZE,
	USB_RNDIS_OID_802_3_RCV_ERROR_ALIGNMENT,
	USB_RNDIS_OID_802_3_XMIT_ONE_COLLISION,
	USB_RNDIS_OID_802_3_XMIT_MORE_COLLISIONS,
};

/* USB Standard Device Descriptor */
static const u8 usbd_rndis_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,                               /* bLength */
	USB_DESC_TYPE_DEVICE,                           /* bDescriptorType */
	0x00, 0x02,                                     /* bcdUSB: 2.0 */
	0xEF,                                           /* bDeviceClass: MISC, the configuration carries an IAD (Ref USB IAD ECN 1.0) */
	0x02,                                           /* bDeviceSubClass: Common Class */
	0x01,                                           /* bDeviceProtocol: IAD */
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
static const u8 usbd_rndis_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
	USB_LEN_DEV_QUALIFIER_DESC,                     /* bLength */
	USB_DESC_TYPE_DEVICE_QUALIFIER,                 /* bDescriptorType */
	0x00, 0x02,                                     /* bcdUSB: 2.0 */
	0xEF,                                           /* bDeviceClass: same as the device descriptor (Ref USB 2.0 9.6.2) */
	0x02,                                           /* bDeviceSubClass */
	0x01,                                           /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize0 */
	0x01,                                           /* bNumConfigurations */
	0x00,                                           /* Reserved */
};
#endif

/* USB RNDIS Configuration Descriptor, Ref RNDIS USB 2.1: a CDC ACM vendor style communication
   interface with one INTR IN endpoint, and a data interface with a BULK pair in its only setting.
   The BULK wMaxPacketSize is patched for the speed it describes, refer to usbd_rndis_patch_desc() */
static const u8 usbd_rndis_config_desc[] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	0x00, 0x00,                                     /* wTotalLength (calculated later) */
	0x02,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
	0x80,                                           /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,                                           /* bMaxPower: 100mA */

	/* IAD for RNDIS (Communication + Data) */
	USB_LEN_IAD_DESC,                               /* bLength */
	USB_DESC_TYPE_IAD,                              /* bDescriptorType */
	0x00,                                           /* bFirstInterface (patched by composite) */
	0x02,                                           /* bInterfaceCount */
	USB_RNDIS_WIRELESS_CLASS_CODE,                  /* bFunctionClass: Wireless Controller */
	USB_RNDIS_WIRELESS_SUBCLASS,                    /* bFunctionSubClass: RF Controller */
	USB_RNDIS_WIRELESS_PROTOCOL,                    /* bFunctionProtocol: Remote NDIS */
	0x00,                                           /* iFunction */

	/* Communication Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_RNDIS_COMM_INTERFACE_NUM,                  /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x01,                                           /* bNumEndpoints */
	USB_RNDIS_WIRELESS_CLASS_CODE,                  /* bInterfaceClass: Wireless Controller */
	USB_RNDIS_WIRELESS_SUBCLASS,                    /* bInterfaceSubClass: RF Controller */
	USB_RNDIS_WIRELESS_PROTOCOL,                    /* bInterfaceProtocol: Remote NDIS */
	0x00,                                           /* iInterface, runtime patched */

	/* CDC Header Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	USB_CDC_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_FUNC_DESC_HEADER,                       /* bDescriptorSubtype */
	0x10, 0x01,                                     /* bcdCDC: 1.10 */

	/* CDC Call Management Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	USB_CDC_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_FUNC_DESC_CALL_MGMT,                    /* bDescriptorSubtype */
	0x00,                                           /* bmCapabilities: no call management */
	USBD_RNDIS_DATA_INTERFACE_NUM,                  /* bDataInterface (rebased at runtime) */

	/* CDC Abstract Control Management Functional Descriptor */
	0x04,                                           /* bFunctionLength */
	USB_CDC_CS_INTERFACE,                           /* bDescriptorType */
	USB_RNDIS_CDC_FUNC_DESC_ACM,                    /* bDescriptorSubtype */
	0x00,                                           /* bmCapabilities: none */

	/* CDC Union Functional Descriptor */
	0x05,                                           /* bFunctionLength */
	USB_CDC_CS_INTERFACE,                           /* bDescriptorType */
	USB_CDC_FUNC_DESC_UNION,                        /* bDescriptorSubtype */
	USBD_RNDIS_COMM_INTERFACE_NUM,                  /* bControlInterface (rebased at runtime) */
	USBD_RNDIS_DATA_INTERFACE_NUM,                  /* bSubordinateInterface0 (rebased at runtime) */

	/* INTR IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_D2H,                                        /* bEndpointAddress: dir IN (placeholder) */
	USB_CH_EP_TYPE_INTR,                            /* bmAttributes */
	USB_LOW_BYTE(USBD_RNDIS_INTR_IN_PACKET_SIZE),   /* wMaxPacketSize */
	USB_HIGH_BYTE(USBD_RNDIS_INTR_IN_PACKET_SIZE),
	USBD_RNDIS_DEFAULT_INTR_IN_INTERVAL_HS,         /* bInterval (patched at runtime) */

	/* Data Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_RNDIS_DATA_INTERFACE_NUM,                  /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x02,                                           /* bNumEndpoints */
	USB_CDC_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	USB_CDC_SUBCLASS_RESERVED,                      /* bInterfaceSubClass */
	USB_CDC_DATA_PROTOCOL_NO_CLASS_SPECIFIC,        /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_D2H,                                        /* bEndpointAddress: dir IN (placeholder) */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes */
	USB_LOW_BYTE(USB_BULK_HS_MAX_MPS),              /* wMaxPacketSize (patched at runtime) */
	USB_HIGH_BYTE(USB_BULK_HS_MAX_MPS),
	0x00,                                           /* bInterval */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USB_H2D,                                        /* bEndpointAddress: dir OUT (placeholder) */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes */
	USB_LOW_BYTE(USB_BULK_HS_MAX_MPS),              /* wMaxPacketSize (patched at runtime) */
	USB_HIGH_BYTE(USB_BULK_HS_MAX_MPS),
	0x00                                            /* bInterval */
};

/* RNDIS Class Driver */
static const usbd_class_driver_t usbd_rndis_driver = {
	.get_descriptor = usbd_rndis_get_descriptor,
	.set_config = usbd_rndis_set_config,
	.clear_config = usbd_rndis_clear_config,
	.setup = usbd_rndis_setup,
	.ep0_data_out = usbd_rndis_handle_ep0_data_out,
	.ep_data_in = usbd_rndis_handle_ep_data_in,
	.ep_data_out = usbd_rndis_handle_ep_data_out,
	.sof = usbd_rndis_sof,
	.status_changed = usbd_rndis_status_changed,
#ifdef CONFIG_USBD_COMPOSITE
	.set_interface_base = usbd_rndis_set_interface_base,
#endif
};

/* RNDIS Device Instance */
static usbd_rndis_dev_t usbd_rndis_dev;

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Tell whether data may flow: endpoints up and a non-zero packet filter set
 * @retval 1 if data may flow, 0 otherwise
 */
static inline u8 usbd_rndis_data_on(void)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;

	return ((rndis->is_configured != 0U) && (rndis->rndis_state == USBD_RNDIS_STATE_DATA_INITIALIZED)) ? 1U : 0U;
}

/**
 * @brief  Send the RESPONSE_AVAILABLE notification if one is pending and INTR IN is idle
 * @note   ISR context only (SETUP, EP0 OUT, SOF and INTR IN completion), which never run
 *         concurrently, so the INTR IN state needs no lock.
 * @retval None
 */
static void usbd_rndis_notify(void)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	usbd_ep_t *ep_intr_in = &rndis->ep_intr_in;
	usb_dev_t *dev = rndis->dev;
	u8 *buf = ep_intr_in->xfer_buf;
	int ret;

	if ((rndis->notify_pending == 0U) || (ep_intr_in->xfer_state != 0U) || (rndis->is_configured == 0U) ||
		(dev == NULL) || (dev->is_ready == 0U)) {
		return;
	}

	/* Ref RNDIS USB 2.2.3: two little endian 32-bit words, RESPONSE_AVAILABLE then 0 */
	usb_os_memset((void *)buf, 0, USB_RNDIS_NOTIFY_SIZE);
	buf[0] = (u8)USB_RNDIS_NOTIFY_RESPONSE_AVAILABLE;
	ep_intr_in->xfer_len = USB_RNDIS_NOTIFY_SIZE;
	ep_intr_in->xfer_state = 1U;
	ret = usbd_ep_transmit(dev, ep_intr_in);
	if (ret == RTK_SUCCESS) {
		rndis->notify_pending = 0U;
	} else {
		/* No completion will clear it: release now, the next SOF retries */
		ep_intr_in->xfer_state = 0U;
	}
}

/**
 * @brief  Answer a QUERY for one OID
 * @param  oid: Object identifier
 * @param  info: Destination of the information buffer
 * @param  cap: Capacity of info in bytes
 * @retval Length of the information buffer, -1 if the OID is not supported
 */
static int usbd_rndis_query_oid(u32 oid, u8 *info, u32 cap)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	const char *desc;
	u32 val;
	u32 len;

	switch (oid) {
	case USB_RNDIS_OID_GEN_SUPPORTED_LIST:
		len = sizeof(usbd_rndis_supported_oids);
		if (len > cap) {
			return -1;
		}
		usb_os_memcpy((void *)info, (const void *)usbd_rndis_supported_oids, len);
		return (int)len;

	case USB_RNDIS_OID_GEN_VENDOR_DESCRIPTION:
		desc = (rndis->cfg->vendor_desc != NULL) ? rndis->cfg->vendor_desc : usbd_rndis_default_vendor_desc;
		/* The terminator is part of the string, cut a description longer than the buffer */
		len = (u32)strlen(desc) + 1U;
		if (len > cap) {
			len = cap;
		}
		usb_os_memcpy((void *)info, (const void *)desc, len);
		info[len - 1U] = 0U;
		return (int)len;

	case USB_RNDIS_OID_802_3_PERMANENT_ADDRESS:
	case USB_RNDIS_OID_802_3_CURRENT_ADDRESS:
		usb_os_memcpy((void *)info, (const void *)rndis->mac, USB_RNDIS_MAC_ADDR_LEN);
		return (int)USB_RNDIS_MAC_ADDR_LEN;

	case USB_RNDIS_OID_GEN_HARDWARE_STATUS:      /* NdisHardwareStatusReady */
	case USB_RNDIS_OID_GEN_MEDIA_SUPPORTED:
	case USB_RNDIS_OID_GEN_MEDIA_IN_USE:
	case USB_RNDIS_OID_GEN_PHYSICAL_MEDIUM:      /* NdisPhysicalMediumUnspecified */
	case USB_RNDIS_OID_GEN_RCV_NO_BUFFER:
	case USB_RNDIS_OID_802_3_RCV_ERROR_ALIGNMENT:
	case USB_RNDIS_OID_802_3_XMIT_ONE_COLLISION:
	case USB_RNDIS_OID_802_3_XMIT_MORE_COLLISIONS:
		val = 0U;                                /* USB_RNDIS_MEDIUM_802_3 for the medium OIDs */
		break;

	case USB_RNDIS_OID_GEN_MAXIMUM_FRAME_SIZE:
		val = USB_RNDIS_MTU;
		break;

	case USB_RNDIS_OID_GEN_LINK_SPEED:
		val = ((rndis->dev != NULL) && (rndis->dev->dev_speed == USB_SPEED_HIGH)) ? USB_CDC_CONNECTION_BITRATE_HS : USB_CDC_CONNECTION_BITRATE_FS;
		val /= USBD_RNDIS_LINK_SPEED_UNIT;
		break;

	case USB_RNDIS_OID_GEN_TRANSMIT_BLOCK_SIZE:
	case USB_RNDIS_OID_GEN_RECEIVE_BLOCK_SIZE:
	case USB_RNDIS_OID_GEN_MAXIMUM_TOTAL_SIZE:
		val = USBD_RNDIS_MAX_TRANSFER_SIZE;
		break;

	case USB_RNDIS_OID_GEN_VENDOR_ID:
		val = USBD_RNDIS_VENDOR_ID;
		break;

	case USB_RNDIS_OID_GEN_CURRENT_PACKET_FILTER:
		val = rndis->packet_filter;
		break;

	case USB_RNDIS_OID_GEN_MEDIA_CONNECT_STATUS:
		val = (rndis->uplink_status != 0U) ? USB_RNDIS_MEDIA_STATE_CONNECTED : USB_RNDIS_MEDIA_STATE_DISCONNECTED;
		break;

	case USB_RNDIS_OID_GEN_XMIT_OK:
		val = rndis->xmit_ok;
		break;

	case USB_RNDIS_OID_GEN_RCV_OK:
		val = rndis->rcv_ok;
		break;

	case USB_RNDIS_OID_GEN_XMIT_ERROR:
		val = rndis->xmit_err;
		break;

	case USB_RNDIS_OID_GEN_RCV_ERROR:
		val = rndis->rcv_err;
		break;

	case USB_RNDIS_OID_802_3_MAXIMUM_LIST_SIZE:
		val = 1U;
		break;

	case USB_RNDIS_OID_802_3_MULTICAST_LIST:
		/* An array of 6-byte addresses: no multicast filter is kept, so the list is empty */
		return 0;

	default:
		return -1;
	}

	usb_os_memcpy((void *)info, (const void *)&val, sizeof(val));
	return (int)sizeof(val);
}

/**
 * @brief  Apply a SET for one OID
 * @param  oid: Object identifier
 * @param  info: Information buffer, bounds already checked
 * @param  len: Length of the information buffer
 * @retval USB_RNDIS_STATUS_xxx
 */
static u32 usbd_rndis_set_oid(u32 oid, const u8 *info, u32 len)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	u32 filter;

	switch (oid) {
	case USB_RNDIS_OID_GEN_CURRENT_PACKET_FILTER:
		if (len != sizeof(filter)) {
			return USB_RNDIS_STATUS_INVALID_DATA;
		}
		usb_os_memcpy((void *)&filter, (const void *)info, sizeof(filter));
		rndis->packet_filter = filter;
		/* Ref RNDIS 1.0: a non-zero filter enters rndis-data-initialized, zero leaves it */
		if (filter != 0U) {
			rndis->rndis_state = USBD_RNDIS_STATE_DATA_INITIALIZED;
		} else if (rndis->rndis_state == USBD_RNDIS_STATE_DATA_INITIALIZED) {
			usbd_rndis_data_stop();
			rndis->rndis_state = USBD_RNDIS_STATE_INITIALIZED;
		} else {
			/* Not in the data state: nothing to leave */
		}
		return USB_RNDIS_STATUS_SUCCESS;

	case USB_RNDIS_OID_802_3_MULTICAST_LIST:
		/* Accepted and ignored: frames are not filtered by destination */
		return ((len % USB_RNDIS_MAC_ADDR_LEN) == 0U) ? USB_RNDIS_STATUS_SUCCESS : USB_RNDIS_STATUS_INVALID_DATA;

	default:
		return USB_RNDIS_STATUS_NOT_SUPPORTED;
	}
}

/**
 * @brief  Execute one encapsulated command and queue its response
 * @note   ISR context (EP0 OUT completion). Ref RNDIS 1.0: one command is outstanding at a time,
 *         so a new command replaces an unread response.
 * @param  cmd: Command message
 * @param  cmd_len: Bytes received
 * @retval RTK_SUCCESS, -RTK_ERR_BADARG for a malformed message
 */
static int usbd_rndis_handle_command(const u8 *cmd, u32 cmd_len)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	const usb_rndis_msg_hdr_t *hdr = (const usb_rndis_msg_hdr_t *)(const void *)cmd;
	const usb_rndis_query_msg_t *query;
	usb_rndis_initialize_cmplt_t *init_cmplt;
	usb_rndis_query_cmplt_t *query_cmplt;
	usb_rndis_status_cmplt_t *status_cmplt;
	usb_rndis_reset_cmplt_t *reset_cmplt;
	u8 *resp = rndis->resp_buf;
	u32 msg_len = hdr->MessageLength;
	u32 info_off;
	u32 info_len;
	int len;

	if ((msg_len < sizeof(usb_rndis_msg_hdr_t)) || (msg_len > cmd_len)) {
		return -RTK_ERR_BADARG;
	}

	rndis->resp_len = 0U;
	usb_os_memset((void *)resp, 0, USBD_RNDIS_RESP_BUF_SIZE);

	switch (hdr->MessageType) {
	case USB_RNDIS_MSG_INITIALIZE:
		if (msg_len < sizeof(usb_rndis_initialize_msg_t)) {
			return -RTK_ERR_BADARG;
		}
		init_cmplt = (usb_rndis_initialize_cmplt_t *)(void *)resp;
		init_cmplt->MessageType = USB_RNDIS_MSG_INITIALIZE_CMPLT;
		init_cmplt->MessageLength = sizeof(usb_rndis_initialize_cmplt_t);
		init_cmplt->RequestId = ((const usb_rndis_initialize_msg_t *)(const void *)cmd)->RequestId;
		init_cmplt->Status = USB_RNDIS_STATUS_SUCCESS;
		init_cmplt->MajorVersion = USB_RNDIS_MAJOR_VERSION;
		init_cmplt->MinorVersion = USB_RNDIS_MINOR_VERSION;
		init_cmplt->DeviceFlags = USB_RNDIS_DF_CONNECTIONLESS;
		init_cmplt->Medium = USB_RNDIS_MEDIUM_802_3;
		init_cmplt->MaxPacketsPerTransfer = 1U;
		init_cmplt->MaxTransferSize = USBD_RNDIS_MAX_TRANSFER_SIZE;
		/* AFListOffset/AFListSize and PacketAlignmentFactor stay 0: no alignment between packets */
		rndis->resp_len = (u16)sizeof(usb_rndis_initialize_cmplt_t);
		/* A repeated INITIALIZE restarts the device, Ref RNDIS 1.0 */
		usbd_rndis_data_stop();
		rndis->packet_filter = 0U;
		rndis->rndis_state = USBD_RNDIS_STATE_INITIALIZED;
		break;

	case USB_RNDIS_MSG_HALT:
		/* No response, Ref RNDIS 1.0 */
		usbd_rndis_data_stop();
		rndis->packet_filter = 0U;
		rndis->rndis_state = USBD_RNDIS_STATE_UNINITIALIZED;
		return RTK_SUCCESS;

	case USB_RNDIS_MSG_QUERY:
	case USB_RNDIS_MSG_SET:
		if (msg_len < sizeof(usb_rndis_query_msg_t)) {
			return -RTK_ERR_BADARG;
		}
		query = (const usb_rndis_query_msg_t *)(const void *)cmd;
		info_off = query->InformationBufferOffset + USB_RNDIS_INFO_BUF_OFFSET_BASE;
		info_len = query->InformationBufferLength;
		/* The information buffer shall lie inside the message; checked without overflow */
		if ((info_len != 0U) && ((info_off > msg_len) || (info_len > (msg_len - info_off)))) {
			info_len = 0xFFFFFFFFU;
		}
		if (hdr->MessageType == USB_RNDIS_MSG_QUERY) {
			query_cmplt = (usb_rndis_query_cmplt_t *)(void *)resp;
			query_cmplt->MessageType = USB_RNDIS_MSG_QUERY_CMPLT;
			query_cmplt->RequestId = query->RequestId;
			len = usbd_rndis_query_oid(query->Oid, resp + sizeof(usb_rndis_query_cmplt_t),
									   USBD_RNDIS_RESP_BUF_SIZE - sizeof(usb_rndis_query_cmplt_t));
			if (len < 0) {
				query_cmplt->Status = USB_RNDIS_STATUS_NOT_SUPPORTED;
				len = 0;
			} else {
				query_cmplt->Status = USB_RNDIS_STATUS_SUCCESS;
				query_cmplt->InformationBufferLength = (u32)len;
				/* Ref RNDIS 1.0: the offset is 0 when the information buffer is empty */
				query_cmplt->InformationBufferOffset = (len != 0) ? (sizeof(usb_rndis_query_cmplt_t) - USB_RNDIS_INFO_BUF_OFFSET_BASE) : 0U;
			}
			query_cmplt->MessageLength = sizeof(usb_rndis_query_cmplt_t) + (u32)len;
			rndis->resp_len = (u16)query_cmplt->MessageLength;
		} else {
			status_cmplt = (usb_rndis_status_cmplt_t *)(void *)resp;
			status_cmplt->MessageType = USB_RNDIS_MSG_SET_CMPLT;
			status_cmplt->MessageLength = sizeof(usb_rndis_status_cmplt_t);
			status_cmplt->RequestId = query->RequestId;
			status_cmplt->Status = (info_len == 0xFFFFFFFFU) ? USB_RNDIS_STATUS_INVALID_DATA :
								   usbd_rndis_set_oid(query->Oid, cmd + info_off, info_len);
			rndis->resp_len = (u16)sizeof(usb_rndis_status_cmplt_t);
		}
		break;

	case USB_RNDIS_MSG_RESET:
		/* Ref RNDIS 1.0: the device drops its queued data, the host resends its multicast list */
		usbd_rndis_data_stop();
		rndis->status_pending = 0U;
		reset_cmplt = (usb_rndis_reset_cmplt_t *)(void *)resp;
		reset_cmplt->MessageType = USB_RNDIS_MSG_RESET_CMPLT;
		reset_cmplt->MessageLength = sizeof(usb_rndis_reset_cmplt_t);
		reset_cmplt->Status = USB_RNDIS_STATUS_SUCCESS;
		reset_cmplt->AddressingReset = 1U;
		rndis->resp_len = (u16)sizeof(usb_rndis_reset_cmplt_t);
		break;

	case USB_RNDIS_MSG_KEEPALIVE:
		if (msg_len < sizeof(usb_rndis_request_msg_t)) {
			return -RTK_ERR_BADARG;
		}
		status_cmplt = (usb_rndis_status_cmplt_t *)(void *)resp;
		status_cmplt->MessageType = USB_RNDIS_MSG_KEEPALIVE_CMPLT;
		status_cmplt->MessageLength = sizeof(usb_rndis_status_cmplt_t);
		status_cmplt->RequestId = ((const usb_rndis_request_msg_t *)(const void *)cmd)->RequestId;
		status_cmplt->Status = USB_RNDIS_STATUS_SUCCESS;
		rndis->resp_len = (u16)sizeof(usb_rndis_status_cmplt_t);
		break;

	default:
		/* An unknown message gets no response; the host times it out */
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, (u8)hdr->MessageType);
		return RTK_SUCCESS;
	}

	rndis->notify_pending = 1U;
	usbd_rndis_notify();

	return RTK_SUCCESS;
}

/**
 * @brief  Stop the data path: drop the queued TX frames and release a blocked sender
 * @note   ISR context, or task context once the class is unregistered. Only the consumer index of
 *         the ring buffer moves, transmit() may be adding a frame on another core.
 * @retval None
 */
static void usbd_rndis_data_stop(void)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;

	usb_ringbuf_drop_all(&rndis->bulk_tx_rb);
	if (rndis->bulk_tx_slot_sema != NULL) {
		usb_os_sema_give(rndis->bulk_tx_slot_sema);
	}
}

/**
 * @brief  Start BULK IN for the head frame of the TX ring buffer
 * @note   ISR context only (SOF and BULK IN completion), the single consumer of the ring buffer.
 *         The frame is copied behind a REMOTE_NDIS_PACKET_MSG header in bulk_tx_dma_buf, so the
 *         DMA never reads ring buffer memory. A transfer of N*MPS bytes is ended by a ZLP, which
 *         Ref RNDIS USB 3.1 allows in place of the 1-byte pad.
 * @retval None
 */
static void usbd_rndis_bulk_tx_start(void)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	usbd_ep_t *ep_bulk_in = &rndis->ep_bulk_in;
	usb_rndis_packet_msg_t *pkt = (usb_rndis_packet_msg_t *)(void *)rndis->bulk_tx_dma_buf;
	u32 frame_len;
	int ret;

	if ((ep_bulk_in->xfer_state != 0U) || (usb_ringbuf_is_empty(&rndis->bulk_tx_rb) != 0)) {
		return;
	}

	frame_len = usb_ringbuf_remove_head(&rndis->bulk_tx_rb, rndis->bulk_tx_dma_buf + USB_RNDIS_PACKET_MSG_HDR_SIZE,
										USB_RNDIS_MAX_FRAME_SIZE, NULL);
	if (frame_len == 0U) {
		return;
	}

	/* A slot just freed up: wake a producer blocked in usbd_rndis_transmit() */
	usb_os_sema_give(rndis->bulk_tx_slot_sema);

	usb_os_memset((void *)pkt, 0, USB_RNDIS_PACKET_MSG_HDR_SIZE);
	pkt->MessageType = USB_RNDIS_MSG_PACKET;
	pkt->MessageLength = USB_RNDIS_PACKET_MSG_HDR_SIZE + frame_len;
	pkt->DataOffset = USB_RNDIS_PACKET_DATA_OFFSET;
	pkt->DataLength = frame_len;

	if ((rndis->dev == NULL) || (rndis->dev->is_ready == 0U)) {
		rndis->xmit_err++;
		return;
	}

	ep_bulk_in->xfer_buf = rndis->bulk_tx_dma_buf;
	ep_bulk_in->xfer_len = pkt->MessageLength;
	ep_bulk_in->xfer_state = 1U;
	ret = usbd_ep_transmit(rndis->dev, ep_bulk_in);
	if (ret != RTK_SUCCESS) {
		/* The frame is out of the ring already and no completion will clear the busy flag */
		ep_bulk_in->xfer_state = 0U;
		rndis->xmit_err++;
	} else {
		rndis->xmit_ok++;
	}
}

/**
 * @brief  Queue a frame into the TX ring buffer
 * @note   Task context only, called by usbd_rndis_transmit() inside its gate.
 * @param  buf: Ethernet frame
 * @param  len: Frame length
 * @param  block: See usbd_rndis_transmit()
 * @retval RTK_SUCCESS on success, -RTK_ERR_BUSY if the frame is dropped
 */
static int usbd_rndis_tx_enqueue(const u8 *buf, u32 len, u8 block)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	u32 tx_timeout_ms;
	int ret;

	ret = usb_ringbuf_add_tail(&rndis->bulk_tx_rb, buf, len, 1U);
	if ((ret == RTK_SUCCESS) || (block == 0U)) {
		return (ret == RTK_SUCCESS) ? RTK_SUCCESS : -RTK_ERR_BUSY;
	}

	/* Ring full: wait for the ISR to free a slot. The sema is also given by a data stop or by
	 * deinit, which the checks below catch. */
	tx_timeout_ms = (rndis->cfg->bulk_tx_timeout_ms != 0U) ? rndis->cfg->bulk_tx_timeout_ms : USBD_RNDIS_DEFAULT_BULK_TX_TIMEOUT_MS;
	do {
		if (usb_os_sema_take(rndis->bulk_tx_slot_sema, tx_timeout_ms) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "TX timeout drop(%u)\n", len);
			return -RTK_ERR_BUSY;
		}
		if ((usbd_rndis_data_on() == 0U) || (rndis->is_closing != 0U)) {
			return -RTK_ERR_BUSY;
		}
		ret = usb_ringbuf_add_tail(&rndis->bulk_tx_rb, buf, len, 1U);
	} while (ret != RTK_SUCCESS);

	return RTK_SUCCESS;
}

/**
 * @brief  Set RNDIS configuration
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 *         Ref RNDIS USB 2.1: the data interface has a single setting, so every endpoint belongs
 *         to the configuration itself.
 * @param  dev: USB device instance
 * @param  config: Configuration number
 * @retval Status
 */
static int usbd_rndis_set_config(usb_dev_t *dev, u8 config)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	u16 mps = (dev->dev_speed == USB_SPEED_HIGH) ? (u16)USB_BULK_HS_MAX_MPS : (u16)USB_BULK_FS_MAX_MPS;
	int ret;

	if (config != 1U) {
		return -RTK_ERR_BADARG;
	}

	rndis->dev = dev;

	rndis->ep_intr_in.xfer_state = 0U;
	rndis->ep_intr_in.info.mps = USBD_RNDIS_INTR_IN_PACKET_SIZE;
	ret = usbd_ep_init(dev, &rndis->ep_intr_in);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	rndis->ep_bulk_in.xfer_state = 0U;
	rndis->ep_bulk_in.info.mps = mps;
	ret = usbd_ep_init(dev, &rndis->ep_bulk_in);
	if (ret != RTK_SUCCESS) {
		usbd_rndis_clear_config(dev, config);
		return ret;
	}

	rndis->ep_bulk_out.info.mps = mps;
	ret = usbd_ep_init(dev, &rndis->ep_bulk_out);
	if (ret != RTK_SUCCESS) {
		usbd_rndis_clear_config(dev, config);
		return ret;
	}

	/* A fresh configuration starts uninitialized, Ref RNDIS USB 2.1 */
	rndis->rndis_state = USBD_RNDIS_STATE_UNINITIALIZED;
	rndis->packet_filter = 0U;
	rndis->connect_status = 0U;
	rndis->resp_len = 0U;
	rndis->status_pending = 0U;
	rndis->notify_pending = 0U;
	rndis->is_configured = 1U;

	ret = usbd_ep_receive(dev, &rndis->ep_bulk_out);
	if (ret != RTK_SUCCESS) {
		usbd_rndis_clear_config(dev, config);
	}

	return ret;
}

/**
 * @brief  Clear RNDIS configuration
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param  dev: USB device instance
 * @param  config: Configuration number
 * @retval None
 */
static void usbd_rndis_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;

	UNUSED(config);

	/* Close the data path first, so no TX/RX submission reaches a de-initialised endpoint */
	rndis->is_configured = 0U;
	rndis->rndis_state = USBD_RNDIS_STATE_UNINITIALIZED;
	rndis->connect_status = 0U;
	rndis->notify_pending = 0U;
	rndis->status_pending = 0U;
	rndis->resp_len = 0U;
	rndis->ctrl_req_pending = 0U;
	rndis->rx_pending_len = 0U;

	usbd_ep_deinit(dev, &rndis->ep_bulk_in);
	usbd_ep_deinit(dev, &rndis->ep_bulk_out);
	usbd_ep_deinit(dev, &rndis->ep_intr_in);
	rndis->ep_bulk_in.xfer_state = 0U;
	rndis->ep_intr_in.xfer_state = 0U;

	usbd_rndis_data_stop();
}

/**
 * @brief  Answer GET_ENCAPSULATED_RESPONSE
 * @note   ISR context. Ref RNDIS USB 2.2.2: with nothing to report the device returns a single
 *         0x00 byte rather than stalling. A pending command response goes first, then a pending
 *         INDICATE_STATUS message.
 * @param  dev: USB device instance
 * @param  w_length: wLength of the request
 * @retval Status
 */
static int usbd_rndis_get_response(usb_dev_t *dev, u16 w_length)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usb_rndis_indicate_status_msg_t *ind;
	u32 len = rndis->resp_len;

	if ((len == 0U) && (rndis->status_pending != 0U)) {
		ind = (usb_rndis_indicate_status_msg_t *)(void *)rndis->resp_buf;
		usb_os_memset((void *)ind, 0, sizeof(usb_rndis_indicate_status_msg_t));
		ind->MessageType = USB_RNDIS_MSG_INDICATE_STATUS;
		ind->MessageLength = sizeof(usb_rndis_indicate_status_msg_t);
		ind->Status = (rndis->status_connect != 0U) ? USB_RNDIS_STATUS_MEDIA_CONNECT : USB_RNDIS_STATUS_MEDIA_DISCONNECT;
		len = sizeof(usb_rndis_indicate_status_msg_t);
		rndis->status_pending = 0U;
	}

	if (len == 0U) {
		ep0_in->xfer_buf[0] = 0U;
		len = 1U;
	} else {
		/* Ref USB 2.0 9.3.5: never more than wLength; the response is consumed either way */
		if (len > w_length) {
			len = w_length;
		}
		if (len > ep0_in->xfer_buf_len) {
			len = ep0_in->xfer_buf_len;
		}
		usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)rndis->resp_buf, len);
		rndis->resp_len = 0U;
	}

	/* A status message still waiting gets its own notification */
	if (rndis->status_pending != 0U) {
		rndis->notify_pending = 1U;
	}

	ep0_in->xfer_len = len;
	return usbd_ep_transmit(dev, ep0_in);
}

/**
 * @brief  Handle RNDIS SETUP requests
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param  dev: USB device instance
 * @param  req: SETUP request
 * @retval Status
 */
static int usbd_rndis_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	u8 is_ours = ((req->wIndex == USBD_RNDIS_COMM_INTERFACE_NUM) || (req->wIndex == USBD_RNDIS_DATA_INTERFACE_NUM)) ? 1U : 0U;
	int ret = -RTK_ERR_BADARG;

	if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) != USB_REQ_RECIPIENT_INTERFACE) {
		/* ENDPOINT_HALT and the like: the core handles them */
		return -RTK_ERR_BADARG;
	}

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		if ((dev->dev_state != USBD_STATE_CONFIGURED) || (is_ours == 0U)) {
			break;
		}
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			/* Both interfaces have alt 0 only. Ref USB 2.0 9.4.10: their endpoints return to
			   the default state, not halted with DATA0 */
			if ((req->wValue != 0U) || (req->wLength != 0U)) {
				break;
			}
			if (req->wIndex == USBD_RNDIS_DATA_INTERFACE_NUM) {
				usbd_ep_clear_stall(dev, &rndis->ep_bulk_in);
				usbd_ep_clear_stall(dev, &rndis->ep_bulk_out);
			} else {
				usbd_ep_clear_stall(dev, &rndis->ep_intr_in);
			}
			ret = RTK_SUCCESS;
			break;

		case USB_REQ_GET_INTERFACE:
			if ((req->wValue == 0U) && (req->wLength == 1U)) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_len = 1U;
				ret = usbd_ep_transmit(dev, ep0_in);
			}
			break;

		case USB_REQ_GET_STATUS:
			if ((req->wValue == 0U) && (req->wLength == 2U)) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				ret = usbd_ep_transmit(dev, ep0_in);
			}
			break;

		default:
			break;
		}
		break;

	case USB_REQ_TYPE_CLASS:
		/* Ref RNDIS USB 2.2: both requests address the communication interface, wValue 0 */
		if ((dev->dev_state != USBD_STATE_CONFIGURED) || (req->wIndex != USBD_RNDIS_COMM_INTERFACE_NUM) || (req->wValue != 0U)) {
			break;
		}
		if ((req->bRequest == USB_RNDIS_SEND_ENCAPSULATED_COMMAND) && ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_H2D)) {
			if ((req->wLength < USBD_RNDIS_MIN_CMD_LEN) || (req->wLength > ep0_out->xfer_buf_len)) {
				break;
			}
			rndis->ctrl_req_len = req->wLength;
			rndis->ctrl_req_pending = 1U;
			ep0_out->xfer_len = req->wLength;
			ret = usbd_ep_receive(dev, ep0_out);
			if (ret != RTK_SUCCESS) {
				rndis->ctrl_req_pending = 0U;
			}
		} else if ((req->bRequest == USB_RNDIS_GET_ENCAPSULATED_RESPONSE) && ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) &&
				   (req->wLength != 0U)) {
			ret = usbd_rndis_get_response(dev, req->wLength);
		} else {
			/* Unsupported request: request error */
		}
		break;

	default:
		break;
	}

	return ret;
}

/**
 * @brief  Handle EP0 data OUT phase: the encapsulated command
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param  dev: USB device instance
 * @retval Status, non-zero makes the core stall the status stage
 */
static int usbd_rndis_handle_ep0_data_out(usb_dev_t *dev)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;

	if (rndis->ctrl_req_pending == 0U) {
		return -RTK_ERR_BADARG;
	}
	rndis->ctrl_req_pending = 0U;

	return usbd_rndis_handle_command(dev->ep0_out.xfer_buf, rndis->ctrl_req_len);
}

/**
 * @brief  Handle EP data IN completion
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param  dev: USB device instance
 * @param  ep_addr: Endpoint address
 * @param  status: Transfer status
 * @retval RTK_SUCCESS for an own endpoint, -RTK_ERR_BADARG otherwise
 */
static int usbd_rndis_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, int status)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;

	UNUSED(dev);
	UNUSED(status);

	if (ep_addr == rndis->cfg->bulk_in_addr) {
		rndis->ep_bulk_in.xfer_state = 0U;
		if (usbd_rndis_data_on() != 0U) {
			usbd_rndis_bulk_tx_start();
		}
	} else if (ep_addr == rndis->cfg->intr_in_addr) {
		rndis->ep_intr_in.xfer_state = 0U;
		usbd_rndis_notify();
	} else {
		return -RTK_ERR_BADARG;
	}

	return RTK_SUCCESS;
}

/**
 * @brief  Deliver the frames of one received transfer to the upper layer
 * @note   RX task context. Ref RNDIS 1.0: a transfer carries one or more REMOTE_NDIS_PACKET_MSG,
 *         possibly followed by a 1-byte pad; each message is bounds-checked before its frame is read.
 * @param  buf: Received transfer
 * @param  len: Transfer length
 * @retval None
 */
static void usbd_rndis_deliver(u8 *buf, u32 len)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	const usbd_rndis_cb_t *cb = rndis->cb;
	const usb_rndis_packet_msg_t *pkt;
	u32 off = 0U;
	u32 msg_len;
	u32 data_off;
	u32 data_len;

	while ((len - off) >= USB_RNDIS_PACKET_MSG_HDR_SIZE) {
		pkt = (const usb_rndis_packet_msg_t *)(const void *)(buf + off);
		msg_len = pkt->MessageLength;
		data_off = pkt->DataOffset + 8U;
		data_len = pkt->DataLength;
		if ((pkt->MessageType != USB_RNDIS_MSG_PACKET) || (msg_len < USB_RNDIS_PACKET_MSG_HDR_SIZE) || (msg_len > (len - off)) ||
			(data_off < USB_RNDIS_PACKET_MSG_HDR_SIZE) || (pkt->DataOffset > msg_len) || (data_off > msg_len) || (data_len > (msg_len - data_off)) ||
			(data_len == 0U)) {
			rndis->rcv_err++;
			return;
		}
		if ((usbd_rndis_data_on() != 0U) && (cb != NULL) && (cb->received != NULL)) {
			/* The frame is consumed either way: nothing to retry on a callback error */
			cb->received(buf + off + data_off, data_len);
			rndis->rcv_ok++;
		}
		off += msg_len;
	}
}

/**
 * @brief  RX delivery thread
 * @details Waits on rx_data_ready_sema, parses the transfer, then sets rx_buf_free so the USB
 *          OUT path can hand off again.
 */
static void usbd_rndis_rx_thread(void *param)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;

	UNUSED(param);

	while (rndis->rx_thread_running != 0U) {
		if (usb_os_sema_take(rndis->rx_data_ready_sema, USBD_RNDIS_RX_SEMA_TAKE_TIMEOUT_MS) != RTK_SUCCESS) {
			continue;
		}
		if (rndis->rx_thread_running == 0U) {
			break;
		}

		usbd_rndis_deliver(rndis->rx_msg_buf, rndis->rx_msg_len);
		rndis->rx_buf_free = 1U;
	}

	rndis->rx_task = NULL;
	rtos_task_delete(NULL);
}

/**
 * @brief  Hand the current RX buffer to the RX thread and arm the other one
 * @note   ISR context, only while rx_buf_free is 1.
 * @param  len: Transfer length
 * @retval None
 */
static void usbd_rndis_rx_handoff(u32 len)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;

	rndis->rx_buf_free = 0U;
	rndis->rx_msg_buf = rndis->rx_buf[rndis->rx_xfer_idx];
	rndis->rx_msg_len = len;
	usb_os_sema_give(rndis->rx_data_ready_sema);

	rndis->rx_xfer_idx ^= 1U;
	rndis->ep_bulk_out.xfer_buf = rndis->rx_buf[rndis->rx_xfer_idx];
}

/**
 * @brief  Handle EP data OUT completion
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param  dev: USB device instance
 * @param  ep_addr: Endpoint address
 * @param  len: Received data length
 * @retval RTK_SUCCESS for an own endpoint, -RTK_ERR_BADARG otherwise
 */
static int usbd_rndis_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	usbd_ep_t *ep_bulk_out = &rndis->ep_bulk_out;

	UNUSED(dev);

	if (ep_addr != rndis->cfg->bulk_out_addr) {
		return -RTK_ERR_BADARG;
	}

	if ((ep_bulk_out->skip_dcache_post_invalidate != 0U) && (len > 0U)) {
		DCache_Invalidate((u32)ep_bulk_out->xfer_buf, len);
	}

	if (rndis->is_configured == 0U) {
		return RTK_SUCCESS;
	}

	/* Frames are dropped until the host set a packet filter, the endpoint stays armed */
	if ((len >= USB_RNDIS_PACKET_MSG_HDR_SIZE) && (usbd_rndis_data_on() != 0U)) {
		if (rndis->rx_buf_free == 0U) {
			/* RX thread busy: leave the endpoint unarmed so the host sees NAK, SOF re-arms it */
			rndis->rx_pending_len = len;
			return RTK_SUCCESS;
		}
		usbd_rndis_rx_handoff(len);
	}

	if (usbd_ep_receive(rndis->dev, ep_bulk_out) != RTK_SUCCESS) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
	}

	return RTK_SUCCESS;
}

/**
 * @brief  SOF interrupt handler
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 *         Reports a media change, retries a pending notification, starts the TX path and re-arms
 *         a deferred BULK OUT. usbd_rndis_set_link_status() only writes uplink_status, so the
 *         notification fields keep the USB ISR as their only writer.
 * @param  dev: USB device instance
 */
static void usbd_rndis_sof(usb_dev_t *dev)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	u8 link_up;

	UNUSED(dev);

	if (rndis->is_configured == 0U) {
		return;
	}

	link_up = ((usbd_rndis_data_on() != 0U) && (rndis->uplink_status != 0U)) ? 1U : 0U;
	if (link_up != rndis->connect_status) {
		rndis->connect_status = link_up;
		/* Ref RNDIS 1.0: INDICATE_STATUS is read by the host through GET_ENCAPSULATED_RESPONSE */
		rndis->status_connect = link_up;
		rndis->status_pending = 1U;
		rndis->notify_pending = 1U;
	}

	usbd_rndis_notify();

	if (usbd_rndis_data_on() != 0U) {
		usbd_rndis_bulk_tx_start();
	}

	if (rndis->rx_pending_len == 0U) {
		return;
	}

	if (usbd_rndis_data_on() != 0U) {
		if (rndis->rx_buf_free == 0U) {
			return;
		}
		usbd_rndis_rx_handoff(rndis->rx_pending_len);
	}
	/* Delivered, or dropped since the data path went down meanwhile: the armed buffer is free again */
	rndis->rx_pending_len = 0U;
	if (usbd_ep_receive(rndis->dev, &rndis->ep_bulk_out) != RTK_SUCCESS) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, rndis->cfg->bulk_out_addr);
	}
}

/**
 * @brief  Patch the runtime-assigned fields in a configuration descriptor block
 * @note   Writes the endpoint addresses from the class configuration, the BULK wMaxPacketSize and
 *         INTR IN bInterval of the speed the descriptor describes, the iInterface string index,
 *         and rebases the Union and Call Management interface references with the interface base.
 *         Patched byte by byte: the block may sit at any offset of the EP0 buffer.
 * @param  desc: Pointer to config descriptor body (starting after config header)
 * @param  len: Length of the descriptor block
 * @param  speed: Speed the descriptor describes
 * @retval None
 */
static void usbd_rndis_patch_desc(u8 *desc, u16 len, usb_speed_type_t speed)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	const usbd_rndis_config_t *cfg = rndis->cfg;
	u16 bulk_mps = (speed == USB_SPEED_HIGH) ? USB_BULK_HS_MAX_MPS : USB_BULK_FS_MAX_MPS;
	u8 interval;
	u8 dlen;
	u8 dtype;
	u16 i;

	if (speed == USB_SPEED_HIGH) {
		interval = (cfg->intr_in_interval_hs != 0U) ? cfg->intr_in_interval_hs : USBD_RNDIS_DEFAULT_INTR_IN_INTERVAL_HS;
	} else {
		interval = (cfg->intr_in_interval_fs != 0U) ? cfg->intr_in_interval_fs : USBD_RNDIS_DEFAULT_INTR_IN_INTERVAL_FS;
	}

	for (i = 0U; i < len;) {
		dlen = desc[i];
		if ((dlen < 2U) || (((u32)i + (u32)dlen) > (u32)len)) {
			break;
		}
		dtype = desc[i + 1U];

		if ((dtype == USB_DESC_TYPE_ENDPOINT) && (dlen >= USB_LEN_EP_DESC)) {
			if ((desc[i + 3U] & USB_EP_XFER_TYPE_MASK) == USB_CH_EP_TYPE_BULK) {
				desc[i + 2U] = ((desc[i + 2U] & USB_REQ_DIR_MASK) == USB_D2H) ? cfg->bulk_in_addr : cfg->bulk_out_addr;
				desc[i + 4U] = USB_LOW_BYTE(bulk_mps);
				desc[i + 5U] = USB_HIGH_BYTE(bulk_mps);
			} else {
				desc[i + 2U] = cfg->intr_in_addr;
				desc[i + 6U] = interval;
			}
		} else if ((dtype == USB_DESC_TYPE_INTERFACE) && (dlen >= USB_LEN_IF_DESC) && (desc[i + 5U] == USB_RNDIS_WIRELESS_CLASS_CODE)) {
			desc[i + 8U] = rndis->itf_str_idx;
		} else if ((dtype == USB_CDC_CS_INTERFACE) && (dlen >= 5U) && (desc[i + 2U] == USB_CDC_FUNC_DESC_UNION)) {
			desc[i + 3U] = (u8)(rndis->if_base + USBD_RNDIS_COMM_INTERFACE_NUM);
			desc[i + 4U] = (u8)(rndis->if_base + USBD_RNDIS_DATA_INTERFACE_NUM);
		} else if ((dtype == USB_CDC_CS_INTERFACE) && (dlen >= 5U) && (desc[i + 2U] == USB_CDC_FUNC_DESC_CALL_MGMT)) {
			desc[i + 4U] = (u8)(rndis->if_base + USBD_RNDIS_DATA_INTERFACE_NUM);
		} else {
			/* Nothing to patch */
		}
		i += dlen;
	}
}

/**
 * @brief  Get USB descriptor
 * @note   This function is called within an interrupt service routine (ISR) context.
 * @param  dev: USB device instance
 * @param  req: Setup request
 * @param  buf: Destination
 * @param  buf_len: Capacity of buf
 * @retval Descriptor length, 0 to stall
 */
static u16 usbd_rndis_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len)
{
	usb_speed_type_t speed = dev->dev_speed;
	usb_speed_type_t desc_speed = speed;
	u8 desc_type = USB_HIGH_BYTE(req->wValue);
	const u8 *desc = NULL;
	u16 len = 0U;
	u8 is_cfg = 0U;

	switch (desc_type) {
	case USB_DESC_TYPE_DEVICE:
		desc = usbd_rndis_dev_desc;
		len = sizeof(usbd_rndis_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		desc = usbd_rndis_config_desc;
		len = sizeof(usbd_rndis_config_desc);
		is_cfg = 1U;
		break;

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		desc = usbd_rndis_device_qualifier_desc;
		len = sizeof(usbd_rndis_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		/* Ref USB 2.0 9.6.4: describes the configuration at the speed the device is not running at */
		desc = usbd_rndis_config_desc;
		len = sizeof(usbd_rndis_config_desc);
		desc_speed = (speed == USB_SPEED_HIGH) ? USB_SPEED_FULL : USB_SPEED_HIGH;
		is_cfg = 1U;
		break;
#endif

	case USB_DESC_TYPE_STRING:
		/* Every string of this class is registered with usbd_add_string() and answered by the core */
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_GET_DESC, 0);
		break;

	default:
		break;
	}

	if (desc == NULL) {
		return 0U;
	}

	/* Truncation is not allowed: a short descriptor is illegal, so stall instead */
	if (len > buf_len) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Desc %d OVSZ %d > %d\n", desc_type, len, buf_len);
		return 0U;
	}

	usb_os_memcpy((void *)buf, (const void *)desc, len);

	if (is_cfg != 0U) {
		buf[USB_CFG_DESC_OFFSET_TYPE] = desc_type;
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		usbd_rndis_patch_desc(buf + USB_LEN_CFG_DESC, len - USB_LEN_CFG_DESC, desc_speed);
	}

	return len;
}

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief  Store the first interface number assigned to this class by the composite framework
 * @note   This function is called within an interrupt service routine (ISR) context.
 * @param  base: First interface number of this class
 */
static void usbd_rndis_set_interface_base(u8 base)
{
	usbd_rndis_dev.if_base = base;
}
#endif

/**
 * @brief  USB attach status changed callback
 * @note   This function is called within an interrupt service routine (ISR) context;
 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
 * @param  dev: USB device instance
 * @param  old_status: Previous status
 * @param  status: Current status
 */
static void usbd_rndis_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	const usbd_rndis_cb_t *cb = rndis->cb;

	UNUSED(dev);

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		/* The bus is gone: data stops at once, clear_config()/deinit() release the rest */
		rndis->rndis_state = USBD_RNDIS_STATE_UNINITIALIZED;
		rndis->connect_status = 0U;
		rndis->notify_pending = 0U;
		rndis->status_pending = 0U;
		rndis->rx_pending_len = 0U;
	}

	if ((cb != NULL) && (cb->status_changed != NULL)) {
		cb->status_changed(old_status, status);
	}
}

/**
 * @brief  Release every resource of the shared private init, in reverse order
 * @note   Task context, with the class not registered. Safe after a partial init.
 * @retval None
 */
static void usbd_rndis_free_resources(void)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	u8 i;

	if (rndis->rx_task != NULL) {
		rndis->rx_thread_running = 0U;
		usb_os_sema_give(rndis->rx_data_ready_sema);
		while (rndis->rx_task != NULL) {
			usb_os_sleep_ms(10U);
		}
	}
	rndis->rx_thread_running = 0U;

	if (rndis->rx_data_ready_sema != NULL) {
		usb_os_sema_delete(rndis->rx_data_ready_sema);
		rndis->rx_data_ready_sema = NULL;
	}
	if (rndis->bulk_tx_slot_sema != NULL) {
		usb_os_sema_delete(rndis->bulk_tx_slot_sema);
		rndis->bulk_tx_slot_sema = NULL;
	}

	usb_os_mfree((void *)rndis->ep_intr_in.xfer_buf);
	rndis->ep_intr_in.xfer_buf = NULL;
	for (i = 0U; i < USBD_RNDIS_RX_BUF_NUM; i++) {
		usb_os_mfree((void *)rndis->rx_buf[i]);
		rndis->rx_buf[i] = NULL;
	}
	rndis->ep_bulk_out.xfer_buf = NULL;
	usb_os_mfree((void *)rndis->bulk_tx_dma_buf);
	rndis->bulk_tx_dma_buf = NULL;
	usb_ringbuf_manager_deinit(&rndis->bulk_tx_rb);
}

/**
 * @brief  Initialize RNDIS class, shared by standalone and composite modes
 * @param  cb: User callbacks
 * @param  cfg: Class configuration
 * @retval Status
 */
static int usbd_rndis_private_init(const usbd_rndis_cb_t *cb, const usbd_rndis_config_t *cfg)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	u8 tx_depth;
	u8 i;
	int ret;

	if ((cb == NULL) || (cfg == NULL)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid param\n");
		return -RTK_ERR_BADARG;
	}

	/* rx_buf[0] only exists between a successful init and deinit */
	if (rndis->rx_buf[0] != NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Already init\n");
		return -RTK_ERR_BADARG;
	}

	/* Every endpoint is mandatory, with the right direction */
	if (((cfg->bulk_in_addr & USB_REQ_DIR_MASK) != USB_D2H) || ((cfg->bulk_out_addr & USB_REQ_DIR_MASK) != USB_H2D) ||
		((cfg->intr_in_addr & USB_REQ_DIR_MASK) != USB_D2H) || ((cfg->bulk_in_addr & (~USB_REQ_DIR_MASK)) == 0U) ||
		((cfg->bulk_out_addr & (~USB_REQ_DIR_MASK)) == 0U) || ((cfg->intr_in_addr & (~USB_REQ_DIR_MASK)) == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP addr %02x/%02x/%02x\n", cfg->bulk_in_addr, cfg->bulk_out_addr, cfg->intr_in_addr);
		return -RTK_ERR_BADARG;
	}

	usb_os_memset((void *)rndis, 0, sizeof(usbd_rndis_dev_t));
	rndis->cfg = cfg;
	rndis->if_base = 0U;
	rndis->itf_str_idx = usbd_add_string(cfg->itf_str);
	usb_os_memcpy((void *)rndis->mac, (const void *)((cfg->mac != NULL) ? cfg->mac : usbd_rndis_default_mac), USB_RNDIS_MAC_ADDR_LEN);

	tx_depth = (cfg->bulk_tx_queue_depth != 0U) ? cfg->bulk_tx_queue_depth : USBD_RNDIS_DEFAULT_BULK_TX_QUEUE_DEPTH;
	ret = usb_ringbuf_manager_init(&rndis->bulk_tx_rb, tx_depth, USB_RNDIS_MAX_FRAME_SIZE, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init TX ring fail\n");
		return -RTK_ERR_NOMEM;
	}

	ret = -RTK_ERR_NOMEM;
	rndis->bulk_tx_dma_buf = (u8 *)usb_os_malloc(USBD_RNDIS_TX_BUF_SIZE);
	if (rndis->bulk_tx_dma_buf == NULL) {
		goto init_fail;
	}
	for (i = 0U; i < USBD_RNDIS_RX_BUF_NUM; i++) {
		rndis->rx_buf[i] = (u8 *)usb_os_malloc(USBD_RNDIS_RX_BUF_SIZE);
		if (rndis->rx_buf[i] == NULL) {
			goto init_fail;
		}
	}
	rndis->ep_intr_in.xfer_buf = (u8 *)usb_os_malloc(USBD_RNDIS_INTR_IN_PACKET_SIZE);
	if (rndis->ep_intr_in.xfer_buf == NULL) {
		goto init_fail;
	}

	rndis->ep_bulk_in.info.addr = cfg->bulk_in_addr;
	rndis->ep_bulk_in.info.type = USB_CH_EP_TYPE_BULK;
	rndis->ep_bulk_in.xfer_buf_len = USBD_RNDIS_TX_BUF_SIZE;

	rndis->ep_bulk_out.info.addr = cfg->bulk_out_addr;
	rndis->ep_bulk_out.info.type = USB_CH_EP_TYPE_BULK;
	rndis->ep_bulk_out.xfer_buf_len = USBD_RNDIS_RX_BUF_SIZE;
	rndis->ep_bulk_out.xfer_len = USBD_RNDIS_RX_BUF_SIZE;
	rndis->ep_bulk_out.xfer_buf = rndis->rx_buf[0];

	rndis->ep_intr_in.info.addr = cfg->intr_in_addr;
	rndis->ep_intr_in.info.type = USB_CH_EP_TYPE_INTR;
	rndis->ep_intr_in.xfer_buf_len = USBD_RNDIS_INTR_IN_PACKET_SIZE;

	if ((usb_os_sema_create(&rndis->bulk_tx_slot_sema) != RTK_SUCCESS) ||
		(usb_os_sema_create(&rndis->rx_data_ready_sema) != RTK_SUCCESS)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto init_fail;
	}

	rndis->rx_buf_free = 1U;
	rndis->rx_thread_running = 1U;
	ret = rtos_task_create(&rndis->rx_task, "usbd_rndis_rx_thread", usbd_rndis_rx_thread, NULL,
						   (cfg->rx_task_stack_size != 0U) ? cfg->rx_task_stack_size : USBD_RNDIS_DEFAULT_RX_TASK_STACK_SIZE,
						   (cfg->rx_task_priority != 0U) ? cfg->rx_task_priority : USBD_RNDIS_DEFAULT_RX_TASK_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create RX thread fail\n");
		rndis->rx_task = NULL;
		ret = -RTK_ERR_IO;
		goto init_fail;
	}

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "User init fail: %d\n", ret);
			goto init_fail;
		}
	}

	rndis->cb = cb;
	rndis->is_closing = 0U;

	return RTK_SUCCESS;

init_fail:
	usbd_rndis_free_resources();
	return ret;
}

/* Exported functions --------------------------------------------------------*/

int usbd_rndis_init(const usbd_rndis_cb_t *cb, const usbd_rndis_config_t *cfg)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	int ret;

	ret = usbd_rndis_private_init(cb, cfg);
	if (ret == RTK_SUCCESS) {
		rndis->from_composite = 0U;
		ret = usbd_register_class(&usbd_rndis_driver);
		if (ret != RTK_SUCCESS) {
			usbd_rndis_deinit();
		}
	}

	return ret;
}

#ifdef CONFIG_USBD_COMPOSITE
int usbd_composite_rndis_init(const usbd_rndis_cb_t *cb, const usbd_rndis_config_t *cfg)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	int ret;

	ret = usbd_rndis_private_init(cb, cfg);
	if (ret == RTK_SUCCESS) {
		rndis->from_composite = 1U;
		ret = usbd_composite_register_driver(&usbd_rndis_driver);
		if (ret != RTK_SUCCESS) {
			usbd_rndis_deinit();
		}
	}

	return ret;
}
#endif

void usbd_rndis_deinit(void)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	const usbd_rndis_cb_t *cb;
	u32 wait_cnt = 0U;
	int ret;

	/* Leave the core first: once accepted no class callback runs any more */
#ifdef CONFIG_USBD_COMPOSITE
	if (rndis->from_composite != 0U) {
		ret = usbd_composite_unregister_driver(&usbd_rndis_driver);
	} else
#endif
	{
		ret = usbd_unregister_class();
	}
	if (ret != RTK_SUCCESS) {
		return;
	}

	/* Close the TX gate, then wait for every task inside usbd_rndis_transmit() to leave */
	usb_os_enter_critical();
	rndis->is_closing = 1U;
	rndis->is_configured = 0U;
	rndis->rndis_state = USBD_RNDIS_STATE_UNINITIALIZED;
	rndis->connect_status = 0U;
	usb_os_exit_critical();

	while (rndis->xfer_users != 0U) {
		usb_os_sema_give(rndis->bulk_tx_slot_sema);
		usb_os_sleep_ms(1U);
		wait_cnt++;
		if (wait_cnt >= USBD_RNDIS_TX_EXIT_WARN_MS) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Wait TX exit\n");
			wait_cnt = 0U;
		}
	}

	rndis->ep_bulk_in.xfer_state = 0U;
	rndis->ep_intr_in.xfer_state = 0U;
	rndis->ctrl_req_pending = 0U;

	usbd_rndis_free_resources();

	cb = rndis->cb;
	rndis->cb = NULL;
	if ((cb != NULL) && (cb->deinit != NULL) && (cb->deinit() != RTK_SUCCESS)) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "CB deinit fail\n");
	}
}

int usbd_rndis_transmit(const u8 *buf, u32 len, u8 block)
{
	usbd_rndis_dev_t *rndis = &usbd_rndis_dev;
	int ret;

	if (usb_os_is_in_interrupt() != 0U) {
		return -RTK_ERR_BADARG;
	}

	if ((buf == NULL) || (len == 0U) || (len > USB_RNDIS_MAX_FRAME_SIZE)) {
		return -RTK_ERR_BADARG;
	}

	/* Counted under the same lock deinit closes the gate with */
	usb_os_enter_critical();
	if ((rndis->is_closing != 0U) || (rndis->rx_buf[0] == NULL) || (usbd_rndis_data_on() == 0U)) {
		usb_os_exit_critical();
		return -RTK_ERR_BUSY;
	}
	rndis->xfer_users++;
	usb_os_exit_critical();

	ret = usbd_rndis_tx_enqueue(buf, len, block);
	if (ret != RTK_SUCCESS) {
		rndis->xmit_err++;
	}

	usb_os_enter_critical();
	rndis->xfer_users--;
	usb_os_exit_critical();

	return ret;
}

int usbd_rndis_get_link_status(void)
{
	return usbd_rndis_dev.connect_status;
}

int usbd_rndis_set_link_status(u8 link_up)
{
	usbd_rndis_dev.uplink_status = (link_up != 0U) ? 1U : 0U;

	return RTK_SUCCESS;
}

__attribute__((weak))
int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbd_rndis_transmit(buf, len, block);
}
