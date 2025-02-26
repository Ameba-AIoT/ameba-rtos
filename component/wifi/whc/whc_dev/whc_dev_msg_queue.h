/******************************************************************************
 *
 * Copyright(c) 2020 - 2021 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __WHC_MSG_QUEUE_H__
#define __WHC_MSG_QUEUE_H__

/* -------------------------- Function declaration -------------------------- */
sint whc_msg_enqueue(void *msg, struct __queue *p_queue);
struct whc_msg_node *whc_msg_dequeue(struct __queue *p_queue);

#endif /* __INIC_MSG_QUEUE_H__ */
