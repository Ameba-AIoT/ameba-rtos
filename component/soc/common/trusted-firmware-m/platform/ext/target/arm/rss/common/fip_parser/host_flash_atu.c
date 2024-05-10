/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "host_flash_atu.h"

#include "flash_layout.h"
#include "device_definition.h"
#include "gpt.h"
#include "fip_parser.h"
#include "plat_def_fip_uuid.h"
#include "host_base_address.h"
#include "platform_base_address.h"

#include <string.h>

#define RSS_ATU_REGION_TEMP_SLOT           2
#define RSS_ATU_REGION_INPUT_IMAGE_SLOT_0  3
#define RSS_ATU_REGION_INPUT_IMAGE_SLOT_1  4
#define RSS_ATU_REGION_OUTPUT_IMAGE_SLOT   5
#define RSS_ATU_REGION_OUTPUT_HEADER_SLOT  6

static inline uint32_t round_down(uint32_t num, uint32_t boundary)
{
    return num - (num % boundary);
}

static inline uint32_t round_up(uint32_t num, uint32_t boundary)
{
    return (num + boundary - 1) - ((num + boundary - 1) % boundary);
}

static int setup_aligned_atu_slot(uint64_t physical_address, uint32_t size,
                           uint32_t boundary, uint32_t atu_slot,
                           uint32_t logical_address,
                           uint32_t *alignment_offset,
                           size_t   *atu_slot_size)
{
    uint64_t aligned_physical_address;
    int err;

    aligned_physical_address = round_down(physical_address, boundary);
    *atu_slot_size = round_up(physical_address + size, boundary)
                     - aligned_physical_address;

    *alignment_offset = physical_address - aligned_physical_address;

    /* Sanity check our parameters, as we do _not_ trust them. We can only map
     * within the host flash, the parameters must not overflow, and we cannot
     * map further than the bounds of the logical address slot.
     */
    if (aligned_physical_address < HOST_FLASH0_BASE
        || aligned_physical_address + *atu_slot_size > HOST_FLASH0_BASE + HOST_FLASH0_SIZE
        || aligned_physical_address + *atu_slot_size < aligned_physical_address
        || aligned_physical_address + *atu_slot_size < *atu_slot_size
        || *atu_slot_size > HOST_IMAGE_MAX_SIZE
        || *alignment_offset > boundary) {
        return 1;
    }

    err = atu_initialize_region(&ATU_DEV_S, atu_slot, logical_address,
                                aligned_physical_address, *atu_slot_size);
    if (err != ATU_ERR_NONE) {
        return 1;
    }

    return 0;
}

int host_flash_atu_setup_image_input_slots_from_fip(uint64_t fip_offset,
                                                    uint32_t slot,
                                                    uintptr_t logical_address,
                                                    uuid_t image_uuid,
                                                    uint32_t *logical_address_offset)
{
    enum atu_error_t err;
    int rc;
    uint64_t region_offset;
    size_t region_size;
    uint64_t physical_address = HOST_FLASH0_BASE + fip_offset;
    uint32_t alignment_offset;
    size_t atu_slot_size;
    size_t page_size = get_page_size(&ATU_DEV_S);

    /* There's no way to tell how big the FIP TOC will be before reading it, so
     * we just map 0x1000.
     */
    rc = setup_aligned_atu_slot(physical_address, 0x1000, page_size,
                                RSS_ATU_REGION_TEMP_SLOT,
                                HOST_FLASH0_TEMP_BASE_S, &alignment_offset,
                                &atu_slot_size);
    if (rc) {
        return rc;
    }

    rc = fip_get_entry_by_uuid(HOST_FLASH0_TEMP_BASE_S + alignment_offset,
                               atu_slot_size - alignment_offset,
                               image_uuid, &region_offset, &region_size);
    if (rc) {
        return rc;
    }

    err = atu_uninitialize_region(&ATU_DEV_S, RSS_ATU_REGION_TEMP_SLOT);
    if (err != ATU_ERR_NONE) {
        return 1;
    }

    /* Initialize primary input region */
    rc = setup_aligned_atu_slot(physical_address + region_offset, region_size,
                                page_size, slot, logical_address,
                                &alignment_offset, &atu_slot_size);
    if (rc) {
        return rc;
    }

    *logical_address_offset = alignment_offset;

    return 0;
}

int host_flash_atu_get_fip_offsets(bool fip_found[2], uint64_t fip_offsets[2])
{
#ifdef RSS_GPT_SUPPORT
    int rc;
    enum atu_error_t err;
    gpt_header_t header;
    gpt_entry_t entry;
    size_t page_size = get_page_size(&ATU_DEV_S);
    uint64_t physical_address;
    uint32_t alignment_offset;
    size_t atu_slot_size;
#endif /* RSS_GPT_SUPPORT */

#ifdef RSS_GPT_SUPPORT
    physical_address = HOST_FLASH0_BASE + FLASH_LBA_SIZE;
    rc = setup_aligned_atu_slot(physical_address, FLASH_LBA_SIZE,
                                page_size, RSS_ATU_REGION_TEMP_SLOT,
                                HOST_FLASH0_TEMP_BASE_S, &alignment_offset,
                                &atu_slot_size);
    if (rc) {
        return rc;
    }

    rc = gpt_get_header(HOST_FLASH0_TEMP_BASE_S + alignment_offset,
                        atu_slot_size - alignment_offset, &header);
    if (rc) {
        return rc;
    }

    err = atu_uninitialize_region(&ATU_DEV_S,
                                  RSS_ATU_REGION_TEMP_SLOT);
    if (err != ATU_ERR_NONE) {
        return 1;
    }

    physical_address = HOST_FLASH0_BASE
                       + header.list_lba * FLASH_LBA_SIZE;
    rc = setup_aligned_atu_slot(physical_address,
                                header.list_entry_size * header.list_num, page_size,
                                RSS_ATU_REGION_TEMP_SLOT,
                                HOST_FLASH0_TEMP_BASE_S, &alignment_offset,
                                &atu_slot_size);
    if (rc) {
        return rc;
    }

    rc = gpt_get_list_entry_by_name(HOST_FLASH0_TEMP_BASE_S + alignment_offset,
                                    header.list_num, header.list_entry_size,
                                    (uint8_t *)PRIMARY_FIP_GPT_NAME,
                                    sizeof(PRIMARY_FIP_GPT_NAME),
                                    atu_slot_size - alignment_offset, &entry);
    if (rc == 0) {
        fip_found[0] = true;
        fip_offsets[0] = entry.first_lba * FLASH_LBA_SIZE;
    } else {
        fip_found[0] = false;
    }

    rc = gpt_get_list_entry_by_name(HOST_FLASH0_TEMP_BASE_S + alignment_offset,
                                    header.list_num, header.list_entry_size,
                                    (uint8_t *)SECONDARY_FIP_GPT_NAME,
                                    sizeof(SECONDARY_FIP_GPT_NAME),
                                    atu_slot_size - alignment_offset, &entry);
    if (rc == 0) {
        fip_found[1] = true;
        fip_offsets[1] = entry.first_lba * FLASH_LBA_SIZE;
    } else {
        fip_found[1] = false;
    }

    err = atu_uninitialize_region(&ATU_DEV_S,
                                  RSS_ATU_REGION_TEMP_SLOT);
    if (err != ATU_ERR_NONE) {
        return 1;
    }
#else
    fip_found[0] = true;
    fip_offsets[0] = FLASH_FIP_A_OFFSET;
    fip_found[1] = true;
    fip_offsets[1] = FLASH_FIP_B_OFFSET;
#endif /* RSS_GPT_SUPPORT */

    return 0;
}

static int setup_image_input_slots(uuid_t image_uuid, uint32_t offsets[2])
{
    int rc;
    bool fip_found[2];
    uint64_t fip_offsets[2];
    bool fip_mapped[2] = {false};

    rc = host_flash_atu_get_fip_offsets(fip_found, fip_offsets);
    if (rc) {
        return rc;
    }

    /* MCUBoot requires that we map both regions. If we can only have the offset
     * of one, then map it to both slots.
     */
    if (fip_found[0] && !fip_found[1]) {
        fip_offsets[1] = fip_offsets[0];
    } else if (fip_found[1] && !fip_found[0]) {
        fip_offsets[0] = fip_offsets[1];
    } else if (!fip_found[0] && !fip_found[1]) {
        return 1;
    }

    rc = host_flash_atu_setup_image_input_slots_from_fip(fip_offsets[0],
                                          RSS_ATU_REGION_INPUT_IMAGE_SLOT_0,
                                          HOST_FLASH0_IMAGE0_BASE_S, image_uuid,
                                          &offsets[0]);
    if (rc == 0) {
        fip_mapped[0] = true;
    }

    rc = host_flash_atu_setup_image_input_slots_from_fip(fip_offsets[1],
                                          RSS_ATU_REGION_INPUT_IMAGE_SLOT_1,
                                          HOST_FLASH0_IMAGE1_BASE_S, image_uuid,
                                          &offsets[1]);
    if (rc == 0) {
        fip_mapped[1] = true;
    }


    /* If one of the mappings failed (more common without GPT support since in
     * that case we're just hoping there's a FIP at the offset) then map the
     * other one into the slot. At this stage if a backup map fails then it's an
     * error since otherwise MCUBoot will attempt to access unmapped ATU space
     * and fault.
     */
    if (fip_mapped[0] && !fip_mapped[1]) {
        rc = host_flash_atu_setup_image_input_slots_from_fip(fip_offsets[0],
                                              RSS_ATU_REGION_INPUT_IMAGE_SLOT_1,
                                              HOST_FLASH0_IMAGE1_BASE_S,
                                              image_uuid, &offsets[1]);
        if (rc) {
            return rc;
        }
    } else if (fip_mapped[1] && !fip_mapped[0]) {
        rc = host_flash_atu_setup_image_input_slots_from_fip(fip_offsets[1],
                                              RSS_ATU_REGION_INPUT_IMAGE_SLOT_0,
                                              HOST_FLASH0_IMAGE0_BASE_S,
                                              image_uuid, &offsets[0]);
        if (rc) {
            return rc;
        }
    } else if (!fip_mapped[0] && !fip_mapped[1]) {
        return 1;
    }

    return 0;
}

static int setup_image_output_slots(uuid_t image_uuid)
{
    uuid_t case_uuid;
    enum atu_error_t atu_err;

    case_uuid = UUID_RSS_FIRMWARE_SCP_BL1;
    if (memcmp(&image_uuid, &case_uuid, sizeof(uuid_t)) == 0) {
        /* Initialize SCP ATU header region */
        atu_err = atu_initialize_region(&ATU_DEV_S,
                                        RSS_ATU_REGION_OUTPUT_HEADER_SLOT,
                                        HOST_BOOT_IMAGE1_LOAD_BASE_S,
                                        SCP_BOOT_SRAM_BASE + SCP_BOOT_SRAM_SIZE
                                        - HOST_IMAGE_HEADER_SIZE,
                                        HOST_IMAGE_HEADER_SIZE);
        if (atu_err != ATU_ERR_NONE) {
            return 1;
        }

        /* Initialize SCP ATU output region */
        atu_err = atu_initialize_region(&ATU_DEV_S,
                                        RSS_ATU_REGION_OUTPUT_IMAGE_SLOT,
                                        HOST_BOOT_IMAGE1_LOAD_BASE_S + HOST_IMAGE_HEADER_SIZE,
                                        SCP_BOOT_SRAM_BASE,
                                        SCP_BOOT_SRAM_SIZE - HOST_IMAGE_HEADER_SIZE);
        if (atu_err != ATU_ERR_NONE) {
            return 1;
        }

        return 0;
    }

    case_uuid = UUID_RSS_FIRMWARE_AP_BL1;
    if (memcmp(&image_uuid, &case_uuid, sizeof(uuid_t)) == 0) {
        /* Initialize AP ATU header region */
        atu_err = atu_initialize_region(&ATU_DEV_S,
                                        RSS_ATU_REGION_OUTPUT_HEADER_SLOT,
                                        HOST_BOOT_IMAGE0_LOAD_BASE_S,
                                        AP_BOOT_SRAM_BASE + AP_BOOT_SRAM_SIZE
                                        - HOST_IMAGE_HEADER_SIZE,
                                        HOST_IMAGE_HEADER_SIZE);
        if (atu_err != ATU_ERR_NONE) {
            return 1;
        }
        /* Initialize AP ATU region */
        atu_err = atu_initialize_region(&ATU_DEV_S,
                                        RSS_ATU_REGION_OUTPUT_IMAGE_SLOT,
                                        HOST_BOOT_IMAGE0_LOAD_BASE_S + HOST_IMAGE_HEADER_SIZE,
                                        AP_BOOT_SRAM_BASE,
                                        AP_BOOT_SRAM_SIZE - HOST_IMAGE_HEADER_SIZE);
        if (atu_err != ATU_ERR_NONE) {
            return 1;
        }

        return 0;
    }

    return 0;
}

int host_flash_atu_init_regions_for_image(uuid_t image_uuid, uint32_t offsets[2])
{
    int rc;

    rc = setup_image_input_slots(image_uuid, offsets);
    if (rc) {
        return rc;
    }

    rc = setup_image_output_slots(image_uuid);
    if (rc) {
        return rc;
    }

    return 0;
}

int host_flash_atu_uninit_regions(void)
{
    enum atu_error_t atu_err;

    atu_err = atu_uninitialize_region(&ATU_DEV_S,
                                      RSS_ATU_REGION_INPUT_IMAGE_SLOT_0);
    if (atu_err != ATU_ERR_NONE) {
        return 1;
    }

    atu_err = atu_uninitialize_region(&ATU_DEV_S,
                                      RSS_ATU_REGION_INPUT_IMAGE_SLOT_1);
    if (atu_err != ATU_ERR_NONE) {
        return 1;
    }

    atu_err = atu_uninitialize_region(&ATU_DEV_S,
                                      RSS_ATU_REGION_OUTPUT_IMAGE_SLOT);
    if (atu_err != ATU_ERR_NONE) {
        return 1;
    }

    atu_err = atu_uninitialize_region(&ATU_DEV_S,
                                      RSS_ATU_REGION_OUTPUT_HEADER_SLOT);
    if (atu_err != ATU_ERR_NONE) {
        return 1;
    }

    return 0;
}
