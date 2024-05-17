/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "gpt.h"

#include "flash_layout.h"
#include "Driver_Flash.h"

#include <string.h>

extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

/* GPT has UTF-16LE strings, but we don't want to support them. FIP partitions
 * are always labelled using ANSI characters, so this function compares the
 * digits of the UTF-16LE string to an ANSI string.
 */
static int gpt_strncmp(uint16_t *gpt_str, size_t gpt_maxlen, uint8_t *str,
                       size_t str_len)
{
    size_t idx;

    if (str_len > gpt_maxlen) {
        return 1;
    }

    for (idx = 0; idx < str_len; idx++) {
        if (str[idx] != gpt_str[idx]) {
            return 1;
        }
    }

    return 0;
}

int gpt_get_header(uint32_t table_base, size_t atu_slot_size,
                   gpt_header_t *header)
{
    ARM_FLASH_CAPABILITIES DriverCapabilities = FLASH_DEV_NAME.GetCapabilities();
    /* Valid entries for data item width */
    uint32_t data_width_byte[] = {
        sizeof(uint8_t),
        sizeof(uint16_t),
        sizeof(uint32_t),
    };
    size_t data_width = data_width_byte[DriverCapabilities.data_width];
    int rc;

    if (atu_slot_size < sizeof(gpt_header_t)) {
        return 1;
    }

    rc = FLASH_DEV_NAME.ReadData(table_base - FLASH_BASE_ADDRESS, header,
                                 sizeof(gpt_header_t) / data_width);
    if (rc != sizeof(gpt_header_t) / data_width) {
        return rc;
    }

    if (memcmp(header->signature, "EFI PART", sizeof(header->signature))) {
        return 1;
    }

    return 0;
}

int gpt_get_list_entry_by_name(uint32_t list_base, uint32_t list_num_entries,
                               size_t list_entry_size, uint8_t *name,
                               size_t name_size, size_t atu_slot_size,
                               gpt_entry_t *entry)
{
    ARM_FLASH_CAPABILITIES DriverCapabilities = FLASH_DEV_NAME.GetCapabilities();
    /* Valid entries for data item width */
    uint32_t data_width_byte[] = {
        sizeof(uint8_t),
        sizeof(uint16_t),
        sizeof(uint32_t),
    };
    size_t data_width = data_width_byte[DriverCapabilities.data_width];
    int rc;
    uint64_t idx;

    /* List entry is using names larger than EFI_NAMELEN_MAX - this is either an
     * RSS GPT config error or an out-of-spec GPT partition, either way we fail
     * here.
     */
    if (list_entry_size > sizeof(gpt_entry_t)) {
        return 1;
    }

    /* Check for overflow */
    if (list_base + list_num_entries * list_entry_size < list_base ||
        list_num_entries * list_entry_size > atu_slot_size) {
        return 1;
    }

    for (idx = list_base;
         idx < list_base + list_num_entries * list_entry_size;
         idx += list_entry_size) {
        rc = FLASH_DEV_NAME.ReadData(idx - FLASH_BASE_ADDRESS, entry,
                                     sizeof(gpt_entry_t));
        if (rc != sizeof(gpt_entry_t) / data_width) {
            return rc;
        }

        if (gpt_strncmp(entry->name,
                        list_entry_size - offsetof(gpt_entry_t, name),
                        name, name_size)) {
            return 0;
        }
    }

    return 1;
}

