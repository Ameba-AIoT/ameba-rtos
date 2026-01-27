#include "lwip_netconf.h"
#include "httpc_util.h"


uint8_t httpc_debug = HTTPC_DEBUG_ON;
uint8_t httpc_http_1_0_request_used = 0;
extern int httpc_setsockopt_rcvtimeo(struct httpc_conn *conn, int recv_timeout);

struct httpc_conn *httpc_conn_new(uint8_t secure, char *client_cert, char *client_key, char *ca_certs)
{
	int ret = 0;
	struct httpc_conn *conn = NULL;

	conn = (struct httpc_conn *) httpc_malloc(sizeof(struct httpc_conn));

	if (conn) {
		memset(conn, 0, sizeof(struct httpc_conn));
		conn->sock = -1;

		if (secure) {
			conn->tls = httpc_tls_new(&conn->sock, client_cert, client_key, ca_certs);

			if (conn->tls == NULL) {
				httpc_log("ERROR: httpc_tls_new");
				ret = -1;
				goto exit;
			}
		}
	} else {
		httpc_log("ERROR: httpc_malloc");
		ret = -1;
		goto exit;
	}

exit:
	if (ret && conn) {
		if (conn->tls) {
			httpc_tls_free(conn->tls);
		}

		httpc_free(conn);
		conn = NULL;
	}

	return conn;
}

void httpc_conn_free(struct httpc_conn *conn)
{
	if (conn->tls) {
		httpc_tls_free(conn->tls);
	}

	if (conn->sock != -1) {
		close(conn->sock);
	}

	if (conn->response.header) {
		httpc_free(conn->response.header);
	}

	if (conn->request_header) {
		httpc_free(conn->request_header);
	}

	if (conn->host) {
		httpc_free(conn->host);
	}

	if (conn->user_password) {
		httpc_free(conn->user_password);
	}

	httpc_free(conn);
}

int httpc_conn_connect(struct httpc_conn *conn, char *host, uint16_t port, uint32_t timeout)
{
	int ret = 0, non_blocked, recv_timeout = 0;
	struct sockaddr_in server_addr;
	struct hostent *server_host;
	fd_set fdset;
	struct timeval select_timeout;
	struct timeval send_timeout;

	recv_timeout = timeout * 1000;
	send_timeout.tv_sec = timeout;
	send_timeout.tv_usec = 0;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	// Support SERVER_HOST in IP or domain name
	server_host = gethostbyname(host);

	if (server_host) {
		memcpy((void *) &server_addr.sin_addr, (void *) server_host->h_addr, 4);
	} else {
		httpc_log("ERROR: gethostbyname");
		ret = -1;
		goto exit;
	}

	conn->host = (char *) httpc_malloc(strlen(host) + 1);

	if (conn->host) {
		memcpy(conn->host, host, (strlen(host) + 1));
	} else {
		httpc_log("ERROR: httpc_malloc");
		ret = -1;
		goto exit;
	}

	conn->port = port;

	if ((conn->sock = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
		if (timeout) {
			// set nonblocked for connect
			non_blocked = 1;

			if (lwip_ioctl(conn->sock, FIONBIO, &non_blocked) != 0) {
				httpc_log("ERROR: FIONBIO");
				ret = -1;
				goto exit;
			}

			connect(conn->sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
			non_blocked = 0;

			if (lwip_ioctl(conn->sock, FIONBIO, &non_blocked) != 0) {
				httpc_log("ERROR: FIONBIO");
				ret = -1;
				goto exit;
			}

			select_timeout.tv_sec = timeout;
			select_timeout.tv_usec = 0;
			FD_ZERO(&fdset);
			FD_SET(conn->sock, &fdset);

			if (select(conn->sock + 1, NULL, &fdset, NULL, &select_timeout) != 1) {
				httpc_log("ERROR: select");
				ret = -1;
				goto exit;
			}

#if LWIP_SO_RCVTIMEO
			if (httpc_setsockopt_rcvtimeo(conn, recv_timeout) < 0) {
				httpc_log("ERROR: setsockopt");
				ret = -1;
				goto exit;
			}
#endif
#if LWIP_SO_SNDTIMEO
			if (setsockopt(conn->sock, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, sizeof(send_timeout)) < 0) {
				httpc_log("ERROR: setsockopt");
				ret = -1;
				goto exit;
			}
#endif
		} else {
			if (connect(conn->sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
				httpc_log("ERROR: connect");
				ret = -1;
				goto exit;
			}
		}

		if (conn->tls) {
			if (httpc_tls_handshake(conn->tls, host) != 0) {
				httpc_log("ERROR: httpc_tls_handshake");
				ret = -1;
				goto exit;
			}
		}
	} else {
		httpc_log("ERROR: socket");
		ret = -1;
		goto exit;
	}

exit:
	return ret;
}

void httpc_conn_close(struct httpc_conn *conn)
{
	if (conn->tls) {
		httpc_tls_close(conn->tls);
	}

	if (conn->sock != -1) {
		close(conn->sock);
		conn->sock = -1;
	}
}

int httpc_conn_setup_user_password(struct httpc_conn *conn, char *user, char *password)
{
	int ret = 0;
	size_t auth_len = strlen(user) + 1 + strlen(password);
	size_t base64_len = (auth_len + 2) / 3 * 4 + 1;
	uint8_t *auth = NULL;

	auth = (uint8_t *) httpc_malloc(auth_len);

	if (auth) {
		memcpy(auth, user, strlen(user));
		memcpy(auth + strlen(user), ":", 1);
		memcpy(auth + strlen(user) + 1, password, strlen(password));
		conn->user_password = (char *) httpc_malloc(base64_len);

		if (conn->user_password) {
			memset(conn->user_password, 0, base64_len);
			ret = httpc_base64_encode(auth, auth_len, conn->user_password, base64_len);

			if (ret) {
				httpc_log("ERROR: httpc_base64_encode");
				ret = -1;
				goto exit;
			}

			httpc_log_verbose("setup conn->user_password[%s]", conn->user_password);
		} else {
			httpc_log("ERROR: httpc_malloc");
			ret = -1;
			goto exit;
		}
	} else {
		httpc_log("ERROR: httpc_malloc");
		ret = -1;
		goto exit;
	}

exit:
	if (auth) {
		httpc_free(auth);
	}

	if (ret) {
		httpc_free(conn->user_password);
		conn->user_password = NULL;
	}

	return ret;
}

void httpc_setup_debug(uint8_t debug)
{
	httpc_debug = debug;
}

void httpc_enable_ignore_content_len(struct httpc_conn *conn)
{
	conn->ignore_content_len = 1;
}

void httpc_set_http_1_0_request_used(uint8_t used)
{
	if (used) {
		httpc_http_1_0_request_used = 1;
	} else {
		httpc_http_1_0_request_used = 0;
	}
}
