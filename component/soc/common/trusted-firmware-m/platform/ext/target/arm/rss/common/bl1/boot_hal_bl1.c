/*
 * Copyright (c) 2019-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "boot_hal.h"
#include "region.h"
#include "device_definition.h"
#include "Driver_Flash.h"
#include "flash_layout.h"
#include "host_base_address.h"
#include "region_defs.h"
#include "platform_base_address.h"
#include "uart_stdout.h"
#include "otp.h"
#include "crypto.h"
#include "tfm_plat_otp.h"
#include "trng.h"
#include "kmu_drv.h"
#include "device_definition.h"
#include "platform_regs.h"
#ifdef CRYPTO_HW_ACCELERATOR
#include "crypto_hw.h"
#include "fih.h"
#include "cc3xx_dma.h"
#endif /* CRYPTO_HW_ACCELERATOR */
#include <string.h>
#include "cmsis_compiler.h"
#include "fip_parser.h"
#include "host_flash_atu.h"
#include "plat_def_fip_uuid.h"

extern uint8_t computed_bl1_2_hash[];
extern uint32_t platform_code_is_bl1_2;
uint32_t image_offsets[2];

/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Base);

uint32_t bl1_image_get_flash_offset(uint32_t image_id)
{
    switch (image_id) {
    case 0:
        return HOST_FLASH0_IMAGE0_BASE_S - FLASH_BASE_ADDRESS + image_offsets[0];
    case 1:
        return HOST_FLASH0_IMAGE1_BASE_S - FLASH_BASE_ADDRESS + image_offsets[1];
    default:
        FIH_PANIC;
    }
}

static int32_t init_atu_regions(void)
{
    enum atu_error_t err;

#ifndef RSS_DEBUG_UART
    /* Initialize UART region */
    err = atu_initialize_region(&ATU_DEV_S,
                                get_supported_region_count(&ATU_DEV_S) - 1,
                                HOST_UART0_BASE_NS, HOST_UART_BASE,
                                HOST_UART_SIZE);
    if (err != ATU_ERR_NONE) {
        return 1;
    }
#endif /* !RSS_DEBUG_UART */

    return 0;
}

/* bootloader platform-specific hw initialization */
int32_t boot_platform_init(void)
{
    int32_t result;
    uint8_t prbg_seed[KMU_PRBG_SEED_LEN];

    /* Initialize stack limit register */
    uint32_t msp_stack_bottom =
            (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Base);

    __set_MSPLIM(msp_stack_bottom);

    /* Enable system reset for the RSS */
    struct rss_sysctrl_t *rss_sysctrl = (void *)RSS_SYSCTRL_BASE_S;
    rss_sysctrl->reset_mask |= (1U << 8U);

    result = init_atu_regions();
    if (result) {
        return result;
    }

#if defined(TFM_BL1_LOGGING) || defined(TEST_BL1_1) || defined(TEST_BL1_2)
    stdio_init();
#endif /* defined(TFM_BL1_LOGGING) || defined(TEST_BL1_1) || defined(TEST_BL1_2) */

    result = FLASH_DEV_NAME.Initialize(NULL);
    if (result != ARM_DRIVER_OK) {
        return 1;
    }

    result = bl1_trng_generate_random(prbg_seed, sizeof(prbg_seed));
    if (result != 0) {
        return result;
    }

    result = kmu_init(&KMU_DEV_S, prbg_seed);
    if (result != KMU_ERROR_NONE) {
        return result;
    }

    result = host_flash_atu_init_regions_for_image(UUID_RSS_FIRMWARE_BL2, image_offsets);
    if (result) {
        return result;
    }

    if(!platform_code_is_bl1_2) {
        /* Clear boot data area */
        memset((void*)BOOT_TFM_SHARED_DATA_BASE, 0, BOOT_TFM_SHARED_DATA_SIZE);
    }

    return 0;
}

int32_t boot_platform_post_init(void)
{
#ifdef CRYPTO_HW_ACCELERATOR
    int32_t result;
    uint32_t idx;
    cc3xx_dma_remap_region_t remap_regions[] = {
        {ITCM_BASE_S, ITCM_SIZE, ITCM_CPU0_BASE_S, 0x01000000},
        {ITCM_BASE_NS, ITCM_SIZE, ITCM_CPU0_BASE_NS, 0x01000000},
        {DTCM_BASE_S, DTCM_SIZE, DTCM_CPU0_BASE_S, 0x01000000},
        {DTCM_BASE_NS, DTCM_SIZE, DTCM_CPU0_BASE_NS, 0x01000000},
    };

    result = crypto_hw_accelerator_init();
    if (result) {
        return 1;
    }

    for (idx = 0; idx < (sizeof(remap_regions) / sizeof(remap_regions[0])); idx++) {
        result = cc3xx_dma_remap_region_init(idx, &remap_regions[idx]);
        if (result) {
            return 1;
        }
    }

    (void)fih_delay_init();
#endif /* CRYPTO_HW_ACCELERATOR */

    return 0;
}


static int rss_derive_key(enum tfm_bl1_key_id_t key_id, uint8_t *label,
                          size_t label_len, uint8_t *out, uint32_t lcs)
{
    int rc;
    uint8_t context[BL1_2_HASH_SIZE + sizeof(uint32_t) * 2] = {0};

    /* TODO load the reprovisioning_bits from OTP, so they can be updated. */
    uint32_t reprovisioning_bits = 0;

    memcpy(context, computed_bl1_2_hash, BL1_2_HASH_SIZE);

    memcpy(context + BL1_2_HASH_SIZE, &lcs, sizeof(uint32_t));

    memcpy(context + BL1_2_HASH_SIZE + sizeof(uint32_t), &reprovisioning_bits,
           sizeof(uint32_t));

    rc = bl1_derive_key(key_id, label, label_len, context,
                        sizeof(context), out, 32);

    return rc;
}

static int invalidate_hardware_keys(void)
{
    enum kmu_error_t kmu_err;
    uint32_t slot;

    for (slot = 0; slot < KMU_USER_SLOT_MIN; slot++) {
        kmu_err = kmu_set_slot_invalid(&KMU_DEV_S, slot);
        if (kmu_err != KMU_ERROR_NONE) {
            return 1;
        }
    }

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

    if (platform_code_is_bl1_2) {
        result = host_flash_atu_uninit_regions();
        if (result) {
            while(1){}
        }
    }


    result = invalidate_hardware_keys();
    if (result) {
        while(1){}
    }

#ifdef CRYPTO_HW_ACCELERATOR
    result = crypto_hw_accelerator_finish();
    if (result) {
        while (1);
    }
#endif /* CRYPTO_HW_ACCELERATOR */

    result = FLASH_DEV_NAME.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while (1){}
    }

#if defined(TFM_BL1_LOGGING) || defined(TEST_BL1_1) || defined(TEST_BL1_2)
    stdio_uninit();
#endif /* defined(TFM_BL1_LOGGING) || defined(TEST_BL1_1) || defined(TEST_BL1_2) */

    vt_cpy = vt;
#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__) \
 || defined(__ARM_ARCH_8_1M_MAIN__)
    /* Restore the Main Stack Pointer Limit register's reset value
     * before passing execution to runtime firmware to make the
     * bootloader transparent to it.
     */
    __set_MSPLIM(0);
#endif /* defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__) \
       || defined(__ARM_ARCH_8_1M_MAIN__) */

    __set_MSP(vt_cpy->msp);
    __DSB();
    __ISB();

    boot_jump_to_next_image(vt_cpy->reset);
}

int boot_platform_post_load(uint32_t image_id)
{
    int rc;
    uint32_t lcs;
    uint8_t vhuk_label[]      = "BL1_VHUK_DERIVATION";
    uint8_t cpak_seed_label[] = "BL1_CPAK_SEED_DERIVATION";
    uint8_t dak_seed_label[]  = "BL1_DAK_SEED_DERIVATION";
    /* The KMU requires word alignment */
    uint8_t __ALIGNED(4) tmp_buf[32];

    rc = tfm_plat_otp_read(PLAT_OTP_ID_LCS, sizeof(lcs), (uint8_t *)&lcs);
    if (rc) {
        return rc;
    }

    /* In PLAT_OTP_LCS_ASSEMBLY_AND_TEST the HUK and GUK may not have yet been
     * provisioned, and therefore the key derivation will fail, so skip it.
     */
    if (lcs != PLAT_OTP_LCS_ASSEMBLY_AND_TEST) {
        /* Derive a Virtual HUK that can be used by TF-M without exposing the
         * real HUK. This allows hardware protection of the HUK, and
         * reprovisioning of the device. This key is the same in all LCSes.
         */
        rc = rss_derive_key(TFM_BL1_KEY_HUK, vhuk_label, sizeof(vhuk_label),
                            tmp_buf, 0);
        if (rc) {
            return rc;
        }
        rc = kmu_set_key(&KMU_DEV_S, KMU_USER_SLOT_MIN + 0, tmp_buf,
                         sizeof(tmp_buf));
        if (rc) {
            return rc;
        }

        memset(tmp_buf, 0, sizeof(tmp_buf));

        rc = rss_derive_key(TFM_BL1_KEY_GUK, cpak_seed_label,
                            sizeof(cpak_seed_label), tmp_buf, lcs);
        if (rc) {
            return rc;
        }

        rc = kmu_set_key(&KMU_DEV_S, KMU_USER_SLOT_MIN + 1, tmp_buf,
                         sizeof(tmp_buf));
        if (rc) {
            return rc;
        }

        memset(tmp_buf, 0, sizeof(tmp_buf));

        rc = rss_derive_key(TFM_BL1_KEY_GUK, dak_seed_label,
                            sizeof(dak_seed_label), tmp_buf, lcs);
        if (rc) {
            return rc;
        }

        rc = kmu_set_key(&KMU_DEV_S, KMU_USER_SLOT_MIN + 2, tmp_buf,
                         sizeof(tmp_buf));
        if (rc) {
            return rc;
        }

        memset(tmp_buf, 0, sizeof(tmp_buf));
    }

    return 0;
}
