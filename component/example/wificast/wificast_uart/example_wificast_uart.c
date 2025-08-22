#include "example_wificast_uart.h"

static const char *TAG = "example_main";
static serial_t g_uart_obj;

static void example_recv_callback(wifi_cast_node_t *pnode, unsigned char *buf, unsigned int len, signed char rssi)
{
	static u32 count = 0;
	if (len < sizeof(struct example_frame_head)) {
		return;
	}

	struct example_frame_head *hdr = (struct example_frame_head *)buf;
	if (hdr->type & WIFI_CAST_UART_DATA) {
		RTK_LOGI(TAG, MAC_FMT", rssi: %d, recv count: %d, size: %d\n", MAC_ARG(pnode->mac), rssi, ++count, hdr->len);
		u8 *print_buf = (u8 *)rtos_mem_zmalloc(hdr->len + 1);
		if (!print_buf) {
			return;
		}
		memcpy(print_buf, buf + sizeof(struct example_frame_head), hdr->len);
		print_buf[hdr->len] = '\0';
		RTK_LOGI(TAG, "data: %s\n", print_buf);
		rtos_mem_free(print_buf);
	}
}

static void task_yield(void)
{
	rtos_time_delay_ms(10);
}

static void example_uart_read_task(void *param)
{
	(void)param;
	u8 *rx_buf;
	wifi_cast_node_t node = {0};
	wifi_cast_node_t *dst_node;
	u32 count = 0;
	int size = 0;

	RTK_LOGI(TAG, "%s, enter\n", __func__);

	rx_buf = (u8 *)rtos_mem_zmalloc(UART_RX_BUF_SZ + sizeof(struct example_frame_head));
	if (!rx_buf) {
		goto exit;
	}

	struct example_frame_head *hdr = (struct example_frame_head *)rx_buf;
	hdr->type |= WIFI_CAST_UART_DATA;

	wifi_cast_frame_info_t info = WIFI_CAST_FRAME_INFO_DEFAULT();
	info.ack = 0;

	memcpy(node.mac, WIFI_CAST_BROADCAST_MAC, ETH_ALEN);

	while (1) {
		size = serial_recv_stream_timeout(&g_uart_obj, (char *)(rx_buf + sizeof(struct example_frame_head)), UART_RX_BUF_SZ, 100, task_yield);
		if (!size) {
			RTK_LOGD(TAG, "%s, uart recv size: %d\n", __func__, size);
			continue;
		}
		hdr->len = size;
		if ((dst_node = wifi_cast_get_node_info(&node)) != NULL) {
			if (wifi_cast_send(dst_node, rx_buf, sizeof(struct example_frame_head) + size, &info) != WIFI_CAST_OK) {
				RTK_LOGE(TAG, "%s, send fail\n", __func__);
				continue;
			}
			RTK_LOGI(TAG, "send count: %d, size: %d, data: %s\n", ++count, size, rx_buf + sizeof(struct example_frame_head));
		}
	}
	rtos_mem_free(rx_buf);
exit:
	RTK_LOGI(TAG, "%s, exit\n", __func__);
	rtos_task_delete(NULL);
}

static void example_uart_init(void)
{
	g_uart_obj.uart_idx = UART_IDX;
	serial_init(&g_uart_obj, UART_TX_PIN, UART_RX_PIN);
	serial_baud(&g_uart_obj, UART_BAUD_RATE);
	serial_format(&g_uart_obj, 8, ParityNone, 1);

	if (rtos_task_create(NULL, ((const char *)"example_uart_read_task"), example_uart_read_task, NULL, 512 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_uart_read_task\n\r");
	}
}

static void example_main_task(void *param)
{
	UNUSED(param);
	rtos_time_delay_ms(2000);
	RTK_LOGI(TAG, "------------->start\n");

	wifi_cast_config_t config = WIFI_CAST_INIT_CONFIG_DEFAULT();
	WIFI_CAST_ERROR_CHECK(wifi_cast_init(&config));
	WIFI_CAST_ERROR_CHECK(wifi_cast_register_recv_cb(example_recv_callback));

	example_uart_init();

	rtos_task_delete(NULL);
}

void app_example(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_main_task"), example_main_task, NULL, 512 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_main_task\n\r");
	}
}

void WifiCastTestApp(u8  *argv[])
{
	if (strcmp((const char *)argv[0], "start") == 0) {
		//app_example();
	}
}

u32
CmdWifiCastTest(
	IN u16 argc,
	IN u8  *argv[]
)
{
	if (argc > 0) {
		WifiCastTestApp(argv);
	}

	return TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   wifi_cast_test_cmd_table[] = {
	{
		(const u8 *)"wificast", 3, CmdWifiCastTest, (const u8 *)"\tWIFI \n"
	},
};

