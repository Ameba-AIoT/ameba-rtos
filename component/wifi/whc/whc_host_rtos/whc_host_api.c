/**
  ******************************************************************************
  * @file    whc_host_api.c
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


/* ------------------------------- Variables -------------------------------- */

struct event_priv_t event_priv;
extern u16 scanned_ap_cnt;
extern u8 rtw_scan_api_inprocess;
extern struct internal_block_param *scan_block_param;
extern struct internal_block_param *scan_abort_block_param;
extern s32(*scan_user_callback_ptr)(u32, void *);
extern s32(*scan_each_report_user_callback_ptr)(struct rtw_scan_result *, void *, u8 *, u32);
extern void (*p_ap_channel_switch_callback)(unsigned char channel, s8 ret);
extern u8(*promisc_user_callback_ptr)(void *);
extern int dhcps_ip_in_table_check(struct netif *pnetif, uint8_t gate, uint8_t d);
extern s32(*scan_acs_report_user_callback_ptr)(struct rtw_acs_mntr_rpt *acs_mntr_rpt);
extern int whc_host_init_done;


const struct event_func_t host_api_handlers[] = {
	{WHC_API_SCAN_USER_CALLBACK,	whc_host_api_scan_user_callback_handler},
	{WHC_API_SCAN_EACH_REPORT_USER_CALLBACK,	whc_host_api_scan_each_report_callback_handler},
	{WHC_API_WIFI_EVENT,	whc_host_api_wifi_event_handler},
	{WHC_API_SET_NETIF_INFO,	whc_host_api_set_netif_info_handler},
	{WHC_API_AP_CH_SWITCH, whc_host_api_ap_ch_switch},
};

void whc_host_api_scan_user_callback_handler(u32 api_id, u32 *param_buf)
{
	unsigned int ap_num = param_buf[0];
	void *user_data = (void *)param_buf[1];

	if (scan_user_callback_ptr) {
		scan_user_callback_ptr(ap_num, user_data);
		scan_user_callback_ptr = NULL;
	}

	if (scan_each_report_user_callback_ptr) {
		scan_each_report_user_callback_ptr(NULL, user_data, NULL, 0);
		scan_each_report_user_callback_ptr = NULL;
	}

	if (scan_acs_report_user_callback_ptr) {
		scan_acs_report_user_callback_ptr = NULL;
	}

	/* if Synchronous scan, up sema when scan done */
	if (scan_block_param) {
		scanned_ap_cnt = ap_num;
		rtos_sema_give(scan_block_param->sema);
	}

	/* Clear state after callback executed and/or block sema released, prevent new scan to overwrite these paras */
	rtw_scan_api_inprocess = 0;

	/* if Synchronous scan abort, up sema when scan done */
	if (scan_abort_block_param) {
		rtos_sema_give(scan_abort_block_param->sema);
	}
}

void whc_host_api_scan_each_report_callback_handler(u32 api_id, u32 *param_buf)
{
	struct rtw_scan_result *scanned_ap_info = (struct rtw_scan_result *)param_buf[0];
	void *user_data = (void *)param_buf[1];
	u8 *ies = (u8 *)param_buf[2];
	u32 ie_len = param_buf[3];

	if (scan_each_report_user_callback_ptr) {
		scan_each_report_user_callback_ptr(scanned_ap_info, user_data, ies, ie_len);
	}
}

void whc_host_api_wifi_event_handler(u32 api_id, u32 *param_buf)
{
	u32 event = (u32)param_buf[0];
	s32 evt_len = (s32)param_buf[1];
	u8 *evt_info = (u8 *)(&param_buf[2]);

	wifi_indication(event, evt_info, evt_len);
}

void whc_host_api_set_netif_info_handler(u32 api_id, u32 *param_buf)
{
	int idx = (u32)param_buf[0];
	unsigned char *dev_addr = (unsigned char *)(&param_buf[1]);

	lwip_wlan_set_netif_info(idx, NULL, dev_addr);
}

void whc_host_api_ap_ch_switch(u32 api_id, u32 *param_buf)
{
	unsigned char channel = (unsigned char)param_buf[0];
	s8 res = (s8)param_buf[1];

	if (p_ap_channel_switch_callback) {
		p_ap_channel_switch_callback(channel, res);
	}
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

	if (!whc_host_init_done) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Host api err: wifi not init\n");
		return;
	}

	RTK_LOGD(TAG_WLAN_INIC, "Host Call API %d %x \n", id, __builtin_return_address(0));

	rtos_mutex_take(event_priv.send_mutex, MUTEX_WAIT_TIMEOUT);
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

	whc_host_send_data((u8 *)info, sizeof(struct whc_api_info) + param_len, buf, 0);

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
		RTK_LOGE(TAG_WLAN_INIC, "Linux API return value is NULL!\n");
	}

#ifdef CONFIG_WHC_INTF_SDIO
	rtos_mem_free(buf);
#endif

exit:
	rtos_mutex_give(event_priv.send_mutex);
	return;
}

void whc_host_api_init(void)
{
	rtos_sema_create(&(event_priv.task_wake_sema), 0, 0xFFFFFFFF);
	rtos_sema_create(&(event_priv.api_ret_sema), 0, 0xFFFFFFFF);
	rtos_mutex_create(&(event_priv.send_mutex));

	if (RTK_SUCCESS != rtos_task_create(NULL, (const char *const)"whc_host_api_task", (rtos_task_function_t)whc_host_api_task, NULL,
										g_rtw_task_size.whc_hst_api_task, 3)) {
		RTK_LOGE(TAG_WLAN_INIC, "Create api_host_task Err\n");
	}
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