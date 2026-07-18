/**
  ******************************************************************************
  * @file    whc_ipc_host_trx.c
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

#define __WHC_IPC_HOST_TRX_C__

/* -------------------------------- Includes -------------------------------- */
/* external head files */

/* internal head files */
#include "rtw_inic_common.h"
#ifndef ZEPHYR_WIFI
#include "wifi_api.h"
#endif
#include "ameba_pmu.h"

#define CONFIG_ENABLE_CACHE

/* -------------------------------- Defines --------------------------------- */

/* -------------------------------- Macros ---------------------------------- */

/* ------------------------------- Data Types ------------------------------- */


/* -------------------------- Function declaration -------------------------- */

/* ---------------------------- Global Variables ---------------------------- */

/* --------------------------- Private Variables ---------------------------- */
struct host_priv g_inic_host_priv __attribute__((aligned(RTOS_MEM_BYTE_ALIGNMENT)));
struct sk_buff *host_skb_buff;
int skb_buf_max_size;
struct rtw_pbuf *rpbuf_pool;

extern u8 wifi_dbg_log_enable; // BIT(0): dp_log

/**
 * @brief  to send skb to device for port idx.
 * @param  idx[in]: which port of wifi to tx.
 * @param  skb[inout]: skb to send.
 * @return -1 failed, 0 successful.
 */
SRAM_WLAN_CRITICAL_CODE_SECTION
int whc_ipc_host_send_skb(int idx, struct sk_buff *skb)
{
	if (idx == -1) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "wlan index is wrong\n");
		return -1;
	}

	whc_ipc_send_msg(IPC_WIFI_CMD_XIMT_PKTS, (u32)skb, 0, idx);

	return 0;
}

/* ---------------------------- Public Functions ---------------------------- */
/**
 * @brief  to initialize the skb in host.
 * @param  none
 * @return none.
 */
void whc_ipc_host_init_skb(void)
{
	int i;
	unsigned char *skb_data_buf = NULL;

	if (host_skb_buff) {
		RTK_LOGE(TAG_WLAN_INIC, "host_skb_xx not free\n");
		return;
	}

	skb_buf_max_size = ((wifi_user_config.skb_buf_size + (SKB_CACHE_SZ - 1)) & ~(SKB_CACHE_SZ - 1));

	host_skb_buff = (struct sk_buff *)rtos_mem_zmalloc(wifi_user_config.skb_num_ap * sizeof(struct sk_buff));
	if (!host_skb_buff) {
		RTK_LOGE(TAG_WLAN_INIC, "host_skb_buff malloc fail!\n\r");
		return;
	}

	skb_data_buf = rtos_mem_zmalloc(wifi_user_config.skb_num_ap * skb_buf_max_size);
	if (!skb_data_buf || ((u32)skb_data_buf & (SKB_CACHE_SZ - 1))) {
		RTK_LOGE(TAG_WLAN_INIC, "skb_data_buf malloc fail!\n\r");
		rtos_mem_free(host_skb_buff);
		return;
	}

	for (i = 0; i < wifi_user_config.skb_num_ap; i++) {
		INIT_LIST_HEAD(&host_skb_buff[i].list);

		host_skb_buff[i].buf = skb_data_buf + skb_buf_max_size * i;
	}

	/*make sure the real memory is set to zero, or DCache_Invalidate in whc_host_send will get wrong values*/
	DCache_Clean((u32)host_skb_buff, (wifi_user_config.skb_num_ap * sizeof(struct sk_buff)));
	DCache_Clean((u32)skb_data_buf, (wifi_user_config.skb_num_ap * skb_buf_max_size));
}

void whc_ipc_host_deinit_skb(void)
{
	if (host_skb_buff) {
		if (host_skb_buff[0].buf) {
			rtos_mem_free((u8 *)host_skb_buff[0].buf);
		}
		rtos_mem_free((u8 *)host_skb_buff);
	}
}

/**
 * @brief  to initialize the rpbuf_pool for rx in host.
 * @param  none
 * @return none.
 */
void whc_ipc_host_init_rtwpbuf_pool(void)
{
#if !defined(CONFIG_PLATFORM_ZEPHYR) && !defined(CONFIG_MP_SHRINK) && defined (WHC_RX_SKB_SHARE_TO_PBUF)
	rpbuf_pool = (struct rtw_pbuf *)rtos_mem_zmalloc(wifi_user_config.skb_num_np * sizeof(struct rtw_pbuf));
	if (!rpbuf_pool) {
		RTK_LOGE(TAG_WLAN_INIC, "rpbuf init fail!\n\r");
	}
#endif
}

void whc_ipc_host_deinit_rtwpbuf_pool(void)
{
#if !defined(CONFIG_PLATFORM_ZEPHYR) && !defined(CONFIG_MP_SHRINK) && defined (WHC_RX_SKB_SHARE_TO_PBUF)
	if (rpbuf_pool) {
		rtos_mem_free(rpbuf_pool);
	}
#endif
}

void whc_ipc_host_statistics(rtos_timer_t timer_hdl)
{
	UNUSED(timer_hdl);

	static u32 tx_drop_pkts_last = 0;
	u32 tx_drop_pkts_cur = g_inic_host_priv.tx_drop_pkts;
	if (tx_drop_pkts_cur != tx_drop_pkts_last) {
		if (wifi_dbg_log_enable & BIT(0)) {
			RTK_LOGA(TAG_WLAN_INIC, "tx skb dp: tl=%d r1s=%d\n", tx_drop_pkts_cur, tx_drop_pkts_cur - tx_drop_pkts_last);
		}
		tx_drop_pkts_last = tx_drop_pkts_cur;
	}

	/* if AP core released wakelock, enlarge timer period to avoid AP core wakeup frequently by timer */
	if (pmu_get_wakelock_status() == 0) {
		rtos_timer_change_period(g_inic_host_priv.whc_stats_timer, 60 * 1000, 0);
	} else {
		rtos_timer_change_period(g_inic_host_priv.whc_stats_timer, 1000, 0);
	}

}

/**
 * @brief  to initialize the parameters of recv.
 * @param  none
 * @return none.
 */
void whc_ipc_host_init_priv(void)
{
	memset(&g_inic_host_priv, 0, sizeof(struct host_priv));

	/* initialize semaphores. */
	rtos_mutex_create_static(&(g_inic_host_priv.host_send_lock));

	rtos_timer_create_static(&g_inic_host_priv.whc_stats_timer,
							 "whc_stats_timer",	// Just a text name, not used by the RTOS kernel.
							 NULL,	// Uniq id used to identify which timer expire..
							 RTOS_MAX_DELAY, // Timer Period, not 0
							 FALSE, // Whether timer will auto-load themselves when expires
							 whc_ipc_host_statistics);
	rtos_timer_change_period(g_inic_host_priv.whc_stats_timer, 1000, RTOS_TIMER_MAX_DELAY);

	g_inic_host_priv.rx_bytes = 0;
	g_inic_host_priv.rx_pkts = 0;

	g_inic_host_priv.tx_bytes = 0;
	g_inic_host_priv.tx_pkts = 0;

}

/**
 * @brief  to put the Rx data from rx buffer into Rx queue.
 * @param  idx_wlan[in]: which port of wifi to rx.
 * @param  skb[inout]: data from the ipc bus, its structure is sk_buff.
 * @return none.
 */

#ifndef CONFIG_PLATFORM_ZEPHYR

SRAM_WLAN_CRITICAL_CODE_SECTION
static void whc_ipc_rx_skb_cache_flush(struct sk_buff *skb)
{
	(void) skb;
#ifdef CONFIG_ENABLE_CACHE
	/*need cache clean here even if NP only need free skb,
	because AP may occur cache full issue and flush to skb to memory, but list in skb is old*/
	DCache_CleanInvalidate((u32)skb->head, skb_buf_max_size);
	DCache_CleanInvalidate(((u32)skb), sizeof(struct sk_buff));
#endif /* CONFIG_ENABLE_CACHE */
}

#ifdef WHC_RX_SKB_SHARE_TO_PBUF

// only alloc in whc_ipc_host_rx_handler, and free in pbuf_free, no need mutex
struct rtw_pbuf *whc_ipc_rx_alloc_rtwpbuf(void)
{
	for (int i = 0; i < wifi_user_config.skb_num_np; i++) {
		if (rpbuf_pool[i].busy == 0) {
			rpbuf_pool[i].busy = 1;
			return &rpbuf_pool[i];
		}
	}
	return NULL;

}

void whc_ipc_rx_free_rtwpbuf(struct rtw_pbuf *rpbuf)
{
	if (rpbuf) {
		rpbuf->busy = 0;
	}
}

SRAM_WLAN_CRITICAL_CODE_SECTION
void whc_ipc_rx_pbuf_free(void *p)
{
	struct rtw_pbuf *rpbuf = (struct rtw_pbuf *)p;

	struct sk_buff *skb = rpbuf->skb;
	whc_ipc_rx_free_rtwpbuf(rpbuf);
	whc_ipc_rx_skb_cache_flush(skb);
	whc_ipc_send_msg(IPC_WIFI_MSG_RECV_DONE, (u32)skb, 0, 0); //free skb
}
#endif /* WHC_RX_SKB_SHARE_TO_PBUF END*/

SRAM_WLAN_CRITICAL_CODE_SECTION
__weak void whc_ipc_host_rx_handler(int idx_wlan, struct sk_buff *skb)
{
	(void)idx_wlan;
	(void)skb;
#ifndef CONFIG_MP_SHRINK
	struct pbuf *p_buf = NULL;

#ifdef CONFIG_ENABLE_CACHE
	DCache_Invalidate(((u32)skb), sizeof(struct sk_buff));
	DCache_Invalidate((u32)skb->head, skb_buf_max_size);
#endif /* CONFIG_ENABLE_CACHE */

	/* allocate pbuf to store ethernet data from IPC. */
#ifdef WHC_RX_SKB_SHARE_TO_PBUF
	/* do memcpy, just move the point from skb->data to pbuf */

	struct rtw_pbuf *rpbuf = whc_ipc_rx_alloc_rtwpbuf();

	if (rpbuf == NULL) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "rpbuf m fail\n\r");
		goto RSP;
	}

	rpbuf->p.custom_free_function = (pbuf_free_custom_fn) whc_ipc_rx_pbuf_free; // this function will be called by pbuf_free
	rpbuf->skb = skb;

	p_buf = pbuf_alloced_custom(PBUF_RAW,
								skb->len,
								PBUF_REF,
								&rpbuf->p,
								skb->data,
								skb->len);
	goto LWIP_RECV;

#else /* NOT DEFINE WHC_RX_SKB_SHARE_TO_PBUF */
	struct pbuf *temp_buf = NULL;
	p_buf = pbuf_alloc(PBUF_RAW, skb->len, PBUF_POOL);
	if (p_buf == NULL) {

		//just send rsp when pbuf alloc fail
		goto RSP;
	}

	/* cpoy data from skb(ipc data) to pbuf(ether net data) */
	temp_buf = p_buf;
	while (temp_buf) {
		/* If tot_len > PBUF_POOL_BUFSIZE_ALIGNED, the skb will be
		 * divided into several pbufs. Therefore, there is a while to
		 * use to assigne data to pbufs.
		 */
		memcpy(temp_buf->payload, skb->data, temp_buf->len);
		skb_pull(skb, temp_buf->len);
		temp_buf = temp_buf->next;
	}
#endif /* WHC_RX_SKB_SHARE_TO_PBUF END*/
RSP:
	whc_ipc_rx_skb_cache_flush(skb);
	whc_ipc_send_msg(IPC_WIFI_MSG_RECV_DONE, (u32)skb, 0, 0);
	goto LWIP_RECV;

LWIP_RECV:
	if (p_buf != NULL) {
		g_inic_host_priv.rx_bytes += p_buf->len;
		g_inic_host_priv.rx_pkts++;

		netif_adapter_wifi_recv_whc(idx_wlan, p_buf);
	}
#endif
}

static inline struct sk_buff *whc_ipc_host_find_one_free_skb(int *skb_index)
{
	struct sk_buff *skb;
	int start_idx = *skb_index;

	do {
		skb = &host_skb_buff[*skb_index];
		DCache_Invalidate((u32)skb, sizeof(struct sk_buff));
		if (skb->busy == 0) {
			return skb;
		}

		*skb_index = (*skb_index + 1) % wifi_user_config.skb_num_ap;
	} while (*skb_index != start_idx);

	return NULL;
}

/**
 * @brief  to send data to device for port idx.
 * @param  idx[in]: which port of wifi to tx.
 * @param  sg_list[in]: pbuf list to send.
 * @param  sg_len[in]: the length of sg_list.
 * @param  total_len[in]: the length of data to send.
 * @return result.
 */
SRAM_WLAN_CRITICAL_CODE_SECTION
int whc_ipc_host_send(int idx, struct eth_drv_sg *sg_list, int sg_len,
					  int total_len, struct skb_raw_para *raw_para, u8 is_special_pkt)
{
	struct sk_buff *skb = NULL;
	struct eth_drv_sg *psg_list;
	int ret = 0, i = 0;
	static int used_skb_num = 0;
	int size = 0;
	u8 special_times = 0;

	if (!g_inic_host_priv.host_init_done) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Host trx err: wifi not init\n");
		return -RTK_ERR_WIFI_NOT_INIT;
	}

	rtos_mutex_take(g_inic_host_priv.host_send_lock, MUTEX_WAIT_TIMEOUT);
	/* allocate the skb buffer */

RETRY:
	/*https://jira.realtek.com/browse/RSWLANDIOT-10773*/
	skb = whc_ipc_host_find_one_free_skb(&used_skb_num);
	if (skb == NULL) {
		/*JIRA: https://jira.realtek.com/browse/RSWLANDIOT-8584 */
		if (is_special_pkt && (special_times < 9)) {
			special_times++;
			rtos_time_delay_ms(1);
			goto RETRY;
		}
		g_inic_host_priv.tx_drop_pkts++;
		rtos_mutex_give(g_inic_host_priv.host_send_lock);
		return -2; //ERR_BUF
	}
	/* Only reset head and the paras behind , excude skb->list and skb->buf. */
	memset(&(skb->head), '\0', sizeof(struct sk_buff) - ((u32) & (skb->head) - (u32)skb));
	size = SKB_DATA_ALIGN(total_len + SKB_DATA_ALIGN(SKB_WLAN_TX_EXTRA_LEN));
	skb->head = skb->buf;
	skb->end = skb->buf + size;
	skb->data = skb->buf + SKB_DATA_ALIGN(SKB_WLAN_TX_EXTRA_LEN);
	skb->tail = skb->buf + SKB_DATA_ALIGN(SKB_WLAN_TX_EXTRA_LEN);
	skb->busy = 1;
	skb->no_free = 1;
	ATOMIC_SET(&skb->ref, 1);

	if (raw_para) {
		skb->tx_raw.enable = TRUE;
		skb->tx_raw.rate = raw_para->rate;
		skb->tx_raw.retry_limit = raw_para->retry_limit;
		skb->tx_raw.ac_queue = raw_para->ac_queue;
		skb->tx_raw.agg_en = raw_para->agg_en;
		skb->tx_raw.sgi = raw_para->sgi;
		skb->tx_raw.device_id = raw_para->device_id;
		skb->tx_raw.bw_40_en = raw_para->bw_40_en;
	}

	used_skb_num++;
	used_skb_num = used_skb_num % wifi_user_config.skb_num_ap;

	psg_list = sg_list;
	for (i = 0; i < sg_len; i++) {
		psg_list = &sg_list[i];
		memcpy(skb->tail, (void *)(psg_list->buf), psg_list->len);
		skb_put(skb, psg_list->len);
	}

	whc_ipc_rx_skb_cache_flush(skb);

	whc_ipc_host_send_skb(idx, skb);
	rtos_mutex_give(g_inic_host_priv.host_send_lock);

	return ret;
}
#endif

/**
* @brief  handle the message of IPC.
* @param  none.
* @return none.
*/
SRAM_WLAN_CRITICAL_CODE_SECTION
void whc_ipc_host_trx_event_hdl(u8 event_num, u32 msg_addr, u8 wlan_idx)
{
	switch (event_num) {
	/* receive the data from device */
	case IPC_WIFI_EVT_RECV_PKTS:
		whc_ipc_host_rx_handler(wlan_idx,
								(struct sk_buff *)msg_addr);
		break;
	/* other contrl operations */
	default:
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Host Unknown event(%lx)!\n\r", event_num);
		break;
	}
}

/* ---------------------------- Public Functions ---------------------------- */
/**
* @brief  to handle the ipc message interrupt. If the message queue is
*  initialized, it will enqueue the ipc message and wake up the message
*  task to handle the message. If last send message cannot be done, I will
*  set pending for next sending message.
* @param  Data[inout]: IPC data.
* @param  IrqStatus[in]: interrupt status.
* @param  ChanNum[in]: IPC channel number.
* @return none.
*/
SRAM_WLAN_CRITICAL_CODE_SECTION
void whc_ipc_host_trx_int_hdl(void *Data, u32 IrqStatus, u32 ChanNum)
{
	(void) Data;
	(void) IrqStatus;
	(void) ChanNum;

	struct whc_ipc_ex_msg *p_ipc_msg = NULL;
	sint ret = RTK_FAIL;

#ifdef IPC_DIR_MSG_RX
	PIPC_MSG_STRUCT p_ipc_recv_msg = ipc_get_message(IPC_DIR_MSG_RX, \
									 IPC_D2H_WIFI_TRX_TRAN);
	p_ipc_msg = (struct whc_ipc_ex_msg *)p_ipc_recv_msg->msg;
#else
	p_ipc_msg = (struct whc_ipc_ex_msg *)ipc_get_message(IPC_INT_CHAN_WIFI_TRX_TRAN);
#endif /* IPC_DIR_MSG_RX */

#ifdef CONFIG_ENABLE_CACHE
	DCache_Invalidate((u32)p_ipc_msg, sizeof(struct whc_ipc_ex_msg));
#endif /* CONFIG_ENABLE_CACHE */

	if (whc_msg_get_queue_status()) {
		/* put the ipc message to the queue */
		ret = whc_msg_enqueue(p_ipc_msg);
	} else {
		/* the message queue doesn't work, call handle function
		* directly */
		whc_ipc_host_trx_event_hdl(p_ipc_msg->event_num, p_ipc_msg->msg_addr, p_ipc_msg->wlan_idx);
		ret = RTK_SUCCESS;
	}

	if (ret == RTK_SUCCESS) {
		p_ipc_msg->msg_queue_status = 0;
	} else {
		p_ipc_msg->msg_queue_status = IPC_WIFI_MSG_MEMORY_NOT_ENOUGH;
	}
	/* enqueuing message is successful, send acknowledgement to another port*/
	p_ipc_msg->event_num = IPC_WIFI_MSG_READ_DONE;
#ifdef CONFIG_ENABLE_CACHE
	DCache_Clean((u32)p_ipc_msg, sizeof(struct whc_ipc_ex_msg));
#endif /* CONFIG_ENABLE_CACHE */
}
