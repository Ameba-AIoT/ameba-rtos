#define __INIC_MSG_QUEUE_C__

#include <rtw_cfg80211_fullmac.h>

static int enqueue_msg_node(struct msg_priv_t *priv, struct inic_msg_node *p_node)
{
	spin_lock(&(priv->lock));
	list_add_tail(&(p_node->list), &(priv->queue_head));
	atomic_inc(&priv->msg_num);
	spin_unlock(&(priv->lock));

	return 0;
}

static struct inic_msg_node *dequeue_msg_node(struct msg_priv_t *priv)
{
	struct inic_msg_node *p_node;
	struct list_head *plist, *phead;

	/* stop interrupt interrupting this process to cause dead lock. */
	spin_lock_irq(&(priv->lock));

	if (list_empty(&(priv->queue_head)) == true) {
		p_node = NULL;
	} else {
		phead = &(priv->queue_head);
		plist = phead->next;
		p_node = list_entry(plist, struct inic_msg_node, list);
		list_del(&(p_node->list));
		atomic_dec(&priv->msg_num);
	}

	spin_unlock_irq(&(priv->lock));

	return p_node;
}

static void inic_msg_q_task(struct work_struct *data)
{
	struct msg_priv_t *priv;
	struct inic_msg_node *p_node = NULL;

	priv = container_of(data, struct msg_priv_t, msg_work);

	mutex_lock(&(priv->msg_work_mutex));
	if (priv->b_queue_working) {
		/* get the data from tx queue. */
		while (1) {
			p_node = dequeue_msg_node(priv);
			if (p_node == NULL) {
				break;
			}
			/* haddle the message */
			if (priv->task_hdl) {
				priv->task_hdl(p_node->msg);
			}

			/* release the memory for this message. */
			kfree(p_node);
		}
	}
	mutex_unlock(&(priv->msg_work_mutex));
}

int inic_msg_q_init(struct msg_priv_t *priv, void (*task_hdl)(void *))
{
	/* initialize queue. */
	spin_lock_init(&priv->lock);
	INIT_LIST_HEAD(&priv->queue_head);
	atomic_set(&priv->msg_num, 0);

	/* assign the haddle function for the task */
	priv->task_hdl = task_hdl;

	mutex_init(&(priv->msg_work_mutex));
	/* initialize message tasklet */
	INIT_WORK(&(priv->msg_work), inic_msg_q_task);

	/* sign the queue is working */
	priv->b_queue_working = 1;

	return 0;
}

int inic_msg_enqueue(struct msg_priv_t *priv, void *msg)
{
	struct inic_msg_node *p_node = NULL;
	int ret = 0;

	/* allocate memory for message node */
	p_node = kzalloc(sizeof(struct inic_msg_node), GFP_KERNEL);
	p_node ->msg = msg;

	/* put the message to the queue */
	ret = enqueue_msg_node(priv, p_node);

	if (global_idev.wowlan_state == 0) {
		/* wakeup task */
		schedule_work(&(priv->msg_work));
	}

	return ret;
}

void inic_msg_q_deinit(struct msg_priv_t *priv)
{
	/* sign the queue is stop */
	priv->b_queue_working = 0;

	/* assign the haddle function to NULL */
	priv->task_hdl = NULL;

	mutex_destroy(&(priv->msg_work_mutex));
}

