/*
 * Copyright (c) 2016-2023, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GPT_H
#define GPT_H

#include "stdint.h"
#include "stddef.h"
#include "uuid.h"

#define PARTITION_TYPE_GPT     0xee
#define EFI_NAMELEN_MAX        36
#define GPT_HEADER_OFFSET      PLAT_PARTITION_BLOCK_SIZE
#define GPT_ENTRY_OFFSET       (GPT_HEADER_OFFSET + PLAT_PARTITION_BLOCK_SIZE)
#define GPT_SIGNATURE          "EFI PART"
#define PRIMARY_FIP_GPT_NAME   "FIP_A"
#define SECONDARY_FIP_GPT_NAME "FIP_B"

typedef struct gpt_entry {
    struct efi_guid type_uuid;
    struct efi_guid unique_uuid;
    uint64_t        first_lba;
    uint64_t        last_lba;
    uint64_t        attr;
    uint16_t        name[EFI_NAMELEN_MAX];
} gpt_entry_t;

typedef struct gpt_header {
    uint8_t         signature[8];
    uint32_t        revision;
    uint32_t        size;
    uint32_t        header_crc;
    uint32_t        reserved;
    uint64_t        current_lba;
    uint64_t        backup_lba;
    uint64_t        first_lba;
    uint64_t        last_lba;
    struct efi_guid disk_uuid;
    /* starting LBA of array of partition entries */
    uint64_t        list_lba;
    /* number of partition entries in array */
    uint32_t        list_num;
    /* size of a single partition entry (usually 128) */
    uint32_t        list_entry_size;
    uint32_t        list_crc;
} gpt_header_t;

/**
 * \brief                    Read a GPT header from flash into a struct.
 *
 * \param[in]  table_base    The RSS address mapped to the GPT table base in
 *                           host flash.
 * \param[in]  atu_slot_size The size of the ATU region that was mapped for
 *                           access to this FIP. This is used to prevent reads
 *                           outside the mapped region.
 * \param[out] header        Pointer to a gpt_header_t struct that will be
 *                           filled with the header data.
 * \return                   0 if operation completed successfully, another
 *                           value on error.
 */
int gpt_get_header(uint32_t table_base, size_t atu_slot_size,
                   gpt_header_t *header);

/**
 * \brief                       Read a GPT list entry from flash into a struct.
 *                              Which list entry should be read is determined
 *                              by matching the name of the partition.
 *
 * \param[in]  list_base        The RSS address mapped to the GPT entry list base
 *                              in host flash.
 * \param[in]  list_num_entries The number of entries in the GPT entry list, as
 *                              read from the header.
 * \param[in]  list_entry_size  The size of entries in the GPT entry list, as
 *                              read from the header.
 * \param[in]  name             The name of the partition whose list entry
 *                              should be read into the struct. Unlike the GPT
 *                              list entry spec, this must be an ascii-encoded
 *                              string.
 * \param[in]  name_size        The size of the name string.
 * \param[in]  atu_slot_size    The size of the ATU region that was mapped for
 *                              access to this FIP. This is used to prevent
 *                              reads outside the mapped region.
 * \param[out] entry            Pointer to a gpt_entry_t struct that will be
 *                              filled with the list entry data.
 * \return                      0 if operation completed successfully, another
 *                              value on error.
 */
int gpt_get_list_entry_by_name(uint32_t list_base, uint32_t list_num_entries,
                               size_t list_entry_size, uint8_t *name,
                               size_t name_size, size_t atu_slot_size,
                               gpt_entry_t *entry);

#endif /* GPT_H */
