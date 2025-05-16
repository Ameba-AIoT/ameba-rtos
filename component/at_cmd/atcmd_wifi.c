/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"

#ifndef CONFIG_MP_SHRINK
#include "atcmd_service.h"
#include "atcmd_wifi.h"
#ifdef CONFIG_LWIP_LAYER
#include "dhcp/dhcps.h"
#endif
#ifdef CONFIG_WLAN
#include "wifi_intf_drv_to_upper.h"
#endif
#ifdef CONFIG_AS_INIC_AP
#ifdef CONFIG_WHC_INTF_IPC
#include "whc_ipc_host_api.h"
#else
#include "whc_host_api.h"
#endif
#endif

#ifdef CONFIG_LWIP_LAYER
struct static_ip_config user_static_ip;
extern struct netif xnetif[NET_IF_NUM];

unsigned char ap_ip[4] = {192, 168, 43, 1}, ap_netmask[4] = {255, 255, 255, 0}, ap_gw[4] = {192, 168, 43, 1};
#endif

#ifdef CONFIG_WLAN
extern struct table  ip_table;
#if defined(CONFIG_ENABLE_WPS) && CONFIG_ENABLE_WPS
extern void cmd_wps(int argc, char **argv);
#endif
static struct rtw_network_info wifi = {0};
static struct rtw_softap_info ap = {0};
static unsigned char password[129] = {0};
static int security = -1;

#if defined(CONFIG_IP_NAT) && (CONFIG_IP_NAT == 1)
extern void ipnat_dump(void);
#endif

extern int wifi_set_ips_internal(u8 enable);

#if (defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET) || (defined(CONFIG_ETHERNET) && CONFIG_ETHERNET)
extern struct netif eth_netif;
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
	wifi.security_type = RTW_SECURITY_OPEN;

	memset(ap.ssid.val, 0, sizeof(ap.ssid.val));
	ap.ssid.len = 0;
	ap.password = NULL;
	ap.password_len = 0;
	ap.channel = 0;
	ap.hidden_ssid = 0;
	security = -1;
}

static void print_wifi_setting(unsigned char wlan_idx, struct rtw_wifi_setting *pSetting)
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
#endif
	} else if (pSetting->security_type == (WPA2_SECURITY | WPA3_SECURITY | ENTERPRISE_ENABLED)) {
		at_printf("  SECURITY => WPA2/WPA3 ENTERPRISE\r\n");
	} else if (pSetting->security_type == (WPA3_SECURITY | ENTERPRISE_ENABLED)) {
		at_printf("  SECURITY => WPA3 ENTERPRISE\r\n");
#ifdef CONFIG_OWE_SUPPORT
	} else if (pSetting->security_type == RTW_SECURITY_WPA3_OWE) {
		at_printf("  SECURITY => WPA3-OWE\r\n");
#endif
	} else {
		at_printf("  SECURITY => UNKNOWN\r\n");
	}

	at_printf("  PASSWORD => %s\r\n", pSetting->password);
	at_printf("\r\n");
#endif
}

static void print_scan_result(struct rtw_scan_result *record)
{
#if (defined(WIFI_LOGO_CERTIFICATION_CONFIG) && WIFI_LOGO_CERTIFICATION_CONFIG)
	at_printf(""MAC_FMT",", MAC_ARG(record->bssid.octet));
	at_printf("%s,\r\n", record->ssid.val);
#else

	at_printf(""MAC_FMT", ", MAC_ARG(record->bssid.octet));
	at_printf("%d, ", record->signal_strength);
	at_printf("%d, ", record->channel);
	at_printf("%s, ", (record->wireless_mode & RTW_80211_AX) ? "AX" :
			  (record->wireless_mode & RTW_80211_AC) ? "AC" :
			  (record->wireless_mode & RTW_80211_N) ? "N" :
			  (record->wireless_mode & RTW_80211_A) ? "A" :
			  (record->wireless_mode & RTW_80211_G) ? "G" :
			  (record->wireless_mode & RTW_80211_B) ? "B" :
			  "Unknown");
	at_printf("\"%s\", ", (record->security == RTW_SECURITY_OPEN) ? "Open" :
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
#endif
			  (record->security == (WPA2_SECURITY | WPA3_SECURITY | ENTERPRISE_ENABLED)) ? "WPA2/WPA3 Enterprise" :
			  (record->security == (WPA3_SECURITY | ENTERPRISE_ENABLED)) ? "WPA3 Enterprise" :
#ifdef CONFIG_OWE_SUPPORT
			  (record->security == RTW_SECURITY_WPA3_OWE) ? "WPA3-OWE" :
#endif
			  "Unknown");

	at_printf("\"%s\" ", record->ssid.val);
	if (record->bss_type == RTW_BSS_TYPE_WTN_HELPER) {
		at_printf(" Helper ");
	}
	at_printf("\r\n");
#endif
}

static void at_wlconn_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+WLCONN=[<type>,<value>,<type>,<value>......]\r\n");
	RTK_LOGI(NOTAG, "\t<type>:\tA string as \"ssid\",\"bssid\",\"pw\",\"key_id\",\"ch\"\r\n");
	RTK_LOGI(NOTAG, "\t<value>:\tAny type of <ssid>, <bssid>, <pw>, <key_id>, <channel>\r\n");
	RTK_LOGI(NOTAG, "\t<ssid>:\tA string SSID name\r\n");
	RTK_LOGI(NOTAG, "\t<bssid>:\tA hex-number string with colons, e.g. 1a:2b:3c:4d:5e:6f\r\n");
	RTK_LOGI(NOTAG, "\t<pw>:\tWPA or WPA2 with length 8~64, WEP with length 5 or 13\r\n");
	RTK_LOGI(NOTAG, "\t<key_id>:\tFor WEP security, must be 0~3, if absent, it is 0\r\n");
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
	int argc = 0, ret = 0, i = 0, j = 0, k = 0;
	int error_no = RTW_AT_OK;
	unsigned int mac[ETH_ALEN];
	char *argv[MAX_ARGC] = {0};
	char empty_bssid[6] = {0};

#ifdef CONFIG_LWIP_LAYER
	unsigned long tick1 = rtos_time_get_current_system_time_ms();
	unsigned long tick3;
#endif

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[+WLCONN] The parameters can not be ignored\r\n");
		error_no = RTW_AT_ERR_REQUIRED_PARAM_MISS;
		goto end;
	}

	argc = parse_param(arg, argv);
	if ((argc < 2) || (argc > 11)) {
		RTK_LOGW(NOTAG, "[+WLCONN] The parameters format ERROR\r\n");
		error_no = RTW_AT_ERR_PARAM_NUM_ERR;
		goto end;
	}

	/* The parameters appear by pairs, so i += 2. */
	for (i = 1; argc > i; i += 2) {
		j = i + 1;  /* next i. */
		/* SSID. */
		if (0 == strcmp("ssid", argv[i])) {
			if ((argc <= j) || (strlen(argv[j]) == 0) || (strlen(argv[j]) > RTW_ESSID_MAX_SIZE)) {
				RTK_LOGW(NOTAG, "[+WLCONN] Invalid SSID\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
			wifi.ssid.len = strlen(argv[j]);
			strncpy((char *)wifi.ssid.val, argv[j], sizeof(wifi.ssid.val) - 1);
		}
		/* BSSID. */
		else if (0 == strcmp("bssid", argv[i])) {
			if ((argc <= j) || (strlen(argv[j]) != 17)) {
				RTK_LOGW(NOTAG, "[+WLCONN] Invalid BSSID\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
			_sscanf_ss(argv[j], MAC_FMT, &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
			for (k = 0; k < ETH_ALEN; k++) {
				wifi.bssid.octet[k] = mac[k] & 0xFF;
			}
		}
		/* password. */
		else if (0 == strcmp("pw", argv[i])) {
			if ((argc <= j) || (strlen(argv[j]) == 0)) {
				RTK_LOGW(NOTAG, "[+WLCONN] Invalid password\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
			strncpy((char *)password, argv[j], sizeof(password) - 1);
			wifi.password = password;
			wifi.password_len = strlen(argv[j]);
		}
		/* Key ID. */
		else if (0 == strcmp("key_id", argv[i])) {
			if ((argc <= j) || (strlen(argv[j]) != 1)
				|| (*(char *)argv[j] < '0') || (*(char *)argv[j] > '3')) {
				RTK_LOGW(NOTAG, "[+WLCONN] Invalid KEYID\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
			wifi.key_id = atoi(argv[j]);
		}
		/* Channel. */
		else if (0 == strcmp("ch", argv[i])) {
			if ((argc > j) && (strlen(argv[j]) != 0)) {
				wifi.channel = (unsigned char)atoi(argv[j]);
			}
		}
		/* Invalid input. */
		else {
			RTK_LOGW(NOTAG, "[+WLCONN] Invalid parameter type\r\n");
			error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
			goto end;
		}
	}

	/* Check bssid is empty && ssid is null */
	if (!memcmp(wifi.bssid.octet, empty_bssid, sizeof(empty_bssid)) && (wifi.ssid.val[0] == 0)) {
		RTK_LOGW(NOTAG, "[+WLCONN] SSID should exist here\r\n");
		error_no = RTW_AT_ERR_REQUIRED_PARAM_MISS;
		goto end;
	}

	/* Check WEP. */
	if ((wifi.key_id >= 0) && (wifi.key_id <= 3) && (wifi.password != NULL)) {
		wifi.security_type = RTW_SECURITY_WEP_SHARED;
	}

	/* Connecting ...... */
	ret = wifi_connect(&wifi, 1);
	if (ret != RTK_SUCCESS) {
		RTK_LOGW(NOTAG, "[+WLCONN] Fail:-0x%x", -ret);
		if ((ret == -RTK_ERR_WIFI_CONN_INVALID_KEY)) {
			RTK_LOGW(NOTAG, "(password format wrong)");
		} else if (ret == -RTK_ERR_WIFI_CONN_SCAN_FAIL) {
			RTK_LOGW(NOTAG, "(not found AP)");
		} else if (ret == -RTK_ERR_BUSY) {
			RTK_LOGW(NOTAG, "(busy)");
		}
		RTK_LOGW(NOTAG, "\r\n");
		error_no = RTW_AT_ERR_CONNECT_FAILED;
		goto end;
	}

#ifdef CONFIG_LWIP_LAYER
	/* Start DHCPClient */
	LwIP_DHCP(0, DHCP_START);
	tick3 = rtos_time_get_current_system_time_ms();
	RTK_LOGI(NOTAG, "\r\n[+WLCONN] Got IP after %d ms.\r\n", (unsigned int)(tick3 - tick1));
#endif

end:
	init_wifi_struct();
	if (error_no == RTW_AT_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		if (error_no >= RTW_AT_ERR_REQUIRED_PARAM_MISS && error_no <= RTW_AT_ERR_PARAM_NUM_ERR) {
			at_wlconn_help();
		}
		at_printf(ATCMD_ERROR_END_STR, error_no);
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
	u8 join_status = RTW_JOINSTATUS_UNKNOWN;
	int timeout = 20, ret = 0;
	int error_no = RTW_AT_OK;
	struct rtw_wifi_setting wifi_setting = {0};

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

	if (wifi_get_join_status(&join_status) != RTK_SUCCESS) {
		error_no = RTW_AT_ERR_UNKNOWN_ERR;
		RTK_LOGW(NOTAG, "[+WLDISCONN] Wifi get join status ERROR\r\n");
		goto end;
	}

	if (join_status == RTW_JOINSTATUS_UNKNOWN) {
		RTK_LOGI(NOTAG, "[+WLDISCONN] Not connected yet\r\n");
		goto end;
	}

	/* Disconnecting ...... */
	ret = wifi_disconnect();
	if (ret < 0) {
		RTK_LOGW(NOTAG, "[+WLDISCONN] Disconnect ERROR\r\n");
		error_no = RTW_AT_ERR_UNKNOWN_ERR;
		goto end;
	}

	/* error_no == RTW_AT_ERR_STATUS_GET_FAILED means that the expected results have not been obtained within the specified time */
	error_no = RTW_AT_ERR_RESULT_GET_FAILED;
	while (timeout > 0) {
		if ((wifi_get_join_status(&join_status) == RTK_SUCCESS) && join_status != RTW_JOINSTATUS_SUCCESS) {
			RTK_LOGI(NOTAG, "[+WLDISCONN] disconnect done\r\n");
			error_no = RTW_AT_OK;
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
	if (error_no == RTW_AT_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

void at_wlscan_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+WLSCAN\r\n");
	RTK_LOGI(NOTAG, "AT+WLSCAN=[<type>,<ssid>,<type>,<chl1>:<chl2>:<chl3>:......]\r\n");
	RTK_LOGI(NOTAG, "\t<type>:\tIt may be \"ssid\" or \"ch\"\r\n");
	RTK_LOGI(NOTAG, "\t\tIf the <type> is \"ssid\", it should be the 1st parameter\r\n");
	RTK_LOGI(NOTAG, "\t\tIf the <type> is \"ch\", it is followed by channel list\r\n");
	RTK_LOGI(NOTAG, "\t\tThe colon \':\' is the segmentation of chennel list\r\n");
}

static int count_get_channel_list(char *arg, u8 *channel_list)
{
	int i = 0, j = 0;
	u8 val = 0;

	for (i = 0; arg[i] != '\0'; i++) {
		if (':' == arg[i]) {
			/* Record the jth val. */
			if (channel_list != NULL) {
				channel_list[j] = val;
				val = 0;
			}
			/* Parse the colon as segmentation. */
			j++;
		}
		/* Should be digital 0~9, otherwise, return an invalid -1. */
		else if ('0' > arg[i] || '9' < arg[i]) {
			j = -1;
			goto end;
		} else if (channel_list != NULL) {
			val = val * 10 + (arg[i] - '0');
		}
	}
	/* The final val. */
	if (channel_list != NULL) {
		channel_list[j] = val;
	}
	j++;

end:
	return j;
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
	int num_channel = 0, ret = 0;
	unsigned int i = 0, j = 0, argc = 0;
	u32 scanned_AP_num = 0;
	int error_no = RTW_AT_OK;
	char *argv[MAX_ARGC] = {0};
	struct rtw_scan_result *scanned_AP_list = NULL;
	struct rtw_scan_param scan_param;
	struct rtw_scan_result *scanned_AP_info;
	u8 join_status = RTW_JOINSTATUS_UNKNOWN;

	memset(&scan_param, 0, sizeof(struct rtw_scan_param));

	if (wifi_get_join_status(&join_status) != RTK_SUCCESS) {
		error_no = RTW_AT_ERR_UNKNOWN_ERR;
		RTK_LOGW(NOTAG, "[+WLSCAN] wifi get join status ERROR\r\n");
		goto end;
	}
	if ((join_status > RTW_JOINSTATUS_UNKNOWN) && (join_status < RTW_JOINSTATUS_SUCCESS)) {
		RTK_LOGW(NOTAG, "[+WLSCAN] Connecting now, forbid scanning\r\n");
		error_no = RTW_AT_ERR_INVALID_WIFI_STATUS;
		goto end;
	}

	argc = parse_param(arg, argv);

	for (i = 1; argc > i; i += 2) {
		j = i + 1;  /* Next i. */
		/* SSID. */
		if (0 == strcmp("ssid", argv[i])) {
			if ((argc <= j) || (0 == strlen(argv[j]))
				|| (RTW_ESSID_MAX_SIZE < strlen(argv[j]))) {
				RTK_LOGW(NOTAG, "[+WLSCAN] Invalid ssid\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
			wifi.ssid.len = strlen(argv[j]);
			strncpy((char *)wifi.ssid.val, argv[j], sizeof(wifi.ssid.val) - 1);
			scan_param.ssid = wifi.ssid.val;
		}
		/* Channel list. */
		else if (0 == strcmp("ch", argv[i])) {
			if ((argc <= j) || (0 == strlen(argv[j]))) {
				RTK_LOGW(NOTAG, "[+WLSCAN] Invalid channel list\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
			num_channel = count_get_channel_list(argv[j], NULL);
			if (0 >= num_channel) {
				RTK_LOGW(NOTAG, "[+WLSCAN] Invalid channel list\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
			channel_list = (u8 *)rtos_mem_zmalloc(num_channel);
			if (channel_list == NULL) {
				RTK_LOGW(NOTAG, "[+WLSCAN]ERROR: Can not malloc memory for channel list\r\n");
				error_no = RTW_AT_ERR_MEM_ALLOC_FAIL;
				goto end;
			}
			scan_param.channel_list_num = count_get_channel_list(argv[j], channel_list);
			scan_param.channel_list = channel_list;
		}
		/* Invalid input. */
		else {
			RTK_LOGW(NOTAG, "[+WLSCAN] Invalid input\r\n");
			error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
			goto end;
		}
	}

	ret = wifi_scan_networks(&scan_param, 1);
	if (ret < RTK_SUCCESS) {
		RTK_LOGW(NOTAG, "[+WLSCAN] wifi_scan_networks ERROR\r\n");
		error_no = RTW_AT_ERR_INVALID_SCAN_PARAM;
		goto end;
	}

	/* get scan results and log them */
	scanned_AP_num = ret;

	if (scanned_AP_num != 0) {
		scanned_AP_list = (struct rtw_scan_result *)rtos_mem_zmalloc(scanned_AP_num * sizeof(struct rtw_scan_result));
		if (scanned_AP_list == NULL) {
			RTK_LOGW(NOTAG, "[+WLSCAN]ERROR: Can not malloc memory for scan result\r\n");
			error_no = RTW_AT_ERR_MEM_ALLOC_FAIL;
			goto end;
		}

		if (wifi_get_scan_records(&scanned_AP_num, scanned_AP_list) < 0) {
			RTK_LOGW(NOTAG, "[+WLSCAN] Get result failed\r\n");
			rtos_mem_free((void *)scanned_AP_list);
			error_no = RTW_AT_ERR_UNKNOWN_ERR;
			goto end;
		}

		for (i = 0; i < scanned_AP_num; i++) {
#if (defined(WIFI_LOGO_CERTIFICATION_CONFIG) && WIFI_LOGO_CERTIFICATION_CONFIG)
			at_printf("[%d],", (i + 1));
#else
			at_printf("%2d, ", (i + 1));
#endif
			scanned_AP_info = &scanned_AP_list[i];
			scanned_AP_info->ssid.val[scanned_AP_info->ssid.len] = 0; /* Ensure the SSID is null terminated */

			print_scan_result(scanned_AP_info);
		}

		rtos_mem_free((void *)scanned_AP_list);
	}

end:
	RTK_LOGI(NOTAG, "\r\n[+WLSCAN] scan_ap_num: %d\r\n", scanned_AP_num);
	rtos_mem_free((void *)channel_list);
	if (error_no == RTW_AT_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		if (error_no == RTW_AT_ERR_INVALID_PARAM_VALUE) {
			at_wlscan_help();
		}
		at_printf(ATCMD_ERROR_END_STR, error_no);
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
	union rtw_phy_stats phy_stats;

	UNUSED(arg);

	RTK_LOGI(NOTAG, "[WLRSSI] _AT_WLAN_GET_RSSI_\r\n");
	wifi_get_phy_stats(STA_WLAN_INDEX, NULL, &phy_stats);

	/* cal complement for logs */
	at_printf("rssi = -%d\r\n", (signed char)(0xFF - phy_stats.sta.rssi + 1));
	at_printf("data rssi = -%d\r\n", (signed char)(0xFF - phy_stats.sta.data_rssi + 1));
	at_printf("beacon rssi = -%d\r\n", (signed char)(0xFF - phy_stats.sta.beacon_rssi + 1));
	at_printf(ATCMD_OK_END_STR);
}

void at_wlstartap_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+WLSTARTAP=[<type>,<value>,<type>,<value>......]\r\n");
	RTK_LOGI(NOTAG, "\t<type>:\tA string as \"ssid\",\"ch\",\"pw\",\"sec\"\r\n");
	RTK_LOGI(NOTAG, "\t<value>:\tAny type of <ssid>, <ch>, <pw>, <sec>\r\n");
	RTK_LOGI(NOTAG, "\t<ssid>:\tThe ssid of AP, could not be empty\r\n");
	RTK_LOGI(NOTAG, "\t<ch>:\t[1,11]\r\n");
	RTK_LOGI(NOTAG, "\t<sec>:\topen/wep/tkip/wpa2/wpa3\r\n");
	RTK_LOGI(NOTAG, "\t<pw>:\tWith length in [8,64]\r\n");
#ifdef CONFIG_LWIP_LAYER
	RTK_LOGI(NOTAG, "\t<ip>:\tThe ip of AP, default 192.168.43.1\r\n");
	RTK_LOGI(NOTAG, "\t<gw>:\tThe gateway of AP, default 192.168.43.1\r\n");
	RTK_LOGI(NOTAG, "\t<msk>:\tThe netmask of AP, default 255.255.255.0\r\n");
	RTK_LOGI(NOTAG, "\t<pl>:\tThe ip pool of AP\r\n");
#endif
	RTK_LOGI(NOTAG, "\te.g.\r\nAT+WLSTARTAP=ssid,test_ssid,pw,12345678,sec,wpa2\r\n");
}

void get_ip_addr(unsigned char *ip, const char *str)
{
#ifdef CONFIG_LWIP_LAYER
	ip[0] = (unsigned char) inet_addr(str) & 0xff;
	ip[1] = (unsigned char)(inet_addr(str) >> 8) & 0xff;
	ip[2] = (unsigned char)(inet_addr(str) >> 16) & 0xff;
	ip[3] = (unsigned char)(inet_addr(str) >> 24) & 0xff;
#else
	(void) ip;
	(void) str;
#endif
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
	int argc = 0, ret = 0, i = 0, j = 0;
	int error_no = RTW_AT_OK;
	char *argv[MAX_ARGC] = {0};
#ifdef CONFIG_LWIP_LAYER
	u32 ip_addr, netmask, gw;
	struct ip_addr start_ip, end_ip;
	struct netif *pnetif = &xnetif[SOFTAP_WLAN_INDEX];
#endif
	int timeout = 20;
	struct rtw_wifi_setting *setting = NULL;
	struct rtw_acs_config acs_config;
	acs_config.band = RTW_SUPPORT_BAND_2_4G_5G_BOTH;

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[+WLSTARTAP] The parameters can not be ignored\r\n");
		error_no = RTW_AT_ERR_REQUIRED_PARAM_MISS;
		goto end;
	}

	argc = parse_param(arg, argv);
	if ((argc < 2) || (argc > 18)) {
		RTK_LOGW(NOTAG, "[+WLSTARTAP] command format error\r\n");
		error_no = RTW_AT_ERR_PARAM_NUM_ERR;
		goto end;
	}

	/* The parameters appear by pairs, so i += 2. */
	for (i = 1; argc > i; i += 2) {
		j = i + 1;  /* next i. */
		/* SSID */
		if (0 == strcmp("ssid", argv[i])) {
			if ((argc <= j) || (strlen(argv[j]) == 0) || (strlen(argv[j]) > RTW_ESSID_MAX_SIZE)) {
				RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid SSID length\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
			ap.ssid.len = strlen(argv[j]);
			strncpy((char *)ap.ssid.val, argv[j], sizeof(ap.ssid.val) - 1);
		}
		/* password */
		else if (0 == strcmp("pw", argv[i])) {
			if ((argc <= j) || (0 == strlen(argv[j])) || (64 < strlen(argv[j]))) {
				RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid password\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
			ap.password_len = strlen(argv[j]);
			strncpy((char *)password, argv[j], sizeof(password) - 1);
			ap.password = password;
		}
		/* Security */
		else if (0 == strcmp("sec", argv[i])) {
			if (argc > j) {
				if (0 == strcmp("open", argv[j])) {
					security = 0;
				} else if (0 == strcmp("wep", argv[j])) {
					security = 1;
				} else if (0 == strcmp("tkip", argv[j])) {
					security = 2;
				} else if (0 == strcmp("wpa2", argv[j])) {
					security = 3;
				} else if (0 == strcmp("wpa3", argv[j])) {
					security = 4;
				} else {
					RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid security value\r\n");
					error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
					goto end;
				}
			}
		}
		/* channel */
		else if (0 == strcmp("ch", argv[i])) {
			if ((argc > j) && (0 != strlen(argv[j]))) {
				ap.channel = atoi(argv[j]);
			}
		}
		/* band */
		else if (0 == strcmp("band", argv[i])) {
			if (argc > j) {
				if (0 == strcmp("2g", argv[j])) {
					acs_config.band = 0;
				} else if (0 == strcmp("5g", argv[j])) {
					acs_config.band = 1;
				} else if (0 == strcmp("2g_5g", argv[j])) {
					acs_config.band = 2;
				} else {
					RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid band value\r\n");
					error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
					goto end;
				}
			}
		}
#ifdef CONFIG_LWIP_LAYER
		/* ip */
		else if (0 == strcmp("ip", argv[i])) {
			if (argv[j] != NULL && inet_addr(argv[j]) != IPADDR_NONE) {
				get_ip_addr(ap_ip, argv[j]);
				get_ip_addr(ap_gw, argv[j]);
			} else {
				RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid ip value\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
		} else if (0 == strcmp("pl", argv[i])) {
			if (argv[j] != NULL && inet_addr(argv[j]) != IPADDR_NONE) {
				ip_addr_set_ip4_u32(&start_ip, inet_addr(argv[j]));
			} else {
				RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid start ip value\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}

			if (argv[j + 1] != NULL && inet_addr(argv[j + 1]) != IPADDR_NONE) {
				ip_addr_set_ip4_u32(&end_ip, inet_addr(argv[j + 1]));
			} else {
				RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid end ip value\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}

			dhcps_set_addr_pool(1, &start_ip, &end_ip);
			i += 1;
		} else if (0 == strcmp("gw", argv[i])) {
			if (argv[j] != NULL && inet_addr(argv[j]) != IPADDR_NONE) {
				get_ip_addr(ap_gw, argv[j]);
			} else {
				RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid gateway value\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
		} else if (0 == strcmp("msk", argv[i])) {
			if (argv[j] != NULL && inet_addr(argv[j]) != IPADDR_NONE) {
				get_ip_addr(ap_netmask, argv[j]);
			} else {
				RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid netmask value\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
		}
#endif
		/* Invalid input. */
		else {
			RTK_LOGW(NOTAG, "[+WLSTARTAP] Invalid parameter type\r\n");
			error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
			goto end;
		}
	}

	/* Check SSID */
	if (ap.ssid.val[0] == 0) {
		RTK_LOGW(NOTAG, "[+WLSTARTAP] Empty SSID here\r\n");
		error_no = RTW_AT_ERR_REQUIRED_PARAM_MISS;
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
			error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
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
		error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
		goto end;
	}

#ifdef CONFIG_LWIP_LAYER
	dhcps_deinit();
	ip_addr = CONCAT_TO_UINT32(GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	netmask = CONCAT_TO_UINT32(NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
	gw = CONCAT_TO_UINT32(GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	LwIP_SetIP(SOFTAP_WLAN_INDEX, ip_addr, netmask, gw);
#endif

	if (ap.channel == 0) {
		ret = wifi_acs_find_ideal_channel(&acs_config, &ap.channel);
		if (ret != RTK_SUCCESS) {
			RTK_LOGW(NOTAG, "[+WLSTARTAP] Auto channel select fail, use default channel 1\r\n");
			ap.channel = 1;
		} else {
			RTK_LOGW(NOTAG, "[+WLSTARTAP] Auto channel select %d\r\n", ap.channel);
		}
	}

	/* Starting ...... */
	wifi_stop_ap();
	ret = wifi_start_ap(&ap);
	if (ret < 0) {
		RTK_LOGW(NOTAG, "[+WLSTARTAP] wifi_start_ap failed\r\n");
		error_no = RTW_AT_ERR_SAP_START_FAILED;
		goto end;
	}

	setting = rtos_mem_zmalloc(sizeof(struct rtw_wifi_setting));
	if (setting == NULL) {
		RTK_LOGW(NOTAG, "[+WLSTARTAP] memory failed for setting\r\n");
		error_no = RTW_AT_ERR_MEM_ALLOC_FAIL;
		goto end;
	}

	/* error_no = RTW_AT_ERR_SAP_START_FAILED means time out. */
	error_no = RTW_AT_ERR_SAP_START_FAILED;
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
	ip_addr = CONCAT_TO_UINT32(AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
	netmask = CONCAT_TO_UINT32(AP_NETMASK_ADDR0, AP_NETMASK_ADDR1, AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
	gw = CONCAT_TO_UINT32(AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
	LwIP_SetIP(SOFTAP_WLAN_INDEX, ip_addr, netmask, gw);
	dhcps_init(pnetif);
#endif

end:
	rtos_mem_free((void *)setting);
	init_wifi_struct();
	if (error_no == RTW_AT_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		if (error_no >= RTW_AT_ERR_REQUIRED_PARAM_MISS && error_no <= RTW_AT_ERR_PARAM_NUM_ERR) {
			at_wlstartap_help();
		}
		at_printf(ATCMD_ERROR_END_STR, error_no);
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
	at_printf(ATCMD_OK_END_STR);
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
#ifdef CONFIG_DHCPS_KEPT_CLIENT_INFO
	uint8_t *p = NULL;
#endif
#ifdef CONFIG_LWIP_LAYER
	u8 *mac = LwIP_GetMAC(0);
	u8 *ip = LwIP_GetIP(0);
	u8 *gw = LwIP_GetGW(0);
	u8 *msk = LwIP_GetMASK(0);
#endif
	struct rtw_wifi_setting *p_wifi_setting = NULL;

	UNUSED(arg);

	p_wifi_setting = (struct rtw_wifi_setting *)rtos_mem_zmalloc(sizeof(struct rtw_wifi_setting));
	if (p_wifi_setting == NULL) {
		RTK_LOGW(NOTAG, "[+WLSTATE]: alloc p_wifi_setting fail \r\n");
		at_printf(ATCMD_ERROR_END_STR, 1);
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

			wifi_get_setting(i, p_wifi_setting);
			print_wifi_setting(i, p_wifi_setting);

#ifdef CONFIG_LWIP_LAYER
			at_printf("Interface (%d)\r\n", i);
			at_printf("==============================\r\n");
			at_printf("MAC => %02x:%02x:%02x:%02x:%02x:%02x\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]) ;
			at_printf("IP  => %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
			at_printf("GW  => %d.%d.%d.%d\r\n", gw[0], gw[1], gw[2], gw[3]);
			at_printf("MSK  => %d.%d.%d.%d\r\n\r\n", msk[0], msk[1], msk[2], msk[3]);
#endif
			if (p_wifi_setting->mode == RTW_MODE_AP || i == 1) {
				unsigned int client_number;
				struct rtw_client_list client_info = {0};
				union rtw_phy_stats phy_stats = {0};
				wifi_ap_get_connected_clients(&client_info);

				at_printf("Associated Client List:\r\n");
				at_printf("==============================\r\n");

				if (client_info.count == 0) {
					at_printf("Client Num: %d\r\n\r\n", client_info.count);
				} else {
					at_printf("Client Num: %d\r\n", client_info.count);
					for (client_number = 0; client_number < client_info.count; client_number++) {
						at_printf("Client %d:\r\n", client_number + 1);
#ifdef CONFIG_DHCPS_KEPT_CLIENT_INFO
						for (int n = 0; n < wifi_user_config.ap_sta_num; n++) {
							p = ip_table.client_mac[n];
							if (memcmp(p, client_info.mac_list[client_number].octet, 6) == 0) {
								at_printf("IPv4 address: %d.%d.%d.%d, ", gw[0], gw[1], gw[2], ip_table.ip_addr4[n]);
								at_printf("MAC address: %02x:%02x:%02x:%02x:%02x:%02x, ", p[0], p[1], p[2], p[3], p[4], p[5]);
								wifi_get_phy_stats(SOFTAP_WLAN_INDEX, p, &phy_stats);
								at_printf("RSSI: %d", phy_stats.ap.data_rssi);
								at_printf("\r\n");
								break;
							}
						}
#else
						at_printf("MAC => "MAC_FMT"\r\n",
								  MAC_ARG(client_info.mac_list[client_number].octet));
#endif
					}
				}
			}
		}
	}

	/* show the ethernet interface info */
#if (defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET) || (defined(CONFIG_ETHERNET) && CONFIG_ETHERNET)
#ifdef CONFIG_LWIP_LAYER
	mac = (uint8_t *)(eth_netif.hwaddr);
	ip = (uint8_t *) & (eth_netif.ip_addr);
	gw = (uint8_t *) & (eth_netif.gw);
	at_printf("Interface ethernet\r\n");
	at_printf("==============================\r\n");
	at_printf("MAC => %02x:%02x:%02x:%02x:%02x:%02x\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]) ;
	at_printf("IP  => %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
	at_printf("GW  => %d.%d.%d.%d\r\n\r\n", gw[0], gw[1], gw[2], gw[3]);
#endif /* CONFIG_LWIP_LAYER */
#endif /* CONFIG_LWIP_USB_ETHERNET || CONFIG_ETHERNET */

	rtos_mem_free((void *)p_wifi_setting);

#if defined(CONFIG_IP_NAT) && (CONFIG_IP_NAT == 1)
	ipnat_dump();
#endif

	at_printf(ATCMD_OK_END_STR);
}

static void at_wlreconn_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+WLRECONN=<command>,<parameter>\r\n");
	RTK_LOGI(NOTAG, "<command>:\tauto: auto reconnect when wifi disconnect or connect fail\r\n");
	RTK_LOGI(NOTAG, "\t<parameter>:\t0: disable auto-reconnect, 1: enable auto-reconnect\r\n");
	RTK_LOGI(NOTAG, "<command>:\tfast: fast reconnect when wifi power on\r\n");
	RTK_LOGI(NOTAG, "\t<parameter>:\t0: clear stored flash data and disable fast reconnect, 1: enable fast reconnect\r\n");

}

/****************************************************************
AT command process:
	AT+WLRECONN
	Wifi AT Command:
	Set auto-connection.
	[+WLRECONN]:OK
****************************************************************/
void at_wlreconn(void *arg)
{
	int error_no = RTW_AT_OK;
	int argc = 0, mode = 0;
	char *argv[MAX_ARGC] = {0};

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[+WLRECONN] The parameters can not be ignored\r\n");
		error_no = RTW_AT_ERR_REQUIRED_PARAM_MISS;
		goto end;
	}

	argc = parse_param(arg, argv);

	if (argc != 3 || argv[1] == NULL || argv[2] == NULL) {
		RTK_LOGW(NOTAG, "[+WLRECONN] Invalid parameter number\r\n");
		error_no = RTW_AT_ERR_PARAM_NUM_ERR;
		goto end;
	}

	mode = atoi(argv[2]);
	if (0 == strcmp("auto", argv[1])) {
		if (mode == 0) {
			RTK_LOGI(NOTAG, "[+WLRECONN] Disable autoreconnect\r\n");
			wifi_set_autoreconnect(0);
		} else if (mode == 1) {
			RTK_LOGI(NOTAG, "[+WLRECONN] Enable autoreconnect\r\n");
			wifi_set_autoreconnect(1);
		} else {
			error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
		}
	} else if (0 == strcmp("fast", argv[1])) {
		extern void wifi_fast_connect_enable(unsigned char enable);
		if (mode == 0) {
			extern int32_t rt_kv_delete(const char *key);
			RTK_LOGI(NOTAG, "[+WLRECONN] Erase wifi flash and disable fast reconnect\r\n");
			rt_kv_delete("wlan_data");
			wifi_fast_connect_enable(0);
		} else if (mode == 1) {
			RTK_LOGI(NOTAG, "[+WLRECONN] Enable fast reconnect\r\n");
			wifi_fast_connect_enable(1);
		} else {
			error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
		}
	} else {
		error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
	}

end:
	if (error_no == RTW_AT_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_wlreconn_help();
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

static void at_wlpromisc_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+WLPROMISC=<enable>[,<all_apall>]\r\n");
	RTK_LOGI(NOTAG, "\t<enable>:\t\"enable\" or \"disable\"\r\n");
	RTK_LOGI(NOTAG, "\t<all_apall>:\t\"all\" or \"apall\" only when enabled\r\n");
}

/****************************************************************
AT command process:
	AT+WLPROMISC
	Wifi AT Command:
	[+WLPROMISC]:OK
****************************************************************/
void at_wlpromisc(void *arg)
{
	int argc = 0, error_no = RTW_AT_OK;
	char *argv[MAX_ARGC] = {0};
	struct rtw_promisc_para promisc_para;
	u32 status;

	memset(&promisc_para, 0, sizeof(struct rtw_promisc_para));

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[WLPROMISC]: The parameters can not be ignored\r\n");
		error_no = RTW_AT_ERR_REQUIRED_PARAM_MISS;
		goto end;
	}

	argc = parse_param(arg, argv);

	if (argc > 1) {
		if (strlen(argv[1]) == 0) {
			RTK_LOGW(NOTAG, "[WLPROMISC]: Input ERROR parameter\r\n");
			error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
			goto end;
		}
		if (strcmp((char *)argv[1], "enable") == 0) {
			if (argc != 3 || argv[2] == NULL) {
				RTK_LOGW(NOTAG, "[WLPROMISC]: Should be filter mode here\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
			status = ENABLE;
			if (strcmp((char *)argv[2], "all") == 0) {
				promisc_para.filter_mode = RTW_PROMISC_FILTER_ALL_PKT;
			} else if (strcmp((char *)argv[2], "apall") == 0) {
				promisc_para.filter_mode = RTW_PROMISC_FILTER_AP_ALL;
			} else {
				RTK_LOGW(NOTAG, "[WLPROMISC]: WRONG filter mode\r\n");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
		} else if (strcmp((char *)argv[1], "disable") == 0) {
			status = DISABLE;
		} else {
			RTK_LOGW(NOTAG, "[WLPROMISC]: ERROR parameter\r\n");
			error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
			goto end;
		}
		wifi_promisc_enable(status, &promisc_para);
	}

end:
	if (error_no == RTW_AT_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_wlpromisc_help();
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

static void at_wldbg_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+WLDBG=<command>[,<parameters>]\r\n");
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
	int error_no = RTW_AT_OK;
	int ret = 0;

	RTK_LOGI(NOTAG, "[WLDBG]: _AT_WLAN_IWPRIV_\r\n");
	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[WLDBG]Usage: AT+WLDBG=COMMAND[PARAMETERS]\r\n");
		error_no = RTW_AT_ERR_REQUIRED_PARAM_MISS;
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
	ret = whc_host_api_iwpriv_command(copy, strlen(copy) + 1, 1);
#else
	ret = rtw_iwpriv_command(STA_WLAN_INDEX, copy, 1);
#endif
	if (ret != RTK_SUCCESS) {
		RTK_LOGW(NOTAG, "[WLDBG] Failed while iwpriv\r\n");
		error_no = RTW_AT_ERR_UNKNOWN_ERR;
		goto end;
	}

end:
	if (error_no == RTW_AT_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		if (error_no == RTW_AT_ERR_REQUIRED_PARAM_MISS) {
			at_wldbg_help();
		}
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

#ifdef CONFIG_WPS
static void at_wlwps_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+WLWPS=<pbc_pin>\r\n");
}

/****************************************************************
AT command process:
	AT+WLWPS
	Wifi AT Command:
	[+WLWPS]:OK
****************************************************************/
void at_wlwps(void *arg)
{
	int error_no = RTW_AT_OK;
#if defined(CONFIG_ENABLE_WPS) && CONFIG_ENABLE_WPS
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	char *wps_argv[4];

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[WLWPS]: The parameters can not be ignored\r\n");
		error_no = RTW_AT_ERR_REQUIRED_PARAM_MISS;
		goto end;
	}
	argc = parse_param(arg, argv);
	if (argc < 2 || strlen(argv[1]) == 0) {
		RTK_LOGW(NOTAG, "[WLWPS]: Should be pbc or pin here\r\n");
		error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
		goto end;
	}

	wps_argv[0] = "wifi_wps";
	wps_argv[1] = argv[1];
	wps_argv[2] = argv[2];  /* Maybe NULL, but does not matter. */
	cmd_wps(argc, wps_argv);

#else
	UNUSED(arg);

	RTK_LOGW(NOTAG, "[WLWPS]: Not supported\r\n");
	error_no = RTW_AT_ERR_FUNC_NOT_SUPPORT;
	goto end;
#endif

end:
	if (error_no == RTW_AT_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		if (error_no == RTW_AT_ERR_REQUIRED_PARAM_MISS || error_no == RTW_AT_ERR_INVALID_PARAM_VALUE) {
			at_wlwps_help();
		}
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}
#endif

static void at_wlps_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+WLPS=<mode>,<enable>[,<mode>,<enable>]");
	RTK_LOGI(NOTAG, "\t<mode>:\tShould be either \"lps\" or \"ips\"\r\n");
	RTK_LOGI(NOTAG, "\t<enable>:\t0: disable, 1: enable\r\n");
}

/****************************************************************
AT command process:
	AT+WLPS
	Wifi AT Command:
	[+WLPS]:OK
****************************************************************/
void at_wlps(void *arg)
{
	int error_no = RTW_AT_OK;
	int argc = 0;
	int i = 0, j = 0;
	char *argv[MAX_ARGC] = {0};
	int ps_en;

	RTK_LOGI(NOTAG, "[WLPS]: _AT_WLAN_POWER_SAVE_MODE_\r\n");

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[WLPS] Usage: AT+WLPS=lps/ips[mode]\r\n");
		error_no = RTW_AT_ERR_REQUIRED_PARAM_MISS;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc < 3) {
		RTK_LOGW(NOTAG, "[WLPS] Usage: AT+WLPS=lps/ips/[mode]\r\n");
		error_no = RTW_AT_ERR_PARAM_NUM_ERR;
		goto end;
	}

	for (i = 1; argc > i; i += 2) {
		j = i + 1;  /* Next i. */
		if (strcmp(argv[i], "lps") == 0) {
			if ((argc <= j) || (strlen(argv[j]) == 0)) {
				RTK_LOGW(NOTAG, "[WLPS] Invalid parameter");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
			ps_en = atoi(argv[j]);
			wifi_set_lps_enable(ps_en);
			RTK_LOGW(NOTAG, "lps %s\r\n", (ps_en == 0) ? "disable" : "enable");
		} else if (strcmp(argv[i], "ips") == 0) {
			if ((argc <= j) || (strlen(argv[j]) == 0)) {
				RTK_LOGW(NOTAG, "[WLPS] Invalid parameter");
				error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
				goto end;
			}
			ps_en = atoi(argv[j]);
			wifi_set_ips_internal(ps_en);
			RTK_LOGW(NOTAG, "ips %s\r\n", (ps_en == 0) ? "disable" : "enable");
		} else {
			RTK_LOGW(NOTAG, "[WLPS] Invalid parameter");
			error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
			goto end;
		}
	}

end:
	if (error_no == RTW_AT_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_wlps_help();
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}
#endif /* CONFIG_WLAN */

#ifdef CONFIG_LWIP_LAYER
static void at_wlstaticip_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+WLSTATICIP=<ip_addr>[,<gateway>,<netmask>]\r\n");
	RTK_LOGI(NOTAG, "\tThe <gateway> and <netmask> should be absent or present together\r\n");
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
	int argc = 0, error_no = RTW_AT_OK;
	char *argv[MAX_ARGC] = {0};

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[+WLSTATICIP]: The parameters can not be ignored\r\n");
		error_no = RTW_AT_ERR_REQUIRED_PARAM_MISS;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 2 && argc != 4) {
		RTK_LOGW(NOTAG, "[+WLSTATICIP]: Invalid parameter number\r\n");
		error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
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
	if (error_no == RTW_AT_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_wlstaticip_help();
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}
#endif /* CONFIG_LWIP_LAYER */

log_item_t at_wifi_items[ ] = {
#ifndef CONFIG_WHC_BRIDGE_HOST
#ifdef CONFIG_LWIP_LAYER
	{"+WLSTATICIP", at_wlstaticip, {NULL, NULL}},
#endif /* CONFIG_LWIP_LAYER */
#ifdef CONFIG_WLAN
	{"+WLCONN", at_wlconn, {NULL, NULL}},
	{"+WLDISCONN", at_wldisconn, {NULL, NULL}},
	{"+WLSCAN", at_wlscan, {NULL, NULL}},
	{"+WLRSSI", at_wlrssi, {NULL, NULL}},
	{"+WLSTARTAP", at_wlstartap, {NULL, NULL}},
	{"+WLSTOPAP", at_wlstopap, {NULL, NULL}},
	{"+WLSTATE", at_wlstate, {NULL, NULL}},
	{"+WLRECONN", at_wlreconn, {NULL, NULL}},
	{"+WLPROMISC", at_wlpromisc, {NULL, NULL}},
	{"+WLDBG", at_wldbg, {NULL, NULL}},
#ifdef CONFIG_WPS
	{"+WLWPS", at_wlwps, {NULL, NULL}},
#endif
	{"+WLPS", at_wlps, {NULL, NULL}},
#endif /* CONFIG_WLAN */
#endif
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

void at_wifi_init(void)
{
#ifdef CONFIG_WLAN
	init_wifi_struct();
#endif
#ifndef CONFIG_MP_SHRINK
	atcmd_service_add_table(at_wifi_items, sizeof(at_wifi_items) / sizeof(at_wifi_items[0]));
#endif
}

#endif /* CONFIG_MP_SHRINK */
