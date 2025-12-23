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

#include "wifi_api_types.h"

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

#ifdef CONFIG_MP_INCLUDED
void wifi_hal_radar_start(void)
{

}

void wifi_hal_radar_stop(void)
{

}

void wifi_hal_radar_bw(u8 bw_idx)
{
	UNUSED(bw_idx);
}

void wifi_hal_radar_chrip(u8 chrip_width, u8 chirp_num)
{
	UNUSED(chrip_width);
	UNUSED(chirp_num);
}

void wifi_hal_radar_frame(u16 frame_intev, u16 frame_num)
{
	UNUSED(frame_intev);
	UNUSED(frame_num);
}

void wifi_hal_radar_trx(u8 start)
{
	UNUSED(start);
}
#endif

#endif /* CONFIG_RADAR */

