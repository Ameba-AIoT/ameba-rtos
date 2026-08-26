/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"
#include "ameba_gspi.h"
#include "example_gspi_fifo_ext.h"
#include "os_wrapper.h"

#define TAG "GSPI_HOST"

/* ---------- Tunable test parameters ---------- */
#define GSPI_BUS_FREQ_HZ       10000000   /* bus clock after activation, max 25MHz */
#define GSPI_FREQ_ACTIVATE     1000000    /* clock used for the activation frame */
#define TP_TX_PAYLOAD_SIZE     1000       /* Host -> Device bytes per packet, 0 = RX only */
#define TP_TX_INTERVAL_MS      0          /* Host TX pacing, 0 = as fast as TX BDs allow */
#define STATS_INTERVAL_MS      5000
#define RX_READS_PER_ROUND     8          /* bounds one drain round */
#define TEST_DURATION_MS       0          /* 0 = run forever */
#define GSPI_USE_DMA           1          /* GDMA for FIFO data phases */
#define GSPI_DMA_THRESHOLD     64         /* data bytes from which DMA is used */

/* ---------- Derived / fixed sizing ---------- */
#define TP_MAX_PAYLOAD_SIZE    2000
#define TP_PKT_MAGIC           0x5AA5
#define TP_PKT_HEADER_SIZE     6          /* magic(2) + seq(2) + data_len(2) */
#define TP_PKT_CRC_SIZE        2
#define TP_MAX_PKT_SIZE        (TP_PKT_HEADER_SIZE + TP_MAX_PAYLOAD_SIZE + TP_PKT_CRC_SIZE)
/* The Device may aggregate several packets into one RX_REQ_LEN. */
#define RX_MAX_LEN             3072

/* INIC_TX_DESC packet type. Values come from enum spdio_rx_data_t: NULL 0x00,
 * ETH 0x83, ATCMD 0x11, USER 0x41. The Device passes the field through to its
 * rx_done_cb without acting on it, but user data belongs at 0x41. */
#define TP_PKT_TYPE_USER       0x41

#define HELLO_DEVICE_MSG       "Hello Device"
#define HELLO_HOST_RESP        "Hello Host! I am Here"

/* 16-byte TX descriptor, matching ameba_inic.h's INIC_TX_DESC (4 dwords) --
 * NOT UM0505's 24-byte Table 14-4 layout. See README. */
typedef struct {
	u16 pktsize;    /* payload bytes only, excluding this header */
	u8  offset;     /* = sizeof(this struct) */
	u8  byte3;      /* bus_agg_num */
	u8  type;
	u8  byte5;
	u16 word3;
	u32 start_addr;
	u16 data_len;
	u16 rsv_last;
} __attribute__((packed)) gspi_desc_t;

typedef struct {
	u16 magic;
	u16 seq;
	u16 data_len;
} __attribute__((packed)) tp_pkt_header_t;

static GSPI_HandleTypeDef gspi;

/* Frame buffers: [CMD 4B][data, 4-byte aligned][Status 8B], cacheline aligned
 * for the DMA data phase. File scope so they never land on the task stack. */
static u8 gspi_txbuf[GSPI_FIFO_BUF_SIZE(GSPI_TX_DESC_SIZE + TP_MAX_PKT_SIZE)] ALIGNMTO(32);
static u8 gspi_rxbuf[GSPI_FIFO_BUF_SIZE(RX_MAX_LEN)] ALIGNMTO(32);

/* Statistics */
static u32 tp_tx_ok;
static u32 tp_tx_nobd;
static u32 tp_tx_err;
static u32 tp_rx_bytes;
static u32 tp_rx_ok;
static u32 tp_rx_err;

/* CRC16-CCITT, same polynomial and seed as mbed_sdio_device_tp. */
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

/* Send one packet: descriptor at buf[4], payload right after it. */
static int tp_tx_packet(const u8 *payload, u16 payload_len)
{
	gspi_desc_t *desc = (gspi_desc_t *)(gspi_txbuf + GSPI_CMD_LEN);
	GSPI_StatusTypeDef sts;

	/* txpktsize carries the payload length only -- it must NOT include the
	 * descriptor. See the Note section of this example's README. */
	_memset(desc, 0, sizeof(gspi_desc_t));
	desc->pktsize = payload_len;
	desc->offset = GSPI_TX_DESC_SIZE;
	desc->byte3 = 1;                 /* bus_agg_num = 1, non-aggregated */
	desc->type = TP_PKT_TYPE_USER;
	desc->data_len = payload_len;
	_memcpy(gspi_txbuf + GSPI_CMD_LEN + GSPI_TX_DESC_SIZE, payload, payload_len);

	return GSPI_WriteTxFifo(&gspi, gspi_txbuf, GSPI_TX_DESC_SIZE + payload_len, &sts);
}

/* Build one TP packet: [header][incrementing payload][CRC16]. */
static void tp_build_packet(u8 *buf, u16 seq, u16 payload_len)
{
	tp_pkt_header_t *hdr = (tp_pkt_header_t *)buf;
	u16 crc;
	u16 i;

	hdr->magic = TP_PKT_MAGIC;
	hdr->seq = seq;
	hdr->data_len = payload_len;

	for (i = 0; i < payload_len; i++) {
		buf[TP_PKT_HEADER_SIZE + i] = (u8)((i + seq) & 0xFF);
	}

	crc = crc16_ccitt(buf, TP_PKT_HEADER_SIZE + payload_len);
	_memcpy(buf + TP_PKT_HEADER_SIZE + payload_len, &crc, TP_PKT_CRC_SIZE);
}

/* Validate one bare [header][payload][CRC16] unit and return bytes consumed,
 * or 0 when the data no longer looks like a packet (end of data / padding). */
static u32 tp_validate_packet(const u8 *pdata, u32 available)
{
	const tp_pkt_header_t *hdr;
	u16 recv_crc, calc_crc;
	u32 pkt_size;

	if (available < TP_PKT_HEADER_SIZE + TP_PKT_CRC_SIZE) {
		return 0;
	}

	hdr = (const tp_pkt_header_t *)pdata;
	if (hdr->magic != TP_PKT_MAGIC) {
		return 0;
	}

	pkt_size = TP_PKT_HEADER_SIZE + hdr->data_len + TP_PKT_CRC_SIZE;
	if (pkt_size > available) {
		return 0;
	}

	_memcpy(&recv_crc, pdata + TP_PKT_HEADER_SIZE + hdr->data_len, TP_PKT_CRC_SIZE);
	calc_crc = crc16_ccitt(pdata, TP_PKT_HEADER_SIZE + hdr->data_len);
	if (recv_crc == calc_crc) {
		tp_rx_ok++;
	} else {
		tp_rx_err++;
	}
	return pkt_size;
}

/* Drain what the Device currently has queued, validating as we go. Capped at
 * RX_READS_PER_ROUND: the Device streams continuously, so an uncapped loop would
 * starve the TX direction and the statistics output.
 *
 * What the payload contains is up to the Device application: SPDIO_DeviceTx()
 * sends the caller's buffer verbatim and never prepends an INIC_RX_DESC, and
 * mbed_sdio_device_tp passes raw payload. A WHC/iNIC-style Device would prepend
 * a descriptor that the caller would have to parse. */
static void tp_drain_rx(void)
{
	GSPI_StatusTypeDef sts;
	u32 pending;
	u32 off;
	u32 consumed;
	u32 round;

	for (round = 0; round < RX_READS_PER_ROUND; round++) {
		pending = GSPI_GetRxLen(&gspi, NULL);
		if ((pending == 0) || (pending > RX_MAX_LEN)) {
			if (pending > RX_MAX_LEN) {
				RTK_LOGE(TAG, "rx: pending %d exceeds buffer\n", (int)pending);
			}
			break;
		}
		if (GSPI_ReadRxFifo(&gspi, gspi_rxbuf, pending, &sts) != GSPI_OK) {
			tp_rx_err++;
			break;
		}
		tp_rx_bytes += pending;

		/* A single read can carry several aggregated packets back to back. */
		off = 0;
		while (off < pending) {
			consumed = tp_validate_packet(&gspi_rxbuf[GSPI_CMD_LEN + off], pending - off);
			if (consumed == 0) {
				break;
			}
			off += consumed;
		}
	}
}

/* Activate GSPI mode and complete the Device's Phase 0 handshake. */
static int gspi_host_link_up(void)
{
	GSPI_InitTypeDef init;
	GSPI_StatusTypeDef sts;
	u32 pending;
	u8 cpu_ind;

	GSPI_StructInit(&init);
	init.GSPI_Index = 0;
	init.GSPI_ClkPin = GSPI_SCLK;
	init.GSPI_MosiPin = GSPI_MOSI;
	init.GSPI_MisoPin = GSPI_MISO;
	init.GSPI_CsPin = GSPI_CS;
	init.GSPI_ClkFreq = GSPI_BUS_FREQ_HZ;
	init.GSPI_ClkFreqInit = GSPI_FREQ_ACTIVATE;
	init.GSPI_DmaEn = GSPI_USE_DMA;
	init.GSPI_DmaThreshold = GSPI_DMA_THRESHOLD;
	if (GSPI_Init(&gspi, &init) != GSPI_OK) {
		RTK_LOGE(TAG, "GSPI_Init failed\n");
		return -1;
	}

	/* CS is already latched high by GSPI_Init; give the Device time to bring up
	 * its SPDIO backend before the first bus transaction. */
	rtos_time_delay_ms(3000);

	if (GSPI_Configuration(&gspi, GSPI_BIG_ENDIAN_32) != GSPI_OK) {
		return -1;
	}
	cpu_ind = GSPI_ReadReg8(&gspi, GSPI_REG_CPU_IND, NULL);

	/* Clear, then unmask, mirroring raw_sdio_host_tp's ordering. */
	GSPI_INTClear(&gspi, 0xFFFFFFFF);
	GSPI_WriteReg32(&gspi, GSPI_REG_RX_AGG, 0, NULL);
	GSPI_INTConfig(&gspi, GSPI_BIT_RX_REQUEST | GSPI_BIT_AVAL_INT | GSPI_BIT_CPWM1_INT, ENABLE);

	if (tp_tx_packet((const u8 *)HELLO_DEVICE_MSG, sizeof(HELLO_DEVICE_MSG) - 1) != GSPI_OK) {
		RTK_LOGE(TAG, "handshake failed: TX write\n");
		return -1;
	}

	pending = 0;
	for (int i = 0; (i < 3000) && (pending == 0); i++) {
		pending = GSPI_GetRxLen(&gspi, NULL);
		if (pending == 0) {
			rtos_time_delay_ms(1);
		}
	}
	if ((pending == 0) || (GSPI_ReadRxFifo(&gspi, gspi_rxbuf, pending, &sts) != GSPI_OK)) {
		RTK_LOGE(TAG, "handshake failed: no reply\n");
		return -1;
	}
	if (_memcmp(&gspi_rxbuf[GSPI_CMD_LEN], HELLO_HOST_RESP, sizeof(HELLO_HOST_RESP) - 1) != 0) {
		RTK_LOGE(TAG, "handshake failed: unexpected reply (%d bytes)\n", (int)pending);
		return -1;
	}

	RTK_LOGI(TAG, "link up: CPU_RDY=%d HIMR=0x%08x dma=%d, handshake OK\n",
			 cpu_ind & GSPI_BIT_CPU_RDY, GSPI_GetINTMask(&gspi), GSPI_DmaEnabled(&gspi));
	return 0;
}

static void gspi_host_task(void)
{
	u32 start_ms;
	u32 last_stats_ms;
	u32 elapsed = 0;
#if (TP_TX_PAYLOAD_SIZE > 0)
	static u8 tp_txpkt[TP_PKT_HEADER_SIZE + TP_MAX_PAYLOAD_SIZE + TP_PKT_CRC_SIZE];
	u16 seq = 0;
	int ret;
#if (TP_TX_INTERVAL_MS > 0)
	u32 last_tx_ms;
#endif
#endif

	if (gspi_host_link_up() != 0) {
		RTK_LOGE(TAG, "GSPI Demo: fail\n");
		rtos_task_delete(NULL);
		return;
	}

	RTK_LOGI(TAG, "TP test started: bus=%dHz tx_payload=%dB interval=%dms duration=%dms\n",
			 GSPI_BUS_FREQ_HZ, TP_TX_PAYLOAD_SIZE, TP_TX_INTERVAL_MS, TEST_DURATION_MS);

	start_ms = rtos_time_get_current_system_time_ms();
	last_stats_ms = start_ms;
#if (TP_TX_PAYLOAD_SIZE > 0) && (TP_TX_INTERVAL_MS > 0)
	last_tx_ms = start_ms;
#endif

	for (;;) {
		u32 now = rtos_time_get_current_system_time_ms();

		elapsed = now - start_ms;
#if (TEST_DURATION_MS != 0)
		if (elapsed >= TEST_DURATION_MS) {
			break;
		}
#endif

		/* Host -> Device */
#if (TP_TX_PAYLOAD_SIZE > 0)
#if (TP_TX_INTERVAL_MS > 0)
		if ((now - last_tx_ms) >= TP_TX_INTERVAL_MS)
#endif
		{
			tp_build_packet(tp_txpkt, seq, TP_TX_PAYLOAD_SIZE);
			ret = tp_tx_packet(tp_txpkt, TP_PKT_HEADER_SIZE + TP_TX_PAYLOAD_SIZE + TP_PKT_CRC_SIZE);
			if (ret == GSPI_OK) {
				tp_tx_ok++;
				seq++;
			} else if (ret == GSPI_NO_TXBD) {
				tp_tx_nobd++;   /* Device back-pressure, retry next round */
			} else {
				tp_tx_err++;
			}
#if (TP_TX_INTERVAL_MS > 0)
			last_tx_ms = now;
#endif
		}
#endif

		/* Device -> Host */
		tp_drain_rx();

		if ((now - last_stats_ms) >= STATS_INTERVAL_MS) {
			u32 secs = (now - start_ms) / 1000;

			RTK_LOGI(TAG, "[%ds] TX ok:%d nobd:%d err:%d | RX pkt ok:%d err:%d bytes:%d | %dKB/s\n",
					 secs, tp_tx_ok, tp_tx_nobd, tp_tx_err, tp_rx_ok, tp_rx_err, tp_rx_bytes,
					 secs ? (tp_rx_bytes / secs / 1024) : 0);
			last_stats_ms = now;
		}
	}

	RTK_LOGI(TAG, "TP test done @%dms: TX ok:%d nobd:%d err:%d | RX pkt ok:%d err:%d\n",
			 elapsed, tp_tx_ok, tp_tx_nobd, tp_tx_err, tp_rx_ok, tp_rx_err);
	if (tp_rx_err == 0 && tp_tx_err == 0 && tp_rx_ok > 0) {
		RTK_LOGI(TAG, "GSPI Demo: success\n");
	} else {
		RTK_LOGE(TAG, "GSPI Demo: fail\n");
	}

	rtos_task_delete(NULL);
}

int example_raw_gspi_host_fifo(void)
{
	if (rtos_task_create(NULL, ((const char *)"gspi_host_fifo"), (rtos_task_t)gspi_host_task,
						 NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "\n\r%s Create gspi_host_task task failed", __FUNCTION__);
	}

	return 0;
}
