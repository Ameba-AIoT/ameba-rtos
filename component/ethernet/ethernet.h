#ifndef __MII_ETHERNETIF_H__
#define __MII_ETHERNETIF_H__

#include "lwip_netconf.h"

#define MII_TX_DESC_CNT		4
#define MII_RX_DESC_CNT		10

#define ETHERNET_IDX       2
#define WLAN_IDX 0

enum prehandler_ret {
	UNUPLOAD_TO_LWIP = 0x0,
	UPLOAD_TO_LWIP = 0x1,
};


enum ext_clk_sel {
	EXT_CLK_50M = 0xb,
	EXT_CLK_25M = 0xc,
};

typedef void (*link_up_down_callback)(int blinkup);
typedef enum prehandler_ret(*eth_rx_prehandler_t)(struct pbuf **p, u8 *buf, u32 len);
void eth_init(void);
int eth_send(struct pbuf *p);

#endif // __MII_ETHERNETIF_H__