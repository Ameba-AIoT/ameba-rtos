/*
 * Copyright (c) 2019-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "cmsis.h"
#include "region.h"
#include "boot_hal.h"
#include "Driver_Flash.h"
#include "flash_layout.h"
#include "fih.h"
#include "bootutil/bootutil_log.h"
#include "fip_parser.h"
#include "flash_map/flash_map.h"
#include "watchdog.h"
#include <string.h>
#include "tfm_plat_otp.h"
#include "tfm_plat_provisioning.h"
#include "fwu_agent.h"

#ifdef PLATFORM_PSA_ADAC_SECURE_DEBUG
#include "psa_adac_platform.h"
#endif

#if defined(CRYPTO_HW_ACCELERATOR) || \
    defined(CRYPTO_HW_ACCELERATOR_OTP_PROVISIONING)
#include "crypto_hw.h"
#endif

#include "efi.h"
#include "partition.h"
#include "platform.h"

static const uint8_t * const tfm_part_names[] = {"tfm_primary", "tfm_secondary"};
static const uint8_t * const fip_part_names[] = {"FIP_A", "FIP_B"};


/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

REGION_DECLARE(Image$$, ER_DATA, $$Base)[];
REGION_DECLARE(Image$$, ARM_LIB_HEAP, $$ZI$$Limit)[];

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
extern struct flash_area flash_map[];

static bool fill_flash_map_with_tfm_data(uint8_t boot_index) {

    if (boot_index >= ARRAY_SIZE(tfm_part_names)) {
        BOOT_LOG_ERR("%d is an invalid boot_index, 0 <= boot_index < %d",
                     boot_index, ARRAY_SIZE(tfm_part_names));
        return false;
    }
    partition_entry_t *tfm_entry =
        get_partition_entry(tfm_part_names[boot_index]);
    if (tfm_entry == NULL) {
        BOOT_LOG_ERR("Could not find partition %s", tfm_part_names[boot_index]);
        return false;
    }
    flash_map[0].fa_off = tfm_entry->start;
    flash_map[0].fa_size = tfm_entry->length;
    return true;
}

static bool fill_flash_map_with_fip_data(uint8_t boot_index) {
    uint32_t tfa_offset = 0;
    size_t tfa_size = 0;
    uint32_t fip_offset = 0;
    size_t fip_size = 0;
    int result;

    if (boot_index >= ARRAY_SIZE(fip_part_names)) {
        BOOT_LOG_ERR("%d is an invalid boot_index, 0 <= boot_index < %d",
                     boot_index, ARRAY_SIZE(fip_part_names));
        return false;
    }
    partition_entry_t *fip_entry =
        get_partition_entry(fip_part_names[boot_index]);
    if (fip_entry == NULL) {
        BOOT_LOG_ERR("Could not find partition %s", fip_part_names[boot_index]);
        return false;
    }

    fip_offset = fip_entry->start;
    fip_size = fip_entry->length;

    /* parse directly from flash using XIP mode */
    /* FIP is large so its not a good idea to load it in memory */
    Select_XIP_Mode_For_Shared_Flash();
    result = parse_fip_and_extract_tfa_info(
        FLASH_BASE_ADDRESS + fip_offset + FIP_SIGNATURE_AREA_SIZE, fip_size,
        &tfa_offset, &tfa_size);
    if (result != FIP_PARSER_SUCCESS) {
        BOOT_LOG_ERR("parse_fip_and_extract_tfa_info failed");
        return false;
    }
    Select_Write_Mode_For_Shared_Flash();
    flash_map[2].fa_off = fip_offset + FIP_SIGNATURE_AREA_SIZE + tfa_offset;
    flash_map[2].fa_size = tfa_size;
    flash_map[3].fa_off = flash_map[2].fa_off + flash_map[2].fa_size;
    flash_map[3].fa_size = tfa_size;

    return true;
}

#ifdef PLATFORM_PSA_ADAC_SECURE_DEBUG
int psa_adac_to_tfm_apply_permissions(uint8_t permissions_mask[16])
{
    (void)permissions_mask;

    int ret;
    uint32_t dcu_reg_values[4];

    /* Below values provide same access as when platform is in development
       life cycle state */
    dcu_reg_values[0] = 0xffffe7fc;
    dcu_reg_values[1] = 0x800703ff;
    dcu_reg_values[2] = 0xffffffff;
    dcu_reg_values[3] = 0xffffffff;

    ret = crypto_hw_apply_debug_permissions((uint8_t*)dcu_reg_values, 16);
    BOOT_LOG_INF("%s: debug permission apply %s\n\r", __func__,
            (ret == 0) ? "success" : "fail");

    return ret;
}

uint8_t secure_debug_rotpk[32];

#endif

int32_t boot_platform_init(void)
{
    int32_t result;
    uint8_t boot_index;

    result = corstone1000_watchdog_init();
    if (result != ARM_DRIVER_OK) {
        return 1;
    }

    result = FLASH_DEV_NAME.Initialize(NULL);
    if (result != ARM_DRIVER_OK) {
        return 1;
    }

    plat_io_storage_init();
    partition_init(PLATFORM_GPT_IMAGE);

    boot_index = bl2_get_boot_bank();

    if (!fill_flash_map_with_tfm_data(boot_index)
#ifndef TFM_S_REG_TEST
    || !fill_flash_map_with_fip_data(boot_index)
#endif
    ) {
        BOOT_LOG_ERR("Filling flash map has failed!");
        return 1;
    }

    return 0;
}

int32_t boot_platform_post_init(void)
{
    int32_t result;
    uint32_t bank_offset;
    enum tfm_plat_err_t plat_err;

#ifdef CRYPTO_HW_ACCELERATOR
    result = crypto_hw_accelerator_init();
    if (result) {
        return 1;
    }
#endif /* CRYPTO_HW_ACCELERATOR */

    plat_err = tfm_plat_otp_init();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            BOOT_LOG_ERR("OTP system initialization failed");
            FIH_PANIC;
    }

#ifdef PLATFORM_PSA_ADAC_SECURE_DEBUG
    if (!tfm_plat_provisioning_is_required()) {

        plat_err = tfm_plat_otp_read(PLAT_OTP_ID_SECURE_DEBUG_PK, 32, secure_debug_rotpk);
        if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            return plat_err;
        }

        result = tfm_to_psa_adac_corstone1000_secure_debug(secure_debug_rotpk, 32);
        BOOT_LOG_INF("%s: dipda_secure_debug is a %s.\r\n", __func__,
                (result == 0) ? "success" : "failure");

    }
#endif

    return 0;
}

void boot_platform_quit(struct boot_arm_vector_table *vt)
{
    /* Clang at O0, stores variables on the stack with SP relative addressing.
     * When manually set the SP then the place of reset vector is lost.
     * Static variables are stored in 'data' or 'bss' section, change of SP has
     * no effect on them.
     */
    static struct boot_arm_vector_table *vt_cpy;
    int32_t result;

#ifdef CRYPTO_HW_ACCELERATOR
    result = crypto_hw_accelerator_finish();
    if (result) {
        while (1);
    }

    (void)fih_delay_init();
#endif /* CRYPTO_HW_ACCELERATOR */

    result = FLASH_DEV_NAME.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while (1);
    }

    vt_cpy = vt;

    __set_MSP(vt->msp);
    __DSB();
    __ISB();

    boot_jump_to_next_image(vt_cpy->reset);
}
