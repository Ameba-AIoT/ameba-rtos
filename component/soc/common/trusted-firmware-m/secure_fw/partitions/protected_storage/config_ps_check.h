/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_PARTITION_PS_H__
#define __CONFIG_PARTITION_PS_H__

#include "config_tfm.h"

/* Check invalid configs. */
#if PS_ROLLBACK_PROTECTION && PLATFORM_NV_COUNTER_MODULE_DISABLED
#error "Invalid config: PS_ROLLBACK_PROTECTION and PLATFORM_NV_COUNTER_MODULE_DISABLED!"
#endif

#if PS_ROLLBACK_PROTECTION && (!defined(PS_ENCRYPTION))
#error "Invalid config: PS_ROLLBACK_PROTECTION and NOT PS_ENCRYPTION!"
#endif

#if (!PS_ROLLBACK_PROTECTION) && defined(PS_ENCRYPTION) && \
    (defined(PS_CRYPTO_AEAD_ALG_GCM) || defined(PS_CRYPTO_AEAD_ALG_CCM))
#error "Invalid config: NOT PS_ROLLBACK_PROTECTION and PS_ENCRYPTION and PSA_ALG_GCM or PSA_ALG_CCM!"
#endif

/*
 * ITS_VALIDATE_METADATA_FROM_FLASH shall be enabled when PS_VALIDATE_METADATA_FROM_FLASH is
 * enabled
 */
#if PS_VALIDATE_METADATA_FROM_FLASH && (!ITS_VALIDATE_METADATA_FROM_FLASH)
#error "Invalid config: ITS_VALIDATE_METADATA_FROM_FLASH shall be enabled when PS_VALIDATE_METADATA_FROM_FLASH is enabled"
#endif

#endif /* __CONFIG_PARTITION_PS_H__ */
