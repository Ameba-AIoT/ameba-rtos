#ifndef _INIC_SPI_DEV_H_
#define _INIC_SPI_DEV_H_

#if defined(CONFIG_FULLMAC_BRIDGE)
#include "whc_bridge_spi_dev.h"
#else
#include "whc_fullmac_spi_dev.h"
#endif

#define whc_dev_trigger_rx_handle   whc_spi_dev_trigger_rx_handle
#define whc_dev_tx_path_avail       whc_spi_dev_tx_path_avail
#define whc_dev_send                whc_spi_dev_send

#define DEV_READY_PIN				_PB_9
#define DEV_READY					1
#define DEV_BUSY					0

#define RX_REQ_PIN					_PB_8
#define DEV_RX_REQ					1
#define DEV_RX_IDLE					0

#define INIC_SPI_DEV				SPI0_DEV

#define INIC_SPI_CLK_MHZ			20
#define INIC_RECOVER_TIM_IDX			10
#define INIC_RECOVER_TO_US			1000

#define DEV_DMA_ALIGN				4

enum inic_spi_dma_type {
	INIC_SPI_TXDMA,
	INIC_SPI_RXDMA
};

#define DEV_STS_IDLE				0
#define DEV_STS_SPI_CS_LOW			BIT(0)
#define DEV_STS_WAIT_RXDMA_DONE			BIT(1)
#define DEV_STS_WAIT_TXDMA_DONE			BIT(2)

struct inic_spi_priv_t {
	u32 dev_status;

	rtos_mutex_t tx_lock;
	rtos_sema_t rxirq_sema;
	rtos_sema_t txirq_sema;
	rtos_sema_t spi_transfer_done_sema;
	rtos_sema_t free_skb_sema;

	GDMA_InitTypeDef SSITxGdmaInitStruct;
	GDMA_InitTypeDef SSIRxGdmaInitStruct;

	struct sk_buff *rx_skb;
	struct inic_buf_info *txbuf_info;

	u8 rx_req;
	u8 wait_tx;
	u8 wait_for_txbuf;

	u8 txdma_initialized: 1;
	u8 ssris_pending: 1;
	u8 set_devsts_pending: 1;

};

static inline void set_dev_rdy_pin(u8 status)
{
	GPIO_WriteBit_Critical(DEV_READY_PIN, status);
}

static inline void set_dev_rxreq_pin(u8 status)
{
	GPIO_WriteBit_Critical(RX_REQ_PIN, status);
}

void inic_spi_init(void);
void inic_dev_event_int_hdl(u8 *rxbuf, struct sk_buff *skb);
void whc_spi_dev_send(struct inic_buf_info *pbuf);
u8 whc_spi_dev_tx_path_avail(void);
void inic_dev_send_from_upper(u8 *buf, u8 *buf_alloc, u16 len);
void whc_spi_dev_trigger_rx_handle(void);

#endif

