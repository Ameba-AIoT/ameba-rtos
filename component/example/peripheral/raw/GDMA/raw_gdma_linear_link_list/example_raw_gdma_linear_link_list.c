/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"

#define DATA_SIZE   8192
#define BLOCK_SIZE  1024
#define LINK_NODE_NUM    (DATA_SIZE/BLOCK_SIZE)

/* gdma link node*/
struct GDMA_CH_LLI gdma_link[LINK_NODE_NUM];

/*src/dst buffer*/
ALIGNMTO(64) uint8_t src_buf[LINK_NODE_NUM][BLOCK_SIZE];
ALIGNMTO(64) uint8_t dst_buf[LINK_NODE_NUM][BLOCK_SIZE];

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
u32 gdma_link_list_irq(void *data)
{
	u32 isr_type = 0;
	GDMA_InitTypeDef *p_gdma_init = (GDMA_InitTypeDef *)data;
	struct GDMA_CH_LLI *cur_node = &gdma_link[p_gdma_init->MuliBlockCunt];
	if (!gdma_test_compare((u8 *)cur_node->LliEle.Sarx, (u8 *)cur_node->LliEle.Darx, (cur_node->BlockSize <<  p_gdma_init->GDMA_SrcDataWidth))) {
		printf("Block %lu Error: Check Data Error!!!\n", p_gdma_init->MuliBlockCunt);
	}
	isr_type = GDMA_ClearINT(p_gdma_init->GDMA_Index, p_gdma_init->GDMA_ChNum);

	if (isr_type & BlockType) {
		printf("Block %lu Over\n", p_gdma_init->MuliBlockCunt++);
	}

	if (isr_type & TransferType) {
		printf("Last Block %lu Complete\n", p_gdma_init->MuliBlockCunt - 1);
		GDMA_ChnlFree(p_gdma_init->GDMA_Index, p_gdma_init->GDMA_ChNum);
		GDMA_Cmd(p_gdma_init->GDMA_Index, p_gdma_init->GDMA_ChNum, DISABLE);
		dma_done = 1;
	}

	return 0;
}

void gdma_link_list_task(void)
{
	u8 channel = 0;
	GDMA_InitTypeDef gdma_init_struct;
	/* 1. set and clear src and buffer. */
	for (int i = 0; i < LINK_NODE_NUM; i++) {
		memset(src_buf[i], i, BLOCK_SIZE);
	}
	DCache_Clean((u32)(src_buf), DATA_SIZE);
	DCache_CleanInvalidate((u32)dst_buf, DATA_SIZE);

	/*2. clear gdma link node*/
	memset(gdma_link, 0, sizeof(gdma_link));

	/*3. initializa gdma*/
	channel = GDMA_ChnlAlloc(0, gdma_link_list_irq, (u32)&gdma_init_struct, INT_PRI_MIDDLE);
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
	gdma_init_struct.GDMA_LlpSrcEn = 1;
	gdma_init_struct.GDMA_LlpDstEn = 1;
	gdma_init_struct.MaxMuliBlock = LINK_NODE_NUM;
	/*4. linear link list mode*/
	for (int i = 0; i < LINK_NODE_NUM; i++) {
		gdma_link[i].LliEle.Sarx = (u32)src_buf[i];
		gdma_link[i].LliEle.Darx = (u32)dst_buf[i];
		gdma_link[i].BlockSize = BLOCK_SIZE >>  gdma_init_struct.GDMA_SrcDataWidth;
		if (i == LINK_NODE_NUM - 1) {
			gdma_link[i].pNextLli = NULL;
		} else {
			gdma_link[i].pNextLli = &gdma_link[i + 1];
		}
	}

	GDMA_Init(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum, &gdma_init_struct);
	GDMA_SetLLP(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum, gdma_init_struct.MaxMuliBlock, gdma_link, 0);
	GDMA_Cmd(gdma_init_struct.GDMA_Index, gdma_init_struct.GDMA_ChNum, ENABLE);
	/*5. Wait for the GDMA transfer to complete */
	while (dma_done == 0);
	printf("transfer over\n");
	while (1);
}

int example_raw_gdma_linear_link_list(void)
{
	if (rtos_task_create(NULL, "GDMA LINK LIST TRANSFER DEMO", (rtos_task_t)gdma_link_list_task, NULL, (3072), (1)) != RTK_SUCCESS) {
		printf("Cannot create dma_multiblock_transfer_task demo task\n\r");
	}

	rtos_sched_start();
	return 0;
}