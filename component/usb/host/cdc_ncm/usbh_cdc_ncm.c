/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include "usb_os.h"
#include "usbh_cdc_ncm.h"

/* Private variables ---------------------------------------------------------*/

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

/* tick count */
#define USBH_CDC_NCM_BULK_OUT_IDLE_MAX_CNT                 (8U * 50U)       /* 50 SOF ticks idle timeout */
#define USBH_CDC_NCM_BULK_OUT_BUSY_MAX_CNT                 (8U * 10U)       /* 10 SOF ticks busy timeout */

#define USBH_CDC_NCM_BULK_IN_IDLE_MAX_CNT                  (8U * 10U)       /* 10 SOF ticks idle timeout */
#define USBH_CDC_NCM_BULK_IN_NAK_CHECK_MAX_CNT             (8U * 5U)        /* 5 SOF ticks NAK check */

/* ncm ethernet connect status check */
#define USBH_CDC_NCM_ETH_STATUS_CHECK_TIME                  300U           /* 300 ms */

/* Max iterations in tx_status_check do-while loop (1 ms per iteration).
 * Prevents indefinite spin if bulk_tx_block is never cleared (e.g. on
 * unexpected device removal during a pending TX). ~100 ms total. */
#define USBH_CDC_NCM_TX_STATUS_RETRY_MAX                   100U

/* Realtek dongle capability flags for vendor-specific initialization */
#define CDC_NCM_CAP_MODIFY_MAC                  (BIT0)  /* MAC Settings, enables the function of overwriting */
#define CDC_NCM_CAP_RCR_CMD                     (BIT1)  /* RCR (Receive Configuration Register) via vendor request */
#define CDC_NCM_CAP_MODIFY_LED                  (BIT2)

/**
 * @brief NCM dongle MAC source type.
 */
typedef enum {
	CDC_NCM_MAC_UNINIT = 0U,        /**< MAC not yet initialized */
	CDC_NCM_MAC_DONGLE_SUPPLY,      /**< Extracted from dongle iMACAddress string */
	CDC_NCM_MAC_UPPER_LAYER_SET,    /**< Provided by app via priv->mac_value */
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
	CDC_NCM_STATE_PRE_SETTING,
	CDC_NCM_STATE_TRANSFER,
	CDC_NCM_STATE_ERROR,
} usbh_cdc_ncm_state_t;

/* Private types -------------------------------------------------------------*/
typedef struct {
	u16 pid;
	u16 feature;
} __PACKED usbh_cdc_ncm_priv_config_t;

/* Private macros ------------------------------------------------------------*/
#define USBH_CDC_NCM_FREE_MEM(x)        do { if ((x) != NULL) { usb_os_mfree(x); (x) = NULL; } } while (0U)

/* Private function prototypes -----------------------------------------------*/
static int usbh_cdc_ncm_attach(usb_host_t *host);
static int usbh_cdc_ncm_detach(usb_host_t *host);
static int usbh_cdc_ncm_process(usb_host_t *host, usbh_event_t *event);
static int usbh_cdc_ncm_setup(usb_host_t *host);
static int usbh_cdc_ncm_sof(usb_host_t *host);
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
static int usbh_cdc_ncm_parse_ctrl(usbh_itf_data_t *itf_data);
static int usbh_cdc_ncm_tx_status_check(void);
static void usbh_cdc_ncm_deinit_all_pipe(void);
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
		.bInterfaceClass = USB_CDC_NCM_COMM_INTERFACE_CLASS_CODE,
		.bInterfaceSubClass = USB_CDC_NCM_SUBCLASS_CODE,
		.bInterfaceProtocol = 0x00,
	},
	{
	},
};

/* USB Host CDC NCM class driver */
static usbh_class_driver_t usbh_cdc_ncm_driver = {
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

static int usbh_cdc_ncm_support_feature(u16 pid, u16 feature_mask)
{
	u8 i;

	for (i = 0; usbh_cdc_ncm_rtk_dongle[i].pid != 0; i++) {
		if (usbh_cdc_ncm_rtk_dongle[i].pid == pid) {
			return (usbh_cdc_ncm_rtk_dongle[i].feature & feature_mask) ? HAL_OK : HAL_ERR_PARA;
		}
	}
	return HAL_ERR_PARA;
}

/* Private functions ---------------------------------------------------------*/
/* USB basic functions ----------------------------------------------------*/

static u16 usbh_cdc_ncm_get_eth_status_check_time(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	if ((cdc->host != NULL) && (cdc->host->dev_speed == USB_SPEED_HIGH)) {
		return 8 * USBH_CDC_NCM_ETH_STATUS_CHECK_TIME;
	}

	return USBH_CDC_NCM_ETH_STATUS_CHECK_TIME;
}

/**
  * @brief Convert hex data to char format
  * @retval
  */
static u8 usbh_cdc_ncm_hex_to_char(u8 hex_num)
{
	if ((hex_num >= 0x30) && (hex_num <= 0x39)) {
		hex_num = 0 + hex_num - 0x30;
	} else if ((hex_num >= 0x41) && (hex_num <= 0x5A)) {
		hex_num = 0xA + (hex_num - 0x41);
	} else if ((hex_num >= 0x61) && (hex_num <= 0x7A)) {
		hex_num = 0xA + (hex_num - 0x61);
	} else {
		hex_num = 0x0;
	}

	return hex_num;
}

/**
  * @brief  Check whether the device has completed enumeration.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @return HAL_OK if enumeration is complete, HAL_BUSY otherwise.
  */
static int usbh_cdc_ncm_check_enum_status(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	if ((cdc->host != NULL) && (cdc->host->connect_state >= USBH_STATE_SETUP)) {
		return HAL_OK;
	}

	return HAL_BUSY;
}

/* Add parse to get the ethernet status */
static int usbh_cdc_ncm_cb_intr_receive(u8 *buf, u32 length)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	if ((buf != NULL) && (length >= 8U)) {
		/* A1 00 00 00 01 00 00 00 - Network Connection */
		if (length == 8 && buf[0] == 0xA1 && buf[1] == USB_CDC_NCM_NOTIFY_NETWORK_CONNECTION) {
			cdc->eth_hw_connect = buf[2];
			if (cdc->eth_hw_connect && (cdc->intr_check_tick != usbh_cdc_ncm_get_eth_status_check_time())) {
				cdc->intr_check_tick = usbh_cdc_ncm_get_eth_status_check_time();
			} else if ((cdc->eth_hw_connect == 0) && (cdc->intr_rx.pipe.ep_interval != 0)) {
				cdc->intr_check_tick = cdc->intr_rx.pipe.ep_interval;
			}
			cdc->intr_rx.pipe.max_timeout_tick = cdc->intr_check_tick;
		}
		/* A1 2A 00 00 01 00 00 08  00 00 00 00 00 00 00 00 - Connection Speed Change */
		else if (length == 16 && buf[0] == 0xA1 && buf[1] == USB_CDC_NCM_NOTIFY_CONNECTION_SPEED_CHANGE) {
			/* Speed change notification - could be parsed if needed */
		}
	}

	return HAL_OK;
}

/* USB CDC NCM Control Functions Start ----------------------------------------------------*/
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
	/* wIndex of a string-descriptor request is the Language ID (US English). */
	setup.req.wIndex = 0x0409U;
	setup.req.wLength = buflen;

	return usbh_ctrl_request(host, &setup, pbuf);
}

/**
  * @brief  Get mac string descriptor
  * @param  host: Host handle
  * @retval Status
  */
static u8 usbh_cdc_ncm_read_mac_str(usb_host_t *host)
{
	u8 i;
	u8 mac_is_valid = 0;
	u8 status = HAL_OK;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	if ((cdc->mac_src_type == CDC_NCM_MAC_UPPER_LAYER_SET)
		&& (cdc->vid == USBH_CDC_NCM_DEFAULT_VID)
		&& (usbh_cdc_ncm_support_feature(cdc->pid, CDC_NCM_CAP_MODIFY_MAC) == HAL_OK)) {
		status = HAL_OK;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Upper set mac[%02x %02x %02x %02x %02x %02x]\n",
				 cdc->mac[0], cdc->mac[1], cdc->mac[2],
				 cdc->mac[3], cdc->mac[4], cdc->mac[5]);
	} else {
		status = usbh_cdc_ncm_process_get_string(host, cdc->dongle_ctrl_buf, USBH_CDC_NCM_MAC_STRING_LEN, cdc->iMACAddressStringId);
		cdc->mac_src_type = CDC_NCM_MAC_DONGLE_SUPPLY;
	}

	if (status == HAL_OK) {
		if (cdc->mac_src_type == CDC_NCM_MAC_DONGLE_SUPPLY) {
			for (i = 0; i < 6; i++) {
				cdc->mac[i] = usbh_cdc_ncm_hex_to_char(cdc->dongle_ctrl_buf[2 + 4 * i]) * 16
							  + usbh_cdc_ncm_hex_to_char(cdc->dongle_ctrl_buf[2 + 4 * i + 2]);
				if (cdc->mac[i] != 0U) {
					mac_is_valid = 1;
				}
			}

			if (mac_is_valid == 0) {
				TRNG_get_random_bytes(cdc->mac, CDC_NCM_MAC_STR_LEN);
				RTK_LOGS(TAG, RTK_LOG_INFO, "Random mac[%02x %02x %02x %02x %02x %02x]\n",
						 cdc->mac[0], cdc->mac[1], cdc->mac[2],
						 cdc->mac[3], cdc->mac[4], cdc->mac[5]);
				cdc->mac_src_type = CDC_NCM_MAC_RANDOM_SET;
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Dongle mac[%02x %02x %02x %02x %02x %02x]\n",
						 cdc->mac[0], cdc->mac[1], cdc->mac[2],
						 cdc->mac[3], cdc->mac[4], cdc->mac[5]);
				cdc->mac_valid = 1;
			}
		}
	}

	return status;
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
	if (ret == HAL_OK) {
		usb_os_memcpy(cdc->mac_ctrl_lock, cdc->dongle_ctrl_buf, CDC_NCM_CTRL_REG_BUF_LEN);
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

	usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->mac, CDC_NCM_CTRL_REG_BUF_LEN);
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
	u8 state;

	if ((cdc->vid != USBH_CDC_NCM_DEFAULT_VID)
		|| (cdc->mac_src_type == CDC_NCM_MAC_DONGLE_SUPPLY)
		|| (usbh_cdc_ncm_support_feature(cdc->pid, CDC_NCM_CAP_MODIFY_MAC) != HAL_OK)) {
		/* Non-Realtek dongle or MAC from dongle string: skip vendor write */
		cdc->sub_status = CDC_NCM_STATE_AT_SETTING_MAX;
		return;
	}

	switch (cdc->sub_status) {
	case CDC_NCM_STATE_CTRL_MAC_GET_LOCK:
		state = usbh_cdc_ncm_process_mac_get_lock(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get MAC lock err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_NCM_STATE_CTRL_MAC_DISABLE_LOCK:
		state = usbh_cdc_ncm_process_mac_set_dis_lock(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Dis MAC lock err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_NCM_STATE_CTRL_MAC_SET_MAC1:
		state = usbh_cdc_ncm_process_mac_set_mac1(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set MAC1 err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_NCM_STATE_CTRL_MAC_SET_MAC2:
		state = usbh_cdc_ncm_process_mac_set_mac2(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set MAC2 err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_NCM_STATE_CTRL_MAC_ENABLE_LOCK:
		state = usbh_cdc_ncm_process_mac_en_lock(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
			cdc->mac_valid = 1;
			RTK_LOGS(TAG, RTK_LOG_INFO, "MAC set success\n");
		} else if (state != HAL_BUSY) {
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
	u8 ret_state = HAL_ERR_UNKNOWN;
	u8 state = HAL_OK;

	switch (cdc->sub_status) {
	case CDC_NCM_STATE_AT_SETTING_IDLE:
		break;

	case CDC_NCM_STATE_GET_MAC_STR:
		state = usbh_cdc_ncm_read_mac_str(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Get MAC fail error[%d]\n", state);
			usb_os_sleep_ms(10);
		}
		break;

	case CDC_NCM_STATE_CTRL_ALT_SETTING:
		state = usbh_cdc_ncm_process_set_alt(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "NCM alt setting err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_NCM_STATE_GET_NTB_PARAMETERS:
		state = usbh_cdc_ncm_process_get_ntb_parameters(host);
		if (state == HAL_OK) {
			usb_cdc_ncm_ntb_parameters_t *params = (usb_cdc_ncm_ntb_parameters_t *)cdc->dongle_ctrl_buf;
			/* Store NTB parameters */
			cdc->ntb_params = *params;
			cdc->ntb_in_max_size = params->dwNtbInMaxSize;
			cdc->ntb_out_max_size = params->dwNtbOutMaxSize;

			/* Clamp to reasonable limits */
			if (cdc->ntb_in_max_size > USBH_CDC_NCM_RX_NTB_BUF_SIZE) {
				cdc->ntb_in_max_size = USBH_CDC_NCM_RX_NTB_BUF_SIZE;
			}
			if (cdc->ntb_out_max_size > USBH_CDC_NCM_RX_NTB_BUF_SIZE) {
				cdc->ntb_out_max_size = USBH_CDC_NCM_RX_NTB_BUF_SIZE;
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "NCM NTB: InMax=%d OutMax=%d\n",
					 cdc->ntb_in_max_size, cdc->ntb_out_max_size);
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get NTB params err\n");
			/* Use defaults */
			cdc->ntb_in_max_size = USB_CDC_NCM_DEFAULT_NTB_INPUT_SIZE;
			cdc->ntb_out_max_size = USB_CDC_NCM_DEFAULT_NTB_INPUT_SIZE;
			cdc->sub_status++;
		}
		break;

	case CDC_NCM_STATE_SET_NTB_INPUT_SIZE:
		state = usbh_cdc_ncm_process_set_ntb_input_size(host);
		if (state == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Set NTB Input Size: %d\n", cdc->ntb_in_max_size);
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set NTB input size err\n");
			cdc->sub_status++;
		}
		break;
	case CDC_NCM_STATE_CTRL_RCR_GET:
		if (usbh_cdc_ncm_support_feature(cdc->pid, CDC_NCM_CAP_RCR_CMD) != HAL_OK) {
			cdc->sub_status = CDC_NCM_STATE_CTRL_MAC_GET_LOCK;
			break;
		}
		usb_os_memset(cdc->rcr, 0, 4);
		state = usbh_cdc_ncm_vendor_reg_read(host, 0xC010, 0x0100);
		if (state == HAL_OK) {
			usb_os_memcpy(cdc->rcr, cdc->dongle_ctrl_buf, CDC_NCM_CTRL_REG_BUF_LEN);
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get RCR err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_NCM_STATE_CTRL_RCR_SET:
		if (usbh_cdc_ncm_support_feature(cdc->pid, CDC_NCM_CAP_RCR_CMD) != HAL_OK) {
			cdc->sub_status = CDC_NCM_STATE_CTRL_MAC_GET_LOCK;
			break;
		}
		/* set bit 0~3 set 1 will enable PING & UDP transfer */
		cdc->rcr[0] = cdc->rcr[0] | 0x0F;
		usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->rcr, CDC_NCM_CTRL_REG_BUF_LEN);
		state = usbh_cdc_ncm_vendor_reg_write(host, 0xC010, 0x010F);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
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
			ret_state = HAL_OK;
		}
		break;


	default:
		ret_state = HAL_OK;
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
  * @brief  Parse NCM control interface
  * @param  itf_data: interface descriptor buffer
  * @retval Status
  */
static int usbh_cdc_ncm_parse_ctrl(usbh_itf_data_t *itf_data)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_cdc_ncm_pipe_info_t *ctrl_ep = &(cdc->intr_rx);
	usb_cdc_ncm_function_desc_t *ncm_desc;
	usbh_itf_desc_t *itf_desc;
	u8 *desc;
	u16 total;
	u8 len;

	if (itf_data == NULL || itf_data->raw_data == NULL || itf_data->itf_desc_array == NULL) {
		return HAL_ERR_PARA;
	}

	itf_desc = &itf_data->itf_desc_array[0];
	if (itf_desc->bNumEndpoints != 1) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Ctrl ep is %d, return\n", itf_desc->bNumEndpoints);
		return HAL_ERR_PARA;
	}

	/* Save the Communication (control) interface number for class-specific requests */
	cdc->ctrl_itf_id = itf_desc->bInterfaceNumber;

	/* Get INTR endpoint from the parsed descriptor */
	usb_os_memcpy(&ctrl_ep->ep_desc, &itf_desc->ep_desc_array[0], sizeof(usbh_ep_desc_t));
	ctrl_ep->valid = 1;

	/* Scan raw_data for the NCM functional descriptor */
	desc = itf_data->raw_data;
	total = 0;
	while (total < itf_data->raw_data_len) {
		len = ((usbh_desc_header_t *)desc)->bLength;
		if (len == 0) {
			break;
		}
		if (((usbh_desc_header_t *)desc)->bDescriptorType == USB_CDC_NCM_CS_INTERFACE) {
			/* First look for Ethernet Networking Functional Descriptor for MAC address */
			if (len >= 13 && desc[2] == USB_CDC_NCM_FUNC_DESC_ETHERNET_NETWORKING) {
				usb_cdc_ncm_ethernet_function_desc_t *eth_desc = (usb_cdc_ncm_ethernet_function_desc_t *)desc;
				cdc->iMACAddressStringId = eth_desc->iMACAddress;
				RTK_LOGS(TAG, RTK_LOG_INFO, "NCM Mac string id(%d)\n", cdc->iMACAddressStringId);
				cdc->sub_status = CDC_NCM_STATE_GET_MAC_STR;
			}
			/* Look for NCM Functional Descriptor */
			else if (len >= 6 && desc[2] == USB_CDC_NCM_FUNC_DESC) {
				ncm_desc = (usb_cdc_ncm_function_desc_t *)desc;
				RTK_LOGS(TAG, RTK_LOG_INFO, "NCM version 0x%04x caps 0x%02x\n",
						 ncm_desc->bcdNcmVersion, ncm_desc->bmNetworkCapabilities);
				/* Could parse capabilities here if needed */
			}
		}
		desc += len;
		total += len;
	}

	return HAL_OK;
}


static int usbh_cdc_ncm_parse_interface_desc(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_dev_id_t dev_id = {0U};
	usbh_dev_desc_t *pdesc;
	usbh_itf_data_t *itf_data;
	usbh_itf_desc_t *data_itf_desc;
	usbh_ep_desc_t *ep;
	u8 alt_idx;
	u8 i;
	u8 ret = HAL_ERR_UNKNOWN;

	pdesc = cdc->host->dev_desc;
	cdc->vid = pdesc->idVendor;
	cdc->pid = pdesc->idProduct;

	/* Get NCM control interface */
	dev_id.bInterfaceClass = USB_CDC_NCM_COMM_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_NCM_SUBCLASS_CODE;
	dev_id.bInterfaceProtocol = 0x00;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	if (itf_data != NULL) {
		ret = usbh_cdc_ncm_parse_ctrl(itf_data);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Ctrl parse fail\n");
			return ret;
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get if fail\n");
		return HAL_ERR_PARA;
	}

	/* Get NCM data interface */
	dev_id.bInterfaceClass = USB_CDC_NCM_DATA_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_NCM_SUBCLASS_RESERVED;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	data_itf_desc = NULL;
	while ((itf_data != NULL) && (data_itf_desc == NULL)) {
		for (alt_idx = 0; alt_idx < itf_data->alt_setting_cnt; alt_idx++) {
			if (itf_data->itf_desc_array[alt_idx].bNumEndpoints > 0) {
				data_itf_desc = &itf_data->itf_desc_array[alt_idx];
				break;
			}
		}
		if (data_itf_desc == NULL) {
			itf_data = itf_data->next;
		}
	}

	if (data_itf_desc != NULL) {
		cdc->data_itf_id = data_itf_desc->bInterfaceNumber;
		cdc->data_alt_set = data_itf_desc->bAlternateSetting;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Get NCM data if(%d)alt(%d)\n", cdc->data_itf_id, cdc->data_alt_set);
		for (i = 0; i < data_itf_desc->bNumEndpoints; i++) {
			ep = &data_itf_desc->ep_desc_array[i];
			if (USB_EP_IS_IN(ep->bEndpointAddress)) {
				usb_os_memcpy(&cdc->bulk_rx.ep_desc, ep, sizeof(usbh_ep_desc_t));
				cdc->bulk_rx.valid = 1;
			} else {
				usb_os_memcpy(&cdc->bulk_tx.ep_desc, ep, sizeof(usbh_ep_desc_t));
				cdc->bulk_tx.valid = 1;
			}
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Data itf parse fail\n");
		return HAL_ERR_PARA;
	}

	return HAL_OK;
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
	u16 ndp_offset;
	u16 datagram_offset;

	if ((eth_buf == NULL) || (eth_len == 0U) || (ntb_buf == NULL) || (ntb_len == NULL)) {
		return HAL_ERR_PARA;
	}

	/* Reject oversized frames before any memcpy into the static tx buffer. */
	if (eth_len > USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "TX frame too large: %d\n", eth_len);
		return HAL_ERR_PARA;
	}

	/* NTH16 at start of NTB */
	nth = (usb_cdc_ncm_nth16_t *)ntb_buf;
	nth->dwSignature = USB_CDC_NCM_NTH16_SIGNATURE;
	nth->wHeaderLength = USB_CDC_NCM_NTH16_LENGTH;
	nth->wSequence = cdc->ntb_sequence;
	cdc->ntb_sequence++;

	/* Calculate NDP offset (after NTH, aligned) */
	ndp_offset = USB_CDC_NCM_NTH16_LENGTH;
	/* Align to 4 bytes */
	ndp_offset = (ndp_offset + 3) & ~3;

	/* NDP16 at ndp_offset */
	ndp = (usb_cdc_ncm_ndp16_t *)(ntb_buf + ndp_offset);
	ndp->dwSignature = USB_CDC_NCM_NDP16_NOCRC_SIGNATURE;
	ndp->wLength = USB_CDC_NCM_NDP16_MIN_LENGTH; /* 4 entries * 4 bytes = 16 */
	ndp->wNextFpIndex = 0; /* Last NDP */

	/* Calculate datagram offset (after NDP, aligned) */
	datagram_offset = ndp_offset + USB_CDC_NCM_NDP16_MIN_LENGTH;
	datagram_offset = (datagram_offset + 3) & ~3;

	/* Final guard: ensure the framed NTB fits the static tx buffer. */
	if (((u32)datagram_offset + eth_len) > USBH_CDC_NCM_TX_BUF_SIZE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "NTB exceeds tx buf: %d\n", (u32)datagram_offset + eth_len);
		return HAL_ERR_PARA;
	}

	/* Fill NDP entries */
	ndp->aEntry[0].wDatagramIndex = datagram_offset;
	ndp->aEntry[0].wDatagramLength = eth_len;
	/* Terminator entry */
	ndp->aEntry[1].wDatagramIndex = 0;
	ndp->aEntry[1].wDatagramLength = 0;

	/* Copy Ethernet frame */
	memcpy(ntb_buf + datagram_offset, eth_buf, eth_len);

	/* Calculate total NTB length */
	*ntb_len = datagram_offset + eth_len;

	/* Update NTH block length */
	nth->wBlockLength = *ntb_len;

	nth->wFpIndex = ndp_offset;

	return HAL_OK;
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
	u16 ndp_offset;
	u16 entry_cnt;
	u16 i;
	u32 block_len;
	int delivered = 0;

	if (ntb_buf == NULL || ntb_len < USB_CDC_NCM_NTH16_LENGTH) {
		return HAL_ERR_PARA;
	}

	nth = (usb_cdc_ncm_nth16_t *)ntb_buf;

	/* Verify NTH16 signature */
	if (nth->dwSignature != USB_CDC_NCM_NTH16_SIGNATURE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "NTH sig mismatch 0x%08x\n", nth->dwSignature);
		return HAL_ERR_PARA;
	}

	/* Trust the actual transfer size; wBlockLength is only used to bound the walk */
	block_len = nth->wBlockLength;
	if ((block_len == 0) || (block_len > ntb_len)) {
		block_len = ntb_len;
	}

	/* Walk the NDP chain (wFpIndex -> wNextFpIndex, 0 terminates) */
	ndp_offset = nth->wFpIndex;
	while (ndp_offset != 0) {
		if (((u32)ndp_offset + USB_CDC_NCM_NDP16_MIN_LENGTH) > block_len) {
			break;
		}

		ndp = (usb_cdc_ncm_ndp16_t *)(ntb_buf + ndp_offset);

		/* Verify NDP16 signature (no-CRC variant) */
		if (ndp->dwSignature != USB_CDC_NCM_NDP16_NOCRC_SIGNATURE) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "NDP sig mismatch 0x%08x\n", ndp->dwSignature);
			break;
		}

		/* Datagram entries = (wLength - 8) / 4; last entry is a 0,0 terminator */
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
		entry_cnt = (ndp->wLength - 8U) / USB_CDC_NCM_NDP16_ENTRY_LENGTH;

		for (i = 0; i < entry_cnt; i++) {
			u16 dg_index = ndp->aEntry[i].wDatagramIndex;
			u16 dg_len = ndp->aEntry[i].wDatagramLength;

			/* Null entry terminates the datagram list */
			if ((dg_index == 0) || (dg_len == 0)) {
				break;
			}

			if (((u32)dg_index + dg_len) > ntb_len) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "NCM data exceeds NTB len\n");
				break;
			}

			if ((cdc->cb != NULL) && (cdc->cb->bulk_received != NULL)) {
				cdc->cb->bulk_received(ntb_buf + dg_index, dg_len);
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
	int status = HAL_ERR_UNKNOWN;
	usbh_cdc_ncm_pipe_info_t *pipe_info;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	cdc->host = host;

	status = usbh_cdc_ncm_parse_interface_desc(host);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse desc fail\n");
		return status;
	}

	/* Control IN (INTR) */
	pipe_info = &(cdc->intr_rx);
	if (pipe_info->valid) {
		usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc));
		pipe_info->pipe.xfer_buf = (u8 *)usb_os_malloc(pipe_info->pipe.ep_mps);
		pipe_info->pipe.xfer_len = pipe_info->pipe.ep_mps;
		pipe_info->pipe.xfer_state = USBH_EP_XFER_START;

		cdc->intr_check_tick = pipe_info->pipe.ep_interval;
		pipe_info->pipe.max_timeout_tick = cdc->intr_check_tick;
	}

	/* BULK OUT (TX) */
	pipe_info = &(cdc->bulk_tx);
	if (pipe_info->valid) {
		usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc));
		pipe_info->pipe.max_timeout_tick = USBH_CDC_NCM_BULK_OUT_BUSY_MAX_CNT;
	}

	/* BULK IN (RX) */
	pipe_info = &(cdc->bulk_rx);
	if (pipe_info->valid) {
		usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc));
		/* NCM use bulk, allocate buffer for NTB reception */
		pipe_info->pipe.xfer_buf = (u8 *)usb_os_malloc(USBH_CDC_NCM_RX_NTB_BUF_SIZE);
		pipe_info->pipe.xfer_len = USBH_CDC_NCM_RX_NTB_BUF_SIZE;
		pipe_info->pipe.xfer_state = USBH_EP_XFER_START;

		pipe_info->pipe.max_timeout_tick = USBH_CDC_NCM_BULK_IN_IDLE_MAX_CNT;
	}
	usbh_cdc_ncm_dump_ncm_cfgdesc();

	cdc->state = CDC_NCM_STATE_IDLE;

	if ((cdc->cb != NULL) && (cdc->cb->attach != NULL)) {
		cdc->cb->attach();
	}

	return HAL_OK;
}

/**
  * @brief  Usb Detach callback function.
  * @param  host: Host handle
  * @retval HAL_OK
  */
static int usbh_cdc_ncm_detach(usb_host_t *host)
{
	UNUSED(host);
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	cdc->state = CDC_NCM_STATE_IDLE;

	cdc->eth_hw_connect = 0;

#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	cdc->tx_agg_xfer_busy = 0U;
#endif

	usbh_cdc_ncm_deinit_all_pipe();

	if ((cdc->cb != NULL) && (cdc->cb->detach != NULL)) {
		cdc->cb->detach();
	}

	return HAL_OK;
}

/**
  * @brief  Usb Setup callback function.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ncm_setup(usb_host_t *host)
{
	int status = HAL_ERR_UNKNOWN;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	cdc->state = CDC_NCM_STATE_PRE_SETTING;

	if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
		cdc->cb->setup();
	}

	status = HAL_OK;

	usbh_notify_class_state_change(host, 0);

	return status;
}

/**
  * @brief  Usb State Machine handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ncm_process(usb_host_t *host, usbh_event_t *event)
{
	u8 req_status = HAL_OK;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	u8 pipe_num;

	switch (cdc->state) {
	case CDC_NCM_STATE_PRE_SETTING:
		req_status = usbh_cdc_ncm_ctrl_setting(host);
		if (req_status == HAL_OK) {
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
			cdc->state = CDC_NCM_STATE_TRANSFER;
			usbh_notify_class_state_change(host, cdc->intr_rx.pipe.pipe_num);
		} else {
			usbh_notify_class_state_change(host, 0);
		}
		break;

	case CDC_NCM_STATE_TRANSFER:
		if (event) {
			pipe_num = event->pipe_num;
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
		req_status = usbh_ctrl_clear_feature(host, 0x00U);
		if (req_status == HAL_OK) {
			cdc->state = CDC_NCM_STATE_IDLE;
		}
		break;

	case CDC_NCM_STATE_IDLE:
	default:
		/* main task in idle/default status, sleep to release CPU */
		usb_os_sleep_ms(1);
		break;
	}

	return req_status;
}

/**
  * @brief  SOF callback for class-specific timing process.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param[in] host: USB host handle.
  * @return 0 on success, non-zero on failure.
  */
static int usbh_cdc_ncm_sof(usb_host_t *host)
{
	UNUSED(host);

	usbh_cdc_ncm_intr_receive();

	if (usbh_cdc_ncm_host.eth_hw_connect) {
		usbh_cdc_ncm_bulk_receive();
		usbh_cdc_ncm_bulk_tx();
	}

	return HAL_OK;
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
	if ((status == HAL_OK) && (ep->xfer_state == USBH_EP_XFER_IDLE)) {
		usbh_cdc_ncm_cb_bulk_send(status);
	} else if (ep->xfer_state == USBH_EP_XFER_START) {
		usbh_notify_class_state_change(host, ep->pipe_num);
	} else if (ep->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail %d\n", usbh_get_urb_state(host, ep));
		ep->xfer_state = USBH_EP_XFER_START;
		usbh_notify_class_state_change(host, ep->pipe_num);
	}
}

static int usbh_cdc_ncm_cb_bulk_receive(u8 *buf, u32 length)
{
	int ret;

	if (buf == NULL || length == 0) {
		return HAL_ERR_PARA;
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

	return HAL_OK;
}

static void usbh_cdc_ncm_process_bulk_in(usb_host_t *host)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_cdc_ncm_pipe_info_t *pipe_info = &(cdc->bulk_rx);
	usbh_pipe_t *bulk_in = &(pipe_info->pipe);
	int status = usbh_transfer_process(host, bulk_in);
	u32 len;

#if USBH_CDC_NCM_STATE_TRACE_ENABLE
	pipe_info->trigger_cnt++;
#endif

	if ((status == HAL_OK) && (bulk_in->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, bulk_in);
		usbh_cdc_ncm_cb_bulk_receive(bulk_in->xfer_buf, len);

		bulk_in->xfer_state = USBH_EP_XFER_START;
		usbh_notify_class_state_change(host, bulk_in->pipe_num);
	} else if (bulk_in->xfer_state == USBH_EP_XFER_START) {
		/* Transfer not yet complete: do not deliver data (last_transfer_size
		   would be 0 or stale). Just keep the pipeline moving. */
		usbh_notify_class_state_change(host, bulk_in->pipe_num);
	} else if (bulk_in->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK RX fail: %d\n", usbh_get_urb_state(host, bulk_in));
		bulk_in->xfer_state = USBH_EP_XFER_START;
		usbh_notify_class_state_change(host, bulk_in->pipe_num);
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
	int status = usbh_transfer_process(host, intr_in);
	u32 len;

#if USBH_CDC_NCM_STATE_TRACE_ENABLE
	pipe_info->trigger_cnt++;
#endif

	if ((status == HAL_OK) && (intr_in->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, intr_in);
		usbh_cdc_ncm_cb_intr_receive(intr_in->xfer_buf, len);
	} else if ((intr_in->xfer_state == USBH_EP_XFER_START)) {
		usbh_notify_class_state_change(host, intr_in->pipe_num);
	} else if (intr_in->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX fail: %d\n", usbh_get_urb_state(host, intr_in));
	}
}

/**
  * @brief  Check NCM BULK RX status and timeout, whether should trigger.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @return HAL_OK if a new transfer should be triggered, HAL_BUSY otherwise.
  */
static int usbh_cdc_ncm_bulk_rx_time_check(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_cdc_ncm_pipe_info_t *pipe_info = &(cdc->bulk_rx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->host;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, ep);

	if ((USBH_URB_IDLE == urb_state) && (usbh_get_elapsed_ticks(host, ep->tick) >= (USBH_CDC_NCM_BULK_IN_IDLE_MAX_CNT))) {
		return HAL_OK;
	}

	if ((USBH_URB_BUSY == urb_state) && (usbh_get_elapsed_ticks(host, ep->tick) >= (USBH_CDC_NCM_BULK_IN_NAK_CHECK_MAX_CNT))) {
		return HAL_OK;
	}

	if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL) || (urb_state == USBH_URB_DONE)) {
		return HAL_OK;
	}

	return HAL_BUSY;
}

/**
  * @brief Check NCM BULK TX status and timeout, whether should trigger.
  *        Shared by both the non-aggregation SOF poll
  *        and the aggregation SOF poll.
  * @retval HAL_OK if the OUT pipe should be re-driven, HAL_BUSY otherwise.
  */
static int usbh_cdc_ncm_bulk_tx_time_check(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_cdc_ncm_pipe_info_t *pipe_info = &(cdc->bulk_tx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->host;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, ep);

	if ((urb_state == USBH_URB_IDLE) && (usbh_get_elapsed_ticks(host, ep->tick) >= (USBH_CDC_NCM_BULK_OUT_IDLE_MAX_CNT))) {
		return HAL_OK;
	}

	if ((urb_state == USBH_URB_BUSY) && (usbh_get_elapsed_ticks(host, ep->tick) >= (USBH_CDC_NCM_BULK_OUT_BUSY_MAX_CNT))) {
		return HAL_OK;
	}

	if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL) || (urb_state == USBH_URB_DONE)) {
		return HAL_OK;
	}

	return HAL_BUSY;
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
  * @brief  Send event to transmit BULK data.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @return 0 on success, non-zero on failure.
  */
static int usbh_cdc_ncm_bulk_tx(void)
{
	int ret = HAL_BUSY;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_pipe_t  *ep = &(cdc->bulk_tx.pipe);
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ncm_check_enum_status() != HAL_OK) {
		return ret;
	}

#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	/* Aggregation mode: only re-drive if there is an in-flight transfer.
	 * Non-agg mode has no xfer_busy flag so the block always executes. */
	if (cdc->tx_agg_xfer_busy)
#endif
	{
		if (cdc->state == CDC_NCM_STATE_TRANSFER) {
			if ((ep->xfer_state == USBH_EP_XFER_IDLE) || (usbh_cdc_ncm_bulk_tx_time_check() == HAL_OK)) {

				usbh_notify_class_state_change(host, ep->pipe_num);
				ret = HAL_OK;
			}
		}
	}

	return ret;
}

/**
  * @brief  Send event to receive BULK data.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @return 0 on success, non-zero on failure.
  */
static int usbh_cdc_ncm_bulk_receive(void)
{
	int ret = HAL_BUSY;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_pipe_t  *ep = &(cdc->bulk_rx.pipe);
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ncm_check_enum_status() != HAL_OK) {
		return ret;
	}

	if (cdc->state == CDC_NCM_STATE_TRANSFER) {
		if ((ep->xfer_state == USBH_EP_XFER_IDLE) || (usbh_cdc_ncm_bulk_rx_time_check() == HAL_OK)) {
			if (ep->xfer_state == USBH_EP_XFER_IDLE) {
				ep->xfer_state = USBH_EP_XFER_START;
			}

			usbh_notify_class_state_change(host, ep->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Send event to receive INTR data.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @return 0 on success, non-zero on failure.
  */
static int usbh_cdc_ncm_intr_receive(void)
{
	int ret = HAL_BUSY;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_pipe_t  *ep = &(cdc->intr_rx.pipe);
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ncm_check_enum_status() != HAL_OK) {
		return ret;
	}

	if (cdc->state == CDC_NCM_STATE_TRANSFER) {
		if (((ep->xfer_state == USBH_EP_XFER_IDLE) && (usbh_get_elapsed_ticks(host, ep->tick) >= cdc->intr_check_tick))
			|| (usbh_cdc_ncm_intr_rx_time_check() == HAL_OK)) {
			ep->xfer_state = USBH_EP_XFER_START;
			cdc->state = CDC_NCM_STATE_TRANSFER;
			usbh_notify_class_state_change(host, ep->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}

#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)

/* Datagram alignment divisor required by the device (host-to-device), default 4 */
static u16 usbh_cdc_ncm_agg_divisor(void)
{
	u16 d = usbh_cdc_ncm_host.ntb_params.wNdbInDivisor;
	return (d == 0U) ? USB_CDC_NCM_DATAGRAM_ALIGN : d;
}

/* NDP alignment required by the device (host-to-device), default 4 */
static u16 usbh_cdc_ncm_agg_ndp_align(void)
{
	u16 a = usbh_cdc_ncm_host.ntb_params.wNdbInAlignment;
	return (a == 0U) ? USB_CDC_NCM_DATAGRAM_ALIGN : a;
}

/* Reset a fill buffer: first datagram starts right after the NTH16, aligned */
static void usbh_cdc_ncm_agg_reset_buf(u8 b)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	u16 div = usbh_cdc_ncm_agg_divisor();
	cdc->tx_agg_write_off[b] = (USB_CDC_NCM_NTH16_LENGTH + (div - 1U)) & ~(div - 1U);
	cdc->tx_agg_pkt_cnt[b] = 0U;
}

/* Whether one more datagram of eth_len bytes (plus the closing NDP) still fits */
static int usbh_cdc_ncm_agg_has_room(u8 b, u16 eth_len)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	u16 div = usbh_cdc_ncm_agg_divisor();
	u16 ndp_align = usbh_cdc_ncm_agg_ndp_align();
	u32 dg_index, ndp_off, ndp_len, total;

	if (cdc->tx_agg_pkt_cnt[b] >= USBH_CDC_NCM_TX_AGG_MAX_DATAGRAMS) {
		return 0;
	}

	dg_index = (cdc->tx_agg_write_off[b] + (div - 1U)) & ~(div - 1U);
	ndp_off = (dg_index + eth_len + (ndp_align - 1U)) & ~(ndp_align - 1U);
	/* NDP = 8B header + (current + this + terminator) * 4B entries */
	ndp_len = 8U + ((u32)cdc->tx_agg_pkt_cnt[b] + 2U) * USB_CDC_NCM_NDP16_ENTRY_LENGTH;
	total = ndp_off + ndp_len;

	return (total <= USBH_CDC_NCM_TX_AGG_BUF_SIZE) ? 1 : 0;
}

/* Append one Ethernet frame to the fill buffer (room already verified) */
static void usbh_cdc_ncm_agg_append(u8 *eth_buf, u16 eth_len)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	u8 b = cdc->tx_agg_fill_idx;
	u16 div = usbh_cdc_ncm_agg_divisor();
	u16 dg_index = (cdc->tx_agg_write_off[b] + (div - 1U)) & ~(div - 1U);

	memcpy(cdc->tx_agg_buf[b] + dg_index, eth_buf, eth_len);
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
	u16 ndp_align = usbh_cdc_ncm_agg_ndp_align();
	u16 ndp_off;
	u16 i;
	u16 cnt = cdc->tx_agg_pkt_cnt[b];

	if (cnt == 0U) {
		return 0U;
	}

	ndp_off = (cdc->tx_agg_write_off[b] + (ndp_align - 1U)) & ~(ndp_align - 1U);

	ndp = (usb_cdc_ncm_ndp16_t *)(cdc->tx_agg_buf[b] + ndp_off);
	ndp->dwSignature = USB_CDC_NCM_NDP16_NOCRC_SIGNATURE;
	/* 8B header + (cnt datagram entries + 1 terminator) * 4B */
	ndp->wLength = 8U + (u16)((cnt + 1U) * USB_CDC_NCM_NDP16_ENTRY_LENGTH);
	ndp->wNextFpIndex = 0U;
	for (i = 0; i < cnt; i++) {
		ndp->aEntry[i].wDatagramIndex = cdc->tx_agg_data_pos_idx[b][i];
		ndp->aEntry[i].wDatagramLength = cdc->tx_agg_pkt_len[b][i];
	}
	ndp->aEntry[cnt].wDatagramIndex = 0U;   /* terminator */
	ndp->aEntry[cnt].wDatagramLength = 0U;

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

	cdc->state = CDC_NCM_STATE_TRANSFER;
	usbh_notify_class_state_change(cdc->host, ep->pipe_num);
}

/* Queue one Ethernet frame; coalesce while a transfer is in flight */
static int usbh_cdc_ncm_agg_send(u8 *buf, u32 len)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	u16 eth_len = (u16)len;
	u8 retry = 0;

	if (usbh_cdc_ncm_check_enum_status() != HAL_OK) {
		return HAL_BUSY;
	}
	if ((eth_len == 0U) || (eth_len > USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE)) {
		return HAL_ERR_PARA;
	}

	usb_os_lock(cdc->tx_agg_lock);

	while (!usbh_cdc_ncm_agg_has_room(cdc->tx_agg_fill_idx, eth_len)) {
		/* Fill buffer full: launch it if the wire is free, otherwise wait for
		   the in-flight transfer to complete so a buffer is freed. */
		if (!cdc->tx_agg_xfer_busy) {
			usbh_cdc_ncm_agg_launch();
		} else {
			usb_os_unlock(cdc->tx_agg_lock);
			if (++retry > 200U) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "TX agg drop(%d)\n", eth_len);
				return HAL_ERR_UNKNOWN;
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

	return HAL_OK;
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
	int ret = HAL_BUSY;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usbh_cdc_ncm_pipe_info_t *pipe_info = &(cdc->bulk_tx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->host;
	u16 ntb_len = 0;

	if (usbh_cdc_ncm_check_enum_status() != HAL_OK) {
		return ret;
	}

	if ((cdc->state == CDC_NCM_STATE_IDLE) || (cdc->state == CDC_NCM_STATE_TRANSFER)) {
		if (ep->xfer_state == USBH_EP_XFER_IDLE) {
			/* Build NTB16 frame from Ethernet frame */
			if (usbh_cdc_ncm_build_ntb16_frame(buf, (u16)len, cdc->tx_ntb_buf, &ntb_len) != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "NTB build fail\n");
				return HAL_ERR_UNKNOWN;
			}

			ep->xfer_buf = cdc->tx_ntb_buf;
			ep->xfer_len = ntb_len;

			if ((ntb_len % ep->ep_mps) == 0) {
				ep->trx_zlp = 1;
			} else {
				ep->trx_zlp = 0;
			}
			ep->xfer_state = USBH_EP_XFER_START;
			cdc->state = CDC_NCM_STATE_TRANSFER;
			usbh_notify_class_state_change(host, ep->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}
#endif /* CONFIG_USBH_CDC_NCM_TX_AGGREGATION */

static int usbh_cdc_ncm_cb_bulk_send(int state)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	if (state == HAL_OK) {
#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
		/* TX done: free the in-flight buffer and drain whatever was coalesced
		   while it was on the wire (handles the idle/tail case, no timer). */
		usb_os_lock(cdc->tx_agg_lock);
		cdc->tx_agg_xfer_busy = 0U;
		usbh_cdc_ncm_agg_launch();
		usb_os_unlock(cdc->tx_agg_lock);
#endif
		/* Only signal a blocked sender; an unconditional give would leave a stale
		 * signal that a later blocking take consumes without waiting for the
		 * actual transfer to complete. */
		if (cdc->bulk_tx_block) {
			usb_os_sema_give(cdc->bulk_tx_sema);
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail %d\n", state);
	}

	return HAL_OK;
}

static int usbh_cdc_ncm_tx_status_check(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	int retry = USBH_CDC_NCM_TX_STATUS_RETRY_MAX;

	if (cdc->bulk_tx_block) {
		do {
			usb_os_sema_give(cdc->bulk_tx_sema);
			usb_os_sleep_ms(1U);
		} while (cdc->bulk_tx_block && --retry > 0);
	}

	return HAL_OK;
}

static void usbh_cdc_ncm_deinit_all_pipe(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	usb_host_t *host = cdc->host;

	if (cdc->intr_rx.pipe.pipe_num != 0U) {
		USBH_CDC_NCM_FREE_MEM(cdc->intr_rx.pipe.xfer_buf);
		usbh_close_pipe(host, &(cdc->intr_rx.pipe));
	}

	if (cdc->bulk_rx.pipe.pipe_num != 0U) {
		USBH_CDC_NCM_FREE_MEM(cdc->bulk_rx.pipe.xfer_buf);
		usbh_close_pipe(host, &(cdc->bulk_rx.pipe));
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
			RTK_LOGS(TAG, RTK_LOG_INFO, "IsrT %lld-%lld/period %lld-%lld\n",
					 cdc->host->isr_process_time_max, cdc->host->isr_process_time,
					 cdc->host->isr_enter_period_max, cdc->host->isr_enter_period);
#endif
		}
		usb_os_sleep_ms(1000);
	}
}

void usbh_cdc_ncm_trace_task_init(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	int status;
	if (cdc->trace_task_init_flag) {
		return;
	}

	status = rtos_task_create(&cdc->trace_task, "ncm_trace", usbh_cdc_ncm_trace_thread, NULL, 1024U * 2, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create ncm dump task fail\n");
	}
}

void usbh_cdc_ncm_trace_task_deinit(void)
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
int usbh_cdc_ncm_init(const usbh_cdc_ncm_state_cb_t *cb, const usbh_cdc_ncm_priv_data_t *priv)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	int ret = HAL_OK;
#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	u8 bi;
#endif

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	if (usb_os_sema_create(&(cdc->bulk_tx_sema)) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create bulk_tx_sema fail\n");
		return HAL_ERR_MEM;
	}

#if !defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	RTK_LOGS(TAG, RTK_LOG_ERROR, "TX Buf %d\n", USBH_CDC_NCM_TX_BUF_SIZE);
	cdc->tx_ntb_buf = (u8 *)usb_os_malloc(USBH_CDC_NCM_TX_BUF_SIZE);
	if (cdc->tx_ntb_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc tx_ntb_buf fail\n");
		ret = HAL_ERR_MEM;
		goto tx_ntb_buf_fail;
	}
#else
	RTK_LOGS(TAG, RTK_LOG_ERROR, "AGG TX Buf %d\n", USBH_CDC_NCM_TX_AGG_BUF_SIZE);
	for (bi = 0U; bi < USBH_CDC_NCM_TX_AGG_NUM_BUF; bi++) {
		cdc->tx_agg_buf[bi] = (u8 *)usb_os_malloc(USBH_CDC_NCM_TX_AGG_BUF_SIZE);
		if (cdc->tx_agg_buf[bi] == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc tx_agg_buf[%d] fail\n", bi);
			/* free any already-allocated buffers */
			while (bi > 0U) {
				bi--;
				USBH_CDC_NCM_FREE_MEM(cdc->tx_agg_buf[bi]);
			}
			ret = HAL_ERR_MEM;
			goto tx_agg_buf_fail;
		}
	}

	usb_os_lock_create(&cdc->tx_agg_lock);
	cdc->tx_agg_xfer_busy = 0U;
	cdc->tx_agg_fill_idx = 0U;
	usbh_cdc_ncm_agg_reset_buf(0U);
	usbh_cdc_ncm_agg_reset_buf(1U);
#endif

	cdc->dongle_ctrl_buf = (u8 *)usb_os_malloc(USBH_CDC_NCM_MAC_STRING_LEN + sizeof(usb_cdc_ncm_ntb_parameters_t));
	if (NULL == cdc->dongle_ctrl_buf) {
		ret = HAL_ERR_MEM;
		goto ctrl_buf_fail;
	}

	cdc->mac_valid = 0;

	/* Reset NCM specific fields */
	memset(&cdc->ntb_params, 0, sizeof(usb_cdc_ncm_ntb_parameters_t));
	cdc->ntb_in_max_size = USB_CDC_NCM_DEFAULT_NTB_INPUT_SIZE;
	cdc->ntb_out_max_size = USB_CDC_NCM_DEFAULT_NTB_INPUT_SIZE;
	cdc->ntb_sequence = 0;

	if (priv == NULL) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Param error\n");
	} else {
		if (priv->mac_value) {
			memcpy((void *) & (cdc->mac[0]), (const void *)priv->mac_value, CDC_NCM_MAC_STR_LEN);
			cdc->mac_src_type = CDC_NCM_MAC_UPPER_LAYER_SET;
		}
	}

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "User init err %d", ret);
			goto user_init_fail;
		}
	}

	cdc->cb = cb;

	usbh_register_class(&usbh_cdc_ncm_driver);

#if USBH_CDC_NCM_STATE_TRACE_ENABLE
	usbh_cdc_ncm_trace_task_init();
#endif

	return ret;

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
  * @retval Status
  */
int usbh_cdc_ncm_deinit(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	u8 bi;
#endif

	cdc->eth_hw_connect = 0;

	usbh_unregister_class(&usbh_cdc_ncm_driver);

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

	return HAL_OK;
}

/**
  * @brief  choose the right config descriptor while there has more then one config descriptor
  * @param  host: Host handle
  * @retval Status
  */
int usbh_cdc_ncm_choose_config(usb_host_t *host)
{
	usbh_dev_id_t dev_id = {0U};
	dev_id.bInterfaceClass = USB_CDC_NCM_COMM_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_NCM_SUBCLASS_CODE;
	dev_id.bInterfaceProtocol = 0x00;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	u8 cfg_idx = usbh_get_configuration(host, &dev_id);
	return usbh_set_configuration(host, cfg_idx);
}

/**
  * @brief  Validates the device configuration descriptor for CDC NCM support.
  * @note   Searches the configuration descriptor for both the NCM Communication
  *         Interface and the NCM Data Interface.  Both must be present for the
  *         device to be treated as a valid CDC NCM device.
  * @param[in] host: USB host handle.
  * @return HAL_OK if both NCM interfaces are found, HAL_ERR_PARA otherwise.
  */
int usbh_cdc_ncm_check_config_desc(usb_host_t *host)
{
	usbh_itf_data_t *itf_data;
	u8 ncm_ctrl_valid = 0;
	u8 ncm_data_valid = 0;

	usbh_dev_id_t dev_id = {0U};
	dev_id.bInterfaceClass = USB_CDC_NCM_COMM_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_NCM_SUBCLASS_CODE;
	dev_id.bInterfaceProtocol = 0x00;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);

	if (itf_data != NULL) {
		ncm_ctrl_valid = 1;
	}

	dev_id.bInterfaceClass = USB_CDC_NCM_DATA_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_NCM_SUBCLASS_RESERVED;
	dev_id.bInterfaceProtocol = 0x00;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);

	if (itf_data != NULL) {
		ncm_data_valid = 1;
	}

	if (ncm_ctrl_valid && ncm_data_valid) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Check success idx\n");
		return HAL_OK;
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Check fail idx [%d-%d]\n", ncm_ctrl_valid, ncm_data_valid);
	}

	return HAL_ERR_PARA;
}

/**
  * @brief  Checks if the CDC NCM device is enumerated and configured on the USB bus.
  * @return 1: USB configured (ready for control/bulk transfer).
  *         0: Device not enumerated or not yet configured.
  */
u8 usbh_cdc_ncm_is_configured(void)
{
	return (usbh_cdc_ncm_check_enum_status() == HAL_OK);
}

/**
  * @brief  Transmits an Ethernet packet to the device via the Bulk OUT endpoint.
  * @param[in] buf: Pointer to the data buffer (Ethernet frame) to be transmitted.
  * @param[in] len: Length of the data in bytes.
  * @param[in] block: Blocking mode flag (e.g., 0 for non-blocking, else for blocking).
  * @return 0 (HAL_OK) on success, non-zero on failure.
  */
int usbh_cdc_ncm_transmit(u8 *buf, u32 len, u8 block)
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
	if (ret == HAL_OK) {
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
		cdc->bulk_tx_block = 1;
	}

	while (1) {
		ret = usbh_cdc_ncm_bulk_send(buf, len);
		if (ret == HAL_OK) {
			break;
		}
		if (++retry_cnt > 100) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "TX drop(%d)\n", len);
			ret = HAL_ERR_UNKNOWN;
			break;
		} else {
			usb_os_delay_us(1);
		}
	}

	if ((ret == HAL_OK) && block) {
		usb_os_sema_take(cdc->bulk_tx_sema, USB_OS_SEMA_TIMEOUT);
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
  * @brief  Checks if the Ethernet link is up (USB enumerated + link active).
  * @return 1: Device enumerated and Ethernet link is up.
  *         0: Device not enumerated or Ethernet link is down.
  */
u8 usbh_cdc_ncm_get_link_status(void)
{
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;

	if (usbh_cdc_ncm_check_enum_status() != HAL_OK) {
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
	u8 i = 0;
	usbh_cdc_ncm_host_t *cdc = &usbh_cdc_ncm_host;
	while (!cdc->mac_valid && i < 10) {
		usb_os_sleep_ms(1000);
		i++;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Wait to get MAC string!\n");
	}

	if (cdc->mac_valid == 0) {
		return NULL;
	}

	return cdc->mac;
}

__attribute__((weak))
int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbh_cdc_ncm_transmit(buf, len, block);
}
