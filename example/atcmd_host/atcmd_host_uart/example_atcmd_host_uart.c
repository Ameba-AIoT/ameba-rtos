/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "serial_api.h"
#include "serial_ex_api.h"
#include "example_atcmd_host_uart.h"
#include "atcmd_service.h"
#include "stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* -------------------------------------------------------------------------
 * Configuration
 * ------------------------------------------------------------------------- */

/*
 * Enable TT (transparent transmission) mode upstream test.
 * The host generates TT_TEST_DATA_LEN bytes and sends them to the slave
 * after the slave issues the ">>>\r\n" trigger.  The slave must be
 * configured for TT mode for this to be meaningful.
 * Set to 0 to disable (the host still handles the ">>>\r\n" trigger but
 * will not start a local sender task).
 */
#define TT_MODE_TEST        1

/* AT_UART ISR buffer.  The ISR delivers one byte per IRQ at modest baud
 * (38400), and the rx task drains it promptly, so a small buffer suffices. */
#define AT_UART_RX_BUF_SIZE 64
#define AT_UART_TX_BUF_SIZE 2048

#define MAX_CMD_LEN         2000
#define FORMAT_LEN          128

/* -------------------------------------------------------------------------
 * State
 * ------------------------------------------------------------------------- */

static serial_t host_sobj;  /* human terminal */
static serial_t at_sobj;    /* slave device link */

static void *host_uart_irq_sema;    /* HOST_UART \r\n received */
static void *at_uart_rx_sema;       /* AT_UART data received from slave */
static void *tt_mode_tx_sema;       /* TT mode flow control (HW/LW watermark) */

static char  host_uart_irq_buf[MAX_CMD_LEN];
static u32   host_uart_irq_count = 0;

/* at_uart_rx_buf / at_uart_rx_len: written by ISR, read+cleared by task */
static u8             at_uart_rx_buf[AT_UART_RX_BUF_SIZE];
static volatile u32   at_uart_rx_len = 0;

static char          uart_format_buffer[FORMAT_LEN];

static volatile u8   tt_mode_task_start = 0;
static u32           tt_len = 2 * 1024 * 1024;

/* -------------------------------------------------------------------------
 * Helpers
 * ------------------------------------------------------------------------- */

static void host_uart_send_string(char *pstr, u32 len)
{
	u32 i = 0;
	while (*(pstr + i) != 0 && i < len) {
		serial_putc(&host_sobj, *(pstr + i));
		i++;
	}
}

static void host_uart_format_output(const char *fmt, ...)
{
	int len;
	va_list ap;
	va_start(ap, fmt);
	len = vsnprintf(uart_format_buffer, FORMAT_LEN, fmt, ap);
	va_end(ap);
	if (len > 0 && len < FORMAT_LEN) {
		host_uart_send_string(uart_format_buffer, len);
	}
}

static void at_uart_send(u8 *buf, u32 len)
{
	u32 i;
	for (i = 0; i < len; i++) {
		serial_putc(&at_sobj, buf[i]);
	}
}

/* -------------------------------------------------------------------------
 * IRQ handlers
 * ------------------------------------------------------------------------- */

/*
 * HOST_UART ISR: collect user input until \r\n or buffer full,
 * then signal uart_irq_handle_task.  Identical logic to other host demos.
 */
static void host_uart_irq(uint32_t id, SerialIrq event)
{
	serial_t *sobj = (void *)id;
	BaseType_t task_woken = pdFALSE;

	if (event == RxIrq) {
		while (serial_readable(sobj)) {
			if (host_uart_irq_count == MAX_CMD_LEN) {
				break;
			}
			host_uart_irq_buf[host_uart_irq_count++] = serial_getc(sobj);
			serial_putc(sobj, host_uart_irq_buf[host_uart_irq_count - 1]); /* echo */
		}

		if ((host_uart_irq_count > 1
			 && host_uart_irq_buf[host_uart_irq_count - 1] == '\n'
			 && host_uart_irq_buf[host_uart_irq_count - 2] == '\r')
			|| (host_uart_irq_count == MAX_CMD_LEN)) {
			xSemaphoreGiveFromISR(host_uart_irq_sema, &task_woken);
			portEND_SWITCHING_ISR(task_woken);
		}
	}
}

/*
 * AT_UART ISR: drain FIFO into at_uart_rx_buf, signal at_uart_rx_task.
 * Data is consumed and the buffer cleared by the task under a critical
 * section, so the ISR always writes from the current at_uart_rx_len offset.
 */
static void at_uart_irq(uint32_t id, SerialIrq event)
{
	(void)id;
	BaseType_t task_woken = pdFALSE;

	if (event == RxIrq) {
		while (serial_readable(&at_sobj)) {
			if (at_uart_rx_len < AT_UART_RX_BUF_SIZE) {
				at_uart_rx_buf[at_uart_rx_len++] = serial_getc(&at_sobj);
			} else {
				/* Buffer full: must drain FIFO to prevent ERBI re-firing indefinitely. */
				(void)serial_getc(&at_sobj);
			}
		}
		xSemaphoreGiveFromISR(at_uart_rx_sema, &task_woken);
		portEND_SWITCHING_ISR(task_woken);
	}
}

/* -------------------------------------------------------------------------
 * Tasks
 * ------------------------------------------------------------------------- */
/*
 * Forward HOST_UART input to the slave via AT_UART.
 * No framing needed: UART is a raw byte stream; the slave's shell_cmd_chk
 * parses commands terminated by \r\n just like a normal AT terminal.
 */
static void uart_irq_handle_task(void)
{
	while (1) {
		xSemaphoreTake(host_uart_irq_sema, 0xFFFFFFFF);
		if (strstr((char *)host_uart_irq_buf, "\r\n") && strstr((char *)host_uart_irq_buf, "AT+TEST=1,")) {
			tt_len = atoi(&host_uart_irq_buf[10]);
		}
		at_uart_send((u8 *)host_uart_irq_buf, host_uart_irq_count);
		memset(host_uart_irq_buf, 0, MAX_CMD_LEN);
		host_uart_irq_count = 0;
	}
}

#if TT_MODE_TEST
/*
 * Upstream throughput test: burst tt_len bytes to the slave.
 * Started dynamically when the slave sends ">>>\r\n".
 * tt_mode_tx_sema provides HW/LW flow control: at_uart_rx_task takes the
 * semaphore on a High Watermark notification and gives it on Low Watermark.
 */
static void tt_mode_test_task(void *param)
{
	(void)param;
	u32 tt_remaining = tt_len;
	u32 send_len;
	u8 *tt_tx_buf = pvPortMalloc(AT_UART_TX_BUF_SIZE);

	if (!tt_tx_buf) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "tt_mode_test_task: alloc fail\r\n");
		vTaskDelete(NULL);
		return;
	}

	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "tt_mode_test_task start\r\n");

	while (tt_remaining > 0) {
		xSemaphoreTake(tt_mode_tx_sema, 0xFFFFFFFF);
		send_len = tt_remaining > AT_UART_TX_BUF_SIZE ? AT_UART_TX_BUF_SIZE : tt_remaining;
		at_uart_send(tt_tx_buf, send_len);
		tt_remaining -= send_len;
		xSemaphoreGive(tt_mode_tx_sema);
	}

	vPortFree(tt_tx_buf);
	tt_mode_task_start = 0;
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "tt_mode_test_task end\r\n");
	vTaskDelete(NULL);
}
#endif /* TT_MODE_TEST */


/*
 * Process data received from the slave via AT_UART one byte at a time.
 *
 * AT_UART delivers a single byte per IRQ, so a multi-byte memcmp against
 * the full pattern string never matches against a per-wakeup snapshot.
 * Instead we keep one small accumulation buffer that is shared across
 * every pattern we care about:
 *   - ATCMD_ENTER_TT_MODE_STR             (TT mode entry trigger)
 *   - ATCMD_DOWNSTREAM_TEST_START_STR     (downstream test marker)
 *   - ATCMD_DOWNSTREAM_TEST_END_STR       (downstream test marker)
 *   - ATCMD_TT_MODE_HIGH_WATERMARK_STR    (TT mode flow control)
 *   - ATCMD_TT_MODE_LOW_WATERMARK_STR     (TT mode flow control)
 *
 * Each incoming byte is appended; while the head of the buffer matches a
 * full pattern we act on it and drop the matched bytes.  A leading byte
 * that cannot start any (still-relevant) pattern is sent straight to
 * HOST_UART — there is no separate display ring buffer.
 */
static void at_uart_rx_task(void)
{
	/* Sized for the longest tracked pattern (HW watermark = 24 bytes). */
	static u8  match_buf[32];
	static u32 match_len = 0;
	static u32 start_time = 0, ds_count = 0;

	const u32 enter_len    = (u32)strlen(ATCMD_ENTER_TT_MODE_STR);
	const u32 hw_len       = (u32)strlen(ATCMD_TT_MODE_HIGH_WATERMARK_STR);
	const u32 lw_len       = (u32)strlen(ATCMD_TT_MODE_LOW_WATERMARK_STR);
	const u32 ds_start_len = (u32)strlen(ATCMD_DOWNSTREAM_TEST_START_STR);
	const u32 ds_end_len   = (u32)strlen(ATCMD_DOWNSTREAM_TEST_END_STR);

	while (1) {
		xSemaphoreTake(at_uart_rx_sema, 0xFFFFFFFF);

		/* Drain the ISR buffer one byte at a time. */
		while (1) {
			u8 b;

			taskENTER_CRITICAL();
			if (at_uart_rx_len == 0) {
				taskEXIT_CRITICAL();
				break;
			}
			b = at_uart_rx_buf[0];
			if (at_uart_rx_len > 1) {
				memmove((void *)at_uart_rx_buf,
						(const void *)(at_uart_rx_buf + 1),
						at_uart_rx_len - 1);
			}
			at_uart_rx_len--;
			taskEXIT_CRITICAL();

			/* Append; safety drain if the match buffer somehow fills up. */
			if (match_len >= sizeof(match_buf)) {
				serial_putc(&host_sobj, match_buf[0]);
				memmove(match_buf, match_buf + 1, match_len - 1);
				match_len--;
			}
			match_buf[match_len++] = b;

			/* Try to consume patterns from the front of match_buf. */
			while (match_len > 0) {
				int progressed = 0;
				int can_wait   = 0;

#if TT_MODE_TEST
				if (tt_mode_task_start) {
					/* TT mode: only HW / LW watermarks matter. */
					if (match_len >= hw_len
						&& memcmp(match_buf, ATCMD_TT_MODE_HIGH_WATERMARK_STR, hw_len) == 0) {
						xSemaphoreTake(tt_mode_tx_sema, 0xFFFFFFFF);
						host_uart_send_string(ATCMD_TT_MODE_HIGH_WATERMARK_STR, hw_len);
						match_len -= hw_len;
						if (match_len > 0) {
							memmove(match_buf, match_buf + hw_len, match_len);
						}
						progressed = 1;
					} else if (match_len >= lw_len
							   && memcmp(match_buf, ATCMD_TT_MODE_LOW_WATERMARK_STR, lw_len) == 0) {
						xSemaphoreGive(tt_mode_tx_sema);
						host_uart_send_string(ATCMD_TT_MODE_LOW_WATERMARK_STR, lw_len);
						match_len -= lw_len;
						if (match_len > 0) {
							memmove(match_buf, match_buf + lw_len, match_len);
						}
						progressed = 1;
					} else {
						if ((match_len < hw_len
							 && memcmp(match_buf, ATCMD_TT_MODE_HIGH_WATERMARK_STR, match_len) == 0)
							|| (match_len < lw_len
								&& memcmp(match_buf, ATCMD_TT_MODE_LOW_WATERMARK_STR, match_len) == 0)) {
							can_wait = 1;
						}
					}
				} else
#endif
				{
					/* Normal mode: TT entry trigger and downstream markers. */
#if TT_MODE_TEST
					if (match_len >= enter_len
						&& memcmp(match_buf, ATCMD_ENTER_TT_MODE_STR, enter_len) == 0) {
						if (xTaskCreate((void *)tt_mode_test_task, "tt_mode_test_task",
										1024 / sizeof(portSTACK_TYPE), NULL, 1, NULL) == pdPASS) {
							tt_mode_task_start = 1;
							RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "tt_mode_test_task start\r\n");
						}
						match_len -= enter_len;
						if (match_len > 0) {
							memmove(match_buf, match_buf + enter_len, match_len);
						}
						progressed = 1;
					} else
#endif
						if (match_len >= ds_start_len
							&& memcmp(match_buf, ATCMD_DOWNSTREAM_TEST_START_STR, ds_start_len) == 0) {
							start_time = xTaskGetTickCount();
							ds_count   = 0;
							RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, ATCMD_DOWNSTREAM_TEST_START_STR);
							match_len -= ds_start_len;
							if (match_len > 0) {
								memmove(match_buf, match_buf + ds_start_len, match_len);
							}
							progressed = 1;
						} else if (start_time != 0
								   && match_len >= ds_end_len
								   && memcmp(match_buf, ATCMD_DOWNSTREAM_TEST_END_STR, ds_end_len) == 0) {
							u32 end_time = xTaskGetTickCount();
							RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, ATCMD_DOWNSTREAM_TEST_END_STR);
							if (end_time != start_time) {
								host_uart_format_output(
									"downstream test: Receive %lu KBytes in %lu ms, %d Kbits/sec\n\r",
									ds_count / 1024, (end_time - start_time),
									(int)((ds_count * 8) / (end_time - start_time)));
							}
							start_time = 0;
							ds_count   = 0;
							match_len -= ds_end_len;
							if (match_len > 0) {
								memmove(match_buf, match_buf + ds_end_len, match_len);
							}
							progressed = 1;
						} else {
							/* Could the buffer still grow into a tracked pattern? */
#if TT_MODE_TEST
							if (match_len < enter_len
								&& memcmp(match_buf, ATCMD_ENTER_TT_MODE_STR, match_len) == 0) {
								can_wait = 1;
							}
#endif
							if (!can_wait && match_len < ds_start_len
								&& memcmp(match_buf, ATCMD_DOWNSTREAM_TEST_START_STR, match_len) == 0) {
								can_wait = 1;
							}
							if (!can_wait && start_time != 0 && match_len < ds_end_len
								&& memcmp(match_buf, ATCMD_DOWNSTREAM_TEST_END_STR, match_len) == 0) {
								can_wait = 1;
							}
						}
				}

				if (progressed) {
					continue;
				}
				if (can_wait) {
					break;
				}

				/* Leading byte is not part of any tracked pattern: emit it
				 * directly to HOST_UART (and count toward downstream test). */
				serial_putc(&host_sobj, match_buf[0]);
				if (start_time != 0
#if TT_MODE_TEST
					&& !tt_mode_task_start
#endif
				   ) {
					ds_count++;
				}
				match_len--;
				if (match_len > 0) {
					memmove(match_buf, match_buf + 1, match_len);
				}
			}
		}
	}
}

/* -------------------------------------------------------------------------
 * Entry point
 * ------------------------------------------------------------------------- */

void example_atcmd_host_uart(void)
{
	host_uart_irq_sema = (void *)xSemaphoreCreateCounting(0xFFFF, 0);
	at_uart_rx_sema    = (void *)xSemaphoreCreateCounting(0xFFFF, 0);
	tt_mode_tx_sema    = (void *)xSemaphoreCreateCounting(1, 1);

	/* HOST_UART: user terminal */
	host_sobj.uart_idx = HOST_UART_IDX;
	serial_init(&host_sobj, HOST_UART_TX, HOST_UART_RX);
	serial_baud(&host_sobj, HOST_UART_BAUDRATE);
	serial_format(&host_sobj, 8, ParityNone, 1);
	host_uart_send_string("UART Host AT Command Demo...\r\n",
						  strlen("UART Host AT Command Demo...\r\n"));
	serial_irq_handler(&host_sobj, host_uart_irq, (uint32_t)&host_sobj);
	serial_irq_set(&host_sobj, RxIrq, 1);
	serial_irq_set(&host_sobj, TxIrq, 1);

	/* AT_UART: link to slave AT command device */
	at_sobj.uart_idx = AT_UART_IDX;
	serial_init(&at_sobj, AT_UART_TX, AT_UART_RX);
	serial_baud(&at_sobj, AT_UART_BAUDRATE);
	serial_format(&at_sobj, 8, ParityNone, 1);
	serial_irq_handler(&at_sobj, at_uart_irq, (uint32_t)&at_sobj);
	serial_irq_set(&at_sobj, RxIrq, 1);
	serial_irq_set(&at_sobj, TxIrq, 1);

	printf("example_atcmd_host_uart: UARTs initialized\r\n");

	if (xTaskCreate((void *)uart_irq_handle_task, "uart_irq_handle_task",
					1024 / sizeof(portSTACK_TYPE), NULL, 5, NULL) != pdPASS) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s: create uart_irq_handle_task failed\r\n", __FUNCTION__);
	}

	if (xTaskCreate((void *)at_uart_rx_task, "at_uart_rx_task",
					2048 / sizeof(portSTACK_TYPE), NULL, 4, NULL) != pdPASS) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s: create at_uart_rx_task failed\r\n", __FUNCTION__);
	}

	printf("example_atcmd_host_uart is running...\r\n");
}
