#ifndef _FULLMAC_SPI_DEV_H_
#define _FULLMAC_SPI_DEV_H_

#define whc_spi_dev_event_int_hdl   whc_fullmac_spi_dev_pkt_rx
#define whc_dev_init                whc_fullmac_spi_dev_init

void whc_fullmac_spi_dev_init(void);
void whc_fullmac_spi_dev_pkt_rx(u8 *rxbuf, struct sk_buff *skb);

#endif

