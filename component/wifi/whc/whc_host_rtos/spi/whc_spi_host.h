#ifndef _WHC_SPI_HOST_H_
#define _WHC_SPI_HOST_H_

#include "spi_api.h"
#include "spi_ex_api.h"

#define DEV_READY_PIN				_PB_9
#define DEV_READY					1
#define DEV_BUSY					0

#define RX_REQ_PIN					_PB_8
#define DEV_RX_REQ					1
#define DEV_RX_IDLE					0

#define HOST_READY_PIN				_PB_26
#define HOST_READY					1
#define HOST_BUSY					0

#define WHC_SPI_DEV				SPI0_DEV

#define WHC_SPI_CLK_MHZ			20
#define WHC_RECOVER_TIM_IDX			0
#define WHC_RECOVER_TO_US			1000

#define DEV_DMA_ALIGN				4

#define SIZE_RX_DESC	0
#define SIZE_TX_DESC	0

#define whc_host_send_data       whc_spi_host_send_data
#define whc_host_init            whc_spi_host_init

#define HOST_RX_DMA_CB_DONE		BIT(0)
#define HOST_TX_DMA_CB_DONE		BIT(1)
struct whc_spi_host_priv_t {
	u8 dev_state;
	u8 host_recv_state;
	u8 host_tx_state;
	u8 host_dma_waiting_status;

	rtos_mutex_t	dev_lock; /* mutex to protect send host event_priv message */
	rtos_sema_t dev_rdy_sema;
	rtos_sema_t host_send; /* sema to protect inic  host send */
	rtos_sema_t host_send_api; /* sema to protect inic ipc host send api */
	rtos_sema_t host_recv_wake; /* for recv task */
	rtos_sema_t rxirq_sema;
	rtos_sema_t txirq_sema;
	rtos_sema_t host_recv_done; /* for recv task */

	GDMA_InitTypeDef SSITxGdmaInitStruct;
	GDMA_InitTypeDef SSIRxGdmaInitStruct;

	u8 *rx_buf;
	u8 *dummy_tx_buf;
	struct whc_buf_info *txbuf_info; /* to free in hdl */

	u8 txdma_initialized: 1;
};



enum whc_spi_dma_type {
	WHC_SPI_TXDMA,
	WHC_SPI_RXDMA
};


#define SPI_DMA_ALIGN(x)	((((x-1)>>5)+1)<<5) //alignement to 32
#define SPI_BUFSZ		(SPI_DMA_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct whc_msg_info)))
#define SPI_SKB_RSVD_LEN	N_BYTE_ALIGMENT(SKB_WLAN_TX_EXTRA_LEN - sizeof(struct whc_msg_info), 4)

#define WIFI_STACK_SIZE_INIC_RX_REQ_TASK		    (268 + 128 + CONTEXT_SAVE_SIZE)
#define WIFI_STACK_WHC_SPI_HOST_RXDMA_IRQ_TASK		4096 //TODO
#define WIFI_STACK_WHC_SPI_HOST_TXDMA_IRQ_TASK		(168 + 128 + CONTEXT_SAVE_SIZE)

static inline void set_host_rdy_pin(u8 status)
{
	GPIO_WriteBit(HOST_READY_PIN, status);
}

void whc_spi_host_send_data(struct whc_buf_info *pbuf);
void whc_spi_host_send_to_dev(u8 *buf, u8 *buf_alloc, u16 len);
void whc_spi_host_init(void);

#endif