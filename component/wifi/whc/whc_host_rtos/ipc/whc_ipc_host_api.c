/**
  ******************************************************************************
  * @file    whc_ipc_host_api.c
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
#include "rtw_inic_common.h"
#include "whc_ipc_host_nan.h"
#ifndef ZEPHYR_WIFI
#include "lwip_netconf.h"
#ifdef CONFIG_WIFI_TUNNEL
#include "mbedtls/cmac.h"
#include "wtn_app_rnat.h"
#ifdef CONFIG_WTN_SOCKET_APP
#include "wtn_app_socket.h"
#endif
#endif
#endif
/* -------------------------------- Defines --------------------------------- */


/* ---------------------------- Global Variables ---------------------------- */
rtos_sema_t  g_host_inic_api_task_wake_sema = NULL;
rtos_mutex_t g_host_inic_api_message_send_lock = NULL;

//todo:move to non-cache data section
struct whc_ipc_host_req_msg g_host_ipc_api_request_info __attribute__((aligned(RTOS_MEM_BYTE_ALIGNMENT)));
u32	latest_api_id = 0;  /*for debug*/
#ifdef IPC_DIR_MSG_TX
IPC_MSG_STRUCT g_host_ipc_api_msg __attribute__((aligned(RTOS_MEM_BYTE_ALIGNMENT)));
#endif

struct whc_ipc_host_unblk_api_q_priv g_host_unblk_api_q_priv __attribute__((aligned(RTOS_MEM_BYTE_ALIGNMENT)));

extern u16 scanned_ap_cnt;
extern u8 rtw_scan_api_inprocess;
extern struct internal_block_param *scan_block_param;
extern struct internal_block_param *scan_abort_block_param;
extern struct host_priv g_inic_host_priv;
/* -------------------------- Function declaration -------------------------- */
extern s32(*scan_user_callback_ptr)(u32, void *);
extern s32(*scan_each_report_user_callback_ptr)(struct rtw_scan_result *, void *, u8 *, u32);
extern s32(*scan_acs_report_user_callback_ptr)(struct rtw_acs_mntr_rpt *acs_mntr_rpt);
extern void (*p_ap_channel_switch_callback)(u8 channel, s8 ret);

extern u8(*promisc_user_callback_ptr)(void *);
#if defined (CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
extern int dhcps_ip_in_table_check(struct netif *pnetif, uint8_t gate, uint8_t d);
#else
extern int dhcps_ip_in_table_check(uint8_t gate, uint8_t d);
#endif
#ifdef CONFIG_WIFI_TUNNEL
extern int(*rmesh_ota_user_callback)(u8 *, u16);
#endif
/* ---------------------------- Private Functions --------------------------- */
/* wifi callback & blocked in NP */
static void whc_ipc_host_api_scan_user_callback_handler(struct whc_ipc_host_unblk_api_node *p_unblk_api_node)
{
	unsigned int ap_num = p_unblk_api_node->param_buf[0];
	void *user_data = (void *)p_unblk_api_node->param_buf[1];

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

static void whc_ipc_host_api_scan_each_report_callback_handler(struct whc_ipc_host_unblk_api_node *p_unblk_api_node)
{
	struct rtw_scan_result *scanned_ap_info = (struct rtw_scan_result *)p_unblk_api_node->param_buf[0];
	void *user_data = (void *)p_unblk_api_node->param_buf[1];
	u8 *ies = (u8 *)p_unblk_api_node->param_buf[2];
	u32 ie_len = p_unblk_api_node->param_buf[3];

	if (scan_each_report_user_callback_ptr) {
		scan_each_report_user_callback_ptr(scanned_ap_info, user_data, ies, ie_len);
	}
}

/* wifi callback & api func with return values */
static void whc_ipc_host_api_lwip_info_handler(struct whc_ipc_dev_req_msg *p_ipc_msg)
{
#if !defined(CONFIG_MP_SHRINK) && defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	u32 type = (u32)p_ipc_msg->param_buf[0];
	unsigned char *input = (unsigned char *)p_ipc_msg->param_buf[1];
	int idx = p_ipc_msg->param_buf[2];

	switch (type) {
	case WHC_WLAN_GET_IP:
		p_ipc_msg->ret = (u32)lwip_get_ip(idx);
		DCache_Clean(p_ipc_msg->ret, 4);
		break;
	case WHC_WLAN_GET_GW:
		p_ipc_msg->ret = (u32)lwip_get_gw(idx);
		DCache_Clean(p_ipc_msg->ret, 4);
		break;
	case WHC_WLAN_GET_GWMSK:
		p_ipc_msg->ret = (u32)lwip_get_mask(idx);
		DCache_Clean(p_ipc_msg->ret, 4);
		break;
	case WHC_WLAN_GET_HW_ADDR:
		p_ipc_msg->ret = (u32)lwip_get_mac(idx);
		DCache_Clean(p_ipc_msg->ret, 6);
		break;
	case WHC_WLAN_IS_VALID_IP:
		DCache_Invalidate((u32)input, 4);
		p_ipc_msg->ret = lwip_is_valid_ip(idx, input);
		break;
	case WHC_WLAN_GET_IPV6_ENABLED:
		p_ipc_msg->ret = lwip_get_ipv6_enabled();
		break;
	}
#else
	(void)p_ipc_msg;
#endif
}

#if !defined(CONFIG_PLATFORM_ZEPHYR)
static u32 whc_ipc_host_api_addr_update_in_wowlan(u32 expected_idle_time, void *param)
{
	/* To avoid gcc warnings */
	(void) expected_idle_time;
	(void) param;
#ifndef CONFIG_MP_SHRINK
	static u8 inic_ipc_old_ip_addr[4] = {0};

	u32 try_cnt = 2500;//wait 5ms
	u8 i;

	u8 *new_addr = lwip_get_ip(NETIF_WLAN_STA_INDEX);
	for (i = 0; i < 4; i++) {
		if (inic_ipc_old_ip_addr[i] != new_addr[i]) {
			goto send;
		}
	}

	return TRUE;

send:
	while (try_cnt) {
		DCache_Invalidate((u32)&g_host_ipc_api_request_info, sizeof(struct whc_ipc_host_req_msg));
		if (g_host_ipc_api_request_info.api_id != WHC_API_PROCESS_DONE) {
			try_cnt --;
			DelayUs(2);
		} else {
			break;
		}
	}

	if (try_cnt == 0) {
		RTK_LOGE(TAG_WLAN_INIC, "update ip addr TO, last inic ipc not hdl\n");
		return FALSE;
	} else {
		try_cnt = 2500;
	}

	memset(&g_host_ipc_api_request_info, 0, sizeof(struct whc_ipc_host_req_msg));

	g_host_ipc_api_request_info.api_id = WHC_API_WIFI_IP_UPDATE;
	g_host_ipc_api_request_info.param_buf[0] = (u32)lwip_get_ip(NETIF_WLAN_STA_INDEX);
	g_host_ipc_api_request_info.param_buf[1] = (u32)lwip_get_gw(NETIF_WLAN_STA_INDEX);
	DCache_Clean(g_host_ipc_api_request_info.param_buf[0], 4);
	DCache_Clean(g_host_ipc_api_request_info.param_buf[1], 4);

	DCache_Clean((u32)&g_host_ipc_api_request_info, sizeof(struct whc_ipc_host_req_msg));

	memset(&g_host_ipc_api_msg, 0, sizeof(IPC_MSG_STRUCT));
	g_host_ipc_api_msg.msg = (u32)&g_host_ipc_api_request_info;
	g_host_ipc_api_msg.msg_type = IPC_USER_POINT;
	g_host_ipc_api_msg.msg_len = sizeof(struct whc_ipc_host_req_msg);
	DCache_Clean((u32)&g_host_ipc_api_msg, sizeof(IPC_MSG_STRUCT));
	ipc_send_message(IPC_DIR_MSG_TX, IPC_H2D_WIFI_API_TRAN, \
					 &g_host_ipc_api_msg);

	while (try_cnt) {
		DCache_Invalidate((u32)&g_host_ipc_api_request_info, sizeof(struct whc_ipc_host_req_msg));
		if (g_host_ipc_api_request_info.api_id != WHC_API_PROCESS_DONE) {
			try_cnt --;
			DelayUs(2);
		} else {
			break;
		}
	}
	if (try_cnt == 0) {
		/* jira: https://jira.realtek.com/browse/RSWLANQC-1036 */
		RTK_LOGE(TAG_WLAN_INIC, "update ip addr TO, Driver busy\n");
		g_host_ipc_api_request_info.api_id = WHC_API_WIFI_MSG_TO;
		DCache_Clean((u32)&g_host_ipc_api_request_info, sizeof(struct whc_ipc_host_req_msg));
		return FALSE;
	}
	/* only update old when success */
	memcpy(inic_ipc_old_ip_addr, new_addr, 4);
#endif
	return TRUE;
}
#endif

static u8 whc_ipc_host_api_promisc_user_callback_handler(struct whc_ipc_dev_req_msg *p_ipc_msg)
{
	struct rtw_rx_pkt_info *ppktinfo = (struct rtw_rx_pkt_info *)p_ipc_msg->param_buf[0];
	u8 ret = RTW_PROMISC_NEED_DRV_HDL;

	if (promisc_user_callback_ptr) {
		/* invalidate will be safe if callback read mem only */
		DCache_Invalidate((u32)ppktinfo, sizeof(struct rtw_rx_pkt_info));
		DCache_Invalidate((u32)ppktinfo->buf, (u32)ppktinfo->len);
		ret = promisc_user_callback_ptr((void *)p_ipc_msg->param_buf[0]);
	}
	return ret;
}

/* wifi event & api func without return values */
static void whc_ipc_host_api_acs_report_callback_handler(struct whc_ipc_host_unblk_api_node *p_unblk_api_node)
{
	struct rtw_acs_mntr_rpt *acs_rpt = (struct rtw_acs_mntr_rpt *)p_unblk_api_node->param_buf[0];

	if (scan_acs_report_user_callback_ptr) {
		scan_acs_report_user_callback_ptr(acs_rpt);
	}
}

static void whc_ipc_host_api_ap_ch_switch_handler(struct whc_ipc_host_unblk_api_node *p_unblk_api_node)
{
	unsigned char channel = (unsigned char)p_unblk_api_node->param_buf[0];
	s8 res = (s8)p_unblk_api_node->param_buf[1];

	if (p_ap_channel_switch_callback) {
		p_ap_channel_switch_callback(channel, res);
	}
}

static void whc_ipc_host_api_wifi_event_handler(struct whc_ipc_host_unblk_api_node *p_unblk_api_node)
{
	u32 event = (u32)p_unblk_api_node->param_buf[0];
	u8 *evt_info = (u8 *)p_unblk_api_node->param_buf[1];

	wifi_event_handle(event, evt_info);
}

static void whc_ipc_host_api_set_netif_info_handler(struct whc_ipc_host_unblk_api_node *p_unblk_api_node)
{
#if !defined(CONFIG_MP_SHRINK) && defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	int idx = (u32)p_unblk_api_node->param_buf[0];
	unsigned char *dev_addr = (unsigned char *)p_unblk_api_node->param_buf[1];

	lwip_wlan_set_netif_info(idx, NULL, dev_addr);
#else
	(void)p_unblk_api_node;
#endif
}

#ifdef CONFIG_WIFI_TUNNEL
#if defined(CONFIG_WTN_SOCKET_APP)
static void whc_ipc_host_api_wtn_socket_init(struct whc_ipc_host_unblk_api_node *p_unblk_api_node)
{
	wtn_socket_init((u8)p_unblk_api_node->param_buf[0], (u8)p_unblk_api_node->param_buf[1]);
}

static void whc_ipc_host_api_wtn_socket_send(struct whc_ipc_dev_req_msg *p_ipc_msg)
{
	u8 *buf = (u8 *)p_ipc_msg->param_buf[0];
	u32 len = p_ipc_msg->param_buf[1];
	DCache_Invalidate((u32)buf, len);
	wtn_socket_send(buf, len);
}
#endif

static void whc_ipc_host_api_wtn_rnat_ap_init(struct whc_ipc_host_unblk_api_node *p_unblk_api_node)
{
	wtn_rnat_ap_init((u8)p_unblk_api_node->param_buf[0]);
}

static int whc_ipc_host_api_wtn_ota_callback_handler(struct whc_ipc_dev_req_msg *p_ipc_msg)
{
	u8 *buf = (u8 *)p_ipc_msg->param_buf[0];
	u16 len = (u16)p_ipc_msg->param_buf[1];
	int ret = RTK_FAIL;

	if (rmesh_ota_user_callback) {
		/* invalidate will be safe if callback read mem only */
		DCache_Invalidate((u32)buf, (u32)len);
		ret = rmesh_ota_user_callback(buf, len);
	}

	return ret;
}
#endif

static u8 *whc_ipc_host_api_unblk_api_copy(u8 *src_buf, u32 buf_len)
{
	u8 *dest_buf = rtos_mem_zmalloc(buf_len);
	if (!dest_buf) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Malloc Evt Buf Failed!\n");
		return NULL;
	}
	DCache_Invalidate((u32)src_buf, buf_len);
	memcpy(dest_buf, src_buf, buf_len);

	return dest_buf;
}

static struct whc_ipc_host_unblk_api_node *whc_ipc_host_api_unblk_api_msg_to_node(u32 enevt_id, u32 *param_buf)
{
	struct whc_ipc_host_unblk_api_node *p_unblk_api_node = NULL;
	void *buffer = NULL;
	u32 buffer_len = 0;

	p_unblk_api_node = (struct whc_ipc_host_unblk_api_node *)rtos_mem_zmalloc(sizeof(struct whc_ipc_host_unblk_api_node));
	if (p_unblk_api_node == NULL) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Malloc p_unblk_api_node Failed!\n");
		return NULL;
	}

	p_unblk_api_node->event_id = enevt_id;

	switch (p_unblk_api_node->event_id) {
	case WHC_API_SCAN_USER_CALLBACK:
		p_unblk_api_node->param_buf[0] = param_buf[0];
		p_unblk_api_node->param_buf[1] = param_buf[1];
		break;
	case WHC_API_SCAN_EACH_REPORT_USER_CALLBACK:
		/* 2 allocated buffers should be freed by host*/
		buffer = whc_ipc_host_api_unblk_api_copy((u8 *)param_buf[0], sizeof(struct rtw_scan_result));
		p_unblk_api_node->param_buf[0] = (u32)buffer;
		p_unblk_api_node->param_buf[1] = param_buf[1];

		buffer = whc_ipc_host_api_unblk_api_copy((u8 *)param_buf[2], param_buf[3]);
		p_unblk_api_node->param_buf[2] = (u32)buffer;
		p_unblk_api_node->param_buf[3] = param_buf[3];
		break;
	case WHC_API_IP_ACS:
		buffer_len = sizeof(struct rtw_acs_mntr_rpt);
		buffer = whc_ipc_host_api_unblk_api_copy((u8 *)param_buf[0], buffer_len);
		p_unblk_api_node->param_buf[0] = (u32)buffer;
		break;
	case WHC_API_AP_CH_SWITCH:
		p_unblk_api_node->param_buf[0] = param_buf[0];
		p_unblk_api_node->param_buf[1] = param_buf[1];
		break;
	case WHC_API_WIFI_EVENT:
		p_unblk_api_node->param_buf[0] = param_buf[0];
		if (param_buf[2]) {
			buffer_len = param_buf[2];
			buffer = whc_ipc_host_api_unblk_api_copy((u8 *)param_buf[1], buffer_len);
			p_unblk_api_node->param_buf[1] = (u32)buffer;
		}
		p_unblk_api_node->param_buf[2] = param_buf[2];
		break;
	case WHC_API_SET_NETIF_INFO:
		p_unblk_api_node->param_buf[0] = param_buf[0];
		buffer_len = ETH_ALEN;
		buffer = whc_ipc_host_api_unblk_api_copy((u8 *)param_buf[1], buffer_len);
		p_unblk_api_node->param_buf[1] = (u32)buffer;
		break;
	case WHC_API_WTN_SOCKET_INIT:
		p_unblk_api_node->param_buf[0] = param_buf[0];
		p_unblk_api_node->param_buf[1] = param_buf[1];
		break;
	case WHC_API_WTN_RNAT_AP_INIT:
		p_unblk_api_node->param_buf[0] = param_buf[0];

		break;
#ifdef CONFIG_WIFI_NAN_HOST_APP
	case WHC_API_CFG80211_NAN_CFGVENDOR_EVENT:
		/* {event_id, event_addr, len} -> copy payload into host-owned buf */
		p_unblk_api_node->param_buf[0] = param_buf[0];
		if (param_buf[2]) {
			buffer_len = param_buf[2];
			buffer = whc_ipc_host_api_unblk_api_copy((u8 *)param_buf[1], buffer_len);
			p_unblk_api_node->param_buf[1] = (u32)buffer;
		}
		p_unblk_api_node->param_buf[2] = param_buf[2];
		break;
	case WHC_API_CFG80211_NAN_CFGVENDOR_CMD_REPLY:
		/* {data_addr, len} -> copy reply into host-owned buf */
		if (param_buf[1]) {
			buffer_len = param_buf[1];
			buffer = whc_ipc_host_api_unblk_api_copy((u8 *)param_buf[0], buffer_len);
			p_unblk_api_node->param_buf[0] = (u32)buffer;
		}
		p_unblk_api_node->param_buf[1] = param_buf[1];
		break;
#endif
	default:
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Enq Host Event Unknown ID(%x)\n", p_unblk_api_node->event_id);
		break;
	}
	return p_unblk_api_node;
}

static void whc_ipc_host_api_unblk_api_enqueue(struct whc_ipc_host_unblk_api_node *p_unblk_api_node)
{
	if (p_unblk_api_node) {
		rtos_mutex_take(g_host_unblk_api_q_priv.host_unblk_api_q_mutex, MUTEX_WAIT_TIMEOUT);
		rtw_list_insert_tail(&p_unblk_api_node->list, &g_host_unblk_api_q_priv.host_unblk_api_queue);
		g_host_unblk_api_q_priv.host_unblk_api_node_cnt++;
		if ((g_host_unblk_api_q_priv.host_unblk_api_node_cnt % CONFIG_INIC_IPC_HOST_EVT_Q_DEPTH == 0) &&
			(g_host_unblk_api_q_priv.host_unblk_api_node_cnt / CONFIG_INIC_IPC_HOST_EVT_Q_DEPTH != 0)) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "%d nodes in Evt_q\n", g_host_unblk_api_q_priv.host_unblk_api_node_cnt);
		}
		rtos_mutex_give(g_host_unblk_api_q_priv.host_unblk_api_q_mutex);

		rtos_sema_give(g_host_unblk_api_q_priv.host_unblk_api_q_sema);
	}
}

void whc_ipc_host_wifi_indication_enqueue(u32 event, u8 *evt_info, s32 evt_len)
{
	struct whc_ipc_host_unblk_api_node *p_unblk_api_node = NULL;
	void *buffer = NULL;

	p_unblk_api_node = (struct whc_ipc_host_unblk_api_node *)rtos_mem_zmalloc(sizeof(struct whc_ipc_host_unblk_api_node));
	if (p_unblk_api_node == NULL) {
		return;
	}
	p_unblk_api_node->event_id = WHC_API_WIFI_EVENT;
	p_unblk_api_node->param_buf[0] = event;
	if (evt_info && evt_len) {
		buffer = rtos_mem_zmalloc(evt_len);
		if (buffer == NULL) {
			return;
		}
		memcpy(buffer, evt_info, evt_len);
		p_unblk_api_node->param_buf[1] = (u32)buffer;
		p_unblk_api_node->param_buf[2] = evt_len;
	}

	whc_ipc_host_api_unblk_api_enqueue(p_unblk_api_node);
}

static struct whc_ipc_host_unblk_api_node *whc_ipc_host_api_unblk_api_dequeue(void)
{
	struct whc_ipc_host_unblk_api_node *p_unblk_api_node = NULL;
	struct list_head *plist;

	rtos_mutex_take(g_host_unblk_api_q_priv.host_unblk_api_q_mutex, MUTEX_WAIT_TIMEOUT);
	if (rtw_is_list_empty(&g_host_unblk_api_q_priv.host_unblk_api_queue) == FALSE) {
		plist = get_next(&g_host_unblk_api_q_priv.host_unblk_api_queue);
		p_unblk_api_node = LIST_CONTAINOR(plist, struct whc_ipc_host_unblk_api_node, list);
		rtw_list_delete(&(p_unblk_api_node->list));
		g_host_unblk_api_q_priv.host_unblk_api_node_cnt--;
	}
	rtos_mutex_give(g_host_unblk_api_q_priv.host_unblk_api_q_mutex);
	return p_unblk_api_node;
}

/* ---------------------------- Public Functions ---------------------------- */
/**
 * @brief  process the ipc message.
 * @param  none.
 * @return none.
 */
void whc_ipc_host_blk_api_task_h(void)
{
	struct whc_ipc_dev_req_msg *p_ipc_msg = NULL;
	struct whc_ipc_host_unblk_api_node *p_unblk_api_node = NULL;
	do {
		rtos_sema_take(g_host_inic_api_task_wake_sema, 0xFFFFFFFF);

#ifdef IPC_DIR_MSG_RX
		PIPC_MSG_STRUCT p_ipc_recv_msg = ipc_get_message(IPC_DIR_MSG_RX, \
										 IPC_D2H_WIFI_API_TRAN);
		p_ipc_msg = (struct whc_ipc_dev_req_msg *)p_ipc_recv_msg->msg;
#else
		p_ipc_msg = (struct whc_ipc_dev_req_msg *)ipc_get_message(IPC_INT_CHAN_WIFI_API_TRAN);
#endif /* IPC_DIR_MSG_RX */

		DCache_Invalidate((u32)p_ipc_msg, sizeof(struct whc_ipc_dev_req_msg));

		if (p_ipc_msg == NULL) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "DEV IPC API msg NULL\n");
			continue;
		}

		switch (p_ipc_msg->enevt_id) {
		/* callback & api with return value & api which require block*/
		case WHC_API_GET_LWIP_INFO:
			whc_ipc_host_api_lwip_info_handler(p_ipc_msg);
			break;

		case WHC_API_PROMISC_CALLBACK:
			p_ipc_msg->ret = (int)whc_ipc_host_api_promisc_user_callback_handler(p_ipc_msg);
			break;

#if !defined(CONFIG_MP_SHRINK)
		case WHC_API_IP_TABLE_CHK:
#if defined (CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
			p_ipc_msg->ret = dhcps_ip_in_table_check(pnetif_ap, p_ipc_msg->param_buf[0], p_ipc_msg->param_buf[1]);
#else
			p_ipc_msg->ret = dhcps_ip_in_table_check(p_ipc_msg->param_buf[0], p_ipc_msg->param_buf[1]);
#endif
			break;
#endif
#if defined(CONFIG_WIFI_TUNNEL)
#if defined(CONFIG_WTN_SOCKET_APP)
		case WHC_API_WTN_SOCKET_SEND:
			whc_ipc_host_api_wtn_socket_send(p_ipc_msg);
			break;
#endif
		case WHC_API_WTN_OTA_CALLBACK:
			p_ipc_msg->ret = whc_ipc_host_api_wtn_ota_callback_handler(p_ipc_msg);
			break;
#endif
		default:
			/* event & api without return value */
			p_unblk_api_node = whc_ipc_host_api_unblk_api_msg_to_node(p_ipc_msg->enevt_id, p_ipc_msg->param_buf);
			whc_ipc_host_api_unblk_api_enqueue(p_unblk_api_node);
			break;
		}
		/*set EVENT_ID to 0 to notify NP that event is finished*/
		p_ipc_msg->enevt_id = 0;
		DCache_Clean((u32)p_ipc_msg, sizeof(struct whc_ipc_dev_req_msg));
	} while (1);
	rtos_task_delete(NULL);
}

const struct ipc_host_unblk_api_func_t whc_ipc_host_unblk_api_handlers[] = {
	{WHC_API_SCAN_USER_CALLBACK, whc_ipc_host_api_scan_user_callback_handler, 0},
	{WHC_API_SCAN_EACH_REPORT_USER_CALLBACK, whc_ipc_host_api_scan_each_report_callback_handler, 0},
	{WHC_API_IP_ACS, whc_ipc_host_api_acs_report_callback_handler, BIT(0)},
	{WHC_API_AP_CH_SWITCH, whc_ipc_host_api_ap_ch_switch_handler, 0},
	{WHC_API_WIFI_EVENT, whc_ipc_host_api_wifi_event_handler, BIT(1)},
	{WHC_API_SET_NETIF_INFO, whc_ipc_host_api_set_netif_info_handler, BIT(1)},
#ifdef CONFIG_WIFI_TUNNEL
#if defined(CONFIG_WTN_SOCKET_APP)
	{WHC_API_WTN_SOCKET_INIT, whc_ipc_host_api_wtn_socket_init, 0},
#endif
	{WHC_API_WTN_RNAT_AP_INIT, whc_ipc_host_api_wtn_rnat_ap_init, 0},
#endif
#ifdef CONFIG_WIFI_NAN_HOST_APP
	{WHC_API_CFG80211_NAN_CFGVENDOR_EVENT, whc_ipc_host_api_nan_cfgvendor_event_handler, BIT(1)},
	{WHC_API_CFG80211_NAN_CFGVENDOR_CMD_REPLY, whc_ipc_host_api_nan_cfgvendor_cmd_reply_handler, BIT(0)},
#endif
};

/**
 * @brief  process the ipc event & api without any return value.
 * @param  none.
 * @return none.
 */
void whc_ipc_host_unblk_api_task_h(void)
{
	struct whc_ipc_host_unblk_api_node *p_unblk_api_node = NULL;
	u8 i = 0, j = 0;
	void (*host_evt_hdl)(struct whc_ipc_host_unblk_api_node * p_unblk_api_node);

	do {
		rtos_sema_take(g_host_unblk_api_q_priv.host_unblk_api_q_sema, 0xFFFFFFFF);

		while (1) {
			p_unblk_api_node = whc_ipc_host_api_unblk_api_dequeue();

			if (p_unblk_api_node == NULL) {
				break;
			}

			host_evt_hdl = NULL;
			for (i = 0; i < sizeof(whc_ipc_host_unblk_api_handlers) / sizeof(struct ipc_host_unblk_api_func_t); i++) {
				if (whc_ipc_host_unblk_api_handlers[i].host_evt_id == p_unblk_api_node->event_id) {
					host_evt_hdl = whc_ipc_host_unblk_api_handlers[i].func;
					break;
				}
			}

			if (host_evt_hdl) {

				host_evt_hdl(p_unblk_api_node);

				/* free malloc buffer in p_unblk_api_node */
				if (whc_ipc_host_unblk_api_handlers[i].free_bitmap) {
					for (j = 0; j < HOST_MSG_PARAM_NUM; j++) {
						if (whc_ipc_host_unblk_api_handlers[i].free_bitmap & BIT(j)) {
							rtos_mem_free((unsigned char *)p_unblk_api_node->param_buf[j]);
						}
					}
				}
			} else {
				RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Host Event Unknown ID(%x)\n", p_unblk_api_node->event_id);
			}

			/* free p_unblk_api_node */
			rtos_mem_free((unsigned char *)p_unblk_api_node);
		}
	} while (1);
	rtos_task_delete(NULL);
}

/**
 * @brief  to handle the ipc message interrupt, wakeup event task to process.
 * @param  Data[inout]: IPC data.
 * @param  IrqStatus[in]: interrupt status.
 * @param  ChanNum[in]: IPC channel number.
 * @return none.
 */
void whc_ipc_host_api_int_hdl(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	(void) Data;
	(void) IrqStatus;
	(void) ChanNum;

	/* wakeup task */
	rtos_sema_give(g_host_inic_api_task_wake_sema);
}

/**
 * @brief  to send a ipc message to device and wait result.
 * @param  id[in]: api_id.
 * @param  param_buf[in]: pointer to API parameter.
 * @param  buf_len[in]: number of u32 entries in param_buf.
 * @return result of API.
 */
int whc_ipc_host_api_message_send(u32 id, u32 *param_buf, u32 buf_len)
{
	int ret = 0, cnt = 0;

#ifndef CONFIG_PLATFORM_ZEPHYR
	if (!g_inic_host_priv.host_init_done) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Host api err: wifi not init\n");
		return -RTK_ERR_WIFI_NOT_INIT;
	}
#endif
	latest_api_id = id;
	rtos_mutex_take(g_host_inic_api_message_send_lock, MUTEX_WAIT_TIMEOUT);

	while (1) {
		DCache_Invalidate((u32)&g_host_ipc_api_request_info, sizeof(struct whc_ipc_host_req_msg));
		if (g_host_ipc_api_request_info.api_id != WHC_API_PROCESS_DONE) {
			rtos_time_delay_ms(1);
			/*When blocking scan is invoked in BT COEXIST, the scan time may increases due to TDMA scan, up to 8.96s (5G) +2.17s (2.4G)*/
			cnt = (cnt + 1) % 12000;
		} else {
			break;
		}
		if (cnt == 0) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "last inic ipc not hdl \n");
		}
	}

	memset(&g_host_ipc_api_request_info, 0, sizeof(struct whc_ipc_host_req_msg));

	g_host_ipc_api_request_info.api_id = id;
	if (param_buf != NULL) {
		memcpy(g_host_ipc_api_request_info.param_buf, param_buf, buf_len * sizeof(u32));
	}
	DCache_Clean((u32)&g_host_ipc_api_request_info, sizeof(struct whc_ipc_host_req_msg));

#ifdef IPC_DIR_MSG_TX
	memset(&g_host_ipc_api_msg, 0, sizeof(IPC_MSG_STRUCT));
	g_host_ipc_api_msg.msg = (u32)&g_host_ipc_api_request_info;
	g_host_ipc_api_msg.msg_type = IPC_USER_POINT;
	g_host_ipc_api_msg.msg_len = sizeof(struct whc_ipc_host_req_msg);
	DCache_Clean((u32)&g_host_ipc_api_msg, sizeof(IPC_MSG_STRUCT));

	ipc_send_message(IPC_DIR_MSG_TX, IPC_H2D_WIFI_API_TRAN, \
					 &g_host_ipc_api_msg);
#else
	ipc_send_message(IPC_INT_CHAN_WIFI_API_TRAN, &g_host_ipc_api_request_info);
#endif /* IPC_DIR_MSG_TX */

	while (1) {
		if (g_host_ipc_api_request_info.api_id != WHC_API_PROCESS_DONE) {
			if (id == WHC_API_WIFI_WAKE_PLL_RDY_IN_PS) {
				DelayUs(5);  //delayUs is beneficial for BT performance
				/*warning every 1s*/
				cnt = (cnt + 1) % 200000;
			} else {
				rtos_time_delay_ms(1);
				/*When blocking scan is invoked in BT COEXIST, the scan time may increases due to TDMA scan, up to 8.96s (5G) +2.17s (2.4G)*/
				cnt = (cnt + 1) % 12000;
			}
			DCache_Invalidate((u32)&g_host_ipc_api_request_info, sizeof(struct whc_ipc_host_req_msg));
		} else {
			break;
		}
		if (cnt == 0) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ALWAYS, "Host api ipc timeout: cur id 0x%x; latest id 0x%x\n", g_host_ipc_api_request_info.api_id, latest_api_id);
		}
	}
	ret = g_host_ipc_api_request_info.ret;
	rtos_mutex_give(g_host_inic_api_message_send_lock);
	return ret;
}

/**
 * @brief  to initialize the ipc host for WIFI api.
 * @param  none.
 * @return none.
 */
void whc_ipc_host_api_init(void)
{
	/* initialize the semaphores */
	rtos_sema_create_static(&g_host_inic_api_task_wake_sema, 0, 0xFFFFFFFF);
	rtos_mutex_create_static(&g_host_inic_api_message_send_lock);

	rtos_sema_create_static(&g_host_unblk_api_q_priv.host_unblk_api_q_sema, 0, 0xFFFFFFFF);
	/* initialize the mutex */
	rtos_mutex_create_static(&g_host_unblk_api_q_priv.host_unblk_api_q_mutex);
	/* initialize the event list */
	rtw_init_listhead(&g_host_unblk_api_q_priv.host_unblk_api_queue);
	g_host_unblk_api_q_priv.host_unblk_api_node_cnt = 0;
#if !defined(CONFIG_PLATFORM_ZEPHYR)
	/*for updating ip address before sleep*/
	pmu_register_sleep_callback(PMU_WLAN_DEVICE, (PSM_HOOK_FUN)whc_ipc_host_api_addr_update_in_wowlan, NULL, NULL, NULL);
#endif
	/* Initialize the event task */
	if (RTK_SUCCESS != rtos_task_create(NULL, (const char *const)"inic_host_blk_api_task", (rtos_task_function_t)whc_ipc_host_blk_api_task_h, NULL,
										g_rtw_task_size.ipc_blk_api_task, CONFIG_INIC_IPC_HOST_API_PRIO)) {
		RTK_LOGE(TAG_WLAN_INIC, "Create api_host_task Err\n");
	}

	if (RTK_SUCCESS != rtos_task_create(NULL, (const char *const)"inic_host_unblk_api_task", (rtos_task_function_t)whc_ipc_host_unblk_api_task_h, NULL,
										g_rtw_task_size.ipc_unblk_api_task, CONFIG_INIC_IPC_HOST_EVT_API_PRIO)) {
		RTK_LOGE(TAG_WLAN_INIC, "Create api_host_evt_task Err\n");
	}
}

u64 whc_ipc_host_api_get_wifi_tsf(unsigned char port_id)
{
	u64 ret = 0;

	if ((HAL_READ32(WIFI_REG_BASE, 0xA4) & 0x7F00) == BIT13) {
		/* in ips flow, it will return 0 or will be hang, thus need additional check*/
		if (port_id == 0) {
			ret = (((u64) HAL_READ32(WIFI_REG_BASE, 0x564)) << 32) | HAL_READ32(WIFI_REG_BASE, 0x560); //REG_P0_TSFTR
		} else if (port_id == 1) {
			ret = (((u64) HAL_READ32(WIFI_REG_BASE, 0x56C)) << 32) | HAL_READ32(WIFI_REG_BASE, 0x568); //REG_P1_TSFTR
		}
	} else {
		ret = 0; /* !pon state */
	}

	return ret;
}

s32 whc_ipc_host_api_get_wifi_latched_tsf_i2s(struct rtw_speaker_read_latch_req *req, struct rtw_speaker_read_latch_rpt *rpt)
{
	if (req == 0 || rpt == 0) {
		return RTK_ERR_BADARG;
	}

#if defined (CONFIG_AMEBAGREEN2)
	u32 usl22_ns, ph_ex = 0, ctr = 0;
	u64 us64;

	usl22_ns = HAL_READ32(WIFI_REG_BASE, 0x155C);
	us64 = whc_ipc_host_api_get_wifi_tsf((HAL_READ32(WIFI_REG_BASE, 0x5F0) & 0x80000000) == 0);

	if (req->i2s_tx) {
		ctr = AUDIO_SP_GetTXCounterVal(0);
		ph_ex = AUDIO_SP_GetTXExactPhaseVal(0);
	} else {
		ctr = AUDIO_SP_GetRXCounterVal(0);
		ph_ex = AUDIO_SP_GetRXExactPhaseVal(0);
	}

	rpt->tsf_us = (us64 & ~((u64)0x3FFFFF)) | (usl22_ns >> 10);
	rpt->tsf_ns = usl22_ns & 0x3FF;
	rpt->i2s_counter = (((u64) ctr) << 11) | ph_ex;

	return RTK_SUCCESS;
#else
	return RTK_FAIL;
#endif
}

int whc_ipc_host_api_get_txbuf_pkt_num(void)
{
	int ret = 0;
#if defined (CONFIG_AMEBASMART)
	u16 queue0_info = (HAL_READ16(WIFI_REG_BASE, 0x400) & 0x7F00) >> 8;//REG_Q0_INFO
	u16 queue1_info = (HAL_READ16(WIFI_REG_BASE, 0x404) & 0x7F00) >> 8;//REG_Q1_INFO
	u16 queue2_info = (HAL_READ16(WIFI_REG_BASE, 0x408) & 0x7F00) >> 8;//REG_Q2_INFO
	u16 queue3_info = (HAL_READ16(WIFI_REG_BASE, 0x40C) & 0x7F00) >> 8;//REG_Q3_INFO
	u16 mgnt_queue_info = (HAL_READ16(WIFI_REG_BASE, 0x410) & 0x7F00) >> 8;//REG_MGQ_INFO
	u16 high_queue_info = (HAL_READ16(WIFI_REG_BASE, 0x414) & 0x7F00) >> 8;//REG_HIQ_INFO
	ret = queue0_info + queue1_info + queue2_info + queue3_info + mgnt_queue_info + high_queue_info;
#endif
	return ret;

}

int whc_ipc_host_api_iwpriv_command(char *cmd, unsigned int cmd_len, int show_msg)
{
	int ret = 0;
	u32 param_buf[3];

	param_buf[0] = (u32)cmd;
	param_buf[1] = (u32)cmd_len;
	param_buf[2] = (u32)show_msg;
	DCache_Clean((u32)cmd, cmd_len);

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_IWPRIV_INFO, param_buf, 3);
	DCache_Invalidate((u32)cmd, 32);  /*read output log for some cmds */
	return ret;
}

void whc_ipc_host_api_mp_command(char *token, unsigned int cmd_len, int show_msg)
{
	u32 param_buf[4];
#if defined(CONFIG_BT_ENABLE_FAST_MP) && CONFIG_BT_ENABLE_FAST_MP
	extern void bt_fast_mp_cmd_handle_api(void *arg);
	if (strncmp(token, "fastmp ", strlen("fastmp ")) == 0) {
		bt_fast_mp_cmd_handle_api(token);
	}
#endif
	char *user_buf = (char *)rtos_mem_zmalloc(WHC_MP_MSG_BUF_SIZE); //support max buf for PSD
	if (user_buf == NULL) {
		RTK_LOGE(TAG_WLAN_INIC, "whc_ipc_host_api_mp_command fail\n");
		return;
	}

	param_buf[0] = (u32)token;
	param_buf[1] = (u32)cmd_len;
	param_buf[2] = (u32)show_msg;
	param_buf[3] = (u32)user_buf;
	DCache_Clean((u32)user_buf, WHC_MP_MSG_BUF_SIZE);
	DCache_Clean((u32)token, cmd_len);

	whc_ipc_host_api_message_send(WHC_API_WIFI_MP_CMD, param_buf, 4);
	/* user_buf contains mp command result(in string format) from NP core,
	Dcache_Invalidate user_buf before use it */
	if (show_msg) {
		DCache_Invalidate((u32)user_buf, WHC_MP_MSG_BUF_SIZE);
		RTK_LOGA(TAG_WLAN_INIC, "Private Message: %s\n", user_buf);
	}
	rtos_mem_free((u8 *)user_buf);
}

/* ---------------------------- Global Variables ---------------------------- */
