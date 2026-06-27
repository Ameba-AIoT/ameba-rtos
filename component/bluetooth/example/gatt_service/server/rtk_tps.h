/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __TPS_SERVICE_H__
#define __TPS_SERVICE_H__

#ifdef __cplusplus
extern "C"  {
#endif

#include <rtk_bt_def.h>

void tx_power_srv_set_level(int8_t tx_power_level);

void tx_power_srv_callback(uint8_t event, void *data);

uint16_t tx_power_srv_add(void);

#ifdef __cplusplus
}
#endif

#endif  /* __TPS_SERVICE_H__ */
