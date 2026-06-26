/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_TRUSTZONECFG_H_
#define _AMEBA_TRUSTZONECFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* this table is configured by usrcfg */
#ifdef CONFIG_TRUSTZONE
extern const SAU_CFG_TypeDef sau_config[];
#endif
extern const TZ_CFG_TypeDef mpc1_config[];
extern const TZ_CFG_TypeDef mpc2_config[];

#ifdef __cplusplus
}
#endif

#endif