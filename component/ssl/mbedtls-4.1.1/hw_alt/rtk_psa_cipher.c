/**
 * *****************************************************************************
 * @file    rtk_psa_cipher.c
 * @brief   PSA transparent driver for the Realtek AES engine.
 *          Replaces aes_alt.c of the 3.6.5 port.
 *
 *          Single-shot (encrypt/decrypt) and multipart (setup/set_iv/update/
 *          finish/abort) entry points are implemented for ECB and CBC.
 *          CTR multipart is NOT accelerated here (returns NOT_SUPPORTED so
 *          the core falls back to the software driver) because CTR is a stream
 *          cipher: residual bytes require buffering keystream, not ciphertext,
 *          and the hardware does not export keystream.  See DECISIONS.md D-10.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#include "rtk_psa_cipher.h"


#include <string.h>
#include "ameba_soc.h"
#include "rtk_psa_error.h"
#include "mbedtls/platform_util.h"

#define RTK_AES_BLOCK_SIZE 16

static int rtk_aes_key_slot(void)
{
	return (TrustZone_IsSecure() == 0) ? KM_AES_KEY_NS_SW1 : KM_AES_KEY_S_SW1;
}

/* The engine only implements AES; key sizes are 128/192/256 bit. */
static psa_status_t rtk_aes_check_key(const psa_key_attributes_t *attributes,
									  size_t key_buffer_size, u32 *key_len_bits)
{
	if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	if (key_buffer_size != 16 && key_buffer_size != 24 && key_buffer_size != 32) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	*key_len_bits = (u32) key_buffer_size * 8;

	return PSA_SUCCESS;
}

static psa_status_t rtk_aes_crypt(psa_algorithm_t alg, u8 is_encryption,
								  u32 key_len_bits, const uint8_t *key_buffer,
								  const uint8_t *iv,
								  const uint8_t *input, size_t input_length,
								  uint8_t *output)
{
	u8 iv_copy[RTK_AES_BLOCK_SIZE];
	int key_id = rtk_aes_key_slot();
	int ret;

	/* The engine updates the IV in place; PSA passes it as const. */
	if (iv != NULL) {
		memcpy(iv_copy, iv, RTK_AES_BLOCK_SIZE);
	}

	/* Serialise key-slot use: the slot must not be overwritten between
	 * set_sw_key() and the operation that consumes it. */
	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);

	ret = crypto_aes_set_sw_key(key_id, key_len_bits, (u8 *) key_buffer);
	if (ret != 0) {
		goto exit;
	}

	switch (alg) {
	case PSA_ALG_ECB_NO_PADDING:
		ret = crypto_aes_ecb(key_id, key_len_bits, is_encryption,
							 input, (u32) input_length, output);
		break;
	case PSA_ALG_CBC_NO_PADDING:
		ret = crypto_aes_cbc(key_id, key_len_bits, is_encryption,
							 input, (u32) input_length, iv_copy, output);
		break;
	case PSA_ALG_CTR:
		ret = crypto_aes_ctr(key_id, key_len_bits, is_encryption,
							 input, (u32) input_length, iv_copy, output);
		break;
	default:
		ret = 0;
		IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
		return PSA_ERROR_NOT_SUPPORTED;
	}

exit:
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);

	return rtk_hw_to_psa_error(ret);
}

/* ECB and CBC take whole blocks only; CTR is a stream cipher. */
static psa_status_t rtk_aes_check_length(psa_algorithm_t alg, size_t input_length)
{
	if (alg == PSA_ALG_CTR) {
		return PSA_SUCCESS;
	}

	if (input_length % RTK_AES_BLOCK_SIZE != 0) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}

	return PSA_SUCCESS;
}

psa_status_t rtk_transparent_cipher_encrypt(const psa_key_attributes_t *attributes,
		const uint8_t *key_buffer, size_t key_buffer_size,
		psa_algorithm_t alg,
		const uint8_t *iv, size_t iv_length,
		const uint8_t *input, size_t input_length,
		uint8_t *output, size_t output_size, size_t *output_length)
{
	u32 key_len_bits;
	psa_status_t status;

	if (alg != PSA_ALG_ECB_NO_PADDING && alg != PSA_ALG_CBC_NO_PADDING &&
		alg != PSA_ALG_CTR) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	status = rtk_aes_check_key(attributes, key_buffer_size, &key_len_bits);
	if (status != PSA_SUCCESS) {
		return status;
	}

	/* ECB takes no IV; the other two need a full block. */
	if (alg == PSA_ALG_ECB_NO_PADDING) {
		if (iv_length != 0) {
			return PSA_ERROR_INVALID_ARGUMENT;
		}
	} else if (iv_length != RTK_AES_BLOCK_SIZE) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}

	status = rtk_aes_check_length(alg, input_length);
	if (status != PSA_SUCCESS) {
		return status;
	}

	if (output_size < input_length) {
		return PSA_ERROR_BUFFER_TOO_SMALL;
	}

	if (input_length == 0) {
		*output_length = 0;
		return PSA_SUCCESS;
	}

	status = rtk_aes_crypt(alg, 1, key_len_bits, key_buffer, iv,
						   input, input_length, output);
	if (status != PSA_SUCCESS) {
		return status;
	}

	*output_length = input_length;

	return PSA_SUCCESS;
}

psa_status_t rtk_transparent_cipher_decrypt(const psa_key_attributes_t *attributes,
		const uint8_t *key_buffer, size_t key_buffer_size,
		psa_algorithm_t alg,
		const uint8_t *input, size_t input_length,
		uint8_t *output, size_t output_size, size_t *output_length)
{
	u32 key_len_bits;
	psa_status_t status;
	const uint8_t *iv = NULL;
	size_t body_length = input_length;

	if (alg != PSA_ALG_ECB_NO_PADDING && alg != PSA_ALG_CBC_NO_PADDING &&
		alg != PSA_ALG_CTR) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	status = rtk_aes_check_key(attributes, key_buffer_size, &key_len_bits);
	if (status != PSA_SUCCESS) {
		return status;
	}

	/* On the decrypt path PSA prepends the IV to the ciphertext. */
	if (alg != PSA_ALG_ECB_NO_PADDING) {
		if (input_length < RTK_AES_BLOCK_SIZE) {
			return PSA_ERROR_INVALID_ARGUMENT;
		}
		iv = input;
		input += RTK_AES_BLOCK_SIZE;
		body_length = input_length - RTK_AES_BLOCK_SIZE;
	}

	status = rtk_aes_check_length(alg, body_length);
	if (status != PSA_SUCCESS) {
		return status;
	}

	if (output_size < body_length) {
		return PSA_ERROR_BUFFER_TOO_SMALL;
	}

	if (body_length == 0) {
		*output_length = 0;
		return PSA_SUCCESS;
	}

	/* CTR decryption is the same operation as encryption. */
	status = rtk_aes_crypt(alg, (alg == PSA_ALG_CTR) ? 1 : 0, key_len_bits,
						   key_buffer, iv, input, body_length, output);
	if (status != PSA_SUCCESS) {
		return status;
	}

	*output_length = body_length;

	return PSA_SUCCESS;
}

/* -------------------------------------------------------------------------
 * Multipart cipher driver (ECB / CBC hardware; CTR → PSA_ERROR_NOT_SUPPORTED
 * so the core falls back to the software driver for CTR multipart).
 *
 * State layout (rtk_psa_cipher_operation_s, hidden behind the opaque buffer):
 *   alg          — PSA_ALG_ECB_NO_PADDING / PSA_ALG_CBC_NO_PADDING
 *   is_encrypt   — 1 = encrypt, 0 = decrypt
 *   key_len_bits — 128 / 192 / 256
 *   key[32]      — raw key bytes
 *   iv[16]       — current IV / chaining value (updated after each HW call)
 *   buf[16]      — residual input not yet pushed to hardware
 *   buf_len      — number of bytes in buf (0..15)
 *
 * Per-update() flow:
 *   1. Prepend buf to incoming data conceptually.
 *   2. Compute how many full blocks (whole_len) fit.
 *   3. If whole_len > 0: run hardware on (buf[0..buf_len] + input[0..X]),
 *      hardware writes back updated IV via AES_GetIv() (confirmed in
 *      DECISIONS.md D-10 / crypto_sym_start():547).
 *   4. Store remaining input tail in buf.
 *
 * finish():
 *   ECB/CBC: buf_len must be 0 (no padding mode).
 *   *output_length = 0.
 * ---------------------------------------------------------------------- */

struct rtk_psa_cipher_operation_s {
	psa_algorithm_t alg;
	uint8_t         is_encrypt;
	uint8_t         key_len_bits_div8; /* key length in bytes: 16/24/32 */
	uint8_t         buf_len;           /* bytes stored in buf: 0..15 */
	uint8_t         _pad;
	uint8_t         key[32];
	uint8_t         iv[RTK_AES_BLOCK_SIZE];
	uint8_t         buf[RTK_AES_BLOCK_SIZE];
};

_Static_assert(RTK_PSA_CIPHER_CONTEXT_SIZE >= sizeof(struct rtk_psa_cipher_operation_s),
			   "RTK_PSA_CIPHER_CONTEXT_SIZE too small");

#define RTK_OP(p) ((struct rtk_psa_cipher_operation_s *)(p))

static psa_status_t rtk_cipher_setup(rtk_psa_cipher_operation_t *operation,
									 const psa_key_attributes_t *attributes,
									 const uint8_t *key_buffer, size_t key_buffer_size,
									 psa_algorithm_t alg, uint8_t is_encrypt)
{
	u32 key_len_bits;
	psa_status_t status;

	/* Only ECB/CBC use the hardware multipart path; CTR falls to software. */
	if (alg != PSA_ALG_ECB_NO_PADDING && alg != PSA_ALG_CBC_NO_PADDING) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	status = rtk_aes_check_key(attributes, key_buffer_size, &key_len_bits);
	if (status != PSA_SUCCESS) {
		return status;
	}

	memset(operation, 0, sizeof(*operation));
	struct rtk_psa_cipher_operation_s *op = RTK_OP(operation);
	op->alg = alg;
	op->is_encrypt = is_encrypt;
	op->key_len_bits_div8 = (uint8_t)(key_len_bits / 8);
	memcpy(op->key, key_buffer, key_buffer_size);

	return PSA_SUCCESS;
}

psa_status_t rtk_transparent_cipher_encrypt_setup(rtk_psa_cipher_operation_t *operation,
		const psa_key_attributes_t *attributes,
		const uint8_t *key_buffer, size_t key_buffer_size,
		psa_algorithm_t alg)
{
	return rtk_cipher_setup(operation, attributes, key_buffer, key_buffer_size, alg, 1);
}

psa_status_t rtk_transparent_cipher_decrypt_setup(rtk_psa_cipher_operation_t *operation,
		const psa_key_attributes_t *attributes,
		const uint8_t *key_buffer, size_t key_buffer_size,
		psa_algorithm_t alg)
{
	return rtk_cipher_setup(operation, attributes, key_buffer, key_buffer_size, alg, 0);
}

psa_status_t rtk_transparent_cipher_set_iv(rtk_psa_cipher_operation_t *operation,
		const uint8_t *iv, size_t iv_length)
{
	struct rtk_psa_cipher_operation_s *op = RTK_OP(operation);

	if (op->alg == PSA_ALG_ECB_NO_PADDING) {
		/* ECB takes no IV. */
		return (iv_length == 0) ? PSA_SUCCESS : PSA_ERROR_INVALID_ARGUMENT;
	}

	if (iv_length != RTK_AES_BLOCK_SIZE) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}

	memcpy(op->iv, iv, RTK_AES_BLOCK_SIZE);
	return PSA_SUCCESS;
}

psa_status_t rtk_transparent_cipher_update(rtk_psa_cipher_operation_t *operation,
		const uint8_t *input, size_t input_length,
		uint8_t *output, size_t output_size, size_t *output_length)
{
	struct rtk_psa_cipher_operation_s *op = RTK_OP(operation);
	psa_status_t status = PSA_SUCCESS;
	size_t total = op->buf_len + input_length;
	size_t whole_len = (total / RTK_AES_BLOCK_SIZE) * RTK_AES_BLOCK_SIZE;
	size_t produced = 0;

	*output_length = 0;

	if (input_length == 0) {
		return PSA_SUCCESS;
	}

	if (whole_len == 0) {
		/* Not enough data to form a block yet; buffer it. */
		if (op->buf_len + input_length > RTK_AES_BLOCK_SIZE) {
			return PSA_ERROR_INSUFFICIENT_MEMORY;
		}
		memcpy(op->buf + op->buf_len, input, input_length);
		op->buf_len += (uint8_t)input_length;
		return PSA_SUCCESS;
	}

	if (output_size < whole_len) {
		return PSA_ERROR_BUFFER_TOO_SMALL;
	}

	/* Process blocks that start from the residual buffer. */
	if (op->buf_len > 0) {
		/* Complete the first block from the residual buffer + input head. */
		size_t need = RTK_AES_BLOCK_SIZE - op->buf_len;
		memcpy(op->buf + op->buf_len, input, need);
		input        += need;
		input_length -= need;

		/* Run one block through hardware. */
		u32 key_len_bits = (u32)op->key_len_bits_div8 * 8;
		int key_id = rtk_aes_key_slot();
		int ret;

		IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
		ret = crypto_aes_set_sw_key((u8)key_id, key_len_bits, op->key);
		if (ret != 0) {
			IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
			return rtk_hw_to_psa_error(ret);
		}

		if (op->alg == PSA_ALG_ECB_NO_PADDING) {
			ret = crypto_aes_ecb((u8)key_id, key_len_bits, op->is_encrypt,
								 op->buf, RTK_AES_BLOCK_SIZE, output);
		} else {
			ret = crypto_aes_cbc((u8)key_id, key_len_bits, op->is_encrypt,
								 op->buf, RTK_AES_BLOCK_SIZE, op->iv, output);
		}
		IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);

		if (ret != 0) {
			return rtk_hw_to_psa_error(ret);
		}

		output       += RTK_AES_BLOCK_SIZE;
		produced     += RTK_AES_BLOCK_SIZE;
		op->buf_len   = 0;
		whole_len    -= RTK_AES_BLOCK_SIZE;
	}

	/* Process remaining full blocks directly from input. */
	if (whole_len > 0) {
		u32 key_len_bits = (u32)op->key_len_bits_div8 * 8;
		int key_id = rtk_aes_key_slot();
		int ret;

		IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
		ret = crypto_aes_set_sw_key((u8)key_id, key_len_bits, op->key);
		if (ret != 0) {
			IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);
			return rtk_hw_to_psa_error(ret);
		}

		if (op->alg == PSA_ALG_ECB_NO_PADDING) {
			ret = crypto_aes_ecb((u8)key_id, key_len_bits, op->is_encrypt,
								 input, (u32)whole_len, output);
		} else {
			ret = crypto_aes_cbc((u8)key_id, key_len_bits, op->is_encrypt,
								 input, (u32)whole_len, op->iv, output);
		}
		IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);

		if (ret != 0) {
			return rtk_hw_to_psa_error(ret);
		}

		produced     += whole_len;
		input        += whole_len;
		input_length -= whole_len;
	}

	/* Buffer any tail that didn't fill a block. */
	if (input_length > 0) {
		memcpy(op->buf, input, input_length);
		op->buf_len = (uint8_t)input_length;
	}

	*output_length = produced;
	(void)status;
	return PSA_SUCCESS;
}

psa_status_t rtk_transparent_cipher_finish(rtk_psa_cipher_operation_t *operation,
		uint8_t *output, size_t output_size, size_t *output_length)
{
	struct rtk_psa_cipher_operation_s *op = RTK_OP(operation);
	(void)output;
	(void)output_size;

	*output_length = 0;

	/* ECB/CBC NO_PADDING: all data must have been consumed in whole blocks. */
	if (op->buf_len != 0) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}

	return PSA_SUCCESS;
}

psa_status_t rtk_transparent_cipher_abort(rtk_psa_cipher_operation_t *operation)
{
	mbedtls_platform_zeroize(operation, sizeof(*operation));
	return PSA_SUCCESS;
}

