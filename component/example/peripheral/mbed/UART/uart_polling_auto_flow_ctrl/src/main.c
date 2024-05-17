/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "serial_api.h"
#include "serial_ex_api.h"
#include "uart_ext.h"
#include "os_wrapper.h"
#include <stdio.h>

#define UART_IDX	0
#define UART_BAUD	38400

#define UART_BUF_SIZE	1000
unsigned char buffer[UART_BUF_SIZE];
serial_t sobj;

void uart_send_string(serial_t *sobj, char *pstr)
{
	unsigned int i = 0;
	while (*(pstr + i) != 0) {
		serial_putc(sobj, *(pstr + i));
		i++;
	}
}

void uart_auto_flow_ctrl_demo(void)
{
	char rc;
	int i, j;
	int rx_side = 0;

	sobj.uart_idx = UART_IDX;

	serial_init(&sobj, UART_TX, UART_RX);
	serial_baud(&sobj, UART_BAUD);
	serial_format(&sobj, 8, ParityNone, 1);
	serial_rx_fifo_level(&sobj, FifoLv1Byte);
	serial_set_flow_control(&sobj, FlowControlNone, 0, 0); // Pin assignment can be ignored when autoflow control function is disabled

	for (i = 0; i < 1000; i++) {
		printf("Wait peer ready...\r\n");
		serial_putc(&sobj, i + 1);
		if (serial_readable(&sobj)) {
			rc = (int)serial_getc(&sobj);
			if (rc > i) {
				rx_side = 1;
			} else {
				rx_side = 0;
			}
			break;
		}
		DelayMs(100);
	}

	serial_clear_rx(&sobj);
	// Enable flow control
	serial_set_flow_control(&sobj, FlowControlRTSCTS, UART_RTS, UART_CTS);

	if (rx_side) {
		printf("UART Flow Control: RX ==>\r\n");
		_memset(buffer, 0x0, UART_BUF_SIZE);
		i = 0;
		j = 0;
		while (1) {
			if (serial_readable(&sobj)) {
				buffer[i] = serial_getc(&sobj);
				i++;
				if (i == UART_BUF_SIZE) {
					break;
				}
				if ((i & 0xF) == 0) {
					// Make some delay to cause the RX FIFO full and then trigger flow controll
					DelayMs(100);
					printf("UART RX got %d bytes\r\n", i);
				}
				j = 0;
			} else {
				DelayMs(10);
				j++;
				if (j == 1000) {
					printf("UART RX Failed, Got %d bytes\r\n", i);
					break;
				}
			}
		}
	} else {
		printf("UART Flow Control: TX ==>\r\n");
		DelayMs(500);
		serial_putc(&sobj, 0xFF);
		DelayMs(500);

		for (i = 0; i < UART_BUF_SIZE; i++) {
			buffer[i] = 0x30 + (i % 10);
		}
		for (i = 0; i < UART_BUF_SIZE; i++) {
			serial_putc(&sobj, buffer[i]);
		}
	}
	printf("UART Flow Control Test Done!\r\n");
	while (1);
}

int main(void)
{
	if (rtos_task_create(NULL, ((const char *)"uart_auto_flow_ctrl_demo"), (rtos_task_t)uart_auto_flow_ctrl_demo, NULL, 1024 * 4,
						 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(uart_auto_flow_ctrl_demo) failed\r\n", __FUNCTION__);
	}

	rtos_sched_start();

	return 0;
}