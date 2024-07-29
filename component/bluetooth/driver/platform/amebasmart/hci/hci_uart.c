/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <osif.h>
#include <string.h>
#include <stdbool.h>
#include "ameba_soc.h"
#include "hci_uart.h"
#include "bt_debug.h"
#include "hal_platform.h"
#include "hci_platform.h"
#include "hci/hci_common.h"

#define HCI_UART_DEV             (UART3_DEV)
#define HCI_UART_IRQ             (UART3_BT_IRQ)
#define HCI_UART_IRQ_PRIO        (INT_PRI_LOWEST)
#define HCI_UART_TX_FIFO_SIZE    (32)
#define HCI_UART_RX_FIFO_SIZE    (32)
#define HCI_UART_RX_BUF_SIZE     (0x2000)   /* RX buffer size 8K */
#define HCI_UART_RX_ENABLE_SIZE  (512)      /* Only 512 left to read */
#define HCI_UART_RX_DISABLE_SIZE (128)      /* Only 128 left to write */

static struct hci_uart_t {
	/* UART */
	UART_InitTypeDef UART_InitStruct;

	/* UART RX RingBuf */
	uint8_t         *ring;
	uint32_t         ring_size;
	uint32_t         write_ptr;
	uint32_t         read_ptr;
	uint8_t          rx_disabled;

	/* UART RX Indication */
	HCI_RECV_IND     rx_ind;

	/* UART TX */
	uint8_t         *tx_buf;
	uint16_t         tx_len;
	void            *tx_done_sem;
} *g_uart = NULL;

uint8_t hci_uart_set_bdrate(uint32_t baudrate)
{
	UART_SetBaud(HCI_UART_DEV, baudrate);
	BT_LOGA("Set baudrate to %d success!\r\n", (int)baudrate);
	return HCI_SUCCESS;
}

uint8_t hci_uart_set_rx_ind(HCI_RECV_IND rx_ind)
{
	g_uart->rx_ind = rx_ind;
	return HCI_SUCCESS;
}

static inline uint16_t _rx_to_read_space(void)
{
	return (g_uart->write_ptr + g_uart->ring_size - g_uart->read_ptr) % g_uart->ring_size;
}

static inline uint16_t _rx_to_write_space(void)
{
	return (g_uart->read_ptr + g_uart->ring_size - g_uart->write_ptr - 1) % g_uart->ring_size;
}

static inline void transmit_chars(void)
{
	uint16_t max_count = HCI_UART_TX_FIFO_SIZE;

	if (!HCI_BT_KEEP_WAKE) {
		/* acquire host wake bt */
		set_reg_value(0x42008250, BIT13 | BIT14, 3); // enable HOST_WAKE_BT No GPIO | HOST_WAKE_BT
		while (1) {
			if ((HAL_READ32(0x42008254, 0) & (BIT4 | BIT3 | BIT2 | BIT1 | BIT0)) == 4) { // 0x42008254[4:0]
				/* bt active */
				break;
			} else if ((HAL_READ32(0x42008208, 0) & BIT13) == 0) { // 0x42008208[13]
				/* bt power off */
				break;
			}
		}
	}

	while (g_uart->tx_len > 0 && max_count-- > 0) {
		UART_CharPut(HCI_UART_DEV, *(g_uart->tx_buf));
		g_uart->tx_buf++;
		g_uart->tx_len--;
	}

	if (!HCI_BT_KEEP_WAKE) {
		/* release host wake bt */
		set_reg_value(0x42008250, BIT13 | BIT14, 0); // disable HOST_WAKE_BT No GPIO | HOST_WAKE_BT
	}

	if (g_uart->tx_len == 0) {
		UART_INTConfig(HCI_UART_DEV, RUART_BIT_ETBEI, DISABLE);
		if (g_uart->tx_done_sem) {
			osif_sem_give(g_uart->tx_done_sem);
		}
	}
}

static inline void receive_chars(void)
{
	uint8_t ch;
	uint16_t write_len = _rx_to_write_space();
	uint16_t max_count = (write_len > HCI_UART_RX_FIFO_SIZE) ? HCI_UART_RX_FIFO_SIZE : write_len;

	while (UART_Readable(HCI_UART_DEV) && max_count-- > 0) {
		UART_CharGet(HCI_UART_DEV, &ch);
#if defined(ARM_CORE_CA32) && ARM_CORE_CA32
		/* prevent multiple accesss both by core1 and core0 of CA32 */
		osif_lock();
#endif
		g_uart->ring[g_uart->write_ptr++] = ch;
		g_uart->write_ptr %= g_uart->ring_size;
#if defined(ARM_CORE_CA32) && ARM_CORE_CA32
		osif_unlock(0);
#endif
	}

	if (!g_uart->rx_disabled && _rx_to_write_space() < HCI_UART_RX_DISABLE_SIZE) {
		UART_INTConfig(HCI_UART_DEV, RUART_BIT_ERBI | RUART_BIT_ETOI, DISABLE);
		g_uart->rx_disabled = 1;
		BT_LOGA("g_uart rx disable!\r\n");
	}

	if (g_uart->rx_ind) {
		g_uart->rx_ind();
	}
}

static uint32_t _uart_irq(void *data)
{
	(void)data;
	uint32_t reg_lsr = UART_LineStatusGet(HCI_UART_DEV);
	uint32_t reg_ier = HAL_READ32(UART3_REG_BASE, 0x4);

	if (reg_lsr & RUART_BIT_RXND_INT) {
		UART_INT_Clear(HCI_UART_DEV, RUART_BIT_RXNDICF);
	}

	if (reg_lsr & RUART_BIT_MONITOR_DONE_INT) {
		UART_INT_Clear(HCI_UART_DEV, RUART_BIT_MDICF);
	}

	if (reg_lsr & RUART_BIT_MODEM_INT) {
		UART_INT_Clear(HCI_UART_DEV, RUART_BIT_MICF);
	}

	if ((reg_lsr & RUART_BIT_TX_EMPTY) && (reg_ier & RUART_BIT_ETBEI)) {
		transmit_chars();
	}

	if (reg_lsr & RUART_BIT_RXFIFO_INT) {
		receive_chars();
	}

	if (reg_lsr & RUART_BIT_TIMEOUT_INT) {
		UART_INT_Clear(HCI_UART_DEV, RUART_BIT_TOICF);
		receive_chars();
	}

	if ((reg_lsr & UART_ALL_RX_ERR)) {
		UART_INT_Clear(HCI_UART_DEV, RUART_BIT_RLSICF);
	}

	return 0;
}

uint16_t hci_uart_send(uint8_t *buf, uint16_t len)
{
	if (!g_uart) {
		BT_LOGE("g_uart is NULL!\r\n");
		return 0;
	}

	/* UART_SendData() does not work */
	g_uart->tx_buf = buf;
	g_uart->tx_len = len;

	UART_INTConfig(HCI_UART_DEV, RUART_BIT_ETBEI, ENABLE);

	if (g_uart->tx_done_sem) {
		if (osif_sem_take(g_uart->tx_done_sem, 0xFFFFFFFF) == false) {
			BT_LOGE("g_uart->tx_done_sem take fail!\r\n");
			return 0;
		}
	}

	/* Trigger TX Empty Interrrupt, so TX done here */
	return len;
}

uint16_t hci_uart_read(uint8_t *buf, uint16_t len)
{
	uint16_t read_len = _rx_to_read_space();
	read_len = (read_len > len) ? len : read_len;

	if (0 == read_len) {
		return 0;
	}

	if (read_len > g_uart->ring_size - g_uart->read_ptr) {
		read_len = g_uart->ring_size - g_uart->read_ptr;
	}

	memcpy(buf, &g_uart->ring[g_uart->read_ptr], read_len);
#if defined(ARM_CORE_CA32) && ARM_CORE_CA32
	/* prevent multiple accesss both by core1 and core0 of CA32 */
	osif_lock();
#endif
	g_uart->read_ptr += read_len;
	g_uart->read_ptr %= g_uart->ring_size;
#if defined(ARM_CORE_CA32) && ARM_CORE_CA32
	osif_unlock(0);
#endif

	if (g_uart->rx_disabled && _rx_to_read_space() < HCI_UART_RX_ENABLE_SIZE) {
		UART_INTConfig(HCI_UART_DEV, RUART_BIT_ERBI | RUART_BIT_ETOI, ENABLE);
		g_uart->rx_disabled = 0;
		BT_LOGA("g_uart rx enable!\r\n");
	}

	return read_len;
}

uint8_t hci_uart_open(void)
{
	/* Init g_uart */
	if (!g_uart) {
		g_uart = (struct hci_uart_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct hci_uart_t));
		if (!g_uart) {
			BT_LOGE("g_uart is NULL!\r\n");
			return HCI_FAIL;
		}
		memset(g_uart, 0, sizeof(struct hci_uart_t));
	}
	if (!g_uart->ring) {
		g_uart->ring = (uint8_t *)osif_mem_aligned_alloc(RAM_TYPE_DATA_ON, HCI_UART_RX_BUF_SIZE, 4);
		if (!g_uart->ring) {
			BT_LOGE("g_uart->ring is NULL!\r\n");
			return HCI_FAIL;
		}
		memset(g_uart->ring, 0, sizeof(HCI_UART_RX_BUF_SIZE));
	}
	g_uart->ring_size = HCI_UART_RX_BUF_SIZE;
	g_uart->read_ptr = 0;
	g_uart->write_ptr = 0;
	g_uart->rx_disabled = 0;

	if (osif_sem_create(&g_uart->tx_done_sem, 0, 1) == false) {
		BT_LOGE("g_uart->tx_done_sem create fail!\r\n");
		return HCI_FAIL;
	}

	/* Enable Clock */
	RCC_PeriphClockCmd(APBPeriph_UART3, APBPeriph_UART3_CLOCK, ENABLE);

	/* Enable UART
	 * Use Flow Control (When rx FIFO reaches level, RTS will be pulled high)
	 * Use Baudrate 115200 (Default)
	 */
	UART_InitTypeDef *pUARTStruct = &g_uart->UART_InitStruct;
	UART_StructInit(pUARTStruct);
	pUARTStruct->WordLen = RUART_WLS_8BITS;
	pUARTStruct->StopBit = RUART_STOP_BIT_1;
	pUARTStruct->Parity = RUART_PARITY_DISABLE;
	pUARTStruct->ParityType = RUART_EVEN_PARITY;
	pUARTStruct->StickParity = RUART_STICK_PARITY_DISABLE;
	pUARTStruct->RxFifoTrigLevel = UART_RX_FIFOTRIG_LEVEL_16BYTES;
	pUARTStruct->FlowControl = ENABLE;
	UART_Init(HCI_UART_DEV, pUARTStruct);
	UART_SetBaud(HCI_UART_DEV, 115200);

	/* Disable and Enable UART Interrupt */
	InterruptDis(HCI_UART_IRQ);
	InterruptUnRegister(HCI_UART_IRQ);
	InterruptRegister((IRQ_FUN)_uart_irq, HCI_UART_IRQ, NULL, HCI_UART_IRQ_PRIO);
	InterruptEn(HCI_UART_IRQ, HCI_UART_IRQ_PRIO);

	UART_INTConfig(HCI_UART_DEV, RUART_BIT_ETBEI, DISABLE);
	UART_INTConfig(HCI_UART_DEV, RUART_BIT_ERBI | RUART_BIT_ELSI | RUART_BIT_ETOI, ENABLE);

	/* Enable Uart High Rate Rx Path */
	UART_RxCmd(HCI_UART_DEV, ENABLE);

	return HCI_SUCCESS;
}

uint8_t hci_uart_close(void)
{
	if (!g_uart) {
		return HCI_FAIL;
	}

	/* Disable UART Interrupt and UART */
	InterruptDis(HCI_UART_IRQ);
	InterruptUnRegister(HCI_UART_IRQ);
	UART_DeInit(HCI_UART_DEV);

	return HCI_SUCCESS;
}

uint8_t hci_uart_free(void)
{
	if (!g_uart) {
		return HCI_FAIL;
	}

	if (g_uart->tx_done_sem) {
		osif_sem_delete(g_uart->tx_done_sem);
		g_uart->tx_done_sem = NULL;
	}

	/* Deinit UART Ringbuf */
	if (g_uart->ring) {
		osif_mem_aligned_free(g_uart->ring);
	}

	osif_mem_free(g_uart);
	g_uart = NULL;

	return HCI_SUCCESS;
}