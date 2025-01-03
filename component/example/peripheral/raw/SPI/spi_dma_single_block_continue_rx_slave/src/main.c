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
#define ClockDivider	20
#define TEST_BUF_SIZE	4096 //for dma mode, buffer size should be multiple of CACHE_LINE_SIZE
#define dfs_mask 		0xFF

/* for dma mode, start address of buffer should be CACHE_LINE_SIZE aligned*/
u8 MasterTxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
u8 SlaveRxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

typedef struct {
	GDMA_InitTypeDef SSIRxGdmaInitStruct;
	SPI_TypeDef *spi_dev;
	u32 Index;
} SPI_OBJ;

SPI_OBJ spi_slave;

volatile int SlaveRxDone;

/* GDMA IRQ Handler */
u32 spi_dma_rx_irq(void *Data)
{
	SPI_OBJ *spi_obj = (SPI_OBJ *) Data;
	PGDMA_InitTypeDef GDMA_InitStruct;

	GDMA_InitStruct = &spi_obj->SSIRxGdmaInitStruct;

	/* Clear Pending ISR */
	GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	/*  Call user RX complete callback */
	SlaveRxDone = 1;

	/* Set SSI DMA Disable */
	SSI_SetDmaEnable(spi_obj->spi_dev, DISABLE, SPI_BIT_RDMAE);
	GDMA_ChnlFree(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);

	return 0;
}

void Spi_free(SPI_OBJ *spi_obj)
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

void Ssi_Print(u8 *pSrc, u8 *pDst, u32 Length)
{
	u32 index;

	for (index = 0; index < Length; index++) {
		printf("[%lu]: 0x%x ---- 0x%x\n", index, pSrc[index] & dfs_mask, pDst[index]);
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

void spi_singleblock_task(void)
{
	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	SSI_InitTypeDef SSI_InitStruct;
	int i = 0;
	int cmp_result = 1;
	SlaveRxDone = 0;

	/* configure SPI0 as slave RX under DMA mode */
	spi_slave.Index = 0x0;
	spi_slave.spi_dev = SPI_DEV_TABLE[spi_slave.Index].SPIx;

	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	Pinmux_Config(SPI0_MOSI, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI0_MISO, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI0_SCLK, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI0_CS, PINMUX_FUNCTION_SPI);

	PAD_PullCtrl(SPI0_CS, GPIO_PuPd_UP);
	PAD_PullCtrl(SPI0_SCLK, GPIO_PuPd_DOWN); // pull down for idle level is low

	SSI_SetRole(spi_slave.spi_dev, SSI_SLAVE);

	SSI_StructInit(&SSI_InitStruct);
	SSI_InitStruct.SPI_Role = SSI_SLAVE;
	SSI_InitStruct.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
	SSI_InitStruct.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
	SSI_InitStruct.SPI_DataFrameNumber = DataFrameSize;
	SSI_InitStruct.SPI_ClockDivider = ClockDivider; // Fssi_clk is 100MHz
	SSI_Init(spi_slave.spi_dev, &SSI_InitStruct);

	_memset(SlaveRxBuf, 0, TEST_BUF_SIZE);
	_memset(MasterTxBuf, 0, TEST_BUF_SIZE);
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		*((u8 *)MasterTxBuf + i) = i;
	}

	printf("------------ SPI rx %d Bytes in DMA mode ---------------\n", TEST_BUF_SIZE);

	SSI_RXGDMA_Init(spi_slave.Index, &spi_slave.SSIRxGdmaInitStruct, &spi_slave,
					(IRQ_FUN) spi_dma_rx_irq, (u8 *) SlaveRxBuf, TEST_BUF_SIZE);
	SSI_SetDmaEnable(spi_slave.spi_dev, ENABLE, SPI_BIT_RDMAE);

	i = 0;
	while (SlaveRxDone == 0) {
		DelayMs(100);
		i++;
		if (i > 150) {
			printf("SPI Timeout\r\n");
			break;
		}
	}

//	Ssi_Print(MasterTxBuf, SlaveRxBuf, TEST_BUF_SIZE);
	cmp_result = Ssi_DataCompare(MasterTxBuf, SlaveRxBuf, TEST_BUF_SIZE);
	printf("\r\nSPI RX test: %s!\r\n", cmp_result ? "OK" : "fail");

	Spi_free(&spi_slave);

	printf("SPI rx Demo finished.\n");

	rtos_task_delete(NULL);

}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_singleblock_task"), (rtos_task_t)spi_singleblock_task, NULL, 1024 * 4, 1) != SUCCESS) {
		printf("\r\n%s rtos_task_create(spi_singleblock_task) failed\r\n", __FUNCTION__);
	}

	rtos_sched_start();
	return 0;
}

