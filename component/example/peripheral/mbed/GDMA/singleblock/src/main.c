/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "device.h"
#include "diag.h"
#include "main.h"
#include "os_wrapper.h"
#include <stdio.h>
#include "dma_api.h"

//Single-Block Example Demo

#define DATA_SIZE     256
#define DMA_SRC_OFFSET  0
#define DMA_DST_OFFSET  0

gdma_t gdma;
ALIGNMTO(64) uint8_t TestBuf1[DATA_SIZE];
ALIGNMTO(64) uint8_t TestBuf2[DATA_SIZE];
volatile uint8_t dma_done;

u32 dma_done_handler(void *id)
{
	UNUSED(id);
	printf("DMA Copy Done!!\r\n");
	dma_done = 1;
	return 0;
}

_OPTIMIZE_NONE_
void dma_singleblock_transfer_task(void)
{
	int i;
	int err;

	dma_memcpy_init(&gdma, dma_done_handler, (uint32_t)&gdma);
	for (i = 0; i < DATA_SIZE; i++) {
		TestBuf1[i] = i;
	}
	DCache_CleanInvalidate(((u32)TestBuf1 & CACHE_LINE_ADDR_MSK), DATA_SIZE);

	_memset(TestBuf2, 0xff, DATA_SIZE);
	DCache_CleanInvalidate(((u32)TestBuf2 & CACHE_LINE_ADDR_MSK), DATA_SIZE);

	dma_done = 0;

	dma_memcpy(&gdma, TestBuf2 + DMA_DST_OFFSET, TestBuf1 + DMA_SRC_OFFSET, DATA_SIZE);

	while (dma_done == 0);
	DCache_Invalidate(((u32)TestBuf2 & CACHE_LINE_ADDR_MSK), DATA_SIZE);
	err = 0;
	for (i = 0; i < DATA_SIZE; i++) {
		if (TestBuf2[i + DMA_DST_OFFSET] != TestBuf1[i + DMA_SRC_OFFSET]) {
			printf("DMA Copy Memory Compare Err, %d %x %x\r\n", i, TestBuf1[i + DMA_SRC_OFFSET], TestBuf2[i + DMA_DST_OFFSET]);
			err = 1;
		}
	}
	if (!err) {
		printf("DMA Copy Memory Compare OK!! \r\n");
	}
	dma_memcpy_deinit(&(gdma));

	while (1);

}

int main(void)
{
	if (rtos_task_create(NULL, "GDMA SINGLE BLOCK TRANSFER DEMO", (rtos_task_t)dma_singleblock_transfer_task, NULL, 3072, (1)) != SUCCESS) {
		printf("Cannot create dma_singleblock_task demo task\n\r");
	}

	rtos_sched_start();
	return 0;
}
