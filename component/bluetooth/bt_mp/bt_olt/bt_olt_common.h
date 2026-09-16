
#ifndef _BT_OLT_COMMON_H_
#define _BT_OLT_COMMON_H_

#include <stdio.h>
#include <stdbool.h>

void hci_olt_open(void);
bool hci_if_olt_send_status(uint16_t opcode, uint8_t *buf, uint16_t len);
#endif