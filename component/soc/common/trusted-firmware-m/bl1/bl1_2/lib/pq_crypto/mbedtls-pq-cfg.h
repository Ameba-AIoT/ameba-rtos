/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __BL1_PQ_MBEDTLS_CFG__
#define __BL1_PQ_MBEDTLS_CFG__

#define MBEDTLS_LMS_C
#define MBEDTLS_LMOTS_C

#define MBEDTLS_MD_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA224_C
#define MBEDTLS_PSA_CRYPTO_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_AES_C
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_CIPHER_C

#endif /* __BL1_PQ_MBEDTLS_CFG__ */
