/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "osif.h"
#include <string.h>
#include "bt_debug.h"
#include "hci_transport.h"
#include "hci_controller.h"
#include "hci_if_dtm.h"

static void *dtm_sem = NULL;
static uint8_t *dtm_buf;

static void hci_dtm_recv(struct hci_rx_packet_t *pkt)
{
	memcpy(dtm_buf, pkt->buf, pkt->len);
	osif_sem_give(dtm_sem);
	hci_rx_pkt_free(pkt);
}

static struct hci_transport_cb dtm_cb = {
	.recv = hci_dtm_recv,
};

void hci_dtm_open(void)
{
	hci_set_mp(true);
	if (hci_controller_open()) {
		hci_transport_register(&dtm_cb);

		if (false == osif_sem_create(&dtm_sem, 0, 1)) {
			BT_LOGE("DTM: dtm_sem create fail!\r\n");
			goto failed;
		}
	}
	return;

failed:
	hci_controller_close();
	hci_controller_free();
	return;
}

void hci_dtm_close(void)
{
	osif_sem_delete(dtm_sem);
	dtm_sem = NULL;

	hci_controller_close();
	hci_controller_free();
	hci_set_mp(false);
}

bool hci_dtm_send(uint16_t opcode, uint8_t *buf, uint16_t len)
{
	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);

	dtm_buf = buf;
	if (len != hci_transport_send(HCI_CMD, buf, len, false)) {
		BT_LOGE("DTM: hci_transport_send fail!\r\n");
		return false;
	}

	osif_sem_take(dtm_sem, BT_TIMEOUT_FOREVER);
	dtm_buf = NULL;

	/* Check Resp: OpCode and Status */
	if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
		BT_LOGE("DTM: check response fail!\r\n");
		return false;
	}

	return true;
}