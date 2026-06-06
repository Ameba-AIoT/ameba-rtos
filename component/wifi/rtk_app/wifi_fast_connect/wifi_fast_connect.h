/**
  ******************************************************************************
  * @file    wifi_fast_connect.h
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef __WIFI_FAST_CONNECT_H__
#define __WIFI_FAST_CONNECT_H__

#include "wifi_api.h"
#include "wpa_lite_intf.h"

#ifdef CONFIG_WLAN
//#define FAST_RECONNECT_DATA (0x80000 - 0x1000)
#define A_SHA_DIGEST_LEN		20


struct wlan_fast_reconnect {
	unsigned char version;
	unsigned char psk_essid[RTW_ESSID_MAX_SIZE + 1];
	unsigned char psk_passphrase[RTW_MAX_PSK_LEN + 1];
	unsigned char wpa_global_PSK[A_SHA_DIGEST_LEN * 2];
	unsigned int  channel;
	unsigned int    security_type;
#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
	unsigned int offer_ip;
	unsigned int server_ip;
#endif
#ifdef CONFIG_WPS
	unsigned char is_wps_trigger;
#endif
};

struct fast_conn_write_flash {
	struct wlan_fast_reconnect     read_data;
	struct wlan_fast_reconnect     wifi_data_to_flash;
	struct rtw_wifi_setting        setting;
	struct psk_info                PSK_info;
	u32                            channel;
};

struct do_fast_conn {
	struct wlan_fast_reconnect     fast_reconn_data;
	struct rtw_network_info        wifi;
	struct psk_info                PSK_INFO;
	int                            wifi_retry_connect;
	u32	                           channel;
	u32                            security_type;
	u8                             key_id;
};
//Function
extern int (*p_wifi_do_fast_connect)(void);
extern int (*p_store_fast_connect_info)(unsigned int data1, unsigned int data2);
int check_is_the_same_ap(void);
void wifi_fast_connect_enable(unsigned char enable);
void wifi_fast_connect_load_fast_dhcp(void);
int wifi_check_fast_connect_data(struct wlan_fast_reconnect *data);
#endif
#endif //#ifndef __WIFI_FAST_CONNECT__
