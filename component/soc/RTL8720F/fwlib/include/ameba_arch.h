/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSTEM_AMEBA_H
#define _SYSTEM_AMEBA_H

#ifdef __cplusplus
extern "C" {
#endif

extern u32 SystemCoreClock;     /*!< System Clock Frequency (Core Clock)  */

void SystemInit(void);
void SystemCoreClockUpdate(void);
void Systick_Cmd(u32 enable);
u32 Systick_State(void);

/**
  * @brief  Set CPU clock Source.
  * @param  Source: This parameter can be one of the following values:
  * For LP:
  *		 @arg CLK_NP_PLL
  *		 @arg CLK_NP_XTAL
  *		 @arg CLK_NP_ANA_4M
  *For HP
  *		 @arg CLK_AP_PLL
  *		 @arg CLK_AP_XTAL
  */
void CPU_ClkSrcSet(u32 Source);
u32 CPU_InInterrupt(void);

u32 irq_disable_save(void);
void irq_enable_restore(u32 PrevStatus);

#ifdef __cplusplus
}
#endif

#endif /* _SYSTEM_AMEBA_H */
