/** mbed Microcontroller Library
  ******************************************************************************
  * @file    sys_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides following mbed system API:
  *				-JTAG OFF
  *				-System Reset
  *				-LOGUART ON/OFF
  *				-OTA image switch
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
#ifndef MBED_SYS_API_H
#define MBED_SYS_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

void sys_jtag_off(void);
void sys_reset(void);

/*
  * @brief  Get currently selected boot device.
  * @retval boot device
  * @note
  *  BootFromNORFlash            = 0,
  *  BootFromNANDFlash           = 1,
  *  BootFromUART                = 2
  */
//uint8_t sys_get_boot_sel(void);

void sys_adc_calibration(u8 write, u16 *a, u16 *b, u16 *c);
void sys_log_uart_on(void);
void sys_log_uart_off(void);
void sys_clear_ota_signature(void);
void sys_recover_ota_signature(void);

#ifdef __cplusplus
}
#endif

#endif
