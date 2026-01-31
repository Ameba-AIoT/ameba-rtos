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

/* Wakeup entry can be set to WAKEUP_NULL/WAKEUP_KM4/WAKEUP_KM0 */
extern struct WakeEvent_TypeDef sleep_wevent_config[];

extern struct PSCFG_TypeDef ps_config;

extern UARTCFG_TypeDef uart_config[3];

#ifdef __cplusplus
}
#endif

#endif