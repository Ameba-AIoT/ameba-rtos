#include <ameba_soc.h>
#include <sdn_intf.h>

#define HCI_H4_HDR_MAX_SIZE    4
#define HCI_H4_BODY_MAX_SIZE   256

#define LE_TO_UINT16(_data, _array)    {            \
        _data = ((uint16_t)(*((uint8_t *)(_array) + 0)) << 0) |        \
                ((uint16_t)(*((uint8_t *)(_array) + 1)) << 8);         \
}

#define HCI_NONE                                          0x00    /* None of the known packet types */
#define HCI_CMD                                           0x01    /* HCI Command packet */
#define HCI_ACL                                           0x02    /* HCI ACL Data packet */
#define HCI_SCO                                           0x03    /* HCI Synchronous Data packet */
#define HCI_EVT                                           0x04    /* HCI Event packet */
#define HCI_ISO                                           0x05

struct hci_acl_hdr {
	uint16_t handle;
	uint16_t len;
} __attribute__((packed));

struct hci_sco_hdr {
	uint16_t handle;
	uint8_t len;
} __attribute__((packed));

struct hci_iso_hdr {
	uint16_t handle;
	uint16_t len;
} __attribute__((packed));

struct hci_cmd_hdr {
	uint16_t opcode;
	uint8_t  param_len;
} __attribute__((packed));

struct hci_evt_hdr {
	uint8_t  evt;
	uint8_t  len;
} __attribute__((packed));

/* Receiver states. */
enum {
	ST_H4_IDLE = 0,    /* Waiting for packet type. */
	ST_H4_HDR,         /* Receiving packet header. */
	ST_H4_PAYLOAD,     /* Receiving packet payload. */
};

static struct h4_rx {
	uint8_t rx_state;
	uint16_t remaining;
	uint8_t buf[HCI_H4_HDR_MAX_SIZE + HCI_H4_BODY_MAX_SIZE];
	uint8_t hdr_len;
	uint16_t body_len;
	uint8_t type;
} *h4;

static UART_InitTypeDef UART_InitStruct;

#define UART_INDEX          0
#define UART_DEV            UART_DEV_TABLE[UART_INDEX].UARTx
#define UART_IRQ_MASK       (RUART_BIT_ERBI | RUART_BIT_ELSI | RUART_BIT_ETOI)
#define UART_POLLING_TX_MAX 16

#define UART_TX     _PA_13
#define UART_RX     _PA_14

#define UART_IRQ            UART_DEV_TABLE[UART_INDEX].IrqNum
#define UART_IRQ_PRIO       (INT_PRI_LOWEST)

#define _UART_FUNCID(__pin, __idx)      PINMUX_FUNCTION_UART##__idx##_##__pin
#define UART_FUNCID(__pin, __idx)       _UART_FUNCID(__pin, __idx)


enum {
	SDN_UART_NONE = 0,
	SDN_UART_LOGUART,
	SDN_UART_UART0,
};

static uint8_t sdn_uart_type = SDN_UART_NONE;

static uint8_t hci_get_hdr_len(uint8_t type)
{
	if (type == HCI_CMD) {
		return sizeof(struct hci_cmd_hdr);
	} else if (type == HCI_EVT) {
		return sizeof(struct hci_evt_hdr);
	} else if (type == HCI_ACL) {
		return sizeof(struct hci_acl_hdr);
	} else if (type == HCI_ISO) {
		return sizeof(struct hci_iso_hdr);
	} else if (type == HCI_SCO) {
		return sizeof(struct hci_sco_hdr);
	}

	return 0;
}

static uint16_t hci_get_body_len(const void *hdr, uint8_t type)
{
	uint16_t len = 0;
	if (type == HCI_CMD) {
		len = ((const struct hci_cmd_hdr *)hdr)->param_len;
	} else if (type == HCI_EVT) {
		len = ((const struct hci_evt_hdr *)hdr)->len;
	} else if (type == HCI_ISO) {
		LE_TO_UINT16(len, &(((const struct hci_iso_hdr *)hdr)->len));
		len &= 0x3FFF;
	} else if (type == HCI_ACL) {
		LE_TO_UINT16(len, &(((const struct hci_acl_hdr *)hdr)->len));
	} else if (type == HCI_SCO) {
		len = ((const struct hci_sco_hdr *)hdr)->len;
	}

	return len;
}

void _uart0_init(IRQ_FUN func)
{
	RCC_PeriphClockCmd(APBPeriph_UARTx[UART_INDEX], APBPeriph_UARTx_CLOCK[UART_INDEX], ENABLE);

	Pinmux_Config(UART_TX, UART_FUNCID(TXD, UART_INDEX));
	Pinmux_Config(UART_RX, UART_FUNCID(RXD, UART_INDEX));

	PAD_PullCtrl(UART_TX, GPIO_PuPd_UP); /* pull up Tx/Rx pin */
	PAD_PullCtrl(UART_RX, GPIO_PuPd_UP); /* pull up Tx/Rx pin */

	UART_StructInit(&UART_InitStruct);
	UART_InitStruct.Parity = RUART_PARITY_DISABLE;
	UART_InitStruct.RxFifoTrigLevel = UART_RX_FIFOTRIG_LEVEL_HALF;

	UART_Init(UART_DEV, &UART_InitStruct);
	UART_SetBaud(UART_DEV, 115200);

	/* Disable and Enable UART Interrupt */
	InterruptDis(UART_IRQ);
	InterruptUnRegister(UART_IRQ);
	InterruptRegister(func, UART_IRQ, (u32)NULL, UART_IRQ_PRIO);
	InterruptEn(UART_IRQ, UART_IRQ_PRIO);
	UART_INTConfig(UART_DEV, UART_IRQ_MASK, ENABLE);

	/* Enable Uart High Rate Rx Path */
	UART_RxCmd(UART_DEV, ENABLE);
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "SDN HCI UART Init OK!\r\n");
}

void _uart0_deinit(void)
{
	InterruptDis(UART_IRQ);
	InterruptUnRegister(UART_IRQ);
	UART_DeInit(UART_DEV);
}

static void _sdn_h4_rx(uint8_t ch)
{
	switch (h4->rx_state) {
	case ST_H4_IDLE: {
		/* Read H4 Type */
		h4->type = ch;
		h4->hdr_len = hci_get_hdr_len(h4->type);
		if (h4->hdr_len) {
			h4->remaining = h4->hdr_len;
			h4->rx_state = ST_H4_HDR;
		}
		break;
	}
	case ST_H4_HDR: {
		h4->buf[h4->hdr_len - h4->remaining] = ch;
		h4->remaining--;
		if (h4->remaining == 0) {
			h4->body_len = hci_get_body_len(h4->buf, h4->type);
			if (h4->body_len == 0) { /* some cmd has no param */
				sdn_h2c(SDN_MSG_PROTOCOL(h4->type), SDN_MSG_TYPE(h4->type), h4->buf, h4->hdr_len);
				h4->rx_state = ST_H4_IDLE;
				break;
			}

			h4->remaining = h4->body_len;
			h4->rx_state = ST_H4_PAYLOAD;
		}
		break;
	}
	case ST_H4_PAYLOAD: {
		h4->buf[h4->hdr_len + h4->body_len - h4->remaining] = ch;
		h4->remaining--;
		if (h4->remaining == 0) {
			sdn_h2c(SDN_MSG_PROTOCOL(h4->type), SDN_MSG_TYPE(h4->type), h4->buf, h4->hdr_len + h4->body_len);
			h4->rx_state = ST_H4_IDLE;
		}
		break;
	}
	default:
		break;
	}
}

// static void _uart0_h4_rx(void);
static uint32_t _uart0_irq(void *data)
{
	uint32_t reg_lsr = UART_LineStatusGet(UART_DEV);
	uint8_t ch;
	(void) data;

	while (UART_Readable(UART_DEV)) {
		UART_CharGet(UART_DEV, &ch);
		_sdn_h4_rx(ch);
	}

	if (reg_lsr & RUART_BIT_TIMEOUT_INT) {
		UART_INT_Clear(UART_DEV, RUART_BIT_TOICF);
	}

	if ((reg_lsr & UART_ALL_RX_ERR)) {
		UART_INT_Clear(UART_DEV, RUART_BIT_RLSICF);
	}
	return 0;
}

bool sdn_uart_is_on(void)
{
	return (sdn_uart_type != SDN_UART_NONE);
}

void sdn_uart_tx(struct sdn_data_buf *pdata_buf)
{
	if (sdn_uart_type == SDN_UART_LOGUART) {
		uint16_t i = 0;
		while (i < pdata_buf->len) {
			while (!LOGUART_Writable());
			// LOGUART_PutChar_RAM(0xFF);
			LOGUART_PutChar_RAM(*((uint8_t *)pdata_buf->pmsg + i));
			i++;
		}
	} else if (sdn_uart_type == SDN_UART_UART0) {
		UART_SendData(UART_DEV, (uint8_t *)pdata_buf->pmsg, pdata_buf->len);
	}
}

static void _h4_init(void)
{
	if (!h4) {
		h4 = rtos_mem_zmalloc(sizeof(struct h4_rx));
	}
}

static void _h4_deinit(void)
{
	if (h4) {
		rtos_mem_free(h4);
		h4 = NULL;
	}
}

static uint8_t check_byte_num = 0;
static uint8_t is_bridge_close_pattern(uint8_t rc)
{
	if (check_byte_num != 4) {
		switch (rc) {
		case 'A':
			check_byte_num = 1;
			break;
		case 'T':
			if (check_byte_num == 1) {
				check_byte_num = 2;
			} else {
				check_byte_num = 0;
			}
			break;
		case 'M':
			if (check_byte_num == 2) {
				check_byte_num = 3;
			} else {
				check_byte_num = 0;
			}
			break;
		case '2':
			if (check_byte_num == 3) {
				check_byte_num = 4;
			} else {
				check_byte_num = 0;
			}
			break;
		default:
			check_byte_num = 0;
			break;
		}
	}

	if (check_byte_num == 4) {
		/* cmd str ends with <CR + LF> in Ameba Trace Tool */
		/* cmd str may end with <CR> or <CR + LF> or <LF> in Tera Term */
		/* stay consistent with KB_ASCII_CR in shell_cmd_chk */
		if (rc == '\r') {
			check_byte_num = 0;
			sdn_bridge_close();
			return TRUE;
		}
	}

	return FALSE;
}

static u32 sdn_bridge_irq(void *data)
{
	(void)data;

	uint8_t rc = 0;
	uint32_t reg_lsr = LOGUART_GetStatus(LOGUART_DEV);

	/* when rx FIFO not empty */
	while (LOGUART_Readable()) {
		rc = LOGUART_GetChar(FALSE);
		if (is_bridge_close_pattern(rc) != TRUE) {
			_sdn_h4_rx(rc);
		} else {
			return 0;
		}
	}

	/* clear timeout interrupt flag */
	if (reg_lsr & LOGUART_BIT_TIMEOUT_INT) {
		LOGUART_INTClear(LOGUART_DEV, LOGUART_BIT_TOICF);
	}

	if (reg_lsr & LOGUART_BIT_RXFIFO_ERR) {
		LOGUART_INTConfig(LOGUART_DEV, LOGUART_BIT_ELSI, DISABLE);
		LOGUART_INTClear(LOGUART_DEV, LOGUART_BIT_RLSICF);
	}
	return 0;
}

void sdn_bridge_open(bool to_loguart)
{
	_h4_init();

	if (to_loguart) {
		sdn_uart_type = SDN_UART_LOGUART;
		LOGUART_WaitTxComplete();
		LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_1, DISABLE);
		LOGUART_SetBaud(LOGUART_DEV, 115200);
		InterruptRegister((IRQ_FUN)sdn_bridge_irq, UART_LOG_IRQ, (uint32_t)NULL, INT_PRI4);
	} else {
		sdn_uart_type = SDN_UART_UART0;
		_uart0_init((IRQ_FUN)_uart0_irq);
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "PA13 is sdn TX, PA14 is sdn RX.\r\n");
	}
}

void sdn_bridge_close(void)
{
	_h4_deinit();

	if (sdn_uart_type == SDN_UART_LOGUART) {
		LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_1, ENABLE);
		LOGUART_SetBaud(LOGUART_DEV, 1500000);
		InterruptRegister((IRQ_FUN) shell_uart_irq_rom, UART_LOG_IRQ, (u32)NULL, INT_PRI_LOWEST);
	} else {
		_uart0_deinit();
	}

	sdn_uart_type = SDN_UART_NONE;
}

bool sdn_uart_is_loguart(void)
{
	return (sdn_uart_type == SDN_UART_LOGUART);
}