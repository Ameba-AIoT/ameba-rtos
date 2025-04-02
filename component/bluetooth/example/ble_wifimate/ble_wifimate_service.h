/*
*******************************************************************************
* Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __BLE_WIFIMATE_SERVICE_H__
#define __BLE_WIFIMATE_SERVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtk_bt_common.h>

#define BLE_WIFIMATE_UUID_SRV                           0xC00D
#define BLE_WIFIMATE_UUID_CHAR_KEY_NEGOTIATE            0xD020
#define BLE_WIFIMATE_UUID_CHAR_WIFI_SCAN_ENABLE         0xD021
#define BLE_WIFIMATE_UUID_CHAR_WIFI_SCAN_INFO           0xD022
#define BLE_WIFIMATE_UUID_CHAR_WIFI_CONNECT_ENABLE      0xD023
#define BLE_WIFIMATE_UUID_CHAR_WIFI_CONNECT_STATE       0xD024

#define BLE_WIFIMATE_KEY_LEN                            (13)

#define BLE_WIFIMATE_CHAR_NOGOTIATE_KEY_LEN             (14)
#define CHAR_NEGOTIATE_KEY_FIRST_ELE_LEN                (1)
#define CHAR_NEGOTIATE_KEY_SECOND_ELE_LEN               (13)

#define CHAR_WIFI_CONN_ENABLE_INITIAL_FIRST_ELE_LEN     (2)
#define CHAR_WIFI_CONN_ENABLE_INITIAL_SECOND_ELE_LEN    (1)
#define CHAR_WIFI_CONN_ENABLE_INITIAL_THIRD_ELE_LEN     (4)

#define CHAR_WIFI_CONN_ENABLE_SEG_FIRST_ELE_LEN         (1)
#define CHAR_WIFI_CONN_ENABLE_SEG_THIRD_ELE_LEN         (1)
#define CHAR_WIFI_CONN_ENABLE_SEG_FORTH_ELE_LEN         (1)

#define BLE_WIFIMATE_CHAR_WIFI_SCAN_ENABLE_DATA_LEN     (1)
#define BLE_WIFIMATE_CHAR_WIFI_CONNECT_STATE_LEN        (2)

#define BLE_WIFIMATE_SSID_LEN_MAX                       (33)

#define BLE_WIFIMATE_ENCRYPT_DECRYPT_KEY_LEN            (16)

struct ble_wifimate_encrypt_decrypt_t {
	uint8_t         algorithm_type;
	uint8_t         key[BLE_WIFIMATE_ENCRYPT_DECRYPT_KEY_LEN];
};

struct wifi_scan_info_t {
	uint8_t         ssid_len;
	unsigned char   ssid[BLE_WIFIMATE_SSID_LEN_MAX];
	int16_t         rssi;
	uint8_t         security;
	uint8_t         channel;
};

struct wifi_conn_config_t {
	uint8_t         ssid_len;
	unsigned char   ssid[BLE_WIFIMATE_SSID_LEN_MAX];
	uint8_t         security;
	uint8_t         password_len;
	unsigned char   *password;
};

enum encypt_decrypt_algorithm_e {
	BLE_WIFIMATE_ENCRYPT_DECRYPT_ALGO_NONE = 0,
	BLE_WIFIMATE_ENCRYPT_DECRYPT_ALGO_AES_ECB = 1,
	BLE_WIFIMATE_ENCRYPT_DECRYPT_ALGO_NUM = 2,
};

struct key_negotiate_config_t {
	uint8_t         algorithm;
	uint8_t         key[BLE_WIFIMATE_KEY_LEN];
};

struct ble_wifimate_char_send_data_t {
	uint8_t         flag;
	uint16_t        total_len;
	uint8_t         seg_num;
	uint32_t        checksum;
	uint8_t         seg_idx;
	uint16_t        offset;
	uint8_t         *data;
};

struct ble_wifimate_char_recv_data_t {
	uint8_t         flag;
	uint16_t        total_len;
	uint8_t         seg_num;
	uint32_t        checksum;
	uint8_t         seg_idx;
	uint16_t        offset;
	uint8_t         *data;
};

enum ble_wifimate_wifi_conn_state_e {
	BWM_WIFI_STATE_IDLE = 0,
	BWM_WIFI_STATE_CONNECTED = 1,
};

enum ble_wifimate_wifi_conn_error_code_e {
	BWM_OK = 0,
	BWM_ERR_ERROR = 1,
	BWM_ERR_PARAM_INVALID = 2,
	BWM_ERR_SSID_EMPTY = 3,
	BWM_ERR_PASSWORD_WRONG = 4,
	BWM_ERR_AP_NOT_FOUND = 5,
	BWM_ERR_BUSY = 6,
	BWM_ERR_NOMEM = 7,
	BWM_ERR_TIMEOUT = 8,
	BWM_ERR_AUTH_FAIL = 9,
	BWM_ERR_ASSOC_FAIL = 10,
	BWM_ERR_DHCP_ADDRESS_ASSIGN_FAIL = 11,
};

enum ble_wifimate_security_type_e {
	BWM_SECURITY_OPEN               = 0,    /**< Open security                           */
	BWM_SECURITY_WEP_PSK            = 1,    /**< WEP Security with open authentication   */
	BWM_SECURITY_WEP_SHARED         = 2,    /**< WEP Security with shared authentication */
	BWM_SECURITY_WPA_TKIP_PSK       = 3,    /**< WPA Security with TKIP                  */
	BWM_SECURITY_WPA_AES_PSK        = 4,    /**< WPA Security with AES                   */
	BWM_SECURITY_WPA_MIXED_PSK      = 5,    /**< WPA Security with AES & TKIP            */
	BWM_SECURITY_WPA2_TKIP_PSK      = 6,    /**< WPA2 Security with TKIP                 */
	BWM_SECURITY_WPA2_AES_PSK       = 7,    /**< WPA2 Security with AES                  */
	BWM_SECURITY_WPA2_MIXED_PSK     = 8,    /**< WPA2 Security with AES & TKIP           */
	BWM_SECURITY_WPA_WPA2_TKIP_PSK  = 9,    /**< WPA/WPA2 Security with TKIP             */
	BWM_SECURITY_WPA_WPA2_AES_PSK   = 10,   /**< WPA/WPA2 Security with AES              */
	BWM_SECURITY_WPA_WPA2_MIXED_PSK = 11,   /**< WPA/WPA2 Security with AES & TKIP       */
	BWM_SECURITY_WPA3_AES_PSK       = 12,   /**< WPA3-SAE with AES security            */
	BWM_SECURITY_WPA3_OWE           = 13,   /**< WPA3-OWE with AES security            */
	BWM_SECURITY_WPA2_WPA3_MIXED    = 14,   /**< WPA3-SAE/WPA2 with AES security           */
	BWM_SECURITY_WPA2_AES_CMAC      = 15,
	BWM_SECURITY_UNKNOWN            = 255,
};

/* ble wifimate server api */
rtk_bt_evt_cb_ret_t ble_wifimate_gatts_app_callback(uint8_t event, void *data, uint32_t len);
uint16_t ble_wifimate_server_add(void);
uint16_t ble_wifimate_server_connect(uint16_t conn_hdl);
void ble_wifimate_server_disconnect(uint16_t conn_hdl);
void ble_wifimate_server_deinit(void);

/* ble wifimate client api */
rtk_bt_evt_cb_ret_t ble_wifimate_gattc_app_callback(uint8_t event, void *data, uint32_t len);
uint16_t ble_wifimate_client_add(void);
uint16_t ble_wifimate_client_attach_conn(uint16_t conn_handle);
uint16_t ble_wifimate_client_detach_conn(uint16_t conn_handle);
uint16_t ble_wifimate_client_encrypt_set(struct ble_wifimate_encrypt_decrypt_t *enc);
uint16_t ble_wifimate_client_wifi_connect(uint16_t conn_handle, struct wifi_conn_config_t *conn_info);
uint16_t ble_wifimate_client_wifi_scan(uint16_t conn_handle);
void ble_wifimate_client_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* __BLE_WIFIMATE_SERVICE_H__ */