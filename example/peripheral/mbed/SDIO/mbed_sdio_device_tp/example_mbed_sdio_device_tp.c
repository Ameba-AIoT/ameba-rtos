/*
 *  SDIO Device Bidirectional Throughput Test Example
 *
 *  This example runs on the SDIO Device (slave) board.
 *  - Phase 0: Receives "Hello Device" from host, responds "Hello Host! I am Here"
 *  - Phase 1: Continuously sends TP data packets to host (1ms interval),
 *             while receiving and verifying packets from host.
 *             Prints TX/RX stats every 1000 received packets.
 *
 *  Note: Pinmux is configured by spdio_init() -> SPDIO_Board_Init() using
 *  SDIO_Pin_Grp from ameba_intfcfg.c. No manual pin config needed.
 */

#include "spdio_api.h"
#include "os_wrapper.h"
#include "ameba_soc.h"

static const char *const TAG = "SDIO_DEV";

#define EX_SPDIO_STACKSIZE		2048
#define EX_SPDIO_TX_BD_NUM		8	/* n*2, 2 BD for one transaction */
#define EX_SPDIO_RX_BD_NUM		4
#define EX_SPDIO_RX_BUFSZ		(SPDIO_RX_BUFSZ_ALIGN(2048 + 24))

/* Protocol definitions */
#define PKT_MAGIC				0x5AA5
#define PKT_HEADER_SIZE			6	/* magic(2) + seq(2) + data_len(2) */
#define PKT_CRC_SIZE			2
#define HELLO_DEVICE_MSG		"Hello Device"
#define HELLO_HOST_RESP			"Hello Host! I am Here"

#define HELLO_DEVICE_LEN		(sizeof(HELLO_DEVICE_MSG) - 1)
#define HELLO_HOST_RESP_LEN		(sizeof(HELLO_HOST_RESP) - 1)

/* TP test configuration */
#define TP_PAYLOAD_SIZE			1000
#define TP_PACKET_SIZE			(PKT_HEADER_SIZE + TP_PAYLOAD_SIZE + PKT_CRC_SIZE)
#define TP_SEND_INTERVAL_MS		1
#define TP_STATS_INTERVAL		1000	/* print stats every N received packets */

/* TP test packet header */
typedef struct {
	u16 magic;
	u16 seq;
	u16 data_len;
} __attribute__((packed)) tp_pkt_header_t;

/* Statistics */
static volatile u32 tp_rx_total = 0;
static volatile u32 tp_rx_correct = 0;
static volatile u32 tp_rx_error = 0;
static volatile u32 tp_tx_count = 0;
static volatile u32 tp_phase = 0; /* 0: waiting for hello, 1: TP test running */

static struct spdio_t spdio_dev;
static rtos_sema_t tx_sema;
static rtos_sema_t start_tx_sema;	/* signaled when phase 0 handshake completes */

/* CRC16-CCITT calculation */
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

/* Send data from device to host.
 * NOTE: called from RX callback context — SPDIO invokes RX callbacks from a
 * dedicated task (not ISR), so the blocking sema take here is permitted. */
static char device_send_to_host(u8 *pdata, u16 size)
{
	struct spdio_buf_t *tx_buf;
	int ret;

	if (rtos_sema_take(tx_sema, 5000) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "tx_sema timeout — TX BD exhausted?\n");
		return RTK_FAIL;
	}

	tx_buf = (struct spdio_buf_t *)rtos_mem_malloc(sizeof(struct spdio_buf_t));
	if (!tx_buf) {
		RTK_LOGE(TAG, "malloc tx_buf fail\n");
		rtos_sema_give(tx_sema);
		return RTK_FAIL;
	}
	memset(tx_buf, 0, sizeof(struct spdio_buf_t));

	tx_buf->buf_allocated = (u32)rtos_mem_malloc(size + SPDIO_DMA_ALIGN_4);
	if (!tx_buf->buf_allocated) {
		RTK_LOGE(TAG, "malloc TX DMA buf fail (size=%u)\n", size);
		rtos_mem_free(tx_buf);
		rtos_sema_give(tx_sema);
		return RTK_FAIL;
	}
	tx_buf->size_allocated = size + SPDIO_DMA_ALIGN_4;
	tx_buf->buf_addr = (u32)N_BYTE_ALIGMENT((u32)tx_buf->buf_allocated, SPDIO_DMA_ALIGN_4);

	memcpy((void *)tx_buf->buf_addr, pdata, size);
	tx_buf->buf_size = size;
	tx_buf->type = SPDIO_RX_DATA_USER;

	ret = spdio_tx(&spdio_dev, tx_buf);
	if (ret == FALSE) {
		RTK_LOGE(TAG, "spdio_tx fail\n");
		rtos_mem_free((void *)tx_buf->buf_allocated);
		rtos_mem_free((void *)tx_buf);
		rtos_sema_give(tx_sema);
		return RTK_FAIL;
	}

	return RTK_SUCCESS;
}

/* Build a TP packet into buffer (device side: no INIC_TX_DESC) */
static void device_build_tp_packet(u8 *buf, u16 seq, u16 payload_len)
{
	tp_pkt_header_t *pkt_hdr;
	u16 crc;
	u32 i;

	pkt_hdr = (tp_pkt_header_t *)buf;
	pkt_hdr->magic = PKT_MAGIC;
	pkt_hdr->seq = seq;
	pkt_hdr->data_len = payload_len;

	for (i = 0; i < payload_len; i++) {
		buf[PKT_HEADER_SIZE + i] = (u8)((i + seq) & 0xFF);
	}

	crc = crc16_ccitt(buf, PKT_HEADER_SIZE + payload_len);
	memcpy(buf + PKT_HEADER_SIZE + payload_len, &crc, 2);
}

/* Device TX thread: continuously sends TP packets after handshake */
static void device_tx_thread(void *param)
{
	UNUSED(param);
	u8 *pkt_buf;
	u16 seq = 0;

	/* Wait for Phase 0 handshake completion */
	rtos_sema_take(start_tx_sema, RTOS_MAX_DELAY);

	RTK_LOGI(TAG, "========================================\n");
	RTK_LOGI(TAG, "  Phase 1: TP test started\n");
	RTK_LOGI(TAG, "  payload=%d B  interval=%d ms  stats every %d pkts\n",
			 TP_PAYLOAD_SIZE, TP_SEND_INTERVAL_MS, TP_STATS_INTERVAL);
	RTK_LOGI(TAG, "========================================\n");

	pkt_buf = (u8 *)rtos_mem_malloc(TP_PACKET_SIZE);
	if (!pkt_buf) {
		RTK_LOGE(TAG, "malloc TX packet buf fail — TX thread abort\n");
		rtos_task_delete(NULL);
		return;
	}

	while (tp_phase == 1) {
		memset(pkt_buf, 0, TP_PACKET_SIZE);
		device_build_tp_packet(pkt_buf, seq, TP_PAYLOAD_SIZE);

		if (device_send_to_host(pkt_buf, TP_PACKET_SIZE) == RTK_SUCCESS) {
			tp_tx_count++;
		}

		seq++;
		rtos_time_delay_ms(TP_SEND_INTERVAL_MS);
	}

	rtos_mem_free(pkt_buf);
	RTK_LOGI(TAG, "TX thread stopped (total sent: %lu)\n", tp_tx_count);
	rtos_task_delete(NULL);
}

/* SPDIO RX done callback (Host -> Device) */
static char ex_spdio_rx_done_cb(void *priv, void *pbuf, u8 *pdata, u16 size, u8 type)
{
	struct spdio_t *obj = (struct spdio_t *)priv;
	struct spdio_buf_t *rx_buf = (struct spdio_buf_t *)pbuf;
	tp_pkt_header_t *hdr;
	u16 recv_crc, calc_crc;
	u32 total_size;
	u32 err_pct;
	u32 new_buf;

	UNUSED(type);

	/* Phase 0: Check for "Hello Device" greeting */
	if (tp_phase == 0) {
		RTK_LOGI(TAG, "[Phase 0] RX %d bytes, type=0x%02x\n", size, type);
		if (size >= HELLO_DEVICE_LEN && memcmp(pdata, HELLO_DEVICE_MSG, HELLO_DEVICE_LEN) == 0) {
			RTK_LOGI(TAG, "[Phase 0] Received: \"%s\"\n", HELLO_DEVICE_MSG);
			RTK_LOGI(TAG, "[Phase 0] Sending:  \"%s\"\n", HELLO_HOST_RESP);
			device_send_to_host((u8 *)HELLO_HOST_RESP, HELLO_HOST_RESP_LEN);
			tp_phase = 1;
			rtos_sema_give(start_tx_sema);
		} else {
			RTK_LOGE(TAG, "[Phase 0] Unexpected message (size=%d) — ignored\n", size);
		}
		goto reassign_buf;
	}

	/* Phase 1: TP data packet verification */
	/* Silently discard repeated "Hello Device" from host during TP test */
	if (size >= HELLO_DEVICE_LEN && memcmp(pdata, HELLO_DEVICE_MSG, HELLO_DEVICE_LEN) == 0) {
		goto reassign_buf;
	}

	tp_rx_total++;

	if (size < PKT_HEADER_SIZE + PKT_CRC_SIZE) {
		tp_rx_error++;
		RTK_LOGE(TAG, "[Phase 1] Pkt too small: %d B (min=%d)\n",
				 size, PKT_HEADER_SIZE + PKT_CRC_SIZE);
		goto stats_check;
	}

	hdr = (tp_pkt_header_t *)pdata;

	if (hdr->magic != PKT_MAGIC) {
		tp_rx_error++;
		RTK_LOGE(TAG, "[Phase 1] Bad magic: 0x%04x (expect 0x%04x) seq=%u\n",
				 hdr->magic, PKT_MAGIC, hdr->seq);
		goto stats_check;
	}

	total_size = PKT_HEADER_SIZE + hdr->data_len + PKT_CRC_SIZE;
	if (total_size != size) {
		tp_rx_error++;
		RTK_LOGE(TAG, "[Phase 1] Size mismatch: hdr=%lu actual=%u seq=%u\n",
				 total_size, size, hdr->seq);
		goto stats_check;
	}

	memcpy(&recv_crc, pdata + PKT_HEADER_SIZE + hdr->data_len, 2);
	calc_crc = crc16_ccitt(pdata, PKT_HEADER_SIZE + hdr->data_len);
	if (recv_crc != calc_crc) {
		tp_rx_error++;
		RTK_LOGE(TAG, "[Phase 1] CRC error: recv=0x%04x calc=0x%04x seq=%u\n",
				 recv_crc, calc_crc, hdr->seq);
		goto stats_check;
	}

	tp_rx_correct++;

stats_check:
	if (tp_rx_total != 0 && tp_rx_total % TP_STATS_INTERVAL == 0) {
		err_pct = tp_rx_error * 100 / tp_rx_total;
		RTK_LOGI(TAG, "[Stats @ RX=%lu] TX sent:%lu | RX total:%lu ok:%lu err:%lu (%lu%%)\n",
				 tp_rx_total, tp_tx_count, tp_rx_total, tp_rx_correct, tp_rx_error, err_pct);
	}

reassign_buf:
	/* Re-assign RX buffer for next DMA transfer */
	new_buf = (u32)rtos_mem_malloc(obj->device_rx_bufsz + SPDIO_DMA_ALIGN_4);
	if (new_buf) {
		rtos_mem_free((void *)rx_buf->buf_allocated);
		rx_buf->buf_allocated = new_buf;
		rx_buf->size_allocated = obj->device_rx_bufsz + SPDIO_DMA_ALIGN_4;
		rx_buf->buf_addr = (u32)N_BYTE_ALIGMENT((u32)rx_buf->buf_allocated, SPDIO_DMA_ALIGN_4);
	} else {
		RTK_LOGE(TAG, "RX buf realloc fail — reusing old buffer\n");
	}

	return RTK_SUCCESS;
}

/* SPDIO TX done callback (Device -> Host) */
static char ex_spdio_tx_done_cb(void *priv, void *pbuf)
{
	UNUSED(priv);
	struct spdio_buf_t *tx_buf = (struct spdio_buf_t *)pbuf;

	rtos_mem_free((void *)tx_buf->buf_allocated);
	rtos_mem_free((void *)tx_buf);

	rtos_sema_give(tx_sema);
	return RTK_SUCCESS;
}

/* SDIO device initialization thread */
static void ex_spdio_thread(void *param)
{
	UNUSED(param);
	u32 i;

	rtos_sema_create(&tx_sema, EX_SPDIO_TX_BD_NUM / 2 - 1, EX_SPDIO_TX_BD_NUM / 2 - 1);
	rtos_sema_create(&start_tx_sema, 0, 1);

	spdio_dev.priv = NULL;
	spdio_dev.host_tx_bd_num = EX_SPDIO_RX_BD_NUM;
	spdio_dev.host_rx_bd_num = EX_SPDIO_TX_BD_NUM;
	spdio_dev.device_rx_bufsz = EX_SPDIO_RX_BUFSZ;
	spdio_dev.pSDIO = SDIO_WIFI;

	spdio_dev.rx_buf = (struct spdio_buf_t *)rtos_mem_malloc(spdio_dev.host_tx_bd_num * sizeof(struct spdio_buf_t));
	if (!spdio_dev.rx_buf) {
		RTK_LOGE(TAG, "malloc rx_buf array fail\n");
		return;
	}
	memset(spdio_dev.rx_buf, 0, spdio_dev.host_tx_bd_num * sizeof(struct spdio_buf_t));

	for (i = 0; i < spdio_dev.host_tx_bd_num; i++) {
		spdio_dev.rx_buf[i].buf_allocated = (u32)rtos_mem_malloc(spdio_dev.device_rx_bufsz + SPDIO_DMA_ALIGN_4);
		if (!spdio_dev.rx_buf[i].buf_allocated) {
			RTK_LOGE(TAG, "malloc rx_buf[%lu] fail — freeing partial alloc\n", i);
			/* Free previously allocated sub-buffers to avoid leak */
			while (i-- > 0) {
				rtos_mem_free((void *)spdio_dev.rx_buf[i].buf_allocated);
			}
			rtos_mem_free(spdio_dev.rx_buf);
			return;
		}
		spdio_dev.rx_buf[i].size_allocated = spdio_dev.device_rx_bufsz + SPDIO_DMA_ALIGN_4;
		spdio_dev.rx_buf[i].buf_addr = (u32)N_BYTE_ALIGMENT((u32)spdio_dev.rx_buf[i].buf_allocated, SPDIO_DMA_ALIGN_4);
	}

	spdio_dev.device_rx_done_cb = ex_spdio_rx_done_cb;
	spdio_dev.device_tx_done_cb = ex_spdio_tx_done_cb;

	spdio_init(&spdio_dev);

	RTK_LOGI(TAG, "========================================\n");
	RTK_LOGI(TAG, "  SDIO Device initialized\n");
	RTK_LOGI(TAG, "  TX_BD=%d  RX_BD=%d  RX_bufsz=%d\n",
			 EX_SPDIO_TX_BD_NUM, EX_SPDIO_RX_BD_NUM, EX_SPDIO_RX_BUFSZ);
	RTK_LOGI(TAG, "  Phase 0: Waiting for host greeting...\n");
	RTK_LOGI(TAG, "========================================\n");

	/* Start TX thread — blocks on start_tx_sema until Phase 0 handshake completes */
	if (rtos_task_create(NULL, ((const char *)"sdio_dev_tx"), device_tx_thread, NULL, EX_SPDIO_STACKSIZE * 4, 5) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create TX task fail\n");
	}

	rtos_task_delete(NULL);
}

int example_mbed_sdio_device_tp(void)
{
	/* SDIO device peripheral clock and pinmux are handled by spdio_init() */
	if (rtos_task_create(NULL, ((const char *)"sdio_dev_tp"), ex_spdio_thread, NULL, EX_SPDIO_STACKSIZE * 4, 5) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create init task fail\n");
	}

	return 0;
}
