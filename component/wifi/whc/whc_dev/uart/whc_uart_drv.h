#ifndef _WHC_UART_DRV_H_
#define _WHC_UART_DRV_H_

#include "whc_uart_dev.h"

#define WHC_UART_DEV	UART0_DEV
#define UART_BAUD	4000000
#define WHC_UART_RX_BURST_SIZE 1
#define WHC_UART_TX_BURST_SIZE 8

//depends on RxFifoTrigLevel
#define WHC_UART_TX_ALIGN_SIZE 0x10

#define DEV_DMA_ALIGN	4

#define whc_uart_dev_event_int_hdl   whc_uart_dev_pkt_rx

#define whc_dev_trigger_rx_handle   whc_uart_dev_trigger_rx_handle
#define whc_dev_tx_path_avail       whc_uart_dev_tx_path_avail
#define whc_dev_send                whc_uart_dev_send

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

struct whc_uart_priv_t {
	u32 dev_status;

	u8 uart_idx;
	u8 wait_for_skb: 1;
	u8 txdma_initialized: 1;

	rtos_mutex_t tx_lock;
	rtos_mutex_t rx_lock;

	rtos_sema_t rxirq_sema;
	rtos_sema_t txirq_sema;
	rtos_sema_t free_skb_sema;

	GDMA_InitTypeDef UARTTxGdmaInitStruct;
	GDMA_InitTypeDef UARTRxGdmaInitStruct;

	UART_InitTypeDef UART_InitStruct;

	struct sk_buff *rx_skb;
	struct whc_buf_info *txbuf_info;
};


void whc_uart_dev_device_init(void);
u8 whc_uart_dev_tx_path_avail(void);
void whc_uart_dev_send(struct whc_buf_info *pbuf);
void whc_uart_dev_trigger_rx_handle(void);
void whc_uart_dev_pkt_rx(u8 *rxbuf, struct sk_buff *skb);
u8 whc_uart_dev_bus_is_idle(void);
void whc_uart_dev_send_data(u8 *buf, u32 len);

#endif

