/**
 * @file      rtk_bt_gap.h
 * @author    ping_yan@realsil.com.cn
 * @brief     Bluetooth common GAP type and function definitions.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_GAP_H__
#define __RTK_BT_GAP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <basic_types.h>
#include <bt_api_config.h>

/**
 * @typedef   rtk_bt_addr_type_t
 * @brief     Bluetooth device address type definition.
 */
typedef enum {
	RTK_BT_ADDR_LE_PUBLIC =             0x00,      /*!< Public Device Address. */
	RTK_BT_ADDR_LE_RANDOM =             0x01,      /*!< Random Device Address. */
	RTK_BT_ADDR_LE_RPA_PUBLIC =         0x02,      /*!< Public Identity Address (RPA). */
	RTK_BT_ADDR_LE_RPA_RANDOM =         0x03,      /*!< Random (static) Identity Address (RPA). */
	RTK_BT_ADDR_CLASSIC =               0x10,      /*!< BR/EDR Device Address. */
	RTK_BT_ADDR_LE_ANONYMOUS =          0xFF,      /*!< Anonymous Device Address. */
} rtk_bt_addr_type_t;

#if defined(RTK_BT_5_2_L2C_ECFC_SUPPORT) && RTK_BT_5_2_L2C_ECFC_SUPPORT

/**
 * @def       RTK_BT_GAP_ECFC_CHANN_MAX_NUM
 * @brief     The maximum number of channels created at one time.
 */
#define RTK_BT_GAP_ECFC_CHANN_MAX_NUM          5

/**
 * @def       RTK_BT_GAP_ECFC_MIN_MTU
 * @brief     Minimum length of MTU.
 */
#define RTK_BT_GAP_ECFC_MIN_MTU                64

/**
 * @def       RTK_BT_GAP_ECFC_MIN_MPS
 * @brief     Minimum length of MPS.
 */
#define RTK_BT_GAP_ECFC_MIN_MPS                64

/**
 * @def       RTK_BD_ADDR_LEN
 * @brief     Bluetooth device address length macro.
 */
#define RTK_BD_ADDR_LEN                        6

/**
 * @typedef   rtk_bt_gap_ecfc_sec_setting_t
 * @brief     Bluetooth GAP ECFC security setting.
 */
typedef enum {
	RTK_BT_GAP_ECFC_SEC_BIT_AUTHEN      = 0x02, /*!< Authentication is required. */
	RTK_BT_GAP_ECFC_SEC_BIT_MITM        = 0x04, /*!< MITM authentication is required. */
	RTK_BT_GAP_ECFC_SEC_BIT_AUTHOR      = 0x08, /*!< User level authorization is required. */
	RTK_BT_GAP_ECFC_SEC_BIT_ENCRYPT     = 0x10, /*!< Encryption on link is required. */
	RTK_BT_GAP_ECFC_SEC_BIT_SC          = 0x20, /*!< Secure connection on link is required. */
} rtk_bt_gap_ecfc_sec_setting_t;

/**
 * @typedef   rtk_bt_gap_ecfc_data_path_t
 * @brief     Bluetooth GAP ECFC data path type.
 */
typedef enum {
	RTK_BT_GAP_ECFC_DATA_PATH_APP      = 0x00, /*!< Application handles the data. ATT layer will not handle the data. */
	RTK_BT_GAP_ECFC_DATA_PATH_GATT     = 0x01, /*!< ATT layer handles the data. */
} rtk_bt_gap_ecfc_data_path_t;

/**
 * @typedef   rtk_bt_gap_ecfc_param_t
 * @brief     Bluetooth GAP ECFC parameter type.
 */
typedef enum {
	RTK_BT_GAP_ECFC_PARAM_INIT_CREDITS      = 0x430,    /*!< Local initial credits. Write-only.
                                                         *   Default: 4.
                                                         *   Range: 1 to 65535. */
	RTK_BT_GAP_ECFC_PARAM_LOCAL_MPS         = 0x431,    /*!< Local MPS. Write-only.
                                                         *   Default: 254.
                                                         *   Range: 64 to 65533. */
} rtk_bt_gap_ecfc_param_t;

/**
 * @struct    rtk_bt_gap_ecfc_param_set_t
 * @brief     Bluetooth GAP ECFC set parameter definition.
 */
typedef struct {
	rtk_bt_gap_ecfc_param_t type;       /*!< Parameter type to set. */
	uint16_t value;                     /*!< Value to set. */
} rtk_bt_gap_ecfc_param_set_t;

/**
 * @struct    rtk_bt_gap_ecfc_register_t
 * @brief     Bluetooth GAP ECFC register protocol definition.
 */
typedef struct {
	uint16_t psm;                             /*!< Protocol/Service Multiplexer associated with the callback function.
                                               *   Set to @ref RTK_BT_PSM_EATT if channels are created for EATT. */
	bool is_le;                               /*!< Transport type of the protocol.
                                               *   - true: LE transport.
                                               *   - false: BR/EDR transport. */
	uint16_t *p_proto_id;                     /*!< Protocol ID assigned by the GAP ECFC module, used in @ref rtk_bt_gap_ecfc_send_conn_req.
                                               *   Must not be NULL. */
	rtk_bt_gap_ecfc_data_path_t data_path;    /*!< Data path used to deliver received data. */
} rtk_bt_gap_ecfc_register_t;

/**
 * @struct    rtk_bt_gap_ecfc_sec_reg_req_t
 * @brief     Bluetooth GAP ECFC security register/unregister request definition.
 */
typedef struct {
	bool is_le;                               /*!< Transport type of the protocol.
                                               *   - true: LE transport.
                                               *   - false: BR/EDR transport. */
	bool active;                              /*!< Whether to register or unregister the security entry.
                                               *   - true: register.
                                               *   - false: unregister. */
	uint16_t psm;                             /*!< PSM value for the protocol.
                                               *   Set to @ref RTK_BT_PSM_EATT if channels are created for EATT. */
	uint16_t uuid;                            /*!< BT protocol/profile UUID.
                                               *   Ignored when is_le is true. */
	uint8_t requirement;                      /*!< Security requirement of the entry.
                                               *   Valid values are combinations of @ref rtk_bt_gap_ecfc_sec_setting_t. */
	uint8_t key_size;                         /*!< Key size requirement of the entry.
                                               *   Max size: 16. */
} rtk_bt_gap_ecfc_sec_reg_req_t;

/**
 * @struct    rtk_bt_gap_ecfc_conn_req_t
 * @brief     Bluetooth GAP ECFC connection request definition.
 */
typedef struct {
	uint16_t psm;                             /*!< Protocol/Service Multiplexer of the L2CAP ECFC channel that will be established.
                                               *   Set to @ref RTK_BT_PSM_EATT if channels are created for EATT. */
	uint16_t proto_id;                        /*!< Protocol ID assigned by @ref rtk_bt_gap_ecfc_register. */
	uint16_t local_mtu;                       /*!< Preferred MTU size value of the L2CAP ECFC channel. */
	uint8_t bd_addr[RTK_BD_ADDR_LEN];         /*!< Bluetooth address of the remote device. */
	uint8_t bd_type;                          /*!< Bluetooth address type of the remote device. */
	uint8_t cid_num;                          /*!< The number of the L2CAP ECFC channels to establish.
                                               *   Range: 1 to RTK_BT_GAP_ECFC_CHANN_MAX_NUM. */
} rtk_bt_gap_ecfc_conn_req_t;

/**
 * @typedef   rtk_bt_gap_ecfc_conn_cfm_cause_t
 * @brief     Bluetooth GAP ECFC connection confirm cause definition.
 */
typedef enum {
	RTK_BT_L2C_ECFC_ALL_CONN_SUCCESS = 0x0000,                                  /*!< All connections successful. */
	RTK_BT_L2C_ECFC_CONN_RSP_ALL_CONN_PSM_NOT_SUPPORTED = 0x0002,               /*!< All connections refused - SPSM not supported. */
	RTK_BT_L2C_ECFC_CONN_RSP_SOME_CONN_NO_RESOURCES = 0x0004,                   /*!< Some connections refused - insufficient resources available. */
	RTK_BT_L2C_ECFC_CONN_RSP_ALL_CONN_INSUFFICIENT_AUTHEN = 0x0005,             /*!< All connections refused - insufficient authentication. */
	RTK_BT_L2C_ECFC_CONN_RSP_ALL_CONN_INSUFFICIENT_AUTHOR = 0x0006,             /*!< All connections refused - insufficient authorization. */
	RTK_BT_L2C_ECFC_CONN_RSP_ALL_CONN_INSUFFICIENT_ENCRYP_KEY_SIZE = 0x0007,    /*!< All connections refused - insufficient encryption key size. */
	RTK_BT_L2C_ECFC_CONN_RSP_ALL_CONN_INSUFFICIENT_ENCRYP = 0x0008,             /*!< All connections refused - insufficient encryption. */
	RTK_BT_L2C_ECFC_CONN_RSP_SOME_CONN_INVALID_SOURCE_CID = 0x0009,             /*!< Some connections refused - invalid Source CID. */
	RTK_BT_L2C_ECFC_CONN_RSP_SOME_CONN_SOURCE_CID_ALREADY_ALLOC = 0x000A,       /*!< Some connections refused - Source CID already allocated. */
	RTK_BT_L2C_ECFC_CONN_RSP_ALL_CONN_UNACCEPTABLE_PARAMS = 0x000B,             /*!< All connections refused - unacceptable parameters. */
	RTK_BT_L2C_ECFC_CONN_RSP_ALL_CONN_INVALID_PARAMS = 0x000C,                  /*!< All connections refused - invalid parameters. */
	RTK_BT_L2C_ECFC_CONN_RSP_ALL_CONN_PENDING_NO_INFO = 0x000D,                 /*!< All connections pending - no further information available. */
	RTK_BT_L2C_ECFC_CONN_RSP_ALL_CONN_PENDING_AUTHER = 0x000E,                  /*!< All connections pending - authentication pending. */
	RTK_BT_L2C_ECFC_CONN_RSP_ALL_CONN_PENDING_AUTHOR = 0x000F,                  /*!< All connections pending - authorization pending. */
} rtk_bt_gap_ecfc_conn_cfm_cause_t;

/**
 * @struct    rtk_bt_gap_ecfc_conn_cfm_t
 * @brief     Bluetooth GAP ECFC connection confirm definition.
 */
typedef struct {
	uint16_t conn_handle;                             /*!< Connection handle of the ACL connection associated with this ECFC connection request.
                                                       *   Copied from the @ref RTK_BT_GAP_EVT_ECFC_CONN_REQ_IND event.
                                                       *   Range: 0x0000 to 0x0EFF. */
	uint16_t identity_id;                             /*!< Identity ID.
                                                       *   The value can be obtained from @ref RTK_BT_GAP_EVT_ECFC_CONN_REQ_IND. */
	rtk_bt_gap_ecfc_conn_cfm_cause_t cause;           /*!< Confirmation cause for the connection request from the remote device. */
	uint16_t p_cid[RTK_BT_GAP_ECFC_CHANN_MAX_NUM];    /*!< CID array allowed to be established.
                                                       *   Must be a subset of the CID array reported in @ref RTK_BT_GAP_EVT_ECFC_CONN_REQ_IND. */
	uint8_t cid_num;                                  /*!< The number of the L2CAP ECFC channels allowed to be established.
                                                       *   Range: 1 to RTK_BT_GAP_ECFC_CHANN_MAX_NUM. */
	uint16_t local_mtu;                               /*!< Preferred local MTU size value of the L2CAP ECFC channel. */
} rtk_bt_gap_ecfc_conn_cfm_t;

/**
 * @struct    rtk_bt_gap_ecfc_disconn_req_t
 * @brief     Bluetooth GAP ECFC disconnection request definition.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection to disconnect the ECFC channel.
                              *   Copied from the @ref RTK_BT_GAP_EVT_ECFC_CONN_CMPL_IND event.
                              *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;            /*!< Local channel ID of the L2CAP ECFC channel to disconnect. */
} rtk_bt_gap_ecfc_disconn_req_t;

/**
 * @struct    rtk_bt_gap_ecfc_send_data_t
 * @brief     Bluetooth GAP ECFC send data request definition.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection to send the ECFC data.
                              *   Copied from the @ref RTK_BT_GAP_EVT_ECFC_CONN_CMPL_IND event.
                              *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;            /*!< Local channel ID.
                              *   The value can be obtained from @ref RTK_BT_GAP_EVT_ECFC_CONN_CMPL_IND. */
	uint8_t *p_data;         /*!< Pointer to the data to send.
                              *   Must not be NULL. */
	uint16_t length;         /*!< Length of the data to send.
                              *   Range: 0 to local_mtu. */
} rtk_bt_gap_ecfc_send_data_t;

/**
 * @struct    rtk_bt_gap_ecfc_reconf_req_t
 * @brief     Bluetooth GAP ECFC reconfigure request definition.
 */
typedef struct {
	uint16_t conn_handle;                             /*!< Connection handle of the ACL connection to reconfigure the ECFC channels.
                                                       *   Copied from the @ref RTK_BT_GAP_EVT_ECFC_CONN_CMPL_IND event.
                                                       *   Range: 0x0000 to 0x0EFF. */
	uint16_t p_cid[RTK_BT_GAP_ECFC_CHANN_MAX_NUM];    /*!< Local CID array to reconfigure. */
	uint8_t cid_num;                                  /*!< The number of CIDs in the p_cid array.
                                                       *   Range: 1 to RTK_BT_GAP_ECFC_CHANN_MAX_NUM. */
	uint16_t local_mtu;                               /*!< Preferred local MTU size value of the L2CAP ECFC channel. */
	uint16_t local_mps;                               /*!< Preferred local MPS value of the L2CAP ECFC channel. */
} rtk_bt_gap_ecfc_reconf_req_t;

/**
 * @struct    rtk_bt_ecfc_conn_cmpl_ind_t
 * @brief     Bluetooth L2CAP ECFC channel connection result indication.
 */
typedef struct {
	uint16_t proto_id;                              /*!< Protocol ID assigned by GAP ECFC module. */
	uint16_t err;                                   /*!< Result of establishing the L2CAP ECFC channels with the remote device. */
	uint16_t conn_handle;                           /*!< Connection handle of the ACL connection on which the ECFC channels were established.
                                                     *   Range: 0x0000 to 0x0EFF. */
	uint8_t cid_num;                                /*!< The number of the L2CAP ECFC channels successfully established. */
	uint16_t cid[RTK_BT_GAP_ECFC_CHANN_MAX_NUM];    /*!< Local CID array assigned by Bluetooth stack. */
	uint16_t remote_mtu;                            /*!< Remote MTU value. */
	uint16_t local_mtu;                             /*!< Local MTU value. */
	uint16_t local_mps;                             /*!< Local MPS value. */
	uint16_t ds_data_offset;                        /*!< Offset used to fill user data when sending L2CAP data to the remote device. */
	uint8_t bd_addr[RTK_BD_ADDR_LEN];               /*!< Bluetooth address of the remote device. */
	rtk_bt_addr_type_t bd_type;                     /*!< Bluetooth address type of the remote device. */
} rtk_bt_ecfc_conn_cmpl_ind_t;

/**
 * @struct    rtk_bt_ecfc_data_ind_t
 * @brief     Bluetooth L2CAP ECFC data reception indication.
 */
typedef struct {
	uint16_t proto_id;       /*!< Protocol ID assigned by GAP ECFC module. */
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection on which the L2CAP ECFC data was received.
                              *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;            /*!< Local CID assigned by Bluetooth stack. */
	uint16_t length;         /*!< Length of L2CAP data. */
	uint8_t *data;           /*!< L2CAP data. */
} rtk_bt_ecfc_data_ind_t;

/**
 * @struct    rtk_bt_ecfc_conn_req_ind_t
 * @brief     Bluetooth L2CAP ECFC channel connection request reception indication.
 */
typedef struct {
	uint16_t proto_id;                              /*!< Protocol ID assigned by GAP ECFC module. */
	uint16_t conn_handle;                           /*!< Connection handle of the ACL connection on which the ECFC connection request was received.
                                                     *   Range: 0x0000 to 0x0EFF. */
	uint16_t remote_mtu;                            /*!< Remote MTU value. */
	uint8_t cid_num;                                /*!< The number of the L2CAP ECFC channels requested to be established. */
	uint16_t cid[RTK_BT_GAP_ECFC_CHANN_MAX_NUM];    /*!< Local CID array assigned by Bluetooth stack. */
	uint8_t bd_addr[RTK_BD_ADDR_LEN];               /*!< Bluetooth address of the remote device. */
	rtk_bt_addr_type_t bd_type;                     /*!< Bluetooth address type of the remote device. */
	uint16_t identity_id;                           /*!< Identity ID for @ref rtk_bt_gap_ecfc_send_conn_cfm. */
} rtk_bt_ecfc_conn_req_ind_t;

/**
 * @struct    rtk_bt_ecfc_disconn_ind_t
 * @brief     Bluetooth L2CAP ECFC channel disconnection indication.
 */
typedef struct {
	uint16_t proto_id;       /*!< Protocol ID assigned by GAP ECFC module. */
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection on which the ECFC channel was disconnected.
                              *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;            /*!< Local CID assigned by Bluetooth stack. */
	uint16_t cause;          /*!< L2CAP channel disconnect reason received. */
} rtk_bt_ecfc_disconn_ind_t;

/**
 * @struct    rtk_bt_ecfc_reconf_req_ind_t
 * @brief     Bluetooth L2CAP ECFC channel reconfigure request reception indication.
 */
typedef struct {
	uint16_t proto_id;                              /*!< Protocol ID assigned by GAP ECFC module. */
	uint16_t conn_handle;                           /*!< Connection handle of the ACL connection on which the ECFC reconfiguration request was received.
                                                     *   Range: 0x0000 to 0x0EFF. */
	uint8_t cid_num;                                /*!< The number of the L2CAP ECFC channels that need to be reconfigured. */
	uint16_t cid[RTK_BT_GAP_ECFC_CHANN_MAX_NUM];    /*!< Local CID array assigned by Bluetooth stack. */
	uint16_t remote_mtu;                            /*!< Remote MTU. */
	uint16_t remote_mps;                            /*!< Remote MPS. */
} rtk_bt_ecfc_reconf_req_ind_t;

/**
 * @struct    rtk_bt_ecfc_reconf_rsp_ind_t
 * @brief     Bluetooth L2CAP ECFC channel reconfigure response reception indication.
 */
typedef struct {
	uint16_t proto_id;                              /*!< Protocol ID assigned by GAP ECFC module. */
	uint16_t cause;                                 /*!< Result of reconfiguration. */
	uint16_t conn_handle;                           /*!< Connection handle of the ACL connection on which the ECFC channels were reconfigured.
                                                     *   Range: 0x0000 to 0x0EFF. */
	uint8_t cid_num;                                /*!< The number of the L2CAP ECFC channels successfully reconfigured. */
	uint16_t cid[RTK_BT_GAP_ECFC_CHANN_MAX_NUM];    /*!< Local CID array assigned by Bluetooth stack. */
	uint16_t local_mtu;                             /*!< Local MTU value. */
	uint16_t local_mps;                             /*!< Local MPS value. */
} rtk_bt_ecfc_reconf_rsp_ind_t;
#endif

/**
 * @struct    rtk_bt_gap_vendor_cmd_param_t
 * @brief     Bluetooth GAP vendor command parameter.
 */
typedef struct {
	uint16_t op;           /*!< Vendor command opcode. */
	uint8_t len;           /*!< Length of the command parameters. */
	uint8_t *cmd_param;    /*!< Pointer to the parameter data to write. */
} rtk_bt_gap_vendor_cmd_param_t;

/********************************* Functions Declaration *******************************/

#if defined(RTK_BT_5_2_L2C_ECFC_SUPPORT) && RTK_BT_5_2_L2C_ECFC_SUPPORT
/**
 * @defgroup  bt_gap BT GAP APIs
 * @brief     BT GAP related function APIs.
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Set a GAP ECFC parameter.
 * @param[in] type: Parameter type.
 * @param[in] value: Parameter value.
 *                   This is dependent on the parameter type and will be cast to the appropriate data type.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gap_ecfc_set_param(rtk_bt_gap_ecfc_param_t type, uint16_t value);

/**
 * @brief     Register a callback function to handle L2CAP ECFC related messages for a specific PSM.
 * @param[in] p_param: Registration parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gap_ecfc_register(rtk_bt_gap_ecfc_register_t *p_param);

/**
 * @brief     Send a request to register/unregister a protocol security entry over L2CAP. The security will be used when establishing an L2CAP ECFC channel.
 * @param[in] p_param: Security registration parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gap_ecfc_send_sec_reg_req(rtk_bt_gap_ecfc_sec_reg_req_t *p_param);

/**
 * @brief     Send a request to create L2CAP ECFC connections. The result is reported asynchronously via event @ref RTK_BT_GAP_EVT_ECFC_CONN_CMPL_IND.
 * @param[in] p_param: Connection request parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gap_ecfc_send_conn_req(rtk_bt_gap_ecfc_conn_req_t *p_param);

/**
 * @brief     Send a confirmation for a L2CAP ECFC connection request from the remote device. The result is reported asynchronously
 *            via event @ref RTK_BT_GAP_EVT_ECFC_CONN_CMPL_IND.
 * @param[in] p_param: Connection confirmation parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gap_ecfc_send_conn_cfm(rtk_bt_gap_ecfc_conn_cfm_t *p_param);

/**
 * @brief     Send a request to disconnect an L2CAP ECFC connection. The result is reported asynchronously via event @ref RTK_BT_GAP_EVT_ECFC_DISCONN_IND.
 * @param[in] conn_handle: Connection handle of the ACL connection to disconnect the ECFC channel.
 *                         The value can be obtained from @ref RTK_BT_GAP_EVT_ECFC_CONN_CMPL_IND.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] cid: Local channel ID of the L2CAP ECFC channel to disconnect.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gap_ecfc_send_disconn_req(uint16_t conn_handle, uint16_t cid);

/**
 * @brief     Send L2CAP ECFC channel data to the remote device.
 * @param[in] conn_handle: Connection handle of the ACL connection to send the ECFC data.
 *                         The value can be obtained from @ref RTK_BT_GAP_EVT_ECFC_CONN_CMPL_IND.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] cid: Local channel ID.
 * @param[in] p_data: Pointer to the data to send.
 *                    Must not be NULL.
 * @param[in] length: Length of the data to send.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API can only be used when the data_path passed to @ref rtk_bt_gap_ecfc_register is @ref RTK_BT_GAP_ECFC_DATA_PATH_APP.
 */
uint16_t rtk_bt_gap_ecfc_send_data(uint16_t conn_handle, uint16_t cid, uint8_t *p_data, uint16_t length);

/**
 * @brief     Send a request to reconfigure an L2CAP ECFC channel. The result is reported asynchronously via event @ref RTK_BT_GAP_EVT_ECFC_RECONF_RSP_IND.
 * @param[in] p_param: Reconfiguration request parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gap_ecfc_send_reconf_req(rtk_bt_gap_ecfc_reconf_req_t *p_param);

/**
 * @}
 */
#endif

/**
 * @brief     Send a vendor command to the controller.
 * @param[in] vendor_param: Vendor command parameters.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_gap_vendor_cmd_req(rtk_bt_gap_vendor_cmd_param_t *vendor_param);

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_GAP_H__ */
