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

void CPU_ClkSet(u32 Source);
u32 CPU_InInterrupt(void);

u32 irq_disable_save(void);
void irq_enable_restore(u32 PrevStatus);

void BOOT_InitGP(void);

#ifdef __cplusplus
}
#endif

#endif /* _SYSTEM_AMEBA_H */
