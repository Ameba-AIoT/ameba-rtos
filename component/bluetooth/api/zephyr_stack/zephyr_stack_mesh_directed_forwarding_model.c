/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <dlist.h>
#include <osif.h>

#include <bt_app_config.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_mesh_def.h>

#include <zephyr_stack_api.h>
#include <zephyr_stack_internal.h>

#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
uint16_t bt_mesh_directed_forwarding_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	BT_LOGE("[%s] Error:This BLE MESH feature is not supported now\r\n", __func__);
	uint16_t ret = RTK_BT_ERR_UNSUPPORTED;
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}
#endif

#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING) && BT_MESH_ENABLE_DIRECTED_FORWARDING
uint16_t bt_mesh_directed_forwarding_common_act_handle(rtk_bt_cmd_t *p_cmd)
{
	BT_LOGE("[%s] Error:This BLE MESH feature is not supported now\r\n", __func__);
	uint16_t ret = RTK_BT_ERR_UNSUPPORTED;
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}
#endif