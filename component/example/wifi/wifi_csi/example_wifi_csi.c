/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include "platform_autoconf.h"
#include "example_wifi_csi.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "lwip_netconf.h"
#include "wifi_conf.h"
#include "rtw_autoconf.h"

extern int wifi_csi_config(rtw_csi_action_parm_t *act_param);
extern int wifi_csi_report(u32 buf_len, u8 *csi_buf, u32 *len);

rtos_sema_t wc_ready_sema = NULL;
unsigned int csi_data_len = 2048;

static void wifi_csi_thread(void *param)
{
	rtw_csi_action_parm_t act_param = {0};
	unsigned int len;
	unsigned char *csi_buf = NULL;

	unsigned long long *buff_tmp = NULL; /* for printf csi data*/
	unsigned int timestamp;
	unsigned char i = 0;
	unsigned char assoc_ap_mac[6] = {0xa4, 0x39, 0xb3, 0xa4, 0xbe, 0x2d};  /* need modify to mac address of associated AP when sta mode */
	struct {
		unsigned int    count;
		rtw_mac_t mac_list[AP_STA_NUM];
	} client_info;

	act_param.group_num = 0;
	act_param.mode = 2;
	act_param.accuracy = 0;
	act_param.trig_period = 200;  /* units: depend on ICs */
	act_param.data_rate = 0xC;  /* ofdm 6 mpbs*/
	act_param.trig_frame_mgnt = 0;   /* no need for rx resp mode, default 0*/
	act_param.trig_frame_ctrl = 0;   /* no need for rx resp mode, default 0*/
	act_param.trig_frame_data = 0;   /* no need for rx resp mode, default 0*/
	memcpy(act_param.mac_addr, assoc_ap_mac, 6);

	while (1) {
NEXT:
		if (wifi_is_running(SOFTAP_WLAN_INDEX)) {
			wifi_get_associated_client_list(&client_info);
			if (client_info.count) {
				memcpy(act_param.mac_addr, client_info.mac_list[0].octet, 6);
				printf(" ### SOFTAP Break ###\n");
				break;
			}
			rtos_time_delay_ms(2000);  /* 2s */
			goto NEXT;

		}
		if (wifi_is_running(STA_WLAN_INDEX) && (wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID)) {
			rtos_time_delay_ms(2000);  /* 2s */
			printf(" ### STA Break ###\n");
			break;
		}
		rtos_time_delay_ms(2000);  /* 2s */
	}

	/* register wifi event callback function */
	wifi_reg_event_handler(WIFI_EVENT_CSI_DONE, example_wifi_csi_report_cb, NULL);

	/**
	 * should use semaphore to wait wifi event happen
	 * the following example shows that we wait for wifi csi ready
	 */
	rtos_sema_create(&wc_ready_sema, 0, 0xFFFFFFFF);
	if (!wc_ready_sema) {
		printf("\nInit wc_ready_sema failed\n");
	}

	/* cis cfg and csi en */
	act_param.act = 1;  /* csi cfg */
	act_param.enable = 0;
	wifi_csi_config(&act_param);

	act_param.act = 0;  /* csi en */
	act_param.enable = 1;
	wifi_csi_config(&act_param);

	while (1) {
		/* example: when wifi csi rx done, call csi report handle function. */
		if (rtos_sema_take(wc_ready_sema, 0xFFFFFFFF) != SUCCESS) {
			rtos_sema_delete(wc_ready_sema);

			act_param.act = 0;  /* csi dis */
			act_param.enable = 0;
			wifi_csi_config(&act_param);
			break;
		}

		csi_buf = rtos_mem_malloc(csi_data_len);
		if (csi_buf != NULL) {
			wifi_csi_report(csi_data_len, csi_buf, &len);

			/*do something for handing csi info*/
			timestamp = (unsigned int)(csi_buf[18] << 24) | (unsigned int)(csi_buf[17] << 16) | (unsigned int)(csi_buf[16] << 8) | (unsigned int)csi_buf[15];
			printf("\n[CH INFO] timestamp = %d us, csi data(header+raw data): \n", timestamp);
			buff_tmp = (u64 *)csi_buf;
			for (i = 0; i < 8; i++) {
				printf("[%02d]0x%016llx\n", i, buff_tmp[i]);
			}
			printf("[CH INFO] ...(only show 64 bytes)\n");

		} else {
			printf("\n csi_buf malloc fail\n");
		}

		if (csi_buf != NULL) {
			rtos_mem_free(csi_buf);
		}
	}

	/* unregister wifi event callback function */
	wifi_unreg_event_handler(WIFI_EVENT_CSI_DONE, example_wifi_csi_report_cb);

	if (wc_ready_sema) {
		rtos_sema_delete(wc_ready_sema);
	}

	rtos_task_delete(NULL);
}

/* wifi csi report callback */
void example_wifi_csi_report_cb(char *buf, int buf_len, int flags, void *userdata)
{
	rtos_sema_give(wc_ready_sema);
	csi_data_len = flags;
	return;
}

void example_wifi_csi(void)
{
	if (rtos_task_create(NULL, ((const char *)"wifi_csi_thread"), wifi_csi_thread, NULL, 1024 * 4, 0) != SUCCESS) {
		printf("\n\r%s rtos_task_create(wifi_csi_thread) failed", __FUNCTION__);
	}

	return;
}

