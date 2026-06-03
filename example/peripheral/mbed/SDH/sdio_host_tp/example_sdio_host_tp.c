/*
 *  SDIO Host Bidirectional Throughput Test Example
 *
 *  This example runs on the SDIO Host (master) board.
 *  - Phase 0: Periodically sends "Hello Device" to device, waits for
 *             "Hello Host! I am Here" response
 *  - Phase 1: Continuously sends TP packets to device (1ms interval),
 *             while receiving and verifying packets from device.
 *             Prints TX/RX stats every 1000 received packets.
 *
 *  Note: Uses polling mode for SDIO device->host data detection instead of
 *  SD_IRQ_NOTIFY to avoid symbol conflict with verification library.
 *
 *  Data format: Each CMD53 write must include an INIC_TX_DESC header (16 bytes)
 *  at the start of the buffer, followed by the actual payload. The device-side
 *  SDIO IP uses this header to determine payload offset and size.
 */

#include "ameba_soc.h"
#include "ameba_sd.h"
#include "ameba_intfcfg.h"
#include "ameba_inic.h"
#include "os_wrapper.h"

static const char *const TAG = "SDIO_HOST";

/* ---------- SDIO local register definitions ---------- */
#define SDIO_REG_TX_CTRL				0x00
#define SDIO_REG_HIMR					0x14
#define SDIO_REG_HISR					0x18
#define SDIO_REG_RX0_REQ_LEN			0x1c
#define SDIO_REG_FREE_TXBD_NUM			0x20

/* HISR bits */
#define SDIO_HISR_RX_REQUEST			BIT(0)
#define SDIO_HISR_AVAL_INT				BIT(1)
#define SDIO_HISR_CPWM1				BIT(18)

/* HIMR bits */
#define SDIO_HIMR_RX_REQUEST_MSK		BIT(0)
#define SDIO_HIMR_AVAL_MSK				BIT(1)
#define SDIO_HIMR_CPWM1_MSK			BIT(18)

#define MASK_SDIO_HISR_CLEAR			(SDIO_HISR_AVAL_INT | SDIO_HISR_CPWM1)

/* ---------- Protocol definitions ---------- */
#define PKT_MAGIC						0x5AA5
#define PKT_HEADER_SIZE					6	/* magic(2) + seq(2) + data_len(2) */
#define PKT_CRC_SIZE					2

#define HELLO_DEVICE_MSG				"Hello Device"
#define HELLO_DEVICE_LEN				(sizeof(HELLO_DEVICE_MSG) - 1)
#define HELLO_HOST_RESP					"Hello Host! I am Here"
#define HELLO_HOST_RESP_LEN				(sizeof(HELLO_HOST_RESP) - 1)

/* INIC_TX_DESC / INIC_RX_DESC header size */
#define INIC_TX_DESC_SIZE				sizeof(INIC_TX_DESC)
#define INIC_RX_DESC_SIZE				sizeof(INIC_RX_DESC)

/* Packet type for user data in INIC_TX_DESC */
#define SPDIO_RX_DATA_USER				0x83

/* TP test configuration */
#define TP_PAYLOAD_SIZE					1000
#define TP_PACKET_SIZE					(PKT_HEADER_SIZE + TP_PAYLOAD_SIZE + PKT_CRC_SIZE)
/* Total CMD53 write size: INIC_TX_DESC + TP packet */
#define TP_TOTAL_WRITE_SIZE				(INIC_TX_DESC_SIZE + TP_PACKET_SIZE)
#define TP_SEND_INTERVAL_MS				1
#define TP_STATS_INTERVAL				1000	/* print stats every N received packets */
#define HELLO_SEND_INTERVAL_MS			1000
#define HELLO_MAX_RETRIES				30

/* Hello message total write size: INIC_TX_DESC + hello message */
#define HELLO_WRITE_SIZE				512

/* SDIO polling */
#define SDIO_POLLING_INTERVAL_MS		50
#define SDIO_POLLING_STACK_SIZE			8192

/* Rounding macro */
#ifndef _RND
#define _RND(sz, r) ((((sz) + ((r) - 1)) / (r)) * (r))
#endif

/* TP packet header */
typedef struct {
	u16 magic;
	u16 seq;
	u16 data_len;
} __attribute__((packed)) tp_pkt_header_t;

/* Global state */
static rtos_sema_t vfs_sd_sema;
static rtos_mutex_t sdio_lock;
static volatile u32 g_sdio_himr;
static volatile u32 tp_phase = 0;	/* 0: hello, 1: TP running, 2: done */
static volatile u32 tp_tx_count = 0;
static volatile u32 tp_tx_fail = 0;
static volatile u32 tp_rx_total = 0;
static volatile u32 tp_rx_correct = 0;
static volatile u32 tp_rx_error = 0;
static volatile u8 hello_response_received = 0;

extern SD_HdlTypeDef hsd0;

/* ---------- SD semaphore wrappers ---------- */
static int sd_give_sema(u32 timeout)
{
	UNUSED(timeout);
	return rtos_sema_give(vfs_sd_sema);
}

static int sd_take_sema(u32 timeout)
{
	return rtos_sema_take(vfs_sd_sema, timeout);
}

static void sd_sema_init(void)
{
	rtos_sema_create(&vfs_sd_sema, 0, 1);
	SD_SetSema(sd_take_sema, sd_give_sema);
}

/* ---------- SDIO register access helpers ---------- */
static int sdio_write32(u32 addr, u32 val)
{
	u8 data[4];
	_memcpy(data, &val, 4);
	if (SD_IO_WriteBytes(SDIO_FUNC1, addr, data, 4) == SD_OK) {
		return 0;
	}
	return -1;
}

static int sdio_read32(u32 addr, u8 *val)
{
	u8 data[4];
	if (SD_IO_ReadBytes(SDIO_FUNC1, addr, data, 4) == SD_OK) {
		_memcpy(val, data, 4);
		return 0;
	}
	return -1;
}

static int sdio_read8(u32 addr, u8 *val)
{
	if (SD_IO_ReadBytes(SDIO_FUNC1, addr, val, 1) == SD_OK) {
		return 0;
	}
	return -1;
}

/* ---------- CRC16 calculation ---------- */
static u16 crc16_ccitt(const u8 *data, u32 len)
{
	u16 crc = 0xFFFF;
	u32 i;
	int j;

	for (i = 0; i < len; i++) {
		crc ^= (u16)data[i];
		for (j = 0; j < 8; j++) {
			if (crc & 1) {
				crc = (crc >> 1) ^ 0x8408;
			} else {
				crc >>= 1;
			}
		}
	}
	return crc;
}

/* ---------- Validate a received TP packet ----------
 *  Device-to-host data has an INIC_RX_DESC header (16 bytes) prepended by
 *  the SDIO IP.  Skip it before parsing the TP packet.                     */
static void host_validate_rx_packet(const u8 *pdata, u32 size)
{
	const tp_pkt_header_t *hdr;
	u16 recv_crc, calc_crc;
	u32 total_size;
	const u8 *payload;
	u32 payload_size;
	u32 err_pct;

	tp_rx_total++;

	/* Skip INIC_RX_DESC header prepended by SDIO device IP */
	if (size < INIC_RX_DESC_SIZE) {
		tp_rx_error++;
		RTK_LOGE(TAG, "[Phase 1] Pkt too small for RX_DESC: %lu B\n", size);
		return;
	}
	payload = pdata + INIC_RX_DESC_SIZE;
	payload_size = size - INIC_RX_DESC_SIZE;

	if (payload_size < PKT_HEADER_SIZE + PKT_CRC_SIZE) {
		tp_rx_error++;
		RTK_LOGE(TAG, "[Phase 1] Payload too small: %lu B (min=%d)\n",
				 payload_size, PKT_HEADER_SIZE + PKT_CRC_SIZE);
		return;
	}

	hdr = (const tp_pkt_header_t *)payload;

	if (hdr->magic != PKT_MAGIC) {
		tp_rx_error++;
		RTK_LOGE(TAG, "[Phase 1] Bad magic: 0x%04x (expect 0x%04x) seq=%u\n",
				 hdr->magic, PKT_MAGIC, hdr->seq);
		return;
	}

	total_size = PKT_HEADER_SIZE + hdr->data_len + PKT_CRC_SIZE;
	if (total_size != payload_size) {
		tp_rx_error++;
		RTK_LOGE(TAG, "[Phase 1] Size mismatch: hdr=%lu actual=%u seq=%u\n",
				 total_size, payload_size, hdr->seq);
		return;
	}

	memcpy(&recv_crc, payload + PKT_HEADER_SIZE + hdr->data_len, 2);
	calc_crc = crc16_ccitt(payload, PKT_HEADER_SIZE + hdr->data_len);
	if (recv_crc != calc_crc) {
		tp_rx_error++;
		RTK_LOGE(TAG, "[Phase 1] CRC error: recv=0x%04x calc=0x%04x seq=%u\n",
				 recv_crc, calc_crc, hdr->seq);
		return;
	}

	tp_rx_correct++;

	if (tp_rx_total != 0 && tp_rx_total % TP_STATS_INTERVAL == 0) {
		err_pct = tp_rx_error * 100 / tp_rx_total;
		RTK_LOGI(TAG, "[Stats @ RX=%lu] TX sent:%lu fail:%lu | RX total:%lu ok:%lu err:%lu (%lu%%)\n",
				 tp_rx_total, tp_tx_count, tp_tx_fail, tp_rx_total, tp_rx_correct, tp_rx_error, err_pct);
	}
}

/* ---------- Receive data from device ---------- */
static u8 *sdio_read_rxfifo(u32 size)
{
	u32 allocsize;
	u32 retry = 0;
	u8 *pbuf;
	SD_RESULT res;

	allocsize = _RND(size, SD_BLOCK_SIZE);

	pbuf = (u8 *)rtos_mem_zmalloc(allocsize);
	if (!pbuf) {
		RTK_LOGE(TAG, "Alloc RX buf fail (size=%lu)\n", allocsize);
		return NULL;
	}

	while (1) {
		res = SD_IO_ReadBlocks(SDIO_FUNC1, 0xE000, pbuf, allocsize / SD_BLOCK_SIZE);
		if (res == SD_OK) {
			break;
		} else {
			if (++retry >= 10) {
				rtos_mem_free(pbuf);
				RTK_LOGE(TAG, "Read port fail after %lu retries\n", retry);
				return NULL;
			}
			rtos_time_delay_ms(1);
		}
	}

	DCache_Invalidate((u32)pbuf, allocsize);
	return pbuf;
}

static void sdio_recv_data_process(void)
{
	u8 tmp[4];
	u8 *pbuf;
	u8 rx_len_rdy;
	u32 himr;
	u32 rxfifo_size = 0;
	u8 retry = 0;
	/* C89: declare loop variables at block scope */
	int i, found, search_len, rd_retry;

	/* Disable RX_REQ interrupt */
	himr = g_sdio_himr & (~SDIO_HIMR_RX_REQUEST_MSK);
	sdio_write32(SDIO_REG_HIMR, himr);

	rtos_mutex_take(sdio_lock, MUTEX_WAIT_TIMEOUT);

	do {
		u8 reg8;

		for (rd_retry = 0; rd_retry < 3; rd_retry++) {
			if (sdio_read8(SDIO_REG_RX0_REQ_LEN + 3, &reg8) == 0) {
				break;
			}
		}
		if (rd_retry >= 3) {
			RTK_LOGE(TAG, "Read RX0_REQ_LEN[3] fail after 3 retries\n");
			break;
		}
		rx_len_rdy = reg8 & BIT(7);

		if (rx_len_rdy) {
			retry = 0;
			while (retry < 3) {
				for (rd_retry = 0; rd_retry < 3; rd_retry++) {
					if (sdio_read32(SDIO_REG_RX0_REQ_LEN, tmp) == 0) {
						break;
					}
				}
				if (rd_retry >= 3) {
					RTK_LOGE(TAG, "Read RX0_REQ_LEN fail after 3 retries\n");
					break;
				}
				rxfifo_size = tmp[0] | (tmp[1] << 8) | (tmp[2] << 16);

				if (rxfifo_size == 0) {
					retry++;
					for (rd_retry = 0; rd_retry < 3; rd_retry++) {
						if (sdio_read8(SDIO_REG_RX0_REQ_LEN + 3, &reg8) == 0) {
							break;
						}
					}
				} else {
					break;
				}
			}
			if (rxfifo_size != 0) {
				retry = 0;
				pbuf = sdio_read_rxfifo(rxfifo_size);
				if (pbuf) {
					/* Phase 0: Check for hello response */
					if (tp_phase == 0 && rxfifo_size >= HELLO_HOST_RESP_LEN) {
						found = 0;
						search_len = rxfifo_size - HELLO_HOST_RESP_LEN;
						for (i = 0; i <= search_len; i++) {
							if (memcmp(pbuf + i, HELLO_HOST_RESP, HELLO_HOST_RESP_LEN) == 0) {
								found = 1;
								break;
							}
						}
						if (found) {
							RTK_LOGI(TAG, "[Phase 0] Received: \"%s\" (%lu bytes total)\n",
									 HELLO_HOST_RESP, rxfifo_size);
							hello_response_received = 1;
						}
						/* Silently discard non-hello data during Phase 0 */
					}
					/* Phase 1: Validate TP packet from device */
					else if (tp_phase == 1) {
						host_validate_rx_packet(pbuf, rxfifo_size);
					}
					rtos_mem_free(pbuf);
				} else {
					break;
				}
			} else {
				break;
			}
		} else {
			break;
		}
	} while (1);

	rtos_mutex_give(sdio_lock);

	/* Restore RX_REQ interrupt */
	himr = g_sdio_himr;
	sdio_write32(SDIO_REG_HIMR, himr);
}

/* Polling-based interrupt handler (no SD_IRQ_NOTIFY dependency) */
static void sdio_poll_handler(void)
{
	u8 data[4];
	u32 value;
	u32 sdio_hisr;
	int rd_retry;

	/* Read HISR directly */
	for (rd_retry = 0; rd_retry < 3; rd_retry++) {
		if (sdio_read32(SDIO_REG_HISR, data) == 0) {
			break;
		}
	}
	if (rd_retry >= 3) {
		RTK_LOGE(TAG, "Read HISR fail after 3 retries\n");
		return;
	}
	memcpy(&sdio_hisr, data, 4);

	if (sdio_hisr & g_sdio_himr) {
		sdio_hisr &= g_sdio_himr;

		/* Clear HISR */
		value = sdio_hisr & MASK_SDIO_HISR_CLEAR;
		if (value) {
			sdio_write32(SDIO_REG_HISR, value);
		}

		if (sdio_hisr & SDIO_HISR_RX_REQUEST) {
			sdio_recv_data_process();
		}
	}
}

/* SDIO polling task (replaces interrupt-based approach) */
static void sdio_polling_task(void *param)
{
	UNUSED(param);
	RTK_LOGI(TAG, "Polling task started (interval=%d ms)\n", SDIO_POLLING_INTERVAL_MS);

	while (tp_phase < 2) {
		sdio_poll_handler();
		rtos_time_delay_ms(SDIO_POLLING_INTERVAL_MS);
	}

	RTK_LOGI(TAG, "Polling task stopped\n");
	rtos_task_delete(NULL);
}

/* ---------- SDIO interrupt init ---------- */
static void sdio_init_interrupt(void)
{
	/* Clear all HISR bits */
	sdio_write32(SDIO_REG_HISR, 0xFFFFFFFF);

	/* Disable all interrupts */
	sdio_write32(SDIO_REG_HIMR, 0);

	/* Set interrupt mask (used for polling check, not actual HW interrupt) */
	g_sdio_himr = (u32)(SDIO_HIMR_RX_REQUEST_MSK | SDIO_HIMR_AVAL_MSK | SDIO_HIMR_CPWM1_MSK);

	/* Enable interrupt bits on device side */
	sdio_write32(SDIO_REG_HIMR, g_sdio_himr);
}

/* ---------- Build a TP test packet with INIC_TX_DESC header ---------- */
static void build_tp_packet(u8 *buf, u16 seq, u16 payload_len)
{
	INIC_TX_DESC *ptxdesc = (INIC_TX_DESC *)buf;
	tp_pkt_header_t *pkt_hdr;
	u16 crc;
	u32 i;

	/* Fill INIC_TX_DESC header */
	memset(ptxdesc, 0, INIC_TX_DESC_SIZE);
	ptxdesc->txpktsize = PKT_HEADER_SIZE + payload_len + PKT_CRC_SIZE;
	ptxdesc->offset = INIC_TX_DESC_SIZE;
	ptxdesc->bus_agg_num = 1;
	ptxdesc->type = SPDIO_RX_DATA_USER;

	/* Fill TP packet header (after INIC_TX_DESC) */
	pkt_hdr = (tp_pkt_header_t *)(buf + INIC_TX_DESC_SIZE);
	pkt_hdr->magic = PKT_MAGIC;
	pkt_hdr->seq = seq;
	pkt_hdr->data_len = payload_len;

	/* Fill payload with pattern */
	for (i = 0; i < payload_len; i++) {
		buf[INIC_TX_DESC_SIZE + PKT_HEADER_SIZE + i] = (u8)((i + seq) & 0xFF);
	}

	/* Calculate CRC over TP header + payload */
	crc = crc16_ccitt(buf + INIC_TX_DESC_SIZE, PKT_HEADER_SIZE + payload_len);
	memcpy(buf + INIC_TX_DESC_SIZE + PKT_HEADER_SIZE + payload_len, &crc, 2);
}

/* ---------- Build hello message with INIC_TX_DESC header ---------- */
static void build_hello_packet(u8 *buf)
{
	INIC_TX_DESC *ptxdesc = (INIC_TX_DESC *)buf;

	/* Fill INIC_TX_DESC header */
	memset(ptxdesc, 0, INIC_TX_DESC_SIZE);
	ptxdesc->txpktsize = HELLO_DEVICE_LEN;
	ptxdesc->offset = INIC_TX_DESC_SIZE;
	ptxdesc->bus_agg_num = 1;
	ptxdesc->type = SPDIO_RX_DATA_USER;

	/* Copy hello message after the header */
	memcpy(buf + INIC_TX_DESC_SIZE, HELLO_DEVICE_MSG, HELLO_DEVICE_LEN);
}

/* ---------- TP test main thread ---------- */
static void sdioh_tp_test_thread(void *param)
{
	UNUSED(param);
	u8 *trx_buff;
	u32 alloc_size;
	SD_RESULT ret;
	u32 hello_retry = 0;
	u16 seq = 0;
	u32 consecutive_fail = 0;

	/* ===== Phase 0: Send greeting and wait for response ===== */
	RTK_LOGI(TAG, "========================================\n");
	RTK_LOGI(TAG, "  SDIO Host TP test starting\n");
	RTK_LOGI(TAG, "  Phase 0: Sending greeting to device...\n");
	RTK_LOGI(TAG, "  Max retries=%d  interval=%d ms\n", HELLO_MAX_RETRIES, HELLO_SEND_INTERVAL_MS);
	RTK_LOGI(TAG, "========================================\n");

	tp_phase = 0;
	hello_response_received = 0;

	alloc_size = _RND(HELLO_WRITE_SIZE, SD_BLOCK_SIZE);
	trx_buff = (u8 *)rtos_mem_malloc(alloc_size);
	if (!trx_buff) {
		RTK_LOGE(TAG, "malloc hello buf fail\n");
		goto exit;
	}

	while (!hello_response_received && hello_retry < HELLO_MAX_RETRIES) {
		memset(trx_buff, 0, alloc_size);
		build_hello_packet(trx_buff);

		rtos_mutex_take(sdio_lock, MUTEX_WAIT_TIMEOUT);
		ret = SD_IO_WriteBlocks(SDIO_FUNC1, 0x8000 | (alloc_size >> 2), trx_buff, alloc_size / SD_BLOCK_SIZE);
		rtos_mutex_give(sdio_lock);

		if (ret == SD_OK) {
			RTK_LOGI(TAG, "[Phase 0] Sent \"%s\" (attempt %lu/%d)\n",
					 HELLO_DEVICE_MSG, hello_retry + 1, HELLO_MAX_RETRIES);
		} else {
			RTK_LOGE(TAG, "[Phase 0] Send fail (ret=%d, attempt %lu/%d)\n",
					 ret, hello_retry + 1, HELLO_MAX_RETRIES);
		}

		hello_retry++;
		rtos_time_delay_ms(HELLO_SEND_INTERVAL_MS);
	}

	if (!hello_response_received) {
		RTK_LOGE(TAG, "[Phase 0] No response after %d attempts — abort\n", HELLO_MAX_RETRIES);
		rtos_mem_free(trx_buff);
		goto exit;
	}

	/* ===== Phase 1: Continuous bidirectional TP test ===== */
	tp_phase = 1;
	rtos_mem_free(trx_buff);

	RTK_LOGI(TAG, "========================================\n");
	RTK_LOGI(TAG, "  Phase 1: TP test started\n");
	RTK_LOGI(TAG, "  payload=%d B  interval=%d ms  stats every %d pkts\n",
			 TP_PAYLOAD_SIZE, TP_SEND_INTERVAL_MS, TP_STATS_INTERVAL);
	RTK_LOGI(TAG, "========================================\n");

	alloc_size = _RND(TP_TOTAL_WRITE_SIZE, SD_BLOCK_SIZE);
	trx_buff = (u8 *)rtos_mem_malloc(alloc_size);
	if (!trx_buff) {
		RTK_LOGE(TAG, "malloc TP buf fail\n");
		goto exit;
	}

	while (tp_phase == 1) {
		memset(trx_buff, 0, alloc_size);
		build_tp_packet(trx_buff, seq, TP_PAYLOAD_SIZE);

		rtos_mutex_take(sdio_lock, MUTEX_WAIT_TIMEOUT);
		ret = SD_IO_WriteBlocks(SDIO_FUNC1, 0x8000 | (alloc_size >> 2), trx_buff, alloc_size / SD_BLOCK_SIZE);
		rtos_mutex_give(sdio_lock);

		if (ret != SD_OK) {
			tp_tx_fail++;
			consecutive_fail++;
			if (consecutive_fail >= 100) {
				RTK_LOGE(TAG, "Fatal error: SDIO link broken (100 consecutive TX failures). Aborting.\n");
				break;
			}
			rtos_time_delay_ms(1);
			continue;
		}

		consecutive_fail = 0;
		tp_tx_count++;
		seq++;
		rtos_time_delay_ms(TP_SEND_INTERVAL_MS);
	}

	rtos_mem_free(trx_buff);
	tp_phase = 2;

exit:
	tp_phase = 2;
	rtos_task_delete(NULL);
}

/* ---------- Main entry ---------- */
int example_sdio_host_tp(void)
{
	sd_sema_init();
	rtos_mutex_create(&sdio_lock);

	/* Disable card detect pin check since SDIO device doesn't use CD signal */
	sdioh_config.sdioh_cd_pin = _PNC;

	/* Initialize SD host controller (includes Func1 enable, bus width, speed) */
	if (SD_Init() == SD_OK) {
		RTK_LOGI(TAG, "SDIO host init OK\n");
	} else {
		RTK_LOGE(TAG, "SDIO host init FAIL\n");
		return -1;
	}

	/* Disable card interrupt signal to prevent SD_IRQHandler from firing
	 * on CARD_INT and calling SD_IRQ_NOTIFY() (which is overridden by
	 * the WiFi driver's whc_sdio_init.c and tries to give a NULL semaphore).
	 * We detect device->host data by polling HISR from sdio_polling_task. */
	SDIO_ConfigNormIntSig(hsd0.Instance, SDIOHOST_BIT_CARD_INT_SIGNAL_EN, DISABLE);
	SDIO_ConfigNormIntSts(hsd0.Instance, SDIOHOST_BIT_CARD_INT_STATUS_EN, DISABLE);

	/* Initialize SDIO interrupt mask (for polling) */
	sdio_init_interrupt();

	/* Start polling task */
	if (rtos_task_create(NULL, "sdio_polling", sdio_polling_task, NULL,
						 SDIO_POLLING_STACK_SIZE, 7) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create polling task fail\n");
	}

	/* Start TP test task */
	if (rtos_task_create(NULL, "sdioh_tp", sdioh_tp_test_thread, NULL,
						 SDIO_POLLING_STACK_SIZE * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create TP task fail\n");
	}

	return 0;
}
