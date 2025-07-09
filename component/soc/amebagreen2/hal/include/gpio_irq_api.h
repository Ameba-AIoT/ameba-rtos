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
 *  @brief      MBED_GPIO driver modules.
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
	IRQ_RISE,
	IRQ_FALL,

	/* merge irq event from gpio_irq_event_ex */
#if 1
	IRQ_LOW = 3,
	IRQ_HIGH = 4,
	IRQ_FALL_RISE = 5	// dual edge trigger, available for 8195B and 8710C
#endif

} gpio_irq_event;
/** @}*/

/** @}*/
/** @}*/
/** @}*/

typedef void (*gpio_irq_handler)(uint32_t id, gpio_irq_event event);

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
