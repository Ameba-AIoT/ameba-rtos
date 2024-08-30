/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "osif.h"
#include "hci_config.h"
#include "hci/hci_common.h"
#include "hci/hci_transport.h"
#include "hci_platform.h"
#include "bt_debug.h"
#include "rtk_coex.h"
#include <string.h>

static struct hci_h4_t {
	void    *rx_ind_sema;
	void    *rx_run_sema;
	void    *tx_mutex;
	void    *rx_thread_hdl;
	uint8_t  rx_run;
	struct hci_transport_cb *cb;
} *hci_h4 = NULL;

static uint8_t h4_recv_ind(void)
{
	if (!hci_h4->rx_run) {
		return HCI_FAIL;
	}

	osif_sem_give(hci_h4->rx_ind_sema);
	return HCI_SUCCESS;
}

static uint16_t h4_recv_data(uint8_t *buf, uint16_t len)
{
	uint16_t read_len = 0;
	uint16_t read_len_per = 0;

	while (1) {
		read_len_per = hci_uart_read(buf + read_len, len - read_len);
		read_len += read_len_per;
		if (read_len == len) {
			break;
		}
		if (0 == read_len_per) {
			osif_sem_take(hci_h4->rx_ind_sema, BT_TIMEOUT_FOREVER);
			if (!hci_h4->rx_run) {
				return 0;
			}
		}
	}

	return read_len;
}

static uint8_t *h4_get_buf(uint8_t type, void *hdr, uint16_t len, uint32_t timeout)
{
	if (hci_h4->cb->get_buf && hci_h4->cb->recv) {
		return hci_h4->cb->get_buf(type, hdr, len, timeout);
	}

	return NULL;
}

static void h4_rx_thread(void *context)
{
	(void)context;
	uint8_t hdr[HCI_HDR_MAX_SIZE];
	uint8_t buffer[CONFIG_HCI_RX_BUF_LEN];
	uint8_t type, hdr_len, discardable, sub_event, *buf;
	uint16_t body_len, discard_len;
	bool is_le_meta_event = false;

	osif_sem_give(hci_h4->rx_run_sema);

	while (1) {
		type = 0;
		discardable = 0;
		sub_event = 0;
		buf = 0;
		is_le_meta_event = false;

		memset(hdr, 0, HCI_HDR_MAX_SIZE);
		/* Read H4 Type */
		if (sizeof(type) != h4_recv_data(&type, sizeof(type))) {
			break;
		}

		hdr_len = hci_get_hdr_len(type);
		if (hdr_len == 0) {
			BT_LOGE("wrong HCI H4 type %d\r\n", type);
			break;
		}

		/* Read HCI Header */
		if (hdr_len != h4_recv_data(hdr, hdr_len)) {
			break;
		}

		body_len = hci_get_body_len(hdr, type);
		if (type == HCI_EVT) {
			if (BT_HCI_EVT_LE_META_EVENT == ((struct hci_evt_hdr *)hdr)->evt) {
				is_le_meta_event = true;
				/* The first event parameter is always a subevent code identifying the specific event for LE meta event.
				   So the len should not be 0. */
				if (body_len == 0) {
					BT_LOGE("HCI meta event length zero\r\n");
					break;
				}

				if (sizeof(sub_event) != h4_recv_data(&sub_event, sizeof(sub_event))) {
					break;
				}

				if (BT_HCI_EVT_LE_ADVERTISING_REPORT == sub_event ||
					BT_HCI_EVT_LE_EXT_ADVERTISING_REPORT == sub_event) {
					discard_len = body_len - 1;
					discardable = 1;
				}
			}
		}

		if (body_len == 0xDEAD) { /* to avoid 0xDEADBEEF received */
			BT_LOGE("ERROR!!!, type %d, len %d\r\n", type, body_len);
			break;
		}

		buf = h4_get_buf(type, hdr, hdr_len + body_len, discardable ? 0 : BT_TIMEOUT_FOREVER);
		if (!buf) {
			if (discardable) {
				if (discard_len != h4_recv_data(buffer, discard_len)) { /* only hci event may be discarded, buffer size is enought. */
					break;
				}
				continue;
			} else {
				BT_LOGE("Hci RX alloc fail\r\n");
				break;
			}
		}

		memcpy(buf, hdr, hdr_len);
		if (is_le_meta_event) {
			buf[hdr_len] = sub_event;
			hdr_len++;
			body_len--;
		}

		/* Read HCI Body */
		if (body_len != h4_recv_data(buf + hdr_len, body_len)) {
			if (hci_h4->cb->cancel) {
				hci_h4->cb->cancel();
			}
			break;
		}

		if (!hci_is_mp_mode()) {
			bt_coex_process_rx_frame(type, buf, hdr_len + body_len);
		}

		if (hci_h4->cb->recv) {
			hci_h4->cb->recv();
		}
	}

	osif_sem_give(hci_h4->rx_run_sema);

	osif_task_delete(NULL);
}

void hci_transport_register(struct hci_transport_cb *cb)
{
	hci_uart_set_rx_ind(NULL);
	hci_h4->cb = cb;
	hci_uart_set_rx_ind(h4_recv_ind);
}

uint16_t hci_transport_send(uint8_t type, uint8_t *buf, uint16_t len, bool has_rsvd_byte)
{
	uint16_t sent = 0;

	if (hci_h4->rx_run != 1) {
		return 0;
	}

	if (type < HCI_CMD || type > HCI_ISO) {
		return 0;
	}

	if (!hci_is_mp_mode()) {
		bt_coex_process_tx_frame(type, buf, len);
	}

	osif_mutex_take(hci_h4->tx_mutex, BT_TIMEOUT_FOREVER);
	if (has_rsvd_byte) {
		*(buf - 1) = type;
		/* Caller only send size of 'len' bytes, so return 'len' */
		sent = hci_uart_send(buf - 1, len + 1) - 1;
	} else {
		if (sizeof(type) == hci_uart_send(&type, sizeof(type))) {
			sent = hci_uart_send(buf, len);
		}
	}
	osif_mutex_give(hci_h4->tx_mutex);

	return sent;
}

uint8_t hci_transport_open(void)
{
	if (!hci_h4) {
		hci_h4 = osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct hci_h4_t));
		if (!hci_h4) {
			return HCI_FAIL;
		}
		memset(hci_h4, 0, sizeof(struct hci_h4_t));
	}

	if (!hci_is_mp_mode()) {
		bt_coex_init();
	}

	osif_sem_create(&hci_h4->rx_ind_sema, 0, 1);
	osif_sem_create(&hci_h4->rx_run_sema, 0, 1);
	osif_mutex_create(&hci_h4->tx_mutex);
	hci_h4->rx_run = 1;
	osif_task_create(&hci_h4->rx_thread_hdl, "h4_rx_thread", h4_rx_thread, 0, CONFIG_HCI_RX_STACK_SIZE, CONFIG_HCI_RX_PRIO);
	osif_sem_take(hci_h4->rx_run_sema, BT_TIMEOUT_FOREVER);

	return HCI_SUCCESS;
}

void hci_transport_close(void)
{
	if (!hci_h4) {
		return;
	}

	hci_h4->rx_run = 0;
	osif_sem_give(hci_h4->rx_ind_sema);
	osif_sem_take(hci_h4->rx_run_sema, BT_TIMEOUT_FOREVER);

	if (!hci_is_mp_mode()) {
		bt_coex_deinit();
	}
}

void hci_transport_free(void)
{
	if (!hci_h4) {
		return;
	}

	osif_sem_delete(hci_h4->rx_run_sema);
	osif_sem_delete(hci_h4->rx_ind_sema);
	osif_mutex_delete(hci_h4->tx_mutex);

	if (hci_h4) {
		osif_mem_free(hci_h4);
	}
	hci_h4 = NULL;
}