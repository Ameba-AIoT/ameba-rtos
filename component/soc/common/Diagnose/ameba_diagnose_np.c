#include "ameba_diagnose.h"
#include "ameba_diagnose_queue.h"
#include "ameba_diagnose_protocol.h"
#include "ameba_diagnose_transform.h"

#include "os_wrapper.h"

static u8 g_initialized = 0;

static const RtkDiagEvent_t *rtk_diag_next_event_callback(void)
{
	return rtk_diag_queue_next_to_prev_find();
}

int rtk_diag_init(u16 heap_capacity, u16 sender_buffer_size)
{
	if (g_initialized == 1) {
		RTK_LOGA(NOTAG, "Diagnose already initialized\n");
		return RTK_SUCCESS;
	}

	int res = rtk_diag_queue_init(heap_capacity);
	if (res) {
		RTK_LOGA(NOTAG, "Diagnose queue initialize failed\n");
		return res;
	}

	res = rtk_diag_proto_init(sender_buffer_size, rtk_diag_next_event_callback);
	if (res) {
		RTK_LOGA(NOTAG, "Diagnose transform initialize failed\n");
		rtk_diag_queue_deinit();
		return res;
	}
	RTK_LOGA(NOTAG, "Diag np init\n");

	g_initialized = 1;
	return RTK_SUCCESS;
}

void rtk_diag_deinit(void)
{
	if (g_initialized == 0) {
		RTK_LOGA(NOTAG, "Diagnose not initialized yet\n");
		return ;
	}
	g_initialized = 0;
	rtk_diag_proto_deinit();
	rtk_diag_queue_deinit();
}

int rtk_diag_event_add(u8 evt_level, u16 evt_type, const u8 *evt_info, u16 evt_len)
{
	/*Disable IPC_event interrupt to avoid km4 IPC interruption when adding tasks*/
	// RTK_LOGA(NOTAG, "Diagnose event add: %u, %u, %s\n", evt_level, evt_type, (const char*)evt_info);
	IPC_INTConfig(DIAG_IPC_DEV, DIAG_IPC_TX_CHANNEL_SHIFT + IPC_A2N_EVENT_REQ, DISABLE);
	int res = rtk_diag_queue_enqueue(rtos_time_get_current_system_time_ms(), evt_level, evt_type, evt_info, evt_len);
	if (res) {
		RTK_LOGA(NOTAG, "Diagnose event add failed: %d\n", res);
	}
	/*Enable IPC_event interrupt*/
	IPC_INTConfig(DIAG_IPC_DEV, DIAG_IPC_TX_CHANNEL_SHIFT + IPC_A2N_EVENT_REQ, ENABLE);
	return res;
}

#ifndef CONFIG_AMEBAD
static int rtk_diag_event_add_ts(u32 timestamp, u8 evt_level, u16 evt_type, const u8 *evt_info, u16 evt_len)
{
	/*Disable IPC_event interrupt to avoid km4 IPC interruption when adding tasks*/
	// RTK_LOGA(NOTAG, "Diagnose event add: %u, %u, %s\n", evt_level, evt_type, (const char*)evt_info);
	IPC_INTConfig(DIAG_IPC_DEV, DIAG_IPC_TX_CHANNEL_SHIFT + IPC_A2N_EVENT_REQ, DISABLE);
	int res = rtk_diag_queue_enqueue(timestamp, evt_level, evt_type, evt_info, evt_len);
	if (res) {
		RTK_LOGA(NOTAG, "Diagnose event add failed: %d\n", res);
	}
	/*Enable IPC_event interrupt*/
	IPC_INTConfig(DIAG_IPC_DEV, DIAG_IPC_TX_CHANNEL_SHIFT + IPC_A2N_EVENT_REQ, ENABLE);
	return res;
}
#endif

const RtkDiagDataFrame_t *rtk_diag_get_event_as_frame(u32 timestamp, u16 offset)
{
#ifndef CONFIG_AMEBAD
	if (IPC_SEMTake(IPC_SEM_DIAGNOSE, 1000) == FALSE) {
		return NULL;
	}
#endif
	u16 global_offset = offset;
	u16 local_offset = 0;
	int result;
	const RtkDiagEvent_t *event = rtk_diag_queue_find(timestamp, &global_offset, &local_offset, &result);
	if (NULL == event) {
#ifndef CONFIG_AMEBAD
		IPC_SEMFree(IPC_SEM_DIAGNOSE);
#endif
		return NULL;
	}
	const RtkDiagDataFrame_t *pack = rtk_diag_proto_pack_data(event, global_offset, local_offset);
#ifndef CONFIG_AMEBAD
	IPC_SEMFree(IPC_SEM_DIAGNOSE);
#endif
	return pack;
}

static int rtk_diag_req_low(u8 cmd, const u8 *payload, u16 payload_length)
{
	RtkDiagDataFrame_t *frame = (RtkDiagDataFrame_t *)rtos_mem_malloc(sizeof(RtkDiagDataFrame_t) + payload_length + 1); //include crc
	if (NULL == frame) {
		RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagDataFrame_t\n");
		const RtkDiagDataFrame_t *frame = rtk_diag_proto_pack_error(cmd, RTK_ERR_DIAG_MALLOC);
		rtk_diag_uart_send(frame);
		return RTK_ERR_DIAG_MALLOC;
	}
	frame->header = RTK_DIAG_FRAME_HEADER;
	frame->size = payload_length + 1;
	frame->cmd = cmd;
	_memcpy(frame->payload, payload, payload_length);
	u8 check_sum = frame->cmd;
	for (int i = 0; i < frame->size - 1; i++) {
		check_sum += frame->payload[i];
	}
	frame->payload[frame->size - 1] = check_sum;
	int res = rtk_diag_uart_send(frame);
	rtos_mem_free(frame);
	return res;
}

int rtk_diag_req_timestamp(void)
{
	u32 tm = rtos_time_get_current_system_time_ms();
	return rtk_diag_req_low(0x01, (u8 *)&tm, sizeof(tm));
}

int rtk_diag_req_version(void)
{
	return rtk_diag_req_low(0x05, (u8 *)"9FC60C4CB6162E49C54FB94511497E16", 32);
}

int rtk_diag_req_set_buf_com_capacity(u16 capacity)
{
	UNUSED(capacity);
	return RTK_SUCCESS;
}

int rtk_diag_req_set_buf_evt_capacity(u16 capacity)
{
	UNUSED(capacity);
	return RTK_SUCCESS;
}

int rtk_diag_req_set_buf_evt_pool_capacity(u16 capacity)
{
	UNUSED(capacity);
	return RTK_SUCCESS;
}

int rtk_diag_req_get_event(u32 timestamp, u16 offset)
{
	/*1. Get hardware ipc sema*/
#ifndef CONFIG_AMEBAD
	if (IPC_SEMTake(IPC_SEM_DIAGNOSE, 1000) == FALSE) {
		rtk_diag_uart_send(rtk_diag_proto_pack_error(RTK_DIAG_CMD_TYPE_READ, RTK_ERR_DIAG_SEM_FAIL));
		return RTK_ERR_DIAG_SEM_FAIL;
	}
#endif
	/*2. Find event*/
	u16 global_offset = offset;
	u16 local_offset = 0;
	int result;
	const RtkDiagEvent_t *event = rtk_diag_queue_find(timestamp, &global_offset, &local_offset, &result);
	if (NULL == event && result != RTK_ERR_DIAG_EVT_NO_MORE) {
		RTK_LOGA(NOTAG, "Find event {%u, %u} failed: %u, %u\n", timestamp, offset, global_offset, local_offset);
		rtk_diag_uart_send(rtk_diag_proto_pack_error(RTK_DIAG_CMD_TYPE_READ, RTK_ERR_DIAG_EVT_FIND_FAIL));
#ifndef CONFIG_AMEBAD
		IPC_SEMFree(IPC_SEM_DIAGNOSE);
#endif
		return RTK_FAIL;
	}
	// RTK_LOGA(NOTAG, "!!! Find event %p, %p {%u, %u}: %u, %u\n", event, event->evt_info,timestamp, offset, global_offset, local_offset);

	RTK_LOGA(NOTAG, "pack event: %u, %u, %p\n", global_offset, local_offset, event);
	/*3. Package event as protocol*/
	const RtkDiagDataFrame_t *frame = rtk_diag_proto_pack_data(event, global_offset, local_offset);
	if (NULL == frame) {
		RTK_LOGA(NOTAG, "Package event {%u, %u} failed\n", timestamp, offset);
		rtk_diag_uart_send(rtk_diag_proto_pack_error(RTK_DIAG_CMD_TYPE_READ, RTK_ERR_DIAG_EVT_FIND_FAIL));
	}

	/*4. Send event*/
	int res = rtk_diag_uart_send(frame);
	RTK_LOGA(NOTAG, "req: [%u|%u], send event: fsize: %u, evt_ts: %u, evt_full_sz: %u, res: %d\n", timestamp, offset, frame->size, event->evt_time,
			 RTK_DIAG_EVENT_STRUCTURE_REAL_SIZE(event), res);
#ifndef CONFIG_AMEBAD
	IPC_SEMFree(IPC_SEM_DIAGNOSE);
#endif
	return res;
}

int rtk_diag_req_del_event_before(u32 timestamp)
{
	u16 count = 0;
	rtk_diag_queue_del_before(timestamp, &count);
	return rtk_diag_req_low(RTK_DIAG_CMD_TYPE_DEL_BF, (u8 *)&count, 2);
}

int rtk_diag_req_del_event_after(u32 timestamp)
{
	u16 count = 0;
	rtk_diag_queue_del_after(timestamp, &count);
	return rtk_diag_req_low(RTK_DIAG_CMD_TYPE_DEL_AF, (u8 *)&count, 2);
}

int rtk_diag_req_get_del_events(void)
{
	const RtkDiagRingArrayHandler_t *list = rtk_diag_queue_get_del_list();
	u32 count = rtk_diag_ring_array_size(list);
	u16 payload_length = count * (sizeof(u16) + sizeof(u32)) + 1; //include crc
	RtkDiagDataFrame_t *frame = (RtkDiagDataFrame_t *)rtos_mem_malloc(sizeof(RtkDiagDataFrame_t) + payload_length);
	if (NULL == frame) {
		RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagDataFrame_t\n");
		const RtkDiagDataFrame_t *frame = rtk_diag_proto_pack_error(0x06, RTK_ERR_DIAG_MALLOC);
		rtk_diag_uart_send(frame);
		return RTK_ERR_DIAG_MALLOC;
	}
	frame->header = RTK_DIAG_FRAME_HEADER;
	frame->size = payload_length;
	frame->cmd = 0x06;
	const RtkDiagDeletedEvent_t *del_event;
	for (u32 i = 0; i < count; ++i) {
		del_event = rtk_diag_ring_array_view(list, i);
		_memcpy(frame->payload + i * (sizeof(u16) + sizeof(u32)), &del_event->evt_type, sizeof(u16));
		_memcpy(frame->payload + i * (sizeof(u16) + sizeof(u32)) + sizeof(u16), &del_event->evt_time, sizeof(u32));
	}

	u8 check_sum = frame->cmd;
	for (int i = 0; i < frame->size - 1; i++) {
		check_sum += frame->payload[i];
	}
	RTK_LOGA(NOTAG, "del count: %u, %u, %u\n", count, payload_length, check_sum);
	frame->payload[frame->size - 1] = check_sum;
	int res = rtk_diag_uart_send(frame);
	rtos_mem_free(frame);
	return res;
}

int rtk_diag_req_clr_del_events(void)
{
	int res = rtk_diag_queue_clr_del_list();
	const RtkDiagDataFrame_t *frame = rtk_diag_proto_pack_error(0x07, (u8)res);
	return rtk_diag_uart_send(frame);
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
#ifndef CONFIG_AMEBAD
static void rtk_diag_ipc_recv(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);
	PIPC_MSG_STRUCT ipc_event_msg = ipc_get_message(DIAG_IPC_DIR, IPC_A2N_EVENT_REQ);
	RtkDiagIpcMsg_t *diag_msg = (RtkDiagIpcMsg_t *)ipc_event_msg->msg;
	DCache_Invalidate(diag_msg->addr, diag_msg->size);

	RTK_LOGA(NOTAG, "np ipc, addr = %u, type = %u\n", diag_msg->addr, diag_msg->type);
	if (diag_msg->type == RTK_DIAG_IPC_MSG_TYPE_ATCMD) {
		//atcmd
		RtkDiagAtCmd_t *atcmd = (RtkDiagAtCmd_t *)diag_msg->addr;
		switch (atcmd->type) {
		case RTK_DIAG_CMD_TYPE_BUFFER:
			diag_msg->flag = rtk_diag_req_set_buf_com_capacity(atcmd->capacity);
			break;
		case RTK_DIAG_CMD_TYPE_READ:
			diag_msg->flag = rtk_diag_req_get_event(atcmd->timestamp, atcmd->offset);
			break;
		case RTK_DIAG_CMD_TYPE_HEAP:
			diag_msg->flag = rtk_diag_req_set_buf_evt_capacity(atcmd->capacity);
			break;
		case RTK_DIAG_CMD_TYPE_VER:
			diag_msg->flag = rtk_diag_req_version();
			break;
		case RTK_DIAG_CMD_TYPE_GET_DEL:
			diag_msg->flag = rtk_diag_req_get_del_events();
			break;
		case RTK_DIAG_CMD_TYPE_CLR_DEL:
			diag_msg->flag = rtk_diag_req_clr_del_events();
			break;
		case RTK_DIAG_CMD_TYPE_DEL_BF:
			diag_msg->flag = rtk_diag_req_del_event_before(atcmd->timestamp);
			break;
		case RTK_DIAG_CMD_TYPE_DEL_AF:
			diag_msg->flag = rtk_diag_req_del_event_after(atcmd->timestamp);
			break;
		case RTK_DIAG_CMD_TYPE_SET_LOG:
			diag_msg->flag = rtk_diag_req_log_enable(atcmd->log);
			break;
		default:
			RTK_LOGA(NOTAG, "np ipc unsupport atcmd type: %u\n", atcmd->type);
			diag_msg->flag = RTK_ERR_BADARG;
			break;
		}
	} else if (diag_msg->type == RTK_DIAG_IPC_MSG_TYPE_EVT_ADD) {
		RtkDiagEvent_t *event = (RtkDiagEvent_t *)diag_msg->addr;
		diag_msg->flag = rtk_diag_event_add_ts(event->evt_time, event->evt_level, event->evt_type, event->evt_info, event->evt_len);
	} else {
		RTK_LOGA(NOTAG, "np ipc unsupport type: %u\n", diag_msg->type);
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
	.IPC_Channel = IPC_A2N_EVENT_REQ
};
#endif
