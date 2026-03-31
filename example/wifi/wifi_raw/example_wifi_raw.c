/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include "example_wifi_raw.h"


u8 addr1[6] = {0xBC, 0x46, 0x99, 0x7B, 0x48, 0x74};
u8 addr2[6] = {0x00, 0xE0, 0x4C, 0x00, 0x02, 0xF0};
u8 addr3[6] = {0xBC, 0x46, 0x99, 0x7B, 0x48, 0x74};

struct rx_promisc_q_priv g_rx_promisc_q_priv;
rtos_sema_t rx_promisc_ready_sema = NULL;

/* ------------------- Initialize & Operating promisc_rx_buffer_q START --------------------- */
static void wifi_rx_promisc_buffer_deinit(void)
{
	u8 i = 0;
	u8 *buffer = NULL;

	if (g_rx_promisc_q_priv.rx_promisc_q_mutex) {
		rtos_mutex_take(g_rx_promisc_q_priv.rx_promisc_q_mutex, MUTEX_WAIT_TIMEOUT);

		for (i = 0; i < RX_PROMISC_BUF_NUM; i++) {
			buffer = g_rx_promisc_q_priv.rx_promisc_pkt[i].buffer;
			if (buffer) {
				rtos_mem_free(buffer);
				g_rx_promisc_q_priv.rx_promisc_pkt[i].buffer = NULL;
				buffer = NULL;
			}
		}

		rtos_mutex_give(g_rx_promisc_q_priv.rx_promisc_q_mutex);

		rtos_mutex_delete_static(g_rx_promisc_q_priv.rx_promisc_q_mutex);
	}
}

static s32 wifi_rx_promisc_buffer_init(void)
{
	u8 i = 0;
	s32 ret = RTK_SUCCESS;
	struct rx_promisc_data *rx_promisc_pkt = NULL;

	rtos_mutex_create_static(&g_rx_promisc_q_priv.rx_promisc_q_mutex);
	rtw_init_listhead(&g_rx_promisc_q_priv.idle_q);
	rtw_init_listhead(&g_rx_promisc_q_priv.busy_q);

	g_rx_promisc_q_priv.idle_q_cnt = 0;

	for (i = 0; i < RX_PROMISC_BUF_NUM; i++) {
		rx_promisc_pkt = &g_rx_promisc_q_priv.rx_promisc_pkt[i];
		rx_promisc_pkt->buffer = rtos_mem_zmalloc(RX_PROMISC_BUF_SIZE);
		if (rx_promisc_pkt->buffer) {
			rtw_init_listhead(&rx_promisc_pkt->list);
			rtw_list_insert_tail(&rx_promisc_pkt->list, &g_rx_promisc_q_priv.idle_q);
			rx_promisc_pkt->data_len = 0;
			rx_promisc_pkt->data_rate = RTW_RATE_UNKNOWN;
			rx_promisc_pkt->rssi = -110;
			g_rx_promisc_q_priv.idle_q_cnt++;
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "rx_promisc buf_q init err: malloc fail\n");
			ret = RTK_FAIL;
			break;
		}
	}

	if (RTK_SUCCESS != ret) {
		wifi_rx_promisc_buffer_deinit();
	}

	return ret;
}

static void wifi_rx_promisc_enqueue_busy_q(struct rx_promisc_data *rx_promisc_pkt)
{

	if (g_rx_promisc_q_priv.rx_promisc_q_mutex) {

		rtos_mutex_take(g_rx_promisc_q_priv.rx_promisc_q_mutex, MUTEX_WAIT_TIMEOUT);

		rtw_init_listhead(&rx_promisc_pkt->list);
		/* enqueue the latest first. */
		rtw_list_insert_tail(&rx_promisc_pkt->list, &g_rx_promisc_q_priv.busy_q);
		g_rx_promisc_q_priv.busy_q_cnt++;

		rtos_mutex_give(g_rx_promisc_q_priv.rx_promisc_q_mutex);
	}
}

static struct rx_promisc_data *wifi_rx_promisc_dequeue_busy_q(bool latest)
{
	struct rx_promisc_data	*rx_promisc_pkt = NULL;

	if (g_rx_promisc_q_priv.rx_promisc_q_mutex) {

		rtos_mutex_take(g_rx_promisc_q_priv.rx_promisc_q_mutex, MUTEX_WAIT_TIMEOUT);

		if (FALSE == rtw_is_list_empty(&g_rx_promisc_q_priv.busy_q)) {
			if (latest) {
				rx_promisc_pkt = container_of(get_prev(&g_rx_promisc_q_priv.busy_q), struct rx_promisc_data, list);
			} else {
				rx_promisc_pkt = container_of(get_next(&g_rx_promisc_q_priv.busy_q), struct rx_promisc_data, list);
			}
			rtw_list_delete(&rx_promisc_pkt->list);
			g_rx_promisc_q_priv.busy_q_cnt--;
		}

		rtos_mutex_give(g_rx_promisc_q_priv.rx_promisc_q_mutex);
	}

	return rx_promisc_pkt;
}

static void wifi_rx_promisc_enqueue_idle_q(struct rx_promisc_data *rx_promisc_pkt)
{
	if (g_rx_promisc_q_priv.rx_promisc_q_mutex) {

		rtos_mutex_take(g_rx_promisc_q_priv.rx_promisc_q_mutex, MUTEX_WAIT_TIMEOUT);

		memset(rx_promisc_pkt->buffer, 0, RX_PROMISC_BUF_SIZE);
		rtw_init_listhead(&rx_promisc_pkt->list);
		rtw_list_insert_tail(&rx_promisc_pkt->list, &g_rx_promisc_q_priv.idle_q);
		rx_promisc_pkt->data_len = 0;
		rx_promisc_pkt->data_rate = RTW_RATE_UNKNOWN;
		rx_promisc_pkt->rssi = -110;
		g_rx_promisc_q_priv.idle_q_cnt++;

		rtos_mutex_give(g_rx_promisc_q_priv.rx_promisc_q_mutex);
	}
}

static struct rx_promisc_data *wifi_rx_promisc_dequeue_idle_q(void)
{
	struct rx_promisc_data	*rx_promisc_pkt = NULL;

	if (g_rx_promisc_q_priv.rx_promisc_q_mutex) {

		rtos_mutex_take(g_rx_promisc_q_priv.rx_promisc_q_mutex, MUTEX_WAIT_TIMEOUT);

		if (FALSE == rtw_is_list_empty(&g_rx_promisc_q_priv.idle_q)) {
			rx_promisc_pkt = container_of(get_next(&g_rx_promisc_q_priv.idle_q), struct rx_promisc_data, list);
			rtw_list_delete(&rx_promisc_pkt->list);
			g_rx_promisc_q_priv.idle_q_cnt--;
		} else if (FALSE == rtw_is_list_empty(&g_rx_promisc_q_priv.busy_q)) {
			/* drop oldest csi because of the variability of the channel */
			rx_promisc_pkt = container_of(get_next(&g_rx_promisc_q_priv.busy_q), struct rx_promisc_data, list);
			memset(rx_promisc_pkt->buffer, 0, RX_PROMISC_BUF_SIZE);
			rx_promisc_pkt->data_len = 0;
			rx_promisc_pkt->data_rate = RTW_RATE_UNKNOWN;
			rx_promisc_pkt->rssi = -110;
			rtw_list_delete(&rx_promisc_pkt->list);
			g_rx_promisc_q_priv.busy_q_cnt--;
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_WARN, "lack of rx_promisc buffer!\n");
		}

		rtos_mutex_give(g_rx_promisc_q_priv.rx_promisc_q_mutex);
	}

	return rx_promisc_pkt;
}
/* ------------------- Initialize & Operating promisc_rx_buffer_q END --------------------- */



/* ----------------------------- wifi_raw_rx START ------------------------------------- */
u8 wifi_rx_promisc_callback(struct rtw_rx_pkt_info *pkt_info)
{
	struct rx_promisc_data	*rx_promisc_pkt = NULL;

	if (!memcmp(&pkt_info->buf[4], addr1, ETH_ALEN)) { /* only show raw_tx pkt: check A1 match? */
		rx_promisc_pkt = wifi_rx_promisc_dequeue_idle_q();

		if (rx_promisc_pkt && (pkt_info->len <= RX_PROMISC_BUF_SIZE)) {
			memcpy(rx_promisc_pkt->buffer, pkt_info->buf, pkt_info->len);
			rx_promisc_pkt->data_len = pkt_info->len;
			rx_promisc_pkt->data_rate = pkt_info->data_rate;
			rx_promisc_pkt->rssi = pkt_info->recv_signal_power;
			wifi_rx_promisc_enqueue_busy_q(rx_promisc_pkt);
			rtos_sema_give(rx_promisc_ready_sema);
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_WARN, "promisc_callback fail(null_buf or smaller size[%ld])\n", pkt_info->len);
		}
	}
	return 0;
}

void wifi_raw_rx_thread(void *param)
{
	UNUSED(param);
	struct rtw_promisc_para promiscpara;
	struct rx_promisc_data	*rx_promisc_pkt = NULL;
	u8 subtype = 0;
	u32 i = 0, j = 0;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s start.\n", __func__);

	/* enable rx_promisc for check raw_tx data */
	memset(&promiscpara, 0, sizeof(struct rtw_promisc_para));
	promiscpara.filter_mode = RTW_PROMISC_FILTER_ALL_PKT;
	promiscpara.callback = wifi_rx_promisc_callback;
	wifi_promisc_enable(ENABLE, &promiscpara);

	/* init rx_promisc buffer_q */
	wifi_rx_promisc_buffer_init();

	/* should use semaphore to wait rx_promisc ready */
	rtos_sema_create(&rx_promisc_ready_sema, 0, 0xFFFFFFFF);
	if (!rx_promisc_ready_sema) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "rx_promisc_ready_sema failed\r\n");
		goto done;
	}

	while (1) {
		/* wait rx_promisc_ready_sema for handle promisc_rx */
		if (rtos_sema_take(rx_promisc_ready_sema, 0xFFFFFFFF) != RTK_SUCCESS) {
			rtos_sema_delete(rx_promisc_ready_sema);
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "get rx_promisc_ready_sema failed");

			wifi_promisc_enable(DISABLE, &promiscpara);
			break;
		}

		rx_promisc_pkt = wifi_rx_promisc_dequeue_busy_q(TRUE);
		if (rx_promisc_pkt != NULL) {

			/*do something for handing rx_promisc pkt: like show payload */
			subtype = WifiGetFrameSubType(rx_promisc_pkt->buffer);
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "Promisc RX [%s: rate = %d, rssi = %d dBm] ->\n",
					 (subtype == RTW_PROBEREQ) ? "probe_req" : "qos_data",
					 rx_promisc_pkt->data_rate, rx_promisc_pkt->rssi, rx_promisc_pkt->data_len);

			for (i = 0; i < 40; i++) {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "%02x ", rx_promisc_pkt->buffer[i]);
				if (++j % 16 == 0) {
					RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
				}
			}
			j = 0;
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\n");

			wifi_rx_promisc_enqueue_idle_q(rx_promisc_pkt);
			rx_promisc_pkt = NULL;
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_WARN, "deq rx_promisc_q fail\n");
		}
	}

done:
	/* free rx_promisc buffer_q */
	wifi_rx_promisc_buffer_deinit();

	if (rx_promisc_ready_sema) {
		rtos_sema_delete(rx_promisc_ready_sema);
	}

	rtos_task_delete(NULL);
}
/* ----------------------------- wifi_raw_rx END ------------------------------------- */


/* ----------------------------- wifi_raw_tx START ------------------------------------- */
static void wifi_raw_tx_construct_mgnt(u8 *pframe)
{
	/**
	 * The construction of management frames must strictly comply with WiFi protocol regulations.
	 * Management Frame Format: Frame Ctrl(2) + Duration(2) + Addr1(6) + Addr2(6) + Addr3(6) + Seq Ctrl(2) + Frame Body(variable)
	 * The example is a probereq frame in the following format,
	 * ->Frame Ctrl：must be 0040
	 * ->Duration：not required, hw maintain
	 * ->Addr：modify as needed
	 * ->Seq Ctrl：not required
	 * ->Frame Body：SSID + Supported Rates + Others
	*/
	unsigned char probe_req_buf[RAW_TX_MGNT_FRAME_LEN] = {
		0x40, 0x00, 												// Frame Control field
		0x00, 0x00, 												// Duration/ID field
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 						// Address 1, should be the mac address of destination
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 						// Address 2, should be the mac address of myself as master
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 						// Address 3, shoule be the BSSID
		0x00, 0x00, 												// Sequence Control field
		0x00, 0x00,													// SSID field
		0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x8c, 0x12, 0x98, 0x24,	// Supported Rates field
		0x32, 0x04, 0xb0, 0x48, 0x60, 0x6c							// Extended Supported Rates field
	};

	memcpy(&probe_req_buf[4], addr1, 6);
	memcpy(&probe_req_buf[10], addr2, 6);
	memcpy(&probe_req_buf[16], addr3, 6);

	memcpy(pframe, probe_req_buf, RAW_TX_MGNT_FRAME_LEN);
}

static void wifi_raw_tx_construct_data(u8 *pframe)
{
	/**
	 * The construction of Qos data frames must strictly comply with WiFi protocol regulations.
	 * Data Frame Format: Frame Ctrl(2) + Duration(2) + Addr1(6) + Addr2(6) + Addr3(6) + Seq Ctrl(2) + Qos Ctrl(2) + Frame Body(variable)
	 * ->Frame Ctrl：must be 0088
	 * ->Duration：not required, hw maintain
	 * ->Addr：modify as needed
	 * ->Seq Ctrl：not required(0~4095)
	 * ->Frame Body：any value
	*/
	unsigned char qos_data_buf[RAW_TX_DATA_FRAME_LEN] = {
		0x88, 0x00, 												// Frame Control field
		0x00, 0x00, 												// Duration/ID field
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 						// Address 1, should be the mac address of destination
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 						// Address 2, should be the mac address of myself as master
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 						// Address 3, shoule be the BSSID
		0x00, 0x00, 												// Sequence Control field
		0x00, 0x00,													// QoS Control filed
	};

	memcpy(&qos_data_buf[4], addr1, 6);
	memcpy(&qos_data_buf[10], addr2, 6);
	memcpy(&qos_data_buf[16], addr3, 6);

	memcpy(pframe, qos_data_buf, RAW_TX_DATA_FRAME_LEN);

	/* random payload */
	for (int i = 24; i < RAW_TX_DATA_FRAME_LEN; i++) {
		pframe[i] = (u8)_rand();
	}
}

void wifi_raw_tx_thread(void *param)
{
	UNUSED(param);
	struct rtw_raw_frame_desc tx_raw_data_desc = {0};
	u8 *data_frame = NULL;
	u8 *mgnt_frame = NULL;
	u8 tx_flag = 0;  /* mod2: 0-tx mgnt; 1-tx data */

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s start.\n", __func__);

	mgnt_frame = (u8 *)rtos_mem_zmalloc(RAW_TX_MGNT_FRAME_LEN);
	data_frame = (u8 *)rtos_mem_zmalloc(RAW_TX_DATA_FRAME_LEN);

	if (!mgnt_frame || !data_frame) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "raw_tx malloc fail\n");
		goto err;
	}

	while (1) {
		if (++tx_flag % 2) {
			wifi_raw_tx_construct_mgnt(mgnt_frame);
			tx_raw_data_desc.wlan_idx = STA_WLAN_INDEX;
			tx_raw_data_desc.buf = mgnt_frame;
			tx_raw_data_desc.buf_len = RAW_TX_MGNT_FRAME_LEN;
			tx_raw_data_desc.tx_rate = RTW_RATE_6M;
		} else {
			wifi_raw_tx_construct_data(data_frame);
			tx_raw_data_desc.wlan_idx = STA_WLAN_INDEX;
			tx_raw_data_desc.buf = data_frame;
			tx_raw_data_desc.buf_len = RAW_TX_DATA_FRAME_LEN;
			tx_raw_data_desc.tx_rate = RTW_RATE_54M;
		}

		if (wifi_send_raw_frame(&tx_raw_data_desc) == -RTK_ERR_WIFI_TX_BUF_FULL) {
			RTK_LOGS(NOTAG, RTK_LOG_WARN, "%s failed!\n", __func__);
			rtos_time_delay_ms(5);
		}
	}

err:
	if (data_frame) {
		rtos_mem_free(data_frame);
	}
	if (mgnt_frame) {
		rtos_mem_free(mgnt_frame);
	}

	rtos_task_delete(NULL);
}
/* ----------------------------- wifi_raw_tx END ------------------------------------- */

void wifi_raw_init_thread(void *param)
{
	UNUSED(param);

	while (wifi_is_running(STA_WLAN_INDEX) == FALSE) {
		rtos_time_delay_ms(1000);
	}

	wifi_set_channel(STA_WLAN_INDEX, WIFI_RAW_CH);

	if (rtos_task_create(NULL, ((const char *)"wifi_raw_tx_"), wifi_raw_tx_thread, NULL, 1024 * 4, 2) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r[%s] Create wifi raw tx task failed", __FUNCTION__);
		goto exit;
	}

	if (rtos_task_create(NULL, ((const char *)"wifi_raw_rx_"), wifi_raw_rx_thread, NULL, 1024 * 4, 4) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r[%s] Create wifi raw rx task failed", __FUNCTION__);
		goto exit;
	}

exit:
	rtos_task_delete(NULL);
}


void example_wifi_raw(void)
{
	if (rtos_task_create(NULL, ((const char *)"wifi_raw_init_"), wifi_raw_init_thread, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r[%s] Create wifi raw task failed", __FUNCTION__);
	}
}


