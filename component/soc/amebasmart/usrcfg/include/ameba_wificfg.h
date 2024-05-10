/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef _AMEBA_WIFICFG_H_
#define _AMEBA_WIFICFG_H_

#ifdef __cplusplus
extern "C" {
#endif

extern struct wifi_user_conf wifi_user_config __attribute__((aligned(64)));

//================functions=================
_WEAK void wifi_set_user_config(void);

#ifdef __cplusplus
}
#endif

#endif
