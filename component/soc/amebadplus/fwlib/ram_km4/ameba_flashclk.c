/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#define FLASH_CALIBRATION_DEBUG		0

#define FLASH_CS_SETUP_MIN			2
#define FLASH_CS_HIGH_MIN			12

static const char *const TAG = "FLASH";

/**
  * @brief  Set the SPIC CS timing for the given SPIC bus clock from the NOR flash CS timing.
  * @param  bus_clk: the SPIC bus clock in Hz it is going to run at. TPR0/TPR1 count bus_clk
  *                  cycles, spi_sclk = bus_clk / (2 * BAUDR).
  * @note   Must be set before switching to that clock. Values beyond the register field
  *         are clamped: setup 1~15 (0 would be ignored), hold 0~15, CS high 0~63.
  * @retval None
  */
SRAMDRAM_ONLY_TEXT_SECTION
static void flash_cs_timing_config(u32 bus_clk)
{
	u32 tcss_ns, tchsh_ns, tshsl_ns;
	u32 cs_setup, cs_hold, cs_high;

	flash_nor_get_cs_timing(&tcss_ns, &tchsh_ns, &tshsl_ns);

	cs_setup = SPIC_CS_SETUP_CYCLES(tcss_ns, bus_clk, flash_init_para.FLASH_baud_rate);
	cs_hold = SPIC_CS_HOLD_CYCLES(tchsh_ns, bus_clk, flash_init_para.FLASH_baud_rate);
	cs_high = SPIC_CS_HIGH_CYCLES(tshsl_ns, bus_clk);

	/* Never less margin than the original driver, which keeps CS setup 2 and CS high 12 on the PLL clock */
	cs_setup = MIN(MAX(cs_setup, FLASH_CS_SETUP_MIN), GET_CR_ACTIVE_SETUP(MASK_CR_ACTIVE_SETUP));
	cs_hold = MIN(cs_hold, GET_CS_ACTIVE_HOLD(MASK_CS_ACTIVE_HOLD));
	cs_high = MIN(MAX(cs_high, FLASH_CS_HIGH_MIN), GET_CS_H_RD_DUM_LEN(MASK_CS_H_RD_DUM_LEN));

	SPIC->TPR1 = (SPIC->TPR1 & ~MASK_CR_ACTIVE_SETUP) | CR_ACTIVE_SETUP(cs_setup);
	SPIC->TPR0 = (SPIC->TPR0 & ~(MASK_CS_ACTIVE_HOLD | MASK_CS_H_WR_DUM_LEN | MASK_CS_H_RD_DUM_LEN)) |
				 CS_ACTIVE_HOLD(cs_hold) | CS_H_WR_DUM_LEN(cs_high) | CS_H_RD_DUM_LEN(cs_high);
}

SRAMDRAM_ONLY_TEXT_SECTION
int flash_handshake_highspeed(u32 div, u32 flash_clk)
{
	u8 Dphy_Dly_Cnt = 3; /* DD recommend this value */
	int Ret = RTK_FAIL;

	/* The flash side of HPM has to be in place before the clock is raised. If it
	 * cannot be set the flash stays on the boot clock. */
	if (flash_nor_set_dc(flash_clk) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Flash HPM set fail, use XTAL\n");
		return RTK_FAIL;
	}

	FLASH_PLLInit_ClockDiv(div);

	/* Open handshake function */
	FLASH_Read_HandShake_Cmd(Dphy_Dly_Cnt, ENABLE);

	if (FLASH_Read_DataIsRight()) {
		Ret = RTK_SUCCESS;
	} else {
		/* should disable it, enbale it outside if needed */
		FLASH_Read_HandShake_Cmd(Dphy_Dly_Cnt, DISABLE);
		RCC_PeriphClockSource_SPIC(BIT_LSYS_CKSL_SPIC_XTAL);
		SPIC->TPR1 = (SPIC->TPR1 & ~MASK_CR_ACTIVE_SETUP) | CR_ACTIVE_SETUP(1);
	}

	RTK_LOGI(TAG, "FLASH HandShake[0x%x %s]\n", div, Ret == RTK_SUCCESS ? "OK" : "FAIL, use XTAL");
	return Ret;
}

SRAMDRAM_ONLY_TEXT_SECTION
int flash_rx_mode_switch(u32 spic_mode, u32 flash_clk)
{
	int Ret = RTK_SUCCESS;
	char *str[] = {"1IO", "2O", "2IO", "4O", "4IO"};
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;

	/* Try sequentially: 4IO, 4O, 2IO, 2O, 1bit */
	while (1) {
		/* 4IO or 2IO should enable dummy byte function */
		if ((spic_mode == Spic4IOBitMode) || (spic_mode == Spic2IOBitMode)) {
			FLASH_InitStruct->FLASH_dum_en = 1;
		} else {
			FLASH_InitStruct->FLASH_dum_en = 0;
		}

		/* Apply the vendor setting this bitmode & clock needs before probing it */
		flash_nor_set_hpm_mode(spic_mode, flash_clk);

		FLASH_Init(spic_mode);

		if (spic_mode == Spic1IOBitMode) {
			RTK_LOGE(TAG, "Flash Switch Read Mode FAIL\n");
			Ret = RTK_FAIL;
			break;
		}

		if (FLASH_Read_DataIsRight()) {
			RTK_LOGI(TAG, "Flash Read %s\n", str[spic_mode]);
			break;
		}

		spic_mode--;
	}

	return Ret;
}

void flash_pad_low_drv_strength(u32 PinLocation)
{
	u8 s0_pin[6] = {_PA_13, _PA_14, _PA_15, _PA_16, _PA_17, _PA_18};
	u8 s1_pin[6] = {_PA_0, _PA_1, _PA_2, _PA_3, _PA_4, _PA_5};
	u8 *sX_pin = NULL;

	if (PinLocation == PINMUX_S0) {
		sX_pin = s0_pin;
	} else {
		sX_pin = s1_pin;
	}

	for (int i = 0; i < 6; i++) {
		PAD_DrvStrength(sX_pin[i], PAD_DRV_ABILITITY_LOW);
	}
}

void flash_highspeed_setup(void)
{
	uint32_t irq_status;

	u8 read_mode, flash_speed;
	u32 Temp, spic_ckd;
	u32 pll_clk = PLL_ClkGet();
	/* Boot clock first: the flash runs at XTAL / 2 until the PLL clock is taken */
	u32 flash_clk = XTAL_ClkGet() / 2;

	read_mode = flash_get_readmode(Flash_ReadMode);
	flash_speed = Flash_Speed - 1;

	spic_ckd = CLKDIV_ROUND_UP(pll_clk, SPIC_CLK_LIMIT) - 1;
	flash_speed = MAX(flash_speed, spic_ckd);

	irq_status = irq_disable_save();

	/* SPIC stay in BUSY state when there are more than 0x1_0000 cycles between two input data.
	 * Disable DREIR to avoid that interrupt hanler time is lager than 0x1_0000 SPIC cycles.
	 */
	SPIC->CTRLR0 |= BIT_SPI_DREIR_R_DIS;

	/* Note: some full voltage range Flash e.g.GD25WQ64E need 10ns
	   HW recommend: at leaset 10ns */
	SPIC->TPR1 = (SPIC->TPR1 & ~MASK_CR_ACTIVE_SETUP) | CR_ACTIVE_SETUP(2);

	/* set tSHSL (CS_H) to min 60ns (12 spic_clk) */
	SPIC->TPR0 = (SPIC->TPR0 & ~MASK_CS_H_RD_DUM_LEN) | CS_H_RD_DUM_LEN(12);

	/* Get flash ID to reinitialize FLASH_InitTypeDef structure */
	flash_nor_get_vendor();

	/* Set flash status register: set QE, the 4IO read mode below needs it */
	flash_nor_set_qe_register();

	/* Set flash I/O mode and high-speed calibration on the boot clock, FPGA works in 4IO as well */
	flash_rx_mode_switch(read_mode, flash_clk);

	/* Two SPIC in Dplus, SPIC0(Boot) Can connect to S0 or S1, when SPIC0 select one, SPIC1(Combo) use another one
	   Set Drive Strength to Low, to reduce RF interference.*/
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);
	if (PINMUX_S0 == flash_init_para.FLASH_pinmux) {
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, Temp & ~LSYS_BIT_FLASH_FEEDBACK_LOC);
		flash_pad_low_drv_strength(PINMUX_S0);
	} else {
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, Temp | LSYS_BIT_FLASH_FEEDBACK_LOC);
		flash_pad_low_drv_strength(PINMUX_S1);
	}

	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) {
		goto exit;
	}

	flash_clk = pll_clk / (2 * (flash_speed + 1));
	RTK_LOGI(TAG, "FLASH CLK: %d Hz\n", flash_clk);

	/* The flash cannot run at that clock even with its DC bit set: stay on XTAL */
	if (flash_nor_check_clk(flash_clk) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Flash out of spec at %d Hz, use XTAL\n", flash_clk);
		goto exit;
	}

	/* CS timing for the PLL clock, in place before the clock is switched */
	flash_cs_timing_config(flash_clk * 2 * flash_init_para.FLASH_baud_rate);

	/* Enable handshake to speed up simulation, a failure leaves the flash on XTAL */
	flash_handshake_highspeed(flash_speed, flash_clk);

exit:
	irq_enable_restore(irq_status);
}

/* init psramc for flash r/w if needed */
void Combo_SPIC_Init(void)
{
	SPIC_TypeDef *ComboSpic = SPIC_COMBO;
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;

	RCC_PeriphClockCmd(APBPeriph_PSRAM, APBPeriph_PSRAM_CLOCK, ENABLE);
	RCC_PeriphClockSource_PSRAM(BIT_LSYS_CKSL_PSRAM_LBUS);

	/*1. Disable psram phy */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL) | LSYS_BIT_PSRAMC_FLASH_EN);

	/*2. ComboSpic pinmux set which is contrary to bootSpic.
		 Set Drive Strength to Low, to reduce RF interference.
	*/
	if (PINMUX_S0 == FLASH_InitStruct->FLASH_pinmux) {
		Pinmux_ComboSpicCtrl(PINMUX_S1, ON);
		flash_pad_low_drv_strength(PINMUX_S1);
	} else {
		Pinmux_ComboSpicCtrl(PINMUX_S0, ON);
		flash_pad_low_drv_strength(PINMUX_S0);
	}

	/*3. use bootSpic para to config ComboSpic(Default NorFlash Config) where 1IO autoread is common for all flash */
	FLASH_Config(ComboSpic, FLASH_InitStruct);

	/* Set ComboSpic 1IO Mode */
	FLASH_SetSpiMode(ComboSpic, FLASH_InitStruct, Spic1IOBitMode);

	/* SPIC stay in BUSY state when there are more than 0x1_0000 cycles between two input data.
	 * Disable DREIR to avoid that interrupt hanler time is lager than 0x1_0000 SPIC cycles.
	 */
	ComboSpic->CTRLR0 |= BIT_SPI_DREIR_R_DIS;
}

void Combo_SPIC_Output_Div_Clk(u32 Div)
{
	u32 temp, timeout = 10;

	/* disable clock div */
	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);
	temp &= ~LSYS_BIT_PSRAMC_FLASH_DIV_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, temp);

	/* set clock div */
	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);
	temp &= ~LSYS_MASK_PSRAMC_FLASH_DIV_INT;
	temp |= LSYS_PSRAMC_FLASH_DIV_INT(Div);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, temp);

	/* enable clock div */
	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);
	temp |= LSYS_BIT_PSRAMC_FLASH_DIV_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, temp);

	/* wait ready flag of Flash clock with phase shift, 65 ns cost */
	while (timeout > 0) {
		timeout--;
		temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);
		if (temp & LSYS_BIT_PSRAMC_FLASH_DIV_RDY) {
			break;
		}
	}

	/* Select source clock firstly, then div module can work corectly */
	RCC_PeriphClockSource_PSRAM(BIT_LSYS_CKSL_PSRAM_CPUPLL);
}
