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
#define __WIFI_FEATURE_DIS_WTN_C__

#include "wifi_api_types.h"

#ifdef CONFIG_WIFI_TUNNEL
#ifndef CONFIG_WHC_HOST
#ifndef CONFIG_RMESH_EN

void whc_ipc_dev_wtn_api_hdl(void *p_ipc_msg)
{
	UNUSED(p_ipc_msg);
}

void wifi_tunnelapi_rnat_softap_bcnie_record(u8 *IEs, u32 ie_len)
{
	UNUSED(IEs);
	UNUSED(ie_len);
}

void wifi_tunnelapi_pending_q_resume(void)
{

}

int wifi_tunnelapi_dynamic_check(void *psta_mlmepriv)
{
	UNUSED(psta_mlmepriv);

	return 0;
}

void wifi_tunnelapi_joinbss_done_indicate(u8 iface_type)
{
	UNUSED(iface_type);
}

int wifi_tunnelapi_joinbss_continue_after_pair_success(void)
{
	return RTK_FAIL;
}

u32 wifi_tunnelapi_joinbss_set_current_network_by_joincmd(void *pcur_network)
{
	UNUSED(pcur_network);

	return 0;
}

void wifi_tunnelapi_joinbss_set_target_network_by_joinscan(void *ptarget_network)
{
	UNUSED(ptarget_network);
}

int wifi_tunnelapi_joinbss_check_helperlist_ready(u8 ap_channel, int ap_rssi)
{
	UNUSED(ap_channel);
	UNUSED(ap_rssi);

	return RTK_SUCCESS;
}

void wifi_tunnelapi_joinbss_set_network_by_user(u8 *user_bssid, u8 *user_ssid)
{
	UNUSED(user_bssid);
	UNUSED(user_ssid);
}

void wifi_tunnelapi_leavebss_indicate(u16 disassoc_reason)
{
	UNUSED(disassoc_reason);
}

void wifi_tunnelapi_on_beacon(void *precv_frame)
{
	UNUSED(precv_frame);
}

int wifi_tunnelapi_rnat_wtn_bcn_bypass(void *precv_frame)
{
	UNUSED(precv_frame);

	return RTK_FAIL;
}

void wifi_tunnelapi_setkey(void *pparm)
{
	UNUSED(pparm);
}

void wifi_tunnelapi_scan_original_rssi_record(void *new_network, u8 is_wtn_bcn)
{
	UNUSED(new_network);
	UNUSED(is_wtn_bcn);
}

void wifi_tunnel_issue_helpprobe(void)
{

}

void hal_wifi_tunnel_prepare_rx_zrpp_hs(u8 channel)
{
	UNUSED(channel);
}

void wifi_tunnel_zrpp_issue_hs_req(u8 *requestor)
{
	UNUSED(requestor);
}

void wifi_tunnelapi_refugee_get_rssi_min(u8 *p_odm_rssi_min, u8 *p_odm_pre_rssi_min)
{
	UNUSED(p_odm_rssi_min);
	UNUSED(p_odm_pre_rssi_min);
}

void wifi_tunnel_beacon_partial_update_tim(void)
{

}

void wifi_tunnelapi_helpie_update_tohw(void)
{

}

void wtn_ps_p1_bcn_interrupt_hdl(void)
{

}

int wifi_tunnelapi_rx_process(u8 *rx_desc, void *precvframe)
{
	UNUSED(rx_desc);
	UNUSED(precvframe);

	return RTK_FAIL;
}

void wifi_tunnelapi_sw_init(void)
{

}

void wifi_tunnelapi_sw_deinit(void)
{

}

void wifi_tunnelapi_update_txdata_macswap(void *ptxdesc, u8 *pframe, u8 sc)
{
	UNUSED(ptxdesc);
	UNUSED(pframe);
	UNUSED(sc);
}

void wifi_tunnel_command(char *cmd_ptr, u32 extra_arg)
{
	UNUSED(cmd_ptr);
	UNUSED(extra_arg);
}

void wifi_tunnelapi_change_txbd_num(void)
{

}
#endif /* CONFIG_RMESH_EN */

#ifndef CONFIG_RMESH_SOCKET_EN
void wifi_tunnel_socket_normal_rpt_send(void)
{

}

void whc_ipc_dev_wtn_socket_init(u8 enable, u8 rnat_ap_start)
{
	UNUSED(enable);
	UNUSED(rnat_ap_start);
}
#endif /* CONFIG_RMESH_SOCKET_EN */

#ifndef CONFIG_RMESH_OTA_EN

void whc_ipc_dev_wtn_ota_api_hdl(void *p_ipc_msg)
{
	UNUSED(p_ipc_msg);
}

int whc_ipc_dev_wtn_ota_callback_indicate(u8 *buf, u16 len)
{
	UNUSED(buf);
	UNUSED(len);

	return RTK_FAIL;
}

int wifi_tunnel_rnat_rx_ota_process(u8 *pframe, u16 pkt_len)
{
	UNUSED(pframe);
	UNUSED(pkt_len);

	return RTK_FAIL;
}
#endif /* CONFIG_RMESH_OTA_EN */
#else
#ifndef CONFIG_RMESH_EN
int whc_ipc_host_api_wtn_identity_key_calc(u8 *password, u32 password_len)
{
	UNUSED(password);
	UNUSED(password_len);

	return -1;
}
#endif /* CONFIG_RMESH_OTA_EN */
#endif /* CONFIG_WHC_HOST */
#endif /* CONFIG_WIFI_TUNNEL */