/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include "usbh.h"
#include "usbh_cdc_ecm.h"
#include "usbh_cdc_ecm_hal.h"
#include "usbh_cdc_ecm_appx.h"

/* Private defines -----------------------------------------------------------*/
//for 8156
static int usbh_cdc_ecm_process_get_rcr(usb_host_t *host);
static int usbh_cdc_ecm_process_set_rcr(usb_host_t *host);
//for 8152
static int usbh_cdc_ecm_process_set_flow_ctrl1(usb_host_t *host);
static int usbh_cdc_ecm_process_set_flow_ctrl2(usb_host_t *host);
static int usbh_cdc_ecm_process_mac_get_lock(usb_host_t *host);
static int usbh_cdc_ecm_process_mac_set_dis_lock(usb_host_t *host);
static int usbh_cdc_ecm_process_mac_set_mac1(usb_host_t *host);
static int usbh_cdc_ecm_process_mac_set_mac2(usb_host_t *host);
static int usbh_cdc_ecm_process_mac_en_lock(usb_host_t *host);
static int usbh_cdc_ecm_process_led_set_ctrl(usb_host_t *host);

static int usbh_cdc_acm_process_get_string(usb_host_t *host, u8 *pbuf, u16 buflen, u8 str_id);
static int usbh_cdc_ecm_ctrl_setting(usb_host_t *host, u16 vid, u16 pid);

#if ECM_ENABLE_PACKETFILTER
static int usbh_cdc_ecm_process_get_statistic(usb_host_t *host);
static int usbh_cdc_ecm_process_set_packet_filter(usb_host_t *host);
static int usbh_cdc_ecm_process_set_muiticast_filter(usb_host_t *host);
#endif

/* Private types -------------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
#define CDC_SET_LINE_CODING                                     0x20U
#define CDC_LINE_CODING_DATA_LEN                                0x07U

#define USB_DT_CS_INTERFACE                                     0x24

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "ECM_APPX";

static usbh_cdc_ecm_appx_t appx_cmd;


/*------------------------------------------------------Common API Start------------------------------------------------------------*/
static void dump_acm_cfgdesc(void)
{
#if ECM_ENABLE_DUMP_DESCRIPYOT_PARSE
	usbh_cdc_ecm_appx_t      *atcmd = &appx_cmd;
	usbh_cdc_ecm_ep_t        *pcomm_itf = &(atcmd->report_ep);
	usbh_cdc_ecm_ep_t        *pdata_tx = &(atcmd->tx_ep);
	usbh_cdc_ecm_ep_t        *pdata_rx = &(atcmd->rx_ep);

	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------Dump Descriptor Start------------------\n");

	//dump atcmd control
	RTK_LOGS(TAG, RTK_LOG_INFO, "INTR epaddr(0x%x) MPS(%d) interval(%d)\n",
			 pcomm_itf->ep_addr, pcomm_itf->ep_mps, pcomm_itf->ep_interval);
	RTK_LOGS(TAG, RTK_LOG_INFO, "MAC(0x%x) \n", atcmd->iMACAddressStringId);

	//dump atcmd data
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK in:epaddr(0x%x) MPS(=%d) out:epaddr(0x%x) MPS(%d)\n",
			 pdata_tx->ep_addr, pdata_tx->ep_mps,
			 pdata_rx->ep_addr, pdata_rx->ep_mps);

	RTK_LOGS(TAG, RTK_LOG_INFO, "---------------------Dump End-----------------------------\n");
#endif
}

static int usbh_cdc_ecm_appx_rx(void)
{
	usbh_cdc_ecm_ep_t  *ep = &appx_cmd.rx_ep;
	return usbh_cdc_ecm_hand_appx_rx(ep);
}

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

static u8 usbh_cdc_ecm_get_mac_str(usb_host_t *host)
{
	u8 i;
	u8 mac_is_valid = 0;
	u8 status = HAL_OK;
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	u16 vid = usbh_cdc_ecm_get_device_vid_info();
	u16 pid = usbh_cdc_ecm_get_device_pid_info();

	if ((atcmd->mac_src_type == CDC_ECM_MAC_UPPER_LAYER_SET) && (vid == 0x0BDA) && ((pid == 0x8152) || (pid == 0x8153))) {
		status = HAL_OK;
		RTK_LOGS(TAG, RTK_LOG_INFO,  "Upper set mac[%02x %02x %02x %02x %02x %02x]\n", atcmd->mac[0], atcmd->mac[1], atcmd->mac[2], atcmd->mac[3], atcmd->mac[4],
				 atcmd->mac[5]);
	} else {
		status = usbh_cdc_acm_process_get_string(host, atcmd->dongle_ctrl_buf, CDC_ECM_MAC_STRING_LEN, atcmd->iMACAddressStringId);
		atcmd->mac_src_type = CDC_ECM_MAC_DONGLE_SUPPLY;
	}

	if (status == HAL_OK) {
		if (atcmd->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY) {
			for (i = 0; i < 6; i++) {
				atcmd->mac[i] = usbh_cdc_ecm_hex_to_char(atcmd->dongle_ctrl_buf[2 + 4 * i]) * 16 + usbh_cdc_ecm_hex_to_char(atcmd->dongle_ctrl_buf[2 + 4 * i + 2]);
				if (atcmd->mac[i]) {
					mac_is_valid = 1;
				}
			}

			if (mac_is_valid == 0) {
				TRNG_get_random_bytes(atcmd->mac, CDC_ECM_MAC_STR_LEN);
				RTK_LOGS(TAG, RTK_LOG_INFO,  "Random mac[%02x %02x %02x %02x %02x %02x]\n", atcmd->mac[0], atcmd->mac[1], atcmd->mac[2], atcmd->mac[3], atcmd->mac[4],
						 atcmd->mac[5]);
				atcmd->mac_src_type = CDC_ECM_MAC_RANDOM_SET;
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO,  "Dongle mac[%02x %02x %02x %02x %02x %02x]\n", atcmd->mac[0], atcmd->mac[1], atcmd->mac[2], atcmd->mac[3], atcmd->mac[4],
						 atcmd->mac[5]);
				atcmd->mac_valid = 1;
			}
		}
	}

	return status;
}
/*------------------------------------------------------Common API End------------------------------------------------------------*/



/*------------------------------------------------------RTK ECM Config Start------------------------------------------------------------*/

/**
  * @brief  Usb alt setting function
  * @param  host: Host handle
  * @retval Status
*/
static int usbh_cdc_ecm_process_set_alt(usb_host_t *host)
{
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_REQ_SET_INTERFACE;
	setup.req.wValue = usbh_cdc_ecm_get_ecm_itf_ifnum();
	setup.req.wIndex = usbh_cdc_ecm_get_ecm_itf_alt();
	setup.req.wLength = 0U;

	return usbh_ctrl_request(host, &setup, NULL);
}

#if ECM_ENABLE_PACKETFILTER
/**
  * @brief  Set Ethernet Packet Filter
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_set_packet_filter(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = CDC_ECM_SET_ETHERNET_PACKET_FILTER;
	setup.req.wValue = atcmd->packet_filter;
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
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = CDC_ECM_SET_ETHERNET_MULTICAST_FILTERS;
	setup.req.wValue = 1;
	setup.req.wIndex = 1U;
	setup.req.wLength = atcmd->muticast_filter_len;

	return usbh_ctrl_request(host, &setup, atcmd->dongle_ctrl_buf);
}

/**
  * @brief  Get Ethernet Statistic
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_get_statistic(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = CDC_ECM_GET_ETHERNET_STATISTIC;
	setup.req.wValue = atcmd->feature_selector;
	setup.req.wIndex = 1U;
	setup.req.wLength = 4;

	return usbh_ctrl_request(host, &setup, atcmd->dongle_ctrl_buf);
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
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xc010;
	setup.req.wIndex = 0x0100;
	setup.req.wLength = 4;

	return usbh_ctrl_request(host, &setup, atcmd->dongle_ctrl_buf);
}

/**
  * @brief  Set 8156 rcr params
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_set_rcr(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xc010;
	setup.req.wIndex = 0x010F;
	setup.req.wLength = 4;

	return usbh_ctrl_request(host, &setup, atcmd->dongle_ctrl_buf);
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
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xC0A4;
	setup.req.wIndex = 0x0103;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, (u8 *)atcmd->dongle_ctrl_buf);
}
/**
  * @brief  Set 8152 flow ctrl params2
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_set_flow_ctrl2(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xC0A8;
	setup.req.wIndex = 0x0103;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, (u8 *)atcmd->dongle_ctrl_buf);
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
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xE81C;
	setup.req.wIndex = 0x010F;
	setup.req.wLength = 4;

	return usbh_ctrl_request(host, &setup, atcmd->dongle_ctrl_buf);
}

static int usbh_cdc_ecm_process_mac_set_dis_lock(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xE81C;
	setup.req.wIndex = 0x010F;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, atcmd->dongle_ctrl_buf);
}

static int usbh_cdc_ecm_process_mac_set_mac1(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xC000;
	setup.req.wIndex = 0x010F;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, atcmd->dongle_ctrl_buf);
}

static int usbh_cdc_ecm_process_mac_set_mac2(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xC004;
	setup.req.wIndex = 0x0103;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, atcmd->dongle_ctrl_buf);
}

static int usbh_cdc_ecm_process_mac_en_lock(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xE81C;
	setup.req.wIndex = 0x010F;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, atcmd->dongle_ctrl_buf);
}

/**
  * @brief  Set led color for 8152
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_led_set_ctrl(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	setup.req.bmRequestType = 0x40;
	setup.req.bRequest = 0x05;
	setup.req.wValue = 0xDD90;
	setup.req.wIndex = 0x0103;
	setup.req.wLength = 4;
	return usbh_ctrl_request(host, &setup, atcmd->dongle_ctrl_buf);
}

static void usbh_cdc_ecm_set_dongle_mac(u8 *mac)
{
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	if (NULL == mac) {
		RTK_LOGE(TAG, "Param error,mac is NULL\n");
		return ;
	}

	memcpy((void *) & (atcmd->mac[0]), (void *)mac, 6);
	atcmd->mac_src_type = CDC_ECM_MAC_UPPER_LAYER_SET;
}

static void usbh_cdc_ecm_set_dongle_led_array(u16 *led, u8 len)
{
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	if (led == NULL || len == 0) {
		RTK_LOGE(TAG, "Param error,led is NULL or len %ld\n", (u32)len);
		return ;
	}

	USBH_ECM_FREE_MEM(atcmd->led_array);

	atcmd->led_array = (u16 *)usb_os_malloc(len * sizeof(u16));
	memcpy((void *)atcmd->led_array, (void *)led, len * sizeof(u16));

	atcmd->led_cnt = len;
}

static void usbh_cdc_ecm_config_dongle_mac(usb_host_t *host, u16 vid, u16 pid)
{
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;
	u8 state = HAL_OK;

	// RTK_LOGS(TAG, RTK_LOG_INFO, "mac issue status[%d]V(0x%04x)P(0x%04x) type %d\n",appx_cmd.sub_status,vid,pid,atcmd->mac_src_type);

	switch (atcmd->sub_status) {
	case CDC_ECM_STATE_CTRL_MAC_GET_LOCK: //8152 mac
		if ((pid != 0x8152) || (vid != 0x0BDA) || (atcmd->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY)) {
			atcmd->sub_status++;
		} else {
			usb_os_memset(atcmd->mac_ctrl_lock, 0, 4);
			state = usbh_cdc_ecm_process_mac_get_lock(host);
			if (state == HAL_OK) {
				usb_os_memcpy(atcmd->mac_ctrl_lock, atcmd->dongle_ctrl_buf, CDC_ECM_MAC_CTRL_REG_LEN);
				atcmd->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get MAC lock err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_DISABLE_LOCK: //8152 mac
		if ((pid != 0x8152) || (vid != 0x0BDA) || (atcmd->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY)) {
			atcmd->sub_status++;
		} else {
			atcmd->mac_ctrl_lock[0] = 0xD0;
			usb_os_memcpy(atcmd->dongle_ctrl_buf, atcmd->mac_ctrl_lock, CDC_ECM_MAC_CTRL_REG_LEN);
			state = usbh_cdc_ecm_process_mac_set_dis_lock(host);
			if (state == HAL_OK) {
				atcmd->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Dis MAC lock err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_SET_MAC1: //8152 mac
		if ((pid != 0x8152) || (vid != 0x0BDA) || (atcmd->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY)) {
			atcmd->sub_status++;
		} else {
			usb_os_memcpy(atcmd->dongle_ctrl_buf, &(atcmd->mac[0]), CDC_ECM_MAC_CTRL_REG_LEN);
			state = usbh_cdc_ecm_process_mac_set_mac1(host);
			if (state == HAL_OK) {
				atcmd->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set MAC1 err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_SET_MAC2: //8152 mac
		if ((pid != 0x8152) || (vid != 0x0BDA) || (atcmd->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY)) {
			atcmd->sub_status++;
		} else {
			usb_os_memcpy(atcmd->dongle_ctrl_buf, &(atcmd->mac[4]), CDC_ECM_MAC_CTRL_REG_LEN);
			atcmd->dongle_ctrl_buf[2] = atcmd->dongle_ctrl_buf[3] = 0xFF;
			state = usbh_cdc_ecm_process_mac_set_mac2(host);
			if (state == HAL_OK) {
				atcmd->sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set MAC2 err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_ENABLE_LOCK: //8152 mac
		if ((pid != 0x8152) || (vid != 0x0BDA) || (atcmd->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY)) {
			atcmd->sub_status++;
		} else {
			atcmd->mac_ctrl_lock[0] = 0x10;
			usb_os_memcpy(atcmd->dongle_ctrl_buf, atcmd->mac_ctrl_lock, CDC_ECM_MAC_CTRL_REG_LEN);
			state = usbh_cdc_ecm_process_mac_en_lock(host);
			if (state == HAL_OK) {
				atcmd->sub_status++;
				atcmd->mac_valid = 1;
				RTK_LOGS(TAG, RTK_LOG_INFO, "Mac set success \n");
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "En MAC lock err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	default:
		atcmd->sub_status++;
		break;
	}
}

static int usbh_cdc_ecm_ctrl_setting(usb_host_t *host, u16 vid, u16 pid)
{
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;
	u8 ret_state = HAL_ERR_UNKNOWN;
	u8 state = HAL_OK;

#if ECM_ENABLE_PACKETFILTER
	u8 mac_str[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif

	switch (appx_cmd.sub_status) {
	case CDC_ECM_STATE_AT_SETTING_IDLE:
		usb_os_sleep_ms(1000);
		break;

	case CDC_ECM_STATE_GET_MAC_STR:
		state = usbh_cdc_ecm_get_mac_str(host);
		if (state == HAL_OK) {
			appx_cmd.sub_status ++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_INFO,   "Get MAC fail error[%d]\n", state);
			usb_os_sleep_ms(10);
		}
		break;

	case CDC_ECM_STATE_CTRL_ALT_SETTING:
		state = usbh_cdc_ecm_process_set_alt(host);
		if (state == HAL_OK) {
			appx_cmd.sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM alt setting err\n");
			usb_os_sleep_ms(100);
		}
		break;

#if ECM_ENABLE_PACKETFILTER
	case CDC_ECM_STATE_CTRL_SET_ETHERNET_MULTICAST_FILTER:
		usb_os_memcpy(atcmd->muticast_filter, mac_str, 6);
		atcmd->muticast_filter_len = 6;
		state = usbh_cdc_ecm_process_set_muiticast_filter(host);
		if (state == HAL_OK) {
			usb_os_memcpy(atcmd->muticast_filter, atcmd->dongle_ctrl_buf, atcmd->muticast_filter_len);
			appx_cmd.sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM set eth multicast filter err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_SET_ETHERNET_PACKET_FILTER:
		atcmd->packet_filter = CDC_ECM_ETH_PACKET_TYPE_DIRECTED;
		state = usbh_cdc_ecm_process_set_packet_filter(host);
		if (state == HAL_OK) {
			appx_cmd.sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM set eth packet filter err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_GET_ETHERNET_STATISTIC:
		state = usbh_cdc_ecm_process_get_statistic(host);
		if (state == HAL_OK) {
			usb_os_memcpy(&(atcmd->eth_statistic_count), atcmd->dongle_ctrl_buf, CDC_ECM_MAC_CTRL_REG_LEN);
			appx_cmd.sub_status++;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM get eth statistic err\n");
			usb_os_sleep_ms(100);
		}
		break;
#endif

///just for 8156
	case CDC_ECM_STATE_CTRL_RCR_GET:
		if ((pid != 0x8156) || (vid != 0x0BDA)) {
			appx_cmd.sub_status++;
		} else {
			usb_os_memset(atcmd->rcr, 0, 4);
			state = usbh_cdc_ecm_process_get_rcr(host);
			if (state == HAL_OK) {
				usb_os_memcpy(atcmd->rcr, atcmd->dongle_ctrl_buf, CDC_ECM_MAC_CTRL_REG_LEN);
				appx_cmd.sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get RCR err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_RCR_SET:
		if ((pid != 0x8156) || (vid != 0x0BDA)) {
			appx_cmd.sub_status++;
		} else {
			//set bit 0~3 set 1 will enable PING & UDP transfer
			atcmd->rcr[0] = atcmd->rcr[0] | 0x0F;
			usb_os_memcpy(atcmd->dongle_ctrl_buf, atcmd->rcr, CDC_ECM_MAC_CTRL_REG_LEN);
			state = usbh_cdc_ecm_process_set_rcr(host);
			if (state == HAL_OK) {
				appx_cmd.sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set RCR err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	///just for 8152
	case CDC_ECM_STATE_FLOW_CTRL1:
		if ((pid != 0x8152) || (vid != 0x0BDA)) {
			appx_cmd.sub_status++;
		} else {
			usb_os_memset(atcmd->flow_ctrl, 0, 4);
			atcmd->flow_ctrl[0] = 0x60;
			usb_os_memcpy(atcmd->dongle_ctrl_buf, atcmd->flow_ctrl, CDC_ECM_MAC_CTRL_REG_LEN);
			state = usbh_cdc_ecm_process_set_flow_ctrl1(host);
			if (state == HAL_OK) {
				appx_cmd.sub_status++;
			} else if (state != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set flow ctrl1 err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_FLOW_CTRL2:
		if ((pid != 0x8152) || (vid != 0x0BDA)) {
			appx_cmd.sub_status++;
		} else {
			usb_os_memset(atcmd->flow_ctrl, 0, 4);
			atcmd->flow_ctrl[0] = 0xa0;
			usb_os_memcpy(atcmd->dongle_ctrl_buf, atcmd->flow_ctrl, CDC_ECM_MAC_CTRL_REG_LEN);
			state = usbh_cdc_ecm_process_set_flow_ctrl2(host);
			if (state == HAL_OK) {
				appx_cmd.sub_status++;
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
		usbh_cdc_ecm_config_dongle_mac(host, vid, pid);
		break;

	case CDC_ECM_STATE_CTRL_LED_COLOR_SET: //8152 led ctrl
		if ((pid != 0x8152) || (vid != 0x0BDA) || (atcmd->led_cnt == 0) || (atcmd->led_array == NULL)) {
			appx_cmd.sub_status++;
		} else {
			usb_os_memset(atcmd->dongle_ctrl_buf, 0xFF, 4);
			usb_os_memcpy(atcmd->dongle_ctrl_buf, (u8 *) & (atcmd->led_array[0]), 2);
			state = usbh_cdc_ecm_process_led_set_ctrl(host);
			if (state == HAL_OK) {
				appx_cmd.sub_status++;
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
/*------------------------------------------------------RTK ECM Config End------------------------------------------------------------*/

/*------------------------------------------------------Quectel 4G ECM Config Start------------------------------------------------------------*/
static u8 usbh_cdc_ecm_at_set_line_coding(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;
	usbh_cdc_ecm_line_coding_t *plinecoding = &atcmd->line_coding;

	plinecoding->b.dwDteRate = 115200;
	plinecoding->b.bCharFormat = CDC_ACM_LINE_CODING_CHAR_FORMAT_1_STOP_BITS;
	plinecoding->b.bParityType = CDC_ACM_LINE_CODING_PARITY_NO;
	plinecoding->b.bDataBits = 8;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = CDC_SET_LINE_CODING;
	setup.req.wValue = 0U;
	setup.req.wIndex = (u16)atcmd->param_item->at_line_idx;
	setup.req.wLength = CDC_LINE_CODING_DATA_LEN;

	usb_os_memcpy(atcmd->dongle_ctrl_buf, plinecoding->d8, sizeof(usbh_cdc_ecm_line_coding_t));

	return usbh_ctrl_request(host, &setup, atcmd->dongle_ctrl_buf);
}

static int usbh_cdc_ecm_eg915_ctrl_setting(usb_host_t *host)
{
	u8 ret_state = HAL_ERR_UNKNOWN;
	u8 state = HAL_OK;

	switch (appx_cmd.sub_status) {
	case CDC_ECM_STATE_GET_MAC_STR:
		state = usbh_cdc_ecm_get_mac_str(host);
		if (state == HAL_OK) {
			appx_cmd.sub_status = CDC_ECM_STATE_AT_SET_LINE_CODING;
			RTK_LOGS(TAG, RTK_LOG_INFO,   "Get MAC success\n");
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_INFO,   "Get MAC fail error[%d]\n", state);
			usb_os_sleep_ms(10);
		}
		break;
	case CDC_ECM_STATE_AT_SET_LINE_CODING:
		state = usbh_cdc_ecm_at_set_line_coding(host);
		if (state == HAL_OK) {
			appx_cmd.sub_status = CDC_ECM_STATE_AT_SETTING_IDLE;
			RTK_LOGS(TAG, RTK_LOG_INFO,   "Set line coding success\n");
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_INFO,   "Set line coding error[%d]\n", state);
			usb_os_sleep_ms(100);
		}
		break;
	default:
		ret_state = HAL_OK;
		break;
	}

	return ret_state;
}

static int usbh_cdc_ecm_eg91_ctrl_setting(usb_host_t *host)
{
	u8 state = HAL_OK;
	u8 ret_state = HAL_ERR_UNKNOWN;

	switch (appx_cmd.sub_status) {
	case CDC_ECM_STATE_GET_MAC_STR:
		state = HAL_OK;
		appx_cmd.sub_status = CDC_ECM_STATE_AT_SET_LINE_CODING;
		break;
	case CDC_ECM_STATE_AT_SET_LINE_CODING:
		state = usbh_cdc_ecm_at_set_line_coding(host);
		if (state == HAL_OK) {
			appx_cmd.sub_status = CDC_ECM_STATE_AT_SETTING_IDLE;
			RTK_LOGS(TAG, RTK_LOG_INFO,  "Set line coding success\n");
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_INFO,  "Set line coding error[%d]\n", state);
			usb_os_sleep_ms(100);
		}
		break;
	default:
		ret_state = HAL_OK;
		break;
	}

	return ret_state;
}
/*------------------------------------------------------Quectel 4G ECM Config End------------------------------------------------------------*/


static u8 usbh_cdc_ecm_ep_init(usb_host_t *host, usbh_ep_desc_t *ep_desc, usbh_cdc_ecm_ep_t *ep, u8 ep_type, usb_timer_func fn, usbh_ecm_xter_type_t type)
{
	u8 pipe_num;

	ep->ep_addr = ep_desc->bEndpointAddress;
	ep->ep_mps = ep_desc->wMaxPacketSize;
	ep->ep_interval = ep_desc->bInterval;

	pipe_num = usbh_alloc_pipe(host, ep->ep_addr);
	if (pipe_num != 0xFFU) {
		ep->pipe_id = pipe_num;
		usbh_ecm_timer_register(pipe_num, 10, fn, type);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Appx ep%02x/pipe %d\n", ep->ep_addr, ep->pipe_id);
	} else {
		RTK_LOGE(TAG,  "Fail to allocate pipe for CDC ECM class.");
		return HAL_ERR_MEM;
	}

	usbh_open_pipe(host,
				   ep->pipe_id,
				   ep->ep_addr,
				   ep_type,
				   ep->ep_mps);

	return HAL_OK;
}



/**
  * @brief  Bulk Out handling function
  * @param  host: Host handle
  * @retval Status
*/
static void usbh_cdc_ecm_at_bulk_out(usb_host_t *host, u8 *next)
{
	u8 loop = 1;
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;
	usbh_cdc_ecm_ep_t  *ep = &atcmd->tx_ep;
	usbh_urb_state_t urb_state = USBH_URB_IDLE;

	switch (ep->xfer_state) {
	case CDC_ECM_TRANSFER_STATE_XFER:
		ep->xfer_state = CDC_ECM_TRANSFER_STATE_BUSY;
		if (ep->xfer_len > ep->ep_mps) {
			usbh_bulk_send_data(host,
								ep->xfer_buf,
								ep->ep_mps,
								ep->pipe_id);
		} else {
			usbh_bulk_send_data(host,
								ep->xfer_buf,
								ep->xfer_len,
								ep->pipe_id);
		}
		break;

	case CDC_ECM_TRANSFER_STATE_BUSY:
		urb_state = usbh_get_urb_state(host, ep->pipe_id);
		if (urb_state == USBH_URB_DONE) {
			if (ep->xfer_len >= ep->ep_mps) {
				ep->xfer_len -= ep->ep_mps;
				ep->xfer_buf += ep->ep_mps;
				ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
			} else {
				usb_os_sema_give(atcmd->acm_tx_sema);
				ep->xfer_len = 0U;
				ep->xfer_state = CDC_ECM_TRANSFER_STATE_IDLE;
			}
		} else if (urb_state == USBH_URB_BUSY) { //retransmit
			ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
			ep->xfer_state = CDC_ECM_TRANSFER_STATE_IDLE;
			usb_os_sema_give(atcmd->acm_tx_sema);
			RTK_LOGE(TAG, "BULK out failed %d\n", urb_state);
		}
		break;

	default:
		loop = 0;
		break;
	}

	if (loop) {
		*next = 1;
	}
}


/**
  * @brief  Bulk In handling  function
  * @param  host: Host handle
  * @retval Status
*/
static void usbh_cdc_ecm_at_bulk_in(usb_host_t *host, u8 *next)
{
	u8 loop = 1;
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;
	usbh_cdc_ecm_ep_t  *ep = &atcmd->rx_ep;
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

	switch (ep->xfer_state) {
	case CDC_ECM_TRANSFER_STATE_XFER:
		usbh_bulk_receive_data(host,
							   ep->xfer_buf,
							   ep->ep_mps,
							   ep->pipe_id);
		ep->xfer_state = CDC_ECM_TRANSFER_STATE_BUSY;
		break;

	case CDC_ECM_TRANSFER_STATE_BUSY:
		urb_state = usbh_get_urb_state(host, ep->pipe_id);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, ep->pipe_id);
			if (atcmd->rx_report != NULL) {
				atcmd->rx_report(ep->xfer_buf, len, USB_CH_EP_TYPE_BULK);
			}
			if (len >= ep->ep_mps) {
				ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
			} else {
				ep->xfer_state = CDC_ECM_TRANSFER_STATE_IDLE;  //set rx to accelerate speed
			}
		} else if (urb_state == USBH_URB_BUSY) {
			usb_os_sleep_ms(1);
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
			ep->xfer_state = CDC_ECM_TRANSFER_STATE_IDLE;
			// RTK_LOGS(TAG, RTK_LOG_INFO,  "App BULK in failed %d\n", urb_state);
		}
		break;

	default:
		loop = 0;
		break;
	}

	if (loop) {
		*next = 1;
	}
}
/**
  * @brief  Intr in function
  * @param  host: Host handle
  * @retval Status
*/
static void usbh_cdc_ecm_at_intr_in(usb_host_t *host, u8 *next)
{
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;
	usbh_cdc_ecm_ep_t  *ep = &atcmd->report_ep;
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

	switch (ep->xfer_state) {
	case CDC_ECM_TRANSFER_STATE_XFER:
		usbh_intr_receive_data(host,
							   ep->xfer_buf,
							   ep->ep_mps,
							   ep->pipe_id);

		ep->xfer_state = CDC_ECM_TRANSFER_STATE_BUSY;
		*next = 1;
		break;

	case CDC_ECM_TRANSFER_STATE_BUSY:
		urb_state = usbh_get_urb_state(host, ep->pipe_id);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, ep->pipe_id);
			if (atcmd->rx_report != NULL) {
				atcmd->rx_report(ep->xfer_buf, len, USB_CH_EP_TYPE_INTR);
			}
			if (len >= ep->ep_mps) {
				ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
			} else {
				ep->xfer_state = CDC_ECM_TRANSFER_STATE_IDLE;
			}
			*next = 1;
		} else if (urb_state == USBH_URB_BUSY) {
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
			RTK_LOGS(TAG, RTK_LOG_INFO,  "INTR in failed %d\n", urb_state);
		} else if (urb_state == USBH_URB_IDLE) {
		}
		break;

	default:
		break;
	}
}

static u8 usbh_cdc_ecm_parse_get_mac_id(usb_host_t *host)
{
	u8 find_mac = 0 ;
	u32 cfglen;
	usbh_cfg_desc_t *desc;
	usbh_itf_desc_t *pbuf;
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	/*
		parse to get mac info
	*/
	desc = (usbh_cfg_desc_t *)usbh_get_active_raw_configuration_descriptor(host);
	cfglen = (u32)desc->wTotalLength;
	pbuf = (usbh_itf_desc_t *) desc;

	while (!find_mac) {
		pbuf = (usbh_itf_desc_t *)usbh_cdc_ecm_find_next_ifdesc((u8 *)pbuf, &cfglen);
		if (pbuf == NULL) {
			break;
		}
		if ((pbuf->bInterfaceClass == CDC_IF_CDC_CTRL_CODE) //find the right interface
			&& (pbuf->bInterfaceSubClass == CDC_IF_CDC_CTRL_SUB_CLASS_ECM_CODE)) {
			cfglen -= pbuf->bLength;
			pbuf = (usbh_itf_desc_t *)((u8 *)pbuf + pbuf->bLength);
			while (!find_mac) {
				if (USB_DT_CS_INTERFACE == ((usbh_cdc_ecm_network_func_t *)pbuf)->bDescriptorType
					&& CDC_ECM_NETWORK_FUNC_DESCRIPTOR == ((usbh_cdc_ecm_network_func_t *)pbuf)->bDescriptorSubtype) {
					atcmd->iMACAddressStringId = ((usbh_cdc_ecm_network_func_t *)pbuf)->iMACAddress;
					RTK_LOGS(TAG, RTK_LOG_INFO,  "Mac string id(%d)\n", atcmd->iMACAddressStringId);
					find_mac = 1;
					break;
				} else {
					cfglen -= ((usb_ecm_descriptor_header *) pbuf)->bLength;
					pbuf = (usbh_itf_desc_t *)((u8 *)pbuf + ((usb_ecm_descriptor_header *) pbuf)->bLength);
				}
			}
		} else {
			cfglen -= pbuf->bLength;
			pbuf = (usbh_itf_desc_t *)((u8 *)pbuf + pbuf->bLength);
		}
	}

	return HAL_OK;
}

static int usbh_cdc_acm_tx_status_check(void)
{
	if (appx_cmd.acm_tx_block) {
		//release the sema to return usbh_cdc_ecm_appx_send_data
		do {
			usb_os_sema_give(appx_cmd.acm_tx_sema);
			usb_os_sleep_ms(1U);
		} while (appx_cmd.acm_tx_block);
	}

	return HAL_OK;
}


/* Exported functions --------------------------------------------------------*/
/*
	below is the public apis that called by other file
*/
/**
  * @brief	Find next standard descriptor interface
  * @param	pbuf: given buffer to find
  			len: given buffer length
  * @retval Pointer of next standard descriptor
  */
u8 *usbh_cdc_ecm_find_next_ifdesc(u8 *pbuf, u32 *len)
{
	u8 *desc = pbuf;

	while (*len > 0) {
		if (((usb_ecm_descriptor_header *)desc)->bDescriptorType == USB_DESC_TYPE_INTERFACE) {
			return (u8 *) desc;
		} else {
			*len -= ((usb_ecm_descriptor_header *)desc)->bLength;
			desc += ((usb_ecm_descriptor_header *)desc)->bLength;
		}
	}

	return NULL;
}

u8 usbh_cdc_ecm_appx_doinit(usbh_cdc_ecm_priv_data_t *priv)
{
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	if (priv == NULL) {
		RTK_LOGE(TAG, "Param error\n");
		atcmd->param_array = NULL;
		atcmd->rx_report = NULL;

		USBH_ECM_FREE_MEM(atcmd->led_array);
		atcmd->led_cnt = 0;
	} else {
		if (priv->mac_value) {
			usbh_cdc_ecm_set_dongle_mac(priv->mac_value);
		}
		if ((priv->led_array != NULL) && (priv->led_cnt > 0)) {
			usbh_cdc_ecm_set_dongle_led_array(priv->led_array, priv->led_cnt);
		}

		atcmd->param_array = priv->app_param;
		atcmd->rx_report = priv->appx_cb;
	}

	atcmd->dongle_ctrl_buf = (u8 *)usb_os_malloc(CDC_ECM_MAC_STRING_LEN);
	if (NULL == atcmd->dongle_ctrl_buf) {
		RTK_LOGE(TAG, "Alloc mem %d fail\n", CDC_ECM_MAC_STRING_LEN);
		return HAL_ERR_MEM;
	}

	atcmd->mac_valid = 0;

	atcmd->acm_tx_block = 0;
	usb_os_sema_create(&(atcmd->acm_tx_sema));

	return HAL_OK;
}

u8 usbh_cdc_ecm_appx_deinit(usb_host_t *host)
{
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;

	usbh_cdc_ecm_appx_deinit_pipe(host);

	USBH_ECM_FREE_MEM(atcmd->dongle_ctrl_buf);

	USBH_ECM_FREE_MEM(atcmd->report_ep.xfer_buf);

	USBH_ECM_FREE_MEM(atcmd->rx_ep.xfer_buf);

	usbh_cdc_acm_tx_status_check();
	usb_os_sema_delete(atcmd->acm_tx_sema);

	RTK_LOGS(TAG, RTK_LOG_INFO,  "Rx deinit\n");

	return HAL_OK;
}

u8 usbh_cdc_ecm_parse_at_cfgdesc(usb_host_t *host, u16 vid, u16 pid)
{
	usbh_cdc_ecm_appx_t *pappx_cmd = &appx_cmd;
	usbh_cdc_ecm_appx_param_t *data;
	usbh_itf_desc_t *comm_itf_desc;
	usbh_ep_desc_t *ep_desc;
	u8 status = HAL_ERR_UNKNOWN;
	u8 ret = HAL_OK;
	u8 ep_id;

	pappx_cmd->param_item = NULL;
	if (vid == USB_DEFAULT_VID) {
		//parse to get the mac string
		usbh_cdc_ecm_parse_get_mac_id(host);
		pappx_cmd->sub_status = CDC_ECM_STATE_GET_MAC_STR;
	} else {
		data = pappx_cmd->param_array;
		while (data->vid != 0) {
			if ((data->vid == vid) && ((vid == USB_DEFAULT_VID) || (data->pid == pid))) { /*rtk dongle*/
				pappx_cmd->param_item = data;
				break;
			}
			data++;
		}
		if (NULL == pappx_cmd->param_item) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Find vid(%d)pid(%d) cfg fail\n", vid, pid);
			pappx_cmd->param_item = pappx_cmd->param_array;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO,  "Choose vid(0x%x)pid(0x%x)atid(%d) \n", pappx_cmd->param_item->vid, pappx_cmd->param_item->pid,
				 pappx_cmd->param_item->at_line_idx);

		/* Get Communication Interface */
		comm_itf_desc = usbh_get_interface_descriptor(host, pappx_cmd->param_item->at_line_idx, 0);///at port
		if (comm_itf_desc == NULL) {
			RTK_LOGE(TAG, "Fail to get the %d common interface.\n", pappx_cmd->param_item->at_line_idx);
			return status;
		}

		if (comm_itf_desc->bNumEndpoints != 3) {
			RTK_LOGS(TAG, RTK_LOG_INFO,  "Maybe Error Happen(id=%d)\n", comm_itf_desc->bNumEndpoints);
		}
		for (ep_id = 0 ; ep_id < comm_itf_desc->bNumEndpoints ; ep_id++) {
			ep_desc = &comm_itf_desc->ep_desc_array[ep_id];

			if (ep_desc->bmAttributes == USB_CH_EP_TYPE_BULK) {
				if (USB_EP_IS_IN(ep_desc->bEndpointAddress)) {
					usbh_cdc_ecm_ep_init(host, ep_desc, &pappx_cmd->rx_ep, USB_CH_EP_TYPE_BULK, usbh_cdc_ecm_appx_rx, USBH_CDC_ECM_APP_TYPE_BULK_IN);
				} else {
					usbh_cdc_ecm_ep_init(host, ep_desc, &pappx_cmd->tx_ep, USB_CH_EP_TYPE_BULK, NULL, USBH_CDC_ECM_APP_TYPE_BULK_OUT);
				}
			} else if (ep_desc->bmAttributes == USB_CH_EP_TYPE_INTR) {
				usbh_cdc_ecm_ep_init(host, ep_desc, &pappx_cmd->report_ep, USB_CH_EP_TYPE_INTR, NULL, USBH_CDC_ECM_APP_TYPE_INTR);
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO,  "Unknown support transfer type(%d)\n", ep_desc->bmAttributes);
			}
		}

		pappx_cmd->report_ep.xfer_buf = (u8 *)usb_os_malloc(pappx_cmd->report_ep.ep_mps);
		pappx_cmd->rx_ep.xfer_buf = (u8 *)usb_os_malloc(pappx_cmd->rx_ep.ep_mps);

		dump_acm_cfgdesc();

		usbh_cdc_ecm_parse_get_mac_id(host);

		pappx_cmd->sub_status = CDC_ECM_STATE_GET_MAC_STR;
	}

	return ret;
}

u8 usbh_cdc_ecm_appx_deinit_pipe(usb_host_t *host)
{
	usbh_cdc_ecm_appx_t *pappx_cmd = &appx_cmd;

	if (pappx_cmd->report_ep.pipe_id) {
		usbh_close_pipe(host, pappx_cmd->report_ep.pipe_id);
		usbh_free_pipe(host, pappx_cmd->report_ep.pipe_id);
		pappx_cmd->report_ep.pipe_id = 0U;
	}
	if (pappx_cmd->rx_ep.pipe_id) {
		usbh_close_pipe(host, pappx_cmd->rx_ep.pipe_id);
		usbh_free_pipe(host, pappx_cmd->rx_ep.pipe_id);
		pappx_cmd->rx_ep.pipe_id = 0U;
	}
	if (pappx_cmd->tx_ep.pipe_id) {
		usbh_close_pipe(host, pappx_cmd->tx_ep.pipe_id);
		usbh_free_pipe(host, pappx_cmd->tx_ep.pipe_id);
		pappx_cmd->tx_ep.pipe_id = 0U;
	}

	return HAL_OK;
}

/*
	ctrl info
	rtk:get mac info
	4G acm ctrl line
*/
u8 usbh_cdc_ecm_appx_pre_setting(usb_host_t *host)
{
	u8 state = HAL_OK;
	u16 vid = usbh_cdc_ecm_get_device_vid_info();
	u16 pid = usbh_cdc_ecm_get_device_pid_info();

	if (vid == USB_DEFAULT_VID) { //rtk
		return usbh_cdc_ecm_ctrl_setting(host, vid, pid);
	} else if ((vid == USB_EF_DONGLE_VID) && (pid == EF_DONGLE_PID_EG915)) {
		return usbh_cdc_ecm_eg915_ctrl_setting(host);
	} else if ((vid == USB_EF_DONGLE_VID) && (pid == EF_DONGLE_PID_EG91)) {
		return usbh_cdc_ecm_eg91_ctrl_setting(host);
	} else {
		state = HAL_OK;
	}

	return state;
}

u8 usbh_cdc_ecm_appx_transfer(usb_host_t *host, u32 msg)
{
	usbh_cdc_ecm_appx_t *atcmd = &appx_cmd;
	u8 re_trigger = 0;

	if (msg == atcmd->tx_ep.pipe_id) {
		re_trigger = 0;
		usbh_cdc_ecm_at_bulk_out(host, &re_trigger);
		if (re_trigger) {
			usbh_notify_class_state_change(host, atcmd->tx_ep.pipe_id);
		}
	}

	else if (msg == atcmd->report_ep.pipe_id) {
		// RTK_LOGS(TAG, RTK_LOG_INFO,  "msg %d report in\n",msg);
		re_trigger = 0;
		usbh_cdc_ecm_at_intr_in(host, &re_trigger);
		if (re_trigger) {
			usbh_notify_class_state_change(host, atcmd->rx_ep.pipe_id);
		}
	}

	else if (msg == atcmd->rx_ep.pipe_id) {
		// RTK_LOGS(TAG, RTK_LOG_INFO,  "msg %d rx_ep\n",msg);
		re_trigger = 0;
		usbh_cdc_ecm_at_bulk_in(host, &re_trigger);
		if (re_trigger) {
			usbh_notify_class_state_change(host, atcmd->rx_ep.pipe_id);
		}
	}

	return HAL_OK;
}


/* Exported functions public api ----------------------------------------*/
/**
  * @brief  get the mac str, if the dongle support the standard CDC ECM
  * @retval mac string,the length is 6 Bytes
  */
const u8 *usbh_cdc_ecm_process_mac_str(void)
{
	u8 i = 0;
	usbh_cdc_ecm_appx_t *pappx_cmd = &appx_cmd;
	while (!pappx_cmd->mac_valid && i < 10) {
		usb_os_sleep_ms(1000);
		i++;
		RTK_LOGS(TAG, RTK_LOG_INFO,  "Wait to get MAC string!\n");
	}
	return pappx_cmd->mac;
}

/**
  * @brief  send the data out throught at port endpoint
  * @retval return the status 0 send success,else fail
  */
u8 usbh_cdc_ecm_appx_send_data(u8 *buf, u32 len)
{
	u8 ret = HAL_TIMEOUT;
	u32 retry_cnt = 0;
	RTK_LOGS(TAG, RTK_LOG_INFO,  "Try to send msg(%s)\n", buf);

	while (1) {
		ret = usbh_cdc_ecm_appx_send(buf, len, &(appx_cmd.tx_ep));
		if (ret == HAL_OK) {
			break;
		}
		if (++retry_cnt > 100) { //check 1s
			RTK_LOGS(TAG, RTK_LOG_INFO,  "TX drop ! \n");
			ret = HAL_TIMEOUT;
			break;
		} else  {
			usb_os_sleep_ms(10);
		}
	}

	//wait reply success,wait signal
	if (0 == HAL_OK) {
		appx_cmd.acm_tx_block = 1;
		usb_os_sema_take(appx_cmd.acm_tx_sema, USB_OS_SEMA_TIMEOUT);
	}

	appx_cmd.acm_tx_block = 0;
	RTK_LOGS(TAG, RTK_LOG_INFO,  "Send msg(%s) success \n", buf);
	return ret;
}
