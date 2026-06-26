/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "crypto_sym_gcm.h"

static int crypto_sym_gcm_update_ad_last(crypto_sym_gcm_context *ctx);

void crypto_sym_gcm_init(crypto_engine_mode engine_mode, crypto_sym_gcm_context *ctx)
{
	if (ctx == NULL) {
		return;
	}

	/* Zero-initialize the entire context structure */
	_memset(ctx, 0, sizeof(crypto_sym_gcm_context));

	/* Store engine mode in context */
	ctx->engine_mode = engine_mode;

	ctx->inited = CRYPTO_CTX_STARTED;
}


void crypto_sym_gcm_free(crypto_sym_gcm_context *ctx)
{
	if (ctx == NULL) {
		return;
	}

	/* Zero-initialize the entire context structure */
	_memset(ctx, 0, sizeof(crypto_sym_gcm_context));
}


int crypto_sym_gcm_setkey(crypto_sym_gcm_context *ctx, u8 key_id, u8 *key_addr, u32 key_len_bits)
{
	if (ctx == NULL) {
		return _ERRNO_CRYPTO_NULL_POINTER;
	}

	/* Validate key length */
	if (key_len_bits != KEY_BIT_128 && key_len_bits != KEY_BIT_192 && key_len_bits != KEY_BIT_256) {
		return _ERRNO_CRYPTO_KEY_LENGTH_ERR;
	}

	/* Validate key ID */
	if (!KM_IS_KEY_SEL_NUMBER(key_id)) {
		return _ERRNO_CRYPTO_KEY_OutRange;
	}

	/* Only store key information in context, don't call crypto_km_set_sw_key */
	ctx->key_id = key_id;
	ctx->key_len_bits = key_len_bits;

	/* For software keys, key material must be provided by the caller.
	 * Hardware keys (e.g. OTP) reside in the engine; key_addr is not needed. */
	if (KM_IS_SW_KEY_NUM(key_id)) {
		if (key_addr == NULL) {
			return _ERRNO_CRYPTO_NULL_POINTER;
		}
		_memcpy(ctx->key, key_addr, key_len_bits / 8);
	}

	return RTK_SUCCESS;
}


int crypto_sym_gcm_starts(crypto_sym_gcm_context *ctx, u8 enc_mode, u8 *iv_addr, u32 iv_len)
{
	AES_TypeDef *AES = AES_Get_AES_Addr();
	int ret = RTK_FAIL;
	u32 temp;
	u32 loopWait;

	if (ctx == NULL || iv_addr == NULL) {
		return _ERRNO_CRYPTO_NULL_POINTER;
	}

	if (ctx->inited != CRYPTO_CTX_STARTED) {
		return _ERRNO_CRYPTO_ENGINE_NOT_INIT;
	}

	/* Check IV length */
	if (iv_len == 0) {
		return _ERRNO_CRYPTO_IV_OutRange;
	}

	/* Reset per-record accumulated state so this context can be reused
	 * across multiple GCM operations (e.g. consecutive TLS records).
	 * total_aad_len, total_msg_len and ctx->tag (the GHASH accumulator
	 * initial value) must all start at zero for each new operation;
	 * leaving stale values causes wrong length fields and a non-zero
	 * GHASH starting point, which produces a bad authentication tag. */
	ctx->total_aad_len  = 0;
	ctx->total_msg_len  = 0;
	_memset(ctx->tag,        0, sizeof(ctx->tag));
	ctx->aad_buffer_len = 0;
	ctx->msg_buffer_len = 0;
	_memset(ctx->aad_buffer, 0, AES_BLOCK_SIZE);
	_memset(ctx->msg_buffer, 0, AES_BLOCK_SIZE);

	/* Store encryption mode in context */
	ctx->enc_mode = enc_mode;

	AES_lock_mutex();

	/* Set SCA mask */
	AES_Set_SCA_Mask();

	/* Set software key before loading */
	ret = crypto_km_set_sw_key(ctx->key_id, ctx->key_len_bits, ctx->key);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	/* Load key using key management */
	ret = crypto_km_load_key(KM_AES_ENGINE, ctx->key_id, ctx->key_len_bits);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	/* Clear tag and length registers */
	AES_InitTag();

	/* Initialize DMA */
	AES_DMA_Init();

	/* Configure key size in AES CONFIG */
	AES->AES_CONFIG = 0;
	switch (ctx->key_len_bits) {
	case KEY_BIT_128:
		AES->AES_CONFIG |= AES_KEY_SIZE(AES_KEY_SIZE_128);
		break;
	case KEY_BIT_192:
		AES->AES_CONFIG |= AES_KEY_SIZE(AES_KEY_SIZE_192);
		break;
	case KEY_BIT_256:
		AES->AES_CONFIG |= AES_KEY_SIZE(AES_KEY_SIZE_256);
		break;
	default:
		ret = _ERRNO_CRYPTO_KEY_LENGTH_ERR;
		goto exit;
	}

	/* Set SM4 core selection if needed */
	if (ctx->engine_mode == CRYPTO_SM4_MODE) {
		AES->AES_CONFIG |= AES_BIT_CORE_SELECT;
	}

	/* Set IV using existing AES_GCM_Set_Iv function */
	ret = AES_GCM_Set_Iv(iv_addr, iv_len);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	/* Backup J0 */
	for (u8 i = 0; i < 4; i++) {
		ctx->iv_origin[i] = AES->AES_IV_x[i];
	}

	/* Backup J0 + 1 as CTR start IV */
	AES->AES_CONFIG |= AES_BIT_IV_ADD_1;
	while (AES->AES_CONFIG & AES_BIT_IV_ADD_1);
	for (u8 i = 0; i < 4; i++) {
		ctx->iv[i] = AES->AES_IV_x[i];
	}

	/* GMAC mode auto init ghash_key, or caculate ghash_key in slave ECB mode */
	if (iv_len == 12) {
		AES->AES_CIPHER_LEN = AES_BLOCK_SIZE_BIT;
		temp = 0;
		SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);
		temp |= AES_CIPHER_MODE(CIPHER_MODE_ECB);	// ECB-MODE
		temp |= AES_BIT_EN_DE;
		temp |= AES_BIT_FIRST_BLOCK;

		/* Set SM4 core selection if needed */
		if (ctx->engine_mode == CRYPTO_SM4_MODE) {
			temp |= AES_BIT_CORE_SELECT;
		}

		AES->AES_CONFIG = temp;

		for (u8 i = 0; i < 4; i++) {
			AES->AES_CPU_DATAIN_x[i] = 0;
		}

		/* polling end flag */
		loopWait = AES_SLAVE_TIMEOUT;
		WAIT_FOR_AES_SLAVE_CIPHER_COMPLETION();
		AES->AES_CHN0_AES_INT_CLEAR = AES_BIT_INT_CLEAR;
		for (u8 i = 0; i < 4; i++) {
			ctx->ghash_key[i] = AES->AES_CPU_DATAOUT_x[i];
		}
	} else {
		for (u8 i = 0; i < 4; i++) {
			ctx->ghash_key[i] = AES->AES_GHASH_KEY_x[i];
		}
	}

	/* Mark context as started */
	ctx->started = 1;
	ret = RTK_SUCCESS;

exit:
	AES_unlock_mutex();
	return ret;
}


int crypto_sym_gcm_update_ad(crypto_sym_gcm_context *ctx, const u8 *aad_addr, u32 aad_len)
{
	AES_TypeDef *AES = AES_Get_AES_Addr();
	u32 temp;
	int ret = RTK_FAIL;
	u32 ctx_copy_length;
	u32 process_length;
	u32 aad_buffer_remain;
	const u8 *current_addr = aad_addr;
	u32 remaining_aad_len = aad_len;

	if (ctx == NULL || aad_addr == NULL) {
		return _ERRNO_CRYPTO_NULL_POINTER;
	}

	if (!ctx->started) {
		return _ERRNO_CRYPTO_ENGINE_NOT_INIT;
	}

	if (aad_len == 0) {
		return RTK_SUCCESS;
	}

	AES_lock_mutex();

	/* only restore tag. gmac will auto gen ghash_key, and not use iv */
	for (u8 i = 0; i < 4; i++) {
		AES->AES_TAG_x[i] = ctx->tag[i];
		AES->AES_GHASH_KEY_x[i] = ctx->ghash_key[i];
	}

	/* Set software key before loading */
	ret = crypto_km_set_sw_key(ctx->key_id, ctx->key_len_bits, ctx->key);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	/* Reload key */
	ret = crypto_km_load_key(KM_AES_ENGINE, ctx->key_id, ctx->key_len_bits);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	/* Process AAD according to the specified algorithm */

	/* Step 1: Process residual data in aad_buffer if not empty */
	if (ctx->aad_buffer_len > 0) {
		aad_buffer_remain = AES_BLOCK_SIZE - ctx->aad_buffer_len;
		ctx_copy_length = (remaining_aad_len <= aad_buffer_remain) ?
						  remaining_aad_len : aad_buffer_remain;

		/* Copy data to ctx buffer and update length */
		_memcpy(&ctx->aad_buffer[ctx->aad_buffer_len], current_addr, ctx_copy_length);
		ctx->aad_buffer_len += ctx_copy_length;
		current_addr += ctx_copy_length;
		remaining_aad_len -= ctx_copy_length;

		/* If aad_buffer is full, process one GMAC calculation using slave mode */
		if (ctx->aad_buffer_len == AES_BLOCK_SIZE) {
			/* Update total AAD length */
			ctx->total_aad_len += AES_BLOCK_SIZE;
			AES->AES_GMAC_LEN_x[1] = AES_BLOCK_SIZE_BIT;
			/* Configure for GMAC mode */
			temp = 0;
			SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);
			temp |= AES_CIPHER_MODE(CIPHER_MODE_GMAC);	/* GMAC-MODE */
			temp |= AES_BIT_EN_DE;						/* Encrypt-MODE */

			/* Set SM4 core selection if needed */
			if (ctx->engine_mode == CRYPTO_SM4_MODE) {
				temp |= AES_BIT_CORE_SELECT;
			}

			AES->AES_CONFIG = temp;

			/* Process the full block using slave mode */
			for (u8 i = 0; i < 4; i++) {
				AES->AES_CPU_DATAIN_x[i] = ((u32 *)ctx->aad_buffer)[i];
			}

			/* Wait for completion */
			u32 loopWait = AES_SLAVE_TIMEOUT;
			WAIT_FOR_AES_SLAVE_MAC_COMPLETION();
			AES->AES_CHN0_AES_INT_CLEAR = 0x1;

			/* Clear ctx buffer and related state variables */
			ctx->aad_buffer_len = 0;
			_memset(ctx->aad_buffer, 0, AES_BLOCK_SIZE);
		}
	}

	/* Step 2: Process remaining data if any */
	if (remaining_aad_len > 0) {
		/* Process complete blocks using slave mode */
		if (remaining_aad_len >= AES_BLOCK_SIZE) {
			process_length = (remaining_aad_len / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
			ctx->total_aad_len += process_length;

			/* Configure for GMAC mode */
			AES->AES_GMAC_LEN_x[1] = process_length * 8;
			temp = 0;
			SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);
			temp |= AES_CIPHER_MODE(CIPHER_MODE_GMAC);	/* GMAC-MODE */
			temp |= AES_BIT_EN_DE;						/* Encrypt-MODE */

			/* Set SM4 core selection if needed */
			if (ctx->engine_mode == CRYPTO_SM4_MODE) {
				temp |= AES_BIT_CORE_SELECT;
			}

			AES->AES_CONFIG = temp;

			/* Process complete blocks using slave mode */
			u32 num_blocks = process_length / AES_BLOCK_SIZE;
			for (u32 block = 0; block < num_blocks; block++) {
				/* Input one block of data */
				for (u8 i = 0; i < 4; i++) {
					AES->AES_CPU_DATAIN_x[i] = ((u32 *)(current_addr + block * AES_BLOCK_SIZE))[i];
				}

			}
			/* Wait for completion */
			u32 loopWait = AES_SLAVE_TIMEOUT;
			WAIT_FOR_AES_SLAVE_MAC_COMPLETION();
			AES->AES_CHN0_AES_INT_CLEAR = 0x1;

			current_addr += process_length;
			remaining_aad_len -= process_length;
		}

		/* Store remaining data in ctx buffer */
		if (remaining_aad_len > 0) {
			_memcpy(ctx->aad_buffer, current_addr, remaining_aad_len);
			ctx->aad_buffer_len = remaining_aad_len;
		}
	}

	/* Save tag */
	for (u8 i = 0; i < 4; i++) {
		ctx->tag[i] = AES->AES_TAG_x[i];
	}

	ret = RTK_SUCCESS;

exit:
	AES_unlock_mutex();
	return ret;
}


static int crypto_sym_gcm_update_ad_last(crypto_sym_gcm_context *ctx)
{
	AES_TypeDef *AES = AES_Get_AES_Addr();
	u32 temp;
	int ret = RTK_FAIL;
	u8 final_block[16]; /* 16 bytes final block */

	if (ctx == NULL) {
		return _ERRNO_CRYPTO_NULL_POINTER;
	}

	if (!ctx->started) {
		return _ERRNO_CRYPTO_ENGINE_NOT_INIT;
	}

	/* Only process if there's remaining AAD data */
	if (ctx->aad_buffer_len > 0) {
		/* Update total AAD length */
		ctx->total_aad_len += ctx->aad_buffer_len;

		/* Configure for GMAC mode */
		AES->AES_GMAC_LEN_x[1] = AES_BLOCK_SIZE_BIT;
		temp = 0;
		SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);
		temp |= AES_CIPHER_MODE(CIPHER_MODE_GMAC);	/* GMAC-MODE */
		temp |= AES_BIT_EN_DE;						/* Encrypt-MODE */

		/* Set SM4 core selection if needed */
		if (ctx->engine_mode == CRYPTO_SM4_MODE) {
			temp |= AES_BIT_CORE_SELECT;
		}

		AES->AES_CONFIG = temp;

		/* Pad the aad_buffer with zeros to 16 bytes if needed */
		_memset(final_block, 0, 16);
		_memcpy(final_block, ctx->aad_buffer, ctx->aad_buffer_len);

		/* Process using slave mode */
		for (u8 i = 0; i < 4; i++) {
			AES->AES_CPU_DATAIN_x[i] = ((u32 *)final_block)[i];
		}

		/* Wait for completion */
		u32 loopWait = AES_SLAVE_TIMEOUT;
		WAIT_FOR_AES_SLAVE_MAC_COMPLETION();
		AES->AES_CHN0_AES_INT_CLEAR = 0x1;

		/* Clear aad_buffer */
		ctx->aad_buffer_len = 0;
		_memset(ctx->aad_buffer, 0, 16);
	}

	ret = RTK_SUCCESS;

exit:
	return ret;
}


int crypto_sym_gcm_update(crypto_sym_gcm_context *ctx, const u8 *input, u32 input_length,
						  u8 *output, u32 output_size, u32 *output_length)
{
	AES_TypeDef *AES = AES_Get_AES_Addr();
	u32 temp;
	int ret = RTK_FAIL;
	u32 ctx_copy_length;
	u32 dma_mode_process_length;
	u32 msg_buffer_remain;
	const u8 *current_input = input;
	u8 *current_output = output;
	u32 remaining_msg_len = input_length;
	u32 actual_output_len = 0;

	if (ctx == NULL || input == NULL || output == NULL || output_length == NULL) {
		return _ERRNO_CRYPTO_NULL_POINTER;
	}

	if (!ctx->started) {
		return _ERRNO_CRYPTO_ENGINE_NOT_INIT;
	}

	/* Check output buffer size - follow mbedtls recommendation of input_length + 15 */
	if (output_size < input_length) {
		return _ERRNO_CRYPTO_MSG_OutRange;
	}

	if (input_length == 0) {
		*output_length = 0;
		return RTK_SUCCESS;
	}

	AES_lock_mutex();

	/* Set software key before loading */
	ret = crypto_km_set_sw_key(ctx->key_id, ctx->key_len_bits, ctx->key);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	/* Reload key */
	ret = crypto_km_load_key(KM_AES_ENGINE, ctx->key_id, ctx->key_len_bits);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	AES_DMA_Init();

	/* restore iv, tag, ghash_key */
	for (u8 i = 0; i < 4; i++) {
		AES->AES_IV_x[i] = ctx->iv[i];
		AES->AES_TAG_x[i] = ctx->tag[i];
		AES->AES_GHASH_KEY_x[i] = ctx->ghash_key[i];
	}

	/* Process any remaining AAD data first */
	if (ctx->aad_buffer_len > 0) {
		ret = crypto_sym_gcm_update_ad_last(ctx);
		if (ret != RTK_SUCCESS) {
			goto exit;
		}
	}

	/* Process message according to the specified algorithm */

	/* Step 1: Process residual data in msg_buffer if not empty */
	if (ctx->msg_buffer_len > 0) {
		msg_buffer_remain = AES_BLOCK_SIZE - ctx->msg_buffer_len;
		ctx_copy_length = (remaining_msg_len <= msg_buffer_remain) ?
						  remaining_msg_len : msg_buffer_remain;

		/* Copy data to ctx buffer and update length */
		_memcpy(&ctx->msg_buffer[ctx->msg_buffer_len], current_input, ctx_copy_length);
		ctx->msg_buffer_len += ctx_copy_length;
		current_input += ctx_copy_length;
		remaining_msg_len -= ctx_copy_length;

		/* If msg_buffer is full, process one GCM calculation using slave mode */
		if (ctx->msg_buffer_len == AES_BLOCK_SIZE) {
			/* Update total message length */
			ctx->total_msg_len += AES_BLOCK_SIZE;

			/* Configure for GCM mode */
			AES->AES_CIPHER_LEN = AES_BLOCK_SIZE_BIT;
			temp = 0;
			SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);
			temp |= AES_CIPHER_MODE(CIPHER_MODE_GCM);	/* GCM-MODE */
			if (ctx->enc_mode) {
				temp |= AES_BIT_EN_DE;
			} else {
				temp &= ~AES_BIT_EN_DE;
			}

			/* Set SM4 core selection if needed */
			if (ctx->engine_mode == CRYPTO_SM4_MODE) {
				temp |= AES_BIT_CORE_SELECT;
			}

			AES->AES_CONFIG = temp;

			/* Process the full block using slave mode */
			for (u8 i = 0; i < 4; i++) {
				AES->AES_CPU_DATAIN_x[i] = ((u32 *)ctx->msg_buffer)[i];
			}

			/* Wait for completion */
			u32 loopWait = AES_SLAVE_TIMEOUT;
			WAIT_FOR_AES_SLAVE_CIPHER_COMPLETION();
			AES->AES_CHN0_AES_INT_CLEAR = 0x1;

			/* Copy output data to user buffer */
			for (u8 i = 0; i < 4; i++) {
				((u32 *)current_output)[i] = AES->AES_CPU_DATAOUT_x[i];
			}
			current_output += AES_BLOCK_SIZE;
			actual_output_len += AES_BLOCK_SIZE;

			/* Clear ctx buffer and related state variables */
			ctx->msg_buffer_len = 0;
			_memset(ctx->msg_buffer, 0, AES_BLOCK_SIZE);
		}
	}

	/* Step 2: Process remaining data if any */
	if (remaining_msg_len > 0) {
		/* Process complete blocks using DMA mode */
		if (remaining_msg_len >= AES_BLOCK_SIZE) {
			dma_mode_process_length = (remaining_msg_len / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
			ctx->total_msg_len += dma_mode_process_length;

			/* Configure for GCM mode */
			AES->AES_CIPHER_LEN = dma_mode_process_length * 8;
			temp = 0;
			SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);
			temp |= AES_BIT_ENGINE_MODE; 				/* DMA-MODE */
			temp |= AES_CIPHER_MODE(CIPHER_MODE_GCM);	/* GCM-MODE */
			if (ctx->enc_mode) {
				temp |= AES_BIT_EN_DE;
			} else {
				temp &= ~AES_BIT_EN_DE;
			}

			/* Set SM4 core selection if needed */
			if (ctx->engine_mode == CRYPTO_SM4_MODE) {
				temp |= AES_BIT_CORE_SELECT;
			}

			AES->AES_CONFIG = temp;

			/* Process complete blocks using DMA */
			ret = AES_ProcessDma(current_input, dma_mode_process_length, current_output);
			if (ret != RTK_SUCCESS) {
				goto exit;
			}

			current_input += dma_mode_process_length;
			current_output += dma_mode_process_length;
			remaining_msg_len -= dma_mode_process_length;
			actual_output_len += dma_mode_process_length;
		}

		/* Store remaining data in ctx buffer */
		if (remaining_msg_len > 0) {
			_memcpy(ctx->msg_buffer, current_input, remaining_msg_len);
			ctx->msg_buffer_len = remaining_msg_len;
		}
	}

	/* backup iv, tag */
	for (u8 i = 0; i < 4; i++) {
		ctx->iv[i] = AES->AES_IV_x[i];
		ctx->tag[i] = AES->AES_TAG_x[i];
	}

	/* Return actual output length */
	*output_length = actual_output_len;
	ret = RTK_SUCCESS;

exit:
	AES_unlock_mutex();
	return ret;
}



int crypto_sym_gcm_finish(crypto_sym_gcm_context *ctx, u8 *output, u32 output_size,
						  u32 *output_length, u8 *tag, u32 tag_len)
{
	AES_TypeDef *AES = AES_Get_AES_Addr();
	u32 temp;
	int ret = RTK_FAIL;
	u32 loopWait = 0;
	u8 final_block[16];
	u32 actual_output_len = 0;

	if (ctx == NULL || tag == NULL || output_length == NULL) {
		return _ERRNO_CRYPTO_NULL_POINTER;
	}

	if (!ctx->started) {
		return _ERRNO_CRYPTO_ENGINE_NOT_INIT;
	}

	/* Validate tag length */
	if (tag_len > 16 || tag_len < 4) {
		return _ERRNO_CRYPTO_TAG_OutRange;
	}

	AES_lock_mutex();

	/* Set software key before loading */
	ret = crypto_km_set_sw_key(ctx->key_id, ctx->key_len_bits, ctx->key);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	/* Reload key */
	ret = crypto_km_load_key(KM_AES_ENGINE, ctx->key_id, ctx->key_len_bits);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	/* restore iv, tag, ghash_key */
	for (u8 i = 0; i < 4; i++) {
		AES->AES_IV_x[i] = ctx->iv[i];
		AES->AES_TAG_x[i] = ctx->tag[i];
		AES->AES_GHASH_KEY_x[i] = ctx->ghash_key[i];
	}

	/* Process any remaining AAD data first */
	if (ctx->aad_buffer_len > 0) {
		ret = crypto_sym_gcm_update_ad_last(ctx);
		if (ret != RTK_SUCCESS) {
			goto exit;
		}
	}

	/* Precondition: if there is a partial block to output, the caller's buffer
	 * must be large enough.  Checked here (before AES slave runs) so that an
	 * invalid output_size does not advance hardware IV/TAG state or mutate
	 * ctx->total_msg_len.
	 * Note: output==NULL is not an error — the caller simply discards the
	 * partial block output, which is a valid use case. */
	if (ctx->msg_buffer_len > 0 && ctx->msg_buffer_len > output_size) {
		ret = _ERRNO_CRYPTO_BUFFER_TOO_SMALL;
		goto exit;
	}

	/* Process any remaining message data */
	if (ctx->msg_buffer_len > 0) {
		/* Update total message length */
		ctx->total_msg_len += ctx->msg_buffer_len;

		/* Configure AES_CIPHER_LEN for GCM mode, this must be real length */
		AES->AES_CIPHER_LEN = ctx->msg_buffer_len * 8;
		temp = 0;
		SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);
		temp |= AES_CIPHER_MODE(CIPHER_MODE_GCM);	/* GCM-MODE */
		if (ctx->enc_mode) {
			temp |= AES_BIT_EN_DE;
		} else {
			temp &= ~AES_BIT_EN_DE;
		}

		/* Set SM4 core selection if needed */
		if (ctx->engine_mode == CRYPTO_SM4_MODE) {
			temp |= AES_BIT_CORE_SELECT;
		}

		AES->AES_CONFIG = temp;

		/* Pad the msg_buffer with zeros to 16 bytes */
		_memset(final_block, 0, 16);
		_memcpy(final_block, ctx->msg_buffer, ctx->msg_buffer_len);

		/* Process using slave mode */
		for (u8 i = 0; i < 4; i++) {
			AES->AES_CPU_DATAIN_x[i] = ((u32 *)final_block)[i];
		}

		loopWait = AES_SLAVE_TIMEOUT;
		WAIT_FOR_AES_SLAVE_CIPHER_COMPLETION();
		AES->AES_CHN0_AES_INT_CLEAR = 0x1;

		/* Copy final output data to user buffer if needed */
		if (output != NULL && ctx->msg_buffer_len > 0) {
			u8 hw_out[16];

			actual_output_len = ctx->msg_buffer_len;
			if (actual_output_len > output_size) {
				ret = _ERRNO_CRYPTO_MESSAGE_LEN_ERR;
				goto exit;
			}

			/* Read the full 16-byte HW output into a local buffer first,
			 * then copy only the valid trailing bytes to the caller.
			 * Writing directly through a u32 pointer would overflow the
			 * caller's buffer whenever the last block is shorter than 16 B.
			 */
			for (u8 i = 0; i < 4; i++) {
				((u32 *)hw_out)[i] = AES->AES_CPU_DATAOUT_x[i];
			}
			_memcpy(output, hw_out, actual_output_len);
		}

		/* Clear msg_buffer */
		ctx->msg_buffer_len = 0;
		_memset(ctx->msg_buffer, 0, 16);
	}

	/* Configure length registers for final GCM processing */
	AES->AES_GMAC_LEN_x[1] = ctx->total_aad_len * 8;
	AES->AES_CIPHER_LEN = ctx->total_msg_len * 8;

	/* Set IV to iv_origin */
	for (u8 i = 0; i < 4; i++) {
		AES->AES_IV_x[i] = ctx->iv_origin[i];
	}

	/* Configure for final GCM processing */
	temp = 0;
	SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);
	temp |= AES_CIPHER_MODE(CIPHER_MODE_GCM);	/* GCM-MODE */
	temp |= AES_BIT_AEAD_LAST;					/* LAST block */
	temp |= AES_BIT_FIRST_BLOCK;
	if (ctx->enc_mode) {
		temp |= AES_BIT_EN_DE;
	} else {
		temp &= ~AES_BIT_EN_DE;
	}

	/* Set SM4 core selection if needed */
	if (ctx->engine_mode == CRYPTO_SM4_MODE) {
		temp |= AES_BIT_CORE_SELECT;
	}

	AES->AES_CONFIG = temp;

	/* Wait for completion */
	loopWait = AES_SLAVE_TIMEOUT;
	WAIT_FOR_AES_SLAVE_CIPHER_COMPLETION();
	AES->AES_CHN0_AES_INT_CLEAR = 0x1;

	/* Get the final tag */
	AES_GetTag(tag, tag_len);

	/* Clear AEAD_LAST flag */
	AES->AES_CONFIG &= ~AES_BIT_AEAD_LAST;

	/* Return actual output length */
	*output_length = actual_output_len;

	/* Mark this operation as finished; all per-operation state
	 * (total_aad_len, total_msg_len, tag, buffers) will be reset
	 * by the next call to crypto_sym_gcm_starts(), following the
	 * same convention as mbedtls software gcm.c where finish()
	 * produces the tag and starts() resets all running state. */
	ctx->started = 0;

	ret = RTK_SUCCESS;

exit:
	AES_unlock_mutex();
	return ret;
}


int crypto_sym_gcm_crypt_and_tag(crypto_sym_gcm_context *ctx, u8 enc_mode, u8 *iv_addr, u32 iv_len, u8 *aad_addr, u32 aad_len,
								 const u8 *input, u8 *output, u32 len, u32 tag_len, u8 *tag)
{
	int ret = RTK_FAIL;
	u32 output_len = 0;
	u32 finish_output_len = 0;

	if (ctx == NULL || iv_addr == NULL || tag == NULL) {
		return _ERRNO_CRYPTO_NULL_POINTER;
	}

	/* Validate tag length */
	if (tag_len > 16 || tag_len < 4) {
		return _ERRNO_CRYPTO_TAG_OutRange;
	}

	/* Check IV length */
	if (iv_len == 0) {
		return _ERRNO_CRYPTO_IV_OutRange;
	}

	/* Start GCM operation */
	ret = crypto_sym_gcm_starts(ctx, enc_mode, iv_addr, iv_len);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	/* Process AAD if provided */
	if (aad_addr != NULL && aad_len > 0) {
		ret = crypto_sym_gcm_update_ad(ctx, aad_addr, aad_len);
		if (ret != RTK_SUCCESS) {
			goto exit;
		}
	}

	/* Process message if provided */
	if (input != NULL && output != NULL && len > 0) {
		ret = crypto_sym_gcm_update(ctx, input, len, output, len, &output_len);
		if (ret != RTK_SUCCESS) {
			goto exit;
		}
	}

	/* Finish and get tag, handle any remaining output */
	ret = crypto_sym_gcm_finish(ctx, output + output_len, len - output_len, &finish_output_len, tag, tag_len);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	ret = RTK_SUCCESS;

exit:
	return ret;
}


int crypto_sym_gcm_auth_decrypt(crypto_sym_gcm_context *ctx, u8 *iv_addr, u32 iv_len, u8 *aad_addr, u32 aad_len,
								const u8 *input, u8 *output, u32 len, u32 tag_len, u8 *tag)
{
	u8 calculated_tag[16] = {0};
	int ret = RTK_FAIL;

	if (ctx == NULL || iv_addr == NULL || tag == NULL) {
		return _ERRNO_CRYPTO_NULL_POINTER;
	}

	/* Validate tag length */
	if (tag_len > 16 || tag_len < 4) {
		return _ERRNO_CRYPTO_TAG_OutRange;
	}

	/* Perform decryption and tag calculation */
	ret = crypto_sym_gcm_crypt_and_tag(ctx, 0, iv_addr, iv_len, aad_addr, aad_len,
									   input, output, len, tag_len, calculated_tag);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	/* Compare calculated tag with provided tag */
	if (_memcmp(tag, calculated_tag, tag_len) != 0) {
		ret = _ERRNO_CRYPTO_GCM_TAG_NOT_MATCH;
		goto exit;
	}

	ret = RTK_SUCCESS;

exit:
	if (ret != RTK_SUCCESS) {
		/* Clear output on authentication failure */
		if (output != NULL && len > 0) {
			_memset(output, 0, len);
			DCache_CleanInvalidate((u32)output, len);
		}
	}

	return ret;
}







/******************* (C) COPYRIGHT 2023 Realtek Semiconductor *****END OF FILE****/
