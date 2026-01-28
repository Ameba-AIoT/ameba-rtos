#include "lwip_netconf.h"
#include "wsclient_api.h"


void (*ws_receive_cb)(wsclient_context **, int) = NULL;
void (*ws_receive_pong_cb)(wsclient_context **) = NULL;
void (*ws_close_cb)(wsclient_context *) = NULL;

uint32_t wsclient_keepalive_idle = 0;
uint32_t wsclient_keepalive_interval = 0;
uint32_t wsclient_keepalive_count = 0;
uint32_t wsclient_recvtimeout = 0;
uint32_t wsclient_sendtimeout = 0;
uint32_t wsclient_sendblocktime = 30000;
uint32_t wsclient_connecttimeout = 0;


static void ws_dispatchBinary(wsclient_context **wsclient)
{
	wsclient_context *wsc = *wsclient;
	while (1) {
		struct wsheader_type ws;
		if (wsc->rx_len < 2) {
			return;
		}
		const uint8_t *data = (uint8_t *) &wsc->rxbuf[0];  // peek, but don't consume
		ws.fin = (data[0] & 0x80) == 0x80;
		wsc->rxRsvBits.RSV1 = (data[0] & 0x40) >> 6;
		wsc->rxRsvBits.RSV2 = (data[0] & 0x20) >> 5;
		wsc->rxRsvBits.RSV3 = (data[0] & 0x10) >> 4;
		ws.opcode = (enum opcode_type)(data[0] & 0x0f);
		ws.mask = (data[1] & 0x80) == 0x80;
		ws.N0 = (data[1] & 0x7f);
		ws.header_size = 2 + (ws.N0 == 126 ? 2 : 0) + (ws.N0 == 127 ? 8 : 0) + (ws.mask ? 4 : 0);
		if (wsc->rx_len < ws.header_size) {
			WSCLIENT_ERROR("ERROR: read rx_len < actual header_size\n");
			return;
		}
		int i = 0;
		if (ws.N0 < 126) {
			ws.N = ws.N0;
			i = 2;
		} else if (ws.N0 == 126) {
			ws.N = 0;
			ws.N |= ((uint64_t) data[2]) << 8;
			ws.N |= ((uint64_t) data[3]) << 0;
			i = 4;
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
			i = 10;
		}
		if (ws.mask) {
			ws.masking_key[0] = ((uint8_t) data[i + 0]) << 0;
			ws.masking_key[1] = ((uint8_t) data[i + 1]) << 0;
			ws.masking_key[2] = ((uint8_t) data[i + 2]) << 0;
			ws.masking_key[3] = ((uint8_t) data[i + 3]) << 0;
		} else {
			ws.masking_key[0] = 0;
			ws.masking_key[1] = 0;
			ws.masking_key[2] = 0;
			ws.masking_key[3] = 0;
		}

		WSCLIENT_DEBUG("ws.header_size = %d, ws.N = %d\r\n", (int)ws.header_size, (int)ws.N);
		WSCLIENT_DEBUG("wsc->rx_len = %d\r\n", wsc->rx_len);

		if ((uint64_t)wsc->rx_len < ws.header_size + ws.N) {
			if (ws.N > (uint64_t)wsc->max_rx_len) {
				WSCLIENT_WARN("Got websocket message: header length = %u, payload length = 0x%x(H)+0x%x(L) exceed the max rx buf = %d set by user\n", ws.header_size,
							  (uint32_t)(ws.N >> 32), (uint32_t)ws.N, wsc->max_rx_len);

				wsc->enable_ws_msg_discarded = 1;
				wsc->discarded_ws_msg_total_len = ws.header_size + ws.N;
				wsc->discarded_ws_msg_rcv_len = (uint64_t)wsc->rx_len;
				WSCLIENT_WARN("Discard the long(total length: %u bytes) websocket message from server: %d bytes(total discarded length: %u bytes)\n",
							  (uint32_t)wsc->discarded_ws_msg_total_len, wsc->rx_len, (uint32_t)wsc->discarded_ws_msg_rcv_len);
				memset(wsc->rxbuf, 0, wsc->max_rx_len + 16);
				wsc->rx_len = 0;
			} else {
				WSCLIENT_DEBUG("ws.N(=%d) <= max_data_len_rx(=%d)\n", (int)ws.N, wsc->max_rx_len);
			}
			return; /* Need: ws.header_size+ws.N - rxbuf.size() */
		} else if ((uint64_t)wsc->rx_len == (ws.header_size + ws.N)) {
			if (ws.N > (uint64_t)wsc->max_rx_len) {
				WSCLIENT_WARN("Got websocket message: header length = %u, payload length = 0x%x(H)+0x%x(L) exceed the max rx buf = %d set by user\n", ws.header_size,
							  (uint32_t)(ws.N >> 32), (uint32_t)ws.N, wsc->max_rx_len);
				WSCLIENT_WARN("Discard the long(total length: %u bytes) websocket message from server\n", (uint32_t)wsc->rx_len);
				memset(wsc->rxbuf, 0, wsc->max_rx_len + 16);
				wsc->rx_len = 0;
				return;
			} else {
				WSCLIENT_DEBUG("ws.N(=%d) <= max_data_len_rx(=%d)\n", (int)ws.N, wsc->max_rx_len);
			}
		}

		// We got a whole message, now do something with it:
		if (ws.opcode == TEXT_FRAME || ws.opcode == BINARY_FRAME || ws.opcode == CONTINUATION) {
			if (ws.mask) {
				for (size_t i = 0; i != ws.N; ++i) {
					wsc->rxbuf[i + ws.header_size] ^= ws.masking_key[i & 0x3];
				}
			}
			uint8_t *rcvData = (uint8_t *)ws_malloc(wsc->wsclient_reallength + ws.N + 1);
			if (!rcvData) {
				if (wsc->receivedData) {
					ws_free(wsc->receivedData);
					wsc->receivedData = NULL;
				}
				return;
			}
			memcpy(rcvData, wsc->receivedData, wsc->wsclient_reallength);
			memcpy(rcvData + wsc->wsclient_reallength, wsc->rxbuf + ws.header_size, ws.N);
			WSCLIENT_DEBUG("Payload Data(len=%d)::%s\n", (int)ws.N, rcvData + wsc->wsclient_reallength);
			wsc->wsclient_reallength += ws.N;
			if (wsc->receivedData) {
				ws_free(wsc->receivedData);
			}
			wsc->receivedData = rcvData;

			WSCLIENT_DEBUG("ws.fin = %d, ws.N = %u\n", ws.fin, (uint32_t)ws.N);
			if (ws.fin != 0) {
				if (ws_receive_cb != NULL) {
					ws_receive_cb(wsclient, wsc->wsclient_reallength);
					WSCLIENT_DEBUG("wsclient_reallength = %d\n", wsc->wsclient_reallength);
					ws_free(wsc->receivedData);
					wsc->receivedData = NULL;
					wsc->wsclient_reallength = 0;
				}
			}
		} else if (ws.opcode == PING) {
			if (ws.mask) {
				for (size_t i = 0; i != ws.N; ++i) {
					wsc->rxbuf[i + ws.header_size] ^= ws.masking_key[i & 0x3];
				}
			}
			WSCLIENT_DEBUG("Get PING from server with payload length=%u\n", (uint32_t)ws.N);
			if (ws.N > 0) {
				ws_sendData(PONG, ws.N, &wsc->rxbuf[ws.header_size], 1, 1, wsc);
			} else {
				ws_sendData(PONG, 0, NULL, 1, 1, wsc);
			}
		} else if (ws.opcode == PONG) {
			WSCLIENT_DEBUG("Get PONG from server\n");
			if (ws_receive_pong_cb != NULL) {
				ws_receive_pong_cb(wsclient);
			}
		} else if (ws.opcode == CLOSE) {
			WSCLIENT_DEBUG("Get CLOSE from server\n");
			ws_close(wsclient);
		} else {
			WSCLIENT_ERROR("ERROR: Got unexpected WebSocket message.\n");
			wsc->fun_ops.client_close(wsc);
			//*wsclient = NULL;
			//should return, otherwise while loop will parse rxbuf continuesly, while rxbuf
			//has been free in client_close
			return;
		}

		wsc->rx_len = wsc->rx_len - (ws.header_size + ws.N);
		// If still got content, move it to the front of rxbuf
		if (wsc->rx_len > 0) {
			memmove(wsc->rxbuf, (wsc->rxbuf + ws.header_size + ws.N), wsc->rx_len);
		}
	}
}

int wss_set_fun_ops(wsclient_context *wsclient)
{
	struct ws_fun_ops *ws_fun = &wsclient->fun_ops;
	if (wsclient->use_ssl == 1) {
		ws_fun->hostname_connect = &wss_hostname_connect;
		ws_fun->client_close = &wss_client_close;
		ws_fun->client_read = &wss_client_read;
		ws_fun->client_send = &wss_client_send;
	} else {
		ws_fun->hostname_connect = &ws_hostname_connect;
		ws_fun->client_close = &ws_client_close;
		ws_fun->client_read = &ws_client_read;
		ws_fun->client_send = &ws_client_send;
	}
	return 0;
}

int ws_set_fun_ops(wsclient_context *wsclient)
{
	struct ws_fun_ops *ws_fun = &wsclient->fun_ops;

	if (wsclient->use_ssl == 0) {
		ws_fun->hostname_connect = &ws_hostname_connect;
		ws_fun->client_close = &ws_client_close;
		ws_fun->client_read = &ws_client_read;
		ws_fun->client_send = &ws_client_send;
		return 0;
	} else {
		WSCLIENT_ERROR("ERROR: Didn't define the USING_SSL\n");
		return -1;
	}
}

void ws_close(wsclient_context **wsclient)
{
	wsclient_context *wsc = *wsclient;
	if (wsclient == NULL || wsc->readyState == WSC_CLOSING || wsc->readyState == WSC_CLOSED) {
		return;
	}
	wsc->readyState = WSC_CLOSING;
	char pong_Frame[6] = {0x88, 0x80, 0x00, 0x00, 0x00, 0x00};
	wsc->fun_ops.client_send(wsc, pong_Frame, 6);
//	wsc->fun_ops.client_close(wsc);
//	*wsclient = NULL;	//To fix the ws_getReadyState() after ws_close() faied issue, then free the context in the example
	printf("\r\n\r\n\r\n>>>>>>>>>>>>>>>Closing the Connection with websocket server<<<<<<<<<<<<<<<<<<\r\n\r\n\r\n");
}

readyStateValues ws_getReadyState(wsclient_context *wsclient)
{
	return wsclient->readyState;
}

void ws_dispatch(void (*callback)(wsclient_context **, int))
{
	ws_receive_cb = callback;
}

void ws_dispatch_close(void (*callback)(wsclient_context *))
{
	ws_close_cb = callback;
}

void ws_pong(void (*callback)(wsclient_context **))
{
	ws_receive_pong_cb = callback;
}

void ws_poll(int timeout, wsclient_context **wsclient)   // timeout in milliseconds
{
	int ret = 0;
	send_buf *tmp_buf = NULL;
	wsclient_context *wsc;
	union {
		fd_set rfds;
		char dummy[16];
	} u_r;
	union {
		fd_set wfds;
		char dummy[16];
	} u_w;

	if (*wsclient == NULL) {
		WSCLIENT_ERROR("ERROR: wsclient is NULL!\n");
		return;
	}
	wsc = *wsclient;
	if (wsc->readyState == WSC_CLOSING) {
		closesocket(wsc->sockfd);
		wsc->fun_ops.client_close(wsc);
	}
	if (wsc->readyState == WSC_CLOSED) {
		if (timeout > 0) {
			struct timeval tv = { timeout / 1000, (timeout % 1000) * 1000 };
			select(0, NULL, NULL, NULL, &tv);
		}
		return;
	}
	if (timeout != 0) {
		struct timeval tv = { timeout / 1000, (timeout % 1000) * 1000 };

		memset(u_r.dummy, 0, sizeof(u_r.dummy));
		memset(u_w.dummy, 0, sizeof(u_w.dummy));

		FD_ZERO(&u_r.rfds);
		FD_ZERO(&u_w.wfds);
		FD_SET(wsc->sockfd, &u_r.rfds);
		if (rtos_queue_peek(wsc->ready_send_buf, (void *)&tmp_buf, 0) == RTK_SUCCESS) {
			if (tmp_buf->tx_len > 0) {
				FD_SET(wsc->sockfd, &u_w.wfds);
			}
		}
		ret = select(wsc->sockfd + 1, &u_r.rfds, &u_w.wfds, 0, timeout > 0 ? &tv : 0);
		if (ret == 0) {
			WSCLIENT_DEBUG("select() timeout!\n");
			return;
		} else if (ret < 0) {
			wsc->fun_ops.client_close(wsc);
			//*wsclient = NULL;
			WSCLIENT_ERROR("ERROR: Select error! ret = %d\n", ret);
			return;
		}

	}

	if ((wsc->sockfd >= 0) && FD_ISSET(wsc->sockfd, &u_r.rfds))  {
		while (1) {
			ret = wsc->fun_ops.client_read(wsc, (char *)&wsc->rxbuf[wsc->rx_len],
										   (wsc->max_rx_len + 16 - wsc->rx_len) > 1500 ? 1500 : (wsc->max_rx_len + 16 - wsc->rx_len));
			if (ret == 0) {
				WSCLIENT_DEBUG("client_read() data length = 0\n");
				break;
			} else if (ret < 0) {
				closesocket(wsc->sockfd);
				wsc->fun_ops.client_close(wsc);
				//*wsclient = NULL;
				WSCLIENT_ERROR("ERROR: Read data failed!\n");
				//should return, otherwise ws_dispatchBinary will parse rxbuf continuesly, while rxbuf
				//has been free in client_close
				return;
			} else {
				WSCLIENT_DEBUG("client_read() data length = %d\r\n", ret);
				if (wsc->enable_ws_msg_discarded == FALSE) {
					wsc->rx_len += ret;
				} else { //if(wsc->enable_ws_msg_discarded == TRUE)
					wsc->discarded_ws_msg_rcv_len += ret;
					WSCLIENT_WARN("Discard the long(total length: %u bytes) websocket message from server: %d bytes(total discarded length: %u bytes)\n",
								  (uint32_t)wsc->discarded_ws_msg_total_len, ret, (uint32_t)wsc->discarded_ws_msg_rcv_len);
					wsc->rx_len = 0;
					memset(wsc->rxbuf, 0, wsc->max_rx_len + 16);
					if (wsc->discarded_ws_msg_rcv_len >= wsc->discarded_ws_msg_total_len) {
						wsc->enable_ws_msg_discarded = FALSE;
						wsc->discarded_ws_msg_rcv_len = 0;
						wsc->discarded_ws_msg_total_len = 0;
					}
				}
			}
		}

		if (wsc->enable_ws_msg_discarded == FALSE) {
			ws_dispatchBinary(wsclient);
		}
	}

	//send buffer in ready queue
	if ((wsc->sockfd >= 0) && FD_ISSET(wsc->sockfd, &u_w.wfds))  {
		while (rtos_queue_peek(wsc->ready_send_buf, (void *)&tmp_buf, 0) == RTK_SUCCESS) {
			WSCLIENT_DEBUG("Pop up message buffer from ready queue\r\n");
			if (tmp_buf->tx_len > 0) {
				int remain_len = tmp_buf->tx_len - tmp_buf->send_offset;
				do {
					if (*wsclient == NULL) {
						return;
					}
					ret = wsc->fun_ops.client_send(wsc, (char *)tmp_buf->txbuf + tmp_buf->send_offset, (remain_len > 1500 ? 1500 : remain_len));
					if (ret == 0) {
						return;
					} else if (ret < 0) {
						closesocket(wsc->sockfd);
						wsc->fun_ops.client_close(wsc);
						//*wsclient = NULL;
						WSCLIENT_ERROR("ERROR: Send data faild!\n");
						return;
					} else {
						WSCLIENT_DEBUG("Send %d bytes data to websocket server\r\n", ret);
					}
					remain_len -= ret;
					tmp_buf->send_offset += ret;
				} while (remain_len > 0);
			}
			rtos_mutex_take(wsc->queue_mutex, MUTEX_WAIT_TIMEOUT);
			rtos_queue_receive(wsc->ready_send_buf, (void *)&tmp_buf, 0);
			wsc->ready_send_buf_num--;
			if (tmp_buf->tx_len > 0) {
				memset(tmp_buf->txbuf, 0, wsc->max_tx_len + 16);
				tmp_buf->tx_len = 0;
			}

			//If free buffers exceed the stable buffer number, it will be dynamically free.
			//Else, it wil be pushed back to recycle queue.
			if (wsc->recycle_send_buf_num >= wsc->stable_buf_num) {
				if (tmp_buf) {
					if (tmp_buf->txbuf) {
						ws_free(tmp_buf->txbuf);
					}
					tmp_buf->tx_len = 0;
					ws_free(tmp_buf);
					WSCLIENT_DEBUG("Buffer is enough. Recycle after polling");
				}
			} else {
				rtos_queue_send(wsc->recycle_send_buf, (void *)&tmp_buf, 0xFFFFFFFF);
				wsc->recycle_send_buf_num++;
				WSCLIENT_DEBUG("send back buffer to recycle queue\r\n");
			}
			rtos_mutex_give(wsc->queue_mutex);
		}
		WSCLIENT_DEBUG("ready_send_buf num: %d, recycle_send_buf num: %d\r\n", wsc->ready_send_buf_num, wsc->recycle_send_buf_num);
	}

}

int ws_sendPing(int use_mask, wsclient_context *wsclient)
{
	int ret = 0;
	ret = ws_sendData(PING, 0, NULL, use_mask, 1, wsclient);
	return ret;
}

int ws_sendBinary(uint8_t *message, int message_len, int use_mask, wsclient_context *wsclient)
{
	int ret = 0;
	if (message_len > wsclient->max_tx_len) {
		WSCLIENT_ERROR("ERROR: The length of data exceeded the max tx buf len: %d\n", wsclient->max_tx_len);
		return -1;
	}
	ret = ws_sendData(BINARY_FRAME, (size_t)message_len, (uint8_t *)message, use_mask, 1, wsclient);
	return ret;
}

int ws_send(char *message, int message_len, int use_mask, wsclient_context *wsclient)
{
	int ret = 0;
	WSCLIENT_DEBUG("Send data: %s\n", message);
	if (message_len > wsclient->max_tx_len) {
		WSCLIENT_ERROR("ERROR: The length of data exceeded the max tx buf len: %d\n", wsclient->max_tx_len);
		return -1;
	}
	ret = ws_sendData(TEXT_FRAME, (size_t)message_len, (uint8_t *)message, use_mask, 1, wsclient);
	return ret;
}

int ws_send_with_opcode(char *message, int message_len, int use_mask, uint8_t opcode, uint8_t fin_flag, wsclient_context *wsclient)
{
	int ret = 0;
	WSCLIENT_DEBUG("Send data: %s, opcode: %d\n", message, opcode);
	if (message_len > wsclient->max_tx_len) {
		WSCLIENT_ERROR("ERROR: The length of data exceeded the max tx buf len: %d\n", wsclient->max_tx_len);
		return -1;
	}
	ret = ws_sendData(opcode, (size_t)message_len, (uint8_t *)message, use_mask, fin_flag, wsclient);
	return ret;
}

int ws_connect_url(wsclient_context *wsclient)
{
	int ret;

	ret = wsclient->fun_ops.hostname_connect(wsclient);
	if (ret == -1) {
		wsclient->fun_ops.client_close(wsclient);
		return -1;
	} else {
		ret = ws_client_handshake(wsclient);
		if (ret <= 0) {
			WSCLIENT_ERROR("ERROR: Sending handshake failed\n");
			wsclient->fun_ops.client_close(wsclient);
			return -1;
		} else {
			ret = ws_check_handshake(wsclient);
			if (ret == 0) {
				WSCLIENT_DEBUG("Connected with websocket server!\n");
			} else {
				WSCLIENT_ERROR("ERROR: Response header is wrong\n");
				wsclient->fun_ops.client_close(wsclient);
				return -1;
			}
		}
	}

	int flag = 1;
	ret = setsockopt(wsclient->sockfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(flag)); // Disable Nagle's algorithm
	if (ret == 0) {
		ret = fcntl(wsclient->sockfd, F_SETFL, O_NONBLOCK);
		if (ret == 0) {
			wsclient->readyState = WSC_OPEN;
			printf("\r\n\r\n\r\n>>>>>>>>>>>>>>>Connected to websocket server<<<<<<<<<<<<<<<<<<\r\n\r\n\r\n");
			return wsclient->sockfd;
		} else {
			wsclient->fun_ops.client_close(wsclient);
			return -1;
		}
	} else {
		wsclient->fun_ops.client_close(wsclient);
		WSCLIENT_ERROR("ERROR: Failed to set socket option\n");
		return -1;
	}
}

void ws_setsockopt_keepalive(uint32_t keepalive_idle, uint32_t keepalive_interval, uint32_t keepalive_count)
{
	wsclient_keepalive_idle = keepalive_idle;
	wsclient_keepalive_interval = keepalive_interval;
	wsclient_keepalive_count = keepalive_count;
}

void ws_setsockopt_timeout(uint32_t recv_timeout, uint32_t send_timeout, uint32_t connect_timeout)
{
	wsclient_recvtimeout = recv_timeout;
	wsclient_sendtimeout = send_timeout;
	wsclient_connecttimeout = connect_timeout;
}

int ws_handshake_header_set_protocol(wsclient_context *wsclient, char *pro, int len)
{
	if (wsclient && pro && len > 0) {
		wsclient->protocol = (char *) ws_malloc(len + 1);
		if (wsclient->protocol == NULL) {
			WSCLIENT_ERROR("ERROR: Malloc(%d bytes) failed\n", len + 1);
			return -1;
		}

		wsclient->protocol_len = len;
		memcpy(wsclient->protocol, pro, len);
		return 0;
	} else {
		WSCLIENT_ERROR("ERROR: Failed to set header protocol\n");
		return -1;
	}
}

int ws_handshake_header_set_version(wsclient_context *wsclient, char *ver, int len)
{
	if (wsclient && ver && len > 0) {
		wsclient->version = (char *) ws_malloc(len + 1);
		if (wsclient->version == NULL) {
			WSCLIENT_ERROR("ERROR: Malloc(%d bytes) failed\n", len + 1);
			return -1;
		}

		wsclient->version_len = len;
		memcpy(wsclient->version, ver, len);
		return 0;
	} else {
		WSCLIENT_ERROR("ERROR: Failed to set header version\n");
		return -1;
	}
}

int ws_handshake_header_custom_token(wsclient_context *wsclient, char *cus, int len)
{
	if (wsclient && cus && len > 0) {
		wsclient->custom_token = (char *) ws_malloc(len + 1);
		if (wsclient->custom_token == NULL) {
			WSCLIENT_ERROR("ERROR: Malloc(%d bytes) failed\n", len + 1);
			return -1;
		}

		wsclient->custom_token_len = len;
		memcpy(wsclient->custom_token, cus, len);
		return 0;
	} else {
		WSCLIENT_ERROR("ERROR: Failed to set header custom token\n");
		return -1;
	}
}

int ws_handshake_set_header_fields(wsclient_context *wsclient, char *header, int len)
{
	if (wsclient && header && len > 0) {
		wsclient->header_fields = (char *) ws_malloc(len + 1);
		if (wsclient->header_fields == NULL) {
			WSCLIENT_ERROR("ERROR: Malloc(%d bytes) failed\n", len + 1);
			return -1;
		}

		wsclient->header_fields_len = len;
		memcpy(wsclient->header_fields, header, len);
		return 0;
	} else {
		WSCLIENT_ERROR("ERROR: Failed to set header\n");
		return -1;
	}
}

int ws_multisend_opts(wsclient_context *wsclient, int stable_buf_num)
{
	if (!wsclient) {
		WSCLIENT_ERROR("ERROR: Please create wsclient before calling ws_multisend_opts\n");
		return -1;
	}
	if (stable_buf_num < 0 || stable_buf_num > wsclient->max_queue_size) {
		WSCLIENT_ERROR("ERROR: stable_buf_num should be positive value and not exceed %d(max_queue_size)\n", wsclient->max_queue_size);
		return -1;
	}
	wsclient->stable_buf_num = stable_buf_num;
	WSCLIENT_DEBUG("stable_buf_num set to %d successfully\r\n", stable_buf_num);

	return 0;
}

void ws_set_senddata_block_time(uint32_t ms)
{
	wsclient_sendblocktime = ms;
}

wsclient_context *create_wsclient(char *url, int port, char *path, char *origin, int tx_buf_len, int rx_buf_len, int max_queue_size)
{
	int i;
	const int initial_item_num = 1;
	send_buf *tmp_buf = NULL;
	size_t url_len = 0;

	wsclient_context *wsclient = (wsclient_context *)ws_malloc(sizeof(wsclient_context));
	if (wsclient == NULL) {
		WSCLIENT_ERROR("ERROR: Malloc(%d bytes) failed\n", sizeof(wsclient_context));
		return NULL;
	}

	wsclient->port = port;
	wsclient->max_tx_len = tx_buf_len;
	wsclient->max_rx_len = rx_buf_len;

	if (origin == NULL) {
		wsclient->origin = NULL;
	} else {
		wsclient->origin = (char *)ws_malloc(sizeof(char) * (strlen(origin) + 1));
		if (!wsclient->origin) {
			WSCLIENT_ERROR("ERROR: Malloc(%d bytes) failed\n", strlen(origin) + 1);
			goto create_wsclient_fail;
		}
		memcpy(wsclient->origin, origin, strlen(origin));
	}

	if (strlen(url) >= DNS_MAX_NAME_LENGTH) {
		WSCLIENT_ERROR("ERROR: Url size exceeded DNS_MAX_NAME_LENGTH(=256)\n");
		goto create_wsclient_fail;
	}

	wsclient->host = NULL;
	wsclient->path = NULL;

	if (path) {
		wsclient->path = (char *)ws_malloc(sizeof(char) * (strlen(path) + 1));
		if (!wsclient->path) {
			ws_free(wsclient->origin);
			wsclient->origin = NULL;
			WSCLIENT_ERROR("ERROR: Malloc(%d bytes) failed\n", strlen(path) + 1);
			goto create_wsclient_fail;
		}
		memcpy(wsclient->path, path, strlen(path));
	}

	if (!strncmp(url, "wss://", strlen("wss://")) || !strncmp(url, "WSS://", strlen("WSS://"))) {
		url_len = strlen(url) - strlen("wss://");
	} else {
		url_len = strlen(url) - strlen("ws://");
	}

	wsclient->host = (char *)ws_malloc(sizeof(char) * (url_len + 1));
	if (!wsclient->host) {
		ws_free(wsclient->origin);
		wsclient->origin = NULL;
		ws_free(wsclient->path);
		wsclient->path = NULL;
		WSCLIENT_ERROR("ERROR: Malloc(%d bytes) failed\n", url_len + 1);
		goto create_wsclient_fail;
	}

	if (!strncmp(url, "wss://", strlen("wss://")) || !strncmp(url, "WSS://", strlen("WSS://"))) {
		memcpy(wsclient->host, (url + strlen("wss://")), (strlen(url) - strlen("wss://")));
		wsclient->use_ssl = 1;
		if (wsclient->port <= 0) {
			wsclient->port = 443;
		}
	} else if (!strncmp(url, "ws://", strlen("ws://")) || !strncmp(url, "WS://", strlen("WS://"))) {
		memcpy(wsclient->host, (url + strlen("ws://")), (strlen(url) - strlen("ws://")));
		wsclient->use_ssl = 0;
		if (wsclient->port <= 0) {
			wsclient->port = 80;
		}
	} else {
		WSCLIENT_ERROR("ERROR: Url format is wrong: %s\n", url);
		goto create_wsclient_fail;
	}

	wsclient->protocol = NULL;
	wsclient->protocol_len = 0;
	wsclient->version = NULL;
	wsclient->version_len = 0;
	wsclient->custom_token = NULL;
	wsclient->custom_token_len = 0;
	wsclient->header_fields = NULL;
	wsclient->header_fields_len = 0;

	wsclient->readyState = WSC_CLOSED;
	wsclient->sockfd = -1;
	wsclient->tx_len = 0;
	wsclient->rx_len = 0;
	wsclient->tls = NULL;
	wsclient->client_cert = NULL;
	wsclient->client_key = NULL;
	wsclient->ca_cert = NULL;

	//create queue for tx
	wsclient->queue_mutex = NULL;
	rtos_mutex_create(&wsclient->queue_mutex);
	if (wsclient->queue_mutex == NULL) {
		WSCLIENT_ERROR("ERROR: Queue mutex semaphore malloc fail\n\r");
		goto create_wsclient_fail;
	}
	if (max_queue_size <= 0) {
		WSCLIENT_ERROR("ERROR: Queue size must larger than 0: \r\n");
		goto create_wsclient_fail;
	}
	if (initial_item_num > max_queue_size) {//from Raymond Lee
		WSCLIENT_ERROR("ERROR: Initial_item_num larger than Queue size !!!\n\r");
		goto create_wsclient_fail;
	}
	wsclient->max_queue_size = max_queue_size;
	wsclient->stable_buf_num = max_queue_size; //set to max buffer num. default not enable buffer recycle mechenism
	wsclient->ready_send_buf_num = 0;
	wsclient->recycle_send_buf_num = 0;
	wsclient->ready_send_buf = NULL;
	wsclient->recycle_send_buf = NULL;

	rtos_queue_create(&wsclient->ready_send_buf, max_queue_size,  sizeof(send_buf *));
	rtos_queue_create(&wsclient->recycle_send_buf, max_queue_size, sizeof(send_buf *));
	if (wsclient->ready_send_buf == NULL || wsclient->recycle_send_buf == NULL) {
		WSCLIENT_ERROR("ERROR: Send_buf queue malloc fail\n\r");
		goto create_wsclient_fail;
	}

	// initialize 1 queue item
	// the rest of queue item will be increased in ws_sendData if queue item is not enough -Raymond Lee
	for (i = 0; i < initial_item_num; i++) {
		tmp_buf = ws_malloc(sizeof(send_buf));
		if (!tmp_buf) {
			WSCLIENT_ERROR("ERROR: Send_buf malloc fail\n\r");
			goto create_wsclient_fail;
		}

		WSCLIENT_DEBUG("Create queue item: %x\r\n", tmp_buf);

		tmp_buf->txbuf = (uint8_t *)ws_malloc(tx_buf_len + 16);
		if (!tmp_buf->txbuf) {
			WSCLIENT_ERROR("ERROR: Send_buf txbuf malloc fail\n\r");
			ws_free(tmp_buf);
			goto create_wsclient_fail;
		}

		WSCLIENT_DEBUG("Create buffer of queue item: %x\r\n", tmp_buf->txbuf);

		if (rtos_queue_send(wsclient->recycle_send_buf, (void *)&tmp_buf, RTOS_MAX_TIMEOUT) != RTK_SUCCESS) {
			WSCLIENT_ERROR("ERROR: Send_buf queue send fail\n\r");
			ws_free(tmp_buf->txbuf);
			ws_free(tmp_buf);
			goto create_wsclient_fail;
		}
		wsclient->recycle_send_buf_num++;
	}

	wsclient->txbuf = (uint8_t *)ws_malloc(tx_buf_len + 16);
	wsclient->rxbuf = (uint8_t *)ws_malloc(rx_buf_len + 16);
	wsclient->receivedData = NULL;
	wsclient->wsclient_reallength = 0;

	if (!wsclient->txbuf || !wsclient->rxbuf) {
		WSCLIENT_ERROR("ERROR: Malloc tx/rx buffer memory fail\n");
		goto create_wsclient_fail;
	}

	wsclient->txRsvBits.RSV1 = 0;
	wsclient->txRsvBits.RSV2 = 0;
	wsclient->txRsvBits.RSV3 = 0;

	wsclient->rxRsvBits.RSV1 = 0;
	wsclient->rxRsvBits.RSV2 = 0;
	wsclient->rxRsvBits.RSV3 = 0;


	if (wsclient_set_fun_ops(wsclient) < 0) {
		WSCLIENT_ERROR("ERROR: Init function failed\n");
		goto create_wsclient_fail;
	}

	return wsclient;

create_wsclient_fail:
	ws_client_close(wsclient);
	if (wsclient) {
		ws_free(wsclient);
	}
	return NULL;

}
