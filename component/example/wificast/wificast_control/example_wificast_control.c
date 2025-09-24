#include "example_wificast_control.h"

static const char *TAG = "example_main";

static control_status_t g_control_status = EXAMPLE_CONTROL_INIT;
static gpio_t g_led_obj;

static void example_control_light_cb(u8 *src_mac, u32 status)
{
	wifi_cast_node_t node = {0};
	memcpy(node.mac, src_mac, ETH_ALEN);

	if (wifi_cast_get_node_info(&node)) {
		gpio_write(&g_led_obj, status);
		RTK_LOGI(TAG, MAC_FMT", status: %s\n", MAC_ARG(src_mac), status ? "on" : "off");
	}
}

static void example_control_bind_cb(u8 *src_mac, u32 status)
{
	wifi_cast_node_t *node;

	RTK_LOGI(TAG, MAC_FMT", status: %s\n", MAC_ARG(src_mac), status ? "bind" : "unbind");

	node = (wifi_cast_node_t *)rtos_mem_malloc(sizeof(wifi_cast_node_t));
	if (node == NULL) {
		RTK_LOGE(TAG, "malloc node failed\n");
		return;
	}
	memset(node, 0x0, sizeof(wifi_cast_node_t));
	memcpy(node->mac, src_mac, ETH_ALEN);

	if (status) {
		WIFI_CAST_ERROR_CHECK(wifi_cast_add_node(node));
	} else {
		WIFI_CAST_ERROR_CHECK(wifi_cast_del_node(node));
	}
	gpio_write(&g_led_obj, status);
	RTK_LOGI(TAG, MAC_FMT", %s success\n", MAC_ARG(src_mac), status ? "bind" : "unbind");
	rtos_mem_free(node);
}

static void example_recv_callback(wifi_cast_node_t *pnode, unsigned char *buf, unsigned int len, signed char rssi)
{
	if (len < sizeof(struct example_frame_head)) {
		return;
	}

	struct example_frame_head *hdr = (struct example_frame_head *)buf;
	u8 value = *(buf + sizeof(struct example_frame_head));
	RTK_LOGI(TAG, MAC_FMT", len: %d, rssi: %d, type: %x\n", MAC_ARG(pnode->mac), len, rssi, hdr->type);
	if (hdr->type & WIFI_CAST_CONTROL_LIGHT) {
		example_control_light_cb(pnode->mac, value);
	} else if (hdr->type & WIFI_CAST_CONTROL_BIND) {
		example_control_bind_cb(pnode->mac, value);
	}
}

static void example_led_init(void)
{
	gpio_init(&g_led_obj, LED_PIN);
	gpio_mode(&g_led_obj, PullNone);
	gpio_dir(&g_led_obj, PIN_OUTPUT);
}

static wcast_err_t example_send(u16 type, const wifi_cast_addr_t dst_mac, u8 *data, u16 data_len)
{
	wifi_cast_node_t node = {0};
	wifi_cast_node_t *dst_node;
	struct example_frame_head *hdr;
	u8 *tx_buf;
	wcast_err_t ret = WIFI_CAST_OK;

	tx_buf = (u8 *)rtos_mem_zmalloc(sizeof(struct example_frame_head) + data_len);
	if (!tx_buf) {
		RTK_LOGE(TAG, "malloc failed\n");
		return WIFI_CAST_ERR;
	}

	hdr = (struct example_frame_head *)tx_buf;
	hdr->type |= type;
	hdr->len = data_len;
	if (data) {
		memcpy(tx_buf + sizeof(struct example_frame_head), data, data_len);
	}

	wifi_cast_frame_info_t info = WIFI_CAST_FRAME_INFO_DEFAULT();
	info.ack = 0;
	memcpy(node.mac, dst_mac, ETH_ALEN);

	if ((dst_node = wifi_cast_get_node_info(&node)) != NULL) {
		ret = wifi_cast_send(dst_node, tx_buf, sizeof(struct example_frame_head) + data_len, &info);
	}
	rtos_mem_free(tx_buf);
	return ret;
}

static void example_control_light(void)
{
	static u8 status = 0;

	if (g_control_status == EXAMPLE_CONTROL_BIND) {
		if (example_send(WIFI_CAST_CONTROL_LIGHT, WIFI_CAST_BROADCAST_MAC, &status, 1) == WIFI_CAST_OK) {
			RTK_LOGI(TAG, "%s, status: %d\n", __func__, status);
			status = !status;
		} else {
			RTK_LOGE(TAG, "%s, send fail\n", __func__);
		}
	} else {
		RTK_LOGI(TAG, "please bind first\n");
	}
}

static void example_control_bind(u8 status)
{
	if (example_send(WIFI_CAST_CONTROL_BIND, WIFI_CAST_BROADCAST_MAC, &status, 1) == WIFI_CAST_OK) {
		if (status) {
			g_control_status = EXAMPLE_CONTROL_BIND;
		} else {
			g_control_status = EXAMPLE_CONTROL_INIT;
		}
		RTK_LOGI(TAG, "%s, status: %s\n", __func__, status ? "bind" : "unbind");
	} else {
		RTK_LOGE(TAG, "%s, send fail\n", __func__);
		return;
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

	example_led_init();

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
	if (_strcmp((const char *)argv[0], "light") == 0) {
		example_control_light();
	} else if (_strcmp((const char *)argv[0], "bind") == 0) {
		example_control_bind(1);
	} else if (_strcmp((const char *)argv[0], "unbind") == 0) {
		example_control_bind(0);
	} else if (_strcmp((const char *)argv[0], "help") == 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\twificast \n");
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t light  : control the device on or off\n");
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t bind   : bind the device\n");
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t unbind : unbind the device\n");
	}
}

u32
CmdWifiCastTest(
	u16 argc,
	u8  *argv[]
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

