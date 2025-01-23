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
#define __WIFI_FEATURE_DIS_CSI_C__

#include "rtw_wifi_constants.h"

#ifdef CONFIG_RADAR
void wifi_hal_radar_en_by_user(u8 *radar_en_by_user)
{
	*radar_en_by_user = 0;
}

void wifi_hal_radar_deinit(void *a)
{
	UNUSED(a);
}

s32 wifi_hal_radar_cfg(void *a)
{
	UNUSED(a);
	return -1;
}

s32 wifi_hal_radar_en(void *a)
{
	UNUSED(a);
	return -1;
}

void wifi_hal_radar_interrupt_handle(void)
{

}

void wifi_hal_radar_rpt_hdl(u8 *buf, void *a)
{
	UNUSED(buf);
	UNUSED(a);
}
#endif /* CONFIG_RADAR */

