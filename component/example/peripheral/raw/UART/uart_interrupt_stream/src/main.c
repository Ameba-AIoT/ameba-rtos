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
#define SRX_BUF_SZ	8

char rx_buf[SRX_BUF_SZ + 1] = {0}; /* the last byte 0x0 represents end of string */
volatile u32 tx_busy = 0;
volatile u32 rx_done = 0;

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

typedef struct {
	UART_InitTypeDef  UART_InitStruct;
	u32 TxCount; // TX data length (Unit: Bytes)
	u32 RxCount; // RX data length (Unit: Bytes)
	char *pTxBuf;
	char *pRxBuf;
	u8 UartIndex;
} UART_OBJ;

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

void uart_send_string_done(void)
{
	tx_busy = 0;
	UART_INTConfig(UART_DEV, RUART_BIT_ETBEI, DISABLE);
}

void uart_recv_string_done(void)
{
	rx_done = 1;
	UART_INTConfig(UART_DEV, RUART_BIT_ERBI | RUART_BIT_ETOI, DISABLE);
}

u32 uart_irq(void *data)
{
	UART_OBJ *uart_obj = (UART_OBJ *)data;
	u32 reg_lsr;
	u32 TransCnt = 0;

	reg_lsr = UART_LineStatusGet(UART_DEV);

	/* tx FIFO empty */
	if (reg_lsr & RUART_BIT_TX_EMPTY) {
		while (uart_obj->TxCount > 0 && UART_Writable(UART_DEV)) {
			UART_CharPut(UART_DEV, *uart_obj->pTxBuf);
			uart_obj->TxCount --;
			uart_obj->pTxBuf ++;
		}

		/* TX_EMPTY: unmask status before amebagreen2 */
		if (0 == uart_obj->TxCount && 1 == tx_busy) {
			uart_send_string_done();
		}
	}

	/* rx FIFO not empty and rx timeout */
	if (reg_lsr & RUART_BIT_RXFIFO_INT || reg_lsr & RUART_BIT_TIMEOUT_INT) {
		TransCnt = UART_ReceiveDataTO(UART_DEV, (u8 *)uart_obj->pRxBuf, uart_obj->RxCount, 1);
		uart_obj->RxCount -= TransCnt;
		uart_obj->pRxBuf += TransCnt;

		if (0 == uart_obj->RxCount) {
			uart_recv_string_done();
		}

		/* clear timeout interrupt flag */
		UART_INT_Clear(UART_DEV, RUART_BIT_TOICF);
	}

	/* rx error */
	if (reg_lsr & UART_ALL_RX_ERR) {
		printf("%s: LSR interrupt\r\n", __FUNCTION__);

		UART_INT_Clear(UART_DEV, RUART_BIT_RLSICF);
	}

	return 0;
}

void uart_int_send(UART_OBJ *uart_obj, char *pstr, u32 len)
{
	uart_obj->pTxBuf = pstr;
	uart_obj->TxCount = len;
	UART_INTConfig(UART_DEV, RUART_BIT_ETBEI, ENABLE);
}

void uart_int_recv(UART_OBJ *uart_obj, char *pstr, u32 len)
{
	uart_obj->pRxBuf = pstr;
	uart_obj->RxCount = len;
	UART_INTConfig(UART_DEV, RUART_BIT_ERBI | RUART_BIT_ETOI, ENABLE);
}

void uart_send_string(UART_OBJ *uart_obj, char *pstr)
{
	if (tx_busy) {
		return;
	}
	tx_busy = 1;
	uart_int_send(uart_obj, pstr, _strlen(pstr));
}

void uart_stream_irq_task(void)
{
	UART_OBJ uart_obj;
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

	uart_obj.UartIndex = uart_idx;
	uart_obj.RxCount = 0;
	uart_obj.TxCount = 0;

	UART_StructInit(&uart_obj.UART_InitStruct);
	uart_obj.UART_InitStruct.Parity = RUART_PARITY_DISABLE;
	UART_Init(UART_DEV, &uart_obj.UART_InitStruct);
	UART_SetBaud(UART_DEV, UART_BAUD);
	UART_RxCmd(UART_DEV, ENABLE);

	InterruptRegister((IRQ_FUN)uart_irq, UART_DEV_TABLE[uart_obj.UartIndex].IrqNum, (u32)&uart_obj, INT_PRI_MIDDLE);
	InterruptEn(UART_DEV_TABLE[uart_obj.UartIndex].IrqNum, INT_PRI_MIDDLE);

	uart_int_recv(&uart_obj, rx_buf, SRX_BUF_SZ);

	while (1) {
		if (rx_done) {
			uart_send_string(&uart_obj, rx_buf);
			rx_done = 0;
			uart_int_recv(&uart_obj, rx_buf, SRX_BUF_SZ);
		}
	}
}

int main(void)
{
	if (SUCCESS != rtos_task_create(NULL, "uart_stream_irq_task", (rtos_task_t)uart_stream_irq_task, (void *)NULL, (1024 * 4), (1))) {
		printf("Create uart_stream_irq_task Err!!\r\n");
	}

	rtos_sched_start();

	return 0;
}
