/**
  ******************************************************************************
  * @file    inic_host_api.c
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include "whc_host.h"


/* -------------------------------- spi --------------------------------- */

struct event_priv_t event_priv;
extern u16 scanned_ap_cnt;
extern u8 rtw_scan_api_inprocess;
extern struct internal_block_param *scan_block_param;
extern struct internal_block_param *scan_abort_block_param;
extern void eap_autoreconnect_hdl(u8 method_id);
extern s32(*scan_user_callback_ptr)(u32, void *);
extern s32(*scan_each_report_user_callback_ptr)(struct rtw_scan_result *, void *);
extern void (*p_ap_channel_switch_callback)(unsigned char channel, s8 ret);
extern u8(*promisc_user_callback_ptr)(void *);
extern int dhcps_ip_in_table_check(uint8_t gate, uint8_t d);


const struct event_func_t host_api_handlers[] = {
	{WHC_API_SCAN_USER_CALLBACK,	whc_host_api_scan_user_callback_handler},
	{WHC_API_SCAN_EACH_REPORT_USER_CALLBACK,	whc_host_api_scan_each_report_callback_handler},
	{WHC_API_HDL,	whc_host_api_wifi_event_handler},
	{WHC_API_GET_LWIP_INFO,	whc_host_api_lwip_info_handler},
	{WHC_API_SET_NETIF_INFO,	whc_host_api_set_netif_info_handler},
	{WHC_API_IP_TABLE_CHK, whc_host_api_ip_table_chk},
	{WHC_API_AP_CH_SWITCH, whc_host_api_ap_ch_switch},
};

void whc_host_api_scan_user_callback_handler(u32 api_id, u32 *param_buf)
{
	int ret = 0;

	unsigned int ap_num = param_buf[0];
	void *user_data = (void *)param_buf[1];

	if (scan_user_callback_ptr) {
		scan_user_callback_ptr(ap_num, user_data);
		scan_user_callback_ptr = NULL;
	}

	if (scan_each_report_user_callback_ptr) {
		scan_each_report_user_callback_ptr(NULL, user_data);
		scan_user_callback_ptr = NULL;
	}

	if (scan_acs_report_user_callback_ptr) {
		scan_acs_report_user_callback_ptr = NULL;
	}

	/* if Synchronous scan, up sema when scan done */
	if (scan_block_param) {
		scanned_ap_cnt = ap_num;
		rtos_sema_give(scan_block_param->sema);
	}

	/* Clear state after callback excuted and/or block sema released, prevent new scan to overwrite these paras */
	rtw_scan_api_inprocess = 0;

	/* if Synchronous scan abort, up sema when scan done */
	if (scan_abort_block_param) {
		rtos_sema_give(scan_abort_block_param->sema);
	}

	whc_host_api_send_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_host_api_scan_each_report_callback_handler(u32 api_id, u32 *param_buf)
{
	int ret = 0;
	struct rtw_scan_result *scanned_ap_info = (struct rtw_scan_result *)param_buf[0];
	void *user_data = (void *)param_buf[1];

	if (scan_each_report_user_callback_ptr) {
		scan_each_report_user_callback_ptr(scanned_ap_info, user_data);
	}

	whc_host_api_send_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_host_api_wifi_event_handler(u32 api_id, u32 *param_buf)
{
	int ret = 0;
	u32 event = (u32)param_buf[0];
	s32 flags = (s32)param_buf[1];
	s32 buf_len = (s32)param_buf[2];
	u8 *buf = (u8 *)(&param_buf[3]);

	wifi_indication(event, buf, buf_len, flags);

	whc_host_api_send_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_host_api_lwip_info_handler(u32 api_id, u32 *param_buf)
{
	int ret;
	u32 addr = 0;
	u8 buf[DEV_REQ_NETWORK_INFO_MAX_LEN] = {0};

	u32 type = (u32)param_buf[0];
	u32 index = (u32)param_buf[1];
	int res_size = 4;
	unsigned char *input = (unsigned char *)param_buf[2];
	switch (type) {
	case WHC_WLAN_GET_IP:
		addr = (u32)LwIP_GetIP(index);
		break;
	case WHC_WLAN_GET_GW:
		addr = (u32)LwIP_GetGW(index);
		break;
	case WHC_WLAN_GET_GWMSK:
		addr = (u32)LwIP_GetMASK(index);
		break;
	case WHC_WLAN_GET_HW_ADDR:
		addr = (u32)LwIP_GetMAC(index);
		res_size = 6;
		break;
	case WHC_WLAN_IS_VALID_IP:
		ret = LwIP_netif_is_valid_IP(index, input);
		break;
	}
	if (addr != 0) {
		memcpy((void *)buf, (void *)addr, res_size);
		whc_host_api_send_ret_value(api_id, (u8 *)buf, res_size);
	} else {
		whc_host_api_send_ret_value(api_id, (u8 *)(&ret), sizeof(ret));
	}
}

void whc_host_api_set_netif_info_handler(u32 api_id, u32 *param_buf)
{
	int ret = 0;

	int idx = (u32)param_buf[0];
	unsigned char *dev_addr = (unsigned char *)(&param_buf[1]);

	LwIP_wlan_set_netif_info(idx, NULL, dev_addr);
	whc_host_api_send_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_host_api_ip_table_chk(u32 api_id, u32 *param_buf)
{
	int ret = 0;

	u8 gate = (u8)param_buf[0];
	u8 ip = (u8)param_buf[1];

	ret = dhcps_ip_in_table_check(gate, ip);
	whc_host_api_send_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_host_api_ap_ch_switch(u32 api_id, u32 *param_buf)
{
	int ret = 0;
	unsigned char channel = (unsigned char)param_buf[0];
	s8 res = (s8)param_buf[1];

	if (p_ap_channel_switch_callback) {
		p_ap_channel_switch_callback(channel, res);
	}

	whc_host_api_send_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

int whc_host_api_iwpriv_command(char *cmd, unsigned int cmd_len, int show_msg)
{
	int ret = 0;
	u32 *param_buf;
	u8 len = 8 + cmd_len + 1;

	param_buf = (u32 *)rtos_mem_malloc(len);
	if (!param_buf) {
		return -1;
	}
	memset(param_buf, '\0', len);
	param_buf[0] = (u32)show_msg;
	param_buf[1] = (u32)cmd_len;

	memcpy((void *)&param_buf[2], (void *)cmd, cmd_len);

	whc_host_api_message_send(WHC_API_WIFI_IWPRIV_INFO, (u8 *)param_buf, len, (u8 *)&ret, sizeof(ret));

	rtos_mem_free(param_buf);
	return ret;
}

void whc_host_api_message_send(u32 id, u8 *param, u32 param_len, u8 *ret, u32 ret_len)
{
	u8 *buf = NULL;
	struct whc_api_info *info;
	struct whc_api_info *ret_msg;
	struct whc_txbuf_info_t *inic_tx;

	RTK_LOGD(TAG_WLAN_INIC, "Host Call API %d %x \n", id, __builtin_return_address(0));

	rtos_sema_take(event_priv.send_mutex, MUTEX_WAIT_TIMEOUT);
	//TODO SDIO need extra TXDESC size
	buf = rtos_mem_zmalloc(sizeof(struct whc_api_info) + param_len + DEV_DMA_ALIGN);
	if (!buf) {
		goto exit;
	}

	info = (struct whc_api_info *)N_BYTE_ALIGMENT((u32)buf, DEV_DMA_ALIGN);
	info->event = WHC_WIFI_EVT_API_CALL;
	info->api_id = id;
	if (param_len > 0) {
		memcpy((void *)(info + 1), param, param_len);
	}

	/* construct struct whc_buf_info & whc_buf_info_t */
	inic_tx = (struct whc_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));
	if (!inic_tx) {
		goto exit;
	}

	inic_tx->txbuf_info.buf_allocated = inic_tx->txbuf_info.buf_addr = (u32)info;
	inic_tx->txbuf_info.size_allocated = inic_tx->txbuf_info.buf_size = sizeof(struct whc_api_info) + param_len;

	inic_tx->ptr = buf;
	inic_tx->is_skb = 0;

	/* send ret_msg + ret_val(buf, len) */
	whc_host_send_data(&inic_tx->txbuf_info);

	/* wait for API calling done */
	event_priv.b_waiting_for_ret = 1;
	rtos_sema_take(event_priv.api_ret_sema, 0xFFFFFFFF);
	event_priv.b_waiting_for_ret = 0;

	RTK_LOGD(TAG_WLAN_INIC, "Device API %ld return\n", id);

	/* get return value */
	ret_msg = (struct whc_api_info *)event_priv.rx_ret_msg;
	event_priv.rx_ret_msg = NULL;
	if (ret_msg != NULL) {
		/* check api_id of return msg */
		if (ret_msg->api_id != id) {
			RTK_LOGE(TAG_WLAN_INIC, "Linux API return value id not match!\n");
		}

		/* copy return value*/
		if (ret != NULL && ret_len != 0) {
			memcpy(ret, (u8 *)(ret_msg + 1), ret_len);
		}

		/* free rx buffer */
		rtos_mem_free((u8 *)ret_msg);
	} else {
		/* free rx buffer */
		rtos_mem_free((u8 *)ret_msg);
		RTK_LOGE(TAG_WLAN_INIC, "Linux API return value is NULL!\n");
	}

	//TODO  SDIO need buf free after send
	rtos_sema_give(event_priv.send_mutex);
	return;

exit:
	if (buf) {
		rtos_mem_free(buf);
	}
	if (inic_tx) {
		rtos_mem_free((u8 *)inic_tx);
	}

	rtos_sema_give(event_priv.send_mutex);
	return;

}


void whc_host_api_send_ret_value(u32 api_id, u8 *pbuf, u32 len)
{
	u8 *buf = NULL;
	struct whc_api_info *ret_msg;
	struct whc_txbuf_info_t *inic_tx;

	buf = rtos_mem_zmalloc(sizeof(struct whc_api_info) + len + DEV_DMA_ALIGN);
	if (!buf) {
		goto exit;
	}
	ret_msg = (struct whc_api_info *)N_BYTE_ALIGMENT((u32)buf, DEV_DMA_ALIGN);

	/* notify NP that event is finished */
	ret_msg->event = WHC_WIFI_EVT_API_RETURN;
	ret_msg->api_id = api_id;

	memcpy((void *)(ret_msg + 1), pbuf, len);

	/* construct struct whc_buf_info & whc_buf_info_t */
	inic_tx = (struct whc_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));
	if (!inic_tx) {
		goto exit;
	}

	inic_tx->txbuf_info.buf_allocated = inic_tx->txbuf_info.buf_addr = (u32)ret_msg;
	inic_tx->txbuf_info.size_allocated = inic_tx->txbuf_info.buf_size = sizeof(struct whc_api_info) + len;

	inic_tx->ptr = buf;
	inic_tx->is_skb = 0;

	/* send ret_msg + ret_val(pbuf, len) */
	whc_host_send_data(&inic_tx->txbuf_info);

	RTK_LOGD(TAG_WLAN_INIC, "Host API %x return\n", api_id);

	return;

exit:
	if (buf) {
		rtos_mem_free(buf);
	}
	if (inic_tx) {
		rtos_mem_free((u8 *)inic_tx);
	}
	return;

}

/**
 * @brief  handle the inic message.
 * @param  none.
 * @return none.
 */
void whc_host_api_task(void)
{
	struct whc_api_info *p_recv_msg;
	u32 *param_buf;
	void (*api_hdl)(u32 api_id, u32 * param_buf);
	u32 i = 0;

	do {
		rtos_sema_take(event_priv.task_wake_sema, 0xFFFFFFFF);

		p_recv_msg = (struct whc_api_info *)event_priv.rx_api_msg;
		event_priv.rx_api_msg = NULL;
		param_buf = (u32 *)(p_recv_msg + 1);

		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_DEBUG, "DEV CALL API(%x)\n", p_recv_msg->api_id);

		api_hdl = NULL;
		for (i = 0; i < sizeof(host_api_handlers) / sizeof(struct event_func_t); i++) {
			if (host_api_handlers[i].api_id == p_recv_msg->api_id) {
				api_hdl = host_api_handlers[i].func;
				break;
			}
		}

		if (api_hdl != NULL) {
			api_hdl(p_recv_msg->api_id, param_buf);
		} else {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Host Unknown API(%x)\n", p_recv_msg->api_id);
		}

		RTK_LOGD(TAG_WLAN_INIC, "Host CALL API(%x) done\n", p_recv_msg->api_id);

		/* free rx_api_msg */
		rtos_mem_free((u8 *)p_recv_msg);
	} while (1);
}