/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usb_os.h"
#include "usbh_cdc_ncm.h"

/* Private defines -----------------------------------------------------------*/

#define USBH_CDC_NCM_ENABLE_DUMP_DESCRIPTOR_PARSE    0

#define USBH_CDC_NCM_RX_SPEED_CHECK                  0     /* NCM rx speed test */
#define USBH_CDC_NCM_TX_SPEED_CHECK                  0     /* NCM tx speed test */

#define USBH_CDC_NCM_DEFAULT_VID                     USB_VID

/* ============================================================================
 * Buffer size configuration (tune here)
 * ----------------------------------------------------------------------------
 * All NCM host RX/TX buffer sizes are collected here for easy modification.
 * Increasing these raises throughput potential at the cost of more RAM.
 *
 * Direction reference (Host perspective):
 *   TX = BULK OUT (host → device)
 *   RX = BULK IN  (device → host)
 * ==========================================================================*/

/* RX: Single BULK IN receive buffer — holds one received NTB (device→host).
 * Also clamps ntb_in_max_size / ntb_out_max_size from device negotiation. */
#define USBH_CDC_NCM_RX_NTB_BUF_SIZE                    (2048U * 4U)   /* 8 KB */

#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
/* TX (aggregation): Per ping‑pong buffer for coalescing multiple datagrams
 * before submitting one multi-frame NTB. Two buffers (tx_agg_buf[0/1])
 * each of this size. Larger RAM for higher burst throughput. */
#define USBH_CDC_NCM_TX_AGG_BUF_SIZE                    (2048U * 4U) /* 8 KB */
#else
/* TX (non‑aggregation): Single NTB buffer for one datagram per OUT transfer.
 * Small footprint — holds NTH16 + NDP16 + one Ethernet frame (~1514 B). */
#define USBH_CDC_NCM_TX_BUF_SIZE                        2048U        /* 2 KB */
#endif

/* Control-transfer string buffer length (MAC string descriptor). */
#define USBH_CDC_NCM_MAC_STRING_LEN                          32U            /* String buffer length for MAC descriptor */
/* iMACAddress string: 2-byte header + 12 hex digits in UTF-16LE (Ref ECM 1.2 5.4, used by NCM 1.0 5.2.1) */
#define USBH_CDC_NCM_MAC_DESC_LEN                            26U

/* Consecutive non-STALL BULK errors: logged on the first and every LOG_STEP-th, the device is re-enumerated at MAX */
#define USBH_CDC_NCM_XFER_ERR_LOG_STEP                       100U
#define USBH_CDC_NCM_XFER_ERR_MAX_CNT                        1000U

/* tick count */
#define USBH_CDC_NCM_BULK_OUT_IDLE_MAX_CNT                 (8U * 50U)       /* 50 SOF ticks idle timeout */
#define USBH_CDC_NCM_BULK_OUT_BUSY_MAX_CNT                 (8U * 10U)       /* 10 SOF ticks busy timeout */

#define USBH_CDC_NCM_BULK_IN_IDLE_MAX_CNT                  (8U * 10U)       /* 10 SOF ticks idle timeout */
#define USBH_CDC_NCM_BULK_IN_NAK_CHECK_MAX_CNT             (8U * 5U)        /* 5 SOF ticks NAK check */

/* intr_rx busy timeout: fixed count, independent of the device bInterval
 * (a tiny bInterval would otherwise resubmit before the device can reply). */
#define USBH_CDC_NCM_INTR_BUSY_MAX_CNT                     (8U * 5U)       /* 5 SOF ticks */

/* ncm ethernet connect status check */
#define USBH_CDC_NCM_ETH_STATUS_CHECK_TIME                  300U           /* 300 ms */

/* Upper bound for deinit to wait for the tasks inside usbh_cdc_ncm_transmit() to leave. A
 * woken caller leaves within microseconds, only a starved one takes longer. */
#define USBH_CDC_NCM_TX_EXIT_TIMEOUT_MS                    500U

/* CLEAR_FEATURE(ENDPOINT_HALT) attempts before the device is re-enumerated */
#define USBH_CDC_NCM_HALT_RETRY_MAX_CNT                    3U

/* Realtek dongle capability flags for vendor-specific initialization */
#define CDC_NCM_CAP_MODIFY_MAC                  (BIT0)  /* MAC Settings, enables the function of overwriting */
#define CDC_NCM_CAP_RCR_CMD                     (BIT1)  /* RCR (Receive Configuration Register) via vendor request */
#define CDC_NCM_CAP_MODIFY_LED                  (BIT2)

/* Private types -------------------------------------------------------------*/

/**
 * @brief NCM dongle MAC source type.
 */
typedef enum {
	CDC_NCM_MAC_UNINIT = 0U,        /**< MAC not yet initialized */
	CDC_NCM_MAC_DONGLE_SUPPLY,      /**< Extracted from dongle iMACAddress string */
	CDC_NCM_MAC_UPPER_LAYER_SET,    /**< Provided by app via usbh_cdc_ncm_config_t::mac_value */
	CDC_NCM_MAC_RANDOM_SET,         /**< Randomly generated (dongle has no valid MAC) */
	CDC_NCM_MAC_TYPE_MAX,
} usbh_cdc_ncm_dongle_mac_type_t;

/**
 * @brief NCM Alternate Setting state machine.
 */
typedef enum {
	CDC_NCM_STATE_AT_SETTING_IDLE = 0U,     /**< No setting in progress */
	CDC_NCM_STATE_GET_MAC_STR,              /**< Read MAC string descriptor from dongle */
	CDC_NCM_STATE_CTRL_ALT_SETTING,         /**< Select data interface alt setting */
	CDC_NCM_STATE_GET_NTB_PARAMETERS,       /**< Get NTB parameters from dongle */
	CDC_NCM_STATE_SET_NTB_INPUT_SIZE,       /**< Set NTB input size */
	/* Realtek vendor-specific initialization */
	CDC_NCM_STATE_CTRL_RCR_GET,             /**< Read RCR register */
	CDC_NCM_STATE_CTRL_RCR_SET,             /**< Write RCR register */
	CDC_NCM_STATE_CTRL_MAC_GET_LOCK,        /**< Get MAC lock register */
	CDC_NCM_STATE_CTRL_MAC_DISABLE_LOCK,    /**< Disable MAC lock */
	CDC_NCM_STATE_CTRL_MAC_SET_MAC1,        /**< Write MAC[0..3] to chip */
	CDC_NCM_STATE_CTRL_MAC_SET_MAC2,        /**< Write MAC[4..5] to chip */
	CDC_NCM_STATE_CTRL_MAC_ENABLE_LOCK,     /**< Enable MAC lock (MAC write done) */
	CDC_NCM_STATE_AT_SETTING_MAX,           /**< Finished, proceed to TRANSFER */
} usbh_cdc_ncm_at_set_state_t;

/* NCM state */
typedef enum {
	CDC_NCM_STATE_IDLE = 0U,
	CDC_NCM_STATE_TRANSFER,
	CDC_NCM_STATE_ERROR,
} usbh_cdc_ncm_state_t;

typedef struct {
	u16 pid;
	u16 feature;
} __PACKED usbh_cdc_ncm_priv_config_t;

/* Private macros ------------------------------------------------------------*/

#define USBH_CDC_NCM_FREE_MEM(x)        do { if ((x) != NULL) { usb_os_mfree((void *)(x)); (x) = NULL; } } while (0U)

/* Private function prototypes -----------------------------------------------*/

static int usbh_cdc_ncm_attach(usb_host_t *host);
static void usbh_cdc_ncm_detach(usb_host_t *host);
static void usbh_cdc_ncm_process(usb_host_t *host, usbh_drv_msg_t *msg);
static int usbh_cdc_ncm_setup(usb_host_t *host);
static void usbh_cdc_ncm_sof(usb_host_t *host);
static void usbh_cdc_ncm_process_bulk_out(usb_host_t *host);
static void usbh_cdc_ncm_process_bulk_in(usb_host_t *host);
static void usbh_cdc_ncm_process_intr_in(usb_host_t *host);
static int usbh_cdc_ncm_bulk_receive(void);
static int usbh_cdc_ncm_bulk_tx(void);
#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
static void usbh_cdc_ncm_agg_reset_buf(u8 b);
#else
static int usbh_cdc_ncm_bulk_send(u8 *buf, u32 len);
#endif
static int usbh_cdc_ncm_cb_intr_receive(u8 *buf, u32 length);
static int usbh_cdc_ncm_cb_bulk_send(int state);
static int usbh_cdc_ncm_intr_receive(void);
static int usbh_cdc_ncm_tx_status_check(void);
static void usbh_cdc_ncm_deinit_all_pipe(void);
static void usbh_cdc_ncm_halt(usb_host_t *host, const usbh_pipe_t *pipe);
static void usbh_cdc_ncm_process_halt(usb_host_t *host);
#if USBH_CDC_NCM_STATE_TRACE_ENABLE
static void usbh_cdc_ncm_trace_task_init(void);
static void usbh_cdc_ncm_trace_task_deinit(void);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "NCMH";

/* USB CDC NCM device identification */
static const usbh_dev_id_t usbh_cdc_ncm_devs[] = {
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO,
		.bInterfaceClass = USB_CDC_COMM_INTERFACE_CLASS_CODE,
		.bInterfaceSubClass = USB_CDC_NCM_SUBCLASS_CODE,
		.bInterfaceProtocol = 0x00,
	},
	{
	},
};

/* USB Host CDC NCM class driver */
static const usbh_class_driver_t usbh_cdc_ncm_driver = {
	.id_table = usbh_cdc_ncm_devs,
	.attach  = usbh_cdc_ncm_attach,
	.detach  = usbh_cdc_ncm_detach,
	.setup   = usbh_cdc_ncm_setup,
	.process = usbh_cdc_ncm_process,
	.sof = usbh_cdc_ncm_sof,
};

static usbh_cdc_ncm_host_t usbh_cdc_ncm_host;

/* Realtek dongle PID-to-feature mapping for vendor-specific initialization */
static const usbh_cdc_ncm_priv_config_t usbh_cdc_ncm_rtk_dongle[] = {
	{0x8156, CDC_NCM_CAP_RCR_CMD | CDC_NCM_CAP_MODIFY_MAC | CDC_NCM_CAP_MODIFY_LED},
	{0U, 0U},
};

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

static int usbh_cdc_ncm_support_feature(u16 pid, u16 feature_mask)
{
	u8 i;

	for (i = 0; usbh_cdc_ncm_rtk_dongle[i].pid != 0; i++) {
		if (usbh_cdc_ncm_rtk_dongle[i].pid == pid) {
			return (usbh_cdc_ncm_rtk_dongle[i].feature & feature_mask) ? RTK_SUCCESS : -RTK_ERR_BADARG;
		}
	}
	return -RTK_ERR_BADARG;
}

/* USB basic functions */

static u16 usbh_cdc_ncm_get_eth_status_check_time(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	if ((cdc->host != NULL) && (cdc->host->dev_speed == USB_SPEED_HIGH)) {
		return 8 * USBH_CDC_NCM_ETH_STATUS_CHECK_TIME;
	}

	return USBH_CDC_NCM_ETH_STATUS_CHECK_TIME;
}

/**
  * @brief  Convert one hexadecimal digit to its value
  * @param  hex_num: ASCII character
  * @retval 0x0 to 0xF, 0xFF when the character is not a hexadecimal digit
  */
static u8 usbh_cdc_ncm_hex_to_char(u8 hex_num)
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
  * @return RTK_SUCCESS if enumeration is complete, -RTK_ERR_BUSY otherwise.
  */
static int usbh_cdc_ncm_check_enum_status(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	if ((cdc->host != NULL) && (cdc->host->connect_state >= USBH_STATE_SETUP)) {
		return RTK_SUCCESS;
	}

	return -RTK_ERR_BUSY;
}

/* Add parse to get the ethernet status */
static int usbh_cdc_ncm_cb_intr_receive(u8 *buf, u32 length)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	if ((buf != NULL) && (length >= 8U)) {
		/* A1 00 00 00 01 00 00 00 - Network Connection */
		if (length == 8 && buf[0] == 0xA1 && buf[1] == USB_CDC_NOTIFY_NETWORK_CONNECTION) {
			cdc->eth_hw_connect = buf[2];
			if (cdc->eth_hw_connect && (cdc->intr_check_tick != usbh_cdc_ncm_get_eth_status_check_time())) {
				cdc->intr_check_tick = usbh_cdc_ncm_get_eth_status_check_time();
			} else if ((cdc->eth_hw_connect == 0) && (cdc->intr_rx.pipe.ep_interval != 0)) {
				cdc->intr_check_tick = cdc->intr_rx.pipe.ep_interval;
			}
			cdc->intr_rx.pipe.max_timeout_tick = cdc->intr_check_tick;
		}
		/* A1 2A 00 00 01 00 00 08  00 00 00 00 00 00 00 00 - Connection Speed Change */
		else if (length == 16 && buf[0] == 0xA1 && buf[1] == USB_CDC_NOTIFY_CONNECTION_SPEED_CHANGE) {
			/* Speed change notification - could be parsed if needed */
		}
	}

	return RTK_SUCCESS;
}

/* USB CDC NCM control functions */

/**
  * @brief  Get string descriptor by str id
  * @param  host: Host handle
  * @param  pbuf: buffer handle which will save the string value
  * @param  buflen: buffer length
  * @param  str_id: string index
  * @retval Status
  */
static int usbh_cdc_ncm_process_get_string(usb_host_t *host, u8 *pbuf, u16 buflen, u8 str_id)
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
  * @brief  Settle the MAC for a device that supplies no usable one
  * @note   A MAC the user supplied is kept. Otherwise a random locally administered unicast one is used (Ref IEEE
  *         802 9.2): I/G bit 0 so it is never a group address, U/L bit 1 so it can not clash with a vendor one.
  * @retval None
  */
static void usbh_cdc_ncm_set_fallback_mac(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	if (cdc->mac_src_type != CDC_NCM_MAC_UPPER_LAYER_SET) {
		TRNG_get_random_bytes(cdc->mac, CDC_NCM_MAC_STR_LEN);
		cdc->mac[0] = (u8)((cdc->mac[0] & 0xFEU) | 0x02U);
		cdc->mac_src_type = CDC_NCM_MAC_RANDOM_SET;
	}
	cdc->mac_valid = 1U;
	RTK_LOGS(TAG, RTK_LOG_WARN, "Fallback mac[%02x %02x %02x %02x %02x %02x]\n", cdc->mac[0], cdc->mac[1], cdc->mac[2], cdc->mac[3],
			 cdc->mac[4], cdc->mac[5]);
}

/**
  * @brief  Decode the iMACAddress string descriptor in dongle_ctrl_buf
  * @note   Ref ECM 1.2 5.4: 12 hexadecimal digits, most significant first, in UTF-16LE. A short or foreign descriptor,
  *         a non hexadecimal digit, or an all-zero address is no MAC, and leaves cdc->mac untouched.
  * @param  host: Host handle
  * @retval RTK_SUCCESS when cdc->mac holds the device MAC, -RTK_ERR_BADARG otherwise
  */
static int usbh_cdc_ncm_parse_mac_str(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	const u8 *buf = cdc->dongle_ctrl_buf;
	u8 mac[CDC_NCM_MAC_STR_LEN];
	u8 hi;
	u8 lo;
	u8 nonzero = 0U;
	u8 i;

	if ((usbh_get_last_ctrl_in_size(host) < USBH_CDC_NCM_MAC_DESC_LEN) || (buf[0] < USBH_CDC_NCM_MAC_DESC_LEN) ||
		(buf[1] != USB_DESC_TYPE_STRING)) {
		return -RTK_ERR_BADARG;
	}

	for (i = 0U; i < CDC_NCM_MAC_STR_LEN; i++) {
		hi = usbh_cdc_ncm_hex_to_char(buf[2U + (4U * i)]);
		lo = usbh_cdc_ncm_hex_to_char(buf[4U + (4U * i)]);
		if ((hi > 0xFU) || (lo > 0xFU)) {
			return -RTK_ERR_BADARG;
		}
		mac[i] = (u8)((hi << 4U) | lo);
		nonzero |= mac[i];
	}

	if (nonzero == 0U) {
		return -RTK_ERR_BADARG;
	}

	usb_os_memcpy((void *)cdc->mac, (const void *)mac, CDC_NCM_MAC_STR_LEN);
	return RTK_SUCCESS;
}

/**
  * @brief  Get mac string descriptor
  * @note   A device that refuses the string, or returns no usable MAC, gets the fallback rather than being retried:
  *         the answer does not change, and retrying only ran into the class setup timeout that drops the class.
  * @param  host: Host handle
  * @retval RTK_SUCCESS when the MAC is settled, -RTK_ERR_BUSY while the request is in flight
  */
static int usbh_cdc_ncm_read_mac_str(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	int status;

	if ((cdc->mac_src_type == CDC_NCM_MAC_UPPER_LAYER_SET)
		&& (cdc->vid == USBH_CDC_NCM_DEFAULT_VID)
		&& (usbh_cdc_ncm_support_feature(cdc->pid, CDC_NCM_CAP_MODIFY_MAC) == RTK_SUCCESS)) {
		/* Programmed into the dongle later, see usbh_cdc_ncm_config_dongle_mac() */
		RTK_LOGS(TAG, RTK_LOG_INFO, "Upper set mac[%02x %02x %02x %02x %02x %02x]\n",
				 cdc->mac[0], cdc->mac[1], cdc->mac[2],
				 cdc->mac[3], cdc->mac[4], cdc->mac[5]);
		return RTK_SUCCESS;
	}

	status = usbh_cdc_ncm_process_get_string(host, cdc->dongle_ctrl_buf, USBH_CDC_NCM_MAC_STRING_LEN, cdc->iMACAddressStringId);
	if (status == -RTK_ERR_BUSY) {
		return status;
	}

	if ((status == RTK_SUCCESS) && (usbh_cdc_ncm_parse_mac_str(host) == RTK_SUCCESS)) {
		cdc->mac_src_type = CDC_NCM_MAC_DONGLE_SUPPLY;
		cdc->mac_valid = 1U;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Dongle mac[%02x %02x %02x %02x %02x %02x]\n",
				 cdc->mac[0], cdc->mac[1], cdc->mac[2],
				 cdc->mac[3], cdc->mac[4], cdc->mac[5]);
	} else {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Get MAC str %d err %d\n", cdc->iMACAddressStringId, status);
		usbh_cdc_ncm_set_fallback_mac();
	}

	return RTK_SUCCESS;
}

/**
  * @brief  Usb alt setting function
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ncm_process_set_alt(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_REQ_SET_INTERFACE;
	/* Per USB spec 9.4.10: wValue = Alternate Setting, wIndex = Interface Number */
	setup.req.wValue = cdc->data_alt_set;
	setup.req.wIndex = cdc->data_itf_id;
	setup.req.wLength = 0U;

	return usbh_ctrl_request(host, &setup, NULL);
}

/**
  * @brief  Get NCM NTB Parameters
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ncm_process_get_ntb_parameters(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_NCM_GET_NTB_PARAMETERS;
	setup.req.wValue = 0U;
	/* Class-specific NCM requests target the Communication (control) interface */
	setup.req.wIndex = cdc->ctrl_itf_id;
	setup.req.wLength = sizeof(usb_cdc_ncm_ntb_parameters_t);

	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

/**
  * @brief  Set NCM NTB Input Size
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ncm_process_set_ntb_input_size(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_NCM_SET_NTB_INPUT_SIZE;
	setup.req.wValue = 0U;
	/* Class-specific NCM requests target the Communication (control) interface */
	setup.req.wIndex = cdc->ctrl_itf_id;
	setup.req.wLength = 4U;

	/* Write ntb_in_max_size into dongle_ctrl_buf */
	cdc->dongle_ctrl_buf[0] = (u8)(cdc->ntb_in_max_size & 0xFF);
	cdc->dongle_ctrl_buf[1] = (u8)((cdc->ntb_in_max_size >> 8) & 0xFF);
	cdc->dongle_ctrl_buf[2] = (u8)((cdc->ntb_in_max_size >> 16) & 0xFF);
	cdc->dongle_ctrl_buf[3] = (u8)((cdc->ntb_in_max_size >> 24) & 0xFF);

	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

/******************Realtek vendor-specific register access*********************/
/**
  * @brief  Read a 4-byte register from the Realtek chip via vendor control request.
  */
static int usbh_cdc_ncm_vendor_reg_read(usb_host_t *host, u16 reg_addr, u16 port_sel)
{
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
	setup.req.bRequest = 0x05;
	setup.req.wValue = reg_addr;
	setup.req.wIndex = port_sel;
	setup.req.wLength = 4;

	return usbh_ctrl_request(host, &setup, usbh_cdc_ncm_host.dongle_ctrl_buf);
}

/**
  * @brief  Write a 4-byte register to the Realtek chip via vendor control request.
  */
static int usbh_cdc_ncm_vendor_reg_write(usb_host_t *host, u16 reg_addr, u16 port_sel)
{
	usbh_setup_req_t setup;

	setup.req.bmRequestType = 0x40;  /* H2D | Vendor | Device */
	setup.req.bRequest = 0x05;
	setup.req.wValue = reg_addr;
	setup.req.wIndex = port_sel;
	setup.req.wLength = 4;

	return usbh_ctrl_request(host, &setup, usbh_cdc_ncm_host.dongle_ctrl_buf);
}

/******************Set MAC for Realtek dongle *****************************/
static int usbh_cdc_ncm_process_mac_get_lock(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	int ret;

	ret = usbh_cdc_ncm_vendor_reg_read(host, 0xE81C, 0x010F);
	if (ret == RTK_SUCCESS) {
		usb_os_memcpy((void *)cdc->mac_ctrl_lock, (const void *)cdc->dongle_ctrl_buf, CDC_NCM_CTRL_REG_BUF_LEN);
	}
	return ret;
}

static int usbh_cdc_ncm_process_mac_set_dis_lock(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	cdc->dongle_ctrl_buf[0] = 0xD0;
	cdc->dongle_ctrl_buf[1] = 0;
	cdc->dongle_ctrl_buf[2] = 0;
	cdc->dongle_ctrl_buf[3] = 0;
	return usbh_cdc_ncm_vendor_reg_write(host, 0xE81C, 0x010F);
}

static int usbh_cdc_ncm_process_mac_set_mac1(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	usb_os_memcpy((void *)cdc->dongle_ctrl_buf, (const void *)cdc->mac, CDC_NCM_CTRL_REG_BUF_LEN);
	return usbh_cdc_ncm_vendor_reg_write(host, 0xC000, 0x010F);
}

static int usbh_cdc_ncm_process_mac_set_mac2(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	cdc->dongle_ctrl_buf[0] = cdc->mac[4];
	cdc->dongle_ctrl_buf[1] = cdc->mac[5];
	cdc->dongle_ctrl_buf[2] = 0xFF;
	cdc->dongle_ctrl_buf[3] = 0xFF;
	return usbh_cdc_ncm_vendor_reg_write(host, 0xC004, 0x0103);
}

static int usbh_cdc_ncm_process_mac_en_lock(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	cdc->dongle_ctrl_buf[0] = 0x10;
	cdc->dongle_ctrl_buf[1] = 0;
	cdc->dongle_ctrl_buf[2] = 0;
	cdc->dongle_ctrl_buf[3] = 0;
	return usbh_cdc_ncm_vendor_reg_write(host, 0xE81C, 0x010F);
}

static void usbh_cdc_ncm_config_dongle_mac(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	int state;

	if ((cdc->vid != USBH_CDC_NCM_DEFAULT_VID)
		|| (cdc->mac_src_type == CDC_NCM_MAC_DONGLE_SUPPLY)
		|| (usbh_cdc_ncm_support_feature(cdc->pid, CDC_NCM_CAP_MODIFY_MAC) != RTK_SUCCESS)) {
		/* Non-Realtek dongle or MAC from dongle string: skip vendor write */
		cdc->sub_status = CDC_NCM_STATE_AT_SETTING_MAX;
		return;
	}

	switch (cdc->sub_status) {
	case CDC_NCM_STATE_CTRL_MAC_GET_LOCK:
		state = usbh_cdc_ncm_process_mac_get_lock(host);
		if (state == RTK_SUCCESS) {
			cdc->sub_status++;
		} else if (state != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get MAC lock err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_NCM_STATE_CTRL_MAC_DISABLE_LOCK:
		state = usbh_cdc_ncm_process_mac_set_dis_lock(host);
		if (state == RTK_SUCCESS) {
			cdc->sub_status++;
		} else if (state != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Dis MAC lock err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_NCM_STATE_CTRL_MAC_SET_MAC1:
		state = usbh_cdc_ncm_process_mac_set_mac1(host);
		if (state == RTK_SUCCESS) {
			cdc->sub_status++;
		} else if (state != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set MAC1 err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_NCM_STATE_CTRL_MAC_SET_MAC2:
		state = usbh_cdc_ncm_process_mac_set_mac2(host);
		if (state == RTK_SUCCESS) {
			cdc->sub_status++;
		} else if (state != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set MAC2 err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_NCM_STATE_CTRL_MAC_ENABLE_LOCK:
		state = usbh_cdc_ncm_process_mac_en_lock(host);
		if (state == RTK_SUCCESS) {
			cdc->sub_status++;
			cdc->mac_valid = 1;
			RTK_LOGS(TAG, RTK_LOG_INFO, "MAC set success\n");
		} else if (state != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "En MAC lock err\n");
			usb_os_sleep_ms(100);
		}
		break;

	default:
		cdc->sub_status = CDC_NCM_STATE_AT_SETTING_MAX;
		break;
	}
}

static int usbh_cdc_ncm_ctrl_setting(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	/* -RTK_ERR_BUSY = sequence still running; only the default case (sub_status past
	 * the last step) reports RTK_SUCCESS. Must not be an error code: the core treats
	 * any non-OK, non-BUSY setup() status as terminal and drops the class. */
	int ret_state = -RTK_ERR_BUSY;
	int state = RTK_SUCCESS;

	switch (cdc->sub_status) {
	case CDC_NCM_STATE_AT_SETTING_IDLE:
		/* Descriptor parsing always leaves sub_status at GET_MAC_STR or further;
		 * self-advance anyway so the sequence can never stall here. */
		cdc->sub_status = CDC_NCM_STATE_CTRL_ALT_SETTING;
		break;

	case CDC_NCM_STATE_GET_MAC_STR:
		/* Returns RTK_SUCCESS once the MAC is settled, the fallback one on any failure */
		if (usbh_cdc_ncm_read_mac_str(host) == RTK_SUCCESS) {
			cdc->sub_status++;
		}
		break;

	case CDC_NCM_STATE_CTRL_ALT_SETTING:
		state = usbh_cdc_ncm_process_set_alt(host);
		if (state == RTK_SUCCESS) {
			cdc->sub_status++;
		} else if (state != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "NCM alt setting err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_NCM_STATE_GET_NTB_PARAMETERS:
		state = usbh_cdc_ncm_process_get_ntb_parameters(host);
		if (state == RTK_SUCCESS) {
			usb_cdc_ncm_ntb_parameters_t *params = (usb_cdc_ncm_ntb_parameters_t *)cdc->dongle_ctrl_buf;
			/* Store NTB parameters */
			cdc->ntb_params = *params;
			/* wNdpOutAlignment shall be a power of 2 and at least 4 (NCM 1.0 Table 6-3); the TX path
			   places the NDP by it, so fall back to the default for a value that breaks the rule */
			if ((cdc->ntb_params.wNdbOutAlignment < USB_CDC_NCM_DATAGRAM_ALIGN) ||
				((cdc->ntb_params.wNdbOutAlignment & (cdc->ntb_params.wNdbOutAlignment - 1U)) != 0U)) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Bad NDP align %d\n", cdc->ntb_params.wNdbOutAlignment);
				cdc->ntb_params.wNdbOutAlignment = USB_CDC_NCM_DATAGRAM_ALIGN;
			}
			cdc->ntb_in_max_size = params->dwNtbInMaxSize;
			cdc->ntb_out_max_size = params->dwNtbOutMaxSize;

			/* IN direction: we must never let the device send more than our RX buffer,
			 * so clamp before SET_NTB_INPUT_SIZE echoes the value back to the device.
			 * OUT direction: dwNtbOutMaxSize is the device's own limit and is only
			 * used to cap what we build, so keep it verbatim (the TX helpers clamp it
			 * against the local TX buffer). */
			if (cdc->ntb_in_max_size > USBH_CDC_NCM_RX_NTB_BUF_SIZE) {
				cdc->ntb_in_max_size = USBH_CDC_NCM_RX_NTB_BUF_SIZE;
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "NCM NTB: InMax=%d OutMax=%d\n",
					 cdc->ntb_in_max_size, cdc->ntb_out_max_size);
			cdc->sub_status++;
		} else if (state != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get NTB params err\n");
			/* Use defaults */
			cdc->ntb_in_max_size = USB_CDC_NCM_DEFAULT_NTB_INPUT_SIZE;
			cdc->ntb_out_max_size = USB_CDC_NCM_DEFAULT_NTB_INPUT_SIZE;
			cdc->sub_status++;
		}
		break;

	case CDC_NCM_STATE_SET_NTB_INPUT_SIZE:
		state = usbh_cdc_ncm_process_set_ntb_input_size(host);
		if (state == RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Set NTB Input Size: %d\n", cdc->ntb_in_max_size);
			cdc->sub_status++;
		} else if (state != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set NTB input size err\n");
			cdc->sub_status++;
		}
		break;
	case CDC_NCM_STATE_CTRL_RCR_GET:
		if (usbh_cdc_ncm_support_feature(cdc->pid, CDC_NCM_CAP_RCR_CMD) != RTK_SUCCESS) {
			cdc->sub_status = CDC_NCM_STATE_CTRL_MAC_GET_LOCK;
			break;
		}
		usb_os_memset((void *)cdc->rcr, 0, 4);
		state = usbh_cdc_ncm_vendor_reg_read(host, 0xC010, 0x0100);
		if (state == RTK_SUCCESS) {
			usb_os_memcpy((void *)cdc->rcr, (const void *)cdc->dongle_ctrl_buf, CDC_NCM_CTRL_REG_BUF_LEN);
			cdc->sub_status++;
		} else if (state != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get RCR err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_NCM_STATE_CTRL_RCR_SET:
		if (usbh_cdc_ncm_support_feature(cdc->pid, CDC_NCM_CAP_RCR_CMD) != RTK_SUCCESS) {
			cdc->sub_status = CDC_NCM_STATE_CTRL_MAC_GET_LOCK;
			break;
		}
		/* set bit 0~3 set 1 will enable PING & UDP transfer */
		cdc->rcr[0] = cdc->rcr[0] | 0x0F;
		usb_os_memcpy((void *)cdc->dongle_ctrl_buf, (const void *)cdc->rcr, CDC_NCM_CTRL_REG_BUF_LEN);
		state = usbh_cdc_ncm_vendor_reg_write(host, 0xC010, 0x010F);
		if (state == RTK_SUCCESS) {
			cdc->sub_status++;
		} else if (state != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set RCR err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_NCM_STATE_CTRL_MAC_GET_LOCK:
	case CDC_NCM_STATE_CTRL_MAC_DISABLE_LOCK:
	case CDC_NCM_STATE_CTRL_MAC_SET_MAC1:
	case CDC_NCM_STATE_CTRL_MAC_SET_MAC2:
	case CDC_NCM_STATE_CTRL_MAC_ENABLE_LOCK:
		usbh_cdc_ncm_config_dongle_mac(host);
		if (cdc->sub_status >= CDC_NCM_STATE_AT_SETTING_MAX) {
			ret_state = RTK_SUCCESS;
		}
		break;

	default:
		ret_state = RTK_SUCCESS;
		break;
	}

	return ret_state;
}

static void usbh_cdc_ncm_dump_ncm_cfgdesc(void)
{
#if USBH_CDC_NCM_ENABLE_DUMP_DESCRIPTOR_PARSE
	usbh_cdc_ncm_host_t   *cdc = &usbh_cdc_ncm_host;
	usbh_ep_desc_t        *ep_desc;
	usbh_cdc_ncm_pipe_info_t *pipe_info;

	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------Dump Descriptor Start------------------\n");

	/* dump ncm control */
	pipe_info = &(cdc->intr_rx);
	ep_desc = &(pipe_info->ep_desc);
	RTK_LOGS(TAG, RTK_LOG_INFO, "INTR addr(0x%x)MPS(%d)pipe(%d)interval(%d) MAC(%d)\n",
			 ep_desc->bEndpointAddress, ep_desc->wMaxPacketSize, pipe_info->pipe.pipe_num, pipe_info->pipe.ep_interval, cdc->iMACAddressStringId);

	/* dump ncm data */
	pipe_info = &(cdc->bulk_tx);
	ep_desc = &(pipe_info->ep_desc);
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK out:addr(0x%x)MPS(%d)pipe(%d)interval(%d)\n",
			 ep_desc->bEndpointAddress, ep_desc->wMaxPacketSize, pipe_info->pipe.pipe_num, pipe_info->pipe.ep_interval);

	pipe_info = &(cdc->bulk_rx);
	ep_desc = &(pipe_info->ep_desc);
	RTK_LOGS(TAG, RTK_LOG_INFO, "in:addr(0x%x)MPS(%d)pipe(%d)interval(%d)\n",
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
  * @param  itf_data: NCM comm interface data
  * @param  subtype: bDescriptorSubtype to look for
  * @param  min_len: Smallest bLength the caller reads
  * @retval The descriptor, NULL if absent or truncated
  */
static const u8 *usbh_cdc_ncm_find_func_desc(const usbh_itf_data_t *itf_data, u8 subtype, u8 min_len)
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
  * @retval RTK_SUCCESS when both are present and no other endpoint is, -RTK_ERR_BADARG otherwise
  */
static int usbh_cdc_ncm_check_data_eps(usbh_itf_desc_t *itf_desc, usbh_ep_desc_t **bulk_in, usbh_ep_desc_t **bulk_out)
{
	usbh_ep_desc_t *ep;
	u8 i;

	*bulk_in = NULL;
	*bulk_out = NULL;

	/* Ref NCM 1.0 5.3: the data interface carries one BULK IN and one BULK OUT, nothing else */
	if (itf_desc->bNumEndpoints != 2U) {
		return -RTK_ERR_BADARG;
	}

	for (i = 0U; i < itf_desc->bNumEndpoints; i++) {
		ep = &itf_desc->ep_desc_array[i];
		if ((ep->bmAttributes & USB_EP_XFER_TYPE_MASK) != USB_CH_EP_TYPE_BULK) {
			return -RTK_ERR_BADARG;
		}
		if (USB_EP_IS_IN(ep->bEndpointAddress)) {
			*bulk_in = ep;
		} else {
			*bulk_out = ep;
		}
	}

	return ((*bulk_in != NULL) && (*bulk_out != NULL)) ? RTK_SUCCESS : -RTK_ERR_BADARG;
}

/**
  * @brief  Locate the NCM function of the device
  * @note   Read only, shared by attach() and usbh_cdc_ncm_check_config_desc() so a configuration that passes the
  *         check is one attach() takes. The comm interface carries one INTR IN endpoint (Ref NCM 1.0 5.2). The
  *         data interface has protocol 0x01 (NTB) and is the one its Union names, falling back to the only one
  *         present; its operational setting is the one with the BULK pair (Ref NCM 1.0 5.3, alternate setting 0
  *         has no endpoint). A device off these rules is refused on purpose rather than bound to a guessed
  *         interface.
  * @param  host: Host handle
  * @param  ctrl: Set to the comm interface data
  * @param  data: Set to the operational data interface setting
  * @retval RTK_SUCCESS when the whole function is found, -RTK_ERR_BADARG otherwise
  */
static int usbh_cdc_ncm_find_function(usb_host_t *host, usbh_itf_data_t **ctrl, usbh_itf_desc_t **data)
{
	usbh_dev_id_t dev_id = {0U};
	usbh_itf_data_t *itf_data;
	usbh_itf_desc_t *itf_desc;
	usbh_ep_desc_t *ep;
	usbh_ep_desc_t *bulk_in;
	usbh_ep_desc_t *bulk_out;
	const u8 *union_desc;
	u8 data_itf_num = 0xFFU;
	u8 alt_idx;

	*data = NULL;

	dev_id.bInterfaceClass = USB_CDC_COMM_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_NCM_SUBCLASS_CODE;
	dev_id.bInterfaceProtocol = 0x00U;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	if ((itf_data == NULL) || (itf_data->itf_desc_array == NULL)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get if fail\n");
		return -RTK_ERR_BADARG;
	}

	itf_desc = &itf_data->itf_desc_array[0];
	ep = &itf_desc->ep_desc_array[0];
	if ((itf_desc->bNumEndpoints != 1U) || ((ep->bmAttributes & USB_EP_XFER_TYPE_MASK) != USB_CH_EP_TYPE_INTR) ||
		!USB_EP_IS_IN(ep->bEndpointAddress)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad ctrl EP\n");
		return -RTK_ERR_BADARG;
	}
	*ctrl = itf_data;

	union_desc = usbh_cdc_ncm_find_func_desc(itf_data, USB_CDC_FUNC_DESC_UNION, USB_CDC_UNION_FUNC_DESC_MIN_SIZE);
	if (union_desc != NULL) {
		data_itf_num = union_desc[4];
	}

	/* Protocol 0x01 (NTB) keeps a composite device's other CDC-Data interface (e.g. ACM's, 0x00) out */
	dev_id.bInterfaceClass = USB_CDC_DATA_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_SUBCLASS_RESERVED;
	dev_id.bInterfaceProtocol = USB_CDC_DATA_PROTOCOL_NETWORK_TRANSFER_BLOCK;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	/* Without a Union only a sole data interface is unambiguous */
	if ((data_itf_num == 0xFFU) && ((itf_data == NULL) || (itf_data->next != NULL))) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No union\n");
		return -RTK_ERR_BADARG;
	}
	while ((itf_data != NULL) && (itf_data->itf_desc_array != NULL) && (data_itf_num != 0xFFU) &&
		   (itf_data->itf_desc_array[0].bInterfaceNumber != data_itf_num)) {
		itf_data = itf_data->next;
	}
	if ((itf_data == NULL) || (itf_data->itf_desc_array == NULL)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No data if %d\n", data_itf_num);
		return -RTK_ERR_BADARG;
	}

	for (alt_idx = 0U; alt_idx < itf_data->alt_setting_cnt; alt_idx++) {
		if (usbh_cdc_ncm_check_data_eps(&itf_data->itf_desc_array[alt_idx], &bulk_in, &bulk_out) == RTK_SUCCESS) {
			*data = &itf_data->itf_desc_array[alt_idx];
			return RTK_SUCCESS;
		}
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "No bulk pair\n");
	return -RTK_ERR_BADARG;
}

/**
  * @brief  Take the endpoints and descriptor indices of the attached device
  * @note   Every field written here describes the device being attached, and is reset first so nothing of a
  *         previous device survives into this one.
  * @param  host: Host handle
  * @retval RTK_SUCCESS on success, -RTK_ERR_BADARG when the device is no usable NCM function
  */
static int usbh_cdc_ncm_parse_interface_desc(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	const u8 *desc;
	usbh_itf_data_t *ctrl_itf;
	usbh_itf_desc_t *data_itf_desc;
	usbh_ep_desc_t *bulk_in;
	usbh_ep_desc_t *bulk_out;

	cdc->vid = host->dev_desc->idVendor;
	cdc->pid = host->dev_desc->idProduct;
	cdc->sub_status = CDC_NCM_STATE_AT_SETTING_IDLE;
	cdc->iMACAddressStringId = 0U;
	cdc->intr_rx.valid = 0U;
	cdc->bulk_rx.valid = 0U;
	cdc->bulk_tx.valid = 0U;

	if (usbh_cdc_ncm_find_function(host, &ctrl_itf, &data_itf_desc) != RTK_SUCCESS) {
		return -RTK_ERR_BADARG;
	}

	/* Save the Communication (control) interface number for class-specific requests */
	cdc->ctrl_itf_id = ctrl_itf->itf_desc_array[0].bInterfaceNumber;

	desc = usbh_cdc_ncm_find_func_desc(ctrl_itf, USB_CDC_FUNC_DESC_ETHERNET_NETWORKING, (u8)sizeof(usb_cdc_ncm_ethernet_function_desc_t));
	if (desc != NULL) {
		cdc->iMACAddressStringId = ((const usb_cdc_ncm_ethernet_function_desc_t *)(const void *)desc)->iMACAddress;
		RTK_LOGS(TAG, RTK_LOG_INFO, "NCM Mac string id(%d)\n", cdc->iMACAddressStringId);
	}

	desc = usbh_cdc_ncm_find_func_desc(ctrl_itf, USB_CDC_NCM_FUNC_DESC, (u8)sizeof(usb_cdc_ncm_function_desc_t));
	if (desc != NULL) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "NCM version 0x%04x caps 0x%02x\n", ((const usb_cdc_ncm_function_desc_t *)(const void *)desc)->bcdNcmVersion,
				 ((const usb_cdc_ncm_function_desc_t *)(const void *)desc)->bmNetworkCapabilities);
	}

	if (cdc->iMACAddressStringId != 0U) {
		cdc->sub_status = CDC_NCM_STATE_GET_MAC_STR;
	} else {
		/* No Ethernet Functional Descriptor (or no MAC string index): there is nothing to read, so skip the
		 * GET_MAC_STR step and use the user or a random MAC */
		RTK_LOGS(TAG, RTK_LOG_WARN, "No MAC str\n");
		usbh_cdc_ncm_set_fallback_mac();
		cdc->sub_status = CDC_NCM_STATE_CTRL_ALT_SETTING;
	}

	usb_os_memcpy((void *)&cdc->intr_rx.ep_desc, (const void *)&ctrl_itf->itf_desc_array[0].ep_desc_array[0], sizeof(usbh_ep_desc_t));
	cdc->intr_rx.valid = 1U;

	/* Already passed in usbh_cdc_ncm_find_function(), called again for the endpoint pointers */
	if (usbh_cdc_ncm_check_data_eps(data_itf_desc, &bulk_in, &bulk_out) != RTK_SUCCESS) {
		return -RTK_ERR_BADARG;
	}
	usb_os_memcpy((void *)&cdc->bulk_rx.ep_desc, (const void *)bulk_in, sizeof(usbh_ep_desc_t));
	cdc->bulk_rx.valid = 1U;
	usb_os_memcpy((void *)&cdc->bulk_tx.ep_desc, (const void *)bulk_out, sizeof(usbh_ep_desc_t));
	cdc->bulk_tx.valid = 1U;

	cdc->data_itf_id = data_itf_desc->bInterfaceNumber;
	cdc->data_alt_set = data_itf_desc->bAlternateSetting;
	RTK_LOGS(TAG, RTK_LOG_INFO, "Get NCM data if(%d)alt(%d)\n", cdc->data_itf_id, cdc->data_alt_set);

	return RTK_SUCCESS;
}

/* Datagram alignment constraints for the NTBs the host sends (OUT direction).
 * NCM 1.0 Table 6-3 keeps separate In/Out sets and the device validates an OUT
 * NTB against the Out fields, so the TX path must never use the In ones. Each
 * helper falls back to the spec default only when the device reports 0. */
static u16 usbh_cdc_ncm_tx_divisor(void)
{
	u16 d = usbh_cdc_ncm_host.ntb_params.wNdbOutDivisor;
	return (d == 0U) ? USB_CDC_NCM_DATAGRAM_ALIGN : d;
}

/* Required residue of a datagram offset modulo the divisor (NCM 1.0 3.3.1) */
static u16 usbh_cdc_ncm_tx_remainder(void)
{
	u16 div = usbh_cdc_ncm_tx_divisor();
	u16 rem = usbh_cdc_ncm_host.ntb_params.wNdbOutPayloadRemainder;
	return (rem < div) ? rem : 0U;
}

/* NDP alignment required by the device (host-to-device), default 4 */
static u16 usbh_cdc_ncm_tx_ndp_align(void)
{
	u16 a = usbh_cdc_ncm_host.ntb_params.wNdbOutAlignment;
	return (a == 0U) ? USB_CDC_NCM_DATAGRAM_ALIGN : a;
}

/* Smallest offset >= off that satisfies (offset % divisor) == payload remainder */
static u32 usbh_cdc_ncm_tx_align_dg(u32 off)
{
	u16 div = usbh_cdc_ncm_tx_divisor();
	u16 rem = usbh_cdc_ncm_tx_remainder();
	u32 cur = off % div;

	if (cur == rem) {
		return off;
	}
	return (cur < rem) ? (off + (rem - cur)) : (off + div - (cur - rem));
}

/* Usable NTB size: never exceed the local buffer nor the device's dwNtbOutMaxSize */
static u32 usbh_cdc_ncm_tx_max_ntb(u32 buf_size)
{
	u32 limit = usbh_cdc_ncm_host.ntb_out_max_size;

	if ((limit == 0U) || (limit > buf_size)) {
		limit = buf_size;
	}
	return limit;
}

#if !defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
/**
  * @brief  Build an NTB16 frame for transmission
  *         Wraps the Ethernet frame with NTH16 + NDP16 headers
  * @param  eth_buf: Ethernet frame data
  * @param  eth_len: Ethernet frame length
  * @param  ntb_buf: Output NTB buffer
  * @param  ntb_len: Output NTB length (returned)
  * @retval Status
  */
static int usbh_cdc_ncm_build_ntb16_frame(u8 *eth_buf, u16 eth_len, u8 *ntb_buf, u16 *ntb_len)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usb_cdc_ncm_nth16_t *nth;
	usb_cdc_ncm_ndp16_t *ndp;
	u32 ndp_align;
	u32 ndp_offset;
	u32 datagram_offset;

	if ((eth_buf == NULL) || (eth_len == 0U) || (ntb_buf == NULL) || (ntb_len == NULL)) {
		return -RTK_ERR_BADARG;
	}

	/* Reject oversized frames before any memcpy into the static tx buffer. */
	if (eth_len > USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "TX frame too large: %d\n", eth_len);
		return -RTK_ERR_BADARG;
	}

	/* NDP offset: right after the NTH16, aligned as the device requires */
	ndp_align = usbh_cdc_ncm_tx_ndp_align();
	ndp_offset = (USB_CDC_NCM_NTH16_LENGTH + (ndp_align - 1U)) & ~(ndp_align - 1U);

	/* Datagram offset: after the NDP, honoring wNdbOutDivisor / wNdbOutPayloadRemainder */
	datagram_offset = usbh_cdc_ncm_tx_align_dg(ndp_offset + USB_CDC_NCM_NDP16_MIN_LENGTH);

	/* Guard before any write: the offsets come from device parameters, and the framed NTB
	 * (the NDP lies below datagram_offset) must fit both the tx buffer and the device's
	 * dwNtbOutMaxSize, otherwise the device may drop the whole NTB. */
	if ((datagram_offset + eth_len) > usbh_cdc_ncm_tx_max_ntb(USBH_CDC_NCM_TX_BUF_SIZE)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "NTB too big: %d\n", datagram_offset + eth_len);
		return -RTK_ERR_BADARG;
	}

	/* NTH16 at start of NTB */
	nth = (usb_cdc_ncm_nth16_t *)ntb_buf;
	nth->dwSignature = USB_CDC_NCM_NTH16_SIGNATURE;
	nth->wHeaderLength = USB_CDC_NCM_NTH16_LENGTH;
	nth->wSequence = cdc->ntb_sequence;
	cdc->ntb_sequence++;

	/* NDP16 at ndp_offset */
	ndp = (usb_cdc_ncm_ndp16_t *)(ntb_buf + ndp_offset);
	ndp->dwSignature = USB_CDC_NCM_NDP16_NOCRC_SIGNATURE;
	ndp->wLength = USB_CDC_NCM_NDP16_MIN_LENGTH; /* 8-byte header + 1 datagram entry + 1 terminator entry */
	ndp->wNextFpIndex = 0; /* Last NDP */

	/* Fill NDP entries */
	ndp->aEntry[0].wDatagramIndex = (u16)datagram_offset;
	ndp->aEntry[0].wDatagramLength = eth_len;
	/* Terminator entry */
	ndp->aEntry[1].wDatagramIndex = 0;
	ndp->aEntry[1].wDatagramLength = 0;

	/* Copy Ethernet frame */
	usb_os_memcpy((void *)(ntb_buf + datagram_offset), (const void *)eth_buf, eth_len);

	/* Calculate total NTB length */
	*ntb_len = (u16)(datagram_offset + eth_len);

	/* Update NTH block length */
	nth->wBlockLength = *ntb_len;

	nth->wFpIndex = (u16)ndp_offset;

	return RTK_SUCCESS;
}
#endif /* !CONFIG_USBH_CDC_NCM_TX_AGGREGATION */

/**
  * @brief  Parse a received NTB16 block and deliver every Ethernet datagram it
  *         contains. An NTB may pack multiple datagrams across one or more NDPs,
  *         so the full NDP chain is walked and each datagram is passed to the
  *         upper layer in place (zero-copy).
  * @param  ntb_buf: Received NTB data
  * @param  ntb_len: Received NTB length (actual bytes transferred)
  * @retval Number of datagrams delivered, or negative on header error
  */
static int usbh_cdc_ncm_parse_ntb16_block(u8 *ntb_buf, u32 ntb_len)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usb_cdc_ncm_nth16_t *nth;
	usb_cdc_ncm_ndp16_t *ndp;
	const usb_cdc_ncm_ndp16_entry_t *entries;
	u16 ndp_offset;
	u16 prev_offset;
	u16 entry_cnt;
	u16 i;
	u32 block_len;
	int delivered = 0;

	if (ntb_buf == NULL || ntb_len < USB_CDC_NCM_NTH16_LENGTH) {
		return -RTK_ERR_BADARG;
	}

	nth = (usb_cdc_ncm_nth16_t *)ntb_buf;

	/* Verify NTH16 signature */
	if (nth->dwSignature != USB_CDC_NCM_NTH16_SIGNATURE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "NTH sig mismatch 0x%08x\n", nth->dwSignature);
		return -RTK_ERR_BADARG;
	}

	/* Trust the actual transfer size; wBlockLength is only used to bound the walk */
	block_len = nth->wBlockLength;
	if ((block_len == 0) || (block_len > ntb_len)) {
		block_len = ntb_len;
	}

	/* Walk the NDP chain (wFpIndex -> wNextFpIndex, 0 terminates).
	 * The chain must strictly move forward: a device (or a corrupted NTB) pointing
	 * wNextFpIndex back at the current or an earlier NDP passes every bounds and
	 * signature check below, so without the progress test the same datagrams would
	 * be delivered forever and the host task would never return. */
	ndp_offset = nth->wFpIndex;
	prev_offset = 0;
	while (ndp_offset != 0) {
		/* Ref NCM 1.0 3.3.1/3.3.2: an NDP lies past the NTH16 on a 4-byte boundary, which also keeps the
		 * packed header reads below on their natural alignment */
		if ((ndp_offset < USB_CDC_NCM_NTH16_LENGTH) || ((ndp_offset & 0x3U) != 0U)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad NDP off %d\n", ndp_offset);
			break;
		}
		if (((u32)ndp_offset + USB_CDC_NCM_NDP16_MIN_LENGTH) > block_len) {
			break;
		}
		if (ndp_offset <= prev_offset) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "NDP chain loop: off=%d prev=%d\n", ndp_offset, prev_offset);
			break;
		}
		prev_offset = ndp_offset;

		ndp = (usb_cdc_ncm_ndp16_t *)(ntb_buf + ndp_offset);

		/* Verify NDP16 signature (no-CRC variant) */
		if (ndp->dwSignature != USB_CDC_NCM_NDP16_NOCRC_SIGNATURE) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "NDP sig mismatch 0x%08x\n", ndp->dwSignature);
			break;
		}

		/* Datagram entries = (wLength - NDP16 header) / 4; last entry is a 0,0 terminator */
		if (ndp->wLength < (USB_CDC_NCM_NDP16_MIN_LENGTH - USB_CDC_NCM_NDP16_ENTRY_LENGTH)) {
			break;
		}
		/* The whole NDP (header + entry array, wLength bytes) must lie within the
		 * block, otherwise reading aEntry[i] below would run past the buffer. */
		if (((u32)ndp_offset + ndp->wLength) > block_len) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "NDP exceeds block: off=%d wLength=%d blk=%d\n",
					 ndp_offset, ndp->wLength, block_len);
			break;
		}
		entry_cnt = (ndp->wLength - USB_CDC_NCM_NDP16_HEADER_LENGTH) / USB_CDC_NCM_NDP16_ENTRY_LENGTH;

		/* Address the entry array through a pointer rather than ndp->aEntry[i]:
		 * aEntry is declared with 2 elements as a place-holder, while a real NDP
		 * carries entry_cnt of them (bounds-checked against block_len just above),
		 * so indexing the declared array would be out of bounds. */
		entries = (const usb_cdc_ncm_ndp16_entry_t *)(ntb_buf + ndp_offset + USB_CDC_NCM_NDP16_HEADER_LENGTH);

		for (i = 0; i < entry_cnt; i++) {
			u16 dg_index = entries[i].wDatagramIndex;
			u16 dg_len = entries[i].wDatagramLength;

			/* Null entry terminates the datagram list */
			if ((dg_index == 0) || (dg_len == 0)) {
				break;
			}

			/* A datagram lies past the NTH16 and within wBlockLength (Ref NCM 1.0 3.3.2); bytes past the
			 * block are not part of this NTB */
			if ((dg_index < USB_CDC_NCM_NTH16_LENGTH) || (((u32)dg_index + dg_len) > block_len)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "NCM data exceeds NTB len\n");
				break;
			}

			if ((cdc->cb != NULL) && (cdc->cb->received != NULL)) {
				cdc->cb->received(ntb_buf + dg_index, dg_len);
			}
			delivered++;
		}

		ndp_offset = ndp->wNextFpIndex;
	}

	return delivered;
}

/**
  * @brief  Usb Attach callback function
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ncm_attach(usb_host_t *host)
{
	int status = -RTK_ERR_BADARG;
	usbh_cdc_ncm_pipe_info_t *pipe_info;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	/* deinit_all_pipe() needs the host to close what a failed attach opened */
	cdc->host = host;

	if (usbh_cdc_ncm_parse_interface_desc(host) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse desc fail\n");
		goto attach_fail;
	}

	/* All three pipes are mandatory, see usbh_cdc_ncm_find_function() */
	pipe_info = &(cdc->intr_rx);
	if (usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc), &usbh_cdc_ncm_driver) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Open intr pipe fail\n");
		goto attach_fail;
	}
	/* The controller may fill a whole packet, adjusted to the DWORD boundary */
	pipe_info->pipe.xfer_buf = (u8 *)usb_os_malloc(usb_get_dma_len(pipe_info->pipe.ep_mps, pipe_info->pipe.ep_mps));
	if (pipe_info->pipe.xfer_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc intr rx buf fail\n");
		status = -RTK_ERR_NOMEM;
		goto attach_fail;
	}
	pipe_info->pipe.xfer_len = pipe_info->pipe.ep_mps;
	pipe_info->pipe.xfer_buf_len = usb_get_dma_len(pipe_info->pipe.ep_mps, pipe_info->pipe.ep_mps);
	pipe_info->pipe.xfer_state = USBH_EP_XFER_START;

	/* Widen the poll window past the 1-frame bInterval=1 case, else the SOF
	 * re-arm fires on the still in-flight INTR IN and hangs it. */
	cdc->intr_check_tick = pipe_info->pipe.ep_interval + USBH_CDC_NCM_INTR_BUSY_MAX_CNT;
	pipe_info->pipe.max_timeout_tick = USBH_CDC_NCM_INTR_BUSY_MAX_CNT;

	pipe_info = &(cdc->bulk_tx);
	if (usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc), &usbh_cdc_ncm_driver) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Open bulk out pipe fail\n");
		goto attach_fail;
	}
	pipe_info->pipe.max_timeout_tick = USBH_CDC_NCM_BULK_OUT_BUSY_MAX_CNT;

	pipe_info = &(cdc->bulk_rx);
	if (usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc), &usbh_cdc_ncm_driver) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Open bulk in pipe fail\n");
		goto attach_fail;
	}
	/* NCM use bulk, allocate buffer for NTB reception */
	pipe_info->pipe.xfer_buf = (u8 *)usb_os_malloc(USBH_CDC_NCM_RX_NTB_BUF_SIZE);
	if (pipe_info->pipe.xfer_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc bulk rx buf fail\n");
		status = -RTK_ERR_NOMEM;
		goto attach_fail;
	}
	pipe_info->pipe.xfer_len = USBH_CDC_NCM_RX_NTB_BUF_SIZE;
	pipe_info->pipe.xfer_buf_len = USBH_CDC_NCM_RX_NTB_BUF_SIZE;
	pipe_info->pipe.xfer_state = USBH_EP_XFER_START;

	pipe_info->pipe.max_timeout_tick = USBH_CDC_NCM_BULK_IN_IDLE_MAX_CNT;

	usbh_cdc_ncm_dump_ncm_cfgdesc();

	cdc->halt_retry_cnt = 0U;
	cdc->tx_err_cnt = 0U;
	cdc->rx_err_cnt = 0U;
	cdc->state = CDC_NCM_STATE_IDLE;

	if ((cdc->cb != NULL) && (cdc->cb->attached != NULL)) {
		cdc->cb->attached();
	}

	return RTK_SUCCESS;

attach_fail:
	/* The core calls no detach() for a failed attach: undo here, and withdraw the host so
	 * usbh_cdc_ncm_is_configured() never reports this class ready on another class's setup */
	usbh_cdc_ncm_deinit_all_pipe();
	cdc->host = NULL;
	return status;
}

/**
  * @brief  Usb Detach callback function.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_cdc_ncm_detach(usb_host_t *host)
{
	UNUSED(host);
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	cdc->state = CDC_NCM_STATE_IDLE;

	/* These describe the device that is leaving: a later attach must not inherit them. A MAC the user set
	 * through init is kept for the next device. */
	cdc->eth_hw_connect = 0;
	cdc->mac_valid = 0;
	cdc->vid = 0U;
	cdc->pid = 0U;
	if (cdc->mac_src_type != CDC_NCM_MAC_UPPER_LAYER_SET) {
		cdc->mac_src_type = CDC_NCM_MAC_UNINIT;
	}

#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	cdc->tx_agg_xfer_busy = 0U;
#endif

	usbh_cdc_ncm_deinit_all_pipe();

	/* Fail a blocked sender: the closed pipe never completes its TX. Given unconditionally
	 * so a sender that has not published bulk_tx_block yet is covered too; it then sees the
	 * cleared state, and a give nobody consumes is dropped by the next blocking send. */
	cdc->bulk_tx_err = 1U;
	(void)usb_os_sema_give(cdc->bulk_tx_sema);

	if ((cdc->cb != NULL) && (cdc->cb->detached != NULL)) {
		cdc->cb->detached();
	}
}

/**
  * @brief  Usb Setup callback function.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ncm_setup(usb_host_t *host)
{
	int status;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	/* Run control-setting here (like ECM setup); it returns -RTK_ERR_BUSY until done
	 * and the core re-invokes setup() on non-OK. Doing it later from process()
	 * let intr_rx dispatch first and drop the one-shot notification. */
	status = usbh_cdc_ncm_ctrl_setting(host);
	if (status != RTK_SUCCESS) {
		return status;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "NCM alt setting finish %d\n", cdc->intr_rx.pipe.pipe_num);

#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	/* NTB parameters (alignment/divisor) are now known; reset TX buffers */
	usb_os_lock(cdc->tx_agg_lock);
	cdc->tx_agg_xfer_busy = 0U;
	cdc->tx_agg_fill_idx = 0U;
	usbh_cdc_ncm_agg_reset_buf(0U);
	usbh_cdc_ncm_agg_reset_buf(1U);
	usb_os_unlock(cdc->tx_agg_lock);
#endif

	if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
		cdc->cb->setup();
	}

	cdc->state = CDC_NCM_STATE_TRANSFER;

	/* Kick the first INTR IN to receive the network-connection notification. */
	usbh_notify(host, cdc->intr_rx.pipe.pipe_num, &usbh_cdc_ncm_driver);

	return RTK_SUCCESS;
}

/**
  * @brief  Start the recovery of a halted endpoint
  * @note   A halted endpoint answers every transaction with STALL until the host clears it
  *         (Ref USB 2.0 8.4.5 / 9.4.5), so re-submitting only repeats the STALL. Entering
  *         CDC_NCM_STATE_ERROR stops the SOF from re-arming any pipe until it is cleared.
  * @param  host: Host handle
  * @param  pipe: Halted pipe
  * @retval None
  */
static void usbh_cdc_ncm_halt(usb_host_t *host, const usbh_pipe_t *pipe)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	/* One halt at a time: another stalled pipe stays idle, is re-armed after this clear and
	 * then reports its own STALL */
	if (cdc->state == CDC_NCM_STATE_ERROR) {
		return;
	}

	cdc->halt_ep_addr = pipe->ep_addr;
	cdc->state = CDC_NCM_STATE_ERROR;
	usbh_notify(host, 0U, &usbh_cdc_ncm_driver);
}

/**
  * @brief  Clear the halted endpoint, re-enumerate the device if that keeps failing
  * @note   CLEAR_FEATURE(ENDPOINT_HALT) also resets the device data toggle to DATA0 (Ref USB
  *         2.0 9.4.5), matching the host toggle the STALL interrupt already reset. Once back in
  *         CDC_NCM_STATE_TRANSFER the SOF re-arms the pipes as usual.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_cdc_ncm_process_halt(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_setup_req_t setup;
	int ret;

	/* Re-enumeration already requested: wait for the detach it brings */
	if (cdc->halt_retry_cnt >= USBH_CDC_NCM_HALT_RETRY_MAX_CNT) {
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
	if (ret == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%d halt cleared\n", cdc->halt_ep_addr);
		cdc->halt_retry_cnt = 0U;
		cdc->state = CDC_NCM_STATE_TRANSFER;
		/* This state swallowed any pipe message that arrived meanwhile: re-drive every pipe so a
		 * completion latched in that window is still consumed */
		usbh_notify(host, cdc->intr_rx.pipe.pipe_num, &usbh_cdc_ncm_driver);
		usbh_notify(host, cdc->bulk_rx.pipe.pipe_num, &usbh_cdc_ncm_driver);
		usbh_notify(host, cdc->bulk_tx.pipe.pipe_num, &usbh_cdc_ncm_driver);
	} else if (ret != -RTK_ERR_BUSY) {
		cdc->halt_retry_cnt++;
		if (cdc->halt_retry_cnt >= USBH_CDC_NCM_HALT_RETRY_MAX_CNT) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%d clear halt fail, re-enum\n", cdc->halt_ep_addr);
			(void)usbh_reenumerate();
		} else {
			usbh_notify(host, 0U, &usbh_cdc_ncm_driver);
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
static void usbh_cdc_ncm_process(usb_host_t *host, usbh_drv_msg_t *msg)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	u8 pipe_num;

	switch (cdc->state) {
	case CDC_NCM_STATE_TRANSFER:
		if (msg) {
			pipe_num = msg->pipe_num;
			if (pipe_num == cdc->bulk_tx.pipe.pipe_num) {
				usbh_cdc_ncm_process_bulk_out(host);
			} else if (pipe_num == cdc->bulk_rx.pipe.pipe_num) {
				usbh_cdc_ncm_process_bulk_in(host);
			} else if (pipe_num == cdc->intr_rx.pipe.pipe_num) {
				usbh_cdc_ncm_process_intr_in(host);
			}
		}
		break;

	case CDC_NCM_STATE_ERROR:
		usbh_cdc_ncm_process_halt(host);
		break;

	case CDC_NCM_STATE_IDLE:
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
static void usbh_cdc_ncm_sof(usb_host_t *host)
{
	UNUSED(host);

	usbh_cdc_ncm_intr_receive();

	if (usbh_cdc_ncm_host.eth_hw_connect) {
		usbh_cdc_ncm_bulk_receive();
		usbh_cdc_ncm_bulk_tx();
	}
}

/**
  * @brief  Account one failed BULK transfer
  * @note   Host task only. A STALL has its own recovery, see usbh_cdc_ncm_halt(). Any other error is re-tried by the
  *         SOF poll at once, so a device that keeps failing would print one log line per (micro)frame: the log is
  *         thinned out, and a run of errors no retry recovers from re-enumerates the device, as an endpoint halt
  *         that can not be cleared does.
  * @param  host: Host handle
  * @param  pipe: Failed pipe
  * @param  err_cnt: Consecutive error count of that pipe
  * @param  urb_state: URB state of the failure
  * @retval None
  */
static void usbh_cdc_ncm_xfer_err(usb_host_t *host, const usbh_pipe_t *pipe, u16 *err_cnt, usbh_urb_state_t urb_state)
{
	if (urb_state == USBH_URB_STALL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x stall\n", pipe->ep_addr);
		usbh_cdc_ncm_halt(host, pipe);
		return;
	}

	if (*err_cnt < USBH_CDC_NCM_XFER_ERR_MAX_CNT) {
		if ((*err_cnt % USBH_CDC_NCM_XFER_ERR_LOG_STEP) == 0U) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x err %d cnt %d\n", pipe->ep_addr, urb_state, *err_cnt);
		}
		(*err_cnt)++;
		if (*err_cnt == USBH_CDC_NCM_XFER_ERR_MAX_CNT) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x err, re-enum\n", pipe->ep_addr);
			(void)usbh_reenumerate();
		}
	}
}

/**
  * @brief  Bulk Out handling function
  * @param  host: Host handle
  * @retval Status
  */
static void usbh_cdc_ncm_process_bulk_out(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_cdc_ncm_pipe_info_t *pipe_info = &(cdc->bulk_tx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	int status;

#if USBH_CDC_NCM_STATE_TRACE_ENABLE
	pipe_info->trigger_cnt++;
#endif

	status = usbh_transfer_process(host, ep);
	if ((status == RTK_SUCCESS) && (ep->xfer_state == USBH_EP_XFER_IDLE)) {
		cdc->tx_err_cnt = 0U;
		usbh_cdc_ncm_cb_bulk_send(status);
	} else if (ep->xfer_state == USBH_EP_XFER_START) {
		usbh_notify(host, ep->pipe_num, &usbh_cdc_ncm_driver);
	} else if (ep->xfer_state == USBH_EP_XFER_ERROR) {
		usbh_cdc_ncm_xfer_err(host, ep, &cdc->tx_err_cnt, usbh_get_urb_state(host, ep));
		/* Drop the NTB (Ethernet is best effort) and release the sender: re-sending it
		 * forever left a blocked sender waiting on a TX that never completes. */
		ep->xfer_state = USBH_EP_XFER_IDLE;
		usbh_cdc_ncm_cb_bulk_send(-RTK_ERR_IO);
	} else {
		/* Idle or still in flight */
	}
}

static int usbh_cdc_ncm_cb_bulk_receive(u8 *buf, u32 length)
{
	int ret;

	if (buf == NULL || length == 0) {
		return -RTK_ERR_BADARG;
	}

#if USBH_CDC_NCM_RX_SPEED_CHECK
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

	/* Parse NTB block and deliver every Ethernet datagram it contains */
	ret = usbh_cdc_ncm_parse_ntb16_block(buf, length);
	if (ret <= 0) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "NCM parse fail len=%d\n", length);
	}

	return RTK_SUCCESS;
}

static void usbh_cdc_ncm_process_bulk_in(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_cdc_ncm_pipe_info_t *pipe_info = &(cdc->bulk_rx);
	usbh_pipe_t *bulk_in = &(pipe_info->pipe);
	int status = usbh_transfer_process(host, bulk_in);
	usbh_urb_state_t urb_state;
	u32 len;

#if USBH_CDC_NCM_STATE_TRACE_ENABLE
	pipe_info->trigger_cnt++;
#endif

	if ((status == RTK_SUCCESS) && (bulk_in->xfer_state == USBH_EP_XFER_IDLE)) {
		cdc->rx_err_cnt = 0U;
		len = usbh_get_last_transfer_size(host, bulk_in);
		usbh_cdc_ncm_cb_bulk_receive(bulk_in->xfer_buf, len);

		bulk_in->xfer_state = USBH_EP_XFER_START;
		usbh_notify(host, bulk_in->pipe_num, &usbh_cdc_ncm_driver);
	} else if (bulk_in->xfer_state == USBH_EP_XFER_START) {
		/* Transfer not yet complete: do not deliver data (last_transfer_size
		   would be 0 or stale). Just keep the pipeline moving. */
		usbh_notify(host, bulk_in->pipe_num, &usbh_cdc_ncm_driver);
	} else if (bulk_in->xfer_state == USBH_EP_XFER_ERROR) {
		urb_state = usbh_get_urb_state(host, bulk_in);
		usbh_cdc_ncm_xfer_err(host, bulk_in, &cdc->rx_err_cnt, urb_state);
		if (urb_state == USBH_URB_STALL) {
			/* Re-armed by the SOF once the halt is cleared */
			bulk_in->xfer_state = USBH_EP_XFER_IDLE;
		} else {
			bulk_in->xfer_state = USBH_EP_XFER_START;
			usbh_notify(host, bulk_in->pipe_num, &usbh_cdc_ncm_driver);
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
static void usbh_cdc_ncm_process_intr_in(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_cdc_ncm_pipe_info_t *pipe_info = &(cdc->intr_rx);
	usbh_pipe_t *intr_in = &(pipe_info->pipe);
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, intr_in);

#if USBH_CDC_NCM_STATE_TRACE_ENABLE
	pipe_info->trigger_cnt++;
#endif

	/* A completed IN shows only as latched urb_state==DONE; consume and deliver it
	 * here rather than via usbh_transfer_process(), which raced the SOF re-arm and
	 * dropped the one-shot NETWORK_CONNECTION. xfer_state==BUSY is stable vs. the
	 * SOF ISR, so this read-check-consume is race-free. */
	if ((intr_in->xfer_state == USBH_EP_XFER_BUSY) && (urb_state == USBH_URB_DONE)) {
		u32 len = usbh_get_last_transfer_size(host, intr_in);
		intr_in->retry_cnt = 0;
		intr_in->xfer_state = USBH_EP_XFER_IDLE;
		usbh_cdc_ncm_cb_intr_receive(intr_in->xfer_buf, len);
		return;
	}

	/* The SOF never re-arms a stalled pipe (see usbh_cdc_ncm_intr_receive()), so a STALL stays
	 * latched with xfer_state==BUSY until handled here; the SOF re-arms it once cleared. */
	if ((intr_in->xfer_state == USBH_EP_XFER_BUSY) && (urb_state == USBH_URB_STALL)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX stall\n");
		intr_in->xfer_state = USBH_EP_XFER_IDLE;
		usbh_cdc_ncm_halt(host, intr_in);
		return;
	}

	/* Not a completion: (re)arm only on START. Never usbh_transfer_process()
	 * while BUSY - re-submitting an already-armed INTR channel corrupts it. */
	if (intr_in->xfer_state == USBH_EP_XFER_START) {
		int status = usbh_transfer_process(host, intr_in);
		if ((status != RTK_SUCCESS) || (intr_in->xfer_state == USBH_EP_XFER_ERROR)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX fail: %d\n", usbh_get_urb_state(host, intr_in));
		}
	}
}

/**
  * @brief  Check NCM BULK RX status and timeout, whether should trigger.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @return RTK_SUCCESS if a new transfer should be triggered, -RTK_ERR_BUSY otherwise.
  */
static int usbh_cdc_ncm_bulk_rx_time_check(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_cdc_ncm_pipe_info_t *pipe_info = &(cdc->bulk_rx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->host;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, ep);

	if ((USBH_URB_IDLE == urb_state) && (usbh_get_elapsed_ticks(host, ep->tick) >= (USBH_CDC_NCM_BULK_IN_IDLE_MAX_CNT))) {
		return RTK_SUCCESS;
	}

	if ((USBH_URB_BUSY == urb_state) && (usbh_get_elapsed_ticks(host, ep->tick) >= (USBH_CDC_NCM_BULK_IN_NAK_CHECK_MAX_CNT))) {
		return RTK_SUCCESS;
	}

	if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL) || (urb_state == USBH_URB_DONE)) {
		return RTK_SUCCESS;
	}

	return -RTK_ERR_BUSY;
}

/**
  * @brief Check NCM BULK TX status and timeout, whether should trigger.
  *        Shared by both the non-aggregation SOF poll
  *        and the aggregation SOF poll.
  * @retval RTK_SUCCESS if the OUT pipe should be re-driven, -RTK_ERR_BUSY otherwise.
  */
static int usbh_cdc_ncm_bulk_tx_time_check(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_cdc_ncm_pipe_info_t *pipe_info = &(cdc->bulk_tx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->host;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, ep);

	if ((urb_state == USBH_URB_IDLE) && (usbh_get_elapsed_ticks(host, ep->tick) >= (USBH_CDC_NCM_BULK_OUT_IDLE_MAX_CNT))) {
		return RTK_SUCCESS;
	}

	if ((urb_state == USBH_URB_BUSY) && (usbh_get_elapsed_ticks(host, ep->tick) >= (USBH_CDC_NCM_BULK_OUT_BUSY_MAX_CNT))) {
		return RTK_SUCCESS;
	}

	if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL) || (urb_state == USBH_URB_DONE)) {
		return RTK_SUCCESS;
	}

	return -RTK_ERR_BUSY;
}

/**
  * @brief Check NCM INTR RX status and timeout, whether should trigger
  * @retval Status
  */
static int usbh_cdc_ncm_intr_rx_time_check(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_cdc_ncm_pipe_info_t *pipe_info = &(cdc->intr_rx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->host;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, ep);

	if ((USBH_URB_IDLE == urb_state) && (usbh_get_elapsed_ticks(host, ep->tick) >= cdc->intr_check_tick)) {
		return RTK_SUCCESS;
	}

	if ((USBH_URB_BUSY == urb_state) && (usbh_get_elapsed_ticks(host, ep->tick) >= cdc->intr_check_tick)) {
		return RTK_SUCCESS;
	}

	if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
		return RTK_SUCCESS;
	}

	return -RTK_ERR_BUSY;
}

/**
  * @brief  Trigger transmission of BULK data.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @return 0 on success, non-zero on failure.
  */
static int usbh_cdc_ncm_bulk_tx(void)
{
	int ret = -RTK_ERR_BUSY;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_pipe_t  *ep = &(cdc->bulk_tx.pipe);
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ncm_check_enum_status() != RTK_SUCCESS) {
		return ret;
	}

#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	/* Aggregation mode: only re-drive if there is an in-flight transfer.
	 * Non-agg mode has no xfer_busy flag so the block always executes. */
	if (cdc->tx_agg_xfer_busy != 0U)
#endif
	{
		if (cdc->state == CDC_NCM_STATE_TRANSFER) {
			/* An idle pipe has nothing to drive: the send path notifies when it arms a TX, and
			   process_bulk_out() is a no-op on IDLE, so only re-drive an armed or in-flight TX */
			if ((ep->xfer_state != USBH_EP_XFER_IDLE) && (usbh_cdc_ncm_bulk_tx_time_check() == RTK_SUCCESS)) {
				usbh_notify(host, ep->pipe_num, &usbh_cdc_ncm_driver);
				ret = RTK_SUCCESS;
			}
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
static int usbh_cdc_ncm_bulk_receive(void)
{
	int ret = -RTK_ERR_BUSY;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_pipe_t  *ep = &(cdc->bulk_rx.pipe);
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ncm_check_enum_status() != RTK_SUCCESS) {
		return ret;
	}

	if (cdc->state == CDC_NCM_STATE_TRANSFER) {
		if ((ep->xfer_state == USBH_EP_XFER_IDLE) || (usbh_cdc_ncm_bulk_rx_time_check() == RTK_SUCCESS)) {
			if (ep->xfer_state == USBH_EP_XFER_IDLE) {
				ep->xfer_state = USBH_EP_XFER_START;
			}

			usbh_notify(host, ep->pipe_num, &usbh_cdc_ncm_driver);
			ret = RTK_SUCCESS;
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
static int usbh_cdc_ncm_intr_receive(void)
{
	int ret = -RTK_ERR_BUSY;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_pipe_t  *ep = &(cdc->intr_rx.pipe);
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ncm_check_enum_status() != RTK_SUCCESS) {
		return ret;
	}

	if (cdc->state == CDC_NCM_STATE_TRANSFER) {
		if (((ep->xfer_state == USBH_EP_XFER_IDLE) && (usbh_get_elapsed_ticks(host, ep->tick) >= cdc->intr_check_tick))
			|| (usbh_cdc_ncm_intr_rx_time_check() == RTK_SUCCESS)) {
			/* A STALL is only reported: re-submitting to a halted endpoint repeats the STALL */
			if ((ep->xfer_state != USBH_EP_XFER_BUSY) || (usbh_get_urb_state(host, ep) != USBH_URB_STALL)) {
				ep->xfer_state = USBH_EP_XFER_START;
			}
			usbh_notify(host, ep->pipe_num, &usbh_cdc_ncm_driver);
			ret = RTK_SUCCESS;
		}
	}

	return ret;
}

#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)

/* Datagram budget: local array size, further capped by wNtbOutMaxDatagrams (0 = no limit) */
static u16 usbh_cdc_ncm_agg_max_datagrams(void)
{
	u16 limit = usbh_cdc_ncm_host.ntb_params.wNtbOutMaxDatagrams;

	if ((limit == 0U) || (limit > USBH_CDC_NCM_TX_AGG_MAX_DATAGRAMS)) {
		limit = USBH_CDC_NCM_TX_AGG_MAX_DATAGRAMS;
	}
	return limit;
}

/* Reset a fill buffer: first datagram starts right after the NTH16, aligned */
static void usbh_cdc_ncm_agg_reset_buf(u8 b)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	cdc->tx_agg_write_off[b] = (u16)usbh_cdc_ncm_tx_align_dg(USB_CDC_NCM_NTH16_LENGTH);
	cdc->tx_agg_pkt_cnt[b] = 0U;
}

/* Whether one more datagram of eth_len bytes (plus the closing NDP) still fits */
static int usbh_cdc_ncm_agg_has_room(u8 b, u16 eth_len)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	u16 ndp_align = usbh_cdc_ncm_tx_ndp_align();
	u32 dg_index, ndp_off, ndp_len, total;

	if (cdc->tx_agg_pkt_cnt[b] >= usbh_cdc_ncm_agg_max_datagrams()) {
		return 0;
	}

	dg_index = usbh_cdc_ncm_tx_align_dg(cdc->tx_agg_write_off[b]);
	ndp_off = (dg_index + eth_len + (ndp_align - 1U)) & ~((u32)ndp_align - 1U);
	/* NDP = NDP16 header + (current + this + terminator) * 4B entries */
	ndp_len = USB_CDC_NCM_NDP16_HEADER_LENGTH + ((u32)cdc->tx_agg_pkt_cnt[b] + 2U) * USB_CDC_NCM_NDP16_ENTRY_LENGTH;
	total = ndp_off + ndp_len;

	return (total <= usbh_cdc_ncm_tx_max_ntb(USBH_CDC_NCM_TX_AGG_BUF_SIZE)) ? 1 : 0;
}

/* Append one Ethernet frame to the fill buffer (room already verified) */
static void usbh_cdc_ncm_agg_append(u8 *eth_buf, u16 eth_len)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	u8 b = cdc->tx_agg_fill_idx;
	u16 dg_index = (u16)usbh_cdc_ncm_tx_align_dg(cdc->tx_agg_write_off[b]);

	usb_os_memcpy((void *)(cdc->tx_agg_buf[b] + dg_index), (const void *)eth_buf, eth_len);
	cdc->tx_agg_data_pos_idx[b][cdc->tx_agg_pkt_cnt[b]] = dg_index;
	cdc->tx_agg_pkt_len[b][cdc->tx_agg_pkt_cnt[b]] = eth_len;
	cdc->tx_agg_pkt_cnt[b]++;
	cdc->tx_agg_write_off[b] = dg_index + eth_len;
}

/* Build the NTH16 + NDP16 for buffer b, return the total NTB length (0 if empty) */
static u16 usbh_cdc_ncm_agg_finalize(u8 b)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usb_cdc_ncm_nth16_t *nth;
	usb_cdc_ncm_ndp16_t *ndp;
	usb_cdc_ncm_ndp16_entry_t *entries;
	u16 ndp_align = usbh_cdc_ncm_tx_ndp_align();
	u16 ndp_off;
	u16 i;
	u16 cnt = cdc->tx_agg_pkt_cnt[b];

	if (cnt == 0U) {
		return 0U;
	}

	ndp_off = (cdc->tx_agg_write_off[b] + (ndp_align - 1U)) & ~(ndp_align - 1U);

	ndp = (usb_cdc_ncm_ndp16_t *)(cdc->tx_agg_buf[b] + ndp_off);
	ndp->dwSignature = USB_CDC_NCM_NDP16_NOCRC_SIGNATURE;
	/* NDP16 header + (cnt datagram entries + 1 terminator) * 4B */
	ndp->wLength = USB_CDC_NCM_NDP16_HEADER_LENGTH + (u16)((cnt + 1U) * USB_CDC_NCM_NDP16_ENTRY_LENGTH);
	ndp->wNextFpIndex = 0U;

	/* Address the entry array through a pointer rather than ndp->aEntry[i]:
	 * aEntry is declared with 2 elements as a place-holder, while cnt may reach
	 * USBH_CDC_NCM_TX_AGG_MAX_DATAGRAMS, so indexing the declared array would be
	 * out of bounds. agg_has_room() already reserved cnt + 1 entries. */
	entries = (usb_cdc_ncm_ndp16_entry_t *)((u8 *)ndp + USB_CDC_NCM_NDP16_HEADER_LENGTH);
	for (i = 0; i < cnt; i++) {
		entries[i].wDatagramIndex = cdc->tx_agg_data_pos_idx[b][i];
		entries[i].wDatagramLength = cdc->tx_agg_pkt_len[b][i];
	}
	entries[cnt].wDatagramIndex = 0U;   /* terminator */
	entries[cnt].wDatagramLength = 0U;

	nth = (usb_cdc_ncm_nth16_t *)cdc->tx_agg_buf[b];
	nth->dwSignature = USB_CDC_NCM_NTH16_SIGNATURE;
	nth->wHeaderLength = USB_CDC_NCM_NTH16_LENGTH;
	nth->wSequence = cdc->ntb_sequence++;
	nth->wBlockLength = ndp_off + ndp->wLength;
	nth->wFpIndex = ndp_off;

	return nth->wBlockLength;
}

/* Finalize the current fill buffer and kick off its bulk OUT transfer.
 * Caller must hold cdc->tx_agg_lock and have verified inflight == 0. */
static void usbh_cdc_ncm_agg_launch(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_pipe_t *ep = &(cdc->bulk_tx.pipe);
	u8 b = cdc->tx_agg_fill_idx;
	u16 ntb_len;

	if (cdc->tx_agg_pkt_cnt[b] == 0U) {
		return;
	}

	ntb_len = usbh_cdc_ncm_agg_finalize(b);
	if (ntb_len == 0U) {
		usbh_cdc_ncm_agg_reset_buf(b);
		return;
	}

	ep->xfer_buf = cdc->tx_agg_buf[b];
	ep->xfer_len = ntb_len;
	ep->trx_zlp = ((ntb_len % ep->ep_mps) == 0U) ? 1U : 0U;
	ep->xfer_state = USBH_EP_XFER_START;

	cdc->tx_agg_xfer_busy = 1U;
	/* Switch to the other buffer for subsequent fills */
	cdc->tx_agg_fill_idx = (b + 1U) % USBH_CDC_NCM_TX_AGG_NUM_BUF;
	usbh_cdc_ncm_agg_reset_buf(cdc->tx_agg_fill_idx);

	/* No state write: the task may have just entered ERROR to clear a halt, it re-drives this pipe after */
	usbh_notify(cdc->host, ep->pipe_num, &usbh_cdc_ncm_driver);
}

/* Queue one Ethernet frame; coalesce while a transfer is in flight */
static int usbh_cdc_ncm_agg_send(u8 *buf, u32 len)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	u16 eth_len;
	u8 retry = 0;

	if (usbh_cdc_ncm_check_enum_status() != RTK_SUCCESS) {
		return -RTK_ERR_BUSY;
	}
	/* Checked at full width: narrowing first would turn an oversized frame into a short valid one */
	if ((len == 0U) || (len > USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE)) {
		return -RTK_ERR_BADARG;
	}
	eth_len = (u16)len;

	usb_os_lock(cdc->tx_agg_lock);

	while (!usbh_cdc_ncm_agg_has_room(cdc->tx_agg_fill_idx, eth_len)) {
		/* Fill buffer full: launch it if the wire is free, otherwise wait for
		   the in-flight transfer to complete so a buffer is freed. */
		if (cdc->tx_agg_pkt_cnt[cdc->tx_agg_fill_idx] == 0U) {
			/* Empty buffer with no room: the frame exceeds what the device accepts
			 * in one NTB (dwNtbOutMaxSize), so waiting or flushing cannot help. */
			usb_os_unlock(cdc->tx_agg_lock);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "TX agg too big(%d)\n", eth_len);
			return -RTK_ERR_BADARG;
		}
		if (!cdc->tx_agg_xfer_busy) {
			usbh_cdc_ncm_agg_launch();
		} else {
			usb_os_unlock(cdc->tx_agg_lock);
			if (++retry > 200U) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "TX agg drop(%d)\n", eth_len);
				return RTK_FAIL;
			}
			usb_os_delay_us(50);
			usb_os_lock(cdc->tx_agg_lock);
		}
	}

	usbh_cdc_ncm_agg_append(buf, eth_len);

	/* Opportunistic: when the wire is idle send immediately for low latency;
	   while a transfer is in flight keep coalescing into the fill buffer. */
	if (!cdc->tx_agg_xfer_busy) {
		usbh_cdc_ncm_agg_launch();
	}

	usb_os_unlock(cdc->tx_agg_lock);

	return RTK_SUCCESS;
}

#else  /* !CONFIG_USBH_CDC_NCM_TX_AGGREGATION */

/**
  * @brief  Start to send BULK data
  *         Builds an NTB frame and sends it
  * @param  buf: Data buffer (Ethernet frame)
  * @param  len: Data length
  * @retval Status
  */
static int usbh_cdc_ncm_bulk_send(u8 *buf, u32 len)
{
	int ret = -RTK_ERR_BUSY;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_cdc_ncm_pipe_info_t *pipe_info = &(cdc->bulk_tx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->host;
	u16 ntb_len = 0;

	if (usbh_cdc_ncm_check_enum_status() != RTK_SUCCESS) {
		return ret;
	}

	/* Checked at full width: narrowing first would turn an oversized frame into a short valid one. An
	 * unopened pipe has pipe_num 0, which is EP0, and ep_mps 0 for the ZLP math below. */
	if ((len == 0U) || (len > USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE) || (ep->pipe_num == 0U) || (ep->ep_mps == 0U)) {
		return -RTK_ERR_BADARG;
	}

	/* Only in TRANSFER, and never write the state: the task may have just entered ERROR to clear a halt */
	if (cdc->state == CDC_NCM_STATE_TRANSFER) {
		if (ep->xfer_state == USBH_EP_XFER_IDLE) {
			/* Build NTB16 frame from Ethernet frame */
			if (usbh_cdc_ncm_build_ntb16_frame(buf, (u16)len, cdc->tx_ntb_buf, &ntb_len) != RTK_SUCCESS) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "NTB build fail\n");
				return RTK_FAIL;
			}

			ep->xfer_buf = cdc->tx_ntb_buf;
			ep->xfer_len = ntb_len;

			if ((ntb_len % ep->ep_mps) == 0) {
				ep->trx_zlp = 1;
			} else {
				ep->trx_zlp = 0;
			}
			ep->xfer_state = USBH_EP_XFER_START;
			usbh_notify(host, ep->pipe_num, &usbh_cdc_ncm_driver);
			ret = RTK_SUCCESS;
		}
	}

	return ret;
}
#endif /* CONFIG_USBH_CDC_NCM_TX_AGGREGATION */

static int usbh_cdc_ncm_cb_bulk_send(int state)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	/* TX done or dropped: free the in-flight buffer and drain whatever was coalesced
	   while it was on the wire (handles the idle/tail case, no timer). */
	usb_os_lock(cdc->tx_agg_lock);
	cdc->tx_agg_xfer_busy = 0U;
	usbh_cdc_ncm_agg_launch();
	usb_os_unlock(cdc->tx_agg_lock);
#endif
	/* Only signal a blocked sender, on success and on failure alike (reported through
	 * bulk_tx_err); an unconditional give would leave a stale signal that a later blocking
	 * take consumes without waiting for the actual transfer to complete. */
	if (cdc->bulk_tx_block) {
		cdc->bulk_tx_err = (state == RTK_SUCCESS) ? 0U : 1U;
		(void)usb_os_sema_give(cdc->bulk_tx_sema);
	}

	return RTK_SUCCESS;
}

static int usbh_cdc_ncm_tx_status_check(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	u32 wait_ms = 0U;

	/* Wake a sender blocked on the sema and wait for every task inside the TX API to leave
	 * before the sema and buffers go away; bounded so a starved caller cannot hang the teardown */
	while ((cdc->bulk_tx_block != 0U) || (cdc->tx_users != 0U)) {
		if (wait_ms >= USBH_CDC_NCM_TX_EXIT_TIMEOUT_MS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Wait TX exit TO\n");
			break;
		}
		(void)usb_os_sema_give(cdc->bulk_tx_sema);
		usb_os_sleep_ms(1U);
		wait_ms++;
	}

	return RTK_SUCCESS;
}

static void usbh_cdc_ncm_deinit_all_pipe(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usb_host_t *host = cdc->host;

	if (host == NULL) {
		return;
	}

	/* Close before freeing: until the channel is halted the controller may still DMA into the buffer */
	if (cdc->intr_rx.pipe.pipe_num != 0U) {
		usbh_close_pipe(host, &(cdc->intr_rx.pipe));
		USBH_CDC_NCM_FREE_MEM(cdc->intr_rx.pipe.xfer_buf);
	}

	if (cdc->bulk_rx.pipe.pipe_num != 0U) {
		usbh_close_pipe(host, &(cdc->bulk_rx.pipe));
		USBH_CDC_NCM_FREE_MEM(cdc->bulk_rx.pipe.xfer_buf);
	}

	if (cdc->bulk_tx.pipe.pipe_num != 0U) {
		usbh_close_pipe(host, &(cdc->bulk_tx.pipe));
	}
}

#if USBH_CDC_NCM_STATE_TRACE_ENABLE
static void usbh_cdc_ncm_trace_thread(void *param)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	UNUSED(param);
	cdc->trace_task_init_flag = 1;
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
			RTK_LOGS(TAG, RTK_LOG_INFO, "IsrT %u-%u/period %u-%u\n",
					 (u32)cdc->host->isr_process_time_max, (u32)cdc->host->isr_process_time,
					 (u32)cdc->host->isr_enter_period_max, (u32)cdc->host->isr_enter_period);
#endif
		}
		usb_os_sleep_ms(1000);
	}
}

static void usbh_cdc_ncm_trace_task_init(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	int status;
	if (cdc->trace_task_init_flag) {
		return;
	}

	status = rtos_task_create(&cdc->trace_task, "usbh_cdc_ncm_trace_thread", usbh_cdc_ncm_trace_thread, NULL, 1024U * 2, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create ncm dump task fail\n");
	}
}

static void usbh_cdc_ncm_trace_task_deinit(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	if (cdc->trace_task != NULL) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Del trace task\n");
		rtos_task_delete(cdc->trace_task);
		cdc->trace_task = NULL;
	}

	cdc->trace_task_init_flag = 0;
}
#endif

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init CDC NCM class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_cdc_ncm_init(const usbh_cdc_ncm_state_cb_t *cb, const usbh_cdc_ncm_config_t *cfg)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	int ret = RTK_SUCCESS;
#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	u8 bi;
#endif

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return -RTK_ERR_BADARG;
	}

	if (usb_os_sema_create(&(cdc->bulk_tx_sema)) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create bulk_tx_sema fail\n");
		return -RTK_ERR_NOMEM;
	}

#if !defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	cdc->tx_ntb_buf = (u8 *)usb_os_malloc(USBH_CDC_NCM_TX_BUF_SIZE);
	if (cdc->tx_ntb_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc tx_ntb_buf fail\n");
		ret = -RTK_ERR_NOMEM;
		goto tx_ntb_buf_fail;
	}
#else
	for (bi = 0U; bi < USBH_CDC_NCM_TX_AGG_NUM_BUF; bi++) {
		cdc->tx_agg_buf[bi] = (u8 *)usb_os_malloc(USBH_CDC_NCM_TX_AGG_BUF_SIZE);
		if (cdc->tx_agg_buf[bi] == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc tx_agg_buf[%d] fail\n", bi);
			/* free any already-allocated buffers */
			while (bi > 0U) {
				bi--;
				USBH_CDC_NCM_FREE_MEM(cdc->tx_agg_buf[bi]);
			}
			ret = -RTK_ERR_NOMEM;
			goto tx_agg_buf_fail;
		}
	}

	usb_os_lock_create(&cdc->tx_agg_lock);
	cdc->tx_agg_xfer_busy = 0U;
	cdc->tx_agg_fill_idx = 0U;
	usbh_cdc_ncm_agg_reset_buf(0U);
	usbh_cdc_ncm_agg_reset_buf(1U);
#endif

	/* Every D2H request on it reserves whole EP0 packets, refer to usbh_ctrl_request() */
	cdc->dongle_ctrl_buf = (u8 *)usb_os_malloc(usb_get_dma_len(USBH_CDC_NCM_MAC_STRING_LEN + sizeof(usb_cdc_ncm_ntb_parameters_t), USB_MAX_EP0_SIZE));
	if (NULL == cdc->dongle_ctrl_buf) {
		ret = -RTK_ERR_NOMEM;
		goto ctrl_buf_fail;
	}

	cdc->mac_valid = 0;

	/* Reset NCM specific fields */
	usb_os_memset((void *)&cdc->ntb_params, 0, sizeof(usb_cdc_ncm_ntb_parameters_t));
	cdc->ntb_in_max_size = USB_CDC_NCM_DEFAULT_NTB_INPUT_SIZE;
	cdc->ntb_out_max_size = USB_CDC_NCM_DEFAULT_NTB_INPUT_SIZE;
	cdc->ntb_sequence = 0;

	/* cfg == NULL is valid: the MAC is then taken from the dongle during enumeration. */
	if ((cfg != NULL) && (cfg->mac_value != NULL)) {
		usb_os_memcpy((void *) & (cdc->mac[0]), (const void *)cfg->mac_value, CDC_NCM_MAC_STR_LEN);
		cdc->mac_src_type = CDC_NCM_MAC_UPPER_LAYER_SET;
	}

	/* Publish cb before cb->init() so the user hook doesn't see NULL. */
	cdc->cb = cb;

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "User init err %d", ret);
			goto user_init_fail;
		}
	}

	ret = usbh_register_class(&usbh_cdc_ncm_driver);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Register class fail %d\n", ret);
		goto user_deinit;
	}

#if USBH_CDC_NCM_STATE_TRACE_ENABLE
	usbh_cdc_ncm_trace_task_init();
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
	cdc->cb = NULL;
	USBH_CDC_NCM_FREE_MEM(cdc->dongle_ctrl_buf);

ctrl_buf_fail:
#if !defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	USBH_CDC_NCM_FREE_MEM(cdc->tx_ntb_buf);
tx_ntb_buf_fail:
#else
	usb_os_lock_delete(cdc->tx_agg_lock);
	for (bi = 0U; bi < USBH_CDC_NCM_TX_AGG_NUM_BUF; bi++) {
		USBH_CDC_NCM_FREE_MEM(cdc->tx_agg_buf[bi]);
	}
tx_agg_buf_fail:
#endif
	usb_os_sema_delete(cdc->bulk_tx_sema);

	return ret;
}

/**
  * @brief  Deinitialize the USB NCM Class
  */
void usbh_cdc_ncm_deinit(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	u8 bi;
#endif

	/* Close the TX gate first: the TX API rejects new callers from here on, and the ones
	 * already inside are waited for in tx_status_check() before their resources go away */
	usb_os_enter_critical();
	cdc->tx_open = 0U;
	usb_os_exit_critical();

	cdc->eth_hw_connect = 0;

	/* Before anything is freed: on RTK_SUCCESS no attach/setup/process/detach of this
	 * class runs any more, an EP0 request it left in flight is halted and a sof() still
	 * running on another core is waited out. Anything else means nothing was released. */
	if (usbh_unregister_class(&usbh_cdc_ncm_driver) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Deinit refused\n");
		return;
	}

#if USBH_CDC_NCM_STATE_TRACE_ENABLE
	usbh_cdc_ncm_trace_task_deinit();
#endif

	usbh_cdc_ncm_tx_status_check();
	usb_os_sema_delete(cdc->bulk_tx_sema);

#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	cdc->tx_agg_xfer_busy = 0U;
	usb_os_lock_delete(cdc->tx_agg_lock);
	for (bi = 0U; bi < USBH_CDC_NCM_TX_AGG_NUM_BUF; bi++) {
		USBH_CDC_NCM_FREE_MEM(cdc->tx_agg_buf[bi]);
	}
#else
	USBH_CDC_NCM_FREE_MEM(cdc->tx_ntb_buf);
#endif

	usbh_cdc_ncm_deinit_all_pipe();

	USBH_CDC_NCM_FREE_MEM(cdc->dongle_ctrl_buf);

	if ((cdc->cb != NULL) && (cdc->cb->deinit != NULL)) {
		cdc->cb->deinit();
	}
}

/**
  * @brief  choose the right config descriptor while there has more then one config descriptor
  * @param  host: Host handle
  * @retval Status
  */
int usbh_cdc_ncm_choose_config(usb_host_t *host)
{
	usbh_dev_id_t dev_id = {0U};
	dev_id.bInterfaceClass = USB_CDC_COMM_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_NCM_SUBCLASS_CODE;
	dev_id.bInterfaceProtocol = 0x00;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	u8 cfg_idx = usbh_get_configuration(host, &dev_id);
	return usbh_set_configuration(host, cfg_idx);
}

/**
  * @brief  Validates the device configuration descriptor for CDC NCM support.
  * @note   Applies the rules of attach(), see usbh_cdc_ncm_find_function(): the NCM Communication Interface with
  *         its INTR IN endpoint and the NCM Data Interface (protocol 0x01) with its BULK pair must be present.
  * @param[in] host: USB host handle.
  * @return RTK_SUCCESS if both NCM interfaces are found, -RTK_ERR_BADARG otherwise.
  */
int usbh_cdc_ncm_check_config_desc(usb_host_t *host)
{
	usbh_itf_data_t *ctrl_itf;
	usbh_itf_desc_t *data_itf_desc;

	/* The same rules as attach(), so a configuration accepted here is one attach() takes */
	if (usbh_cdc_ncm_find_function(host, &ctrl_itf, &data_itf_desc) == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Check success idx\n");
		return RTK_SUCCESS;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Check fail idx\n");
	return -RTK_ERR_BADARG;
}

/**
  * @brief  Checks if the CDC NCM device is enumerated and configured on the USB bus.
  * @return 1: USB configured (ready for control/bulk transfer).
  *         0: Device not enumerated or not yet configured.
  */
u8 usbh_cdc_ncm_is_configured(void)
{
	return (usbh_cdc_ncm_check_enum_status() == RTK_SUCCESS);
}

/**
  * @brief  Send one Ethernet frame, called by usbh_cdc_ncm_transmit() inside the TX gate
  * @param  buf: Ethernet frame
  * @param  len: Frame length
  * @param  block: Non-zero to wait until the transfer completes
  * @retval Status
  */
static int usbh_cdc_ncm_tx_frame(u8 *buf, u32 len, u8 block)
{
#if !defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	u8 retry_cnt = 0;
#endif
	int ret;

#if USBH_CDC_NCM_TX_SPEED_CHECK
	static u64 usb_tx_start_time = 0, usb_tx_end_time, usb_tx_interval_time;
	static u64 usb_tx_total_len = 0;

	usb_tx_total_len += len;
	if (usb_tx_start_time == 0) {
		usb_tx_start_time = usb_os_get_timestamp_ms();
	}
#endif

#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	/* Aggregation copies the frame into an internal NTB buffer and returns
	   immediately, so the caller never needs to block (data is safe). */
	UNUSED(block);
	ret = usbh_cdc_ncm_agg_send(buf, len);
#if USBH_CDC_NCM_TX_SPEED_CHECK
	if (ret == RTK_SUCCESS) {
		usb_tx_end_time = usb_os_get_timestamp_ms();
		usb_tx_interval_time = (usb_tx_end_time - usb_tx_start_time) * RTOS_TICK_RATE_MS;
		if (usb_tx_interval_time >= 3000) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Heap %d TX %dB in %d ms, %d Kbps\n",
					 rtos_mem_get_free_heap_size(),
					 (u32)usb_tx_total_len, (u32)usb_tx_interval_time, (u32)((usb_tx_total_len * 8 * 1000) / (usb_tx_interval_time * 1024)));
			usb_tx_start_time = usb_tx_end_time;
			usb_tx_total_len = 0;
		}
	}
#endif
	return ret;
#else

	if (block) {
		cdc->bulk_tx_err = 0U;
		/* Drop a give left over from a TX nobody waited for (e.g. by detach), else the take
		 * below would return before this TX completes */
		(void)usb_os_sema_take(cdc->bulk_tx_sema, 0U);
		cdc->bulk_tx_block = 1;
	}

	while (1) {
		ret = usbh_cdc_ncm_bulk_send(buf, len);
		if (ret == RTK_SUCCESS) {
			break;
		}
		if (++retry_cnt > 100) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "TX drop(%d)\n", len);
			ret = RTK_FAIL;
			break;
		} else {
			usb_os_delay_us(1);
		}
	}

	if ((ret == RTK_SUCCESS) && block) {
		/* No timeout on purpose: every submitted TX ends in cb_bulk_send() or detach, and returning
		 * earlier would hand the buffer back to the caller while the controller still reads it.
		 * The caller shall be a task that can block, see usbh_cdc_ncm_transmit(): inside a critical section
		 * or the PMU suspend flow the take fails at once. */
		(void)usb_os_sema_take(cdc->bulk_tx_sema, USB_OS_SEMA_TIMEOUT);
		if (cdc->bulk_tx_err != 0U) {
			ret = -RTK_ERR_IO;
		}
#if USBH_CDC_NCM_TX_SPEED_CHECK
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
#endif /* CONFIG_USBH_CDC_NCM_TX_AGGREGATION */
}

/**
  * @brief  Transmits an Ethernet packet to the device via the Bulk OUT endpoint.
  * @note   Task context only, one sender at a time. Gated against deinit: a caller is either
  *         counted in tx_users before deinit closes the gate, or rejected after it.
  * @param  buf: Ethernet frame
  * @param  len: Frame length
  * @param  block: Non-zero to wait until the transfer completes
  * @retval RTK_SUCCESS on success, -RTK_ERR_IO while the class is not initialized or being
  *         de-initialized, -RTK_ERR_BADARG in interrupt context, others on transfer failure.
  */
int usbh_cdc_ncm_transmit(u8 *buf, u32 len, u8 block)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	int ret;

	/* A blocking send waits on a semaphore */
	if (usb_os_is_in_interrupt() != 0U) {
		return -RTK_ERR_BADARG;
	}

	/* Counted under the same lock deinit closes the gate with, so deinit either sees this
	 * caller or this caller sees the gate closed: never both missed */
	usb_os_enter_critical();
	if (cdc->tx_open == 0U) {
		usb_os_exit_critical();
		return -RTK_ERR_IO;
	}
	cdc->tx_users++;
	usb_os_exit_critical();

	ret = usbh_cdc_ncm_tx_frame(buf, len, block);

	usb_os_enter_critical();
	cdc->tx_users--;
	usb_os_exit_critical();

	return ret;
}

/**
  * @brief  Checks if the Ethernet link is up (USB enumerated + link active).
  * @return 1: Device enumerated and Ethernet link is up.
  *         0: Device not enumerated or Ethernet link is down.
  */
u8 usbh_cdc_ncm_get_link_status(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	if (usbh_cdc_ncm_check_enum_status() != RTK_SUCCESS) {
		return 0;
	}

	return cdc->eth_hw_connect;
}

/**
  * @brief  Gets the MAC address of the connected device.
  * @return Pointer to the 6-byte MAC address array on success, NULL on failure.
  */
const u8 *usbh_cdc_ncm_get_mac_str(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	/* No wait: the MAC is settled in attach()/setup(), before the device reports ready, so
	 * waiting here only stalled the caller's link poll while no device was attached.
	 * The host task writes mac before mac_valid and both before connect_state reaches SETUP, so a caller that
	 * first waits for usbh_cdc_ncm_is_configured(), as every in-tree one does, reads a complete MAC. */
	if (cdc->mac_valid == 0) {
		return NULL;
	}

	return cdc->mac;
}

/* Weak default for the lwIP USB Ethernet TX hook. The host CDC ECM class defines the same weak symbol, and the
 * linker keeps one of the two without a warning, so CONFIG_USBH_CDC_ECM_MENU and CONFIG_USBH_CDC_NCM_MENU shall not
 * be enabled together, or the application shall provide its own strong usb_ethernet_transmit(). */
__attribute__((weak))
int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbh_cdc_ncm_transmit(buf, len, block);
}
