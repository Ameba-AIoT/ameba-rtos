/******************************************************************************
 *
 * Copyright(c) 2020 - 2021 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __INIC_MSG_QUEUE_H__
#define __INIC_MSG_QUEUE_H__

/* -------------------------- Function declaration -------------------------- */
sint inic_msg_enqueue(void *msg, struct __queue *p_queue);
struct inic_msg_node *inic_msg_dequeue(struct __queue *p_queue);

#endif /* __INIC_MSG_QUEUE_H__ */
