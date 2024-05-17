/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "stdint.h"

#include "Driver_Flash.h"
#include "flash_layout.h"

#include "io_driver.h"
#include "io_flash.h"
#include "io_storage.h"

#include "platform.h"

#define PLAT_LOG_MODULE_NAME    "platform"
#include "platform_log.h"

typedef struct {
    uintptr_t dev_handle;
    uintptr_t image_spec;
} platform_image_source_t;

extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

static io_dev_connector_t *flash_dev_con;
static uint8_t local_block_flash[FLASH_SECTOR_SIZE];
static io_flash_dev_spec_t flash_dev_spec = {
    .buffer = local_block_flash,
    .bufferlen = FLASH_SECTOR_SIZE,
    .base_addr = FLASH_BASE_ADDRESS,
    .flash_driver = &FLASH_DEV_NAME,
};
static io_block_spec_t flash_spec = {
    .offset = FLASH_BASE_ADDRESS,
    .length = FLASH_TOTAL_SIZE
};

static platform_image_source_t platform_image_source[] = {
    [PLATFORM_GPT_IMAGE] = {
        .dev_handle = NULL,
        .image_spec = &flash_spec,
    }
};

/* Initialize io storage of the platform */
int32_t plat_io_storage_init(void)
{
    int rc = -1;
    uintptr_t flash_dev_handle = NULL;
    uintptr_t flash_handle = NULL;

    rc = register_io_dev_flash((const io_dev_connector_t **) &flash_dev_con);
    if (rc != 0) {
        ERROR("Failed to register io flash rc: %d", rc);
        return rc;
    }

    rc = io_dev_open(flash_dev_con, (const uintptr_t)&flash_dev_spec, &flash_dev_handle);
    if (rc != 0) {
        ERROR("Failed to open io flash dev rc: %d", rc);
        return rc;
    }

    VERBOSE("Flash_dev_handle = %p",flash_dev_handle);

    rc = io_open(flash_dev_handle, (const uintptr_t)&flash_spec, &flash_handle);
    if (rc != 0) {
        ERROR("Failed to open io flash rc: %d", rc);
        return rc;
    }

    VERBOSE("Flash_handle = %p",flash_handle);

    rc = io_close(flash_handle);
    if (rc != 0) {
        ERROR("Failed to close io flash rc: %d", rc);
        return rc;
    }
    /* Update the platform image source that uses the flash with dev handles */
    platform_image_source[PLATFORM_GPT_IMAGE].dev_handle = flash_dev_handle;

    return rc;
}

/* Return an IO device handle and specification which can be used to access
 * an image. This has to be implemented for the GPT parser. */
int32_t plat_get_image_source(unsigned int image_id, uintptr_t *dev_handle,
                              uintptr_t *image_spec) {
    if (image_id >= PLATFORM_IMAGE_COUNT) {
        return -1;
    }
    *dev_handle = platform_image_source[image_id].dev_handle;
    *image_spec = platform_image_source[image_id].image_spec;
    return 0;
}
