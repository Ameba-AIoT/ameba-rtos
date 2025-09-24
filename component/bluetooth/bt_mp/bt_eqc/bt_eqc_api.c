/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <basic_types.h>
#include "bt_debug.h"
#include "hci_common.h"
#include "hci_transport.h"
#include "hci_uart.h"
#include "bt_eqc_api.h"
#include "hci_transport.h"

void rtk_bt_eqc_power_on(void)
{
	hci_eqc_open();
}

void rtk_bt_eqc_power_off(void)
{
	hci_eqc_close();
}

bool rtk_bt_eqc_set_event_mask(void)
{
	uint8_t event_mask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F};
	uint8_t buf[8 + 3] = {0};

	buf[2] = (uint8_t)8;
	buf[3] = event_mask[0];
	buf[4] = event_mask[1];
	buf[5] = event_mask[2];
	buf[6] = event_mask[3];
	buf[7] = event_mask[4];
	buf[8] = event_mask[5];
	buf[9] = event_mask[6];
	buf[10] = event_mask[7];

	if (HCI_SUCCESS != hci_if_eqc_send_comple(0x0C01, buf, buf[2] + 3)) {
		return false;
	}
	//recieved command complete event & status == 0
	return true;
}

bool rtk_bt_eqc_read_bt_address(uint8_t *bt_address)
{
	uint8_t buf[14 + 3] = {0};

	buf[2] = (uint8_t)0;

	if (HCI_SUCCESS != hci_if_eqc_send_comple(0x1009, buf, buf[2] + 3)) {
		return false;
	}

	for (uint8_t i = 0; i < 6; i ++) {
		bt_address[i] = buf[i + 6];
	}

	return true;
}

bool rtk_bt_eqc_le_start_advertising(bool enable)
{
	uint8_t buf[6 + 3] = {0};

	buf[2] = (uint8_t)1;

	if (1 == enable) {
		buf[3] = 0x1;
	} else {
		buf[3] = 0x0;
	}

	if (HCI_SUCCESS != hci_if_eqc_send_comple(0x200A, buf, buf[2] + 3)) {
		return false;
	}

	return true;
}

bool rtk_bt_eqc_le_connect(uint8_t *bt_address)
{
	uint8_t buf[25 + 3] = {0};

	rtk_bt_le_create_conn_param_t le_connect_param = {
		.scan_interval     = 0xA0,
		.scan_window       = 0xA0,
		.filter_policy     = RTK_BT_LE_CONN_FILTER_WITHOUT_WHITELIST,
		.conn_interval_max = 0x2C,
		.conn_interval_min = 0x2C,
		.conn_latency      = 0,
		.supv_timeout      = 0xDC,
		.scan_timeout      = 0,
		.own_addr_type     = 0,
		.peer_addr = {
			.type = (rtk_bt_le_addr_type_t)0,
			.addr_val = {0x66, 0x55, 0x44, 0x33, 0x22, 0x11}
		}
	};

	buf[2] = (uint8_t)25;
	buf[3] = le_connect_param.scan_interval & 0xFF;
	buf[4] = le_connect_param.scan_interval >> 8 & 0xFF;
	buf[5] = le_connect_param.scan_window & 0xFF;
	buf[6] = le_connect_param.scan_window >> 8 & 0xFF;
	buf[7] = le_connect_param.filter_policy;
	buf[8] = 0;

	for (uint8_t i = 0; i < 6; i ++) {
		buf[i + 9] = bt_address[i];
	}

	buf[15] = le_connect_param.own_addr_type;
	buf[16] = le_connect_param.conn_interval_min & 0xFF;
	buf[17] = le_connect_param.conn_interval_min >> 8 & 0xFF;
	buf[18] = le_connect_param.conn_interval_max & 0xFF;
	buf[19] = le_connect_param.conn_interval_max >> 8 & 0xFF;
	buf[20] = le_connect_param.conn_latency & 0xFF;
	buf[21] = le_connect_param.conn_latency >> 8 & 0xFF;
	buf[22] = le_connect_param.supv_timeout & 0xFF;
	buf[23] = le_connect_param.supv_timeout >> 8 & 0xFF;
	buf[24] = 0x50;  //ce_lengh
	buf[25] = 0x00;  //ce_lengh
	buf[26] = 0x50;  //ce_lengh
	buf[27] = 0x00;  //ce_lengh

	if (HCI_SUCCESS != hci_if_eqc_send_status(0x200D, buf, buf[2] + 3)) {
		return false;
	}
	//recieved command status event & status == 0
	return true;
}

bool rtk_bt_eqc_le_connect_cancel(void)
{
	uint8_t buf[8 + 3] = {0};

	buf[2] = (uint8_t)0;

	if (HCI_SUCCESS != hci_if_eqc_send_comple(0x200E, buf, buf[2] + 3)) {
		return false;
	}
	//recieved command complete event & status == 0
	return true;
}

bool rtk_bt_eqc_le_disconnect(uint16_t conn_handle, uint8_t disc_reason)
{
	uint8_t buf[6 + 3] = {0};

	buf[2] = (uint8_t)3;
	buf[3] = conn_handle & 0xFF;
	buf[4] = conn_handle >> 8 & 0xFF;
	buf[5] = disc_reason;

	if (HCI_SUCCESS != hci_if_eqc_send_status(0x0406, buf, buf[2] + 3)) {
		return false;
	}
	//recieved command status event & status == 0
	return true;
}

bool rtk_bt_eqc_le_read_current_channel_map(uint16_t conn_handle)
{
	uint8_t buf[8 + 3] = {0};
	uint8_t channel_map[5] = {0};

	buf[2] = (uint8_t)2;
	buf[3] = conn_handle & 0xFF;
	buf[4] = conn_handle >> 8 & 0xFF;

	if (HCI_SUCCESS != hci_if_eqc_send_comple(0x2015, buf, buf[2] + 3)) {
		return false;
	}
	//recieved command status event & status == 0
	BT_LOGA("Geted channel map 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n\r", buf[6], buf[7], buf[8], buf[9], buf[10]);

	if (memcmp(channel_map, &buf[6], 5) == 0) {
		BT_LOGE("invalid channelmap\n\r");
		return false;
	}

	return true;
}