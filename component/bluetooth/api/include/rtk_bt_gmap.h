/**
 * @file      rtk_bt_gmap.h
 * @author    michael_kong@realsil.com.cn
 * @brief     Bluetooth LE Audio Gaming Audio Profile type and function definition
 * @copyright Copyright (c) 2025. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_GMAP_H__
#define __RTK_BT_GMAP_H__

#include <rtk_bt_def.h>
#include <basic_types.h>
#include <bt_api_config.h>
#include <rtk_bt_le_audio_def.h>

#ifdef __cplusplus
extern "C"
{
#endif  /* __cplusplus */

#if (defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT)

#define RTK_BT_GATT_UUID_GMAS                        0x1858    /**< Gaming Audio Service. */
#define RTK_BT_GMAS_UUID_CHAR_GMAP_ROLE              0x2C00    /**< GMAP Role. */
#define RTK_BT_GMAS_CHAR_UUID_UGG_FEATURES           0x2C01    /**< UGG Features. */
#define RTK_BT_GMAS_CHAR_UUID_UGT_FEATURES           0x2C02    /**< UGT Features. */
#define RTK_BT_GMAS_CHAR_UUID_BGS_FEATURES           0x2C03    /**< BGS Features. */
#define RTK_BT_GMAS_CHAR_UUID_BGR_FEATURES           0x2C04    /**< BGR Features. */
/* ---------------------------------- Date Types ------------------------------------- */

/**
 * @typedef   rtk_bt_le_audio_gmap_role_t
 * @brief     Define BT GMAP role
 */
typedef enum {
	RTK_BT_LE_AUDIO_GMAP_ROLE_UNKNOWN = 0,
	RTK_BT_LE_AUDIO_GMAP_ROLE_UGG = 0x01,                   /**< GMAP Unicast Game Gateway. */
	RTK_BT_LE_AUDIO_GMAP_ROLE_UGT = 0x02,                   /**< GMAP Unicast Game Terminal. */
	RTK_BT_LE_AUDIO_GMAP_ROLE_BGS = 0x04,                   /**< GMAP Broadcast Game Sender. */
	RTK_BT_LE_AUDIO_GMAP_ROLE_BGR = 0x08,                   /**< GMAP Broadcast Game Receiver. */
} rtk_bt_le_audio_gmap_role_t;

/**
 * @struct    rtk_bt_le_audio_gmas_read_ind_t
 * @brief     gmas client read result indication structure.
 */
typedef struct {
	uint16_t conn_handle;
	uint16_t cid;
	uint16_t char_uuid;
} rtk_bt_le_audio_gmas_read_ind_t;

/**
 * @struct    rtk_bt_le_audio_gmas_client_dis_t
 * @brief     gmas client discovery result structure.
 */
typedef struct {
	uint16_t conn_handle;
	bool    is_found;
	bool    load_from_ftl;
} rtk_bt_le_audio_gmas_client_dis_t;

/**
 * @struct    rtk_bt_le_audio_gmas_client_read_role_result_t
 * @brief     gmas client read role result structure.
 */
typedef struct {
	uint16_t conn_handle;
	rtk_bt_le_audio_gmap_role_t gmap_role;
	uint16_t cause;
} rtk_bt_le_audio_gmas_client_read_role_result_t;

/**
 * @struct    rtk_bt_le_audio_gmas_client_read_features_t
 * @brief     gmas client read features structure.
 */
typedef struct {
	uint16_t conn_handle;
	uint16_t char_uuid;
} rtk_bt_le_audio_gmas_client_read_features_t;

/**
 * @struct    rtk_bt_le_audio_gmas_client_read_features_result_t
 * @brief     gmas client read features result structure.
 */
typedef struct {
	uint16_t conn_handle;
	uint16_t char_uuid;
	uint8_t  features;
	uint16_t cause;
} rtk_bt_le_audio_gmas_client_read_features_result_t;

/* ------------------------------ Functions Declaration ------------------------------ */
/**
 * @defgroup  bt_gmap BT GMAP APIs
 * @brief     BT GMAP related function APIs
 * @ingroup   bt_le_audio
 * @{
 */

/**
 * @brief     GMAS client read gmap role.
 * @param[in] conn_handle: ACL connection handle
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_le_audio_gmas_client_read_role(uint16_t conn_handle);

/**
 * @brief     GMAS client read gmap role features.
 * @param[in] conn_handle: ACL connection handle
 * @param[in] char_uuid: GMAS Characteristic UUID
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_le_audio_gmas_client_read_features(uint16_t conn_handle, uint16_t char_uuid);

/**
 * @}
 */

#endif /* RTK_BLE_AUDIO_GMAP_SUPPORT */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __RTK_BT_GMAP_H__ */