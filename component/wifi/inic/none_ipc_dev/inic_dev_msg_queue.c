/******************************************************************************
 *
 * Copyright(c) 2020 - 2021 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#include "inic_dev.h"

/**
 * @brief  get the inic message from queue.
 * @param  p_queue[in]: the queue used to store the p_node.
 * @return the inic_msg_node got from message queue.
 */
struct inic_msg_node *inic_msg_dequeue(struct __queue *p_queue)
{
	struct inic_msg_node *p_node;
	struct list_head *plist, *phead;

	rtos_critical_enter();

	if (rtw_queue_empty(p_queue) == TRUE) {
		p_node = NULL;
	} else {
		phead = get_list_head(p_queue);
		plist = get_next(phead);
		p_node = LIST_CONTAINOR(plist, struct inic_msg_node, list);

		rtw_list_delete(&(p_node->list));
	}

	rtos_critical_exit();

	return p_node;
}

/**
 * @brief  put the inic message to queue.
 * @param  p_node[in]: the pointer for the inic message node that need to be
 * 	pushed into the queue.
 * @param  p_queue[in]: the queue used to store the p_node.
 * @return status, always SUCCESS.
 */
sint inic_msg_enqueue(void *msg, struct __queue *p_queue)
{
	struct inic_msg_node *p_node = NULL;

	/* allocate memory for message node. */
	p_node = rtos_mem_zmalloc(sizeof(struct inic_msg_node));
	if (p_node == NULL) {
		return FAIL;
	}
	p_node->msg = msg;

	/* put the inic message to the tail of the queue */
	rtos_critical_enter();
	rtw_list_insert_tail(&(p_node->list), get_list_head(p_queue));
	rtos_critical_exit();

	return SUCCESS;
}
