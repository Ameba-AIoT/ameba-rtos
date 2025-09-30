/*
//  *******************************************************************************
//  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
//  *******************************************************************************
*/
#include <string.h>
#include "hci_common.h"
#include "hci_transport.h"
#include "hci_if_inic_sdn.h"
#include "bt_debug.h"

#if defined(CONFIG_BT_DUAL_MODE) && CONFIG_BT_DUAL_MODE
#define CONFIG_HCI_RX_DATA_BUF_LEN     1056  /* shall be 4 byte aligned */
#else
#define CONFIG_HCI_RX_DATA_BUF_LEN     288   /* shall be 4 byte aligned */
#endif
#define CONFIG_HCI_RX_DATA_NUM         8
/* RX HCI event */
#define CONFIG_HCI_RX_EVT_BUF_LEN      260   /* shall be 4 byte aligned */
#define CONFIG_HCI_RX_EVT_NUM          8

#define CONFIG_HCI_RX_TOTAL_NUM        (CONFIG_HCI_RX_DATA_NUM + CONFIG_HCI_RX_EVT_NUM)

static struct hci_sdn_t {
	void    *rx_data_free;
	void    *rx_evt_free;
	struct hci_rx_packet_t rxpkts[CONFIG_HCI_RX_TOTAL_NUM];
	uint8_t rx_data_buf[CONFIG_HCI_RX_DATA_NUM][CONFIG_HCI_RX_DATA_BUF_LEN]; /* buf start pionter shall be 4 byte aligned */
	uint8_t rx_evt_buf[CONFIG_HCI_RX_EVT_NUM][CONFIG_HCI_RX_EVT_BUF_LEN]; /* buf start pionter shall be 4 byte aligned */
} *hci_sdn = NULL;

static void bt_inic_send_cmd_complete_evt(uint16_t opcode, uint8_t status)
{
	uint8_t evt[6] = {0x0e, 0x04, 0x03, 0x00, 0x00, 0x00};
	evt[3] = opcode & 0xFF;
	evt[4] = (opcode >> 8) & 0xFF;
	evt[5] = status;
	bt_inic_send_to_host(HCI_EVT, evt, sizeof(evt));
}

static bool hci_rx_pkt_alloc_sdn(struct hci_rx_packet_t **pkt, uint8_t type)
{
	if (!hci_sdn) {
		return false;
	}

	void *rx_free_q = ((HCI_EVT == type) ? hci_sdn->rx_evt_free : hci_sdn->rx_data_free);
	return osif_msg_recv(rx_free_q, pkt, BT_TIMEOUT_NONE);
}

static bool hci_rx_pkt_free_sdn(struct hci_rx_packet_t *pkt)
{
	if (!hci_sdn || !pkt) {
		return false;
	}

	void *rx_free_q = ((HCI_EVT == pkt->type) ? hci_sdn->rx_evt_free : hci_sdn->rx_data_free);

	if (!osif_msg_send(rx_free_q, &pkt, BT_TIMEOUT_NONE)) {
		return false;
	}

	return true;
}

static uint8_t hci_sdn_transport_open(void)
{
	int i;
	void *pkt;

	if (hci_sdn) {
		return true;
	}

	hci_sdn = osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct hci_sdn_t));
	if (!hci_sdn) {
		return false;
	}
	memset(hci_sdn, 0, sizeof(struct hci_sdn_t));

	if (false == osif_msg_queue_create(&hci_sdn->rx_data_free, CONFIG_HCI_RX_DATA_NUM, sizeof(struct hci_rx_packet_t *))) {
		goto failed;
	}

	if (false == osif_msg_queue_create(&hci_sdn->rx_evt_free, CONFIG_HCI_RX_EVT_NUM, sizeof(struct hci_rx_packet_t *))) {
		goto failed;
	}

	for (i = 0; i < CONFIG_HCI_RX_TOTAL_NUM; i++) {
		struct hci_rx_packet_t *pkt = &hci_sdn->rxpkts[i];
		if (i < CONFIG_HCI_RX_DATA_NUM) {
			pkt->buf = &hci_sdn->rx_data_buf[i][0];
			if (false == osif_msg_send(hci_sdn->rx_data_free, &pkt, BT_TIMEOUT_NONE)) {
				goto failed;
			}
		} else {
			pkt->buf = &hci_sdn->rx_evt_buf[i - CONFIG_HCI_RX_DATA_NUM][0];
			if (false == osif_msg_send(hci_sdn->rx_evt_free, &pkt, BT_TIMEOUT_NONE)) {
				goto failed;
			}
		}
	}

	return true;

failed:
	if (hci_sdn->rx_data_free) {
		while (osif_msg_recv(hci_sdn->rx_data_free, &pkt, BT_TIMEOUT_NONE));
		osif_msg_queue_delete(hci_sdn->rx_data_free);
	}
	if (hci_sdn->rx_evt_free) {
		while (osif_msg_recv(hci_sdn->rx_evt_free, &pkt, BT_TIMEOUT_NONE));
		osif_msg_queue_delete(hci_sdn->rx_evt_free);
	}
	if (hci_sdn) {
		osif_mem_free(hci_sdn);
		hci_sdn = NULL;
	}

	return false;
}

static void hci_sdn_transport_free(void)
{
	void *pkt;
	if (!hci_sdn) {
		return;
	}

	if (hci_sdn->rx_data_free) {
		while (osif_msg_recv(hci_sdn->rx_data_free, &pkt, BT_TIMEOUT_NONE));
		osif_msg_queue_delete(hci_sdn->rx_data_free);
	}

	if (hci_sdn->rx_evt_free) {
		while (osif_msg_recv(hci_sdn->rx_evt_free, &pkt, BT_TIMEOUT_NONE));
		osif_msg_queue_delete(hci_sdn->rx_evt_free);
	}

	osif_mem_free(hci_sdn);
	hci_sdn = NULL;
}

uint8_t *bt_inic_sdn_get_buf(void **packet, uint8_t type, void *hdr, uint16_t len, uint32_t timeout)
{
	UNUSED(hdr);
	UNUSED(timeout);

	struct hci_rx_packet_t *rx_packet = NULL;

	if (hci_rx_pkt_alloc_sdn(&rx_packet, type)) {
		if (rx_packet == NULL) {
			BT_LOGE("Failed to allocate memory for rx_packet\r\n");
			*packet = NULL;
			return NULL;
		}

		rx_packet->type = type;
		rx_packet->len = len;
		*packet = (void *)rx_packet;
		return rx_packet->buf;
	} else {
		BT_LOGE("Failed to allocate memory for rx_packet\r\n");
		*packet = NULL;
		return NULL;
	}
}

/* packet is received from controller, send it to host. */
void bt_inic_sdn_hci_recv(void *packet)
{
	struct hci_rx_packet_t *pkt = (struct hci_rx_packet_t *)packet;

	// BT_LOGA("--> Recv: type %d, len = %d, data:\r\n", pkt->type, pkt->len);
	BT_DUMPA("Host recv: ", pkt->buf, pkt->len);

	bt_inic_send_to_host(pkt->type, pkt->buf, pkt->len);
	hci_rx_pkt_free_sdn(pkt);
}

static bool bt_sdn_controller_open(void)
{
	if (!hci_sdn_transport_open()) {
		return false;
	}

	if (!bt_controller_hci_open()) {
		return false;
	}
	return true;
}

static void bt_sdn_controller_close(void)
{
	bt_controller_hci_close();
	hci_sdn_transport_free();
	return;
}

static bool is_inic_vendor_cmd(uint16_t opcode)
{
	bool is_inic_cmd = true;
	uint8_t status = 0;

	switch (opcode) {
	case BT_HCI_CMD_VS_BT_ON:
		status = bt_sdn_controller_open() ? 0 : 1;
		BT_LOGA("BT INIC Open\r\n");
		break;
	case BT_HCI_CMD_VS_BT_OFF:
		bt_sdn_controller_close();
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

static int bt_sdn_hci_send(uint8_t type, uint8_t *pbuf, uint16_t len)
{
	bt_controller_hci_send(type, pbuf, len);
	return 0;
}

/* packet is received from host, send it to controller. */
void bt_inic_recv_from_host(uint8_t type, uint8_t *pdata, uint32_t len)
{
	uint16_t opcode;
	if (type == HCI_CMD) {
		LE_TO_UINT16(opcode, pdata);
		if (is_inic_vendor_cmd(opcode)) {
			return;
		}
	}

	// BT_LOGA("--> Send: type %d, len = %d, data:\r\n", type, len);
	BT_DUMPA("Host send: ", pdata, len);

	if (!bt_controller_hci_is_opened()) {
		BT_LOGA("Controller is off now, Power on.\r\n", type);
		bt_sdn_controller_open(); // auto power on
	}

	bt_sdn_hci_send(type, pdata, len);
}