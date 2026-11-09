/**
 * @file      rtk_bt_gmap.h
 * @author    michael_kong@realsil.com.cn
 * @brief     Bluetooth LE Audio Gaming Audio Profile type and function definition.
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

#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT

#define RTK_BT_GATT_UUID_GMAS                        0x1858    /*!< Gaming Audio Service. */
#define RTK_BT_GMAS_UUID_CHAR_GMAP_ROLE              0x2C00    /*!< GMAP Role. */
#define RTK_BT_GMAS_CHAR_UUID_UGG_FEATURES           0x2C01    /*!< UGG Features. */
#define RTK_BT_GMAS_CHAR_UUID_UGT_FEATURES           0x2C02    /*!< UGT Features. */
#define RTK_BT_GMAS_CHAR_UUID_BGS_FEATURES           0x2C03    /*!< BGS Features. */
#define RTK_BT_GMAS_CHAR_UUID_BGR_FEATURES           0x2C04    /*!< BGR Features. */
/************************************** Data Types *************************************/

/**
 * @typedef   rtk_bt_le_audio_gmap_role_t
 * @brief     Bluetooth LE Audio GMAP role bit mask of a device.
 */
typedef enum {
	RTK_BT_LE_AUDIO_GMAP_ROLE_UNKNOWN = 0,    /*!< Unknown GMAP role. */
	RTK_BT_LE_AUDIO_GMAP_ROLE_UGG = 0x01,     /*!< GMAP Unicast Game Gateway. */
	RTK_BT_LE_AUDIO_GMAP_ROLE_UGT = 0x02,     /*!< GMAP Unicast Game Terminal. */
	RTK_BT_LE_AUDIO_GMAP_ROLE_BGS = 0x04,     /*!< GMAP Broadcast Game Sender. */
	RTK_BT_LE_AUDIO_GMAP_ROLE_BGR = 0x08,     /*!< GMAP Broadcast Game Receiver. */
} rtk_bt_le_audio_gmap_role_t;

/**
 * @struct    rtk_bt_le_audio_gmas_read_ind_t
 * @brief     Bluetooth LE Audio GMAS server read indication structure.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection.
                              *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;            /*!< L2CAP channel identifier of the read request. */
	uint16_t char_uuid;      /*!< UUID of the GMAS characteristic being read. */
} rtk_bt_le_audio_gmas_read_ind_t;

/**
 * @struct    rtk_bt_le_audio_gmas_client_dis_t
 * @brief     Bluetooth LE Audio GMAS client discovery result structure.
 */
typedef struct {
	uint16_t conn_handle;     /*!< Connection handle of the ACL connection.
                               *   Range: 0x0000 to 0x0EFF. */
	bool    is_found;         /*!< Whether GMAS is found on the remote device.
                               *   - true: GMAS is found.
                               *   - false: GMAS is not found. */
	bool    load_from_ftl;    /*!< Whether the service table is loaded from FTL.
                               *   - true: The service table is restored from FTL.
                               *   - false: The service table is obtained by discovery on the remote device. */
} rtk_bt_le_audio_gmas_client_dis_t;

/**
 * @struct    rtk_bt_le_audio_gmas_client_read_role_result_t
 * @brief     Bluetooth LE Audio GMAS client read role result structure.
 */
typedef struct {
	uint16_t conn_handle;                     /*!< Connection handle of the ACL connection.
                                               *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_audio_gmap_role_t gmap_role;    /*!< GMAP role of the remote device, as a bit mask of @ref rtk_bt_le_audio_gmap_role_t. */
	uint16_t cause;                           /*!< Result code of the read operation. */
} rtk_bt_le_audio_gmas_client_read_role_result_t;

/**
 * @struct    rtk_bt_le_audio_gmas_client_read_features_t
 * @brief     Bluetooth LE Audio GMAS client read features parameters.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection.
                              *   Range: 0x0000 to 0x0EFF. */
	uint16_t char_uuid;      /*!< UUID of the GMAS features characteristic to read. */
} rtk_bt_le_audio_gmas_client_read_features_t;

/**
 * @struct    rtk_bt_le_audio_gmas_client_read_features_result_t
 * @brief     Bluetooth LE Audio GMAS client read features result structure.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection.
                              *   Range: 0x0000 to 0x0EFF. */
	uint16_t char_uuid;      /*!< UUID of the GMAS features characteristic that was read. */
	uint8_t  features;       /*!< GMAP features bit mask read from the characteristic.
                              *   The meaning of each bit depends on char_uuid. */
	uint16_t cause;          /*!< Result code of the read operation. */
} rtk_bt_le_audio_gmas_client_read_features_result_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_gmap BT GMAP APIs
 * @brief     BT GMAP related function APIs.
 * @ingroup   bt_le_audio
 * @{
 */

/**
 * @brief     Read the GMAP role of the remote device from its GMAS. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_AUDIO_GMAS_EVT_CLIENT_READ_ROLE_RESULT.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_gmas_client_read_role(uint16_t conn_handle);

/**
 * @brief     Read a GMAP features characteristic of the remote device from its GMAS. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_AUDIO_GMAS_EVT_CLIENT_READ_FEATURES_RESULT.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] char_uuid: UUID of the features characteristic to read.
 *                       One of @ref RTK_BT_GMAS_CHAR_UUID_UGG_FEATURES, @ref RTK_BT_GMAS_CHAR_UUID_UGT_FEATURES,
 *                       @ref RTK_BT_GMAS_CHAR_UUID_BGS_FEATURES, @ref RTK_BT_GMAS_CHAR_UUID_BGR_FEATURES.
 * @return
 *            - @ref rtk_bt_err_common
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
