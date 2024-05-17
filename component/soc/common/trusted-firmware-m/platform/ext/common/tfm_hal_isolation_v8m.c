/*
 * Copyright (c) 2020-2023, Arm Limited. All rights reserved.
 * Copyright (c) 2022-2023 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * The current implementation in this file only supports isolation
 * level 1 and level 2.
 */

#include <arm_cmse.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "array.h"
#include "cmsis.h"
#include "region.h"
#include "mpu_armv8.h"
#include "common_target_cfg.h"
#include "tfm_hal_defs.h"
#include "tfm_hal_isolation.h"
#include "tfm_peripherals_def.h"
#include "load/spm_load_api.h"

#define PROT_BOUNDARY_VAL \
    ((1U << HANDLE_ATTR_PRIV_POS) & HANDLE_ATTR_PRIV_MASK)
/* Boundary handle binding macros. */
#define HANDLE_ATTR_PRIV_POS            1U
#define HANDLE_ATTR_PRIV_MASK           (0x1UL << HANDLE_ATTR_PRIV_POS)
#define HANDLE_ATTR_NS_POS              0U
#define HANDLE_ATTR_NS_MASK             (0x1UL << HANDLE_ATTR_NS_POS)

#ifdef CONFIG_TFM_ENABLE_MEMORY_PROTECT
static uint32_t n_configured_regions = 0;

#ifdef CONFIG_TFM_USE_TRUSTZONE
REGION_DECLARE(Image$$, ER_VENEER, $$Base);
REGION_DECLARE(Image$$, VENEER_ALIGN, $$Limit);
#endif /* CONFIG_TFM_USE_TRUSTZONE */
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_APP_CODE_START, $$Base);
REGION_DECLARE(Image$$, TFM_APP_CODE_END, $$Base);
REGION_DECLARE(Image$$, TFM_APP_RW_STACK_START, $$Base);
REGION_DECLARE(Image$$, TFM_APP_RW_STACK_END, $$Base);
#ifdef CONFIG_TFM_PARTITION_META
REGION_DECLARE(Image$$, TFM_SP_META_PTR, $$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_META_PTR, $$ZI$$Limit);
#endif /* CONFIG_TFM_PARTITION_META */

#define ARM_MPU_NON_TRANSIENT        ( 1U )
#define ARM_MPU_TRANSIENT            ( 0U )
#define ARM_MPU_WRITE_BACK           ( 1U )
#define ARM_MPU_WRITE_THROUGH        ( 0U )
#define ARM_MPU_READ_ALLOCATE        ( 1U )
#define ARM_MPU_NON_READ_ALLOCATE    ( 0U )
#define ARM_MPU_WRITE_ALLOCATE       ( 1U )
#define ARM_MPU_NON_WRITE_ALLOCATE   ( 0U )
#define ARM_MPU_READ_ONLY            ( 1U )
#define ARM_MPU_READ_WRITE           ( 0U )
#define ARM_MPU_UNPRIVILEGED         ( 1U )
#define ARM_MPU_PRIVILEGED           ( 0U )
#define ARM_MPU_EXECUTE_NEVER        ( 1U )
#define ARM_MPU_EXECUTE_OK           ( 0U )
#ifdef TFM_PXN_ENABLE
    #define ARM_MPU_PRIVILEGE_EXECUTE_NEVER        ( 1U )
    #define ARM_MPU_PRIVILEGE_EXECUTE_OK           ( 0U )
#endif

#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */

enum tfm_hal_status_t tfm_hal_set_up_static_boundaries(
                                            uintptr_t *p_spm_boundary)
{
#ifdef CONFIG_TFM_ENABLE_MEMORY_PROTECT
const ARM_MPU_Region_t mpu_region_attributes[] = {
#ifdef CONFIG_TFM_USE_TRUSTZONE
    /* Veneer region
     * Region Number 0, Non-shareable, Read-Only, Non-Privileged, Executable,
     * Privilege Executable - if PXN available, Attribute set: 0
     */
    {
        ARM_MPU_RBAR((uint32_t)&REGION_NAME(Image$$, ER_VENEER, $$Base),
                     ARM_MPU_SH_NON,
                     ARM_MPU_READ_ONLY,
                     ARM_MPU_UNPRIVILEGED,
                     ARM_MPU_EXECUTE_OK),
        #ifdef TFM_PXN_ENABLE
        ARM_MPU_RLAR_PXN((uint32_t)&REGION_NAME(Image$$, VENEER_ALIGN, $$Limit) - 1,
                         ARM_MPU_PRIVILEGE_EXECUTE_OK,
                         0)
        #else
        ARM_MPU_RLAR((uint32_t)&REGION_NAME(Image$$, VENEER_ALIGN, $$Limit) - 1,
                     0)
        #endif
    },
#endif /* CONFIG_TFM_USE_TRUSTZONE */
    /* TFM Core unprivileged code region
     * Region Number 1, Non-shareable, Read-Only, Non-Privileged, Executable,
     * Privilege Executable - if PXN available, Attribute set: 0
     */
    {
        ARM_MPU_RBAR((uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Base),
                     ARM_MPU_SH_NON,
                     ARM_MPU_READ_ONLY,
                     ARM_MPU_UNPRIVILEGED,
                     ARM_MPU_EXECUTE_OK),
        #ifdef TFM_PXN_ENABLE
        ARM_MPU_RLAR_PXN((uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit) - 1,
                         ARM_MPU_PRIVILEGE_EXECUTE_OK,
                         0)
        #else
        ARM_MPU_RLAR((uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit) - 1,
                     0)
        #endif
    },
    /* RO region
     * Region Number 2, Non-shareable, Read-Only, Non-Privileged, Executable,
     * PXN depends on isolation level, Attribute set: 0
     */
    {
        ARM_MPU_RBAR((uint32_t)&REGION_NAME(Image$$, TFM_APP_CODE_START, $$Base),
                     ARM_MPU_SH_NON,
                     ARM_MPU_READ_ONLY,
                     ARM_MPU_UNPRIVILEGED,
                     ARM_MPU_EXECUTE_OK),
        #ifdef TFM_PXN_ENABLE
        ARM_MPU_RLAR_PXN((uint32_t)&REGION_NAME(Image$$, TFM_APP_CODE_END, $$Base) - 1,
            #if TFM_LVL == 1
                         ARM_MPU_PRIVILEGE_EXECUTE_OK,
            #else
                         ARM_MPU_PRIVILEGE_EXECUTE_NEVER,
            #endif
                         0)
        #else
        ARM_MPU_RLAR((uint32_t)&REGION_NAME(Image$$, TFM_APP_CODE_END, $$Base) - 1,
                     0)
        #endif
    },
    /* RW, ZI and stack as one region
     * Region Number 3, Non-shareable, Read-Write, Non-Privileged, Execute Never
     * Attribute set: 1, Privilege Execute Never - if PXN available
     */
    {
        ARM_MPU_RBAR((uint32_t)&REGION_NAME(Image$$, TFM_APP_RW_STACK_START, $$Base),
                     ARM_MPU_SH_NON,
                     ARM_MPU_READ_WRITE,
                     ARM_MPU_UNPRIVILEGED,
                     ARM_MPU_EXECUTE_NEVER),
        #ifdef TFM_PXN_ENABLE
        ARM_MPU_RLAR_PXN((uint32_t)&REGION_NAME(Image$$, TFM_APP_RW_STACK_END, $$Base) - 1,
                         ARM_MPU_PRIVILEGE_EXECUTE_NEVER,
                         1)
        #else
        ARM_MPU_RLAR((uint32_t)&REGION_NAME(Image$$, TFM_APP_RW_STACK_END, $$Base) - 1,
                     1)
        #endif
    },
#ifdef CONFIG_TFM_PARTITION_META
    /* TFM partition metadata pointer region
     * Region Number 4, Non-shareable, Read-Write, Non-Privileged, Execute Never
     * Attribute set: 1, Privilege Execute Never - if PXN available
     */
    {
        ARM_MPU_RBAR((uint32_t)&REGION_NAME(Image$$, TFM_SP_META_PTR, $$ZI$$Base),
                     ARM_MPU_SH_NON,
                     ARM_MPU_READ_WRITE,
                     ARM_MPU_UNPRIVILEGED,
                     ARM_MPU_EXECUTE_NEVER),
        #ifdef TFM_PXN_ENABLE
        ARM_MPU_RLAR_PXN((uint32_t)&REGION_NAME(Image$$, TFM_SP_META_PTR, $$ZI$$Limit) - 1,
                         ARM_MPU_PRIVILEGE_EXECUTE_NEVER,
                         1)
        #else
        ARM_MPU_RLAR((uint32_t)&REGION_NAME(Image$$, TFM_SP_META_PTR, $$ZI$$Limit) - 1,
                     1)
        #endif
    }
#endif
};
    ARM_MPU_Region_t localcfg;
#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */
    /* Set up isolation boundaries between SPE and NSPE */
    sau_and_idau_cfg();
    if (mpc_init_cfg() != TFM_PLAT_ERR_SUCCESS) {
        return TFM_HAL_ERROR_GENERIC;
    }
    ppc_init_cfg();

    /* Set up static isolation boundaries inside SPE */
#ifdef CONFIG_TFM_ENABLE_MEMORY_PROTECT
    int32_t i;

    uint32_t mpu_region_num =
        (MPU ->TYPE & MPU_TYPE_DREGION_Msk) >> MPU_TYPE_DREGION_Pos;

    if (mpu_region_num < ARRAY_SIZE(mpu_region_attributes)) {
        return TFM_HAL_ERROR_GENERIC;
    }

    /* Turn off MPU during configuration */
    if ((MPU->CTRL & MPU_CTRL_ENABLE_Msk)) {
        ARM_MPU_Disable();
    }
    /* Disable all regions */
    for (i = 0; i < mpu_region_num; i++) {
        ARM_MPU_ClrRegion(i);
    }

    /* Configure attribute registers
     * Attr0 : Normal memory, Inner/Outer Cacheable, Write-Trough Read-Allocate
     */
    ARM_MPU_SetMemAttr(0,
                   ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(ARM_MPU_NON_TRANSIENT,
                                                     ARM_MPU_WRITE_THROUGH,
                                                     ARM_MPU_READ_ALLOCATE,
                                                     ARM_MPU_NON_WRITE_ALLOCATE),
                                ARM_MPU_ATTR_MEMORY_(ARM_MPU_NON_TRANSIENT,
                                                     ARM_MPU_WRITE_THROUGH,
                                                     ARM_MPU_READ_ALLOCATE,
                                                     ARM_MPU_NON_WRITE_ALLOCATE)));
    /* Attr1 : Normal memory, Inner/Outer Cacheable, Write-Back R-W Allocate */
    ARM_MPU_SetMemAttr(1,
                    ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(ARM_MPU_NON_TRANSIENT,
                                                     ARM_MPU_WRITE_BACK,
                                                     ARM_MPU_READ_ALLOCATE,
                                                     ARM_MPU_WRITE_ALLOCATE),
                                ARM_MPU_ATTR_MEMORY_(ARM_MPU_NON_TRANSIENT,
                                                     ARM_MPU_WRITE_BACK,
                                                     ARM_MPU_READ_ALLOCATE,
                                                     ARM_MPU_WRITE_ALLOCATE)));
    /* Attr2 : Device memory, nGnRE */
    ARM_MPU_SetMemAttr(2,
                       ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE,
                                    ARM_MPU_ATTR_DEVICE_nGnRE));

    /* Configure regions */
    for (i = 0; i < ARRAY_SIZE(mpu_region_attributes); i++) {
        localcfg.RBAR = mpu_region_attributes[i].RBAR;
        localcfg.RLAR = mpu_region_attributes[i].RLAR;
        ARM_MPU_SetRegion(i, localcfg.RBAR, localcfg.RLAR);
    }
    n_configured_regions = i;

    /* Enable MPU with the above configurations. Allow default memory map for
     * privileged software and enable MPU during HardFault and NMI handlers.
     */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk);
#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */

    *p_spm_boundary = (uintptr_t)PROT_BOUNDARY_VAL;

    return TFM_HAL_SUCCESS;
}

/*
 * Implementation of tfm_hal_bind_boundary():
 *
 * The API encodes some attributes into a handle and returns it to SPM.
 * The attributes include isolation boundaries, privilege, and MMIO information.
 * When scheduler switches running partitions, SPM compares the handle between
 * partitions to know if boundary update is necessary. If update is required,
 * SPM passes the handle to platform to do platform settings and update
 * isolation boundaries.
 */
enum tfm_hal_status_t tfm_hal_bind_boundary(
                                    const struct partition_load_info_t *p_ldinf,
                                    uintptr_t *p_boundary)
{
    uint32_t i, j;
    bool privileged;
    bool ns_agent;
    uint32_t partition_attrs = 0;
    const struct asset_desc_t *p_asset;
    struct platform_data_t *plat_data_ptr;
    const uintptr_t* mmio_list;
    size_t mmio_list_length;

#if TFM_LVL == 2
    ARM_MPU_Region_t local_mpu_region;
    uint32_t mpu_region_num;
#endif
    if (!p_ldinf || !p_boundary) {
        return TFM_HAL_ERROR_GENERIC;
    }

#if TFM_LVL == 1
    privileged = true;
#else
    privileged = IS_PSA_ROT(p_ldinf);
#endif

    ns_agent = IS_NS_AGENT(p_ldinf);
    p_asset = LOAD_INFO_ASSET(p_ldinf);

    get_partition_named_mmio_list(&mmio_list, &mmio_list_length);

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
        for (j = 0; j < mmio_list_length; j++) {
            if (p_asset[i].dev.dev_ref == mmio_list[j]) {
                break;
            }
        }

        if (j == mmio_list_length) {
            /* The MMIO asset is not in the allowed list of platform. */
            return TFM_HAL_ERROR_GENERIC;
        }
        /* Assume PPC & MPC settings are required even under level 1 */
        plat_data_ptr = REFERENCE_TO_PTR(p_asset[i].dev.dev_ref,
                                         struct platform_data_t *);

        if (plat_data_ptr->periph_ppc_bank != PPC_SP_DO_NOT_CONFIGURE) {
            ppc_configure_to_secure(plat_data_ptr->periph_ppc_bank,
                                    plat_data_ptr->periph_ppc_mask);
            if (privileged) {
                ppc_clr_secure_unpriv(plat_data_ptr->periph_ppc_bank,
                                      plat_data_ptr->periph_ppc_mask);
            } else {
                ppc_en_secure_unpriv(plat_data_ptr->periph_ppc_bank,
                                     plat_data_ptr->periph_ppc_mask);
            }
        }
#if TFM_LVL == 2
        /*
         * Static boundaries are set. Set up MPU region for MMIO.
         * Setup regions for unprivileged assets only.
         */
        if (!privileged) {
            //Turn off MPU during configuration
            if ((MPU->CTRL & MPU_CTRL_ENABLE_Msk)) {
                ARM_MPU_Disable();
            }
            /* Assemble region base and limit address register contents. */
            local_mpu_region.RBAR = ARM_MPU_RBAR(plat_data_ptr->periph_start,
                                                 ARM_MPU_SH_NON,
                                                 ARM_MPU_READ_WRITE,
                                                 ARM_MPU_UNPRIVILEGED,
                                                 ARM_MPU_EXECUTE_NEVER);
            /* Attr2 contains required attribute set for device regions */
            #ifdef TFM_PXN_ENABLE
            local_mpu_region.RLAR = ARM_MPU_RLAR_PXN(plat_data_ptr->periph_limit,
                                                     ARM_MPU_PRIVILEGE_EXECUTE_NEVER,
                                                     2);
            #else
            local_mpu_region.RLAR = ARM_MPU_RLAR(plat_data_ptr->periph_limit,
                                                 2);
            #endif
            n_configured_regions++;

            mpu_region_num =
                (MPU->TYPE & MPU_TYPE_DREGION_Msk) >> MPU_TYPE_DREGION_Pos;

            /* There is a limited number of availale MPU regions in v8M */
            if (mpu_region_num < n_configured_regions) {
                return TFM_HAL_ERROR_GENERIC;
            }

            /* Configure device mpu region */
            ARM_MPU_SetRegion(n_configured_regions,
                              local_mpu_region.RBAR,
                              local_mpu_region.RLAR);

            /* Enable MPU with the new region added */
            ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk);
        }
#endif
    }

    partition_attrs = ((uint32_t)privileged << HANDLE_ATTR_PRIV_POS) &
                        HANDLE_ATTR_PRIV_MASK;
    partition_attrs |= ((uint32_t)ns_agent << HANDLE_ATTR_NS_POS) &
                        HANDLE_ATTR_NS_MASK;
    *p_boundary = (uintptr_t)partition_attrs;

    return TFM_HAL_SUCCESS;
}

enum tfm_hal_status_t tfm_hal_activate_boundary(
                             const struct partition_load_info_t *p_ldinf,
                             uintptr_t boundary)
{
    CONTROL_Type ctrl;
    bool privileged = !!((uint32_t)boundary & HANDLE_ATTR_PRIV_MASK);

    /* Privileged level is required to be set always */
    ctrl.w = __get_CONTROL();
    ctrl.b.nPRIV = privileged ? 0 : 1;
    __set_CONTROL(ctrl.w);

    return TFM_HAL_SUCCESS;
}

enum tfm_hal_status_t tfm_hal_memory_check(uintptr_t boundary, uintptr_t base,
                                           size_t size, uint32_t access_type)
{
    int flags = 0;

    /* If size is zero, this indicates an empty buffer and base is ignored */
    if (size == 0) {
        return TFM_HAL_SUCCESS;
    }

    if (!base) {
        return TFM_HAL_ERROR_INVALID_INPUT;
    }

    if ((access_type & TFM_HAL_ACCESS_READWRITE) == TFM_HAL_ACCESS_READWRITE) {
        flags |= CMSE_MPU_READWRITE;
    } else if (access_type & TFM_HAL_ACCESS_READABLE) {
        flags |= CMSE_MPU_READ;
    } else {
        return TFM_HAL_ERROR_INVALID_INPUT;
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
        return TFM_HAL_SUCCESS;
    } else {
        return TFM_HAL_ERROR_MEM_FAULT;
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
