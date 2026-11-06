/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "at_intf_uart.h"
#include "atcmd_service.h"

#if defined (CONFIG_AMEBASMART)
u8 UART_TX = _PA_3; // UART0 TX
u8 UART_RX = _PA_2; // UART0 RX
#elif defined (CONFIG_AMEBALITE)
/* fully programmable zone */
u8 UART_TX = _PA_22; // UART TX
u8 UART_RX = _PA_21; // UART RX
#elif defined (CONFIG_AMEBADPLUS)
/* fully programmable zone */
u8 UART_TX = _PA_31; // UART TX
u8 UART_RX = _PA_30; // UART RX
#elif defined (CONFIG_AMEBAGREEN2)
/* fully programmable zone */
u8 UART_TX = _PA_26; // UART TX
u8 UART_RX = _PA_25; // UART RX
#else
u8 UART_TX = _PA_4; // UART TX
u8 UART_RX = _PA_5; // UART RX
#endif

signed char UART_RTS = -1;
signed char UART_CTS = -1;

u32 UART_BAUD = 38400;

static GDMA_InitTypeDef GDMA_InitStruct;
static rtos_sema_t atcmd_uart_tx_sema;
static rtos_sema_t atcmd_uart_rx_sema;
static rtos_sema_t atcmd_uart_tt_sema;

extern volatile UART_LOG_CTL shell_ctl;
extern UART_LOG_BUF shell_rxbuf;

#define UART_TT_BUF_LEN 1024
#define UART_TT_TASK_PRIO 6	/* above the atcmd tasks (5) that consume the tt ring buffer */
static u8 uart_tt_buf[UART_TT_BUF_LEN];
static u16 uart_tt_buf_len;
static volatile u8 uart_tt_rx_paused;
static u8 uart_hw_flow_ctrl;
extern char g_tt_mode;
extern char g_tt_mode_check_watermark;
extern char g_tt_mode_indicate_high_watermark;
extern char g_tt_mode_indicate_low_watermark;
extern RingBuffer *atcmd_tt_mode_rx_ring_buf;
extern rtos_sema_t atcmd_tt_mode_sema;
extern int atcmd_service(char *line_buf);

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
	(void) buf;
	uart_dma_free();
	rtos_sema_give(atcmd_uart_tx_sema);

	return 0;
}

void atio_uart_out_dma(char *buf, int len)
{
	u32 uart_idx = uart_get_idx(UART_DEV);
	bool ret;
	static u8 *buf_new = NULL;

	if (buf_new != NULL) {
		rtos_mem_free((void *)buf_new);
		buf_new = NULL;
	}

	u32 malloc_size = (((len >> 5) + 1) << 5);
	buf_new = (u8 *)rtos_mem_malloc(malloc_size);

	memcpy(buf_new, buf, len);

	UART_TXDMAConfig(UART_DEV, DMA_TX_BURST_SIZE);
	UART_TXDMACmd(UART_DEV, ENABLE);

	ret = UART_TXGDMA_Init(uart_idx, &GDMA_InitStruct, (void *)buf_new, (IRQ_FUN)uart_dma_cb, buf_new, len);

	if (!ret) {
		RTK_LOGI(NOTAG, "%s Error(%d)\n", __FUNCTION__, ret);
		rtos_sema_give(atcmd_uart_tx_sema);
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
	rtos_sema_take(atcmd_uart_tx_sema, MUTEX_WAIT_TIMEOUT);

	if (len > POLL_LEN_MAX) {
		// tx by dma
		atio_uart_out_dma(buf, len);
	} else {
		// tx by polling
		atio_uart_out_polling(buf, len);
		rtos_sema_give(atcmd_uart_tx_sema);
	}
}

/* atcmd_tt_mode_end clears the ring buffer in this critical section and may run on the other core under SMP; 0 once cleared */
static u32 uart_tt_ring_space(void)
{
	u32 space = 0;

	rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
	if (atcmd_tt_mode_rx_ring_buf != NULL) {
		space = RingBuffer_Space(atcmd_tt_mode_rx_ring_buf);
	}
	rtos_critical_exit(RTOS_CRITICAL_DEFAULT);

	return space;
}

u32 atio_uart_handler(void *data)
{
	(void)data;

	PUART_LOG_BUF pShellRxBuf;
	u32 uart_lsr, uart_ier;
	u32 i = 0;

	uart_lsr = UART_LineStatusGet(UART_DEV);


	if (uart_lsr & RUART_BIT_TIMEOUT_INT) {
		if (g_tt_mode) {
			UART_INT_Clear(UART_DEV, RUART_BIT_TOICF);
		} else {
			UART_ClearRxFifo(UART_DEV);
			return 0;
		}
	}

	if (uart_lsr & UART_ALL_RX_ERR) {
		if (uart_lsr & RUART_BIT_OVR_ERR) {
			RTK_LOGI(NOTAG, "%s: LSR over run interrupt\n", __FUNCTION__);
		}

		if (uart_lsr & RUART_BIT_PAR_ERR) {
			RTK_LOGI(NOTAG, "%s: LSR parity error interrupt\n", __FUNCTION__);
		}

		if (uart_lsr & RUART_BIT_FRM_ERR) {
			RTK_LOGI(NOTAG, "%s: LSR frame error(stop bit error) interrupt\n", __FUNCTION__);
		}

		if (uart_lsr & RUART_BIT_BREAK_INT) {
			RTK_LOGI(NOTAG, "%s: LSR break error interrupt\n", __FUNCTION__);
		}
		/* clear Receiver Line Status */
		UART_INT_Clear(UART_DEV, RUART_BIT_RLSICF);
		return 0;
	}

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
		u32 room = UART_TT_BUF_LEN;

		/* With RTS/CTS, never take more from the FIFO than the ring buffer can hold: the rest stays in
		 * the FIFO, AFE deasserts RTS and the host stops. RX irq is re-enabled by atcmd_uart_tt_task. */
		if (uart_hw_flow_ctrl) {
			/* 0 also when tt mode just ended: atcmd_uart_tt_task then resumes RX right away */
			u32 free = uart_tt_ring_space();
			room = free < room ? free : room;
			if (room == 0) {
				uart_tt_rx_paused = 1;
				UART_DEV->IER = uart_ier & ~(RUART_BIT_ERBI | RUART_BIT_ETOI);
				rtos_sema_give(atcmd_uart_tt_sema);
				return 0;
			}
		}

		while (UART_Readable(UART_DEV)) {
			if (uart_tt_buf_len == room) {
				break;
			}
			UART_CharGet(UART_DEV, &(uart_tt_buf[uart_tt_buf_len++]));
		}

		if (uart_tt_buf_len > 0) {
			u32 space = 0;

			/* check and write the ring buffer within the critical section in which atcmd_tt_mode_end clears it */
			rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
			if (atcmd_tt_mode_rx_ring_buf != NULL) {
				space = RingBuffer_Space(atcmd_tt_mode_rx_ring_buf);
				if (space >= uart_tt_buf_len) {
					RingBuffer_Write(atcmd_tt_mode_rx_ring_buf, uart_tt_buf, uart_tt_buf_len);
				}
			}
			rtos_critical_exit(RTOS_CRITICAL_DEFAULT);

			if (g_tt_mode_check_watermark) {
				if (space - uart_tt_buf_len < MAX_TT_HEAP_SIZE * (1 - TT_MODE_HIGH_WATERMARK) && g_tt_mode_indicate_high_watermark == 0) {
					g_tt_mode_indicate_high_watermark = 1;
					g_tt_mode_indicate_low_watermark = 0;
					/* at_printf is not ISR safe (mutex, DMA, malloc): let atcmd_uart_tt_task send it */
					rtos_sema_give(atcmd_uart_tt_sema);
				}
			}

			if (space >= uart_tt_buf_len) {
				rtos_sema_give(atcmd_tt_mode_sema);
			} else {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "atcmd_tt_mode_rx_ring_buf is full, drop data\n");
			}

			/*recv stop char under tt mode*/
			if (uart_tt_buf_len == 1 && uart_tt_buf[0] == '<') {
				g_tt_mode_stop_char_cnt++;
			} else {
				g_tt_mode_stop_char_cnt = 0;
			}

			/*cancel tt mode stop timer if recv pkt before timeout*/
			if (g_tt_mode_stop_flag == 0 && rtos_timer_is_timer_active(xTimers_TT_Mode)) {
				rtos_timer_stop(xTimers_TT_Mode, 0);
				g_tt_mode_stop_char_cnt = 0;
			}

			/*start tt mode stop timer once*/
			if (g_tt_mode_stop_char_cnt >= 3) {
				if (rtos_timer_is_timer_active(xTimers_TT_Mode) == 0) {
					rtos_timer_start(xTimers_TT_Mode, 0);
				}
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
		if (pShellRxBuf->BufCount >= CMD_BUFLEN) {
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
			rtos_sema_give(atcmd_uart_rx_sema);
		}
	}

	/* recv all data one time */
	if ((pShellRxBuf->BufCount != i) && (pShellRxBuf->BufCount != 0)) {
		goto recv_again;
	}

	UART_DEV->IER = uart_ier;

	return 0;
}

void atcmd_uart_input_handler_task(void)
{
	PUART_LOG_BUF pCmdLogBuf = shell_ctl.pTmpLogBuf;
	u32 ret = FALSE;

	while (1) {
		rtos_sema_take(atcmd_uart_rx_sema, 0xFFFFFFFF);

		ret = atcmd_service((char *)pCmdLogBuf->UARTLogBuf);
		if (ret == FALSE) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\r\nunknown command '%s'", pCmdLogBuf->UARTLogBuf);
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\r\n\n#\r\n");
		}
		memset((u8 *)pCmdLogBuf->UARTLogBuf, '\0', CMD_BUFLEN);
		pCmdLogBuf->BufCount = 0;
	}
}

/* Task-context work posted by atio_uart_handler: RX resume (with RTS/CTS) or High Watermark (without) */
static void atcmd_uart_tt_task(void *param)
{
	(void)param;

	while (1) {
		rtos_sema_take(atcmd_uart_tt_sema, 0xFFFFFFFF);

		if (uart_tt_rx_paused) {
			/* wait for the consumer to free space or tt mode to end */
			while (g_tt_mode && uart_tt_ring_space() == 0) {
				rtos_time_delay_ms(1);
			}
			uart_tt_rx_paused = 0;
			/* FIFO still holds data, so the RX irq fires again right away */
			UART_INTConfig(UART_DEV, RUART_BIT_ERBI | RUART_BIT_ETOI, ENABLE);
		}

		/* skip if the consumer already drained below the low watermark meanwhile */
		if (g_tt_mode && g_tt_mode_indicate_high_watermark) {
			at_printf(ATCMD_TT_MODE_HIGH_WATERMARK_STR);
		}
	}
}

int atio_uart_init(void)
{
	UART_InitTypeDef UART_InitStruct;
	u32 uart_idx = uart_get_idx(UART_DEV);

	if (0xFF == uart_idx) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s FAIL!!! Invalid UART index!\n", __FUNCTION__);
		return -1;
	}

	rtos_sema_create(&atcmd_uart_tx_sema, 1, 0xFFFF);
	rtos_sema_create(&atcmd_uart_rx_sema, 0, 0xFFFF);

	/* enable uart clock and function */
	RCC_PeriphClockCmd(APBPeriph_UARTx[uart_idx], APBPeriph_UARTx_CLOCK[uart_idx], ENABLE);

#if defined (CONFIG_AMEBASMART)
	/* Configure UART TX and RX pin */
	Pinmux_Config(UART_TX, PINMUX_FUNCTION_UART);
	Pinmux_Config(UART_RX, PINMUX_FUNCTION_UART);
#else
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

	if (UART_RTS > 0 && UART_CTS > 0) {
		UART_SetRxLevel(UART_DEV_TABLE[uart_idx].UARTx, UART_RX_FIFOTRIG_LEVEL_1BYTES);
#if defined (CONFIG_AMEBASMART)
		Pinmux_Config(UART_RTS, PINMUX_FUNCTION_UART_RTSCTS);
		Pinmux_Config(UART_CTS, PINMUX_FUNCTION_UART_RTSCTS);
#else
		Pinmux_Config(UART_RTS, PINMUX_FUNCTION_UART0_RTS);
		Pinmux_Config(UART_CTS, PINMUX_FUNCTION_UART0_CTS);
#endif
		UART_DEV_TABLE[uart_idx].UARTx->MCR |= RUART_BIT_AFE;
		UART_DEV_TABLE[uart_idx].UARTx->MCR |= RUART_BIT_RTS;
		uart_hw_flow_ctrl = 1;
	}

	InterruptRegister((IRQ_FUN)atio_uart_handler, UART_DEV_TABLE[uart_idx].IrqNum, (u32)UART_DEV, INT_PRI_MIDDLE);
	InterruptEn(UART_DEV_TABLE[uart_idx].IrqNum, INT_PRI_MIDDLE);
	UART_INTConfig(UART_DEV, RUART_BIT_ERBI | RUART_BIT_ELSI | RUART_BIT_ETOI, ENABLE);

	if (rtos_task_create(NULL, ((const char *)"atcmd_uart_input_handler_task"), (rtos_task_t)atcmd_uart_input_handler_task, NULL, 6 * 1024, 5) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r%s rtos_task_create(atcmd_uart_input_handler_task) failed", __FUNCTION__);
		return -1;
	}

	rtos_sema_create(&atcmd_uart_tt_sema, 0, 1);
	if (rtos_task_create(NULL, ((const char *)"atcmd_uart_tt_task"), (rtos_task_t)atcmd_uart_tt_task, NULL, 1024, UART_TT_TASK_PRIO) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r%s create atcmd_uart_tt_task failed", __FUNCTION__);
		return -1;
	}

	out_buffer = atio_uart_output;
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s @%dbps\n", __FUNCTION__, UART_BAUD);

	return 0;
}
