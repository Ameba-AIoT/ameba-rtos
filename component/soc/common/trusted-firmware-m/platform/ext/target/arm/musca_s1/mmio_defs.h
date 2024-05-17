/*
 * Copyright (c) 2021-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __MMIO_DEFS_H__
#define __MMIO_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "tfm_peripherals_def.h"

/* Boundary handle binding macros. */
#define HANDLE_ATTR_PRIV_POS            1U
#define HANDLE_ATTR_PRIV_MASK           (0x1UL << HANDLE_ATTR_PRIV_POS)
#define HANDLE_ATTR_NS_POS              0U
#define HANDLE_ATTR_NS_MASK             (0x1UL << HANDLE_ATTR_NS_POS)

/* Allowed named MMIO of this platform */
const uintptr_t partition_named_mmio_list[] = {
    (uintptr_t)TFM_PERIPHERAL_TIMER0,
    (uintptr_t)TFM_PERIPHERAL_STD_UART,
#ifdef PSA_API_TEST_IPC
    (uintptr_t)FF_TEST_UART_REGION,
    (uintptr_t)FF_TEST_WATCHDOG_REGION,
    (uintptr_t)FF_TEST_NVMEM_REGION,
    (uintptr_t)FF_TEST_SERVER_PARTITION_MMIO,
    (uintptr_t)FF_TEST_DRIVER_PARTITION_MMIO,
#endif
};

/*
 * Platform MUSCA_S1 only has named MMIO.
 * If the platform has numbered MMIO, define them in another list.
 */

#ifdef __cplusplus
}
#endif

#endif /* __MMIO_DEFS_H__ */
