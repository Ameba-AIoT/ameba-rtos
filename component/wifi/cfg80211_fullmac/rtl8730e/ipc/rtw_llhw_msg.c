// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#define __INIC_IPC_MSG_QUEUE_C__

#include <rtw_cfg80211_fullmac.h>

static int enqueue_ipc_msg_node(struct ipc_msg_q_priv *priv, struct ipc_msg_node *p_node)
{
	spin_lock(&(priv->lock));
	list_add_tail(&(p_node->list), &(priv->queue_head));
	spin_unlock(&(priv->lock));

	return 0;
}

static struct ipc_msg_node *dequeue_ipc_msg_node(struct ipc_msg_q_priv *priv)
{
	struct ipc_msg_node *p_node;
	struct list_head *plist, *phead;

	/* stop ipc interrupt interrupting this process to cause dead lock. */
	spin_lock_irq(&(priv->lock));

	if (list_empty(&(priv->queue_head)) == true) {
		p_node = NULL;
	} else {
		phead = &(priv->queue_head);
		plist = phead->next;
		p_node = list_entry(plist, struct ipc_msg_node, list);
		list_del(&(p_node->list));
	}

	spin_unlock_irq(&(priv->lock));

	return p_node;
}

static void inic_msg_q_task(struct work_struct *data)
{
	struct ipc_msg_q_priv *priv = &global_idev.msg_priv;
	struct ipc_msg_node *p_node = NULL;

	mutex_lock(&(priv->msg_work_lock));
	if (priv->b_queue_working) {
		/* get the data from tx queue. */
		while (1) {
			p_node = dequeue_ipc_msg_node(priv);
			if (p_node == NULL) {
				break;
			}
			/* haddle the message */
			if (priv->task_hdl) {
				priv->task_hdl(p_node->event_num, p_node->msg_addr, p_node->wlan_idx);
			}

			spin_lock_irq(&(priv->lock));
			/* release the memory for this ipc message. */
			p_node->is_used = 0;
			priv->queue_free++;
			spin_unlock_irq(&(priv->lock));
		}
	}
	mutex_unlock(&(priv->msg_work_lock));
}

int inic_msg_q_init(struct device *pdev, void (*task_hdl)(u8 event_num, u32 msg_addr, u8 wlan_idx))
{
	int i = 0;
	struct ipc_msg_q_priv *msg_priv = &global_idev.msg_priv;

	/* initialize queue. */
	spin_lock_init(&msg_priv->lock);
	spin_lock_init(&msg_priv->ipc_send_msg_lock);
	INIT_LIST_HEAD(&msg_priv->queue_head);

	msg_priv->p_inic_ipc_msg = dmam_alloc_coherent(pdev, sizeof(struct inic_ipc_ex_msg), &msg_priv->ipc_msg_phy_addr, GFP_KERNEL);
	if (!msg_priv->p_inic_ipc_msg) {
		dev_dbg(global_idev.fullmac_dev, "%s: allloc p_inic_ipc_msg error.\n", __func__);
		return -ENOMEM;
	}

	/* assign the haddle function for the task */
	msg_priv->task_hdl = task_hdl;

	for (i = 0; i < IPC_MSG_QUEUE_DEPTH; i++) {
		msg_priv->ipc_msg_pool[i].is_used = 0;
	}
	msg_priv->queue_free = IPC_MSG_QUEUE_DEPTH;

	mutex_init(&(msg_priv->msg_work_lock));
	/* initialize message tasklet */
	INIT_WORK(&(msg_priv->msg_work), inic_msg_q_task);

	/* sign the queue is working */
	msg_priv->b_queue_working = 1;

	return 0;
}

int inic_msg_enqueue(struct inic_ipc_ex_msg *p_ipc_msg)
{
	struct ipc_msg_q_priv *priv = &global_idev.msg_priv;
	struct ipc_msg_node *p_node = NULL;
	int ret = 0, i = 0;

	/* allocate memory for message node */
	spin_lock(&(priv->lock));
	for (i = 0; i < IPC_MSG_QUEUE_DEPTH; i++) {
		if (priv->ipc_msg_pool[i].is_used == 0) {
			p_node = &(priv->ipc_msg_pool[i]);
			/* a node is used, the free node will decrease */
			p_node->is_used = 1;
			priv->queue_free--;
			break;
		}
	}
	spin_unlock(&(priv->lock));

	if (p_node == NULL) {
		ret = -ENOMEM;
		dev_err(global_idev.fullmac_dev, "[AP] NO buffer for new nodes, waiting!\n\r");
		goto func_out;
	}

	/* To store the ipc message to queue's node. */
	p_node->event_num = p_ipc_msg->event_num;
	p_node->msg_addr = p_ipc_msg->msg_addr;
	p_node->wlan_idx = p_ipc_msg->wlan_idx;

	/* put the ipc message to the queue */
	ret = enqueue_ipc_msg_node(priv, p_node);

	/* the free number of nodes is smaller than the warning depth. */
	if (priv->queue_free <= IPC_MSG_QUEUE_WARNING_DEPTH) {
		/* ask peer to wait */
		ret = -EBUSY;
	}

func_out:
	if (global_idev.wowlan_state == 0) {
		/* wakeup task */
		schedule_work(&(priv->msg_work));
	}

	return ret;
}

void inic_msg_q_deinit(void)
{
	struct ipc_msg_q_priv *msg_priv = &global_idev.msg_priv;

	/* sign the queue is stop */
	msg_priv->b_queue_working = 0;

	/* assign the haddle function to NULL */
	msg_priv->task_hdl = NULL;

	/* free sema to wakeup the message queue task */

	dma_free_coherent(global_idev.ipc_dev, sizeof(struct inic_ipc_ex_msg), msg_priv->p_inic_ipc_msg, msg_priv->ipc_msg_phy_addr);

	mutex_destroy(&(msg_priv->msg_work_lock));
}
