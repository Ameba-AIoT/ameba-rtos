#ifndef __WHC_HOST_SDIO_INIT_COMMON_H__
#define __WHC_HOST_SDIO_INIT_COMMON_H__

struct whc_sdio;

uint8_t rtw_sdio_query_txbd_status(struct whc_sdio *priv);
uint8_t rtw_sdio_get_tx_max_size(struct whc_sdio *priv);
void rtw_sdio_init_interrupt(struct whc_sdio *priv);

#endif
