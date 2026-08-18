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
#define TAG "SPI_MASTER"

/* compatible pinmux_funcid_name with RTL872xD */
#ifndef CONFIG_AMEBAD
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RTL8720F)
#define PINMUX_FUNCTION_SPIM	PINMUX_FUNCTION_SPI0
#else
#define PINMUX_FUNCTION_SPIM	PINMUX_FUNCTION_SPI
#endif
#endif

#define DataFrameSize	8
#define dfs_mask		0xFF
#define ClockDivider	20
#define TEST_BUF_SIZE	512

u8 MasterTxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
u8 MasterRxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

volatile int MasterTxDone;
volatile int MasterRxDone;

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

u32 Ssi_dma_tx_irq(void *Data)
{
	SPI_OBJ *spi_obj = (SPI_OBJ *) Data;
	PGDMA_InitTypeDef GDMA_InitStruct;

	GDMA_InitStruct = &spi_obj->SSITxGdmaInitStruct;

	GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	SSI_SlaveErrRecovery(spi_obj->spi_dev);

	MasterTxDone = 1;

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

	GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	DCache_Invalidate((u32) pRxData, Length);

	SSI_SlaveErrRecovery(spi_obj->spi_dev);

	MasterRxDone = 1;

	SSI_SetDmaEnable(spi_obj->spi_dev, DISABLE, SPI_BIT_RDMAE);
	GDMA_ChnlFree(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);

	return 0;
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

	SSI_SetDmaEnable(spi_obj->spi_dev, DISABLE, SPI_BIT_RDMAE);

	GDMA_ClearINT(GDMA_Rx->GDMA_Index, GDMA_Rx->GDMA_ChNum);
	GDMA_ChCleanAutoReload(GDMA_Rx->GDMA_Index, GDMA_Rx->GDMA_ChNum, CLEAN_RELOAD_SRC_DST);
	GDMA_Cmd(GDMA_Rx->GDMA_Index, GDMA_Rx->GDMA_ChNum, DISABLE);
	GDMA_ChnlFree(GDMA_Rx->GDMA_Index, GDMA_Rx->GDMA_ChNum);

	PGDMA_InitTypeDef GDMA_Tx = &spi_obj->SSITxGdmaInitStruct;

	SSI_SetDmaEnable(spi_obj->spi_dev, DISABLE, SPI_BIT_TDMAE);

	GDMA_ClearINT(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum);
	GDMA_ChCleanAutoReload(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum, CLEAN_RELOAD_SRC_DST);
	GDMA_Cmd(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum, DISABLE);
	GDMA_ChnlFree(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum);

	SSI_Cmd(spi_obj->spi_dev, DISABLE);
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

	SSI_RXGDMA_Init(spi_obj->Index, &spi_obj->SSIRxGdmaInitStruct, (void *)spi_obj,
					(IRQ_FUN) Ssi_dma_rx_irq, rx_buffer, length);
	SSI_SetDmaEnable(spi_obj->spi_dev, ENABLE, SPI_BIT_RDMAE);

	SSI_TXGDMA_Init(spi_obj->Index, &spi_obj->SSITxGdmaInitStruct, (void *)spi_obj,
					(IRQ_FUN) Ssi_dma_tx_irq, tx_buffer, length);
	SSI_SetDmaEnable(spi_obj->spi_dev, ENABLE, SPI_BIT_TDMAE);
}

void Spi_master_read_stream_dma(SPI_OBJ *spi_obj, u8 *rx_buffer, u32 length)
{
	assert_param(length != 0);
	assert_param(rx_buffer != NULL);

	spi_obj->RxLength = length;
	spi_obj->RxData = (void *)rx_buffer;

	SSI_RXGDMA_Init(spi_obj->Index, &spi_obj->SSIRxGdmaInitStruct, (void *)spi_obj,
					(IRQ_FUN) Ssi_dma_rx_irq, rx_buffer, length);
	SSI_SetDmaEnable(spi_obj->spi_dev, ENABLE, SPI_BIT_RDMAE);

	spi_obj->TxLength = length;
	spi_obj->TxData = (void *)MasterTxBuf;

	SSI_SetDmaEnable(spi_obj->spi_dev, ENABLE, SPI_BIT_TDMAE);
	SSI_TXGDMA_Init(spi_obj->Index, &spi_obj->SSITxGdmaInitStruct, (void *)spi_obj,
					(IRQ_FUN) Ssi_dma_tx_irq, MasterTxBuf, length);
}

void spi_dma_task(void)
{
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	int result = TRUE;
	int i = 0;

	/* init SPI0 as master */
	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	Pinmux_Config(SPI_MOSI, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_MISO, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_SCLK, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_CS, PINMUX_FUNCTION_SPIM);
	PAD_PullCtrl((u32)SPI_CS, GPIO_PuPd_UP);

	spi_master.Index = 0x0;
	spi_master.spi_dev = SPI_DEV_TABLE[spi_master.Index].SPIx;

	SSI_InitTypeDef SSI_InitStructM;
	SSI_SetRole(spi_master.spi_dev, SSI_MASTER);
	SSI_StructInit(&SSI_InitStructM);
	SSI_InitStructM.SPI_Role = SSI_MASTER;
	SSI_InitStructM.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
	SSI_InitStructM.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
	SSI_InitStructM.SPI_DataFrameNumber = DFS_8_BITS;
	SSI_InitStructM.SPI_ClockDivider = ClockDivider;
	SSI_Init(spi_master.spi_dev, &SSI_InitStructM);

	_memset(MasterTxBuf, 0, TEST_BUF_SIZE);
	_memset(MasterRxBuf, 0, TEST_BUF_SIZE);
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		*((u8 *)MasterTxBuf + i) = i;
	}

	/**
	 * Phase 1: Master write/read (full duplex)
	 */
	RTK_LOGI(TAG, "---Phase1: Master write/read---\n");

	MasterRxDone = 0;
	MasterTxDone = 0;

	Spi_master_write_read_stream_dma(&spi_master, MasterTxBuf, MasterRxBuf, TEST_BUF_SIZE);

	i = 0;
	while (MasterRxDone == 0) {
		DelayMs(100);
		if (i++ > 150) {
			RTK_LOGE(TAG, "SPI Timeout\n");
			break;
		}
	}

	int phase1_ok = TRUE;
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		if ((MasterTxBuf[i] & dfs_mask) != MasterRxBuf[i]) {
			phase1_ok = FALSE;
			break;
		}
	}
	if (phase1_ok) {
		RTK_LOGI(TAG, "Phase1: success\n");
	} else {
		RTK_LOGE(TAG, "Phase1: fail\n");
	}

	/**
	 * Phase 2: Master read (Slave write) - unidirectional
	 */
	RTK_LOGI(TAG, "---Phase2: Master read---\n");

	MasterRxDone = 0;
	MasterTxDone = 0;

	Spi_flush_rx_fifo(&spi_master);

	Spi_master_read_stream_dma(&spi_master, MasterRxBuf, TEST_BUF_SIZE);

	i = 0;
	while (MasterRxDone == 0) {
		DelayMs(100);
		i++;
		if (i > 150) {
			RTK_LOGE(TAG, "SPI Timeout\n");
			break;
		}
	}

	int phase2_ok = TRUE;
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		if ((MasterRxBuf[i] & dfs_mask) != (i & 0xFF)) {
			phase2_ok = FALSE;
			break;
		}
	}
	if (phase2_ok) {
		RTK_LOGI(TAG, "Phase2: success\n");
	} else {
		RTK_LOGE(TAG, "Phase2: fail\n");
	}

	Spi_free(&spi_master);

	result = phase1_ok && phase2_ok;
	if (result) {
		RTK_LOGI(TAG, "SPI Demo: success\n");
	} else {
		RTK_LOGE(TAG, "SPI Demo: fail\n");
	}

	rtos_task_delete(NULL);
}

int example_spi_trx_dma_master(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_dma_task"), (rtos_task_t)spi_dma_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "%s Create spi_dma_task task failed", __FUNCTION__);
	}
	return 0;
}
