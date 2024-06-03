/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"
#include "os_wrapper.h"
#include "log_service.h"
#include "atcmd_wifi.h"
#ifdef CONFIG_LWIP_LAYER
#include <lwip_netconf.h>
#include <dhcp/dhcps.h>
#endif
#if CONFIG_WLAN
#include <wifi_conf.h>
#include <wifi_intf_drv_to_upper.h>
#endif

/******************************************************************************/
#define	_AT_WLAN_SET_SSID_          "ATW0"
#define	_AT_WLAN_SET_PASSPHRASE_    "ATW1"
#define	_AT_WLAN_SET_KEY_ID_        "ATW2"
#define	_AT_WLAN_AP_SET_SSID_       "ATW3"
#define	_AT_WLAN_AP_SET_SEC_KEY_    "ATW4"
#define	_AT_WLAN_AP_SET_CHANNEL_    "ATW5"
#define _AT_WLAN_SET_BSSID_         "ATW6"
#define _AT_WLAN_SET_WPA_MODE_      "ATW8"
#define	_AT_WLAN_AP_ACTIVATE_       "ATWA"
#define _AT_WLAN_AP_STA_ACTIVATE_   "ATWB"
#define	_AT_WLAN_JOIN_NET_          "ATWC"
#define	_AT_WLAN_DISC_NET_          "ATWD"
#define	_AT_WLAN_WEB_SERVER_        "ATWE"
#define _AT_WLAN_P2P_FIND_          "ATWF"
#define _AT_WLAN_P2P_START_         "ATWG"
#define _AT_WLAN_P2P_STOP_          "ATWH"
#define _AT_WLAN_PING_TEST_         "ATWI"
#define _AT_WLAN_P2P_CONNECT_       "ATWJ"
#define _AT_WLAN_P2P_DISCONNECT_    "ATWK"
#define _AT_WLAN_PROMISC_           "ATWM"
#define _AT_WLAN_P2P_INFO_          "ATWN"
#define _AT_WLAN_OTA_UPDATE_        "ATWO"
#define	_AT_WLAN_POWER_             "ATWP"
#define	_AT_WLAN_SIMPLE_CONFIG_     "ATWQ"
#define	_AT_WLAN_GET_RSSI_          "ATWR"
#define	_AT_WLAN_SCAN_              "ATWS"
#define _AT_WLAN_SCAN_WITH_SSID_    "ATWs"
#define _AT_WLAN_TCP_TEST_          "ATWT"
#define _AT_WLAN_UDP_TEST_          "ATWU"
#define _AT_WLAN_WPS_               "ATWW"
#define _AT_WLAN_AP_WPS_            "ATWw"
#define _AT_WLAN_AIRKISS_           "ATWX"
#define _AT_WLAN_SET_MESH_GROUP_    "ATGP"
#define _AT_WLAN_IWPRIV_            "ATWZ"
#define	_AT_WLAN_INFO_              "ATW?"
#define	_AT_WLAN_FLUSH_CONNCTION_INFO_     "ATWf"

#define	_AT_WLAN_EXTEND_POWER_MODE_        "ATXP"

#define SCAN_WITH_SSID		1

#define RTW_BUFFER_UNAVAILABLE_TEMPORARY	9

#ifdef CONFIG_LWIP_LAYER
extern void cmd_iperf(int argc, char **argv);
extern void cmd_ping(int argc, char **argv);
#endif

#ifndef CONFIG_MP_SHRINK

#if CONFIG_WLAN
#if defined(CONFIG_ENABLE_WPS) && CONFIG_ENABLE_WPS
extern void cmd_wps(int argc, char **argv);
#endif

#ifdef CONFIG_AS_INIC_AP
extern int inic_iwpriv_command(char *cmd, unsigned int cmd_len, int show_msg);
#endif
#ifdef CONFIG_LWIP_LAYER
extern struct netif xnetif[NET_IF_NUM];
#endif

#if defined(CONFIG_IP_NAT) && (CONFIG_IP_NAT == 1)
extern void ipnat_dump(void);
#endif

extern int wifi_set_ips_internal(u8 enable);

#define JOIN_CONNECTING             (uint32_t)(1 << 10)

/* fastconnect use wifi AT command. Not init_wifi_struct when log service disabled
 * static initialize all values for using fastconnect when log service disabled
 */
static rtw_network_info_t wifi = {0};

static rtw_softap_info_t ap = {0};
static unsigned char password[129] = {0};
#ifdef CONFIG_FPGA
int security = -1;
#endif

struct static_ip_config user_static_ip;

static void init_wifi_struct(void)
{
	memset(wifi.ssid.val, 0, sizeof(wifi.ssid.val));
	memset(wifi.bssid.octet, 0, ETH_ALEN);
	memset(password, 0, sizeof(password));
	wifi.ssid.len = 0;
	wifi.password = NULL;
	wifi.password_len = 0;
	wifi.key_id = -1;
	wifi.channel = 0;
	wifi.pscan_option = 0;
	wifi.joinstatus_user_callback = NULL;
	memset(ap.ssid.val, 0, sizeof(ap.ssid.val));
	ap.ssid.len = 0;
	ap.password = NULL;
	ap.password_len = 0;
	ap.channel = 1;
	ap.hidden_ssid = 0;
#ifdef CONFIG_FPGA
	security = -1;
#endif
}

static void print_scan_result(rtw_scan_result_t *record)
{
#if WIFI_LOGO_CERTIFICATION_CONFIG
	RTW_API_INFO(""MAC_FMT",", MAC_ARG(record->BSSID.octet));
	RTW_API_INFO("%s,\r\n", record->SSID.val);
#else
	RTW_API_INFO("%s\t ", (record->bss_type == RTW_BSS_TYPE_ADHOC) ? "Adhoc" : "Infra");
	RTW_API_INFO(MAC_FMT, MAC_ARG(record->BSSID.octet));
	RTW_API_INFO(" %d\t ", record->signal_strength);
	RTW_API_INFO(" %d\t  ", record->channel);
	RTW_API_INFO(" %d\t  ", (unsigned int)record->wps_type);
	RTW_API_INFO("%s\t\t ", (record->security == RTW_SECURITY_OPEN) ? "Open               " :
				 (record->security == RTW_SECURITY_WEP_PSK) ? "WEP" :
				 (record->security == RTW_SECURITY_WPA_TKIP_PSK) ? "WPA TKIP" :
				 (record->security == RTW_SECURITY_WPA_AES_PSK) ? "WPA AES" :
				 (record->security == RTW_SECURITY_WPA_MIXED_PSK) ? "WPA Mixed" :
				 (record->security == RTW_SECURITY_WPA2_AES_PSK) ? "WPA2 AES" :
				 (record->security == RTW_SECURITY_WPA2_TKIP_PSK) ? "WPA2 TKIP" :
				 (record->security == RTW_SECURITY_WPA2_MIXED_PSK) ? "WPA2 Mixed" :
				 (record->security == RTW_SECURITY_WPA_WPA2_TKIP_PSK) ? "WPA/WPA2 TKIP" :
				 (record->security == RTW_SECURITY_WPA_WPA2_AES_PSK) ? "WPA/WPA2 AES" :
				 (record->security == RTW_SECURITY_WPA_WPA2_MIXED_PSK) ? "WPA/WPA2 Mixed" :
				 (record->security == (RTW_SECURITY_WPA_TKIP_PSK | ENTERPRISE_ENABLED)) ? "WPA TKIP Enterprise" :
				 (record->security == (RTW_SECURITY_WPA_AES_PSK | ENTERPRISE_ENABLED)) ? "WPA AES Enterprise" :
				 (record->security == (RTW_SECURITY_WPA_MIXED_PSK | ENTERPRISE_ENABLED)) ? "WPA Mixed Enterprise" :
				 (record->security == (RTW_SECURITY_WPA2_TKIP_PSK | ENTERPRISE_ENABLED)) ? "WPA2 TKIP Enterprise" :
				 (record->security == (RTW_SECURITY_WPA2_AES_PSK | ENTERPRISE_ENABLED)) ? "WPA2 AES Enterprise" :
				 (record->security == (RTW_SECURITY_WPA2_MIXED_PSK | ENTERPRISE_ENABLED)) ? "WPA2 Mixed Enterprise" :
				 (record->security == (RTW_SECURITY_WPA_WPA2_TKIP_PSK | ENTERPRISE_ENABLED)) ? "WPA/WPA2 TKIP Enterprise" :
				 (record->security == (RTW_SECURITY_WPA_WPA2_AES_PSK | ENTERPRISE_ENABLED)) ? "WPA/WPA2 AES Enterprise" :
				 (record->security == (RTW_SECURITY_WPA_WPA2_MIXED_PSK | ENTERPRISE_ENABLED)) ? "WPA/WPA2 Mixed Enterprise" :
#ifdef CONFIG_SAE_SUPPORT
				 (record->security == RTW_SECURITY_WPA3_AES_PSK) ? "WPA3-SAE AES" :
				 (record->security == RTW_SECURITY_WPA2_WPA3_MIXED) ? "WPA2/WPA3-SAE AES" :
				 (record->security == (WPA3_SECURITY | ENTERPRISE_ENABLED)) ? "WPA3 Enterprise" :
#endif
				 "Unknown            ");

	RTW_API_INFO(" %s ", record->SSID.val);
	RTW_API_INFO("\r\n");
#endif
}

static rtw_result_t app_scan_result_handler(unsigned int scanned_AP_num, void *user_data)
{
	/* To avoid gcc warnings */
	(void) user_data;

	rtw_scan_result_t *scanned_AP_info;
	char *scan_buf = NULL;
	unsigned int i = 0;

	if (scanned_AP_num == 0) {/* scanned no AP*/
		return RTW_ERROR;
	}

	scan_buf = (char *)rtos_mem_zmalloc(scanned_AP_num * sizeof(rtw_scan_result_t));
	if (scan_buf == NULL) {
		return RTW_ERROR;
	}

	if (wifi_get_scan_records(&scanned_AP_num, scan_buf) < 0) {
		rtos_mem_free((u8 *)scan_buf);
		return RTW_ERROR;
	}

	for (i = 0; i < scanned_AP_num; i++) {
#if WIFI_LOGO_CERTIFICATION_CONFIG
		RTW_API_INFO("[%d],", (i + 1));
#else
		RTW_API_INFO("%d\t ", (i + 1));
#endif
		scanned_AP_info = (rtw_scan_result_t *)(scan_buf + i * sizeof(rtw_scan_result_t));
		scanned_AP_info->SSID.val[scanned_AP_info->SSID.len] = 0; /* Ensure the SSID is null terminated */

		print_scan_result(scanned_AP_info);
	}
	rtos_mem_free((u8 *)scan_buf);

	return RTW_SUCCESS;
}

static void print_wifi_setting(unsigned char wlan_idx, rtw_wifi_setting_t *pSetting)
{
#ifndef CONFIG_INIC_NO_FLASH

	RTW_API_INFO("WLAN%d Setting:\n\r", wlan_idx);
	RTW_API_INFO("==============================\n\r");

	switch (pSetting->mode) {
	case RTW_MODE_AP:
		RTW_API_INFO("      MODE => AP\n\r");
		break;
	case RTW_MODE_STA:
		RTW_API_INFO("      MODE => STATION\n\r");
		break;
	default:
		RTW_API_INFO("      MODE => UNKNOWN\n\r");
	}
	RTW_API_INFO("      SSID => %s\n\r", pSetting->ssid);
	RTW_API_INFO("     BSSID => %02x:%02x:%02x:%02x:%02x:%02x\n\r", pSetting->bssid[0], pSetting->bssid[1], pSetting->bssid[2], pSetting->bssid[3],
				 pSetting->bssid[4], pSetting->bssid[5]);
	RTW_API_INFO("   CHANNEL => %d\n\r", pSetting->channel);

	if (pSetting->security_type == RTW_SECURITY_OPEN) {
		RTW_API_INFO("  SECURITY => OPEN\n\r");
	} else if (pSetting->security_type == RTW_SECURITY_WEP_PSK) {
		RTW_API_INFO("  SECURITY => WEP\n\r");
		RTW_API_INFO(" KEY INDEX => %d\n\r", pSetting->key_idx);
	} else if (pSetting->security_type == RTW_SECURITY_WPA_TKIP_PSK) {
		RTW_API_INFO("  SECURITY => WPA TKIP\n\r");
	} else if (pSetting->security_type == RTW_SECURITY_WPA_AES_PSK) {
		RTW_API_INFO("  SECURITY => WPA AES\n\r");
	} else if (pSetting->security_type == RTW_SECURITY_WPA_MIXED_PSK) {
		RTW_API_INFO("  SECURITY => WPA MIXED\n\r");
	} else if (pSetting->security_type == RTW_SECURITY_WPA2_AES_PSK) {
		RTW_API_INFO("  SECURITY => WPA2 AES\n\r");
	} else if (pSetting->security_type == RTW_SECURITY_WPA2_TKIP_PSK) {
		RTW_API_INFO("  SECURITY => WPA2 TKIP\n\r");
	} else if (pSetting->security_type == RTW_SECURITY_WPA2_MIXED_PSK) {
		RTW_API_INFO("  SECURITY => WPA2 MIXED\n\r");
	} else if (pSetting->security_type == RTW_SECURITY_WPA_WPA2_TKIP_PSK) {
		RTW_API_INFO("  SECURITY => WPA/WPA2 TKIP\n\r");
	} else if (pSetting->security_type == RTW_SECURITY_WPA_WPA2_AES_PSK) {
		RTW_API_INFO("  SECURITY => WPA/WPA2 AES\n\r");
	} else if (pSetting->security_type == RTW_SECURITY_WPA_WPA2_MIXED_PSK) {
		RTW_API_INFO("  SECURITY => WPA/WPA2 MIXED\n\r");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA_TKIP_PSK | ENTERPRISE_ENABLED)) {
		RTW_API_INFO("  SECURITY => WPA TKIP ENTERPRISE\n\r");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA_AES_PSK | ENTERPRISE_ENABLED)) {
		RTW_API_INFO("  SECURITY => WPA AES ENTERPRISE\n\r");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA_MIXED_PSK | ENTERPRISE_ENABLED)) {
		RTW_API_INFO("  SECURITY => WPA MIXED ENTERPRISE\n\r");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA2_TKIP_PSK | ENTERPRISE_ENABLED)) {
		RTW_API_INFO("  SECURITY => WPA2 TKIP ENTERPRISE\n\r");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA2_AES_PSK | ENTERPRISE_ENABLED)) {
		RTW_API_INFO("  SECURITY => WPA2 AES ENTERPRISE\n\r");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA2_MIXED_PSK | ENTERPRISE_ENABLED)) {
		RTW_API_INFO("  SECURITY => WPA2 MIXED ENTERPRISE\n\r");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA_WPA2_TKIP_PSK | ENTERPRISE_ENABLED)) {
		RTW_API_INFO("  SECURITY => WPA/WPA2 TKIP ENTERPRISE\n\r");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA_WPA2_AES_PSK | ENTERPRISE_ENABLED)) {
		RTW_API_INFO("  SECURITY => WPA/WPA2 AES ENTERPRISE\n\r");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA_WPA2_MIXED_PSK | ENTERPRISE_ENABLED)) {
		RTW_API_INFO("  SECURITY => WPA/WPA2 MIXED ENTERPRISE\n\r");
#ifdef CONFIG_SAE_SUPPORT
	} else if (pSetting->security_type == RTW_SECURITY_WPA3_AES_PSK) {
		RTW_API_INFO("  SECURITY => WPA3-SAE AES\n\r");
	} else if (pSetting->security_type == RTW_SECURITY_WPA2_WPA3_MIXED) {
		RTW_API_INFO("  SECURITY => WPA2/WPA3-SAE AES\n\r");
	} else if (pSetting->security_type == (WPA3_SECURITY | ENTERPRISE_ENABLED)) {
		RTW_API_INFO("  SECURITY => WPA3 ENTERPRISE\n\r");
#endif
	} else {
		RTW_API_INFO("  SECURITY => UNKNOWN\n\r");
	}

	RTW_API_INFO("  PASSWORD => %s\n\r", pSetting->password);
	RTW_API_INFO("\n\r");
#endif
}

void fATWD(void *arg)
{
	/* To avoid gcc warnings */
	(void) arg;

	int timeout = 20;
	volatile int ret = RTW_SUCCESS;
	rtw_wifi_setting_t wifi_setting = {RTW_MODE_NONE, {0}, {0}, 0, RTW_SECURITY_OPEN, {0}, 0, 0, 0, 0, 0};

	RTK_LOGS(NOTAG, "[ATWD]: _AT_WLAN_DISC_NET_\n\r");
	RTK_LOGS(NOTAG, "Deassociating AP ...\n\r");

	if (wifi_get_setting(STA_WLAN_INDEX, &wifi_setting) >= 0) {
		if (wifi_setting.mode == RTW_MODE_AP) {
			RTK_LOGS(NOTAG, "currently in softap mode, no need disconnect\n\r");
			goto exit_success;
		}
	} else {
		RTK_LOGS(NOTAG, "netif is down, no need disconnect\n\r");
		goto exit_success;
	}

	if (wifi_is_connected_to_ap() != RTW_SUCCESS) {
		RTK_LOGS(NOTAG, "not connected yet\n\r");
		goto exit_success;
	}
	if ((ret = wifi_disconnect()) < 0) {
		RTK_LOGS(NOTAG, "ERROR: Operation failed!\n\r");
		goto exit;
	}

	while (1) {
		if (wifi_is_connected_to_ap() != RTW_SUCCESS) {
			RTK_LOGS(NOTAG, "WIFI disconnect succeed\n\r");
			break;
		}

		if (timeout == 0) {
			RTK_LOGS(NOTAG, "ERROR: Deassoc timeout!\n\r");
			ret = RTW_TIMEOUT;
			break;
		}

		rtos_time_delay_ms(1 * 1000);
		timeout --;
	}
	RTK_LOGS(NOTAG, "\n\r");

#ifdef CONFIG_LWIP_LAYER
	user_static_ip.use_static_ip = 0;
	LwIP_ReleaseIP(STA_WLAN_INDEX);
#endif
exit:
	init_wifi_struct();
	return;
exit_success:
	init_wifi_struct();
	return;
}

void fATWS(void *arg)
{
	char buf[33] = {0};
	u8 *channel_list = NULL;
	int num_channel = 0;
	int i, argc = 0;
	char *argv[MAX_ARGC] = {0};
	volatile int ret = RTW_SUCCESS;
	rtw_scan_param_t scan_param = {RTW_SCAN_NOUSE, 0, 0, 0, {0}, 0, 0, 0, 0};
	enum rtw_join_status_type join_status = RTW_JOINSTATUS_UNKNOWN;
	RTK_LOGI(NOTAG, "[ATWS]: _AT_WLAN_SCAN_\n\r");

	join_status = wifi_get_join_status();
	if ((join_status > RTW_JOINSTATUS_UNKNOWN) && (join_status < RTW_JOINSTATUS_SUCCESS)) {
		goto exit;
	}
	if (arg) {
		strncpy(buf, arg, sizeof(buf) - 1);
		argc = parse_param(buf, argv);
		if (argc < 2) {
			ret = RTW_BADARG;
			goto exit;
		}
		num_channel = atoi(argv[1]);
		channel_list = (u8 *)malloc(num_channel);
		if (!channel_list) {
			RTK_LOGI(NOTAG, "[ATWS]ERROR: Can't malloc memory for channel list\n\r");
			ret = RTW_BUFFER_UNAVAILABLE_TEMPORARY;
			goto exit;
		}
		//parse command channel list
		for (i = 2; i <= argc - 1 ; i++) {
			*(channel_list + i - 2) = (u8)atoi(argv[i]);
		}
		scan_param.channel_list = channel_list;
		scan_param.channel_list_num = num_channel;
	}

	scan_param.scan_user_callback = app_scan_result_handler;
	if ((ret = wifi_scan_networks(&scan_param, 0)) != RTW_SUCCESS) {
		RTK_LOGI(NOTAG, "[ATWS]ERROR: wifi scan failed\n\r");
		goto exit;
	}
exit:
	if (arg && channel_list) {
		free(channel_list);
	}
}

void fATWx(void *arg)
{
	/* To avoid gcc warnings */
	(void) arg;

	int i = 0;
#ifdef CONFIG_LWIP_LAYER
	u8 *mac = LwIP_GetMAC(0);
	u8 *ip = LwIP_GetIP(0);
	u8 *gw = LwIP_GetGW(0);
	u8 *msk = LwIP_GetMASK(0);
#endif
	rtw_wifi_setting_t *p_wifi_setting = NULL;
	rtw_sw_statistics_t stats;

	p_wifi_setting = (struct _rtw_wifi_setting_t *)rtos_mem_zmalloc(sizeof(struct _rtw_wifi_setting_t));
	if (p_wifi_setting == NULL) {
		RTK_LOGI(NOTAG, "[ATW?]: alloc p_wifi_setting fail \n\r");
		return;
	}

	RTK_LOGI(NOTAG, "[ATW?]: _AT_WLAN_INFO_\n\r");
	for (i = 0; i < NET_IF_NUM; i++) {
		if (wifi_is_running(i)) {
#ifdef CONFIG_LWIP_LAYER
			mac = LwIP_GetMAC(i);
			ip = LwIP_GetIP(i);
			gw = LwIP_GetGW(i);
			msk = LwIP_GetMASK(i);
#endif
			RTK_LOGI(NOTAG, "WLAN%d Status: Running\n\r",  i);
			RTK_LOGI(NOTAG, "==============================\n\r");

			wifi_get_sw_statistic(i, &stats);
			if (i == 0) {
				RTK_LOGI(NOTAG, "max_skbinfo_used_num=%d, skbinfo_used_num=%d\n", stats.max_skbbuf_used_number, stats.skbbuf_used_number);
				RTK_LOGI(NOTAG, "max_skbdata_used_num=%d, skbdata_used_num=%d\n\n", stats.max_skbdata_used_number, stats.skbdata_used_number);
			}
			wifi_get_setting(i, p_wifi_setting);
			print_wifi_setting(i, p_wifi_setting);

#ifdef CONFIG_LWIP_LAYER
			RTK_LOGI(NOTAG, "Interface (%d)\n\r", i);
			RTK_LOGI(NOTAG, "==============================\n\r");
			RTK_LOGI(NOTAG, "\tMAC => %02x:%02x:%02x:%02x:%02x:%02x\n\r", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]) ;
			RTK_LOGI(NOTAG, "\tIP  => %d.%d.%d.%d\n\r", ip[0], ip[1], ip[2], ip[3]);
			RTK_LOGI(NOTAG, "\tGW  => %d.%d.%d.%d\n\r", gw[0], gw[1], gw[2], gw[3]);
			RTK_LOGI(NOTAG, "\tmsk  => %d.%d.%d.%d\n\r\n\r", msk[0], msk[1], msk[2], msk[3]);
#endif
			if (p_wifi_setting->mode == RTW_MODE_AP || i == 1) {
				unsigned int client_number;
				rtw_client_list_t client_info = {0};
				wifi_get_associated_client_list(&client_info);

				RTK_LOGI(NOTAG, "Associated Client List:\n\r");
				RTK_LOGI(NOTAG, "==============================\n\r");

				if (client_info.count == 0) {
					RTK_LOGI(NOTAG, "Client Num: %d\n\r\n\r", client_info.count);
				} else {
					RTK_LOGI(NOTAG, "Client Num: %d\n\r", client_info.count);
					for (client_number = 0; client_number < client_info.count; client_number++) {
						RTK_LOGI(NOTAG, "Client %d:\n\r", client_number + 1);
						RTK_LOGI(NOTAG, "\tMAC => "MAC_FMT"\n\r",
								 MAC_ARG(client_info.mac_list[client_number].octet));

					}
					RTK_LOGI(NOTAG, "\n\r");
				}
			}
		}
// show the ethernet interface info
		else {
#if defined(CONFIG_ETHERNET) && CONFIG_ETHERNET
			if (i == NET_IF_NUM - 1) {
#ifdef CONFIG_LWIP_LAYER
				mac = LwIP_GetMAC(i);
				ip = LwIP_GetIP(i);
				gw = LwIP_GetGW(i);
				RTK_LOGI(NOTAG, "Interface ethernet\n\r");
				RTK_LOGI(NOTAG, "==============================\n\r");
				RTK_LOGI(NOTAG, "\tMAC => %02x:%02x:%02x:%02x:%02x:%02x\n\r", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]) ;
				RTK_LOGI(NOTAG, "\tIP  => %d.%d.%d.%d\n\r", ip[0], ip[1], ip[2], ip[3]);
				RTK_LOGI(NOTAG, "\tGW  => %d.%d.%d.%d\n\r\n\r", gw[0], gw[1], gw[2], gw[3]);
#endif // end CONFIG_LWIP_LAYER
			}
#endif // end CONFIG_ETHERNET
		}
	}

	rtos_mem_free((u8 *)p_wifi_setting);
#if defined(CONFIG_IP_NAT) && (CONFIG_IP_NAT == 1)
	ipnat_dump();
#endif

#if defined(configUSE_TRACE_FACILITY) && (configUSE_TRACE_FACILITY == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS == 1)
	{
		signed char pcWriteBuffer[1024];
		// TODO
		// vTaskList((char *)pcWriteBuffer);
		pcWriteBuffer[0] = 0;
		RTK_LOGI(NOTAG, "Task List: \n\r%s\n\r", pcWriteBuffer);
	}
#endif

}

void fATW0(void *arg)
{
	volatile int ret = RTW_SUCCESS;
	(void) ret;
	if (!arg) {
		RTK_LOGS(NOTAG, "[ATW0]Usage: ATW0=SSID(Maximum length is 32)\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	if (strlen((char *)arg) > 32) {
		RTK_LOGS(NOTAG, "[ATW0]Error: SSID length can't exceed 32\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	RTK_LOGS(NOTAG, "[ATW0]: _AT_WLAN_SET_SSID_ [%s]\n\r", (char *)arg);
	strncpy((char *)wifi.ssid.val, (char *)arg, sizeof(wifi.ssid.val) - 1);
	wifi.ssid.len = strlen((char *)arg);
exit:
	return;
}

void fATW1(void *arg)
{
	volatile int ret = RTW_SUCCESS;
	(void) ret;
	if (!arg) {
		RTK_LOGS(NOTAG, "[ATW1]Usage: ATW1=PASSPHRASE\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	RTK_LOGS(NOTAG, "[ATW1]: _AT_WLAN_SET_PASSPHRASE_ [%s]\n\r", (char *)arg);

	strncpy((char *)password, (char *)arg, sizeof(password) - 1);
	wifi.password = password;
	wifi.password_len = strlen((char *)arg);
exit:
	return;
}

void fATW2(void *arg)
{
	volatile int ret = RTW_SUCCESS;
	(void) ret;
	if (!arg) {
		RTK_LOGS(NOTAG, "[ATW2]Usage: ATW2=KEYID\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	RTK_LOGS(NOTAG, "[ATW2]: _AT_WLAN_SET_KEY_ID_ [%s]\n\r", (char *)arg);
	if ((strlen((const char *)arg) != 1) || (*(char *)arg < '0' || *(char *)arg > '3')) {
		RTK_LOGS(NOTAG, "Wrong WEP key id. Must be one of 0,1,2, or 3.\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	wifi.key_id = atoi((const char *)(arg));
exit:
	return;
}

void fATW3(void *arg)
{
	volatile int ret = RTW_SUCCESS;
	(void) ret;
	if (!arg) {
		RTK_LOGS(NOTAG, "[ATW3]Usage: ATW3=SSID\n\r");
		ret = RTW_BADARG;
		goto exit;
	}

	ap.ssid.len = strlen((char *)arg);

	if (ap.ssid.len > 32) {
		RTK_LOGS(NOTAG, "[ATW3]Error: SSID length can't exceed 32\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	strncpy((char *)ap.ssid.val, (char *)arg, sizeof(ap.ssid.val) - 1);

	RTK_LOGS(NOTAG, "[ATW3]: _AT_WLAN_AP_SET_SSID_ [%s]\n\r", ap.ssid.val);
exit:
	return;
}

void fATW4(void *arg)
{
	volatile int ret = RTW_SUCCESS;
	(void) ret;
	if (!arg) {
		RTK_LOGS(NOTAG, "[ATW4]Usage: ATW4=PASSWORD\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	if (strlen((char *)arg) > 64) {
		RTK_LOGS(NOTAG, "[ATW4]Error: PASSWORD length can't exceed 64\n\r");
		ret = RTW_BADARG;
		goto exit;
	}

	strncpy((char *)password, (char *)arg, sizeof(password) - 1);
	ap.password = password;
	ap.password_len = strlen((char *)arg);
	RTK_LOGS(NOTAG, "[ATW4]: _AT_WLAN_AP_SET_SEC_KEY_ [%s]\n\r", ap.password);
exit:
	return;
}

void fATW5(void *arg)
{
	volatile int ret = RTW_SUCCESS;
	(void) ret;
	if (!arg) {
		RTK_LOGS(NOTAG, "[ATW5]Usage: ATW5=CHANNEL\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	ap.channel = (unsigned char) atoi((const char *)arg);
	RTK_LOGS(NOTAG, "[ATW5]: _AT_WLAN_AP_SET_CHANNEL_ [channel %d]\n\r", ap.channel);
exit:
	return;
}

void fATW6(void *arg)
{
	unsigned int	mac[ETH_ALEN];
	u32		i;
	volatile int ret = RTW_SUCCESS;
	(void) ret;
	if (!arg) {
		RTK_LOGS(NOTAG, "[ATW6]Usage: ATW6=BSSID\n\r");
		ret =  RTW_BADARG;
		goto exit;
	}
	RTK_LOGS(NOTAG, "[ATW6]: _AT_WLAN_SET_BSSID_ [%s]\n\r", (char *)arg);
	_sscanf_ss(arg, MAC_FMT, mac, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5);
	for (i = 0; i < ETH_ALEN; i ++) {
		wifi.bssid.octet[i] = (u8)mac[i] & 0xFF;
	}
exit:
	return;
}

#ifdef CONFIG_FPGA
void fATW7(void *arg)
{
	if (!arg) {
		RTK_LOGI(NOTAG, "[ATW7]Usage: ATW7=0,1,2 or 3(open, WEP, TKIP or AES)\n\r");
		return;
	}
	volatile int ret = RTW_SUCCESS;
	(void) ret;
	RTK_LOGI(NOTAG, "[ATW7]: _AT_WLAN_SET_SECURITY [%s]\n\r", (char *)arg);
	if ((strlen((const char *)arg) != 1) || (*(char *)arg < '0' || *(char *)arg > '3')) {
		RTK_LOGI(NOTAG, "Wrong num. Must be one of 0,1,2 or 3.\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	security = atoi((const char *)(arg));
exit:
	return;
}
#endif
void fATWf(void *arg)
{
	if (!arg) {
		RTK_LOGI(NOTAG, "        1 : DISABLE AUTORECONNECT\n\r");
		return;
	}
	int mode =  atoi((const char *)arg);

	if (mode == 1) {
		RTK_LOGI(NOTAG, "[ATWf] Disable autoreconnect...\n\r");
		wifi_config_autoreconnect(RTW_AUTORECONNECT_DISABLE);
	}
	return;
}

void fATWA(void *arg)
{
	/* To avoid gcc warnings */
	(void) arg;
#ifdef CONFIG_LWIP_LAYER
	struct netif *pnetif = &xnetif[SOFTAP_WLAN_INDEX];
	u32 ip_addr;
	u32 netmask;
	u32 gw;
#endif
	int timeout = 20;
	volatile int ret = RTW_SUCCESS;
	struct _rtw_wifi_setting_t *setting = NULL;
	RTK_LOGI(NOTAG, "[ATWA]: _AT_WLAN_AP_ACTIVATE_\n\r");
	if (ap.ssid.val[0] == 0) {
		RTK_LOGI(NOTAG, "[ATWA]Error: SSID can't be empty\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	if (ap.password == NULL) {
		ap.security_type = RTW_SECURITY_OPEN;
	} else {
		if (ap.password_len <= RTW_WPA2_MAX_PSK_LEN &&
			ap.password_len >= RTW_MIN_PSK_LEN) {
			ap.security_type = RTW_SECURITY_WPA2_AES_PSK;
		}
#ifdef CONFIG_FPGA
		else if (ap.password_len == 5) {
			ap.security_type = RTW_SECURITY_WEP_PSK;
		}
#endif
		else {
			RTK_LOGI(NOTAG, "[ATWA]Error: password should be 64 hex characters or 8-63 ASCII characters\n\r");
			ret = RTW_INVALID_KEY;
			goto exit;
		}
	}
#ifdef CONFIG_FPGA
	if (security == 0) {
		ap.security_type = RTW_SECURITY_OPEN;
	} else if (security == 1) {
		ap.security_type = RTW_SECURITY_WEP_PSK;
	} else if (security == 2) {
		ap.security_type = RTW_SECURITY_WPA2_TKIP_PSK;
	} else if (security == 3) {
		ap.security_type = RTW_SECURITY_WPA2_AES_PSK;
	}

	if (security > 0 && ap.password == NULL) {
		RTK_LOGI(NOTAG, "[ATWA]Error: password = NULL,please input password \n\r");
		goto exit;
	}
#endif

#ifdef CONFIG_LWIP_LAYER
	dhcps_deinit();
	ip_addr = WIFI_MAKEU32(GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	netmask = WIFI_MAKEU32(NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
	gw = WIFI_MAKEU32(GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	LwIP_SetIP(SOFTAP_WLAN_INDEX, ip_addr, netmask, gw);
#endif

	RTK_LOGI(NOTAG, "Starting AP ...\n");

	wifi_stop_ap();
	if ((ret = wifi_start_ap(&ap)) < 0) {
		RTK_LOGI(NOTAG, "ERROR: Operation failed!\n");
		goto exit;
	}

	if ((setting = rtos_mem_zmalloc(sizeof(struct _rtw_wifi_setting_t))) == NULL) {
		goto exit;
	}

	while (1) {
		wifi_get_setting(SOFTAP_WLAN_INDEX, setting);
		if (strlen((const char *)setting->ssid) > 0) {
			if (strcmp((const char *) setting->ssid, (const char *)ap.ssid.val) == 0) {
				RTK_LOGI(NOTAG, "%s started\n", ap.ssid.val);
				ret = RTW_SUCCESS;
				break;
			}
		}

		if (timeout == 0) {
			RTK_LOGI(NOTAG, "ERROR: Start AP timeout!\n");
			ret = RTW_TIMEOUT;
			break;
		}

		rtos_time_delay_ms(1 * 1000);
		timeout --;
	}

	if (setting) {
		rtos_mem_free(setting);
	}
#ifdef CONFIG_LWIP_LAYER
	ip_addr = WIFI_MAKEU32(AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
	netmask = WIFI_MAKEU32(AP_NETMASK_ADDR0, AP_NETMASK_ADDR1, AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
	gw = WIFI_MAKEU32(AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
	LwIP_SetIP(SOFTAP_WLAN_INDEX, ip_addr, netmask, gw);
	dhcps_init(pnetif);
#endif

exit:
	init_wifi_struct();
}

void fATWAO(void *arg)
{
	/* To avoid gcc warnings */
	(void) arg;

	wifi_stop_ap();
}

void fATWC(void *arg)
{
	/* To avoid gcc warnings */
	(void) arg;

	int ret;
	rtw_wifi_setting_t *p_wifi_setting = NULL;
	unsigned long tick1 = rtos_time_get_current_system_time_ms();
	unsigned long tick2, tick3;
	char empty_bssid[6] = {0}, assoc_by_bssid = 0;
	char buf[32] = {0};
	char *argv[MAX_ARGC] = {0};
	int argc = 0;
	unsigned char channel = 0;

	p_wifi_setting = (struct _rtw_wifi_setting_t *)rtos_mem_zmalloc(sizeof(struct _rtw_wifi_setting_t));
	if (p_wifi_setting == NULL) {
		RTK_LOGI(NOTAG, "[ATWC]: alloc p_wifi_setting fail \n\r");
		return;
	}

	RTK_LOGI(NOTAG, "[ATWC]: _AT_WLAN_JOIN_NET_\n\r");

	if (arg) {
		strncpy(buf, arg, sizeof(buf) - 1);
		argc = parse_param(buf, argv);
		if (argc <= 2) {
			channel = (unsigned char) atoi(argv[1]);
		} else {
			RTK_LOGI(NOTAG, "[ATWC]Normal connect Usage: ATWC\n\r");
			RTK_LOGI(NOTAG, "[ATWC]Fast connect Usage: ATWC=channel_num\n\r");
			goto EXIT;
		}
	}

	wifi.channel = channel;

	if (memcmp(wifi.bssid.octet, empty_bssid, 6)) {
		assoc_by_bssid = 1;
	} else if (wifi.ssid.val[0] == 0) {
		RTK_LOGI(NOTAG, "[ATWC]Error: SSID can't be empty\n\r");
		ret = RTW_BADARG;
		goto EXIT;
	}
	if (wifi.password != NULL) {
		if ((wifi.key_id >= 0) && (wifi.key_id <= 3)) {
			wifi.security_type = RTW_SECURITY_WEP_SHARED;
		} else {
			wifi.security_type = RTW_SECURITY_WPA2_AES_PSK;
		}
	} else {
		wifi.security_type = RTW_SECURITY_OPEN;
	}

	//Check if in AP mode
	wifi_get_setting(STA_WLAN_INDEX, p_wifi_setting);
	if (p_wifi_setting->mode == RTW_MODE_AP) {
#ifdef CONFIG_LWIP_LAYER
		dhcps_deinit();
#endif
		if (wifi_on(RTW_MODE_STA) < 0) {
			RTK_LOGI(NOTAG, "ERROR: Wifi on failed!\n\r");
			ret = RTW_ERROR;
			goto EXIT;
		}
	}

	if (assoc_by_bssid) {
		RTK_LOGI(NOTAG, "Joining BSS by BSSID "MAC_FMT" ...\n\r", MAC_ARG(wifi.bssid.octet));
	} else {
		RTK_LOGI(NOTAG, "Joining BSS by SSID %s...\n\r", (char *)wifi.ssid.val);
	}
	ret = wifi_connect(&wifi, 1);


	if (ret != RTW_SUCCESS) {
		if (ret == RTW_INVALID_KEY) {
			RTK_LOGI(NOTAG, "ERROR:Invalid Key \n\r");
		}

		RTK_LOGI(NOTAG, "ERROR: Can't connect to AP\n\r");
		goto EXIT;
	}
	tick2 = rtos_time_get_current_system_time_ms();
	RTK_LOGI(NOTAG, "Connected after %d ms.\n\r\n", (unsigned int)(tick2 - tick1));
#ifdef CONFIG_LWIP_LAYER
	/* Start DHCPClient */
	LwIP_DHCP(0, DHCP_START);
	tick3 = rtos_time_get_current_system_time_ms();
	RTK_LOGI(NOTAG, "Got IP after %d ms.\n\r\n", (unsigned int)(tick3 - tick1));
#endif


	RTK_LOGI(NOTAG, "\n\r");
EXIT:
	rtos_mem_free((u8 *)p_wifi_setting);
	init_wifi_struct();
}

#if SCAN_WITH_SSID
void fATWs(void *arg)
{
	char buf[32] = {0};
	u8 *channel_list = NULL;
	int num_channel = 0;
	rtw_scan_param_t scan_param;
	int i, argc = 0;
	char *argv[MAX_ARGC] = {0};
	RTK_LOGI(NOTAG, "[ATWs]: _AT_WLAN_SCAN_WITH_SSID_ [%s]\n\r", (char *)wifi.ssid.val);
	if (arg) {
		strncpy(buf, arg, sizeof(buf) - 1);
		argc = parse_param(buf, argv);
		if (argc > 2) {
			num_channel = atoi(argv[1]);
			channel_list = (u8 *)malloc(num_channel);
			if (!channel_list) {
				RTK_LOGI(NOTAG, "[ATWs]ERROR: Can't malloc memory for channel list\n\r");
				goto exit;
			}
			//parse command channel list
			for (i = 2; i <= argc - 1 ; i++) {
				*(channel_list + i - 2) = (u8)atoi(argv[i]);
			}
		} else {
			RTK_LOGI(NOTAG, "[ATWs]For Scan all channel Usage: ATWs\n\r");
			RTK_LOGI(NOTAG, "[ATWs]For Scan partial channel Usage: ATWs=num_channels[channel_num1, ...]\n\r");
			goto exit;
		}
	}
	if (!wifi.ssid.len) { //not set ssid yet
		RTK_LOGI(NOTAG, "[ATWs]ssid not set yet, use ATW0 to set ssid\n\r");
		goto exit;
	}
	memset(&scan_param, 0, sizeof(rtw_scan_param_t));
	scan_param.ssid = (char *)wifi.ssid.val;
	scan_param.scan_user_callback = app_scan_result_handler;
	scan_param.channel_list = channel_list;
	scan_param.channel_list_num = num_channel;
	if (wifi_scan_networks(&scan_param, 0) != RTW_SUCCESS) {
		RTK_LOGI(NOTAG, "[ATWs]ERROR: wifi scan failed\n\r");
	}
exit:
	init_wifi_struct();
	if (arg && channel_list) {
		free(channel_list);
	}
}
#endif

void fATWR(void *arg)
{
	/* To avoid gcc warnings */
	(void) arg;

	rtw_phy_statistics_t phy_statistics;
	RTK_LOGI(NOTAG, "[ATWR]: _AT_WLAN_GET_RSSI_\n\r");
	wifi_fetch_phy_statistic(&phy_statistics);
	RTK_LOGI(NOTAG, "rssi = %d\n\r", phy_statistics.rssi);
	RTK_LOGI(NOTAG, "data rssi = %d\n\r", phy_statistics.data_rssi);
	RTK_LOGI(NOTAG, "beacon rssi = %d\n\r", phy_statistics.beacon_rssi);
	RTK_LOGI(NOTAG, "\n\r");
}

void fATWY(void *arg)
{
	/* To avoid gcc warnings */
	(void) arg;
	rtw_phy_statistics_t phy_statistics;
	RTK_LOGI(NOTAG, "[ATWY]: _AT_WLAN_GET_SNR_\n\r");
	wifi_fetch_phy_statistic(&phy_statistics);
	RTK_LOGI(NOTAG, "snr = %d\n\r", phy_statistics.snr);
	RTK_LOGI(NOTAG, "\n\r");
}

void fATWP(void *arg)
{
	/* To avoid gcc warnings */
	(void) arg;
	RTK_LOGS(NOTAG, "This command is not supported yet.\n");

	return;
}

void fATWM(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};
	promisc_para_t promisc_para;
	u32 status;
	memset(&promisc_para, 0, sizeof(promisc_para_t));
	if ((argc = parse_param(arg, argv)) > 0) {
		if (_strcmp((char *)argv[1], "enable") == 0) {
			status = ENABLE;
			if (_strcmp((char *)argv[2], "all") == 0) {
				promisc_para.filter_mode = RCR_ALL_PKT;
			} else if (_strcmp((char *)argv[2], "apall") == 0) {
				promisc_para.filter_mode = RCR_AP_ALL;
			}
		} else {
			status = DISABLE;
		}

		wifi_promisc_enable(status, &promisc_para);
	}
}

void fATWW(void *arg)
{
#if defined(CONFIG_ENABLE_WPS) && CONFIG_ENABLE_WPS
	int argc = 0;
	char *argv[4];

	RTK_LOGS(NOTAG, "[ATWW]: _AT_WLAN_WPS_\n\r");
	if (!arg) {
		RTK_LOGS(NOTAG, "[ATWW]Usage: ATWW=pbc/pin\n\r");
		return;
	}
	argv[argc++] = (char *)"wifi_wps";
	argv[argc++] = arg;
	cmd_wps(argc, argv);
#else
	(void) arg;
	RTK_LOGS(NOTAG, "Please set CONFIG_ENABLE_WPS 1 by menuconfig to enable ATWW command\n");
#endif
}

void fATWZ(void *arg)
{
	char buf[64] = {0};
	char *copy = buf;
	int i = 0;
	int len = 0;
	volatile int ret = RTW_SUCCESS;
	(void) ret;

	RTK_LOGI(NOTAG, "[ATWZ]: _AT_WLAN_IWPRIV_\n\r");
	if (!arg) {
		RTK_LOGI(NOTAG, "[ATWZ]Usage: ATWZ=COMMAND[PARAMETERS]\n\r");
		ret = RTW_BADARG;
		goto exit;
	}
	strncpy(copy, arg, sizeof(buf) - 1);
	len = strlen(copy);
	do {
		if ((*(copy + i) == '[')) {
			*(copy + i) = ' ';
		}
		if ((*(copy + i) == ']') || (*(copy + i) == '\0')) {
			*(copy + i) = '\0';
			break;
		}
	} while ((i++) < len);

	i = 0;
	do {
		if ((*(copy + i) == ',')) {
			*(copy + i) = ' ';
			break;
		}
	} while ((i++) < len);

#ifdef CONFIG_AS_INIC_AP
	inic_iwpriv_command(copy, strlen(copy) + 1, 1);
#else
	rtw_iwpriv_command(STA_WLAN_INDEX, copy, 1);
#endif

exit:
	return;	// exit label cannot be last statement
}

void fATXP(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	volatile int ret = 0;
	(void) ret;
	int ps_en;

	RTK_LOGS(NOTAG, "[ATXP]: _AT_WLAN_POWER_MODE_\r\n");

	if (!arg) {
		RTK_LOGS(NOTAG, "[ATXP] Usage: ATXP=lps/ips/dtim[mode]\r\n");
		ret = RTW_BADARG;
		goto exit;
	} else {
		argc = parse_param(arg, argv);
		if (argc < 3) {
			RTK_LOGS(NOTAG, "[ATXP] Usage: ATXP=lps/ips/dtim[mode]\r\n");
			ret = RTW_BADARG;
			goto exit;
		}
	}

	if (strcmp(argv[1], "lps") == 0) {
		ps_en = atoi(argv[2]);
		wifi_set_lps_enable(ps_en);
		RTK_LOGS(NOTAG, "lps %s\r\n", (ps_en == 0) ? "disable" : "enable");
	} else if (strcmp(argv[1], "ips") == 0) {
		ps_en = atoi(argv[2]);
		wifi_set_ips_internal(ps_en);
		RTK_LOGS(NOTAG, "ips %s\r\n", (ps_en == 0) ? "disable" : "enable");
	}

exit:
	return;
}

void fATPE(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (!arg) {
		RTK_LOGS(NOTAG, "[ATPE] Usage : ATPE=<ip>(,<gateway>,<mask>)\n\r");
		return;
	}

	argc = parse_param(arg, argv);
	if ((argc != 2) && (argc != 4)) {
		RTK_LOGS(NOTAG, "[ATPE] command error\n\r");
		return;
	}

	//static IP will be set in LwIP_DHCP()
	user_static_ip.use_static_ip = 1;
	user_static_ip.addr = PP_HTONL(inet_addr(argv[1]));
	user_static_ip.gw = PP_HTONL(inet_addr(argv[2]));
	user_static_ip.netmask = PP_HTONL(inet_addr(argv[3]));

	return;
}

#if WIFI_LOGO_CERTIFICATION_CONFIG
#ifdef CONFIG_SAE_SUPPORT
void fATWGRP(void *arg)
{

	unsigned char grp_id = 0, i = 0, error = 0;
	int target_grp_id[10] = {19, 20};

	if (!arg) {
		error = 1;
	} else {
		grp_id = atoi((const char *)(arg));

		for (i = 0; i < 2; i++)
			if (grp_id == target_grp_id[i]) {
				break;
			}

		if (i == 2) {
			error = 1;
		}
	}

	if (error) {
		RTK_LOGI(NOTAG, "[ATGP]error cmd  !!\n\r");
		RTK_LOGI(NOTAG, "[ATGP]Usage: ATGP = group_id \n\r");
		RTK_LOGI(NOTAG, "      *************************************************\n\r");
		RTK_LOGI(NOTAG, "      ECC group: 19, 20 \n\r");
		RTK_LOGI(NOTAG, "      *************************************************\n\r");
	} else {
		RTK_LOGI(NOTAG, "[ATGP]: _AT_WLAN_SET_GRPID [%s]\n\r", (char *)arg);
		wifi_set_group_id(grp_id);
	}

	return;
}
#endif

void fATWPMK(void *arg)
{

	unsigned char pmk_enable = 0, error = 0;

	if (!arg) {
		error = 1;
	} else {
		if (1 != atoi((const char *)(arg))) {
			pmk_enable = 0;
		} else {
			pmk_enable = 1;
		}

		RTK_LOGI(NOTAG, "pmk_enable = %d\r\n", pmk_enable);
		RTK_LOGI(NOTAG, "[ATPM]: _AT_WLAN_SET_PMK [%s]\n\r", (char *)arg);
		wifi_set_pmk_cache_enable(pmk_enable);

	}

	if (error) {
		RTK_LOGI(NOTAG, "[ATPM]error cmd  !!\n\r");
		RTK_LOGI(NOTAG, "[ATPM]Usage: ATPM = enable \n\r");
		RTK_LOGI(NOTAG, "      *************************************************\n\r");
		RTK_LOGI(NOTAG, "      1: enable; 0: disable \r\n");
		RTK_LOGI(NOTAG, "      *************************************************\n\r");
	}

}

#ifdef CONFIG_IEEE80211W
void fATWPMF(void *arg)
{

	int ret;
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	unsigned char pmf_mode;

	RTK_LOGI(NOTAG, "[ATMF]: _AT_WLAN_PROTECTED_MANAGEMENT_FRAME_\r\n");

	if (!arg) {
		RTK_LOGI(NOTAG, "[ATMF] Usage: ATMF=none/optional/required\r\n");
		ret = RTW_BADARG;
		goto exit;
	} else {
		argc = parse_param(arg, argv);
		if (argc < 2) {
			RTK_LOGI(NOTAG, "[ATMF] Usage: ATMF=none/optional/required\r\n");
			ret = RTW_BADARG;
			goto exit;
		}
	}

	if (strcmp(argv[1], "none") == 0) {
		pmf_mode = 0;
		ret = wifi_set_pmf_mode(pmf_mode);
		if (ret == 0) {
			RTK_LOGI(NOTAG, "[ATMF]: set station no management protection\r\n");
		}
	}

	if (strcmp(argv[1], "optional") == 0) {
		pmf_mode = 1;
		ret = wifi_set_pmf_mode(pmf_mode);
		if (ret == 0) {
			RTK_LOGI(NOTAG, "[ATMF]: set station pmf optional\r\n");
		}
	}

	if (strcmp(argv[1], "required") == 0) {
		pmf_mode = 2;
		ret = wifi_set_pmf_mode(pmf_mode);
		if (ret == 0) {
			RTK_LOGI(NOTAG, "[ATMF]: set station pmf required\r\n");
		}
	}
exit:
	return;
}
#endif
#endif //WIFI_LOGO_CERTIFICATION_CONFIG

#endif // end of #if CONFIG_WLAN

#ifdef CONFIG_LWIP_LAYER
void fATWI(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	RTK_LOGS(NOTAG, "[ATWI]: _AT_WLAN_PING_TEST_\n\r");

	if (!arg) {
		RTK_LOGS(NOTAG, "[ATWI] Usage: ATWI=[host],[options]\n\r");
		RTK_LOGS(NOTAG, "       stop      Terminate ping\n\r");
		RTK_LOGS(NOTAG, "       -t    #   Ping the specified host until stopped\n\r");
		RTK_LOGS(NOTAG, "       -n    #   Number of echo requests to send (default 4 times)\n\r");
		RTK_LOGS(NOTAG, "       -l    #   Send buffer size (default 32 bytes)\n\r");
		RTK_LOGS(NOTAG, "       if    #   specify ping interface, 0 is for STA and 1 is for soft\n\r");
		RTK_LOGS(NOTAG, "   Example:\n\r");
		RTK_LOGS(NOTAG, "       ATWI=192.168.1.2,-n,100,-l,5000\n\r");
		return;
	}

	argv[0] = (char *)"ping";

	if ((argc = parse_param(arg, argv)) > 1) {
		cmd_ping(argc, argv);
	}
}

/*
 * To aviod compile error when cmd_iperf3 is not implemented
 */
_WEAK void cmd_iperf3(int argc, char **argv)

{
	/* To avoid gcc warnings */
	(void) argc;
	(void) argv;
	RTK_LOGS(NOTAG, " iperf3 is not compiled \n");
}

void fATWT(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (!arg) {
		RTK_LOGS(NOTAG, "[ATWT] iperf3 Usage: More Usage: ATWT=-help\n\r");

		RTK_LOGS(NOTAG, "[ATWT] iperf1 Usage: ATWT=[-s|-c,host|stop],[options]\n\r");
		RTK_LOGS(NOTAG, "[ATWT] Usage: ATWT=[-s|-c,host|stop],[options]\n\r");
		RTK_LOGS(NOTAG, "   Client/Server:\n\r");
		RTK_LOGS(NOTAG, "  	  ?     		List all stream status\n\r");
		RTK_LOGS(NOTAG, "  	 stop  #		terminate specific stream id or terminate all stream if no id specified\n\r");
		RTK_LOGS(NOTAG, "  	 -i    #		seconds between periodic bandwidth reports\n\r");
		RTK_LOGS(NOTAG, "  	 -l    #		length of buffer to read or write (default 1460 Bytes)\n\r");
		RTK_LOGS(NOTAG, "  	 -p    #		server port to listen on/connect to (default 5001)\n\r");
		RTK_LOGS(NOTAG, "   Server specific:\n\r");
		RTK_LOGS(NOTAG, "  	 -s 			run in server mode\n\r");
		RTK_LOGS(NOTAG, "   Client specific:\n\r");
		RTK_LOGS(NOTAG, "  	 -c    <host>	run in client mode, connecting to <host>\n\r");
		RTK_LOGS(NOTAG, "  	 -d 			Do a bidirectional test simultaneously\n\r");
		RTK_LOGS(NOTAG, "  	 -t    #		time in seconds to transmit for (default 10 secs)\n\r");
		RTK_LOGS(NOTAG, "  	 -n    #[KM]	number of bytes to transmit (instead of -t)\n\r");
		RTK_LOGS(NOTAG, "   Example:\n\r");
		RTK_LOGS(NOTAG, "  	 ATWT=-s,-p,5002\n\r");
		RTK_LOGS(NOTAG, "  	 ATWT=-c,192.168.1.2,-t,100,-p,5002\n\r");
		return;
	}

	if (!memcmp(arg, "iperf3", 6))  {
		RTK_LOGS(NOTAG, "[ATWT]: _AT_WLAN_IPERF3_TEST_\n\r");
		argv[0] = (char *)"iperf3";

		if ((argc = parse_param((char *)arg + 6 + 1, argv)) > 1) { //skip "iperf3,"
			cmd_iperf3(argc, argv);
		} else {
			RTK_LOGS(NOTAG, "[ATWT] More Usage: ATWT=-help\n\r");
		}
	} else {
		RTK_LOGS(NOTAG, "[ATWT]: _AT_WLAN_IPERF1_TCP_TEST_\n\r");
		argv[0] = (char *)"tcp";
		if ((argc = parse_param(arg, argv)) > 1) {
			cmd_iperf(argc, argv);
		}
	}
}

void fATWU(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	RTK_LOGS(NOTAG, "[ATWU]: _AT_WLAN_UDP_TEST_\n\r");

	if (!arg) {
		RTK_LOGS(NOTAG, "[ATWU] Usage: ATWU=[-s|-c,host|stop][options]\n\r");
		RTK_LOGS(NOTAG, "   Client/Server:\n\r");
		RTK_LOGS(NOTAG, "  	  ?     		List all stream status\n\r");
		RTK_LOGS(NOTAG, "  	 stop  #		terminate specific stream id or terminate all stream if no id specified\n\r");
		RTK_LOGS(NOTAG, "  	 -i    #		seconds between periodic bandwidth reports\n\r");
		RTK_LOGS(NOTAG, "  	 -l    #		length of buffer to read or write (default 1460 Bytes)\n\r");
		RTK_LOGS(NOTAG, "  	 -p    #		server port to listen on/connect to (default 5001)\n\r");
		RTK_LOGS(NOTAG, "   Server specific:\n\r");
		RTK_LOGS(NOTAG, "  	 -s 			run in server mode\n\r");
		RTK_LOGS(NOTAG, "   Client specific:\n\r");
		RTK_LOGS(NOTAG, "  	 -b    #[KM]	for UDP, bandwidth to send at in bits/sec (default 1 Mbit/sec)\n\r");
		RTK_LOGS(NOTAG, "  	 -c    <host>	run in client mode, connecting to <host>\n\r");
		RTK_LOGS(NOTAG, "  	 -d 			Do a bidirectional test simultaneously\n\r");
		RTK_LOGS(NOTAG, "  	 -t    #		time in seconds to transmit for (default 10 secs)\n\r");
		RTK_LOGS(NOTAG, "  	 -n    #[KM]	number of bytes to transmit (instead of -t)\n\r");
		RTK_LOGS(NOTAG, "       -S    #        set the IP 'type of service'\n\r");
		RTK_LOGS(NOTAG, "   Example:\n\r");
		RTK_LOGS(NOTAG, "       ATWU=-s,-p,5002\n\r");
		RTK_LOGS(NOTAG, "       ATWU=-c,192.168.1.2,-t,100,-p,5002\n\r");
		return;
	}

	argv[0] = (char *)"udp";

	if ((argc = parse_param(arg, argv)) > 1) {
		cmd_iperf(argc, argv);
	}
}
#endif
log_item_t at_wifi_items[ ] = {
#ifdef CONFIG_LWIP_LAYER
	{"ATWI", fATWI, {NULL, NULL}},
	{"ATWT", fATWT, {NULL, NULL}},
	{"ATWU", fATWU, {NULL, NULL}},
#endif
	{"ATPE", fATPE, {NULL, NULL}}, // set static IP for STA
#if WIFI_LOGO_CERTIFICATION_CONFIG
#ifdef CONFIG_SAE_SUPPORT
	{"ATGP", fATWGRP,}, // set SAE group
#endif
	{"ATPM", fATWPMK,},// enable pmk
#ifdef CONFIG_IEEE80211W
	{"ATMF", fATWPMF, {NULL, NULL}},
#endif
#endif // WIFI_LOGO_CERTIFICATION_CONFIG
#if CONFIG_WLAN
	{"ATW0", fATW0, {NULL, NULL}},
	{"ATW1", fATW1, {NULL, NULL}},
	{"ATW2", fATW2, {NULL, NULL}},
	{"ATW3", fATW3, {NULL, NULL}},
	{"ATW4", fATW4, {NULL, NULL}},
	{"ATW5", fATW5, {NULL, NULL}},
	{"ATW6", fATW6, {NULL, NULL}},
	{"ATWf", fATWf, {NULL, NULL}},
#ifdef CONFIG_FPGA
	{"ATW7", fATW7, {NULL, NULL}},
#endif
	{"ATWA", fATWA, {NULL, NULL}},
	{"ATWAO", fATWAO, {NULL, NULL}},
	{"ATWC", fATWC, {NULL, NULL}},
	{"ATWD", fATWD, {NULL, NULL}},
	{"ATWP", fATWP, {NULL, NULL}},
	{"ATWR", fATWR, {NULL, NULL}},
	{"ATWS", fATWS, {NULL, NULL}},
#if SCAN_WITH_SSID
	{"ATWs", fATWs, {NULL, NULL}},
#endif
	{"ATWM", fATWM, {NULL, NULL}},
	{"ATWZ", fATWZ, {NULL, NULL}},

#ifdef CONFIG_WPS
	{"ATWW", fATWW, {NULL, NULL}},
#endif
	{"ATWY", fATWY, {NULL, NULL}},
	{"ATW?", fATWx, {NULL, NULL}},
	{"ATW+ABC", fATWx, {NULL, NULL}},
	{"ATXP", fATXP, {NULL, NULL}},
#endif
};
#else
log_item_t at_wifi_items[ ] = {

};

#endif
void at_wifi_init(void)
{
#if CONFIG_WLAN && !defined(CONFIG_MP_SHRINK)
	init_wifi_struct();
#endif
	log_service_add_table(at_wifi_items, sizeof(at_wifi_items) / sizeof(at_wifi_items[0]));
}

#ifdef SUPPORT_LOG_SERVICE
log_module_init(at_wifi_init);
#endif
