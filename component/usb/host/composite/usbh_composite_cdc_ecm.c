/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include "usb_os.h"
#include "usbh.h"
#include "usbh_composite_cdc_ecm.h"

/* Private defines -----------------------------------------------------------*/
#define USBH_CDC_ECM_ENABLE_DUMP_DESCRIPTOR_PARSE    0

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
} usbh_composite_cdc_ecm_dongle_capability_t;

typedef enum {
	CDC_ECM_MAC_UNINIT = 0U,
	CDC_ECM_MAC_DONGLE_SUPPLY,
	CDC_ECM_MAC_UPPER_LAYER_SET,
	CDC_ECM_MAC_RANDOM_SET,
	CDC_ECM_MAC_TYPE_MAX,
} usbh_composite_cdc_ecm_dongle_mac_type_t;

/* CDC ECM state */
typedef enum {
	CDC_ECM_STATE_IDLE = 0U,
	CDC_ECM_STATE_TRANSFER,
	CDC_ECM_STATE_ERROR,
} usbh_composite_cdc_ecm_state_t;

typedef enum {
	CDC_ECM_STATE_AT_SETTING_IDLE      = 0U,
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
} usbh_composite_cdc_ecm_ctrl_state_t;

/* Private types -------------------------------------------------------------*/
typedef struct {
	u16 pid;
	u16 feature;
} __PACKED usbh_composite_cdc_ecm_priv_config_t;

/* Private macros ------------------------------------------------------------*/
#define USBH_COMPOSITE_CDC_ECM_FREE_MEM(x)  if(x){ usb_os_mfree(x); x = NULL;}

/* Private function prototypes -----------------------------------------------*/
static int usbh_composite_cdc_ecm_attach(usb_host_t *host);
static int usbh_composite_cdc_ecm_detach(usb_host_t *host);
static int usbh_composite_cdc_ecm_process(usb_host_t *host, usbh_event_t *event);
static int usbh_composite_cdc_ecm_setup(usb_host_t *host);
static int usbh_composite_cdc_ecm_sof(usb_host_t *host);
static void usbh_composite_cdc_ecm_process_bulk_out(usb_host_t *host);
static void usbh_composite_cdc_ecm_process_bulk_in(usb_host_t *host);
static void usbh_composite_cdc_ecm_process_intr_in(usb_host_t *host);
static int usbh_composite_cdc_ecm_bulk_receive(void);
static int usbh_composite_cdc_ecm_bulk_tx(void);
static int usbh_composite_cdc_ecm_intr_receive(void);
static int usbh_composite_cdc_ecm_parse_ctrl_interface(usbh_itf_data_t *itf);
static int usbh_composite_cdc_ecm_bulk_tx_time_check(void);
static int usbh_composite_cdc_ecm_bulk_rx_time_check(void);
static int usbh_composite_cdc_ecm_intr_rx_time_check(void);
#if USBH_COMP_ECM_STATE_DEBUG_ENABLE
static void usbh_composite_cdc_ecm_debug_task_init(void);
static void usbh_composite_cdc_ecm_debug_task_deinit(void);
#endif
/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "ECMH";

static const usbh_composite_cdc_ecm_priv_config_t usbh_composite_cdc_ecm_rtk_dongle[] = {
	{0x8152, CDC_ECM_CAP_FIFO_CTRL | CDC_ECM_CAP_MODIFY_MAC | CDC_ECM_CAP_MODIFY_LED},
	{0x8153, CDC_ECM_CAP_RCR_CMD | CDC_ECM_CAP_MODIFY_MAC | CDC_ECM_CAP_MODIFY_LED},
	{0x8156, CDC_ECM_CAP_RCR_CMD | CDC_ECM_CAP_MODIFY_MAC | CDC_ECM_CAP_MODIFY_LED},

	{0, 0x00},
};

/* USB Class Driver */
const usbh_class_driver_t usbh_composite_cdc_ecm_driver = {
	.attach  = usbh_composite_cdc_ecm_attach,
	.detach  = usbh_composite_cdc_ecm_detach,
	.setup   = usbh_composite_cdc_ecm_setup,
	.process = usbh_composite_cdc_ecm_process,
	.sof = usbh_composite_cdc_ecm_sof,
};

#if USBH_COMP_ECM_STATE_DEBUG_ENABLE
rtos_task_t usbh_composite_cdc_ecm_trace_task;
u8 usbh_composite_cdc_ecm_trace_task_init_flag;
#endif
static usbh_composite_cdc_ecm_host_t usbh_composite_cdc_ecm_host;

/* Private functions ---------------------------------------------------------*/
/* USB basic functions ----------------------------------------------------*/
static u8 usbh_composite_cdc_ecm_support_feature(u16 pid, usbh_composite_cdc_ecm_dongle_capability_t type)
{
	usbh_composite_cdc_ecm_priv_config_t *ecm_dongle;
	u16 i;

	for (i = 0; usbh_composite_cdc_ecm_rtk_dongle[i].pid; i++) {
		ecm_dongle = (usbh_composite_cdc_ecm_priv_config_t *) & (usbh_composite_cdc_ecm_rtk_dongle[i]);
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

static u16 usbh_composite_cdc_ecm_get_eth_status_check_time(void)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	if ((cdc->driver != NULL) && (cdc->driver->host != NULL) && (cdc->driver->host->dev_speed == USB_SPEED_HIGH)) {
		return 8 * USBH_CDC_ECM_ETH_STATUS_CHECK_TIME;
	}

	return USBH_CDC_ECM_ETH_STATUS_CHECK_TIME;
}

static int usbh_composite_cdc_ecm_tx_status_check(void)
{
	if (usbh_composite_cdc_ecm_host.bulk_tx_block) {
		//release the sema to return usbh_composite_cdc_ecm_send_data
		do {
			usb_os_sema_give(usbh_composite_cdc_ecm_host.bulk_tx_sema);
			usb_os_sleep_ms(1U);
		} while (usbh_composite_cdc_ecm_host.bulk_tx_block);
	}

	return HAL_OK;
}

static void usbh_composite_cdc_ecm_cb_bulk_send(int state)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	if (state == HAL_OK) {
		usb_os_sema_give(cdc->bulk_tx_sema);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail\n");
	}
}

static int usbh_composite_cdc_ecm_cb_bulk_receive(u8 *pbuf, u32 len)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	if (pbuf == NULL || len == 0) {
		return HAL_ERR_PARA;
	}

#if USBH_CDC_ECM_RX_SPEED_CHECK
	static u64 usb_rx_start_time = 0, usb_rx_end_time, usb_rx_interval_time; //ms
	static u64 usb_rx_total_len = 0;

	usb_rx_total_len += len;
	if (usb_rx_start_time == 0) {
		usb_rx_start_time = usb_os_get_timestamp_ms();
	}
	usb_rx_end_time = usb_os_get_timestamp_ms();
	usb_rx_interval_time = (usb_rx_end_time - usb_rx_start_time);

	if (usb_rx_interval_time >= 2000) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Heap %d RX %dB in %d ms, %d Kbps\n",
				 rtos_mem_get_free_heap_size(),
				 (u32)usb_rx_total_len, (u32)usb_rx_interval_time, (u32)((usb_rx_total_len * 8 * 1000) / (usb_rx_interval_time * 1024)));
		usb_rx_start_time = usb_rx_end_time;
		usb_rx_total_len = 0;
	}
#endif

	if ((cdc->cb != NULL) && (cdc->cb->bulk_received != NULL) && (len > 0)) {
		cdc->cb->bulk_received(pbuf, len);
	}

	return HAL_OK;
}

static int usbh_composite_cdc_ecm_cb_intr_receive(u8 *buf, u32 length)
{
	usbh_composite_cdc_ecm_host_t   *cdc = &usbh_composite_cdc_ecm_host;
	if (buf && length >= 8) {
		/*A1 00 00 00 01 00 00 00 */
		if (length == 8 && buf[0] == 0xA1 && buf[1] == USB_CDC_ECM_NOTIFY_NETWORK_CONNECTION) {
			cdc->eth_hw_connect = buf[2];
			if (cdc->eth_hw_connect && (cdc->intr_check_tick != usbh_composite_cdc_ecm_get_eth_status_check_time())) {
				cdc->intr_check_tick = usbh_composite_cdc_ecm_get_eth_status_check_time();
			} else if ((cdc->eth_hw_connect == 0) && (cdc->intr_rx.pipe.ep_interval != 0)) {
				cdc->intr_check_tick = cdc->intr_rx.pipe.ep_interval;
			}
			cdc->intr_rx.pipe.max_timeout_tick = cdc->intr_check_tick;
		}
		/*A1 2A 00 00 01 00 00 08 	00 00 00 00 00 00 00 00 */
		else if (length == 16 && buf[0] == 0xA1 && buf[1] == USB_CDC_ECM_NOTIFY_CONNECTION_SPEED_CHANGE) {
			//usb_cdc_ecm_notify_t
		}
	}

	return HAL_OK;
}

static void usbh_composite_cdc_ecm_dump_ecm_cfgdesc(void)
{
#if USBH_CDC_ECM_ENABLE_DUMP_DESCRIPTOR_PARSE
	usbh_composite_cdc_ecm_host_t   *cdc = &usbh_composite_cdc_ecm_host;
	usbh_ep_desc_t        *ep_desc;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info;

	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------ECM Dump Descriptor Start------------------\n");

	//dump ecm control
	pipe_info = &(cdc->intr_rx);
	ep_desc = &(pipe_info->ep_desc);
	RTK_LOGS(TAG, RTK_LOG_INFO, "INTR addr(0x%x)MPS(%d)pipe(%d)interval(%d) MAC(%d)\n",
			 ep_desc->bEndpointAddress, ep_desc->wMaxPacketSize, pipe_info->pipe.pipe_num, pipe_info->pipe.ep_interval, cdc->mac_str_id);

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
  * @brief  Get string descriptor by str id
  * @param  host: Host handle
  * @param  pbuf: buffer handle which will save the string value
  * @param  buflen: buffer length
  * @param  str_id: string index
  * @retval Status
  */
static int usbh_composite_cdc_ecm_get_string(usb_host_t *host, u8 *pbuf, u16 buflen, u8 str_id)
{
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD;
	setup.req.bRequest = USB_REQ_GET_DESCRIPTOR;
	setup.req.wValue = USB_DESC_STRING | str_id;
	setup.req.wIndex = 1U;
	setup.req.wLength = buflen;

	return usbh_ctrl_request(host, &setup, pbuf);
}

static u8 usbh_composite_cdc_ecm_get_mac_str(usb_host_t *host)
{
	u8 i;
	u8 mac_is_valid = 0;
	u8 status = HAL_OK;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	u16 vid = cdc->vid;
	u16 pid = cdc->pid;

	if ((cdc->mac_src_type == CDC_ECM_MAC_UPPER_LAYER_SET) && (vid == USBH_COMPOSITE_DEVICE_VID) &&
		(usbh_composite_cdc_ecm_support_feature(pid, CDC_ECM_CAP_MODIFY_MAC) == HAL_OK)) {
		status = HAL_OK;
		RTK_LOGS(TAG, RTK_LOG_INFO,  "Upper set mac[%02x %02x %02x %02x %02x %02x]\n", cdc->mac[0], cdc->mac[1], cdc->mac[2], cdc->mac[3], cdc->mac[4],
				 cdc->mac[5]);
	} else {
		status = usbh_composite_cdc_ecm_get_string(host, cdc->dongle_ctrl_buf, USBH_CDC_ECM_MAC_STRING_LEN, cdc->mac_str_id);
		cdc->mac_src_type = CDC_ECM_MAC_DONGLE_SUPPLY;
	}

	if (status == HAL_OK) {
		if (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY) {
			for (i = 0; i < 6; i++) {
				cdc->mac[i] = usbh_composite_cdc_ecm_hex_to_char(cdc->dongle_ctrl_buf[2 + 4 * i]) * 16 + usbh_composite_cdc_ecm_hex_to_char(
								  cdc->dongle_ctrl_buf[2 + 4 * i + 2]);
				if (cdc->mac[i]) {
					mac_is_valid = 1;
				}
			}

			if (mac_is_valid == 0) {
				TRNG_get_random_bytes(cdc->mac, USBH_COMP_ECM_MAC_STR_LEN);
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
static int usbh_composite_cdc_ecm_process_set_alt(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_REQ_SET_INTERFACE;
	setup.req.wValue = cdc->if_num;
	setup.req.wIndex = cdc->alt_set;
	setup.req.wLength = 0U;

	return usbh_ctrl_request(host, &setup, NULL);
}

#if USBH_CDC_ECM_ENABLE_PACKETFILTER
/**
  * @brief  Set Ethernet Packet Filter
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_cdc_ecm_process_set_packet_filter(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

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
static int usbh_composite_cdc_ecm_process_set_muiticast_filter(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

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
static int usbh_composite_cdc_ecm_process_get_statistic(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

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
static int usbh_composite_cdc_ecm_process_get_rcr(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

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
static int usbh_composite_cdc_ecm_process_set_rcr(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

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
static int usbh_composite_cdc_ecm_process_set_flow_ctrl1(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

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
static int usbh_composite_cdc_ecm_process_set_flow_ctrl2(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

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
static int usbh_composite_cdc_ecm_process_mac_get_lock(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xE81C;
	setup.req.wIndex = 0x010F;
	setup.req.wLength = 4;

	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

static int usbh_composite_cdc_ecm_process_mac_set_dis_lock(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xE81C;
	setup.req.wIndex = 0x010F;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

static int usbh_composite_cdc_ecm_process_mac_set_mac1(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xC000;
	setup.req.wIndex = 0x010F;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

static int usbh_composite_cdc_ecm_process_mac_set_mac2(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xC004;
	setup.req.wIndex = 0x0103;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

static int usbh_composite_cdc_ecm_process_mac_en_lock(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

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
static int usbh_composite_cdc_ecm_process_led_set_ctrl(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xDD90;
	setup.req.wIndex = 0x0103;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

static void usbh_composite_cdc_ecm_set_dongle_mac(const u8 *mac)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	if (NULL == mac) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Param error,mac is NULL\n");
		return ;
	}

	memcpy((void *) & (cdc->mac[0]), (const void *)mac, USBH_COMP_ECM_MAC_STR_LEN);
	cdc->mac_src_type = CDC_ECM_MAC_UPPER_LAYER_SET;
}

static void usbh_composite_cdc_ecm_set_dongle_led_array(const u16 *led, u8 len)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	if (led == NULL || len == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Param error,led is NULL or len %d\n", len);
		return ;
	}

	USBH_COMPOSITE_CDC_ECM_FREE_MEM(cdc->led_array);

	cdc->led_array = (u16 *)usb_os_malloc(len * sizeof(u16));
	if (cdc->led_array == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc led array fail\n");
		cdc->led_cnt = 0;
		return ;
	}
	memcpy((void *)cdc->led_array, led, len * sizeof(u16));

	cdc->led_cnt = len;
}

static void usbh_composite_cdc_ecm_config_dongle_mac(usb_host_t *host, u16 vid, u16 pid)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	u8 state = HAL_OK;

	if ((vid != USBH_COMPOSITE_DEVICE_VID) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY) ||
		(usbh_composite_cdc_ecm_support_feature(pid, CDC_ECM_CAP_MODIFY_MAC) != HAL_OK)) {
		return;
	}

	switch (cdc->sub_status) {
	case CDC_ECM_STATE_CTRL_MAC_GET_LOCK: //8152 mac
		usb_os_memset(cdc->mac_ctrl_lock, 0, 4);
		state = usbh_composite_cdc_ecm_process_mac_get_lock(host);
		if (state == HAL_OK) {
			usb_os_memcpy(cdc->mac_ctrl_lock, cdc->dongle_ctrl_buf, USBH_COMP_ECM_CTRL_REG_BUF_LEN);
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get MAC lock err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_DISABLE_LOCK: //8152 mac
		cdc->mac_ctrl_lock[0] = 0xD0;
		usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->mac_ctrl_lock, USBH_COMP_ECM_CTRL_REG_BUF_LEN);
		state = usbh_composite_cdc_ecm_process_mac_set_dis_lock(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Dis MAC lock err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_SET_MAC1: //8152 mac
		usb_os_memcpy(cdc->dongle_ctrl_buf, &(cdc->mac[0]), USBH_COMP_ECM_CTRL_REG_BUF_LEN);
		state = usbh_composite_cdc_ecm_process_mac_set_mac1(host);
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
		state = usbh_composite_cdc_ecm_process_mac_set_mac2(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set MAC2 err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_ENABLE_LOCK: //8152 mac
		cdc->mac_ctrl_lock[0] = 0x10;
		usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->mac_ctrl_lock, USBH_COMP_ECM_CTRL_REG_BUF_LEN);
		state = usbh_composite_cdc_ecm_process_mac_en_lock(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
			cdc->mac_valid = 1;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Mac set done\n");
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

int usbh_composite_cdc_ecm_ctrl_setting(usb_host_t *host)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
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
		if (pid != USBH_COMPOSITE_QUECTEL_DONGLE_EG91_PID) {
			state = usbh_composite_cdc_ecm_get_mac_str(host);
			if (state == HAL_OK) {
				cdc->sub_status ++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Get MAC fail\n");
				usb_os_sleep_ms(10);
			}
		} else  { //eg91 not support mac string
			cdc->sub_status ++;
		}
		break;

	case CDC_ECM_STATE_CTRL_ALT_SETTING:
		state = usbh_composite_cdc_ecm_process_set_alt(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
			// cdc->sub_status = CDC_ECM_STATE_AT_SETTING_MAX; //force to jump
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set alt err\n");
			usb_os_sleep_ms(100);
		}
		break;

#if USBH_CDC_ECM_ENABLE_PACKETFILTER
	case CDC_ECM_STATE_CTRL_SET_ETHERNET_MULTICAST_FILTER:
		usb_os_memcpy(cdc->muticast_filter, mac_str, 6);
		cdc->muticast_filter_len = 6;
		state = usbh_composite_cdc_ecm_process_set_muiticast_filter(host);
		if (state == HAL_OK) {
			usb_os_memcpy(cdc->muticast_filter, cdc->dongle_ctrl_buf, cdc->muticast_filter_len);
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set mul-filter err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_SET_ETHERNET_PACKET_FILTER:
		cdc->packet_filter = CDC_ECM_ETH_PACKET_TYPE_DIRECTED;
		state = usbh_composite_cdc_ecm_process_set_packet_filter(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set pkt-filter err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_GET_ETHERNET_STATISTIC:
		state = usbh_composite_cdc_ecm_process_get_statistic(host);
		if (state == HAL_OK) {
			usb_os_memcpy(&(cdc->eth_statistic_count), cdc->dongle_ctrl_buf, USBH_COMP_ECM_CTRL_REG_BUF_LEN);
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get statistic err\n");
			usb_os_sleep_ms(100);
		}
		break;
#endif

	case CDC_ECM_STATE_CTRL_RCR_GET:
		if ((vid != USBH_COMPOSITE_DEVICE_VID) || (usbh_composite_cdc_ecm_support_feature(pid, CDC_ECM_CAP_RCR_CMD) != HAL_OK)) {
			cdc->sub_status++;
		} else {
			usb_os_memset(cdc->rcr, 0, 4);
			state = usbh_composite_cdc_ecm_process_get_rcr(host);
			if (state == HAL_OK) {
				usb_os_memcpy(cdc->rcr, cdc->dongle_ctrl_buf, USBH_COMP_ECM_CTRL_REG_BUF_LEN);
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get RCR err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_RCR_SET:
		if ((vid != USBH_COMPOSITE_DEVICE_VID) || (usbh_composite_cdc_ecm_support_feature(pid, CDC_ECM_CAP_RCR_CMD) != HAL_OK)) {
			cdc->sub_status++;
		} else {
			//set bit 0~3 set 1 will enable PING & UDP transfer
			cdc->rcr[0] = cdc->rcr[0] | 0x0F;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->rcr, USBH_COMP_ECM_CTRL_REG_BUF_LEN);
			state = usbh_composite_cdc_ecm_process_set_rcr(host);
			if (state == HAL_OK) {
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set RCR err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_FLOW_CTRL1:
		if ((vid != USBH_COMPOSITE_DEVICE_VID) || (usbh_composite_cdc_ecm_support_feature(pid, CDC_ECM_CAP_FIFO_CTRL) != HAL_OK)) {
			cdc->sub_status++;
		} else {
			usb_os_memset(cdc->flow_ctrl, 0, 4);
			cdc->flow_ctrl[0] = 0x60;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->flow_ctrl, USBH_COMP_ECM_CTRL_REG_BUF_LEN);
			state = usbh_composite_cdc_ecm_process_set_flow_ctrl1(host);
			if (state == HAL_OK) {
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set flow ctrl1 err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_FLOW_CTRL2:
		if ((vid != USBH_COMPOSITE_DEVICE_VID) || (usbh_composite_cdc_ecm_support_feature(pid, CDC_ECM_CAP_FIFO_CTRL) != HAL_OK)) {
			cdc->sub_status++;
		} else {
			usb_os_memset(cdc->flow_ctrl, 0, 4);
			cdc->flow_ctrl[0] = 0xa0;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->flow_ctrl, USBH_COMP_ECM_CTRL_REG_BUF_LEN);
			state = usbh_composite_cdc_ecm_process_set_flow_ctrl2(host);
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
		if ((vid != USBH_COMPOSITE_DEVICE_VID) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY) ||
			(usbh_composite_cdc_ecm_support_feature(pid, CDC_ECM_CAP_MODIFY_MAC) != HAL_OK)) {
			cdc->sub_status = CDC_ECM_STATE_CTRL_LED_COLOR_SET;
		} else {
			usbh_composite_cdc_ecm_config_dongle_mac(host, vid, pid);
		}
		break;

	case CDC_ECM_STATE_CTRL_LED_COLOR_SET:
		if ((vid != USBH_COMPOSITE_DEVICE_VID) || (cdc->led_cnt == 0) || (cdc->led_array == NULL) ||
			(usbh_composite_cdc_ecm_support_feature(pid, CDC_ECM_CAP_MODIFY_LED) != HAL_OK)) {
			cdc->sub_status++;
		} else {
			usb_os_memset(cdc->dongle_ctrl_buf, 0xFF, 4);
			usb_os_memcpy(cdc->dongle_ctrl_buf, (u8 *) & (cdc->led_array[0]), 2);
			state = usbh_composite_cdc_ecm_process_led_set_ctrl(host);
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

/* Close all opened pipes only. The xfer buffers are owned by init/deinit
   (not attach/detach) so they survive a detach and are reused on re-attach;
   they must NOT be freed here. */
static void usbh_composite_cdc_ecm_deinit_all_pipe(void)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usb_host_t *host = cdc->driver->host;

	if (cdc->intr_rx.pipe.pipe_num) {
		usbh_close_pipe(host, &(cdc->intr_rx.pipe));
	}

	if (cdc->bulk_rx.pipe.pipe_num) {
		usbh_close_pipe(host, &(cdc->bulk_rx.pipe));
	}

	if (cdc->bulk_tx.pipe.pipe_num) {
		usbh_close_pipe(host, &(cdc->bulk_tx.pipe));
	}
}

/**
  * @brief  Parse audio streaming interface
  * @param  pbuf: given descriptor buffer
  * @param  length: length of parse desc length
  * @param  buf_length: length of given buffer
  * @retval Status
  */
static int usbh_composite_cdc_ecm_parse_ctrl_interface(usbh_itf_data_t *itf_data)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *ctrl_ep = &(cdc->intr_rx);
	usbh_itf_desc_t *itf_desc;
	usb_cdc_ecm_ethernet_function_desc_t *eth_desc;
	u8 *desc;
	u16 total;
	u8 len;

	if (itf_data == NULL || itf_data->raw_data == NULL || itf_data->itf_desc_array == NULL) {
		return HAL_ERR_PARA;
	}

	itf_desc = &itf_data->itf_desc_array[0];
	if (itf_desc->bNumEndpoints != 1) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Ctrl EP num 1-%d\n", itf_desc->bNumEndpoints);
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
		if (((usbh_desc_header_t *)desc)->bDescriptorType == USB_CDC_ECM_CS_INTERFACE) {
			eth_desc = (usb_cdc_ecm_ethernet_function_desc_t *)desc;
			if (eth_desc->bDescriptorSubtype == USB_CDC_ECM_FUNC_DESC_ETHERNET_NETWORKING) {
				cdc->mac_str_id = eth_desc->iMACAddress;
				cdc->sub_status = CDC_ECM_STATE_GET_MAC_STR;
				break;
			}
		}
		desc += len;
		total += len;
	}

	return HAL_OK;
}


static int usbh_composite_cdc_ecm_parse_interface_desc(usb_host_t *host)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_dev_id_t dev_id_info = {0,};
	usbh_itf_desc_t *data_itf_desc;
	usbh_dev_desc_t *pdesc;
	usbh_itf_data_t *itf;
	usbh_ep_desc_t *ep;
	u8 ret = HAL_ERR_UNKNOWN;
	u8 alt_idx;
	u8 i;

	pdesc = cdc->driver->host->dev_desc;
	cdc->vid = pdesc->idVendor;
	cdc->pid = pdesc->idProduct;

	/* Get interface index as per supported class & protocol */
	/*
		cdc ecm ctrl
	*/
	dev_id_info.bInterfaceClass = USB_CDC_ECM_CLASS_CODE;
	dev_id_info.bInterfaceSubClass = USB_CDC_ECM_SUBCLASS_ECM;
	dev_id_info.bInterfaceProtocol = 0x00;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf = (usbh_itf_data_t *) usbh_get_interface_descriptor(host, &dev_id_info);

	if (itf) {
		ret = usbh_composite_cdc_ecm_parse_ctrl_interface(itf);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Ctrl parse fail\n");
			return ret;
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get if fail\n");
		return HAL_ERR_PARA;
	}

	dev_id_info.bInterfaceClass = USB_CDC_ECM_DATA_INTERFACE_CLASS_CODE;
	dev_id_info.bInterfaceSubClass = USB_CDC_ECM_SUBCLASS_RESERVED;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS;
	itf = (usbh_itf_data_t *) usbh_get_interface_descriptor(host, &dev_id_info);
	data_itf_desc = NULL;
	while (itf && data_itf_desc == NULL) { // find the ecm alt setting usbh_itf_desc_t
		for (alt_idx = 0; alt_idx < itf->alt_setting_cnt; alt_idx++) {
			if (itf->itf_desc_array[alt_idx].bNumEndpoints > 0) { //alt has endpoint
				data_itf_desc = &itf->itf_desc_array[alt_idx];
				break;
			}
		}
		if (data_itf_desc == NULL) {
			itf = itf->next;
		}
	}

	if (data_itf_desc != NULL) {
		cdc->if_num = data_itf_desc->bInterfaceNumber;
		cdc->alt_set = data_itf_desc->bAlternateSetting;
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
static int usbh_composite_cdc_ecm_attach(usb_host_t *host) //parse all ep info
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info;
	int status = HAL_ERR_UNKNOWN;

	status = usbh_composite_cdc_ecm_parse_interface_desc(host);
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse desc fail\n");
		return status;
	}

	//control in
	pipe_info = &(cdc->intr_rx);
	if (pipe_info->valid) {
		usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc));
		/* buffer sized to the real INTR ep_mps: the controller DMAs up to a
		   full MPS packet, so the buffer must be >= ep_mps (not just the
		   notify struct size). Allocated here / freed in detach. */
		cdc->intr_rx_buf = (u8 *)usb_os_malloc(pipe_info->pipe.ep_mps);
		if (cdc->intr_rx_buf == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc intr rx buf fail\n");
			usbh_close_pipe(host, &(pipe_info->pipe));
			return HAL_ERR_MEM;
		}
		pipe_info->pipe.xfer_buf = cdc->intr_rx_buf;
		pipe_info->pipe.xfer_len = pipe_info->pipe.ep_mps;
		pipe_info->pipe.xfer_state = USBH_EP_XFER_START;

		/* clamp to >= 1 tick: a 0 interval would make intr_rx_time_check fire
		   every SOF (>= 0 is always true), flooding the INTR pipe. */
		cdc->intr_check_tick = (pipe_info->pipe.ep_interval != 0) ? pipe_info->pipe.ep_interval : 10U;
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
		/* xfer_buf is pre-allocated in init with USBH_CDC_ECM_BULK_BUF_MAX_SIZE
		   (512*3), large enough for a whole 1542-byte ethernet packet. */
		pipe_info->pipe.xfer_buf = cdc->bulk_rx_buf;
		pipe_info->pipe.xfer_len = USBH_CDC_ECM_BULK_BUF_MAX_SIZE;
		pipe_info->pipe.xfer_state = USBH_EP_XFER_START;

		pipe_info->pipe.max_timeout_tick = USBH_CDC_ECM_BULK_IN_IDLE_MAX_CNT;
	}

	usbh_composite_cdc_ecm_dump_ecm_cfgdesc();

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
static int usbh_composite_cdc_ecm_detach(usb_host_t *host)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	UNUSED(host);

	cdc->state = CDC_ECM_STATE_IDLE;

	usbh_composite_cdc_ecm_deinit_all_pipe();   /* closes pipes -> stops DMA */
	/* intr_rx_buf was sized to this device's INTR ep_mps in attach; free it
	   only after the pipe is closed (DMA stopped). bulk_rx_buf stays
	   init-owned. */
	USBH_COMPOSITE_CDC_ECM_FREE_MEM(cdc->intr_rx_buf);

	/* clear endpoint valid flags so a re-attach that fails to parse the new
	   device's descriptors cannot reopen pipes from a stale layout. */
	cdc->intr_rx.valid = 0;
	cdc->bulk_tx.valid = 0;
	cdc->bulk_rx.valid = 0;

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
static int usbh_composite_cdc_ecm_setup(usb_host_t *host)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
		cdc->cb->setup();
	}

	cdc->state = CDC_ECM_STATE_TRANSFER;

	usbh_notify_class_state_change(host, 0);

	return HAL_OK;
}


/**
  * @brief  Usb State Machine handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_cdc_ecm_process(usb_host_t *host, usbh_event_t *event)
{
	u8 req_status = HAL_OK;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	switch (cdc->state) {
	case CDC_ECM_STATE_TRANSFER:
		if (event) {
			if (event->pipe_num == cdc->bulk_tx.pipe.pipe_num) {
				usbh_composite_cdc_ecm_process_bulk_out(host);
			} else if (event->pipe_num == cdc->bulk_rx.pipe.pipe_num) {
				usbh_composite_cdc_ecm_process_bulk_in(host);
			} else if (event->pipe_num == cdc->intr_rx.pipe.pipe_num) {
				usbh_composite_cdc_ecm_process_intr_in(host);
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
  * @brief  SOF callback for composite CDC-ECM class-specific timing process.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  host: USB host handle.
  * @retval Status
  */
static int usbh_composite_cdc_ecm_sof(usb_host_t *host)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	UNUSED(host);

	if (cdc->ready_to_xfer) {
		/* pipe_num != 0 means the pipe is open (0 is reserved for control).
		   Guard before notifying so a not-yet-opened pipe is never triggered. */
		if (cdc->intr_rx.pipe.pipe_num != 0) {
			usbh_composite_cdc_ecm_intr_receive();
		}

		if (cdc->eth_hw_connect) {
			if (cdc->bulk_rx.pipe.pipe_num != 0) {
				usbh_composite_cdc_ecm_bulk_receive();
			}
			if (cdc->bulk_tx.pipe.pipe_num != 0) {
				usbh_composite_cdc_ecm_bulk_tx();
			}
		}
	}

	return HAL_OK;
}

/**
  * @brief  Bulk Out handling function
  * @param  host: Host handle
  * @retval Status
*/
static void usbh_composite_cdc_ecm_process_bulk_out(usb_host_t *host)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info = &(cdc->bulk_tx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	int status;

#if USBH_COMP_ECM_STATE_DEBUG_ENABLE
	pipe_info->trigger_cnt++;
#endif

	status = usbh_transfer_process(host, ep);
	if ((status == HAL_OK) && (ep->xfer_state == USBH_EP_XFER_IDLE)) {
		usbh_composite_cdc_ecm_cb_bulk_send(status);
	} else if (ep->xfer_state == USBH_EP_XFER_START) {
		usbh_notify_class_state_change(host, ep->pipe_num);
	} else if (ep->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail %d\n", usbh_get_urb_state(host, ep));
		ep->xfer_state = USBH_EP_XFER_START;
		usbh_notify_class_state_change(host, ep->pipe_num);
	}
}

static void usbh_composite_cdc_ecm_process_bulk_in(usb_host_t *host)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info = &(cdc->bulk_rx);
	usbh_pipe_t *bulk_in = &(pipe_info->pipe);
	int status = usbh_transfer_process(host, bulk_in);
	u32 len;

#if USBH_COMP_ECM_STATE_DEBUG_ENABLE
	pipe_info->trigger_cnt++;
#endif

	if ((status == HAL_OK) && (bulk_in->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, bulk_in);
		usbh_composite_cdc_ecm_cb_bulk_receive(bulk_in->xfer_buf, len);

		bulk_in->xfer_state = USBH_EP_XFER_START;
		usbh_notify_class_state_change(host, bulk_in->pipe_num);
	} else if (bulk_in->xfer_state == USBH_EP_XFER_START) {
		/* Not a duplicate report: for a BULK IN whose payload is an exact multiple
		   of MPS and expects a trailing ZLP, usbh_in_transfer_done_process leaves
		   xfer_state at START *after* the N*MPS payload was really received, so the
		   data must be reported here (exactly once). The following ZLP completes
		   with len == 0 and is dropped by cb_bulk_receive's len > 0 check.
		   INTR has no such sub-state (its done path always goes IDLE), which is why
		   process_intr_in intentionally does NOT report in its START branch. */
		len = usbh_get_last_transfer_size(host, bulk_in);
		usbh_composite_cdc_ecm_cb_bulk_receive(bulk_in->xfer_buf, len);

		usbh_notify_class_state_change(host, bulk_in->pipe_num);
	} else if (bulk_in->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK RX fail %d\n", usbh_get_urb_state(host, bulk_in));
		bulk_in->xfer_state = USBH_EP_XFER_START;
		usbh_notify_class_state_change(host, bulk_in->pipe_num);
	}
}

/**
  * @brief  Intr in function
  * @param  host: Host handle
  * @retval Status
*/
static void usbh_composite_cdc_ecm_process_intr_in(usb_host_t *host)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info = &(cdc->intr_rx);
	usbh_pipe_t *intr_in = &(pipe_info->pipe);
	int status = usbh_transfer_process(host, intr_in);
	u32 len;

#if USBH_COMP_ECM_STATE_DEBUG_ENABLE
	pipe_info->trigger_cnt++;
#endif

	if ((status == HAL_OK) && (intr_in->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, intr_in);
		usbh_composite_cdc_ecm_cb_intr_receive(intr_in->xfer_buf, len);
	} else if ((intr_in->xfer_state == USBH_EP_XFER_START)) {
		usbh_notify_class_state_change(host, intr_in->pipe_num);
	} else if (intr_in->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX fail %d\n", usbh_get_urb_state(host, intr_in));
	}
}

/**
  * @brief Check ecm bulk rx status and tiemout, whether should trigger
  * @retval Status
  */
static int usbh_composite_cdc_ecm_bulk_rx_time_check(void)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info = &(cdc->bulk_rx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->driver->host;
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
static int usbh_composite_cdc_ecm_bulk_tx_time_check(void)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info = &(cdc->bulk_tx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->driver->host;
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
static int usbh_composite_cdc_ecm_intr_rx_time_check(void)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info = &(cdc->intr_rx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->driver->host;
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
  * @brief  check USB device enum status
  * @retval return HAL_OK if enum success, else return HAL_BUSY
  */
static int usbh_composite_cdc_ecm_check_enum_status(void)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	if ((cdc->driver->host != NULL) && (cdc->driver->host->connect_state >= USBH_STATE_SETUP)) {
		return HAL_OK;
	}

	return HAL_BUSY;
}

/**
  * @brief  Send event for bulk tx
  * @param  none
  * @retval Status
  */
static int usbh_composite_cdc_ecm_bulk_tx(void)
{
	int ret = HAL_BUSY;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_pipe_t  *ep = &(cdc->bulk_tx.pipe);
	usb_host_t *host = cdc->driver->host;

	if (usbh_composite_cdc_ecm_check_enum_status() != HAL_OK) {
		return ret;
	}

	if (cdc->state == CDC_ECM_STATE_TRANSFER) {
		if ((ep->xfer_state == USBH_EP_XFER_IDLE) || (usbh_composite_cdc_ecm_bulk_tx_time_check() == HAL_OK)) {
			usbh_notify_class_state_change(host, ep->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Send event to receive BULK data
  * @retval Status
  */
static int usbh_composite_cdc_ecm_bulk_receive(void)
{
	int ret = HAL_BUSY;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_pipe_t  *ep = &(cdc->bulk_rx.pipe);
	usb_host_t *host = cdc->driver->host;

	if (usbh_composite_cdc_ecm_check_enum_status() != HAL_OK) {
		return ret;
	}

	if (cdc->state == CDC_ECM_STATE_TRANSFER) {
		if ((ep->xfer_state == USBH_EP_XFER_IDLE) || (usbh_composite_cdc_ecm_bulk_rx_time_check() == HAL_OK)) {
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
  * @brief  Send event to receive INTR data
  * @retval Status
  */
static int usbh_composite_cdc_ecm_intr_receive(void)
{
	int ret = HAL_BUSY;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_pipe_t  *ep = &(cdc->intr_rx.pipe);
	usb_host_t *host = cdc->driver->host;

	if (usbh_composite_cdc_ecm_check_enum_status() != HAL_OK) {
		return ret;
	}

	if (cdc->state == CDC_ECM_STATE_TRANSFER) {
		if (((ep->xfer_state == USBH_EP_XFER_IDLE) && (usbh_get_elapsed_ticks(host, ep->tick) >= cdc->intr_check_tick))
			|| (usbh_composite_cdc_ecm_intr_rx_time_check() == HAL_OK)) {
			ep->xfer_state = USBH_EP_XFER_START;
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
static int usbh_composite_cdc_ecm_bulk_send(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info = &(cdc->bulk_tx);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->driver->host;

	if (usbh_composite_cdc_ecm_check_enum_status() != HAL_OK) {
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

#if USBH_COMP_ECM_STATE_DEBUG_ENABLE
static u8 usbh_composite_cdc_ecm_get_hw_state(void)
{
	return usbh_composite_cdc_ecm_host.eth_hw_connect;
}

static void usbh_composite_cdc_ecm_state_check_task(void *param)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	UNUSED(param);
	usbh_composite_cdc_ecm_trace_task_init_flag = 1;

	while (1) {
		if (cdc->driver->host) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Status %d-%d-%d/out %d-%d %d-%d/in %d-%d/intr %d-%d/%d-%d-%d\n",
					 cdc->state, usbh_composite_cdc_ecm_get_hw_state(), (usbh_composite_cdc_ecm_check_enum_status() == HAL_OK),
					 cdc->bulk_tx.pipe.xfer_state, usbh_get_urb_state(cdc->driver->host, &(cdc->bulk_tx.pipe)),
					 cdc->bulk_tx.pipe.tick, cdc->bulk_tx_block,
					 cdc->bulk_rx.pipe.xfer_state, usbh_get_urb_state(cdc->driver->host, &(cdc->bulk_rx.pipe)),
					 cdc->intr_rx.pipe.xfer_state, usbh_get_urb_state(cdc->driver->host, &(cdc->intr_rx.pipe)),
					 cdc->bulk_tx.trigger_cnt, cdc->bulk_rx.trigger_cnt, cdc->intr_rx.trigger_cnt
					);
		}
		usb_os_sleep_ms(1000);
	}
}

void usbh_composite_cdc_ecm_debug_task_init(void)
{
	int status;

	if (usbh_composite_cdc_ecm_trace_task_init_flag) {
		return;
	}

	status = rtos_task_create(&(usbh_composite_cdc_ecm_trace_task), "usbh_composite_cdc_ecm_state_check_task", usbh_composite_cdc_ecm_state_check_task, NULL,
							  1024U * 2, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create dump task fail\n");
	}
}

void usbh_composite_cdc_ecm_debug_task_deinit(void)
{
	if (usbh_composite_cdc_ecm_trace_task != NULL) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Del debug task\n");
		rtos_task_delete(usbh_composite_cdc_ecm_trace_task);
		usbh_composite_cdc_ecm_trace_task = NULL;
	}

	usbh_composite_cdc_ecm_trace_task_init_flag = 0;
}
#endif

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Init CDC ECM class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_composite_cdc_ecm_init(usbh_composite_host_t *host, const usbh_composite_cdc_ecm_usr_cb_t *cb)
{
	int ret = HAL_OK;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	const usbh_composite_cdc_ecm_priv_data_t *priv;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	usb_os_memset(cdc, 0x00, sizeof(usbh_composite_cdc_ecm_host_t));

	cdc->driver = host;
	priv = cb->priv;

	cdc->dongle_ctrl_buf = (u8 *)usb_os_malloc(USBH_CDC_ECM_MAC_STRING_LEN);
	if (NULL == cdc->dongle_ctrl_buf) {
		return HAL_ERR_MEM;
	}

	/* bulk rx buffer has a fixed upper bound (ethernet frame), so it stays
	   init-owned. intr rx buffer is sized to the device's INTR ep_mps and is
	   therefore allocated in attach / freed in detach. */
	cdc->bulk_rx_buf = (u8 *)usb_os_malloc(USBH_CDC_ECM_BULK_BUF_MAX_SIZE);
	if (cdc->bulk_rx_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc bulk rx buf fail\n");
		USBH_COMPOSITE_CDC_ECM_FREE_MEM(cdc->bulk_rx_buf);
		USBH_COMPOSITE_CDC_ECM_FREE_MEM(cdc->dongle_ctrl_buf);
		return HAL_ERR_MEM;
	}

	cdc->mac_valid = 0;

	if (priv == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Param error\n");
		USBH_COMPOSITE_CDC_ECM_FREE_MEM(cdc->led_array);
		cdc->led_cnt = 0;
	} else {
		if (priv->mac_value) {
			usbh_composite_cdc_ecm_set_dongle_mac(priv->mac_value);
		}
		if ((priv->led_array != NULL) && (priv->led_cnt > 0)) {
			usbh_composite_cdc_ecm_set_dongle_led_array(priv->led_array, priv->led_cnt);
		}
	}

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "CB init fail\n");
			USBH_COMPOSITE_CDC_ECM_FREE_MEM(cdc->bulk_rx_buf);
			USBH_COMPOSITE_CDC_ECM_FREE_MEM(cdc->led_array);
			USBH_COMPOSITE_CDC_ECM_FREE_MEM(cdc->dongle_ctrl_buf);
			return ret;
		}
	}

#if USBH_COMP_ECM_STATE_DEBUG_ENABLE
	usbh_composite_cdc_ecm_debug_task_init();
#endif

	cdc->cb = cb;

	if (usb_os_sema_create(&(cdc->bulk_tx_sema)) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Sema create fail\n");
		USBH_COMPOSITE_CDC_ECM_FREE_MEM(cdc->bulk_rx_buf);
		USBH_COMPOSITE_CDC_ECM_FREE_MEM(cdc->led_array);
		USBH_COMPOSITE_CDC_ECM_FREE_MEM(cdc->dongle_ctrl_buf);
		cdc->led_cnt = 0;
		return HAL_ERR_MEM;
	}

	return ret;
}

/**
  * @brief  Deinit CDC ECM class
  * @retval Status
  */
int usbh_composite_cdc_ecm_deinit(void)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	cdc->eth_hw_connect = 0;

#if USBH_COMP_ECM_STATE_DEBUG_ENABLE
	usbh_composite_cdc_ecm_debug_task_deinit();
#endif
	usbh_composite_cdc_ecm_tx_status_check();
	usb_os_sema_delete(usbh_composite_cdc_ecm_host.bulk_tx_sema);

	usbh_composite_cdc_ecm_deinit_all_pipe();

	USBH_COMPOSITE_CDC_ECM_FREE_MEM(cdc->intr_rx_buf);
	USBH_COMPOSITE_CDC_ECM_FREE_MEM(cdc->bulk_rx_buf);
	USBH_COMPOSITE_CDC_ECM_FREE_MEM(cdc->led_array);
	USBH_COMPOSITE_CDC_ECM_FREE_MEM(cdc->dongle_ctrl_buf);
	cdc->led_cnt = 0;

	if ((cdc->cb != NULL) && (cdc->cb->deinit != NULL)) {
		cdc->cb->deinit();
	}

	return HAL_OK;
}

/**
  * @brief  choose the right config descriptor index while there has more then one config descriptor
  * @param  host: Host handle
  * @param  cfg_num: config index
  * @retval Status
  */
int usbh_composite_cdc_ecm_choose_config(usb_host_t *host)
{
	//choose ecm cfg_num
	usbh_dev_id_t dev_id_info = {0,};
	dev_id_info.bInterfaceClass = USB_CDC_ECM_CLASS_CODE;
	dev_id_info.bInterfaceSubClass = USB_CDC_ECM_SUBCLASS_ECM;
	dev_id_info.bInterfaceProtocol = 0x00;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	u8 cfg_id = usbh_get_configuration(host, &dev_id_info);
	return usbh_set_configuration(host, cfg_id);
}

int usbh_composite_cdc_ecm_check_config_desc(usb_host_t *host, u8 cfg_max)
{
	UNUSED(cfg_max);
	usbh_itf_data_t *itf;
	u8 ecm_ctrl_valid = 0;
	u8 ecm_data_valid = 0;

	usbh_dev_id_t dev_id_info = {0,};
	dev_id_info.bInterfaceClass = USB_CDC_ECM_CLASS_CODE;
	dev_id_info.bInterfaceSubClass = USB_CDC_ECM_SUBCLASS_ECM;
	dev_id_info.bInterfaceProtocol = 0x00;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf = (usbh_itf_data_t *) usbh_get_interface_descriptor(host, &dev_id_info);

	if (itf) {
		ecm_ctrl_valid = 1;
	}

	dev_id_info.bInterfaceClass = USB_CDC_ECM_DATA_INTERFACE_CLASS_CODE;
	dev_id_info.bInterfaceSubClass = USB_CDC_ECM_SUBCLASS_RESERVED;
	dev_id_info.bInterfaceProtocol = 0x00;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf = (usbh_itf_data_t *) usbh_get_interface_descriptor(host, &dev_id_info);

	if (itf) {
		ecm_data_valid = 1;
	}

	if (ecm_ctrl_valid && ecm_data_valid) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Check success idx max %d\n", cfg_max);
		return HAL_OK;
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Check fail idx max %d[%d-%d]\n",  cfg_max, ecm_ctrl_valid, ecm_data_valid);
	}

	return HAL_ERR_PARA;
}

/**
  * @brief  get the mac str, if the dongle support the standard CDC ECM
  * @retval mac string,the length is 6 Bytes
  */
const u8 *usbh_composite_cdc_ecm_process_mac_str(void)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	u8 i = 0;

	while (!cdc->mac_valid && i < 10) {
		usb_os_sleep_ms(1000);
		i++;
		RTK_LOGS(TAG, RTK_LOG_INFO,  "Wait to get MAC string!\n");
	}
	return cdc->mac;
}

/**
  * @brief  hex to char
  * @retval
  */
u8 usbh_composite_cdc_ecm_hex_to_char(u8 hex_num)
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
  * @brief  ecm device is ready
  * @retval
  */
int usbh_composite_cdc_ecm_get_connect_status(void)//1 up
{
	u8 ret1 = usbh_composite_cdc_ecm_host.eth_hw_connect;
	u8 ret2 = 0;

	if (usbh_composite_cdc_ecm_check_enum_status() == HAL_OK) {
		ret2 = 1;
	}

	return ret1 && ret2;
}

u8 usbh_composite_cdc_ecm_prepare_done(void)
{
	usbh_composite_cdc_ecm_host.ready_to_xfer = 1;
	return HAL_OK;
}

int usbh_composite_cdc_ecm_send_data(u8 *buf, u32 len, u8 block)
{
	u8 retry_cnt = 0;
	int ret;

	UNUSED(block);

#if USBH_CDC_ECM_TX_SPEED_CHECK
	static u64 usb_tx_start_time = 0, usb_tx_end_time, usb_tx_interval_time;
	static u64 usb_tx_total_len = 0;

	usb_tx_total_len += len;
	if (usb_tx_start_time == 0) {
		usb_tx_start_time = usb_os_get_timestamp_ms();
	}
#endif

	while (1) {
		ret = usbh_composite_cdc_ecm_bulk_send(buf, len);
		if (ret == HAL_OK) {
			//success
			break;
		}
		if (++retry_cnt > 100) { //100ms
			RTK_LOGS(TAG, RTK_LOG_ERROR, "TX drop(%d)\n", len);
			ret = HAL_ERR_UNKNOWN;
			break;
		} else {
			usb_os_sleep_ms(1);
		}
	}

	//wait usbh_composite_cdc_ecm_cb_bulk_send to give the sema
	if (ret == HAL_OK) {
		usbh_composite_cdc_ecm_host.bulk_tx_block = 1;
		usb_os_sema_take(usbh_composite_cdc_ecm_host.bulk_tx_sema, USB_OS_SEMA_TIMEOUT);
#if USBH_CDC_ECM_TX_SPEED_CHECK
		usb_tx_end_time = usb_os_get_timestamp_ms();
		usb_tx_interval_time = (usb_tx_end_time - usb_tx_start_time);

		if (usb_tx_interval_time >= 3000) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Heap %d TX %dB in %d ms, %d Kbps\n",
					 rtos_mem_get_free_heap_size(),
					 (u32)usb_tx_total_len, (u32)usb_tx_interval_time, (u32)((usb_tx_total_len * 8 * 1000) / (usb_tx_interval_time * 1024)));
			usb_tx_start_time = usb_tx_end_time;
			usb_tx_total_len = 0;
		}
#endif
	}

	usbh_composite_cdc_ecm_host.bulk_tx_block = 0;

	return ret;
}

__attribute__((weak))
int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbh_composite_cdc_ecm_send_data(buf, len, block);
}
