/*
 * Copyright (c) 2020-2022, Arm Limited. All rights reserved.
 * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_api.h"
#include "tfm_hal_defs.h"
#include "tfm_multi_core.h"
#include "tfm_hal_isolation.h"
#include "mpu_config.h"
#include "mmio_defs.h"

#define PROT_BOUNDARY_VAL \
    ((1U << HANDLE_ATTR_PRIV_POS) & HANDLE_ATTR_PRIV_MASK)

#ifdef CONFIG_TFM_ENABLE_MEMORY_PROTECT

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

static void configure_mpu(uint32_t rnr, uint32_t base, uint32_t limit,
                          uint32_t is_xn_exec, uint32_t ap_permissions)
{
    uint32_t size; /* region size */
    uint32_t rasr; /* region attribute and size register */
    uint32_t rbar; /* region base address register */

    size = get_rbar_size_field(limit - base);

    rasr = ARM_MPU_RASR(is_xn_exec, ap_permissions, TEX, NOT_SHAREABLE,
            NOT_CACHEABLE, NOT_BUFFERABLE, SUB_REGION_DISABLE, size);
    rbar = base & MPU_RBAR_ADDR_Msk;

    ARM_MPU_SetRegionEx(rnr, rbar, rasr);
}

#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */

enum tfm_hal_status_t tfm_hal_set_up_static_boundaries(
                                            uintptr_t *p_spm_boundary)
{
#ifdef CONFIG_TFM_ENABLE_MEMORY_PROTECT
    uint32_t rnr = TFM_ISOLATION_REGION_START_NUMBER; /* current region number */
    uint32_t base; /* start address */
    uint32_t limit; /* end address */

    ARM_MPU_Disable();

    /* TFM Core unprivileged code region */
    base = (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Base);
    limit = (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit);

    configure_mpu(rnr++, base, limit, XN_EXEC_OK, AP_RO_PRIV_UNPRIV);

    /* RO region */
    base = (uint32_t)&REGION_NAME(Image$$, TFM_APP_CODE_START, $$Base);
    limit = (uint32_t)&REGION_NAME(Image$$, TFM_APP_CODE_END, $$Base);

    configure_mpu(rnr++, base, limit, XN_EXEC_OK, AP_RO_PRIV_UNPRIV);

    /* RW, ZI and stack as one region */
    base = (uint32_t)&REGION_NAME(Image$$, TFM_APP_RW_STACK_START, $$Base);
    limit = (uint32_t)&REGION_NAME(Image$$, TFM_APP_RW_STACK_END, $$Base);

    configure_mpu(rnr++, base, limit, XN_EXEC_NOT_OK, AP_RW_PRIV_UNPRIV);

#ifdef CONFIG_TFM_PARTITION_META
    /* TFM partition metadata pointer region */
    base = (uint32_t)&REGION_NAME(Image$$, TFM_SP_META_PTR, $$ZI$$Base);
    limit = (uint32_t)&REGION_NAME(Image$$, TFM_SP_META_PTR, $$ZI$$Limit);

    configure_mpu(rnr++, base, limit, XN_EXEC_NOT_OK, AP_RW_PRIV_UNPRIV);
#endif

    arm_mpu_enable();

#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */

    *p_spm_boundary = (uintptr_t)PROT_BOUNDARY_VAL;

    return TFM_HAL_SUCCESS;
}

enum tfm_hal_status_t tfm_hal_memory_check(uintptr_t boundary,
                                           uintptr_t base,
                                           size_t size,
                                           uint32_t access_type)
{
    enum tfm_status_e status;
    uint32_t flags = 0;

    if ((access_type & TFM_HAL_ACCESS_READWRITE) == TFM_HAL_ACCESS_READWRITE) {
        flags |= MEM_CHECK_MPU_READWRITE;
    } else if (access_type & TFM_HAL_ACCESS_READABLE) {
        flags |= MEM_CHECK_MPU_READ;
    } else {
        return TFM_HAL_ERROR_INVALID_INPUT;
    }

    if (!((uint32_t)boundary & HANDLE_ATTR_PRIV_MASK)) {
        flags |= MEM_CHECK_MPU_UNPRIV;
    }

    if ((uint32_t)boundary & HANDLE_ATTR_NS_MASK) {
        flags |= MEM_CHECK_NONSECURE;
    }

    status = tfm_has_access_to_region((const void *)base, size, flags);
    if (status != TFM_SUCCESS) {
         return TFM_HAL_ERROR_MEM_FAULT;
    }

    return TFM_HAL_SUCCESS;
}

/*
 * Implementation of tfm_hal_bind_boundary() on Corstone1000:
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
 * This is a reference implementation on Corstone1000, and may have some limitations.
 * 1. The maximum number of allowed MMIO regions is 5.
 * 2. Highest 8 bits are for index. It supports 256 unique handles at most.
 *
 */

enum tfm_hal_status_t tfm_hal_bind_boundary(
                                    const struct partition_load_info_t *p_ldinf,
                                    uintptr_t *p_boundary)
{
    bool privileged;
    bool ns_agent;
    uint32_t partition_attrs = 0;

    if (!p_ldinf || !p_boundary) {
        return TFM_HAL_ERROR_GENERIC;
    }

#if TFM_LVL == 1
    privileged = true;
#else
    privileged = IS_PSA_ROT(p_ldinf);
#endif

    ns_agent = IS_NS_AGENT(p_ldinf);
    partition_attrs = ((uint32_t)privileged << HANDLE_ATTR_PRIV_POS) &
                        HANDLE_ATTR_PRIV_MASK;
    partition_attrs |= ((uint32_t)ns_agent << HANDLE_ATTR_NS_POS) &
                        HANDLE_ATTR_NS_MASK;
    *p_boundary = (uintptr_t)partition_attrs;

    /* NOTE: Need to add validation of numbered MMIO if platform requires. */
    /* Platform does not have a need for MMIO yet. */

    return TFM_HAL_SUCCESS;
}

enum tfm_hal_status_t tfm_hal_activate_boundary(
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

    return TFM_HAL_SUCCESS;
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
