/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup GPIO GPIO
  * @{
  */

static GPIO_TypeDef *GPIO_PortAddrGet(u32 GPIO_Port)
{
	GPIO_TypeDef *GPIO = NULL;

	assert_param(IS_GPIO_PORT_NUM(GPIO_Port));

	if (TrustZone_IsSecure()) {
		GPIO = GPIO_PORTx_S[GPIO_Port];
	} else {
		GPIO = GPIO_PORTx[GPIO_Port];
	}

	return GPIO;
}

/** @defgroup GPIO_Exported_Functions GPIO Exported Functions
  * @{
  */

/**
  * @brief  Initialize the GPIO registers according to the specified parameters
  *         in GPIO_InitStruct.
  * @param  GPIO_InitStruct Pointer to a GPIO_InitTypeDef structure that contains
  *         the configuration information for the GPIO peripheral.
  * @note   OpenDrain output: on drive + OUT + GPIO[gpio_bit] = 0, pin should have pull-up resistor
  * @note   Input HighZ: no drive + IN, user can input high or low use this pin
  */
void GPIO_Init(GPIO_InitTypeDef  *GPIO_InitStruct)
{
	/* open gpio function and clock */
	//RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

	assert_param(GPIO_InitStruct->GPIO_Mode <= GPIO_Mode_INT);

	/* GPIO Pad shouddown control: Turn on the corresponding GPIO Pad and pinmux to GPIO */
	Pinmux_Config(GPIO_InitStruct->GPIO_Pin, PINMUX_FUNCTION_GPIO);

	if (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_INT) {
		GPIO_Direction(GPIO_InitStruct->GPIO_Pin, GPIO_Mode_IN);
		PAD_PullCtrl(GPIO_InitStruct->GPIO_Pin, GPIO_InitStruct->GPIO_PuPd);

		GPIO_INTMode(GPIO_InitStruct->GPIO_Pin, ENABLE, GPIO_InitStruct->GPIO_ITTrigger,
					 GPIO_InitStruct->GPIO_ITPolarity, GPIO_InitStruct->GPIO_ITDebounce);
	} else {
		GPIO_INTMode(GPIO_InitStruct->GPIO_Pin, DISABLE, 0, 0, 0);

		if (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_OUT) {
			GPIO_Direction(GPIO_InitStruct->GPIO_Pin, GPIO_Mode_OUT);
		} else if (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_IN) {
			GPIO_Direction(GPIO_InitStruct->GPIO_Pin, GPIO_Mode_IN);
			PAD_PullCtrl(GPIO_InitStruct->GPIO_Pin, GPIO_InitStruct->GPIO_PuPd);
		}
	}
}

/**
  * @brief  Get the interrupt status.
  * @param  GPIO_Port Specifies port number, which can be any GPIO_PORT_X defined in GPIO_Port_Type.
  * @return The interrupt status of the specified port pins
  */
u32 GPIO_INTStatusGet(u32 GPIO_Port)
{
	GPIO_TypeDef *GPIO = GPIO_PortAddrGet(GPIO_Port);

	return GPIO->GPIO_INT_STATUS;
}

/**
  * @brief  Clear the edge interrupt status.
  * @param  GPIO_Port Specifies port number, which can be any GPIO_PORT_X defined in GPIO_Port_Type.
  */
void GPIO_INTStatusClearEdge(u32 GPIO_Port)
{
	GPIO_TypeDef *GPIO = NULL;
	u32 IntStatus;

	GPIO = GPIO_PortAddrGet(GPIO_Port);

	IntStatus = GPIO->GPIO_INT_STATUS;

	/* Clear pending edge interrupt */
	GPIO->GPIO_INT_EOI = IntStatus;
}

/** @} */

/** @} */

/** @} */
