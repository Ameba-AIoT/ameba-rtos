/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"
#include "ameba_gspi.h"
#include "example_gspi_ext.h"
#include "os_wrapper.h"

#define TAG "GSPI_HOST"

/* ---------- Tunable test parameters ---------- */
#define GSPI_BUS_FREQ_HZ       10000000   /* bus clock after activation, max 25MHz */
#define GSPI_FREQ_ACTIVATE     1000000    /* clock used for the activation frame */
#define GSPI_BOOT_DELAY_MS     3000       /* let the Device bring up its SPDIO backend */

static GSPI_HandleTypeDef gspi;

/* Read one register and report the Status phase that came with it. */
static void gspi_show_reg(const char *name, u32 addr)
{
	GSPI_StatusTypeDef sts;
	u32 val = GSPI_ReadReg32(&gspi, addr, &sts);

	RTK_LOGI(TAG, "  %s(0x%02x) = %08x   status %08x %08x %s\n", name, addr, val,
			 sts.word0, sts.word1, sts.valid ? "" : "(not driven)");
}

static void gspi_host_task(void)
{
	GSPI_InitTypeDef init;
	GSPI_StatusTypeDef sts;
	u8 cpu_ind;
	u8 saved, readback;

	/* GSPI_Init configures SPI0, the per-signal pinmux and the CS GPIO, and
	 * latches CS high before enabling its output. */
	GSPI_StructInit(&init);
	init.GSPI_Index = 0;
	init.GSPI_ClkPin = GSPI_SCLK;
	init.GSPI_MosiPin = GSPI_MOSI;
	init.GSPI_MisoPin = GSPI_MISO;
	init.GSPI_CsPin = GSPI_CS;
	init.GSPI_ClkFreq = GSPI_BUS_FREQ_HZ;
	init.GSPI_ClkFreqInit = GSPI_FREQ_ACTIVATE;
	if (GSPI_Init(&gspi, &init) != GSPI_OK) {
		RTK_LOGE(TAG, "GSPI_Init failed\n");
		goto exit;
	}

	Pinmux_Config(GSPI_INT, PINMUX_FUNCTION_GPIO);   /* input, not used here */

	rtos_time_delay_ms(GSPI_BOOT_DELAY_MS);

	/* Step 1: activation. This must be the first bus transaction after the
	 * Device boots, otherwise the Device latches SDIO mode. */
	RTK_LOGI(TAG, "Step1: activate GSPI (write SPI_CFG, read back)\n");
	if (GSPI_Configuration(&gspi, GSPI_BIG_ENDIAN_32) != GSPI_OK) {
		RTK_LOGE(TAG, "GSPI Demo: fail (activation)\n");
		goto exit;
	}
	RTK_LOGI(TAG, "  activated, bus now %dHz\n", GSPI_BUS_FREQ_HZ);

	/* Step 2: read the registers that prove the bus really talks. CPU_IND
	 * reading 0xFF means MISO is just idling at its pull-up level. */
	RTK_LOGI(TAG, "Step2: register reads\n");
	cpu_ind = GSPI_ReadReg8(&gspi, GSPI_REG_CPU_IND, &sts);
	RTK_LOGI(TAG, "  CPU_IND(0x87) = 0x%02x, CPU_RDY=%d\n", cpu_ind, cpu_ind & GSPI_BIT_CPU_RDY);
	if (cpu_ind == 0xFF) {
		RTK_LOGE(TAG, "GSPI Demo: fail (CPU_IND 0xFF, bus not talking)\n");
		goto exit;
	}
	gspi_show_reg("HISR      ", GSPI_REG_HISR);
	gspi_show_reg("RX0_REQ   ", GSPI_REG_RX0_REQ_LEN);
	gspi_show_reg("FREE_TX_BD", GSPI_REG_FREE_TX_BD_NUM);
	gspi_show_reg("HCPWM     ", GSPI_REG_HCPWM);

	/* Step 3: prove the write path too. HPS_CLKR (0x84) is documented "not
	 * used", so it is the one safe scratch byte; it must be written byte-wise
	 * because CPU_IND shares its 32-bit word. */
	RTK_LOGI(TAG, "Step3: write-readback on HPS_CLKR(0x84)\n");
	saved = GSPI_ReadReg8(&gspi, GSPI_REG_HPS_CLKR, NULL);
	GSPI_WriteReg8(&gspi, GSPI_REG_HPS_CLKR, 0x5A, NULL);
	readback = GSPI_ReadReg8(&gspi, GSPI_REG_HPS_CLKR, NULL);
	GSPI_WriteReg8(&gspi, GSPI_REG_HPS_CLKR, saved, NULL);
	RTK_LOGI(TAG, "  wrote 0x5a, read 0x%02x %s (restored 0x%02x)\n",
			 readback, (readback == 0x5A) ? "MATCH" : "MISMATCH", saved);

	if (readback == 0x5A) {
		RTK_LOGI(TAG, "GSPI Demo: success\n");
	} else {
		RTK_LOGE(TAG, "GSPI Demo: fail (register write not observed)\n");
	}

exit:
	rtos_task_delete(NULL);
}

int example_raw_gspi_host_min(void)
{
	if (rtos_task_create(NULL, ((const char *)"gspi_host_task"), (rtos_task_t)gspi_host_task,
						 NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "\n\r%s Create gspi_host_task task failed", __FUNCTION__);
	}

	return 0;
}
