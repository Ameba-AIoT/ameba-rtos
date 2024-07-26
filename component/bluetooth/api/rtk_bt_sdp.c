/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <stdio.h>
#include <osif.h>
#include <rtk_bt_common.h>
#include <rtk_bt_def.h>
#include <rtk_bt_device.h>
#include <rtk_bt_sdp.h>

extern uint16_t bt_stack_sdp_record_add(void *record, uint32_t length);

uint16_t rtk_sdp_record_add(void *record, uint32_t length)
{
	/* sdp record add should be invoked before bt enable */
	if (rtk_bt_is_enable()) {
		return RTK_BT_ERR_NOT_READY;
	}
	/* Check validity */
	if (!record) {
		BT_LOGE("rtk_sdp_record_add: record is NULL \r\n");
		return RTK_BT_ERR_NO_PERM;
	}
	/* length check */
	if (length > RTK_BT_SDK_MAX_RECORD_LENGTH) {
		BT_LOGE("rtk_sdp_record_add: SDP record length %d is over max record memory \r\n", (int)length);
		return RTK_BT_FAIL;
	}

	return bt_stack_sdp_record_add(record, length);
}


uint16_t rtk_bt_sdp_discov_start(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_SDP, RTK_BT_SDP_ACT_DISCOV_START,
						  bd_addr, 6);

	return ret;
}