#ifndef _WHC_UART_HOST_H_
#define _WHC_UART_HOST_H_

#include "whc_uart_dev.h"

#define WHC_UART_DEV				UART0_DEV
#define UART_BAUD					4000000
#define WHC_UART_RX_BURST_SIZE		16
#define WHC_UART_TX_BURST_SIZE		8

#define WHC_UART_RX_TASK_PRIO		7
#define DEV_DMA_ALIGN				4

#define SIZE_RX_DESC				0
#define SIZE_TX_DESC				0

#define UART_DMA_ALIGN(x)			((((x-1)>>5)+1)<<5) //alignement to 32 for cache line
#define UART_BUFSZ					(UART_DMA_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct whc_msg_info)))
/* uart as flow controller when rx */
#define UART_SKB_RSVD_LEN			N_BYTE_ALIGMENT(SKB_WLAN_TX_EXTRA_LEN - sizeof(struct whc_msg_info), WHC_UART_RX_BURST_SIZE)

#ifdef UART_RX_DMA
#define UART_RX_RSVD_LEN			0
#else
#define UART_RX_RSVD_LEN			32
#endif

//depends on RxFifoTrigLevel
#define WHC_UART_TX_ALIGN_SIZE		0x10

#define TX_BUF_NUM					2

#if defined (CONFIG_AMEBASMART)
#define UART_TX						_PA_3 // UART0 TX
#define UART_RX						_PA_2 // UART0 RX
#elif defined (CONFIG_AMEBALITE)
/* fully programmable zone */
#define UART_TX						_PA_28 // UART0 TX
#define UART_RX						_PA_29 // UART0 RX
#elif defined (CONFIG_AMEBADPLUS)
/* fully programmable zone */
#define UART_TX						_PA_26 // UART0 TX
#define UART_RX						_PA_27 // UART0 RX
#elif defined (CONFIG_AMEBAGREEN2)
/* fully programmable zone */
#define UART_TX						_PA_25//_PA_18 // UART0 TX
#define UART_RX						_PA_26//_PA_19 // UART0 RX
#endif

#define WHC_UART_HOST_RX_DONE       0x0
#define WHC_UART_HOST_RX_HEADER     0x1
#define WHC_UART_HOST_RX_REQ        0x2
#define WHC_UART_HOST_RX_PAYLOAD    0x3
#define WHC_UART_HOST_HDR_ACK       0x4
#define WHC_UART_HOST_RX_END        0x5
#define WHC_UART_HOST_RX_DMA_EN     0x6

#define whc_host_send_data			whc_uart_host_send
#define whc_host_init				whc_uart_host_init

struct whc_uart_host_priv_t {
	struct whc_uart_hdr rx_hdr;
	GDMA_InitTypeDef UARTTxGdmaInitStruct;
	GDMA_InitTypeDef UARTRxGdmaInitStruct;
	UART_InitTypeDef UART_InitStruct;

	rtos_mutex_t host_send;
	rtos_sema_t tx_lock;
	rtos_sema_t hdr_reply;
	rtos_sema_t session_lock;
	rtos_sema_t rxirq_sema;
	rtos_sema_t txirq_sema;

	struct whc_buf_info *txbuf_info;
	u8 *rx_buf;

	u32 rx_size_done;
	u32 rx_size_total;
	u32 payload_len;
	u32 checksum;
	u8 uart_idx;
	u8 rx_state;
	u8 used_buf_num;
	u8 tx_waiting_ack: 1;
	u8 wait_for_skb: 1;
	u8 txdma_initialized: 1;

	u8 tx_buf[TX_BUF_NUM][32 + UART_BUFSZ] __attribute__((aligned(4)));
};

void whc_uart_host_send(u8 *buf, u16 len, void *buf_alloc, u8 is_skb);
void whc_uart_host_init(void);

#endif
