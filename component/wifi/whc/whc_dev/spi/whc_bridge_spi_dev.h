#ifndef _BRIDGE_SPI_DEV_H_
#define _BRIDGE_SPI_DEV_H_

#define inic_dev_event_int_hdl  bridge_spi_dev_pkt_rx
#define inic_dev_init_lite      bridge_spi_dev_init
//#define whc_bridge_dev_send_to_host whc_bridge_spi_dev_send_to_host

void bridge_spi_dev_init(void);
void bridge_spi_dev_pkt_rx(u8 *rxbuf, struct sk_buff *skb);

#endif

