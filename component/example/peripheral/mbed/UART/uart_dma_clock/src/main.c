/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
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

#define OUT_CLK_HZ	40000 /* 40KHz */
#define SRAM_LEN	100 /* 100B -> 100*5 clock periods(f=baudrate/2) */
char sim_clock[SRAM_LEN + 1];

volatile uint32_t is_stop = 0;
static serial_t sobj_clk;

void uart_clock_send_string(serial_t *sobj, char *pstr)
{
	int32_t ret = 0;
	ret = serial_send_stream_dma(sobj, pstr, _strlen(pstr));
	if (ret != 0) {
		printf("%s Error(%ld)\n", __FUNCTION__, ret);
	}
}

void uart_clock_send_string_done(uint32_t id)
{
	serial_t *sobj = (void *)id;
	if (!is_stop) {
		uart_clock_send_string(sobj, sim_clock);
	}
}

void uart_clock_deinit(void)
{
	is_stop = 1;
	serial_free(&sobj_clk);
}

void uart_clock_init(int rate)
{
	int i;

	for (i = 0; i < SRAM_LEN; i++) {
		sim_clock[i] = 0x55;
	}
	sim_clock[SRAM_LEN] = 0;

	sobj_clk.uart_idx = UART_IDX;

	serial_init(&sobj_clk, UART_TX, UART_RX);
	serial_baud(&sobj_clk, rate * 2);
	serial_format(&sobj_clk, 8, ParityNone, 0);

	serial_send_comp_handler(&sobj_clk, (void *)uart_clock_send_string_done, (uint32_t) &sobj_clk);
}

void uart_clock_on(void)
{
	is_stop = 0;
	uart_clock_send_string(&sobj_clk, sim_clock);
}

void uart_clock_off(void)
{
	is_stop = 1;
}

void uart_clock_demo(void)
{
	uart_clock_init(OUT_CLK_HZ);

	while (1) {
		uart_clock_on();
		DelayMs(2000);
		uart_clock_off();
		DelayMs(3000);
	}
}

int main(void)
{
	if (rtos_task_create(NULL, ((const char *)"uart_clock_demo"), (rtos_task_t)uart_clock_demo, NULL, 1024 * 4, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(uart_clock_demo) failed\r\n", __FUNCTION__);
	}

	rtos_sched_start();

	return 0;
}