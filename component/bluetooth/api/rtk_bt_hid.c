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
#include <rtk_bt_hid.h>

extern uint16_t bt_stack_hid_descriptor_add(void *des, uint32_t length);

uint16_t rtk_hid_descriptor_add(void *des, uint32_t length)
{
	/* sdp record add should be invoked before bt enable */
	if (rtk_bt_is_enable()) {
		return RTK_BT_ERR_NOT_READY;
	}
	/* Check validity */
	if (!des) {
		BT_LOGE("rtk_hid_descriptor_add: descriptor is NULL \r\n");
		return RTK_BT_ERR_NO_PERM;
	}
	/* length check */
	if (length > RTK_BT_HID_MAX_DESCRIPTOR_LENGTH) {
		BT_LOGE("rtk_hid_descriptor_add: HID des length %d is over max descriptor memory \r\n", (int)length);
		return RTK_BT_FAIL;
	}

	return bt_stack_hid_descriptor_add(des, length);
}

uint16_t rtk_bt_hid_disconnect(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_HID, RTK_BT_HID_ACT_DISCONNECT,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_hid_get_report_rsp(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_HID, RTK_BT_HID_ACT_GET_REPORT_RSP,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_hid_input_data_send(uint8_t *bd_addr, uint8_t *data, uint32_t len)
{
	uint16_t ret = 0;
	rtk_bt_hid_input_data_t data_t = {0};

	if (!bd_addr || !data) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	if (len > RTK_BT_HID_MAX_INPUT_DATA_LENGTH) {
		BT_LOGE("%s: input data length is too large \r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	memcpy((void *)data_t.bd_addr, bd_addr, 6);
	memcpy((void *)data_t.data, data, len);
	data_t.len = len;

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_HID, RTK_BT_HID_ACT_INPUT_DATA_SEND,
						  &data_t, sizeof(rtk_bt_hid_input_data_t));

	return ret;
}