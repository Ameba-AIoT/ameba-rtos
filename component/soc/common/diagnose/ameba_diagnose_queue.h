/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_DIAGNOSE_QUEUE_H_
#define _AMEBA_DIAGNOSE_QUEUE_H_

#include "ameba_diagnose_ring_array.h"

#ifdef __cplusplus
extern "C" {
#endif

int rtk_diag_queue_init(u16 heap_capacity, u16 total_capacity);
void rtk_diag_queue_deinit(void);
int rtk_diag_queue_set_total_capacity(u16 total_capacity);
u16 rtk_diag_queue_get_total_capacity(void);
int rtk_diag_queue_enqueue(u32 timestamp, u8 evt_level, u16 evt_type, const u8 *evt_info, u16 evt_len);
RtkDiagEvent_t *rtk_diag_queue_dequeue(void); //NOTE: caller should free returned event when finish
int rtk_diag_queue_clear(void);
int rtk_diag_queue_del_before(u32 timestamp, u16 *count); //including the event with exact timestamp
int rtk_diag_queue_del_after(u32 timestamp, u16 *count);  //not including the event with exact timestamp
const RtkDiagRingArrayHandler_t *rtk_diag_queue_get_del_list(void);
int rtk_diag_queue_clr_del_list(void);
const RtkDiagEvent_t *rtk_diag_queue_find(u32 timestamp, u16 *global_offset, u16 *local_offset, int *result);
const RtkDiagEvent_t *rtk_diag_queue_next_to_prev_find(void);

#ifdef __cplusplus
}
#endif

#endif