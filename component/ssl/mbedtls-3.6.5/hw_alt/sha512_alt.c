/**
 * *****************************************************************************
 * @file    sha512_alt.c
 * @author  
 * @version V1.0.0
 * @date    2023-09-22
 * @brief  Mbedtls SHA-512 Hardware impliment for lalu secure engine
 * *****************************************************************************
 * @attention
 * 
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 * 
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#include "common.h"

#if defined(MBEDTLS_SHA512_ALT)

#include <string.h>

#include "mbedtls/sha512.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "mbedtls/platform.h"

#include "ameba_soc.h"
#include "mbedtls_alt_helper.h"

#define SHA512_BLOCK_SIZE 128

void mbedtls_sha512_init(mbedtls_sha512_context *ctx)
{
    _memset(ctx, 0, sizeof(mbedtls_sha512_context));
}

void mbedtls_sha512_free(mbedtls_sha512_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    mbedtls_platform_zeroize(ctx, sizeof(mbedtls_sha512_context));
}

void mbedtls_sha512_clone(mbedtls_sha512_context *dst,
                          const mbedtls_sha512_context *src)
{
    *dst = *src;
}

/*
 * SHA-512 context setup
 */
int mbedtls_sha512_starts(mbedtls_sha512_context *ctx, int is384)
{
    u8 SHA_MODE = 0;
#if defined(MBEDTLS_SHA384_C) && defined(MBEDTLS_SHA512_C)
    if (is384 != 0 && is384 != 1) {
        return MBEDTLS_ERR_SHA512_BAD_INPUT_DATA;
    }
#elif defined(MBEDTLS_SHA512_C)
    if (is384 != 0) {
        return MBEDTLS_ERR_SHA512_BAD_INPUT_DATA;
    }
#else /* defined MBEDTLS_SHA384_C only */
    if (is384 == 0) {
        return MBEDTLS_ERR_SHA512_BAD_INPUT_DATA;
    }
#endif

    SHA_MODE = is384 ? SHA_384 : SHA_512;
    return map_hw_to_mbedtls_error(crypto_sha2_init(ctx, SHA_MODE), MBEDTLS_ALT_ALGO_SHA512);
}

int mbedtls_internal_sha512_process(mbedtls_sha512_context *ctx,
                                    const unsigned char data[SHA512_BLOCK_SIZE])
{
    return mbedtls_sha512_update(ctx, data, SHA512_BLOCK_SIZE);
}

/*
 * SHA-512 process buffer
 */
int mbedtls_sha512_update(mbedtls_sha512_context *ctx,
                          const unsigned char *input,
                          size_t ilen)
{
    if (ilen == 0) {
        return 0;
    }
    return map_hw_to_mbedtls_error(crypto_sha2_update(ctx, input, NULL, ilen), MBEDTLS_ALT_ALGO_SHA512);
}

/*
 * SHA-512 final digest
 */
int mbedtls_sha512_finish(mbedtls_sha512_context *ctx,
                          unsigned char *output)
{
    return map_hw_to_mbedtls_error(crypto_sha2_final(ctx, output), MBEDTLS_ALT_ALGO_SHA512);
}

#endif /* !MBEDTLS_SHA512_ALT */
