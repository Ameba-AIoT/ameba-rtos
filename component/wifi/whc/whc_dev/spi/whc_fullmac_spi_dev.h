#ifndef _FULLMAC_SPI_DEV_H_
#define _FULLMAC_SPI_DEV_H_

void inic_dev_init(void);
void inic_dev_event_int_hdl(u8 *rxbuf, struct sk_buff *skb);

#endif

