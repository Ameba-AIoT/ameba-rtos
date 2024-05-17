/** mbed Microcontroller Library
  ******************************************************************************
  * @file    efuse_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides mbed EFUSE API.
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

#ifndef MBED_EXT_EFUSE_API_EXT_H
#define MBED_EXT_EFUSE_API_EXT_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

int efuse_get_remaining_length(void);
void efuse_mtp_read(uint8_t *data);
int efuse_mtp_write(uint32_t offset, uint32_t len, uint8_t *data);
int efuse_otp_read(uint32_t offset, uint32_t len, uint8_t *buf);
int efuse_otp_write(uint32_t offset, uint32_t len, uint8_t *buf);
int efuse_otp_chk(u8 len, u8 *buf);

/*
  * @brief  Disable jtag
  * @retval 0: Success
  */
//int efuse_disable_jtag(void);


#ifdef __cplusplus
}
#endif

#endif // MBED_EXT_EFUSE_API_EXT_H
