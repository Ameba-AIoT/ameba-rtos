/* mbed Microcontroller Library
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
 */
#include "wait_api.h"
#include "us_ticker_api.h"

//extern void DelayUs(u32 us);
extern void DelayUs(unsigned int us);

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_DELAY
 *  @brief      MBED_DELAY driver modules.
 *  @{
 */


/** @defgroup MBED_DELAY_Exported_Functions MBED_DELAY Exported Functions
  * @{
  */

/**
  * @brief	Delay x sec.
  * @param	us: delay time, unit is sec
  */
void wait(float s)
{
	DelayUs((int)(s * 1000000.0f));
}

/**
  * @brief  Delay x ms.
  * @param  ms: delay time, unit is ms
  */
void wait_ms(int ms)
{
	DelayUs(ms * 1000);
}

/**
  * @brief	Delay x us.
  * @param	us: delay time, unit is us
  */
void wait_us(int us)
{
	DelayUs(us);
}

/** @} */
/** @} */
/** @} */
