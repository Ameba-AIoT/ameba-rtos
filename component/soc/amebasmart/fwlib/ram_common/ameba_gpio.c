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
	assert_param(IS_GPIO_PORT_NUM(GPIO_Port));

	return GPIO_PORTx[GPIO_Port];
}

/** @defgroup GPIO_Exported_Functions GPIO Exported Functions
  * @{
  */

/**
  * @brief  Get the interrupt status.
  * @param  GPIO_Port Specifies port number, which can be any GPIO_PORT_X defined in @ref GPIO_Port_Type.
  * @return The interrupt status of the specified port pins
  */
u32 GPIO_INTStatusGet(u32 GPIO_Port)
{
	GPIO_TypeDef *GPIO = GPIO_PortAddrGet(GPIO_Port);

	return GPIO->GPIO_INT_STATUS;
}

/**
  * @brief  Clear the edge interrupt status.
  * @param  GPIO_Port Specifies port number, which can be any GPIO_PORT_X defined in @ref GPIO_Port_Type.
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

/**
  * @brief  Get the data direction of a GPIO pin.
  * @param  port Port number, which can be any GPIO_PORT_X defined in @ref GPIO_Port_Type.
  * @param  pin_mask Bitmask with exactly one bit set, identifying the pin.
  * @return GPIO_Mode_OUT if the pin is configured as output, GPIO_Mode_IN otherwise.
  */
u32 GPIO_DirectionGet(u8 port, u32 pin_mask)
{
	GPIO_TypeDef *GPIO = GPIO_PortAddrGet(port);

	assert_param(pin_mask != 0 && (pin_mask & (pin_mask - 1)) == 0);

	return (GPIO->PORT[0].GPIO_DDR & pin_mask) ? GPIO_Mode_OUT : GPIO_Mode_IN;
}

/** @} */

/** @} */

/** @} */
