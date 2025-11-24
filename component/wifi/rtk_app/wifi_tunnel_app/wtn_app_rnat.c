/**
  ******************************************************************************
  * @file    wtn_app_rnat.c
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

#define __WTN_APP_RNAT_C__


/* -------------------------------- Includes -------------------------------- */
#include "rtw_autoconf.h"
#include "wifi_api.h"
#include "lwip_netconf.h"
#include "wifi_api.h"
#include "dhcp/dhcps.h"
#include "wtn_app_rnat.h"

#ifdef CONFIG_RMESH_EN
/*dnrd.c will use this*/
extern char *rptssid;
extern int wifi_repeater_ap_config_complete;
extern struct table ip_table; /*for rnat search client ip*/
rtos_task_t rnat_ap_start_task_hdl = NULL;
rtos_task_t rnat_poll_ip_task_hdl = NULL;

extern void dns_relay_service_init(void);
extern void ip_nat_reinitialize(void);
extern void ip_nat_sync_dns_serever_data(void);

static void rnat_wifi_stop_ap(void)
{
	u32 addr;
	u32 netmask;
	u32 gw;

	// stop dhcp server
	dhcps_deinit();
	addr = CONCAT_TO_UINT32(GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	netmask = CONCAT_TO_UINT32(NAT_AP_NETMASK_ADDR0, NAT_AP_NETMASK_ADDR1, NAT_AP_NETMASK_ADDR2, NAT_AP_NETMASK_ADDR3);
	gw = CONCAT_TO_UINT32(GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	LwIP_SetIP(NETIF_WLAN_AP_INDEX, addr, netmask, gw);

	wifi_stop_ap();
}

static int rnat_wifi_restart_ap(struct rtw_softap_info *softAP_config, u32 softap_ip, u32 softap_netmask)
{
	u32 addr;
	u32 netmask;
	u32 gw;
	uint8_t iptab[4];
	int timeout = 20;

	rnat_wifi_stop_ap();

	// start ap
	if (wifi_start_ap(softAP_config) < 0) {
		return -1;
	}

	while (1) {
		struct rtw_wifi_setting softapsetting;
		wifi_get_setting(SOFTAP_WLAN_INDEX, &softapsetting);
		if (strlen((const char *)softapsetting.ssid) > 0) {
			if (strcmp((const char *) softapsetting.ssid, (const char *)softAP_config->ssid.val) == 0) {
				break;
			}
		}

		if (timeout == 0) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r%s(%d)ERROR: Start AP timeout!\n", __FUNCTION__, __LINE__);
			return -1;
		}

		rtos_time_delay_ms(1000);
		timeout --;
	}

	iptab[0] = (uint8_t)(softap_ip);
	iptab[1] = (uint8_t)(softap_ip >> 8);
	iptab[2] = (uint8_t)(softap_ip >> 16);
	iptab[3] = (uint8_t)(softap_ip >> 24);
	addr = CONCAT_TO_UINT32(iptab[0], iptab[1], iptab[2], iptab[3]);
	gw = CONCAT_TO_UINT32(iptab[0], iptab[1], iptab[2], 1);

	iptab[0] = (uint8_t)(softap_netmask);
	iptab[1] = (uint8_t)(softap_netmask >> 8);
	iptab[2] = (uint8_t)(softap_netmask >> 16);
	iptab[3] = (uint8_t)(softap_netmask >> 24);
	netmask = CONCAT_TO_UINT32(iptab[0], iptab[1], iptab[2], iptab[3]);

	LwIP_SetIP(NETIF_WLAN_AP_INDEX, addr, netmask, gw);
	// start dhcp server
	dhcps_init(pnetif_ap);
#if defined(CONFIG_IP6_RLOCAL) && (CONFIG_IP6_RLOCAL == 1)
	LwIP_AUTOIP_IPv6(NETIF_WLAN_AP_INDEX);
#endif
	return 0;
}

static int rnat_avoid_confliction_ip(void)
{
	struct rtw_wifi_setting setting;
	struct rtw_softap_info softAP_config = {0};
	u32 wlan1_ip, wlan1_mask, wlan0_ip, wlan0_mask, maskVal;
	u32 bitmask = 0;
	int i = 0;

	wlan0_ip = *((unsigned int *)(LwIP_GetIP(NETIF_WLAN_STA_INDEX)));
	wlan0_mask = *((unsigned int *)(LwIP_GetMASK(NETIF_WLAN_STA_INDEX)));

	wlan1_ip = *((unsigned int *)(LwIP_GetIP(NETIF_WLAN_AP_INDEX)));
	wlan1_mask = *((unsigned int *)(LwIP_GetMASK(NETIF_WLAN_AP_INDEX)));

	memcpy(&maskVal, wlan1_mask > wlan0_mask ? &wlan0_mask : &wlan1_mask, 4);

	if ((wlan0_ip & maskVal) == (wlan1_ip & maskVal)) { //wan ip conflict lan ip
		for (i = 0; i < 31; i++) {
			bitmask = (u32)htonl(1 << i);
			if ((maskVal & bitmask) != 0) {
				break;
			}
		}

		if ((wlan1_ip & bitmask) == 0) {
			wlan1_ip = wlan1_ip | bitmask;
		} else {
			wlan1_ip = wlan1_ip & (~bitmask);
		}

		wifi_get_setting(SOFTAP_WLAN_INDEX, &setting);
		softAP_config.ssid.len = strlen((char *)setting.ssid);
		memcpy(softAP_config.ssid.val, setting.ssid, softAP_config.ssid.len);
		softAP_config.password = setting.password;
		softAP_config.password_len = strlen((char *)setting.password);
		softAP_config.security_type = setting.security_type;
		softAP_config.channel = setting.channel;

		if (rnat_wifi_restart_ap(&softAP_config, wlan1_ip, wlan1_mask) < 0) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\rERROR: wifi_restart_ap Operation failed!");
		}
	}

	return 0;
}

static void rnat_poll_ip_changed_thread(void *param)
{
	(void) param;
	unsigned int oldip, newip;
	memcpy(&oldip, LwIP_GetIP(NETIF_WLAN_STA_INDEX), 4);

	while (1) {
		memcpy(&newip, LwIP_GetIP(NETIF_WLAN_STA_INDEX), 4);
		if (0x0 == newip) {
			goto nextcheck;
		}

		if (1 == wifi_repeater_ap_config_complete && oldip != newip) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s(%d)oldip=%x,newip=%x\n", __FUNCTION__, __LINE__, oldip, newip);
			ip_nat_reinitialize();
			ip_nat_sync_dns_serever_data();
			rnat_avoid_confliction_ip();
			oldip = newip;
		}

nextcheck:
		rtos_time_delay_ms(1000);
	}

	rtos_task_delete(NULL);
}

static void rnat_ap_start_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;
	u32 ip_addr;
	u32 netmask;
	u32 gw;
	struct rtw_wifi_setting wifi_setting = {0};
	struct rtw_softap_info softap_config = {0};
	u8 timeout = 20;

	/*step1: check if STA port connected to AP successfully*/
	while (1) {
		if (LwIP_Check_Connectivity(NETIF_WLAN_STA_INDEX) == CONNECTION_VALID) {
			break;
		}
		rtos_time_delay_ms(200);
	}

	/*step2: get STA port setting and start softap with same ssid and password*/
	wifi_get_setting(STA_WLAN_INDEX, &wifi_setting);
	if (wifi_setting.security_type & WPA_SECURITY) {/*softap only support WPA2, currently not support WPA*/
		softap_config.security_type = (wifi_setting.security_type & (~WPA_SECURITY)) | (WPA2_SECURITY);
	} else {
		softap_config.security_type = wifi_setting.security_type;
	}
	if (wifi_setting.security_type != RTW_SECURITY_OPEN) {
		u8 softap_password[RTW_MAX_PSK_LEN] = {0};
		softap_config.password_len = strlen((char *)wifi_setting.password);
		memcpy(softap_password, wifi_setting.password, softap_config.password_len);
		softap_config.password = softap_password;
	}
	softap_config.channel = wifi_setting.channel;
	softap_config.ssid.len = strlen((char *)wifi_setting.ssid);
	memcpy(softap_config.ssid.val, wifi_setting.ssid, softap_config.ssid.len);
	if (rptssid == NULL) {
		rptssid = (char *)rtos_mem_zmalloc(RTW_ESSID_MAX_SIZE + 1);
	}
	memcpy(rptssid, wifi_setting.ssid, softap_config.ssid.len);
	if (wifi_start_ap(&softap_config) < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r[RMESH NAT] ERROR: wifi_start_ap failed\n");
		goto exit;
	}

	while (timeout > 0) {
		wifi_get_setting(SOFTAP_WLAN_INDEX, &wifi_setting);
		if (strlen((char *)wifi_setting.ssid) > 0) {
			if (strcmp((char *)wifi_setting.ssid, (char *)softap_config.ssid.val) == 0) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[RMESH NAT] %s started\n", softap_config.ssid.val);
				break;
			}
		}

		rtos_time_delay_ms(1000);
		timeout--;
	}


	ip_addr = CONCAT_TO_UINT32(NAT_AP_IP_ADDR0, NAT_AP_IP_ADDR1, NAT_AP_IP_ADDR2, NAT_AP_IP_ADDR3);
	netmask = CONCAT_TO_UINT32(NAT_AP_NETMASK_ADDR0, NAT_AP_NETMASK_ADDR1, NAT_AP_NETMASK_ADDR2, NAT_AP_NETMASK_ADDR3);
	gw = CONCAT_TO_UINT32(NAT_AP_GW_ADDR0, NAT_AP_GW_ADDR1, NAT_AP_GW_ADDR2, NAT_AP_GW_ADDR3);
	LwIP_SetIP(NETIF_WLAN_AP_INDEX, ip_addr, netmask, gw);

	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r[RMESH NAT] Start DHCP server\n");
	dhcps_init(pnetif_ap);

#if defined(CONFIG_IP6_RLOCAL) && (CONFIG_IP6_RLOCAL == 1)
	LwIP_AUTOIP_IPv6(NETIF_WLAN_AP_INDEX);
#endif
	rtos_time_delay_ms(1000);

	wifi_repeater_ap_config_complete = 1;
exit:
	rnat_ap_start_task_hdl = NULL;
	rtos_task_delete(NULL);
}

u8 wtn_rnat_search_client_ip(u8 *src_mac)
{
	return dhcps_search_client_ip(src_mac);
}
#endif

void wtn_rnat_ap_init(u8 enable)
{
#ifdef CONFIG_RMESH_EN
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "RNAT AP INIT=%d\n", enable);
	if (enable) {
		wifi_repeater_ap_config_complete = 0;

		if (rnat_ap_start_task_hdl == NULL) {
			if (rtos_task_create(&rnat_ap_start_task_hdl, ((const char *)"rnat_start_thread"), rnat_ap_start_thread, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS,  "\n\r%s rtos_task_create failed\n", __FUNCTION__);
			}
		}
		if (rnat_poll_ip_task_hdl == NULL) {
			if (rtos_task_create(&rnat_poll_ip_task_hdl, ((const char *)"rnat_poll_ip_changed_thread"), rnat_poll_ip_changed_thread, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r%s rtos_task_create failed\n", __FUNCTION__);
			}
			dns_relay_service_init();
		}
	} else {
		wifi_repeater_ap_config_complete = 0;
		rnat_wifi_stop_ap();
	}
#else
	UNUSED(enable);
#endif
}
