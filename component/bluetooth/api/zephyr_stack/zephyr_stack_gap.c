/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <dlist.h>
#include <osif.h>

#include <rtk_bt_def.h>
#include <rtk_bt_gap.h>
#include <rtk_bt_common.h>
#include <zephyr_stack_config.h>
#include <bt_api_config.h>

#include <zephyr/bluetooth/hci.h>
#include <zephyr_stack_api.h>
#include <zephyr_stack_internal.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/net/buf.h>
#include <stack/host/hci_core.h>
#include <stack/host/conn_internal.h>
#include <stack/host/smp.h>
#include <stack/host/adv.h>
#include <zephyr/sys/byteorder.h>
#include <stack/host/keys.h>
#include <stack/host/settings.h>

uint16_t bt_stack_gap_vendor_cmd_req(void *param)
{
	struct net_buf *net_buf = NULL;
	rtk_bt_gap_vendor_cmd_param_t *vendor = (rtk_bt_gap_vendor_cmd_param_t *)param;

	net_buf = bt_hci_cmd_create(vendor->op, vendor->len);
	if (!net_buf) {
		return RTK_BT_ERR_NO_RESOURCE;
	}

	if (vendor->len) {
		net_buf_add_mem(net_buf, vendor->cmd_param, vendor->len);
	}

	if (bt_hci_cmd_send(vendor->op, net_buf)) {
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

uint16_t bt_stack_gap_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = 0;
	BT_LOGD("%s: act = %d \r\n", __func__, p_cmd->act);
	switch (p_cmd->act) {
	case RTK_BT_GAP_ACT_VENDOR_CMD_REQ:
		ret = bt_stack_gap_vendor_cmd_req(p_cmd->param);
		break;
	default:
		BT_LOGE("%s: unknown act: %d \r\n", __func__, p_cmd->act);
		ret = RTK_BT_ERR_NO_CASE_ELEMENT;
		break;
	}

	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}