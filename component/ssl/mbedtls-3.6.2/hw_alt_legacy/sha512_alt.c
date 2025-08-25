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
#include "ameba_soc.h"
#include "mbedtls/sha512.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "mbedtls/platform.h"

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
    int ret;
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

    SHA_MODE = is384 ? SHA2_384 : SHA2_512;

    IPC_SEMTake(IPC_SEM_CRYPTO, 0xffffffff);
    ret = rtl_crypto_sha2_init(SHA_MODE,ctx);
    IPC_SEMFree(IPC_SEM_CRYPTO);

    return ret;
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
    int ret;
    IPC_SEMTake(IPC_SEM_CRYPTO, 0xffffffff);
    ret =  rtl_crypto_sha2_update((uint8_t *)input, ilen,ctx);
    IPC_SEMFree(IPC_SEM_CRYPTO);

    return ret;
}

/*
 * SHA-512 final digest
 */
int mbedtls_sha512_finish(mbedtls_sha512_context *ctx,
                          unsigned char *output)
{
    int ret;
    IPC_SEMTake(IPC_SEM_CRYPTO, 0xffffffff);
    ret =  rtl_crypto_sha2_final(output,ctx);
    IPC_SEMFree(IPC_SEM_CRYPTO);

    return ret;
}

/*
 * output = SHA-512( input buffer )
 */
int mbedtls_sha512(const unsigned char *input,
                   size_t ilen,
                   unsigned char *output,
                   int is384)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_sha512_context ctx;

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

    mbedtls_sha512_init(&ctx);

    if ((ret = mbedtls_sha512_starts(&ctx, is384)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_sha512_update(&ctx, input, ilen)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_sha512_finish(&ctx, output)) != 0) {
        goto exit;
    }

exit:
    mbedtls_sha512_free(&ctx);

    return ret;
}

#endif /* !MBEDTLS_SHA512_ALT */
