#include "ameba_diagnose.h"
#include "ameba_diagnose_transform.h"

#include "os_wrapper.h"

static u8 g_initialized = 0;
static rtos_mutex_t g_ipc_mutex = NULL;
static RtkDiagAtCmd_t g_at_cmd;

SRAM_NOCACHE_DATA_SECTION
static volatile RtkDiagIpcMsg_t g_diag_ipc_msg = {0};

int rtk_diag_init(void)
{
#ifdef DIAG_DEBUG_TEST
	void rtk_diag_debug_create_task(void);
	rtk_diag_debug_create_task();
#endif
	if (g_initialized == 1) {
		RTK_LOGA(NOTAG, "Diagnose already initialized\n");
		return RTK_SUCCESS;
	}

	if (rtos_mutex_create(&g_ipc_mutex) != RTK_SUCCESS) {
		RTK_LOGA(NOTAG, "Failed to create queue for receiving events from AP\n");
		return RTK_FAIL;
	}

	return RTK_SUCCESS;
}

void rtk_diag_deinit(void)
{
	if (g_initialized == 0) {
		RTK_LOGA(NOTAG, "Diagnose already deinitialized\n");
		return;
	}
	rtos_mutex_delete(g_ipc_mutex);
	g_ipc_mutex = NULL;
	g_initialized = 0;
	return;
}

static int rtk_diag_ipc_status_get(void)
{
	return g_diag_ipc_msg.flag;
}

static void rtk_diag_ipc_status_set(int flag)
{
	// __disable_irq();
	g_diag_ipc_msg.flag = flag;
	// __enable_irq();
}

static int rtk_diag_ipc_send(u32 addr, u32 size, u8 type)
{
	if (rtos_mutex_take(g_ipc_mutex, MUTEX_WAIT_TIMEOUT) != RTK_SUCCESS) {
		return RTK_ERR_BUSY;
	}

	DCache_Clean(addr, size);

	g_diag_ipc_msg.addr = addr;
	g_diag_ipc_msg.size = size;
	g_diag_ipc_msg.type = type;
	RTK_LOGA(NOTAG, "ap ipc: addr = %x, size = %d\n", g_diag_ipc_msg.addr, g_diag_ipc_msg.size);

	IPC_MSG_STRUCT ipc_event_msg = {0};
	ipc_event_msg.msg = (u32)&g_diag_ipc_msg;

	rtk_diag_ipc_status_set(RTK_DIAG_IPC_WAIT_RESPONSE);
	DCache_Clean((u32)&g_diag_ipc_msg, sizeof(RtkDiagIpcMsg_t));
	ipc_send_message(DIAG_IPC_DIR, IPC_A2N_EVENT_REQ, &ipc_event_msg);
	//timeout handle
	int timeout = RTK_DIAG_IPC_WAIT_TIMEOUT;
	int ret = RTK_ERR_TIMEOUT;
	while (timeout--) {
		// DCache_Invalidate((u32)&g_diag_ipc_msg, sizeof(RtkDiagIpcMsg_t));
		if (rtk_diag_ipc_status_get() != RTK_DIAG_IPC_WAIT_RESPONSE) {
			ret = rtk_diag_ipc_status_get();
			RTK_LOGA(NOTAG, "ipc response: %d\n", ret);
			break;
		}
	}
	RTK_LOGA(NOTAG, "ap ipc send: %d, %d\n", ret, timeout);
	rtos_mutex_give(g_ipc_mutex);
	return ret;
}

int rtk_diag_event_add(u8 evt_level, u16 evt_type, const u8 *evt_info, u16 evt_len)
{
	u32 ts = rtos_time_get_current_system_time_ms();//get ts as quick as possible

	RtkDiagEvent_t *event = (RtkDiagEvent_t *)rtos_mem_malloc(sizeof(RtkDiagEvent_t) + evt_len);
	if (event == NULL) {
		return RTK_ERR_DIAG_MALLOC;
	}
	event->evt_type = evt_type;
	event->evt_len = evt_len;
	event->evt_time = ts;
	event->evt_level = evt_level;
	_memcpy(event->evt_info, evt_info, evt_len);

	int ret = rtk_diag_ipc_send((u32)event, sizeof(RtkDiagEvent_t) + evt_len, RTK_DIAG_IPC_MSG_TYPE_EVT_ADD);
	rtos_mem_free(event);
	return ret;
}

static int rtk_diag_req_low(u8 cmd, const u8 *payload, u16 payload_length)
{
	RtkDiagDataFrame_t *frame = (RtkDiagDataFrame_t *)rtos_mem_malloc(sizeof(RtkDiagDataFrame_t) + payload_length + 1);
	if (NULL == frame) {
		RTK_LOGA(NOTAG, "Failed to allocate memory for RtkDiagDataFrame_t\n");
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
	return rtk_diag_req_low(RTK_DIAG_CMD_TYPE_OSTIME, (u8 *)&tm, sizeof(tm));
}

int rtk_diag_req_version(void)
{
	extern const char *g_rtk_diag_format_hash;
	// return rtk_diag_req_low(RTK_DIAG_CMD_TYPE_VER, (u8 *)"9FC60C4CB6162E49C54FB94511497E16", 32);
	return rtk_diag_req_low(RTK_DIAG_CMD_TYPE_VER, (u8 *)g_rtk_diag_format_hash, strlen(g_rtk_diag_format_hash));
}

int rtk_diag_req_set_buf_com_capacity(u16 capacity)
{
	g_at_cmd.type = RTK_DIAG_CMD_TYPE_BUFFER;
	g_at_cmd.capacity = capacity;
	return rtk_diag_ipc_send((u32)&g_at_cmd, sizeof(RtkDiagAtCmd_t), RTK_DIAG_IPC_MSG_TYPE_ATCMD);
}

int rtk_diag_req_set_buf_evt_capacity(u16 capacity)
{
	g_at_cmd.type = RTK_DIAG_CMD_TYPE_HEAP;
	g_at_cmd.capacity = capacity;
	return rtk_diag_ipc_send((u32)&g_at_cmd, sizeof(RtkDiagAtCmd_t), RTK_DIAG_IPC_MSG_TYPE_ATCMD);
}

// int rtk_diag_req_set_buf_evt_pool_capacity(u16 capacity)
// {
//   g_at_cmd.type = RTK_DIAG_CMD_TYPE_HEAP;
//   g_at_cmd.capacity = capacity;
//   return rtk_diag_ipc_send((u32)&g_at_cmd, sizeof(RtkDiagAtCmd_t), RTK_DIAG_IPC_MSG_TYPE_ATCMD);
// }

int rtk_diag_req_get_event(u32 timestamp, u16 offset)
{
	g_at_cmd.type = RTK_DIAG_CMD_TYPE_READ;
	g_at_cmd.timestamp = timestamp;
	g_at_cmd.offset = offset;
	return rtk_diag_ipc_send((u32)&g_at_cmd, sizeof(RtkDiagAtCmd_t), RTK_DIAG_IPC_MSG_TYPE_ATCMD);
}

int rtk_diag_req_del_event_before(u32 timestamp)
{
	g_at_cmd.type = RTK_DIAG_CMD_TYPE_DEL_BF;
	g_at_cmd.timestamp = timestamp;
	return rtk_diag_ipc_send((u32)&g_at_cmd, sizeof(RtkDiagAtCmd_t), RTK_DIAG_IPC_MSG_TYPE_ATCMD);
}

int rtk_diag_req_del_event_after(u32 timestamp)
{
	g_at_cmd.type = RTK_DIAG_CMD_TYPE_DEL_AF;
	g_at_cmd.timestamp = timestamp;
	return rtk_diag_ipc_send((u32)&g_at_cmd, sizeof(RtkDiagAtCmd_t), RTK_DIAG_IPC_MSG_TYPE_ATCMD);
}

int rtk_diag_req_get_del_events(void)
{
	g_at_cmd.type = RTK_DIAG_CMD_TYPE_GET_DEL;
	return rtk_diag_ipc_send((u32)&g_at_cmd, sizeof(RtkDiagAtCmd_t), RTK_DIAG_IPC_MSG_TYPE_ATCMD);
}

int rtk_diag_req_clr_del_events(void)
{
	g_at_cmd.type = RTK_DIAG_CMD_TYPE_CLR_DEL;
	return rtk_diag_ipc_send((u32)&g_at_cmd, sizeof(RtkDiagAtCmd_t), RTK_DIAG_IPC_MSG_TYPE_ATCMD);
}

int rtk_diag_req_log_enable(u8 state)
{
	if (state) {
		rtk_log_level_set("*", RTK_LOG_INFO);
	} else {
		rtk_log_level_set("*", RTK_LOG_NONE);
	}
	g_at_cmd.type = RTK_DIAG_CMD_TYPE_SET_LOG;
	g_at_cmd.log = state;
	return rtk_diag_ipc_send((u32)&g_at_cmd, sizeof(RtkDiagAtCmd_t), RTK_DIAG_IPC_MSG_TYPE_ATCMD);
}

#ifndef CONFIG_AMEBA_RLS
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