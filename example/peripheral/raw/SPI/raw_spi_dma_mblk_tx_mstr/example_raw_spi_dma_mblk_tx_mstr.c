/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "example_spi_ext.h"
#include "os_wrapper.h"
#include <stdio.h>

/* compatible pinmux_funcid_name with RTL872xD */
#ifndef CONFIG_AMEBAD
#if defined(CONFIG_AMEBAGREEN2)
#define PINMUX_FUNCTION_SPIM	PINMUX_FUNCTION_SPI1
//#define PINMUX_FUNCTION_SPIS	PINMUX_FUNCTION_SPI0
#else
#define PINMUX_FUNCTION_SPIM	PINMUX_FUNCTION_SPI
//#define PINMUX_FUNCTION_SPIS	PINMUX_FUNCTION_SPI
#endif
#endif

#define SSI_DEBUG_ENABLE	0
#define DataFrameSize		8
#define TEST_BUF_SIZE		8192//8400 //for dma mode, buffer size should be multiple of CACHE_LINE_SIZE
#define BLOCK_SIZE			2048  //number of bytes in a block
#define ClockDivider		100  //IpClk of SPI0 or SPI1 is 100MHz

/* under dma mode, start address of buffer should be CACHE_LINE_SIZE aligned */
u8 MasterTxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

SPI_TypeDef *spi_master;
GDMA_InitTypeDef SSITxGdmaInitStruct;

volatile int MasterTxDone;
volatile int MultiTxDone;

static u32 If_single = TEST_BUF_SIZE % BLOCK_SIZE;

/* GDMA single block IRQ Handler */
u32 Gdma_singleblock_irq(void *Data)
{
	PGDMA_InitTypeDef GDMA_InitStruct = (PGDMA_InitTypeDef) Data;

	/* Clear Pending ISR */
	GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	/*  Call user TX complete callback */
	MasterTxDone = 1;

	/* Set SSI DMA Disable */
	SSI_SetDmaEnable(spi_master, DISABLE, SPI_BIT_TDMAE);
	GDMA_ChnlFree(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);

	return 0;
}

u32 Gdma_multiblock_irq(void *Data)
{
	PGDMA_InitTypeDef GDMA_InitStruct = (PGDMA_InitTypeDef) Data;
	u8 *pDstData = NULL;
	u8 IsrTypeMap = 0;
	u8 this_is_last_two = 0;

	if (GDMA_InitStruct->MaxMuliBlock == GDMA_InitStruct->MuliBlockCunt + 2) {
		this_is_last_two = 1;
	}

	// Clean Auto Reload Bit
	if (this_is_last_two) {
		GDMA_ChCleanAutoReload(0, GDMA_InitStruct->GDMA_ChNum, CLEAN_RELOAD_SRC_DST);
	}

	pDstData = (u8 *)GDMA_InitStruct->GDMA_DstAddr ;

	/* reload cache from sram before use the DMA destination data */
	DCache_Invalidate(((u32)pDstData & CACHE_LINE_ADDR_MSK), (BLOCK_SIZE + CACHE_LINE_SIZE));

#if defined (SSI_DEBUG_ENABLE) && (SSI_DEBUG_ENABLE == 1)
	u8 *pSrcData = (u8 *)(GDMA_InitStruct->GDMA_SrcAddr + BLOCK_SIZE * (GDMA_InitStruct->MuliBlockCunt));
	for (int num = 0; num < BLOCK_SIZE; num++) {
		printf("\r\src: %d\n", *(pSrcData + num));
	}
#endif

	DCache_Clean((u32)pDstData, (BLOCK_SIZE + CACHE_LINE_SIZE));

	//Clear Pending ISR, next block will start transfer
	IsrTypeMap = GDMA_ClearINT(0, GDMA_InitStruct->GDMA_ChNum);

	if (IsrTypeMap & BlockType) {
//		printf("DMA Block %d\n",GDMA_InitStruct->MuliBlockCunt);
		GDMA_InitStruct->MuliBlockCunt++;
	}

	//last IRQ is TransferType in multi-block
	if (IsrTypeMap & TransferType) {
//		printf("DMA Block %d\n",GDMA_InitStruct->MuliBlockCunt);
//		printf("DMA data complete MaxMuliBlock:%d \n", GDMA_InitStruct->MaxMuliBlock);
		if (!If_single) {
			MasterTxDone = 1;
		} else {
			MultiTxDone = 1;
		}
		GDMA_ChnlFree(0, GDMA_InitStruct->GDMA_ChNum);
	}

	return 0;
}

bool SSI_Multi_TXGDMA_Init(PGDMA_InitTypeDef GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pTxData, u32 Length)
{
	u8 GdmaChnl;
	IRQn_Type IrqNum;

	assert_param(GDMA_InitStruct != NULL);

	DCache_CleanInvalidate((u32) pTxData, Length);

	GdmaChnl = GDMA_ChnlAlloc(0, CallbackFunc, (u32)CallbackData, 6);
	if (GdmaChnl == 0xFF) {
		printf("No Available DMA channel... \r\n");
		return FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct->GDMA_DIR = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_DstHandshakeInterface = SPI_DEV_TABLE[1].Tx_HandshakeInterface;
#ifndef CONFIG_AMEBAD
	GDMA_InitStruct->GDMA_DstAddr = (u32)&SPI_DEV_TABLE[1].SPIx->SPI_DRx;
#else
	GDMA_InitStruct->GDMA_DstAddr = (u32)&SPI_DEV_TABLE[1].SPIx->DR;
#endif
	GDMA_InitStruct->GDMA_Index = 0;
	GDMA_InitStruct->GDMA_ChNum = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);

	GDMA_InitStruct->GDMA_SrcMsize = MsizeOne;
	GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_DstInc = NoChange;
	GDMA_InitStruct->GDMA_SrcInc = IncType;

	/*  Cofigure GDMA transfer */
	if (DataFrameSize > 8) {
		/*  16~9 bits mode */
		if (((Length & 0x03) == 0) && (((u32)(pTxData) & 0x03) == 0)) {
			/*  4-bytes aligned, move 4 bytes each transfer */
			GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
			GDMA_InitStruct->GDMA_DstMsize  = MsizeEight;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthTwoBytes;
		} else if (((Length & 0x01) == 0) && (((u32)(pTxData) & 0x01) == 0)) {
			/*  2-bytes aligned, move 2 bytes each transfer */
			GDMA_InitStruct->GDMA_SrcMsize   = MsizeEight;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;
			GDMA_InitStruct->GDMA_DstMsize  = MsizeEight;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthTwoBytes;
		} else {
			printf("SSI_TXGDMA_Init: Aligment Err: pTxData=0x%p,  Length=%lu\n", pTxData, Length);
			return FALSE;
		}
	} else {
		/*  8~4 bits mode */
		if (((Length & 0x03) == 0) && (((u32)(pTxData) & 0x03) == 0)) {
			/*  4-bytes aligned, move 4 bytes each transfer */
			GDMA_InitStruct->GDMA_SrcMsize = MsizeOne;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
		} else {
			GDMA_InitStruct->GDMA_SrcMsize = MsizeFour;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
		}

		GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	}

	GDMA_InitStruct->GDMA_BlockSize = BLOCK_SIZE >> GDMA_InitStruct->GDMA_SrcDataWidth; //based on src width
	assert_param(GDMA_InitStruct->GDMA_BlockSize <= 4096);

//	printf("BlockSize = %d\n", GDMA_InitStruct->GDMA_BlockSize);

//	GDMA_ChnlRegister(0, GDMA_InitStruct->GDMA_ChNum);
	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->MaxMuliBlock = Length / BLOCK_SIZE;  //number of blocks
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->GDMA_ReloadDst = 1;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)pTxData;

	IrqNum = GDMA_GetIrqNum(0, GDMA_InitStruct->GDMA_ChNum);
	InterruptRegister((IRQ_FUN)Gdma_multiblock_irq, IrqNum, (u32)GDMA_InitStruct, 6);
	InterruptEn(IrqNum, 6);

	/*  Enable GDMA for TX */
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}

void Spi_free(SPI_TypeDef *spi_dev)
{
	PGDMA_InitTypeDef GDMA_Tx = &SSITxGdmaInitStruct;

	/* Set SSI Tx DMA Disable */
	SSI_SetDmaEnable(spi_dev, DISABLE, SPI_BIT_TDMAE);

	/* Clear Pending ISR */
	GDMA_ClearINT(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum);
	GDMA_ChCleanAutoReload(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum, CLEAN_RELOAD_SRC_DST);
	GDMA_Cmd(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum, DISABLE);
	GDMA_ChnlFree(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum);

	SSI_Cmd(spi_dev, DISABLE);
}

void spi_multiblock_task(void)
{
	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	int i = 0;
	SSI_InitTypeDef SSI_InitStruct;

	RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, ENABLE);
	Pinmux_Config(SPI1_MOSI, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI1_MISO, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI1_SCLK, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI1_CS, PINMUX_FUNCTION_SPIM);

	/* init master SPI */
	spi_master = SPI_DEV_TABLE[1].SPIx;
	SSI_SetRole(spi_master, SSI_MASTER);

	SSI_StructInit(&SSI_InitStruct);
	SSI_InitStruct.SPI_Role = SSI_MASTER;
	SSI_InitStruct.SPI_SclkPhase  = SCPH_TOGGLES_IN_MIDDLE;
	SSI_InitStruct.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
	SSI_InitStruct.SPI_DataFrameSize = DataFrameSize - 1;
	SSI_InitStruct.SPI_ClockDivider = ClockDivider;
	SSI_Init(spi_master, &SSI_InitStruct);

	_memset(MasterTxBuf, 0, TEST_BUF_SIZE);
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		*((u8 *)MasterTxBuf + i) = i;
	}

	printf("----------- SPI DMA multi-block TX test -----------\n");

	MasterTxDone = 0;
	MultiTxDone  = 0;

	SSI_SetDmaEnable(spi_master, ENABLE, SPI_BIT_TDMAE);
	SSI_Multi_TXGDMA_Init(&SSITxGdmaInitStruct, spi_master, (IRQ_FUN) Gdma_multiblock_irq, MasterTxBuf, TEST_BUF_SIZE);

	/* transfer the remaining data with single block */
	i = 0;
	if (If_single) {
		while (MultiTxDone == 0) {
			DelayMs(5);
			i++;
			if (i > 3000) {
				printf("SPI Multi Block Timeout\r\n");
				break;
			}
		}

		SSI_TXGDMA_Init(1, &SSITxGdmaInitStruct, &SSITxGdmaInitStruct, (IRQ_FUN) Gdma_singleblock_irq, (u8 *)(MasterTxBuf + TEST_BUF_SIZE - If_single), If_single);
	}

	i = 0;
	while (MasterTxDone == 0) {
		DelayMs(100);
		i++;
		if (i > 150) {
			printf("SPI Timeout\r\n");
			break;
		}
	}

	Spi_free(spi_master);

	printf("SPI tx Demo finished.\n\n");

	rtos_task_delete(NULL);

}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_raw_spi_multi_dma_tx_master(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_multiblock_task"), (rtos_task_t)spi_multiblock_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		printf("\n\r%s rtos_task_create(spi_multiblock_task) failed", __FUNCTION__);
	}

	// rtos_sched_start();
	return 0;
}

