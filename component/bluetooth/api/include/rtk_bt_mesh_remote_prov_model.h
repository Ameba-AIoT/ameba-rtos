/**
 * @file      rtk_bt_mesh_remote_prov_model.h
 * @author    pico_zhai@realsil.com.cn
 * @brief     Bluetooth LE MESH remote provisioning client and server model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_REMOTE_PROVISIONING_H__
#define __RTK_BT_MESH_REMOTE_PROVISIONING_H__

#include <stdint.h>
#include <rtk_bt_mesh_common.h>

/*************************** Remote Provisioning Client Model **************************/

/** @addtogroup BT_Types BT Types
 * @{
 */
/** @addtogroup BT_Mesh_Types BT Mesh Types
 * @{
 */
/** @addtogroup BT_Mesh_Remote_Provisioning_Model_Types BT Mesh Remote Provisioning Model Types
 * @{
 */
/**
 * @enum      rtk_bt_mesh_remote_prov_client_act_t
 * @brief     BLE MESH remote provisioning client model act definition.
 */
typedef enum {
	RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_SCAN_START = 1,        /*!< Send a Remote Provisioning Scan Start message to the server. */
	RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_SCAN_CAPA_GET,         /*!< Send a Remote Provisioning Scan Capabilities Get message to the server. */
	RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_LINK_OPEN,             /*!< Send a Remote Provisioning Link Open message to the server to open a provisioning bearer. */
	RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_LINK_OPEN_DKRI,        /*!< Send a Remote Provisioning Link Open message to the server to open the Node Provisioning Protocol
                                                               *   Interface. */
	RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_DEVICE_KEY_REFRESH,    /*!< Start the Device Key Refresh procedure on the opened Node Provisioning Protocol Interface. */
	RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_NODE_ADDR_REFRESH,     /*!< Start the Node Address Refresh procedure on the opened Node Provisioning Protocol Interface. */
	RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_COMPO_DATA_REFRESH,    /*!< Start the Node Composition Refresh procedure on the opened Node Provisioning Protocol Interface. */
	RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_MAX,                   /*!< Reserved. */
} rtk_bt_mesh_remote_prov_client_act_t;

/**
 * @enum      rtk_bt_mesh_remote_prov_client_evt_t
 * @brief     BLE MESH remote provisioning client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_SCAN_CAPA_STATUS = 1,    /*!< Indicate a Remote Provisioning Scan Capabilities Status message received from the server, with msg
                                                                 *   @ref rtk_bt_mesh_rmt_prov_client_scan_capa_status_t. */
	RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_SCAN_STATUS,             /*!< Indicate a Remote Provisioning Scan Status message received from the server, with msg
                                                                 *   @ref rtk_bt_mesh_rmt_prov_client_scan_status_t. */
	RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_SCAN_REPORT,             /*!< Indicate a Remote Provisioning Scan Report message received from the server, with msg
                                                                 *   @ref rtk_bt_mesh_rmt_prov_client_scan_report_t. */
	RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_EXTENDED_SCAN_REPORT,    /*!< Indicate a Remote Provisioning Extended Scan Report message received from the server, with msg
                                                                 *   @ref rtk_bt_mesh_rmt_prov_client_extended_scan_report_t. */
	RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_LINK_STATUS,             /*!< Indicate a Remote Provisioning Link Status message received from the server, with msg
                                                                 *   @ref rtk_bt_mesh_rmt_prov_client_link_status_t. */
	RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_LINK_REPORT,             /*!< Indicate a Remote Provisioning Link Report message received from the server, with msg
                                                                 *   @ref rtk_bt_mesh_rmt_prov_client_link_report_t. */
} rtk_bt_mesh_remote_prov_client_evt_t;

/**
 * @enum      rtk_bt_mesh_rmt_prov_status_t
 * @brief     BLE MESH remote provisioning status definition.
 */
typedef enum {
	RTK_BT_MESH_RMT_PROV_SUCCESS,                                     /*!< Status code 0x00: Success. */
	RTK_BT_MESH_RMT_PROV_SCANNING_CANNOT_START,                       /*!< Status code 0x01: Scanning Cannot Start. */
	RTK_BT_MESH_RMT_PROV_INVALID_STATE,                               /*!< Status code 0x02: Invalid State. */
	RTK_BT_MESH_RMT_PROV_LIMITED_RESOURCES,                           /*!< Status code 0x03: Limited Resources. */
	RTK_BT_MESH_RMT_PROV_LINK_CANNOT_OPEN,                            /*!< Status code 0x04: Link Cannot Open. */
	RTK_BT_MESH_RMT_PROV_LINK_OPEN_FAILED,                            /*!< Status code 0x05: Link Open Failed. */
	RTK_BT_MESH_RMT_PROV_LINK_CLOSED_BY_DEVICE,                       /*!< Status code 0x06: Link Closed by Device. */
	RTK_BT_MESH_RMT_PROV_LINK_CLOSED_BY_SERVER,                       /*!< Status code 0x07: Link Closed by Server. */
	RTK_BT_MESH_RMT_PROV_LINK_CLOSED_BY_CLIENT,                       /*!< Status code 0x08: Link Closed by Client. */
	RTK_BT_MESH_RMT_PROV_LINK_CLOSED_AS_CANNOT_RECEIVE_PDU,           /*!< Status code 0x09: Link Closed as Cannot Receive PDU. */
	RTK_BT_MESH_RMT_PROV_LINK_CLOSED_AS_CANNOT_SEND_PDU,              /*!< Status code 0x0A: Link Closed as Cannot Send PDU. */
	RTK_BT_MESH_RMT_PROV_LINK_CLOSED_AS_CANNOT_DELIVER_PDU_REPORT,    /*!< Status code 0x0B: Link Closed as Cannot Deliver PDU Report. */
} rtk_bt_mesh_rmt_prov_status_t;

/**
 * @enum      rtk_bt_mesh_rmt_prov_scan_state_t
 * @brief     BLE MESH remote provisioning scan status definition.
 */
typedef enum {
	RTK_BT_MESH_RMT_PROV_SCAN_STATE_IDLE,                /*!< Idle. */
	RTK_BT_MESH_RMT_PROV_SCAN_STATE_MULTIPLE_DEVICES,    /*!< Multiple Devices Scan, not limited to one device. */
	RTK_BT_MESH_RMT_PROV_SCAN_STATE_SINGLE_DEVICE,       /*!< Single Device Scan, limited to one device. */
} rtk_bt_mesh_rmt_prov_scan_state_t;

/**
 * @enum      rtk_bt_mesh_rmt_prov_link_state_t
 * @brief     BLE MESH remote provisioning link status definition.
 */
typedef enum {
	RTK_BT_MESH_RMT_PROV_LINK_STATE_IDLE,                  /*!< Idle. */
	RTK_BT_MESH_RMT_PROV_LINK_STATE_LINK_OPENING,          /*!< Link Opening. */
	RTK_BT_MESH_RMT_PROV_LINK_STATE_LINK_ACTIVE,           /*!< Link Active. */
	RTK_BT_MESH_RMT_PROV_LINK_STATE_OUTBOUND_PKT_TRANS,    /*!< Outbound Packet Transfer. */
	RTK_BT_MESH_RMT_PROV_LINK_STATE_LINK_CLOSING,          /*!< Link Closing. */
} rtk_bt_mesh_rmt_prov_link_state_t;

/**
 * @enum      rtk_bt_mesh_rmt_prov_link_close_reason_t
 * @brief     BLE MESH remote provisioning link close reason definition.
 */
typedef enum {
	RTK_BT_MESH_RMT_PROV_LINK_CLOSE_SUCCESS,    /*!< Success, the provisioning was successful. */
	RTK_BT_MESH_RMT_PROV_LINK_CLOSE_TIMEOUT,    /*!< Timeout, the provisioning transaction timed out. */
	RTK_BT_MESH_RMT_PROV_LINK_CLOSE_FAIL,       /*!< Fail, the provisioning failed. */
	RTK_BT_MESH_RMT_PROV_LINK_NOT_CLOSE,        /*!< No link close reason is carried in the Link Report message.
                                                 *   It is not a value defined by the specification. */
} rtk_bt_mesh_rmt_prov_link_close_reason_t;

/**
 * @typedef   rtk_bt_mesh_rmt_prov_dkri_procedure_t
 * @brief     BLE MESH remote provisioning DKRI PROCEDURE definition.
 */
enum {
	RTK_BT_MESH_RMT_PROV_DKRI_DEV_KEY_REFRESH,       /*!< Device Key Refresh procedure. */
	RTK_BT_MESH_RMT_PROV_DKRI_NODE_ADDR_REFRESH,     /*!< Node Address Refresh procedure. */
	RTK_BT_MESH_RMT_PROV_DKRI_NODE_COMPO_REFRESH,    /*!< Node Composition Refresh procedure. */
} _SHORT_ENUM_;
typedef uint8_t rtk_bt_mesh_rmt_prov_dkri_procedure_t;

/**
 * @struct    rtk_bt_mesh_remote_prov_client_scan_capa_get_t
 * @brief     BLE MESH remote provisioning scan capability get data struct definition.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Remote Provisioning Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_remote_prov_client_scan_capa_get_t;

/**
 * @struct    rtk_bt_mesh_rmt_prov_client_scan_capa_status_t
 * @brief     BLE MESH remote provisioning scan capability status data struct definition.
 */
typedef struct {
	uint16_t src;                /*!< Unicast address of the server that sent the Status message. */
	uint8_t max_scan_items;      /*!< Maximum number of unprovisioned devices that the server can report during scanning.
                                  *   0x00 to 0x03: Prohibited.
                                  *   0x04 to 0xFF: Maximum number of unprovisioned devices. */
	bool support_active_scan;    /*!< Whether the server supports active scanning.
                                  *   - true: The server supports active scanning.
                                  *   - false: The server does not support active scanning. */
} rtk_bt_mesh_rmt_prov_client_scan_capa_status_t;

/**
 * @struct    rtk_bt_mesh_remote_prov_client_scan_start_t
 * @brief     BLE MESH remote provisioning scan start data struct definition.
 */
typedef struct {
	uint16_t dst;                   /*!< Unicast address of the Remote Provisioning Server. */
	uint16_t net_key_index;         /*!< Index of the NetKey of the subnet the message is sent through.
                                     *   Range: 0x0000 to 0x0FFF. */
	uint8_t scanned_items_limit;    /*!< Maximum number of unprovisioned devices that the server may report.
                                     *   0x00: No limit on the number of unprovisioned devices. */
	uint8_t scan_timeout;           /*!< Time limit for the scan.
                                     *   Unit: second.
                                     *   Must not be 0. */
	uint8_t uuid[16];               /*!< Device UUID of the unprovisioned device to scan for.
                                     *   If it is all zero, the server scans for all unprovisioned devices within its radio range.
                                     *   Otherwise the server scans for the unprovisioned device identified by the UUID only. */
} rtk_bt_mesh_remote_prov_client_scan_start_t;

/**
 * @struct    rtk_bt_mesh_rmt_prov_client_scan_status_t
 * @brief     BLE MESH remote provisioning scan status data struct definition.
 */
typedef struct {
	uint16_t src;                                     /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_rmt_prov_status_t prov_status;        /*!< Status of the most recent operation on the Remote Provisioning Scan state. */
	rtk_bt_mesh_rmt_prov_scan_state_t scan_status;    /*!< Remote Provisioning Scan state of the server. */
	uint8_t scanned_items_limit;                      /*!< Maximum number of unprovisioned devices that the server may report. */
	uint8_t timeout;                                  /*!< Current value of the Remote Provisioning Timeout state.
                                                       *   Unit: second.
                                                       *   0x00: The scan is not in progress.
                                                       *   0x01 to 0xFF: The scan is in progress, the value is the maximum time remaining before the scan stops. */
} rtk_bt_mesh_rmt_prov_client_scan_status_t;

/**
 * @struct    rtk_bt_mesh_rmt_prov_client_scan_report_t
 * @brief     BLE MESH remote provisioning scan report data struct definition.
 */
typedef struct {
	uint16_t src;         /*!< Unicast address of the server that sent the Report message. */
	int8_t rssi;          /*!< Received signal strength of the packets sent by the unprovisioned device, measured by the server.
                           *   Unit: dBm.
                           *   127: The RSSI cannot be read. */
	uint16_t oob;         /*!< OOB Information of the unprovisioned device. */
	uint32_t uri_hash;    /*!< URI Hash of the unprovisioned device.
                           *   Only present when the unprovisioned device advertises a URI. */
	uint8_t uuid[16];     /*!< Device UUID of the unprovisioned device. */
} rtk_bt_mesh_rmt_prov_client_scan_report_t;

/**
 * @struct    rtk_bt_mesh_rmt_prov_client_extended_scan_report_t
 * @brief     BLE MESH remote provisioning extended scan report data struct definition.
 */
typedef struct {
	uint16_t src;                /*!< Unicast address of the server that sent the Report message. */
	uint8_t uuid[16];            /*!< Device UUID of the unprovisioned device or of the server. */
	uint16_t oob;                /*!< OOB Information of the unprovisioned device or of the server. */
	uint16_t adv_structs_len;    /*!< Length in bytes of the AD Structures requested by the client.
                                  *   The AD Structures are located right after the structure in the event data. */
} rtk_bt_mesh_rmt_prov_client_extended_scan_report_t;

/**
 * @struct    rtk_bt_mesh_remote_prov_client_link_open_t
 * @brief     BLE MESH remote provisioning link open data struct definition.
 */
typedef struct {
	uint16_t dst;                 /*!< Unicast address of the Remote Provisioning Server. */
	uint16_t net_key_index;       /*!< Index of the NetKey of the subnet the message is sent through.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint8_t uuid[16];             /*!< Device UUID of the unprovisioned device to open the provisioning bearer to. */
	uint8_t link_open_timeout;    /*!< Time limit for opening the provisioning bearer.
                                   *   Unit: second.
                                   *   0x00: Prohibited.
                                   *   0x01 to 0x3C: Timeout.
                                   *   0x3D to 0xFF: Prohibited. */
} rtk_bt_mesh_remote_prov_client_link_open_t;

/**
 * @struct    rtk_bt_mesh_rmt_prov_client_link_status_t
 * @brief     BLE MESH remote provisioning link status data struct definition.
 */
typedef struct {
	uint16_t src;                                     /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_rmt_prov_status_t prov_status;        /*!< Status of the processing of the message sent to the server. */
	rtk_bt_mesh_rmt_prov_link_state_t link_status;    /*!< Remote Provisioning Link state of the server. */
} rtk_bt_mesh_rmt_prov_client_link_status_t;

/**
 * @struct    rtk_bt_mesh_rmt_prov_client_link_report_t
 * @brief     BLE MESH remote provisioning link report data struct definition.
 */
typedef struct {
	uint16_t src;                                             /*!< Unicast address of the server that sent the Report message. */
	rtk_bt_mesh_rmt_prov_status_t prov_status;                /*!< Status of the provisioning bearer or the Node Provisioning Protocol Interface. */
	rtk_bt_mesh_rmt_prov_link_state_t link_status;            /*!< Remote Provisioning Link state of the server. */
	rtk_bt_mesh_rmt_prov_link_close_reason_t close_reason;    /*!< Reason for closing the provisioning bearer link.
                                                               *   Only carried when the provisioning link is closed. */
} rtk_bt_mesh_rmt_prov_client_link_report_t;

/**
 * @struct    rtk_bt_mesh_remote_prov_client_link_open_dkri_t
 * @brief     BLE MESH remote provisioning link open dkri data struct definition.
 */
typedef struct {
	uint16_t dst;                                            /*!< Unicast address of the Remote Provisioning Server. */
	uint16_t net_key_index;                                  /*!< Index of the NetKey of the subnet the message is sent through.
                                                              *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_rmt_prov_dkri_procedure_t dkri_procedure;    /*!< Node Provisioning Protocol Interface procedure that the server will execute. */
} rtk_bt_mesh_remote_prov_client_link_open_dkri_t;

/**
 * @struct    rtk_bt_mesh_remote_prov_client_node_addr_refresh_t
 * @brief     BLE MESH remote provisioning node address refresh data struct definition.
 */
typedef struct {
	uint16_t node_addr;    /*!< New unicast address to assign to the node. */
	uint8_t attn_dur;      /*!< Attention Timer duration of the node, carried in the Provisioning Invite.
                            *   Unit: second.
                            *   0x00: Off.
                            *   0x01 to 0xFF: On, the duration. */
} rtk_bt_mesh_remote_prov_client_node_addr_refresh_t;

/*************************** Remote Provisioning Server Model **************************/
/**
 * @enum      rtk_bt_mesh_remote_prov_server_act_t
 * @brief     BLE MESH remote provisioning server model act definition.
 */
typedef enum {
	RTK_BT_MESH_REMOTE_PROV_SERVER_ACT_SET_PREFER_BEARER = 1,    /*!< Set the bearer that the Remote Provisioning Server on this node prefers when opening a provisioning
                                                                  *   link. */
	RTK_BT_MESH_REMOTE_PROV_SERVER_ACT_MAX,                      /*!< Reserved. */
} rtk_bt_mesh_remote_prov_server_act_t;

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
/** @addtogroup BT_Mesh_Remote_Provisioning_Model_Functions BT Mesh Remote Provisioning Model Functions
 * @{
 */

/**
 * @brief     Send a Remote Provisioning Scan Start message to a Remote Provisioning Server to scan for unprovisioned devices within its radio range.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_SCAN_STATUS,
 *            the unprovisioned devices found are reported via event @ref RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_SCAN_REPORT.
 * @param[in] rmt_scan: Remote Provisioning Scan Start message parameters.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_remote_prov_scan_start(rtk_bt_mesh_remote_prov_client_scan_start_t *rmt_scan);

/**
 * @brief     Send a Remote Provisioning Scan Capabilities Get message to a Remote Provisioning Server to read its scan capabilities.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_SCAN_CAPA_STATUS.
 * @param[in] scan_capa_get: Remote Provisioning Scan Capabilities Get message parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_remote_prov_scan_capa_get(rtk_bt_mesh_remote_prov_client_scan_capa_get_t *scan_capa_get);

/**
 * @brief     Send a Remote Provisioning Link Open message to a Remote Provisioning Server to open the provisioning bearer to an unprovisioned device.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_LINK_STATUS,
 *            the state changes of the link are reported via event @ref RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_LINK_REPORT.
 * @param[in] link_open: Remote Provisioning Link Open message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_remote_prov_link_open(rtk_bt_mesh_remote_prov_client_link_open_t *link_open);

/**
 * @brief     Send a Remote Provisioning Link Open message to a Remote Provisioning Server to open the Node Provisioning Protocol Interface.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_LINK_STATUS,
 *            the state changes of the link are reported via event @ref RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_LINK_REPORT.
 * @param[in] link_open_dkri: Remote Provisioning Link Open message parameters.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_remote_prov_link_open_dkri(rtk_bt_mesh_remote_prov_client_link_open_dkri_t *link_open_dkri);

/**
 * @brief     Start the Device Key Refresh procedure on the opened Node Provisioning Protocol Interface link.
 *            It follows @ref rtk_bt_mesh_remote_prov_link_open_dkri.
 *            The completion is reported asynchronously via event @ref RTK_BT_MESH_STACK_EVT_PROV_COMPLETE.
 * @param[in] attn_dur: Attention Timer duration of the node, carried in the Provisioning Invite.
 *                      Unit: second, 0 means Attention Timer off.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The dkri_procedure of @ref rtk_bt_mesh_remote_prov_link_open_dkri shall be @ref RTK_BT_MESH_RMT_PROV_DKRI_DEV_KEY_REFRESH.
 */
uint16_t rtk_bt_mesh_remote_prov_dev_key_refresh(uint8_t *attn_dur);

/**
 * @brief     Start the Node Address Refresh procedure on the opened Node Provisioning Protocol Interface link.
 *            It follows @ref rtk_bt_mesh_remote_prov_link_open_dkri.
 *            The completion is reported asynchronously via event @ref RTK_BT_MESH_STACK_EVT_PROV_COMPLETE.
 * @param[in] node_refresh: Node Address Refresh parameters.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The dkri_procedure of @ref rtk_bt_mesh_remote_prov_link_open_dkri shall be @ref RTK_BT_MESH_RMT_PROV_DKRI_NODE_ADDR_REFRESH.
 */
uint16_t rtk_bt_mesh_remote_prov_node_addr_refresh(rtk_bt_mesh_remote_prov_client_node_addr_refresh_t *node_refresh);

/**
 * @brief     Start the Node Composition Refresh procedure on the opened Node Provisioning Protocol Interface link.
 *            It follows @ref rtk_bt_mesh_remote_prov_link_open_dkri.
 *            The completion is reported asynchronously via event @ref RTK_BT_MESH_STACK_EVT_PROV_COMPLETE.
 * @param[in] attn_dur: Attention Timer duration of the node, carried in the Provisioning Invite.
 *                      Unit: second, 0 means Attention Timer off.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The dkri_procedure of @ref rtk_bt_mesh_remote_prov_link_open_dkri shall be @ref RTK_BT_MESH_RMT_PROV_DKRI_NODE_COMPO_REFRESH.
 */
uint16_t rtk_bt_mesh_remote_prov_composition_refresh(uint8_t *attn_dur);

/**
 * @brief     Set the bearer that the Remote Provisioning Server on this node prefers when opening a provisioning link to an unprovisioned device.
 * @param[in] bearer: Preferred bearer, @ref RTK_BT_MESH_BEARER_FIELD_ADV or @ref RTK_BT_MESH_BEARER_FIELD_GATT.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_remote_prov_set_prefer_bearer(rtk_bt_mesh_bearer_field_t *bearer);

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

#endif /* __RTK_BT_MESH_REMOTE_PROVISIONING_H__ */
