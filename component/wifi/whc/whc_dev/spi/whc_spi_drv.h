#ifndef _WHC_SPI_DRV_H_
#define _WHC_SPI_DRV_H_

#include "whc_spi_dev.h"

#define whc_dev_trigger_rx_handle   whc_spi_dev_trigger_rx_handle
#define whc_dev_tx_path_avail       whc_spi_dev_tx_path_avail
#define whc_dev_send                whc_spi_dev_send

#define whc_spi_dev_event_int_hdl   whc_spi_dev_pkt_rx


#ifdef CONFIG_AMEBAGREEN2  // need use QFN100
#define PINMUX_FUNCTION_SPIS	    PINMUX_FUNCTION_SPI0
#define DEV_READY_PIN				_PA_18
#define RX_REQ_PIN					_PA_19
#define SPI0_MOSI                   _PA_30
#define SPI0_MISO                   _PA_31
#define SPI0_SCLK                   _PA_29
#define SPI0_CS                     _PB_0
#define WHC_RECOVER_TIM_IDX			0 //todo: need check

#elif defined (CONFIG_AMEBADPLUS)
#define PINMUX_FUNCTION_SPIS	    PINMUX_FUNCTION_SPI
#define DEV_READY_PIN				_PB_9
#define RX_REQ_PIN					_PB_8
#define SPI0_MOSI	                _PB_24
#define SPI0_MISO	                _PB_25
#define SPI0_SCLK	                _PB_23
#define SPI0_CS		                _PB_26
#define WHC_RECOVER_TIM_IDX			10

#elif defined (CONFIG_AMEBALITE)
#define PINMUX_FUNCTION_SPIS	    PINMUX_FUNCTION_SPI
#define DEV_READY_PIN				_PB_3   // amebalite use different pin due to _PA_1(num 15, GPIO22 got by command "pinout") will be pull down by raspberry pi
#define RX_REQ_PIN					_PB_2
#define SPI0_MOSI	                _PA_29
#define SPI0_MISO	                _PA_30
#define SPI0_SCLK	                _PA_28
#define SPI0_CS		                _PA_31
#define WHC_RECOVER_TIM_IDX			10
#endif

#define DEV_READY					1
#define DEV_BUSY					0

#define DEV_RX_REQ					1
#define DEV_RX_IDLE					0

#define WHC_SPI_DEV				    SPI0_DEV

#define WHC_SPI_CLK_MHZ			    20
#define WHC_RECOVER_TO_US			1000
#define WHC_DEV_SPI_TRANSFER_TIMEOUT	1000

#define DEV_DMA_ALIGN				4

enum whc_spi_dma_type {
	WHC_SPI_TXDMA,
	WHC_SPI_RXDMA
};

#define DEV_STS_IDLE				0
#define DEV_STS_SPI_CS_LOW			BIT(0)
#define DEV_STS_WAIT_RXDMA_DONE			BIT(1)
#define DEV_STS_WAIT_TXDMA_DONE			BIT(2)

struct whc_spi_priv_t {
	u32 dev_status;

	rtos_mutex_t tx_lock;
	rtos_sema_t rxirq_sema;
	rtos_sema_t txirq_sema;
	rtos_sema_t spi_transfer_done_sema;
	rtos_sema_t free_skb_sema;

	GDMA_InitTypeDef SSITxGdmaInitStruct;
	GDMA_InitTypeDef SSIRxGdmaInitStruct;

	struct sk_buff *rx_skb;
	struct whc_buf_info *txbuf_info;

	u8 rx_req;
	u8 wait_tx;
	u8 wait_for_txbuf;

	u8 txdma_initialized: 1;
	u8 ssris_pending: 1;
	u8 set_devsts_pending: 1;

};

static inline void set_dev_rdy_pin(u8 status)
{
#ifdef CONFIG_AMEBAGREEN2
	GPIO_WriteBit(DEV_READY_PIN, status);
#else
	GPIO_WriteBit_Critical(DEV_READY_PIN, status);
#endif
}

static inline void set_dev_rxreq_pin(u8 status)
{
#ifdef CONFIG_AMEBAGREEN2
	GPIO_WriteBit(RX_REQ_PIN, status);
#else
	GPIO_WriteBit_Critical(RX_REQ_PIN, status);
#endif
}

void whc_spi_dev_device_init(void);
void whc_spi_dev_event_int_hdl(u8 *rxbuf, struct sk_buff *skb);
void whc_spi_dev_send(struct whc_buf_info *pbuf);
u8 whc_spi_dev_tx_path_avail(void);
void whc_spi_dev_send_to_host(u8 *buf, u8 *buf_alloc, u16 len);
void whc_spi_dev_trigger_rx_handle(void);
void whc_spi_dev_send_data(u8 *buf, u32 len);
void whc_spi_dev_send_cmd_data(u8 *buf, u32 len);

void whc_spi_dev_pkt_rx(u8 *rxbuf, struct sk_buff *skb);

#endif

