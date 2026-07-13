/**
  ******************************************************************************
  * @file    inic_ipc_host_api_basic.c
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
#include "wifi_api.h"

extern struct host_priv g_inic_host_priv;
extern struct sk_buff *host_skb_buff;
extern int skb_buf_max_size;

int (*rx_callback_ptr)(uint8_t idx, void *buffer, uint16_t len);
void (*tx_read_pkt_ptr)(void *pkt_addr, void *data, size_t length);
struct net_pkt;
extern int net_pkt_read(struct net_pkt *pkt, void *data, size_t length);
extern int eth_rtk_rx(uint8_t idx, void *buffer, uint16_t len);

/*****************************dummy func start****************************************/
/* for compile lib only, --whole-archive should be used when link this lib */
__weak int eth_rtk_rx(uint8_t idx, void *buffer, uint16_t len)
{
	(void)idx;
	(void)buffer;
	(void)len;
	DiagPrintf("!!!error, should't call weak func: %s %d\r\n", __func__, __LINE__);

	return 0;
}

__weak int settings_save_one(const char *name, const void *value, size_t val_len)
{
	(void)name;
	(void)value;
	(void)val_len;
	DiagPrintf("!!!error, should't call weak func: %s %d\r\n", __func__, __LINE__);

	return 0;

}

__weak int settings_load_one(const char *name, void *buf, size_t buf_len)
{
	(void)name;
	(void)buf;
	(void)buf_len;
	DiagPrintf("!!!error, should't call weak func: %s %d\r\n", __func__, __LINE__);

	return 0;

}

__weak void ameba_wifi_handle_connect_event(void)
{
	DiagPrintf("!!!error, should't call weak func: %s %d\r\n", __func__, __LINE__);
	return;
}

__weak int dhcps_ip_in_table_check(u8 gate, u8 d)
{
	/* TODO */
	(void) gate;
	(void) d;

	return 1;
}
/*****************************dummy func end****************************************/

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

int whc_host_send_zephyr(int idx, void *pkt_addr, uint32_t len)
{
	static int used_skb_num = 0;
	struct sk_buff *skb = NULL;
	int size;
	rtos_mutex_take(g_inic_host_priv.host_send_lock, MUTEX_WAIT_TIMEOUT);

	skb = whc_ipc_host_find_one_free_skb(&used_skb_num);

	if (skb == NULL) {
		rtos_mutex_give(g_inic_host_priv.host_send_lock);
		return -1;
	}

	memset(&(skb->head), '\0', sizeof(struct sk_buff) - ((u32) & (skb->head) - (u32)skb));
	size = SKB_DATA_ALIGN(len + SKB_DATA_ALIGN(SKB_WLAN_TX_EXTRA_LEN));
	skb->head = skb->buf;
	skb->end = skb->buf + size;
	skb->data = skb->buf + SKB_DATA_ALIGN(SKB_WLAN_TX_EXTRA_LEN);
	skb->tail = skb->buf + SKB_DATA_ALIGN(SKB_WLAN_TX_EXTRA_LEN);
	skb->busy = 1;
	skb->no_free = 1;
	ATOMIC_SET(&skb->ref, 1);

	used_skb_num = (used_skb_num + 1) % wifi_user_config.skb_num_ap;

	net_pkt_read((struct net_pkt *)pkt_addr, skb->tail, len);
	skb_put(skb, len);

	DCache_CleanInvalidate((u32)skb->head, skb_buf_max_size);
	DCache_CleanInvalidate((u32)skb, sizeof(struct sk_buff));

	whc_ipc_host_send_skb(idx, skb);
	rtos_mutex_give(g_inic_host_priv.host_send_lock);

	return 0;
}

void whc_ipc_host_rx_handler(int idx_wlan, struct sk_buff *skb)
{
	DCache_Invalidate(((u32)skb), sizeof(struct sk_buff));
	DCache_CleanInvalidate((u32)skb->head, skb_buf_max_size);

	eth_rtk_rx(idx_wlan, skb->data, skb->len);

	DCache_CleanInvalidate(((u32)skb), sizeof(struct sk_buff));
	DCache_CleanInvalidate((u32)skb->head, skb_buf_max_size);

	whc_ipc_send_msg(IPC_WIFI_MSG_RECV_DONE, (u32)skb, 0, 0);
}