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
#include <rtk_bt_a2dp.h>

extern rtk_bt_a2dp_media_codec_sbc_t rtk_codec_sbc;
extern rtk_bt_a2dp_media_codec_aac_t rtk_codec_aac;

uint16_t rtk_bt_a2dp_codec_cfg(uint8_t codec_type, void *codec_t, uint32_t length)
{
	/* Check bt enable status (rtk_bt_a2dp_codec_cfg should only be invoked before RTK_BT_IS_ENABLED()) */
	if (rtk_bt_is_enable()) {
		BT_LOGE("[A2DP]: rtk_bt_a2dp_codec_cfg should not be invoked after bt enable \r\n");
		return RTK_BT_FAIL;
	}

	switch (codec_type) {
	case RTK_BT_AUDIO_CODEC_SBC: {
		memcpy((void *)&rtk_codec_sbc, codec_t, length);
	}
	break;

	case RTK_BT_AUDIO_CODEC_AAC: {
		memcpy((void *)&rtk_codec_aac, codec_t, length);
	}
	break;

	default : {
		BT_LOGE("[A2DP]: Unknown codec type %d \r\n", codec_type);
	}
	break;
	}

	return RTK_BT_OK;
}

uint16_t rtk_bt_a2dp_connect(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_A2DP, RTK_BT_A2DP_ACT_CONNECT,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_a2dp_disconnect(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_A2DP, RTK_BT_A2DP_ACT_DISCONNECT,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_a2dp_start(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_A2DP, RTK_BT_A2DP_ACT_START,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_a2dp_suspend(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_A2DP, RTK_BT_A2DP_ACT_SUSPEND,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_a2dp_data_send(rtk_bt_a2dp_stream_data_send_t *p_data_send_t)
{
	uint16_t ret = 0;

	if (!p_data_send_t) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_A2DP, RTK_BT_A2DP_ACT_SEND_DATA,
						  p_data_send_t, sizeof(rtk_bt_a2dp_stream_data_send_t));

	return ret;
}
