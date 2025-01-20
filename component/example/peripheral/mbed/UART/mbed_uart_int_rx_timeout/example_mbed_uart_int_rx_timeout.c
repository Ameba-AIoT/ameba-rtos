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

#define RX_DATA_INTIME	13
#define SRX_BUF_SZ		100
#define RX_TO_MS		1000

//SRAM_NOCACHE_DATA_SECTION
char rx_buf[SRX_BUF_SZ] = {0};

volatile uint32_t tx_busy = 0;
volatile uint32_t rx_done = 0;

void uart_send_string_done(uint32_t id)
{
	(void)id;
	tx_busy = 0;
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

void Release_CPU(void)
{
	// while waitting UART transfer done, try to wakeup other task
#if 1
	// make this task to sleep, so other task can wakeup
	rtos_time_delay_ms(10);
#else
	// force the OS scheduler to do a context switch, but if the
	// priority of this task is the highest then no other task can wake up
	taskYIELD();
#endif
}

void uart_rx_timeout_demo(void)
{
	serial_t sobj;
	int ret;

	sobj.uart_idx = UART_IDX;

	serial_init(&sobj, UART_TX, UART_RX);
	serial_baud(&sobj, UART_BAUD);
	serial_format(&sobj, 8, ParityNone, 1);

	serial_send_comp_handler(&sobj, (void *)uart_send_string_done, (uint32_t) &sobj);

	while (1) {
		// expect to receive maximum 13 bytes with timeout 1000ms
		// If you don't know what is Task Yield or no RTOS, then just keep the last argument is NULL
		// Do Task Yield while waitting UART RX done
		ret = serial_recv_stream_timeout(&sobj, rx_buf, RX_DATA_INTIME, RX_TO_MS, (void *)Release_CPU);


		if (ret < RX_DATA_INTIME) {
			RTK_LOGI(NOTAG, "Serial Rcv Timeout, Got %d bytes\n", ret);
		} else if (ret == RX_DATA_INTIME) {
			RTK_LOGI(NOTAG, "Recv all %d Bytes\n", ret);
			rx_buf[ret] = 0x0; // end of string
			uart_send_string(&sobj, rx_buf);
		}
	}
}

int example_mbed_uart_int_rx_timeout(void)
{
	if (rtos_task_create(NULL, "uart uart_rx_timeout_demo demo", (rtos_task_t)uart_rx_timeout_demo, (void *)NULL, (2048 * 2), (1)) != SUCCESS) {
		RTK_LOGE(NOTAG, "Cannot create uart_rx_timeout_demo test demo task\n");
	}

	//rtos_sched_start();

	return 0;
}