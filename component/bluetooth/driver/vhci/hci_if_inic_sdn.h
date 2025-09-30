/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef _HCI_IF_SDN_H_
#define _HCI_IF_SDN_H_

#include <stdbool.h>
#include <stdint.h>
#include "hci_common.h"

#define BT_OP(ogf, ocf)                         ((ocf) | ((ogf) << 10))
#define BT_OGF_VS                               0x3F
#define BT_HCI_CMD_VS_BT_ON                     BT_OP(BT_OGF_VS, 0x0201)
#define BT_HCI_CMD_VS_BT_OFF                    BT_OP(BT_OGF_VS, 0x0202)

bool bt_controller_hci_open(void);
void bt_controller_hci_send(uint8_t type, uint8_t *pbuf, uint16_t len);
void bt_controller_hci_close(void);
bool bt_controller_hci_is_opened(void);

void bt_inic_sdn_hci_recv(void *packet);
uint8_t *bt_inic_sdn_get_buf(void **packet, uint8_t type, void *hdr, uint16_t len, uint32_t timeout);
void bt_inic_send_to_host(uint8_t type, uint8_t *pdata, uint32_t len);
void bt_inic_recv_from_host(uint8_t type, uint8_t *pdata, uint32_t len);

#endif