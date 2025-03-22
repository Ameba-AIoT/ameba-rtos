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
#define TEST_LOOP			10

/* compatible pinmux_funcid_name with AmebaD */
#ifndef CONFIG_AMEBAD
#define PINMUX_FUNCTION_SPIM	PINMUX_FUNCTION_SPI
#define PINMUX_FUNCTION_SPIS	PINMUX_FUNCTION_SPI
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
#else
SPI_OBJ spi_slave;
#endif

static u32 spi_interrupt(void *Adaptor)
{
	SPI_OBJ *spi_obj = (SPI_OBJ *)Adaptor;
	u32 InterruptStatus = SSI_GetIsr(spi_obj->spi_dev);
	u32 TransLen = 0;

	SSI_SetIsrClean(spi_obj->spi_dev, InterruptStatus);

	if (InterruptStatus & (SPI_BIT_TXOIS | SPI_BIT_RXUIS | SPI_BIT_RXOIS | SPI_BIT_TXUIS)) {
		printf("[INT] Tx/Rx Warning %lx ", InterruptStatus);
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

		printf("\n[%p] ", line);

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
	int Counter = 0;
	int i;

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
	InterruptRegister((IRQ_FUN)spi_interrupt, spi_master.IrqNum, (u32)&spi_master, 6);
	InterruptEn(spi_master.IrqNum, 6);
	SSI_Init(spi_master.spi_dev, &SSI_InitStructM);

	/* set format */
	SSI_SetSclkPhase(spi_master.spi_dev, SclkPhase);
	SSI_SetSclkPolarity(spi_master.spi_dev, SclkPolarity);
	SSI_SetDataFrameSize(spi_master.spi_dev, DataFrameSize - 1);

	/* set frequency */
	SSI_SetBaudDiv(spi_master.spi_dev, ClockDivider); // IpClk of SPI1 is 50MHz, IpClk of SPI0 is 100MHz


	while (Counter < TEST_LOOP) {
		printf("\r\n======= Test Loop %d =======\r\n", Counter);

		if (Counter % 2) {
			for (i = 0; i < TEST_BUF_SIZE; i++) {
				if (DataFrameSize > 8) {
					*((u16 *)TestBuf + i) = i;
				} else {
					*((u8 *)TestBuf + i) = i;
				}
			}
			GPIO_WriteBit(SPI_GPIO_CS0, 0);
			GPIO_WriteBit(SPI_GPIO_CS1, 1);
			// wait Slave ready
			DelayMs(1000);
		} else {
			for (i = 0; i < TEST_BUF_SIZE; i++) {
				if (DataFrameSize > 8) {
					*((u16 *)TestBuf + i) = ~i;
				} else {
					*((u8 *)TestBuf + i) = ~i;
				}
			}

			GPIO_WriteBit(SPI_GPIO_CS0, 1);
			GPIO_WriteBit(SPI_GPIO_CS1, 0);
			// wait Slave ready
			DelayMs(1000);
		}

		printf("SPI Master Write Test==>\r\n");
		TrDone = 0;

		spi_master_write_stream(&spi_master, TestBuf, TEST_BUF_SIZE);

		i = 0;
		printf("SPI Master Wait Write Done...\r\n");
		while (TrDone == 0) {
			DelayMs(100);
			i++;
		}
		printf("SPI Master Write Done!!\r\n");

		GPIO_WriteBit(SPI_GPIO_CS0, 1);
		GPIO_WriteBit(SPI_GPIO_CS1, 1);

		DelayMs(4000);
		Counter++;
	}
	/* free spi master */
	spi_free(&spi_master);

	printf("SPI Master Test <==\r\n");

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
	Pinmux_Config(SPI0_MOSI, PINMUX_FUNCTION_SPIS);
	Pinmux_Config(SPI0_MISO, PINMUX_FUNCTION_SPIS);
	Pinmux_Config(SPI0_SCLK, PINMUX_FUNCTION_SPIS);
	Pinmux_Config(SPI0_CS, PINMUX_FUNCTION_SPIS);
	PAD_PullCtrl(SPI0_CS, GPIO_PuPd_UP);

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
		PAD_PullCtrl((u32)SPI0_SCLK, GPIO_PuPd_DOWN);
	} else {
		PAD_PullCtrl((u32)SPI0_SCLK, GPIO_PuPd_UP);
	}

	while (SSI_Busy(spi_slave.spi_dev)) {
		printf("Wait SPI Bus Ready...\r\n");
		DelayMs(1000);
	}

	while (Counter < TEST_LOOP) {
		printf("\r\n======= Test Loop %d =======\r\n", Counter);
		_memset(TestBuf, 0, TEST_BUF_SIZE);
		printf("SPI Slave Read Test ==>\r\n");

		TrDone = 0;

		/* flush rx fifo */
		spi_flush_rx_fifo(&spi_slave);

		spi_slave_read_stream(&spi_slave, (u8 *)TestBuf, TEST_BUF_SIZE);

		i = 0;
		printf("SPI Slave Wait Read Done...\r\n");
		while (TrDone == 0) {
			DelayMs(100);
			i++;

//			if (i > 150) {
			if (i > 60) {
				printf("SPI Slave Wait Timeout\r\n");
				break;
			}
		}

		dump_data((u8 *)TestBuf, TEST_BUF_SIZE, "SPI Slave Read Data:");

		Counter++;
	}
	/* free spi slave */
	spi_free(&spi_slave);

#endif

	printf("\nSPI Demo finished.\n");

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
		printf("\n\r%s rtos_task_create(spi_multislave_task) failed", __FUNCTION__);
	}

	// rtos_sched_start();
	return 0;
}
