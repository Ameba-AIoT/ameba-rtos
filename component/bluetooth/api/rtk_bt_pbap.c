/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <rtk_bt_common.h>
#include <rtk_bt_def.h>
#include <rtk_bt_device.h>
#include <rtk_bt_pbap.h>

uint16_t rtk_bt_pbap_connect(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_PBAP, RTK_BT_PBAP_ACT_CONNECT,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_pbap_disconnect(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_PBAP, RTK_BT_PBAP_ACT_DISCONNECT,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_pbap_set_phone_book(rtk_bt_pbap_set_pb_t *p_set_pb_t)
{
	uint16_t ret = 0;

	if (!p_set_pb_t) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_PBAP, RTK_BT_PBAP_ACT_SET_PHONE_BOOK,
						  p_set_pb_t, sizeof(rtk_bt_pbap_set_pb_t));

	return ret;
}

uint16_t rtk_bt_pbap_phone_book_pull(rtk_bt_pbap_pb_pull_t *p_pb_pull_t)
{
	uint16_t ret = 0;

	if (!p_pb_pull_t) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_PBAP, RTK_BT_PBAP_ACT_PHONE_BOOK_PULL,
						  p_pb_pull_t, sizeof(rtk_bt_pbap_pb_pull_t));

	return ret;
}

uint16_t rtk_bt_pbap_phone_book_pull_continue(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_PBAP, RTK_BT_PBAP_ACT_PHONE_BOOK_PULL_CONTINUE,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_pbap_phone_book_size_get(rtk_bt_pbap_pb_size_get_t *p_pb_size_t)
{
	uint16_t ret = 0;

	if (!p_pb_size_t) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_PBAP, RTK_BT_PBAP_ACT_PHONE_BOOK_SIZE_GET,
						  p_pb_size_t, sizeof(rtk_bt_pbap_pb_size_get_t));

	return ret;
}

uint16_t rtk_bt_pbap_pull_caller_id_name(rtk_bt_pbap_pull_caller_id_name_t *p_pull_caller_id_name_t)
{
	uint16_t ret = 0;

	if (!p_pull_caller_id_name_t) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_PBAP, RTK_BT_PBAP_ACT_PULL_CALLER_ID_NAME,
						  p_pull_caller_id_name_t, sizeof(rtk_bt_pbap_pull_caller_id_name_t));

	return ret;
}