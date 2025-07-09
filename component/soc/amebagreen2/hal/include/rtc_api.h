/** mbed Microcontroller Library
  ******************************************************************************
  * @file    rtc_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides mbed RTC API
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
#ifndef MBED_RTC_API_H
#define MBED_RTC_API_H

#include "device.h"

#include <time.h>
#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_RTC
 *  @brief      MBED_RTC driver modules.
 *  @{
 */

/** @addtogroup MBED_RTC_Exported_Types MBED_RTC Exported Types
  * @{
  */

/** @defgroup MBED_RTC_Structure_Type Structure Type
  * @{
  */

/**
  * @brief struct alarm_s
  */
typedef struct alarm_s {
	uint32_t yday;//which day of the year
	uint32_t hour;
	uint32_t min;
	uint32_t sec;
} alarm_t;

/**
  * @brief typedef function pointer to point interrupt handler
  */
typedef void (*alarm_irq_handler)(void);
/** @}*/
/** @}*/
/** @}*/
/** @}*/

void rtc_init(void);
void rtc_free(void);
int rtc_isenabled(void);
time_t rtc_read(void);
void rtc_write(time_t t);
u32 rtc_set_alarm(alarm_t *alrm, alarm_irq_handler alarmHandler);
void rtc_disable_alarm(void);

#ifdef __cplusplus
}
#endif

#endif
