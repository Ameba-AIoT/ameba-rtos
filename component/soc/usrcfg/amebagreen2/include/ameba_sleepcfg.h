/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef _AMEBA_SLEEPCFG_H_
#define _AMEBA_SLEEPCFG_H_

/* wakeup attribute can be set to WAKEUP_NULL/WAKEUP_NP/WAKEUP_AP */
extern struct WakeEvent_TypeDef sleep_wevent_config[];

/* config attribute can be set to LOW_LEVEL_WAKEUP/HIGH_LEVEL_WAKEUP/DISABLE_WAKEPIN */
extern struct WAKEPIN_TypeDef sleep_wakepin_config[];

extern struct PSCFG_TypeDef ps_config;

extern UARTCFG_TypeDef uart_config[4];

#endif