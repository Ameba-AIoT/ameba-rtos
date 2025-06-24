/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include "example_wifi_csi.h"

extern s32 wifi_csi_config(struct rtw_csi_action_parm *act_param);
struct csi_report_q_priv g_csi_rpt_q_priv;
rtos_sema_t wc_ready_sema = NULL;

static void wifi_csi_buffer_deinit(void)
{
	u8 i = 0;
	u8 *csi_buffer = NULL;

	if (g_csi_rpt_q_priv.csi_rpt_q_mutex) {
		rtos_mutex_take(g_csi_rpt_q_priv.csi_rpt_q_mutex, MUTEX_WAIT_TIMEOUT);

		for (i = 0; i < CSI_REPORT_BUF_NUM; i++) {
			csi_buffer = g_csi_rpt_q_priv.csi_rpt_pkt[i].csi_buffer;
			if (csi_buffer) {
				rtos_mem_free(csi_buffer);
				g_csi_rpt_q_priv.csi_rpt_pkt[i].csi_buffer = NULL;
				csi_buffer = NULL;
			}
		}

		rtos_mutex_give(g_csi_rpt_q_priv.csi_rpt_q_mutex);

		rtos_mutex_delete_static(g_csi_rpt_q_priv.csi_rpt_q_mutex);
	}
}

static s32 wifi_csi_buffer_init(void)
{
	u8 i = 0;
	s32 ret = RTK_SUCCESS;
	struct csi_report_data *csi_rpt_pkt = NULL;

	rtos_mutex_create_static(&g_csi_rpt_q_priv.csi_rpt_q_mutex);
	rtw_init_listhead(&g_csi_rpt_q_priv.idle_q);
	rtw_init_listhead(&g_csi_rpt_q_priv.busy_q);

	g_csi_rpt_q_priv.idle_q_cnt = 0;

	for (i = 0; i < CSI_REPORT_BUF_NUM; i++) {
		csi_rpt_pkt = &g_csi_rpt_q_priv.csi_rpt_pkt[i];
		csi_rpt_pkt->csi_buffer = rtos_mem_zmalloc(CSI_REPORT_BUF_SIZE);
		if (csi_rpt_pkt->csi_buffer) {
			rtw_init_listhead(&csi_rpt_pkt->list);
			rtw_list_insert_tail(&csi_rpt_pkt->list, &g_csi_rpt_q_priv.idle_q);
			csi_rpt_pkt->length = 0;
			g_csi_rpt_q_priv.idle_q_cnt++;
		} else {
			RTK_LOGA(NOTAG, "ERR: csi rpt init err: malloc fail\n");
			ret = RTK_FAIL;
			break;
		}
	}

	if (RTK_SUCCESS != ret) {
		wifi_csi_buffer_deinit();
	}

	return ret;
}

static void wifi_csi_enqueue_busy_q(struct csi_report_data *csi_rpt_pkt)
{

	if (g_csi_rpt_q_priv.csi_rpt_q_mutex) {

		rtos_mutex_take(g_csi_rpt_q_priv.csi_rpt_q_mutex, MUTEX_WAIT_TIMEOUT);

		rtw_init_listhead(&csi_rpt_pkt->list);
		/* enqueue the latest first. */
		rtw_list_insert_tail(&csi_rpt_pkt->list, &g_csi_rpt_q_priv.busy_q);
		g_csi_rpt_q_priv.busy_q_cnt++;

		rtos_mutex_give(g_csi_rpt_q_priv.csi_rpt_q_mutex);
	}
}

static struct csi_report_data *wifi_csi_dequeue_busy_q(bool latest)
{
	struct csi_report_data	*csi_rpt_pkt = NULL;

	if (g_csi_rpt_q_priv.csi_rpt_q_mutex) {

		rtos_mutex_take(g_csi_rpt_q_priv.csi_rpt_q_mutex, MUTEX_WAIT_TIMEOUT);

		if (FALSE == rtw_is_list_empty(&g_csi_rpt_q_priv.busy_q)) {
			if (latest) {
				csi_rpt_pkt = container_of(get_prev(&g_csi_rpt_q_priv.busy_q), struct csi_report_data, list);
			} else {
				csi_rpt_pkt = container_of(get_next(&g_csi_rpt_q_priv.busy_q), struct csi_report_data, list);
			}
			rtw_list_delete(&csi_rpt_pkt->list);
			g_csi_rpt_q_priv.busy_q_cnt--;
		}

		rtos_mutex_give(g_csi_rpt_q_priv.csi_rpt_q_mutex);
	}

	return csi_rpt_pkt;
}

static void wifi_csi_enqueue_idle_q(struct csi_report_data *csi_rpt_pkt)
{
	if (g_csi_rpt_q_priv.csi_rpt_q_mutex) {

		rtos_mutex_take(g_csi_rpt_q_priv.csi_rpt_q_mutex, MUTEX_WAIT_TIMEOUT);

		memset(csi_rpt_pkt->csi_buffer, 0, CSI_REPORT_BUF_SIZE);
		rtw_init_listhead(&csi_rpt_pkt->list);
		rtw_list_insert_tail(&csi_rpt_pkt->list, &g_csi_rpt_q_priv.idle_q);
		csi_rpt_pkt->length = 0;
		g_csi_rpt_q_priv.idle_q_cnt++;

		rtos_mutex_give(g_csi_rpt_q_priv.csi_rpt_q_mutex);
	}
}

static struct csi_report_data *wifi_csi_dequeue_idle_q(void)
{
	struct csi_report_data	*csi_rpt_pkt = NULL;

	if (g_csi_rpt_q_priv.csi_rpt_q_mutex) {

		rtos_mutex_take(g_csi_rpt_q_priv.csi_rpt_q_mutex, MUTEX_WAIT_TIMEOUT);

		if (FALSE == rtw_is_list_empty(&g_csi_rpt_q_priv.idle_q)) {
			csi_rpt_pkt = container_of(get_next(&g_csi_rpt_q_priv.idle_q), struct csi_report_data, list);
			rtw_list_delete(&csi_rpt_pkt->list);
			g_csi_rpt_q_priv.idle_q_cnt--;
		} else if (FALSE == rtw_is_list_empty(&g_csi_rpt_q_priv.busy_q)) {
			/* drop oldest csi because of the variability of the channel */
			csi_rpt_pkt = container_of(get_next(&g_csi_rpt_q_priv.busy_q), struct csi_report_data, list);
			memset(csi_rpt_pkt->csi_buffer, 0, CSI_REPORT_BUF_SIZE);
			csi_rpt_pkt->length = 0;
			rtw_list_delete(&csi_rpt_pkt->list);
			g_csi_rpt_q_priv.busy_q_cnt--;
		} else {
			RTK_LOGA(NOTAG, "WARN: lack of csi buf!\n");
		}

		rtos_mutex_give(g_csi_rpt_q_priv.csi_rpt_q_mutex);
	}

	return csi_rpt_pkt;
}

/* wifi csi report callback */
void example_wifi_csi_report_cb(u8 *buf, s32 buf_len, s32 flags, void *userdata)
{
	(void) flags;
	(void) userdata;
	struct csi_report_data	*csi_rpt_pkt = NULL;

	csi_rpt_pkt = wifi_csi_dequeue_idle_q();

	if (csi_rpt_pkt && (buf_len <= CSI_REPORT_BUF_SIZE)) {
		memcpy(csi_rpt_pkt->csi_buffer, buf, buf_len);
		csi_rpt_pkt->length = buf_len;
		wifi_csi_enqueue_busy_q(csi_rpt_pkt);
		rtos_sema_give(wc_ready_sema);
	} else {
		RTK_LOGA(NOTAG, "ERR: csi_callback fail(null_buf or smaller size)\n");
	}
}

static void wifi_csi_thread(void *param)
{
	(void)param;
	u8 join_status = RTW_JOINSTATUS_UNKNOWN;
	struct rtw_csi_action_parm act_param = {0};
	struct csi_report_data	*csi_rpt_pkt = NULL;
	unsigned char assoc_ap_mac[6] = {0xa4, 0x39, 0xb3, 0xa4, 0xbe, 0x2d};  /* need modify to mac address of associated AP when sta mode */
	struct rtw_client_list client_info = {0};
	memset(&client_info, 0, sizeof(struct rtw_client_list));
	memcpy(act_param.mac_addr, assoc_ap_mac, 6);

	while (1) {
NEXT:
		if (wifi_is_running(SOFTAP_WLAN_INDEX)) {
			wifi_ap_get_connected_clients(&client_info);
			if (client_info.count) {
				memcpy(act_param.mac_addr, client_info.mac_list[0].octet, 6);
				RTK_LOGA(NOTAG, "### SOFTAP Break ###\r\n");
				break;
			}
			rtos_time_delay_ms(2000);  /* 2s */
			goto NEXT;

		}
		if (wifi_is_running(STA_WLAN_INDEX) && wifi_get_join_status(&join_status) == RTK_SUCCESS
			&& (join_status == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID)) {
			rtos_time_delay_ms(2000);  /* 2s */
			RTK_LOGA(NOTAG, "### STA Break ###\r\n");
			break;
		}
		rtos_time_delay_ms(2000);  /* 2s */
	}

	/* register wifi event callback function */
	wifi_reg_event_handler(RTW_EVENT_CSI_DONE, example_wifi_csi_report_cb, NULL);

	/* init csi report buffer pool */
	if (wifi_csi_buffer_init() != RTK_SUCCESS) {
		goto done;
	}
	/**
	 * should use semaphore to wait wifi csi report happen
	 * the following example shows that we wait for semaphore: wc_ready_sema
	 */
	rtos_sema_create(&wc_ready_sema, 0, 0xFFFFFFFF);
	if (!wc_ready_sema) {
		RTK_LOGA(NOTAG, "ERR: wc_ready_sema failed\r\n");
		goto done;
	}

	/* config csi parameters and enable wifi csi for first peer device */
	act_param.group_num = RTW_CSI_GROUP_NUM_1;
	act_param.mode = RTW_CSI_MODE_RX_RESP;
	act_param.accuracy = RTW_CSI_ACCU_1BYTE;
	act_param.trig_period = 200;          /* units: 320us */
	act_param.data_rate = RTW_RATE_6M;    /* ofdm 6 mpbs*/
	act_param.trig_frame_mgnt = 0;        /* no need for rx resp mode, default 0*/
	act_param.trig_frame_ctrl = 0;        /* no need for rx resp mode, default 0*/
	act_param.trig_frame_data = 0;        /* no need for rx resp mode, default 0*/
	act_param.csi_role = RTW_CSI_OP_ROLE_TRX;

	/* cis cfg and csi en */
	act_param.act = RTW_CSI_ACT_CFG;  /* csi cfg */
	wifi_csi_config(&act_param);

	act_param.act = RTW_CSI_ACT_EN;  /* csi en */
	act_param.enable = 1;
	wifi_csi_config(&act_param);

	// /* config csi parameters and enable wifi csi for second peer device */
	// act_param.xxx = xxxx;
	// /* cis cfg and csi en */
	// act_param.act = RTW_CSI_ACT_CFG;  /* csi cfg */
	// wifi_csi_config(&act_param);

	// act_param.act = RTW_CSI_ACT_EN;  /* csi en */
	// act_param.enable = 1;
	// wifi_csi_config(&act_param);

	// /* config csi parameters and enable wifi csi for third peer device */
	//.....

	while (1) {
		/* example: when wifi csi rx done, dequeue csi report and do some process. */
		if (rtos_sema_take(wc_ready_sema, 0xFFFFFFFF) != RTK_SUCCESS) {
			rtos_sema_delete(wc_ready_sema);
			RTK_LOGA(NOTAG,  "ERR: get wc_ready_sema failed");

			/* first peer device csi disable */
			act_param.act = RTW_CSI_ACT_EN;  /* csi dis */
			act_param.enable = 0;
			wifi_csi_config(&act_param);

			// /* second peer device csi disable */
			//......

			break;
		}

		csi_rpt_pkt = wifi_csi_dequeue_busy_q(TRUE);
		if (csi_rpt_pkt != NULL) {

			/*do something for handing csi info: like show csi data */
			wifi_csi_show(csi_rpt_pkt);

			wifi_csi_enqueue_idle_q(csi_rpt_pkt);
			csi_rpt_pkt = NULL;
		} else {
			RTK_LOGA(NOTAG, "WARN: deq csi_rpt_q fail\n");
		}
	}

done:
	/* unregister wifi event callback function */
	wifi_unreg_event_handler(RTW_EVENT_CSI_DONE, example_wifi_csi_report_cb);

	/* free csi report buffer */
	wifi_csi_buffer_deinit();

	if (wc_ready_sema) {
		rtos_sema_delete(wc_ready_sema);
	}

	rtos_task_delete(NULL);
}

void wifi_csi_show(struct csi_report_data *csi_rpt_pkt)
{
	struct rtw_csi_header *csi_header = (struct rtw_csi_header *)csi_rpt_pkt->csi_buffer;
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

	RTK_LOGA(NOTAG, "[CH INFO] csi raw data: len = %d[%d]\r\n", csi_header->csi_data_length, csi_rpt_pkt->length);

	buff_tmp = (u64 *)(csi_rpt_pkt->csi_buffer + csi_header->hdr_len + 3);
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
	if (rtos_task_create(NULL, ((const char *)"wifi_csi_thread"), wifi_csi_thread, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGA(NOTAG, "\n\r%s rtos_task_create(wifi_csi_thread) failed", __FUNCTION__);
	}

	return;
}

