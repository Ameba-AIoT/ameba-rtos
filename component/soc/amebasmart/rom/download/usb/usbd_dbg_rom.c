/**
  ******************************************************************************
  * @file    usbd_dbg_rom.c
  * @author  Realsil WLAN5 Team
  * @version V1.0.0
  * @date    2020-12-18
  * @brief   This file provides the GPIO debug functionalities for USB download
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */

#include "usbd_hal_rom.h"
#include "usbd_dbg_rom.h"

/* Private defines -----------------------------------------------------------*/

#define USBD_DEBUG_GPIO		_PA_6

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init USB download debug GPIO
  * @param  None
  * @retval None
  */
USBD_WEEK USBD_TEXT_SECTION
void USBD_DebugInit(void)
{
	if ((BKUP_Read(0) & BKUP_BIT_UARTBURN_DEBUG) == 0) {
		return;
	}

	RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
	Pinmux_Config(USBD_DEBUG_GPIO, PINMUX_FUNCTION_GPIO);

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin = USBD_DEBUG_GPIO;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_Init(&GPIO_InitStruct);
}

/**
  * @brief  USB download debug
  * @param  code : debug code
  * @param  delay : whether delay the GPIO active output, used to distinguish debug messages from error messages
  * @retval None
  */
USBD_WEEK USBD_TEXT_SECTION
void USBD_Debug(u32 code, u32 delay)
{
	u32 cnt = code;
	int i;

	if ((BKUP_Read(0) & BKUP_BIT_UARTBURN_DEBUG) == 0) {
		return;
	}

	while (1) {
		i = 0;
		GPIO_WriteBit(USBD_DEBUG_GPIO, 0);
		if (delay) {
			while (i++ < 10) {
				asm("nop");
			}
		}
		GPIO_WriteBit(USBD_DEBUG_GPIO, 1);

		cnt--;
		if (cnt == 0) {
			break;
		}
	}
}

/**
  * @brief  USB download debug config
  * @param  enable : 0 - Disable; 1 - Enable
  * @retval None
  */
USBD_WEEK USBD_TEXT_SECTION
void USBD_Debug_Config(u8 enable)
{
	if (enable) {
		DBG_ERR_MSG_ON(MODULE_USB_OTG);
		DBG_WARN_MSG_OFF(MODULE_USB_OTG);
		DBG_INFO_MSG_OFF(MODULE_USB_OTG);
	} else {
		DBG_ERR_MSG_OFF(MODULE_USB_OTG);
		DBG_WARN_MSG_OFF(MODULE_USB_OTG);
		DBG_INFO_MSG_OFF(MODULE_USB_OTG);
	}
}

