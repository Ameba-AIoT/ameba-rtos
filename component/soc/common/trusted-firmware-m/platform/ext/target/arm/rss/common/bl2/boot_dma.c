/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "bootutil/bootutil_log.h"
#include "boot_dma.h"
#include "cmsis.h"
#include "tfm_plat_defs.h"
#include "dma350_lib.h"
#include "device_definition.h"

#define BOOT_DMA0_COMBINED_S_SIGNAL                             (24)
#define BOOT_DMA_NUM_CHANNELS                                   (4)

static struct dma350_ch_dev_t *const dma350_channel_list[BOOT_DMA_NUM_CHANNELS] = {
    &DMA350_DMA0_CH0_DEV_S,
    &DMA350_DMA0_CH1_DEV_S,
    &DMA350_DMA0_CH2_DEV_S,
    &DMA350_DMA0_CH3_DEV_S
};

/* DMA350 checker layer has to know the TCM remaps */
/* Subordinate TCM Interface provides system access only to TCM internal to each
 * CPU. The memory map presented on the interface for TCM access are defined by
 * TRM. Below address remaps by adding offset provides access to respective
 * CPU instruction and data TCM.
 */

/* TCM memories addresses from perspective of cpu0
 * 0x0000_0000 .. 0x00ff_ffff    NS ITCM
 * 0x1000_0000 .. 0x10ff_ffff    S ITCM
 * 0x2000_0000 .. 0x20ff_ffff    NS DTCM
 * 0x3000_0000 .. 0x30ff_ffff    S DTCM
*/

const struct dma350_remap_range_t dma350_address_remap_list[] = {
    /* Non-secure CPU ITCM */
    {.begin = 0x00000000, .end = 0x00FFFFFF, .offset = 0x0A000000},
    /* Secure CPU ITCM */
    {.begin = 0x10000000, .end = 0x10FFFFFF, .offset = 0x0A000000},
    /* Non-secure CPU DTCM */
    {.begin = 0x20000000, .end = 0x20FFFFFF, .offset = 0x04000000},
    /* Secure CPU DTCM */
    {.begin = 0x30000000, .end = 0x30FFFFFF, .offset = 0x04000000}
};

const struct dma350_remap_list_t dma350_address_remap = {
    .size = sizeof(dma350_address_remap_list)/
            sizeof(dma350_address_remap_list[0]),
    .map = dma350_address_remap_list
};

/*------------------- DMA configuration functions -------------------------*/
enum tfm_plat_err_t boot_dma_init_cfg(void)
{
    enum dma350_error_t dma_err;
    enum dma350_ch_error_t ch_err;
    struct dma350_ch_dev_t *dma_ch_ptr;
    uint32_t i;

    dma_err = dma350_init(&DMA350_DMA0_DEV_S);
    if (dma_err != DMA350_ERR_NONE) {
        BOOT_LOG_ERR("DMA350_DMA0_DEV_S init failed!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    for (i = 0; i < BOOT_DMA_NUM_CHANNELS; i++) {
        dma_ch_ptr = dma350_channel_list[i];

        ch_err = dma350_ch_init(dma_ch_ptr);
        if (ch_err != DMA350_CH_ERR_NONE) {
            BOOT_LOG_ERR("DMA350_DMA0_CH%u_DEV_S init failed!", i);
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }

        dma_err = dma350_set_ch_privileged(&DMA350_DMA0_DEV_S, i);
        if (dma_err != DMA350_ERR_NONE) {
            BOOT_LOG_ERR("Failed to set DMA350_DM0 channel:%u privileged!",
                         i);
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }

        dma_err = dma350_set_ch_secure(&DMA350_DMA0_DEV_S, i);
        if (dma_err != DMA350_ERR_NONE) {
            BOOT_LOG_ERR("Failed to set DMA350_DM0 channel:%u secure!",
                         i);
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    /* Use combined secure interrupt because there are no channel IRQs */
    DMA350_DMA0_DEV_S.cfg->dma_sec_ctrl->SEC_CTRL |= 0x1UL;
    DMA350_DMA0_DEV_S.cfg->dma_nsec_ctrl->NSEC_CTRL |= 0x1UL;

    return TFM_PLAT_ERR_SUCCESS;
}

int32_t boot_dma_memcpy(uint32_t src_addr,
                        uint32_t dest_addr,
                        uint32_t size,
                        uint32_t ch_idx)
{
    struct dma350_ch_dev_t *dma_ch_ptr;
    enum dma350_lib_error_t dma_config_ret_val =
                                             DMA350_LIB_ERR_INVALID_CONFIG_TYPE;

    if (ch_idx >= BOOT_DMA_NUM_CHANNELS) {
        BOOT_LOG_ERR("[DMA350 BL2] Input dma channel: %u is invalid \r\n",
                     ch_idx);
        return -1;
    }

    dma_ch_ptr = dma350_channel_list[ch_idx];
    dma_config_ret_val = dma350_memcpy(dma_ch_ptr,
                                       (void *)src_addr,
                                       (void *)dest_addr,
                                       size,
                                       DMA350_LIB_EXEC_BLOCKING);

    if (dma_config_ret_val != 0) {
        BOOT_LOG_ERR("[DMA350 BL2] dma350_memcpy return value: 0x%x",
                     dma_config_ret_val);
        BOOT_LOG_ERR("[DMA350 BL2] Fail to copy %u bytes from 0x%x to 0x%x",
                     size, src_addr, dest_addr);
        return -1;
    }

    return 0;
}
