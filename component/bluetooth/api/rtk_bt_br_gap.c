/*
*******************************************************************************
* Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include "platform_autoconf.h"
#include <string.h>
#include <stdio.h>
#include <osif.h>
#include <bt_api_config.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_br_gap.h>
#include <rtk_bt_device.h>

extern uint32_t class_of_device;
extern uint16_t supervision_timeout;
extern rtk_bt_br_inquiry_param_t inquiry_t;
extern rtk_bt_br_page_param_t page_t;
extern uint16_t bt_stack_br_gap_set_default_param(rtk_bt_br_gap_default_param_t *param);

uint16_t rtk_bt_br_gap_set_default_param(rtk_bt_br_gap_default_param_t *param)
{
	/* BR/EDR set default param should be invoked before bt enable */
	if (rtk_bt_is_enable()) {
		return RTK_BT_ERR_NOT_READY;
	}

	if (!param) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	return bt_stack_br_gap_set_default_param(param);
}

uint16_t rtk_bt_br_gap_get_bd_addr(rtk_bt_br_bd_addr_t *paddr)
{
	uint16_t ret = 0;

	if (!paddr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_GET_DEVICE_ADDR, paddr, sizeof(rtk_bt_br_bd_addr_t));

	return ret;
}

uint16_t rtk_bt_br_gap_set_device_name(const uint8_t *name)
{
	uint16_t ret = 0;

	if (!name) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_SET_DEVICE_NAME, (void *)name, strlen((const char *)name));

	return ret;
}

/* init page parameters */
uint16_t rtk_bt_br_gap_set_page_param(rtk_bt_br_page_param_t *page_param)
{
	/* Check bt enable status (rtk_bt_br_gap_set_page_param should only be invoked before RTK_BT_IS_ENABLED()) */
	if (rtk_bt_is_enable()) {
		BT_LOGE("[BR GAP]: rtk_bt_br_gap_set_page_param should not be invoked after bt enable \r\n");
		return RTK_BT_FAIL;
	}

	if (!page_param) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	memcpy((void *)&page_t, (void *)page_param, sizeof(rtk_bt_br_page_param_t));

	return RTK_BT_OK;
}

uint16_t rtk_bt_br_gap_cfg_page_param(uint8_t type, uint16_t interval, uint16_t window)
{
	uint16_t ret = 0;
	rtk_bt_br_page_param_t page_t = {0};

	page_t.pagescan_type = type;
	page_t.pagescan_interval = interval;
	page_t.pagescan_window = window;
	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_CFG_PAGE_PARAM, (void *)&page_t, sizeof(rtk_bt_br_page_param_t));

	return ret;
}

/* init inquiry parameters */
uint16_t rtk_bt_br_gap_set_inquiry_param(rtk_bt_br_inquiry_param_t *inquiry_param)
{
	/* Check bt enable status (rtk_bt_br_gap_set_inquiry_param should only be invoked before RTK_BT_IS_ENABLED()) */
	if (rtk_bt_is_enable()) {
		BT_LOGE("[BR GAP]: rtk_bt_br_gap_set_inquiry_param should not be invoked after bt enable \r\n");
		return RTK_BT_FAIL;
	}

	if (!inquiry_param) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	memcpy((void *)&inquiry_t, (void *)inquiry_param, sizeof(rtk_bt_br_inquiry_param_t));

	return RTK_BT_OK;
}

uint16_t rtk_bt_br_gap_cfg_inquiry_param(uint8_t type, uint16_t interval, uint16_t window)
{
	uint16_t ret = 0;
	rtk_bt_br_inquiry_param_t inquiry_t = {0};

	inquiry_t.inquiryscan_type = type;
	inquiry_t.inquiryscan_interval = interval;
	inquiry_t.inquiryscan_window = window;
	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_CFG_INQUIRY_PARAM, (void *)&inquiry_t, sizeof(rtk_bt_br_inquiry_param_t));

	return ret;
}

uint16_t rtk_bt_br_sm_set_security_param(rtk_bt_br_security_param_t *p_sec_param)
{
	uint16_t ret = 0;

	if (!p_sec_param ||
		p_sec_param->io_cap >= RTK_BR_IO_CAP_MAX ||
		p_sec_param->io_cap != RTK_BR_IO_CAP_NO_IN_NO_OUT
	   ) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_SET_SEC_PARAM,
						  p_sec_param, sizeof(rtk_bt_br_security_param_t));

	return ret;
}

uint16_t rtk_bt_br_gap_disconnect(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_DISCONNECT, (void *)bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_br_gap_start_inquiry(rtk_bt_br_inquiry_start_t *inquiry_start_param)
{
	uint16_t ret = 0;

	if (!inquiry_start_param) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_START_INQUIRY, (void *)inquiry_start_param, sizeof(rtk_bt_br_inquiry_start_t));

	return ret;
}

uint16_t rtk_bt_br_gap_get_remote_name(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_GET_REMOTE_NAME, (void *)bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_br_gap_max_bond_num_get(uint8_t *pmax_bond_num)
{
	uint16_t ret = 0;

	if (!pmax_bond_num) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_MAX_BOND_NUM_GET, (void *)pmax_bond_num, sizeof(uint8_t));

	return ret;
}

uint16_t rtk_bt_br_gap_bond_num_get(uint8_t *pbond_num)
{
	uint16_t ret = 0;

	if (!pbond_num) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_BOND_NUM_GET, (void *)pbond_num, sizeof(uint8_t));

	return ret;
}

uint16_t rtk_bt_br_gap_bond_addr_get(uint8_t priority, uint8_t *bd_addr)
{
	uint16_t ret = 0;
	rtk_bt_br_bond_addr_t bond_addr_t = {0};

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	bond_addr_t.priority = priority;
	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_BOND_ADDR_GET, (void *)&bond_addr_t, sizeof(rtk_bt_br_bond_addr_t));
	if (RTK_BT_OK == ret) {
		memcpy((void *)bd_addr, (void *)bond_addr_t.bd_addr, 6);
	}

	return ret;
}

uint16_t rtk_bt_br_gap_bond_index_get(uint8_t *pindex, uint8_t *bd_addr)
{
	uint16_t ret = 0;
	rtk_bt_br_bond_index_t bond_index_t = {0};

	if (!pindex || !bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	memcpy((void *)bond_index_t.bd_addr, (void *)bd_addr, 6);
	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_BOND_INDEX_GET, (void *)&bond_index_t, sizeof(rtk_bt_br_bond_index_t));
	if (RTK_BT_OK == ret) {
		memcpy((void *)pindex, (void *)&bond_index_t.index, 1);
	}

	return ret;
}

uint16_t rtk_bt_br_gap_bond_key_get(uint8_t *bd_addr, uint8_t *key_type, uint8_t *bond_key)
{
	uint16_t ret = 0;
	rtk_bt_br_bond_key_t bond_key_t = {0};

	if (!bd_addr || !key_type || !bond_key) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	memcpy((void *)bond_key_t.bd_addr, (void *)bd_addr, 6);
	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_BOND_KEY_GET, (void *)&bond_key_t, sizeof(rtk_bt_br_bond_key_t));
	if (RTK_BT_OK == ret) {
		memcpy((void *)bond_key, (void *)bond_key_t.link_key, 16);
		*key_type = bond_key_t.key_type;
	}

	return ret;
}

uint16_t rtk_bt_br_gap_bond_key_set(rtk_bt_br_bond_key_t *pbond_key_t)
{
	uint16_t ret = 0;

	if (!pbond_key_t) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_BOND_KEY_SET, (void *)pbond_key_t, sizeof(rtk_bt_br_bond_key_t));

	return ret;
}

uint16_t rtk_bt_br_gap_bond_priority_set(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_BOND_PRIORITY_SET, (void *)bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_br_gap_bond_delete(uint8_t *bd_addr)
{
	uint16_t ret = 0;

	if (!bd_addr) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_BOND_DELETE, (void *)bd_addr, 6);

	return ret;
}

uint16_t rtk_bt_br_gap_bond_clear()
{
	uint16_t ret = 0;

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_BOND_CLEAR, NULL, 0);

	return ret;
}

uint16_t rtk_bt_br_gap_set_pincode(uint8_t *pin_code, uint32_t length)
{
	uint16_t ret = 0;

	if (!pin_code) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_SET_PINCODE, (void *)pin_code, length);

	return ret;
}

uint16_t rtk_bt_br_gap_set_radio_mode(uint8_t radio_mode)
{
	uint16_t ret = 0;

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_SET_RADIO_MODE, (void *)&radio_mode, sizeof(uint8_t));

	return ret;
}

uint16_t rtk_bt_br_gap_set_sniff_mode(uint8_t enable, uint8_t *bd_addr, uint16_t min_interval, uint16_t max_interval, uint16_t sniff_attempt,
									  uint16_t sniff_timeout)
{
	uint16_t ret = 0;
	rtk_bt_br_sniff_mode_t sniff_mode_t = {0};

	sniff_mode_t.enable = enable;
	memcpy((void *)sniff_mode_t.bd_addr, (void *)bd_addr, 6);
	sniff_mode_t.min_interval = min_interval;
	sniff_mode_t.max_interval = max_interval;
	sniff_mode_t.sniff_attempt = sniff_attempt;
	sniff_mode_t.sniff_timeout = sniff_timeout;

	ret = rtk_bt_send_cmd(RTK_BT_BR_GP_GAP, RTK_BT_BR_GAP_ACT_SET_SNIFF_MODE, (void *)&sniff_mode_t, sizeof(rtk_bt_br_sniff_mode_t));

	return ret;
}

uint16_t rtk_bt_br_gap_set_cod(uint32_t *bt_cod)
{
	/* Check bt enable status (rtk_bt_br_gap_set_cod should only be invoked before RTK_BT_IS_ENABLED()) */
	if (rtk_bt_is_enable()) {
		BT_LOGE("[BR GAP]: rtk_bt_br_gap_set_cod should not be invoked after bt enable \r\n");
		return RTK_BT_FAIL;
	}
	if (!bt_cod) {
		BT_LOGE("[BR GAP]: bt_cod is NULL \r\n");
		return RTK_BT_FAIL;
	}

	memcpy((void *)&class_of_device, (void *)bt_cod, sizeof(uint32_t));

	return RTK_BT_OK;
}

uint16_t rtk_bt_br_gap_set_supvisiontimeout(uint16_t *supv_timeout)
{
	/* Check bt enable status (rtk_bt_br_gap_set_supvisiontimeout should only be invoked before RTK_BT_IS_ENABLED()) */
	if (rtk_bt_is_enable()) {
		BT_LOGE("[BR GAP]: rtk_bt_br_gap_set_supvisiontimeout should not be invoked after bt enable \r\n");
		return RTK_BT_FAIL;
	}
	if (!supv_timeout) {
		BT_LOGE("[BR GAP]: supv_timeout is NULL \r\n");
		return RTK_BT_FAIL;
	}

	memcpy((void *)&supervision_timeout, (void *)supv_timeout, sizeof(uint16_t));

	return RTK_BT_OK;
}