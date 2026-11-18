/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#define FLASH_CALIBRATION_DEBUG		0

#define FLASH_CS_HIGH_MIN			12

static const char *const TAG = "FLASH";

/**
  * @brief  Set the SPIC CS timing for the given SPIC bus clock from the NOR flash CS timing.
  * @param  bus_clk: the SPIC bus clock in Hz it is going to run at, spi_sclk = bus_clk / (2 * BAUDR).
  * @note   Must be set before switching to that clock. CS high keeps at least the original 12 SPIC
  *         cycles. Values beyond the register field are clamped: hold 0~15, CS high 0~63.
  * @retval None
  */
static void flash_cs_timing_config(u32 bus_clk)
{
	u32 tcss_ns, tchsh_ns, tshsl_ns;
	u32 cs_hold, cs_high;

	flash_nor_get_cs_timing(&tcss_ns, &tchsh_ns, &tshsl_ns);

	cs_hold = SPIC_CS_HOLD_CYCLES(tchsh_ns, bus_clk, flash_init_para.FLASH_baud_rate);
	cs_high = SPIC_CS_HIGH_CYCLES(tshsl_ns, bus_clk);

	cs_hold = MIN(cs_hold, GET_CS_ACTIVE_HOLD(MASK_CS_ACTIVE_HOLD));
	cs_high = MIN(MAX(cs_high, FLASH_CS_HIGH_MIN), GET_CS_H_RD_DUM_LEN(MASK_CS_H_RD_DUM_LEN));

	SPIC->TPR0 = (SPIC->TPR0 & ~(MASK_CS_ACTIVE_HOLD | MASK_CS_H_WR_DUM_LEN | MASK_CS_H_RD_DUM_LEN)) |
				 CS_ACTIVE_HOLD(cs_hold) | CS_H_WR_DUM_LEN(cs_high) | CS_H_RD_DUM_LEN(cs_high);
}

void flash_clksrc_sel(FlashDivInt_E div, u8 isCPUPLL, u8 is_handshake)
{
	if (isCPUPLL == BIT_LSYS_CKSL_SPIC_CPUPLL) {
		/*HW need DSPPLL & CPUPLL open*/
		FLASH_CalibrationPLLSel(FALSE);
		FLASH_Calibration_PSPLL_Open();
		if (is_handshake) {
			/* clock gate for ps_pll */
			FLASH_CalibrationPLLPSCmd(DISABLE);
		}

		/* SPIC clock switch to PLLM */
		FLASH_CalibrationPLLSel(TRUE);
		FLASH_PLLInit_ClockDiv(div);
		if (is_handshake) {
			/* clock gate for ps_pll */
			FLASH_CalibrationPLLPSCmd(DISABLE);
		}
	} else {
		/*HW need DSPPLL & CPUPLL open*/
		FLASH_CalibrationPLLSel(TRUE);
		FLASH_Calibration_PSPLL_Open();
		if (is_handshake) {
			/* clock gate for ps_pll */
			FLASH_CalibrationPLLPSCmd(DISABLE);
		}

		/* SPIC clock switch to PLLD */
		FLASH_CalibrationPLLSel(FALSE);
		FLASH_PLLInit_ClockDiv(div);
		if (is_handshake) {
			/* clock gate for ps_pll */
			FLASH_CalibrationPLLPSCmd(DISABLE);
		}
	}
}

SRAMDRAM_ONLY_TEXT_SECTION
int flash_handshake_highspeed(FlashDivInt_E div, u32 flash_clk)
{
	int Ret = RTK_SUCCESS;
	u8 spic_mode = flash_init_para.FLASH_cur_bitmode;
	u8 Dphy_Dly_Cnt = 3; /* DD recommend this value */

	/* The flash side of HPM has to be in place before the clock is raised. If it
	 * cannot be set the flash stays on the boot clock. */
	if (flash_nor_set_dc(flash_clk) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Flash HPM set fail, use XTAL\n");
		return RTK_FAIL;
	}

	flash_init_para.debug = FLASH_CALIBRATION_DEBUG;
	flash_clksrc_sel(div, BIT_LSYS_CKSL_SPIC_CPUPLL, TRUE);

	RSIP_MMU_Cache_Clean(); 	/* Clean Cache in RSIP when otf enabled */

	if (FLASH_Read_HandShake(&flash_init_para, spic_mode, Dphy_Dly_Cnt) == TRUE) {
		FLASH_Read_HandShake_Cmd(Dphy_Dly_Cnt, ENABLE);
		flash_init_para.FLASH_rd_sample_dly_cycle_cal = Dphy_Dly_Cnt + 2;
		flash_init_para.FLASH_rd_sample_dly_cycle = flash_init_para.FLASH_rd_sample_dly_cycle_cal;

		/* this code is rom code, so it is safe */
		FLASH_Init(spic_mode);

		RTK_LOGI(TAG, "FLASH HandShake[0x%lx OK]\n", div);
	} else {
		RCC_PeriphClockSource_SPIC(BIT_LSYS_CKSL_SPIC_LBUS);
		RTK_LOGE(TAG, "FLASH HandShake[0x%lx FAIL]\n", div);
		Ret = RTK_FAIL;
	}

#if FLASH_CALIBRATION_DEBUG
	BKUP_Write(BKUP_REG3, Ret);
#endif

	return Ret;
}

static u8 flash_get_option(u32 sys_data, bool is_speed)
{
	u16 tmp = (u16)BIT15;
	u8 cnt = 0;

	while (tmp) {
		if ((sys_data & tmp) != 0) {
			break;
		} else {
			tmp = tmp >> 1;
			cnt++;
		}
	}

	/* cnt =0 --> return FLASH_CLK_DIV10
	 * cnt =1 --> return FLASH_CLK_DIV9
	 * ...
	 * cnt>=8 --> return FLASH_CLK_DIV2
	*/
	if (is_speed) {
		if (cnt > 8) {
			cnt = 8;
		}

		cnt = FLASH_CLK_DIV10 - cnt;
	}

	return cnt;
}

/**
  * @brief  Switch the flash read bitmode, falling back until the data reads back correctly.
  * @param  spic_mode: the bitmode to try first, degraded down to Spic1IOBitMode on failure.
  * @param  flash_clk: the flash clock in Hz spic_mode will run at. Passed on to flash_nor_set_hpm_mode().
  * @retval RTK_SUCCESS if the flash reads back in spic_mode, RTK_FAIL otherwise
  */
SRAMDRAM_ONLY_TEXT_SECTION
int flash_rx_mode_switch(u32 spic_mode, u32 flash_clk)
{
	u32 req_mode = spic_mode;
	u32 pdata[2];
	char *str[] = {"1IO", "2O", "2IO", "4O", "4IO"};

	/* Try sequentially: 4IO, 4O, 2IO, 2O, 1bit */
	do {
		/* 4IO or 2IO should enable dummy byte function */
		if ((spic_mode == SpicQuadIOBitMode) || (spic_mode == SpicDualIOBitMode)) {
			flash_init_para.FLASH_dum_en = 1;
		}

		/* Apply the vendor setting this bitmode & clock needs before probing it */
		flash_nor_set_hpm_mode(spic_mode, flash_clk);

		flash_init_para.FLASH_rd_sample_dly_cycle = SPIC_LOWSPEED_SAMPLE_PHASE;
		FLASH_Init(spic_mode);

		DCache_Invalidate(SPI_FLASH_BASE, 8);
		RSIP_MMU_Cache_Clean();		/* Clean Cache in RSIP when otf enabled */
		pdata[0] = HAL_READ32(SPI_FLASH_BASE, 0x00);
		pdata[1] = HAL_READ32(SPI_FLASH_BASE, 0x04);

		if (_memcmp(pdata, SPIC_CALIB_PATTERN, 8) == 0) {
			break;
		}
	} while (spic_mode--);

	if (spic_mode != req_mode) {
		RTK_LOGE(TAG, "Flash Read %s not reached\n", str[req_mode]);
		return RTK_FAIL;
	}

	RTK_LOGI(TAG, "Flash Read %s\n", str[spic_mode]);
	return RTK_SUCCESS;
}

/**
  * @brief  Set the flash read mode and clock.
  * @param  read_mode: the read bitmode to set.
  * @param  spic_max: the SPIC bus clock upper limit in Hz. The fastest PLL divider not above it is used.
  * @note   Fail leaves the flash readable (fallback bitmode and/or boot clock), the request is not met.
  * @retval RTK_SUCCESS if the flash is in read_mode and calibrated at the selected clock, RTK_FAIL otherwise
  */
int flash_highspeed_setup(u32 read_mode, u32 spic_max)
{
	uint32_t irq_status;
	u8 flash_speed;
	u8 spic_ckd;
	int Ret;
	u32 pllm_clk = PLL_ClkGet(CLK_CPU_MPLL);
	u32 flash_clk;

	flash_speed = flash_get_option(Flash_Speed, TRUE);

	spic_ckd = CLKDIV_ROUND_UP(pllm_clk, spic_max) - 1;
	flash_speed = MAX(flash_speed, spic_ckd);

	irq_status = irq_disable_save();

	/* SPIC stay in BUSY state when there are more than 0x1_0000 cycles between two input data.
	 * Disable DREIR to avoid that interrupt hanler time is lager than 0x1_0000 SPIC cycles.
	 */
	SPIC_TypeDef *spi_flash = SPIC;
	spi_flash->CTRLR0 |= BIT_SPI_DREIR_R_DIS;

	/* set tSHSL (CS_H) to min 60ns (12 spic_clk) */
	spi_flash->TPR0 = (spi_flash->TPR0 & ~MASK_CS_H_RD_DUM_LEN) | CS_H_RD_DUM_LEN(12);

	/* if 8 auto rd wrap ever occur, and user rd + auto rd wrap is together, SPIC will return 0xFFFFFFFF only in Lite. */
	/* Set DIS_WRAP_ALIGN = 1, SPIC will chop wrap transation into two incremental transactions to avoid corner case.*/
	spi_flash->CTRLR2 |= BIT_DIS_WRAP_ALIGN;
	/* use uncacheable read (no wrap) to restore to normal function */
	DCache_Invalidate(SPI_FLASH_BASE, 8);
	RSIP_MMU_Cache_Clean(); 	/* Clean Cache in RSIP when otf enabled */
	HAL_READ32(SPI_FLASH_BASE, 0x00);

	/* Get flash ID to reinitialize FLASH_InitTypeDef structure */
	flash_nor_get_vendor();

	/* Set flash status register: set QE, the 4IO read mode below needs it */
	flash_nor_set_qe_register();

	/* Set flash I/O mode on the boot clock. The flash still runs on the boot clock here, so no High Performance Mode yet */
	Ret = flash_rx_mode_switch(read_mode, 0);

	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) {
		goto exit;
	}

	flash_clk = pllm_clk / (2 * (flash_speed + 1));
	RTK_LOGI(TAG, "FLASH CLK: %d Hz\n", flash_clk);

	/* The flash cannot run at that clock even with its DC bit set: stay on the boot clock */
	if (flash_nor_check_clk(flash_clk) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Flash out of spec at %d Hz, use boot clock\n", flash_clk);
		Ret = RTK_FAIL;
		goto exit;
	}

	/* CS timing for the PLL clock, in place before the clock is switched */
	flash_cs_timing_config(flash_clk * 2 * flash_init_para.FLASH_baud_rate);

	/* A failure leaves the flash on the boot clock */
	if (flash_handshake_highspeed(flash_speed, flash_clk) != RTK_SUCCESS) {
		Ret = RTK_FAIL;
	}

exit:
	irq_enable_restore(irq_status);
	return Ret;
}
