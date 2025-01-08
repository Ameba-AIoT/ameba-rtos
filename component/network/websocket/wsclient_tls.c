#include "platform_autoconf.h"
#include <websocket/libwsclient.h>

#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"

struct wss_tls {
	mbedtls_ssl_context ctx;
	mbedtls_ssl_config conf;
	mbedtls_net_context socket;
	mbedtls_x509_crt ca;             /*!< CA certificates */
	mbedtls_x509_crt cert;           /*!< Certificate */
	mbedtls_pk_context key;          /*!< Private key */
};

static char *ws_itoa(int value)
{
	char *val_str;
	int tmp = value, len = 1;

	while ((tmp /= 10) > 0) {
		len ++;
	}

	val_str = (char *) rtos_mem_malloc(len + 1);
	sprintf(val_str, "%d", value);

	return val_str;
}

int ws_random(void *p_rng, unsigned char *output, size_t output_len);
void *wss_tls_connect(int *sock, char *host, int port)
{
	int ret;
	struct wss_tls *tls = NULL;

	tls = (struct wss_tls *) malloc(sizeof(struct wss_tls));

	if (tls) {
		mbedtls_ssl_context *ssl = &tls->ctx;
		mbedtls_ssl_config *conf = &tls->conf;
		mbedtls_net_context *server_fd = &tls->socket;
		memset(tls, 0, sizeof(struct wss_tls));

		server_fd->fd = *sock;
		char *port_str = ws_itoa(port);

		if ((ret = mbedtls_net_connect(server_fd, host, port_str, MBEDTLS_NET_PROTO_TCP)) != 0) {
			printf("\n[WSCLIENT] ERROR: net_connect %d\n", ret);
			free(port_str);
			goto exit;
		}

		free(port_str);
		*sock = server_fd->fd;
		mbedtls_ssl_init(ssl);
		mbedtls_ssl_config_init(conf);
		mbedtls_ssl_set_bio(ssl, server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

		if ((ret = mbedtls_ssl_config_defaults(conf,
											   MBEDTLS_SSL_IS_CLIENT,
											   MBEDTLS_SSL_TRANSPORT_STREAM,
											   MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
			printf("\n[WSCLIENT] ERROR: ssl_config %d\n", ret);
			goto exit;
		}

		mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_NONE);
		mbedtls_ssl_conf_rng(conf, ws_random, NULL);

		if ((ret = mbedtls_ssl_setup(ssl, conf)) != 0) {
			printf("\n[WSCLIENT] ERROR: ssl_setup %d\n", ret);
			goto exit;
		}
	} else {
		printf("\n[WSCLIENT] ERROR: malloc\n");
		ret = -1;
		goto exit;
	}
exit:
	if (ret && tls) {
		mbedtls_net_free(&tls->socket);
		mbedtls_ssl_free(&tls->ctx);
		mbedtls_ssl_config_free(&tls->conf);
		free(tls);
		tls = NULL;
	}
	return (void *) tls;
}

static int _verify_func(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
	/* To avoid gcc warnings */
	(void) data;
	(void) depth;

	char buf[1024];
	mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);

	if (*flags) {
		WSCLIENT_ERROR("ERROR: certificate verify\n%s\n", buf);
	}

	return 0;
}

int wss_tls_set_cert_and_key(wsclient_context *wsclient, char *client_cert, char *client_key, char *ca_cert)
{
	int ret = 0;
	struct wss_tls *tls = (struct wss_tls *)wsclient->tls;
	mbedtls_ssl_config *conf = &tls->conf;

	if (wsclient == NULL) {
		WSCLIENT_ERROR("ERROR: wsclient is NULL\n");
		ret = -1;
		goto exit;
	}

	mbedtls_x509_crt_init(&tls->ca);
	mbedtls_x509_crt_init(&tls->cert);
	mbedtls_pk_init(&tls->key);

	if (client_cert && client_key) {
		if ((ret = mbedtls_x509_crt_parse(&tls->cert, (const unsigned char *) client_cert, strlen(client_cert) + 1)) != 0) {
			WSCLIENT_ERROR("ERROR: mbedtls_x509_crt_parse %d\n", ret);
			goto exit;
		}

		if ((ret = mbedtls_pk_parse_key(&tls->key, (const unsigned char *) client_key, strlen(client_key) + 1, NULL, 0, NULL, NULL)) != 0) {
			WSCLIENT_ERROR("ERROR: mbedtls_pk_parse_key %d\n", ret);
			ret = -1;
			goto exit;
		}

		if ((ret = mbedtls_ssl_conf_own_cert(conf, &tls->cert, &tls->key)) != 0) {
			WSCLIENT_ERROR("ERROR: mbedtls_ssl_conf_own_cert %d\n", ret);
			ret = -1;
			goto exit;
		}
	}

	if (ca_cert) {
		if ((ret = mbedtls_x509_crt_parse(&tls->ca, (const unsigned char *) ca_cert, strlen(ca_cert) + 1)) != 0) {
			WSCLIENT_ERROR("ERROR: mbedtls_x509_crt_parse %d\n", ret);
			ret = -1;
			goto exit;
		}

		mbedtls_ssl_conf_ca_chain(conf, &tls->ca, NULL);
		mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_REQUIRED);
		mbedtls_ssl_conf_verify(conf, _verify_func, NULL);
	}

exit:
	return ret;
}

int wss_tls_handshake(void *tls_in)
{
	struct wss_tls *tls = (struct wss_tls *) tls_in;

	int ret;

	if ((ret = mbedtls_ssl_handshake(&tls->ctx)) != 0) {
		printf("\n[WSCLIENT] ERROR: ssl_handshake -0x%x\n", -ret);
		ret = -1;
	} else {
		printf("\n[WSCLIENT] Use ciphersuite %s\n", mbedtls_ssl_get_ciphersuite(&tls->ctx));
	}

	return ret;
}


void wss_tls_close(void *tls_in, int *sock)
{
	struct wss_tls *tls = (struct wss_tls *) tls_in;

	if (tls) {
		mbedtls_ssl_close_notify(&tls->ctx);
	} else {
		return;
	}

	if (*sock != -1) {
		mbedtls_net_free(&tls->socket);
		*sock = -1;
	}
	mbedtls_ssl_free(&tls->ctx);
	mbedtls_x509_crt_free(&tls->ca);
	mbedtls_x509_crt_free(&tls->cert);
	mbedtls_pk_free(&tls->key);
	if (tls) {
		mbedtls_ssl_config_free(&tls->conf);
	}
	free(tls);
	tls = NULL;
}

int wss_tls_write(void *tls_in, char *request, int request_len)
{
	int ret;
	struct wss_tls *tls = (struct wss_tls *) tls_in;

	ret = mbedtls_ssl_write(&tls->ctx, (unsigned char const *)request, request_len);
	if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
		ret = 0;
	}
	return ret;
}

int wss_tls_read(void *tls_in, char *buffer, int buf_len)
{
	int ret;
	struct wss_tls *tls = (struct wss_tls *) tls_in;

	ret = mbedtls_ssl_read(&tls->ctx, (unsigned char *)buffer, buf_len);
	if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE
		|| ret == MBEDTLS_ERR_NET_RECV_FAILED) {
		ret = 0;
	}
	return ret;
}