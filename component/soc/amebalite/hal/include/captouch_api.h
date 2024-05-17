/** mbed Microcontroller Library
  ******************************************************************************
  * @file    captouch_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides following mbed CAPTOUCH API
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2006-2013 ARM Limited
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  ******************************************************************************
  */
#ifndef MBED_CAPTOUCH_API_H
#define MBED_CAPTOUCH_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_CAPTOUCH
 *  @brief MBED_CAPTOUCH driver modules.
 *  @{
 */


/* reserved region for storage enum and structure, used by Doxygen */


/**
  * @}
  */

/**
  * @}
  */

typedef struct captouch_s captouch_t;
typedef void (*ctc_irq_handler)(u8 ch);

void captouch_init(captouch_t *obj);
void captouch_deinit(captouch_t *obj);
void captouch_ch_enable(captouch_t *obj, u8 Channel);
void captouch_ch_disable(captouch_t *obj, u8 Channel);
void captouch_set_scan_interval(captouch_t *obj, u32 Interval);
u32 captouch_get_ch_baseline(captouch_t *obj, u8 Channel);
u32 captouch_get_ch_data(captouch_t *obj, u8 Channel);

#ifdef __cplusplus
}
#endif

#endif
