/** mbed Microcontroller Library
  ******************************************************************************
  * @file    gpio_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides following mbed GPIO API
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
#ifndef MBED_GPIO_API_H
#define MBED_GPIO_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

void gpio_init(gpio_t *obj, PinName pin);
uint32_t gpio_set(PinName pin);
void gpio_mode(gpio_t *obj, PinMode mode);
void gpio_dir(gpio_t *obj, PinDirection direction);
void gpio_write(gpio_t *obj, int value);
int  gpio_read(gpio_t *obj);

void gpio_direct_write(gpio_t *obj, BOOL value) ;
void gpio_pull_ctrl(gpio_t *obj, PinMode pull_type);
void gpio_deinit(gpio_t *obj);
void gpio_change_dir(gpio_t *obj, PinDirection direction);

#ifdef __cplusplus
}
#endif

#endif//MBED_GPIO_API_H
