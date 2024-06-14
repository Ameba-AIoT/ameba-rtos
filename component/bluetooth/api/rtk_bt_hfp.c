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
#include <rtk_bt_hfp.h>

extern rtk_bt_hfp_ag_conf_t ag_conf;
extern rtk_bt_hfp_hf_conf_t hf_conf;

uint16_t rtk_bt_hfp_cfg(uint8_t role, void *p_conf_t)
{
	/* Check bt enable status (rtk_bt_hfp_cfg should only be invoked before RTK_BT_IS_ENABLED()) */
	if (rtk_bt_is_enable()) {
		BT_LOGE("[HFP]: rtk_bt_hfp_cfg should not be invoked after bt enable \r\n");
		return RTK_BT_FAIL;
	}
	if (!p_conf_t) {
		BT_LOGE("[HFP]: p_conf_t is NULL \r\n");
		return RTK_BT_FAIL;
	}

	switch (role) {
	case RTK_BT_AUDIO_HFP_ROLE_AG: {
		memcpy((void *)&ag_conf, p_conf_t, sizeof(rtk_bt_hfp_ag_conf_t));
	}
	break;

	case RTK_BT_AUDIO_HFP_ROLE_HF: {
		memcpy((void *)&hf_conf, p_conf_t, sizeof(rtk_bt_hfp_hf_conf_t));
	}
	break;

	default : {
		BT_LOGE("[HFP]: Unknown role %d \r\n", role);
	}
	break;
	}

	return RTK_BT_OK;
}

uint16_t rtk_bt_hfp_connect(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_HFP, RTK_BT_HFP_ACT_CONNECT,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_hfp_disconnect(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_HFP, RTK_BT_HFP_ACT_DISCONNECT,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_hfp_sco_connect(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_HFP, RTK_BT_HFP_ACT_SCO_CONNECT,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_hfp_sco_disconnect(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_HFP, RTK_BT_HFP_ACT_SCO_DISCONNECT,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_hfp_call_incoming(uint8_t *bd_addr, const char *call_num, uint8_t call_num_len, uint8_t call_num_type)
{
	uint16_t ret = 0;
	rtk_bt_hfp_call_incoming_t call_incoming_t = {0};

	if (!bd_addr || !call_num) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	/* call_num_len should contain strlen of call number + 1('\0') */
	if (call_num_len > 20) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	memcpy((void *)&call_incoming_t.bd_addr, bd_addr, 6);
	memcpy((void *)call_incoming_t.call_num, call_num, call_num_len);
	call_incoming_t.call_num_len = call_num_len;
	call_incoming_t.call_num_type = call_num_type;

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_HFP, RTK_BT_HFP_ACT_CALL_INCOMING,
						  (uint8_t *)&call_incoming_t, sizeof(rtk_bt_hfp_call_incoming_t));

	return ret;
}

uint16_t rtk_bt_hfp_call_answer(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_HFP, RTK_BT_HFP_ACT_CALL_ANSWER,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_hfp_call_terminate(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_HFP, RTK_BT_HFP_ACT_CALL_TERMINATE,
						  bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_hfp_data_send(rtk_bt_hfp_sco_data_send_t *p_data_send_t)
{
	uint16_t ret = 0;

	if (!p_data_send_t) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_HFP, RTK_BT_HFP_ACT_SEND_SCO_DATA,
						  p_data_send_t, sizeof(rtk_bt_hfp_sco_data_send_t));

	return ret;
}

uint16_t rtk_bt_hfp_batt_level_report(uint8_t *bd_addr, uint8_t power_level)
{
	uint16_t ret = 0;
	rtk_bt_hfp_batt_level_t batt_level_t = {0};

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	memcpy((void *)batt_level_t.bd_addr, (void *)bd_addr, 6);
	batt_level_t.level = power_level;
	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_HFP, RTK_BT_HFP_ACT_REPORT_BATT_LEVEL,
						  (uint8_t *)&batt_level_t, sizeof(rtk_bt_hfp_batt_level_t));

	return ret;
}

uint16_t rtk_bt_hfp_speaker_gain_report(uint8_t *bd_addr, uint8_t gain)
{
	uint16_t ret = 0;
	rtk_bt_hfp_speaker_gain_t param_t = {0};

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	memcpy((void *)param_t.bd_addr, (void *)bd_addr, 6);
	param_t.gain = gain;
	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_HFP, RTK_BT_HFP_ACT_REPORT_SPEAKER_GAIN,
						  (uint8_t *)&param_t, sizeof(rtk_bt_hfp_speaker_gain_t));

	return ret;
}

uint16_t rtk_bt_hfp_microphone_gain_report(uint8_t *bd_addr, uint8_t gain)
{
	uint16_t ret = 0;
	rtk_bt_hfp_microphone_gain_t param_t = {0};

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	memcpy((void *)param_t.bd_addr, (void *)bd_addr, 6);
	param_t.gain = gain;
	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_HFP, RTK_BT_HFP_ACT_REPORT_MICROPHONE_GAIN,
						  (uint8_t *)&param_t, sizeof(rtk_bt_hfp_microphone_gain_t));

	return ret;
}