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
#include "bt_inic.h"
#include "hci_if_inic.h"

#if 0
#define H5_LOG  BT_LOGA
#else
#define H5_LOG(...) {}
#endif

#define CONFIG_H5_THREAD_PRIO        5
#define CONFIG_H5_THREAD_STACK_SIZE  (2048)

#define H5_TX_WIN       4
#define H5_ACT_TIMEOUT  250
#define H5_RETX_TIMEOUT 250

/* note: Selected UART shall not conflict with UART_BT, */
/* e.g. UART3 in AmebaSmart/AmebaLite or UART2 in AmebaDplus */
#if defined(CONFIG_AMEBASMART)
#define UART_INDEX  0
#define UART_TX     _PA_3
#define UART_RX     _PA_2
#elif defined(CONFIG_AMEBALITE)
#define UART_INDEX  0
#define UART_TX     _PA_28
#define UART_RX     _PA_29
#elif defined(CONFIG_AMEBADPLUS)
#define UART_INDEX  0
#define UART_TX     _PA_26
#define UART_RX     _PA_27
#elif defined(CONFIG_AMEBAGREEN2)
#define UART_INDEX  0
#define UART_TX     _PA_4
#define UART_RX     _PA_5
#endif

#define UART_DEV    UART_DEV_TABLE[UART_INDEX].UARTx
#define UART_IRQ    UART_DEV_TABLE[UART_INDEX].IrqNum
#define UART_IRQ_PRIO   (INT_PRI_LOWEST)
#define UART_IRQ_MASK   (RUART_BIT_ERBI | RUART_BIT_ELSI | RUART_BIT_ETOI)
#define UART_POLLING_TX_MAX 16

#define H5_LINK_UNINIT  0
#define H5_LINK_INIT    1
#define H5_LINK_ACTIVE  2

/* Receiver states. */
#define ST_START    0   /* Waiting for packet start byte 0xc0. */
#define ST_HDR      1   /* Receiving packet header. */
#define ST_PAYLOAD  2   /* Receiving packet payload. */
#define ST_END      3   /* Waiting for packet end byte 0xc0. */
#define ST_DISCARD  4   /* Dropping packet. */

/* BT inic states. */
#define BT_INIC_CLOSED  0   /* BT INIC is closed. */
#define BT_INIC_RUNNING 1   /* BT INIC is running. */
#define BT_INIC_CLOSING 2   /* BT INIC is closing. */

/* H5 packet type. */
#define HCI_3WIRE_ACK               0x00
#define HCI_3WIRE_LINK              0x0f

#define SLIP_DELIMITER  0xc0
#define SLIP_ESC        0xdb
#define SLIP_ESC_DELIM  0xdc
#define SLIP_ESC_ESC    0xdd

#define H5_RX_ESC   1
#define H5_TX_ACK_PEND  2

#define H5_HDR_SEQ(hdr)     ((hdr)[0] & 0x07)
#define H5_HDR_ACK(hdr)     (((hdr)[0] >> 3) & 0x07)
#define H5_HDR_CRC(hdr)     (((hdr)[0] >> 6) & 0x01)
#define H5_HDR_RELIABLE(hdr)    (((hdr)[0] >> 7) & 0x01)
#define H5_HDR_PKT_TYPE(hdr)    ((hdr)[1] & 0x0f)
#define H5_HDR_LEN(hdr)     ((((hdr)[1] >> 4) & 0x0f) + ((hdr)[2] << 4))

#define H5_SET_SEQ(hdr, seq)    ((hdr)[0] |= (seq))
#define H5_SET_ACK(hdr, ack)    ((hdr)[0] |= (ack) << 3)
#define H5_SET_CRC(hdr, crc)    ((hdr)[0] |= (crc) << 6)
#define H5_SET_RELIABLE(hdr)    ((hdr)[0] |= 1 << 7)
#define H5_SET_TYPE(hdr, type)  ((hdr)[1] |= type)
#define H5_SET_LEN(hdr, len)    (((hdr)[1] |= ((len) & 0x0f) << 4), \
                 ((hdr)[2] |= (len) >> 4))

#define H5_CHECK_SUM(hdr)   ((~(hdr[0] + hdr[1] + hdr[2])) & 0xff)

#define H5_HDR_SIZE     4

static uint8_t sync_req[] = {0x01, 0x7e};
static uint8_t sync_rsp[] = {0x02, 0x7d};
static uint8_t conf_req[] = {0x03, 0xfc};
static uint8_t conf_rsp[3] = {0x04, 0x7b,};

static void *h5_thread_hdl;
static uint8_t bt_inic_state;
static uint32_t tx_busy_num = 0;
static UART_InitTypeDef UART_InitStruct;
static void *h5_sem = NULL;
static void *ack_timer = NULL;
static void *retx_timer = NULL;
static bool ack_timeout_flag = false;
static bool retx_timeout_flag = false;

static struct h5 {
	uint8_t link_state;
	uint8_t rx_state;
	uint16_t remaining;
	uint8_t hdr[H5_HDR_SIZE];
	uint8_t *buf;
	bool last_is_esc;
	uint8_t tx_win;
	uint8_t tx_ack; /* Local expected seq of next received reliable packet */
	uint8_t tx_seq; /* seq of next reliable packet to send */
	uint8_t rx_ack; /* Remote(Host) expected seq of next received reliable packet */
	void *unack_queue;
	void *tx_queue;
	void *rx_queue;
} h5;

struct h5_reliable_tx {
	uint8_t type;
	uint8_t len;
	uint8_t *buf;
};

static bool reliable_packet(uint8_t type)
{
	switch (type) {
	case HCI_CMD:
	case HCI_ACL:
	case HCI_EVT:
	case HCI_ISO:
	case HCI_SCO:
		return true;
	default:
		return false;
	}
}

static void h5_print_header(const uint8_t *hdr, const char *str)
{
	(void)str;
	H5_LOG("%s header: %02x %02x %02x %02x\r\n", str, hdr[0], hdr[1], hdr[2], hdr[3]);
	if (H5_HDR_RELIABLE(hdr)) {
		H5_LOG("%s REL: seq %u ack %u crc %u type %u len %u\r\n", str, H5_HDR_SEQ(hdr),
			   H5_HDR_ACK(hdr), H5_HDR_CRC(hdr), H5_HDR_PKT_TYPE(hdr), H5_HDR_LEN(hdr));
	} else {
		H5_LOG("%s UNREL: ack %u crc %u type %u len %u\r\n", str, H5_HDR_ACK(hdr),
			   H5_HDR_CRC(hdr), H5_HDR_PKT_TYPE(hdr), H5_HDR_LEN(hdr));
	}
}

static int h5_unslip_byte(uint8_t byte, uint8_t *buf)
{
	if (h5.last_is_esc) {
		h5.last_is_esc = false;
		switch (byte) {
		case SLIP_ESC_DELIM:
			*buf = SLIP_DELIMITER;
			break;
		case SLIP_ESC_ESC:
			*buf = SLIP_ESC;
			break;
		default:
			H5_LOG("Invalid escape byte %x\n", byte);
			return -1;
		}
	} else {
		if (byte == SLIP_ESC) {
			h5.last_is_esc = true;
			return 0;
		}
		*buf = byte;
	}

	h5.remaining--;
	return 0;
}

static void h5_reset_rx(void)
{
	if (h5.buf) {
		osif_mem_aligned_free(h5.buf);
		h5.buf = NULL;
	}

	h5.rx_state = ST_START;
}

static uint32_t _uart_irq(void *data)
{
	(void)data;
	uint8_t byte;
	uint32_t reg_lsr = UART_LineStatusGet(UART_DEV);

	if (reg_lsr & RUART_BIT_TIMEOUT_INT) {
		UART_INT_Clear(UART_DEV, RUART_BIT_TOICF);
	}

	if ((reg_lsr & UART_ALL_RX_ERR)) {
		UART_INT_Clear(UART_DEV, RUART_BIT_RLSICF);
	}

	while (UART_Readable(UART_DEV)) {
		UART_CharGet(UART_DEV, &byte);
		switch (h5.rx_state) {
		case ST_START: {
			h5_reset_rx(); /* to avoid h5.buf memory leak when link sync_req received. */
			if (byte == SLIP_DELIMITER) {
				h5.remaining = H5_HDR_SIZE;
				h5.rx_state = ST_HDR;
			}
			break;
		}
		case ST_HDR: {
			/* In a case we confuse ending slip delimiter
			 * with starting one.
			 */
			if (byte == SLIP_DELIMITER) {
				h5.remaining = H5_HDR_SIZE;
				continue;
			}

			if (h5_unslip_byte(byte, &h5.hdr[H5_HDR_SIZE - h5.remaining]) < 0) {
				h5_reset_rx();
				continue;
			}

			if (h5.remaining == 0) {
				if (h5.hdr[3] != H5_CHECK_SUM(h5.hdr)) {
					H5_LOG("checksum error, drop\r\n");
					h5.rx_state = ST_START;
					break;
				}

				h5.remaining = H5_HDR_LEN(h5.hdr);
				h5.buf = osif_mem_aligned_alloc(RAM_TYPE_DATA_ON, h5.remaining + H5_HDR_SIZE, 4);
				memcpy(h5.buf, h5.hdr, H5_HDR_SIZE);
				if (h5.remaining) {
					h5.rx_state = ST_PAYLOAD;
				} else { /* ack packet */
					h5.rx_state = ST_END;
				}
			}
			break;
		}
		case ST_PAYLOAD: {
			if (h5_unslip_byte(byte, h5.buf + H5_HDR_SIZE + H5_HDR_LEN(h5.hdr) - h5.remaining) < 0) {
				h5_reset_rx();
				continue;
			}

			if (h5.remaining == 0) {
				h5.rx_state = ST_END;
			}
			break;
		}
		case ST_END: {
			if (byte != SLIP_DELIMITER) {
				h5_reset_rx();
				continue;
			}
			osif_msg_send(h5.rx_queue, &h5.buf, BT_TIMEOUT_NONE);
			osif_sem_give(h5_sem);
			h5.buf = NULL;
			h5.rx_state = ST_START;
			break;
		}
		default:
			break;
		}
	}

	return 0;
}

static void h5_slip_bytes(uint8_t *bytes, uint16_t len)
{
	uint8_t data;
	int i;

	for (i = 0; i < len; i++) {
		switch (*(bytes + i)) {
		case SLIP_DELIMITER:
			data = SLIP_ESC;
			UART_SendData(UART_DEV, &data, 1);
			data = SLIP_ESC_DELIM;
			UART_SendData(UART_DEV, &data, 1);
			break;
		case SLIP_ESC:
			data = SLIP_ESC;
			UART_SendData(UART_DEV, &data, 1);
			data = SLIP_ESC_ESC;
			UART_SendData(UART_DEV, &data, 1);
			break;
		default:
			UART_SendData(UART_DEV, bytes + i, 1);
			break;
		}
	}
}

static void h5_send(uint8_t *payload, uint8_t type, uint16_t len)
{
	uint8_t hdr[H5_HDR_SIZE];
	uint8_t marker = SLIP_DELIMITER;

	(void)memset(hdr, 0, sizeof(hdr));

	/* Set ACK for outgoing packet and stop delayed work */
	H5_SET_ACK(hdr, h5.tx_ack);
	/* If cancel fails we may ack the same seq number twice, this is OK. */
	osif_timer_stop(&ack_timer);

	if (reliable_packet(type)) {
		H5_SET_RELIABLE(hdr);
		H5_SET_SEQ(hdr, h5.tx_seq);
		h5.tx_seq = (h5.tx_seq + 1) % 8;
	}

	H5_SET_TYPE(hdr, type);
	H5_SET_LEN(hdr, len);

	/* Calculate CRC */
	hdr[3] = H5_CHECK_SUM(hdr);

	h5_print_header(hdr, "TX: <");

	UART_SendData(UART_DEV, &marker, 1);
	h5_slip_bytes(hdr, H5_HDR_SIZE);
	h5_slip_bytes(payload, len);
	UART_SendData(UART_DEV, &marker, 1);
}

static void process_unack(void)
{
	uint8_t next_seq = h5.tx_seq;
	uint32_t num;

	osif_msg_queue_peek(h5.unack_queue, &num);
	if (!num) {
		return;
	}

	H5_LOG("rx_ack %d tx_ack %d tx_seq %d unack_queue_len %d\r\n", h5.rx_ack, h5.tx_ack, h5.tx_seq,
		   num);

	while (num > 0) {
		if (next_seq == h5.rx_ack) {
			/* Next sequence number is the same as last received
			 * ack number
			 */
			break;
		}

		num--;
		/* Similar to (n - 1) % 8 with unsigned conversion */
		next_seq = (next_seq - 1) & 0x07;
	}

	if (num == 0) {
		H5_LOG("Wrong sequence: rx_ack %d tx_seq %d next_seq %d\r\n", h5.rx_ack, h5.tx_seq,
			   next_seq);
		return;
	}

	H5_LOG("Need to remove %u packet from the queue\r\n", num);

	while (num) {
		void *buf = NULL;
		osif_msg_recv(h5.unack_queue, &buf, BT_TIMEOUT_NONE);

		if (!buf) {
			H5_LOG("Unack queue is empty\r\n");
			break;
		}

		osif_mem_aligned_free(buf);
		num--;
	}
}

static void process_link_packet(uint8_t *data, uint16_t len)
{
	if (!memcmp(data, sync_req, sizeof(sync_req))) {
		if (h5.link_state != H5_LINK_INIT) {
			void *buf;

			H5_LOG("H5 sync, reset link\r\n");
			h5.link_state = H5_LINK_INIT;
			h5.rx_state = ST_START;
			h5.rx_ack = 0;
			h5.tx_ack = 0;
			h5.tx_seq = 0;
			while (osif_msg_recv(h5.unack_queue, &buf, BT_TIMEOUT_NONE)) {
				osif_mem_aligned_free(buf);
			}
			while (osif_msg_recv(h5.tx_queue, &buf, BT_TIMEOUT_NONE)) {
				osif_mem_aligned_free(buf);
			}
		}
		h5_send(sync_rsp, HCI_3WIRE_LINK, sizeof(sync_rsp));
	} else if (!memcmp(data, conf_req, 2)) {
		if (h5.link_state == H5_LINK_INIT) {
			if (len > 2) {
				/* Configuration field present */
				h5.tx_win = MIN(h5.tx_win, data[2] & 0x07);
			}

			H5_LOG("Finished H5 configuration, tx_win %d\r\n", h5.tx_win);

			conf_rsp[2] = h5.tx_win;
			h5_send(conf_rsp, HCI_3WIRE_LINK, sizeof(conf_rsp));
			h5.link_state = H5_LINK_ACTIVE;
		}
	} else {
		H5_LOG("Not handled yet %x %x\r\n", data[0], data[1]);
	}
}

static void process_rx(uint8_t *buf)
{
	uint8_t *hdr = buf;
	uint8_t type = H5_HDR_PKT_TYPE(hdr);

	h5_print_header(hdr, "RX: >");
	if (H5_HDR_RELIABLE(hdr)) {
		/* Drop disordered reliable packet. */
		if (H5_HDR_SEQ(hdr) != h5.tx_ack) {
			return;
		}

		/* For reliable packet increment next transmit ack number */
		h5.tx_ack = (h5.tx_ack + 1) % 8;
		H5_LOG("tx_ack update to %u\r\n", h5.tx_ack);
		/* Submit delayed work to ack the packet */
		osif_timer_restart(&ack_timer, H5_ACT_TIMEOUT);
	}

	/* rx_ack should be in every packet */
	h5.rx_ack = H5_HDR_ACK(hdr);
	H5_LOG("rx_ack update to %u\r\n", h5.rx_ack);

	if (type == HCI_3WIRE_LINK) {
		process_link_packet(buf + H5_HDR_SIZE, H5_HDR_LEN(hdr));
	} else if (reliable_packet(type)) {
		if (h5.link_state == H5_LINK_ACTIVE) {
			bt_inic_recv_from_host(type, buf + H5_HDR_SIZE, H5_HDR_LEN(hdr));
		}
	}
}

static void h5_reliable_send(uint8_t *buf)
{
	struct h5_reliable_tx *tx = (struct h5_reliable_tx *)buf;

	h5_send(tx->buf, tx->type, tx->len);
	osif_msg_send(h5.unack_queue, &tx, BT_TIMEOUT_NONE);
	osif_timer_start(&retx_timer);
}

/* tx/rx/unack queue are only processed in this thread. */
static void h5_thread(void *context)
{
	(void)context;
	uint32_t unack_queue_len;
	uint8_t *buf;

	bt_inic_state = BT_INIC_RUNNING;

	while (osif_sem_take(h5_sem, BT_TIMEOUT_FOREVER)) {
		if (bt_inic_state != BT_INIC_RUNNING) {
			break;
		}

		/* handle packets received from host */
		while (osif_msg_recv(h5.rx_queue, &buf, BT_TIMEOUT_NONE)) {
			process_rx(buf);
			process_unack();
			osif_mem_aligned_free(buf);
		}

		if (h5.link_state != H5_LINK_ACTIVE) {
			continue;
		}

		/* ack timeout handler */
		if (ack_timeout_flag) {
			h5_send(NULL, HCI_3WIRE_ACK, 0);
			ack_timeout_flag = false;
		}

		/* retx timeout handler */
		if (retx_timeout_flag) {
			uint8_t *unack[H5_TX_WIN];
			uint8_t idx = 0, num = 0;
			while (osif_msg_recv(h5.unack_queue, &buf, BT_TIMEOUT_NONE)) {
				unack[num++] = buf;
				h5.tx_seq = (h5.tx_seq - 1) & 0x07;
			}
			for (idx = 0; idx < num; idx++) {
				h5_reliable_send(unack[idx]);
			}
			retx_timeout_flag = false;
		}

		/* send reliable packet to host */
		osif_msg_queue_peek(h5.unack_queue, &unack_queue_len);
		while (unack_queue_len < h5.tx_win) {
			if (!osif_msg_recv(h5.tx_queue, &buf, BT_TIMEOUT_NONE)) {
				break;
			}
			h5_reliable_send(buf);
			unack_queue_len++;
		}
	}
	osif_task_delete(NULL);
	bt_inic_state = BT_INIC_CLOSED;
}

void bt_inic_send_to_host(u8 type, u8 *pdata, u32 len)
{
	uint32_t flags = 0;

	flags = osif_lock();
	tx_busy_num++;
	osif_unlock(flags);

	struct h5_reliable_tx *tx = NULL;

	if (bt_inic_state != BT_INIC_RUNNING) {
		goto end;
	}

	tx = (struct h5_reliable_tx *)osif_mem_aligned_alloc(RAM_TYPE_DATA_ON, len + sizeof(struct h5_reliable_tx), 4);
	if (tx) {
		tx->type = type;
		tx->len = len;
		tx->buf = (uint8_t *)tx + sizeof(struct h5_reliable_tx);
		memcpy(tx->buf, pdata, len);
		osif_msg_send(h5.tx_queue, &tx, BT_TIMEOUT_NONE);
		osif_sem_give(h5_sem);
	}

end:
	flags = osif_lock();
	tx_busy_num--;
	osif_unlock(flags);
}

static void ack_timer_hdl(void *arg)
{
	(void)arg;

	ack_timeout_flag = true;
	osif_sem_give(h5_sem);
}

static void retx_timer_hdl(void *arg)
{
	(void)arg;
	uint32_t num;

	osif_msg_queue_peek(h5.unack_queue, &num);
	if (num) {
		H5_LOG("retx: unack queue len %d\r\n", num);
		retx_timeout_flag = true;
		osif_sem_give(h5_sem);
	}
}

#ifndef CONFIG_AMEBASMART
#define _UART_FUNCID(__pin, __idx)      PINMUX_FUNCTION_UART##__idx##_##__pin
#define UART_FUNCID(__pin, __idx)       _UART_FUNCID(__pin, __idx)
#endif
void bt_inic_h5_init(void)
{
	osif_sem_create(&h5_sem, 0, 1);
	osif_timer_create(&ack_timer, "h5_ack", 0, H5_ACT_TIMEOUT, false, ack_timer_hdl);
	osif_timer_create(&retx_timer, "h5_retx", 0, H5_RETX_TIMEOUT, false, retx_timer_hdl);

	memset(&h5, 0, sizeof(h5));
	h5.tx_win = H5_TX_WIN;
	h5.link_state = H5_LINK_UNINIT;
	osif_msg_queue_create(&h5.unack_queue, H5_TX_WIN, sizeof(uint8_t *));
	osif_msg_queue_create(&h5.tx_queue, H5_TX_WIN, sizeof(uint8_t *));
	osif_msg_queue_create(&h5.rx_queue, H5_TX_WIN, sizeof(uint8_t *));
	osif_task_create(&h5_thread_hdl, "h5_trx", h5_thread, 0, CONFIG_H5_THREAD_STACK_SIZE, CONFIG_H5_THREAD_PRIO);

	/* enable uart clock and function */
	RCC_PeriphClockCmd(APBPeriph_UARTx[UART_INDEX], APBPeriph_UARTx_CLOCK[UART_INDEX], ENABLE);

#if defined(CONFIG_AMEBASMART)
	/* Configure UART TX and RX pin */
	Pinmux_Config(UART_TX, PINMUX_FUNCTION_UART);
	Pinmux_Config(UART_RX, PINMUX_FUNCTION_UART);
#elif defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBAGREEN2)
	/* Configure UART TX and RX pin */
	Pinmux_Config(UART_TX, UART_FUNCID(TXD, UART_INDEX));
	Pinmux_Config(UART_RX, UART_FUNCID(RXD, UART_INDEX));
#endif

	PAD_PullCtrl(UART_TX, GPIO_PuPd_UP); /* pull up Tx/Rx pin */
	PAD_PullCtrl(UART_RX, GPIO_PuPd_UP); /* pull up Tx/Rx pin */

	/* Enable UART
	 * Use Flow Control (When rx FIFO reaches level, RTS will be pulled high)
	 * Use Baudrate 115200 (Default)
	 */
	UART_StructInit(&UART_InitStruct);
	UART_InitStruct.Parity = RUART_PARITY_DISABLE;
	UART_InitStruct.FlowControl = DISABLE;
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
}

void bt_inic_h5_deinit(void)
{
	uint8_t *buf;

	/* Stop Rx, waiting received packet processed */
	InterruptDis(UART_IRQ);
	InterruptUnRegister(UART_IRQ);
	bt_inic_state = BT_INIC_CLOSING;
	while (bt_inic_state != BT_INIC_CLOSED) {
		osif_delay(5);
	}

	/* Stop Tx, waiting transmitting packet completed */
	while (tx_busy_num) {
		osif_delay(5);
	}

	/* Disable Uart when TX and RX are both idle */
	UART_DeInit(UART_DEV);

	/* free memory */
	osif_timer_delete(&ack_timer);
	osif_timer_delete(&retx_timer);
	while (osif_msg_recv(h5.unack_queue, &buf, BT_TIMEOUT_NONE)) {
		osif_mem_aligned_free(buf);
	}
	while (osif_msg_recv(h5.tx_queue, &buf, BT_TIMEOUT_NONE)) {
		osif_mem_aligned_free(buf);
	}
	while (osif_msg_recv(h5.rx_queue, &buf, BT_TIMEOUT_NONE)) {
		osif_mem_aligned_free(buf);
	}
	osif_msg_queue_delete(h5.unack_queue);
	osif_msg_queue_delete(h5.tx_queue);
	osif_msg_queue_delete(h5.rx_queue);
	osif_sem_delete(h5_sem);
	h5_sem = NULL;
}