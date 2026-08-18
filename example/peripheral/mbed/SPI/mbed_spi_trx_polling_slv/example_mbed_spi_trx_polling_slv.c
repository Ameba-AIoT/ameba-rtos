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
#include "os_wrapper.h"

#define TAG "SPI_SLAVE"

#define TEST_BUF_SIZE	512

spi_t spi_slave;
u8 SlaveTxBuf[TEST_BUF_SIZE];
u8 SlaveRxBuf[TEST_BUF_SIZE];

void spi_task(void)
{
	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	/* Init SPI0 as Slave (dual-board: this board is the slave) */
	spi_slave.spi_idx = MBED_SPI0;
	spi_init(&spi_slave, SPI_MOSI, SPI_MISO, SPI_SCLK, SPI_CS);
	spi_format(&spi_slave, 8, 0, 1);

	int i;
	int result = 1;
	int ReadData;

	/* Fill TX buffer with incrementing pattern */
	_memset(SlaveTxBuf, 0, TEST_BUF_SIZE);
	_memset(SlaveRxBuf, 0, TEST_BUF_SIZE);
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		SlaveTxBuf[i] = i;
	}

	/**
	 * Phase 1: Full-duplex (Slave read/write)
	 * Preload first TX byte, then for each byte:
	 * - Read what Master sent
	 * - Compare with expected
	 * - Preload next TX byte for Master to read
	 */
	RTK_LOGI(TAG, "---Phase1: Slave read/write---\n");
	int phase1_ok = 1;

	/* Preload first TX byte */
	spi_slave_write(&spi_slave, SlaveTxBuf[0]);

	for (i = 0; i < TEST_BUF_SIZE; i++) {
		ReadData = spi_slave_read(&spi_slave);
		SlaveRxBuf[i] = (u8)(ReadData & 0xFF);

		if (SlaveRxBuf[i] != SlaveTxBuf[i]) {
			phase1_ok = 0;
		}

		/* Write next byte for master to read (skip after last byte) */
		if (i < TEST_BUF_SIZE - 1) {
			spi_slave_write(&spi_slave, SlaveTxBuf[i + 1]);
		}
	}
	RTK_LOGI(TAG, "Phase1: %s\n", phase1_ok ? "success" : "fail");

	/**
	 * Phase 2: Unidirectional (Master write, Slave read)
	 * Slave only reads and compares with expected pattern.
	 */
	RTK_LOGI(TAG, "---Phase2: Slave read---\n");
	int phase2_ok = 1;
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		ReadData = spi_slave_read(&spi_slave);
		if (ReadData != (i & 0xFF)) {
			phase2_ok = 0;
		}
	}
	RTK_LOGI(TAG, "Phase2: %s\n", phase2_ok ? "success" : "fail");

	spi_free(&spi_slave);

	result = phase1_ok && phase2_ok;
	RTK_LOGI(TAG, "SPI Demo: %s\n", result ? "success" : "fail");

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_mbed_spi_trx_polling_slave(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_task"), (rtos_task_t)spi_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "\n\r%s Create spi_task task failed", __FUNCTION__);
	}

	return 0;
}
