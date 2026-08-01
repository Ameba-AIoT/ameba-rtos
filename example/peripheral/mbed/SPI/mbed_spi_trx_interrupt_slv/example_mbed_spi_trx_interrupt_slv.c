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

#define TAG "SPI_SLAVE"

#define DataFrameSize	8
#define dfs_mask		0xFF
#define Mode			0
#define TEST_BUF_SIZE	512

SRAM_NOCACHE_DATA_SECTION u8 SlaveTxBuf[TEST_BUF_SIZE];
SRAM_NOCACHE_DATA_SECTION u8 SlaveRxBuf[TEST_BUF_SIZE];

volatile int SlaveTxDone;
volatile int SlaveRxDone;

spi_t spi_slave;

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
		RTK_LOGE(TAG, "unknown interrupt event!\n");
	}
}

void spi_interrupt_task(void)
{
	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	/* Init SPI0 as Slave (dual-board: this board is the slave) */
	spi_slave.spi_idx = MBED_SPI0;
	spi_init(&spi_slave, SPI_MOSI, SPI_MISO, SPI_SCLK, SPI_CS);
	spi_format(&spi_slave, DataFrameSize, Mode, 1);

	int i;
	int result = 1;
	u32 trans_bytes = TEST_BUF_SIZE;

	_memset(SlaveTxBuf, 0, TEST_BUF_SIZE);
	_memset(SlaveRxBuf, 0, TEST_BUF_SIZE);
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		*((u8 *)SlaveTxBuf + i) = i;
	}

	spi_irq_hook(&spi_slave, (spi_irq_handler)Slave_tr_done_callback, (uint32_t)&spi_slave);

	/**
	 * Phase 1: Full-duplex (Slave read/write)
	 */
	RTK_LOGI(TAG, "---Phase1: Slave read/write---\n");

	SlaveRxDone = 0;
	SlaveTxDone = 0;

	spi_flush_rx_fifo(&spi_slave);
	while (spi_busy(&spi_slave)); /* wait for bus idle */
	spi_slave_read_stream(&spi_slave, (char *)SlaveRxBuf, trans_bytes);
	spi_slave_write_stream(&spi_slave, (char *)SlaveTxBuf, trans_bytes);

	i = 0;
	while ((SlaveTxDone && SlaveRxDone) == 0) {
		wait_ms(100);
		i++;
		if (i > 150) {
			RTK_LOGE(TAG, "SPI Wait Timeout\n");
			break;
		}
	}

	result = 1;
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		if ((SlaveTxBuf[i] & dfs_mask) != SlaveRxBuf[i]) {
			result = 0;
			break;
		}
	}
	RTK_LOGI(TAG, "Phase1: %s\n", result ? "success" : "fail");

	/**
	 * Phase 2: Slave write (Master read) - unidirectional
	 */
	RTK_LOGI(TAG, "---Phase2: Slave write---\n");

	/* flush stale RX data before starting Phase2 */
	spi_flush_rx_fifo(&spi_slave);

	SlaveTxDone = 0;

	spi_slave_write_stream(&spi_slave, (char *)SlaveTxBuf, trans_bytes);

	i = 0;
	while (SlaveTxDone == 0) {
		wait_ms(100);
		i++;
		if (i > 150) {
			RTK_LOGE(TAG, "SPI Slave Wait Timeout\n");
			break;
		}
	}

	if (SlaveTxDone) {
		RTK_LOGI(TAG, "Phase2: success\n");
	} else {
		RTK_LOGI(TAG, "Phase2: fail\n");
		result = 0;
	}

	spi_free(&spi_slave);

	RTK_LOGI(TAG, "SPI Demo: %s\n", result ? "success" : "fail");

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_mbed_spi_trx_interrupt_slave(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_interrupt_task"), (rtos_task_t)spi_interrupt_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "\n\r%s Create spi_interrupt_task task failed", __FUNCTION__);
	}

	return 0;
}
