/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "uart_ext.h"
#include "os_wrapper.h"
#include <stdio.h>

#define UART_DEV	UART0_DEV
#define UART_BAUD	38400

#define DMA_TX_BURST_SIZE	8
#define DMA_RX_BURST_SIZE	16 // RX_BURST_SIZE >= RX GDMA_SrcMsize
#define DMA_RX_MAX_BYTE		16

#define CACHE_LINE_SIZE_	32
/* NOTE: DMA buffer size must be a multiple of 32 bytes with D-Cache enabled */
#define SRX_BUF_SZ			2 *(CACHE_LINE_SIZE_)
/* NOTE: DMA buffer must be 32-byte aligned with D-Cache enabled */
char rx_buf[SRX_BUF_SZ]__attribute__((aligned(32))) = {0};

volatile u32 uart_idx;
volatile u32 tx_busy = 0;
volatile u32 rx_done = 0;
UART_InitTypeDef UART_InitStruct;
GDMA_InitTypeDef GDMA_InitStruct;

#if defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS)
const u8 UART_TX_FID[MAX_UART_INDEX] = {
	PINMUX_FUNCTION_UART0_TXD,
	PINMUX_FUNCTION_UART1_TXD,
	PINMUX_FUNCTION_UART2_TXD,
#if defined (CONFIG_AMEBALITE)
	PINMUX_FUNCTION_UART3_TXD
#endif
};

const u8 UART_RX_FID[MAX_UART_INDEX] = {
	PINMUX_FUNCTION_UART0_RXD,
	PINMUX_FUNCTION_UART1_RXD,
	PINMUX_FUNCTION_UART2_RXD,
#if defined (CONFIG_AMEBALITE)
	PINMUX_FUNCTION_UART3_RXD
#endif
};
#endif

u32 uart_get_idx(UART_TypeDef *UartDEV)
{
	u32 i;

	for (i = 0; i < MAX_UART_INDEX; i++) {
		if (UartDEV == UART_DEV_TABLE[i].UARTx) {
			return i;
		}
	}

	return 0xFF;
}

void dma_free(void)
{
	GDMA_Cmd(GDMA_InitStruct.GDMA_Index, GDMA_InitStruct.GDMA_ChNum, DISABLE);
	GDMA_ClearINT(GDMA_InitStruct.GDMA_Index, GDMA_InitStruct.GDMA_ChNum);
	GDMA_ChnlFree(GDMA_InitStruct.GDMA_Index, GDMA_InitStruct.GDMA_ChNum);
	UART_TXDMACmd(UART_DEV, DISABLE);
	UART_RXDMACmd(UART_DEV, DISABLE);
}

u32 uart_send_string_done(void *data)
{
	UNUSED(data);
	dma_free();
	tx_busy = 0;

	return 0;
}

u32 uart_recv_string_done(void *data)
{
	UNUSED(data);

	/*!!!To solve the cache consistency problem, DMA mode need it!!!*/
	DCache_Invalidate((u32)rx_buf, SRX_BUF_SZ);
	dma_free();
	rx_done = 1;

	return 0;
}

void uart_dma_send(char *pstr, u32 len)
{
	BOOL ret;

	UART_TXDMAConfig(UART_DEV, DMA_TX_BURST_SIZE);
	UART_TXDMACmd(UART_DEV, ENABLE);

	ret = UART_TXGDMA_Init(uart_idx, &GDMA_InitStruct, UART_DEV, (IRQ_FUN)uart_send_string_done, (u8 *)pstr, len);

	if (!ret) {
		printf("%s Error(%d)\n", __FUNCTION__, ret);
		tx_busy = 0;
	}
}

BOOL uart_dma_recv(char *pstr, u32 len)
{
	BOOL ret;

	UART_INTConfig(UART_DEV, (RUART_BIT_ERBI | RUART_BIT_ELSI), DISABLE);
	UART_RXDMAConfig(UART_DEV, DMA_RX_BURST_SIZE);
	UART_RXDMACmd(UART_DEV, ENABLE);

	ret = UART_RXGDMA_Init(uart_idx, &GDMA_InitStruct, UART_DEV, (IRQ_FUN)uart_recv_string_done, (u8 *)pstr, len);
	return ret;
}

void uart_send_string(char *pstr)
{
	if (tx_busy) {
		return;
	}
	tx_busy = 1;
	uart_dma_send(pstr, _strlen(pstr));
}

void uart_dma_demo(void)
{
	BOOL ret;
	u32 i = 0;
	u32 len = 13;

	uart_idx = uart_get_idx(UART_DEV);
	if (0xFF == uart_idx) {
		return;
	}

	/* enable uart clock and function */
	RCC_PeriphClockCmd(APBPeriph_UARTx[uart_idx], APBPeriph_UARTx_CLOCK[uart_idx], ENABLE);

#if defined (CONFIG_AMEBASMART)
	/* Configure UART0 TX and RX pin */
	Pinmux_Config(UART_TX, PINMUX_FUNCTION_UART);
	Pinmux_Config(UART_RX, PINMUX_FUNCTION_UART);
#elif defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS)
	/* Configure UART0 TX and RX pin */
	Pinmux_Config(UART_TX, UART_TX_FID[uart_idx]);
	Pinmux_Config(UART_RX, UART_RX_FID[uart_idx]);
#endif

	PAD_PullCtrl(UART_TX, GPIO_PuPd_UP); // pull up Tx/Rx pin
	PAD_PullCtrl(UART_RX, GPIO_PuPd_UP); // pull up Tx/Rx pin

	UART_StructInit(&UART_InitStruct);
	UART_InitStruct.Parity = RUART_PARITY_DISABLE;
	UART_Init(UART_DEV, &UART_InitStruct);
	UART_SetBaud(UART_DEV, UART_BAUD);
	UART_RxCmd(UART_DEV, ENABLE);

	DelayMs(1000);
	printf("Ready to receive %lu-byte-data...\r\n", len);
	ret = uart_dma_recv(rx_buf, len);

	if (!ret) {
		printf(" %s: Recv Error(%d)\n", __FUNCTION__, ret);
		rx_done = 1;
	}

	while (1) {
		/* Wait for inputing x character to initiate DMA.*/
		if (rx_done) {
			rx_done = 0;

			rx_buf[len] = 0;
			DCache_Clean((u32)rx_buf, SRX_BUF_SZ);

			uart_send_string(rx_buf);
			while (tx_busy);

			/* data size: 2Byte ~ 15Byte */
			len = i % (DMA_RX_MAX_BYTE - 2) + 2;
			i++;
			printf("Receive %lu-byte-data\r\n", len);
			ret = uart_dma_recv(rx_buf, len);

			if (!ret) {
				printf(" %s: Recv Error(%d)\n", __FUNCTION__, ret);
				rx_done = 1;
			}
		}
	}
}

int main(void)
{
	if (SUCCESS != rtos_task_create(NULL, "uart_dma_demo", (rtos_task_t)uart_dma_demo, (void *)NULL, (512 * 6), (1))) {
		printf("Create uart_dma_demo Err!!\r\n");
	}

	rtos_sched_start();

	return 0;
}