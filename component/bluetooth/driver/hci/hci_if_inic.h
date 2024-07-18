/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef _HCI_IF_INIC_H_
#define _HCI_IF_INIC_H_

#include "basic_types.h"


bool hci_if_open(void);
bool hci_if_close(void);
void hci_if_send(u8 type, u8 *buf, u32 len);

#endif
// #endif

// #endif