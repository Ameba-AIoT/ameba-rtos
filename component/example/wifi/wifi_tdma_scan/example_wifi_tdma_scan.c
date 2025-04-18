#include "example_wifi_tdma_scan.h"
#include "wifi_api.h"
#include "basic_types.h"
#include "os_wrapper.h"


#define CH_NUM_PER_SCAN		10  /* number of channels per scan , set by customer*/
#define SCAN_DELAY			500  /* delay to proceed the next scan , set by customer*/

#define MAX_GET_CH_LIST_RETRY_CNT	3
#define TDMA_SCAN_DBG	0		//for debug log

#if TDMA_SCAN_DBG
#define SCAN_DBG(fmt, ...)     \
			do {\
				DiagPrintf("" fmt, ##__VA_ARGS__);\
			} while (0)
#else
#define SCAN_DBG(...)   do {} while(0)
#endif

struct rtw_scanned_ap_list {
	u8 ap_num;
	struct rtw_scan_result *scanned_ap_info;
};


static void scan_result_print(struct rtw_scan_result *record)
{
	(void)record;

	SCAN_DBG(""MAC_FMT", ", MAC_ARG(record->bssid.octet));
	SCAN_DBG("%d, ", record->signal_strength);
	SCAN_DBG("%ld, ", record->channel);
	SCAN_DBG("%s, ", (record->wireless_mode & RTW_80211_AX) ? "AX" :
			 (record->wireless_mode & RTW_80211_AC) ? "AC" :
			 (record->wireless_mode & RTW_80211_N) ? "N" :
			 (record->wireless_mode & RTW_80211_A) ? "A" :
			 (record->wireless_mode & RTW_80211_G) ? "G" :
			 (record->wireless_mode & RTW_80211_B) ? "B" :
			 "Unknown");
	SCAN_DBG("\"%s\", ", (record->security == RTW_SECURITY_OPEN) ? "Open" :
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
#ifdef CONFIG_OWE_SUPPORT
			 (record->security == RTW_SECURITY_WPA3_OWE) ? "WPA3-OWE" :
#endif
			 "Unknown");

	SCAN_DBG("\"%s\" ", record->ssid.val);
	SCAN_DBG("\r\n");
}


s32 wifi_tdma_scan(struct rtw_channel_list *ch_list)
{
	struct rtw_scan_param scan_param;
	u8 i, round = 0, round_total, scan_ch_num;
	u8 ch_num_accu = 0, ch_num_remain = ch_list->ch_num;
	u8 *scan_ch_list;
	unsigned long tick1, tick2;
	int scanned_ap_num, scanned_ap_num_total = 0;
	struct rtw_scanned_ap_list *scanned_ap_list;
	struct rtw_scan_result *scanned_ap_result, *scanned_ap_result_tmp, *scanned_AP_info;

	RTK_LOGA(NOTAG, "\nTotal ch_num %d, scan %d ch each time, delay %d\n", ch_list->ch_num, CH_NUM_PER_SCAN, SCAN_DELAY);

	round_total = ch_list->ch_num / CH_NUM_PER_SCAN;
	round_total += (ch_list->ch_num % CH_NUM_PER_SCAN) ? 1 : 0;
	scanned_ap_list = (struct rtw_scanned_ap_list *)rtos_mem_zmalloc(round_total * sizeof(struct rtw_scanned_ap_list));

	while (ch_num_remain) {
		if (ch_num_remain >= CH_NUM_PER_SCAN) {
			scan_ch_num = CH_NUM_PER_SCAN;
		} else {
			scan_ch_num = ch_num_remain;
		}

		scan_ch_list = (u8 *)malloc(sizeof(u8 *) * CH_NUM_PER_SCAN);
		SCAN_DBG("ch include: \n");
		for (i = 0; i < scan_ch_num; i++) {
			scan_ch_list[i] = ch_list->ch_info[ch_num_accu + i].channel;
			SCAN_DBG("%d ", scan_ch_list[i]);
		}
		SCAN_DBG("\n");

		//set scan_param for scan
		memset(&scan_param, 0, sizeof(struct rtw_scan_param));
		scan_param.channel_list = scan_ch_list;
		scan_param.channel_list_num = scan_ch_num;
		tick1 = rtos_time_get_current_system_time_ms();
		scanned_ap_num = wifi_scan_networks(&scan_param, 1);
		tick2 = rtos_time_get_current_system_time_ms();
		RTK_LOGA(NOTAG, "round %d cost %ld ms \n", round + 1, tick2 - tick1);

		if (scanned_ap_num > 0 && round < round_total) {
			scanned_ap_list[round].scanned_ap_info = (struct rtw_scan_result *)rtos_mem_zmalloc(scanned_ap_num * sizeof(struct rtw_scan_result));
			if (scanned_ap_list[round].scanned_ap_info) {
				if (wifi_get_scan_records((u32 *)&scanned_ap_num, scanned_ap_list[round].scanned_ap_info) < 0) {
					rtos_mem_free((u8 *)scanned_ap_list[round].scanned_ap_info);
				} else {
					scanned_ap_list[round].ap_num = scanned_ap_num;
					scanned_ap_num_total += scanned_ap_num;
					SCAN_DBG("ap_num %d \n", scanned_ap_num);
				}
			}
		}

		ch_num_accu += scan_ch_num;
		ch_num_remain -= scan_ch_num;
		round++;

		rtos_time_delay_ms(SCAN_DELAY);
	}

	scanned_ap_result = (struct rtw_scan_result *)rtos_mem_zmalloc(scanned_ap_num_total * sizeof(struct rtw_scan_result));
	scanned_ap_result_tmp = scanned_ap_result;

	for (i = 0; i < round_total; i++) {
		if (scanned_ap_list[i].scanned_ap_info) {
			memcpy(scanned_ap_result_tmp, scanned_ap_list[i].scanned_ap_info, scanned_ap_list[i].ap_num * sizeof(struct rtw_scan_result));
			rtos_mem_free((u8 *)scanned_ap_list[i].scanned_ap_info);
		}
		scanned_ap_result_tmp += scanned_ap_list[i].ap_num;
	}
	rtos_mem_free((u8 *)scanned_ap_list);

	/* printf scanned AP info */
	SCAN_DBG("total ap_num %d \n", scanned_ap_num_total);
	for (i = 0; i < scanned_ap_num_total; i++) {
		scanned_AP_info = &scanned_ap_result[i];
		scanned_AP_info->ssid.val[scanned_AP_info->ssid.len] = 0; /* Ensure the SSID is null terminated */

		scan_result_print(scanned_AP_info);
	}

	rtos_mem_free((u8 *)scanned_ap_result);

	return RTK_SUCCESS;
}


void wifi_tdma_scan_thread(void *param)
{
	(void)param;
	struct rtw_channel_list ch_list;
	u8 retry = 0;
	unsigned long tick1, tick2;

	rtos_time_delay_ms(10000);
	RTK_LOGA(NOTAG, "\nExample: wifi_scan \n");

	while (1) {
		tick1 = rtos_time_get_current_system_time_ms();
		wifi_get_channel_list(&ch_list);

		if (!ch_list.ch_num) {
			RTK_LOGE(NOTAG, "Avail ch list is empty, retry!\n");
			retry++;
			if (retry >= MAX_GET_CH_LIST_RETRY_CNT) {
				goto exit;
			}
			rtos_time_delay_ms(1000);
			continue;
		}

		if (CH_NUM_PER_SCAN == 0) {
			RTK_LOGE(NOTAG, "Error ch num set!\n");
			goto exit;
		}

		wifi_tdma_scan(&ch_list);
		tick2 = rtos_time_get_current_system_time_ms();

		RTK_LOGA(NOTAG, "[Wifi tdma scan]: done! total cost %ld ms\n", tick2 - tick1);

		rtos_time_delay_ms(10000);
	}

exit:
	rtos_task_delete(NULL);
}

void example_wifi_tdma_scan(void)
{
	if (rtos_task_create(NULL, ((const char *)"wifi_tdma_scan_thread"), wifi_tdma_scan_thread, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGA(NOTAG, "\n\r%s rtos_task_create(wifi_tdma_scan_thread) failed", __FUNCTION__);
	}
	return;
}
