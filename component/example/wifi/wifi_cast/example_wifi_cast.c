#include "wifi_intf_drv_to_app_cast.h"
#include "os_wrapper.h"

#define APMODE	1

#if WIFI_CAST_DEBUG_ON
#define WIFI_CAST_LOGD(fmt, arg...) \
do {\
	if (WIFI_CAST_DEBUG_LEVEL >= WIFI_CAST_DEBUG) {\
		printf("debug %s, " fmt "\n", __func__, ##arg);\
	}\
}while(0)
#define WIFI_CAST_LOGI(fmt, arg...) \
do {\
	if (WIFI_CAST_DEBUG_LEVEL >= WIFI_CAST_INFO) {\
		printf("info %s, " fmt "\n", __func__, ##arg);\
	}\
}while(0)
#define WIFI_CAST_LOGW(fmt, arg...) \
do {\
	if (WIFI_CAST_DEBUG_LEVEL >= WIFI_CAST_WARN) {\
		printf("warn %s, " fmt "\n", __func__, ##arg);\
	}\
}while(0)
#define WIFI_CAST_LOGE(fmt, arg...) \
do {\
	if (WIFI_CAST_DEBUG_LEVEL >= WIFI_CAST_ERROR) {\
		printf("error %s, " fmt "\n", __func__, ##arg);\
	}\
}while(0)
#else
#define WIFI_CAST_LOGD(fmt, arg...)
#define WIFI_CAST_LOGI(fmt, arg...)
#define WIFI_CAST_LOGW(fmt, arg...)
#define WIFI_CAST_LOGE(fmt, arg...)
#endif


char wd_stop = 0;
int wd_delay = 1000;
int wd_len = 1024;
int wd_count = 0;
int wd_seq = 0;
unsigned int start_tick, end_tick;

void wifi_cast_set_params(wifi_cast_adapter_t *padapter);
void wifi_cast_test_init(wifi_cast_adapter_t *padapter);


static void wifi_cast_recv_callback(wifi_cast_node_t *pnode, unsigned char *buf, unsigned int len)
{
	// user define
#if APMODE
	WIFI_CAST_LOGD(MAC_FMT" rssi: %d, len: %d", MAC_ARG(pnode->mac), buf[0], len);
#else
	WIFI_CAST_LOGD(MAC_FMT" seq: %d", MAC_ARG(pnode->mac), *(int *)buf);
	wd_count++;
	if (*(int *)buf < wd_seq) {
		WIFI_CAST_LOGI("cur seq: %d < last seq: %d", *(int *)buf, wd_seq);
	}
	if (*(int *)buf != wd_seq + 1) {
		WIFI_CAST_LOGI("cur seq: %d, last seq: %d, lost: %d", *(int *)buf, wd_seq, *(int *)buf - (wd_seq + 1));
	}
	wd_seq = *(int *)buf;
#endif
	return;
}

static void wifi_cast_send_callback(wifi_cast_node_t *pnode, unsigned char *buf, unsigned int len)
{
	// user define
#if APMODE
	WIFI_CAST_LOGD(MAC_FMT" seq: %d, len: %d", MAC_ARG(pnode->mac), *(int *)buf, len);
#else
	WIFI_CAST_LOGD(MAC_FMT" len: %d", MAC_ARG(pnode->mac), len);
#endif
	return;
}

static void wifi_cast_reconnect_task(void *param)
{

	wifi_cast_adapter_t adapter = {0};

	WIFI_CAST_LOGI("------------->start");

	/* 1. Initialize wifi cast adapter parameters. */
	wifi_cast_set_params(&adapter);
	adapter.channel = 0;

	WIFI_CAST_ERROR_CHECK(wifi_cast_deinit(&adapter));
	wifi_cast_test_init(&adapter);

	rtos_task_delete(NULL);
}

static void wifi_cast_node_statuc_chk_callback(wifi_cast_node_t *pnode, unsigned short id)
{
	// user define
#if APMODE
	WIFI_CAST_LOGI(MAC_FMT" id: %d", MAC_ARG(pnode->mac), id);
#else
	if (pnode != NULL) {
		WIFI_CAST_LOGI(MAC_FMT" disconn reason code: %d", MAC_ARG(pnode->mac), id);
	} else {
		WIFI_CAST_LOGI("disconn reason code: %d", id);
	}
	// create reconnect task
	if (rtos_task_create(NULL, ((const char *)"reconnect_task"), wifi_cast_reconnect_task, NULL, 1024 * 4, 1) != SUCCESS) {
		WIFI_CAST_LOGE("Failed to create reconnect_task\n\r");
	}

#endif
	return;
}

void wifi_cast_send_task(void *param)
{
	wifi_cast_node_t *node = (wifi_cast_node_t *)param;
	wifi_cast_node_t *dst_node;
	wifi_cast_node_info_t *dst_node_info;
	char *data;
	int len = 4096;
	int seq = 1;
	unsigned int  start_time_1, end_time, bandwidth_time, report_start_time;
	int64_t  bandwidth_size = 0, report_size = 0, bandwidth = 40 * 1024 * 1024 / 8;// 40Mbps


	data = (char *)rtos_mem_malloc(len);
	if (data == NULL) {
		WIFI_CAST_LOGE("data malloc failed");
		goto exit;
	}
	memset(data, 0x0, len);

	for (int i = 0; i < len; i++) {
		data[i] = i % 255;
	}

	while (1) {
		if ((dst_node = wifi_cast_get_node_info(node)) != NULL) {
			if (dst_node->priv != NULL) {
				dst_node_info = (wifi_cast_node_info_t *)dst_node->priv;
				if (dst_node_info->status != WIFI_CAST_NODE_ONLINE) {
					WIFI_CAST_LOGI(MAC_FMT" is not online", MAC_ARG(dst_node->mac));
				} else {
					/* (3). trx monitor mode, */
					*(int *)data = seq;
					if (wifi_cast_send(dst_node, data, wd_len) != WIFI_CAST_OK) {
						WIFI_CAST_LOGD("wifi_cast_send fail\n");
						rtos_time_delay_ms(2);
					} else {
						seq++;

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
							printf("ap: Send %d KBytes in %d ms, %d Kbits/sec\n\r", (int)(report_size / 1024), (int)(end_time - report_start_time),
								   (int)((report_size * 8) / (end_time - report_start_time)));
							report_start_time = end_time;
							bandwidth_time = end_time;
							report_size = 0;
							bandwidth_size = 0;
						}

					}
				}
			}

		}
		if (wd_stop) {
			WIFI_CAST_LOGI("send end, total send: %d packets, total time: %d ms", (seq - 1), (end_tick - start_tick));
			seq = 1;
		}
		while (1) {
			if (!wd_stop) {
				start_time_1 = rtos_time_get_current_system_time_ms();
				end_time = start_time_1;
				bandwidth_time = start_time_1;
				report_start_time = start_time_1;
				break;
			}
			rtos_time_delay_ms(10);
		}
		if (wd_delay != 0) {
			rtos_time_delay_ms(wd_delay);
		}
	}
exit:
	if (data) {
		rtos_mem_free(data);
	}
	rtos_task_delete(NULL);
}

void wifi_cast_set_params(wifi_cast_adapter_t *padapter)
{
	unsigned char ssid[WIFI_CAST_MAX_SSID_LEN] = "wd_test_gloria";
	unsigned char passwd[WIFI_CAST_MAX_PASSWORD_LEN] = "";

	/* 1. Initialize wifi cast adapter parameters. */
#if APMODE
	padapter->node_type = WIFI_CAST_MASTER_MODE;
	padapter->trx_mode = WIFI_CAST_TRX_UNICAST;
#else
	padapter->node_type = WIFI_CAST_SLAVE_MODE;
	padapter->trx_mode = WIFI_CAST_TRX_MONITOR;
#endif
	padapter->hb_interval = 1000;
	padapter->hb_timeout = 8000;
	padapter->powersave_en = DISABLE;
	padapter->retrylimit = 8;
	padapter->encrypt = 0;
	padapter->channel = 44;

	memcpy(padapter->ssid, ssid, strlen(ssid));
	memcpy(padapter->password, passwd, strlen(passwd));
}

void wifi_cast_test_init(wifi_cast_adapter_t *padapter)
{
	WIFI_CAST_ERROR_CHECK(wifi_cast_init(padapter));
	WIFI_CAST_ERROR_CHECK(wifi_cast_register_recv_cb(wifi_cast_recv_callback));
	WIFI_CAST_ERROR_CHECK(wifi_cast_register_send_cb(wifi_cast_send_callback));
	WIFI_CAST_ERROR_CHECK(wifi_cast_register_node_status_cb(wifi_cast_node_statuc_chk_callback));
}

void wifi_cast_task(void *param)
{
	UNUSED(param);
	wifi_cast_adapter_t adapter = {0};

	rtos_time_delay_ms(3000);
	WIFI_CAST_LOGI("------------->start");

	/* 1. Initialize wifi cast adapter parameters. */
	wifi_cast_set_params(&adapter);

	/* 2. Initialize wifi cast adapter and register callback function. */
	wifi_cast_test_init(&adapter);

	wifi_cast_node_t *node;
	wifi_cast_node_info_t *node_info;

	node = (wifi_cast_node_t *)rtos_mem_malloc(sizeof(wifi_cast_node_t));
	if (node == NULL) {
		WIFI_CAST_LOGE("malloc node failed");
		/* 6. Deinitialize wifi cast adapter */
		WIFI_CAST_ERROR_CHECK(wifi_cast_deinit(&adapter));

		goto exit;
	}
	node_info = (wifi_cast_node_info_t *)rtos_mem_malloc(sizeof(wifi_cast_node_info_t));
	if (node_info == NULL) {
		WIFI_CAST_LOGE("malloc node info failed");
		rtos_mem_free(node);
		/* 6. Deinitialize wifi cast adapter */
		WIFI_CAST_ERROR_CHECK(wifi_cast_deinit(&adapter));
		goto exit;
	}
	memset(node, 0x0, sizeof(wifi_cast_node_t));
	memset(node_info, 0x0, sizeof(wifi_cast_node_info_t));
#if APMODE
	wifi_cast_node_t *node2;
	wifi_cast_node_info_t *node_info2;

	node2 = (wifi_cast_node_t *)rtos_mem_malloc(sizeof(wifi_cast_node_t));
	if (node2 == NULL) {
		WIFI_CAST_LOGE("malloc node failed");
		rtos_mem_free(node);
		rtos_mem_free(node_info);
		/* 6. Deinitialize wifi cast adapter */
		WIFI_CAST_ERROR_CHECK(wifi_cast_deinit(&adapter));

		goto exit;
	}
	node_info2 = (wifi_cast_node_info_t *)rtos_mem_malloc(sizeof(wifi_cast_node_info_t));
	if (node_info2 == NULL) {
		WIFI_CAST_LOGE("malloc node info failed");
		rtos_mem_free(node2);
		rtos_mem_free(node);
		rtos_mem_free(node_info);
		/* 6. Deinitialize wifi cast adapter */
		WIFI_CAST_ERROR_CHECK(wifi_cast_deinit(&adapter));
		goto exit;
	}
	memset(node2, 0x0, sizeof(wifi_cast_node_t));
	memset(node_info2, 0x0, sizeof(wifi_cast_node_info_t));

	/* 3. Add slave nodes */
	char s_node_mac[ETH_LEN] = {0x00, 0xe0, 0x4c, 0x87, 0x30, 0x01};
	char s_node_mac2[ETH_LEN] = {0x00, 0xe0, 0x4c, 0x00, 0x02, 0xce};

	memcpy(node2->mac, s_node_mac2, ETH_LEN);
	node2->priv = node_info2;
	WIFI_CAST_ERROR_CHECK(wifi_cast_add_node(node2));

	memcpy(node->mac, s_node_mac, ETH_LEN);
	node->priv = node_info;
	WIFI_CAST_ERROR_CHECK(wifi_cast_add_node(node));
#else
	/* 3. Add master node */
	char m_node_mac[ETH_LEN] = {0x00, 0xe0, 0x4c, 0x00, 0x02, 0xD3};

	memcpy(node->mac, m_node_mac, ETH_LEN);
	node->priv = node_info;
	WIFI_CAST_ERROR_CHECK(wifi_cast_add_node(node));
#endif


	/* 4. Start send */
#if APMODE
	if (rtos_task_create(NULL, ((const char *)"send_task"), wifi_cast_send_task, node, 1024 * 4, 1) != SUCCESS) {
		WIFI_CAST_LOGE("Failed to create send_task\n\r");
	}
#endif
	/* 5. Start recv data from recv callback */
exit:
	rtos_task_delete(NULL);
}

void example_wifi_cast(void)
{
	if (rtos_task_create(NULL, ((const char *)"wifi_cast_task"), wifi_cast_task, NULL, 1024 * 4, 1) != SUCCESS) {
		WIFI_CAST_LOGE("Failed to create wifi_cast_task\n\r");
	}
}

void WifiCastTestApp(u8  *argv[])
{
#if APMODE
	if (strcmp(argv[0], "stop") == 0) {
		wd_stop = 1;
		end_tick = rtos_time_get_current_system_time_ms();
		WIFI_CAST_LOGI("stop send");
	} else if (strcmp(argv[0], "start") == 0) {
		wd_stop = 0;
		wd_delay = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
		wd_len = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		WIFI_CAST_LOGI("start send, delay: %d, len: %d", wd_delay, wd_len);
		start_tick = rtos_time_get_current_system_time_ms();
	} else if (strcmp(argv[0], "set_channel") == 0) {
		unsigned char channel = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
		wifi_cast_set_channel(channel);
		WIFI_CAST_LOGI("softap channge channel to %d", channel);
	}
#else
	if (strcmp(argv[0], "stop") == 0) {
		WIFI_CAST_LOGI("recv end, total recv: %d packets, total send: %d packets, loss rate: %d", wd_count, wd_seq, (wd_seq - wd_count) * 100 / wd_seq);
		wd_count = 0;
	}
#endif
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

