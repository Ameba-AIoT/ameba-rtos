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

#include "rtw_wifi_constants.h"
#include "wifi_conf.h"
#include "wpa_lite_intf.h"

#ifdef CONFIG_WLAN
#define RTW_PASSPHRASE_MAX_SIZE RTW_PASSPHRASE_WPA3_SIZE
//#define FAST_RECONNECT_DATA (0x80000 - 0x1000)
#define RTW_ESSID_MAX_SIZE         32
#define A_SHA_DIGEST_LEN		20


struct wlan_fast_reconnect {
	unsigned char version;
	unsigned char psk_essid[RTW_ESSID_MAX_SIZE + 4];
	unsigned char psk_passphrase[RTW_PASSPHRASE_MAX_SIZE + 1];
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

//Function
extern int (*p_wifi_do_fast_connect)(void);
extern int (*p_store_fast_connect_info)(unsigned int data1, unsigned int data2);
int check_is_the_same_ap(void);
void wifi_fast_connect_enable(unsigned char enable);
void wifi_fast_connect_load_fast_dhcp(void);
int wifi_check_fast_connect_data(struct wlan_fast_reconnect *data);
#endif
#endif //#ifndef __WIFI_FAST_CONNECT__
