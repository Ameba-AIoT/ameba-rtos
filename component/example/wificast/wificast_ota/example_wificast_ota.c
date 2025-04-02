#include "example_wificast_ota.h"

static const char *const TAG = "example_main";

#define PORT	8082
static const char *host = "192.168.137.1";  //"m-apps.oss-cn-shenzhen.aliyuncs.com"
static const char *resource = "ota_all.bin";     //"051103061600.bin"
static ota_context *ctx = NULL;

static rtos_queue_t g_scan_report_q;
static rtos_queue_t g_ota_request_q;
static rtos_queue_t g_ota_recv_cb_q;
static struct example_scan_info *g_info_list = NULL;
static struct example_ota_status *g_ota_status = NULL;
static u8 ota_role = WIFI_CAST_OTA_RECEIVER;
static flash_t flash_obj;

static void dump_buf(char *info, u8 *buf, int len)
{
	int i;

	RTK_LOGI(TAG, "%s\n", info);
	RTK_LOGI(TAG, "len: %d\n", len);
	for (i = 0; i < len; i++) {
		DiagPrintf("%s%02X%s", i % 16 == 0 ? "\r\n\t" : " ", buf[i], i == len - 1 ? "\r\n" : "");
	}
}

static void firmware_download_end(void)
{
	sys_clear_ota_signature();
	rtos_time_delay_ms(500);
	sys_reset();
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

	return WIFI_CAST_OK;
}

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

static void example_ota_scan_request_cb(u8 *src_mac)
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

static void example_ota_scan_response_cb(u8 *mac)
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

static void example_ota_status_request_cb(u8 *src_mac, u8 *data)
{
	RTK_LOGI(TAG, "%s, recv status request from"MAC_FMT"\n", __func__, MAC_ARG(src_mac));
	if (ota_role == WIFI_CAST_OTA_SENDER) {
		return;
	}

	struct example_ota_status *recv_status = (struct example_ota_status *)data;
	struct example_ota_status *tmp_status;
	static u32 start_tick = 0;

	if (!g_ota_status) {
		g_ota_status = rtos_mem_zmalloc(sizeof(struct example_ota_status) + WIFI_CAST_OTA_PROGRESS_MAX * 10);
		if (!g_ota_status) {
			RTK_LOGE(TAG, "%s, g_ota_status malloc failed", __func__);
			return;
		}
		rt_kv_get("ota_status", (u8 *)g_ota_status, sizeof(struct example_ota_status) + WIFI_CAST_OTA_PROGRESS_MAX * 10);
	}

	if (recv_status->status == WIFI_CAST_OTA_RESET) {
		firmware_download_end();
		return;
	}

	if (g_ota_status->status == WIFI_CAST_OTA_ONGOING ||
		(g_ota_status->total_size == recv_status->total_size && g_ota_status->checksum == recv_status->checksum)) {
		goto response;
	}

	memcpy(g_ota_status, recv_status, sizeof(struct example_ota_status));
	memset(g_ota_status->progress_array[0], 0x0, g_ota_status->packet_num / 8 + 1);
	g_ota_status->status = WIFI_CAST_OTA_ONGOING;
	start_tick = rtos_time_get_current_system_time_ms();

	rt_kv_set("ota_status", g_ota_status, sizeof(struct example_ota_status) + g_ota_status->packet_num / 8 + 1);

	int sector_num = g_ota_status->total_size / 4096 + 1;
	for (int i = 0; i < sector_num; i++) {
		flash_erase_sector(&flash_obj, g_ota_status->image_addr + 4096 * i);
	}

response:
	RTK_LOGI(TAG, "%s, ota status, image_id: %d, image_addr: 0x%x, total_size: %d bytes, written_size: %d bytes, packet_num: %d\n",
			 __func__, g_ota_status->image_id, g_ota_status->image_addr, g_ota_status->total_size, g_ota_status->written_size, g_ota_status->packet_num);
	if (g_ota_status->written_size == g_ota_status->total_size) {
		if (WIFI_CAST_OK !=
			example_send(WIFI_CAST_OTA_STATUS_RESPONSE, src_mac, (u8 *)g_ota_status, sizeof(struct example_ota_status))) {
			RTK_LOGE(TAG, "%s, send fail\n", __func__);
			return;
		}
	}

	tmp_status = (struct example_ota_status *)rtos_mem_malloc(sizeof(struct example_ota_status) + WIFI_CAST_OTA_PROGRESS_MAX);
	if (!tmp_status) {
		RTK_LOGE(TAG, "%s, malloc failed", __func__);
		return;
	}
	memcpy(tmp_status, g_ota_status, sizeof(struct example_ota_status));
	dump_buf("progress:", g_ota_status->progress_array[0], g_ota_status->packet_num / 8 + 1);
	for (int seq = 0; seq < g_ota_status->packet_num; seq++) {
		if (!WIFI_CAST_OTA_GET_BITS(g_ota_status->progress_array, seq)) {
			tmp_status->progress_index = seq / (8 * WIFI_CAST_OTA_PROGRESS_MAX);
			memcpy(tmp_status->progress_array[0], g_ota_status->progress_array[tmp_status->progress_index], WIFI_CAST_OTA_PROGRESS_MAX);
			if (WIFI_CAST_OK !=
				example_send(WIFI_CAST_OTA_STATUS_RESPONSE, src_mac, (u8 *)tmp_status, sizeof(struct example_ota_status) + WIFI_CAST_OTA_PROGRESS_MAX)) {
				RTK_LOGE(TAG, "%s, send fail\n", __func__);
				return;
			}
			RTK_LOGI(TAG, "%s, send status response, total_size: %d, written_size: %d, progress_index: %d\n",
					 __func__, tmp_status->total_size, tmp_status->written_size, tmp_status->progress_index);
			break;
		}
	}
	if (g_ota_status->written_size == g_ota_status->total_size &&
		g_ota_status->status == WIFI_CAST_OTA_ONGOING) {
		g_ota_status->status = WIFI_CAST_OTA_FINISHED;
		// rtos_mem_free(g_ota_status);
		RTK_LOGI(TAG, "%s, upgrade done, spend time: %d ms\n", __func__, rtos_time_get_current_system_time_ms() - start_tick);
	}
	rtos_mem_free(tmp_status);
}

static void example_ota_status_response_cb(u8 *src_mac, u8 *data)
{
	RTK_LOGI(TAG, "%s, recv status response from"MAC_FMT"\n", __func__, MAC_ARG(src_mac));

	struct example_ota_status *status = NULL;
	status = (struct example_ota_status *)rtos_mem_zmalloc(sizeof(struct example_ota_status) + WIFI_CAST_OTA_PROGRESS_MAX);
	if (!status) {
		RTK_LOGE(TAG, "%s, status malloc failed\n", __func__);
		return;
	}

	memcpy(status, data, sizeof(struct example_ota_status) + WIFI_CAST_OTA_PROGRESS_MAX);

	if (g_ota_request_q) {
		if (RTK_SUCCESS != rtos_queue_send(g_ota_request_q, &status, 0)) {
			RTK_LOGE(TAG, "%s, send queue failed\n", __func__);
			rtos_mem_free(status);
			return;
		}
	} else {
		rtos_mem_free(status);
	}
}

static void example_ota_data_cb(u8 *data, u16 data_len)
{
	if (!g_ota_status) {
		RTK_LOGW(TAG, "%s, ota has not started yet\n", __func__);
		return;
	}
	struct ota_packet_head *hdr = (struct ota_packet_head *)data;
	if (WIFI_CAST_OTA_GET_BITS(g_ota_status->progress_array, hdr->seq)) {
		RTK_LOGD(TAG, "%s, retransmit ota packet, seq: %d\n", __func__, hdr->seq);
		return;
	}
	int size = data_len - sizeof(struct ota_packet_head);
	flash_stream_write(&flash_obj, g_ota_status->image_addr + hdr->seq * size, size, data + sizeof(struct ota_packet_head));
	WIFI_CAST_OTA_SET_BITS(g_ota_status->progress_array, hdr->seq);
	g_ota_status->written_size += size;
	rt_kv_set("ota_status", g_ota_status, sizeof(struct example_ota_status) + g_ota_status->packet_num / 8 + 1);
}

static void example_recv_cb_task(void *param)
{
	(void)param;
	struct example_cb_recv_data *recv_data = NULL;

	while (1) {
		if (RTK_SUCCESS == rtos_queue_receive(g_ota_recv_cb_q, &recv_data, 10)) {
			struct example_frame_head *hdr = (struct example_frame_head *)recv_data->data;
			RTK_LOGD(TAG, MAC_FMT", len: %d, type: %x\n", MAC_ARG(recv_data->mac), recv_data->data_len, hdr->type);
			if (hdr->type & WIFI_CAST_OTA_DATA) {
				example_ota_data_cb(recv_data->data + sizeof(struct example_frame_head), hdr->len);
			} else if (hdr->type & WIFI_CAST_SCAN_REQUEST) {
				example_ota_scan_request_cb(recv_data->mac);
			} else if (hdr->type & WIFI_CAST_SCAN_RESPONSE) {
				example_ota_scan_response_cb(recv_data->mac);
			} else if (hdr->type & WIFI_CAST_OTA_STATUS_REQUEST) {
				example_ota_status_request_cb(recv_data->mac, recv_data->data + sizeof(struct example_frame_head));
			} else if (hdr->type & WIFI_CAST_OTA_STATUS_RESPONSE) {
				example_ota_status_response_cb(recv_data->mac, recv_data->data + sizeof(struct example_frame_head));
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
	if (g_ota_recv_cb_q) {
		if (RTK_SUCCESS != rtos_queue_send(g_ota_recv_cb_q, &recv_data, 0)) {
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

static void example_ota_initial_scan(struct example_scan_info **info_list, int *num, u32 wait_ms)
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

static u32 example_firmware_download(struct example_ota_status *status)
{
	int ret = -1;
	u32 total_size = 0;
	u32 start_ms = rtos_time_get_current_system_time_ms();

	ctx = (ota_context *)rtos_mem_malloc(sizeof(ota_context));
	if (ctx == NULL) {
		RTK_LOGE(TAG, "%s, ctx malloc failed\n", __func__);
		goto exit;
	}

	memset(ctx, 0, sizeof(ota_context));

	ret = ota_update_init(ctx, (char *)host, PORT, (char *)resource, OTA_HTTP);
	if (ret != 0) {
		RTK_LOGE(TAG, "%s, ota_update_init failed\n", __func__);
		goto exit;
	}

	ret = ota_update_start(ctx);
	if (ret != 0) {
		RTK_LOGE(TAG, "%s, ota_update failed\n", __func__);
		goto exit;
	}
	total_size = ctx->otactrl->ImageLen;
	status->image_id = ctx->otactrl->ImgId;
	status->image_addr = ctx->otactrl->FlashAddr;
	status->checksum = ctx->otaTargetHdr->FileImgHdr[0].Checksum;
	RTK_LOGI(TAG, "%s, server firmware download is finished, spend time: %d ms, total size: %d\n",
			 __func__, rtos_time_get_current_system_time_ms() - start_ms, total_size);
exit:
	return total_size;
}

static void	example_ota_request_status(u8(*progress_array)[WIFI_CAST_OTA_PROGRESS_MAX],
									   struct example_ota_status *status, struct example_ota_result *result)
{
	if (example_send(WIFI_CAST_OTA_STATUS_REQUEST, WIFI_CAST_BROADCAST_MAC, (u8 *)status, sizeof(struct example_ota_status)) != WIFI_CAST_OK) {
		RTK_LOGE(TAG, "%s, send fail\n", __func__);
	}
	RTK_LOGI(TAG, "%s, send ota request\n", __func__);

	struct example_ota_status *recv_status = NULL;
	if (RTK_SUCCESS == rtos_queue_receive(g_ota_request_q, &recv_status, 3000)) {
		RTK_LOGI(TAG, "%s, recv ota req response, written_size: %d, progress_index: %d\n", __func__, recv_status->written_size, recv_status->progress_index);
		if (recv_status->written_size == status->total_size) {
			result->unfinished_num--;
			result->successed_num++;
		} else {
			if (recv_status->written_size == 0) {
				memset(progress_array, 0x0, status->packet_num / 8 + 1);
			} else {
				for (int i = 0; i < WIFI_CAST_OTA_PROGRESS_MAX
					 && (recv_status->progress_index * WIFI_CAST_OTA_PROGRESS_MAX  + i) * 8 < status->packet_num; i++) {
					progress_array[recv_status->progress_index][i] &= recv_status->progress_array[0][i];
				}
			}
		}
		rtos_mem_free(recv_status);
	}
}

static void example_firmware_send(struct example_ota_status *status, struct example_ota_result *result)
{
	u8(*progress_array)[WIFI_CAST_OTA_PROGRESS_MAX] = rtos_mem_malloc(status->packet_num / 8 + 1);
	if (!progress_array) {
		RTK_LOGE(TAG, "%s, malloc failed\n", __func__);
		return;
	}
	u16 data_len = WIFI_CAST_OTA_PACKET_SIZE;
	u8 *data = (u8 *)rtos_mem_zmalloc(data_len + sizeof(struct ota_packet_head));
	if (!data) {
		rtos_mem_free(progress_array);
		RTK_LOGE(TAG, "%s, malloc failed\n", __func__);
		return;
	}
	u32 start_tick = rtos_time_get_current_system_time_ms();

	for (int round = 0; round < WIFI_CAST_OTA_ROUND_MAX && result->unfinished_num > 0; round++) {
		memset(progress_array, 0xff, status->packet_num / 8 + 1);
		example_ota_request_status(progress_array, status, result);
		dump_buf("progress:", progress_array[0], status->packet_num / 8 + 1);
		for (int seq = 0; seq < status->packet_num && result->unfinished_num > 0; seq++) {
			if (!WIFI_CAST_OTA_GET_BITS(progress_array, seq)) {
				struct ota_packet_head *hdr = (struct ota_packet_head *)data;
				hdr->seq = seq;
				data_len = (seq == status->packet_num - 1) ?
						   (status->total_size - seq * WIFI_CAST_OTA_PACKET_SIZE) : WIFI_CAST_OTA_PACKET_SIZE;
				flash_stream_read(&flash_obj, ctx->otactrl->FlashAddr + seq * data_len, data_len, data + sizeof(struct ota_packet_head));
				if (WIFI_CAST_OK != example_send(WIFI_CAST_OTA_DATA, WIFI_CAST_BROADCAST_MAC, data, data_len + sizeof(struct ota_packet_head))) {
					rtos_time_delay_ms(2);
				}
			}
		}
		RTK_LOGI(TAG, "%s, round: %d\n", __func__, round);
	}
	RTK_LOGI(TAG, "%s, all devices upgrade done, successed_num: %d, spend time: %d ms\n",
			 __func__, result->successed_num, rtos_time_get_current_system_time_ms() - start_tick);
	rtos_mem_free(progress_array);
	rtos_mem_free(data);
	status->status = WIFI_CAST_OTA_RESET;
	if (WIFI_CAST_OK != example_send(WIFI_CAST_OTA_STATUS_REQUEST, WIFI_CAST_BROADCAST_MAC, (u8 *)status, sizeof(struct example_ota_status))) {
		RTK_LOGE(TAG, "%s, send failed\n", __func__);
		return;
	}
	rtos_time_delay_ms(1000);
	firmware_download_end();
}

static void example_ota_task(void *param)
{
	(void)param;
	struct example_scan_info *info_list = NULL;
	int info_num = 0;
	struct example_ota_status status = {0};
	struct example_ota_result result = {0};

	ota_role = WIFI_CAST_OTA_SENDER;

	u32 firmware_size = example_firmware_download(&status);
	if (firmware_size == 0) {
		RTK_LOGE(TAG, "%s, ota failed, firmware size: %d\n", __func__, firmware_size);
		goto exit;
	}

	example_ota_initial_scan(&info_list, &info_num, 3000);
	RTK_LOGI(TAG, "%s, scan info num: %d\n", __func__, info_num);

	if (info_num == 0) {
		goto exit;
	}

	for (int i = 0; i < info_num; i++) {
		example_add_node(info_list[i].mac);
	}

	status.packet_num = (firmware_size + WIFI_CAST_OTA_PACKET_SIZE - 1) / WIFI_CAST_OTA_PACKET_SIZE;
	status.total_size = firmware_size;
	result.unfinished_num = info_num;
	rtos_queue_create(&g_ota_request_q, result.unfinished_num, sizeof(struct example_ota_status *));
	RTK_LOGI(TAG, "%s, start send firmware, total_size: %d bytes, packet_num: %d\n", __func__, status.total_size, status.packet_num);
	example_firmware_send(&status, &result);

exit:
	rtos_mem_free(g_info_list);
	g_info_list = NULL;
	ota_update_deinit(ctx);
	rtos_mem_free(ctx);
	rtos_task_delete(NULL);
}

static void example_app_ota(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_ota_task"), example_ota_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_ota_task\n\r");
	}
}

static void example_main_task(void *param)
{
	UNUSED(param);
	RTK_LOGI(TAG, "------------->start\n");

	wifi_cast_config_t config = WIFI_CAST_INIT_CONFIG_DEFAULT();
	WIFI_CAST_ERROR_CHECK(wifi_cast_init(&config));
	WIFI_CAST_ERROR_CHECK(wifi_cast_register_recv_cb(example_recv_callback));

	rtos_queue_create(&g_scan_report_q, 16, sizeof(struct example_scan_info));
	rtos_queue_create(&g_ota_recv_cb_q, WIFI_CAST_OTA_PROGRESS_MAX, sizeof(struct example_cb_recv_data *));

	if (rtos_task_create(NULL, ((const char *)"example_recv_cb_task"), example_recv_cb_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_recv_cb_task\n\r");
	}

	rtos_task_delete(NULL);
}

void app_example(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_main_task"), example_main_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_main_task\n\r");
	}
}

void WifiCastTestApp(u8  *argv[])
{
	if (strcmp((const char *)argv[0], "download") == 0) {
		example_app_ota();
	} else if (_strcmp((const char *)argv[0], "help") == 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\twificast \n");
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t download  : start self firmware upgrade and help other devices upgrade\n");
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

