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
#define TAG "SPI_MASTER"

/* compatible pinmux_funcid_name with RTL872xD */
#ifndef CONFIG_AMEBAD
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RTL8720F)
#define PINMUX_FUNCTION_SPIM	PINMUX_FUNCTION_SPI0
#else
#define PINMUX_FUNCTION_SPIM	PINMUX_FUNCTION_SPI
#endif
#endif

#define DataFrameSize	DFS_8_BITS
#define ClockDivider	500
#define TEST_BUF_SIZE	512

u8 MasterTxBuf[TEST_BUF_SIZE];
u8 MasterRxBuf[TEST_BUF_SIZE];

void spi_task(void)
{
	int result = 1;
	int i = 0;
	u32 read_byte;

	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	/* init SPI0 as Master */
	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	Pinmux_Config(SPI_MOSI, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_MISO, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_SCLK, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_CS, PINMUX_FUNCTION_SPIM);
	PAD_PullCtrl((u32)SPI_CS, GPIO_PuPd_UP);

	SSI_SetRole(SPI0_DEV, SSI_MASTER);
	SSI_InitTypeDef SSI_InitStructM;
	SSI_StructInit(&SSI_InitStructM);
	SSI_InitStructM.SPI_Role = SSI_MASTER;
	SSI_Init(SPI0_DEV, &SSI_InitStructM);

	/* set format */
	SSI_SetSclkPhase(SPI0_DEV, SCPH_TOGGLES_IN_MIDDLE);
	SSI_SetSclkPolarity(SPI0_DEV, SCPOL_INACTIVE_IS_LOW);
	SSI_SetDataFrameSize(SPI0_DEV, DataFrameSize);

	/* set frequency */
	SSI_SetBaudDiv(SPI0_DEV, ClockDivider);

	/* fill TX buffer with pattern */
	_memset(MasterTxBuf, 0, TEST_BUF_SIZE);
	_memset(MasterRxBuf, 0, TEST_BUF_SIZE);
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		MasterTxBuf[i] = i;
	}

	/**
	 * Phase 1: Master write/read (full duplex)
	 */
	RTK_LOGI(TAG, "---Phase1: Master write/read---\n");
	result = 1;
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		while (!SSI_Writeable(SPI0_DEV));
		SSI_WriteData(SPI0_DEV, MasterTxBuf[i]);

		while (!SSI_Readable(SPI0_DEV));
		read_byte = SSI_ReadData(SPI0_DEV);
		MasterRxBuf[i] = read_byte;

		if (MasterRxBuf[i] != MasterTxBuf[i]) {
			result = 0;
		}
	}
	if (result) {
		RTK_LOGI(TAG, "Phase1: success\n");
	} else {
		RTK_LOGE(TAG, "Phase1: fail\n");
	};

	/**
	 * Phase 2: Master write (Slave read) - unidirectional
	 */
	RTK_LOGI(TAG, "---Phase2: Master write---\n");
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		while (!SSI_Writeable(SPI0_DEV));
		SSI_WriteData(SPI0_DEV, MasterTxBuf[i]);

		while (!SSI_Readable(SPI0_DEV));
		SSI_ReadData(SPI0_DEV); /* dummy read to generate clock */
	}
	RTK_LOGI(TAG, "Phase2: success\n");

	SSI_Cmd(SPI0_DEV, DISABLE);

	if (result) {
		RTK_LOGI(TAG, "SPI Demo: success\n");
	} else {
		RTK_LOGE(TAG, "SPI Demo: fail\n");
	};

	rtos_task_delete(NULL);
}

int example_spi_trx_polling_master(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_task"), (rtos_task_t)spi_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "%s Create spi_task task failed", __FUNCTION__);
	}
	return 0;
}
