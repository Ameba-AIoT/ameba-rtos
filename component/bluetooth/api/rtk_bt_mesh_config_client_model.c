#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdio.h>
#include <stdint.h>
#include <osif.h>

#include <rtk_bt_device.h>
#include <rtk_bt_common.h>
#include <rtk_bt_def.h>

#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_config_client_model.h>

#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
uint16_t rtk_bt_mesh_config_client_add_app_key(rtk_bt_mesh_config_client_add_app_key_t *add_app_key)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_ADD_APP_KEY, add_app_key,
						  sizeof(rtk_bt_mesh_config_client_add_app_key_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_app_bind(rtk_bt_mesh_config_client_model_app_bind_t *model_app_bind)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_MODEL_APP_BIND, model_app_bind,
						  sizeof(rtk_bt_mesh_config_client_model_app_bind_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_sub_add(rtk_bt_mesh_config_client_model_sub_add_t *model_sub_add)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_ADD, model_sub_add,
						  sizeof(rtk_bt_mesh_config_client_model_sub_add_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_default_ttl_get(rtk_bt_mesh_config_client_model_default_ttl_get_t *ttl_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_DEFAULT_TTL_GET, ttl_get,
						  sizeof(rtk_bt_mesh_config_client_model_default_ttl_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_default_ttl_set(rtk_bt_mesh_config_client_model_default_ttl_set_t *ttl_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_DEFAULT_TTL_SET, ttl_set,
						  sizeof(rtk_bt_mesh_config_client_model_default_ttl_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_relay_get(rtk_bt_mesh_config_client_model_relay_get_t *relay_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_RELAY_GET, relay_get,
						  sizeof(rtk_bt_mesh_config_client_model_relay_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_relay_set(rtk_bt_mesh_config_client_model_relay_set_t *relay_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_RELAY_SET, relay_set,
						  sizeof(rtk_bt_mesh_config_client_model_relay_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_net_transmit_get(rtk_bt_mesh_config_client_model_net_transmit_get_t *net_transmit_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_NET_TRANSMIT_GET, net_transmit_get,
						  sizeof(rtk_bt_mesh_config_client_model_net_transmit_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_net_transmit_set(rtk_bt_mesh_config_client_model_net_transmit_set_t *net_transmit_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_NET_TRANSMIT_SET, net_transmit_set,
						  sizeof(rtk_bt_mesh_config_client_model_net_transmit_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_node_reset(rtk_bt_mesh_config_client_model_node_reset_t *node_reset)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_NODE_RESET, node_reset,
						  sizeof(rtk_bt_mesh_config_client_model_node_reset_t));

	return ret;
}

/************************** add remaining configuration client send message ********************************************/

uint16_t rtk_bt_mesh_config_client_model_compo_data_get(rtk_bt_mesh_config_client_model_compo_data_get_t *compo_data_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_COMPO_DATA_GET, compo_data_get,
						  sizeof(rtk_bt_mesh_config_client_model_compo_data_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_beacon_get(rtk_bt_mesh_config_client_model_beacon_get_t *beacon_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_BEACON_GET, beacon_get,
						  sizeof(rtk_bt_mesh_config_client_model_beacon_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_beacon_set(rtk_bt_mesh_config_client_model_beacon_set_t *beacon_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_BEACON_SET, beacon_set,
						  sizeof(rtk_bt_mesh_config_client_model_beacon_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_proxy_get(rtk_bt_mesh_config_client_model_proxy_get_t *proxy_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_PROXY_GET, proxy_get,
						  sizeof(rtk_bt_mesh_config_client_model_proxy_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_proxy_set(rtk_bt_mesh_config_client_model_proxy_set_t *proxy_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_PROXY_SET, proxy_set,
						  sizeof(rtk_bt_mesh_config_client_model_proxy_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_pub_get(rtk_bt_mesh_config_client_model_model_pub_get_t *pub_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_MODEL_PUB_GET, pub_get,
						  sizeof(rtk_bt_mesh_config_client_model_model_pub_get_t));

	return ret;
}
/*********************virtual address question****************************/
uint16_t rtk_bt_mesh_config_client_model_pub_set(rtk_bt_mesh_config_client_model_model_pub_set_t *pub_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_MODEL_PUB_SET, pub_set,
						  sizeof(rtk_bt_mesh_config_client_model_model_pub_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_sub_delete(rtk_bt_mesh_config_client_model_model_sub_delete_t *sub_delete)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_DELETE, sub_delete,
						  sizeof(rtk_bt_mesh_config_client_model_model_sub_delete_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_sub_delete_all(rtk_bt_mesh_config_client_model_model_sub_delete_all_t *sub_delete_all)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_DELETE_ALL, sub_delete_all,
						  sizeof(rtk_bt_mesh_config_client_model_model_sub_delete_all_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_sub_overwrite(rtk_bt_mesh_config_client_model_model_sub_overwrite_t *sub_overwrite)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_OVERWRITE, sub_overwrite,
						  sizeof(rtk_bt_mesh_config_client_model_model_sub_overwrite_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_sub_get(rtk_bt_mesh_config_client_model_model_sub_get_t *sub_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_GET, sub_get,
						  sizeof(rtk_bt_mesh_config_client_model_model_sub_get_t));

	return ret;
}
/****************************virtual address question end*************************************/
uint16_t rtk_bt_mesh_config_client_model_net_key_add(rtk_bt_mesh_config_client_model_net_key_add_t *net_key_add)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_NET_KEY_ADD, net_key_add,
						  sizeof(rtk_bt_mesh_config_client_model_net_key_add_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_net_key_update(rtk_bt_mesh_config_client_model_net_key_update_t *net_key_update)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_NET_KEY_UPDATE, net_key_update,
						  sizeof(rtk_bt_mesh_config_client_model_net_key_update_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_net_key_delete(rtk_bt_mesh_config_client_model_net_key_delete_t *net_key_delete)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_NET_KEY_DELETE, net_key_delete,
						  sizeof(rtk_bt_mesh_config_client_model_net_key_delete_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_net_key_get(rtk_bt_mesh_config_client_model_net_key_get_t *net_key_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_NET_KEY_GET, net_key_get,
						  sizeof(rtk_bt_mesh_config_client_model_net_key_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_app_key_update(rtk_bt_mesh_config_client_model_app_key_update_t *app_key_update)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_APP_KEY_UPDATE, app_key_update,
						  sizeof(rtk_bt_mesh_config_client_model_app_key_update_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_app_key_delete(rtk_bt_mesh_config_client_model_app_key_delete_t *app_key_delete)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_APP_KEY_DELETE, app_key_delete,
						  sizeof(rtk_bt_mesh_config_client_model_app_key_delete_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_app_key_get(rtk_bt_mesh_config_client_model_app_key_get_t *app_key_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_APP_KEY_GET, app_key_get,
						  sizeof(rtk_bt_mesh_config_client_model_app_key_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_node_identity_get(rtk_bt_mesh_config_client_model_node_identity_get_t *node_identity_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_NODE_IDENTITY_GET, node_identity_get,
						  sizeof(rtk_bt_mesh_config_client_model_node_identity_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_node_identity_set(rtk_bt_mesh_config_client_model_node_identity_set_t *node_identity_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_NODE_IDENTITY_SET, node_identity_set,
						  sizeof(rtk_bt_mesh_config_client_model_node_identity_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_app_unbind(rtk_bt_mesh_config_client_model_model_app_unbind_t *model_app_unbind)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_MODEL_APP_UNBIND, model_app_unbind,
						  sizeof(rtk_bt_mesh_config_client_model_model_app_unbind_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_app_get(rtk_bt_mesh_config_client_model_model_app_get_t *model_app_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_MODEL_APP_GET, model_app_get,
						  sizeof(rtk_bt_mesh_config_client_model_model_app_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_frnd_get(rtk_bt_mesh_config_client_model_frnd_get_t *frnd_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_FRND_GET, frnd_get,
						  sizeof(rtk_bt_mesh_config_client_model_frnd_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_frnd_set(rtk_bt_mesh_config_client_model_frnd_set_t *frnd_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_FRND_SET, frnd_set,
						  sizeof(rtk_bt_mesh_config_client_model_frnd_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_lpn_poll_timeout_get(rtk_bt_mesh_config_client_model_lpn_poll_timeout_get_t *lpn_poll_timeout_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_LPN_POLL_TIMEOUT_GET, lpn_poll_timeout_get,
						  sizeof(rtk_bt_mesh_config_client_model_lpn_poll_timeout_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_key_refresh_phase_get(rtk_bt_mesh_config_client_model_key_refresh_phase_get_t *key_refresh_phase_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_KEY_REFRESH_PHASE_GET, key_refresh_phase_get,
						  sizeof(rtk_bt_mesh_config_client_model_key_refresh_phase_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_key_refresh_phase_set(rtk_bt_mesh_config_client_model_key_refresh_phase_set_t *key_refresh_phase_set)
{
	uint16_t ret;

	BT_LOGA("enter rtk_bt_mesh_config_client_model_key_refresh_phase_set \r\n");
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_KEY_REFRESH_PHASE_SET, key_refresh_phase_set,
						  sizeof(rtk_bt_mesh_config_client_model_key_refresh_phase_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_hb_pub_get(rtk_bt_mesh_config_client_model_hb_pub_get_t *hb_pub_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_HB_PUB_GET, hb_pub_get,
						  sizeof(rtk_bt_mesh_config_client_model_hb_pub_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_hb_pub_set(rtk_bt_mesh_config_client_model_hb_pub_set_t *hb_pub_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_HB_PUB_SET, hb_pub_set,
						  sizeof(rtk_bt_mesh_config_client_model_hb_pub_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_hb_sub_get(rtk_bt_mesh_config_client_model_hb_sub_get_t *hb_sub_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_HB_SUB_GET, hb_sub_get,
						  sizeof(rtk_bt_mesh_config_client_model_hb_sub_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_config_client_model_hb_sub_set(rtk_bt_mesh_config_client_model_hb_sub_set_t *hb_sub_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, RTK_BT_MESH_CONFIG_MODEL_HB_SUB_SET, hb_sub_set,
						  sizeof(rtk_bt_mesh_config_client_model_hb_sub_set_t));

	return ret;
}
/***************************************** end ********************************************************/
#endif // end of RTK_BLE_MESH_PROVISIONER_SUPPORT
#endif // end of RTK_BLE_MESH_SUPPORT
