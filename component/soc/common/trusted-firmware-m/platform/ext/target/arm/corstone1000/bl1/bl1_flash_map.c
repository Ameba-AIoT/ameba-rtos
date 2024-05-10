/*
 * Copyright (c) 2019-2021, 2023 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include "flash_map/flash_map.h"
#include "flash_map_backend/flash_map_backend.h"
#include "bootutil/bootutil_log.h"
#include "sysflash/sysflash.h"
#include "Driver_Flash.h"

/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

struct flash_area flash_map[] = {
    {
        .fa_id = FLASH_AREA_8_ID,
        .fa_device_id = FLASH_DEVICE_ID,
        .fa_driver = &FLASH_DEV_NAME,
        .fa_off = FLASH_INVALID_OFFSET,
        .fa_size = FLASH_AREA_8_SIZE,
    },
    /* Secondary slot is not supported */
    {
        .fa_id = FLASH_INVALID_ID,
        .fa_device_id = FLASH_DEVICE_ID,
        .fa_driver = &FLASH_DEV_NAME,
        .fa_off = FLASH_INVALID_OFFSET,
        .fa_size = FLASH_INVALID_SIZE,
    },
};

const int flash_map_entry_num = ARRAY_SIZE(flash_map);

void set_flash_area_image_offset(uint32_t offset)
{
    flash_map[0].fa_off = offset;
}
