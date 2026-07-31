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
#define ClockDivider	100
#define TEST_BUF_SIZE	512

SRAM_NOCACHE_DATA_SECTION u8 MasterTxBuf[TEST_BUF_SIZE];
SRAM_NOCACHE_DATA_SECTION u8 MasterRxBuf[TEST_BUF_SIZE];

volatile int MasterTxDone;
volatile int MasterRxDone;

typedef struct {
	IRQn_Type IrqNum;
	SPI_TypeDef *spi_dev;

	void *RxData;
	void *TxData;
	u32 RxLength;
	u32 TxLength;

	u32 Index;
	u32 Role;
} SPI_OBJ;

SPI_OBJ spi_master;

static u32 ssi_interrupt(void *Adaptor)
{
	SPI_OBJ *spi_obj = (SPI_OBJ *) Adaptor;
	u32 InterruptStatus = SSI_GetIsr(spi_obj->spi_dev);
	u32 TransLen = 0;

	SSI_SetIsrClean(spi_obj->spi_dev, InterruptStatus);

	if (InterruptStatus & (SPI_BIT_TXOIS | SPI_BIT_RXUIS | SPI_BIT_RXOIS | SPI_BIT_TXUIS)) {
	}

	if ((InterruptStatus & SPI_BIT_RXFIS)) {
		TransLen = SSI_ReceiveData(spi_obj->spi_dev, spi_obj->RxData, spi_obj->RxLength);
		spi_obj->RxLength -= TransLen;
		if (DataFrameSize > 8) {
			spi_obj->RxData = (void *)(((u16 *)spi_obj->RxData) + TransLen);
		} else {
			spi_obj->RxData = (void *)(((u8 *)spi_obj->RxData) + TransLen);
		}

		if (spi_obj->RxLength == 0) {
			SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_RXFIM | SPI_BIT_RXOIM | SPI_BIT_RXUIM), DISABLE);
			MasterRxDone = 1;
		}
	}

	if (InterruptStatus & SPI_BIT_TXEIS) {
		if (spi_obj->TxLength == 0) {
			SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_TXOIM | SPI_BIT_TXEIM), DISABLE);
		} else {
			TransLen = SSI_SendData(spi_obj->spi_dev, spi_obj->TxData, spi_obj->TxLength, spi_obj->Role);

			spi_obj->TxLength -= TransLen;
			if (spi_obj->TxData != NULL) {
				if (DataFrameSize > 8) {
					spi_obj->TxData = (void *)(((u16 *)spi_obj->TxData) + TransLen);
				} else {
					spi_obj->TxData = (void *)(((u8 *)spi_obj->TxData) + TransLen);
				}
			}
			if (spi_obj->TxLength == 0) {
				SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_TXOIM | SPI_BIT_TXEIM), DISABLE);
				if (spi_obj->TxData != NULL) {
					MasterTxDone = 1;
				}
			}
		}
	}

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
	InterruptDis(spi_obj->IrqNum);
	InterruptUnRegister(spi_obj->IrqNum);
	SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_RXFIM | SPI_BIT_RXOIM | SPI_BIT_RXUIM), DISABLE);
	SSI_Cmd(spi_obj->spi_dev, DISABLE);
}

void Spi_master_write_read_stream(SPI_OBJ *spi_obj, u8 *tx_buffer, u8 *rx_buffer, u32 length)
{
	assert_param(length != 0);

	while (SSI_Busy(spi_obj->spi_dev));

	if (DataFrameSize > 8) {
		spi_obj->RxLength = length >> 1;
		spi_obj->TxLength = length >> 1;
	} else {
		spi_obj->RxLength = length;
		spi_obj->TxLength = length;
	}
	spi_obj->RxData = rx_buffer;
	SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_RXFIM | SPI_BIT_RXOIM | SPI_BIT_RXUIM), ENABLE);

	spi_obj->TxData = (void *)tx_buffer;
	SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_TXOIM | SPI_BIT_TXEIM), ENABLE);
}

void Spi_master_read_stream(SPI_OBJ *spi_obj, u8 *rx_buffer, u32 length)
{
	assert_param(length != 0);

	while (SSI_Busy(spi_obj->spi_dev));

	if (DataFrameSize > 8) {
		spi_obj->RxLength = length >> 1;
	} else {
		spi_obj->RxLength = length;
	}
	spi_obj->RxData = rx_buffer;
	SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_RXFIM | SPI_BIT_RXOIM | SPI_BIT_RXUIM), ENABLE);

	if (DataFrameSize > 8) {
		spi_obj->TxLength = length >> 1;
	} else {
		spi_obj->TxLength = length;
	}
	spi_obj->TxData = (void *)NULL;
	SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_TXOIM | SPI_BIT_TXEIM), ENABLE);
}

void Spi_interrupt_task(void)
{
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	int result = TRUE;
	int i = 0;

	/* SPI0 as Master */
	spi_master.Index = 0x0;
	spi_master.Role = SSI_MASTER;
	spi_master.spi_dev = SPI_DEV_TABLE[spi_master.Index].SPIx;
	spi_master.IrqNum = SPI_DEV_TABLE[spi_master.Index].IrqNum;

	SSI_InitTypeDef SSI_InitStructM;
	SSI_StructInit(&SSI_InitStructM);
	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	Pinmux_Config(SPI_MOSI, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_MISO, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_SCLK, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_CS, PINMUX_FUNCTION_SPIM);
	PAD_PullCtrl((u32)SPI_CS, GPIO_PuPd_UP);

	SSI_SetRole(spi_master.spi_dev, SSI_MASTER);
	SSI_InitStructM.SPI_Role = SSI_MASTER;
	SSI_InitStructM.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
	SSI_InitStructM.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
	SSI_InitStructM.SPI_DataFrameSize = DataFrameSize - 1;
	SSI_InitStructM.SPI_ClockDivider = ClockDivider;
	InterruptRegister((IRQ_FUN)ssi_interrupt, spi_master.IrqNum, (u32)&spi_master, 6);
	InterruptEn(spi_master.IrqNum, 6);
	SSI_Init(spi_master.spi_dev, &SSI_InitStructM);

	SSI_SetSclkPhase(spi_master.spi_dev, SCPH_TOGGLES_IN_MIDDLE);
	SSI_SetSclkPolarity(spi_master.spi_dev, SCPOL_INACTIVE_IS_LOW);
	SSI_SetDataFrameSize(spi_master.spi_dev, DataFrameSize - 1);
	SSI_SetBaudDiv(spi_master.spi_dev, ClockDivider);

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

	Spi_flush_rx_fifo(&spi_master);
	Spi_master_write_read_stream(&spi_master, MasterTxBuf, MasterRxBuf, TEST_BUF_SIZE);

	i = 0;
	while (MasterRxDone == 0) {
		DelayMs(100);
		i++;
		if (i > 150) {
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

	Spi_flush_rx_fifo(&spi_master);

	Spi_master_read_stream(&spi_master, MasterRxBuf, TEST_BUF_SIZE);

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

int example_spi_trx_interrupt_master(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_interrupt_task"), (rtos_task_t)Spi_interrupt_task, NULL, 1024 * 8, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "%s Create spi_interrupt_task task failed", __FUNCTION__);
	}
	return 0;
}
