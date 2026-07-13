/**
  ******************************************************************************
  * @file    whc_ipc_host_msg_queue.c
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

#define __WHC_IPC_MSG_QUEUE_C__

/* -------------------------------- Includes -------------------------------- */
/* external head files */
#include "rtw_inic_common.h"

#include "platform_autoconf.h"
#include "rtw_skbuff.h"

extern void whc_ipc_host_trx_event_hdl(u8 event_num, u32 msg_addr, u8 wlan_idx);

/* ---------------------------- Global Variables ---------------------------- */

/* --------------------------- Private Variables ---------------------------- */

static struct ipc_msg_q_priv g_ipc_msg_q_priv;
#ifdef IPC_DIR_MSG_TX
struct whc_ipc_ex_msg g_whc_ipc_ex_msg __attribute__((aligned(RTOS_MEM_BYTE_ALIGNMENT)));
#else
static struct whc_ipc_ex_msg g_whc_ipc_ex_msg = {0};
#endif

#ifdef ZEPHYR_WIFI
static struct k_thread whc_msg_q_thread;
K_THREAD_STACK_DEFINE(inic_msg_q_stack, WIFI_WHC_IPC_MSG_Q_TASK_SIZE);
#endif

/* ---------------------------- Private Functions --------------------------- */
/**
 * @brief  put the ipc message to queue.
 * @param  p_node[in]: the pointer for the ipc message node that need to be
 * 	pushed into the queue.
 * @param  p_queue[in]: the queue used to store the p_node.
 * @return status, always RTK_SUCCESS.
 */
static sint enqueue_ipc_msg_node(struct ipc_msg_node *p_node, struct __queue *p_queue)
{
	rtos_critical_enter(RTOS_CRITICAL_WIFI);
	/* put the ipc message to the tail of the queue */
	rtw_list_insert_tail(&(p_node->list), get_list_head(p_queue));
	rtos_critical_exit(RTOS_CRITICAL_WIFI);
	return RTK_SUCCESS;
}

/**
 * @brief  get the ipc message from queue.
 * @param  p_queue[in]: the queue used to store the p_node.
 * @return the ipc_msg_node got from message queue.
 */
static struct ipc_msg_node *dequeue_ipc_msg_node(struct __queue *p_queue)
{
	struct ipc_msg_node *p_node;
	struct list_head *plist, *phead;

	rtos_critical_enter(RTOS_CRITICAL_WIFI);
	if (rtw_queue_empty(p_queue) == TRUE) {
		p_node = NULL;
	} else {
		phead = get_list_head(p_queue);
		plist = get_next(phead);
		p_node = LIST_CONTAINOR(plist, struct ipc_msg_node, list);
		rtw_list_delete(&(p_node->list));
	}
	rtos_critical_exit(RTOS_CRITICAL_WIFI);
	return p_node;
}

/**
 * @brief  task to operation the queue when the queue is not empty.
 * @param  none
 * @return none
 */
SRAM_WLAN_CRITICAL_CODE_SECTION
void whc_msg_q_task(void)
{
	struct ipc_msg_node *p_node = NULL;
	struct __queue *p_queue = NULL;
	u32	msg_addr;
	u8	wlan_idx;
	u8	event_num;

	p_queue = &g_ipc_msg_q_priv.msg_queue;
	do {
		rtos_sema_take(g_ipc_msg_q_priv.msg_q_sema, RTOS_MAX_TIMEOUT);

		/* get the data from tx queue. */
		while (1) {
			p_node = dequeue_ipc_msg_node(p_queue);
			if (p_node == NULL) {
				break;
			}

			event_num = p_node->event_num;
			msg_addr = p_node->msg_addr;
			wlan_idx = p_node->wlan_idx;

			/* release the memory for this ipc message. */
			rtos_critical_enter(RTOS_CRITICAL_WIFI);
			p_node->is_used = 0;
			rtos_critical_exit(RTOS_CRITICAL_WIFI);

			/* handle the message */
			whc_ipc_host_trx_event_hdl(event_num, msg_addr, wlan_idx);
		}
	} while (g_ipc_msg_q_priv.b_queue_working);
	rtos_task_delete(NULL);
}

/* ---------------------------- Public Functions ---------------------------- */
/**
 * @brief  to initialize the message queue.
 * @param  none
 * @return none
 */
void whc_msg_q_init(void)
{
	int i = 0;

	if (g_ipc_msg_q_priv.ipc_msg_pool) {/*https://jira.realtek.com/browse/RSWLANDIOT-10146*/
		return;
	}

	memset(&g_ipc_msg_q_priv, 0, sizeof(struct ipc_msg_q_priv));
	memset(&g_whc_ipc_ex_msg, 0, sizeof(struct whc_ipc_ex_msg));

	/* initialize queue. */
	rtw_init_queue(&(g_ipc_msg_q_priv.msg_queue));

	/* initialize the sema to wakeup the message queue task */
	rtos_sema_create_static(&g_ipc_msg_q_priv.msg_q_sema, 0, RTOS_MAX_TIMEOUT);
	rtos_mutex_create_static(&g_ipc_msg_q_priv.msg_send_lock);

	g_ipc_msg_q_priv.ipc_msg_node_max = wifi_user_config.skb_num_np;
	g_ipc_msg_q_priv.ipc_msg_pool = (struct ipc_msg_node *)rtos_mem_zmalloc(g_ipc_msg_q_priv.ipc_msg_node_max * sizeof(struct ipc_msg_node));
	for (i = 0; i < g_ipc_msg_q_priv.ipc_msg_node_max; i++) {
		g_ipc_msg_q_priv.ipc_msg_pool[i].is_used = 0;
	}

	/* Initialize the queue task */
#ifndef ZEPHYR_WIFI
	if (RTK_SUCCESS != rtos_task_create(NULL, (const char *const)"whc_msg_q_task", (rtos_task_function_t)whc_msg_q_task, NULL, g_rtw_task_size.ipc_msg_q_task,
										CONFIG_INIC_IPC_MSG_Q_PRI)) {
		RTK_LOGE(TAG_WLAN_INIC, "Create msg_q_task Err!\n");
	}
#else
	k_thread_create(&whc_msg_q_thread, inic_msg_q_stack,
					WIFI_WHC_IPC_MSG_Q_TASK_SIZE,
					(k_thread_entry_t)whc_msg_q_task, NULL, NULL, NULL,
					3, K_USER,
					K_NO_WAIT);
#endif

	/* sign the queue is working */
	g_ipc_msg_q_priv.b_queue_working = 1;
}

/**
 * @brief  put the ipc message to queue.
 * @param  p_ipc_msg[in]: the pointer for the ipc message that need to be
 * 	pushed into the queue.
 * @return status, always RTK_SUCCESS.
 */
SRAM_WLAN_CRITICAL_CODE_SECTION
sint whc_msg_enqueue(struct whc_ipc_ex_msg *p_ipc_msg)
{
	struct ipc_msg_node *p_node = NULL;
	struct __queue *p_queue = &(g_ipc_msg_q_priv.msg_queue);
	sint ret = RTK_FAIL;
	int i = 0;

	rtos_critical_enter(RTOS_CRITICAL_WIFI);
	/* allocate memory for message node */
	for (i = 0; i < g_ipc_msg_q_priv.ipc_msg_node_max; i++) {
		if (g_ipc_msg_q_priv.ipc_msg_pool[i].is_used == 0) {
			p_node = &(g_ipc_msg_q_priv.ipc_msg_pool[i]);
			/* a node is used, the free node will decrease */
			p_node->is_used = 1;
			break;
		}
	}
	rtos_critical_exit(RTOS_CRITICAL_WIFI);

	if (p_node == NULL) {
		RTK_LOGE(TAG_WLAN_INIC, "NO buf for new nodes!\n");
		goto func_out;
	}

	/* To store the ipc message to queue's node. */
	p_node->event_num = p_ipc_msg->event_num;
	p_node->msg_addr = p_ipc_msg->msg_addr;
	p_node->wlan_idx = p_ipc_msg->wlan_idx;

	/* put the ipc message to the queue */
	ret = enqueue_ipc_msg_node(p_node, p_queue);

func_out:
	/* wakeup task */
	rtos_sema_give(g_ipc_msg_q_priv.msg_q_sema);
	return ret;
}

/**
 * @brief  to deinitialize the message queue.
 * @param  none.
 * @return none
 */
void whc_msg_q_deinit(void)
{
	/* sign the queue is stop */
	g_ipc_msg_q_priv.b_queue_working = 0;

	/* free sema to wakeup the message queue task */
	rtos_sema_delete_static(g_ipc_msg_q_priv.msg_q_sema);

	/* de initialize queue, todo */
}

/**
 * @brief  to get the status of queue, working or stop.
 * @param  none.
 * @return the status of queue, 1 means working, 0 means stop.
 */
SRAM_WLAN_CRITICAL_CODE_SECTION
u8 whc_msg_get_queue_status(void)
{
	return g_ipc_msg_q_priv.b_queue_working;
}

/**
 * @brief  to send the ipc message. It will wait when the last message is not
 * 	read.
 * @param  p_ipc_msg[inout]: the message to send.
 * @return none.
 */
SRAM_WLAN_CRITICAL_CODE_SECTION
void whc_ipc_send_msg(u32 event_num, u32 msg_addr, u32 msg_queue_status, u32 wlan_idx)
{
	IPC_MSG_STRUCT g_inic_ipc_msg = {0};

	u32 cnt = 100000;

	rtos_mutex_take(g_ipc_msg_q_priv.msg_send_lock, MUTEX_WAIT_TIMEOUT);

	/* Wait for another port ack acknowledgement last message sending */
	while (g_whc_ipc_ex_msg.event_num != IPC_WIFI_MSG_READ_DONE) {
		DelayUs(2);
		DCache_Invalidate((u32)&g_whc_ipc_ex_msg, sizeof(struct whc_ipc_ex_msg));
		cnt--;
		if (cnt == 0) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "WIFI TRX IPC %d timeout\n", event_num);
			break;
		}
	}
	/* Get the warning of queue's depth not enough after recv MSG_READ_DONE,
	delay send the next message */
	if (g_whc_ipc_ex_msg.msg_queue_status == IPC_WIFI_MSG_MEMORY_NOT_ENOUGH) {
		rtos_time_delay_ms(1);
	}

	/* Send the new message after last one acknowledgement */
	g_whc_ipc_ex_msg.event_num = event_num;
	g_whc_ipc_ex_msg.msg_addr = msg_addr;
	g_whc_ipc_ex_msg.msg_queue_status = msg_queue_status;
	g_whc_ipc_ex_msg.wlan_idx = wlan_idx;
	DCache_Clean((u32)&g_whc_ipc_ex_msg, sizeof(struct whc_ipc_ex_msg));

#ifdef IPC_DIR_MSG_TX
	g_inic_ipc_msg.msg_type = IPC_USER_POINT;
	g_inic_ipc_msg.msg = (u32)&g_whc_ipc_ex_msg;
	g_inic_ipc_msg.msg_len = sizeof(struct whc_ipc_ex_msg);
	ipc_send_message(IPC_DIR_MSG_TX, IPC_INT_CHAN_WIFI_TRX_TRAN, \
					 (PIPC_MSG_STRUCT)&g_inic_ipc_msg);
#else
	ipc_send_message(IPC_INT_CHAN_WIFI_TRX_TRAN, (PIPC_MSG_STRUCT)&g_whc_ipc_ex_msg);
#endif /* IPC_DIR_MSG_TX */

	rtos_mutex_give(g_ipc_msg_q_priv.msg_send_lock);
}
