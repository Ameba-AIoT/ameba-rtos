/*
 *******************************************************************************
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __APP_QUEUE_MGR_H__
#define __APP_QUEUE_MGR_H__


#ifdef __cplusplus
extern "C"  {
#endif

#include <stdio.h>
#include <string.h>
#include <rtk_bt_def.h>
#include <bt_debug.h>

typedef struct {
	int q_write;
	int q_read;
	void *mtx;
	short *queue;
	uint32_t queue_size;//unit: bytes
	uint32_t queue_max_len;
} app_bt_queue_t;

//enqueue_len: unit is bytes
uint16_t app_queue_mgr_enqueue(app_bt_queue_t *p_queue, int8_t *in_buf, uint32_t enqueue_size);
uint16_t app_queue_mgr_dequeue(app_bt_queue_t *p_queue, int8_t *out_buf, uint32_t dequeue_size);
uint16_t app_queue_mgr_flush_queue(app_bt_queue_t *p_queue);
uint16_t app_queue_mgr_init(app_bt_queue_t *p_queue, short *queue, uint32_t queue_len);
uint16_t app_queue_mgr_deinit(app_bt_queue_t *p_queue);

#ifdef __cplusplus
}
#endif

#endif  /* _APP_QUEUE_MGR_H__ */