/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#define FLASH_CALIBRATION_DEBUG		0

static FlashInfo_TypeDef *current_IC;

/* Flag to check configuration register or not. Necessary for wide-range VCC MXIC flash */
static u8 check_config_reg = 0;

static const char *TAG = "FLASH";

SRAMDRAM_ONLY_TEXT_SECTION
u32 flash_handshake_highspeed(u32 div)
{
	u8 Dphy_Dly_Cnt = 3; /* DD recommend this value */
	u32 Ret = _TRUE;

	FLASH_PLLInit_ClockDiv(div);

	/* Open handshake function */
	FLASH_Read_HandShake_Cmd(Dphy_Dly_Cnt, ENABLE);

	if (FLASH_Read_DataIsRight()) {
	} else {
		/* should disable it, enbale it outside if needed */
		FLASH_Read_HandShake_Cmd(Dphy_Dly_Cnt, DISABLE);
		RCC_PeriphClockSource_SPIC(BIT_LSYS_CKSL_SPIC_XTAL);
		SPIC->TPR1 = (SPIC->TPR1 & ~MASK_CR_ACTIVE_SETUP) | CR_ACTIVE_SETUP(1);
		Ret = _FALSE;
	}

	RTK_LOGI(TAG, "FLASH HandShake[0x%x %s]\n", div, Ret ? "OK" : "FAIL");
	return Ret;
}

static void flash_get_vendor(void)
{
	u8 flash_ID[4];
	u32 flash_capacity = 0;
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;

	/* Read flash ID */
	FLASH_RxCmd(FLASH_InitStruct->FLASH_cmd_rd_id, 3, flash_ID);
	/* Byte -> Mbits: 10 + 10 - 3 = 17 (0x11) */
	flash_capacity = (1 << (flash_ID[2] - 0x11));
	RTK_LOGI(TAG, "Flash ID: %x-%x-%x (Capacity: %dM-bit)\n", flash_ID[0], flash_ID[1], flash_ID[2], flash_capacity);

	/* Get flash chip information */
	current_IC = flash_get_chip_info((flash_ID[2] << 16) | (flash_ID[1] << 8) | flash_ID[0]);
	if (current_IC == NULL) {
		RTK_LOGW(TAG, "This flash type is not supported!\n");
		assert_param(0);
	}

	/* Re-initialize flash init structure according to classification */
	switch (current_IC->flash_class) {
	case FlashClass1:
		FLASH_StructInit(FLASH_InitStruct);
		break;
	case FlashClass2:
		FLASH_StructInit_GD(FLASH_InitStruct);
		if (flash_ID[2] > 0x15) { /* GD capacity more than 2MB */
			FLASH_InitStruct->FLASH_cmd_wr_status2 = 0x31;
		}
		break;
	case FlashClass3:
		FLASH_StructInit_MXIC(FLASH_InitStruct);
		break;
	case FlashClass4:	/* EON without QE bit */
		FLASH_StructInit_MXIC(FLASH_InitStruct);
		FLASH_InitStruct->FLASH_QuadEn_bit = 0;
		break;
	case FlashClass5:
		FLASH_StructInit_Micron(FLASH_InitStruct);
		break;
	case FlashClass6:	/* MXIC wide-range VCC chip */
		FLASH_StructInit_MXIC(FLASH_InitStruct);
		check_config_reg = 1;
		break;
	case FlashClassUser:
		assert_param(current_IC->FlashInitHandler != NULL);
		current_IC->FlashInitHandler();
		break;
	default:
		break;
	}

	if (SYSCFG_OTP_SPICAddr4ByteEn()) {
		FLASH_InitStruct->FLASH_addr_phase_len = ADDR_4_BYTE;
	}

}

static void flash_set_status_register(void)
{
	u8 StatusLen = 1;
	u32 data = 0;
	u32 status = 0;
	u32 mask = current_IC->sta_mask;
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;

	if (FLASH_InitStruct->FLASH_QuadEn_bit != 0) {
		data |= FLASH_InitStruct->FLASH_QuadEn_bit;
	}

	/* read status1 register */
	FLASH_RxCmd(FLASH_InitStruct->FLASH_cmd_rd_status, 1, (u8 *)&status);

	/* check if status2 exist */
	if (FLASH_InitStruct->FLASH_Status2_exist) {
		StatusLen = 2;
		FLASH_RxCmd(FLASH_InitStruct->FLASH_cmd_rd_status2, 1, ((u8 *)&status) + 1);

	} else if (check_config_reg) {	/* for MXIC wide-range flash, 1 status register + 2 config register */
		/* Read configuration register */
		FLASH_RxCmd(0x15, 2, ((u8 *)&status) + 1);
		StatusLen = 3;

		/* L/H Switch */
		data |= (BIT(9) << 8);
	}

	status &= mask;
	if (_memcmp((void *)&status, (void *)&data, StatusLen)) {
		if (!FLASH_InitStruct->FLASH_cmd_wr_status2) {
			FLASH_SetStatus(FLASH_InitStruct->FLASH_cmd_wr_status, StatusLen, (u8 *)&data);
		} else {
			FLASH_SetStatus(FLASH_InitStruct->FLASH_cmd_wr_status, 1, (u8 *)&data);
			FLASH_SetStatus(FLASH_InitStruct->FLASH_cmd_wr_status2, 1, ((u8 *)&data) + 1);
		}
		RTK_LOGI(TAG, "Flash status register changed:0x%x -> 0x%x\n", status, data);
	}
}

SRAMDRAM_ONLY_TEXT_SECTION
u32 flash_rx_mode_switch(u32 spic_mode)
{
	u32 Ret = _SUCCESS;
	u8 status = 0;
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

		if (FLASH_InitStruct->FLASH_Id == FLASH_ID_MICRON) {
			FLASH_RxCmd(0x85, 1, &status);
			status = (status & 0x0f) | (FLASH_InitStruct->FLASH_rd_dummy_cyle[spic_mode] << 4);
			FLASH_SetStatus(0x81, 1, &status);
		}

		FLASH_Init(spic_mode);

		if (spic_mode == Spic1IOBitMode) {
			RTK_LOGE(TAG, "Flash Switch Read Mode FAIL\n");
			Ret = _FAIL;
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
	u8 read_mode, flash_speed;
	u32 Temp, spic_ckd;
	u32 pll_clk = PLL_ClkGet();

	read_mode = flash_get_readmode(Flash_ReadMode);
	flash_speed = Flash_Speed - 1;

	spic_ckd = CLKDIV_ROUND_UP(pll_clk, SPIC_CLK_LIMIT) - 1;
	flash_speed = MAX(flash_speed, spic_ckd);

	__disable_irq();

	/* SPIC stay in BUSY state when there are more than 0x1_0000 cycles between two input data.
	 * Disable DREIR to avoid that interrupt hanler time is lager than 0x1_0000 SPIC cycles.
	 */
	SPIC->CTRLR0 |= BIT_SPI_DREIR_R_DIS;
	if (flash_speed == spic_ckd) {
		/* CS setup time changed to 2 SPIC Clk for greater than tSLCH if SPIC is 200MHz */
		SPIC->TPR1 = (SPIC->TPR1 & ~MASK_CR_ACTIVE_SETUP) | CR_ACTIVE_SETUP(2);
	}

	/* Get flash ID to reinitialize FLASH_InitTypeDef structure */
	flash_get_vendor();

	/* Set flash status register: set QE, clear protection bits */
	flash_set_status_register();

	/* Set flash I/O mode and high-speed calibration */
	flash_rx_mode_switch(read_mode);

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

	if (SYSCFG_CHIPType_Get() != CHIP_TYPE_FPGA) {
		flash_handshake_highspeed(flash_speed);
	}

	__enable_irq();
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

