/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "nor_flash.h"

#define FLASH_CALIBRATION_DEBUG		0

static const char *const TAG = "FLASH";
static u32 SPIC_DATA_FLASH_CALIB_PATTERN[2] = {0};

static void DATA_FLASH_PLLInit_ClockDiv(void)
{
	u8 psramc_ckd;
	u32 usb_pll_clk = GBSS_DEV->clk_info_bk.USBPLL_CLK;
	u32 sys_pll_clk = GBSS_DEV->clk_info_bk.SYSPLL_CLK;

	/* set spic clock src and div */
	psramc_ckd = PLL_ClkSrcGet(sys_pll_clk, usb_pll_clk, CLK_LIMIT_SPIC);
	if (psramc_ckd & IS_SYS_PLL) {
		RCC_PeriphClockDividerSet(SYS_PLL_PSRAM, GET_CLK_DIV(psramc_ckd));
		RCC_PeriphClockSourceSet(PSRAM, SYS_PLL);
	} else {
		RCC_PeriphClockDividerSet(USB_PLL_PSRAM, GET_CLK_DIV(psramc_ckd));
		RCC_PeriphClockSourceSet(PSRAM, USB_PLL);
	}

	/* save spic clock para into retention memory */
	IPC_SEMTake(IPC_SEM_RRAM, 0xffffffff);
	GBSS_DEV->clk_info_bk.psramc_ckd = psramc_ckd;
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
		ComboSpic->CTRLR2 |= BIT_PHY_RD_VALID;
	} else {
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, temp & ~LSYS_BIT_PSRAMC_DPHY_EN);
		ComboSpic->CTRLR2 &= ~BIT_PHY_RD_VALID;
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
		RTK_LOGI(TAG, "read data from data flash address0 as SPIC_DATA_FLASH_CALIB_PATTERN when flash is in 1io 20M. \n"
				 "DATA FLASH_Read Expected: 0x%08lx 0x%08lx, Got: 0x%08lx 0x%08lx\n",
				 SPIC_DATA_FLASH_CALIB_PATTERN[0], SPIC_DATA_FLASH_CALIB_PATTERN[1], pgolden_data[0], pgolden_data[1]);
	}

	/* compare data */
	if (_memcmp(pgolden_data, SPIC_DATA_FLASH_CALIB_PATTERN, 8) == 0) {
		return TRUE;
	} else {
		RTK_LOGE(TAG, "Data flash calibration fail. \n"
				 "DATA FLASH_Read Expected: 0x%08lx 0x%08lx, Got: 0x%08lx 0x%08lx\n"
				 "If use nand flash, consider bad block\n",
				 SPIC_DATA_FLASH_CALIB_PATTERN[0], SPIC_DATA_FLASH_CALIB_PATTERN[1], pgolden_data[0], pgolden_data[1]);
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

	RTK_LOGI(TAG, "DATA FLASH HandShake %s\n", Ret == RTK_SUCCESS ? "OK" : "FAIL");
	return Ret;
}

int data_flash_rx_mode_switch(u32 spic_mode)
{
	int Ret = RTK_SUCCESS;
	char *str[] = {"1IO", "2O", "2IO", "4O", "4IO"};
	FLASH_InitTypeDef *FLASH_InitStruct = data_flash.para;

	/* Try sequentially: 4IO, 4O, 2IO, 2O, 1bit */
	while (1) {
		/* 4IO or 2IO should enable dummy byte function */
		if ((spic_mode == Spic4IOBitMode) || (spic_mode == Spic2IOBitMode)) {
			FLASH_InitStruct->FLASH_prm_avoid = 1;
		} else {
			FLASH_InitStruct->FLASH_prm_avoid = 0;
		}
		/* Dummy cycles of the default profile, before every attempt */
		nor_flash_set_default_dummy(&data_flash);

		data_flash.init(spic_mode);
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
	FLASH_InitTypeDef *FLASH_InitStruct = data_flash.para;
	u32 read_mode = flash_get_readmode(Data_Flash_ReadMode);
	u8 mem_type = ChipInfo_MemoryType();

	if ((mem_type != MCM_TYPE_NOR_FLASH) && (mem_type != MCM_TYPE_NAND_FLASH)) {
		RTK_LOGE(TAG, "Can't support second flash for chip without mem flash\n");
		return;
	}

	DATA_FLASH_StructInit(FLASH_InitStruct);
	FLASH_InitStruct->FLASH_baud_rate = 1;
	FLASH_InitStruct->FLASH_baud_boot = 1;

	/* Identify the flash and apply its strategy, then set QE if a quad read mode is asked for */
	nor_flash_get_vendor(&data_flash);
	nor_flash_set_qe(&data_flash, read_mode);

	/* read data from data flash address0 as calib data when flash is in 1io 20M*/
	DCache_Invalidate(DATA_FLASH_BASE, 8);
	SPIC_DATA_FLASH_CALIB_PATTERN[0] = HAL_READ32(DATA_FLASH_BASE, 0x00);
	SPIC_DATA_FLASH_CALIB_PATTERN[1] = HAL_READ32(DATA_FLASH_BASE, 0x04);

	/* Set flash I/O mode and high-speed calibration */
	data_flash_rx_mode_switch(read_mode);

	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_ASIC) {
		data_flash_handshake_highspeed();
	}
}
