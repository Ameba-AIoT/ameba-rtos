#include "ameba_diagnose_types.h"
#include "ameba_diagnose_queue.h"
#include "ameba_diagnose_heap.h"
#include "ameba_diagnose_ring_array.h"

#include "os_wrapper.h"

typedef struct tagRtkDiagQueueNode {
	struct tagRtkDiagQueueNode *next;
	RtkDiagEvent_t *event;
} RtkDiagQueueNode_t;

typedef struct {
	rtos_mutex_t mutex;
	RtkDiagHeapHandler_t *heap_handler;
	RtkDiagRingArrayHandler_t *ring_array_handler;
	RtkDiagQueueNode_t *head;
	RtkDiagQueueNode_t *tail;
	RtkDiagQueueNode_t *prev_find_node;
	u32 overwrite_count;
} RtkDiagQueueHandler_t;

#define RTK_DIAG_DELETED_EVENT_MAX_COUNT 20


static RtkDiagQueueHandler_t *g_handler = NULL;

u32 rtk_diag_element_length_callback(void *data)
{
	RtkDiagEvent_t *event = (RtkDiagEvent_t *)data;
	return sizeof(RtkDiagEvent_t) + event->evt_len;
}

void rtk_diag_element_remove_callback(void *data)
{
	RtkDiagEvent_t *event = (RtkDiagEvent_t *)data;
	//NOTE: 删除队列中event之前的的event(理论上应该都是大event, 因为只有数据块要
	//      被覆写时才会执行该回调, 被覆写的永远是最早的, 而queue又是有序的)
	// RTK_LOGA(NOTAG, "Event overwrite\n");
	RtkDiagQueueNode_t *tmp_node;
	while (g_handler->head->event != event) {
		tmp_node = g_handler->head;
		assert_param(tmp_node->event->evt_len > RTK_DIAG_LITTLE_EVENT_THRESHOLD);
		g_handler->head = tmp_node->next;

		//record deleted data
		rtk_diag_ring_array_emplace_back(g_handler->ring_array_handler, tmp_node->event);

		rtos_mem_free(tmp_node->event);
		rtos_mem_free(tmp_node);
		g_handler->overwrite_count++;
	}

	//将该小event的node也删除, 因为其event在heap上的空间即将被回收
	//record deleted data
	rtk_diag_ring_array_emplace_back(g_handler->ring_array_handler, event);
	tmp_node = g_handler->head;
	g_handler->head = tmp_node->next;
	if (g_handler->tail == tmp_node) {
		//要删除的是最后一个node
		g_handler->tail = NULL;
	}
	g_handler->overwrite_count++;
	rtos_mem_free(tmp_node);
}

void rtk_diag_deleted_event_emplace_callback(void *address, const void *data)
{
	const RtkDiagEvent_t *event = (const RtkDiagEvent_t *)data;
	RtkDiagDeletedEvent_t *obj = (RtkDiagDeletedEvent_t *)address;
	obj->evt_type = event->evt_type;
	obj->evt_time = event->evt_time;
	obj->evt_len = event->evt_len;
}

int rtk_diag_queue_init(u16 heap_capacity)
{
	//init queue handler
	g_handler = (RtkDiagQueueHandler_t *)rtos_mem_malloc(sizeof(RtkDiagQueueHandler_t));
	if (NULL == g_handler) {
		RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagQueueHandler_t\n");
		return RTK_ERR_DIAG_MALLOC;
	}

	if (rtos_mutex_create(&g_handler->mutex) != RTK_SUCCESS) {
		RTK_LOGA(NOTAG, "Failed to create mutex for queue\n");
		rtos_mem_free(g_handler);
		g_handler = NULL;
		return RTK_FAIL;
	}

	//init heap handler
	g_handler->heap_handler = rtk_diag_heap_create(heap_capacity,
							  rtk_diag_element_length_callback,
							  rtk_diag_element_remove_callback);
	if (NULL == g_handler->heap_handler) {
		RTK_LOGA(NOTAG, "Failed to init diag heap\n");
		rtos_mutex_delete(g_handler->mutex);
		rtos_mem_free(g_handler);
		g_handler = NULL;
		return RTK_ERR_DIAG_UNINIT;
	}

	//init ring array handler
	g_handler->ring_array_handler = rtk_diag_ring_array_create(RTK_DIAG_DELETED_EVENT_MAX_COUNT,
									sizeof(RtkDiagDeletedEvent_t),
									rtk_diag_deleted_event_emplace_callback);
	if (NULL == g_handler->ring_array_handler) {
		RTK_LOGA(NOTAG, "Failed to init diag ring array\n");
		rtk_diag_heap_destroy(g_handler->heap_handler);
		rtos_mutex_delete(g_handler->mutex);
		rtos_mem_free(g_handler);
		g_handler = NULL;
		return RTK_ERR_DIAG_UNINIT;
	}
	g_handler->head = NULL;
	g_handler->tail = NULL;
	g_handler->overwrite_count = 0;
	return RTK_SUCCESS;
}

void rtk_diag_queue_deinit(void)
{
	if (NULL == g_handler) {
		return;
	}
	RTK_LOGA(NOTAG, "Overwrite %u data totally\n", g_handler->overwrite_count);

	//destroy g_handler
	rtk_diag_queue_clear();
	rtk_diag_ring_array_destroy(g_handler->ring_array_handler);
	rtk_diag_heap_destroy(g_handler->heap_handler);
	rtos_mutex_delete(g_handler->mutex);
	rtos_mem_free(g_handler);
	g_handler = NULL;
}

int rtk_diag_queue_enqueue(u32 timestamp, u8 evt_level, u16 evt_type, const u8 *evt_info, u16 evt_len)
{
	if (rtos_mutex_take(g_handler->mutex, MUTEX_WAIT_TIMEOUT) != RTK_SUCCESS) {
		return RTK_ERR_BUSY;
	}

	RtkDiagQueueNode_t *node = (RtkDiagQueueNode_t *)rtos_mem_malloc(sizeof(RtkDiagQueueNode_t));
	if (NULL == node) {
		RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagQueueNode_t\n");
		rtos_mutex_give(g_handler->mutex);
		return RTK_ERR_DIAG_MALLOC;
	}

	RtkDiagEvent_t *event = NULL;
	//alloc event space in different area depends on evt_len
	u32 frame_len = sizeof(RtkDiagEvent_t) + evt_len;
	if (evt_len <= RTK_DIAG_LITTLE_EVENT_THRESHOLD) {
		event = (RtkDiagEvent_t *)rtk_diag_heap_malloc(g_handler->heap_handler, frame_len);
	} else if (evt_len <= RTK_DIAG_BIT_EVENT_THRESHOLD) {
		event = (RtkDiagEvent_t *)rtos_mem_malloc(frame_len);
	} else {
		rtos_mem_free(node);
		rtos_mutex_give(g_handler->mutex);
		return RTK_ERR_DIAG_TOO_LARGE_BUFF;
	}

	if (NULL == event) {
		RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagEvent_t\n");
		rtos_mem_free(node);
		rtos_mutex_give(g_handler->mutex);
		return RTK_ERR_DIAG_MALLOC;
	}

	event->evt_type = evt_type;
	event->evt_len = evt_len;
	event->evt_time = timestamp;
	event->evt_level = evt_level;
	_memcpy(event->evt_info, evt_info, evt_len);

	node->next = NULL;
	node->event = event;
	if (NULL == g_handler->head) {
		g_handler->head = node;
	}
	if (NULL == g_handler->tail) {
		g_handler->tail = node;
	} else {
		g_handler->tail->next = node;
		g_handler->tail = node;
	}
	rtos_mutex_give(g_handler->mutex);
	RTK_LOGI(NOTAG, "Event add: %u\n", timestamp);
	return RTK_SUCCESS;
}

RtkDiagEvent_t *rtk_diag_queue_dequeue(void)
{
	if (rtos_mutex_take(g_handler->mutex, MUTEX_WAIT_TIMEOUT) != RTK_SUCCESS) {
		return NULL;
	}
	if (NULL == g_handler->head) {
		rtos_mutex_give(g_handler->mutex);
		return NULL;
	}
	RtkDiagQueueNode_t *head = g_handler->head;
	g_handler->head = head->next;

	RtkDiagEvent_t *event = NULL;
	if (head->event->evt_len <= RTK_DIAG_LITTLE_EVENT_THRESHOLD) {
		//NOTE: deep copy when use static heap
		event = (RtkDiagEvent_t *)rtos_mem_malloc(sizeof(RtkDiagEvent_t) + head->event->evt_len);
		_memcpy(event, head->event, sizeof(RtkDiagEvent_t) + head->event->evt_len);
		rtk_diag_heap_free(g_handler->heap_handler); //release memory in static heap
	} else if (head->event->evt_len <= RTK_DIAG_BIT_EVENT_THRESHOLD) {
		//NOTE: shallow copy when use system heap
		event = head->event;
	} else {
		//WARNING: should not come here!!!
		assert_param(1);
	}

	rtos_mem_free(head);
	if (NULL == g_handler->head) {
		g_handler->tail = NULL;
	}
	rtos_mutex_give(g_handler->mutex);
	return event;
}

int rtk_diag_queue_clear(void)
{
	if (rtos_mutex_take(g_handler->mutex, MUTEX_WAIT_TIMEOUT) != RTK_SUCCESS) {
		return RTK_ERR_BUSY;
	}
	RtkDiagQueueNode_t *temp_node;
	while (g_handler->head) {
		if (g_handler->head->event->evt_len <= RTK_DIAG_LITTLE_EVENT_THRESHOLD) {
			rtk_diag_heap_free(g_handler->heap_handler);
		} else if (g_handler->head->event->evt_len <= RTK_DIAG_BIT_EVENT_THRESHOLD) {
			rtos_mem_free(g_handler->head->event);
		} else {
			//WARNING: should not come here!!!
			assert_param(1);
		}
		temp_node = g_handler->head->next;
		rtos_mem_free(g_handler->head);
		g_handler->head = temp_node;
	}
	g_handler->tail = NULL;
	rtos_mutex_give(g_handler->mutex);
	return RTK_SUCCESS;
}

int rtk_diag_queue_del_before(u32 timestamp, u16 *count)
{
	*count = 0;
	if (rtos_mutex_take(g_handler->mutex, MUTEX_WAIT_TIMEOUT) != RTK_SUCCESS) {
		return RTK_ERR_BUSY;
	}

	RtkDiagQueueNode_t *temp_node;
	while (g_handler->head) {
		if (g_handler->head->event->evt_time > timestamp) {
			break;
		}
		if (g_handler->head->event->evt_len <= RTK_DIAG_LITTLE_EVENT_THRESHOLD) {
			rtk_diag_heap_free(g_handler->heap_handler);
		} else if (g_handler->head->event->evt_len <= RTK_DIAG_BIT_EVENT_THRESHOLD) {
			rtos_mem_free(g_handler->head->event);
		} else {
			//WARNING: should not come here!!!
			assert_param(1);
		}
		temp_node = g_handler->head->next;
		rtos_mem_free(g_handler->head);
		(*count)++;
		g_handler->head = temp_node;
	}
	if (NULL == g_handler->head) {
		g_handler->tail = NULL;
	}
	rtos_mutex_give(g_handler->mutex);
	return RTK_SUCCESS;
}

int rtk_diag_queue_del_after(u32 timestamp, u16 *count)
{
	UNUSED(timestamp); //WARNING: delete after any tm is difficult for fixed heap, so here just clear all
	*count = 0;
	if (rtos_mutex_take(g_handler->mutex, MUTEX_WAIT_TIMEOUT) != RTK_SUCCESS) {
		return RTK_ERR_BUSY;
	}
	RtkDiagQueueNode_t *temp_node;
	while (g_handler->head) {
		if (g_handler->head->event->evt_len <= RTK_DIAG_LITTLE_EVENT_THRESHOLD) {
			rtk_diag_heap_free(g_handler->heap_handler);
		} else if (g_handler->head->event->evt_len <= RTK_DIAG_BIT_EVENT_THRESHOLD) {
			rtos_mem_free(g_handler->head->event);
		} else {
			//WARNING: should not come here!!!
			assert_param(1);
		}
		temp_node = g_handler->head->next;
		rtos_mem_free(g_handler->head);
		(*count)++;
		g_handler->head = temp_node;
	}
	g_handler->tail = NULL;
	rtos_mutex_give(g_handler->mutex);
	return RTK_SUCCESS;
}

const RtkDiagRingArrayHandler_t *rtk_diag_queue_get_del_list(void)
{
	return g_handler->ring_array_handler;
}

int rtk_diag_queue_clr_del_list(void)
{
	return rtk_diag_ring_array_clear(g_handler->ring_array_handler);
}

const RtkDiagEvent_t *rtk_diag_queue_view_head(void)
{
	return g_handler->head->event;
}
const RtkDiagEvent_t *rtk_diag_queue_view_tail(void)
{
	return g_handler->tail->event;
}

//入参*offset是timestamp下所有数据拼接起来的offset, 也是数据传输协议中的offset
//函数执行完毕后修改*offset为全局offset下对应到返回event中的offset(局部)
const RtkDiagEvent_t *rtk_diag_queue_find(u32 timestamp, u16 *global_offset, u16 *local_offset, int *result)
{
	RtkDiagQueueNode_t *tmp_node = g_handler->head;
	*result = RTK_SUCCESS;
	while (tmp_node && tmp_node->event->evt_time < timestamp) {
		tmp_node = tmp_node->next;
	}
	g_handler->prev_find_node = tmp_node;

	/*--------------------------------------------------*/
	//>>> When: tmp_node->event->evt_time < timestamp
	//请求的timestamp太新(Spec3.3.1, 所有event都已经发送完毕), 没有event满足(或者event压根就是空的)
	if (NULL == tmp_node) {
		//timestamp is newer than any event in nodes
		*result = RTK_ERR_DIAG_EVT_NO_MORE;
		g_handler->prev_find_node = NULL;
		return NULL;
	}
	/*--------------------------------------------------*/

	/*--------------------------------------------------*/
	//>>> When: tmp_node->event->evt_time > timestamp
	//请求的timestamp下所有event都已经被删除或timestamp为0
	if (tmp_node->event->evt_time > timestamp) {
		//Spec 3.4.4, 3.4.7, Spec 3.4.6 case 2
		*global_offset = 0;
		*local_offset = 0;
		return tmp_node->event;
	}
	/*--------------------------------------------------*/

	/*--------------------------------------------------*/
	//--- When: tmp_node->event->evt_time == timestamp
	//tmp_node此时是队列中timestamp下的第一个event(最早的一个)
	u16 total_offset = 0;
	//计算目标时间戳(timestamp)下被删除的event的offset总和
	for (u32 i = 0; i < rtk_diag_ring_array_size(g_handler->ring_array_handler); ++i) {
		const RtkDiagDeletedEvent_t *del_event = rtk_diag_ring_array_view(g_handler->ring_array_handler, i);
		if (del_event->evt_time == timestamp) {
			total_offset += RTK_DIAG_EVENT_STRUCTURE_REAL_SIZE(del_event);
		} else if (del_event->evt_time > timestamp) {
			//Should not go here
			assert_param(1);
		}
	}

	if (*global_offset < total_offset) {
		//Spec 3.4.6 case 1, spec 3.4.8
		//请求的event已经被删除, 直接上报timestamp下最头部(最早)的event
		*global_offset = total_offset;
		*local_offset = 0;
		return tmp_node->event;
	} else if (*global_offset == total_offset) {
		//请求的event刚好是timestamp下最头部(最早)的event
		*global_offset = total_offset;
		*local_offset = 0;
		return tmp_node->event;
	} else {
		//请求的event还在, 根据offset将其找到, 注意global_offset不一定正好卡在event边界上, 因为大event分片传输, 此时global_offset可能在event内部
		u32 tmp_total_offset = total_offset;
		while (tmp_node && tmp_node->event->evt_time == timestamp) {
			tmp_total_offset = total_offset + RTK_DIAG_EVENT_STRUCTURE_REAL_SIZE(tmp_node->event);
			if (tmp_total_offset > *global_offset) {
				*local_offset = *global_offset - total_offset; //更新请求的offset在目标event中的分片偏移量
				break;
			}
			total_offset = tmp_total_offset;
			tmp_node = tmp_node->next;
		}
		g_handler->prev_find_node = tmp_node;

		if (NULL == tmp_node) {
			assert_param(*global_offset == total_offset);
			//队列中最尾部(最新)的event正是timestamp, 说明所有event已经发送完毕
			*result = RTK_ERR_DIAG_EVT_NO_MORE;
			g_handler->prev_find_node = NULL;
			return NULL;
		} else {
			if (tmp_node->event->evt_time != timestamp) {
				//遍历完timestamp下所有的event还是找不到对应的offset, 此时选择发送下一个timestamp的event
				*global_offset = 0;  //下一个timestamp的中的event肯定还未被删除过, 因为timestamp的event还有在队列中, 肯定是更早的先被删除
				*local_offset = 0;
				return tmp_node->event;
			} else {
				//找到了真正请求的event:
				// *global_offset 保持原来的值
				// 这里 *local_offset 是在上面的while循环中更新好的
				return tmp_node->event;
			}
		}
	}
}

const RtkDiagEvent_t *rtk_diag_queue_next_to_prev_find(void)
{
	if (g_handler->prev_find_node) {
		g_handler->prev_find_node = g_handler->prev_find_node->next;
		return g_handler->prev_find_node ? g_handler->prev_find_node->event : NULL;
	} else {
		return NULL;
	}
}

// #ifdef CONFIG_AMEBADPLUS
// #error remove this part
// #else
// int rtk_diag_queue_enqueue_ts(u8 evt_level, u16 evt_type, const u8 *evt_info, u16 evt_len, u32 timestamp) {
//   RtkDiagQueueNode_t * node = (RtkDiagQueueNode_t *)rtos_mem_malloc(sizeof(RtkDiagQueueNode_t));
//   if (NULL == node) {
//     RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagQueueNode_t\n");
//     return RTK_ERR_DIAG_MALLOC;
//   }

//   RtkDiagEvent_t* event = NULL;
//   //alloc event space in different area depends on evt_len
//   u32 frame_len = sizeof(RtkDiagEvent_t) + evt_len;
//   if (evt_len <= RTK_DIAG_LITTLE_EVENT_THRESHOLD) {
//     event = (RtkDiagEvent_t *)rtk_diag_heap_malloc(g_handler->heap_handler, frame_len);
//   } else if (evt_len <= RTK_DIAG_BIT_EVENT_THRESHOLD) {
//     event = (RtkDiagEvent_t *)rtos_mem_malloc(frame_len);
//   } else {
//     rtos_mem_free(node);
//     return RTK_ERR_DIAG_TOO_LARGE_BUFF;
//   }

//   if (NULL == event) {
//     RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagEvent_t\n");
//     rtos_mem_free(node);
//     return RTK_ERR_DIAG_MALLOC;
//   }

//   event->evt_type = evt_type;
//   event->evt_len = evt_len;
//   event->evt_time = timestamp;
//   event->evt_level = evt_level;
//   _memcpy(event->evt_info, evt_info, evt_len);

//   node->next = NULL;
//   node->event = event;
//   if (NULL == g_handler->head) {
//     g_handler->head = node;
//   }
//   if (NULL == g_handler->tail) {
//     g_handler->tail = node;
//   } else {
//     printf(">>>> %p\n", g_handler->tail);
//     g_handler->tail->next = node;
//     g_handler->tail = node;
//   }
//   return RTK_SUCCESS;
// }
// #endif