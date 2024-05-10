#include "platform_autoconf.h"
#include "wifi_conf.h"
#include "os_wrapper.h"

/*
 * SCAN_DONE_INTERVAL is the interval between each channel scan done,
 * to make AP mode can send beacon during this interval.
 * It is to fix client disconnection when doing wifi scan in AP/concurrent mode.
 * User can fine tune SCAN_DONE_INTERVAL value.
 */
#define SCAN_DONE_INTERVAL 100 //100ms

#define SCAN_CHANNEL_NUM 13+25 //2.4GHz + 5GHz

u8 scan_channel_list[SCAN_CHANNEL_NUM] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
										  36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 149, 153, 157, 161, 165
										 };


u8 scan_running = 0;
/* this callback will be executed once a single channel is scanned done, thus it will be executed multiple times*/
static rtw_result_t scan_mcc_result_handler(unsigned int scanned_AP_num, void *user_data)
{
	/* To avoid gcc warnings */
	(void) user_data;

	int ap_num_on_single_channel = scanned_AP_num;
	rtw_scan_result_t *scanned_AP_info;
	int i = 0;
	char *scan_buf = NULL;
	int ret = RTW_ERROR;

	if (ap_num_on_single_channel > 0) {
		scan_buf = (char *)rtos_mem_zmalloc(ap_num_on_single_channel * sizeof(rtw_scan_result_t));
		if (scan_buf == NULL) {
			printf("malloc scan buf fail for scan mcc\n");
			ret = RTW_ERROR;
			goto EXIT;
		}
		if (wifi_get_scan_records(&ap_num_on_single_channel, scan_buf) < 0) {
			rtos_mem_free(scan_buf);
			ret = RTW_ERROR;
			goto EXIT;
		}
		//print each scan result on one channel
		for (i = 0; i < ap_num_on_single_channel; i++) {
			scanned_AP_info = (rtw_scan_result_t *)(scan_buf + i * sizeof(rtw_scan_result_t));
			printf("%s\t ", (scanned_AP_info->bss_type == RTW_BSS_TYPE_ADHOC) ? "Adhoc" : "Infra");
			printf(MAC_FMT, MAC_ARG(scanned_AP_info->BSSID.octet));
			printf(" %d\t ", scanned_AP_info->signal_strength);
			printf(" %d\t  ", scanned_AP_info->channel);
			printf(" %d\t  ", (unsigned int)scanned_AP_info->wps_type);
			printf("%s\t\t ", (scanned_AP_info->security == RTW_SECURITY_OPEN) ? "Open" :
				   (scanned_AP_info->security == RTW_SECURITY_WEP_PSK) ? "WEP" :
				   (scanned_AP_info->security == RTW_SECURITY_WPA_TKIP_PSK) ? "WPA TKIP" :
				   (scanned_AP_info->security == RTW_SECURITY_WPA_AES_PSK) ? "WPA AES" :
				   (scanned_AP_info->security == RTW_SECURITY_WPA_MIXED_PSK) ? "WPA Mixed" :
				   (scanned_AP_info->security == RTW_SECURITY_WPA2_AES_PSK) ? "WPA2 AES" :
				   (scanned_AP_info->security == RTW_SECURITY_WPA2_TKIP_PSK) ? "WPA2 TKIP" :
				   (scanned_AP_info->security == RTW_SECURITY_WPA2_MIXED_PSK) ? "WPA2 Mixed" :
				   (scanned_AP_info->security == RTW_SECURITY_WPA_WPA2_TKIP_PSK) ? "WPA/WPA2 TKIP" :
				   (scanned_AP_info->security == RTW_SECURITY_WPA_WPA2_AES_PSK) ? "WPA/WPA2 AES" :
				   (scanned_AP_info->security == RTW_SECURITY_WPA_WPA2_MIXED_PSK) ? "WPA/WPA2 Mixed" :
#ifdef CONFIG_SAE_SUPPORT
				   (scanned_AP_info->security == RTW_SECURITY_WPA3_AES_PSK) ? "WPA3-SAE AES" :
				   (scanned_AP_info->security == RTW_SECURITY_WPA2_WPA3_MIXED) ? "WPA2/WPA3-SAE AES" :
#endif
				   "Unknown            ");
			printf(" %s\n", scanned_AP_info->SSID.val);
		}
		rtos_mem_free(scan_buf);
		ret = RTW_SUCCESS;
	}
	printf("Total num of AP scanned = %d\n\n", ap_num_on_single_channel);
EXIT:
	scan_running = 0;
	return ret;
}

int wifi_scan_networks_mcc(scan_user_callback_t user_callback, void *user_data)
{
	u8 channel_index;
	rtw_scan_param_t scan_param;

	/* Add 2s delay to prevent conflict with wifi scan when doing fast connect */
	rtos_time_delay_ms(2000);

	/* lock 2s to forbid suspend under scan */
	rtw_wakelock_timeout(2 * 1000);

#ifdef SUPPORT_5G_CHANNEL
	for (channel_index = 0; channel_index < SCAN_CHANNEL_NUM; channel_index++) {
#else
	for (channel_index = 0; channel_index < 13; channel_index++) {
#endif
		if (scan_running) {
			int count = 200;
			while (scan_running && count > 0) {
				rtos_time_delay_ms(5);
				count --;
			}
			if (count == 0) {
				printf("\n\r[%d]WiFi: Scan is running. Wait 1s timeout.\n", rtos_time_get_current_system_time_ms());
				return RTW_TIMEOUT;
			}
		}
		//the interval between each channel scan done, make AP mode can send beacon during this interval
		rtos_time_delay_ms(SCAN_DONE_INTERVAL);

		scan_running = 1;
		memset(&scan_param, 0, sizeof(rtw_scan_param_t));
		scan_param.scan_user_callback = user_callback;
		scan_param.scan_user_data = user_data;

		//set partial scan for entering to listen beacon quickly
		scan_param.channel_list = &scan_channel_list[channel_index];
		scan_param.channel_list_num = 1;
		printf("Current scan channel is %d\n", scan_channel_list[channel_index]);
		if (wifi_scan_networks(&scan_param, 0) < 0) {
			return RTW_ERROR;
		}
	}

	return RTW_SUCCESS;
}

void wifi_scan_mcc_thread(void *param)
{
	printf("\n EXAMPLE: wifi_scan_mcc \n");
	while (!wifi_is_running(WLAN0_IDX)) {
		rtos_time_delay_ms(2000);
	}
	wifi_scan_networks_mcc(scan_mcc_result_handler, NULL);
	rtos_task_delete(NULL);
}

void example_wifi_scan_mcc(void)
{
	if (rtos_task_create(NULL, ((const char *)"wifi_scan_mcc_thread"), wifi_scan_mcc_thread, NULL, 1024 * 4, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(wifi_scan_mcc_thread) failed", __FUNCTION__);
	}

	return;
}

