/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "s_io_storage_test.h"

#include "Driver_Flash.h"
#include "flash_layout.h"
#include "io_block.h"
#include "io_driver.h"
#include "io_flash.h"
#include "tfm_sp_log.h"

#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(*(array)))

extern ARM_DRIVER_FLASH Driver_FLASH0;
extern ARM_DRIVER_FLASH Driver_TEST_FLASH;
extern uintptr_t flash_base_address;

void s_test_io_storage_multiple_flash_simultaneous(struct test_result_t *ret) {
    /* FLASH0 */
    static io_dev_connector_t* flash0_dev_con;
    static uint8_t local_block_flash0[FLASH_SECTOR_SIZE];
    ARM_FLASH_INFO* flash0_info = Driver_FLASH0.GetInfo();
    size_t flash0_block_size = flash0_info->sector_size;
    io_flash_dev_spec_t flash0_dev_spec = {
        .buffer = local_block_flash0,
        .bufferlen = flash0_block_size,
        .base_addr = FLASH_BASE_ADDRESS,
        .flash_driver = &Driver_FLASH0,
    };
    io_block_spec_t flash0_spec = {
        .offset = FLASH_BASE_ADDRESS,
        .length = flash0_info->sector_count * flash0_info->sector_size};
    uintptr_t flash0_dev_handle = NULL;
    uintptr_t flash0_handle = NULL;

    /* EMU TEST FLASH */
    static io_dev_connector_t* flash_emu_dev_con;
    static uint8_t local_block_flash_emu[TEST_FLASH_SECTOR_SIZE_IN_BYTES]
        __attribute__((aligned(TEST_FLASH_SECTOR_SIZE_IN_BYTES)));
    ARM_FLASH_INFO* flash_emu_info = Driver_TEST_FLASH.GetInfo();
    size_t flash_emu_block_size = flash_emu_info->sector_size;
    io_flash_dev_spec_t flash_emu_dev_spec = {
        .buffer = local_block_flash_emu,
        .bufferlen = flash_emu_block_size,
        .base_addr = flash_base_address,
        .flash_driver = &Driver_TEST_FLASH,
    };
    io_block_spec_t flash_emu_spec = {
        .offset = flash_base_address,
        .length = flash_emu_info->sector_count * flash_emu_info->sector_size};
    uintptr_t flash_emu_dev_handle = NULL;
    uintptr_t flash_emu_handle = NULL;

    /* Common */
    int rc = -1;
    static uint8_t test_data[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA,
                                  0x10, 0x50, 0xA0, 0xD0, 0x51,
                                  0x55, 0x44, 0x33, 0x22, 0x11};
    static uint8_t actual_data[15];
    size_t bytes_written_count = 0;
    size_t bytes_read_count = 0;

    memset(local_block_flash0, -1, sizeof(local_block_flash0));
    memset(local_block_flash_emu, -1, sizeof(local_block_flash_emu));

    /* Register */
    register_io_dev_flash(&flash0_dev_con);
    register_io_dev_flash(&flash_emu_dev_con);

    io_dev_open(flash0_dev_con, &flash0_dev_spec, &flash0_dev_handle);
    io_dev_open(flash_emu_dev_con, &flash_emu_dev_spec, &flash_emu_dev_handle);

    /* Write Data */
    io_open(flash0_dev_handle, &flash0_spec, &flash0_handle);
    io_open(flash_emu_dev_handle, &flash_emu_spec, &flash_emu_handle);

    io_seek(flash0_handle, IO_SEEK_SET,
            BANK_1_PARTITION_OFFSET + flash0_info->sector_size - 7);
    io_seek(flash_emu_handle, IO_SEEK_SET, flash_emu_info->sector_size - 7);

    io_write(flash0_handle, test_data, ARRAY_LENGTH(test_data),
             &bytes_written_count);
    if (bytes_written_count != ARRAY_LENGTH(test_data)) {
        LOG_ERRFMT("io_write failed to write %d bytes for flash0",
                   ARRAY_LENGTH(test_data));
        LOG_ERRFMT("bytes_written_count %d for flash0", bytes_written_count);
        ret->val = TEST_FAILED;
    }
    io_write(flash_emu_handle, test_data, ARRAY_LENGTH(test_data),
             &bytes_written_count);
    if (bytes_written_count != ARRAY_LENGTH(test_data)) {
        LOG_ERRFMT("io_write failed to write %d bytes for flash emu",
                   ARRAY_LENGTH(test_data));
        LOG_ERRFMT("bytes_written_count %d for flash emu", bytes_written_count);
        ret->val = TEST_FAILED;
    }
    io_close(flash0_handle);
    io_close(flash_emu_handle);

    /* Read Data */
    io_open(flash0_dev_handle, &flash0_spec, &flash0_handle);
    io_open(flash_emu_dev_handle, &flash_emu_spec, &flash_emu_handle);

    io_seek(flash0_handle, IO_SEEK_SET,
            BANK_1_PARTITION_OFFSET + flash0_info->sector_size - 7);
    io_seek(flash_emu_handle, IO_SEEK_SET, flash_emu_info->sector_size - 7);

    /* Flash0 */
    io_read(flash0_handle, actual_data, ARRAY_LENGTH(actual_data),
            &bytes_read_count);
    if (bytes_read_count != ARRAY_LENGTH(test_data)) {
        LOG_ERRFMT("io_read failed to read %d bytes for flash0",
                   ARRAY_LENGTH(test_data));
        LOG_ERRFMT("bytes_read_count %d for flash0", bytes_read_count);
        ret->val = TEST_FAILED;
    }
    if (memcmp((uint8_t*)test_data, actual_data, ARRAY_LENGTH(actual_data)) !=
        0) {
        LOG_ERRFMT("Data written != Data read\r\n");
        ret->val = TEST_FAILED;
    }

    memset(actual_data, -1, sizeof(actual_data));

    /* Flash Emu */
    io_read(flash_emu_handle, actual_data, ARRAY_LENGTH(actual_data),
            &bytes_read_count);
    if (bytes_read_count != ARRAY_LENGTH(test_data)) {
        LOG_ERRFMT("io_read failed to read %d bytes for flash emu",
                   ARRAY_LENGTH(test_data));
        LOG_ERRFMT("bytes_read_count %d for flash emu", bytes_read_count);
        ret->val = TEST_FAILED;
    }
    if (memcmp((uint8_t*)test_data, actual_data, ARRAY_LENGTH(actual_data)) !=
        0) {
        LOG_ERRFMT("Data written != Data read\r\n");
        ret->val = TEST_FAILED;
    }

    LOG_INFFMT("PASS: %s\n\r", __func__);
    ret->val = TEST_PASSED;
}