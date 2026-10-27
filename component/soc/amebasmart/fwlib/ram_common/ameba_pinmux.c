/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PINMUX PINMUX
  * @{
  */

/** @defgroup PAD_Exported_Functions PAD Exported Functions
  * @{
  */

/**
  * @brief  Set the PAD slew rate control status.
  * @param  PinName: Specifies the target pin. This parameter must be one of the values
  *                  defined in @ref PINMUX_Pin_Name_definitions.
  * @param  NewState: Specifies the slew rate control status. Refer to the datasheet for detail informations.
  *                   This parameter can be one of the following values:
  *           @arg PAD_SlewRate_Slow
  *           @arg PAD_SlewRate_Fast
  */
void PAD_SlewRateCtrl(u8 PinName, u32 NewState)
{
	u32 Temp = 0;

	/* get PADCTR */
	Temp = PINMUX->PADCTR[PinName];

	if (NewState == PAD_SlewRate_Slow) {
		Temp |= PAD_BIT_GPIOx_SR;
	} else {
		Temp &= ~PAD_BIT_GPIOx_SR;
	}

	/* set PADCTR register */
	PINMUX->PADCTR[PinName] = Temp;
}

/**
  *  @brief Set the PAD Schmit control status.
  *  @param PinName: Value of @ref PINMUX_Pin_Name_definitions.
  *  @param NewState: Specifies the control status. Refer to the datasheet for detail informations.
  *                   This parameter can be: ENABLE or DISABLE.
  */
void PAD_SchmitCtrl(u8 PinName, u32 NewState)
{
	u32 Temp = 0;

	/* get PADCTR */
	Temp = PINMUX->PADCTR[PinName];

	if (NewState == ENABLE) {
		Temp |= PAD_BIT_GPIOx_SMT;
	} else {
		Temp &= ~PAD_BIT_GPIOx_SMT;
	}

	/* set PADCTR register */
	PINMUX->PADCTR[PinName] = Temp;
}

/** @} */

/** @} */

/** @} */
