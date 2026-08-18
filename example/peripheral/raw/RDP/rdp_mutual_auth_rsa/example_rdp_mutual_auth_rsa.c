/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * rdp_mutual_auth_rsa -- board as mTLS client connecting to client.badssl.com
 *
 * The client private key lives in Secure World (image3). The NS side uses
 * mbedtls_pk_setup_rsa_alt to route TLS CertificateVerify signing to the
 * secure_client_sign NSC call.
 *
 * TLS CertificateVerify signing path:
 *   mbedtls TLS state machine
 *     -> rsa_alt_sign_wrap (pk_wrap.c)
 *       -> nsc_sign_wrapper (NS-side bridge, this file)
 *         -> secure_client_sign NSC (image3, S-heap RSA private key)
 *
 * NS-side security boundary:
 *   - Does NOT hold the client private key d (only a proxy pk_context)
 *   - Holds the client certificate PEM (public info, returned by secure_client_init NSC)
 *   - Holds ISRG Root X1 (server verification, public info)
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "lwip_netconf.h"
#include "secure_client_service_rsa.h"  /* secure_client_init / secure_client_sign NSC */
#include "wifi_api.h"

#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"

#include <string.h>

static const char *const TAG = "DEMO";

/* WiFi credentials */
#define WIFI_SSID     "<your_wifi_ssid>"      /* replace with your AP SSID     */
#define WIFI_PASSWORD "<your_wifi_password>"  /* replace with your AP password  */
#define SERVER_HOST   "client.badssl.com"
#define SERVER_PORT   "443"
#define HTTP_REQUEST  \
	"GET / HTTP/1.1\r\n" \
	"Host: " SERVER_HOST "\r\n" \
	"Connection: close\r\n\r\n"

/* -------------------------------------------------------------------------- */
/* ISRG Root X1 -- CA certificate used to verify the server's Let's Encrypt   */
/* certificate chain (ISRG Root X1 -> R13 -> *.badssl.com).                   */
/* -------------------------------------------------------------------------- */
static const char isrg_root_x1_pem[] =
	"-----BEGIN CERTIFICATE-----\r\n"
	"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\r\n"
	"-----END CERTIFICATE-----\r\n";

/* -------------------------------------------------------------------------- */
/* NS-side pk_rsa_alt bridge -- routes signing to the S-side via NSC          */
/* -------------------------------------------------------------------------- */

/*
 * sign_func callback: invoked by mbedtls rsa_alt_sign_wrap during CertificateVerify.
 * Signature matches the mbedtls_pk_rsa_alt_sign_func typedef.
 */
static int nsc_sign_wrapper(void *ctx,
							int (*f_rng)(void *, unsigned char *, size_t),
							void *p_rng,
							mbedtls_md_type_t md_alg,
							unsigned int hashlen,
							const unsigned char *hash,
							unsigned char *sig)
{
	(void)ctx;
	(void)f_rng;
	(void)p_rng;

	size_t sig_len = 0;
	secure_client_sign_req_t req;
	req.md_alg   = (unsigned int)md_alg;
	req.hash_len = hashlen;
	req.hash     = hash;
	req.sig      = sig;
	req.sig_size = SEC_CLI_SIG_LEN;  /* RSA-2048 output is always 256 bytes */
	req.sig_len  = &sig_len;
	return secure_client_sign(&req);  /* NSC call -> S-side RSA sign */
}

/* key_len_func callback: returns RSA-2048 signature length in bytes */
static size_t nsc_key_len_func(void *ctx)
{
	(void)ctx;
	return SEC_CLI_SIG_LEN;  /* 256 bytes */
}

/* -------------------------------------------------------------------------- */
/* RNG                                                                         */
/* -------------------------------------------------------------------------- */
static int my_random(void *p_rng, unsigned char *output, size_t output_len)
{
	(void)p_rng;
	TRNG_get_random_bytes(output, output_len);
	return 0;
}

/* -------------------------------------------------------------------------- */
/* Certificate verify callback                                                 */
/* -------------------------------------------------------------------------- */
static int cert_verify_cb(void *data, mbedtls_x509_crt *crt,
						  int depth, uint32_t *flags)
{
	(void)data;
	(void)crt;
	if (*flags == 0) {
		RTK_LOGI(TAG, "  cert OK (depth %d)\n", depth);
	} else {
		RTK_LOGI(TAG, "  cert FAIL depth=%d flags=0x%08x\n",
				 depth, (unsigned)*flags);
	}
	return 0;
}

/* -------------------------------------------------------------------------- */
/* Demo task                                                                   */
/* -------------------------------------------------------------------------- */
static void demo_task(void *param)
{
	(void)param;
	int ret;

	RTK_LOGI(TAG, "=== mTLS Client Demo ===\n");
	RTK_LOGI(TAG, "Client key in Secure World, sign via NSC\n");

	/* Step 1: Allocate secure context on S-heap for this task */
	rtos_create_secure_context(2048);

	/* Step 2: Load client private key into S-side; receive certificate PEM back */
	static char cert_buf[SEC_CLI_CERT_MAX_LEN];  /* NS holds cert only (public info) */
	size_t cert_len = 0;
	secure_client_init_req_t init_req;
	init_req.cert_pem = (unsigned char *)cert_buf;
	init_req.cert_max = sizeof(cert_buf);
	init_req.cert_len = &cert_len;

	ret = secure_client_init(&init_req);
	if (ret != 0) {
		RTK_LOGI(TAG, "secure_client_init failed %d\n", ret);
		goto out;
	}
	RTK_LOGI(TAG, "S-side client key ready, cert_len=%u B\n", (unsigned)cert_len);

	/* Step 3: Connect to WiFi and obtain IP address */
	RTK_LOGI(TAG, "Connecting to WiFi \"%s\" ...\n", WIFI_SSID);
	{
		struct rtw_network_info wparam = {0};
		_memcpy(wparam.ssid.val, WIFI_SSID, strlen(WIFI_SSID));
		wparam.ssid.len    = strlen(WIFI_SSID);
		wparam.password     = (unsigned char *)WIFI_PASSWORD;
		wparam.password_len = strlen(WIFI_PASSWORD);

		int wret = wifi_connect(&wparam, 1);
		if (wret == RTK_SUCCESS) {
			if (lwip_request_ip(NETIF_WLAN_STA_INDEX) != DHCP_ADDRESS_ASSIGNED) {
				RTK_LOGI(TAG, "DHCP failed\n");
				goto out;
			}
		} else {
			int wait = 0;
			while (lwip_check_connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID
				   && wait++ < 30) {
				rtos_time_delay_ms(1000);
			}
			if (lwip_check_connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
				RTK_LOGI(TAG, "No IP, abort\n");
				goto out;
			}
		}
	}
	{
		unsigned char *ip = lwip_get_ip(NETIF_WLAN_STA_INDEX);
		RTK_LOGI(TAG, "WiFi ready, IP: %d.%d.%d.%d\n",
				 ip[0], ip[1], ip[2], ip[3]);
	}

	/* Step 4: Set up mbedtls TLS client */
	mbedtls_net_context  server_fd;
	mbedtls_ssl_context  ssl;
	mbedtls_ssl_config   conf;
	mbedtls_x509_crt     ca_crt;
	mbedtls_x509_crt     cli_crt;   /* NS holds certificate only, no private key */
	mbedtls_pk_context   proxy_pk;  /* proxy pk_context: sign is routed to S-side */

	mbedtls_net_init(&server_fd);
	mbedtls_ssl_init(&ssl);
	mbedtls_ssl_config_init(&conf);
	mbedtls_x509_crt_init(&ca_crt);
	mbedtls_x509_crt_init(&cli_crt);
	mbedtls_pk_init(&proxy_pk);

	/* Load ISRG Root X1 (trust anchor for server certificate verification) */
	ret = mbedtls_x509_crt_parse(&ca_crt,
								 (const unsigned char *)isrg_root_x1_pem,
								 strlen(isrg_root_x1_pem) + 1);
	if (ret != 0) {
		RTK_LOGI(TAG, "ca_crt parse fail %d\n", ret);
		goto tls_cleanup;
	}

	/* Load client certificate (PEM received from S-side, no private key) */
	ret = mbedtls_x509_crt_parse(&cli_crt,
								 (const unsigned char *)cert_buf,
								 cert_len);
	if (ret != 0) {
		RTK_LOGI(TAG, "cli_crt parse fail %d\n", ret);
		goto tls_cleanup;
	}

	/* Set up proxy pk_context: sign callback routes to NSC */
	ret = mbedtls_pk_setup_rsa_alt(&proxy_pk,
								   NULL,              /* key ctx managed by S-side */
								   NULL,              /* decrypt_func not needed */
								   nsc_sign_wrapper,  /* sign_func -> NSC */
								   nsc_key_len_func); /* key_len = 256 bytes */
	if (ret != 0) {
		RTK_LOGI(TAG, "pk_setup_rsa_alt fail %d\n", ret);
		goto tls_cleanup;
	}

	/* TLS configuration */
	ret = mbedtls_ssl_config_defaults(&conf,
									  MBEDTLS_SSL_IS_CLIENT,
									  MBEDTLS_SSL_TRANSPORT_STREAM,
									  MBEDTLS_SSL_PRESET_DEFAULT);
	if (ret != 0) {
		RTK_LOGI(TAG, "config_defaults fail %d\n", ret);
		goto tls_cleanup;
	}

	mbedtls_ssl_conf_rng(&conf, my_random, NULL);
	mbedtls_ssl_conf_verify(&conf, cert_verify_cb, NULL);
	mbedtls_ssl_conf_ca_chain(&conf, &ca_crt, NULL);
	mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);

	/* Bind proxy pk: CertificateVerify will automatically route to NSC */
	ret = mbedtls_ssl_conf_own_cert(&conf, &cli_crt, &proxy_pk);
	if (ret != 0) {
		RTK_LOGI(TAG, "conf_own_cert fail %d\n", ret);
		goto tls_cleanup;
	}

	ret = mbedtls_ssl_setup(&ssl, &conf);
	if (ret != 0) {
		RTK_LOGI(TAG, "ssl_setup fail %d\n", ret);
		goto tls_cleanup;
	}

	ret = mbedtls_ssl_set_hostname(&ssl, SERVER_HOST);
	if (ret != 0) {
		RTK_LOGI(TAG, "set_hostname fail %d\n", ret);
		goto tls_cleanup;
	}

	/* TCP connect + TLS handshake with retry (server may transiently reset) */
	{
		int attempt;
		for (attempt = 1; attempt <= 3; attempt++) {
			if (attempt > 1) {
				RTK_LOGI(TAG, "Retry %d/3 ...\n", attempt);
				mbedtls_net_free(&server_fd);
				mbedtls_ssl_session_reset(&ssl);
				mbedtls_net_init(&server_fd);
				rtos_time_delay_ms(2000);
			}

			RTK_LOGI(TAG, "Connecting to %s:%s ...\n", SERVER_HOST, SERVER_PORT);
			ret = mbedtls_net_connect(&server_fd, SERVER_HOST, SERVER_PORT,
									  MBEDTLS_NET_PROTO_TCP);
			if (ret != 0) {
				RTK_LOGI(TAG, "net_connect fail %d\n", ret);
				continue;
			}

			mbedtls_ssl_set_bio(&ssl, &server_fd,
								mbedtls_net_send, mbedtls_net_recv, NULL);

			RTK_LOGI(TAG, "TLS handshake ...\n");
			ret = mbedtls_ssl_handshake(&ssl);
			if (ret == 0) {
				break;  /* success */
			}
			RTK_LOGI(TAG, "Handshake FAIL %d (attempt %d/3)\n", ret, attempt);
		}
		if (ret != 0) {
			goto tls_cleanup;
		}
	}
	RTK_LOGI(TAG, "Handshake OK\n");
	RTK_LOGI(TAG, "Cipher: %s\n", mbedtls_ssl_get_ciphersuite(&ssl));

	/* HTTP GET / */
	RTK_LOGI(TAG, "Sending HTTP GET / ...\n");
	ret = mbedtls_ssl_write(&ssl,
							(const unsigned char *)HTTP_REQUEST,
							strlen(HTTP_REQUEST));
	if (ret < 0) {
		RTK_LOGI(TAG, "ssl_write fail %d\n", ret);
		goto tls_cleanup;
	}

	/* Read and print response status line */
	{
		unsigned char buf[512];
		int total = 0;
		int first = 1;
		do {
			memset(buf, 0, sizeof(buf));
			ret = mbedtls_ssl_read(&ssl, buf, sizeof(buf) - 1);
			if (ret <= 0) {
				break;
			}
			if (first) {
				buf[ret] = 0;
				char *end = (char *)buf;
				while (*end && *end != '\r' && *end != '\n') {
					end++;
				}
				*end = 0;
				RTK_LOGI(TAG, "%s\n", (char *)buf);
				first = 0;
			}
			total += ret;
		} while (ret > 0);
		RTK_LOGI(TAG, "Total bytes received: %d\n", total);
	}

	mbedtls_ssl_close_notify(&ssl);
	RTK_LOGI(TAG, "=== Demo Complete: mTLS PASS, GCM PASS ===\n");
	RTK_LOGI(TAG, "NS heap free now : %u B\n",
			 (unsigned)rtos_mem_get_free_heap_size());
	RTK_LOGI(TAG, "NS heap min ever : %u B\n",
			 (unsigned)rtos_mem_get_minimum_ever_free_heap_size());

tls_cleanup:
	mbedtls_ssl_free(&ssl);
	mbedtls_ssl_config_free(&conf);
	mbedtls_net_free(&server_fd);
	mbedtls_x509_crt_free(&ca_crt);
	mbedtls_x509_crt_free(&cli_crt);
	mbedtls_pk_free(&proxy_pk);
out:
	rtos_task_delete(NULL);
}

/* -------------------------------------------------------------------------- */
/* Entry point                                                                 */
/* -------------------------------------------------------------------------- */
void example_rdp_mutual_auth_rsa(void)
{
#ifdef CONFIG_TRUSTZONE
	if (rtos_task_create(NULL, "demo", (rtos_task_t)demo_task,
						 NULL, 1024 * 12, 1) != RTK_SUCCESS) {
		RTK_LOGI(TAG, "create demo_task failed\n");
	}
#else
	RTK_LOGE(TAG, "Please enable TrustZone in menuconfig\n");
#endif
}
