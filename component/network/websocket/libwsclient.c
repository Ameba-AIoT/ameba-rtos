#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <websocket/libwsclient.h>
#include <websocket/wsclient_api.h>

#define struct_entry(ptr, type, member) \
    ((type *)( (char *)(ptr) - (unsigned long)(&((type*)0)->member)))

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR   (-1)
static const unsigned char encode[64] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
	'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '+', '/'
};


extern int max_data_len;
extern uint32_t wsclient_keepalive_idle;
extern uint32_t wsclient_keepalive_interval;
extern uint32_t wsclient_keepalive_count;
extern uint32_t wsclient_recvtimeout;
extern uint32_t wsclient_sendtimeout;
extern uint32_t wsclient_sendblocktime;


/***************Base Framing Protocol for reference*****************/
//
//	0					1					2					3
//	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-------+-+-------------+-------------------------------+
// |F|R|R|R| opcode|M| Payload len |	Extended payload length    |
// |I|S|S|S|  (4)  |A|	   (7)	   |			 (16/64)		   |
// |N|V|V|V|	   |S|			   |   (if payload len==126/127)   |
// | |1|2|3|	   |K|			   |							   |
// +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
// |	 Extended payload length continued, if payload len == 127  |
// + - - - - - - - - - - - - - - - +-------------------------------+
// |							   |Masking-key, if MASK set to 1  |
// +-------------------------------+-------------------------------+
// | Masking-key (continued)	   |		  Payload Data		   |
// +-------------------------------- - - - - - - - - - - - - - - - +
// :					 Payload Data continued ... 			   :
// + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
// |					 Payload Data continued ... 			   |
// +---------------------------------------------------------------+
/*******************************************************************/
static unsigned int ws_arc4random(void)
{
	unsigned int res = rtos_time_get_current_system_time_ms();
	static unsigned int seed = 0xDEADB00B;

	seed = ((seed & 0x007F00FF) << 7) ^
		   ((seed & 0x0F80FF00) >> 8) ^ // be sure to stir those low bits
		   (res << 13) ^ (res >> 9);    // using the clock too!

	return seed;
}

static int ws_b64_encode_string(const unsigned char *in, int in_len, unsigned char *out, int out_size)
{
	/* To avoid gcc warnings */
	(void) out_size;

	int i, n;
	int C1, C2, C3;

	if (in_len == 0) {
		return -1;
	}

	n = (in_len / 3) * 3;

	for (i = 0; i < n; i += 3) {
		C1 = *in++;
		C2 = *in++;
		C3 = *in++;

		*out++ = encode[(C1 >> 2) & 0x3F];
		*out++ = encode[(((C1 &  3) << 4) + (C2 >> 4)) & 0x3F];
		*out++ = encode[(((C2 & 15) << 2) + (C3 >> 6)) & 0x3F];
		*out++ = encode[C3 & 0x3F];
	}

	if (i < in_len) {
		C1 = *in++;
		C2 = ((i + 1) < in_len) ? *in++ : 0;

		*out++ = encode[(C1 >> 2) & 0x3F];
		*out++ = encode[(((C1 & 3) << 4) + (C2 >> 4)) & 0x3F];

		if ((i + 1) < in_len) {
			*out++ = encode[((C2 & 15) << 2) & 0x3F];
		} else {
			*out++ = '=';
		}

		*out++ = '=';
	}

	*out = 0;

	return 0;

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
static char *ws_handshake_header(char *host, char *path, char *origin, int port, int DefaultPort)
{
	unsigned char key_b64[25], hash[16];
	char *header = NULL;
	char *header_port = NULL, *header_origin = NULL, *header_pro = NULL, *header_ver = NULL, *urlname = NULL;
	int header_len = 0;
	int port_str_len = 0;
	int temp = 0;
	int origin_len = 0, path_len = 0;

	wsclient_context *wsclient = struct_entry(host, wsclient_context, host);

	memset(key_b64, 0, 25);
	memset(hash, 0, 16);
	ws_get_random_bytes(hash, 16);
	ws_b64_encode_string(hash, 16, key_b64, 24);//Content of Sec-WebSocket-Key

	if (path != NULL) {
		path_len = strlen(path);
	}

	urlname = wsclient->host;

	header_len = strlen("GET /") + path_len + strlen(" HTTP/1.1\r\nHost: ") + strlen(urlname)
				 + strlen("\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: ") + sizeof(key_b64)
				 + strlen("\r\nSec-WebSocket-Protocol: \r\nSec-WebSocket-Version: \r\n\r\n");

	if (DefaultPort != 1) {
		temp = port;
		while ((temp /= 10) > 0) {
			port_str_len ++;
		}
		port_str_len = port_str_len + 1; //get the str_len of port

		header_len += strlen(":") + port_str_len;
		header_port = (char *) ws_malloc(strlen(":") + port_str_len + 1);
		if (header_port == NULL) {
			WSCLIENT_ERROR("ERROR: Malloc(%d bytes) for port failed\n", strlen(":") + port_str_len + 1);
			return NULL;
		}

		sprintf(header_port, ":%d", port);
	} else {
		header_port = "";
	}

	if (origin != NULL) {
		origin_len = strlen(origin);
	}

	if (origin_len != 0) {
		header_len += strlen("Origin: \r\n") + origin_len;
		header_origin = (char *) ws_malloc(strlen("Origin: \r\n") + origin_len + 1);
		if (header_origin == NULL) {
			WSCLIENT_ERROR("ERROR: Malloc(%d bytes) for origin failed\n", strlen("Origin: \r\n") + origin_len + 1);
			if (strlen(header_port) > 0) {
				ws_free(header_port);
			}
			return NULL;
		}

		sprintf(header_origin, "Origin: %s\r\n", origin);
	} else {
		header_origin = "";
	}

	if (wsclient->protocol && wsclient->protocol_len > 0) {
		header_pro = wsclient->protocol;
		header_len += wsclient->protocol_len;
	} else {
		header_pro = "chat, superchat";
		header_len += strlen("chat, superchat");
	}

	if (wsclient->version && wsclient->version_len > 0) {
		header_ver = wsclient->version;
		header_len += wsclient->version_len;
	} else {
		header_ver = "13";
		header_len += strlen("13");
	}

	if (wsclient->custom_token && wsclient->custom_token_len > 0) {
		header_len += wsclient->custom_token_len;
	} else {
		wsclient->custom_token = "";
	}

	header = (char *) ws_malloc(header_len + 1);

	if (header == NULL) {
		WSCLIENT_ERROR("ERROR: Malloc(%d bytes) failed\n", header_len + 1);
	} else {
		sprintf(header,
				"GET /%s HTTP/1.1\r\nHost: %s%s\r\nUpgrade: websocket\r\nConnection: Upgrade\r\n%sSec-WebSocket-Key: %s\r\nSec-WebSocket-Protocol: %s\r\nSec-WebSocket-Version: %s\r\n%s\r\n",
				path ? path : "", urlname, header_port, header_origin, key_b64, header_pro, header_ver, wsclient->custom_token);
	}

	if (strlen(header_port) > 0) {
		ws_free(header_port);
	}

	if (strlen(header_origin) > 0) {
		ws_free(header_origin);
	}

	return header;
}

void ws_get_random_bytes(void *buf, size_t len)
{
	unsigned int ranbuf;
	unsigned int *lp;
	int i, count;
	count = len / sizeof(unsigned int);
	lp = (unsigned int *) buf;

	for (i = 0; i < count; i ++) {
		lp[i] = ws_arc4random();
		len -= sizeof(unsigned int);
	}

	if (len > 0) {
		ranbuf = ws_arc4random();
		memcpy(&lp[i], &ranbuf, len);
	}
}

_WEAK void *ws_malloc(size_t size)
{
	return (void *)rtos_mem_zmalloc(size);
}

_WEAK void ws_free(void *buf)
{
	rtos_mem_free((u8 *)buf);
}

int ws_random(void *p_rng, unsigned char *output, size_t output_len)
{
	/* To avoid gcc warnings */
	(void) p_rng;

	TRNG_get_random_bytes(output, output_len);
	return 0;
}

void ws_client_close(wsclient_context *wsclient)
{
	if (wsclient->sockfd != -1) {
		closesocket(wsclient->sockfd);
		wsclient->sockfd = -1;
	}

	wsclient->readyState = WSC_CLOSED;
	wsclient->use_ssl = 0;
	wsclient->port = 0;
	wsclient->tx_len = 0;
	wsclient->rx_len = 0;

	send_buf *tmp_buf = NULL;
	if (wsclient->ready_send_buf && wsclient->recycle_send_buf) {
		while (rtos_queue_receive(wsclient->ready_send_buf, (void *)&tmp_buf, 0) == SUCCESS) {
			rtos_queue_send(wsclient->recycle_send_buf, (void *)&tmp_buf, 0);
		}
		WSCLIENT_DEBUG("module close - move item to recycle\n\r");
		while (rtos_queue_receive(wsclient->recycle_send_buf, (void *)&tmp_buf, 0) == SUCCESS) {
			WSCLIENT_DEBUG("module close - tmp_item %x\n\r", tmp_buf);
			if (tmp_buf) {
				if (tmp_buf->txbuf) {
					ws_free(tmp_buf->txbuf);
				}
				tmp_buf->tx_len = 0;
				ws_free(tmp_buf);
				WSCLIENT_DEBUG("module close - data_addr %x\n\r", tmp_buf->txbuf);
			}
		}
		WSCLIENT_DEBUG("module close - clean resource in recycle\n\r");
		rtos_queue_delete(wsclient->recycle_send_buf);
		rtos_queue_delete(wsclient->ready_send_buf);

		WSCLIENT_DEBUG("module close - free queue\n\r");
	} else {
		if (wsclient->ready_send_buf) {
			rtos_queue_delete(wsclient->ready_send_buf);
		}
		if (wsclient->recycle_send_buf) {
			rtos_queue_delete(wsclient->recycle_send_buf);
		}
	}

	if (wsclient->queue_mutex) {
		rtos_mutex_delete(wsclient->queue_mutex);
	}

	if (wsclient->txbuf) {
		ws_free(wsclient->txbuf);
		wsclient->txbuf = NULL;
	}
	if (wsclient->rxbuf) {
		ws_free(wsclient->rxbuf);
		wsclient->rxbuf = NULL;
	}
	if (wsclient->receivedData) {
		ws_free(wsclient->receivedData);
		wsclient->receivedData = NULL;
	}
	if (wsclient->tls) {
		ws_free(wsclient->tls);
		wsclient->tls = NULL;
	}
	if (wsclient->protocol && wsclient->protocol_len > 0) {
		ws_free(wsclient->protocol);
		wsclient->protocol = NULL;
		wsclient->protocol_len = 0;
	}

	if (wsclient->version && wsclient->version_len > 0) {
		ws_free(wsclient->version);
		wsclient->version = NULL;
		wsclient->version_len = 0;
	}

	if (wsclient->custom_token && wsclient->custom_token_len > 0) {
		ws_free(wsclient->custom_token);
		wsclient->custom_token = NULL;
		wsclient->custom_token_len = 0;
	}

	if (wsclient->host) {
		ws_free(wsclient->host);
		wsclient->host = NULL;
	}

	if (wsclient->path) {
		ws_free(wsclient->path);
		wsclient->path = NULL;
	}

	if (wsclient->origin) {
		ws_free(wsclient->origin);
		wsclient->origin = NULL;
	}

//To fix the ws_getReadyState() after ws_close() faied issue, then free the context in the example
	/*	if(wsclient){
			ws_free(wsclient);
			wsclient = NULL;
		}*/
}

int ws_client_read(wsclient_context *wsclient, char *data, size_t data_len)
{
	int ret = 0;
	ret = recv(wsclient->sockfd, data, data_len, 0);

	if (ret < 0 && (errno == EAGAIN || errno == ENOMEM)) {
		return 0;
	} else if (ret < 0) {
		return -1;
	} else {
		return ret;
	}
}

int ws_client_send(wsclient_context *wsclient, char *data, size_t data_len)
{
	int ret = 0;
	ret = send(wsclient->sockfd, data, data_len, 0);

	if (ret < 0 && (errno == EAGAIN || errno == ENOMEM)) {
		return 0;
	} else if (ret < 0) {
		return -1;
	} else {
		return ret;
	}
}

int ws_sendData(uint8_t type, size_t message_size, uint8_t *message, int useMask, wsclient_context *wsclient)
{

	if (wsclient->readyState == WSC_CLOSING || wsclient->readyState == WSC_CLOSED) {
		return -1;
	}

	uint8_t masking_key[4];
	uint8_t *header;
	size_t header_len;

	if (useMask) {
		ws_get_random_bytes(masking_key, 4);
		//WSCLIENT_DEBUG("The mask key is %2x, %2x, %2x, %2x\n",masking_key[0], masking_key[1], masking_key[2], masking_key[3]);
	} else {
		WSCLIENT_ERROR("WARNING: ALL PACKETS FROM CLIENT SHOULD BE MASKED!");
	}

	header_len = (2 + (message_size >= 126 ? 2 : 0) + (message_size >= 65536 ? 6 : 0) + (useMask ? 4 : 0));
	header = ws_malloc((u32)header_len);
	if (header == NULL) {
		WSCLIENT_ERROR("Malloc header failed\n");
		return -1;
	}

	header[0] = 0x80 | (wsclient->txRsvBits.RSV1 << 6 | wsclient->txRsvBits.RSV2 << 5 | wsclient->txRsvBits.RSV3 << 4) | type;
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
		header[2] = 0;//(message_size >> 56) & 0xff;exceed message_size limit, should be 0
		header[3] = 0;//(message_size >> 48) & 0xff;exceed message_size limit, should be 0
		header[4] = 0;//(message_size >> 40) & 0xff;exceed message_size limit, should be 0
		header[5] = 0;//(message_size >> 32) & 0xff;exceed message_size limit, should be 0
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
	if (type == PONG) { //Reply PONG as soon as possible
		char *Pong_Frame = ws_malloc((u32)(header_len + message_size + 1));
		if (Pong_Frame == NULL) {
			WSCLIENT_ERROR("Malloc Pong Frame failed\n");
			ws_free(header);
			return -1;
		}

		memcpy(Pong_Frame, header, header_len);
		memcpy(Pong_Frame + header_len, message, message_size);
		if (useMask) {
			for (size_t i = 0; i != message_size; i++) {
				Pong_Frame[header_len + i] ^= masking_key[i & 0x3];
			}
		}
		WSCLIENT_DEBUG("Reply Pong frame\r\n");

		int ret = wsclient->fun_ops.client_send(wsclient, Pong_Frame, (header_len + message_size));
		ws_free(Pong_Frame);
		if (ret < 0) {
			WSCLIENT_ERROR("ERROR: Replay PING failed\n");
			ws_free(header);
			wsclient->fun_ops.client_close(wsclient);
			return -1;
		}
		ws_free(header);
		return 0;
	}

//receive buffer item from recycle queue then send message to ready queue
	send_buf *tmp_buf = NULL;

	rtos_mutex_take(wsclient->queue_mutex, MUTEX_WAIT_TIMEOUT);
	while (rtos_queue_receive(wsclient->recycle_send_buf, (void *)&tmp_buf, 0) != SUCCESS) {
		if ((wsclient->ready_send_buf_num + wsclient->recycle_send_buf_num) < wsclient->max_queue_size) {
			tmp_buf = ws_malloc(sizeof(send_buf));
			if (!tmp_buf) {
				rtos_mutex_give(wsclient->queue_mutex);
				WSCLIENT_ERROR("ERROR: Send_buf malloc fail\n\r");
				ws_free(header);
				wsclient->fun_ops.client_close(wsclient);
				return -1;
			}

			WSCLIENT_DEBUG("Create queue item: %x\r\n", tmp_buf);

			tmp_buf->txbuf = (uint8_t *)ws_malloc(max_data_len + 16);
			if (!tmp_buf->txbuf) {
				rtos_mutex_give(wsclient->queue_mutex);
				WSCLIENT_ERROR("ERROR: Send_buf txbuf malloc fail\n\r");
				ws_free(header);
				ws_free(tmp_buf);
				wsclient->fun_ops.client_close(wsclient);
				return -1;
			}

			WSCLIENT_DEBUG("Create buffer of queue item: %x\r\n", tmp_buf->txbuf);

			if (rtos_queue_send(wsclient->recycle_send_buf, (void *)&tmp_buf, RTOS_MAX_TIMEOUT) != SUCCESS) {
				rtos_mutex_give(wsclient->queue_mutex);
				WSCLIENT_ERROR("ERROR: Send_buf queue send fail\n\r");
				ws_free(header);
				ws_free(tmp_buf->txbuf);
				ws_free(tmp_buf);
				wsclient->fun_ops.client_close(wsclient);
				return -1;
			}
			wsclient->recycle_send_buf_num++;
		} else {
			rtos_mutex_give(wsclient->queue_mutex);
			if (wsclient_sendblocktime != 0) {
				if (rtos_queue_receive(wsclient->recycle_send_buf, (void *)&tmp_buf, wsclient_sendblocktime) == SUCCESS) {
					rtos_mutex_take(wsclient->queue_mutex, MUTEX_WAIT_TIMEOUT);
					break;
				}
			}
			WSCLIENT_ERROR("ERROR: Not get usable buffer, Please enlarge max_queue_size!\r\n");
			ws_free(header);
			//wsclient->fun_ops.client_close(wsclient);
			return -1;
		}
	}

	wsclient->recycle_send_buf_num--;
	WSCLIENT_DEBUG("Receiving empty buffer from recycle queue\r\n");

	memset(tmp_buf->txbuf, 0, max_data_len + 16);
	memcpy(tmp_buf->txbuf, header, header_len);
	if (message_size > 0 && message != NULL) {
		memcpy(tmp_buf->txbuf + header_len, message, message_size);
	}
	tmp_buf->tx_len = header_len + message_size;
	tmp_buf->send_offset = 0;
	ws_free(header);

	if (useMask) {
		for (size_t i = 0; i != message_size; i++) {
			tmp_buf->txbuf[header_len + i] ^= masking_key[i & 0x3];
		}
	}
	if (rtos_queue_send(wsclient->ready_send_buf, (void *)&tmp_buf, RTOS_MAX_TIMEOUT) != SUCCESS) {
		rtos_mutex_give(wsclient->queue_mutex);
		WSCLIENT_ERROR("ERROR: Send_buf queue send fail\n\r");
		wsclient->fun_ops.client_close(wsclient);
		return -1;
	}
	wsclient->ready_send_buf_num++;
	rtos_mutex_give(wsclient->queue_mutex);
	WSCLIENT_DEBUG("Send message: %s to ready queue\r\n", message);

	WSCLIENT_DEBUG("ready_send_buf num: %d, recycle_send_buf num: %d\r\n", wsclient->ready_send_buf_num, wsclient->recycle_send_buf_num);

	return 0;
}

int ws_hostname_connect(wsclient_context *wsclient)
{
	struct addrinfo hints;
	struct addrinfo *result;
	struct addrinfo *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	char sport[5];
	static int opt = 1, option = 1;
	memset(sport, 0, 5);
	sprintf(sport, "%d", wsclient->port);
	if ((wsclient->sockfd = getaddrinfo(wsclient->host, sport, &hints, &result)) != 0) {
		WSCLIENT_ERROR("ERROR: Getaddrinfo failed: %s(%d)\n", wsclient->host, wsclient->sockfd);
		return -1;
	}
	for (p = result; p != NULL; p = p->ai_next) {
		wsclient->sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (wsclient->sockfd == INVALID_SOCKET) {
			continue;
		}
		if ((setsockopt(wsclient->sockfd, SOL_SOCKET, SO_KEEPALIVE, (const char *)&opt, sizeof(opt))) < 0) {
			WSCLIENT_ERROR("ERROR: Setting socket option keepalive failed!\n");
			return -1;
		}
		if ((setsockopt(wsclient->sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&option, sizeof(option))) < 0) {
			WSCLIENT_ERROR("ERROR: Setting socket option SO_REUSEADDR failed!\n");
			return -1;
		}

		if (wsclient_keepalive_idle != 0)
			if (setsockopt(wsclient->sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &wsclient_keepalive_idle, sizeof(wsclient_keepalive_idle)) != 0) {
				WSCLIENT_ERROR("ERROR: set TCP_KEEPIDLE fail\n");
				return -1;
			}
		if (wsclient_keepalive_interval != 0)
			if (setsockopt(wsclient->sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &wsclient_keepalive_interval, sizeof(wsclient_keepalive_interval)) != 0) {
				WSCLIENT_ERROR("ERROR: set TCP_KEEPINTVL fail\n");
				return -1;
			}
		if (wsclient_keepalive_count != 0)
			if (setsockopt(wsclient->sockfd, IPPROTO_TCP, TCP_KEEPCNT, &wsclient_keepalive_count, sizeof(wsclient_keepalive_count)) != 0) {
				WSCLIENT_ERROR("ERROR: set TCP_KEEPCNT fail\n");
				return -1;
			}

		if (wsclient_recvtimeout != 0)
			if (setsockopt(wsclient->sockfd, SOL_SOCKET, SO_RCVTIMEO, &wsclient_recvtimeout, sizeof(wsclient_recvtimeout)) != 0) {
				WSCLIENT_ERROR("ERROR: set SO_RCVTIMEO fail\n");
				return -1;
			}

		if (wsclient_sendtimeout != 0)
			if (setsockopt(wsclient->sockfd, SOL_SOCKET, SO_SNDTIMEO, &wsclient_sendtimeout, sizeof(wsclient_sendtimeout)) != 0) {
				WSCLIENT_ERROR("ERROR: set SO_SNDTIMEO fail\n");
				return -1;
			}

		if (connect(wsclient->sockfd, p->ai_addr, p->ai_addrlen) != SOCKET_ERROR) {
			break;
		}
		closesocket(wsclient->sockfd);
		wsclient->sockfd = INVALID_SOCKET;
	}
	freeaddrinfo(result);
	return wsclient->sockfd;
}

int ws_client_handshake(wsclient_context *wsclient)
{
	int ret = 0;
	int DefaultPort = 0;

	if (((wsclient->use_ssl == 1) && (wsclient->port == 443))
		|| ((wsclient->use_ssl == 0) && (wsclient->port == 80))) {
		DefaultPort = 1;
	}

	char *hostadd = (char *)&wsclient->host;
	char *header = ws_handshake_header(hostadd, wsclient->path, wsclient->origin, wsclient->port, DefaultPort);
	WSCLIENT_DEBUG("The header is:\r\n%s\r\nwhich size is %d\r\n", header, strlen(header));

	ret = wsclient->fun_ops.client_send(wsclient, header, strlen(header));
	if ((ret > 0) && (ret < (int)strlen(header))) {
		//this means header send not complete, ret 0 indicate handshake fail
		ret = 0;
	}
	ws_free(header);
	return ret;
}

int ws_check_handshake(wsclient_context *wsclient)
{
	int i, status, ret;
	char line[256];
	for (i = 0; i < 2 || (i < 255 && line[i - 2] != '\r' && line[i - 1] != '\n'); ++i) {
		ret = wsclient->fun_ops.client_read(wsclient, line + i, 1);
		if (ret == 0) {
			return -1;
		}
	}
	line[i] = 0;
	if (i == 255) {
		return -1;
	}
	WSCLIENT_DEBUG("The response's first line is : %s\n", line);
	if (sscanf(line, "HTTP/1.1 %d", &status) != 1 || status != 101) {
		WSCLIENT_ERROR("ERROR: Got bad status connecting to %s\n", line);
		return -1;
	}

	while (1) {
		memset(line, 0, 256);
		for (i = 0; i < 2 || (i < 255 && line[i - 2] != '\r' && line[i - 1] != '\n'); ++i) {
			ret = wsclient->fun_ops.client_read(wsclient, line + i, 1);
			if (ret == 0) {
				return -1;
			}
		}
		if (line[0] == '\r' && line[1] == '\n') {
			break;
		}
	}
	return 0;
}

#ifdef USING_SSL
int wss_hostname_connect(wsclient_context *wsclient)
{
	int ret = -1;
	static int opt = 1, option = 1;
//ssl connect
	wsclient->sockfd = -1;
	wsclient->tls = (void *)wss_tls_connect(&wsclient->sockfd, wsclient->host, wsclient->port);

	if (wsclient->tls == NULL) {
		WSCLIENT_ERROR("ERROR: ssl connect failed ret(%d)\n", ret);
		goto exit;
	}
//set sock
	if ((ret = setsockopt(wsclient->sockfd, SOL_SOCKET, SO_KEEPALIVE, (const char *)&opt, sizeof(opt))) < 0) {
		WSCLIENT_ERROR("ERROR: setsockopt SO_KEEPALIVE failed ret(%d)\n", ret);
		goto exit;
	}
	if ((ret = setsockopt(wsclient->sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&option, sizeof(option))) < 0) {
		WSCLIENT_ERROR("ERROR: setsockopt SO_REUSEADDR failed ret(%d)\n", ret);
		goto exit;
	}

	if (wsclient_keepalive_idle != 0)
		if (setsockopt(wsclient->sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &wsclient_keepalive_idle, sizeof(wsclient_keepalive_idle)) != 0) {
			WSCLIENT_ERROR("ERROR: set TCP_KEEPIDLE fail\n");
			goto exit;
		}
	if (wsclient_keepalive_interval != 0)
		if (setsockopt(wsclient->sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &wsclient_keepalive_interval, sizeof(wsclient_keepalive_interval)) != 0) {
			WSCLIENT_ERROR("ERROR: set TCP_KEEPINTVL fail\n");
			goto exit;
		}
	if (wsclient_keepalive_count != 0)
		if (setsockopt(wsclient->sockfd, IPPROTO_TCP, TCP_KEEPCNT, &wsclient_keepalive_count, sizeof(wsclient_keepalive_count)) != 0) {
			WSCLIENT_ERROR("ERROR: set TCP_KEEPCNT fail\n");
			goto exit;
		}

//ssl handshake
	if ((ret = wss_tls_handshake(wsclient->tls)) != 0) {
		WSCLIENT_ERROR("ERROR: ssl handshake failed ret(%d)\n", ret);
		goto exit;
	}
	return 0;
exit:
	return -1;
}

void wss_client_close(wsclient_context *wsclient)
{
	wss_tls_close(wsclient->tls, &wsclient->sockfd);

	wsclient->readyState = WSC_CLOSED;
	wsclient->use_ssl = 0;
	wsclient->port = 0;
	wsclient->tx_len = 0;

	send_buf *tmp_buf = NULL;
	if (wsclient->ready_send_buf && wsclient->recycle_send_buf) {
		while (rtos_queue_receive(wsclient->ready_send_buf, (void *)&tmp_buf, 0) == SUCCESS) {
			rtos_queue_send(wsclient->recycle_send_buf, (void *)&tmp_buf, 0);
		}
		WSCLIENT_DEBUG("module close - move item to recycle\n\r");
		while (rtos_queue_receive(wsclient->recycle_send_buf, (void *)&tmp_buf, 0) == SUCCESS) {
			WSCLIENT_DEBUG("module close - tmp_item %x\n\r", tmp_buf);
			if (tmp_buf) {
				if (tmp_buf->txbuf) {
					ws_free(tmp_buf->txbuf);
				}
				tmp_buf->tx_len = 0;
				ws_free(tmp_buf);
				WSCLIENT_DEBUG("module close - data_addr %x\n\r", tmp_buf->txbuf);
			}
		}
		WSCLIENT_DEBUG("module close - clean resource in recycle\n\r");
		rtos_queue_delete(wsclient->recycle_send_buf);
		rtos_queue_delete(wsclient->ready_send_buf);

		WSCLIENT_DEBUG("module close - free queue\n\r");
	} else {
		if (wsclient->ready_send_buf) {
			rtos_queue_delete(wsclient->ready_send_buf);
		}
		if (wsclient->recycle_send_buf) {
			rtos_queue_delete(wsclient->recycle_send_buf);
		}
	}

	if (wsclient->queue_mutex) {
		rtos_mutex_delete(wsclient->queue_mutex);
	}

	if (wsclient->txbuf) {
		ws_free(wsclient->txbuf);
		wsclient->txbuf = NULL;
	}

	if (wsclient->rxbuf) {
		ws_free(wsclient->rxbuf);
		wsclient->rxbuf = NULL;
	}
	if (wsclient->receivedData) {
		ws_free(wsclient->receivedData);
		wsclient->receivedData = NULL;
	}

	if (wsclient->protocol && wsclient->protocol_len > 0) {
		ws_free(wsclient->protocol);
		wsclient->protocol = NULL;
		wsclient->protocol_len = 0;
	}

	if (wsclient->version && wsclient->version_len > 0) {
		ws_free(wsclient->version);
		wsclient->version = NULL;
		wsclient->version_len = 0;
	}

	if (wsclient->custom_token && wsclient->custom_token_len > 0) {
		ws_free(wsclient->custom_token);
		wsclient->custom_token = NULL;
		wsclient->custom_token_len = 0;
	}

	if (wsclient->host) {
		ws_free(wsclient->host);
		wsclient->host = NULL;
	}

	if (wsclient->path) {
		ws_free(wsclient->path);
		wsclient->path = NULL;
	}

	if (wsclient->origin) {
		ws_free(wsclient->origin);
		wsclient->origin = NULL;
	}

//To fix the ws_getReadyState() after ws_close() faied issue, then free the context in the example
	/*
		if(wsclient){
			ws_free(wsclient);
			wsclient = NULL;
		}*/
}

int wss_client_read(wsclient_context *wsclient, char *data, size_t data_len)
{
	int ret = 0;

	ret = wss_tls_read(wsclient->tls, data, data_len);

	if (ret < 0 && (errno == EAGAIN || errno == ENOMEM)) {
		return 0;
	} else if (ret < 0) {
		WSCLIENT_DEBUG("ssl_read failed, return: %d", ret);
		return -1;
	} else {
		return ret;
	}
}

int wss_client_send(wsclient_context *wsclient, char *data, size_t data_len)
{
	int ret = 0;

	ret = wss_tls_write(wsclient->tls, data, data_len);

	if (ret < 0 && (errno == EAGAIN || errno == ENOMEM)) {
		return 0;
	} else if (ret < 0) {
		WSCLIENT_DEBUG("ssl_write failed, return: %d", ret);
		return -1;
	} else {
		return ret;
	}
}
#endif
