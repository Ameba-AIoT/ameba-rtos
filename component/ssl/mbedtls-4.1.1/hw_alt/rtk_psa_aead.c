/**
 * *****************************************************************************
 * @file    rtk_psa_aead.c
 * @brief   PSA transparent driver for the Realtek AES-GCM / AES-CCM engine.
 * *****************************************************************************
 * @attention
 *
 * @note    Unlike the raw AES entry points (see rtk_psa_cipher.c), the GCM/CCM
 *          hardware layer keeps the key inside its own context and reloads it
 *          into the key-management slot on every operation
 *          (crypto_sym_gcm.c:115, :247, :460, :636). The slot therefore does
 *          not have to stay claimed between calls, which is what makes a full
 *          multipart driver possible here.
 *
 * @note    Locking strategy: IPC_SEM_CRYPTO_AES_SW_KEY is taken for each
 *          individual HAL call that reaches AES_ProcessDma (set_nonce /
 *          set_lengths(CCM) / update_ad / update / finish). IPC_SEMTake
 *          invokes taskENTER_CRITICAL internally, preventing RTOS task
 *          switches between DCache_CleanInvalidate and DCache_Invalidate
 *          inside AES_ProcessDma. Without this, a preempted task could
 *          dirty the DMA output buffer before the CPU re-reads it, causing
 *          silent data corruption (same issue fixed in 3.6.5 gcm_alt.c /
 *          ccm_alt.c). The semaphore is NOT held across PSA multipart calls
 *          — each call takes and releases it independently, so the hold time
 *          is bounded to one HAL operation.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#include "rtk_psa_aead.h"
#include "rtk_psa_error.h"

#include "mbedtls/constant_time.h"

#include "crypto_aes_gcm.h"
#include "crypto_aes_ccm.h"
#include "ameba_key_management.h"
#include "ameba_sema_rom.h"

#include <string.h>

_Static_assert(RTK_PSA_AEAD_CONTEXT_SIZE >= sizeof(crypto_aes_gcm_context),
			   "RTK_PSA_AEAD_CONTEXT_SIZE too small for crypto_aes_gcm_context");
_Static_assert(RTK_PSA_AEAD_CONTEXT_SIZE >= sizeof(crypto_aes_ccm_context),
			   "RTK_PSA_AEAD_CONTEXT_SIZE too small for crypto_aes_ccm_context");

#define RTK_GCM_CTX(op) ((crypto_aes_gcm_context *) (op)->hw_ctx)
#define RTK_CCM_CTX(op) ((crypto_aes_ccm_context *) (op)->hw_ctx)

#define RTK_AEAD_IS_GCM(alg) (PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg) == PSA_ALG_GCM)
#define RTK_AEAD_IS_CCM(alg) (PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg) == PSA_ALG_CCM)

static int rtk_aead_key_slot(void)
{
	return (TrustZone_IsSecure() == 0) ? KM_AES_KEY_NS_SW1 : KM_AES_KEY_S_SW1;
}

/* Accept only AES-GCM / AES-CCM with a key size the engine implements.
 * Everything else (ChaCha20-Poly1305, SM4 keys, ...) falls through to the
 * software driver. */
static psa_status_t rtk_aead_check(const psa_key_attributes_t *attributes,
								   size_t key_buffer_size, psa_algorithm_t alg)
{
	if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	if (!RTK_AEAD_IS_GCM(alg) && !RTK_AEAD_IS_CCM(alg)) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	if (key_buffer_size != 16 && key_buffer_size != 24 && key_buffer_size != 32) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	return PSA_SUCCESS;
}

static psa_status_t rtk_aead_setup(rtk_psa_aead_operation_t *operation,
								   const psa_key_attributes_t *attributes,
								   const uint8_t *key_buffer, size_t key_buffer_size,
								   psa_algorithm_t alg, uint8_t is_encrypt)
{
	psa_status_t status;
	size_t tag_length = PSA_AEAD_TAG_LENGTH(psa_get_key_type(attributes),
										   psa_get_key_bits(attributes), alg);
	int ret;

	status = rtk_aead_check(attributes, key_buffer_size, alg);
	if (status != PSA_SUCCESS) {
		return status;
	}

	if (tag_length < 4 || tag_length > 16) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	memset(operation, 0, sizeof(*operation));
	operation->alg = alg;
	operation->tag_length = tag_length;
	operation->is_encrypt = is_encrypt;

	if (RTK_AEAD_IS_GCM(alg)) {
		crypto_aes_gcm_init(RTK_GCM_CTX(operation));
		ret = crypto_aes_gcm_setkey(RTK_GCM_CTX(operation), (u8) rtk_aead_key_slot(),
									(u8 *) key_buffer, (u32) key_buffer_size * 8);
	} else {
		crypto_aes_ccm_init(RTK_CCM_CTX(operation));
		ret = crypto_aes_ccm_setkey(RTK_CCM_CTX(operation), (u8) rtk_aead_key_slot(),
									(u32) key_buffer_size * 8, (u8 *) key_buffer);
	}

	return rtk_hw_to_psa_error(ret);
}

/* -------------------------------------------------------------------------
 * Single-shot entry points
 * ---------------------------------------------------------------------- */

static psa_status_t rtk_aead_one_shot(const psa_key_attributes_t *attributes,
									  const uint8_t *key_buffer, size_t key_buffer_size,
									  psa_algorithm_t alg, uint8_t is_encrypt,
									  const uint8_t *nonce, size_t nonce_length,
									  const uint8_t *ad, size_t ad_length,
									  const uint8_t *input, size_t input_length,
									  uint8_t *output, uint8_t *tag)
{
	rtk_psa_aead_operation_t operation;
	psa_status_t status;
	size_t out_length = 0;
	size_t tag_out_length = 0;

	status = rtk_aead_setup(&operation, attributes, key_buffer, key_buffer_size,
						    alg, is_encrypt);
	if (status != PSA_SUCCESS) {
		return status;
	}

	status = rtk_transparent_aead_set_nonce(&operation, nonce, nonce_length);
	if (status != PSA_SUCCESS) {
		goto exit;
	}

	/* CCM needs the totals up front to build the B0 block; GCM tolerates it. */
	status = rtk_transparent_aead_set_lengths(&operation, ad_length, input_length);
	if (status != PSA_SUCCESS) {
		goto exit;
	}

	if (ad_length != 0) {
		status = rtk_transparent_aead_update_ad(&operation, ad, ad_length);
		if (status != PSA_SUCCESS) {
			goto exit;
		}
	}

	if (input_length != 0) {
		status = rtk_transparent_aead_update(&operation, input, input_length,
											output, input_length, &out_length);
		if (status != PSA_SUCCESS) {
			goto exit;
		}
	}

	status = rtk_transparent_aead_finish(&operation, NULL, 0, &out_length,
										 tag, operation.tag_length, &tag_out_length);

exit:
	(void) rtk_transparent_aead_abort(&operation);
	return status;
}

psa_status_t rtk_transparent_aead_encrypt(
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg,
	const uint8_t *nonce, size_t nonce_length,
	const uint8_t *additional_data, size_t additional_data_length,
	const uint8_t *plaintext, size_t plaintext_length,
	uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length)
{
	psa_status_t status;
	size_t tag_length = PSA_AEAD_TAG_LENGTH(psa_get_key_type(attributes),
											psa_get_key_bits(attributes), alg);

	status = rtk_aead_check(attributes, key_buffer_size, alg);
	if (status != PSA_SUCCESS) {
		return status;
	}

	/* PSA appends the tag to the ciphertext. */
	if (ciphertext_size < plaintext_length + tag_length) {
		return PSA_ERROR_BUFFER_TOO_SMALL;
	}

	status = rtk_aead_one_shot(attributes, key_buffer, key_buffer_size, alg, 1,
							   nonce, nonce_length,
							   additional_data, additional_data_length,
							   plaintext, plaintext_length,
							   ciphertext, ciphertext + plaintext_length);
	if (status != PSA_SUCCESS) {
		return status;
	}

	*ciphertext_length = plaintext_length + tag_length;
	return PSA_SUCCESS;
}

psa_status_t rtk_transparent_aead_decrypt(
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg,
	const uint8_t *nonce, size_t nonce_length,
	const uint8_t *additional_data, size_t additional_data_length,
	const uint8_t *ciphertext, size_t ciphertext_length,
	uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_length)
{
	psa_status_t status;
	uint8_t check_tag[16];
	size_t body_length;
	size_t tag_length = PSA_AEAD_TAG_LENGTH(psa_get_key_type(attributes),
											psa_get_key_bits(attributes), alg);

	status = rtk_aead_check(attributes, key_buffer_size, alg);
	if (status != PSA_SUCCESS) {
		return status;
	}

	if (tag_length > sizeof(check_tag) || ciphertext_length < tag_length) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}

	body_length = ciphertext_length - tag_length;
	if (plaintext_size < body_length) {
		return PSA_ERROR_BUFFER_TOO_SMALL;
	}

	status = rtk_aead_one_shot(attributes, key_buffer, key_buffer_size, alg, 0,
							   nonce, nonce_length,
							   additional_data, additional_data_length,
							   ciphertext, body_length,
							   plaintext, check_tag);
	if (status != PSA_SUCCESS) {
		goto exit;
	}

	if (mbedtls_ct_memcmp(check_tag, ciphertext + body_length, tag_length) != 0) {
		status = PSA_ERROR_INVALID_SIGNATURE;
		goto exit;
	}

	*plaintext_length = body_length;

exit:
	if (status != PSA_SUCCESS) {
		mbedtls_platform_zeroize(plaintext, plaintext_size);
	}
	mbedtls_platform_zeroize(check_tag, sizeof(check_tag));
	return status;
}

/* -------------------------------------------------------------------------
 * Multipart entry points
 * ---------------------------------------------------------------------- */

psa_status_t rtk_transparent_aead_encrypt_setup(
	rtk_psa_aead_operation_t *operation,
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg)
{
	return rtk_aead_setup(operation, attributes, key_buffer, key_buffer_size, alg, 1);
}

psa_status_t rtk_transparent_aead_decrypt_setup(
	rtk_psa_aead_operation_t *operation,
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg)
{
	return rtk_aead_setup(operation, attributes, key_buffer, key_buffer_size, alg, 0);
}

psa_status_t rtk_transparent_aead_set_nonce(
	rtk_psa_aead_operation_t *operation,
	const uint8_t *nonce, size_t nonce_length)
{
	int ret;

	if (nonce == NULL || nonce_length == 0) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}

	if (RTK_AEAD_IS_GCM(operation->alg)) {
		u8 mode = operation->is_encrypt ? CIPHER_ENCRYPTION_MODE : CIPHER_DECRYPTION_MODE;
		IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
		ret = crypto_aes_gcm_starts(RTK_GCM_CTX(operation), mode,
									(u8 *) nonce, (u32) nonce_length);
		IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
	} else {
		u8 mode = operation->is_encrypt ? CCM_MODE_ENCRYPT : CCM_MODE_DECRYPT;
		/* q = 15 - iv_len must stay in the CCM-legal 2..8 range. */
		if (nonce_length < 7 || nonce_length > 13) {
			return PSA_ERROR_INVALID_ARGUMENT;
		}
		/* ccm_starts only sets up context state; no DMA on this path. */
		ret = crypto_aes_ccm_starts(RTK_CCM_CTX(operation), mode, nonce, nonce_length);
	}

	return rtk_hw_to_psa_error(ret);
}

psa_status_t rtk_transparent_aead_set_lengths(
	rtk_psa_aead_operation_t *operation,
	size_t ad_length, size_t plaintext_length)
{
	int ret;

	/* GCM streams the lengths as it goes and has no equivalent call. */
	if (RTK_AEAD_IS_GCM(operation->alg)) {
		operation->lengths_set = 1;
		return PSA_SUCCESS;
	}

	/* CCM ccm_set_lengths builds the B0 block via AES_ProcessDma. */
	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	ret = crypto_aes_ccm_set_lengths(RTK_CCM_CTX(operation), ad_length,
									 plaintext_length, operation->tag_length);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
	if (ret == 0) {
		operation->lengths_set = 1;
	}

	return rtk_hw_to_psa_error(ret);
}

psa_status_t rtk_transparent_aead_update_ad(
	rtk_psa_aead_operation_t *operation,
	const uint8_t *input, size_t input_length)
{
	int ret;

	if (input_length == 0) {
		return PSA_SUCCESS;
	}

	if (input == NULL) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	if (RTK_AEAD_IS_GCM(operation->alg)) {
		ret = crypto_aes_gcm_update_ad(RTK_GCM_CTX(operation), input, (u32) input_length);
	} else {
		/* CCM cannot start hashing AAD before the B0 block is known. */
		if (!operation->lengths_set) {
			IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
			return PSA_ERROR_BAD_STATE;
		}
		ret = crypto_aes_ccm_update_ad(RTK_CCM_CTX(operation), input, input_length);
	}
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);

	return rtk_hw_to_psa_error(ret);
}

psa_status_t rtk_transparent_aead_update(
	rtk_psa_aead_operation_t *operation,
	const uint8_t *input, size_t input_length,
	uint8_t *output, size_t output_size, size_t *output_length)
{
	int ret;

	if (input_length == 0) {
		*output_length = 0;
		return PSA_SUCCESS;
	}

	if (input == NULL || output == NULL) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	if (RTK_AEAD_IS_GCM(operation->alg)) {
		u32 out_len = 0;
		ret = crypto_aes_gcm_update(RTK_GCM_CTX(operation), input, (u32) input_length,
									output, (u32) output_size, &out_len);
		*output_length = (size_t) out_len;
	} else {
		size_t out_len = 0;
		if (!operation->lengths_set) {
			IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
			return PSA_ERROR_BAD_STATE;
		}
		ret = crypto_aes_ccm_update(RTK_CCM_CTX(operation), input, input_length,
									output, output_size, &out_len);
		*output_length = out_len;
	}
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);

	return rtk_hw_to_psa_error(ret);
}

psa_status_t rtk_transparent_aead_finish(
	rtk_psa_aead_operation_t *operation,
	uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length,
	uint8_t *tag, size_t tag_size, size_t *tag_length)
{
	int ret;

	if (tag_size < operation->tag_length) {
		return PSA_ERROR_BUFFER_TOO_SMALL;
	}

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	if (RTK_AEAD_IS_GCM(operation->alg)) {
		u32 out_len = 0;
		ret = crypto_aes_gcm_finish(RTK_GCM_CTX(operation), ciphertext,
									(u32) ciphertext_size, &out_len,
									tag, (u32) operation->tag_length);
		*ciphertext_length = (size_t) out_len;
	} else {
		/* CCM buffers nothing past the last update(): no trailing output. */
		ret = crypto_aes_ccm_finish(RTK_CCM_CTX(operation), tag, operation->tag_length);
		*ciphertext_length = 0;
	}
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);

	if (ret != 0) {
		return rtk_hw_to_psa_error(ret);
	}

	*tag_length = operation->tag_length;
	return PSA_SUCCESS;
}

psa_status_t rtk_transparent_aead_verify(
	rtk_psa_aead_operation_t *operation,
	uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_length,
	const uint8_t *tag, size_t tag_length)
{
	psa_status_t status;
	uint8_t check_tag[16];
	size_t check_tag_length = 0;

	if (tag_length > sizeof(check_tag)) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}

	status = rtk_transparent_aead_finish(operation, plaintext, plaintext_size,
										 plaintext_length, check_tag,
										 sizeof(check_tag), &check_tag_length);
	if (status == PSA_SUCCESS) {
		if (tag_length != check_tag_length ||
			mbedtls_ct_memcmp(tag, check_tag, tag_length) != 0) {
			status = PSA_ERROR_INVALID_SIGNATURE;
		}
	}

	mbedtls_platform_zeroize(check_tag, sizeof(check_tag));
	return status;
}

psa_status_t rtk_transparent_aead_abort(rtk_psa_aead_operation_t *operation)
{
	if (RTK_AEAD_IS_GCM(operation->alg)) {
		crypto_aes_gcm_free(RTK_GCM_CTX(operation));
	} else if (RTK_AEAD_IS_CCM(operation->alg)) {
		crypto_aes_ccm_free(RTK_CCM_CTX(operation));
	}

	mbedtls_platform_zeroize(operation, sizeof(*operation));
	return PSA_SUCCESS;
}
