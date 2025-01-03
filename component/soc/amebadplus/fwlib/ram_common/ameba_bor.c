/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/** @addtogroup Ameba_Periph_Driver
 * @{
 */

/** @defgroup BOR
 * @brief BOR driver modules
 * @{
 */

/* Exported functions --------------------------------------------------------*/
/** @defgroup BOR_Exported_Functions BOR Exported Functions
 * @{
 */

/**
 * @brief Set BOR rising and falling threshold.
 * @param Thres_Fall BOR falling threshold.
 * 		This parameter can be one of the following values:
 * 		@arg 00010: 2.82
 * 		@arg 00011: 2.78
 * 		@arg 00100: 2.74
 * 		@arg 00101: 2.69
 * 		@arg 00110: 2.65
 * 		@arg 00111: 2.61
 * 		@arg 01000: 2.56
 * 		@arg 01001: 2.53
 * 		@arg 01010: 2.49
 * 		@arg 01011: 2.45
 * 		@arg 01100: 2.41
 * 		@arg 01101: 2.37
 * 		@arg 01110: 2.32
 * 		@arg 01111: 2.21
 * 		@arg 10000: 2.16
 * 		@arg 10001: 2.12
 * 		@arg 10010: 2.08
 * 		@arg 10011: 2.04
 * 		@arg 10100: 2.00
 * 		@arg 10101: 1.96
 * 		@arg 10110: 1.93
 * 		@arg 10111: 1.88
 * 		@arg 11000: 1.83
 * 		@arg 11001: 1.79
 * 		@arg 11010: 1.75
 * 		@arg 11011: 1.71
 * 		@arg 11100: 1.69
 * 		@arg 11101: 1.65
 * 		@arg 11110: 1.61
 * 		@arg 11111: 1.58
 * @param Thres_Rise: BOR rising threshold.
 * 		This parameter can be one of the following values:
 * 		@arg 00000: 2.94
 * 		@arg 00001: 2.86
 * 		@arg 00010: 2.83
 * 		@arg 00011: 2.79
 * 		@arg 00100: 2.75
 * 		@arg 00101: 2.71
 * 		@arg 00110: 2.66
 * 		@arg 00111: 2.62
 * 		@arg 01000: 2.58
 * 		@arg 01001: 2.54
 * 		@arg 01010: 2.50
 * 		@arg 01011: 2.46
 * 		@arg 01100: 2.42
 * 		@arg 01101: 2.38
 * 		@arg 01110: 2.33
 * 		@arg 01111: 2.22
 * 		@arg 10000: 2.17
 * 		@arg 10001: 2.13
 * 		@arg 10010: 2.09
 * 		@arg 10011: 2.05
 * 		@arg 10100: 2.01
 * 		@arg 10101: 1.97
 * 		@arg 10110: 1.94
 * 		@arg 10111: 1.89
 * 		@arg 11000: 1.85
 * 		@arg 11001: 1.80
 * 		@arg 11010: 1.77
 * 		@arg 11011: 1.73
 * 		@arg 11100: 1.70
 * 		@arg 11101: 1.66
 * @note For chip to work normally, it is advised to make rising threshold 200mV larger than falling threshold at least.
 * @return None
 */
void BOR_ThresholdSet(u32 Thres_Fall, u32 Thres_Rise)
{
	u32 temp;
	assert_param(IS_BOR_TH_FALL(Thres_Fall));
	assert_param(IS_BOR_TH_RISE(Thres_Rise));

	temp = LDO_BASE->LDO_BOD;
	temp &= ~(LDO_MASK_REG_BOD_F | LDO_MASK_REG_BOD_R);
	temp |= LDO_REG_BOD_F(Thres_Fall) | LDO_REG_BOD_R(Thres_Rise);
	LDO_BASE->LDO_BOD = temp;
}

/**
 * @brief Enable BOD or not.
 * @param newstatus This parameter can be one of the following values:
 * 		@arg ENABLE: Enable bod.
 * 		@arg DISABLE: Disable bod.
 * @return None
 */
void BOR_Enable(u32 newstatus)
{
	if (newstatus == ENABLE) {
		LDO_BASE->LDO_BOD |= LDO_BIT_REG_BOD_R_EN;
	} else {
		LDO_BASE->LDO_BOD &= ~LDO_BIT_REG_BOD_R_EN;
	}
}

/**
 * @brief Set BOR interrupt mode debounce cycle.
 * @param Option BOR mode. This parameter can be one of the following values:
 * 		@arg BOR_RESET: BOR reset mode
 * 		@arg BOR_INTR: BOR interrupt mode
 * @param Dbnc_Value Debounce cycle in unit of ANA4M clock cycles.
 * 		@arg 000: 0us
 * 		@arg 001: 100us
 * 		@arg 010: 500us
 * 		@arg 011: 1000us
 * 		@arg 100: 2000us
 * 		@arg 101: 4000us
 * 		@arg 110: 8000us
 * 		@arg 111: 16000us
 * @return None
 */
void BOR_DbncSet(u32 Option, u32 Dbnc_Value)
{
	u32 temp = 0;
	if (BOR_INTR == Option) {
		assert_param(IS_BOR_INTR_DBNC_THRES(Dbnc_Value));
		temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_BOR_CTRL);
		temp &= ~AON_MASK_BOR_TDBC;
		temp |= AON_BOR_TDBC(Dbnc_Value);
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_BOR_CTRL, temp);
	}
}

/**
 * @brief Clear BOR interrupt.
 * @return None
 */
void BOR_ClearINT(void)
{
	u32 temp = 0;

	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_BOR_ISR);
	temp |= AON_BIT_BOR_ISR;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_BOR_ISR, temp);
}

/**
 * @brief Enable BOR.
 * @param Option BOR mode. This parameter can be one of the following values:
 * 		@arg BOR_RESET: BOR reset mode
 * 		@arg BOR_INTR: BOR interrupt mode
 * @return None
 */
void BOR_ModeSet(u32 Option)
{
	u32 temp = 0;

	if (BOR_INTR == Option) {
		temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_BOR_CTRL);
		temp |= AON_BIT_BOR_INTR_MODE;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_BOR_CTRL, temp);

		temp = HAL_READ32(PMC_BASE, WAK_MASK0_NP);
		temp |= PMC_WAK_KM0_IMR_31_0(WAKE_SRC_BOR);
		HAL_WRITE32(PMC_BASE, WAK_MASK0_NP, temp);
	} else {
		RCC_PeriphClockCmd(APBPeriph_BOR, APBPeriph_CLOCK_NULL, ENABLE);
		temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_BOR_CTRL);
		temp &= ~AON_BIT_BOR_INTR_MODE;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_BOR_CTRL, temp);
	}
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
