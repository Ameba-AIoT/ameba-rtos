#ifndef __SDN_HOST_H__
#define __SDN_HOST_H__

#include "sdn_intf.h"

/* These APIs must NOT be called in interrupt or critical context. */
uint32_t sdn_host_enable(uint8_t protocol);
uint32_t sdn_host_send(uint8_t protocol, uint8_t type, uint8_t *pdata, uint16_t len);
void sdn_host_disable(uint8_t protocol);
bool sdn_host_is_enabled(void);
void sdn_host_set_mp(bool is_mp);
/*******************************************************************/
#endif