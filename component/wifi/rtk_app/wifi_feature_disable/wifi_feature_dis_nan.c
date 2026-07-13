/**
  ******************************************************************************
  * @file    wifi_feature_dis_nan.c
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
#ifndef __WIFI_FEATURE_DIS_NAN_C__
#define __WIFI_FEATURE_DIS_NAN_C__

#include "wifi_intf_drv_to_app_internal.h"
#include "wifi_api_types.h"

#ifndef CONFIG_WIFI_NAN_ENABLE

u8 whc_ipc_dev_nan_api_hdl(void *p_ipc_msg)
{
	UNUSED(p_ipc_msg);
	return FALSE;
}

void rtw_nan_init(void)
{
}

void rtw_nan_deinit(void)
{
}

int rtw_nan_intfs_init(void)
{
	return RTK_FAIL;
}

void rtw_nan_intfs_deinit(void)
{
}

void rtw_nan_start_api(u8 master_pref, u8 band_support, u8 nan_cmd_type)
{
	UNUSED(master_pref);
	UNUSED(band_support);
	UNUSED(nan_cmd_type);
}

void rtw_nan_stop_api(void)
{
}

int rtw_nan_func_add(void *func_param, u64 nan_func_pointer)
{
	UNUSED(func_param);
	UNUSED(nan_func_pointer);
	return RTK_FAIL;
}

int rtw_nan_func_del(u64 cookie)
{
	UNUSED(cookie);
	return RTK_FAIL;
}

void rtw_cfgvendor_cmd_process(u16 vendor_cmd, void *data, u32 len)
{
	UNUSED(vendor_cmd);
	UNUSED(data);
	UNUSED(len);
}

u8 rtw_is_nan_frame(void *precv_frame)
{
	UNUSED(precv_frame);
	return FALSE;
}

int rtw_nan_on_action_public(void *precv_frame)
{
	UNUSED(precv_frame);
	return RTK_FAIL;
}

int rtw_nan_bcn_rx(void *precv_frame)
{
	UNUSED(precv_frame);
	return RTK_FAIL;
}

int rtw_check_nan_exist(void)
{
	return FALSE;
}

int rtw_check_nan_enabled(void)
{
	return FALSE;
}

int rtw_check_nan_datalink_exist(void)
{
	return FALSE;
}

void rtw_macid_nan_mgt_init(void *pmacid_mgt)
{
	UNUSED(pmacid_mgt);
}
struct nan_peer_info_t *rtw_nan_peer_info_get_by_ndi(u8 *mac_addr)
{
	UNUSED(mac_addr);
	return NULL;
}

struct nan_peer_info_t *rtw_nan_peer_info_first_entry_get(u8 is_ndp_exist)
{
	UNUSED(is_ndp_exist);
	return NULL;
}

void wifi_hal_nan_interrupt_handle(void)
{
}

void wifi_hal_nan_beacon_buffer_update(u8 alloc, u8 nan_bcn_type)
{
	UNUSED(alloc);
	UNUSED(nan_bcn_type);
}

void wifi_hal_nan_update_txdesc(void *pxmitframe, u8 *pmem)
{
	UNUSED(pxmitframe);
	UNUSED(pmem);
}

void wifi_hal_nan_c2h_hdl(u8 *payload, u8 plen)
{
	UNUSED(payload);
	UNUSED(plen);
}

u8 rtw_nan_pasn_rx_checking(u8 *peer_nmi, u8 *auth_frame, u16 auth_frame_len)
{
	UNUSED(peer_nmi);
	UNUSED(auth_frame);
	UNUSED(auth_frame_len);
	return FALSE;
}

void wifi_hal_nan_bcn_tx_start(u8 bcnType)
{
	UNUSED(bcnType);
}

void rtw_nan_pasn_tx_mgmt_revise(const u8 *buf)
{
	UNUSED(buf);
}

void rtw_xmit_make_wlanhdr_nanaddr(void	*pwlanhdr, void *pattrib)
{
	UNUSED(pwlanhdr);
	UNUSED(pattrib);
}

u8 rtw_nan_schdlr_set_pause_schedule(u8 iface_type, bool bpause)
{
	UNUSED(iface_type);
	UNUSED(bpause);
	return FALSE;
}

void rtw_nan_schdlr_ap_set(u8 iface_type, u8 status)
{
	UNUSED(iface_type);
	UNUSED(status);
}

void rtw_nan_schdlr_sta_set(u8 iface_type, bool bconnect)
{
	UNUSED(iface_type);
	UNUSED(bconnect);
}

void rtw_nan_channel_switch_pending_flush(void)
{
}

void rtw_nan_linked_status_chk(void)
{
}

void wififw_inthdl_dwearly(void)
{
}

void wififw_inthdl_dwstart(void)
{
}
#endif
#endif
