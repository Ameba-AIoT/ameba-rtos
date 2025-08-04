/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "basic_types.h"

typedef enum {
	HCI_IF_EVT_OPENED,     /* hci I/F open completed */
	HCI_IF_EVT_CLOSED,     /* hci I/F close completed */
	HCI_IF_EVT_DATA_IND,   /* hci I/F rx data indicated */
	HCI_IF_EVT_DATA_XMIT,  /* hci I/F tx data transmitted */
	HCI_IF_EVT_ERROR,      /* hci I/F error occurred */
} HCI_IF_EVT;

typedef bool (*HCI_IF_CALLBACK)(HCI_IF_EVT evt, bool status, uint8_t *buf, uint32_t len);

bool hci_if_open(HCI_IF_CALLBACK callback)
{
	(void)callback;
	return false;
}

bool hci_if_close(void)
{
	return false;
}

void hci_if_del_task(void)
{
	hci_if_close();
}

void hci_if_deinit(void)
{

}

bool hci_if_write(uint8_t *buf, uint32_t len)
{
	(void)buf;
	(void)len;
	return false;
}

bool hci_if_confirm(uint8_t *buf)
{
	(void)buf;
	return false;
}