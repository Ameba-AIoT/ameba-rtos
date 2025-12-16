#ifndef _WHC_UART_HOST_H_
#define _WHC_UART_HOST_H_

#include "whc_uart_dev.h"

#define WHC_UART_DEV	UART0_DEV
#define UART_BAUD	4000000
#define WHC_UART_RX_BURST_SIZE 16
#define WHC_UART_TX_BURST_SIZE 8

#define DEV_DMA_ALIGN	4

#define UART_DMA_ALIGN(x)	((((x-1)>>5)+1)<<5) //alignement to 32 for cache line
#define UART_BUFSZ		(UART_DMA_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct whc_msg_info)))
/* uart as flow controller when rx */
#define UART_SKB_RSVD_LEN	N_BYTE_ALIGMENT(SKB_WLAN_TX_EXTRA_LEN - sizeof(struct whc_msg_info), WHC_UART_RX_BURST_SIZE)

#ifdef UART_RX_DMA
#define UART_RX_RSVD_LEN		0
#else
#define UART_RX_RSVD_LEN		32
#endif

//depends on RxFifoTrigLevel
#define WHC_UART_TX_ALIGN_SIZE	0x10

#define whc_host_send_data       whc_uart_host_send_data
#define whc_host_init            whc_uart_host_init

#if defined (CONFIG_AMEBASMART)
#define UART_TX		_PA_3 // UART0 TX
#define UART_RX		_PA_2 // UART0 RX
#define UART_RTS	_PA_5 // UART0 RTS
#define UART_CTS	_PA_4 // UART0 CTS

#elif defined (CONFIG_AMEBALITE)
/* fully programmable zone */
#define UART_TX		_PA_28 // UART0 TX
#define UART_RX		_PA_29 // UART0 RX
#define UART_RTS	_PA_30 // UART0 RTS
#define UART_CTS	_PA_31 // UART0 CTS

#elif defined (CONFIG_AMEBADPLUS)
/* fully programmable zone */
#define UART_TX		_PA_26 // UART0 TX
#define UART_RX		_PA_27 // UART0 RX
#define UART_RTS	_PA_28 // UART0 RTS
#define UART_CTS	_PA_29 // UART0 CTS

#elif defined (CONFIG_AMEBAGREEN2)
/* fully programmable zone */
#define UART_TX		_PA_18 // UART0 TX
#define UART_RX		_PA_19 // UART0 RX
#define UART_RTS	_PA_25 // UART0 RTS
#define UART_CTS	_PA_26 // UART0 CTS

#endif

struct whc_uart_hdr {
	u16 buf_size;
	u16 rsvd;
	u32 seq;
};
struct whc_uart_host_priv_t {
	u32 dev_status;

	u8 uart_idx;
	u8 wait_for_skb: 1;
	u8 txdma_initialized: 1;
	u8 rx_state;
	u32 rx_size_done;
	u32 rx_size_total;

	rtos_mutex_t tx_lock;
	rtos_mutex_t rx_lock;
	rtos_mutex_t host_send;

	rtos_sema_t rxirq_sema;
	rtos_sema_t txirq_sema;
	rtos_sema_t free_skb_sema;

	GDMA_InitTypeDef UARTTxGdmaInitStruct;
	GDMA_InitTypeDef UARTRxGdmaInitStruct;

	UART_InitTypeDef UART_InitStruct;

	u8 *rx_buf;
	struct whc_buf_info *txbuf_info;
	u8 rx_hdr[sizeof(struct whc_uart_hdr)];
};



#define WHC_UART_HOST_RX_DONE       0x0
#define WHC_UART_HOST_RX_HEADER     0x1
#define WHC_UART_HOST_RX_PAYLOAD    0x2
#define WHC_UART_HOST_RX_END        0x3


void whc_uart_host_send_data(struct whc_buf_info *pbuf);
void whc_uart_host_init(void);

#endif

