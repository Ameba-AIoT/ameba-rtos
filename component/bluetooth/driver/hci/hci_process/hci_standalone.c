/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <osif.h>
#include "hci/hci_transport.h"
#include "hci/hci_common.h"

static void *recv_sem = NULL;
static uint8_t *hci_buf;

uint8_t *hci_sa_recv_get_buf(uint8_t type, void *hdr, uint16_t len, uint32_t timeout)
{
	(void)timeout;
	(void)hdr;
	(void)len;

	if (HCI_EVT != type) {
		return NULL;
	}

	/* Use Send Buf as Recv Buf */
	return hci_buf;
}

void hci_sa_recv(void)
{
	osif_sem_give(recv_sem);
}

struct hci_transport_cb hci_sa_cb = {
	.get_buf = hci_sa_recv_get_buf,
	.recv = hci_sa_recv,
	.cancel = NULL,
};

uint8_t hci_sa_send(uint8_t type, uint8_t *buf, uint16_t len, bool is_sync)
{
	if (!is_sync) {
		if (len != hci_transport_send(type, buf, len, true)) {
			return HCI_FAIL;
		}

		return HCI_SUCCESS;
	}

	osif_sem_create(&recv_sem, 0, 1);
	hci_buf = buf;

	if (len != hci_transport_send(type, buf, len, true)) {
		return HCI_FAIL;
	}

	osif_sem_take(recv_sem, BT_TIMEOUT_FOREVER);
	osif_sem_delete(recv_sem);
	recv_sem = NULL;
	hci_buf = NULL;

	/* Then We can process Response */
	return HCI_SUCCESS;
}

/* 1 byte must be reserved ahead 'buf' */
uint8_t hci_sa_send_cmd_sync(uint16_t opcode, uint8_t *buf, uint16_t len)
{
	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);

	if (HCI_SUCCESS != hci_sa_send(HCI_CMD, buf, len, true)) {
		return HCI_FAIL;
	}

	/* Check Resp: OpCode and Status */
	if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
		return HCI_FAIL;
	}

	return HCI_SUCCESS;
}