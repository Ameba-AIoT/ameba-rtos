#ifndef _WHC_UART_DRV_H_
#define _WHC_UART_DRV_H_

#define WHC_UART_DEV						UART0_DEV
#define UART_BAUD							4000000
#define WHC_UART_RX_BURST_SIZE				16
#define WHC_UART_TX_BURST_SIZE				8

//depends on RxFifoTrigLevel
#define WHC_UART_TX_ALIGN_SIZE				0x10

#define DEV_DMA_ALIGN						4

#if defined (CONFIG_AMEBASMART)
#define UART_TX								_PA_3
#define UART_RX								_PA_2

#elif defined (CONFIG_AMEBALITE)
/* fully programmable zone */
#define UART_TX								_PA_28
#define UART_RX								_PA_29

#elif defined (CONFIG_AMEBADPLUS)
/* fully programmable zone */
#define UART_TX								_PA_26
#define UART_RX								_PA_27

#elif defined (CONFIG_AMEBAGREEN2)
/* fully programmable zone */
#define UART_TX								_PA_25
#define UART_RX								_PA_26
#endif

#define WHC_UART_DEV_RX_IDLE				0x0
#define WHC_UART_DEV_RX_HANDSHAKE			0x1
#define WHC_UART_DEV_RX_HEADER				0x2
#define WHC_UART_DEV_WAIT_PAYLOAD			0x3
#define WHC_UART_DEV_RX_PAYLOAD				0x4

#define UART_DMA_ALIGN(x)					((((x-1)>>5)+1)<<5) //alignement to 32 for cache line
#define UART_BUFSZ							(UART_DMA_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct whc_msg_info)))
/* uart as flow controller when rx */
#define UART_SKB_RSVD_LEN					N_BYTE_ALIGMENT(SKB_WLAN_TX_EXTRA_LEN - sizeof(struct whc_msg_info), WHC_UART_RX_BURST_SIZE)

#define whc_dev_intf_init					whc_uart_dev_init
#define whc_dev_bus_is_idle					whc_uart_dev_bus_is_idle
#define whc_dev_trigger_rx_handle()			whc_uart_dev_trigger_rx_handle()
#define whc_dev_send						whc_uart_dev_send
#define whc_dev_flowctrl(a, b)

enum WHC_UART_HANDSHAKE_TYPE {
	WHC_UART_HDR = 1,
	WHC_UART_ACK,
};

struct whc_uart_hdr {
	u32 checksum;
	u16 buf_size;
	u16 subtype;
};

struct whc_uart_priv_t {
	GDMA_InitTypeDef UARTTxGdmaInitStruct;
	GDMA_InitTypeDef UARTRxGdmaInitStruct;
	UART_InitTypeDef UART_InitStruct;
	struct whc_uart_hdr rx_hdr;

	struct sk_buff *rx_skb;
	struct whc_buf_info *txbuf_info;

	rtos_sema_t tx_lock;
	rtos_sema_t rx_lock;
	rtos_sema_t hdr_reply;
	rtos_sema_t session_lock;
	rtos_sema_t rx_wakeup_sema;
	rtos_sema_t txirq_sema;
	rtos_sema_t free_skb_sema;

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

static inline u32 uart_get_idx(UART_TypeDef *Uartx)
{
	u32 i;

	for (i = 0; i < MAX_UART_INDEX; i++) {
		if (Uartx == UART_DEV_TABLE[i].UARTx) {
			return i;
		}
	}

	return 0xFF;
}

void whc_uart_dev_init(void);
void whc_uart_dev_send(u8 *buf, u16 len, void *buf_alloc, u8 is_skb);
u8 whc_uart_dev_bus_is_idle(void);
void whc_uart_dev_trigger_rx_handle(void);

#endif
