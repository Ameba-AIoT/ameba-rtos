/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "PLL";

void PLL_Enable(u8 pll_name)
{
	/* RL6955 no software required to set PLL */
	UNUSED(pll_name);
}

void PLL_Disable(u8 pll_name)
{
	/* RL6955 no software required to set PLL */
	UNUSED(pll_name);
}

/**
  * @brief  Set PLL clock.
  * @param  pll_name: This parameter can be one of the following values:
  *		 @arg USB_PLL
  *		 @arg SYS_PLL
  * @param  pll_freq: This parameter is used to set PLL frequency, for example PllClk=600000000 means 600MHz
  */
void PLL_SetFreq(u8 pll_name, u32 pll_freq)
{
	/* RL6955 no software required to set PLL */
	UNUSED(pll_name);
	UNUSED(pll_freq);
}

/**
  * @brief  Get PLL clock.
  * @param  pll_name: This parameter can be one of the following values:
  *		 @arg USB_PLL
  *		 @arg SYS_PLL
  * @return  target pll freq in Hz
  */
u32 PLL_GetFreq(u8 pll_name)
{
	if (pll_name == SYS_PLL) {
		return PLL_400M;
	} else if (pll_name == USB_PLL) {
		return PLL_960M;
	} else {
		RTK_LOGE(TAG, "invalid pll name:%d \n", pll_name);
		assert_param(0);
	}
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
