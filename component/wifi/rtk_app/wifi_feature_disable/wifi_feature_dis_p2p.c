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
#ifndef __WIFI_FEATURE_DIS_P2P_C__
#define __WIFI_FEATURE_DIS_P2P_C__

#include "wifi_intf_drv_to_app_internal.h"

#ifdef CONFIG_P2P
void wifi_hal_p2p_addr_cam_init(void)
{
}

void rtw_p2p_set_role(enum rtw_p2p_role role)
{
	UNUSED(role);
}

u8 *rtw_p2p_set_assoc_p2p_ie(u8 *pframe, u32 *pktlen)
{
	UNUSED(pframe);
	UNUSED(pktlen);
	return pframe;
}

u8 *rtw_p2p_set_probereq_p2p_ie(u8 *pframe, u32 *pktlen)
{
	UNUSED(pframe);
	UNUSED(pktlen);
	return pframe;
}

u8 *rtw_get_p2p_ie(u8 *in_ie, u32 in_len, u8 *p2p_ie, u32 *p2p_ielen)
{
	UNUSED(in_ie);
	UNUSED(in_len);
	UNUSED(p2p_ie);
	UNUSED(p2p_ielen);
	return NULL;
}

u8 *rtw_remove_p2p_ie(u8 *pframe, u32 *pkt_len)
{
	UNUSED(pframe);
	UNUSED(pkt_len);
	return pframe;
}

int  rtw_p2p_on_action(u8 iface_type, void *precv_frame)
{
	UNUSED(iface_type);
	UNUSED(precv_frame);
	return RTK_FAIL;
}

int rtw_p2p_check_probersp_ess_bit(void *scan_network, u8 *pframe)
{
	UNUSED(scan_network);
	UNUSED(pframe);
	return RTK_FAIL;
}

void rtw_p2p_set_mgnt_tx_rate(void)
{
}

void rtw_p2p_set_supported_rates(u8 *supported_rateset, u8 bssrate_len)
{
	UNUSED(supported_rateset);
	UNUSED(bssrate_len);
}

int rtw_p2p_probereq_indicate(u8 *pframe, u32 len, u8 iface_type)
{
	UNUSED(pframe);
	UNUSED(len);
	UNUSED(iface_type);
	return RTK_FAIL;
}

void rtw_p2p_check_wildcast_ssid(u32 scan_ssidlen, u8 *be_p2p_wildcast_ssid)
{
	UNUSED(scan_ssidlen);
	UNUSED(be_p2p_wildcast_ssid);
}

int rtw_p2p_check_scan_option(u8 scan_option, u8 *iface_type)
{
	UNUSED(scan_option);
	UNUSED(iface_type);
	return 0;
}

void rtw_p2p_scan_done_process(void)
{
}

void rtw_p2p_set_wlan1_scan(u8 enable, u8 *p_iface_type)
{
	UNUSED(enable);
	UNUSED(p_iface_type);
}

u8 rtw_p2p_get_roch_state(void)
{
	return 0;
}

void wifi_hal_p2p_go_addr_cam_set(u8 enable)
{
	UNUSED(enable);
}

void wifi_hal_p2p_scan_addr_cam_set(u8 survey_state)
{
	UNUSED(survey_state);
}

u8 rtw_get_p2p_role(void)
{
	return P2P_ROLE_DISABLE;
}

int rtw_p2p_remain_on_ch(u8 wlan_idx, u8 enable)
{
	UNUSED(wlan_idx);
	UNUSED(enable);
	return 0;
}

#endif
#endif


