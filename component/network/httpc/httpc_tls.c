#include "platform_stdlib.h"
#include "basic_types.h"
#include "os_wrapper.h"
#include "ameba.h"
#include <lwipconf.h>
#include "httpc.h"

int httpc_setsockopt_rcvtimeo(struct httpc_conn *conn, int recv_timeout)
{
	int ret = 0;
#if defined(LWIP_SO_SNDRCVTIMEO_NONSTANDARD) && (LWIP_SO_SNDRCVTIMEO_NONSTANDARD == 0)	//lwip 2.0.2
	struct timeval timeout;
	timeout.tv_sec  = recv_timeout / 1000;
	timeout.tv_usec = (recv_timeout % 1000) * 1000;
	ret = setsockopt(conn->sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#else	//lwip 1.4.1
	ret = setsockopt(conn->sock, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));
#endif
	return ret;
}


#include "mbedtls/ssl.h"
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/base64.h"
#include "mbedtls/config.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include "mbedtls/version.h"

struct httpc_tls {
	mbedtls_ssl_context ctx;         /*!< Context for mbedTLS */
	mbedtls_ssl_config conf;         /*!< Configuration for mbedTLS */
	mbedtls_x509_crt ca;             /*!< CA certificates */
	mbedtls_x509_crt cert;           /*!< Certificate */
	mbedtls_pk_context key;          /*!< Private key */
};

static int _verify_func(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
	/* To avoid gcc warnings */
	(void) data;
	(void) depth;

	char buf[1024];
	mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);

	if (*flags) {
		printf("\n[HTTPC] ERROR: certificate verify\n%s\n", buf);
	} else {
		//printf("\n[HTTPC] Certificate verified\n%s\n", buf);
	}

	return 0;
}

static int _random_func(void *p_rng, unsigned char *output, size_t output_len)
{
	/* To avoid gcc warnings */
	(void) p_rng;

	TRNG_get_random_bytes(output, output_len);
	return 0;
}

void *httpc_tls_new(int *sock, char *client_cert, char *client_key, char *ca_certs)
{
	int ret = 0;
	struct httpc_tls *tls = NULL;

	tls = (struct httpc_tls *) malloc(sizeof(struct httpc_tls));

	if (tls) {
		mbedtls_ssl_context *ssl = &tls->ctx;
		mbedtls_ssl_config *conf = &tls->conf;

		memset(tls, 0, sizeof(struct httpc_tls));
		mbedtls_x509_crt_init(&tls->ca);
		mbedtls_x509_crt_init(&tls->cert);

		mbedtls_pk_init(&tls->key);
		mbedtls_ssl_init(ssl);
		mbedtls_ssl_config_init(conf);

		if ((ret = mbedtls_ssl_config_defaults(conf,
											   MBEDTLS_SSL_IS_CLIENT,
											   MBEDTLS_SSL_TRANSPORT_STREAM,
											   MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {

			printf("\n[HTTPC] ERROR: mbedtls_ssl_config_defaults %d\n", ret);
			ret = -1;
			goto exit;
		}

		mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_NONE);
		mbedtls_ssl_conf_rng(conf, _random_func, NULL);

		if (client_cert && client_key) {
			if ((ret = mbedtls_x509_crt_parse(&tls->cert, (const unsigned char *) client_cert, strlen(client_cert) + 1)) != 0) {
				printf("\n[HTTPC] ERROR: mbedtls_x509_crt_parse %d\n", ret);
				ret = -1;
				goto exit;
			}

#if defined(MBEDTLS_VERSION_NUMBER) && (MBEDTLS_VERSION_NUMBER >= 0x03000000)
			if ((ret = mbedtls_pk_parse_key(&tls->key, (const unsigned char *) client_key, strlen(client_key) + 1, NULL, 0, NULL, NULL)) != 0) {
#else
			if ((ret = mbedtls_pk_parse_key(&tls->key, (const unsigned char *) client_key, strlen(client_key) + 1, NULL, 0)) != 0) {
#endif
				printf("\n[HTTPC] ERROR: mbedtls_pk_parse_key %d\n", ret);
				ret = -1;
				goto exit;
			}

			if ((ret = mbedtls_ssl_conf_own_cert(conf, &tls->cert, &tls->key)) != 0) {
				printf("\n[HTTPC] ERROR: mbedtls_ssl_conf_own_cert %d\n", ret);
				ret = -1;
				goto exit;
			}
		}

		if (ca_certs) {
			// set trusted ca certificates next to client certificate
			if ((ret = mbedtls_x509_crt_parse(&tls->ca, (const unsigned char *) ca_certs, strlen(ca_certs) + 1)) != 0) {
				printf("\n[HTTPC] ERROR: mbedtls_x509_crt_parse %d\n", ret);
				ret = -1;
				goto exit;
			}

			mbedtls_ssl_conf_ca_chain(conf, &tls->ca, NULL);
			mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_REQUIRED);
			mbedtls_ssl_conf_verify(conf, _verify_func, NULL);
		}

		if ((ret = mbedtls_ssl_setup(ssl, conf)) != 0) {
			printf("\n[HTTPC] ERROR: mbedtls_ssl_setup %d\n", ret);
			ret = -1;
			goto exit;
		}

		mbedtls_ssl_set_bio(ssl, sock, mbedtls_net_send, mbedtls_net_recv, NULL);
	} else {
		printf("\n[HTTPC] ERROR: malloc\n");
		ret = -1;
		goto exit;
	}

exit:
	if (ret && tls) {
		mbedtls_ssl_free(&tls->ctx);
		mbedtls_ssl_config_free(&tls->conf);

		mbedtls_x509_crt_free(&tls->ca);
		mbedtls_x509_crt_free(&tls->cert);

		mbedtls_pk_free(&tls->key);

		free(tls);
		tls = NULL;
	}

	return (void *) tls;
}

void httpc_tls_free(void *tls_in)
{
	struct httpc_tls *tls = (struct httpc_tls *) tls_in;

	mbedtls_ssl_free(&tls->ctx);
	mbedtls_ssl_config_free(&tls->conf);
	mbedtls_x509_crt_free(&tls->ca);
	mbedtls_x509_crt_free(&tls->cert);
	mbedtls_pk_free(&tls->key);

	free(tls);
}

int httpc_tls_handshake(void *tls_in, char *host)
{
	struct httpc_tls *tls = (struct httpc_tls *) tls_in;
	int ret = 0;

	mbedtls_ssl_set_hostname(&tls->ctx, host);

	if ((ret = mbedtls_ssl_handshake(&tls->ctx)) != 0) {
		printf("\n[HTTPC] ERROR: mbedtls_ssl_handshake %d\n", ret);
		ret = -1;
	} else {
		printf("\n[HTTPC] Use ciphersuite %s\n", mbedtls_ssl_get_ciphersuite(&tls->ctx));
	}

	return ret;
}

void httpc_tls_close(void *tls_in)
{
	struct httpc_tls *tls = (struct httpc_tls *) tls_in;

	mbedtls_ssl_close_notify(&tls->ctx);
}

int httpc_tls_read(void *tls_in, uint8_t *buf, size_t buf_len)
{
	struct httpc_tls *tls = (struct httpc_tls *) tls_in;

	return mbedtls_ssl_read(&tls->ctx, buf, buf_len);
}

int httpc_tls_write(void *tls_in, uint8_t *buf, size_t buf_len)
{
	struct httpc_tls *tls = (struct httpc_tls *) tls_in;

	return mbedtls_ssl_write(&tls->ctx, buf, buf_len);
}

int httpc_base64_encode(uint8_t *data, size_t data_len, char *base64_buf, size_t buf_len)
{
	int ret = 0;
	size_t output_len = 0;

	if ((ret = mbedtls_base64_encode((unsigned char *)base64_buf, buf_len, &output_len, data, data_len)) != 0) {
		printf("\n[HTTPC] ERROR: mbedtls_base64_encode %d\n", ret);
		ret = -1;
	}

	return ret;
}

int httpc_tls_set_ciphersuites(struct httpc_conn *conn, int *ciphersuites)
{
	mbedtls_ssl_context *ssl_ctx = (mbedtls_ssl_context *) conn->tls;

	if (ssl_ctx) {
		mbedtls_ssl_config *ssl_conf = (mbedtls_ssl_config *) ssl_ctx->conf;

		if (ssl_conf && ciphersuites) {
			mbedtls_ssl_conf_ciphersuites(ssl_conf, ciphersuites);
			return 0;
		}
	}

	return -1;
}
