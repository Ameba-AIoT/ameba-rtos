/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "main.h"
#include "os_wrapper.h"

#define DATA_SIZE   4096

/* abort transfer*/
uint8_t abort_transfer = 0;


/*src/dst buffer*/
ALIGNMTO(64) uint8_t src_buf[DATA_SIZE];
ALIGNMTO(64) uint8_t dst_buf[DATA_SIZE];

volatile uint8_t dma_done = 0;

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
u32 gdma_single_block_irq(void *data)
{
	u32 isr_type = 0;
	GDMA_InitTypeDef *p_gdma_init = (GDMA_InitTypeDef *)data;

	/* Write the cached value back to memory */
	DCache_Invalidate((u32)dst_buf, DATA_SIZE);
	if (!gdma_test_compare((u8 *)src_buf, (u8 *)dst_buf, DATA_SIZE)) {
		printf("Block %lu Error: Check Data Error!!!\n", p_gdma_init->MuliBlockCunt);
	}
	isr_type = GDMA_ClearINT(0, p_gdma_init->GDMA_ChNum);

	if (isr_type & BlockType) {
		printf("Block %lu Over\r\n\r\n", p_gdma_init->MuliBlockCunt++);
	}

	if (isr_type & TransferType) {
		printf("Last Block %lu Complete\n", p_gdma_init->MuliBlockCunt - 1);
		GDMA_ChnlFree(p_gdma_init->GDMA_Index, p_gdma_init->GDMA_ChNum);
		GDMA_Cmd(p_gdma_init->GDMA_Index, p_gdma_init->GDMA_ChNum, DISABLE);
		dma_done = 1;
	}

	return 0;
}

void gdma_suspend_resume_task(void)
{
	u8 channel = 0;
	GDMA_InitTypeDef gdma_init_struct;
	/* 1. set and clear src and buffer. */

	memset(src_buf, 0xA, DATA_SIZE);

	DCache_Clean((u32)(src_buf), DATA_SIZE);
	DCache_CleanInvalidate((u32)dst_buf, DATA_SIZE);

	/*2. initializa gdma*/
	channel = GDMA_ChnlAlloc(0, gdma_single_block_irq, (u32)&gdma_init_struct, INT_PRI_MIDDLE);
	if (channel == 0xFF) {
		printf("No free channel\n");
		return;
	}
	GDMA_StructInit(&gdma_init_struct);
	gdma_init_struct.GDMA_DIR = TTFCMemToMem;
	gdma_init_struct.GDMA_Index = 0;
	gdma_init_struct.GDMA_ChNum = channel;
	gdma_init_struct.GDMA_DstMsize = MsizeFour;
	gdma_init_struct.GDMA_SrcMsize   = MsizeFour;
	gdma_init_struct.GDMA_SrcDataWidth = TrWidthFourBytes;
	gdma_init_struct.GDMA_DstDataWidth = TrWidthFourBytes;
	gdma_init_struct.GDMA_IsrType = (TransferType | BlockType | ErrType);

	gdma_init_struct.GDMA_SrcAddr = (u32)src_buf;
	gdma_init_struct.GDMA_DstAddr = (u32)dst_buf;
	gdma_init_struct.GDMA_ReloadDst = 0;
	gdma_init_struct.GDMA_ReloadSrc = 0;
	gdma_init_struct.GDMA_BlockSize = DATA_SIZE >> gdma_init_struct.GDMA_SrcDataWidth;

	GDMA_Init(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum, &gdma_init_struct);
	GDMA_Cmd(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum, ENABLE);
	for (int i = 0; i < 2; i++) {
		GDMA_Suspend(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum);
		uint32_t size = GDMA_GetBlkSize(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum);
		printf("transfer suspend, and %lu bytes have been moved\n", size);
		// gdma_dump_memory(dst_buf, size);
		DelayMs(2000);
		if (abort_transfer) {
			GDMA_Abort(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum);
			GDMA_ChnlFree(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum);
			GDMA_Cmd(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum, DISABLE);
			printf("transfer abort\n");
			dma_done = 2;
			break;
		}
		GDMA_Resume(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum);
		printf("transfer resume\n");

	}

	/*3. Wait for the GDMA transfer to complete */
	while (dma_done == 0);
	printf("transfer over\n");
	while (1);
}

int example_raw_gdma_suspend_resume_abort(void)
{
	if (rtos_task_create(NULL, "GDMA SUSPEND RESUME DEMO", (rtos_task_t)gdma_suspend_resume_task, NULL, (3072), (1)) != RTK_SUCCESS) {
		printf("Cannot create dma_multiblock_transfer_task demo task\n\r");
	}

	rtos_sched_start();
	return 0;
}