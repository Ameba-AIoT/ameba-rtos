/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
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

#endif /* CONFIG_WIFI_ANTDIV */

