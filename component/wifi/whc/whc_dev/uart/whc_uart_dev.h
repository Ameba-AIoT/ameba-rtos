#ifndef _WHC_UART_DRV_H_
#define _WHC_UART_DRV_H_

#define WHC_UART_DEV	UART0_DEV
#define UART_BAUD	4000000
#define WHC_UART_RX_BURST_SIZE 16
#define WHC_UART_TX_BURST_SIZE 8

//depends on RxFifoTrigLevel
#define WHC_UART_TX_ALIGN_SIZE 0x10

#define DEV_DMA_ALIGN	4

#define whc_dev_intf_init                     whc_uart_dev_init
#define whc_dev_bus_is_idle                   whc_uart_dev_bus_is_idle
#define whc_dev_send                          whc_uart_dev_send

#if defined (CONFIG_AMEBASMART)
#define UART_TX		_PA_3 // UART0 TX
#define UART_RX		_PA_2 // UART0 RX

#elif defined (CONFIG_AMEBALITE)
/* fully programmable zone */
#define UART_TX		_PA_28 // UART0 TX
#define UART_RX		_PA_29 // UART0 RX

#elif defined (CONFIG_AMEBADPLUS)
/* fully programmable zone */
#define UART_TX		_PA_26 // UART0 TX
#define UART_RX		_PA_27 // UART0 RX

#elif defined (CONFIG_AMEBAGREEN2)
/* fully programmable zone */
#define UART_TX		_PA_25//_PA_18 // UART0 TX
#define UART_RX		_PA_26 //_PA_19 // UART0 RX
#endif

enum WHC_UART_HDR_TYPE {
	WHC_UART_HDR_NORMAL = 0x0,
	WHC_UART_HDR_ACK_REQ,
	WHC_UART_HDR_ACK_REPLY,

	WHC_UART_HDR_MAX,
};
struct whc_uart_hdr {
	u32 checksum;
	u16 buf_size;
	u16 subtype;
};

struct whc_uart_priv_t {
	struct whc_uart_hdr rx_hdr;
	GDMA_InitTypeDef UARTTxGdmaInitStruct;
	GDMA_InitTypeDef UARTRxGdmaInitStruct;
	UART_InitTypeDef UART_InitStruct;

	struct sk_buff *rx_skb;
	struct whc_buf_info *txbuf_info;

	rtos_sema_t tx_lock;
	rtos_sema_t rx_lock;
	rtos_sema_t hdr_reply;
	rtos_sema_t session_lock;
	rtos_sema_t rxirq_sema;
	rtos_sema_t txirq_sema;

	u32 rx_size_done;
	u32 rx_size_total;
	u32 checksum;
	u32 payload_len;
	u8 uart_idx;
	u8 rx_state;
	u8 tx_waiting_ack: 1;
	u8 wait_for_skb: 1;
	u8 txdma_initialized: 1;
};

#define WHC_UART_DEV_RX_DONE       0x0
#define WHC_UART_DEV_RX_HEADER     0x1
#define WHC_UART_DEV_RX_REQ        0x2
#define WHC_UART_DEV_RX_PAYLOAD    0x3
#define WHC_UART_DEV_RX_DMA_EN     0x4
#define WHC_UART_DEV_HDR_ACK       0x5
#define WHC_UART_DEV_RX_END        0x6

void whc_uart_dev_init(void);
void whc_uart_dev_send(u8 *buf, u16 len, void *buf_alloc, u8 is_skb);
u8 whc_uart_dev_bus_is_idle(void);

#endif
