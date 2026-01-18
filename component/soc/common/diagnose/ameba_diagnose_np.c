/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_diagnose.h"
#include "ameba_diagnose_queue.h"
#include "ameba_diagnose_protocol.h"
#include "ameba_diagnose_transform.h"

#include "os_wrapper.h"

#ifdef DIAG_CORE_AS_AP
rtk_diag_sender_t g_diag_sender = NULL;
#endif
RtkDiagDataFrame_t *g_using_frame_buffer = NULL;
u16 g_using_frame_buffer_capacity = 0;

static u8 g_initialized = 0;
u8 g_diag_debug_log_state;

static const RtkDiagEvent_t *rtk_diag_next_event_callback(void)
{
	return rtk_diag_queue_next_to_prev_find();
}

static inline u32 rtk_diag_lock_acquire(u8 cmd_type)
{
#ifdef DIAG_CORE_AS_AP
	UNUSED(cmd_type);
	return 1;
#else
	u32 res = IPC_SEMTake(IPC_SEM_DIAGNOSE, 0);
	if (!res && cmd_type != RTK_DIAG_CMD_TYPE_NONE) {
		rtk_diag_req_low(cmd_type, NULL, 0, RTK_ERR_BUSY);
	}
	return res;
#endif
}

static inline void rtk_diag_lock_release(void)
{
#ifdef DIAG_CORE_AS_AP
#else
	IPC_SEMFree(IPC_SEM_DIAGNOSE);
#endif
}

int rtk_diag_init(u16 heap_capacity, u16 sender_buffer_size)
{
#ifdef DIAG_DEBUG_TASK
	void rtk_diag_debug_create_task(void);
	rtk_diag_debug_create_task();
#endif
	if (g_initialized == 1) {
		return RTK_SUCCESS;
	}

	int res = rtk_diag_queue_init(heap_capacity, RTK_DIAG_SYS_HEAP_UPPER_LIMIT);
	if (res) {
		RTK_LOGA("DIAG", "queue init failed\n");
		return res;
	}

	res = rtk_diag_proto_init(rtk_diag_next_event_callback);
	if (res) {
		RTK_LOGA("DIAG", "transform init failed\n");
		rtk_diag_queue_deinit();
		return res;
	}

	UNUSED(sender_buffer_size);

	g_initialized = 1;
	g_diag_debug_log_state = DISABLE;
	return RTK_SUCCESS;
}

void rtk_diag_deinit(void)
{
	if (g_initialized == 0) {
		return ;
	}
	g_initialized = 0;
	rtk_diag_proto_deinit();
	rtk_diag_queue_deinit();
}

#ifdef DIAG_CORE_AS_AP
int rtk_diag_config_transform(rtk_diag_sender_t sender, u8 *sender_buffer, u16 sender_buffer_size)
{
	if (!rtk_diag_lock_acquire(RTK_DIAG_CMD_TYPE_NONE)) {
		return RTK_ERR_BUSY;
	}
	g_diag_sender = sender == NULL ? rtk_diag_uart_send : sender;

	if (sender_buffer) {
		if (sender_buffer_size < RTK_DIAG_SEND_BUFFER_SIZE_MIN) {
			RTK_LOGA("DIAG", "buf cap(%u) must > %u\n", sender_buffer_size, RTK_DIAG_SEND_BUFFER_SIZE_MIN);
			return RTK_ERR_DIAG_TOO_SMALL_BUFF;
		}
		if ((RtkDiagDataFrame_t *)sender_buffer == g_using_frame_buffer && g_using_frame_buffer_capacity == sender_buffer_size) {
			return RTK_SUCCESS;
		}
		g_using_frame_buffer = (RtkDiagDataFrame_t *)sender_buffer;
		g_using_frame_buffer_capacity = sender_buffer_size;
	}
	rtk_diag_proto_set_buffer(g_using_frame_buffer, g_using_frame_buffer_capacity);
	rtk_diag_lock_release();
	return RTK_SUCCESS;
}

bool rtk_diag_transform_has_configed(void)
{
	return g_diag_sender && g_using_frame_buffer;
}
#endif

int rtk_diag_event_add(u8 evt_level, u16 evt_type, const u8 *evt_info, u16 evt_len)
{
	// RTK_LOGA("DIAG", "evt add: %u, %u, %s\n", evt_level, evt_type, (const char*)evt_info);
	if (!rtk_diag_lock_acquire(RTK_DIAG_CMD_TYPE_NONE)) {
		return RTK_ERR_BUSY;
	}
	int res = rtk_diag_queue_enqueue(rtos_time_get_current_system_time_ms(), evt_level, evt_type, evt_info, evt_len);
	if (res) {
		RTK_LOGA("DIAG", "evt add failed: %d\n", res);
	}
	rtk_diag_lock_release();
	return res;
}

int rtk_diag_req_add_event(u32 timestamp, u8 evt_level, u16 evt_type, const u8 *evt_info, u16 evt_len)
{
	if (!rtk_diag_lock_acquire(RTK_DIAG_CMD_TYPE_READ)) {
		return RTK_ERR_BUSY;
	}
	int res = rtk_diag_queue_enqueue(timestamp, evt_level, evt_type, evt_info, evt_len);
	if (res) {
		RTK_LOGA("DIAG", "evt add failed: %d\n", res);
	}
	rtk_diag_lock_release();
	return res;
}

#ifdef DIAG_CORE_AS_AP
int rtk_diag_req_timestamp(void)
{
	u32 tm = rtos_time_get_current_system_time_ms();
	return rtk_diag_req_low(RTK_DIAG_CMD_TYPE_OSTIME, (u8 *)&tm, sizeof(tm), 0);
}

int rtk_diag_req_version(void)
{
	extern const char *g_rtk_diag_format_hash;
	return rtk_diag_req_low(RTK_DIAG_CMD_TYPE_VER, (u8 *)g_rtk_diag_format_hash, strlen(g_rtk_diag_format_hash), 0);
}

int rtk_diag_req_set_buf_com_capacity(u8 *sender_buffer, u16 capacity)
{
	if (!rtk_diag_lock_acquire(RTK_DIAG_CMD_TYPE_BUFFER)) {
		return RTK_ERR_BUSY;
	}

	int ret = RTK_SUCCESS;
	if (sender_buffer) {
		if (g_using_frame_buffer != (RtkDiagDataFrame_t *)sender_buffer || g_using_frame_buffer_capacity != capacity) {
			g_using_frame_buffer = (RtkDiagDataFrame_t *)sender_buffer;
			g_using_frame_buffer_capacity = capacity;
		}
		rtk_diag_req_low(RTK_DIAG_CMD_TYPE_BUFFER, (u8 *)&capacity, sizeof(capacity), 0);
	} else {
		ret = capacity < RTK_DIAG_SEND_BUFFER_SIZE_MIN ? RTK_ERR_DIAG_TOO_SMALL_BUFF : RTK_ERR_DIAG_MALLOC;
		rtk_diag_req_low(RTK_DIAG_CMD_TYPE_BUFFER, NULL, 0, ret);
	}

	rtk_diag_lock_release();
	return ret;
}
#endif
int rtk_diag_req_set_buf_evt_capacity(u16 capacity)
{
	if (!rtk_diag_lock_acquire(RTK_DIAG_CMD_TYPE_HEAP)) {
		return RTK_ERR_BUSY;
	}
	rtk_diag_queue_set_total_capacity(capacity);
	capacity = rtk_diag_queue_get_total_capacity();
	int ret = rtk_diag_req_low(RTK_DIAG_CMD_TYPE_HEAP, (u8 *)&capacity, sizeof(capacity), 0);
	rtk_diag_lock_release();
	return ret;
}

int rtk_diag_req_get_event(u32 timestamp, u16 offset)
{
	/*1. Get hardware ipc sema*/
	if (!rtk_diag_lock_acquire(RTK_DIAG_CMD_TYPE_READ)) {
		return RTK_ERR_BUSY;
	}
	/*2. Find event*/
	u16 global_offset = offset;
	u16 local_offset = 0;
	int result;
	const RtkDiagEvent_t *event = rtk_diag_queue_find(timestamp, &global_offset, &local_offset, &result);
	if (NULL == event && result != RTK_ERR_DIAG_EVT_NO_MORE) {
		RTK_LOGA("DIAG", "Find evt {%u, %u} failed: %u, %u\n", timestamp, offset, global_offset, local_offset);
		rtk_diag_req_low(RTK_DIAG_CMD_TYPE_READ, NULL, 0, RTK_ERR_DIAG_EVT_FIND_FAIL);
		rtk_diag_lock_release();
		return RTK_FAIL;
	}
	// RTK_LOGA("DIAG", "!!! Find event %p, %p {%u, %u}: %u, %u\n", event, event->evt_info,timestamp, offset, global_offset, local_offset);

	// RTK_LOGA("DIAG", "pack event: %u, %u, %p\n", global_offset, local_offset, event);
	/*3. Package event as protocol*/
	rtk_diag_proto_pack_event(event, global_offset, local_offset);
	if (NULL == g_using_frame_buffer) {
		RTK_LOGA("DIAG", "pkg evt {%u, %u} failed\n", timestamp, offset);
		rtk_diag_req_low(RTK_DIAG_CMD_TYPE_READ, NULL, 0, RTK_ERR_DIAG_EVT_FIND_FAIL);
		return RTK_FAIL;
	}

	/*4. Send event*/
	int res = RTK_SUCCESS;
#ifdef DIAG_CORE_AS_AP
	res = g_diag_sender((u8 *)g_using_frame_buffer, RTK_DIAG_FRAME_STRUCTURE_REAL_SIZE(g_using_frame_buffer));
#endif
#ifdef DIAG_DEBUG_TEST
	if (event) {
		RTK_LOGA("DIAG", "req: [%u|%u], send evt: fsize: %u, evt_ts: %u, evt_full_sz: %u, res: %d\n", timestamp, offset, g_using_frame_buffer->size, event->evt_time,
				 RTK_DIAG_EVENT_STRUCTURE_REAL_SIZE(event), res);
	} else {
		RTK_LOGA("DIAG", "req: [%u|%u], send evt: fsize: %u, no more event, res: %d\n", timestamp, offset, g_using_frame_buffer->size, res);
	}
#endif
	rtk_diag_lock_release();
	return res;
}

int rtk_diag_req_del_event_before(u32 timestamp)
{
	if (!rtk_diag_lock_acquire(RTK_DIAG_CMD_TYPE_DEL_BF)) {
		return RTK_ERR_BUSY;
	}
	u16 count = 0;
	rtk_diag_queue_del_before(timestamp, &count);
	int ret = rtk_diag_req_low(RTK_DIAG_CMD_TYPE_DEL_BF, (u8 *)&count, 2, 0);
	rtk_diag_lock_release();
	return ret;
}

int rtk_diag_req_del_event_after(u32 timestamp)
{
	if (!rtk_diag_lock_acquire(RTK_DIAG_CMD_TYPE_DEL_AF)) {
		return RTK_ERR_BUSY;
	}
	u16 count = 0;
	rtk_diag_queue_del_after(timestamp, &count);
	int ret = rtk_diag_req_low(RTK_DIAG_CMD_TYPE_DEL_AF, (u8 *)&count, 2, 0);
	rtk_diag_lock_release();
	return ret;
}

int rtk_diag_req_get_del_events(void)
{
	if (!rtk_diag_lock_acquire(RTK_DIAG_CMD_TYPE_GET_DEL)) {
		return RTK_ERR_BUSY;
	}
	const RtkDiagRingArrayHandler_t *list = rtk_diag_queue_get_del_list();
	u32 count = rtk_diag_ring_array_size(list);
	u16 payload_length = count * (sizeof(u16) + sizeof(u32)) + 1; //include crc
	if (sizeof(RtkDiagDataFrame_t) + payload_length > g_using_frame_buffer_capacity) {
		count = (g_using_frame_buffer_capacity - sizeof(RtkDiagDataFrame_t) - 1) / (sizeof(u16) + sizeof(u32));
		payload_length = count * (sizeof(u16) + sizeof(u32)) + 1; //include crc
	}

	g_using_frame_buffer->header = RTK_DIAG_FRAME_HEADER;
	g_using_frame_buffer->size = payload_length;
	g_using_frame_buffer->cmd = RTK_DIAG_CMD_TYPE_GET_DEL;
	const RtkDiagDeletedEvent_t *del_event;
	for (u32 i = 0; i < count; ++i) {
		del_event = rtk_diag_ring_array_view(list, i);
		_memcpy(g_using_frame_buffer->payload + i * (sizeof(u16) + sizeof(u32)), &del_event->evt_type, sizeof(u16));
		_memcpy(g_using_frame_buffer->payload + i * (sizeof(u16) + sizeof(u32)) + sizeof(u16), &del_event->evt_time, sizeof(u32));
	}

	rtk_diag_proto_pack_crc(g_using_frame_buffer);
	int res = RTK_SUCCESS;
#ifdef DIAG_CORE_AS_AP
	res = g_diag_sender((u8 *)g_using_frame_buffer, RTK_DIAG_FRAME_STRUCTURE_REAL_SIZE(g_using_frame_buffer));
#endif
	rtk_diag_lock_release();
	return res;
}

int rtk_diag_req_clr_del_events(void)
{
	if (!rtk_diag_lock_acquire(RTK_DIAG_CMD_TYPE_CLR_DEL)) {
		return RTK_ERR_BUSY;
	}
	rtk_diag_queue_clr_del_list();
	int ret = rtk_diag_req_low(RTK_DIAG_CMD_TYPE_CLR_DEL, (u8 *)&ret, 1, 0);
	rtk_diag_lock_release();
	return ret;
}

int rtk_diag_req_log_enable(u8 state)
{
	if (state) {
		rtk_log_level_set("*", RTK_LOG_INFO);
	} else {
		rtk_log_level_set("*", RTK_LOG_NONE);
	}
	return RTK_SUCCESS;
}

#ifndef DIAG_CORE_AS_AP
static void rtk_diag_ipc_recv(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);
	PIPC_MSG_STRUCT ipc_event_msg = ipc_get_message(DIAG_IPC_DIR, DIAG_IPC_CHANNEL);
	RtkDiagIpcMsg_t *diag_msg = (RtkDiagIpcMsg_t *)ipc_event_msg->msg;
	DCache_Invalidate(diag_msg->addr, diag_msg->size);

	if (diag_msg->type == RTK_DIAG_IPC_MSG_TYPE_CONFIG) {
		g_using_frame_buffer = (RtkDiagDataFrame_t *)diag_msg->addr;
		g_using_frame_buffer_capacity = diag_msg->size;
		rtk_diag_proto_set_buffer(g_using_frame_buffer, g_using_frame_buffer_capacity);
		diag_msg->flag = RTK_SUCCESS;
	} else if (diag_msg->type == RTK_DIAG_IPC_MSG_TYPE_ATCMD) {
		//atcmd
		diag_msg->flag = RTK_DIAG_IPC_WAIT_RESPONSE;
		RtkDiagAtCmd_t *atcmd = (RtkDiagAtCmd_t *)diag_msg->addr;
		switch (atcmd->type) {
		case RTK_DIAG_CMD_TYPE_READ:
			rtk_diag_req_get_event(atcmd->timestamp, atcmd->offset);
			break;
		case RTK_DIAG_CMD_TYPE_HEAP:
			rtk_diag_req_set_buf_evt_capacity(atcmd->capacity);
			break;
		case RTK_DIAG_CMD_TYPE_GET_DEL:
			rtk_diag_req_get_del_events();
			break;
		case RTK_DIAG_CMD_TYPE_CLR_DEL:
			rtk_diag_req_clr_del_events();
			break;
		case RTK_DIAG_CMD_TYPE_DEL_BF:
			rtk_diag_req_del_event_before(atcmd->timestamp);
			break;
		case RTK_DIAG_CMD_TYPE_DEL_AF:
			diag_msg->flag = rtk_diag_req_del_event_after(atcmd->timestamp);
			break;
		case RTK_DIAG_CMD_TYPE_SET_LOG:
			diag_msg->flag = rtk_diag_req_log_enable(atcmd->log);
			break;
#ifdef DIAG_DEBUG_TEST
		case RTK_DIAG_CMD_TYPE_SET_DBG_LOG:
			if (atcmd->log == 0 || atcmd->log == 1) {
				diag_msg->flag = rtk_diag_req_dbg_log_enable(atcmd->log);
			} else {
				extern u8 g_diag_trans_print;
				g_diag_trans_print = atcmd->log == 10 ? 0 : 1;
				diag_msg->flag = RTK_SUCCESS;
			}
			diag_msg->flag = rtk_diag_req_dbg_log_enable(atcmd->log);
			break;
#endif
		default:
			RTK_LOGA("DIAG", "np unsupport atcmd type: %u\n", atcmd->type);
			diag_msg->flag = RTK_ERR_BADARG;
			break;
		}
		if (diag_msg->flag == RTK_DIAG_IPC_WAIT_RESPONSE) {
			diag_msg->flag = RTK_DIAG_IPC_DATA_READY;
			DCache_Clean((u32)g_using_frame_buffer, RTK_DIAG_FRAME_STRUCTURE_REAL_SIZE(g_using_frame_buffer));
		}
	} else if (diag_msg->type == RTK_DIAG_IPC_MSG_TYPE_EVT_ADD) {
		RtkDiagEvent_t *event = (RtkDiagEvent_t *)diag_msg->addr;
		diag_msg->flag = rtk_diag_req_add_event(event->evt_time, event->evt_level, event->evt_type, event->evt_info, event->evt_len);
	} else {
		diag_msg->flag = RTK_ERR_BADARG;
	}
	DCache_CleanInvalidate((u32)diag_msg, sizeof(RtkDiagIpcMsg_t));
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_event_table = {
	.USER_MSG_TYPE = IPC_USER_DATA,
	.Rxfunc = rtk_diag_ipc_recv,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = DIAG_IPC_DIR,
	.IPC_Channel = DIAG_IPC_CHANNEL
};
#endif

#ifdef DIAG_DEBUG_TEST
#if defined(CONFIG_WHC_HOST) || defined (CONFIG_WHC_NONE)
//used in ameba_diagnose_debug.c
int rtk_diag_req_add_event_demo1(u8 evt_level, const char *data)
{
	struct diag_evt_demo_1 demo = {
		.u8_data = 1,
		.u16_data = 2,
		.f_data = 1.1,
		.ch_data = 'r',
	};
	strncpy(demo.str_data, data, 31);
	return rtk_diag_event_add(evt_level, RTK_EVENT_TYPE_DBG_DEMO_1, (const u8 *)&demo, sizeof(struct diag_evt_demo_1) - (32 - (strlen(data) + 1)));
}
#endif
int rtk_diag_req_dbg_log_enable(u8 state)
{
	g_diag_debug_log_state = state;
	return RTK_SUCCESS;
}
#endif
