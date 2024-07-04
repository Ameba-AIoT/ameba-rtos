/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef _HCI_UART_H_
#define _HCI_UART_H_

#include "hci/hci_transport.h"

typedef uint8_t (*HCI_RECV_IND)(void);

uint8_t hci_uart_set_bdrate(uint32_t baudrate);
uint8_t hci_uart_set_rx_ind(HCI_RECV_IND rx_ind);
uint16_t hci_uart_send(uint8_t *buf, uint16_t len);
uint16_t hci_uart_read(uint8_t *buf, uint16_t len);
uint8_t hci_uart_open(void);
uint8_t hci_uart_close(void);
uint8_t hci_uart_free(void);

#endif
