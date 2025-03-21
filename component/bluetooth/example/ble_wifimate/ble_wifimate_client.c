/*
*******************************************************************************
* Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <basic_types.h>
#include <rtk_bt_def.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_gattc.h>
#include <rtk_bt_att_defs.h>
#include <rtk_gcs_client.h>
#include <rtk_bt_le_gap.h>
#include <rtk_client_config.h>
#include <ble_wifimate_service.h>

enum ble_wifimate_charac_index_e {
	BLE_WIFIMATE_CHAR_NEGOTIATE_KEY_IDX = 0,
	BLE_WIFIMATE_CHAR_WIFI_SCAN_ENABLE_IDX = 1,
	BLE_WIFIMATE_CHAR_WIFI_SCAN_INFO_IDX = 2,
	BLE_WIFIMATE_CHAR_WIFI_CONNECT_ENABLE_IDX = 3,
	BLE_WIFIMATE_CHAR_WIFI_CONNECT_STATE_IDX = 4,
	BLE_WIFIMATE_CHAR_NUM,
};

struct ble_wifimate_charac_db_t {
	uint8_t  properties;
	uint16_t uuid;
	uint16_t char_val_handle;
	uint16_t cccd_handle;
};

struct ble_wifimate_client_db_t {
	client_disc_state_t                 disc_state;
	uint16_t                            start_handle;
	uint16_t                            end_handle;
	struct ble_wifimate_charac_db_t     char_db[BLE_WIFIMATE_CHAR_NUM];
};

static uint16_t s_mtu_size = 0;
static struct ble_wifimate_client_db_t *ble_wifimate_database[RTK_BLE_GAP_MAX_LINKS] = {0};
static struct ble_wifimate_char_send_data_t s_multi_write = {0};
static struct ble_wifimate_char_recv_data_t s_multi_recv = {0};

static uint32_t ble_wifimate_client_checksum_cal(uint16_t len, uint8_t *data)
{
	uint32_t checksum = 0;
	for (uint16_t i = 0; i < len; ++i) {
		checksum += *(data + i);
	}

	return checksum;
}

static uint16_t ble_wifimate_client_database_free(uint8_t conn_id)
{
	if (!ble_wifimate_database[conn_id]) {
		return RTK_BT_OK;
	}

	osif_mem_free(ble_wifimate_database[conn_id]);
	ble_wifimate_database[conn_id] = NULL;

	return RTK_BT_OK;
}

static uint16_t ble_wifimate_char_multi_recv_data_init(uint16_t total_len, uint8_t seg_num, uint32_t checksum)
{
	if (s_multi_recv.data) {
		osif_mem_free((void *)s_multi_recv.data);
		s_multi_recv.data = NULL;
	}

	memset(&s_multi_recv, 0, sizeof(s_multi_recv));
	s_multi_recv.flag = true;
	s_multi_recv.total_len = total_len;
	s_multi_recv.seg_num = seg_num;
	s_multi_recv.checksum = checksum;
	s_multi_recv.seg_idx = 0;
	s_multi_recv.offset = 0;
	s_multi_recv.data = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, total_len);
	if (!s_multi_recv.data) {
		BT_LOGE("[APP] BLE WifiMate client init multi indication receive data: can't alloc memory, len=%u\r\n", total_len);
		memset(&s_multi_recv, 0, sizeof(s_multi_recv));
		return RTK_BT_ERR_NO_MEMORY;
	}
	memset(s_multi_recv.data, 0, total_len);

	return RTK_BT_OK;
}

static void ble_wifimate_char_multi_recv_data_deinit(void)
{
	if (s_multi_recv.data) {
		osif_mem_free((void *)s_multi_recv.data);
	}
	memset(&s_multi_recv, 0, sizeof(s_multi_recv));
}

static uint16_t ble_wifimate_char_multi_write_data_init(struct wifi_conn_config_t *conn_info)
{
	uint8_t offset = 0;
	uint8_t char_data_len;

	if (!conn_info) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	char_data_len = CHAR_WIFI_CONN_ENABLE_SEG_FIRST_ELE_LEN + conn_info->ssid_len + \
					CHAR_WIFI_CONN_ENABLE_SEG_THIRD_ELE_LEN + CHAR_WIFI_CONN_ENABLE_SEG_FORTH_ELE_LEN + \
					conn_info->password_len;

	if (s_multi_write.data) {
		osif_mem_free((void *)s_multi_write.data);
	}

	memset(&s_multi_write, 0, sizeof(s_multi_write));
	s_multi_write.flag = true;
	s_multi_write.total_len = char_data_len;
	s_multi_write.seg_num = (uint8_t)ceil((double)char_data_len / (s_mtu_size - 3 - 1));
	s_multi_write.data = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, char_data_len);
	if (!s_multi_write.data) {
		BT_LOGE("[APP] BLE WifiMate client Write failed: can't alloc memory\r\n");
		memset(&s_multi_write, 0, sizeof(s_multi_write));
		return RTK_BT_ERR_NO_MEMORY;
	}
	memset(s_multi_write.data, 0, char_data_len);

	memcpy(s_multi_write.data, &conn_info->ssid_len, CHAR_WIFI_CONN_ENABLE_SEG_FIRST_ELE_LEN);
	offset = CHAR_WIFI_CONN_ENABLE_SEG_FIRST_ELE_LEN;
	memcpy(s_multi_write.data + offset, conn_info->ssid, conn_info->ssid_len);
	offset += conn_info->ssid_len;
	memcpy(s_multi_write.data + offset, &conn_info->security, CHAR_WIFI_CONN_ENABLE_SEG_THIRD_ELE_LEN);
	offset += CHAR_WIFI_CONN_ENABLE_SEG_THIRD_ELE_LEN;
	memcpy(s_multi_write.data + offset, &conn_info->password_len, CHAR_WIFI_CONN_ENABLE_SEG_FORTH_ELE_LEN);
	offset += CHAR_WIFI_CONN_ENABLE_SEG_FORTH_ELE_LEN;
	memcpy(s_multi_write.data + offset,  conn_info->password, conn_info->password_len);

	s_multi_write.checksum = ble_wifimate_client_checksum_cal(s_multi_write.total_len, s_multi_write.data);

	return RTK_BT_OK;
}

static void ble_wifimate_char_multi_write_data_deinit(void)
{
	if (s_multi_write.data) {
		osif_mem_free((void *)s_multi_write.data);
	}
	memset(&s_multi_write, 0, sizeof(s_multi_write));
}

static uint16_t ble_wifimate_client_negotiate_key(uint16_t conn_handle, struct key_negotiate_config_t *config)
{
	uint8_t conn_id;
	uint16_t ret;
	struct ble_wifimate_client_db_t *conn_db;
	rtk_bt_gattc_write_param_t write_param = {0};

	if (!config) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (config->algorithm != BLE_WIFIMATE_KEY_ENCODE_ALGORITHM_NONE) {
		BT_LOGE("[APP] BLE WifiMate key encryption algorithms are not currently supported !!!\r\n");
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	conn_db = ble_wifimate_database[conn_id];
	if (!conn_db) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	if (conn_db->disc_state != DISC_DONE) {
		BT_LOGE("[APP] BLE WifiMate client need discover service before write charac !!!\r\n");
		return RTK_BT_ERR_STATE_INVALID;
	}

	write_param.conn_handle = conn_handle;
	write_param.profile_id = BLE_WIFIMATE_CLIENT_PROFILE_ID;
	write_param.type = RTK_BT_GATT_CHAR_WRITE_REQ;
	write_param.handle = conn_db->char_db[BLE_WIFIMATE_CHAR_NEGOTIATE_KEY_IDX].char_val_handle;
	write_param.length = BLE_WIFIMATE_CHAR_NOGOTIATE_KEY_LEN;
	write_param.data = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, write_param.length);
	if (!write_param.data) {
		BT_LOGE("[APP] BLE WifiMate client Write failed: can't alloc memory\r\n");
		return RTK_BT_ERR_NO_MEMORY;
	}

	memcpy(write_param.data, &config->algorithm, CHAR_NEGOTIATE_KEY_FIRST_ELE_LEN);
	memcpy((uint8_t *)write_param.data + CHAR_NEGOTIATE_KEY_FIRST_ELE_LEN, &config->key, CHAR_NEGOTIATE_KEY_SECOND_ELE_LEN);

	ret = rtk_bt_gattc_write(&write_param);

	osif_mem_free(write_param.data);
	return ret;
}

static uint16_t ble_wifimate_write_wlconn_enable_segment(uint16_t value_handle, uint16_t conn_handle)
{
	int     remain = s_multi_write.total_len - s_multi_write.offset;
	uint16_t ret;
	rtk_bt_gattc_write_param_t write_param = {0};

	if (!s_multi_write.flag) {
		BT_LOGD("[APP] BLE WifiMate client no need write wifi connect enable charac.\r\n");
		return RTK_BT_OK;
	}

	if ((s_multi_write.seg_idx >= s_multi_write.seg_num) ||
		(remain <= 0)) {
		BT_LOGD("[APP] BLE WifiMate client write wifi connect enable charac done.\r\n");
		ble_wifimate_char_multi_write_data_deinit();
		return RTK_BT_OK;
	}

	write_param.conn_handle = conn_handle;
	write_param.profile_id = BLE_WIFIMATE_CLIENT_PROFILE_ID;
	write_param.type = RTK_BT_GATT_CHAR_WRITE_REQ;
	write_param.handle = value_handle;
	write_param.length = (remain < (s_mtu_size - 3 - 1)) ? (remain + 1) : (s_mtu_size - 3);
	write_param.data = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, write_param.length);
	if (!write_param.data) {
		BT_LOGE("[APP] BLE WifiMate client Write failed: can't alloc memory\r\n");
		ble_wifimate_char_multi_write_data_deinit();
		return RTK_BT_ERR_NO_MEMORY;
	}

	memcpy(write_param.data, &s_multi_write.seg_idx, 1);

	memcpy((uint8_t *)write_param.data + 1, s_multi_write.data + s_multi_write.offset, write_param.length - 1);
	s_multi_write.offset += write_param.length - 1;
	s_multi_write.seg_idx++;

	BT_DUMPD("[APP] GATTC write:\r\n", write_param.data, write_param.length);
	ret = rtk_bt_gattc_write(&write_param);
	if (ret != RTK_BT_OK) {
		ble_wifimate_char_multi_write_data_deinit();
	}

	osif_mem_free(write_param.data);
	return ret;
}

static uint16_t ble_wifimate_write_wlconn_enable_initial(uint16_t conn_handle, struct wifi_conn_config_t *conn_info)
{
	uint8_t conn_id;
	uint8_t offset = 0;
	uint16_t ret;
	struct ble_wifimate_client_db_t *conn_db;
	rtk_bt_gattc_write_param_t write_param = {0};

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	conn_db = ble_wifimate_database[conn_id];
	if (!conn_db) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	if (conn_db->disc_state != DISC_DONE) {
		BT_LOGE("[APP] BLE WifiMate client need discover service before write charac !!!\r\n");
		return RTK_BT_ERR_STATE_INVALID;
	}

	ret = ble_wifimate_char_multi_write_data_init(conn_info);
	if (ret != RTK_BT_OK) {
		BT_LOGE("[APP] BLE WifiMate client init multi write fail!\r\n");
		return ret;
	}

	write_param.conn_handle = conn_handle;
	write_param.profile_id = BLE_WIFIMATE_CLIENT_PROFILE_ID;
	write_param.type = RTK_BT_GATT_CHAR_WRITE_REQ;
	write_param.handle = conn_db->char_db[BLE_WIFIMATE_CHAR_WIFI_CONNECT_ENABLE_IDX].char_val_handle;
	write_param.length = CHAR_WIFI_CONN_ENABLE_INITIAL_FIRST_ELE_LEN + \
						 CHAR_WIFI_CONN_ENABLE_INITIAL_SECOND_ELE_LEN + \
						 CHAR_WIFI_CONN_ENABLE_INITIAL_THIRD_ELE_LEN;
	write_param.data = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, write_param.length);
	if (!write_param.data) {
		BT_LOGE("[APP] BLE WifiMate client Write failed: can't alloc memory\r\n");
		ble_wifimate_char_multi_write_data_deinit();
		return RTK_BT_ERR_NO_MEMORY;
	}
	memset(write_param.data, 0, write_param.length);

	memcpy(write_param.data, &s_multi_write.total_len, CHAR_WIFI_CONN_ENABLE_INITIAL_FIRST_ELE_LEN);
	offset = CHAR_WIFI_CONN_ENABLE_INITIAL_FIRST_ELE_LEN;

	memcpy((uint8_t *)write_param.data + offset, &s_multi_write.seg_num, CHAR_WIFI_CONN_ENABLE_INITIAL_SECOND_ELE_LEN);
	offset += CHAR_WIFI_CONN_ENABLE_INITIAL_SECOND_ELE_LEN;

	memcpy((uint8_t *)write_param.data + offset, &s_multi_write.checksum, CHAR_WIFI_CONN_ENABLE_INITIAL_THIRD_ELE_LEN);

	BT_LOGD("%s: total_len=%u seg_num=%d checksum=%u write_param.length=%d\r\n", __func__,
			s_multi_write.total_len, s_multi_write.seg_num, s_multi_write.checksum, write_param.length);
	BT_DUMPD("[APP] GATTC write:\r\n", write_param.data, write_param.length);

	ret = rtk_bt_gattc_write(&write_param);
	if (ret != RTK_BT_OK) {
		ble_wifimate_char_multi_write_data_deinit();
	}

	osif_mem_free(write_param.data);
	return ret;
}

uint16_t ble_wifimate_client_wifi_connect(uint16_t conn_handle, struct wifi_conn_config_t *conn_info)
{
	return ble_wifimate_write_wlconn_enable_initial(conn_handle, conn_info);
}

static uint16_t ble_wifimate_client_srv_discover(uint16_t conn_handle)
{
	uint8_t conn_id;
	uint16_t ret = 0;
	uint16_t srv_uuid = BLE_WIFIMATE_UUID_SRV;
	struct ble_wifimate_client_db_t *conn_db;
	rtk_bt_gattc_discover_param_t disc_param = {0};

	disc_param.profile_id = BLE_WIFIMATE_CLIENT_PROFILE_ID;
	disc_param.conn_handle = conn_handle;
	disc_param.type = RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID;
	disc_param.disc_primary_by_uuid.uuid_type = BT_UUID_TYPE_16;
	memcpy(disc_param.disc_primary_by_uuid.uuid, &srv_uuid, BT_UUID_SIZE_16);

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	conn_db = ble_wifimate_database[conn_id];
	if (!conn_db) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	ret = rtk_bt_gattc_discover(&disc_param);
	if (RTK_BT_OK != ret) {
		return ret;
	}

	conn_db->disc_state = DISC_START;
	return RTK_BT_OK;
}

static uint16_t ble_wifimate_client_desc_discover(uint16_t conn_handle)
{
	uint16_t ret = 0;
	rtk_bt_gattc_discover_param_t disc_param = {0};
	uint8_t conn_id;
	struct ble_wifimate_client_db_t *conn_db;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	conn_db = ble_wifimate_database[conn_id];
	if (!conn_db) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	disc_param.profile_id = BLE_WIFIMATE_CLIENT_PROFILE_ID;
	disc_param.conn_handle = conn_handle;
	disc_param.type = RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL;
	disc_param.disc_char_by_uuid.start_handle =
		conn_db->char_db[BLE_WIFIMATE_CHAR_NEGOTIATE_KEY_IDX].char_val_handle;
	disc_param.disc_char_by_uuid.end_handle = conn_db->end_handle;

	ret = rtk_bt_gattc_discover(&disc_param);
	if (RTK_BT_OK != ret) {
		return ret;
	}

	conn_db->disc_state = DISC_START;
	return RTK_BT_OK;
}

static uint16_t ble_wifimate_client_charac_discover(uint16_t conn_handle)
{
	uint16_t ret = 0;
	uint8_t conn_id;
	rtk_bt_gattc_discover_param_t disc_param = {0};
	struct ble_wifimate_client_db_t *conn_db = NULL;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	conn_db = ble_wifimate_database[conn_id];

	if (!conn_db) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	disc_param.profile_id = BLE_WIFIMATE_CLIENT_PROFILE_ID;
	disc_param.conn_handle = conn_handle;
	disc_param.type = RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL;
	disc_param.disc_char_all.start_handle = conn_db->start_handle;
	disc_param.disc_char_all.end_handle   = conn_db->end_handle;

	ret = rtk_bt_gattc_discover(&disc_param);
	if (RTK_BT_OK != ret) {
		return ret;
	}

	conn_db->disc_state = DISC_START;
	return RTK_BT_OK;
}

static void ble_wifimate_client_mtu_exchange_hdl(void *data)
{
	rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)data;

	if (!p_gatt_mtu_ind) {
		return;
	}

	if (p_gatt_mtu_ind->result == RTK_BT_OK) {
		BT_LOGA("[APP] GATTC mtu exchange success, mtu_size: %d, conn_handle: %d \r\n",
				p_gatt_mtu_ind->mtu_size, p_gatt_mtu_ind->conn_handle);
		s_mtu_size = p_gatt_mtu_ind->mtu_size;
		BT_LOGD("%s s_mtu_size=%d\r\n", __func__, s_mtu_size);
		ble_wifimate_client_srv_discover(p_gatt_mtu_ind->conn_handle);
	} else {
		BT_LOGE("[APP] GATTC mtu exchange fail \r\n");
	}

	return;
}

static uint16_t ble_wifimate_client_set_cccd_indicate(uint16_t conn_handle, uint8_t index, bool enable)
{
	uint16_t ret = 0;
	rtk_bt_gattc_update_cccd_param_t cccd_param = {0};
	struct ble_wifimate_client_db_t *conn_db;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		BT_LOGE("[APP] BLE WiFiMate client get conn id fail !!!\r\n");
		return RTK_BT_FAIL;
	}

	conn_db = ble_wifimate_database[conn_id];
	if (!conn_db) {
		BT_LOGE("[APP] BLE WiFiMate client conn_db invalid !!!\r\n");
		return RTK_BT_ERR_NO_ENTRY;
	}

	if (conn_db->disc_state != DISC_DONE) {
		BT_LOGE("[APP] BLE WiFiMate client need discover service before wrtie cccd !!!\r\n");
		return RTK_BT_ERR_STATE_INVALID;
	}

	cccd_param.profile_id = BLE_WIFIMATE_CLIENT_PROFILE_ID;
	cccd_param.conn_handle = conn_handle;
	cccd_param.bindicate = true;
	cccd_param.char_val_handle =
		conn_db->char_db[index].char_val_handle;
	cccd_param.cccd_handle =
		conn_db->char_db[index].cccd_handle;

	BT_LOGD("[APP] %s char_val_handle=0x%x cccd_handle=0x%x\r\n",
			__func__, cccd_param.char_val_handle, cccd_param.cccd_handle);

	if (enable) {
		ret = rtk_bt_gattc_enable_notify_or_indicate(&cccd_param);
	} else {
		ret = rtk_bt_gattc_disable_notify_or_indicate(&cccd_param);
	}
	BT_LOGD("[APP] BLE WiFiMate client set cccd indicate conn_handle=%d, index=%d, enable=%d ret=%d\r\n",
			conn_handle, index, enable, ret);

	return ret;
}

static void ble_wifimate_client_print_wifi_scan_info(void)
{
	uint8_t ap_num = 0;
	uint8_t ap_len = 0;
	uint32_t offset = 0;
	struct wifi_scan_info_t ap_info = {0};

	if (!s_multi_recv.flag || !s_multi_recv.data) {
		BT_LOGE("[APP] BLE WiFiMate s_multi_recv invalid\r\n");
		return;
	}
	for (uint32_t idx = 0; idx < s_multi_recv.total_len; idx += ap_len) {
		memset(&ap_info, 0, sizeof(ap_info));

		memcpy(&ap_info.ssid_len, s_multi_recv.data + offset, sizeof(uint8_t));
		offset += sizeof(uint8_t);

		memcpy(ap_info.ssid, s_multi_recv.data + offset, ap_info.ssid_len);
		offset += ap_info.ssid_len;

		memcpy(&ap_info.rssi, s_multi_recv.data + offset, sizeof(int16_t));
		offset += sizeof(int16_t);

		memcpy(&ap_info.security, s_multi_recv.data + offset, sizeof(uint8_t));
		offset += sizeof(uint8_t);

		memcpy(&ap_info.channel, s_multi_recv.data + offset, sizeof(uint8_t));
		offset += sizeof(uint8_t);

		ap_len = ap_info.ssid_len + 5;

		++ap_num;
		BT_LOGD("[APP] %s ap_info.ssid_len=%d offset=%u idx=%u ap_len=%d\r\n", __func__, ap_info.ssid_len, offset, idx, ap_len);
		BT_LOGA("[APP] WiFi scan info: ssid=%s, rssi=%d, security=%d, channel=%d\r\n",
				ap_info.ssid, ap_info.rssi, ap_info.security, ap_info.channel);
	}

	BT_LOGA("[APP] BLE WiFiMate wifi scan num=%d \r\n", ap_num);
}

static uint16_t ble_wifimate_client_receive_wifi_scan_info(uint16_t conn_handle, uint16_t len, uint8_t *value)
{
	uint16_t ret = RTK_BT_OK;

	if (!value) {
		BT_LOGA("[APP] BLE WiFiMate indicate wifi_scan_info value NULL %d!\r\n");
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (!s_multi_recv.flag) {
		uint16_t offset = 0;
		uint16_t total_len;
		uint8_t seg_num;
		uint32_t checksum;

		if (len != 7) {
			BT_LOGA("[APP] BLE WiFiMate indicate wifi_scan_info length invalid %d!\r\n", len);
			return RTK_BT_ERR_PARAM_INVALID;
		}

		memcpy(&total_len, value, sizeof(uint16_t));
		offset += sizeof(uint16_t);

		memcpy(&seg_num,   value + offset, sizeof(uint8_t));
		offset += sizeof(uint8_t);

		memcpy(&checksum,  value + offset, sizeof(uint32_t));

		BT_LOGD("[APP] %s total_len=%u, seg_num=%u, checksum=%u\r\n", __func__, total_len, seg_num, checksum);

		if (seg_num > 0) {
			ble_wifimate_char_multi_recv_data_init(total_len, seg_num, checksum);
		} else {
			BT_LOGA("[APP] BLE WiFiMate conn_handle=%d wifi scan failed!\r\n", conn_handle);
			return RTK_BT_FAIL;
		}
	} else {
		uint32_t checksum;
		memcpy(&s_multi_recv.seg_idx, value, 1);
		memcpy(s_multi_recv.data + s_multi_recv.offset, value + 1, len - 1);
		s_multi_recv.offset += len - 1;

		if (s_multi_recv.total_len == s_multi_recv.offset) {
			checksum = ble_wifimate_client_checksum_cal(s_multi_recv.total_len, s_multi_recv.data);
			BT_LOGD("%s: recv_checksum=%u, cal_checksum=%u\r\n",
					__func__, s_multi_recv.checksum, checksum);
			if (checksum == s_multi_recv.checksum) {
				BT_LOGA("[APP] BLE WiFiMate wifi scan info checksum pass!\r\n", conn_handle);
				ble_wifimate_client_print_wifi_scan_info();
			} else {
				ret = RTK_BT_FAIL;
				BT_LOGA("[APP] BLE WiFiMate wifi scan info checksum fail\r\n", conn_handle);
			}
			ble_wifimate_char_multi_recv_data_deinit();
		}
	}

	return ret;
}

static uint16_t ble_wifimate_client_receive_wifi_conn_state(uint16_t conn_handle, uint16_t len, uint8_t *value)
{
	uint8_t wifi_state;
	uint8_t err_code;

	if (!value) {
		BT_LOGA("[APP] BLE WiFiMate indicate wifi_conn_state value NULL %d!\r\n");
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (len != 2) {
		BT_LOGA("[APP] BLE WiFiMate indicate wifi_conn_state length invalid %d!\r\n", len);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	memcpy(&wifi_state, value, 1);
	memcpy(&err_code, value + 1, 1);

	BT_LOGA("[APP] BLE WiFiMate Client conn_handle=%d, wifi_state=%d, err_code=%d\r\n", conn_handle, wifi_state, err_code);
	return RTK_BT_OK;
}

static void ble_wifimate_client_indicate_hdl(void *data)
{
	rtk_bt_gattc_cccd_value_ind_t *indicate_ind = (rtk_bt_gattc_cccd_value_ind_t *)data;
	uint16_t att_handle = indicate_ind->value_handle;
	uint16_t conn_handle = indicate_ind->conn_handle;
	uint16_t len = indicate_ind->len;
	uint8_t *value = indicate_ind->value;
	uint8_t conn_id;
	struct ble_wifimate_client_db_t *conn_db;

	BT_DUMPD("[APP] GATTC indicate:\r\n", indicate_ind->value, indicate_ind->len);

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id)) {
		return;
	}

	conn_db = ble_wifimate_database[conn_id];
	if (!conn_db) {
		return;
	}

	for (uint8_t i = 0; i < BLE_WIFIMATE_CHAR_NUM; ++i) {
		if (att_handle == conn_db->char_db[i].char_val_handle) {
			if (conn_db->char_db[i].uuid == BLE_WIFIMATE_UUID_CHAR_WIFI_SCAN_INFO) {
				BT_LOGD("[APP] BLE WiFiMate indicate charac wifi scan info received\r\n");
				ble_wifimate_client_receive_wifi_scan_info(conn_handle, len, value);
			} else if (conn_db->char_db[i].uuid == BLE_WIFIMATE_UUID_CHAR_WIFI_CONNECT_STATE) {
				BT_LOGD("[APP] BLE WiFiMate indicate charac wifi connect state received\r\n");
				ble_wifimate_client_receive_wifi_conn_state(conn_handle, len, value);
			} else {
				BT_LOGA("[APP] BLE WiFiMate invalid indicate char index 0x%02x!\r\n", i);
			}
			break;
		}
	}
}

static void ble_wifimate_client_cccd_enable_hdl(void *data)
{
	rtk_bt_gattc_cccd_update_ind_t *cccd_update =
		(rtk_bt_gattc_cccd_update_ind_t *)data;
	rtk_bt_status_t status = cccd_update->status;
	uint16_t conn_handle = cccd_update->conn_handle;
	uint8_t conn_id;
	struct ble_wifimate_client_db_t *conn_db;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	conn_db = ble_wifimate_database[conn_id];
	if (!conn_db || RTK_BT_STATUS_DONE != status) {
		return;
	}

	if (conn_db->char_db[BLE_WIFIMATE_CHAR_WIFI_SCAN_INFO_IDX].cccd_handle == cccd_update->cccd_handle) {
		BT_LOGD("[APP] BLE WiFiMate client enable wifi scan info char indicate succeed\r\n");
		//enable wifi connect state char char cccd
		ble_wifimate_client_set_cccd_indicate(conn_handle, BLE_WIFIMATE_CHAR_WIFI_CONNECT_STATE_IDX, true);
	} else if (conn_db->char_db[BLE_WIFIMATE_CHAR_WIFI_CONNECT_STATE_IDX].cccd_handle == cccd_update->cccd_handle) {
		struct key_negotiate_config_t config = {0};
		config.algorithm = BLE_WIFIMATE_KEY_ENCODE_ALGORITHM_NONE;
		BT_LOGD("[APP] BLE WiFiMate client negotiate key...\r\n");
		ble_wifimate_client_negotiate_key(conn_handle, &config);
	}
}

void ble_wifimate_client_discover_res_hdl(void *data)
{
	uint16_t uuid = 0;
	rtk_bt_gattc_discover_ind_t *disc_res = (rtk_bt_gattc_discover_ind_t *)data;
	rtk_bt_status_t disc_status = disc_res->status;
	uint16_t conn_handle = disc_res->conn_handle;
	uint8_t conn_id;
	struct ble_wifimate_client_db_t *conn_db;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	conn_db = ble_wifimate_database[conn_id];

	if (!conn_db || conn_db->disc_state != DISC_START) {
		return;
	}

	if (RTK_BT_STATUS_CONTINUE == disc_status) {
		switch (disc_res->type) {
		case RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID:
			conn_db->start_handle = disc_res->disc_primary_all_per.start_handle;
			conn_db->end_handle   = disc_res->disc_primary_all_per.end_handle;
			break;
		case RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL: {
			uint8_t properties = disc_res->disc_char_all_per.properties;
			uint16_t value_handle = disc_res->disc_char_all_per.value_handle;
			struct ble_wifimate_charac_db_t *char_db = NULL;

			if (BT_UUID_TYPE_16 == disc_res->disc_char_all_per.uuid_type) {
				memcpy(&uuid, disc_res->disc_char_all_per.uuid, sizeof(uint16_t));
			}
			switch (uuid) {
			case BLE_WIFIMATE_UUID_CHAR_KEY_NEGOTIATE:
				char_db = &conn_db->char_db[BLE_WIFIMATE_CHAR_NEGOTIATE_KEY_IDX];
				BT_LOGD("[APP] BLE_WIFIMATE_UUID_CHAR_KEY_NEGOTIATE discover success\r\n");
				break;
			case BLE_WIFIMATE_UUID_CHAR_WIFI_SCAN_ENABLE:
				char_db = &conn_db->char_db[BLE_WIFIMATE_CHAR_WIFI_SCAN_ENABLE_IDX];
				BT_LOGD("[APP] BLE_WIFIMATE_UUID_CHAR_WIFI_SCAN_ENABLE discover success\r\n");
				break;
			case BLE_WIFIMATE_UUID_CHAR_WIFI_SCAN_INFO:
				char_db = &conn_db->char_db[BLE_WIFIMATE_CHAR_WIFI_SCAN_INFO_IDX];
				BT_LOGD("[APP] BLE_WIFIMATE_UUID_CHAR_WIFI_SCAN_INFO discover success\r\n");
				break;
			case BLE_WIFIMATE_UUID_CHAR_WIFI_CONNECT_ENABLE:
				char_db = &conn_db->char_db[BLE_WIFIMATE_CHAR_WIFI_CONNECT_ENABLE_IDX];
				BT_LOGD("[APP] BLE_WIFIMATE_UUID_CHAR_WIFI_CONNECT_ENABLE discover success\r\n");
				break;
			case BLE_WIFIMATE_UUID_CHAR_WIFI_CONNECT_STATE:
				char_db = &conn_db->char_db[BLE_WIFIMATE_CHAR_WIFI_CONNECT_STATE_IDX];
				BT_LOGD("[APP] BLE_WIFIMATE_UUID_CHAR_WIFI_CONNECT_STATE discover success\r\n");
				break;
			default:
				break;
			}

			if (char_db) {
				char_db->char_val_handle = value_handle;
				char_db->properties      = properties;
				char_db->uuid            = uuid;
			}
			break;
		}
		case RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL: {
			uint16_t desc_handle = disc_res->disc_descriptor_per.handle;
			memcpy(&uuid, disc_res->disc_descriptor_per.uuid, sizeof(uint16_t));
			if (BT_UUID_GATT_CCC_VAL == uuid) {
				if (desc_handle > conn_db->char_db[BLE_WIFIMATE_CHAR_WIFI_SCAN_INFO_IDX].char_val_handle &&
					desc_handle < conn_db->char_db[BLE_WIFIMATE_CHAR_WIFI_CONNECT_ENABLE_IDX].char_val_handle) {
					conn_db->char_db[BLE_WIFIMATE_CHAR_WIFI_SCAN_INFO_IDX].cccd_handle = desc_handle;
				} else if (desc_handle > conn_db->char_db[BLE_WIFIMATE_CHAR_WIFI_CONNECT_STATE_IDX].char_val_handle &&
						   desc_handle <= conn_db->end_handle) {
					conn_db->char_db[BLE_WIFIMATE_CHAR_WIFI_CONNECT_STATE_IDX].cccd_handle = desc_handle;
				}
			}
			break;
		}
		default:
			break;
		}
	} else if (RTK_BT_STATUS_DONE == disc_status) {
		switch (disc_res->type) {
		case RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID:
			if (0 == conn_db->start_handle && 0 == conn_db->end_handle) {
				conn_db->disc_state = DISC_FAILED;
				BT_LOGE("[APP] BLE WiFiMate client discover service failed\r\n");
			} else {
				if (ble_wifimate_client_charac_discover(conn_handle) != RTK_BT_OK) {
					BT_LOGE("[APP] BLE WiFiMate client discover characteristics failed\r\n");
				}
			}
			break;
		case RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL: {
			ble_wifimate_client_desc_discover(conn_handle);
			break;
		}
		case RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL: {
			conn_db->disc_state = DISC_DONE;
			BT_LOGA("[APP] BLE WiFiMate client discover all success\r\n");
			ble_wifimate_client_set_cccd_indicate(conn_handle, BLE_WIFIMATE_CHAR_WIFI_SCAN_INFO_IDX, true);
			break;
		}
		default:
			break;
		}
	}

	general_client_discover_res_hdl(data);
}

static void ble_wifimate_client_write_res_hdl(void *data)
{
	rtk_bt_gattc_write_ind_t *write_res = (rtk_bt_gattc_write_ind_t *)data;
	rtk_bt_status_t write_status = write_res->status;
	uint16_t att_handle = write_res->handle;
	uint16_t conn_handle = write_res->conn_handle;
	uint8_t conn_id;
	struct ble_wifimate_client_db_t *conn_db;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	conn_db = ble_wifimate_database[conn_id];
	if (!conn_db) {
		return;
	}

	if (RTK_BT_STATUS_DONE == write_status) {
		for (uint8_t i = 0; i < BLE_WIFIMATE_CHAR_NUM; ++i) {
			if (att_handle == conn_db->char_db[i].char_val_handle) {
				if (conn_db->char_db[i].uuid == BLE_WIFIMATE_UUID_CHAR_KEY_NEGOTIATE) {
					BT_LOGA("[APP] BLE WiFiMate client write charac negotiate key success\r\n");
				} else if (conn_db->char_db[i].uuid == BLE_WIFIMATE_UUID_CHAR_WIFI_SCAN_ENABLE) {
					BT_LOGA("[APP] BLE WiFiMate client write charac wifi scan enable success\r\n");
				} else if (conn_db->char_db[i].uuid == BLE_WIFIMATE_UUID_CHAR_WIFI_CONNECT_ENABLE) {
					ble_wifimate_write_wlconn_enable_segment(att_handle, conn_handle);
					BT_LOGD("[APP] BLE WiFiMate client write charac wifi connect enable success\r\n");
				}
				break;
			}
		}
	} else if (RTK_BT_STATUS_FAIL == write_status) {
		BT_LOGE("[APP] BLE WiFiMate client write charac value_handle=0x%x fail, err_code %u\r\n",
				att_handle, write_res->err_code);
	}
}

rtk_bt_evt_cb_ret_t ble_wifimate_gattc_app_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;

	if (!data) {
		return RTK_BT_EVT_CB_FAIL;
	}

	switch (event) {
	case RTK_BT_GATTC_EVT_MTU_EXCHANGE:
		ble_wifimate_client_mtu_exchange_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND:
		ble_wifimate_client_discover_res_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_WRITE_RESULT_IND:
		ble_wifimate_client_write_res_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_INDICATE_IND:
		ble_wifimate_client_indicate_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_CCCD_ENABLE_IND:
		ble_wifimate_client_cccd_enable_hdl(data);
		break;
	default:
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

uint16_t ble_wifimate_client_wifi_scan(uint16_t conn_handle)
{
	uint8_t conn_id;
	uint8_t enable = true;
	struct ble_wifimate_client_db_t *conn_db;
	rtk_bt_gattc_write_param_t write_param = {0};

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	conn_db = ble_wifimate_database[conn_id];
	if (!conn_db) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	if (conn_db->disc_state != DISC_DONE) {
		BT_LOGE("[APP] BLE WifiMate client need discover service before write charac !!!\r\n");
		return RTK_BT_ERR_STATE_INVALID;
	}

	write_param.conn_handle = conn_handle;
	write_param.profile_id = BLE_WIFIMATE_CLIENT_PROFILE_ID;
	write_param.type = RTK_BT_GATT_CHAR_WRITE_REQ;
	write_param.handle = conn_db->char_db[BLE_WIFIMATE_CHAR_WIFI_SCAN_ENABLE_IDX].char_val_handle;
	write_param.length = BLE_WIFIMATE_CHAR_WIFI_SCAN_ENABLE_DATA_LEN;
	write_param.data = &enable;

	return rtk_bt_gattc_write(&write_param);
}

uint16_t ble_wifimate_client_attach_conn(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		BT_LOGE("[APP] BLE WifiMate client get conn id fail\r\n");
		return RTK_BT_FAIL;
	}

	ble_wifimate_database[conn_id] = (struct ble_wifimate_client_db_t *)
									 osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct ble_wifimate_client_db_t));
	if (!ble_wifimate_database[conn_id]) {
		BT_LOGE("[APP] BLE WifiMate client alloc database fail\r\n");
		return RTK_BT_ERR_NO_MEMORY;
	}
	memset(ble_wifimate_database[conn_id], 0, sizeof(struct ble_wifimate_client_db_t));

	return RTK_BT_OK;
}

uint16_t ble_wifimate_client_detach_conn(uint16_t conn_handle)
{
	uint8_t conn_id;

	ble_wifimate_char_multi_write_data_deinit();
	ble_wifimate_char_multi_recv_data_deinit();

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	return ble_wifimate_client_database_free(conn_id);
}

uint16_t ble_wifimate_client_add(void)
{
	return rtk_bt_gattc_register_profile(BLE_WIFIMATE_CLIENT_PROFILE_ID);
}

void ble_wifimate_client_deinit(void)
{
	ble_wifimate_char_multi_write_data_deinit();
	ble_wifimate_char_multi_recv_data_deinit();

	for (uint8_t idx = 0; idx < RTK_BLE_GAP_MAX_LINKS; ++idx) {
		ble_wifimate_client_database_free(idx);
	}
}