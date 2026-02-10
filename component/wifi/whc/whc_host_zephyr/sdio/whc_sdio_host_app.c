#include "rtw_whc_common.h"
struct event_priv_t event_priv;

__weak void whc_host_pkt_rx_to_user(u8 *pbuf)
{
	int counter = 0;
	uint32_t event = *(uint32_t *)(pbuf + SIZE_RX_DESC);

	if (event == WHC_WIFI_TEST) {
		if (!event_priv.b_waiting_for_ret) {
			k_free(pbuf);
			return;
		}

		while (event_priv.rx_ret_msg) {
			/* waiting last buf done */
			k_sleep(K_MSEC(1));
			counter ++;
			if (counter == 500) {
				counter = 0;
				printf("%s: waiting for last event \n", __func__);
			}
		}

		event_priv.rx_ret_msg = pbuf;
		k_sem_give(&event_priv.api_ret_sema);
	} else {

		printf("%s: recv pbuf, user realize here \n", __func__);
		k_free(pbuf);
	}
}

/**
 * @brief  bridge sdio host send msg to dev.
 * @param  msg: msg buf
 * @param  msg_len: message len to be sent.
 * @param  ret: return value buf if have return value
 * @param  ret_len: len of return value.
 * @return none.
 */
// linux name: whc_bridge_host_send_data_to_dev
void whc_bridge_sdio_host_send_msg(uint8_t *msg, uint32_t msg_len, uint8_t *ret, uint32_t ret_len)
{
	uint8_t *buf = NULL;
	uint8_t *ptr = NULL;
	struct whc_api_info *ret_msg;
	uint32_t buf_len = 0;

	//printf("Host Call API %d \n", id);

	k_mutex_lock(&event_priv.send_mutex, MUTEX_WAIT_TIMEOUT);

	buf_len = msg_len + SIZE_TX_DESC;
	buf = k_malloc(buf_len);
	if (!buf) {
		goto exit;
	}

	if (msg_len > 0) {
		memcpy((void *)(buf + SIZE_TX_DESC), msg, msg_len);
	}

	/* send ret_msg + ret_val(buf, len) */
	whc_bridge_sdio_host_send_data(buf, buf_len, NULL);

	/* wait for API calling done */
	if (ret && (ret_len != 0)) {
		event_priv.b_waiting_for_ret = 1;
		k_sem_take(&event_priv.api_ret_sema, MUTEX_WAIT_TIMEOUT);
		event_priv.b_waiting_for_ret = 0;

		//printf("Device API %d return\n", id);

		/* get return value */
		ptr = event_priv.rx_ret_msg;
		ret_msg = (struct whc_api_info *)(ptr + SIZE_RX_DESC);
		event_priv.rx_ret_msg = NULL;

		if (ret_msg != NULL) {
			/* copy return value*/
			if (ret != NULL && ret_len != 0) {
				memcpy(ret, (u8 *)(ret_msg + 1), ret_len);
			}
			/* free rx buffer */
			k_free((uint8_t *)ptr);
		} else {
			/* free rx buffer */
			k_free((uint8_t *)ptr);
			printf("Host API return value is NULL!\n");
		}


	}

	//printf("Device API %d done\n", id);

exit:
	if (buf) {
		k_free(buf);
	}

	k_mutex_unlock(&event_priv.send_mutex);
	return;

}

int whc_host_get_mac_addr(u8 idx, struct rtw_mac *mac, u8 efuse)
{
	(void)efuse;

	int ret = 0;
	struct whc_api_info *msginfo;
	u8 *ptr;

	u8 *msg = NULL;
	u8 len = sizeof(struct whc_api_info) + 1;

	msg = k_malloc(len);
	msginfo = (struct whc_api_info *)msg;
	ptr = msg;

	msginfo->event = WHC_WIFI_TEST;
	msginfo->api_id = WHC_WIFI_TEST_GET_MAC_ADDR;
	ptr += sizeof(struct whc_api_info);
	*ptr = idx;

	whc_bridge_sdio_host_send_msg(msg, len, (uint8_t *)mac, sizeof(struct rtw_mac));

	k_free(msg);

	return ret;
}


int whc_host_get_ip(uint8_t idx, uint8_t *ip)
{
	struct whc_api_info msginfo;
	u8 len = sizeof(struct whc_api_info);

	msginfo.event = WHC_WIFI_TEST;
	msginfo.api_id = WHC_WIFI_TEST_GET_IP;
	memset(ip, 0, 4);

	whc_bridge_sdio_host_send_msg((u8 *)&msginfo, len, (uint8_t *)ip, 4);
	return RTK_SUCCESS;
}

void whc_host_set_state(uint8_t state)
{
	struct whc_api_info msginfo;
	u8 len = sizeof(struct whc_api_info);

	msginfo.event = WHC_WIFI_TEST;
	if (state) {
		msginfo.api_id = WHC_WIFI_TEST_SET_READY;
	} else {
		msginfo.api_id = WHC_WIFI_TEST_SET_UNREADY;
	}

	whc_bridge_sdio_host_send_msg((u8 *)&msginfo, len, NULL, 0);

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

