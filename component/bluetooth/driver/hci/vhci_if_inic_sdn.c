#include <sdn_host.h>
#include <hci_if_inic.h>
#include <bt_debug.h>

static void bt_inic_send_cmd_complete_evt(uint16_t opcode, uint8_t status)
{
	uint8_t evt[6] = {0x0e, 0x04, 0x03, 0x00, 0x00, 0x00};
	evt[3] = opcode & 0xFF;
	evt[4] = (opcode >> 8) & 0xFF;
	evt[5] = status;
	bt_inic_send_to_host(HCI_EVT, evt, sizeof(evt));
}

static bool is_inic_vendor_cmd(uint16_t opcode)
{
	bool is_inic_cmd = true;
	uint8_t status = 0;

	switch (opcode) {
	case BT_HCI_CMD_VS_BT_ON:
		status = sdn_host_enable(SDN_INTF_BT) ? 0 : 1;
		BT_LOGA("BT INIC Open\r\n");
		break;
	case BT_HCI_CMD_VS_BT_OFF:
		sdn_host_disable(SDN_INTF_BT);
		BT_LOGA("BT INIC Close\r\n");
		break;
	default:
		is_inic_cmd = false;
		break;
	}

	if (is_inic_cmd) {
		bt_inic_send_cmd_complete_evt(opcode, status);
	}

	return is_inic_cmd;
}

void bt_inic_recv_from_host(uint8_t type, uint8_t *pdata, uint32_t len)
{
	uint16_t opcode;
	if (type == HCI_CMD) {
		LE_TO_UINT16(opcode, pdata);
		if (is_inic_vendor_cmd(opcode)) {
			return;
		}
	}

	if (!sdn_host_is_enabled()) {
		BT_LOGA("Controller is off now, Power on.\r\n", type);
		sdn_host_enable(SDN_INTF_BT); // auto power on
	}
	sdn_host_send(SDN_MSG_PROTOCOL(type), SDN_MSG_TYPE(type), pdata, len);
}