/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usb_os.h"
#include "usbh_cdc_ecm.h"
#if USBH_CDC_ECM_STATE_TRACE_ENABLE
#include <stdint.h>
#include <inttypes.h>
#endif

/* Private defines -----------------------------------------------------------*/

#define USBH_CDC_ECM_ENABLE_DUMP_DESCRIPTOR_PARSE    0

#define USBH_CDC_ECM_BULK_BUF_MAX_SIZE               (512*3)

#define USBH_CDC_ECM_DEVICE_VID                      USB_VID  /**< Default Vendor ID. */

#define USBH_CDC_ECM_RX_SPEED_CHECK                  0     /* CDC ECM rx speed test */
#define USBH_CDC_ECM_TX_SPEED_CHECK                  0     /* CDC ECM tx speed test */

#define USBH_CDC_ECM_ENABLE_PACKETFILTER             0

#define USBH_CDC_ECM_QUECTEL_DONGLE_VID             (0x2C7C)
#define USBH_CDC_ECM_QUECTEL_DONGLE_EG915_PID       (0x0901)
#define USBH_CDC_ECM_QUECTEL_DONGLE_EG91_PID        (0x0191)

/* tick count */
/* in ms*/
#define USBH_CDC_ECM_BULK_OUT_IDLE_MAX_CNT           8U * 50 //send total 16k data
#define USBH_CDC_ECM_BULK_OUT_BUSY_MAX_CNT           8U * 10

#define USBH_CDC_ECM_BULK_IN_IDLE_MAX_CNT            8U * 10  ///idle
#define USBH_CDC_ECM_BULK_IN_NAK_CHECK_MAX_CNT       8U * 5   /// 4*5 busy nak drop

/* intr_rx busy timeout: fixed count, independent of the device bInterval
 * (a tiny bInterval would otherwise resubmit before the device can reply). */
#define USBH_CDC_ECM_INTR_BUSY_MAX_CNT               (8U * 5)

/* ecm ethernet connect status check */
#define USBH_CDC_ECM_ETH_STATUS_CHECK_TIME           300U  //300ms

#define USBH_CDC_ECM_MAC_STRING_LEN                  (32)
/* iMACAddress string: 2-byte header + 12 hex digits in UTF-16LE (Ref ECM 1.2 5.4) */
#define USBH_CDC_ECM_MAC_DESC_LEN                    (26U)

/* CLEAR_FEATURE(ENDPOINT_HALT) attempts before the device is re-enumerated */
#define USBH_CDC_ECM_HALT_RETRY_MAX_CNT              3U

/* Upper bound for deinit to wait for the tasks inside usbh_cdc_ecm_send_data() to leave. A
 * woken caller leaves within microseconds, only a starved one takes longer. */
#define USBH_CDC_ECM_TX_EXIT_TIMEOUT_MS              500U

/* Private types -------------------------------------------------------------*/

typedef enum {
	CDC_ECM_CAP_RCR_CMD      = (BIT0),  /* Receive Control Register, enables UDP functionality */
	CDC_ECM_CAP_FIFO_CTRL    = (BIT1),  /* FIFO Control, controls the water level of the transmit and receive FIFOs */
	CDC_ECM_CAP_MODIFY_MAC   = (BIT2),  /* MAC Settings, enables the function of setting MAC address */
	CDC_ECM_CAP_MODIFY_LED   = (BIT3),  /* LED Settings, enables the function of setting LED value */
	CDC_ECM_CAP_MAX,
} usbh_cdc_ecm_dongle_capability_t;

typedef enum {
	CDC_ECM_MAC_UNINIT = 0U,
	CDC_ECM_MAC_DONGLE_SUPPLY,
	CDC_ECM_MAC_UPPER_LAYER_SET,
	CDC_ECM_MAC_RANDOM_SET,
	CDC_ECM_MAC_TYPE_MAX,
} usbh_cdc_ecm_dongle_mac_type_t;

typedef enum {
	CDC_ECM_STATE_AT_SETTING_IDLE = 0U,
	CDC_ECM_STATE_GET_MAC_STR,
	CDC_ECM_STATE_CTRL_ALT_SETTING,

#if USBH_CDC_ECM_ENABLE_PACKETFILTER
	CDC_ECM_STATE_CTRL_SET_ETHERNET_MULTICAST_FILTER,
	CDC_ECM_STATE_CTRL_SET_ETHERNET_PACKET_FILTER,
	CDC_ECM_STATE_CTRL_GET_ETHERNET_STATISTIC,
#endif

	CDC_ECM_STATE_CTRL_RCR_GET,
	CDC_ECM_STATE_CTRL_RCR_SET,
	CDC_ECM_STATE_FLOW_CTRL1,
	CDC_ECM_STATE_FLOW_CTRL2,

	/*RTL8152, set mac address */
	CDC_ECM_STATE_CTRL_MAC_GET_LOCK,
	CDC_ECM_STATE_CTRL_MAC_DISABLE_LOCK,
	CDC_ECM_STATE_CTRL_MAC_SET_MAC1,
	CDC_ECM_STATE_CTRL_MAC_SET_MAC2,
	CDC_ECM_STATE_CTRL_MAC_ENABLE_LOCK,
	/*RTL8152, set led color */
	CDC_ECM_STATE_CTRL_LED_COLOR_SET,

	CDC_ECM_STATE_AT_SETTING_MAX,
} usbh_cdc_ecm_at_set_state_t;

/* CDC ECM state */
typedef enum {
	CDC_ECM_STATE_IDLE = 0U,
	CDC_ECM_STATE_TRANSFER,
	CDC_ECM_STATE_ERROR,
} usbh_cdc_ecm_state_t;

typedef struct {
	u16 pid;
	u16 feature;
} __PACKED usbh_cdc_ecm_priv_config_t;

/* Private macros ------------------------------------------------------------*/

#define  USBH_CDC_ECM_FREE_MEM(x)  do { if ((x) != NULL) { usb_os_mfree((void *)(x)); (x) = NULL; } } while (0)

/* Private function prototypes -----------------------------------------------*/

#if USBH_CDC_ECM_STATE_TRACE_ENABLE
static u32 usbh_cdc_ecm_cmd_entry(u16 argc, u8 *argv[]);
#endif

static int usbh_cdc_ecm_attach(usb_host_t *host);
static void usbh_cdc_ecm_detach(usb_host_t *host);
static void usbh_cdc_ecm_process(usb_host_t *host, usbh_drv_msg_t *msg);
static int usbh_cdc_ecm_setup(usb_host_t *host);
static void usbh_cdc_ecm_sof(usb_host_t *host);
static void usbh_cdc_ecm_process_bulk_out(usb_host_t *host);
static void usbh_cdc_ecm_process_bulk_in(usb_host_t *host);
static void usbh_cdc_ecm_process_intr_in(usb_host_t *host);
static int usbh_cdc_ecm_bulk_receive(void);
static int usbh_cdc_ecm_bulk_tx(void);
static int usbh_cdc_ecm_cb_intr_receive(u8 *buf, u32 length);
static int usbh_cdc_ecm_cb_bulk_send(int state);
static int usbh_cdc_ecm_intr_receive(void);
static int usbh_cdc_ecm_tx_status_check(void);
static int usbh_cdc_ecm_bulk_send(u8 *buf, u32 len);
static void usbh_cdc_ecm_deinit_all_pipe(void);
static void usbh_cdc_ecm_halt(usb_host_t *host, const usbh_pipe_t *pipe);
static void usbh_cdc_ecm_process_halt(usb_host_t *host);
#if USBH_CDC_ECM_STATE_TRACE_ENABLE
static void usbh_cdc_ecm_trace_task_init(void);
static void usbh_cdc_ecm_trace_task_deinit(void);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "ECMH";

static const usbh_cdc_ecm_priv_config_t usbh_cdc_ecm_rtk_dongle[] = {
	{0x8152, CDC_ECM_CAP_FIFO_CTRL | CDC_ECM_CAP_MODIFY_MAC | CDC_ECM_CAP_MODIFY_LED},
	{0x8153, CDC_ECM_CAP_RCR_CMD | CDC_ECM_CAP_MODIFY_MAC | CDC_ECM_CAP_MODIFY_LED},
	{0x8156, CDC_ECM_CAP_RCR_CMD | CDC_ECM_CAP_MODIFY_MAC | CDC_ECM_CAP_MODIFY_LED},

	{0, 0x00},
};

/* USB CDC ECM device identification */
static const usbh_dev_id_t usbh_cdc_ecm_devs[] = {
	{
		/* Protocol 0x00 is the only one ECM defines (Ref ECM 1.2 4.4), and the one attach() looks
		 * the interface up with: claiming any other would keep it from its own driver for nothing. */
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO,
		.bInterfaceClass = USB_CDC_CLASS_CODE,
		.bInterfaceSubClass = USB_CDC_ECM_SUBCLASS_ECM,
		.bInterfaceProtocol = 0x00U,
	},
	{
	},
};

/* USB Host CDC ECM class driver */
static const usbh_class_driver_t usbh_cdc_ecm_driver = {
	.id_table = usbh_cdc_ecm_devs,
	.attach  = usbh_cdc_ecm_attach,
	.detach  = usbh_cdc_ecm_detach,
	.setup   = usbh_cdc_ecm_setup,
	.process = usbh_cdc_ecm_process,
	.sof = usbh_cdc_ecm_sof,
};

#if USBH_CDC_ECM_STATE_TRACE_ENABLE
static rtos_task_t 	            usbh_cdc_ecm_trace_task;
static u8                       usbh_cdc_ecm_trace_task_init_flag = 0;
#endif

static usbh_cdc_ecm_host_t usbh_cdc_ecm_host;

/* Exported variables --------------------------------------------------------*/

#if USBH_CDC_ECM_STATE_TRACE_ENABLE
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbh_cdc_ecm_cmd_table[] = {
	{"ecmh", usbh_cdc_ecm_cmd_entry},
};
#endif /* USBH_CDC_ECM_STATE_TRACE_ENABLE */

/* Private functions ---------------------------------------------------------*/

/* USB basic functions */

static u8 usbh_cdc_ecm_support_feature(u16 pid, usbh_cdc_ecm_dongle_capability_t type)
{
	const usbh_cdc_ecm_priv_config_t *ecm_dongle;
	u16 i;

	for (i = 0; usbh_cdc_ecm_rtk_dongle[i].pid != 0U; i++) {
		ecm_dongle = &usbh_cdc_ecm_rtk_dongle[i];
		if (pid != ecm_dongle->pid) {
			continue;
		}

		if ((ecm_dongle->feature & (u16)type) != 0U) {
			return HAL_OK;
		} else {
			return HAL_ERR_PARA;
		}
	}

	return HAL_ERR_PARA;
}

static u16 usbh_cdc_ecm_get_eth_status_check_time(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	if ((cdc->host != NULL) && (cdc->host->dev_speed == USB_SPEED_HIGH)) {
		return 8 * USBH_CDC_ECM_ETH_STATUS_CHECK_TIME;
	}

	return USBH_CDC_ECM_ETH_STATUS_CHECK_TIME;
}

/**
  * @brief  Convert one hexadecimal digit to its value
  * @param  hex_num: ASCII character
  * @retval 0x0 to 0xF, 0xFF when the character is not a hexadecimal digit
  */
static u8 usbh_cdc_ecm_hex_to_char(u8 hex_num)
{
	u8 val;

	if ((hex_num >= 0x30U) && (hex_num <= 0x39U)) {
		val = hex_num - 0x30U;
	} else if ((hex_num >= 0x41U) && (hex_num <= 0x46U)) {
		val = 0xAU + (hex_num - 0x41U);
	} else if ((hex_num >= 0x61U) && (hex_num <= 0x66U)) {
		val = 0xAU + (hex_num - 0x61U);
	} else {
		val = 0xFFU;
	}

	return val;
}

/**
  * @brief  Check whether the device has completed enumeration.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @return HAL_OK if enumeration is complete, HAL_BUSY otherwise.
  */
static int usbh_cdc_ecm_check_enum_status(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	if ((cdc->host != NULL) && (cdc->host->connect_state >= USBH_STATE_SETUP)) {
		return HAL_OK;
	}

	return HAL_BUSY;
}

//add parse to get the ethernet status
static int usbh_cdc_ecm_cb_intr_receive(u8 *buf, u32 length)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	if (buf && length >= 8) {
		/*A1 00 00 00 01 00 00 00 */
		if (length == 8 && buf[0] == 0xA1 && buf[1] == USB_CDC_NOTIFY_NETWORK_CONNECTION) {
			cdc->eth_hw_connect = buf[2];
			if (cdc->eth_hw_connect && (cdc->intr_check_tick != usbh_cdc_ecm_get_eth_status_check_time())) {
				cdc->intr_check_tick = usbh_cdc_ecm_get_eth_status_check_time();
			} else if ((cdc->eth_hw_connect == 0) && (cdc->intr_rx.pipe.ep_interval != 0)) {
				cdc->intr_check_tick = cdc->intr_rx.pipe.ep_interval;
			}
			cdc->intr_rx.pipe.max_timeout_tick = cdc->intr_check_tick;
		}
		/*A1 2A 00 00 01 00 00 08 	00 00 00 00 00 00 00 00 */
		else if (length == 16 && buf[0] == 0xA1 && buf[1] == USB_CDC_NOTIFY_CONNECTION_SPEED_CHANGE) {
			//usbh_cdc_ecm_speed_change_t
		}
	}

	return HAL_OK;
}

/* USB CDC ECM control functions */

/**
  * @brief  Get string descriptor by str id
  * @param  host: Host handle
  * @param  pbuf: buffer handle which will save the string value
  * @param  buflen: buffer length
  * @param  str_id: string index
  * @retval Status
  */
static int usbh_cdc_ecm_process_get_string(usb_host_t *host, u8 *pbuf, u16 buflen, u8 str_id)
{
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD;
	setup.req.bRequest = USB_REQ_GET_DESCRIPTOR;
	setup.req.wValue = USB_DESC_STRING | str_id;
	/* wIndex of a string-descriptor request is a LANGID the device supports, Ref USB 2.0 9.6.7 */
	setup.req.wIndex = usbh_get_lang_id(host);
	setup.req.wLength = buflen;

	return usbh_ctrl_request(host, &setup, pbuf);
}

/**
  * @brief  Use a random MAC for a device that supplies no usable one
  * @note   Locally administered unicast (Ref IEEE 802 9.2): I/G bit 0 so it is never a group address, U/L bit 1 so
  *         it can not clash with a vendor assigned one.
  * @retval None
  */
static void usbh_cdc_ecm_set_random_mac(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	TRNG_get_random_bytes(cdc->mac, USBH_CDC_ECM_MAC_STR_LEN);
	cdc->mac[0] = (u8)((cdc->mac[0] & 0xFEU) | 0x02U);
	cdc->mac_src_type = CDC_ECM_MAC_RANDOM_SET;
	cdc->mac_valid = 1U;
	RTK_LOGS(TAG, RTK_LOG_WARN, "Random mac[%02x %02x %02x %02x %02x %02x]\n", cdc->mac[0], cdc->mac[1], cdc->mac[2], cdc->mac[3], cdc->mac[4],
			 cdc->mac[5]);
}

/**
  * @brief  Decode the iMACAddress string descriptor in dongle_ctrl_buf
  * @note   Ref ECM 1.2 5.4: 12 hexadecimal digits, most significant first, in UTF-16LE. A short or foreign descriptor,
  *         a non hexadecimal digit, or an all-zero address is no MAC.
  * @param  host: Host handle
  * @retval HAL_OK when cdc->mac holds the device MAC, HAL_ERR_PARA otherwise
  */
static int usbh_cdc_ecm_parse_mac_str(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	const u8 *buf = cdc->dongle_ctrl_buf;
	u8 mac[USBH_CDC_ECM_MAC_STR_LEN];
	u8 hi;
	u8 lo;
	u8 nonzero = 0U;
	u8 i;

	if ((usbh_get_last_ctrl_in_size(host) < USBH_CDC_ECM_MAC_DESC_LEN) || (buf[0] < USBH_CDC_ECM_MAC_DESC_LEN) ||
		(buf[1] != USB_DESC_TYPE_STRING)) {
		return HAL_ERR_PARA;
	}

	/* Decoded aside, so a bad string leaves cdc->mac, possibly the user one, untouched */
	for (i = 0U; i < USBH_CDC_ECM_MAC_STR_LEN; i++) {
		hi = usbh_cdc_ecm_hex_to_char(buf[2U + (4U * i)]);
		lo = usbh_cdc_ecm_hex_to_char(buf[4U + (4U * i)]);
		if ((hi > 0xFU) || (lo > 0xFU)) {
			return HAL_ERR_PARA;
		}
		mac[i] = (u8)((hi << 4U) | lo);
		nonzero |= mac[i];
	}

	if (nonzero == 0U) {
		return HAL_ERR_PARA;
	}

	usb_os_memcpy((void *)cdc->mac, (const void *)mac, USBH_CDC_ECM_MAC_STR_LEN);
	return HAL_OK;
}

/**
  * @brief  Get mac string descriptor
  * @note   A device that refuses the string, or returns no usable MAC, gets a random one rather than being retried:
  *         the answer does not change, and retrying only ran into the class setup timeout that drops the class.
  * @param  host: Host handle
  * @retval HAL_OK when the MAC is settled, HAL_BUSY while the request is in flight
  */
static int usbh_cdc_ecm_get_mac_str(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	int status;

	if ((cdc->mac_src_type == CDC_ECM_MAC_UPPER_LAYER_SET) && (cdc->vid == USBH_CDC_ECM_DEVICE_VID) &&
		(usbh_cdc_ecm_support_feature(cdc->pid, CDC_ECM_CAP_MODIFY_MAC) == HAL_OK)) {
		/* Programmed into the dongle later, see usbh_cdc_ecm_config_dongle_mac() */
		RTK_LOGS(TAG, RTK_LOG_INFO,  "Upper set mac[%02x %02x %02x %02x %02x %02x]\n", cdc->mac[0], cdc->mac[1], cdc->mac[2], cdc->mac[3], cdc->mac[4],
				 cdc->mac[5]);
		return HAL_OK;
	}

	/* String index 0 names no string but the LANGID table (Ref USB 2.0 9.6.7). A MAC the user supplied is kept. */
	if (cdc->iMACAddressStringId == 0U) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "No MAC str\n");
		if (cdc->mac_src_type == CDC_ECM_MAC_UPPER_LAYER_SET) {
			cdc->mac_valid = 1U;
		} else {
			usbh_cdc_ecm_set_random_mac();
		}
		return HAL_OK;
	}

	status = usbh_cdc_ecm_process_get_string(host, cdc->dongle_ctrl_buf, USBH_CDC_ECM_MAC_STRING_LEN, cdc->iMACAddressStringId);
	if (status == HAL_BUSY) {
		return status;
	}

	if ((status == HAL_OK) && (usbh_cdc_ecm_parse_mac_str(host) == HAL_OK)) {
		cdc->mac_src_type = CDC_ECM_MAC_DONGLE_SUPPLY;
		cdc->mac_valid = 1U;
		RTK_LOGS(TAG, RTK_LOG_INFO,  "Dongle mac[%02x %02x %02x %02x %02x %02x]\n", cdc->mac[0], cdc->mac[1], cdc->mac[2], cdc->mac[3], cdc->mac[4],
				 cdc->mac[5]);
	} else if (cdc->mac_src_type == CDC_ECM_MAC_UPPER_LAYER_SET) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Get MAC str %d err %d, use upper\n", cdc->iMACAddressStringId, status);
		cdc->mac_valid = 1U;
	} else {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Get MAC str %d err %d\n", cdc->iMACAddressStringId, status);
		usbh_cdc_ecm_set_random_mac();
	}

	return HAL_OK;
}

/**
  * @brief  Usb alt setting function
  * @param  host: Host handle
  * @retval Status
*/
static int usbh_cdc_ecm_process_set_alt(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_REQ_SET_INTERFACE;
	setup.req.wValue = cdc->data_alt_set;
	setup.req.wIndex = cdc->data_itf_id;
	setup.req.wLength = 0U;

	return usbh_ctrl_request(host, &setup, NULL);
}

#if USBH_CDC_ECM_ENABLE_PACKETFILTER
/**
  * @brief  Set Ethernet Packet Filter
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_set_packet_filter(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_SET_ETHERNET_PACKET_FILTER;
	setup.req.wValue = cdc->packet_filter;
	setup.req.wIndex = 1U;
	setup.req.wLength = 0;

	return usbh_ctrl_request(host, &setup, NULL);
}

/**
  * @brief  Set Ethernet Multicast Filter
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_set_multicast_filter(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_SET_ETHERNET_MULTICAST_FILTERS;
	setup.req.wValue = 1;
	setup.req.wIndex = 1U;
	setup.req.wLength = cdc->multicast_filter_len;

	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

/**
  * @brief  Get Ethernet Statistic
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_get_statistic(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_GET_ETHERNET_STATISTIC;
	setup.req.wValue = cdc->feature_selector;
	setup.req.wIndex = 1U;
	setup.req.wLength = 4;

	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}
#endif

//for 8156
/**
  * @brief  Get 8156 rcr params
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_get_rcr(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xc010;
	setup.req.wIndex = 0x0100;
	setup.req.wLength = 4;

	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

/**
  * @brief  Set 8156 rcr params
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_set_rcr(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xc010;
	setup.req.wIndex = 0x010F;
	setup.req.wLength = 4;

	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

//for 8152
/**
  * @brief  Set 8152 flow ctrl params1
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_set_flow_ctrl1(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xC0A4;
	setup.req.wIndex = 0x0103;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, (u8 *)cdc->dongle_ctrl_buf);
}
/**
  * @brief  Set 8152 flow ctrl params2
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_set_flow_ctrl2(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xC0A8;
	setup.req.wIndex = 0x0103;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, (u8 *)cdc->dongle_ctrl_buf);
}

/******************Set MAC for 8152 *****************************/
/**
  * @brief  Set 8152 mac flow ctrl
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_mac_get_lock(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xE81C;
	setup.req.wIndex = 0x010F;
	setup.req.wLength = 4;

	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

static int usbh_cdc_ecm_process_mac_set_dis_lock(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xE81C;
	setup.req.wIndex = 0x010F;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

static int usbh_cdc_ecm_process_mac_set_mac1(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xC000;
	setup.req.wIndex = 0x010F;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

static int usbh_cdc_ecm_process_mac_set_mac2(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xC004;
	setup.req.wIndex = 0x0103;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

static int usbh_cdc_ecm_process_mac_en_lock(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xE81C;
	setup.req.wIndex = 0x010F;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

/**
  * @brief  Set led color for 8152
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_led_set_ctrl(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xDD90;
	setup.req.wIndex = 0x0103;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

static void usbh_cdc_ecm_set_dongle_mac(const u8 *mac)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	if (NULL == mac) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Param error, mac is NULL\n");
		return ;
	}

	usb_os_memcpy((void *) & (cdc->mac[0]), (const void *)mac, USBH_CDC_ECM_MAC_STR_LEN);
	cdc->mac_src_type = CDC_ECM_MAC_UPPER_LAYER_SET;
}

static void usbh_cdc_ecm_set_dongle_led_array(const u16 *led, u8 len)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	if (led == NULL || len == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Param error,led is NULL or len %d\n", len);
		return ;
	}

	USBH_CDC_ECM_FREE_MEM(cdc->led_array);

	cdc->led_array = (u16 *)usb_os_malloc(len * sizeof(u16));
	if (cdc->led_array == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc led array fail\n");
		cdc->led_cnt = 0;
		return ;
	}
	usb_os_memcpy((void *)cdc->led_array, (const void *)led, len * sizeof(u16));

	cdc->led_cnt = len;
}

static void usbh_cdc_ecm_config_dongle_mac(usb_host_t *host, u16 vid, u16 pid)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	u8 state = HAL_OK;

	if ((vid != USBH_CDC_ECM_DEVICE_VID) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY) ||
		(usbh_cdc_ecm_support_feature(pid, CDC_ECM_CAP_MODIFY_MAC) != HAL_OK)) {
		return;
	}

	switch (cdc->sub_status) {
	case CDC_ECM_STATE_CTRL_MAC_GET_LOCK: //8152 mac
		usb_os_memset((void *)cdc->mac_ctrl_lock, 0, 4);
		state = usbh_cdc_ecm_process_mac_get_lock(host);
		if (state == HAL_OK) {
			usb_os_memcpy((void *)cdc->mac_ctrl_lock, (const void *)cdc->dongle_ctrl_buf, USBH_CDC_ECM_CTRL_REG_BUF_LEN);
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get MAC lock err\n");
			usb_os_sleep_ms(100);
		}

		break;

	case CDC_ECM_STATE_CTRL_MAC_DISABLE_LOCK: //8152 mac
		cdc->mac_ctrl_lock[0] = 0xD0;
		usb_os_memcpy((void *)cdc->dongle_ctrl_buf, (const void *)cdc->mac_ctrl_lock, USBH_CDC_ECM_CTRL_REG_BUF_LEN);
		state = usbh_cdc_ecm_process_mac_set_dis_lock(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Dis MAC lock err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_SET_MAC1: //8152 mac
		usb_os_memcpy((void *)cdc->dongle_ctrl_buf, (const void *) & (cdc->mac[0]), USBH_CDC_ECM_CTRL_REG_BUF_LEN);
		state = usbh_cdc_ecm_process_mac_set_mac1(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set MAC1 err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_SET_MAC2: //8152 mac
		cdc->dongle_ctrl_buf[0] = cdc->mac[4];
		cdc->dongle_ctrl_buf[1] = cdc->mac[5];
		cdc->dongle_ctrl_buf[2] = 0xFF;
		cdc->dongle_ctrl_buf[3] = 0xFF;
		state = usbh_cdc_ecm_process_mac_set_mac2(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set MAC2 err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_ENABLE_LOCK: //8152 mac
		cdc->mac_ctrl_lock[0] = 0x10;
		usb_os_memcpy((void *)cdc->dongle_ctrl_buf, (const void *)cdc->mac_ctrl_lock, USBH_CDC_ECM_CTRL_REG_BUF_LEN);
		state = usbh_cdc_ecm_process_mac_en_lock(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
			cdc->mac_valid = 1;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Mac set success \n");
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "En MAC lock err\n");
			usb_os_sleep_ms(100);
		}
		break;

	default:
		cdc->sub_status++;
		break;
	}
}

static int usbh_cdc_ecm_ctrl_setting(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	/* HAL_BUSY = sequence still running; only the default case (sub_status past
	 * the last step) reports HAL_OK. Must not be an error code: the core treats
	 * any non-OK, non-BUSY setup() status as terminal and drops the class. */
	u8 ret_state = HAL_BUSY;
	u8 state = HAL_OK;
	u16 vid = cdc->vid;
	u16 pid = cdc->pid;

#if USBH_CDC_ECM_ENABLE_PACKETFILTER
	u8 mac_str[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif

	switch (cdc->sub_status) {
	case CDC_ECM_STATE_AT_SETTING_IDLE:
		/* No Ethernet Networking Functional Descriptor: iMACAddressStringId stays 0, which GET_MAC_STR settles
		 * with a random MAC (or the user one), so the sequence never parks here */
		RTK_LOGS(TAG, RTK_LOG_WARN, "No eth func desc\n");
		cdc->sub_status = CDC_ECM_STATE_GET_MAC_STR;
		break;

	case CDC_ECM_STATE_GET_MAC_STR:
		/* Quectel EG91 does not provide iMACAddress in its ECM functional
		 * descriptor (iMACAddress = 0), so GET_DESCRIPTOR(STRING) would fail
		 * or return garbage. The MAC is instead supplied via a proprietary AT
		 * command (AT+QNETDEVMAC) at the example layer. Skip this state so the
		 * ctrl_setting sequence can continue without stalling. */
		if ((vid == USBH_CDC_ECM_QUECTEL_DONGLE_VID) && (pid == USBH_CDC_ECM_QUECTEL_DONGLE_EG91_PID)) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "EG91:skip GET_MAC_STR\n");
			cdc->sub_status++;
			break;
		}
		/* Returns HAL_OK once the MAC is settled, a random one on any failure */
		if (usbh_cdc_ecm_get_mac_str(host) == HAL_OK) {
			cdc->sub_status++;
		}
		break;

	case CDC_ECM_STATE_CTRL_ALT_SETTING:
		state = usbh_cdc_ecm_process_set_alt(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
			// cdc->sub_status = CDC_ECM_STATE_AT_SETTING_MAX; //force to jump
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM alt setting err\n");
			usb_os_sleep_ms(100);
		}
		break;

#if USBH_CDC_ECM_ENABLE_PACKETFILTER
	case CDC_ECM_STATE_CTRL_SET_ETHERNET_MULTICAST_FILTER:
		usb_os_memcpy((void *)cdc->multicast_filter, (const void *)mac_str, 6);
		cdc->multicast_filter_len = 6;
		state = usbh_cdc_ecm_process_set_multicast_filter(host);
		if (state == HAL_OK) {
			usb_os_memcpy((void *)cdc->multicast_filter, (const void *)cdc->dongle_ctrl_buf, cdc->multicast_filter_len);
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM set eth multicast filter err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_SET_ETHERNET_PACKET_FILTER:
		cdc->packet_filter = CDC_ECM_ETH_PACKET_TYPE_DIRECTED;
		state = usbh_cdc_ecm_process_set_packet_filter(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM set eth packet filter err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_GET_ETHERNET_STATISTIC:
		state = usbh_cdc_ecm_process_get_statistic(host);
		if (state == HAL_OK) {
			usb_os_memcpy((void *) & (cdc->eth_statistic_count), (const void *)cdc->dongle_ctrl_buf, USBH_CDC_ECM_CTRL_REG_BUF_LEN);
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM get eth statistic err\n");
			usb_os_sleep_ms(100);
		}
		break;
#endif

	case CDC_ECM_STATE_CTRL_RCR_GET:
		if ((vid != USBH_CDC_ECM_DEVICE_VID) || (usbh_cdc_ecm_support_feature(pid, CDC_ECM_CAP_RCR_CMD) != HAL_OK)) {
			cdc->sub_status++;
		} else {
			usb_os_memset((void *)cdc->rcr, 0, 4);
			state = usbh_cdc_ecm_process_get_rcr(host);
			if (state == HAL_OK) {
				usb_os_memcpy((void *)cdc->rcr, (const void *)cdc->dongle_ctrl_buf, USBH_CDC_ECM_CTRL_REG_BUF_LEN);
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get RCR err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_RCR_SET:
		if ((vid != USBH_CDC_ECM_DEVICE_VID) || (usbh_cdc_ecm_support_feature(pid, CDC_ECM_CAP_RCR_CMD) != HAL_OK)) {
			cdc->sub_status++;
		} else {
			//set bit 0~3 set 1 will enable PING & UDP transfer
			cdc->rcr[0] = cdc->rcr[0] | 0x0F;
			usb_os_memcpy((void *)cdc->dongle_ctrl_buf, (const void *)cdc->rcr, USBH_CDC_ECM_CTRL_REG_BUF_LEN);
			state = usbh_cdc_ecm_process_set_rcr(host);
			if (state == HAL_OK) {
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set RCR err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_FLOW_CTRL1:
		if ((vid != USBH_CDC_ECM_DEVICE_VID) || (usbh_cdc_ecm_support_feature(pid, CDC_ECM_CAP_FIFO_CTRL) != HAL_OK)) {
			cdc->sub_status++;
		} else {
			usb_os_memset((void *)cdc->flow_ctrl, 0, 4);
			cdc->flow_ctrl[0] = 0x60;
			usb_os_memcpy((void *)cdc->dongle_ctrl_buf, (const void *)cdc->flow_ctrl, USBH_CDC_ECM_CTRL_REG_BUF_LEN);
			state = usbh_cdc_ecm_process_set_flow_ctrl1(host);
			if (state == HAL_OK) {
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set flow ctrl1 err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_FLOW_CTRL2:
		if ((vid != USBH_CDC_ECM_DEVICE_VID) || (usbh_cdc_ecm_support_feature(pid, CDC_ECM_CAP_FIFO_CTRL) != HAL_OK)) {
			cdc->sub_status++;
		} else {
			usb_os_memset((void *)cdc->flow_ctrl, 0, 4);
			cdc->flow_ctrl[0] = 0xa0;
			usb_os_memcpy((void *)cdc->dongle_ctrl_buf, (const void *)cdc->flow_ctrl, USBH_CDC_ECM_CTRL_REG_BUF_LEN);
			state = usbh_cdc_ecm_process_set_flow_ctrl2(host);
			if (state == HAL_OK) {
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set flow ctrl2 err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_GET_LOCK:
	case CDC_ECM_STATE_CTRL_MAC_DISABLE_LOCK:
	case CDC_ECM_STATE_CTRL_MAC_SET_MAC1:
	case CDC_ECM_STATE_CTRL_MAC_SET_MAC2:
	case CDC_ECM_STATE_CTRL_MAC_ENABLE_LOCK:
		if ((vid != USBH_CDC_ECM_DEVICE_VID) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY) ||
			(usbh_cdc_ecm_support_feature(pid, CDC_ECM_CAP_MODIFY_MAC) != HAL_OK)) {
			cdc->sub_status = CDC_ECM_STATE_CTRL_LED_COLOR_SET;
		} else {
			usbh_cdc_ecm_config_dongle_mac(host, vid, pid);
		}
		break;

	case CDC_ECM_STATE_CTRL_LED_COLOR_SET:
		if ((vid != USBH_CDC_ECM_DEVICE_VID) || (cdc->led_cnt == 0) || (cdc->led_array == NULL) ||
			(usbh_cdc_ecm_support_feature(pid, CDC_ECM_CAP_MODIFY_LED) != HAL_OK)) {
			cdc->sub_status++;
		} else {
			usb_os_memset((void *)cdc->dongle_ctrl_buf, 0xFF, 4);
			usb_os_memcpy((void *)cdc->dongle_ctrl_buf, (const void *) & (cdc->led_array[0]), 2);
			state = usbh_cdc_ecm_process_led_set_ctrl(host);
			if (state == HAL_OK) {
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set led color err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	default:
		ret_state = HAL_OK;
		break;
	}

	return ret_state;
}

static void usbh_cdc_ecm_dump_ecm_cfgdesc(void)
{
#if USBH_CDC_ECM_ENABLE_DUMP_DESCRIPTOR_PARSE
	usbh_cdc_ecm_host_t   *cdc = &usbh_cdc_ecm_host;
	usbh_ep_desc_t        *ep_desc;
	usbh_cdc_ecm_pipe_info_t *pipe_info;

	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------Dump Descriptor Start------------------\n");

	//dump ecm control
	pipe_info = &(cdc->intr_rx);
	ep_desc = &(pipe_info->ep_desc);
	RTK_LOGS(TAG, RTK_LOG_INFO, "INTR addr(0x%x)MPS(%d)pipe(%d)interval(%d) MAC(%d)\n",
			 ep_desc->bEndpointAddress, ep_desc->wMaxPacketSize, pipe_info->pipe.pipe_num, pipe_info->pipe.ep_interval, cdc->iMACAddressStringId);

	//dump ecm data
	pipe_info = &(cdc->bulk_tx);
	ep_desc = &(pipe_info->ep_desc);
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK out:addr(0x%x)MPS(%d)pipe(%d)interval(%d)\n",
			 ep_desc->bEndpointAddress, ep_desc->wMaxPacketSize, pipe_info->pipe.pipe_num, pipe_info->pipe.ep_interval);

	pipe_info = &(cdc->bulk_rx);
	ep_desc = &(pipe_info->ep_desc);
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK in:addr(0x%x)MPS(%d)pipe(%d)interval(%d)\n",
			 ep_desc->bEndpointAddress, ep_desc->wMaxPacketSize, pipe_info->pipe.pipe_num, pipe_info->pipe.ep_interval);

	RTK_LOGS(TAG, RTK_LOG_INFO, "---------------------Dump End-----------------------------\n");
#endif
}

/**
  * @brief  Find the class-specific descriptor of one subtype in the comm interface
  * @note   Read only. raw_data comes straight from the device, so every descriptor is bounds-checked before it is
  *         read. bLength is the only way to the next descriptor, so a value below the header size or one running
  *         past the end leaves no resync point and ends the scan. A Union is only taken when its bControlInterface
  *         names this interface (Ref CDC 1.2 5.2.3.2): a composite device may carry another function's.
  * @param  itf_data: ECM comm interface data
  * @param  subtype: bDescriptorSubtype to look for
  * @param  min_len: Smallest bLength the caller reads
  * @retval The descriptor, NULL if absent or truncated
  */
static const u8 *usbh_cdc_ecm_find_func_desc(const usbh_itf_data_t *itf_data, u8 subtype, u8 min_len)
{
	const u8 *desc = itf_data->raw_data;
	u32 total = 0U;
	u8 len;

	if (desc == NULL) {
		return NULL;
	}

	while ((total + USB_LEN_DESC_HEADER) <= itf_data->raw_data_len) {
		len = desc[0];
		if ((len < USB_LEN_DESC_HEADER) || ((total + len) > itf_data->raw_data_len)) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Bad desc len %d at %d\n", len, total);
			break;
		}
		if ((desc[1] == USB_CDC_CS_INTERFACE) && (len > USB_LEN_DESC_HEADER) && (desc[2] == subtype)) {
			if (len < min_len) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Short func desc %d len %d\n", subtype, len);
			} else if ((subtype != USB_CDC_FUNC_DESC_UNION) || (desc[3] == itf_data->itf_desc_array[0].bInterfaceNumber)) {
				return desc;
			} else {
				/* A Union of another control interface */
			}
		}
		desc += len;
		total += len;
	}

	return NULL;
}

/**
  * @brief  Check that an interface descriptor carries exactly one BULK IN and one BULK OUT endpoint
  * @param  itf_desc: Interface descriptor (one alternate setting)
  * @param  bulk_in: Set to the BULK IN endpoint
  * @param  bulk_out: Set to the BULK OUT endpoint
  * @retval HAL_OK when both are present and no other endpoint is, HAL_ERR_PARA otherwise
  */
static int usbh_cdc_ecm_check_data_eps(usbh_itf_desc_t *itf_desc, usbh_ep_desc_t **bulk_in, usbh_ep_desc_t **bulk_out)
{
	usbh_ep_desc_t *ep;
	u8 i;

	*bulk_in = NULL;
	*bulk_out = NULL;

	/* Ref ECM 1.2 3.3.2: the data interface carries one BULK IN and one BULK OUT, nothing else */
	if (itf_desc->bNumEndpoints != 2U) {
		return HAL_ERR_PARA;
	}

	for (i = 0U; i < itf_desc->bNumEndpoints; i++) {
		ep = &itf_desc->ep_desc_array[i];
		if ((ep->bmAttributes & USB_EP_XFER_TYPE_MASK) != USB_CH_EP_TYPE_BULK) {
			return HAL_ERR_PARA;
		}
		if (USB_EP_IS_IN(ep->bEndpointAddress)) {
			*bulk_in = ep;
		} else {
			*bulk_out = ep;
		}
	}

	return ((*bulk_in != NULL) && (*bulk_out != NULL)) ? HAL_OK : HAL_ERR_PARA;
}

/**
  * @brief  Locate the ECM function of the active configuration
  * @note   Read only, shared by attach() and usbh_cdc_ecm_check_config_desc() so a configuration that passes the
  *         check is one attach() takes. The comm interface carries one INTR IN endpoint (Ref ECM 1.2 3.3.1). The
  *         data interface is the one its Union names, falling back to the only one present, and its operational
  *         setting is the one with the BULK pair (Ref ECM 1.2 3.3.2, alternate setting 0 has no endpoint).
  *         A device off these rules, e.g. a data interface with extra endpoints, or several data interfaces and
  *         no Union, is refused on purpose rather than bound to a guessed interface.
  * @param  host: Host handle
  * @param  ctrl: Set to the comm interface data
  * @param  data: Set to the operational data interface setting
  * @retval HAL_OK when the whole function is found, HAL_ERR_PARA otherwise
  */
static int usbh_cdc_ecm_find_function(usb_host_t *host, usbh_itf_data_t **ctrl, usbh_itf_desc_t **data)
{
	usbh_dev_id_t dev_id = {0,};
	usbh_itf_data_t *itf_data;
	usbh_itf_desc_t *itf_desc;
	usbh_ep_desc_t *ep;
	usbh_ep_desc_t *bulk_in;
	usbh_ep_desc_t *bulk_out;
	const u8 *union_desc;
	u8 data_itf_num = 0xFFU;
	u8 alt_idx;

	*data = NULL;

	dev_id.bInterfaceClass = USB_CDC_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_ECM_SUBCLASS_ECM;
	dev_id.bInterfaceProtocol = 0x00U;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	if ((itf_data == NULL) || (itf_data->itf_desc_array == NULL)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get if fail\n");
		return HAL_ERR_PARA;
	}

	itf_desc = &itf_data->itf_desc_array[0];
	ep = &itf_desc->ep_desc_array[0];
	if ((itf_desc->bNumEndpoints != 1U) || ((ep->bmAttributes & USB_EP_XFER_TYPE_MASK) != USB_CH_EP_TYPE_INTR) ||
		!USB_EP_IS_IN(ep->bEndpointAddress)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad ctrl EP\n");
		return HAL_ERR_PARA;
	}
	*ctrl = itf_data;

	union_desc = usbh_cdc_ecm_find_func_desc(itf_data, USB_CDC_FUNC_DESC_UNION, (u8)sizeof(usb_cdc_ecm_union_func_desc_t));
	if (union_desc != NULL) {
		data_itf_num = ((const usb_cdc_ecm_union_func_desc_t *)(const void *)union_desc)->bSubordinateInterface0;
	}

	dev_id.bInterfaceClass = USB_CDC_DATA_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_SUBCLASS_RESERVED;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	/* Without a Union only a sole data interface is unambiguous: another function of a composite device may own
	 * any other one */
	if ((data_itf_num == 0xFFU) && ((itf_data == NULL) || (itf_data->next != NULL))) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No union\n");
		return HAL_ERR_PARA;
	}
	while ((itf_data != NULL) && (itf_data->itf_desc_array != NULL) && (data_itf_num != 0xFFU) &&
		   (itf_data->itf_desc_array[0].bInterfaceNumber != data_itf_num)) {
		itf_data = itf_data->next;
	}
	if ((itf_data == NULL) || (itf_data->itf_desc_array == NULL)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No data if %d\n", data_itf_num);
		return HAL_ERR_PARA;
	}

	for (alt_idx = 0U; alt_idx < itf_data->alt_setting_cnt; alt_idx++) {
		if (usbh_cdc_ecm_check_data_eps(&itf_data->itf_desc_array[alt_idx], &bulk_in, &bulk_out) == HAL_OK) {
			*data = &itf_data->itf_desc_array[alt_idx];
			return HAL_OK;
		}
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "No bulk pair\n");
	return HAL_ERR_PARA;
}

/**
  * @brief  Take the endpoints and descriptor indices of the attached device
  * @note   Every field written here describes the device being attached, and is reset first so nothing of a
  *         previous device survives into this one.
  * @param  host: Host handle
  * @retval HAL_OK on success, HAL_ERR_PARA when the device is no usable ECM function
  */
static int usbh_cdc_ecm_parse_interface_desc(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	const usb_cdc_ecm_ethernet_function_desc_t *eth_desc;
	usbh_itf_data_t *ctrl_itf;
	usbh_itf_desc_t *data_itf_desc;
	usbh_ep_desc_t *bulk_in;
	usbh_ep_desc_t *bulk_out;
	u16 seg_size;

	cdc->vid = host->dev_desc->idVendor;
	cdc->pid = host->dev_desc->idProduct;
	cdc->sub_status = CDC_ECM_STATE_AT_SETTING_IDLE;
	cdc->iMACAddressStringId = 0U;
	cdc->intr_rx.valid = 0U;
	cdc->bulk_rx.valid = 0U;
	cdc->bulk_tx.valid = 0U;

	if (usbh_cdc_ecm_find_function(host, &ctrl_itf, &data_itf_desc) != HAL_OK) {
		return HAL_ERR_PARA;
	}
	/* Comm interface: already claimed by the core's Phase 1 match, no need to re-claim here */

	eth_desc = (const usb_cdc_ecm_ethernet_function_desc_t *)(const void *)usbh_cdc_ecm_find_func_desc(ctrl_itf,
			   USB_CDC_FUNC_DESC_ETHERNET_NETWORKING, (u8)sizeof(usb_cdc_ecm_ethernet_function_desc_t));
	if (eth_desc != NULL) {
		cdc->iMACAddressStringId = eth_desc->iMACAddress;
		cdc->sub_status = CDC_ECM_STATE_GET_MAC_STR;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Mac string id(%d)\n", cdc->iMACAddressStringId);
		/* The RX buffer holds one 1514-byte frame plus VLAN/QinQ tags; a larger segment is cut short */
		seg_size = eth_desc->wMaxSegmentSize;
		if (seg_size > USBH_CDC_ECM_BULK_BUF_MAX_SIZE) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "MaxSegSize %d > %d\n", seg_size, USBH_CDC_ECM_BULK_BUF_MAX_SIZE);
		}
	}

	usb_os_memcpy((void *)&cdc->intr_rx.ep_desc, (const void *)&ctrl_itf->itf_desc_array[0].ep_desc_array[0], sizeof(usbh_ep_desc_t));
	cdc->intr_rx.valid = 1U;

	/* Already passed in usbh_cdc_ecm_find_function(), called again for the endpoint pointers */
	if (usbh_cdc_ecm_check_data_eps(data_itf_desc, &bulk_in, &bulk_out) != HAL_OK) {
		return HAL_ERR_PARA;
	}
	usb_os_memcpy((void *)&cdc->bulk_rx.ep_desc, (const void *)bulk_in, sizeof(usbh_ep_desc_t));
	cdc->bulk_rx.valid = 1U;
	usb_os_memcpy((void *)&cdc->bulk_tx.ep_desc, (const void *)bulk_out, sizeof(usbh_ep_desc_t));
	cdc->bulk_tx.valid = 1U;

	cdc->data_itf_id = data_itf_desc->bInterfaceNumber;
	cdc->data_alt_set = data_itf_desc->bAlternateSetting;
	RTK_LOGS(TAG, RTK_LOG_INFO, "Get ECM data if(%d)alt(%d)\n", cdc->data_itf_id, cdc->data_alt_set);

	return HAL_OK;
}

/**
  * @brief  Usb Attach callback function
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_attach(usb_host_t *host) //parse all ep info
{
	int status = HAL_ERR_PARA;
	usbh_cdc_ecm_pipe_info_t *pipe_info;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	/* deinit_all_pipe() needs the host to close what a failed attach opened */
	cdc->host = host;

	if (usbh_cdc_ecm_parse_interface_desc(host) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse desc fail\n");
		goto attach_fail;
	}

	/* All three pipes are mandatory, see usbh_cdc_ecm_find_function() */
	pipe_info = &(cdc->intr_rx);
	if (usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc), &usbh_cdc_ecm_driver) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Open intr pipe fail\n");
		goto attach_fail;
	}
	/* The controller may fill a whole packet, adjusted to the DWORD boundary */
	pipe_info->pipe.xfer_buf = (u8 *)usb_os_malloc(usb_get_dma_len(pipe_info->pipe.ep_mps, pipe_info->pipe.ep_mps));
	if (pipe_info->pipe.xfer_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc intr rx buf fail\n");
		status = HAL_ERR_MEM;
		goto attach_fail;
	}
	pipe_info->pipe.xfer_len = pipe_info->pipe.ep_mps;
	pipe_info->pipe.xfer_buf_len = usb_get_dma_len(pipe_info->pipe.ep_mps, pipe_info->pipe.ep_mps);
	pipe_info->pipe.xfer_state = USBH_EP_XFER_START;

	/* Widen the poll window past the 1-frame bInterval=1 case, else the SOF
	 * re-arm fires on the still in-flight INTR IN and hangs it. */
	cdc->intr_check_tick = pipe_info->pipe.ep_interval + USBH_CDC_ECM_INTR_BUSY_MAX_CNT;
	pipe_info->pipe.max_timeout_tick = USBH_CDC_ECM_INTR_BUSY_MAX_CNT;

	pipe_info = &(cdc->bulk_tx);
	if (usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc), &usbh_cdc_ecm_driver) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Open bulk out pipe fail\n");
		goto attach_fail;
	}
	pipe_info->pipe.max_timeout_tick = USBH_CDC_ECM_BULK_OUT_BUSY_MAX_CNT;

	pipe_info = &(cdc->bulk_rx);
	if (usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc), &usbh_cdc_ecm_driver) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Open bulk in pipe fail\n");
		goto attach_fail;
	}
	/* One 1514-byte Ethernet frame plus VLAN/QinQ tags, rounded up to whole 512-byte packets */
	pipe_info->pipe.xfer_buf = (u8 *)usb_os_malloc(USBH_CDC_ECM_BULK_BUF_MAX_SIZE);
	if (pipe_info->pipe.xfer_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc bulk rx buf fail\n");
		status = HAL_ERR_MEM;
		goto attach_fail;
	}
	pipe_info->pipe.xfer_len = USBH_CDC_ECM_BULK_BUF_MAX_SIZE;
	pipe_info->pipe.xfer_buf_len = USBH_CDC_ECM_BULK_BUF_MAX_SIZE;
	pipe_info->pipe.xfer_state = USBH_EP_XFER_START;

	pipe_info->pipe.max_timeout_tick = USBH_CDC_ECM_BULK_IN_IDLE_MAX_CNT;

	usbh_cdc_ecm_dump_ecm_cfgdesc();

	cdc->halt_retry_cnt = 0U;
	cdc->state = CDC_ECM_STATE_IDLE;

	if ((cdc->cb != NULL) && (cdc->cb->attached != NULL)) {
		cdc->cb->attached();
	}

	return HAL_OK;

attach_fail:
	/* The core calls no detach() for a failed attach: undo here, and withdraw the host so
	 * usbh_cdc_ecm_usb_is_ready() never reports this class ready on another class's setup */
	usbh_cdc_ecm_deinit_all_pipe();
	cdc->host = NULL;
	return status;
}

/**
  * @brief  Usb Detach callback function.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_cdc_ecm_detach(usb_host_t *host)
{
	UNUSED(host);
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	cdc->state = CDC_ECM_STATE_IDLE;
	cdc->ready_to_xfer = 0;
	/* These describe the device that is leaving: a later attach must not inherit them. A MAC the user set
	 * through init is kept for the next device. */
	cdc->eth_hw_connect = 0;
	cdc->mac_valid = 0;
	cdc->vid = 0U;
	cdc->pid = 0U;
	if (cdc->mac_src_type != CDC_ECM_MAC_UPPER_LAYER_SET) {
		cdc->mac_src_type = CDC_ECM_MAC_UNINIT;
	}

	usbh_cdc_ecm_deinit_all_pipe();
	cdc->host = NULL;

	/* Fail a blocked sender: the closed pipe never completes its TX. Given unconditionally
	 * so a sender that has not published bulk_tx_block yet is covered too; it then sees the
	 * cleared state, and a give nobody consumes is dropped by the next blocking send. */
	cdc->bulk_tx_err = 1U;
	usb_os_sema_give(cdc->bulk_tx_sema);

	if ((cdc->cb != NULL) && (cdc->cb->detached != NULL)) {
		cdc->cb->detached();
	}
}

/**
  * @brief  Usb Setup callback function.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_setup(usb_host_t *host)
{
	int status = HAL_ERR_UNKNOWN;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	/* Run the ECM control-setting sequence (GET_MAC / SET_INTERFACE alt /
	 * packet filter / ) here. usbh_cdc_ecm_ctrl_setting() is a state machine
	 * that returns HAL_BUSY until the sequence completes; returning non-OK
	 * keeps the core in USBH_CLASS_REQUEST and re-invokes setup() to drive
	 * the next step, so no separate PRE_SETTING state is needed. */
	status = usbh_cdc_ecm_ctrl_setting(host);
	if (status != HAL_OK) {
		return status;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "ECM alt setting finish %d\n", cdc->intr_rx.pipe.pipe_num);

	if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
		cdc->cb->setup();
	}

	cdc->state = CDC_ECM_STATE_TRANSFER;

	/* Kick the first INTR IN so the network-connection notification can be
	 * received once transfers are enabled. */
	usbh_notify(host, cdc->intr_rx.pipe.pipe_num, &usbh_cdc_ecm_driver);

	return HAL_OK;
}

/**
  * @brief  Start the recovery of a halted endpoint
  * @note   A halted endpoint answers every transaction with STALL until the host clears it
  *         (Ref USB 2.0 8.4.5 / 9.4.5), so re-submitting only repeats the STALL. Entering
  *         CDC_ECM_STATE_ERROR stops the SOF from re-arming any pipe until it is cleared.
  * @param  host: Host handle
  * @param  pipe: Halted pipe
  * @retval None
  */
static void usbh_cdc_ecm_halt(usb_host_t *host, const usbh_pipe_t *pipe)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	/* One halt at a time: another stalled pipe stays idle, is re-armed after this clear and
	 * then reports its own STALL */
	if (cdc->state == CDC_ECM_STATE_ERROR) {
		return;
	}

	cdc->halt_ep_addr = pipe->ep_addr;
	cdc->state = CDC_ECM_STATE_ERROR;
	usbh_notify(host, 0U, &usbh_cdc_ecm_driver);
}

/**
  * @brief  Clear the halted endpoint, re-enumerate the device if that keeps failing
  * @note   CLEAR_FEATURE(ENDPOINT_HALT) also resets the device data toggle to DATA0 (Ref USB
  *         2.0 9.4.5), matching the host toggle the STALL interrupt already reset. Once back in
  *         CDC_ECM_STATE_TRANSFER the SOF re-arms the pipes as usual.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_cdc_ecm_process_halt(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_setup_req_t setup;
	int ret;

	/* Re-enumeration already requested: wait for the detach it brings */
	if (cdc->halt_retry_cnt >= USBH_CDC_ECM_HALT_RETRY_MAX_CNT) {
		return;
	}

	/* Sent through usbh_ctrl_request(), which rejects the call while another class of a
	 * composite device owns EP0; usbh_ctrl_clear_feature() lacks that owner check */
	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_ENDPOINT;
	setup.req.bRequest = USB_REQ_CLEAR_FEATURE;
	setup.req.wValue = USB_FEATURE_EP_HALT;
	setup.req.wIndex = cdc->halt_ep_addr;
	setup.req.wLength = 0U;
	ret = usbh_ctrl_request(host, &setup, NULL);
	if (ret == HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%d halt cleared\n", cdc->halt_ep_addr);
		cdc->halt_retry_cnt = 0U;
		cdc->state = CDC_ECM_STATE_TRANSFER;
		/* This state swallowed any pipe message that arrived meanwhile: re-drive every pipe so a
		 * completion latched in that window is still consumed */
		usbh_notify(host, cdc->intr_rx.pipe.pipe_num, &usbh_cdc_ecm_driver);
		usbh_notify(host, cdc->bulk_rx.pipe.pipe_num, &usbh_cdc_ecm_driver);
		usbh_notify(host, cdc->bulk_tx.pipe.pipe_num, &usbh_cdc_ecm_driver);
	} else if (ret != HAL_BUSY) {
		cdc->halt_retry_cnt++;
		if (cdc->halt_retry_cnt >= USBH_CDC_ECM_HALT_RETRY_MAX_CNT) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%d clear halt fail, re-enum\n", cdc->halt_ep_addr);
			(void)usbh_reenumerate();
		} else {
			usbh_notify(host, 0U, &usbh_cdc_ecm_driver);
		}
	} else {
		/* In flight: the EP0 completion re-drives process() */
	}
}

/**
  * @brief  Usb State Machine handling callback
  * @param  host: Host handle
  * @retval None
  */
static void usbh_cdc_ecm_process(usb_host_t *host, usbh_drv_msg_t *msg)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	u8 pipe_num;

	switch (cdc->state) {
	case CDC_ECM_STATE_TRANSFER:
		if (msg) {
			pipe_num = msg->pipe_num;
			if (pipe_num == cdc->bulk_tx.pipe.pipe_num) {
				usbh_cdc_ecm_process_bulk_out(host);
			} else if (pipe_num == cdc->bulk_rx.pipe.pipe_num) {
				usbh_cdc_ecm_process_bulk_in(host);
			} else if (pipe_num == cdc->intr_rx.pipe.pipe_num) {
				usbh_cdc_ecm_process_intr_in(host);
			}
		}
		break;

	case CDC_ECM_STATE_ERROR:
		usbh_cdc_ecm_process_halt(host);
		break;

	case CDC_ECM_STATE_IDLE:
	default:
		/* main task in idle/default status, sleep to release CPU */
		usb_os_sleep_ms(1);
		break;
	}
}

/**
  * @brief  SOF callback for class-specific timing process.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param[in] host: USB host handle.
  * @retval None
  */
static void usbh_cdc_ecm_sof(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	UNUSED(host);

	if (cdc->ready_to_xfer) {
		/* pipe_num != 0 means the pipe is open (0 is reserved for control).
		   Guard before notifying so a not-yet-opened pipe is never triggered. */
		if (cdc->intr_rx.pipe.pipe_num != 0) {
			usbh_cdc_ecm_intr_receive();
		}

		if (cdc->eth_hw_connect) {
			if (cdc->bulk_rx.pipe.pipe_num != 0) {
				usbh_cdc_ecm_bulk_receive();
			}
			if (cdc->bulk_tx.pipe.pipe_num != 0) {
				usbh_cdc_ecm_bulk_tx();
			}
		}
	}
}

/**
  * @brief  Bulk Out handling function
  * @param  host: Host handle
  * @retval Status
*/
static void usbh_cdc_ecm_process_bulk_out(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_pipe_info_t *pipe_info = &(cdc->bulk_tx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usbh_urb_state_t urb_state;
	int status;

#if USBH_CDC_ECM_STATE_TRACE_ENABLE
	pipe_info->trigger_cnt++;
#endif

	status = usbh_transfer_process(host, ep);
	if ((status == HAL_OK) && (ep->xfer_state == USBH_EP_XFER_IDLE)) {
		usbh_cdc_ecm_cb_bulk_send(status);
	} else if (ep->xfer_state == USBH_EP_XFER_START) {
		usbh_notify(host, ep->pipe_num, &usbh_cdc_ecm_driver);
	} else if (ep->xfer_state == USBH_EP_XFER_ERROR) {
		urb_state = usbh_get_urb_state(host, ep);
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail %d\n", urb_state);
		if (urb_state == USBH_URB_STALL) {
			usbh_cdc_ecm_halt(host, ep);
		}
		/* Drop the frame (Ethernet is best effort) and release the sender: re-sending it
		 * forever left a blocked sender waiting on a TX that never completes. */
		ep->xfer_state = USBH_EP_XFER_IDLE;
		usbh_cdc_ecm_cb_bulk_send(HAL_ERR_HW);
	} else {
		/* Idle or still in flight */
	}
}

static int usbh_cdc_ecm_cb_bulk_receive(u8 *buf, u32 length)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	if (buf == NULL || length == 0) {
		return HAL_ERR_PARA;
	}

#if USBH_CDC_ECM_RX_SPEED_CHECK
	static u64 usb_rx_start_time = 0, usb_rx_end_time, usb_rx_interval_time; //ms
	static u64 usb_rx_total_len = 0;

	usb_rx_total_len += length;
	if (usb_rx_start_time == 0) {
		usb_rx_start_time = usb_os_get_timestamp_ms();
	}
	usb_rx_end_time = usb_os_get_timestamp_ms();
	usb_rx_interval_time = (usb_rx_end_time - usb_rx_start_time) * RTOS_TICK_RATE_MS;

	if (usb_rx_interval_time >= 2000) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Heap %d RX %dB in %d ms, %d Kbps\n",
				 rtos_mem_get_free_heap_size(),
				 (u32)usb_rx_total_len, (u32)usb_rx_interval_time, (u32)((usb_rx_total_len * 8 * 1000) / (usb_rx_interval_time * 1024)));
		usb_rx_start_time = usb_rx_end_time;
		usb_rx_total_len = 0;
	}
#endif

	if ((cdc->cb != NULL) && (cdc->cb->received != NULL)) {
#if 0
		for (u32 i = 0; i < length; i++) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "%02x ", (u8)buf[i]);
		}
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
#endif
		cdc->cb->received(buf, length);
	}

	return HAL_OK;
}

static void usbh_cdc_ecm_process_bulk_in(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_pipe_info_t *pipe_info = &(cdc->bulk_rx);
	usbh_pipe_t *bulk_in = &(pipe_info->pipe);
	int status = usbh_transfer_process(host, bulk_in);
	usbh_urb_state_t urb_state;
	u32 len = 0;

#if USBH_CDC_ECM_STATE_TRACE_ENABLE
	pipe_info->trigger_cnt++;
#endif

	if ((status == HAL_OK) && (bulk_in->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, bulk_in);
		usbh_cdc_ecm_cb_bulk_receive(bulk_in->xfer_buf, len);

		bulk_in->xfer_state = USBH_EP_XFER_START;
		usbh_notify(host, bulk_in->pipe_num, &usbh_cdc_ecm_driver);
	} else if (bulk_in->xfer_state == USBH_EP_XFER_START) {
		/* rx_len is written only by a transfer-complete interrupt and zeroed on every submit,
		 * so a transfer that is merely being retried reads 0 and the callback drops it. A
		 * non-zero length means the transfer did complete just as the retry fired (or the
		 * N*MPS data before a trailing ZLP landed), so it is real data and is delivered. */
		len = usbh_get_last_transfer_size(host, bulk_in);
		usbh_cdc_ecm_cb_bulk_receive(bulk_in->xfer_buf, len);

		usbh_notify(host, bulk_in->pipe_num, &usbh_cdc_ecm_driver);
	} else if (bulk_in->xfer_state == USBH_EP_XFER_ERROR) {
		urb_state = usbh_get_urb_state(host, bulk_in);
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK RX fail %d\n", urb_state);
		if (urb_state == USBH_URB_STALL) {
			/* Re-armed by the SOF once the halt is cleared */
			bulk_in->xfer_state = USBH_EP_XFER_IDLE;
			usbh_cdc_ecm_halt(host, bulk_in);
		} else {
			bulk_in->xfer_state = USBH_EP_XFER_START;
			usbh_notify(host, bulk_in->pipe_num, &usbh_cdc_ecm_driver);
		}
	} else {
		/* Still in flight */
	}
}

/**
  * @brief  Intr in function
  * @param  host: Host handle
  * @retval Status
*/
static void usbh_cdc_ecm_process_intr_in(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_pipe_info_t *pipe_info = &(cdc->intr_rx);
	usbh_pipe_t *intr_in = &(pipe_info->pipe);
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, intr_in);

#if USBH_CDC_ECM_STATE_TRACE_ENABLE
	pipe_info->trigger_cnt++;
#endif

	/* A completed IN shows only as latched urb_state==DONE; consume and deliver it
	 * here rather than via usbh_transfer_process(), which raced the SOF re-arm and
	 * dropped the one-shot network-connection notification. xfer_state==BUSY is
	 * stable vs. the SOF ISR, so this read-check-consume is race-free. */
	if ((intr_in->xfer_state == USBH_EP_XFER_BUSY) && (urb_state == USBH_URB_DONE)) {
		u32 len = usbh_get_last_transfer_size(host, intr_in);
		intr_in->retry_cnt = 0;
		intr_in->xfer_state = USBH_EP_XFER_IDLE;
		usbh_cdc_ecm_cb_intr_receive(intr_in->xfer_buf, len);
		return;
	}

	/* The SOF never re-arms a stalled pipe (see usbh_cdc_ecm_intr_receive()), so a STALL stays
	 * latched with xfer_state==BUSY until handled here; the SOF re-arms it once cleared. */
	if ((intr_in->xfer_state == USBH_EP_XFER_BUSY) && (urb_state == USBH_URB_STALL)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX stall\n");
		intr_in->xfer_state = USBH_EP_XFER_IDLE;
		usbh_cdc_ecm_halt(host, intr_in);
		return;
	}

	/* Not a completion: (re)arm only on START. Never usbh_transfer_process()
	 * while BUSY - re-submitting an already-armed INTR channel corrupts it. */
	if (intr_in->xfer_state == USBH_EP_XFER_START) {
		int status = usbh_transfer_process(host, intr_in);
		if ((status != HAL_OK) || (intr_in->xfer_state == USBH_EP_XFER_ERROR)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX fail %d\n", usbh_get_urb_state(host, intr_in));
		}
	}
}

/**
  * @brief  Check ECM BULK RX status and timeout, whether should trigger.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @return HAL_OK if a new transfer should be triggered, HAL_BUSY otherwise.
  */
static int usbh_cdc_ecm_bulk_rx_time_check(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_pipe_info_t *pipe_info = &(cdc->bulk_rx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->host;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, ep);

	if ((USBH_URB_IDLE == urb_state) && (usbh_get_elapsed_ticks(host, ep->tick) >= (USBH_CDC_ECM_BULK_IN_IDLE_MAX_CNT))) {
		return HAL_OK;
	}

	if ((USBH_URB_BUSY == urb_state) && (usbh_get_elapsed_ticks(host, ep->tick) >= (USBH_CDC_ECM_BULK_IN_NAK_CHECK_MAX_CNT))) {
		return HAL_OK;
	}

	if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL) || (urb_state == USBH_URB_DONE)) { //if check done, force to trigger a msg
		return HAL_OK;
	}

	return HAL_BUSY;
}

/**
  * @brief Check ECM BULK TX status and timeout, whether should trigger
  * @retval Status
  */
static int usbh_cdc_ecm_bulk_tx_time_check(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_pipe_info_t *pipe_info = &(cdc->bulk_tx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->host;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, ep);

	if ((urb_state == USBH_URB_IDLE) && (usbh_get_elapsed_ticks(host, ep->tick) >= (USBH_CDC_ECM_BULK_OUT_IDLE_MAX_CNT))) {
		return HAL_OK;
	}

	if ((urb_state == USBH_URB_BUSY) && (usbh_get_elapsed_ticks(host, ep->tick) >= (USBH_CDC_ECM_BULK_OUT_BUSY_MAX_CNT))) {
		return HAL_OK;
	}

	if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL) || (urb_state == USBH_URB_DONE)) {
		return HAL_OK;
	}

	return HAL_BUSY;
}

/**
  * @brief Check ECM INTR RX status and timeout, whether should trigger
  * @retval Status
  */
static int usbh_cdc_ecm_intr_rx_time_check(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_pipe_info_t *pipe_info = &(cdc->intr_rx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->host;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, ep);

	if ((USBH_URB_IDLE == urb_state) && (usbh_get_elapsed_ticks(host, ep->tick) >= cdc->intr_check_tick)) {
		return HAL_OK;
	}

	if ((USBH_URB_BUSY == urb_state) && (usbh_get_elapsed_ticks(host, ep->tick) >= cdc->intr_check_tick)) {
		return HAL_OK;
	}

	if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
		return HAL_OK;
	}

	return HAL_BUSY;
}

/**
  * @brief  Trigger transmission of BULK data.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @return 0 on success, non-zero on failure.
  */
static int usbh_cdc_ecm_bulk_tx(void)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_pipe_t  *ep = &(cdc->bulk_tx.pipe);
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ecm_check_enum_status() != HAL_OK) {
		return ret;
	}

	if (cdc->state == CDC_ECM_STATE_TRANSFER) {
		/* An idle pipe has nothing to drive: bulk_send() notifies when it arms a TX, and
		   process_bulk_out() is a no-op on IDLE, so only re-drive an armed or in-flight TX */
		if ((ep->xfer_state != USBH_EP_XFER_IDLE) && (usbh_cdc_ecm_bulk_tx_time_check() == HAL_OK)) {
			usbh_notify(host, ep->pipe_num, &usbh_cdc_ecm_driver);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Trigger reception of BULK data.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @return 0 on success, non-zero on failure.
  */
static int usbh_cdc_ecm_bulk_receive(void)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_pipe_t  *ep = &(cdc->bulk_rx.pipe);
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ecm_check_enum_status() != HAL_OK) {
		return ret;
	}

	if (cdc->state == CDC_ECM_STATE_TRANSFER) {
		if ((ep->xfer_state == USBH_EP_XFER_IDLE) || (usbh_cdc_ecm_bulk_rx_time_check() == HAL_OK)) {
			if (ep->xfer_state == USBH_EP_XFER_IDLE) {
				ep->xfer_state = USBH_EP_XFER_START;
			}

			usbh_notify(host, ep->pipe_num, &usbh_cdc_ecm_driver);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Trigger reception of INTR data.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @return 0 on success, non-zero on failure.
  */
static int usbh_cdc_ecm_intr_receive(void)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_pipe_t  *ep = &(cdc->intr_rx.pipe);
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ecm_check_enum_status() != HAL_OK) {
		return ret;
	}

	if (cdc->state == CDC_ECM_STATE_TRANSFER) {
		if (((ep->xfer_state == USBH_EP_XFER_IDLE) && (usbh_get_elapsed_ticks(host, ep->tick) >= cdc->intr_check_tick))
			|| (usbh_cdc_ecm_intr_rx_time_check() == HAL_OK)) {
			/* A STALL is only reported: re-submitting to a halted endpoint repeats the STALL */
			if ((ep->xfer_state != USBH_EP_XFER_BUSY) || (usbh_get_urb_state(host, ep) != USBH_URB_STALL)) {
				ep->xfer_state = USBH_EP_XFER_START;
			}
			usbh_notify(host, ep->pipe_num, &usbh_cdc_ecm_driver);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Start to send BULK data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
static int usbh_cdc_ecm_bulk_send(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_pipe_info_t *pipe_info = &(cdc->bulk_tx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ecm_check_enum_status() != HAL_OK) {
		return ret;
	}

	/* An unopened pipe has pipe_num 0, which is EP0, and ep_mps 0 for the ZLP math below */
	if ((ep->pipe_num == 0U) || (ep->ep_mps == 0U)) {
		return HAL_ERR_PARA;
	}

	/* Only in TRANSFER, and never write the state: the task may have just entered ERROR to clear a halt */
	if (cdc->state == CDC_ECM_STATE_TRANSFER) {
		if (ep->xfer_state == USBH_EP_XFER_IDLE) {
			ep->xfer_buf = buf;
			ep->xfer_len = len;

			/* A BULK transfer of N*MPS bytes ends with a ZLP (Ref USB 2.0 5.8.3) */
			if ((len % ep->ep_mps) == 0U) {
				ep->trx_zlp = 1;
			} else {
				ep->trx_zlp = 0;
			}
			ep->xfer_state = USBH_EP_XFER_START;
			usbh_notify(host, ep->pipe_num, &usbh_cdc_ecm_driver);
			ret = HAL_OK;
		}
	}

	return ret;
}

static int usbh_cdc_ecm_cb_bulk_send(int state)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	/* Release a blocked sender on success and on failure alike, failure reported through bulk_tx_err */
	if (cdc->bulk_tx_block) {
		cdc->bulk_tx_err = (state == HAL_OK) ? 0U : 1U;
		usb_os_sema_give(cdc->bulk_tx_sema);
	}

	return HAL_OK;
}

static int usbh_cdc_ecm_tx_status_check(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	u32 wait_ms = 0U;

	/* Wake a sender blocked on the sema and wait for every task inside the TX API to leave
	 * before the sema and buffers go away; bounded so a starved caller cannot hang the teardown */
	while ((cdc->bulk_tx_block != 0U) || (cdc->tx_users != 0U)) {
		if (wait_ms >= USBH_CDC_ECM_TX_EXIT_TIMEOUT_MS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Wait TX exit TO\n");
			break;
		}
		(void)usb_os_sema_give(cdc->bulk_tx_sema);
		usb_os_sleep_ms(1U);
		wait_ms++;
	}

	return HAL_OK;
}

static void usbh_cdc_ecm_deinit_all_pipe(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host = cdc->host;

	if (host == NULL) {
		return;
	}

	if (cdc->intr_rx.pipe.pipe_num) {
		usbh_close_pipe(host, &(cdc->intr_rx.pipe));
		USBH_CDC_ECM_FREE_MEM(cdc->intr_rx.pipe.xfer_buf);
	}

	if (cdc->bulk_rx.pipe.pipe_num) {
		usbh_close_pipe(host, &(cdc->bulk_rx.pipe));
		USBH_CDC_ECM_FREE_MEM(cdc->bulk_rx.pipe.xfer_buf);
	}

	if (cdc->bulk_tx.pipe.pipe_num) {
		usbh_close_pipe(host, &(cdc->bulk_tx.pipe));
	}
}

#if USBH_CDC_ECM_STATE_TRACE_ENABLE
static void usbh_cdc_ecm_trace_thread(void *param)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	UNUSED(param);
	usbh_cdc_ecm_trace_task_init_flag = 1;
	while (1) {
		if (cdc->host) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Status %d-%d-%d-%d/out %d-%d %d/in %d-%d/intr %d-%d/%d-%d-%d\n",
					 cdc->state, cdc->eth_hw_connect, cdc->host->connect_state, cdc->intr_check_tick,
					 cdc->bulk_tx.pipe.xfer_state, usbh_get_urb_state(cdc->host, &(cdc->bulk_tx.pipe)), cdc->bulk_tx.pipe.tick,
					 cdc->bulk_rx.pipe.xfer_state, usbh_get_urb_state(cdc->host, &(cdc->bulk_rx.pipe)),
					 cdc->intr_rx.pipe.xfer_state, usbh_get_urb_state(cdc->host, &(cdc->intr_rx.pipe)),
					 cdc->bulk_tx.trigger_cnt, cdc->bulk_rx.trigger_cnt, cdc->intr_rx.trigger_cnt);
#if USBH_TP_TRACE_DEBUG
			/* ISR timing metrics only exist on usb_host_t when TP trace is built. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "IsrT %lld-%lld/period %lld-%lld\n",
					 cdc->host->isr_process_time_max, cdc->host->isr_process_time,
					 cdc->host->isr_enter_period_max, cdc->host->isr_enter_period);
#endif
		}
		usb_os_sleep_ms(1000);
	}
}

static void usbh_cdc_ecm_trace_task_init(void)
{
	int status;
	if (usbh_cdc_ecm_trace_task_init_flag) {
		return;
	}

	status = rtos_task_create(&usbh_cdc_ecm_trace_task, "usbh_cdc_ecm_trace_thread", usbh_cdc_ecm_trace_thread, NULL, 1024U * 2, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create ecm dump task fail\n");
	}
}

static void usbh_cdc_ecm_trace_task_deinit(void)
{
	if (usbh_cdc_ecm_trace_task != NULL) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Del trace task\n");
		rtos_task_delete(usbh_cdc_ecm_trace_task);
		usbh_cdc_ecm_trace_task = NULL;
	}

	usbh_cdc_ecm_trace_task_init_flag = 0;
}

static void usbh_cdc_ecm_trace_time_cnt(void)
{
#if USBH_TP_TRACE_DEBUG
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	if (cdc->host) {
		printf("Status %lld %lld %lld 0x%" PRIx64 " %lld\n",
			   cdc->host->isr_process_time_max, cdc->host->isr_process_time,
			   cdc->host->isr_enter_period_max, cdc->host->isr_enter_period,
			   cdc->host->isr_enter_time);
	}
#endif
}

static void usbh_cdc_ecm_reset_test_cnt(void)
{
#if USBH_TP_TRACE_DEBUG
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host;

	if (cdc && cdc->host) {
		host = cdc->host;

		host->isr_process_time_max = 0;
		host->isr_enter_period_max = 0;

		host->isr_enter_time = 0;
		usbh_cdc_ecm_trace_time_cnt();
	}
#endif
}

static u32 usbh_cdc_ecm_cmd_entry(u16 argc, u8 *argv[])
{
	int status = HAL_OK;
	const char *cmd;

	if (argc == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid argument\n");
		return HAL_ERR_PARA;
	}

	cmd = (const char *)argv[0];
	RTK_LOGS(TAG, RTK_LOG_INFO, "Cmd %s\n", cmd);
	if (_stricmp(cmd, "reset") == 0) {
		usbh_cdc_ecm_reset_test_cnt();
	} else if (_stricmp(cmd, "dump_time") == 0) {
		usbh_cdc_ecm_trace_time_cnt();
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Input cmd err %s\n", cmd);
	}

	return status;
}

/*
	ecmh reset
	ecmh dump_time
*/
#endif

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init CDC ECM class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_cdc_ecm_init(const usbh_cdc_ecm_state_cb_t *cb, const usbh_cdc_ecm_config_t *cfg)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	int ret = HAL_OK;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	usb_os_memset((void *)cdc, 0x00, sizeof(usbh_cdc_ecm_host_t));

	if (usb_os_sema_create(&(cdc->bulk_tx_sema)) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "TX_Sema create fail\n");
		return HAL_ERR_PARA;
	}

	/* Every D2H request on it reserves a whole EP0 packet, whatever its wLength */
	cdc->dongle_ctrl_buf = (u8 *)usb_os_malloc(USB_MAX_EP0_SIZE);
	if (NULL == cdc->dongle_ctrl_buf) {
		ret = HAL_ERR_MEM;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get ctrl buf mem fail\n");
		goto ctrl_buf_fail;
	}

	cdc->mac_valid = 0;

	/* cfg == NULL is valid (e.g. usbh_wifi_bridge passes NULL): mac_valid is set later during
	 * enumeration, and led_cnt == 0 makes LED setup skip gracefully. */
	cdc->led_cnt = 0;
	if (cfg != NULL) {
		if (cfg->mac_value != NULL) {
			usbh_cdc_ecm_set_dongle_mac(cfg->mac_value);
		}
		if ((cfg->led_array != NULL) && (cfg->led_cnt > 0U)) {
			usbh_cdc_ecm_set_dongle_led_array(cfg->led_array, cfg->led_cnt);
		}
	}

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "CB init fail\n");
			goto user_init_fail;
		}
	}

	cdc->cb = cb;

	ret = usbh_register_class(&usbh_cdc_ecm_driver);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Register class fail %d\n", ret);
		cdc->cb = NULL;
		goto user_deinit;
	}

#if USBH_CDC_ECM_STATE_TRACE_ENABLE
	usbh_cdc_ecm_trace_task_init();
#endif

	/* Every TX resource exists from here on: let the TX API in. The critical section orders
	   the resource stores before the gate for the other core. */
	usb_os_enter_critical();
	cdc->tx_open = 1U;
	usb_os_exit_critical();

	return ret;

user_deinit:
	if (cb->deinit != NULL) {
		cb->deinit();
	}
user_init_fail:
	USBH_CDC_ECM_FREE_MEM(cdc->dongle_ctrl_buf);
	USBH_CDC_ECM_FREE_MEM(cdc->led_array);
ctrl_buf_fail:
	usb_os_sema_delete(cdc->bulk_tx_sema);

	return ret;
}

/**
  * @brief  Deinitialize the USB ECM Class
  */
void usbh_cdc_ecm_deinit(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	/* Close the TX gate first: the TX API rejects new callers from here on, and the ones
	 * already inside are waited for in tx_status_check() before their resources go away */
	usb_os_enter_critical();
	cdc->tx_open = 0U;
	usb_os_exit_critical();

	cdc->eth_hw_connect = 0;
	cdc->ready_to_xfer = 0;

	/* Before anything is freed: on HAL_OK no attach/setup/process/detach of this
	 * class runs any more, an EP0 request it left in flight is halted and a sof() still
	 * running on another core is waited out. Anything else means nothing was released. */
	if (usbh_unregister_class(&usbh_cdc_ecm_driver) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Deinit refused\n");
		return;
	}

#if USBH_CDC_ECM_STATE_TRACE_ENABLE
	usbh_cdc_ecm_trace_task_deinit();
#endif

	usbh_cdc_ecm_tx_status_check();
	usb_os_sema_delete(cdc->bulk_tx_sema);

	usbh_cdc_ecm_deinit_all_pipe();
	cdc->host = NULL;

	USBH_CDC_ECM_FREE_MEM(cdc->dongle_ctrl_buf);
	USBH_CDC_ECM_FREE_MEM(cdc->led_array);
	cdc->led_cnt = 0;

	if ((cdc->cb != NULL) && (cdc->cb->deinit != NULL)) {
		cdc->cb->deinit();
	}

	cdc->cb = NULL;
}

/**
  * @brief  choose the right config descriptor while there has more then one config descriptor
  * @param  host: Host handle
  * @retval Status
  */
int usbh_cdc_ecm_choose_config(usb_host_t *host)
{
	//choose ecm cfg_num
	usbh_dev_id_t dev_id = {0,};
	dev_id.bInterfaceClass = USB_CDC_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_ECM_SUBCLASS_ECM;
	dev_id.bInterfaceProtocol = 0x00;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	u8 cfg_idx = usbh_get_configuration(host, &dev_id);
	return usbh_set_configuration(host, cfg_idx);
}

int usbh_cdc_ecm_check_config_desc(usb_host_t *host)
{
	usbh_itf_data_t *ctrl_itf;
	usbh_itf_desc_t *data_itf_desc;

	/* The same rules as attach(), so a configuration accepted here is one attach() takes */
	if (usbh_cdc_ecm_find_function(host, &ctrl_itf, &data_itf_desc) == HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Check success idx\n");
		return HAL_OK;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Check fail idx\n");
	return HAL_ERR_PARA;
}

/**
  * @brief  Check usb enum status
  * @retval enum status
  */
u8 usbh_cdc_ecm_usb_is_ready(void)
{
	return (usbh_cdc_ecm_check_enum_status() == HAL_OK);
}

/**
  * @brief  Signal upper-layer preparation is complete; allow SOF to schedule
  *         Ethernet data transfer (bulk/intr). See usbh_cdc_ecm_sof().
  */
void usbh_cdc_ecm_prepare_done(void)
{
	usbh_cdc_ecm_host.ready_to_xfer = 1;
}

/**
  * @brief  Send one Ethernet frame, called by usbh_cdc_ecm_send_data() inside the TX gate
  * @param  buf: Data buffer
  * @param  len: Data length
  * @param  block: Whether block until xfer done
  * @retval Status
  */
static int usbh_cdc_ecm_tx_frame(u8 *buf, u32 len, u8 block)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	u8 retry_cnt = 0;
	int ret;

#if USBH_CDC_ECM_TX_SPEED_CHECK
	static u64 usb_tx_start_time = 0, usb_tx_end_time, usb_tx_interval_time;
	static u64 usb_tx_total_len = 0;

	usb_tx_total_len += len;
	if (usb_tx_start_time == 0) {
		usb_tx_start_time = usb_os_get_timestamp_ms();
	}
#endif

	if (block) {
		cdc->bulk_tx_err = 0U;
		/* Drop a give left over from a TX nobody waited for (e.g. by detach), else the take
		 * below would return before this TX completes */
		(void)usb_os_sema_take(cdc->bulk_tx_sema, 0U);
		cdc->bulk_tx_block = 1;
	}

	while (1) {
		ret = usbh_cdc_ecm_bulk_send(buf, len);
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

	//wait cdc_ecm_cb_bulk_send to give the sema
	if ((ret == HAL_OK) && block) {
		/* No timeout on purpose: every submitted TX ends in cb_bulk_send() or detach, and returning
		 * earlier would hand the buffer back to the caller while the controller still reads it. A deinit,
		 * which closes tx_open first and then halts every pipe, ends the wait too.
		 * The caller shall be a task that can block, see usbh_cdc_ecm_send_data(): inside a critical section
		 * or the PMU suspend flow the take fails at once and the host task that gives the sema never runs. */
		while ((usb_os_sema_take(cdc->bulk_tx_sema, USB_OS_SEMA_TIMEOUT) != HAL_OK) && (cdc->tx_open != 0U)) {
			usb_os_sleep_ms(1U);
		}
		if (cdc->bulk_tx_err != 0U) {
			ret = HAL_ERR_HW;
		}
#if USBH_CDC_ECM_TX_SPEED_CHECK
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

	cdc->bulk_tx_block = 0;

	return ret;
}

/**
  * @brief  Transmits an Ethernet packet to the device via the Bulk OUT endpoint.
  * @note   Task context only, one sender at a time. Gated against deinit: a caller is either
  *         counted in tx_users before deinit closes the gate, or rejected after it.
  * @param  buf: Ethernet frame
  * @param  len: Frame length
  * @param  block: Non-zero to wait until the transfer completes
  * @retval HAL_OK on success, HAL_ERR_HW while the class is not initialized or being
  *         de-initialized, HAL_ERR_PARA in interrupt context, others on transfer failure.
  */
int usbh_cdc_ecm_send_data(u8 *buf, u32 len, u8 block)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	int ret;

	/* A blocking send waits on a semaphore */
	if (usb_os_is_in_interrupt() != 0U) {
		return HAL_ERR_PARA;
	}

	/* Counted under the same lock deinit closes the gate with, so deinit either sees this
	 * caller or this caller sees the gate closed: never both missed */
	usb_os_enter_critical();
	if (cdc->tx_open == 0U) {
		usb_os_exit_critical();
		return HAL_ERR_HW;
	}
	cdc->tx_users++;
	usb_os_exit_critical();

	ret = usbh_cdc_ecm_tx_frame(buf, len, block);

	usb_os_enter_critical();
	cdc->tx_users--;
	usb_os_exit_critical();

	return ret;
}

/**
  * @brief  Get the current network link state (uplink status).
  *
  * @note   For RTK ECM devices the link state is driven by the CDC
  *         NOTIFY_NETWORK_CONNECTION notification received over the interrupt
  *         endpoint and reflected in eth_hw_connect.
  *
  *         For 4G cellular dongles (any non-RTK VID) the ECM interface is a
  *         bridge to a mobile data connection: the physical "link" concept maps
  *         to the cellular registration state, which is managed entirely by the
  *         modem's AT command layer (e.g. AT+CGATT, AT+NETOPEN). By the time
  *         usbh_cdc_ecm_prepare_done() is called the AT dial sequence has
  *         already confirmed the data path is up, so eth_hw_connect is not
  *         meaningful -- we return 1 unconditionally and let the upper layer
  *         (link_change_thread) trigger DHCP / IP assignment.
  *
  * @retval 1: Network link is up (connected to uplink).
  *         0: Network link is down (disconnected, or USB not yet enumerated).
  */
int usbh_cdc_ecm_get_link_status(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	/* check whether the device setup phase is complete */
	if (usbh_cdc_ecm_check_enum_status() != HAL_OK) {
		return 0;
	}

	/* eth_hw_connect is updated exclusively by the intr-RX callback
	 * (usbh_cdc_ecm_cb_intr_receive), which is only scheduled when
	 * ready_to_xfer == 1. Guard here makes the dependency explicit:
	 * link state is meaningless until the transfer path is live. */
	if (!cdc->ready_to_xfer) {
		return 0;
	}

	/* RTK ECM device: honour the NOTIFY_NETWORK_CONNECTION indication. */
	if (usbh_cdc_ecm_get_device_vid() == USB_VID) {
		return cdc->eth_hw_connect;
	}

	/* 4G dongle (non-RTK VID): cellular link state is managed by the AT
	 * command layer, not by CDC notifications. Return 1 so the upper layer
	 * proceeds to DHCP / IP assignment immediately after prepare_done(). */
	return 1;
}

/**
  * @brief  Get the mac str, if the dongle support the standard CDC ECM
  * @retval mac string,the length is 6 Bytes
  */
const u8 *usbh_cdc_ecm_process_mac_str(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	/* No wait: the MAC is settled in setup(), before the device reports ready, so waiting here
	 * only stalled the caller's link poll while no device was attached.
	 * The host task writes mac before mac_valid and both before connect_state reaches SETUP, so a caller that
	 * first waits for usbh_cdc_ecm_usb_is_ready(), as every in-tree one does, reads a complete MAC. */
	if (cdc->mac_valid == 0) {
		return NULL;
	}

	return cdc->mac;
}

/**
  * @brief  Get the Vendor ID of the attached CDC ECM device.
  * @retval VID, or 0 if not attached.
  */
u16 usbh_cdc_ecm_get_device_vid(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	return cdc->vid;
}

/**
  * @brief  Get the Product ID of the attached CDC ECM device.
  * @retval PID, or 0 if not attached.
  */
u16 usbh_cdc_ecm_get_device_pid(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	return cdc->pid;
}

__attribute__((weak))
int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbh_cdc_ecm_send_data(buf, len, block);
}
