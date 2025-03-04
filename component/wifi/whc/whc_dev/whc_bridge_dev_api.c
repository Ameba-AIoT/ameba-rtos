/******************************************************************************
 *
 * Copyright(c) 2020 - 2021 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#include "whc_dev.h"
#include "whc_dev_bridge.h"

#include "lwip/sys.h"
#include "lwip/etharp.h"
#include "lwip/prot/tcp.h"
#include "lwip/prot/dns.h"
#include "lwip/udp.h"
#include "lwip_netconf.h"

#ifdef CONFIG_WHC_BRIDGE
/* set host state for dev */
u8 whc_bridge_hostrdy;
#endif

u8 whc_bridge_default_direction = PORT_TO_HOST;
struct list_head bridge_filter_head;

#ifdef CONFIG_WHC_BRIDGE

/**
 * @brief  create a node for target filter
 * @param  filter: target entry
 * @return none
 */
struct PktFilterNode *whc_bridge_dev_api_create_filter_node(struct whc_bridge_dev_pkt_filter *filter)
{
	struct PktFilterNode *new_node = (struct PktFilterNode *)rtos_mem_zmalloc(sizeof(struct PktFilterNode));
	if (!new_node) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
		return NULL;
	}

	memcpy((u8 *) & (new_node->filter), (u8 *)filter, sizeof(struct whc_bridge_dev_pkt_filter));
	return new_node;
}

/**
 * @brief  add target filter entry
 * @param  filter: target entry to add
 * @return none
 */
void whc_bridge_dev_api_add_filter_node(struct whc_bridge_dev_pkt_filter *filter)
{
	struct PktFilterNode *new_node = whc_bridge_dev_api_create_filter_node(filter);

	if (!new_node) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, fail to add filter!!\n", __func__);
		return;
	}

	// Insert the new node at the end of the list
	rtw_list_insert_tail(&(new_node->list), &bridge_filter_head);
}

/**
 * @brief  get target filter entry info by identity
 * @param  filter: pointer to the structure where the filter information will be copied
 * @return none
 */
void whc_bridge_dev_api_get_filter_node(struct whc_bridge_dev_pkt_filter *filter, u32_t identity)
{
	struct list_head *plist, *phead;
	struct PktFilterNode *target;
	u8 match = 0;
	phead = &bridge_filter_head;
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
		memcpy((u8 *)filter, (u8 *)&target->filter, sizeof(struct whc_bridge_dev_pkt_filter));
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
void whc_bridge_dev_api_delete_filter_node(u32_t identity)
{
	struct list_head *plist, *phead;
	struct PktFilterNode *target;
	u8 match = 0;

	phead = &bridge_filter_head;
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
 * @param  state: WHC_BRIDGE_HOST_READY/WHC_BRIDGE_HOST_UNREADY
 * @return none.
 * @note: host shouldn't inform dev to set host ready until host can recv pkts.
 */
void whc_bridge_dev_api_set_host_state(u8 state)
{
	whc_bridge_hostrdy = state;
}

/**
 * @brief  get host state
 * @return WHC_BRIDGE_HOST_READY/WHC_BRIDGE_HOST_UNREADY.
 */
u8 whc_bridge_dev_api_get_host_rdy(void)
{
	return whc_bridge_hostrdy;
}

/**
 * @brief  set default direction for pkts when pkt not match any filter entry
 * @param  dir: PORT_TO_HOST/PORT_TO_BOTH/PORT_TO_UP
 */
void whc_bridge_dev_api_set_default_direction(u8 dir)
{
	whc_bridge_default_direction = dir;
}

/**
 * @brief  set default direction for pkts when pkt not match any filter entry
 * @return PORT_TO_HOST/PORT_TO_BOTH/PORT_TO_UP
 * @note: default PORT_TO_HOST in RTK code.
 */
u8  whc_bridge_dev_api_get_default_direction(void)
{
	return whc_bridge_default_direction;
}

/**
 * @brief  to send data to host
 * @param  data: data buf to be sent.
 * @param  len: data len to be sent.
 * @return none.
 */
void whc_bridge_dev_api_send_to_host(u8 *data, u32 len)
{
	whc_dev_api_send_data(data, len);
}

#endif

