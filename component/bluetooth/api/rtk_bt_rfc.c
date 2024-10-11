/*
*******************************************************************************
* Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include "platform_autoconf.h"
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <rtk_bt_rfc.h>
#include <rtk_bt_def.h>
#include <rtk_bt_device.h>
#include <rtk_bt_common.h>

uint16_t rtk_bt_rfc_connect(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_RFC, RTK_BT_RFC_ACT_CONNECT,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_rfc_disconnect(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_RFC, RTK_BT_RFC_ACT_DISCONNECT,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_rfc_data_send(rtk_bt_rfc_data_send_t *p_data_send_t)
{
	uint16_t ret = 0;

	if (!p_data_send_t) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_RFC, RTK_BT_RFC_ACT_SEND_DATA,
						  p_data_send_t, sizeof(rtk_bt_rfc_data_send_t));

	return ret;
}

uint16_t rtk_bt_rfc_credits_give(rtk_bt_rfc_credits_give_t *p_credits_give_t)
{
	uint16_t ret = 0;

	if (!p_credits_give_t) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_RFC, RTK_BT_RFC_ACT_CREDITS_GIVE,
						  p_credits_give_t, sizeof(rtk_bt_rfc_credits_give_t));

	return ret;
}
