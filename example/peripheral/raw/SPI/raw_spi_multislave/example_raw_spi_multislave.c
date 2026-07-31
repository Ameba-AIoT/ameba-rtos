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

#define TEST_BUF_SIZE		2048
#define DataFrameSize		8
#define ClockDivider 		1000
#define MASTER_NR_SLAVES	2

#ifndef LOOP_COUNT
#define LOOP_COUNT 10
#endif

/* compatible pinmux_funcid_name with RTL872xD */
#ifndef CONFIG_AMEBAD
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RTL8720F)
#define PINMUX_FUNCTION_SPIM	PINMUX_FUNCTION_SPI1
#define PINMUX_FUNCTION_SPIS	PINMUX_FUNCTION_SPI0
#else
#define PINMUX_FUNCTION_SPIM	PINMUX_FUNCTION_SPI
#define PINMUX_FUNCTION_SPIS	PINMUX_FUNCTION_SPI
#endif
#endif

SRAM_NOCACHE_DATA_SECTION u8 TestBuf[TEST_BUF_SIZE];
volatile int TrDone;

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

#if SPI_IS_AS_MASTER
SPI_OBJ spi_master;
static rtos_mutex_t spi_master_lock;
static volatile int master_workers_done;
#define TAG "SPI_MASTER"
#else
SPI_OBJ spi_slave;
#define TAG "SPI_SLAVE"
#endif

static u32 spi_interrupt(void *Adaptor)
{
	SPI_OBJ *spi_obj = (SPI_OBJ *)Adaptor;
	u32 InterruptStatus = SSI_GetIsr(spi_obj->spi_dev);
	u32 TransLen = 0;

	SSI_SetIsrClean(spi_obj->spi_dev, InterruptStatus);

	if (InterruptStatus & (SPI_BIT_TXOIS | SPI_BIT_RXUIS | SPI_BIT_RXOIS | SPI_BIT_TXUIS)) {
		RTK_LOGI(TAG, "[INT] Tx/Rx Warning %08x ", InterruptStatus);
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
			TrDone = 1;
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
		/* all data written into fifo */
		if (spi_obj->TxLength == 0) {
			SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_TXOIM | SPI_BIT_TXEIM), DISABLE);
			// If it's not a dummy TX for master read SPI, then call the TX_done callback
			TrDone = 1;
		}
	}

	return 0;
}

static void spi_free(SPI_OBJ *spi_obj)
{
	InterruptDis(spi_obj->IrqNum);
	InterruptUnRegister(spi_obj->IrqNum);
	SSI_INTConfig(spi_obj->spi_dev, (SPI_BIT_RXFIM | SPI_BIT_RXOIM | SPI_BIT_RXUIM), DISABLE);

	SSI_Cmd(spi_obj->spi_dev, DISABLE);
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
}

#if SPI_IS_AS_MASTER

static void spi_master_write_stream(SPI_OBJ *spi_obj, u8 *tx_buffer, u32 length)
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

static void spi_master_write_stream_with_cs(SPI_OBJ *spi_obj, u8 *tx_buffer, u32 length, u32 cs_pin)
{
	/* assert CS */
	GPIO_WriteBit(cs_pin, 0);
	RTK_LOGI(TAG, "%s: enable\n",
			 (cs_pin == SPI_GPIO_CS0) ? "SPI_GPIO_CS0" :
			 (cs_pin == SPI_GPIO_CS1) ? "SPI_GPIO_CS1" : "CS");
	/* wait Slave ready */
	DelayMs(1000);

	RTK_LOGI(TAG, "SPI Master Write Test==>\n");
	TrDone = 0;
	spi_master_write_stream(spi_obj, tx_buffer, length);

	RTK_LOGI(TAG, "SPI Master Wait Write Done...\n");
	while (TrDone == 0) {
		DelayMs(100);
	}
	RTK_LOGI(TAG, "SPI Master Write Done!!\n");

	/* deassert CS */
	GPIO_WriteBit(cs_pin, 1);
}

/*
 * Two master worker threads, one per GPIO CS line.  Both share the
 * same SPI bus via spi_master_lock, simulating an RTOS scenario
 * where multiple tasks contend for a shared SPI bus.
 *
 * arg holds the GPIO CS pin number to use.
 */
static void spi_master_worker(void *arg)
{
	u32 cs_pin = (u32)(uintptr_t)arg;
	const char *cs_name = (cs_pin == SPI_GPIO_CS0) ? "CS0" : "CS1";
	int Counter, i;

	RTK_LOGI(TAG, "[%s] worker started\n", cs_name);
	for (Counter = 0; Counter < LOOP_COUNT; Counter++) {
		RTK_LOGI(TAG, "\n[%s] ====== Test Loop %d =======\n", cs_name, Counter);

		rtos_mutex_take(spi_master_lock, RTOS_MAX_DELAY);

		/* prepare pattern inside the lock */
		for (i = 0; i < TEST_BUF_SIZE; i++) {
			if (DataFrameSize > 8) {
				*((u16 *)TestBuf + i) = (cs_pin == SPI_GPIO_CS0) ? i : ~i;
			} else {
				*((u8 *)TestBuf + i) = (cs_pin == SPI_GPIO_CS0) ? i : ~i;
			}
		}
		spi_master_write_stream_with_cs(&spi_master, TestBuf, TEST_BUF_SIZE, cs_pin);

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

#else

static void spi_flush_rx_fifo(SPI_OBJ *spi_obj)
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

static void spi_slave_read_stream(SPI_OBJ *spi_obj, u8 *rx_buffer, u32 length)
{
	assert_param(length != 0);
//	printf(">>> WAIT: SSI_Busy 0 !!! \r\n");

	/*  As a Slave mode, if the peer(Master) side is power off, the BUSY flag is always on */
	while (SSI_Busy(spi_obj->spi_dev));
//	printf("<<< EXIT: SSI_Busy 0 !!! \r\n");

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
#endif

void spi_multislave_task(void)
{
	u32 SclkPhase = SCPH_TOGGLES_IN_MIDDLE; // SCPH_TOGGLES_IN_MIDDLE or SCPH_TOGGLES_AT_START
	u32 SclkPolarity = SCPOL_INACTIVE_IS_LOW; // SCPOL_INACTIVE_IS_LOW or SCPOL_INACTIVE_IS_HIGH

	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

#if SPI_IS_AS_MASTER
	/* enable GPIO clock and function */
	// RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

	/* init gpio as CS0, default output high */
	GPIO_InitTypeDef GPIO_InitStructM1;
	GPIO_InitStructM1.GPIO_Pin = SPI_GPIO_CS0;
	GPIO_InitStructM1.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructM1.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(&GPIO_InitStructM1);
	GPIO_WriteBit(SPI_GPIO_CS0, 1);

	/* init gpio as CS1, default output high */
	GPIO_InitTypeDef GPIO_InitStructM2;
	GPIO_InitStructM2.GPIO_Pin = SPI_GPIO_CS1;
	GPIO_InitStructM2.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructM2.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(&GPIO_InitStructM2);
	GPIO_WriteBit(SPI_GPIO_CS1, 1);

	/* SPI1 as Master */
	spi_master.Index = 1;
	spi_master.Role = SSI_MASTER;
	spi_master.spi_dev = SPI_DEV_TABLE[spi_master.Index].SPIx;
	spi_master.IrqNum = SPI_DEV_TABLE[spi_master.Index].IrqNum;

	/* init spi master */
	SSI_InitTypeDef SSI_InitStructM;
	SSI_StructInit(&SSI_InitStructM);
	RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, ENABLE);
	Pinmux_Config(SPI_MOSI, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_MISO, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_SCLK, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_CS, PINMUX_FUNCTION_SPIM);
	PAD_PullCtrl((u32)SPI_CS, GPIO_PuPd_UP);

	SSI_SetRole(spi_master.spi_dev, SSI_MASTER);
	SSI_InitStructM.SPI_Role = SSI_MASTER;
	InterruptRegister((IRQ_FUN)spi_interrupt, spi_master.IrqNum, (u32)&spi_master, 6);
	InterruptEn(spi_master.IrqNum, 6);
	SSI_Init(spi_master.spi_dev, &SSI_InitStructM);

	/* set format */
	SSI_SetSclkPhase(spi_master.spi_dev, SclkPhase);
	SSI_SetSclkPolarity(spi_master.spi_dev, SclkPolarity);
	SSI_SetDataFrameSize(spi_master.spi_dev, DataFrameSize - 1);

	/* set frequency */
	SSI_SetBaudDiv(spi_master.spi_dev, ClockDivider); // IpClk of SPI1 is 50MHz, IpClk of SPI0 is 100MHz


	/* Create mutex to protect the shared SPI0 bus */
	if (rtos_mutex_create(&spi_master_lock) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create spi_master_lock failed\n");
		rtos_task_delete(NULL);
		return;
	}

	/* Create two worker threads, one per GPIO CS line */
	if (rtos_task_create(NULL, ((const char *)"spi_master_cs0"), (rtos_task_t)spi_master_worker,
						 (void *)SPI_GPIO_CS0, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create master worker CS0 failed\n");
	}
	if (rtos_task_create(NULL, ((const char *)"spi_master_cs1"), (rtos_task_t)spi_master_worker,
						 (void *)SPI_GPIO_CS1, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create master worker CS1 failed\n");
	}

	/* Wait for both workers to complete before continuing */
	while (master_workers_done < MASTER_NR_SLAVES) {
		rtos_time_delay_ms(100);
	}

#else
	/* SPI0 as Slave */
	spi_slave.Index = 0;
	spi_slave.Role = SSI_SLAVE;
	spi_slave.spi_dev = SPI_DEV_TABLE[spi_slave.Index].SPIx;
	spi_slave.IrqNum = SPI_DEV_TABLE[spi_slave.Index].IrqNum;

	/* init spi slave */
	SSI_InitTypeDef SSI_InitStructS;
	SSI_StructInit(&SSI_InitStructS);
	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	Pinmux_Config(SPI_MOSI, PINMUX_FUNCTION_SPIS);
	Pinmux_Config(SPI_MISO, PINMUX_FUNCTION_SPIS);
	Pinmux_Config(SPI_SCLK, PINMUX_FUNCTION_SPIS);
	Pinmux_Config(SPI_CS, PINMUX_FUNCTION_SPIS);
	PAD_PullCtrl(SPI_CS, GPIO_PuPd_UP);

	SSI_SetRole(spi_slave.spi_dev, SSI_SLAVE);
	SSI_InitStructS.SPI_Role = SSI_SLAVE;
	InterruptRegister((IRQ_FUN)spi_interrupt, spi_slave.IrqNum, (u32)&spi_slave, 6);
	InterruptEn(spi_slave.IrqNum, 6);
	SSI_Init(spi_slave.spi_dev, &SSI_InitStructS);

	/* set format */
	SSI_SetSclkPhase(spi_slave.spi_dev, SclkPhase);
	SSI_SetSclkPolarity(spi_slave.spi_dev, SclkPolarity);
	SSI_SetDataFrameSize(spi_slave.spi_dev, DataFrameSize - 1);

	if (SclkPolarity == SCPOL_INACTIVE_IS_LOW) {
		PAD_PullCtrl((u32)SPI_SCLK, GPIO_PuPd_DOWN);
	} else {
		PAD_PullCtrl((u32)SPI_SCLK, GPIO_PuPd_UP);
	}

	int Counter, i;
	int cs_low_pass = 0, cs_low_fail = 0, cs_deselected = 0;

	for (Counter = 0; Counter < LOOP_COUNT; Counter++) {
		RTK_LOGI(TAG, "\n======= Test Loop %d =======\n", Counter);
		_memset(TestBuf, 0, TEST_BUF_SIZE);
		RTK_LOGI(TAG, "SPI Slave Read Test ==>\n");

		TrDone = 0;

		/* flush rx fifo */
		spi_flush_rx_fifo(&spi_slave);

		spi_slave_read_stream(&spi_slave, (u8 *)TestBuf, TEST_BUF_SIZE);

		i = 0;
		RTK_LOGI(TAG, "SPI Slave Wait Read Done...\n");
		while (TrDone == 0) {
			DelayMs(100);
			i++;

			if (i > 150) {
				RTK_LOGE(TAG, "SPI Slave Wait Timeout\n");
				break;
			}
		}

		if (TrDone) {
			int pass = 1;
			int is_inc = (TestBuf[0] == 0); /* 0x00: inc pattern, 0xFF: dec pattern */
			for (i = 0; i < TEST_BUF_SIZE; i++) {
				u8 expected = is_inc ? (u8)i : (u8)(~i);
				if (TestBuf[i] != expected) {
					RTK_LOGE(TAG, "mismatch at [%d]: got %08x expected %08x\n", i, (u32)TestBuf[i], (u32)expected);
					pass = 0;
					break;
				}
			}
			if (pass) {
				RTK_LOGI(TAG, "CS low: data check PASS\n");
				cs_low_pass++;
			} else {
				dump_data((u8 *)TestBuf, TEST_BUF_SIZE, "SPI Slave Read Data:");
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

	/* free spi slave */
	spi_free(&spi_slave);

#endif

	RTK_LOGI(TAG, "\nSPI Demo finished.\n");

	rtos_task_delete(NULL);

}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_raw_spi_multislave(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_multislave_task"), (rtos_task_t)spi_multislave_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "%s Create spi_multislave_task task failed", __FUNCTION__);
	}

	return 0;
}
