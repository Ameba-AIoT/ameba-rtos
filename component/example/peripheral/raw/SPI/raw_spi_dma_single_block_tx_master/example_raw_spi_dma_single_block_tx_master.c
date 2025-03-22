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

/* compatible pinmux_funcid_name with AmebaD */
#ifndef CONFIG_AMEBAD
#define PINMUX_FUNCTION_SPIM	PINMUX_FUNCTION_SPI
// #define PINMUX_FUNCTION_SPIS	PINMUX_FUNCTION_SPI
#endif

#define DataFrameSize	DFS_8_BITS
#define ClockDivider	20
#define TEST_BUF_SIZE	2048 //for dma mode, buffer size should be multiple of CACHE_LINE_SIZE

/* for dma mode, start address of buffer should be CACHE_LINE_SIZE aligned*/
u8 MasterTxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

typedef struct {
	GDMA_InitTypeDef SSITxGdmaInitStruct;
	SPI_TypeDef *spi_dev;
	u32 Index;
} SPI_OBJ;

SPI_OBJ spi_master;

volatile int MasterTxDone;

/* GDMA IRQ Handler */
u32 spi_dma_tx_irq(void *Data)
{
	SPI_OBJ *spi_obj = (SPI_OBJ *) Data;
	PGDMA_InitTypeDef GDMA_InitStruct;

	GDMA_InitStruct = &spi_obj->SSITxGdmaInitStruct;

	/* Clear Pending ISR */
	GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	/*  Call user TX complete callback */
	MasterTxDone = 1;

	/* Set SSI DMA Disable */
	SSI_SetDmaEnable(spi_obj->spi_dev, DISABLE, SPI_BIT_TDMAE);
	GDMA_ChnlFree(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);

	return 0;
}

void Spi_free(SPI_OBJ *spi_obj)
{
	PGDMA_InitTypeDef GDMA_Tx = &spi_obj->SSITxGdmaInitStruct;

	/* Set SSI Tx DMA Disable */
	SSI_SetDmaEnable(spi_obj->spi_dev, DISABLE, SPI_BIT_TDMAE);

	/* Clear Pending ISR */
	GDMA_ClearINT(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum);
	GDMA_ChCleanAutoReload(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum, CLEAN_RELOAD_SRC_DST);
	GDMA_Cmd(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum, DISABLE);
	GDMA_ChnlFree(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum);

	SSI_Cmd(spi_obj->spi_dev, DISABLE);
}

void spi_singleblock_task(void)
{
	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	int i = 0;

	/* configure SPI1 as master tx with dma mode */
	spi_master.Index = 0x1;
	spi_master.spi_dev = SPI_DEV_TABLE[spi_master.Index].SPIx;

	/* Init SPI1 as master */
	RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, ENABLE);

	Pinmux_Config(SPI1_MOSI, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI1_MISO, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI1_SCLK, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI1_CS, PINMUX_FUNCTION_SPIM);

	SSI_InitTypeDef SSI_InitStructM;

	SSI_SetRole(spi_master.spi_dev, SSI_MASTER);

	SSI_StructInit(&SSI_InitStructM);
	SSI_InitStructM.SPI_Role = SSI_MASTER;
	SSI_InitStructM.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
	SSI_InitStructM.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
	SSI_InitStructM.SPI_DataFrameNumber = DataFrameSize;
	SSI_InitStructM.SPI_ClockDivider = ClockDivider; // Fssi_clk is 100MHz
	SSI_Init(spi_master.spi_dev, &SSI_InitStructM);

	_memset(MasterTxBuf, 0, TEST_BUF_SIZE);
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		*((u8 *)MasterTxBuf + i) = i;
	}

	printf("------------ SPI tx %d Bytes in DMA mode ---------------\n", TEST_BUF_SIZE);
	MasterTxDone = 0;

	SSI_SetDmaEnable(spi_master.spi_dev, ENABLE, SPI_BIT_TDMAE);
	SSI_TXGDMA_Init(spi_master.Index, &spi_master.SSITxGdmaInitStruct, &spi_master,
					(IRQ_FUN) spi_dma_tx_irq, (u8 *) MasterTxBuf, TEST_BUF_SIZE);

	i = 0;
	while (MasterTxDone == 0) {
		DelayMs(100);
		i++;
		if (i > 150) {
			printf("SPI Timeout\r\n");
			break;
		}
	}

	Spi_free(&spi_master);

	printf("SPI tx Demo finished.\n");

	rtos_task_delete(NULL);

}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_spi_dma_single_block_tx_master(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_singleblock_task"), (rtos_task_t)spi_singleblock_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		printf("\n\r%s rtos_task_create(spi_singleblock_task) failed", __FUNCTION__);
	}

	// rtos_sched_start();
	return 0;
}

