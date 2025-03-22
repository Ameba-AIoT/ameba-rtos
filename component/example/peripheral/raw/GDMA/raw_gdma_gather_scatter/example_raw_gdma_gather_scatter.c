/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"

#define DATA_SIZE   			128
#define SRC_GATHER_COUNT 		16
#define SRC_GATHER_INTERVAL 	16

#define DST_SCATTER_COUNT 		16
#define DST_SCATTER_INTERVAL 	16


/*src/dst buffer must be aligned with cache line*/
ALIGNMTO(64) uint8_t gather_src_buf[DATA_SIZE];
ALIGNMTO(64) uint8_t gather_dst_buf[DATA_SIZE];

ALIGNMTO(64) uint8_t scatter_src_buf[DATA_SIZE];
ALIGNMTO(64) uint8_t scatter_dst_buf[DATA_SIZE * 2];

volatile uint8_t src_gather_done = 0;
volatile uint8_t dst_scatter_done = 0;
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
u32 gdma_source_gather_irq(void *data)
{
	GDMA_InitTypeDef *p_gdma_init = (GDMA_InitTypeDef *)data;

	/* Write the cached value back to memory */
	DCache_Clean((u32)gather_dst_buf, sizeof(gather_dst_buf));

	if (TransferType & GDMA_ClearINT(0, p_gdma_init->GDMA_ChNum)) {
		GDMA_ChnlFree(p_gdma_init->GDMA_Index, p_gdma_init->GDMA_ChNum);
		GDMA_Cmd(p_gdma_init->GDMA_Index, p_gdma_init->GDMA_ChNum, DISABLE);
		src_gather_done = 1;
	}

	return 0;
}

u32 gdma_dest_scatter_irq(void *data)
{
	GDMA_InitTypeDef *p_gdma_init = (GDMA_InitTypeDef *)data;

	/* Write the cached value back to memory */
	DCache_Clean((u32)scatter_dst_buf, sizeof(scatter_dst_buf));

	if (TransferType & GDMA_ClearINT(0, p_gdma_init->GDMA_ChNum)) {
		GDMA_ChnlFree(p_gdma_init->GDMA_Index, p_gdma_init->GDMA_ChNum);
		GDMA_Cmd(p_gdma_init->GDMA_Index, p_gdma_init->GDMA_ChNum, DISABLE);
		dst_scatter_done = 1;
	}

	return 0;
}

void gdma_source_gather_task(void)
{
	u8 channel = 0;
	GDMA_InitTypeDef gdma_init_struct;
	printf("****Source Gather Task Start****\n");
	/* 1. set data buffer. */
	for (uint32_t i = 0; i < sizeof(gather_src_buf); i++) {
		gather_src_buf[i] = i;
	}
	memset(gather_dst_buf, 0, sizeof(gather_dst_buf));

	DCache_Clean((u32)(gather_src_buf), sizeof(gather_src_buf));
	DCache_CleanInvalidate((u32)gather_dst_buf, sizeof(gather_dst_buf));

	/*2. initializa gdma*/
	channel = GDMA_ChnlAlloc(0, gdma_source_gather_irq, (u32)&gdma_init_struct, INT_PRI_MIDDLE);
	if (channel == 0xFF) {
		printf("No free channel\n");
		return;
	}
	GDMA_StructInit(&gdma_init_struct);
	gdma_init_struct.GDMA_DIR = TTFCMemToMem;
	gdma_init_struct.GDMA_Index = 0;
	gdma_init_struct.GDMA_ChNum = channel;
	gdma_init_struct.GDMA_DstMsize = MsizeOne;
	gdma_init_struct.GDMA_SrcMsize   = MsizeOne;
	gdma_init_struct.GDMA_SrcDataWidth = TrWidthTwoBytes;
	gdma_init_struct.GDMA_DstDataWidth = TrWidthTwoBytes;
	gdma_init_struct.GDMA_IsrType = (TransferType | BlockType | ErrType);

	gdma_init_struct.GDMA_SrcAddr = (u32)gather_src_buf;
	gdma_init_struct.GDMA_DstAddr = (u32)gather_dst_buf;
	gdma_init_struct.GDMA_BlockSize = (sizeof(gather_src_buf) >> gdma_init_struct.GDMA_SrcDataWidth);

	GDMA_Init(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum, &gdma_init_struct);

	GDMA_SourceGather(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum, SRC_GATHER_COUNT, SRC_GATHER_INTERVAL);

	GDMA_Cmd(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum, ENABLE);

	/*3. Wait for the GDMA transfer to complete */
	while (src_gather_done == 0);
	printf("Gather Src buffer data\n");
	gdma_dump_memory(gather_src_buf, sizeof(gather_src_buf));
	printf("Gather Dst buffer data\n");
	gdma_dump_memory(gather_dst_buf, sizeof(gather_dst_buf));
	printf("****transfer over****\n");
	rtos_task_delete(NULL);
}

void gdma_dest_scatter_task(void)
{
	u8 channel = 0;
	GDMA_InitTypeDef gdma_init_struct;
	printf("====destination Scatter Task Start====\n");
	/* 1. set data buffer. */
	for (uint32_t i = 0; i < sizeof(scatter_src_buf); i++) {
		scatter_src_buf[i] = i;
	}
	memset(scatter_dst_buf, 0,  sizeof(scatter_dst_buf));

	DCache_Clean((u32)(scatter_src_buf), sizeof(scatter_src_buf));
	DCache_CleanInvalidate((u32)scatter_dst_buf, sizeof(scatter_dst_buf));
	/* 2. initializa gdma*/
	channel = GDMA_ChnlAlloc(0, gdma_dest_scatter_irq, (u32)&gdma_init_struct, INT_PRI_LOWEST);
	if (channel == 0xFF) {
		printf("No free channel\n");
		return;
	}
	GDMA_StructInit(&gdma_init_struct);
	gdma_init_struct.GDMA_DIR = TTFCMemToMem;
	gdma_init_struct.GDMA_Index = 0;
	gdma_init_struct.GDMA_ChNum = channel;
	gdma_init_struct.GDMA_DstMsize = MsizeOne;
	gdma_init_struct.GDMA_SrcMsize   = MsizeOne;
	gdma_init_struct.GDMA_SrcDataWidth = TrWidthOneByte;
	gdma_init_struct.GDMA_DstDataWidth = TrWidthOneByte;
	gdma_init_struct.GDMA_IsrType = (TransferType | BlockType | ErrType);

	gdma_init_struct.GDMA_SrcAddr = (u32)scatter_src_buf;
	gdma_init_struct.GDMA_DstAddr = (u32)scatter_dst_buf;
	gdma_init_struct.GDMA_BlockSize = (sizeof(scatter_src_buf) >> gdma_init_struct.GDMA_DstDataWidth);

	GDMA_Init(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum, &gdma_init_struct);

	GDMA_DestinationScatter(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum, DST_SCATTER_COUNT, DST_SCATTER_INTERVAL);

	GDMA_Cmd(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum, ENABLE);

	/*3. Wait for the GDMA transfer to complete */
	while (dst_scatter_done == 0);
	printf("scatter src buffer data\n");
	gdma_dump_memory(scatter_src_buf, sizeof(scatter_src_buf));
	printf("scatter dst buffer data\n");
	gdma_dump_memory(scatter_dst_buf, sizeof(scatter_dst_buf));
	printf("====transfer over====\n");
	rtos_task_delete(NULL);
}

int example_raw_gdma_gather_scatter(void)
{
	if (rtos_task_create(NULL, "GDMA SRC_GATHER TRANSFER DEMO", (rtos_task_t)gdma_source_gather_task, NULL, (3072), (3)) != RTK_SUCCESS) {
		printf("Cannot create dma_multiblock_transfer_task demo task\n\r");
	}

	if (rtos_task_create(NULL, "GDMA DST_SCATTER TRANSFER DEMO", (rtos_task_t)gdma_dest_scatter_task, NULL, (3072), (2)) != RTK_SUCCESS) {
		printf("Cannot create dma_multiblock_transfer_task demo task\n\r");
	}

	rtos_sched_start();
	return 0;
}