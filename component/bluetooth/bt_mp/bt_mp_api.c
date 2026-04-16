/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "bt_debug.h"
#include <platform_autoconf.h>
#if defined(CONFIG_SDN_BT) && CONFIG_SDN_BT
#include "sdn_host.h"
#include "hci_common.h"
#else
#include "hci_transport.h"
#include "hci_controller.h"
#include "hci_uart.h"
#endif

#if defined(CONFIG_SDN_BT) && CONFIG_SDN_BT
enum {
	ST_H4_IDLE = 0,    /* Waiting for packet type. */
	ST_H4_HDR,         /* Receiving packet header. */
	ST_H4_PAYLOAD,     /* Receiving packet payload. */
	ST_H4_DISCARD,     /* Dropping packet. */
};

#define HCI_H4_HDR_MAX_SIZE    4
#define HCI_H4_BODY_MAX_SIZE   1024
static struct h4_rx {
	uint8_t rx_state;
	uint8_t type;
	uint16_t remaining;
	uint8_t hdr_len;
	uint16_t body_len;
	uint8_t buf[HCI_H4_HDR_MAX_SIZE + HCI_H4_BODY_MAX_SIZE];
} *h4 = NULL;

void sdn_bqb_h4_rx(uint8_t ch)
{
	switch (h4->rx_state) {
	case ST_H4_IDLE: {
		/* Read H4 Type */
		h4->type = ch;
		h4->hdr_len = hci_get_hdr_len(h4->type);
		if (h4->hdr_len) {
			h4->remaining = h4->hdr_len;
			h4->rx_state = ST_H4_HDR;
		}
		break;
	}
	case ST_H4_HDR: {
		h4->buf[h4->hdr_len - h4->remaining] = ch;
		h4->remaining--;
		if (h4->remaining == 0) {
			h4->body_len = hci_get_body_len(h4->buf, h4->type);
			if (h4->body_len == 0) { /* some cmd has no param */
				sdn_host_send(SDN_INTF_BT, h4->type, h4->buf, h4->hdr_len);
				h4->rx_state = ST_H4_IDLE;
				break;
			}

			h4->remaining = h4->body_len;
			h4->rx_state = ST_H4_PAYLOAD;
		}
		break;
	}
	case ST_H4_PAYLOAD: {
		h4->buf[h4->hdr_len + h4->body_len - h4->remaining] = ch;
		h4->remaining--;
		if (h4->remaining == 0) {
			sdn_host_send(SDN_INTF_BT, h4->type, h4->buf, h4->hdr_len + h4->body_len);
			h4->rx_state = ST_H4_IDLE;
		}
		break;
	}
	default:
		break;
	}
}
#endif

/**
 * @brief     BT power on for MP test.
 * @param     None
 * @return    None
 */
void rtk_bt_mp_power_on(void)
{
	hci_set_mp(true);
#if defined(CONFIG_SDN_BT) && CONFIG_SDN_BT
	if (!h4) {
		h4 = rtos_mem_zmalloc(sizeof(struct h4_rx));
	}
	sdn_host_enable(SDN_INTF_IPC, SDN_INTF_BT);
#else
	if (hci_controller_open()) {
		BT_LOGA("Patch download End!\r\n");
		BT_LOGA("After download patch, deinit HCI driver & HCI uart!\r\n");
		/* In order to keep controller powerd on, do not use hci_controller_close() */
		hci_uart_close();
		hci_transport_close();
		hci_controller_free();
	}
#endif
}

/**
 * @brief     BT power off for MP test.
 * @param     None
 * @return    None
 */
void rtk_bt_mp_power_off(void)
{
#if defined(CONFIG_SDN_BT) && CONFIG_SDN_BT
	sdn_host_disable(SDN_INTF_BT);
	rtos_mem_free(h4);
	h4 = NULL;
#else
	/* just power off controller, uart & transport are already closed & freed */
	hci_controller_close();
#endif
	hci_set_mp(false);
}