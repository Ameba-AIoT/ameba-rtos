/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_peripherals_def.h"
#include "array.h"
#include "cmsis.h"

/* Allowed named MMIO of this platform */
const uintptr_t partition_named_mmio_list[] = {
    (uintptr_t)TFM_PERIPHERAL_TIMER0,
    (uintptr_t)TFM_PERIPHERAL_STD_UART,
    (uintptr_t)TFM_PERIPHERAL_DMA0_CH0,
    (uintptr_t)TFM_PERIPHERAL_DMA0_CH1
};

void get_partition_named_mmio_list(const uintptr_t** list, size_t* length) {
    *list = partition_named_mmio_list;
    *length = ARRAY_SIZE(partition_named_mmio_list);
}
