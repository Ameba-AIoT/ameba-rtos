/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#ifndef _VAL_DEBUG_H_
#define _VAL_DEBUG_H_

#include "val_common.h"
#include "val_target.h"
#include "pal_interfaces.h"

#define DBG_TIMEOUT             0x100000

#define DBG_INIT_SIGNATURE      0xDEED007
#define DBG_NEXT_SIGNATURE      0xDEED008

#define DBG_FLAG_TXFULL         0x1
#define DBG_FLAG_RXFULL         0x2
#define DBG_FLAG_READY          0x4
#define DBG_FLAG_RESERVED_BITS  0xFFFFFFFC

typedef enum {
    DBG_READ  = 1,
    DBG_WRITE = 2,
    DBG_INIT  = 3,
    DBG_MAX   = 0xFFFF
} dbg_access_t;

typedef enum {
    DPM_OPEN_STATE   = 0x1,
    DPM_CLOSED_STATE = 0x2,
    DPM_LOCKED_STATE = 0x3,
    DPM_MAX          = 0xFFFF
} dpm_status_t;

#define DPM_EN_STATE          0x01
#define DPM_EN_IMPLEMENTED    0x02
#define DPM_LOCK_STATE        0x04
#define DPM_LOCK_IMPLEMENTED  0x08

typedef enum {
    SEQ_OPEN_STATE_READ    = 0x10,
    SEQ_OPEN_STATE_WRITE   = 0x20,
    SEQ_CLOSED_STATE_READ  = 0x30,
    SEQ_CLOSED_STATE_WRITE = 0x40,
    SEQ_LOCKED_STATE_READ  = 0x50,
    SEQ_LOCKED_STATE_WRITE = 0x60,
    SEQ_NEXT_TEST          = 0xE0,
    SEQ_TEST_END           = 0xF0,
    SEQ_MAX                = 0xFFFF
} dbg_seq_status_t;

tbsa_status_t val_debug_set_status(dbg_access_t dbg_access, dbg_seq_status_t dbg_status);
tbsa_status_t val_debug_get_status(dbg_access_t dbg_access);
tbsa_status_t val_dpm_set_state(dpm_desc_t *dpm_desc, dpm_status_t dpm_status);
tbsa_status_t val_dpm_get_state(uint32_t index, uint32_t *dpm_status);
tbsa_status_t val_dpm_set_access_ns_only(uint32_t index, bool_t access_ns);

#endif /* _VAL_DEBUG_H_ */
