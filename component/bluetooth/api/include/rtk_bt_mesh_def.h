
#ifndef __RTK_BT_MESH_DEF_H__
#define __RTK_BT_MESH_DEF_H__

#include <stdint.h>
#include <bt_api_config.h>

#ifndef _PACKED4_
#define _PACKED4_            __attribute__ ((packed))
#endif

#ifndef _SHORT_ENUM_
#define _SHORT_ENUM_        __attribute__ ((packed))
#endif

#define ACCESS_OPCODE_SIZE(opcode)              ((opcode) >= 0xc00000 ? 3 : ((opcode) >= 0x8000 ? 2 : 1))

#define RTK_BT_MESH_EVT_MEM_OP_MAX_SIZE    0xffffffff

#define COMPANY_ID        0x005D
#define PRODUCT_ID        0x0000
#define VERSION_ID        0x0000

/**
 * @name      Mesh Default Configuration Parameters
 * @brief     Default configuration parameters of the mesh node.
 * @{
 */
#define BT_MESH_CONFIG_NET_TRANS_COUNTS          6
#define BT_MESH_CONFIG_NET_TRANS_STEPS           0
#define BT_MESH_CONFIG_RELAY_IS_ENABL            1
#define BT_MESH_CONFIG_RELAY_RETRANS_COUNTS      2
#define BT_MESH_CONFIG_RELAY_RETRANS_STEPS       0
#define BT_MESH_CONFIG_TRANS_RETRANS_COUNTS      4
#define BT_MESH_CONFIG_MSG_TTL                   5
/** @} */

/**
 * @name      Mesh 1.1 Features
 * @brief     Switches of the features introduced in the Mesh 1.1.
 * @{
 */
#define BT_MESH_ENABLE_DIRECTED_FORWARDING                             1
#define BT_MESH_ENABLE_SUBNET_BRIDGE                                   1
#define BT_MESH_ENABLE_PRIVATE_BEACON                                  1
#define BT_MESH_ENABLE_EPA_PROVISION                                   1
#define BT_MESH_ENABLE_REMOTE_PROVISION                                1
/** @} */

#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING) && BT_MESH_ENABLE_DIRECTED_FORWARDING
/**
 * @name      Mesh DF Default Configuration Parameters
 * @brief     Default configuration parameters of the Directed Forwarding.
 * @{
 */
#define BT_MESH_CONFIG_DF_ACCESS_NET_TRANS_COUNTS          6
#define BT_MESH_CONFIG_DF_ACCESS_NET_TRANS_STEPS           0
#define BT_MESH_CONFIG_DF_ACCESS_RELAY_RETRANS_COUNTS      2
#define BT_MESH_CONFIG_DF_ACCESS_RELAY_RETRANS_STEPS       0
#define BT_MESH_CONFIG_DF_CTL_NET_TRANS_COUNTS             6
#define BT_MESH_CONFIG_DF_CTL_NET_TRANS_STEPS              0
#define BT_MESH_CONFIG_DF_CTL_RELAY_RETRANS_COUNTS         2
#define BT_MESH_CONFIG_DF_CTL_RELAY_RETRANS_STEPS          0
/** @} */
#endif

/**
 * @name      Mesh Models Enable Configuration
 * @brief     Switches of the mesh models.
 * @{
 */
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING) && BT_MESH_ENABLE_DIRECTED_FORWARDING
#define BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL                1
#endif
#if defined(BT_MESH_ENABLE_SUBNET_BRIDGE) && BT_MESH_ENABLE_SUBNET_BRIDGE
#define BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL                      1
#endif
#if defined(BT_MESH_ENABLE_PRIVATE_BEACON) && BT_MESH_ENABLE_PRIVATE_BEACON
#define BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL                     1
#endif
#if defined(BT_MESH_ENABLE_REMOTE_PROVISION) && BT_MESH_ENABLE_REMOTE_PROVISION
#define BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL                1
#endif
#define BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL                     1
#define BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL                      1
#define BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL    1
#define BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL                1
#define BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL                1
#define BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL                    1
#define BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL                   1
#define BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL                   1
#define BT_MESH_ENABLE_SENSOR_CLIENT_MODEL                             1
#define BT_MESH_ENABLE_TIME_CLIENT_MODEL                               1
#define BT_MESH_ENABLE_SCENE_CLIENT_MODEL                              1
#define BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL                          1
#define BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL                    1
#define BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL                          1
#define BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL                          1
#define BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL                          1
#define BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL                           1
#define BT_MESH_ENABLE_DFU_INITIATOR_ROLE                              0  /*!< initiator and standalone updater can not enable at same time */
#define BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE                     1  /*!< initiator and standalone updater can not enable at same time */
#endif

#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
#if defined(BT_MESH_ENABLE_REMOTE_PROVISION) && BT_MESH_ENABLE_REMOTE_PROVISION
#define BT_MESH_ENABLE_REMOTE_PROVISIONING_SERVER_MODEL                1
#endif
#define BT_MESH_ENABLE_GENERIC_ON_OFF_SERVER_MODEL                     1
#define BT_MESH_ENABLE_GENERIC_LEVEL_SERVER_MODEL                      1
#define BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSTION_TIME_SERVER_MODEL     1
#define BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SERVER_MODEL               1
#define BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL         1
#define BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL                1
#define BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL          1
#define BT_MESH_ENABLE_GENERIC_BATTERY_SERVER_MODEL                    1
#define BT_MESH_ENABLE_GENERIC_LOCATION_SERVER_MODEL                   1
#define BT_MESH_ENABLE_GENERIC_LOCATION_SETUP_SERVER_MODEL             1
#define BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL             1
#define BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL      1
#define BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL              1
#define BT_MESH_ENABLE_GENERIC_CLIENT_PROPERTY_SERVER_MODEL            1
#define BT_MESH_ENABLE_SENSOR_SERVER_MODEL                             1
#define BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL                       1
#define BT_MESH_ENABLE_TIME_SERVER_MODEL                               1
#define BT_MESH_ENABLE_TIME_SETUP_SERVER_MODEL                         1
#define BT_MESH_ENABLE_SCENE_SERVER_MODEL                              1
#define BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL                        1
#define BT_MESH_ENABLE_SCHEDULER_SERVER_MODEL                          1
#define BT_MESH_ENABLE_SCHEDULER_SETUP_SERVER_MODEL                    1
#define BT_MESH_ENABLE_LIGHT_LIGHTNESS_SERVER_MODEL                    1
#define BT_MESH_ENABLE_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL              1
#define BT_MESH_ENABLE_LIGHT_CTL_SERVER_MODEL                          1
#define BT_MESH_ENABLE_LIGHT_CTL_SETUP_SERVER_MODEL                    1
#define BT_MESH_ENABLE_LIGHT_CTL_TEMPERATURE_SERVER_MODEL              1
#define BT_MESH_ENABLE_LIGHT_HSL_SERVER_MODEL                          1
#define BT_MESH_ENABLE_LIGHT_HSL_SETUP_SERVER_MODEL                    1
#define BT_MESH_ENABLE_LIGHT_HSL_HUE_SERVER_MODEL                      1
#define BT_MESH_ENABLE_LIGHT_HSL_SATURATION_SERVER_MODEL               1
#define BT_MESH_ENABLE_LIGHT_XYL_SERVER_MODEL                          1
#define BT_MESH_ENABLE_LIGHT_XYL_SETUP_SERVER_MODEL                    1
#define BT_MESH_ENABLE_LIGHT_LC_SERVER_MODEL                           1
#define BT_MESH_ENABLE_LIGHT_LC_SETUP_SERVER_MODEL                     1
#define BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE                            0  /*!< distributor and target can not enable at same time */
#define BT_MESH_ENABLE_DFU_TARGET_ROLE                                 1  /*!< distributor and target can not enable at same time */
#endif

#define BT_MESH_ENABLE_DATATRANS_MODEL                                 1
/** @} */


/**
 * @name      Mesh Provisioning Capability Settings
 * @brief     Provisioning capability settings of the device, and the Static OOB authentication value.
 * @{
 */
#define PROV_SUPPORT_STATIC_OOB           0    /*!< Whether support static oob for provisining */
#if defined(BT_MESH_ENABLE_EPA_PROVISION) && BT_MESH_ENABLE_EPA_PROVISION
#define PROV_SUPPORT_OOB_AUTH_ONLY        0    /*!< Whether only the OOB authenticated provisioning is supported */
#endif
#define PROV_SUPPORT_OUTPUT_OOB_ACTION    RTK_BT_MESH_PROV_CAP_OUTPUT_OOB_ACTION_NOT_ENABLE    /*!< @ref rtk_bt_mesh_stack_prov_cap_output_oob_action_t */
#define PROV_SUPPORT_OUTPUT_OOB_SIZE      0    /*!< The bytes store random number for mesh stack */
#define PROV_SUPPORT_INPUT_OOB_ACTION     RTK_BT_MESH_PROV_CAP_INPUT_OOB_ACTION_NOT_ENABLE    /*!< @ref rtk_bt_mesh_stack_prov_cap_input_oob_action_t */
#define PROV_SUPPORT_INPUT_OOB_SIZE       0    /*!< The bytes store random number for mesh stack */

#define STATIC_OOB_VALUE_FOR_AUTHENTICATION    {0x1,0x2,0x3}    /*!< Static OOB authentication value, the remaining bytes of the 16 bytes are 0 */
/** @} */

/**
 * @enum      rtk_bt_mesh_msg_send_result
 * @brief     BLE MESH access message send result definition.
 */
typedef enum {
	RTK_BT_MESH_MSG_SEND_CAUSE_SUCCESS,                      /*!< The access msg is successfully processed by the stack. */
	RTK_BT_MESH_MSG_SEND_CAUSE_INVALID_ACCESS_OPCODE,        /*!< The access msg shall be formatted with a valid access opcode as defined in the spec. */
	RTK_BT_MESH_MSG_SEND_CAUSE_INVALID_ELEMENT,              /*!< The model sends the access msg with an invalid element index.
                                                              *   It may be due to the model is unregistered. */
	RTK_BT_MESH_MSG_SEND_CAUSE_INVALID_MODEL,                /*!< The model sends the access msg with an invalid model index.
                                                              *   It may be due to the model is unregistered. */
	RTK_BT_MESH_MSG_SEND_CAUSE_NODE_UNPROVISIONED,           /*!< An unprovisioned device can not send an access msg. */
	RTK_BT_MESH_MSG_SEND_CAUSE_INVALID_SRC,                  /*!< The app may send an access msg without a model, so it needs to appoint the src itself.
                                                              *   In this case, the stack checks the validity of the src address. */
	RTK_BT_MESH_MSG_SEND_CAUSE_INVALID_DST,                  /*!< The dst address is assigned by the app, and the stack checks its validity.
                                                              *   - The dst address can not be the unassigned address.
                                                              *   - When the msg is encrypted by the device key, the device key shall exist. */
	RTK_BT_MESH_MSG_SEND_CAUSE_PAYLOAD_SIZE_EXCEED,          /*!< The access pdu exceeds the maximum size @ref ACCESS_PAYLOAD_MAX_SIZE.
                                                              *   Depending on the msg size, the access msg is sent unsegmented or segmented. */
	RTK_BT_MESH_MSG_SEND_CAUSE_PAYLOAD_SIZE_EXCEED1,         /*!< The access pdu exceeds the maximum size when the SZMIC bit is set to 1.
                                                              *   If the SZMIC bit is 0, the TransMIC is a 32-bit value, otherwise a 64-bit value.
                                                              *   When the SZMIC bit is 1, the maximum payload size of a segmented access pdu is reduced by 4. */
	RTK_BT_MESH_MSG_SEND_CAUSE_INVALID_VIR_ADDR,             /*!< The virtual address is invalid. */
	RTK_BT_MESH_MSG_SEND_CAUSE_INVALID_APP_KEY_INDEX,        /*!< The AppKey index exceeds the upper limit @ref mesh_node.app_key_num. */
	RTK_BT_MESH_MSG_SEND_CAUSE_INVALID_APP_KEY_STATE,        /*!< The AppKey has not been added. */
	RTK_BT_MESH_MSG_SEND_CAUSE_APP_KEY_NOT_BOUND_TO_MODEL,   /*!< The AppKey has not been bound to the model when sending the msg with a model. */
	RTK_BT_MESH_MSG_SEND_CAUSE_INVALID_NET_KEY_INDEX,        /*!< The NetKey index exceeds the upper limit @ref mesh_node.net_key_num. */
	RTK_BT_MESH_MSG_SEND_CAUSE_INVALID_NET_KEY_STATE,        /*!< The NetKey has not been added. */
	RTK_BT_MESH_MSG_SEND_CAUSE_NO_BUFFER_AVAILABLE,          /*!< The mesh msg is sent via the advertising channel, which takes some resource and time.
                                                              *   When the resource is exhausted currently, the msg can not be sent. */
	RTK_BT_MESH_MSG_SEND_CAUSE_NO_MEMORY,                    /*!< The heap space which needs to be allocated in some cases is exhausted. */
	RTK_BT_MESH_MSG_SEND_CAUSE_TRANS_TX_BUSY,                /*!< The segmented msg needs to be buffered at the transport layer to retransmit.
                                                              *   The buffer size at the transport layer is limited, so a new segmented msg
                                                              *   can not be sent when the buffer is full. */
	RTK_BT_MESH_MSG_SEND_CAUSE_INVALID_ACCESS_PARAMETER,     /*!< The access parameter is invalid. */
	RTK_BT_MESH_MSG_SEND_CAUSE_FAIL,                         /*!< Other failure. */
} rtk_bt_mesh_msg_send_result;

/**
 * @struct    rtk_bt_mesh_app_conf_t
 * @brief     BLE MESH app configuration structure.
 */
typedef struct {
	uint8_t bt_mesh_role;                    /*!< Role of the mesh node, see @ref rtk_bt_mesh_stack_role_t. */
	uint8_t bt_mesh_uuid[16];                /*!< Device UUID of the mesh node. */
	uint32_t bt_mesh_flash_size;             /*!< Size in bytes of the flash used to store the mesh data. */
	uint16_t bt_mesh_rpl_num;                /*!< Number of entries of the Replay Protection List. */
	uint8_t trans_retrans_count;             /*!< Transport Transmit Count, the number of retransmissions of each segment of a segmented message. */
	uint8_t net_trans_count: 3;              /*!< Network Transmit Count. */
	uint8_t net_trans_interval_step: 5;      /*!< Network Transmit Interval Steps. */
	uint8_t relay_retrans_count: 3;          /*!< Relay Retransmit Count. */
	uint8_t relay_retrans_interval_step: 5;  /*!< Relay Retransmit Interval Steps. */
	uint8_t ttl;                             /*!< Default TTL. */
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t bt_mesh_dev_key_num;            /*!< Number of device keys that a provisioner can store. */
#endif
} rtk_bt_mesh_app_conf_t;

/**
 * @enum      rtk_bt_mesh_generic_stat_t
 * @brief     BLE MESH status code of the range states of the Generic and Light models.
 */
typedef enum {
	MESH_GENERIC_STAT_SUCCESS,                 /*!< The command is successfully processed. */
	MESH_GENERIC_STAT_CANNOT_SET_RANGE_MIN,    /*!< The range minimum value cannot be set. */
	MESH_GENERIC_STAT_CANNOT_SET_RANGE_MAX     /*!< The range maximum value cannot be set. */
} rtk_bt_mesh_generic_stat_t;

/**
 * @enum      rtk_bt_mesh_msg_subtype
 * @brief     BLE MESH subtype of T_IO_MSG for IO_MSG_TYPE_LE_MESH.
 */
typedef enum {
	RTK_BT_MESH_IO_MSG_SUBTYPE_ADV,                           /*!< Send the one shot advertising. */
	RTK_BT_MESH_IO_MSG_SUBTYPE_BLOB_CLIENT_CHUNK_TRANSFER,    /*!< Start the chunk transfer of the BLOB client. */
	RTK_BT_MESH_IO_MSG_SUBTYPE_BLOB_CLIENT_PROCEDURE,         /*!< The procedure timeout of the BLOB client. */
	RTK_BT_MESH_IO_MSG_SUBTYPE_BLOB_CLIENT_RETRY,             /*!< The retry timeout of the BLOB client. */
	RTK_BT_MESH_IO_MSG_SUBTYPE_DFU_DIST_APP_TIMEOUT_MSG,      /*!< The timeout of the DFU Distributor application. */
	RTK_BT_MESH_IO_MSG_SUBTYPE_DFU_INIT_APP_TIMEOUT_MSG,      /*!< The timeout of the DFU Initiator application. */
} rtk_bt_mesh_msg_subtype;

/**
 * @struct    rtk_bt_mesh_addr_range_t
 * @brief     BLE MESH unicast address range structure, as in the unicast address range format of the spec.
 */
typedef struct {
	union {
		struct {
			uint16_t range_start : 15;    /*!< 15 least significant bits of the starting unicast address of the range. */
			uint16_t len_present : 1;     /*!< Whether the range contains more than one address.
                                           *   - true: The range contains more than one address, range_len is present.
                                           *   - false: The range contains a single address, range_len is not present. */
		};
		struct {
			uint16_t len_present_access : 1;     /*!< Same as len_present, as laid out in the bytes of an access message. */
			uint16_t range_start_access : 15;    /*!< Same as range_start, as laid out in the bytes of an access message. */
		};
		uint16_t addr;    /*!< Raw 16-bit value of the first two bytes. */
	};
	uint8_t range_len;    /*!< Number of addresses in the range, in the range 0x02 to 0xFF.
                           *   It only takes up a byte of the access message when the length is present. */
} _PACKED4_ rtk_bt_mesh_addr_range_t;

/**
 * @defgroup  ble_mesh_api_group BT LE Mesh APIs
 * @brief     List all BT LE Mesh related APIs
 * @ingroup   BT_APIs
 */

#endif /* __RTK_BT_MESH_DEF_H__ */
