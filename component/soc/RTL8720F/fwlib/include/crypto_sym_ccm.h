/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _CRYPTO_SYM_CCM_H_
#define _CRYPTO_SYM_CCM_H_

#include "ameba_crypto_symmetric_cipher.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CCM Mode definitions */
#define CCM_MODE_DECRYPT        0
#define CCM_MODE_ENCRYPT        1
#define CCM_MODE_STAR_DECRYPT   2
#define CCM_MODE_STAR_ENCRYPT   3

/* CCM State definitions */
#define CCM_STATE_STARTED       (1 << 0)  /* starts() called - q, mode, nonce known */
#define CCM_STATE_LENGTHS_SET   (1 << 1)  /* set_lengths() called - lengths, tag_len known */
#define CCM_STATE_B0_DONE       (1 << 2)  /* B0 processed and hardware ready */

/**
 * @brief Hardware CCM context structure
 */
typedef struct {
	/* Hardware engine state */
	uint8_t key[32];              /* Saved sw key - support up to 256 bits */
	uint8_t tag[16];             /* Saved hardware TAG register state */
	uint8_t iv[16];              /* Saved hardware IV register state */
	uint8_t nonce[13];            /* Saved IV/nonce for deferred initialization */
	uint8_t nonce_len;           /* Saved IV/nonce length */

	/* Buffer management */
	uint8_t aad_buffer[16];        /* Authentication data buffer (for incomplete blocks) */
	uint8_t msg_buffer[16];       /* Message buffer (for incomplete blocks) */
	uint32_t aad_buffer_len;    /* Authentication data buffer length */
	uint32_t msg_buffer_len;   /* Message buffer length */

	uint32_t aad_total_len;    /* Authentication data buffer length */
	uint32_t msg_total_len;   /* Message buffer length */

	/* State management */
	u32 inited;              /*!< Context inited flag will set to CRYPTO_CTX_STARTED pattern in init */
	uint8_t state;                /* Operation state flags: CCM_STATE_STARTED, CCM_STATE_LENGTHS_SET, CCM_STATE_B0_DONE */
	uint8_t tag_len;             /* Tag length */
	uint8_t q;                    /* Length encoding parameter q = 15 - iv_len */
	uint8_t ccm_mode;                 /* CCM mode: CCM_MODE_DECRYPT, CCM_MODE_ENCRYPT, CCM_MODE_STAR_DECRYPT, CCM_MODE_STAR_ENCRYPT  */
	uint8_t aad_processed_flag;   /* Flag indicating if AAD formatting has been performed */
	uint8_t key_id;               /* Key ID */
	uint32_t key_len_bits;        /* Key length */
	crypto_engine_mode engine_mode; /* AES/SM4 engine mode */
} crypto_sym_ccm_context;

/**
 * @brief Initialize CCM context
 * @param engine_mode Crypto engine mode selection
 *		 @arg CRYPTO_AES_MODE for AES
 *		 @arg CRYPTO_SM4_MODE for SM4
 * @param ctx Pointer to CCM context structure
 */
void crypto_sym_ccm_init(crypto_engine_mode engine_mode, crypto_sym_ccm_context *ctx);

/**
 * @brief Set CCM key
 * @param ctx Pointer to CCM context structure
 * @param key_id Key ID
 * @param key_len_bits Key length in bits (128/192/256)
 * @param key_addr Pointer to key data (for software keys)
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_sym_ccm_setkey(crypto_sym_ccm_context *ctx,
						  uint8_t key_id,
						  uint32_t key_len_bits,
						  uint8_t *key_addr);

/**
 * @brief Free CCM context
 * @param ctx Pointer to CCM context structure
 */
void crypto_sym_ccm_free(crypto_sym_ccm_context *ctx);

/**
 * @brief Set processing lengths
 * @param ctx Pointer to CCM context structure
 * @param total_ad_len Total authentication data length
 * @param plaintext_len Total plaintext length
 * @param tag_len Tag length (0, 4, 6, 8, 10, 12, 14, or 16)
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_sym_ccm_set_lengths(crypto_sym_ccm_context *ctx,
							   size_t total_ad_len,
							   size_t plaintext_len,
							   size_t tag_len);

/**
 * @brief Start CCM operation
 * @param ctx Pointer to CCM context structure
 * @param ccm_mode CCM mode (CCM_MODE_ENCRYPT/CCM_MODE_DECRYPT/CCM_MODE_STAR_ENCRYPT/CCM_MODE_STAR_DECRYPT)
 * @param iv Initialization vector/nonce
 * @param iv_len IV length (7-13 bytes)
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_sym_ccm_starts(crypto_sym_ccm_context *ctx,
						  uint8_t ccm_mode,
						  const uint8_t *iv,
						  size_t iv_len);

/**
 * @brief Process authentication data
 * @param ctx Pointer to CCM context structure
 * @param ad Authentication data pointer
 * @param ad_len Authentication data length
 * @return RTK_SUCCESS on success, error code on failure
 */
int crypto_sym_ccm_update_ad(crypto_sym_ccm_context *ctx,
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
int crypto_sym_ccm_update(crypto_sym_ccm_context *ctx,
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
int crypto_sym_ccm_finish(crypto_sym_ccm_context *ctx,
						  uint8_t *tag,
						  size_t tag_len);

#ifdef __cplusplus
}
#endif

#endif /* _CRYPTO_SYM_CCM_H_ */