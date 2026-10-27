/**
 * @file      rtk_bt_gatts.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth GATT server type and function definitions.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_GATTS_H__
#define __RTK_BT_GATTS_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <bt_api_config.h>
#include <dlist.h>

/**
 * @def       RTK_BT_GATT_INTERNAL
 * @brief     This attribute will be handled by the stack internally. When the attribute is a
 *            characteristic value, this flag is equal to @ref RTK_BT_GATT_VOID.
 */
#define RTK_BT_GATT_INTERNAL                0

/**
 * @def       RTK_BT_GATT_APP
 * @brief     This attribute will be handled by user APP. All read/write to this attribute
 *            will be indicated to user APP by a callback, and user APP needs to call
 *            @ref rtk_bt_gatts_read_resp or @ref rtk_bt_gatts_write_resp to respond to
 *            read/write request from remote and modify the attribute value maintained
 *            in user APP when receive write request.
 */
#define RTK_BT_GATT_APP                     1

/**
 * @def       RTK_BT_GATT_VOID
 * @brief     This attribute value will be passed into lower stack as a pointer. If this
 *            attribute is read by remote, lower stack will auto send this attribute value
 *            to respond to read request. This flag is not suggested to be used when the
 *            attribute is writable(including encrypted/authenticated write), because it
 *            will not indicate APP the write events.
 */
#define RTK_BT_GATT_VOID                    2

/**
 * @struct    rtk_bt_gatt_chrc
 * @brief     Characteristic Attribute Value.
 */
struct rtk_bt_gatt_chrc {
	const struct bt_uuid *uuid;    /*!< Characteristic UUID. */
	uint16_t value_handle;         /*!< Characteristic Value handle. */
	uint8_t properties;            /*!< Characteristic properties. */
};

/**
 * @def       RTK_BT_GATT_ATTRIBUTE
 * @brief     Declaration Macro.
 *            Helper macro to declare an attribute.
 * @param     _uuid Attribute uuid.
 * @param     _perm Attribute access permissions.
 * @param     _user_data Attribute user data.
 * @param     _len Attribute user data length.
 * @param     _flag Attribute flag.
 */
#define RTK_BT_GATT_ATTRIBUTE(_uuid, _perm, _user_data, _len, _flag) \
    {                                                                \
        .uuid = (struct bt_uuid *)_uuid,                             \
        .perm = _perm,                                               \
        .user_data = (void*)_user_data,                              \
        .len = _len,                                                 \
        .flag = _flag,                                               \
    }

/**
 * @def       RTK_BT_GATT_PRIMARY_SERVICE
 * @brief     Primary Service Declaration Macro.
 *            Helper macro to declare a primary service attribute.
 * @param     _service Service attribute value. @ref struct bt_uuid *.
 */
#define RTK_BT_GATT_PRIMARY_SERVICE(_service)                          \
    RTK_BT_GATT_ATTRIBUTE(BT_UUID_GATT_PRIMARY, RTK_BT_GATT_PERM_READ, \
                          _service, 0, RTK_BT_GATT_INTERNAL)

/**
 * @def       RTK_BT_GATT_SECONDARY_SERVICE
 * @brief     Secondary Service Declaration Macro.
 *            Helper macro to declare a secondary service attribute.
 * @param     _service Service attribute value. @ref struct bt_uuid *.
 */
#define RTK_BT_GATT_SECONDARY_SERVICE(_service)                          \
    RTK_BT_GATT_ATTRIBUTE(BT_UUID_GATT_SECONDARY, RTK_BT_GATT_PERM_READ, \
                          _service, 0, RTK_BT_GATT_INTERNAL)

/**
 * @def       RTK_BT_GATT_INCLUDE_SERVICE
 * @brief     Include Service Declaration Macro.
 *            Helper macro to declare database internal include service attribute.
 * @param     _service_incl Pointer to a include service struct @ref struct rtk_bt_gatt_service *.
 */
#define RTK_BT_GATT_INCLUDE_SERVICE(_service_incl)                     \
    RTK_BT_GATT_ATTRIBUTE(BT_UUID_GATT_INCLUDE, RTK_BT_GATT_PERM_READ, \
                          _service_incl, 0, RTK_BT_GATT_INTERNAL)

/**
 * @def       RTK_BT_GATT_CHRC_INIT
 * @brief     Initialization Macro.
 *            Helper macro to init an attribute.
 * @param     _uuid Attribute uuid.
 * @param     _handle Attribute handle.
 * @param     _props Attribute access properties.
 */
#define RTK_BT_GATT_CHRC_INIT(_uuid, _handle, _props) \
    {                                                 \
        .uuid = (struct bt_uuid *)_uuid,                                \
        .value_handle = _handle,                      \
        .properties = _props,                         \
    }

/**
 * @def       RTK_BT_GATT_CHARACTERISTIC
 * @brief     Characteristic and Value Declaration Macro.
 *            Helper macro to declare a characteristic attribute along with its
 *            attribute value.
 * @param     _uuid Characteristic attribute uuid.
 * @param     _props Characteristic attribute properties.
 * @param     _perm Characteristic Attribute access permissions.
 */
#define RTK_BT_GATT_CHARACTERISTIC(_uuid, _props, _perm)                \
    RTK_BT_GATT_ATTRIBUTE(BT_UUID_GATT_CHRC, RTK_BT_GATT_PERM_READ,     \
                          ((struct rtk_bt_gatt_chrc[]){                 \
                              RTK_BT_GATT_CHRC_INIT(_uuid, 0U, _props), \
                          }), 0, RTK_BT_GATT_INTERNAL),                 \
    RTK_BT_GATT_ATTRIBUTE(_uuid, _perm, NULL, 0, RTK_BT_GATT_APP)

/**
 * @def       RTK_BT_GATT_DESCRIPTOR
 * @brief     Descriptor Declaration Macro.
 *            Helper macro to declare a descriptor attribute.
 * @note      If use RTK_BT_GATT_DESCRIPTOR directly, MUST use _user_data as a raw data!
 * @param     _uuid Descriptor attribute uuid.
 * @param     _perm Descriptor attribute access permissions.
 * @param     _user_data Descriptor attribute user data.
 * @param     _len Attribute user data length.
 * @param     _flag Attribute flag.
 */
#define RTK_BT_GATT_DESCRIPTOR(_uuid, _perm, _user_data, _len, _flag) \
    RTK_BT_GATT_ATTRIBUTE(_uuid, _perm, _user_data, _len, _flag)

/**
 * @def       RTK_BT_GATT_CEP
 * @brief     Characteristic Extended Properties Declaration Macro.
 *            Helper macro to declare a CEP attribute.
 * @param     _value Pointer to a struct rtk_bt_gatt_cep. (const struct rtk_bt_gatt_cep *)
 */
#define RTK_BT_GATT_CEP(_value) \
    RTK_BT_GATT_DESCRIPTOR(BT_UUID_GATT_CEP, RTK_BT_GATT_PERM_READ, (void *)_value, 0, RTK_BT_GATT_INTERNAL)

/**
 * @def       RTK_BT_GATT_CUD
 * @brief     Characteristic User Description Descriptor Declaration Macro.
 *            Helper macro to declare a CUD attribute.
 *
 * @param     _value User description NULL-terminated C string. (const uint8_t *value)
 * @param     _perm Descriptor attribute access permissions.
 */
#define RTK_BT_GATT_CUD(_value, _perm) \
    RTK_BT_GATT_DESCRIPTOR(BT_UUID_GATT_CUD, _perm, (void *)_value, 0, RTK_BT_GATT_INTERNAL)

/**
 * @def       RTK_BT_GATT_CCC
 * @brief     Client Characteristic Configuration Declaration Macro.
 *            Helper macro to declare a CCC attribute.
 * @note      Although CCCD value read/write will be handled internally, but User APP will be notified.
 * @param     _perm CCC access permissions.
 */
#define RTK_BT_GATT_CCC(_perm) \
    RTK_BT_GATT_DESCRIPTOR(BT_UUID_GATT_CCC, _perm, 0, 0, RTK_BT_GATT_INTERNAL)

/**
 * @def       RTK_BT_GATT_SCC
 * @brief     Server Characteristic Configuration Declaration Macro.
 *            Helper macro to declare an SCC attribute.
 * @param     _value Pointer to Server Characteristic Configuration bits. (struct rtk_bt_gatt_scc *)
 * @param     _perm SCC access permissions.
 */
#define RTK_BT_GATT_SCC(_value, _perm) \
    RTK_BT_GATT_DESCRIPTOR(BT_UUID_GATT_SCC, _perm, (void *)_value, 0, RTK_BT_GATT_INTERNAL)

/**
 * @def       RTK_BT_GATT_CPF
 * @brief     Characteristic Presentation Format Descriptor Declaration Macro.
 *            Helper macro to declare a CPF attribute.
 * @param     _value Pointer to a struct rtk_bt_gatt_cpf. (struct rtk_bt_gatt_cpf*)
 */
#define RTK_BT_GATT_CPF(_value) \
    RTK_BT_GATT_DESCRIPTOR(BT_UUID_GATT_CPF, RTK_BT_GATT_PERM_READ, (void *)_value, 0, RTK_BT_GATT_INTERNAL)

/**
 * @def       RTK_BT_GATT_CAF
 * @brief     Characteristic Aggregate Format Descriptor Declaration Macro.
 *            Helper macro to declare a CAF attribute.
 * @param     _value Pointer to a struct rtk_bt_gatt_caf. (struct rtk_bt_gatt_caf*)
 */
#define RTK_BT_GATT_CAF(_value) \
    RTK_BT_GATT_DESCRIPTOR(BT_UUID_GATT_CAF, RTK_BT_GATT_PERM_READ, (void *)_value, 0, RTK_BT_GATT_INTERNAL)

/**
 * @def       RTK_BT_GATT_SERVICE
 * @brief     Service Structure Declaration Macro.
 *            Helper macro to declare a service structure.
 * @param     _attrs Service attributes.
 * @param     _app_id Service app id.
 */
#define RTK_BT_GATT_SERVICE(_attrs, _app_id)                  \
    {                                                         \
        .attrs = _attrs,                                      \
        .attr_count = (sizeof(_attrs) / sizeof((_attrs)[0])), \
        .app_id = _app_id,                                    \
    }

/**
 * @typedef   rtk_bt_gatts_client_supported_features_t
 * @brief     Bluetooth GATT Server Service Client Supported Features bit field.
 */
typedef enum {
	RTK_BT_GATTS_CLIENT_SUPPORTED_FEATURES_ROBUST_CACHING_BIT = 0x01,    /*!< The client supports robust caching. */
	RTK_BT_GATTS_CLIENT_SUPPORTED_FEATURES_EATT_BEARER_BIT = 0x02,       /*!< The client supports Enhanced ATT bearer. */
	RTK_BT_GATTS_CLIENT_SUPPORTED_FEATURES_MULTI_NOTIF_BIT = 0x04,       /*!< The client supports receiving ATT_MULTIPLE_HANDLE_VALUE_NTF PDUs. */
} rtk_bt_gatts_client_supported_features_t;

/**
 * @typedef   rtk_bt_gatts_server_supported_features_t
 * @brief     Bluetooth GATT Server Service Server Supported Features bit field.
 */
typedef enum {
	RTK_BT_GATTS_SERVER_SUPPORTED_FEATURES_EATT_BIT = 0x01,    /*!< The server supports Enhanced ATT bearer. */
} rtk_bt_gatts_server_supported_features_t;

/**
 * @typedef   rtk_bt_gatts_srv_type_t
 * @brief     Bluetooth GATT Server Service type.
 */
typedef enum {
	GATT_SERVICE_OVER_BREDR,                /*!< 0, Service over BREDR. */
	GATT_SERVICE_OVER_BLE,                  /*!< 1, Service over BLE. */
	GATT_SERVICE_TYPE_RESERVED,             /*!< 2, RESERVED. */
} rtk_bt_gatts_srv_type_t;


/**
 * @struct    rtk_bt_gatt_cpf
 * @brief     Bluetooth GATT Server Characteristic Presentation Format Attribute Value.
 */
struct rtk_bt_gatt_cpf {
	uint8_t format;                         /*!< Format of the characteristic value. */
	int8_t exponent;                        /*!< Exponent used to further scale the characteristic value. */
	uint16_t unit;                          /*!< Unit of the characteristic value. */
	uint8_t name_space;                     /*!< Namespace of the description to identify the organization. */
	uint16_t description;                   /*!< Description of the characteristic as defined in a higher-layer profile. */
};

/**
 * @struct    rtk_bt_gatt_caf
 * @brief     Bluetooth GATT Server Characteristic Aggregate Format Attribute Value.
 */
struct rtk_bt_gatt_caf {
	uint16_t count;                         /*!< The number of handles in the list of attribute handles. */
	uint16_t *handles;                      /*!< List of Attribute Handles. */
};


/**
 * @struct    rtk_bt_gatt_cep
 * @brief     Bluetooth GATT Server Characteristic Extended Properties Attribute Value.
 */
struct rtk_bt_gatt_cep {
	uint16_t properties;                    /*!< Characteristic Extended properties. */
};


/**
 * @struct    rtk_bt_gatt_scc
 * @brief     Bluetooth GATT Server Server Characteristic Configuration Attribute Value.
 */
struct rtk_bt_gatt_scc {
	uint16_t flags;                         /*!< Server Characteristic Configuration flags. */
};

/**
 * @struct    rtk_bt_gatt_attr_t
 * @brief     Bluetooth GATT Server Attribute structure.
 */
typedef struct {
	const struct bt_uuid *uuid;             /*!< Attribute UUID. */
	union {
		void *read;                         /*!< Attribute read callback. */
		uint16_t flag;                      /*!< Attribute flag (RTK_BT_GATT_INTERNAL/RTK_BT_GATT_APP). */
	};
	union {
		void *write;                        /*!< Attribute write callback. */
		uint16_t len;                       /*!< Attribute user data length. */
	};
	void *user_data;                        /*!< Attribute user data. */
	uint16_t handle;                        /*!< Attribute handle. */
	uint8_t perm;                           /*!< Attribute permissions. */
} rtk_bt_gatt_attr_t;

/**
 * @struct    rtk_bt_gatts_read_resp_param_t
 * @brief     Bluetooth GATT Server response parameter for remote client's read request.
 */
typedef struct {
	uint16_t seq;                           /*!< Sequence number, for convenience, not mandatory. */
	uint16_t app_id;                        /*!< Application identifier of the service. */
	uint16_t conn_handle;                   /*!< Connection handle of the ACL connection to send read response.
                                             *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;                           /*!< L2CAP channel identifier for sending the response.
                                             *   Must match the cid from @ref rtk_bt_gatts_read_ind_t.
                                             *   Ignored when RTK_BLE_MGR_LIB is 0. */
	uint16_t index;                         /*!< Attribute index within the service. */
	uint8_t err_code;                       /*!< ATT error code; see @ref rtk_bt_err_att. Set to 0x00 when not error response. */
	uint16_t len;                           /*!< Length of the response value.
                                             *   only valid when err_code is 0. */
	const void *data;                       /*!< Pointer to the response value data.
                                             *   Only valid when err_code is 0. */
} rtk_bt_gatts_read_resp_param_t;

/**
 * @struct    rtk_bt_gatts_write_resp_param_t
 * @brief     Bluetooth GATT Server response parameter for remote client's write request.
 */
typedef struct {
	uint16_t seq;                           /*!< Sequence number, for convenience, not mandatory. */
	uint16_t app_id;                        /*!< Application identifier of the service. */
	uint16_t conn_handle;                   /*!< Connection handle of the ACL connection to send write response.
                                             *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;                           /*!< L2CAP channel identifier for sending the response.
                                             *   0 selects the channel automatically.
                                             *   Ignored when RTK_BLE_MGR_LIB is 0. */
	uint16_t index;                         /*!< Attribute index within the service. */
	uint8_t type;                           /*!< Write type as received from the remote client. */
	uint8_t err_code;                       /*!< ATT error code; see @ref rtk_bt_err_att. Set to 0x00 when not error response. */
} rtk_bt_gatts_write_resp_param_t;

/**
 * @struct    rtk_bt_gatts_ntf_and_ind_param_t
 * @brief     Bluetooth GATT Server notify or indicate parameter definition.
 */
typedef struct {
	uint16_t seq;                           /*!< Sequence number, for convenience, not mandatory. */
	uint16_t app_id;                        /*!< Application identifier of the service. */
	uint16_t conn_handle;                   /*!< Connection handle of the ACL connection to send indicate or notify.
                                             *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;                           /*!< L2CAP channel identifier for sending the notification or indication.
                                             *   0 selects the channel automatically.
                                             *   Ignored when RTK_BLE_MGR_LIB is 0. */
	uint16_t index;                         /*!< Attribute index within the service. */
	uint16_t len;                           /*!< Length of the notification or indication value. */
	const void *data;                       /*!< Pointer to the notification or indication value data. */
} rtk_bt_gatts_ntf_and_ind_param_t;

/**
 * @struct    rtk_bt_gatts_service_changed_indicate_param_t
 * @brief     Bluetooth GATT Server service changed indicate parameter definition.
 */
typedef struct {
	uint16_t conn_handle;                   /*!< Connection handle of the ACL connection to send service changed indicate.
                                             *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;                           /*!< L2CAP channel identifier for sending the service changed indicate.
                                             *   0 selects the channel automatically.
                                             *   Ignored when RTK_BLE_MGR_LIB is 0. */
	uint16_t start_handle;                  /*!< Start attribute handle of the changed range. */
	uint16_t end_handle;                    /*!< Last attribute handle of the changed range. */
} rtk_bt_gatts_service_changed_indicate_param_t;

/**
 * @struct    rtk_bt_gatts_get_attr_handle_param_t
 * @brief     Bluetooth GATT Server get attribute handle parameter definition.
 */
typedef struct {
	uint16_t app_id;                        /*!< Application identifier of the service. */
	uint16_t attr_index;                    /*!< Zero-based attribute index within the service. */
	uint16_t *attr_handle;                  /*!< Pointer to store the retrieved attribute handle. */
} rtk_bt_gatts_get_attr_handle_param_t;

/**
 * @struct    rtk_bt_gatt_service
 * @brief     Bluetooth GATT Server Service structure.
 */
struct rtk_bt_gatt_service {
	struct list_head list;                  /*!< Node for linking this service into the service list. */
	uint8_t alloc_ind;                      /*!< Indicates whether this service structure was dynamically allocated. */
	uint16_t app_id;                        /*!< Application identifier of the service. */
	rtk_bt_gatt_attr_t *attrs;              /*!< Pointer to the attribute table for this service. */
	uint16_t attr_count;                    /*!< Number of attributes in the attribute table. */
#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
	bool assgin_handle_flag;                /*!< Indicates whether the service start attribute handle is user-assigned.
                                             *   - true: start_handle is used as the first attribute handle.
                                             *   - false: the stack assigns the start handle automatically. */
	uint16_t start_handle;                  /*!< User-assigned start attribute handle for the service.
                                             *   Valid when assgin_handle_flag is true. */
#endif
	rtk_bt_gatts_srv_type_t type;           /*!< Type of the service. */
	uint8_t register_status;                /*!< Current registration status of the service. */
	void *user_data;                        /*!< Application-provided user data pointer. */
	uint32_t server_info;                   /*!< Internal server information. */
};


/**
 * @struct    rtk_bt_gatts_reg_ind_t
 * @brief     Bluetooth GATT Server service register indication event msg.
 */
typedef struct {
	uint16_t reg_status;                    /*!< Result of the service registration; 0x00 indicates success. */
	uint16_t app_id;                        /*!< Application identifier of the registered service. */
} rtk_bt_gatts_reg_ind_t;

/**
 * @struct    rtk_bt_gatt_mtu_exchange_ind_t
 * @brief     Bluetooth ATT MTU exchange indication event msg.
 */
typedef struct {
	uint16_t result;                        /*!< Result code of the MTU exchange procedure; 0x00 indicates success. */
	uint16_t conn_handle;                   /*!< Connection handle of the ACL connection associated with the received MTU exchange result.
                                             *   Range: 0x0000 to 0x0EFF. */
	uint16_t mtu_size;                      /*!< Negotiated ATT MTU size. */
} rtk_bt_gatt_mtu_exchange_ind_t;

/**
 * @struct    rtk_bt_gatts_read_ind_t
 * @brief     Bluetooth GATT Server received remote client read request event msg.
 */
typedef struct {
	uint16_t app_id;                        /*!< Application identifier of the service owning the attribute. */
	uint16_t conn_handle;                   /*!< Connection handle of the ACL connection associated with the received read request.
                                             *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;                           /*!< L2CAP channel identifier of the request received. */
	uint16_t index;                         /*!< Attribute index within the service. */
	uint16_t offset;                        /*!< Offset into the attribute value for blob reads. */
} rtk_bt_gatts_read_ind_t;

/**
 * @enum      rtk_bt_gatts_write_type_t
 * @brief     Bluetooth GATT Server received remote write request type definition.
 */
typedef enum {
	RTK_BT_GATTS_WRITE_REQ,                 /*!< GATT client write request. */
	RTK_BT_GATTS_WRITE_NO_RESP,             /*!< GATT client write cmd. */
	RTK_BT_GATTS_WRITE_NO_RESP_SIGNED,      /*!< GATT client write cmd (signed). */
	RTK_BT_GATTS_WRITE_LONG,                /*!< GATT client write long. */
} rtk_bt_gatts_write_type_t;


/**
 * @struct    rtk_bt_gatts_write_ind_t
 * @brief     Bluetooth GATT Server received remote client write request event msg.
 */
typedef struct {
	uint16_t app_id;                        /*!< Application identifier of the service owning the attribute. */
	uint16_t conn_handle;                   /*!< Connection handle of the ACL connection associated with the received write result.
                                             *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;                           /*!< L2CAP channel identifier of the request received. */
	uint16_t index;                         /*!< Attribute index within the service. */
	uint16_t offset;                        /*!< Byte offset into the attribute value for queued write operations. */
	rtk_bt_gatts_write_type_t  type;        /*!< Write type. */
	uint16_t len;                           /*!< Length of the write data. */
	uint8_t *value;                         /*!< Pointer to the write data payload. */
} rtk_bt_gatts_write_ind_t;

/**
 * @struct    rtk_bt_gatts_cccd_ind_t
 * @brief     Bluetooth GATT Server cccd updated event msg.
 */
typedef struct {
	uint16_t app_id;                        /*!< Application identifier of the service owning the CCCD. */
	uint16_t conn_handle;                   /*!< Connection handle identifying the client that updated the CCCD.
                                             *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;                           /*!< L2CAP channel identifier of the request received. */
	uint16_t index;                         /*!< Attribute index of the CCCD within the service. */
	uint16_t value;                         /*!< Updated CCCD value written by the remote client. */
} rtk_bt_gatts_cccd_ind_t;

/**
 * @struct    rtk_bt_gatts_ntf_and_ind_ind_t
 * @brief     Bluetooth GATT Server send notify or indicate complete msg.
 */
typedef struct {
	uint16_t app_id;                        /*!< Application identifier of the service that sent the notification or indication. */
	uint16_t conn_handle;                   /*!< Connection handle identifying the client to which the notification or indication was sent.
                                             *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;                           /*!< L2CAP channel identifier for sending notify or indicate. */
	uint16_t index;                         /*!< Attribute index within the service. */
	uint16_t seq;                           /*!< Sequence number for convenience tracking; not mandatory. */
	uint16_t err_code;                      /*!< Status of the send notify or indicate procedure. 0x00 on success. */
} rtk_bt_gatts_ntf_and_ind_ind_t;

/**
 * @struct    rtk_bt_gatts_client_supported_features_ind_t
 * @brief     Bluetooth GATT Server Client Supported Features Characteristic written event msg.
 */
typedef struct {
	uint16_t conn_handle;                   /*!< Connection handle.
                                             *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;                           /*!< L2CAP channel identifier on which the write procedure was processed. */
	uint8_t features;                       /*!< Client Supported Features after being written, which is a bit combination of @ref rtk_bt_gatts_client_supported_features_t. */
} rtk_bt_gatts_client_supported_features_ind_t;

/**
 * @struct    rtk_bt_gatts_service_changed_cccd_ind_t
 * @brief     Bluetooth GATT Server Service Changed Characteristic cccd updated event msg.
 */
typedef struct {
	uint16_t conn_handle;                   /*!< Connection handle identifying the client that updated the CCCD.
                                             *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;                           /*!< L2CAP channel identifier on which the CCCD update was received. */
	bool cccd_enable;                       /*!< Indicates whether the Service Changed CCCD is now enabled.
                                             *   - true: CCCD enabled, the client will receive Service Changed indications when attribute handle is changed.
                                             *   - false: CCCD disabled, the client will not receive any Service Changed indications when attribute handle is changed. */
} rtk_bt_gatts_service_changed_cccd_ind_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_gatts BT GATT Server APIs
 * @brief     BT GATT Server related function APIs.
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Register a GATT service and add its attributes to the local ATT database.
 *            The result is reported asynchronously via event @ref RTK_BT_GATTS_EVT_REGISTER_SERVICE.
 * @param[in] param: Service structure describing the service to register.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gatts_register_service(struct rtk_bt_gatt_service *param);

/**
 * @brief     Unregister GATT service.
 * @param[in] param: The service app registered before, same as the param used in @ref rtk_bt_gatts_register_service.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gatts_unregister_service(struct rtk_bt_gatt_service *param);
/**
 * @brief     Send a GATT notification to a connected client for a specified attribute.
 *            The result is reported asynchronously via event @ref RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND.
 * @param[in] param: The parameters for notification.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - param->len must not exceed ATT_MTU - 3 bytes; can use @ref rtk_bt_le_gap_get_mtu_size to get the negotiated MTU.
 *            - param->index is the zero-based attribute index in the registered service attribute table, not the ATT handle.
 *            - Multiple notifications can be queued.
 */
uint16_t rtk_bt_gatts_notify(rtk_bt_gatts_ntf_and_ind_param_t *param);

/**
 * @brief     Send a GATT indication to a connected client for a specified attribute.
 *            The result is reported asynchronously via event @ref RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND.
 * @param[in] param: The parameters for indication.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - param->len must not exceed ATT_MTU - 3 bytes; can use @ref rtk_bt_le_gap_get_mtu_size to get the negotiated MTU.
 *            - param->index is the zero-based attribute index in the registered service attribute table, not the ATT handle.
 *            - Multiple indications can be queued.
 */
uint16_t rtk_bt_gatts_indicate(rtk_bt_gatts_ntf_and_ind_param_t *param);

/**
 * @brief     Send an ATT read response to a connected client for a read request.
 *            This function must be called in response to each @ref RTK_BT_GATTS_EVT_READ_IND event received by the application.
 * @param[in] param: The response parameters for a read request.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The read response must be sent within the ATT transaction timeout (~30 s); otherwise the remote client may drop the connection.
 *            - param->cid must match the cid field from @ref rtk_bt_gatts_read_ind_t.
 */
uint16_t rtk_bt_gatts_read_resp(rtk_bt_gatts_read_resp_param_t *param);

/**
 * @brief     Send an ATT Write Response (or ATT Error Response) to a client's write.
 *            This function must be called in response to each @ref RTK_BT_GATTS_EVT_WRITE_IND event.
 * @param[in] param: The response parameters for a write request.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The write response must be sent within the ATT transaction timeout (~30 s).
 *            - param->cid must match the cid field from @ref rtk_bt_gatts_write_ind_t.
 *            - The response must use the same parameters(conn_handle/app_id/cid/index/type) as the triggering event.
 *            - When param->type is @ref RTK_BT_GATTS_WRITE_NO_RESP or @ref RTK_BT_GATTS_WRITE_NO_RESP_SIGNED, this function will directly return,
 *              no response will be sent.
 */
uint16_t rtk_bt_gatts_write_resp(rtk_bt_gatts_write_resp_param_t *param);

/**
 * @brief     Send a Service Changed indication to a connected client. The indication alerts the client that server attributes within
 *            the specified handle range have changed and rediscovery of those attributes may be required.
 * @param[in] conn_handle: Connection handle identifying the target client.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] cid: L2CAP channel identifier for sending the indication.
 *                 0 selects the channel automatically.
 *                 Ignored when RTK_BLE_MGR_LIB is 0.
 * @param[in] start_handle: Start of affected attribute handle range.
 * @param[in] end_handle: End of affected attribute handle range.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Only the built-in GATT service supports sending this indication.
 *            - The client must have previously enabled the Service Changed CCCD.
 */
uint16_t rtk_bt_gatts_service_changed_indicate(uint16_t conn_handle, uint16_t cid,
											   uint16_t start_handle, uint16_t end_handle);

/**
 * @brief     Get the absolute attribute handle for a given service and attribute index. The returned handle can be used directly
 *            as the attribute identifier in notification and indication parameters.
 * @param[in] app_id: Every service has an app_id.
 * @param[in] attr_index: Zero-based index of the attribute within the service.
 * @param[out] attr_handle: Pointer to store the retrieved attribute handle.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - attr_index is the zero-based position of the attribute in the service's attribute table, not the ATT handle.
 *            - Before calling this function, the corresponding service must have already been registered;
 *              the application must have received the event @ref RTK_BT_GATTS_EVT_REGISTER_SERVICE.
 */
uint16_t rtk_bt_gatts_get_attribute_handle(uint16_t app_id, uint16_t attr_index, uint16_t *attr_handle);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_GATTS_H__ */
