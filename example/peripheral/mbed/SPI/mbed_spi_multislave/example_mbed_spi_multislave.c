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
#include "gpio_api.h"
#include "wait_api.h"
#include "os_wrapper.h"

#define TEST_BUF_SIZE		2048
#define SCLK_FREQ			100000
#define MASTER_NR_SLAVES	2

#ifndef LOOP_COUNT
#define LOOP_COUNT 10
#endif

#if SPI_IS_AS_MASTER
#define TAG "SPI_MASTER"
spi_t spi_master;
static rtos_mutex_t spi_master_lock;
static volatile int master_workers_done;

#else /* SPI_IS_AS_SLAVE */
#define TAG "SPI_SLAVE"
spi_t spi_slave;
#endif

gpio_t spi_cs0;
gpio_t spi_cs1;

u8 TestBuf[TEST_BUF_SIZE];
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

void dump_data(u8 *start, u32 size, char *strHeader)
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
		RTK_LOGI(TAG, "%s", strHeader);
	}

	column = size % 16;
	row = (size / 16) + 1;
	for (index = 0; index < row; index++, line += 16) {
		buf = (u8 *)line;

		max = (index == row - 1) ? column : 16;
		if (max == 0) {
			break;    /* If we need not dump this line, break it. */
		}

		RTK_LOGI(TAG, "\n[%08x] ", (u32)line);

		//Hex
		for (index2 = 0; index2 < max; index2++) {
			if (index2 == 8) {
				RTK_LOGI(TAG, "  ");
			}
			RTK_LOGI(TAG, "%02x ", (u8) buf[index2]);
		}

		if (max != 16) {
			if (max < 8) {
				RTK_LOGI(TAG, "  ");
			}
			for (index2 = 16 - max; index2 > 0; index2--) {
				RTK_LOGI(TAG, "   ");
			}
		}

	}

	return;
}

#if SPI_IS_AS_MASTER
static void spi_master_write_stream_with_cs(spi_t *spi_obj, char *tx_buffer, u32 length, gpio_t *cs_pin)
{
	/* assert CS */
	gpio_write(cs_pin, 0);
	RTK_LOGI(TAG, "%s: enable\n",
			 (cs_pin == &spi_cs0) ? "SPI_GPIO_CS0" :
			 (cs_pin == &spi_cs1) ? "SPI_GPIO_CS1" : "CS");
	// wait Slave ready
	wait_ms(1000);

	spi_irq_hook(spi_obj, (spi_irq_handler)master_tr_done_callback, (uint32_t)spi_obj);
	RTK_LOGI(TAG, "SPI Master Write Test==>\n");
	TrDone = 0;

	spi_master_write_stream(spi_obj, tx_buffer, length);

	int i = 0;
	RTK_LOGI(TAG, "SPI Master Wait Write Done...\n");
	while (TrDone == 0) {
		wait_ms(10);
		i++;
	}
	RTK_LOGI(TAG, "SPI Master Write Done!!\n");

	/* deassert CS */
	gpio_write(cs_pin, 1);
}

static void spi_master_worker(void *arg)
{
	gpio_t *cs_pin = (gpio_t *)arg;
	const char *cs_name = (cs_pin == &spi_cs0) ? "CS0" : "CS1";
	int Counter, i;

	RTK_LOGI(TAG, "[%s] worker started\n", cs_name);
	for (Counter = 0; Counter < LOOP_COUNT; Counter++) {
		RTK_LOGI(TAG, "\n[%s] ====== Test Loop %d =======\n", cs_name, Counter);

		rtos_mutex_take(spi_master_lock, RTOS_MAX_DELAY);

		/* prepare pattern inside the lock */
		for (i = 0; i < TEST_BUF_SIZE; i++) {
			TestBuf[i] = (cs_pin == &spi_cs0) ? i : ~i;
		}
		spi_master_write_stream_with_cs(&spi_master, (char *)TestBuf, TEST_BUF_SIZE, cs_pin);

		rtos_mutex_give(spi_master_lock);

		/* yield so the sibling thread gets a fair chance */
		rtos_time_delay_ms(2000);
	}

	RTK_LOGI(TAG, "[%s] completed all loops\n", cs_name);

	rtos_mutex_take(spi_master_lock, RTOS_MAX_DELAY);
	if (++master_workers_done == MASTER_NR_SLAVES) {
		spi_free(&spi_master);
		RTK_LOGI(TAG, "SPI Master Test <==\n");
	}
	rtos_mutex_give(spi_master_lock);

	rtos_task_delete(NULL);
}
#endif

void spi_multislave_task(void)
{
	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

#if SPI_IS_AS_MASTER
	gpio_init(&spi_cs0, SPI_GPIO_CS0);
	gpio_dir(&spi_cs0, PIN_OUTPUT);    // Direction: Output
	gpio_mode(&spi_cs0, PullNone);     // No pull
	gpio_write(&spi_cs0, 1);//Initialize GPIO Pin to high

	gpio_init(&spi_cs1, SPI_GPIO_CS1);
	gpio_dir(&spi_cs1, PIN_OUTPUT);    // Direction: Output
	gpio_mode(&spi_cs1, PullNone);     // No pull
	gpio_write(&spi_cs1, 1);//Initialize GPIO Pin to high

	spi_master.spi_idx = MBED_SPI1;
	spi_init(&spi_master, SPI_MOSI, SPI_MISO, SPI_SCLK, SPI_CS);
	spi_frequency(&spi_master, SCLK_FREQ);
	spi_format(&spi_master, 8, (SPI_SCLK_IDLE_LOW | SPI_SCLK_TOGGLE_START), 0);


	/* Create mutex to protect the shared SPI bus */
	if (rtos_mutex_create(&spi_master_lock) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create spi_master_lock failed\n");
		rtos_task_delete(NULL);
		return;
	}

	/* Create two worker threads, one per GPIO CS line */
	if (rtos_task_create(NULL, ((const char *)"spi_master_cs0"), (rtos_task_t)spi_master_worker,
						 (void *)&spi_cs0, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create master worker CS0 failed\n");
	}
	if (rtos_task_create(NULL, ((const char *)"spi_master_cs1"), (rtos_task_t)spi_master_worker,
						 (void *)&spi_cs1, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create master worker CS1 failed\n");
	}

	/* Wait for both workers to complete before continuing */
	while (master_workers_done < MASTER_NR_SLAVES) {
		rtos_time_delay_ms(100);
	}

#else
	spi_slave.spi_idx = MBED_SPI0;
	spi_init(&spi_slave, SPI_MOSI, SPI_MISO, SPI_SCLK, SPI_CS);
	spi_format(&spi_slave, 8, (SPI_SCLK_IDLE_LOW | SPI_SCLK_TOGGLE_START), 1);

	while (spi_busy(&spi_slave)) {
		RTK_LOGI(TAG, "Wait SPI Bus Ready...\n");
		wait_ms(1000);
	}

	int Counter, i;
	int cs_low_pass = 0, cs_low_fail = 0, cs_deselected = 0;

	for (Counter = 0; Counter < LOOP_COUNT; Counter++) {
		RTK_LOGI(TAG, "\n======= Test Loop %d =======\n", Counter);
		_memset(TestBuf, 0, TEST_BUF_SIZE);
		RTK_LOGI(TAG, "SPI Slave Read Test ==>\n");
		spi_irq_hook(&spi_slave, (spi_irq_handler)slave_tr_done_callback, (uint32_t)&spi_slave);
		TrDone = 0;

		/* Wait for bus idle — mbed ssi_int_read returns FALSE if SSI is busy. */
		while (spi_busy(&spi_slave));
		spi_slave_read_stream(&spi_slave, (char *)TestBuf, TEST_BUF_SIZE);

		/* wait for transfer complete or timeout */
		i = 0;
		RTK_LOGI(TAG, "SPI Slave Wait Read Done...\n");
		while (TrDone == 0) {
			wait_ms(100);
			i++;
			if (i > 150) {
				RTK_LOGE(TAG, "SPI Slave Wait Timeout\n");

				/* clear sw_busy_flag, to fix slave register interrupt assert warning after timeout happened */
				spi_slave.state &= ~SPI_STATE_RX_BUSY;
				break;
			}
		}

		if (TrDone) {
			int pass = 1;
			int is_inc = (TestBuf[0] == 0); /* 0x00: inc pattern, 0xFF: dec pattern */
			for (i = 0; i < TEST_BUF_SIZE; i++) {
				u8 expected = is_inc ? (u8)i : (u8)(~i);
				if (TestBuf[i] != expected) {
					RTK_LOGE(TAG, "mismatch at [%d]: got %02x expected %02x\n", i, TestBuf[i], expected);
					pass = 0;
					break;
				}
			}
			if (pass) {
				RTK_LOGI(TAG, "CS low: data check PASS\n");
				cs_low_pass++;
			} else {
				dump_data(TestBuf, TEST_BUF_SIZE, "SPI Slave Read Data:");
				RTK_LOGE(TAG, "CS low: data check FAIL\n");
				cs_low_fail++;
			}
		} else {
			RTK_LOGI(TAG, "No data received (deselected)\n");
			cs_deselected++;
		}

	}

	RTK_LOGI(TAG, "\n=== Summary ===\n");
	RTK_LOGI(TAG, "success: %d, fail: %d, deselected: %d\n", cs_low_pass, cs_low_fail, cs_deselected);

	spi_free(&spi_slave);
#endif

	RTK_LOGI(TAG, "SPI Demo finished.\n");

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_mbed_spi_multislave(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_multislave_task"), (rtos_task_t)spi_multislave_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "%s Create spi_multislave_task task failed", __FUNCTION__);
	}

	return 0;
}
