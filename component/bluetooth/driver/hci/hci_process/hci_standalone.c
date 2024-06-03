/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <osif.h>
#include "hci/hci_transport.h"
#include "hci/hci_process.h"
#include "hci/hci_common.h"

static void *recv_sem = NULL;
static uint8_t *hci_buf;
//static uint16_t hci_buf_len;

bool hci_sa_recv_get_buf(hci_rx_t *info, uint32_t timeout)
{
	(void)timeout;

	if (H4_EVT != info->type) {
		return false;
	}

	/* Use Send Buf as Recv Buf */
	info->data = hci_buf;
	return true;
}

uint8_t hci_sa_recv(hci_rx_t *info)
{
	(void)info;

	osif_sem_give(recv_sem);

	return HCI_SUCCESS;
}

uint8_t hci_sa_send(uint8_t type, uint8_t *buf, uint16_t len, bool is_sync)
{
	if (!is_sync) {
		if (len != hci_transport_send(type, buf, len, 1)) {
			return HCI_FAIL;
		}

		return HCI_SUCCESS;
	}

	osif_sem_create(&recv_sem, 0, 1);
	hci_buf = buf;

	if (len != hci_transport_send(type, buf, len, 1)) {
		return HCI_FAIL;
	}

	osif_sem_take(recv_sem, BT_TIMEOUT_FOREVER);
	osif_sem_delete(recv_sem);
	recv_sem = NULL;
	hci_buf = NULL;

	/* Then We can process Response */
	return HCI_SUCCESS;
}