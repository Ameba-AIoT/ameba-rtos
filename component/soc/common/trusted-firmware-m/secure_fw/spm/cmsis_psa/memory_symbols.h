/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __MEMORY_SYMBOLS_H__
#define __MEMORY_SYMBOLS_H__

#include "region.h"

/*
 * This file contains memory symbols required by TF-M SPM.
 *
 * Symbols can be reserved by linkage, or globally defined in a
 * source file with toolchain-specific flags.
 */


/* ---------- SPM boot stack - the default stack when booting up ---------- */
REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Base);
REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Limit);

#define SPM_BOOT_STACK_TOP            \
                    (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Base)
#define SPM_BOOT_STACK_BOTTOM         \
                    (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Limit)


/* ----------- ROM loader specific symbols ------------------------ */
/* Partition load data region */
REGION_DECLARE(Image$$, TFM_SP_LOAD_LIST, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_SP_LOAD_LIST, $$RO$$Limit);

/* Partition and service runtime pool region */
REGION_DECLARE(Image$$, ER_PART_RT_POOL, $$ZI$$Base);
REGION_DECLARE(Image$$, ER_PART_RT_POOL, $$ZI$$Limit);
REGION_DECLARE(Image$$, ER_SERV_RT_POOL, $$ZI$$Base);
REGION_DECLARE(Image$$, ER_SERV_RT_POOL, $$ZI$$Limit);

#define PART_INFOLIST_START           \
                (uintptr_t)&REGION_NAME(Image$$, TFM_SP_LOAD_LIST, $$RO$$Base)
#define PART_INFOLIST_END             \
                (uintptr_t)&REGION_NAME(Image$$, TFM_SP_LOAD_LIST, $$RO$$Limit)
#define PART_INFORAM_START            \
                (uintptr_t)&REGION_NAME(Image$$, ER_PART_RT_POOL, $$ZI$$Base)
#define PART_INFORAM_END              \
                (uintptr_t)&REGION_NAME(Image$$, ER_PART_RT_POOL, $$ZI$$Limit)
#define SERV_INFORAM_START            \
                (uintptr_t)&REGION_NAME(Image$$, ER_SERV_RT_POOL, $$ZI$$Base)
#define SERV_INFORAM_END              \
                (uintptr_t)&REGION_NAME(Image$$, ER_SERV_RT_POOL, $$ZI$$Limit)

/* ----------- IPC Local Storage specific symbols ------------------------ */
#ifdef CONFIG_TFM_PARTITION_META
REGION_DECLARE(Image$$, TFM_SP_META_PTR, $$ZI$$Base);

#define PART_LOCAL_STORAGE_PTR_POS    \
                ((uintptr_t)&REGION_NAME(Image$$, TFM_SP_META_PTR, $$ZI$$Base))

#endif

#endif /* __MEMORY_SYMBOLS_H__ */
