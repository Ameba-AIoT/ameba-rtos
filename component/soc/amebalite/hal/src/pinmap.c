/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */
#include "objects.h"
#include "pinmap.h"

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_PINMAP
 *  @brief      MBED_PINMAP driver modules.
 *  @{
 */

/** @defgroup MBED_PINMAP_Exported_Functions MBED_PINMAP Exported Functions
  * @{
  */

/**
  *  @brief Configure pin enable and function type.
  *  @param PinName : value of @ref PinName.
  *  @param PinFunc : the function type for the pin, value of @ref PINMUX_Function_definitions.
  *  @retval None
  */
void pin_function(PinName pin, int function)
{
	u8 pin_index = 0;
	u32 Temp = 0;

	assert_param(pin != NC);

	/* 32bit per one pad */
	/* get PADCTR index*/
	pin_index = (pin) & 0x7F;

	/* get PADCTR */
	Temp = PINMUX->REG_GPIOx[pin_index];

	/* set needs function */
	Temp &= ~PAD_MASK_GPIOx_SEL;
	Temp |= PAD_GPIOx_SEL(function);

	/* set PADCTR register */
	PINMUX->REG_GPIOx[pin_index] = Temp;
}

/**
  *  @brief Configure pin Configure pin pull-up/pull-down mode.
  *  @param PinName : value of @ref PinName.
  *  @param PinFunc : the function type for the pin, value of @ref PinMode.
  *  @retval None
  */
void pin_mode(PinName pin, PinMode mode)
{
	u8 pull_type;

	switch (mode) {
	case PullNone:
		pull_type = GPIO_PuPd_NOPULL;
		PAD_PullCtrl((u32)pin, (u32)pull_type);
		break;

	case PullDown:
		pull_type = GPIO_PuPd_DOWN;
		PAD_PullCtrl((u32)pin, (u32)pull_type);
		break;

	case PullUp:
		pull_type = GPIO_PuPd_UP;
		PAD_PullCtrl((u32)pin, (u32)pull_type);
		break;

	case PullNoneSlp:/* No driver -> Input & High Impendance */
		pull_type = GPIO_PuPd_NOPULL;
		PAD_SleepPullCtrl((u32)pin, (u32)pull_type);
		break;

	case PullDownSlp:
		pull_type = GPIO_PuPd_DOWN;
		PAD_SleepPullCtrl((u32)pin, (u32)pull_type);
		break;

	case PullUpSlp:
		pull_type = GPIO_PuPd_UP;
		PAD_SleepPullCtrl((u32)pin, (u32)pull_type);
		break;

	default:
		pull_type = GPIO_PuPd_NOPULL;
		PAD_PullCtrl((u32)pin, (u32)pull_type);
		break;
	}
}
/** @} */
/** @} */
/** @} */