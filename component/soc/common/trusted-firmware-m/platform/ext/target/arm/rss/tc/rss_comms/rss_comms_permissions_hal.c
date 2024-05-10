/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rss_comms_permissions_hal.h"

#include "psa_manifest/sid.h"
#include "tfm_hal_platform.h"

#ifdef TFM_PARTITION_INITIAL_ATTESTATION
#include "tfm_attest_defs.h"
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */
#ifdef TFM_PARTITION_MEASURED_BOOT
#include "measured_boot_defs.h"
#endif /* TFM_PARTITION_MEASURED_BOOT */
#ifdef TFM_PARTITION_DELEGATED_ATTESTATION
#include "tfm_delegated_attest_defs.h"
#endif /* TFM_PARTITION_DELEGATED_ATTESTATION */
#ifdef TFM_PARTITION_CRYPTO
#include "tfm_crypto_defs.h"
#endif /*TFM_PARTITION_CRYPTO */
#ifdef TFM_PARTITION_PLATFORM
#include "tfm_platform_api.h"
#endif /* TFM_PARTITION_PLATFORM */

#define INVALID_REGION_COUNTER_MAX  128
#define INVALID_SERVICE_COUNTER_MAX 64

static uint32_t invalid_region_counter = 0;
static uint32_t invalid_service_counter = 0;

/* Check if the interface is  getting a lot of invalid requests, and shutdown
 * the system if it exceeds the threshold. This is intended to make fuzzing the
 * interface difficult.
 */
static void counter_check(void) {
    if (invalid_region_counter > INVALID_REGION_COUNTER_MAX) {
#ifdef CONFIG_TFM_HALT_ON_CORE_PANIC
        tfm_hal_system_halt();
#else
        tfm_hal_system_reset();
#endif /* CONFIG_TFM_HALT_ON_CORE_PANIC */
    }

    if (invalid_service_counter > INVALID_SERVICE_COUNTER_MAX) {
#ifdef CONFIG_TFM_HALT_ON_CORE_PANIC
        tfm_hal_system_halt();
#else
        tfm_hal_system_reset();
#endif /* CONFIG_TFM_HALT_ON_CORE_PANIC */
    }

    return;
}

/* This interface is accessed only from root world, so we don't care about the
 * owners of host memory. However, we should still be somewhat discerning about
 * where data is coming from or going to.
 */
enum tfm_plat_err_t comms_permissions_memory_check(uint64_t host_ptr,
                                                   uint32_t size,
                                                   bool is_write)
{
    /* Is fully within the Secure ROM and is a read */
    if (host_ptr >= 0x0 && host_ptr + size < 0x80000 && !is_write) {
        return TFM_PLAT_ERR_SUCCESS;
    }

    if (host_ptr >= 0x4000000 && host_ptr + size < 0x4080000) {
        return TFM_PLAT_ERR_SUCCESS;
    }

    /* Is fully within the Non-secure ROM and is a read */
    if (host_ptr >= 0x5000000 && host_ptr + size < 0x5080000 && !is_write) {
        return TFM_PLAT_ERR_SUCCESS;
    }

    /* Is fully within the Non-secure RAM */
    if (host_ptr >= 0x6000000 && host_ptr + size < 0x6080000) {
        return TFM_PLAT_ERR_SUCCESS;
    }

    invalid_region_counter++;
    counter_check();

    return TFM_PLAT_ERR_UNSUPPORTED;
}

enum tfm_plat_err_t comms_permissions_service_check(psa_handle_t handle,
                                                    const psa_invec *in_vec,
                                                    size_t in_len,
                                                    int32_t type)
{
    uint32_t function_id = 0;

    switch(handle) {
#ifdef TFM_PARTITION_INITIAL_ATTESTATION
    case TFM_ATTESTATION_SERVICE_HANDLE:
        switch(type) {
        case TFM_ATTEST_GET_TOKEN:
        case TFM_ATTEST_GET_TOKEN_SIZE:
            return TFM_PLAT_ERR_SUCCESS;
        default:
            goto out_err;
        }
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */
#ifdef TFM_PARTITION_DELEGATED_ATTESTATION
    case TFM_DELEGATED_ATTESTATION_HANDLE:
        switch(type) {
        case DELEGATED_ATTEST_GET_DELEGATED_KEY:
        case DELEGATED_ATTEST_GET_PLATFORM_TOKEN:
            return TFM_PLAT_ERR_SUCCESS;
        default:
            goto out_err;
        }
#endif /* TFM_PARTITION_DELEGATED_ATTESTATION */
#ifdef TFM_PARTITION_MEASURED_BOOT
    case TFM_MEASURED_BOOT_HANDLE:
        switch(type) {
        case TFM_MEASURED_BOOT_EXTEND:
        case TFM_MEASURED_BOOT_READ:
            return TFM_PLAT_ERR_SUCCESS;
        default:
            goto out_err;
        }
#endif /* TFM_PARTITION_MEASURED_BOOT */
#ifdef TFM_PARTITION_CRYPTO
    case TFM_CRYPTO_HANDLE:
        if (in_len >= 1) {
            function_id = ((struct tfm_crypto_pack_iovec *)in_vec[0].base)->function_id;
            switch(function_id) {
            case (TFM_CRYPTO_EXPORT_PUBLIC_KEY_SID):
                return TFM_PLAT_ERR_SUCCESS;
            default:
                goto out_err;
            }
        }
        goto out_err;
#endif /* TFM_PARTITION_CRYPTO */
#ifdef TFM_PARTITION_PLATFORM
    case TFM_PLATFORM_SERVICE_HANDLE:
        switch(type) {
        case TFM_PLATFORM_API_ID_NV_READ:
        case TFM_PLATFORM_API_ID_NV_INCREMENT:
            return TFM_PLAT_ERR_SUCCESS;
        default:
            goto out_err;
        }
#endif /* TFM_PARTITION_PLATFORM */
    default:
        goto out_err;
    }

out_err:
    invalid_service_counter++;
    counter_check();

    return TFM_PLAT_ERR_UNSUPPORTED;
}
