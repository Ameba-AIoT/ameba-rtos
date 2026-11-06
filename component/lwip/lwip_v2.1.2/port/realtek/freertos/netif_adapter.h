#ifndef __NETIF_ADAPTER_H__
#define __NETIF_ADAPTER_H__

#include "lwip/netif.h"

//----- ------------------------------------------------------------------
// Ethernet Buffer
//----- ------------------------------------------------------------------


#define MAX_ETH_DRV_SG    32
#define MAX_ETH_MSG    1540

void netif_adapter_wifi_recv(uint8_t idx, int total_len);
void netif_adapter_wifi_recv_whc(uint8_t idx, struct pbuf *p_buf);
err_t netif_adapter_init(struct netif *netif);
void netif_adapter_usb_eth_recv(u8 *buf, u32 frame_len);
struct pbuf *netif_adapter_eth_buf_copy(u32 frame_len, u8 *src_buf);
void netif_adapter_eth_recv(struct pbuf *p);
#endif
