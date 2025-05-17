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
	ret = serial_send_stream_dma(sobj, pstr, _strlen(pstr));
	if (ret != 0) {
		RTK_LOGE(NOTAG, "%s Error(%d)\n", __FUNCTION__, ret);
		tx_busy = 0;
	}
}

void uart_stream_dma(void)
{
	serial_t sobj;
	int i = 0;
	int ret;
	int len = 2;

	sobj.uart_idx = UART_IDX;

	serial_init(&sobj, UART_TX, UART_RX);
	serial_baud(&sobj, UART_BAUD);
	serial_format(&sobj, 8, ParityNone, 1);

	serial_send_comp_handler(&sobj, (void *)uart_send_string_done, (uint32_t) &sobj);
	serial_recv_comp_handler(&sobj, (void *)uart_recv_string_done, (uint32_t) &sobj);

	RTK_LOGI(NOTAG, "Ready to receive %d-byte-data...\n", len);

	ret = serial_recv_stream_dma(&sobj, rx_buf, len);

	if (ret) {
		RTK_LOGE(NOTAG, "%s: Recv Error(%d)\n", __FUNCTION__, ret);
		rx_done = 1;
	}

	while (1) {
		if (rx_done) {
			rx_buf[len] = 0; // end of string
			DCache_Clean((u32)rx_buf, SRX_BUF_SZ);

			uart_send_string(&sobj, rx_buf);
			rx_done = 0;

			/* data size: 2Byte ~ 33Byte */
			len = i % CACHE_LINE_SIZE_ + 2;
			i++;

			/* Wait for inputing [len] characters to initiate DMA. */
			RTK_LOGI(NOTAG, "Ready to receive %d-byte-data...\n", len);

			ret = serial_recv_stream_dma(&sobj, rx_buf, len);

			if (ret) {
				RTK_LOGI(NOTAG, " %s: Recv Error(%d)\n", __FUNCTION__, ret);
				rx_done = 1;
			}

		}
	}
}

int example_mbed_uart_dma_stream(void)
{
	if (RTK_SUCCESS != rtos_task_create(NULL, "uart_stream_dma", (rtos_task_t)uart_stream_dma, (void *)NULL, 3072, (1))) {
		RTK_LOGE(NOTAG, "Create uart_stream_dma Err!!!\n");
	}

	//rtos_sched_start();

	return 0;
}