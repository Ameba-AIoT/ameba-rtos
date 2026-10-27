/**
  ******************************************************************************
  * @file    wifi_api_rmesh_rnat.h
  * @author
  * @version
  * @date
  * @brief   This file provides user interface for Wi-Fi station and AP mode
  *          configuration base on the functionalities provided by Realtek Wi-Fi driver.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef __WIFI_API_RMESH_RNAT_H
#define __WIFI_API_RMESH_RNAT_H

#ifdef __cplusplus
extern "C" {
#endif

/* rmesh_rnat ADV frame layout: [hdrlen][RMESH_RNAT_ADV_PATTERN][IE_ID(1B)][IE_LEN(2B,LE)][IE_VALUE]...;
 * hdrlen varies with the QoS bit. unknown IE_IDs are skipped on rx, for forward compat.
 */
#define RMESH_RNAT_ADV_PATTERN		"RMESHADV"
#define RMESH_RNAT_ADV_PATTERN_LEN	8
#define RMESH_RNAT_ADV_IE_OFFSET(hdrlen)	((hdrlen) + RMESH_RNAT_ADV_PATTERN_LEN)

/* IE_ID values; add new ones here as the ADV payload grows */
#define RMESH_RNAT_IE_ID_NEIGHBOR_INFO	1	/* value: N * struct wtn_rnat_neighbor */
#define RMESH_RNAT_IE_ID_RNAT_PROFILE	2	/* value: rnat_mac(6B), seq(u16,LE), then N * ip(4B); first ip is rnat_mac's own ip */

struct wtn_rnat_neighbor {
	u8 mac[ETH_ALEN];
	s8 rssi;
	u8 gw_byte3;
};

/**
 * @brief  Fill out[] with this node's own directly-heard rmesh_rnat neighbors, for building the
 *         outgoing ADV payload's RMESH_RNAT_IE_ID_NEIGHBOR_INFO IE.
 * @param[out]  out: Buffer to store neighbor entries, size must be at least max_count entries.
 * @param[in]   max_count: Capacity of out[].
 * @return  Number of neighbor entries written to out[].
 */
u8 wifi_rmesh_rnat_get_neighbor_info(struct wtn_rnat_neighbor *out, u8 max_count);

/**
 * @brief  Set this node's own rmesh_rnat gw address.
 * @param[in]  gw: Pointer to this node's own IP address, 4 bytes in the same wire order as
 *                 lwIP's ip4_addr_t.addr (gw[0] = IP's 1st octet).
 * @return  None
 */
void wifi_rmesh_rnat_set_gw(const u8 *gw);

/**
 * @brief  Set/update/remove one local P1 client's IP in the rmesh_rnat driver's client table,
 *         looked up by mac. If an entry for mac already exists: ip_byte4 != 0 refreshes its IP,
 *         ip_byte4 == 0 removes the entry. If no entry exists: ip_byte4 != 0 creates one (ip_byte4
 *         == 0 is a no-op).
 * @param[in]  mac: MAC address of the client, ETH_ALEN bytes.
 * @param[in]  ip_byte4: Last byte of the client's IP (always shares this node's own gw subnet),
 *                       or 0 to remove the client's entry.
 * @return  None
 */
void wifi_rmesh_rnat_set_client_ip(const u8 *mac, u8 ip_byte4);

/**
 * @brief  Register a callback to receive every valid rmesh_rnat ADV frame heard by the driver.
 *         The driver still parses the ADV itself afterwards; the callback return value is ignored.
 * @param[in]  adv_callback: Called with the whole received frame (802.11 header + RMESH_RNAT_ADV_PATTERN + IEs).
 *                           buf is only valid during the call and must be treated as read-only. NULL to unregister.
 * @return  None
 */
void wifi_rmesh_rnat_register_adv_callback(int (*adv_callback)(u8 *buf, u16 len));

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_API_RMESH_RNAT_H */
