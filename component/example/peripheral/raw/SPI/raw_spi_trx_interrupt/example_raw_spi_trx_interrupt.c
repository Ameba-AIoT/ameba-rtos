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
#define PINMUX_FUNCTION_SPIS	PINMUX_FUNCTION_SPI
#endif

#define DataFrameSize	8
#define dfs_mask		0xFF
#define ClockDivider	100
#define TEST_BUF_SIZE	512

SRAM_NOCACHE_DATA_SECTION u8 MasterTxBuf[TEST_BUF_SIZE];
SRAM_NOCACHE_DATA_SECTION u8 MasterRxBuf[TEST_BUF_SIZE];
SRAM_NOCACHE_DATA_SECTION u8 SlaveTxBuf[TEST_BUF_SIZE];
SRAM_NOCACHE_DATA_SECTION u8 SlaveRxBuf[TEST_BUF_SIZE];

volatile int MasterTxDone;
volatile int MasterRxDone;
volatile int SlaveTxDone;
volatile int SlaveRxDone;

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
SPI_OBJ spi_slave;

static u32  ssi_interrupt(void *Adaptor)
{
	SPI_OBJ *spi_obj = (SPI_OBJ *) Adaptor;
	u32 InterruptStatus = SSI_GetIsr(spi_obj->spi_dev);
	u32 TransLen = 0;

	SSI_SetIsrClean(spi_obj->spi_dev, InterruptStatus);

	if (InterruptStatus & (SPI_BIT_TXOIS | SPI_BIT_RXUIS | SPI_BIT_RXOIS | SPI_BIT_TXUIS)) {
		//printf("[INT] Tx/Rx Warning %x \n", InterruptStatus);
	}

	if ((InterruptStatus & SPI_BIT_RXFIS)) {

		TransLen = SSI_ReceiveData(spi_obj->spi_dev, spi_obj->RxData, spi_obj->RxLength);
		spi_obj->RxLength -= TransLen;
		if (DataFrameSize > 8) {
			// 16~9 bits mode
			spi_obj->RxData = (void *)(((u16 *)spi_obj->RxData) + TransLen);
		} else {
			// 8~4 bits mode
			spi_obj->RxData = (void *)(((u8 *)spi_obj->RxData) + TransLen);
		}

		if (spi_obj->RxLength == 0) {
			SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_RXFIM | SPI_BIT_RXOIM | SPI_BIT_RXUIM), DISABLE);
			if (spi_obj->Index == 0) {
				SlaveRxDone = 1;
			} else {
				MasterRxDone = 1;
			}
		}
	}

	if (InterruptStatus & SPI_BIT_TXEIS) {

		TransLen = SSI_SendData(spi_obj->spi_dev, spi_obj->TxData, spi_obj->TxLength, spi_obj->Role);

		spi_obj->TxLength -= TransLen;
		if (spi_obj->TxData != NULL) {
			if (DataFrameSize > 8) {
				// 16~9 bits mode
				spi_obj->TxData = (void *)(((u16 *)spi_obj->TxData) + TransLen);
			} else {
				// 8~4 bits mode
				spi_obj->TxData = (void *)(((u8 *)spi_obj->TxData) + TransLen);
			}
		}
		/* all data write into fifo */
		if (spi_obj->TxLength == 0) {
			SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_TXOIM | SPI_BIT_TXEIM), DISABLE);
			// If it's not a dummy TX for master read SPI, then call the TX_done callback
			if (spi_obj->TxData != NULL) {
				if (spi_obj->Index == 0) {
					SlaveTxDone = 1;
				} else {
					MasterTxDone = 1;
				}
			}
		}
	}

	return 0;
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

void Spi_free(SPI_OBJ *spi_obj)
{
	InterruptDis(spi_obj->IrqNum);
	InterruptUnRegister(spi_obj->IrqNum);
	SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_RXFIM | SPI_BIT_RXOIM | SPI_BIT_RXUIM), DISABLE);
	SSI_Cmd(spi_obj->spi_dev, DISABLE);
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

void Spi_slave_write_stream(SPI_OBJ *spi_obj, u8 *tx_buffer, u32 length)
{
	assert_param(length != 0);

	if (DataFrameSize > 8) {
		/*  16~9 bits mode */
		spi_obj->TxLength = length >> 1; // 2 bytes(16 bit) every transfer
	} else {
		/*  8~4 bits mode */
		spi_obj->TxLength = length; // 1 byte(8 bit) every transfer
	}
	spi_obj->TxData = (void *)tx_buffer;
	SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_TXOIM | SPI_BIT_TXEIM), ENABLE);
}

void Spi_slave_read_stream(SPI_OBJ *spi_obj, u8 *rx_buffer, u32 length)
{
	assert_param(length != 0);

	/*  As a Slave mode, if the peer(Master) side is power off, the BUSY flag is always on */
	while (SSI_Busy(spi_obj->spi_dev));

	if (DataFrameSize > 8) {
		/*  16~9 bits mode */
		spi_obj->RxLength = length >> 1; // 2 bytes(16 bit) every transfer
	} else {
		/*  8~4 bits mode */
		spi_obj->RxLength = length; // 1 byte(8 bit) every transfer
	}
	spi_obj->RxData = (void *)rx_buffer;
	SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_RXFIM | SPI_BIT_RXOIM | SPI_BIT_RXUIM), ENABLE);
}

void Spi_master_write_read_stream(SPI_OBJ *spi_obj, u8 *tx_buffer, u8 *rx_buffer, u32 length)
{
	assert_param(length != 0);

	// wait bus idle
	while (SSI_Busy(spi_obj->spi_dev));

	if (DataFrameSize > 8) {
		/*  16~9 bits mode */
		spi_obj->RxLength = length >> 1; // 2 bytes(16 bit) every transfer
		spi_obj->TxLength = length >> 1; // 2 bytes(16 bit) every transfer
	} else {
		/*  8~4 bits mode */
		spi_obj->RxLength = length; // 1 byte(8 bit) every transfer
		spi_obj->TxLength = length; // 1 byte(8 bit) every transfer
	}
	spi_obj->RxData = rx_buffer;
	spi_obj->TxData = (void *)tx_buffer;
	SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_RXFIM | SPI_BIT_RXOIM | SPI_BIT_RXUIM | SPI_BIT_TXOIM | SPI_BIT_TXEIM), ENABLE);
}

void Spi_master_read_stream(SPI_OBJ *spi_obj, u8 *rx_buffer, u32 length)
{
	assert_param(length != 0);

	// wait bus idle
	while (SSI_Busy(spi_obj->spi_dev));

	if (DataFrameSize > 8) {
		/*  16~9 bits mode */
		spi_obj->RxLength = length >> 1; // 2 bytes(16 bit) every transfer
	} else {
		/*  8~4 bits mode */
		spi_obj->RxLength = length; // 1 byte(8 bit) every transfer
	}
	spi_obj->RxData = rx_buffer;
	SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_RXFIM | SPI_BIT_RXOIM | SPI_BIT_RXUIM), ENABLE);

	/* It is necessary to push data to master Tx FIFO to generate clock signal,
	or the slave cannot transmit data to master */

	if (DataFrameSize > 8) {
		/*  16~9 bits mode */
		spi_obj->TxLength = length >> 1; // 2 bytes(16 bit) every transfer
	} else {
		/*  8~4 bits mode */
		spi_obj->TxLength = length; // 1 byte(8 bit) every transfer
	}
	spi_obj->TxData = (void *)NULL;
	SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_TXOIM | SPI_BIT_TXEIM), ENABLE);
}

void Spi_interrupt_task(void)
{
	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	u32 SclkPhase = SCPH_TOGGLES_IN_MIDDLE; // SCPH_TOGGLES_IN_MIDDLE or SCPH_TOGGLES_AT_START
	u32 SclkPolarity = SCPOL_INACTIVE_IS_LOW; // SCPOL_INACTIVE_IS_LOW or SCPOL_INACTIVE_IS_HIGH
	int result = TRUE;
	int i = 0;

	/* SPI1 as Master */
	spi_master.Index = 0x1;
	spi_master.Role = SSI_MASTER;
	spi_master.spi_dev = SPI_DEV_TABLE[spi_master.Index].SPIx;
	spi_master.IrqNum = SPI_DEV_TABLE[spi_master.Index].IrqNum;

	/* init SPI1 */
	SSI_InitTypeDef SSI_InitStructM;
	SSI_StructInit(&SSI_InitStructM);
	RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, ENABLE);
	Pinmux_Config(SPI1_MOSI, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI1_MISO, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI1_SCLK, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI1_CS, PINMUX_FUNCTION_SPIM);
	PAD_PullCtrl((u32)SPI1_CS, GPIO_PuPd_UP);

	SSI_SetRole(spi_master.spi_dev, SSI_MASTER);
	SSI_InitStructM.SPI_Role = SSI_MASTER;
	InterruptRegister((IRQ_FUN)ssi_interrupt, spi_master.IrqNum, (u32)&spi_master, 6);
	InterruptEn(spi_master.IrqNum, 6);
	SSI_Init(spi_master.spi_dev, &SSI_InitStructM);

	/* set format */
	SSI_SetSclkPhase(spi_master.spi_dev, SclkPhase);
	SSI_SetSclkPolarity(spi_master.spi_dev, SclkPolarity);
	SSI_SetDataFrameSize(spi_master.spi_dev, DataFrameSize - 1);

	/* set frequency */
	SSI_SetBaudDiv(spi_master.spi_dev, ClockDivider); // Fspi_clk is 100MHz

	/* SPI0 as Slave */
	spi_slave.Index = 0x0;
	spi_slave.Role = SSI_SLAVE;
	spi_slave.spi_dev = SPI_DEV_TABLE[spi_slave.Index].SPIx;
	spi_slave.IrqNum = SPI_DEV_TABLE[spi_slave.Index].IrqNum;

	/* init SPI0 */
	SSI_InitTypeDef SSI_InitStructS;
	SSI_StructInit(&SSI_InitStructS);

	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	Pinmux_Config(SPI0_MOSI, PINMUX_FUNCTION_SPIS);
	Pinmux_Config(SPI0_MISO, PINMUX_FUNCTION_SPIS);
	Pinmux_Config(SPI0_SCLK, PINMUX_FUNCTION_SPIS);
	Pinmux_Config(SPI0_CS, PINMUX_FUNCTION_SPIS);
	PAD_PullCtrl((u32)SPI0_CS, GPIO_PuPd_UP);

	SSI_SetRole(spi_slave.spi_dev, SSI_SLAVE);
	SSI_InitStructS.SPI_Role = SSI_SLAVE;

	InterruptRegister((IRQ_FUN)ssi_interrupt, spi_slave.IrqNum, (u32)&spi_slave, 6);
	InterruptEn(spi_slave.IrqNum, 6);

	SSI_Init(spi_slave.spi_dev, &SSI_InitStructS);

	/* set format */
	SSI_SetSclkPhase(spi_slave.spi_dev, SclkPhase);
	SSI_SetSclkPolarity(spi_slave.spi_dev, SclkPolarity);
	SSI_SetDataFrameSize(spi_slave.spi_dev, DataFrameSize - 1);

	if (SclkPolarity == SCPOL_INACTIVE_IS_LOW) {
		PAD_PullCtrl((u32)SPI0_SCLK, GPIO_PuPd_DOWN);
	} else {
		PAD_PullCtrl((u32)SPI0_SCLK, GPIO_PuPd_UP);
	}

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

	Spi_slave_read_stream(&spi_slave, SlaveRxBuf, TEST_BUF_SIZE);
	Spi_slave_write_stream(&spi_slave, SlaveTxBuf, TEST_BUF_SIZE);
	Spi_master_write_read_stream(&spi_master, MasterTxBuf, MasterRxBuf, TEST_BUF_SIZE);

	i = 0;
	while ((MasterRxDone && SlaveRxDone) == 0) {
		DelayMs(100);
		i++;
		if (i > 150) {
			printf("SPI Slave Wait Timeout\r\n");
			break;
		}
	}

	result = Spi_data_compare(MasterTxBuf, SlaveRxBuf, TEST_BUF_SIZE);
	if (result == FALSE) {
		goto DATA_ERROR;
	}

	result = Spi_data_compare(SlaveTxBuf, MasterRxBuf, TEST_BUF_SIZE);
	if (result == FALSE) {
		goto DATA_ERROR;
	}

	/**
	 * Master read, Slave write
	 */
	printf("\n---Master read, Slave write---\n");

	MasterRxDone = 0;

	/* flush rx fifo */
	Spi_flush_rx_fifo(&spi_master);

	Spi_slave_write_stream(&spi_slave, SlaveTxBuf, TEST_BUF_SIZE);
	Spi_master_read_stream(&spi_master, MasterRxBuf, TEST_BUF_SIZE);

	i = 0;
	while (MasterRxDone == 0) {
		DelayMs(100);
		if (i++ > 150) {
			printf("SPI Master Wait Timeout\r\n");
			break;
		}
	}

	result = Spi_data_compare(SlaveTxBuf, MasterRxBuf, TEST_BUF_SIZE);
	if (result == FALSE) {
		goto DATA_ERROR;
	}

DATA_ERROR:
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
int example_raw_spi_trx_interrupt(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_interrupt_task"), (rtos_task_t)Spi_interrupt_task, NULL, 1024 * 4, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(spi_interrupt_task) failed", __FUNCTION__);
	}

	// rtos_sched_start();
	return 0;
}

