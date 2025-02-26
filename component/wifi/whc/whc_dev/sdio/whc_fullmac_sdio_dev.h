#ifndef _WHC_FULLMAC_SDIO_DEV_H_
#define _WHC_FULLMAC_SDIO_DEV_H_

#define whc_sdio_dev_event_int_hdl  whc_fullmac_sdio_dev_pkt_rx
#define whc_dev_init                whc_fullmac_sdio_dev_init

void whc_fullmac_sdio_dev_pkt_rx(u8 *rxbuf, struct sk_buff *skb, u16 size);
void whc_fullmac_sdio_dev_init(void);

#endif

