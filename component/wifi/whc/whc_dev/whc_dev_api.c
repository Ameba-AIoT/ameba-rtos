/******************************************************************************
 *
 * Copyright(c) 2020 - 2021 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#include "whc_dev.h"

#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
#include "lwip_netconf.h"

/* set host state for dev */
static u8 whc_hostrdy;
static u8 whc_default_direction = INDICATE_TO_HOST;
struct list_head whc_filter_head;

/**
 * @brief  add target filter entry
 * @param  filter: target entry to add
 * @return none
 */
void whc_dev_api_add_filter_node(struct whc_dev_pkt_filter *filter)
{
	struct whc_dev_pktfilter_node *new_node = (struct whc_dev_pktfilter_node *)rtos_mem_zmalloc(sizeof(struct whc_dev_pktfilter_node));

	if (!new_node) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
		return;
	}
	memcpy((u8 *) & (new_node->filter), (u8 *)filter, sizeof(struct whc_dev_pkt_filter));

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
	struct whc_dev_pktfilter_node *target;
	u8 match = 0;
	phead = &whc_filter_head;
	if (list_empty(phead)) {
		goto error;
	}

	plist = get_next(phead);

	while ((rtw_end_of_queue_search(phead, plist)) == FALSE) {
		target = LIST_CONTAINOR(plist, struct whc_dev_pktfilter_node, list);

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
	struct whc_dev_pktfilter_node *target;
	u8 match = 0;

	phead = &whc_filter_head;
	if (list_empty(phead)) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, list is empty, nothing to delete\n", __func__);
		return;
	}

	plist = get_next(phead);

	while ((rtw_end_of_queue_search(phead, plist)) == FALSE) {
		target = LIST_CONTAINOR(plist, struct whc_dev_pktfilter_node, list);

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
 * @param  dir: INDICATE_TO_HOST/INDICATE_TO_BOTH/INDICATE_TO_DEV_LWIP
 */
void whc_dev_api_set_default_direction(u8 dir)
{
	whc_default_direction = dir;
}

/**
 * @brief  set default direction for pkts when pkt not match any filter entry
 * @return INDICATE_TO_HOST/INDICATE_TO_BOTH/INDICATE_TO_DEV_LWIP
 * @note: default INDICATE_TO_HOST in RTK code.
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
 */
void whc_dev_api_send_to_host(u8 *data, u32 len, uint8_t *ret, uint32_t ret_len)
{
	int val;

	if (ret && ret_len > 0) {
		val = rtos_mutex_take(whc_cmdpath_data.whc_user_blocksend_mutex, 5000);
		if (val != RTK_SUCCESS) {
			RTK_LOGE(TAG_WLAN_INIC, "%s, fail to take mutex!!\n", __func__);
			return;
		} else {
			whc_cmdpath_data.ret = ret;
			whc_cmdpath_data.ret_len = ret_len;
		}
	}

	whc_dev_api_send_data(data, len);

	if (ret && ret_len > 0) {
		val = rtos_sema_take(whc_cmdpath_data.whc_user_blocksend_sema, 10000);
		if (val != RTK_SUCCESS) {
			RTK_LOGE(TAG_WLAN_INIC, "%s, fail to take sema!!\n", __func__);
		}

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
	return whc_dev_bus_is_idle();
}