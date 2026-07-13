/**
  ******************************************************************************
  * @file    whc_host_common.c
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include "whc_host.h"
#include <lwip/pbuf.h>

int whc_host_init_done;

extern struct event_priv_t event_priv;
void(*bt_inic_spi_recv_host_ptr)(uint8_t *buffer, uint16_t len);

#define RX_PBUF_RETRY_MAX		100

struct whc_txbuf_info_t *whc_host_alloc_buf_info(u8 *buf, u32 len, void *alloc_buf, u8 is_skb)
{
	struct whc_txbuf_info_t *buf_info;

	buf_info = (struct whc_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));
	if (!buf_info) {
		return NULL;
	}

	buf_info->txbuf_info.buf_allocated = buf_info->txbuf_info.buf_addr = (u32)buf;
	buf_info->txbuf_info.size_allocated = buf_info->txbuf_info.buf_size = len;
	buf_info->ptr = alloc_buf;
	buf_info->is_skb = is_skb;

	return buf_info;
}

/**
 * @brief  common rx handler for all whc transports (SPI/SDIO/UART).
 *         parse whc_msg_info, copy data to pbuf and deliver to lwIP.
 * @param  buf: pointer to whc_msg_info (transport-specific headers must
 *         already be stripped / accounted for by the caller).
 * @return none.
 */
void whc_host_rx_handler(u8 *buf)
{
	struct whc_msg_info *msg_info = (struct whc_msg_info *)buf;
	char *data = (char *)(buf + sizeof(struct whc_msg_info) + msg_info->pad_len);
	struct pbuf *p_buf;
	struct pbuf *temp_buf;
	static int rx_alloc_fail_cnt = 0;
	int retry_cnt = 0;

retry:
	p_buf = pbuf_alloc(PBUF_RAW, msg_info->data_len, PBUF_POOL);
	if (p_buf == NULL) {
		rx_alloc_fail_cnt++;
		rtos_time_delay_ms(1);
		if (retry_cnt++ < RX_PBUF_RETRY_MAX) {
			goto retry;
		}
		RTK_LOGE(TAG_WLAN_INIC, "%s: Alloc pbuf Err\n", __func__);
		return;
	}

	if (rx_alloc_fail_cnt >= 1000) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: Alloc pbuf retry %d times\n", __func__, rx_alloc_fail_cnt);
		rx_alloc_fail_cnt = 0;
	}

	temp_buf = p_buf;
	while (temp_buf) {
		memcpy(temp_buf->payload, data, temp_buf->len);
		data += temp_buf->len;
		temp_buf = temp_buf->next;
	}

	netif_adapter_wifi_recv_whc(msg_info->wlan_idx, p_buf);
}

/**
 * @brief  common event dispatch for all whc transports (SPI/SDIO/UART).
 *         parse the whc event and route to pkt / api / cmd handling.
 *         buffer ownership is transferred here and it is freed before return.
 * @param  buf: received buffer starting with the RX descriptor. The whc payload
 *         starts at buf + SIZE_RX_DESC (SIZE_RX_DESC is 0 on SPI/UART). Since the
 *         three transports are mutually exclusive per build, SIZE_RX_DESC resolves
 *         to the active transport's value.
 * @param  copy_len: payload bytes to copy for the API path; each transport knows
 *         its own length (SDIO pkt_len / SPI SPI_BUFSZ / UART payload_len).
 * @return none.
 */
void whc_host_recv_dispatch(u8 *buf, u32 len)
{
	u32 event = *(u32 *)(buf + SIZE_RX_DESC);
#ifdef CONFIG_WHC_CMD_PATH
	struct whc_cmd_path_hdr *hdr;
#endif
#ifdef CONFIG_WHC_WIFI_API_PATH
	struct whc_api_info *ret_msg;
	u8 *api_buf;
	int counter = 0;
#else
	(void)len;
#endif

	switch (event) {
	case WHC_WIFI_EVT_RECV_PKTS:
		whc_host_rx_handler(buf + SIZE_RX_DESC);
		break;
#ifdef CONFIG_WHC_WIFI_API_PATH
	case WHC_WIFI_EVT_API_CALL:
		/* copy out the whc_api_info payload (skip RX_DESC) so the consumer can
		 * read from offset 0; the original buf is freed below. */
		api_buf = rtos_mem_zmalloc(len);
		memcpy(api_buf, buf + SIZE_RX_DESC, len);
		while (event_priv.rx_api_msg) {
			rtos_time_delay_ms(1);
			counter ++;
			if (counter == 500) {
				counter = 0;
				RTK_LOGE(TAG_WLAN_INIC, "%s: waiting for last event \n", __func__);
			}
		}
		event_priv.rx_api_msg = api_buf;
		rtos_sema_give(event_priv.task_wake_sema);
		break;
	case WHC_WIFI_EVT_API_RETURN:
		if (event_priv.b_waiting_for_ret) {
			api_buf = rtos_mem_zmalloc(len);
			memcpy(api_buf, buf + SIZE_RX_DESC, len);
			while (event_priv.rx_ret_msg) {
				rtos_time_delay_ms(1);
				counter ++;
				if (counter == 500) {
					counter = 0;
					RTK_LOGE(TAG_WLAN_INIC, "%s: waiting for last event \n", __func__);
				}
			}
			event_priv.rx_ret_msg = api_buf;

			/* unblock API calling func */
			rtos_sema_give(event_priv.api_ret_sema);
		} else {
			ret_msg = (struct whc_api_info *)(buf + SIZE_RX_DESC);
			RTK_LOGE(TAG_WLAN_INIC, "too late to receive API ret, ID: 0x%x!\n", ret_msg->api_id);
		}
		break;
#endif
#ifdef CONFIG_WHC_CMD_PATH
	case WHC_WIFI_EVT_CMD:
		/* WHY(71066): device prepends whc_cmd_path_hdr{event,len} before the cmd
		 * payload (see whc_dev_api_send_to_host). Must skip the hdr and use hdr->len;
		 * don't pass buf/desc size directly. */
		hdr = (struct whc_cmd_path_hdr *)(buf + SIZE_RX_DESC);
		whc_host_pkt_rx_to_user((u8 *)(hdr + 1), hdr->len);
		break;
#endif
	default:
		if (event >= WHC_BT_EVT_BASE) {
			/* copy by bt, skb no change */
			if (bt_inic_spi_recv_host_ptr) {
				bt_inic_spi_recv_host_ptr(buf + SIZE_RX_DESC, len - SIZE_RX_DESC);
			}
		} else {
			RTK_LOGE(TAG_WLAN_INIC, "%s: unknown event:%d\n", __func__, event);
		}
		break;
	}

	rtos_mem_free(buf);
}
