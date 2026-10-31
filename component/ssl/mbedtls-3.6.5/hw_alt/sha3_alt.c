/**
 * *****************************************************************************
 * @file    sha3_alt.c
 * @author
 * @version V1.0.0
 * @date    2026-10-22
 * @brief   Mbedtls SHA-3 hardware implementation for the LALU SHA3 engine
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#include "common.h"

#if defined(MBEDTLS_SHA3_ALT)

#include <string.h>

#include "mbedtls/sha3.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "mbedtls/platform.h"

#include "ameba_soc.h"

/* The HAL and mbedtls error domains collide numerically, so the HAL code is mapped by meaning:
   a bad argument is the caller's mistake, anything else is a fault of the accelerator. */
static int sha3_alt_map_error(int hw_ret)
{
    switch (hw_ret) {
        case RTK_SUCCESS:
            return 0;
        case _ERRNO_CRYPTO_NULL_POINTER:
        case _ERRNO_CRYPTO_MODE_ERR:
        case _ERRNO_CRYPTO_MESSAGE_LEN_ERR:
            return MBEDTLS_ERR_SHA3_BAD_INPUT_DATA;
        default:
            return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
}

void mbedtls_sha3_init(mbedtls_sha3_context *ctx)
{
    _memset(ctx, 0, sizeof(mbedtls_sha3_context));
}

void mbedtls_sha3_free(mbedtls_sha3_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    mbedtls_platform_zeroize(ctx, sizeof(mbedtls_sha3_context));
}

void mbedtls_sha3_clone(mbedtls_sha3_context *dst,
                        const mbedtls_sha3_context *src)
{
    *dst = *src;
}

/*
 * SHA-3 context setup
 */
int mbedtls_sha3_starts(mbedtls_sha3_context *ctx, mbedtls_sha3_id id)
{
    u32 mode;

    switch (id) {
        case MBEDTLS_SHA3_224:
            mode = SHA3_224;
            break;
        case MBEDTLS_SHA3_256:
            mode = SHA3_256;
            break;
        case MBEDTLS_SHA3_384:
            mode = SHA3_384;
            break;
        case MBEDTLS_SHA3_512:
            mode = SHA3_512;
            break;
        default:
            return MBEDTLS_ERR_SHA3_BAD_INPUT_DATA;
    }

    return sha3_alt_map_error(crypto_sha3_init(ctx, mode));
}

/*
 * SHA-3 process buffer
 */
int mbedtls_sha3_update(mbedtls_sha3_context *ctx,
                        const uint8_t *input,
                        size_t ilen)
{
    if (ilen == 0) {
        return 0;
    }

    return sha3_alt_map_error(crypto_sha3_update(ctx, input, ilen));
}

/*
 * SHA-3 final digest. Like the software one, the function clears the context afterwards.
 */
int mbedtls_sha3_finish(mbedtls_sha3_context *ctx,
                        uint8_t *output, size_t olen)
{
    int ret;

    if (olen < ctx->digest_len) {
        ret = MBEDTLS_ERR_SHA3_BAD_INPUT_DATA;
        goto exit;
    }

    /* the driver writes exactly digest_len bytes, a longer output buffer is left untouched beyond that */
    ret = sha3_alt_map_error(crypto_sha3_final(ctx, output));

exit:
    mbedtls_sha3_free(ctx);
    return ret;
}

#endif /* MBEDTLS_SHA3_ALT */
