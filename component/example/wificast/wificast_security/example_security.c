#include "wifi_intf_drv_to_app_cast.h"
#include "os_wrapper.h"
#include "PinNames.h"
#include "serial_api.h"
#include "serial_ex_api.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdh.h"

static const char *TAG = "example_main";
static const char *pers = "wifi_cast_sec";
#define WIFI_CAST_UART_DATA		BIT(13)
#define WIFI_CAST_SEC_SCAN_REQUEST		BIT(12)
#define WIFI_CAST_SEC_SCAN_RESPONSE		BIT(11)
#define WIFI_CAST_SEC_PUBKEY_EXCHANGE_REQUEST	BIT(10)
#define WIFI_CAST_SEC_PUBKEY_EXCHANGE_RESPONSE	BIT(9)

#define UART_IDX  0
#define UART_BAUD_RATE 1500000
#define UART_TX_PIN    PA_28
#define UART_RX_PIN    PA_29

#define UART_RX_BUF_SZ	200

struct example_frame_head {
	u16 type;
	u16 len;
} __attribute__((packed));

struct example_sec_info {
	u8 mac[6];
} __attribute__((packed));

struct example_node_list {
	u8 is_init;
	int size;
	struct example_sec_info info[MAX_NODE_NUM];
};

struct pubkey_exchange_info {
	u16 type;
	u8 mac[6];
	u8 pubkey[MBEDTLS_ECP_MAX_PT_LEN];
	int pubkey_len;
} __attribute__((packed));

struct example_keypair {
	u8 pubkey[MBEDTLS_ECP_MAX_PT_LEN];
	int pubkey_len;
	u8 privkey[MBEDTLS_ECP_MAX_BYTES];
	int privkey_len;
	u8 shared_key[MBEDTLS_ECP_MAX_PT_LEN];
	int shared_key_len;
};

static serial_t g_uart_obj;
static rtos_queue_t g_scan_report_q;
static rtos_queue_t g_pubkey_exchange_q;
static struct example_sec_info *g_info_list = NULL;
static struct example_node_list g_node_list = {0};
static struct example_keypair g_keypair;

static int generate_keypair(void);
static int compute_shared_key(u8 *pubkey, int pubkey_len);

static void dump_buf(char *info, u8 *buf, int len)
{
	int i;

	RTK_LOGI(TAG, "%s\n", info);
	RTK_LOGI(TAG, "len: %d\n", len);
	for (i = 0; i < len; i++) {
		DiagPrintf("%s%02X%s", i % 16 == 0 ? "\r\n\t" : " ", buf[i], i == len - 1 ? "\r\n" : "");
	}
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

static wcast_err_t example_set_key(u8 *mac, u8 *key)
{
	wifi_cast_node_t node = {0};
	memcpy(node.mac, mac, 6);
	memcpy(node.key, key, WIFI_CAST_KEY_LEN);
	node.encrypt = 1;
	return wifi_cast_add_node(&node);
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

static void example_security_scan_request_cb(u8 *src_mac)
{
	struct example_sec_info info = {0};
	rtw_mac_t self_mac = {0};

	wifi_get_mac_address(0, &self_mac, 0);
	memcpy(info.mac, self_mac.octet, 6);

	if (example_send(WIFI_CAST_SEC_SCAN_RESPONSE, WIFI_CAST_BROADCAST_MAC, (u8 *)&info, sizeof(info)) != WIFI_CAST_OK) {
		RTK_LOGE(TAG, "%s, send fail\n", __func__);
		return;
	}

	if (example_add_node(src_mac) != WIFI_CAST_OK) {
		return;
	}

	RTK_LOGI(TAG, "%s, recv scan request from src_mac: "MAC_FMT"\n", __func__, MAC_ARG(src_mac));
	RTK_LOGI(TAG, "%s, send response success\n", __func__);
}

static void example_security_scan_response_cb(u8 *mac)
{
	struct example_sec_info info = {0};
	memcpy(info.mac, mac, 6);

	RTK_LOGD(TAG, "%s, recv response from"MAC_FMT"\n", __func__, MAC_ARG(mac));

	if (g_scan_report_q) {
		if (SUCCESS != rtos_queue_send(g_scan_report_q, &info, 0)) {
			RTK_LOGW(TAG, "%s, send queue failed\n", __func__);
			return;
		}
	}
}

static void example_security_pubkey_exchange_request_cb(u8 *src_mac, u8 *pubkey, int pubkey_len)
{
	RTK_LOGI(TAG, "%s, recv pubkey exchange from"MAC_FMT"\n", __func__, MAC_ARG(src_mac));
	RTK_LOGI(TAG, "%s, start generate_keypair\n", __func__);
	if (generate_keypair() != 0) {
		RTK_LOGE(TAG, "%s, generate keypair fail\n", __func__);
		return;
	}
	RTK_LOGI(TAG, "%s, generate_keypair ok\n", __func__);
	if (example_send(WIFI_CAST_SEC_PUBKEY_EXCHANGE_RESPONSE, src_mac, g_keypair.pubkey, g_keypair.pubkey_len) != WIFI_CAST_OK) {
		RTK_LOGE(TAG, "%s, send fail\n", __func__);
		return;
	}
	RTK_LOGI(TAG, "%s, send pubkey exchange to"MAC_FMT"\n", __func__, MAC_ARG(src_mac));

	if (compute_shared_key(pubkey, pubkey_len) != 0) {
		RTK_LOGE(TAG, "%s, compute_shared_key fail\n", __func__);
		return;
	}
	/* set key for node */
	if (example_set_key(src_mac, g_keypair.shared_key) != WIFI_CAST_OK) {
		RTK_LOGE(TAG, "%s, example_set_key fail\n", __func__);
		return;
	}
}

static void example_security_pubkey_exchange_response_cb(u8 *src_mac, u8 *pubkey, int pubkey_len)
{
	RTK_LOGI(TAG, "%s, recv pubkey exchange from"MAC_FMT"\n", __func__, MAC_ARG(src_mac));

	if (compute_shared_key(pubkey, pubkey_len) != 0) {
		RTK_LOGE(TAG, "%s, compute_shared_key fail\n", __func__);
		return;
	}
	/* set key for node */
	if (example_set_key(src_mac, g_keypair.shared_key) != WIFI_CAST_OK) {
		RTK_LOGE(TAG, "%s, example_set_key fail\n", __func__);
		return;
	}
}

static void example_recv_callback(wifi_cast_node_t *pnode, unsigned char *buf, unsigned int len, signed char rssi)
{
	static u32 count = 0;
	if (len < sizeof(struct example_frame_head)) {
		return;
	}

	struct example_frame_head *hdr = (struct example_frame_head *)buf;
	if (hdr->type & WIFI_CAST_UART_DATA) {
		RTK_LOGI(TAG, MAC_FMT", rssi: %d, recv count: %d, size: %d, data: %s\n",
				 MAC_ARG(pnode->mac), rssi, ++count, hdr->len, buf + sizeof(struct example_frame_head));
	} else if (hdr->type & WIFI_CAST_SEC_SCAN_REQUEST) {
		example_security_scan_request_cb(pnode->mac);
	} else if (hdr->type & WIFI_CAST_SEC_SCAN_RESPONSE) {
		example_security_scan_response_cb(buf + sizeof(struct example_frame_head));
	} else if (hdr->type & WIFI_CAST_SEC_PUBKEY_EXCHANGE_REQUEST ||
			   hdr->type & WIFI_CAST_SEC_PUBKEY_EXCHANGE_RESPONSE) {
		struct pubkey_exchange_info *info = NULL;
		info = (struct pubkey_exchange_info *)rtos_mem_zmalloc(sizeof(struct pubkey_exchange_info));
		if (!info) {
			RTK_LOGE(TAG, "%s, pubkey_exchange_info malloc failed\n", __func__);
			return;
		}
		info->type = hdr->type;
		info->pubkey_len = hdr->len;
		memcpy(info->mac, pnode->mac, 6);
		memcpy(info->pubkey, buf + sizeof(struct example_frame_head), hdr->len);
		if (g_pubkey_exchange_q) {
			if (SUCCESS != rtos_queue_send(g_pubkey_exchange_q, &info, 0)) {
				RTK_LOGE(TAG, "%s, send queue failed\n", __func__);
				rtos_mem_free(info);
				return;
			}
		} else {
			rtos_mem_free(info);
		}
	}
}

static void pubkey_exchange_task(void *param)
{
	(void)param;
	struct pubkey_exchange_info *info = NULL;

	while (1) {
		if (SUCCESS == rtos_queue_receive(g_pubkey_exchange_q, &info, 0)) {
			if (info->type & WIFI_CAST_SEC_PUBKEY_EXCHANGE_REQUEST) {
				example_security_pubkey_exchange_request_cb(info->mac, info->pubkey, info->pubkey_len);
			} else if (info->type & WIFI_CAST_SEC_PUBKEY_EXCHANGE_RESPONSE) {
				example_security_pubkey_exchange_response_cb(info->mac, info->pubkey, info->pubkey_len);
			}
			rtos_mem_free(info);
		}
		rtos_time_delay_ms(10);
	}
	rtos_task_delete(NULL);
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

	while (1) {
		size = serial_recv_stream_timeout(&g_uart_obj, (char *)(rx_buf + sizeof(struct example_frame_head)), UART_RX_BUF_SZ, 100, task_yield);
		if (!size) {
			RTK_LOGD(TAG, "%s, uart recv size: %d\n", __func__, size);
			continue;
		}
		hdr->len = size;
		if (g_node_list.is_init) {
			for (int i = 0; i < g_node_list.size; i++) {
				memcpy(node.mac, g_node_list.info[i].mac, ETH_ALEN);
				if ((dst_node = wifi_cast_get_node_info(&node)) != NULL) {
					if (wifi_cast_send(dst_node, rx_buf, sizeof(struct example_frame_head) + size, &info) != WIFI_CAST_OK) {
						RTK_LOGE(TAG, "%s, send fail\n", __func__);
						continue;
					}
					RTK_LOGI(TAG, "send count: %d, size: %d, data: %s\n", ++count, size, rx_buf + sizeof(struct example_frame_head));
				}
			}
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

	if (rtos_task_create(NULL, ((const char *)"example_uart_read_task"), example_uart_read_task, NULL, 512 * 4, 1) != SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_uart_read_task\n\r");
	}
}

static int entropy_source(void *data, u8 *output, int len, int *olen)
{
	(void)data;
	u32 seed;

	seed = rand();
	if (len > (int)sizeof(seed)) {
		len = sizeof(seed);
	}

	memcpy(output, &seed, len);

	*olen = len;
	return 0;
}

static int compute_shared_key(u8 *pubkey, int pubkey_len)
{
	int ret;
	u8 buf[MBEDTLS_ECP_MAX_PT_LEN];

	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_mpi secret;
	mbedtls_ecp_keypair ctx;

	mbedtls_ecp_keypair_init(&ctx);

	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);
	mbedtls_mpi_init(&secret);

	mbedtls_entropy_add_source(&entropy, (mbedtls_entropy_f_source_ptr)entropy_source, NULL,
							   MBEDTLS_ENTROPY_MAX_GATHER,
							   MBEDTLS_ENTROPY_SOURCE_STRONG);
	ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
								(const unsigned char *)pers,
								strlen(pers));
	if (ret != 0) {
		RTK_LOGE(TAG, "%s, mbedtls_ctr_drbg_seed fail, ret: %d\n", __func__, ret);
		goto exit;
	}
	ret = mbedtls_ecp_group_load(&ctx.grp, MBEDTLS_ECP_DP_SECP256R1);
	if (ret != 0) {
		RTK_LOGE(TAG, "%s, mbedtls_ecp_group_load fail, ret: %d\n", __func__, ret);
		goto exit;
	}

	/* load pubkey: Q */
	buf[0] = 0x04; 	/* format type */
	memcpy(&buf[1], pubkey, pubkey_len);
	dump_buf("pubkey:", buf, pubkey_len + 1);
	mbedtls_ecp_point_read_binary(&ctx.grp, &ctx.Q, buf, pubkey_len + 1);

	dump_buf("privkey:", g_keypair.privkey, g_keypair.privkey_len);
	/* load privkey: d */
	mbedtls_mpi_read_binary(&ctx.d, g_keypair.privkey, g_keypair.privkey_len);

	ret = mbedtls_ecdh_compute_shared(&ctx.grp, &secret, &ctx.Q, &ctx.d, mbedtls_ctr_drbg_random, &ctr_drbg);
	if (ret != 0) {
		RTK_LOGE(TAG, "%s, mbedtls_ecdh_compute_shared fail, ret: %d\n", __func__, ret);
		goto exit;
	}

	memset(g_keypair.shared_key, 0, sizeof(g_keypair.shared_key));
	mbedtls_mpi_write_binary(&secret, g_keypair.shared_key, mbedtls_mpi_size(&secret));
	g_keypair.shared_key_len = mbedtls_mpi_size(&secret);

	dump_buf("shared_key:", g_keypair.shared_key, g_keypair.shared_key_len);

exit:
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
	mbedtls_ecp_keypair_free(&ctx);
	mbedtls_mpi_free(&secret);

	return ret;
}

static int generate_keypair(void)
{
	int ret;
	size_t olen;
	u8 buf[MBEDTLS_ECP_MAX_PT_LEN];

	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_ecp_keypair ctx;
	mbedtls_mpi secret;

	mbedtls_ecp_keypair_init(&ctx);
	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);
	mbedtls_mpi_init(&secret);

	mbedtls_entropy_add_source(&entropy, (mbedtls_entropy_f_source_ptr)entropy_source, NULL,
							   MBEDTLS_ENTROPY_MAX_GATHER,
							   MBEDTLS_ENTROPY_SOURCE_STRONG);
	ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
								(const unsigned char *) pers,
								strlen(pers));
	if (ret != 0) {
		RTK_LOGE(TAG, "%s, mbedtls_ctr_drbg_seed fail, ret: %d\n", __func__, ret);
		goto exit;
	}

	ret = mbedtls_ecp_group_load(&ctx.grp, MBEDTLS_ECP_DP_SECP256R1);
	if (ret != 0) {
		RTK_LOGE(TAG, "%s, mbedtls_ctr_drbg_seed fail, ret: %d\n", __func__, ret);
		goto exit;
	}

	ret = mbedtls_ecdh_gen_public(&ctx.grp, &ctx.d, &ctx.Q, mbedtls_ctr_drbg_random, &ctr_drbg);
	if (ret != 0) {
		RTK_LOGE(TAG, "%s, mbedtls_ecdh_gen_public fail, ret: %d\n", __func__, ret);
		goto exit;
	}

	memset(buf, 0, sizeof(buf));
	mbedtls_ecp_point_write_binary(&ctx.grp, &ctx.Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &olen, buf, sizeof(buf));
	memcpy(g_keypair.pubkey, buf + 1, olen - 1);
	g_keypair.pubkey_len = olen - 1;
	dump_buf("pubkey:", g_keypair.pubkey, g_keypair.pubkey_len);

	memset(g_keypair.privkey, 0, sizeof(g_keypair.privkey));
	mbedtls_mpi_write_binary(&ctx.d, g_keypair.privkey, mbedtls_mpi_size(&ctx.d));
	g_keypair.privkey_len = mbedtls_mpi_size(&ctx.d);
	dump_buf("privkey:", g_keypair.privkey, g_keypair.privkey_len);

	ret = mbedtls_ecdh_compute_shared(&ctx.grp, &secret, &ctx.Q, &ctx.d, mbedtls_ctr_drbg_random, &ctr_drbg);
	if (ret != 0) {
		RTK_LOGE(TAG, "%s, mbedtls_ecdh_compute_shared fail, ret: %d\n", __func__, ret);
		goto exit;
	}

	memset(buf, 0, sizeof(buf));
	/* show public parameter */
	mbedtls_mpi_write_binary(&secret, buf, mbedtls_mpi_size(&secret));
	dump_buf("shared key:", buf, mbedtls_mpi_size(&secret));

exit:
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
	mbedtls_ecp_keypair_free(&ctx);
	return ret;
}

static void example_security_initial_scan(struct example_sec_info **info_list, int *num, u32 wait_ms)
{
	struct example_sec_info info = {0};
	int info_num = 0;

	do {
		example_send(WIFI_CAST_SEC_SCAN_REQUEST, WIFI_CAST_BROADCAST_MAC, NULL, 0);
		if (SUCCESS == rtos_queue_receive(g_scan_report_q, &info, 50)) {
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
			g_info_list = (struct example_sec_info *)rtos_mem_realloc(g_info_list, (info_num + 1) * sizeof(struct example_sec_info));
			memcpy(g_info_list[info_num].mac, info.mac, 6);
			info_num++;
		}
		wait_ms -= 50;
	} while (wait_ms > 0);

	*info_list = g_info_list;
	*num = info_num;
}

static void example_security_scan_task(void *param)
{
	(void)param;
	struct example_sec_info *info_list = NULL;
	int info_num = 0;
	example_security_initial_scan(&info_list, &info_num, 3000);
	RTK_LOGI(TAG, "%s, scan info num: %d\n", __func__, info_num);

	if (info_num == 0) {
		goto exit;
	}

	RTK_LOGI(TAG, "%s, start generate_keypair\n", __func__);

	if (generate_keypair() != 0) {
		RTK_LOGE(TAG, "%s, generate keypair fail\n", __func__);
		goto exit;
	}
	RTK_LOGI(TAG, "%s, generate_keypair ok\n", __func__);

	for (int i = 0; i < info_num; i++) {
		example_add_node(info_list[i].mac);
		if (example_send(WIFI_CAST_SEC_PUBKEY_EXCHANGE_REQUEST, info_list[i].mac, g_keypair.pubkey, g_keypair.pubkey_len) != WIFI_CAST_OK) {
			RTK_LOGE(TAG, "%s, send fail\n", __func__);
			continue;
		}
		RTK_LOGI(TAG, "%s, send pubkey exchange to"MAC_FMT"\n", __func__, MAC_ARG(info_list[i].mac));
		rtos_time_delay_ms(10);
	}
exit:
	rtos_mem_free(g_info_list);
	g_info_list = NULL;
	rtos_task_delete(NULL);
}

static void example_security_scan(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_security_scan_task"), example_security_scan_task, NULL, 1024 * 4, 1) != SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_security_scan_task\n\r");
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
	rtos_queue_create(&g_scan_report_q, 16, sizeof(struct example_sec_info));
	rtos_queue_create(&g_pubkey_exchange_q, 16, sizeof(struct pubkey_exchange_info *));

	if (rtos_task_create(NULL, ((const char *)"pubkey_exchange_task"), pubkey_exchange_task, NULL, 1024 * 4, 1) != SUCCESS) {
		RTK_LOGE(TAG, "Failed to create pubkey_exchange_task\n\r");
	}

	rtos_task_delete(NULL);
}

void app_example(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_main_task"), example_main_task, NULL, 1024 * 4, 1) != SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_main_task\n\r");
	}
}

void WifiCastTestApp(u8  *argv[])
{
	if (_strcmp((const char *)argv[0], "scan") == 0) {
		example_security_scan();
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

	return _TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   wifi_cast_test_cmd_table[] = {
	{
		(const u8 *)"wificast", 3, CmdWifiCastTest, (const u8 *)"\tWIFI \n"
	},
};

