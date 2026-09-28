/* Copyright (c) 2022 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <string.h>

#include <zephyr/sys/byteorder.h>
#include "mbedtls/cmac.h"
#include "mbedtls/cipher.h"
#include "common/bt_str.h"
#include "bt_crypto.h"

int bt_crypto_aes_cmac(const uint8_t *key, const uint8_t *in, size_t len, uint8_t *out)
{
	const mbedtls_cipher_info_t *info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);

	if (mbedtls_cipher_cmac(info, key, 128, in, len, out) != 0) {
		return -EIO;
	}

	return 0;
}
