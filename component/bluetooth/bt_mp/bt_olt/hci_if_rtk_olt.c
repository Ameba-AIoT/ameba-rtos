#include "osif.h"
#include "hci_transport.h"
#include "hci_uart.h"
#include <string.h>
#include "bt_debug.h"
#include "dlist.h"
#include "hci_controller.h"
#include <bt_olt_common.h>

#define IS_ALIGNED(ptr, align) (((uintptr_t)(ptr)) % (align) == 0)

static uint8_t *hci_olt_buf;
void *olt_recv_sem = NULL;

static void olt_recv(struct hci_rx_packet_t *pkt)
{
	if (!pkt) {
		BT_LOGE("pkt is NULL \n\r");
		return;
	}

	if (!IS_ALIGNED(pkt->buf, 4)) {
		BT_LOGE("rx buf not aligned \n\r");
		goto end;
	}

	if (HCI_EVT == pkt->type && BT_HCI_EVT_CMD_STATUS == (*(pkt->buf))) {  /* command status event */
		memcpy(hci_olt_buf, pkt->buf, pkt->len);
		osif_sem_give(olt_recv_sem);
	}

end:
	hci_rx_pkt_free(pkt);
}

struct hci_transport_cb olt_cb = {
	.recv = olt_recv,
};

void hci_olt_open(void)
{
	if (hci_controller_open()) {
		hci_transport_register(&olt_cb);

		if (false == osif_sem_create(&olt_recv_sem, 0, 1)) {
			BT_LOGE("OLT:olt_recv_sem create fail!\r\n");
			goto failed;
		}
	}

	return;

failed:
	hci_controller_close();
	hci_controller_free();
	return;
}

/*command status*/
bool hci_if_olt_send_status(uint16_t opcode, uint8_t *buf, uint16_t len)
{
	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);

	hci_olt_buf = buf;

	if (len != hci_transport_send(HCI_CMD, buf, len, true)) {
		BT_LOGE("OLT:hci_transport_send fail\n\r");
		return false;
	}

	osif_sem_take(olt_recv_sem, BT_TIMEOUT_FOREVER);
	hci_olt_buf = NULL;

	/* Check Resp: OpCode and Status */
	if (buf[4] != (uint8_t)(opcode >> 0) || buf[5] != (uint8_t)(opcode >> 8) || buf[2] != 0x00) {
		BT_LOGE("check command status[0x%x] fail\n\r", buf[2]);
		return false;
	}

	return true;
}
