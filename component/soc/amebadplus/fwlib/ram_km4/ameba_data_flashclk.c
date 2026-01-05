/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#define FLASH_CALIBRATION_DEBUG		0

static const char *const TAG = "DFLASHCLK";

static FlashInfo_TypeDef *current_IC;

/* Flag to check configuration register or not. Necessary for wide-range VCC MXIC flash */
static u8 check_config_reg = 0;

static u32 SPIC_DATA_FLASH_CALIB_PATTERN[2] = {0};

u32 DATA_FLASH_SIZE = 0;

/**
  * @brief  Set New Calibration 600M PS Disable/Enable
  * @param  NewStatus: Disable/Enable
  */
void DATA_FLASH_CalibrationPLLPSCmd(u32 NewStatus)
{
	PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;
	/* set PLL 400M phase shift enable */
	if (NewStatus == ENABLE) {
		PLL->PLL_PS |= PLL_BIT_EN_CK400M_PS;
	} else {
		PLL->PLL_PS &= ~PLL_BIT_EN_CK400M_PS;
	}

}

/**
  * @brief  Set New Calibration phase shift in PLL Cycle
  * @param  phase_sel means how many phase shift
  */
void DATA_FLASH_CalibrationPLLPS_Shift(u32 phase_sel)
{
	u32 temp = 0;
	PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;

	temp = PLL->PLL_PS;
	temp &= ~PLL_MASK_PHASE_SEL;
	temp |= PLL_PHASE_SEL(phase_sel);	/* set phase frac: 3bits : 0/8 ~ 7/8 CPUPLL cycle */
	PLL->PLL_PS = temp;

}

/**
  * @brief  Set Flash Clock
  * @param  Div: 400M clock Dividor for SPIC Clock.
  *   This parameter can be any one of the following values:
  *		 @arg FLASH_CLK_DIV2  SPIC-PLLCLK/2  SPI-SPICCLK/2
  *		 @arg FLASH_CLK_DIV3  SPIC-PLLCLK/3  SPI-SPICCLK/3
  *		 @arg ...
  *		 @arg FLASH_CLK_DIV10  SPIC-PLLCLK/10  SPI-SPICCLK/10
  * @param  ps_pll_en: to open ps pll or not. This parameter can be: ENABLE or DISABLE.
  */
u32 DATA_FLASH_PLLInit_ClockDiv(FlashDivInt_E Div)
{
	u32 temp = 0;
	int timeout = 10;

	/* Gate flash clock, dont reset SPIC */
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_PSRAM_CLOCK, DISABLE);

	/* disable clock div */
	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);
	temp &= ~LSYS_BIT_PSRAMC_FLASH_DIV_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, temp);

	/* Polling PLL Power */
	//FLASH_Calibration_PLL_Open();

	/* Select source clock firstly, then div module can work corectly */
	RCC_PeriphClockSource_PSRAM(BIT_LSYS_CKSL_PSRAM_CPUPLL);

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

	/* Release gating of flash clock after BIT_FLASH_DIV_RDY. */
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_PSRAM_CLOCK, ENABLE);

	return TRUE;
}

/**
  * @brief  Set New Calibration Clock Phase
  * @param  phase_int: phase int
  *   This parameter can be any one of the following values:
  *		 @arg 0 0*(PLL Cycle Delay)
  *		 @arg 1 1*(PLL Cycle Delay)
  *		 @arg 2 2*(PLL Cycle Delay)
  *		 @arg 3 3*(PLL Cycle Delay)
  *		 @arg 4 4*(PLL Cycle Delay)
  *		 @arg 5 5*(PLL Cycle Delay)
  * @param  phase_sel: phase frac
  *   This parameter can be any one of the following values:
  *		 @arg 0 0*(PLL Cycle Delay)/8
  *		 @arg 1 1*(PLL Cycle Delay)/8
  *		 @arg 2 2*(PLL Cycle Delay)/8
  *		 @arg 3 3*(PLL Cycle Delay)/8
  *		 @arg 4 4*(PLL Cycle Delay)/8
  *		 @arg 5 5*(PLL Cycle Delay)/8
  *		 @arg 6 6*(PLL Cycle Delay)/8
  *		 @arg 7 7*(PLL Cycle Delay)/8
  */
u32 DATA_FLASH_CalibrationPhase(u8 phase_int, u8 phase_sel)
{
	u32 temp = 0;
	int timeout = 10;

	/* 1. Gate flash clock */
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_PSRAM_CLOCK, DISABLE);

	/* 2. disable pll div and ps_pll div, otherwise flash clock phase is not sync */
	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);
	temp &= ~(LSYS_BIT_PSRAMC_FLASH_PS_DIV_EN | LSYS_BIT_PSRAMC_FLASH_DIV_EN);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, temp);

	/* 3. wait ready flag disable for Flash clock with phase shift */
	while (timeout > 0) {
		timeout--;
		temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);
		if ((temp & (LSYS_BIT_PSRAMC_FLASH_PS_DIV_RDY | LSYS_BIT_PSRAMC_FLASH_DIV_RDY)) == 0) {
			break;
		}
	}

	/* close PLL ps for little glich if pll ps is exist */
	DATA_FLASH_CalibrationPLLPSCmd(DISABLE);

	/* 4.1 set phase frac */
	DATA_FLASH_CalibrationPLLPS_Shift(phase_sel);

	/* 4.2 set phase_int */
	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);
	temp &= ~(LSYS_MASK_PSRAMC_FLASH_PS_PHASE); 	/* set phase_int: 3bits : 0 ~ 7 DSPPLL cycle */
	temp |= LSYS_PSRAMC_FLASH_PS_PHASE(phase_int);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, temp);

	/* open PLL ps */
	DATA_FLASH_CalibrationPLLPSCmd(ENABLE);

	/* 5. enable clock div */
	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);
	temp |= (LSYS_BIT_PSRAMC_FLASH_PS_DIV_EN | LSYS_BIT_PSRAMC_FLASH_DIV_EN);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, temp);

	/* 6. wait ready flag of Flash clock with phase shift */
	timeout = 10;
	while (timeout > 0) {
		timeout--;
		temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);
		if ((temp & LSYS_BIT_PSRAMC_FLASH_PS_DIV_RDY) && (temp & LSYS_BIT_PSRAMC_FLASH_DIV_RDY)) {
			break;
		}
	}

	/* 7. Release gating of flash clock */
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_PSRAM_CLOCK, ENABLE);

	return TRUE;
}

/**
  * @brief  Set New Calibration Clock Phase based on index
  * @param  phase_idx: NEW_CALIBREATION_DATA array index get from calibration
  * @note  phase_idx is used to get phase_int & phase_sel from NEW_CALIBREATION_DATA
  */
u32 DATA_FLASH_CalibrationPhaseIdx(u8 phase_idx)
{
	u8 phase_int = FLASH_SHIFT_IDX_TO_PLLCLK(phase_idx);
	u8 phase_sel = FLASH_SHIFT_IDX_TO_PAHSE(phase_idx);

	return DATA_FLASH_CalibrationPhase(phase_int, phase_sel);
}

/**
  * @brief  Set New Calibration Disable/Enable
  * @param  NewStatus: Disable/Enable
  */
u32 DATA_FLASH_CalibrationNewCmd(u32 NewStatus)
{
	u32 temp = 0;

	/* set PLL 600M phase shift enable */
	DATA_FLASH_CalibrationPLLPSCmd(NewStatus);

	/* set phase calibration enable */
	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL);

	if (NewStatus == ENABLE) {
		temp |= LSYS_BIT_PSRAMC_FLASH_CAL_EN;
	} else {
		temp &= ~LSYS_BIT_PSRAMC_FLASH_CAL_EN;
	}

	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PSRAMC_FLASH_CTRL, temp);

	return TRUE;
}

/**
  * @brief  Flash New calibration function
  * @param  FLASH_InitStruct: pointer to a FLASH_InitTypeDef structure that contains
  *         the configuration information for the SPIC Clock.
  * @param  SpicBitMode: This parameter can be any combination of the following values
  *		 @arg Spic1IOBitMode
  *		 @arg SpicDualBitMode
  *		 @arg SpicQuadBitMode
  * @param  new_calibration_end: 1 SPIC CLK equal to FLASHDIVINT_TO_PLL_CLK(Div) * 8.
  * @param  CalStep: CalStep * (600M_delay)/8 =  CalStep * 0.2083ns delay every calibration point
  * @param  LineDelay(13~21ns): include FlashDelay(winbond: 6ns)+LineDelay(1ns)+AmebaDelay(13ns)
  * @param  StartIdx: calibration start index of NEW_CALIBREATION_DATA
  * @retval status value: (window1_size | (window1_start << 16) | (window1_end << 24))
  * @note 100MHz baudrate read 32bit will cost 8(cmd)+6(addr)+6(dummy)+4(data) = 24cycles = 240ns
  * @note step=2 & 100MHz will take 56.5us for a LineDelay calibration
  * @note cache will be disable in this function, you should open cache after this function if needed
  */
u32 DATA_FLASH_CalibrationNew(FLASH_InitTypeDef *FLASH_InitStruct, u8 SpicBitMode, u8 new_calibration_end, u8 CalStep, u8 SpicClkDelay, u8 StartIdx)
{
	SPIC_TypeDef *ComboSpic = SPIC_COMBO;

	u8 data_idx = 0;
	u8 phase_int = 0;
	u8 phase_frac = 0;

	int window1_start = -1;
	int window1_size = 0;
	int window1_end = -1;

	/* set bit mode  */
	DATA_FLASH_SetSpiMode(ComboSpic, FLASH_InitStruct, SpicBitMode);

	/* set spic register before IC clock close */
	ComboSpic->BAUDR = SCKDV(FLASH_InitStruct->FLASH_baud_rate);
	ComboSpic->AUTO_LENGTH = (ComboSpic->AUTO_LENGTH & ~MASK_IN_PHYSICAL_CYC) | IN_PHYSICAL_CYC(SpicClkDelay);

	/* calibration Enable */
	DATA_FLASH_CalibrationNewCmd(DISABLE);
	DATA_FLASH_CalibrationNewCmd(ENABLE);

	/* first phase sample data */
	data_idx = StartIdx;
	while (1) {
		/* shift unit is CalStep*PLLCLK/FLASH_PLLCLK_EQUAL_PAHSE */
		phase_int = FLASH_SHIFT_IDX_TO_PLLCLK(data_idx);
		phase_frac = FLASH_SHIFT_IDX_TO_PAHSE(data_idx);

		if (data_idx > new_calibration_end) {
			break;
		}

		DATA_FLASH_CalibrationPhaseIdx(data_idx);

		if (FLASH_InitStruct->debug) {
			RTK_LOGI(TAG, "DATA_FLASH_CalibrationNew phase_int:%x phase_frac:%x\n", phase_int, phase_frac);
		}

		/* compare data */
		if (DATA_FLASH_Read_DataIsRight()) {
			if (window1_start < 0) {
				window1_size = 1;
				window1_start = data_idx;
			} else {
				window1_size += CalStep;
			}
			window1_end = data_idx;
		} else {
			if (window1_size > 0) {
				window1_end = data_idx - CalStep;
				break;
			}
		}

		data_idx = data_idx + CalStep;
	}

	if (FLASH_InitStruct->debug) {
		RTK_LOGI(TAG,  "DATA_FLASH_CalibrationNew LineDelay:%x phase_int:%x phase_frac:%x\n", SpicClkDelay, phase_int, phase_frac);
		RTK_LOGI(TAG, "DATA_FLASH_CalibrationNew window1_start:%d window1_size:%d \n", window1_start, window1_size);
	}

	if (window1_size > 0) {
		/* get phase_shift_idx */
		FLASH_InitStruct->phase_shift_idx = window1_start + window1_size / 2;

		if (FLASH_InitStruct->debug) {
			RTK_LOGI(TAG, "DATA_FLASH_CalibrationNew phase_shift_idx:%d \n", FLASH_InitStruct->phase_shift_idx);
		}
	}

	/* should disable it, enbale it outside if needed */
	DATA_FLASH_CalibrationNewCmd(DISABLE);

	/* Recover  */
	ComboSpic->BAUDR = SCKDV(FLASH_InitStruct->FLASH_baud_boot);
	DATA_FLASH_SetSpiMode(ComboSpic, FLASH_InitStruct, FLASH_InitStruct->FLASH_cur_bitmode);

	return (window1_size | (window1_start << 16) | (window1_end << 24));
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
	/* Need Gold data in Data Flash */
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

bool _data_flash_calibration_highspeed(u8 SpicBitMode, FlashDivInt_E Div)
{
	u32 window_size = 0;
	u32 phase_shift_idx = 0;
	u32 spic_cyc_dly = 0;
	u32 spic_cyc_dly_temp = 0;
	u32 window_temp = 0;
	u32 window_size_temp = 0;
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;

	/* init last point before calibration, Minus 1 because 0-based */
	u8 new_calibration_end = FLASH_SPICCLK_TO_PLLCLK(Div) * FLASH_PLLCLK_EQUAL_PAHSE - 1;

	/* 4.808 ms take when enter this function */
	/* 511us will take for flash full calibration */
	for (spic_cyc_dly_temp = 0; spic_cyc_dly_temp <= 4; spic_cyc_dly_temp++) {
		window_temp = DATA_FLASH_CalibrationNew(FLASH_InitStruct, SpicBitMode, new_calibration_end, 2, spic_cyc_dly_temp, 1);

		if (FLASH_InitStruct->phase_shift_idx != 0) {
			FLASH_InitStruct->phase_shift_idx |= BIT(0); /* odd is better */
		}

		window_size_temp = (window_temp & 0xFF);
#if FLASH_CALIBRATION_DEBUG
		u32 window_start_temp = ((window_temp >> 16) & 0xFF);
		u32 window_end_temp = ((window_temp >> 24) & 0xFF);

		RTK_LOGI(TAG, "calibration_result:[%lu:%lu:%lu][0x%lx:0x%lx] \n", spic_cyc_dly_temp, window_size_temp, FLASH_InitStruct->phase_shift_idx,
				 window_start_temp, window_end_temp);
		BKUP_Write(BKUP_REG3, ((window_start_temp << 24) | (spic_cyc_dly_temp << 16) | (window_size_temp << 8) | FLASH_InitStruct->phase_shift_idx));
#endif
		if (window_size_temp > window_size) {
			window_size = window_size_temp;
			spic_cyc_dly = spic_cyc_dly_temp;
			phase_shift_idx = FLASH_InitStruct->phase_shift_idx;
		} else {
			/*if window_size == window_size_temp == 0, shall not break. if window_size == window_size_temp != 0, means window is already max size.*/
			if (window_size_temp != window_size) {
				break;
			}
		}

		FLASH_InitStruct->phase_shift_idx = 0;
	}
	if (window_size > 0) {
		RTK_LOGI(TAG, "calibration_ok:[%lu:%lu:%lu] \n", spic_cyc_dly, window_size, phase_shift_idx);

		FLASH_InitStruct->phase_shift_idx = phase_shift_idx;
		FLASH_InitStruct->FLASH_rd_sample_dly_cycle_cal = spic_cyc_dly;
		FLASH_InitStruct->FLASH_rd_sample_dly_cycle = FLASH_InitStruct->FLASH_rd_sample_dly_cycle_cal;

		return TRUE;
	} else {
		FLASH_InitStruct->phase_shift_idx = 0;
		FLASH_InitStruct->FLASH_rd_sample_dly_cycle = SPIC_LOWSPEED_SAMPLE_PHASE;
	}

	return FALSE;
}

int data_flash_calibration_highspeed(FlashDivInt_E div)
{
	int Ret = RTK_SUCCESS;
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;
	u8 spic_mode = FLASH_InitStruct->FLASH_cur_bitmode;

	/* SPIC clock switch to PLL */
	DATA_FLASH_PLLInit_ClockDiv(div);

	FLASH_InitStruct->debug = FLASH_CALIBRATION_DEBUG;
	if (_data_flash_calibration_highspeed(spic_mode, div) == TRUE) {
		/* we should open calibration new first, and then set phase index */
		DATA_FLASH_CalibrationNewCmd(ENABLE);
		DATA_FLASH_CalibrationPhaseIdx(FLASH_InitStruct->phase_shift_idx);

		/* this code is rom code, so it is safe */
		DATA_FLASH_Init(spic_mode);
		RTK_LOGI(TAG, "DATA FLASH CALIB[0x%x OK]\n", div);
	} else {
		/* calibration fail, revert SPIC clock to XTAL */
		RCC_PeriphClockSource_PSRAM(BIT_LSYS_CKSL_PSRAM_LBUS);
		SPIC_COMBO->TPR1 = (SPIC_COMBO->TPR1 & ~MASK_CR_ACTIVE_SETUP) | CR_ACTIVE_SETUP(1);

		RTK_LOGE(TAG, "DATA FLASH CALIB[0x%x FAIL]\n", div);
		Ret = RTK_FAIL;
	}

	return Ret;
}

int data_flash_handshake_highspeed(FlashDivInt_E div)
{
	u8 Dphy_Dly_Cnt = 3; /* DD recommend this value */
	int Ret = RTK_FAIL;

	/* SPIC clock switch to PLL */
	DATA_FLASH_PLLInit_ClockDiv(div);

	/* Open handshake function */
	DATA_FLASH_Read_HandShake_Cmd(Dphy_Dly_Cnt, ENABLE);

	if (DATA_FLASH_Read_DataIsRight()) {
		Ret = RTK_SUCCESS;
	} else {
		DATA_FLASH_Read_HandShake_Cmd(Dphy_Dly_Cnt, DISABLE);
		RCC_PeriphClockSource_PSRAM(BIT_LSYS_CKSL_PSRAM_LBUS);
		SPIC_COMBO->TPR1 = (SPIC_COMBO->TPR1 & ~MASK_CR_ACTIVE_SETUP) | CR_ACTIVE_SETUP(1);
	}

	RTK_LOGI(TAG, "DATA FLASH HandShake[0x%x %s]\n", div, Ret == RTK_SUCCESS ? "OK" : "FAIL");
	return Ret;
}

static void data_flash_get_vendor(void)
{
	u8 flash_ID[4];
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;

	/* Read flash ID */
	DATA_FLASH_RxCmd(FLASH_InitStruct->FLASH_cmd_rd_id, 3, flash_ID);
	RTK_LOGI(TAG, "Flash ID: %x-%x-%x\n", flash_ID[0], flash_ID[1], flash_ID[2]);

	DATA_FLASH_SIZE = (1 << (flash_ID[2] - 0x11));

	/* Get flash chip information */
	current_IC = flash_get_chip_info((flash_ID[2] << 16) | (flash_ID[1] << 8) | flash_ID[0]);
	if (current_IC == NULL) {
		RTK_LOGW(TAG,  "This flash type is not supported!\n");
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

static void data_flash_set_status_register(void)
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
	u8 status = 0;
	char *str[] = {"1IO", "2O", "2IO", "4O", "4IO"};
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;

	/* Try sequentially: 4IO, 4O, 2IO, 2O, 1bit */
	while (1) {
		/* 4IO or 2IO should enable dummy byte function */
		if ((spic_mode == Spic4IOBitMode) || (spic_mode == Spic2IOBitMode)) {
			FLASH_InitStruct->FLASH_dum_en = 1;
		} else {
			FLASH_InitStruct->FLASH_dum_en = 0;
		}

		if (FLASH_InitStruct->FLASH_Id == FLASH_ID_MICRON) {
			DATA_FLASH_RxCmd(0x85, 1, &status);

			status = (status & 0x0f) | (FLASH_InitStruct->FLASH_rd_dummy_cycle[spic_mode] << 4);
			DATA_FLASH_SetStatus(0x81, 1, &status);
		}

		DATA_FLASH_Init(spic_mode);

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
	u8 read_mode, flash_speed;
	u32 spic_ckd, pll_clk = PLL_ClkGet();

#if !defined(CONFIG_SECOND_FLASH_NOR)
	UNUSED(FLASH_InitStruct);
	UNUSED(read_mode);
	UNUSED(flash_speed);
	UNUSED(spic_ckd);
	UNUSED(pll_clk);
	return;
#endif

	DATA_FLASH_StructInit(FLASH_InitStruct);
	FLASH_InitStruct->FLASH_baud_rate = 1;
	FLASH_InitStruct->FLASH_baud_boot = 1;
	//FLASH_InitStruct->FLASH_addr_phase_len = ADDR_4_BYTE;

	read_mode = flash_get_readmode(Data_Flash_ReadMode);
	flash_speed = Data_Flash_Speed - 1;

	spic_ckd = CLKDIV_ROUND_UP(pll_clk, SPIC_CLK_LIMIT) - 1;
	flash_speed = MAX(flash_speed, spic_ckd);

	__disable_irq();

	if (flash_speed == spic_ckd) {
		/* CS setup time changed to 2 SPIC Clk for greater than tSLCH if SPIC is 200MHz */
		SPIC_COMBO->TPR1 = (SPIC_COMBO->TPR1 & ~MASK_CR_ACTIVE_SETUP) | CR_ACTIVE_SETUP(2);
	}

	/* Get flash ID to reinitialize FLASH_InitTypeDef structure */
	data_flash_get_vendor();

	/* Set flash status register: set QE, clear protection bits */
	data_flash_set_status_register();

	/* read data from data flash address0 as calib data when flash is in 1io 20M*/
	DCache_Invalidate(DATA_FLASH_BASE, 8);
	SPIC_DATA_FLASH_CALIB_PATTERN[0] = HAL_READ32(DATA_FLASH_BASE, 0x00);
	SPIC_DATA_FLASH_CALIB_PATTERN[1] = HAL_READ32(DATA_FLASH_BASE, 0x04);

	/* Set flash I/O mode and high-speed calibration */
	data_flash_rx_mode_switch(read_mode);

	if (SYSCFG_CHIPType_Get() != CHIP_TYPE_FPGA) {
		//data_flash_calibration_highspeed(flash_speed);
		data_flash_handshake_highspeed(flash_speed);
	}

	__enable_irq();
}

