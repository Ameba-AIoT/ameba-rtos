/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __BOOT_MEASUREMENT_H__
#define __BOOT_MEASUREMENT_H__

enum boot_measurement_slot_t {
    BOOT_MEASUREMENT_SLOT_BL1_2 = 0,
    BOOT_MEASUREMENT_SLOT_BL2,
    BOOT_MEASUREMENT_SLOT_RT_0,
    BOOT_MEASUREMENT_SLOT_RT_1,
    BOOT_MEASUREMENT_SLOT_RT_2,
    BOOT_MEASUREMENT_SLOT_MAX = 32,
    BOOT_MEASUREMENT_SLOT_MAX_THEORETICAL = 63  /* Slot index is stored in
                                                 * 6 bits in the shared
                                                 * memory area.
                                                 */
};

#endif /* __BOOT_MEASUREMENT_H__ */
