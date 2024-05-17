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
#ifdef CONFIG_WLAN
#include <wifi_conf.h>
#include <wifi_intf_drv_to_upper.h>
#endif

#ifndef CONFIG_MP_INCLUDED
#ifdef CONFIG_LWIP_LAYER
struct static_ip_config user_static_ip;
extern struct netif xnetif[NET_IF_NUM];

extern void cmd_iperf(int argc, char **argv);
extern void cmd_ping(int argc, char **argv);
#endif

#ifdef CONFIG_WLAN
static rtw_network_info_t wifi = {0};
static rtw_softap_info_t ap = {0};
static unsigned char password[129] = {0};
static int security = -1;

#if ENABLE_SET_MAC_ADDRESS
extern int wifi_set_mac_address(int idx, unsigned char *mac, u8 efuse);
#endif
extern int wifi_set_ips_internal(u8 enable);
#ifdef CONFIG_AS_INIC_AP
extern int inic_iwpriv_command(char *cmd, unsigned int cmd_len, int show_msg);
#endif

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
	security = -1;
}

static void print_wifi_setting(unsigned char wlan_idx, rtw_wifi_setting_t *pSetting)
{
#ifndef CONFIG_INIC_NO_FLASH

	at_printf("WLAN%d Setting:\r\n", wlan_idx);
	at_printf("==============================\r\n");

	switch (pSetting->mode) {
	case RTW_MODE_AP:
		at_printf("      MODE => AP\r\n");
		break;
	case RTW_MODE_STA:
		at_printf("      MODE => STATION\r\n");
		break;
	default:
		at_printf("      MODE => UNKNOWN\r\n");
	}
	at_printf("      SSID => %s\r\n", pSetting->ssid);
	at_printf("     BSSID => %02x:%02x:%02x:%02x:%02x:%02x\r\n", pSetting->bssid[0], pSetting->bssid[1], pSetting->bssid[2], pSetting->bssid[3],
			  pSetting->bssid[4], pSetting->bssid[5]);
	at_printf("   CHANNEL => %d\r\n", pSetting->channel);

	if (pSetting->security_type == RTW_SECURITY_OPEN) {
		at_printf("  SECURITY => OPEN\r\n");
	} else if (pSetting->security_type == RTW_SECURITY_WEP_PSK) {
		at_printf("  SECURITY => WEP\r\n");
		at_printf(" KEY INDEX => %d\r\n", pSetting->key_idx);
	} else if (pSetting->security_type == RTW_SECURITY_WPA_TKIP_PSK) {
		at_printf("  SECURITY => WPA TKIP\r\n");
	} else if (pSetting->security_type == RTW_SECURITY_WPA_AES_PSK) {
		at_printf("  SECURITY => WPA AES\r\n");
	} else if (pSetting->security_type == RTW_SECURITY_WPA_MIXED_PSK) {
		at_printf("  SECURITY => WPA MIXED\r\n");
	} else if (pSetting->security_type == RTW_SECURITY_WPA2_AES_PSK) {
		at_printf("  SECURITY => WPA2 AES\r\n");
	} else if (pSetting->security_type == RTW_SECURITY_WPA2_TKIP_PSK) {
		at_printf("  SECURITY => WPA2 TKIP\r\n");
	} else if (pSetting->security_type == RTW_SECURITY_WPA2_MIXED_PSK) {
		at_printf("  SECURITY => WPA2 MIXED\r\n");
	} else if (pSetting->security_type == RTW_SECURITY_WPA_WPA2_TKIP_PSK) {
		at_printf("  SECURITY => WPA/WPA2 TKIP\r\n");
	} else if (pSetting->security_type == RTW_SECURITY_WPA_WPA2_AES_PSK) {
		at_printf("  SECURITY => WPA/WPA2 AES\r\n");
	} else if (pSetting->security_type == RTW_SECURITY_WPA_WPA2_MIXED_PSK) {
		at_printf("  SECURITY => WPA/WPA2 MIXED\r\n");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA_TKIP_PSK | ENTERPRISE_ENABLED)) {
		at_printf("  SECURITY => WPA TKIP ENTERPRISE\r\n");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA_AES_PSK | ENTERPRISE_ENABLED)) {
		at_printf("  SECURITY => WPA AES ENTERPRISE\r\n");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA_MIXED_PSK | ENTERPRISE_ENABLED)) {
		at_printf("  SECURITY => WPA MIXED ENTERPRISE\r\n");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA2_TKIP_PSK | ENTERPRISE_ENABLED)) {
		at_printf("  SECURITY => WPA2 TKIP ENTERPRISE\r\n");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA2_AES_PSK | ENTERPRISE_ENABLED)) {
		at_printf("  SECURITY => WPA2 AES ENTERPRISE\r\n");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA2_MIXED_PSK | ENTERPRISE_ENABLED)) {
		at_printf("  SECURITY => WPA2 MIXED ENTERPRISE\r\n");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA_WPA2_TKIP_PSK | ENTERPRISE_ENABLED)) {
		at_printf("  SECURITY => WPA/WPA2 TKIP ENTERPRISE\r\n");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA_WPA2_AES_PSK | ENTERPRISE_ENABLED)) {
		at_printf("  SECURITY => WPA/WPA2 AES ENTERPRISE\r\n");
	} else if (pSetting->security_type == (RTW_SECURITY_WPA_WPA2_MIXED_PSK | ENTERPRISE_ENABLED)) {
		at_printf("  SECURITY => WPA/WPA2 MIXED ENTERPRISE\r\n");
#ifdef CONFIG_SAE_SUPPORT
	} else if (pSetting->security_type == RTW_SECURITY_WPA3_AES_PSK) {
		at_printf("  SECURITY => WPA3-SAE AES\r\n");
	} else if (pSetting->security_type == RTW_SECURITY_WPA2_WPA3_MIXED) {
		at_printf("  SECURITY => WPA2/WPA3-SAE AES\r\n");
	} else if (pSetting->security_type == (WPA3_SECURITY | ENTERPRISE_ENABLED)) {
		at_printf("  SECURITY => WPA3 ENTERPRISE\r\n");
#endif
	} else {
		at_printf("  SECURITY => UNKNOWN\r\n");
	}

	at_printf("  PASSWORD => %s\r\n", pSetting->password);
	at_printf("\r\n");
#endif
}

static void print_scan_result(rtw_scan_result_t *record)
{
#if (defined(WIFI_LOGO_CERTIFICATION_CONFIG) && WIFI_LOGO_CERTIFICATION_CONFIG)
	at_printf(""MAC_FMT",", MAC_ARG(record->BSSID.octet));
	at_printf("%s,\r\n", record->SSID.val);
#else
	at_printf("%s\t ", (record->bss_type == RTW_BSS_TYPE_ADHOC) ? "Adhoc" : "Infra");
	at_printf(""MAC_FMT",", MAC_ARG(record->BSSID.octet));
	at_printf(" %d\t ", record->signal_strength);
	at_printf(" %d\t  ", record->channel);
	at_printf(" %d\t  ", (unsigned int)record->wps_type);
	at_printf("%s\t\t ", (record->security == RTW_SECURITY_OPEN) ? "Open               " :
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

	at_printf(" %s ", record->SSID.val);
	at_printf("\r\n");
#endif
}

static rtw_result_t app_scan_result_handler(unsigned int scanned_AP_num, void *user_data)
{
	rtw_scan_result_t *scanned_AP_info;
	char *scan_buf = NULL;
	unsigned int i = 0;

	UNUSED(user_data);

	if (scanned_AP_num == 0) {/* scanned no AP*/
		return RTW_ERROR;
	}

	scan_buf = (char *)rtos_mem_zmalloc(scanned_AP_num * sizeof(rtw_scan_result_t));
	if (scan_buf == NULL) {
		return RTW_ERROR;
	}

	if (wifi_get_scan_records(&scanned_AP_num, scan_buf) < 0) {
		rtos_mem_free((void *)scan_buf);
		return RTW_ERROR;
	}

	for (i = 0; i < scanned_AP_num; i++) {
#if (defined(WIFI_LOGO_CERTIFICATION_CONFIG) && WIFI_LOGO_CERTIFICATION_CONFIG)
		at_printf("[%d],", (i + 1));
#else
		at_printf("%d\t ", (i + 1));
#endif
		scanned_AP_info = (rtw_scan_result_t *)(scan_buf + i * sizeof(rtw_scan_result_t));
		scanned_AP_info->SSID.val[scanned_AP_info->SSID.len] = 0; /* Ensure the SSID is null terminated */

		print_scan_result(scanned_AP_info);
	}
	rtos_mem_free((void *)scan_buf);

	return RTW_SUCCESS;
}

static void at_wlconn_help(void)
{
	at_printf("\r\n");
	at_printf("AT+WLCONN=<ssid>,<pwd>[,<key_id>,<bssid>]\r\n");
	at_printf("\t<ssid>:\t The wifi ssid, this parameter can not be empty\r\n");
	at_printf("\t<pwd>:\tWPA or WPA2 with length 8~64, WEP with length 5 or 13\r\n");
	at_printf("\t<key_id>:\tFor WEP security, must be 0~3, if absent, it is 0\r\n");
	at_printf("\t<bssid>:\tA hexnumber of 6 bytes, e.g. \"11aa22cc33ee\"\r\n");
}

/****************************************************************
AT command process:
	AT+WLCONN
	Wifi AT Command:
	Connect to a wifi network.
	[+WLCONN]:OK
****************************************************************/
void at_wlconn(void *arg)
{
	const int ssid_idx = 1, password_idx = 2, keyid_idx = 3, bssid_idx = 4, channel_idx = 5;
	int argc = 0, error_no = 0, ret = 0, i = 0;
	unsigned char channel = 0;
	unsigned int mac[ETH_ALEN];
	char *argv[MAX_ARGC] = {0};
	char empty_bssid[6] = {0};
	char assoc_by_bssid = 0;
	unsigned long tick1 = rtos_time_get_current_system_time_ms();
	unsigned long tick2;
#ifdef CONFIG_LWIP_LAYER
	unsigned long tick3;
#endif
	struct _rtw_wifi_setting_t *p_wifi_setting = NULL;

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[+WLCONN] The parameters can not be ignored\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if ((argc < 2) || (argc > 6)) {
		RTK_LOGW(NOTAG, "[+WLCONN] The parameters format ERROR\r\n");
		error_no = 1;
		goto end;
	}

	/* SSID */
	wifi.ssid.len = strlen(argv[ssid_idx]);
	if ((argv[ssid_idx] == NULL) || (wifi.ssid.len >= INIC_MAX_SSID_LENGTH)) {
		RTK_LOGW(NOTAG, "[+WLCONN] Invalid SSID\r\n");
		error_no = 2;
		goto end;
	}
	strncpy((char *)wifi.ssid.val, argv[ssid_idx], sizeof(wifi.ssid.val) - 1);

	/* Password (maybe not exist) */
	if ((argc > password_idx) && (argv[password_idx] != NULL)) {
		strncpy((char *)password, argv[password_idx], sizeof(password) - 1);
		wifi.password = password;
		wifi.password_len = strlen(argv[password_idx]);
	}

	/* KEYID (maybe not exist) */
	if ((argc > keyid_idx) && (argv[keyid_idx] != NULL)) {
		if ((strlen(argv[keyid_idx]) != 1) || (*(char *)argv[keyid_idx] < '0' || *(char *)argv[keyid_idx] > '3')) {
			RTK_LOGW(NOTAG, "[+WLCONN] Invalid KEYID\r\n");
			error_no = 2;
			goto end;
		}
		wifi.key_id = atoi(argv[keyid_idx]);
	}

	/* BSSID (maybe not exist) */
	if ((argc > bssid_idx) && (argv[bssid_idx] != NULL)) {
		if (strlen(argv[bssid_idx]) != 12) {
			RTK_LOGW(NOTAG, "[+WLCONN] Invalid BSSID\r\n");
			error_no = 2;
			goto end;
		}
		_sscanf_ss(argv[bssid_idx], MAC_FMT, &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
		for (i = 0; i < ETH_ALEN; i++) {
			wifi.bssid.octet[i] = mac[i] & 0xFF;
		}
	}

	/* Channel (maybe not exist) */
	if ((argc > channel_idx) && (argv[channel_idx] != NULL)) {
		channel = (unsigned char)atoi(argv[channel_idx]);
	}
	wifi.channel = channel;

	/* Connect to network */
	p_wifi_setting = (struct _rtw_wifi_setting_t *)rtos_mem_zmalloc(sizeof(struct _rtw_wifi_setting_t));
	if (p_wifi_setting == NULL) {
		RTK_LOGW(NOTAG, "[+WLCONN] alloc p_wifi_setting fail\r\n");
		error_no = 3;
		goto end;
	}

	/* Check bssid */
	if (memcmp(wifi.bssid.octet, empty_bssid, sizeof(empty_bssid))) {
		assoc_by_bssid = 1;
	} else if (wifi.ssid.val[0] == 0) {
		RTK_LOGW(NOTAG, "[+WLCONN] SSID should exist here\r\n");
		error_no = 3;
		goto end;
	}

	/* Check password. */
	if (wifi.password != NULL) {
		wifi.security_type = ((wifi.key_id >= 0) && (wifi.key_id <= 3)) ? RTW_SECURITY_WEP_SHARED : RTW_SECURITY_WPA2_AES_PSK;
	} else {
		wifi.security_type = RTW_SECURITY_OPEN;
	}

	/* Check AP mode */
	wifi_get_setting(STA_WLAN_INDEX, p_wifi_setting);
	if (p_wifi_setting->mode == RTW_MODE_AP) {
#ifdef CONFIG_LWIP_LAYER
		dhcps_deinit();
#endif
		if (wifi_on(RTW_MODE_STA) < 0) {
			RTK_LOGW(NOTAG, "[+WLCONN] Wifi on failed\r\n");
			error_no = 5;
			goto end;
		}
	}

	/* Connecting ...... */
	if (assoc_by_bssid) {
		RTK_LOGI(NOTAG, "Joining BSS by BSSID "MAC_FMT" ...\r\n", MAC_ARG(wifi.bssid.octet));
	} else {
		RTK_LOGI(NOTAG, "Joining BSS by SSID %s...\r\n", (char *)wifi.ssid.val);
	}
	ret = wifi_connect(&wifi, 1);
	if (ret != RTW_SUCCESS) {
		if (ret == RTW_INVALID_KEY) {
			RTK_LOGW(NOTAG, "[+WLCONN] Invalid Key set\r\n");
		}
		RTK_LOGW(NOTAG, "[+WLCONN] Can not connect to AP\r\n");
		error_no = 4;
		goto end;
	}

	tick2 = rtos_time_get_current_system_time_ms();
	RTK_LOGI(NOTAG, "[+WLCONN] Connected after %d ms.\r\n", (unsigned int)(tick2 - tick1));

#ifdef CONFIG_LWIP_LAYER
	/* Start DHCPClient */
	LwIP_DHCP(0, DHCP_START);
	tick3 = rtos_time_get_current_system_time_ms();
	RTK_LOGI(NOTAG, "[+WLCONN] Got IP after %d ms.\r\n", (unsigned int)(tick3 - tick1));
#endif

end:
	rtos_mem_free((void *)p_wifi_setting);
	init_wifi_struct();
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+WLCONN:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+WLCONN:", error_no);
		if (error_no == 1 || error_no == 2) {
			at_wlconn_help();
		}
	}
}

/****************************************************************
AT command process:
	AT+WLDISCONN
	Wifi AT Command:
	Disconnect from a wifi network.
	[+WLDISCONN]:OK
****************************************************************/
void at_wldisconn(void *arg)
{
	int timeout = 20;
	int error_no = 0, ret = 0;
	struct _rtw_wifi_setting_t wifi_setting = {RTW_MODE_NONE, {0}, {0}, 0, RTW_SECURITY_OPEN, {0}, 0, 0, 0, 0, 0};

	UNUSED(arg);

	if (wifi_get_setting(STA_WLAN_INDEX, &wifi_setting) >= 0) {
		if (wifi_setting.mode == RTW_MODE_AP) {
			RTK_LOGI(NOTAG, "[+WLDISCONN] No need do disconnect for softap mode\r\n");
			goto end;
		}
	} else {
		RTK_LOGI(NOTAG, "[+WLDISCONN] No need do disconnect when netif is down\r\n");
		goto end;
	}

	if (wifi_is_connected_to_ap() != RTW_SUCCESS) {
		RTK_LOGI(NOTAG, "[+WLDISCONN] Not connected yet\r\n");
		goto end;
	}

	/* Disconnecting ...... */
	ret = wifi_disconnect();
	if (ret < 0) {
		RTK_LOGW(NOTAG, "[+WLDISCONN] Disconnect ERROR\r\n");
		error_no = 3;
		goto end;
	}

	/* error_no == 4 means time out. */
	error_no = 4;
	while (timeout > 0) {
		if (wifi_is_connected_to_ap() != RTW_SUCCESS) {
			RTK_LOGI(NOTAG, "[+WLDISCONN] disconnect done\r\n");
			error_no = 0;
			break;
		}

		/* Delay 1s */
		rtos_time_delay_ms(1000);
		timeout--;
	}

end:
#ifdef CONFIG_LWIP_LAYER
	user_static_ip.use_static_ip = 0;
	LwIP_ReleaseIP(STA_WLAN_INDEX);
#endif
	init_wifi_struct();
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+WLDISCONN:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+WLDISCONN:", error_no);
	}
}

void at_wlscan_help(void)
{
	at_printf("\r\n");
	at_printf("AT+WLSCAN\r\n");
	at_printf("AT+WLSCAN=<num_channel>[,chl1,chl2,chl3,......]\r\n");
	at_printf("\tIf add parameters, the length of chlx list must be same as <num_channel>\r\n");
}

/****************************************************************
AT command process:
	AT+WLSCAN
	Wifi AT Command:
	Scan all the SSIDs.
	[+WLSCAN]:OK
****************************************************************/
void at_wlscan(void *arg)
{
	u8 *channel_list = NULL;
	int num_channel = 0;
	int i, argc = 0;
	int error_no = 0, ret = 0;
	char *argv[MAX_ARGC] = {0};
	struct _rtw_scan_param_t scan_param = {RTW_SCAN_NOUSE, 0, 0, 0, {0}, 0, 0, 0, 0};
	enum rtw_join_status_type join_status = RTW_JOINSTATUS_UNKNOWN;

	join_status = wifi_get_join_status();
	if ((join_status > RTW_JOINSTATUS_UNKNOWN) && (join_status < RTW_JOINSTATUS_SUCCESS)) {
		error_no = 1;
		goto end;
	}

	if (arg != NULL) {
		argc = parse_param(arg, argv);
		if (argc < 2) {
			error_no = 1;
			goto end;
		}
		num_channel = atoi(argv[1]);
		channel_list = (u8 *)rtos_mem_zmalloc(num_channel);
		if (channel_list == NULL) {
			RTK_LOGW(NOTAG, "[+WLSCAN]ERROR: Can not malloc memory for channel list\r\n");
			error_no = 3;
			goto end;
		}
		/* parse channel list */
		for (i = 2; i <= argc - 1 ; i++) {
			*(channel_list + i - 2) = (u8)atoi(argv[i]);
		}
		scan_param.channel_list = channel_list;
		scan_param.channel_list_num = num_channel;
	}

	scan_param.scan_user_callback = app_scan_result_handler;
	ret = wifi_scan_networks(&scan_param, 0);
	if (ret != RTW_SUCCESS) {
		RTK_LOGW(NOTAG, "[+WLSCAN] wifi_scan_networks ERROR\r\n");
		error_no = 5;
		goto end;
	}

end:
	rtos_mem_free((void *)channel_list);
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+WLSCAN:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+WLSCAN:", error_no);
		if (error_no == 1) {
			at_wlscan_help();
		}
	}
}

void at_wlscanssid_help(void)
{
	at_printf("\r\n");
	at_printf("AT+WLSCANSSID\r\n");
	at_printf("AT+WLSCANSSID=<num_channel>,[chl1,chl2,chl3,......]\r\n");
	at_printf("\t<num_channel>:\tThe channel number it will scan\r\n");
	at_printf("\t[list......]:\tThe channel list should be same length as num_channel\r\n");
}

/****************************************************************
AT command process:
	AT+WLSCANSSID
	Wifi AT Command:
	Scan the specific SSID.
	[+WLSCANSSID]:OK
****************************************************************/
void at_wlscanssid(void *arg)
{
	const int ssid_idx = 1, channel_start = 2;
	int argc = 0, i = 0, ret = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	u8 *channel_list = NULL;
	int num_channel = 0;
	rtw_scan_param_t scan_param;

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[+WLSCANSSID] NULL parameter\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc < 2) {
		RTK_LOGW(NOTAG, "[+WLSCANSSID] Invalid parameter\r\n");
		error_no = 1;
		goto end;
	}

	/* SSID */
	wifi.ssid.len = strlen(argv[ssid_idx]);
	if ((argv[ssid_idx] == NULL) || (wifi.ssid.len >= INIC_MAX_SSID_LENGTH)) {
		RTK_LOGW(NOTAG, "[+WLSCANSSID] Invalid SSID\r\n");
		error_no = 2;
		goto end;
	}
	strncpy((char *)wifi.ssid.val, argv[ssid_idx], sizeof(wifi.ssid.val) - 1);

	/* Channel list */
	if (argc > channel_start) {
		num_channel = argc - channel_start;
		channel_list = (u8 *)rtos_mem_malloc(num_channel);
		if (channel_list == NULL) {
			RTK_LOGW(NOTAG, "[+WLSCANSSID] Memory Failure\r\n");
			error_no = 3;
			goto end;
		}
		/* Get channel list */
		for (i = channel_start; i < argc; i++) {
			channel_list[i - channel_start] = (u8)atoi(argv[i]);
		}
	}

	memset(&scan_param, 0, sizeof(rtw_scan_param_t));
	scan_param.ssid = (char *)wifi.ssid.val;
	scan_param.scan_user_callback = app_scan_result_handler;
	scan_param.channel_list = channel_list;
	scan_param.channel_list_num = num_channel;
	ret = wifi_scan_networks(&scan_param, 0);
	if (ret != RTW_SUCCESS) {
		RTK_LOGI(NOTAG, "[WLSCANSSID]ERROR: wifi scan failed\r\n");
		error_no = 4;
		goto end;
	}

end:
	init_wifi_struct();
	rtos_mem_free((void *)channel_list);
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+WLSCANSSID:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+WLSCANSSID:", error_no);
		if (error_no == 1 || error_no == 2) {
			at_wlscanssid_help();
		}
	}
}

/****************************************************************
AT command process:
	AT+WLRSSI
	Wifi AT Command:
	Get the RSSI value of current network.
	[+WLRSSI]:OK
****************************************************************/
void at_wlrssi(void *arg)
{
	struct _rtw_phy_statistics_t phy_statistics;

	UNUSED(arg);

	RTK_LOGI(NOTAG, "[WLRSSI] _AT_WLAN_GET_RSSI_\r\n");
	wifi_fetch_phy_statistic(&phy_statistics);
	at_printf("rssi = %d\r\n", phy_statistics.rssi);
	at_printf("data rssi = %d\r\n", phy_statistics.data_rssi);
	at_printf("beacon rssi = %d\r\n", phy_statistics.beacon_rssi);
	at_printf("\r\n%sOK\r\n", "+WLRSSI:");
}

void at_wlstartap_help(void)
{
	at_printf("\r\n");
	at_printf("AT+WLSTARTAP=<ssid>[,<channel>,<open/wep/tkip/wpa2/wpa3>,<password>]\r\n");
	at_printf("\t<ssid>:\tThe ssid of AP, could not be empty\r\n");
	at_printf("\t<channel>:\t[1,11]\r\n");
	at_printf("\t<open/wep/tkip/wpa2/wpa3>:\t[0,3]\r\n");
	at_printf("\t<password>:\tWith length in [8,64]\r\n");
}

/****************************************************************
AT command process:
	AT+WLSTARTAP
	Wifi AT Command:
	Set the wifi soft AP.
	[+WLSTARTAP]:OK
****************************************************************/
void at_wlstartap(void *arg)
{
	const int ssid_idx = 1, channel_idx = 2, security_idx = 3, password_idx = 4;
	int argc = 0, error_no = 0, ret = 0;
	char *argv[MAX_ARGC] = {0};
#ifdef CONFIG_LWIP_LAYER
	u32 ip_addr, netmask, gw;
	struct netif *pnetif = &xnetif[SOFTAP_WLAN_INDEX];
#endif
	int timeout = 20;
	struct _rtw_wifi_setting_t *setting = NULL;

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[+WLSTARTAP] The parameters can not be ignored\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if ((argc < 2) || (argc > 5)) {
		RTK_LOGW(NOTAG, "[+WLSTARTAP] command format error\r\n");
		error_no = 2;
		goto end;
	}

	/* SSID */
	if (argv[ssid_idx] != NULL) {
		ap.ssid.len = strlen(argv[ssid_idx]);
		if (ap.ssid.len >= INIC_MAX_SSID_LENGTH) {
			RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid SSID length\r\n");
			error_no = 2;
			goto end;
		}
		strncpy((char *)ap.ssid.val, argv[ssid_idx], sizeof(ap.ssid.val) - 1);
	} else {
		RTK_LOGW(NOTAG, "[+WLSTARTAP] NULL SSID\r\n");
		error_no = 2;
		goto end;
	}

	/* Channel */
	if (argc > channel_idx) {
		if (argv[channel_idx] != NULL) {
			ap.channel = atoi(argv[channel_idx]);
		} else {
			RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid channel\r\n");
			error_no = 2;
			goto end;
		}
	}

	/* Security <open/wep/tkip/wpa2/wpa3> (maybe not exist) */
	if (argc > security_idx) {
		if (argv[security_idx] == NULL) {
			RTK_LOGW(NOTAG, "[+WLSTARTAP] ERROR <open/wep/tkip/wpa2/wpa3>\r\n");
			error_no = 2;
			goto end;
		}
		if (0 == strcmp("open", argv[security_idx])) {
			security = 0;
		} else if (0 == strcmp("wep", argv[security_idx])) {
			security = 1;
		} else if (0 == strcmp("tpic", argv[security_idx])) {
			security = 2;
		} else if (0 == strcmp("wpa2", argv[security_idx])) {
			security = 3;
		} else if (0 == strcmp("wpa3", argv[security_idx])) {
			security = 4;
		} else {
			RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid security value\r\n");
			error_no = 2;
			goto end;
		}
	}

	/* Password (maybe not exist) */
	if (argc > password_idx) {
		if (argv[password_idx] != NULL) {
			ap.password_len = strlen(argv[password_idx]);
			if (ap.password_len > 64) {
				RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid password length\r\n");
				error_no = 2;
				goto end;
			}
			strncpy((char *)password, argv[password_idx], sizeof(password) - 1);
			ap.password = password;
		} else {
			RTK_LOGW(NOTAG, "[+WLSTARTAP] NULL password\r\n");
			error_no = 2;
			goto end;
		}
	}

	/* Check SSID */
	if (ap.ssid.val[0] == 0) {
		RTK_LOGW(NOTAG, "[+WLSTARTAP] Empty SSID here\r\n");
		error_no = 2;
		goto end;
	}

	/* Check password */
	if (ap.password == NULL) {
		ap.security_type = RTW_SECURITY_OPEN;
	} else {
		if (ap.password_len <= RTW_WPA2_MAX_PSK_LEN &&
			ap.password_len >= RTW_MIN_PSK_LEN) {
			ap.security_type = RTW_SECURITY_WPA2_AES_PSK;
		} else if (ap.password_len == 5) {
			ap.security_type = RTW_SECURITY_WEP_PSK;
		} else {
			RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid password saved\r\n");
			error_no = 2;
			goto end;
		}
	}

	/* Check security */
	if (security == 0) {
		ap.security_type = RTW_SECURITY_OPEN;
	} else if (security == 1) {
		ap.security_type = RTW_SECURITY_WEP_PSK;
	} else if (security == 2) {
		ap.security_type = RTW_SECURITY_WPA2_TKIP_PSK;
	} else if (security == 3) {
		ap.security_type = RTW_SECURITY_WPA2_AES_PSK;
	} else if (security == 4) {
		ap.security_type = RTW_SECURITY_WPA3_AES_PSK;
	}
	if ((security > 0) && (ap.password == NULL)) {
		RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid password, please input it\r\n");
		error_no = 2;
		goto end;
	}

#ifdef CONFIG_LWIP_LAYER
	dhcps_deinit();
	ip_addr = WIFI_MAKEU32(GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	netmask = WIFI_MAKEU32(NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
	gw = WIFI_MAKEU32(GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	LwIP_SetIP(SOFTAP_WLAN_INDEX, ip_addr, netmask, gw);
#endif

	/* Starting ...... */
	wifi_stop_ap();
	ret = wifi_start_ap(&ap);
	if (ret < 0) {
		RTK_LOGW(NOTAG, "[+WLSTARTAP] wifi_start_ap failed\r\n");
		error_no = 4;
		goto end;
	}

	setting = rtos_mem_zmalloc(sizeof(struct _rtw_wifi_setting_t));
	if (setting == NULL) {
		RTK_LOGW(NOTAG, "[+WLSTARTAP] memory failed for setting\r\n");
		error_no = 5;
		goto end;
	}

	/* error_no = 3 means time out. */
	error_no = 3;
	while (timeout > 0) {
		wifi_get_setting(SOFTAP_WLAN_INDEX, setting);
		if (strlen((char *)setting->ssid) > 0) {
			if (strcmp((char *) setting->ssid, (char *)ap.ssid.val) == 0) {
				RTK_LOGI(NOTAG, "[+WLSTARTAP] %s started\n", ap.ssid.val);
				error_no = 0;
				break;
			}
		}

		/* Delay 1s */
		rtos_time_delay_ms(1000);
		timeout--;
	}

#ifdef CONFIG_LWIP_LAYER
	ip_addr = WIFI_MAKEU32(AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
	netmask = WIFI_MAKEU32(AP_NETMASK_ADDR0, AP_NETMASK_ADDR1, AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
	gw = WIFI_MAKEU32(AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
	LwIP_SetIP(SOFTAP_WLAN_INDEX, ip_addr, netmask, gw);
	dhcps_init(pnetif);
#endif

end:
	rtos_mem_free((void *)setting);
	init_wifi_struct();
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+WLSTARTAP:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+WLSTARTAP:", error_no);
		if (error_no == 1 || error_no == 2) {
			at_wlstartap_help();
		}
	}
}

/****************************************************************
AT command process:
	AT+WLSTOPAP
	Wifi AT Command:
	Set the wifi soft AP.
	[+WLSTOPAP]:OK
****************************************************************/
void at_wlstopap(void *arg)
{
	UNUSED(arg);

	wifi_stop_ap();
	at_printf("\r\n%sOK\r\n", "+WLSTOPAP:");
}

/****************************************************************
AT command process:
	AT+WLSTATE
	Wifi AT Command:
	Get the parameters of network.
	[+WLSTATE]:OK
****************************************************************/
void at_wlstate(void *arg)
{
	int i = 0;
#ifdef CONFIG_LWIP_LAYER
	u8 *mac = LwIP_GetMAC(0);
	u8 *ip = LwIP_GetIP(0);
	u8 *gw = LwIP_GetGW(0);
	u8 *msk = LwIP_GetMASK(0);
#endif
	struct _rtw_wifi_setting_t *p_wifi_setting = NULL;
	rtw_sw_statistics_t stats;

	UNUSED(arg);

	p_wifi_setting = (struct _rtw_wifi_setting_t *)rtos_mem_zmalloc(sizeof(struct _rtw_wifi_setting_t));
	if (p_wifi_setting == NULL) {
		RTK_LOGW(NOTAG, "[+WLSTATE]: alloc p_wifi_setting fail \r\n");
		at_printf("\r\n%sERROR: %d\r\n", "+WLSTATE:", 1);
		return;
	}

	RTK_LOGI(NOTAG, "[+WLSTATE]: _AT_WLAN_INFO_\r\n");
	for (i = 0; i < NET_IF_NUM; i++) {
		if (wifi_is_running(i)) {
#ifdef CONFIG_LWIP_LAYER
			mac = LwIP_GetMAC(i);
			ip = LwIP_GetIP(i);
			gw = LwIP_GetGW(i);
			msk = LwIP_GetMASK(i);
#endif
			at_printf("WLAN%d Status: Running\r\n",  i);
			at_printf("==============================\r\n");

			wifi_get_sw_statistic(i, &stats);
			if (i == 0) {
				at_printf("max_skbinfo_used_num=%d, skbinfo_used_num=%d\r\n", stats.max_skbbuf_used_number, stats.skbbuf_used_number);
				at_printf("max_skbdata_used_num=%d, skbdata_used_num=%d\r\n", stats.max_skbdata_used_number, stats.skbdata_used_number);
			}
			wifi_get_setting(i, p_wifi_setting);
			print_wifi_setting(i, p_wifi_setting);

#ifdef CONFIG_LWIP_LAYER
			at_printf("Interface (%d)\r\n", i);
			at_printf("==============================\r\n");
			at_printf("\tMAC => %02x:%02x:%02x:%02x:%02x:%02x\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]) ;
			at_printf("\tIP  => %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
			at_printf("\tGW  => %d.%d.%d.%d\r\n", gw[0], gw[1], gw[2], gw[3]);
			at_printf("\tmsk  => %d.%d.%d.%d\r\n\r\n", msk[0], msk[1], msk[2], msk[3]);
#endif
			if (p_wifi_setting->mode == RTW_MODE_AP || i == 1) {
				unsigned int client_number;
				rtw_client_list_t client_info = {0};
				wifi_get_associated_client_list(&client_info);

				at_printf("Associated Client List:\r\n");
				at_printf("==============================\r\n");

				if (client_info.count == 0) {
					at_printf("Client Num: %d\r\n\r\n", client_info.count);
				} else {
					at_printf("Client Num: %d\r\n", client_info.count);
					for (client_number = 0; client_number < client_info.count; client_number++) {
						at_printf("Client %d:\r\n", client_number + 1);
						at_printf("\tMAC => "MAC_FMT"\r\n",
								  MAC_ARG(client_info.mac_list[client_number].octet));
					}
					at_printf("\r\n");
				}
			}
		} else {
			/* show the ethernet interface info */
#if defined(CONFIG_ETHERNET) && CONFIG_ETHERNET
			if (i == NET_IF_NUM - 1) {
#ifdef CONFIG_LWIP_LAYER
				mac = LwIP_GetMAC(i);
				ip = LwIP_GetIP(i);
				gw = LwIP_GetGW(i);
				at_printf("Interface ethernet\r\n");
				at_printf("==============================\r\n");
				at_printf("\tMAC => %02x:%02x:%02x:%02x:%02x:%02x\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]) ;
				at_printf("\tIP  => %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
				at_printf("\tGW  => %d.%d.%d.%d\r\n\r\n", gw[0], gw[1], gw[2], gw[3]);
#endif /* CONFIG_LWIP_LAYER */
			}
#endif /* CONFIG_ETHERNET */
		}
	}

	rtos_mem_free((void *)p_wifi_setting);
	at_printf("\r\n%sOK\r\n", "+WLSTATE:");

#if defined(CONFIG_IP_NAT) && (CONFIG_IP_NAT == 1)
	ipnat_dump();
#endif

#if defined(configUSE_TRACE_FACILITY) && (configUSE_TRACE_FACILITY == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS == 1)
	{
		signed char pcWriteBuffer[1024];
		vTaskList((char *)pcWriteBuffer);
		at_printf("Task List: \r\n%s\r\n", pcWriteBuffer);
	}
#endif
}

static void at_wlautoconn_help(void)
{
	at_printf("\r\n");
	at_printf("AT+WLAUTOCONN=<mode>");
	at_printf("\t<mode>:\t1: disable auto-reconnect, 2: clear stored flash data\r\n");
}

/****************************************************************
AT command process:
	AT+WLAUTOCONN
	Wifi AT Command:
	Set auto-connection.
	[+WLAUTOCONN]:OK
****************************************************************/
void at_wlautoconn(void *arg)
{
	int error_no = 0;
	int argc = 0, mode = 0;
	char *argv[MAX_ARGC] = {0};

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[+WLAUTOCONN] Invalid parameter\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 2 || argv[1] == NULL) {
		RTK_LOGW(NOTAG, "[+WLAUTOCONN] Invalid parameter number\r\n");
		error_no = 1;
		goto end;
	}

	mode = atoi(argv[1]);
	if (mode == 0) {
		RTK_LOGI(NOTAG, "[+WLAUTOCONN] Disable autoreconnect\r\n");
		wifi_config_autoreconnect(RTW_AUTORECONNECT_DISABLE);
	} else if (mode == 1) {
		RTK_LOGI(NOTAG, "[+WLAUTOCONN] Enable autoreconnect\r\n");
		wifi_config_autoreconnect(RTW_AUTORECONNECT_FINITE);
	} else {
		error_no = 2;
	}

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+WLAUTOCONN:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+WLAUTOCONN:", error_no);
		at_wlautoconn_help();
	}
}

#if ENABLE_SET_MAC_ADDRESS
static void at_wlmac_help(void)
{
	at_printf("\r\n");
	at_printf("AT+WLMAC=<mac_addr>[,<er_idx>,<i_idx>]");
	at_printf("\t<mac_addr>:\tA hexnumber of 6 bytes, e.g. 2c033ad355f1\r\n");
	at_printf("\t<er_idx>:\tStore in efuse or RAM? 0 in RAM, 1 in efuse\r\n");
	at_printf("\t<i_idx>:\tNet device index\r\n");
}

/****************************************************************
AT command process:
	AT+WLMAC
	Wifi AT Command:
	Set MAC address.
	[+WLMAC]:OK
****************************************************************/
void at_wlmac(void *arg)
{
	const int mac_idx = 1, er_idx = 2, i_idx = 3;
	int argc = 0, ret = 0, error_no = 0;
	int i = 0, efuse_ram = 0;
	char *argv[MAX_ARGC] = {0};

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[+WLMAC] Invalid parameter\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if ((argc < 2) || (argc > 4)) {
		RTK_LOGW(NOTAG, "[+WLMAC] Invalid parameter number\r\n");
		error_no = 1;
		goto end;
	}

	if (strlen(argv[mac_idx]) != 12) {
		RTK_LOGW(NOTAG, "[+WLMAC] Invalid MAC address\r\n");
		error_no = 1;
		goto end;
	}
	for (i = 0; i < 12; i++) {
		/* Ensure it is a valid MAC address. */
		if (!(('0' <= argv[mac_idx][i] && '9' >= argv[mac_idx][i])
			  || ('a' <= argv[mac_idx][i] && 'f' >= argv[mac_idx][i])
			  || ('A' <= argv[mac_idx][i] && 'F' >= argv[mac_idx][i]))) {
			RTK_LOGW(NOTAG, "[+WLMAC] Invalid MAC address string\r\n");
			error_no = 1;
			goto end;
		}
	}

	/* Efuse or RAM. If this parameter is absent, use default 0 (RAM). */
	if ((argc > er_idx) && (argv[er_idx] != NULL)) {
		efuse_ram = atoi(argv[er_idx]);
		if ((efuse_ram != 0) && (efuse_ram != 1)) {
			RTK_LOGW(NOTAG, "[+WLMAC] Invalid efuse_ram value\r\n");
			error_no = 2;
			goto end;
		}
	}

	/* Index */
	if ((argc > i_idx) && (argv[i_idx] != NULL)) {
		i = atoi(argv[i_idx]);
		if ((i < 0) || (i >= NET_IF_NUM)) {
			RTK_LOGW(NOTAG, "[+WLMAC] Invalid Index value\r\n");
			error_no = 2;
			goto end;
		}
	} else {
		/* Default idx = 0 */
		i = 0;
	}

	ret = wifi_set_mac_address(i, (unsigned char *)argv[mac_idx], efuse_ram);
	if (ret != RTW_SUCCESS) {
		RTK_LOGW(NOTAG, "[+WLMAC] wifi_set_mac_address failed\r\n");
		error_no = 3;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+WLMAC:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+WLMAC:", error_no);
		if (error_no == 1 || error_no == 2) {
			at_wlmac_help();
		}
	}
}
#endif

static void at_wlpromisc_help(void)
{
	at_printf("\r\n");
	at_printf("AT+WLPROMISC=<enable>[,<all_apall>]\r\n");
	at_printf("\t<enable>:\t\"enable\" or \"disable\"\r\n");
	at_printf("\t<all_apall>:\t\"all\" or \"apall\" only when enabled\r\n");
}

/****************************************************************
AT command process:
	AT+WLPROMISC
	Wifi AT Command:
	[+WLPROMISC]:OK
****************************************************************/
void at_wlpromisc(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	promisc_para_t promisc_para;
	u32 status;

	memset(&promisc_para, 0, sizeof(promisc_para_t));

	argc = parse_param(arg, argv);
	if (argc > 1) {
		if (argv[1] == NULL) {
			RTK_LOGW(NOTAG, "[WLPROMISC]: Input ERROR parameter\r\n");
			error_no = 1;
			goto end;
		}
		if (strcmp((char *)argv[1], "enable") == 0) {
			if (argc != 3 || argv[2] == NULL) {
				RTK_LOGW(NOTAG, "[WLPROMISC]: Should be filter mode here\r\n");
				error_no = 2;
				goto end;
			}
			status = ENABLE;
			if (strcmp((char *)argv[2], "all") == 0) {
				promisc_para.filter_mode = RCR_ALL_PKT;
			} else if (strcmp((char *)argv[2], "apall") == 0) {
				promisc_para.filter_mode = RCR_AP_ALL;
			} else {
				RTK_LOGW(NOTAG, "[WLPROMISC]: WRONG filter mode\r\n");
				error_no = 3;
				goto end;
			}
		} else if (strcmp((char *)argv[1], "disable") == 0) {
			status = DISABLE;
		} else {
			RTK_LOGW(NOTAG, "[WLPROMISC]: ERROR parameter\r\n");
			error_no = 1;
			goto end;
		}
		wifi_promisc_enable(status, &promisc_para);
	}

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+WLPROMISC:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+WLPROMISC:", error_no);
		at_wlpromisc_help();
	}
}

static void at_wldbg_help(void)
{
	at_printf("\r\n");
	at_printf("AT+WLDBG=<command>[,<parameters>]\r\n");
}

/****************************************************************
AT command process:
	AT+WLDBG
	Wifi AT Command:
	[+WLDBG]:OK
****************************************************************/
void at_wldbg(void *arg)
{
	char buf[64] = {0};
	char *copy = buf;
	int i = 0;
	int len = 0;
	int error_no = 0;
	int ret = 0;

	RTK_LOGI(NOTAG, "[WLDBG]: _AT_WLAN_IWPRIV_\r\n");
	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[WLDBG]Usage: AT+WLDBG=COMMAND[PARAMETERS]\r\n");
		error_no = 1;
		goto end;
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
	ret = inic_iwpriv_command(copy, strlen(copy) + 1, 1);
#else
	ret = rtw_iwpriv_command(STA_WLAN_INDEX, copy, 1);
#endif
	if (ret != RTW_SUCCESS) {
		RTK_LOGW(NOTAG, "[WLDBG] Failed while iwpriv\r\n");
		error_no = 2;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+WLDBG:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+WLDBG:", error_no);
		if (error_no == 1) {
			at_wldbg_help();
		}
	}
}

#ifdef CONFIG_WPS
static void at_wlwps_help(void)
{
	at_printf("\r\n");
	at_printf("AT+WLWPS=<pbc_pin>\r\n");
}

/****************************************************************
AT command process:
	AT+WLWPS
	Wifi AT Command:
	[+WLWPS]:OK
****************************************************************/
void at_wlwps(void *arg)
{
	int error_no = 0;
#if defined(CONFIG_ENABLE_WPS) && CONFIG_ENABLE_WPS
	int argc = 0, wps_argc = 0;
	char *argv[MAX_ARGC] = {0};
	char *wps_argv[4];

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[WLWPS]: ERROR parameter\r\n");
		error_no = 1;
		goto end;
	}
	argc = parse_param(arg, argv);
	if (argc != 2 || argv[1] == NULL) {
		RTK_LOGW(NOTAG, "[WLWPS]: Should be pbc or pin here\r\n");
		error_no = 1;
		goto end;
	}

	wps_argv[wps_argc++] = "wifi_wps";
	wps_argv[wps_argc++] = argv[1];
	cmd_wps(wps_argc, wps_argv);

#else
	UNUSED(arg);

	RTK_LOGW(NOTAG, "[WLWPS]: Not supported\r\n");
	error_no = 2;
	goto end;
#endif

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+WLWPS:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+WLWPS:", error_no);
		if (error_no == 1 || error_no == 2) {
			at_wlwps_help();
		}
	}
}
#endif

static void at_wlpwrmode_help(void)
{
	at_printf("\r\n");
	at_printf("AT+WLPWRMODE=<lps_ips>,<enable>");
	at_printf("\t<lps_ips>:\tShould be either \"lps\" or \"ips\"\r\n");
	at_printf("\t<enable>:\t0: disable, 1: enable\r\n");
}

/****************************************************************
AT command process:
	AT+WLPWRMODE
	Wifi AT Command:
	[+WLPWRMODE]:OK
****************************************************************/
void at_wlpwrmode(void *arg)
{
	int error_no = 0;
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	int ps_en;

	RTK_LOGI(NOTAG, "[WLPWRMODE]: _AT_WLAN_POWER_MODE_\r\n");

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[WLPWRMODE] Usage: AT+WLPWRMODE=lps/ips/dtim[mode]\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc < 3) {
		RTK_LOGW(NOTAG, "[WLPWRMODE] Usage: AT+WLPWRMODE=lps/ips/dtim[mode]\r\n");
		error_no = 1;
		goto end;
	}

	if (strcmp(argv[1], "lps") == 0) {
		ps_en = atoi(argv[2]);
		wifi_set_lps_enable(ps_en);
		RTK_LOGW(NOTAG, "lps %s\r\n", (ps_en == 0) ? "disable" : "enable");
	} else if (strcmp(argv[1], "ips") == 0) {
		ps_en = atoi(argv[2]);
		wifi_set_ips_internal(ps_en);
		RTK_LOGW(NOTAG, "ips %s\r\n", (ps_en == 0) ? "disable" : "enable");
	} else {
		RTK_LOGW(NOTAG, "[WLPWRMODE] Invalid parameter");
		error_no = 2;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+WLPWRMODE:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+WLPWRMODE:", error_no);
		at_wlpwrmode_help();
	}
}
#endif /* CONFIG_WLAN */

#ifdef CONFIG_LWIP_LAYER
static void at_wlstaticip_help(void)
{
	at_printf("\r\n");
	at_printf("AT+WLSTATICIP=<ip_addr>[,<gateway>,<netmask>]\r\n");
	at_printf("\tThe <gateway> and <netmask> should be absent or present together\r\n");
}

/****************************************************************
AT command process:
	AT+WLSTATICIP
	Wifi AT Command:
	Set static IP address.
	[+WLSTATICIP]:OK
****************************************************************/
void at_wlstaticip(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[+WLSTATICIP]: Invalid parameter\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 2 && argc != 4) {
		RTK_LOGW(NOTAG, "[+WLSTATICIP]: Invalid parameter number\r\n");
		error_no = 1;
		goto end;
	}

	/* Static IP will be set in LwIP_DHCP(). */
	user_static_ip.use_static_ip = 1;
	user_static_ip.addr = PP_HTONL(inet_addr(argv[1]));
	if (argc == 4) {
		user_static_ip.gw = PP_HTONL(inet_addr(argv[2]));
		user_static_ip.netmask = PP_HTONL(inet_addr(argv[3]));
	}

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+WLSTATICIP:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+WLSTATICIP:", error_no);
		at_wlstaticip_help();
	}
}

static void at_ping_help(void)
{
	at_printf("\r\n");
	at_printf("AT+PING=<host>[,<options>]\r\n");
	at_printf("\t-t\tPing the specified host until stopped\r\n");
	at_printf("\t-n\tNumber of echo requests to send (default 4 times)\r\n");
	at_printf("\t-l\tSend buffer size (default 32 bytes)\r\n");
}

/****************************************************************
AT command process:
	AT+PING
	Wifi AT Command:
	[+PING]:OK
****************************************************************/
void at_ping(void *arg)
{
	int argc = 0;
	int error_no = 0;
	char *argv[MAX_ARGC] = {0};

	RTK_LOGI(NOTAG, "[+PING]: _AT_WLAN_PING_TEST_\r\n");

	if (arg == NULL) {
		RTK_LOGI(NOTAG, "[+PING] Usage: AT+PING=[host],[options]\r\n");
		RTK_LOGI(NOTAG, "       stop      Terminate ping\r\n");
		RTK_LOGI(NOTAG, "       -t    #   Ping the specified host until stopped\r\n");
		RTK_LOGI(NOTAG, "       -n    #   Number of echo requests to send (default 4 times)\r\n");
		RTK_LOGI(NOTAG, "       -l    #   Send buffer size (default 32 bytes)\r\n");
		RTK_LOGI(NOTAG, "       if    #   specify ping interface, 0 is for STA and 1 is for soft\r\n");
		RTK_LOGI(NOTAG, "   Example:\r\n");
		RTK_LOGI(NOTAG, "       AT+PING=192.168.1.2,-n,100,-l,5000\r\n");
		error_no = 1;
		goto end;
	}

	argv[0] = (char *)"ping";
	argc = parse_param(arg, argv);
	if (argc > 1) {
		cmd_ping(argc, argv);
	} else {
		RTK_LOGI(NOTAG, "PING error happend\r\n");
		error_no = 2;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+PING:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+PING:", error_no);
		at_ping_help();
	}
}

/* The command iperf3 will be implemented later. */
static void cmd_iperf3(int argc, char **argv)
{
	UNUSED(argc);
	UNUSED(argv);
	RTK_LOGW(NOTAG, " iperf3 is not supported yet\r\n");
}

static void at_iperf_help(void)
{
	at_printf("\r\n");
	at_printf("AT+IPERF=-help\r\n");
	at_printf("AT+IPERF=[-s|-c,host|stop],[options]\r\n");
	at_printf("\tExample for TCP:\r\n");
	at_printf("\tAT+IPERF=-s,-p,5002\r\n");
	at_printf("\tAT+IPERF=-c,192.168.1.2,-t,100,-p,5002\r\n");
	at_printf("\tExample for UDP:\r\n");
	at_printf("\tAT+UDP=-s,-p,5002,-u\r\n");
	at_printf("\tAT+UDP=-c,192.168.1.2,-t,100,-p,5002,-u\r\n");
}

/****************************************************************
AT command process:
	AT+IPERF
	Wifi AT Command:
	[+IPERF]:OK
****************************************************************/
void at_iperf(void *arg)
{
	int error_no = 0;
	int argc;
	char *argv[MAX_ARGC] = {0};
	char *pos;
	char *input = NULL;
	char *char_arg = (char *)arg;

	if (arg == NULL) {
		RTK_LOGI(NOTAG, "[+IPERF] iperf1 Usage: AT+IPERF=[-s|-c,host|stop],[options]\r\n");
		RTK_LOGI(NOTAG, "[+IPERF] Usage: AT+IPERF=[-s|-c,host|stop],[options]\r\n");
		RTK_LOGI(NOTAG, "	Client/Server:\r\n");
		RTK_LOGI(NOTAG, "	  ? 			List all stream status\r\n");
		RTK_LOGI(NOTAG, "	 stop  #		terminate specific stream id or terminate all stream if no id specified\r\n");
		RTK_LOGI(NOTAG, "	 -i    #		seconds between periodic bandwidth reports\r\n");
		RTK_LOGI(NOTAG, "	 -l    #		length of buffer to read or write (default 1460 Bytes)\r\n");
		RTK_LOGI(NOTAG, "	 -p    #		server port to listen on/connect to (default 5001)\r\n");
		RTK_LOGI(NOTAG, "	 -u    #		use UDP protocol (default TCP)\r\n");
		RTK_LOGI(NOTAG, "	Server specific:\r\n");
		RTK_LOGI(NOTAG, "	 -s 			run in server mode\r\n");
		RTK_LOGI(NOTAG, "	Client specific:\r\n");
		RTK_LOGI(NOTAG, "	 -b    #[KM]	for UDP, bandwidth to send at in bits/sec (default 1 Mbit/sec)\r\n");
		RTK_LOGI(NOTAG, "	 -c    <host>	run in client mode, connecting to <host>\r\n");
		RTK_LOGI(NOTAG, "	 -d 			Do a bidirectional test simultaneously\r\n");
		RTK_LOGI(NOTAG, "	 -t    #		time in seconds to transmit for (default 10 secs)\r\n");
		RTK_LOGI(NOTAG, "	 -n    #[KM]	number of bytes to transmit (instead of -t)\r\n");
		RTK_LOGI(NOTAG, "		-S	  # 	   for UDP, set the IP 'type of service'\r\n");
		RTK_LOGI(NOTAG, "	Example for TCP:\r\n");
		RTK_LOGI(NOTAG, "	 AT+IPERF=-s,-p,5002\r\n");
		RTK_LOGI(NOTAG, "	 AT+IPERF=-c,192.168.1.2,-t,100,-p,5002\r\n");
		RTK_LOGI(NOTAG, "	Example for UDP:\r\n");
		RTK_LOGI(NOTAG, "	 AT+IPERF=-s,-p,5002,-u\r\n");
		RTK_LOGI(NOTAG, "	 AT+IPERF=-c,192.168.1.2,-t,100,-p,5002,-u\r\n");

		error_no = 1;
		goto end;
	}

	pos = strpbrk(char_arg, "u");

	if (pos) {
		if ((memcmp(pos - 1, "-", 1)) || (pos == char_arg)) {
			at_printf("- needs to be added before u\r\n");
			error_no = 1;
			goto end;
		}

		input = (char *)rtos_mem_zmalloc(strlen(char_arg) - 2); /* delete "-u,"or ",-u" and need '\0' at the end */
		if (pos - char_arg == 1) {  // "-u" is at the beginning of arg
			memcpy(input, char_arg + 3, strlen(char_arg) - 3);
		} else {                      // "-u" is at the end or middle of arg
			memcpy(input, char_arg, pos - char_arg - 2);                //copy str before "-u"
			memcpy(input + strlen(input), pos + 1, strlen(pos) - 1);  //copy str after "-u"
		}
		argv[0] = (char *)"udp";
	} else {
		input = (char *)rtos_mem_zmalloc(strlen(char_arg) + 1); /* need '\0' at the end */
		memcpy(input, char_arg, strlen(char_arg));
		argv[0] = (char *)"tcp";
	}

	RTK_LOGI(NOTAG, "[+IPERF]: _AT_WLAN_IPERF1_TCP_TEST_\r\n");

	argc = parse_param(input, argv);
	if (argc > 1) {
		cmd_iperf(argc, argv);
	} else {
		RTK_LOGI(NOTAG, "[+IPERF] Should be some argc\r\n");
		error_no = 3;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+IPERF:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+IPERF:", error_no);
		at_iperf_help();
	}
	if (input) {
		rtos_mem_free((void *)input);
	}
}

static void at_iperf3_help(void)
{
	at_printf("\r\n");
	at_printf("AT+IPERF3=-help\r\n");
	at_printf("AT+IPERF3=[-s|-c,host|stop],[options]\r\n");
	at_printf("\tExample:\r\n");
	at_printf("\tAT+IPERF3=-s,-p,5002\r\n");
	at_printf("\tAT+IPERF3=-c,192.168.1.2,-t,100,-p,5002\r\n");
}

/****************************************************************
AT command process:
	AT+IPERF3
	Wifi AT Command:
	[+IPERF3]:OK
****************************************************************/
void at_iperf3(void *arg)
{
	int error_no = 0;
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (arg == NULL) {
		RTK_LOGI(NOTAG, "[+IPERF3] iperf3 Usage: More Usage: AT+IPERF3=-help\r\n");
		RTK_LOGI(NOTAG, "[+IPERF3] Usage: AT+IPERF3=[-s|-c,host|stop],[options]\r\n");
		RTK_LOGI(NOTAG, "	Client/Server:\r\n");
		RTK_LOGI(NOTAG, "	  ? 			List all stream status\r\n");
		RTK_LOGI(NOTAG, "	 stop  #		terminate specific stream id or terminate all stream if no id specified\r\n");
		RTK_LOGI(NOTAG, "	 -i    #		seconds between periodic bandwidth reports\r\n");
		RTK_LOGI(NOTAG, "	 -l    #		length of buffer to read or write (default 1460 Bytes)\r\n");
		RTK_LOGI(NOTAG, "	 -p    #		server port to listen on/connect to (default 5001)\r\n");
		RTK_LOGI(NOTAG, "	Server specific:\r\n");
		RTK_LOGI(NOTAG, "	 -s 			run in server mode\r\n");
		RTK_LOGI(NOTAG, "	Client specific:\r\n");
		RTK_LOGI(NOTAG, "	 -c    <host>	run in client mode, connecting to <host>\r\n");
		RTK_LOGI(NOTAG, "	 -d 			Do a bidirectional test simultaneously\r\n");
		RTK_LOGI(NOTAG, "	 -t    #		time in seconds to transmit for (default 10 secs)\r\n");
		RTK_LOGI(NOTAG, "	 -n    #[KM]	number of bytes to transmit (instead of -t)\r\n");
		RTK_LOGI(NOTAG, "	Example:\r\n");
		RTK_LOGI(NOTAG, "	 AT+IPERF3=-s,-p,5002\r\n");
		RTK_LOGI(NOTAG, "	 AT+IPERF3=-c,192.168.1.2,-t,100,-p,5002\r\n");
		error_no = 1;
		goto end;
	}

	RTK_LOGI(NOTAG, "[+IPERF3]: _AT_WLAN_IPERF1_TCP_TEST_\r\n");
	argv[0] = (char *)"iperf3";
	argc = parse_param(arg, argv);
	if (argc > 1) {
		cmd_iperf3(argc, argv);
	} else {
		RTK_LOGI(NOTAG, "[+IPERF3] Should be some argc\r\n");
		error_no = 2;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+IPERF3:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+IPERF3:", error_no);
		at_iperf3_help();
	}
}

#endif /* CONFIG_LWIP_LAYER */

log_item_t at_wifi_items[ ] = {
#ifdef CONFIG_LWIP_LAYER
	{"+WLSTATICIP", at_wlstaticip, {NULL, NULL}},
	{"+PING", at_ping, {NULL, NULL}},
	{"+IPERF", at_iperf, {NULL, NULL}},
	{"+IPERF3", at_iperf3, {NULL, NULL}},
#endif /* CONFIG_LWIP_LAYER */
#ifdef CONFIG_WLAN
	{"+WLCONN", at_wlconn, {NULL, NULL}},
	{"+WLDISCONN", at_wldisconn, {NULL, NULL}},
	{"+WLSCAN", at_wlscan, {NULL, NULL}},
	{"+WLSCANSSID", at_wlscanssid, {NULL, NULL}},
	{"+WLRSSI", at_wlrssi, {NULL, NULL}},
	{"+WLSTARTAP", at_wlstartap, {NULL, NULL}},
	{"+WLSTOPAP", at_wlstopap, {NULL, NULL}},
	{"+WLSTATE", at_wlstate, {NULL, NULL}},
	{"+WLAUTOCONN", at_wlautoconn, {NULL, NULL}},
#if ENABLE_SET_MAC_ADDRESS
	{"+WLMAC", at_wlmac, {NULL, NULL}},
#endif
	{"+WLPROMISC", at_wlpromisc, {NULL, NULL}},
	{"+WLDBG", at_wldbg, {NULL, NULL}},
#ifdef CONFIG_WPS
	{"+WLWPS", at_wlwps, {NULL, NULL}},
#endif
	{"+WLPWRMODE", at_wlpwrmode, {NULL, NULL}},
#endif /* CONFIG_WLAN */
};

void print_wifi_at(void)
{
	int index;
	int cmd_len = 0;

	cmd_len = sizeof(at_wifi_items) / sizeof(at_wifi_items[0]);
	for (index = 0; index < cmd_len; index++) {
		at_printf("AT%s\r\n", at_wifi_items[index].log_cmd);
	}
}

void print_wlan_help(void)
{
	at_printf("AT+WLCONN=<ss_id>[,passwd,key_id,bss_id]\r\n");
	at_printf("AT+WLDISCONN\r\n");
	at_printf("AT+WLSTARTAP=<ss_id>,<channel>,<open/wep/tkip/wpa2/wpa3>,<password>\r\n");
	at_printf("AT+WLSTOPAP\r\n");
	at_printf("AT+WLSCAN\r\n");
	at_printf("AT+WLSCANSSID=<ssid>[,chnl_0,chnl_1,...]\r\n");
	at_printf("AT+WLSTATE\r\n");
	at_printf("AT+WLRSSI\r\n");
	at_printf("AT+WLPROMISC=<enable/disable>[,<all/apall>]\r\n");
	at_printf("AT+WLWPS=<pbc/pin>\r\n");
	at_printf("AT+WLSTATICIP=<ip>[,<gateway>,<mask>]\r\n");
	at_printf("AT+PING=[host],[options]\r\n");
	at_printf("AT+IPERF=[-s|-c,host|stop],[options]\r\n");
	at_printf("AT+IPERF3=[-s|-c,host|stop],[options]\r\n");
	at_printf("AT+UDP=[-s|-c,host|stop],[options]\r\n");
	at_printf("AT+WLDBG=COMMAND[PARAMETERS]\r\n");
	at_printf("AT+WLPWRMODE=lps/ips/dtim[mode]\r\n");
	at_printf("\r\n");
}

void at_wifi_init(void)
{
#ifdef CONFIG_WLAN
	init_wifi_struct();
#endif
#ifndef CONFIG_MP_INCLUDED
	log_service_add_table(at_wifi_items, sizeof(at_wifi_items) / sizeof(at_wifi_items[0]));
#endif
}

#ifdef SUPPORT_LOG_SERVICE
log_module_init(at_wifi_init);
#endif
#endif /* CONFIG_MP_INCLUDED */
