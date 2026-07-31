/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include <stdint.h>
#include "serial_api.h"
#include "serial_ex_api.h"
#include "example_uart_ext.h"
#include "os_wrapper.h"

#define UART_IDX	0
#define UART_BAUD	38400
#define UART_BUF_SIZE	1000

static uint8_t s_tx_buf[UART_BUF_SIZE];
static uint8_t s_rx_buf[UART_BUF_SIZE];

serial_t sobj;

void uart_send_string(serial_t *sobj, char *pstr)
{
	unsigned int i = 0;
	while (*(pstr + i) != 0) {
		serial_putc(sobj, *(pstr + i));
		i++;
	}
}

void uart_tx_task(void *param)
{
	uint32_t i;
	(void)param;

	RTK_LOGI(NOTAG, "UART TX task started\n");

	for (i = 0; i < UART_BUF_SIZE; i++) {
		s_tx_buf[i] = 0x30 + (i % 10);
	}

	/* extra sync: peer may still be in sync loop */
	rtos_time_delay_ms(100);
	serial_putc(&sobj, 0xFF);
	rtos_time_delay_ms(200);
	serial_putc(&sobj, 0xFF);
	rtos_time_delay_ms(200);

	RTK_LOGI(NOTAG, "UART TX data send start\n");

	for (i = 0; i < UART_BUF_SIZE; i++) {
		while (!serial_writable(&sobj)) {
			rtos_time_delay_ms(1); /* yield to let lower-priority RX drain FIFO (prevents deadlock) */
		}
		serial_putc(&sobj, s_tx_buf[i]);
		if ((i & 0xFF) == 0) {
			/* busy-wait to starve lower-priority RX task, triggering flow control */
			DelayMs(80);
			rtos_time_delay_ms(20);
			RTK_LOGI(NOTAG, "UART TX sent %d bytes\n", i);
		}
	}

	RTK_LOGI(NOTAG, "UART TX task done\n");
	while (1) {
		rtos_time_delay_ms(1000);
	}
}

void uart_rx_task(void *param)
{
	uint32_t rx_count = 0, errors = 0, i;
	(void)param;

	RTK_LOGI(NOTAG, "UART RX task started\n");

	while (rx_count < UART_BUF_SIZE) {
		if (serial_readable(&sobj)) {
			uint8_t b = (uint8_t)serial_getc(&sobj);
			if (b == 0xFF) {
				continue;
			}
			s_rx_buf[rx_count++] = b;
		} else {
			rtos_time_delay_ms(1);
		}
	}

	for (i = 0; i < UART_BUF_SIZE; i++) {
		if (s_rx_buf[i] != (0x30 + (i % 10))) {
			errors++;
		}
	}

	if (errors) {
		RTK_LOGE(NOTAG, "UART RX done: %d / %d bytes mismatch\n", errors, UART_BUF_SIZE);
	} else {
		RTK_LOGI(NOTAG, "UART RX done: all %d bytes match\n", UART_BUF_SIZE);
	}

	while (1) {
		rtos_time_delay_ms(1000);
	}
}

void uart_auto_flow_ctrl_demo(void)
{
	uint32_t i, retries = 0, sync_count = 0;

	sobj.uart_idx = UART_IDX;

	serial_init(&sobj, UART_TX, UART_RX);
	serial_baud(&sobj, UART_BAUD);
	serial_format(&sobj, 8, ParityNone, 1);
	serial_rx_fifo_level(&sobj, FifoLv1Byte);
	serial_set_flow_control(&sobj, FlowControlNone, 0, 0); // Pin assignment can be ignored when autoflow control function is disabled

	/* sync handshake: send 0xFF until we hear 2 from the peer */
	RTK_LOGI(NOTAG, "UART sync start\n");

	while (1) {
		serial_putc(&sobj, 0xFF);
		rtos_time_delay_ms(200);
		if (sync_count >= 2) {
			break;
		}
		/* poll for incoming sync bytes during wait */
		for (i = 0; i < 20; i++) {
			if (serial_readable(&sobj)) {
				uint8_t b = (uint8_t)serial_getc(&sobj);
				if (b == 0xFF) {
					sync_count++;
					if (sync_count >= 2) {
						break;
					}
				}
			}
			rtos_time_delay_ms(10);
		}
		if (sync_count >= 2) {
			break;
		}
		retries++;
		if (retries > 100) {
			RTK_LOGE(NOTAG, "UART sync timeout\n");
			while (1);
		}
		if ((retries % 5) == 0) {
			RTK_LOGI(NOTAG, "UART sync retry %d\n", retries);
		}
	}

	RTK_LOGI(NOTAG, "UART sync OK\n");

	serial_clear_rx(&sobj);
	/* Enable flow control */
	serial_set_flow_control(&sobj, FlowControlRTSCTS, UART_RTS, UART_CTS);

	RTK_LOGI(NOTAG, "UART creating TX (pri 2) and RX (pri 1) tasks\n");

	if (RTK_SUCCESS != rtos_task_create(NULL, "uart_tx_task",
										(rtos_task_t)uart_tx_task, NULL,
										1024 * 4, 2)) {
		RTK_LOGE(NOTAG, "create tx failed\n");
	}
	if (RTK_SUCCESS != rtos_task_create(NULL, "uart_rx_task",
										(rtos_task_t)uart_rx_task, NULL,
										1024 * 4, 1)) {
		RTK_LOGE(NOTAG, "create rx failed\n");
	}

	while (1) {
		rtos_time_delay_ms(1000);
	}
}

int example_mbed_uart_polling_auto_flow_ctrl(void)
{
	if (rtos_task_create(NULL, ((const char *)"uart_auto_flow_ctrl_demo"), (rtos_task_t)uart_auto_flow_ctrl_demo, NULL, 1024 * 4,
						 1) != RTK_SUCCESS) {
		RTK_LOGE(NOTAG, "%s Create uart_auto_flow_ctrl_demo task failed\n", __FUNCTION__);
	}

	return 0;
}