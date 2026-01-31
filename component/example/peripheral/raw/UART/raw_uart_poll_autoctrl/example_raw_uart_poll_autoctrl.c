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
#define UART_BUF_SIZE	1000

u8 buffer[UART_BUF_SIZE];

UART_InitTypeDef  UART_InitStruct;

#if defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS) || defined (CONFIG_AMEBAGREEN2)
const u8 UART_TX_FID[MAX_UART_INDEX] = {
	PINMUX_FUNCTION_UART0_TXD,
	PINMUX_FUNCTION_UART1_TXD,
	PINMUX_FUNCTION_UART2_TXD,
#if defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBAGREEN2)
	PINMUX_FUNCTION_UART3_TXD
#endif
};

const u8 UART_RX_FID[MAX_UART_INDEX] = {
	PINMUX_FUNCTION_UART0_RXD,
	PINMUX_FUNCTION_UART1_RXD,
	PINMUX_FUNCTION_UART2_RXD,
#if defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBAGREEN2)
	PINMUX_FUNCTION_UART3_RXD
#endif
};

const u8 UART_CTS_FID[MAX_UART_INDEX] = {
	PINMUX_FUNCTION_UART0_CTS,
	NULL,
#if defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBAGREEN2)
	NULL,
	PINMUX_FUNCTION_UART3_CTS
#elif defined (CONFIG_AMEBADPLUS)
	PINMUX_FUNCTION_UART2_CTS
#endif
};

const u8 UART_RTS_FID[MAX_UART_INDEX] = {
	PINMUX_FUNCTION_UART0_RTS,
	NULL,
#if defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBAGREEN2)
	NULL,
	PINMUX_FUNCTION_UART3_RTS
#elif defined (CONFIG_AMEBADPLUS)
	PINMUX_FUNCTION_UART2_RTS
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

void uart_auto_flow_ctrl_demo(void)
{
	u8 rc;
	u32 i, j;
	u32 rx_side = 0;
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
#elif defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS) || defined (CONFIG_AMEBAGREEN2)
	/* Configure UART0 TX and RX pin */
	Pinmux_Config(UART_TX, UART_TX_FID[uart_idx]);
	Pinmux_Config(UART_RX, UART_RX_FID[uart_idx]);
#endif

	PAD_PullCtrl(UART_TX, GPIO_PuPd_UP); // pull up Tx/Rx pin
	PAD_PullCtrl(UART_RX, GPIO_PuPd_UP); // pull up Tx/Rx pin

	UART_StructInit(&UART_InitStruct);
	UART_InitStruct.WordLen = RUART_WLS_8BITS;
	UART_InitStruct.StopBit = RUART_STOP_BIT_1;
	UART_InitStruct.Parity = RUART_PARITY_DISABLE;
	UART_InitStruct.ParityType = RUART_ODD_PARITY;
	UART_InitStruct.StickParity = RUART_STICK_PARITY_DISABLE;
	UART_InitStruct.RxFifoTrigLevel = UART_RX_FIFOTRIG_LEVEL_1BYTES;
	UART_InitStruct.RxTimeOutCnt = 64;
	UART_Init(UART_DEV, &UART_InitStruct);
	UART_SetBaud(UART_DEV, UART_BAUD);
	UART_RxCmd(UART_DEV, ENABLE);

	for (i = 0; i < 128; i++) {
		RTK_LOGI(NOTAG, "Wait peer ready...\n");
		UART_CharPut(UART_DEV, i + 1);
		if (UART_Readable(UART_DEV)) {
			UART_CharGet(UART_DEV, &rc);
			if (rc > i) {
				rx_side = 1;
			} else {
				rx_side = 0;
			}
			break;
		}
		DelayMs(100);
	}
	UART_ClearRxFifo(UART_DEV);

#if defined (CONFIG_AMEBASMART)
	/* Configure UART0 RTS and CTS pin to enable auto flow control */
	Pinmux_Config(UART_RTS, PINMUX_FUNCTION_UART_RTSCTS);
	Pinmux_Config(UART_CTS, PINMUX_FUNCTION_UART_RTSCTS);
#elif defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS) || defined (CONFIG_AMEBAGREEN2)
	/* Configure UART0 RTS and CTS pin to enable auto flow control */
	Pinmux_Config(UART_RTS, UART_RTS_FID[uart_idx]);
	Pinmux_Config(UART_CTS, UART_CTS_FID[uart_idx]);
#endif

	/* enable auto flow control */
	UART_DEV->MCR |= RUART_MCL_FLOW_ENABLE;

	if (rx_side) {
		RTK_LOGI(NOTAG, "UART Flow Control: RX ==>\n");
		_memset(buffer, 0, UART_BUF_SIZE);
		i = 0;
		j = 0;
		while (1) {
			if (UART_Readable(UART_DEV)) {
				UART_CharGet(UART_DEV, buffer + i);
				i++;
				if (i == UART_BUF_SIZE) {
					break;
				}
				if ((i & 0xF) == 0) {
					// Make some delay to cause the RX FIFO full and then trigger flow controll
					DelayMs(100);
					RTK_LOGI(NOTAG, "UART RX got %d bytes\n", i);
				}
				j = 0;
			} else {
				DelayMs(10);
				j++;
				if (j == 1000) {
					RTK_LOGE(NOTAG, "UART RX Failed, Got %d bytes\n", i);
					break;
				}
			}
		}

	} else {
		RTK_LOGI(NOTAG, "UART Flow Control: TX ==>\n");
		DelayMs(400);
		/*send large number to make peer device be rx side */
		UART_CharPut(UART_DEV, 0xFF);
		DelayMs(100);

		for (i = 0; i < UART_BUF_SIZE; i++) {
			buffer[i] = 0x30 + (i % 10);
		}

		for (i = 0; i < UART_BUF_SIZE; i++) {
			while (!UART_Writable(UART_DEV));
			UART_CharPut(UART_DEV, buffer[i]);
		}
	}

	RTK_LOGI(NOTAG, "UART Flow Control Test Done!\n");
	while (1);
}

int example_raw_uart_polling_auto_flow_ctrl(void)
{
	if (rtos_task_create(NULL, ((const char *)"uart_auto_flow_ctrl_demo"), (rtos_task_t)uart_auto_flow_ctrl_demo, NULL, 1024 * 4,
						 1) != RTK_SUCCESS) {
		RTK_LOGE(NOTAG, "%s rtos_task_create(uart_auto_flow_ctrl_demo) failed\n", __FUNCTION__);
	}

	//rtos_sched_start();

	return 0;
}
