/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdbool.h>
#include "hci_common.h"
#include "hci_if_zephyr_sdn.h"
#include "hci_transport.h"
#include "hci_platform.h"
#include "bt_debug.h"
#include <zephyr/drivers/bluetooth/hci_driver.h>
#include <zephyr_log.h>

static uint8_t _get_type(struct net_buf *buf)
{
	uint8_t type = bt_buf_get_type(buf);

	if (type == BT_BUF_CMD) {
		return HCI_CMD;
	} else if (type == BT_BUF_EVT) {
		return HCI_EVT;
	} else if (type == BT_BUF_ACL_IN || BT_BUF_ACL_OUT) {
		return HCI_ACL;
	} else if (type == BT_BUF_ISO_IN || BT_BUF_ISO_OUT) {
		return HCI_ISO;
	} else if (type == BT_BUF_SCO_IN || BT_BUF_SCO_OUT) {
		return HCI_ISO;
	}

	return 0xFF;
}

uint8_t *zephyr_get_buf(void **packet, uint8_t type, void *hdr, uint16_t len, uint32_t timeout)
{
	struct net_buf *buf = NULL;

	if (type == HCI_EVT) {
		buf = bt_buf_get_evt(((struct bt_hci_evt_hdr *)hdr)->evt, timeout == 0, Z_TIMEOUT_TICKS(z_ms_to_systime(timeout)));
		if (!buf) {
			return NULL;
		}
		bt_buf_set_type(buf, BT_BUF_EVT);
	} else if (type == HCI_ACL) {
		buf = bt_buf_get_rx(BT_BUF_ACL_IN, K_FOREVER);
		bt_buf_set_type(buf, BT_BUF_ACL_IN);
	} else if (type == HCI_ISO) {
		buf = bt_buf_get_rx(BT_BUF_ISO_IN, K_FOREVER);
		bt_buf_set_type(buf, BT_BUF_ISO_IN);
	} else if (type == HCI_SCO) {
		buf = bt_buf_get_rx(BT_BUF_SCO_IN, K_FOREVER);
		bt_buf_set_type(buf, BT_BUF_SCO_IN);
	}

	if (buf->size < len) {
		net_buf_unref(buf);
		return NULL;
	}

	*packet = (void *)buf;
	return net_buf_add(buf, len);
}

void zephyr_recv(void *packet)
{
	struct net_buf *buf = packet;

	//btsnoop_send(_get_type(buf), buf->data, buf->len, true);
	bt_recv(buf);
}

static int hci_open(void)
{
	if (!bt_controller_hci_open()) {
		return -1;
	}

	return 0;
}

static int hci_send(struct net_buf *buf)
{
	uint8_t type = _get_type(buf);

	// BT_LOGA("hci_send type %d\r\n", type);
	// BT_DUMPA("", buf->data, buf->len);
	bt_controller_hci_send(type, buf->data, buf->len);

	//btsnoop_send(type, buf->data, buf->len, false);
	net_buf_unref(buf);

	return 0;
}

static int hci_close(void)
{
	bt_controller_hci_close();
	return 0;
}

bool hci_if_write_internal(uint8_t *buf, uint32_t len)
{
	(void)len;
	struct net_buf *net_buf;
	uint16_t opcode;
	uint8_t param_len;

	if (buf[0] != HCI_CMD) {
		return false;
	}

	opcode = (buf[2] << 8) | buf[1];
	param_len = buf[3];

	net_buf = bt_hci_cmd_create(opcode, param_len);
	if (!net_buf) {
		return false;
	}

	if (param_len) {
		net_buf_add_mem(net_buf, buf + 4, param_len);
	}

	if (bt_hci_cmd_send(opcode, net_buf)) {
		return false;
	}

	return true;
}

static const struct bt_hci_driver drv = {
	.name       = "H:4",
	.bus        = BT_HCI_DRIVER_BUS_VIRTUAL,
	.open       = hci_open,
	.send       = hci_send,
	.close      = hci_close,
#if defined(CONFIG_BT_QUIRK_NO_RESET)
	.quirks = BT_QUIRK_NO_RESET,
#endif
};

bool is_stack_never_enabled(void);
int bt_uart_init(void)
{
	if (is_stack_never_enabled()) {
		return bt_hci_driver_register(&drv);
	}

	return 0;
}
