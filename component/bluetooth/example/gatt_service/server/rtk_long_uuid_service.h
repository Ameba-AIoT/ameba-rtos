/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __LONG_UUID_SERVICE_H__
#define __LONG_UUID_SERVICE_H__

#ifdef __cplusplus
extern "C"  {
#endif

#include <rtk_bt_def.h>

void long_uuid_service_callback(uint8_t event, void *data);

uint16_t long_uuid_srv_add(void);

#ifdef __cplusplus
}
#endif

#endif  /* __LONG_UUID_SERVICE_H__ */