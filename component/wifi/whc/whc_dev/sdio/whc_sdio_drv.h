#ifndef _WHC_SDIO_DRV_H_
#define _WHC_SDIO_DRV_H_

#define SPDIO_TX_BD_NUM	8	//n*2, 2 bd for one transaction
#define SPDIO_RX_BD_NUM	4

#define DEV_DMA_ALIGN	SPDIO_DMA_ALIGN_4

#define whc_dev_trigger_rx_handle   whc_sdio_dev_trigger_rx_handle
#define whc_dev_tx_path_avail       whc_sdio_dev_tx_path_avail
#define whc_dev_send                whc_sdio_dev_send

struct whc_sdio_priv_t {
	struct spdio_t dev;

	rtos_mutex_t tx_lock;
	rtos_sema_t rxbd_release_sema;

};

void whc_sdio_dev_send(struct spdio_buf_t *pbuf);
u8 whc_sdio_dev_tx_path_avail(void);
void whc_sdio_dev_init(void);
void whc_sdio_dev_send_data(u8 *data, u32 len, u8 need_free);
void whc_sdio_dev_trigger_rx_handle(void);

#endif

