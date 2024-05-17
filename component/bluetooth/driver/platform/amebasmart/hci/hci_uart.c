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
#include "hci_dbg.h"
#include "hal_platform.h"
#include "hci_platform.h"

#define HCI_UART_DEV             (UART3_DEV)
#define HCI_UART_IRQ             (UART3_BT_IRQ)
#define HCI_UART_IRQ_PRIO        (INT_PRI_LOWEST)
#define HCI_UART_TX_FIFO_SIZE    (32)
#define HCI_UART_RX_FIFO_SIZE    (32)
#define HCI_UART_RX_BUF_SIZE     (0x2000)   /* RX buffer size 8K */
#define HCI_UART_RX_ENABLE_SIZE  (512)      /* Only 512 left to read */
#define HCI_UART_RX_DISABLE_SIZE (128)      /* Only 128 left to write */

static struct amebasmart_uart_t {
	/* UART */
	UART_InitTypeDef UART_InitStruct;

	/* UART RX RingBuf */
	uint8_t         *ring_buffer;
	uint32_t         ring_buffer_size;
	uint32_t         write_ptr;
	uint32_t         read_ptr;
	uint8_t          rx_disabled;

	/* UART RX Indication */
	HCI_RECV_IND     rx_ind;

	/* UART TX */
	uint8_t         *tx_buf;
	uint16_t         tx_len;
	void            *tx_done_sem;

	/* UART Bridge */
	bool             bridge_flag;
} *amebasmart_uart = NULL;

_WEAK void bt_uart_bridge_putc(uint8_t tx_data)
{
	(void)tx_data;
}

void amebasmart_uart_bridge_open(bool flag)
{
	if (amebasmart_uart) {
		amebasmart_uart->bridge_flag = flag;
	} else {
		HCI_ERR("amebasmart_uart is NULL!");
	}
}

void amebasmart_uart_bridge_to_hci(uint8_t rc)
{
	UART_CharPut(HCI_UART_DEV, rc);
}

void amebasmart_uart_hci_to_bridge(uint8_t rc)
{
	bt_uart_bridge_putc(rc);
}

static uint8_t amebasmart_uart_set_bdrate(uint32_t baudrate)
{
	UART_SetBaud(HCI_UART_DEV, baudrate);
	HCI_INFO("Set baudrate to %d success!", (int)baudrate);
	return HCI_SUCCESS;
}

static uint8_t amebasmart_uart_set_rx_ind(HCI_RECV_IND rx_ind)
{
	amebasmart_uart->rx_ind = rx_ind;
	return HCI_SUCCESS;
}

static inline uint16_t amebasmart_uart_rx_to_read_space(void)
{
	return (amebasmart_uart->write_ptr + amebasmart_uart->ring_buffer_size - amebasmart_uart->read_ptr) % amebasmart_uart->ring_buffer_size;
}

static inline uint16_t amebasmart_uart_rx_to_write_space(void)
{
	return (amebasmart_uart->read_ptr + amebasmart_uart->ring_buffer_size - amebasmart_uart->write_ptr - 1) % amebasmart_uart->ring_buffer_size;
}

#if 0//mask for IAR warning(useless code); unmask it when you want use these functions
static inline uint8_t amebasmart_uart_irq_tx_ready(void)
{
	return (UART_LineStatusGet(HCI_UART_DEV) & RUART_BIT_ETBEI);
}

static inline uint8_t amebasmart_uart_irq_rx_ready(void)
{
	return (UART_LineStatusGet(HCI_UART_DEV) & (RUART_BIT_ERBI | RUART_BIT_ETOI));
}
static inline uint8_t amebasmart_uart_irq_is_pending(void)
{
	return (amebasmart_uart_irq_tx_ready() | amebasmart_uart_irq_rx_ready());
}
#endif

static uint32_t cal_bit_shift(uint32_t Mask)
{
	uint32_t i;
	for (i = 0; i < 31; i++) {
		if (((Mask >> i) & 0x1) == 1) {
			break;
		}
	}
	return (i);
}

static void set_reg_value(uint32_t reg_address, uint32_t Mask, uint32_t val)
{
	uint32_t shift = 0;
	uint32_t data = 0;
	data = HAL_READ32(reg_address, 0);
	shift = cal_bit_shift(Mask);
	data = ((data & (~Mask)) | (val << shift));
	HAL_WRITE32(reg_address, 0, data);
	data = HAL_READ32(reg_address, 0);
}

static inline void transmit_chars(void)
{
	uint16_t max_count = HCI_UART_TX_FIFO_SIZE;

	if (!HCI_BT_KEEP_WAKE) {
		/* acquire host wake up bt */
		uint32_t data;

		set_reg_value(0x42008250, BIT13 | BIT14, 3); // enable HOST_WAKE_BT No GPIO | HOST_WAKE_BT
		while (1) {
			data = HAL_READ32(0x42008254, 0) & 0x1F; // 0x42008254 [0:4]
			if (data == 4) {
				/* bt active */
				break;
			}
		}
	}

	while (amebasmart_uart->tx_len > 0 && max_count-- > 0) {
		UART_CharPut(HCI_UART_DEV, *(amebasmart_uart->tx_buf));
		amebasmart_uart->tx_buf++;
		amebasmart_uart->tx_len--;
	}

	if (!HCI_BT_KEEP_WAKE) {
		/* release host wake up bt */
		set_reg_value(0x42008250, BIT13 | BIT14, 0); // disable HOST_WAKE_BT No GPIO | HOST_WAKE_BT
	}

	if (amebasmart_uart->tx_len == 0) {
		UART_INTConfig(HCI_UART_DEV, RUART_BIT_ETBEI, DISABLE);
		if (amebasmart_uart->tx_done_sem) {
			osif_sem_give(amebasmart_uart->tx_done_sem);
		}
	}
}

static inline void receive_chars(void)
{
	uint8_t ch;
	uint16_t write_len = amebasmart_uart_rx_to_write_space();
	uint16_t max_count = (write_len > HCI_UART_RX_FIFO_SIZE) ? HCI_UART_RX_FIFO_SIZE : write_len;

	if (amebasmart_uart->bridge_flag) {
		while (UART_Readable(HCI_UART_DEV) && max_count-- > 0) {
			UART_CharGet(HCI_UART_DEV, &ch);
			amebasmart_uart_hci_to_bridge(ch);
		}
	} else {
		while (UART_Readable(HCI_UART_DEV) && max_count-- > 0) {
			UART_CharGet(HCI_UART_DEV, &ch);
			amebasmart_uart->ring_buffer[amebasmart_uart->write_ptr++] = ch;
			amebasmart_uart->write_ptr %= amebasmart_uart->ring_buffer_size;
		}

		if (!amebasmart_uart->rx_disabled && amebasmart_uart_rx_to_write_space() < HCI_UART_RX_DISABLE_SIZE) {
			UART_INTConfig(HCI_UART_DEV, RUART_BIT_ERBI | RUART_BIT_ETOI, DISABLE);
			amebasmart_uart->rx_disabled = 1;
			HCI_INFO("amebasmart_uart rx disable!");
		}

		if (amebasmart_uart->rx_ind) {
			amebasmart_uart->rx_ind();
		}
	}
}

static uint32_t amebasmart_uart_irq(void *data)
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

static uint16_t amebasmart_uart_send(uint8_t *buf, uint16_t len)
{
	if (!amebasmart_uart) {
		HCI_ERR("amebasmart_uart is NULL!");
		return 0;
	}

	/* UART_SendData() does not work */
	amebasmart_uart->tx_buf = buf;
	amebasmart_uart->tx_len = len;

	UART_INTConfig(HCI_UART_DEV, RUART_BIT_ETBEI, ENABLE);

	if (amebasmart_uart->tx_done_sem) {
		if (osif_sem_take(amebasmart_uart->tx_done_sem, 0xFFFFFFFF) == false) {
			HCI_ERR("amebasmart_uart->tx_done_sem take fail!");
			return 0;
		}
	}

	/* Trigger TX Empty Interrrupt, so TX done here */
	return len;
}

static uint16_t amebasmart_uart_read(uint8_t *buf, uint16_t len)
{
	uint16_t read_len = amebasmart_uart_rx_to_read_space();
	read_len = (read_len > len) ? len : read_len;

	if (0 == read_len) {
		return 0;
	}

	if (read_len > amebasmart_uart->ring_buffer_size - amebasmart_uart->read_ptr) {
		read_len = amebasmart_uart->ring_buffer_size - amebasmart_uart->read_ptr;
	}

	memcpy(buf, &amebasmart_uart->ring_buffer[amebasmart_uart->read_ptr], read_len);
	amebasmart_uart->read_ptr += read_len;
	amebasmart_uart->read_ptr %= amebasmart_uart->ring_buffer_size;

	if (amebasmart_uart->rx_disabled && amebasmart_uart_rx_to_read_space() < HCI_UART_RX_ENABLE_SIZE) {
		UART_INTConfig(HCI_UART_DEV, RUART_BIT_ERBI | RUART_BIT_ETOI, ENABLE);
		amebasmart_uart->rx_disabled = 0;
		HCI_INFO("amebasmart_uart rx enable!");
	}

	return read_len;
}

static uint8_t amebasmart_uart_open(void)
{
	/* Init amebasmart_uart */
	if (!amebasmart_uart) {
		amebasmart_uart = (struct amebasmart_uart_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct amebasmart_uart_t));
		if (!amebasmart_uart) {
			HCI_ERR("amebasmart_uart is NULL!");
			return HCI_FAIL;
		}
		memset(amebasmart_uart, 0, sizeof(struct amebasmart_uart_t));
	}
	if (!amebasmart_uart->ring_buffer) {
		amebasmart_uart->ring_buffer = (uint8_t *)osif_mem_aligned_alloc(RAM_TYPE_DATA_ON, HCI_UART_RX_BUF_SIZE, 4);
		if (!amebasmart_uart->ring_buffer) {
			HCI_ERR("amebasmart_uart->ring_buffer is NULL!");
			return HCI_FAIL;
		}
		memset(amebasmart_uart->ring_buffer, 0, sizeof(HCI_UART_RX_BUF_SIZE));
	}
	amebasmart_uart->ring_buffer_size = HCI_UART_RX_BUF_SIZE;
	amebasmart_uart->read_ptr = 0;
	amebasmart_uart->write_ptr = 0;
	amebasmart_uart->rx_disabled = 0;

	if (osif_sem_create(&amebasmart_uart->tx_done_sem, 0, 1) == false) {
		HCI_ERR("amebasmart_uart->tx_done_sem create fail!");
		return HCI_FAIL;
	}

	/* Enable Clock */
	RCC_PeriphClockCmd(APBPeriph_UART3, APBPeriph_UART3_CLOCK, ENABLE);

	/* Enable UART
	 * Use Flow Control (When rx FIFO reaches level, RTS will be pulled high)
	 * Use Baudrate 115200 (Default)
	 */
	UART_InitTypeDef *pUARTStruct = &amebasmart_uart->UART_InitStruct;
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
	InterruptRegister((IRQ_FUN)amebasmart_uart_irq, HCI_UART_IRQ, NULL, HCI_UART_IRQ_PRIO);
	InterruptEn(HCI_UART_IRQ, HCI_UART_IRQ_PRIO);

	UART_INTConfig(HCI_UART_DEV, RUART_BIT_ETBEI, DISABLE);
	UART_INTConfig(HCI_UART_DEV, RUART_BIT_ERBI | RUART_BIT_ELSI | RUART_BIT_ETOI, ENABLE);

	/* Enable Uart High Rate Rx Path */
	UART_RxCmd(HCI_UART_DEV, ENABLE);

	return HCI_SUCCESS;
}

static uint8_t amebasmart_uart_close(void)
{
	if (!amebasmart_uart) {
		HCI_ERR("amebasmart_uart is NULL!");
		return HCI_FAIL;
	}

	/* Disable UART Interrupt and UART */
	InterruptDis(HCI_UART_IRQ);
	InterruptUnRegister(HCI_UART_IRQ);
	UART_DeInit(HCI_UART_DEV);

	return HCI_SUCCESS;
}

static uint8_t amebasmart_uart_free(void)
{
	if (!amebasmart_uart) {
		HCI_ERR("amebasmart_uart is NULL!");
		return HCI_FAIL;
	}

	if (amebasmart_uart->tx_done_sem) {
		osif_sem_delete(amebasmart_uart->tx_done_sem);
		amebasmart_uart->tx_done_sem = NULL;
	}

	/* Deinit UART Ringbuf */
	if (amebasmart_uart->ring_buffer) {
		osif_mem_aligned_free(amebasmart_uart->ring_buffer);
	}

	osif_mem_free(amebasmart_uart);
	amebasmart_uart = NULL;

	return HCI_SUCCESS;
}

HCI_UART_OPS hci_uart_ops = {
	.open          = amebasmart_uart_open,
	.close         = amebasmart_uart_close,
	.free_ops      = amebasmart_uart_free,
	.send          = amebasmart_uart_send,
	.read          = amebasmart_uart_read,
	.set_rx_ind    = amebasmart_uart_set_rx_ind,
	.set_bdrate    = amebasmart_uart_set_bdrate,
	.bridge_open   = amebasmart_uart_bridge_open,
	.bridge_to_hci = amebasmart_uart_bridge_to_hci,
};
