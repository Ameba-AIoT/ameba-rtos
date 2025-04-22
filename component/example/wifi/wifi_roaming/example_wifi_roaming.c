#include "rtw_autoconf.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "wifi_api.h"
#include "lwip_netconf.h"
#include "lwip/netif.h"

#ifndef ETH_ALEN
#define ETH_ALEN 			6
#endif

#define SCAN_BUFLEN 500 	//each scan list length= 14 + ssid_length(32MAX). so SCAN_BUFLEN should be AP_NUM*(14+32) at least
#define RSSI_THRESHOLD -65
#define MAX_POLLING_COUNT 5
#define MAX_AP_NUM 3

typedef struct wifi_roaming_ap {
	u8 	ssid[33];
	u8 	bssid[ETH_ALEN];
	u8	channel;
	enum rtw_security		security_type;
	u8 	password[65];
	u8	key_idx;
	s32	rssi;
#ifdef CONFIG_LWIP_LAYER
	u8	ip[4];
#endif
} wifi_roaming_ap_t;

static wifi_roaming_ap_t *ap_list[MAX_AP_NUM] = {0};
static u32 ap_count = 0;//scanned AP NUM
static u8 pscan_enable = FALSE; // if set TRUE, please set pscan_channel_list
static u8 pscan_channel_list[] = {1, 3, 5}; // set by customer

u32 wifi_roaming_find_ap_from_scan_buf(u8 *target_ssid, void *user_data, int ap_num)
{
	(void)target_ssid;
	wifi_roaming_ap_t *pwifi = (wifi_roaming_ap_t *)user_data;
	struct rtw_scan_result *scanned_ap_info;
	wifi_roaming_ap_t *candicate, *temp;
	u32 i, j = 0;
	pwifi->rssi = -100;//init
	struct rtw_scan_result *scanned_ap_list = NULL;

	scanned_ap_list = (struct rtw_scan_result *)rtos_mem_zmalloc(ap_num * sizeof(struct rtw_scan_result));
	if (scanned_ap_list == NULL) {
		printf("malloc scan buf for example wifi roaming\n");
		return -1;
	}

	if (wifi_get_scan_records((u32 *)&ap_num, scanned_ap_list) < 0) {
		rtos_mem_free(scanned_ap_list);
		return -1;
	}

	for (i = 0; i < (u32)ap_num; i++) {
		scanned_ap_info = &scanned_ap_list[i];
		if (pwifi->security_type == scanned_ap_info->security ||
			((pwifi->security_type & (WPA2_SECURITY | WPA_SECURITY)) && (scanned_ap_info->security & (WPA2_SECURITY | WPA_SECURITY)))) {
			if (ap_count < MAX_AP_NUM) {
				candicate = (wifi_roaming_ap_t *)malloc(sizeof(wifi_roaming_ap_t));
				if (!candicate) {
					printf("\r\n malloc buf for AP info fail!");
					break;
				}
				memset(candicate, 0, sizeof(wifi_roaming_ap_t));
				memcpy(candicate->ssid, scanned_ap_info->ssid.val, scanned_ap_info->ssid.len);
				memcpy(candicate->bssid, scanned_ap_info->bssid.octet, ETH_ALEN);
				candicate->channel = scanned_ap_info->channel;
				candicate->security_type = scanned_ap_info->security;
				memcpy(candicate->password, pwifi->password, strlen((char const *)pwifi->password));
				candicate->key_idx = pwifi->key_idx;
				candicate->rssi = scanned_ap_info->signal_strength;
				ap_list[ap_count++] = candicate;
			}
		}
	}

	for (i = 0; i < ap_count; i++) {
		for (j = 0; j < ap_count - 1 - i ; j++)
			if (ap_list[j]->rssi < ap_list[j + 1]->rssi) {
				temp = ap_list[j];
				ap_list[j] = ap_list[j + 1];
				ap_list[j + 1] = temp;
			}
	}
	rtos_mem_free(scanned_ap_list);
	return 0;
}

void wifi_ip_changed_hdl(u8 *buf, u32 buf_len, u32 flags, void *userdata)
{
	(void)buf;
	(void)buf_len;
	(void)flags;
	(void)userdata;
	//todo for customer
	printf("\r\n IP has channged!");
}
void wifi_roaming_thread(void *param)
{
	(void)param;
	struct rtw_wifi_setting	setting;
	wifi_roaming_ap_t	roaming_ap;
	u32	i = 0, polling_count = 0;
	struct rtw_scan_param scan_param;
	int scanned_ap_num = 0;
	struct rtw_network_info connect_param;
	union rtw_phy_stats phy_stats;
	memset(&scan_param, 0, sizeof(struct rtw_scan_param));
	memset(&connect_param, 0, sizeof(struct rtw_network_info));
	memset(&setting, 0, sizeof(struct rtw_wifi_setting));
	memset(&roaming_ap, 0, sizeof(wifi_roaming_ap_t));
	roaming_ap.rssi = -100;
	u8 autoreconn_en = 0;
	u8 join_status = RTW_JOINSTATUS_UNKNOWN;
#ifdef CONFIG_LWIP_LAYER
	uint8_t *IP = LwIP_GetIP(0);
#endif
	printf("\nExample: wifi_roaming \n");
	while (1) {
		if (wifi_is_running(STA_WLAN_INDEX) && wifi_get_join_status(&join_status) == RTK_SUCCESS
			&& ((join_status == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
			wifi_get_phy_stats(STA_WLAN_INDEX, NULL, &phy_stats);
			if ((phy_stats.sta.rssi < RSSI_THRESHOLD)) {
				if (polling_count >= (MAX_POLLING_COUNT - 1)) {
					wifi_get_setting(STA_WLAN_INDEX, &setting);
					strcpy((char *)roaming_ap.ssid, (char const *)setting.ssid);
					roaming_ap.security_type =  setting.security_type;
					strcpy((char *)roaming_ap.password, (char const *)setting.password);
					roaming_ap.key_idx = setting.key_idx;
					memcpy(roaming_ap.bssid, setting.bssid, 6);
#ifdef CONFIG_LWIP_LAYER
					memcpy(roaming_ap.ip, IP, 4);
#endif
					if (pscan_enable == TRUE) {
						scan_param.channel_list = pscan_channel_list;
						scan_param.channel_list_num = sizeof(pscan_channel_list);
					}

					scan_param.ssid = roaming_ap.ssid;
					scanned_ap_num = wifi_scan_networks(&scan_param, 1);
					if (scanned_ap_num > 0) {
						wifi_roaming_find_ap_from_scan_buf(roaming_ap.ssid, (void *)&roaming_ap, scanned_ap_num);
					}

					if ((wifi_get_autoreconnect(&autoreconn_en) == RTK_SUCCESS) && autoreconn_en) {
						wifi_set_autoreconnect(0);
					}

					i = 0;
connect_ap:
					if (ap_list[i] && memcmp(roaming_ap.bssid, ap_list[i]->bssid, ETH_ALEN)) {
						u8 retry_time = 0;
						while (1) {
							connect_param.channel = ap_list[i]->channel;
							memcpy(connect_param.ssid.val, (char *)ap_list[i]->ssid, strlen((char const *)ap_list[i]->ssid));
							connect_param.ssid.len = strlen((char const *)ap_list[i]->ssid);
							memcpy(connect_param.bssid.octet, ap_list[i]->bssid, ETH_ALEN);
							connect_param.security_type = ap_list[i]->security_type;
							connect_param.password = ap_list[i]->password;
							connect_param.password_len =  strlen((char const *)ap_list[i]->password);
							connect_param.key_id = ap_list[i]->key_idx;
							if (RTK_SUCCESS == wifi_connect(&connect_param, 1)) {
#ifdef CONFIG_LWIP_LAYER

								LwIP_DHCP(0, DHCP_START);

								if (memcmp(roaming_ap.ip, IP, 4)) {
									wifi_ip_changed_hdl(NULL, 0, 0, NULL);
								}
#endif
								break;
							}

							retry_time ++;
							if (retry_time > 3) {
								i++;
								goto connect_ap;
							}
						}
					}

					while (ap_count) {
						free(ap_list[ap_count - 1]);
						ap_count--;
					}
					memset(ap_list, 0, sizeof(ap_list));

					polling_count = 0;

					if (autoreconn_en) {
						wifi_set_autoreconnect(1);
					}

				} else {
					polling_count++;
				}
			} else {
				polling_count = 0;
			}
		}
		rtos_time_delay_ms(2000);// 2s
	}
	//rtos_task_delete(NULL);
}

void example_wifi_roaming(void)
{

	if (rtos_task_create(NULL, ((const char *)"wifi_roaming_thread"), wifi_roaming_thread, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		printf("\n\r%s rtos_task_create(wifi_roaming_thread) failed", __FUNCTION__);
	}

	return;
}
