/** mbed Microcontroller Library
  ******************************************************************************
  * @file    timer_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides following mbed timer API
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
#ifndef MBED_EXT_TIMER_API_EXT_H
#define MBED_EXT_TIMER_API_EXT_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_TIMER
 *  @brief      MBED_TIMER driver modules.
 *  @{
 */

/** @addtogroup MBED_TIMER_Exported_Types MBED_TIMER Exported Types
  * @{
  */

/** @defgroup MBED_TIMER_Enumeration_Type Enumeration Type
  * @{
  */

/**
  * @brief enum TIMER_ID
  */
typedef enum {
	TIMER0 = 0,
	TIMER1 = 1,
	TIMER2 = 2,
	TIMER3 = 3,
	TIMER4 = 4,
	TIMER5 = 5,
	TIMER6 = 6,
	TIMER7 = 7,
	TIMER8 = 8,
	TIMER9 = 9,
	TIMER10 = 10,
	TIMER11 = 11,
	TIMER12 = 12,
	TIMER13 = 13,
	TIMER14 = 14,

	GTIMER_MAX = 15
} TIMER_ID;
/** @} */

/** @defgroup MBED_TIMER_Structure_Type Structure Type
  * @{
  */
/**
  * @brief typedef struct gtimer_s to gtimer_t
  */
typedef struct gtimer_s gtimer_t;
/** @} */
/** @} */
/** @} */
/** @} */

/**
  * @brief typedef function pointer to point interrupt handler
  */
typedef void (*gtimer_irq_handler)(uint32_t id);


void gtimer_init(gtimer_t *obj, uint32_t tid);
void gtimer_deinit(gtimer_t *obj);
uint32_t gtimer_read_tick(gtimer_t *obj);
uint64_t gtimer_read_us(gtimer_t *obj);
void gtimer_reload(gtimer_t *obj, uint32_t duration_us);
void gtimer_start(gtimer_t *obj);
void gtimer_start_one_shout(gtimer_t *obj, uint32_t duration_us, void *handler, uint32_t hid);
void gtimer_start_periodical(gtimer_t *obj, uint32_t duration_us, void *handler, uint32_t hid);
void gtimer_stop(gtimer_t *obj);

#ifdef __cplusplus
}
#endif

#endif /* MBED_TIMER_API_H */