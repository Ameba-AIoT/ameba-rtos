/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "spdio_api.h"
#include "PinNames.h"
#include "os_wrapper.h"
#include "at_intf_sdio.h"
#include "atcmd_service.h"

static const char *const TAG = "AT_SDIO-D";

#define SDIO_TX_BD_NUM	4	//n*2, 2 bd for one transaction
#define SDIO_RX_BD_NUM	10
#define SDIO_RX_BUFSZ	(SPDIO_RX_BUFSZ_ALIGN(2048+24)) //n*64, must be rounded to 64, extra 24 bytes for spdio header info

struct spdio_t spdio_dev;

rtos_sema_t atcmd_sdio_rx_sema;
rtos_sema_t atcmd_sdio_tx_sema;
rtos_sema_t atcmd_sdio_tx_done_sema;
RingBuffer *at_sdio_rx_ring_buf = NULL;
RingBuffer *at_sdio_tx_ring_buf = NULL;

u8 sdio_txbuf[ATCMD_SDIO_MAX_SIZE];

u8 g_sdio_device_ready = 0;

extern volatile UART_LOG_CTL shell_ctl;
extern UART_LOG_BUF shell_rxbuf;

char ex_spdio_tx(u8 *pdata, u16 size, u8 type)
{
	UNUSED(type);
	int ret;
	struct spdio_buf_t *tx_buf = (struct spdio_buf_t *)rtos_mem_malloc(sizeof(struct spdio_buf_t));
	memset((u8 *)tx_buf, 0, sizeof(struct spdio_buf_t));

	if (!tx_buf) {
		return RTK_FAIL;
	}
	tx_buf->buf_allocated = (u32)rtos_mem_malloc(size + SPDIO_DMA_ALIGN_4);
	if (!tx_buf->buf_allocated) {
		rtos_mem_free((u8 *)tx_buf);
		return RTK_FAIL;
	}
	tx_buf->size_allocated = size + SPDIO_DMA_ALIGN_4;

	tx_buf->buf_addr = (u32)N_BYTE_ALIGMENT((u32)(tx_buf->buf_allocated), SPDIO_DMA_ALIGN_4);

	// copy data
	memcpy((void *)tx_buf->buf_addr, pdata, size);

	tx_buf->buf_size = size;
	tx_buf->type = SPDIO_RX_DATA_USER; // you can define your own data type in spdio_rx_data_t and spdio_tx_data_t

	ret = spdio_tx(&spdio_dev, tx_buf);

	if (ret == FALSE) {
		rtos_mem_free((u8 *)tx_buf->buf_allocated);
		rtos_mem_free((u8 *)tx_buf);
	}

	return RTK_SUCCESS;
}


/*spdio rx done callback (HOST->Device), manage your package and buffer*/
char ex_spdio_rx_done_cb(void *priv, void *pbuf, u8 *pdata, u16 size, u8 type)
{
	UNUSED(type);
	struct spdio_t *obj = (struct spdio_t *)priv;
	struct spdio_buf_t *rx_buf = (struct spdio_buf_t *)pbuf;
	u32 space;

	if (g_tt_mode) {
		space = RingBuffer_Space(atcmd_tt_mode_rx_ring_buf);

		if (g_tt_mode_check_watermark) {
			if (space - size < MAX_TT_HEAP_SIZE * (1 - TT_MODE_HIGH_WATERMARK) && g_tt_mode_indicate_high_watermark == 0) {
				g_tt_mode_indicate_high_watermark = 1;
				g_tt_mode_indicate_low_watermark = 0;
				at_printf(ATCMD_TT_MODE_HIGH_WATERMARK_STR);
			}
		}

		/*recv stop char under tt mode*/
		if (size == 1 && pdata[0] == '<') {
			g_tt_mode_stop_char_cnt++;
		} else {
			g_tt_mode_stop_char_cnt = 0;

			if (space >= size) {
				RingBuffer_Write(atcmd_tt_mode_rx_ring_buf, pdata, size);
				rtos_sema_give(atcmd_tt_mode_sema);
			} else if (space > 0) {
				RingBuffer_Write(atcmd_tt_mode_rx_ring_buf, pdata, space);
				rtos_sema_give(atcmd_tt_mode_sema);
				RTK_LOGW(TAG, "atcmd_tt_mode_rx_ring_buf is full, drop partial data\n");
			} else {
				RTK_LOGW(TAG, "atcmd_tt_mode_rx_ring_buf is full, drop data\n");
			}
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

	} else {
		space = RingBuffer_Space(at_sdio_rx_ring_buf);

		if (space >= size) {
			RingBuffer_Write(at_sdio_rx_ring_buf, pdata, size);
			rtos_sema_give(atcmd_sdio_rx_sema);
		} else {
			RTK_LOGW(TAG, "at_sdio_rx_ring_buf is full, drop data\n");
		}
	}

	// manage rx_buf here
	rtos_mem_free((char *)rx_buf->buf_allocated);

	// assign new buffer for SPDIO RX
	rx_buf->buf_allocated = (u32)rtos_mem_malloc(obj->rx_bd_bufsz + SPDIO_DMA_ALIGN_4);
	rx_buf->size_allocated = obj->rx_bd_bufsz + SPDIO_DMA_ALIGN_4;

	// this buffer must be 4 byte alignment
	rx_buf->buf_addr = (u32)N_BYTE_ALIGMENT((u32)(rx_buf->buf_allocated), SPDIO_DMA_ALIGN_4);

	return RTK_SUCCESS;

}

/*spdio tx done callback(Device->HOST), manage buffer*/
// this API will be called after package have been moved to HOST
char ex_spdio_tx_done_cb(void *priv, void *pbuf)
{
	UNUSED(priv);
	struct spdio_buf_t *tx_buf = (struct spdio_buf_t *)pbuf;

	rtos_sema_give(atcmd_sdio_tx_done_sema);

	rtos_mem_free((u8 *)tx_buf->buf_allocated);
	rtos_mem_free((u8 *)tx_buf);
	return RTK_SUCCESS;
}

void ex_spdio_thread(void *param)
{
	UNUSED(param);
	u32 i;

	spdio_dev.priv = NULL;
	spdio_dev.rx_bd_num = SDIO_RX_BD_NUM;
	spdio_dev.tx_bd_num = SDIO_TX_BD_NUM;
	spdio_dev.rx_bd_bufsz = SDIO_RX_BUFSZ;

	spdio_dev.rx_buf = (struct spdio_buf_t *)rtos_mem_malloc(spdio_dev.rx_bd_num * sizeof(struct spdio_buf_t));
	if (!spdio_dev.rx_buf) {
		RTK_LOGE(TAG, "malloc failed for spdio buffer structure!\n");
		return;
	}

	for (i = 0; i < spdio_dev.rx_bd_num; i++) {
		spdio_dev.rx_buf[i].buf_allocated = (u32)rtos_mem_malloc(spdio_dev.rx_bd_bufsz + SPDIO_DMA_ALIGN_4);
		if (!spdio_dev.rx_buf[i].buf_allocated) {
			RTK_LOGE(TAG, "malloc failed for spdio buffer!\n");
			return;
		}
		spdio_dev.rx_buf[i].size_allocated = spdio_dev.rx_bd_bufsz + SPDIO_DMA_ALIGN_4;
		// this buffer must be 4 byte alignment
		spdio_dev.rx_buf[i].buf_addr = (u32)N_BYTE_ALIGMENT((u32)(spdio_dev.rx_buf[i].buf_allocated), SPDIO_DMA_ALIGN_4);
	}

	spdio_dev.rx_done_cb = ex_spdio_rx_done_cb;
	spdio_dev.tx_done_cb = ex_spdio_tx_done_cb;

	spdio_init(&spdio_dev);
	RTK_LOGI(TAG, "SDIO device ready!\n");
	g_sdio_device_ready = 1;
	rtos_task_delete(NULL);
}


void atcmd_sdio_input_handler_task(void)
{
	PUART_LOG_BUF pShellRxBuf = &shell_rxbuf;
	u32 i = 0, actual_len = 0;
	while (1) {
		pShellRxBuf->BufCount = 0;
		i = 0;

		rtos_sema_take(atcmd_sdio_rx_sema, 0xFFFFFFFF);

		actual_len = RingBuffer_Available(at_sdio_rx_ring_buf);
		RingBuffer_Read(at_sdio_rx_ring_buf, pShellRxBuf->UARTLogBuf, actual_len);

		pShellRxBuf->BufCount = actual_len;

recv_again:
		if (shell_cmd_chk(pShellRxBuf->UARTLogBuf[i++], (UART_LOG_CTL *)&shell_ctl, ENABLE) == 2) {
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
	}
}

void atcmd_sdio_output_handler_task(void)
{
	u32 actual_len = 0, send_len;
	while (1) {
		rtos_sema_take(atcmd_sdio_tx_sema, 0xFFFFFFFF);

		actual_len = RingBuffer_Available(at_sdio_tx_ring_buf);

		if (actual_len > 0) {
			send_len = actual_len > ATCMD_SDIO_MAX_SIZE ? ATCMD_SDIO_MAX_SIZE : actual_len;

			memset(sdio_txbuf, 0, ATCMD_SDIO_MAX_SIZE);
			RingBuffer_Read(at_sdio_tx_ring_buf, sdio_txbuf, send_len);

			// take tx done sema
			rtos_sema_take(atcmd_sdio_tx_done_sema, 0xFFFFFFFF);

			ex_spdio_tx(sdio_txbuf, send_len, 0);

			if (actual_len > send_len) {
				rtos_sema_give(atcmd_sdio_tx_sema);
			}
		}
	}
}

void atio_sdio_output(char *buf, int len)
{
	int space = 0;
	int send_len = len;

	while (1) {
		space = RingBuffer_Space(at_sdio_tx_ring_buf);
		if (space >= send_len) {
			RingBuffer_Write(at_sdio_tx_ring_buf, (u8 *)buf, send_len);
			break;
		} else if (space > 0) {
			RingBuffer_Write(at_sdio_tx_ring_buf, (u8 *)buf, space);
			send_len -= space;
			buf += space;
		}

		rtos_time_delay_ms(1);
	}

	if (g_sdio_device_ready) {
		rtos_sema_give(atcmd_sdio_tx_sema);
	}

}

int atio_sdio_init(void)
{
	rtos_sema_create(&atcmd_sdio_rx_sema, 0, 0xFFFF);
	rtos_sema_create(&atcmd_sdio_tx_sema, 0, 0xFFFF);
	rtos_sema_create(&atcmd_sdio_tx_done_sema, 1, 0xFFFF);
	at_sdio_rx_ring_buf = RingBuffer_Create(NULL, 4 * 1024, LOCAL_RINGBUFF, 1);
	at_sdio_tx_ring_buf = RingBuffer_Create(NULL, 32 * 1024, LOCAL_RINGBUFF, 1);

	if (atcmd_sdio_rx_sema == NULL || atcmd_sdio_tx_sema == NULL || atcmd_sdio_tx_done_sema == NULL ||
		at_sdio_rx_ring_buf == NULL || at_sdio_tx_ring_buf == NULL) {
		return -1;
	}

	out_buffer = atio_sdio_output;

	if (rtos_task_create(NULL, ((const char *)"ex_spdio_thread"), ex_spdio_thread, NULL, 1024, 5) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "rtos_task_create(ex_spdio_thread) failed\n");
	}

	if (rtos_task_create(NULL, ((const char *)"atcmd_sdio_input_handler_task"), (rtos_task_t)atcmd_sdio_input_handler_task, NULL, 1024, 5) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "\n\r%s rtos_task_create(atcmd_sdio_input_handler_task) failed", __FUNCTION__);
		return -1;
	}

	if (rtos_task_create(NULL, ((const char *)"atcmd_sdio_output_handler_task"), (rtos_task_t)atcmd_sdio_output_handler_task, NULL, 1024, 5) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "\n\r%s rtos_task_create(atcmd_sdio_output_handler_task) failed", __FUNCTION__);
		return -1;
	}

	return 0;
}

