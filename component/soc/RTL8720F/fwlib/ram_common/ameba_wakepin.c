/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"


/**
  * @brief  set REGU wakeup_pin (wakepin mux to 5 pads).
  * @param  PinMask: aon wakepin index
  *		This parameter can be one of the following values:
  *		 @arg WAKEPIN_0 : PA19
  *		 @arg WAKEPIN_1 : PA20
  *		 @arg WAKEPIN_2 : PA21
  *		 @arg WAKEPIN_3 : PA22
  * @param  level: aon wakepin level
  *		This parameter can be one of the following values:
  *		 @arg 1 : rising edge wakeup
  *		 @arg 0 : falling edge wakeup
  * @note wakeup state: sleep PG & CG & deep sleep
  * @retval None
  */
void Wakepin_Setting(u32 PinIdx, u32 level)
{
	u32 Rtemp = 0;

	/* disable wakepin */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL);
	Rtemp &= (~(AON_GPIO_WAKE_FEN(BIT(PinIdx))));
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL, Rtemp);

	/* set edge and internal PU/PD resistence */
	Rtemp &= ~(AON_GPIO_WAKDET_POLY(BIT(PinIdx))); /* clear level, set to low active */
	if (level == HIGH_LEVEL_WAKEUP) {
		Rtemp |= AON_GPIO_WAKDET_POLY(BIT(PinIdx)); /* if rising edge active */
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL, Rtemp);

	/* enable wakepin */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL);
	Rtemp |= (AON_GPIO_WAKE_FEN(BIT(PinIdx)));
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL, Rtemp);
}

/**
 * @brief Wakepin 0 can be selected from any of the following pins
 *
 * @param pin_sel:
 *		 @arg 0 : PA19
 *		 @arg 1 : PA20
 *		 @arg 2 : PA21
 *		 @arg 3 : PA22
 */
void Wakepin_Select(u32 pin_sel)
{
	u32 Rtemp = 0;

	/* disable wakepin */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL);
	Rtemp &= ~(AON_GPIO_WAKE_FEN(0));
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL, Rtemp);

	/* select pin */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL);
	Rtemp &= ~AON_MASK_GPIO_WAK_SEL;
	Rtemp |= AON_GPIO_WAK_SEL(pin_sel);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL, Rtemp);

	/* enable wakepin */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL);
	Rtemp |= AON_GPIO_WAKE_FEN(0);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL, Rtemp);
}
/**
  * @brief  set REGU wakeup_pin debounce cycle.
  * @param  Dbnc_cycle: wakeup_pin debounce cycle, unit is 100k/128k.
  *		This parameter must be set to a value in the 0-0x7FFF range.
  * @param  Status: enable or disable wakepin debounce function
  * @note Dbnc_cycle is valid when Status is set ENABLE.
  * @retval None
  */
void Wakepin_Debounce_Setting(u32 Dbnc_cycle, u32 Status)
{
	u32 Rtemp = 0;
	u32 FEN_backup = 0;
	assert_param(Dbnc_cycle <= 0x7FFF);

	/* disable wakepin */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL);
	FEN_backup = AON_GET_GPIO_WAKE_FEN(Rtemp);
	Rtemp &= (~AON_MASK_GPIO_WAKE_FEN);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL, Rtemp);

	/* set gpio debounce cycle */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL);
	if (ENABLE == Status) {
		Rtemp &= (~AON_MASK_GPIO_DBNC_CYC);
		Rtemp |= AON_GPIO_DBNC_CYC(Dbnc_cycle);
		Rtemp |= AON_BIT_GPIO_DBNC_FEN;
	} else {
		Rtemp &= (~AON_BIT_GPIO_DBNC_FEN);
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL, Rtemp);

	/* enable wakepin */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL);
	Rtemp |= AON_GPIO_WAKE_FEN(FEN_backup);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_GPIO_CTRL, Rtemp);
}

/**
  * @brief  get wakepin index.
  * @param  None
  * @retval BIT(0): wakepin0, BIT(1): wakepin1, BIT(2): wakepin2, BIT(3): wakepin3
  */
int WakePin_Get_Idx(void)
{
	int WakePin = 0;

	WakePin = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_AON_WAK_EVT) & \
			  (AON_BIT_GPIO_PIN3_WAKDET_EVT | AON_BIT_GPIO_PIN2_WAKDET_EVT | \
			   AON_BIT_GPIO_PIN1_WAKDET_EVT | AON_BIT_GPIO_PIN0_WAKDET_EVT);
	WakePin = WakePin >> 4;

	return WakePin;
}


/**
  * @brief  Clear wakepin interrupt.
  * @retval   None
  */
void WakePin_ClearINT(u32 wakepin)
{
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_GPIO_WAKE_ISR, wakepin);
}
