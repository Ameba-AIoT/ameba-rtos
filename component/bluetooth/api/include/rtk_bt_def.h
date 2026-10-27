/**
 * @file      rtk_bt_def.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth status and error definition.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_DEF_H__
#define __RTK_BT_DEF_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @typedef   rtk_bt_status_t
 * @brief     Bluetooth API status definition.
 */
typedef enum {
	RTK_BT_STATUS_DONE = 0,    /*!< The operation is done. */
	RTK_BT_STATUS_CONTINUE,    /*!< The operation needs to continue. */
	RTK_BT_STATUS_FAIL,        /*!< The operation failed. */
} rtk_bt_status_t;

/**
 * @enum      rtk_bt_err_group
 * @brief     Bluetooth API error group mask.
 */
enum rtk_bt_err_group {
	RTK_BT_ERR_COMMON_GROUP     =   0x0000,    /*!< BT common error group, @ref rtk_bt_err_common. */
	RTK_BT_ERR_HCI_GROUP        =   0x0100,    /*!< BT HCI error group, @ref rtk_bt_err_hci. */
	RTK_BT_ERR_L2CAP_GROUP      =   0x0200,    /*!< BT L2CAP error group. Different L2CAP packets may have different subgroup errors, @ref rtk_bt_err_l2cap_subgroup. */
	RTK_BT_ERR_ATT_GROUP        =   0x0400,    /*!< BT ATT error group, @ref rtk_bt_err_att. */
	RTK_BT_ERR_SM_GROUP         =   0x0500,    /*!< BT SMP error group, @ref rtk_bt_err_smp. */
	RTK_BT_ERR_GATT_GROUP       =   0x0C00,    /*!< BT GATT error group, @ref rtk_bt_err_gatt. */
};

/**
 * @enum      rtk_bt_err_common
 * @brief     Bluetooth API common sub error code.
 */
enum rtk_bt_err_common {
	RTK_BT_OK                       =   0x00,    /*!< Success. */
	RTK_BT_FAIL                     =   0x01,    /*!< Common failure. */
	RTK_BT_ERR_NO_PERM              =   0x02,    /*!< The operation is not permitted. */
	RTK_BT_ERR_NOT_READY            =   0x03,    /*!< BT is not in the state required by the operation, e.g. BT is not enabled. */
	RTK_BT_ERR_NO_MEMORY            =   0x04,    /*!< Memory allocation failed. */
	RTK_BT_ERR_MISMATCH             =   0x05,    /*!< The request does not match the current configuration or state. */
	RTK_BT_ERR_BUSY                 =   0x06,    /*!< The resource is busy. */
	RTK_BT_ERR_OS_OPERATION         =   0x07,    /*!< An OS operation failed, e.g. creating a semaphore, queue or task. */
	RTK_BT_ERR_MSG_SEND             =   0x08,    /*!< Failed to send the message to the stack task. */
	RTK_BT_ERR_UNSUPPORTED          =   0x09,    /*!< The operation is not supported. */
	RTK_BT_ERR_PARAM_INVALID        =   0x0A,    /*!< The parameter is invalid. */
	RTK_BT_ERR_UNHANDLED            =   0x0B,    /*!< The request or event is not handled. */
	RTK_BT_ERR_NEED_TRY_AGAIN       =   0x0C,    /*!< The operation cannot be done now, try again later. */
	RTK_BT_ERR_ALREADY_DONE         =   0x0D,    /*!< The operation has already been done. */
	RTK_BT_ERR_ALREADY_IN_PROGRESS  =   0x0E,    /*!< The operation is already in progress. */
	RTK_BT_ERR_POINTER_INVALID      =   0x0F,    /*!< A pointer is invalid, e.g. NULL. */
	RTK_BT_ERR_NO_CREDITS           =   0x10,    /*!< No credits are available to send data. */
	RTK_BT_ERR_NO_BUFS              =   0x11,    /*!< No buffer is available. */
	RTK_BT_ERR_NO_ENTRY             =   0x12,    /*!< No matching entry is found. */
	RTK_BT_ERR_NO_RESOURCE          =   0x13,    /*!< No resource is available. */
	RTK_BT_ERR_ADV_LENGTH_INVALID   =   0x14,    /*!< The advertising data length is invalid. */
	RTK_BT_ERR_NO_CONNECTION        =   0x15,    /*!< No connection is established. */
	RTK_BT_ERR_IRK_NOT_FOUND        =   0x16,    /*!< The IRK is not found. */
	RTK_BT_ERR_STATE_INVALID        =   0x17,    /*!< The current state does not allow the operation. */
	RTK_BT_ERR_QUEUE_FULL           =   0x18,    /*!< The message queue is full. */
	RTK_BT_ERR_SEND_REQ_FAILURE     =   0x19,    /*!< Failed to send the request. */
	RTK_BT_ERR_PDU_SIZE_INVALID     =   0x1A,    /*!< The PDU size is invalid. */
	RTK_BT_ERR_CONN_NUM_LIMIT       =   0x1B,    /*!< The maximum number of connections is reached. */
	RTK_BT_ERR_NO_BOND              =   0x1C,    /*!< No bonding information is found. */
	RTK_BT_ERR_SYNC_TIMEOUT         =   0x1D,    /*!< Timed out when waiting for the command to be executed by the stack. */
	RTK_BT_ERR_LOWER_STACK_API      =   0x1E,    /*!< A lower stack API returned a failure. */
	RTK_BT_ERR_LOWER_STACK_CB       =   0x1F,    /*!< A lower stack callback reported a failure. */
	RTK_BT_ERR_CREATE_CONN_TIMEOUT  =   0x20,    /*!< Timed out when creating the connection. */
	RTK_BT_ERR_NO_CASE_ELEMENT      =   0x21,    /*!< The action is unknown. */
	RTK_BT_ERR_UNKNOWN              =   0xFF,    /*!< Unknown error. */
};

/**
 * @enum      rtk_bt_err_hci
 * @brief     Bluetooth API HCI sub error code.
 */
enum rtk_bt_err_hci {
	RTK_BT_HCI_ERR_UNKNOWN_CMD                     =    0x01,    /*!< Unknown HCI command. */
	RTK_BT_HCI_ERR_UNKNOWN_CONN_ID                 =    0x02,    /*!< Unknown connection identifier. */
	RTK_BT_HCI_ERR_HARDWARE_FAIL                   =    0x03,    /*!< Hardware failure. */
	RTK_BT_HCI_ERR_PAGE_TIMEOUT                    =    0x04,    /*!< Page timeout. */
	RTK_BT_HCI_ERR_AUTHEN_FAIL                     =    0x05,    /*!< Authentication failure. */
	RTK_BT_HCI_ERR_KEY_MISSING                     =    0x06,    /*!< PIN or key missing. */
	RTK_BT_HCI_ERR_MEMORY_FULL                     =    0x07,    /*!< Memory capacity exceeded. */
	RTK_BT_HCI_ERR_CONN_TIMEOUT                    =    0x08,    /*!< Connection timeout. */
	RTK_BT_HCI_ERR_MAX_NUM_CONN                    =    0x09,    /*!< Connection limit exceeded. */
	RTK_BT_HCI_ERR_MAX_NUM_SCO_CONN                =    0x0A,    /*!< Synchronous connection limit to a device exceeded. */
	RTK_BT_HCI_ERR_ACL_CONN_EXIST                  =    0x0B,    /*!< Connection already exists. */
	RTK_BT_HCI_ERR_CMD_DISALLOWED                  =    0x0C,    /*!< Command disallowed. */
	RTK_BT_HCI_ERR_REJECT_LIMITED_RESOURCE         =    0x0D,    /*!< Connection rejected due to limited resources. */
	RTK_BT_HCI_ERR_REJECT_SECURITY_REASON          =    0x0E,    /*!< Connection rejected due to security reasons. */
	RTK_BT_HCI_ERR_REJECT_UNACCEPTABLE_ADDR        =    0x0F,    /*!< Rejected due to unacceptable BD_ADDR. */
	RTK_BT_HCI_ERR_HOST_TIMEOUT                    =    0x10,    /*!< Connection accept timeout exceeded. */
	RTK_BT_HCI_ERR_UNSUPPORTED_PARAM               =    0x11,    /*!< Unsupported feature or parameter value. */
	RTK_BT_HCI_ERR_INVALID_PARAM                   =    0x12,    /*!< Invalid HCI command parameters. */
	RTK_BT_HCI_ERR_REMOTE_USER_TERMINATE           =    0x13,    /*!< Remote user terminated the connection. */
	RTK_BT_HCI_ERR_REMOTE_LOW_RESOURCE             =    0x14,    /*!< Remote device terminated the connection due to low resources. */
	RTK_BT_HCI_ERR_REMOTE_POWER_OFF                =    0x15,    /*!< Remote device terminated the connection due to power off. */
	RTK_BT_HCI_ERR_LOCAL_HOST_TERMINATE            =    0x16,    /*!< Connection terminated by the local host. */
	RTK_BT_HCI_ERR_REPEATED_ATTEMPTS               =    0x17,    /*!< Repeated attempts. */
	RTK_BT_HCI_ERR_PARING_NOT_ALLOWED              =    0x18,    /*!< Pairing not allowed. */
	RTK_BT_HCI_ERR_UNKNOWN_LMP_PDU                 =    0x19,    /*!< Unknown LMP PDU. */
	RTK_BT_HCI_ERR_UNSUPPORTED_REMOTE_FEAT         =    0x1A,    /*!< Unsupported remote feature / Unsupported LMP feature. */
	RTK_BT_HCI_ERR_SCO_OFFSET_REJECTED             =    0x1B,    /*!< SCO offset rejected. */
	RTK_BT_HCI_ERR_SCO_INTERVAL_REJECTED           =    0x1C,    /*!< SCO interval rejected. */
	RTK_BT_HCI_ERR_SCO_AIR_MODE_REJECTED           =    0x1D,    /*!< SCO air mode rejected. */
	RTK_BT_HCI_ERR_INVALID_LMP_PARAM               =    0x1E,    /*!< Invalid LMP parameters / Invalid LL parameters. */
	RTK_BT_HCI_ERR_UNSPECIFIED_ERROR               =    0x1F,    /*!< Unspecified error. */
	RTK_BT_HCI_ERR_UNSUPPORTED_LMP_PARAM           =    0x20,    /*!< Unsupported LMP parameter value / Unsupported LL parameter value. */
	RTK_BT_HCI_ERR_ROLE_CHANGE_NOT_ALLOWED         =    0x21,    /*!< Role change not allowed. */
	RTK_BT_HCI_ERR_LMP_RESPONSE_TIMEOUT            =    0x22,    /*!< LMP response timeout / LL response timeout. */
	RTK_BT_HCI_ERR_LMP_ERROR_TRANS_COLLISION       =    0x23,    /*!< LMP error transaction collision / LL procedure collision. */
	RTK_BT_HCI_ERR_LMP_PDU_NOT_ALLOWED             =    0x24,    /*!< LMP PDU not allowed. */
	RTK_BT_HCI_ERR_ENCRYPT_MODE_NOT_ACCEPTABLE     =    0x25,    /*!< Encryption mode not acceptable. */
	RTK_BT_HCI_ERR_UNIT_KEY_USED                   =    0x26,    /*!< Link key cannot be changed. */
	RTK_BT_HCI_ERR_QOS_NOT_SUPPORTED               =    0x27,    /*!< Requested QoS not supported. */
	RTK_BT_HCI_ERR_INSTANT_PASSED                  =    0x28,    /*!< Instant passed. */
	RTK_BT_HCI_ERR_PAIR_UNIT_KEY_NOT_SUPPORT       =    0x29,    /*!< Pairing with unit key not supported. */
	RTK_BT_HCI_ERR_DIFF_TRANS_COLLISION            =    0x2A,    /*!< Different transaction collision. */
	RTK_BT_HCI_ERR_QOS_UNACCEPTABLE_PARAM          =    0x2C,    /*!< QoS unacceptable parameter. */
	RTK_BT_HCI_ERR_QOS_REJECT                      =    0x2D,    /*!< QoS rejected. */
	RTK_BT_HCI_ERR_CHANN_ASSESS_NOT_SUPPORT        =    0x2E,    /*!< Channel assessment not supported. */
	RTK_BT_HCI_ERR_INSUFFICIENT_SECURITY           =    0x2F,    /*!< Insufficient security. */
	RTK_BT_HCI_ERR_PARAM_OUT_OF_RANGE              =    0x30,    /*!< Parameter out of mandatory range. */
	RTK_BT_HCI_ERR_ROLE_SWITCH_PANDING             =    0x32,    /*!< Role switch pending. */
	RTK_BT_HCI_ERR_RESERVED_SLOT_VIOLATION         =    0x34,    /*!< Reserved slot violation. */
	RTK_BT_HCI_ERR_ROLE_SWITCH_FAILED              =    0x35,    /*!< Role switch failed. */
	RTK_BT_HCI_ERR_EXT_INQUIRY_RSP_TOO_LARGE       =    0x36,    /*!< Extended inquiry response too large. */
	RTK_BT_HCI_ERR_SSP_NOT_SUPPORTED_BY_HOST       =    0x37,    /*!< Secure Simple Pairing not supported by the host. */
	RTK_BT_HCI_ERR_HOST_BUSY_PAIRING               =    0x38,    /*!< Host busy - pairing. */
	RTK_BT_HCI_ERR_REJECT_NO_SUITABLE_CHANN        =    0x39,    /*!< Rejected due to no suitable channel found. */
	RTK_BT_HCI_ERR_CONTROLLER_BUSY                 =    0x3A,    /*!< Controller busy. */
	RTK_BT_HCI_ERR_UNACCEPTABLE_CONN_INTERVAL      =    0x3B,    /*!< Unacceptable connection parameters. */
	RTK_BT_HCI_ERR_DIRECTED_ADV_TIMEOUT            =    0x3C,    /*!< Advertising timeout. */
	RTK_BT_HCI_ERR_MIC_FAILURE                     =    0x3D,    /*!< Connection terminated due to MIC failure. */
	RTK_BT_HCI_ERR_FAIL_TO_ESTABLISH_CONN          =    0x3E,    /*!< Connection failed to be established. */
	RTK_BT_HCI_ERR_MAC_CONN_FAIL                   =    0x3F,    /*!< MAC connection failed. */
	RTK_BT_HCI_ERR_COARSE_CLOCK_ADJUST_REJECTED    =    0x40,    /*!< Coarse clock adjustment rejected but will try to adjust using clock dragging. */
	RTK_BT_HCI_ERR_TYPE0_SUBMAP_NOT_DEFINED        =    0x41,    /*!< Type0 submap not defined. */
	RTK_BT_HCI_ERR_UNKNOWN_ADVERTISING_IDENTIFIER  =    0x42,    /*!< Unknown advertising identifier. */
	RTK_BT_HCI_ERR_LIMIT_REACHED                   =    0x43,    /*!< Limit reached. */
	RTK_BT_HCI_ERR_OPERATION_CANCELLED_BY_HOST     =    0x44,    /*!< Operation cancelled by the host. */
	/* Local error code */
	RTK_BT_HCI_ERR_INVALID_ADDR                    =    0x64,    /*!< Invalid address. */
	RTK_BT_HCI_ERR_TIMEOUT                         =    0x65,    /*!< Timeout. */
	RTK_BT_HCI_ERR_OUT_OF_SYNC                     =    0x66,    /*!< Out of synchronization. */
	RTK_BT_HCI_ERR_NO_DESCRIPTOR                   =    0x67,    /*!< No descriptor. */
	RTK_BT_HCI_ERR_NO_MEMORY                       =    0x68,    /*!< No memory. */
};

/**
 * @enum      rtk_bt_err_l2cap_subgroup
 * @brief     Bluetooth API L2CAP subgroup error code.
 */
enum rtk_bt_err_l2cap_subgroup {
	RTK_BT_ERR_L2CAP_LE_COC_SUBGROUP  =   0x30,    /*!< L2CAP LE credit based connection-oriented channel(COC) error subgroup, @ref rtk_bt_err_l2cap_le_coc. */
	RTK_BT_ERR_L2CAP_ECFC_SUBGROUP    =   0x50,    /*!< L2CAP enhanced credit based flow control(ECFC) error subgroup, @ref rtk_bt_gap_ecfc_conn_cfm_cause_t. */
};

/**
 * @enum      rtk_bt_err_l2cap_le_coc
 * @brief     Bluetooth API LE credit based connection-oriented channel (COC) error code.
 */
enum rtk_bt_err_l2cap_le_coc {
	RTK_BT_LE_COC_ERR_PSM_NOT_SUPPORTED               =   0x02,    /*!< Connection refused - SPSM not supported. */
	RTK_BT_LE_COC_ERR_NO_RESOURCES                    =   0x04,    /*!< Connection refused - no resources available. */
	RTK_BT_LE_COC_ERR_INSUFFICIENT_AUTHEN             =   0x05,    /*!< Connection refused - insufficient authentication. */
	RTK_BT_LE_COC_ERR_INSUFFICIENT_AUTHOR             =   0x06,    /*!< Connection refused - insufficient authorization. */
	RTK_BT_LE_COC_ERR_INSUFFICIENT_ENCRYP_KEY_SIZE    =   0x07,    /*!< Connection refused - insufficient encryption key size. */
	RTK_BT_LE_COC_ERR_INSUFFICIENT_ENCRYP             =   0x08,    /*!< Connection refused - insufficient encryption. */
	RTK_BT_LE_COC_ERR_INVALID_SOURCE_CID              =   0x09,    /*!< Connection refused - invalid Source CID. */
	RTK_BT_LE_COC_ERR_SOURCE_CID_ALREADY_ALLOC        =   0x0A,    /*!< Connection refused - Source CID already allocated. */
};

/**
 * @enum      rtk_bt_err_att
 * @brief     Bluetooth API ATT sub error code.
 */
enum rtk_bt_err_att {
	RTK_BT_ATT_ERR_INVALID_HANDLE               =   0x01,    /*!< Invalid handle. */
	RTK_BT_ATT_ERR_READ_NOT_PERMITTED           =   0x02,    /*!< Read not permitted. */
	RTK_BT_ATT_ERR_WRITE_NOT_PERMITTED          =   0x03,    /*!< Write not permitted. */
	RTK_BT_ATT_ERR_INVALID_PDU                  =   0x04,    /*!< Invalid PDU. */
	RTK_BT_ATT_ERR_INSUFFICIENT_AUTHEN          =   0x05,    /*!< Insufficient authentication. */
	RTK_BT_ATT_ERR_UNSUPPORTED_REQ              =   0x06,    /*!< Request not supported. */
	RTK_BT_ATT_ERR_INVALID_OFFSET               =   0x07,    /*!< Invalid offset. */
	RTK_BT_ATT_ERR_INSUFFICIENT_AUTHOR          =   0x08,    /*!< Insufficient authorization. */
	RTK_BT_ATT_ERR_PREP_QUEUE_FULL              =   0x09,    /*!< Prepare queue full. */
	RTK_BT_ATT_ERR_ATTR_NOT_FOUND               =   0x0A,    /*!< Attribute not found. */
	RTK_BT_ATT_ERR_ATTR_NOT_LONG                =   0x0B,    /*!< Attribute not long. */
	RTK_BT_ATT_ERR_INSUFFICIENT_KEY_SIZE        =   0x0C,    /*!< Encryption key size too short. */
	RTK_BT_ATT_ERR_INVALID_VALUE_SIZE           =   0x0D,    /*!< Invalid attribute value length. */
	RTK_BT_ATT_ERR_UNLIKELY                     =   0x0E,    /*!< Unlikely error. */
	RTK_BT_ATT_ERR_INSUFFICIENT_ENCRYPT         =   0x0F,    /*!< Insufficient encryption. */
	RTK_BT_ATT_ERR_UNSUPPORTED_GROUP_TYPE       =   0x10,    /*!< Unsupported group type. */
	RTK_BT_ATT_ERR_INSUFFICIENT_RESOURCES       =   0x11,    /*!< Insufficient resources. */
	RTK_BT_ATT_ERR_DATABASE_OUT_OF_SYNC         =   0x12,    /*!< Database out of sync. */
	RTK_BT_ATT_ERR_VALUE_NOT_ALLOWED            =   0x13,    /*!< Value not allowed. */
	/* Application Error 0x80 - 0x9F */
	RTK_BT_ATT_ERR_INVALID_VALUE                =   0x80,    /*!< Application error: invalid value. */
	/* Manufacturer specific error */
	RTK_BT_ATT_ERR_INVALID_CCC_BITS             =   0xC0,    /*!< Invalid CCC bits. */
	RTK_BT_ATT_ERR_INVALID_SIGNED_COUNT         =   0xC1,    /*!< Invalid signed count. */
	RTK_BT_ATT_ERR_INVALID_SIGNED_MAC_FAILED    =   0xC2,    /*!< Signed MAC verification failed. */
	RTK_BT_ATT_ERR_MIN_APPLIC_CODE              =   0xC3,    /*!< Minimum application error code. */
	/* Common Profiles and Service Error 0xE0 - 0xFF */
	RTK_BT_ATT_ERR_WRITE_REQUEST_REJECTED       =   0xFC,    /*!< Write request rejected. */
	RTK_BT_ATT_ERR_CCCD_IMPROPERLY_CONFIGURED   =   0xFD,    /*!< Client Characteristic Configuration descriptor improperly configured. */
	RTK_BT_ATT_ERR_PROC_ALREADY_IN_PROGRESS     =   0xFE,    /*!< Procedure already in progress. */
	RTK_BT_ATT_ERR_OUT_OF_RANGE                 =   0xFF,    /*!< Out of range. */
};

/**
 * @enum      rtk_bt_err_smp
 * @brief     Bluetooth API SMP sub error code.
 */
enum rtk_bt_err_smp {
	/* Pairing failed error code */
	RTK_BT_SMP_ERR_PASSKEY_ENTRY_FAIL           =   0x01,    /*!< Passkey entry failed. */
	RTK_BT_SMP_ERR_OOB_NOT_AVAIABLE             =   0x02,    /*!< OOB not available. */
	RTK_BT_SMP_ERR_AUTHEN_REQUIREMENTS          =   0x03,    /*!< Authentication requirements. */
	RTK_BT_SMP_ERR_CFM_VALUE_FAILED             =   0x04,    /*!< Confirm value failed. */
	RTK_BT_SMP_ERR_PAIRING_NOT_SUPPORTED        =   0x05,    /*!< Pairing not supported. */
	RTK_BT_SMP_ERR_ENCRYPTION_KEY_SIZE          =   0x06,    /*!< Encryption key size. */
	RTK_BT_SMP_ERR_CMD_NOT_SUPPORTED            =   0x07,    /*!< Command not supported. */
	RTK_BT_SMP_ERR_UNSPECIFIED_REASON           =   0x08,    /*!< Unspecified reason. */
	RTK_BT_SMP_ERR_REPEATED_ATTEMPTS            =   0x09,    /*!< Repeated attempts. */
	RTK_BT_SMP_ERR_INVALID_PARAM                =   0x0A,    /*!< Invalid parameters. */
	RTK_BT_SMP_ERR_DHKEY_CHECK_FAILED           =   0x0B,    /*!< DHKey check failed. */
	RTK_BT_SMP_ERR_NUMERIC_COMPARISION_FAIL     =   0x0C,    /*!< Numeric comparison failed. */
	RTK_BT_SMP_ERR_BREDR_PAIRING_IN_PROGRESS    =   0x0D,    /*!< BR/EDR pairing in progress. */
	RTK_BT_SMP_ERR_CRS_TRANS_KEY_GEN_NOT_ALLOW  =   0x0E,    /*!< Cross-transport key derivation/generation not allowed. */
	/* Local error code */
	RTK_BT_SM_ERR_NO_ENTRY                      =   0xE1,    /*!< No entry found. */
	RTK_BT_SM_ERR_DB_FULL                       =   0xE2,    /*!< Bond database full. */
	RTK_BT_SM_ERR_INVALID_PARAM                 =   0xE3,    /*!< Invalid parameters. */
	RTK_BT_SM_ERR_INSUFFICIENT_LINK_KEY         =   0xE4,    /*!< Insufficient link key. */
	RTK_BT_SM_ERR_LE_ADDR_NOT_RESOLVED          =   0xE5,    /*!< LE address not resolved. */
	RTK_BT_SM_ERR_INVALID_STATE                 =   0xE6,    /*!< Invalid state. */
	RTK_BT_SM_ERR_NO_RESOURCE                   =   0xE7,    /*!< No resource available. */
	RTK_BT_SM_ERR_LINK_KEY_MISSING              =   0xE8,    /*!< Link key missing. */
	RTK_BT_SM_ERR_DISCONNECT                    =   0xE9,    /*!< Disconnected. */
	RTK_BT_SM_ERR_PARING_NOT_ALLOWED            =   0xEA,    /*!< Pairing not allowed. */
	RTK_BT_SM_ERR_KEY_SAVE_FAILED               =   0xEB,    /*!< Failed to save the key. */
	RTK_BT_SM_ERR_TIMEOUT                       =   0xEC,    /*!< Timeout. */
	RTK_BT_SM_ERR_UNKNOWN                       =   0xED,    /*!< Unknown error. */
	RTK_BT_SM_ERR_NO_PAIRABLE_MODE              =   0xEF,    /*!< Not in pairable mode. */
	RTK_BT_SM_ERR_VENDOR                        =   0xFF,    /*!< Vendor specific error. */
};

/**
 * @enum      rtk_bt_err_gatt
 * @brief     Bluetooth API GATT sub error code.
 */
enum rtk_bt_err_gatt {
	RTK_BT_GATT_ERR_OUT_OF_RESOURCE        =    0x01,    /*!< Out of resource. */
	RTK_BT_GATT_ERR_UNSUPPORTED            =    0x02,    /*!< Operation not supported. */
	RTK_BT_GATT_ERR_INVALID_ROLE           =    0x03,    /*!< Invalid role. */
	RTK_BT_GATT_ERR_INVALID_STATE          =    0x04,    /*!< Invalid state. */
	RTK_BT_GATT_ERR_INVALID_CODING         =    0x05,    /*!< Invalid coding. */
	RTK_BT_GATT_ERR_INVALID_HANDLE         =    0x06,    /*!< Invalid handle. */
	RTK_BT_GATT_ERR_INVALID_PARAM          =    0x07,    /*!< Invalid parameters. */
	RTK_BT_GATT_ERR_INTERNAL               =    0x08,    /*!< Internal error. */
	RTK_BT_GATT_ERR_NOT_ALLOWED            =    0x09,    /*!< Operation not allowed. */
	RTK_BT_GATT_ERR_NOTIF_IND_NOT_CFG      =    0x0A,    /*!< Notification or indication is not configured. */
	RTK_BT_GATT_ERR_NOTIF_IND_CFG          =    0x0B,    /*!< Notification or indication is configured. */
	RTK_BT_GATT_ERR_NOTIF_IND_CONF_PD      =    0x0C,    /*!< Indication confirmation is pending. */
	RTK_BT_GATT_ERR_TIMEOUT                =    0x0D,    /*!< Timeout. */
	RTK_BT_GATT_ERR_LINK_DEACTIVATED       =    0x0E,    /*!< Link deactivated. */
	RTK_BT_GATT_ERR_NOT_AUTHENTICATED      =    0x0F,    /*!< Link is not authenticated. */
	RTK_BT_GATT_ERR_NOT_ENCRYPTED          =    0x10,    /*!< Link is not encrypted. */
	RTK_BT_GATT_ERR_PROC_FAIL              =    0x11,    /*!< Procedure failed. */
};

/**
 * @defgroup  BT_APIs BT APIs
 * @brief     List all BT related APIs.
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_DEF_H__ */
