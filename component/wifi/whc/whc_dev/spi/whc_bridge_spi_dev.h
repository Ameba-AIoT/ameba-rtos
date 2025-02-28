#ifndef _BRIDGE_SPI_DEV_H_
#define _BRIDGE_SPI_DEV_H_

#define whc_spi_dev_event_int_hdl  whc_bridge_spi_dev_pkt_rx
#define whc_dev_init_lite      whc_bridge_spi_dev_init
#define whc_dev_api_send_data   whc_bridge_spi_send_data

void whc_bridge_spi_dev_init(void);
void whc_bridge_spi_dev_pkt_rx(u8 *rxbuf, struct sk_buff *skb);
void whc_bridge_spi_send_data(u8 *buf, u32 len);

#endif

