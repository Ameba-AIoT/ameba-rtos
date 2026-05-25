/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _CRYPTO_AES_GCM_H_
#define _CRYPTO_AES_GCM_H_

#include "crypto_sym_gcm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AES-GCM context typedef
 */
typedef crypto_sym_gcm_context crypto_aes_gcm_context;

/**
 * @brief Initialize GCM context
 * @param ctx Pointer to GCM context structure
 */
void crypto_aes_gcm_init(crypto_aes_gcm_context *ctx);

/**
 * @brief Free GCM context
 * @param ctx Pointer to GCM context structure
 */
void crypto_aes_gcm_free(crypto_aes_gcm_context *ctx);

/**
 * @brief Set GCM key
 * @param ctx Pointer to GCM context structure
 * @param key_id Key ID
 * @param key_addr Pointer to key data
 * @param key_len_bits Key length in bits (128/192/256)
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_aes_gcm_setkey(crypto_aes_gcm_context *ctx, u8 key_id, u8 *key_addr, u32 key_len_bits);

/**
 * @brief Start GCM operation
 * @param ctx Pointer to GCM context structure
 * @param mode CIPHER_ENCRYPTION_MODE / CIPHER_DECRYPTION_MODE
 * @param iv_addr Pointer to initialization vector
 * @param iv_len IV length in bytes
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_aes_gcm_starts(crypto_aes_gcm_context *ctx, u8 mode, u8 *iv_addr, u32 iv_len);

/**
 * @brief Process authentication data
 * @param ctx Pointer to GCM context structure
 * @param aad_addr Pointer to authentication data
 * @param aad_len Authentication data length
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_aes_gcm_update_ad(crypto_aes_gcm_context *ctx, const u8 *aad_addr, u32 aad_len);

/**
 * @brief Process message data
 * @param ctx Pointer to GCM context structure
 * @param input Pointer to input data
 * @param input_length Input data length
 * @param output Pointer to output buffer
 * @param output_size Output buffer size
 * @param output_length Pointer to actual output length
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_aes_gcm_update(crypto_aes_gcm_context *ctx, const u8 *input, u32 input_length,
						  u8 *output, u32 output_size, u32 *output_length);

/**
 * @brief Finish GCM operation and get tag
 * @param ctx Pointer to GCM context structure
 * @param output Pointer to final output buffer (can be NULL)
 * @param output_size Output buffer size
 * @param output_length Pointer to actual output length
 * @param tag Pointer to output tag buffer
 * @param tag_len Tag length (4-16 bytes)
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_aes_gcm_finish(crypto_aes_gcm_context *ctx, u8 *output, u32 output_size,
						  u32 *output_length, u8 *tag, u32 tag_len);

/**
 * @brief Perform GCM encryption/decryption with tag generation in one call
 * @param ctx Pointer to GCM context structure
 * @param mode CIPHER_ENCRYPTION_MODE / CIPHER_DECRYPTION_MODE
 * @param iv_addr Pointer to initialization vector
 * @param iv_len IV length in bytes
 * @param aad_addr Pointer to authentication data (can be NULL)
 * @param aad_len Authentication data length
 * @param input Pointer to input data (can be NULL)
 * @param output Pointer to output buffer
 * @param len Input/output data length
 * @param tag_len Tag length (4-16 bytes)
 * @param tag Pointer to output tag buffer
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_aes_gcm_crypt_and_tag(crypto_aes_gcm_context *ctx, u8 mode, u8 *iv_addr, u32 iv_len, u8 *aad_addr, u32 aad_len,
								 const u8 *input, u8 *output, u32 len, u32 tag_len, u8 *tag);

/**
 * @brief Perform GCM authenticated decryption
 * @param ctx Pointer to GCM context structure
 * @param iv_addr Pointer to initialization vector
 * @param iv_len IV length in bytes
 * @param aad_addr Pointer to authentication data (can be NULL)
 * @param aad_len Authentication data length
 * @param input Pointer to input data
 * @param output Pointer to output buffer
 * @param len Input/output data length
 * @param tag_len Tag length (4-16 bytes)
 * @param tag Pointer to authentication tag to verify
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_aes_gcm_auth_decrypt(crypto_aes_gcm_context *ctx, u8 *iv_addr, u32 iv_len, u8 *aad_addr, u32 aad_len,
								const u8 *input, u8 *output, u32 len, u32 tag_len, u8 *tag);

#ifdef __cplusplus
}
#endif

#endif /* _CRYPTO_AES_GCM_H_ */