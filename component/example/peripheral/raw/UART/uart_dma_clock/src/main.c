/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "uart_ext.h"
#include "os_wrapper.h"
#include <stdio.h>

#define UART_DEV	UART0_DEV
#define OUT_CLK_HZ	40000 /* 40KHz */
#define SRAM_LEN	100 /* 100B -> 100*5 clock periods(f=baudrate/2) */
u8 sim_clock[SRAM_LEN];

#if defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS)
const u8 UART_TX_FID[MAX_UART_INDEX] = {
	PINMUX_FUNCTION_UART0_TXD,
	PINMUX_FUNCTION_UART1_TXD,
	PINMUX_FUNCTION_UART2_TXD,
#if defined (CONFIG_AMEBALITE)
	PINMUX_FUNCTION_UART3_TXD
#endif
};
#endif

volatile u32 uart_idx;
volatile u32 is_stop = 0;
UART_InitTypeDef  UART_InitStruct;
GDMA_InitTypeDef GDMA_InitStruct;

void uart_clock_send_string_done(void);

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
}

void uart_clock_send_string(UART_TypeDef *UartDEV, u8 *pstr)
{
	u32 ret = 0;

	UART_TXDMAConfig(UartDEV, 8);
	UART_TXDMACmd(UartDEV, ENABLE);
	ret = UART_TXGDMA_Init(uart_idx, &GDMA_InitStruct, UART_DEV, (IRQ_FUN)uart_clock_send_string_done, pstr, SRAM_LEN);

	if (ret == 0) {
		printf("%s Error(%lu)\n", __FUNCTION__, ret);
	}
}

void uart_clock_send_string_done(void)
{
	dma_free();
	if (!is_stop) {
		uart_clock_send_string(UART_DEV, sim_clock);
	}
}

void uart_clock_init(u32 rate)
{
	u32 i;

	uart_idx = uart_get_idx(UART_DEV);
	if (0xFF == uart_idx) {
		return;
	}

	/* enable uart clock and function */
	RCC_PeriphClockCmd(APBPeriph_UARTx[uart_idx], APBPeriph_UARTx_CLOCK[uart_idx], ENABLE);

#if defined (CONFIG_AMEBASMART)
	/* Configure UART0 TX and RX pin */
	Pinmux_Config(UART_TX, PINMUX_FUNCTION_UART);
#elif defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS)
	/* Configure UART0 TX and RX pin */
	Pinmux_Config(UART_TX, UART_TX_FID[uart_idx]);
#endif

	PAD_PullCtrl(UART_TX, GPIO_PuPd_UP); // pull up Tx pin

	for (i = 0; i < SRAM_LEN; i++) {
		sim_clock[i] = 0x55;
	}

	UART_StructInit(&UART_InitStruct);
	UART_InitStruct.Parity = RUART_PARITY_DISABLE;
	UART_Init(UART_DEV, &UART_InitStruct);
	UART_SetBaud(UART_DEV, rate * 2);
	UART_RxCmd(UART_DEV, ENABLE);
}

void uart_clock_on(void)
{
	is_stop = 0;
	uart_clock_send_string(UART_DEV, sim_clock);
}

void uart_clock_off(void)
{
	is_stop = 1;
	UART_INTConfig(UART_DEV, RUART_BIT_ETBEI, DISABLE);
	UART_ClearTxFifo(UART_DEV);
}

void uart_clock_demo(void)
{
	uart_clock_init(OUT_CLK_HZ);

	while (1) {
		uart_clock_on();
		/*set clock time*/
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