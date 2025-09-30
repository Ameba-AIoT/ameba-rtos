/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef _HCI_IF_EQC_H_
#define _HCI_IF_EQC_H_

#include <stdio.h>
#include <stdbool.h>
#include "hci_controller.h"

typedef struct {
	uint16_t len;
	void *buf;
} E_EVENT_MSG;

void hci_eqc_open(void);
void hci_eqc_close(void);
bool hci_if_eqc_send_comple(uint16_t opcode, uint8_t *buf, uint16_t len);
bool hci_if_eqc_send_status(uint16_t opcode, uint8_t *buf, uint16_t len);

#endif