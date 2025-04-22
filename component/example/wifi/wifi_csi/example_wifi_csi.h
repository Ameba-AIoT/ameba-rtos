#ifndef __EXAMPLE_WIFI_CSI_H__
#define __EXAMPLE_WIFI_CSI_H__

/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include "platform_autoconf.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "lwip_netconf.h"
#include "wifi_api.h"
#include "rtw_autoconf.h"

void example_wifi_csi_report_cb(u8 *buf, s32 buf_len, s32 flags, void *userdata);
void wifi_csi_show(u8 *csi_buf);
void example_wifi_csi(void);

#endif //#ifndef __EXAMPLE_WIFI_CSI_H__
