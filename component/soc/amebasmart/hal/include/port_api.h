/** mbed Microcontroller Library
  ******************************************************************************
  * @file    port_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides following mbed GPIO PORT API
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
#ifndef MBED_PORTMAP_H
#define MBED_PORTMAP_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_GPIO
 *  @brief MBED_GPIO driver modules.
 *  @{
 */

/** @addtogroup MBED_GPIO_Exported_Types MBED_GPIO Exported Types
  * @{
  */

/** @defgroup MBED_GPIO_Structure_Type Structure Type
  * @{
  */

typedef struct port_s port_t;

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


void port_init(port_t *obj, PortName port, int mask, PinDirection dir);
PinName port_pin(PortName port, int pin_n);
void port_mode(port_t *obj, PinMode mode);
void port_dir(port_t *obj, PinDirection dir);
void port_write(port_t *obj, int value);
int port_read(port_t *obj);


#ifdef __cplusplus
}
#endif

#endif
