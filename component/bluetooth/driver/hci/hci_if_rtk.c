/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "osif.h"
#include "hci_if_rtk.h"
#include "hci_transport.h"
#include "hci_uart.h"
#include "hci_platform.h"
#include "bt_debug.h"
#include "dlist.h"

#define H4_HDR_LEN          (1)

#define SECURE_CONTEXT_SIZE (128)

#define HCI_IF_TASK_SIZE    (2*1024)
#define HCI_IF_TASK_PRIO    (5)


#define HCI_IF_TASK_CLOSED 0
#define HCI_IF_TASK_RUNNING 1
#define HCI_IF_TASK_CLOSING 2

struct tx_packet_t {
	struct list_head list;
	uint8_t *buf;
	uint32_t len;
};

static struct {
	HCI_IF_CALLBACK cb;
	uint8_t state;
	struct list_head tx_list;
	void *tx_ind_sem;
	void *tx_list_mtx;
	void *task_hdl;
} hci_if_rtk;

static void rtk_stack_recv(struct hci_rx_packet_t *pkt)
{
	uint8_t *buf, offset = H4_HDR_LEN;

	if (!pkt || !hci_if_rtk.cb) {
		return;
	}

	if (pkt->type == HCI_ACL || pkt->type == HCI_ISO) {
		offset += HCI_H4_RX_ACL_PKT_BUF_OFFSET;
	} else if (pkt->type == HCI_SCO) {
		offset += HCI_H4_RX_SCO_PKT_BUF_OFFSET;
	}

	buf = (uint8_t *)osif_mem_aligned_alloc(RAM_TYPE_DATA_ON, pkt->len + offset, 4);

	memset(buf, 0, offset);
	buf[0] = pkt->type;
	memcpy(buf + offset, pkt->buf, pkt->len);

	/* If indicate OK, stack will call hci_if_confirm when process of the packet is completed. */
	if (!hci_if_rtk.cb(HCI_IF_EVT_DATA_IND, true, buf, pkt->len + offset)) {
		osif_mem_aligned_free(buf);
	}
}

static struct hci_transport_cb rtk_stack_cb = {
	.recv = rtk_stack_recv,
};

static void _hci_if_send(uint8_t *buf, uint32_t len)
{
	uint16_t offset = H4_HDR_LEN;
	if (HCI_ACL == buf[0] || HCI_ISO == buf[0]) {
		offset += HCI_H4_TX_ACL_PKT_BUF_OFFSET;
	}

	hci_transport_send(buf[0], buf + offset, len - offset, true);
	if (hci_if_rtk.cb) {
		hci_if_rtk.cb(HCI_IF_EVT_DATA_XMIT, true, buf, len);
	}
}

static bool _tx_list_add(uint8_t *buf, uint32_t len)
{
	bool ret = false;
	struct tx_packet_t *pkt = NULL;

	/* buf == NULL is exit flag */
	if (hci_if_rtk.state != HCI_IF_TASK_RUNNING && buf) {
		goto end;
	}

	pkt = osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct tx_packet_t));

	if (!pkt) {
		if (hci_if_rtk.cb) {
			hci_if_rtk.cb(HCI_IF_EVT_DATA_XMIT, false, buf, len);
		}
		BT_LOGE("pkt alloc fail!\r\n");
		goto end;
	}

	pkt->buf = buf;
	pkt->len = len;

	osif_mutex_take(hci_if_rtk.tx_list_mtx, BT_TIMEOUT_FOREVER);
	list_add_tail(&pkt->list, &hci_if_rtk.tx_list);
	osif_mutex_give(hci_if_rtk.tx_list_mtx);

	osif_sem_give(hci_if_rtk.tx_ind_sem);
	ret = true;

end:
	return ret;
}

static void hci_if_task(void *context)
{
	(void)context;

	while (true) {
		osif_sem_take(hci_if_rtk.tx_ind_sem, BT_TIMEOUT_FOREVER);
		while (true) {
			struct tx_packet_t *pkt = NULL;

			osif_mutex_take(hci_if_rtk.tx_list_mtx, BT_TIMEOUT_FOREVER);
			if (!list_empty(&hci_if_rtk.tx_list)) {
				pkt = (struct tx_packet_t *)hci_if_rtk.tx_list.next;
				list_del(&pkt->list);
			}
			osif_mutex_give(hci_if_rtk.tx_list_mtx);

			if (!pkt) {
				break;
			}

			if (!pkt->buf) {
				osif_mem_free(pkt);
				goto out;
			}

			_hci_if_send(pkt->buf, pkt->len);
			osif_mem_free(pkt);
		}
	}

out:
	hci_if_rtk.state = HCI_IF_TASK_CLOSED;
	osif_task_delete(NULL);
}

bool hci_if_open(HCI_IF_CALLBACK callback)
{
	if (hci_controller_is_enabled()) {
		BT_LOGD("Hci Driver Already Open!\r\n");
		goto end;
	}

	if (!hci_controller_enable()) {
		return false;
	}

	if (!hci_is_mp_mode()) {
		/* HCI Transport Bridge to RTK Stack */
		hci_transport_register(&rtk_stack_cb);
	}

	memset(&hci_if_rtk, 0, sizeof(hci_if_rtk));
	hci_if_rtk.cb = callback;
	INIT_LIST_HEAD(&hci_if_rtk.tx_list);
	osif_sem_create(&hci_if_rtk.tx_ind_sem, 0, 1);
	osif_mutex_create(&hci_if_rtk.tx_list_mtx);
	osif_task_create(&hci_if_rtk.task_hdl, "hci_if_task", hci_if_task,
					 0, HCI_IF_TASK_SIZE, HCI_IF_TASK_PRIO);
	hci_if_rtk.state = HCI_IF_TASK_RUNNING;

end:
	/* Upperstack will call hci_if_write immediately after this OPEN cb.
	 * Therefore, please make sure hci_if_write is ready here. */
	if (hci_if_rtk.cb) {
		hci_if_rtk.cb(HCI_IF_EVT_OPENED, true, NULL, 0);
	}
	BT_LOGA("Start upper stack\r\n");
	return true;
}

bool hci_if_close(void)
{
	if (!hci_controller_is_enabled()) {
		return true;
	}

	hci_if_rtk.state = HCI_IF_TASK_CLOSING;

	_tx_list_add(NULL, 0);

	while (hci_if_rtk.state != HCI_IF_TASK_CLOSED) {
		osif_delay(5);
	}

	hci_controller_disable();

	osif_sem_delete(hci_if_rtk.tx_ind_sem);
	osif_mutex_delete(hci_if_rtk.tx_list_mtx);

	if (hci_if_rtk.cb) {
		hci_if_rtk.cb(HCI_IF_EVT_CLOSED, true, NULL, 0);
	}

	return true;
}

void hci_if_del_task(void)
{
	hci_if_close();
}

void hci_if_deinit(void)
{
	memset(&hci_if_rtk, 0, sizeof(hci_if_rtk));
	hci_controller_free();
}

/* Stack tx use, indicate to stack when tx done */
bool hci_if_write(uint8_t *buf, uint32_t len)
{
	return _tx_list_add(buf, len);
}

bool hci_if_confirm(uint8_t *buf)
{
	osif_mem_aligned_free(buf);
	return true;
}