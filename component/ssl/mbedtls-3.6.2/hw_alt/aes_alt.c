/**
 * *****************************************************************************
 * @file    aes_alt.c
 * @author  
 * @version V1.0.0
 * @date    2023-09-22
 * @brief   Mbedtls AES Hardware impliment for lalu secure engine
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

#if defined(MBEDTLS_AES_ALT)

#include <string.h>

#include "mbedtls/aes.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"

#include "ameba_soc.h"

/* Parameter validation macros based on platform_util.h */
#define MBEDTLS_INTERNAL_VALIDATE_RET(cond, ret)  do { } while (0)
#define MBEDTLS_INTERNAL_VALIDATE(cond)           do { } while (0)

#define AES_VALIDATE_RET( cond )    \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_AES_BAD_INPUT_DATA )
#define AES_VALIDATE( cond )        \
    MBEDTLS_INTERNAL_VALIDATE( cond )

void mbedtls_aes_init( mbedtls_aes_context *ctx )
{
    AES_VALIDATE( ctx != NULL );

    _memset( ctx, 0, sizeof( mbedtls_aes_context ) );
}

void mbedtls_aes_free( mbedtls_aes_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_platform_zeroize( ctx, sizeof( mbedtls_aes_context ) );
}

#if defined(MBEDTLS_CIPHER_MODE_XTS)
void mbedtls_aes_xts_init( mbedtls_aes_xts_context *ctx )
{
    AES_VALIDATE( ctx != NULL );

    mbedtls_aes_init( &ctx->crypt );
    mbedtls_aes_init( &ctx->tweak );
}

void mbedtls_aes_xts_free( mbedtls_aes_xts_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_aes_free( &ctx->crypt );
    mbedtls_aes_free( &ctx->tweak );
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */

/*
 * AES key schedule (encryption)
 */
int mbedtls_aes_setkey_enc( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( key != NULL );

    switch( keybits )
    {
        case 128: 
        case 192: 
        case 256: break;
        default : return( MBEDTLS_ERR_AES_INVALID_KEY_LENGTH );
    }

    ctx->key_len_bits = keybits;
    memcpy( ctx->key_val, key, keybits / 8 );

    return( 0 );
}

/*
 * AES key schedule (decryption)
 */
int mbedtls_aes_setkey_dec( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    int ret;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( key != NULL );

    /* Also checks keybits */
    if( ( ret = mbedtls_aes_setkey_enc( ctx, key, keybits ) ) != 0 )
        goto exit;

exit:
    return( ret );
}

#if defined(MBEDTLS_CIPHER_MODE_XTS)
static int mbedtls_aes_xts_decode_keys( const unsigned char *key,
                                        unsigned int keybits,
                                        const unsigned char **key1,
                                        unsigned int *key1bits,
                                        const unsigned char **key2,
                                        unsigned int *key2bits )
{
    const unsigned int half_keybits = keybits / 2;
    const unsigned int half_keybytes = half_keybits / 8;

    switch( keybits )
    {
        case 256: break;
        case 512: break;
        default : return( MBEDTLS_ERR_AES_INVALID_KEY_LENGTH );
    }

    *key1bits = half_keybits;
    *key2bits = half_keybits;
    *key1 = &key[0];
    *key2 = &key[half_keybytes];

    return 0;
}

int mbedtls_aes_xts_setkey_enc( mbedtls_aes_xts_context *ctx,
                                const unsigned char *key,
                                unsigned int keybits)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    const unsigned char *key1, *key2;
    unsigned int key1bits, key2bits;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( key != NULL );

    ret = mbedtls_aes_xts_decode_keys( key, keybits, &key1, &key1bits,
                                       &key2, &key2bits );
    if( ret != 0 )
        return( ret );

    /* Set the tweak key. Always set tweak key for the encryption mode. */
    ret = mbedtls_aes_setkey_enc( &ctx->tweak, key2, key2bits );
    if( ret != 0 )
        return( ret );

    /* Set crypt key for encryption. */
    return mbedtls_aes_setkey_enc( &ctx->crypt, key1, key1bits );
}

int mbedtls_aes_xts_setkey_dec( mbedtls_aes_xts_context *ctx,
                                const unsigned char *key,
                                unsigned int keybits)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    const unsigned char *key1, *key2;
    unsigned int key1bits, key2bits;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( key != NULL );

    ret = mbedtls_aes_xts_decode_keys( key, keybits, &key1, &key1bits,
                                       &key2, &key2bits );
    if( ret != 0 )
        return( ret );

    /* Set the tweak key. Always set tweak key for encryption. */
    ret = mbedtls_aes_setkey_enc( &ctx->tweak, key2, key2bits );
    if( ret != 0 )
        return( ret );

    /* Set crypt key for decryption. */
    return mbedtls_aes_setkey_dec( &ctx->crypt, key1, key1bits );
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */


/*
 * AES-ECB block encryption/decryption
 */
int mbedtls_aes_crypt_ecb( mbedtls_aes_context *ctx,
                           int mode,
                           const unsigned char input[16],
                           unsigned char output[16] )
{
    int ret = 0;
    u32 key_id = 0;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );
    
    // use different sw key
	if (TrustZone_IsSecure() == 0) {
        key_id = KM_AES_KEY_NS_SW1;
    } else {
        key_id = KM_AES_KEY_S_SW1;
	}

    // A mutex is required to ensure that 
    // the SW key will not be modified during the calculation process.
    IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
    ret = crypto_aes_set_sw_key(key_id, ctx->key_len_bits, ctx->key_val);
    if(ret != 0)
        goto exit;
    // ecb use lalu slave mode, no cache alignment requirements 
    ret = crypto_aes_ecb_slave(key_id, ctx->key_len_bits, mode, input, output);

exit:
    IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
    return ret;
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)
/*
 * AES-CBC buffer encryption/decryption
 */
int mbedtls_aes_crypt_cbc( mbedtls_aes_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output )
{
    size_t key_id = 0;
    int ret = 0;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );
    AES_VALIDATE_RET( iv != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    if( length % 16 )
        return( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );

    if( length > 0 )
    {
        if (TrustZone_IsSecure() == 0) {
            key_id = KM_AES_KEY_NS_SW1;
        } else {
            key_id = KM_AES_KEY_S_SW1;
        }
        IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
        ret = crypto_aes_set_sw_key(key_id, ctx->key_len_bits, ctx->key_val);
        if(ret != 0)
            goto exit;
        ret = crypto_aes_cbc(key_id, ctx->key_len_bits, mode, input, length, iv, output);
    }

exit:
    IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
    return ret;
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_XTS)

/*
 * AES-XTS buffer encryption/decryption
 */
int mbedtls_aes_crypt_xts( mbedtls_aes_xts_context *ctx,
                           int mode,
                           size_t length,
                           const unsigned char data_unit[16],
                           const unsigned char *input,
                           unsigned char *output )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );
    AES_VALIDATE_RET( data_unit != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    /* Data units must be at least 16 bytes long. */
    if( length < 16 )
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;

    /* NIST SP 800-38E disallows data units larger than 2**20 blocks. */
    if( length > ( 1 << 20 ) * 16 )
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;

    // use different sw key
    size_t key_id = 0, xts_key2_id = 0;
	if (TrustZone_IsSecure() == 0) {
        key_id = KM_AES_KEY_NS_SW1;
        xts_key2_id = KM_AES_KEY_NS_SW2;
    } else {
        key_id = KM_AES_KEY_S_SW1;
        xts_key2_id = KM_AES_KEY_S_SW2;
	}

    // A mutex is required to ensure that 
    // the SW key will not be modified during the calculation process.
    IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);

    ret = crypto_aes_set_sw_key(key_id, ctx->crypt.key_len_bits, ctx->crypt.key_val);
    if(ret != 0)
        goto exit;

    ret = crypto_aes_set_sw_key(xts_key2_id, ctx->tweak.key_len_bits, ctx->tweak.key_val);
    if(ret != 0)
        goto exit;

    ret = crypto_aes_xts(key_id, xts_key2_id, ctx->crypt.key_len_bits, mode, input, length, (uint8_t *)data_unit, output);

exit:
    IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
    return( 0 );
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
/*
 * AES-CFB128 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb128( mbedtls_aes_context *ctx,
                       int mode,
                       size_t length,
                       size_t *iv_off,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    size_t n;
    size_t key_id = 0;
    int ret = 0;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );
    AES_VALIDATE_RET( iv_off != NULL );
    AES_VALIDATE_RET( iv != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    n = *iv_off;

    if( n > 15 )
        return( MBEDTLS_ERR_AES_BAD_INPUT_DATA );
		
    if (TrustZone_IsSecure() == 0) {
        key_id = KM_AES_KEY_NS_SW1;
    } else {
        key_id = KM_AES_KEY_S_SW1;
    }
    IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
    ret = crypto_aes_set_sw_key(key_id, ctx->key_len_bits, ctx->key_val);
    if(ret != 0)
        goto exit;

    ret = crypto_aes_cfb(key_id, ctx->key_len_bits, mode, input, length, iv, output);

    *iv_off = n;

exit:
    IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
    return ret;
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb8( mbedtls_aes_context *ctx,
                            int mode,
                            size_t length,
                            unsigned char iv[16],
                            const unsigned char *input,
                            unsigned char *output )
{
    unsigned char c;
    unsigned char ov[17];

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );
    AES_VALIDATE_RET( iv != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );
    while( length-- )
    {
        memcpy( ov, iv, 16 );
        mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );

        if( mode == MBEDTLS_AES_DECRYPT )
            ov[16] = *input;

        c = *output++ = (unsigned char)( iv[0] ^ *input++ );

        if( mode == MBEDTLS_AES_ENCRYPT )
            ov[16] = c;

        memcpy( iv, ov + 1, 16 );
    }

    return( 0 );
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_OFB)
/*
 * AES-OFB (Output Feedback Mode) buffer encryption/decryption
 */
int mbedtls_aes_crypt_ofb( mbedtls_aes_context *ctx,
                           size_t length,
                           size_t *iv_off,
                           unsigned char iv[16],
                           const unsigned char *input,
                           unsigned char *output )
{
    int ret = 0;
    size_t n;
    size_t key_id = 0;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( iv_off != NULL );
    AES_VALIDATE_RET( iv != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    n = *iv_off;

    if( n > 15 )
        return( MBEDTLS_ERR_AES_BAD_INPUT_DATA );

    if (TrustZone_IsSecure() == 0) {
        key_id = KM_AES_KEY_NS_SW1;
    } else {
        key_id = KM_AES_KEY_S_SW1;
    }
    IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
    ret = crypto_aes_set_sw_key(key_id, ctx->key_len_bits, ctx->key_val);
    if(ret != 0)
        goto exit;
    ret = crypto_aes_ofb(key_id, ctx->key_len_bits, MBEDTLS_AES_ENCRYPT, input, length, iv, output);


    *iv_off = n;

exit:
    IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
    return( ret );
}
#endif /* MBEDTLS_CIPHER_MODE_OFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
/*
 * AES-CTR buffer encryption/decryption
 */
int mbedtls_aes_crypt_ctr( mbedtls_aes_context *ctx,
                       size_t length,
                       size_t *nc_off,
                       unsigned char nonce_counter[16],
                       unsigned char stream_block[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    size_t n;
    int ret = 0;
    size_t key_id = 0;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( nc_off != NULL );
    AES_VALIDATE_RET( nonce_counter != NULL );
    AES_VALIDATE_RET( stream_block != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    n = *nc_off;

    if ( n > 0x0F )
        return( MBEDTLS_ERR_AES_BAD_INPUT_DATA );

    if (TrustZone_IsSecure() == 0) {
        key_id = KM_AES_KEY_NS_SW1;
    } else {
        key_id = KM_AES_KEY_S_SW1;
    }
    IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
    ret = crypto_aes_set_sw_key(key_id, ctx->key_len_bits, ctx->key_val);
    if(ret != 0)
        goto exit;
    ret = crypto_aes_ctr(key_id, ctx->key_len_bits, MBEDTLS_AES_ENCRYPT, input, length, nonce_counter, output);
    _memcpy(stream_block, nonce_counter, 16);

    *nc_off = n;

exit:
    IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
    return ret;
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#endif /* MBEDTLS_AES_ALT */
