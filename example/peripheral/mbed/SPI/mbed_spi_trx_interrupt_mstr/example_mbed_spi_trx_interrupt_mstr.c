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
#include "device.h"
#include "main.h"
#include "spi_api.h"
#include "spi_ex_api.h"
#include "wait_api.h"
#include "os_wrapper.h"

#define TAG "SPI_MASTER"

#define DataFrameSize	8
#define dfs_mask		0xFF
#define Mode			0
#define SCLK_FREQ		100000
#define TEST_BUF_SIZE	512

SRAM_NOCACHE_DATA_SECTION u8 MasterTxBuf[TEST_BUF_SIZE];
SRAM_NOCACHE_DATA_SECTION u8 MasterRxBuf[TEST_BUF_SIZE];

volatile int MasterRxDone;

spi_t spi_master;

void Master_tr_done_callback(uint32_t pdata, SpiIrq event)
{
	(void)pdata;

	switch (event) {
	case SpiRxIrq:
		MasterRxDone = 1;
		break;
	case SpiTxIrq:
		break;
	default:
		RTK_LOGE(TAG, "unknown interrupt event!\n");
	}
}

void spi_interrupt_task(void)
{
	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	/* Init SPI0 as Master (dual-board: this board is the master)
	 * Both boards use the same SPI0 pins, connected pin-to-pin.
	 * Master/slave role is distinguished by spi_format slave_mode parameter.
	 */
	spi_master.spi_idx = MBED_SPI0;
	spi_init(&spi_master, SPI_MOSI, SPI_MISO, SPI_SCLK, SPI_CS);
	spi_format(&spi_master, DataFrameSize, Mode, 0);
	spi_frequency(&spi_master, SCLK_FREQ);

	int i;
	int result = 1;
	u32 trans_bytes = TEST_BUF_SIZE;

	_memset(MasterTxBuf, 0, TEST_BUF_SIZE);
	_memset(MasterRxBuf, 0, TEST_BUF_SIZE);
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		*((u8 *)MasterTxBuf + i) = i;
	}

	/**
	 * Phase 1: Full-duplex (Master write/read)
	 */
	RTK_LOGI(TAG, "---Phase1: Master write/read---\n");

	spi_irq_hook(&spi_master, (spi_irq_handler)Master_tr_done_callback, (uint32_t)&spi_master);

	MasterRxDone = 0;

	spi_flush_rx_fifo(&spi_master);
	spi_master_write_read_stream(&spi_master, (char *)MasterTxBuf, (char *)MasterRxBuf, trans_bytes);

	i = 0;
	while (MasterRxDone == 0 || spi_busy(&spi_master)) {
		wait_ms(100);
		i++;
		if (i > 150) {
			RTK_LOGE(TAG, "SPI Wait Timeout\n");
			break;
		}
	}

	/* drain: ensure all remaining FIFO/DMA data has been received */
	wait_ms(100);
	int phase1_ok = 1;
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		if ((MasterTxBuf[i] & dfs_mask) != MasterRxBuf[i]) {
			phase1_ok = 0;
			break;
		}
	}
	RTK_LOGI(TAG, "Phase1: %s\n", phase1_ok ? "success" : "fail");

	/**
	 * Phase 2: Master read (Slave write) - unidirectional
	 */
	RTK_LOGI(TAG, "---Phase2: Master read---\n");

	MasterRxDone = 0;

	spi_flush_rx_fifo(&spi_master);

	spi_master_read_stream(&spi_master, (char *)MasterRxBuf, trans_bytes);

	i = 0;
	while (MasterRxDone == 0 || spi_busy(&spi_master)) {
		wait_ms(100);
		i++;
		if (i > 150) {
			RTK_LOGE(TAG, "SPI Master Wait Timeout\n");
			break;
		}
	}

	/* drain: ensure all remaining FIFO/DMA data has been received */
	wait_ms(100);
	int phase2_ok = 1;
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		if ((MasterRxBuf[i] & dfs_mask) != (i & 0xFF)) {
			phase2_ok = 0;
			break;
		}
	}
	RTK_LOGI(TAG, "Phase2: %s\n", phase2_ok ? "success" : "fail");

	spi_free(&spi_master);

	result = phase1_ok && phase2_ok;
	RTK_LOGI(TAG, "SPI Demo: %s\n", result ? "success" : "fail");

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_mbed_spi_trx_interrupt_master(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_interrupt_task"), (rtos_task_t)spi_interrupt_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "\n\r%s Create spi_interrupt_task task failed", __FUNCTION__);
	}

	return 0;
}
