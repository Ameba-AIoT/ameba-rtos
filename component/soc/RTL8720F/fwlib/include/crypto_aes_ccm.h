/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _CRYPTO_AES_CCM_H_
#define _CRYPTO_AES_CCM_H_

#include "crypto_sym_ccm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AES-CCM context typedef
 */
typedef crypto_sym_ccm_context crypto_aes_ccm_context;

/**
 * @brief Initialize CCM context
 * @param ctx Pointer to CCM context structure
 */
void crypto_aes_ccm_init(crypto_aes_ccm_context *ctx);

/**
 * @brief Set CCM key
 * @param ctx Pointer to CCM context structure
 * @param key_id Key ID
 * @param key_len_bits Key length in bits (128/192/256)
 * @param key_addr Pointer to key data
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_aes_ccm_setkey(crypto_aes_ccm_context *ctx,
						  uint8_t key_id,
						  uint32_t key_len_bits,
						  uint8_t *key_addr);

/**
 * @brief Free CCM context
 * @param ctx Pointer to CCM context structure
 */
void crypto_aes_ccm_free(crypto_aes_ccm_context *ctx);

/**
 * @brief Set processing lengths
 * @param ctx Pointer to CCM context structure
 * @param total_ad_len Total authentication data length
 * @param plaintext_len Total plaintext length
 * @param tag_len Tag length (0, 4, 6, 8, 10, 12, 14, or 16)
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_aes_ccm_set_lengths(crypto_aes_ccm_context *ctx,
							   size_t total_ad_len,
							   size_t plaintext_len,
							   size_t tag_len);

/**
 * @brief Start CCM operation
 * @param ctx Pointer to CCM context structure
 * @param mode CCM mode (CCM_MODE_ENCRYPT/CCM_MODE_DECRYPT/CCM_MODE_STAR_ENCRYPT/CCM_MODE_STAR_DECRYPT)
 * @param iv Initialization vector/nonce
 * @param iv_len IV length (7-13 bytes)
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_aes_ccm_starts(crypto_aes_ccm_context *ctx,
						  uint8_t mode,
						  const uint8_t *iv,
						  size_t iv_len);

/**
 * @brief Process authentication data
 * @param ctx Pointer to CCM context structure
 * @param ad Authentication data pointer
 * @param ad_len Authentication data length
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_aes_ccm_update_ad(crypto_aes_ccm_context *ctx,
							 const uint8_t *ad,
							 size_t ad_len);

/**
 * @brief Process message data
 * @param ctx Pointer to CCM context structure
 * @param input Input data pointer
 * @param input_len Input data length
 * @param output Output data pointer
 * @param output_size Output buffer size
 * @param output_len Pointer to output actual length
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_aes_ccm_update(crypto_aes_ccm_context *ctx,
						  const uint8_t *input, size_t input_len,
						  uint8_t *output, size_t output_size,
						  size_t *output_len);

/**
 * @brief Finish CCM operation and get tag
 * @param ctx Pointer to CCM context structure
 * @param tag Output tag buffer
 * @param tag_len Tag length
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_aes_ccm_finish(crypto_aes_ccm_context *ctx,
						  uint8_t *tag,
						  size_t tag_len);

#ifdef __cplusplus
}
#endif

#endif /* _CRYPTO_AES_CCM_H_ */