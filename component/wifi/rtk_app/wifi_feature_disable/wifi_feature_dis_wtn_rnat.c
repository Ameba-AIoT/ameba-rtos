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

#if defined(CONFIG_WIFI_TUNNEL) && defined(CONFIG_WTN_RNAT_INTERCOM) && !defined(CONFIG_WHC_HOST) && !defined(CONFIG_RNAT_INTERCOM_EN)

/* signatures must match the __weak ones in rtw_wtn_rnat_intercom.c / whc_ipc_dev_api.c */
union recv_frame;
struct rtw_crypt_info;
struct pkt_attrib;
struct txdesc_priv;
struct wtn_rnat_neighbor;
struct whc_ipc_host_req_msg;

void wtn_rnat_intercom_init(void)
{

}

void wtn_rnat_intercom_deinit(void)
{

}

/* RTK_FAIL: the caller handles the frame/pkt as without intercom */
int wtn_rnat_intercom_rx_process(union recv_frame *precv_frame)
{
	UNUSED(precv_frame);

	return RTK_FAIL;
}

int wtn_rnat_intercom_on_ip_pkt_tx(u8 *eth, u16 eth_len, u8 tx_raw)
{
	UNUSED(eth);
	UNUSED(eth_len);
	UNUSED(tx_raw);

	return RTK_FAIL;
}

int wtn_rnat_intercom_key_process(u8 iface_type, struct rtw_crypt_info *crypt)
{
	UNUSED(iface_type);
	UNUSED(crypt);

	return RTK_FAIL;
}

void wtn_rnat_intercom_gtk_restore(void)
{

}

void wtn_rnat_intercom_iv_set(u8 *pframe, u8 device_id, u8 iface_type, struct pkt_attrib *pattrib)
{
	UNUSED(pframe);
	UNUSED(device_id);
	UNUSED(iface_type);
	UNUSED(pattrib);
}

void wtn_rnat_intercom_enc_process(struct txdesc_priv *ptxdesc, struct pkt_attrib *pattrib)
{
	UNUSED(ptxdesc);
	UNUSED(pattrib);
}

u8 wtn_rnat_intercom_get_self_neighbor_info(struct wtn_rnat_neighbor *out, u8 max_count)
{
	UNUSED(out);
	UNUSED(max_count);

	return 0;
}

void wtn_rnat_intercom_set_gw(const u8 *gw)
{
	UNUSED(gw);
}

void wtn_rnat_intercom_set_client_ip(const u8 *mac, u8 ip_byte4)
{
	UNUSED(mac);
	UNUSED(ip_byte4);
}

#if defined(CONFIG_WHC_DEV) && defined(CONFIG_WHC_INTF_IPC)
/* FALSE: the ipc msg is not handled here */
u8 whc_ipc_dev_rmesh_rnat_api_hdl(struct whc_ipc_host_req_msg *p_ipc_msg)
{
	UNUSED(p_ipc_msg);

	return FALSE;
}
#endif

#endif /* CONFIG_RNAT_INTERCOM_EN */
