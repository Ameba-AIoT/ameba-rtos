/**
  ******************************************************************************
  * @file    wtn_app_rnat_intercom.c
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

/* -------------------------------- Includes -------------------------------- */
#include "rtw_autoconf.h"

#if defined(CONFIG_RNAT_EN) && defined(CONFIG_WTN_RNAT_INTERCOM)
#include "wifi_api.h"
#include "lwip_netconf.h"
#include "wtn_app_rnat.h"
#include "wifi_api_rmesh_rnat.h"
#include "wifi_intf_drv_to_app_internal.h"
#include <lwip/sockets.h>

//#define CONFIG_WTN_RNAT_INTERCOM_TEST

static struct rmesh_rnat_intercom_priv *g_rnat_intercom;

static void rnat_relay_profile_update(const u8 *ie, u16 ie_len)
{
	struct rnat_relay_profile *e = NULL, *old = NULL;
	u16 seq = ie[ETH_ALEN] | (ie[ETH_ALEN + 1] << 8);
	u32 i, age, old_age = 0;

	struct rmesh_rnat_intercom_priv *priv = g_rnat_intercom;

	if (!priv || ie_len < ETH_ALEN + 2 + 4 || ie_len > RNAT_RELAY_IE_MAX) {
		return;
	}

	rtos_mutex_take(priv->relay_mutex, MUTEX_WAIT_TIMEOUT);
	if (!memcmp(ie, priv->relay_tbl, ETH_ALEN)) {
		goto exit;
	}
	for (i = 0; i < RNAT_RELAY_PROFILE_MAX; i++) {
		struct rnat_relay_profile *cur = RNAT_RELAY_ENTRY(priv->relay_tbl, i);
		age = cur->len ? RTOS_TIME_GET_PASSING_TIME_MS(cur->ts) : 0xFFFFFFFF;
		if (cur->len && !memcmp(cur->ie, ie, ETH_ALEN)) {
			/* same node: only a newer seq counts; an aged entry is stale, accept anything (node may have rebooted) */
			if (age <= RNAT_RELAY_PROFILE_AGE_MS && (s16)(seq - cur->seq) <= 0) {
				goto exit;
			}
			e = cur;
			break;
		}
		if (age >= old_age) {
			old = cur;
			old_age = age;
		}
	}
	if (!e) {
		e = old;
	}
	e->ts = rtos_time_get_current_system_time_ms();
	e->seq = seq;
	e->len = (u8)ie_len;
	memcpy(e->ie, ie, ie_len);
exit:
	rtos_mutex_give(priv->relay_mutex);
}

/* buf is the whole frame (802.11 hdr + RMESH_RNAT_ADV_PATTERN + IEs), already pattern-checked by driver; read-only */
static int rnat_adv_rx_callback(u8 *buf, u16 len)
{
	u16 hdrlen = (buf[0] & 0x80) ? 26 : 24;	/* QoS data bit, same as the driver's hdrlen choice */
	u16 off = RMESH_RNAT_ADV_IE_OFFSET(hdrlen);

	while (off + 3 <= len) {
		u16 ie_len = buf[off + 1] | (buf[off + 2] << 8);
		if (off + 3 + ie_len > len) {
			break;
		}
		if (buf[off] == RMESH_RNAT_IE_ID_RNAT_PROFILE) {
			rnat_relay_profile_update(buf + off + 3, ie_len);
		}
		off += 3 + ie_len;
	}
	return RTK_SUCCESS;
}

static int rmesh_rnat_intercom_init(void)
{
	struct rmesh_rnat_intercom_priv *priv;
	struct rtw_mac self_mac = {0};

	priv = rtos_mem_zmalloc(sizeof(struct rmesh_rnat_intercom_priv));
	if (!priv) {
		return RTK_FAIL;
	}
	priv->relay_tbl = rtos_mem_zmalloc(RNAT_RELAY_TBL_HDR + RNAT_RELAY_STRIDE * RNAT_RELAY_PROFILE_MAX);
	if (!priv->relay_tbl) {
		rtos_mem_free(priv);
		return RTK_FAIL;
	}
	wifi_get_mac_address(SOFTAP_WLAN_INDEX, &self_mac, 0);
	memcpy(priv->relay_tbl, self_mac.octet, ETH_ALEN);
	rtos_mutex_create(&priv->relay_mutex);
	g_rnat_intercom = priv;
	wifi_rmesh_rnat_register_adv_callback(rnat_adv_rx_callback);
	return RTK_SUCCESS;
}

static void rmesh_rnat_intercom_deinit(void)
{
	struct rmesh_rnat_intercom_priv *priv = g_rnat_intercom;

	wifi_rmesh_rnat_register_adv_callback(NULL);
	g_rnat_intercom = NULL;
	rtos_mutex_take(priv->relay_mutex, MUTEX_WAIT_TIMEOUT);
	rtos_mutex_give(priv->relay_mutex);
	rtos_mutex_delete(priv->relay_mutex);
	rtos_mem_free(priv->relay_tbl);
	rtos_mem_free(priv);
}

static void rnat_adv_send_thread(void *param)
{
	struct rmesh_rnat_intercom_priv *priv = param;

	/* wlan hdr A3 len + qos control, then adv pattern + neighbor info ie + self rnat profile ie + relayed profile ies */
	u32 pframe_size = 26 + RMESH_RNAT_ADV_PATTERN_LEN + 3 + sizeof(struct wtn_rnat_neighbor) * RNAT_ADV_MAX_NEIGHBOR
					  + (1 + RNAT_RELAY_PROFILE_MAX) * (3 + RNAT_RELAY_IE_MAX);
	u8 *pframe = rtos_mem_zmalloc(pframe_size);
	struct wtn_rnat_neighbor neighbors[RNAT_ADV_MAX_NEIGHBOR];
	u16 seq = 0;
	u8 bcmc[ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	struct rtw_raw_frame_desc desc = {0};
	struct rtw_mac softap_mac = {0};

	while (wifi_is_running(SOFTAP_WLAN_INDEX) == FALSE) {
		rtos_time_delay_ms(500);
	}

	wifi_get_mac_address(SOFTAP_WLAN_INDEX, &softap_mac, 0);

	while (pframe && wifi_is_running(SOFTAP_WLAN_INDEX) == TRUE) {
		u16 pkt_len = 26;
		u32 k;

		memset(pframe, 0, pframe_size);
		pframe[0] = 0x88; /*qos data*/
		pframe[1] = 0x01; /*tods=1*/
		memcpy(pframe + 4, bcmc, ETH_ALEN); /*A1=BCMC*/
		memcpy(pframe + 10, softap_mac.octet, ETH_ALEN); /*A2=softap mac*/
		memcpy(pframe + 16, softap_mac.octet, ETH_ALEN); /*A3=softap mac*/

		u8 neighbor_count = wifi_rmesh_rnat_get_neighbor_info(neighbors, RNAT_ADV_MAX_NEIGHBOR);
		u16 neighbor_ie_len = sizeof(struct wtn_rnat_neighbor) * neighbor_count;
		u8 *own_ip = lwip_get_ip(NETIF_WLAN_AP_INDEX);
		u8 *p = pframe + pkt_len;

		memcpy(p, RMESH_RNAT_ADV_PATTERN, RMESH_RNAT_ADV_PATTERN_LEN);
		p += RMESH_RNAT_ADV_PATTERN_LEN;
		pkt_len += RMESH_RNAT_ADV_PATTERN_LEN;

		p[0] = RMESH_RNAT_IE_ID_NEIGHBOR_INFO;
		p[1] = (u8)neighbor_ie_len;
		p[2] = (u8)(neighbor_ie_len >> 8);
		memcpy(p + 3, neighbors, neighbor_ie_len);
		p += 3 + neighbor_ie_len;
		pkt_len += 3 + neighbor_ie_len;

		struct rtw_client_list client_info = {0};
		u8 *ip_list = p + 3 + ETH_ALEN + 2;
		u8 ip_count = 1;
		memcpy(ip_list, own_ip, 4);
		wifi_ap_get_connected_clients(&client_info);
		for (k = 0; k < client_info.count && ip_count < 1 + wifi_user_config.ap_sta_num; k++) {
			u8 host_byte = wtn_rnat_search_client_ip(client_info.mac_list[k].octet);
			wifi_rmesh_rnat_set_client_ip(client_info.mac_list[k].octet, host_byte);
			if (host_byte == 0) {
				continue;
			}
			memcpy(ip_list + 4 * ip_count, own_ip, 3);
			ip_list[4 * ip_count + 3] = host_byte;
			ip_count++;
		}

		p[0] = RMESH_RNAT_IE_ID_RNAT_PROFILE;
		p[1] = (u8)(ETH_ALEN + 2 + 4 * ip_count);
		p[2] = (u8)((ETH_ALEN + 2 + 4 * ip_count) >> 8);
		memcpy(p + 3, softap_mac.octet, ETH_ALEN);
		p[3 + ETH_ALEN] = (u8)seq;
		p[3 + ETH_ALEN + 1] = (u8)(seq >> 8);
		p += 3 + ETH_ALEN + 2 + 4 * ip_count;
		pkt_len += 3 + ETH_ALEN + 2 + 4 * ip_count;
		seq++;

		rtos_mutex_take(priv->relay_mutex, MUTEX_WAIT_TIMEOUT);
		for (k = 0; k < RNAT_RELAY_PROFILE_MAX; k++) {
			struct rnat_relay_profile *e = RNAT_RELAY_ENTRY(priv->relay_tbl, k);
			if (e->len == 0) {
				continue;
			}
			if (RTOS_TIME_GET_PASSING_TIME_MS(e->ts) > RNAT_RELAY_PROFILE_AGE_MS) {
				e->len = 0;
				continue;
			}
			p[0] = RMESH_RNAT_IE_ID_RNAT_PROFILE;
			p[1] = e->len;
			p[2] = 0;
			memcpy(p + 3, e->ie, e->len);
			p += 3 + e->len;
			pkt_len += 3 + e->len;
		}
		rtos_mutex_give(priv->relay_mutex);

		desc.wlan_idx = SOFTAP_WLAN_INDEX;
		desc.buf = pframe;
		desc.buf_len = pkt_len;
		wifi_send_raw_frame(&desc);

		rtos_time_delay_ms(2000);
	}
	rtos_mem_free(pframe);
	rmesh_rnat_intercom_deinit();
	rtos_task_delete(NULL);
}

#ifdef CONFIG_WTN_RNAT_INTERCOM_TEST
/* auto UDP echo test between rnat nodes' softap gw addrs (10.200.<gw_byte3>.1), to tell whether
 * socket-level tx/rx between two rnat nodes works independently of icmp
 */
static void rnat_socket_udp_test_thread(void *param)
{
	(void) param;
	int fd;
	int opt = 1;
	struct sockaddr_in addr, dest_addr, from_addr;
	socklen_t fromlen;
	struct ifreq ap_ifr;
	struct rtw_mac ap_mac = {0};
	ip4_addr_t ap_gw;
	struct eth_addr ap_gw_eth;
	u8 pkt[8];
	u16 seq = 0;
	u8 *ap_ip;
	struct wtn_rnat_neighbor neighbors[RNAT_ADV_MAX_NEIGHBOR];
	u8 neighbor_count, i;
	fd_set rdset;
	struct timeval timeout;
	int ret, recv_len;
	u32 last_tx_ms;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket test create fail\n");
		goto exit;
	}
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(RNAT_SOCKET_TEST_PORT);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket test bind fail\n");
		closesocket(fd);
		goto exit;
	}

	/* bind to AP/P1 netif (instead of bind()-ing local ip) so src ip is auto-filled as
	 * the AP's own mesh gw and udp_sendto_if()'s local_ip-vs-netif check (see doc) is never hit
	 */
	memset(&ap_ifr, 0, sizeof(ap_ifr));
	netif_index_to_name(netif_get_index(pnetif_ap), ap_ifr.ifr_name);
	if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, &ap_ifr, sizeof(ap_ifr)) < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket test bind netif fail\n");
		closesocket(fd);
		goto exit;
	}

	do {
		ap_ip = lwip_get_ip(NETIF_WLAN_AP_INDEX);
		if (ap_ip[0] != 0) {
			break;
		}
		rtos_time_delay_ms(500);
	} while (1);

	/* AP's gw is self-referential (== its own ip); give it a static ARP entry pointing at
	 * our own P1 mac so etharp_output() never has to actually resolve it (see doc)
	 */
	wifi_get_mac_address(SOFTAP_WLAN_INDEX, &ap_mac, 0);
	IP4_ADDR(&ap_gw, ap_ip[0], ap_ip[1], ap_ip[2], ap_ip[3]);
	memcpy(ap_gw_eth.addr, ap_mac.octet, ETH_ALEN);
	etharp_add_static_entry(&ap_gw, &ap_gw_eth);

	last_tx_ms = rtos_time_get_current_system_time_ms() - RNAT_SOCKET_TEST_INTERVAL_MS;

	while (1) {
		if (RTOS_TIME_GET_PASSING_TIME_MS(last_tx_ms) >= RNAT_SOCKET_TEST_INTERVAL_MS) {
			u8 own_byte3;

			last_tx_ms = rtos_time_get_current_system_time_ms();
			ap_ip = lwip_get_ip(NETIF_WLAN_AP_INDEX);
			own_byte3 = ap_ip[2];

			neighbor_count = wifi_rmesh_rnat_get_neighbor_info(neighbors, RNAT_ADV_MAX_NEIGHBOR);
			for (i = 0; i < neighbor_count; i++) {
				if (neighbors[i].gw_byte3 == 0 || neighbors[i].gw_byte3 == own_byte3) {
					continue;
				}
				memset(&dest_addr, 0, sizeof(dest_addr));
				dest_addr.sin_family = AF_INET;
				dest_addr.sin_port = htons(RNAT_SOCKET_TEST_PORT);
				dest_addr.sin_addr.s_addr = ((u32)1 << 24) | ((u32)neighbors[i].gw_byte3 << 16) | (200u << 8) | 10u;

				pkt[0] = 'R';
				pkt[1] = 'N';
				pkt[2] = RNAT_SOCKET_TEST_TYPE_REQ;
				pkt[3] = (u8)seq;
				pkt[4] = (u8)(seq >> 8);
				seq++;

				if (sendto(fd, pkt, 5, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
					RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket udp test tx fail gw=%d\n", neighbors[i].gw_byte3);
				} else {
					RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket udp test tx req seq=%d gw=%d\n", seq - 1, neighbors[i].gw_byte3);
				}
			}
		}

		FD_ZERO(&rdset);
		FD_SET(fd, &rdset);
		timeout.tv_sec = 0;
		timeout.tv_usec = 200000;

		ret = lwip_select(fd + 1, &rdset, NULL, NULL, &timeout);
		if (ret <= 0) {
			continue;
		}

		fromlen = sizeof(from_addr);
		recv_len = lwip_recvfrom(fd, pkt, sizeof(pkt), 0, (struct sockaddr *)&from_addr, &fromlen);
		if (recv_len < 5 || pkt[0] != 'R' || pkt[1] != 'N') {
			continue;
		}

		if (pkt[2] == RNAT_SOCKET_TEST_TYPE_REQ) {
			pkt[2] = RNAT_SOCKET_TEST_TYPE_REP;
			sendto(fd, pkt, 5, 0, (struct sockaddr *)&from_addr, fromlen);
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket udp test rx req seq=%d, replied\n", (u16)(pkt[3] | (pkt[4] << 8)));
		} else if (pkt[2] == RNAT_SOCKET_TEST_TYPE_REP) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket udp test rx reply seq=%d, link OK\n", (u16)(pkt[3] | (pkt[4] << 8)));
		}
	}

exit:
	if (g_rnat_intercom) {
		g_rnat_intercom->sock_test_task = NULL;
	}
	rtos_task_delete(NULL);
}

/* auto TCP echo test between rnat nodes' softap gw addrs (10.200.<gw_byte3>.1). Only the side
 * with the smaller gw_byte3 actively connects, so both ends don't race to connect at once; the
 * listening socket is bound to the AP/P1 netif before listen() so every accept()ed connection
 * inherits netif_idx automatically (see doc), needing no extra per-connection setup
 */
static void rnat_socket_test_tcp_thread(void *param)
{
	(void) param;
	int lfd;
	int opt = 1;
	struct sockaddr_in addr, from_addr;
	socklen_t fromlen;
	struct ifreq ap_ifr;
	struct rtw_mac ap_mac = {0};
	ip4_addr_t ap_gw;
	struct eth_addr ap_gw_eth;
	u8 pkt[8];
	u16 seq = 0;
	u8 *ap_ip;
	u8 own_byte3;
	struct wtn_rnat_neighbor neighbors[RNAT_ADV_MAX_NEIGHBOR];
	u8 neighbor_count, n, i, slot;
	int conn_fd[RNAT_SOCKET_TEST_TCP_MAX_CONN];
	u8 conn_gw[RNAT_SOCKET_TEST_TCP_MAX_CONN];
	u32 conn_last_ms[RNAT_SOCKET_TEST_TCP_MAX_CONN];
	fd_set rdset;
	struct timeval timeout;
	int ret, recv_len, maxfd;

	for (i = 0; i < RNAT_SOCKET_TEST_TCP_MAX_CONN; i++) {
		conn_fd[i] = -1;
		conn_gw[i] = 0;
	}

	lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (lfd < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket test tcp create fail\n");
		goto exit;
	}
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(RNAT_SOCKET_TEST_TCP_PORT);
	if (bind(lfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket test tcp bind fail\n");
		closesocket(lfd);
		goto exit;
	}

	/* bind listen socket to AP/P1 netif before listen(); accept()ed conns inherit netif_idx
	 * (tcp_in.c: npcb->netif_idx = pcb->netif_idx), see doc
	 */
	memset(&ap_ifr, 0, sizeof(ap_ifr));
	netif_index_to_name(netif_get_index(pnetif_ap), ap_ifr.ifr_name);
	if (setsockopt(lfd, SOL_SOCKET, SO_BINDTODEVICE, &ap_ifr, sizeof(ap_ifr)) < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket test tcp bind netif fail\n");
		closesocket(lfd);
		goto exit;
	}

	if (listen(lfd, RNAT_SOCKET_TEST_TCP_MAX_CONN) < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket test tcp listen fail\n");
		closesocket(lfd);
		goto exit;
	}

	do {
		ap_ip = lwip_get_ip(NETIF_WLAN_AP_INDEX);
		if (ap_ip[0] != 0) {
			break;
		}
		rtos_time_delay_ms(500);
	} while (1);
	own_byte3 = ap_ip[2];

	/* same self-gw static ARP workaround as the UDP test (see doc) */
	wifi_get_mac_address(SOFTAP_WLAN_INDEX, &ap_mac, 0);
	IP4_ADDR(&ap_gw, ap_ip[0], ap_ip[1], ap_ip[2], ap_ip[3]);
	memcpy(ap_gw_eth.addr, ap_mac.octet, ETH_ALEN);
	etharp_add_static_entry(&ap_gw, &ap_gw_eth);

	while (1) {
		neighbor_count = wifi_rmesh_rnat_get_neighbor_info(neighbors, RNAT_ADV_MAX_NEIGHBOR);

		for (n = 0; n < neighbor_count; n++) {
			struct sockaddr_in dest_addr;
			u8 gw = neighbors[n].gw_byte3;
			int cfd;

			if (gw == 0 || gw >= own_byte3) {
				continue;
			}

			slot = RNAT_SOCKET_TEST_TCP_MAX_CONN;
			for (i = 0; i < RNAT_SOCKET_TEST_TCP_MAX_CONN; i++) {
				if (conn_gw[i] == gw) {
					slot = i;
					break;
				}
			}
			if (slot == RNAT_SOCKET_TEST_TCP_MAX_CONN) {
				for (i = 0; i < RNAT_SOCKET_TEST_TCP_MAX_CONN; i++) {
					if (conn_gw[i] == 0) {
						slot = i;
						break;
					}
				}
				if (slot == RNAT_SOCKET_TEST_TCP_MAX_CONN) {
					continue;
				}
				conn_gw[slot] = gw;
				conn_last_ms[slot] = rtos_time_get_current_system_time_ms() - RNAT_SOCKET_TEST_INTERVAL_MS;
			}

			if (conn_fd[slot] >= 0 || RTOS_TIME_GET_PASSING_TIME_MS(conn_last_ms[slot]) < RNAT_SOCKET_TEST_INTERVAL_MS) {
				continue;
			}
			conn_last_ms[slot] = rtos_time_get_current_system_time_ms();

			cfd = socket(AF_INET, SOCK_STREAM, 0);
			if (cfd < 0) {
				continue;
			}
			if (setsockopt(cfd, SOL_SOCKET, SO_BINDTODEVICE, &ap_ifr, sizeof(ap_ifr)) < 0) {
				closesocket(cfd);
				continue;
			}

			memset(&dest_addr, 0, sizeof(dest_addr));
			dest_addr.sin_family = AF_INET;
			dest_addr.sin_port = htons(RNAT_SOCKET_TEST_TCP_PORT);
			dest_addr.sin_addr.s_addr = ((u32)1 << 24) | ((u32)gw << 16) | (200u << 8) | 10u;

			if (connect(cfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket test tcp connect fail gw=%d\n", gw);
				closesocket(cfd);
				continue;
			}
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket test tcp connected gw=%d\n", gw);
			conn_fd[slot] = cfd;
		}

		for (i = 0; i < RNAT_SOCKET_TEST_TCP_MAX_CONN; i++) {
			if (conn_fd[i] < 0 || RTOS_TIME_GET_PASSING_TIME_MS(conn_last_ms[i]) < RNAT_SOCKET_TEST_INTERVAL_MS) {
				continue;
			}
			conn_last_ms[i] = rtos_time_get_current_system_time_ms();

			pkt[0] = 'R';
			pkt[1] = 'N';
			pkt[2] = RNAT_SOCKET_TEST_TYPE_REQ;
			pkt[3] = (u8)seq;
			pkt[4] = (u8)(seq >> 8);
			seq++;

			if (send(conn_fd[i], pkt, 5, 0) < 0) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket test tcp tx fail gw=%d\n", conn_gw[i]);
				closesocket(conn_fd[i]);
				conn_fd[i] = -1;
			} else {
				//RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket test tcp tx req seq=%d gw=%d\n", seq - 1, conn_gw[i]);
			}
		}

		FD_ZERO(&rdset);
		FD_SET(lfd, &rdset);
		maxfd = lfd;
		for (i = 0; i < RNAT_SOCKET_TEST_TCP_MAX_CONN; i++) {
			if (conn_fd[i] < 0) {
				continue;
			}
			FD_SET(conn_fd[i], &rdset);
			if (conn_fd[i] > maxfd) {
				maxfd = conn_fd[i];
			}
		}

		timeout.tv_sec = 0;
		timeout.tv_usec = 200000;
		ret = lwip_select(maxfd + 1, &rdset, NULL, NULL, &timeout);
		if (ret <= 0) {
			continue;
		}

		if (FD_ISSET(lfd, &rdset)) {
			int afd;

			fromlen = sizeof(from_addr);
			afd = accept(lfd, (struct sockaddr *)&from_addr, &fromlen);
			if (afd >= 0) {
				u8 peer_gw = (u8)(from_addr.sin_addr.s_addr >> 16);

				slot = RNAT_SOCKET_TEST_TCP_MAX_CONN;
				for (i = 0; i < RNAT_SOCKET_TEST_TCP_MAX_CONN; i++) {
					if (conn_gw[i] == peer_gw) {
						slot = i;
						break;
					}
				}
				if (slot == RNAT_SOCKET_TEST_TCP_MAX_CONN) {
					for (i = 0; i < RNAT_SOCKET_TEST_TCP_MAX_CONN; i++) {
						if (conn_gw[i] == 0) {
							slot = i;
							break;
						}
					}
				}
				if (slot < RNAT_SOCKET_TEST_TCP_MAX_CONN) {
					if (conn_fd[slot] >= 0) {
						closesocket(conn_fd[slot]);
					}
					conn_fd[slot] = afd;
					conn_gw[slot] = peer_gw;
					conn_last_ms[slot] = rtos_time_get_current_system_time_ms();
					RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket test tcp accepted gw=%d\n", peer_gw);
				} else {
					closesocket(afd);
				}
			}
		}

		for (i = 0; i < RNAT_SOCKET_TEST_TCP_MAX_CONN; i++) {
			if (conn_fd[i] < 0 || !FD_ISSET(conn_fd[i], &rdset)) {
				continue;
			}

			recv_len = recv(conn_fd[i], pkt, sizeof(pkt), 0);
			if (recv_len <= 0) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket test tcp closed gw=%d\n", conn_gw[i]);
				closesocket(conn_fd[i]);
				conn_fd[i] = -1;
				conn_last_ms[i] = rtos_time_get_current_system_time_ms();
				continue;
			}
			if (recv_len < 5 || pkt[0] != 'R' || pkt[1] != 'N') {
				continue;
			}

			if (pkt[2] == RNAT_SOCKET_TEST_TYPE_REQ) {
				pkt[2] = RNAT_SOCKET_TEST_TYPE_REP;
				send(conn_fd[i], pkt, 5, 0);
				//RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket test tcp rx req seq=%d, replied\n", (u16)(pkt[3] | (pkt[4] << 8)));
			} else if (pkt[2] == RNAT_SOCKET_TEST_TYPE_REP) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "socket test tcp rx reply seq=%d, link OK\n", (u16)(pkt[3] | (pkt[4] << 8)));
			}
		}
	}

exit:
	if (g_rnat_intercom) {
		g_rnat_intercom->sock_test_tcp_task = NULL;
	}
	rtos_task_delete(NULL);
}
#endif

void wtn_rnat_intercom_set_self_gw(void)
{
	struct rtw_mac ap_mac = {0};
	struct eth_addr ap_gw_eth;
	ip4_addr_t ap_gw;
	u8 *ap_ip = lwip_get_ip(NETIF_WLAN_AP_INDEX);

	wifi_rmesh_rnat_set_gw(ap_ip);
	/*self-gw static ARP workaround*/
	wifi_get_mac_address(SOFTAP_WLAN_INDEX, &ap_mac, 0);
	IP4_ADDR(&ap_gw, ap_ip[0], ap_ip[1], ap_ip[2], ap_ip[3]);
	memcpy(ap_gw_eth.addr, ap_mac.octet, ETH_ALEN);
	etharp_add_static_entry(&ap_gw, &ap_gw_eth);
}

int wtn_rnat_intercom_start(void)
{
	if (g_rnat_intercom != NULL) {
		return RTK_SUCCESS;
	}
	if (rmesh_rnat_intercom_init() != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r%s intercom init failed\n", __FUNCTION__);
		return RTK_FAIL;
	}
	if (rtos_task_create(&g_rnat_intercom->adv_task, ((const char *)"rnat_adv_thread"), rnat_adv_send_thread, g_rnat_intercom, RTOS_MINIMAL_STACK_SIZE,
						 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r%s rtos_task_create failed\n", __FUNCTION__);
		rmesh_rnat_intercom_deinit();
		return RTK_FAIL;
	}
#ifdef CONFIG_WTN_RNAT_INTERCOM_TEST
	if (rtos_task_create(&g_rnat_intercom->sock_test_task, ((const char *)"rnat_socket_udp_test_thread"), rnat_socket_udp_test_thread, NULL, 1024 * 2,
						 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r%s rtos_task_create failed\n", __FUNCTION__);
	}
	if (rtos_task_create(&g_rnat_intercom->sock_test_tcp_task, ((const char *)"rnat_socket_test_tcp_thread"), rnat_socket_test_tcp_thread, NULL, 1024 * 2,
						 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r%s rtos_task_create failed\n", __FUNCTION__);
	}
#endif
	return RTK_SUCCESS;
}

#endif /* CONFIG_RNAT_EN && CONFIG_WTN_RNAT_INTERCOM */
