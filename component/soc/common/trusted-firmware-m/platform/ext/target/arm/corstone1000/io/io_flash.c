/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "io_flash.h"

#include <assert.h>
#include <errno.h>

#include "Driver_Flash.h"
#include "io_block.h"
#include "io_defs.h"
#include "io_driver.h"
#include "io_storage.h"

#if MAX_IO_FLASH_DEVICES > MAX_IO_BLOCK_DEVICES
#error \
    "FLASH devices are BLOCK devices .. MAX_IO_FLASH_DEVICES should be less or equal to MAX_IO_BLOCK_DEVICES"
#endif

/* Private Prototypes */

static int flash_dev_open(const uintptr_t dev_spec, io_dev_info_t **dev_info);
static size_t flash_read(int lba, uintptr_t buf, size_t size, size_t flash_id);
static size_t flash_write(int lba, const uintptr_t buf, size_t size,
                          size_t flash_id);
static size_t flash0_read(int lba, uintptr_t buf, size_t size);
static size_t flash0_write(int lba, uintptr_t buf, size_t size);
static size_t flash1_read(int lba, uintptr_t buf, size_t size);
static size_t flash1_write(int lba, uintptr_t buf, size_t size);

/** Private Data **/

/* Flash device data */
static const io_dev_connector_t flash_dev_connector = {.dev_open =
                                                           flash_dev_open};
static size_t flash_dev_count = 0;
static io_flash_dev_spec_t *flash_dev_specs[MAX_IO_FLASH_DEVICES];

/* Block device data */
static io_dev_connector_t block_dev_connectors[MAX_IO_FLASH_DEVICES];
static io_block_dev_spec_t block_dev_spec[MAX_IO_FLASH_DEVICES];

/* Flash devices read/write function pointers */
static io_block_ops_t flashs_ops[MAX_IO_FLASH_DEVICES] = {
    [0] = {.read = flash0_read, .write = flash0_write},
    [1] = {.read = flash1_read, .write = flash1_write},
};

/* Flash ops functions */
static size_t flash_read(int lba, uintptr_t buf, size_t size, size_t flash_id) {
    ARM_DRIVER_FLASH *flash_driver =
        ((ARM_DRIVER_FLASH *)flash_dev_specs[flash_id]->flash_driver);
    ARM_FLASH_INFO *info = flash_driver->GetInfo();
    uint32_t addr = info->sector_size * lba;
    uint32_t offset = addr - flash_dev_specs[flash_id]->base_addr;
    size_t rem = info->sector_count * info->sector_size - offset;
    size_t cnt = size < rem ? size : rem;

    return flash_driver->ReadData(offset, buf, cnt);
}

static size_t flash_write(int lba, const uintptr_t buf, size_t size,
                          size_t flash_id) {
    ARM_DRIVER_FLASH *flash_driver =
        ((ARM_DRIVER_FLASH *)flash_dev_specs[flash_id]->flash_driver);
    ARM_FLASH_INFO *info = flash_driver->GetInfo();
    int32_t rc = 0;
    uint32_t addr = info->sector_size * lba;
    uint32_t offset = addr - flash_dev_specs[flash_id]->base_addr;
    size_t rem = info->sector_count * info->sector_size - offset;
    size_t cnt = size < rem ? size : rem;

    flash_driver->EraseSector(offset);
    rc = flash_driver->ProgramData(offset, buf, cnt);
    return rc;
}

/* Flash ops functions wrapper for each device */

static size_t flash0_read(int lba, uintptr_t buf, size_t size) {
    return flash_read(lba, buf, size, 0);
}

static size_t flash0_write(int lba, uintptr_t buf, size_t size) {
    return flash_write(lba, buf, size, 0);
}

static size_t flash1_read(int lba, uintptr_t buf, size_t size) {
    return flash_read(lba, buf, size, 1);
}

static size_t flash1_write(int lba, uintptr_t buf, size_t size) {
    return flash_write(lba, buf, size, 1);
}

/**
 *  Helper function to find the index of stored flash_dev_specs or
 *  return a free slot in case of a new dev_spec
 */
static int find_flash_dev_specs(const uintptr_t dev_spec) {
    /* Search in the saved ones */
    for (int i = 0; i < flash_dev_count; ++i) {
        if (flash_dev_specs[i] != NULL &&
            flash_dev_specs[i]->flash_driver ==
                ((io_flash_dev_spec_t *)dev_spec)->flash_driver) {
            return i;
        }
    }
    /* Find the first empty flash_dev_specs to be used */
    for (int i = 0; i < flash_dev_count; ++i) {
        if (flash_dev_specs[i] == NULL) {
            return i;
        }
    }
    return -1;
}

/**
 * This function should be called
 */
static int flash_dev_open(const uintptr_t dev_spec, io_dev_info_t **dev_info) {
    ARM_DRIVER_FLASH *flash_driver;
    assert(dev_info != NULL);
    assert(dev_spec != NULL);

    size_t index = find_flash_dev_specs(dev_spec);

    /* Check if Flash ops functions are defined for this flash */
    assert(flashs_ops[index].read && flashs_ops[index].write);

    flash_dev_specs[index] = dev_spec;
    flash_driver = flash_dev_specs[index]->flash_driver;

    block_dev_spec[index].block_size = flash_driver->GetInfo()->sector_size;
    block_dev_spec[index].buffer.offset = flash_dev_specs[index]->buffer;
    block_dev_spec[index].buffer.length = flash_dev_specs[index]->bufferlen;
    block_dev_spec[index].ops = flashs_ops[index];

    flash_driver->Initialize(NULL);

    block_dev_connectors[index].dev_open(&block_dev_spec[index], dev_info);

    return 0;
}

/* Exported functions */

/**
 * Register the flash device.
 * Internally it register a block device.
 */
int register_io_dev_flash(const io_dev_connector_t **dev_con) {
    int result;

    if (flash_dev_count >= MAX_IO_FLASH_DEVICES) {
        return -ENOENT;
    }
    assert(dev_con != NULL);

    result = register_io_dev_block(dev_con);
    if (result == 0) {
        /* Store the block dev connector */
        block_dev_connectors[flash_dev_count++] = **dev_con;
        /* Override dev_con with the flash dev connector */
        *dev_con = &flash_dev_connector;
    }
    return result;
}
