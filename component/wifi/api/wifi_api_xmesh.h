/**
  ******************************************************************************
  * @file    wifi_api_xmesh.h
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

#ifndef __WIFI_API_XMESH_H
#define __WIFI_API_XMESH_H

#ifdef __cplusplus
extern "C" {
#endif

#define XMESH_HDR_LEN           20
#define XMESH_HDR_FLAGS_OFFSET  16
#define XMESH_HDR_FLAG_RETRY        BIT(0)
/******************************************************
 *               Function Declarations
 ******************************************************/
/**
 * @brief  Initialize the xmesh driver in one call: advertise the boot epoch, set the group ID,
 *         switch to the working channel and assign the self ID.
 * @note   The caller is responsible for computing/persisting these values
 *         (e.g. bumping the boot epoch and loading the self ID from flash) before calling this API.
 * @param[in]  channel: Working channel to switch to.
 * @param[in]  groupid: Group ID to assign.
 * @param[in]  selfid: Self ID to assign (1-based).
 * @param[in]  boot_epoch: Monotonically-increasing per-boot identifier. Neighbors detect a sender
 *                         reset by observing a higher epoch and wipe their stored rxseq/bitmap state.
 * @return
 *        - @ref RTK_SUCCESS : The API executed successfully.
 *        - @ref RTK_FAIL : Driver internal error.
 */
int wifi_xmesh_init(u8 channel, u16 groupid, u8 selfid, u8 boot_epoch);
/**
 * @brief  Enable promiscuous mode and register a callback to receive all xmesh packets.
 * @param[in]  callback: Handler invoked for each received packet. Runs in Wi-Fi RX context,
 *                       so it must be lightweight and non-blocking. The return value follows
 *                       the promisc callback convention.
 * @return None.
 */
void wifi_xmesh_register_cb(u8(*callback)(struct rtw_rx_pkt_info *pkt_info));
/**
 * @brief  Build and send a raw xmesh frame carrying the given payload.
 * @param[in]  payload: Pointer to the payload bytes to send.
 * @param[in]  len: Payload length in bytes.
 * @return
 *        - @ref RTK_SUCCESS : The frame was sent successfully.
 *        - @ref RTK_FAIL : Buffer allocation failed or the frame could not be transmitted.
 */
int wifi_xmesh_send_frame(u8 *payload, u16 len);
/**
 * @brief  Get RSSI between self and nodes with ID ranging from 1 to max_id
 * @param[out]   buf: Buffer to store RSSI values, size must be at least *max_id bytes.
 * @param[inout] max_id: Pointer to the expected maximum node ID.
 *                      - Input: Expected maximum number of nodes; used to determine buffer allocation size.
 *                      - Output: Actual highest node ID with valid RSSI data.
 * @return
 *        - @ref RTK_SUCCESS : The API executed successfully.
 *        - @ref RTK_FAIL : Driver internal error.
 */
int wifi_xmesh_get_rssi_info_list(u8 *buf, u8 *max_id);

/**
 * @brief  Get the local relay count and per-node packet-loss statistics.
 * @param[out]   relay_cnt: Buffer to store the number of packets relayed (forwarded) by self.
 * @param[out]   loss_2_cnt: Buffer for per-node count of 2 consecutive lost packets.
 *                           Size must be at least *max_id entries.
 * @param[out]   loss_3_cnt: Buffer for per-node count of 3 consecutive lost packets.
 *                           Size must be at least *max_id entries.
 * @param[out]   loss_m3_cnt: Buffer for per-node count of more than 3 consecutive lost packets.
 *                            Size must be at least *max_id entries.
 * @param[inout] max_id: Pointer to the expected maximum node ID.
 *                      - Input: Expected maximum number of nodes; used to determine buffer allocation size.
 *                      - Output: Actual highest node ID with valid loss data.
 * @return
 *        - @ref RTK_SUCCESS : The API executed successfully.
 *        - @ref RTK_FAIL : Driver internal error.
 */
int wifi_xmesh_get_trx_statistic(u32 *relay_cnt, u16 *loss_2_cnt, u16 *loss_3_cnt, u16 *loss_m3_cnt, u8 *max_id);
/**
 * @brief  Clear the xmesh relay and packet-loss statistics maintained by the driver.
 * @return
 *        - @ref RTK_SUCCESS : The API executed successfully.
 *        - @ref RTK_FAIL : Driver internal error.
 */
int wifi_xmesh_clear_trx_statistic(void);
/**
 * @brief  Stop or resume xmesh beacon transmission.
 * @param[in]  enable: 1 to stop sending xmesh beacons.
 * @return
 *        - @ref RTK_SUCCESS : The API executed successfully.
 *        - @ref RTK_FAIL : Driver internal error.
 */
int wifi_xmesh_stop_xmesh(u8 enable);

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_API_XMESH_H */
