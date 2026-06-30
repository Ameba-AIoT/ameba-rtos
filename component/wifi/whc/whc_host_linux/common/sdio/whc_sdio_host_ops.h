#ifndef __RTW_SDIO_OPS_H__
#define __RTW_SDIO_OPS_H__

/* SDIO common functions */
u32 rtw_sdio_init_common(struct whc_sdio *priv);
u32 rtw_sdio_get_rx_len(struct whc_sdio *priv);

/* SDIO data transfer functions */
void whc_sdio_host_send_data(u8 *buf, u32 len, struct sk_buff *pskb);

int whc_sdio_xfer_download(struct whc_sdio *priv);
#endif
