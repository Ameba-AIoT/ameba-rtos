/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "at_intf_uart.h"
#include "atcmd_service.h"

GDMA_InitTypeDef GDMA_InitStruct;
volatile u32 dma_tx_busy = 0;

extern volatile UART_LOG_CTL shell_ctl;
extern UART_LOG_BUF shell_rxbuf;

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

static u32 uart_get_idx(UART_TypeDef *Uartx)
{
	u32 i;

	for (i = 0; i < MAX_UART_INDEX; i++) {
		if (Uartx == UART_DEV_TABLE[i].UARTx) {
			return i;
		}
	}

	return 0xFF;
}

static void uart_dma_free(void)
{
	GDMA_Cmd(GDMA_InitStruct.GDMA_Index, GDMA_InitStruct.GDMA_ChNum, DISABLE);
	GDMA_ClearINT(GDMA_InitStruct.GDMA_Index, GDMA_InitStruct.GDMA_ChNum);
	GDMA_ChnlFree(GDMA_InitStruct.GDMA_Index, GDMA_InitStruct.GDMA_ChNum);
	UART_TXDMACmd(UART_DEV, DISABLE);
}

u32 uart_dma_cb(void *buf)
{
	uart_dma_free();
	rtos_mem_free(buf);
	dma_tx_busy = 0;

	return 0;
}

void atio_uart_out_dma(char *buf, int len)
{
	u32 uart_idx = uart_get_idx(UART_DEV);
	BOOL ret;

	do {} while (dma_tx_busy);
	dma_tx_busy = 1;

	u8 *buf_new = (u8 *)rtos_mem_malloc(len);
	memcpy(buf_new, buf, len);

	UART_TXDMAConfig(UART_DEV, DMA_TX_BURST_SIZE);
	UART_TXDMACmd(UART_DEV, ENABLE);

	ret = UART_TXGDMA_Init(uart_idx, &GDMA_InitStruct, (void *)buf_new, (IRQ_FUN)uart_dma_cb, buf_new, len);

	if (!ret) {
		printf("%s Error(%d)\n", __FUNCTION__, ret);
		dma_tx_busy = 0;
	}
}

void atio_uart_out_polling(char *buf, int len)
{
	u32 i = 0;

	while (len--) {
		while (!UART_Writable(UART_DEV));
		UART_CharPut(UART_DEV, *(buf + i));
		i++;
	}
}

void atio_uart_output(char *buf, int len)
{
	if (len > POLL_LEN_MAX) {
		// tx by dma
		atio_uart_out_dma(buf, len);
	} else {
		// tx by polling
		atio_uart_out_polling(buf, len);
	}
}

u32 atio_uart_handler(void *data)
{
	(void)data;

	PUART_LOG_BUF pShellRxBuf;
	u32 uart_lsr, uart_ier;
	u8 i = 0;

	uart_lsr = UART_LineStatusGet(UART_DEV);

	/* moves on only for rx data ready intr(At least one characters in the RX FIFO), in case other intr is enable in linux */
	if (!(uart_lsr & RUART_BIT_DRDY)) {
		return 0;
	}

	/* init pShellRxBuf for store log data and count */
	pShellRxBuf = &shell_rxbuf;
	pShellRxBuf->BufCount = 0;

	uart_ier = UART_DEV->IER;
	UART_INTConfig(UART_DEV, uart_ier, DISABLE);

recv_again:

	/* fetch all data in Uart rx fifo before processing each character */
	while (UART_Readable(UART_DEV)) {
		if (pShellRxBuf->BufCount >= UART_LOG_CMD_BUFLEN) {
			break;
		}
		UART_CharGet(UART_DEV, &(pShellRxBuf->UARTLogBuf[pShellRxBuf->BufCount]));
		pShellRxBuf->BufCount++;
	}

	/* if fetch no data in rx fifo, return */
	if (pShellRxBuf->BufCount == 0) {
		return 0;
	}

	if (shell_cmd_chk(pShellRxBuf->UARTLogBuf[i++], (UART_LOG_CTL *)&shell_ctl, ENABLE) == 2) {
		//4 check UartLog buffer to prevent from incorrect access
		if (shell_ctl.pTmpLogBuf != NULL) {
			shell_ctl.ExecuteCmd = _TRUE;

			if (shell_ctl.shell_task_rdy) {
				shell_ctl.GiveSema();
			}
		} else {
			shell_array_init((u8 *)shell_ctl.pTmpLogBuf->UARTLogBuf, UART_LOG_CMD_BUFLEN, '\0');
		}
	}

	/* recv all data one time */
	if ((pShellRxBuf->BufCount != i) && (pShellRxBuf->BufCount != 0)) {
		goto recv_again;
	}

	UART_DEV->IER = uart_ier;

	return 0;
}

void atio_uart_init(void)
{
	UART_InitTypeDef UART_InitStruct;
	u32 uart_idx = uart_get_idx(UART_DEV);

#if defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS)
	/* set bt uart as normal uart */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0) | LSYS_BIT_BT_USE_EXT_UART);
#endif

	/* enable uart clock and function */
	RCC_PeriphClockCmd(APBPeriph_UARTx[uart_idx], APBPeriph_UARTx_CLOCK[uart_idx], ENABLE);

#if defined (CONFIG_AMEBASMART)
	/* Configure UART TX and RX pin */
	Pinmux_Config(UART_TX, PINMUX_FUNCTION_UART);
	Pinmux_Config(UART_RX, PINMUX_FUNCTION_UART);
#elif defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS)
	/* Configure UART TX and RX pin */
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

	InterruptRegister((IRQ_FUN)atio_uart_handler, UART_DEV_TABLE[uart_idx].IrqNum, (u32)UART_DEV, INT_PRI_MIDDLE);
	InterruptEn(UART_DEV_TABLE[uart_idx].IrqNum, INT_PRI_MIDDLE);
	UART_INTConfig(UART_DEV, RUART_BIT_ERBI | RUART_BIT_ELSI | RUART_BIT_ETOI, ENABLE);

	out_buffer = atio_uart_output;
	at_printf("%s @%dbps\n", __FUNCTION__, UART_BAUD);
}

