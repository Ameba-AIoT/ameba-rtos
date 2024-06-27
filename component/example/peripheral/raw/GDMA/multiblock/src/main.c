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

#define DATA_SIZE 		128
#define BLOCK_SIZE 		32
#define DMA_SRC_OFFSET  0
#define DMA_DST_OFFSET  0

enum {
	SingleBlock   = 1,
	ReloadDst     = 2,
	ReloadSrc     = 3,
	ReloadSrcDst  = 4,
	MAXGdmaTestMode
};

static u8 	GdmaType = ReloadDst;
static u32 	Gdma_BlockLen = 0;

struct gdma_s {
	u8 index;
	u8 ch_num;
	IRQ_FUN user_cb;
	u32 user_cb_data;
};
typedef struct gdma_s gdma_t;
gdma_t gdma;

ALIGNMTO(64) uint8_t TestBuf1[DATA_SIZE];
ALIGNMTO(64) uint8_t TestBuf2[DATA_SIZE];

volatile uint8_t dma_done;


bool gdma_test_compare(u8 *pSrc, u8 *pDst, u32 PacketLen)
{
	u32 CheckIndex = 0;

	for (CheckIndex = 0; CheckIndex < PacketLen; CheckIndex++) {
		if (pSrc[CheckIndex] != pDst[CheckIndex]) {
			return FALSE;
		}
	}
	return TRUE;
}
void gdma_dump_memory(u8 *src, u32 len)
{
	u32 display_num = 16;
	for (uint32_t i = 0; i < len; i++) {
		if (!i) {
			printf("[%p] ", src);
		} else if (i % display_num == 0) {
			printf("\r\n[%p] ", (src + i));
		}
		printf("%02x ", src[i]);
	}
	printf("\n");
}
u32 gdma_multiblock_irq(void *Data)
{
	PGDMA_InitTypeDef GDMA_InitStruct = (PGDMA_InitTypeDef) Data;
	u8 *pSrcData = NULL, *pDstData = NULL;
	u32 IsrTypeMap = 0;
	static u8 one_left = 0;

	if (GDMA_InitStruct->MaxMuliBlock == GDMA_InitStruct->MuliBlockCunt + 2) {
		one_left = 1;
	}

	// Clear Auto Reload Bit
	if (one_left) {
		GDMA_ChCleanAutoReload(0, GDMA_InitStruct->GDMA_ChNum, CLEAN_RELOAD_SRC_DST);
	}

	switch (GdmaType) {
	case  ReloadDst:
		pSrcData = (u8 *)(GDMA_InitStruct->GDMA_SrcAddr +
						  Gdma_BlockLen * (GDMA_InitStruct->MuliBlockCunt));
		pDstData = (u8 *)GDMA_InitStruct->GDMA_DstAddr ;
		break;
	case ReloadSrc:
		pSrcData = (u8 *)GDMA_InitStruct->GDMA_SrcAddr;
		pDstData = (u8 *)(GDMA_InitStruct->GDMA_DstAddr +
						  Gdma_BlockLen * (GDMA_InitStruct->MuliBlockCunt));
		break;
	case ReloadSrcDst:
		pSrcData = (u8 *)GDMA_InitStruct->GDMA_SrcAddr;
		pDstData = (u8 *)GDMA_InitStruct->GDMA_DstAddr;
		break;
	}

	/* reload cache from sram before use the DMA destination data */
	DCache_Invalidate(((u32)pDstData & CACHE_LINE_ADDR_MSK), (Gdma_BlockLen + CACHE_LINE_SIZE));

	//Compare Data should before HalGdmaChEn
	if (!gdma_test_compare(pSrcData, pDstData, Gdma_BlockLen)) {
		printf("DMA Block %lu Error: Check Data Error!!!\n", GDMA_InitStruct->MuliBlockCunt);
		printf("Src addr= 0x%08lx, and length = %d\r\n", (uint32_t)pSrcData, BLOCK_SIZE);
		gdma_dump_memory(pSrcData, BLOCK_SIZE);
		printf("Dst addr= 0x%08lx, and length = %d\r\n", (uint32_t)pDstData, BLOCK_SIZE);
		gdma_dump_memory(pDstData, BLOCK_SIZE);
	}

	//reset dest for next block compare
	_memset(pDstData, 0x00, Gdma_BlockLen);
	DCache_Clean((u32)pDstData, (Gdma_BlockLen + CACHE_LINE_SIZE));

	//Clear Pending ISR, next block will start transfer
	IsrTypeMap = GDMA_ClearINT(0, GDMA_InitStruct->GDMA_ChNum);

	//printf("DMA INT %08x\n", IsrTypeMap);
	if (IsrTypeMap & BlockType || one_left) {
		printf("DMA Block %lu Transfer Complete\r\n\r\n", GDMA_InitStruct->MuliBlockCunt++);
	}

	//last IRQ is TransferType in multi-block
	if (IsrTypeMap & TransferType) {
		printf("DMA Last Block Complete\n");
		printf("DMA Data MaxMuliBlock = %lu \n", GDMA_InitStruct->MaxMuliBlock);
		GDMA_ChnlFree(0, GDMA_InitStruct->GDMA_ChNum);
		dma_done = 1;
	}
	return 0;
}

void gdma_multiblock_transfer_task(void)
{
	u8 ch = 0;
	GDMA_InitTypeDef GDMA_InitStructTest;
	PGDMA_InitTypeDef   GDMA_InitStruct;
	GDMA_InitStruct = (PGDMA_InitTypeDef) &GDMA_InitStructTest;
	printf("\n%s: \n", __FUNCTION__);

	//1. Set source & destination buffer
	for (int i = 0; i < DATA_SIZE; i++) {
		TestBuf1[i] = i;
	}
	DCache_CleanInvalidate(((u32)TestBuf1 & CACHE_LINE_ADDR_MSK), DATA_SIZE);
	printf("Src=0x%p ==> Dst=0x%p, Len=%d\r\n", TestBuf1 + DMA_SRC_OFFSET, TestBuf2 + DMA_DST_OFFSET, DATA_SIZE);
	_memset(TestBuf2, 0xff, DATA_SIZE);
	DCache_CleanInvalidate(((u32)TestBuf2 & CACHE_LINE_ADDR_MSK), DATA_SIZE);

	//2. Configure GDMA parameters and register interrupt
	assert_param(GdmaType > SingleBlock && GdmaType < MAXGdmaTestMode);
	gdma.index = 0;
	gdma.user_cb = (IRQ_FUN)gdma_multiblock_irq;
	gdma.user_cb_data = (u32)GDMA_InitStruct;
	ch = GDMA_ChnlAlloc(gdma.index, gdma.user_cb, gdma.user_cb_data, INT_PRI_MIDDLE);
	if (ch == 0xFF) {
		return;
	}
	gdma.ch_num = ch;
	GDMA_StructInit(GDMA_InitStruct);

	GDMA_InitStruct->GDMA_Index = gdma.index;
	GDMA_InitStruct->GDMA_ChNum = gdma.ch_num;
	GDMA_InitStruct->GDMA_DIR    = TTFCMemToMem;
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_SrcInc = IncType;

	GDMA_InitStruct->GDMA_SrcAddr = (u32)(&TestBuf1[0]);
	GDMA_InitStruct->GDMA_DstAddr = (u32)(&TestBuf2[0]);
	GDMA_InitStruct->GDMA_SrcMsize = MsizeOne;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_DstMsize = MsizeOne;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;

	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->MaxMuliBlock = DATA_SIZE / BLOCK_SIZE;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);
	Gdma_BlockLen = BLOCK_SIZE;
	if (GdmaType == ReloadDst) {
		printf("ReloadDst Test\n");
		GDMA_InitStruct->GDMA_ReloadSrc = 0;
		GDMA_InitStruct->GDMA_ReloadDst = 1;
	} else if (GdmaType == ReloadSrc) {
		printf("ReloadSrc Test\n");
		GDMA_InitStruct->GDMA_ReloadSrc = 1;
		GDMA_InitStruct->GDMA_ReloadDst = 0;
	} else if (GdmaType == ReloadSrcDst) {
		printf("ReloadSrcDst Test\n");
		GDMA_InitStruct->GDMA_ReloadSrc = 1;
		GDMA_InitStruct->GDMA_ReloadDst = 1;
	}

	if (((BLOCK_SIZE & 0x03) != 0) || (((u32)(&TestBuf1[0]) & 0x03) != 0)) {
		/* move 1 byte each transfer */
		GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
	}
	GDMA_InitStruct->GDMA_BlockSize = BLOCK_SIZE  >> GDMA_InitStruct->GDMA_SrcDataWidth; //based on src width

	//3. Clear interrupt handler flag
	dma_done = 0;

	//4. Intialize GDMA and enable GDMA
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	//5. Wait for the GDMA transfer to complete
	while (dma_done == 0);
	while (1);
}

int main(void)
{
	if (rtos_task_create(NULL, "GDMA MULTI BLOCK TRANSFER DEMO", (rtos_task_t)gdma_multiblock_transfer_task, NULL, (3072), (1)) != SUCCESS) {
		printf("Cannot create dma_multiblock_transfer_task demo task\n\r");
	}

	rtos_sched_start();
	return 0;
}