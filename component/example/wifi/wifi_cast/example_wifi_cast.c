#include "wifi_intf_drv_to_app_cast.h"
#include "os_wrapper.h"
#include "example_wifi_cast.h"

static const char *TAG = "WIFI CAST EXAMPLE";
char wd_tp_start = 0;
int wd_len = 1024;
int wd_count = 0;
int wd_seq = 0;
unsigned int start_tick, end_tick;
unsigned char inputmac[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char ch = 1;
unsigned char encrypt = 0;


static void wifi_cast_recv_callback(wifi_cast_node_t *pnode, unsigned char *buf, unsigned int len, signed char rssi)
{
	// user define
	if (buf[0] == 0xFF) {
		wd_count++;
		if (*(int *)(buf + 1) < wd_seq) {
			RTK_LOGI(TAG, "cur seq: %d < last seq: %d\n", *(int *)(buf + 1), wd_seq);
		}
		if (*(int *)(buf + 1) != wd_seq + 1) {
			RTK_LOGI(TAG, "cur seq: %d, last seq: %d, lost: %d\n", *(int *)(buf + 1), wd_seq, *(int *)(buf + 1) - (wd_seq + 1));
		}
		wd_seq = *(int *)(buf + 1);

	} else {
		RTK_LOGI(TAG, MAC_FMT", len: %d, rssi: %d\n", MAC_ARG(pnode->mac), len, rssi);
	}
	return;
}

void wifi_cast_send_task(void *param)
{
	UNUSED(param);
	wifi_cast_node_t node = {0};
	wifi_cast_node_t *dst_node;
	wifi_cast_frame_info_t info = {0};
	unsigned char *data;
	int len = 1460;
	int seq = 1;
	unsigned int  start_time_1, end_time, bandwidth_time, report_start_time;
	int64_t  bandwidth_size = 0, report_size = 0, bandwidth = 10 * 1024 * 1024 / 8;// 40Mbps

	RTK_LOGI(TAG, "------------->start\n");

	data = (unsigned char *)rtos_mem_malloc(len);
	if (data == NULL) {
		RTK_LOGE(TAG, "data malloc failed");
		goto exit;
	}
	memset(data, 0x0, len);

	for (int i = 0; i < len; i++) {
		data[i] = i % 255;
	}
	memcpy(node.mac, inputmac, ETH_ALEN);
	data[0] = 0xFF;
	report_start_time = start_tick;

	info.ack = 1;
	info.wait_ms = 500;
	info.retry_limit = 4;
	info.tx_rate = MGN_54M;

	while (1) {
		if ((dst_node = wifi_cast_get_node_info(&node)) != NULL) {

			/* (3). trx monitor mode, */
			*(int *)(data + 1) = seq;
			if (wifi_cast_send(dst_node, data, len, &info) != WIFI_CAST_OK) {
				RTK_LOGD(TAG, "wifi_cast_send fail\n");
				rtos_time_delay_ms(2);
			} else {
				seq++;
				wd_seq = seq;
				bandwidth_size += wd_len;
				report_size += wd_len;
				end_time = rtos_time_get_current_system_time_ms();
				if ((bandwidth_size >= bandwidth) && ((end_time - bandwidth_time) < (RTOS_TICK_RATE_HZ * 1))) {
					rtos_time_delay_ms(RTOS_TICK_RATE_HZ * 1 - (end_time - bandwidth_time));
					end_time = rtos_time_get_current_system_time_ms();
					bandwidth_time = end_time;
					bandwidth_size = 0;
				}

				if ((end_time - report_start_time) >= (RTOS_TICK_RATE_HZ * 1)) {
					RTK_LOGI(TAG, "ap: Send %d KBytes in %d ms, %d Kbits/sec\n\r", (int)(report_size / 1024), (int)(end_time - report_start_time),
							 (int)((report_size * 8) / (end_time - report_start_time)));
					report_start_time = end_time;
					bandwidth_time = end_time;
					report_size = 0;
					bandwidth_size = 0;
				}

			}

		}

		start_time_1 = rtos_time_get_current_system_time_ms();
		end_time = start_time_1;
		bandwidth_time = start_time_1;
		report_start_time = start_time_1;
	}
exit:
	if (data) {
		rtos_mem_free(data);
	}
	rtos_task_delete(NULL);
}

void wifi_cast_task(void *param)
{
	UNUSED(param);
	wifi_cast_config_t config = {0};

	//rtos_time_delay_ms(3000);
	RTK_LOGI(TAG, "------------->start\n");

	/* 1. Initialize wifi cast configration. */
	config.channel = ch;

	/* 2. Initialize wifi cast adapter and register callback function. */
	WIFI_CAST_ERROR_CHECK(wifi_cast_init(&config));
	WIFI_CAST_ERROR_CHECK(wifi_cast_register_recv_cb(wifi_cast_recv_callback));

	/* 3. Add node */
	wifi_cast_node_t *node;
	node = (wifi_cast_node_t *)rtos_mem_malloc(sizeof(wifi_cast_node_t));
	if (node == NULL) {
		RTK_LOGE(TAG, "malloc node failed\n");
		/* 6. Deinitialize wifi cast adapter */
		WIFI_CAST_ERROR_CHECK(wifi_cast_deinit(&config));
		goto exit;
	}

	memset(node, 0x0, sizeof(wifi_cast_node_t));
	memcpy(node->mac, inputmac, ETH_LEN);
	if (encrypt) {
		node->encrypt = 1;
		memset(node->key, 1, WIFI_CAST_KEY_LEN);
	}
	WIFI_CAST_ERROR_CHECK(wifi_cast_add_node(node));
	rtos_mem_free(node);

	/* 4. Start send */
	/* 5. Start recv data from recv callback */
exit:
	rtos_task_delete(NULL);
}

void example_wifi_cast_simple(void)
{
	if (rtos_task_create(NULL, ((const char *)"wifi_cast_task"), wifi_cast_task, NULL, 1024 * 4, 1) != SUCCESS) {
		RTK_LOGE(TAG, "Failed to create wifi_cast_task\n\r");
	}
}
void example_wifi_cast(void)
{
}

void test_txraw_packet(void)
{
	wifi_cast_node_t node = {0};
	wifi_cast_node_t *dst_node;
	wifi_cast_frame_info_t info = {0};

	u8 buf[20];

	for (int i = 0; i < 20; i++) {
		buf[i] = i;
	}

	memcpy(node.mac, inputmac, ETH_ALEN);
	info.ack = 1;
	info.wait_ms = 500;
	info.retry_limit = 4;
	info.tx_rate = MGN_54M;

	if ((dst_node = wifi_cast_get_node_info(&node)) != NULL) {
		if (wifi_cast_send(dst_node, buf, 20, &info) != WIFI_CAST_OK) {
			RTK_LOGD(TAG, "wifi_cast_send fail\n");
		}
	}
}


rtos_task_t send_task_handle;
void WifiCastTestApp(u8  *argv[])
{
	if (strcmp((const char *)argv[0], "start") == 0) {
		example_wifi_cast_simple();
	} else if (_strcmp((const char *)argv[0], "starttp") == 0) {
		start_tick = rtos_time_get_current_system_time_ms();
		if (!wd_tp_start) {
			wd_tp_start = 1;
			if (rtos_task_create(&send_task_handle, ((const char *)"send_task"), wifi_cast_send_task, NULL, 1024 * 4, 1) != SUCCESS) {
				RTK_LOGE(TAG, "Failed to create send_task\n\r");
			}
		}
	} else if (strcmp((const char *)argv[0], "stoptp") == 0) {
		if (wd_tp_start) {
			end_tick = rtos_time_get_current_system_time_ms();
			wd_tp_start = 0;
			rtos_task_delete(send_task_handle);
			RTK_LOGI(TAG, "send end, total send: %d packets, total time: %d ms\n", (wd_seq - 1), (end_tick - start_tick));
		} else {
			RTK_LOGI(TAG, "recv end, total recv: %d packets, total send: %d packets, loss rate: %d\n", wd_count, wd_seq, (wd_seq - wd_count) * 100 / wd_seq);
			wd_count = 0;
		}
	} else if (strcmp((const char *)argv[0], "set_channel") == 0) {
		ch = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
		RTK_LOGI(TAG, "set channel %d\n", ch);
	} else if (_strcmp((const char *)argv[0], "setmac") == 0) {
		RTK_LOGI(TAG, "set mac: %s\n", argv[1]);
		char *DString = (char *)argv[1];
		int mac[ETH_ALEN];
		sscanf(DString, "%02x:%02x:%02x:%02x:%02x:%02x", mac, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5);
		for (int i = 0; i < ETH_ALEN; i ++) {
			inputmac[i] = (u8)mac[i] & 0xFF;
		}
		RTK_LOGI(TAG, "cmd input mac:%02x:%02x:%02x:%02x:%02x:%02x\n",
				 inputmac[0], inputmac[1], inputmac[2], inputmac[3], inputmac[4], inputmac[5]);
	} else if (_strcmp((const char *)argv[0], "txraw") == 0) {
		test_txraw_packet();
	} else if (_strcmp((const char *)argv[0], "encrypt") == 0) {
		encrypt = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
		RTK_LOGI(TAG, "set encrypt %d\n", encrypt);
	} else if (_strcmp((const char *)argv[0], "deinit") == 0) {
		WIFI_CAST_ERROR_CHECK(wifi_cast_deinit(NULL));
	}

}

u32
CmdWIFI_CASTTest(
	IN u16 argc,
	IN u8  *argv[]
)
{
	if (argc > 0) {
		WifiCastTestApp(argv);
	}

	return _TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   wifi_cast_test_cmd_table[] = {
	{
		(const u8 *)"wifi_cast_test", 3, CmdWIFI_CASTTest, (const u8 *)"\tWIFI \n"
	},
};

