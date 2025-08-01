#include "ameba_diagnose_types.h"
#include "ameba_diagnose_queue.h"
#include "ameba_diagnose_heap.h"
#include "ameba_diagnose_ring_array.h"

#include "os_wrapper.h"
typedef struct {
	RtkDiagHeapHandler_t *heap_handler;
	RtkDiagRingArrayHandler_t *deleted_evt_arr;
	RtkDiagRingArrayHandler_t *evt_node_arr;
	u16 prev_find_node;
	u32 overwrite_count;
	u16 total_size; //total_size should be equal or less total_capacity
	u16 total_capacity;
} RtkDiagQueueHandler_t;

#define RTK_DIAG_DELETED_EVENT_MAX_COUNT 20
#define INVALID_EVENT_NODE (65535)

static RtkDiagQueueHandler_t *g_handler = NULL;
extern u8 g_diag_debug_log_state;

u32 rtk_diag_element_length_callback(void *data)
{
	RtkDiagEvent_t *event = (RtkDiagEvent_t *)data;
	return sizeof(RtkDiagEvent_t) + event->evt_len;
}

static void rtk_diag_heap_element_remove_callback(void *data)
{
	RtkDiagEvent_t *event = (RtkDiagEvent_t *)data;
	//NOTE: 两种情况会触发该回调:
	//  1. rtk_diag_heap_malloc触发: 小事件的heap满了, 要overwrite event了, 此时需要从queue中将其及其之前的event都删掉, 此时其前面的event理论上应该都是big event
	//  2. rtk_diag_heap_free触发: enqueue时总内存使用达到了total_capacity, 需要持续释放空间直到足够添加新event, 此时是由rtk_diag_heap_free
	//  正常情况下队列中一定有该event, 且该event前面的节点存储的一定是big event
	RTK_LOGA(NOTAG, "Event overwrite\n");
	RtkDiagEvent_t *tmp_evt;
	// RTK_LOGA(NOTAG, "DBG2 %d, %p\n", __LINE__, event);
	while (1) {
		tmp_evt = rtk_diag_ring_array_pop(g_handler->evt_node_arr);
		rtk_diag_ring_array_emplace_back(g_handler->deleted_evt_arr, tmp_evt);
		if (tmp_evt->evt_len > RTK_DIAG_LITTLE_EVENT_THRESHOLD) {
			g_handler->total_size -= RTK_DIAG_EVENT_STRUCTURE_REAL_SIZE(tmp_evt);
			rtos_mem_free(tmp_evt);
		}
		g_handler->overwrite_count++;
		if (tmp_evt == event) {
			break;
		}
	}
}

void rtk_diag_deleted_event_emplace_callback(void *address, const void *data)
{
	const RtkDiagEvent_t *event = (const RtkDiagEvent_t *)data;
	RtkDiagDeletedEvent_t *obj = (RtkDiagDeletedEvent_t *)address;
	obj->evt_type = event->evt_type;
	obj->evt_time = event->evt_time;
	obj->evt_len = event->evt_len;
}

int rtk_diag_queue_init(u16 heap_capacity, u16 total_capacity)
{
	//init queue handler
	g_handler = (RtkDiagQueueHandler_t *)rtos_mem_malloc(sizeof(RtkDiagQueueHandler_t));
	if (NULL == g_handler) {
		RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagQueueHandler_t\n");
		return RTK_ERR_DIAG_MALLOC;
	}

	//init heap handler
	g_handler->heap_handler = rtk_diag_heap_create(heap_capacity,
							  rtk_diag_element_length_callback,
							  rtk_diag_heap_element_remove_callback);
	if (NULL == g_handler->heap_handler) {
		RTK_LOGA(NOTAG, "Failed to init diag heap\n");
		rtos_mem_free(g_handler);
		g_handler = NULL;
		return RTK_ERR_DIAG_UNINIT;
	}

	//init deleted_evt_arr
	g_handler->deleted_evt_arr = rtk_diag_ring_array_create(RTK_DIAG_DELETED_EVENT_MAX_COUNT,
								 sizeof(RtkDiagDeletedEvent_t),
								 rtk_diag_deleted_event_emplace_callback);
	if (NULL == g_handler->deleted_evt_arr) {
		RTK_LOGA(NOTAG, "Failed to init diag ring array\n");
		rtk_diag_heap_destroy(g_handler->heap_handler);
		rtos_mem_free(g_handler);
		g_handler = NULL;
		return RTK_ERR_DIAG_UNINIT;
	}
	//init evt_node_arr
	g_handler->evt_node_arr = rtk_diag_ring_array_create(total_capacity / sizeof(RtkDiagEvent_t),
							  sizeof(RtkDiagEvent_t *), NULL);
	if (NULL == g_handler->evt_node_arr) {
		RTK_LOGA(NOTAG, "Failed to init event node array\n");
		rtk_diag_ring_array_destroy(g_handler->deleted_evt_arr);
		rtk_diag_heap_destroy(g_handler->heap_handler);
		rtos_mem_free(g_handler);
		g_handler = NULL;
		return RTK_ERR_DIAG_UNINIT;
	}

	g_handler->overwrite_count = 0;
	g_handler->total_size = heap_capacity; //小事件的固定heap整个大小计入统计
	g_handler->total_capacity = total_capacity; //事件总内存使用上限
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
	rtk_diag_ring_array_destroy(g_handler->deleted_evt_arr);
	rtk_diag_ring_array_destroy(g_handler->evt_node_arr);
	rtk_diag_heap_destroy(g_handler->heap_handler);
	rtos_mem_free(g_handler);
	g_handler = NULL;
}

int rtk_diag_queue_set_total_capacity(u16 total_capacity)
{
	if (total_capacity > rtk_diag_heap_get_capacity(g_handler->heap_handler) + RTK_DIAG_LITTLE_EVENT_THRESHOLD + sizeof(RtkDiagEvent_t)) {
		//WARNING: MUST larger than fix-sized heap capacity + one big event(smallest)
		RTK_LOGA(NOTAG, "Change total capacity from %u to %u\n", g_handler->total_capacity, total_capacity);
		g_handler->total_capacity = total_capacity;
		return RTK_SUCCESS;
	}
	RTK_LOGA(NOTAG, "Invalid total capacity %u\n", total_capacity);
	return RTK_ERR_BADARG;
}


u16 rtk_diag_queue_get_total_capacity(void)
{
	return g_handler->total_capacity;
}

int rtk_diag_queue_enqueue(u32 timestamp, u8 evt_level, u16 evt_type, const u8 *evt_info, u16 evt_len)
{
	if (g_diag_debug_log_state) {
		RTK_LOGA(NOTAG, "Diag event add: %u, %u, %u\n", timestamp, evt_level, evt_type);
	}

	if (evt_len > RTK_DIAG_BIT_EVENT_THRESHOLD) {
		return RTK_ERR_DIAG_TOO_LARGE_BUFF;
	}

	u32 event_len = sizeof(RtkDiagEvent_t) + evt_len;
	if (evt_len > RTK_DIAG_LITTLE_EVENT_THRESHOLD) {
		//大事件需要检查是否超过允许的最大内存使用空间, 小事件不用检查, 因为其整个heap已经被计算在内了
		RtkDiagEvent_t *tmp_evt = NULL;
		while (rtk_diag_ring_array_size(g_handler->evt_node_arr) > 0 && g_handler->total_size + event_len > g_handler->total_capacity) {
			tmp_evt = (RtkDiagEvent_t *)rtk_diag_ring_array_pop(g_handler->evt_node_arr);
			rtk_diag_ring_array_emplace_back(g_handler->deleted_evt_arr, tmp_evt);
			if (tmp_evt->evt_len <= RTK_DIAG_LITTLE_EVENT_THRESHOLD) {
				rtk_diag_heap_free(g_handler->heap_handler); //release memory in static heap
			} else if (tmp_evt->evt_len <= RTK_DIAG_BIT_EVENT_THRESHOLD) {
				g_handler->total_size -= RTK_DIAG_EVENT_STRUCTURE_REAL_SIZE(tmp_evt);
				rtos_mem_free(tmp_evt);
			} else {
				//WARNING: should not come here!!!
				assert_param(1);
			}
		}
	}

	if (evt_len > RTK_DIAG_LITTLE_EVENT_THRESHOLD && g_handler->total_size + event_len > g_handler->total_capacity) {
		RTK_LOGA(NOTAG, "Failed to add event, sys heap limit: %u + %u > %u\n", g_handler->total_size, event_len, g_handler->total_capacity);
		return RTK_ERR_DIAG_EVT_ADD_FAIL;
	}

	RtkDiagEvent_t *event = NULL;
	//alloc event space in different area depends on evt_len
	if (evt_len <= RTK_DIAG_LITTLE_EVENT_THRESHOLD) {
		event = (RtkDiagEvent_t *)rtk_diag_heap_malloc(g_handler->heap_handler, event_len);
	} else {
		event = (RtkDiagEvent_t *)rtos_mem_malloc(event_len);
	}

	if (NULL == event) {
		RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagEvent_t\n");
		return RTK_ERR_DIAG_MALLOC;
	}
	// RTK_LOGA(NOTAG, "DBG2 %d, [%p]\n", __LINE__, event);

	event->evt_type = evt_type;
	event->evt_len = evt_len;
	event->evt_time = timestamp;
	event->evt_level = evt_level;
	_memcpy(event->evt_info, evt_info, evt_len);
	rtk_diag_ring_array_emplace_back(g_handler->evt_node_arr, event);

	if (evt_len > RTK_DIAG_LITTLE_EVENT_THRESHOLD) {
		g_handler->total_size += event_len;
	}
	return RTK_SUCCESS;
}

RtkDiagEvent_t *rtk_diag_queue_dequeue(void)
{
	if (rtk_diag_ring_array_size(g_handler->evt_node_arr) == 0) {
		return NULL;
	}

	RtkDiagEvent_t *tmp_event = rtk_diag_ring_array_pop(g_handler->evt_node_arr);
	RtkDiagEvent_t *event;
	if (tmp_event->evt_len <= RTK_DIAG_LITTLE_EVENT_THRESHOLD) {
		//NOTE: deep copy when use static heap
		event = (RtkDiagEvent_t *)rtos_mem_malloc(sizeof(RtkDiagEvent_t) + tmp_event->evt_len);
		_memcpy(event, tmp_event, sizeof(RtkDiagEvent_t) + tmp_event->evt_len);
		rtk_diag_heap_free(g_handler->heap_handler); //release memory in static heap
	} else if (event->evt_len <= RTK_DIAG_BIT_EVENT_THRESHOLD) {
		//NOTE: shallow copy when use system heap
	} else {
		//WARNING: should not come here!!!
		assert_param(1);
	}
	return event;
}

int rtk_diag_queue_clear(void)
{
	RtkDiagEvent_t *event;
	while (rtk_diag_ring_array_size(g_handler->evt_node_arr) > 0) {
		event = rtk_diag_ring_array_pop(g_handler->evt_node_arr);
		if (event->evt_len <= RTK_DIAG_LITTLE_EVENT_THRESHOLD) {
			rtk_diag_heap_free(g_handler->heap_handler);
		} else if (event->evt_len <= RTK_DIAG_BIT_EVENT_THRESHOLD) {
			rtos_mem_free(event);
		} else {
			//WARNING: should not come here!!!
			assert_param(1);
		}
	}
	return RTK_SUCCESS;
}

int rtk_diag_queue_del_before(u32 timestamp, u16 *count)
{
	*count = 0;

	RtkDiagEvent_t *tmp_event;
	while (rtk_diag_ring_array_size(g_handler->evt_node_arr) > 0) {
		tmp_event = rtk_diag_ring_array_at(g_handler->evt_node_arr, 0);
		if (tmp_event->evt_time > timestamp) {
			break;
		}
		if (tmp_event->evt_len <= RTK_DIAG_LITTLE_EVENT_THRESHOLD) {
			rtk_diag_heap_free(g_handler->heap_handler);
		} else if (tmp_event->evt_len <= RTK_DIAG_BIT_EVENT_THRESHOLD) {
			rtos_mem_free(tmp_event);
		} else {
			//WARNING: should not come here!!!
			assert_param(1);
		}
		(void)rtk_diag_ring_array_pop(g_handler->evt_node_arr);
		(*count)++;
	}
	return RTK_SUCCESS;
}

int rtk_diag_queue_del_after(u32 timestamp, u16 *count)
{
	UNUSED(timestamp); //WARNING: delete after any tm is difficult for fixed heap, so here just clear all
	*count = 0;
	RtkDiagEvent_t *tmp_event;
	while (rtk_diag_ring_array_size(g_handler->evt_node_arr) > 0) {
		tmp_event = rtk_diag_ring_array_at(g_handler->evt_node_arr, 0);
		if (tmp_event->evt_len <= RTK_DIAG_LITTLE_EVENT_THRESHOLD) {
			rtk_diag_heap_free(g_handler->heap_handler);
		} else if (tmp_event->evt_len <= RTK_DIAG_BIT_EVENT_THRESHOLD) {
			rtos_mem_free(tmp_event);
		} else {
			//WARNING: should not come here!!!
			assert_param(1);
		}
		(void)rtk_diag_ring_array_pop(g_handler->evt_node_arr);
		(*count)++;
	}
	return RTK_SUCCESS;
}

const RtkDiagRingArrayHandler_t *rtk_diag_queue_get_del_list(void)
{
	return g_handler->deleted_evt_arr;
}

int rtk_diag_queue_clr_del_list(void)
{
	return rtk_diag_ring_array_clear(g_handler->deleted_evt_arr);
}

const RtkDiagEvent_t *rtk_diag_queue_view_head(void)
{
	return rtk_diag_ring_array_view(g_handler->evt_node_arr, 0);
}
const RtkDiagEvent_t *rtk_diag_queue_view_tail(void)
{
	return rtk_diag_ring_array_view(g_handler->evt_node_arr, rtk_diag_ring_array_size(g_handler->evt_node_arr) - 1);
}

//入参*offset是timestamp下所有数据拼接起来的offset, 也是数据传输协议中的offset
//函数执行完毕后修改*offset为全局offset下对应到返回event中的offset(局部)
const RtkDiagEvent_t *rtk_diag_queue_find(u32 timestamp, u16 *global_offset, u16 *local_offset, int *result)
{
	*result = RTK_SUCCESS;
	const RtkDiagEvent_t *tmp_event;
	u16 index;
	for (index = 0; index < rtk_diag_ring_array_size(g_handler->evt_node_arr); ++index) {
		tmp_event = rtk_diag_ring_array_view(g_handler->evt_node_arr, index);
		if (tmp_event->evt_time >= timestamp) {
			break;
		}
	}
	g_handler->prev_find_node = index;
	/*--------------------------------------------------*/
	//>>> When: tmp_node->event->evt_time < timestamp
	//请求的timestamp太新(Spec3.3.1, 所有event都已经发送完毕), 没有event满足(或者queue本就是空的)
	if (index == rtk_diag_ring_array_size(g_handler->evt_node_arr)) {
		//timestamp is newer than any event in nodes
		*result = RTK_ERR_DIAG_EVT_NO_MORE;
		g_handler->prev_find_node = INVALID_EVENT_NODE;
		return NULL;
	}
	/*--------------------------------------------------*/

	/*--------------------------------------------------*/
	//>>> When: tmp_node->event->evt_time > timestamp
	//请求的timestamp下所有event都已经被删除或timestamp为0
	if (tmp_event->evt_time > timestamp) {
		//Spec 3.4.4, 3.4.7, Spec 3.4.6 case 2
		*global_offset = 0;
		*local_offset = 0;
		return tmp_event;
	}
	/*--------------------------------------------------*/

	/*--------------------------------------------------*/
	//--- When: tmp_node->event->evt_time == timestamp
	//tmp_node此时是队列中timestamp下的第一个event(最早的一个)
	u16 total_offset = 0;
	//计算目标时间戳(timestamp)下被删除的event的offset总和
	for (u32 i = 0; i < rtk_diag_ring_array_size(g_handler->deleted_evt_arr); ++i) {
		const RtkDiagDeletedEvent_t *del_event = rtk_diag_ring_array_view(g_handler->deleted_evt_arr, i);
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
		return tmp_event;
	} else if (*global_offset == total_offset) {
		//请求的event刚好是timestamp下最头部(最早)的event
		*global_offset = total_offset;
		*local_offset = 0;
		return tmp_event;
	} else {
		//请求的event还在, 根据offset将其找到, 注意global_offset不一定正好卡在event边界上, 因为大event分片传输, 此时global_offset可能在event内部
		u32 tmp_total_offset = total_offset;
		while (index < rtk_diag_ring_array_size(g_handler->evt_node_arr) && tmp_event->evt_time == timestamp) {
			tmp_total_offset = total_offset + RTK_DIAG_EVENT_STRUCTURE_REAL_SIZE(tmp_event);
			if (tmp_total_offset > *global_offset) {
				*local_offset = *global_offset - total_offset; //更新请求的offset在目标event中的分片偏移量
				break;
			}
			total_offset = tmp_total_offset;
			++index;
			tmp_event = rtk_diag_ring_array_view(g_handler->evt_node_arr, index);
		}
		g_handler->prev_find_node = index;

		if (index == rtk_diag_ring_array_size(g_handler->evt_node_arr)) {
			assert_param(*global_offset == total_offset);
			//队列中最尾部(最新)的event正是timestamp, 说明所有event已经发送完毕
			*result = RTK_ERR_DIAG_EVT_NO_MORE;
			g_handler->prev_find_node = INVALID_EVENT_NODE;
			return NULL;
		} else {
			if (tmp_event->evt_time != timestamp) {
				//遍历完timestamp下所有的event还是找不到对应的offset, 此时选择发送下一个timestamp的event
				*global_offset = 0;  //下一个timestamp的中的event肯定还未被删除过, 因为timestamp的event还有在队列中, 肯定是更早的先被删除
				*local_offset = 0;
				return tmp_event;
			} else {
				//找到了真正请求的event:
				// *global_offset 保持原来的值
				// 这里 *local_offset 是在上面的while循环中更新好的
				return tmp_event;
			}
		}
	}
}

const RtkDiagEvent_t *rtk_diag_queue_next_to_prev_find(void)
{
	if (g_handler->prev_find_node != INVALID_EVENT_NODE) {
		++g_handler->prev_find_node;
		return rtk_diag_ring_array_view(g_handler->evt_node_arr, g_handler->prev_find_node);
	} else {
		return NULL;
	}
}
