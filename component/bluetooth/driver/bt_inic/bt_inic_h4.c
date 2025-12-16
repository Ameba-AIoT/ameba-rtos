/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <osif.h>
#include <string.h>
#include <stdbool.h>
#include "ameba_soc.h"
#include "bt_debug.h"

#ifdef CONFIG_SDN
#include "hci_if_inic_sdn.h"
#else
#include "hci_if_inic.h"
#endif

#define CONFIG_INIC_RX_PRIO        5
#define CONFIG_INIC_RX_STACK_SIZE  (2048)

/* note: Selected UART shall not conflict with UART_BT, */
/* e.g. UART3 in AmebaSmart/AmebaLite or UART2 in AmebaDplus */
#if defined(CONFIG_AMEBASMART)
#define UART_INDEX  0
#define UART_TX     _PA_3
#define UART_RX     _PA_2
#define UART_CTS    _PA_4
#define UART_RTS    _PA_5
#elif defined(CONFIG_AMEBALITE)
#define UART_INDEX  0
#define UART_TX     _PA_28
#define UART_RX     _PA_29
#define UART_CTS    _PA_30
#define UART_RTS    _PA_31
#elif defined(CONFIG_AMEBADPLUS)
#define UART_INDEX  0
#define UART_TX     _PA_26
#define UART_RX     _PA_27
#define UART_CTS    _PB_22
#define UART_RTS    _PA_30
#elif defined(CONFIG_AMEBAGREEN2)
#define UART_INDEX  0
#define UART_TX     _PA_4
#define UART_RX     _PA_5
#define UART_CTS    _PA_6
#define UART_RTS    _PA_7
#elif defined(CONFIG_AMEBAL2)
#define UART_INDEX  0
#define UART_TX     _PA_4
#define UART_RX     _PA_5
#define UART_CTS    _PA_6
#define UART_RTS    _PA_7
#elif defined(CONFIG_RTL8720F)
#define UART_INDEX  0
#define UART_TX     _PA_4
#define UART_RX     _PA_5
#define UART_CTS    _PA_6
#define UART_RTS    _PA_7
#endif

#define UART_DEV    UART_DEV_TABLE[UART_INDEX].UARTx
#define UART_IRQ    UART_DEV_TABLE[UART_INDEX].IrqNum
#define UART_IRQ_PRIO   (INT_PRI_LOWEST)
#define UART_IRQ_MASK   (RUART_BIT_ERBI | RUART_BIT_ELSI | RUART_BIT_ETOI)
#define UART_POLLING_TX_MAX 16

/* Receiver states. */
enum {
	ST_IDLE = 0,    /* Waiting for packet type. */
	ST_HDR,         /* Receiving packet header. */
	ST_PAYLOAD,     /* Receiving packet payload. */
	ST_DISCARD,     /* Dropping packet. */
};

/* BT inic states. */
enum {
	H4_CLOSED = 0,      /* BT INIC is closed. */
	H4_RUNNING,     /* BT INIC is running. */
	H4_CLOSING,     /* BT INIC is closing. */
};

/* When memory allocate fail, read data from uart and discard them. */
#define H4_DISCARD_LEN 33

static void *rx_thread_hdl;
static uint8_t h4_state;
static uint32_t tx_busy_num = 0;
static UART_InitTypeDef UART_InitStruct;
static GDMA_InitTypeDef GDMA_InitStruct;
static void *tx_done_sem = NULL;
static void *tx_mutex = NULL;
static void *rx_sem = NULL;

static struct h4 {
	uint8_t rx_state;
	uint16_t remaining;
	uint8_t hdr[HCI_HDR_MAX_SIZE];
	uint8_t hdr_len;
	uint16_t body_len;
	uint8_t *buf;
	uint8_t type;
} h4;

static uint32_t _uart_irq(void *data)
{
	(void)data;
	uint32_t reg_lsr = UART_LineStatusGet(UART_DEV);

	if (UART_Readable(UART_DEV)) {
		/* disable uart interrupt, otherwise uart_rx_thread has no chance to run and fetch rx FIFO */
		UART_INTConfig(UART_DEV, UART_IRQ_MASK, DISABLE);
		osif_sem_give(rx_sem);
	}

	if (reg_lsr & RUART_BIT_TIMEOUT_INT) {
		UART_INT_Clear(UART_DEV, RUART_BIT_TOICF);
	}

	if ((reg_lsr & UART_ALL_RX_ERR)) {
		UART_INT_Clear(UART_DEV, RUART_BIT_RLSICF);
	}

	return 0;
}

static void uart_rx_thread(void *context)
{
	(void)context;

	h4_state = H4_RUNNING;

	while (true) {
		osif_sem_take(rx_sem, BT_TIMEOUT_FOREVER);
		if (h4_state != H4_RUNNING) {
			if (h4.buf) {
				osif_mem_aligned_free(h4.buf);
			}
			break;
		}

		while (UART_Readable(UART_DEV)) {
			switch (h4.rx_state) {
			case ST_IDLE: {
				/* Read H4 Type */
				UART_CharGet(UART_DEV, &h4.type);
				h4.hdr_len = hci_get_hdr_len(h4.type);
				if (h4.hdr_len) {
					h4.remaining = h4.hdr_len;
					h4.rx_state = ST_HDR;
				}
				break;
			}
			case ST_HDR: {
				h4.remaining -= UART_ReceiveDataTO(UART_DEV, h4.hdr + h4.hdr_len - h4.remaining, h4.remaining, 1);
				if (h4.remaining == 0) {
					h4.body_len = hci_get_body_len(h4.hdr, h4.type);
					if (h4.body_len == 0) { /* some cmd has no param */
						bt_inic_recv_from_host(h4.type, h4.hdr, h4.hdr_len);
						h4.rx_state = ST_IDLE;
						break;
					}

					h4.remaining = h4.body_len;
					h4.buf = osif_mem_aligned_alloc(RAM_TYPE_DATA_ON, h4.hdr_len + h4.body_len, 4);
					if (h4.buf) {
						memcpy(h4.buf, h4.hdr, h4.hdr_len);
						h4.rx_state = ST_PAYLOAD;
					} else {
						h4.rx_state = ST_DISCARD;
					}
				}
				break;
			}
			case ST_PAYLOAD: {
				h4.remaining -= UART_ReceiveDataTO(UART_DEV, h4.buf + h4.hdr_len + h4.body_len - h4.remaining, h4.remaining, 1);
				if (h4.remaining == 0) {
					bt_inic_recv_from_host(h4.type, h4.buf, h4.hdr_len + h4.body_len);
					osif_mem_aligned_free(h4.buf);
					h4.rx_state = ST_IDLE;
				}
				break;
			}
			case ST_DISCARD: {
				uint8_t discard[H4_DISCARD_LEN];
				h4.remaining -= UART_ReceiveDataTO(UART_DEV, discard, (h4.remaining > H4_DISCARD_LEN) ? H4_DISCARD_LEN : h4.remaining, 1);
				if (h4.remaining == 0) {
					h4.rx_state = ST_IDLE;
				}
				break;
			}
			default:
				break;
			}
		}
		/* re-enable uart interrupt */
		UART_INTConfig(UART_DEV, UART_IRQ_MASK, ENABLE);
	}
	h4_state = H4_CLOSED;
	osif_task_delete(NULL);
}

static uint32_t uart_dma_cb(void *data)
{
	(void)data;
	GDMA_Cmd(GDMA_InitStruct.GDMA_Index, GDMA_InitStruct.GDMA_ChNum, DISABLE);
	GDMA_ClearINT(GDMA_InitStruct.GDMA_Index, GDMA_InitStruct.GDMA_ChNum);
	GDMA_ChnlFree(GDMA_InitStruct.GDMA_Index, GDMA_InitStruct.GDMA_ChNum);
	UART_TXDMACmd(UART_DEV, DISABLE);
	osif_sem_give(tx_done_sem);

	return 0;
}

void bt_inic_send_to_host(uint8_t type, uint8_t *pdata, uint32_t len)
{
	uint32_t flags = 0;

	flags = osif_lock();
	tx_busy_num++;
	osif_unlock(flags);

	if (h4_state != H4_RUNNING) {
		goto end;
	}

	osif_mutex_take(tx_mutex, BT_TIMEOUT_FOREVER);
	UART_SendData(UART_DEV, &type, 1);
	if (len > UART_POLLING_TX_MAX) {
		UART_TXDMACmd(UART_DEV, ENABLE);
		if (UART_TXGDMA_Init(UART_INDEX, &GDMA_InitStruct, NULL, (IRQ_FUN)uart_dma_cb, pdata, len)) {
			osif_sem_take(tx_done_sem, BT_TIMEOUT_FOREVER);
		} else {
			UART_TXDMACmd(UART_DEV, DISABLE);
			BT_LOGE("UART_TXGDMA_Init fail!\r\n");
			UART_SendData(UART_DEV, pdata, len);
		}
	} else {
		UART_SendData(UART_DEV, pdata, len);
	}
	osif_mutex_give(tx_mutex);

end:
	flags = osif_lock();
	tx_busy_num--;
	osif_unlock(flags);
}

#ifndef CONFIG_AMEBASMART
#define _UART_FUNCID(__pin, __idx)      PINMUX_FUNCTION_UART##__idx##_##__pin
#define UART_FUNCID(__pin, __idx)       _UART_FUNCID(__pin, __idx)
#endif
void bt_inic_h4_init(void)
{
	if (h4_state == H4_RUNNING) {
		return;
	}

	osif_sem_create(&tx_done_sem, 0, 1);
	osif_sem_create(&rx_sem, 0, 1);
	osif_mutex_create(&tx_mutex);

	memset(&h4, 0, sizeof(h4));
	osif_task_create(&rx_thread_hdl, "inic_rx", uart_rx_thread, 0, CONFIG_INIC_RX_STACK_SIZE, CONFIG_INIC_RX_PRIO);

	/* enable uart clock and function */
	RCC_PeriphClockCmd(APBPeriph_UARTx[UART_INDEX], APBPeriph_UARTx_CLOCK[UART_INDEX], ENABLE);

#if defined(CONFIG_AMEBASMART)
	/* Configure UART TX and RX pin */
	Pinmux_Config(UART_TX, PINMUX_FUNCTION_UART);
	Pinmux_Config(UART_RX, PINMUX_FUNCTION_UART);
	Pinmux_Config(UART_CTS, PINMUX_FUNCTION_UART_RTSCTS);
	Pinmux_Config(UART_RTS, PINMUX_FUNCTION_UART_RTSCTS);
#elif defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBAGREEN2)
	/* Configure UART TX and RX pin */
	Pinmux_Config(UART_TX, UART_FUNCID(TXD, UART_INDEX));
	Pinmux_Config(UART_RX, UART_FUNCID(RXD, UART_INDEX));
	Pinmux_Config(UART_CTS, UART_FUNCID(CTS, UART_INDEX));
	Pinmux_Config(UART_RTS, UART_FUNCID(RTS, UART_INDEX));
#elif defined(CONFIG_AMEBAL2)
#endif

	PAD_PullCtrl(UART_TX, GPIO_PuPd_UP); /* pull up Tx/Rx pin */
	PAD_PullCtrl(UART_RX, GPIO_PuPd_UP); /* pull up Tx/Rx pin */

	/* Enable UART
	 * Use Flow Control (When rx FIFO reaches level, RTS will be pulled high)
	 * Use Baudrate 115200 (Default)
	 */
	UART_StructInit(&UART_InitStruct);
	UART_InitStruct.Parity = RUART_PARITY_DISABLE;
#if defined(CONFIG_AMEBASMART)
	UART_InitStruct.RxFifoTrigLevel = UART_RX_FIFOTRIG_LEVEL_32BYTES;
#else
	UART_InitStruct.RxFifoTrigLevel = UART_RX_FIFOTRIG_LEVEL_HALF;
#endif
	UART_InitStruct.FlowControl = ENABLE;
	UART_Init(UART_DEV, &UART_InitStruct);
	UART_SetBaud(UART_DEV, 115200);
	UART_TXDMAConfig(UART_DEV, 8);

	/* Disable and Enable UART Interrupt */
	InterruptDis(UART_IRQ);
	InterruptUnRegister(UART_IRQ);
	InterruptRegister((IRQ_FUN)_uart_irq, UART_IRQ, NULL, UART_IRQ_PRIO);
	InterruptEn(UART_IRQ, UART_IRQ_PRIO);
	UART_INTConfig(UART_DEV, UART_IRQ_MASK, ENABLE);

	/* Enable Uart High Rate Rx Path */
	UART_RxCmd(UART_DEV, ENABLE);
	BT_LOGA("BT UART Init OK!\r\n");
}

void bt_inic_h4_deinit(void)
{
	if (h4_state != H4_RUNNING) {
		return;
	}

	/* Stop Rx, waiting received packet processed */
	h4_state = H4_CLOSING;
	osif_sem_give(rx_sem);
	while (h4_state != H4_CLOSED) {
		osif_delay(5);
	}

	/* Stop Tx, waiting transmitting packet completed */
	while (tx_busy_num) {
		osif_delay(5);
	}

	/* Disable Uart when TX and RX are both idle */
	InterruptDis(UART_IRQ);
	InterruptUnRegister(UART_IRQ);
	UART_DeInit(UART_DEV);

	/* free memory */
	osif_sem_delete(rx_sem);
	rx_sem = NULL;
	osif_sem_delete(tx_done_sem);
	tx_done_sem = NULL;
	osif_mutex_delete(tx_mutex);
	tx_mutex = NULL;
}