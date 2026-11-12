/**
 * @file      rtk_bt_sdp.h
 * @brief     Bluetooth BR/EDR SDP type and function definition.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_SDP_H__
#define __RTK_BT_SDP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define RTK_BT_SDK_MAX_RECORD_LENGTH    600                /*!< Maximum length of a single SDP service record, in bytes. */

/************************************** Data Types *************************************/

/* SDP data element headers. */
#define RTK_BT_SDP_UUID16_HDR                      0x19  /*!< Data element header of a 16-bit UUID (type 3, size index 1: 2 bytes). */
#define RTK_BT_SDP_UUID32_HDR                      0x1a  /*!< Data element header of a 32-bit UUID (type 3, size index 2: 4 bytes). */
#define RTK_BT_SDP_UUID128_HDR                     0x1c  /*!< Data element header of a 128-bit UUID (type 3, size index 4: 16 bytes). */
#define RTK_BT_SDP_DATA_ELEM_SEQ_HDR               0x35  /*!< Data element sequence header (type 6, size index 5: length in the next byte). */
#define RTK_BT_SDP_DATA_ELEM_SEQ_HDR_2BYTE         0x36  /*!< Data element sequence header (type 6, size index 6: length in the next two bytes). */
#define RTK_BT_SDP_STRING_HDR                      0x25  /*!< Text string header (type 4, size index 5: length in the next byte). */

#define RTK_BT_SDP_UNSIGNED_ONE_BYTE               0x08  /*!< Data element header of a 1-byte unsigned integer. */
#define RTK_BT_SDP_UNSIGNED_TWO_BYTE               0x09  /*!< Data element header of a 2-byte unsigned integer. */
#define RTK_BT_SDP_UNSIGNED_FOUR_BYTE              0x0A  /*!< Data element header of a 4-byte unsigned integer. */
#define RTK_BT_SDP_BOOL_ONE_BYTE                   0x28  /*!< Data element header of a 1-byte boolean. */

/* SDP attribute IDs. */
#define RTK_BT_SDP_ATTR_SRV_RECORD_HANDLE          0x0000  /*!< Service record handle. */
#define RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST          0x0001  /*!< Service Class ID List. */
#define RTK_BT_SDP_ATTR_SRV_RECORD_STATE           0x0002  /*!< Service record state. */
#define RTK_BT_SDP_ATTR_SRV_ID                     0x0003  /*!< Service ID. */
#define RTK_BT_SDP_ATTR_PROTO_DESC_LIST            0x0004  /*!< Protocol Descriptor List. */
#define RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST          0x0005  /*!< Browse Group List. */
#define RTK_BT_SDP_ATTR_LANG_BASE_ATTR_ID_LIST     0x0006  /*!< Language Base Attribute ID List. */
#define RTK_BT_SDP_ATTR_SRV_INFO_TIME_TO_LIVE      0x0007  /*!< Service info time to live, in seconds. */
#define RTK_BT_SDP_ATTR_SRV_AVAILABILITY           0x0008  /*!< Service availability, in percent. */
#define RTK_BT_SDP_ATTR_PROFILE_DESC_LIST          0x0009  /*!< Bluetooth profile descriptor list. */
#define RTK_BT_SDP_ATTR_DOC_URL                    0x000A  /*!< Documentation URL. */
#define RTK_BT_SDP_ATTR_CLIENT_EXEC_URL            0x000B  /*!< Client executable URL. */
#define RTK_BT_SDP_ATTR_ICON_URL                   0x000C  /*!< Icon URL. */
#define RTK_BT_SDP_ATTR_ADD_PROTO_DESC_LIST        0x000D  /*!< Additional Protocol Descriptor List. */

#define RTK_BT_SDP_ATTR_SRV_NAME                   0x0000  /*!< Base value of the service name attribute ID, added to a language base offset. */
#define RTK_BT_SDP_ATTR_SRV_DESC                   0x0001  /*!< Base value of the service description attribute ID, added to a language base offset. */
#define RTK_BT_SDP_ATTR_PROVIDER_NAME              0x0002  /*!< Base value of the provider name attribute ID, added to a language base offset. */

#define RTK_BT_SDP_ATTR_VER_NUMBER_LIST            0x0200  /*!< Version Number List of the Service Discovery Server record. */
#define RTK_BT_SDP_ATTR_GROUP_ID                   0x0200  /*!< Group ID of a group descriptor record. */
#define RTK_BT_SDP_ATTR_IP_SUBNET                  0x0200  /*!< IP subnet of a LAN access profile record. */
#define RTK_BT_SDP_ATTR_SRV_DB_STATE               0x0201  /*!< Service database state of the Service Discovery Server record. */

#define RTK_BT_SDP_BASE_LANG_OFFSET                0x0100  /*!< Offset to base language. */
#define RTK_BT_SDP_LANG_ENGLISH                    0x656e  /*!< Language code "en" (English), as defined by ISO 639:1988. */
#define RTK_BT_SDP_LANG_GERMAN                     0x6465  /*!< Language code "de" (German), as defined by ISO 639:1988. */
#define RTK_BT_SDP_CHARACTER_UTF8                  106     /*!< UTF8 encoding, cf. http://www.isi.edu/in-notes/iana/assignments/character-sets. */

#define RTK_BT_SDP_ATTR_L2C_PSM                    0x0200  /*!< L2CAP PSM of the OBEX over L2CAP transport. */

#define RTK_BT_SDP_ATTR_SRV_VER                    0x0300  /*!< Service version. */
#define RTK_BT_SDP_ATTR_EXT_NETWORK                0x0301  /*!< External network. */
#define RTK_BT_SDP_ATTR_FAX_CLASS1_SUPPORT         0x0302  /*!< Fax Class 1 support. */
#define RTK_BT_SDP_ATTR_RMT_AUDIO_VOL_CTRL         0x0302  /*!< Remote audio volume control. */
#define RTK_BT_SDP_ATTR_FAX_CLASS20_SUPPORT        0x0303  /*!< Fax Class 2.0 support. */
#define RTK_BT_SDP_ATTR_SUPPORTED_FORMATS_LIST     0x0303  /*!< Supported formats list. */
#define RTK_BT_SDP_ATTR_FAX_CLASS2_SUPPORT         0x0304  /*!< Fax Class 2 support. */
#define RTK_BT_SDP_ATTR_AUDIO_FEEDBACK_SUPPORT     0x0305  /*!< Audio feedback support. */
#define RTK_BT_SDP_ATTR_NETWORK_ADDR               0x0306  /*!< Network address. */
#define RTK_BT_SDP_ATTR_WAP_GATEWAY                0x0307  /*!< WAP gateway. */
#define RTK_BT_SDP_ATTR_HOME_PAGE_URL              0x0308  /*!< Home page URL. */
#define RTK_BT_SDP_ATTR_WAP_STACK_TYPE             0x0309  /*!< WAP stack type. */
#define RTK_BT_SDP_ATTR_SECURITY_DESC              0x030A  /*!< Security description. */
#define RTK_BT_SDP_ATTR_NET_ACCESS_TYPE            0x030B  /*!< Network access type. */
#define RTK_BT_SDP_ATTR_MAX_NET_ACCESS_RATE        0x030C  /*!< Maximum network access rate, in bits per second. */
#define RTK_BT_SDP_ATTR_SUPPORTED_CAPABILITIES     0x0310  /*!< Supported imaging capabilities. */
#define RTK_BT_SDP_ATTR_SUPPORTED_FEATURES         0x0311  /*!< Supported imaging features. */
#define RTK_BT_SDP_ATTR_SUPPORTED_FUNCTIONS        0x0312  /*!< Supported imaging functions. */
#define RTK_BT_SDP_ATTR_TOTAL_IMAGING_CAPACITY     0x0313  /*!< Total imaging data capacity, in bytes. */
#define RTK_BT_SDP_ATTR_SUPPORTED_REPOSITORIES     0x0314  /*!< Supported repositories. */

#define RTK_BT_SDP_ATTR_MAS_INSTANCE_ID            0x0315  /*!< Message Access Server instance ID. */
#define RTK_BT_SDP_ATTR_SUPPORTED_MSG_TYPES        0x0316  /*!< Supported message types. */
#define RTK_BT_SDP_ATTR_MAP_SUPPERTED_FEATS        0x0317  /*!< MAP supported features. */

#define RTK_BT_SDP_ATTR_HID_DEV_RELEASE_NUM        0x0200  /*!< HID device release number. */
#define RTK_BT_SDP_ATTR_HID_PARSER_VER             0x0201  /*!< HID parser version. */
#define RTK_BT_SDP_ATTR_HID_DEV_SUB_CLASS          0x0202  /*!< HID device subclass. */
#define RTK_BT_SDP_ATTR_HID_COUNTRY_CODE           0x0203  /*!< HID country code. */
#define RTK_BT_SDP_ATTR_HID_VIRTUAL_CABLE          0x0204  /*!< HID virtual cable support. */
#define RTK_BT_SDP_ATTR_HID_RECONN_INIT            0x0205  /*!< HID reconnect initiate. */
#define RTK_BT_SDP_ATTR_HID_DESC_LIST              0x0206  /*!< HID report descriptor list. */
#define RTK_BT_SDP_ATTR_HID_LANG_ID_BASE_LIST      0x0207  /*!< HID language ID base list. */
#define RTK_BT_SDP_ATTR_HID_SDP_DISABLE            0x0208  /*!< HID SDP disable. */
#define RTK_BT_SDP_ATTR_HID_BATTERY_POWER          0x0209  /*!< HID battery power. */
#define RTK_BT_SDP_ATTR_HID_REMOTE_WAKE            0x020A  /*!< HID remote wake. */
#define RTK_BT_SDP_ATTR_HID_PROFILE_VER            0x020B  /*!< HID profile version. */
#define RTK_BT_SDP_ATTR_HID_SUPERVISION_TIMEOUT    0x020C  /*!< HID supervision timeout. */
#define RTK_BT_SDP_ATTR_HID_NORMALLY_CONNECTABLE   0x020D  /*!< HID normally connectable. */
#define RTK_BT_SDP_ATTR_HID_BOOT_DEV               0x020E  /*!< HID boot device. */

#define RTK_BT_SDP_ATTR_PBAP_SUPPORTED_FEAT        0x0317  /*!< PBAP supported features. */

#define RTK_BT_SDP_ATTR_DIP_SPECIFICATION_ID       0x0200  /*!< Device ID specification ID. */
#define RTK_BT_SDP_ATTR_DIP_VENDOR_ID              0x0201  /*!< Vendor ID. */
#define RTK_BT_SDP_ATTR_DIP_PRODUCT_ID             0x0202  /*!< Product ID. */
#define RTK_BT_SDP_ATTR_DIP_PRODUCT_VERSION        0x0203  /*!< Product version. */
#define RTK_BT_SDP_ATTR_DIP_PRIMARY_RECORD         0x0204  /*!< Primary record flag. */
#define RTK_BT_SDP_ATTR_DIP_VENDOR_ID_SOURCE       0x0205  /*!< Vendor ID source. */

/**
 * @struct    rtk_sdp_record_t
 * @brief     Bluetooth BR/EDR SDP service record.
 */
typedef struct {
	struct list_head    list;                                    /*!< Linked-list node chaining the SDP records. */
	uint8_t             record[RTK_BT_SDK_MAX_RECORD_LENGTH];    /*!< Raw SDP record data. */
	uint32_t            length;                                  /*!< Length of the SDP record data in bytes. */
} rtk_sdp_record_t;

/**
 * @struct    rtk_sdp_did_attr_info
 * @brief     Bluetooth BR/EDR remote Device ID (DID) attributes.
 */
typedef struct {
	uint8_t             bd_addr[6];     /*!< Bluetooth address of the remote device. */
	uint16_t            specific_id;    /*!< Device ID specification version supported by the remote device. */
	uint16_t            vendor_id;      /*!< Vendor ID of the remote device. */
	uint16_t            vendor_src;     /*!< Source that assigned the vendor ID.
                                         *   1: Bluetooth SIG.
                                         *   2: USB Implementers Forum. */
	uint16_t            product_id;     /*!< Product ID assigned by the vendor. */
	uint16_t            version;        /*!< Product version assigned by the vendor. */
} rtk_sdp_did_attr_info;

/**
 * @struct    rtk_sdp_discov_cmpl
 * @brief     Bluetooth BR/EDR SDP discovery completion information.
 */
typedef struct {
	uint8_t             bd_addr[6];    /*!< Bluetooth address of the remote device. */
	uint16_t            cause;         /*!< Completion cause reported by the stack for the SDP discovery; 0x00 on success. */
} rtk_sdp_discov_cmpl;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_sdp_api BT SDP APIs
 * @brief     BT SDP related function APIs.
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Add a local SDP service record to the profile database.
 * @param[in] record: SDP record data.
 *                    Must not be NULL.
 * @param[in] length: Length of the SDP record in bytes.
 *                    Max: @ref RTK_BT_SDK_MAX_RECORD_LENGTH.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API shall be called before BT is enabled.
 */
uint16_t rtk_sdp_record_add(void *record, uint32_t length);

/**
 * @brief     Start SDP discovery on a remote device to read its Device ID attributes.
 *            The result is reported asynchronously via event @ref RTK_BT_SDP_EVT_DISCOV_CMPL.
 * @param[in] bd_addr: Bluetooth address of the remote device, 6 bytes.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_sdp_discov_start(uint8_t *bd_addr);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_SDP_H__ */
