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

static u8 s_tx_buf[UART_BUF_SIZE];
static u8 s_rx_buf[UART_BUF_SIZE];

UART_InitTypeDef  UART_InitStruct;

#if defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS) || defined (CONFIG_AMEBAGREEN2) || defined (CONFIG_RTL8720F)
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

void uart_tx_task(void *param)
{
	u32 i;
	(void)param;

	RTK_LOGI(NOTAG, "UART TX task started\n");

	for (i = 0; i < UART_BUF_SIZE; i++) {
		s_tx_buf[i] = 0x30 + (i % 10);
	}

	/* extra sync: peer may still be in sync loop */
	rtos_time_delay_ms(100);
	UART_CharPut(UART_DEV, 0xFF);
	rtos_time_delay_ms(200);
	UART_CharPut(UART_DEV, 0xFF);
	rtos_time_delay_ms(200);

	RTK_LOGI(NOTAG, "UART TX data send start\n");

	for (i = 0; i < UART_BUF_SIZE; i++) {
		while (!UART_Writable(UART_DEV)) {
			rtos_time_delay_ms(1); /* yield to let lower-priority RX drain FIFO (prevents deadlock) */
		}
		UART_CharPut(UART_DEV, s_tx_buf[i]);
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
	u32 rx_count = 0, errors = 0, i;
	(void)param;

	RTK_LOGI(NOTAG, "UART RX task started\n");

	while (rx_count < UART_BUF_SIZE) {
		if (UART_Readable(UART_DEV)) {
			u8 b;
			UART_CharGet(UART_DEV, &b);
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
	u32 i, retries = 0, sync_count = 0;
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
#elif defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS) || defined (CONFIG_AMEBAGREEN2) || defined (CONFIG_RTL8720F)
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
	UART_ClearRxFifo(UART_DEV);

#if defined (CONFIG_AMEBASMART)
	/* Configure UART0 RTS and CTS pin to enable auto flow control */
	Pinmux_Config(UART_RTS, PINMUX_FUNCTION_UART_RTSCTS);
	Pinmux_Config(UART_CTS, PINMUX_FUNCTION_UART_RTSCTS);
#elif defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS) || defined (CONFIG_AMEBAGREEN2) || defined (CONFIG_RTL8720F)
	/* Configure UART0 RTS and CTS pin to enable auto flow control */
	Pinmux_Config(UART_RTS, UART_RTS_FID[uart_idx]);
	Pinmux_Config(UART_CTS, UART_CTS_FID[uart_idx]);
#endif

	/* enable auto flow control */
	UART_DEV->MCR |= RUART_MCL_FLOW_ENABLE;

	/* sync handshake: send 0xFF until we hear 2 from the peer */
	RTK_LOGI(NOTAG, "UART sync start\n");

	while (1) {
		UART_CharPut(UART_DEV, 0xFF);
		rtos_time_delay_ms(200);
		if (sync_count >= 2) {
			break;
		}
		/* poll for incoming sync bytes during wait */
		for (i = 0; i < 20; i++) {
			if (UART_Readable(UART_DEV)) {
				u8 b;
				UART_CharGet(UART_DEV, &b);
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

int example_raw_uart_polling_auto_flow_ctrl(void)
{
	if (rtos_task_create(NULL, ((const char *)"uart_auto_flow_ctrl_demo"), (rtos_task_t)uart_auto_flow_ctrl_demo, NULL, 1024 * 4,
						 1) != RTK_SUCCESS) {
		RTK_LOGE(NOTAG, "%s Create uart_auto_flow_ctrl_demo task failed\n", __FUNCTION__);
	}

	return 0;
}
