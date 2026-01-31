#include "lwip_netconf.h"
#include "wsserver_api.h"
#include "ws_server_msg.h"

extern void ws_get_random_bytes(void *buf, size_t len);
extern int ws_server_tls_read(void *tls_in, uint8_t *buf, size_t buf_len);
extern int ws_server_tls_write(void *tls_in, uint8_t *buf, size_t buf_len);
extern int ws_server_base64_encode(uint8_t *data, size_t data_len, char *base64_buf, size_t buf_len);
extern void ws_server_sha1(const unsigned char *input, size_t ilen, unsigned char output[20]);
extern void ws_server_tls_setup_free(void);

extern uint8_t ws_server_max_conn;
extern size_t ws_server_tx_size;
extern size_t ws_server_rx_size;
extern size_t ws_server_tmp_buf_size;
#define WS_SERVER_CRLF    "\r\n"
#define WS_SERVER_MAGIC_STRING	"258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

void (*ws_server_receive_cb)(ws_conn *, int, enum opcode_type) = NULL;

void *ws_server_malloc(size_t size)
{
	void *ptr = (void *)rtos_mem_malloc(size);
	ws_server_log_verbose("%x = malloc(%d)", ptr, size);
	return ptr;
}

void ws_server_free(void *ptr)
{
	rtos_mem_free((u8 *)ptr);

	ws_server_log_verbose("free(%x)", ptr);
}

int ws_server_write(ws_conn *conn, uint8_t *buf, size_t buf_len)
{
	if (conn->tls) {
		return ws_server_tls_write(conn->tls, buf, buf_len);
	}

	//return write(conn->sock, buf, buf_len);
	int ret = 0;
	ret = write(conn->sock, buf, buf_len);

	if (ret < 0 && (errno == EAGAIN || errno == ENOMEM || errno == 0)) {
		return 0;
	} else if (ret < 0) {
		return -1;
	} else {
		return ret;
	}
}

int ws_server_read(ws_conn *conn, uint8_t *buf, size_t buf_len)
{
	if (conn->tls) {
		return ws_server_tls_read(conn->tls, buf, buf_len);
	}

	//return read(conn->sock, buf, buf_len);
	else {
		int ret = 0;
		ret = read(conn->sock, buf, buf_len);

		if (ret < 0 && (errno == EAGAIN || errno == ENOMEM || errno == 0)) {
			return 0;
		} else if (ret < 0) {
			return -1;
		} else {
			return ret;
		}
	}
}

void ws_server_dispatch(void (*callback)(ws_conn *, int, enum opcode_type))
{
	ws_server_receive_cb = callback;
}

void ws_server_dispatchBinary(ws_conn *conn)
{
	int data_start = 0;//the lenth of data already read
	int rlen = conn->rx_len;
	ws_server_log_verbose("%s called by connection %d to dispatch data with length %d", __FUNCTION__, conn->sock, rlen);
	while (1) {
		struct ws_data_header_type ws;
		int rxbuf_len;
		rxbuf_len = rlen - data_start;//lenth of data not read
		if (rxbuf_len < 2) {
			return;
		}
		const uint8_t *data = (uint8_t *) &conn->rxbuf[data_start];  // peek, but don't consume
		ws.fin = (data[0] & 0x80) == 0x80;
		ws.opcode = (enum opcode_type)(data[0] & 0x0f);
		ws.mask = (data[1] & 0x80) == 0x80;
		ws.N0 = (data[1] & 0x7f);
		ws.header_size = 2 + (ws.N0 == 126 ? 2 : 0) + (ws.N0 == 127 ? 8 : 0) + (ws.mask ? 4 : 0);
		if (rxbuf_len < (int)ws.header_size) {
			return;
		}
		int pos = 0;
		if (ws.N0 < 126) {
			ws.N = ws.N0;
			pos = 2;
		} else if (ws.N0 == 126) {
			ws.N = 0;
			ws.N |= ((uint64_t) data[2]) << 8;
			ws.N |= ((uint64_t) data[3]) << 0;
			pos = 4;
		} else if (ws.N0 == 127) {
			ws.N = 0;
			ws.N |= ((uint64_t) data[2]) << 56;
			ws.N |= ((uint64_t) data[3]) << 48;
			ws.N |= ((uint64_t) data[4]) << 40;
			ws.N |= ((uint64_t) data[5]) << 32;
			ws.N |= ((uint64_t) data[6]) << 24;
			ws.N |= ((uint64_t) data[7]) << 16;
			ws.N |= ((uint64_t) data[8]) << 8;
			ws.N |= ((uint64_t) data[9]) << 0;
			pos = 10;
		}
		if (ws.mask) {
			ws.masking_key[0] = ((uint8_t) data[pos + 0]) << 0;
			ws.masking_key[1] = ((uint8_t) data[pos + 1]) << 0;
			ws.masking_key[2] = ((uint8_t) data[pos + 2]) << 0;
			ws.masking_key[3] = ((uint8_t) data[pos + 3]) << 0;
		} else {
			ws.masking_key[0] = 0;
			ws.masking_key[1] = 0;
			ws.masking_key[2] = 0;
			ws.masking_key[3] = 0;
		}
		if ((uint64_t)rxbuf_len < ws.header_size + ws.N) {
			return; /* Need: ws.header_size+ws.N - rxbuf.size() */
		}
		ws_server_log_verbose("websocket message size: 0x%x(H)+0x%x(L)", (uint32_t)((ws.header_size + ws.N) >> 32), (uint32_t)(ws.header_size + ws.N));
		// We got a whole message, now do something with it:
		if (ws.opcode == TEXT_FRAME || ws.opcode == BINARY_FRAME || ws.opcode == CONTINUATION) {
			if (ws.mask) {
				for (size_t i = 0; i != ws.N; ++i) {
					conn->rxbuf[i + data_start + ws.header_size] ^= ws.masking_key[i & 0x3];
				}
			}
			memset(conn->receivedData, 0, ws_server_rx_size);
			memcpy(conn->receivedData, (conn->rxbuf + data_start + ws.header_size), ws.N);
			ws_server_log_verbose("message content: %s", conn->receivedData);
			if (ws.fin != 0) {
				conn->last_data_comm_time = rtos_time_get_current_system_time_ms();
				if (ws_server_receive_cb != NULL) {
					ws_server_log_verbose("Calling ws_server_receive_cb to return received data\r\n");
					ws_server_receive_cb(conn, ws.N, ws.opcode);
				} else {
					ws_server_log_verbose("\r\nmessage content: %s\r\n", conn->receivedData);
				}
			}
		} else if (ws.opcode == PING) {
			if (ws.mask) {
				for (size_t i = 0; i != ws.N; ++i) {
					conn->rxbuf[i + ws.header_size] ^= ws.masking_key[i & 0x3];
				}
			}
			ws_server_log_verbose("Get PING\n");

			int ret = 0;
			if (ws.N > 0) {
				ws_server_sendData(PONG, ws.N, &conn->rxbuf[ws.header_size], 0, WS_SERVER_SEND_DIRECTLY, 1, conn);
			} else {
				uint8_t pong_Frame[2] = {0x8a, 0x00};
				ret = ws_server_write(conn, pong_Frame, sizeof(pong_Frame));
			}

			if (ret < 0) {
				ws_server_log("ERROR: server send PONG failed\n");
				ws_server_set_close_reason(conn, WSS_SEND_PONG_FAILED);
				conn->state = WSS_CLOSED;
				break;
			}
		} else if (ws.opcode == PONG) {
			ws_server_log_verbose("Receive PONG from(%d)\n", conn->sock);
			conn->state = WSS_CONNECTED1;
		} else if (ws.opcode == CLOSE) {
			ws_server_log_verbose("Receive CLOSE, close the conn(%d)\n", conn->sock);
			ws_server_set_close_reason(conn, WSS_RECEIVE_CLIENT_CLOSE);
			conn->state = WSS_CLOSED;
			break;
		} else {
			ws_server_log("ERROR: Got unexpected WebSocket message.\n");
		}
		data_start = data_start + ws.header_size + ws.N;
		conn->rx_len = 0;
	}
}

void ws_server_sendData(uint8_t type, size_t message_size, uint8_t *message, int useMask, uint8_t send_mode, uint8_t fin_flag, ws_conn *conn)
{

	if (conn->state == WSS_CLOSING || conn->state == WSS_CLOSED) {
		return;
	}

	uint8_t masking_key[4];
	uint8_t *header;
	int header_len;

	if (useMask) {
		ws_get_random_bytes(masking_key, 4);
		ws_server_log_verbose("The mask key is %2x, %2x, %2x, %2x\n", masking_key[0], masking_key[1], masking_key[2], masking_key[3]);
	}

	header_len = (2 + (message_size >= 126 ? 2 : 0) + (message_size >= 65536 ? 6 : 0) + (useMask ? 4 : 0));
	header = ws_server_malloc(header_len);
	if (fin_flag) {
		header[0] = 0x80 | type;
	} else {
		header[0] = type;
	}
	if (message_size < 126) {
		header[1] = (message_size & 0xff) | (useMask ? 0x80 : 0);
		if (useMask) {
			header[2] = masking_key[0];
			header[3] = masking_key[1];
			header[4] = masking_key[2];
			header[5] = masking_key[3];
		}
	} else if (message_size < 65536) {
		header[1] = 126 | (useMask ? 0x80 : 0);
		header[2] = (message_size >> 8) & 0xff;
		header[3] = (message_size >> 0) & 0xff;
		if (useMask) {
			header[4] = masking_key[0];
			header[5] = masking_key[1];
			header[6] = masking_key[2];
			header[7] = masking_key[3];
		}
	} else {
		header[1] = 127 | (useMask ? 0x80 : 0);
		header[2] = 0; // (message_size >> 56) & 0xff; exceed message_size limit, should be 0  From Raymond Lee      
		header[3] = 0; // (message_size >> 48) & 0xff; exceed message_size limit, should be 0        
		header[4] = 0; // (message_size >> 40) & 0xff; exceed message_size limit, should be 0
		header[5] = 0;//(message_size >> 32) & 0xff;
		header[6] = (message_size >> 24) & 0xff;
		header[7] = (message_size >> 16) & 0xff;
		header[8] = (message_size >>  8) & 0xff;
		header[9] = (message_size >>  0) & 0xff;
		if (useMask) {
			header[10] = masking_key[0];
			header[11] = masking_key[1];
			header[12] = masking_key[2];
			header[13] = masking_key[3];
		}
	}


	if (send_mode == WS_SERVER_SEND_BY_POLL) {
		memset(conn->txbuf, 0, ws_server_tx_size + 16);
		memcpy(conn->txbuf, header, header_len);
		memcpy(conn->txbuf + header_len, message, message_size);
		conn->tx_len = header_len + message_size;
		ws_server_free(header);

		if (useMask) {
			for (size_t i = 0; i != message_size; i++) {
				conn->txbuf[header_len + i] ^= masking_key[i & 0x3];
			}
		}
	} else if (send_mode == WS_SERVER_SEND_DIRECTLY) {
		uint8_t *ws_tmp_txbuf;
		int ws_tmp_txbuf_len;
		int ret;
		ws_tmp_txbuf = (uint8_t *)ws_server_malloc(ws_server_tx_size + 16);
		if (!ws_tmp_txbuf) {
			ws_server_log("ERROR: ws_tmp_txbuf malloc failed.\n");
			return;
		}
		memset(ws_tmp_txbuf, 0, ws_server_tx_size + 16);
		memcpy(ws_tmp_txbuf, header, header_len);
		memcpy(ws_tmp_txbuf + header_len, message, message_size);
		ws_tmp_txbuf_len = header_len + message_size;
		ws_server_free(header);

		if (useMask) {
			for (size_t i = 0; i != message_size; i++) {
				ws_tmp_txbuf[header_len + i] ^= masking_key[i & 0x3];
			}
		}

		if (ws_tmp_txbuf_len > 0) {
			int remain_len = ws_tmp_txbuf_len;
			do {
				ret = ws_server_write(conn, ws_tmp_txbuf + (ws_tmp_txbuf_len - remain_len), remain_len);
				if (ret == 0) {
				} else if (ret < 0) {
					ws_server_conn_remove(conn);
					ws_server_log("ERROR: ws_server_sendData send data failed!\n");
					return;
				} else {
					ws_server_log_verbose("ws_server_sendData send %d bytes data to websocket server\n", ret);
					conn->last_data_comm_time = rtos_time_get_current_system_time_ms();
				}
				remain_len -= ret;
			} while (remain_len > 0);
		}
		ws_server_free(ws_tmp_txbuf);
	} else {
		ws_server_free(header);
		ws_server_log("ERROR: ws_server_sendData message not sent\n");
	}
	return;
}

void ws_server_sendPing(ws_conn *conn)
{
	uint8_t ping_Frame[2] = {0x89, 0x00};
	ws_server_write(conn, ping_Frame, 2);
	conn->state = WSS_CONNECTED2;
}

void ws_server_sendBinary(uint8_t *message, size_t message_len, int use_mask, ws_conn *conn)
{
	if (message_len > ws_server_tx_size) {
		ws_server_log("ERROR: The length of data exceeded the tx buf len: %d\n", ws_server_tx_size);
		return;
	}
	ws_server_sendData(BINARY_FRAME, message_len, (uint8_t *)message, use_mask, WS_SERVER_SEND_BY_POLL, 1, conn);
}

void ws_server_sendText(char *message, size_t message_len, int use_mask, ws_conn *conn)
{
	ws_server_log_verbose("Send data: %s\n", message);
	if (message_len > ws_server_tx_size) {
		ws_server_log("ERROR: The length of data exceeded the tx buf len: %d\n", ws_server_tx_size);
		return;
	}
	ws_server_sendData(TEXT_FRAME, message_len, (uint8_t *)message, use_mask, WS_SERVER_SEND_BY_POLL, 1, conn);
}

void ws_server_direct_sendBinary(uint8_t *message, size_t message_len, int use_mask, ws_conn *conn)
{
	if (message_len > ws_server_tx_size) {
		ws_server_log("ERROR: The length of data exceeded the tx buf len: %d\n", ws_server_tx_size);
		return;
	}
	ws_server_sendData(BINARY_FRAME, message_len, (uint8_t *)message, use_mask, WS_SERVER_SEND_DIRECTLY, 1, conn);
}

void ws_server_direct_sendText(char *message, size_t message_len, int use_mask, ws_conn *conn)
{
	ws_server_log_verbose("Send data: %s\n", message);
	if (message_len > ws_server_tx_size) {
		ws_server_log("ERROR: The length of data exceeded the tx buf len: %d\n", ws_server_tx_size);
		return;
	}
	ws_server_sendData(TEXT_FRAME, message_len, (uint8_t *)message, use_mask, WS_SERVER_SEND_DIRECTLY, 1, conn);
}


void ws_server_sendClose(ws_conn *conn)
{
	if (conn->state == WSS_CLOSING || conn->state == WSS_CLOSED) {
		return;
	}
	conn->state = WSS_CLOSING;
	uint8_t close_Frame[2] = {0x88, 0x00};
	ws_server_write(conn, close_Frame, sizeof(close_Frame));
	if (conn->state == WSS_CLOSING) {
		ws_server_set_close_reason(conn, WSS_SERVER_SEND_CLOSE);
		ws_server_conn_remove(conn);
	}
}

int ws_response_write_header_start(ws_conn *conn, char *status, char *content_type, size_t content_len)
{
	char buf[100];
	int ret = 0;
	size_t header_len = 0;

	// remove previous response header
	if (conn->response_header) {
		ws_server_free(conn->response_header);
		conn->response_header = NULL;
	}

	header_len = sprintf(buf, "HTTP/1.1 %s%s", status ? status : "", WS_SERVER_CRLF);

	if (content_type) {
		header_len += sprintf(buf, "Content-Type: %s%s", content_type, WS_SERVER_CRLF);
	}

	if (content_len) {
		header_len += sprintf(buf, "Content-Length: %d%s", content_len, WS_SERVER_CRLF);
	}

	conn->response_header = (char *) ws_server_malloc(header_len + 1);

	if (conn->response_header) {
		memset(conn->response_header, 0, header_len + 1);
		sprintf(conn->response_header, "HTTP/1.1 %s%s", status ? status : "", WS_SERVER_CRLF);

		if (content_type) {
			sprintf(conn->response_header + strlen(conn->response_header), "Content-Type: %s%s", content_type, WS_SERVER_CRLF);
		}

		if (content_len) {
			sprintf(conn->response_header + strlen(conn->response_header), "Content-Length: %d%s", content_len, WS_SERVER_CRLF);
		}
	} else {
		ws_server_log("ERROR: ws_server_malloc");
		ret = -1;
		goto exit;
	}

exit:
	return ret;
}

int ws_response_write_header(ws_conn *conn, char *name, char *value)
{
	char buf[100];
	int ret = 0;
	char *response_header = NULL;
	size_t header_len = 0;

	if (conn->response_header) {
		header_len = sprintf(buf, "%s: %s%s", name ? name : "", value ? value : "", WS_SERVER_CRLF);
		header_len += strlen(conn->response_header);
		response_header = (char *) ws_server_malloc(header_len + 1);

		if (response_header) {
			memset(response_header, 0, header_len + 1);
			sprintf(response_header, "%s", conn->response_header);
			sprintf(response_header + strlen(response_header), "%s: %s%s", name ? name : "", value ? value : "", WS_SERVER_CRLF);
			ws_server_free(conn->response_header);
			conn->response_header = response_header;
		} else {
			ws_server_log("ERROR: ws_server_malloc");
			ret = -1;
			goto exit;
		}
	} else {
		ws_server_log("ERROR: no header start");
		ret = -1;
		goto exit;
	}

exit:
	return ret;
}

int ws_response_write_header_finish(ws_conn *conn)
{
	int ret = 0;
	char *response_header = NULL;
	size_t header_len = 0;

	if (conn->response_header) {
		header_len = strlen(WS_SERVER_CRLF);
		header_len += strlen(conn->response_header);
		response_header = (char *) ws_server_malloc(header_len + 1);

		if (response_header) {
			memset(response_header, 0, header_len + 1);
			sprintf(response_header, "%s", conn->response_header);
			sprintf(response_header + strlen(response_header), "%s", WS_SERVER_CRLF);
			ws_server_log_verbose("handshake response:\r\n%s", response_header);
			ret = ws_server_write(conn, (uint8_t *)response_header, strlen(response_header));
			ws_server_free(response_header);
			ws_server_free(conn->response_header);
			conn->response_header = NULL;
		} else {
			ws_server_log("ERROR: ws_server_malloc");
			ret = -1;
			goto exit;
		}
	} else {
		ws_server_log("ERROR: no header start");
		ret = -1;
		goto exit;
	}

exit:
	return ret;
}

int ws_response_write_data(ws_conn *conn, uint8_t *data, size_t data_len)
{
	return ws_server_write(conn, data, data_len);
}

void ws_server_response_too_many_requests(ws_conn *conn, char *msg)
{
	char msg_buf[50];

	if (msg == NULL) {
		memset(msg_buf, 0, sizeof(msg_buf));
		sprintf(msg_buf, "Too Many Requests\r\nMAX connections: %d", ws_server_max_conn);
		msg = msg_buf;
	}

	ws_response_write_header_start(conn, "429 Too Many Requests", "text/plain", strlen(msg));
	ws_response_write_header_finish(conn);
	ws_response_write_data(conn, (uint8_t *)msg, strlen(msg));
}

void ws_server_response_bad_request(ws_conn *conn, char *msg)
{
	if (msg == NULL) {
		msg = "Bad Request - Invalid Header";
	}

	ws_response_write_header_start(conn, "400 Bad Request", "text/plain", strlen(msg));
	ws_response_write_header_finish(conn);
	ws_response_write_data(conn, (uint8_t *)msg, strlen(msg));
}


/***************WebSocket handshake request*****************
*	GET /chat HTTP/1.1
*	Host: server.example.com(:port)
*	Upgrade: websocket
*	Connection: Upgrade
*	Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==
*	Sec-WebSocket-Protocol: chat, superchat
*	Sec-WebSocket-Version: 13
*	Origin: http://example.com
************************************************************/
int ws_server_handshake_read_header(ws_conn *conn)
{
	int ret = 0;
	uint8_t *tmp_buf = NULL;
	size_t header_len = 0;
	int header_existed = 0;
	int header_end_steps = 0;
	uint8_t data_byte = 0;
	char *tmp_ptr = NULL;
	size_t tmp_len = 0;

	ws_server_log_verbose("%s", __FUNCTION__);

	// remove previous request header for keepalive connection
	if (conn->request.header) {
		ws_server_free(conn->request.header);
		memset(&conn->request, 0, sizeof(struct ws_request));
	}

	tmp_buf = (uint8_t *) ws_server_malloc(ws_server_tmp_buf_size);

	if (tmp_buf == NULL) {
		ws_server_log("ERROR: ws_server_malloc");
		ret = -1;
		goto exit;
	} else {
		memset(tmp_buf, 0, ws_server_tmp_buf_size);
	}

	header_end_steps = 0;

	while ((ws_server_read(conn, &data_byte, 1) == 1) && (header_len < ws_server_tmp_buf_size)) {
		if ((data_byte == '\r') && (header_end_steps == 0)) {
			header_end_steps = 1;
		} else if ((data_byte == '\n') && (header_end_steps == 1)) {
			header_end_steps = 2;
		} else if ((data_byte == '\r') && (header_end_steps == 2)) {
			header_end_steps = 3;
		} else if ((data_byte == '\n') && (header_end_steps == 3)) {
			header_end_steps = 4;
		} else {
			header_end_steps = 0;
		}

		tmp_buf[header_len] = data_byte;
		header_len ++;

		if (header_end_steps == 4) {
			header_existed = 1;
			tmp_buf[header_len] = 0;
			ws_server_log_verbose("header[%d]: %s", header_len, tmp_buf);
			break;
		}
	}

	if (header_existed) {
		conn->request.header = (char *) ws_server_malloc(header_len + 1);
		conn->request.header_len = header_len;

		if (conn->request.header) {
			char *ptr;

			memset(conn->request.header, 0, header_len + 1);
			memcpy(conn->request.header, tmp_buf, header_len);
			ret = 0;

			// get method (method must be "GET")
			ptr = conn->request.header;

			while (ptr < (conn->request.header + conn->request.header_len)) {
				if (*ptr != ' ') {
					tmp_ptr = ptr;
					break;
				}

				ptr ++;
			}

			ptr = tmp_ptr + 1;

			while (ptr < (conn->request.header + conn->request.header_len)) {
				if (*ptr == ' ') {
					tmp_len = ptr - tmp_ptr;
					break;
				}

				ptr ++;
			}

			if ((tmp_ptr == NULL) || (tmp_len == 0) ||
				(memcmp(tmp_ptr, "GET", strlen("GET")) != 0)) {
				ws_server_log_verbose("Method must be GET!");
				ret = -1;
				goto exit;
			}

			// get path
			ptr = tmp_ptr + tmp_len;

			while (ptr < (conn->request.header + conn->request.header_len)) {
				if (*ptr != ' ') {
					conn->request.path = ptr;
					break;
				}

				ptr ++;
			}

			ptr = conn->request.path + 1;

			while (ptr < (conn->request.header + conn->request.header_len)) {
				if ((*ptr == ' ') || (*ptr == '?')) {
					conn->request.path_len = ptr - conn->request.path;
					break;
				}

				ptr ++;
			}

			if ((conn->request.path == NULL) ||
				(conn->request.path && (conn->request.path_len == 0))) {

				ret = -1;
				goto exit;
			}

			// get query string
			ptr = conn->request.path + conn->request.path_len;

			if (*ptr == '?') {
				conn->request.query = ptr;
				ptr = conn->request.query + 1;

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if (*ptr == ' ') {
						conn->request.query_len = ptr - conn->request.query;
						break;
					}

					ptr ++;
				}
			}

			if (conn->request.query && (conn->request.query_len == 0)) {
				ret = -1;
				goto exit;
			}

			// get http version (Now just support http 1.1)
			if (conn->request.query) {
				ptr = conn->request.query + conn->request.query_len;
			} else {
				ptr = conn->request.path + conn->request.path_len;
			}

			conn->request.version = (char *) strstr((char *)ptr, "HTTP/1.1\r\n");

			if (conn->request.version) {
				conn->request.version_len = strlen("HTTP/1.x");
			} else {
				ws_server_log_verbose("Now just support HTTP 1.1");
				ret = -1;
				goto exit;
			}

			// get Upgrade
			ptr = conn->request.version + conn->request.version_len;

			if (strstr(ptr, "Upgrade:")) {
				ptr = (char *) strstr(ptr, "Upgrade:") + strlen("Upgrade:");

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if (*ptr != ' ') {
						tmp_ptr = ptr;
						break;
					}

					ptr ++;
				}

				ptr = tmp_ptr + 1;

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if ((*ptr == '\r') && (*(ptr + 1) == '\n')) {
						tmp_len = ptr - tmp_ptr;
						break;
					}

					ptr ++;
				}

				if ((tmp_ptr == NULL) || (tmp_len == 0) || (
						(memcmp(tmp_ptr, "websocket", strlen("websocket") != 0)) &&
						(memcmp(tmp_ptr, "WebSocket", strlen("websocket") != 0)) &&
						(memcmp(tmp_ptr, "Websocket", strlen("websocket") != 0)))) {
					ret = -1;
					goto exit;
				}
			} else {
				ret = -1;
				goto exit;
			}

			// get Connection
			ptr = conn->request.version + conn->request.version_len;

			if (strstr(ptr, "Connection:")) {
				ptr = (char *) strstr(ptr, "Connection:") + strlen("Connection:");

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if (*ptr != ' ') {
						tmp_ptr = ptr;
						break;
					}

					ptr ++;
				}
				if (tmp_ptr == NULL) {
					ret = -1;
					goto exit;
				}
				ptr = tmp_ptr + 1;

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if ((*ptr == '\r') && (*(ptr + 1) == '\n')) {
						tmp_len = ptr - tmp_ptr;
						break;
					}

					ptr ++;
				}

				//some websocket client includes keep-alive in http request
				//For example, "Connection: keep-alive, Upgrade"
				if ((tmp_len == 0) || (
						(!strstr(tmp_ptr, "upgrade\r\n")) &&
						(!strstr(tmp_ptr, "Upgrade\r\n")) &&
						(!strstr(tmp_ptr, "upgrade,")) &&
						(!strstr(tmp_ptr, "Upgrade,")))) {
					ret = -1;
					goto exit;
				}
			} else {
				ret = -1;
				goto exit;
			}

			// get websocekt key
			ptr = conn->request.version + conn->request.version_len;

			if (strstr(ptr, "Sec-WebSocket-Key:")) {
				ptr = (char *) strstr(ptr, "Sec-WebSocket-Key:") + strlen("Sec-WebSocket-Key:");

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if (*ptr != ' ') {
						conn->request.ws_key = ptr;
						break;
					}

					ptr ++;
				}

				ptr = conn->request.ws_key + 1;

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if ((*ptr == '\r') && (*(ptr + 1) == '\n')) {
						conn->request.ws_key_len = ptr - conn->request.ws_key;
						break;
					}

					ptr ++;
				}

				if ((conn->request.ws_key == NULL) || (conn->request.ws_key_len == 0)) {
					ret = -1;
					goto exit;
				}
			} else {
				ret = -1;
				goto exit;
			}

			// get websocekt version (13)
			ptr = conn->request.version + conn->request.version_len;

			if (strstr(ptr, "Sec-WebSocket-Version:")) {
				ptr = (char *) strstr(ptr, "Sec-WebSocket-Version:") + strlen("Sec-WebSocket-Version:");

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if (*ptr != ' ') {
						conn->request.ws_version = ptr;
						break;
					}

					ptr ++;
				}

				ptr = conn->request.ws_version + 1;

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if ((*ptr == '\r') && (*(ptr + 1) == '\n')) {
						conn->request.ws_version_len = ptr - conn->request.ws_version;
						break;
					}

					ptr ++;
				}

				if ((conn->request.ws_version == NULL) || (conn->request.ws_version_len == 0) ||
					(memcmp(conn->request.ws_version, "13", strlen("13") != 0))) {
					ret = -1;
					goto exit;
				}
			} else {
				ret = -1;
				goto exit;
			}
		} else {
			ws_server_log("ERROR: ws_server_malloc");
			ret = -1;
			goto exit;
		}
	} else {
		ws_server_log("ERROR: parse header[%d]: %s", header_len, tmp_buf);
		tmp_buf[header_len] = 0;
		ret = -1;
		goto exit;
	}

exit:
	if (ret && conn->request.header) {
		ws_server_log("ERROR: read handshake header fail");
		ws_server_free(conn->request.header);
		memset(&conn->request, 0, sizeof(struct ws_request));
	}

	if (tmp_buf) {
		ws_server_free(tmp_buf);
	}

	return ret;
}



/***************WebSocket handshake response*****************
*	HTTP/1.1 101 Switching Protocols
*	Upgrade: websocket
*	Connection: Upgrade
*	Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
************************************************************/

int ws_server_handshake_response(ws_conn *conn)
{
	unsigned char *ws_key_magic;
	unsigned char ws_accept_sha1[20];
	char ws_accept[29];
	size_t ws_accept_len = sizeof(ws_accept);

	ws_key_magic = ws_server_malloc(conn->request.ws_key_len + strlen(WS_SERVER_MAGIC_STRING) + 1);
	if (ws_key_magic == NULL) {
		ws_server_log("ERROR: ws_server_malloc fail");
		return -1;
	}
	memset(ws_key_magic, 0, (conn->request.ws_key_len + strlen(WS_SERVER_MAGIC_STRING) + 1));
	//sprintf(ws_key_magic, "%s%s", conn->request.ws_key, WS_SERVER_MAGIC_STRING);
	memcpy(ws_key_magic, conn->request.ws_key, conn->request.ws_key_len);
	memcpy(ws_key_magic + conn->request.ws_key_len, WS_SERVER_MAGIC_STRING, strlen(WS_SERVER_MAGIC_STRING));
	ws_server_log_verbose("sec_key with magic string: %s", ws_key_magic);
	memset(ws_accept_sha1, 0, 20);
	ws_server_sha1(ws_key_magic, strlen((char *)ws_key_magic), ws_accept_sha1);
	ws_server_free(ws_key_magic);
	memset(ws_accept, 0, 29);
	ws_server_base64_encode(ws_accept_sha1, 20, ws_accept, ws_accept_len);
	ws_server_log_verbose("Sec-WebSocket-Accept: %s", ws_accept);

	ws_response_write_header_start(conn, "101 Switching Protocols", NULL, 0);
	ws_response_write_header(conn, "Upgrade", "websocket");
	ws_response_write_header(conn, "Connection", "Upgrade");
	ws_response_write_header(conn, "Sec-WebSocket-Accept", ws_accept);
	return ws_response_write_header_finish(conn);

}

