/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _CRYPTO_SYM_GCM_H_
#define _CRYPTO_SYM_GCM_H_

#include "ameba_crypto_symmetric_cipher.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  AES-GCM Context Structure Definition
  */
typedef struct {
	u8 key[32];              /*!< Key storage (AES-256 max) */
	u32 key_len_bits;        /*!< Key length in bits */
	u8 key_id;               /*!< Key ID for key management */
	u32 iv[4];               /*!< IV buffer */
	u32 iv_origin[4];               /*!< IV buffer */
	u32 tag[4];              /*!< Tag buffer backup */
	u32 ghash_key[4];        /*!< GHASH key backup */
	u8 aad_buffer[16];       /*!< Residual AAD buffer */
	u32 aad_buffer_len;      /*!< Residual AAD length */
	u8 msg_buffer[16];       /*!< Residual message buffer */
	u32 msg_buffer_len;      /*!< Residual message length */
	u32 total_aad_len;       /*!< Total AAD length processed */
	u32 total_msg_len;       /*!< Total message length processed */
	u8 enc_mode;                 /*!< CIPHER_ENCRYPTION_MODE / CIPHER_DECRYPTION_MODE */
	u8 started;              /*!< Context started flag */
	u32 inited;              /*!< Context inited flag will set to CRYPTO_CTX_STARTED pattern in init */
	crypto_engine_mode engine_mode; /*!< AES/SM4 engine mode */
} crypto_sym_gcm_context;

/**
 * @brief Initialize GCM context
 * @param engine_mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param ctx Pointer to GCM context structure
 */
void crypto_sym_gcm_init(crypto_engine_mode engine_mode, crypto_sym_gcm_context *ctx);

/**
 * @brief Free GCM context
 * @param ctx Pointer to GCM context structure
 */
void crypto_sym_gcm_free(crypto_sym_gcm_context *ctx);

/**
 * @brief Set GCM key
 * @param ctx Pointer to GCM context structure
 * @param key_id Key ID
 * @param key_addr Pointer to key data
 * @param key_len_bits Key length in bits (128/192/256)
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_sym_gcm_setkey(crypto_sym_gcm_context *ctx, u8 key_id, u8 *key_addr, u32 key_len_bits);

/**
 * @brief Start GCM operation
 * @param ctx Pointer to GCM context structure
 * @param enc_mode CIPHER_ENCRYPTION_MODE / CIPHER_DECRYPTION_MODE
 * @param iv_addr Pointer to initialization vector
 * @param iv_len IV length in bytes
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_sym_gcm_starts(crypto_sym_gcm_context *ctx, u8 enc_mode, u8 *iv_addr, u32 iv_len);

/**
 * @brief Process authentication data
 * @param ctx Pointer to GCM context structure
 * @param aad_addr Pointer to authentication data
 * @param aad_len Authentication data length
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_sym_gcm_update_ad(crypto_sym_gcm_context *ctx, const u8 *aad_addr, u32 aad_len);

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
int crypto_sym_gcm_update(crypto_sym_gcm_context *ctx, const u8 *input, u32 input_length,
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
int crypto_sym_gcm_finish(crypto_sym_gcm_context *ctx, u8 *output, u32 output_size,
						  u32 *output_length, u8 *tag, u32 tag_len);

/**
 * @brief Perform GCM encryption/decryption with tag generation in one call
 * @param ctx Pointer to GCM context structure
 * @param enc_mode CIPHER_ENCRYPTION_MODE / CIPHER_DECRYPTION_MODE
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
int crypto_sym_gcm_crypt_and_tag(crypto_sym_gcm_context *ctx, u8 enc_mode, u8 *iv_addr, u32 iv_len, u8 *aad_addr, u32 aad_len,
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
int crypto_sym_gcm_auth_decrypt(crypto_sym_gcm_context *ctx, u8 *iv_addr, u32 iv_len, u8 *aad_addr, u32 aad_len,
								const u8 *input, u8 *output, u32 len, u32 tag_len, u8 *tag);

#ifdef __cplusplus
}
#endif

#endif /* _CRYPTO_SYM_GCM_H_ */