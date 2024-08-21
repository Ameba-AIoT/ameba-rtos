/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef _HCI_TRANSPORTS_H_
#define _HCI_TRANSPORTS_H_

#define HCI_ISO_DATA_PACKET  1


struct hci_transport_cb {
	/** @brief A new packet is comming.
	 *
	 *  This callback notifies the stack of a new packet coming.
	 *  Stack should prepare buffer to store the packet.
	 *  When hci driver receives this packet correctly, it will call recv callback to indicate stack to procecss the packet.
	 *  Otherwise hci driver will call cancel callback to indicate stack to drop this packet.
	 *
	 *  Stack MUST keep buffer valid until recv or cancel is called.
	 *  Packet reception is serially excuted. A get_buf callback is always followed by a recv or cancel callback.
	 *
	 *  @param type Type of the new hci packet.
	 *  @param hdr Header of the new hci packet.
	 *  @param len Length of the new hci packet.
	 *  @param timeout Timeout in milliseconds for getting buffer.
	 *
	 *  @return buffer to store the new hci packet.
	 */
	uint8_t *(*get_buf)(uint8_t type, void *hdr, uint16_t len, uint32_t timeout);

	/** @brief Reception of the new packet is completed.
	 *
	 *  This callback indicates the stack that the new packet is receved correctly.
	 */
	void (*recv)(void);

	/** @brief Reception of the new packet is Interrupted.
	 *
	 *  This callback indicates the stack to drop the packet because the reception is interrupted.
	 */
	void (*cancel)(void);
};

void hci_transport_register(struct hci_transport_cb *cb);
uint16_t hci_transport_send(uint8_t type, uint8_t *buf, uint16_t len, bool has_rsvd_byte);
uint8_t hci_transport_open(void);
void hci_transport_close(void);
void hci_transport_free(void);

extern struct hci_transport_cb hci_sa_cb;
uint8_t hci_sa_send(uint8_t type, uint8_t *buf, uint16_t len, bool is_sync);
#endif