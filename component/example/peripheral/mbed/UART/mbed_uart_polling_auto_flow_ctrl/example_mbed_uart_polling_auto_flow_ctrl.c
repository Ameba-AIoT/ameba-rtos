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
#include "example_uart_ext.h"
#include "os_wrapper.h"

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
		RTK_LOGI(NOTAG, "Wait peer ready...\n");
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
		RTK_LOGI(NOTAG, "UART Flow Control: RX ==>\n");
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
					RTK_LOGI(NOTAG, "UART RX got %d bytes\n", i);
				}
				j = 0;
			} else {
				DelayMs(10);
				j++;
				if (j == 1000) {
					RTK_LOGE(NOTAG, "UART RX Failed, Got %d bytes\n", i);
					break;
				}
			}
		}
	} else {
		RTK_LOGI(NOTAG, "UART Flow Control: TX ==>\n");
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
	RTK_LOGI(NOTAG, "UART Flow Control Test Done!\n");
	while (1);
}

int example_mbed_uart_polling_auto_flow_ctrl(void)
{
	if (rtos_task_create(NULL, ((const char *)"uart_auto_flow_ctrl_demo"), (rtos_task_t)uart_auto_flow_ctrl_demo, NULL, 1024 * 4,
						 1) != SUCCESS) {
		RTK_LOGE(NOTAG, "%s rtos_task_create(uart_auto_flow_ctrl_demo) failed\n", __FUNCTION__);
	}

	//rtos_sched_start();

	return 0;
}