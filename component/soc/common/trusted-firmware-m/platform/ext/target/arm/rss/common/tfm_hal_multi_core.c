/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_hal_multi_core.h"

void tfm_hal_boot_ns_cpu(uintptr_t start_addr)
{
    /* Nothing to do, other CPUs are booted by BL2 */
    (void)start_addr;
    return;
}

void tfm_hal_wait_for_ns_cpu_ready(void)
{
    /* Nothing to do, all necessary synchronization done by BL2 */
    return;
}
