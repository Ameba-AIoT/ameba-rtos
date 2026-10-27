/**
 * @file      rtk_bt_gattc.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth GATT Client part type and function definition.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_GATTC_H__
#define __RTK_BT_GATTC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <bt_api_config.h>
#include <dlist.h>
#include <rtk_bt_def.h>

#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB

/**
 * @struct    rtk_bt_gattc_uuid_t
 * @brief     Define service UUID structure.
 */
typedef struct {
	bool is_uuid16;                  /*!< Whether the UUID is 16-bit.
                                      *   - true: p.uuid16 holds the UUID value.
                                      *   - false: p.uuid128 holds the UUID value. */
	uint8_t instance_id;             /*!< Instance index when multiple attributes share the same UUID.
                                      *   Starts from zero. */
	union {
		uint16_t uuid16;             /*!< 16-bit UUID value, valid when is_uuid16 is true. */
		uint8_t uuid128[16];         /*!< 128-bit UUID value, valid when is_uuid16 is false. */
	} p;
} rtk_bt_gattc_uuid_t;

/**
 * @struct    rtk_bt_gattc_register_t
 * @brief     Bluetooth GATT Client register callback for a specific service.
 */
typedef struct {
	uint16_t profile_id;                            /*!< Profile ID. */
	rtk_bt_gattc_uuid_t srv_uuid;                   /*!< Service UUID. */
} rtk_bt_gattc_register_t;

/**
 * @typedef   rtk_bt_gattc_cccd_cfg_t
 * @brief     Bluetooth GATT cccd value.
 */
typedef enum {
	RTK_BT_GATT_CCCD_DISABLE = 0x00,                /*!< Client disable service cccd. */
	RTK_BT_GATT_CCCD_NOTIFICATION = 0x01,           /*!< Client enable service notification. */
	RTK_BT_GATT_CCCD_INDICATION = 0x02,             /*!< Client enable service indication. */
	RTK_BT_GATT_CCCD_ALL = 0x03,                    /*!< Client enable all properties service supported. */
} rtk_bt_gattc_cccd_cfg_t;

/**
 * @typedef   rtk_bt_gattc_find_type_t
 * @brief     Bluetooth GATT Client find type.
 */
typedef enum {
	RTK_BT_GATT_FIND_CHARACTERISTIC_HANDLE,         /*!< Find characteristic handle by characteristic UUID. */
	RTK_BT_GATT_FIND_CHARACTERISTIC_DESC_RANGE,     /*!< Find handle range of characteristic descriptors by characteristic UUID. */
	RTK_BT_GATT_FIND_CHARACTERISTIC_CCCD_HANDLE,    /*!< Find characteristic cccd descriptor by characteristic UUID. */
	RTK_BT_GATT_FIND_INCLUDE_SERVICE,               /*!< Find if a service is included in specific primary service by service UUID. */
} rtk_bt_gattc_find_type_t;

/**
 * @struct    rtk_bt_gattc_find_param_t
 * @brief     Bluetooth GATT Client discover parameter for different operations.
 */
typedef struct {
	uint16_t conn_handle;                        /*!< Connection handle of the ACL connection to be discovered.
                                                  *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_gattc_find_type_t type;               /*!< Find type. */

	union {
		struct find_char_param {
			rtk_bt_gattc_uuid_t srv_uuid;        /*!< Service UUID. */
			rtk_bt_gattc_uuid_t char_uuid;       /*!< Characteristic UUID. */
			uint16_t *p_handle;                  /*!< Output: characteristic value handle. */
		} find_char;                             /*!< Used when type is RTK_BT_GATT_FIND_CHARACTERISTIC_HANDLE to find characteristic handle by characteristic UUID. */

		struct find_char_range_param {
			rtk_bt_gattc_uuid_t srv_uuid;        /*!< Service UUID. */
			rtk_bt_gattc_uuid_t char_uuid;       /*!< Characteristic UUID. */
			uint16_t *p_handle_start;            /*!< Output: start handle of the characteristic descriptors. */
			uint16_t *p_handle_end;              /*!< Output: end handle of the characteristic descriptors. */
			uint16_t *p_cccd_handle;             /*!< Output: CCCD handle, or zero if the characteristic does not contain a Client Characteristic Configuration descriptor. */
		} find_char_range;                       /*!< Used when type is RTK_BT_GATT_FIND_CHARACTERISTIC_DESC_RANGE to find handle range of characteristic descriptors.
                                                  *   by characteristic UUID. */

		struct find_char_cccd_param {
			rtk_bt_gattc_uuid_t srv_uuid;        /*!< Service UUID. */
			rtk_bt_gattc_uuid_t char_uuid;       /*!< Characteristic UUID. */
			uint16_t *p_handle;                  /*!< Output: CCCD descriptor handle. */
			bool *p_notify;                      /*!< Output:
                                                  *   - true: the characteristic supports notifications.
                                                  *   - false: the characteristic does not support notifications. */
			bool *p_indicate;                    /*!< Output:
                                                  *   - true: the characteristic supports indications.
                                                  *   - false: the characteristic does not support indications. */
		} find_char_cccd;                        /*!< Used when type is RTK_BT_GATT_FIND_CHARACTERISTIC_CCCD_HANDLE to find characteristic cccd descriptor.
                                                  *   by characteristic UUID. */

		struct find_include_srv_param {
			rtk_bt_gattc_uuid_t primary_uuid;    /*!< Primary service UUID. */
			rtk_bt_gattc_uuid_t include_uuid;    /*!< Include service UUID. */
			uint8_t *p_instance_num;             /*!< Output: number of matching included service instances found. */
			uint8_t instance_id[20];             /*!< Output: array of instance IDs for each matching included service. */
		} find_include_srv;                      /*!< Used when type is RTK_BT_GATT_FIND_INCLUDE_SERVICE to find if a service is included in a specific primary service
                                                  *   by service UUID. */
	};                                           /*!< Anonymous union member in rtk_bt_gattc_find_param_t. */
} rtk_bt_gattc_find_param_t;

/**
 * @typedef   rtk_bt_gattc_read_type_t
 * @brief     Bluetooth GATT Client read type.
 */
typedef enum {
	RTK_BT_GATT_CHAR_READ_BY_HANDLE,         /*!< Read characteristic value by a known attribute handle. */
	RTK_BT_GATT_CHAR_READ_BY_UUID,           /*!< Read characteristic values by UUID. */
} rtk_bt_gattc_read_type_t;

/**
 * @struct    rtk_bt_gattc_read_param_t
 * @brief     Bluetooth GATT Client read parameter.
 */
typedef struct {
	uint16_t profile_id;                     /*!< Profile ID. */
	uint16_t conn_handle;                    /*!< Connection handle of the ACL connection to be read.
                                              *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_gattc_read_type_t type;           /*!< Characteristic value read type. */
	union {
		struct by_handle_param {
			uint16_t handle;                 /*!< Characteristic value handle. */
		} by_handle;                         /*!< Read characteristic value. */

		struct by_uuid_param {
			uint16_t start_handle;           /*!< Start attribute handle for search range. */
			uint16_t end_handle;             /*!< End attribute handle for search range. */
			rtk_bt_gattc_uuid_t char_uuid;   /*!< Characteristic UUID. */
		} by_uuid;                           /*!< Read using characteristic UUID. */
	};                                       /*!< Anonymous union member in rtk_bt_gattc_read_param_t. */
} rtk_bt_gattc_read_param_t;

/**
 * @typedef   rtk_bt_gattc_write_type_t
 * @brief     Bluetooth GATT Client write type.
 */
typedef enum {
	RTK_BT_GATT_CHAR_WRITE_REQ = 0x00,        /*!< Write characteristic value request. */
	RTK_BT_GATT_CHAR_WRITE_NO_RSP,            /*!< Write characteristic value without response. */
	RTK_BT_GATT_CHAR_WRITE_NO_RSP_SIGNED,     /*!< Write characteristic value without response and with signed data. */
} rtk_bt_gattc_write_type_t;

/**
 * @struct    rtk_bt_gattc_write_param_t
 * @brief     Bluetooth GATT Client write parameter.
 */
typedef struct {
	uint16_t profile_id;                      /*!< Profile ID. */
	uint16_t conn_handle;                     /*!< Connection handle of the ACL connection to be written.
                                               *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_gattc_write_type_t type;           /*!< Characteristic write type. */
	uint16_t handle;                          /*!< Characteristic value handle. */
	uint16_t length;                          /*!< Length of the data. */
	void *data;                               /*!< Data to be written. */
} rtk_bt_gattc_write_param_t;

/**
 * @struct    rtk_bt_gattc_update_cccd_param_t
 * @brief     Bluetooth GATT Client write cccd parameter.
 */
typedef struct {
	uint16_t profile_id;                      /*!< Profile ID‌. */
	uint16_t conn_handle;                     /*!< Connection handle of the ACL connection to update CCCD.
                                               *   Range: 0x0000 to 0x0EFF. */
	bool srv_cfg;                             /*!< Whether this CCCD update applies to the entire service.
                                               *   - true: update all CCCDs in the service identified by srv_uuid.
                                               *   - false: update only the CCCD of the characteristic identified by char_uuid. */
	rtk_bt_gattc_uuid_t srv_uuid;             /*!< UUID of the service containing the target characteristic(s). */
	rtk_bt_gattc_uuid_t char_uuid;            /*!< UUID of the target characteristic; ignored when srv_cfg is true. */
	bool bnotify;                             /*!< Whether to update the notification bit of the CCCD.
                                               *   - true: set the notification enable bit.
                                               *   - false: leave the notification bit unchanged. */
	bool bindicate;                           /*!< Whether to update the indication bit of the CCCD.
                                               *   - true: set the indication enable bit.
                                               *   - false: leave the indication bit unchanged. */
} rtk_bt_gattc_update_cccd_param_t;

/**
 * @struct    rtk_bt_gattc_discover_ind_t
 * @brief     Bluetooth GATTC event discover result event msg.
 */
typedef struct {
	uint16_t profile_id;                      /*!< Profile ID. */
	uint16_t conn_handle;                     /*!< Connection handle of the ACL connection associated with the received discovery result.
                                               *   Range: 0x0000 to 0x0EFF. */
	bool is_found;                            /*!< Whether the service was found on the remote peer.
                                               *   - true: service was found.
                                               *   - false: the service was not present on the remote server. */
	bool load_from_storage;                   /*!< Whether the discovery result was loaded from cached storage.
                                               *   - true: loaded from non-volatile storage.
                                               *   - false: obtained by discovery over the air. */
	uint8_t srv_instance_num;                 /*!< Number of instances of the service found on the remote server. */
} rtk_bt_gattc_discover_ind_t;

/**
 * @struct    rtk_bt_gattc_discover_all_state_ind_t
 * @brief     Bluetooth GATTC event discover all state event msg.
 */
typedef struct {
	uint16_t conn_handle;                     /*!< Connection handle of the ACL connection associated with the received discovery all state.
                                               *   Range: 0x0000 to 0x0EFF. */
	bool is_success;                          /*!< Whether the full discovery completed without error.
                                               *   - true: all services and attributes were discovered successfully.
                                               *   - false: discovery was incomplete or failed. */
	bool load_from_storage;                   /*!< Whether the discovery result was loaded from cached storage.
                                               *   - true: loaded from storage.
                                               *   - false: obtained by discovery over the air. */
} rtk_bt_gattc_discover_all_state_ind_t;

/**
 * @typedef   rtk_bt_gatt_service_char_t
 * @brief     GATT service characteristic exist flag.
 */
typedef enum {
	RTK_BT_GATT_SVC_DATABASE_HASH_FLAG = 0x01,                                     /*!< Database hash characteristic bit. */
	RTK_BT_GATT_SVC_CLIENT_SUPPORTED_FEATURES_FLAG = 0x02,                         /*!< Client supported features characteristic bit. */
	RTK_BT_GATT_SVC_SERVER_SUPPORTED_FEATURES_FLAG = 0x04,                         /*!< Server supported features characteristic bit. */
	RTK_BT_GATT_SVC_SERVICE_CHANGED_FLAG = 0x08,                                   /*!< Service changed characteristic bit. */
} rtk_bt_gatt_service_char_t;

/**
 * @def       RTK_BT_GATT_SVC_DATABASE_HASH_LEN
 * @brief     GATT Service Database Hash length.
 */
#define RTK_BT_GATT_SVC_DATABASE_HASH_LEN   16

/**
 * @def       RTK_BT_GATT_SVC_CLIENT_SUPPORTED_FEATURES_MAX_LEN
 * @brief     GATT Service Client supported features maximum length.
 */
#define RTK_BT_GATT_SVC_CLIENT_SUPPORTED_FEATURES_MAX_LEN 1

/**
 * @def       RTK_BT_GATT_SVC_SERVER_SUPPORTED_FEATURES_MAX_LEN
 * @brief     GATT Service Server supported features maximum length.
 */
#define RTK_BT_GATT_SVC_SERVER_SUPPORTED_FEATURES_MAX_LEN 1

/**
 * @struct    rtk_bt_gattc_gatt_service_info_ind_t
 * @brief     Bluetooth GATTC discover service info event msg.
 */
typedef struct {
	uint16_t conn_handle;                                                          /*!< Connection handle of the ACL connection associated with the received service info result.
                                                                                    *   Range: 0x0000 to 0x0EFF. */
	bool is_found;                                                                 /*!< Whether the GATT service was found on the remote peer.
                                                                                    *   - true: service info fields below are valid.
                                                                                    *   - false: no GATT service was discovered; all info fields are invalid. */
	bool load_from_storage;                                                        /*!< Whether the service info was loaded from cached storage.
                                                                                    *   - true: loaded from storage.
                                                                                    *   - false: obtained by live discovery over the air. */
	uint8_t char_flag;                                                             /*!< Characteristic exist bit field; see @ref rtk_bt_gatt_service_char_t. */
	uint8_t database_hash[RTK_BT_GATT_SVC_DATABASE_HASH_LEN];                      /*!< Database Hash value; The value can be used when RTK_BT_GATT_SVC_DATABASE_HASH_FLAG is set in char_flag. */
	uint8_t client_features_len;                                                   /*!< Length of client_features; The value can be used when
                                                                                    *   RTK_BT_GATT_SVC_CLIENT_SUPPORTED_FEATURES_FLAG is set in char_flag. */
	uint8_t client_features[RTK_BT_GATT_SVC_CLIENT_SUPPORTED_FEATURES_MAX_LEN];    /*!< Client Supported Features value; The value can be used when
                                                                                    *   RTK_BT_GATT_SVC_CLIENT_SUPPORTED_FEATURES_FLAG is set in char_flag. */
	uint8_t server_features_len;                                                   /*!< Length of server_features; The value can be used when
                                                                                    *   RTK_BT_GATT_SVC_SERVER_SUPPORTED_FEATURES_FLAG is set in char_flag. */
	uint8_t server_features[RTK_BT_GATT_SVC_SERVER_SUPPORTED_FEATURES_MAX_LEN];    /*!< Server Supported Features value; The value can be used when
                                                                                    *   RTK_BT_GATT_SVC_SERVER_SUPPORTED_FEATURES_FLAG is set in char_flag. */
} rtk_bt_gattc_gatt_service_info_ind_t;

/**
 * @typedef   rtk_bt_gattc_char_type_t
 * @brief     Define characteristic type.
 */
typedef enum {
	RTK_BT_GATT_CHAR_UNKNOWN = 0x00,
	RTK_BT_GATT_CHAR_VALUE = 0x01,         /*!< Characteristic value. */
	RTK_BT_GATT_CHAR_CCCD = 0x02,          /*!< Characteristic cccd. */
	RTK_BT_GATT_CHAR_DESCRIPTOR = 0x03,    /*!< Characteristic descriptor. */
} rtk_bt_gattc_char_type_t;

/**
 * @struct    rtk_bt_gattc_read_ind_t
 * @brief     Bluetooth GATT Client read result event msg.
 */
typedef struct {
	uint16_t profile_id;                   /*!< Profile ID. */
	uint16_t conn_handle;                  /*!< Connection handle of the ACL connection associated with the received read result.
                                            *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_gattc_char_type_t char_type;    /*!< Characteristic type. */
	uint8_t srv_instance_id;               /*!< ID of the service in which the characteristic is contained. */
	rtk_bt_gattc_uuid_t char_uuid;         /*!< Characteristic UUID. */
	uint16_t handle;                       /*!< Characteristic handle. */
	uint16_t len;                          /*!< Characteristic value length. */
	uint8_t *value;                        /*!< Characteristic value. */
	rtk_bt_status_t status;                /*!< Characteristic read result. */
	uint16_t err_code;                     /*!< Status of the Characteristic read procedure. 0x00: success. */
} rtk_bt_gattc_read_ind_t;

/**
 * @struct    rtk_bt_gattc_write_ind_t
 * @brief     Bluetooth GATT Client write result event msg.
 */
typedef struct {
	uint16_t profile_id;                   /*!< Profile ID. */
	uint16_t conn_handle;                  /*!< Connection handle of the ACL connection associated with the received write result.
                                            *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_gattc_char_type_t char_type;    /*!< Characteristic type. */
	uint8_t  srv_instance_id;              /*!< ID of the service in which the characteristic is contained. */
	rtk_bt_gattc_uuid_t char_uuid;         /*!< Characteristic UUID. */
	rtk_bt_gattc_write_type_t type;        /*!< Characteristic write type. */
	uint16_t handle;                       /*!< Characteristic handle. */
	rtk_bt_status_t status;                /*!< Characteristic write result. */
	uint16_t err_code;                     /*!< Status of the Characteristic write procedure. 0x00: success. */
} rtk_bt_gattc_write_ind_t;

/**
 * @struct    rtk_bt_gattc_cccd_value_ind_t
 * @brief     Bluetooth GATT Client receive remote server's notify or indicate event msg.
*/
typedef struct {
	uint16_t profile_id;                   /*!< Profile ID. */
	uint16_t conn_handle;                  /*!< Connection handle of the ACL connection associated with the received indicate or notify result.
                                            *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;                          /*!< L2CAP channel ID to indicate which channel the value is received from. */
	uint8_t  srv_instance_id;              /*!< ID of the service in which the characteristic is contained. */
	rtk_bt_gattc_uuid_t char_uuid;         /*!< Characteristic UUID. */
	uint16_t value_handle;                 /*!< Characteristic handle. */
	uint16_t len;                          /*!< Characteristic value length. */
	uint8_t *value;                        /*!< Characteristic value. */
} rtk_bt_gattc_cccd_value_ind_t;

/**
 * @struct    rtk_bt_gattc_cccd_update_ind_t
 * @brief     Bluetooth GATT Client write cccd complete event msg.
 */
typedef struct {
	uint16_t profile_id;                  /*!< Profile ID. */
	uint16_t conn_handle;                 /*!< Connection handle of the ACL connection associated with the received cccd update result.
                                           *   Range: 0x0000 to 0x0EFF. */
	bool srv_cfg;                         /*!< Whether the update applied to all CCCDs in the service.
                                           *   - true: all CCCDs in the service were updated.
                                           *   - false: only the CCCD of a single characteristic was updated. */
	uint8_t srv_instance_id;              /*!< ID of the service in which the characteristic is contained. */
	uint16_t cccd_data;                   /*!< Raw CCCD attribute value written to the server. */
	rtk_bt_gattc_uuid_t uuid;             /*!< UUID of the characteristic whose CCCD was updated. */
	rtk_bt_status_t status;               /*!< Overall result of the CCCD write operation. */
	uint16_t err_code;                    /*!< Status of the Characteristic cccd update procedure. 0x00: success. */
	bool bnotify;                         /*!< Whether the notification enable bit changed.
                                           *   - true: notification bit was modified.
                                           *   - false: notification bit was not changed. */
	bool bindicate;                       /*!< Whether the indication enable bit changed.
                                           *   - true: indication bit was modified.
                                           *   - false: indication bit was not changed. */
} rtk_bt_gattc_cccd_update_ind_t;

/**
 * @typedef   rtk_bt_gap_chann_type_t
 * @brief     Bluetooth GAP channel type.
 */
typedef enum {
	RTK_BT_GAP_CHANN_TYPE_LE_ATT,         /*!< ATT bearer on LE transport (default LE connection). */
	RTK_BT_GAP_CHANN_TYPE_LE_ECFC,        /*!< EATT (Enhanced ATT) bearer on LE transport. */
	RTK_BT_GAP_CHANN_TYPE_BREDR_ATT,      /*!< ATT bearer on BR/EDR (Classic Bluetooth) transport. */
	RTK_BT_GAP_CHANN_TYPE_BREDR_ECFC,     /*!< EATT bearer on BR/EDR transport. */
} rtk_bt_gap_chann_type_t;

/**
 * @struct    rtk_bt_gattc_conn_del_t
 * @brief     The event data for GATT_CLIENT_EVENT_CONN_DEL.
 */
typedef struct {
	uint16_t profile_id;                  /*!< Profile ID. */
	uint16_t cid;                         /*!< L2CAP channel id. */
	rtk_bt_gap_chann_type_t chann_type;   /*!< L2CAP channel type. */
} rtk_bt_gattc_conn_del_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_gattc_ble_mgr BT GATT Client ble_mgr APIs
 * @brief     BT GATT Client related ble_mgr function APIs.
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Register GATT Client profile.
 * @param[in] profile_id: Profile ID.
 *                        Must be unique among all registered profiles.
 * @param[in] srv_uuid: UUID of the service.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gattc_register_profile(uint16_t profile_id, rtk_bt_gattc_uuid_t srv_uuid);

/**
 * @brief     Initiate an attribute discovery on the remote server, discovering all attributes.
 * @param[in] conn_handle: Connection handle of the ACL connection to discover all attributes.
 *                         Range: 0x0000 to 0x0EFF.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gattc_discover_all(uint16_t conn_handle);

/**
 * @brief     GATT Client discovery operations.
 *            The result is reported asynchronously via event @ref RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND.
 * @param[in,out] p_find_param: Pointer to GATT Client find parameters.
 *                              Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gattc_find(rtk_bt_gattc_find_param_t *p_find_param);

/**
 * @brief     Read a characteristic value from the remote server.
 *            The result is reported asynchronously via event @ref RTK_BT_GATTC_EVT_READ_RESULT_IND.
 * @param[in] p_read_param: Pointer to GATT Client Read parameters.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gattc_read(rtk_bt_gattc_read_param_t *p_read_param);

/**
 * @brief     Write a characteristic value to the remote server using the specified write procedure.
 *            The result is reported asynchronously via event @ref RTK_BT_GATTC_EVT_WRITE_RESULT_IND.
 * @param[in] p_write_param: Pointer to GATT Client write parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gattc_write(rtk_bt_gattc_write_param_t *p_write_param);

/**
 * @brief     Enable notifications or indications on one or more remote server characteristics by writing their CCCDs.
 *            The result is reported asynchronously via event @ref RTK_BT_GATTC_EVT_CCCD_ENABLE_IND.
 * @param[in] p_update_cccd_param: Pointer to GATT Client update cccd parameters.
 *                                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gattc_enable_notify_or_indicate(
	rtk_bt_gattc_update_cccd_param_t *p_update_cccd_param);

/**
 * @brief     Disable notifications or indications on one or more remote server characteristics by clearing their CCCDs.
 *            The result is reported asynchronously via event @ref RTK_BT_GATTC_EVT_CCCD_DISABLE_IND.
 * @param[in] p_update_cccd_param: Pointer to GATT Client update cccd parameters.
 *                                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gattc_disable_notify_or_indicate(
	rtk_bt_gattc_update_cccd_param_t *p_update_cccd_param);
/**
 * @}
 */
#else /******************************* #if RTK_BLE_MGR_LIB *****************************/
#define RTK_BT_GATTC_ATTACH_ID(_profile_id,_conn_handle)        \
        (((_profile_id)<<16) | (_conn_handle))                  \

#define RTK_BT_GET_PROFILE_ID(_val)                             \
        (((_val) & 0xFFFF0000) >> 16)                           \

#define RTK_BT_GET_CONN_HANDLE(_val)                            \
        ((_val)&0x0000FFFF)                                     \

/**
 * @typedef   rtk_bt_gattc_discover_type_t
 * @brief     Bluetooth GATT Client discover type.
 */
typedef enum {
	RTK_BT_GATT_DISCOVER_PRIMARY_ALL,               /*!< Discover all primary services. */
	RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID,           /*!< Discover primary services by service UUID. */
	RTK_BT_GATT_DISCOVER_INCLUDE,                   /*!< Find included services within a specified handle range. */
	RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL,        /*!< Discover all characteristics of a service. */
	RTK_BT_GATT_DISCOVER_CHARACTERISTIC_BY_UUID,    /*!< Discover characteristics by UUID. */
	RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL,           /*!< Discover all characteristic descriptors. */
} rtk_bt_gattc_discover_type_t;

/**
 * @struct    rtk_bt_gattc_discover_param_t
 * @brief     Bluetooth GATT Client discover parameter for initiating different GATT Client discovery procedures.
 */
typedef struct {
	uint16_t profile_id;                            /*!< Profile ID. */
	uint16_t conn_handle;                           /*!< Connection handle of the ACL connection to be discovered.
                                                     *   Range: 0x0000 to 0x0EFF. */
	uint16_t seq;                                   /*!< Sequence number, for convenience, not mandatory. */
	rtk_bt_gattc_discover_type_t type;              /*!< Discovery type. */

	union {
		struct disc_primary_all_param {
			uint8_t dummy;                /*!< Useless, just for integrity. */
		} disc_primary_all;               /*!< Discover all primary services. */

		struct disc_primary_by_uuid_param {
			uint8_t uuid[16];             /*!< Service UUID to match, in little-endian format. */
			uint8_t uuid_type;            /*!< UUID type of the value in uuid[]. @ref rtk_bt_uuid_type_t. */
		} disc_primary_by_uuid;           /*!< Discover primary service by service UUID. */

		struct disc_inc_param {
			uint16_t start_handle;        /*!< Start handle of the service to search for included services. */
			uint16_t end_handle;          /*!< End handle of the service to search for included services. */
		} disc_inc;                       /*!< Find included services. */

		struct disc_char_all_param {
			uint16_t start_handle;        /*!< Start handle of the service's attribute range. */
			uint16_t end_handle;          /*!< End handle of the service's attribute range. */
		} disc_char_all;                  /*!< Discover all characteristics of a service. */

		struct disc_char_by_uuid_param {
			uint16_t start_handle;        /*!< Start handle of the attribute range to search. */
			uint16_t end_handle;          /*!< End handle of the attribute range to search. */
			uint8_t uuid[16];             /*!< Characteristic UUID to match, in little-endian format. */
			uint8_t uuid_type;            /*!< UUID type of the value in uuid[]. @ref rtk_bt_uuid_type_t. */
		} disc_char_by_uuid;              /*!< Discover characteristics by UUID. */

		struct disc_descriptor_param {
			uint16_t start_handle;        /*!< Start handle of the characteristic's attribute range. */
			uint16_t end_handle;          /*!< End handle of the characteristic's attribute range. */
		} disc_descriptor;                /*!< Discover all characteristic descriptors. */
	};                                              /*!< Anonymous union member in rtk_bt_gattc_discover_param_t. */
} rtk_bt_gattc_discover_param_t;

/**
 * @struct    rtk_bt_gattc_discover_ind_t
 * @brief     Bluetooth GATTC event discover result event msg.
 */
typedef struct {
	uint16_t profile_id;                        /*!< Profile ID. */
	uint16_t conn_handle;                       /*!< Connection handle of the ACL connection to be discovered.
                                                 *   Range: 0x0000 to 0x0EFF. */
	uint16_t seq;                               /*!< Sequence number echoed from the originating request. */
	rtk_bt_gattc_discover_type_t type;          /*!< Discovery procedure variant that produced this result. */
	rtk_bt_status_t status;                     /*!< Discovery result status.
                                                 *   One discover request may raise multiple discover events because many attribute entries may be reported as results
                                                 *   when discover completes. Each event reports one attribute entry.
                                                 *   - if status is @ref RTK_BT_STATUS_CONTINUE, it means one discover attribute entry is reported with this msg,
                                                 *   discover data of union below is not empty, another msg will follow this.
                                                 *   - if status is @ref RTK_BT_STATUS_DONE, means the discover is finished, and discover data of union below is empty.
                                                 *   - if status is @ref RTK_BT_STATUS_FAIL, means the discover is failed, inquire err_code for the cause of failure. */
	uint16_t err_code;                          /*!< Status of the discover procedure. 0x00: success. */

	union {
		struct disc_primary_all_per_param {
			uint16_t start_handle;        /*!< Primary service start handle. */
			uint16_t end_handle;          /*!< Primary service end handle. */
			uint8_t uuid[16];             /*!< Primary service UUID value, in little-endian format. */
			uint8_t uuid_type;            /*!< UUID type of the value in uuid[]. @ref rtk_bt_uuid_type_t. */
		} disc_primary_all_per;           /*!< Discover primary all services result (per attribute). */

		struct disc_primary_by_uuid_per_param {
			uint16_t start_handle;        /*!< Primary service start handle. */
			uint16_t end_handle;          /*!< Primary service end handle. */
		} disc_primary_by_uuid_per;       /*!< Discover primary services by service UUID result (per attribute). */

		struct disc_inc_per_param {
			uint16_t handle;              /*!< Include service declaration handle. */
			uint16_t start_handle;        /*!< Include service start handle. */
			uint16_t end_handle;          /*!< Include service end handle. */
			uint8_t uuid[16];             /*!< Include service UUID value, in little-endian format. */
			uint8_t uuid_type;            /*!< UUID type of the value in uuid[]. @ref rtk_bt_uuid_type_t. */
		} disc_inc_per;                   /*!< Find included services result (per service). */

		struct disc_char_all_per_param {
			uint16_t handle;              /*!< Characteristic declaration handle. */
			uint8_t properties;           /*!< Characteristic properties. */
			uint16_t value_handle;        /*!< Characteristic value attribute handle. */
			uint8_t uuid[16];             /*!< Characteristic UUID value, in little-endian format. */
			uint8_t uuid_type;            /*!< UUID type of the value in uuid[]. @ref rtk_bt_uuid_type_t. */
		} disc_char_all_per;              /*!< Discover all characteristics result (per attribute). */

		struct disc_char_by_uuid_per_param {
			uint16_t handle;              /*!< Characteristic declaration handle. */
			uint8_t properties;           /*!< Characteristic properties bitmask. */
			uint16_t value_handle;        /*!< Characteristic value attribute handle. */
			uint8_t uuid[16];             /*!< Characteristic UUID value, in little-endian format. */
			uint8_t uuid_type;            /*!< UUID type of the value in uuid[]. @ref rtk_bt_uuid_type_t. */
		} disc_char_by_uuid_per;          /*!< Discover characteristics by UUID result (per attribute). */

		struct disc_descriptor_per_param {
			uint16_t handle;              /*!< Characteristic descriptor attribute handle. */
			uint8_t uuid[16];             /*!< Characteristic descriptor UUID, in little-endian format. */
			uint8_t uuid_type;            /*!< UUID type of the value in uuid[]. @ref rtk_bt_uuid_type_t. */
		} disc_descriptor_per;            /*!< Discover all characteristic descriptors result (per attribute). */
	};                                          /*!< Anonymous union member in rtk_bt_gattc_discover_ind_t. */
} rtk_bt_gattc_discover_ind_t;

/**
 * @typedef   rtk_bt_gattc_read_type_t
 * @brief     Bluetooth GATT Client read type.
 */
typedef enum {
	RTK_BT_GATT_CHAR_READ_BY_HANDLE,                /*!< Read characteristic value by handle. */
	RTK_BT_GATT_CHAR_READ_BY_UUID,                  /*!< Read characteristic value by UUID. */
	RTK_BT_GATT_CHAR_READ_MULTIPLE,                 /*!< Read multiple characteristic values. */
	RTK_BT_GATT_CHAR_READ_MULTIPLE_VARIABLE,        /*!< Read multiple variable characteristic values. */
} rtk_bt_gattc_read_type_t;


/**
 * @struct    rtk_bt_gattc_read_param_t
 * @brief     Bluetooth GATT Client read parameter for a GATT Client read procedure.
 */
typedef struct {
	uint16_t profile_id;                        /*!< Profile ID. */
	uint16_t conn_handle;                       /*!< Connection handle of the ACL connection to be read.
                                                 *   Range: 0x0000 to 0x0EFF. */
	uint16_t seq;                               /*!< Sequence number, for convenience, not mandatory. */
	rtk_bt_gattc_read_type_t type;              /*!< Characteristic value read type. */
	union {
		struct by_handle_param {
			uint16_t handle;          /*!< Characteristic value handle to read. */
		} by_handle;                  /*!< Read characteristic value by handle. */

		struct by_uuid_param {
			uint16_t start_handle;    /*!< Start of the attribute handle range to read. */
			uint16_t end_handle;      /*!< End of the attribute handle range to read. */
			uint8_t uuid[16];         /*!< Characteristic UUID to read, 2 or 16 octet UUID. */
			uint8_t uuid_type;        /*!< UUID type of the value in uuid[]. @ref rtk_bt_uuid_type_t. */
		} by_uuid;                    /*!< Read characteristic value by UUID. */

		struct multiple_param {
			uint32_t handle_count;    /*!< Number of attribute handles in the handles array. */
			uint16_t *handles;        /*!< Array of characteristic value handles to read. */
		} multiple;                   /*!< Read multiple characteristic values. */

		struct multiple_variable_param {
			uint32_t handle_count;    /*!< Number of attribute handles in the handles array. */
			uint16_t *handles;        /*!< Array of characteristic value handles to read. */
		} multiple_variable;          /*!< Read multiple variable-length characteristic values. */
	};                                          /*!< Anonymous union member in rtk_bt_gattc_read_param_t. */

} rtk_bt_gattc_read_param_t;

/**
 * @struct    rtk_bt_gattc_read_ind_t
 * @brief     Bluetooth GATTC event read result event msg.
 */
typedef struct {
	uint16_t profile_id;                     /*!< Profile ID. */
	uint16_t conn_handle;                    /*!< Connection handle of the ACL connection associated with the received read result.
                                              *   Range: 0x0000 to 0x0EFF. */
	uint16_t seq;                            /*!< Sequence number, for convenience, not mandatory. */
	rtk_bt_gattc_read_type_t type;           /*!< Characteristic value read type. */
	rtk_bt_status_t status;                  /*!< GATTC read status.
                                              *   One read request may raise multiple read events because many attribute entries may be reported as results
                                              *   when read completes. Each event reports one attribute entry.
                                              *   - if status is @ref RTK_BT_STATUS_CONTINUE, means one read attribute entry is reported with this msg,
                                              *   read data of union below is not empty, and another msg will follow this.
                                              *   - if status is @ref RTK_BT_STATUS_DONE, means the read is finished, and read data of union below is empty.
                                              *   - if status is @ref RTK_BT_STATUS_FAIL, means the read is failed, inquire err_code for the cause of failure. */
	uint16_t err_code;                       /*!< Status of the Characteristic read procedure. 0x00: success. */

	union {
		struct by_handle_res_param {
			uint16_t handle;          /*!< Characteristic value handle. */
			uint16_t len;             /*!< The length of characteristic value. */
			uint8_t *value;           /*!< Characteristic value. */
		} by_handle;                  /*!< Read characteristic value result. */

		struct by_uuid_per_param {
			uint16_t handle;          /*!< Characteristic value handle. */
			uint16_t len;             /*!< The length of characteristic value. */
			uint8_t *value;           /*!< Characteristic value. */
		} by_uuid_per;                /*!< Read using characteristic UUID result (per attribute). */

		struct multiple_per_param {
			uint16_t dummy;           /*!< Useless, just for padding, make the 4 union element the same memory structure. */
			uint16_t len;             /*!< The length of characteristic value. */
			uint8_t *value;           /*!< Characteristic value. */
		} multiple_per;               /*!< Read multiple characteristic values (per attribute). */

		struct multiple_variable_per_param {
			uint16_t dummy;           /*!< Useless, just for padding, make the 4 union element the same memory structure. */
			uint16_t len;             /*!< The length of characteristic value. */
			uint8_t *value;           /*!< Characteristic value. */
		} multiple_variable_per;      /*!< Read multiple variable length characteristic values (per attribute). */
	};                                       /*!< Anonymous union member in rtk_bt_gattc_read_ind_t. */
} rtk_bt_gattc_read_ind_t;

/**
 * @typedef   rtk_bt_gattc_write_type_t
 * @brief     Bluetooth GATT Client write type.
 */
typedef enum {
	RTK_BT_GATT_CHAR_WRITE_REQ = 0x00,       /*!< Write characteristic value with response. */
	RTK_BT_GATT_CHAR_WRITE_NO_RSP,           /*!< Write characteristic value without response. */
	RTK_BT_GATT_CHAR_WRITE_NO_RSP_SIGNED,    /*!< Write characteristic value without response, and with signed data. */
} rtk_bt_gattc_write_type_t;

/**
 * @struct    rtk_bt_gattc_write_param_t
 * @brief     Bluetooth GATT Client write parameter.
 */
typedef struct {
	uint16_t profile_id;                     /*!< Profile ID. */
	uint16_t conn_handle;                    /*!< Connection handle of the ACL connection to be written.
                                              *   Range: 0x0000 to 0x0EFF. */
	uint16_t seq;                            /*!< Sequence number, for convenience, not mandatory. */
	rtk_bt_gattc_write_type_t type;          /*!< Characteristic value write type. */
	uint16_t handle;                         /*!< Characteristic value attribute handle to write. */
	uint16_t length;                         /*!< Length of the data to write. */
	void *data;                              /*!< Pointer to the data buffer to write. */
} rtk_bt_gattc_write_param_t;

/**
 * @struct    rtk_bt_gattc_write_ind_t
 * @brief     Bluetooth GATTC event write result event msg.
 */
typedef struct {
	uint16_t profile_id;                     /*!< Profile ID. */
	uint16_t conn_handle;                    /*!< Connection handle of the ACL connection associated with the received write result.
                                              *   Range: 0x0000 to 0x0EFF. */
	uint16_t seq;                            /*!< Sequence number, for convenience, not mandatory. */
	rtk_bt_gattc_write_type_t type;          /*!< Characteristic value write type. */
	rtk_bt_status_t status;                  /*!< Write result status.
                                              *   - if status is @ref RTK_BT_STATUS_DONE, means write succeeded,
                                              *   - if status is @ref RTK_BT_STATUS_FAIL, means write is failed, inquire err_code for the cause of failure. */
	uint16_t err_code;                       /*!< Status of the Characteristic write procedure. 0x00: success. */
	uint16_t handle;                         /*!< Characteristic value attribute handle that was written. */
} rtk_bt_gattc_write_ind_t;

/**
 * @struct    rtk_bt_gattc_update_cccd_param_t
 * @brief     Bluetooth GATT Client write cccd parameter for enabling or disabling remote server notifications or indications via a CCCD write.
 */
typedef struct {
	uint16_t profile_id;               /*!< Profile ID. */
	uint16_t conn_handle;              /*!< Connection handle of the ACL connection to update CCCD.
                                        *   Range: 0x0000 to 0x0EFF. */
	uint16_t seq;                      /*!< Sequence number, for convenience, not mandatory. */
	uint16_t char_val_handle;          /*!< Characteristic value handle associated with the CCCD. */
	uint16_t cccd_handle;              /*!< Client characteristic configuration descriptor (CCCD) handle. */
	bool bnotify;                      /*!< Whether to set the Notify bit in the CCCD.
                                        *   - true: request notification.
                                        *   - false: leave the Notify bit unchanged. */
	bool bindicate;                    /*!< Whether to set the Indicate bit in the CCCD.
                                        *   - true: request indication.
                                        *   - false: leave the Indicate bit unchanged. */
} rtk_bt_gattc_update_cccd_param_t;

/**
 * @struct    rtk_bt_gattc_cccd_update_ind_t
 * @brief     Bluetooth GATT Client write cccd complete event msg.
 */
typedef struct {
	uint16_t profile_id;         /*!< Profile ID. */
	uint16_t conn_handle;        /*!< Connection handle of the ACL connection associated with the received cccd update result.
                                  *   Range: 0x0000 to 0x0EFF. */
	uint16_t seq;                /*!< Sequence number, for convenience, not mandatory. */
	rtk_bt_status_t status;      /*!< GATTC cccd update status. */
	uint16_t err_code;           /*!< Status of the Characteristic cccd update procedure. 0x00: success. */
	uint16_t cccd_handle;        /*!< Client characteristic configuration descriptor (CCCD) handle. */
	bool bnotify;                /*!< Whether the Notify bit was written to the CCCD.
                                  *   - true: the Notify bit was set.
                                  *   - false: the Notify bit was not set. */
	bool bindicate;              /*!< Whether the Indicate bit was written to the CCCD.
                                  *   - true: the Indicate bit was set.
                                  *   - false: the Indicate bit was not set. */
} rtk_bt_gattc_cccd_update_ind_t;

/**
 * @struct    rtk_bt_gattc_cccd_value_ind_t
 * @brief     Bluetooth GATT Client receive remote server's notify or indicate event msg.
*/
typedef struct {
	uint16_t profile_id;         /*!< Profile ID. */
	uint16_t conn_handle;        /*!< Connection handle of the ACL connection associated with the received indicate or notify result.
                                  *   Range: 0x0000 to 0x0EFF. */
	uint16_t value_handle;       /*!< Characteristic value handle that triggered the notification or indication. */
	uint16_t len;                /*!< Length of the characteristic value. */
	uint8_t *value;              /*!< Buffer holding the characteristic value. */
} rtk_bt_gattc_cccd_value_ind_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_gattc_normal BT GATT Client Normal APIs
 * @brief     BT GATT Client related function normal APIs.
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Register GATT Client profile.
 * @param[in] profile_id: Unique identifier for the GATT Client profile to register.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gattc_register_profile(uint16_t profile_id);

/**
 * @brief     Request an ATT MTU exchange on the specified connection. This allows the client and server to negotiate an ATT MTU size.
 *            The result is reported asynchronously via event @ref RTK_BT_GATTC_EVT_MTU_EXCHANGE.
 * @param[in] conn_handle: Connection handle of the ACL connection to perform MTU exchange.
 *                         Range: 0x0000 to 0x0EFF.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gattc_exchange_mtu(uint16_t conn_handle);

/**
 * @brief     Perform a GATT discovery procedure on the remote server.
 *            The result is reported asynchronously via event @ref RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND.
 * @param[in] p_dis_param: GATT Client Discovery parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gattc_discover(rtk_bt_gattc_discover_param_t *p_dis_param);

/**
 * @brief     Perform a GATT read procedure on the remote server.
 *            The result is reported asynchronously via event @ref RTK_BT_GATTC_EVT_READ_RESULT_IND.
 * @param[in] p_read_param: GATT Client Read parameters.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gattc_read(rtk_bt_gattc_read_param_t *p_read_param);

/**
 * @brief     Perform a GATT write procedure on the remote server.
 *            The result is reported asynchronously via event @ref RTK_BT_GATTC_EVT_WRITE_RESULT_IND.
 * @param[in] p_write_param: GATT Client write parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gattc_write(rtk_bt_gattc_write_param_t *p_write_param);

/**
 * @brief     Write the CCCD of a remote characteristic to enable notifications or indications.
 *            The result is reported asynchronously via event @ref RTK_BT_GATTC_EVT_CCCD_ENABLE_IND.
 * @param[in] p_update_cccd_param: CCCD enable parameters.
 *                                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gattc_enable_notify_or_indicate(
	rtk_bt_gattc_update_cccd_param_t *p_update_cccd_param);

/**
 * @brief     Write the CCCD of a remote characteristic to disable notifications or indications.
 *            The result is reported asynchronously via event @ref RTK_BT_GATTC_EVT_CCCD_DISABLE_IND.
 * @param[in] p_update_cccd_param: CCCD disable parameters.
 *                                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gattc_disable_notify_or_indicate(
	rtk_bt_gattc_update_cccd_param_t *p_update_cccd_param);
/**
 * @}
 */
#endif /* #if RTK_BLE_MGR_LIB */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_GATTC_H__ */

