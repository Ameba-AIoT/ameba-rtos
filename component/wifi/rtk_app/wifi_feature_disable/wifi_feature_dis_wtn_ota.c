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
#define __WIFI_FEATURE_DIS_WTN_OTA_C__

#include "wifi_api_types.h"
#include "whc_ipc.h"

#ifdef CONFIG_WIFI_TUNNEL
void wifi_rmesh_register_ota_callback(int (*ota_callback)(u8 *buf, u16 len))
{
	UNUSED(ota_callback);

	return;
}

int wifi_rmesh_get_node_info(enum rtw_rmesh_node_type type, struct rtw_rmesh_node_info *node_info)
{
	UNUSED(type);
	UNUSED(node_info);

	return RTK_FAIL;
}

u8 wifi_rmesh_get_child_num(void)
{
	return 0;
}

int wifi_rmesh_get_child_info_list(u8 *child_num, struct rtw_rmesh_node_info *child_info_list)
{
	UNUSED(child_num);
	UNUSED(child_info_list);

	return RTK_FAIL;
}

int wifi_rmesh_update_node_ota_ver(u8 *ota_ver, u8 ota_ver_len, u8 ota_ongoing)
{
	UNUSED(ota_ver);
	UNUSED(ota_ver_len);
	UNUSED(ota_ongoing);

	return RTK_FAIL;
}

int wifi_rmesh_check_node_ota_ver(u8 *ota_ver, u8 ota_ver_len, enum rtw_rmesh_node_type node_type)
{
	UNUSED(ota_ver);
	UNUSED(ota_ver_len);
	UNUSED(node_type);

	return RTK_FAIL;
}

void whc_ipc_dev_wtn_api_hdl(struct whc_ipc_host_req_msg *p_ipc_msg)
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

#endif /* CONFIG_WIFI_TUNNEL */
