#ifndef _WHC_SPI_DRV_H_
#define _WHC_SPI_DRV_H_

#define whc_dev_intf_init              whc_spi_dev_init
#define whc_dev_bus_is_idle            whc_spi_dev_bus_is_idle
#define whc_dev_send                   whc_spi_dev_send

#ifdef CONFIG_AMEBAGREEN2  // need use QFN100
#define PINMUX_FUNCTION_SPIS	    PINMUX_FUNCTION_SPI0
#define DEV_READY_PIN				_PA_18
#define DEV_TX_REQ_PIN				_PA_19
#define SPIS_MOSI                   _PA_30
#define SPIS_MISO                   _PA_31
#define SPIS_SCLK                   _PA_29
#define SPIS_CS                     _PB_0
#define WHC_RECOVER_TIM_IDX			0 //todo: need check

#elif defined (CONFIG_AMEBADPLUS)
#define PINMUX_FUNCTION_SPIS	    PINMUX_FUNCTION_SPI
#define DEV_READY_PIN				_PB_9
#define DEV_TX_REQ_PIN				_PB_8
#define SPIS_MOSI	                _PB_24
#define SPIS_MISO	                _PB_25
#define SPIS_SCLK	                _PB_23
#define SPIS_CS		                _PB_26
#define WHC_RECOVER_TIM_IDX			10

#elif defined (CONFIG_AMEBALITE)
#define PINMUX_FUNCTION_SPIS	    PINMUX_FUNCTION_SPI
#define DEV_READY_PIN				_PB_3   // amebalite use different pin due to _PA_1(num 15, GPIO22 got by command "pinout") will be pull down by raspberry pi
#define DEV_TX_REQ_PIN				_PB_2
#define SPIS_MOSI	                _PA_29
#define SPIS_MISO	                _PA_30
#define SPIS_SCLK	                _PA_28
#define SPIS_CS		                _PA_31
#define WHC_RECOVER_TIM_IDX			10
#endif

#define DEV_READY					1
#define DEV_BUSY					0

#define DEV_TX_REQ					1 // dev request send data to host
#define DEV_TX_IDLE					0

#define WHC_SPI_DEV				    SPI0_DEV
#define SPIS_IRQ                    SPI0_IRQ

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

#define SPI_DMA_EVT_TX_DONE			1U
#define SPI_DMA_EVT_RX_DONE			2U

struct whc_spi_priv_t {
	u32 dev_status;

	rtos_mutex_t tx_lock;
	rtos_queue_t dma_irq_queue;
	rtos_sema_t spi_transfer_done_sema;
	rtos_sema_t free_skb_sema;

	GDMA_InitTypeDef SSITxGdmaInitStruct;
	GDMA_InitTypeDef SSIRxGdmaInitStruct;

	struct sk_buff *rx_skb;
	struct whc_buf_info *txbuf_info;

	u8 tx_req;
	u8 wait_tx;

	u8 txdma_initialized: 1;

};

static inline void set_dev_rdy_pin(u8 status)
{
#ifdef CONFIG_AMEBAGREEN2
	GPIO_WriteBit(DEV_READY_PIN, status);
#else
	GPIO_WriteBit_Critical(DEV_READY_PIN, status);
#endif
}

static inline void set_dev_txreq_pin(u8 status)
{
#ifdef CONFIG_AMEBAGREEN2
	GPIO_WriteBit(DEV_TX_REQ_PIN, status);
#else
	GPIO_WriteBit_Critical(DEV_TX_REQ_PIN, status);
#endif
}

u8 whc_spi_dev_bus_is_idle(void);
void whc_spi_dev_init(void);
void whc_spi_dev_send(u8 *buf, u16 len, void *buf_alloc, u8 is_skb);

#endif

