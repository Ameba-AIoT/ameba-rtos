/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "crypto_sym_ccm.h"

/* Helper function to process remaining AAD buffer */
static int crypto_sym_ccm_update_ad_final(crypto_sym_ccm_context *ctx);

/* Helper function to process remaining message buffer */
static int crypto_sym_ccm_update_final(crypto_sym_ccm_context *ctx);

/* Helper function to restore hardware context */
static int crypto_sym_ccm_restore_context_and_key(crypto_sym_ccm_context *ctx);

/* Helper function to save hardware context */
static void crypto_sym_ccm_save_context(crypto_sym_ccm_context *ctx);

/* Helper function to build B0 block for CBC-MAC */
static int crypto_sym_ccm_build_b0(crypto_sym_ccm_context *ctx, const uint8_t *iv, uint8_t *b0);

/* Helper function to build CTR0 counter block */
static void crypto_sym_ccm_build_ctr0(crypto_sym_ccm_context *ctx, const uint8_t *iv, uint8_t *ctr0);

/* Helper function to try B0 initialization when both starts() and set_lengths() are called */
static int crypto_sym_ccm_try_init_b0_and_ctr0(crypto_sym_ccm_context *ctx);
static int crypto_sym_ccm_ctr_crypto(crypto_sym_ccm_context *ctx, uint8_t *output, size_t output_size);

/* Helper function to check block boundary condition */
static inline uint8_t crypto_sym_ccm_is_block_boundary(size_t input_len, uint32_t processed)
{
	size_t offset = processed % 16;
	size_t use_len = 16 - offset;

	if (use_len > input_len) {
		use_len = input_len;
	}

	/* Return 1 if we reach block boundary or this is the last data */
	return (use_len + offset == 16) ? 1 : 0;
}

/**
 * @brief Initialize CCM context
 */
void crypto_sym_ccm_init(crypto_engine_mode engine_mode, crypto_sym_ccm_context *ctx)
{
	/*
	 * Parameter validation and clear context
	 */
	if (ctx == NULL) {
		return;
	}

	/* Clear entire context structure */
	_memset((uint8_t *)ctx, 0, sizeof(crypto_sym_ccm_context));

	ctx->inited = CRYPTO_CTX_STARTED;

	/* Store engine mode in context */
	ctx->engine_mode = engine_mode;
}

/**
 * @brief Set CCM key
 */
int crypto_sym_ccm_setkey(crypto_sym_ccm_context *ctx,
						  uint8_t key_id,
						  uint32_t key_len_bits,
						  uint8_t *key_addr)
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

	/* Save key information in context */
	ctx->key_id = key_id;
	ctx->key_len_bits = key_len_bits;

	/* For software keys, copy the key to context */
	if (KM_IS_SW_KEY_NUM(key_id) && key_addr != NULL) {
		size_t key_len_bytes = key_len_bits / 8;
		_memcpy(ctx->key, key_addr, key_len_bytes);
	}

	return RTK_SUCCESS;
}

/**
 * @brief Free CCM context
 */
void crypto_sym_ccm_free(crypto_sym_ccm_context *ctx)
{
	/*
	 * Clear sensitive data, mark context invalid, clear entire ctx structure.
	 * No need to operate hardware registers.
	 */
	if (ctx == NULL) {
		return;
	}

	/* Clear all sensitive data and context structure */
	_memset((uint8_t *)ctx, 0, sizeof(crypto_sym_ccm_context));
}

/**
 * @brief Set processing lengths
 */
int crypto_sym_ccm_set_lengths(crypto_sym_ccm_context *ctx,
							   size_t total_ad_len,
							   size_t plaintext_len,
							   size_t tag_len)
{
	int ret = RTK_FAIL;

	if (ctx == NULL) {
		return _ERRNO_CRYPTO_NULL_POINTER;
	}

	/* Check if lengths already set - in this case, we should reject the call */
	if (ctx->state & CCM_STATE_LENGTHS_SET) {
		return _ERRNO_CRYPTO_TAG_OutRange;  /* Reuse appropriate error code */
	}

	/* Validate AAD length: total_ad_len < 0xFF00 */
	if (total_ad_len >= 0xFF00) {
		return _ERRNO_CRYPTO_AAD_LENGTH_OutRange;
	}

	/* Validate tag length (support CCM* zero tag length) */
	if (tag_len > 16 || (tag_len != 0 && (tag_len < 4 || tag_len % 2 != 0))) {
		return _ERRNO_CRYPTO_TAG_OutRange;
	}

	/* Additional validation: tag_len=0 only allowed in CCM* mode */
	/* Note: We cannot validate mode here since it's set later in starts() function */
	/* The actual validation will be done in crypto_sym_ccm_build_b0() */

	/* Save length parameters in context */
	ctx->aad_total_len = total_ad_len;
	ctx->msg_total_len = plaintext_len;
	ctx->tag_len = tag_len;

	/* Mark as lengths set */
	ctx->state |= CCM_STATE_LENGTHS_SET;

	/* Try to initialize B0 if starts() has already been called */
	ret = crypto_sym_ccm_try_init_b0_and_ctr0(ctx);
	if (ret != RTK_SUCCESS) {
		/* Rollback the state on error */
		ctx->state &= ~CCM_STATE_LENGTHS_SET;
		return ret;
	}

	return RTK_SUCCESS;
}

/* Helper function to try B0 initialization when both starts() and set_lengths() are called */
static int crypto_sym_ccm_try_init_b0_and_ctr0(crypto_sym_ccm_context *ctx)
{
	AES_TypeDef *AES = AES_Get_AES_Addr();
	u8 b0[16] = {0};
	u8 ctr0[16] = {0};
	u32 *b0_word = (u32 *)b0;
	u32 temp = 0;
	int ret = RTK_FAIL;
	u32 loopWait = AES_SLAVE_TIMEOUT;
	u32 i;

	/* Check if both starts() and set_lengths() have been called */
	if (!(ctx->state & CCM_STATE_STARTED) || !(ctx->state & CCM_STATE_LENGTHS_SET)) {
		return RTK_SUCCESS;  /* Not ready yet, that's OK */
	}

	/* Check if already initialized */
	if (ctx->state & CCM_STATE_B0_DONE) {
		return RTK_SUCCESS;
	}

	/* Validate message length based on q parameter (now that we have both q and msg_total_len) */
	if (ctx->msg_total_len >= (1ULL << (8 * ctx->q))) {
		return _ERRNO_CRYPTO_MSG_OutRange;
	}

	AES_lock_mutex();

	/* Set software key if needed */
	if (KM_IS_SW_KEY_NUM(ctx->key_id)) {
		CRYPTO_CHK(crypto_km_set_sw_key(ctx->key_id, ctx->key_len_bits, ctx->key), ret);
	}

	/* Load key to AES engine */
	CRYPTO_CHK(crypto_km_load_key(KM_AES_ENGINE, ctx->key_id, ctx->key_len_bits), _ERRNO_CRYPTO_KEY_LOAD_ERR);

	/* Clear TAG registers */
	for (i = 0; i < 4; i++) {
		AES->AES_TAG_x[i] = 0;
	}

	/* Build B0 block for CBC-MAC using saved nonce */
	CRYPTO_CHK(crypto_sym_ccm_build_b0(ctx, ctx->nonce, b0), ret);

	/* Configure AES engine for CBC-MAC */
	AES->AES_CIPHER_LEN = AES_BLOCK_SIZE_BIT;
	temp = 0;
	temp |= AES_CIPHER_MODE(CIPHER_MODE_CBC_MAC);
	temp |= AES_BIT_EN_DE;  /* Encrypt for MAC calculation */
	SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);

	/* Set SM4 core selection if needed */
	if (ctx->engine_mode == CRYPTO_SM4_MODE) {
		temp |= AES_BIT_CORE_SELECT;
	}

	AES->AES_CONFIG = temp;

	/* Input B0 block */
	for (i = 0; i < 4; i++) {
		AES->AES_CPU_DATAIN_x[i] = b0_word[i];
	}

	/* Wait for operation completion */
	WAIT_FOR_AES_SLAVE_MAC_COMPLETION();

	/* Build CTR0 counter block and set IV using saved nonce */
	crypto_sym_ccm_build_ctr0(ctx, ctx->nonce, ctr0);
	AES_SetIv(ctr0);

	// crypto_dump_word(ctr0, 16, "new_ctr0");

	// crypto_dump_word(&AES->AES_TAG_x[0], 16, "new_after_b0_process_tag");

	/* Save hardware context */
	crypto_sym_ccm_save_context(ctx);

	/* Mark B0 initialization as complete */
	ctx->state |= CCM_STATE_B0_DONE;

	ret = RTK_SUCCESS;

exit:
	/* Clear interrupt flag */
	AES->AES_CHN0_AES_INT_CLEAR &= AES_BIT_INT_CLEAR;
	AES_unlock_mutex();
	return ret;
}

/* Helper function to restore hardware context */
static int crypto_sym_ccm_restore_context_and_key(crypto_sym_ccm_context *ctx)
{
	AES_TypeDef *AES = AES_Get_AES_Addr();
	u32 i;
	u32 *tag_word = (u32 *)ctx->tag;
	u32 *iv_word = (u32 *)ctx->iv;
	int ret = RTK_FAIL;

	/* Restore TAG registers */
	for (i = 0; i < 4; i++) {
		AES->AES_TAG_x[i] = tag_word[i];
	}

	/* Restore IV registers */
	for (i = 0; i < 4; i++) {
		AES->AES_IV_x[i] = iv_word[i];
	}

	/* Set software key if needed */
	if (KM_IS_SW_KEY_NUM(ctx->key_id)) {
		CRYPTO_CHK(crypto_km_set_sw_key(ctx->key_id, ctx->key_len_bits, ctx->key), ret);
	}

	/* Load key to AES engine */
	CRYPTO_CHK(crypto_km_load_key(KM_AES_ENGINE, ctx->key_id, ctx->key_len_bits), _ERRNO_CRYPTO_KEY_LOAD_ERR);

exit:
	return ret;
}

/* Helper function to save hardware context */
static void crypto_sym_ccm_save_context(crypto_sym_ccm_context *ctx)
{
	AES_TypeDef *AES = AES_Get_AES_Addr();
	u32 i;
	u32 *tag_word = (u32 *)ctx->tag;
	u32 *iv_word = (u32 *)ctx->iv;

	/* Save TAG registers */
	for (i = 0; i < 4; i++) {
		tag_word[i] = AES->AES_TAG_x[i];
	}

	/* Save IV registers */
	for (i = 0; i < 4; i++) {
		iv_word[i] = AES->AES_IV_x[i];
	}
}

/* Helper function to build B0 block for CBC-MAC */
static int crypto_sym_ccm_build_b0(crypto_sym_ccm_context *ctx, const uint8_t *iv, uint8_t *b0)
{
	u32 i;

	/* B0[0] = flags: Bits 7-6: AAD present, Bits 5-3: M field, Bits 2-0: q-1 */
	/* Handle tag_len=0 case for CCM* mode */
	u8 m_field;
	if (ctx->tag_len == 0) {
		/* For CCM* mode with tag_len=0, M field should be 0 */
		if (ctx->ccm_mode == CCM_MODE_STAR_ENCRYPT || ctx->ccm_mode == CCM_MODE_STAR_DECRYPT) {
			m_field = 0;
		} else {
			/* Invalid: tag_len=0 only allowed in CCM* mode */
			return _ERRNO_CRYPTO_TAG_OutRange;
		}
	} else {
		m_field = (ctx->tag_len - 2) / 2;
	}

	b0[0] = ((ctx->aad_total_len > 0) ? 0x40 : 0x00) |
			((m_field & 0x07) << 3) |
			(ctx->q - 1);

	/* Copy IV/nonce */
	_memcpy(&b0[1], iv, 15 - ctx->q);

	// RTK_LOG(NOTAG, RTK_LOG_INFO, "[%s] ctx->msg_total_len:%u\n", __func__, ctx->msg_total_len);
	/* Encode message length in q bytes (big-endian) */
	for (i = 0; i < ctx->q; i++) {
		b0[15 - i] = (ctx->msg_total_len >> (i * 8)) & 0xFF;
	}

	return RTK_SUCCESS;
}

/* Helper function to build CTR0 counter block */
static void crypto_sym_ccm_build_ctr0(crypto_sym_ccm_context *ctx, const uint8_t *iv, uint8_t *ctr0)
{
	/* CTR0[0] = flags: Bits 2-0: q-1, other bits = 0 */
	ctr0[0] = ctx->q - 1;

	/* Copy IV/nonce */
	_memcpy(&ctr0[1], iv, 15 - ctx->q);

	/* Zero the counter field (q bytes) */
	_memset(&ctr0[15 - ctx->q + 1], 0, ctx->q);

	/* Counter starts at 1 */
	ctr0[15] = 1;
}

/**
 * @brief Start CCM operation
 */
int crypto_sym_ccm_starts(crypto_sym_ccm_context *ctx,
						  uint8_t ccm_mode,
						  const uint8_t *iv,
						  size_t iv_len)
{
	int ret = RTK_FAIL;

	if (ctx == NULL || iv == NULL) {
		return _ERRNO_CRYPTO_NULL_POINTER;
	}

	/* Validate IV length and calculate q parameter */
	if (iv_len < 7 || iv_len > 13) {
		return _ERRNO_CRYPTO_IV_OutRange;
	}

	if (ctx->inited != CRYPTO_CTX_STARTED) {
		return _ERRNO_CRYPTO_ENGINE_NOT_INIT;
	}

	/* Validate mode */
	if (ccm_mode != CCM_MODE_ENCRYPT && ccm_mode != CCM_MODE_DECRYPT &&
		ccm_mode != CCM_MODE_STAR_ENCRYPT && ccm_mode != CCM_MODE_STAR_DECRYPT) {
		return _ERRNO_CRYPTO_MODE_ERR;
	}

	/* Check if already started - in this case, we should reject the call */
	if (ctx->state & CCM_STATE_STARTED) {
		return _ERRNO_CRYPTO_MODE_ERR;  /* Reuse appropriate error code */
	}

	/* Save parameters for deferred initialization */
	ctx->ccm_mode = ccm_mode;
	ctx->q = 15 - iv_len;
	ctx->nonce_len = iv_len;
	_memcpy(ctx->nonce, iv, iv_len);

	/* Mark as started */
	ctx->state |= CCM_STATE_STARTED;

	/* Try to initialize B0 if set_lengths() has already been called */
	ret = crypto_sym_ccm_try_init_b0_and_ctr0(ctx);
	if (ret != RTK_SUCCESS) {
		/* Rollback the state on error */
		ctx->state &= ~CCM_STATE_STARTED;
		return ret;
	}

	return RTK_SUCCESS;
}

/* Helper function to process remaining AAD buffer */
static int crypto_sym_ccm_update_ad_final(crypto_sym_ccm_context *ctx)
{
	AES_TypeDef *AES = AES_Get_AES_Addr();
	u8 padded_block[16] = {0};
	u32 *padded_word = (u32 *)padded_block;
	u32 temp = 0;
	int ret = RTK_FAIL;
	u32 loopWait = AES_SLAVE_TIMEOUT;
	u32 i;

	if (ctx->aad_buffer_len == 0) {
		return RTK_SUCCESS;
	}

	/* Pad the remaining AAD data with zeros */
	_memcpy(padded_block, ctx->aad_buffer, ctx->aad_buffer_len);

	AES->AES_CIPHER_LEN = AES_BLOCK_SIZE_BIT;
	/* Configure AES engine for CBC-MAC slave mode */
	temp = 0;
	temp |= AES_CIPHER_MODE(CIPHER_MODE_CBC_MAC);
	temp |= AES_BIT_EN_DE;
	SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);

	/* Set SM4 core selection if needed */
	if (ctx->engine_mode == CRYPTO_SM4_MODE) {
		temp |= AES_BIT_CORE_SELECT;
	}

	AES->AES_CONFIG = temp;

	/* Input the padded AAD block */
	for (i = 0; i < 4; i++) {
		AES->AES_CPU_DATAIN_x[i] = padded_word[i];
	}

	/* Wait for operation completion */
	WAIT_FOR_AES_SLAVE_MAC_COMPLETION();

	/* Clear AAD buffer */
	ctx->aad_buffer_len = 0;
	_memset(ctx->aad_buffer, 0, sizeof(ctx->aad_buffer));

	ret = RTK_SUCCESS;

exit:
	AES->AES_CHN0_AES_INT_CLEAR &= AES_BIT_INT_CLEAR;
	return ret;
}

/**
 * @brief Process authentication data
 */
int crypto_sym_ccm_update_ad(crypto_sym_ccm_context *ctx,
							 const uint8_t *ad,
							 size_t ad_len)
{
	AES_TypeDef *AES = AES_Get_AES_Addr();
	u8 block[16] = {0};
	u32 *block_word = (u32 *)block;
	u32 temp = 0;
	int ret = RTK_FAIL;
	u32 loopWait = AES_SLAVE_TIMEOUT;
	u32 processed = 0;
	u32 i;

	if (ctx == NULL || (ad == NULL && ad_len > 0)) {
		return _ERRNO_CRYPTO_NULL_POINTER;
	}

	if (ad_len == 0) {
		return RTK_SUCCESS;
	}

	AES_lock_mutex();

	/* Restore TAG to hardware registers */
	CRYPTO_CHK(crypto_sym_ccm_restore_context_and_key(ctx), ret);

	/* Check if B0 initialization is complete */
	if (!(ctx->state & CCM_STATE_B0_DONE)) {
		/* Try to initialize B0 in case both starts() and set_lengths() are called but not yet processed */
		ret = crypto_sym_ccm_try_init_b0_and_ctr0(ctx);
		if (ret != RTK_SUCCESS) {
			goto exit;
		}
		/* If still not ready, that means one of starts() or set_lengths() hasn't been called yet */
		if (!(ctx->state & CCM_STATE_B0_DONE)) {
			ret = _ERRNO_CRYPTO_MODE_ERR;
			goto exit;
		}
	}

	if (ctx->aad_processed_flag == FALSE) {
		ctx->aad_buffer[0] = ctx->aad_total_len >> 8;
		ctx->aad_buffer[1] = ctx->aad_total_len;
		ctx->aad_buffer_len = 2;
		ctx->aad_processed_flag = TRUE;
	}

	/* Process existing AAD buffer first */
	if (ctx->aad_buffer_len > 0) {
		u32 remaining_space = 16 - ctx->aad_buffer_len;
		u32 copy_len = (ad_len < remaining_space) ? ad_len : remaining_space;

		/* Copy data to fill the buffer */
		_memcpy(ctx->aad_buffer + ctx->aad_buffer_len, ad, copy_len);
		ctx->aad_buffer_len += copy_len;
		processed += copy_len;

		/* If buffer is full, process it */
		if (ctx->aad_buffer_len == 16) {
			/* config AES engine, set to slave mode */
			AES->AES_CIPHER_LEN = AES_BLOCK_SIZE_BIT;

			/* Configure AES engine for CBC-MAC slave mode */
			temp = 0;
			temp |= AES_CIPHER_MODE(CIPHER_MODE_CBC_MAC);
			temp |= AES_BIT_EN_DE;
			SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);

			/* Set SM4 core selection if needed */
			if (ctx->engine_mode == CRYPTO_SM4_MODE) {
				temp |= AES_BIT_CORE_SELECT;
			}
			AES->AES_CONFIG = temp;

			/* Input the complete AAD block */
			block_word = (u32 *)ctx->aad_buffer;
			for (i = 0; i < 4; i++) {
				AES->AES_CPU_DATAIN_x[i] = block_word[i];
			}

			/* Wait for operation completion */
			WAIT_FOR_AES_SLAVE_MAC_COMPLETION();

			/* Clear AAD buffer */
			ctx->aad_buffer_len = 0;
			_memset(ctx->aad_buffer, 0, sizeof(ctx->aad_buffer));
		}
	}

	// crypto_dump_word(&AES->AES_TAG_x[0], 16, "new_before_aad_tag");
	// crypto_dump_word(ad, ad_len, "ad");

	/* Process remaining full 16-byte blocks */
	if (processed + 16 <= ad_len) {
		/* Configure AES engine for CBC-MAC slave mode */
		AES->AES_CIPHER_LEN = AES_BLOCK_SIZE_BIT;

		temp = 0;
		temp |= AES_CIPHER_MODE(CIPHER_MODE_CBC_MAC);
		temp |= AES_BIT_EN_DE;
		temp |= AES_BIT_FIRST_BLOCK;
		SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);
		/* Set SM4 core selection if needed */
		if (ctx->engine_mode == CRYPTO_SM4_MODE) {
			temp |= AES_BIT_CORE_SELECT;
		}
		AES->AES_CONFIG = temp;

		while (processed + 16 <= ad_len) {
			/* Input the AAD block */
			block_word = (u32 *)(ad + processed);
			for (i = 0; i < 4; i++) {
				AES->AES_CPU_DATAIN_x[i] = block_word[i];
			}

			processed += 16;
		}

		/* Wait for operation completion */
		WAIT_FOR_AES_SLAVE_MAC_COMPLETION();
	}

	/* Save remaining data to buffer */
	if (processed < ad_len) {
		u32 remaining = ad_len - processed;
		_memcpy(ctx->aad_buffer, ad + processed, remaining);
		ctx->aad_buffer_len = remaining;
	}


	// crypto_dump_word(&AES->AES_TAG_x[0], 16, "new_after_aad_tag");

	/* Save TAG to context */
	crypto_sym_ccm_save_context(ctx);

	ret = RTK_SUCCESS;

exit:
	AES->AES_CHN0_AES_INT_CLEAR &= AES_BIT_INT_CLEAR;
	AES_unlock_mutex();
	return ret;
}

/* Helper function to process remaining message buffer */
static int crypto_sym_ccm_update_final(crypto_sym_ccm_context *ctx)
{
	AES_TypeDef *AES = AES_Get_AES_Addr();
	u8 padded_block[16] = {0};
	u32 *padded_word = (u32 *)padded_block;
	u32 temp = 0;
	int ret = RTK_FAIL;
	u32 loopWait = AES_SLAVE_TIMEOUT;
	u32 i;

	if (ctx->msg_buffer_len == 0) {
		return RTK_SUCCESS;
	}

	if (ctx->ccm_mode == CCM_MODE_ENCRYPT || ctx->ccm_mode == CCM_MODE_STAR_ENCRYPT) {
		/* Pad the remaining message data with zeros */
		_memcpy(padded_block, ctx->msg_buffer, ctx->msg_buffer_len);
	} else {
		/* Dec mode use PT */
		CRYPTO_CHK(crypto_sym_ccm_ctr_crypto(ctx, padded_block, ctx->msg_buffer_len), ret);
	}

	/* Configure AES engine for CBC-MAC mode to finalize authentication */
	AES->AES_CIPHER_LEN = AES_BLOCK_SIZE_BIT;
	temp = 0;
	temp |= AES_CIPHER_MODE(CIPHER_MODE_CBC_MAC);
	temp |= AES_BIT_EN_DE;
	SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);
	/* Set SM4 core selection if needed */
	if (ctx->engine_mode == CRYPTO_SM4_MODE) {
		temp |= AES_BIT_CORE_SELECT;
	}
	AES->AES_CONFIG = temp;

	/* Set cipher length for this final block */
	AES->AES_CIPHER_LEN = AES_BLOCK_SIZE_BIT;

	/* Input the padded message block for final MAC calculation */
	for (i = 0; i < 4; i++) {
		AES->AES_CPU_DATAIN_x[i] = padded_word[i];
	}

	/* Wait for operation completion */
	WAIT_FOR_AES_SLAVE_MAC_COMPLETION();

	/* Clear message buffer */
	ctx->msg_buffer_len = 0;
	_memset(ctx->msg_buffer, 0, sizeof(ctx->msg_buffer));

	ret = RTK_SUCCESS;

exit:
	AES->AES_CHN0_AES_INT_CLEAR &= AES_BIT_INT_CLEAR;
	return ret;
}

/**
 * @brief caculate output, not update iv and tag
 */
static int crypto_sym_ccm_ctr_crypto(crypto_sym_ccm_context *ctx, uint8_t *output, size_t output_size)
{
	AES_TypeDef *AES = AES_Get_AES_Addr();
	u8 temp_arr[16] = {0};
	u32 *temp_arr_word = (u32 *)temp_arr;
	int ret = RTK_FAIL;
	u32 loopWait = AES_SLAVE_TIMEOUT;
	u8 offset = ctx->msg_buffer_len - output_size;
	u32 i = 0;

	/* save iv */
	if (ctx->msg_buffer_len != AES_BLOCK_SIZE) {
		for (i = 0; i < 4; i++) {
			temp_arr_word[i] = AES->AES_IV_x[i];
		}
	}

	/* config AES engine, set to slave mode */
	AES->AES_CIPHER_LEN = AES_BLOCK_SIZE_BIT;

	/* Configure AES engine for CCM slave mode */
	u32 temp = 0;
	temp |= AES_CIPHER_MODE(CIPHER_MODE_CTR);
	if (ctx->ccm_mode == CCM_MODE_ENCRYPT || ctx->ccm_mode == CCM_MODE_STAR_ENCRYPT) {
		temp |= AES_BIT_EN_DE;
	}
	SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);
	temp |= AES_BIT_FIRST_BLOCK;
	/* Set SM4 core selection if needed */
	if (ctx->engine_mode == CRYPTO_SM4_MODE) {
		temp |= AES_BIT_CORE_SELECT;
	}
	AES->AES_CONFIG = temp;

	/* Input the complete message block */
	u32 *block_word = (u32 *)ctx->msg_buffer;
	for (i = 0; i < 4; i++) {
		AES->AES_CPU_DATAIN_x[i] = block_word[i];
	}

	/* Wait for operation completion */
	WAIT_FOR_AES_SLAVE_CIPHER_COMPLETION();

	/* restore iv and read data out */
	for (i = 0; i < 4; i++) {
		if (ctx->msg_buffer_len != AES_BLOCK_SIZE) {
			AES->AES_IV_x[i] = temp_arr_word[i];
		}
		temp_arr_word[i] = AES->AES_CPU_DATAOUT_x[i];
	}
	_memcpy(output, temp_arr + offset, output_size);

exit:
	return ret;
}

/**
 * @brief Process message data
 */
int crypto_sym_ccm_update(crypto_sym_ccm_context *ctx,
						  const uint8_t *input, size_t input_len,
						  uint8_t *output, size_t output_size,
						  size_t *output_len)
{
	AES_TypeDef *AES = AES_Get_AES_Addr();
	u8 block[16] = {0};
	u32 *block_word = (u32 *)block;
	u32 temp = 0;
	int ret = RTK_FAIL;
	u32 loopWait = AES_SLAVE_TIMEOUT;
	u32 processed = 0;
	u32 i;

	if (ctx == NULL || (input == NULL && input_len > 0) ||
		output == NULL || output_len == NULL) {
		return _ERRNO_CRYPTO_NULL_POINTER;
	}

	if (input_len == 0) {
		*output_len = 0;
		return RTK_SUCCESS;
	}

	if (output_size < input_len) {
		return _ERRNO_CRYPTO_BUFFER_TOO_SMALL;
	}

	/* Set output length same with input */
	*output_len = input_len;

	AES_lock_mutex();

	/* Restore TAG and IV to hardware registers */
	CRYPTO_CHK(crypto_sym_ccm_restore_context_and_key(ctx), ret);

	/* Check if B0 initialization is complete */
	if (!(ctx->state & CCM_STATE_B0_DONE)) {
		/* Try to initialize B0 in case both starts() and set_lengths() are called but not yet processed */
		ret = crypto_sym_ccm_try_init_b0_and_ctr0(ctx);
		if (ret != RTK_SUCCESS) {
			goto exit;
		}
		/* If still not ready, that means one of starts() or set_lengths() hasn't been called yet */
		if (!(ctx->state & CCM_STATE_B0_DONE)) {
			ret = _ERRNO_CRYPTO_MODE_ERR;
			goto exit;
		}
	}

	/* Check if AAD buffer is not empty, process it first */
	if (ctx->aad_buffer_len > 0) {
		CRYPTO_CHK(crypto_sym_ccm_update_ad_final(ctx), ret);
	}

	/* Process existing message buffer first */
	if (ctx->msg_buffer_len > 0) {
		u32 remaining_space = 16 - ctx->msg_buffer_len;
		u32 copy_len = (input_len < remaining_space) ? input_len : remaining_space;

		/* Copy data to fill the buffer */
		_memcpy(ctx->msg_buffer + ctx->msg_buffer_len, input, copy_len);
		ctx->msg_buffer_len += copy_len;
		processed += copy_len;

		/* Only ctr crypto, no iv / tag update */
		CRYPTO_CHK(crypto_sym_ccm_ctr_crypto(ctx, output, copy_len), ret);
		output += copy_len;

		/* If buffer is full, update TAG */
		if (ctx->msg_buffer_len == 16) {
			/* config AES engine, set to slave mode */
			AES->AES_CIPHER_LEN = AES_BLOCK_SIZE_BIT;

			/* Configure AES engine for CBC-MAC slave mode */
			temp = 0;
			temp |= AES_CIPHER_MODE(CIPHER_MODE_CBC_MAC);
			temp |= AES_BIT_EN_DE;
			SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);
			temp |= AES_BIT_FIRST_BLOCK;
			/* Set SM4 core selection if needed */
			if (ctx->engine_mode == CRYPTO_SM4_MODE) {
				temp |= AES_BIT_CORE_SELECT;
			}
			AES->AES_CONFIG = temp;

			/* Input the complete message block */
			if (ctx->ccm_mode == CCM_MODE_ENCRYPT || ctx->ccm_mode == CCM_MODE_STAR_ENCRYPT) {
				block_word = (u32 *)ctx->msg_buffer;
			} else {
				block_word = (u32 *)(output - AES_BLOCK_SIZE);
			}

			for (i = 0; i < 4; i++) {
				AES->AES_CPU_DATAIN_x[i] = block_word[i];
			}

			/* Wait for operation completion */
			WAIT_FOR_AES_SLAVE_MAC_COMPLETION();

			/* Clear message buffer */
			ctx->msg_buffer_len = 0;
			_memset(ctx->msg_buffer, 0, sizeof(ctx->msg_buffer));
		}
	}

	/* Process remaining full 16-byte blocks using DMA mode */
	u32 remaining_data = input_len - processed;
	if (remaining_data > 0) {
		u32 dma_data = remaining_data - (remaining_data % 16);  /* Round down to 16-byte boundary */

		if (dma_data > 0) {
			/* Initialize DMA */
			AES_DMA_Init();

			/* Configure AES engine for CCM DMA mode */
			temp = 0;
			temp |= AES_BIT_ENGINE_MODE;  /* DMA mode */
			temp |= AES_CIPHER_MODE(CIPHER_MODE_CCM);
			if (ctx->ccm_mode == CCM_MODE_ENCRYPT || ctx->ccm_mode == CCM_MODE_STAR_ENCRYPT) {
				temp |= AES_BIT_EN_DE;
			}
			SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);
			/* Set SM4 core selection if needed */
			if (ctx->engine_mode == CRYPTO_SM4_MODE) {
				temp |= AES_BIT_CORE_SELECT;
			}
			AES->AES_CONFIG = temp;

			/* Set cipher length */
			AES->AES_CIPHER_LEN = dma_data * 8;

			/* Process with DMA using ROM function */
			CRYPTO_CHK(AES_ProcessDma(input + processed, dma_data, output), _ERRNO_CRYPTO_DMA_ERR);

			processed += dma_data;
			output += dma_data;
		}
	}

	/* Save remaining data to buffer */
	if (processed < input_len) {
		u32 remaining = input_len - processed;
		_memcpy(ctx->msg_buffer, input + processed, remaining);
		ctx->msg_buffer_len = remaining;

		/* Only ctr crypto, no iv tag update */
		CRYPTO_CHK(crypto_sym_ccm_ctr_crypto(ctx, output, remaining), ret);
		output += remaining;
	}

	// crypto_dump_word(&AES->AES_TAG_x[0], 16, "new_after_msg_tag");

	/* Save TAG and IV to context */
	crypto_sym_ccm_save_context(ctx);

	ret = RTK_SUCCESS;

exit:
	AES->AES_CHN0_AES_INT_CLEAR &= AES_BIT_INT_CLEAR;
	AES_unlock_mutex();
	return ret;
}

/**
 * @brief Finish CCM operation and get tag
 */
int crypto_sym_ccm_finish(crypto_sym_ccm_context *ctx,
						  uint8_t *tag,
						  size_t tag_len)
{
	AES_TypeDef *AES = AES_Get_AES_Addr();
	u32 tag_word[4];
	u8 *tag_byte = (u8 *)tag_word;
	u32 temp = 0;
	int ret = RTK_FAIL;
	u32 loopWait = AES_SLAVE_TIMEOUT;
	u32 i;

	if (ctx == NULL || tag == NULL) {
		return _ERRNO_CRYPTO_NULL_POINTER;
	}

	if (tag_len > 16) {
		return _ERRNO_CRYPTO_TAG_OutRange;
	}

	AES_lock_mutex();

	/* Restore TAG and IV to hardware registers */
	CRYPTO_CHK(crypto_sym_ccm_restore_context_and_key(ctx), ret);

	/* Check if B0 initialization is complete */
	if (!(ctx->state & CCM_STATE_B0_DONE)) {
		/* Try to initialize B0 in case both starts() and set_lengths() are called but not yet processed */
		ret = crypto_sym_ccm_try_init_b0_and_ctr0(ctx);
		if (ret != RTK_SUCCESS) {
			goto exit;
		}
		/* If still not ready, that means one of starts() or set_lengths() hasn't been called yet */
		if (!(ctx->state & CCM_STATE_B0_DONE)) {
			ret = _ERRNO_CRYPTO_MODE_ERR;
			goto exit;
		}
	}

	/* Check if AAD buffer is not empty, process it first */
	if (ctx->aad_buffer_len > 0) {
		CRYPTO_CHK(crypto_sym_ccm_update_ad_final(ctx), ret);
	}

	/* Check if message buffer is not empty, process it */
	if (ctx->msg_buffer_len > 0) {
		CRYPTO_CHK(crypto_sym_ccm_update_final(ctx), ret);
	}

	/* Final tag calculation using CCM slave mode */
	AES->AES_CIPHER_LEN = 0;
	temp = 0;
	temp |= AES_CIPHER_MODE(CIPHER_MODE_CCM);
	temp |= AES_BIT_AEAD_LAST;  /* Set AEAD_LAST for final tag calculation */
	temp |= AES_BIT_FIRST_BLOCK;
	SET_AES_CONFIG_KEY_LEN(ctx->key_len_bits, temp);
	if (ctx->ccm_mode == CCM_MODE_ENCRYPT || ctx->ccm_mode == CCM_MODE_STAR_ENCRYPT) {
		temp |= AES_BIT_EN_DE;
	}
	/* Set SM4 core selection if needed */
	if (ctx->engine_mode == CRYPTO_SM4_MODE) {
		temp |= AES_BIT_CORE_SELECT;
	}
	AES->AES_CONFIG = temp;

	/* Wait for final tag calculation completion */
	WAIT_FOR_AES_SLAVE_CIPHER_COMPLETION();

	/* Read the final tag from hardware registers */
	for (i = 0; i < 4; i++) {
		tag_word[i] = AES->AES_TAG_x[i];
	}

	/* Copy requested tag length to output */
	for (i = 0; i < tag_len; i++) {
		tag[i] = tag_byte[i];
	}

	/* Reset streaming state on success path, mirroring crypto_sym_gcm_finish().
	 * Key material (key/key_id/key_len_bits/engine_mode) and inited are
	 * preserved so the context can be reused with starts() directly. */
	ctx->state = 0;
	ctx->aad_total_len = 0;
	ctx->msg_total_len = 0;
	ctx->tag_len = 0;
	ctx->q = 0;
	ctx->ccm_mode = 0;
	ctx->aad_processed_flag = 0;
	ctx->aad_buffer_len = 0;
	ctx->msg_buffer_len = 0;
	_memset(ctx->aad_buffer, 0, sizeof(ctx->aad_buffer));
	_memset(ctx->msg_buffer, 0, sizeof(ctx->msg_buffer));
	_memset(ctx->tag, 0, sizeof(ctx->tag));
	_memset(ctx->iv, 0, sizeof(ctx->iv));
	_memset(ctx->nonce, 0, sizeof(ctx->nonce));
	ctx->nonce_len = 0;

	ret = RTK_SUCCESS;

exit:
	/* Clear interrupt flag and clean up */
	AES->AES_CONFIG &= ~AES_BIT_AEAD_LAST;
	AES->AES_CHN0_AES_INT_CLEAR &= AES_BIT_INT_CLEAR;
	AES_unlock_mutex();

	return ret;
}
