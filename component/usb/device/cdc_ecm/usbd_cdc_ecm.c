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

/* Private function prototypes -----------------------------------------------*/

static int cdc_ecm_set_config(usb_dev_t *dev, u8 config);
static int cdc_ecm_clear_config(usb_dev_t *dev, u8 config);
static int cdc_ecm_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 cdc_ecm_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int cdc_ecm_handle_ep0_data_out(usb_dev_t *dev);
static int cdc_ecm_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int cdc_ecm_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static void cdc_ecm_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
static int cdc_ecm_bulk_tx_status_check(void);
static int cdc_ecm_intr_in_send(void *data, u16 len);
static int cdc_ecm_send_notification(void);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "ECM";
static u8 ecm_mac[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};  /**< MAC address */

/* USB Standard Device Descriptor */
static const u8 usbd_cdc_ecm_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,                               /* bLength */
	USB_DESC_TYPE_DEVICE,                           /* bDescriptorType */
	0x00, 0x02,                                     /* bcdUSB: 2.0 */
	USB_CDC_COMM_INTERFACE_CLASS_CODE,              /* bDeviceClass: CDC */
	USB_CDC_SUBCLASS_ECM,                           /* bDeviceSubClass: ECM */
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
	USB_CDC_COMM_INTERFACE_CLASS_CODE,              /* bInterfaceClass: CDC */
	USB_CDC_SUBCLASS_ECM,                           /* bInterfaceSubClass: ECM */
	0x00,                                           /* bInterfaceProtocol */
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
	USB_CDC_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	0x00,                                           /* bInterfaceSubClass */
	0x00,                                           /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* Data Interface Descriptor (Alt 1 - Active) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_ECM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x01,                                           /* bAlternateSetting */
	0x02,                                           /* bNumEndpoints */
	USB_CDC_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
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
	USB_CDC_COMM_INTERFACE_CLASS_CODE,              /* bInterfaceClass: CDC */
	USB_CDC_SUBCLASS_ECM,                           /* bInterfaceSubClass: ECM */
	0x00,                                           /* bInterfaceProtocol */
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
	USB_CDC_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
	0x00,                                           /* bInterfaceSubClass */
	0x00,                                           /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* Data Interface Descriptor (Alt 1 - Active) */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_CDC_ECM_DATA_INTERFACE_NUM,                /* bInterfaceNumber */
	0x01,                                           /* bAlternateSetting */
	0x02,                                           /* bNumEndpoints */
	USB_CDC_DATA_INTERFACE_CLASS_CODE,              /* bInterfaceClass */
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
	.status_changed = cdc_ecm_status_changed,
};

/* CDC ECM Device Instance */
static usbd_cdc_ecm_dev_t usbd_cdc_ecm_dev;

/* Private functions ---------------------------------------------------------*/
static inline u8 usbh_cdc_ecm_char_to_hex(u8 value)
{
	if (value <= 0x9) {
		/* 0-9 convert to '0'-'9' (0x30-0x39) */
		return 0x30 + value;
	} else if (value <= 0xF) {
		/* 10-15 convert to 'A'-'F' (0x41-0x46) */
		return 0x41 + (value - 0xA);
	} else {
		/* error '0' */
		return 0x30;
	}
}

static void usbd_cdc_ecm_mac_to_string(u8 *mac, char *mac_str)
{
	u8 str_index = 0;
	u8 i;

	for (i = 0; i < 6; i++) {
		/* high 4 bits */
		mac_str[str_index++] = usbh_cdc_ecm_char_to_hex((mac[i] >> 4) & 0x0F);

		/* high 4 bits */
		mac_str[str_index++] = usbh_cdc_ecm_char_to_hex(mac[i] & 0x0F);
	}

	mac_str[str_index] = '\0';
}

static void usbd_cdc_ecm_set_dongle_mac(u8 *mac)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;

	if (NULL == mac) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "No Param\n");
		return ;
	}

	memcpy((void *) & (ecm->mac[0]), (void *)mac, CDC_ECM_MAC_STR_LEN);
	ecm->mac_src_type = CDC_ECM_MAC_UPPER_LAYER_SET;

	ecm->mac_valid = 1;
}

static int cdc_ecm_bulk_tx_status_check(void)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	if (ecm->bulk_tx_block) {
		//release the sema to return usbd_cdc_ecm_transmit
		do {
			usb_os_sema_give(ecm->bulk_tx_sema);
			usb_os_sleep_ms(1U);
		} while (ecm->bulk_tx_block);
	}

	return HAL_OK;
}

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
 * @brief Process notification state machine
 */
static int cdc_ecm_send_notification(void)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	usb_cdc_ecm_notify_t event;
	int status;
	u16 length;

	event.bmRequestType = 0xA1;
	event.wIndex = USBD_CDC_ECM_DATA_INTERFACE_NUM;

	switch (ecm->notify_state) {
	case ECM_NOTIFY_CONNECT:
		event.bNotificationCode = USB_CDC_ECM_NOTIFY_NETWORK_CONNECTION;
		event.wValue = ecm->connect_status;
		event.wLength = 0;
		length = USB_CDC_ECM_NETWORK_CONNECTION_SIZE;
		ecm->notify_state = ECM_NOTIFY_SPEED;
		break;

	case ECM_NOTIFY_SPEED:
		event.bNotificationCode = USB_CDC_ECM_NOTIFY_CONNECTION_SPEED_CHANGE;
		event.wValue = 0;
		event.wLength = 8;
		event.data.DLBitRate = 0; /* Downstream bits/sec */
		event.data.ULBitRate = 0; /* Upstream bits/sec */
		length = USB_CDC_ECM_CONNECTION_SPEED_CHANGE_SIZE;
		ecm->notify_state = ECM_NOTIFY_CONNECT;
		break;

	case ECM_NOTIFY_NONE:
	default:
		return HAL_ERR_PARA;
	}

	status = cdc_ecm_intr_in_send(&event, length);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Notify fail %d\n", status);
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

	return HAL_OK;
}

/**
 * @brief Handle CDC ECM SETUP requests
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

	switch (req_type) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ecm->interface_id = req->wIndex;

				if (ecm->interface_id == USBD_CDC_ECM_DATA_INTERFACE_NUM) {
					/* Trigger network connection notification */
					ecm->notify_state = ECM_NOTIFY_SPEED;
					ecm->connect_status = 1;
					cdc_ecm_send_notification();
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
				ep0_in->xfer_buf[0] = ecm->interface_id;
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
				/* Host-to-Device with data stage */
				ret = ecm->cb->setup(req, ep0_in->xfer_buf);
				if (ret == HAL_OK) {
					ep0_in->xfer_len = req->wLength;
					usbd_ep_transmit(dev, ep0_in);
				}
			} else {
				/* Device-to-Host with data stage */
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

	if (ep_addr == USBD_CDC_ECM_BULK_IN_EP) {
		ep_bulk_in->xfer_state = 0U;
		usb_os_sema_give(ecm->bulk_tx_sema);
		if (status != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "TX fail %d\n", status);
		}
	} else if (ep_addr == USBD_CDC_ECM_INTR_IN_EP) {
		ep_intr_in->xfer_state = 0U;
		cdc_ecm_send_notification(); /* Continue notification state machine */
		if (status != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Notify TX fail %d\n", status);
		}
	}

	return HAL_OK;
}

/**
 * @brief Handle EP data OUT completion
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

	/* Notify application */
	if (len > 0) {
		cdc_ecm_bulk_receive(ep_bulk_out->xfer_buf, len);
	}

	/* Continue receiving */
	return usbd_ep_receive(ecm->dev, ep_bulk_out);
}

/**
 * @brief Handle EP0 data OUT phase
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
			usbd_cdc_ecm_mac_to_string((u8 *)(ecm->mac), mac_buf);
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
 * @param dev: USB device instance
 * @param old_status: Previous status
 * @param status: Current status
 */
static void cdc_ecm_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;

	UNUSED(dev);

	if (ecm->cb && ecm->cb->status_changed) {
		ecm->cb->status_changed(old_status, status);
	}
}

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

	usb_os_sema_create(&(ecm->bulk_tx_sema));

	/* BULK IN use the caller buffer */
	info = &ep_bulk_in->info;
	info->addr = USBD_CDC_ECM_BULK_IN_EP;
	info->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_in->xfer_buf_len = 0;
	ep_bulk_in->xfer_buf = NULL;

	/* Allocate BULK OUT buffer */
	info = &ep_bulk_out->info;
	info->addr = USBD_CDC_ECM_BULK_OUT_EP;
	info->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_out->xfer_buf_len = USBD_CDC_ECM_BULK_BUF_MAX_SIZE;
	ep_bulk_out->xfer_buf = (u8 *)usb_os_malloc(ep_bulk_out->xfer_buf_len);
	ep_bulk_out->xfer_len = ep_bulk_out->xfer_buf_len;
	if (ep_bulk_out->xfer_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc BULK OUT buf fail\n");
		ret = HAL_ERR_MEM;
		goto exit;
	}

	/* Allocate INTR IN buffer */
	info = &ep_intr_in->info;
	info->addr = USBD_CDC_ECM_INTR_IN_EP;
	info->type = USB_CH_EP_TYPE_INTR;
	ep_intr_in->xfer_buf_len = USB_CDC_ECM_INTR_IN_PACKET_SIZE;
	ep_intr_in->xfer_buf = (u8 *)usb_os_malloc(ep_intr_in->xfer_buf_len);
	if (ep_intr_in->xfer_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc INTR IN buf fail\n");
		ret = HAL_ERR_MEM;
		goto cleanup_bulk_out;
	}

	/* Initialize user callbacks */
	if (cb != NULL) {
		ecm->cb = cb;

		if ((cb->priv != NULL) && (cb->priv->mac_value)) {
			usbd_cdc_ecm_set_dongle_mac(cb->priv->mac_value);
		} else {
			usbd_cdc_ecm_set_dongle_mac((u8 *)ecm_mac);
		}

		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "User init fail: %d\n", ret);
				goto cleanup_intr_in;
			}
		}
	}

	/* Register CDC ECM class driver */
	usbd_register_class(&usbd_cdc_driver);

	return HAL_OK;

cleanup_intr_in:
	usb_os_mfree(ep_intr_in->xfer_buf);
	ep_intr_in->xfer_buf = NULL;

cleanup_bulk_out:
	usb_os_mfree(ep_bulk_out->xfer_buf);
	ep_bulk_out->xfer_buf = NULL;

exit:
	usb_os_sema_delete(ecm->bulk_tx_sema);

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

	ecm->connect_status = 0;

	/* Wait for ongoing transfers done */
	while (ep_bulk_in->is_busy || ep_intr_in->is_busy) {
		usb_os_delay_us(100);
	}

	/* Unregister class driver */
	usbd_unregister_class();

	cdc_ecm_bulk_tx_status_check();
	usb_os_sema_delete(ecm->bulk_tx_sema);

	/* Call user deinit */
	if (ecm->cb && ecm->cb->deinit) {
		ecm->cb->deinit();
	}

	/* Free buffers */
	if (ep_intr_in->xfer_buf != NULL) {
		usb_os_mfree(ep_intr_in->xfer_buf);
		ep_intr_in->xfer_buf = NULL;
	}

	if (ep_bulk_out->xfer_buf != NULL) {
		usb_os_mfree(ep_bulk_out->xfer_buf);
		ep_bulk_out->xfer_buf = NULL;
	}

	return HAL_OK;
}

/**
  * @brief  Start to transmit data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @param  block: Whether block until xfer done
  * @retval Status
  */
int usbd_cdc_ecm_transmit(u8 *buf, u32 len, u8 block)
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;
	u8 retry_cnt = 0;
	int ret;

#if USBD_ECM_TX_SPEED_CHECK
	static u64 usb_tx_start_time = 0, usb_tx_end_time, usb_tx_interval_time;
	static u64 usb_tx_total_len = 0;

	usb_tx_total_len += len;
	if (usb_tx_start_time == 0) {
		usb_tx_start_time = usb_os_get_timestamp_ms();
	}
#endif

	while (1) {
		ret = cdc_ecm_bulk_send(buf, len);
		if (ret == HAL_OK) {
			//success
			break;
		}
		if (++retry_cnt > 100) { //100ms
			RTK_LOGS(TAG, RTK_LOG_ERROR, "TX drop(%d)\n", len);
			ret = HAL_ERR_UNKNOWN;
			break;
		} else {
			usb_os_delay_us(1);
		}
	}

	//wait cdc_ecm_handle_ep_data_in to give the sema
	if ((ret == HAL_OK) && block) {
		ecm->bulk_tx_block = 1;
		usb_os_sema_take(ecm->bulk_tx_sema, USB_OS_SEMA_TIMEOUT);
#if USBD_ECM_TX_SPEED_CHECK
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
	}

	ecm->bulk_tx_block = 0;

	return ret;
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
int usbd_cdc_ecm_get_connect_status(void)//1 up
{
	usbd_cdc_ecm_dev_t *ecm = &usbd_cdc_ecm_dev;

	return ecm->connect_status;
}

__attribute__((weak))
int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbd_cdc_ecm_transmit(buf, len, block);
}
