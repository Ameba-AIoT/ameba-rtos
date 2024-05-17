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

#ifdef CONFIG_CSI
void wifi_hal_csi_deinit(void *a)
{
	UNUSED(a);
}

s32 wifi_hal_csi_cfg(void *a, void *b)
{
	UNUSED(a);
	UNUSED(b);
	return -1;
}

s32 wifi_hal_csi_en(void *a, void *b)
{
	UNUSED(a);
	UNUSED(b);
	return -1;
}

s32 wifi_hal_csi_report(void *a, u32 buf_len, void *c, void *d)
{
	UNUSED(a);
	UNUSED(buf_len);
	UNUSED(c);
	UNUSED(d);
	return -1;
}

void wifi_hal_csi_pkt_hdl(void *a, void *b, void *c)
{
	UNUSED(a);
	UNUSED(b);
	UNUSED(c);
}
#endif /* CONFIG_CSI */

