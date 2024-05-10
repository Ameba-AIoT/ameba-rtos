/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "serial_api.h"
#include "uart_ext.h"
#include "os_wrapper.h"
#include <stdio.h>

#define UART_IDX	0
#define UART_BAUD	38400

volatile char rc = 0;

void uart_send_string(serial_t *sobj, char *pstr)
{
	unsigned int i = 0;
	while (*(pstr + i) != 0) {
		serial_putc(sobj, *(pstr + i));
		i++;
	}
}

void uart_irq(uint32_t id, SerialIrq event)
{
	serial_t *sobj = (void *)id;

	if (event == RxIrq) {
		while (serial_readable(sobj)) {
			rc = serial_getc(sobj);
			serial_putc(sobj, rc);
		}
	}

	if (event == TxIrq && rc != 0) {
		rc = 0;
	}

}

void uart_irq_demo(void)
{
	serial_t sobj;

	sobj.uart_idx = UART_IDX;

	serial_init(&sobj, UART_TX, UART_RX);
	serial_baud(&sobj, UART_BAUD);
	serial_format(&sobj, 8, ParityNone, 1);

	uart_send_string(&sobj, "UART IRQ API Demo...\r\n");
	uart_send_string(&sobj, "Hello World!!!\r\n");
	serial_irq_handler(&sobj, uart_irq, (uint32_t)&sobj);
	serial_irq_set(&sobj, RxIrq, 1);
	serial_irq_set(&sobj, TxIrq, 1);

	while (1);
}

int main(void)
{
	if (rtos_task_create(NULL, ((const char *)"uart_irq_demo"), (rtos_task_t)uart_irq_demo, NULL, 1024 * 4, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(uart_irq_demo) failed\r\n", __FUNCTION__);
	}

	rtos_sched_start();

	return 0;
}
