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
#define TAG "SPI_SLAVE"

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

u8 SlaveTxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
u8 SlaveRxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

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

SPI_OBJ spi_slave;

u32 Ssi_dma_tx_irq(void *Data)
{
	SPI_OBJ *spi_obj = (SPI_OBJ *) Data;
	PGDMA_InitTypeDef GDMA_InitStruct;

	GDMA_InitStruct = &spi_obj->SSITxGdmaInitStruct;

	GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	SSI_SlaveErrRecovery(spi_obj->spi_dev);

	SlaveTxDone = 1;

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

	SlaveRxDone = 1;

	SSI_SetDmaEnable(spi_obj->spi_dev, DISABLE, SPI_BIT_RDMAE);
	GDMA_ChnlFree(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);

	return 0;
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

void Spi_slave_read_stream_dma(SPI_OBJ *spi_obj, u8 *rx_buffer, u32 length)
{
	assert_param(length != 0);
	assert_param(rx_buffer != NULL);

	spi_obj->RxLength = length;
	spi_obj->RxData = (void *)rx_buffer;

	SSI_RXGDMA_Init(spi_obj->Index, &spi_obj->SSIRxGdmaInitStruct, (void *)spi_obj,
					(IRQ_FUN) Ssi_dma_rx_irq, rx_buffer, length);
	SSI_SetDmaEnable(spi_obj->spi_dev, ENABLE, SPI_BIT_RDMAE);
}

void Spi_slave_write_stream_dma(SPI_OBJ *spi_obj, u8 *tx_buffer, u32 length)
{
	assert_param(length != 0);
	assert_param(tx_buffer != NULL);

	spi_obj->TxLength = length;
	spi_obj->TxData = (void *)tx_buffer;

	SSI_TXGDMA_Init(spi_obj->Index, &spi_obj->SSITxGdmaInitStruct, (void *)spi_obj,
					(IRQ_FUN) Ssi_dma_tx_irq, tx_buffer, length);
	SSI_SetDmaEnable(spi_obj->spi_dev, ENABLE, SPI_BIT_TDMAE);
}

void spi_dma_task(void)
{
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	int result = TRUE;
	int i = 0;

	/* init SPI0 as slave */
	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	Pinmux_Config(SPI_MOSI, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_MISO, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_SCLK, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_CS, PINMUX_FUNCTION_SPIM);
	PAD_PullCtrl((u32)SPI_CS, GPIO_PuPd_UP);
	PAD_PullCtrl((u32)SPI_SCLK, GPIO_PuPd_DOWN);

	spi_slave.Index = 0x0;
	spi_slave.spi_dev = SPI_DEV_TABLE[spi_slave.Index].SPIx;

	SSI_InitTypeDef SSI_InitStructS;
	SSI_SetRole(spi_slave.spi_dev, SSI_SLAVE);
	SSI_StructInit(&SSI_InitStructS);
	SSI_InitStructS.SPI_Role = SSI_SLAVE;
	SSI_InitStructS.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
	SSI_InitStructS.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
	SSI_InitStructS.SPI_DataFrameNumber = DFS_8_BITS;
	SSI_InitStructS.SPI_ClockDivider = ClockDivider;
	SSI_Init(spi_slave.spi_dev, &SSI_InitStructS);

	_memset(SlaveTxBuf, 0, TEST_BUF_SIZE);
	_memset(SlaveRxBuf, 0, TEST_BUF_SIZE);
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		*((u8 *)SlaveTxBuf + i) = i;
	}

	/**
	 * Phase 1: Slave read/write (full duplex)
	 */
	RTK_LOGI(TAG, "---Phase1: Slave read/write---\n");

	SlaveRxDone = 0;
	SlaveTxDone = 0;

	Spi_slave_read_stream_dma(&spi_slave, SlaveRxBuf, TEST_BUF_SIZE);
	Spi_slave_write_stream_dma(&spi_slave, SlaveTxBuf, TEST_BUF_SIZE);

	i = 0;
	while ((SlaveTxDone && SlaveRxDone) == 0) {
		DelayMs(100);
		if (i++ > 150) {
			RTK_LOGE(TAG, "SPI Timeout\n");
			break;
		}
	}

	result = TRUE;
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		if ((SlaveTxBuf[i] & dfs_mask) != SlaveRxBuf[i]) {
			result = FALSE;
			break;
		}
	}
	if (result) {
		RTK_LOGI(TAG, "Phase1: success\n");
	} else {
		RTK_LOGE(TAG, "Phase1: fail\n");
	}

	/**
	 * Phase 2: Slave write (Master read) - unidirectional
	 */
	RTK_LOGI(TAG, "---Phase2: Slave write---\n");

	SlaveTxDone = 0;

	Spi_slave_write_stream_dma(&spi_slave, SlaveTxBuf, TEST_BUF_SIZE);

	i = 0;
	while (SlaveTxDone == 0) {
		DelayMs(100);
		i++;
		if (i > 150) {
			RTK_LOGE(TAG, "SPI Timeout\n");
			break;
		}
	}

	RTK_LOGI(TAG, "Phase2: success\n");

	Spi_free(&spi_slave);

	if (result) {
		RTK_LOGI(TAG, "SPI Demo: success\n");
	} else {
		RTK_LOGE(TAG, "SPI Demo: fail\n");
	}

	rtos_task_delete(NULL);
}

int example_spi_trx_dma_slave(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_dma_task"), (rtos_task_t)spi_dma_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "%s Create spi_dma_task task failed", __FUNCTION__);
	}
	return 0;
}
