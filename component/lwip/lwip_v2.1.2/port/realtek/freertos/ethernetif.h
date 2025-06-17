#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "lwip/netif.h"

//----- ------------------------------------------------------------------
// Ethernet Buffer
//----- ------------------------------------------------------------------


#define MAX_ETH_DRV_SG    32
#define MAX_ETH_MSG    1540

void ethernetif_recv(struct netif *netif, int total_len);
err_t ethernetif_init(struct netif *netif);
err_t ethernetif_mii_init(struct netif *netif);
void ethernetif_mii_recv(u8 *buf, u32 frame_len);
struct pbuf * ethernetif_rmii_buf_copy(u32 frame_len, u8 *src_buf);
void ethernetif_rmii_netif_recv(struct pbuf *p);
#endif
