#ifndef __MII_ETHERNETIF_H__
#define __MII_ETHERNETIF_H__

#include "lwip_netconf.h"

enum prehandler_ret {
	UNUPLOAD_TO_LWIP = 0x0,
	UPLOAD_TO_LWIP = 0x1,
};

typedef enum prehandler_ret(*eth_rx_prehandler_t)(struct pbuf **p, u8 *buf, u32 len);

extern rtos_sema_t eth_link_sema;

void eth_init(void);
int eth_send(struct pbuf *p);
void eth_register_rx_patch(eth_rx_prehandler_t pfunc1);

#endif // __MII_ETHERNETIF_H__
