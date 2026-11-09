/* Copyright (c) 2022 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <string.h>

#include <zephyr/sys/byteorder.h>
#if defined(CONFIG_BT_USE_PSA_API)
#include "psa/crypto.h"
#else
#include "mbedtls/cmac.h"
#include "mbedtls/cipher.h"
#endif
#include "common/bt_str.h"
#include "bt_crypto.h"

int bt_crypto_aes_cmac(const uint8_t *key, const uint8_t *in, size_t len, uint8_t *out)
{
#if defined(CONFIG_BT_USE_PSA_API)
	psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
	psa_key_id_t key_id;
	psa_status_t status;
	size_t out_len;

	psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
	psa_set_key_bits(&attr, 128);
	psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_MESSAGE);
	psa_set_key_algorithm(&attr, PSA_ALG_CMAC);
	if (psa_import_key(&attr, key, 16, &key_id) != PSA_SUCCESS) {
		return -EIO;
	}

	status = psa_mac_compute(key_id, PSA_ALG_CMAC, in, len, out, 16, &out_len);
	psa_destroy_key(key_id);

	return (status == PSA_SUCCESS) ? 0 : -EIO;
#else
	const mbedtls_cipher_info_t *info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);

	if (mbedtls_cipher_cmac(info, key, 128, in, len, out) != 0) {
		return -EIO;
	}

	return 0;
#endif
}
