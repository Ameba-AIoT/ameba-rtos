/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "example_uart_ext.h"
#include "os_wrapper.h"

#define UART_DEV	UART0_DEV
#define UART_BAUD	38400

#define RX_DATA_INTIME	13
#define SRX_BUF_SZ		100
#define RX_TO_MS		1000

char rx_buf[SRX_BUF_SZ] = {0};
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
	UART_InitTypeDef UART_InitStruct;
	int TxCount;
	int RxCount;
	u8 *pTxBuf;
	u8 *pRxBuf;
	u8  UartIndex;
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
	UART_INTConfig(UART_DEV, RUART_BIT_EMDI | RUART_BIT_ERBI | RUART_BIT_ETOI, DISABLE);
}

u32 uart_irq(void *data)
{
	UART_OBJ *uart_obj = (UART_OBJ *) data;
	u32 reg_lsr;
	u32 TransCnt = 0;

	reg_lsr = UART_LineStatusGet(UART_DEV);

	/* rx FIFO not empty */
	if ((reg_lsr & RUART_BIT_RXFIFO_INT) || (reg_lsr & RUART_BIT_TIMEOUT_INT)) {
		TransCnt = UART_ReceiveDataTO(UART_DEV, uart_obj->pRxBuf, uart_obj->RxCount, 1);
		uart_obj->RxCount -= TransCnt;
		uart_obj->pRxBuf += TransCnt;
		if (0 == uart_obj->RxCount) {
			uart_recv_string_done();
		}

		/* clear timeout interrupt flag */
		if (reg_lsr & RUART_BIT_TIMEOUT_INT) {
			UART_INT_Clear(UART_DEV, RUART_BIT_TOICF);
		}
	}

	/* rx error */
	if (reg_lsr & UART_ALL_RX_ERR) {
		RTK_LOGI(NOTAG, "%s: LSR interrupt\n", __FUNCTION__);

		UART_INT_Clear(UART_DEV, RUART_BIT_RLSICF);
	}

	/* tx FIFO empty */
	if ((reg_lsr & RUART_BIT_TX_EMPTY)) {
		if (tx_busy) {
			TransCnt = UART_SendDataTO(UART_DEV, uart_obj->pTxBuf, uart_obj->TxCount, 1);
			uart_obj->TxCount -= TransCnt;
			uart_obj->pTxBuf += TransCnt;

			if (0 == uart_obj->TxCount) {
				uart_send_string_done();
			}
		}
	}

	return 0;
}

void uart_int_send(UART_OBJ *uart_obj, u8 *pstr, u32 len)
{
	uart_obj->pTxBuf = pstr;
	uart_obj->TxCount = len;
	UART_INTConfig(UART_DEV, RUART_BIT_ETBEI, ENABLE);
}

u32 uart_int_recv(UART_OBJ *uart_obj, u8 *pstr, u32 len, u32 timeout_ms, void *force_cs)
{
	rx_done = 0;
	u32 startcount = 0;
	void (*task_yield)(void);

	uart_obj->pRxBuf = pstr;
	uart_obj->RxCount = len;
	UART_INTConfig(UART_DEV, RUART_BIT_EMDI | RUART_BIT_ERBI | RUART_BIT_ETOI, ENABLE);

	SYSTIMER_Init();
	startcount = SYSTIMER_TickGet();

	if (uart_obj->RxCount > 0) {
		task_yield = (void(*)(void)) force_cs;
		while (1) {
			if (rx_done != 1) {
				/*time out*/
				if (SYSTIMER_GetPassTime(startcount) > timeout_ms) {
					UART_INTConfig(UART_DEV, (RUART_BIT_ERBI | RUART_BIT_ELSI | RUART_BIT_ETOI), DISABLE);
					UART_ClearRxFifo(UART_DEV);
					break;
				}
				if (NULL != task_yield) {
					task_yield();
				}
			} else {
				break;
			}
		}
	}
	return (len - uart_obj->RxCount);
}

void uart_send_string(UART_OBJ *uart_obj, char *pstr)
{
	if (tx_busy) {
		return;
	}
	tx_busy = 1;
	uart_int_send(uart_obj, (u8 *)pstr, _strlen(pstr));
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
	u32 ret;
	UART_OBJ uart_obj;

	u32 uart_idx = uart_get_idx(UART_DEV);

	if (0xFF == uart_idx) {
		return;
	}

	/* enable uart clock and function */
	RCC_PeriphClockCmd(APBPeriph_UARTx[uart_idx], APBPeriph_UARTx_CLOCK[uart_idx], ENABLE);
	/* enable timer clock and function */
	RCC_PeriphClockCmd(APBPeriph_TIMx[0], APBPeriph_TIMx_CLOCK[0], ENABLE);

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

	while (1) {
		// expect to receive maximum 13 bytes with timeout 1000ms
		// If you don't know what is Task Yield or no RTOS, then just keep the last argument is NULL
		ret = uart_int_recv(&uart_obj, (u8 *)rx_buf, RX_DATA_INTIME, RX_TO_MS, (void *)Release_CPU);

		if (ret < RX_DATA_INTIME) {
			RTK_LOGI(NOTAG, "Serial Rcv Timeout, Got %d bytes\n", ret);
		} else if (ret == RX_DATA_INTIME) {
			RTK_LOGI(NOTAG, "Recv all %d Bytes\n", ret);
			rx_buf[ret] = 0x0; // end of string
			uart_send_string(&uart_obj, rx_buf);
		}
	}
}

int example_raw_uart_int_rx_timeout(void)
{
	if (rtos_task_create(NULL, "uart_rx_timeout_demo", (rtos_task_t)uart_rx_timeout_demo, (void *)NULL, (2048 * 2), (1)) != RTK_SUCCESS) {
		RTK_LOGE(NOTAG, "Cannot create uart_rx_timeout_demo task\n");
	}

	//rtos_sched_start();

	return 0;
}