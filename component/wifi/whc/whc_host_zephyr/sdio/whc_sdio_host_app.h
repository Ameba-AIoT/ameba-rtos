#ifndef __WHC_SDIO_HOST_APP_H__
#define __WHC_SDIO_HOST_APP_H__

struct event_priv_t {
	struct k_mutex send_mutex;
	struct k_sem api_ret_sema;

	uint8_t *rx_ret_msg;

	uint8_t b_waiting_for_ret: 1;
};

void whc_host_deliver_rxbuf_to_user(u8 *pbuf);
void whc_host_sdio_send_msg(uint8_t *msg, uint32_t msg_len, uint8_t *ret, uint32_t ret_len);

int whc_host_get_ip(uint8_t idx, uint8_t *ip);
void whc_host_set_state(uint8_t state);
int whc_host_get_mac_addr(int idx, struct rtw_mac *mac, u8 efuse);

/* TODO */
void wifi_disconnect(void);

#endif
