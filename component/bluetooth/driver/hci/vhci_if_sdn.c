/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdbool.h>

#include "bt_debug.h"
#include "hci_common.h"
#include "hci_platform.h"
#include <sdn_host.h>

static bool g_bt_controller_open = false;

/*
function:whether the hci rx packet is discardable(when receive ADV report or EXT ADV report)
HCI event packet:HCI type(1byte 0x04) + event code(1byte) + length(1byte) + param0 + param1 + ......
HCI adv/ext adv report:HCI type(1byte 0x04) + event code(0x3e) + length(0x14) + subevent code(0x02 for adv,0x0d for ext adv) + number reports(1byte) + ......
HCI le adv report packet raw data
04 3E 14 02 01 03 01 88 32 9A
44 FC CF 08 07 FF 4C 00 12 02
00 00 A1
the input param p_rx is start of event_code(2nd bytes of HCI RX event packet), this function only be called when HCI type is HCI event(0x04)
*/
bool vhci_rx_evt_discardable(uint8_t evt, uint8_t le_sub_evt)
{
	if (BT_HCI_EVT_LE_META_EVENT == evt) {
		if ((le_sub_evt == BT_HCI_EVT_LE_ADVERTISING_REPORT) ||
			(le_sub_evt == BT_HCI_EVT_LE_EXT_ADVERTISING_REPORT)) {
			return true;
		}
	}

	return false;
}

extern int hci_recv(uint8_t type, uint8_t *pdata, uint16_t len, bool discardable);
int bt_hci_host_rx_pkt(uint8_t type, uint8_t *pdata, uint16_t len)
{
	bool rx_discardable = false;

	rx_discardable = vhci_rx_evt_discardable(pdata[0], pdata[2]);

	return hci_recv(type, pdata, len, rx_discardable);
}

int vhci_host_open(void)
{
	if (g_bt_controller_open) {
		BT_LOGA("bt controller has already open \r\n");
		return 0;
	}

	rtk_bt_pre_enable();

	if (sdn_host_enable(SDN_INTF_BT)) {
		return -1;
	}
	rtk_bt_post_enable();

	g_bt_controller_open = true;

	return 0;
}

/*
example for HCI reset command and event from HCI uart bus
HCI command packet:01 03 0C 00      01(HCI pkg type for Command) + 03 0c(HCI reset command opcode) + 0(param len)
HCI command complete event packet:04 0E 04 03 03 0C 00      04(HCI pkg type for Event) + 0e(command complete event code) + 04(length of param) + 03(Num_HCI_Command_Packets) + 03 0c(command opcode) + 00(return param)
in hci_send() buf->data:03 0C 00  buf-len:3
before call bt_recv() buf->data:0E 04 03 03 0C 00  buf->len:6

example for HCI LE set ADV enable command and event from HCI uart bus
HCI command packet:01 0A 20 01 01      01(HCI pkg type for command) + 0a 20(HCI set ADV enable opcode) + 01(param len) + 01(ADV enable)
HCI command complete event packet:04 0E 04 02 0A 20 00      04(HCI pkg type for Event) + 0e(command complete event code) + 04(length of param) + 02(Num_HCI_Command_Packets) + 0a 20(command opcode) + 00(return param)
*/
int vhci_host_send(uint8_t hci_type, uint8_t *pdata, uint16_t len)
{
	if (sdn_host_send(SDN_INTF_BT, hci_type, pdata, len)) {
		return -1;
	}

	return 0;
}

void vhci_host_close(void)
{
	sdn_host_disable(SDN_INTF_BT);

	g_bt_controller_open = false;
}



