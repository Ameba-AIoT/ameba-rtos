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

extern int wifi_csi_config(struct _rtw_csi_action_parm_t *act_param);
extern int wifi_csi_report(u32 buf_len, u8 *csi_buf, u32 *len);

rtos_sema_t wc_ready_sema = NULL;
unsigned int csi_data_len = 2048;

static void wifi_csi_thread(void *param)
{
	(void)param;
	struct _rtw_csi_action_parm_t act_param = {0};
	u32 len;
	unsigned char *csi_buf = NULL;
	unsigned char assoc_ap_mac[6] = {0xa4, 0x39, 0xb3, 0xa4, 0xbe, 0x2d};  /* need modify to mac address of associated AP when sta mode */
	struct _rtw_client_list_t client_info;
	memset(&client_info, 0, sizeof(struct _rtw_client_list_t));
	memcpy(act_param.mac_addr, assoc_ap_mac, 6);

	while (1) {
NEXT:
		if (wifi_is_running(SOFTAP_WLAN_INDEX)) {
			wifi_get_associated_client_list(&client_info);
			if (client_info.count) {
				memcpy(act_param.mac_addr, client_info.mac_list[0].octet, 6);
				RTK_LOGA(NOTAG, "### SOFTAP Break ###\r\n");
				break;
			}
			rtos_time_delay_ms(2000);  /* 2s */
			goto NEXT;

		}
		if (wifi_is_running(STA_WLAN_INDEX) && (wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID)) {
			rtos_time_delay_ms(2000);  /* 2s */
			RTK_LOGA(NOTAG, "### STA Break ###\r\n");
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
		RTK_LOGE(NOTAG, "\r\nInit wc_ready_sema failed\r\n\n");
	}

	/* config csi parameters and enable wifi csi */
	act_param.group_num = CSI_GROUP_NUM_1;
	act_param.mode = CSI_MODE_RX_RESP;
	act_param.accuracy = CSI_ACCU_1BYTE;
	act_param.trig_period = 200;     /* units: 320us */
	act_param.data_rate = MGN_6M;    /* ofdm 6 mpbs*/
	act_param.trig_frame_mgnt = 0;   /* no need for rx resp mode, default 0*/
	act_param.trig_frame_ctrl = 0;   /* no need for rx resp mode, default 0*/
	act_param.trig_frame_data = 0;   /* no need for rx resp mode, default 0*/
	act_param.csi_role = CSI_OP_ROLE_TRX;

	/* cis cfg and csi en */
	act_param.act = CSI_ACT_CFG;  /* csi cfg */
	wifi_csi_config(&act_param);

	act_param.act = CSI_ACT_EN;  /* csi en */
	act_param.enable = 1;
	wifi_csi_config(&act_param);

	while (1) {
		/* example: when wifi csi rx done, call csi report handle function. */
		if (rtos_sema_take(wc_ready_sema, 0xFFFFFFFF) != SUCCESS) {
			rtos_sema_delete(wc_ready_sema);

			act_param.act = CSI_ACT_EN;  /* csi dis */
			act_param.enable = 0;
			wifi_csi_config(&act_param);
			break;
		}

		csi_buf = rtos_mem_malloc(csi_data_len);
		if (csi_buf != NULL) {
			wifi_csi_report(csi_data_len, csi_buf, &len);

			/*do something for handing csi info: like show csi data */
			wifi_csi_show(csi_buf);

		} else {
			RTK_LOGE(NOTAG, "\r\n csi_buf malloc fail\r\n");
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
	(void)buf;
	(void)buf_len;
	(void)userdata;
	rtos_sema_give(wc_ready_sema);
	csi_data_len = flags;
	return;
}

void wifi_csi_show(unsigned char *csi_buf)
{
	struct rtw_csi_header *csi_header = (struct rtw_csi_header *)csi_buf;
	unsigned long long *buff_tmp = NULL; /* for printf csi data*/
	unsigned char print_len = 0, i = 0;

	RTK_LOGA(NOTAG, "[CH INFO] csi header info:\r\n");
	RTK_LOGA(NOTAG, "# sta_mac_addr     = " MAC_FMT "\r\n", MAC_ARG(csi_header->mac_addr));
	RTK_LOGA(NOTAG, "# trigger_mac_addr = " MAC_FMT "\r\n", MAC_ARG(csi_header->trig_addr));
	RTK_LOGA(NOTAG, "# trigger_flag     = %d\r\n", csi_header->trig_flag);
	RTK_LOGA(NOTAG, "# timestamp        = %lu us\r\n", csi_header->hw_assigned_timestamp);
	RTK_LOGA(NOTAG, "# csi_data_len     = %lu\r\n", csi_header->csi_data_length);
	RTK_LOGA(NOTAG, "# csi_sequence     = %lu\r\n", csi_header->csi_sequence);
	RTK_LOGA(NOTAG, "# channel          = %d\r\n", csi_header->channel);
	RTK_LOGA(NOTAG, "# bandwidth        = %d\r\n", csi_header->bandwidth);
	RTK_LOGA(NOTAG, "# rx_rate          = %d\r\n", csi_header->rx_rate);
	RTK_LOGA(NOTAG, "# protocol_mode    = %d [ofdm(0)/ht(1)/vht(2)/he(3)]\r\n", csi_header->protocol_mode);
	RTK_LOGA(NOTAG, "# num_sub_carrier  = %d\r\n", csi_header->num_sub_carrier);
	RTK_LOGA(NOTAG, "# num_bit_per_tone = %d [I:%d bits; Q:%d bits]\r\n", csi_header->num_bit_per_tone, csi_header->num_bit_per_tone / 2,
			 csi_header->num_bit_per_tone / 2);
	RTK_LOGA(NOTAG, "# rssi[0]          = %d dbm\r\n", csi_header->rssi[0]);
	RTK_LOGA(NOTAG, "# rxsc             = %d\r\n", csi_header->rxsc);
	RTK_LOGA(NOTAG, "# csi_valid        = %d\r\n", csi_header->csi_valid);

	RTK_LOGA(NOTAG, "[CH INFO] csi raw data: len = %d[%d]\r\n", csi_header->csi_data_length, csi_data_len);

	buff_tmp = (u64 *)(csi_buf + csi_header->hdr_len + 3);
	print_len = csi_header->csi_data_length >> 3;
	if (csi_header->csi_data_length % 8) {
		print_len++;
	}

	for (i = 0; i < print_len; i++) {
		RTK_LOGA(NOTAG, "[%02d]0x%016llx\r\n", i, buff_tmp[i]);
	}

	RTK_LOGA(NOTAG, "[CH INFO] raw data done!\r\n\n");
}

void example_wifi_csi(void)
{
	if (rtos_task_create(NULL, ((const char *)"wifi_csi_thread"), wifi_csi_thread, NULL, 1024 * 4, 0) != SUCCESS) {
		RTK_LOGA(NOTAG, "\n\r%s rtos_task_create(wifi_csi_thread) failed", __FUNCTION__);
	}

	return;
}

