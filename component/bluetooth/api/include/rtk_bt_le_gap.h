/**
 * @file      rtk_bt_le_gap.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth LE GAP part type and function definition.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_LE_GAP_H__
#define __RTK_BT_LE_GAP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <basic_types.h>
#include <bt_api_config.h>

/**
 * @typedef   rtk_bt_le_appearance_t
 * @brief     Bluetooth LE device appearance definition.
 */
typedef enum {
	RTK_BT_LE_GAP_APPEARANCE_UNKNOWN                             = 0x0000,    /*!< Unknown appearance. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_PHONE                       = 0x0040,    /*!< Generic Phone. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_COMPUTER                    = 0x0080,    /*!< Generic Computer. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_WATCH                       = 0x00C0,    /*!< Generic Watch. */
	RTK_BT_LE_GAP_APPEARANCE_SPORTS_WATCH                        = 0x00C1,    /*!< Watch: Sports Watch. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_CLOCK                       = 0x0100,    /*!< Generic Clock. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_DISPLAY                     = 0x0140,    /*!< Generic Display. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_REMOTE                      = 0x0180,    /*!< Generic Remote Control. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_EYEGLASSES                  = 0x01C0,    /*!< Generic Eye-glasses. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_TAG                         = 0x0200,    /*!< Generic Tag. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_KEYRING                     = 0x0240,    /*!< Generic Keyring. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_MEDIA_PLAYER                = 0x0280,    /*!< Generic Media Player. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_BARCODE_SCANNER             = 0x02C0,    /*!< Generic Barcode Scanner. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_THERMOMETER                 = 0x0300,    /*!< Generic Thermometer. */
	RTK_BT_LE_GAP_APPEARANCE_THERMOMETER_EAR                     = 0x0301,    /*!< Thermometer: Ear. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_HEART_RATE                  = 0x0340,    /*!< Generic Heart Rate Sensor. */
	RTK_BT_LE_GAP_APPEARANCE_HEART_RATE_BELT                     = 0x0341,    /*!< Heart Rate Sensor: Heart Rate Belt. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_BLOOD_PRESSURE              = 0x0380,    /*!< Generic Blood Pressure. */
	RTK_BT_LE_GAP_APPEARANCE_BLOOD_PRESSURE_ARM                  = 0x0381,    /*!< Blood Pressure: Arm. */
	RTK_BT_LE_GAP_APPEARANCE_BLOOD_PRESSURE_WRIST                = 0x0382,    /*!< Blood Pressure: Wrist. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_HID                         = 0x03C0,    /*!< Generic Human Interface Device. */
	RTK_BT_LE_GAP_APPEARANCE_HID_KEYBOARD                        = 0x03C1,    /*!< HID: Keyboard. */
	RTK_BT_LE_GAP_APPEARANCE_HID_MOUSE                           = 0x03C2,    /*!< HID: Mouse. */
	RTK_BT_LE_GAP_APPEARANCE_HID_JOYSTICK                        = 0x03C3,    /*!< HID: Joystick. */
	RTK_BT_LE_GAP_APPEARANCE_HID_GAMEPAD                         = 0x03C4,    /*!< HID: Gamepad. */
	RTK_BT_LE_GAP_APPEARANCE_HID_DIGITIZER_TABLET                = 0x03C5,    /*!< HID: Digitizer Tablet. */
	RTK_BT_LE_GAP_APPEARANCE_HID_CARD_READER                     = 0x03C6,    /*!< HID: Card Reader. */
	RTK_BT_LE_GAP_APPEARANCE_HID_DIGITAL_PEN                     = 0x03C7,    /*!< HID: Digital Pen. */
	RTK_BT_LE_GAP_APPEARANCE_HID_BARCODE_SCANNER                 = 0x03C8,    /*!< HID: Barcode Scanner. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_GLUCOSE                     = 0x0400,    /*!< Generic Glucose Meter. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_WALKING                     = 0x0440,    /*!< Generic Running Walking Sensor. */
	RTK_BT_LE_GAP_APPEARANCE_WALKING_IN_SHOE                     = 0x0441,    /*!< Running Walking Sensor: In-Shoe. */
	RTK_BT_LE_GAP_APPEARANCE_WALKING_ON_SHOE                     = 0x0442,    /*!< Running Walking Sensor: On-Shoe. */
	RTK_BT_LE_GAP_APPEARANCE_WALKING_ON_HIP                      = 0x0443,    /*!< Running Walking Sensor: On-Hip. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_CYCLING                     = 0x0480,    /*!< Generic Cycling. */
	RTK_BT_LE_GAP_APPEARANCE_CYCLING_COMPUTER                    = 0x0481,    /*!< Cycling: Cycling Computer. */
	RTK_BT_LE_GAP_APPEARANCE_CYCLING_SPEED                       = 0x0482,    /*!< Cycling: Speed Sensor. */
	RTK_BT_LE_GAP_APPEARANCE_CYCLING_CADENCE                     = 0x0483,    /*!< Cycling: Cadence Sensor. */
	RTK_BT_LE_GAP_APPEARANCE_CYCLING_POWER                       = 0x0484,    /*!< Cycling: Power Sensor. */
	RTK_BT_LE_GAP_APPEARANCE_CYCLING_SPEED_CADENCE               = 0x0485,    /*!< Cycling: Speed and Cadence Sensor. */
	RTK_BT_LE_GAP_APPEARANCE_EARBUD                              = 0x0941,    /*!< Earbud. */
	RTK_BT_LE_GAP_APPEARANCE_HEADSET                             = 0x0942,    /*!< Headset. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_PULSE_OXIMETER              = 0x0C40,    /*!< Generic Pulse Oximeter. */
	RTK_BT_LE_GAP_APPEARANCE_PULSE_OXIMETER_FINGERTIP            = 0x0C41,    /*!< Pulse Oximeter: Fingertip. */
	RTK_BT_LE_GAP_APPEARANCE_PULSE_OXIMETER_WRIST                = 0x0C42,    /*!< Pulse Oximeter: Wrist Worn. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_WEIGHT                      = 0x0C80,    /*!< Generic Weight Scale. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_PERSONAL_MOBILITY_DEVICE    = 0x0CC0,    /*!< Generic Personal Mobility Device. */
	RTK_BT_LE_GAP_APPEARANCE_POWERED_WHEELCHAIR                  = 0x0CC1,    /*!< Personal Mobility Device: Powered Wheelchair. */
	RTK_BT_LE_GAP_APPEARANCE_MOBILITY_SCOOTER                    = 0x0CC2,    /*!< Personal Mobility Device: Mobility Scooter. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_CONTINUOUS_GLUCOSE_MONITOR  = 0x0D00,    /*!< Generic Continuous Glucose Monitor. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_INSULIN_PUMP                = 0x0D40,    /*!< Generic Insulin Pump. */
	RTK_BT_LE_GAP_APPEARANCE_INSULIN_PUMP_DURABLE_PUMP           = 0x0D41,    /*!< Insulin Pump: Durable Pump. */
	RTK_BT_LE_GAP_APPEARANCE_INSULIN_PUMP_PATCH_PUMP             = 0x0D44,    /*!< Insulin Pump: Patch Pump. */
	RTK_BT_LE_GAP_APPEARANCE_INSULIN_PEN                         = 0x0D48,    /*!< Insulin Pen. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_MEDICATION_DELIVERY         = 0x0D80,    /*!< Generic Medication Delivery. */
	RTK_BT_LE_GAP_APPEARANCE_GENERIC_OUTDOOR_SPORTS              = 0x1440,    /*!< Generic Outdoor Sports Activity. */
	RTK_BT_LE_GAP_APPEARANCE_OUTDOOR_SPORTS_LOCATION             = 0x1441,    /*!< Outdoor Sports Activity: Location Display Device. */
	RTK_BT_LE_GAP_APPEARANCE_OUTDOOR_SPORTS_LOCATION_AND_NAV     = 0x1442,    /*!< Outdoor Sports Activity: Location and Navigation Display Device. */
	RTK_BT_LE_GAP_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD         = 0x1443,    /*!< Outdoor Sports Activity: Location Pod. */
	RTK_BT_LE_GAP_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD_AND_NAV = 0x1444,    /*!< Outdoor Sports Activity: Location and Navigation Pod. */
} rtk_bt_le_appearance_t;


/**
 * @typedef   rtk_bt_le_gap_ad_type_t
 * @brief     Bluetooth LE GAP advertising data type definition.
 */
typedef enum {
	RTK_BT_LE_GAP_ADTYPE_FLAGS                                   = 0x01,      /*!< The Flags data type contains one bit Boolean flags. */
	RTK_BT_LE_GAP_ADTYPE_16BIT_MORE                              = 0x02,      /*!< Service: More 16-bit UUIDs available. */
	RTK_BT_LE_GAP_ADTYPE_16BIT_COMPLETE                          = 0x03,      /*!< Service: Complete list of 16-bit UUIDs. */
	RTK_BT_LE_GAP_ADTYPE_32BIT_MORE                              = 0x04,      /*!< Service: More 32-bit UUIDs available. */
	RTK_BT_LE_GAP_ADTYPE_32BIT_COMPLETE                          = 0x05,      /*!< Service: Complete list of 32-bit UUIDs. */
	RTK_BT_LE_GAP_ADTYPE_128BIT_MORE                             = 0x06,      /*!< Service: More 128-bit UUIDs available. */
	RTK_BT_LE_GAP_ADTYPE_128BIT_COMPLETE                         = 0x07,      /*!< Service: Complete list of 128-bit UUIDs. */
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_SHORT                        = 0x08,      /*!< Shortened local name. */
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE                     = 0x09,      /*!< Complete local name. */
	RTK_BT_LE_GAP_ADTYPE_POWER_LEVEL                             = 0x0A,      /*!< TX Power Level: = 0xXX: -127 to +127 dBm. */
	RTK_BT_LE_GAP_ADTYPE_OOB_CLASS_OF_DEVICE                     = 0x0D,      /*!< Simple Pairing OOB Tag: Class of device (3 octets). */
	RTK_BT_LE_GAP_ADTYPE_OOB_SIMPLE_PAIRING_HASHC                = 0x0E,      /*!< Simple Pairing OOB Tag: Simple Pairing Hash C (16 octets). */
	RTK_BT_LE_GAP_ADTYPE_OOB_SIMPLE_PAIRING_RANDR                = 0x0F,      /*!< Simple Pairing OOB Tag: Simple Pairing Randomizer R (16 octets). */
	RTK_BT_LE_GAP_ADTYPE_SM_TK                                   = 0x10,      /*!< Security Manager TK Value. */
	RTK_BT_LE_GAP_ADTYPE_SM_OOB_FLAG                             = 0x11,      /*!< Security Manager OOB Flags. */
	RTK_BT_LE_GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE               = 0x12,      /*!< Min and Max values of the connection interval (2 octets Min, 2 octets Max) (0xFFFF
                                                                               *   indicates no conn interval min or max). */
	RTK_BT_LE_GAP_ADTYPE_SIGNED_DATA                             = 0x13,      /*!< Signed Data field. */
	RTK_BT_LE_GAP_ADTYPE_SERVICES_LIST_16BIT                     = 0x14,      /*!< Service Solicitation: list of 16-bit Service UUIDs. */
	RTK_BT_LE_GAP_ADTYPE_SERVICES_LIST_128BIT                    = 0x15,      /*!< Service Solicitation: list of 128-bit Service UUIDs. */
	RTK_BT_LE_GAP_ADTYPE_SERVICE_DATA                            = 0x16,      /*!< Service Data. */
	RTK_BT_LE_GAP_ADTYPE_PUBLIC_TGT_ADDR                         = 0x17,      /*!< Public Target Address. */
	RTK_BT_LE_GAP_ADTYPE_RANDOM_TGT_ADDR                         = 0x18,      /*!< Random Target Address. */
	RTK_BT_LE_GAP_ADTYPE_APPEARANCE                              = 0x19,      /*!< Appearance. */
	RTK_BT_LE_GAP_ADTYPE_ADV_INTERVAL                            = 0x1A,      /*!< Advertising Interval. */
	RTK_BT_LE_GAP_ADTYPE_LE_BT_ADDR                              = 0x1B,      /*!< LE Bluetooth Device Address. */
	RTK_BT_LE_GAP_ADTYPE_LE_ROLE                                 = 0x1C,      /*!< LE Role. */
	RTK_BT_LE_GAP_ADTYPE_SP_HASH_C256                            = 0x1D,      /*!< Simple Pairing Hash C-256. */
	RTK_BT_LE_GAP_ADTYPE_SP_RAND_R256                            = 0x1E,      /*!< Simple Pairing Randomizer R-256. */
	RTK_BT_LE_GAP_ADTYPE_LIST_32BIT_SILI                         = 0x1F,      /*!< List of 32-bit Service Solicitation UUIDs. */
	RTK_BT_LE_GAP_ADTYPE_SERVICE_DATA_32BIT                      = 0x20,      /*!< Service Data - 32-bit UUID. */
	RTK_BT_LE_GAP_ADTYPE_SERVICE_DATA_128BIT                     = 0x21,      /*!< Service Data - 128-bit UUID. */
	RTK_BT_LE_GAP_ADTYPE_SC_CONF_VALUE                           = 0x22,      /*!< LE Secure Connections Confirmation Value. */
	RTK_BT_LE_GAP_ADTYPE_SC_RAND_VALUE                           = 0x23,      /*!< LE Secure Connections Random Value. */
	RTK_BT_LE_GAP_ADTYPE_URI                                     = 0x24,      /*!< URI. */
	RTK_BT_LE_GAP_ADTYPE_INDOOR_POSITION                         = 0x25,      /*!< Indoor Positioning. */
	RTK_BT_LE_GAP_ADTYPE_TRANSPORT_DISCOVERY_DATA                = 0x26,      /*!< Transport Discovery Data. */
	RTK_BT_LE_GAP_ADTYPE_LE_SUPPORTED_FEATURES                   = 0x27,      /*!< LE Supported Features. */
	RTK_BT_LE_GAP_ADTYPE_CHAN_MAP_UPDATE_IND                     = 0x28,      /*!< Channel Map Update Indication. */
	RTK_BT_LE_GAP_ADTYPE_MESH_PB_ADV                             = 0x29,      /*!< Mesh Pb-Adv. */
	RTK_BT_LE_GAP_ADTYPE_MESH_PACKET                             = 0x2A,      /*!< Mesh Packet. */
	RTK_BT_LE_GAP_ADTYPE_MESH_BEACON                             = 0x2B,      /*!< Mesh Beacon. */
	RTK_BT_LE_GAP_ADTYPE_RSI                                     = 0x2E,      /*!< RSI. */
	RTK_BT_LE_GAP_ADTYPE_BROADCAST_NAME                          = 0x30,      /*!< Broadcast Name. */
	RTK_BT_LE_GAP_ADTYPE_3D_INFO_DATA                            = 0x3D,      /*!< 3D Information Data. */
	RTK_BT_LE_GAP_ADTYPE_MANUFACTURER_SPECIFIC                   = 0xFF,      /*!< Manufacturer Specific Data. */
} rtk_bt_le_gap_ad_type_t;

/**
 * @typedef   rtk_bt_le_gap_ad_flags_data_t
 * @brief     Bluetooth LE GAP advertising data type data definition.
 */
typedef enum {
	RTK_BT_LE_GAP_ADTYPE_FLAGS_LIMITED                           = 0x01,      /*!< Discovery Mode: LE Limited Discoverable Mode. */
	RTK_BT_LE_GAP_ADTYPE_FLAGS_GENERAL                           = 0x02,      /*!< Discovery Mode: LE General Discoverable Mode. */
	RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED               = 0x04,      /*!< Discovery Mode: BR/EDR Not Supported. */
	RTK_BT_LE_GAP_ADTYPE_FLAGS_SIMULTANEOUS_LE_BREDR_CONTROLLER  = 0x08,      /*!< Discovery Mode: Simultaneous LE and BR/EDR Controller Supported. */
	RTK_BT_LE_GAP_ADTYPE_FLAGS_SIMULTANEOUS_LE_BREDR_HOST        = 0x10,      /*!< Discovery Mode: Simultaneous LE and BR/EDR Host Supported. */
} rtk_bt_le_gap_ad_flags_data_t;

/**
 * @typedef   rtk_bt_le_adv_type_t
 * @brief     Bluetooth LE GAP adv type definition.
 */
typedef enum {
	RTK_BT_LE_ADV_TYPE_IND                     = 0x00,                        /*!< Connectable and scannable undirected advertising (ADV_IND). */
	RTK_BT_LE_ADV_TYPE_DIRECT_IND_HIGH         = 0x01,                        /*!< Connectable high duty cycle directed advertising (ADV_DIRECT_IND, high duty cycle). */
	RTK_BT_LE_ADV_TYPE_SCAN_IND                = 0x02,                        /*!< Scannable undirected advertising (ADV_SCAN_IND). */
	RTK_BT_LE_ADV_TYPE_NONCONN_IND             = 0x03,                        /*!< Non connectable undirected advertising (ADV_NONCONN_IND). */
	RTK_BT_LE_ADV_TYPE_DIRECT_IND_LOW          = 0x04,                        /*!< Connectable low duty cycle directed advertising (ADV_DIRECT_IND, low duty cycle). */
	RTK_BT_LE_ADV_TYPE_RESERVED,                                              /*!< Reserved. */
} rtk_bt_le_adv_type_t;

/**
 * @typedef   rtk_bt_le_version_info_t
 * @brief     Bluetooth LE GAP stack version information.
 */
typedef struct {
	uint16_t hci_revision;                   /*!< HCI revision number reported by the controller. */
	uint16_t lmp_subversion;                 /*!< LMP sub-version number reported by the controller. */
	uint16_t btgap_revision;                 /*!< Btgap stack version. */
	uint16_t btgap_buildnum;                 /*!< Btgap stack build number. */
} rtk_bt_le_version_info_t;

/**
 * @typedef   rtk_bt_le_get_conn_id_t
 * @brief     Bluetooth LE GAP get connection id from connection handle.
 */
typedef struct {
	uint16_t conn_handle;                   /*!< Connection handle of the ACL connection to get corresponding conn id.
                                             *   Range: 0x0000 to 0x0EFF. */
	uint8_t *p_conn_id;                     /*!< Output: receives the connection ID corresponding to conn_handle.
                                             *   Must not be NULL. */
} rtk_bt_le_get_conn_id_t;

/**
 * @typedef   rtk_bt_le_phy_type_t
 * @brief     Bluetooth LE PHY type.
 */
typedef enum {
	RTK_BT_LE_PHYS_1M       = 0x01,         /*!< LE PHY 1M used. */
	RTK_BT_LE_PHYS_2M       = 0x02,         /*!< LE PHY 2M used. */
	RTK_BT_LE_PHYS_CODED    = 0x03          /*!< LE Coded PHY used. */
} rtk_bt_le_phy_type_t;

/**
 * @typedef   rtk_bt_le_phy_prim_adv_t
 * @brief     Bluetooth LE Primary Adv PHY.
 */
typedef enum {
	RTK_BT_LE_PHYS_PRIM_ADV_1M      = 1,    /*!< Primary advertising PHY is LE 1M, 1 Msym/s uncoded. */
	RTK_BT_LE_PHYS_PRIM_ADV_CODED   = 3     /*!< Primary advertising PHY is LE Coded, used for coded advertising on the primary advertising channel. */
} rtk_bt_le_phy_prim_adv_t;

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
/**
 * @typedef   rtk_bt_le_adv_event_prop_t
 * @brief     Bluetooth LE GAP adv event properties.
 */
typedef enum {
	RTK_BT_LE_EXT_ADV_EXTENDED_ADV_NON_SCAN_NON_CONN_UNDIRECTED = 0x00,    /*!< Extended PDU: non-connectable, non-scannable, undirected; Advertising data may be carried
                                                                            *   in AUX_ADV_IND and AUX_CHAIN_IND PDUs. */
	RTK_BT_LE_EXT_ADV_EXTENDED_ADV_NON_SCAN_NON_CONN_DIRECTED   = 0x04,    /*!< Extended PDU: non-connectable, non-scannable, directed to a specific peer. */
	RTK_BT_LE_EXT_ADV_EXTENDED_ADV_CONN_UNDIRECTED              = 0x01,    /*!< Extended PDU: connectable, non-scannable, undirected; advertising data shall not exceed
                                                                            *   245 bytes. */
	RTK_BT_LE_EXT_ADV_EXTENDED_ADV_CONN_DIRECTED                = 0x05,    /*!< Extended PDU: connectable, non-scannable, directed; advertising data shall not exceed 239
                                                                            *   bytes. */
	RTK_BT_LE_EXT_ADV_EXTENDED_ADV_SCAN_UNDIRECTED              = 0x02,    /*!< Extended PDU: non-connectable, scannable, undirected; Scannable extended advertising may
                                                                            *   be followed by AUX_SCAN_RSP. */
	RTK_BT_LE_EXT_ADV_EXTENDED_ADV_SCAN_DIRECTED                = 0x06,    /*!< Extended PDU: non-connectable, scannable, directed; Scannable extended advertising may be
                                                                            *   followed by AUX_SCAN_RSP. */
	RTK_BT_LE_EXT_ADV_LEGACY_ADV_CONN_SCAN_UNDIRECTED           = 0x13,    /*!< Legacy PDU (ADV_IND): connectable, scannable, undirected; advertising data and scan
                                                                            *   response each limited to 31 bytes. */
	RTK_BT_LE_EXT_ADV_LEGACY_ADV_CONN_LOW_DUTY_DIRECTED         = 0x15,    /*!< Legacy PDU (ADV_DIRECT_IND low duty cycle): connectable directed at low duty cycle;
                                                                            *   no advertising data payload. */
	RTK_BT_LE_EXT_ADV_LEGACY_ADV_CONN_HIGH_DUTY_DIRECTED        = 0x1D,    /*!< Legacy PDU (ADV_DIRECT_IND high duty cycle): connectable directed at high duty cycle;
                                                                            *   no advertising data payload. */
	RTK_BT_LE_EXT_ADV_LEGACY_ADV_SCAN_UNDIRECTED                = 0x12,    /*!< Legacy PDU (ADV_SCAN_IND): non-connectable, scannable, undirected; advertising and scan
                                                                            *   response each limited to 31 bytes. */
	RTK_BT_LE_EXT_ADV_LEGACY_ADV_NON_SCAN_NON_CONN_UNDIRECTED   = 0x10,    /*!< Legacy PDU (ADV_NONCONN_IND): non-connectable, non-scannable, undirected; advertising data
                                                                            *   limited to 31 bytes. */
} rtk_bt_le_adv_event_prop_t;
#endif

/**
 * @typedef   rtk_bt_le_adv_ch_map_t
 * @brief     Bluetooth LE GAP adv channel map definition.
 */
typedef enum {
	RTK_BT_LE_ADV_CHNL_37               = 0x01,                            /*!< Channel 37 shall be used. */
	RTK_BT_LE_ADV_CHNL_38               = 0x02,                            /*!< Channel 38 shall be used. */
	RTK_BT_LE_ADV_CHNL_39               = 0x04,                            /*!< Channel 39 shall be used. */
	RTK_BT_LE_ADV_CHNL_ALL              = 0x07,                            /*!< Channel 37,38,39 shall be used. */
	RTK_BT_LE_ADV_CHNL_RESERVED,                                           /*!< Reserved. */
} rtk_bt_le_adv_ch_map_t;

/**
 * @typedef   rtk_bt_le_adv_filter_t
 * @brief     Bluetooth LE GAP adv filter type definition.
 */
typedef enum {
	RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY              = 0,          /*!< Allow both scan and connection requests from anyone. */
	RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY             = 1,          /*!< Allow both scan requests from White List devices only and connection requests from anyone. */
	RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST             = 2,          /*!< Allow both scan requests from anyone and connection requests from White List devices only. */
	RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST            = 3,          /*!< Allow scan and connection requests from White List devices only. */
	RTK_BT_LE_ADV_FILTER_RESERVED,                                         /*!< All bigger values are reserved. */
} rtk_bt_le_adv_filter_t;

/**
 * @struct    rtk_bt_le_preferred_conn_param_t
 * @brief     Bluetooth LE preferred connection parameters definition.
 */
typedef struct {
	uint16_t conn_interval_min;                      /*!< Minimum preferred connection interval.
                                                      *   Range: 0x0006 to 0x0C80.
                                                      *   Time = N * 1.25 ms.
                                                      *   Time Range: 7.5 ms to 4 s.
                                                      *   0xFFFF = no specific minimum preference.
                                                      *   Values outside [0x0006, 0x0C80] except 0xFFFF are reserved. */
	uint16_t conn_interval_max;                      /*!< Maximum preferred connection interval.
                                                      *   Range: 0x0006 to 0x0C80.
                                                      *   Time = N * 1.25 ms.
                                                      *   Time Range: 7.5 ms to 4 s.
                                                      *   Must be >= conn_interval_min.
                                                      *   0xFFFF = no specific maximum preference.
                                                      *   Values outside [0x0006, 0x0C80] except 0xFFFF are reserved. */
	uint16_t slave_latency;                          /*!< Preferred peripheral latency: number of consecutive connection events the peripheral may skip without losing the connection.
                                                      *   Range: 0x0000 to 0x01F3 (0 = no latency).
                                                      *   0xFFFF = no specific value requested.
                                                      *   Values outside [0x0000, 0x01F3] except 0xFFFF are reserved. */
	uint16_t supv_timeout;                           /*!< Preferred supervision timeout.
                                                      *   Range: 0x000A to 0x0C80.
                                                      *   Time = N * 10 ms.
                                                      *   Time Range: 100 ms to 32 s.
                                                      *   Must be > (1 + slave_latency) * conn_interval_max * 2.
                                                      *   0xFFFF = no specific value requested.
                                                      *   Values outside [0x000A, 0x0C80] except 0xFFFF are reserved. */
} rtk_bt_le_preferred_conn_param_t;

/**
 * @typedef   rtk_bt_le_addr_type_t
 * @brief     Bluetooth device address type definition.
 */
typedef enum {
	RTK_BT_LE_ADDR_TYPE_PUBLIC =            0x00,    /*!< Public Device Address. */
	RTK_BT_LE_ADDR_TYPE_RANDOM =            0x01,    /*!< Random Device Address. */
	RTK_BT_LE_ADDR_TYPE_RPA_PUBLIC =        0x02,    /*!< Public Identity Address (RPA). */
	RTK_BT_LE_ADDR_TYPE_RPA_RANDOM =        0x03,    /*!< Random (static) Identity Address (RPA). */

	RTK_BT_LE_ADDR_TYPE_PEER_ANONYMOUS =    0xFF,    /*!< Anonymous Device Address. */
} rtk_bt_le_addr_type_t;

/**
 * @enum      rtk_bt_le_ident_addr_type_t
 * @brief     Bluetooth LE identity address type definition.
 */
typedef enum {
	RTK_BT_LE_IDENT_ADDR_PUBLIC      = 0x00,         /*!< Public identity address. */
	RTK_BT_LE_IDENT_ADDR_RAND        = 0x01,         /*!< Random identity address. */
} rtk_bt_le_ident_addr_type_t;

/**
 * @typedef   rtk_bt_le_rand_addr_type_t
 * @brief     Bluetooth device address type definition.
 */
typedef enum {
	RTK_BT_LE_RAND_ADDR_STATIC =            0x00,    /*!< Static random device address. */
	RTK_BT_LE_RAND_ADDR_NON_RESOLVABLE =    0x01,    /*!< Non resolvable random device address. */
	RTK_BT_LE_RAND_ADDR_RESOLVABLE =        0x02,    /*!< Resolvable random device address. */
} rtk_bt_le_rand_addr_type_t;

/**
 * @def       RTK_BD_ADDR_LEN
 * @brief     Bluetooth device address length macro.
 */
#define RTK_BD_ADDR_LEN             6

/**
 * @struct    rtk_bt_le_addr_t
 * @brief     Bluetooth LE device address definition.
 */
typedef struct {
	rtk_bt_le_addr_type_t type;                      /*!< Address type. */
	uint8_t addr_val[RTK_BD_ADDR_LEN];               /*!< Address. */
} rtk_bt_le_addr_t;

/**
 * @struct    rtk_bt_le_ident_addr_t
 * @brief     Bluetooth LE identity address definition.
 */
typedef struct {
	rtk_bt_le_ident_addr_type_t type;               /*!< Identity address type. */
	uint8_t addr_val[RTK_BD_ADDR_LEN];              /*!< Identity address value. */
} rtk_bt_le_ident_addr_t;

/**
 * @struct    rtk_bt_le_set_rand_addr_t
 * @brief     Bluetooth LE set random address parameter definition.
 */
typedef struct {
	bool auto_generate;                   /*!< Whether to auto-generate a random address of the specified type.
                                           *   - true: the stack generates a random address of @ref type and returns it via @ref addr_val.
                                           *   - false: the provided @ref addr_val is programmed directly; @ref type is ignored. */
	rtk_bt_le_rand_addr_type_t type;      /*!< Random address type. */
	uint8_t *addr_val;                    /*!< Random address. */
} rtk_bt_le_set_rand_addr_t;

/**
 * @typedef   rtk_bt_le_scan_type_t
 * @brief     Bluetooth LE GAP scan type definition.
 */
typedef enum {
	RTK_BT_LE_SCAN_TYPE_PASSIVE = 0,                               /*!< Passive scan. */
	RTK_BT_LE_SCAN_TYPE_ACTIVE,                                    /*!< Active scan. */
	RTK_BT_LE_SCAN_TYPE_RESERVED,                                  /*!< All bigger values are reserved. */
} rtk_bt_le_scan_type_t;

/**
 * @typedef   rtk_bt_le_conn_filter_t
 * @brief     Bluetooth LE GAP connection filter type definition.
 */
typedef enum {
	RTK_BT_LE_CONN_FILTER_WITHOUT_WHITELIST,                       /*!< 0, filter without whitelist. */
	RTK_BT_LE_CONN_FILTER_WITH_WHITELIST,                          /*!< 1, filter with whitelist. */
	RTK_BT_LE_CONN_FILTER_RESERVED,                                /*!< All bigger values are reserved. */
} rtk_bt_le_conn_filter_t;

/**
 * @typedef   rtk_bt_le_scan_filter_t
 * @brief     Bluetooth LE GAP scan filter type definition.
 */
typedef enum {
	RTK_BT_LE_SCAN_FILTER_ALLOW_ALL             = 0x0,             /*!< Accept all:
                                                                    *   1. advertisement packets except directed advertising packets not addressed to this device (default). */
	RTK_BT_LE_SCAN_FILTER_ALLOW_ONLY_WLST       = 0x1,             /*!< Accept only :
                                                                    *   1. advertisement packets from devices where the advertiser's address is in the White list.
                                                                    *   2. Directed advertising packets which are not addressed for this device shall be ignored. */
	RTK_BT_LE_SCAN_FILTER_ALLOW_UND_RPA_DIR     = 0x2,             /*!< Accept all :
                                                                    *   1. undirected advertisement packets, and
                                                                    *   2. directed advertising packets where the initiator address is a resolvable private address, and
                                                                    *   3. directed advertising packets addressed to this device. */
	RTK_BT_LE_SCAN_FILTER_ALLOW_WLIST_RPA_DIR   = 0x3,             /*!< Accept all :
                                                                    *   1. advertisement packets from devices where the advertiser's address is in the White list, and
                                                                    *   2. directed advertising packets where the initiator address is a resolvable private address, and
                                                                    *   3. directed advertising packets addressed to this device. */
} rtk_bt_le_scan_filter_t;


/**
 * @typedef   rtk_bt_le_scan_duplicate_t
 * @brief     Bluetooth LE GAP scan duplicate filter type definition.
 */
typedef enum {
	RTK_BT_LE_SCAN_DUPLICATE_DISABLE = 0,                          /*!< Link Layer should generate advertising reports to the host for each packet received. */
	RTK_BT_LE_SCAN_DUPLICATE_ENABLE  = 1,                          /*!< Link Layer should filter out duplicate advertising reports to the Host. */
	RTK_BT_LE_SCAN_DUPLICATE_ENABLED_RESET_FOR_EACH_PERIOD = 2,    /*!< Duplicate filtering enabled, reset for each scan period. */
	RTK_BT_LE_SCAN_DUPLICATE_RESERVED,                             /*!< Reserved. */
} rtk_bt_le_scan_duplicate_t;

/**
 * @typedef   rtk_bt_le_link_role_t
 * @brief     Bluetooth LE GAP link role definition.
 */
typedef enum {
	RTK_BT_LE_ROLE_MASTER           = 0x00,              /*!< Means central. */
	RTK_BT_LE_ROLE_SLAVE            = 0x01,              /*!< Means peripheral. */
	RTK_BT_LE_ROLE_UNKNOWN,                              /*!< Unknown. */
} rtk_bt_le_link_role_t;

/**
 * @typedef   rtk_bt_le_adv_stop_reason_t
 * @brief     Bluetooth LE ADV type Definition.
 */
typedef enum {
	RTK_BT_LE_ADV_STOP_BY_HOST      = 0,                /*!< Stopped by host. */
	RTK_BT_LE_ADV_STOP_BY_CONN      = 1,                /*!< Stopped due to connection established. */
	RTK_BT_LE_ADV_STOP_BY_DURATION  = 2,                /*!< Stopped due to duration expired or number of extended adv events exceeded. */
	RTK_BT_LE_ADV_STOP_UNKNOWN,                         /*!< Unknown. */
} rtk_bt_le_adv_stop_reason_t;

/**
 * @struct    rtk_bt_le_adv_param_t
 * @brief     Bluetooth LE GAP set adv data parameter definition.
 */
typedef struct {
	uint16_t interval_min;                             /*!< Minimum advertising interval for undirected and low duty cycle directed advertising.
                                                        *   Range: 0x0020 to 0x4000.
                                                        *   Default: 0x0800 (1.28 s)
                                                        *   Time = N * 0.625 ms.
                                                        *   Time Range: 20 ms to 10.24 s. */
	uint16_t interval_max;                             /*!< Maximum advertising interval for undirected and low duty cycle directed advertising.
                                                        *   Range: 0x0020 to 0x4000.
                                                        *   Default: 0x0800 (1.28 s)
                                                        *   Time = N * 0.625 ms.
                                                        *   Time Range: 20 ms to 10.24 s. */
	rtk_bt_le_adv_type_t type;                         /*!< Advertising Type. */
	rtk_bt_le_addr_type_t
	own_addr_type;               /*!< Own Address Type: specifies the Bluetooth device address type used by the Controller during advertising. */
	rtk_bt_le_addr_t peer_addr;                        /*!< Peer Address. */
	rtk_bt_le_adv_ch_map_t channel_map;                /*!< Advertising Channel Map. */
	rtk_bt_le_adv_filter_t filter_policy;              /*!< Advertising Filter Policy. */
} rtk_bt_le_adv_param_t;

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
/**
 * @struct    rtk_bt_le_ext_adv_param_t
 * @brief     Bluetooth LE GAP ext adv parameter definition.
 */
typedef struct {
	rtk_bt_le_adv_event_prop_t adv_event_prop;         /*!< Advertising Event Properties. */
	uint32_t primary_adv_interval_min;                 /*!< Minimum advertising interval for undirected and low duty cycle directed advertising.
                                                        *   Range: 0x000020 to 0xFFFFFF.
                                                        *   Time = N * 0.625 ms.
                                                        *   Time Range: 20 ms to 10,485.759375 s. */
	uint32_t primary_adv_interval_max;                 /*!< Maximum advertising interval for undirected and low duty cycle directed advertising.
                                                        *   Range: 0x000020 to 0xFFFFFF.
                                                        *   Time = N * 0.625 ms.
                                                        *   Time Range: 20 ms to 10,485.759375 s. */
	rtk_bt_le_adv_ch_map_t primary_adv_channel_map;    /*!< Advertising Primary Channel Map. */
	rtk_bt_le_addr_t own_addr;                         /*!< Own Address Type and Address. */
	rtk_bt_le_addr_t peer_addr;                        /*!< Peer Address Type and Address. */
	rtk_bt_le_adv_filter_t filter_policy;              /*!< Advertising Filter Policy. */
	int8_t tx_power;                                   /*!< Advertising TX Power: indicates max Tx power.
                                                        *   Range: -127 to +20 dBm.
                                                        *   0x7F: Host has no preference. */
	rtk_bt_le_phy_prim_adv_t primary_adv_phy;          /*!< Primary Advertising PHY. */
	uint8_t secondary_adv_max_skip;                    /*!< Secondary Advertising Max Skip. */
	rtk_bt_le_phy_type_t secondary_adv_phy;            /*!< Secondary Advertising PHY. */
	uint8_t adv_sid;                                   /*!< Value of the Advertising SID subfield in the ADI field of the PDU.
                                                        *   Range: 0x00 to 0x0F. */
} rtk_bt_le_ext_adv_param_t;

/**
 * @struct    rtk_bt_le_ext_adv_create_t
 * @brief     Bluetooth LE GAP create ext adv definition.
 */
typedef struct {
	rtk_bt_le_ext_adv_param_t *p_adv_param;            /*!< Advertising parameter. */
	uint8_t *p_adv_handle;                             /*!< Advertising set handle created by stack. */
} rtk_bt_le_ext_adv_create_t;

/**
 * @struct    rtk_bt_le_ext_adv_start_t
 * @brief     Bluetooth LE GAP start ext adv definition.
 */
typedef struct {
	uint8_t adv_handle;                        /*!< Identify an advertising set, which is assigned by @ref rtk_bt_le_gap_create_ext_adv. */
	uint16_t duration;                         /*!< Maximum advertising duration.
                                                *   0 = advertise until user stopped.
                                                *   Range: 0x0000 to 0xFFFF.
                                                *   Time = N * 10 ms.
                                                *   Time Range: 10 ms to 655,350 ms. */
	uint8_t num_events;                        /*!< Range:
                                                *   non-zero: indicates the maximum extended advertising events that shall attempt to send prior to terminating the extended advertising
                                                *   even if the duration parameter has not expired.
                                                *   0x00: No maximum number of advertising events. */
} rtk_bt_le_ext_adv_start_t;

/**
 * @typedef   rtk_bt_le_ext_adv_data_t
 * @brief     Bluetooth LE GAP set ext adv data definition.
 */
typedef struct {
	uint8_t adv_handle;                        /*!< Identify an advertising set, which is assigned by @ref rtk_bt_le_gap_create_ext_adv. */
	uint8_t *pdata;                            /*!< Advertising Data. */
	uint16_t len;                              /*!< Advertising Data Len. */
} rtk_bt_le_ext_adv_data_t;

/**
 * @struct    rtk_bt_le_ext_create_conn_param_t
 * @brief     Bluetooth LE GAP create ext connection parameter definition.
 */
typedef struct {
	rtk_bt_le_conn_filter_t filter_policy;    /*!< Initiator Filter Policy. */
	rtk_bt_le_addr_type_t own_addr_type;      /*!< Own Address Type. */
	rtk_bt_le_addr_t peer_addr;               /*!< Address of the peer device to connect to; ignored when filter_policy uses the Filter Accept List. */
	bool init_phys[3];                        /*!< Initiating_PHYs.
                                               *   init_phys[0]: 1M PHY.
                                               *   init_phys[1]: 2M PHY.
                                               *   init_phys[2]: Coded PHY.
                                               *   - true: initiation enabled for that PHY.
                                               *   - false: initiation disabled for that PHY. */
	uint16_t scan_interval[3];                /*!< Time interval from when the Controller started its last scan until it begins the subsequent scan on the primary
                                               *   advertising channel.
                                               *   Range: 0x0004 to 0xFFFF.
                                               *   Time = N * 0.625 ms.
                                               *   Time Range: 2.5 ms to 40.959375 s. */
	uint16_t scan_window[3];                  /*!< Duration of the scan on the primary advertising channel. Shall be less than or equal to scan_interval[i].
                                               *   Range: 0x0004 to 0xFFFF.
                                               *   Time = N * 0.625 ms.
                                               *   Time Range: 2.5 ms to 40.959375 s. */
	uint16_t conn_interval_min[3];            /*!< Minimum value for the connection interval. This shall be less than or equal to conn_interval_max[i].
                                               *   Range: 0x0006 to 0x0C80.
                                               *   Time = N * 1.25 ms.
                                               *   Time Range: 7.5 ms to 4 s. */
	uint16_t conn_interval_max[3];            /*!< Maximum value for the connection interval. This shall be greater than or equal to conn_interval_min[i].
                                               *   Range: 0x0006 to 0x0C80.
                                               *   Time = N * 1.25 ms.
                                               *   Time Range: 7.5 ms to 4 s. */
	uint16_t conn_latency[3];                 /*!< Peripheral latency for the connection in number of connection events.
                                               *   Range: 0x0000 to 0x01F3.
                                               *   conn_latency[0]: 1M PHY.
                                               *   conn_latency[1]: 2M PHY.
                                               *   conn_latency[2]: Coded PHY. */
	uint16_t supv_timeout[3];                 /*!< Supervision timeout for the LE connection.
                                               *   Range: 0x000A to 0x0C80.
                                               *   Time = N * 10 ms.
                                               *   Time Range: 100 ms to 32 s.
                                               *   supv_timeout[0]: 1M PHY.
                                               *   supv_timeout[1]: 2M PHY.
                                               *   supv_timeout[2]: Coded PHY. */
	uint16_t scan_timeout;                    /*!< Create connection scan timeout value.
                                               *   If time has expired before connection has been established, stack will auto cancel connection.
                                               *   Time = N * 10 ms.
                                               *   0: stack will not cancel create connection forever. */
} rtk_bt_le_ext_create_conn_param_t;
#endif

#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
/**
 * @typedef   rtk_bt_le_pa_prop_t
 * @brief     Periodic Advertising Properties flag. Use the combination of enum definitions to indicate which fields should be included in the advertising packet.
 */
typedef enum {
	RTK_BT_LE_PA_ADV_PROP_INCLUDE_TX_POWER = 0x0040    /*!< Include TxPower in the periodic header of the advertising PDU. */
} rtk_bt_le_pa_prop_t;

/**
 * @struct    rtk_bt_le_pa_param_t
 * @brief     Bluetooth LE GAP set periodic adv parameter definition.
 */
typedef struct {
	uint8_t adv_handle;               /*!< Identify an advertising set, which is assigned by @ref rtk_bt_le_gap_create_ext_adv. */
	uint16_t pa_interval_min;         /*!< Minimum advertising interval for periodic advertising.
                                       *   Should not be the same value as pa_interval_max.
                                       *   Range: 0x0006 to 0xFFFF.
                                       *   Time = N * 1.25 ms.
                                       *   Time Range: 7.5 ms to 81.91875 s. */
	uint16_t pa_interval_max;         /*!< Maximum advertising interval for periodic advertising.
                                       *   Should not be the same value as pa_interval_min.
                                       *   Range: 0x0006 to 0xFFFF.
                                       *   Time = N * 1.25 ms.
                                       *   Time Range: 7.5 ms to 81.91875 s. */
	uint16_t pa_prop;                 /*!< Periodic_Advertising_Properties, @ref rtk_bt_le_pa_prop_t. */
	uint8_t *pa_data;                 /*!< Advertising Data. */
	uint16_t pa_len;                  /*!< Advertising Data Len. */
#if defined(RTK_BLE_5_4_PA_RSP_SUPPORT) && RTK_BLE_5_4_PA_RSP_SUPPORT
	uint8_t num_subevents;            /*!< Number of subevents.
                                       *   If zero, the periodic advertiser will be a broadcaster, without responses. */
	uint8_t subevent_interval;        /*!< Interval between subevents (N * 1.25 ms), shall be between 7.5ms and 318.75ms. */
	uint8_t response_slot_delay;      /*!< Time between the advertising packet in a subevent and the first response slot (N * 1.25 ms). */
	uint8_t response_slot_spacing;    /*!< Time between response slots (N * 0.125 ms), shall be between 0.25 and 31.875ms. */
	uint8_t num_response_slots;       /*!< Number of subevent response slots.
                                       *   If zero, response_slot_delay and response_slot_spacing are ignored. */
#endif
} rtk_bt_le_pa_param_t;

/**
 * @struct    rtk_bt_le_pa_update_t
 * @brief     Bluetooth LE GAP update periodic adv data definition.
 */
typedef struct {
	uint8_t adv_handle;               /*!< Identify an advertising set, which is assigned by @ref rtk_bt_le_gap_create_ext_adv. */
	bool update_did_only;             /*!< Whether to update only the periodic advertising DID.
                                       *   - true: Update only the periodic advertising DID and keep the periodic advertising data unchanged.
                                       *         No new periodic advertising data shall be provided.
                                       *   - false: Update the periodic advertising data. */
	uint8_t *pa_data;                 /*!< Periodic advertising data. */
	uint16_t pa_len;                  /*!< Periodic Advertising Data Length. */
} rtk_bt_le_pa_update_t;
#endif

/**
 * @struct    rtk_bt_le_scan_info_filter_param_t
 * @brief     Bluetooth LE GAP scan info filter parameters definition.
 */
typedef struct {
	bool enable;                      /*!< Whether to enable the scan info comparison function. */
	uint8_t offset;                   /*!< The start offset of the scan info to compare. */
	uint8_t len;                      /*!< Length of data to compare. */
	uint8_t *p_filter;                /*!< Point the data to compare with the scan info. */
} rtk_bt_le_scan_info_filter_param_t;

/**
 * @struct    rtk_bt_le_scan_param_t
 * @brief     Bluetooth LE GAP scan parameters definition.
 */
typedef struct {
	rtk_bt_le_scan_type_t type;                  /*!< Scanning mode. */
	uint16_t interval;                           /*!< This is defined as the time interval from when the Controller started its last LE scan until it begins the subsequent
                                                  *   LE scan.
                                                  *   Range: 0x0004 to 0x4000.
                                                  *   Default: 0x0010 (10 ms).
                                                  *   Time = N * 0.625 ms.
                                                  *   Time Range: 2.5 ms to 10.24 s. */
	uint16_t window;                             /*!< The duration of the LE scan. LE Scan Window shall be less than or equal to LE Scan Interval.
                                                  *   Range: 0x0004 to 0x4000.
                                                  *   Default: 0x0010 (10 ms).
                                                  *   Time = N * 0.625 ms.
                                                  *   Time Range: 2.5 ms to 10.24 s. */
	rtk_bt_le_addr_type_t own_addr_type;         /*!< Address type used by the Controller during scanning. */
	rtk_bt_le_scan_filter_t filter_policy;       /*!< Filter policy determining which advertising packets are accepted during scanning. */
	uint8_t duplicate_opt;                       /*!< Scanning Filter Duplicated Option, @ref rtk_bt_le_scan_duplicate_t.
                                                  *   Controls whether duplicate advertising reports are filtered out during the current scan period. */
} rtk_bt_le_scan_param_t;

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
/**
 * @struct    rtk_bt_le_ext_scan_param_t
 * @brief     Bluetooth LE GAP ext scan parameters definition.
 */
typedef struct {
	rtk_bt_le_addr_type_t own_addr_type;         /*!< Own Address Type. */
	bool phys[2];                                /*!< Scanning PHYs:
                                                  *   phys[0]: 1M PHY.
                                                  *   phys[1]: Coded PHY.
                                                  *   - true: scanning enabled on that PHY.
                                                  *   - false: scanning disabled on that PHY. */
	rtk_bt_le_scan_type_t type[2];               /*!< LE Scan Type:
                                                  *   type[0] for 1M PHY,
                                                  *   type[1] for Coded PHY. */
	uint16_t interval[2];                        /*!< This is defined as the time interval from when the Controller started its last LE scan until it begins the
                                                  *   subsequent LE scan.
                                                  *   Range: 0x0004 to 0xFFFF.
                                                  *   Time = N * 0.625 ms.
                                                  *   Time Range: 2.5 ms to 40.959375 s. */
	uint16_t window[2];                          /*!< The duration of the LE scan. LE Scan Window shall be less than or equal to LE Scan Interval.
                                                  *   Range: 0x0004 to 0xFFFF.
                                                  *   Time = N * 0.625 ms.
                                                  *   Time Range: 2.5 ms to 40.959375 s. */
	uint16_t duration;                           /*!< 0x0000 indicates scan continuously until explicitly disable.
                                                  *   Other values indicate scan duration.
                                                  *   Range: 0x0001 to 0xFFFF.
                                                  *   Time = N * 10 ms.
                                                  *   Time Range: 10 ms to 655.35 s. */
	uint16_t period;                             /*!< 0x0000 indicates scan continuously.
                                                  *   Other values indicate time interval from when the Controller started its last Scan_Duration until it begins the next Scan_Duration.
                                                  *   Range: 0x0000 to 0xFFFF.
                                                  *   Time = N * 1.28 s.
                                                  *   Time Range: 1.28 s to 83,884.8 s. */
	rtk_bt_le_scan_filter_t filter_policy;       /*!< Scanning Filter Policy. */
	rtk_bt_le_scan_duplicate_t duplicate_opt;    /*!< Scanning Duplicated Option.
                                                  *   Controls whether duplicate advertising reports are filtered out during the current scan period. */
} rtk_bt_le_ext_scan_param_t;
#endif

/**
 * @struct    rtk_bt_le_create_conn_param_t
 * @brief     Bluetooth LE GAP create connect parameters definition.
 */
typedef struct {
	uint16_t scan_interval;                   /*!< This is defined as the time interval from when the Controller started its last LE scan until it begins the subsequent
                                               *   LE scan.
                                               *   Range: 0x0004 to 0x4000.
                                               *   Time = N * 0.625 ms.
                                               *   Time Range: 2.5 ms to 10.24 s. */
	uint16_t scan_window;                     /*!< Amount of time for the duration of the LE scan. LE Scan Window shall be less than or equal to LE Scan Interval.
                                               *   Range: 0x0004 to 0x4000.
                                               *   Time = N * 0.625 ms.
                                               *   Time Range: 2.5 ms to 10.24 s. */
	rtk_bt_le_conn_filter_t filter_policy;    /*!< Initiator Filter Policy. */
	rtk_bt_le_addr_t peer_addr;               /*!< Peer Address. */
	rtk_bt_le_addr_type_t own_addr_type;      /*!< Own Address Type. */
	uint16_t conn_interval_min;               /*!< Minimum value for the connection interval. This shall be less than or equal to Connection Interval Max.
                                               *   Range: 0x0006 to 0x0C80.
                                               *   Time = N * 1.25 ms.
                                               *   Time Range: 7.5 ms to 4 s. */
	uint16_t conn_interval_max;               /*!< Maximum value for the connection interval. This shall be greater than or equal to Connection Interval Min.
                                               *   Range: 0x0006 to 0x0C80.
                                               *   Time = N * 1.25 ms.
                                               *   Time Range: 7.5 ms to 4 s. */
	uint16_t conn_latency;                    /*!< Peripheral latency for the connection in number of connection events.
                                               *   Range: 0x0000 to 0x01F3. */
	uint16_t supv_timeout;                    /*!< Supervision timeout for the LE connection.
                                               *   Range: 0x000A to 0x0C80.
                                               *   Time = N * 10 ms.
                                               *   Time Range: 100 ms to 32 s. */
	uint16_t scan_timeout;                    /*!< Create connection scan timeout value.
                                               *   If time has expired before connection has been established, stack will auto cancel connection.
                                               *   Time = N * 10 ms.
                                               *   0: stack will not cancel create connection forever. */
} rtk_bt_le_create_conn_param_t;

/**
 * @struct    rtk_bt_le_update_conn_param_t
 * @brief     Bluetooth LE GAP update connection parameters definition.
 */
typedef struct {
	uint16_t conn_handle;                     /*!< Connection handle of the ACL connection to update connection parameters.
                                               *   Range: 0x0000 to 0x0EFF. */
	uint16_t conn_interval_min;               /*!< Minimum value for the connection interval. This shall be less than or equal to conn_interval_max.
                                               *   Range: 0x0006 to 0x0C80.
                                               *   Time = N * 1.25 ms.
                                               *   Time Range: 7.5 ms to 4 s. */
	uint16_t conn_interval_max;               /*!< Maximum value for the connection interval. This shall be greater than or equal to conn_interval_min.
                                               *   Range: 0x0006 to 0x0C80.
                                               *   Time = N * 1.25 ms.
                                               *   Time Range: 7.5 ms to 4 s. */
	uint16_t conn_latency;                    /*!< Maximum allowed Peripheral latency in number of connection events.
                                               *   Range: 0x0000 to 0x01F3. */
	uint16_t supv_timeout;                    /*!< Supervision timeout for the LE connection.
                                               *   Range: 0x000A to 0x0C80.
                                               *   Time = N * 10 ms.
                                               *   Time Range: 100 ms to 32 s. */
} rtk_bt_le_update_conn_param_t;

/**
 * @enum      rtk_bt_le_wl_op_t
 * @brief     Bluetooth LE GAP whitelist operation type definition.
 */
typedef enum {
	RTK_BT_LE_WHITELIST_CLEAR,                 /*!< Clear whitelist. */
	RTK_BT_LE_WHITELIST_ADD,                   /*!< Add device to whitelist. */
	RTK_BT_LE_WHITELIST_REMOVE,                /*!< Remove device from whitelist. */
	RTK_BT_LE_WHITELIST_RESERVED,              /*!< All bigger values are reserved. */
} rtk_bt_le_wl_op_t;

/**
 * @struct    rtk_bt_le_modify_wl_param_t
 * @brief     Bluetooth LE GAP whitelist operation parameters definition.
 */
typedef struct {
	rtk_bt_le_wl_op_t op;                      /*!< Whitelist operation type. */
	rtk_bt_le_addr_t addr;                     /*!< Device address. */
} rtk_bt_le_modify_wl_param_t;

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
/**
 * @enum      rtk_bt_le_resolv_list_op_t
 * @brief     Bluetooth LE resolving list operation definition.
 */
typedef enum {
	RTK_BT_LE_RESOLV_LIST_OP_CLEAR  = 0x00,    /*!< Clear resolving list operation. */
	RTK_BT_LE_RESOLV_LIST_OP_ADD    = 0x01,    /*!< Add a device to the resolving list operation. */
	RTK_BT_LE_RESOLV_LIST_OP_REMOVE = 0x02     /*!< Remove a device from the resolving list operation. */
} rtk_bt_le_resolv_list_op_t;

/**
 * @struct    rtk_bt_le_resolv_list_entry_t
 * @brief     Bluetooth LE modify resolving list parameter.
 */
typedef struct {
	rtk_bt_le_ident_addr_type_t addr_type;     /*!< Peer identity address type. */
	uint8_t addr[RTK_BD_ADDR_LEN];             /*!< Peer identity address. */
	bool device_mode;                          /*!< Peer enable device privacy mode. */
} rtk_bt_le_resolv_list_entry_t;

/**
 * @struct    rtk_bt_le_modify_resolv_list_t
 * @brief     Bluetooth LE modify resolving list parameter.
 */
typedef struct {
	rtk_bt_le_resolv_list_op_t op;             /*!< Resolving list modify operation. */
	rtk_bt_le_resolv_list_entry_t entry;       /*!< Resolving list entry. */
} rtk_bt_le_modify_resolv_list_t;

/**
 * @struct    rtk_bt_le_modify_resolv_list_t
 * @brief     Bluetooth LE modify resolving list parameter.
 */
typedef struct {
	rtk_bt_le_resolv_list_entry_t *p_entry;    /*!< Resolving list entry. */
	uint8_t *p_size;                           /*!< Resolving list entry size. */
} rtk_bt_le_get_resolv_list_t;
#endif

/**
 * @def       RTK_BT_LE_CHAN_MAP_LEN
 * @brief     Bluetooth LE GAP channel map size macro.
 */
#define RTK_BT_LE_CHAN_MAP_LEN                5

/**
 * @struct    rtk_bt_le_conn_info_t
 * @brief     Bluetooth LE connection info definition.
 */
typedef struct {
	rtk_bt_le_link_role_t role;    /*!< Connection role. */
	rtk_bt_le_addr_t remote;       /*!< Peer device address for this connection. */
	uint16_t interval;             /*!< Connection interval used on this connection.
                                    *   Range: 0x0006 to 0x0C80.
                                    *   Time = N * 1.25 ms.
                                    *   Time Range: 7.5 ms to 4000 ms. */
	uint16_t latency;              /*!< Peripheral latency for the connection in number of connection events.
                                    *   Range: 0x0000 to 0x01F3. */
	uint16_t supv_timeout;         /*!< Connection supervision timeout for this connection.
                                    *   Range: 0x000A to 0x0C80.
                                    *   Time = N * 10 ms.
                                    *   Time Range: 100 ms to 32 s. */
	uint8_t tx_phy;                /*!< Connection tx phy.
                                    *   0x1: LE 1M PHY.
                                    *   0x2: LE 2M PHY.
                                    *   0x3: LE Coded PHY. */
	uint8_t rx_phy;                /*!< Connection rx phy.
                                    *   0x1: LE 1M PHY.
                                    *   0x2: LE 2M PHY.
                                    *   0x3: LE Coded PHY. */
} rtk_bt_le_conn_info_t;

/**
 * @enum      rtk_bt_le_gap_init_state
 * @brief     Bluetooth LE GAP init state.
 */
enum rtk_bt_le_gap_init_state {
	RTK_BT_LE_GAP_INIT_STATE_INIT           = 0,    /*!< GAP is in initiating status. */
	RTK_BT_LE_GAP_INIT_STATE_STACK_READY    = 1,    /*!< GAP stack is ready. */
};

/**
 * @enum      rtk_bt_le_adv_state
 * @brief     Bluetooth LE GAP adv state.
 */
enum rtk_bt_le_adv_state {
	RTK_BT_LE_ADV_STATE_IDLE                = 0,    /*!< Idle, no advertising. */
	RTK_BT_LE_ADV_STATE_START               = 1,    /*!< Start Advertising. A temporary state, haven't received the result. */
	RTK_BT_LE_ADV_STATE_ADVERTISING         = 2,    /*!< Advertising. */
	RTK_BT_LE_ADV_STATE_STOP                = 3,    /*!< Stop Advertising. A temporary state, haven't received the result. */
};

/**
 * @enum      rtk_bt_le_scan_state
 * @brief     Bluetooth LE GAP scan state.
 */
enum rtk_bt_le_scan_state {
	RTK_BT_LE_SCAN_STATE_IDLE               = 0,    /*!< Idle, no scanning. */
	RTK_BT_LE_SCAN_STATE_START              = 1,    /*!< Start scanning. A temporary state, haven't received the result. */
	RTK_BT_LE_SCAN_STATE_SCANNING           = 2,    /*!< Scanning. */
	RTK_BT_LE_SCAN_STATE_STOP               = 3,    /*!< Stop scanning. A temporary state, haven't received the result. */
};

/**
 * @enum      rtk_bt_le_conn_state
 * @brief     Bluetooth LE GAP connection state.
 */
enum rtk_bt_le_conn_state {
	RTK_BT_LE_CONN_STATE_IDLE               = 0,    /*!< Idle. */
	RTK_BT_LE_CONN_STATE_INITIATING         = 1,    /*!< Initiating Connection. */
};

/**
 * @struct    rtk_bt_le_gap_dev_state_t
 * @brief     Bluetooth LE GAP dev state.
 */
typedef struct {
	uint8_t gap_init_state;                         /*!< @ref rtk_bt_le_gap_init_state. */
	uint8_t gap_adv_state;                          /*!< @ref rtk_bt_le_adv_state. */
	uint8_t gap_scan_state;                         /*!< @ref rtk_bt_le_scan_state. */
	uint8_t gap_conn_state;                         /*!< @ref rtk_bt_le_conn_state. */
} rtk_bt_le_gap_dev_state_t;

/**
 * @struct    rtk_bt_le_get_active_conn_t
 * @brief     Bluetooth LE GAP get active connection num and their handles.
 */
typedef struct {
	uint16_t conn_handle[RTK_BLE_GAP_MAX_LINKS];    /*!< Connection handles of active ACL connections.
                                                     *   Range: 0x0000 to 0x0EFF. */
	uint16_t conn_num;                              /*!< Number of active connections. */
} rtk_bt_le_get_active_conn_t;

#if defined(RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT) && RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT
/**
 * @struct    rtk_bt_le_set_datalen_param_t
 * @brief     Bluetooth LE GAP set connection data length parameters definition.
 */
typedef struct {
	uint16_t conn_handle;                           /*!< Connection handle of the ACL connection to set data len parameters.
                                                     *   Range: 0x0000 to 0x0EFF. */
	uint16_t max_tx_octets;                         /*!< Preferred maximum number of payload octets that the local Controller should include in a single LL Data PDU
                                                     *   on this connection.
                                                     *   Range: 0x001B to 0x00FB.
                                                     *   ALL other values reserved for future use. */
	uint16_t max_tx_time;                           /*!< Preferred maximum number of microseconds that the local Controller should use to transmit a single Link Layer
                                                     *   packet containing an LL Data PDU on this connection.
                                                     *   Range 0x0148 to 0x4290.
                                                     *   ALL other values reserved for future use. */
} rtk_bt_le_set_datalen_param_t;
#endif

#if defined(RTK_BLE_5_0_SET_PHYS_SUPPORT) && RTK_BLE_5_0_SET_PHYS_SUPPORT
/**
 * @typedef   rtk_bt_le_all_phy_prefer_t
 * @brief     Bluetooth LE ALL_PHYs prefer bit field.
 */
typedef enum {
	RTK_BT_LE_PHYS_PREFER_ALL    = 0,
	RTK_BT_LE_PHYS_NO_PREFER_TX  = (1 << 0),         /*!< The Host has no preference among the transmitter PHYs supported by the Controller. */
	RTK_BT_LE_PHYS_NO_PREFER_RX  = (1 << 1),         /*!< The Host has no preference among the receiver PHYs supported by the Controller. */
} rtk_bt_le_all_phy_prefer_t;

/**
 * @typedef   rtk_bt_le_trx_phy_prefer_t
 * @brief     Bluetooth LE TX_PHYs or RX_PHYs prefer bit field.
 */
typedef enum {
	RTK_BT_LE_PHYS_PREFER_1M     = (1 << 0),         /*!< The Host prefers to use the LE 1M transmitter PHY (possibly among others). */
	RTK_BT_LE_PHYS_PREFER_2M     = (1 << 1),         /*!< The Host prefers to use the LE 2M transmitter PHY (possibly among others). */
	RTK_BT_LE_PHYS_PREFER_CODED  = (1 << 2),         /*!< The Host prefers to use the LE Coded transmitter PHY (possibly among others). */
} rtk_bt_le_trx_phy_prefer_t;

/**
 * @typedef   rtk_bt_le_phy_option_t
 * @brief     Bluetooth LE PHY options.
 */
typedef enum {
	RTK_BT_LE_PHYS_OPTIONS_CODED_PREFER_NO = 0x0,    /*!< Host has no preferred coding when transmitting on the LE Coded PHY. */
	RTK_BT_LE_PHYS_OPTIONS_CODED_PREFER_S2 = 0x1,    /*!< Host prefers that S=2 coding be used when transmitting on the LE Coded PHY. */
	RTK_BT_LE_PHYS_OPTIONS_CODED_PREFER_S8 = 0x2,    /*!< Host prefers that S=8 coding be used when transmitting on the LE Coded PHY. */
} rtk_bt_le_phy_option_t;

/**
 * @struct    rtk_bt_le_set_phy_param_t
 * @brief     Bluetooth LE GAP set connection phy parameters definition.
 */
typedef struct {
	uint16_t conn_handle;                            /*!< Connection handle of the ACL connection to set PHY parameters.
                                                      *   Range: 0x0000 to 0x0EFF. */
	uint8_t all_phys;                                /*!< Preferred all phys, @ref rtk_bt_le_all_phy_prefer_t. */
	uint8_t tx_phys;                                 /*!< Preferred tx phys, @ref rtk_bt_le_trx_phy_prefer_t. */
	uint8_t rx_phys;                                 /*!< Preferred rx phys, @ref rtk_bt_le_trx_phy_prefer_t. */
	uint16_t phy_options;                            /*!< Preferred phy options, @ref rtk_bt_le_phy_option_t. */
} rtk_bt_le_set_phy_param_t;
#endif

/**
 * @typedef   rtk_bt_le_pairing_mode_t
 * @brief     Bluetooth LE SM pairing mode definition.
 */
typedef enum {
	RTK_PAIRING_MODE_NO_PAIRING  = 0x00,
	RTK_PAIRING_MODE_PAIRABLE    = 0x01,
} rtk_bt_le_pairing_mode_t;

/**
 * @typedef   rtk_bt_le_io_cap_t
 * @brief     Bluetooth LE SM IO capability type definition.
 */
typedef enum {
	RTK_IO_CAP_DISPALY_ONLY     = 0x00,    /*!< 0x00 DisplayOnly. */
	RTK_IO_CAP_DISPLAY_YES_NO   = 0x01,    /*!< 0x01 DisplayYesNo. */
	RTK_IO_CAP_KEYBOARD_ONLY    = 0x02,    /*!< 0x02 KeyboardOnly. */
	RTK_IO_CAP_NO_IN_NO_OUT     = 0x03,    /*!< 0x03 NoInputNoOutput. */
	RTK_IO_CAP_KEYBOARD_DISPALY = 0x04,    /*!< 0x04 KeyboardDisplay. */
	RTK_IO_CAP_MAX              = 0x05,    /*!< 0x05-0xFF Reserved for future use. */
} rtk_bt_le_io_cap_t;

/**
 * @def       RTK_PAIRING_PASSKEY_MAX
 * @brief     Bluetooth LE SM max passkey value macro.
 */
#define RTK_PAIRING_PASSKEY_MAX     999999

/**
 * @struct    rtk_bt_le_security_param_t
 * @brief     Bluetooth LE SM security parameter type definition.
 */
typedef struct {
	rtk_bt_le_io_cap_t io_cap;             /*!< IO Capability. */
	uint8_t oob_data_flag;                 /*!< OOB data flag.
                                            *   1: OOB Authentication data from remote device present.
                                            *   0: OOB Authentication data not present.
                                            *   ALL other values reserved. */
	uint8_t bond_flag;                     /*!< Bonding flags.
                                            *   1: Bonding.
                                            *   0: No bonding.
                                            *   ALL other values reserved. */
	uint8_t mitm_flag;                     /*!< MITM flag.
                                            *   1: Request MITM protection.
                                            *   0: Not request MITM protection.
                                            *   ALL other values reserved. */
	uint8_t sec_pair_flag;                 /*!< Secure connection pairing support flag.
                                            *   1: Support Secure connection pairing.
                                            *   0: Not support Secure connection pairing.
                                            *   ALL other values reserved. */
	uint8_t sec_pair_only_flag;            /*!< Only accept LE Secure Connection pairing when local sec_pair_flag is set.
                                            *   1: Only accept LE Secure Connection pairing when local sec_pair_flag is set.
                                            *   0: Accept LE Secure Connection pairing and legacy pairing.
                                            *   ALL other values reserved. */
	uint8_t use_fixed_key;                 /*!< Pairing use fixed passkey.
                                            *   1: Use fixed passkey when pairing, fixed passkey ref @ref fixed_key.
                                            *   0: Use random passkey when pairing.
                                            *   ALL other values reserved. */
	uint32_t fixed_key;                    /*!< Fixed passkey value.
                                                Only used when @ref use_fixed_key is 1. */
	uint8_t auto_sec_req;                  /*!< Auto send security request when connected.
                                            *   1: Stack auto send security request when connected.
                                            *   0: Stack not auto send security request when connected.
                                            *   ALL other values reserved. */
	uint8_t sign_key_flag;                 /*!< Sign key flag, this field can't be get by @ref rtk_bt_le_sm_get_security_param.
                                            *   1: Distribution sign key when pairing.
                                            *   0: No distribution sign key when pairing.
                                            *   ALL other values reserved. */
} rtk_bt_le_security_param_t;

/**
 * @struct    rtk_bt_le_pair_cfm_t
 * @brief     Bluetooth LE SM pairing confirm parameter definition.
 */
typedef struct {
	uint16_t conn_handle;          /*!< Connection handle of the ACL connection awaiting pairing confirmation.
                                    *   Copied from the @ref RTK_BT_LE_GAP_EVT_AUTH_PAIRING_CONFIRM_IND event.
                                    *   Range: 0x0000 to 0x0EFF. */
	uint8_t confirm;               /*!< Pairing decision.
                                    *   1: accept (allow the pairing procedure to continue).
                                    *   0: reject (stack sends a Pairing Failed PDU and aborts). */
} rtk_bt_le_pair_cfm_t;

/**
 * @struct    rtk_bt_le_auth_key_confirm_t
 * @brief     Bluetooth LE SM key confirm definition.
 */
typedef struct {
	uint16_t conn_handle;          /*!< Connection handle of the ACL connection undergoing Numeric Comparison pairing.
                                    *   Copied from the @ref RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_CONFIRM_IND event.
                                    *   Range: 0x0000 to 0x0EFF. */
	uint8_t confirm;               /*!< Numeric Comparison result.
                                    *   1: the six-digit values shown on both devices match; pairing proceeds.
                                    *   0: values differ; stack sends Pairing Failed and aborts. */
} rtk_bt_le_auth_key_confirm_t;

/**
 * @struct    rtk_bt_le_auth_key_input_t
 * @brief     Bluetooth LE SM input auth key definition.
 */
typedef struct {
	uint16_t conn_handle;          /*!< Connection handle of the ACL connection requesting passkey input.
                                    *   Copied from the @ref RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_INPUT_IND event.
                                    *   Range: 0x0000 to 0x0EFF. */
	uint32_t passkey;              /*!< Six-digit decimal passkey entered by the user.
                                    *   Range: 000000 to 999999.
                                    *   Must match the value displayed on the initiating peer device. */
} rtk_bt_le_auth_key_input_t;

/**
 * @def       RTK_BT_LE_SM_OOB_KEY_LEN
 * @brief     Bluetooth LE SM max oob key length macro.
 */
#define RTK_BT_LE_SM_OOB_KEY_LEN              16

/**
 * @struct    rtk_bt_le_set_oob_key_t
 * @brief     Bluetooth LE SM set OOB key parameter definition.
 */
typedef struct {
	uint16_t conn_handle;                         /*!< Connection handle of the ACL connection to set OOB data.
                                                   *   Copied from the @ref RTK_BT_LE_GAP_EVT_AUTH_OOB_KEY_INPUT_IND event.
                                                   *   Range: 0x0000 to 0x0EFF. */
	uint8_t oob_key[RTK_BT_LE_SM_OOB_KEY_LEN];    /*!< OOB Key to input. */
} rtk_bt_le_set_oob_key_t;

/**
 * @struct    rtk_bt_le_sc_local_oob_data_t
 * @brief     Bluetooth LE SM get secure connection pairing local OOB data definition.
 */
typedef struct {
	uint8_t rand[16];                     /*!< Local OOB random value. Transmitted to the peer device via the out-of-band channel. */
	uint8_t confirm[16];                  /*!< LE Secure Connections OOB confirm value. */
} rtk_bt_le_sc_local_oob_data_t;

/**
 * @struct    rtk_bt_le_sc_peer_oob_data_t
 * @brief     Bluetooth LE SM input secure connection pairing peer OOB data definition.
 */
typedef struct {
	uint8_t addr[RTK_BD_ADDR_LEN];        /*!< Bluetooth device address of the peer that generated the OOB data. */
	uint8_t rand[16];                     /*!< Random number generated by the peer and received via the out-of-band channel. */
	uint8_t confirm[16];                  /*!< Confirm value computed by the peer and received via the out-of-band channel. */
} rtk_bt_le_sc_peer_oob_data_t;

/**
 * @struct    rtk_bt_le_bond_info_t
 * @brief     Bluetooth LE SM bond information type definition.
 */
typedef struct {
	rtk_bt_le_addr_t remote_addr;         /*!< Bonded device address. */
	rtk_bt_le_ident_addr_t ident_addr;    /*!< Identity address. */
} rtk_bt_le_bond_info_t;

/**
 * @struct    rtk_bt_le_adv_start_ind_t
 * @brief     Bluetooth LE adv start indication msg.
 */
typedef struct {
	uint16_t err;                         /*!< Status of the start adv procedure; 0x00 on success. */
	rtk_bt_le_adv_type_t  adv_type;       /*!< Adv type. */
} rtk_bt_le_adv_start_ind_t;

/**
 * @struct    rtk_bt_le_adv_stop_ind_t
 * @brief     Bluetooth LE adv stop indication msg.
 */
typedef struct {
	uint16_t err;                                /*!< Status of the stop adv procedure; 0x00 on success. */
	rtk_bt_le_adv_stop_reason_t  stop_reason;    /*!< Reason for advertising stopped. */
} rtk_bt_le_adv_stop_ind_t;

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
/**
 * @struct    rtk_bt_le_ext_adv_ind_t
 * @brief     Bluetooth LE ext adv state indication msg.
 */
typedef struct {
	uint8_t adv_handle;                             /*!< Handle of the advertising set, assigned by @ref rtk_bt_le_gap_create_ext_adv. */
	bool is_start;                                  /*!< - true: advertising started.
                                                     *   - false: advertising stopped. */
	uint16_t err;                                   /*!< Status of the start/stop extended adv procedure; 0x00 on success. */
	rtk_bt_le_adv_stop_reason_t stop_reason;        /*!< Reason advertising stopped; only valid when is_start is false. */
	uint16_t conn_handle;                           /*!< Connection handle of the ACL connection that stopped advertising, only valid when stop_reason is @ref RTK_BT_LE_ADV_STOP_BY_CONN.
                                                     *   Range: 0x0000 to 0x0EFF. */
} rtk_bt_le_ext_adv_ind_t;
#endif

#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
/**
 * @typedef   rtk_bt_le_pa_state_t
 * @brief     Bluetooth LE periodic adv state.
 */
typedef enum {
	RTK_BT_LE_PA_STATE_IDLE,                        /*!< Idle, no Periodic advertising. */
	RTK_BT_LE_PA_STATE_ADVERTISING,                 /*!< Periodic advertising is active and PDUs are being transmitted. */
	RTK_BT_LE_PA_STATE_WAIT_EXT_ADV_ADVERTISING,    /*!< Periodic advertising is configured but cannot start until the associated extended advertising set (adv_handle) is
                                                     *   enabled. Once that extended advertising set starts, periodic advertising begins automatically. */
} rtk_bt_le_pa_state_t;

/**
 * @struct    rtk_bt_le_pa_ind_t
 * @brief     Bluetooth LE periodic adv state indication msg.
 */
typedef struct {
	uint8_t adv_handle;                             /*!< Identify an advertising set, assigned by @ref rtk_bt_le_gap_create_ext_adv. */
	rtk_bt_le_pa_state_t state;                     /*!< New periodic advertising state. */
	uint16_t cause;                                 /*!< Status of the periodic adv procedure; 0 on success. */
} rtk_bt_le_pa_ind_t;
#endif

#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
/**
 * @typedef   rtk_bt_le_pa_synced_info_t
 * @brief     Bluetooth LE GAP PA Sync Common Sync parameter.
 */
typedef struct {
	uint16_t sync_handle;                                          /*!< Sync_Handle identifying the periodic advertising train.
                                                                    *   Range: 0x0000 to 0x0EFF. */
	uint8_t adv_sid;                                               /*!< Advertising SID from the ADI field of the advertising PDU referring to the periodic advertising train.
                                                                    *   Range: 0x00 to 0x0F. */
	rtk_bt_le_addr_t addr;                                         /*!< Address of the periodic advertiser. */
	rtk_bt_le_phy_type_t adv_phy;                                  /*!< PHY used for the periodic advertising. */
	uint8_t adv_clock_accuracy;                                    /*!< Sleep clock accuracy of the advertiser.
                                                                    *   Range: 0x00 to 0x07.
                                                                    *   0x00: 500 ppm.
                                                                    *   0x01: 250 ppm.
                                                                    *   0x02: 150 ppm.
                                                                    *   0x03: 100 ppm.
                                                                    *   0x04: 75 ppm.
                                                                    *   0x05: 50 ppm.
                                                                    *   0x06: 30 ppm.
                                                                    *   0x07: 20 ppm.
                                                                    *   All bigger values are reserved for future use. */
	uint16_t periodic_adv_interval;                                /*!< Periodic advertising interval.
                                                                    *   Range: 0x0006 to 0xFFFF.
                                                                    *   Time = N * 1.25 ms.
                                                                    *   Time Range: 7.5 ms to 81.91875 s. */
#if defined(RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT) && RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT
	uint8_t num_subevents;                                         /*!< Number of subevents in each periodic advertising event.
                                                                    *   0x00: Periodic advertising without responses.
                                                                    *   0x01 to 0x80: Number of subevents. */
	uint8_t subevent_interval;                                     /*!< Time between consecutive subevents.
                                                                    *   Range: 0x06 to 0xFF.
                                                                    *   Time = N * 1.25 ms.
                                                                    *   Time Range: 7.5 ms to 318.75 ms.
                                                                    *   Ignored when num_subevents is 0x00 or 0x01. */
	uint8_t response_slot_delay;                                   /*!< Time from the start of a subevent to the first response slot.
                                                                    *   Range: 0x01 to 0xFE.
                                                                    *   Time = N * 1.25 ms.
                                                                    *   Time Range: 1.25 ms to 317.5 ms.
                                                                    *   Ignored when num_subevents is 0x00. */
	uint8_t response_slot_spacing;                                 /*!< Time between consecutive response slots within a subevent.
                                                                    *   Range: 0x02 to 0xFF.
                                                                    *   Time = N * 0.125 ms.
                                                                    *   Time Range: 0.25 ms to 31.875 ms.
                                                                    *   Ignored when num_subevents is 0x00. */
#endif
	bool past_received;                                            /*!< Indicates whether synchronization was established via Periodic Advertising Sync Transfer (PAST).
                                                                    *   - true: Established via PAST.
                                                                    *   - false: Established via local create-sync procedure. */
	uint16_t conn_handle;                                          /*!< Connection handle of the ACL connection for which PAST was received.
                                                                    *   Only valid when past_received is true.
                                                                    *   Range: 0x0000 to 0x0EFF. */
	uint16_t service_data;                                         /*!< Service_Data value provided by the Host of the device sending the synchronization information.
                                                                    *   Only valid when past_received is true. */
} rtk_bt_le_pa_synced_info_t;

/**
 * @typedef   rtk_bt_le_pa_sync_state_t
 * @brief     Bluetooth LE GAP PA synchronization state.
 */
typedef enum {
	RTK_BT_LE_PA_SYNC_STATE_TERMINATED                  = 0x00,    /*!< Synchronization has been terminated; no periodic advertising reports will be generated. */
	RTK_BT_LE_PA_SYNC_STATE_SYNCHRONIZING_WAIT_SCANNING = 0x02,    /*!< Waiting for extended scanning to be enabled before it can attempt synchronization.
                                                                    *   No synchronization attempt is made while extended scanning is disabled. */
	RTK_BT_LE_PA_SYNC_STATE_SYNCHRONIZING               = 0x03,    /*!< Synchronization is in progress. */
	RTK_BT_LE_PA_SYNC_STATE_SYNCHRONIZED                = 0x04,    /*!< Synchronization has been successfully established; periodic advertising reports will be delivered
                                                                    *   via @ref RTK_BT_LE_GAP_EVT_PA_ADV_REPORT_IND. */
} rtk_bt_le_pa_sync_state_t;

/**
 * @struct    rtk_bt_le_pa_sync_ind_t
 * @brief     Bluetooth LE periodic adv synchronization state indication msg.
 */
typedef struct {
	uint8_t sync_id;                                               /*!< Identify the synchronization with a periodic advertising train. */
	rtk_bt_le_pa_sync_state_t state;                               /*!< New synchronization state. */
	uint16_t cause;                                                /*!< Status of the periodic adv synchronization procedure; 0x00 on success. */
	rtk_bt_le_pa_synced_info_t info;                               /*!< Synchronization info; only valid when state == RTK_BT_LE_PA_SYNC_STATE_SYNCHRONIZED. */
} rtk_bt_le_pa_sync_ind_t;

/**
 * @struct    rtk_bt_le_gap_pawr_data_req_ind_t
 * @brief     Bluetooth LE periodic adv subevent data request indication msg.
 */
typedef struct {
	uint8_t adv_handle;                 /*!< Identify an advertising set, assigned by @ref rtk_bt_le_gap_create_ext_adv. */
	uint8_t start;                      /*!< Index of the first subevent for which data can be set. */
	uint8_t count;                      /*!< Number of consecutive subevents for which data can be set. */
} rtk_bt_le_gap_pawr_data_req_ind_t;

/**
 * @struct    rtk_bt_le_gap_pawr_rsp_ind_t
 * @brief     Bluetooth LE periodic adv subevent data request indication msg.
 */
typedef struct {
	uint8_t adv_handle;                 /*!< Identify an advertising set, assigned by @ref rtk_bt_le_gap_create_ext_adv. */
	uint8_t subevent;                   /*!< Subevent index in which the response was received.
                                         *   Range: 0x00 to 0x7F. */
	uint8_t tx_status;                  /*!< Transmission status of the AUX_SYNC_SUBEVENT_IND packet.
                                         *   0x00: AUX_SYNC_SUBEVENT_IND packet was transmitted.
                                         *   0x01: AUX_SYNC_SUBEVENT_IND packet was not transmitted.
                                         *   All bigger values are reserved for future use. */
	int8_t tx_power;                    /*!< TX power of the response PDU.
                                         *   Range: -127 to +20 dBm.
                                         *   0x7F: TX power information is unavailable. */
	int8_t rssi;                        /*!< RSSI of the received response PDU.
                                         *   Range: -127 to +20 dBm.
                                         *   0x7F: RSSI is unavailable. */
	uint8_t cte_type;                   /*!< Constant Tone Extension type of the response; see @ref rtk_bt_le_gap_cte_type_t. */
	uint8_t response_slot;              /*!< Response slot index within the subevent in which the response was received.
                                         *   Range: 0x00 to 0xFF. */
	uint16_t len;                       /*!< Length of the received response data. */
	uint8_t *data;                      /*!< Pointer to the received Periodic advertising response data. */
} rtk_bt_le_gap_pawr_rsp_ind_t;

/**
 * @typedef   rtk_bt_le_pa_sync_report_data_status_t
 * @brief     Bluetooth LE Periodic Advertising Report data status definition.
 */
typedef enum {
	RTK_BT_LE_PA_REPORT_DATA_STATUS_COMPLETE  = 0x00, /*!< Data complete. */
	RTK_BT_LE_PA_REPORT_DATA_STATUS_MORE      = 0x01, /*!< Data incomplete, more data to come. */
	RTK_BT_LE_PA_REPORT_DATA_STATUS_TRUNCATED = 0x02, /*!< Data incomplete, data truncated, no more to come. */
	RTK_BT_LE_PA_REPORT_DATA_STATUS_RX_FAILED = 0xFF, /*!< Data received fail, no data is received. */
} rtk_bt_le_pa_sync_report_data_status_t;

/**
 * @struct    rtk_bt_le_pa_adv_report_ind_t
 * @brief     Bluetooth LE periodic adv synchronization adv report indication msg.
 */
typedef struct {
	uint8_t sync_id;                    /*!< Application-level sync identifier matching the value assigned at create time. */
	uint16_t sync_handle;               /*!< Handle identifying the periodic advertising train. */
	int8_t tx_power;                    /*!< TX power information for the periodic advertising report.
                                         *   Range: -127 to +20 dBm.
                                         *   0x7F: TX power information is unavailable. */
	int8_t rssi;                        /*!< RSSI of the received periodic advertising packet, ignoring any Constant Tone Extension.
                                         *   Range: -127 to +20 dBm.
                                         *   0x7F: RSSI is unavailable. */
	uint8_t cte_type;                   /*!< Constant Tone Extension type present in this packet; see @ref rtk_bt_le_gap_cte_type_t. */
	uint8_t data_status;                /*!< Data status for this periodic advertising report; see @ref rtk_bt_le_pa_sync_report_data_status_t. */
#if defined(RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT) && RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT
	uint16_t periodic_event_counter;    /*!< Event counter value of the periodic advertising event in which this subevent indication was received. */
	uint8_t subevent;                   /*!< Subevent index within the periodic advertising event where this indication was received.
                                         *   Range: 0x00 to 0x7F.
                                         *   0xFF: Periodic Advertising does not have subevents. */
#endif
	uint8_t data_len;                   /*!< Length of the advertising payload in p_data.
                                         *   Range: 0 to 247. */
	uint8_t *p_data;                    /*!< Pointer to the periodic advertising payload data. */
} rtk_bt_le_pa_adv_report_ind_t;

/**
 * @typedef   rtk_bt_le_pa_sync_create_opt_t
 * @brief     Synchronization State of Periodic Advertising Create Sync Options.
 *            Use the combination of macro definitions to determine whether the Periodic Advertiser List is used,
 *            whether periodic advertising reports for this periodic advertising train are initially enabled or disabled,
 *            and whether duplicate reports are filtered or not.
 */
typedef enum {
	RTK_BT_LE_PA_SYNC_CREATE_OPTIONS_USE_PERIODIC_ADV_LIST              = 0x01,    /*!< Use the Periodic Advertiser List to determine which advertiser to listen to.
                                                                                    *   Otherwise use the Advertising_SID and adv_addr fields to determine. */
	RTK_BT_LE_PA_SYNC_CREATE_OPTIONS_REPORT_INITIALLY_DISABLED          = 0x02,    /*!< Reports initially disabled.
                                                                                    *   Otherwise reports enabled. */
	RTK_BT_LE_PA_SYNC_CREATE_OPTIONS_DUPLICATE_FILTER_INITIALLY_ENABLED = 0x04,    /*!< Duplicate report filtering is initially enabled.
                                                                                    *   Otherwise duplicate filtering initially disabled. */
} rtk_bt_le_pa_sync_create_opt_t;

/**
 * @typedef   rtk_bt_le_pa_sync_cte_t
 * @brief     Synchronization State of Periodic Advertising Sync CTE Type.
 *            Use the combination of macro definitions to specify whether to only synchronize to periodic advertising
 *            with certain types of Constant Tone Extension (a value of 0 indicates that the presence or absence of a
 *            Constant Tone Extension is irrelevant).
 */
typedef enum {
	RTK_BT_LE_PA_SYNC_CTE_TYPE_CTE_IRRELEVANT            = 0x00,   /*!< A value of 0 (i.e. all bits clear) indicates that the presence or absence of
                                                                        a Constant Tone Extension is irrelevant. */

	RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITH_AOA_CTE     = 0x01,   /*!< Do not sync to packets with an AoA Constant Tone Extension. */
	RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITH_AOD_CTE_1US = 0x02,   /*!< Do not sync to packets with an AoD Constant Tone Extension with 1 us slots. */
	RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITH_AOD_CTE_2US = 0x04,   /*!< Do not sync to packets with an AoD Constant Tone Extension with 2 us slots. */
	RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITH_TYPE_3_CTE  = 0x08,   /*!< Do not sync to packets with a type 3 Constant Tone Extension (currently reserved for future use). */
	RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITHOUT_CTE      = 0x10,   /*!< Do not sync to packets without a Constant Tone Extension. */
} rtk_bt_le_pa_sync_cte_t;

/**
 * @struct    rtk_bt_le_pa_sync_create_t
 * @brief     Bluetooth LE periodic adv synchronization create sync parameter.
 */
typedef struct {
	uint8_t options;              /*!< Bitmask of @ref rtk_bt_le_pa_sync_create_opt_t controlling list usage. */
	uint8_t sync_cte_type;        /*!< Bitmask of @ref rtk_bt_le_pa_sync_cte_t specifying which CTE types to exclude. */
	uint8_t adv_sid;              /*!< If Periodic Advertiser List is not used (@ref rtk_bt_le_pa_sync_create_opt_t),
                                   *   Advertising SID subfield in the ADI field used to identify the Periodic Advertising. */
	rtk_bt_le_addr_t adv_addr;    /*!< Address of the target periodic advertiser.
                                   *   Used only when RTK_BT_LE_PA_SYNC_CREATE_OPTIONS_USE_PERIODIC_ADV_LIST is not set.
                                   *   Only @ref RTK_BT_LE_ADDR_TYPE_PUBLIC and @ref RTK_BT_LE_ADDR_TYPE_RANDOM could be used for creating sync. */
	uint16_t skip;                /*!< The maximum number of periodic advertising events that can be skipped after a successful receive.
                                   *   Range: 0x0000 to 0x01F3. */
	uint16_t sync_timeout;        /*!< Maximum time allowed to receive a periodic advertising packet before sync is considered lost.
                                   *   Range: 0x000A to 0x4000.
                                   *   Time = N * 10 ms.
                                   *   Time Range: 100 ms to 163.84 s. */
	uint8_t *p_sync_id;           /*!< Output: assigned sync_id. */
} rtk_bt_le_pa_sync_create_t;

/**
 * @typedef   rtk_bt_le_pa_sync_advlist_op_t
 * @brief     Definition of LE Periodic Advertiser list operation.
 */
typedef enum {
	RTK_BT_LE_PA_SYNC_ADV_LIST_OP_CLEAR  = 0x00,    /*!< Clear all entries from Periodic Advertiser list. */
	RTK_BT_LE_PA_SYNC_ADV_LIST_OP_ADD    = 0x01,    /*!< Add an entry to Periodic Advertiser list. */
	RTK_BT_LE_PA_SYNC_ADV_LIST_OP_REMOVE = 0x02     /*!< Remove one entry from Periodic Advertiser list. */
} rtk_bt_le_pa_sync_advlist_op_t;

/**
 * @struct    rtk_bt_le_pa_sync_modify_advlist_t
 * @brief     Bluetooth LE periodic adv synchronization modify advertiser.
 */
typedef struct {
	rtk_bt_le_pa_sync_advlist_op_t operation;    /*!< List operation. */
	rtk_bt_le_addr_t adv_addr;                   /*!< Address of the advertiser to add or remove; ignored for CLEAR. */
	uint8_t adv_sid;                             /*!< Advertising SID subfield in the ADI field used to identify the Periodic Advertising,
                                                  *   ignored for CLEAR. */
} rtk_bt_le_pa_sync_modify_advlist_t;
#endif

#if defined(RTK_BLE_5_1_PAST_SENDER_SUPPORT) && RTK_BLE_5_1_PAST_SENDER_SUPPORT
/**
 * @struct    rtk_bt_le_past_send_t
 * @brief     Bluetooth LE periodic adv sync transfer parameter.
 */
typedef struct {
	uint16_t conn_handle;                        /*!< Connection handle of the ACL connection that the peer will receive the sync information.
                                                  *   Range: 0x0000 to 0x0EFF. */
	uint16_t service_data;                       /*!< Application-defined 16-bit value used by the peer to identify this periodic advertising train after transfer. */
	bool use_sync_id;                            /*!< Selects the meaning of the idx field.
                                                  *   - true: idx is a sync_id obtained from a remote device by sync procedure.
                                                  *   - false: idx is an adv_handle for a locally-generated periodic advertising set. */
	uint8_t idx;                                 /*!< Sync_id (if use_sync_id is true) or adv_handle (if use_sync_id is false). */
} rtk_bt_le_past_send_t;
#endif

#if defined(RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT) && RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT
/**
 * @typedef   rtk_bt_le_past_recv_mode_t
 * @brief     Bluetooth LE GAP PAST recipient mode.
 */
typedef enum {
	RTK_BT_LE_PAST_RECV_MODE_NO_ATTEMPT_TO_SYNCHRONIZE = 0x00,                      /*!< Default. No attempt to synchronize to the periodic advertising (default). */
	RTK_BT_LE_PAST_RECV_MODE_PA_ADV_REPORT_DISABLED = 0x01,                         /*!< Synchronize after PAST but do not forward payload reports to the host;
                                                                                     *   @ref RTK_BT_LE_GAP_EVT_PA_ADV_REPORT_IND is disabled. */
	RTK_BT_LE_PAST_RECV_MODE_PA_ADV_REPORT_ENABLED = 0x02,                          /*!< Synchronize after PAST and forward all payload reports to the host;
                                                                                     *   @ref RTK_BT_LE_GAP_EVT_PA_ADV_REPORT_IND is enabled with duplicate filtering disabled. */
	RTK_BT_LE_PAST_RECV_MODE_PA_ADV_REPORT_ENABLED_WITH_DUPLICATE_FILTER = 0x03,    /*!< Synchronize after PAST and forward deduplicated payload reports;
                                                                                     *   @ref RTK_BT_LE_GAP_EVT_PA_ADV_REPORT_IND is enabled with duplicate filtering enabled. */
} rtk_bt_le_past_recv_mode_t;

/**
 * @struct    rtk_bt_le_past_recv_param_t
 * @brief     Bluetooth LE periodic adv sync transfer receive parameter.
 */
typedef struct {
	uint16_t conn_handle;                   /*!< Connection handle of the ACL connection for which PAST is expected.
                                             *   Ignored when used with @ref rtk_bt_le_gap_default_past_recipient_set.
                                             *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_past_recv_mode_t mode;        /*!< Synchronization and reporting mode applied when PAST is received.
                                             *   Controls whether sync is attempted and whether PA reports are forwarded to the host. */
	uint8_t sync_cte_type;                  /*!< Bitmask of @ref rtk_bt_le_pa_sync_cte_t.
                                             *   0 means CTE type is irrelevant. */
	uint16_t skip;                          /*!< Maximum number of consecutive periodic advertising packets that can be skipped.
                                             *   Range: 0x0000 to 0x01F3. */
	uint16_t sync_timeout;                  /*!< Maximum permitted time between successful receives before synchronization is lost.
                                             *   Range: 0x000A to 0x4000.
                                             *   Time = N * 10 ms.
                                             *   Time Range: 100 ms to 163.84 s. */
} rtk_bt_le_past_recv_param_t;
#endif

/**
 * @struct    rtk_bt_le_scan_start_ind_t
 * @brief     Bluetooth LE scan start indication msg.
 */
typedef struct {
	uint16_t err;                           /*!< Status of the start scan procedure; 0x00 on success. */
	rtk_bt_le_scan_type_t scan_type;        /*!< Scan type. */
} rtk_bt_le_scan_start_ind_t;

/**
 * @enum      rtk_bt_le_adv_report_type_t
 * @brief     Bluetooth LE adv report event type.
 */
typedef enum {
	RTK_BT_LE_EVT_CONN_ADV       = 0x00,    /*!< ADV_IND         - connectable undirected advertising. */
	RTK_BT_LE_EVT_CONN_DIR_ADV   = 0x01,    /*!< ADV_DIRECT_IND  - connectable directed advertising. */
	RTK_BT_LE_EVT_SCAN_UNDIR_ADV = 0x02,    /*!< ADV_SCAN_IND    - scannable undirected advertising. */
	RTK_BT_LE_EVT_NON_CONN_ADV   = 0x03,    /*!< ADV_NONCONN_IND - non-connectable undirected advertising. */
	RTK_BT_LE_EVT_SCAN_RSP       = 0x04,    /*!< SCAN_RSP        - scan response to an active scan request. */
} rtk_bt_le_adv_report_type_t;

/**
 * @def       RTK_BT_LE_MAX_ADV_DATA_LEN
 * @brief     Bluetooth LE max adv data len.
 */
#define RTK_BT_LE_MAX_ADV_DATA_LEN  31
#define RTK_BT_LE_MAX_EXT_ADV_DATA_LEN  1650

/**
 * @struct    rtk_bt_le_adv_report_t
 * @brief     Bluetooth LE adv report data.
 */
typedef struct {
	rtk_bt_le_adv_report_type_t evt_type;        /*!< Advertising PDU type. */
	rtk_bt_le_addr_t addr;                       /*!< Address of the advertising device. */
	uint8_t len;                                 /*!< Length of the advertising payload in data.
                                                  *   Range: 0 to 31. */
	uint8_t data[RTK_BT_LE_MAX_ADV_DATA_LEN];    /*!< Data field of the advertising report. */
	int8_t rssi;                                 /*!< RSSI of the received advertising packet.
                                                  *   Range: -127 to +20 dBm.
                                                  *   0x7F: RSSI is unavailable. */
} rtk_bt_le_adv_report_t;

/**
 * @struct    rtk_bt_le_scan_res_ind_t
 * @brief     Bluetooth LE scan result indication msg.
 */
typedef struct {
	uint8_t num_report;                          /*!< Number of advertising reports in this event; always 1. */
	rtk_bt_le_adv_report_t adv_report;           /*!< The advertising report delivered from the controller. */
} rtk_bt_le_scan_res_ind_t;

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV

#define RTK_BT_LE_EXT_ADV_EVT_BIT_CONNECTABLE_ADV   (1 << 0)
#define RTK_BT_LE_EXT_ADV_EVT_BIT_SCANNABLE_ADV     (1 << 1)
#define RTK_BT_LE_EXT_ADV_EVT_BIT_DIRECTED_ADV      (1 << 2)
#define RTK_BT_LE_EXT_ADV_EVT_BIT_SCAN_RESPONSE     (1 << 3)

/**
 * @typedef   rtk_bt_le_ext_adv_report_type_t
 * @brief     Bluetooth LE ext adv report event type.
 */
typedef enum {
	RTK_BT_LE_EXT_EVT_LEGACY_ADV_IND =                  0x13,    /*!< Legacy ADV_IND PDU (connectable, scannable, undirected). */
	RTK_BT_LE_EXT_EVT_LEGACY_ADV_DIRECT_IND =           0x15,    /*!< Legacy ADV_DIRECT_IND PDU (connectable, directed). */
	RTK_BT_LE_EXT_EVT_LEGACY_ADV_SCAN_IND =             0x12,    /*!< Legacy ADV_SCAN_IND PDU (scannable, undirected). */
	RTK_BT_LE_EXT_EVT_LEGACY_ADV_NONCONN_IND =          0x10,    /*!< Legacy ADV_NONCONN_IND PDU (non-connectable, non-scannable). */
	RTK_BT_LE_EXT_EVT_LEGACY_SCAN_RSP_TO_ADV_IND =      0x1B,    /*!< Scan response to ADV_IND (connectable, scannable, scan response). */
	RTK_BT_LE_EXT_EVT_LEGACY_SCAN_RSP_TO_ADV_SCAN_IND = 0x1A,    /*!< Scan response to ADV_SCAN_IND (scannable, scan response). */
} rtk_bt_le_ext_adv_report_type_t;

/**
 * @typedef   rtk_bt_le_ext_adv_data_status_t
 * @brief     Definition of data status in LE Extended Advertising Report.
 */
typedef enum {
	RTK_BT_LE_EXT_ADV_DATA_STATUS_COMPLETE  = 0x00,              /*!< Advertising data is complete; the entire payload is present. */
	RTK_BT_LE_EXT_ADV_DATA_STATUS_MORE      = 0x01,              /*!< Advertising data is incomplete; more fragments will follow in subsequent reports. */
	RTK_BT_LE_EXT_ADV_DATA_STATUS_TRUNCATED = 0x02,              /*!< Advertising data was truncated by the controller; no further fragments will arrive. */
	RTK_BT_LE_EXT_ADV_DATA_STATUS_RFU       = 0x03               /*!< Reserved for future use. */
} rtk_bt_le_ext_adv_data_status_t;

/**
 * @struct    rtk_bt_le_ext_scan_res_ind_t
 * @brief     Bluetooth LE ext scan result indication msg.
 */
typedef struct {
	uint16_t evt_type;                           /*!< Advertising event type; use RTK_BT_LE_EXT_ADV_EVT_BIT_* macros for bit-field decode or
                                                  *   compare against @ref rtk_bt_le_ext_adv_report_type_t named constants for legacy PDUs. */
	rtk_bt_le_addr_t addr;                       /*!< Address of the advertising device. */
	rtk_bt_le_addr_t direct_addr;                /*!< Target address for directed advertising; Ignored for undirected advertising reports. */
	int8_t rssi;                                 /*!< RSSI of the received packet.
                                                  *   Range: -127 to +20 dBm.
                                                  *   0x7F: RSSI is not available. */
	uint8_t primary_phy;                         /*!< PHY used for the primary advertising channel.
                                                  *   0x01: LE 1M PHY.
                                                  *   0x03: LE Coded PHY.
                                                  *   All other values reserved. */
	uint8_t secondary_phy;                       /*!< PHY used for the secondary advertising channel.
                                                  *   0x00: No packets on the secondary advertising physical channel.
                                                  *   0x01: LE 1M PHY.
                                                  *   0x02: LE 2M PHY.
                                                  *   0x03: LE Coded PHY. */
	uint8_t adv_sid;                             /*!< Advertising SID subfield value from the ADI field of the PDU.
                                                  *   0x00 to 0x0F: Value of the Advertising SID subfield read from the ADI field of the PDU or, for scan responses, from the ADI field of the original
                                                  *   scannable advertisement.
                                                  *   0xFF: No ADI field is present.
                                                  *   All other values are reserved for future use. */
	int8_t tx_power;                             /*!< TX power reported in the advertising PDU.
                                                  *   Range: -127 to +20 dBm.
                                                  *   0x7F: TX power information is unavailable. */
	uint16_t peri_adv_interval;                  /*!< Periodic advertising interval.
                                                  *   0x0000: no associated periodic advertising.
                                                  *   Otherwise:
                                                  *   Range: 0x0006 to 0xFFFF.
                                                  *   Time = N * 1.25 ms.
                                                  *   Time Range: 7.5 ms to 81.91875 s. */
	uint8_t data_status;                         /*!< Status of the advertising data; see @ref rtk_bt_le_ext_adv_data_status_t. */
	uint16_t len;                                /*!< Length of the advertising payload in data.
                                                  *   Range: 0 to 229. */
	uint8_t *data;                               /*!< Pointer to the advertising or scan response data.
                                                  *   Must remain the last member of this struct. */
} rtk_bt_le_ext_scan_res_ind_t;
#endif

/**
 * @enum      rtk_bt_le_scan_stop_reason_t
 * @brief     Bluetooth LE scan stop reason.
 */
typedef enum {
	RTK_BT_LE_SCAN_STOP_BY_HOST      = 0,        /*!< Scan was stopped by an explicit host request. */
	RTK_BT_LE_SCAN_STOP_BY_DURATION  = 1,        /*!< Scan was stopped because the configured scan duration timer expired. */
	RTK_BT_LE_SCAN_STOP_UNKNOWN,                 /*!< Scan stopped for an unrecognized reason. */
} rtk_bt_le_scan_stop_reason_t;

/**
 * @struct    rtk_bt_le_scan_stop_ind_t
 * @brief     Bluetooth LE scan stop indication msg.
 */
typedef struct {
	uint16_t err;                                /*!< Status of the stop scan procedure; 0x00 on success. */
	rtk_bt_le_scan_stop_reason_t stop_reason;    /*!< Reason scanning was stopped. */
} rtk_bt_le_scan_stop_ind_t;

/**
 * @struct    rtk_bt_le_conn_ind_t
 * @brief     Bluetooth LE connection complete event msg.
 */
typedef struct {
	uint16_t err;                  /*!< Status of the connection complete procedure; 0x00 on success. */
	rtk_bt_le_link_role_t role;    /*!< Local device's role in this connection. */
	uint16_t conn_handle;          /*!< Connection handle of the established ACL connection.
                                    *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_addr_t peer_addr;    /*!< Address of the remote device. */
	uint16_t conn_interval;        /*!< Negotiated connection interval.
                                    *   Range: 0x0006 to 0x0C80.
                                    *   Time = N * 1.25 ms.
                                    *   Time Range: 7.5 ms to 4 s. */
	uint16_t conn_latency;         /*!< Peripheral latency for the connection in number of connection events.
                                    *   Range: 0x0000 to 0x01F3. */
	uint16_t supv_timeout;         /*!< Connection supervision timeout; connection is considered lost if no packet is received within this time.
                                    *   Range: 0x000A to 0x0C80.
                                    *   Time = N * 10 ms.
                                    *   Time Range: 100 ms to 32 s. */
} rtk_bt_le_conn_ind_t;

/**
 * @struct    rtk_bt_le_disconn_ind_t
 * @brief     Bluetooth LE disconnect complete event msg.
 */
typedef struct {
	uint16_t reason;               /*!< Status of the disconnect complete procedure; 0x00 on success. */
	rtk_bt_le_link_role_t role;    /*!< Local device's role in the connection that was disconnected. */
	uint16_t conn_handle;          /*!< Connection handle of the disconnected ACL connection.
                                    *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_addr_t peer_addr;    /*!< Address of the remote device. */
} rtk_bt_le_disconn_ind_t;

/**
 * @struct    rtk_bt_le_conn_update_ind_t
 * @brief     Bluetooth LE connection parameters updated event msg.
 */
typedef struct {
	uint16_t err;                  /*!< Status of the Connection Update procedure; 0x00 on success. */
	uint16_t conn_handle;          /*!< Connection handle of the updated ACL connection.
                                    *   Range: 0x0000 to 0x0EFF. */
	uint16_t conn_interval;        /*!< New connection interval.
                                    *   Range: 0x0006 to 0x0C80.
                                    *   Time = N * 1.25 ms.
                                    *   Time Range: 7.5 ms to 4 s. */
	uint16_t conn_latency;         /*!< New peripheral latency for the connection in number of connection events.
                                    *   Range: 0x0000 to 0x01F3. */
	uint16_t supv_timeout;         /*!< New supervision timeout.
                                    *   Range: 0x000A to 0x0C80.
                                    *   Time = N * 10 ms.
                                    *   Time Range: 100 ms to 32 s. */
} rtk_bt_le_conn_update_ind_t;

/**
 * @struct    rtk_bt_le_remote_conn_update_req_ind_t
 * @brief     Bluetooth LE remote device connection parameters change request event msg.
 */
typedef struct {
	uint16_t conn_handle;          /*!< Connection handle of the ACL connection for which the connection parameter update request was received.
                                    *   Range: 0x0000 to 0x0EFF. */
	uint16_t conn_interval_min;    /*!< Minimum acceptable connection interval requested by remote device.
                                    *   Range: 0x0006 to 0x0C80.
                                    *   Time = N * 1.25 ms.
                                    *   Time Range: 7.5 ms to 4 s. */
	uint16_t conn_interval_max;    /*!< Maximum acceptable connection interval requested by remote device.
                                    *   Range: 0x0006 to 0x0C80.
                                    *   Time = N * 1.25 ms.
                                    *   Time Range: 7.5 ms to 4 s. */
	uint16_t conn_latency;         /*!< Maximum allowed Peripheral latency for the connection requested by the remote device.
                                    *   Range: 0x0000 to 0x01F3. */
	uint16_t supv_timeout;         /*!< Supervision timeout for the connection requested by the remote device.
                                    *   Range: 0x000A to 0x0C80.
                                    *   Time = N * 10 ms.
                                    *   Time Range: 100 ms to 32 s. */
} rtk_bt_le_remote_conn_update_req_ind_t;

/**
 * @struct    rtk_bt_le_auth_pair_cfm_ind_t
 * @brief     Bluetooth LE Just Works pairing need to confirm event msg.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection currently being paired.
                              *   Range: 0x0000 to 0x0EFF. */
} rtk_bt_le_auth_pair_cfm_ind_t;

/**
 * @struct    rtk_bt_le_auth_key_display_ind_t
 * @brief     Bluetooth LE pairing key display event msg.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection currently being paired.
                              *   Range: 0x0000 to 0x0EFF. */
	uint32_t passkey;        /*!< Six-digit passkey value (000000-999999) to display to the user. */
} rtk_bt_le_auth_key_display_ind_t;

/**
 * @struct    rtk_bt_le_auth_key_input_ind_t
 * @brief     Bluetooth LE pairing key input event msg.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection currently being paired.
                              *   Range: 0x0000 to 0x0EFF. */
} rtk_bt_le_auth_key_input_ind_t;

/**
 * @struct    rtk_bt_le_auth_key_cfm_ind_t
 * @brief     Bluetooth LE pairing key comparison and confirmation event msg.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection currently being paired.
                              *   Range: 0x0000 to 0x0EFF. */
	uint32_t passkey;        /*!< Six-digit passkey value (000000-999999) to display and confirm with the user. */
} rtk_bt_le_auth_key_cfm_ind_t;

/**
 * @struct    rtk_bt_le_auth_oob_input_ind_t
 * @brief     Bluetooth LE pairing oob key need to input event msg.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection currently being paired.
                              *   Range: 0x0000 to 0x0EFF. */
} rtk_bt_le_auth_oob_input_ind_t;

/**
 * @struct    rtk_bt_le_sec_level_t
 * @brief     Bluetooth LE security level.
 */
typedef enum {
	RTK_BT_LE_SEC_LEVEL_NONE          = 0x01,    /*!< Security Mode 1 Level 1: no security, no authentication and no encryption. */
	RTK_BT_LE_SEC_LEVEL_UNAUTHEN      = 0x02,    /*!< Security Mode 1 Level 2: unauthenticated pairing with encryption. */
	RTK_BT_LE_SEC_LEVEL_AUTHEN        = 0x03,    /*!< Security Mode 1 Level 3: authenticated pairing with encryption. */
	RTK_BT_LE_SEC_LEVEL_SC_AUTHEN_128 = 0x04,    /*!< Security Mode 1 Level 4: authenticated LE Secure Connections pairing with 128-bit encryption key. */
} rtk_bt_le_sec_level_t;

/**
 * @struct    rtk_bt_le_auth_complete_ind_t
 * @brief     Bluetooth LE auth completed event msg.
 */
typedef struct {
	uint16_t err;                                /*!< Status of the authentication procedure; 0x00 on success. */
	uint16_t conn_handle;                        /*!< Connection handle of the ACL connection associated with this authentication result.
                                                  *   Range: 0x0000 to 0x0EFF. */
	uint8_t dev_ltk_length;                      /*!< Device long term key length.*/
	uint8_t dev_ltk[32];                         /*!< Device long term key. */
	rtk_bt_le_sec_level_t sec_level;             /*!< Bluetooth LE security level of the connection. */
} rtk_bt_le_auth_complete_ind_t;

#if defined(RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT) && RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT
/**
 * @struct    rtk_bt_le_data_len_change_ind_t
 * @brief     Bluetooth LE data length changed event msg.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection for which data length parameters changed.
                              *   Range: 0x0000 to 0x0EFF. */
	uint16_t max_tx_octets;  /*!< Maximum number of payload octets in an LL Data PDU that the local Controller will send on this connection.
                              *   Units: octets.
                              *   Range: 0x001B to 0x00FB. */
	uint16_t max_tx_time;    /*!< Maximum time that the local Controller will take to send a Link Layer packet
                              *   containing an LL Data PDU on this connection.
                              *   Units: µs.
                              *   Range: 0x0148 to 0x4290. */
	uint16_t max_rx_octets;  /*!< Maximum number of payload octets in a Link Layer packet that the local Controller
                              *   expects to receive on this connection.
                              *   Units: octets.
                              *   Range: 0x001B to 0x00FB. */
	uint16_t max_rx_time;    /*!< Maximum time that the local Controller expects to take to receive a Link Layer packet
                              *   on this connection.
                              *   Units: µs.
                              *   Range: 0x0148 to 0x4290. */
} rtk_bt_le_data_len_change_ind_t;
#endif

#if defined(RTK_BLE_5_0_SET_PHYS_SUPPORT) && RTK_BLE_5_0_SET_PHYS_SUPPORT
/**
 * @struct    rtk_bt_le_phy_update_ind_t
 * @brief     Bluetooth LE PHY updated event msg.
 */
typedef struct {
	uint16_t err;            /*!< Status of the PHY update procedure; 0x00 on success. */
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection for which the PHY was updated.
                              *   Range: 0x0000 to 0x0EFF. */
	uint8_t tx_phy;          /*!< Transmitter PHY in use for the connection.
                              *   0x01: LE 1M.
                              *   0x02: LE 2M.
                              *   0x03: LE Coded.
                              *   All other values: Reserved for future use. */
	uint8_t rx_phy;          /*!< Receiver PHY in use for the connection.
                              *   0x01: LE 1M.
                              *   0x02: LE 2M.
                              *   0x03: LE Coded.
                              *   All other values: Reserved for future use. */
} rtk_bt_le_phy_update_ind_t;
#endif

/**
 * @struct    rtk_bt_le_read_remote_version_ind_t
 * @brief     Bluetooth LE read remote version event msg.
 */
typedef struct {
	uint16_t err;            /*!< Status of the remote version information procedure; 0x00 on success. */
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection for which the remote version information was obtained.
                              *   Range: 0x0000 to 0x0EFF. */
	uint8_t version;         /*!< Version of the current LMP or Link Layer supported by the remote Controller. */
	uint16_t company_id;     /*!< Bluetooth SIG-assigned company identifier of the remote controller manufacturer. */
	uint16_t subversion;     /*!< Revision of the LMP or Link Layer implementation in the remote Controller. This value is vendor-specific. */
} rtk_bt_le_read_remote_version_ind_t;

/**
 * @struct    rtk_bt_le_wl_modify_ind_t
 * @brief     Bluetooth LE whitelist modified event msg.
 */
typedef struct {
	rtk_bt_le_wl_op_t op;    /*!< Whitelist operation that completed. */
	uint16_t err;            /*!< Status of modifying the Filter Accept List procedure; 0x00 on success. */
} rtk_bt_le_wl_modify_ind_t;

/**
 * @enum      rtk_bt_le_bond_op_t
 * @brief     Bluetooth LE bond operation definition.
 */
typedef enum {
	RTK_BT_LE_BOND_DELETE,                /*!< A previously bonded device was removed from the bond list. */
	RTK_BT_LE_BOND_ADD,                   /*!< A newly paired device was added to the bond list. */
	RTK_BT_LE_BOND_CLEAR,                 /*!< All bonded devices were cleared from the bond list. */
	RTK_BT_LE_BOND_FULL,                  /*!< The bond list storage is full. */
	RTK_BT_LE_BOND_KEY_MISSING,           /*!< The LTK for a previously bonded peer is missing. */
	RTK_BT_LE_BOND_RESERVED,              /*!< Reserved. */
} rtk_bt_le_bond_op_t;

/**
 * @struct    rtk_bt_le_bond_modify_ind_t
 * @brief     Bluetooth LE bond modified event msg.
 */
typedef struct {
	rtk_bt_le_bond_op_t op;               /*!< Bond modify operation. */
	rtk_bt_le_addr_t remote_addr;         /*!< Bonded device address. */
	rtk_bt_le_ident_addr_t ident_addr;    /*!< Identity address. */
} rtk_bt_le_bond_modify_ind_t;

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
/**
 * @struct    rtk_bt_le_modify_resolv_list_ind_t
 * @brief     Bluetooth LE resolving list modified indication.
 */
typedef struct {
	rtk_bt_le_resolv_list_op_t op;                   /*!< Resolving list operation that completed. */
	rtk_bt_le_resolv_list_entry_t entry;             /*!< Resolving list entry that was added, removed, or updated. */
	uint16_t err;                                    /*!< Status of the modify Resolving List procedure; 0x00 on success. */
} rtk_bt_le_modify_resolv_list_ind_t;

/**
 * @enum      rtk_bt_le_resolv_list_pending_reason_t
 * @brief     Bluetooth LE resolving list modification pending reason definition.
 */
typedef enum {
	RTK_BT_LE_RESOLV_LIST_PEND_BY_ADV     = 0x01,    /*!< Resolving list modification is pended because device is advertising. */
	RTK_BT_LE_RESOLV_LIST_PEND_BY_SCAN    = 0x02,    /*!< Resolving list modification is pended because device is scanning. */
	RTK_BT_LE_RESOLV_LIST_PEND_BY_CONNECT = 0x04     /*!< Resolving list modification is pended because device is connecting. */
} rtk_bt_le_resolv_list_pending_reason_t;

/**
 * @struct    rtk_bt_le_resolv_list_pending_ind_t
 * @brief     Bluetooth LE resolving list modification pending indication.
 */
typedef struct {
	uint8_t reason;                               /*!< Bits combination of @ref rtk_bt_le_resolv_list_pending_reason_t. */
} rtk_bt_le_resolv_list_pending_ind_t;

/**
 * @enum      rtk_bt_le_privacy_mode_t
 * @brief     Bluetooth LE privacy mode type.
 */
typedef enum {
	RTK_BT_LE_NETWORK_PRIVACY_MODE = 0x00,        /*!< Network privacy mode. */
	RTK_BT_LE_DEVICE_PRIVACY_MODE  = 0x01,        /*!< Device privacy mode. */
} rtk_bt_le_privacy_mode_t;
#endif

#if defined(RTK_BLE_5_2_POWER_CONTROL_SUPPORT) && RTK_BLE_5_2_POWER_CONTROL_SUPPORT
/**
 * @def       RTK_BT_LE_TXPOWER_LEVEL
 * @brief     Bluetooth LE tx power level.
 */
#define RTK_BLE_LE_TXPOWER_UNAVAILABLE      0x7F
#define RTK_BLE_LE_TXPOWER_NOT_MANAGING     0x7E

/**
 * @enum      rtk_bt_le_txpower_phy_t
 * @brief     Bluetooth LE phy definition in tx power report.
 */
typedef enum {
	RTK_BT_LE_TXPOWER_PHYS_1M          = 0x01,        /*!< LE 1M PHY. */
	RTK_BT_LE_TXPOWER_PHYS_2M          = 0x02,        /*!< LE 2M PHY. */
	RTK_BT_LE_TXPOWER_PHYS_CODED_S8    = 0x03,        /*!< LE Coded PHY with S=8 data coding. */
	RTK_BT_LE_TXPOWER_PHYS_CODED_S2    = 0x04,        /*!< LE Coded PHY with S=2 data coding. */
} rtk_bt_le_txpower_phy_t;

/**
 * @enum      rtk_bt_le_txpower_flag_t
 * @brief     Bluetooth LE TX power level flag Bits.
 */
typedef enum {
	RTK_BT_LE_TXPOWER_FLAG_MIN_BIT         = 0x01,    /*!< Transmit power level is at minimum level. */
	RTK_BT_LE_TXPOWER_FLAG_MAX_BIT         = 0x02,    /*!< Transmit power level is at maximum level. */
} rtk_bt_le_txpower_flag_t;

/**
 * @struct    rtk_bt_le_txpower_read_local_t
 * @brief     Bluetooth LE read local TX power parameter definition.
 */
typedef struct {
	uint16_t conn_handle;                             /*!< Connection handle of the ACL connection for which the transmit power levels are read.
                                                       *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_txpower_phy_t phy;                      /*!< PHY for which to read the transmit power levels of the local Controller. */
	int8_t *cur_txpower;                              /*!< Output: current transmit power level of the local Controller on this PHY.
                                                       *   Range: -127 to +20 dBm.
                                                       *   0x7F: Power level is unavailable. */
	int8_t *max_txpower;                              /*!< Output: maximum transmit power level of the local Controller on this PHY.
                                                       *   Range: -127 to +20 dBm. */
} rtk_bt_le_txpower_read_local_t;

/**
 * @struct    rtk_bt_le_txpower_read_remote_t
 * @brief     Bluetooth LE read remote TX power parameter definition.
 */
typedef struct {
	uint16_t conn_handle;           /*!< Connection handle of the ACL connection for which the transmit power levels are read.
                                     *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_txpower_phy_t phy;    /*!< PHY for which to read the transmit power levels of the remote Controller. */
} rtk_bt_le_txpower_read_remote_t;

/**
 * @struct    rtk_bt_le_txpower_report_set_t
 * @brief     Bluetooth LE read TX power parameter indication.
 */
typedef struct {
	uint16_t conn_handle;           /*!< Connection handle of the ACL connection for which transmit power reporting is configured.
                                     *   Range: 0x0000 to 0x0EFF. */
	bool local_enable;              /*!< Controls reporting of local transmit power level changes.
                                     *   - true: enable local transmit power reports.
                                     *   - false: disable local transmit power reports. */
	bool remote_enable;             /*!< Controls reporting of remote transmit power level changes.
                                     *   - true: enable remote transmit power reports.
                                     *   - false: disable remote transmit power reports. */
} rtk_bt_le_txpower_report_set_t;

/**
 * @enum      rtk_bt_le_txpower_report_type_t
 * @brief     Bluetooth LE TX power report type definition.
 */
typedef enum {
	RTK_BT_LE_TXPOWER_REPORT_LOCAL_CHANGED         = 0x00,    /*!< Local transmit power changed. */
	RTK_BT_LE_TXPOWER_REPORT_REMOTE_CHANGED        = 0x01,    /*!< Remote transmit power changed. */
	RTK_BT_LE_TXPOWER_REPORT_REMOTE_READ_RESPONSE  = 0x02,    /*!< Response of read remote transmit power. */
} rtk_bt_le_txpower_report_type_t;

/**
 * @struct    rtk_bt_le_txpower_ind_t
 * @brief     Bluetooth LE TX power report indication.
 */
typedef struct {
	uint16_t conn_handle;                                     /*!< Connection handle of the ACL connection for which the transmit power level is reported.
                                                               *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_txpower_report_type_t type;                     /*!< Transmit power report type. */
	rtk_bt_le_txpower_phy_t phy;                              /*!< PHY to which this transmit power report applies. */
	int8_t txpower;                                           /*!< Reported transmit power level.
                                                               *   Range: -127 to +20 dBm.
                                                               *   0x7E (@ref RTK_BLE_LE_TXPOWER_NOT_MANAGING): remote device is not managing power levels on this PHY.
                                                               *   0x7F (@ref RTK_BLE_LE_TXPOWER_UNAVAILABLE): transmit power level is not available. */
	uint8_t flag;                                             /*!< Boundary flags; bitmask of @ref rtk_bt_le_txpower_flag_t. */
	int8_t delta;                                             /*!< Change in transmit power level since the last report.
                                                               *   Units: dB.
                                                               *   Positive: power increased.
                                                               *   Negative: power decreased.
                                                               *   0: unchanged. */
} rtk_bt_le_txpower_ind_t;
#endif

#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
/**
 * @typedef   rtk_bt_le_coc_security_mode_t
 * @brief     Bluetooth LE COC security mode.
 */
typedef enum {
	RTK_BT_LE_COC_SEC_NONE,                    /*!< Security None. */
	RTK_BT_LE_COC_SEC_UNAUTHEN_ENCRYPT,        /*!< Security unauthenticated encryption. */
	RTK_BT_LE_COC_SEC_AUTHEN_ENCRYPT,          /*!< Security authenticated encryption. */
	RTK_BT_LE_COC_SEC_UNAUTHEN_DATA_SIGN,      /*!< Security unauthenticated data signed. */
	RTK_BT_LE_COC_SEC_AUTHEN_DATA_SIGN,        /*!< Security authenticated data signed. */
	RTK_BT_LE_COC_SEC_AUTHOR,                  /*!< Security authorized. */
	RTK_BT_LE_COC_SEC_SECURE_CONN_UNAUTHEN,    /*!< Security unauthenticated LE secure connection. */
	RTK_BT_LE_COC_SEC_SECURE_CONN_AUTHEN,      /*!< Security LE secure connection. */
} rtk_bt_le_coc_security_mode_t;

/**
 * @typedef   rtk_bt_le_coc_param_type_t
 * @brief     Bluetooth LE COC global parameter type.
 */
typedef enum {
	RTK_BT_LE_COC_PARAM_LOCAL_MTU = 0x1,    /*!< Local Maximum Transmission Unit (MTU) for all COC channels. */
} rtk_bt_le_coc_param_type_t;

/**
 * @typedef   rtk_bt_le_coc_chan_param_type_t
 * @brief     Bluetooth LE COC per-channel parameter type.
 */
typedef enum {
	RTK_BT_LE_COC_CHAN_PARAM_CUR_CREDITS = 0x0,    /*!< Current number of credits available on the channel (indicates how many
                                                    *   more credit-based frames the peer may send). */
	RTK_BT_LE_COC_CHAN_PARAM_MAX_CREDITS = 0x1,    /*!< Maximum number of credits configured for the channel. */
	RTK_BT_LE_COC_CHAN_PARAM_MTU         = 0x2,    /*!< Negotiated MTU (Maximum Transmission Unit) for the channel in bytes. */
} rtk_bt_le_coc_chan_param_type_t;

/**
 * @struct    rtk_bt_le_coc_conn_state_ind_t
 * @brief     Bluetooth LE COC connection state indication.
 */
typedef struct {
	uint16_t conn_handle;                          /*!< Connection handle of the ACL connection which COC connection established.
                                                    *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;                                  /*!< Local L2cap channel ID assigned by bt stack. */
	uint16_t err;                                  /*!< Status of the L2cap channel connect or disconnect procedure. 0x00 on success. */
} rtk_bt_le_coc_conn_state_ind_t;

/**
 * @struct    rtk_bt_le_coc_send_data_res_ind_t
 * @brief     Bluetooth LE COC send data indication.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection to send coc data.
                              *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;            /*!< Local L2cap channel ID. */
	uint16_t err;            /*!< Status of the L2cap channel send data procedure. 0x00 on success. */
	uint8_t credit;          /*!< Remain credits number after send data. */
} rtk_bt_le_coc_send_data_res_ind_t;

/**
 * @struct    rtk_bt_le_coc_receive_data_ind_t
 * @brief     Bluetooth LE COC receive data indication.
 */
typedef struct {
	uint16_t conn_handle;               /*!< Connection handle of the ACL connection to receive coc data.
                                         *   Range: 0x0000 to 0x0EFF. */
	uint16_t cid;                       /*!< Local L2cap channel ID. */
	uint16_t len;                       /*!< Length of received data. */
	uint8_t *data;                      /*!< Received data from remote. */
} rtk_bt_le_coc_receive_data_ind_t;
#endif /* RTK_BLE_COC_SUPPORT */

/****************** Data structure for API func param pack and unpack ******************/
/**
 * @struct    rtk_bt_le_gen_rand_addr_t
 * @brief     Bluetooth LE generate random address parameter definition.
 */
typedef struct {
	rtk_bt_le_rand_addr_type_t type;    /*!< Type of random address to generate. */
	uint8_t *p_addr;                    /*!< Output: generated 6-byte address. */
} rtk_bt_le_gen_rand_addr_t;

/**
 * @struct    rtk_bt_le_read_rssi_param_t
 * @brief     Bluetooth LE read rssi parameter definition.
 */
typedef struct {
	uint16_t conn_handle;               /*!< Connection handle of the ACL connection to read RSSI.
                                         *   Range: 0x0000 to 0x0EFF. */
	int8_t *p_rssi;                     /*!< Output: RSSI value. */
} rtk_bt_le_read_rssi_param_t;

/**
 * @struct    rtk_bt_le_disconn_with_reason_param_t
 * @brief     Bluetooth LE disconnect with reason parameter definition.
 */
typedef struct {
	uint16_t conn_handle;                  /*!< Connection handle of the ACL connection to disconnect.
                                            *   Range: 0x0000 to 0x0EFF. */
	uint8_t reason;                        /*!< Disconnection reason code reported to the remote device. */
} rtk_bt_le_disconn_with_reason_param_t;

/**
 * @struct    rtk_bt_le_get_conn_info_param_t
 * @brief     Bluetooth LE get connection infomation parameter definition.
 */
typedef struct {
	uint16_t conn_handle;                  /*!< Connection handle of the ACL connection to get connection information.
                                            *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_conn_info_t *p_conn_info;    /*!< Output: connection parameters of the specified connection. */
} rtk_bt_le_get_conn_info_param_t;

/**
 * @struct    rtk_bt_le_get_conn_handle_by_addr_param_t
 * @brief     Bluetooth LE get connection handle by address parameter definition.
 */
typedef struct {
	rtk_bt_le_addr_t *p_addr;              /*!< BLE address of the peer device to look up. */
	uint16_t *p_conn_handle;               /*!< Output: the ACL connection handle of the matching active connection. */
} rtk_bt_le_get_conn_handle_by_addr_param_t;

/**
 * @struct    rtk_bt_le_get_mtu_size_param_t
 * @brief     Bluetooth LE get mtu size parameter definition.
 */
typedef struct {
	uint16_t conn_handle;                  /*!< Connection handle of the ACL connection to get MTU size.
                                            *   Range: 0x0000 to 0x0EFF. */
	uint16_t *p_mtu_size;                  /*!< Output: negotiated ATT MTU value. */
} rtk_bt_le_get_mtu_size_param_t;

/**
 * @struct    rtk_bt_le_get_bond_info_param_t
 * @brief     Bluetooth LE get bond information parameter definition.
 */
typedef struct {
	rtk_bt_le_bond_info_t *p_bond_info;    /*!< Pointer to the bond info array to fill. */
	uint8_t *p_size;                       /*!< On entry, the number of slots available in p_bond_info.
                                            *   On return, updated to the total number of bonded devices stored in the stack. */
	uint8_t size;                          /*!< Number of slots available in p_bond_info. */
} rtk_bt_le_get_bond_info_param_t;

/**
 * @struct    rtk_bt_le_get_tx_pending_num_param_t
 * @brief     Bluetooth LE get tx pending number parameter definition.
 */
typedef struct {
	uint16_t conn_handle;                  /*!< Connection handle of the ACL connection to query.
                                            *   Range: 0x0000 to 0x0EFF. */
	uint16_t *p_tx_pending_num;            /*!< Output: the number of TX packets currently pending for this connection. */
} rtk_bt_le_get_tx_pending_num_param_t;

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
/**
 * @struct    rtk_bt_le_get_eadv_by_conn_handle_param_t
 * @brief     Bluetooth LE get extended advertising handle by connection handle parameter definition.
 */
typedef struct {
	uint16_t conn_handle;                                /*!< Connection handle of the ACL connection to get the corresponding advertising set.
                                                          *   Range: 0x0000 to 0x0EFF. */
	uint8_t *adv_handle;                                 /*!< Output: the advertising set handle that stopped due to this connection. */
} rtk_bt_le_get_eadv_by_conn_handle_param_t;
#endif

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
/**
 * @struct    rtk_bt_le_read_local_rpa_param_t
 * @brief     Bluetooth LE read local RPA parameter definition.
 */
typedef struct {
	rtk_bt_le_ident_addr_type_t peer_ident_addr_type;    /*!< Peer identity address type. */
	uint8_t *peer_addr;                                  /*!< Peer identity address. */
	uint8_t *local_rpa;                                  /*!< Output: the local RPA. */
} rtk_bt_le_read_local_rpa_param_t;

/**
 * @struct    rtk_bt_le_read_peer_rpa_param_t
 * @brief     Bluetooth LE read peer's RPA parameter definition.
 */
typedef struct {
	rtk_bt_le_ident_addr_type_t peer_ident_addr_type;    /*!< Peer identity address type. */
	uint8_t *peer_addr;                                  /*!< Peer identity address. */
	uint8_t *peer_rpa;                                   /*!< Output: the peer's RPA. */
} rtk_bt_le_read_peer_rpa_param_t;

/**
 * @struct    rtk_bt_le_set_privacy_mode_param_t
 * @brief     Bluetooth LE set privacy mode parameter definition.
 */
typedef struct {
	rtk_bt_le_ident_addr_type_t peer_ident_addr_type;    /*!< Identity address type of the peer. */
	uint8_t *peer_addr;                                  /*!< 6-byte peer identity address. */
	rtk_bt_le_privacy_mode_t privacy_mode;               /*!< Privacy mode to apply. */
} rtk_bt_le_set_privacy_mode_param_t;
#endif

#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
/**
 * @struct    rtk_bt_le_coc_psm_reg_param_t
 * @brief     Bluetooth LE COC psm register parameter definition.
 */
typedef struct {
	uint8_t is_register;                                 /*!< 1: register the PSM.
                                                          *   0: unregister the PSM.
                                                          *   All bigger values are reserved. */
	uint16_t le_psm;                                     /*!< LE PSM value to register or unregister.
                                                          *   Valid range: 0x0001 to 0x00FF. */
} rtk_bt_le_coc_psm_reg_param_t;

/**
 * @struct    rtk_bt_le_coc_set_psm_sec_param_t
 * @brief     Bluetooth LE COC set psm security parameter definition.
 */
typedef struct {
	uint16_t le_psm;                           /*!< LE PSM value whose security is being configured.
                                                *   Must be a registered PSM. */
	uint8_t active;                            /*!< 1: Allocate the specified security entry.
                                                *   0: Release the specified security entry.
                                                *   All bigger values are reserved. */
	rtk_bt_le_coc_security_mode_t sec_mode;    /*!< COC security mode. */
	uint8_t key_size;                          /*!< Required encryption key size in bytes. */
} rtk_bt_le_coc_set_psm_sec_param_t;

/**
 * @struct    rtk_bt_le_coc_param_set_t
 * @brief     Bluetooth LE COC set parameter definition.
 */
typedef struct {
	rtk_bt_le_coc_param_type_t param_type;         /*!< Parameter type identifier. */
	uint16_t value;                                /*!< Parameter value. The valid range depends on the parameter type. */
} rtk_bt_le_coc_param_set_t;

/**
 * @struct    rtk_bt_le_coc_chan_param_get_t
 * @brief     Bluetooth LE get COC channel parameter definition.
 */
typedef struct {
	rtk_bt_le_coc_chan_param_type_t param_type;    /*!< Channel parameter type to get. */
	uint16_t cid;                                  /*!< COC channel ID of the established channel. */
	uint16_t *value;                               /*!< Output: the parameter value. */
} rtk_bt_le_coc_chan_param_get_t;

/**
 * @struct    rtk_bt_le_coc_connect_param_t
 * @brief     Bluetooth LE COC connect parameter definition.
 */
typedef struct {
	uint16_t conn_handle;                          /*!< Connection handle of the ACL connection to send COC connection request.
                                                    *   Range: 0x0000 to 0x0EFF. */
	uint16_t le_psm;                               /*!< LE PSM value to connect to on the remote device. */
} rtk_bt_le_coc_connect_param_t;

/**
 * @struct    rtk_bt_le_coc_send_data_param_t
 * @brief     Bluetooth LE COC send data parameter definition.
 */
typedef struct {
	uint16_t cid;                             /*!< COC channel ID of the channel to send data. */
	uint16_t len;                             /*!< Length of the data payload in bytes. */
	uint8_t *data;                            /*!< Pointer to the data to transmit. */
} rtk_bt_le_coc_send_data_param_t;
#endif /* RTK_BLE_COC_SUPPORT */
/***************************************************************************************/

/**
 * @typedef   rtk_bt_le_gap_cte_type_t
 * @brief     Bluetooth LE Constant Tone Extension (CTE) type.
 */
typedef enum {
	RTK_BT_LE_GAP_CTE_TYPE_NONE    = 0,       /*!< No CTE type; used where no CTE is present or none is allowed. */
	RTK_BT_LE_GAP_CTE_TYPE_AOA     = 0x01,    /*!< AoA Constant Tone Extension. */
	RTK_BT_LE_GAP_CTE_TYPE_AOD_1US = 0x02,    /*!< AoD Constant Tone Extension with 1us slots. */
	RTK_BT_LE_GAP_CTE_TYPE_AOD_2US = 0x04,    /*!< AoD Constant Tone Extension with 2us slots. */
} rtk_bt_le_gap_cte_type_t;


#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT

/* BLE connectionless CTE transmit parameters */
#define RTK_BLE_GAP_CTE_LEN_MIN                         (0x02u)
#define RTK_BLE_GAP_CTE_LEN_MAX                         (0x14u)
#define RTK_BLE_GAP_CTE_LEN_VALUE_IN_RANGE(value)       \
    (((value) >= RTK_BLE_GAP_CTE_LEN_MIN) && ((value) <= RTK_BLE_GAP_CTE_LEN_MAX))

#define RTK_BLE_GAP_CTE_TYPE_VALUE_IN_RANGE(value)      \
    ((value) == RTK_BT_LE_GAP_CTE_TYPE_AOA ||           \
     (value) == RTK_BT_LE_GAP_CTE_TYPE_AOD_1US ||       \
     (value) == RTK_BT_LE_GAP_CTE_TYPE_AOD_2US)

#define RTK_BLE_GAP_CTE_CNT_MIN                         (0x01u)
#define RTK_BLE_GAP_CTE_CNT_MAX                         (0x10u)
#define RTK_BLE_GAP_CTE_CNT_VALUE_IN_RANGE(value)       \
    (((value) >= RTK_BLE_GAP_CTE_CNT_MIN) && ((value) <= RTK_BLE_GAP_CTE_CNT_MAX))

#define RTK_BLE_GAP_CTE_NUM_ANT_IDS_MIN                 (0x02u)
#define RTK_BLE_GAP_CTE_NUM_ANT_IDS_MAX                 (0x0Cu)
#define RTK_BLE_GAP_CTE_NUM_ANT_IDS_VALUE_IN_RANGE(value)   \
    (((value) >= RTK_BLE_GAP_CTE_NUM_ANT_IDS_MIN) && ((value) <= RTK_BLE_GAP_CTE_NUM_ANT_IDS_MAX))

#define RTK_BLE_GAP_CTE_TX_ENABLE_VALUE_IN_RANGE(value) ((value) <= 0x01u)

/* BLE connectionless IQ sampling parameter */
#define RTK_BLE_GAP_CTE_RX_EANBLE_VALUE_IN_RANGE(value) ((value) <= 0x01u)

#define RTK_BLE_GAP_CTE_SLOT_DURATION_MIN               (0x01u)
#define RTK_BLE_GAP_CTE_SLOT_DURATION_MAX               (0x02u)
#define RTK_BLE_GAP_CTE_SLOT_DURATION_VALUE_IN_RANGE(value)     \
    (((value) >= RTK_BLE_GAP_CTE_SLOT_DURATION_MIN) && ((value) <= RTK_BLE_GAP_CTE_SLOT_DURATION_MAX))

#define RTK_BLE_GAP_CTE_MAX_SAMPLED_CTES_MIN            (0x00u)
#define RTK_BLE_GAP_CTE_MAX_SAMPLED_CTES_MAX            (0x10u)
#define RTK_BLE_GAP_CTE_MAX_SAMPLED_CTES_VALUE_IN_RANGE(value)      \
    ((value) <= RTK_BLE_GAP_CTE_MAX_SAMPLED_CTES_MAX)

#define RTK_BLE_GAP_CTE_TYPES_VALUE_IN_RANGE(value)         \
    ((value) <= (RTK_BT_LE_GAP_CTE_TYPE_AOA | RTK_BT_LE_GAP_CTE_TYPE_AOD_1US | RTK_BT_LE_GAP_CTE_TYPE_AOD_2US))

#define RTK_BLE_GAP_CTE_REQ_INTERVAL_MIN                (0x0000u)
#define RTK_BLE_GAP_CTE_REQ_INTERVAL_MAX                (0xFFFFu)
#define RTK_BLE_GAP_CTE_REQ_INTERVAL_VALUE_IN_RANGE(value)  \
    (((value) >= RTK_BLE_GAP_CTE_REQ_INTERVAL_MIN) && ((value) <= RTK_BLE_GAP_CTE_REQ_INTERVAL_MAX))

/**
 * @struct    rtk_bt_le_gap_connless_cte_tx_param_t
 * @brief     Bluetooth LE Connectionless CTE transmit parameters.
 */
typedef struct {
	uint16_t duration;                              /*!< CTE duration.
                                                     *   Time = N * 10 ms. */
	uint8_t  cte_len;                               /*!< Constant Tone Extension length in 8 µs units.
                                                     *   Range: 0x02 to 0x14.
                                                     *   All other values: Reserved for future use. */
	uint8_t  cte_type;                              /*!< Constant Tone Extension type @ref rtk_bt_le_gap_cte_type_t. */
	uint8_t  cte_count;                             /*!< The number of Constant Tone Extensions to transmit in each periodic advertising event.
                                                     *   Range: 0x01 to 0x10. */
	uint8_t  num_ant_ids;                           /*!< The number of Antenna IDs in the switching pattern.
                                                     *   For AoD: Range: 0x02 to RTK_BLE_GAP_CTE_NUM_ANT_IDS_MAX.
                                                     *   For AoA: ignored; its default value is 0. */
	uint8_t  *ant_ids;                              /*!< Antenna IDs in the switching pattern.
                                                     *   For AoA: ignored; its default value is NULL. */
} rtk_bt_le_gap_connless_cte_tx_param_t;

/**
 * @struct    rtk_bt_le_gap_connless_cte_tx_start_t
 * @brief     Combines an Advertising handle with its associated connectionless CTE transmit parameters, grouping the two inputs required to start connectionless
 *            CTE transmit.
 */
typedef struct {
	uint8_t adv_handle;                              /*!< Advertising handle identifying the advertising set. */
	rtk_bt_le_gap_connless_cte_tx_param_t   *param;  /*!< Connectionless CTE transmit parameters for the advertising set. */
} rtk_bt_le_gap_connless_cte_tx_start_t;

/**
 * @typedef   rtk_bt_le_gap_slot_durations_e
 * @brief     Bluetooth LE Antenna switching and IQ sampling slot duration for CTE operations.
 */
typedef enum {
	RTK_BT_LE_GAP_SLOT_DURATIONS_1US = 1,            /*!< Switching and sampling slots are 1 μs each. */
	RTK_BT_LE_GAP_SLOT_DURATIONS_2US = 2,            /*!< Switching and sampling slots are 2 μs each. */
} rtk_bt_le_gap_slot_durations_e;

/**
 * @struct    rtk_bt_le_gap_connless_cte_rx_param_t
 * @brief     Bluetooth LE Connectionless CTE reception parameters.
 */
typedef struct {
	uint8_t  slot_durations;                         /*!< Switching and sampling slots @ref rtk_bt_le_gap_slot_durations_e. */
	uint8_t  max_sampled_ctes;                       /*!< The maximum number of Constant Tone Extensions to sample and report in each periodic advertising interval.
                                                      *   0x00: Sample and report all available Constant Tone Extensions.
                                                      *   0x01-0x10: The maximum number of Constant Tone Extensions to sample and report. */
	uint8_t  num_ant_ids;                            /*!< The number of Antenna IDs in the switching pattern.
                                                      *   For AoA: Range: 0x02 to RTK_BLE_GAP_CTE_NUM_ANT_IDS_MAX.
                                                      *   For AoD: ignored; its default value is 0. */
	uint8_t  *ant_ids;                               /*!< Antenna IDs in the switching pattern.
                                                      *   For AoD: ignored; its default value is NULL. */
} rtk_bt_le_gap_connless_cte_rx_param_t;

/**
 * @struct    rtk_bt_le_gap_connless_cte_rx_start_t
 * @brief     Combines a periodic advertising sync ID with its associated connectionless CTE receive parameters, grouping the two inputs required to start connectionless
 *            CTE reception.
 */
typedef struct {
	uint8_t sync_id;                                 /*!< Periodic advertising sync id. */
	rtk_bt_le_gap_connless_cte_rx_param_t *param;    /*!< Connectionless CTE receive parameters. */
} rtk_bt_le_gap_connless_cte_rx_start_t;

/**
 * @struct    rtk_bt_le_gap_conn_cte_tx_param_t
 * @brief     Bluetooth LE Connection-oriented CTE transmit parameters.
 */
typedef struct {
	uint8_t  cte_types;                              /*!< Constant Tone Extension types: bit mask @ref rtk_bt_le_gap_cte_type_t. */
	uint8_t  num_ant_ids;                            /*!< The number of Antenna IDs in the switching pattern.
                                                      *   For AoD: Range: 0x02 to RTK_BLE_GAP_CTE_NUM_ANT_IDS_MAX.
                                                      *   For AoA: ignored; its default value is 0. */
	uint8_t  *ant_ids;                               /*!< Antenna IDs in the switching pattern.
                                                      *   For AoA: ignored; its default value is NULL. */
} rtk_bt_le_gap_conn_cte_tx_param_t;

/**
 * @struct    rtk_bt_le_gap_conn_cte_tx_start_t
 * @brief     Combines a connection handle with its associated connection-oriented CTE transmit parameters, grouping the two inputs required to start connection-oriented
 *            CTE transmission.
 */
typedef struct {
	uint16_t conn_handle;                              /*!< Connection handle of the ACL connection to CTE TX.
                                                        *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_gap_conn_cte_tx_param_t *param;          /*!< Connection CTE transmit parameter. */
} rtk_bt_le_gap_conn_cte_tx_start_t;

/**
 * @struct    rtk_bt_le_gap_conn_cte_rx_param_t
 * @brief     Connection-oriented CTE receive parameters passed to @ref rtk_bt_le_gap_conn_cte_rx_start.
 */
typedef struct {
	uint16_t cte_req_interval;                         /*!< CTE request interval, indicating whether CTE Request procedure is initiated only once or periodically.
                                                        *   0x0000: Initiate the CTE Request procedure once.
                                                        *   Other values: initiate every N underlying connection events. */
	uint8_t  req_cte_len;                              /*!< Minimum length of the Constant Tone Extension being requested in 8μs units.
                                                        *   Range: 0x02 to 0x14. */
	rtk_bt_le_gap_cte_type_t  req_cte_type;            /*!< Requested CTE type. */
	uint8_t  slot_durations;                           /*!< Switching and sampling slots @ref rtk_bt_le_gap_slot_durations_e. */
	uint8_t  num_ant_ids;                              /*!< The number of Antenna IDs in the switching pattern.
                                                        *   For AoA: Range: 0x02 to RTK_BLE_GAP_CTE_NUM_ANT_IDS_MAX.
                                                        *   For AoD: ignored; its default value is 0. */
	uint8_t  *ant_ids;                                 /*!< Antenna IDs in the switching pattern.
                                                        *   For AoD: ignored; its default value is NULL. */
} rtk_bt_le_gap_conn_cte_rx_param_t;

/**
 * @struct    rtk_bt_le_gap_conn_cte_rx_t
 * @brief     Controls enabling or disabling connection-oriented CTE reception on an established connection.
 */
typedef struct {
	uint16_t conn_handle;                              /*!< Connection handle of the ACL connection to CTE RX.
                                                        *   Range: 0x0000 to 0x0EFF. */
	bool enable;                                       /*!< CTE reception control.
                                                        *   - true: start connection-oriented CTE reception.
                                                        *   - false: stop connection-oriented CTE reception. */
	rtk_bt_le_gap_conn_cte_rx_param_t  *p_rx_param;    /*!< Connection CTE receive parameters; ignored when enable is false. */
} rtk_bt_le_gap_conn_cte_rx_t;

/**
 * @typedef   rtk_bt_le_gap_connless_cte_tx_state_e
 * @brief     Bluetooth LE connectionless CTE transmit state.
 */
typedef enum {
	RTK_BT_LE_GAP_CONNLESS_CTE_TX_STATE_IDLE = 0,
	RTK_BT_LE_GAP_CONNLESS_CTE_TX_STATE_EXT_ADV_IDLE,
	RTK_BT_LE_GAP_CONNLESS_CTE_TX_STATE_WAIT_EXT_ADV,
	RTK_BT_LE_GAP_CONNLESS_CTE_TX_STATE_WAIT_PA_ADV,
	RTK_BT_LE_GAP_CONNLESS_CTE_TX_STATE_ENABLING,
	RTK_BT_LE_GAP_CONNLESS_CTE_TX_STATE_TRANSMITTING,
} rtk_bt_le_gap_connless_cte_tx_state_e;

/**
 * @struct    rtk_bt_le_gap_connless_cte_tx_ind_t
 * @brief     Bluetooth LE connectionless CTE transmit state change indication.
 */
typedef struct {
	uint8_t  adv_handle;                                                 /*!< Extended advertising handle of the CTE transmit set. */
	uint16_t cause;                                                      /*!< Result code; 0 on success, non-zero indicates the reason for the state change. */
	rtk_bt_le_gap_connless_cte_tx_state_e state;                         /*!< New connectionless CTE transmit state. */
} rtk_bt_le_gap_connless_cte_tx_ind_t;

/**
 * @typedef   rtk_bt_le_gap_cte_packet_status_type_e
 * @brief     Bluetooth LE received CTE packet status type.
 */
typedef enum {
	RTK_BT_LE_GAP_CTE_PACKET_CRC_CORRECT                      = 0x00,                      /*!< CRC was correct. */
	RTK_BT_LE_GAP_CTE_PACKET_CRC_INCORRECT_DETERMINE          = 0x01,                      /*!< CRC was incorrect, and the Length and CTETime fields of the packet were used to determine
                                                                                            *   sampling points. */
	RTK_BT_LE_GAP_CTE_PACKET_CRC_INCORRECT_DETERMINE_OTHERWAY = 0x02,                      /*!< CRC was incorrect but the Controller has determined the position and length of the
                                                                                            *   Constant Tone Extension in some other way. */
	RTK_BT_LE_GAP_CTE_PACKET_INSUFFI_RESOURCES                = 0xFF,                      /*!< Insufficient resources to sample (Data_Channel_Index, CTE_Type, and Slot_Durations invalid). */
} rtk_bt_le_gap_cte_packet_status_type_e;

#define RTK_BT_LE_MAX_IQ_REPORT_CNT  (82)

/**
 * @struct    rtk_bt_le_gap_connless_iq_report_ind_t
 * @brief     Bluetooth LE IQ sample report from connectionless CTE reception on a periodic advertising train.
 */
typedef struct {
	uint8_t                                 sync_id;                                       /*!< Identify the periodic advertising train. */
	uint16_t                                sync_handle;                                   /*!< Sync_Handle identifying the periodic advertising train.
                                                                                            *   Range: 0x0000 to 0x0EFF. */
	uint8_t                                 channel_index;                                 /*!< The index of the channel on which the packet was received.
                                                                                            *   Range: 0x00 to 0x27.
                                                                                            *   Note: 0x25 to 0x27 can be used only for packets generated during test modes. */
	int16_t                                 rssi;                                          /*!< RSSI of the packet, ignoring the Constant Tone Extension.
                                                                                            *   Range: -1270 to +200.
                                                                                            *   Units: 0.1 dBm.
                                                                                            *   0x007F: RSSI was not available. */
	uint8_t                                 rssi_antenna_id;                               /*!< ID of the antenna on which the RSSI was measured. */
	rtk_bt_le_gap_cte_type_t                cte_type;                                      /*!< CTE type. */
	rtk_bt_le_gap_slot_durations_e          slot_durations;                                /*!< Specifies the sampling rate used by the Controller. */
	rtk_bt_le_gap_cte_packet_status_type_e  packet_status;                                 /*!< CTE packet status. */
	uint16_t                                periodic_event_counter;                        /*!< The value of paEventCounter for the reported AUX_SYNC_IND PDU. */
	uint8_t                                 sample_count;                                  /*!< Number of IQ sample pairs.
                                                                                            *   0x00: No samples provided (only permitted if Packet_Status is 0xFF).
                                                                                            *   0x09 to 0x52: Total number of sample pairs. */
	int8_t                                  iq_sample[RTK_BT_LE_MAX_IQ_REPORT_CNT * 2];    /*!< IQ sample pairs.
                                                                                            *   I_Sample[0],
                                                                                            *   Q_Sample[0], ..., I_Sample[i], Q_Sample[i].
                                                                                            *   Length = sample_count * 2.
                                                                                            *   Value 0x80 indicates no valid sample available. */
} rtk_bt_le_gap_connless_iq_report_ind_t;

/**
 * @struct    rtk_bt_le_gap_conn_iq_report_ind_t
 * @brief     Bluetooth LE IQ sample report from connection-oriented CTE reception.
 */
typedef struct {
	uint16_t
	conn_handle;                                   /*!< Connection handle of the ACL connection for which the CTE was received.
                                                                                            *   Range: 0x0000 to 0x0EFF. */
	uint8_t                                 rx_phy;                                        /*!< RX PHY used for the received PDU.
                                                                                            *   0x01: The receiver PHY for the connection is LE 1M.
                                                                                            *   0x02: The receiver PHY for the connection is LE 2M. */
	uint8_t
	data_chan_index;                               /*!< The index of the data channel on which the Data Physical Channel PDU was
                                                                                            *   received.
                                                                                            *   Range: 0x00 to 0x24. */
	int16_t                                 rssi;                                          /*!< RSSI of the packet ignoring the Constant Tone Extension.
                                                                                            *   Range: -1270 to +200.
                                                                                            *   Units: 0.1 dBm.
                                                                                            *   0x007F: RSSI was not available. */
	uint8_t                                 rssi_antenna_id;                               /*!< ID of the antenna on which the RSSI is measured. */
	rtk_bt_le_gap_cte_type_t                cte_type;                                      /*!< CTE type. */
	rtk_bt_le_gap_slot_durations_e          slot_durations;                                /*!< Switching and sampling slot durations. */
	rtk_bt_le_gap_cte_packet_status_type_e  packet_status;                                 /*!< CTE packet status. */
	uint16_t                                connection_event_counter;                      /*!< The value of connEventCounter for the reported PDU. */
	uint8_t                                 sample_count;                                  /*!< Number of IQ sample pairs.
                                                                                            *   0x00: No samples provided (only permitted if Packet_Status is 0xFF).
                                                                                            *   0x09 to 0x52: Total number of sample pairs (equal I and Q sample count). */
	int8_t                                  iq_sample[RTK_BT_LE_MAX_IQ_REPORT_CNT * 2];    /*!< IQ sample pairs.
                                                                                            *   I_Sample[0],
                                                                                            *   Q_Sample[0], ..., I_Sample[i], Q_Sample[i].
                                                                                            *   Length = sample_count * 2.
                                                                                            *   Value 0x80 indicates no valid sample available. */
} rtk_bt_le_gap_conn_iq_report_ind_t;

/**
 * @struct    rtk_bt_le_gap_cte_req_fail_ind_t
 * @brief     Bluetooth LE connection-oriented CTE Request fail indication msg.
 */
typedef struct {
	uint16_t conn_handle;                          /*!< Connection handle of the ACL connection for which the CTE request failed.
                                                    *   Range: 0x0000 to 0x0EFF. */
	uint16_t cause;                                /*!< CTE request fail cause. */
} rtk_bt_le_gap_cte_req_fail_ind_t;

/**
 * @struct    rtk_bt_le_gap_antenna_info_t
 * @brief     Bluetooth LE antenna information msg.
 */
typedef struct {
	uint8_t supported_switching_sampling_rates;    /*!< A bit field that indicates the switching rates and sampling rates supported by the Controller.
                                                    *   bit 0: 1 µs switching supported for AoD transmission.
                                                    *   bit 1: 1 µs sampling supported for AoD reception.
                                                    *   bit 2: 1 µs switching and sampling supported for AoA reception.
                                                    *   bit 3 to bit 7: Reserved for future use. */
	uint8_t num_antennae;                          /*!< The number of antennae supported by the Controller.
                                                    *   Range: 0x01 to 0x4B.
                                                    *   All other values: Reserved for future use. */
	uint8_t max_switching_pattern_length;          /*!< Maximum length of antenna switching pattern supported by the Controller.
                                                    *   Range: 0x02 to 0x4B.
                                                    *   All other values: Reserved for future use. */
	uint8_t max_cte_length;                        /*!< Maximum length of a transmitted Constant Tone Extension supported in 8µs units.
                                                    *   Range: 0x02 to 0x14.
                                                    *   All other values: Reserved for future use. */
} rtk_bt_le_gap_antenna_info_t;

#endif /* RTK_BLE_5_1_CTE_SUPPORT */

/**
 * @typedef   rtk_bt_le_dtm_rx_version_t
 * @brief     Receiver test version.
 */
typedef enum {
	RTK_BT_LE_DTM_RX_VERSION_V1 = 0x01,    /*!< Version 1. */
	RTK_BT_LE_DTM_RX_VERSION_V2 = 0x02,    /*!< Version 2. */
	RTK_BT_LE_DTM_RX_VERSION_V3 = 0x03,    /*!< Version 3. */
} rtk_bt_le_dtm_rx_version_t;

/**
 * @typedef   rtk_bt_le_dtm_tx_version_t
 * @brief     Transmitter test version.
 */
typedef enum {
	RTK_BT_LE_DTM_TX_VERSION_V1 = 0x01,    /*!< Version 1. */
	RTK_BT_LE_DTM_TX_VERSION_V2 = 0x02,    /*!< Version 2. */
	RTK_BT_LE_DTM_TX_VERSION_V3 = 0x03,    /*!< Version 3. */
	RTK_BT_LE_DTM_TX_VERSION_V4 = 0x04,    /*!< Version 4. */
} rtk_bt_le_dtm_tx_version_t;

/**
 * @typedef   rtk_bt_le_dtm_phy_rx_t
 * @brief     Bluetooth LE PHY type for receiver.
 */
typedef enum {
	RTK_BT_LE_DTM_RX_PHYS_1M       = 0x01,       /*!< LE PHY 1M used. */
	RTK_BT_LE_DTM_RX_PHYS_2M       = 0x02,       /*!< LE PHY 2M used. */
	RTK_BT_LE_DTM_RX_PHYS_CODED    = 0x03,       /*!< LE Coded PHY used. */
} rtk_bt_le_dtm_phy_rx_t;

/**
 * @typedef   rtk_bt_le_dtm_phy_tx_t
 * @brief     Bluetooth LE PHY type for transmitter.
 */
typedef enum {
	RTK_BT_LE_DTM_TX_PHYS_1M          = 0x01,    /*!< LE PHY 1M used. */
	RTK_BT_LE_DTM_TX_PHYS_2M          = 0x02,    /*!< LE PHY 2M used. */
	RTK_BT_LE_DTM_TX_PHYS_CODED_S8    = 0x03,    /*!< LE Coded PHY with S=8 data coding used. */
	RTK_BT_LE_DTM_TX_PHYS_CODED_S2    = 0x04,    /*!< LE Coded PHY with S=2 data coding used. */
} rtk_bt_le_dtm_phy_tx_t;

/**
 * @typedef   rtk_bt_le_dtm_mod_idx_t
 * @brief     Assume transmitter will have a certain type of modulation index.
 */
typedef enum {
	RTK_BT_LE_DTM_MODULATION_INDEX_STANDARD   = 0x00,    /*!< Standard modulation index. */
	RTK_BT_LE_DTM_MODULATION_INDEX_STABLE     = 0x01,    /*!< Stable modulation index. */
} rtk_bt_le_dtm_mod_idx_t;

/**
 * @typedef   rtk_bt_le_dtm_cte_type_t
 * @brief     Constant Tone Extension type.
 */
typedef enum {
	RTK_BT_LE_DTM_CTE_TYPE_AOA                = 0x00,            /*!< AoA Constant Tone Extension. */
	RTK_BT_LE_DTM_CTE_TYPE_AOD_1US_SLOT       = 0x01,            /*!< AoD Constant Tone Extension with 1 us slots. */
	RTK_BT_LE_DTM_CTE_TYPE_AOD_2US_SLOT       = 0x02,            /*!< AoD Constant Tone Extension with 2 us slots. */
} rtk_bt_le_dtm_cte_type_t;

/**
 * @typedef   rtk_bt_le_dtm_slot_durations_t
 * @brief     Switching and sampling slot duration type.
 */
typedef enum {
	RTK_BT_LE_DTM_SLOT_DURATIONS_SWITCH_SAMPLE_1US    = 0x01,    /*!< Switching and sampling slots are 1 us each. */
	RTK_BT_LE_DTM_SLOT_DURATIONS_SWITCH_SAMPLE_2US    = 0x02,    /*!< Switching and sampling slots are 2 us each. */
} rtk_bt_le_dtm_slot_durations_t;

/**
 * @typedef   rtk_bt_le_dtm_packet_payload_t
 * @brief     Transmitted packet payload type.
 */
typedef enum {
	RTK_BT_LE_DTM_PACKET_PAYLOAD_PRBS9    = 0x00,    /*!< PRBS9 sequence '11111111100000111101' (in transmission order). */
	RTK_BT_LE_DTM_PACKET_PAYLOAD_1100     = 0x01,    /*!< Repeated '11110000' (in transmission order) sequence. */
	RTK_BT_LE_DTM_PACKET_PAYLOAD_10       = 0x02,    /*!< Repeated '10101010' (in transmission order) sequence. */
	RTK_BT_LE_DTM_PACKET_PAYLOAD_PRBS15   = 0x03,    /*!< PRBS15 sequence. */
	RTK_BT_LE_DTM_PACKET_PAYLOAD_ALL1     = 0x04,    /*!< Repeated '11111111' (in transmission order) sequence. */
	RTK_BT_LE_DTM_PACKET_PAYLOAD_ALL0     = 0x05,    /*!< Repeated '00000000' (in transmission order) sequence. */
	RTK_BT_LE_DTM_PACKET_PAYLOAD_0011     = 0x06,    /*!< Repeated '00001111' (in transmission order) sequence. */
	RTK_BT_LE_DTM_PACKET_PAYLOAD_01       = 0x07,    /*!< Repeated '01010101' (in transmission order) sequence. */
} rtk_bt_le_dtm_packet_payload_t;

/**
 * @struct    rtk_bt_le_dtm_rx_param_t
 * @brief     Bluetooth LE receiver test parameters.
 */

typedef struct {
	uint8_t rx_channel;                               /*!< Channel to receive packets.
                                                       *   Range: 0x00 to 0x27.
                                                       *   Frequency Range: 2402 to 2480 MHz. */
	rtk_bt_le_dtm_phy_rx_t phy;                       /*!< PHY used by the receiver. */
	rtk_bt_le_dtm_mod_idx_t mod_idx;                  /*!< Modulation index used to receive packets. */
	uint8_t exp_cte_len;                              /*!< Expected length of the Constant Tone Extensions in received test reference packets.
                                                       *   Range: 0x00 (No Constant Tone Extension expected, default) or 0x02 to 0x14.
                                                       *   Units: 8 µs. */
	rtk_bt_le_dtm_cte_type_t exp_cte_type;            /*!< Expected type of the Constant Tone Extension in received test reference packets. */
	rtk_bt_le_dtm_slot_durations_t slot_durations;    /*!< Slot duration used for packet reception.
                                                       *   Only used when an AoA Constant Tone Extension is expected.
                                                       *   Ignored when an AoD Constant Tone Extension is expected. */
	uint8_t sw_pattern_len;                           /*!< Number of Antenna IDs in the switching pattern.
                                                       *   Only used when an AoA Constant Tone Extension is expected.
                                                       *   Ignored when an AoD Constant Tone Extension is expected.
                                                       *   Range: 0x02 to max_switching_pattern_length supported by the controller.
                                                       *   max_switching_pattern_length is less than or equal to 0x4B. */
	uint8_t *p_antenna_ids;                           /*!< Antenna ID in the pattern.
                                                       *   only used when an AoA Constant Tone Extension is expected.
                                                       *   Ignored when an AoD Constant Tone Extension is expected. */
} rtk_bt_le_dtm_rx_param_t;

/**
 * @struct    rtk_bt_le_dtm_tx_param_t
 * @brief     Bluetooth LE transmitter test parameters.
 */

typedef struct  {
	uint8_t tx_channel;                               /*!< Channel to transmit packets.
                                                       *   Range: 0x00 to 0x27.
                                                       *   Frequency Range: 2402 to 2480 MHz. */
	uint8_t data_len;                                 /*!< Payload length in each packet.
                                                       *   Range: 0x00 to 0xFF. */
	rtk_bt_le_dtm_packet_payload_t pkt_pl;            /*!< Payload contents of the test reference packets. */
	rtk_bt_le_dtm_phy_tx_t phy;                       /*!< PHY used by the transmitter. */
	uint8_t cte_len;                                  /*!< Length of the Constant Tone Extension in transmitted test reference packets.
                                                       *   Range: 0x00 (No Constant Tone Extension expected, default) or 0x02 to 0x14.
                                                       *   Units: 8 µs. */
	rtk_bt_le_dtm_cte_type_t cte_type;                /*!< Type of the Constant Tone Extension in transmitted test reference packets. */
	uint8_t sw_pattern_len;                           /*!< Number of antenna IDs in the switching pattern.
                                                       *   Ignored when cte_type is RTK_BT_LE_DTM_CTE_TYPE_AOA.
                                                       *   Range: 0x02 to max_switching_pattern_length supported by controller. max_switching_pattern_length shall be less than or
                                                       *   equal to 0x4B. */
	uint8_t *p_antenna_ids;                           /*!< Antenna IDs in the switching pattern.
                                                       *   Ignored when cte_type is RTK_BT_LE_DTM_CTE_TYPE_AOA. */
	int8_t tx_power_level;                            /*!< Set transmitter to the specified or the nearest transmit power level.
                                                       *   Range: -127 to +20 dBm.
                                                       *   0x7E: Set transmitter to minimum transmit power level.
                                                       *   0x7F: Set transmitter to maximum transmit power level. */
} rtk_bt_le_dtm_tx_param_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_le_gap BT LE GAP APIs
 * @brief     BT LE GAP related function APIs.
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Get the local connection id associated with a given connection handle. The connection id is an internal index (0 to RTK_BLE_GAP_MAX_LINKS - 1)
 *            that identifies a BLE connection within the local stack.
 * @param[in] conn_handle: Connection handle of the ACL connection on which the conn id to get.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[out] p_conn_id: Pointer to a variable that stores the connection id.
 *                        On success the value is in the range [0, RTK_BLE_GAP_MAX_LINKS - 1].
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_get_conn_id(uint16_t conn_handle, uint8_t *p_conn_id);

/**
 * @brief     Get local Bluetooth LE stack version information. Useful for debugging and for verifying which version of the stack is running.
 * @param[out] version: Output buffer for the local BT stack version fields.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_get_version(rtk_bt_le_version_info_t *version);

/**
 * @brief     Get local device's address. Returns the public device address.
 * @param[out] paddr: Output buffer for the local public address.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_get_bd_addr(rtk_bt_le_addr_t *paddr);

/**
 * @brief     Write the GAP Device Name characteristic value stored in the local attribute database. The name is exposed to remote devices
 *            via the GAP Device Name characteristic and may be included in advertising data.
 * @param[in] name: Pointer to a UTF-8 string. The value shall be 0 to 248 octets in length.
 *                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API does not affect the device name of advertising data if device name is also included in advertising data.
 *            - If device name is also included in advertising data, it is recommended to keep the two consistent.
 */
uint16_t rtk_bt_le_gap_set_device_name(const uint8_t *name);

/**
 * @brief     Write the GAP Appearance characteristic value in the local attribute database. The appearance is exposed to remote devices
 *            via the GAP Appearance characteristic and may be included in advertising data.
 * @param[in] appearance: 16-bit appearance value.
 *                        Passing an unknown value is permitted but may confuse peer devices.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API does not affect the appearance of advertising data if appearance is also included in advertising data.
 */
uint16_t rtk_bt_le_gap_set_appearance(uint16_t appearance);

/**
 * @brief     Write the Peripheral Preferred Connection Parameters (PPCP) characteristic in the local GAP service. Remote central devices can read this
 *            characteristic after connection establishment to determine preferred connection parameters.
 * @param[in] p_preferred_conn_param: Preferred connection parameter configuration.
 *                                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The PPCP values are advisory; the central is not required to honour them.
 *            - Only meaningful for peripheral role devices that advertise the Peripheral Preferred Connection Parameters characteristic
 *              in the GATT database.
 */
uint16_t rtk_bt_le_gap_set_preferred_conn_param(rtk_bt_le_preferred_conn_param_t *p_preferred_conn_param);

/**
 * @brief     Set a random address to the controller.
 *            If auto_generate is true, a random address of the specified type is automatically generated by the stack and set.
 *            If auto_generate is false, the provided address is used directly.
 * @param[in] auto_generate: If true, the stack generates a random address of type @p type and ignores the input value of p_addr.
 *                           If false, the address pointed to by p_addr is applied.
 * @param[in] type: Type of random address to generate when auto_generate is true.
 *                  Ignored when auto_generate is false.
 * @param[in,out] p_addr: When auto_generate is false: the 6-byte random address to program.
 *                        When auto_generate is true: output buffer that receives the generated address.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API must not be called while advertising, scanning, or an initiating procedure is active.
 *            - Stop all such procedures before calling this function, then restart them after it returns successfully.
 *            - If set static random address as local address, it should also be set as identity address.
 */
uint16_t rtk_bt_le_gap_set_rand_addr(bool auto_generate, rtk_bt_le_rand_addr_type_t type, uint8_t *p_addr);

/**
 * @brief     Generate a random address of the specified type and return it to the caller.
 * @param[in] type: Type of random address to generate.
 * @param[out] p_addr: Pointer to a 6-byte buffer that receives the generated address.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The generated address is returned in p_addr but is not applied to the controller.
 *              Call @ref rtk_bt_le_gap_set_rand_addr with auto_generate = false to apply it.
 */
uint16_t rtk_bt_le_gap_gen_rand_addr(rtk_bt_le_rand_addr_type_t type, uint8_t *p_addr);

/**
 * @brief     Configure the local identity address.
 * @param[in] type: Identity address type.
 * @param[in] p_addr: Pointer to the 6-byte identity address.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Only when you want to use a static random address as the device's address, you'd better set it as local identity address,
 *              Passing other address types is not recommended.
 *            - If not set, the default local identity address is your device's public address.
 */
uint16_t rtk_bt_le_gap_cfg_local_ident_addr(rtk_bt_le_ident_addr_type_t type, uint8_t *p_addr);

/**
 * @brief     Set the payload for the LE GAP legacy advertising PDU. The data is a sequence of AD structures,
 *            advertising data with the format: [Length 1][Data Type 1][Data 1][Length 2][Data Type 2][Data 2] ... .
 * @param[in] padv_data: Pointer to the advertising payload byte array.
 * @param[in] adv_len: Length of padv_data in bytes.
 *                     Range: 0 to 31.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_set_adv_data(uint8_t *padv_data, uint32_t adv_len);

/**
 * @brief     Set the payload for the LE GAP legacy scan response PDU, which is sent in reply to active scan requests when advertising is active. The data is buffered.
 *            It follows the same AD structure format as advertising data.
 * @param[in] pscan_rsp_data: Pointer to the scan response payload byte array.
 * @param[in] scan_rsp_len: Length of pscan_rsp_data in bytes.
 *                          Range: 0 to 31.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Scan response data is only transmitted for scannable advertising types.
 */
uint16_t rtk_bt_le_gap_set_scan_rsp_data(uint8_t *pscan_rsp_data, uint32_t scan_rsp_len);

/**
 * @brief     Start LE GAP legacy advertising with the supplied parameters and the previously set advertising and scan response data.
 *            The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_ADV_START_IND.
 * @param[in] padv_param: Legacy advertising parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Before calling rtk_bt_le_gap_start_adv, the adv state must be @ref RTK_BT_LE_ADV_STATE_IDLE. Call
 *              @ref rtk_bt_le_gap_get_dev_state to confirm.
 */
uint16_t rtk_bt_le_gap_start_adv(rtk_bt_le_adv_param_t *padv_param);

/**
 * @brief     Stop LE GAP legacy advertising. The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_ADV_STOP_IND.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Before calling rtk_bt_le_gap_stop_adv, the adv state must be @ref RTK_BT_LE_ADV_STATE_ADVERTISING. Call
 *              @ref rtk_bt_le_gap_get_dev_state to confirm.
 */
uint16_t rtk_bt_le_gap_stop_adv(void);

/**
 * @brief     Read the advertising parameters that are configured.
 * @param[out] padv_param: Output buffer for the current advertising parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_get_adv_param(rtk_bt_le_adv_param_t *padv_param);

/**
 * @brief     Check whether LE GAP advertising state is currently idle.
 * @return
 *            - true: Legacy advertising state is idle (not advertising)
 *            - false: Legacy advertising is currently active
 */
bool rtk_bt_le_gap_adv_is_idle(void);

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
/**
 * @brief     Create an extended advertising set for LE extended advertising. Each set is identified
 *            by a handle (adv_handle) that is used in subsequent operations such as @ref rtk_bt_le_gap_set_ext_adv_data, @ref rtk_bt_le_gap_start_ext_adv, and @ref rtk_bt_le_gap_remove_ext_adv.
 *            Extended advertising sets allow multiple independent advertising instances, each with its own parameters, data, and PHY.
 * @param[in] p_adv_param: Extended advertising parameters for the advertising set.
 *                         Must not be NULL.
 * @param[out] p_adv_handle: Pointer to a variable that receives the advertising set handle assigned by the stack.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_create_ext_adv(rtk_bt_le_ext_adv_param_t *p_adv_param, uint8_t *p_adv_handle);

/**
 * @brief     Set the LE GAP advertising data for an extended advertising set. The data is not immediately sent to the controller; it is cached and transferred to the controller
 *            when @ref rtk_bt_le_gap_start_ext_adv is called.
 * @param[in] adv_handle: Handle of the target advertising set, assigned by @ref rtk_bt_le_gap_create_ext_adv.
 *                        Range: 0x00 to 0xEF.
 * @param[in] padv_data: Pointer to the extended advertising payload.
 *                       The buffer must remain valid until @ref rtk_bt_le_gap_start_ext_adv executes.
 * @param[in] adv_len: Length of padv_data in bytes.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This function only saves the pointer to the advertising data, it does not copy the buffer.
 *              The pointer must remain valid and the data unchanged until @ref rtk_bt_le_gap_start_ext_adv is executed.
 */
uint16_t rtk_bt_le_gap_set_ext_adv_data(uint8_t adv_handle, uint8_t *padv_data, uint16_t adv_len);

/**
 * @brief     Set the LE GAP scan response payload for an extended advertising set. The data is not immediately sent to the controller;
 *            it is cached and transferred to the controller when @ref rtk_bt_le_gap_start_ext_adv is called.
 * @param[in] adv_handle: Handle of the target advertising set, assigned by @ref rtk_bt_le_gap_create_ext_adv.
 *                        Range: 0x00 to 0xEF.
 * @param[in] pscan_rsp_data: Pointer to the scan response payload.
 *                            The buffer must remain valid until @ref rtk_bt_le_gap_start_ext_adv executes.
 * @param[in] scan_rsp_len: Length of pscan_rsp_data in bytes.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This function only saves the pointer to the advertising data, it does not copy the buffer.
 *              The pointer must remain valid and the data unchanged until @ref rtk_bt_le_gap_start_ext_adv is executed.
 */
uint16_t rtk_bt_le_gap_set_ext_scan_rsp_data(uint8_t adv_handle, uint8_t *pscan_rsp_data, uint16_t scan_rsp_len);

/**
 * @brief     Start LE GAP extended advertising for a given advertising set. The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_EXT_ADV_IND.
 * @param[in] adv_handle: Advertising set handle, which is assigned by @ref rtk_bt_le_gap_create_ext_adv.
 *                        Range: 0x00 to 0xEF.
 * @param[in] duration: Maximum duration for which to advertise.
 *                      Range: 0 to 65535. 0 means advertising continues indefinitely until explicitly stopped.
 *                      Time = N * 10 ms.
 * @param[in] num_events: Maximum number of extended advertising events the controller shall attempt before stopping.
 *                        Range: 0 - 255. 0 means no limit.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Call @ref rtk_bt_le_gap_create_ext_adv, @ref rtk_bt_le_gap_set_ext_adv_data or
 *              @ref rtk_bt_le_gap_set_ext_scan_rsp_data before calling this function.
 *            - The data buffer(s) staged via those calls must remain valid until this function returns.
 */
uint16_t rtk_bt_le_gap_start_ext_adv(uint8_t adv_handle, uint16_t duration, uint8_t num_events);

/**
 * @brief     Disable a running extended advertising set. The advertising set definition is retained and can be restarted via @ref rtk_bt_le_gap_start_ext_adv.
 *            The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_EXT_ADV_IND.
 * @param[in] adv_handle: Handle of the advertising set to stop, which is assigned by @ref rtk_bt_le_gap_create_ext_adv.
 *                        Range: 0x00 to 0xEF.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_stop_ext_adv(uint8_t adv_handle);

/**
 * @brief     Remove an extended advertising set. Once removed, the adv_handle becomes invalid and must not be used again.
 * @param[in] adv_handle: Handle of the advertising set to remove, which is assigned by @ref rtk_bt_le_gap_create_ext_adv.
 *                        Range: 0x00 to 0xEF.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Stop the advertising set with @ref rtk_bt_le_gap_stop_ext_adv before calling this function.
 *            - Attempting to remove an active set will fail.
 */
uint16_t rtk_bt_le_gap_remove_ext_adv(uint8_t adv_handle);

/**
 * @brief     Resolve which extended advertising set was stopped because it resulted in a connection. When an extended connectable advertising set leads
 *            to a connection, it is stopped automatically. This function maps the connection handle to the advertising set handle.
 * @param[in] conn_handle: Connection handle of the ACL connection to get the corresponding conn id.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[out] adv_handle: Pointer to the advertising set handle that stopped due to this connection.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_get_ext_adv_handle_by_conn_handle(uint16_t conn_handle, uint8_t *adv_handle);

/**
 * @brief     Initiate an extended LE connection to a remote device using extended advertising connection parameters. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_GAP_EVT_CONNECT_IND.
 * @param[in] p_ext_conn_param: Extended connection creation parameters.
 *                              Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_ext_connect(rtk_bt_le_ext_create_conn_param_t *p_ext_conn_param);
#endif

#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
/**
 * @brief     Set and start LE GAP periodic advertising. Periodic advertising allows the advertiser to transmit data on a regular schedule
 *            that scanners can synchronize with. The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_PA_IND.
 * @param[in] param: Periodic advertising configuration for the set.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Periodic advertising is only supported on extended advertising sets whose event properties
 *              are @ref RTK_BT_LE_EXT_ADV_EXTENDED_ADV_NON_SCAN_NON_CONN_UNDIRECTED or @ref RTK_BT_LE_EXT_ADV_EXTENDED_ADV_NON_SCAN_NON_CONN_DIRECTED.
 *            - The associated extended advertising set must already exist and be started via @ref rtk_bt_le_gap_start_ext_adv.
 */
uint16_t rtk_bt_le_gap_start_pa(rtk_bt_le_pa_param_t *param);

/**
 * @brief     Stop periodic advertising on the specified extended advertising set. The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_PA_IND.
 * @param[in] adv_handle: Handle of the extended advertising set whose periodic advertising should be stopped. which is assigned by @ref rtk_bt_le_gap_create_ext_adv.
 *                        Range: 0x00 to 0xEF.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_stop_pa(uint8_t adv_handle);

/**
 * @brief     Update the periodic advertising data or the Advertising Data ID (DID) on a running periodic advertising set. Updating the DID causes synchronized scanners
 *            to detect a data change without requiring a full re-synchronisation. The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_PA_IND.
 * @param[in] adv_handle: Handle of the extended advertising set to update. which is assigned by @ref rtk_bt_le_gap_create_ext_adv.
 *                        Range: 0x00 to 0xEF.
 * @param[in] update_did_only: If true: only the DID is updated; the data and len parameters are ignored and the payload remains unchanged.
 *                             If false: the new payload provided in data and len is applied and the DID is also updated.
 * @param[in] data: Pointer to the new periodic advertising payload.
 *                  Ignored when update_did_only is true.
 *                  Must not be NULL.
 * @param[in] len: Length of new periodic advertising data in bytes.
 *                 Ignored when update_did_only is true.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_update_pa(uint8_t adv_handle, bool update_did_only, uint8_t *data, uint16_t len);
#endif

#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
/**
 * @brief     Modify the Periodic Advertiser List by adding, removing, or clearing entries. The controller uses this list when synchronizing to periodic advertising
 *            trains with the allow-list filter enabled.
 * @param[in] operation: Whether to add a new entry, remove an existing entry, or clear all entries.
 * @param[in] adv_addr: Address of the periodic advertiser to add or remove.
 * @param[in] adv_sid: Advertising SID (0x00 to 0x0F) in the ADI field that identifies the periodic advertising train.
 *                     Ignored when operation is @ref RTK_BT_LE_PA_SYNC_ADV_LIST_OP_CLEAR.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - When operation is @ref RTK_BT_LE_PA_SYNC_ADV_LIST_OP_CLEAR, the adv_addr and adv_sid parameters are irrelevant and are ignored.
 *            - Only address types @ref RTK_BT_LE_ADDR_TYPE_PUBLIC and @ref RTK_BT_LE_ADDR_TYPE_RANDOM are valid for Add and Remove operations.
 */
uint16_t rtk_bt_le_gap_pa_sync_modify_adv_list(rtk_bt_le_pa_sync_advlist_op_t operation, rtk_bt_le_addr_t adv_addr, uint8_t adv_sid);

/**
 * @brief     Initiate synchronisation with a periodic advertising train. Once synchronized, sync state is delivered via event @ref RTK_BT_LE_GAP_EVT_PA_SYNC_STATE_IND.
 *            If adv report is enabled by param->options. The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_PA_ADV_REPORT_IND.
 * @param[in] param: Periodic advertising synchronization creation parameters.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Only one pending sync creation is allowed at a time.
 *            - Call @ref rtk_bt_le_gap_pa_sync_terminate to cancel a pending attempt before starting another.
 */
uint16_t rtk_bt_le_gap_pa_sync_create(rtk_bt_le_pa_sync_create_t *param);

/**
 * @brief     Terminate an established periodic advertising sync or cancel a pending sync creation attempt. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_GAP_EVT_PA_SYNC_STATE_IND.
 * @param[in] sync_id: Identifier of the periodic advertising sync or pending creation to terminate. Assigned by the stack when sync is established or a creation attempt
 *                     is initiated.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - After termination, the sync_id is invalid and must not be reused.
 *            - To re-establish sync, call @ref rtk_bt_le_gap_pa_sync_create again.
 */
uint16_t rtk_bt_le_gap_pa_sync_terminate(uint8_t sync_id);
#endif

#if defined(RTK_BLE_5_1_PAST_SENDER_SUPPORT) && RTK_BLE_5_1_PAST_SENDER_SUPPORT
/**
 * @brief     Initiate a Periodic Advertising Sync Transfer (PAST) to a connected peer. The peer can use the transferred sync information to synchronise
 *            with the periodic advertising train without scanning for it. On completion, @ref RTK_BT_LE_GAP_EVT_PA_SYNC_STATE_IND is delivered on the peer when
 *            synchronization succeeds.
 * @param[in] conn_handle: Connection handle of the ACL connection on which to transfer the sync information.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] service_data: Application-defined value to identify the periodic advertising train to the peer device.
 * @param[in] use_sync_id: Whether the idx parameter is a PA sync ID or an advertising set handle.
 *                         If true: idx is a PA sync ID obtained from a remote device via synchronization.
 *                         If false: idx is an advertising set handle from local periodic advertising.
 * @param[in] idx: When use_sync_id is true: PA sync ID obtained from @ref rtk_bt_le_gap_pa_sync_create.
 *                 When use_sync_id is false: advertising set handle from @ref rtk_bt_le_gap_create_ext_adv.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - A valid ACL connection identified by @p conn_handle must exist before calling this function.
 */
uint16_t rtk_bt_le_gap_past_send(uint16_t conn_handle, uint16_t service_data, bool use_sync_id, uint8_t idx);
#endif

#if defined(RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT) && RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT
/**
 * @brief     Configure the PAST reception parameters for a specific ACL connection. When a PAST packet is received
 *            on that connection, the event @ref RTK_BT_LE_GAP_EVT_PA_SYNC_STATE_IND is delivered.
 * @param[in] param: PAST reception parameters for this connection.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Call this function before the PAST packet arrives on the target connection.
 */
uint16_t rtk_bt_le_gap_past_recipient_set(rtk_bt_le_past_recv_param_t *param);

/**
 * @brief     Configure the default PAST reception parameters applied to all future ACL connections. When a PAST packet is received on any connection using
 *            these defaults, the event @ref RTK_BT_LE_GAP_EVT_PA_SYNC_STATE_IND is delivered.
 * @param[in] param: Default PAST reception parameters for all future connections.
 *                   The conn_handle field is ignored.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_default_past_recipient_set(rtk_bt_le_past_recv_param_t *param);
#endif
/**
 * @brief     Configure host-side scan result filtering criteria. Only advertising reports that match all specified filter conditions are forwarded
 *            to the application via @ref RTK_BT_LE_GAP_EVT_SCAN_RES_IND or @ref RTK_BT_LE_GAP_EVT_EXT_SCAN_RES_IND. This filter is applied in the host stack layer,
 *            independently of the controller's built-in filter policy.
 * @param[in] p_scan_info: Scan result filter criteria.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_scan_info_filter(rtk_bt_le_scan_info_filter_param_t *p_scan_info);

/**
 * @brief     Set the LE legacy scan parameters. Parameters are cached in the stack and transferred to the controller when scanning is started. Calling this function
 *            while scanning is active does not affect the ongoing scan. Restart scanning to apply new parameters.
 * @param[in] p_gap_scan_param: Legacy scan configuration.
 *                              Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Set parameters before calling @ref rtk_bt_le_gap_start_scan.
 */
uint16_t rtk_bt_le_gap_set_scan_param(rtk_bt_le_scan_param_t *p_gap_scan_param);

/**
 * @brief     Get the cached LE legacy scan parameters previously set by @ref rtk_bt_le_gap_set_scan_param.
 * @param[out] pscan_param: Output buffer for the current legacy scan parameters.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_get_scan_param(rtk_bt_le_scan_param_t *pscan_param);

/**
 * @brief     Start LE legacy scanning. The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_SCAN_START_IND. Each discovered advertising report
 *            that passes the host filter is delivered via @ref RTK_BT_LE_GAP_EVT_SCAN_RES_IND.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Set parameters before calling this API.
 *            - Before calling this API, verify the scan state via @ref rtk_bt_le_gap_get_dev_state. The state must be @ref RTK_BT_LE_SCAN_STATE_IDLE.
 */
uint16_t rtk_bt_le_gap_start_scan(void);

/**
 * @brief     Stop LE legacy scanning. The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_SCAN_STOP_IND.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Before calling this API, verify the scan state via @ref rtk_bt_le_gap_get_dev_state. The state must be @ref RTK_BT_LE_SCAN_STATE_SCANNING.
 */
uint16_t rtk_bt_le_gap_stop_scan(void);

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
/**
 * @brief     Set LE extended scan parameters for one or both PHYs (LE 1M and/or LE Coded) used by @ref rtk_bt_le_gap_start_ext_scan. Parameters are cached and applied
 *            to the controller when scanning is started.
 * @param[in] p_param: Extended scan parameters for one or both PHYs.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Call this function before @ref rtk_bt_le_gap_start_ext_scan.
 */
uint16_t rtk_bt_le_gap_set_ext_scan_param(rtk_bt_le_ext_scan_param_t *p_param);

/**
 * @brief     Start LE extended scanning. The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_SCAN_START_IND.
 *            Each extended advertising report that passes the host filter is delivered as @ref RTK_BT_LE_GAP_EVT_EXT_SCAN_RES_IND.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Set parameters before calling this API.
 *            - Before calling this API, verify the scan state via @ref rtk_bt_le_gap_get_dev_state. The state must be @ref RTK_BT_LE_SCAN_STATE_IDLE.
 */
uint16_t rtk_bt_le_gap_start_ext_scan(void);

/**
 * @brief     Stop LE extended scanning. The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_SCAN_STOP_IND.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Before calling this API, verify the scan state via @ref rtk_bt_le_gap_get_dev_state. The state must be @ref RTK_BT_LE_SCAN_STATE_SCANNING.
 */
uint16_t rtk_bt_le_gap_stop_ext_scan(void);
#endif

/**
 * @brief     Initiate a legacy LE connection procedure. The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_CONNECT_IND.
 * @param[in] p_conn_param: Connection creation parameters.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Only one initiating procedure may be active at a time.
 *            - To use the whitelist for filtering, set p_conn_param->use_filter_list to true, p_conn_param->peer_addr is then ignored and at least one
 *              whitelist entry must exist.
 */
uint16_t rtk_bt_le_gap_connect(rtk_bt_le_create_conn_param_t *p_conn_param);

/**
 * @brief     Cancel an ongoing LE connection establishment procedure. The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_DISCONN_IND.
 * @param[in] cancel_addr: Address of the peer device whose pending connection attempt should be cancelled.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This function is only valid when an initiating procedure started by @ref rtk_bt_le_gap_connect or @ref rtk_bt_le_gap_ext_connect is pending.
 *            - Calling it when no initiation is in progress will fail.
 */
uint16_t rtk_bt_le_gap_connect_cancel(rtk_bt_le_addr_t *cancel_addr);

/**
 * @brief     Disconnect an established LE connection. The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_DISCONN_IND.
 * @param[in] conn_handle: Connection handle of the ACL connection to disconnect.
 *                         Range: 0x0000 to 0x0EFF.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - To disconnect with a specific error reason, use @ref rtk_bt_le_gap_disconnect_with_reason instead.
 */
uint16_t rtk_bt_le_gap_disconnect(uint16_t conn_handle);

/**
 * @brief     Disconnect an established LE connection with a specific reason code. This allows the application to convey a specific reason
 *            to the peer device. The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_DISCONN_IND.
 * @param[in] conn_handle: Connection handle of the ACL connection to disconnect.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] reason: Reason code for disconnection.
 *                    - @ref RTK_BT_HCI_ERR_REMOTE_USER_TERMINATE: remote user terminate.
 *                    - @ref RTK_BT_HCI_ERR_REMOTE_LOW_RESOURCE: remote low resource.
 *                    - @ref RTK_BT_HCI_ERR_REMOTE_POWER_OFF: remote power off.
 *                    - @ref RTK_BT_HCI_ERR_UNSUPPORTED_REMOTE_FEAT: unsupported remote feature.
 *                    - @ref RTK_BT_HCI_ERR_UNACCEPTABLE_CONN_INTERVAL: unacceptable connection parameters.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_disconnect_with_reason(uint16_t conn_handle, uint8_t reason);

/**
 * @brief     Request an update to the LE connection parameters for an established connection.
 *            If the local device is the central, it updates the connection parameters directly. If the local device is the peripheral, it sends an L2CAP Connection
 *            Parameter Update Request to the central.
 *            If the controller updates the connection parameters, the result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_CONN_UPDATE_IND.
 * @param[in] p_update_conn_param: New connection parameter values to request update.
 *                                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The supervision timeout must be greater than (1 + latency) * interval_max * 2.
 *            - The central may reject the request.
 */
uint16_t rtk_bt_le_gap_update_conn_param(rtk_bt_le_update_conn_param_t *p_update_conn_param);

/**
 * @brief     Modify device Filter Accept List (formerly White List). The list is used by advertising, scanning, and connection
 *            procedures when the whitelist filter policy is selected.
 * @param[in] p_wl_op_param: Filter Accept List modification parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The whitelist cannot be modified while any of the following apply:
 *              1. advertising uses the Filter Accept List and is enabled.
 *              2. scanning uses the Filter Accept List and scanning is active.
 *              3. the initiator filter policy
 *              uses the Filter Accept List and connection creation is pending.
 *            - To clear the entire whitelist, set operation to @ref RTK_BT_LE_WL_OP_CLEAR.
 *            - Address shall be ignored when Address_Type is 0xFF.
 */
uint16_t rtk_bt_le_gap_modify_whitelist(rtk_bt_le_modify_wl_param_t *p_wl_op_param);

/**
 * @brief     Read the RSSI (Received Signal Strength Indication) of a connected LE connection.
 * @param[in] conn_handle: Connection handle of the ACL connection to read RSSI.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[out] p_rssi: Pointer to a signed 8-bit variable that receives the RSSI value in dBm.
 *                     Range: -127 to +20 dBm.
 *                     The value 0x7F (127) indicates RSSI is unavailable.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_read_rssi(uint16_t conn_handle, int8_t *p_rssi);

/**
 * @brief     Read the version information of a remote device's controller. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_GAP_EVT_READ_REMOTE_VERSION_IND.
 * @param[in] conn_handle: Connection handle of ACL connection to read the version of the remote device.
 *                         Range: 0x0000 to 0x0EFF.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_read_remote_version(uint16_t conn_handle);

/**
 * @brief     Get the current GAP device state, including initialization state, advertising state, scanning state, and connection state.
 * @param[out] p_dev_state: Output buffer for the current device state.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_get_dev_state(rtk_bt_le_gap_dev_state_t *p_dev_state);

/**
 * @brief     Get the connection information for a specific LE connection.
 * @param[in] conn_handle: Connection handle of the ACL connection to get connection information.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[out] p_conn_info: Output buffer for the connection parameters of the specified connection.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The connection identified by @p conn_handle must already be in the connected state; otherwise the call returns an error.
 */
uint16_t rtk_bt_le_gap_get_conn_info(uint16_t conn_handle, rtk_bt_le_conn_info_t *p_conn_info);

/**
 * @brief     Get the negotiated ATT MTU size for an established LE connection. The ATT_MTU is determined during MTU exchange
 *            and is the minimum of the local device's maximum MTU and the remote device's maximum MTU. The minimum ATT_MTU is 23 bytes.
 * @param[in] conn_handle: Connection handle of the ACL connection to get MTU size.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[out] p_mtu_size: Output buffer for the negotiated ATT MTU value in bytes.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The MTU value is only meaningful after a connection has been established.
 *            - Before an MTU exchange the default ATT MTU of 23 bytes is reported.
 */
uint16_t rtk_bt_le_gap_get_mtu_size(uint16_t conn_handle, uint16_t *p_mtu_size);

/**
 * @brief     Get the number of active LE connections and their connection handles.
 *            This is useful for enumerating all current connections, e.g., when needing to disconnect existing connected devices.
 * @param[out] p_active_conn: Output buffer for the list of currently active connections.
 *                            On success, conn_handle[0..conn_num-1] are populated with active connection handles.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_get_active_conn(rtk_bt_le_get_active_conn_t *p_active_conn);

/**
 * @brief     Get the connection handle of an active LE connection by the peer device address.
 *            This is useful for looking up the connection handle when only the peer address is known.
 * @param[in] p_addr: BLE address of the peer device to look up.
 *                    Both address value and address type must match the address used during connection establishment.
 *                    Must not be NULL.
 * @param[out] p_conn_handle: Output buffer for the connection handle of the matching active ACL connection.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_get_conn_handle_by_addr(rtk_bt_le_addr_t *p_addr, uint16_t *p_conn_handle);

/**
 * @brief     Configure the maximum ATT MTU size that the local device will propose during an ATT_EXCHANGE_MTU procedure. The negotiated ATT_MTU
 *            for a connection is the minimum of the local maximum and the remote device's maximum. A larger MTU allows GATT reads and writes to carry more data per ATT
 *            PDU. The default ATT_MTU is 23 bytes.
 * @param[in] mtu_size: Maximum ATT MTU size to negotiate.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This function must be called before the first LE connection is established.
 *            - The configured MTU applies globally to all subsequent connections.
 *            - The actual negotiated MTU per connection may be lower if the peer advertises a smaller MTU.
 */
uint16_t rtk_bt_le_gap_set_max_mtu_size(uint16_t mtu_size);

/**
 * @brief     Set the LE channel map used by the local controller.
 * @param[in] p_chan_map: Pointer to a 5-byte array (@ref RTK_BT_LE_CHAN_MAP_LEN) encoding the desired LE channel map bitmask.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - At least two channels must remain enabled in the map.
 */
uint16_t rtk_bt_le_gap_set_channels(uint8_t *p_chan_map);

#if defined(RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT) && RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT
/**
 * @brief     Request a change to the maximum PDU payload size and transmission time for a specified LE connection using the Data Length Extension feature.
 *            If the controller accepts and applies the new parameters it will generate a @ref RTK_BT_LE_GAP_EVT_DATA_LEN_CHANGE_IND event carrying the effective values
 *            negotiated with the peer.
 * @param[in] p_datalen_param: Requested maximum PDU payload size and transmission time for the connection.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The values in the event may differ from the requested values.
 */
uint16_t rtk_bt_le_gap_set_data_len(rtk_bt_le_set_datalen_param_t *p_datalen_param);
#endif

#if defined(RTK_BLE_5_0_SET_PHYS_SUPPORT) && RTK_BLE_5_0_SET_PHYS_SUPPORT
/**
 * @brief     Request a PHY update for a specified LE connection, expressing the host's preferred transmit and receive PHYs (1M, 2M, or Coded). This requests
 *            the controller to use the specified PHY preferences for transmission and reception. The Controller might not be able to make the change (e.g. because the peer
 *            does not support the requested PHY) or may decide that the current PHY is preferable. If the controller updates the PHY, the result is reported
 *            asynchronously via event @ref RTK_BT_LE_GAP_EVT_PHY_UPDATE_IND.
 * @param[in] p_phy_param: Preferred PHY configuration for transmit and receive directions.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_set_phy(rtk_bt_le_set_phy_param_t *p_phy_param);
#endif

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
/**
 * @brief     Enable and initialize the LE privacy.  Privacy uses Resolvable Private Addresses (RPAs) to reduce the ability to track.
 * @param[in] whitelist: - true: also use the whitelist (Filter Accept List) in conjunction with privacy;
 *                       - false: not use whitelist (Filter Accept List).
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Must be called after BT init and before any advertising or scanning operation or initiating connections that rely on address privacy.
 */
uint16_t rtk_bt_le_gap_privacy_init(bool whitelist);

/**
 * @brief     Set the LE privacy mode for a specific peer identity address. Two privacy modes are available:
 *            - Network Privacy Mode (default): if the peer has distributed its IRK, the Controller shall not accept
 *            advertising packets containing the peer's Identity Address.
 *            - Device Privacy Mode: the Controller shall accept advertising packets from the peer whether they contain
 *            the peer's Identity Address or a private address, even if the peer has distributed its IRK.
 * @param[in] peer_ident_addr_type: Identity address type of the peer.
 * @param[in] peer_addr: 6-byte peer identity address.
 *                       Must not be NULL.
 * @param[in] privacy_mode: Privacy mode to apply.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_set_privacy_mode(rtk_bt_le_ident_addr_type_t peer_ident_addr_type,
										uint8_t *peer_addr, rtk_bt_le_privacy_mode_t privacy_mode);

/**
 * @brief     Read the current local Resolvable Private Address (RPA) that the controller is using for the given peer identity address.
 * @param[in] peer_ident_addr_type: Peer identity address type.
 * @param[in] peer_addr: Peer identity address.
 *                       Must not be NULL.
 * @param[out] local_rpa: Buffer to receive the local RPA.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Call @ref rtk_bt_le_gap_privacy_init first.
 */
uint16_t rtk_bt_le_gap_read_local_resolv_addr(rtk_bt_le_ident_addr_type_t peer_ident_addr_type,
											  uint8_t *peer_addr, uint8_t *local_rpa);

/**
 * @brief     Read the current peer Resolvable Private Address (RPA) that the remote device is currently using for the specified peer identity address.
 * @param[in] peer_ident_addr_type: Peer identity address type.
 * @param[in] peer_addr: Peer identity address.
 *                       Must not be NULL.
 * @param[out] peer_rpa: Buffer to receive the peer's RPA.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Call @ref rtk_bt_le_gap_privacy_init first.
 */
uint16_t rtk_bt_le_gap_read_peer_resolv_addr(rtk_bt_le_ident_addr_type_t peer_ident_addr_type,
											 uint8_t *peer_addr, uint8_t *peer_rpa);
#endif

/**
 * @brief     Set the LE Security Manager pairing mode for the local device.
 *            When set to @ref RTK_PAIRING_MODE_PAIRABLE, the stack accepts incoming pairing requests;
 *            when set to @ref RTK_PAIRING_MODE_NO_PAIRING, all pairing requests from remote devices are rejected.
 *            The change takes effect immediately for subsequent pairing attempts.
 * @param[in] pairing_mode: Desired pairing mode.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This setting applies globally to all connections.
 */
uint16_t rtk_bt_le_sm_set_pairing_mode(rtk_bt_le_pairing_mode_t pairing_mode);

/**
 * @brief     Configure the LE Security Manager security parameters used for all subsequent pairing procedures.
 * @param[in] p_sec_param: Pointer to security parameter structure.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_sm_set_security_param(rtk_bt_le_security_param_t *p_sec_param);

/**
 * @brief     Get the currently configured LE Security Manager security parameters.
 * @param[out] p_sec_param: Output buffer to receive the current security configuration.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_sm_get_security_param(rtk_bt_le_security_param_t *p_sec_param);

/**
 * @brief     Initiate or request a security upgrade on an established LE connection. The pairing result is reported asynchronously
 *            via event @ref RTK_BT_LE_GAP_EVT_AUTH_COMPLETE_IND.
 * @param[in] conn_handle: Connection handle of ACL connection on which to start security.
 *                         Range: 0x0000 to 0x0EFF.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - When the local device is the central, it sends an SMP Pairing Request to the peer.
 *            - When the local device is the peripheral, it sends a Security Request to ask the central to initiate pairing.
 */
uint16_t rtk_bt_le_sm_start_security(uint16_t conn_handle);

/**
 * @brief     Respond to a Just Works pairing confirmation request received via event @ref RTK_BT_LE_GAP_EVT_AUTH_PAIRING_CONFIRM_IND. The application must call
 *            this function to either accept or reject the pairing; setting confirm to true allows the procedure to continue, while confirm to false causes the stack to send
 *            a Pairing Failed response.
 * @param[in] p_pair_cfm: Pointer to pairing confirm parameter structure.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_sm_pairing_confirm(rtk_bt_le_pair_cfm_t *p_pair_cfm);

/**
 * @brief     Supply the passkey entered by the user in response to an authentication key input request received via event @ref RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_INPUT_IND.
 * @param[in] p_key_input: Pointer to the authentication key input parameter structure.
 *                         The valid range of passkey field is 000000 to 999999 inclusive.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_sm_passkey_entry(rtk_bt_le_auth_key_input_t *p_key_input);

/**
 * @brief     Confirm or reject the passkey displayed during Numeric Comparison pairing in response to event @ref RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_CONFIRM_IND. The user
 *            must visually compare the six-digit value shown on both devices; set confirm=1 if they match, 0 if they do not.
 * @param[in] p_key_cfm: Pointer to the authentication key confirmation parameter structure.
 *                       Set confirm field to 1 if the displayed values match, 0 otherwise.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The peer device must have an established ACL connection before calling this function.
 */
uint16_t rtk_bt_le_sm_passkey_confirm(rtk_bt_le_auth_key_confirm_t *p_key_cfm);

#if defined(RTK_BLE_SMP_OOB_SUPPORT) && RTK_BLE_SMP_OOB_SUPPORT
/**
 * @brief     Set OOB data for LE Legacy OOB pairing. This function must be called after receiving event @ref RTK_BT_LE_GAP_EVT_AUTH_OOB_KEY_INPUT_IND,
 *            which indicates the stack is waiting for the OOB TK value that was exchanged with the peer device through an out-of-band channel (e.g. NFC or QR code).
 * @param[in] p_set_oob_key: Pointer to the OOB TK data structure.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The TK value must be obtained from the peer device through a secure out-of-band channel prior to calling this function.
 *            - The peer device must be connected before this function can succeed.
 */
uint16_t rtk_bt_le_sm_set_oob_tk(rtk_bt_le_set_oob_key_t *p_set_oob_key);

/**
 * @brief     Generate the local OOB data (random number and confirm value) for LE Secure Connections OOB pairing. The returned rand and confirm values must
 *            be transmitted to the peer device through a secure out-of-band channel before the SC pairing procedure begins.
 * @param[out] local_oob: Pointer to a caller-allocated structure that receives the generated random number and confirmation value.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_sm_get_sc_local_oob(rtk_bt_le_sc_local_oob_data_t *local_oob);

/**
 * @brief     Input the LE Secure Connections OOB data received from the peer device via an out-of-band channel. The peer device must already be connected to the local device
 *            before calling this function.
 * @param[in] peer_oob: Peer OOB data.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_sm_input_sc_peer_oob(rtk_bt_le_sc_peer_oob_data_t *peer_oob);
#endif

/**
 * @brief     Get the total number of bonded devices currently stored in the security manager's bond database.
 * @param[out] bond_num: Pointer to the variable to store the number of bonded devices.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_sm_get_bond_num(uint8_t *bond_num);

/**
 * @brief     Check if a specific device is bonded in the security manager's bond database by its Bluetooth address and address type.
 * @param[in] paddr: BLE address of the peer device to check.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - For devices using RPA, provide the identity address.
 */
bool rtk_bt_le_sm_is_device_bonded(rtk_bt_le_addr_t *paddr);

/**
 * @brief     Get information about bonded devices stored in the host stack.
 * @param[in,out] p_bond_info: Pointer to the bond info array to fill.
 *                             Must not be NULL.
 * @param[in,out] size: On entry, the number of slots available in p_bond_info.
 *                      On return, updated to the total number of bonded devices stored in the stack.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Allocate the @p p_bond_info buffer based on the value returned by @ref rtk_bt_le_sm_get_bond_num to ensure all entries are retrieved.
 *            - If the output @p *size exceeds the input @p *size, repeat the call with a larger buffer.
 */
uint16_t rtk_bt_le_sm_get_bond_info(rtk_bt_le_bond_info_t *p_bond_info, uint8_t *size);

/**
 * @brief     Delete a bonded device entry from the security manager's bond database. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_GAP_EVT_BOND_MODIFY_IND.
 * @param[in] paddr: Pointer to the address of the device whose bond record is to be deleted.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_sm_delete_bond_device(rtk_bt_le_addr_t *paddr);

/**
 * @brief     Remove all bonding records from the security manager's bond database, clearing the entire bond list. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_GAP_EVT_BOND_MODIFY_IND.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_sm_clear_bond_list(void);

/**
 * @brief     Get the number of outgoing ACL data packets queued in the APP TX buffer for a specific connection, providing a flow-control view of unsent TX buffers.
 * @param[in] conn_handle: Connection handle of the ACL connection to get tx pending number.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[out] tx_pending_num: Pointer to a variable that receives the number of TX packets currently pending for this connection.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Use this function for application-level flow control before sending large amounts of data.
 */
uint16_t rtk_bt_le_gap_get_tx_pending_num(uint16_t conn_handle, uint16_t *tx_pending_num);

#if defined(RTK_BLE_5_2_POWER_CONTROL_SUPPORT) && RTK_BLE_5_2_POWER_CONTROL_SUPPORT
/**
 * @brief     Read the current and maximum transmit power levels of the local controller on the specified ACL connection and PHY.
 * @param[in] conn_handle: Connection handle of the ACL connection to read local TX power.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] phy: PHY to read power for.
 * @param[out] cur_txpower: Output buffer for the current TX power level in dBm.
 *                          Range: -127 to +20 dBm.
 *                          0x7F: TX power information is unavailable.
 *                          Must not be NULL.
 * @param[out] max_txpower: Output buffer for the maximum TX power level in dBm.
 *                          Range: -127 to +20 dBm.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The connection must be established and the LE Power Control feature must be supported by both local and remote controllers.
 */
uint16_t rtk_bt_le_gap_read_local_tx_power(uint16_t conn_handle, rtk_bt_le_txpower_phy_t phy,
										   int8_t *cur_txpower, int8_t *max_txpower);

/**
 * @brief     Read the transmit power level currently used by the remote controller on the specified ACL connection and PHY.
 * @param[in] conn_handle: Connection handle of the ACL connection to read remote TX power.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] phy: PHY to read power for.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The connection must be established and the LE Power Control feature must be supported by both local and remote controllers.
 */
uint16_t rtk_bt_le_gap_read_remote_tx_power(uint16_t conn_handle, rtk_bt_le_txpower_phy_t phy);

/**
 * @brief     Enable or disable automatic reporting of transmit power level changes for the local and/or remote controller on an ACL connection. When enabled, any TX
 *            power change triggers event @ref RTK_BT_LE_GAP_EVT_TXPOWER_REPORT_IND asynchronously.
 * @param[in] conn_handle: Connection handle of the ACL connection to set TX power report enable.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] local_enable: - true: enable reporting of local TX power level changes.
 *                          - false: disable reporting of local TX power level changes.
 * @param[in] remote_enable: - true: enable reporting of remote TX power level changes.
 *                           - false: disable reporting of remote TX power level changes.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Both @p local_enable and @p remote_enable can be controlled independently.
 *            - The LE Power Control feature must be supported by both sides for remote reporting.
 *            - Setting both to false disables all TX power change notifications on the connection.
 */
uint16_t rtk_bt_le_gap_tx_power_report_set(uint16_t conn_handle, bool local_enable, bool remote_enable);
#endif

#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT

/**
 * @brief     Get the controller's Constant Tone Extension (CTE) antenna switching and sampling capabilities.
 * @param[out] antenna_info: Output buffer for the controller's antenna capabilities.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_get_antenna_info(rtk_bt_le_gap_antenna_info_t *antenna_info);

#if ((defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV) && \
    (defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT))
/**
 * @brief     Configure and start a connectionless CTE transmitter, simultaneously enabling Extended Advertising and Periodic Advertising on a newly allocated advertising
 *            set. The CTE is embedded in periodic advertising PDUs. The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_CONNLESS_CTE_TX_STATE_IND.
 * @param[in] p_cte_param: Connectionless CTE transmit parameters.
 *                         Must not be NULL.
 * @param[in] p_eadv_param: Extended advertising parameters.
 *                          Must not be NULL.
 * @param[in] p_pa_param: Periodic advertising parameters.
 *                        Must not be NULL.
 * @param[out] p_adv_handle: Pointer to a variable that receives the allocated advertising set handle.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_connless_cte_tx_start(rtk_bt_le_gap_connless_cte_tx_param_t *p_cte_param,
											 rtk_bt_le_ext_adv_param_t *p_eadv_param,
											 rtk_bt_le_pa_param_t *p_pa_param,
											 uint8_t *p_adv_handle);

/**
 * @brief     Stop the connectionless CTE transmitter on the specified extended advertising set, disabling the CTE in periodic advertising PDUs.
 * @param[in] adv_handle: Extended advertising set handle previously returned by @ref rtk_bt_le_gap_connless_cte_tx_start.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This function also stops the associated Periodic Advertising and Extended Advertising on the specified handle.
 *            - The adv_handle is released after this call.
 */
uint16_t rtk_bt_le_gap_connless_cte_tx_stop(uint8_t adv_handle);

#endif /* RTK_BLE_5_0_USE_EXTENDED_ADV && RTK_BLE_5_0_PA_ADV_SUPPORT */

/**
 * @brief     Enable connectionless CTE reception on a Periodic Advertising synchronization train identified by @p sync_id. The controller will sample IQ data
 *            from received CTE packets and deliver results via event @ref RTK_BT_LE_GAP_EVT_CONNLESS_IQ_REPORT_IND.
 * @param[in] sync_id: Periodic advertising synchronization ID identifying the train to receive CTE from.
 *                     Obtain from @ref RTK_BT_LE_GAP_EVT_PA_SYNC_STATE_IND after sync is established.
 * @param[in] params: Connectionless CTE receive parameters.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - A valid PA sync must exist for @p sync_id before calling this function.
 *            - The advertiser must be transmitting CTE-bearing periodic advertising PDUs.
 */
uint16_t rtk_bt_le_gap_connless_cte_rx_start(uint8_t sync_id, rtk_bt_le_gap_connless_cte_rx_param_t *params);

/**
 * @brief     Disable connectionless CTE reception on the Periodic Advertising synchronization train identified by @p sync_id.
 *            After this call, no further CTE IQ reports are generated for that sync train.
 * @param[in] sync_id: Periodic advertising synchronization ID previously used in @ref rtk_bt_le_gap_connless_cte_rx_start.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_connless_cte_rx_stop(uint8_t sync_id);

/**
 * @brief     Enable connection-oriented CTE transmission on an established ACL connection. The controller will append CTE to transmitted packets according to @p params.
 *            CTE transmission continues until stopped by @ref rtk_bt_le_gap_conn_cte_tx_stop.
 * @param[in] conn_handle: Connection handle of the ACL connection to enable CTE TX.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] params: Pointer to connection-oriented CTE transmit parameters.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_conn_cte_tx_start(uint16_t conn_handle, rtk_bt_le_gap_conn_cte_tx_param_t *params);

/**
 * @brief     Disable connection-oriented CTE transmission on the specified ACL connection.
 *            After this call the controller stops appending CTE to transmitted packets on the connection.
 * @param[in] conn_handle: Connection handle of the ACL connection to stop CTE TX.
 *                         Range: 0x0000 to 0x0EFF.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_conn_cte_tx_stop(uint16_t conn_handle);

/**
 * @brief     Enable connection-oriented CTE reception on an established ACL connection. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_GAP_EVT_CONN_IQ_REPORT_IND.
 * @param[in] conn_handle: Connection handle of the ACL connection to enable CTE RX.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] rx_params: Pointer to connection-oriented CTE receive parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_conn_cte_rx_start(uint16_t conn_handle,
										 rtk_bt_le_gap_conn_cte_rx_param_t *rx_params);

/**
 * @brief     Stop connection-oriented CTE reception on the specified ACL connection.
 *            After this call, no further CTE IQ reports are generated.
 * @param[in] conn_handle: Connection handle of the ACL connection to stop CTE RX.
 *                         Range: 0x0000 to 0x0EFF.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_conn_cte_rx_stop(uint16_t conn_handle);
#endif /* RTK_BLE_5_1_CTE_SUPPORT */

#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
/**
 * @brief     Register or unregister a LE Protocol/Service Multiplexer (PSM) value for a LE Credit Based Connection-Oriented Channel (COC).
 * @param[in] is_register: 1: register the PSM;
 *                         0: unregister the PSM.
 * @param[in] le_psm: LE PSM value to register or unregister.
 *                    Valid range: 0x0001-0x00FF.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The PSM must be registered before any remote device can establish a COC channel to it.
 */
uint16_t rtk_bt_le_gap_coc_register_psm(uint8_t is_register, uint16_t le_psm);

/**
 * @brief     Configure security requirements for a registered LE COC PSM, specifying the authentication and encryption mode required before a COC connection on that PSM
 *            can be accepted.
 * @param[in] le_psm: LE PSM value whose security is being configured.
 *                    Must be a registered PSM.
 * @param[in] active: 1: Allocate the specified security entry.
 *                    0: Release the specified security entry.
 * @param[in] sec_mode: COC security mode.
 * @param[in] key_size: Required encryption key size in bytes. Usually set as 16 (common encryption key size).
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The PSM must be registered via @ref rtk_bt_le_gap_coc_register_psm before setting its security policy.
 */
uint16_t rtk_bt_le_gap_coc_set_psm_security(uint16_t le_psm, uint8_t active,
											rtk_bt_le_coc_security_mode_t sec_mode,
											uint8_t key_size);

/**
 * @brief     Set an LE COC protocol parameter that applies to all subsequent COC channel operations.
 * @param[in] param_type: Parameter type identifier.
 * @param[in] value: Parameter value. The valid range depends on the parameter type.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_coc_set_param(rtk_bt_le_coc_param_type_t param_type, uint16_t value);

/**
 * @brief     Get a parameter from an established LE COC channel identified by its channel ID (cid) and return the value via @p value.
 * @param[in] param_type: Channel parameter type to get.
 * @param[in] cid: COC channel ID of the established channel.
 * @param[out] value: Pointer to a variable that receives the parameter value.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_coc_get_chan_param(rtk_bt_le_coc_chan_param_type_t param_type,
										  uint16_t cid, uint16_t *value);

/**
 * @brief     Initiate an LE Credit Based COC connection request to a remote device over an existing ACL connection on the specified PSM. The result is reported
 *            asynchronously via event @ref RTK_BT_LE_GAP_EVT_COC_CONNECT_IND.
 * @param[in] conn_handle: Connection handle of the ACL connection to send COC connection request.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] le_psm: LE PSM value to connect to on the remote device.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - An ACL connection identified by @p conn_handle must exist before calling this function.
 */
uint16_t rtk_bt_le_gap_coc_connect(uint16_t conn_handle, uint16_t le_psm);

/**
 * @brief     Initiate disconnection of an established LE COC channel identified by @p cid.
 *            The result is reported asynchronously via event @ref RTK_BT_LE_GAP_EVT_COC_DISCONNECT_IND.
 * @param[in] cid: COC channel ID of the channel to disconnect.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The underlying ACL connection is not affected; only the COC channel is closed.
 *            - After disconnection @p cid is no longer valid.
 */
uint16_t rtk_bt_le_gap_coc_disconnect(uint16_t cid);

/**
 * @brief     Send a data SDU over an established LE COC channel. The transmission result is reported asynchronously
 *            via event @ref RTK_BT_LE_GAP_EVT_COC_SEND_DATA_RESULT_IND.
 * @param[in] cid: COC channel ID of the channel to send data.
 * @param[in] len: Length of the data payload in bytes.
 * @param[in] data: Pointer to the data buffer to transmit.
 *                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_gap_coc_send_data(uint16_t cid, uint16_t len, uint8_t *data);
#endif /* RTK_BLE_COC_SUPPORT */

/**
 * @brief     Configure and start an LE Direct Test Mode (DTM) receiver test using the specified parameters.
 * @param[in] p_rx_param: DTM receiver test parameter.
 *                        Must not be NULL.
 * @param[in] rx_version: HCI receiver test command version to use (v1, v2, or v3).
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This function must be called after BT init.
 *            - DO NOT execute ADV / Scan / Connection related actions before calling this API.
 *            - After calling this API, ADV/Scan/Connection operations must NOT be performed until BT deinit and BT init are performed again.
 */
uint16_t rtk_bt_le_gap_dtm_rx_test(rtk_bt_le_dtm_rx_param_t *p_rx_param, rtk_bt_le_dtm_rx_version_t rx_version);

/**
 * @brief     Configure and start an LE Direct Test Mode (DTM) transmitter test using the specified parameters.
 * @param[in] p_tx_param: DTM transmitter test parameter.
 *                        Must not be NULL.
 * @param[in] tx_version: HCI transmitter test command version to use.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This function must be called after BT init.
 *            - DO NOT execute ADV / Scan / Connection related actions before calling this API.
 *            - After calling this API, ADV/Scan/Connection operations must NOT be performed until BT deinit and BT init are performed again.
 */
uint16_t rtk_bt_le_gap_dtm_tx_test(rtk_bt_le_dtm_tx_param_t *p_tx_param, rtk_bt_le_dtm_tx_version_t tx_version);

/**
 * @brief     Stop an active LE Direct Test Mode (DTM) test (RX or TX) and get the number of test packets received during the test. The packet count is meaningful only
 *            for RX tests.
 * @param[out] p_num_pkts: Pointer to a uint16_t that receives the number of test packets received during the DTM RX test.
 *                         For DTM TX tests the value is always 0.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This function must be called after BT init and after an active DTM RX or TX test.
 *            - DO NOT execute ADV / Scan / Connection related actions before calling this API.
 *            - After calling this API, BT deinit followed by BT init must be performed before resuming ADV/Scan/Connection operations.
 */
uint16_t rtk_bt_le_gap_dtm_end(uint16_t *p_num_pkts);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_LE_GAP_H__ */
