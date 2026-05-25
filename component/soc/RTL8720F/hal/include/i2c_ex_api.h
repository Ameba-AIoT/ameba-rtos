/** mbed Microcontroller Library
  ******************************************************************************
  * @file    i2c_ex_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides mbed API for I2C.
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
  * http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an AS IS BASIS, WITHOUT
  * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
#ifndef I2C_EX_API_H
#define I2C_EX_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_I2C
 *  @brief      MBED_I2C driver modules.
 *  @{
 */

/** @addtogroup MBED_I2C_Exported_Types MBED_I2C Exported Types
  * @{
  */

/** @defgroup MBED_I2C_Enumeration_Type Enumeration Type
  * @{
  */

/**
  * @brief enum I2CCallback
  */
typedef enum {
	I2C_TX_COMPLETE     = 0,
	I2C_RX_COMPLETE     = 1,
	I2C_RD_REQ_COMMAND  = 2,
	I2C_ERR_OCCURRED    = 3,
} I2CCallback;
/** @}*/

/** @}*/
/** @}*/
/** @}*/


int i2c_enable_control(i2c_t *obj, int enable);
void i2c_restart_enable(i2c_t *obj);
void i2c_restart_disable(i2c_t *obj);

#ifdef __cplusplus
}
#endif

#endif

