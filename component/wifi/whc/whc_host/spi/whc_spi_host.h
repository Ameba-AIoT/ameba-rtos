#ifndef _INIC_SPI_HOST_H_
#define _INIC_SPI_HOST_H_

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

#define INIC_SPI_DEV				SPI0_DEV

#define INIC_SPI_CLK_MHZ			20
#define INIC_RECOVER_TIM_IDX			0
#define INIC_RECOVER_TO_US			1000

#define DEV_DMA_ALIGN				4

#define SIZE_RX_DESC	0
#define SIZE_TX_DESC	0

struct inic_spi_host_priv_t {
	u8 dev_state;
	u8 host_recv_state;
	u8 host_tx_state;
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
	struct inic_buf_info *txbuf_info; /* to free in hdl */

	u8 txdma_initialized: 1;
};



enum inic_spi_dma_type {
	INIC_SPI_TXDMA,
	INIC_SPI_RXDMA
};


#define SPI_DMA_ALIGN(x)	((((x-1)>>5)+1)<<5) //alignement to 32
#define SPI_BUFSZ		(SPI_DMA_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct inic_msg_info)))
#define SPI_SKB_RSVD_LEN	N_BYTE_ALIGMENT(SKB_WLAN_TX_EXTRA_LEN - sizeof(struct inic_msg_info), 4)

#define WIFI_STACK_SIZE_INIC_RX_REQ_TASK		(268 + 128 + CONTEXT_SAVE_SIZE)
#define WIFI_STACK_INIC_SPI_RXDMA_IRQ_TASK		(332 + 128 + CONTEXT_SAVE_SIZE)
#define WIFI_STACK_INIC_SPI_TXDMA_IRQ_TASK		(168 + 128 + CONTEXT_SAVE_SIZE)

static inline void set_host_rdy_pin(u8 status)
{
	GPIO_WriteBit(HOST_READY_PIN, status);
}

void inic_host_send_data(struct inic_buf_info *pbuf);
void inic_host_send_from_upper(u8 *buf, u8 *buf_alloc, u16 len);

#endif

