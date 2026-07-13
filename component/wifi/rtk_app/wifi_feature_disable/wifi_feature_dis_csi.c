/**
  ******************************************************************************
  * @file    wifi_feature_dis_csi.c
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
#define __WIFI_FEATURE_DIS_CSI_C__

#include "wifi_api_types.h"

#ifdef CONFIG_CSI
void wifi_hal_csi_deinit(void *a)
{
	UNUSED(a);
}

s32 wifi_hal_csi_cfg(void *b)
{
	UNUSED(b);
	return -1;
}

s32 wifi_hal_csi_en(void *b)
{
	UNUSED(b);
	return -1;
}

void wifi_hal_csi_pkt_hdl(void *b, void *c)
{
	UNUSED(b);
	UNUSED(c);
}

void wifi_hal_csi_en_by_user(u8 *csi_en_by_user)
{
	*csi_en_by_user = 0;
}
#endif /* CONFIG_CSI */

