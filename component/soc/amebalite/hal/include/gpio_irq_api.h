/** mbed Microcontroller Library
  ******************************************************************************
  * @file    gpio_irq_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides following mbed GPIO IRQ API
  ******************************************************************************
  * @attention
  *
  * Copyright(c) 2006 - 2022 Realtek Corporation. All rights reserved.
  *
  * SPDX-License-Identifier: Apache-2.0
  *
  * Licensed under the Apache License, Version 2.0 (the License); you may
  * not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an AS IS BASIS, WITHOUT
  * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
#ifndef MBED_GPIO_IRQ_API_H
#define MBED_GPIO_IRQ_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif
/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_GPIO
 *  @brief    MBED_GPIO driver modules.
 *  @{
 */

/** @addtogroup MBED_GPIO_Exported_Types MBED_GPIO Exported Types
  * @{
  */

/** @defgroup MBED_GPIO_Enumeration_Type Enumeration Type
  * @{
  */

/**
  * @brief enum gpio_irq_event
  */
typedef enum {
	IRQ_NONE,
	/* for edge trigger irq */
	IRQ_RISE,
	IRQ_FALL,

	/* for level trigger irq */
	IRQ_LOW = 3,
	IRQ_HIGH = 4,

	/* for dual-edge trigger irq */
	IRQ_FALL_RISE = 5

} gpio_irq_event;
/** @}*/

/** @}*/
/** @}*/
/** @}*/

/*
 * Note:
 *
 * This 32-bit structure contains the following fields:
 *
 * uint32_t irq_event_type : 2;  // Bits [1:0] - Event type
 *                                  1: Rising edge, high level
 *                                  2: Falling edge, low level
 *
 * uint32_t reserved       : 14; // Bits [15:2] - Reserved
 *
 * uint32_t pin_num        : 5;  // Bits [20:16] - Pin number
 *
 * uint32_t port_num       : 2;  // Bits [22:21] - Port number
 *
 * uint32_t unused         : 9;  // Bits [31:23] - Unused
 */
typedef void (*gpio_irq_handler)(uint32_t id, uint32_t event);

int gpio_irq_init(gpio_irq_t *obj, PinName pin, gpio_irq_handler handler, uint32_t id);
void gpio_irq_free(gpio_irq_t *obj);
void gpio_irq_set(gpio_irq_t *obj, gpio_irq_event event, uint32_t enable);
void gpio_irq_enable(gpio_irq_t *obj);
void gpio_irq_disable(gpio_irq_t *obj);
void gpio_irq_set_event(gpio_irq_t *obj, gpio_irq_event event);

void gpio_irq_deinit(gpio_irq_t *obj);
void gpio_irq_pull_ctrl(gpio_irq_t *obj, PinMode pull_type);

#ifdef __cplusplus
}
#endif

#endif
