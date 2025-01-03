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

#define DataFrameSize	8
#define dfs_mask		0xFF
#define ClockDivider	20
#define TEST_BUF_SIZE	512 //for dma mode, buffer size should be multiple of CACHE_LINE_SIZE

/* for dma mode, start address of buffer should be CACHE_LINE_SIZE aligned*/
u8 MasterTxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
u8 MasterRxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
u8 SlaveTxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
u8 SlaveRxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

volatile int MasterTxDone;
volatile int MasterRxDone;
volatile int SlaveTxDone;
volatile int SlaveRxDone;

typedef struct {
	GDMA_InitTypeDef SSITxGdmaInitStruct;
	GDMA_InitTypeDef SSIRxGdmaInitStruct;

	SPI_TypeDef *spi_dev;

	void *RxData;
	void *TxData;
	u32  RxLength;
	u32  TxLength;

	u32 Index;
} SPI_OBJ;

SPI_OBJ spi_master;
SPI_OBJ spi_slave;

/* GDMA IRQ Handler */
u32 Ssi_dma_tx_irq(void *Data)
{
	SPI_OBJ *spi_obj = (SPI_OBJ *) Data;
	PGDMA_InitTypeDef GDMA_InitStruct;

	GDMA_InitStruct = &spi_obj->SSITxGdmaInitStruct;

	/* Clear Pending ISR */
	GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	/*  Call user TX complete callback */
	if (spi_obj->Index == 0) {
		SlaveTxDone = 1;
	} else {
		MasterTxDone = 1;
	}

	/* Set SSI DMA Disable */
	SSI_SetDmaEnable(spi_obj->spi_dev, DISABLE, SPI_BIT_TDMAE);
	GDMA_ChnlFree(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);

	return 0;
}

u32 Ssi_dma_rx_irq(void *Data)
{
	SPI_OBJ *spi_obj = (SPI_OBJ *) Data;
	u32 Length = spi_obj->RxLength;
	u32 *pRxData = spi_obj->RxData;
	PGDMA_InitTypeDef GDMA_InitStruct;

	GDMA_InitStruct = &spi_obj->SSIRxGdmaInitStruct;

	/* Clear Pending ISR */
	GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	DCache_Invalidate((u32) pRxData, Length);

	/*  Call user RX complete callback */
	if (spi_obj->Index == 0) {
		SlaveRxDone = 1;
	} else {
		MasterRxDone = 1;
	}

	/* Set SSI DMA Disable */
	SSI_SetDmaEnable(spi_obj->spi_dev, DISABLE, SPI_BIT_RDMAE);

	GDMA_ChnlFree(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);

	return 0;
}

static void spi_board_init(void)
{
	RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);

	/* SPI1 is as Master */
	Pinmux_Config(SPI1_MOSI, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI1_MISO, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI1_SCLK, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI1_CS, PINMUX_FUNCTION_SPI);
	PAD_PullCtrl((u32)SPI1_CS, GPIO_PuPd_UP);

	/* SPI0 is as Slave */
	Pinmux_Config(SPI0_MOSI, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI0_MISO, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI0_SCLK, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI0_CS, PINMUX_FUNCTION_SPI);
	PAD_PullCtrl(SPI0_CS, GPIO_PuPd_UP);
}

/* Configure SPI1 as mater, SPI0 as slave */
static void spi_driver_init(void)
{
	SSI_InitTypeDef SSI_InitStructM;
	SSI_InitTypeDef SSI_InitStructS;

	/* init SPI1 as master */
	SSI_SetRole(SPI1_DEV, SSI_MASTER);
	SSI_StructInit(&SSI_InitStructM);
	SSI_InitStructM.SPI_Role = SSI_MASTER;
	SSI_InitStructM.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
	SSI_InitStructM.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
	SSI_InitStructM.SPI_DataFrameNumber = DFS_8_BITS;
	SSI_InitStructM.SPI_ClockDivider = 10; // Fssi_clk is 100MHz
	SSI_Init(SPI1_DEV, &SSI_InitStructM);

	/* init SPI0 as slave*/
	SSI_SetRole(SPI0_DEV, SSI_SLAVE);
	SSI_StructInit(&SSI_InitStructS);
	SSI_InitStructS.SPI_Role = SSI_SLAVE;
	SSI_InitStructS.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
	SSI_InitStructS.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
	SSI_InitStructS.SPI_DataFrameNumber = DFS_8_BITS;
	SSI_Init(SPI0_DEV, &SSI_InitStructS);

	if (SSI_InitStructS.SPI_SclkPolarity == SCPOL_INACTIVE_IS_LOW) {
		PAD_PullCtrl((u32)SPI0_SCLK, GPIO_PuPd_DOWN);
	} else {
		PAD_PullCtrl((u32)SPI0_SCLK, GPIO_PuPd_UP);
	}

}

bool Spi_data_compare(u8 *pSrc, u8 *pDst, u32 Length)
{
	u32 Index;
	u8 res = TRUE;

	printf("\nres - idx - src data - dst data\n\n");

	for (Index = 0; Index < Length; ++Index) {
		if ((pSrc[Index] & dfs_mask) != pDst[Index]) {
			printf("[ERR]   %lu:\t0x%x ----- 0x%x\n", Index, pSrc[Index]&dfs_mask, pDst[Index]);
			res = FALSE;
		} else {
			printf("[OK]    %lu:\t0x%x ----- 0x%x\n", Index, pSrc[Index]&dfs_mask, pDst[Index]);
		}
	}

	return res;
}

void Spi_flush_rx_fifo(SPI_OBJ *spi_obj)
{
	u32 rx_fifo_level;
	u32 j;

	while (SSI_Readable(spi_obj->spi_dev)) {
		rx_fifo_level = SSI_GetRxCount(spi_obj->spi_dev);
		for (j = 0; j < rx_fifo_level; j++) {
			SSI_ReadData(spi_obj->spi_dev);
		}
	}
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

void Spi_slave_read_stream_dma(SPI_OBJ *spi_obj, u8 *rx_buffer, u32 length)
{
	assert_param(length != 0);
	assert_param(rx_buffer != NULL);

	spi_obj->RxLength = length;
	spi_obj->RxData = (void *)rx_buffer;

	SSI_RXGDMA_Init(spi_obj->Index, &spi_obj->SSIRxGdmaInitStruct, (void *)spi_obj, (IRQ_FUN) Ssi_dma_rx_irq, rx_buffer, length);

	/* Enable SPI Rx DMA */
	SSI_SetDmaEnable(spi_obj->spi_dev, ENABLE, SPI_BIT_RDMAE);
}

void Spi_slave_write_stream_dma(SPI_OBJ *spi_obj, u8 *tx_buffer, u32 length)
{
	assert_param(length != 0);
	assert_param(tx_buffer != NULL);

	spi_obj->TxLength = length;
	spi_obj->TxData = (void *)tx_buffer;

	SSI_TXGDMA_Init(spi_obj->Index, &spi_obj->SSITxGdmaInitStruct, (void *)spi_obj, (IRQ_FUN) Ssi_dma_tx_irq, tx_buffer, length);

	/* Enable SPI Tx DMA */
	SSI_SetDmaEnable(spi_obj->spi_dev, ENABLE, SPI_BIT_TDMAE);
}

void Spi_master_read_stream_dma(SPI_OBJ *spi_obj, u8 *rx_buffer, u32 length)
{
	assert_param(length != 0);
	assert_param(rx_buffer != NULL);

	spi_obj->RxLength = length;
	spi_obj->RxData = (void *)rx_buffer;

	SSI_RXGDMA_Init(spi_obj->Index, &spi_obj->SSIRxGdmaInitStruct, (void *)spi_obj, (IRQ_FUN) Ssi_dma_rx_irq, rx_buffer, length);
	/* Enable SPI Rx DMA */
	SSI_SetDmaEnable(spi_obj->spi_dev, ENABLE, SPI_BIT_RDMAE);

	/* It is necessary to push some data to master Tx FIFO to generate clock signal,
	or the slave cannot transmit data to master */
	spi_obj->TxLength = length;
	spi_obj->TxData = (void *)SlaveTxBuf;

	SSI_SetDmaEnable(spi_obj->spi_dev, ENABLE, SPI_BIT_TDMAE);
	/* Enable SPI Tx DMA */
	SSI_TXGDMA_Init(spi_obj->Index, &spi_obj->SSITxGdmaInitStruct, (void *)spi_obj, (IRQ_FUN) Ssi_dma_tx_irq, SlaveTxBuf, length);
}

void Spi_master_write_read_stream_dma(SPI_OBJ *spi_obj, u8 *tx_buffer, u8 *rx_buffer, u32 length)
{
	assert_param(length != 0);
	assert_param(tx_buffer != NULL);
	assert_param(rx_buffer != NULL);

	spi_obj->RxLength = length;
	spi_obj->RxData = (void *)rx_buffer;
	spi_obj->TxLength = length;
	spi_obj->TxData = (void *)tx_buffer;

	SSI_RXGDMA_Init(spi_obj->Index, &spi_obj->SSIRxGdmaInitStruct, (void *)spi_obj, (IRQ_FUN) Ssi_dma_rx_irq, rx_buffer, length);
	/* Enable SPI Rx DMA */
	SSI_SetDmaEnable(spi_obj->spi_dev, ENABLE, SPI_BIT_RDMAE);

	SSI_TXGDMA_Init(spi_obj->Index, &spi_obj->SSITxGdmaInitStruct, (void *)spi_obj, (IRQ_FUN) Ssi_dma_tx_irq, tx_buffer, length);
	/* Enable SPI Tx DMA */
	SSI_SetDmaEnable(spi_obj->spi_dev, ENABLE, SPI_BIT_TDMAE);
}

void spi_dma_task(void)
{
	int result = TRUE;
	int i = 0;

	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	/* SPI1 as Master and SPI0 as salve*/
	spi_board_init();
	spi_driver_init();

	/* SPI1 as Master */
	spi_master.Index = 0x1;
	spi_master.spi_dev = SPI_DEV_TABLE[spi_master.Index].SPIx;

	/* SPI0 as Slave */
	spi_slave.Index = 0x0;
	spi_slave.spi_dev = SPI_DEV_TABLE[spi_slave.Index].SPIx;

	_memset(MasterTxBuf, 0, TEST_BUF_SIZE);
	_memset(MasterRxBuf, 0, TEST_BUF_SIZE);
	_memset(SlaveTxBuf, 0, TEST_BUF_SIZE);
	_memset(SlaveRxBuf, 0, TEST_BUF_SIZE);

	for (i = 0; i < TEST_BUF_SIZE; i++) {
		*((u8 *)MasterTxBuf + i) = i;
		*((u8 *)SlaveTxBuf + i) = i;
	}

	/**
	 * Master write/read, Slave read/write
	 */
	printf("\n---Master write/read, Slave read/write---\n");

	MasterRxDone = 0;
	SlaveRxDone = 0;
	MasterTxDone = 0;
	MasterTxDone = 0;

	Spi_slave_read_stream_dma(&spi_slave, SlaveRxBuf, TEST_BUF_SIZE);
	Spi_slave_write_stream_dma(&spi_slave, SlaveTxBuf, TEST_BUF_SIZE);
	Spi_master_write_read_stream_dma(&spi_master, MasterTxBuf, MasterRxBuf, TEST_BUF_SIZE);

	i = 0;
	while ((MasterRxDone && SlaveRxDone) == 0) {
		DelayMs(100);
		if (i++ > 150) {
			printf("SPI Wait Timeout\r\n");
			break;
		}
	}

	printf("\n---Master tx, Slave rx---\n");
	result = Spi_data_compare(MasterTxBuf, SlaveRxBuf, TEST_BUF_SIZE);
	if (result == FALSE) {
		goto TEST_RESULT;
	}

	printf("\n---Master rx, Slave tx---\n");
	result = Spi_data_compare(SlaveTxBuf, MasterRxBuf, TEST_BUF_SIZE);
	if (result == FALSE) {
		goto TEST_RESULT;
	}

	/**
	 * Master read, Slave write
	 */
	printf("\n---Master read, Slave write---\n");

	MasterRxDone = 0;
	MasterTxDone = 0;

	/* flush rx fifo */
	Spi_flush_rx_fifo(&spi_master);

	Spi_slave_write_stream_dma(&spi_slave, SlaveTxBuf, TEST_BUF_SIZE);
	Spi_master_read_stream_dma(&spi_master, MasterRxBuf, TEST_BUF_SIZE);

	i = 0;
	while (MasterRxDone == 0) {
		DelayMs(100);
		i++;
		if (i > 150) {
			printf("SPI Master Wait Timeout\r\n");
			break;
		}
	}

	result = Spi_data_compare(SlaveTxBuf, MasterRxBuf, TEST_BUF_SIZE);
	if (result == FALSE) {
		goto TEST_RESULT;
	}

TEST_RESULT:
	printf("\r\nTest result is %s.\r\n", (result) ? "ok" : "error");

	Spi_free(&spi_master);
	Spi_free(&spi_slave);

	printf("SPI Demo finished.\n\n");

	rtos_task_delete(NULL);

}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_dma_task"), (rtos_task_t)spi_dma_task, NULL, 1024 * 4, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(spi_dma_task) failed", __FUNCTION__);
	}

	rtos_sched_start();
	return 0;
}

