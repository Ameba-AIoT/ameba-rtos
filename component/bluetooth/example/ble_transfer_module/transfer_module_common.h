/*
*******************************************************************************
* Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __TRANSFER_MODULE_COMMON_H__
#define __TRANSFER_MODULE_COMMON_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <rtk_bt_common.h>
#include <rtk_bt_gatts.h>

#define TRANSFER_MODULE_ATTR_SERVICE        0
#define TRANSFER_MODULE_ATTR_READ           1
#define TRANSFER_MODULE_ATTR_WRITE          2
#define TRANSFER_MODULE_ATTR_NOTIFY         3
#define TRANSFER_MODULE_ATTR_INDICATE       4

#define TRANSFER_MODULE_DEV_NAME_MAX_LEN    40
#define TRANSFER_MODULE_READ_VAL_MAX_LEN    30

#define TRANSFER_MODULE_USER_DEFINED_SRV_MAX_NUM    4
#define TRANSFER_MODULE_USER_DEFINED_CHAR_MAX_NUM   8

#define TRNASFER_MODULE_NOTIFY              1
#define TRANSFER_MODULE_INDICATE            2

#define TRANSFER_MODULE_SERVICE_INDEX            0
#define TRANSFER_MODULE_READ_CHRC_INDEX          1
#define TRANSFER_MODULE_READ_VAL_INDEX           2
#define TRANSFER_MODULE_WRITE_CHRC_INDEX         3
#define TRANSFER_MODULE_WRITE_VAL_INDEX          4
#define TRANSFER_MODULE_NOTIFY_CHRC_INDEX        5
#define TRANSFER_MODULE_NOTIFY_VAL_INDEX         6
#define TRANSFER_MODULE_NOTIFY_CCCD_INDEX        7
#define TRANSFER_MODULE_INDICATE_CHRC_INDEX      8
#define TRANSFER_MODULE_INDICATE_VAL_INDEX       9
#define TRANSFER_MODULE_INDICATE_CCCD_INDEX      10

struct transfer_module_char_t {
	uint16_t char_uuid;
	uint16_t char_property;
};

struct transfer_module_srv_t {
	uint16_t srv_uuid;
	uint16_t char_num;
	struct transfer_module_char_t chars[TRANSFER_MODULE_USER_DEFINED_CHAR_MAX_NUM];
};

enum transfer_module_char_index_type_e {
	CHAR_INDEX_TYPE_READ_VALUE_INDEX = 0,
	CHAR_INDEX_TYPE_WRITE_VALUE_INDEX = 1,
	CHAR_INDEX_TYPE_NOTIFY_VALUE_INDEX = 2,
	CHAR_INDEX_TYPE_INDICATE_VALUE_INDEX = 3,
	CHAR_INDEX_TYPE_NOTIFY_CCCD_INDEX = 4,
	CHAR_INDEX_TYPE_INDICATE_CCCD_INDEX = 5
};

uint16_t ble_transfer_module_get_dev_name(uint8_t buf_len, char *buf);
uint16_t ble_transfer_module_set_dev_name(char *name);

/********************************Transfer Module Server related*******************************/
rtk_bt_evt_cb_ret_t ble_transfer_module_gatts_app_callback(uint8_t event, void *data, uint32_t len);
uint16_t ble_transfer_module_server_disconnect(uint16_t conn_handle);
uint16_t ble_transfer_module_server_add(void);
uint16_t ble_transfer_module_server_deinit(void);
uint16_t ble_transfer_module_get_uuid(uint8_t attr, uint16_t *uuid);
uint16_t ble_transfer_module_set_uuid(uint8_t attr, uint16_t uuid);

uint16_t ble_transfer_module_srv_add(struct transfer_module_srv_t *add_srv);
uint16_t ble_transfer_module_srv_add_check(uint16_t uuid);
void ble_transfer_module_srv_del(void);
uint16_t ble_transfer_module_char_read_val_set(uint16_t app_id, uint16_t char_idx, uint8_t len, const void *data);

/********************************Transfer Module Client related*******************************/
rtk_bt_evt_cb_ret_t ble_transfer_module_gattc_app_callback(uint8_t event, void *data, uint32_t len);
uint16_t ble_transfer_module_client_add(void);


#ifdef __cplusplus
}
#endif

#endif /* __TRANSFER_MODULE_COMMON_H__ */