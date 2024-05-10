/*
 * Copyright (c) 2019-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "boot_hal.h"

#include <string.h>

#ifdef PLATFORM_HAS_BOOT_DMA
#include "boot_dma.h"
#endif /* PLATFORM_HAS_BOOT_DMA */
#include "flash_layout.h"
#include "bootutil/bootutil_log.h"
#include "bootutil/bootutil.h"
#include "device_definition.h"
#include "host_base_address.h"
#include "platform_base_address.h"
#include "platform_regs.h"

#ifdef CRYPTO_HW_ACCELERATOR
#include "crypto_hw.h"
#include "fih.h"
#endif /* CRYPTO_HW_ACCELERATOR */
#include "bl2_image_id.h"
#include "Driver_Flash.h"
#include "host_flash_atu.h"
#include "sic_boot.h"
#include "plat_def_fip_uuid.h"
#include "flash_map/flash_map.h"

#ifdef FLASH_DEV_NAME
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;
#endif /* FLASH_DEV_NAME */

extern struct boot_rsp rsp;
extern struct flash_area flash_map[];
extern const int flash_map_entry_num;

int32_t boot_platform_post_init(void)
{
    enum mhu_v2_x_error_t status;
#ifdef PLATFORM_HAS_BOOT_DMA
    enum tfm_plat_err_t plat_err;
#endif /* PLATFORM_HAS_BOOT_DMA */

#ifdef CRYPTO_HW_ACCELERATOR
    int32_t result;

    result = crypto_hw_accelerator_init();
    if (result) {
        return 1;
    }

    (void)fih_delay_init();
#endif /* CRYPTO_HW_ACCELERATOR */

    status = mhu_v2_x_driver_init(&MHU_SCP_TO_RSS_DEV, MHU_REV_READ_FROM_HW);
    if (status != MHU_V_2_X_ERR_NONE) {
        BOOT_LOG_ERR("SCP->RSS MHU driver initialization failed");
        return 1;
    }
    BOOT_LOG_INF("SCP->RSS MHU driver initialized successfully");

    status = mhu_v2_x_driver_init(&MHU_RSS_TO_SCP_DEV, MHU_REV_READ_FROM_HW);
    if (status != MHU_V_2_X_ERR_NONE) {
        BOOT_LOG_ERR("RSS->SCP MHU driver initialization failed");
        return 1;
    }
    BOOT_LOG_INF("RSS->SCP MHU driver initialized successfully");

#ifdef PLATFORM_HAS_BOOT_DMA
    plat_err = boot_dma_init_cfg();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        BOOT_LOG_ERR("DMA driver initialization failed: ", plat_err);
        return 1;
    }
    BOOT_LOG_INF("DMA350 driver initialized successfully.");
#endif /* PLATFORM_HAS_BOOT_DMA */

#ifdef RSS_XIP
    result = sic_boot_init();
    if (result) {
        return result;
    }
#endif /* RSS_XIP */

    return 0;
}

static struct flash_area *flash_map_slot_from_flash_area_id(uint32_t area_id)
{
    uint32_t idx;
    for (idx = 0; idx < flash_map_entry_num; idx++) {
        if (area_id == flash_map[idx].fa_id) {
            return &flash_map[idx];
        }
    }
    return NULL;
}

int boot_platform_pre_load(uint32_t image_id)
{
    uuid_t uuid;
    uint32_t offsets[2];
    struct flash_area *flash_area_primary =
        flash_map_slot_from_flash_area_id(FLASH_AREA_IMAGE_PRIMARY(image_id));
    struct flash_area *flash_area_secondary =
        flash_map_slot_from_flash_area_id(FLASH_AREA_IMAGE_SECONDARY(image_id));
    int rc;

    if (flash_area_primary == NULL || flash_area_secondary == NULL) {
        return 1;
    }

    switch(image_id) {
    case RSS_BL2_IMAGE_SCP:
        uuid = UUID_RSS_FIRMWARE_SCP_BL1;
        break;
    case RSS_BL2_IMAGE_AP:
        uuid = UUID_RSS_FIRMWARE_AP_BL1;
        break;
    case RSS_BL2_IMAGE_NS:
#ifndef RSS_XIP
        uuid = UUID_RSS_FIRMWARE_NS;
#else
        uuid = UUID_RSS_SIC_TABLES_NS;
#endif /* RSS_XIP */
        break;
    case RSS_BL2_IMAGE_S:
#ifndef RSS_XIP
        uuid = UUID_RSS_FIRMWARE_S;
#else
        uuid = UUID_RSS_SIC_TABLES_S;
#endif /* RSS_XIP */
        break;
    default:
        return 1;
    }

    rc = host_flash_atu_init_regions_for_image(uuid, offsets);
    if (rc) {
        return rc;
    }

    flash_area_primary->fa_off += offsets[0];
    flash_area_secondary->fa_off += offsets[1];

    return 0;
}

int boot_platform_post_load(uint32_t image_id)
{
    int err;

#ifdef RSS_XIP
    err = sic_boot_post_load(image_id, rsp.br_image_off);
    if (err) {
        return err;
    }
#endif /* RSS_XIP */

    if (image_id == RSS_BL2_IMAGE_SCP) {
        memset(HOST_BOOT_IMAGE1_LOAD_BASE_S, 0, HOST_IMAGE_HEADER_SIZE);
        uint32_t channel_stat = 0;
        struct rss_sysctrl_t *sysctrl =
                                     (struct rss_sysctrl_t *)RSS_SYSCTRL_BASE_S;

        /* Release SCP CPU from wait */
        sysctrl->gretreg = 0x1;

        /* Wait for SCP to finish its startup */
        BOOT_LOG_INF("Waiting for SCP BL1 started event");
        while (channel_stat == 0) {
            mhu_v2_x_channel_receive(&MHU_SCP_TO_RSS_DEV, 0, &channel_stat);
        }
        BOOT_LOG_INF("Got SCP BL1 started event");

    } else if (image_id == RSS_BL2_IMAGE_AP) {
        memset(HOST_BOOT_IMAGE0_LOAD_BASE_S, 0, HOST_IMAGE_HEADER_SIZE);
        BOOT_LOG_INF("Telling SCP to start AP cores");
        mhu_v2_x_initiate_transfer(&MHU_RSS_TO_SCP_DEV);
        /* Slot 0 is used in the SCP protocol */
        mhu_v2_x_channel_send(&MHU_RSS_TO_SCP_DEV, 0, 1);
    }

    err = host_flash_atu_uninit_regions();
    if (err) {
        return err;
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

    vt_cpy = vt;

#ifdef RSS_XIP
    result = sic_boot_pre_quit(&vt_cpy);
    if (result) {
        while(1){}
    }
#endif /* RSS_XIP */

#ifdef CRYPTO_HW_ACCELERATOR
    result = crypto_hw_accelerator_finish();
    if (result) {
        while(1){}
    }
#endif /* CRYPTO_HW_ACCELERATOR */

#ifdef FLASH_DEV_NAME
    result = FLASH_DEV_NAME.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while(1){}
    }
#endif /* FLASH_DEV_NAME */
#ifdef FLASH_DEV_NAME_2
    result = FLASH_DEV_NAME_2.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while(1){}
    }
#endif /* FLASH_DEV_NAME_2 */
#ifdef FLASH_DEV_NAME_3
    result = FLASH_DEV_NAME_3.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while(1){}
    }
#endif /* FLASH_DEV_NAME_3 */
#ifdef FLASH_DEV_NAME_SCRATCH
    result = FLASH_DEV_NAME_SCRATCH.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while(1){}
    }
#endif /* FLASH_DEV_NAME_SCRATCH */

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
