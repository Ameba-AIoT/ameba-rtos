/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SLEEPCFG_H_
#define _AMEBA_SLEEPCFG_H_

/* Wakeup entry can be set to WAKEUP_NULL/WAKEUP_KM4/WAKEUP_KM0 */
extern WakeEvent_TypeDef sleep_wevent_config[];
/* can be used by sleep mode & deep sleep mode */
/* config can be set to DISABLE_WAKEPIN/HIGH_LEVEL_WAKEUP/LOW_LEVEL_WAKEUP */
extern WAKEPIN_TypeDef sleep_wakepin_config[];

extern PSCFG_TypeDef ps_config;

extern UARTCFG_TypeDef uart_config[3];

#endif