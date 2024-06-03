/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef _HCI_TRANSPORTS_H_
#define _HCI_TRANSPORTS_H_

#define HCI_ISO_DATA_PACKET  1

#define H4_HEADER_LEN 1
#define H5_HEADER_LEN 4

#define H4_NONE       0x00
#define H4_CMD        0x01
#define H4_ACL        0x02
#define H4_SCO        0x03
#define H4_EVT        0x04
#define H4_ISO        0x05

typedef struct {
	uint8_t type;       /* HCI packet type. Set by hci driver. */
	uint8_t hdr[4];     /* HCI packet header content. Set by hci driver. */
	uint16_t len;       /* HCI packet content length. Set by hci driver. */

	uint8_t *data;      /* Address to store whole packet content. Set in HCI_GET_BUF handler by stack */
	void *arg;          /* Argument reserved for stack. Set in HCI_GET_BUF handler by stack */
} hci_rx_t;

typedef uint8_t (*HCI_OPEN_CB)(uint8_t status);
typedef uint8_t (*HCI_RECV)(hci_rx_t *rx);
typedef bool (*HCI_GET_BUF)(hci_rx_t *rx, uint32_t timeout);
typedef void (*HCI_FREE_BUF)(hci_rx_t *rx);
typedef uint8_t (*HCI_SEND_CB)(void);
typedef uint8_t (*HCI_RECV_IND)(void);

typedef struct {
	/* Open the HCI proto transport */
	uint8_t (*open)(void);

	/* Close the HCI proto transport */
	uint8_t (*close)(void);

	/* Free the HCI proto transport */
	uint8_t (*free_ops)(void);

	/* Send HCI buffer to bus */
	uint16_t (*send)(uint8_t type, uint8_t *buf, uint16_t len, uint8_t is_reserved);

	/* Set Targte where HCI send to */
	void (*set_recv)(HCI_RECV hci_recv);

	/* Set HCI buffer Operation */
	void (*set_buf_ops)(HCI_GET_BUF get_buf, HCI_FREE_BUF free_buf);

	/* Recv bus buffer to HCI drv */
	uint8_t (*recv_ind)(void);

} HCI_TRANSPORT_OPS;

extern HCI_TRANSPORT_OPS hci_transport_ops;

static inline void hci_transport_set_recv(HCI_RECV hci_recv)
{
	hci_transport_ops.set_recv(hci_recv);
}

static inline void hci_transport_set_buf_ops(HCI_GET_BUF get_buf, HCI_FREE_BUF free_buf)
{
	hci_transport_ops.set_buf_ops(get_buf, free_buf);
}

static inline uint8_t hci_transport_recv_ind(void)
{
	return hci_transport_ops.recv_ind();
}

static inline uint16_t hci_transport_send(uint8_t type, uint8_t *buf, uint16_t len, uint8_t is_reserved)
{
	return hci_transport_ops.send(type, buf, len, is_reserved);
}

static inline uint8_t hci_transport_open(void)
{
	return hci_transport_ops.open();
}

static inline uint8_t hci_transport_close(void)
{
	return hci_transport_ops.close();
}

static inline uint8_t hci_transport_free(void)
{
	return hci_transport_ops.free_ops();
}

/* HCI StandAlone */
bool hci_sa_recv_get_buf(hci_rx_t *info, uint32_t timeout);
uint8_t hci_sa_recv(hci_rx_t *info);
uint8_t hci_sa_send(uint8_t type, uint8_t *buf, uint16_t len, bool is_sync);
#endif