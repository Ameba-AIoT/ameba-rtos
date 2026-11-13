/**
  ******************************************************************************
  * @file    wifi_intf_drv_to_lwip.h
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef __LWIP_INTF_H__
#define __LWIP_INTF_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include <rtw_skbuff.h>
struct netif;

//----- ------------------------------------------------------------------
// Ethernet Buffer
//----- ------------------------------------------------------------------
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
#include "netif_adapter.h"  // moved to netif_adapter.h by jimmy 12/2/2015
#endif
//----- ------------------------------------------------------------------
// Wlan Interface Provided
//----- ------------------------------------------------------------------
void wifi_if_tx_inc(int idx);
void wifi_if_tx_dec(int idx);
struct sk_buff *wifi_if_get_recv_skb(int idx);
struct sk_buff *wifi_if_alloc_skb(unsigned int total_len);
void wifi_netif_set_info(int idx_wlan, void *dev, unsigned char *dev_addr);
void wifi_if_send_skb(int idx, struct sk_buff *skb);	//struct sk_buff as defined above comment line
void wifi_if_netif_rx(struct sk_buff *skb, void *preorder_ctrl);
void wifi_if_set_wowlan_ipv6_wake(u8 en);
u8 wifi_if_is_netdev_enable(int idx);
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
int wifi_netif_send(int idx, struct eth_drv_sg *sg_list, int sg_len, int total_len, u8 is_special_pkt);
void wifi_netif_recv(int idx, struct eth_drv_sg *sg_list, int sg_len);
#endif

//----- ------------------------------------------------------------------
// Network Interface provided
//----- ------------------------------------------------------------------

unsigned char *wifi_netif_get_hwaddr(int idx_wlan);
int wifi_netif_rx(int idx, unsigned int len);
void wifi_netif_set_up(u8 idx);
void wifi_netif_set_down(u8 idx);
void wifi_netif_link_up(u8 idx);
void wifi_netif_link_down(u8 idx);
int wifi_netif_dhcps_ip_check(u8 gate, u8 ip);
void wifi_netif_get_ip_gw(u8 wowlan_mode, u8 *ip, u8 *gw);

extern unsigned char *wifi_netif_get_ip(int idx);
extern unsigned char *wifi_netif_get_gw(int idx);
extern unsigned char *wifi_netif_get_mask(int idx);

#ifdef	__cplusplus
}
#endif

#endif //#ifndef __LWIP_INTF_H__
