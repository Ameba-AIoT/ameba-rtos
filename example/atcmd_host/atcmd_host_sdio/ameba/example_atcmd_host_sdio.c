/*
 * AT Command Host SDIO Example — AmebaGreen2
 *
 * Connects to an Ameba SDIO device running at_intf_sdio.c.
 * Relays AT commands from UART to device and prints responses to UART.
 * Supports downstream throughput test markers and TT (transparent) mode.
 *
 * SDIO protocol:
 *   Host->Device: SD_IO_WriteBlocks(FUNC1, 0x8000|(rnd>>2), buf, rnd/512)
 *                 Prepend INIC_TX_DESC (16B) before payload.
 *   Device->Host: SD_IO_ReadBlocks(FUNC1, 0xE000, buf, rnd/512)
 *                 Device SDIO IP prepends INIC_RX_DESC (16B); strip it.
 *   Registers:    SD_IO_ReadBytes/WriteBytes(FUNC1, offset, buf, n)
 */

#include "ameba_soc.h"
#include <stdlib.h>
#include "ameba_sd.h"
#include "ameba_inic.h"
#include "ameba_intfcfg.h"
#include "os_wrapper.h"
#include "serial_api.h"
#include "serial_ex_api.h"
#include "example_atcmd_host_sdio.h"

/* ---------- User-configurable UART pins ---------- */
#define HOST_UART_TX        PA_14
#define HOST_UART_RX        PA_15
#define HOST_UART_BAUDRATE  115200
#define MAX_CMD_LEN         2000

/* ---------- SDIO device register offsets (local domain) ---------- */
#define SDIO_REG_HIMR            0x14U
#define SDIO_REG_HISR            0x18U
#define SDIO_REG_RX0_REQ_LEN     0x1CU
#define SDIO_REG_FREE_TXBD_NUM   0x20U

#define SDIO_HISR_RX_REQUEST      BIT(0)
#define SDIO_HIMR_RX_REQUEST_MSK  BIT(0)

/* ---------- Protocol ---------- */
#define SPDIO_RX_DATA_USER   0x83U
#define INIC_TX_DESC_SIZE    ((u32)sizeof(INIC_TX_DESC))  /* 16 B */
#define INIC_RX_DESC_SIZE    ((u32)sizeof(INIC_RX_DESC))  /* 16 B */
#define ATCMD_SDIO_MAX_SIZE  (2048U - 16U)                /* max AT payload per frame */
/* RX FIFO static buffer: _RND(ATCMD_SDIO_MAX_SIZE + INIC_RX_DESC_SIZE, SD_BLOCK_SIZE)
 * = _RND(2048, 512) = 2048 (4 blocks). Add one block margin for device edge cases. */
#define SDIO_RX_MAX_RNDSIZE  (5U * SD_BLOCK_SIZE)         /* 2560 B — 5 blocks */

#ifndef _RND
#define _RND(sz, r)  ((((sz) + ((r) - 1U)) / (r)) * (r))
#endif

/* ---------- AT strings ---------- */
#define DS_TEST_START_STR   "Downstream Test Start\r\n"
#define DS_TEST_END_STR     "Downstream Test End\r\n"
#define TT_HIGH_WM_STR      "[$][TT]:High Watermark\r\n"
#define TT_LOW_WM_STR       "[$][TT]:Low Watermark\r\n"

static const char *const TAG = "ATCMD_SDIOH";

/* ---------- SD driver semaphore (required by SD_SetSema) ---------- */
static rtos_sema_t vfs_sd_sema;

/* ---------- Sync objects ---------- */
static rtos_sema_t  host_rx_sema;                        /* given by SD_IRQ_Notify, taken by rx task */
static rtos_sema_t  uart_irq_sema; /* given by UART IRQ, taken by uart input task */
static rtos_sema_t  tt_tx_sema;   /* gate: 1=open, 0=closed by High Watermark */
static rtos_mutex_t sdio_tx_mutex;                         /* serialize concurrent TX (uart + TT tasks) */

/* ---------- UART ---------- */
static serial_t sobj;
static char     uart_irq_buf[MAX_CMD_LEN];
static u32      uart_irq_cnt;

/* ---------- TT mode ---------- */
static volatile u8  g_tt_mode;
static volatile u32 g_tt_len;

/* ---------- Pre-allocated RX DMA buffer ----------
 * Used exclusively by atcmd_sdio_rx_task (no locking needed).
 * Must be cache-line aligned: SD DMA writes bypass cache; DCache_Invalidate
 * is called after each ReadBlocks so the CPU sees the fresh DMA data. */
static u8 g_sdio_rx_buf[SDIO_RX_MAX_RNDSIZE] ALIGNMTO(CACHE_LINE_SIZE);

/* ---------- hsd0 exported by ameba_sd.c ---------- */
extern SD_HdlTypeDef hsd0;

static int sd_give_sema(u32 timeout)
{
	UNUSED(timeout);
	return rtos_sema_give(vfs_sd_sema);
}

static int sd_take_sema(u32 timeout)
{
	return rtos_sema_take(vfs_sd_sema, timeout);
}

/*
 * Called from SD_IRQHandler (ISR context) when SDIO device asserts CARD INT.
 * rtos_sema_give is ISR-safe: internally uses xSemaphoreGiveFromISR.
 * Disable CARD_INT_STATUS_EN here to prevent interrupt storm while DAT1
 * remains asserted; RX task re-enables it after draining the FIFO.
 */
void SD_IRQ_Notify(void)
{
	SDIO_ConfigNormIntSts(hsd0.Instance, SDIOHOST_BIT_CARD_INT_STATUS_EN, DISABLE);
	rtos_sema_give(host_rx_sema);
}

static int sdio_read_reg(u32 addr, u8 *buf, u16 len)
{
	return (SD_IO_ReadBytes(SDIO_FUNC1, addr, buf, len) == SD_OK) ? 0 : -1;
}

static int sdio_write_reg(u32 addr, u8 *buf, u16 len)
{
	return (SD_IO_WriteBytes(SDIO_FUNC1, addr, buf, len) == SD_OK) ? 0 : -1;
}

static int sdio_host_init(void)
{
	u8  buf[4];
	u32 val;

	/* SDIO device has no card-detect signal */
	sdioh_config.sdioh_cd_pin = _PNC;

	/* SD driver needs semaphore callbacks for DMA completion */
	rtos_sema_create(&vfs_sd_sema, 0, 1);
	SD_SetSema(sd_take_sema, sd_give_sema);

	if (SD_Init() != SD_OK) {
		RTK_LOGE(TAG, "SD_Init fail\n");
		return -1;
	}
	RTK_LOGI(TAG, "SD_Init OK\n");

	/* Clear all device HISR bits */
	val = 0xFFFFFFFFU;
	_memcpy(buf, &val, 4);
	sdio_write_reg(SDIO_REG_HISR, buf, 4);

	/* Enable device HIMR: only RX_REQUEST triggers CARD INT */
	val = SDIO_HIMR_RX_REQUEST_MSK;
	_memcpy(buf, &val, 4);
	sdio_write_reg(SDIO_REG_HIMR, buf, 4);

	/* Enable host card-interrupt status and signal -> SD_IRQ_Notify will fire */
	SDIO_ConfigNormIntSts(hsd0.Instance, SDIOHOST_BIT_CARD_INT_STATUS_EN, ENABLE);
	SDIO_ConfigNormIntSig(hsd0.Instance, SDIOHOST_BIT_CARD_INT_SIGNAL_EN, ENABLE);

	return 0;
}

/*
 * Send AT command (or TT data) to SDIO device.
 * Buffer layout: INIC_TX_DESC (16B) + payload, rounded up to 512B boundary.
 * TX FIFO write address: 0x8000 | (rounded_total >> 2).
 * Both uart_input_task and tt_tx_task call this — serialized by sdio_tx_mutex.
 */
static int sdio_send_to_device(const u8 *data, u32 len)
{
	u32 total, rnd_total;
	u8 *buf;
	INIC_TX_DESC *ptxdesc;
	SD_RESULT res;

	total     = INIC_TX_DESC_SIZE + len;
	rnd_total = _RND(total, SD_BLOCK_SIZE);

	buf = (u8 *)rtos_mem_zmalloc(rnd_total);
	if (!buf) {
		RTK_LOGE(TAG, "TX alloc fail len=%u\n", len);
		return -1;
	}

	ptxdesc = (INIC_TX_DESC *)buf;
	ptxdesc->txpktsize   = (u16)len;
	ptxdesc->offset      = (u8)INIC_TX_DESC_SIZE;
	ptxdesc->bus_agg_num = 1;
	ptxdesc->type        = SPDIO_RX_DATA_USER;

	_memcpy(buf + INIC_TX_DESC_SIZE, data, len);

	rtos_mutex_take(sdio_tx_mutex, MUTEX_WAIT_TIMEOUT);
	res = SD_IO_WriteBlocks(SDIO_FUNC1,
							0x8000U | (rnd_total >> 2),
							buf,
							rnd_total / SD_BLOCK_SIZE);
	rtos_mutex_give(sdio_tx_mutex);

	rtos_mem_free(buf);

	if (res != SD_OK) {
		RTK_LOGE(TAG, "TX WriteBlocks fail len=%u\n", len);
		return -1;
	}
	return 0;
}

static void uart_send(const char *str, u32 len)
{
	u32 i;
	for (i = 0; i < len; i++) {
		serial_putc(&sobj, str[i]);
	}
}

static void atcmd_sdio_tt_tx_task(void *param)
{
	UNUSED(param);
	u8  *tt_buf;
	u32  chunk;
	u8   txbd_buf[1];

	tt_buf = (u8 *)rtos_mem_malloc(ATCMD_SDIO_MAX_SIZE);
	if (!tt_buf) {
		RTK_LOGE(TAG, "TT buf alloc fail\n");
		g_tt_mode = 0;
		rtos_task_delete(NULL);
		return;
	}
	_memset(tt_buf, 0xAAU, ATCMD_SDIO_MAX_SIZE);

	RTK_LOGI(TAG, "TT task start, total=%u B\n", g_tt_len);

	while (g_tt_len > 0) {
		/*
		 * Peek gate: block here when High Watermark closed it.
		 * Take then immediately give back so the gate is never held
		 * during the actual send — lets RX task close it non-blockingly.
		 */
		rtos_sema_take(tt_tx_sema, RTOS_MAX_DELAY);
		rtos_sema_give(tt_tx_sema);

		/* Wait for a free TX buffer descriptor on the device */
		do {
			if (sdio_read_reg(SDIO_REG_FREE_TXBD_NUM,
							  txbd_buf, 1) < 0 || txbd_buf[0] == 0) {
				rtos_time_delay_ms(1);
			} else {
				break;
			}
		} while (1);

		chunk = (g_tt_len > ATCMD_SDIO_MAX_SIZE) ?
				ATCMD_SDIO_MAX_SIZE : g_tt_len;

		if (sdio_send_to_device(tt_buf, chunk) == 0) {
			g_tt_len -= chunk;
		} else {
			RTK_LOGE(TAG, "TT send fail\n");
			rtos_time_delay_ms(1);
		}
	}

	rtos_mem_free(tt_buf);
	g_tt_mode = 0;
	RTK_LOGI(TAG, "TT task done\n");
	rtos_task_delete(NULL);
}

static void handle_rx_payload(const u8 *payload, u32 len)
{
	static u32 ds_start_ms = 0;
	static u32 ds_bytes    = 0;
	char       stat_buf[80];
	u32        elapsed;
	u32        pos;
	const u8  *end_ptr;

	if (len == 0) {
		return;
	}

	/* TT High Watermark: close the tx gate (non-blocking) */
	if (len >= sizeof(TT_HIGH_WM_STR) - 1 &&
		_memcmp(payload, TT_HIGH_WM_STR, sizeof(TT_HIGH_WM_STR) - 1) == 0) {
		rtos_sema_take(tt_tx_sema, 0);
		uart_send(TT_HIGH_WM_STR, sizeof(TT_HIGH_WM_STR) - 1);
		return;
	}

	/* TT Low Watermark: open the tx gate */
	if (len >= sizeof(TT_LOW_WM_STR) - 1 &&
		_memcmp(payload, TT_LOW_WM_STR, sizeof(TT_LOW_WM_STR) - 1) == 0) {
		rtos_sema_give(tt_tx_sema);
		uart_send(TT_LOW_WM_STR, sizeof(TT_LOW_WM_STR) - 1);
		return;
	}

	/* TT entry: device sends ">>>" after host issues a TT AT command */
	if (len >= 3 && _memcmp(payload, ">>>", 3) == 0 && g_tt_len > 0) {
		RTK_LOGI(TAG, "TT mode: entering, total=%u B\n", g_tt_len);
		uart_send(">>>\r\n", 5);
		g_tt_mode = 1;
		if (rtos_task_create(NULL, "sdio_tt_tx", atcmd_sdio_tt_tx_task,
							 NULL, 2048, 4) != RTK_SUCCESS) {
			RTK_LOGE(TAG, "TT task create fail\n");
			g_tt_mode = 0;
		}
		return;
	}

	/* Downstream throughput test: start marker */
	if (len >= sizeof(DS_TEST_START_STR) - 1 &&
		_memcmp(payload, DS_TEST_START_STR, sizeof(DS_TEST_START_STR) - 1) == 0) {
		ds_start_ms = rtos_time_get_current_system_time_ms();
		ds_bytes    = 0;
		uart_send(DS_TEST_START_STR, sizeof(DS_TEST_START_STR) - 1);
		return;
	}

	/* Downstream throughput test: accumulate / detect end marker */
	if (ds_start_ms != 0) {
		end_ptr = NULL;
		for (pos = 0; pos + (sizeof(DS_TEST_END_STR) - 1) <= len; pos++) {
			if (_memcmp(payload + pos, DS_TEST_END_STR,
						sizeof(DS_TEST_END_STR) - 1) == 0) {
				end_ptr = payload + pos;
				break;
			}
		}
		if (end_ptr) {
			elapsed  = rtos_time_get_current_system_time_ms() - ds_start_ms;
			ds_bytes += (u32)(end_ptr - payload);
			if (elapsed == 0) {
				elapsed = 1;
			}
			snprintf(stat_buf, sizeof(stat_buf),
					 "DS: %u KB in %u ms = %u Kbps\r\n",
					 (unsigned int)(ds_bytes / 1024U),
					 (unsigned int)elapsed,
					 (unsigned int)((ds_bytes * 8U) / elapsed));
			uart_send(stat_buf, (u32)strlen(stat_buf));
			ds_start_ms = 0;
			ds_bytes    = 0;
		} else {
			ds_bytes += len;
		}
		return;
	}

	/* Normal AT response: forward to UART */
	uart_send((const char *)payload, len);
}

static void sdio_drain_rx_fifo(void)
{
	u8  tmp[4];
	u8  rx_rdy;
	u32 rx_size, rnd_size;
	INIC_RX_DESC *prxdesc;
	u16 pkt_len;
	int rd_retry;

	do {
		/* Check RX_LEN_RDY (bit 7 of byte 3 in RX0_REQ_LEN register) */
		if (sdio_read_reg(SDIO_REG_RX0_REQ_LEN + 3, &rx_rdy, 1) < 0) {
			break;
		}
		if (!(rx_rdy & BIT(7))) {
			break;
		}

		/* Read 24-bit packet size from bytes 0-2 */
		if (sdio_read_reg(SDIO_REG_RX0_REQ_LEN, tmp, 4) < 0) {
			break;
		}
		rx_size = (u32)tmp[0] | ((u32)tmp[1] << 8) | ((u32)tmp[2] << 16);
		if (rx_size == 0) {
			break;
		}

		rnd_size = _RND(rx_size, SD_BLOCK_SIZE);
		if (rnd_size > SDIO_RX_MAX_RNDSIZE) {
			RTK_LOGE(TAG, "RX pkt too large: %u > %u\n", rnd_size, SDIO_RX_MAX_RNDSIZE);
			break;
		}

		/* Read from RX FIFO into pre-allocated buffer (address 0xE000, block mode) */
		for (rd_retry = 0; rd_retry < 3; rd_retry++) {
			if (SD_IO_ReadBlocks(SDIO_FUNC1, 0xE000U,
								 g_sdio_rx_buf, rnd_size / SD_BLOCK_SIZE) == SD_OK) {
				break;
			}
		}
		if (rd_retry >= 3) {
			RTK_LOGE(TAG, "RX ReadBlocks fail\n");
			break;
		}

		/* SD DMA writes bypass DCache; invalidate so CPU sees fresh data */
		DCache_Invalidate((u32)g_sdio_rx_buf, rnd_size);

		/* Strip INIC_RX_DESC header (16B); pkt_len is the payload size */
		if (rx_size < INIC_RX_DESC_SIZE) {
			RTK_LOGE(TAG, "RX pkt too small: %u\n", rx_size);
			break;
		}
		prxdesc = (INIC_RX_DESC *)g_sdio_rx_buf;
		pkt_len = prxdesc->pkt_len;

		if (pkt_len > 0) {
			handle_rx_payload(g_sdio_rx_buf + INIC_RX_DESC_SIZE, (u32)pkt_len);
		}
	} while (1);
}

static void atcmd_sdio_rx_task(void *param)
{
	UNUSED(param);

	while (1) {
		rtos_sema_take(host_rx_sema, RTOS_MAX_DELAY);

		/*
		 * No need to read HISR here: HIMR is configured with only
		 * SDIO_HIMR_RX_REQUEST_MSK, so the only event that can trigger
		 * SD_IRQ_Notify (and thus give host_rx_sema) is RX_REQUEST.
		 * Reading HISR would be a redundant CMD52 round-trip per packet.
		 *
		 * Add back the HISR read if HIMR is expanded to enable additional
		 * bits (e.g. AVAL_INT, CPWM1), so the correct handler can be
		 * dispatched based on which event actually fired.
		 */
		/*
		 * SD_IO_ReadBlocks (drain) and SD_IO_WriteBlocks (send_to_device) share
		 * the same SD host controller.  Take sdio_tx_mutex to prevent concurrent
		 * RX and TX operations, which cause SDH NOT READY on rapid RX→TX transitions.
		 */
		rtos_mutex_take(sdio_tx_mutex, MUTEX_WAIT_TIMEOUT);
		sdio_drain_rx_fifo();
		rtos_mutex_give(sdio_tx_mutex);

		/* Re-enable CARD_INT_STATUS_EN (disabled in SD_IRQ_Notify) */
		SDIO_ConfigNormIntSts(hsd0.Instance,
							  SDIOHOST_BIT_CARD_INT_STATUS_EN, ENABLE);
	}
}

static void atcmd_uart_irq_cb(uint32_t id, SerialIrq event)
{
	serial_t *psobj = (serial_t *)id;

	if (event != RxIrq) {
		return;
	}

	while (serial_readable(psobj)) {
		if (uart_irq_cnt >= MAX_CMD_LEN) {
			break;
		}
		uart_irq_buf[uart_irq_cnt] = (char)serial_getc(psobj);
		serial_putc(psobj, uart_irq_buf[uart_irq_cnt]);  /* echo */
		uart_irq_cnt++;
	}

	/* Give semaphore on \r\n or when buffer is full */
	if ((uart_irq_cnt >= 2 &&
		 uart_irq_buf[uart_irq_cnt - 1] == '\n' &&
		 uart_irq_buf[uart_irq_cnt - 2] == '\r') ||
		uart_irq_cnt >= MAX_CMD_LEN) {
		rtos_sema_give(uart_irq_sema);
	}
}

static void atcmd_sdio_uart_input_task(void *param)
{
	UNUSED(param);
	static char cmd[MAX_CMD_LEN + 1];
	u32  cmd_len;

	while (1) {
		rtos_sema_take(uart_irq_sema, RTOS_MAX_DELAY);

		cmd_len = uart_irq_cnt;
		_memcpy(cmd, uart_irq_buf, cmd_len);
		cmd[cmd_len] = '\0';
		uart_irq_cnt = 0;

		/* AT+TEST=1,<N> sets the TT transmission length */
		if (cmd_len > 10 && _memcmp(cmd, "AT+TEST=1,", 10) == 0) {
			g_tt_len = (u32)atoi(cmd + 10);
			RTK_LOGI(TAG, "TT len=%u\n", g_tt_len);
		}

		if (sdio_send_to_device((const u8 *)cmd, cmd_len) < 0) {
			RTK_LOGE(TAG, "UART->SDIO TX fail\n");
		}
	}
}

static void uart_init(void)
{
	sobj.uart_idx = 0;
	serial_init(&sobj, HOST_UART_TX, HOST_UART_RX);
	serial_baud(&sobj, HOST_UART_BAUDRATE);
	serial_format(&sobj, 8, ParityNone, 1);
	serial_irq_handler(&sobj, atcmd_uart_irq_cb, (uint32_t)&sobj);
	serial_irq_set(&sobj, RxIrq, 1);
	serial_irq_set(&sobj, TxIrq, 1);
	uart_send("AT Command Host (SDIO) ready\r\n", 30);
}

static void atcmd_sdio_main_task(void *param)
{
	UNUSED(param);

	/* Create sync objects */
	rtos_sema_create(&host_rx_sema,  0, 0xFFFFU);
	rtos_sema_create(&uart_irq_sema, 0, 0xFFFFU);
	rtos_sema_create(&tt_tx_sema,    1, 1);       /* gate starts open */
	rtos_mutex_create(&sdio_tx_mutex);

	uart_irq_cnt = 0;
	g_tt_mode    = 0;
	g_tt_len     = 0;

	/* UART first so we can print init progress */
	uart_init();

	if (sdio_host_init() < 0) {
		RTK_LOGE(TAG, "SDIO init failed\n");
		uart_send("SDIO init FAIL\r\n", 16);
		rtos_task_delete(NULL);
		return;
	}
	uart_send("SDIO init OK\r\n", 14);

	if (rtos_task_create(NULL, "sdio_rx", atcmd_sdio_rx_task,
						 NULL, 4096, 5) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create sdio_rx task fail\n");
	}

	if (rtos_task_create(NULL, "sdio_uart_in", atcmd_sdio_uart_input_task,
						 NULL, 2048, 5) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create uart_input task fail\n");
	}

	rtos_task_delete(NULL);
}

int example_atcmd_host_sdio(void)
{
	if (rtos_task_create(NULL, "sdio_main", atcmd_sdio_main_task,
						 NULL, 8192, 5) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create sdio_main task fail\n");
		return -1;
	}
	return 0;
}
