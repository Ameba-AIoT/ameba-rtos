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
#include "hci_dbg.h"
#include "rtk_coex.h"
#include <string.h>

#define BT_HCI_EVT_LE_META_EVENT         0x3e
#define BT_HCI_EVT_LE_ADVERTISING_REPORT 0x02
#define BT_HCI_EVT_LE_EXT_ADVERTISING_REPORT 0x0d

#if defined (__ICCARM__)
#define __H4_PACKED
#else
#define __H4_PACKED __attribute__ ((packed))
#endif

struct bt_hci_acl_hdr {
	uint16_t handle;
	uint16_t len;
} __H4_PACKED;

struct bt_hci_sco_hdr {
	uint16_t handle;
	uint8_t len;
} __H4_PACKED;

struct bt_hci_evt_hdr {
	uint8_t  evt;
	uint8_t  len;
} __H4_PACKED;

struct bt_hci_iso_hdr {
	uint16_t handle;
	uint16_t len;
} __H4_PACKED;

typedef union {
	struct bt_hci_evt_hdr evt;
	struct bt_hci_acl_hdr acl;
	struct bt_hci_iso_hdr iso;
	struct bt_hci_sco_hdr sco;
} HCI_HDR;

static struct hci_h4_t {
	void    *rx_ind_sema;
	void    *rx_run_sema;
	void    *rx_thread_hdl;
	uint8_t  rx_run;
	HCI_RECV recv;
	HCI_GET_BUF get_buf;
	HCI_FREE_BUF free_buf;
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

static bool h4_get_buf(hci_rx_t *info, uint32_t timeout)
{
	if (hci_h4->get_buf && hci_h4->recv) {
		return hci_h4->get_buf(info, timeout);
	}

	return false;
}

static void h4_rx_thread(void *context)
{
	(void)context;
	hci_rx_t info = {0};
	HCI_HDR *hdr = (HCI_HDR *)info.hdr;
	uint8_t buffer[CONFIG_HCI_RX_BUF_LEN];
	uint8_t type, hdr_len, discardable, sub_event, *buf;
	uint16_t body_len, discard_len;

	osif_sem_give(hci_h4->rx_run_sema);

	while (1) {
		type = H4_NONE;
		discardable = 0;
		sub_event = 0;
		buf = 0;

		memset(&info, 0, sizeof(hci_rx_t));
		/* Read H4 Type */
		if (sizeof(type) != h4_recv_data(&type, sizeof(type))) {
			break;
		}

		if (type == H4_EVT) {
			hdr_len = sizeof(struct bt_hci_evt_hdr);
		} else if (type == H4_ACL) {
			hdr_len = sizeof(struct bt_hci_acl_hdr);
		} else if (type == H4_ISO) {
			hdr_len = sizeof(struct bt_hci_iso_hdr);
		} else if (type == H4_SCO) {
			hdr_len = sizeof(struct bt_hci_sco_hdr);
		} else {
			HCI_ERR("wrong HCI H4 type %d", type);
			break;
		}
		/* Read HCI Header */
		if (hdr_len != h4_recv_data((uint8_t *)hdr, hdr_len)) {
			break;
		}

		if (type == H4_EVT) {
			body_len = hdr->evt.len;
			if (BT_HCI_EVT_LE_META_EVENT == hdr->evt.evt) {
				/* The first event parameter is always a subevent code identifying the specific event for LE meta event.
				   So the len should not be 0. */
				if (body_len == 0) {
					HCI_ERR("HCI meta event length zero");
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
		} else if (type == H4_ACL) {
			body_len = hdr->acl.len;
		} else if (type == H4_ISO) {
			body_len = hdr->iso.len;
		} else { /* if (type == H4_SCO) */
			body_len = hdr->sco.len;
		}

		if (body_len == 0xDEAD) { /* to avoid 0xDEADBEEF received */
			HCI_ERR("ERROR!!!, type %d, len %d", type, body_len);
			break;
		}

		info.type = type;
		info.len = hdr_len + body_len;
		if (!h4_get_buf(&info, discardable ? 0 : BT_TIMEOUT_FOREVER) || !info.data) {
			if (discardable) {
				if (discard_len != h4_recv_data(buffer, discard_len)) { /* only hci event may be discarded, buffer size is enought. */
					break;
				}
				continue;
			} else {
				HCI_ERR("Hci RX alloc fail");
				break;
			}
		}
		buf = info.data;
		memcpy(buf, hdr, hdr_len);
		if (H4_EVT == type && BT_HCI_EVT_LE_META_EVENT == hdr->evt.evt) {
			buf[hdr_len] = sub_event;
			hdr_len++;
			body_len--;
		}

		/* Read HCI Body */
		if (body_len != h4_recv_data(buf + hdr_len, body_len)) {
			if (hci_h4->free_buf) {
				hci_h4->free_buf(&info);
			}
			break;
		}

		bt_coex_process_rx_frame(type, buf, hdr_len + body_len);

		if (hci_h4->recv) {
			hci_h4->recv(&info);
		}
	}

	osif_sem_give(hci_h4->rx_run_sema);

	osif_task_delete(NULL);
}

static void h4_set_recv(HCI_RECV hci_recv)
{
	hci_h4->recv = hci_recv;
}

static void h4_set_buf_ops(HCI_GET_BUF get_buf, HCI_FREE_BUF free_buf)
{
	hci_h4->get_buf = get_buf;
	hci_h4->free_buf = free_buf;
}

static uint16_t h4_send(uint8_t type, uint8_t *buf, uint16_t len, uint8_t is_reserved)
{
	if (type <= H4_NONE || type > H4_ISO) {
		return 0;
	}

	bt_coex_process_tx_frame(type, buf, len);

	if (is_reserved) {
		*(buf - 1) = type;
		/* Caller only send size of 'len' bytes, so return 'len' */
		return (hci_uart_send(buf - 1, len + 1) - 1);
	} else {
		if (sizeof(type) != hci_uart_send(&type, sizeof(type))) {
			return 0;
		}
		return hci_uart_send(buf, len);
	}
}

static uint8_t h4_open(void)
{
	if (!hci_h4) {
		hci_h4 = osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct hci_h4_t));
		if (!hci_h4) {
			return HCI_FAIL;
		}
		memset(hci_h4, 0, sizeof(struct hci_h4_t));
	}

	bt_coex_init();

	osif_sem_create(&hci_h4->rx_ind_sema, 0, 1);
	osif_sem_create(&hci_h4->rx_run_sema, 0, 1);
	hci_h4->rx_run = 1;
	osif_task_create(&hci_h4->rx_thread_hdl, "h4_rx_thread", h4_rx_thread, 0, CONFIG_HCI_RX_STACK_SIZE, CONFIG_HCI_RX_PRIO);
	osif_sem_take(hci_h4->rx_run_sema, BT_TIMEOUT_FOREVER);

	return HCI_SUCCESS;
}

static uint8_t h4_close(void)
{
	if (!hci_h4) {
		return HCI_FAIL;
	}

	hci_h4->rx_run = 0;
	osif_sem_give(hci_h4->rx_ind_sema);
	osif_sem_take(hci_h4->rx_run_sema, BT_TIMEOUT_FOREVER);

	bt_coex_deinit();

	return HCI_SUCCESS;
}

static uint8_t h4_free(void)
{
	if (!hci_h4) {
		return HCI_FAIL;
	}

	osif_sem_delete(hci_h4->rx_run_sema);
	osif_sem_delete(hci_h4->rx_ind_sema);

	if (hci_h4) {
		osif_mem_free(hci_h4);
	}
	hci_h4 = NULL;

	return HCI_SUCCESS;
}

HCI_TRANSPORT_OPS hci_transport_ops = {
	.open        = h4_open,
	.close       = h4_close,
	.free_ops    = h4_free,
	.send        = h4_send,
	.set_recv    = h4_set_recv,
	.set_buf_ops = h4_set_buf_ops,
	.recv_ind    = h4_recv_ind,
};
