/**
 * *****************************************************************************
 * @file    sha256_alt.c
 * @author  
 * @version V1.0.0
 * @date    2023-09-22
 * @brief   Mbedtls SHA-256 Hardware impliment for lalu secure engine
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

#if defined(MBEDTLS_SHA256_ALT)

#include <string.h>
#include "ameba_soc.h"
#include "mbedtls/sha256.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "mbedtls/platform.h"


#define SHA256_BLOCK_SIZE 64

void mbedtls_sha256_init(mbedtls_sha256_context *ctx)
{
    _memset(ctx, 0, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_free(mbedtls_sha256_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    mbedtls_platform_zeroize(ctx, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_clone(mbedtls_sha256_context *dst,
                          const mbedtls_sha256_context *src)
{
    *dst = *src;
}

/*
 * SHA-256 context setup
 */
int mbedtls_sha256_starts(mbedtls_sha256_context *ctx, int is224)
{
    u8 SHA_MODE = 0;
    int ret = 0;
#if defined(MBEDTLS_SHA224_C) && defined(MBEDTLS_SHA256_C)
    if (is224 != 0 && is224 != 1) {
        return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
    }
#elif defined(MBEDTLS_SHA256_C)
    if (is224 != 0) {
        return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
    }
#else /* defined MBEDTLS_SHA224_C only */
    if (is224 == 0) {
        return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
    }
#endif
    SHA_MODE = is224 ? SHA2_224 : SHA2_256;

    IPC_SEMTake(IPC_SEM_CRYPTO, 0xffffffff);
    ret = rtl_crypto_sha2_init(SHA_MODE,ctx);
    IPC_SEMFree(IPC_SEM_CRYPTO);

    return ret;
}


int mbedtls_internal_sha256_process(mbedtls_sha256_context *ctx,
                                    const unsigned char data[SHA256_BLOCK_SIZE])
{
    return mbedtls_sha256_update(ctx, data, SHA256_BLOCK_SIZE);
}


/*
 * SHA-256 process buffer
 */
int mbedtls_sha256_update(mbedtls_sha256_context *ctx,
                          const unsigned char *input,
                          size_t ilen)
{
    int ret;
    IPC_SEMTake(IPC_SEM_CRYPTO, 0xffffffff);
    ret = rtl_crypto_sha2_update((uint8_t *)input, ilen,ctx);
    IPC_SEMFree(IPC_SEM_CRYPTO);

    return ret;
}

/*
 * SHA-256 final digest
 */
int mbedtls_sha256_finish(mbedtls_sha256_context *ctx,
                          unsigned char *output)
{
    int ret;
    IPC_SEMTake(IPC_SEM_CRYPTO, 0xffffffff);
    ret = rtl_crypto_sha2_final(output,ctx);
    IPC_SEMFree(IPC_SEM_CRYPTO);

    return ret;
}

/*
 * output = SHA-256( input buffer )
 */
int mbedtls_sha256(const unsigned char *input,
                   size_t ilen,
                   unsigned char *output,
                   int is224)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_sha256_context ctx;

#if defined(MBEDTLS_SHA224_C) && defined(MBEDTLS_SHA256_C)
    if (is224 != 0 && is224 != 1) {
        return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
    }
#elif defined(MBEDTLS_SHA256_C)
    if (is224 != 0) {
        return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
    }
#else /* defined MBEDTLS_SHA224_C only */
    if (is224 == 0) {
        return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
    }
#endif

    mbedtls_sha256_init(&ctx);

    if ((ret = mbedtls_sha256_starts(&ctx, is224)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_sha256_update(&ctx, input, ilen)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_sha256_finish(&ctx, output)) != 0) {
        goto exit;
    }

exit:
    mbedtls_sha256_free(&ctx);

    return ret;
}

#endif /* !MBEDTLS_SHA256_ALT */
