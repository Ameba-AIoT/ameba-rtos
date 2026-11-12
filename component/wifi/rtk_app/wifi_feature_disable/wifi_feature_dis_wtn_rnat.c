/**
  ******************************************************************************
  * @file    wifi_feature_dis_wtn_rnat.c
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
#define __WIFI_FEATURE_DIS_WTN_RNAT_C__

#include "wifi_api_types.h"

#if defined(CONFIG_WIFI_TUNNEL) && defined(CONFIG_WTN_RNAT_AUTOSEL) && !defined(CONFIG_WHC_HOST) && !defined(CONFIG_RNAT_AUTOSEL_EN)

void wifi_tunnelapi_rnat_sel_init(void)
{

}

void wifi_tunnelapi_rnat_sel_deinit(void)
{

}

void wifi_tunnelapi_rnat_sel_cand_reset(void)
{

}

u8 wifi_tunnelapi_rnat_sel_group_mark(void *scan_network, void *pattrib)
{
	UNUSED(scan_network);
	UNUSED(pattrib);

	return 0;
}

void wifi_tunnelapi_rnat_sel_record(void *scan_network)
{
	UNUSED(scan_network);
}

u8 wifi_tunnelapi_rnat_sel_judge(void *new_network)
{
	UNUSED(new_network);

	return TRUE;
}

/* RTK_FAIL: the caller handles it as without auto selection */
int wtn_rnat_sel_joincmd(void *pcur_network)
{
	UNUSED(pcur_network);

	return RTK_FAIL;
}

int wtn_rnat_sel_joinbss_done(void)
{
	return RTK_FAIL;
}

void wtn_rnat_sel_leavebss(void)
{

}

void wtn_rnat_sel_on_bcn(void *phelpie, u8 *sender, s8 rssi)
{
	UNUSED(phelpie);
	UNUSED(sender);
	UNUSED(rssi);
}

void wifi_tunnelapi_rnat_sel_target_fixup(void)
{

}

#endif /* CONFIG_RNAT_AUTOSEL_EN */
