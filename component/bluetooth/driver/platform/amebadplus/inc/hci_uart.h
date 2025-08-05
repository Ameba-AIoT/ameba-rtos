/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef _HCI_UART_H_
#define _HCI_UART_H_

uint8_t hci_uart_set_bdrate(uint32_t baudrate);
uint16_t hci_uart_send(uint8_t *buf, uint16_t len);
uint16_t hci_uart_read(uint8_t *buf, uint16_t len);
void hci_uart_rx_irq(bool enable);
uint8_t hci_uart_open(void);
uint8_t hci_uart_close(void);
uint8_t hci_uart_free(void);
void hci_uart_force_rts(bool op);

#endif
