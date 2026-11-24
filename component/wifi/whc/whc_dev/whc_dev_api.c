/******************************************************************************
 *
 * Copyright(c) 2020 - 2021 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#include "whc_dev.h"

#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
#include "whc_dev_tcpip.h"

#include "lwip/sys.h"
#include "lwip/etharp.h"
#include "lwip/prot/tcp.h"
#include "lwip/prot/dns.h"
#include "lwip/udp.h"
#include "lwip_netconf.h"

/* set host state for dev */
u8 whc_hostrdy;

u8 whc_default_direction = PORT_TO_HOST;
struct list_head whc_filter_head;

/**
 * @brief  create a node for target filter
 * @param  filter: target entry
 * @return none
 */
struct PktFilterNode *whc_dev_api_create_filter_node(struct whc_dev_pkt_filter *filter)
{
	struct PktFilterNode *new_node = (struct PktFilterNode *)rtos_mem_zmalloc(sizeof(struct PktFilterNode));
	if (!new_node) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
		return NULL;
	}

	memcpy((u8 *) & (new_node->filter), (u8 *)filter, sizeof(struct whc_dev_pkt_filter));
	return new_node;
}

/**
 * @brief  add target filter entry
 * @param  filter: target entry to add
 * @return none
 */
void whc_dev_api_add_filter_node(struct whc_dev_pkt_filter *filter)
{
	struct PktFilterNode *new_node = whc_dev_api_create_filter_node(filter);

	if (!new_node) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, fail to add filter!!\n", __func__);
		return;
	}

	// Insert the new node at the end of the list
	rtw_list_insert_tail(&(new_node->list), &whc_filter_head);
}

/**
 * @brief  get target filter entry info by identity
 * @param  filter: pointer to the structure where the filter information will be copied
* @return none
 */
void whc_dev_api_get_filter_node(struct whc_dev_pkt_filter *filter, u32_t identity)
{
	struct list_head *plist, *phead;
	struct PktFilterNode *target;
	u8 match = 0;
	phead = &whc_filter_head;
	if (list_empty(phead)) {
		goto error;
	}

	plist = get_next(phead);

	while ((rtw_end_of_queue_search(phead, plist)) == FALSE) {
		target = LIST_CONTAINOR(plist, struct PktFilterNode, list);

		if (target && target->filter.identity == identity) {
			match = 1;
			break;
		}
		plist = get_next(plist);
	}

	if (match) {
		memcpy((u8 *)filter, (u8 *)&target->filter, sizeof(struct whc_dev_pkt_filter));
		return;
	}

error:
	RTK_LOGE(TAG_WLAN_INIC, "%s, can't find valid node for identity %u!\n", __func__, identity);
	return;

}

/**
 * @brief  delete target filter entry by identity
 * @param  identity: identity for target entry
 * @return none
 */
void whc_dev_api_delete_filter_node(u32_t identity)
{
	struct list_head *plist, *phead;
	struct PktFilterNode *target;
	u8 match = 0;

	phead = &whc_filter_head;
	if (list_empty(phead)) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, list is empty, nothing to delete\n", __func__);
		return;
	}

	plist = get_next(phead);

	while ((rtw_end_of_queue_search(phead, plist)) == FALSE) {
		target = LIST_CONTAINOR(plist, struct PktFilterNode, list);

		if (target && target->filter.identity == identity) {
			match = 1;
			break;
		}
		plist = get_next(plist);
	}


	if (!match) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, can't find valid node for identity %u!\n", __func__, identity);
		return;
	}

	list_del(&(target->list));
	rtos_mem_free(target);
}

/**
 * @brief  set host state, pkt to host only when host in ready state
 * @param  state: WHC_HOST_READY/WHC_HOST_UNREADY
 * @return none.
 * @note: host shouldn't inform dev to set host ready until host can recv pkts.
 */
void whc_dev_api_set_host_state(u8 state)
{
	whc_hostrdy = state;
	if (state == WHC_HOST_READY) {
		pmu_acquire_wakelock(PMU_WHC_WIFI);
	} else {
		pmu_release_wakelock(PMU_WHC_WIFI);
	}
}

/**
 * @brief  get host state
 * @return WHC_HOST_READY/WHC_HOST_UNREADY.
 */
u8 whc_dev_api_get_host_rdy(void)
{
	return whc_hostrdy;
}

/**
 * @brief  set default direction for pkts when pkt not match any filter entry
 * @param  dir: PORT_TO_HOST/PORT_TO_BOTH/PORT_TO_UP
 */
void whc_dev_api_set_default_direction(u8 dir)
{
	whc_default_direction = dir;
}

/**
 * @brief  set default direction for pkts when pkt not match any filter entry
 * @return PORT_TO_HOST/PORT_TO_BOTH/PORT_TO_UP
 * @note: default PORT_TO_HOST in RTK code.
 */
u8  whc_dev_api_get_default_direction(void)
{
	return whc_default_direction;
}
#endif

#ifdef CONFIG_WHC_CMD_PATH
/**
 * @brief  to send data to host
 * @param  data: data buf to be sent.
 * @param  len: data len to be sent.
 * @param  ret: return value buf. It can be NULL if return value is not needed.
 * @param  ret_len: return value len. It can be 0 if return value is not needed.
 * @return none.
 * @note   The caller should free the buffer after calling this function if the input buffer is dynamically allocated.
 */
void whc_dev_api_send_to_host(u8 *data, u32 len, uint8_t *ret, uint32_t ret_len)
{
	int val;
	u8 *buf = NULL, *txbuf = NULL;
	struct whc_cmd_path_hdr *hdr = NULL;
	struct whc_txbuf_info_t *whc_tx = NULL;
	u8 block = (ret && ret_len > 0) ? 1 : 0;

	if (block) {
		val = rtos_mutex_take(whc_cmdpath_data.whc_user_blocksend_mutex, 5000);
		if (val != RTK_SUCCESS) {
			RTK_LOGE(TAG_WLAN_INIC, "%s, fail to take mutex!!\n", __func__);
			return;
		} else {
			whc_cmdpath_data.ret = ret;
			whc_cmdpath_data.ret_len = ret_len;
		}
	}

	buf = rtos_mem_zmalloc(len + sizeof(struct whc_cmd_path_hdr) + DEV_DMA_ALIGN);
	whc_tx = (struct whc_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));
	if (!buf || !whc_tx) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
		rtos_mem_free(buf);
		rtos_mem_free(whc_tx);
		goto exit;
	}

	/* append whc_cmd_path_hdr */
	txbuf = (u8 *)N_BYTE_ALIGMENT((u32)buf, DEV_DMA_ALIGN);
	hdr = (struct whc_cmd_path_hdr *)txbuf;
	hdr->event = WHC_WIFI_EVT_CMD;
	hdr->len = len;
	memcpy(txbuf + sizeof(struct whc_cmd_path_hdr), data, len);

	whc_tx->txbuf_info.buf_allocated = whc_tx->txbuf_info.buf_addr = (u32)txbuf;
	whc_tx->txbuf_info.size_allocated = whc_tx->txbuf_info.buf_size = len + sizeof(struct whc_cmd_path_hdr);
	whc_tx->ptr = buf;
	whc_tx->is_skb = 0;

	/* send hdr + data, buf is freed in bus tx done */
	whc_dev_send(&whc_tx->txbuf_info);

	if (block) {
		/* block until the host returns the value. The user should give sema after the device completes the processing */
		val = rtos_sema_take(whc_cmdpath_data.whc_user_blocksend_sema, 10000);
		if (val != RTK_SUCCESS) {
			RTK_LOGE(TAG_WLAN_INIC, "%s, fail to take sema!!\n", __func__);
		}
	}

exit:
	if (block) {
		whc_cmdpath_data.ret = NULL;
		whc_cmdpath_data.ret_len = 0;
		rtos_mutex_give(whc_cmdpath_data.whc_user_blocksend_mutex);
	}
}

#endif

/**
 * @brief  Check if the bus is currently idle
 * @return  TRUE if bus is idle, FALSE otherwise.
 * @note  Only SPI implements this function
 */
u8 whc_dev_api_bus_is_idle(void)
{
	return _whc_dev_api_bus_is_idle();
}
