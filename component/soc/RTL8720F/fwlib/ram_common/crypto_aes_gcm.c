/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "crypto_aes_gcm.h"

void crypto_aes_gcm_init(crypto_aes_gcm_context *ctx)
{
	crypto_sym_gcm_init(CRYPTO_AES_MODE, ctx);
}


void crypto_aes_gcm_free(crypto_aes_gcm_context *ctx)
{
	crypto_sym_gcm_free(ctx);
}


int crypto_aes_gcm_setkey(crypto_aes_gcm_context *ctx, u8 key_id, u8 *key_addr, u32 key_len_bits)
{
	return crypto_sym_gcm_setkey(ctx, key_id, key_addr, key_len_bits);
}


int crypto_aes_gcm_starts(crypto_aes_gcm_context *ctx, u8 enc_mode, u8 *iv_addr, u32 iv_len)
{
	return crypto_sym_gcm_starts(ctx, enc_mode, iv_addr, iv_len);
}


int crypto_aes_gcm_update_ad(crypto_aes_gcm_context *ctx, const u8 *aad_addr, u32 aad_len)
{
	return crypto_sym_gcm_update_ad(ctx, aad_addr, aad_len);
}

int crypto_aes_gcm_update(crypto_aes_gcm_context *ctx, const u8 *input, u32 input_length,
						  u8 *output, u32 output_size, u32 *output_length)
{
	return crypto_sym_gcm_update(ctx, input, input_length, output, output_size, output_length);
}



int crypto_aes_gcm_finish(crypto_aes_gcm_context *ctx, u8 *output, u32 output_size,
						  u32 *output_length, u8 *tag, u32 tag_len)
{
	return crypto_sym_gcm_finish(ctx, output, output_size, output_length, tag, tag_len);
}


int crypto_aes_gcm_crypt_and_tag(crypto_aes_gcm_context *ctx, u8 enc_mode, u8 *iv_addr, u32 iv_len, u8 *aad_addr, u32 aad_len,
								 const u8 *input, u8 *output, u32 len, u32 tag_len, u8 *tag)
{
	return crypto_sym_gcm_crypt_and_tag(ctx, enc_mode, iv_addr, iv_len, aad_addr, aad_len,
										input, output, len, tag_len, tag);
}


int crypto_aes_gcm_auth_decrypt(crypto_aes_gcm_context *ctx, u8 *iv_addr, u32 iv_len, u8 *aad_addr, u32 aad_len,
								const u8 *input, u8 *output, u32 len, u32 tag_len, u8 *tag)
{
	return crypto_sym_gcm_auth_decrypt(ctx, iv_addr, iv_len, aad_addr, aad_len,
										input, output, len, tag_len, tag);
}

/******************* (C) COPYRIGHT 2023 Realtek Semiconductor *****END OF FILE****/
