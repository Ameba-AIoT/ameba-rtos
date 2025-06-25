#include "ameba_diagnose_types.h"
#include "ameba_diagnose_heap.h"

#include "os_wrapper.h"

// Diag heap structure
struct tagRtkDiagHeapHandler_t {
	u32 magic;
	u8 hdr_ver;
	u8 rsvd[3];
	u32 build_time;
	u32 capacity;
	u32 head_begin;
	u32 head_end;
	u32 tail_begin;
	u32 tail_end;
	u32 used_size;
	rtk_diag_heap_length_getter_t getter;
	rtk_diag_heap_release_notice_t notice;
	u8 heap_pool[0];  // Flexible array member for heap
} ;

void debug_print(const char *tag, const RtkDiagHeapHandler_t *handler)
{
	RTK_LOGA(NOTAG, "--------------\n");
	RTK_LOGA(NOTAG, "    %s\n", tag);
	RTK_LOGA(NOTAG, "    pool: %p\n", handler->heap_pool);
	RTK_LOGA(NOTAG, "    capacity: %u\n", handler->capacity);
	RTK_LOGA(NOTAG, "    head_begin: %u\n", handler->head_begin);
	RTK_LOGA(NOTAG, "    head_end: %u\n", handler->head_end);
	RTK_LOGA(NOTAG, "    tail_begin: %u\n", handler->tail_begin);
	RTK_LOGA(NOTAG, "    tail_end: %u\n", handler->tail_end);
	RTK_LOGA(NOTAG, "    used_size: %u\n", handler->used_size);
	// RTK_LOGA(NOTAG, "    cavity_start: %u\n", handler->cavity_start);
	RTK_LOGA(NOTAG, "--------------\n");

	if (handler->head_begin == handler->tail_begin) { //head_overlap_tail
		assert_param(handler->used_size == (handler->tail_end - handler->tail_begin));
	} else {
		assert_param(handler->used_size == (handler->tail_end - handler->tail_begin) + (handler->head_end - handler->head_begin));
	}
}

RtkDiagHeapHandler_t *rtk_diag_heap_create(u32 capacity, rtk_diag_heap_length_getter_t getter, rtk_diag_heap_release_notice_t notice)
{
	RtkDiagHeapHandler_t *handler = (RtkDiagHeapHandler_t *)rtos_mem_malloc(sizeof(RtkDiagHeapHandler_t) + capacity);
	if (NULL == handler) {
		RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagHeapHandler_t\n");
		return NULL;
	}

	handler->magic = 0x12345678;
	handler->hdr_ver = 0x01;
	handler->build_time = 0x20250102;
	handler->capacity = capacity;
	handler->head_begin = 0;  //used space's head part's start's offset from begin of heap_pool
	handler->head_end = 0;    //used space's head part's end's offset from begin of heap_pool
	handler->tail_begin = 0;  //used space's tail part's start's offset from begin of heap_pool
	handler->tail_end = 0;    //used space's tail part's end's offset from begin of heap_pool
	handler->used_size = 0;

	handler->getter = getter;
	handler->notice = notice;
	_memset(handler->rsvd, 0, sizeof(handler->rsvd));
	_memset(handler->heap_pool, 0, capacity);

	return handler;
}
int rtk_diag_heap_destroy(RtkDiagHeapHandler_t *handler)
{
	if (handler) {
		rtos_mem_free(handler);
	}
	return RTK_SUCCESS;
}

//头部在前尾部在后(used_start < used_end), 或者初始没有数据的状态(used_start和used_end均为0)
static void *rtk_diag_heap_malloc_normal(RtkDiagHeapHandler_t *handler, u32 length)
{
	void *result = NULL;
	assert_param(handler->head_end == handler->tail_end);
	assert_param(handler->used_size == handler->head_end);
	if (handler->capacity - handler->tail_end >= length) { //尾部有足够的空间
		//Just allocate space from tail of space
		result = handler->heap_pool + handler->tail_end;
		handler->tail_end += length;
		handler->used_size += length;
		if (handler->head_begin == handler->tail_begin) {
			handler->head_end = handler->tail_end;
		}
	} else { //从头部开始申请空间
		u32 free_space = handler->head_begin; //head_begin可能不为0, 数据块可能被主动free过
		u32 current_head_data_length;
		while (free_space < length) {
			if (handler->head_begin < handler->head_end) {
				//head_begin指向一个可以释放的数据块
				handler->notice(handler->heap_pool + handler->head_begin); //通知用户要释放的数据块
				current_head_data_length = handler->getter(handler->heap_pool + handler->head_begin); //获取待释放的数据块大小
				handler->head_begin += current_head_data_length; //指向下一个数据块
				//head_end不变
				handler->used_size -= current_head_data_length;
				free_space += current_head_data_length;
			} else {
				//head_begin进入了空洞块
				assert_param(handler->used_size == 0);
				handler->head_begin = 0;
				handler->head_end = length;
				break;
			}
		}
		handler->tail_begin = 0;
		handler->tail_end = length;
		handler->used_size += length;
		result = handler->heap_pool;
		if (handler->head_begin >= handler->head_end) {
			//上述while中可能在if中恰好申请足够的内存, 但此时head_begin可能进入空洞区了
			assert_param(handler->head_begin == handler->head_end);
			handler->head_begin = 0;
			handler->head_end = handler->tail_end;
		}
	}
	return result;
}

void *rtk_diag_heap_malloc(RtkDiagHeapHandler_t *handler, u32 length)
{
	if (length > handler->capacity) {
		RTK_LOGA(NOTAG, "Failed to allocate memory, greeter than capacity: %u > %u\n", length, handler->capacity);
		return NULL;
	}
	void *result = NULL;

	if (handler->head_begin == handler->tail_begin) {
		/*
		 * heap layout like below(* is data, o is free space, │ is block boundary):
		 *
		 *   head_begin         head_end
		 *       │                  │
		 *       ▼                  ▼
		 *       ┌──┬────┬──────────┬────────┐
		 *       │**│****│**********│oooooooo│
		 *       └──┴────┴──────────┴────────┘
		 *       ▲                  ▲
		 *       │                  │
		 *   tail_begin          tail_begin
		 * or
		 *      head_begin      head_end
		 *          │               │
		 *          ▼               ▼
		 *       ┌──┬────┬──────────┬────────┐
		 *       │oo│****│**********│oooooooo│
		 *       └──┴────┴──────────┴────────┘
		 *          ▲               ▲
		 *          │               │
		 *      tail_begin       tail_begin
		 */
		return rtk_diag_heap_malloc_normal(handler, length);
	} else {
		/*
		 * heap layout like below(* is data, o is free space, │ is block boundary):
		 *
		 *               head_begin  head_end
		 *                     │        │
		 *                     ▼        ▼
		 *      ┌─┬─┬──┬───────┬─┬───┬──┬───┐
		 *      │*│*│**│ooooooo│*│***│**│ooo│
		 *      └─┴─┴──┴───────┴─┴───┴──┴───┘
		 *      ▲      ▲
		 *      │      │
		 * tail_begin tail_end
		 */
		assert_param(handler->tail_begin == 0);
		assert_param(handler->tail_end > handler->tail_begin);
		assert_param(handler->head_begin >= handler->tail_end);
		assert_param(handler->head_end > handler->head_begin);
		assert_param(handler->capacity >= handler->head_end);
		assert_param(handler->used_size == (handler->tail_end - handler->tail_begin) + (handler->head_end - handler->head_begin));

		if (handler->head_begin - handler->tail_end >= length) { //tail到head之间的区域足够本次申请
			result = handler->heap_pool + handler->tail_end;
			handler->tail_end += length;
			handler->used_size += length;
			//tail_begin, head_begin, head_end保持变化
		} else { //tail到head之间的区域不够本次申请, 需要从head_begin开始释放空间
			u32 free_space = handler->head_begin - handler->tail_end;
			u32 current_head_data_length;
			while (free_space < length) {
				if (handler->head_begin < handler->head_end) { //head_begin指向数据块时则将其释放
					handler->notice(handler->heap_pool + handler->head_begin); //通知用户要释放的数据块
					current_head_data_length = handler->getter(handler->heap_pool + handler->head_begin); //获取待释放的数据块大小
					handler->head_begin += current_head_data_length; //指向下一个数据块
					handler->used_size -= current_head_data_length;
					free_space += current_head_data_length;
				} else {
					//尝试剩余的空洞空间
					free_space += handler->capacity - handler->head_end;
					handler->head_begin = 0;
					handler->head_end = handler->tail_end;
					if (free_space >= length) { //足够则开始分配
						break;
					} else { //仍然不够则进入第一种状态
						return rtk_diag_heap_malloc_normal(handler, length);
					}
				}
			}
			result = handler->heap_pool + handler->tail_end;
			handler->tail_end += length;
			handler->used_size += length;

			if (handler->head_begin == 0) {
				handler->head_end = handler->tail_end;
			} else if (handler->head_begin >= handler->head_end) {
				//上述while中可能在if中恰好申请足够的内存, 但此时head_begin可能进入空洞区了
				assert_param(handler->head_begin == handler->head_end);
				handler->head_begin = 0;
				handler->head_end = handler->tail_end;
			}
		}
	}
	return result;
}

u32 rtk_diag_heap_free(RtkDiagHeapHandler_t *handler)
{
	u32 current_head_data_length = 0;
	if (handler->used_size > 0) {
		u8 is_head_overlap_tail = handler->head_begin == handler->tail_begin;
		// handler->notice(handler->heap_pool + handler->head_begin); //通知用户要释放的数据块
		current_head_data_length = handler->getter(handler->heap_pool + handler->head_begin); //获取待释放的数据块大小
		handler->used_size -= current_head_data_length;
		handler->head_begin += current_head_data_length; //指向下一个数据块
		if (handler->head_begin >= handler->head_end) {
			//指向空洞数据块
			assert_param(handler->head_begin == handler->head_end);
			if (is_head_overlap_tail) {
				handler->head_begin = 0;
				handler->head_end = 0;
				handler->tail_begin = 0;
				handler->tail_end = 0;
			} else {
				handler->head_begin = handler->tail_begin;
				handler->head_end = handler->tail_end;
			}
		} else if (is_head_overlap_tail) { //更新tail_begin
			handler->tail_begin = handler->head_begin;
		}

	}
	return current_head_data_length;
}


u32 rtk_diag_heap_get_capacity(const RtkDiagHeapHandler_t *handler)
{
	return handler->capacity;
}