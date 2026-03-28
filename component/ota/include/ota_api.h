/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _OTA_API_H_
#define _OTA_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ota_internal.h"


/* ============ Exported functions ======================== */

int ota_init(ota_context_t *ctx, char *host, int port, char *resource, u8 type);
void ota_deinit(ota_context_t *ctx);
int ota_start(ota_context_t *ctx);
int ota_register_progress_cb(ota_context_t *ctx, ota_progress_cb_t cb);
int ota_register_user_read_func(ota_context_t *ctx, ota_user_read_func_t func);
int ota_register_user_open_func(ota_context_t *ctx, ota_user_open_func_t func);

#ifdef __cplusplus
}
#endif

#endif //_OTA_API_H_
