/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include "example_wifi_csi.h"

struct csi_report_q_priv g_csi_rpt_q_priv;
extern int wifi_csi_config(rtw_csi_action_parm_t *act_param);

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
	s32 ret = SUCCESS;
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
			ret = FAIL;
			break;
		}
	}

	if (SUCCESS != ret) {
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

		if (_FALSE == rtw_is_list_empty(&g_csi_rpt_q_priv.busy_q)) {
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

		if (_FALSE == rtw_is_list_empty(&g_csi_rpt_q_priv.idle_q)) {
			csi_rpt_pkt = container_of(get_next(&g_csi_rpt_q_priv.idle_q), struct csi_report_data, list);
			rtw_list_delete(&csi_rpt_pkt->list);
			g_csi_rpt_q_priv.idle_q_cnt--;
		} else if (_FALSE == rtw_is_list_empty(&g_csi_rpt_q_priv.busy_q)) {
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
void example_wifi_csi_report_cb(char *buf, int buf_len, int flags, void *userdata)
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
	(void) param;
	rtw_csi_action_parm_t act_param = {0};
	struct csi_report_data	*csi_rpt_pkt = NULL;
	unsigned long long *buff_tmp = NULL; /* for printf csi data*/
	unsigned int timestamp;
	unsigned char i = 0;
	unsigned char assoc_ap_mac[6] = {0xa4, 0x39, 0xb3, 0xa4, 0xbe, 0x2d};  /* need modify to mac address of associated AP when sta mode */
	struct _rtw_client_list_t client_info = {0};

	while (1) {
NEXT:
		if (wifi_is_running(SOFTAP_WLAN_INDEX)) {
			wifi_get_associated_client_list(&client_info);
			if (client_info.count) {
				memcpy(act_param.mac_addr, client_info.mac_list[0].octet, 6);
				RTK_LOGA(NOTAG, "\n### SOFTAP Break ###\n");
				break;
			}
			rtos_time_delay_ms(2000);  /* 2s */
			goto NEXT;

		}
		if (wifi_is_running(STA_WLAN_INDEX) && (wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID)) {
			rtos_time_delay_ms(2000);  /* 2s */
			RTK_LOGA(NOTAG, "\n### STA Break ###\n");
			break;
		}
		rtos_time_delay_ms(2000);  /* 2s */
	}

	/* register wifi event callback function */
	wifi_reg_event_handler(WIFI_EVENT_CSI_DONE, example_wifi_csi_report_cb, NULL);

	/* init csi report buffer pool */
	if (wifi_csi_buffer_init() != SUCCESS) {
		goto done;
	}

	/**
	 * should use semaphore to wait wifi csi report happen
	 * the following example shows that we wait for semaphore: wc_ready_sema
	 */
	rtos_sema_create(&wc_ready_sema, 0, 0xFFFFFFFF);
	if (!wc_ready_sema) {
		RTK_LOGA(NOTAG, "\nERR: init wc_ready_sema failed\n");
		goto done;
	}

	/* config and enable first peer device CSI */
	act_param.group_num = 0;
	act_param.mode = 2;
	act_param.accuracy = 0;
	act_param.trig_period = 200;  /* units: depend on ICs */
	act_param.data_rate = 0xC;  /* ofdm 6 mpbs*/
	act_param.trig_frame_mgnt = 0;   /* no need for rx resp mode, default 0*/
	act_param.trig_frame_ctrl = 0;   /* no need for rx resp mode, default 0*/
	act_param.trig_frame_data = 0;   /* no need for rx resp mode, default 0*/
	memcpy(act_param.mac_addr, assoc_ap_mac, 6);

	/* csi cfg and csi en */
	act_param.act = 1;  /* csi cfg */
	act_param.enable = 0;
	wifi_csi_config(&act_param);

	act_param.act = 0;  /* csi en */
	act_param.enable = 1;
	wifi_csi_config(&act_param);

	// /* config and enable second peer device CSI */
	// act_param.xxx = xxxx;
	// /* cis cfg and csi en */
	// act_param.act = 1;  /* csi cfg */
	// act_param.enable = 0;
	// wifi_csi_config(&act_param);

	// act_param.act = 0;  /* csi en */
	// act_param.enable = 1;
	// wifi_csi_config(&act_param);

	// /* config and enable third peer device CSI */
	//......

	while (1) {
		/* example: when wifi csi rx done, dequeue csi report and do some process. */
		if (rtos_sema_take(wc_ready_sema, 0xFFFFFFFF) != SUCCESS) {
			RTK_LOGA(NOTAG, "\nERR: get wc_ready_sema failed");
			rtos_sema_delete(wc_ready_sema);

			/* first peer device csi disable */
			act_param.act = 0;  /* csi dis */
			act_param.enable = 0;
			wifi_csi_config(&act_param);

			// /* second peer device csi siable */
			//......
			break;
		}

		csi_rpt_pkt = wifi_csi_dequeue_busy_q(TRUE);

		if (csi_rpt_pkt != NULL) {
			/*do something for handing csi info*/
			timestamp = (unsigned int)(csi_rpt_pkt->csi_buffer[18] << 24) | (unsigned int)(csi_rpt_pkt->csi_buffer[17] << 16) |
						(unsigned int)(csi_rpt_pkt->csi_buffer[16] << 8) | (unsigned int)csi_rpt_pkt->csi_buffer[15];

			RTK_LOGA(NOTAG, "\n[CH INFO] timestamp = %d us, csi data(header+raw data): \n", timestamp);
			buff_tmp = (u64 *)csi_rpt_pkt->csi_buffer;
			for (i = 0; i < 8; i++) {
				RTK_LOGA(NOTAG, "[%02d]0x%016llx\n", i, buff_tmp[i]);
			}
			RTK_LOGA(NOTAG, "[CH INFO] ...(only show 64 bytes)\n");

			wifi_csi_enqueue_idle_q(csi_rpt_pkt);
			csi_rpt_pkt = NULL;
		} else {
			RTK_LOGA(NOTAG, "WARN: deq csi_rpt_q fail\n");
		}

	}

done:
	/* unregister wifi event callback function */
	wifi_unreg_event_handler(WIFI_EVENT_CSI_DONE, example_wifi_csi_report_cb);

	/* free csi report buffer */
	wifi_csi_buffer_deinit();

	if (wc_ready_sema) {
		rtos_sema_delete(wc_ready_sema);
	}

	rtos_task_delete(NULL);
}

void example_wifi_csi(void)
{
	if (rtos_task_create(NULL, ((const char *)"wifi_csi_thread"), wifi_csi_thread, NULL, 1024 * 4, 1) != SUCCESS) {
		RTK_LOGA(NOTAG, "\n\r%s ERR: rtos_task_create(wifi_csi_thread) failed", __FUNCTION__);
	}

	return;
}

