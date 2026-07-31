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

#define UART_DEV		UART0_DEV
#define UART_BAUD		38400

#define SEND_BUF_SZ		256
#define RECV_BUF_SZ		256
#define ECHO_TIMEOUT_MS	2000

static u8 send_buf[SEND_BUF_SZ];
static u8 recv_buf[RECV_BUF_SZ];
static u32 send_len;

/* Set by the "uarttool" console command, consumed by the monitor task.
 * The monitor task is the sole owner of the Test-UART RX FIFO, so the
 * command must not touch UART0 directly (that would race the task).
 *
 * Thread-safety contract: uart_tool_cmd() (shell task) sets send_pending=1,
 * then waits.  uart_tool_monitor_task() consumes it and sets it back to 0.
 * Only one such operation is in flight at a time because the shell processes
 * commands sequentially.  No atomic guard is needed.
 *
 * monitor_ready enforces initialization ordering so send_pending never stays
 * set 1 before the monitor task is ready to clear it. */
static volatile u8 send_pending;

/* Guards command handler: monitor task must finish uart_tool_init() before
 * any "uarttool" command can be processed.  Without this guard, the shell
 * may invoke uart_tool_cmd() before the monitor task is scheduled, causing
 * send_pending to be set 1 and never cleared — deadlocking the shell. */
static volatile u8 monitor_ready;

UART_InitTypeDef UART_InitStruct;

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

static void uart_tool_send_string(char *pstr)
{
	u32 i = 0;
	while (*(pstr + i) != 0) {
		while (!UART_Writable(UART_DEV));
		UART_CharPut(UART_DEV, *(pstr + i));
		i++;
	}
}

/* Assemble the shell argv[0..argc-1] (space-separated) into send_buf.
 * The shell tokenises the input line on spaces, so a line with multiple
 * words arrives as several argv entries; rejoin them with single spaces. */
static void uart_tool_build_send_buf(u16 argc, u8 *argv[])
{
	u32 i, j;

	send_len = 0;
	_memset(send_buf, 0, SEND_BUF_SZ);

	for (i = 0; i < argc; i++) {
		if ((i > 0) && (send_len < SEND_BUF_SZ - 1)) {
			send_buf[send_len++] = ' ';
		}
		for (j = 0; argv[i][j] != '\0'; j++) {
			if (send_len < SEND_BUF_SZ - 1) {	/* leave room for '\0' */
				send_buf[send_len++] = argv[i][j];
			}
		}
	}

	send_buf[send_len] = '\0';
}

/* SEND + VERIFY:
 *   - Send send_buf to DUT via Test UART (UART0)
 *   - Wait for echo from DUT, forward each byte to Console (LOGUART TX)
 *   - Compare send_buf vs recv_buf, report result
 */
static void uart_tool_verify(void)
{
	u32 recv_len = 0;
	u32 timeout = 0;
	u8 c;
	u32 match;

	/* ── SEND: transmit buffered data to DUT ── */
	uart_tool_send_string((char *)send_buf);

	/* ── VERIFY: wait for echo ── */
	_memset(recv_buf, 0, RECV_BUF_SZ);

	while ((recv_len < send_len) && (timeout < ECHO_TIMEOUT_MS)) {
		if (UART_Readable(UART_DEV)) {
			UART_CharGet(UART_DEV, &c);
			if (recv_len < RECV_BUF_SZ) {
				recv_buf[recv_len++] = c;
			}
			DiagPutChar(c);			/* forward echo to Console */
			timeout = 0;			/* reset timeout on each received byte */
		} else {
			rtos_time_delay_ms(1);
			timeout++;
		}
	}

	/* ── REPORT ── */

	/* If nothing received within timeout window */
	if (recv_len == 0) {
		DiagPutChar('\r');
		DiagPutChar('\n');
		RTK_LOGI(NOTAG, "[TIMEOUT] No echo, sent %d bytes \"%s\"\n",
				 send_len, send_buf);
		return;
	}

	/* Compare send vs receive */
	match = 1;
	if (recv_len == send_len) {
		for (u32 i = 0; i < send_len; i++) {
			if (recv_buf[i] != send_buf[i]) {
				match = 0;
				break;
			}
		}
	} else {
		match = 0;
	}

	DiagPutChar('\r');
	DiagPutChar('\n');

	if (match) {
		RTK_LOGI(NOTAG, "PASS: %d/%d bytes matched\n",
				 recv_len, send_len);
	} else {
		RTK_LOGI(NOTAG, "FAIL: sent %d bytes, got %d bytes\n",
				 send_len, recv_len);
		RTK_LOGI(NOTAG, "  sent: \"%s\"\n", send_buf);
		recv_buf[recv_len] = '\0';
		RTK_LOGI(NOTAG, "  recv: \"%s\"\n", recv_buf);
	}
}

/* Console command handler: "uarttool <text>".
 *
 * The console/CLI (LOGUART RX) is owned solely by the shell; taking the user
 * input via a registered command avoids the example polling LOGUART RX behind
 * the shell's back (which caused cross-core RX-FIFO contention on 0x401C6000).
 * This handler runs in the shell task context, so blocking waits are fine.
 *
 * The actual send+verify is handed to the monitor task (the sole owner of the
 * Test-UART RX FIFO) via send_pending; we block here until it completes so the
 * result is reported before the shell prompt returns.
 *
 * Note: the monitor task must be running before this command is issued,
 * otherwise send_pending would never be cleared and the shell would hang
 * forever.  The monitor_ready flag enforces this ordering. */
static u32 uart_tool_cmd(u16 argc, u8 *argv[])
{
	if (argc == 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Usage: uarttool <text>\n");
		return HAL_ERR_PARA;
	}

	/* Wait for the monitor task to finish uart_tool_init() before proceeding.
	 * Without this guard, the shell can invoke uart_tool_cmd() before the
	 * monitor task runs, and send_pending would never be cleared — deadlock.
	 * A bounded wait prevents the deadlock without rejecting the command on
	 * the first try (which would break automated tests). */
	{
		u32 _wait = 0;
		while (!monitor_ready && _wait < 1000) {
			rtos_time_delay_ms(1);
			_wait++;
		}
		if (!monitor_ready) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "uarttool: monitor failed to start\n");
			return HAL_BUSY;
		}
	}

	uart_tool_build_send_buf(argc, argv);
	if (send_len == 0) {
		return HAL_ERR_PARA;
	}

	send_pending = 1;
	while (send_pending) {
		rtos_time_delay_ms(1);
	}

	return HAL_OK;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE uart_tool_cmd_table[] = {
	{"uarttool", uart_tool_cmd},
};

static void uart_tool_init(void)
{
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
	/* Configure UART0 TX and RX pin (normal pinmux, physical cross-wiring) */
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
	UART_Init(UART_DEV, &UART_InitStruct);
	UART_SetBaud(UART_DEV, UART_BAUD);
	UART_RxCmd(UART_DEV, ENABLE);

	RTK_LOGI(NOTAG, "\n========== UART Tool Ready ==========\n");
	RTK_LOGI(NOTAG, "Test UART: UART0 @ %d 8N1\n", UART_BAUD);
	RTK_LOGI(NOTAG, "Type on console: uarttool <text>\n");
	RTK_LOGI(NOTAG, "======================================\n\n");
}

/* MONITOR task: sole owner of the Test-UART (UART0) RX FIFO.
 *   - continuously forward DUT output (UART0 RX) → Console (so the user
 *     always sees what the DUT sends)
 *   - when the "uarttool" command queues a line, send it + verify the echo
 */
static void uart_tool_monitor_task(void *param)
{
	u8 c;

	(void)param;

	uart_tool_init();
	monitor_ready = 1;	/* signal command handler it is safe to proceed */

	while (1) {
		/* Forward DUT output → Console */
		if (UART_Readable(UART_DEV)) {
			UART_CharGet(UART_DEV, &c);
			DiagPutChar(c);
		}

		/* A console "uarttool" command queued a line to send + verify */
		if (send_pending) {
			uart_tool_verify();
			send_pending = 0;
		}
	}
}

int example_uart_tool(void)
{
	if (rtos_task_create(NULL, ((const char *)"uart_tool_monitor"), (rtos_task_t)uart_tool_monitor_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(NOTAG, "%s uart_tool_monitor task create failed\n", __FUNCTION__);
	}

	return 0;
}
