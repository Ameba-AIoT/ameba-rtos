#ifndef __SDN_CLIENT_H__
#define __SDN_CLIENT_H__

#include "sdn_intf.h"

/* These functions can only be called inside sdn. */
uint8_t *sdn_client_intf_get_tx_buf(uint8_t protocol, uint8_t type, uint16_t len, void **pdata_buf);
uint8_t sdn_client_intf_get_coex_buf(uint8_t type, uint16_t len, void **pbuf, uint8_t **pdata);
void sdn_client_intf_send(void *pdata_buf);
uint8_t sdn_client_intf_get_free_rx_num(uint8_t type, uint8_t sub_type);
uint8_t sdn_client_intf_get_rx_bt_acl_max_num(void);
uint8_t sdn_client_intf_get_rx_bt_acl_max_len(void);
bool sdn_in_mp(void);

/*---------------------------------------------------*/
#endif