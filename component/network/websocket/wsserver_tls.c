#include "ameba_soc.h"
#include "wsserver_api.h"
#include "ws_server_msg.h"

#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/base64.h"
#include "mbedtls/sha1.h"

struct wss_tls {
	mbedtls_ssl_context ctx;
	mbedtls_ssl_config conf;
};

static mbedtls_x509_crt wss_certs; /*!< Certificates of server and CA */
static mbedtls_pk_context wss_key; /*!< Private key of server */

static int _verify_func(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
	/* To avoid gcc warnings */
	(void) data;
	(void) depth;

	char buf[1024];
	mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);

	if (*flags) {
		printf("\n[WS_SERVER] ERROR: certificate verify\n%s\n", buf);
	} else {
		printf("\n[WS_SERVER] Certificate verified\n%s\n", buf);
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


int ws_server_tls_setup_init(const char *server_cert, const char *server_key, const char *ca_certs)
{
	int ret = 0;

	memset(&wss_certs, 0, sizeof(mbedtls_x509_crt));
	memset(&wss_key, 0, sizeof(mbedtls_pk_context));
	mbedtls_x509_crt_init(&wss_certs);
	mbedtls_pk_init(&wss_key);

	// set server certificate for the first certificate
	if ((ret = mbedtls_x509_crt_parse(&wss_certs, (const unsigned char *) server_cert, strlen(server_cert) + 1)) != 0) {
		printf("\n[WS_SERVER] ERROR: mbedtls_x509_crt_parse %d\n", ret);
		ret = -1;
		goto exit;
	}

	// set trusted ca certificates next to server certificate
	if ((ret = mbedtls_x509_crt_parse(&wss_certs, (const unsigned char *) ca_certs, strlen(ca_certs) + 1)) != 0) {
		printf("\n[WS_SERVER] ERROR: mbedtls_x509_crt_parse %d\n", ret);
		ret = -1;
		goto exit;
	}

	if ((ret = mbedtls_pk_parse_key(&wss_key, (const unsigned char *) server_key, strlen(server_key) + 1, NULL, 0, TRNG_get_random_bytes_f_rng, (void *)1)) != 0) {
		printf("\n[WS_SERVER] ERROR: mbedtls_pk_parse_key %d\n", ret);
		ret = -1;
		goto exit;
	}

exit:
	if (ret) {
		mbedtls_x509_crt_free(&wss_certs);
		mbedtls_pk_free(&wss_key);
	}

	return ret;
}

void ws_server_tls_setup_free(void)
{
	mbedtls_x509_crt_free(&wss_certs);
	mbedtls_pk_free(&wss_key);
}

void *ws_server_tls_new_handshake(int *sock, uint8_t secure)
{
	int ret = 0;
	struct wss_tls *tls = NULL;
	mbedtls_ssl_context *ssl;
	mbedtls_ssl_config *conf;

	if ((tls = (struct wss_tls *) malloc(sizeof(struct wss_tls))) != NULL) {
		memset(tls, 0, sizeof(struct wss_tls));
		ssl = &tls->ctx;
		conf = &tls->conf;

		mbedtls_ssl_init(ssl);
		mbedtls_ssl_config_init(conf);

		if ((ret = mbedtls_ssl_config_defaults(conf,
											   MBEDTLS_SSL_IS_SERVER,
											   MBEDTLS_SSL_TRANSPORT_STREAM,
											   MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {

			printf("\n[WS_SERVER] ERROR: mbedtls_ssl_config_defaults %d\n", ret);
			ret = -1;
			goto exit;
		}

		mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_NONE);
		mbedtls_ssl_conf_rng(conf, _random_func, NULL);
		mbedtls_ssl_conf_ca_chain(conf, wss_certs.next, NULL);

		if (secure == WS_SERVER_SECURE_TLS_VERIFY) {
			mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_REQUIRED);
			mbedtls_ssl_conf_verify(conf, _verify_func, NULL);
		}

		if ((ret = mbedtls_ssl_conf_own_cert(conf, &wss_certs, &wss_key)) != 0) {
			printf("\n[WS_SERVER] ERROR: mbedtls_ssl_conf_own_cert %d\n", ret);
			ret = -1;
			goto exit;
		}

		if ((ret = mbedtls_ssl_setup(ssl, conf)) != 0) {
			printf("\n[WS_SERVER] ERROR: mbedtls_ssl_setup %d\n", ret);
			ret = -1;
			goto exit;
		}

		mbedtls_ssl_set_bio(ssl, sock, mbedtls_net_send, mbedtls_net_recv, NULL);

		if ((ret = mbedtls_ssl_handshake(ssl)) != 0) {
			printf("\n[WS_SERVER] ERROR: mbedtls_ssl_handshake %d\n", ret);
			ret = -1;
			goto exit;
		} else {
			printf("\n[WS_SERVER] Use ciphersuite %s\n", mbedtls_ssl_get_ciphersuite(ssl));
		}

	} else {
		printf("\n[WS_SERVER] ERROR: wss malloc\n");
		ret = -1;
		goto exit;
	}

exit:
	if (ret && tls) {
		mbedtls_ssl_close_notify(ssl);
		mbedtls_ssl_free(ssl);
		mbedtls_ssl_config_free(conf);
		free(tls);
		tls = NULL;
	}

	return (void *) tls;
}

void ws_server_tls_close(void *tls_in)
{
	struct wss_tls *tls = (struct wss_tls *) tls_in;

	mbedtls_ssl_close_notify(&tls->ctx);
}

void ws_server_tls_free(void *tls_in)
{
	struct wss_tls *tls = (struct wss_tls *) tls_in;

	mbedtls_ssl_free(&tls->ctx);
	mbedtls_ssl_config_free(&tls->conf);
	free(tls);
}

int ws_server_tls_write(void *tls_in, uint8_t *buf, size_t buf_len)
{
	int ret;
	struct wss_tls *tls = (struct wss_tls *) tls_in;

	ret = mbedtls_ssl_write(&tls->ctx, buf, buf_len);
	if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
		ret = 0;
	}
	return ret;
}

int ws_server_tls_read(void *tls_in, uint8_t *buf, size_t buf_len)
{
	int ret;
	struct wss_tls *tls = (struct wss_tls *) tls_in;

	ret = mbedtls_ssl_read(&tls->ctx, buf, buf_len);
	if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE
		|| ret == MBEDTLS_ERR_NET_RECV_FAILED) {
		ret = 0;
	}
	return ret;
}

int ws_server_base64_encode(uint8_t *data, size_t data_len, char *base64_buf, size_t buf_len)
{
	int ret = 0;
	size_t output_len = 0;

	if ((ret = mbedtls_base64_encode((unsigned char *)base64_buf, buf_len, &output_len, data, data_len)) != 0) {
		printf("\n[WS_SERVER] ERROR: mbedtls_base64_encode %d\n", ret);
		ret = -1;
	}

	return ret;
}

void ws_server_sha1(const unsigned char *input, size_t ilen, unsigned char output[20])
{
	mbedtls_sha1(input, ilen, output);
}
