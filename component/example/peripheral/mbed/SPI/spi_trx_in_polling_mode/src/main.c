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
#include "os_wrapper.h"
#include <stdio.h>

spi_t spi_master;
spi_t spi_slave;

void spi_task(void)
{
	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	/* SPI1 is as Master */
	spi_master.spi_idx = MBED_SPI1;
	spi_init(&spi_master, SPI1_MOSI, SPI1_MISO, SPI1_SCLK, SPI1_CS);
	spi_format(&spi_master, 8, 0, 0);
	spi_frequency(&spi_master, 200000);
	/* SPI0 is as Slave */
	spi_slave.spi_idx = MBED_SPI0;
	spi_init(&spi_slave,  SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CS);
	spi_format(&spi_slave, 8, 0, 1);

	int TestingTimes = 10;
	int Counter		 = 0;
	int TestData 	 = 0;
	int ReadData	 = 0;

	int result = 1;

	/**
	* Master read/write, Slave read/write
	*/
	printf("--------------------------------------------------------\n");
	for (Counter = 0, TestData = 0x01; Counter < TestingTimes; Counter++) {
		ReadData = spi_master_write(&spi_master, TestData);
		printf("Master write: %02X, read: %02X\n", TestData, ReadData);
		if (TestData - 1 != ReadData) {
			result = 0;
		}

		TestData++;

		spi_slave_write(&spi_slave, TestData);
		ReadData = spi_slave_read(&spi_slave);
		printf("Slave  write: %02X, read: %02X\n", TestData, ReadData);
		if (TestData - 1 != ReadData) {
			result = 0;
		}

		TestData++;
	}

	/**
	* Master write, Slave read
	*/
	printf("--------------------------------------------------------\n");
	for (Counter = 0, TestData = 0xFF; Counter < TestingTimes; Counter++) {
		spi_master_write(&spi_master, TestData);
		ReadData = spi_slave_read(&spi_slave);
		printf("Master write: %02X\n", TestData);
		printf("Slave  read : %02X\n", ReadData);
		if (TestData != ReadData) {
			result = 0;
		}

		TestData--;
	}

	spi_free(&spi_master);
	spi_free(&spi_slave);

	printf("SPI Demo finished.\n");

	printf("\r\nResult is %s\r\n", (result) ? "success" : "fail");
	for (;;);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_task"), (rtos_task_t)spi_task, NULL, 1024 * 4, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(spi_task) failed", __FUNCTION__);
	}

	rtos_sched_start();
	return 0;
}

