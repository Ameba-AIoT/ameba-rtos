/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef _HCI_IF_VHCI_H_
#define _HCI_IF_VHCI_H_

bool bt_controller_hci_open(void);
void bt_controller_hci_send(uint8_t type, uint8_t *pbuf, uint16_t len);
void bt_controller_hci_close(void);

#endif