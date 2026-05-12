#ifndef __MII_ETHERNETIF_H__
#define __MII_ETHERNETIF_H__

#include "lwip_netconf.h"

enum prehandler_ret {
	UNUPLOAD_TO_LWIP = 0x0,
	UPLOAD_TO_LWIP = 0x1,
};


enum ext_clk_sel {
	EXT_CLK_50M = 0xb,
	EXT_CLK_25M = 0xc,
};

typedef void (*link_up_down_callback)(int link_up);
typedef enum prehandler_ret(*eth_rx_prehandler_t)(struct pbuf **p, u8 *buf, u32 len);
void eth_init(void);
int eth_send(struct pbuf *p);
void eth_register_rx_patch(eth_rx_prehandler_t pfunc1);
void eth_register_link_cb(link_up_down_callback pfunc);

#endif // __MII_ETHERNETIF_H__