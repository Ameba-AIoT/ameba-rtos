/**
  ******************************************************************************
  * @file    wifi_feature_dis_p2p.c
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
#ifndef __WIFI_FEATURE_DIS_P2P_C__
#define __WIFI_FEATURE_DIS_P2P_C__

#include "wifi_intf_drv_to_app_internal.h"

#ifdef CONFIG_P2P
void wifi_hal_p2p_addr_cam_init(void)
{
}

void rtw_p2p_init(void)
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

u8 *rtw_p2p_set_bcn_p2p_ie(u8 *pframe, u32 *pktlen)
{
	UNUSED(pframe);
	UNUSED(pktlen);
	return pframe;
}

u8 *rtw_p2p_set_probersp_p2p_ie(u8 *pframe, u32 *pktlen)
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

int rtw_p2p_probereq_indicate(u8 *pframe, u32 len, u8 iface_type, u8 *is_valid_p2p_probereq)
{
	UNUSED(pframe);
	UNUSED(len);
	UNUSED(iface_type);
	UNUSED(is_valid_p2p_probereq);
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

u8 whc_ipc_dev_p2p_api_hdl(void *p_ipc_msg)
{
	UNUSED(p_ipc_msg);
	return FALSE;
}
#endif
#endif
