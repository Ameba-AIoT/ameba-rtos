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
#define RX_TO_MS	500

#define CACHE_LINE_SIZE_	32
/* NOTE: DMA buffer size must be a multiple of 32 bytes with D-Cache enabled */
#define SRX_BUF_SZ			2 *(CACHE_LINE_SIZE_)
/* NOTE: DMA buffer must be 32-byte aligned with D-Cache enabled */
char rx_buf[SRX_BUF_SZ]__attribute__((aligned(32))) = {0};

volatile uint32_t tx_busy = 0;
volatile uint32_t rx_done = 0;

void uart_send_string_done(uint32_t id)
{
	(void)id;
	tx_busy = 0;

	_memset((void *)rx_buf, 0, SRX_BUF_SZ);
	DCache_Clean((u32)rx_buf, SRX_BUF_SZ);
}

void uart_recv_string_done(uint32_t id)
{
	(void)id;
	rx_done = 1;

	DCache_Invalidate((u32)rx_buf, SRX_BUF_SZ);
}

void uart_send_string(serial_t *sobj, char *pstr)
{
	int32_t ret = 0;

	if (tx_busy) {
		return;
	}

	tx_busy = 1;
	ret = serial_send_stream_dma(sobj, pstr, _strlen(pstr));
	if (ret != 0) {
		printf("%s Error(%ld)\n", __FUNCTION__, ret);
		tx_busy = 0;
	}
}

void uart_stream_rx_dma_flow_ctrl_demo(void)
{
	serial_t sobj;
	int ret;

	sobj.uart_idx = UART_IDX;

	serial_init(&sobj, UART_TX, UART_RX);
	serial_baud(&sobj, UART_BAUD);
	serial_format(&sobj, 8, ParityNone, 1);

	serial_send_comp_handler(&sobj, (void *)uart_send_string_done, (uint32_t) &sobj);
	serial_recv_comp_handler(&sobj, (void *)uart_recv_string_done, (uint32_t) &sobj);

	ret = serial_recv_stream_dma_timeout(&sobj, rx_buf, 0, RX_TO_MS, NULL);

	if (ret) {
		printf(" %s: Recv Error(%d)\n", __FUNCTION__, ret);
		rx_done = 1;
	}

	while (1) {
		if (rx_done) {
			uart_send_string(&sobj, rx_buf);
			rx_done = 0;

			/* uart is flow controller and you can set rx timeout (unit is ms)*/
			ret = serial_recv_stream_dma_timeout(&sobj, rx_buf, 0, RX_TO_MS, NULL);

			if (ret) {
				printf(" %s: Recv Error(%d)\n", __FUNCTION__, ret);
				rx_done = 1;
			}
		}
	}
}

int main(void)
{
	if (SUCCESS != rtos_task_create(NULL, "uart_stream_rx_dma_flow_ctrl_demo", (rtos_task_t)uart_stream_rx_dma_flow_ctrl_demo, (void *)NULL, 3072,
									(1))) {
		printf("Create uart_stream_rx_dma_flow_ctrl_demo Err!!\r\n");
	}

	rtos_sched_start();

	return 0;
}