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

#define TRANSFER_MODULE_ATTR_SERVICE        0
#define TRANSFER_MODULE_ATTR_READ           1
#define TRANSFER_MODULE_ATTR_WRITE          2
#define TRANSFER_MODULE_ATTR_NOTIFY         3
#define TRANSFER_MODULE_ATTR_INDICATE       4

#define TRANSFER_MODULE_DEV_NAME_MAX_LEN    40
#define TRANSFER_MODULE_READ_VAL_MAX_LEN    30

uint16_t ble_transfer_module_get_dev_name(uint8_t buf_len, char *buf);
uint16_t ble_transfer_module_set_dev_name(char *name);

/********************************Transfer Module Server related*******************************/
rtk_bt_evt_cb_ret_t ble_transfer_module_gatts_app_callback(uint8_t event, void *data, uint32_t len);
uint16_t ble_transfer_module_server_disconnect(uint16_t conn_handle);
uint16_t ble_transfer_module_server_add(void);
uint16_t ble_transfer_module_server_deinit(void);
uint16_t ble_transfer_module_get_uuid(uint8_t attr, uint16_t *uuid);
uint16_t ble_transfer_module_set_uuid(uint8_t attr, uint16_t uuid);
uint16_t ble_transfer_module_update_read_val(uint8_t len, uint8_t *data);

/********************************Transfer Module Client related*******************************/
rtk_bt_evt_cb_ret_t ble_transfer_module_gattc_app_callback(uint8_t event, void *data, uint32_t len);
uint16_t ble_transfer_module_client_add(void);


#ifdef __cplusplus
}
#endif

#endif /* __TRANSFER_MODULE_COMMON_H__ */