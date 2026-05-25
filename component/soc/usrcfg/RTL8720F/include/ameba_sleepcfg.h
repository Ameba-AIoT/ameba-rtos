/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SLEEPCFG_H_
#define _AMEBA_SLEEPCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* wakeup attribute can be set to WAKEUP_NULL/WAKEUP_NP/WAKEUP_AP */
extern const struct WakeEvent_TypeDef sleep_wevent_config[];

extern struct PSCFG_TypeDef ps_config;

extern UARTCFG_TypeDef uart_config[4];

#ifdef __cplusplus
}
#endif

#endif