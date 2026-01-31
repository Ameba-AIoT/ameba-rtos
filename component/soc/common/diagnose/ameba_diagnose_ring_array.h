/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_DIAGNOSE_RING_ARRAY_H_
#define _AMEBA_DIAGNOSE_RING_ARRAY_H_

#include "ameba_diagnose_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagRtkDiagRingArrayHandler RtkDiagRingArrayHandler_t;

RtkDiagRingArrayHandler_t *rtk_diag_ring_array_create(u32 capacity, u32 element_size, rtk_diag_ring_array_emplace_t emplace);
int rtk_diag_ring_array_destroy(RtkDiagRingArrayHandler_t *handler);
int rtk_diag_ring_array_clear(RtkDiagRingArrayHandler_t *handler);
void rtk_diag_ring_array_emplace_back(RtkDiagRingArrayHandler_t *handler, const void *origin_data);
u32 rtk_diag_ring_array_size(const RtkDiagRingArrayHandler_t *handler);
void *rtk_diag_ring_array_at(RtkDiagRingArrayHandler_t *handler, u32 index);
const void *rtk_diag_ring_array_view(const RtkDiagRingArrayHandler_t *handler, u32 index);
void *rtk_diag_ring_array_pop(RtkDiagRingArrayHandler_t *handler);

#ifdef __cplusplus
}
#endif

#endif