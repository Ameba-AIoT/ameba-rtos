/** mbed Microcontroller Library
 ******************************************************************************
 * @file    analogin_api.h
 * @author
 * @version V1.0.0
 * @brief   This file provides following mbed Analog_in API
 ******************************************************************************
 * @attention
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
#ifndef MBED_ANALOGIN_API_H
#define MBED_ANALOGIN_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_ADC
 *  @brief MBED_ADC driver modules.
 *  @{
 */


/* reserved region for storage enum and structure, used by Doxygen */


/** @} */
/** @} */

typedef struct analogin_s analogin_t;

void analogin_init(analogin_t *obj, PinName pin);
void analogin_deinit(analogin_t *obj);
float analogin_read(analogin_t *obj);
uint16_t analogin_read_u16(analogin_t *obj);
uint32_t analogin_voltage_norm(uint32_t adc_data);
uint32_t analogin_voltage_vbat(uint32_t adc_data);

#ifdef __cplusplus
}
#endif

#endif//MBED_ANALOGIN_API_H
