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
#define TAG "SPI_SLAVE"

/* compatible pinmux_funcid_name with RTL872xD */
#ifndef CONFIG_AMEBAD
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RTL8720F)
#define PINMUX_FUNCTION_SPIM	PINMUX_FUNCTION_SPI0
#else
#define PINMUX_FUNCTION_SPIM	PINMUX_FUNCTION_SPI
#endif
#endif

#define DataFrameSize	DFS_8_BITS
#define TEST_BUF_SIZE	512

u8 SlaveTxBuf[TEST_BUF_SIZE];
u8 SlaveRxBuf[TEST_BUF_SIZE];

void spi_task(void)
{
	int result = 1;
	int i = 0;

	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(5000);
	Pinmux_Swdoff();

	/* init SPI0 as Slave */
	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	Pinmux_Config(SPI_MOSI, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_MISO, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_SCLK, PINMUX_FUNCTION_SPIM);
	Pinmux_Config(SPI_CS, PINMUX_FUNCTION_SPIM);
	PAD_PullCtrl((u32)SPI_CS, GPIO_PuPd_UP);
	PAD_PullCtrl((u32)SPI_SCLK, GPIO_PuPd_DOWN);

	SSI_SetRole(SPI0_DEV, SSI_SLAVE);
	SSI_InitTypeDef SSI_InitStructS;
	SSI_StructInit(&SSI_InitStructS);
	SSI_InitStructS.SPI_Role = SSI_SLAVE;
	SSI_InitStructS.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
	SSI_InitStructS.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
	SSI_InitStructS.SPI_DataFrameNumber = DataFrameSize;
	SSI_Init(SPI0_DEV, &SSI_InitStructS);

	/* fill TX buffer with pattern */
	_memset(SlaveTxBuf, 0, TEST_BUF_SIZE);
	_memset(SlaveRxBuf, 0, TEST_BUF_SIZE);
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		SlaveTxBuf[i] = i;
	}

	/**
	 * Phase 1: Slave read/write (full duplex)
	 * Preload first TX byte to align master's read.
	 */
	RTK_LOGI(TAG, "---Phase1: Slave read/write---\n");
	int phase1_ok = 1;

	/* preload first TX byte */
	while (!SSI_Writeable(SPI0_DEV));
	SSI_WriteData(SPI0_DEV, SlaveTxBuf[0]);

	for (i = 0; i < TEST_BUF_SIZE; i++) {
		while (!SSI_Readable(SPI0_DEV));
		SlaveRxBuf[i] = SSI_ReadData(SPI0_DEV);

		if (SlaveRxBuf[i] != SlaveTxBuf[i]) {
			phase1_ok = 0;
		}

		/* write next byte for master to read (skip after last byte) */
		if (i < TEST_BUF_SIZE - 1) {
			while (!SSI_Writeable(SPI0_DEV));
			SSI_WriteData(SPI0_DEV, SlaveTxBuf[i + 1]);
		}
	}
	if (phase1_ok) {
		RTK_LOGI(TAG, "Phase1: success\n");
	} else {
		RTK_LOGE(TAG, "Phase1: fail\n");
	};

	/**
	 * Phase 2: Slave read (Master write) - unidirectional
	 */
	RTK_LOGI(TAG, "---Phase2: Slave read---\n");
	int phase2_ok = 1;
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		while (!SSI_Readable(SPI0_DEV));
		SlaveRxBuf[i] = SSI_ReadData(SPI0_DEV);

		if (SlaveRxBuf[i] != (i & 0xFF)) {
			phase2_ok = 0;
		}
	}
	if (phase2_ok) {
		RTK_LOGI(TAG, "Phase2: success\n");
	} else {
		RTK_LOGE(TAG, "Phase2: fail\n");
	};

	SSI_Cmd(SPI0_DEV, DISABLE);

	result = phase1_ok && phase2_ok;
	if (result) {
		RTK_LOGI(TAG, "SPI Demo: success\n");
	} else {
		RTK_LOGE(TAG, "SPI Demo: fail\n");
	};

	rtos_task_delete(NULL);
}

int example_spi_trx_polling_slave(void)
{
	if (rtos_task_create(NULL, ((const char *)"spi_task"), (rtos_task_t)spi_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "%s Create spi_task task failed", __FUNCTION__);
	}
	return 0;
}
