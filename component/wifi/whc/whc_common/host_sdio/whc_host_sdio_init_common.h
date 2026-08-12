#ifndef __WHC_HOST_SDIO_INIT_COMMON_H__
#define __WHC_HOST_SDIO_INIT_COMMON_H__

struct whc_sdio;

/* common */
uint8_t whc_sdio_host_query_txbd_status(struct whc_sdio *priv);
uint8_t whc_sdio_host_get_tx_max_size(struct whc_sdio *priv);
void whc_sdio_host_init_interrupt(struct whc_sdio *priv);
uint32_t whc_sdio_host_init(struct whc_sdio *priv);

/* platform-specific, provided by each port's whc_sdio_init.c */
extern uint32_t whc_sdio_host_enable_func(struct whc_sdio *priv);
extern void rtw_sdio_polling_init(void);

#endif
