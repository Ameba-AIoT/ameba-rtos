/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __RTK_BT_CONFIG_SERVICE_H__
#define __RTK_BT_CONFIG_SERVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtk_bt_def.h>

#define BT_CONFIG_UUID_SRV                      0xFF01
#define BT_CONFIG_UUID_CHAR_VAL_DATA_TRX        0x2A0D

#define RTK_BT_UUID_BT_CONFIG                   BT_UUID_DECLARE_16(BT_CONFIG_UUID_SRV)
#define RTK_BT_UUID_DATA_TRX_VAL                BT_UUID_DECLARE_16(BT_CONFIG_UUID_CHAR_VAL_DATA_TRX)

#define BT_CONFIG_READ_MAX_LEN                  300
#define BT_CONFIG_READ_WRITE_INDEX              2

uint16_t bt_config_srv_add(void);
void bt_config_service_callback(uint8_t event, void *data);

#ifdef __cplusplus
}
#endif

#endif // __RTK_BT_CONFIG_SERVICE_H__
