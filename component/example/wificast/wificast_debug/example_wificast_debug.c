#include "example_wificast_debug.h"
#include <stdio.h>

#define WIFI_CAST_MONITORED      0

static const char *TAG = "example_main";

static rtos_queue_t g_recv_cb_q;
static rtos_queue_t g_log_q;
static rtos_queue_t g_scan_report_q;
static struct example_node_list g_node_list = {0};
static struct example_scan_info *g_info_list = NULL;
extern volatile UART_LOG_CTL		shell_ctl;

int vasprintf(char **strp, const char *format, va_list ap);

static wcast_err_t example_send(u16 type, const wifi_cast_addr_t dst_mac, u8 *data, u16 data_len)
{
	wifi_cast_node_t node = {0};
	wifi_cast_node_t *dst_node;
	struct example_frame_head *hdr;
	u8 *tx_buf;
	wcast_err_t ret = WIFI_CAST_ERR;

	tx_buf = (u8 *)rtos_mem_zmalloc(sizeof(struct example_frame_head) + data_len);
	if (!tx_buf) {
		RTK_LOGE(TAG, "malloc failed\n");
		return ret;
	}

	hdr = (struct example_frame_head *)tx_buf;
	hdr->type |= type;
	hdr->len = data_len;
	if (data) {
		memcpy(tx_buf + sizeof(struct example_frame_head), data, data_len);
	}

	wifi_cast_frame_info_t info = WIFI_CAST_FRAME_INFO_DEFAULT();
	info.ack = 0;
	info.retransmit_count = 3;
	memcpy(node.mac, dst_mac, ETH_ALEN);

	if ((dst_node = wifi_cast_get_node_info(&node)) != NULL) {
		ret = wifi_cast_send(dst_node, tx_buf, sizeof(struct example_frame_head) + data_len, &info);
		if (ret != WIFI_CAST_OK) {
			RTK_LOGD(TAG, "%s, send failed, ret: %d\n", __func__, ret);
		}
	}
	rtos_mem_free(tx_buf);
	return ret;
}

static wcast_err_t example_add_node(u8 *mac)
{
	wifi_cast_node_t node = {0};
	memcpy(node.mac, mac, 6);
	if (wifi_cast_get_node_info(&node)) {
		return WIFI_CAST_ERR_NODE_EXIST;
	}

	if (wifi_cast_add_node(&node) != WIFI_CAST_OK) {
		return WIFI_CAST_ERR;
	}

	if (!g_node_list.is_init) {
		g_node_list.is_init = 1;
	}

	memcpy(g_node_list.info[g_node_list.size].mac, mac, 6);
	g_node_list.size = (g_node_list.size + 1) % MAX_NODE_NUM;

	return WIFI_CAST_OK;
}

static void example_debug_command_cb(u8 *data, u32 data_len)
{
	RTK_LOGI(TAG, "recv command: %s(%d)\n", data, data_len);

	PUART_LOG_BUF pUartLogBuf = shell_ctl.pTmpLogBuf;
	pUartLogBuf->BufCount = data_len;
	memcpy((void *)&pUartLogBuf->UARTLogBuf[0], data, data_len);

	shell_ctl.ExecuteCmd = TRUE;
	if (shell_ctl.shell_task_rdy) {
		shell_ctl.GiveSema();
	}
}

static void example_debug_log_cb(u8 *data, u32 data_len)
{
	RTK_LOGI(TAG, "recv log: (%d)\n", data_len);
	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "%s\n", data);
}

static void example_debug_scan_request_cb(u8 *src_mac)
{
	struct example_scan_info info = {0};
	struct rtw_mac self_mac = {0};

	wifi_get_mac_address(0, &self_mac, 0);
	memcpy(info.mac, self_mac.octet, 6);

	if (example_send(WIFI_CAST_SCAN_RESPONSE, WIFI_CAST_BROADCAST_MAC, (u8 *)&info, sizeof(info)) != WIFI_CAST_OK) {
		RTK_LOGE(TAG, "%s, send fail\n", __func__);
		return;
	}

	if (example_add_node(src_mac) != WIFI_CAST_OK) {
		return;
	}

	RTK_LOGI(TAG, "%s, recv scan request from src_mac: "MAC_FMT"\n", __func__, MAC_ARG(src_mac));
	RTK_LOGI(TAG, "%s, send response success\n", __func__);
}

static void example_debug_scan_response_cb(u8 *mac)
{
	struct example_scan_info info = {0};
	memcpy(info.mac, mac, 6);

	RTK_LOGD(TAG, "%s, recv response from"MAC_FMT"\n", __func__, MAC_ARG(mac));

	if (g_scan_report_q) {
		if (RTK_SUCCESS != rtos_queue_send(g_scan_report_q, &info, 0)) {
			RTK_LOGW(TAG, "%s, send queue failed\n", __func__);
			return;
		}
	}
}

static void example_recv_cb_task(void *param)
{
	(void)param;
	struct example_cb_recv_data *recv_data = NULL;

	while (1) {
		if (RTK_SUCCESS == rtos_queue_receive(g_recv_cb_q, &recv_data, 10)) {
			struct example_frame_head *hdr = (struct example_frame_head *)recv_data->data;
			RTK_LOGD(TAG, MAC_FMT", len: %d, type: %x\n", MAC_ARG(recv_data->mac), recv_data->data_len, hdr->type);
			if (hdr->type & WIFI_CAST_DEBUG_COMMAND) {
				example_debug_command_cb(recv_data->data + sizeof(struct example_frame_head), hdr->len);
			} else if (hdr->type & WIFI_CAST_DEBUG_LOG) {
				example_debug_log_cb(recv_data->data + sizeof(struct example_frame_head), hdr->len);
			} else if (hdr->type & WIFI_CAST_SCAN_REQUEST) {
				example_debug_scan_request_cb(recv_data->mac);
			} else if (hdr->type & WIFI_CAST_SCAN_RESPONSE) {
				example_debug_scan_response_cb(recv_data->mac);
			}
			rtos_mem_free(recv_data->data);
			rtos_mem_free(recv_data);
		}
	}
	rtos_task_delete(NULL);
}

static void example_recv_callback(wifi_cast_node_t *pnode, unsigned char *buf, unsigned int len, signed char rssi)
{
	if (len < sizeof(struct example_frame_head)) {
		return;
	}
	RTK_LOGD(TAG, MAC_FMT", len: %d, rssi: %d\n", MAC_ARG(pnode->mac), len, rssi);

	struct example_cb_recv_data *recv_data = NULL;
	recv_data = (struct example_cb_recv_data *)rtos_mem_zmalloc(sizeof(struct example_cb_recv_data));
	if (!recv_data) {
		RTK_LOGE(TAG, "%s, recv_data malloc failed\n", __func__);
		return;
	}
	recv_data->data = (u8 *)rtos_mem_zmalloc(len);
	if (!recv_data->data) {
		rtos_mem_free(recv_data);
		RTK_LOGE(TAG, "%s, recv_data malloc failed\n", __func__);
		return;
	}
	recv_data->data_len = len;
	memcpy(recv_data->mac, pnode->mac, ETH_ALEN);
	memcpy(recv_data->data, buf, len);
	if (g_recv_cb_q) {
		if (RTK_SUCCESS != rtos_queue_send(g_recv_cb_q, &recv_data, 0)) {
			RTK_LOGD(TAG, "%s, send queue failed\n", __func__);
			rtos_mem_free(recv_data->data);
			rtos_mem_free(recv_data);
			return;
		}
	} else {
		rtos_mem_free(recv_data->data);
		rtos_mem_free(recv_data);
	}
}

void wifi_cast_send_log(const char *fmt, va_list ap)
{
	/* wifi cast send broadcast */
	struct log_info_s *log_info = NULL;
	log_info = (struct log_info_s *)rtos_mem_zmalloc(sizeof(struct log_info_s));
	if (!log_info) {
		RTK_LOGE(TAG, "%s, log_info malloc failed\n", __func__);
		return;
	}
	char *buf = NULL;

	log_info->size = vasprintf(&buf, fmt, ap);
	if (!buf) {
		RTK_LOGE(TAG, "%s, buf malloc failed\n", __func__);
		return;
	}
	log_info->data = buf;

	if (g_log_q) {
		if (RTK_SUCCESS != rtos_queue_send(g_log_q, &log_info, 0)) {
			RTK_LOGD(TAG, "%s, send queue failed\n", __func__);
			rtos_mem_free(buf);
			rtos_mem_free(log_info);
			return;
		}
	} else {
		rtos_mem_free(buf);
		rtos_mem_free(log_info);
	}
}

#if WIFI_CAST_MONITORED
static void example_log_send_task(void *param)
{
	(void)param;
	struct log_info_s *log_info = NULL;

	while (1) {
		if (RTK_SUCCESS == rtos_queue_receive(g_log_q, &log_info, 10)) {
			if (WIFI_CAST_OK != example_send(WIFI_CAST_DEBUG_LOG, WIFI_CAST_BROADCAST_MAC, (u8 *)log_info->data, log_info->size)) {
				RTK_LOGE(TAG, "%s, send fail\n", __func__);
			}
			rtos_mem_free(log_info->data);
			rtos_mem_free(log_info);
		}
	}
	rtos_task_delete(NULL);
}
#endif

static void example_debug_initial_scan(struct example_scan_info **info_list, int *num, u32 wait_ms)
{
	struct example_scan_info info = {0};
	int info_num = 0;

	do {
		example_send(WIFI_CAST_SCAN_REQUEST, WIFI_CAST_BROADCAST_MAC, NULL, 0);
		if (RTK_SUCCESS == rtos_queue_receive(g_scan_report_q, &info, 50)) {
			u8 exist = 0;
			if (g_info_list) {
				for (int i = 0; i < info_num; i++) {
					if (!memcmp(info.mac, g_info_list[i].mac, 6)) {
						exist = 1;
						break;
					}
				}
			}
			if (exist) {
				continue;
			}
			RTK_LOGD(TAG, "%s, recv response from"MAC_FMT"\n", __func__, MAC_ARG(info.mac));
			g_info_list = (struct example_scan_info *)rtos_mem_realloc(g_info_list, (info_num + 1) * sizeof(struct example_scan_info));
			memcpy(g_info_list[info_num].mac, info.mac, 6);
			info_num++;
		}
		wait_ms -= 50;
	} while (wait_ms > 0);

	*info_list = g_info_list;
	*num = info_num;
}

static void scan_result_print(void)
{
	RTK_LOGI(TAG, "addr_list: \n");
	for (int i = 0; i < g_node_list.size; i++) {
		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "%d. "MAC_FMT"\n", i + 1, MAC_ARG(g_node_list.info[i].mac));
	}
}

static void scan_task(void *param)
{
	(void)param;
	struct example_scan_info *info_list = NULL;
	int info_num = 0;

	example_debug_initial_scan(&info_list, &info_num, 3000);
	RTK_LOGI(TAG, "%s, scan info num: %d\n", __func__, info_num);

	if (info_num == 0) {
		goto exit;
	}

	for (int i = 0; i < info_num; i++) {
		example_add_node(info_list[i].mac);
	}

	scan_result_print();

exit:
	rtos_mem_free(g_info_list);
	g_info_list = NULL;
	rtos_task_delete(NULL);
}

static void scan_handle(void)
{
	if (rtos_task_create(NULL, ((const char *)"scan_task"), scan_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create scan_task\n\r");
	}
}

void command_handle(u16 argc, u8 *argv[])
{
	unsigned int tmp[6];
	u8 mac[6];
	PCOMMAND_TABLE  pCmdTbl = shell_ctl.pCmdTbl;
	int CmdNum = shell_ctl.CmdTblSz;
	u8 is_shell = FALSE;
	u8 *cmdbuf = NULL;
	u16 cmdsize = 0;

	cmdbuf = (u8 *)rtos_mem_zmalloc(UART_LOG_CMD_BUFLEN);
	if (!cmdbuf) {
		RTK_LOGE(TAG, "malloc cmdbuf fail\n\r");
		return;
	}

	sscanf((char *)argv[1], MAC_FMT, tmp, tmp + 1, tmp + 2, tmp + 3, tmp + 4, tmp + 5);
	for (int i = 0; i < 6; i++) {
		mac[i] = tmp[i] & 0xFF;
	}
	RTK_LOGI(TAG, "send command %s to "MAC_FMT"\n", argv[2], MAC_ARG(mac));

	for (int i = 0; i < CmdNum; i++) {
		if ((_stricmp((const char *)argv[2], (const char *)pCmdTbl[i].cmd)) == 0) {
			is_shell = TRUE;
			break;
		}
	}

	if (is_shell) {
		for (int i = 2; i < argc; i++) {
			DiagSnPrintf((char *)(cmdbuf + cmdsize), strlen((const char *)argv[i]), "%s ", argv[i]);
			cmdsize += strlen((const char *)argv[i]) + 1;
		}
	} else {
		memcpy(cmdbuf, argv[2], strlen((const char *)argv[2]) + 1);
		cmdsize = strlen((const char *)argv[2]) + 1;
	}

	if (WIFI_CAST_OK != example_send(WIFI_CAST_DEBUG_COMMAND, mac, cmdbuf, cmdsize)) {
		RTK_LOGE(TAG, "%s, send fail\n", __func__);
	}
	rtos_mem_free(cmdbuf);
}

static void example_main_task(void *param)
{
	UNUSED(param);
	rtos_time_delay_ms(2000);
	RTK_LOGI(TAG, "------------->start\n");

	wifi_cast_config_t config = WIFI_CAST_INIT_CONFIG_DEFAULT();
	WIFI_CAST_ERROR_CHECK(wifi_cast_init(&config));
	WIFI_CAST_ERROR_CHECK(wifi_cast_register_recv_cb(example_recv_callback));

	rtos_queue_create(&g_recv_cb_q, 32, sizeof(struct example_cb_recv_data *));
	rtos_queue_create(&g_scan_report_q, 16, sizeof(struct example_scan_info));

	if (rtos_task_create(NULL, ((const char *)"example_recv_cb_task"), example_recv_cb_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_recv_cb_task\n\r");
	}

#if WIFI_CAST_MONITORED
	rtos_queue_create(&g_log_q, 16, sizeof(struct log_info_s *));
	if (rtos_task_create(NULL, ((const char *)"example_log_send_task"), example_log_send_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_log_send_task\n\r");
	}
#endif
	rtos_task_delete(NULL);
}

void app_example(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_main_task"), example_main_task, NULL, 512 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_main_task\n\r");
	}
}

void WifiCastTestApp(u16 argc, u8 *argv[])
{
	if (strcmp((const char *)argv[0], "command") == 0) {
		command_handle(argc, argv);
	} else if (_strcmp((const char *)argv[0], "scan") == 0) {
		scan_handle();
	} else if (_strcmp((const char *)argv[0], "addr_list") == 0) {
		scan_result_print();
	} else if (_strcmp((const char *)argv[0], "help") == 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\twificast \n");
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t command <xx:xx:xx:xx:xx:xx> <Command> : set MAC of the monitored device and command require devices execute\n");
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t scan : find monitored devices\n");
	}
}

u32
CmdWifiCastTest(
	IN u16 argc,
	IN u8  *argv[]
)
{
	if (argc > 0) {
		WifiCastTestApp(argc, argv);
	}

	return TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   wifi_cast_test_cmd_table[] = {
	{
		(const u8 *)"wificast", 3, CmdWifiCastTest, (const u8 *)"\twificast \n"
	},
};

