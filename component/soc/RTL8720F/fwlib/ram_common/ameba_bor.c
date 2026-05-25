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
 * @param Thres_Rise BOR rising threshold.
 * @note For chip to work normally, BOR_TH_LOW0 and BOR_TH_HIGH0 are not supported.
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
 * @param NewStatus This parameter can be one of the following values:
 * 		@arg ENABLE: Enable bod.
 * 		@arg DISABLE: Disable bod.
 * @return None
 */
void BOR_Enable(u32 NewStatus)
{
	if (NewStatus == ENABLE) {
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
		temp |= PMC_WAK_NP_IMR_29_0(WAKE_SRC_BOR);
		HAL_WRITE32(PMC_BASE, WAK_MASK0_NP, temp);
	} else {
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
