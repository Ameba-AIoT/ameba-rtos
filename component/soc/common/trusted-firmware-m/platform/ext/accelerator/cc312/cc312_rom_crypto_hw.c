/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "crypto_hw.h"

#include <stdint.h>
#include <stddef.h>
#include "region_defs.h"
#include "cc3xx_drv.h"
#include "device_definition.h"

int crypto_hw_accelerator_init(void)
{
    return cc3xx_init();
}

int crypto_hw_accelerator_finish(void)
{
    return cc3xx_finish();
}

int fih_delay_init(void)
{
    return 0;
}
