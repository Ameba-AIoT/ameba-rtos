/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sic_boot.h"

#include "device_definition.h"
#include "bl2_image_id.h"
#include "region_defs.h"
#include "tfm_plat_otp.h"
#include "host_flash_atu.h"
#include "plat_def_fip_uuid.h"

#include <string.h>

#define RSS_ATU_S_IMAGE_XIP_REGION  0
#define RSS_ATU_NS_IMAGE_XIP_REGION 1

#define RSS_SIC_S_IMAGE_DECRYPT_REGION  0
#define RSS_SIC_NS_IMAGE_DECRYPT_REGION 1

#define FLASH_SIC_HTR_SIZE 0x800

uint32_t s_image_offset;
uint32_t ns_image_offset;

struct rss_xip_htr_table {
    uint32_t fw_revision;
    uint32_t nonce[2];
    size_t htr_size;
    uint8_t htr[FLASH_SIC_HTR_SIZE];
};


int sic_boot_init(void)
{
    enum sic_error_t sic_err;

    sic_err = sic_auth_init(&SIC_DEV_S, SIC_DIGEST_SIZE_256,
                            SIC_DIGEST_COMPARE_FIRST_QWORD,
                            RSS_RUNTIME_S_XIP_BASE_S, FLASH_S_PARTITION_SIZE);
    if (sic_err != SIC_ERROR_NONE) {
        return 1;
    }

    sic_err = sic_decrypt_init(&SIC_DEV_S, SIC_DECRYPT_KEYSIZE_256, false);
    if (sic_err != SIC_ERROR_NONE) {
        return 1;
    }

    return 0;
}

int sic_boot_post_load(uint32_t image_id, uint32_t image_load_offset)
{
    enum sic_error_t sic_err;
    struct rss_xip_htr_table *table;
    uint32_t key[8];
    enum tfm_plat_err_t plat_err;
    size_t sic_page_size;
    enum tfm_otp_element_id_t decrypt_key_otp_id;
    uint32_t decrypt_region;
    uint32_t xip_region_base_addr;
    uint32_t xip_region_size;
    uint64_t fip_offsets[2];
    bool fip_found[2];
    uint64_t fip_offset;
    uint32_t atu_region;
    uuid_t image_uuid;
    uint32_t *image_offset;

    int rc;

    sic_page_size = sic_page_size_get(&SIC_DEV_S);

    rc = host_flash_atu_get_fip_offsets(fip_found, fip_offsets);
    if (rc) {
        return rc;
    }

    switch (image_id) {
    case RSS_BL2_IMAGE_NS:
        table = (struct rss_xip_htr_table*)(BL2_XIP_TABLES_START
                                            + FLASH_SIC_TABLE_SIZE
                                            + BL2_HEADER_SIZE);

        if (image_load_offset >= FLASH_AREA_11_OFFSET
            && image_load_offset < FLASH_AREA_11_OFFSET + FLASH_AREA_11_SIZE) {
            if (fip_found[0]) {
                fip_offset = fip_offsets[0];
            } else {
                return 1;
            }
        } else if (image_load_offset >= FLASH_AREA_13_OFFSET
                   && image_load_offset < FLASH_AREA_13_OFFSET + FLASH_AREA_13_SIZE) {
            if (fip_found[1]) {
                fip_offset = fip_offsets[1];
            } else {
                return 1;
            }
        } else {
            return 1;
        }

        decrypt_key_otp_id = PLAT_OTP_ID_KEY_NON_SECURE_ENCRYPTION;
        atu_region = RSS_ATU_NS_IMAGE_XIP_REGION;
        decrypt_region = RSS_SIC_NS_IMAGE_DECRYPT_REGION;
        xip_region_base_addr = RSS_RUNTIME_NS_XIP_BASE_S;
        xip_region_size      = NS_CODE_SIZE;
        image_uuid = UUID_RSS_FIRMWARE_NS;
        image_offset = &ns_image_offset;

        break;
    case RSS_BL2_IMAGE_S:
        table = (struct rss_xip_htr_table*)(BL2_XIP_TABLES_START + BL2_HEADER_SIZE);

        if (image_load_offset >= FLASH_AREA_10_OFFSET
            && image_load_offset < FLASH_AREA_10_OFFSET + FLASH_AREA_10_SIZE) {
            fip_offset = fip_offsets[0];
        } else if (image_load_offset >= FLASH_AREA_12_OFFSET
            && image_load_offset < FLASH_AREA_12_OFFSET + FLASH_AREA_12_SIZE) {
            fip_offset = fip_offsets[1];
        } else {
            return 1;
        }

        decrypt_key_otp_id = PLAT_OTP_ID_KEY_SECURE_ENCRYPTION;
        atu_region = RSS_ATU_S_IMAGE_XIP_REGION;
        decrypt_region = RSS_SIC_S_IMAGE_DECRYPT_REGION;
        xip_region_base_addr = RSS_RUNTIME_S_XIP_BASE_S;
        xip_region_size      = S_CODE_SIZE;
        image_uuid = UUID_RSS_FIRMWARE_S;
        image_offset = &s_image_offset;

        break;

    case RSS_BL2_IMAGE_AP:
    case RSS_BL2_IMAGE_SCP:
        return 0;
    default:
        return 1;
    }

    rc = host_flash_atu_setup_image_input_slots_from_fip(fip_offset,
                                                         atu_region,
                                                         xip_region_base_addr,
                                                         image_uuid,
                                                         image_offset);
    if (rc) {
        return rc;
    }

    /* RSS XIP images must be aligned to, at minimum, the SIC authentication
     * page size and the SIC decrypt page size. Alignments that do not match
     * the ATU page size cause problems in jumping to NS code, and seem to
     * cause startup failure in some cases, so 8KiB alignment is required.
     */
    if (*image_offset % 0x1000 != 0) {
        return 1;
    }


    sic_err = sic_auth_table_set(&SIC_DEV_S, (uint32_t*)(table->htr),
                                 table->htr_size, (xip_region_base_addr
                                                   - SIC_HOST_BASE_S)
                                                  / sic_page_size * 32);
    if (sic_err != SIC_ERROR_NONE) {
        return 1;
    }

    plat_err = tfm_plat_otp_read(decrypt_key_otp_id, sizeof(key), (uint8_t*)key);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return 1;
    }

    sic_err = sic_decrypt_region_enable(&SIC_DEV_S,
                                        decrypt_region, xip_region_base_addr,
                                        xip_region_size,
                                        image_load_offset, table->nonce,
                                        key);
    if (sic_err != SIC_ERROR_NONE) {
        memset(key, 0, sizeof(key));
        return 1;
    }


    memset(key, 0, sizeof(key));

    return 0;
}

int sic_boot_pre_quit(struct boot_arm_vector_table **vt_cpy)
{
    enum sic_error_t sic_err;

    sic_err = sic_auth_enable(&SIC_DEV_S);
    if (sic_err != SIC_ERROR_NONE) {
        return 1;
    }

    sic_err = sic_enable(&SIC_DEV_S);
    if (sic_err != SIC_ERROR_NONE) {
        return 1;
    }

    *vt_cpy = (struct boot_arm_vector_table *)(RSS_RUNTIME_S_XIP_BASE_S + s_image_offset);

    return 0;
}
