#include "ameba_diagnose_types.h"
#include "ameba_diagnose_protocol.h"

#include "os_wrapper.h"

enum {
	RTK_DIAG_TRANSFORM_COMPLETE = 0,
	RTK_DIAG_TRANSFORM_ONGOING,
};

typedef struct {
	RtkDiagDataFrame_t *data_frame;
	RtkDiagDataFrame_t *error_frame;
	rtk_diag_queue_next_event_getter_t getter;
	u16 payload_capacity;
	u16 event_capacity; //==payload_capacity - sizeof(offset):2 - sizeof(crc):1

	const RtkDiagEvent_t *last_event;
	u8 last_event_state;
	u16 last_event_offset; //上个event分片传输的偏移量
} RtkDiagTransformHandler_t;

static RtkDiagTransformHandler_t *g_handler = NULL;

static int rtk_diag_proto_load_data_empty(void)
{
	g_handler->data_frame->cmd = 0x03;
	g_handler->data_frame->size = 1;
	return RTK_SUCCESS;
}

static int rtk_diag_proto_load_data_from_start(const RtkDiagEvent_t *event, u16 local_offset)
{
	assert_param(local_offset == 0);
#if 1
	if (g_handler->last_event_state == RTK_DIAG_TRANSFORM_ONGOING) {
		//此时设备端没有记录待发送的数据
		if (g_handler->last_event == event) {
			//还是传上一个event, 可能是第一片数据丢了
		} else {
			//和上次的event不一样, 可能是上次的分片event被删除了
		}
	}
#endif
	u16 event_size = RTK_DIAG_EVENT_STRUCTURE_REAL_SIZE(event);
	u16 total_event_size = 0;
	if (event_size > g_handler->event_capacity) {
		//大于buffer容量的事件分片传输
		g_handler->data_frame->cmd = 0x03;
		total_event_size = g_handler->event_capacity; //loads using whole buffer
		g_handler->last_event = event;
		g_handler->last_event_offset = g_handler->event_capacity;
		g_handler->last_event_state = RTK_DIAG_TRANSFORM_ONGOING;
		_memcpy(g_handler->data_frame->payload + 2, (void *)event, g_handler->event_capacity);
	} else {
		//小于buffer容量的事件尝试一次性传输多个
		g_handler->last_event_offset = 0;
		g_handler->last_event_state = RTK_DIAG_TRANSFORM_COMPLETE;
		g_handler->data_frame->cmd = 0x03;
		while (event) {
			event_size = RTK_DIAG_EVENT_STRUCTURE_REAL_SIZE(event);
			if (event_size + total_event_size > g_handler->event_capacity) {
				break;
			}
			g_handler->last_event = event;
			_memcpy(g_handler->data_frame->payload + 2 + total_event_size, (void *)event, event_size);
			RTK_LOGA(NOTAG, "load evt: %p, %u, %u, %u\n", g_handler->data_frame->payload + 2 + total_event_size, event->evt_time, event_size, event->evt_len);
			total_event_size += event_size;
			event = g_handler->getter();
		}
	}
	g_handler->data_frame->size = total_event_size + 2 + 1; //add offset and cmd
	return RTK_SUCCESS;
}

static int rtk_diag_proto_load_data_from_middle(const RtkDiagEvent_t *event, u16 local_offset)
{
	assert_param(local_offset != 0);
#if 1
	if (g_handler->last_event_state == RTK_DIAG_TRANSFORM_COMPLETE) {
		if (g_handler->last_event == event) {
			//重传某一片数据
			RTK_LOGA(NOTAG, "Maybe data lost, retransform from offset: %u\n", local_offset);
		} else {
			//重传的片和上次记录的event不一样? 不应该进入这里
			assert_param(1);
		}
	}
#endif
	u16 total_event_size = 0;
	g_handler->last_event = event;
	u16 event_size = RTK_DIAG_EVENT_STRUCTURE_REAL_SIZE(event);
	if (event_size - local_offset > g_handler->event_capacity) {
		//需要继续分片
		g_handler->data_frame->cmd = 0x13;
		g_handler->last_event_state = RTK_DIAG_TRANSFORM_ONGOING;
		g_handler->last_event_offset = local_offset;
		total_event_size = g_handler->event_capacity;
	} else {
		//一次性发完
		g_handler->data_frame->cmd = 0x23;
		g_handler->last_event_state = RTK_DIAG_TRANSFORM_COMPLETE;
		g_handler->last_event_offset = 0;
		total_event_size = event_size - local_offset;
	}
	_memcpy(g_handler->data_frame->payload + 2, (u8 *)event + local_offset, total_event_size);
	g_handler->data_frame->size = total_event_size + 2 + 1; //add offset and cmd
	return RTK_SUCCESS;
}

int rtk_diag_proto_init(u16 payload_capacity, rtk_diag_queue_next_event_getter_t getter)
{
	//init queue handler
	if (payload_capacity < RTK_DIAG_SEND_BUFFER_SIZE_MIN) {
		RTK_LOGA(NOTAG, "Protocol buffer capacity must larger than %u, now is %u\n", RTK_DIAG_SEND_BUFFER_SIZE_MIN, payload_capacity);
		return RTK_ERR_DIAG_TOO_SMALL_BUFF;
	}

	g_handler = (RtkDiagTransformHandler_t *)rtos_mem_malloc(sizeof(RtkDiagTransformHandler_t));
	if (NULL == g_handler) {
		RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagTransformHandler_t\n");
		return RTK_ERR_DIAG_MALLOC;
	}
	g_handler->data_frame = (RtkDiagDataFrame_t *)rtos_mem_malloc(sizeof(RtkDiagDataFrame_t) + payload_capacity);
	if (NULL == g_handler->data_frame) {
		RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagDataFrame_t\n");
		rtos_mem_free(g_handler);
		return RTK_ERR_DIAG_MALLOC;
	}
	g_handler->error_frame = (RtkDiagDataFrame_t *)rtos_mem_malloc(sizeof(RtkDiagDataFrame_t) + 2); //2 is 1bytes error and 1 byte crc
	if (NULL == g_handler->error_frame) {
		RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagDataFrame_t\n");
		rtos_mem_free(g_handler->data_frame);
		rtos_mem_free(g_handler);
		return RTK_ERR_DIAG_MALLOC;
	}

	g_handler->data_frame->header = RTK_DIAG_FRAME_HEADER;

	g_handler->error_frame->header = RTK_DIAG_FRAME_HEADER;
	g_handler->error_frame->size = 2;
	g_handler->error_frame->cmd = 0x0;
	g_handler->error_frame->payload[0] = 0;

	g_handler->getter = getter;
	g_handler->payload_capacity = payload_capacity;
	g_handler->event_capacity = payload_capacity - 2 - 1;//==payload_capacity - sizeof(offset):2 - sizeof(crc):1

	g_handler->last_event = NULL;
	g_handler->last_event_state = RTK_DIAG_TRANSFORM_COMPLETE;
	return RTK_SUCCESS;
}

void rtk_diag_proto_deinit(void)
{
	if (g_handler) {
		rtos_mem_free(g_handler->data_frame);
		rtos_mem_free(g_handler->error_frame);
		rtos_mem_free(g_handler);
		g_handler = NULL;
	}
}

int rtk_diag_proto_set_capacity(u16 payload_capacity)
{
	if (NULL == g_handler) {
		return RTK_ERR_DIAG_UNINIT;
	}

	if (payload_capacity == g_handler->payload_capacity) {
		return RTK_SUCCESS;
	}

	if (payload_capacity < RTK_DIAG_SEND_BUFFER_SIZE_MIN) {
		RTK_LOGA(NOTAG, "Protocol buffer capacity must larger than %u, now is %u\n", RTK_DIAG_SEND_BUFFER_SIZE_MIN, payload_capacity);
		return RTK_ERR_DIAG_TOO_SMALL_BUFF;
	}

	RtkDiagDataFrame_t *new_frame = (RtkDiagDataFrame_t *)rtos_mem_malloc(sizeof(RtkDiagDataFrame_t) + payload_capacity);
	if (NULL == new_frame) {
		RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagDataFrame_t\n");
		return RTK_ERR_DIAG_MALLOC;
	}
	rtos_mem_free(g_handler->data_frame);
	g_handler->data_frame = new_frame;
	g_handler->data_frame->header = RTK_DIAG_FRAME_HEADER;
	g_handler->payload_capacity = payload_capacity;
	g_handler->event_capacity = payload_capacity - 2 - 1;//==payload_capacity - sizeof(offset):2 - sizeof(crc):1
	return RTK_SUCCESS;
}

u16 rtk_diag_proto_get_capacity(void)
{
	return g_handler ? g_handler->payload_capacity : 0;
}

const RtkDiagDataFrame_t *rtk_diag_proto_pack_error(u8 cmd_type, u8 error)
{
	if (NULL == g_handler) {
		return NULL;
	}
	if (error) {
		g_handler->error_frame->cmd = BIT7 | cmd_type;
	} else {
		g_handler->error_frame->cmd = (~BIT7) & cmd_type;
	}
	g_handler->error_frame->payload[0] = error;
	g_handler->error_frame->payload[1] = error ^ g_handler->error_frame->cmd; //calculate crc
	return g_handler->error_frame;
}

const RtkDiagDataFrame_t *rtk_diag_proto_pack_data(const RtkDiagEvent_t *event, u16 global_offset, u16 local_offset)
{
	if (NULL == g_handler) {
		return NULL;
	}

	if (event == NULL) {
		//pack an empty frame indicate all event are transferred
		rtk_diag_proto_load_data_empty();
	} else {
		if (local_offset == 0) {
			rtk_diag_proto_load_data_from_start(event, local_offset);
		} else {
			rtk_diag_proto_load_data_from_middle(event, local_offset);
		}
		_memcpy(g_handler->data_frame->payload, &global_offset, sizeof(u16));
		// *((u16*)g_handler->data_frame->payload) = global_offset;
	}
	u8 check_sum = g_handler->data_frame->cmd;
	for (int i = 0; i < g_handler->data_frame->size - 1; i++) {
		check_sum ^= g_handler->data_frame->payload[i];
	}
	g_handler->data_frame->payload[g_handler->data_frame->size - 1] = check_sum; //set crc

	return g_handler->data_frame;
}
