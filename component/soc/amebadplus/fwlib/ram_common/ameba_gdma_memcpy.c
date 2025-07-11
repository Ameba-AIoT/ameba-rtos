/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

struct gdma_memcopy_s {
	u32 ch_num;
	volatile u32 dma_done;
	GDMA_InitTypeDef GDMA_InitStruct;
};

TIMESENSITIVE_DATA_SECTION
struct gdma_memcopy_s gdma_memcpy;
//rtos_sema_t dma_memcpy_sema = NULL;

TIMESENSITIVE_TEXT_SECTION
static u32 memcpy_gdma_int(void *pData)
{
	/* To avoid gcc warnings */
	UNUSED(pData);

	/* Clear Pending ISR */
	GDMA_ClearINT(0, gdma_memcpy.ch_num);
	GDMA_Cmd(0, gdma_memcpy.ch_num, DISABLE);

	gdma_memcpy.dma_done = 1;
	//rtos_sema_give(dma_memcpy_sema);
	return TRUE;
}

TIMESENSITIVE_TEXT_SECTION
void memcpy_gdma_init(void)
{
	gdma_memcpy.dma_done = 1;
	gdma_memcpy.ch_num = GDMA_ChnlAlloc(0, (IRQ_FUN)memcpy_gdma_int, (u32)NULL, INT_PRI_MIDDLE);

	GDMA_StructInit(&(gdma_memcpy.GDMA_InitStruct));
	gdma_memcpy.GDMA_InitStruct.GDMA_ChNum = gdma_memcpy.ch_num;
	gdma_memcpy.GDMA_InitStruct.GDMA_Index = 0;
	gdma_memcpy.GDMA_InitStruct.GDMA_IsrType = (TransferType | ErrType);

	gdma_memcpy.GDMA_InitStruct.GDMA_SrcMsize = MsizeEight;
	gdma_memcpy.GDMA_InitStruct.GDMA_DstMsize = MsizeEight;

	//rtos_sema_create_binary(&dma_memcpy_sema);
}

TIMESENSITIVE_TEXT_SECTION
static inline u32 memcpy_use_cpu(void *dest, void *src, u32 size)
{
	/* To avoid gcc warnings */
	(void) dest;
	(void) src;

	if (size < 128) {
		return TRUE;
	}

	if (gdma_memcpy.dma_done == 0) {
		return TRUE;
	}

	return FALSE;
}

TIMESENSITIVE_TEXT_SECTION
int memcpy_gdma(void *dest, void *src, u32 size)
{
	u32 size_4byte = size & ~(0x03);
	u32 left = size & (0x03);

	if (memcpy_use_cpu(dest, src, size) == TRUE) {
		_memcpy(dest, src, size);

		return 0;
	}
	gdma_memcpy.dma_done = 0;

	if (((u32)(src) & 0x03) || ((u32)(dest) & 0x03)) {
		gdma_memcpy.GDMA_InitStruct.GDMA_SrcDataWidth = TrWidthOneByte;
		gdma_memcpy.GDMA_InitStruct.GDMA_DstDataWidth = TrWidthOneByte;
		gdma_memcpy.GDMA_InitStruct.GDMA_BlockSize = size;
	} else {
		/* 4-bytes aligned, move 4 bytes each transfer */
		gdma_memcpy.GDMA_InitStruct.GDMA_SrcDataWidth = TrWidthFourBytes;
		gdma_memcpy.GDMA_InitStruct.GDMA_DstDataWidth = TrWidthFourBytes;
		gdma_memcpy.GDMA_InitStruct.GDMA_BlockSize = size_4byte >> 2;

		if (left != 0) {
			char *dst0 = (char *) dest + size_4byte;
			char *src0 = (char *) src + size_4byte;
			while (left--) {
				*dst0++ = *src0++;
			}
		}
	}

	gdma_memcpy.GDMA_InitStruct.GDMA_SrcAddr = (u32)(src);
	gdma_memcpy.GDMA_InitStruct.GDMA_DstAddr = (u32)(dest);
	GDMA_Init(0, gdma_memcpy.ch_num, &(gdma_memcpy.GDMA_InitStruct));
	GDMA_Cmd(0, gdma_memcpy.ch_num, ENABLE);

	//rtos_sema_take(dma_memcpy_sema, portMAX_DELAY);
	while (gdma_memcpy.dma_done == 0);

	return 0;
}

