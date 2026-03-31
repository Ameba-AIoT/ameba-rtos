/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ota_internal.h"
#include "lwip_netconf.h"

static int ota_ssl_random(void *p_rng, u8 *output, size_t output_len)
{
	(void) p_rng;
	TRNG_get_random_bytes(output, output_len);
	return 0;
}

static int ota_ssl_verify(void *data, mbedtls_x509_crt *crt, int depth, int *flags)
{
	(void) data;
	(void) depth;
	char *buf = NULL;

	buf = (char *)rtos_mem_zmalloc(1024);
	if (!buf) {
		return 0;
	}

	mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);

	if (*flags) {
		RTK_LOGI(OTA_TAG, "[TLS] ERROR: certificate verify\n%s\n", buf);
	} else {
		RTK_LOGI(OTA_TAG, "[TLS] Certificate verified\n%s\n", buf);
	}
	rtos_mem_free(buf);
	return 0;
}

int ota_http_tls_init(ota_context_t *ctx)
{
	int ret = -1;

	ctx->tls = (ota_tls_t *)rtos_mem_malloc(sizeof(ota_tls_t));
	if (!ctx->tls) {
		RTK_LOGE(OTA_TAG, "[TLS] malloc failed\n");
		return OTA_ERR;
	}

	mbedtls_ssl_context *ssl = &ctx->tls->ssl;
	mbedtls_ssl_config *conf = &ctx->tls->conf;
	mbedtls_x509_crt *ca = &ctx->tls->ca;
	mbedtls_x509_crt *cert = &ctx->tls->cert;
	mbedtls_pk_context *key = &ctx->tls->key;

	RTK_LOGI(OTA_TAG, "[TLS]  . Setting up the SSL/TLS structure...\n");

	mbedtls_ssl_init(ssl);
	mbedtls_ssl_config_init(conf);
	mbedtls_x509_crt_init(ca);
	mbedtls_x509_crt_init(cert);
	mbedtls_pk_init(key);

	if ((ret = mbedtls_ssl_config_defaults(conf,
										   MBEDTLS_SSL_IS_CLIENT,
										   MBEDTLS_SSL_TRANSPORT_STREAM,
										   MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {

		RTK_LOGE(OTA_TAG, "[TLS] Config defaults failed: -0x%x\n", -ret);
		return OTA_ERR;
	}

	mbedtls_ssl_conf_rng(conf, ota_ssl_random, NULL);

	if ((ret = mbedtls_ssl_setup(ssl, conf)) != 0) {
		RTK_LOGE(OTA_TAG, "[TLS] Setup failed: -0x%x\n", -ret);
		return OTA_ERR;
	}

	RTK_LOGI(OTA_TAG, "[TLS] ok\n");

	mbedtls_ssl_set_bio(ssl, &ctx->fd, mbedtls_net_send, mbedtls_net_recv, NULL);

	if (ctx->ca_cert != NULL) {
		if (mbedtls_x509_crt_parse(ca, (const unsigned char *)ctx->ca_cert, strlen(ctx->ca_cert) + 1) != 0) {
			RTK_LOGE(OTA_TAG, "[TLS] parse ca_crt failed!\n");
			return OTA_ERR;
		}
		mbedtls_ssl_conf_ca_chain(conf, ca, NULL);
		mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_REQUIRED);
		mbedtls_ssl_conf_verify(conf, (int (*)(void *, mbedtls_x509_crt *, int, uint32_t *))ota_ssl_verify, NULL);
		RTK_LOGI(OTA_TAG, "[TLS] ca_crt parse done\n");
	} else {
		mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_NONE);
	}

	if (ctx->client_cert != NULL && ctx->private_key != NULL) {
		if (mbedtls_x509_crt_parse(cert, (const unsigned char *)ctx->client_cert, strlen(ctx->client_cert) + 1) != 0) {
			RTK_LOGE(OTA_TAG, "[TLS] parse client_crt failed!\n");
			return OTA_ERR;
		}

		if (mbedtls_pk_parse_key(key, (const unsigned char *)ctx->private_key, strlen(ctx->private_key) + 1, NULL, 0, NULL, NULL) != 0) {
			RTK_LOGE(OTA_TAG, "[TLS] parse private_key failed!\n");
			return OTA_ERR;
		}

		mbedtls_ssl_conf_own_cert(conf, cert, key);
	}

	RTK_LOGI(OTA_TAG, "[TLS]  . Performing the SSL/TLS handshake...\n");

	if ((ret = mbedtls_ssl_handshake(&ctx->tls->ssl)) != 0) {
		RTK_LOGE(OTA_TAG, "[TLS] Handshake failed: -0x%x\n", -ret);
		return OTA_ERR;
	}
	RTK_LOGI(OTA_TAG, "[TLS] ok\n");
	RTK_LOGI(OTA_TAG, "[TLS]  . Use ciphersuite %s\n", mbedtls_ssl_get_ciphersuite(&ctx->tls->ssl));

	return OTA_OK;
}

void ota_http_tls_free(ota_context_t *ctx)
{
	if (!ctx->tls) {
		return;
	}

	RTK_LOGI(OTA_TAG, "[TLS] Freeing resources...\n");

	mbedtls_x509_crt_free(&ctx->tls->ca);
	mbedtls_x509_crt_free(&ctx->tls->cert);
	mbedtls_pk_free(&ctx->tls->key);
	mbedtls_ssl_free(&ctx->tls->ssl);
	mbedtls_ssl_config_free(&ctx->tls->conf);

	rtos_mem_free(ctx->tls);
	ctx->tls = NULL;

	RTK_LOGI(OTA_TAG, "[TLS] Resources freed\n");
}

int ota_http_tls_write(ota_context_t *ctx, u8 *buf, int len)
{
	return mbedtls_ssl_write(&ctx->tls->ssl, buf, len);
}

int ota_http_tls_read(ota_context_t *ctx, u8 *buf, int len)
{
	return mbedtls_ssl_read(&ctx->tls->ssl, buf, len);
}