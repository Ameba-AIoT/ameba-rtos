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
#define USBH_CDC_ECM_ENABLE_DUMP_DESCRIPYOT_PARSE    0

#define USBH_CDC_ECM_BULK_BUF_MAX_SIZE               (512*3)

#define USBH_CDC_ECM_RX_SPEED_CHECK                  0     /* CDC ECM rx speed test */
#define USBH_CDC_ECM_TX_SPEED_CHECK                  0     /* CDC ECM tx speed test */

#define USBH_CDC_ECM_ENABLE_PACKETFILTER             0

/* tick count */
/* in ms*/
#define USBH_CDC_ECM_BULK_OUT_IDLE_MAX_CNT           8U * 50 //send total 16k data
#define USBH_CDC_ECM_BULK_OUT_BUSY_MAX_CNT           8U * 10

#define USBH_CDC_ECM_BULK_IN_IDLE_MAX_CNT            8U * 10  ///idle
#define USBH_CDC_ECM_BULK_IN_NAK_CHECK_MAX_CNT       8U * 5   /// 4*5 busy nak drop

/* ecm ethernet connect status check */
#define USBH_CDC_ECM_ETH_STATUS_CHECK_TIME           300U  //300ms

#define USBH_CDC_ECM_MAC_STRING_LEN                  (32)

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
	CDC_ECM_STATE_PRE_SETTING,
	CDC_ECM_STATE_TRANSFER,
	CDC_ECM_STATE_ERROR,
} usbh_cdc_ecm_state_t;

/* Private types -------------------------------------------------------------*/
typedef struct {
	u16 pid;
	u16 feature;
} __PACKED usbh_cdc_ecm_priv_config_t;

/* Private macros ------------------------------------------------------------*/
#define  USBH_CDC_ECM_FREE_MEM(x)  if(x){ usb_os_mfree(x); x = NULL;}

/* Private function prototypes -----------------------------------------------*/
static int usbh_cdc_ecm_attach(usb_host_t *host);
static int usbh_cdc_ecm_detach(usb_host_t *host);
static int usbh_cdc_ecm_process(usb_host_t *host, usbh_event_t *event);
static int usbh_cdc_ecm_setup(usb_host_t *host);
static int usbh_cdc_ecm_sof(usb_host_t *host);
static void usbh_cdc_ecm_process_bulk_out(usb_host_t *host);
static void usbh_cdc_ecm_process_bulk_in(usb_host_t *host);
static void usbh_cdc_ecm_process_intr_in(usb_host_t *host);
static int usbh_cdc_ecm_bulk_receive(void);
static int usbh_cdc_ecm_bulk_tx(void);
static int usbh_cdc_ecm_cb_intr_receive(u8 *buf, u32 length);
static int usbh_cdc_ecm_cb_bulk_send(int state);
static int usbh_cdc_ecm_intr_receive(void);
static int usbh_cdc_ecm_parse_ctrl(usbh_itf_data_t *itf_data);
static int usbh_cdc_ecm_tx_status_check(void);
static int usbh_cdc_ecm_bulk_send(u8 *buf, u32 len);
static void usbh_cdc_ecm_deinit_all_pipe(void);
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
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO,
		.bInterfaceClass = USB_CDC_CLASS_CODE,
		.bInterfaceSubClass = USB_CDC_SUBCLASS_ECM,
		.bInterfaceProtocol = 0x00,
	},
	{
	},
};

/* USB Host CDC ECM class driver */
static usbh_class_driver_t usbh_cdc_ecm_driver = {
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

/* Private functions ---------------------------------------------------------*/
/* USB basic functions ----------------------------------------------------*/
static u8 usbh_cdc_ecm_support_feature(u16 pid, usbh_cdc_ecm_dongle_capability_t type)
{
	usbh_cdc_ecm_priv_config_t *ecm_dongle;
	u16 i;

	for (i = 0; usbh_cdc_ecm_rtk_dongle[i].pid; i++) {
		ecm_dongle = (usbh_cdc_ecm_priv_config_t *) & (usbh_cdc_ecm_rtk_dongle[i]);
		if (pid != ecm_dongle->pid) {
			continue;
		}

		if (ecm_dongle->feature & type) {
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
  * @brief Convert hex data to char format
  * @retval
  */
static u8 usbh_cdc_ecm_hex_to_char(u8 hex_num)
{
	if ((hex_num >= 0x30) && (hex_num <= 0x39)) {
		/* code */
		hex_num = 0 + hex_num - 0x30;
	} else if ((hex_num >= 0x41) && (hex_num <= 0x5A)) {
		/* code */
		hex_num = 0xA + (hex_num - 0x41);
	} else if ((hex_num >= 0x61) && (hex_num <= 0x7A)) {
		/* code */
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
		if (length == 8 && buf[0] == 0xA1 && buf[1] == USB_CDC_ECM_NOTIFY_NETWORK_CONNECTION) {
			cdc->eth_hw_connect = buf[2];
			if (cdc->eth_hw_connect && (cdc->intr_check_tick != usbh_cdc_ecm_get_eth_status_check_time())) {
				cdc->intr_check_tick = usbh_cdc_ecm_get_eth_status_check_time();
			} else if ((cdc->eth_hw_connect == 0) && (cdc->intr_rx.pipe.ep_interval != 0)) {
				cdc->intr_check_tick = cdc->intr_rx.pipe.ep_interval;
			}
			cdc->intr_rx.pipe.max_timeout_tick = cdc->intr_check_tick;
		}
		/*A1 2A 00 00 01 00 00 08 	00 00 00 00 00 00 00 00 */
		else if (length == 16 && buf[0] == 0xA1 && buf[1] == USB_CDC_ECM_NOTIFY_CONNECTION_SPEED_CHANGE) {
			//usbh_cdc_ecm_speed_change_t
		}
	}

	return HAL_OK;
}

/* USB CDC ECM Control Functions Start ----------------------------------------------------*/
/**
  * @brief  Get string descriptor by str id
  * @param  host: Host handle
  * @param  pbuf: buffer handle which will save the string value
  * @param  buflen: buffer length
  * @param  str_id: string index
  * @retval Status
  */
static int usbh_cdc_acm_process_get_string(usb_host_t *host, u8 *pbuf, u16 buflen, u8 str_id)
{
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD;
	setup.req.bRequest = USB_REQ_GET_DESCRIPTOR;
	setup.req.wValue = USB_DESC_STRING | str_id;
	setup.req.wIndex = 1U;
	setup.req.wLength = buflen;

	return usbh_ctrl_request(host, &setup, pbuf);
}

/**
  * @brief  Get mac string descriptor
  * @param  host: Host handle
  * @retval Status
  */
static u8 usbh_cdc_ecm_get_mac_str(usb_host_t *host)
{
	u8 i;
	u8 mac_is_valid = 0;
	u8 status = HAL_OK;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	u16 vid = cdc->vid;
	u16 pid = cdc->pid;

	if ((cdc->mac_src_type == CDC_ECM_MAC_UPPER_LAYER_SET) && (vid == USB_DEFAULT_VID) && (usbh_cdc_ecm_support_feature(pid, CDC_ECM_CAP_MODIFY_MAC) == HAL_OK)) {
		status = HAL_OK;
		RTK_LOGS(TAG, RTK_LOG_INFO,  "Upper set mac[%02x %02x %02x %02x %02x %02x]\n", cdc->mac[0], cdc->mac[1], cdc->mac[2], cdc->mac[3], cdc->mac[4],
				 cdc->mac[5]);
	} else {
		status = usbh_cdc_acm_process_get_string(host, cdc->dongle_ctrl_buf, USBH_CDC_ECM_MAC_STRING_LEN, cdc->iMACAddressStringId);
		cdc->mac_src_type = CDC_ECM_MAC_DONGLE_SUPPLY;
	}

	if (status == HAL_OK) {
		if (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY) {
			for (i = 0; i < 6; i++) {
				cdc->mac[i] = usbh_cdc_ecm_hex_to_char(cdc->dongle_ctrl_buf[2 + 4 * i]) * 16 + usbh_cdc_ecm_hex_to_char(cdc->dongle_ctrl_buf[2 + 4 * i + 2]);
				if (cdc->mac[i]) {
					mac_is_valid = 1;
				}
			}

			if (mac_is_valid == 0) {
				TRNG_get_random_bytes(cdc->mac, CDC_ECM_MAC_STR_LEN);
				RTK_LOGS(TAG, RTK_LOG_INFO,  "Random mac[%02x %02x %02x %02x %02x %02x]\n", cdc->mac[0], cdc->mac[1], cdc->mac[2], cdc->mac[3], cdc->mac[4],
						 cdc->mac[5]);
				cdc->mac_src_type = CDC_ECM_MAC_RANDOM_SET;
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO,  "Dongle mac[%02x %02x %02x %02x %02x %02x]\n", cdc->mac[0], cdc->mac[1], cdc->mac[2], cdc->mac[3], cdc->mac[4],
						 cdc->mac[5]);
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
static int usbh_cdc_ecm_process_set_alt(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_REQ_SET_INTERFACE;
	setup.req.wValue = cdc->data_itf_id;
	setup.req.wIndex = cdc->data_alt_set;
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
	setup.req.bRequest = USB_CDC_ECM_SET_ETHERNET_PACKET_FILTER;
	setup.req.wValue = cdc->packet_filter;
	setup.req.wIndex = 1U;
	setup.req.wLength = 0;

	return usbh_ctrl_request(host, &setup, NULL);
}

/**
  * @brief  Set Ethernet Muticast Filter
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_set_muiticast_filter(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_ECM_SET_ETHERNET_MULTICAST_FILTERS;
	setup.req.wValue = 1;
	setup.req.wIndex = 1U;
	setup.req.wLength = cdc->muticast_filter_len;

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
	setup.req.bRequest = USB_CDC_ECM_GET_ETHERNET_STATISTIC;
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

static void usbh_cdc_ecm_set_dongle_mac(u8 *mac)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	if (NULL == mac) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Param error,mac is NULL\n");
		return ;
	}

	memcpy((void *) & (cdc->mac[0]), (void *)mac, CDC_ECM_MAC_STR_LEN);
	cdc->mac_src_type = CDC_ECM_MAC_UPPER_LAYER_SET;
}

static void usbh_cdc_ecm_set_dongle_led_array(u16 *led, u8 len)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	if (led == NULL || len == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Param error,led is NULL or len %d\n", len);
		return ;
	}

	USBH_CDC_ECM_FREE_MEM(cdc->led_array);

	cdc->led_array = (u16 *)usb_os_malloc(len * sizeof(u16));
	memcpy((void *)cdc->led_array, (void *)led, len * sizeof(u16));

	cdc->led_cnt = len;
}

static void usbh_cdc_ecm_config_dongle_mac(usb_host_t *host, u16 vid, u16 pid)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	u8 state = HAL_OK;

	// RTK_LOGS(TAG, RTK_LOG_INFO, "mac issue status[%d]V(0x%04x)P(0x%04x) type %d\n",appx_cmd.sub_status,vid,pid,cdc->mac_src_type);

	if ((vid != USB_DEFAULT_VID) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY) || (usbh_cdc_ecm_support_feature(pid, CDC_ECM_CAP_MODIFY_MAC) != HAL_OK)) {
		return;
	}

	switch (cdc->sub_status) {
	case CDC_ECM_STATE_CTRL_MAC_GET_LOCK: //8152 mac
		usb_os_memset(cdc->mac_ctrl_lock, 0, 4);
		state = usbh_cdc_ecm_process_mac_get_lock(host);
		if (state == HAL_OK) {
			usb_os_memcpy(cdc->mac_ctrl_lock, cdc->dongle_ctrl_buf, CDC_ECM_CTRL_REG_BUF_LEN);
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get MAC lock err\n");
			usb_os_sleep_ms(100);
		}

		break;

	case CDC_ECM_STATE_CTRL_MAC_DISABLE_LOCK: //8152 mac
		cdc->mac_ctrl_lock[0] = 0xD0;
		usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->mac_ctrl_lock, CDC_ECM_CTRL_REG_BUF_LEN);
		state = usbh_cdc_ecm_process_mac_set_dis_lock(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Dis MAC lock err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_SET_MAC1: //8152 mac
		usb_os_memcpy(cdc->dongle_ctrl_buf, &(cdc->mac[0]), CDC_ECM_CTRL_REG_BUF_LEN);
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
		cdc->dongle_ctrl_buf[2] = cdc->dongle_ctrl_buf[3] = 0xFF;
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
		usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->mac_ctrl_lock, CDC_ECM_CTRL_REG_BUF_LEN);
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
	u8 ret_state = HAL_ERR_UNKNOWN;
	u8 state = HAL_OK;
	u16 vid = cdc->vid;
	u16 pid = cdc->pid;

#if USBH_CDC_ECM_ENABLE_PACKETFILTER
	u8 mac_str[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif

	switch (cdc->sub_status) {
	case CDC_ECM_STATE_AT_SETTING_IDLE:
		break;

	case CDC_ECM_STATE_GET_MAC_STR:
		state = usbh_cdc_ecm_get_mac_str(host);
		if (state == HAL_OK) {
			cdc->sub_status ++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Get MAC fail error[%d]\n", state);
			usb_os_sleep_ms(10);
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
		usb_os_memcpy(cdc->muticast_filter, mac_str, 6);
		cdc->muticast_filter_len = 6;
		state = usbh_cdc_ecm_process_set_muiticast_filter(host);
		if (state == HAL_OK) {
			usb_os_memcpy(cdc->muticast_filter, cdc->dongle_ctrl_buf, cdc->muticast_filter_len);
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
			usb_os_memcpy(&(cdc->eth_statistic_count), cdc->dongle_ctrl_buf, CDC_ECM_CTRL_REG_BUF_LEN);
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM get eth statistic err\n");
			usb_os_sleep_ms(100);
		}
		break;
#endif

	case CDC_ECM_STATE_CTRL_RCR_GET:
		if ((vid != USB_DEFAULT_VID) || (usbh_cdc_ecm_support_feature(pid, CDC_ECM_CAP_RCR_CMD) != HAL_OK)) {
			cdc->sub_status++;
		} else {
			usb_os_memset(cdc->rcr, 0, 4);
			state = usbh_cdc_ecm_process_get_rcr(host);
			if (state == HAL_OK) {
				usb_os_memcpy(cdc->rcr, cdc->dongle_ctrl_buf, CDC_ECM_CTRL_REG_BUF_LEN);
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get RCR err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_RCR_SET:
		if ((vid != USB_DEFAULT_VID) || (usbh_cdc_ecm_support_feature(pid, CDC_ECM_CAP_RCR_CMD) != HAL_OK)) {
			cdc->sub_status++;
		} else {
			//set bit 0~3 set 1 will enable PING & UDP transfer
			cdc->rcr[0] = cdc->rcr[0] | 0x0F;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->rcr, CDC_ECM_CTRL_REG_BUF_LEN);
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
		if ((vid != USB_DEFAULT_VID) || (usbh_cdc_ecm_support_feature(pid, CDC_ECM_CAP_FIFO_CTRL) != HAL_OK)) {
			cdc->sub_status++;
		} else {
			usb_os_memset(cdc->flow_ctrl, 0, 4);
			cdc->flow_ctrl[0] = 0x60;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->flow_ctrl, CDC_ECM_CTRL_REG_BUF_LEN);
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
		if ((vid != USB_DEFAULT_VID) || (usbh_cdc_ecm_support_feature(pid, CDC_ECM_CAP_FIFO_CTRL) != HAL_OK)) {
			cdc->sub_status++;
		} else {
			usb_os_memset(cdc->flow_ctrl, 0, 4);
			cdc->flow_ctrl[0] = 0xa0;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->flow_ctrl, CDC_ECM_CTRL_REG_BUF_LEN);
			state = usbh_cdc_ecm_process_set_flow_ctrl2(host);
			if (state == HAL_OK) {
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set flow ctrl1 err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_GET_LOCK:
	case CDC_ECM_STATE_CTRL_MAC_DISABLE_LOCK:
	case CDC_ECM_STATE_CTRL_MAC_SET_MAC1:
	case CDC_ECM_STATE_CTRL_MAC_SET_MAC2:
	case CDC_ECM_STATE_CTRL_MAC_ENABLE_LOCK:
		if ((vid != USB_DEFAULT_VID) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY) || (usbh_cdc_ecm_support_feature(pid, CDC_ECM_CAP_MODIFY_MAC) != HAL_OK)) {
			cdc->sub_status = CDC_ECM_STATE_CTRL_LED_COLOR_SET;
		} else {
			usbh_cdc_ecm_config_dongle_mac(host, vid, pid);
		}
		break;

	case CDC_ECM_STATE_CTRL_LED_COLOR_SET:
		if ((vid != USB_DEFAULT_VID) || (cdc->led_cnt == 0) || (cdc->led_array == NULL) || (usbh_cdc_ecm_support_feature(pid, CDC_ECM_CAP_MODIFY_LED) != HAL_OK)) {
			cdc->sub_status++;
		} else {
			usb_os_memset(cdc->dongle_ctrl_buf, 0xFF, 4);
			usb_os_memcpy(cdc->dongle_ctrl_buf, (u8 *) & (cdc->led_array[0]), 2);
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
#if USBH_CDC_ECM_ENABLE_DUMP_DESCRIPYOT_PARSE
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
	RTK_LOGS(TAG, RTK_LOG_INFO, "in:addr(0x%x)MPS(%d)pipe(%d)interval(%d)\n",
			 ep_desc->bEndpointAddress, ep_desc->wMaxPacketSize, pipe_info->pipe.pipe_num, pipe_info->pipe.ep_interval);

	RTK_LOGS(TAG, RTK_LOG_INFO, "---------------------Dump End-----------------------------\n");
#endif
}

/**
  * @brief  Parse audio streaming interface
  * @param  itf_data: interface descriptor buffer
  * @retval Status
  */
static int usbh_cdc_ecm_parse_ctrl(usbh_itf_data_t *itf_data)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_pipe_info_t *ctrl_ep = &(cdc->intr_rx);
	usb_cdc_ethernet_function_desc_t *eth_desc;
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

	/* Get INTR endpoint from the parsed descriptor */
	usb_os_memcpy(&ctrl_ep->ep_desc, &itf_desc->ep_desc_array[0], sizeof(usbh_ep_desc_t));
	ctrl_ep->valid = 1;

	/* Scan raw_data only for the ECM Ethernet Networking CS functional descriptor,
	 * which carries iMACAddress and is not parsed by the generic framework. */
	desc = itf_data->raw_data;
	total = 0;
	while (total < itf_data->raw_data_len) {
		len = ((usbh_desc_header_t *)desc)->bLength;
		if (len == 0) {
			break;
		}
		if (((usbh_desc_header_t *)desc)->bDescriptorType == USB_CDC_CS_INTERFACE) {
			eth_desc = (usb_cdc_ethernet_function_desc_t *)desc;
			if (eth_desc->bDescriptorSubtype == USB_CDC_FUNC_DESC_ETHERNET_NETWORKING) {
				cdc->iMACAddressStringId = eth_desc->iMACAddress;
				RTK_LOGS(TAG, RTK_LOG_INFO, "Mac string id(%d)\n", cdc->iMACAddressStringId);
				cdc->sub_status = CDC_ECM_STATE_GET_MAC_STR;
				break;
			}
		}
		desc += len;
		total += len;
	}

	return HAL_OK;
}


static int usbh_cdc_ecm_parse_interface_desc(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_dev_id_t dev_id = {0,};
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

	/* Get interface index as per supported class & protocol */
	dev_id.bInterfaceClass = USB_CDC_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_SUBCLASS_ECM;
	dev_id.bInterfaceProtocol = 0x00;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	if (itf_data) {
		ret = usbh_cdc_ecm_parse_ctrl(itf_data);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Ctrl parse fail\n");
			return ret;
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get if fail\n");
		return HAL_ERR_PARA;
	}

	dev_id.bInterfaceClass = USB_CDC_DATA_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_SUBCLASS_RESERVED;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	data_itf_desc = NULL;
	while (itf_data && data_itf_desc == NULL) {
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "Get ECM data if(%d)alt(%d)\n", cdc->data_itf_id, cdc->data_alt_set);
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

/**
  * @brief  Usb Attach callback function
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_attach(usb_host_t *host) //parse all ep info
{
	int status = HAL_ERR_UNKNOWN;
	usbh_cdc_ecm_pipe_info_t *pipe_info;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	cdc->host = host;

	status = usbh_cdc_ecm_parse_interface_desc(host);
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse desc fail\n");
		return status;
	}

	//control in
	pipe_info = &(cdc->intr_rx);
	if (pipe_info->valid) {
		usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc));
		pipe_info->pipe.xfer_buf = (u8 *)usb_os_malloc(pipe_info->pipe.ep_mps);
		pipe_info->pipe.xfer_len = pipe_info->pipe.ep_mps;
		pipe_info->pipe.xfer_state = USBH_EP_XFER_START;

		cdc->intr_check_tick = pipe_info->pipe.ep_interval;
		pipe_info->pipe.max_timeout_tick = cdc->intr_check_tick;
	}

	//bulk out
	pipe_info = &(cdc->bulk_tx);
	if (pipe_info->valid) {
		usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc));
		pipe_info->pipe.max_timeout_tick = USBH_CDC_ECM_BULK_OUT_BUSY_MAX_CNT;
	}

	//bulk in
	pipe_info = &(cdc->bulk_rx);
	if (pipe_info->valid) {
		usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc));
		/* ecm use bulk, the max ethernet packet size is 1542, malloc (512*3) to rx a whole ethernet packet*/
		pipe_info->pipe.xfer_buf = (u8 *)usb_os_malloc(USBH_CDC_ECM_BULK_BUF_MAX_SIZE);
		pipe_info->pipe.xfer_len = USBH_CDC_ECM_BULK_BUF_MAX_SIZE;
		pipe_info->pipe.xfer_state = USBH_EP_XFER_START;

		pipe_info->pipe.max_timeout_tick = USBH_CDC_ECM_BULK_IN_IDLE_MAX_CNT;
	}
	usbh_cdc_ecm_dump_ecm_cfgdesc();

	cdc->state = CDC_ECM_STATE_IDLE;

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
static int usbh_cdc_ecm_detach(usb_host_t *host)
{
	UNUSED(host);
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	cdc->state = CDC_ECM_STATE_IDLE;

	usbh_cdc_ecm_deinit_all_pipe();

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
static int usbh_cdc_ecm_setup(usb_host_t *host)
{
	int status = HAL_ERR_UNKNOWN;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	cdc->state = CDC_ECM_STATE_PRE_SETTING;

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
static int usbh_cdc_ecm_process(usb_host_t *host, usbh_event_t *event)
{
	u8 req_status = HAL_OK;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	u8 pipe_num;

	switch (cdc->state) {
	case CDC_ECM_STATE_PRE_SETTING:
		req_status = usbh_cdc_ecm_ctrl_setting(host);
		if (req_status == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "ECM alt setting finish %d\n", cdc->intr_rx.pipe.pipe_num);
			cdc->state = CDC_ECM_STATE_TRANSFER;
			usbh_notify_class_state_change(host, cdc->intr_rx.pipe.pipe_num);
		} else {
			usbh_notify_class_state_change(host, 0);
		}
		break;

	case CDC_ECM_STATE_TRANSFER:
		if (event) {
			pipe_num = event->pipe_num;
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
		req_status = usbh_ctrl_clear_feature(host, 0x00U);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ECM_STATE_IDLE;
		}
		break;

	case CDC_ECM_STATE_IDLE:
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
static int usbh_cdc_ecm_sof(usb_host_t *host)
{
	UNUSED(host);

	usbh_cdc_ecm_intr_receive();

	if (usbh_cdc_ecm_host.eth_hw_connect) {
		usbh_cdc_ecm_bulk_receive();
		usbh_cdc_ecm_bulk_tx();
	}

	return HAL_OK;
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
	int status;

#if USBH_CDC_ECM_STATE_TRACE_ENABLE
	pipe_info->trigger_cnt++;
#endif

	status = usbh_transfer_process(host, ep);
	if ((status == HAL_OK) && (ep->xfer_state == USBH_EP_XFER_IDLE)) {
		usbh_cdc_ecm_cb_bulk_send(status);
	} else if (ep->xfer_state == USBH_EP_XFER_START) {
		usbh_notify_class_state_change(host, ep->pipe_num);
	} else if (ep->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail %d\n", usbh_get_urb_state(host, ep));
		ep->xfer_state = USBH_EP_XFER_START;
		usbh_notify_class_state_change(host, ep->pipe_num);
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "Heap %d RX %dB in %d ms, %d Kbps\n",
				 rtos_mem_get_free_heap_size(),
				 (u32)usb_rx_total_len, (u32)usb_rx_interval_time, (u32)((usb_rx_total_len * 8 * 1000) / (usb_rx_interval_time * 1024)));
		usb_rx_start_time = usb_rx_end_time;
		usb_rx_total_len = 0;
	}
#endif

	if ((cdc->cb != NULL) && (cdc->cb->bulk_received != NULL)) {
#if 0
		for (u32 i = 0; i < length; i++) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "%02x ", (u8)buf[i]);
		}
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
#endif
		cdc->cb->bulk_received(buf, length);
	}

	return HAL_OK;
}

static void usbh_cdc_ecm_process_bulk_in(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_pipe_info_t *pipe_info = &(cdc->bulk_rx);
	usbh_pipe_t *bulk_in = &(pipe_info->pipe);
	int status = usbh_transfer_process(host, bulk_in);
	u32 len;

#if USBH_CDC_ECM_STATE_TRACE_ENABLE
	pipe_info->trigger_cnt++;
#endif

	if ((status == HAL_OK) && (bulk_in->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, bulk_in);
		usbh_cdc_ecm_cb_bulk_receive(bulk_in->xfer_buf, len);

		bulk_in->xfer_state = USBH_EP_XFER_START;
		usbh_notify_class_state_change(host, bulk_in->pipe_num);
	} else if (bulk_in->xfer_state == USBH_EP_XFER_START) {
		len = usbh_get_last_transfer_size(host, bulk_in);
		usbh_cdc_ecm_cb_bulk_receive(bulk_in->xfer_buf, len);

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
static void usbh_cdc_ecm_process_intr_in(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_pipe_info_t *pipe_info = &(cdc->intr_rx);
	usbh_pipe_t *intr_in = &(pipe_info->pipe);
	int status = usbh_transfer_process(host, intr_in);
	u32 len;

#if USBH_CDC_ECM_STATE_TRACE_ENABLE
	pipe_info->trigger_cnt++;
#endif

	if ((status == HAL_OK) && (intr_in->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, intr_in);
		usbh_cdc_ecm_cb_intr_receive(intr_in->xfer_buf, len);
	} else if ((intr_in->xfer_state == USBH_EP_XFER_START)) {
		usbh_notify_class_state_change(host, intr_in->pipe_num);
	} else if (intr_in->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX fail: %d\n", usbh_get_urb_state(host, intr_in));
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
  * @brief  Send event to transmit BULK data.
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
		if ((ep->xfer_state == USBH_EP_XFER_IDLE) || (usbh_cdc_ecm_bulk_tx_time_check() == HAL_OK)) {

			usbh_notify_class_state_change(host, ep->pipe_num);
			ret = HAL_OK;
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
			ep->xfer_state = USBH_EP_XFER_START;
			cdc->state = CDC_ECM_STATE_TRANSFER;
			usbh_notify_class_state_change(host, ep->pipe_num);
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

	if ((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->state == CDC_ECM_STATE_TRANSFER)) {
		if (ep->xfer_state == USBH_EP_XFER_IDLE) {
			ep->xfer_buf = buf;
			ep->xfer_len = len;

			if ((len % ep->ep_mps) == 0) { //ZLP
				ep->trx_zlp = 1;
			} else {
				ep->trx_zlp = 0;
			}
			ep->xfer_state = USBH_EP_XFER_START;
			cdc->state = CDC_ECM_STATE_TRANSFER;
			usbh_notify_class_state_change(host, ep->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}

static int usbh_cdc_ecm_cb_bulk_send(int state)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	if (state == HAL_OK) {
		usb_os_sema_give(cdc->bulk_tx_sema);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail %d\n", state);
	}

	return HAL_OK;
}

static int usbh_cdc_ecm_tx_status_check(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	if (cdc->bulk_tx_block) {
		//release the sema to return usbh_cdc_ecm_send_data
		do {
			usb_os_sema_give(cdc->bulk_tx_sema);
			usb_os_sleep_ms(1U);
		} while (cdc->bulk_tx_block);
	}

	return HAL_OK;
}

static void usbh_cdc_ecm_deinit_all_pipe(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host = cdc->host;

	if (cdc->intr_rx.pipe.pipe_num) {
		USBH_CDC_ECM_FREE_MEM(cdc->intr_rx.pipe.xfer_buf);
		usbh_close_pipe(host, &(cdc->intr_rx.pipe));
	}

	if (cdc->bulk_rx.pipe.pipe_num) {
		USBH_CDC_ECM_FREE_MEM(cdc->bulk_rx.pipe.xfer_buf);
		usbh_close_pipe(host, &(cdc->bulk_rx.pipe));
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
			RTK_LOGS(TAG, RTK_LOG_INFO, "Status %d-%d-%d-%d/out %d-%d %d/in %d-%d/intr %d-%d/%d-%d-%d/%d %d %d %d\n",
					 cdc->state, cdc->eth_hw_connect, cdc->host->connect_state, cdc->intr_check_tick,
					 cdc->bulk_tx.pipe.xfer_state, usbh_get_urb_state(cdc->host, &(cdc->bulk_tx.pipe)), cdc->bulk_tx.pipe.tick,
					 cdc->bulk_rx.pipe.xfer_state, usbh_get_urb_state(cdc->host, &(cdc->bulk_rx.pipe)),
					 cdc->intr_rx.pipe.xfer_state, usbh_get_urb_state(cdc->host, &(cdc->intr_rx.pipe)),
					 cdc->bulk_tx.trigger_cnt, cdc->bulk_rx.trigger_cnt, cdc->intr_rx.trigger_cnt,
					 cdc->host->isr_process_time_max, cdc->host->isr_process_time,
					 cdc->host->isr_enter_period_max, cdc->host->isr_enter_period
					);
		}
		usb_os_sleep_ms(1000);
	}
}

void usbh_cdc_ecm_trace_task_init(void)
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

void usbh_cdc_ecm_trace_task_deinit(void)
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
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	if (cdc->host) {
		printf("Status %lld %lld %lld 0x%" PRIx64 " %lld\n",
			   cdc->host->isr_process_time_max, cdc->host->isr_process_time,
			   cdc->host->isr_enter_period_max, cdc->host->isr_enter_period,
			   cdc->host->isr_enter_time);
	}
}

static void usbh_cdc_ecm_reset_test_cnt(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host;

	if (cdc && cdc->host) {
		host = cdc->host;

		host->isr_process_time_max = 0;
		host->isr_enter_period_max = 0;

		host->isr_enter_time = 0;

		usbh_cdc_ecm_trace_time_cnt();
	}
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
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbh_cdc_ecm_cmd_table[] = {
	{"ecmh", usbh_cdc_ecm_cmd_entry},
};
#endif

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Init CDC ECM class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_cdc_ecm_init(usbh_cdc_ecm_state_cb_t *cb, usbh_cdc_ecm_priv_data_t *priv)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	int ret = HAL_OK;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	usb_os_sema_create(&(cdc->bulk_tx_sema));

	cdc->dongle_ctrl_buf = (u8 *)usb_os_malloc(USBH_CDC_ECM_MAC_STRING_LEN);
	if (NULL == cdc->dongle_ctrl_buf) {
		ret = HAL_ERR_MEM;
		goto ctrl_buf_fail;
	}

	cdc->mac_valid = 0;

	/* priv == NULL is valid (e.g. usbh_wifi_bridge always passes NULL). mac_valid is set later
	 * during enumeration; led_array/led_cnt == 0 causes LED setup to be skipped gracefully.
	 * Return HAL_OK so callers checking ret < 0 are not falsely triggered. */
	if (priv == NULL) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Param error\n");
		USBH_CDC_ECM_FREE_MEM(cdc->led_array);
		cdc->led_cnt = 0;
	} else {
		if (priv->mac_value) {
			usbh_cdc_ecm_set_dongle_mac(priv->mac_value);
		}
		if ((priv->led_array != NULL) && (priv->led_cnt > 0)) {
			usbh_cdc_ecm_set_dongle_led_array(priv->led_array, priv->led_cnt);
		}
	}

	cdc->cb = cb;
	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "User init err %d", ret);
			return ret;
		}
	}

	usbh_register_class(&usbh_cdc_ecm_driver);

#if USBH_CDC_ECM_STATE_TRACE_ENABLE
	usbh_cdc_ecm_trace_task_init();
#endif

	return ret;

ctrl_buf_fail:
	usb_os_sema_delete(cdc->bulk_tx_sema);

	return ret;
}

/**
  * @brief  Deinitialize the USB ECM Class
  * @retval Status
  */
int usbh_cdc_ecm_deinit(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	cdc->eth_hw_connect = 0;

	usbh_unregister_class(&usbh_cdc_ecm_driver);

#if USBH_CDC_ECM_STATE_TRACE_ENABLE
	usbh_cdc_ecm_trace_task_deinit();
#endif

	usbh_cdc_ecm_tx_status_check();
	usb_os_sema_delete(cdc->bulk_tx_sema);

	usbh_cdc_ecm_deinit_all_pipe();

	USBH_CDC_ECM_FREE_MEM(cdc->dongle_ctrl_buf);
	USBH_CDC_ECM_FREE_MEM(cdc->led_array);
	cdc->led_cnt = 0;

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
int usbh_cdc_ecm_choose_config(usb_host_t *host)
{
	//choose ecm cfg_num
	usbh_dev_id_t dev_id = {0,};
	dev_id.bInterfaceClass = USB_CDC_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_SUBCLASS_ECM;
	dev_id.bInterfaceProtocol = 0x00;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	u8 cfg_idx = usbh_get_configuration(host, &dev_id);
	return usbh_set_configuration(host, cfg_idx);
}

int usbh_cdc_ecm_check_config_desc(usb_host_t *host)
{
	usbh_itf_data_t *itf_data;
	u8 ecm_ctrl_valid = 0;
	u8 ecm_data_valid = 0;

	usbh_dev_id_t dev_id = {0,};
	dev_id.bInterfaceClass = USB_CDC_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_SUBCLASS_ECM;
	dev_id.bInterfaceProtocol = 0x00;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);

	if (itf_data) {
		ecm_ctrl_valid = 1;
	}

	dev_id.bInterfaceClass = USB_CDC_DATA_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_SUBCLASS_RESERVED;
	dev_id.bInterfaceProtocol = 0x00;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);

	if (itf_data) {
		ecm_data_valid = 1;
	}

	if (ecm_ctrl_valid && ecm_data_valid) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Check success idx\n");
		return HAL_OK;
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Check fail idx [%d-%d]\n",  ecm_ctrl_valid, ecm_data_valid);
	}

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
  * @brief  Start to transmit data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @param  block: Whether block until xfer done
  * @retval Status
  */
int usbh_cdc_ecm_send_data(u8 *buf, u32 len, u8 block)
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
		cdc->bulk_tx_block = 1;
		usb_os_sema_take(cdc->bulk_tx_sema, USB_OS_SEMA_TIMEOUT);
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
	// RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM TX (%d) Done\n", len);
	return ret;
}

/**
  * @brief  Get ecm device connect status
  * @retval device connect status
  */
int usbh_cdc_ecm_get_connect_status(void)//1 up
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	/* check whether the device setup phase is complete */
	if (usbh_cdc_ecm_check_enum_status() != HAL_OK) {
		return 0;
	}

	return cdc->eth_hw_connect;
}

/**
  * @brief  Get the mac str, if the dongle support the standard CDC ECM
  * @retval mac string,the length is 6 Bytes
  */
const u8 *usbh_cdc_ecm_process_mac_str(void)
{
	u8 i = 0;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	while (!cdc->mac_valid && i < 10) {
		usb_os_sleep_ms(1000);
		i++;
		RTK_LOGS(TAG, RTK_LOG_INFO,  "Wait to get MAC string!\n");
	}

	if (cdc->mac_valid == 0) {
		return NULL;
	}

	return cdc->mac;
}

__attribute__((weak))
int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbh_cdc_ecm_send_data(buf, len, block);
}
