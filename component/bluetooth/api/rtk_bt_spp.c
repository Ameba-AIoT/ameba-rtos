/*
*******************************************************************************
* Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include "platform_autoconf.h"
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <rtk_bt_spp.h>
#include <rtk_bt_def.h>
#include <rtk_bt_device.h>
#include <rtk_bt_common.h>

/* SPP service register config */
uint8_t spp_max_link_num;
uint8_t spp_rfc_max_service_num;
uint8_t default_mtu_size;
uint8_t default_credits;
extern uint16_t bt_stack_spp_src_cfg(uint8_t server_chann, uint8_t *service_uuid, uint16_t uuid_length);

uint16_t rtk_bt_spp_init_cfg(uint8_t max_link_num, uint8_t max_service_num, uint16_t mtu_size, uint8_t credits)
{
	if (rtk_bt_is_enable()) {
		BT_LOGE("[SPP]: rtk_bt_spp_init_cfg should not be invoked after bt enable \r\n");
		return RTK_BT_FAIL;
	}

	spp_max_link_num = max_link_num;
	spp_rfc_max_service_num = max_service_num;
	default_mtu_size = mtu_size;
	default_credits = credits;

	return RTK_BT_OK;
}

uint16_t rtk_bt_service_register_cfg(void *spp_service_class_uuid, uint32_t length, uint8_t local_server_chann)
{
	if (rtk_bt_is_enable()) {
		BT_LOGE("[SPP]: rtk_bt_service_register_cfg should not be invoked after bt enable \r\n");
		return RTK_BT_FAIL;
	}

	return bt_stack_spp_src_cfg(local_server_chann, spp_service_class_uuid, length);
}

uint16_t rtk_bt_spp_connect(uint8_t *bd_addr)
{
	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_BR_GP_SPP, RTK_BT_SPP_ACT_CONNECT, bd_addr, 6);
}

uint16_t rtk_bt_spp_disconnect(rtk_bt_spp_disconn_req_t *p_disconn_req_t)
{
	if (!p_disconn_req_t) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_BR_GP_SPP, RTK_BT_SPP_ACT_DISCONNECT,
						   p_disconn_req_t, sizeof(rtk_bt_spp_disconn_req_t));
}

uint16_t rtk_bt_spp_disconnect_all(uint8_t *bd_addr)
{
	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_BR_GP_SPP, RTK_BT_SPP_ACT_DISCONNECT_ALL, bd_addr, 6);
}

uint16_t rtk_bt_spp_send_data(rtk_bt_spp_send_data_t *p_send_data_t)
{
	if (!p_send_data_t) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_BR_GP_SPP, RTK_BT_SPP_ACT_SEND_DATA,
						   p_send_data_t, sizeof(rtk_bt_spp_send_data_t));
}

uint16_t rtk_bt_spp_credits_give(rtk_bt_spp_credits_give_t *p_credits_give_t)
{
	if (!p_credits_give_t) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_BR_GP_SPP, RTK_BT_SPP_ACT_GIVE_CREDITS,
						   p_credits_give_t, sizeof(rtk_bt_spp_credits_give_t));
}
