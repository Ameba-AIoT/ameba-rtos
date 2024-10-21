/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "diag.h"
#include "main.h"
#include "spdio_api.h"
#include "PinNames.h"
#include "basic_types.h"
#include "os_wrapper.h"

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
#define EX_SPDIO_STACKSIZE	2048
#define EX_SPDIO_TX_BD_NUM	24	//n*2, 2 bd for one transaction
#define EX_SPDIO_RX_BD_NUM	20
#define EX_SPDIO_RX_BUFSZ	(SPDIO_RX_BUFSZ_ALIGN(2048+24)) //n*64, must be rounded to 64, extra 24 bytes for spdio header info

struct spdio_t spdio_dev;

/*
*	param: pdata, package
*/
char ex_spdio_tx(u8 *pdata, u16 size, u8 type)
{
	UNUSED(type);
	static int loop_cnt = 0;
	int ret;
	struct spdio_buf_t *tx_buf = (struct spdio_buf_t *)rtos_mem_malloc(sizeof(struct spdio_buf_t));
	memset((u8 *)tx_buf, 0, sizeof(struct spdio_buf_t));

	if (!tx_buf) {
		return FAIL;
	}
	tx_buf->buf_allocated = (u32)rtos_mem_malloc(size + SPDIO_DMA_ALIGN_4);
	if (!tx_buf->buf_allocated) {
		rtos_mem_free((u8 *)tx_buf);
		return FAIL;
	}
	tx_buf->size_allocated = size + SPDIO_DMA_ALIGN_4;

	tx_buf->buf_addr = (u32)N_BYTE_ALIGMENT((u32)(tx_buf->buf_allocated), SPDIO_DMA_ALIGN_4);

	// copy data
	memcpy((void *)tx_buf->buf_addr, pdata, size);

	tx_buf->buf_size = size;
	tx_buf->type = SPDIO_RX_DATA_USER; // you can define your own data type in spdio_rx_data_t and spdio_tx_data_t
	printf("loopback package, size = %d (cnt = %d) heap=%lu\n", size, ++loop_cnt, rtos_mem_get_free_heap_size());
	// loopback
	ret = spdio_tx(&spdio_dev, tx_buf);
	if (ret == _FALSE) {
		rtos_mem_free((u8 *)tx_buf->buf_allocated);
		rtos_mem_free((u8 *)tx_buf);
	}

	return SUCCESS;
}


/*spdio rx done callback (HOST->Device), manage your package and buffer*/
char ex_spdio_rx_done_cb(void *priv, void *pbuf, u8 *pdata, u16 size, u8 type)
{
	struct spdio_t *obj = (struct spdio_t *)priv;
	struct spdio_buf_t *rx_buf = (struct spdio_buf_t *)pbuf;

	/* check data */
	for (int i = 0; i < size; i++) {
		if (pdata[i] != i % 256) {
			printf("Loopback data check fail:%d, %d, %d\n", i, pdata[i], i % 256);
		}
	}
	// handle package received
	ex_spdio_tx(pdata, size, type);

	// manage rx_buf here
	rtos_mem_free((char *)rx_buf->buf_allocated);

	// assign new buffer for SPDIO RX
	rx_buf->buf_allocated = (u32)rtos_mem_malloc(obj->rx_bd_bufsz + SPDIO_DMA_ALIGN_4);
	rx_buf->size_allocated = obj->rx_bd_bufsz + SPDIO_DMA_ALIGN_4;

	// this buffer must be 4 byte alignment
	rx_buf->buf_addr = (u32)N_BYTE_ALIGMENT((u32)(rx_buf->buf_allocated), SPDIO_DMA_ALIGN_4);

	return SUCCESS;
}

/*spdio tx done callback(Device->HOST), manage buffer*/
// this API will be called after package have been moved to HOST
char ex_spdio_tx_done_cb(void *priv, void *pbuf)
{
	UNUSED(priv);
	struct spdio_buf_t *tx_buf = (struct spdio_buf_t *)pbuf;

	rtos_mem_free((u8 *)tx_buf->buf_allocated);
	rtos_mem_free((u8 *)tx_buf);
	return SUCCESS;
}


void ex_spdio_thread(void *param)
{
	UNUSED(param);
	u32 i;

	spdio_dev.priv = NULL;
	spdio_dev.rx_bd_num = EX_SPDIO_RX_BD_NUM;
	spdio_dev.tx_bd_num = EX_SPDIO_TX_BD_NUM;
	spdio_dev.rx_bd_bufsz = EX_SPDIO_RX_BUFSZ;

	spdio_dev.rx_buf = (struct spdio_buf_t *)rtos_mem_malloc(spdio_dev.rx_bd_num * sizeof(struct spdio_buf_t));
	if (!spdio_dev.rx_buf) {
		printf("malloc failed for spdio buffer structure!\n");
		return;
	}

	for (i = 0; i < spdio_dev.rx_bd_num; i++) {
		spdio_dev.rx_buf[i].buf_allocated = (u32)rtos_mem_malloc(spdio_dev.rx_bd_bufsz + SPDIO_DMA_ALIGN_4);
		if (!spdio_dev.rx_buf[i].buf_allocated) {
			printf("malloc failed for spdio buffer!\n");
			return;
		}
		spdio_dev.rx_buf[i].size_allocated = spdio_dev.rx_bd_bufsz + SPDIO_DMA_ALIGN_4;
		// this buffer must be 4 byte alignment
		spdio_dev.rx_buf[i].buf_addr = (u32)N_BYTE_ALIGMENT((u32)(spdio_dev.rx_buf[i].buf_allocated), SPDIO_DMA_ALIGN_4);
	}

	spdio_dev.rx_done_cb = ex_spdio_rx_done_cb;
	spdio_dev.tx_done_cb = ex_spdio_tx_done_cb;

	spdio_init(&spdio_dev);
	printf("SDIO device starts loopback!\n");
	rtos_task_delete(NULL);
}

int main(void)
{
	if (rtos_task_create(NULL, ((const char *)"ex_spdio_thread"), ex_spdio_thread, NULL, EX_SPDIO_STACKSIZE * 4, 5) != SUCCESS) {
		printf("rtos_task_create(ex_spdio_thread) failed\r\n");
	}

	rtos_sched_start();
}

