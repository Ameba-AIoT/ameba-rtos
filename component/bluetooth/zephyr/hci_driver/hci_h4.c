/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdbool.h>

#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/drivers/bluetooth/hci_driver.h>

extern int vhci_host_open(void);
extern int vhci_host_send(uint8_t hci_type, uint8_t *pdata, uint16_t len);
extern void vhci_host_close(void);

int hci_recv(uint8_t type, uint8_t *pdata, uint16_t len, bool discardable)
{
	uint8_t buf_type = 0;
	struct net_buf *buf = NULL;

	switch (type) {
	case BT_HCI_H4_EVT:
		buf = bt_buf_get_evt(((struct bt_hci_evt_hdr *)pdata)->evt, discardable, discardable ? K_NO_WAIT : K_FOREVER);
		buf_type = BT_BUF_EVT;
		break;

	case BT_HCI_H4_ACL:
		buf = bt_buf_get_rx(BT_BUF_ACL_IN, K_FOREVER);
		buf_type = BT_BUF_ACL_IN;
		break;

	case BT_HCI_H4_ISO:
		buf = bt_buf_get_rx(BT_BUF_ISO_IN, K_FOREVER);
		buf_type = BT_BUF_ISO_IN;
		break;

	case BT_HCI_H4_SCO:
		buf = bt_buf_get_rx(BT_BUF_SCO_IN, K_FOREVER);
		buf_type = BT_BUF_SCO_IN;
		break;

	default:
		break;
	}

	if (!buf) {
		return -1;
	}

	if (buf->size < len) {
		net_buf_unref(buf);
		return -2;
	}

	btsnoop_send(type, pdata, len, true);

	bt_buf_set_type(buf, buf_type);

	net_buf_add_mem(buf, pdata, len);

	return bt_recv(buf);
}

static int hci_open(void)
{
	if (0 != vhci_host_open()) {
		return -1;
	}

	return 0;
}

// switch zephyr HCI tyep to spec HCI type
static uint8_t get_spec_hci_type_from_zephyr(struct net_buf *buf)
{
	uint8_t type = bt_buf_get_type(buf);

	if (type == BT_BUF_CMD) {
		return BT_HCI_H4_CMD;
	} else if (type == BT_BUF_EVT) {
		return BT_HCI_H4_EVT;
	} else if (type == BT_BUF_ACL_IN || type == BT_BUF_ACL_OUT) {
		return BT_HCI_H4_ACL;
	} else if (type == BT_BUF_ISO_IN || type == BT_BUF_ISO_OUT) {
		return BT_HCI_H4_ISO;
	} else if (type == BT_BUF_SCO_IN || type == BT_BUF_SCO_OUT) {
		return BT_HCI_H4_SCO;
	}

	return 0xFF;
}

static int hci_send(struct net_buf *buf)
{
	int ret_val = 0;
	uint8_t hci_type;

	if (!buf) {
		return -1;
	}

	hci_type = get_spec_hci_type_from_zephyr(buf);

	if (0xFF == hci_type) {
		ret_val = -2;
		goto hci_tx_exit;
	}

	btsnoop_send(hci_type, buf->data, buf->len, false);

	if (0 != vhci_host_send(hci_type, buf->data, buf->len)) {
		ret_val = -4;
		goto hci_tx_exit;
	}

hci_tx_exit:
	net_buf_unref(buf);

	return ret_val;
}

static int hci_close(void)
{
	vhci_host_close();

	return 0;
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

int bt_uart_init(void)
{
	return bt_hci_driver_register(&drv);
}