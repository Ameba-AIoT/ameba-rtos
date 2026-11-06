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

/**
  * @brief  Get the pull type of the specified PAD.
  * @param  pin_name: Specifies the target pin. This parameter must be one of the values
  *                   defined in @ref PINMUX_Pin_Name_definitions.
  * @note   For compatibility with Zephyr. Only port A and port B are available.
  * @retval GPIO_PuPd_UP, GPIO_PuPd_DOWN or GPIO_PuPd_NOPULL
  */
u8 PAD_PullCtrlGet(u8 pin_name)
{
	u32 reg_pu = 0;
	u32 reg_pd = 0;
	u8 port_num = PORT_NUM(pin_name);
	u8 pin_num = PIN_NUM(pin_name);

	switch (port_num) {
	case 0:	/* GPIO-A */
		reg_pu = HAL_READ32(PINMUX_REG_BASE, REG_PAD_PU_1);
		reg_pd = HAL_READ32(PINMUX_REG_BASE, REG_PAD_PD_1);
		break;
	case 1:	/* GPIO-B */
		reg_pu = HAL_READ32(PINMUX_REG_BASE, REG_PAD_PU_2);
		reg_pd = HAL_READ32(PINMUX_REG_BASE, REG_PAD_PD_2);
		break;
	default:
		return GPIO_PuPd_NOPULL;
	}

	u8 pu_bit = (reg_pu >> pin_num) & 0x1;
	u8 pd_bit = (reg_pd >> pin_num) & 0x1;

	if (pu_bit && !pd_bit) {
		return GPIO_PuPd_UP;
	} else if (!pu_bit && pd_bit) {
		return GPIO_PuPd_DOWN;
	} else {
		return GPIO_PuPd_NOPULL;
	}
}

/** @} */

/** @} */
