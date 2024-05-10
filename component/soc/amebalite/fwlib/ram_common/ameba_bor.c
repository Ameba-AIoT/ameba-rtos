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
  * @param Thres_Fall: BOR falling threshold.
  *        This parameter can be one of the following values:
  *          @arg 0x9: 2.82
  *          @arg 0xA: 2.77
  *          @arg 0xB: 2.73
  *          @arg 0xC: 2.69
  *          @arg 0xD: 2.64
  *          @arg 0xE: 2.58
  *          @arg 0xF: 2.54
  * @param Thres_Rise: BOR rising threshold.
  *        This parameter can be one of the following values:
  *          @arg 0x7: 2.97
  *          @arg 0x8: 2.92
  *          @arg 0x9: 2.87
  *          @arg 0xA: 2.83
  *          @arg 0xB: 2.78
  *          @arg 0xC: 2.74
  *          @arg 0xD: 2.68
  * @note To make chip work normally, BOR_TH_LOW0 and BOR_TH_HIGH0 are not supported.
  * @retval None
  */
void BOR_ThresholdSet(u32 Thres_Fall, u32 Thres_Rise)
{
	u32 temp = 0;
	REGU_TypeDef *regu = REGU_BASE;
	assert_param(IS_BOR_TH_FALL(Thres_Fall));
	assert_param(IS_BOR_TH_RISE(Thres_Rise));

	temp = regu->REGU_AON_BOR_CTRL;
	temp &= (~(REGU_MASK_REG_BOD_R_4_0 | REGU_MASK_REG_BOD_F_4_0));
	temp |= (REGU_REG_BOD_F_4_0(Thres_Fall) | \
			 REGU_REG_BOD_R_4_0(Thres_Rise));
	regu->REGU_AON_BOR_CTRL = temp;
}

/**
  * @brief  BOD Enable,
  * @param  newstatus: can be one of the following values:
  *		 @arg ENABLE: enable bod.
  *		 @arg DISABLE: disable bod.
  * @note NA
  */
void BOR_Enable(u32 newstatus)
{

	REGU_TypeDef *regu = REGU_BASE;

	if (newstatus == ENABLE) {
		regu->REGU_AON_BOR_CTRL |= REGU_BIT_REG_BOD_R_5;
	} else {
		regu->REGU_AON_BOR_CTRL &= ~REGU_BIT_REG_BOD_R_5;
	}
}

/**
  * @brief  Set BOR debounce cycle.
  * @param  Dbnc_Value: debounce cycle, in unit of ANA4M clock cycles.
  *            @arg 000: 0us
  *            @arg 001: 100us
  *            @arg 010: 500us
  *            @arg 011: 1000us
  *            @arg 100: 2000us
  *            @arg 101: 4000us
  *            @arg 110: 8000us
  *            @arg 111: 16000us
  * @retval   None
  */
void BOR_DbncSet(u32 Dbnc_Value)
{
	u32 temp = 0;
	assert_param(IS_BOR_DBNC_THRES(Dbnc_Value));
	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_BOR_CTRL);
	temp &= (~AON_MASK_BOR_TDBC);
	temp |= AON_BOR_TDBC(Dbnc_Value);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_BOR_CTRL, temp);
}

/**
  * @brief  Clear BOR interrupt.
  * @retval   None
  */
void BOR_ClearINT(void)
{
	u32 temp = 0;

	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_BOR_ISR);
	temp |= AON_BIT_BOR_ISR;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_BOR_ISR, temp);
}

/**
  * @brief  Enable BOR.
  * @param  Option: BOR mode.
  *          This parameter can be one of the following values:
  *            @arg BOR_RESET: BOR reset mode
  *            @arg BOR_INTR: BOR interrupt mode
  * @retval   None
  */
void BOR_ModeSet(u32 Option)
{
	u32 temp = 0;

	if (BOR_INTR == Option) {
		temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_BOR_CTRL);
		temp |= AON_BIT_BOR_INTR_MODE;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_BOR_CTRL, temp);

		/* Please set wake event in sleep_wevent_config[] in ameba_sleepcfg.c */
//		temp = HAL_READ32(PMC_BASE, WAK_MASK0_NP);
//		temp |= PMC_WAK_NP_IMR_31_0(WAKE_SRC_BOR);
//		HAL_WRITE32(PMC_BASE, WAK_MASK0_NP, temp);
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
