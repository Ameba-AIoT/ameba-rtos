/*
*******************************************************************************
* Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <basic_types.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_def.h>
#include <rtk_service_config.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_device.h>
#include <bt_utils.h>
#include <ble_wifimate_service.h>
#include <wifi_api.h>
#include <lwip_netconf.h>

#define BLE_WIFIMATE_DECODE_KEY_LEN                             (16)

#define RTK_BT_UUID_BLE_WIFIMATE_SRV                            BT_UUID_DECLARE_16(BLE_WIFIMATE_UUID_SRV)
#define RTK_BT_UUID_BLE_WIFIMATE_KEY_NEGOTIATE                  BT_UUID_DECLARE_16(BLE_WIFIMATE_UUID_CHAR_KEY_NEGOTIATE)
#define RTK_BT_UUID_BLE_WIFIMATE_WIFI_SCAN_ENABLE               BT_UUID_DECLARE_16(BLE_WIFIMATE_UUID_CHAR_WIFI_SCAN_ENABLE)
#define RTK_BT_UUID_BLE_WIFIMATE_WIFI_SCAN_INFO                 BT_UUID_DECLARE_16(BLE_WIFIMATE_UUID_CHAR_WIFI_SCAN_INFO)
#define RTK_BT_UUID_BLE_WIFIMATE_WIFI_CONNECT_ENABLE            BT_UUID_DECLARE_16(BLE_WIFIMATE_UUID_CHAR_WIFI_CONNECT_ENABLE)
#define RTK_BT_UUID_BLE_WIFIMATE_WIFI_CONNECT_STATE             BT_UUID_DECLARE_16(BLE_WIFIMATE_UUID_CHAR_WIFI_CONNECT_STATE)


#define BLE_WIFIMATE_KEY_NEGOTIATE_INDEX                        (2)
#define BLE_WIFIMATE_WIFI_SCAN_ENABLE_INDEX                     (4)
#define BLE_WIFIMATE_WIFI_SCAN_INFO_VAL_INDEX                   (6)
#define BLE_WIFIMATE_WIFI_SCAN_INFO_CCCD_INDEX                  (7)
#define BLE_WIFIMATE_WIFI_CONNECT_ENABLE_INDEX                  (9)
#define BLE_WIFIMATE_WIFI_CONNECT_STATE_VAL_INDEX               (11)
#define BLE_WIFIMATE_WIFI_CONNECT_STATE_CCCD_INDEX              (12)

#define BLE_WIFIMATE_MAX_WIFI_SCAN_AP_NUM                       (64)

#define CHAR_WIFI_SCAN_INFO_INITIAL_LEN                 (7)
#define CHAR_WIFI_SCAN_INFO_INITIAL_FIRST_ELE_LEN       (2)
#define CHAR_WIFI_SCAN_INFO_INITIAL_SECOND_ELE_LEN      (1)
#define CHAR_WIFI_SCAN_INFO_INITIAL_THIRD_ELE_LEN       (4)

#define CHAR_WIFI_SCAN_INFO_SEG_FIRST_ELE_LEN           (1)
#define CHAR_WIFI_SCAN_INFO_SEG_THIRD_ELE_LEN           (2)
#define CHAR_WIFI_SCAN_INFO_SEG_FORTH_ELE_LEN           (1)
#define CHAR_WIFI_SCAN_INFO_SEG_FIFTH_ELE_LEN           (1)

enum ble_wifimate_server_state_e {
	BLE_WIFIMATE_SERVER_STATE_IDLE = 0,
	BLE_WIFIMATE_SERVER_STATE_CONNECT = 1,
	BLE_WIFIMATE_SERVER_STATE_WIFI_SCAN = 2,
	BLE_WIFIMATE_SERVER_STATE_WIFI_CONN = 3,
};

struct password_decode_t {
	uint8_t         algorithm;
	uint8_t         key[BLE_WIFIMATE_DECODE_KEY_LEN];
};

struct ble_wifimate_wifi_scan_result_t {
	uint8_t                 ap_num;
	struct rtw_scan_result  ap_info[BLE_WIFIMATE_MAX_WIFI_SCAN_AP_NUM];
};

static rtk_bt_gatt_attr_t ble_wifimate_attrs[] = {
	/* Primary Service: BLE WIFIMATE */
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_BLE_WIFIMATE_SRV),

	/* Characteristic: WI-FI Key Negotiate */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_BLE_WIFIMATE_KEY_NEGOTIATE,
							   RTK_BT_GATT_CHRC_WRITE,
							   RTK_BT_GATT_PERM_WRITE),

	/* Characteristic: WI-FI Scan Enable */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_BLE_WIFIMATE_WIFI_SCAN_ENABLE,
							   RTK_BT_GATT_CHRC_WRITE,
							   RTK_BT_GATT_PERM_WRITE),

	/* Characteristic: WI-FI Scan Info */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_BLE_WIFIMATE_WIFI_SCAN_INFO,
							   RTK_BT_GATT_CHRC_INDICATE,
							   RTK_BT_GATT_PERM_NONE),
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),

	/* Characteristic: WI-FI Connect Enable */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_BLE_WIFIMATE_WIFI_CONNECT_ENABLE,
							   RTK_BT_GATT_CHRC_WRITE,
							   RTK_BT_GATT_PERM_WRITE),

	/* Characteristic: WI-FI Connect State */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_BLE_WIFIMATE_WIFI_CONNECT_STATE,
							   RTK_BT_GATT_CHRC_INDICATE,
							   RTK_BT_GATT_PERM_NONE),
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
};

static struct rtk_bt_gatt_service s_ble_wifimate_srv = RTK_BT_GATT_SERVICE(ble_wifimate_attrs, BLE_WIFIMATE_SRV_ID);

static uint16_t s_mtu_size = 23;
static uint8_t s_cccd_ind_en_wifi_scan = 0;
static uint8_t s_cccd_ind_en_wifi_conn = 0;
static uint8_t s_ble_wifimate_state = 0;
static struct password_decode_t s_pw_decode = {0};

static struct ble_wifimate_char_send_data_t s_multi_indicate = {0};
static struct ble_wifimate_char_recv_data_t s_multi_recv = {0};

static uint32_t ble_wifimate_server_checksum_cal(uint16_t len, uint8_t *data)
{
	uint32_t checksum = 0;
	for (uint16_t i = 0; i < len; ++i) {
		checksum += *(data + i);
	}

	return checksum;
}

static uint16_t ble_wifimate_char_multi_indicate_data_init(struct ble_wifimate_wifi_scan_result_t *wifi_scan_result)
{
	uint32_t offset = 0;
	uint32_t total_len = 0;
	struct rtw_scan_result *ap_info = NULL;

	if (!wifi_scan_result) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (s_multi_indicate.data) {
		osif_mem_free((void *)s_multi_indicate.data);
	}

	for (uint8_t i = 0; i < wifi_scan_result->ap_num; ++i) {
		total_len += CHAR_WIFI_SCAN_INFO_SEG_FIRST_ELE_LEN + wifi_scan_result->ap_info[i].SSID.len + \
					 CHAR_WIFI_SCAN_INFO_SEG_THIRD_ELE_LEN + CHAR_WIFI_SCAN_INFO_SEG_FORTH_ELE_LEN + \
					 CHAR_WIFI_SCAN_INFO_SEG_FIFTH_ELE_LEN;
	}

	memset(&s_multi_indicate, 0, sizeof(s_multi_indicate));
	s_multi_indicate.flag = true;
	s_multi_indicate.total_len = total_len;
	s_multi_indicate.seg_num = (uint8_t)ceil((double)total_len / (s_mtu_size - 3 - 1));
	s_multi_indicate.data = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, total_len);
	if (!s_multi_indicate.data) {
		BT_LOGE("[APP] BLE WifiMate server indicate failed: can't alloc memory\r\n");
		memset(&s_multi_indicate, 0, sizeof(s_multi_indicate));
		return RTK_BT_ERR_NO_MEMORY;
	}
	memset(s_multi_indicate.data, 0, total_len);

	for (uint8_t i = 0; i < wifi_scan_result->ap_num; ++i) {
		ap_info = &wifi_scan_result->ap_info[i];

		memcpy(s_multi_indicate.data + offset, &ap_info->SSID.len, CHAR_WIFI_SCAN_INFO_SEG_FIRST_ELE_LEN);
		offset += CHAR_WIFI_SCAN_INFO_SEG_FIRST_ELE_LEN;

		memcpy(s_multi_indicate.data + offset, ap_info->SSID.val, ap_info->SSID.len);
		offset += ap_info->SSID.len;

		memcpy(s_multi_indicate.data + offset, &ap_info->signal_strength, CHAR_WIFI_SCAN_INFO_SEG_THIRD_ELE_LEN);
		offset += CHAR_WIFI_SCAN_INFO_SEG_THIRD_ELE_LEN;

		memcpy(s_multi_indicate.data + offset, &ap_info->security, CHAR_WIFI_SCAN_INFO_SEG_FORTH_ELE_LEN);
		offset += CHAR_WIFI_SCAN_INFO_SEG_FORTH_ELE_LEN;

		memcpy(s_multi_indicate.data + offset, &ap_info->channel, CHAR_WIFI_SCAN_INFO_SEG_FIFTH_ELE_LEN);
		offset += CHAR_WIFI_SCAN_INFO_SEG_FIFTH_ELE_LEN;
	}

	s_multi_indicate.checksum = ble_wifimate_server_checksum_cal(s_multi_indicate.total_len, s_multi_indicate.data);
	BT_LOGD("%s total_len=%u, seg_num=%u, checksum=%u\r\n", __func__,
			s_multi_indicate.total_len, s_multi_indicate.seg_num, s_multi_indicate.checksum);

	return RTK_BT_OK;
}

static void ble_wifimate_char_multi_indicate_data_deinit(void)
{
	if (s_multi_indicate.data) {
		osif_mem_free((void *)s_multi_indicate.data);
	}
	memset(&s_multi_indicate, 0, sizeof(s_multi_indicate));
}

static uint16_t ble_wifimate_char_multi_recv_data_init(uint16_t total_len, uint8_t seg_num, uint32_t checksum)
{
	if (s_multi_recv.data) {
		osif_mem_free((void *)s_multi_recv.data);
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
		BT_LOGE("[BLE WifiMate client] init multi indication receive data: can't alloc memory\r\n");
		memset(&s_multi_recv, 0, sizeof(s_multi_recv));
		return RTK_BT_ERR_NO_MEMORY;
	}

	return RTK_BT_OK;
}

static void ble_wifimate_char_multi_recv_data_deinit(void)
{
	if (s_multi_recv.data) {
		osif_mem_free((void *)s_multi_recv.data);
	}
	memset(&s_multi_recv, 0, sizeof(s_multi_recv));
}

static void ble_wifimate_server_mtu_exchange_hdl(void *data)
{
	rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)data;

	if (!p_gatt_mtu_ind) {
		return;
	}

	if (p_gatt_mtu_ind->result == RTK_BT_OK) {
		BT_LOGA("[APP] GATTS mtu exchange successfully, mtu_size: %d, conn_handle: %d \r\n",
				p_gatt_mtu_ind->mtu_size, p_gatt_mtu_ind->conn_handle);
		s_mtu_size = p_gatt_mtu_ind->mtu_size;
		BT_LOGD("%s: s_mtu_size=%d\r\n", __func__, s_mtu_size);
	} else {
		BT_LOGE("[APP] GATTS mtu exchange fail \r\n");
	}
}

static void ble_wifimate_server_register_service_hdl(void *data)
{
	rtk_bt_gatts_reg_ind_t *p_gatts_reg_ind = (rtk_bt_gatts_reg_ind_t *)data;

	if (!p_gatts_reg_ind || p_gatts_reg_ind->app_id != BLE_WIFIMATE_SRV_ID) {
		return;
	}

	if (p_gatts_reg_ind->reg_status == RTK_BT_OK) {
		BT_LOGA("[APP] BLE WiFiMate service register succeed!\r\n");
	} else
		BT_LOGE("[APP] BLE WiFiMate service register failed, err: 0x%x\r\n",
				p_gatts_reg_ind->reg_status);
}

static uint16_t ble_wifimate_server_key_negotiate_hdl(uint16_t len, uint8_t *data)
{
	uint16_t ret;
	rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};

	if (!data) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (len != BLE_WIFIMATE_CHAR_NOGOTIATE_KEY_LEN) {
		BT_LOGE("[APP] BLE WiFiMate server write nogotiate key length unmatch:%d\r\n", len);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	memset(&s_pw_decode, 0, sizeof(s_pw_decode));
	memcpy(&s_pw_decode.algorithm, data, CHAR_NEGOTIATE_KEY_FIRST_ELE_LEN);
	memcpy(&s_pw_decode.key, data + CHAR_NEGOTIATE_KEY_FIRST_ELE_LEN, CHAR_NEGOTIATE_KEY_SECOND_ELE_LEN);

	if (s_pw_decode.algorithm != BLE_WIFIMATE_KEY_ENCODE_ALGORITHM_NONE) {
		BT_LOGE("[APP] BLE WiFiMate server does not support key encryption and decryption algorithms temporarily\r\n");
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (s_pw_decode.algorithm != BLE_WIFIMATE_KEY_ENCODE_ALGORITHM_NONE) {
		if ((ret = rtk_bt_le_gap_get_bd_addr(&bd_addr)) != RTK_BT_OK) {
			return ret;
		}

		s_pw_decode.key[BLE_WIFIMATE_DECODE_KEY_LEN - 3] = bd_addr.addr_val[2];
		s_pw_decode.key[BLE_WIFIMATE_DECODE_KEY_LEN - 2] = bd_addr.addr_val[1];
		s_pw_decode.key[BLE_WIFIMATE_DECODE_KEY_LEN - 1] = bd_addr.addr_val[0];
	}

	BT_LOGA("[APP] BLE WiFiMate server negotiate key procedure: algorithm=%d, \
			key=%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\r\n",
			s_pw_decode.algorithm,
			s_pw_decode.key[0], s_pw_decode.key[1], s_pw_decode.key[2], s_pw_decode.key[3],
			s_pw_decode.key[4], s_pw_decode.key[5], s_pw_decode.key[6], s_pw_decode.key[7],
			s_pw_decode.key[8], s_pw_decode.key[9], s_pw_decode.key[10], s_pw_decode.key[11],
			s_pw_decode.key[12], s_pw_decode.key[13], s_pw_decode.key[14], s_pw_decode.key[15]);

	return RTK_BT_OK;
}

static uint16_t ble_wifimate_server_send_wifi_scan_info_initial(uint16_t conn_handle)
{
	uint16_t offset = 0;
	uint16_t ret = RTK_BT_OK;
	uint8_t ind_data[CHAR_WIFI_SCAN_INFO_INITIAL_LEN] = {0};
	rtk_bt_gatts_ntf_and_ind_param_t ind_param = {0};

	if (s_cccd_ind_en_wifi_scan) {
		uint16_t len = CHAR_WIFI_SCAN_INFO_INITIAL_FIRST_ELE_LEN + \
					   CHAR_WIFI_SCAN_INFO_INITIAL_SECOND_ELE_LEN + \
					   CHAR_WIFI_SCAN_INFO_INITIAL_THIRD_ELE_LEN;

		memcpy(ind_data, &s_multi_indicate.total_len, CHAR_WIFI_CONN_ENABLE_INITIAL_FIRST_ELE_LEN);
		offset = CHAR_WIFI_CONN_ENABLE_INITIAL_FIRST_ELE_LEN;

		memcpy(ind_data + offset, &s_multi_indicate.seg_num, CHAR_WIFI_CONN_ENABLE_INITIAL_SECOND_ELE_LEN);
		offset += CHAR_WIFI_CONN_ENABLE_INITIAL_SECOND_ELE_LEN;

		memcpy(ind_data + offset, &s_multi_indicate.checksum, CHAR_WIFI_CONN_ENABLE_INITIAL_THIRD_ELE_LEN);

		ind_param.app_id = BLE_WIFIMATE_SRV_ID;
		ind_param.conn_handle = conn_handle;
		ind_param.index = BLE_WIFIMATE_WIFI_SCAN_INFO_VAL_INDEX;
		ind_param.seq = 0;
		ind_param.len = len;
		ind_param.data = ind_data;

		BT_LOGD("%s: total_len=%u, seg_num=%u, checksum=%u\r\n",
				__func__, s_multi_indicate.total_len, s_multi_indicate.seg_num, s_multi_indicate.checksum);

		ret = rtk_bt_gatts_indicate(&ind_param);
		if (ret != RTK_BT_OK) {
			ble_wifimate_char_multi_indicate_data_deinit();
			BT_LOGE("%s: indicate error 0x%02x\r\n", __func__, ret);
		}

	}

	return ret;
}

static uint16_t ble_wifimate_server_send_wifi_scan_info_segment(uint16_t conn_handle)
{
	int     remain = s_multi_indicate.total_len - s_multi_indicate.offset;
	uint16_t ret;
	uint8_t *ind_data = NULL;
	rtk_bt_gatts_ntf_and_ind_param_t ind_param = {0};

	if (!s_multi_indicate.flag) {
		BT_LOGA("[APP] BLE WifiMate server no need indicate wifi scan info charac.\r\n");
		return RTK_BT_OK;
	}

	if ((s_multi_indicate.seg_idx >= s_multi_indicate.seg_num) ||
		(remain <= 0)) {
		BT_LOGA("[APP] BLE WifiMate server no need indicate wifi scan info charac done.\r\n");
		ble_wifimate_char_multi_indicate_data_deinit();
		return RTK_BT_OK;
	}

	if (s_cccd_ind_en_wifi_scan) {
		uint16_t len = (remain < (s_mtu_size - 3 - 1)) ? (remain + 1) : (s_mtu_size - 3);

		ind_data = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, len);
		if (!ind_data) {
			BT_LOGE("[APP] BLE WifiMate server indicate failed: can't alloc memory\r\n");
			ble_wifimate_char_multi_indicate_data_deinit();
			return RTK_BT_ERR_NO_MEMORY;
		}
		/* set seg_idx */
		memcpy(ind_data, &s_multi_indicate.seg_idx, 1);
		/* set data segment */
		memcpy(ind_data + 1, s_multi_indicate.data + s_multi_indicate.offset, len - 1);
		s_multi_indicate.offset += len - 1;
		s_multi_indicate.seg_idx++;

		ind_param.app_id = BLE_WIFIMATE_SRV_ID;
		ind_param.conn_handle = conn_handle;
		ind_param.index = BLE_WIFIMATE_WIFI_SCAN_INFO_VAL_INDEX;
		ind_param.seq = 0;

		ind_param.len = len;
		ind_param.data = ind_data;

		BT_LOGD("%s: indicate seg_idx=%d offest=%u total_len=%u len=%d\r\n", __func__,
				s_multi_indicate.seg_idx, s_multi_indicate.offset, s_multi_indicate.total_len, len);
		BT_DUMPD("[APP] GATTS indicate event:\r\n", ind_data, ind_param.len);

		ret = rtk_bt_gatts_indicate(&ind_param);
		if (ret != RTK_BT_OK) {
			ble_wifimate_char_multi_indicate_data_deinit();
			BT_LOGE("%s: indicate error 0x%02x\r\n", __func__, ret);
		}

		osif_mem_free(ind_data);
	}

	return ret;
}

static uint8_t ble_wifimate_convert_wifi_conn_security(uint8_t security)
{
	uint32_t dest =  BWM_SECURITY_UNKNOWN;

	dest = (security == BWM_SECURITY_OPEN) ? RTW_SECURITY_OPEN :
		   (security == BWM_SECURITY_WEP_PSK) ? RTW_SECURITY_WEP_PSK :
		   (security == BWM_SECURITY_WPA_TKIP_PSK) ? RTW_SECURITY_WPA_TKIP_PSK :
		   (security == BWM_SECURITY_WPA_AES_PSK) ? RTW_SECURITY_WPA_AES_PSK :
		   (security == BWM_SECURITY_WPA_MIXED_PSK) ? RTW_SECURITY_WPA_MIXED_PSK :
		   (security == BWM_SECURITY_WPA2_AES_PSK) ?  RTW_SECURITY_WPA2_AES_PSK :
		   (security == BWM_SECURITY_WPA2_TKIP_PSK) ? RTW_SECURITY_WPA2_TKIP_PSK :
		   (security == BWM_SECURITY_WPA2_MIXED_PSK) ? RTW_SECURITY_WPA2_MIXED_PSK :
		   (security == BWM_SECURITY_WPA_WPA2_TKIP_PSK) ? RTW_SECURITY_WPA_WPA2_TKIP_PSK :
		   (security == BWM_SECURITY_WPA_WPA2_AES_PSK) ? RTW_SECURITY_WPA_WPA2_AES_PSK :
		   (security == BWM_SECURITY_WPA_WPA2_MIXED_PSK) ? RTW_SECURITY_WPA_WPA2_MIXED_PSK :
#ifdef CONFIG_SAE_SUPPORT
		   (security == BWM_SECURITY_WPA3_AES_PSK) ? RTW_SECURITY_WPA3_AES_PSK :
		   (security == BWM_SECURITY_WPA2_WPA3_MIXED) ?  RTW_SECURITY_WPA2_WPA3_MIXED :
#endif
#ifdef CONFIG_OWE_SUPPORT
		   (security == BWM_SECURITY_WPA3_OWE) ? RTW_SECURITY_WPA3_OWE :
#endif
		   BWM_SECURITY_WEP_PSK;

	return dest;
}

static uint8_t ble_wifimate_convert_wifi_scan_security(uint32_t security)
{
	uint8_t dest =  BWM_SECURITY_UNKNOWN;

	dest = (security == RTW_SECURITY_OPEN) ? BWM_SECURITY_OPEN :
		   (security == RTW_SECURITY_WEP_PSK) ? BWM_SECURITY_WEP_PSK :
		   (security == RTW_SECURITY_WPA_TKIP_PSK) ? BWM_SECURITY_WPA_TKIP_PSK :
		   (security == RTW_SECURITY_WPA_AES_PSK) ? BWM_SECURITY_WPA_AES_PSK :
		   (security == RTW_SECURITY_WPA_MIXED_PSK) ? BWM_SECURITY_WPA_MIXED_PSK :
		   (security == RTW_SECURITY_WPA2_AES_PSK) ? BWM_SECURITY_WPA2_AES_PSK :
		   (security == RTW_SECURITY_WPA2_TKIP_PSK) ? BWM_SECURITY_WPA2_TKIP_PSK :
		   (security == RTW_SECURITY_WPA2_MIXED_PSK) ? BWM_SECURITY_WPA2_MIXED_PSK :
		   (security == RTW_SECURITY_WPA_WPA2_TKIP_PSK) ? BWM_SECURITY_WPA_WPA2_TKIP_PSK :
		   (security == RTW_SECURITY_WPA_WPA2_AES_PSK) ? BWM_SECURITY_WPA_WPA2_AES_PSK :
		   (security == RTW_SECURITY_WPA_WPA2_MIXED_PSK) ? BWM_SECURITY_WPA_WPA2_MIXED_PSK :
		   (security == (RTW_SECURITY_WPA_TKIP_PSK | ENTERPRISE_ENABLED)) ? BWM_SECURITY_WPA_TKIP_PSK :
		   (security == (RTW_SECURITY_WPA_AES_PSK | ENTERPRISE_ENABLED)) ? BWM_SECURITY_WPA_AES_PSK :
		   (security == (RTW_SECURITY_WPA_MIXED_PSK | ENTERPRISE_ENABLED)) ? BWM_SECURITY_WPA_MIXED_PSK :
		   (security == (RTW_SECURITY_WPA2_TKIP_PSK | ENTERPRISE_ENABLED)) ? BWM_SECURITY_WPA2_TKIP_PSK :
		   (security == (RTW_SECURITY_WPA2_AES_PSK | ENTERPRISE_ENABLED)) ? BWM_SECURITY_WPA2_AES_PSK :
		   (security == (RTW_SECURITY_WPA2_MIXED_PSK | ENTERPRISE_ENABLED)) ? BWM_SECURITY_WPA2_MIXED_PSK :
		   (security == (RTW_SECURITY_WPA_WPA2_TKIP_PSK | ENTERPRISE_ENABLED)) ? BWM_SECURITY_WPA_WPA2_TKIP_PSK :
		   (security == (RTW_SECURITY_WPA_WPA2_AES_PSK | ENTERPRISE_ENABLED)) ? BWM_SECURITY_WPA_WPA2_AES_PSK :
		   (security == (RTW_SECURITY_WPA_WPA2_MIXED_PSK | ENTERPRISE_ENABLED)) ? BWM_SECURITY_WPA_WPA2_MIXED_PSK :
#ifdef CONFIG_SAE_SUPPORT
		   (security == RTW_SECURITY_WPA3_AES_PSK) ? BWM_SECURITY_WPA3_AES_PSK :
		   (security == RTW_SECURITY_WPA2_WPA3_MIXED) ? BWM_SECURITY_WPA2_WPA3_MIXED :
		   (security == (WPA3_SECURITY | ENTERPRISE_ENABLED)) ? BWM_SECURITY_WPA3_AES_PSK :
#endif
#ifdef CONFIG_OWE_SUPPORT
		   (security == RTW_SECURITY_WPA3_OWE) ? BWM_SECURITY_WPA3_OWE :
#endif
		   BWM_SECURITY_UNKNOWN;

	return dest;
}

static uint16_t ble_wifimate_server_write_wifi_scan_hdl(uint16_t conn_handle, uint16_t len, uint8_t *data)
{
	uint16_t ret = RTK_BT_OK;
	uint8_t enable = 0;
	struct rtw_scan_result *scanned_AP_list = NULL;
	int join_status = RTW_JOINSTATUS_UNKNOWN;
	int scan_network = 0;
	unsigned int scanned_AP_num = 0;
	struct _rtw_scan_param_t scan_param;
	struct ble_wifimate_wifi_scan_result_t *wifi_scan_result = NULL;

	if (len != 1 || !data) {
		BT_LOGE("[APP] BLE WifiMate server wifi scan length unmatch\r\n");
		return RTK_BT_ERR_PARAM_INVALID;
	}

	enable = *data;
	if (!enable) {
		BT_LOGE("[APP] BLE WifiMate server wifi scan disable\r\n");
		return RTK_BT_OK;
	}

	wifi_scan_result = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct ble_wifimate_wifi_scan_result_t));
	if (!wifi_scan_result) {
		BT_LOGE("[APP] BLE WifiMate server can't alloc memory for wifi scan result\r\n");
		return RTK_BT_ERR_NO_MEMORY;
	}

	memset(wifi_scan_result, 0, sizeof(struct ble_wifimate_wifi_scan_result_t));
	memset(&scan_param, 0, sizeof(struct _rtw_scan_param_t));

	wifi_get_join_status(&join_status);
	BT_LOGD("%s join_status=%d\r\n", __func__, join_status);
	if ((join_status > RTW_JOINSTATUS_UNKNOWN) && (join_status < RTW_JOINSTATUS_SUCCESS)) {
		BT_LOGE("[APP] WiFi Connecting now, forbid scanning, exit\r\n");
		ret = RTK_BT_FAIL;
		goto end;
	}

	scan_network = wifi_scan_networks(&scan_param, 1);
	BT_LOGD("%s scan_network=%d\r\n", __func__, scan_network);
	if (scan_network < RTK_SUCCESS) {
		BT_LOGE("[APP] wifi_scan_networks ERROR\r\n");
		ret = RTK_BT_FAIL;
		goto end;
	}

	if (scan_network > 0) {
		scanned_AP_num = (scan_network <= BLE_WIFIMATE_MAX_WIFI_SCAN_AP_NUM) ? \
						 scan_network : BLE_WIFIMATE_MAX_WIFI_SCAN_AP_NUM;

		scanned_AP_list = (struct rtw_scan_result *)osif_mem_alloc(RAM_TYPE_DATA_ON, scanned_AP_num * sizeof(struct rtw_scan_result));
		if (scanned_AP_list == NULL) {
			BT_LOGE(NOTAG, "[+WLSCAN]ERROR: Can not malloc memory for scan result\r\n");
			ret = RTK_BT_ERR_NO_MEMORY;
			goto end;
		}
		memset(scanned_AP_list, 0, sizeof(scanned_AP_num * sizeof(struct rtw_scan_result)));

		if (wifi_get_scan_records(&scanned_AP_num, scanned_AP_list) < 0) {
			BT_LOGE(NOTAG, "[+WLSCAN] Get result failed\r\n");
			ret = RTK_BT_FAIL;
			goto end;
		}

		for (uint32_t i = 0; (i < scanned_AP_num) && (i < BLE_WIFIMATE_MAX_WIFI_SCAN_AP_NUM); i++) {
			struct rtw_scan_result *scanned_ap_info = &scanned_AP_list[i];
			/* Ensure the SSID is null terminated */
			scanned_ap_info->SSID.val[scanned_ap_info->SSID.len] = 0;
			/* security */
			scanned_ap_info->security = ble_wifimate_convert_wifi_scan_security(scanned_ap_info->security);
			memcpy(&(wifi_scan_result->ap_info[wifi_scan_result->ap_num]), scanned_ap_info, sizeof(struct rtw_scan_result));
			BT_LOGD("Wifi scan result: ap_num=%d SSID_LEN=%d ssid=%s, rssi=%d, security=%u, channel=%d\r\n",
					wifi_scan_result->ap_num,
					wifi_scan_result->ap_info[wifi_scan_result->ap_num].SSID.len,
					wifi_scan_result->ap_info[wifi_scan_result->ap_num].SSID.val,
					wifi_scan_result->ap_info[wifi_scan_result->ap_num].signal_strength,
					wifi_scan_result->ap_info[wifi_scan_result->ap_num].security,
					wifi_scan_result->ap_info[wifi_scan_result->ap_num].channel);

			wifi_scan_result->ap_num++;
		}
		BT_LOGD("Wifi scan result: ap_nums=%d\r\n", scanned_AP_num);
	}
end:
	if (scanned_AP_list) {
		osif_mem_free(scanned_AP_list);
	}

	if (ret == RTK_BT_OK) {
		ret = ble_wifimate_char_multi_indicate_data_init(wifi_scan_result);
		if (wifi_scan_result) {
			osif_mem_free(wifi_scan_result);
		}
		if (ret != RTK_BT_OK) {
			ble_wifimate_char_multi_indicate_data_deinit();
			return ret;
		}
		ret = ble_wifimate_server_send_wifi_scan_info_initial(conn_handle);
	}
	return ret;
}

static uint8_t ble_wifimate_wifi_conn_result_to_bwm_errcode(int wifi_res)
{
	uint8_t err_code = 0;

	switch (wifi_res) {
	case RTK_SUCCESS:
		err_code = BWM_OK;
		break;
	case -RTK_ERR_BADARG:
		err_code = BWM_ERR_PARAM_INVALID;
		break;
	case -RTK_ERR_BUSY:
		err_code = BWM_ERR_BUSY;
		break;
	case -RTK_ERR_NOMEM:
		err_code = BWM_ERR_NOMEM;
		break;
	case -RTK_ERR_TIMEOUT:
		err_code = BWM_ERR_TIMEOUT;
		break;
	case -RTK_ERR_WIFI_CONN_INVALID_KEY:
		err_code = BWM_ERR_PASSWORD_WRONG;
		break;
	case -RTK_ERR_WIFI_CONN_SCAN_FAIL:
		err_code = BWM_ERR_AP_NOT_FOUND;
		break;
	case -RTK_ERR_WIFI_CONN_AUTH_FAIL:
		err_code = BWM_ERR_AUTH_FAIL;
		break;
	case -RTK_ERR_WIFI_CONN_AUTH_PASSWORD_WRONG:
		err_code = BWM_ERR_PASSWORD_WRONG;
		break;
	case -RTK_ERR_WIFI_CONN_ASSOC_FAIL:
		err_code = BWM_ERR_ASSOC_FAIL;
		break;
	case -RTK_ERR_WIFI_CONN_4WAY_HANDSHAKE_FAIL:
		err_code = BWM_ERR_PASSWORD_WRONG;
		break;
	case -RTK_ERR_WIFI_CONN_4WAY_PASSWORD_WRONG:
		err_code = BWM_ERR_PASSWORD_WRONG;
		break;
	default:
		err_code = BWM_ERR_ERROR;
		break;
	}

	return err_code;
}

static uint16_t ble_wifimate_server_indicate_wifi_conn_state(uint16_t conn_handle, uint8_t state, uint8_t err_code)
{
	uint16_t ret = RTK_BT_OK;
	uint8_t ind_data[BLE_WIFIMATE_CHAR_WIFI_CONNECT_STATE_LEN] = {0};
	rtk_bt_gatts_ntf_and_ind_param_t ind_param = {0};

	if (s_cccd_ind_en_wifi_conn) {
		ind_data[0] = state;
		ind_data[1] = err_code;

		ind_param.app_id = BLE_WIFIMATE_SRV_ID;
		ind_param.conn_handle = conn_handle;
		ind_param.index = BLE_WIFIMATE_WIFI_CONNECT_STATE_VAL_INDEX;
		ind_param.seq = 0;
		ind_param.len = BLE_WIFIMATE_CHAR_WIFI_CONNECT_STATE_LEN;
		ind_param.data = ind_data;

		ret = rtk_bt_gatts_indicate(&ind_param);
		if (ret != RTK_BT_OK) {
			BT_LOGE("%s: indicate error 0x%02x\r\n", __func__, ret);
		}
		BT_LOGD("%s: state=%d err_code=%d ind_param.len=%d\r\n", __func__, state, err_code, ind_param.len);
		BT_DUMPD("[APP] GATTS indicate wifi conn state:\r\n", (uint8_t *)ind_param.data, ind_param.len);
	}

	BT_AT_PRINT("+BTDEMO:ble_wifimate_device,%d,%d,%d\r\n", conn_handle, state, err_code);
	return ret;
}

static uint16_t ble_wifimate_wifi_connect(uint16_t conn_handle, struct wifi_conn_config_t *conn_config)
{
	int ret = 0;
	struct _rtw_network_info_t wifi;
	struct _rtw_wifi_setting_t setting = {0};

	if (!conn_config) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	memset(&wifi, 0, sizeof(wifi));
	wifi.ssid.len = conn_config->ssid_len;
	memcpy(wifi.ssid.val, conn_config->ssid, wifi.ssid.len);
	wifi.password_len = conn_config->password_len;
	wifi.password = conn_config->password;
	wifi.security_type = ble_wifimate_convert_wifi_conn_security(conn_config->security);
	wifi.key_id = 0; // WEP key ID missed in BT Config, default WEP key ID 0

	BT_LOGD("%s:Wifi Connect ssid_len=%d ssid=%s wifi.password_len=%d wifi.password=%s\r\n",
			__func__, wifi.ssid.len, wifi.ssid.val, wifi.password_len, wifi.password);

	if (wifi.ssid.len == 0) {
		BT_LOGE("[APP] BLE WiFiMate server error: SSID can't be empty\r\n");
		return RTK_BT_ERR_PARAM_INVALID;
	}

	//Check if in AP mode
	wifi_get_setting(WLAN0_IDX, &setting);

	ret = wifi_connect(&wifi, 1);
	if (ret != RTK_SUCCESS) {
		uint8_t err_code = ble_wifimate_wifi_conn_result_to_bwm_errcode(ret);
		BT_LOGA("[APP] BLE WiFiMate server can't connect to AP, ret=%d err_code=%d\r\n", ret, err_code);
		return ble_wifimate_server_indicate_wifi_conn_state(conn_handle, BWM_WIFI_STATE_IDLE, err_code);
	}

	BT_LOGA("[APP] BLE WiFiMate server wifi connected.\r\n");

#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	uint8_t DCHP_state;
	/* Start DHCPClient */
	DCHP_state = LwIP_DHCP(0, DHCP_START);

	if (DCHP_state != DHCP_ADDRESS_ASSIGNED) {
		BT_LOGA("[APP] BLE WiFiMate server DHCP fail, DHCP_state=%d\r\n", DCHP_state);
		return ble_wifimate_server_indicate_wifi_conn_state(conn_handle, BWM_WIFI_STATE_IDLE, BWM_ERR_DHCP_ADDRESS_ASSIGN_FAIL);
	}
	BT_LOGA("[APP] BLE WiFiMate server wifi got IP.\r\n");
#endif
	return ble_wifimate_server_indicate_wifi_conn_state(conn_handle, BWM_WIFI_STATE_CONNECTED, BWM_OK);
}

static void ble_wifimate_password_decode(uint8_t *pw, uint8_t *decode_pw)
{
	//TODO
	(void)pw;
	(void)decode_pw;
	return;
}

static uint16_t ble_wifimate_parse_wifi_connect_config(
	uint16_t len, uint8_t *data, struct wifi_conn_config_t *conn_config)
{
	uint16_t offset = 0;

	if (!conn_config || !data) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	memset(conn_config, 0, sizeof(conn_config));

	if (len > (offset + CHAR_WIFI_CONN_ENABLE_SEG_FIRST_ELE_LEN)) {
		conn_config->ssid_len = *data;
		offset += CHAR_WIFI_CONN_ENABLE_SEG_FIRST_ELE_LEN;
	}

	if (len > (offset + conn_config->ssid_len)) {
		memcpy(conn_config->ssid, data + offset, conn_config->ssid_len);
		offset += conn_config->ssid_len;
	}

	if (len > (offset + CHAR_WIFI_CONN_ENABLE_SEG_THIRD_ELE_LEN)) {
		conn_config->security = *(data + offset);
		offset += CHAR_WIFI_CONN_ENABLE_SEG_THIRD_ELE_LEN;
	}

	if (len > (offset + CHAR_WIFI_CONN_ENABLE_SEG_FORTH_ELE_LEN)) {
		conn_config->password_len = *(data + offset);
		offset += CHAR_WIFI_CONN_ENABLE_SEG_FORTH_ELE_LEN;
	}

	if (len == (offset + conn_config->password_len)) {
		conn_config->password = data + offset;
		if (s_pw_decode.algorithm) {
			ble_wifimate_password_decode(data + offset, conn_config->password);
		}
	}

	return RTK_BT_OK;
}

static uint16_t ble_wifimate_server_write_wifi_connect_hdl(uint16_t conn_handle, uint16_t len, uint8_t *data)
{
	uint16_t ret = 0;

	if (!data) {
		BT_LOGA("[APP] BLE WiFiMate indicate wifi_scan_info value NULL %d!\r\n");
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (!s_multi_recv.flag) {
		uint16_t total_len;
		uint8_t seg_num;
		uint32_t checksum;

		if (len != 7) {
			BT_LOGA("[APP] BLE WiFiMate indicate wifi_scan_info length invalid %d!\r\n", len);
			return RTK_BT_ERR_PARAM_INVALID;
		}

		memcpy(&total_len, data, sizeof(uint16_t));
		memcpy(&seg_num,   data + sizeof(uint16_t), sizeof(uint8_t));
		memcpy(&checksum,  data + sizeof(uint16_t) + sizeof(uint8_t), sizeof(uint32_t));

		BT_LOGD("%s:total_len=%d, seg_num=%d, checksum=%d\r\n",
				__func__, total_len, seg_num, checksum);
		if (seg_num > 0) {
			ble_wifimate_char_multi_recv_data_init(total_len, seg_num, checksum);
		} else {
			BT_LOGA("[APP] BLE WiFiMate conn_handle=%d wifi connect enable invalid!\r\n", conn_handle);
			return RTK_BT_FAIL;
		}
	} else {
		uint32_t checksum;
		memcpy(&s_multi_recv.seg_idx, data, sizeof(uint8_t));
		memcpy(s_multi_recv.data + s_multi_recv.offset, data + sizeof(uint8_t), len - 1);
		s_multi_recv.offset += len - 1;

		BT_LOGD("%s: s_multi_recv.total_len=%u s_multi_recv.offset=%u seg_idx=%d\r\n", __func__,
				s_multi_recv.total_len, s_multi_recv.offset, s_multi_recv.seg_idx);

		if (s_multi_recv.total_len == s_multi_recv.offset) {
			checksum = ble_wifimate_server_checksum_cal(s_multi_recv.total_len, s_multi_recv.data);
			BT_LOGD("%s: recv_checksum=%u, cal_checksum=%u\r\n",
					__func__, s_multi_recv.checksum, checksum);
			if (checksum == s_multi_recv.checksum) {
				struct wifi_conn_config_t conn_config = {0};
				BT_LOGA("[APP] BLE WiFiMate conn_handle=%d wifi scan indicate recv checksum pass!\r\n", conn_handle);
				ret = ble_wifimate_parse_wifi_connect_config(s_multi_recv.total_len, s_multi_recv.data, &conn_config);
				if (ret == RTK_BT_OK) {
					ret = ble_wifimate_wifi_connect(conn_handle, &conn_config);
				}
			} else {
				ret = RTK_BT_FAIL;
				BT_LOGA("[APP] BLE WiFiMate conn_handle=%d wifi scan indicate recv checksum fail!\r\n", conn_handle);
			}
			ble_wifimate_char_multi_recv_data_deinit();
		}
	}

	return ret;
}

static uint8_t ble_wifimate_state_check_update(uint8_t hdl_idx)
{
	uint8_t check = false;

	switch (hdl_idx) {
	case BLE_WIFIMATE_KEY_NEGOTIATE_INDEX:
		if (s_ble_wifimate_state == BLE_WIFIMATE_SERVER_STATE_CONNECT) {
			check = true;
		}
		break;
	case BLE_WIFIMATE_WIFI_SCAN_ENABLE_INDEX:
		if ((s_ble_wifimate_state == BLE_WIFIMATE_SERVER_STATE_CONNECT) || \
			(s_ble_wifimate_state == BLE_WIFIMATE_SERVER_STATE_WIFI_SCAN)) {
			s_ble_wifimate_state = BLE_WIFIMATE_SERVER_STATE_WIFI_SCAN;
			check = true;
		}
		break;
	case BLE_WIFIMATE_WIFI_CONNECT_ENABLE_INDEX:
		if ((s_ble_wifimate_state == BLE_WIFIMATE_SERVER_STATE_WIFI_SCAN) || \
			(s_ble_wifimate_state == BLE_WIFIMATE_SERVER_STATE_WIFI_CONN)) {
			s_ble_wifimate_state = BLE_WIFIMATE_SERVER_STATE_WIFI_CONN;
			check = true;
		}

		break;
	default:
		break;
	}
	return check;
}
static void ble_wifimate_server_write_hdl(void *data)
{
	uint8_t state_match = 0;
	uint16_t index;
	rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
	rtk_bt_gatts_write_resp_param_t write_resp = {0};
	uint16_t ret = RTK_BT_OK;

	if (!p_write_ind || p_write_ind->app_id != BLE_WIFIMATE_SRV_ID) {
		return;
	}

	index = p_write_ind->index;
	write_resp.app_id = p_write_ind->app_id;
	write_resp.conn_handle = p_write_ind->conn_handle;
	write_resp.cid = p_write_ind->cid;
	write_resp.index = p_write_ind->index;
	write_resp.type = p_write_ind->type;

	if (!p_write_ind->len || !p_write_ind->value) {
		BT_LOGA("[APP] BLE WiFiMate server write value is empty!\r\n");
		write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE_SIZE;
		goto send_write_rsp;
	}

	BT_DUMPD("[APP] GATTS recv write:\r\n", p_write_ind->value, p_write_ind->len);

	switch (p_write_ind->index) {
	case BLE_WIFIMATE_KEY_NEGOTIATE_INDEX:
		ret = ble_wifimate_server_key_negotiate_hdl(p_write_ind->len, p_write_ind->value);
		if (ret != RTK_BT_OK) {
			write_resp.err_code = RTK_BT_ATT_ERR_WRITE_REQUEST_REJECTED;
		}
		break;
	case BLE_WIFIMATE_WIFI_SCAN_ENABLE_INDEX:
		BT_LOGD("[APP] BLE WiFiMate server write wifi scan enable\r\n");
		break;
	case BLE_WIFIMATE_WIFI_CONNECT_ENABLE_INDEX:
		BT_LOGD("[APP] BLE WiFiMate server write wifi connect enable\r\n");
		break;
	default:
		BT_LOGE("[APP] BLE WiFiMate server write event unknown index: %d\r\n", p_write_ind->index);
		write_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
		break;
	}

	state_match = ble_wifimate_state_check_update(index);
	if (!state_match) {
		write_resp.err_code = RTK_BT_ATT_ERR_WRITE_REQUEST_REJECTED;
	}

send_write_rsp:
	ret = rtk_bt_gatts_write_resp(&write_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA("[APP] BLE WiFiMate server response for client write success!\r\n");
	} else {
		BT_LOGE("[APP] BLE WiFiMate server response for client write failed, err: 0x%x\r\n", ret);
	}

	if (state_match) {
		switch (index) {
		case BLE_WIFIMATE_WIFI_SCAN_ENABLE_INDEX:
			ble_wifimate_server_write_wifi_scan_hdl(p_write_ind->conn_handle, p_write_ind->len, p_write_ind->value);
			break;
		case BLE_WIFIMATE_WIFI_CONNECT_ENABLE_INDEX:
			ble_wifimate_server_write_wifi_connect_hdl(p_write_ind->conn_handle, p_write_ind->len, p_write_ind->value);
			break;
		default:
			break;
		}
	}
}

static void ble_wifimate_server_cccd_hdl(void *data)
{
	rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;

	if (!p_cccd_ind || p_cccd_ind->app_id != BLE_WIFIMATE_SRV_ID) {
		BT_LOGA("[APP] BLE WiFiMate server cccd invalid\r\n");
		return;
	}

	switch (p_cccd_ind->index) {
	case BLE_WIFIMATE_WIFI_SCAN_INFO_CCCD_INDEX:
		if (p_cccd_ind->value & RTK_BT_GATT_CCC_INDICATE) {
			s_cccd_ind_en_wifi_scan = 1;
			BT_LOGA("[APP] BLE WiFiMate server wifi scan info indicate cccd, indicate bit enable\r\n");
		} else {
			s_cccd_ind_en_wifi_scan = 0;
			BT_LOGA("[APP] BLE WiFiMate server wifi scan info indicate cccd, indicate bit disable\r\n");
		}
		break;
	case BLE_WIFIMATE_WIFI_CONNECT_STATE_CCCD_INDEX:
		if (p_cccd_ind->value & RTK_BT_GATT_CCC_INDICATE) {
			s_cccd_ind_en_wifi_conn = 1;
			BT_LOGA("[APP] BLE WiFiMate server wifi connect state indicate cccd, indicate bit enable\r\n");
		} else {
			s_cccd_ind_en_wifi_conn = 0;
			BT_LOGA("[APP] BLE WiFiMate server wifi connect state indicate cccd, indicate bit disable\r\n");
		}
		break;
	default:
		break;
	}
}

static void ble_wifimate_server_indicate_complete_hdl(void *data)
{
	rtk_bt_gatts_ntf_and_ind_ind_t *p_ind_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;

	if (!p_ind_ind || p_ind_ind->app_id != BLE_WIFIMATE_SRV_ID) {
		return;
	}

	if (RTK_BT_OK == p_ind_ind->err_code) {
		BT_LOGD("[APP] BLE WiFiMate server indicate succeed!\r\n");
	} else {
		BT_LOGE("[APP] BLE WiFiMate server indicate failed, err: 0x%x\r\n", p_ind_ind->err_code);
	}

	if (p_ind_ind->index == BLE_WIFIMATE_WIFI_SCAN_INFO_VAL_INDEX) {
		ble_wifimate_server_send_wifi_scan_info_segment(p_ind_ind->conn_handle);
	} else if (p_ind_ind->index == BLE_WIFIMATE_WIFI_CONNECT_STATE_VAL_INDEX) {
		rtk_bt_le_gap_disconnect(p_ind_ind->conn_handle);
	}
}

rtk_bt_evt_cb_ret_t ble_wifimate_gatts_app_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;

	switch (event) {
	case RTK_BT_GATTS_EVT_MTU_EXCHANGE: {
		ble_wifimate_server_mtu_exchange_hdl(data);
		break;
	}

	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		ble_wifimate_server_register_service_hdl(data);
		break;
	}

	case RTK_BT_GATTS_EVT_WRITE_IND: {
		ble_wifimate_server_write_hdl(data);
		break;
	}

	case RTK_BT_GATTS_EVT_CCCD_IND: {
		ble_wifimate_server_cccd_hdl(data);
		break;
	}

	case RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND: {
		ble_wifimate_server_indicate_complete_hdl(data);
		break;
	}

	default:
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

uint16_t ble_wifimate_server_add(void)
{
	s_ble_wifimate_srv.type = GATT_SERVICE_OVER_BLE;
	s_ble_wifimate_srv.server_info = 0;
	s_ble_wifimate_srv.user_data = NULL;
	s_ble_wifimate_srv.register_status = 0;

	return rtk_bt_gatts_register_service(&s_ble_wifimate_srv);
}

uint16_t ble_wifimate_server_connect(uint16_t conn_hdl)
{
	(void)conn_hdl;

	if (s_ble_wifimate_state != BLE_WIFIMATE_SERVER_STATE_IDLE) {
		BT_LOGA("[APP] ble wifimate has already connected.\r\n");
	}
	s_ble_wifimate_state = BLE_WIFIMATE_SERVER_STATE_CONNECT;

	return RTK_BT_OK;
}

void ble_wifimate_server_disconnect(uint16_t conn_hdl)
{
	(void)conn_hdl;
	s_cccd_ind_en_wifi_scan = 0;
	s_cccd_ind_en_wifi_conn = 0;
	s_ble_wifimate_state = BLE_WIFIMATE_SERVER_STATE_IDLE;
	memset(&s_pw_decode, 0, sizeof(s_pw_decode));

	ble_wifimate_char_multi_indicate_data_deinit();
	ble_wifimate_char_multi_recv_data_deinit();
}

void ble_wifimate_server_deinit(void)
{
	s_cccd_ind_en_wifi_scan = 0;
	s_cccd_ind_en_wifi_conn = 0;
	s_ble_wifimate_state = BLE_WIFIMATE_SERVER_STATE_IDLE;
	memset(&s_pw_decode, 0, sizeof(s_pw_decode));

	ble_wifimate_char_multi_indicate_data_deinit();
	ble_wifimate_char_multi_recv_data_deinit();
}
