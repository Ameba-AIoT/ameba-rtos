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
#include "os_wrapper.h"
#include <stdio.h>

void spi_task(void)
{
	u32 SclkPhase = SCPH_TOGGLES_IN_MIDDLE; // SCPH_TOGGLES_IN_MIDDLE or SCPH_TOGGLES_AT_START
	u32 SclkPolarity = SCPOL_INACTIVE_IS_LOW; // SCPOL_INACTIVE_IS_LOW or SCPOL_INACTIVE_IS_HIGH
	u32 ClockDivider = 500; // Fssi_clk is 100MHz
	int TestingTimes = 10;
	int Counter = 0;
	int TestData = 0;
	int ReadData = 0;
	int result = 1;

	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	/* SPI1 is as Master and SPI0 is as Slave */
	/* init SPI1 */
	SSI_SetRole(SPI1_DEV, SSI_MASTER);
	SSI_InitTypeDef SSI_InitStructM;
	SSI_StructInit(&SSI_InitStructM);

	RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, ENABLE);
	Pinmux_Config(SPI1_MOSI, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI1_MISO, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI1_SCLK, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI1_CS, PINMUX_FUNCTION_SPI);
	PAD_PullCtrl((u32)SPI1_CS, GPIO_PuPd_UP);

	SSI_InitStructM.SPI_Role = SSI_MASTER;
	SSI_Init(SPI1_DEV, &SSI_InitStructM);

	/* set format */
	SSI_SetSclkPhase(SPI1_DEV, SclkPhase);
	SSI_SetSclkPolarity(SPI1_DEV, SclkPolarity);
	SSI_SetDataFrameSize(SPI1_DEV, DFS_8_BITS);

	/* set frequency */
	SSI_SetBaudDiv(SPI1_DEV, ClockDivider); // Fssi_clk is 100MHz

	/* init SPI0 */
	SSI_SetRole(SPI0_DEV, SSI_SLAVE);
	SSI_InitTypeDef SSI_InitStructS;
	SSI_StructInit(&SSI_InitStructS);

	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	Pinmux_Config(SPI0_MOSI, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI0_MISO, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI0_SCLK, PINMUX_FUNCTION_SPI);
	Pinmux_Config(SPI0_CS, PINMUX_FUNCTION_SPI);
	PAD_PullCtrl(SPI0_CS, GPIO_PuPd_UP); //cs
	if (SclkPolarity == SCPOL_INACTIVE_IS_LOW) {
		PAD_PullCtrl((u32)SPI0_SCLK, GPIO_PuPd_DOWN);
	} else {
		PAD_PullCtrl((u32)SPI0_SCLK, GPIO_PuPd_UP);
	}

	SSI_InitStructS.SPI_Role = SSI_SLAVE;
	SSI_Init(SPI0_DEV, &SSI_InitStructS);

	/* set format */
	SSI_SetSclkPhase(SPI0_DEV, SclkPhase);
	SSI_SetSclkPolarity(SPI0_DEV, SclkPolarity);
	SSI_SetDataFrameSize(SPI0_DEV, DFS_8_BITS);

	/**
	* Master read/write, Slave read/write
	*/
	printf("------------- Master read/write, Slave read/write --------------------------\n");
	for (Counter = 0, TestData = 0x01; Counter < TestingTimes; Counter++) {
		/* master write */
		while (!SSI_Writeable(SPI1_DEV));
		SSI_WriteData(SPI1_DEV, TestData);

		/* master read */
		while (!SSI_Readable(SPI1_DEV));
		ReadData = (int)SSI_ReadData(SPI1_DEV);

		printf("Master write: %02X, read: %02X\n", TestData, ReadData);
		if (TestData - 1 != ReadData) {
			result = 0;
		}
		TestData++;

		/* slave write */
		while (!SSI_Writeable(SPI0_DEV));
		SSI_WriteData(SPI0_DEV, TestData);

		/* slave read */
		while (!SSI_Readable(SPI0_DEV));
		ReadData = (int)SSI_ReadData(SPI0_DEV);

		printf("Slave  write: %02X, read: %02X\n", TestData, ReadData);
		if (TestData - 1 != ReadData) {
			result = 0;
		}

		TestData++;
	}

	/**
	* Master write, Slave read
	*/
	printf("-------------- Master write, Slave read ------------------------------------\n");
	for (Counter = 0, TestData = 0xFF; Counter < TestingTimes; Counter++) {
		/* master write */
		while (!SSI_Writeable(SPI1_DEV));
		SSI_WriteData(SPI1_DEV, TestData);

		/* slave read */
		while (!SSI_Readable(SPI0_DEV));
		ReadData = (int)SSI_ReadData(SPI0_DEV);

		printf("Master write: %02X\n", TestData);
		printf("Slave  read : %02X\n", ReadData);
		if (TestData != ReadData) {
			result = 0;
		}

		TestData--;
	}

	/* free SPI1 & SPI0*/
	SSI_Cmd(SPI1_DEV, DISABLE);
	SSI_Cmd(SPI0_DEV, DISABLE);

	printf("SPI Demo finished.\n");
	printf("\r\nResult is %s\r\n", (result) ? "success" : "fail");

	rtos_task_delete(NULL);
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

