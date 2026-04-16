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

void Systick_Cmd(u32 enable);

void irq_enable_restore(u32 PrevStatus);
u32 irq_disable_save(void);

#ifdef __cplusplus
}
#endif

#endif /* _SYSTEM_AMEBA_H */
