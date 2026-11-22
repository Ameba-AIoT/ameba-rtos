/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "osif.h"
#include "hci_transport.h"
#include "hci_uart.h"
#include "hci_platform.h"
#include "bt_debug.h"
#include "dlist.h"
#include "hci_controller.h"
#include <bt_eqc_common.h>

#define IS_ALIGNED(ptr, align) (((uintptr_t)(ptr)) % (align) == 0)

extern void *eqc_event_sem;
extern void *eqc_evt_queue_handle;  //!< Event queue handle
static uint8_t *hci_eqc_buf;
static uint16_t hci_eqc_buf_size;
void *eqc_recv_sem = NULL;

struct tx_packet_t {
	uint8_t *buf;
	uint32_t len;
};

static void eqc_recv(struct hci_rx_packet_t *pkt)
{
	E_EVENT_MSG event_msg;

	if (!IS_ALIGNED(pkt->buf, 4)) {
		BT_LOGE("rx buf not aligned\r\n");
		goto failed;
	}

	if (HCI_EVT == pkt->type && (BT_HCI_EVT_CMD_COMPLETE == (*(pkt->buf)) ||
								 BT_HCI_EVT_CMD_STATUS == (*(pkt->buf)))) {  /* command complete && command status event */
		if (!hci_eqc_buf || (pkt->len > hci_eqc_buf_size)) {
			BT_LOGE("EQC: rx pkt too long (%d)\r\n", pkt->len);
			goto failed;
		}
		memcpy(hci_eqc_buf, pkt->buf, pkt->len);
		osif_sem_give(eqc_recv_sem);
	} else {
		if (eqc_evt_queue_handle) {
			uint8_t *pbuf = (uint8_t *)osif_mem_alloc(0, pkt->len);
			if (!pbuf) {
				BT_LOGE("alloc pbuf fail\n\r");
				goto failed;
			}
			memset(pbuf, 0, pkt->len);
			memcpy(pbuf, pkt->buf, pkt->len);
			event_msg.buf = (void *)pbuf;
			event_msg.len = pkt->len;

			if (false == osif_msg_send(eqc_evt_queue_handle, &event_msg, 0)) {
				BT_LOGE("send event queue fail!\n\r");
				osif_mem_free(pbuf);
			}
		} else {
			BT_LOGE("evt_queue_handle is NULL\n\r");
		}
	}

failed:
	hci_rx_pkt_free(pkt);
}

struct hci_transport_cb eqc_cb = {
	.recv = eqc_recv,
};

void hci_eqc_open(void)
{
	hci_set_mp(true);
	if (hci_controller_open()) {
		hci_transport_register(&eqc_cb);

		if (false == osif_sem_create(&eqc_recv_sem, 0, 1)) {
			BT_LOGE("EQC:eqc_recv_sem create fail!\r\n");
			goto failed;
		}
	}
	return;

failed:
	hci_controller_close();
	hci_controller_free();
	return;
}

void hci_eqc_close(void)
{
	osif_sem_delete(eqc_recv_sem);
	eqc_recv_sem = NULL;
	hci_eqc_buf = NULL;
	hci_eqc_buf_size = 0;

	hci_controller_close();
	hci_controller_free();
	hci_set_mp(false);
}

/*command status*/
bool hci_if_eqc_send_status(uint16_t opcode, uint8_t *buf, uint16_t len, uint16_t buf_size)
{
	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);

	hci_eqc_buf = buf;
	hci_eqc_buf_size = buf_size;

	if (len != hci_transport_send(HCI_CMD, buf, len, true)) {
		hci_eqc_buf = NULL;
		hci_eqc_buf_size = 0;
		return false;
	}

	osif_sem_take(eqc_recv_sem, BT_TIMEOUT_FOREVER);
	hci_eqc_buf = NULL;
	hci_eqc_buf_size = 0;

	/* Check Resp: OpCode and Status */
	if (buf[4] != (uint8_t)(opcode >> 0) || buf[5] != (uint8_t)(opcode >> 8) || buf[2] != 0x00) {
		BT_LOGE("check command status[0x%x] fail\n\r", buf[2]);
		return false;
	}

	return true;
}

/*command complete*/
bool hci_if_eqc_send_comple(uint16_t opcode, uint8_t *buf, uint16_t len, uint16_t buf_size)
{
	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);

	hci_eqc_buf = buf;
	hci_eqc_buf_size = buf_size;

	if (len != hci_transport_send(HCI_CMD, buf, len, true)) {
		hci_eqc_buf = NULL;
		hci_eqc_buf_size = 0;
		return false;
	}

	osif_sem_take(eqc_recv_sem, BT_TIMEOUT_FOREVER);
	hci_eqc_buf = NULL;
	hci_eqc_buf_size = 0;

	/* Check Resp: OpCode and Status */
	if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
		BT_LOGE("check command complete[0x%x] fail\n\r", buf[5]);
		return false;
	}

	return true;
}