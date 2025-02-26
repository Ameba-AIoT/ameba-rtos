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

#ifdef CONFIG_FULLMAC_BRIDGE
/* set host state for dev */
u8 whc_bridge_hostrdy;
#endif

u8 whc_bridge_default_direction = PORT_TO_HOST;
struct PktFilterNode *bridge_filter_head = NULL;
struct PktFilterNode *bridge_filter_tail = NULL;

#ifdef CONFIG_FULLMAC_BRIDGE

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

	new_node->filter = (struct whc_bridge_dev_pkt_filter *)rtos_mem_zmalloc(sizeof(struct whc_bridge_dev_pkt_filter));
	if (!new_node->filter) {
		rtos_mem_free(new_node);
		RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
		return NULL;
	}

	memcpy(new_node->filter, filter, sizeof(struct whc_bridge_dev_pkt_filter));
	new_node->next = NULL;
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
	bridge_filter_tail->next = new_node;
	bridge_filter_tail = new_node;
}

/**
 * @brief  get target filter entry info by identity
 * @param  filter: pointer to the structure where the filter information will be copied
 * @return none
 */
void whc_bridge_dev_api_get_filter_node(struct whc_bridge_dev_pkt_filter *filter, u32_t identity)
{
	struct PktFilterNode *temp = bridge_filter_head->next;

	if (filter == NULL) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, provided filter pointer is NULL!\n", __func__);
		return;
	}

	while (temp && temp->filter && (temp->filter->identity != identity)) {
		temp = temp->next;
	}

	if (!temp || !temp->filter) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, can't find valid node by identity %u!!\n", __func__, identity);
		return;
	}

	memcpy(filter, temp->filter, sizeof(struct whc_bridge_dev_pkt_filter));
}

/**
 * @brief  delete target filter entry by identity
 * @param  identity: identity for target entry
 * @return none
 */
void whc_bridge_dev_api_delete_filter_node(u32_t identity)
{
	struct PktFilterNode *temp = bridge_filter_head->next;
	struct PktFilterNode *prev = NULL;

	if (!bridge_filter_head || !temp) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, list is empty, nothing to delete\n", __func__);
		return;
	}

	while (temp && temp->filter && (temp->filter->identity != identity)) {
		prev = temp;
		temp = temp->next;
	}

	if (!temp || !temp->filter) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, can't find valid node for identity %u!\n", __func__, identity);
		return;
	}

	prev->next = temp->next;

	if (bridge_filter_tail == temp) {
		bridge_filter_tail = prev;
	}

	rtos_mem_free(temp->filter);
	rtos_mem_free(temp);
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
 * @param  data: data buf to be sent, data shold always 4Bytes aligned.
 * @param  len: data len to be sent.
 * @return none.
 * @note: data buf free after data move to host in RTK code, static data buf is not allowed.
 */
void whc_bridge_dev_api_send_to_host(u8 *user_alloc_drv_free_data, u32 len)
{
	whc_dev_api_send_data(user_alloc_drv_free_data, len);
}

#endif

