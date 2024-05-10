/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_BUILTIN_KEY_LOADER_IDS_H__
#define __PLATFORM_BUILTIN_KEY_LOADER_IDS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define TFM_BUILTIN_MAX_KEY_LEN 96

enum psa_drv_slot_number_t {
    TFM_BUILTIN_KEY_SLOT_HUK = 0,
    TFM_BUILTIN_KEY_SLOT_IAK,
#ifdef TFM_PARTITION_DELEGATED_ATTESTATION
    TFM_BUILTIN_KEY_SLOT_DAK_SEED,
#endif /* TFM_PARTITION_DELEGATED_ATTESTATION */
    TFM_BUILTIN_KEY_SLOT_HOST_S_ROTPK,
    TFM_BUILTIN_KEY_SLOT_HOST_NS_ROTPK,
    TFM_BUILTIN_KEY_SLOT_HOST_CCA_ROTPK,
    TFM_BUILTIN_KEY_SLOT_MAX,
};

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_BUILTIN_KEY_LOADER_IDS_H__ */
