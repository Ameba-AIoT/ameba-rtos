/*
 * Copyright (c) 2025 Realtek Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "osif.h"
#include "hci_common.h"
#include "hci_platform.h"
#include "bt_debug.h"
#include "ameba.h"
#include "platform_autoconf.h"
#if defined(CONFIG_WLAN) && CONFIG_WLAN
#include "wifi_api.h"
#include "wifi_intf_drv_to_app_internal.h"
extern int wifi_set_ips_internal(u8 enable);
#endif

bool rtk_bt_pre_enable(void)
{
#if defined(CONFIG_WLAN) && CONFIG_WLAN
	if (!(wifi_is_running(STA_WLAN_INDEX) || wifi_is_running(SOFTAP_WLAN_INDEX))) {
		BT_LOGE("WiFi is OFF! Please Restart BT after Wifi on!\r\n");
		return false;
	}

	wifi_ps_en_by_bt_state(DISABLE);
#endif

	return true;
}

void rtk_bt_post_enable(void)
{
#if defined(CONFIG_WLAN) && CONFIG_WLAN
	wifi_ps_en_by_bt_state(ENABLE);
#endif
}

