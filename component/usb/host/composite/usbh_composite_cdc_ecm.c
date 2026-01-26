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
#define ECM_RX_IN_HIGH_TP                   1
#define ECM_ENABLE_DUMP_DESCRIPYOT_PARSE    0

#define USBH_CDC_ECM_BULK_BUF_MAX_SIZE      (512*3)

#define USBH_ECM_RX_SPEED_CHECK             0     /* CDC ECM rx speed test */
#define USBH_ECM_TX_SPEED_CHECK             0     /* CDC ECM tx speed test */

/* for RTL8156BG, use this to allow ping/UDP data transfer */
#define ECM_ENABLE_RCR_CONFIGURATION                            1

/* for RTL8152, config the fifo flow control for data transfer */
#define ECM_ENABLE_FIFO_FLOW_CTRL                               1

#define ECM_ENABLE_PACKETFILTER                                 0

/* tick count */
/* in ms*/
#define USB_BULK_OUT_IDLE_MAX_CNT        8U * 50 //send total 16k data
#define USB_BULK_OUT_BUSY_MAX_CNT        8U * 10
#define USB_BULK_OUT_BUSY_CHECK_CNT      5

#define USB_BULK_IN_IDLE_MAX_CNT         8U * 10  ///idle
#define USB_BULK_IN_NAK_CHECK_MAX_CNT    8U * 5   /// 4*5 busy nak drop
#define USB_BULK_IN_NAK_CHECK_CNT        4

#define USB_INTR_IN_IDLE_MAX_CNT         80U
#define USB_INTR_IN_BUSY_MAX_CNT         10U

/* ecm ethernet connect status check */
#define USBH_ECM_ETH_STATUS_CHECK                               500U  //ms

#define CDC_ECM_MAC_STRING_LEN                                  (32)

/* [CDC]Class-Specific Codes Start -----------------------------------------------------------*/
/* Table 11: Class-Specific Notification Codes for Ethernet subclass */
#define CDC_ECM_NOTIFY_NETWORK_CONNECTION                       0x00U    /**< MAC string length. */
#define CDC_ECM_NOTIFY_CONNECTION_SPEED_CHANGE                  0x2AU    /**< MAC string length. */

/* Table 6: Class-Specific Request Codes for Ethernet subclass */
#define CDC_ECM_SET_ETHERNET_MULTICAST_FILTERS                  0x40U
#define CDC_ECM_SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER    0x41U
#define CDC_ECM_GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER    0x42U
#define CDC_ECM_SET_ETHERNET_PACKET_FILTER                      0x43U
#define CDC_ECM_GET_ETHERNET_STATISTIC                          0x44U
#define CDC_ECM_SPECIAL_REQUEST_CODE_RESERVED                   0x45U

/*  bDescriptor SubType in Communications Class Functional Descriptors  */
#define CDC_ECM_NETWORK_INTERFACE                               0x24
#define CDC_ECM_NETWORK_FUNC_DESCRIPTOR                         0x0FU
/* [CDC]Class-Specific Codes End -----------------------------------------------------------*/

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

#if ECM_ENABLE_PACKETFILTER
	CDC_ECM_STATE_CTRL_SET_ETHERNET_MULTICAST_FILTER,
	CDC_ECM_STATE_CTRL_SET_ETHERNET_PACKET_FILTER,
	CDC_ECM_STATE_CTRL_GET_ETHERNET_STATISTIC,
#endif

	CDC_ECM_STATE_CTRL_RCR_GET,  /* RTL8156 */
	CDC_ECM_STATE_CTRL_RCR_SET,
	CDC_ECM_STATE_FLOW_CTRL1,    /* RTL8152 */
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

/* CDC ECM Ethernet Packet Filter Bitmap  */
typedef enum {
	CDC_ECM_ETH_PACKET_TYPE_PROMISCUOUS     = 0x01U, /* ALL frames received by the networking device are forwarded up to the host (required) */
	CDC_ECM_ETH_PACKET_TYPE_ALL_MULTICAST   = 0x02U, /* ALL multicast frames received by the networking device are forwarded up to the host, not just
                                                        the ones enumerated in the device's multicast address list (required) */
	CDC_ECM_ETH_PACKET_TYPE_DIRECTED        = 0x04U, /* Directed packets received containing a destination address equal to the MAC address of the
                                                        networking device are forwarded up to the host (required) */
	CDC_ECM_ETH_PACKET_TYPE_BROADCAST       = 0x08U, /* All broadcast packets received by the networking device are forwarded up to the host. */
	CDC_ECM_ETH_PACKET_TYPE_MULTICAST       = 0x10U, /* All multicast packets enumerated in the device's multicast address list are forwarded up to the host */
	CDC_ECM_ETH_PACKET_TYPE_MAX,
} usbh_composite_cdc_ecm_eth_packet_filter_bitmap_t;

/* Private types -------------------------------------------------------------*/

/*
	Ethernet Networking Functional Descriptor
	CDC ECM basic struction
*/
typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u8  bDescriptorSubtype;                              /* Ethernet Networking functional descriptor subtype */
	u8  iMACAddress;                                     /* Index of string descriptor */
	u32 bmEthernetStatistics;                            /* Indicates which Ethernet statistics functions the device collects. */
	u8  wMaxSegmentSize;                                 /* The maximum segment size that the Ethernet device is capable of supporting */
	u16 wNumberMCFilters;                                /* Contains the number of multicast filters that can be configured by the host */
	u8  bNumberPowerFilters;                             /* Contains the number of pattern filters that are available for causing wake-up of the host. */
} usbh_composite_cdc_ecm_network_func_t ;

/* ConnectionSpeedChange Data Structure */
typedef union {
	u32 DLBitRate;      /* Contains the downlink bit rate, in bits per second, as sent on the IN pipe */
	u32 ULBitRate;    /* Contains the uplink bit rate, in bits per second, as sent on the OUT pipe */
} usbh_composite_cdc_ecm_speed_change_t;

/* Private macros ------------------------------------------------------------*/
#define USBH_COMPOSITE_ECM_FREE_MEM(x)  if(x){ usb_os_mfree(x); x = NULL;}

/* Private function prototypes -----------------------------------------------*/
static int usbh_composite_cdc_ecm_attach(usb_host_t *host);
static int usbh_composite_cdc_ecm_detach(usb_host_t *host);
static int usbh_composite_cdc_ecm_process(usb_host_t *host, u32 msg);
static int usbh_composite_cdc_ecm_cb_sof(usb_host_t *host);
static int usbh_composite_cdc_ecm_setup(usb_host_t *host);
static int usbh_composite_cdc_ecm_process_bulk_out(usb_host_t *host);
static int usbh_composite_cdc_ecm_process_bulk_in(usb_host_t *host);
static int usbh_composite_cdc_ecm_process_intr_in(usb_host_t *host);
static int usbh_composite_cdc_ecm_bulk_receive(void);
static int usbh_composite_cdc_ecm_bulk_tx(void);
static int usbh_composite_cdc_ecm_intr_receive(void);
static int usbh_composite_cdc_ecm_parse_ctrl(usbh_itf_data_t *itf);
static int usbh_composite_cdc_ecm_parse_data(usbh_itf_data_t *itf);
static int usbh_composite_cdc_ecm_bulk_tx_time_check(void);
static int usbh_composite_cdc_ecm_bulk_rx_time_check(void);
static int usbh_composite_cdc_ecm_intr_rx_time_check(void);
#if ECM_STATE_DEBUG_ENABLE
static void usbh_composite_cdc_ecm_debug_task_init(void);
static void usbh_composite_cdc_ecm_debug_task_deinit(void);
#endif
/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "ECMH";

/* USB Standard Device Descriptor */
const usbh_class_driver_t usbh_composite_cdc_ecm_driver = {
	.attach  = usbh_composite_cdc_ecm_attach,
	.detach  = usbh_composite_cdc_ecm_detach,
	.setup   = usbh_composite_cdc_ecm_setup,
	.process = usbh_composite_cdc_ecm_process,
	.sof = usbh_composite_cdc_ecm_cb_sof,
};

#if ECM_STATE_DEBUG_ENABLE
rtos_task_t                   state_check_task;
u32                           state_check_task_alive;
#endif
static usbh_composite_cdc_ecm_host_t usbh_composite_cdc_ecm_host;

/* Private functions ---------------------------------------------------------*/
static int usbh_composite_cdc_ecm_tx_status_check(void)
{
	if (usbh_composite_cdc_ecm_host.cdc_ecm_tx_block) {
		//release the sema to return usbh_composite_cdc_ecm_send_data
		do {
			usb_os_sema_give(usbh_composite_cdc_ecm_host.cdc_ecm_tx_sema);
			usb_os_sleep_ms(1U);
		} while (usbh_composite_cdc_ecm_host.cdc_ecm_tx_block);
	}

	return HAL_OK;
}

static int usbh_composite_cdc_ecm_cb_bulk_send(usbh_urb_state_t state)
{
	usb_os_sema_give(usbh_composite_cdc_ecm_host.cdc_ecm_tx_sema);
	if (state != USBH_URB_DONE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail %d\n", state);
	}

	return HAL_OK;
}

static int usbh_composite_cdc_ecm_cb_bulk_receive(u8 *pbuf, u32 len)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
#if USBH_ECM_RX_SPEED_CHECK
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
	if (buf && length >= 8) {
		// RTK_LOGS(TAG, RTK_LOG_INFO, "Data(%02x %02x %02x %02x )\n",buf[0],buf[1],buf[2],buf[3]);
		/*A1 00 00 00 01 00 00 00 */
		if (length == 8 && buf[0] == 0xA1 && buf[1] == CDC_ECM_NOTIFY_NETWORK_CONNECTION) {
			usbh_composite_cdc_ecm_host.eth_hw_connect = buf[2];
		}
		/*A1 2A 00 00 01 00 00 08 	00 00 00 00 00 00 00 00 */
		else if (length == 16 && buf[0] == 0xA1 && buf[1] == CDC_ECM_NOTIFY_CONNECTION_SPEED_CHANGE) {
			//usbh_composite_cdc_ecm_speed_change_t
		}
	}

	return HAL_OK;
}

static void usbh_composite_cdc_ecm_dump_ecm_cfgdesc(void)
{
#if ECM_ENABLE_DUMP_DESCRIPYOT_PARSE
	usbh_composite_cdc_ecm_host_t   *ecm = &usbh_composite_cdc_ecm_host;
	usbh_ep_desc_t        *ep_desc;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info;

	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------Dump Descriptor Start------------------\n");

	//dump ecm control
	pipe_info = &(ecm->ecm_ctrl_ep);
	ep_desc = &(pipe_info->ep_desc);
	RTK_LOGS(TAG, RTK_LOG_INFO, "INTR addr(0x%x)MPS(%d)pipe(%d)interval(%d) MAC(%d)\n",
			 ep_desc->bEndpointAddress, ep_desc->wMaxPacketSize, pipe_info->pipe.pipe_num, pipe_info->pipe.ep_interval, ecm->mac_str_id);

	//dump ecm data
	pipe_info = &(ecm->ecm_tx_ep);
	ep_desc = &(pipe_info->ep_desc);
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK out:addr(0x%x)MPS(%d)pipe(%d)interval(%d)\n",
			 ep_desc->bEndpointAddress, ep_desc->wMaxPacketSize, pipe_info->pipe.pipe_num, pipe_info->pipe.ep_interval);

	pipe_info = &(ecm->ecm_rx_ep);
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
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;

	u16 vid = cdc->vid;
	u16 pid = cdc->pid;

	if ((cdc->mac_src_type == CDC_ECM_MAC_UPPER_LAYER_SET) && (vid == 0x0BDA) && ((pid == 0x8152) || (pid == 0x8153))) {
		status = HAL_OK;
		RTK_LOGS(TAG, RTK_LOG_INFO,  "Upper set mac[%02x %02x %02x %02x %02x %02x]\n", cdc->mac[0], cdc->mac[1], cdc->mac[2], cdc->mac[3], cdc->mac[4],
				 cdc->mac[5]);
	} else {
		status = usbh_composite_cdc_ecm_get_string(host, cdc->dongle_ctrl_buf, CDC_ECM_MAC_STRING_LEN, cdc->mac_str_id);
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
static int usbh_composite_cdc_ecm_process_set_alt(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_REQ_SET_INTERFACE;
	setup.req.wValue = cdc->alt_set;
	setup.req.wIndex = cdc->if_num;
	setup.req.wLength = 0U;

	return usbh_ctrl_request(host, &setup, NULL);
}

#if ECM_ENABLE_PACKETFILTER
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
	setup.req.bRequest = CDC_ECM_SET_ETHERNET_PACKET_FILTER;
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
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = CDC_ECM_SET_ETHERNET_MULTICAST_FILTERS;
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
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = CDC_ECM_GET_ETHERNET_STATISTIC;
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
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;

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
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;

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
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;

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
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;

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
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;

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
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;

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
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;

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
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;

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
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;

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
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xDD90;
	setup.req.wIndex = 0x0103;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

static void usbh_composite_cdc_ecm_set_dongle_mac(u8 *mac)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;

	if (NULL == mac) {
		RTK_LOGE(TAG, "Param error,mac is NULL\n");
		return ;
	}

	memcpy((void *) & (cdc->mac[0]), (void *)mac, 6);
	cdc->mac_src_type = CDC_ECM_MAC_UPPER_LAYER_SET;
}

static void usbh_composite_cdc_ecm_set_dongle_led_array(u16 *led, u8 len)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;

	if (led == NULL || len == 0) {
		RTK_LOGE(TAG, "Param error,led is NULL or len %ld\n", (u32)len);
		return ;
	}

	USBH_COMPOSITE_ECM_FREE_MEM(cdc->led_array);

	cdc->led_array = (u16 *)usb_os_malloc(len * sizeof(u16));
	memcpy((void *)cdc->led_array, (void *)led, len * sizeof(u16));

	cdc->led_cnt = len;
}

static void usbh_composite_cdc_ecm_config_dongle_mac(usb_host_t *host, u16 vid, u16 pid)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;;
	u8 state = HAL_OK;

	// RTK_LOGS(TAG, RTK_LOG_INFO, "mac issue status[%d]V(0x%04x)P(0x%04x) type %d\n",appx_cmd.sub_status,vid,pid,cdc->mac_src_type);

	switch (cdc->sub_status) {
	case CDC_ECM_STATE_CTRL_MAC_GET_LOCK: //8152 mac
		if ((pid != 0x8152) || (vid != 0x0BDA) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY)) {
			cdc->sub_status++;
		} else {
			usb_os_memset(cdc->mac_ctrl_lock, 0, 4);
			state = usbh_composite_cdc_ecm_process_mac_get_lock(host);
			if (state == HAL_OK) {
				usb_os_memcpy(cdc->mac_ctrl_lock, cdc->dongle_ctrl_buf, CDC_ECM_MAC_CTRL_REG_LEN);
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get MAC lock err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_DISABLE_LOCK: //8152 mac
		if ((pid != 0x8152) || (vid != 0x0BDA) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY)) {
			cdc->sub_status++;
		} else {
			cdc->mac_ctrl_lock[0] = 0xD0;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->mac_ctrl_lock, CDC_ECM_MAC_CTRL_REG_LEN);
			state = usbh_composite_cdc_ecm_process_mac_set_dis_lock(host);
			if (state == HAL_OK) {
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Dis MAC lock err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_SET_MAC1: //8152 mac
		if ((pid != 0x8152) || (vid != 0x0BDA) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY)) {
			cdc->sub_status++;
		} else {
			usb_os_memcpy(cdc->dongle_ctrl_buf, &(cdc->mac[0]), CDC_ECM_MAC_CTRL_REG_LEN);
			state = usbh_composite_cdc_ecm_process_mac_set_mac1(host);
			if (state == HAL_OK) {
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set MAC1 err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_SET_MAC2: //8152 mac
		if ((pid != 0x8152) || (vid != 0x0BDA) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY)) {
			cdc->sub_status++;
		} else {
			usb_os_memcpy(cdc->dongle_ctrl_buf, &(cdc->mac[4]), CDC_ECM_MAC_CTRL_REG_LEN);
			cdc->dongle_ctrl_buf[2] = cdc->dongle_ctrl_buf[3] = 0xFF;
			state = usbh_composite_cdc_ecm_process_mac_set_mac2(host);
			if (state == HAL_OK) {
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set MAC2 err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_ENABLE_LOCK: //8152 mac
		if ((pid != 0x8152) || (vid != 0x0BDA) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY)) {
			cdc->sub_status++;
		} else {
			cdc->mac_ctrl_lock[0] = 0x10;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->mac_ctrl_lock, CDC_ECM_MAC_CTRL_REG_LEN);
			state = usbh_composite_cdc_ecm_process_mac_en_lock(host);
			if (state == HAL_OK) {
				cdc->sub_status++;
				cdc->mac_valid = 1;
				RTK_LOGS(TAG, RTK_LOG_INFO, "Mac set success \n");
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "En MAC lock err\n");
				usb_os_sleep_ms(100);
			}
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

#if ECM_ENABLE_PACKETFILTER
	u8 mac_str[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif

	switch (cdc->sub_status) {
	case CDC_ECM_STATE_AT_SETTING_IDLE:
		break;

	case CDC_ECM_STATE_GET_MAC_STR:
		if (pid != EF_DONGLE_PID_EG91) {
			state = usbh_composite_cdc_ecm_get_mac_str(host);
			if (state == HAL_OK) {
				cdc->sub_status ++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Get MAC fail error[%d]\n", state);
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
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM alt setting err\n");
			usb_os_sleep_ms(100);
		}
		break;

#if ECM_ENABLE_PACKETFILTER
	case CDC_ECM_STATE_CTRL_SET_ETHERNET_MULTICAST_FILTER:
		usb_os_memcpy(cdc->muticast_filter, mac_str, 6);
		cdc->muticast_filter_len = 6;
		state = usbh_composite_cdc_ecm_process_set_muiticast_filter(host);
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
		state = usbh_composite_cdc_ecm_process_set_packet_filter(host);
		if (state == HAL_OK) {
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM set eth packet filter err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_GET_ETHERNET_STATISTIC:
		state = usbh_composite_cdc_ecm_process_get_statistic(host);
		if (state == HAL_OK) {
			usb_os_memcpy(&(cdc->eth_statistic_count), cdc->dongle_ctrl_buf, CDC_ECM_MAC_CTRL_REG_LEN);
			cdc->sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM get eth statistic err\n");
			usb_os_sleep_ms(100);
		}
		break;
#endif

///just for 8156
	case CDC_ECM_STATE_CTRL_RCR_GET:
		if ((pid != 0x8156) || (vid != 0x0BDA)) {
			cdc->sub_status++;
		} else {
			usb_os_memset(cdc->rcr, 0, 4);
			state = usbh_composite_cdc_ecm_process_get_rcr(host);
			if (state == HAL_OK) {
				usb_os_memcpy(cdc->rcr, cdc->dongle_ctrl_buf, CDC_ECM_MAC_CTRL_REG_LEN);
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get RCR err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_RCR_SET:
		if ((pid != 0x8156) || (vid != 0x0BDA)) {
			cdc->sub_status++;
		} else {
			//set bit 0~3 set 1 will enable PING & UDP transfer
			cdc->rcr[0] = cdc->rcr[0] | 0x0F;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->rcr, CDC_ECM_MAC_CTRL_REG_LEN);
			state = usbh_composite_cdc_ecm_process_set_rcr(host);
			if (state == HAL_OK) {
				cdc->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set RCR err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	///just for 8152
	case CDC_ECM_STATE_FLOW_CTRL1:
		if ((pid != 0x8152) || (vid != 0x0BDA)) {
			cdc->sub_status++;
		} else {
			usb_os_memset(cdc->flow_ctrl, 0, 4);
			cdc->flow_ctrl[0] = 0x60;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->flow_ctrl, CDC_ECM_MAC_CTRL_REG_LEN);
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
		if ((pid != 0x8152) || (vid != 0x0BDA)) {
			cdc->sub_status++;
		} else {
			usb_os_memset(cdc->flow_ctrl, 0, 4);
			cdc->flow_ctrl[0] = 0xa0;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->flow_ctrl, CDC_ECM_MAC_CTRL_REG_LEN);
			state = usbh_composite_cdc_ecm_process_set_flow_ctrl2(host);
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
		usbh_composite_cdc_ecm_config_dongle_mac(host, vid, pid);
		break;

	case CDC_ECM_STATE_CTRL_LED_COLOR_SET: //8152 led ctrl
		if ((pid != 0x8152) || (vid != 0x0BDA) || (cdc->led_cnt == 0) || (cdc->led_array == NULL)) {
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

static void usbh_composite_cdc_ecm_deinit_ep_buf(void)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	USBH_COMPOSITE_ECM_FREE_MEM(cdc->ecm_rx_ep.buf);
	USBH_COMPOSITE_ECM_FREE_MEM(cdc->ecm_ctrl_ep.buf);
}

static void usbh_composite_cdc_ecm_deinit_all_pipe(void)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usb_host_t *host = cdc->driver->host;

	if (cdc->ecm_ctrl_ep.pipe.pipe_num) {
		usbh_close_pipe(host, &(cdc->ecm_ctrl_ep.pipe));
	}

	if (cdc->ecm_rx_ep.pipe.pipe_num) {
		usbh_close_pipe(host, &(cdc->ecm_rx_ep.pipe));
	}

	if (cdc->ecm_tx_ep.pipe.pipe_num) {
		usbh_close_pipe(host, &(cdc->ecm_tx_ep.pipe));
	}
}

/**
  * @brief  Parse audio streaming interface
  * @param  pbuf: given descriptor buffer
  * @param  length: length of parse desc length
  * @param  buf_length: length of given buffer
  * @retval Status
  */
static int usbh_composite_cdc_ecm_parse_ctrl(usbh_itf_data_t *itf_data)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *ctrl_ep = &(cdc->ecm_ctrl_ep);
	usbh_ep_desc_t *ep_desc;
	u16 itf_total_len = 0;
	u8 *desc;
	u8 len;

	desc = itf_data->raw_data;

	if (((usbh_itf_desc_t *)desc)->bNumEndpoints != 1) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Ctrl ep is %d, return\n", ((usbh_itf_desc_t *)desc)->bNumEndpoints);
		return HAL_ERR_PARA;
	}

	len = ((usbh_desc_header_t *) desc)->bLength;
	desc = desc + len;
	itf_total_len += len;

	while (1) {
		if (desc == NULL || itf_total_len >= itf_data->raw_data_len) {
			break;
		}

		switch (((usbh_desc_header_t *) desc)->bDescriptorType) {
		case USB_DESC_TYPE_INTERFACE:
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Ctrl intf new %d, return\n", ((usbh_itf_desc_t *)desc)->bInterfaceNumber);
			return HAL_OK;

		case CDC_ECM_NETWORK_INTERFACE:
			if ((((usbh_composite_cdc_ecm_network_func_t *) desc)->bDescriptorSubtype) == CDC_ECM_NETWORK_FUNC_DESCRIPTOR) {
				cdc->mac_str_id = ((usbh_composite_cdc_ecm_network_func_t *)desc)->iMACAddress;
				RTK_LOGS(TAG, RTK_LOG_INFO,  "Mac string id(%d)\n", cdc->mac_str_id);
				cdc->sub_status = CDC_ECM_STATE_GET_MAC_STR;
			}
			len = ((usbh_desc_header_t *) desc)->bLength;
			desc = desc + len;
			break;

		case USB_DESC_TYPE_ENDPOINT:
			ep_desc = (usbh_ep_desc_t *)desc;
			usb_os_memcpy(&(ctrl_ep->ep_desc), ep_desc, sizeof(usbh_ep_desc_t));
			ctrl_ep->valid = 1;

			len = ((usbh_desc_header_t *) desc)->bLength;
			desc = desc + len;
			break;

		default:
			len = ((usbh_desc_header_t *) desc)->bLength;
			desc = desc + len;
			break;
		}

		itf_total_len += len;
	}

	return HAL_OK;
}

/**
  * @brief  Parse audio streaming interface
  * @param  pbuf: given descriptor buffer
  * @retval Status
  */
static int usbh_composite_cdc_ecm_parse_data(usbh_itf_data_t *itf_data)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_ep_desc_t *ep_desc;
	u8 *desc = itf_data->raw_data;
	u16 itf_total_len = 0;
	u16 len = 0;
	u8 itf_num;

	itf_num = ((usbh_itf_desc_t *)desc)->bInterfaceNumber;
	len = ((usbh_desc_header_t *) desc)->bLength;
	desc = desc + len;
	itf_total_len += len;

	while (1) {
		if (desc == NULL || itf_total_len >= itf_data->raw_data_len) {
			break;
		}

		switch (((usbh_desc_header_t *) desc)->bDescriptorType) {
		case USB_DESC_TYPE_INTERFACE:
			if (((usbh_itf_desc_t *)desc)->bInterfaceNumber != itf_num) {
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "AS intf new %d:old %d, return\n", ((usbh_itf_desc_t *)desc)->bInterfaceNumber, itf_num);
				return HAL_OK;
			}

			cdc->if_num = desc[2];
			cdc->alt_set = desc[3];
			len = ((usbh_desc_header_t *) desc)->bLength;
			desc = desc + len;
			break;

		case USB_DESC_TYPE_ENDPOINT: {
			ep_desc = (usbh_ep_desc_t *)desc;
			if (USB_EP_IS_IN(ep_desc->bEndpointAddress)) { //in
				usb_os_memcpy(&(cdc->ecm_rx_ep.ep_desc), ep_desc, sizeof(usbh_ep_desc_t));
				cdc->ecm_rx_ep.valid = 1;
			} else {  //out
				usb_os_memcpy(&(cdc->ecm_tx_ep.ep_desc), ep_desc, sizeof(usbh_ep_desc_t));
				cdc->ecm_tx_ep.valid = 1;
			}

			len = ((usbh_desc_header_t *) desc)->bLength;
			desc = desc + len;
		}
		break;

		default: {
			len = ((usbh_desc_header_t *) desc)->bLength;
			desc = desc + len;
		}
		break;
		}

		itf_total_len += len;
	}

	return HAL_OK;
}

static int usbh_composite_cdc_ecm_parse_interface_desc(usb_host_t *host)
{
	usbh_composite_cdc_ecm_host_t *ecm = &usbh_composite_cdc_ecm_host;
	usbh_dev_desc_t *pdesc;
	u8 ret = HAL_ERR_UNKNOWN;

	pdesc = ecm->driver->host->dev_desc;
	ecm->vid = pdesc->idVendor;
	ecm->pid = pdesc->idProduct;

	/* Get interface index as per supported class & protocol */
	/*
		cdc ecm ctrl
	*/
	usbh_dev_id_t dev_id_info = {0,};
	dev_id_info.bInterfaceClass = CDC_IF_CDC_CTRL_CODE;
	dev_id_info.bInterfaceSubClass = CDC_IF_CDC_CTRL_SUB_CLASS_ECM_CODE;
	dev_id_info.bInterfaceProtocol = 0x00;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	usbh_itf_data_t *itf = (usbh_itf_data_t *) usbh_get_interface_descriptor(host, &dev_id_info);

	if (itf) {
		ret = usbh_composite_cdc_ecm_parse_ctrl(itf);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Ctrl parse fail\n");
			return ret;
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get if fail\n");
	}

	dev_id_info.bInterfaceClass = CDC_IF_CDC_DATA_CODE;
	dev_id_info.bInterfaceSubClass = CDC_IF_CDC_DATA_SUB_CLASS_DATA_CODE;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS;
	itf = (usbh_itf_data_t *) usbh_get_interface_descriptor(host, &dev_id_info);
	while (itf) {
		if (itf->itf_desc_array[0].bAlternateSetting == 0) { //setting 0
			ret = usbh_composite_cdc_ecm_parse_data(itf);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Data parse fail\n");
				return ret;
			}
		}
		itf = itf->next;
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
	int status = HAL_ERR_UNKNOWN;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info;
	usbh_composite_cdc_ecm_host_t *ecm = &usbh_composite_cdc_ecm_host;

	status = usbh_composite_cdc_ecm_parse_interface_desc(host);
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse ECM desc fail\n");
		return status;
	}

	//control in
	pipe_info = &(ecm->ecm_ctrl_ep);
	if (pipe_info->valid) {
		usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc));
		pipe_info->buf = (u8 *)usb_os_malloc(pipe_info->pipe.ep_mps);
		pipe_info->buf_len = pipe_info->pipe.ep_mps;
		pipe_info->pipe.xfer_state = USBH_EP_XFER_START;
	}

	//bulk out
	pipe_info = &(ecm->ecm_tx_ep);
	if (pipe_info->valid) {
		usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc));
	}

	//bulk in
	pipe_info = &(ecm->ecm_rx_ep);
	if (pipe_info->valid) {
		usbh_open_pipe(host, &(pipe_info->pipe), &(pipe_info->ep_desc));
		/* ecm use bulk, the max ethernet packet size is 1542, malloc (512*3) to rx a whole ethernet packet*/
		pipe_info->buf = (u8 *)usb_os_malloc(USBH_CDC_ECM_BULK_BUF_MAX_SIZE);
		pipe_info->buf_len = USBH_CDC_ECM_BULK_BUF_MAX_SIZE;
		pipe_info->pipe.xfer_state = USBH_EP_XFER_START;
	}

	usbh_composite_cdc_ecm_dump_ecm_cfgdesc();

	ecm->state = CDC_ECM_STATE_IDLE;

	if ((ecm->cb != NULL) && (ecm->cb->attach != NULL)) {
		ecm->cb->attach();
	}

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Attach done\n");

	return HAL_OK;
}


/**
  * @brief  Usb Detach callback function.
  * @param  host: Host handle
  * @retval HAL_OK
  */
static int usbh_composite_cdc_ecm_detach(usb_host_t *host)
{
	UNUSED(host);
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	cdc->state = CDC_ECM_STATE_IDLE;
	cdc->usb_setup = 0;

	usbh_composite_cdc_ecm_deinit_all_pipe();

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
	cdc->usb_setup = 1;
	usbh_notify_class_state_change(host, 0);

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Setup up done\n");

	return HAL_OK;
}


/**
  * @brief  Usb State Machine handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_cdc_ecm_process(usb_host_t *host, u32 param)
{
	u8 req_status = HAL_OK;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_event_t event;
	event.d32 = param;
	u8 msg = event.msg.pipe_num;


	switch (cdc->state) {
	case CDC_ECM_STATE_TRANSFER:
		if (msg == cdc->ecm_tx_ep.pipe.pipe_num) {
			cdc->next_xfer = 0;
			usbh_composite_cdc_ecm_process_bulk_out(host);
			if (cdc->next_xfer) {
				usbh_notify_class_state_change(host, cdc->ecm_tx_ep.pipe.pipe_num);
			}
		} else if (msg == cdc->ecm_rx_ep.pipe.pipe_num) {
			cdc->next_xfer = 0;
			usbh_composite_cdc_ecm_process_bulk_in(host);
			if (cdc->next_xfer) {
				usbh_notify_class_state_change(host, cdc->ecm_rx_ep.pipe.pipe_num);
			}
		} else if (msg == cdc->ecm_ctrl_ep.pipe.pipe_num) {
			cdc->next_xfer = 0;
			usbh_composite_cdc_ecm_process_intr_in(host);
			if (cdc->next_xfer) {
				usbh_notify_class_state_change(host, cdc->ecm_ctrl_ep.pipe.pipe_num);
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

static int usbh_composite_cdc_ecm_cb_sof(usb_host_t *host)
{
	UNUSED(host);

	if (usbh_composite_cdc_ecm_host.ready_to_xfer) {
		usbh_composite_cdc_ecm_intr_receive();
		//if intr in rx connect, slow to release CPU

		if (usbh_composite_cdc_ecm_host.eth_hw_connect) {
			usbh_composite_cdc_ecm_bulk_receive();
			usbh_composite_cdc_ecm_bulk_tx();
		}
	}

	return HAL_OK;
}

/**
  * @brief  Bulk Out handling function
  * @param  host: Host handle
  * @retval Status
*/
static int usbh_composite_cdc_ecm_process_bulk_out(usb_host_t *host)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info = &(cdc->ecm_tx_ep);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usbh_urb_state_t urb_state = USBH_URB_IDLE;

#if ECM_STATE_DEBUG_ENABLE
	pipe_info->trigger_cnt++;
#endif

	// RTK_LOGS(TAG, RTK_LOG_INFO, "ECM Bulk out %d-%d\n", ep->pipe_num, ep->xfer_state);
	switch (ep->xfer_state) {
	case USBH_EP_XFER_START:
		ep->xfer_buf = pipe_info->buf;
		ep->xfer_len = pipe_info->buf_len;
		ep->xfer_state = USBH_EP_XFER_BUSY;
		usbh_transfer_data(host, ep);

		ep->tick = usbh_get_tick(host);
		pipe_info->busy_tick = ep->tick;
		cdc->next_xfer = 1;
		break;

	case USBH_EP_XFER_BUSY:
		urb_state = usbh_get_urb_state(host, ep);
		switch (urb_state) {
		case USBH_URB_DONE:
			if (ep->trx_zlp) { //ZLP
				ep->trx_zlp = 0U;
				ep->xfer_len = 0U;
				ep->xfer_buf = NULL;
				ep->xfer_state = USBH_EP_XFER_START;
				cdc->next_xfer = 1;
			} else {
				ep->xfer_state = USBH_EP_XFER_IDLE;
				usbh_composite_cdc_ecm_cb_bulk_send(urb_state);
			}
			break;

		case USBH_URB_BUSY:
			if (usbh_get_elapsed_ticks(host, pipe_info->busy_tick) >= USB_BULK_OUT_BUSY_MAX_CNT) { ///timeout 5ms
				if (usbh_check_nak_timeout(host, ep->pipe_num, USB_BULK_OUT_BUSY_CHECK_CNT) == HAL_OK) { // loong time not get the nak
					RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK out 1\n");
					usbh_prepare_retransfer(host, ep->pipe_num);
					ep->xfer_state = USBH_EP_XFER_START;
					cdc->next_xfer = 1;
				} else {
					usbh_increase_busy_cnt(host, ep->pipe_num, 1);
					usbh_enable_nak_interrupt(host, ep->pipe_num);
					pipe_info->busy_tick = usbh_get_tick(host);
				}
			}
			break;

		case USBH_URB_ERROR:
			// RTK_LOGD(TAG, "BULK out restart\n");
			ep->xfer_state = USBH_EP_XFER_START;
			cdc->next_xfer = 1;
			break;

		case USBH_URB_STALL:
			ep->xfer_state = USBH_EP_XFER_IDLE;
			usbh_composite_cdc_ecm_cb_bulk_send(urb_state);
			//RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK out failed %d\n", urb_state);
			break;

		case USBH_URB_IDLE:
			if (usbh_get_elapsed_ticks(host, ep->tick) >= (USB_BULK_OUT_IDLE_MAX_CNT)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK out 2\n");
				usbh_prepare_retransfer(host, ep->pipe_num);
				ep->xfer_state = USBH_EP_XFER_START;
				cdc->next_xfer = 1;
			}
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}

	return HAL_OK;
}


/**
  * @brief  Bulk In handling  function
  * @param  host: Host handle
  * @retval Status
*/
static void usbh_composite_cdc_ecm_process_bulk_in_req(usb_host_t *host, usbh_composite_cdc_ecm_pipe_info_t *pipe_info)
{
	usbh_pipe_t *ep = &(pipe_info->pipe);
	ep->xfer_buf = pipe_info->buf;
	ep->xfer_len = pipe_info->buf_len;
	u32 tick = 0;

	usbh_transfer_data(host, ep);
	ep->xfer_state = USBH_EP_XFER_BUSY;
	tick = usbh_get_tick(host);
	ep->tick = tick;
	pipe_info->busy_tick = tick;
}

static int usbh_composite_cdc_ecm_process_bulk_in(usb_host_t *host)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info = &(cdc->ecm_rx_ep);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

#if ECM_STATE_DEBUG_ENABLE
	pipe_info->trigger_cnt++;
#endif

	switch (ep->xfer_state) {
	case USBH_EP_XFER_START:
		usbh_composite_cdc_ecm_process_bulk_in_req(host, pipe_info);
		break;

	case USBH_EP_XFER_BUSY:
		urb_state = usbh_get_urb_state(host, ep);
		switch (urb_state) {
		case USBH_URB_DONE: { //done
			len = usbh_get_last_transfer_size(host, ep);
			usbh_composite_cdc_ecm_cb_bulk_receive(ep->xfer_buf, len);
#if ECM_RX_IN_HIGH_TP
			usbh_composite_cdc_ecm_process_bulk_in_req(host, pipe_info);
#else
			ep->xfer_state = USBH_EP_XFER_IDLE;
			cdc->next_xfer = 1;
#endif
		}
		break;

		case USBH_URB_BUSY:
			if (usbh_get_elapsed_ticks(host, pipe_info->busy_tick) >= USB_BULK_IN_NAK_CHECK_MAX_CNT) {
				if (usbh_check_nak_timeout(host, ep->pipe_num, USB_BULK_IN_NAK_CHECK_CNT) == HAL_OK) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK in 1\n");
					usbh_prepare_retransfer(host, ep->pipe_num);
					ep->xfer_state = USBH_EP_XFER_START;
					cdc->next_xfer = 1;
				} else {
					usbh_increase_busy_cnt(host, ep->pipe_num, 1);
					usbh_enable_nak_interrupt(host, ep->pipe_num);
					pipe_info->busy_tick = usbh_get_tick(host);
				}
			}
			break;

		case USBH_URB_ERROR: {
			ep->xfer_state = USBH_EP_XFER_START;
			cdc->next_xfer = 1;

			pipe_info->busy_tick = usbh_get_tick(host);;
			ep->tick = usbh_get_tick(host);
		}
		break;

		case USBH_URB_STALL: {
#if ECM_RX_IN_HIGH_TP
			usbh_composite_cdc_ecm_process_bulk_in_req(host, pipe_info);
#else
			ep->xfer_state = USBH_EP_XFER_IDLE;
#endif
#if ECM_STATE_DEBUG_ENABLE
			RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK in failed %d\n", urb_state);
#endif
		}
		break;

		case USBH_URB_IDLE: {
			if (usbh_get_elapsed_ticks(host, ep->tick) >= (USB_BULK_IN_IDLE_MAX_CNT)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK out 2\n");
				usbh_prepare_retransfer(host, ep->pipe_num);
				usbh_composite_cdc_ecm_process_bulk_in_req(host, pipe_info);
			}
		}
		break;

		default:
			break;
		}
		break;

	default:
		cdc->next_xfer = 1;
		ep->xfer_state = USBH_EP_XFER_START;
		break;
	}

	return HAL_OK;
}
/**
  * @brief  Intr in function
  * @param  host: Host handle
  * @retval Status
*/
static int usbh_composite_cdc_ecm_process_intr_in(usb_host_t *host)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info = &(cdc->ecm_ctrl_ep);
	usbh_pipe_t *intr_in = &(pipe_info->pipe);
	u32 len;

#if ECM_STATE_DEBUG_ENABLE
	pipe_info->trigger_cnt++;
#endif
	intr_in->xfer_buf = pipe_info->buf;
	intr_in->xfer_len = pipe_info->buf_len;
	int status = usbh_transfer_process(host, intr_in);
	if ((status == HAL_OK) && (intr_in->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, intr_in);
		usbh_composite_cdc_ecm_cb_intr_receive(intr_in->xfer_buf, len);
	} else if ((intr_in->xfer_state == USBH_EP_XFER_START)) {
		usbh_notify_class_state_change(host, intr_in->pipe_num);
	} else if (intr_in->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX fail: %d\n", usbh_get_urb_state(host, intr_in));
	}

	return HAL_OK;
}

/**
  * @brief Check ecm bulk rx status and tiemout, whether should trigger
  * @retval Status
  */
static int usbh_composite_cdc_ecm_bulk_rx_time_check(void)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info = &(cdc->ecm_rx_ep);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->driver->host;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, ep);

	if ((USBH_URB_IDLE == urb_state) && (usbh_get_elapsed_ticks(host, ep->tick) >= (USB_BULK_IN_IDLE_MAX_CNT))) {
		return HAL_OK;
	}

	if ((USBH_URB_BUSY == urb_state) && (usbh_get_elapsed_ticks(host, pipe_info->busy_tick) >= (USB_BULK_IN_NAK_CHECK_MAX_CNT))) {
		return HAL_OK;
	}

	if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL) || (urb_state == USBH_URB_DONE)) { //if check done, force to trigger a msg
		return HAL_OK;
	}

	return HAL_BUSY;
}

/**
  * @brief Check ecm bulk tx status and timeout, whether should trigger
  * @retval Status
  */
static int usbh_composite_cdc_ecm_bulk_tx_time_check(void)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info = &(cdc->ecm_tx_ep);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->driver->host;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, ep);

	if ((urb_state == USBH_URB_IDLE) && (usbh_get_elapsed_ticks(host, ep->tick) >= (USB_BULK_OUT_IDLE_MAX_CNT))) {
		return HAL_OK;
	}

	if ((urb_state == USBH_URB_BUSY) && (usbh_get_elapsed_ticks(host, ep->tick) >= (USB_BULK_OUT_BUSY_MAX_CNT))) {
		return HAL_OK;
	}

	if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL) || (urb_state == USBH_URB_DONE)) {
		return HAL_OK;
	}

	return HAL_BUSY;
}

/**
  * @brief Check ecm intr rx status and timeout, whether should trigger
  * @retval Status
  */
static int usbh_composite_cdc_ecm_intr_rx_time_check(void)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info = &(cdc->ecm_ctrl_ep);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->driver->host;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, ep);

	if ((USBH_URB_IDLE == urb_state) && (usbh_get_elapsed_ticks(host, ep->tick) >= USB_INTR_IN_IDLE_MAX_CNT)) {
		return HAL_OK;
	}

	if ((USBH_URB_BUSY == urb_state) && (usbh_get_elapsed_ticks(host, pipe_info->busy_tick) >= USB_INTR_IN_BUSY_MAX_CNT)) {
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
	usbh_pipe_t  *ep = &(cdc->ecm_tx_ep.pipe);
	usb_host_t *host = cdc->driver->host;

	if (usbh_composite_cdc_ecm_check_enum_status() != HAL_OK) {
		return ret;
	}

	if (cdc->state == CDC_ECM_STATE_TRANSFER) {
		if ((usbh_composite_cdc_ecm_bulk_tx_time_check() == HAL_OK)) {
			usbh_notify_class_state_change(host, ep->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Send event for bulk rx
  * @param  none
  * @retval Status
  */
static int usbh_composite_cdc_ecm_bulk_receive(void)
{
	int ret = HAL_BUSY;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_pipe_t  *ep = &(cdc->ecm_rx_ep.pipe);
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
  * @brief  Send event for intr rx
  * @retval Status
  */
static int usbh_composite_cdc_ecm_intr_receive(void)
{
	int ret = HAL_BUSY;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_pipe_t  *ep = &(cdc->ecm_ctrl_ep.pipe);
	usb_host_t *host = cdc->driver->host;

	if (usbh_composite_cdc_ecm_check_enum_status() != HAL_OK) {
		return ret;
	}

	if (cdc->state == CDC_ECM_STATE_TRANSFER) {
		if ((ep->xfer_state == USBH_EP_XFER_IDLE) || (usbh_composite_cdc_ecm_intr_rx_time_check() == HAL_OK)) {
			ep->xfer_state = USBH_EP_XFER_START;
			usbh_notify_class_state_change(host, ep->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Init CDC ECM class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_composite_cdc_ecm_init(usbh_composite_host_t *host, usbh_composite_cdc_ecm_usr_cb_t *cb)
{
	int ret = HAL_OK;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usb_os_memset(cdc, 0x00, sizeof(usbh_composite_cdc_ecm_host_t));

	cdc->driver = host;
	usbh_composite_cdc_ecm_priv_data_t *priv = cb->priv;

	cdc->dongle_ctrl_buf = (u8 *)usb_os_malloc(CDC_ECM_MAC_STRING_LEN);
	if (NULL == cdc->dongle_ctrl_buf) {
		RTK_LOGE(TAG, "Alloc mem %d fail\n", CDC_ECM_MAC_STRING_LEN);
		return HAL_ERR_MEM;
	}

	cdc->mac_valid = 0;

	if (priv == NULL) {
		RTK_LOGE(TAG, "Param error\n");
		USBH_COMPOSITE_ECM_FREE_MEM(cdc->led_array);
		cdc->led_cnt = 0;
	} else {
		if (priv->mac_value) {
			usbh_composite_cdc_ecm_set_dongle_mac(priv->mac_value);
		}
		if ((priv->led_array != NULL) && (priv->led_cnt > 0)) {
			usbh_composite_cdc_ecm_set_dongle_led_array(priv->led_array, priv->led_cnt);
		}
	}

	if (cb != NULL) {
		cdc->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "User init err %d", ret);
				return ret;
			}
		}
	}

#if ECM_STATE_DEBUG_ENABLE
	usbh_composite_cdc_ecm_debug_task_init();
#endif

	usb_os_sema_create(&(cdc->cdc_ecm_tx_sema));

	return ret;
}

/**
  * @brief  Deinit CDC ECM class
  * @retval Status
  */
int usbh_composite_cdc_ecm_deinit(void)
{
	int ret = HAL_OK;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

#if ECM_STATE_DEBUG_ENABLE
	usbh_composite_cdc_ecm_debug_task_deinit();
#endif
	usbh_composite_cdc_ecm_tx_status_check();
	usb_os_sema_delete(usbh_composite_cdc_ecm_host.cdc_ecm_tx_sema);

	usbh_composite_cdc_ecm_deinit_all_pipe();

	usbh_composite_cdc_ecm_deinit_ep_buf();

	USBH_COMPOSITE_ECM_FREE_MEM(cdc->led_array);
	USBH_COMPOSITE_ECM_FREE_MEM(cdc->dongle_ctrl_buf);

	if ((cdc->cb != NULL) && (cdc->cb->deinit != NULL)) {
		cdc->cb->deinit();
	}

	return ret;
}

/**
  * @brief  Start to send bulk out data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_composite_cdc_ecm_bulk_send(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	usbh_composite_cdc_ecm_pipe_info_t *pipe_info = &(cdc->ecm_tx_ep);
	usbh_pipe_t *ep = &(pipe_info->pipe);
	usb_host_t *host = cdc->driver->host;

	if (usbh_composite_cdc_ecm_check_enum_status() != HAL_OK) {
		return ret;
	}
	// RTK_LOGS(TAG, RTK_LOG_ERROR, "[bulk]%s-%d-%d-%d\n", __func__, __LINE__, ep->xfer_state, ep->pipe_num);
	if ((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->state == CDC_ECM_STATE_TRANSFER)) {
		if (ep->xfer_state == USBH_EP_XFER_IDLE) {
			pipe_info->buf = buf;
			pipe_info->buf_len = len;

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
	dev_id_info.bInterfaceClass = CDC_IF_CDC_CTRL_CODE;
	dev_id_info.bInterfaceSubClass = CDC_IF_CDC_CTRL_SUB_CLASS_ECM_CODE;
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
	dev_id_info.bInterfaceClass = CDC_IF_CDC_CTRL_CODE;
	dev_id_info.bInterfaceSubClass = CDC_IF_CDC_CTRL_SUB_CLASS_ECM_CODE;
	dev_id_info.bInterfaceProtocol = 0x00;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf = (usbh_itf_data_t *) usbh_get_interface_descriptor(host, &dev_id_info);

	if (itf) {
		ecm_ctrl_valid = 1;
	}

	dev_id_info.bInterfaceClass = CDC_IF_CDC_DATA_CODE;
	dev_id_info.bInterfaceSubClass = CDC_IF_CDC_DATA_SUB_CLASS_DATA_CODE;
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
	u8 i = 0;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
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
	u8 ret1 = usbh_composite_cdc_ecm_host.usb_setup;
	u8 ret2 = usbh_composite_cdc_ecm_host.eth_hw_connect;
	return ret1 && ret2;
}

u8 usbh_composite_cdc_ecm_prepare_done(void)
{
	usbh_composite_cdc_ecm_host.ready_to_xfer = 1;
	return HAL_OK;
}

int usbh_cdc_ecm_send_data(u8 *buf, u32 len)
{
	int ret;
	u8 retry_cnt = 0;

#if USBH_ECM_TX_SPEED_CHECK
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
		usbh_composite_cdc_ecm_host.cdc_ecm_tx_block = 1;
		usb_os_sema_take(usbh_composite_cdc_ecm_host.cdc_ecm_tx_sema, USB_OS_SEMA_TIMEOUT);
#if USBH_ECM_TX_SPEED_CHECK
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

	usbh_composite_cdc_ecm_host.cdc_ecm_tx_block = 0;

	return ret;
}

#if ECM_STATE_DEBUG_ENABLE
static int usbh_composite_cdc_ecm_get_hw_state(void)
{
	return usbh_composite_cdc_ecm_host.eth_hw_connect;
}
static int usbh_composite_cdc_ecm_get_usb_state(void)
{
	return usbh_composite_cdc_ecm_host.usb_setup;
}

static void usbh_composite_cdc_ecm_state_check_task(void *param)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	UNUSED(param);
	cdc->state_check_task_alive = 1;
	while (1) {
		if (cdc->driver->host) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Status %d-%d-%d/out %d-%d %d-%d/in %d-%d/intr %d-%d/%d-%d-%d\n",
					 cdc->state, usbh_composite_cdc_ecm_get_hw_state(), usbh_composite_cdc_ecm_get_usb_state(),
					 cdc->ecm_tx_ep.pipe.xfer_state, usbh_get_urb_state(cdc->driver->host, &(cdc->ecm_tx_ep.pipe)),
					 cdc->ecm_tx_ep.pipe.tick, cdc->cdc_ecm_tx_block,
					 cdc->ecm_rx_ep.pipe.xfer_state, usbh_get_urb_state(cdc->driver->host, &(cdc->ecm_rx_ep.pipe)),
					 cdc->ecm_ctrl_ep.pipe.xfer_state, usbh_get_urb_state(cdc->driver->host, &(cdc->ecm_ctrl_ep.pipe)),
					 cdc->ecm_tx_ep.trigger_cnt, cdc->ecm_rx_ep.trigger_cnt, cdc->ecm_ctrl_ep.trigger_cnt
					);
		}
		usb_os_sleep_ms(1000);
	}
}

void usbh_composite_cdc_ecm_debug_task_init(void)
{
	int status;
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;

	if (cdc->state_check_task_alive) {
		return;
	}

	status = rtos_task_create(&(cdc->state_check_task), "Ecm_debug", usbh_composite_cdc_ecm_state_check_task, NULL, 1024U * 2, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create ecm dump task fail\n");
	}
}

void usbh_composite_cdc_ecm_debug_task_deinit(void)
{
	usbh_composite_cdc_ecm_host_t *cdc = &usbh_composite_cdc_ecm_host;
	if (cdc->state_check_task != NULL) {
		RTK_LOGI(TAG, "Del debug task\n");
		rtos_task_delete(cdc->state_check_task);
		cdc->state_check_task = NULL;
	}

	cdc->state_check_task_alive = 0;
}
#endif