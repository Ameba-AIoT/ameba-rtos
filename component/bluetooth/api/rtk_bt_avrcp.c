/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <string.h>
#include <osif.h>
#include <rtk_bt_common.h>
#include <rtk_bt_def.h>
#include <rtk_bt_device.h>
#include <rtk_bt_avrcp.h>

uint16_t rtk_bt_avrcp_connect(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_CONNECT,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_avrcp_disconnect(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_DISCONNECT,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_avrcp_play(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_PLAY,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_avrcp_pause(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_PAUSE,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_avrcp_stop(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_STOP,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_avrcp_forward(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_FORWARD,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_avrcp_backward(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_BACKWARD,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_avrcp_rewind_start(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_REWIND_START,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_avrcp_rewind_stop(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_REWIND_STOP,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_avrcp_fast_forward_start(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_FAST_FORWARD_START,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_avrcp_fast_forward_stop(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_FAST_FORWARD_STOP,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_avrcp_absolute_volume_set(uint8_t *bd_addr, uint8_t volume)
{
	uint16_t ret = 0;
	rtk_bt_avrcp_absolute_volume_set_t volume_t = {0};

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	memcpy((void *)&volume_t.bd_addr, (void *)bd_addr, 6);
	volume_t.volume = volume;

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_ABSOLUTE_VOLUME_SET,
						  (void *)&volume_t, sizeof(rtk_bt_avrcp_absolute_volume_set_t));

	return ret;
}

uint16_t rtk_bt_avrcp_volume_change_req(uint8_t *bd_addr, uint8_t volume)
{
	uint16_t ret = 0;
	rtk_bt_avrcp_volume_change_req_t volume_t = {0};

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	memcpy((void *)&volume_t.bd_addr, (void *)bd_addr, 6);
	volume_t.volume = volume;

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_VOLUME_CHANGE_REQ,
						  (void *)&volume_t, sizeof(rtk_bt_avrcp_volume_change_req_t));

	return ret;
}

uint16_t rtk_bt_avrcp_get_element_attr(uint8_t *bd_addr, uint8_t num, uint8_t *p_attr)
{
	uint16_t ret = 0;
	rtk_bt_avrcp_get_element_attr_req_t req_t = {0};

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}
	memcpy((void *)&req_t.bd_addr, (void *)bd_addr, 6);
	req_t.num = num;
	req_t.p_attr = p_attr;

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_GET_ELEMENT_ATTR,
						  (void *)&req_t, sizeof(rtk_bt_avrcp_get_element_attr_req_t));

	return ret;
}

uint16_t rtk_bt_avrcp_cover_art_connect(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_CONN_COVER_ART,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_avrcp_app_setting_attrs_list(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_APP_SETTING_ATTRS_LIST,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_avrcp_app_setting_values_list(uint8_t *bd_addr, uint8_t attr_id)
{
	uint16_t ret = 0;
	rtk_bt_avrcp_app_setting_values_t values = {0};

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}
	memcpy((void *)&values.bd_addr, (void *)bd_addr, 6);
	values.attr_id = attr_id;

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_APP_SETTING_VALUES_LIST,
						  (void *)&values, sizeof(rtk_bt_avrcp_app_setting_values_t));

	return ret;
}

uint16_t rtk_bt_avrcp_app_setting_value_get(uint8_t *bd_addr, uint8_t attr_num, uint8_t *attr_list)
{
	uint16_t ret = 0;
	rtk_bt_avrcp_app_setting_value_get_t get = {0};

	if (!bd_addr || !attr_list) {
		return RTK_BT_ERR_POINTER_INVALID;
	}
	memcpy((void *)&get.bd_addr, (void *)bd_addr, 6);
	get.attr_num = attr_num;
	get.attr_list = attr_list;

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_APP_SETTING_VALUE_GET,
						  (void *)&get, sizeof(rtk_bt_avrcp_app_setting_value_get_t));

	return ret;
}

uint16_t rtk_bt_avrcp_app_setting_value_set(uint8_t *bd_addr, uint8_t attr_id, uint8_t setting)
{
	uint16_t ret = 0;
	rtk_bt_avrcp_app_setting_value_set_t set = {0};

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}
	memcpy((void *)&set.bd_addr, (void *)bd_addr, 6);
	set.attr_id = attr_id;
	set.setting = setting;

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_ACT_APP_SETTING_VALUE_SET,
						  (void *)&set, sizeof(rtk_bt_avrcp_app_setting_value_set_t));

	return ret;
}