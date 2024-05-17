/*
 * Copyright (c) 2020-2022, Arm Limited. All rights reserved.
 * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <arm_cmse.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "array.h"
#include "cmsis.h"
#include "Driver_Common.h"
#include "mmio_defs.h"
#include "mpu_armv8m_drv.h"
#include "region.h"
#include "target_cfg.h"
#include "tfm_hal_defs.h"
#include "tfm_hal_isolation.h"
#include "tfm_plat_defs.h"
#include "region_defs.h"
#include "tfm_peripherals_def.h"
#include "load/partition_defs.h"
#include "load/asset_defs.h"
#include "load/spm_load_api.h"

#ifdef FLOW_CONTROL
#include "target_flowcontrol.h"
#else
/* dummy definitions */
extern volatile uint32_t uFlowStage;
#define FLOW_CONTROL_STEP(C,B,A) ((void)0)
#define FLOW_CONTROL_CHECK(B,A) ((void)0)
#define FLOW_STAGE_CFG          (0x0)
#define FLOW_STAGE_CHK          (0x1)
#endif

/* It can be retrieved from the MPU_TYPE register. */
#define MPU_REGION_NUM                  8
#define PROT_BOUNDARY_VAL \
    ((1U << HANDLE_ATTR_PRIV_POS) & HANDLE_ATTR_PRIV_MASK)

#ifdef CONFIG_TFM_ENABLE_MEMORY_PROTECT
static uint32_t n_configured_regions = 0;

struct mpu_armv8m_dev_t dev_mpu_s = { MPU_BASE };
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_UNPRIV_DATA, $$RW$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_DATA, $$ZI$$Limit);

REGION_DECLARE(Image$$, TFM_APP_RW_STACK_START, $$Base);
REGION_DECLARE(Image$$, TFM_APP_RW_STACK_END, $$Base);

REGION_DECLARE(Image$$, ER_VENEER, $$Limit);

const struct mpu_armv8m_region_cfg_t region_cfg[] = {
    /* TFM Core unprivileged code region */
    {
        0, /* will be updated before using */
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Base),
        FLASH_BASE_S + FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE - 32,
        MPU_ARMV8M_MAIR_ATTR_CODE_IDX,
        MPU_ARMV8M_XN_EXEC_OK,
        MPU_ARMV8M_AP_RO_PRIV_UNPRIV,
        MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
        FLOW_STEP_MPU_S_EN_R0,
        FLOW_CTRL_MPU_S_EN_R0,
        FLOW_STEP_MPU_S_CH_R0,
        FLOW_CTRL_MPU_S_CH_R0,
#endif /* FLOW_CONTROL */
    },
    /* TFM_Core privileged code region   */
    {
        0, /* will be updated before using */
        S_CODE_START,
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Base),
        MPU_ARMV8M_MAIR_ATTR_CODE_IDX,
        MPU_ARMV8M_XN_EXEC_OK,
        MPU_ARMV8M_AP_RO_PRIV_ONLY,
        MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
        FLOW_STEP_MPU_S_EN_R2,
        FLOW_CTRL_MPU_S_EN_R2,
        FLOW_STEP_MPU_S_CH_R2,
        FLOW_CTRL_MPU_S_CH_R2,
#endif /* FLOW_CONTROL */
    },
    {
      0, /* will be updated before using */
        (uint32_t)&REGION_NAME(Image$$, TFM_APP_RW_STACK_START, $$Base),
        (uint32_t)&REGION_NAME(Image$$, TFM_APP_RW_STACK_END, $$Base),
        MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
        MPU_ARMV8M_XN_EXEC_NEVER,
        MPU_ARMV8M_AP_RW_PRIV_UNPRIV,
        MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
        FLOW_STEP_MPU_S_EN_R1,
        FLOW_CTRL_MPU_S_EN_R1,
        FLOW_STEP_MPU_S_CH_R1,
        FLOW_CTRL_MPU_S_CH_R1,
#endif /* FLOW_CONTROL */
    },
    /* TFM_Core privileged data region   */
    {
        0, /* will be updated before using */
        (uint32_t)&REGION_NAME(Image$$, TFM_APP_RW_STACK_END, $$Base),
        S_DATA_LIMIT + 1,
        MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
        MPU_ARMV8M_XN_EXEC_NEVER,
        MPU_ARMV8M_AP_RW_PRIV_ONLY,
        MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
        FLOW_STEP_MPU_S_EN_R3,
        FLOW_CTRL_MPU_S_EN_R3,
        FLOW_STEP_MPU_S_CH_R3,
        FLOW_CTRL_MPU_S_CH_R3,
#endif /* FLOW_CONTROL */
    },
    /* peripheral for AppROT partition */
    {
        0, /* will be updated before using */
        PERIPH_BASE_S,
        PERIPH_BASE_S+0x10000000,
        MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX,
        MPU_ARMV8M_XN_EXEC_NEVER,
        MPU_ARMV8M_AP_RW_PRIV_UNPRIV,
        MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
        FLOW_STEP_MPU_S_EN_R4,
        FLOW_CTRL_MPU_S_EN_R4,
        FLOW_STEP_MPU_S_CH_R4,
        FLOW_CTRL_MPU_S_CH_R4,
#endif /* FLOW_CONTROL */
    },
    /* TFM Non volatile data region (OTP/NVM/SST/ITS) */
    /* OTP is write protected (Option Byte protection) */
    {
        0, /* will be updated before using */
        TFM_NV_DATA_START,
        TFM_NV_DATA_LIMIT + 1,
        MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX,
        MPU_ARMV8M_XN_EXEC_NEVER,
        MPU_ARMV8M_AP_RW_PRIV_ONLY,
        MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
        FLOW_STEP_MPU_S_EN_R5,
        FLOW_CTRL_MPU_S_EN_R5,
        FLOW_STEP_MPU_S_CH_R5,
        FLOW_CTRL_MPU_S_CH_R5,
#endif /* FLOW_CONTROL */
    },
    /* RAM Non Secure Alias Not Executable */
    {
        0, /* will be updated before using */
        SRAM1_BASE_NS,
        SRAM4_BASE_NS + SRAM4_SIZE,
        MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
        MPU_ARMV8M_XN_EXEC_NEVER,
        MPU_ARMV8M_AP_RW_PRIV_ONLY,
        MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
        FLOW_STEP_MPU_S_EN_R6,
        FLOW_CTRL_MPU_S_EN_R6,
        FLOW_STEP_MPU_S_CH_R6,
        FLOW_CTRL_MPU_S_CH_R6,
#endif /* FLOW_CONTROL */
    },
    /* FLASH Non Secure Alias Not Executable, read only */
    {
        0, /* will be updated before using */
        FLASH_BASE_NS,
        FLASH_BASE_NS + FLASH_AREA_1_OFFSET + FLASH_AREA_1_SIZE - 32,
        MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX,
        MPU_ARMV8M_XN_EXEC_NEVER,
        MPU_ARMV8M_AP_RO_PRIV_ONLY,
        MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
        FLOW_STEP_MPU_S_EN_R7,
        FLOW_CTRL_MPU_S_EN_R7,
        FLOW_STEP_MPU_S_CH_R7,
        FLOW_CTRL_MPU_S_CH_R7,
#endif /* FLOW_CONTROL */
    },
};

static enum tfm_hal_status_t mpu_init(void)
{
    uint32_t i;
    struct mpu_armv8m_region_cfg_t localcfg;

    /* configuration stage */
    if (uFlowStage == FLOW_STAGE_CFG)
    {
        mpu_armv8m_clean(&dev_mpu_s);

        /* configure secure MPU regions */
        for (i = 0; i < ARRAY_SIZE(region_cfg); i++)
        {
            memcpy(&localcfg, &region_cfg[i], sizeof(localcfg));
            localcfg.region_nr = i;

            if (mpu_armv8m_region_enable(&dev_mpu_s,
                        &localcfg) != MPU_ARMV8M_OK)
            {
                return TFM_HAL_ERROR_GENERIC;
            }
            else
            {
                /* Execution stopped if flow control failed */
                FLOW_CONTROL_STEP(uFlowProtectValue, localcfg.flow_step_enable,
                        region_cfg[i].flow_ctrl_enable);
            }
        }
        n_configured_regions = i;

        /* enable secure MPU */
        mpu_armv8m_enable(&dev_mpu_s, PRIVILEGED_DEFAULT_ENABLE, HARDFAULT_NMI_ENABLE);
        FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_S_EN, FLOW_CTRL_MPU_S_EN);
    }
    /* verification stage */
    else
    {
        /* check secure MPU regions */
        for (i = 0; i < ARRAY_SIZE(region_cfg); i++)
        {
            memcpy(&localcfg, &region_cfg[i], sizeof(localcfg));
            localcfg.region_nr = i;

            if (mpu_armv8m_region_enable_check(&dev_mpu_s,
                        &localcfg) != MPU_ARMV8M_OK)
            {
                Error_Handler();
            }
            else
            {
                /* Execution stopped if flow control failed */
                FLOW_CONTROL_STEP(uFlowProtectValue, localcfg.flow_step_check,
                        region_cfg[i].flow_ctrl_check);
            }
        }

        /* check secure MPU */
        if (mpu_armv8m_check(&dev_mpu_s, PRIVILEGED_DEFAULT_ENABLE,
                    HARDFAULT_NMI_ENABLE) != MPU_ARMV8M_OK)
        {
            Error_Handler();
        }
        else
        {
            /* Execution stopped if flow control failed */
            FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_S_CH, FLOW_CTRL_MPU_S_CH);
        }

        /* Lock MPU config */
        __HAL_RCC_SYSCFG_CLK_ENABLE();
        /* in TFM_DEV_MODE MPU is not locked, this allows to see MPU configuration */
#ifndef TFM_DEV_MODE
        SYSCFG->CSLCKR |= SYSCFG_CSLCKR_LOCKSMPU;
#endif
        FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_S_LCK, FLOW_CTRL_MPU_S_LCK);
#ifndef TFM_DEV_MODE
        if (!(SYSCFG->CSLCKR & SYSCFG_CSLCKR_LOCKSMPU))
            Error_Handler();
#endif
        FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_S_LCK_CH, FLOW_CTRL_MPU_S_LCK_CH);
    }

    return TFM_HAL_SUCCESS;
}
#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */

FIH_RET_TYPE(enum tfm_hal_status_t) tfm_hal_set_up_static_boundaries(
                                            uintptr_t *p_spm_boundary)
{
    /* Set up isolation boundaries between SPE and NSPE */
    /* Configures non-secure memory spaces in the target */
    uFlowStage = FLOW_STAGE_CFG;
    gtzc_init_cfg();
    sau_and_idau_cfg();
    pinmux_init_cfg();

    /* Check configurations with Flow control to resist to basic HW attacks */
    uFlowStage = FLOW_STAGE_CHK;
    gtzc_init_cfg();
    sau_and_idau_cfg();

    /* Start HW randomization */
    if (RNG_Init()){
        Error_Handler();
    };

#ifdef CONFIG_TFM_ENABLE_MEMORY_PROTECT
    uFlowStage = FLOW_STAGE_CFG;
    mpu_init();
    uFlowStage = FLOW_STAGE_CHK;
    mpu_init();
#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */

    *p_spm_boundary = (uintptr_t)PROT_BOUNDARY_VAL;

    FIH_RET(fih_int_encode(TFM_HAL_SUCCESS));
}

#ifdef TFM_FIH_PROFILE_ON
fih_int tfm_hal_verify_static_boundaries(void)
{
    FIH_RET(fih_int_encode(TFM_HAL_SUCCESS));
}
#endif

/*
 * Implementation of tfm_hal_bind_boundary() on STM:
 *
 * The API encodes some attributes into a handle and returns it to SPM.
 * The attributes include isolation boundaries, privilege, and MMIO information.
 * When scheduler switches running partitions, SPM compares the handle between
 * partitions to know if boundary update is necessary. If update is required,
 * SPM passes the handle to platform to do platform settings and update
 * isolation boundaries.
 *
 * The handle should be unique under isolation level 3. The implementation
 * encodes an index at the highest 8 bits to assure handle uniqueness. While
 * under isolation level 1/2, handles may not be unique.
 *
 * The encoding format assignment:
 * - For isolation level 3
 *      BIT | 31        24 | 23         20 | ... | 7           4 | 3       0 |
 *          | Unique Index | Region Attr 5 | ... | Region Attr 1 | Base Attr |
 *
 *      In which the "Region Attr i" is:
 *      BIT |       3      | 2        0 |
 *          | 1: RW, 0: RO | MMIO Index |
 *
 *      In which the "Base Attr" is:
 *      BIT |               1                |                           0                     |
 *          | 1: privileged, 0: unprivileged | 1: Trustzone-specific NSPE, 0: Secure partition |
 *
 * - For isolation level 1/2
 *      BIT | 31     2 |              1                |                           0                     |
 *          | Reserved |1: privileged, 0: unprivileged | 1: Trustzone-specific NSPE, 0: Secure partition |
 *
 * This is a reference implementation on STM, and may have some limitations.
 * 1. The maximum number of allowed MMIO regions is 5.
 * 2. Highest 8 bits are for index. It supports 256 unique handles at most.
 */

FIH_RET_TYPE(enum tfm_hal_status_t) tfm_hal_bind_boundary(
                                    const struct partition_load_info_t *p_ldinf,
                                    uintptr_t *p_boundary)
{
    uint32_t i, j;
    bool privileged;
    bool ns_agent;
    uint32_t partition_attrs = 0;
    const struct asset_desc_t *p_asset;
    struct platform_data_t *plat_data_ptr;
#if TFM_LVL == 2
    struct mpu_armv8m_region_cfg_t localcfg;
#endif

    if (!p_ldinf || !p_boundary) {
        FIH_RET(fih_int_encode(TFM_HAL_ERROR_GENERIC));
    }

#if TFM_LVL == 1
    privileged = true;
#else
    privileged = IS_PSA_ROT(p_ldinf);
#endif

    ns_agent = IS_NS_AGENT(p_ldinf);
    p_asset = LOAD_INFO_ASSET(p_ldinf);

    /*
     * Validate if the named MMIO of partition is allowed by the platform.
     * Otherwise, skip validation.
     *
     * NOTE: Need to add validation of numbered MMIO if platform requires.
     */
    for (i = 0; i < p_ldinf->nassets; i++) {
        if (!(p_asset[i].attr & ASSET_ATTR_NAMED_MMIO)) {
            continue;
        }
        for (j = 0; j < ARRAY_SIZE(partition_named_mmio_list); j++) {
            if (p_asset[i].dev.dev_ref == partition_named_mmio_list[j]) {
                break;
            }
        }

        if (j == ARRAY_SIZE(partition_named_mmio_list)) {
            /* The MMIO asset is not in the allowed list of platform. */
            FIH_RET(fih_int_encode(TFM_HAL_ERROR_GENERIC));
        }
#if TFM_LVL == 2
        plat_data_ptr = REFERENCE_TO_PTR(p_asset[i].dev.dev_ref,
                                         struct platform_data_t *);
        /*
         * Static boundaries are set. Set up MPU region for MMIO.
         * Setup regions for unprivileged assets only.
         */
        if (!privileged) {
            localcfg.region_base = plat_data_ptr->periph_start;
            localcfg.region_limit = plat_data_ptr->periph_limit;
            localcfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DEVICE_IDX;
            localcfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_UNPRIV;
            localcfg.attr_sh = MPU_ARMV8M_SH_NONE;
            localcfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
            localcfg.region_nr = n_configured_regions++;

            if (mpu_armv8m_region_enable(&dev_mpu_s, &localcfg)
                != MPU_ARMV8M_OK) {
                FIH_RET(fih_int_encode(TFM_HAL_ERROR_GENERIC));
            }
        }
#endif
    }
    partition_attrs = ((uint32_t)privileged << HANDLE_ATTR_PRIV_POS) &
                        HANDLE_ATTR_PRIV_MASK;
    partition_attrs |= ((uint32_t)ns_agent << HANDLE_ATTR_NS_POS) &
                        HANDLE_ATTR_NS_MASK;
    *p_boundary = (uintptr_t)partition_attrs;

    FIH_RET(fih_int_encode(TFM_HAL_SUCCESS));
}

FIH_RET_TYPE(enum tfm_hal_status_t) tfm_hal_activate_boundary(
                             const struct partition_load_info_t *p_ldinf,
                             uintptr_t boundary)
{
    CONTROL_Type ctrl;
    uint32_t local_handle = (uint32_t)boundary;
    bool privileged = !!(local_handle & HANDLE_ATTR_PRIV_MASK);

    /* Privileged level is required to be set always */
    ctrl.w = __get_CONTROL();
    ctrl.b.nPRIV = privileged ? 0 : 1;
    __set_CONTROL(ctrl.w);

    FIH_RET(fih_int_encode(TFM_HAL_SUCCESS));
}

FIH_RET_TYPE(enum tfm_hal_status_t) tfm_hal_memory_check(uintptr_t boundary, uintptr_t base,
                                           size_t size, uint32_t access_type)
{
    int flags = 0;

    /* If size is zero, this indicates an empty buffer and base is ignored */
    if (size == 0) {
        FIH_RET(fih_int_encode(TFM_HAL_SUCCESS));
    }

    if (!base) {
        FIH_RET(fih_int_encode(TFM_HAL_ERROR_INVALID_INPUT));
    }

    if ((access_type & TFM_HAL_ACCESS_READWRITE) == TFM_HAL_ACCESS_READWRITE) {
        flags |= CMSE_MPU_READWRITE;
    } else if (access_type & TFM_HAL_ACCESS_READABLE) {
        flags |= CMSE_MPU_READ;
    } else {
        FIH_RET(fih_int_encode(TFM_HAL_ERROR_INVALID_INPUT));
    }

    if (!((uint32_t)boundary & HANDLE_ATTR_PRIV_MASK)) {
        flags |= CMSE_MPU_UNPRIV;
    }

    if ((uint32_t)boundary & HANDLE_ATTR_NS_MASK) {
        CONTROL_Type ctrl;
        ctrl.w = __TZ_get_CONTROL_NS();
        if (ctrl.b.nPRIV == 1) {
            flags |= CMSE_MPU_UNPRIV;
        } else {
            flags &= ~CMSE_MPU_UNPRIV;
        }
        flags |= CMSE_NONSECURE;
    }

    if (cmse_check_address_range((void *)base, size, flags) != NULL) {
        FIH_RET(fih_int_encode(TFM_HAL_SUCCESS));
    } else {
        FIH_RET(fih_int_encode(TFM_HAL_ERROR_MEM_FAULT));
    }
}

bool tfm_hal_boundary_need_switch(uintptr_t boundary_from,
                                  uintptr_t boundary_to)
{
    if (boundary_from == boundary_to) {
        return false;
    }

    if (((uint32_t)boundary_from & HANDLE_ATTR_PRIV_MASK) &&
        ((uint32_t)boundary_to & HANDLE_ATTR_PRIV_MASK)) {
        return false;
    }
    return true;
}
