#ifndef _WHC_BRIDGE_SDIO_DEV_H_
#define _WHC_BRIDGE_SDIO_DEV_H_

#define inic_dev_event_int_hdl  whc_bridge_sdio_dev_pkt_rx
#define inic_dev_init_lite      whc_bridge_sdio_dev_init
#define whc_bridge_dev_send_to_host whc_bridge_sdio_dev_send_to_host

void whc_bridge_dev_pkt_rx_to_user(u8 *rxbuf);
void whc_bridge_sdio_dev_pkt_rx(u8 *rxbuf, struct sk_buff *skb);
void whc_bridge_sdio_dev_send_to_host(u8 *data, u32 len, u8 need_free);
void whc_bridge_sdio_dev_init(void);

#endif

