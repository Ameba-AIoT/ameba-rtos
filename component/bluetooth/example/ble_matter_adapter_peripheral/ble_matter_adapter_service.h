/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __BLE_MATTER_ADAPTER_SERVICE_H__
#define __BLE_MATTER_ADAPTER_SERVICE_H__

#ifdef __cplusplus
extern "C"  {
#endif

#include <rtk_bt_def.h>

#define BLE_MATTER_ADAPTER_SRV_ID           0x55

#define BLE_MATTER_SERVICE_CHAR_RX_INDEX                 0x02
#define BLE_MATTER_SERVICE_CHAR_TX_INDEX                 0x04
#define BLE_MATTER_SERVICE_CHAR_INDICATE_CCCD_INDEX      (BLE_MATTER_SERVICE_CHAR_TX_INDEX + 1)
#define BLE_MATTER_SERVICE_C3_INDEX                      0x07

typedef enum {
    BLE_MATTER_MSG_CONNECTED = 1,
    BLE_MATTER_MSG_DISCONNECTED,
    BLE_MATTER_MSG_WRITE_CB,
    BLE_MATTER_MSG_CCCD_EN,
    BLE_MATTER_MSG_CCCD_DIS,
    BLE_MATTER_MSG_SEND_COMPLETE,
    BLE_MATTER_MSG_READ
} BLE_MATTER_APP_MSG;

typedef struct {
    uint8_t conn_id;
} BLE_MATTER_APP_CONNECT_CB_ARG;

typedef struct {
    uint8_t conn_id;
    uint16_t disc_cause;
} BLE_MATTER_APP_DISCONNECT_CB_ARG;

typedef struct {
    uint8_t conn_id;
    uint8_t *p_value;
    uint16_t len;
} BLE_MATTER_APP_WRITE_CB_ARG;

typedef struct {
    uint8_t conn_id;
    uint8_t indicationsEnabled;
    uint8_t notificationsEnabled;
} BLE_MATTER_APP_CCCD_CB_ARG;

typedef struct {
    uint8_t conn_id;
} BLE_MATTER_APP_SEND_COMPLETE_CB_ARG;

typedef struct {
    uint8_t **pp_value;
    uint16_t *p_len;
} BLE_MATTER_APP_READ_CB_ARG;

void ble_matter_adapter_ble_service_callback(uint8_t event, void *data);

void ble_matter_adapter_srv_cccd_indicate(uint16_t conn_handle);

uint16_t ble_matter_adapter_srv_add(void);

void ble_matter_srv_disconnect(uint16_t conn_handle);

void ble_matter_srv_status_deinit(void);

#ifdef __cplusplus
}
#endif

#endif  /* __SIMPLE_BLE_SERVICE_H__ */
