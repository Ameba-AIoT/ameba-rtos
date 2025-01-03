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
#include "device.h"
#include "diag.h"
#include "main.h"
#include "spi_api.h"
#include "spi_ex_api.h"
#include "wait_api.h"
#include "os_wrapper.h"
#include <stdio.h>

#define DataFrameSize	8
#define dfs_mask		0xFF
#define Mode			0
#define SCLK_FREQ		100000
#define TEST_BUF_SIZE	512 //for dma mode, buffer size should be multiple of CACHE_LINE_SIZE 

/* for dma mode, start address of buffer should be CACHE_LINE_SIZE  aligned*/
u16 MasterTxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
u16 MasterRxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
u16 SlaveTxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
u16 SlaveRxBuf[TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

volatile int MasterTxDone;
volatile int MasterRxDone;
volatile int SlaveTxDone;
volatile int SlaveRxDone;

spi_t spi_master;
spi_t spi_slave;

void Master_tr_done_callback(uint32_t pdata, SpiIrq event)
{
	(void)pdata;

	switch (event) {
	case SpiRxIrq:
		MasterRxDone = 1;
		break;
	case SpiTxIrq:
		MasterTxDone = 1;
		break;
	default:
		printf("unknown interrput event!\n");
	}
}

void Slave_tr_done_callback(uint32_t pdata, SpiIrq event)
{
	(void)pdata;

	switch (event) {
	case SpiRxIrq:
		SlaveRxDone = 1;
		break;
	case SpiTxIrq:
		SlaveTxDone = 1;
		break;
	default:
		printf("unknown interrput event!\n");
	}
}

bool Ssi_DataCompare(u16 *pSrc, u16 *pDst, u32 Length)
{
	u32 Index;
	u8 *PSrc_8 = (u8 *)pSrc;
	u8 *PDst_8 = (u8 *)pDst;
	u8 res = TRUE;

	printf("\nres - idx - src data - dst data\n\n");

	if (DataFrameSize > 8) {
		for (Index = 0; Index < Length; ++Index) {
			if ((pSrc[Index] & dfs_mask) != pDst[Index]) {
				printf("[ERR]   %lu:\t0x%x ----- 0x%x\n", Index, pSrc[Index]&dfs_mask, pDst[Index]);
				res = FALSE;
			} else {
				printf("[OK]    %lu:\t0x%x ----- 0x%x\n", Index, pSrc[Index]&dfs_mask, pDst[Index]);
			}
		}
	} else {
		for (Index = 0; Index < Length; ++Index) {
			if ((PSrc_8[Index] & dfs_mask) != PDst_8[Index]) {
				printf("[ERR]   %lu:\t0x%x ----- 0x%x\n", Index, PSrc_8[Index]&dfs_mask, PDst_8[Index]);
				res = FALSE;
			} else {
				printf("[OK]    %lu:\t0x%x ----- 0x%x\n", Index, PSrc_8[Index]&dfs_mask, PDst_8[Index]);
			}
		}
	}
	return res;
}

void spi_dma_task(void)
{
	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	/* SPI1 is as Master */
	spi_master.spi_idx = MBED_SPI1;
	spi_init(&spi_master, SPI1_MOSI, SPI1_MISO, SPI1_SCLK, SPI1_CS);
	spi_format(&spi_master, DataFrameSize, Mode, 0);
	spi_frequency(&spi_master, SCLK_FREQ);

	/* SPI0 is as Slave */
	spi_slave.spi_idx = MBED_SPI0;
	spi_init(&spi_slave,  SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CS);
	spi_format(&spi_slave, DataFrameSize, Mode, 1);

	int i = 0;
	int result1 = 1;
	int result2 = 1;
	int result3 = 1;
	u32 trans_bytes = (DataFrameSize > 8) ? (TEST_BUF_SIZE * 2) : TEST_BUF_SIZE;

	_memset(MasterTxBuf, 0, TEST_BUF_SIZE);
	_memset(MasterRxBuf, 0, TEST_BUF_SIZE);
	_memset(SlaveTxBuf, 0, TEST_BUF_SIZE);
	_memset(SlaveRxBuf, 0, TEST_BUF_SIZE);

	for (i = 0; i < TEST_BUF_SIZE; i++) {
		if (DataFrameSize > 8) {
			*((u16 *)MasterTxBuf + i) = i;
		} else {
			*((u8 *)MasterTxBuf + i) = i;
		}
	}

	for (i = 0; i < TEST_BUF_SIZE; i++) {
		if (DataFrameSize > 8) {
			*((u16 *)SlaveTxBuf + i) = i;
		} else {
			*((u8 *)SlaveTxBuf + i) = i;
		}
	}

	/**
	* Master write/read, Slave read/write
	*/
	printf("\n---Master write/read, Slave read/write---\n");

	spi_irq_hook(&spi_master, (spi_irq_handler) Master_tr_done_callback, (uint32_t)&spi_master);
	spi_irq_hook(&spi_slave, (spi_irq_handler) Slave_tr_done_callback, (uint32_t)&spi_slave);

	MasterRxDone = 0;
	SlaveRxDone = 0;

	spi_slave_read_stream_dma(&spi_slave, (char *)SlaveRxBuf, trans_bytes);
	spi_slave_write_stream_dma(&spi_slave, (char *)SlaveTxBuf, trans_bytes);
	spi_master_write_read_stream_dma(&spi_master, (char *) MasterTxBuf, (char *)MasterRxBuf, trans_bytes);

	i = 0;
	while ((MasterRxDone && SlaveRxDone) == 0) {
		wait_ms(100);
		i++;
		if (i > 150) {
			printf("SPI Wait Timeout\r\n");
			break;
		}
	}

	result1 = Ssi_DataCompare(MasterTxBuf, SlaveRxBuf, TEST_BUF_SIZE);
	result2 = Ssi_DataCompare(SlaveTxBuf, MasterRxBuf, TEST_BUF_SIZE);

	/**
	* Master read, Slave write
	*/
	printf("\n---Master read, Slave write---\n");

	spi_irq_hook(&spi_master, (spi_irq_handler) Master_tr_done_callback, (uint32_t)&spi_master);

	MasterRxDone = 0;

	spi_flush_rx_fifo(&spi_master);

	spi_slave_write_stream_dma(&spi_slave, (char *)SlaveTxBuf, trans_bytes);
	spi_master_read_stream_dma(&spi_master, (char *)MasterRxBuf, trans_bytes);

	i = 0;
	while (MasterRxDone == 0) {
		wait_ms(100);
		i++;
		if (i > 150) {
			printf("SPI Master Wait Timeout\r\n");
			break;
		}
	}

	result3 = Ssi_DataCompare(SlaveTxBuf, MasterRxBuf, TEST_BUF_SIZE);

	printf("\r\nTest result is %s.\r\n", (result1 && result2 && result3) ? "ok" : "error");

	spi_free(&spi_master);
	spi_free(&spi_slave);

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


