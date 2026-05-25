/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "crypto_aes_ccm.h"


void crypto_aes_ccm_init(crypto_aes_ccm_context *ctx)
{
	crypto_sym_ccm_init(CRYPTO_AES_MODE, ctx);
}

int crypto_aes_ccm_setkey(crypto_aes_ccm_context *ctx,
						  uint8_t key_id,
						  uint32_t key_len_bits,
						  uint8_t *key_addr)
{
	return crypto_sym_ccm_setkey(ctx, key_id, key_len_bits, key_addr);
}

void crypto_aes_ccm_free(crypto_aes_ccm_context *ctx)
{
	crypto_sym_ccm_free(ctx);
}

int crypto_aes_ccm_set_lengths(crypto_aes_ccm_context *ctx,
							   size_t total_ad_len,
							   size_t plaintext_len,
							   size_t tag_len)
{
	return crypto_sym_ccm_set_lengths(ctx, total_ad_len, plaintext_len, tag_len);
}

int crypto_aes_ccm_starts(crypto_aes_ccm_context *ctx,
						  uint8_t mode,
						  const uint8_t *iv,
						  size_t iv_len)
{
	return crypto_sym_ccm_starts(ctx, mode, iv, iv_len);
}

int crypto_aes_ccm_update_ad(crypto_aes_ccm_context *ctx,
							 const uint8_t *ad,
							 size_t ad_len)
{
	return crypto_sym_ccm_update_ad(ctx, ad, ad_len);
}

int crypto_aes_ccm_update(crypto_aes_ccm_context *ctx,
						  const uint8_t *input, size_t input_len,
						  uint8_t *output, size_t output_size,
						  size_t *output_len)
{
	return crypto_sym_ccm_update(ctx, input, input_len, output, output_size, output_len);
}

int crypto_aes_ccm_finish(crypto_aes_ccm_context *ctx,
						  uint8_t *tag,
						  size_t tag_len)
{
	return crypto_sym_ccm_finish(ctx, tag, tag_len);
}
