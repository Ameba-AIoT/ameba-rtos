/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef _HCI_IF_DTM_H_
#define _HCI_IF_DTM_H_

#include <stdio.h>
#include <stdbool.h>

void hci_dtm_open(void);
void hci_dtm_close(void);
bool hci_dtm_send(uint16_t opcode, uint8_t *buf, uint16_t len);

#endif