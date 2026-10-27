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
#include "dns_proxy.h"
#include "rnat_netif.h"
#include "lwip_ipnat.h"

#ifdef CONFIG_RNAT_EN
rtos_task_t rnat_ap_start_task_hdl = NULL;

static rnat_ip_info_t g_ap_custom_ip = {
	.ip = { .addr = PP_HTONL(LWIP_MAKEU32(10, 200, 1, 1)) },
	.gw = { .addr = PP_HTONL(LWIP_MAKEU32(10, 200, 1, 1)) },
	.netmask = { .addr = PP_HTONL(LWIP_MAKEU32(255, 255, 255, 0)) }
};

static rnat_wifi_config_t g_ap_wifi_config = {0};

const rnat_netif_config_t g_rnat_sta_netif_config = {
	.role = RNAT_ROLE_WAN,
	.ip_method = RNAT_IP_METHOD_DHCP_CLIENT,
	.priority = RNAT_ROUTE_PRIO_WAN_STA,
	.ip_info = NULL,
	.if_desc = "STA",
	.status_callback = NULL,
	.callback_user_data = NULL
};

const rnat_netif_config_t g_rnat_ap_netif_config = {
	.role = RNAT_ROLE_LAN,
	.ip_method = RNAT_IP_METHOD_DHCP_SERVER,
	.priority = RNAT_ROUTE_PRIO_LAN,
	.ip_info = &g_ap_custom_ip,  /* Use custom static IP */
	.if_desc = "AP",
	.status_callback = NULL,
	.callback_user_data = NULL
};

static void rnat_wifi_stop_ap(void)
{
	u32 addr;
	u32 netmask;
	u32 gw;

	// stop dhcp server
	dhcps_deinit(pnetif_ap);
	addr = CONCAT_TO_UINT32(GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	netmask = CONCAT_TO_UINT32(NAT_AP_NETMASK_ADDR0, NAT_AP_NETMASK_ADDR1, NAT_AP_NETMASK_ADDR2, NAT_AP_NETMASK_ADDR3);
	gw = CONCAT_TO_UINT32(GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	lwip_set_ip(NETIF_WLAN_AP_INDEX, addr, netmask, gw);

	wifi_stop_ap();
}


static void rnat_ap_start_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;
	struct rtw_wifi_setting wifi_setting = {0};
	struct rtw_wifi_setting ap_setting = {0};
	u8 *sta_ip;

	/*step1: check if STA port connected to AP successfully*/
	while (1) {
		if (lwip_check_connectivity(NETIF_WLAN_STA_INDEX) == CONNECTION_VALID) {
			break;
		}
		rtos_time_delay_ms(200);
	}

	/* softap already running with the same ssid/password as the STA's connected AP: nothing to start */
	wifi_get_setting(STA_WLAN_INDEX, &wifi_setting);
	rnat_netif_t *ap_netif = rnat_netif_get(lwip_idx_get_netif(NETIF_WLAN_AP_INDEX));
	if (wifi_is_running(SOFTAP_WLAN_INDEX) == TRUE) {
		wifi_get_setting(SOFTAP_WLAN_INDEX, &ap_setting);
		if (!strcmp((char *)ap_setting.ssid, (char *)wifi_setting.ssid) &&
			(wifi_setting.security_type == RTW_SECURITY_OPEN ? ap_setting.security_type == RTW_SECURITY_OPEN :
			 !strcmp((char *)ap_setting.password, (char *)wifi_setting.password))) {
			goto exit;/*restart softap only when realAP change*/
		}

		if (ap_netif) {/*rnat already started，no need do dns_proxy/rnat_init again*/
			u8 *ap_ip = lwip_get_ip(NETIF_WLAN_AP_INDEX);
			if (ap_ip) {
				etharp_remove_static_entry((ip4_addr_t *)ap_ip);
			}
			goto restart_ap;
		}
	}

	rnat_netif_t *sta_netif = rnat_netif_get(lwip_idx_get_netif(NETIF_WLAN_STA_INDEX));
	if (sta_netif == NULL) {
		/* Initialize and start DNS Proxy */
		dns_proxy_init(DNS_PROXY_CACHE_MAX_ENTRIES, DNS_PROXY_TX_MAX_PENDING);
		dns_proxy_start(DNS_PROXY_LISTEN_PORT);
		/* Initialize R-NAT system */
		rnat_init();

		sta_netif = rnat_netif_create(NETIF_WLAN_STA_INDEX, &g_rnat_sta_netif_config);
		/* just now, so the one-shot LWIP_NSC_LINK_CHANGED event was missed and is_active stayed false;
		* without this the STA (prio 102) loses to the AP (prio 0) in rnat_update_default_gw()
		*/
		if (sta_netif) {
			sta_netif->is_active = true;
		}
	}

restart_ap:
	/*step2: softap ssid = STA port's connected ap ssid + last byte of STA port's mac (hex)*/
	strncpy(g_ap_wifi_config.ssid, (char *)wifi_setting.ssid, sizeof(g_ap_wifi_config.ssid) - 1);
	g_ap_wifi_config.ssid[sizeof(g_ap_wifi_config.ssid) - 1] = '\0';
	strncpy(g_ap_wifi_config.password, (char *)wifi_setting.password, sizeof(g_ap_wifi_config.password) - 1);
	g_ap_wifi_config.channel = wifi_setting.channel;

	/*step3: softap ip/gw: only the 3rd byte of g_ap_custom_ip.ip is replaced by the last byte of STA port's ip*/
	sta_ip = lwip_get_ip(NETIF_WLAN_STA_INDEX);
	((u8 *)&g_ap_custom_ip.ip.addr)[2] = sta_ip[3];
	g_ap_custom_ip.gw = g_ap_custom_ip.ip;

	ap_netif = rnat_netif_get(lwip_idx_get_netif(NETIF_WLAN_AP_INDEX));
	if (!ap_netif) {
		ap_netif = rnat_netif_create(NETIF_WLAN_AP_INDEX, &g_rnat_ap_netif_config);
	} else {
		/* rnat_netif_create() only applies ip_info once; on restart apply the new subnet before DHCP server is re-inited */
		struct ip_addr check_ip;
		ip_addr_copy_from_ip4(check_ip, g_ap_custom_ip.ip);
		if (!lwip_subnet_is_used(&check_ip, ap_netif->lwip_netif)) {
			netifapi_netif_set_addr(ap_netif->lwip_netif, &g_ap_custom_ip.ip, &g_ap_custom_ip.netmask, &g_ap_custom_ip.gw);
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_WARN, "AP custom subnet conflicts, auto-allocating\n");
			if (lwip_alloc_ip(NETIF_WLAN_AP_INDEX) != 0) {
				goto exit;
			}
		}
	}
	if (rnat_netif_start(ap_netif, (void *)&g_ap_wifi_config) < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r%s(%d)ERROR: Start AP failed!\n", __FUNCTION__, __LINE__);
		goto exit;
	}

#ifdef CONFIG_WTN_RNAT_INTERCOM
	wtn_rnat_intercom_set_self_gw();
#endif

#if defined(CONFIG_IP6_RLOCAL) && (CONFIG_IP6_RLOCAL == 1)
	lwip_autoip_ipv6(NETIF_WLAN_AP_INDEX);
#endif
	rtos_time_delay_ms(1000);

exit:
	rnat_ap_start_task_hdl = NULL;
	rtos_task_delete(NULL);
}

u8 wtn_rnat_search_client_ip(u8 *src_mac)
{
	return dhcps_search_client_ip(pnetif_ap, src_mac);
}

#endif

void wtn_rnat_ap_init(u8 enable)
{
#ifdef CONFIG_RNAT_EN
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "RNAT AP INIT=%d\n", enable);
	if (enable) {
		if (rnat_ap_start_task_hdl == NULL) {
			if (rtos_task_create(&rnat_ap_start_task_hdl, ((const char *)"rnat_start_thread"), rnat_ap_start_thread, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS,  "\n\r%s rtos_task_create failed\n", __FUNCTION__);
			}
		}
#ifdef CONFIG_WTN_RNAT_INTERCOM
		if (wifi_user_config.wtn_rnat_intercom_en) {
			wtn_rnat_intercom_start();
		}
#endif
	} else {
#ifdef CONFIG_WTN_RNAT_INTERCOM
		if (wifi_user_config.wtn_rnat_intercom_en) {
			return;
		}
#endif
		rnat_wifi_stop_ap();
	}
#else
	UNUSED(enable);
#endif
}
