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
#include "main.h"
#include "spi_api.h"
#include "spi_ex_api.h"
#include "gpio_api.h"
#include "wait_api.h"
#include "os_wrapper.h"
#include <stdio.h>

#define TEST_BUF_SIZE		2048
#define SCLK_FREQ			100000
#define TEST_LOOP			10

#if SPI_IS_AS_MASTER
spi_t spi_master;

#else /* SPI_IS_AS_SLAVE */
spi_t spi_slave;
#endif

gpio_t spi_cs0;
gpio_t spi_cs1;

char TestBuf[TEST_BUF_SIZE];
volatile int TrDone;

void master_tr_done_callback(uint32_t data, SpiIrq event)
{
	(void)data;
	(void)event;

	TrDone = 1;
}

void slave_tr_done_callback(uint32_t data, SpiIrq event)
{
	(void)data;
	(void)event;

	TrDone = 1;
}

void dump_data(char *start, u32 size, char *strHeader)
{
	int row, column, index, index2, max;
	u8 *buf, *line;

	if (!start || (size == 0)) {
		return;
	}

	line = (u8 *)start;

	/*
	16 bytes per line
	*/
	if (strHeader) {
		printf("%s", strHeader);
	}

	column = size % 16;
	row = (size / 16) + 1;
	for (index = 0; index < row; index++, line += 16) {
		buf = (u8 *)line;

		max = (index == row - 1) ? column : 16;
		if (max == 0) {
			break;    /* If we need not dump this line, break it. */
		}

		printf("\n[%08x] ", (unsigned int)line);

		//Hex
		for (index2 = 0; index2 < max; index2++) {
			if (index2 == 8) {
				printf("  ");
			}
			printf("%02x ", (u8) buf[index2]);
		}

		if (max != 16) {
			if (max < 8) {
				printf("  ");
			}
			for (index2 = 16 - max; index2 > 0; index2--) {
				printf("   ");
			}
		}

	}

	printf("\n");
	return;
}

void spi_multislave_task(void)
{
	int Counter = 0;
	int i;

	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

#if SPI_IS_AS_MASTER
	gpio_init(&spi_cs0, SPI_GPIO_CS0);
	gpio_write(&spi_cs0, 1);//Initialize GPIO Pin to high
	gpio_dir(&spi_cs0, PIN_OUTPUT);    // Direction: Output
	gpio_mode(&spi_cs0, PullNone);     // No pull

	gpio_init(&spi_cs1, SPI_GPIO_CS1);
	gpio_write(&spi_cs1, 1);//Initialize GPIO Pin to high
	gpio_dir(&spi_cs1, PIN_OUTPUT);    // Direction: Output
	gpio_mode(&spi_cs1, PullNone);     // No pull

	spi_master.spi_idx = MBED_SPI1;
	spi_init(&spi_master, SPI1_MOSI, SPI1_MISO, SPI1_SCLK, SPI1_CS);
	spi_frequency(&spi_master, SCLK_FREQ);
	spi_format(&spi_master, 8, (SPI_SCLK_IDLE_LOW | SPI_SCLK_TOGGLE_START), 0);


	while (Counter < TEST_LOOP) {
		printf("======= Test Loop %d =======\r\n", Counter);

		if (Counter % 2) {
			for (i = 0; i < TEST_BUF_SIZE; i++) {
				TestBuf[i] = i;
			}
			gpio_write(&spi_cs0, 0);
			gpio_write(&spi_cs1, 1);
			// wait Slave ready
			wait_ms(1000);
		} else {
			for (i = 0; i < TEST_BUF_SIZE; i++) {
				TestBuf[i] = ~i;
			}
			gpio_write(&spi_cs0, 1);
			gpio_write(&spi_cs1, 0);
			// wait Slave ready
			wait_ms(1000);

		}

		spi_irq_hook(&spi_master, (spi_irq_handler)master_tr_done_callback, (uint32_t)&spi_master);
		printf("SPI Master Write Test==>\r\n");
		TrDone = 0;

		spi_master_write_stream(&spi_master, TestBuf, TEST_BUF_SIZE);

		i = 0;
		printf("SPI Master Wait Write Done...\r\n");
		while (TrDone == 0) {
			wait_ms(10);
			i++;
		}
		printf("SPI Master Write Done!!\r\n");

		gpio_write(&spi_cs0, 1);
		gpio_write(&spi_cs1, 1);

		wait_ms(4000);
		Counter++;
	}
	spi_free(&spi_master);
	printf("SPI Master Test <==\r\n");

#else
	spi_slave.spi_idx = MBED_SPI0;
	spi_init(&spi_slave, SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CS);
	spi_format(&spi_slave, 8, (SPI_SCLK_IDLE_LOW | SPI_SCLK_TOGGLE_START), 1);

	while (spi_busy(&spi_slave)) {
		printf("Wait SPI Bus Ready...\r\n");
		wait_ms(1000);
	}

	while (Counter < TEST_LOOP) {
		printf("======= Test Loop %d =======\r\n", Counter);
		_memset(TestBuf, 0, TEST_BUF_SIZE);
		printf("SPI Slave Read Test ==>\r\n");
		spi_irq_hook(&spi_slave, (spi_irq_handler)slave_tr_done_callback, (uint32_t)&spi_slave);
		TrDone = 0;
		spi_flush_rx_fifo(&spi_slave);

		spi_slave_read_stream(&spi_slave, TestBuf, TEST_BUF_SIZE);

		i = 0;
		printf("SPI Slave Wait Read Done...\r\n");
		while (TrDone == 0) {
			wait_ms(100);
			i++;
			if (i > 60) {
				printf("SPI Slave Wait Timeout\r\n");

				/* clear sw_busy_flag, to fix slave register interrupt assert warning after timeout happened */
				spi_slave.state &= ~SPI_STATE_RX_BUSY;
				break;
			}
		}
		dump_data(TestBuf, TEST_BUF_SIZE, "SPI Slave Read Data:");

		Counter++;
	}
	spi_free(&spi_slave);
#endif

	printf("SPI Demo finished.\n");

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_multislave_task"), (rtos_task_t)spi_multislave_task, NULL, 1024 * 4, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(spi_multislave_task) failed", __FUNCTION__);
	}

	rtos_sched_start();
	return 0;
}