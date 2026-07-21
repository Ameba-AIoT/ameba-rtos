#include "rtw_whc_common.h"
struct event_priv_t event_priv;

__weak void whc_host_pkt_rx_to_user(u8 *pbuf)
{
	int counter = 0;
	/* pbuf is the raw rx buffer; whc_cmd_path_hdr starts at pbuf + SIZE_RX_DESC */
	uint32_t event = *(uint32_t *)(pbuf + SIZE_RX_DESC);

	if (event != WHC_WIFI_EVT_CMD) {
		printf("%s: recv pbuf, user realize here \n", __func__);
		whc_free(pbuf);
		return;
	}

	if (!event_priv.b_waiting_for_ret) {
		whc_free(pbuf);
		return;
	}

	while (event_priv.rx_ret_msg) {
		/* waiting last buf done */
		k_sleep(K_MSEC(1));
		counter++;
		if (counter == 500) {
			counter = 0;
			printf("%s: waiting for last event \n", __func__);
		}
	}

	event_priv.rx_ret_msg = pbuf;
	k_sem_give(&event_priv.api_ret_sema);
}

/**
 * @brief  sdio host send msg to dev.
 * @param  msg: msg buf
 * @param  msg_len: message len to be sent.
 * @param  ret: return value buf if have return value
 * @param  ret_len: len of return value.
 * @return none.
 */
void whc_host_sdio_send_msg(uint8_t *msg, uint32_t msg_len, uint8_t *ret, uint32_t ret_len)
{
	struct whc_cmd_path_hdr *hdr;
	uint8_t *buf = NULL;
	uint8_t *ptr = NULL;
	uint32_t buf_len = 0;

	k_mutex_lock(&event_priv.send_mutex, MUTEX_WAIT_TIMEOUT);

	buf_len = msg_len + SIZE_TX_DESC + sizeof(struct whc_cmd_path_hdr);
	buf = whc_malloc(buf_len);
	if (!buf) {
		goto exit;
	}

	hdr = (struct whc_cmd_path_hdr *)(buf + SIZE_TX_DESC);
	hdr->event = WHC_WIFI_EVT_CMD;
	hdr->len = msg_len;
	if (msg_len > 0) {
		memcpy((void *)(buf + SIZE_TX_DESC + sizeof(struct whc_cmd_path_hdr)), msg, msg_len);
	}

	whc_host_sdio_send_data(buf, buf_len, NULL);

	if (ret && (ret_len != 0)) {
		event_priv.b_waiting_for_ret = 1;
		k_sem_take(&event_priv.api_ret_sema, MUTEX_WAIT_TIMEOUT);
		event_priv.b_waiting_for_ret = 0;

		ptr = event_priv.rx_ret_msg;
		event_priv.rx_ret_msg = NULL;

		if (ptr != NULL) {
			if (ret != NULL && ret_len != 0) {
				/* device echoes the request prefix then appends result;
				 * skip INIC_RX_DESC + whc_cmd_path_hdr + msg_len to reach return data */
				memcpy(ret, ptr + SIZE_RX_DESC + sizeof(struct whc_cmd_path_hdr) + msg_len, ret_len);
			}
			whc_free((uint8_t *)ptr);
		} else {
			printf("Host API return value is NULL!\n");
		}
	}

exit:
	if (buf) {
		whc_free(buf);
	}

	k_mutex_unlock(&event_priv.send_mutex);
	return;
}

int whc_host_get_mac_addr(u8 idx, struct rtw_mac *mac, u8 efuse)
{
	(void)efuse;
	u8 msg[6];
	u8 *ptr = msg;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	*ptr++ = WHC_WIFI_TEST_GET_MAC_ADDR;
	*ptr = idx;

	whc_host_sdio_send_msg(msg, sizeof(msg), (uint8_t *)mac, sizeof(struct rtw_mac));
	return 0;
}


int whc_host_get_ip(uint8_t idx, uint8_t *ip)
{
	(void)idx;
	u8 msg[5];
	u8 *ptr = msg;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	*ptr = WHC_WIFI_TEST_GET_IP;
	memset(ip, 0, 4);

	whc_host_sdio_send_msg(msg, sizeof(msg), ip, 4);
	return RTK_SUCCESS;
}

void whc_host_set_state(uint8_t state)
{
	u8 msg[5];
	u8 *ptr = msg;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	*ptr = state ? WHC_WIFI_TEST_SET_READY : WHC_WIFI_TEST_SET_UNREADY;

	whc_host_sdio_send_msg(msg, sizeof(msg), NULL, 0);
}

void wifi_disconnect(void)
{
	//todo
}

int wifi_scan(void)
{
	int ret = 0;

	//to do if need in zephyr
	return ret;
}

