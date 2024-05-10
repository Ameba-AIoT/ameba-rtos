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

#include "pal_wd_stm.h"
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  \
	WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

/**
    @brief           - Initializes an hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
                     - time_us         : Time in micro seconds
                     - timer_tick_us   : Number of ticks per micro second
    @return          - SUCCESS/FAILURE
**/
int pal_wd_stm_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us)
{
   volatile uint32_t ti = time_us;
   volatile uint32_t titi = timer_tick_us;
   ti;
   titi;
   /*  enable watchog clock  */
   #define RCC_APB1ENR1_WWDGEN_Pos              (11U)
   #define RCC_APB1ENR1_WWDGEN_Msk            (0x1UL << RCC_APB1ENR1_WWDGEN_Pos)/*!< 0x00000800 */
   #define RCC_APB1ENR1_WWDGEN                  RCC_APB1ENR1_WWDGEN_Msk
   /*   RCC_APB1ENR1 */
   volatile uint32_t *reg = (uint32_t *)0x40021058;
   *reg |= RCC_APB1ENR1_WWDGEN;
   *reg |= RCC_APB1ENR1_WWDGEN;
   /*  prescaler  */
   MODIFY_REG(((WWDG_TypeDef *)base_addr)->CFR, WWDG_CFR_WDGTB, LL_WWDG_PRESCALER_128);
   /* Counter 0..0x7F (7 bit counter value)*/
   MODIFY_REG(((WWDG_TypeDef *)base_addr)->CR, WWDG_CR_T, 0x7F);
   /* Window 0x00..0x7F*/
   MODIFY_REG(((WWDG_TypeDef *)base_addr)->CFR, WWDG_CFR_W, 0x7F);

   return 0;
}

/**
    @brief           - Enables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_stm_enable(addr_t base_addr)
{
#if 0
    /* Enable counter by enabling intr and reset */
    ((WWDG_TypeDef *)base_addr)->CR |= WWDG_CR_WDGA;
    /*  reload the counter */
    ((WWDG_TypeDef *)base_addr)->CR = 0x7f;
#else
    volatile addr_t ba = base_addr;
    ba;
#endif
    return 0;
}

/**
    @brief           - Disables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_stm_disable(addr_t base_addr)
{
/*  detect failure */
    ((WWDG_TypeDef *)base_addr)->CR = 0x7f;
    return 0;
}

/**
    @brief           - Checks whether hardware watchdog timer is enabled
    @param           - base_addr       : Base address of the watchdog module
    @return          - Enabled : 1, Disabled : 0
**/
int pal_wd_stm_is_enabled(addr_t base_addr)
{
    return ((((WWDG_TypeDef *)base_addr)->CR  & WWDG_CR_WDGA) ?  1 : 0);

}

