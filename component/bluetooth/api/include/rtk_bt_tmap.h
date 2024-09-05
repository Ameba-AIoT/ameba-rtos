/**
 * @file      rtk_bt_tmap.h
 * @author    atonio_liu@realsil.com.cn
 * @brief     Bluetooth LE Audio Public Broadcast Profile type and function definition
 * @copyright Copyright (c) 2023. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_TMAP_H__
#define __RTK_BT_TMAP_H__

#include <app_bt_le_audio_common.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if (defined(CONFIG_BT_TMAP_SUPPORT) && CONFIG_BT_TMAP_SUPPORT)
/* ---------------------------------- Date Types ------------------------------------- */
/**
 * @typedef   rtk_bt_le_audio_tmas_client_discovery_done_ind_t
 * @brief     Bluetooth LE audio TMAS client discovery done indication
 */
typedef struct {
	uint16_t conn_handle;                                      /**< connect handle. */
	bool    is_found;                                          /**< Whether to find service. */
	bool    load_from_ftl;                                     /**< Whether the service table is loaded from FTL. */
} rtk_bt_le_audio_tmas_client_discovery_done_ind_t;

/**
 * @typedef   rtk_bt_le_audio_tmas_read_role_result_t
 * @brief     Bluetooth LE audio TMAS client read TMAP role result
 */
typedef struct {
	uint16_t conn_handle;
	uint16_t role;                                              /**< @ref rtk_bt_le_audio_tmap_role_t  */
	uint16_t cause;
} rtk_bt_le_audio_tmas_read_role_result_t;

/* ------------------------------ Functions Declaration ------------------------------ */
/**
 * @defgroup  bt_tmap BT TMAP APIs
 * @brief     BT TMAP related function APIs
 * @ingroup   bt_le_audio
 * @{
 */

/**
 * @brief     BLE AUDIO TMAS CLIENT READ ROLE.
 * @param[in] conn_handle:  connect handle.
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_tmas_client_read_role(uint16_t conn_handle);

/**
 * @brief     config tmap broadcast media receiver channel or neighbor device.
 * @param[in] channel: channel allocation, 1 for left, 2 for right, 3 for stereo
 * @param[in] p_neighbor_addr: pointer of neghbor device bt address, NULL for no neighbor
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_tmap_broadcast_media_receiver_cfg(uint8_t sound_channel, uint8_t *p_neighbor_addr);

/**
 * @brief     config tmap unicast media receiver channel or neighbor device
 * @param[in] channel: channel allocation, 1 for left, 2 for right, 3 for stereo
 * @param[in] p_neighbor_addr: pointer of neghbor device bt address, NULL for no neighbor
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_tmap_unicast_media_receiver_cfg(uint8_t channel, uint8_t *p_neighbor_addr);
#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_TMAP_H__ */
