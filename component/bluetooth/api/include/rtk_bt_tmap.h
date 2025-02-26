/**
 * @file      rtk_bt_tmap.h
 * @author    atonio_liu@realsil.com.cn
 * @brief     Bluetooth LE Audio Public Broadcast Profile type and function definition
 * @copyright Copyright (c) 2023. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_TMAP_H__
#define __RTK_BT_TMAP_H__

#include <rtk_bt_def.h>
#include <basic_types.h>
#include <bt_api_config.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_le_audio_def.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if (defined(CONFIG_BT_TMAP_SUPPORT) && CONFIG_BT_TMAP_SUPPORT)
/* ---------------------------------- Date Types ------------------------------------- */
/**
 * @typedef    rtk_bt_le_audio_tmap_role_t
 * @brief     Bluetooth LE AUDIO TMAP Role.
 */
typedef enum {
	RTK_BT_LE_AUDIO_TMAP_ROLE_UNKNOWN = 0,
	RTK_BT_LE_AUDIO_TMAP_ROLE_CG = 0x01,                    /**< TMAP Call Gateway. */
	RTK_BT_LE_AUDIO_TMAP_ROLE_CT = 0x02,                    /**< TMAP Call Terminal. */
	RTK_BT_LE_AUDIO_TMAP_ROLE_UMS = 0x04,                   /**< TMAP Unicast Media Sender. */
	RTK_BT_LE_AUDIO_TMAP_ROLE_UMR = 0x08,                   /**< TMAP Unicast Media Receiver. */
	RTK_BT_LE_AUDIO_TMAP_ROLE_BMS = 0x10,                   /**< TMAP Broadcast Media Sender. */
	RTK_BT_LE_AUDIO_TMAP_ROLE_BMR = 0x20,                   /**< TMAP Broadcast Media Receiver. */
} rtk_bt_le_audio_tmap_role_t;

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
 * @}
 */

#endif /* CONFIG_BT_TMAP_SUPPORT */
#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_TMAP_H__ */
