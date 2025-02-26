/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "at_intf_uart.h"
#include "atcmd_service.h"
#include "cJSON.h"
#include "kv.h"
#include "stdlib.h"
#include "ringbuffer.h"

#if defined (CONFIG_AMEBASMART)
u8 UART_TX = _PA_3; // UART0 TX
u8 UART_RX = _PA_2; // UART0 RX
#elif defined (CONFIG_AMEBALITE)
/* fully programmable zone */
u8 UART_TX = _PA_28; // UART TX
u8 UART_RX = _PA_29; // UART RX
#elif defined (CONFIG_AMEBADPLUS)
/* fully programmable zone */
u8 UART_TX = _PA_26; // UART TX
u8 UART_RX = _PA_27; // UART RX
#elif defined (CONFIG_AMEBAGREEN2)
/* fully programmable zone */
u8 UART_TX = _PA_4; // UART TX
u8 UART_RX = _PA_5; // UART RX
#endif

u32 UART_BAUD = 38400;

GDMA_InitTypeDef GDMA_InitStruct;
rtos_mutex_t uart_tx_mutex;

extern volatile UART_LOG_CTL shell_ctl;
extern UART_LOG_BUF shell_rxbuf;
extern char lfs_mount_fail;

#define UART_TT_BUF_LEN 1024
u8 uart_tt_buf[UART_TT_BUF_LEN];
u16 uart_tt_buf_len;
extern char g_tt_mode;
extern char g_tt_mode_check_watermark;
extern char g_tt_mode_indicate_high_watermark;
extern char g_tt_mode_indicate_low_watermark;
extern RingBuffer *atcmd_tt_mode_rx_ring_buf;
extern rtos_sema_t atcmd_tt_mode_sema;

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
	rtos_mutex_give(uart_tx_mutex);

	return 0;
}

void atio_uart_out_dma(char *buf, int len)
{
	u32 uart_idx = uart_get_idx(UART_DEV);
	bool ret;

	u8 *buf_new = (u8 *)rtos_mem_malloc(len);
	memcpy(buf_new, buf, len);

	UART_TXDMAConfig(UART_DEV, DMA_TX_BURST_SIZE);
	UART_TXDMACmd(UART_DEV, ENABLE);

	ret = UART_TXGDMA_Init(uart_idx, &GDMA_InitStruct, (void *)buf_new, (IRQ_FUN)uart_dma_cb, buf_new, len);

	if (!ret) {
		RTK_LOGI(NOTAG, "%s Error(%d)\n", __FUNCTION__, ret);
		rtos_mutex_give(uart_tx_mutex);
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
	rtos_mutex_take(uart_tx_mutex, MUTEX_WAIT_TIMEOUT);

	atio_uart_out_polling(buf, len);
	rtos_mutex_give(uart_tx_mutex);

	// TODO: dma mode
	// if (len > POLL_LEN_MAX) {
	// 	// tx by dma
	// 	atio_uart_out_dma(buf, len);
	// } else {
	// 	// tx by polling
	// 	atio_uart_out_polling(buf, len);
	// 	rtos_mutex_give(uart_tx_mutex);
	// }
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

tt_recv_again:

	if (g_tt_mode) {
		while (UART_Readable(UART_DEV)) {
			if (uart_tt_buf_len == UART_TT_BUF_LEN) {
				break;
			}
			UART_CharGet(UART_DEV, &(uart_tt_buf[uart_tt_buf_len++]));
		}

		if (uart_tt_buf_len > 0) {
			u32 space = RingBuffer_Space(atcmd_tt_mode_rx_ring_buf);

			if (g_tt_mode_check_watermark) {
				if (space - uart_tt_buf_len < MAX_TT_HEAP_SIZE * (1 - TT_MODE_HIGH_WATERMARK) && g_tt_mode_indicate_high_watermark == 0) {
					g_tt_mode_indicate_high_watermark = 1;
					g_tt_mode_indicate_low_watermark = 0;
					at_printf(ATCMD_TT_MODE_HIGH_WATERMARK_STR);
				}
			}

			if (space >= uart_tt_buf_len) {
				RingBuffer_Write(atcmd_tt_mode_rx_ring_buf, uart_tt_buf, uart_tt_buf_len);
				rtos_sema_give(atcmd_tt_mode_sema);
			} else {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "atcmd_tt_mode_rx_ring_buf is full, drop data\n");
			}
			uart_tt_buf_len = 0;
			goto tt_recv_again;
		}

		UART_DEV->IER = uart_ier;

		return 0;
	}

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
			shell_ctl.ExecuteCmd = TRUE;

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

int atio_uart_init(void)
{
	UART_InitTypeDef UART_InitStruct;
	u32 uart_idx = uart_get_idx(UART_DEV);

	if (0xFF == uart_idx) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s FAIL!!! Invalid UART index!\n", __FUNCTION__);
		return -1;
	}

	rtos_mutex_create(&uart_tx_mutex);

	/* enable uart clock and function */
	RCC_PeriphClockCmd(APBPeriph_UARTx[uart_idx], APBPeriph_UARTx_CLOCK[uart_idx], ENABLE);

#if defined (CONFIG_AMEBASMART)
	/* Configure UART TX and RX pin */
	Pinmux_Config(UART_TX, PINMUX_FUNCTION_UART);
	Pinmux_Config(UART_RX, PINMUX_FUNCTION_UART);
#elif defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS) || defined (CONFIG_AMEBAGREEN2)
	/* Configure UART TX and RX pin */
	Pinmux_Config(UART_TX, PINMUX_FUNCTION_UART0_TXD);
	Pinmux_Config(UART_RX, PINMUX_FUNCTION_UART0_RXD);
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
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s @%dbps\n", __FUNCTION__, UART_BAUD);

	return 0;
}

