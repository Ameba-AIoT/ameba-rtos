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
#define __WIFI_FEATURE_DIS_MCC_C__

#include "rtw_wifi_constants.h"

#ifdef CONFIG_MCC_MODE
void rtw_mccapi_port0_sitesurvey_start(void)
{

}

void rtw_mccapi_port0_sitesurvey_complete(void)
{

}

void rtw_mccapi_port0_leavebss(void)
{

}

int rtw_mccapi_port0_joinbss_chk_done_concurrent(u8 sta_chan, int join_res)
{
	UNUSED(sta_chan);
	UNUSED(join_res);
	return FAIL;
}

void rtw_mccapi_port1_add_sta(u8 mac_id, u8 add)
{
	UNUSED(mac_id);
	UNUSED(add);
}

void rtw_mccapi_port1_start_bss_mcc_start(u8 ap_chan)
{
	UNUSED(ap_chan);
}

void rtw_mccapi_port1_stop_bss_network(void)
{

}

void rtw_mcc_drv_switch_channel(u8 mcc_order, u8 ch)
{
	UNUSED(mcc_order);
	UNUSED(ch);
}

void wifi_hal_mccapi_port1_bcn_early_hdl(void)
{

}

u8 *rtw_mcc_append_vendor_ie(u8 *pbuf, u32 *frlen, u8 *pbuf_end)
{
	UNUSED(frlen);
	UNUSED(pbuf_end);
	return pbuf;
}

int rtw_mcc_parse_vendor_ie(u8 *pframe, u32 pkt_len)
{
	UNUSED(pframe);
	UNUSED(pkt_len);
	return FAIL;
}

u8 rtw_mcc_filter_mgnt_rx_by_port(u8 iface_type, u8 subtype)
{
	UNUSED(iface_type);
	UNUSED(subtype);
	return FALSE;
}

/* NAN Realted */
void rtw_mccapi_swch_port2_add_sta(u8 mac_id, u8 add)
{
	UNUSED(mac_id);
	UNUSED(add);
}

u8 rtw_mccapi_swch_port2_runaway(u8 next_slot)
{
	UNUSED(next_slot);
	return 0;
}

u8 rtw_mccapi_swch_port2_comeback_early(u8 b_p2_tbtt, u8 nan_occupied_time_tu)
{
	UNUSED(b_p2_tbtt);
	UNUSED(nan_occupied_time_tu);
	return 0;
}

u8 rtw_mccapi_swch_port2_comeback(u8 nan_chan, u8 nan_bw40_sc, u8 nan_bw)
{
	UNUSED(nan_chan);
	UNUSED(nan_bw40_sc);
	UNUSED(nan_bw);
	return 0;
}

/* P2P Related */
void rtw_p2p_go_noa_update(u8 type, u32 p1_tbtt_before, u32 ap_active_dur)
{
	UNUSED(type);
	UNUSED(p1_tbtt_before);
	UNUSED(ap_active_dur);
}
#endif /* CONFIG_MCC_MODE */

