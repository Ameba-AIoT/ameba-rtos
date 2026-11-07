/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "FLASH";

static u32 FLASH_PLLGet_ClockFreq(void)
{
	u8 spic_ckd;
	u32 flash_clk;
	u32 usb_pll_clk = GBSS_DEV->clk_info_bk.USBPLL_CLK;
	u32 sys_pll_clk = GBSS_DEV->clk_info_bk.SYSPLL_CLK;

	/* set spic clock src and div */
	spic_ckd = PLL_ClkSrcGet(sys_pll_clk, usb_pll_clk, CLK_LIMIT_SPIC);
	if (spic_ckd & IS_SYS_PLL) {
		flash_clk = sys_pll_clk / (2 * GET_CLK_DIV(spic_ckd));
	} else {
		flash_clk = usb_pll_clk / (2 * GET_CLK_DIV(spic_ckd));
	}
	RTK_LOGI(TAG, "FLASH CLK: %d Hz\n", flash_clk);

	/* save spic clock para into retention memory */
	IPC_SEMTake(IPC_SEM_RRAM, 0xffffffff);
	GBSS_DEV->clk_info_bk.spic_ckd = spic_ckd;
	IPC_SEMFree(IPC_SEM_RRAM);

	return flash_clk;
}

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

	cs_setup = MIN(MAX(SPIC_CS_SETUP_CYCLES(tcss_ns, bus_clk, flash_init_para.FLASH_baud_rate), 1), GET_CS_ACTIVE_SETUP(MASK_CS_ACTIVE_SETUP));
	cs_hold = MIN(SPIC_CS_HOLD_CYCLES(tchsh_ns, bus_clk, flash_init_para.FLASH_baud_rate), GET_CS_ACTIVE_HOLD(MASK_CS_ACTIVE_HOLD));
	cs_high = MIN(SPIC_CS_HIGH_CYCLES(tshsl_ns, bus_clk), GET_CS_H_RD_DUM_LEN(MASK_CS_H_RD_DUM_LEN));

	SPIC->TPR1 = (SPIC->TPR1 & ~MASK_CS_ACTIVE_SETUP) | CS_ACTIVE_SETUP(cs_setup);
	SPIC->TPR0 = (SPIC->TPR0 & ~(MASK_CS_ACTIVE_HOLD | MASK_CS_H_WR_DUM_LEN | MASK_CS_H_RD_DUM_LEN)) |
				 CS_ACTIVE_HOLD(cs_hold) | CS_H_WR_DUM_LEN(cs_high) | CS_H_RD_DUM_LEN(cs_high);
}

SRAMDRAM_ONLY_TEXT_SECTION
static void FLASH_PLLInit_ClockDiv(void)
{
	u8 spic_ckd = GBSS_DEV->clk_info_bk.spic_ckd;

	if (spic_ckd & IS_SYS_PLL) {
		RCC_PeriphClockDividerSet(SYS_PLL_SPIC, GET_CLK_DIV(spic_ckd));
		RCC_PeriphClockSourceSet(SPIC, SYS_PLL);
	} else {
		RCC_PeriphClockDividerSet(USB_PLL_SPIC, GET_CLK_DIV(spic_ckd));
		RCC_PeriphClockSourceSet(SPIC, USB_PLL);
	}
}

SRAMDRAM_ONLY_TEXT_SECTION
int flash_handshake_highspeed(void)
{
	u8 Dphy_Dly_Cnt = 3; /* DD recommend this value */
	int Ret = RTK_FAIL;

	/* Open handshake function */
	FLASH_Read_HandShake_Cmd(Dphy_Dly_Cnt, ENABLE);

	/* If XiP, code may in flash. Handshake should be enabled before set to PLL clk */
	FLASH_PLLInit_ClockDiv();

	if (FLASH_Read_DataIsRight(valid_img1_addr)) {
		Ret = RTK_SUCCESS;
	} else {
		/* should disable it, enbale it outside if needed */
		FLASH_Read_HandShake_Cmd(Dphy_Dly_Cnt, DISABLE);

		RCC_PeriphClockSourceSet(SPIC, XTAL);
	}

	RTK_LOGI(TAG, "FLASH HandShake %s\n", Ret == RTK_SUCCESS ? "OK" : "FAIL, use XTAL");
	return Ret;
}

/**
  * @brief  Switch the flash read bitmode, falling back until the data reads back correctly.
  * @param  spic_mode: the bitmode to try first, degraded down to Spic1IOBitMode on failure.
  * @param  flash_clk: the flash clock in Hz spic_mode will run at, 0 if the flash still
  *                    runs on the boot clock. Passed on to flash_nor_set_hpm_mode().
  * @retval RTK_SUCCESS or RTK_FAIL
  */
SRAMDRAM_ONLY_TEXT_SECTION
int flash_rx_mode_switch(u32 spic_mode, u32 flash_clk)
{
	int Ret = RTK_SUCCESS;
	char *str[] = {"1IO", "2O", "2IO", "4O", "4IO"};

	/* Try sequentially: 4IO, 4O, 2IO, 2O, 1bit */
	while (1) {
		/* Apply the vendor setting this bitmode & clock needs before probing it */
		flash_nor_set_hpm_mode(spic_mode, flash_clk);

		FLASH_Init(spic_mode);

		if (spic_mode == Spic1IOBitMode) {
			RTK_LOGE(TAG, "Flash Switch Read Mode FAIL\n");
			Ret = RTK_FAIL;
			break;
		}

		if (FLASH_Read_DataIsRight(valid_img1_addr)) {
			RTK_LOGI(TAG, "Flash Read %s\n", str[spic_mode]);
			break;
		}

		spic_mode--;
	}

	return Ret;
}


SRAMDRAM_ONLY_TEXT_SECTION
void flash_highspeed_setup(void)
{
	uint32_t irq_status;
	u32 read_mode, Temp;
	u32 flash_clk = 0;
	read_mode = flash_get_readmode(Flash_ReadMode);

	irq_status = irq_disable_save();

	/* SPIC stay in BUSY state when there are more than 0x1_0000 cycles between two input data.
	 * Disable DREIR to avoid that interrupt hanler time is lager than 0x1_0000 SPIC cycles.
	 */
	SPIC->CTRLR0 |= BIT_SPI_DREIR_R_DIS;

	/* Get flash ID to reinitialize FLASH_InitTypeDef structure */
	flash_nor_get_vendor();

	/* Set flash status register: set QE, clear protection bits */
	flash_nor_set_qe_register();

	if (SYSCFG_CHIPType_Get() != CHIP_TYPE_FPGA) {
		/* Calculate flash actual freq according to CLK_LIMIT_SPIC. The dummy cycle and
		   the flash side setting it implies are handled by flash_nor_set_hpm_mode() */
		flash_clk = FLASH_PLLGet_ClockFreq();

		/* CS timing for the PLL clock, in place before the clock is switched */
		flash_cs_timing_config(flash_clk * 2 * flash_init_para.FLASH_baud_rate);
	}

	/* Set flash I/O mode and high-speed calibration */
	flash_rx_mode_switch(read_mode, flash_clk);

	/* Two SPIC in Dplus, SPIC0(Boot) Can connect to S0 or S1, when SPIC0 select one, SPIC1(Combo) use another one */
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);

	/* RL6955 LSYS_BIT_FLASH_FEEDBACK_LOC bit define is different from rl7005 project */
	if (PINMUX_EXTERNAL == flash_init_para.FLASH_pinmux) {
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, Temp | LSYS_BIT_FLASH_FEEDBACK_LOC);
	} else {
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, Temp & ~LSYS_BIT_FLASH_FEEDBACK_LOC);
	}

	/* Enable handshake to speed up simulation */
	if (SYSCFG_CHIPType_Get() != CHIP_TYPE_FPGA) {
		if (flash_handshake_highspeed() != RTK_SUCCESS) {
			/* fell back to XTAL */
			flash_cs_timing_config(XTAL_ClkGet());
		}
	}

	irq_enable_restore(irq_status);
}

/* init psramc for flash r/w if needed */
void Combo_SPIC_Init(void)
{
	SPIC_TypeDef *ComboSpic = SPIC_COMBO;
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;

#if !defined(CONFIG_SECOND_FLASH_NOR)
	return;
#endif

	RCC_PeriphClockCmd(APBPeriph_PSRAM, APBPeriph_PSRAM_CLOCK, ENABLE);
	RCC_PeriphClockSourceSet(PSRAM, XTAL);

	/*1. Disable psram phy */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL) | LSYS_BIT_PSRAMC_FLASH_EN);

	/*2. ComboSpic pinmux set which is contrary to bootSpic */
	if (PINMUX_EXTERNAL == FLASH_InitStruct->FLASH_pinmux) {
		Pinmux_ComboSpicCtrl(PINMUX_INTERNAL, ON);
	} else {
		Pinmux_ComboSpicCtrl(PINMUX_EXTERNAL, ON);
	}

	/*3. use bootSpic para to config ComboSpic(Default NorFlash Config) where 1IO autoread is common for all flash */
#if defined(CONFIG_SECOND_FLASH_NOR)
	FLASH_Config(ComboSpic, FLASH_InitStruct);
	/* Set ComboSpic 1IO Mode */
	FLASH_SetSpiMode(ComboSpic, FLASH_InitStruct, Spic1IOBitMode);
#endif
	/* SPIC stay in BUSY state when there are more than 0x1_0000 cycles between two input data.
	 * Disable DREIR to avoid that interrupt hanler time is lager than 0x1_0000 SPIC cycles.
	 */
	ComboSpic->CTRLR0 |= BIT_SPI_DREIR_R_DIS;
}
