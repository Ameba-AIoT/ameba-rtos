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

static void DATA_FLASH_PLLInit_ClockDiv(void)
{
	u8 psramc_ckd;
	u32 usb_pll_clk = RRAM_DEV->clk_info_bk.USBPLL_CLK;
	u32 sys_pll_clk = RRAM_DEV->clk_info_bk.SYSPLL_CLK;

	/* set spic clock src and div */
	psramc_ckd = PLL_ClkSrcGet(sys_pll_clk, usb_pll_clk, CLK_LIMIT_PSRAM);
	if (psramc_ckd & IS_SYS_PLL) {
		RCC_PeriphClockDividerSet(SYS_PLL_PSRAM, GET_CLK_DIV(psramc_ckd));
		RCC_PeriphClockSourceSet(PSRAM, SYS_PLL);
	} else {
		RCC_PeriphClockDividerSet(USB_PLL_PSRAM, GET_CLK_DIV(psramc_ckd));
		RCC_PeriphClockSourceSet(PSRAM, USB_PLL);
	}

	/* save spic clock para into retention memory */
	IPC_SEMTake(IPC_SEM_RRAM, 0xffffffff);
	RRAM_DEV->clk_info_bk.psramc_ckd = psramc_ckd;
	IPC_SEMFree(IPC_SEM_RRAM);
}

/**
  * @brief  Enables or disables the FLASH Read HandShake Function
  * @param  Dphy_Dly_Cnt: when set ≥ 3, data  will occur after DPHY_RD_DELAY_CNT + 2 axi clock from the rising edge of spi_rx_data_en
  * @param  NewState: new state of the FLASH Read HandShake Function. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DATA_FLASH_Read_HandShake_Cmd(u32 Dphy_Dly_Cnt, u32 NewState)
{
	SPIC_TypeDef *ComboSpic = SPIC_COMBO;

	/* spic_dphy output dealy cnt */
	u32 temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);
	temp &= ~LSYS_MASK_PSRAMC_DPHY_RD_DELAY_CNT;
	temp |= LSYS_PSRAMC_DPHY_RD_DELAY_CNT(Dphy_Dly_Cnt);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, temp);

	/* enable spic_dphy function */
	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);
	if (NewState != DISABLE) {
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, temp | LSYS_BIT_PSRAMC_DPHY_EN);
		ComboSpic->CTRLR2 |= BIT_RD_VALID_EN;
	} else {
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, temp & ~LSYS_BIT_PSRAMC_DPHY_EN);
		ComboSpic->CTRLR2 &= ~BIT_RD_VALID_EN;
	}
}

/**
  * @brief  read flash data to compare with golden data
  * @param  None
  * @retval 1: read flash data is right
  *			0: read flash data is wrong
  * @retval None
  */
u32 DATA_FLASH_Read_DataIsRight(void)
{
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;
	u32 pgolden_data[2];
	u32 LoaderLogAddr = DATA_FLASH_BASE;

	DCache_Invalidate(LoaderLogAddr, 8);
	pgolden_data[0] = HAL_READ32(LoaderLogAddr, 0x00);
	pgolden_data[1] = HAL_READ32(LoaderLogAddr, 0x04);

	if (FLASH_InitStruct->debug) {
		RTK_LOGI(TAG, "DATA FLASH_Read 0x%lx:0x%lx\n", pgolden_data[0], pgolden_data[1]);
	}

	/* compare data */
	if (_memcmp(pgolden_data, SPIC_CALIB_PATTERN, 8) == 0) {
		return TRUE;
	} else {
		RTK_LOGE(TAG, "Data flash calibration fail. \n"
				 "Make sure to program the calibration pattern (0x%lx:0x%lx) at address 0 of data flash.\n"
				 "If use nand flash, consider bad block\n",
				 SPIC_CALIB_PATTERN[0], SPIC_CALIB_PATTERN[1]);
		return FALSE;
	}
}

int data_flash_handshake_highspeed(void)
{
	u8 Dphy_Dly_Cnt = 3; /* DD recommend this value */
	int Ret = RTK_FAIL;

	/* SPIC clock switch to PLL */
	DATA_FLASH_PLLInit_ClockDiv();

	/* Set CSsetuptime to 2 SPIC Clk if SPIC > 200MHz in order to great than tSLCH(5ns).
	   Note: Full voltage range Flash e.g.GD25WQ64E need 10ns */
	if (PSRAMC_ClkGet() > SPIC_CS_SETUP_TIME_LMT) {
		SPIC_COMBO->TPR1 = (SPIC_COMBO->TPR1 & ~MASK_CS_ACTIVE_SETUP) | CS_ACTIVE_SETUP(2);
	}

	/* Open handshake function */
	DATA_FLASH_Read_HandShake_Cmd(Dphy_Dly_Cnt, ENABLE);

	if (DATA_FLASH_Read_DataIsRight()) {
		Ret = RTK_SUCCESS;
	} else {
		DATA_FLASH_Read_HandShake_Cmd(Dphy_Dly_Cnt, DISABLE);
		RCC_PeriphClockSourceSet(PSRAM, XTAL);
		SPIC_COMBO->TPR1 = (SPIC_COMBO->TPR1 & ~MASK_CS_ACTIVE_SETUP) | CS_ACTIVE_SETUP(1);
	}

	RTK_LOGI(TAG, "DATA FLASH HandShake[0x%x %s]\n", div, Ret == RTK_SUCCESS ? "OK" : "FAIL");
	return Ret;
}

void data_flash_get_vendor(void)
{
	u8 flash_ID[4];
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;

	/* Read flash ID */
	DATA_FLASH_RxCmd(FLASH_InitStruct->FLASH_cmd_rd_id, 3, flash_ID);
	RTK_LOGI(TAG, "Flash ID: %x-%x-%x\n", flash_ID[0], flash_ID[1], flash_ID[2]);

	/* Get flash chip information */
	current_IC = data_flash_get_chip_info((flash_ID[2] << 16) | (flash_ID[1] << 8) | flash_ID[0]);
	if (current_IC == NULL) {
		RTK_LOGE(TAG, "This flash type is not supported!\n");
		assert_param(0);
	}

	/* Re-initialize flash init structure according to classification */
	switch (current_IC->flash_class) {
	case FlashClass1:
		DATA_FLASH_StructInit(FLASH_InitStruct);
		break;
	case FlashClass2:
		DATA_FLASH_StructInit_GD(FLASH_InitStruct);
		if (flash_ID[2] > 0x15) { /* GD capacity more than 2MB */
			FLASH_InitStruct->FLASH_cmd_wr_status2 = 0x31;
		}
		break;
	case FlashClass3:
		DATA_FLASH_StructInit_MXIC(FLASH_InitStruct);
		break;
	case FlashClass4:	/* EON without QE bit */
		DATA_FLASH_StructInit_MXIC(FLASH_InitStruct);
		FLASH_InitStruct->FLASH_QuadEn_bit = 0;
		break;
	case FlashClass5:
		DATA_FLASH_StructInit_Micron(FLASH_InitStruct);
		break;
	case FlashClass6:	/* MXIC wide-range VCC chip */
		DATA_FLASH_StructInit_MXIC(FLASH_InitStruct);
		check_config_reg = 1;
		break;
	case FlashClassUser:
		assert_param(current_IC->FlashInitHandler != NULL);
		current_IC->FlashInitHandler();
		break;
	default:
		break;
	}

	//FLASH_InitStruct->FLASH_addr_phase_len = ADDR_4_BYTE;
}

void data_flash_set_status_register(void)
{
	u8 StatusLen = 1;
	u32 data = 0;
	u32 status;
	u32 mask = current_IC->sta_mask;
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;

	if (FLASH_InitStruct->FLASH_QuadEn_bit != 0) {
		data |= FLASH_InitStruct->FLASH_QuadEn_bit;
	}

	/* read status1 register */
	DATA_FLASH_RxCmd(FLASH_InitStruct->FLASH_cmd_rd_status, 1, (u8 *)&status);

	/* check if status2 exist */
	if (FLASH_InitStruct->FLASH_Status2_exist) {
		StatusLen = 2;
		DATA_FLASH_RxCmd(FLASH_InitStruct->FLASH_cmd_rd_status2, 1, ((u8 *)&status) + 1);

	} else if (check_config_reg) {	/* for MXIC wide-range flash, 1 status register + 2 config register */
		/* Read configuration register */
		DATA_FLASH_RxCmd(0x15, 2, ((u8 *)&status) + 1);
		StatusLen = 3;

		/* L/H Switch */
		data |= (BIT(9) << 8);
	}

	status &= mask;
	if (_memcmp((void *)&status, (void *)&data, StatusLen)) {
		if (!FLASH_InitStruct->FLASH_cmd_wr_status2) {
			DATA_FLASH_SetStatus(FLASH_InitStruct->FLASH_cmd_wr_status, StatusLen, (u8 *)&data);
		} else {
			DATA_FLASH_SetStatus(FLASH_InitStruct->FLASH_cmd_wr_status, 1, (u8 *)&data);
			DATA_FLASH_SetStatus(FLASH_InitStruct->FLASH_cmd_wr_status2, 1, ((u8 *)&data) + 1);
		}
	}
}

int data_flash_rx_mode_switch(u32 spic_mode)
{
	int Ret = RTK_SUCCESS;
	char *str[] = {"1IO", "2O", "2IO", "4O", "4IO"};
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;

#if !defined(CONFIG_SECOND_FLASH_NOR) && !defined(CONFIG_SECOND_FLASH_NAND)
	UNUSED(FLASH_InitStruct);
	return RTK_FAIL;
#endif

	/* Try sequentially: 4IO, 4O, 2IO, 2O, 1bit */
	while (1) {
		/* 4IO or 2IO should enable dummy byte function */
		if ((spic_mode == Spic4IOBitMode) || (spic_mode == Spic2IOBitMode)) {
			FLASH_InitStruct->FLASH_prm_avoid = 1;
		} else {
			FLASH_InitStruct->FLASH_prm_avoid = 0;
		}
#if defined(CONFIG_SECOND_FLASH_NOR)
		u8 status = 0;

		if (FLASH_InitStruct->FLASH_Id == FLASH_ID_MICRON) {
			DATA_FLASH_RxCmd(0x85, 1, &status);
			status = (status & 0x0f) | (FLASH_InitStruct->FLASH_rd_dummy_cycle[spic_mode] << 4);
			DATA_FLASH_SetStatus(0x81, 1, &status);
		}

		DATA_FLASH_Init(spic_mode);
#elif defined(CONFIG_SECOND_FLASH_NAND)
		DATA_NAND_Init(spic_mode);
#endif
		if (spic_mode == Spic1IOBitMode) {
			RTK_LOGE(TAG, "DATA Flash Switch Read Mode FAIL\n");
			Ret = RTK_FAIL;
			break;
		}

		if (DATA_FLASH_Read_DataIsRight()) {
			RTK_LOGI(TAG, "DATA Flash Read %s\n", str[spic_mode]);
			break;
		}

		spic_mode--;
	}

	return Ret;
}

void data_flash_highspeed_setup(void)
{
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;
	u32 read_mode = flash_get_readmode(Data_Flash_ReadMode);

#if !defined(CONFIG_SECOND_FLASH_NOR) && !defined(CONFIG_SECOND_FLASH_NAND)
	UNUSED(FLASH_InitStruct);
	return;
#endif

	if (ChipInfo_MemoryType() == MCM_TYPE_PSRAM) {
		RTK_LOGE(TAG, "Can't support second flash for chip with mem psram\r\n");
		return;
	}

	__disable_irq();

#if defined(CONFIG_SECOND_FLASH_NOR)
	DATA_FLASH_StructInit(FLASH_InitStruct);
	FLASH_InitStruct->FLASH_baud_rate = 1;
	FLASH_InitStruct->FLASH_baud_boot = 1;
	// FLASH_InitStruct->FLASH_addr_phase_len = ADDR_4_BYTE;

	/* Get flash ID to reinitialize FLASH_InitTypeDef structure */
	data_flash_get_vendor();

	/* Set flash status register: set QE, clear protection bits */
	data_flash_set_status_register();
#elif defined(CONFIG_SECOND_FLASH_NAND)
	/* init data nand flash default settings */
	NAND_StructInit(FLASH_InitStruct);

	/* get flash page size */
	u32 page_size_byte = 0;
	if (RTK_SUCCESS == DATA_NAND_GetPageSize(&page_size_byte)) {
		if (page_size_byte == 0x1000) {
			FLASH_InitStruct->FLASH_pagesize_main_bit_exp = NAND_PAGE_SIZE_MAIN_4K_BIT_EXP;
			RTK_LOGI(TAG, "data nand flash page size set to 4k bytes\n");
		} else {
			FLASH_InitStruct->FLASH_pagesize_main_bit_exp = NAND_PAGE_SIZE_MAIN_2K_BIT_EXP;
		}
	} else {
		FLASH_InitStruct->FLASH_pagesize_main_bit_exp = NAND_PAGE_SIZE_MAIN_2K_BIT_EXP;
		RTK_LOGE(TAG, "Warning: DATA_NAND_GetPageSize fail, keep 2k bytes page size setting\n");
	}

	/* get flash vender */
	data_nand_get_vendor();

	/* unlock flash */
	if (NAND_BL_ALL_UNLOCKED != DATA_NAND_GetStatus(NAND_REG_BLOCK_LOCK)) {
		DATA_NAND_SetStatus(NAND_REG_BLOCK_LOCK, NAND_BL_ALL_UNLOCKED);
		RTK_LOGI(TAG, "data NAND flash protection unlocked: 0x%x\n", DATA_NAND_GetStatus(NAND_REG_BLOCK_LOCK));
	}
#endif
	/* Set flash I/O mode and high-speed calibration */
	data_flash_rx_mode_switch(read_mode);

	if (SYSCFG_CHIPType_Get() != CHIP_TYPE_FPGA) {
		data_flash_handshake_highspeed();
	}
	__enable_irq();
}
