/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#define FLASH_CALIBRATION_DEBUG		0

static const char *const TAG = "FLASH";
static FlashInfo_TypeDef *current_IC;

/* Flag to check configuration register or not. Necessary for wide-range VCC MXIC flash */
static u8 check_config_reg = 0;

static u32 FLASH_PLLGet_ClockDiv(void)
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

	/* Note: some full voltage range Flash e.g.GD25WQ64E need 10ns
	   HW recommend: at leaset 10ns */
	SPIC->TPR1 = (SPIC->TPR1 & ~MASK_CS_ACTIVE_SETUP) | CS_ACTIVE_SETUP(2);

	/* set tSHSL (CS_H) to min 60ns (12 spic_clk) */
	SPIC->TPR0 = (SPIC->TPR0 & ~MASK_CS_H_RD_DUM_LEN) | CS_H_RD_DUM_LEN(12);

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

		SPIC->TPR1 = (SPIC->TPR1 & ~MASK_CS_ACTIVE_SETUP) | CS_ACTIVE_SETUP(1);
	}

	RTK_LOGI(TAG, "FLASH HandShake %s\n", Ret == RTK_SUCCESS ? "OK" : "FAIL, use XTAL");
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
		/* GD flash */
		if (flash_ID[0] == 0xC8) {
			/* 3.3v flash_id[1] = 40h or 1.8v ~ 3.3v flash_id[1] = 65h */
			if ((flash_ID[1] == 0x40) || (flash_ID[1] == 0x65)) {
				/* GD capacity more than 2MB, need 31h cmd to write SR2 */
				if (flash_ID[2] >= 0x16) {
					FLASH_InitStruct->FLASH_cmd_wr_status2 = 0x31;
				}
			}
		} else {
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
int flash_rx_mode_switch(u32 spic_mode)
{
	int Ret = RTK_SUCCESS;
	u8 status = 0;
	char *str[] = {"1IO", "2O", "2IO", "4O", "4IO"};
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;

	/* Try sequentially: 4IO, 4O, 2IO, 2O, 1bit */
	while (1) {
		if (FLASH_InitStruct->FLASH_Id == FLASH_ID_MICRON) {
			FLASH_RxCmd(0x85, 1, &status);
			status = (status & 0x0f) | (FLASH_InitStruct->FLASH_rd_dummy_cycle[spic_mode] << 4);
			FLASH_SetStatus(0x81, 1, &status);
		} else if (current_IC->flash_id == 0x85) { // PUYA FLASH
			if (FLASH_InitStruct->FLASH_rd_dummy_cycle[Spic4IOBitMode] > FLASH_DM_CYCLE_4IO) {
				/* set FLASH DC bit in configuration register */
				FLASH_RxCmd(FLASH_CMD_RDCR, 1, &status);
				status |= BIT1; // DC bit of PY25Q32H
				FLASH_SetStatus(FLASH_CMD_WRCR, 1, &status);
			}
		}

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
	FLASH_InitTypeDef *FLASH_InitStruct = &flash_init_para;
	read_mode = flash_get_readmode(Flash_ReadMode);

	irq_status = irq_disable_save();

	/* SPIC stay in BUSY state when there are more than 0x1_0000 cycles between two input data.
	 * Disable DREIR to avoid that interrupt hanler time is lager than 0x1_0000 SPIC cycles.
	 */
	SPIC->CTRLR0 |= BIT_SPI_DREIR_R_DIS;

	/* Get flash ID to reinitialize FLASH_InitTypeDef structure */
	flash_get_vendor();

	/* Set flash status register: set QE, clear protection bits */
	flash_set_status_register();

	if (SYSCFG_CHIPType_Get() != CHIP_TYPE_FPGA) {
		/* Calculate flash actual freq according to CLK_LIMIT_SPIC */
		/* Set FLASH DC bit and change SPIC dummy cycle if flash clk is over 104MHz */
		if (FLASH_PLLGet_ClockDiv() > (104 * MHZ_TICK_CNT)) {
			/* Change SPIC calibration data */
			/* TODO: other bit modes & other flash */
			FLASH_InitStruct->FLASH_rd_dummy_cycle[Spic4IOBitMode] = 0xA; // 4IO dummy cycle
		}
	}

	/* Set flash I/O mode and high-speed calibration */
	flash_rx_mode_switch(read_mode);

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
		flash_handshake_highspeed();
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
