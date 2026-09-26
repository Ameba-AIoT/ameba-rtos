/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/check.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/crypto.h>

#include "mbedtls/aes.h"
#include "mbedtls/hmac_drbg.h"
#include "mbedtls/md.h"

#include "common/bt_str.h"

#include "hci_core.h"

#define LOG_LEVEL CONFIG_BT_HCI_CORE_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(bt_host_crypto);

static mbedtls_hmac_drbg_context prng;

static int prng_entropy_cb(void *ctx, unsigned char *buf, size_t len)
{
	(void)ctx;
	return bt_hci_le_rand(buf, len);
}

__attribute__((unused))
static int prng_reseed(mbedtls_hmac_drbg_context *h)
{
	int64_t extra;
	int ret;

	extra = k_uptime_get();

	ret = mbedtls_hmac_drbg_reseed(h, (uint8_t *)&extra, sizeof(extra));
	if (ret != 0) {
		LOG_ERR("Failed to re-seed PRNG");
		return -EIO;
	}

	return 0;
}

int prng_init(void)
{
	uint8_t perso[8];
	int ret;

	ret = bt_hci_le_rand(perso, sizeof(perso));
	if (ret) {
		return ret;
	}

	mbedtls_hmac_drbg_init(&prng);
	ret = mbedtls_hmac_drbg_seed(&prng, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), prng_entropy_cb, NULL, perso, sizeof(perso));
	if (ret != 0) {
		LOG_ERR("Failed to initialize PRNG");
		return -EIO;
	}

	/* re-seed is needed after init */
	return 0;
}

#if ZEPHYR_RTK_PATCH
/* zephyr_patch: release the MD/HMAC contexts that mbedtls_hmac_drbg_seed() allocates
 * inside prng_init(). prng_init() runs on every bt_enable(), so without this each
 * enable/disable cycle leaks them. Safe to call even if the PRNG was never seeded. */
void prng_deinit(void)
{
	mbedtls_hmac_drbg_free(&prng);
}
#endif

#if defined(CONFIG_BT_HOST_CRYPTO_PRNG)
int bt_rand(void *buf, size_t len)
{

	CHECKIF(buf == NULL || len == 0) {
		return -EINVAL;
	}

	return mbedtls_hmac_drbg_random(&prng, buf, len) == 0 ? 0 : -EIO;
}
#else /* !CONFIG_BT_HOST_CRYPTO_PRNG */
int bt_rand(void *buf, size_t len)
{
	CHECKIF(buf == NULL || len == 0) {
		return -EINVAL;
	}

	return bt_hci_le_rand(buf, len);
}
#endif /* CONFIG_BT_HOST_CRYPTO_PRNG */

int bt_encrypt_le(const uint8_t key[16], const uint8_t plaintext[16],
		  uint8_t enc_data[16])
{
	mbedtls_aes_context ctx;
	uint8_t tmp_key[16], tmp_pt[16];
	int ret;

	CHECKIF(key == NULL || plaintext == NULL || enc_data == NULL) {
		return -EINVAL;
	}

	sys_memcpy_swap(tmp_key, key, 16);
	sys_memcpy_swap(tmp_pt, plaintext, 16);

	LOG_DBG("key %s", bt_hex(key, 16));
	LOG_DBG("plaintext %s", bt_hex(plaintext, 16));

	mbedtls_aes_init(&ctx);
	ret = mbedtls_aes_setkey_enc(&ctx, tmp_key, 128);
	if (ret == 0) {
		ret = mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, tmp_pt, enc_data);
	}
	mbedtls_aes_free(&ctx);

	if (ret != 0) {
		return -EIO;
	}
	sys_mem_swap(enc_data, 16);

	LOG_DBG("enc_data %s", bt_hex(enc_data, 16));

	return 0;
}

int bt_encrypt_be(const uint8_t key[16], const uint8_t plaintext[16],
		  uint8_t enc_data[16])
{
	mbedtls_aes_context ctx;
	int ret;

	CHECKIF(key == NULL || plaintext == NULL || enc_data == NULL) {
		return -EINVAL;
	}

	LOG_DBG("key %s", bt_hex(key, 16));
	LOG_DBG("plaintext %s", bt_hex(plaintext, 16));

	mbedtls_aes_init(&ctx);
	ret = mbedtls_aes_setkey_enc(&ctx, key, 128);
	if (ret == 0) {
		ret = mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, plaintext, enc_data);
	}

	LOG_DBG("enc_data %s", bt_hex(enc_data, 16));

	mbedtls_aes_free(&ctx);
	return (ret != 0) ? -EIO : 0;
}

#ifdef ZTEST_UNITTEST
struct mbedtls_hmac_drbg_context *bt_crypto_get_hmac_prng_instance(void)
{
	return &prng;
}
#endif /* ZTEST_UNITTEST */
