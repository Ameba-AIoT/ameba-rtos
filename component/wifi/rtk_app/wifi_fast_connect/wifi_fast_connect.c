/**
  ******************************************************************************
  * @file    wifi_fast_connect.c
  * @author
  * @version
  * @date
  * @brief   This example demonstrate how to implement wifi fast reconnection
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include <platform_autoconf.h>
#include <wifi_fast_connect.h>

#include "os_wrapper.h"
#include "flash_api.h"
#ifdef CONFIG_LWIP_LAYER
#include <lwip_netconf.h>
extern struct netif xnetif[NET_IF_NUM];
#endif
#include "kv.h"

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
uint32_t offer_ip = 0;
uint32_t server_ip = 0;
#endif
#define WIFI_RETRYCOUNT 5
#define WIFI_FAST_CON_PARTION_SIZE 0x400
#define TOTAL_PARTION_SIZE 0x1000

int wifi_check_fast_connect_data(struct wlan_fast_reconnect *data);

/*
* Usage:
*       wifi connection indication trigger this function to save current
*       wifi profile in flash
*/

int write_fast_connect_data_to_flash(unsigned int offer_ip, unsigned int server_ip)
{
	/* To avoid gcc warnings */
#if(!defined(CONFIG_FAST_DHCP) || (!CONFIG_FAST_DHCP))
	(void) offer_ip;
	(void) server_ip;
#endif
	struct wlan_fast_reconnect read_data = {0};
	struct wlan_fast_reconnect wifi_data_to_flash = {0};
	struct rtw_wifi_setting setting;
	struct psk_info PSK_info;
	u32 channel = 0;

	/* STEP1: get current connect info from wifi driver*/
	if (wifi_get_setting(STA_WLAN_INDEX, &setting) || setting.mode == RTW_MODE_AP) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\r\n %s():wifi_get_setting fail or ap mode", __func__);
		return RTK_FAIL;
	}

	memset(&wifi_data_to_flash, 0, sizeof(struct wlan_fast_reconnect));
	channel = (u32)setting.channel;

	switch (setting.security_type) {
	case RTW_SECURITY_OPEN:
	case RTW_SECURITY_WPA3_OWE:
		memcpy(wifi_data_to_flash.psk_essid, setting.ssid, strlen((const char *)setting.ssid));
		wifi_data_to_flash.security_type = RTW_SECURITY_OPEN;
		break;
	case RTW_SECURITY_WEP_PSK:
	case RTW_SECURITY_WEP_SHARED:
		memcpy(wifi_data_to_flash.psk_essid, setting.ssid, strlen((const char *)setting.ssid));
		memcpy(wifi_data_to_flash.psk_passphrase, setting.password, strlen((const char *)setting.password));
		channel |= (setting.key_idx) << 28;
		wifi_data_to_flash.security_type = RTW_SECURITY_WEP_SHARED;
		break;
	case RTW_SECURITY_WPA_TKIP_PSK:
	case RTW_SECURITY_WPA2_TKIP_PSK:
	case RTW_SECURITY_WPA_WPA2_TKIP_PSK:
	case RTW_SECURITY_WPA_AES_PSK:
	case RTW_SECURITY_WPA_MIXED_PSK:
	case RTW_SECURITY_WPA2_AES_PSK:
	case RTW_SECURITY_WPA2_MIXED_PSK:
	case RTW_SECURITY_WPA_WPA2_AES_PSK:
	case RTW_SECURITY_WPA_WPA2_MIXED_PSK:
#ifdef CONFIG_SAE_SUPPORT
	case RTW_SECURITY_WPA3_AES_PSK:
	case RTW_SECURITY_WPA2_WPA3_MIXED:
#endif
		memset(&PSK_info, 0, sizeof(struct psk_info));
		rtw_psk_get_psk_info(&PSK_info);
		memcpy(wifi_data_to_flash.psk_essid, PSK_info.psk_essid, sizeof(wifi_data_to_flash.psk_essid));
		memcpy(wifi_data_to_flash.psk_passphrase, PSK_info.psk_passphrase, sizeof(wifi_data_to_flash.psk_passphrase));
		memcpy(wifi_data_to_flash.wpa_global_PSK, PSK_info.wpa_global_PSK, sizeof(wifi_data_to_flash.wpa_global_PSK));
		wifi_data_to_flash.security_type = setting.security_type;
		wifi_data_to_flash.is_wps_trigger = setting.is_wps_trigger;
		break;

	default:
		break;
	}
	memcpy(&(wifi_data_to_flash.channel), &channel, 4);
#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
	wifi_data_to_flash.offer_ip = offer_ip;
	wifi_data_to_flash.server_ip = server_ip;
#endif

	/* STEP2: get last time fast connect info from flash*/
	memset(&read_data, 0xff, sizeof(struct wlan_fast_reconnect));
#ifndef CONFIG_ZEPHYR_SDK
	rt_kv_get("wlan_data", (uint8_t *) &read_data, sizeof(struct wlan_fast_reconnect));
#endif

	/* STEP3: wirte new connect info to flash if different content: SSID, Passphrase, Channel, Security type*/
	if (memcmp((u8 *) &wifi_data_to_flash, (u8 *) &read_data, sizeof(struct wlan_fast_reconnect)) != 0) {
#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
		DiagPrintf("\r\n %s():not the same ssid/passphrase/channel/offer_ip, write new profile to flash \n", __func__);
#else
		DiagPrintf("\r\n %s():not the same ssid/passphrase/channel, write new profile to flash \n", __func__);
#endif
#ifndef CONFIG_ZEPHYR_SDK
		rt_kv_set("wlan_data", (uint8_t *)&wifi_data_to_flash, sizeof(struct wlan_fast_reconnect));
#endif
	}

	return RTK_SUCCESS;
}

/*
* Usage:
*       This function read previous saved wlan profile in flash and execute connection.
*
*/
int wifi_do_fast_connect(void)
{
	struct wlan_fast_reconnect *data;
	uint32_t	channel;
	uint32_t    security_type;
	u8 key_id;
	int ret;
#ifndef CONFIG_ENABLE_WPS
	int wifi_retry_connect = WIFI_RETRYCOUNT;	// For fast wifi connect retry
#else
	int wifi_retry_connect = 0;	// For fast wifi connect retry times in WPS function
#endif
	struct rtw_network_info wifi = {0};
	struct psk_info PSK_INFO = {0};

#ifdef CONFIG_LWIP_LAYER
	netifapi_netif_set_up(&xnetif[0]);
#endif

	data = (struct wlan_fast_reconnect *)malloc(sizeof(struct wlan_fast_reconnect));
	if (data) {
		memset(data, 0xff, sizeof(struct wlan_fast_reconnect));
#ifndef CONFIG_ZEPHYR_SDK
		ret = rt_kv_get("wlan_data", (uint8_t *)data, sizeof(struct wlan_fast_reconnect));
#endif
		if (ret < 0) {
			DiagPrintf("[FAST_CONNECT] Fast connect profile is not exist\n");
			return 0;
		}

		/* Check whether stored flash profile is empty */
		struct wlan_fast_reconnect *empty_data;
		empty_data = (struct wlan_fast_reconnect *)malloc(sizeof(struct wlan_fast_reconnect));
		if (empty_data) {
			memset(empty_data, 0xff, sizeof(struct wlan_fast_reconnect));
			if (memcmp(empty_data, data, sizeof(struct wlan_fast_reconnect)) == 0) {
				DiagPrintf("[FAST_CONNECT] Fast connect profile is empty, abort fast connection\n");
				free(data);
				free(empty_data);
				return 0;
			}
			free(empty_data);
		}

		memset(&PSK_INFO, 0, sizeof(struct psk_info));
		PSK_INFO.security_type = data->security_type;
		memcpy(PSK_INFO.psk_essid, data->psk_essid, sizeof(data->psk_essid));
		memcpy(PSK_INFO.psk_passphrase, data->psk_passphrase, sizeof(data->psk_passphrase));
		memcpy(PSK_INFO.wpa_global_PSK, data->wpa_global_PSK, sizeof(data->wpa_global_PSK));

		if (wifi_check_fast_connect_data(data) < 0) {
			DiagPrintf("[FAST_CONNECT] Fast connect profile is wrong, abort fast connection\n");
			free(data);
			return -1;
		}
		PSK_INFO.index = STA_WLAN_INDEX;
		rtw_psk_set_psk_info(&PSK_INFO);

		channel = data->channel;
		key_id = channel >> 28;
		channel &= 0xff;
		security_type = data->security_type;
		//set partial scan for entering to listen beacon quickly
WIFI_RETRY_LOOP:
		wifi.channel = channel;
		wifi.pscan_option = RTW_PSCAN_FAST_SURVEY;
		wifi.security_type = (enum rtw_security)security_type;
		//SSID
		wifi.ssid.len = strlen((char *)(data->psk_essid));
		if (wifi.ssid.len > 32) {
			DiagPrintf("[FAST_CONNECT] SSID length can't exceed 32\n\r");
			free(data);
			return -1;
		}
		strncpy((char *)wifi.ssid.val, (char *)(data->psk_essid), sizeof(wifi.ssid.val) - 1);

#ifdef CONFIG_WPS
		wifi.is_wps_trigger = data->is_wps_trigger;
#endif

		switch (security_type) {
		case RTW_SECURITY_WEP_PSK:
		case RTW_SECURITY_WEP_SHARED:
			wifi.password = (unsigned char *)(data->psk_passphrase);
			wifi.password_len = strlen((char *)(data->psk_passphrase));
			wifi.key_id = key_id;
			break;
		case RTW_SECURITY_WPA_TKIP_PSK:
		case RTW_SECURITY_WPA2_TKIP_PSK:
		case RTW_SECURITY_WPA_WPA2_TKIP_PSK:
		case RTW_SECURITY_WPA_AES_PSK:
		case RTW_SECURITY_WPA_MIXED_PSK:
		case RTW_SECURITY_WPA2_AES_PSK:
		case RTW_SECURITY_WPA2_MIXED_PSK:
		case RTW_SECURITY_WPA_WPA2_AES_PSK:
		case RTW_SECURITY_WPA_WPA2_MIXED_PSK:
#ifdef CONFIG_SAE_SUPPORT
		case RTW_SECURITY_WPA3_AES_PSK:
		case RTW_SECURITY_WPA2_WPA3_MIXED:
#endif
			wifi.password = (unsigned char *)(data->psk_passphrase);
			wifi.password_len = strlen((char *)(data->psk_passphrase));
			break;
		default:
			break;
		}

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
		offer_ip = data->offer_ip;
		server_ip = data->server_ip;
#endif

		ret = wifi_connect(&wifi, 1);
		if (ret != RTK_SUCCESS) {
			wifi_retry_connect--;
			if (wifi_retry_connect > 0) {
				/* Add the delay to wait for the _rtw_join_timeout_handler
				 * If there is no this delay, there are some error when rhe AP
				 * send the disassociation frame. It will cause the connection
				 * to be failed at first time after resetting. So keep 300ms delay
				 * here. For the detail about this error, please refer to
				 * [RSWLANDIOT-1954].
				 */
				rtos_time_delay_ms(300);
				DiagPrintf("wifi retry for fast connect.\r\n");
				goto WIFI_RETRY_LOOP;
			}
		}
#ifdef CONFIG_LWIP_LAYER
		if (ret == RTK_SUCCESS) {
			LwIP_DHCP(0, DHCP_START);
		}
#endif
		free(data);
	}

	return 0;
}

/*
* Usage:
*       After wifi connected, check if it is the same AP which previously stored in flash.
*       Return 0 means not the same AP or FASH DHCP not enabled, return 1 means the same AP.
*/
int check_is_the_same_ap()
{
	int ret = 0;
#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
	if (p_store_fast_connect_info != NULL) {
		struct wlan_fast_reconnect data;
		struct rtw_wifi_setting setting;

		if (wifi_get_setting(STA_WLAN_INDEX, &setting) || setting.mode == RTW_MODE_AP) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\r\n %s():wifi_get_setting fail or ap mode", __func__);
			return 0;
		}

		memset(&data, 0xff, sizeof(struct wlan_fast_reconnect));
		rt_kv_get("wlan_data", (uint8_t *)&data, sizeof(struct wlan_fast_reconnect));

		if (strncmp((const char *)data.psk_essid, (const char *)setting.ssid, strlen((char const *)setting.ssid)) == 0) {
			ret = 1;
		} else {
			ret = 0;
		}
	}
#endif
	return ret;
}

/*
* Usage:
*       config fast connect related function.
*       input param: enable, set to 1 means enable fast connect, set to 0 means disable fast connect
*/
void wifi_fast_connect_enable(u8 enable)
{
#if defined(CONFIG_MP_INCLUDED)	//always disable fastconnect in mp
	enable = 0;
#endif
	if (enable == 0) {
		p_wifi_do_fast_connect = NULL;
		p_store_fast_connect_info = NULL;
	} else {
		// this function will be called after wlan init done to read info from flash and do fast connect
		p_wifi_do_fast_connect = wifi_do_fast_connect;

		// this function will be called after wifi_connection success to store connect info to flash
		p_store_fast_connect_info = write_fast_connect_data_to_flash;
	}
}

/*
* Usage:
*       This function load previous saved ip in flash for fast dhcp.
*
*/
void wifi_fast_connect_load_fast_dhcp(void)
{
#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
	struct wlan_fast_reconnect *data;
	data = (struct wlan_fast_reconnect *)malloc(sizeof(struct wlan_fast_reconnect));
	if (data) {
		memset(data, 0xff, sizeof(struct wlan_fast_reconnect));
		rt_kv_get("wlan_data", (uint8_t *)data, sizeof(struct wlan_fast_reconnect));

		/* Check whether stored flash profile is empty */
		struct wlan_fast_reconnect *empty_data;
		empty_data = (struct wlan_fast_reconnect *)malloc(sizeof(struct wlan_fast_reconnect));
		if (empty_data) {
			memset(empty_data, 0xff, sizeof(struct wlan_fast_reconnect));
			if (memcmp(empty_data, data, sizeof(struct wlan_fast_reconnect)) == 0) {
				free(data);
				free(empty_data);
				return;
			}
			free(empty_data);
		}

		offer_ip = data->offer_ip;
		server_ip = data->server_ip;
		free(data);
	}
#endif
}

int wifi_check_fast_connect_data(struct wlan_fast_reconnect *data)
{
	//Check SSID
	int ret = 0;
	if ((strlen((const char *)data->psk_essid) > 32) || (strlen((const char *)data->psk_essid) == 0)) {
		ret = -1;
		return ret;
	}

	//Check Password
	if (strlen((const char *)data->psk_passphrase) > 64) {
		ret = -1;
		return ret;
	}

	return ret;
}
