/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SOLO_H_
#define _AMEBA_SOLO_H_


#ifdef __cplusplus
extern "C" {
#endif

void SOLO_Plfm1ResetHandler(void);
u32 SOLO_Rxi300TimoutHandler(void *param);

#ifdef __cplusplus
}
#endif

#endif