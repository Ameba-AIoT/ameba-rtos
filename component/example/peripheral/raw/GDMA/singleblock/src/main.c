/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "main.h"
#include "os_wrapper.h"
#include <stdio.h>

#define DATA_SIZE     256
#define DMA_SRC_OFFSET  0
#define DMA_DST_OFFSET  0

ALIGNMTO(64) uint8_t TestBuf1[DATA_SIZE];
ALIGNMTO(64) uint8_t TestBuf2[DATA_SIZE];

volatile uint8_t dma_done;

struct gdma_s {
	u8 index;
	u8 ch_num;
	IRQ_FUN user_cb;
	u32 user_cb_data;
};
typedef struct gdma_s gdma_t;
gdma_t gdma;

void gdma_dump_memory(u8 *src, u32 len)
{
	u32 display_num = 16;
	for (uint32_t i = 0; i < len; i++) {
		if (!i) {
			printf("[%p] ", src);
		} else if (i % display_num == 0) {
			printf("\r\n[%p] ", src + i);
		}
		printf("%02x ", src[i]);
	}
	printf("\n");
}

void gdma_done_callback(void)
{
	printf("DMA Copy Done!!\r\n");
	dma_done = 1;
}

u32 gdma_memcpy_int(void *pData)
{
	u32 IsrTypeMap = 0;
	u8 *pSrcData = NULL, *pDstData = NULL;
	gdma_t *dma_obj = (gdma_t *)pData;

	pSrcData = TestBuf1;
	pDstData = TestBuf2;
	/* Clean Auto Reload Bit */
	GDMA_ChCleanAutoReload(dma_obj->index, dma_obj->ch_num, CLEAN_RELOAD_SRC_DST);

	/* Write the cached value back to memory */
	DCache_Invalidate(((u32)TestBuf2 & CACHE_LINE_ADDR_MSK), DATA_SIZE);

	for (int i = 0; i < DATA_SIZE; i++) {
		if (pSrcData[i] != pDstData[i]) {
			printf("DMA Copy Memory Compare Err, %d %x %x\r\n", i, TestBuf1[i + DMA_SRC_OFFSET], TestBuf2[i + DMA_DST_OFFSET]);
			printf("Src addr= 0x%08lx, and length = %d\r\n", (uint32_t)pSrcData, DATA_SIZE);
			gdma_dump_memory(pSrcData, DATA_SIZE);
			printf("Dst addr= 0x%08lx, and length = %d\r\n", (uint32_t)pDstData, DATA_SIZE);
			gdma_dump_memory(pDstData, DATA_SIZE);
		}
	}

	/* Clear Pending ISR */
	IsrTypeMap = GDMA_ClearINT(dma_obj->index, dma_obj->ch_num);

	if (IsrTypeMap & TransferType) {
		if (dma_obj->user_cb != NULL) {
			dma_obj->user_cb((void *)dma_obj->user_cb_data);
		}
		GDMA_ChnlFree(gdma.index, gdma.ch_num);
		printf("DMA Copy Memory Compare OK!! \r\n");
	}
	return 0;
}
_OPTIMIZE_NONE_
void gdma_singleblock_transfer_task(void)
{
	int i;
	GDMA_InitTypeDef GDMA_InitStruct;
	u8 ch_num;

	//1. Configure struct basic parameters
	printf("\n%s:\n", __FUNCTION__);
	gdma.index = 0;
	gdma.user_cb = (IRQ_FUN)gdma_done_callback;
	gdma.user_cb_data = (u32)(&gdma);
	//2. Clear flag and Register interrupt for channel x
	dma_done = 0;
	ch_num = GDMA_ChnlAlloc(gdma.index, (IRQ_FUN) gdma_memcpy_int, (u32)(&gdma), 3);
	if (0xFF == ch_num) {
		printf("%s: Cannot allocate a GDMA Channel\n", __FUNCTION__);
		return;
	}
	gdma.ch_num = ch_num;

	//3. set buffer
	printf("Src=0x%p ==> Dst=0x%p Len=%d\r\n", TestBuf1 + DMA_SRC_OFFSET, TestBuf2 + DMA_DST_OFFSET, DATA_SIZE);
	for (i = 0; i < DATA_SIZE; i++) {
		TestBuf1[i] = i;
	}
	DCache_CleanInvalidate(((u32)TestBuf1 & CACHE_LINE_ADDR_MSK), DATA_SIZE);
	_memset(TestBuf2, 0xff, DATA_SIZE);
	DCache_CleanInvalidate(((u32)TestBuf2 & CACHE_LINE_ADDR_MSK), DATA_SIZE);

	//4. Configure GDMA parameters
	_memset((void *)&GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));
	GDMA_InitStruct.GDMA_Index = gdma.index;
	GDMA_InitStruct.GDMA_ChNum = gdma.ch_num;
	GDMA_InitStruct.GDMA_DIR    = TTFCMemToMem;
	GDMA_InitStruct.GDMA_DstInc = IncType;
	GDMA_InitStruct.GDMA_SrcInc = IncType;
	GDMA_InitStruct.GDMA_IsrType = (TransferType | ErrType);

	if ((((u32)(TestBuf1 + DMA_SRC_OFFSET) & 0x03) == 0) &&	(((u32)(TestBuf2 + DMA_DST_OFFSET) & 0x03) == 0) && ((DATA_SIZE & 0x03) == 0)) {
		/* 4-bytes aligned, move 4 bytes each transfer */
		GDMA_InitStruct.GDMA_SrcMsize   = MsizeEight;
		GDMA_InitStruct.GDMA_SrcDataWidth = TrWidthFourBytes;
		GDMA_InitStruct.GDMA_DstMsize = MsizeEight;
		GDMA_InitStruct.GDMA_DstDataWidth = TrWidthFourBytes;
		GDMA_InitStruct.GDMA_BlockSize = DATA_SIZE >> TrWidthFourBytes;
	} else {
		GDMA_InitStruct.GDMA_SrcMsize   = MsizeEight;
		GDMA_InitStruct.GDMA_SrcDataWidth = TrWidthOneByte;
		GDMA_InitStruct.GDMA_DstMsize = MsizeEight;
		GDMA_InitStruct.GDMA_DstDataWidth = TrWidthOneByte;
		GDMA_InitStruct.GDMA_BlockSize = DATA_SIZE;
	}
	GDMA_InitStruct.GDMA_SrcAddr = (u32)(TestBuf1 + DMA_SRC_OFFSET);
	GDMA_InitStruct.GDMA_DstAddr = (u32)(TestBuf2 + DMA_DST_OFFSET);

	printf("GDMA_InitStruct->GDMA_BlockSize = %d\n", GDMA_InitStruct.GDMA_BlockSize);

	GDMA_Init(GDMA_InitStruct.GDMA_Index, GDMA_InitStruct.GDMA_ChNum, &GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct.GDMA_Index, GDMA_InitStruct.GDMA_ChNum, ENABLE);

	//5. Wait for the GDMA transfer to complete
	while (dma_done == 0);
	while (1);
}

int main(void)
{
	if (rtos_task_create(NULL, "GDMA SINGLE BLOCK TRANSFER DEMO", (rtos_task_t)gdma_singleblock_transfer_task, NULL, (3072), (1)) != SUCCESS) {
		printf("Cannot create dma_singleblock_task demo task\n\r");
	}

	rtos_sched_start();
	return 0;
}
