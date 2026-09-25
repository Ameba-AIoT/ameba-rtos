/*
 * Copyright (c) 2017 Intel Corporation
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include "mbedtls/cmac.h"
#include "mbedtls/cipher.h"
#include "mbedtls/md.h"
#include "mbedtls/ecp.h"
#include "mbedtls/ecdh.h"

#include <zephyr/bluetooth/mesh.h>
#include <zephyr/bluetooth/crypto.h>

#define LOG_LEVEL CONFIG_BT_MESH_CRYPTO_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(bt_mesh_crypto_tc);

#include "mesh.h"
#include "crypto.h"
#include "prov.h"

static struct {
	bool is_ready;
	uint8_t private_key_be[PRIV_KEY_SIZE];
	uint8_t public_key_be[PUB_KEY_SIZE];
} dh_pair;

static int bt_rand_wrapper(void *ctx, unsigned char *buf, size_t len)
{
	(void)ctx;
	return bt_rand(buf, len);
}

int bt_mesh_encrypt(const struct bt_mesh_key *key, const uint8_t plaintext[16],
		    uint8_t enc_data[16])
{
	return bt_encrypt_be(key->key, plaintext, enc_data);
}

int bt_mesh_ccm_encrypt(const struct bt_mesh_key *key, uint8_t nonce[13], const uint8_t *plaintext,
			size_t len, const uint8_t *aad, size_t aad_len, uint8_t *enc_data,
			size_t mic_size)
{
	return bt_ccm_encrypt(key->key, nonce, plaintext, len, aad, aad_len, enc_data, mic_size);
}

int bt_mesh_ccm_decrypt(const struct bt_mesh_key *key, uint8_t nonce[13], const uint8_t *enc_data,
			size_t len, const uint8_t *aad, size_t aad_len, uint8_t *plaintext,
			size_t mic_size)
{
	return bt_ccm_decrypt(key->key, nonce, enc_data, len, aad, aad_len, plaintext, mic_size);
}

int bt_mesh_aes_cmac_raw_key(const uint8_t key[16], struct bt_mesh_sg *sg, size_t sg_len,
			     uint8_t mac[16])
{
	const mbedtls_cipher_info_t *info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);
	mbedtls_cipher_context_t ctx;
	int ret;

	mbedtls_cipher_init(&ctx);
	ret = mbedtls_cipher_setup(&ctx, info);
	if (ret != 0) {
		goto out;
	}
	ret = mbedtls_cipher_cmac_starts(&ctx, key, 128);
	if (ret != 0) {
		goto out;
	}

	for (; sg_len; sg_len--, sg++) {
		if (sg->len == 0) {
			continue;
		}
		ret = mbedtls_cipher_cmac_update(&ctx, sg->data, sg->len);
		if (ret != 0) {
			goto out;
		}
	}
	ret = mbedtls_cipher_cmac_finish(&ctx, mac);
out:
	mbedtls_cipher_free(&ctx);
	return ret ? -EIO : 0;
}

int bt_mesh_aes_cmac_mesh_key(const struct bt_mesh_key *key, struct bt_mesh_sg *sg,
			size_t sg_len, uint8_t mac[16])
{
	return bt_mesh_aes_cmac_raw_key(key->key, sg, sg_len, mac);
}

int bt_mesh_sha256_hmac_raw_key(const uint8_t key[32], struct bt_mesh_sg *sg, size_t sg_len,
				uint8_t mac[32])
{
	const mbedtls_md_info_t *info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
	mbedtls_md_context_t ctx;
	int ret;

	mbedtls_md_init(&ctx);
	ret = mbedtls_md_setup(&ctx, info, 1);
	if (ret != 0) {
		goto out;
	}

	ret = mbedtls_md_hmac_starts(&ctx, key, 32);
	if (ret != 0) {
		goto out;
	}

	for (; sg_len; sg_len--, sg++) {
		ret = mbedtls_md_hmac_update(&ctx, sg->data, sg->len);
		if (ret != 0) {
			goto out;
		}
	}
	ret = mbedtls_md_hmac_finish(&ctx, mac);
out:
	mbedtls_md_free(&ctx);
	return ret ? -EIO : 0;
}

int bt_mesh_pub_key_gen(void)
{
	mbedtls_ecp_group grp;
	mbedtls_mpi priv;
	mbedtls_ecp_point pub;
	uint8_t tmp[1 + PUB_KEY_SIZE];
	size_t olen;
	int ret;

	mbedtls_ecp_group_init(&grp);
	mbedtls_mpi_init(&priv);
	mbedtls_ecp_point_init(&pub);

	ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
	if (ret) {
		goto out;
	}
	ret = mbedtls_ecp_gen_keypair(&grp, &priv, &pub, bt_rand_wrapper, NULL);
	if (ret) {
		goto out;
	}
	ret = mbedtls_mpi_write_binary(&priv, dh_pair.private_key_be, PRIV_KEY_SIZE);
	if (ret) {
		goto out;
	}
	ret = mbedtls_ecp_point_write_binary(&grp, &pub, MBEDTLS_ECP_PF_UNCOMPRESSED,
					     &olen, tmp, sizeof(tmp));
	if (ret == 0) {
		memcpy(dh_pair.public_key_be, tmp + 1, PUB_KEY_SIZE);
	}
out:
	mbedtls_ecp_group_free(&grp);
	mbedtls_mpi_free(&priv);
	mbedtls_ecp_point_free(&pub);

	if (ret) {
		dh_pair.is_ready = false;
		LOG_ERR("Failed to create public/private pair");
		return -EIO;
	}

	dh_pair.is_ready = true;

	return 0;
}

const uint8_t *bt_mesh_pub_key_get(void)
{
	return dh_pair.is_ready ? dh_pair.public_key_be : NULL;
}

int bt_mesh_dhkey_gen(const uint8_t *pub_key, const uint8_t *priv_key, uint8_t *dhkey)
{
	mbedtls_ecp_group grp;
	mbedtls_mpi priv, secret;
	mbedtls_ecp_point pub;
	uint8_t tmp[1 + PUB_KEY_SIZE];
	int ret;

	mbedtls_ecp_group_init(&grp);
	mbedtls_mpi_init(&priv);
	mbedtls_mpi_init(&secret);
	mbedtls_ecp_point_init(&pub);

	ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
	if (ret) {
		goto out;
	}

	/* pub_key is 64 bytes (x||y); prepend 0x04 for uncompressed point format */
	tmp[0] = 0x04;
	memcpy(tmp + 1, pub_key, PUB_KEY_SIZE);
	ret = mbedtls_ecp_point_read_binary(&grp, &pub, tmp, sizeof(tmp));
	if (ret) {
		goto out;
	}

	ret = mbedtls_mpi_read_binary(&priv,
				      priv_key ? priv_key : dh_pair.private_key_be,
				      PRIV_KEY_SIZE);
	if (ret) {
		goto out;
	}

	ret = mbedtls_ecdh_compute_shared(&grp, &secret, &pub, &priv, bt_rand_wrapper, NULL);
	if (ret) {
		goto out;
	}

	ret = mbedtls_mpi_write_binary(&secret, dhkey, PRIV_KEY_SIZE);
out:
	mbedtls_ecp_group_free(&grp);
	mbedtls_mpi_free(&priv);
	mbedtls_mpi_free(&secret);
	mbedtls_ecp_point_free(&pub);

	return ret ? -EIO : 0;
}

int bt_mesh_crypto_init(void)
{
	return 0;
}
