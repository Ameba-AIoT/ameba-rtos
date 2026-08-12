/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __IPSS_SERVICE_H__
#define __IPSS_SERVICE_H__

#ifdef __cplusplus
extern "C"  {
#endif

#include <rtk_bt_def.h>

void ipss_srv_callback(uint8_t event, void *data);

uint16_t ipss_srv_add(void);

#ifdef __cplusplus
}
#endif

#endif  /* __IPSS_SERVICE_H__ */
