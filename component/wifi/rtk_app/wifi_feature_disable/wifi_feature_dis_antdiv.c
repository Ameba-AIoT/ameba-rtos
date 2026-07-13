/**
  ******************************************************************************
  * @file    wifi_feature_dis_antdiv.c
  * @author
  * @version
  * @date
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
#define __WIFI_FEATURE_DIS_ANTDIV_C__

#include "wifi_api_types.h"

#ifdef CONFIG_WIFI_ANTDIV
void wifi_hal_antdiv_training_timer_init(void *a)
{
	UNUSED(a);
}

void wifi_hal_antdiv_init(void *a)
{
	UNUSED(a);
}

void wifi_hal_antdiv_watchdog(void *a)
{
	UNUSED(a);
}

void wifi_hal_antdiv_get_rssi_info(bool a, u8 b, u8 c)
{
	UNUSED(a);
	UNUSED(b);
	UNUSED(c);
}

void wifi_hal_antdiv_get_rate_info(bool a, u8 b, u8 c)
{
	UNUSED(a);
	UNUSED(b);
	UNUSED(c);
}

void wifi_hal_antdiv_get_evm_info(bool a, u8 b)
{
	UNUSED(a);
	UNUSED(b);
}

void wifi_hal_antdiv_set_ant(u8 target_ant)
{
	UNUSED(target_ant);
}

void wifi_hal_antdiv_update_rx_idle_ant(u8 ant)
{
	UNUSED(ant);
}
#endif /* CONFIG_WIFI_ANTDIV */

