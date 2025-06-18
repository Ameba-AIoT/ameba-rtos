/*
*******************************************************************************
* Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <app_queue_mgr.h>

static bool app_bt_queue_mgr_dequeue_size_is_enough(app_bt_queue_t *p_queue, uint32_t dequeue_size)
{
	uint32_t queue_size = 0;

	queue_size = p_queue->queue_size;

	if (queue_size < dequeue_size) {
		return false;
	}

	return true;
}

static bool app_bt_queue_mgr_enqueue_is_full(app_bt_queue_t *p_queue, uint32_t enqueue_size)
{
	uint32_t queue_size = 0;
	uint32_t queue_max_len = 0;

	queue_size = p_queue->queue_size;
	queue_max_len = p_queue->queue_max_len;

	if ((queue_size + enqueue_size) > 2 * queue_max_len) {
		return true;
	}

	return false;

}

static bool app_bt_queue_mgr_queue_is_empty(app_bt_queue_t *p_queue)
{
	if (p_queue->q_write == -1) {
		return true;
	}
	return false;

}

//enqueue_len: unit is bytes
uint16_t app_queue_mgr_enqueue(app_bt_queue_t *p_queue, int8_t *in_buf, uint32_t enqueue_size)
{
	uint32_t p_write = 0;
	uint32_t p_read = 0;
	uint32_t buf_write = 0;
	uint32_t queue_max_len = 0;

	short *p_que = NULL;
	p_que = p_queue->queue;
	queue_max_len = p_queue->queue_max_len;

	if (app_bt_queue_mgr_enqueue_is_full(p_queue, enqueue_size)) {
		BT_LOGE("[APP] %s No enough space! queue size now is %d\r\n", __func__, (int)(p_queue->queue_size));
		return RTK_BT_FAIL;
	}

	//enqueue
	if (app_bt_queue_mgr_queue_is_empty(p_queue)) {
		if (enqueue_size) {
			p_queue->q_write += enqueue_size / 2;
			p_queue->q_read = 0;
			memcpy((void *)p_que, (void *)in_buf, enqueue_size);
			p_queue->queue_size += enqueue_size;
			BT_LOGD("[APP] %s queue is empty!\r\n", __func__);
			return RTK_BT_OK;
		}
	}

	p_write = (p_queue->q_write) % queue_max_len;
	p_read = (p_queue->q_read) % queue_max_len;

	if (p_write >= p_read) {
		if (p_write + enqueue_size / 2 <= queue_max_len - 1) {
			//[p_wite,enqueue_size-1] buf is enough
			memcpy(p_que + (p_write + 1) % queue_max_len, (void *)in_buf, enqueue_size);
		} else {
			//[p_write,enqueue_size-1] buf is not enough
			//[0, p_write]
			uint32_t right_size = 0;
			uint32_t left_size = 0;
			right_size = 2 * (queue_max_len - 1 - p_write);
			left_size = enqueue_size - right_size;

			memcpy(p_que + (p_write + 1) % queue_max_len, in_buf, right_size);
			buf_write += right_size;
			memcpy(p_que, in_buf + buf_write, left_size);
		}
	} else {
		memcpy(p_que + (p_write + 1) % queue_max_len, (void *)in_buf, enqueue_size);
	}
	p_queue->q_write = (p_queue->q_write + enqueue_size / 2) % queue_max_len;
	p_queue->queue_size += enqueue_size;

	BT_LOGD("[APP] %s enqueue %u success! queue_size: %u q_write: %d q_read: %d\r\n", __func__, enqueue_size, p_queue->queue_size, p_queue->q_write,
			p_queue->q_read);
	return RTK_BT_OK;
}

uint16_t app_queue_mgr_dequeue(app_bt_queue_t *p_queue, int8_t *out_buf, uint32_t dequeue_size)
{
	uint32_t p_write = 0;
	uint32_t p_read = 0;
	uint32_t buf_write = 0;
	uint32_t queue_max_len = 0;

	short *p_que = NULL;
	p_que = p_queue->queue;
	queue_max_len = p_queue->queue_max_len;

	if (app_bt_queue_mgr_queue_is_empty(p_queue)) {
		BT_LOGE("[APP] %s dequeue failed !!! queue is empty\r\n", __func__);
		return RTK_BT_FAIL;
	}

	if (!app_bt_queue_mgr_dequeue_size_is_enough(p_queue, dequeue_size)) {
		BT_LOGD("[APP] %s dequeue num is not enough!!!\r\n", __func__);
		return RTK_BT_FAIL;
	}

	if (!out_buf) {
		BT_LOGE("[APP] %s out buf is NULL!\r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (!dequeue_size) {
		BT_LOGE("[APP] %s dequeue_size is 0!\r\n", __func__);
		return RTK_BT_FAIL;
	}

	p_write = (p_queue->q_write) % queue_max_len;
	p_read = (p_queue->q_read) % queue_max_len;

	if (p_write >= p_read) {
		memcpy((void *)out_buf, (void *)(p_que + p_read), dequeue_size);
	} else {
		//2*(queue_max_len - 1 - p_read+1) >= dequeue_size
		if (p_read + dequeue_size / 2 <= queue_max_len) {
			//buf is enough
			memcpy(out_buf, p_que + p_read, dequeue_size);
		} else {
			uint32_t right_size = 0;
			uint32_t left_size = 0;
			right_size = 2 * ((queue_max_len - 1) - p_read + 1);
			left_size = dequeue_size - right_size;
			memcpy(out_buf + buf_write, p_que + p_read, right_size);
			buf_write += right_size;
			memcpy(out_buf + buf_write, (void *)p_que, left_size);
		}
	}
	p_queue->q_read = (p_queue->q_read + dequeue_size / 2) % queue_max_len;
	p_queue->queue_size -= dequeue_size;

	BT_LOGD("[APP] %s dequeue %u success! queue_size: %u q_write: %d q_read: %d\r\n", __func__, dequeue_size, p_queue->queue_size, p_queue->q_write,
			p_queue->q_read);
	return RTK_BT_OK;

}

uint16_t app_queue_mgr_flush_queue(app_bt_queue_t *p_queue)
{
	if (p_queue != NULL) {
		p_queue->q_write = -1;
		p_queue->q_read = -1;
		p_queue->queue_size = 0;
		BT_LOGD("[APP] %s success\r\n", __func__);
		return RTK_BT_OK;
	}
	BT_LOGE("[APP] %s failed \r\n", __func__);
	return RTK_BT_FAIL;
}

uint16_t app_queue_mgr_init(app_bt_queue_t *p_queue, short *queue, uint32_t queue_len)
{
	if (p_queue != NULL) {
		p_queue->q_write = -1;
		p_queue->q_read = -1;
		p_queue->queue = queue;
		p_queue->queue_size = 0;
		p_queue->queue_max_len = queue_len;
		if (p_queue->mtx == NULL) {
			osif_mutex_create(&p_queue->mtx);
		}
		BT_LOGA("[APP] %s queue init success\r\n", __func__);
		return RTK_BT_OK;
	}
	BT_LOGE("[APP] %s queue init failed\r\n", __func__);
	return RTK_BT_FAIL;
}

uint16_t app_queue_mgr_deinit(app_bt_queue_t *p_queue)
{
	if (p_queue != NULL) {
		p_queue->q_write = -1;
		p_queue->q_read = -1;
		p_queue->queue = NULL;
		p_queue->queue_size = 0;
		p_queue->queue_max_len = 0;
		if (p_queue->mtx) {
			osif_mutex_delete(p_queue->mtx);
			p_queue->mtx = NULL;
		}
		BT_LOGA("[APP] %s queue deinit success\r\n", __func__);
		return RTK_BT_OK;
	}
	BT_LOGE("[APP] %s: p_queue is NULL\r\n", __func__);
	return RTK_BT_FAIL;
}