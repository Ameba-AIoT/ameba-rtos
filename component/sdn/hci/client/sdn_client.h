#ifndef __SDN_CLIENT_H__
#define __SDN_CLIENT_H__

#include "sdn_intf.h"

/* These functions can only be called inside sdn. */
uint8_t *sdn_client_intf_get_bt_buf(uint16_t len, void **pdata_buf, bool discardable);
uint8_t *sdn_client_intf_get_154_buf(uint8_t type, uint16_t len, void **pdata_buf);
uint8_t sdn_client_intf_get_coex_buf(uint8_t type, uint16_t len, void **pbuf, uint8_t **pdata);
void sdn_client_intf_send(void *pdata_buf);
uint8_t sdn_client_intf_get_free_bt_cmd_num(void);
bool sdn_in_mp(void);

/*---------------------------------------------------*/
#endif