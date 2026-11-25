/**
 * @file      rtk_bt_mesh_config_client_model.h
 * @author    pico_zhai@realsil.com.cn
 * @brief     Bluetooth LE MESH configuration client model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_CONFIG_CLIENT_MODEL_H__
#define __RTK_BT_MESH_CONFIG_CLIENT_MODEL_H__

#include <stdint.h>
#include <rtk_bt_mesh_def.h>

/********************************* Config Client Model *********************************/
/** @addtogroup BT_Types BT Types
 * @{
 */
/** @addtogroup BT_Mesh_Types BT Mesh Types
 * @{
 */
/** @addtogroup BT_Mesh_Config_Client_Model_Types BT Mesh Config Client Model Types
 * @{
 */
/**
 * @enum      rtk_bt_mesh_config_client_api_t
 * @brief     BLE MESH config client model act and event definition.
 */
typedef enum {
	RTK_BT_MESH_CONFIG_MODEL_ADD_APP_KEY = 1,           /*!< Send a Config AppKey Add message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_MODEL_APP_BIND,            /*!< Send a Config Model App Bind message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_ADD,             /*!< Send a Config Model Subscription Add message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_DEFAULT_TTL_GET,           /*!< Send a Config Default TTL Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_DEFAULT_TTL_SET,           /*!< Send a Config Default TTL Set message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_DEFAULT_TTL_STAT,          /*!< Indicate a Config Default TTL Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_default_ttl_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_RELAY_GET,                 /*!< Send a Config Relay Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_RELAY_SET,                 /*!< Send a Config Relay Set message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_RELAY_STAT,                /*!< Indicate a Config Relay Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_relay_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_NET_TRANSMIT_GET,          /*!< Send a Config Network Transmit Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_NET_TRANSMIT_SET,          /*!< Send a Config Network Transmit Set message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_NET_TRANSMIT_STAT,         /*!< Indicate a Config Network Transmit Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_net_transmit_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_NODE_RESET,                /*!< Send a Config Node Reset message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_NODE_RESET_STAT,           /*!< Indicate a Config Node Reset Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_config_client_model_node_reset_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_COMPO_DATA_GET,            /*!< Send a Config Composition Data Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_BEACON_GET,                /*!< Send a Config Beacon Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_BEACON_SET,                /*!< Send a Config Beacon Set message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_PROXY_GET,                 /*!< Send a Config GATT Proxy Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_PROXY_SET,                 /*!< Send a Config GATT Proxy Set message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_MODEL_PUB_GET,             /*!< Send a Config Model Publication Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_MODEL_PUB_SET,             /*!< Send a Config Model Publication Set message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_DELETE,          /*!< Send a Config Model Subscription Delete message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_DELETE_ALL,      /*!< Send a Config Model Subscription Delete All message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_OVERWRITE,       /*!< Send a Config Model Subscription Overwrite message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_GET,             /*!< Send a Config SIG Model Subscription Get or Config Vendor Model Subscription Get message to the remote
                                                         *   device. */
	RTK_BT_MESH_CONFIG_MODEL_NET_KEY_ADD,               /*!< Send a Config NetKey Add message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_NET_KEY_UPDATE,            /*!< Send a Config NetKey Update message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_NET_KEY_DELETE,            /*!< Send a Config NetKey Delete message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_NET_KEY_GET,               /*!< Send a Config NetKey Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_APP_KEY_UPDATE,            /*!< Send a Config AppKey Update message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_APP_KEY_DELETE,            /*!< Send a Config AppKey Delete message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_APP_KEY_GET,               /*!< Send a Config AppKey Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_NODE_IDENTITY_GET,         /*!< Send a Config Node Identity Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_NODE_IDENTITY_SET,         /*!< Send a Config Node Identity Set message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_MODEL_APP_UNBIND,          /*!< Send a Config Model App Unbind message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_MODEL_APP_GET,             /*!< Send a Config SIG Model App Get or Config Vendor Model App Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_FRND_GET,                  /*!< Send a Config Friend Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_FRND_SET,                  /*!< Send a Config Friend Set message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_LPN_POLL_TIMEOUT_GET,      /*!< Send a Config Low Power Node PollTimeout Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_KEY_REFRESH_PHASE_GET,     /*!< Send a Config Key Refresh Phase Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_KEY_REFRESH_PHASE_SET,     /*!< Send a Config Key Refresh Phase Set message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_HB_PUB_GET,                /*!< Send a Config Heartbeat Publication Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_HB_PUB_SET,                /*!< Send a Config Heartbeat Publication Set message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_HB_SUB_GET,                /*!< Send a Config Heartbeat Subscription Get message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_HB_SUB_SET,                /*!< Send a Config Heartbeat Subscription Set message to the Configuration Server. */
	RTK_BT_MESH_CONFIG_MODEL_BEACON_STAT,               /*!< Indicate a Config Beacon Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_beacon_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_COMPO_DATA_STAT,           /*!< Indicate a Config Composition Data Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_compo_data_stat_call_t. */
	RTK_BT_MESH_CONFIG_MODEL_PROXY_STAT,                /*!< Indicate a Config GATT Proxy Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_proxy_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_MODEL_PUB_STAT,            /*!< Indicate a Config Model Publication Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_model_pub_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_STAT,            /*!< Indicate a Config Model Subscription Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_model_sub_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_SIG_MODEL_SUB_LIST,        /*!< Indicate a Config SIG Model Subscription List message received from the Configuration Server.
                                                         *   The msg is a byte stream filled in the order below, there is no structure for it:
                                                         *   - Status code, 2 bytes.
                                                         *   - Unicast address of the Configuration Server that sent the message, 2 bytes.
                                                         *   - Length in bytes of the whole msg, 4 bytes.
                                                         *   - Model ID, 4 bytes, only if the status code is success.
                                                         *   - Element index, 2 bytes, only if the status code is success.
                                                         *     It is the address of the element that contains the model
                                                         *     minus the unicast address of the Configuration Server.
                                                         *   - Number of addresses, 2 bytes, only if the status code is success.
                                                         *   - Subscription addresses, 2 bytes each, only if the status code is success. */
	RTK_BT_MESH_CONFIG_MODEL_VENDOR_MODEL_SUB_LIST,     /*!< Indicate a Config Vendor Model Subscription List message received from the Configuration Server.
                                                         *   The msg has the same layout as the one of event @ref RTK_BT_MESH_CONFIG_MODEL_SIG_MODEL_SUB_LIST. */
	RTK_BT_MESH_CONFIG_MODEL_NET_KEY_STAT,              /*!< Indicate a Config NetKey Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_net_key_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_NET_KEY_LIST,              /*!< Indicate a Config NetKey List message received from the Configuration Server.
                                                         *   The msg is a byte stream filled in the order below, there is no structure for it:
                                                         *   - Valid flag, 2 bytes, 1 if the list is valid, otherwise 0.
                                                         *   - Number of NetKey Indexes if the list is valid, otherwise the length in bytes of the received list, 2 bytes.
                                                         *   - Length in bytes of the whole msg, 4 bytes.
                                                         *   - NetKey Indexes, 2 bytes each, only if the list is valid. */
	RTK_BT_MESH_CONFIG_MODEL_APP_KEY_STAT,              /*!< Indicate a Config AppKey Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_app_key_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_APP_KEY_LIST,              /*!< Indicate a Config AppKey List message received from the Configuration Server.
                                                         *   The msg is a byte stream filled in the order below, there is no structure for it:
                                                         *   - Valid flag, 2 bytes, 1 if the list is valid, otherwise 0.
                                                         *   - NetKey Index of the list if the list is valid, otherwise the status code, 2 bytes.
                                                         *   - Length in bytes of the whole msg, 4 bytes.
                                                         *   - Number of AppKey Indexes if the list is valid, otherwise the length in bytes of the received list, 2 bytes.
                                                         *   - AppKey Indexes, 2 bytes each, only if the list is valid. */
	RTK_BT_MESH_CONFIG_MODEL_NODE_IDENTITY_STAT,        /*!< Indicate a Config Node Identity Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_node_identity_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_MODEL_APP_STAT,            /*!< Indicate a Config Model App Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_model_app_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_SIG_MODEL_APP_LIST,        /*!< Indicate a Config SIG Model App List message received from the Configuration Server.
                                                         *   The msg is a byte stream filled in the order below, there is no structure for it:
                                                         *   - Valid flag, 2 bytes, 1 if the list is valid, otherwise 0.
                                                         *   - Element index if the list is valid, otherwise the status code, 2 bytes.
                                                         *     The element index is the address of the element that contains the model
                                                         *     minus the unicast address of the Configuration Server.
                                                         *   - Length in bytes of the whole msg, 4 bytes.
                                                         *   - Model ID, 4 bytes, only if the list is valid.
                                                         *   - Number of AppKey Indexes, 2 bytes, only if the list is valid.
                                                         *   - AppKey Indexes, 2 bytes each, only if the list is valid. */
	RTK_BT_MESH_CONFIG_MODEL_VENDOR_MODEL_APP_LIST,     /*!< Indicate a Config Vendor Model App List message received from the Configuration Server.
                                                         *   The msg has the same layout as the one of event @ref RTK_BT_MESH_CONFIG_MODEL_SIG_MODEL_APP_LIST. */
	RTK_BT_MESH_CONFIG_MODEL_FRND_STAT,                 /*!< Indicate a Config Friend Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_frnd_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_LPN_POLL_TO_STAT,          /*!< Indicate a Config Low Power Node PollTimeout Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_lpn_poll_timeout_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_KEY_REFRESH_PHASE_STAT,    /*!< Indicate a Config Key Refresh Phase Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_key_refresh_phase_stat_call_t. */
	RTK_BT_MESH_CONFIG_MODEL_HB_PUB_STAT,               /*!< Indicate a Config Heartbeat Publication Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_hb_pub_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_HB_SUB_STAT,               /*!< Indicate a Config Heartbeat Subscription Status message received from the Configuration Server, with msg
                                                         *   @ref rtk_bt_mesh_cfg_hb_sub_stat_t. */
	RTK_BT_MESH_CONFIG_MODEL_MAX,                       /*!< Reserved. */
} rtk_bt_mesh_config_client_api_t;

/**
 * @union     rtk_bt_mesh_hb_pub_features_t
 * @brief     BLE MESH Config Pub Features definition.
 */
typedef union {
	uint16_t raw;                /*!< All the feature bits as one 16-bit value. */
	struct {
		uint16_t relay: 1;    /*!< Relay feature change trigger.
                               *   0: The change of the Relay feature does not trigger a Heartbeat message.
                               *   1: The change of the Relay feature triggers a Heartbeat message. */
		uint16_t proxy: 1;    /*!< Proxy feature change trigger.
                               *   0: The change of the Proxy feature does not trigger a Heartbeat message.
                               *   1: The change of the Proxy feature triggers a Heartbeat message. */
		uint16_t frnd: 1;     /*!< Friend feature change trigger.
                               *   0: The change of the Friend feature does not trigger a Heartbeat message.
                               *   1: The change of the Friend feature triggers a Heartbeat message. */
		uint16_t lpn: 1;      /*!< Low Power feature change trigger.
                               *   0: The change of the Low Power feature does not trigger a Heartbeat message.
                               *   1: The change of the Low Power feature triggers a Heartbeat message. */
		uint16_t rfu: 12;     /*!< Reserved for future use. */
	};
} _PACKED4_ rtk_bt_mesh_hb_pub_features_t;

/**
 * @struct    rtk_bt_mesh_pub_key_info_t
 * @brief     BLE MESH Config Pub Key Information definition.
 */
typedef struct {
	union {
		uint16_t value;                    /*!< AppKey Index, Friendship Credential Flag and RFU as one 16-bit value. */
		struct {
			uint16_t app_key_index: 12;    /*!< Index of the application key used to publish messages.
                                            *   Range: 0x0000 to 0x0FFF. */
			uint16_t frnd_flag: 1;         /*!< Friendship Credential Flag, the credentials used to publish messages.
                                            *   0: Managed flooding security credentials or directed security credentials are used.
                                            *   1: Friendship security credentials are used. */
			uint16_t rfu: 3;               /*!< Reserved for future use. */
		};
	};
} _PACKED4_ rtk_bt_mesh_pub_key_info_t;

/**
 * @struct    rtk_bt_mesh_pub_period_t
 * @brief     BLE MESH Config Pub Period definition.
 */
typedef struct {
	uint8_t steps : 6;    /*!< Number of steps of the Publish Period.
                           *   0x00: Publish Period is disabled.
                           *   0x01 to 0x3F: Number of steps.
                           *   Publish Period = Step Resolution * Number of Steps. */
	uint8_t resol : 2;    /*!< Step Resolution of the Publish Period.
                           *   0b00: 100 milliseconds.
                           *   0b01: 1 second.
                           *   0b10: 10 seconds.
                           *   0b11: 10 minutes. */
} _PACKED4_ rtk_bt_mesh_pub_period_t;

/**
 * @struct    rtk_bt_mesh_pub_retrans_info_t
 * @brief     BLE MESH Config Pub Retrans Information definition.
 */
typedef struct {
	uint8_t count : 3;    /*!< Publish Retransmit Count, the number of times that a published message is retransmitted.
                           *   Range: 0x00 to 0x07.
                           *   0: No retransmission. */
	uint8_t steps : 5;    /*!< Publish Retransmit Interval Steps, the number of 50-millisecond steps between retransmissions.
                           *   Range: 0x00 to 0x1F.
                           *   Retransmission interval = (steps + 1) * 50 ms. */
} _PACKED4_ rtk_bt_mesh_pub_retrans_info_t;

/**
 * @enum      rtk_bt_mesh_msg_stat_t
 * @brief     BLE MESH config client model received msg status definition.
 */
typedef enum {
	RTK_BT_MESH_MSG_STAT_SUCCESS,                     /*!< Status code 0x00: The operation succeeded. */
	RTK_BT_MESH_MSG_STAT_INVALID_ADDR,                /*!< Status code 0x01: Invalid Address. */
	RTK_BT_MESH_MSG_STAT_INVALID_MODEL,               /*!< Status code 0x02: Invalid Model. */
	RTK_BT_MESH_MSG_STAT_INVALID_APP_KEY_INDEX,       /*!< Status code 0x03: Invalid AppKey Index. */
	RTK_BT_MESH_MSG_STAT_INVALID_NET_KEY_INDEX,       /*!< Status code 0x04: Invalid NetKey Index. */
	RTK_BT_MESH_MSG_STAT_INSUFFICIENT_RESOURCES,      /*!< Status code 0x05: Insufficient Resources. */
	RTK_BT_MESH_MSG_STAT_KEY_INDEX_ALREADY_STORED,    /*!< Status code 0x06: Key Index Already Stored. */
	RTK_BT_MESH_MSG_STAT_INVALID_PUB_PARAMS,          /*!< Status code 0x07: Invalid Publish Parameters. */
	RTK_BT_MESH_MSG_STAT_NOT_SUB_MODEL,               /*!< Status code 0x08: Not a Subscribe Model. */
	RTK_BT_MESH_MSG_STAT_STORAGE_FAIL,                /*!< Status code 0x09: Storage Failure. */
	RTK_BT_MESH_MSG_STAT_FEATURE_NOT_SUPPORTED,       /*!< Status code 0x0A: Feature Not Supported. */
	RTK_BT_MESH_MSG_STAT_CANNOT_UPDATE,               /*!< Status code 0x0B: Cannot Update. */
	RTK_BT_MESH_MSG_STAT_CANNOT_REMOVE,               /*!< Status code 0x0C: Cannot Remove. */
	RTK_BT_MESH_MSG_STAT_CANNOT_BIND,                 /*!< Status code 0x0D: Cannot Bind. */
	RTK_BT_MESH_MSG_STAT_TEMP_UNABLE_CHANGE_STATE,    /*!< Status code 0x0E: Temporarily Unable to Change State. */
	RTK_BT_MESH_MSG_STAT_CANNOT_SET,                  /*!< Status code 0x0F: Cannot Set. */
	RTK_BT_MESH_MSG_STAT_UNSPECIFIED_ERROR,           /*!< Status code 0x10: Unspecified Error. */
	RTK_BT_MESH_MSG_STAT_INVALID_BINDING,             /*!< Status code 0x11: Invalid Binding. */
	RTK_BT_MESH_MSG_STAT_INVALID_PATH_ENTRY,          /*!< Status code 0x12: Invalid Path Entry. */
	RTK_BT_MESH_MSG_STAT_CANNOT_GET,                  /*!< Status code 0x13: Cannot Get. */
	RTK_BT_MESH_MSG_STAT_OBSOLETE_INFORMATION,        /*!< Status code 0x14: Obsolete Information. */
	RTK_BT_MESH_MSG_STAT_INVALID_BEARER,              /*!< Status code 0x15: Invalid Bearer. */
} rtk_bt_mesh_msg_stat_t;
/**
 * @struct    rtk_bt_mesh_cfg_app_key_stat_t
 * @brief     BLE MESH Config AppKey Status definition.
 */
typedef struct {
	rtk_bt_mesh_msg_stat_t stat;    /*!< Status code for the requesting message. */
	uint8_t key_index[3];           /*!< NetKey Index and AppKey Index, two 12-bit indexes packed into three octets.
                                     *   Octet 0: bits 0 to 7 of the NetKey Index.
                                     *   Octet 1: bits 0 to 3 are bits 8 to 11 of the NetKey Index, bits 4 to 7 are bits 0 to 3 of the AppKey Index.
                                     *   Octet 2: bits 4 to 11 of the AppKey Index.
                                     *   Range of each index: 0x0000 to 0x0FFF. */
} _PACKED4_ rtk_bt_mesh_cfg_app_key_stat_t;

/**
 * @struct    rtk_bt_mesh_cfg_model_app_stat_t
 * @brief     BLE MESH Config Model App Status definition.
 */
typedef struct {
	rtk_bt_mesh_msg_stat_t stat;    /*!< Status code for the requesting message. */
	uint16_t element_addr;          /*!< Unicast address of the element that contains the model. */
	uint16_t app_key_index;         /*!< Index of the AppKey that is bound to or unbound from the model.
                                     *   Range: 0x0000 to 0x0FFF. */
	uint32_t model_id;              /*!< Identifier of the model, a SIG Model ID (2 bytes) or a Vendor Model ID (4 bytes).
                                     *   It is the value in the received message, so only the low 16 bits are the SIG Model ID for a SIG model. */
} _PACKED4_ rtk_bt_mesh_cfg_model_app_stat_t;

/**
 * @struct    rtk_bt_mesh_cfg_model_sub_stat_t
 * @brief     BLE MESH Config Model Subscription Status definition.
 */
typedef struct {
	rtk_bt_mesh_msg_stat_t stat;    /*!< Status code for the requesting message. */
	uint16_t element_addr;          /*!< Unicast address of the element that contains the model. */
	uint16_t addr;                  /*!< Address that was used to modify the Subscription List, or the unassigned address.
                                     *   For a Label UUID it is the virtual address. */
	uint32_t model_id;              /*!< Identifier of the model, a SIG Model ID (2 bytes) or a Vendor Model ID (4 bytes).
                                     *   It is the value in the received message, so only the low 16 bits are the SIG Model ID for a SIG model. */
} _PACKED4_ rtk_bt_mesh_cfg_model_sub_stat_t;

/**
 * @struct    rtk_bt_mesh_cfg_default_ttl_stat_t
 * @brief     BLE MESH Config Default TTL Status definition.
 */
typedef struct {
	uint8_t ttl;    /*!< Default TTL of the Configuration Server.
                     *   0x00, 0x02 to 0x7F: The Default TTL state.
                     *   0x01, 0x80 to 0xFF: Prohibited. */
} _PACKED4_ rtk_bt_mesh_cfg_default_ttl_stat_t;

/**
 * @struct    rtk_bt_mesh_cfg_relay_stat_t
 * @brief     BLE MESH Config Relay Status message structure.
 */
typedef struct {
	uint8_t state;        /*!< Relay state of the Configuration Server.
                           *   0x00: The Relay feature is supported and disabled.
                           *   0x01: The Relay feature is supported and enabled.
                           *   0x02: The Relay feature is not supported.
                           *   0x03 to 0xFF: Prohibited. */
	uint8_t count : 3;    /*!< Relay Retransmit Count, controls the number of transmissions of each relayed Network PDU.
                           *   Number of transmissions = count + 1.
                           *   Range: 0x00 to 0x07. */
	uint8_t steps : 5;    /*!< Relay Retransmit Interval Steps, the number of 10-millisecond steps between retransmissions.
                           *   Transmission interval = (steps + 1) * 10 ms.
                           *   Range: 0x00 to 0x1F. */
} _PACKED4_ rtk_bt_mesh_cfg_relay_stat_t;

/**
 * @struct    rtk_bt_mesh_cfg_net_transmit_stat_t
 * @brief     BLE MESH Config Network Transmit Status message structure.
 */
typedef struct {
	uint8_t count : 3;    /*!< Network Transmit Count, controls the number of transmissions of each Network PDU originated by the node.
                           *   Number of transmissions = count + 1.
                           *   Range: 0x00 to 0x07. */
	uint8_t steps : 5;    /*!< Network Transmit Interval Steps, the number of 10-millisecond steps between transmissions.
                           *   Transmission interval = (steps + 1) * 10 ms.
                           *   Range: 0x00 to 0x1F. */
} _PACKED4_ rtk_bt_mesh_cfg_net_transmit_stat_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_node_reset_stat_t
 * @brief     BLE MESH Config Node Reset Status message structure.
 */
typedef struct {
	uint16_t src;    /*!< Unicast address of the Configuration Server that sent the Status message. */
} _PACKED4_ rtk_bt_mesh_config_client_model_node_reset_stat_t;

/**
 * @struct    rtk_bt_mesh_config_client_add_app_key_t
 * @brief     BLE MESH Config AppKey Add message structure.
 */
typedef struct {
	uint16_t unicast_addr;     /*!< Unicast address of the Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey that the AppKey is bound to.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t app_key_index;    /*!< Index of the AppKey to add.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_config_client_add_app_key_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_app_bind_t
 * @brief     BLE MESH Config Model App Bind message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Configuration Server. */
	uint16_t element_addr;     /*!< Unicast address of the element that contains the model. */
	uint16_t app_key_index;    /*!< Index of the AppKey to bind to the model.
                                *   Range: 0x0000 to 0x0FFF. */
	uint32_t model_id;         /*!< Model ID of the model.
                                *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                                *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
} rtk_bt_mesh_config_client_model_app_bind_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_sub_add_t
 * @brief     BLE MESH Config Model Subscription Add message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Configuration Server. */
	uint16_t element_addr;     /*!< Unicast address of the element that contains the model. */
	bool va_flag;              /*!< Whether the address to add is a virtual address.
                                *   - true: group_addr is the 16-byte Label UUID of a virtual address.
                                *   - false: the first 2 bytes of group_addr are the address. */
	uint8_t group_addr[16];    /*!< Address to add to the Subscription List.
                                *   If va_flag is true, the 16-byte Label UUID of the virtual address.
                                *   If va_flag is false, the first two bytes are the 16-bit group address in little-endian, the others are not used.
                                *   The unassigned address, a unicast address and the all-nodes address are Prohibited when va_flag is false. */
	uint32_t model_id;         /*!< Model ID of the model.
                                *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                                *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
} rtk_bt_mesh_config_client_model_sub_add_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_default_ttl_get_t
 * @brief     BLE MESH Config Default TTL Get message structure.
 */
typedef struct {
	uint16_t dst;    /*!< Unicast address of the Configuration Server. */
} rtk_bt_mesh_config_client_model_default_ttl_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_default_ttl_set_t
 * @brief     BLE MESH Config Default TTL Set message structure.
 */
typedef struct {
	uint16_t dst;    /*!< Unicast address of the Configuration Server. */
	uint8_t ttl;     /*!< Default TTL to set.
                      *   0x00, 0x02 to 0x7F: The Default TTL state.
                      *   0x01, 0x80 to 0xFF: Prohibited. */
} rtk_bt_mesh_config_client_model_default_ttl_set_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_relay_get_t
 * @brief     BLE MESH Config Relay Get message structure.
 */
typedef struct {
	uint16_t dst;    /*!< Unicast address of the Configuration Server. */
} rtk_bt_mesh_config_client_model_relay_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_relay_set_t
 * @brief     BLE MESH Config Relay Set message structure.
 */
typedef struct {
	uint16_t dst;         /*!< Unicast address of the Configuration Server. */
	uint8_t state;        /*!< Relay state to set.
                           *   0x00: The Relay feature is supported and disabled.
                           *   0x01: The Relay feature is supported and enabled.
                           *   0x02: The Relay feature is not supported.
                           *   0x03 to 0xFF: Prohibited. */
	uint8_t count : 3;    /*!< Relay Retransmit Count to set, controls the number of transmissions of each relayed Network PDU.
                           *   Number of transmissions = count + 1.
                           *   Range: 0x00 to 0x07. */
	uint8_t steps : 5;    /*!< Relay Retransmit Interval Steps to set, the number of 10-millisecond steps between retransmissions.
                           *   Transmission interval = (steps + 1) * 10 ms.
                           *   Range: 0x00 to 0x1F. */
} rtk_bt_mesh_config_client_model_relay_set_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_net_transmit_get_t
 * @brief     BLE MESH Config Network Transmit Get message structure.
 */
typedef struct {
	uint16_t dst;    /*!< Unicast address of the Configuration Server. */
} rtk_bt_mesh_config_client_model_net_transmit_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_net_transmit_set_t
 * @brief     BLE MESH Config Network Transmit Set message structure.
 */
typedef struct {
	uint16_t dst;         /*!< Unicast address of the Configuration Server. */
	uint8_t count : 3;    /*!< Network Transmit Count to set, controls the number of transmissions of each Network PDU originated by the node.
                           *   Number of transmissions = count + 1.
                           *   Range: 0x00 to 0x07. */
	uint8_t steps : 5;    /*!< Network Transmit Interval Steps to set, the number of 10-millisecond steps between transmissions.
                           *   Transmission interval = (steps + 1) * 10 ms.
                           *   Range: 0x00 to 0x1F. */
} rtk_bt_mesh_config_client_model_net_transmit_set_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_node_reset_t
 * @brief     BLE MESH Config Node Reset message structure.
 */
typedef struct {
	uint16_t dst;    /*!< Unicast address of the Configuration Server. */
} rtk_bt_mesh_config_client_model_node_reset_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_compo_data_get_t
 * @brief     BLE MESH Config Composition Data Get message structure.
 */
typedef struct {
	uint16_t dst;    /*!< Unicast address of the Configuration Server. */
	uint8_t page;    /*!< Page number of the Composition Data to read.
                      *   If the Configuration Server does not support the page, it replies with the largest page it supports that is less than this page. */
} rtk_bt_mesh_config_client_model_compo_data_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_beacon_get_t
 * @brief     BLE MESH Config Beacon Get message structure.
 */
typedef struct {
	uint16_t dst;    /*!< Unicast address of the Configuration Server. */
} rtk_bt_mesh_config_client_model_beacon_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_beacon_set_t
 * @brief     BLE MESH Config Beacon Set message structure.
 */
typedef struct {
	uint16_t dst;     /*!< Unicast address of the Configuration Server. */
	uint8_t state;    /*!< Secure Network Beacon state to set.
                       *   0x00: The node is not broadcasting a Secure Network beacon.
                       *   0x01: The node is broadcasting a Secure Network beacon.
                       *   0x02 to 0xFF: Prohibited. */
} rtk_bt_mesh_config_client_model_beacon_set_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_proxy_get_t
 * @brief     BLE MESH Config GATT Proxy Get message structure.
 */
typedef struct {
	uint16_t dst;    /*!< Unicast address of the Configuration Server. */
} rtk_bt_mesh_config_client_model_proxy_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_proxy_set_t
 * @brief     BLE MESH Config GATT Proxy Set message structure.
 */
typedef struct {
	uint16_t dst;     /*!< Unicast address of the Configuration Server. */
	uint8_t state;    /*!< GATT Proxy state to set.
                       *   0x00: The Proxy feature is disabled.
                       *   0x01: The Proxy feature is enabled.
                       *   0x02: The Proxy feature is not supported.
                       *   0x03 to 0xFF: Prohibited. */
} rtk_bt_mesh_config_client_model_proxy_set_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_model_pub_get_t
 * @brief     BLE MESH Config Model Publication Get message structure.
 */
typedef struct {
	uint16_t dst;             /*!< Unicast address of the Configuration Server. */
	uint16_t element_addr;    /*!< Unicast address of the element that contains the model. */
	uint32_t model_id;        /*!< Model ID of the model.
                               *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                               *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
} rtk_bt_mesh_config_client_model_model_pub_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_model_pub_set_t
 * @brief     BLE MESH Config Model Publication Set message structure.
 */
typedef struct {
	uint16_t dst;                                       /*!< Unicast address of the Configuration Server. */
	uint16_t element_addr;                              /*!< Unicast address of the element that contains the model. */
	bool va_flag;                                       /*!< Whether the publish address is a virtual address.
                                                         *   - true: pub_addr is the 16-byte Label UUID of a virtual address.
                                                         *   - false: the first 2 bytes of pub_addr are the publish address. */
	uint8_t pub_addr[16];                               /*!< Publish address of the model.
                                                         *   A unicast address, a group address or the unassigned address (publication disabled) when va_flag is false. */
	rtk_bt_mesh_pub_key_info_t pub_key_info;            /*!< Publish AppKey index and Friendship Credential flag of the model. */
	uint8_t pub_ttl;                                    /*!< TTL of the published messages.
                                                         *   0x00 to 0x7F: Publish TTL.
                                                         *   0x80 to 0xFE: Prohibited.
                                                         *   0xFF: Use the Default TTL. */
	rtk_bt_mesh_pub_period_t pub_period;                /*!< Period of the periodic publication. */
	rtk_bt_mesh_pub_retrans_info_t pub_retrans_info;    /*!< Retransmission of each published message. */
	uint32_t model_id;                                  /*!< Model ID of the model.
                                                         *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                                                         *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
} rtk_bt_mesh_config_client_model_model_pub_set_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_model_sub_delete_t
 * @brief     BLE MESH Config Model Subscription Delete message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Configuration Server. */
	uint16_t element_addr;     /*!< Unicast address of the element that contains the model. */
	bool va_flag;              /*!< Whether the address to delete is a virtual address.
                                *   - true: group_addr is the 16-byte Label UUID of a virtual address.
                                *   - false: the first 2 bytes of group_addr are the address. */
	uint8_t group_addr[16];    /*!< Address to remove from the Subscription List.
                                *   The unassigned address, a unicast address and the all-nodes address are Prohibited when va_flag is false. */
	uint32_t model_id;         /*!< Model ID of the model.
                                *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                                *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
} rtk_bt_mesh_config_client_model_model_sub_delete_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_model_sub_delete_all_t
 * @brief     BLE MESH Config Model Subscription Delete All message structure.
 */
typedef struct {
	uint16_t dst;             /*!< Unicast address of the Configuration Server. */
	uint16_t element_addr;    /*!< Unicast address of the element that contains the model. */
	uint32_t model_id;        /*!< Model ID of the model.
                               *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                               *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
} rtk_bt_mesh_config_client_model_model_sub_delete_all_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_model_sub_overwrite_t
 * @brief     BLE MESH Config Model Subscription Overwrite message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Configuration Server. */
	uint16_t element_addr;     /*!< Unicast address of the element that contains the model. */
	bool va_flag;              /*!< Whether the new address is a virtual address.
                                *   - true: group_addr is the 16-byte Label UUID of a virtual address.
                                *   - false: the first 2 bytes of group_addr are the address. */
	uint8_t group_addr[16];    /*!< Address to be the only entry of the Subscription List.
                                *   The unassigned address, a unicast address and the all-nodes address are Prohibited when va_flag is false. */
	uint32_t model_id;         /*!< Model ID of the model.
                                *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                                *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
} rtk_bt_mesh_config_client_model_model_sub_overwrite_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_model_sub_get_t
 * @brief     BLE MESH Config Model Subscription Get message structure.
 */
typedef struct {
	uint16_t dst;             /*!< Unicast address of the Configuration Server. */
	uint16_t element_addr;    /*!< Unicast address of the element that contains the model. */
	uint32_t model_id;        /*!< Model ID of the model.
                               *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                               *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
} rtk_bt_mesh_config_client_model_model_sub_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_net_key_add_t
 * @brief     BLE MESH Config NetKey Add message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the network key.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t net_key[16];       /*!< Network key. */
} rtk_bt_mesh_config_client_model_net_key_add_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_net_key_update_t
 * @brief     BLE MESH Config NetKey Update message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the network key.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t net_key[16];       /*!< New value of the network key. */
} rtk_bt_mesh_config_client_model_net_key_update_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_net_key_delete_t
 * @brief     BLE MESH Config NetKey Delete message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the network key.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_config_client_model_net_key_delete_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_net_key_get_t
 * @brief     BLE MESH Config NetKey Get message structure.
 */
typedef struct {
	uint16_t dst;    /*!< Unicast address of the Configuration Server. */
} rtk_bt_mesh_config_client_model_net_key_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_app_key_update_t
 * @brief     BLE MESH Config AppKey Update message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the network key the application key is bound to.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t app_key_index;    /*!< Index of the application key.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t app_key[16];       /*!< New value of the application key. */
} rtk_bt_mesh_config_client_model_app_key_update_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_app_key_delete_t
 * @brief     BLE MESH Config AppKey Delete message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the network key the application key is bound to.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t app_key_index;    /*!< Index of the application key.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_config_client_model_app_key_delete_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_app_key_get_t
 * @brief     BLE MESH Config AppKey Get message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the network key whose bound application keys are requested.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_config_client_model_app_key_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_node_identity_get_t
 * @brief     BLE MESH Config Node Identity Get message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the network key.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_config_client_model_node_identity_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_node_identity_set_t
 * @brief     BLE MESH Config Node Identity Set message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the network key.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t identity;          /*!< Node Identity state to set.
                                *   0x00: Advertising with Node Identity for the subnet is stopped.
                                *   0x01: Advertising with Node Identity for the subnet is running.
                                *   0x02: Advertising with Node Identity is not supported.
                                *   0x03 to 0xFF: Prohibited. */
} rtk_bt_mesh_config_client_model_node_identity_set_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_model_app_unbind_t
 * @brief     BLE MESH Config Model App Unbind message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Configuration Server. */
	uint16_t element_addr;     /*!< Unicast address of the element that contains the model. */
	uint16_t app_key_index;    /*!< Index of the application key.
                                *   Range: 0x0000 to 0x0FFF. */
	uint32_t model_id;         /*!< Model ID of the model.
                                *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                                *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
} rtk_bt_mesh_config_client_model_model_app_unbind_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_model_app_get_t
 * @brief     BLE MESH Config Model App Get message structure.
 */
typedef struct {
	uint16_t dst;             /*!< Unicast address of the Configuration Server. */
	uint16_t element_addr;    /*!< Unicast address of the element that contains the model. */
	uint32_t model_id;        /*!< Model ID of the model.
                               *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                               *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
} rtk_bt_mesh_config_client_model_model_app_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_frnd_get_t
 * @brief     BLE MESH Config Friend Get message structure.
 */
typedef struct {
	uint16_t dst;    /*!< Unicast address of the Configuration Server. */
} rtk_bt_mesh_config_client_model_frnd_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_frnd_set_t
 * @brief     BLE MESH Config Friend Set message structure.
 */
typedef struct {
	uint16_t dst;     /*!< Unicast address of the Configuration Server. */
	uint8_t state;    /*!< Friend state to set.
                       *   0x00: The Friend feature is supported and disabled.
                       *   0x01: The Friend feature is supported and enabled.
                       *   0x02: The Friend feature is not supported.
                       *   0x03 to 0xFF: Prohibited. */
} rtk_bt_mesh_config_client_model_frnd_set_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_lpn_poll_timeout_get_t
 * @brief     BLE MESH Config Low Power Node PollTimeout Get message structure.
 */
typedef struct {
	uint16_t dst;         /*!< Unicast address of the Configuration Server, which should be a Friend node. */
	uint16_t lpn_addr;    /*!< Primary unicast address of the Low Power node whose PollTimeout timer is requested. */
} rtk_bt_mesh_config_client_model_lpn_poll_timeout_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_key_refresh_phase_get_t
 * @brief     BLE MESH Config Key Refresh Phase Get message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the network key.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_config_client_model_key_refresh_phase_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_key_refresh_phase_set_t
 * @brief     BLE MESH Config Key Refresh Phase Set message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the network key.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t state;             /*!< Key Refresh Phase transition to apply.
                                *   0x02: Transition 2, moves the first phase to the second phase of the Key Refresh procedure.
                                *   0x03: Transition 3, invokes the third phase of the Key Refresh procedure and then moves to the normal operation.
                                *   The transition must be one that is allowed for the current Key Refresh Phase state of the Configuration Server. */
} rtk_bt_mesh_config_client_model_key_refresh_phase_set_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_hb_pub_get_t
 * @brief     BLE MESH Config Heartbeat Publication Get message structure.
 */
typedef struct {
	uint16_t dst;    /*!< Unicast address of the Configuration Server. */
} rtk_bt_mesh_config_client_model_hb_pub_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_hb_pub_set_t
 * @brief     BLE MESH Config Heartbeat Publication Set message structure.
 */
typedef struct {
	uint16_t dst;                              /*!< Unicast address of the Configuration Server that sets its Heartbeat Publication state. */
	uint16_t dst_pub;                          /*!< Destination address of the Heartbeat messages.
                                                *   The unassigned address (Heartbeat messages are not sent), a unicast address or a group address. */
	uint8_t count_log;                         /*!< Number of Heartbeat messages to be sent, in the Count Log form.
                                                *   0x00: Periodic Heartbeat messages are not published.
                                                *   0x01 to 0x11: Number of Heartbeat messages = 2^(n-1).
                                                *   0x12 to 0xFE: Prohibited.
                                                *   0xFF: Periodic Heartbeat messages are published indefinitely. */
	uint8_t period_log;                        /*!< Period of the Heartbeat messages, in the Period Log form.
                                                *   0x00: Periodic Heartbeat messages are not published.
                                                *   0x01 to 0x11: Period = 2^(n-1) seconds.
                                                *   0x12 to 0xFF: Prohibited. */
	uint8_t ttl;                               /*!< TTL of the Heartbeat messages.
                                                *   Range: 0x00 to 0x7F.
                                                *   0x80 to 0xFF: Prohibited. */
	rtk_bt_mesh_hb_pub_features_t features;    /*!< Features whose change triggers a Heartbeat message. */
	uint16_t net_key_index;                    /*!< Index of the network key used to send the Heartbeat messages.
                                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_config_client_model_hb_pub_set_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_hb_sub_get_t
 * @brief     BLE MESH Config Heartbeat Subscription Get message structure.
 */
typedef struct {
	uint16_t dst;    /*!< Unicast address of the Configuration Server. */
} rtk_bt_mesh_config_client_model_hb_sub_get_t;

/**
 * @struct    rtk_bt_mesh_config_client_model_hb_sub_set_t
 * @brief     BLE MESH Config Heartbeat Subscription Set message structure.
 */
typedef struct {
	uint16_t dst;          /*!< Unicast address of the Configuration Server that sets its Heartbeat Subscription state. */
	uint16_t src;          /*!< Source address of the Heartbeat messages to process.
                            *   The unassigned address (Heartbeat messages are not processed) or a unicast address. */
	uint16_t dst_set;      /*!< Destination address of the Heartbeat messages to process.
                            *   The unassigned address (Heartbeat messages are not processed),
                            *   the primary unicast address of the Configuration Server or a group address. */
	uint8_t period_log;    /*!< Period for processing Heartbeat messages, in the Period Log form.
                            *   0x00: Heartbeat messages are not processed.
                            *   0x01 to 0x11: Period = 2^(n-1) seconds.
                            *   0x12 to 0xFF: Prohibited. */
} rtk_bt_mesh_config_client_model_hb_sub_set_t;

/***************************** Config Status Callback Data *****************************/
/**
 * @struct    rtk_bt_mesh_cfg_beacon_stat_t
 * @brief     BLE MESH Config Beacon Status definition.
 */
typedef struct {
	uint8_t state;    /*!< Secure Network Beacon state of the Configuration Server.
                       *   0x00: The node is not broadcasting a Secure Network beacon.
                       *   0x01: The node is broadcasting a Secure Network beacon.
                       *   0x02 to 0xFF: Prohibited. */
} rtk_bt_mesh_cfg_beacon_stat_t;

/**
 * @struct    rtk_bt_mesh_cfg_compo_data_stat_t
 * @brief     BLE MESH Config Composition Data Status definition.
 */
typedef struct {
	uint8_t page;       /*!< Page number of the Composition Data carried in the message. */
	uint8_t data[2];    /*!< Composition Data of the page, variable length. */
} _PACKED4_ rtk_bt_mesh_cfg_compo_data_stat_t;

/**
 * @struct    rtk_bt_mesh_cfg_compo_data_stat_call_t
 * @brief     BLE MESH Config Composition Data Status definition for callback to app.
 */
typedef struct {
	uint8_t page;        /*!< Page number of the Composition Data carried in the message. */
	uint16_t msg_len;    /*!< Length in bytes of the received Config Composition Data Status message.
                          *   Use length to get the size of the Composition Data. */
	uint16_t src;        /*!< Unicast address of the Configuration Server that sent the Status message. */
	uint8_t flag;        /*!< Whether the Composition Data can be parsed as Page 0 by the application.
                          *   1: The Composition Data is a complete Page 0 that can be parsed.
                          *   0: Otherwise, the Composition Data shall not be parsed as Page 0. */
	uint32_t length;     /*!< Length in bytes of the event data, which is the fixed fields (10 bytes) plus the Composition Data. */
	uint8_t *data;       /*!< Composition Data carried in the message, without the Page field.
                          *   This member only marks where the data starts,
                          *   the Composition Data is located right after the fixed fields (10 bytes) of the event data, its length is length minus 10.
                          *   For Page 0 it starts with Company ID, Product ID, Version ID, CRPL and Features (2 bytes each), followed by one or more elements.
                          *   Each element is made of the following fields in order:
                          *   - Location descriptor, 2 bytes.
                          *   - Number of SIG Model IDs (NumS), 1 byte.
                          *   - Number of Vendor Model IDs (NumV), 1 byte.
                          *   - NumS SIG Model IDs, 2 bytes each.
                          *   - NumV Vendor Model IDs, 4 bytes each, a Company ID (2 bytes) and a Vendor Model ID (2 bytes). */
} _PACKED4_ rtk_bt_mesh_cfg_compo_data_stat_call_t;

/**
 * @struct    rtk_bt_mesh_cfg_proxy_stat_t
 * @brief     BLE MESH Config GATT Proxy Status definition.
 */
typedef struct {
	uint8_t state;    /*!< GATT Proxy state of the Configuration Server.
                       *   0x00: The Proxy feature is supported and disabled.
                       *   0x01: The Proxy feature is supported and enabled.
                       *   0x02: The Proxy feature is not supported.
                       *   0x03 to 0xFF: Prohibited. */
} _PACKED4_ rtk_bt_mesh_cfg_proxy_stat_t;

/**
 * @struct    rtk_bt_mesh_cfg_model_pub_stat_t
 * @brief     BLE MESH Config Model Publication Status definition.
 */
typedef struct {
	rtk_bt_mesh_msg_stat_t stat;                        /*!< Status code of the last operation on the Model Publication parameters. */
	uint16_t element_addr;                              /*!< Unicast address of the element that contains the model. */
	uint16_t pub_addr;                                  /*!< Publish address of the model.
                                                         *   If the publish address is a Label UUID, this is its virtual address. */
	rtk_bt_mesh_pub_key_info_t pub_key_info;            /*!< Publish AppKey index and Friendship Credential flag of the model. */
	uint8_t pub_ttl;                                    /*!< TTL of the published messages.
                                                         *   0x00 to 0x7F: Publish TTL.
                                                         *   0x80 to 0xFE: Prohibited.
                                                         *   0xFF: Use the Default TTL. */
	rtk_bt_mesh_pub_period_t pub_period;                /*!< Period of the periodic publication. */
	rtk_bt_mesh_pub_retrans_info_t pub_retrans_info;    /*!< Retransmission of each published message. */
	uint32_t model_id;                                  /*!< Identifier of the model, a SIG Model ID (2 bytes) or a Vendor Model ID (4 bytes).
                                                         *   It is the value in the received message, so only the low 16 bits are the SIG Model ID for a SIG model. */
} _PACKED4_ rtk_bt_mesh_cfg_model_pub_stat_t;

/**
 * @struct    rtk_bt_mesh_cfg_model_sub_list_t
 * @brief     BLE MESH Config Model Subscription List definition.
 */
typedef struct {
	rtk_bt_mesh_msg_stat_t stat;    /*!< Status code of the last operation on the Subscription List. */
	uint16_t element_addr;          /*!< Unicast address of the element that contains the model. */
	uint32_t model_id;              /*!< Identifier of the model.
                                     *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                                     *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
	uint8_t data[2];                /*!< All addresses of the Subscription List of the model, variable length.
                                     *   A Label UUID is reported as its virtual address.
                                     *   This member only indicates what the data is, the event msg is reported as a byte stream,
                                     *   see event @ref RTK_BT_MESH_CONFIG_MODEL_SIG_MODEL_SUB_LIST for its fields and order. */
} _PACKED4_ rtk_bt_mesh_cfg_model_sub_list_t;

/**
 * @struct    rtk_bt_mesh_cfg_net_key_stat_t
 * @brief     BLE MESH Config NetKey Status definition.
 */
typedef struct {
	rtk_bt_mesh_msg_stat_t stat;    /*!< Status code of the requesting message. */
	uint16_t net_key_index;         /*!< Index of the NetKey.
                                     *   Range: 0x0000 to 0x0FFF. */
} _PACKED4_ rtk_bt_mesh_cfg_net_key_stat_t;

/**
 * @struct    rtk_bt_mesh_cfg_net_key_list_t
 * @brief     BLE MESH Config NetKey List definition.
 */
typedef struct {
	uint8_t net_key_indexes[2];    /*!< Indexes of all NetKeys known to the Configuration Server, variable length.
                                    *   This member only indicates what the data is, the event msg is reported as a byte stream,
                                    *   see event @ref RTK_BT_MESH_CONFIG_MODEL_NET_KEY_LIST for its fields and order. */
} rtk_bt_mesh_cfg_net_key_list_t;

/**
 * @struct    rtk_bt_mesh_cfg_app_key_list_t
 * @brief     BLE MESH Config AppKey List definition.
 */
typedef struct {
	rtk_bt_mesh_msg_stat_t stat;    /*!< Status code of the last operation on the AppKeys of the NetKey. */
	uint16_t net_key_index;         /*!< Index of the NetKey that the AppKeys are bound to.
                                     *   Range: 0x0000 to 0x0FFF. */
	uint8_t app_key_indexes[2];     /*!< Indexes of all AppKeys bound to the NetKey, variable length.
                                     *   This member only indicates what the data is, the event msg is reported as a byte stream,
                                     *   see event @ref RTK_BT_MESH_CONFIG_MODEL_APP_KEY_LIST for its fields and order. */
} _PACKED4_ rtk_bt_mesh_cfg_app_key_list_t;

/**
 * @struct    rtk_bt_mesh_cfg_node_identity_stat_t
 * @brief     BLE MESH Config Node Identity Status definition.
 */
typedef struct {
	rtk_bt_mesh_msg_stat_t stat;    /*!< Status code of the requesting message. */
	uint16_t net_key_index;         /*!< Index of the NetKey of the subnet.
                                     *   Range: 0x0000 to 0x0FFF. */
	uint8_t identity;               /*!< Node Identity state of the subnet.
                                     *   0x00: Advertising with Node Identity for the subnet is stopped.
                                     *   0x01: Advertising with Node Identity for the subnet is running.
                                     *   0x02: Advertising with Node Identity is not supported.
                                     *   0x03 to 0xFF: Prohibited. */
} _PACKED4_ rtk_bt_mesh_cfg_node_identity_stat_t;

/**
 * @struct    rtk_bt_mesh_cfg_model_app_list_t
 * @brief     BLE MESH Config Model App List definition.
 */
typedef struct {
	rtk_bt_mesh_msg_stat_t stat;    /*!< Status code of the requesting message. */
	uint16_t element_addr;          /*!< Unicast address of the element that contains the model. */
	uint32_t model_id;              /*!< Identifier of the model.
                                     *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                                     *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
	uint8_t app_key_indexes[3];     /*!< Indexes of all AppKeys bound to the model, variable length.
                                     *   This member only indicates what the data is, the event msg is reported as a byte stream,
                                     *   see event @ref RTK_BT_MESH_CONFIG_MODEL_SIG_MODEL_APP_LIST for its fields and order. */
} rtk_bt_mesh_cfg_model_app_list_t;

/**
 * @struct    rtk_bt_mesh_cfg_frnd_stat_t
 * @brief     BLE MESH Config Friend Status definition.
 */
typedef struct {
	uint8_t state;    /*!< Friend state of the Configuration Server.
                       *   0x00: The node supports the Friend feature and it is disabled.
                       *   0x01: The node supports the Friend feature and it is enabled.
                       *   0x02: The Friend feature is not supported.
                       *   0x03 to 0xFF: Prohibited. */
} _PACKED4_ rtk_bt_mesh_cfg_frnd_stat_t;

/**
 * @struct    rtk_bt_mesh_cfg_lpn_poll_timeout_stat_t
 * @brief     BLE MESH Config Low Power Node PollTimeout Status definition.
 */
typedef struct {
	uint16_t lpn_addr;     /*!< Primary unicast address of the Low Power node. */
	uint8_t poll_to[3];    /*!< Current value of the PollTimeout timer of the Low Power node, in units of 100 milliseconds, little endian.
                            *   0x000000: The node is not a Friend node of the Low Power node.
                            *   0x000001 to 0x000009: Prohibited.
                            *   0x00000A to 0x34BBFF: PollTimeout timer value.
                            *   0x34BC00 to 0xFFFFFF: Prohibited. */
} _PACKED4_ rtk_bt_mesh_cfg_lpn_poll_timeout_stat_t;

/**
 * @struct    rtk_bt_mesh_cfg_key_refresh_phase_stat_t
 * @brief     BLE MESH Config Key Refresh Phase Status definition.
 */
typedef struct {
	rtk_bt_mesh_msg_stat_t stat;    /*!< Status code of the requesting message. */
	uint16_t net_key_index;         /*!< Index of the NetKey.
                                     *   Range: 0x0000 to 0x0FFF. */
	uint8_t state;                  /*!< Key Refresh Phase state of the NetKey.
                                     *   0x00: Normal operation, the Key Refresh procedure is not active.
                                     *   0x01: First phase of the Key Refresh procedure.
                                     *   0x02: Second phase of the Key Refresh procedure.
                                     *   0x03 to 0xFF: Prohibited. */
} _PACKED4_ rtk_bt_mesh_cfg_key_refresh_phase_stat_t;

/**
 * @struct    rtk_bt_mesh_cfg_key_refresh_phase_stat_call_t
 * @brief     BLE MESH Config Key Refresh Phase Status definition for callback to app.
 */
typedef struct {
	rtk_bt_mesh_msg_stat_t stat;    /*!< Status code of the requesting message. */
	uint16_t net_key_index;         /*!< Index of the NetKey.
                                     *   Range: 0x0000 to 0x0FFF. */
	uint8_t state;                  /*!< Key Refresh Phase state of the NetKey.
                                     *   0x00: Normal operation, the Key Refresh procedure is not active.
                                     *   0x01: First phase of the Key Refresh procedure.
                                     *   0x02: Second phase of the Key Refresh procedure.
                                     *   0x03 to 0xFF: Prohibited. */
	uint16_t src;                   /*!< Unicast address of the Configuration Server that sent the Status message. */
} _PACKED4_ rtk_bt_mesh_cfg_key_refresh_phase_stat_call_t;

/**
 * @struct    rtk_bt_mesh_cfg_hb_pub_stat_t
 * @brief     BLE MESH Config Heartbeat Publication Status definition.
 */
typedef struct {
	rtk_bt_mesh_msg_stat_t stat;               /*!< Status code of the requesting message. */
	uint16_t dst;                              /*!< Destination address of the Heartbeat messages.
                                                *   The unassigned address (Heartbeat messages are not sent), a unicast address or a group address. */
	uint8_t count_log;                         /*!< Number of Heartbeat messages remaining to be sent, in the Count Log form.
                                                *   0x00: Periodic Heartbeat messages are not published.
                                                *   0x01 to 0x11: Number of Heartbeat messages that remain to be sent = 2^(n-1).
                                                *   0x12 to 0xFE: Prohibited.
                                                *   0xFF: Periodic Heartbeat messages are published indefinitely. */
	uint8_t period_log;                        /*!< Period of the Heartbeat messages, in the Period Log form.
                                                *   0x00: Periodic Heartbeat messages are not published.
                                                *   0x01 to 0x11: Period = 2^(n-1) seconds.
                                                *   0x12 to 0xFF: Prohibited. */
	uint8_t ttl;                               /*!< TTL of the Heartbeat messages.
                                                *   Range: 0x00 to 0x7F.
                                                *   0x80 to 0xFF: Prohibited. */
	rtk_bt_mesh_hb_pub_features_t features;    /*!< Features whose change triggers a Heartbeat message. */
	uint16_t net_key_index;                    /*!< Index of the NetKey used to send the Heartbeat messages.
                                                *   Range: 0x0000 to 0x0FFF. */
} _PACKED4_ rtk_bt_mesh_cfg_hb_pub_stat_t;

/**
 * @struct    rtk_bt_mesh_cfg_hb_sub_stat_t
 * @brief     BLE MESH Config Heartbeat Subscription Status definition.
 */
typedef struct {
	rtk_bt_mesh_msg_stat_t stat;    /*!< Status code of the requesting message. */
	uint16_t src;                   /*!< Source address of the Heartbeat messages to process.
                                     *   The unassigned address (Heartbeat messages are not processed) or a unicast address. */
	uint16_t dst;                   /*!< Destination address of the Heartbeat messages to process.
                                     *   The unassigned address (Heartbeat messages are not processed),
                                     *   the primary unicast address of the Configuration Server, or a group address. */
	uint8_t period_log;             /*!< Remaining period for processing Heartbeat messages, in the Period Log form.
                                     *   0x00: Heartbeat messages are not processed.
                                     *   0x01 to 0x10: Remaining period is in the range 2^(n-1) to 2^n - 1 seconds.
                                     *   0x11: Remaining period is 65536 seconds.
                                     *   0x12 to 0xFF: Prohibited. */
	uint8_t count_log;              /*!< Number of Heartbeat messages received, in the Count Log form.
                                     *   0x00: No Heartbeat message has been received.
                                     *   0x01 to 0x10: Number of received Heartbeat messages is in the range 2^(n-1) to 2^n - 1.
                                     *   0xFF: More than 0xFFFE Heartbeat messages have been received. */
	uint8_t min_hops;               /*!< Minimum hops registered when receiving Heartbeat messages.
                                     *   Range: 0x00 to 0x7F.
                                     *   0x80 to 0xFF: Prohibited. */
	uint8_t max_hops;               /*!< Maximum hops registered when receiving Heartbeat messages.
                                     *   Range: 0x00 to 0x7F.
                                     *   0x80 to 0xFF: Prohibited. */
} _PACKED4_ rtk_bt_mesh_cfg_hb_sub_stat_t;

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

/********************************* Functions Declaration *******************************/
/** @addtogroup BT_Functions BT Functions
 * @{
 */
/** @addtogroup BT_Mesh_Functions BT Mesh Functions
 * @{
 */
/** @addtogroup BT_Mesh_Config_Client_Model_Functions BT Mesh Config Client Model Functions
 * @{
 */

/**
 * @brief     Send a Config AppKey Add message to a Configuration Server to add an AppKey to its AppKey list.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_APP_KEY_STAT.
 * @param[in] add_app_key: Config AppKey Add message parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_add_app_key(rtk_bt_mesh_config_client_add_app_key_t *add_app_key);

/**
 * @brief     Send a Config Model App Bind message to a Configuration Server to bind an AppKey to a model.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_MODEL_APP_STAT.
 * @param[in] model_app_bind: Config Model App Bind message parameters.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_app_bind(rtk_bt_mesh_config_client_model_app_bind_t *model_app_bind);

/**
 * @brief     Send a Config Model Subscription Add message to a Configuration Server to add an address to the subscription list of a model.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_STAT.
 * @param[in] model_sub_add: Config Model Subscription Add message parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_sub_add(rtk_bt_mesh_config_client_model_sub_add_t *model_sub_add);

/**
 * @brief     Send a Config Default TTL Get message to a Configuration Server to read its default TTL state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_DEFAULT_TTL_STAT.
 * @param[in] ttl_get: Config Default TTL Get message parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_default_ttl_get(rtk_bt_mesh_config_client_model_default_ttl_get_t *ttl_get);

/**
 * @brief     Send a Config Default TTL Set message to a Configuration Server to set its default TTL state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_DEFAULT_TTL_STAT.
 * @param[in] ttl_set: Config Default TTL Set message parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_default_ttl_set(rtk_bt_mesh_config_client_model_default_ttl_set_t *ttl_set);

/**
 * @brief     Send a Config Relay Get message to a Configuration Server to read its relay state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_RELAY_STAT.
 * @param[in] relay_get: Config Relay Get message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_relay_get(rtk_bt_mesh_config_client_model_relay_get_t *relay_get);

/**
 * @brief     Send a Config Relay Set message to a Configuration Server to set its relay state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_RELAY_STAT.
 * @param[in] relay_set: Config Relay Set message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_relay_set(rtk_bt_mesh_config_client_model_relay_set_t *relay_set);

/**
 * @brief     Send a Config Network Transmit Get message to a Configuration Server to read its network transmit state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_NET_TRANSMIT_STAT.
 * @param[in] net_transmit_get: Config Network Transmit Get message parameters.
 *                              Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_net_transmit_get(rtk_bt_mesh_config_client_model_net_transmit_get_t *net_transmit_get);

/**
 * @brief     Send a Config Network Transmit Set message to a Configuration Server to set its network transmit state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_NET_TRANSMIT_STAT.
 * @param[in] net_transmit_set: Config Network Transmit Set message parameters.
 *                              Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_net_transmit_set(rtk_bt_mesh_config_client_model_net_transmit_set_t *net_transmit_set);

/**
 * @brief     Send a Config Node Reset message to a Configuration Server to reset the node.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_NODE_RESET_STAT.
 * @param[in] node_reset: Config Node Reset message parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_node_reset(rtk_bt_mesh_config_client_model_node_reset_t *node_reset);

/**
 * @brief     Send a Config Composition Data Get message to a Configuration Server to read a page of its composition data.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_COMPO_DATA_STAT.
 * @param[in] compo_data_get: Config Composition Data Get message parameters.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_compo_data_get(rtk_bt_mesh_config_client_model_compo_data_get_t *compo_data_get);

/**
 * @brief     Send a Config Beacon Get message to a Configuration Server to read its secure network beacon state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_BEACON_STAT.
 * @param[in] beacon_get: Config Beacon Get message parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_beacon_get(rtk_bt_mesh_config_client_model_beacon_get_t *beacon_get);

/**
 * @brief     Send a Config Beacon Set message to a Configuration Server to set its secure network beacon state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_BEACON_STAT.
 * @param[in] beacon_set: Config Beacon Set message parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_beacon_set(rtk_bt_mesh_config_client_model_beacon_set_t *beacon_set);

/**
 * @brief     Send a Config GATT Proxy Get message to a Configuration Server to read its GATT proxy state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_PROXY_STAT.
 * @param[in] proxy_get: Config GATT Proxy Get message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_proxy_get(rtk_bt_mesh_config_client_model_proxy_get_t *proxy_get);

/**
 * @brief     Send a Config GATT Proxy Set message to a Configuration Server to set its GATT proxy state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_PROXY_STAT.
 * @param[in] proxy_set: Config GATT Proxy Set message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_proxy_set(rtk_bt_mesh_config_client_model_proxy_set_t *proxy_set);

/**
 * @brief     Send a Config Model Publication Get message to a Configuration Server to read the publication state of a model.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_MODEL_PUB_STAT.
 * @param[in] pub_get: Config Model Publication Get message parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_pub_get(rtk_bt_mesh_config_client_model_model_pub_get_t *pub_get);

/**
 * @brief     Send a Config Model Publication Set message to a Configuration Server to set the publication state of a model.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_MODEL_PUB_STAT.
 * @param[in] pub_set: Config Model Publication Set message parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_pub_set(rtk_bt_mesh_config_client_model_model_pub_set_t *pub_set);

/**
 * @brief     Send a Config Model Subscription Delete message to a Configuration Server to delete an address from the subscription list of a model.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_STAT.
 * @param[in] sub_delete: Config Model Subscription Delete message parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_sub_delete(rtk_bt_mesh_config_client_model_model_sub_delete_t *sub_delete);

/**
 * @brief     Send a Config Model Subscription Delete All message to a Configuration Server to delete all addresses from the subscription list of a model.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_STAT.
 * @param[in] sub_delete_all: Config Model Subscription Delete All message parameters.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_sub_delete_all(rtk_bt_mesh_config_client_model_model_sub_delete_all_t *sub_delete_all);

/**
 * @brief     Send a Config Model Subscription Overwrite message to a Configuration Server to overwrite the subscription list of a model with an address.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_STAT.
 * @param[in] sub_overwrite: Config Model Subscription Overwrite message parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_sub_overwrite(rtk_bt_mesh_config_client_model_model_sub_overwrite_t *sub_overwrite);

/**
 * @brief     Send a Config Model Subscription Get message to a Configuration Server to read the subscription list of a model.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_SIG_MODEL_SUB_LIST for a SIG model, or
 *            @ref RTK_BT_MESH_CONFIG_MODEL_VENDOR_MODEL_SUB_LIST for a vendor model.
 * @param[in] sub_get: Config Model Subscription Get message parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_sub_get(rtk_bt_mesh_config_client_model_model_sub_get_t *sub_get);

/**
 * @brief     Send a Config NetKey Add message to a Configuration Server to add a NetKey to its NetKey list.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_NET_KEY_STAT.
 * @param[in] net_key_add: Config NetKey Add message parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_net_key_add(rtk_bt_mesh_config_client_model_net_key_add_t *net_key_add);

/**
 * @brief     Send a Config NetKey Update message to a Configuration Server to update a NetKey in its NetKey list.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_NET_KEY_STAT.
 * @param[in] net_key_update: Config NetKey Update message parameters.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_net_key_update(rtk_bt_mesh_config_client_model_net_key_update_t *net_key_update);

/**
 * @brief     Send a Config NetKey Delete message to a Configuration Server to delete a NetKey from its NetKey list.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_NET_KEY_STAT.
 * @param[in] net_key_delete: Config NetKey Delete message parameters.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_net_key_delete(rtk_bt_mesh_config_client_model_net_key_delete_t *net_key_delete);

/**
 * @brief     Send a Config NetKey Get message to a Configuration Server to read its NetKey list.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_NET_KEY_LIST.
 * @param[in] net_key_get: Config NetKey Get message parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_net_key_get(rtk_bt_mesh_config_client_model_net_key_get_t *net_key_get);

/**
 * @brief     Send a Config AppKey Update message to a Configuration Server to update an AppKey in its AppKey list.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_APP_KEY_STAT.
 * @param[in] app_key_update: Config AppKey Update message parameters.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_app_key_update(rtk_bt_mesh_config_client_model_app_key_update_t *app_key_update);

/**
 * @brief     Send a Config AppKey Delete message to a Configuration Server to delete an AppKey from its AppKey list.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_APP_KEY_STAT.
 * @param[in] app_key_delete: Config AppKey Delete message parameters.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_app_key_delete(rtk_bt_mesh_config_client_model_app_key_delete_t *app_key_delete);

/**
 * @brief     Send a Config AppKey Get message to a Configuration Server to read its AppKey list bound to a NetKey.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_APP_KEY_LIST.
 * @param[in] app_key_get: Config AppKey Get message parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_app_key_get(rtk_bt_mesh_config_client_model_app_key_get_t *app_key_get);

/**
 * @brief     Send a Config Node Identity Get message to a Configuration Server to read its node identity state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_NODE_IDENTITY_STAT.
 * @param[in] node_identity_get: Config Node Identity Get message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_node_identity_get(rtk_bt_mesh_config_client_model_node_identity_get_t *node_identity_get);

/**
 * @brief     Send a Config Node Identity Set message to a Configuration Server to set its node identity state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_NODE_IDENTITY_STAT.
 * @param[in] node_identity_set: Config Node Identity Set message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_node_identity_set(rtk_bt_mesh_config_client_model_node_identity_set_t *node_identity_set);

/**
 * @brief     Send a Config Model App Unbind message to a Configuration Server to unbind an AppKey from a model.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_MODEL_APP_STAT.
 * @param[in] model_app_unbind: Config Model App Unbind message parameters.
 *                              Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_app_unbind(rtk_bt_mesh_config_client_model_model_app_unbind_t *model_app_unbind);

/**
 * @brief     Send a Config Model App Get message to a Configuration Server to read the AppKeys bound to a model.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_SIG_MODEL_APP_LIST for a SIG model, or
 *            @ref RTK_BT_MESH_CONFIG_MODEL_VENDOR_MODEL_APP_LIST for a vendor model.
 * @param[in] model_app_get: Config Model App Get message parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_app_get(rtk_bt_mesh_config_client_model_model_app_get_t *model_app_get);

/**
 * @brief     Send a Config Friend Get message to a Configuration Server to read its friend state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_FRND_STAT.
 * @param[in] frnd_get: Config Friend Get message parameters.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_frnd_get(rtk_bt_mesh_config_client_model_frnd_get_t *frnd_get);

/**
 * @brief     Send a Config Friend Set message to a Configuration Server to set its friend state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_FRND_STAT.
 * @param[in] frnd_set: Config Friend Set message parameters.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_frnd_set(rtk_bt_mesh_config_client_model_frnd_set_t *frnd_set);

/**
 * @brief     Send a Config Low Power Node PollTimeout Get message to a Configuration Server to read the PollTimeout of a low power node it is friend of.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_LPN_POLL_TO_STAT.
 * @param[in] lpn_poll_timeout_get: Config Low Power Node PollTimeout Get message parameters.
 *                                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_lpn_poll_timeout_get(rtk_bt_mesh_config_client_model_lpn_poll_timeout_get_t *lpn_poll_timeout_get);

/**
 * @brief     Send a Config Key Refresh Phase Get message to a Configuration Server to read the key refresh phase of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_KEY_REFRESH_PHASE_STAT.
 * @param[in] key_refresh_phase_get: Config Key Refresh Phase Get message parameters.
 *                                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_key_refresh_phase_get(rtk_bt_mesh_config_client_model_key_refresh_phase_get_t *key_refresh_phase_get);

/**
 * @brief     Send a Config Key Refresh Phase Set message to a Configuration Server to set the key refresh phase transition of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_KEY_REFRESH_PHASE_STAT.
 * @param[in] key_refresh_phase_set: Config Key Refresh Phase Set message parameters.
 *                                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_key_refresh_phase_set(rtk_bt_mesh_config_client_model_key_refresh_phase_set_t *key_refresh_phase_set);

/**
 * @brief     Send a Config Heartbeat Publication Get message to a Configuration Server to read its heartbeat publication state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_HB_PUB_STAT.
 * @param[in] hb_pub_get: Config Heartbeat Publication Get message parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_hb_pub_get(rtk_bt_mesh_config_client_model_hb_pub_get_t *hb_pub_get);

/**
 * @brief     Send a Config Heartbeat Publication Set message to a Configuration Server to set its heartbeat publication state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_HB_PUB_STAT.
 * @param[in] hb_pub_set: Config Heartbeat Publication Set message parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_hb_pub_set(rtk_bt_mesh_config_client_model_hb_pub_set_t *hb_pub_set);

/**
 * @brief     Send a Config Heartbeat Subscription Get message to a Configuration Server to read its heartbeat subscription state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_HB_SUB_STAT.
 * @param[in] hb_sub_get: Config Heartbeat Subscription Get message parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_hb_sub_get(rtk_bt_mesh_config_client_model_hb_sub_get_t *hb_sub_get);

/**
 * @brief     Send a Config Heartbeat Subscription Set message to a Configuration Server to set its heartbeat subscription state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_CONFIG_MODEL_HB_SUB_STAT.
 * @param[in] hb_sub_set: Config Heartbeat Subscription Set message parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_config_client_model_hb_sub_set(rtk_bt_mesh_config_client_model_hb_sub_set_t *hb_sub_set);

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

#endif /* __RTK_BT_MESH_CONFIG_CLIENT_MODEL_H__ */
