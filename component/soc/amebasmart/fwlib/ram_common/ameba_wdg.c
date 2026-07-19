/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup WDG WDG
* @brief WDG driver modules
* @{
*/

/* Exported functions ------------------------------------------------------------*/
/** @defgroup WDG_Exported_Functions WDG Exported Functions
  * @{
  */

/**
  * @}
  */

/**
  * @brief  Set the structure according to the parameters passed in.
  * @param  WDG_InitStruct: pointer to a WDG_InitTypeDef structure that contains
  *         the configuration information for the WDG peripheral.
  * @param  timeout: WDG timeout (ms).
  * @param  window: WDG timeout.
  * @param  eicnt:  WDG early intrrupt cnt.
  * @note   For compatibility with Zephyr.
  * @retval None
  */
void WDG_StructMemValueSet(WDG_InitTypeDef *WDG_InitStruct, u32 window, u32 timeout, u32 eicnt)
{
	WDG_StructInit(WDG_InitStruct);
	WDG_InitStruct->Window = window;
	WDG_InitStruct->Timeout = timeout;
	WDG_InitStruct->EIMOD = ENABLE;
	WDG_InitStruct->EICNT = eicnt;
}

/**
  * @brief  Disable/Enable WDG
  * @param  WDG: WDG peripheral base.
  * @param  NewState: new state of the WDG.
  *          This parameter can be: ENABLE or DISABLE
  * @note
  * 	- WDG can't disabled by software, for compatibility with Zephyr.
  * @retval None
  */
void WDG_Cmd(WDG_TypeDef *WDG, u32 NewState)
{
	assert_param(IS_WDG_ALL_PERIPH(WDG));
	if (NewState == ENABLE) {
		WDG_Wait_Busy(WDG);
		WDG->WDG_MKEYR = WDG_FUNC_EN;
	} else {
		RTK_LOGE(NOTAG, "WDG can't disabled by software\n");
	}
}

/**
  * @}
  */

/**
  * @}
  */
