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

#define SRX_BUF_SZ	8

char rx_buf[SRX_BUF_SZ + 1] = {0}; /* the last data 0x0 represents end of string */
volatile uint32_t tx_busy = 0;
volatile uint32_t rx_done = 0;

void uart_send_string_done(uint32_t id)
{
	(void)id;
	tx_busy = 0;
}

void uart_recv_string_done(uint32_t id)
{
	(void)id;
	rx_done = 1;
}

void uart_send_string(serial_t *sobj, char *pstr)
{
	int32_t ret = 0;

	if (tx_busy) {
		return;
	}
	tx_busy = 1;
	ret = serial_send_stream(sobj, pstr, _strlen(pstr));
	if (ret != 0) {
		RTK_LOGE(NOTAG, "%s Error(%d)\n", __FUNCTION__, ret);
		tx_busy = 0;
	}
}

void uart_stream_irq_demo(void)
{
	serial_t sobj;
	int ret;

	sobj.uart_idx = UART_IDX;

	serial_init(&sobj, UART_TX, UART_RX);
	serial_baud(&sobj, UART_BAUD);
	serial_format(&sobj, 8, ParityNone, 1);

	serial_send_comp_handler(&sobj, (void *)uart_send_string_done, (uint32_t) &sobj);
	serial_recv_comp_handler(&sobj, (void *)uart_recv_string_done, (uint32_t) &sobj);

	ret = serial_recv_stream(&sobj, rx_buf, SRX_BUF_SZ);
	if (ret) {
		RTK_LOGE(NOTAG, "%s: Recv Error(%d)\n", __FUNCTION__, ret);
		rx_done = 1;
	}

	while (1) {
		if (rx_done) {
			uart_send_string(&sobj, rx_buf);
			rx_done = 0;
			ret = serial_recv_stream(&sobj, rx_buf, SRX_BUF_SZ);
			if (ret) {
				RTK_LOGE(NOTAG, "%s: Recv Error(%d)\n", __FUNCTION__, ret);
				rx_done = 1;
			}
		}
	}
}

int example_mbed_uart_int_stream(void)
{
	if (rtos_task_create(NULL, ((const char *)"uart_stream_irq_demo"), (rtos_task_t)uart_stream_irq_demo, NULL, 1024 * 4, 1) != SUCCESS) {
		RTK_LOGE(NOTAG, "%s rtos_task_create(uart_stream_irq_demo) failed\n", __FUNCTION__);
	}

	//rtos_sched_start();

	return 0;
}