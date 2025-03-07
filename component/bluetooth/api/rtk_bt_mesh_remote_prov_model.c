#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT

#include <stdio.h>
#include <stdint.h>

#include <rtk_bt_def.h>
#include <rtk_bt_device.h>
#include <rtk_bt_common.h>
#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_remote_prov_model.h>

#if defined(BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL) && BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL
uint16_t rtk_bt_mesh_remote_prov_scan_start(rtk_bt_mesh_remote_prov_client_scan_start_t *rmt_scan)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL, RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_SCAN_START, rmt_scan,
						  sizeof(rtk_bt_mesh_remote_prov_client_scan_start_t));

	return ret;
}

uint16_t rtk_bt_mesh_remote_prov_scan_capa_get(rtk_bt_mesh_remote_prov_client_scan_capa_get_t *scan_capa_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL, RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_SCAN_CAPA_GET, scan_capa_get,
						  sizeof(rtk_bt_mesh_remote_prov_client_scan_capa_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_remote_prov_link_open(rtk_bt_mesh_remote_prov_client_link_open_t *link_open)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL, RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_LINK_OPEN, link_open,
						  sizeof(rtk_bt_mesh_remote_prov_client_link_open_t));

	return ret;
}

uint16_t rtk_bt_mesh_remote_prov_link_open_dkri(rtk_bt_mesh_remote_prov_client_link_open_dkri_t *link_open_dkri)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL, RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_LINK_OPEN_DKRI, link_open_dkri,
						  sizeof(rtk_bt_mesh_remote_prov_client_link_open_dkri_t));

	return ret;
}

uint16_t rtk_bt_mesh_remote_prov_dev_key_refresh(uint8_t *attn_dur)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL, RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_DEVICE_KEY_REFRESH, attn_dur,
						  sizeof(uint8_t));

	return ret;
}

uint16_t rtk_bt_mesh_remote_prov_node_addr_refresh(rtk_bt_mesh_remote_prov_client_node_addr_refresh_t *node_refresh)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL, RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_NODE_ADDR_REFRESH, node_refresh,
						  sizeof(rtk_bt_mesh_remote_prov_client_node_addr_refresh_t));

	return ret;
}

uint16_t rtk_bt_mesh_remote_prov_composition_refresh(uint8_t *attn_dur)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL, RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_COMPO_DATA_REFRESH, attn_dur,
						  sizeof(uint8_t));

	return ret;
}

#endif // end of BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL
#endif // end of RTK_BLE_MESH_SUPPORT
