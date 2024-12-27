/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "spi_ext.h"
#include "os_wrapper.h"
#include <stdio.h>

#define DataFrameSize	DFS_8_BITS
#define TEST_BUF_SIZE	8192 // for dma mode, buffer size should be multiple of CACHE_LINE_SIZE
#define BLOCK_SIZE		2048 // number of bytes in a block
#define BLOCK_COUNT		TEST_BUF_SIZE / BLOCK_SIZE // multi block only works when block count >= 2
#define IF_SINGLE_BUF	TEST_BUF_SIZE % BLOCK_SIZE
#define ClockDivider	10 // IpClk of SPI is 100MHz
#define dfs_mask		0xFF

/* under dma mode, start address of buffer should be CACHE_LINE_SIZE aligned */
u8 SlaveRxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
u8 MasterTxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

typedef struct {
	GDMA_InitTypeDef SSIRxGdmaInitStruct;
	SPI_TypeDef *spi_dev;
	void *RxData;
	u32 RxLength;
	u32 Index;
} SPI_OBJ;

SPI_OBJ spi_slave;

volatile int SlaveRxDone;
volatile int MultiRxDone;

/* GDMA single block IRQ Handler */
u32 Gdma_singleblock_irq(void *Data)
{
	SPI_OBJ *spi_obj = (SPI_OBJ *) Data;
	PGDMA_InitTypeDef GDMA_InitStruct = &spi_obj->SSIRxGdmaInitStruct;

	/* Clear Pending ISR */
	GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	/*  Call user RX complete callback */
	MultiRxDone = 1;

	/* Set SSI DMA Disable */
	SSI_SetDmaEnable(spi_obj->spi_dev, DISABLE, SPI_BIT_RDMAE);
	GDMA_ChnlFree(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);

	return 0;
}

u32 Gdma_multiblock_irq(void *Data)
{
	PGDMA_InitTypeDef GDMA_InitStruct = (PGDMA_InitTypeDef) Data;
	u8 IsrTypeMap = 0;
	u8 this_is_last_two = 0;

	if (GDMA_InitStruct->MaxMuliBlock == GDMA_InitStruct->MuliBlockCunt + 2) {
		this_is_last_two = 1;
	}

	// Clean Auto Reload Bit
	if (this_is_last_two) {
		GDMA_ChCleanAutoReload(0, GDMA_InitStruct->GDMA_ChNum, CLEAN_RELOAD_SRC_DST);
	}

	/* reload cache from sram before use the DMA destination data */

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
		MultiRxDone = 1;
	}

	return 0;
}

bool SSI_Multi_RXGDMA_Init(u8 Index, PGDMA_InitTypeDef GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pRxData, u32 Length)
{
	u8 GdmaChnl;
	IRQn_Type IrqNum;

	assert_param(GDMA_InitStruct != NULL);

	DCache_CleanInvalidate((u32) pRxData, Length);

	GdmaChnl = GDMA_ChnlAlloc(0, CallbackFunc, (u32)CallbackData, 6);
	if (GdmaChnl == 0xFF) {
		// No Available DMA channel
		return FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct->GDMA_DIR = TTFCPeriToMem;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->GDMA_SrcHandshakeInterface = SPI_DEV_TABLE[Index].Rx_HandshakeInterface;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)&SPI_DEV_TABLE[Index].SPIx->SPI_DRx[0];
	GDMA_InitStruct->GDMA_Index = 0;
	GDMA_InitStruct->GDMA_ChNum = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType | SrcTransferType | DstTransferType);

	GDMA_InitStruct->GDMA_SrcMsize = MsizeEight;
	GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_SrcInc = NoChange;
	GDMA_InitStruct->GDMA_DstInc = IncType;

	/* Cofigure GDMA transfer */
	if (DataFrameSize > 8) {
		/* 16~9 bits mode */
		GDMA_InitStruct->GDMA_SrcMsize = MsizeFour;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;

		if (((Length & 0x03)  == 0) && (((u32)(pRxData) & 0x03) == 0)) {
			/* 4-bytes aligned, move 4 bytes each transfer */
			GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
		} else if (((Length & 0x01) == 0) && (((u32)(pRxData) & 0x01) == 0)) {
			/* 2-bytes aligned, move 2 bytes each transfer */
			GDMA_InitStruct->GDMA_DstMsize = MsizeEight;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthTwoBytes;
		} else {
			printf("%s: Aligment Err: pTxData=0x%p, Length=%lu\n", __FUNCTION__, pRxData, Length);
			return FALSE;
		}
	} else {
		/* 8~4 bits mode */
		GDMA_InitStruct->GDMA_SrcMsize = MsizeFour;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;

		if (((Length & 0x03) == 0) && (((u32)(pRxData) & 0x03) == 0)) {
			/* 4-bytes aligned, move 4 bytes each transfer */
			GDMA_InitStruct->GDMA_DstMsize = MsizeOne;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
		} else {
			GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
		}
	}
	GDMA_InitStruct->GDMA_BlockSize = BLOCK_SIZE >> GDMA_InitStruct->GDMA_SrcDataWidth; //based on src width
	assert_param(GDMA_InitStruct->GDMA_BlockSize <= 4096);

//	GDMA_ChnlRegister(0, GDMA_InitStruct->GDMA_ChNum);
	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->MaxMuliBlock = Length / BLOCK_SIZE; // number of blocks
	GDMA_InitStruct->GDMA_ReloadSrc = 1;
	GDMA_InitStruct->GDMA_ReloadDst = 0;
	GDMA_InitStruct->GDMA_DstAddr = (u32)pRxData;

	IrqNum = GDMA_GetIrqNum(0, GDMA_InitStruct->GDMA_ChNum);
	InterruptRegister((IRQ_FUN)Gdma_multiblock_irq, IrqNum, (u32)GDMA_InitStruct, 6);
	InterruptEn(IrqNum, 6);

	/* Enable GDMA for RX */
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}

void Ssi_Print(u8 *pSrc, u8 *pDst, u32 Length)
{
	u32 index;

	for (index = 0; index < Length; index++) {
		if ((pSrc[index] & dfs_mask) != pDst[index]) {
			printf("[%lu]: 0x%x ---- 0x%x\n", index, pSrc[index] & dfs_mask, pDst[index]);
		}
	}
}

bool Ssi_DataCompare(u8 *pSrc, u8 *pDst, u32 Length)
{
	u32 index;
	u8 res = TRUE;

	for (index = 0; index < Length; ++index) {
		if ((pSrc[index] & dfs_mask) != pDst[index]) {
			res = FALSE;
		}
	}

	return res;
}

void Spi_free(SPI_OBJ    *spi_obj)
{
	PGDMA_InitTypeDef GDMA_Rx = &spi_obj->SSIRxGdmaInitStruct;

	/* Set SSI Rx DMA Disable */
	SSI_SetDmaEnable(spi_obj->spi_dev, DISABLE, SPI_BIT_RDMAE);

	/* Clear Pending ISR */
	GDMA_ClearINT(GDMA_Rx->GDMA_Index, GDMA_Rx->GDMA_ChNum);
	GDMA_ChCleanAutoReload(GDMA_Rx->GDMA_Index, GDMA_Rx->GDMA_ChNum, CLEAN_RELOAD_SRC_DST);
	GDMA_Cmd(GDMA_Rx->GDMA_Index, GDMA_Rx->GDMA_ChNum, DISABLE);
	GDMA_ChnlFree(GDMA_Rx->GDMA_Index, GDMA_Rx->GDMA_ChNum);

	SSI_Cmd(spi_obj->spi_dev, DISABLE);
}

void spi_multiblock_task(void)
{
	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	SSI_InitTypeDef SSI_InitStruct;
	int i = 0;
	int cmp_result = 1;
	SlaveRxDone = 0;
	MultiRxDone  = 0;

	/* configure SPI0 as slave RX under DMA mode */
	spi_slave.Index = 0x0;
	spi_slave.spi_dev = SPI_DEV_TABLE[spi_slave.Index].SPIx;

	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	Pinmux_Config(SPI0_MOSI, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI0_MISO, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI0_SCLK, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI0_CS, PINMUX_FUNCTION_SPI);

	PAD_PullCtrl(SPI0_CS, GPIO_PuPd_UP);
	PAD_PullCtrl(SPI0_SCLK, GPIO_PuPd_DOWN); // pull down for idel level is low

	SSI_SetRole(spi_slave.spi_dev, SSI_SLAVE);

	SSI_StructInit(&SSI_InitStruct);
	SSI_InitStruct.SPI_Role = SSI_SLAVE;
	SSI_InitStruct.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
	SSI_InitStruct.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
	SSI_InitStruct.SPI_DataFrameSize = DataFrameSize;
	SSI_InitStruct.SPI_ClockDivider = ClockDivider;
	SSI_Init(spi_slave.spi_dev, &SSI_InitStruct);

	_memset(SlaveRxBuf, 0, TEST_BUF_SIZE);
	_memset(MasterTxBuf, 0, TEST_BUF_SIZE);
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		*((u8 *)MasterTxBuf + i) = i;
	}

	printf("----------- SPI DMA multi-block RX test -----------\n");

#if BLOCK_COUNT >= 2
	SSI_Multi_RXGDMA_Init(spi_slave.Index, &spi_slave.SSIRxGdmaInitStruct, &spi_slave, (IRQ_FUN) Gdma_multiblock_irq, SlaveRxBuf, TEST_BUF_SIZE);
	SSI_SetDmaEnable(spi_slave.spi_dev, ENABLE, SPI_BIT_RDMAE);
#else
	SSI_RXGDMA_Init(spi_slave.Index, &spi_slave.SSIRxGdmaInitStruct, &spi_slave, (IRQ_FUN) Gdma_singleblock_irq, SlaveRxBuf, TEST_BUF_SIZE);
	SSI_SetDmaEnable(spi_slave.spi_dev, ENABLE, SPI_BIT_RDMAE);
#endif

	i = 0;
	while (MultiRxDone == 0) {
		DelayMs(100);
		i++;
		if (i > 150) {
			printf("SPI Multi Block Timeout\r\n");
			break;
		}
	}

//	Ssi_Print(MasterTxBuf, SlaveRxBuf, TEST_BUF_SIZE);
	cmp_result = Ssi_DataCompare(MasterTxBuf, SlaveRxBuf, TEST_BUF_SIZE);
	printf("\r\nSPI RX test: %s!\r\n", cmp_result ? "OK" : "fail");

	Spi_free(&spi_slave);

	printf("SPI rx Demo finished.\n\n");

	rtos_task_delete(NULL);

}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
#if IF_SINGLE_BUF
	printf("Data buffer size must be integer multiple of block size!\r\n");
#else
	if (rtos_task_create(NULL, ((const char *)"spi_multiblock_task"), (rtos_task_t)spi_multiblock_task, NULL, 1024 * 4, 1) != SUCCESS) {
		printf("\r\n%s rtos_task_create(spi_multiblock_task) failed\r\n", __FUNCTION__);
	}

	rtos_sched_start();
	return 0;
#endif
}

