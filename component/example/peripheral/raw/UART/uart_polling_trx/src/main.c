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
UART_InitTypeDef UART_InitStruct;

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

void uart_send_string(char *pstr)
{
	u32 i = 0;
	while (*(pstr + i) != 0) {
		while (!UART_Writable(UART_DEV));
		UART_CharPut(UART_DEV, *(pstr + i));
		i++;
	}
}

void uart_trx_demo(void)
{
	u8 rc = '0';
	u32 uart_idx = uart_get_idx(UART_DEV);

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

	uart_send_string("UART API Demo...\r\n");
	uart_send_string("Hello World!!!\r\n");

	while (1) {
		while (!UART_Readable(UART_DEV));
		UART_CharGet(UART_DEV, &rc);

		while (!UART_Writable(UART_DEV));
		UART_CharPut(UART_DEV, rc);
	}

}

int main(void)
{
	if (rtos_task_create(NULL, ((const char *)"uart_trx_demo"), (rtos_task_t)uart_trx_demo, NULL, 1024 * 4, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(uart_trx_demo) failed\r\n", __FUNCTION__);
	}

	rtos_sched_start();

	return 0;
}
