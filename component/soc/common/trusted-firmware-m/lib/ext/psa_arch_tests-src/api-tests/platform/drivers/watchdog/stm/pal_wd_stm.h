/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#ifndef _PAL_WD_CMSDK_H_
#define _PAL_WD_CMSDK_H_

#include "pal_common.h"

#define WDOG_TIMER_MAX_VALUE           0xFFFFFFFF
#define __IO volatile
typedef struct {
  __IO uint32_t CR;          /*!< WWDG Control register,       Address offset: 0x00 */
  __IO uint32_t CFR;         /*!< WWDG Configuration register, Address offset: 0x04 */
  __IO uint32_t SR;          /*!< WWDG Status register,        Address offset: 0x08 */
} WWDG_TypeDef;

#define WWDG_CR_WDGA_Pos        (7U)
#define WWDG_CR_WDGA_Msk        (0x1UL << WWDG_CR_WDGA_Pos)                 /*!< 0x00000080 */
#define WWDG_CR_WDGA            WWDG_CR_WDGA_Msk                            /*!<Activation bit*/

/******************************************************************************/
/*                                                                            */
/*                            Window WATCHDOG                                 */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for WWDG_CR register  ********************/
#define WWDG_CR_T_Pos           (0U)
#define WWDG_CR_T_Msk           (0x7FUL << WWDG_CR_T_Pos)      /*!< 0x0000007F */
#define WWDG_CR_T               WWDG_CR_T_Msk   /*!<T[6:0] bits (7-Bit counter (MSB to LSB)) */
#define WWDG_CR_T_0             (0x01UL << WWDG_CR_T_Pos)                      /*!< 0x00000001 */
#define WWDG_CR_T_1             (0x02UL << WWDG_CR_T_Pos)                      /*!< 0x00000002 */
#define WWDG_CR_T_2             (0x04UL << WWDG_CR_T_Pos)                      /*!< 0x00000004 */
#define WWDG_CR_T_3             (0x08UL << WWDG_CR_T_Pos)                      /*!< 0x00000008 */
#define WWDG_CR_T_4             (0x10UL << WWDG_CR_T_Pos)                      /*!< 0x00000010 */
#define WWDG_CR_T_5             (0x20UL << WWDG_CR_T_Pos)                      /*!< 0x00000020 */
#define WWDG_CR_T_6             (0x40UL << WWDG_CR_T_Pos)                      /*!< 0x00000040 */

#define WWDG_CR_WDGA_Pos        (7U)
#define WWDG_CR_WDGA_Msk        (0x1UL << WWDG_CR_WDGA_Pos)                    /*!< 0x00000080 */
#define WWDG_CR_WDGA            WWDG_CR_WDGA_Msk                               /*!<Activation bit */

/*******************  Bit definition for WWDG_CFR register  *******************/
#define WWDG_CFR_W_Pos          (0U)
#define WWDG_CFR_W_Msk          (0x7FUL << WWDG_CFR_W_Pos)                     /*!< 0x0000007F */
#define WWDG_CFR_W              WWDG_CFR_W_Msk      /*!<W[6:0] bits (7-bit window value) */
#define WWDG_CFR_W_0            (0x01UL << WWDG_CFR_W_Pos)                     /*!< 0x00000001 */
#define WWDG_CFR_W_1            (0x02UL << WWDG_CFR_W_Pos)                     /*!< 0x00000002 */
#define WWDG_CFR_W_2            (0x04UL << WWDG_CFR_W_Pos)                     /*!< 0x00000004 */
#define WWDG_CFR_W_3            (0x08UL << WWDG_CFR_W_Pos)                     /*!< 0x00000008 */
#define WWDG_CFR_W_4            (0x10UL << WWDG_CFR_W_Pos)                     /*!< 0x00000010 */
#define WWDG_CFR_W_5            (0x20UL << WWDG_CFR_W_Pos)                     /*!< 0x00000020 */
#define WWDG_CFR_W_6            (0x40UL << WWDG_CFR_W_Pos)                     /*!< 0x00000040 */

#define WWDG_CFR_WDGTB_Pos      (11U)
#define WWDG_CFR_WDGTB_Msk      (0x7UL << WWDG_CFR_WDGTB_Pos)                  /*!< 0x00003800 */
#define WWDG_CFR_WDGTB          WWDG_CFR_WDGTB_Msk      /*!<WDGTB[2:0] bits (Timer Base) */
#define WWDG_CFR_WDGTB_0        (0x1UL << WWDG_CFR_WDGTB_Pos)                  /*!< 0x00000800 */
#define WWDG_CFR_WDGTB_1 (0x2UL << WWDG_CFR_WDGTB_Pos)                  /*!< 0x00001000 */
#define WWDG_CFR_WDGTB_2 (0x4UL << WWDG_CFR_WDGTB_Pos)                  /*!< 0x00002000 */

#define WWDG_CFR_WDGTB_Pos (11U)
#define WWDG_CFR_WDGTB_Msk (0x7UL << WWDG_CFR_WDGTB_Pos)                  /*!< 0x00003800 */
#define WWDG_CFR_WDGTB WWDG_CFR_WDGTB_Msk


/** @defgroup WWDG_LL_EC_PRESCALER  PRESCALER
  * @{
  */
/*!< WWDG counter clock = (PCLK1/4096)/1 */
/*!< WWDG counter clock = (PCLK1/4096)/2 */
/*!< WWDG counter clock = (PCLK1/4096)/4 */
/*!< WWDG counter clock = (PCLK1/4096)/8 */
/*!< WWDG counter clock = (PCLK1/4096)/16 */
/*!< WWDG counter clock = (PCLK1/4096)/32 */
/*!< WWDG counter clock = (PCLK1/4096)/64 */
/*!< WWDG counter clock = (PCLK1/4096)/128 */

#define LL_WWDG_PRESCALER_1      0x00000000u
#define LL_WWDG_PRESCALER_2      WWDG_CFR_WDGTB_0
#define LL_WWDG_PRESCALER_4      WWDG_CFR_WDGTB_1
#define LL_WWDG_PRESCALER_8      (WWDG_CFR_WDGTB_0 | WWDG_CFR_WDGTB_1)
#define LL_WWDG_PRESCALER_16     WWDG_CFR_WDGTB_2
#define LL_WWDG_PRESCALER_32     (WWDG_CFR_WDGTB_2 | WWDG_CFR_WDGTB_0)
#define LL_WWDG_PRESCALER_64     (WWDG_CFR_WDGTB_2 | WWDG_CFR_WDGTB_1)
#define LL_WWDG_PRESCALER_128    (WWDG_CFR_WDGTB_2 | WWDG_CFR_WDGTB_1 | WWDG_CFR_WDGTB_0)



int pal_wd_stm_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);
int pal_wd_stm_enable(addr_t base_addr);
int pal_wd_stm_disable(addr_t base_addr);
int pal_wd_stm_is_enabled(addr_t base_addr);

#endif /* PAL_WD_CMSDK_H */
