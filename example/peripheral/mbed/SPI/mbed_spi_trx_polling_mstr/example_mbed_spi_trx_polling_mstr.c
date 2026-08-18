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
#include "os_wrapper.h"

#define TAG "SPI_MASTER"

#define TEST_BUF_SIZE	512

spi_t spi_master;
u8 MasterTxBuf[TEST_BUF_SIZE];
u8 MasterRxBuf[TEST_BUF_SIZE];

void spi_task(void)
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
	spi_format(&spi_master, 8, 0, 0);
	spi_frequency(&spi_master, 200000);

	int i;
	int result = 1;
	int ReadData;

	/* Fill TX buffer with incrementing pattern */
	_memset(MasterTxBuf, 0, TEST_BUF_SIZE);
	_memset(MasterRxBuf, 0, TEST_BUF_SIZE);
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		MasterTxBuf[i] = i;
	}

	/**
	 * Phase 1: Full-duplex (Master write/read)
	 * Master sends pattern i, Slave sends pattern i.
	 * Master reads back what Slave sent, expects i.
	 */
	RTK_LOGI(TAG, "---Phase1: Master write/read---\n");
	spi_flush_rx_fifo(&spi_master);	/* clear stale RX data from init */
	result = 1;
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		ReadData = spi_master_write(&spi_master, MasterTxBuf[i]);
		MasterRxBuf[i] = (u8)(ReadData & 0xFF);
		if (MasterRxBuf[i] != MasterTxBuf[i]) {
			result = 0;
		}
	}
	RTK_LOGI(TAG, "Phase1: %s\n", result ? "success" : "fail");

	/**
	 * Phase 2: Unidirectional (Master write, Slave read)
	 * Master only writes; dummy read to generate clock.
	 */
	RTK_LOGI(TAG, "---Phase2: Master write---\n");
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		spi_master_write(&spi_master, MasterTxBuf[i]);	/* ignore return */
	}
	RTK_LOGI(TAG, "Phase2: success\n");

	spi_free(&spi_master);

	RTK_LOGI(TAG, "SPI Demo: %s\n", result ? "success" : "fail");

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_mbed_spi_trx_polling_master(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_task"), (rtos_task_t)spi_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "\n\r%s Create spi_task task failed", __FUNCTION__);
	}

	return 0;
}
