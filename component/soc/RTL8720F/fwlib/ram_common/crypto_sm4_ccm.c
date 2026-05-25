/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "crypto_sm4_ccm.h"


void crypto_sm4_ccm_init(crypto_sm4_ccm_context *ctx)
{
	crypto_sym_ccm_init(CRYPTO_SM4_MODE, ctx);
}

int crypto_sm4_ccm_setkey(crypto_sm4_ccm_context *ctx,
						  uint8_t key_id,
						  uint32_t key_len_bits,
						  uint8_t *key_addr)
{
	return crypto_sym_ccm_setkey(ctx, key_id, key_len_bits, key_addr);
}

void crypto_sm4_ccm_free(crypto_sm4_ccm_context *ctx)
{
	crypto_sym_ccm_free(ctx);
}

int crypto_sm4_ccm_set_lengths(crypto_sm4_ccm_context *ctx,
							   size_t total_ad_len,
							   size_t plaintext_len,
							   size_t tag_len)
{
	return crypto_sym_ccm_set_lengths(ctx, total_ad_len, plaintext_len, tag_len);
}

int crypto_sm4_ccm_starts(crypto_sm4_ccm_context *ctx,
						  uint8_t mode,
						  const uint8_t *iv,
						  size_t iv_len)
{
	return crypto_sym_ccm_starts(ctx, mode, iv, iv_len);
}

int crypto_sm4_ccm_update_ad(crypto_sm4_ccm_context *ctx,
							 const uint8_t *ad,
							 size_t ad_len)
{
	return crypto_sym_ccm_update_ad(ctx, ad, ad_len);
}

int crypto_sm4_ccm_update(crypto_sm4_ccm_context *ctx,
						  const uint8_t *input, size_t input_len,
						  uint8_t *output, size_t output_size,
						  size_t *output_len)
{
	return crypto_sym_ccm_update(ctx, input, input_len, output, output_size, output_len);
}

int crypto_sm4_ccm_finish(crypto_sm4_ccm_context *ctx,
						  uint8_t *tag,
						  size_t tag_len)
{
	return crypto_sym_ccm_finish(ctx, tag, tag_len);
}

#ifdef CRYPTO_NOT_USE_ROM_CCM_API

int crypto_sym_ccm_encrypt_and_tag(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 *key_addr, u8 is_encryption, u32 msg_len,
								   u8 *nonce_addr, u32 nonce_len, u8 *aad_addr, u32 aad_len,
								   const u8 *input, u8 *output, u32 tag_len, u8 *tag)
{
	int ret = RTK_FAIL;
	crypto_sm4_ccm_context ctx;
	size_t out_len;
	UNUSED(mode);
	crypto_sm4_ccm_init(&ctx);
	CRYPTO_CHK(crypto_sm4_ccm_setkey(&ctx, key_id, key_len_bits, key_addr), ret);
	CRYPTO_CHK(crypto_sm4_ccm_set_lengths(&ctx, aad_len, msg_len, tag_len), ret);
	CRYPTO_CHK(crypto_sm4_ccm_starts(&ctx, is_encryption, nonce_addr, nonce_len), ret);
	CRYPTO_CHK(crypto_sm4_ccm_update_ad(&ctx, aad_addr, aad_len), ret);
	CRYPTO_CHK(crypto_sm4_ccm_update(&ctx, input, msg_len, output, msg_len, &out_len), ret);
	CRYPTO_CHK(crypto_sm4_ccm_finish(&ctx, tag, tag_len), ret);

exit:
	return ret;
}

int crypto_sym_ccm_auth_decrypt(crypto_engine_mode mode, u8 key_id, u32 key_len_bits, u8 *key_addr, u32 msg_len,
								u8 *nonce_addr, u32 nonce_len, u8 *aad_addr, u32 aad_len,
								const u8 *input, u8 *output, u32 tag_len, u8 *tag)
{
	uint8_t final_tag_output[AES_BLOCK_SIZE] __attribute__((aligned(32)));
	int ret = RTK_FAIL;

	CRYPTO_CHK(crypto_sym_ccm_encrypt_and_tag(mode, key_id, key_len_bits, key_addr, FALSE, msg_len,
			   nonce_addr, nonce_len, aad_addr, aad_len, input, output, tag_len, final_tag_output), ret);

	CRYPTO_CHK(_memcmp(final_tag_output, tag, tag_len), RTK_FAIL);

exit:
	if (ret != RTK_SUCCESS) {
		_memset(output, 0, msg_len);
		DCache_CleanInvalidate((u32) output, msg_len);
	}
	return ret;
}

#endif
