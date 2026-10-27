/**
  ******************************************************************************
  * @file    wtn_app_rnat.h
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

#ifndef WTN_APP_RNAT_H
#define WTN_APP_RNAT_H

/* -------------------------------- Includes -------------------------------- */
#define NAT_AP_IP_ADDR0 192
#define NAT_AP_IP_ADDR1 168
#define NAT_AP_IP_ADDR2 43
#define NAT_AP_IP_ADDR3 1

#define NAT_AP_NETMASK_ADDR0 255
#define NAT_AP_NETMASK_ADDR1 255
#define NAT_AP_NETMASK_ADDR2 255
#define NAT_AP_NETMASK_ADDR3 0

#define NAT_AP_GW_ADDR0 192
#define NAT_AP_GW_ADDR1 168
#define NAT_AP_GW_ADDR2 43
#define NAT_AP_GW_ADDR3 1

#define RNAT_ROUTE_PRIO_WAN_STA	102	/* STA as WAN */
#define RNAT_ROUTE_PRIO_LAN		0	/* LAN interfaces - lowest priority */

#define RNAT_ADV_MAX_NEIGHBOR	16
#define RNAT_RELAY_PROFILE_MAX		8
#define RNAT_RELAY_PROFILE_AGE_MS	20000

#define RNAT_SOCKET_TEST_PORT	7654
#define RNAT_SOCKET_TEST_INTERVAL_MS	3000
#define RNAT_SOCKET_TEST_TYPE_REQ	1
#define RNAT_SOCKET_TEST_TYPE_REP	2
#define RNAT_SOCKET_TEST_TCP_PORT	7655
#define RNAT_SOCKET_TEST_TCP_MAX_CONN	RNAT_ADV_MAX_NEIGHBOR

/* single profile IE value limit: mac + seq + own ip + ap_sta_num client ips (ap_sta_num is fixed at runtime and <= 60, so len fits u8) */
#define RNAT_RELAY_IE_MAX	(ETH_ALEN + 2 + 4 + 4 * wifi_user_config.ap_sta_num)
#define RNAT_RELAY_STRIDE	((sizeof(struct rnat_relay_profile) + RNAT_RELAY_IE_MAX + 3) & ~3)
#define RNAT_RELAY_TBL_HDR	8
#define RNAT_RELAY_ENTRY(tbl, i)	((struct rnat_relay_profile *)((tbl) + RNAT_RELAY_TBL_HDR + (i) * RNAT_RELAY_STRIDE))

/* other rnats' profile IEs heard in ADVs, kept one per mac (highest seq) so each is relayed once in our own ADV */
struct rnat_relay_profile {
	u32 ts;		/* last time seq advanced */
	u16 seq;
	u8 len;		/* 0 = free slot */
	u8 ie[];	/* profile IE value: mac(6B), seq(2B LE), N * ip(4B) */
};

/* all app-layer rnat intercom runtime state; relay_tbl/relay_mutex are created in rmesh_rnat_intercom_init() */
struct rmesh_rnat_intercom_priv {
	rtos_task_t adv_task;
	rtos_task_t sock_test_task;
	rtos_task_t sock_test_tcp_task;
	rtos_mutex_t relay_mutex;
	u8 *relay_tbl;	/* [self softap mac, padded to RNAT_RELAY_TBL_HDR][RNAT_RELAY_PROFILE_MAX * RNAT_RELAY_STRIDE entries] */
};

void wtn_rnat_ap_init(u8 enable);
u8 wtn_rnat_search_client_ip(u8 *src_mac);
int wtn_rnat_intercom_start(void);
void wtn_rnat_intercom_set_self_gw(void);
#endif