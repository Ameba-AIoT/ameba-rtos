/** mbed Microcontroller Library
  ******************************************************************************
  * @file    i2c_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides following mbed I2C API
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
#ifndef MBED_I2C_API_H
#define MBED_I2C_API_H

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
  * @brief enum I2C_ERR_REASON
  */
enum {
	I2C_ERROR_NO_SLAVE = -1,
	I2C_ERROR_BUS_BUSY = -2
};
/** @}*/

/** @}*/
/** @}*/
/** @}*/

typedef struct i2c_s i2c_t;
void i2c_init(i2c_t *obj, PinName sda, PinName scl);
void i2c_frequency(i2c_t *obj, int hz);
int i2c_start(i2c_t *obj);
int i2c_stop(i2c_t *obj);
void i2c_reset(i2c_t *obj);
int i2c_byte_read(i2c_t *obj, int last);
int i2c_byte_write(i2c_t *obj, int data);
void i2c_slave_mode(i2c_t *obj, int enable_slave);
int i2c_slave_receive(i2c_t *obj);
void i2c_slave_address(i2c_t *obj, int idx, uint32_t address, uint32_t mask);
int i2c_read(i2c_t *obj, int address, char *data, int length, int stop);
int i2c_write(i2c_t *obj, int address, const char *data, int length, int stop);
int i2c_slave_read(i2c_t *obj, char *data, int length);
int i2c_slave_write(i2c_t *obj, const char *data, int length);
int i2c_slave_set_for_rd_req(i2c_t *obj, int set);
int i2c_slave_set_for_data_nak(i2c_t *obj, int set_nak);
int i2c_repeatread(i2c_t *obj, int address, u8 *pWriteBuf, int Writelen, u8 *pReadBuf, int Readlen) ;
void i2c_send_restart(I2C_TypeDef *I2Cx, u8 *pBuf, u8 len, u8 restart);


#ifdef __cplusplus
}
#endif

#endif/* MBED_I2C_API_H */
