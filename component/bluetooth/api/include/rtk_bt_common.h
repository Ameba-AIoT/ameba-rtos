/**
 * @file      rtk_bt_common.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth common type and function definition.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_COMMON_H__
#define __RTK_BT_COMMON_H__

#include <dlist.h>
#include <basic_types.h>
#include <bt_app_config.h>
#include <bt_debug.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define RTK_BT_API_LE_BASE          0x0     /*!< Base value of the LE API command/event groups. */
#define RTK_BT_API_BR_BASE          0x80    /*!< Base value of the BR/EDR API command/event groups. */
#define RTK_BT_API_COMMON_BASE      0xE0    /*!< Base value of the common API command/event groups. */
#define RTK_BT_API_TASK_EXIT        0xFF    /*!< Message value that makes the API task exit. */
#define RTK_BT_EVENT_TASK_EXIT      0xFF    /*!< Event group value that makes the BT event task exit. */
#define RTK_BT_DRC_EVENT_TASK_EXIT  0xFE    /*!< Reserved task exit value of the direct calling event, currently not used. */

/* BT Protocol/Profile UUID definitions. */
#define RTK_BT_UUID_SDP                            0x0001     /*!< SDP. */
#define RTK_BT_UUID_UDP                            0x0002     /*!< UDP. */
#define RTK_BT_UUID_RFCOMM                         0x0003     /*!< RFCOMM. */
#define RTK_BT_UUID_TCP                            0x0004     /*!< TCP. */
#define RTK_BT_UUID_TCSBIN                         0x0005     /*!< TCS-BIN. */
#define RTK_BT_UUID_TCSAT                          0x0006     /*!< TCS-AT. */
#define RTK_BT_UUID_ATT                            0x0007     /*!< ATT. */
#define RTK_BT_UUID_OBEX                           0x0008     /*!< OBEX. */
#define RTK_BT_UUID_IP                             0x0009     /*!< IP. */
#define RTK_BT_UUID_FTP                            0x000A     /*!< FTP. */
#define RTK_BT_UUID_HTTP                           0x000C     /*!< HTTP. */
#define RTK_BT_UUID_WSP                            0x000E     /*!< WSP. */
#define RTK_BT_UUID_BNEP                           0x000F     /*!< BNEP. */
#define RTK_BT_UUID_UPNP                           0x0010     /*!< UPNP. */
#define RTK_BT_UUID_HIDP                           0x0011     /*!< HID Protocol. */
#define RTK_BT_UUID_HC_CONTROL                     0x0012     /*!< HardcopyControlChannel. */
#define RTK_BT_UUID_HC_DATA                        0x0014     /*!< HardcopyDataChannel. */
#define RTK_BT_UUID_HC_NOTIFICATION                0x0016     /*!< HardcopyNotificationChannel. */
#define RTK_BT_UUID_AVCTP                          0x0017     /*!< AVCTP. */
#define RTK_BT_UUID_AVDTP                          0x0019     /*!< AVDTP. */
#define RTK_BT_UUID_CMTP                           0x001B     /*!< CMTP. */
#define RTK_BT_UUID_HDP_CTRL_CHANNEL               0x001E     /*!< MCAP Control Channel. */
#define RTK_BT_UUID_HDP_DATA_CHANNEL               0x001F     /*!< MCAP Data Channel. */
#define RTK_BT_UUID_RDTP                           0x00D1     /*!< RDTP. */

#define RTK_BT_UUID_L2CAP                          0x0100     /*!< L2CAP. */

#define RTK_BT_UUID_SERVICE_DISCOVERY_SERVER       0x1000     /*!< ServiceDiscoveryServerServiceClassID. */
#define RTK_BT_UUID_BROWSE_GROUP_DESCRIPTOR        0x1001     /*!< BrowseGroupDescriptorServiceClassID. */
#define RTK_BT_UUID_PUBLIC_BROWSE_GROUP            0x1002     /*!< PublicBrowseRoot. */
#define RTK_BT_UUID_SERIAL_PORT                    0x1101     /*!< Serial Port. */
#define RTK_BT_UUID_LAN_ACCESS                     0x1102     /*!< LANAccessUsingPPP. */
#define RTK_BT_UUID_DIAL_UP_NETWORKING             0x1103     /*!< Dial-up Networking. */
#define RTK_BT_UUID_IRMC_SYNC                      0x1104     /*!< IrMCSync. */
#define RTK_BT_UUID_OBEX_OBJECT_PUSH               0x1105     /*!< OBEXObjectPush. */
#define RTK_BT_UUID_OBEX_FILE_TRANSFER             0x1106     /*!< OBEX File Transfer. */
#define RTK_BT_UUID_IRM_SYNC_COMMAND               0x1107     /*!< IrMCSyncCommand. */
#define RTK_BT_UUID_HEADSET                        0x1108     /*!< Headset. */
#define RTK_BT_UUID_HEADSET_HS                     0x1131     /*!< Headset HS. */
#define RTK_BT_UUID_CORDLESS_TELEPHONY             0x1109     /*!< CordlessTelephony. */
#define RTK_BT_UUID_AUDIO_SOURCE                   0x110A     /*!< Audio Source. */
#define RTK_BT_UUID_AUDIO_SINK                     0x110B     /*!< Audio Sink. */
#define RTK_BT_UUID_AV_REMOTE_CONTROL_TARGET       0x110C     /*!< A/V Remote Control Target. */
#define RTK_BT_UUID_ADVANCED_AUDIO_DISTRIBUTION    0x110D     /*!< Advanced Audio Distribution. */
#define RTK_BT_UUID_AV_REMOTE_CONTROL              0x110E     /*!< A/V Remote Control. */
#define RTK_BT_UUID_AV_REMOTE_CONTROL_CONTROLLER   0x110F     /*!< A/V Remote Control Controller. */
#define RTK_BT_UUID_INTERCOM                       0x1110     /*!< Intercom. */
#define RTK_BT_UUID_FAX                            0x1111     /*!< Fax. */
#define RTK_BT_UUID_HEADSET_AUDIO_GATEWAY          0x1112     /*!< Headset Audio Gateway. */
#define RTK_BT_UUID_WAP                            0x1113     /*!< WAP. */
#define RTK_BT_UUID_WAP_CLIENT                     0x1114     /*!< WAP_CLIENT. */
#define RTK_BT_UUID_PANU                           0x1115     /*!< PANU. */
#define RTK_BT_UUID_NAP                            0x1116     /*!< NAP. */
#define RTK_BT_UUID_GN                             0x1117     /*!< GN. */
#define RTK_BT_UUID_DIRECT_PRINTING                0x1118     /*!< DirectPrinting. */
#define RTK_BT_UUID_REFERENCE_PRINTING             0x1119     /*!< ReferencePrinting. */
#define RTK_BT_UUID_IMAGING                        0x111A     /*!< Imaging. */
#define RTK_BT_UUID_IMAGING_RESPONDER              0x111B     /*!< Imaging Responder. */
#define RTK_BT_UUID_IMAGING_AUTOMATIC_ARCHIVE      0x111C     /*!< Imaging Automatic Archive. */
#define RTK_BT_UUID_REFERENCED_OBJECTS             0x111D     /*!< Imaging Referenced Objects. */
#define RTK_BT_UUID_HANDSFREE                      0x111E     /*!< Hands-Free. */
#define RTK_BT_UUID_HANDSFREE_AUDIO_GATEWAY        0x111F     /*!< AG Hands-Free. */
#define RTK_BT_UUID_DIRECT_PRINTING_REF_OBJECTS    0x1120     /*!< DirectPrintingReferenceObjects. */
#define RTK_BT_UUID_REFLECTED_UI                   0x1121     /*!< ReflectedUI. */
#define RTK_BT_UUID_BASIC_PRINTING                 0x1122     /*!< BasicPrinting. */
#define RTK_BT_UUID_PRINTING_STATUS                0x1123     /*!< PrintingStatus. */
#define RTK_BT_UUID_HUMAN_INTERFACE_DEVICE_SERVICE 0x1124     /*!< HID. */
#define RTK_BT_UUID_HARDCOPY_CABLE_REPLACEMENT     0x1125     /*!< HardcopyCableReplacement. */
#define RTK_BT_UUID_HCR_PRINT                      0x1126     /*!< HCR_Print. */
#define RTK_BT_UUID_HCR_SCAN                       0x1127     /*!< HCR_Scan. */
#define RTK_BT_UUID_COMMON_ISDN_ACCESS             0x1128     /*!< Common_ISDN_Access. */
#define RTK_BT_UUID_VIDEO_CONFERENCING_GW          0x1129     /*!< Video Conferencing Gateway. */
#define RTK_BT_UUID_SIM                            0x112D     /*!< SIM Access. */
#define RTK_BT_UUID_PBAP_PCE                       0x112E     /*!< PhoneBook Access Client. */
#define RTK_BT_UUID_PBAP_PSE                       0x112F     /*!< PhoneBook Access Server. */
#define RTK_BT_UUID_PBAP                           0x1130     /*!< PhoneBook Access Profile. */
#define RTK_BT_UUID_MSG_ACCESS_SERVER              0x1132     /*!< Message Access Server. */
#define RTK_BT_UUID_MSG_NOTIFICATION_SERVER        0x1133     /*!< Message Notification Server. */
#define RTK_BT_UUID_MSG_ACCESS_PROFILE             0x1134     /*!< Message Access profile. */
#define RTK_BT_UUID_PNP_INFORMATION                0x1200     /*!< PnPInformation. */
#define RTK_BT_UUID_GENERIC_NETWORKING             0x1201     /*!< Generic Networking. */
#define RTK_BT_UUID_GENERIC_FILE_TRANSFER          0x1202     /*!< GenericFile Transfe. */
#define RTK_BT_UUID_GENERIC_AUDIO                  0x1203     /*!< Generic Audio. */
#define RTK_BT_UUID_GENERIC_TELEPHONY              0x1204     /*!< GenericTelephony. */
#define RTK_BT_UUID_HDP_PROFILE                    0x1400     /*!< HDP. */
#define RTK_BT_UUID_HDP_PROFILE_SOURCE             0x1401     /*!< HDP Source. */
#define RTK_BT_UUID_HDP_PROFILE_SINK               0x1402     /*!< HDP Sink. */
#define RTK_BT_UUID_GAP                            0x1800     /*!< GAP. */
#define RTK_BT_UUID_GATT                           0x1801     /*!< GATT. */

/* BT Protocol/Service Multiplexer definitions. */
#define RTK_BT_PSM_SDP                             0x0001     /*!< SDP. */
#define RTK_BT_PSM_RFCOMM                          0x0003     /*!< RFCOMM. */
#define RTK_BT_PSM_HID_CONTROL                     0x0011     /*!< HID_control. */
#define RTK_BT_PSM_HID_INTERRUPT                   0x0013     /*!< HID_interrupt. */
#define RTK_BT_PSM_AVCTP                           0x0017     /*!< AVCTP. */
#define RTK_BT_PSM_AVDTP                           0x0019     /*!< AVDTP. */
#define RTK_BT_PSM_AVCTP_BROWSING                  0x001B     /*!< AVCTP_browsing. */
#define RTK_BT_PSM_ATT                             0x001F     /*!< ATT. */
#define RTK_BT_PSM_RDTP                            0x00D1     /*!< RDTP. */
#define RTK_BT_PSM_EATT                            0x0027     /*!< EATT. */

/**
 * @typedef   rtk_bt_profile_t
 * @brief     Bluetooth profile type bit mask.
 */
typedef enum {
	RTK_BT_PROFILE_GATTS    =  BIT0,     /*!< GATT Server. */
	RTK_BT_PROFILE_GATTC    =  BIT1,     /*!< GATT Client. */
	RTK_BT_PROFILE_A2DP     =  BIT2,     /*!< Advanced Audio Distribution Profile. */
	RTK_BT_PROFILE_AVRCP    =  BIT3,     /*!< Audio/Video Remote Control Profile. */
	RTK_BT_PROFILE_SDP      =  BIT4,     /*!< Service Discovery Protocol. */
	RTK_BT_PROFILE_LEAUDIO  =  BIT5,     /*!< LE Audio. */
	RTK_BT_PROFILE_LE_ISO   =  BIT6,     /*!< LE Isochronous Channels. */
	RTK_BT_PROFILE_BAP      =  BIT7,     /*!< Basic Audio Profile. */
	RTK_BT_PROFILE_CAP      =  BIT8,     /*!< Common Audio Profile. */
	RTK_BT_PROFILE_SPP      =  BIT9,     /*!< Serial Port Profile. */
	RTK_BT_PROFILE_RFC      =  BIT10,    /*!< RFCOMM protocol. */
	RTK_BT_PROFILE_HFP      =  BIT11,    /*!< Hands-Free Profile. */
	RTK_BT_PROFILE_PBAP     =  BIT12,    /*!< Phone Book Access Profile. */
	RTK_BT_PROFILE_MESH     =  BIT13,    /*!< Bluetooth Mesh. */
	RTK_BT_PROFILE_HID      =  BIT14,    /*!< Human Interface Device Profile. */
	RTK_BT_PROFILE_TMAP     =  BIT15,    /*!< Telephony and Media Audio Profile. */
	RTK_BT_PROFILE_GMAP     =  BIT16,    /*!< Gaming Audio Profile. */
} rtk_bt_profile_t;

/************************************** BLE group **************************************/

/**
 * @typedef   rtk_bt_le_group_t
 * @brief     Bluetooth LE action/event group.
 */
typedef enum {
	RTK_BT_LE_GP_GAP = RTK_BT_API_LE_BASE,                             /*!< LE GAP group. */
	RTK_BT_LE_GP_GATTC,                                                /*!< LE GATT Client group. */
	RTK_BT_LE_GP_GATTS,                                                /*!< LE GATT Server group. */
	RTK_BT_LE_GP_ISO,                                                  /*!< LE ISO group. */
	RTK_BT_LE_GP_BAP,                                                  /*!< LE Audio BAP group. */
	RTK_BT_LE_GP_CAP,                                                  /*!< LE Audio CAP group. */
	RTK_BT_LE_GP_TMAP,                                                 /*!< LE Audio TMAP group. */
	RTK_BT_LE_GP_GMAP,                                                 /*!< LE Audio GMAP group. */
	RTK_BT_LE_GP_MESH_STACK,                                           /*!< LE Mesh stack group. */
	RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL,                             /*!< LE Mesh config client model group. */
	RTK_BT_LE_GP_MESH_GENERIC_ONOFF_CLIENT_MODEL,                      /*!< LE Mesh generic OnOff client model group. */
	RTK_BT_LE_GP_MESH_GENERIC_ONOFF_SERVER_MODEL,                      /*!< LE Mesh generic OnOff server model group. */
	RTK_BT_LE_GP_MESH_DATATRANS_MODEL,                                 /*!< LE Mesh Datatrans model group. */
	RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL,                        /*!< LE Mesh remote provisioning client model group. */
	RTK_BT_LE_GP_MESH_REMOTE_PROV_SERVER_MODEL,                        /*!< LE Mesh remote provisioning server model group. */
	RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL,                    /*!< LE Mesh light lightness client model group. */
	RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL,                          /*!< LE Mesh light CTL client model group. */
	RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL,                          /*!< LE Mesh light HSL client model group. */
	RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL,                          /*!< LE Mesh light XYL client model group. */
	RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL,                           /*!< LE Mesh light LC client model group. */
	RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SERVER_MODEL,                    /*!< LE Mesh light lightness server model group. */
	RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL,              /*!< LE Mesh light lightness setup server model group. */
	RTK_BT_LE_GP_MESH_LIGHT_CTL_SERVER_MODEL,                          /*!< LE Mesh light CTL server model group. */
	RTK_BT_LE_GP_MESH_LIGHT_CTL_SETUP_SERVER_MODEL,                    /*!< LE Mesh light CTL setup server model group. */
	RTK_BT_LE_GP_MESH_LIGHT_CTL_TEMPERATURE_SERVER_MODEL,              /*!< LE Mesh light CTL temperature server model group. */
	RTK_BT_LE_GP_MESH_LIGHT_HSL_SERVER_MODEL,                          /*!< LE Mesh light HSL server model group. */
	RTK_BT_LE_GP_MESH_LIGHT_HSL_HUE_SERVER_MODEL,                      /*!< LE Mesh light HSL hue server model group. */
	RTK_BT_LE_GP_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL,               /*!< LE Mesh light HSL saturation server model group. */
	RTK_BT_LE_GP_MESH_LIGHT_HSL_SETUP_SERVER_MODEL,                    /*!< LE Mesh light HSL setup server model group. */
	RTK_BT_LE_GP_MESH_LIGHT_XYL_SERVER_MODEL,                          /*!< LE Mesh light XYL server model group. */
	RTK_BT_LE_GP_MESH_LIGHT_XYL_SETUP_SERVER_MODEL,                    /*!< LE Mesh light XYL setup server model group. */
	RTK_BT_LE_GP_MESH_LIGHT_LC_SERVER_MODEL,                           /*!< LE Mesh light LC server model group. */
	RTK_BT_LE_GP_MESH_LIGHT_LC_SETUP_SERVER_MODEL,                     /*!< LE Mesh light LC setup server model group. */
	RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL,                               /*!< LE Mesh time client model group. */
	RTK_BT_LE_GP_MESH_TIME_SERVER_MODEL,                               /*!< LE Mesh time server model group. */
	RTK_BT_LE_GP_MESH_TIME_SETUP_SERVER_MODEL,                         /*!< LE Mesh time setup server model group. */
	RTK_BT_LE_GP_MESH_SCHEDULER_CLIENT_MODEL,                          /*!< LE Mesh scheduler client model group. */
	RTK_BT_LE_GP_MESH_SCHEDULER_SERVER_MODEL,                          /*!< LE Mesh scheduler server model group. */
	RTK_BT_LE_GP_MESH_SCHEDULER_SETUP_SERVER_MODEL,                    /*!< LE Mesh scheduler setup server model group. */
	RTK_BT_LE_GP_MESH_SCENE_CLIENT_MODEL,                              /*!< LE Mesh scene client model group. */
	RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL,                              /*!< LE Mesh scene server model group. */
	RTK_BT_LE_GP_MESH_SCENE_SETUP_SERVER_MODEL,                        /*!< LE Mesh scene setup server model group. */
	RTK_BT_LE_GP_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL,    /*!< LE Mesh generic default transition time client model group. */
	RTK_BT_LE_GP_MESH_GENERIC_DEFAULT_TRANSITION_TIME_SERVER_MODEL,    /*!< LE Mesh generic default transition time server model group. */
	RTK_BT_LE_GP_MESH_GENERIC_LEVEL_CLIENT_MODEL,                      /*!< LE Mesh generic level client model group. */
	RTK_BT_LE_GP_MESH_GENERIC_LEVEL_SERVER_MODEL,                      /*!< LE Mesh generic level server model group. */
	RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_CLIENT_MODEL,               /*!< LE Mesh generic power OnOff client model group. */
	RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_SERVER_MODEL,               /*!< LE Mesh generic power OnOff server model group. */
	RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL,         /*!< LE Mesh generic power OnOff setup server model group. */
	RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL,                /*!< LE Mesh generic power level client model group. */
	RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL,                /*!< LE Mesh generic power level server model group. */
	RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL,          /*!< LE Mesh generic power level setup server model group. */
	RTK_BT_LE_GP_MESH_GENERIC_BATTERY_CLIENT_MODEL,                    /*!< LE Mesh generic battery client model group. */
	RTK_BT_LE_GP_MESH_GENERIC_BATTERY_SERVER_MODEL,                    /*!< LE Mesh generic battery server model group. */
	RTK_BT_LE_GP_MESH_GENERIC_LOCATION_CLIENT_MODEL,                   /*!< LE Mesh generic location client model group. */
	RTK_BT_LE_GP_MESH_GENERIC_LOCATION_SERVER_MODEL,                   /*!< LE Mesh generic location server model group. */
	RTK_BT_LE_GP_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL,             /*!< LE Mesh generic location setup server model group. */
	RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL,                   /*!< LE Mesh generic property client model group. */
	RTK_BT_LE_GP_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL,              /*!< LE Mesh generic user property server model group. */
	RTK_BT_LE_GP_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL,             /*!< LE Mesh generic admin property server model group. */
	RTK_BT_LE_GP_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL,              /*!< LE Mesh generic manufacturer property server model group. */
	RTK_BT_LE_GP_MESH_GENERIC_CLIENT_PROPERTY_SERVER_MODEL,            /*!< LE Mesh generic client property server model group. */
	RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL,                             /*!< LE Mesh sensor client model group. */
	RTK_BT_LE_GP_MESH_SENSOR_SERVER_MODEL,                             /*!< LE Mesh sensor server model group. */
	RTK_BT_LE_GP_MESH_SENSOR_SETUP_SERVER_MODEL,                       /*!< LE Mesh sensor setup server model group. */
	RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL,                             /*!< LE Mesh health client model group. */
	RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL,                             /*!< LE Mesh health server model group. */
	RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL,                /*!< LE Mesh directed forwarding client model group. */
	RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON,                      /*!< LE Mesh directed forwarding common group. */
	RTK_BT_LE_GP_MESH_SUBNET_BRIDGE_CLIENT_MODEL,                      /*!< LE Mesh subnet bridge client model group. */
	RTK_BT_LE_GP_MESH_PRIVATE_BEACON_CLIENT_MODEL,                     /*!< LE Mesh private beacon client model group. */
	RTK_BT_LE_GP_MESH_DFU_INITIATOR_MODEL,                             /*!< LE Mesh DFU initiator model group. */
	RTK_BT_LE_GP_MESH_DFU_STANDALONE_UPDATER_MODEL,                    /*!< LE Mesh DFU standalone updater model group. */
	RTK_BT_LE_GP_MESH_DFU_DISTRIBUTOR_MODEL,                           /*!< LE Mesh DFU distributor model group. */
	RTK_BT_LE_GP_MESH_DFU_TARGET_MODEL,                                /*!< LE Mesh DFU target model group. */
	RTK_BT_LE_GP_MAX,                                                  /*!< Reserved. */
} rtk_bt_le_group_t;

/**
 * @typedef   rtk_bt_le_gap_act_t
 * @brief     Bluetooth LE GAP action.
 */
typedef enum {
	RTK_BT_LE_GAP_ACT_GET_VERSION = 1,                      /*!< Get the local stack version information. */
	RTK_BT_LE_GAP_ACT_GET_DEVICE_ADDR,                      /*!< Get the local public device address. */
	RTK_BT_LE_GAP_ACT_SET_DEVICE_NAME,                      /*!< Set the GAP Device Name characteristic value. */
	RTK_BT_LE_GAP_ACT_SET_APPEARANCE,                       /*!< Set the GAP Appearance characteristic value. */
	RTK_BT_LE_GAP_ACT_SET_PREFERRED_CONN_PARAM,             /*!< Set the Peripheral Preferred Connection Parameters characteristic value. */
	RTK_BT_LE_GAP_ACT_SET_RAND_ADDR,                        /*!< Set the random address of the local device. */
	RTK_BT_LE_GAP_ACT_GEN_RAND_ADDR,                        /*!< Generate a random address of the specified type. */
	RTK_BT_LE_GAP_ACT_CFG_LOCAL_IDENT_ADDR,                 /*!< Configure the local identity address. */
	RTK_BT_LE_GAP_ACT_SET_ADV_DATA,                         /*!< Set the legacy advertising data. */
	RTK_BT_LE_GAP_ACT_SET_SCAN_RSP_DATA,                    /*!< Set the legacy scan response data. */
	RTK_BT_LE_GAP_ACT_START_ADV,                            /*!< Start legacy advertising. */
	RTK_BT_LE_GAP_ACT_STOP_ADV,                             /*!< Stop legacy advertising. */
	RTK_BT_LE_GAP_ACT_GET_ADV_PARAM,                        /*!< Get the configured advertising parameters. */
	RTK_BT_LE_GAP_ACT_SCAN_INFO_FILTER,                     /*!< Configure the host-side scan info filter. */
	RTK_BT_LE_GAP_ACT_SET_SCAN_PARAM,                       /*!< Set the legacy scan parameters. */
	RTK_BT_LE_GAP_ACT_GET_SCAN_PARAM,                       /*!< Get the legacy scan parameters. */
	RTK_BT_LE_GAP_ACT_START_SCAN,                           /*!< Start legacy scanning. */
	RTK_BT_LE_GAP_ACT_STOP_SCAN,                            /*!< Stop legacy scanning. */
	RTK_BT_LE_GAP_ACT_CONN,                                 /*!< Initiate a legacy LE connection. */
	RTK_BT_LE_GAP_ACT_CONN_CANCEL,                          /*!< Cancel an ongoing LE connection establishment. */
	RTK_BT_LE_GAP_ACT_DISCONN,                              /*!< Disconnect an LE connection. */
	RTK_BT_LE_GAP_ACT_DISCONN_WITH_REASON,                  /*!< Disconnect an LE connection with a specified reason code. */
	RTK_BT_LE_GAP_ACT_UPDATE_CONN_PARAM,                    /*!< Request an update of the LE connection parameters. */
	RTK_BT_LE_GAP_ACT_READ_RSSI,                            /*!< Read the RSSI of an LE connection. */
	RTK_BT_LE_GAP_ACT_READ_REMOTE_VERSION,                  /*!< Read the version information of the remote device. */
	RTK_BT_LE_GAP_ACT_MODIFY_WHITELIST,                     /*!< Modify the Filter Accept List. */
	RTK_BT_LE_GAP_ACT_GET_CONN_INFO,                        /*!< Get the information of an LE connection. */
	RTK_BT_LE_GAP_ACT_GET_DEV_STATE,                        /*!< Get the current GAP device state. */
	RTK_BT_LE_GAP_ACT_GET_ACTIVE_CONN,                      /*!< Get the number and handles of the active LE connections. */
	RTK_BT_LE_GAP_ACT_GET_CONN_HANDLE_BY_ADDR,              /*!< Get the connection handle by the peer device address. */
	RTK_BT_LE_GAP_ACT_GET_MTU_SIZE,                         /*!< Get the negotiated ATT MTU size of an LE connection. */
	RTK_BT_LE_GAP_ACT_SET_MAX_MTU_SIZE,                     /*!< Set the maximum ATT MTU size proposed by the local device. */
	RTK_BT_LE_GAP_ACT_SET_CHANNELS,                         /*!< Set the LE channel map. */
	RTK_BT_LE_GAP_ACT_SET_DATA_LEN,                         /*!< Request a data length update of an LE connection. */
	RTK_BT_LE_GAP_ACT_SET_PHY,                              /*!< Request a PHY update of an LE connection. */
	RTK_BT_LE_GAP_ACT_PRIVACY_INIT,                         /*!< Initialize LE privacy. */
	RTK_BT_LE_GAP_ACT_SET_PRIVACY_MODE,                     /*!< Set the privacy mode for a peer identity address. */
	RTK_BT_LE_GAP_ACT_READ_LOCAL_RESOLV_ADDR,               /*!< Read the local Resolvable Private Address used with a peer. */
	RTK_BT_LE_GAP_ACT_READ_PEER_RESOLV_ADDR,                /*!< Read the Resolvable Private Address used by a peer. */
	RTK_BT_LE_GAP_ACT_SET_PAIRING_MODE,                     /*!< Set the pairing mode of the Security Manager. */
	RTK_BT_LE_GAP_ACT_SET_SEC_PARAM,                        /*!< Set the security parameters of the Security Manager. */
	RTK_BT_LE_GAP_ACT_GET_SEC_PARAM,                        /*!< Get the security parameters of the Security Manager. */
	RTK_BT_LE_GAP_ACT_START_SECURITY,                       /*!< Initiate or request a security upgrade of an LE connection. */
	RTK_BT_LE_GAP_ACT_PAIRING_CONFIRM,                      /*!< Respond to a Just Works pairing confirmation request. */
	RTK_BT_LE_GAP_ACT_PASSKEY_ENTRY,                        /*!< Provide the passkey entered by the user. */
	RTK_BT_LE_GAP_ACT_PASSKEY_CONFIRM,                      /*!< Confirm or reject the displayed passkey. */
	RTK_BT_LE_GAP_ACT_OOB_KEY_INPUT,                        /*!< Provide the OOB temporary key for legacy OOB pairing. */
	RTK_BT_LE_GAP_ACT_GET_SC_LOCAL_OOB,                     /*!< Get the local OOB data for Secure Connections pairing. */
	RTK_BT_LE_GAP_ACT_INPUT_SC_PEER_OOB,                    /*!< Provide the peer OOB data for Secure Connections pairing. */
	RTK_BT_LE_GAP_ACT_GET_BOND_NUM,                         /*!< Get the number of bonded devices. */
	RTK_BT_LE_GAP_ACT_GET_BOND_INFO,                        /*!< Get the information of the bonded devices. */
	RTK_BT_LE_GAP_ACT_DELETE_BOND_DEVICE,                   /*!< Delete a bonded device. */
	RTK_BT_LE_GAP_ACT_CLEAR_BOND_LIST,                      /*!< Clear all bonded devices. */
	RTK_BT_LE_GAP_ACT_SET_EXT_ADV_DATA,                     /*!< Set the extended advertising data. */
	RTK_BT_LE_GAP_ACT_SET_EXT_SCAN_RSP_DATA,                /*!< Set the extended scan response data. */
	RTK_BT_LE_GAP_ACT_CREATE_EXT_ADV,                       /*!< Create an extended advertising set. */
	RTK_BT_LE_GAP_ACT_START_EXT_ADV,                        /*!< Start extended advertising. */
	RTK_BT_LE_GAP_ACT_STOP_EXT_ADV,                         /*!< Stop extended advertising. */
	RTK_BT_LE_GAP_ACT_REMOVE_EXT_ADV,                       /*!< Remove an extended advertising set. */
	RTK_BT_LE_GAP_ACT_GET_EXT_ADV_HANDLE_BY_CONN_HANDLE,    /*!< Get the extended advertising handle by the connection handle. */
	RTK_BT_LE_GAP_ACT_EXT_SCAN_SET_PARAM,                   /*!< Set the extended scan parameters. */
	RTK_BT_LE_GAP_ACT_EXT_SCAN_START,                       /*!< Start extended scanning. */
	RTK_BT_LE_GAP_ACT_EXT_SCAN_STOP,                        /*!< Stop extended scanning. */
	RTK_BT_LE_GAP_ACT_EXT_CONN,                             /*!< Initiate an extended LE connection. */
	RTK_BT_LE_GAP_ACT_START_PA,                             /*!< Start periodic advertising. */
	RTK_BT_LE_GAP_ACT_STOP_PA,                              /*!< Stop periodic advertising. */
	RTK_BT_LE_GAP_ACT_UPDATE_PA,                            /*!< Update the periodic advertising data. */
	RTK_BT_LE_GAP_ACT_PA_SYNC_MODIFY_ADV_LIST,              /*!< Modify the Periodic Advertiser List. */
	RTK_BT_LE_GAP_ACT_PA_SYNC_CREATE,                       /*!< Create a synchronization to a periodic advertising train. */
	RTK_BT_LE_GAP_ACT_PA_SYNC_TERMINATE,                    /*!< Terminate or cancel a periodic advertising synchronization. */
	RTK_BT_LE_GAP_ACT_PAST_TRANSFER,                        /*!< Transfer periodic advertising sync information to a connected peer. */
	RTK_BT_LE_GAP_ACT_PAST_RECV_SET,                        /*!< Set the PAST reception parameters of a connection. */
	RTK_BT_LE_GAP_ACT_DEFAULT_PAST_RECV_SET,                /*!< Set the default PAST reception parameters. */
	RTK_BT_LE_GAP_ACT_GET_TX_PENDING_NUM,                   /*!< Get the number of pending TX packets of a connection. */
	RTK_BT_LE_GAP_ACT_READ_LOCAL_TX_POWER,                  /*!< Read the local TX power level. */
	RTK_BT_LE_GAP_ACT_READ_REMOTE_TX_POWER,                 /*!< Read the remote TX power level. */
	RTK_BT_LE_GAP_ACT_TX_POWER_REPORT_SET,                  /*!< Enable or disable TX power level reporting. */
	RTK_BT_LE_GAP_ACT_GET_CONN_ID,                          /*!< Get the connection ID by the connection handle. */
	RTK_BT_LE_GAP_ACT_ADV_STATE,                            /*!< Check whether the advertising state is idle. */
	RTK_BT_LE_GAP_ACT_GET_ANTENNA_INFO,                     /*!< Get the antenna information of the controller. */
	RTK_BT_LE_GAP_ACT_CONNLESS_CTE_TX_START,                /*!< Start connectionless CTE transmission. */
	RTK_BT_LE_GAP_ACT_CONNLESS_CTE_TX_STOP,                 /*!< Stop connectionless CTE transmission. */
	RTK_BT_LE_GAP_ACT_CONNLESS_CTE_RX_START,                /*!< Start connectionless CTE reception. */
	RTK_BT_LE_GAP_ACT_CONNLESS_CTE_RX_STOP,                 /*!< Stop connectionless CTE reception. */
	RTK_BT_LE_GAP_ACT_CONN_CTE_TX_START,                    /*!< Start connection-oriented CTE transmission. */
	RTK_BT_LE_GAP_ACT_CONN_CTE_TX_STOP,                     /*!< Stop connection-oriented CTE transmission. */
	RTK_BT_LE_GAP_ACT_CONN_CTE_RX_START,                    /*!< Start connection-oriented CTE reception. */
	RTK_BT_LE_GAP_ACT_CONN_CTE_RX_STOP,                     /*!< Stop connection-oriented CTE reception. */
	RTK_BT_LE_GAP_ACT_PAWR_SET_SUBEVENT_DATA,               /*!< Set the subevent data of Periodic Advertising with Responses. */
	RTK_BT_LE_GAP_ACT_PAWR_SET_RESPONSE_DATA,               /*!< Set the response data of Periodic Advertising with Responses. */
	RTK_BT_LE_GAP_ACT_PAWR_SYNC_SUBEVENT,                   /*!< Set the subevents to synchronize with in Periodic Advertising with Responses. */
	RTK_BT_LE_GAP_ACT_COC_REG_PSM,                          /*!< Register or unregister an LE COC PSM. */
	RTK_BT_LE_GAP_ACT_COC_SET_PSM_SECURITY,                 /*!< Set the security requirements of an LE COC PSM. */
	RTK_BT_LE_GAP_ACT_COC_SET_PARAM,                        /*!< Set an LE COC protocol parameter. */
	RTK_BT_LE_GAP_ACT_COC_GET_CHAN_PARAM,                   /*!< Get a parameter of an LE COC channel. */
	RTK_BT_LE_GAP_ACT_COC_CONN,                             /*!< Initiate an LE COC connection. */
	RTK_BT_LE_GAP_ACT_COC_DISCONN,                          /*!< Disconnect an LE COC channel. */
	RTK_BT_LE_GAP_ACT_COC_SEND_DATA,                        /*!< Send data over an LE COC channel. */
	RTK_BT_LE_GAP_ACT_DTM_RX_TEST_V1,                       /*!< Start a DTM receiver test, version 1. */
	RTK_BT_LE_GAP_ACT_DTM_RX_TEST_V2,                       /*!< Start a DTM receiver test, version 2. */
	RTK_BT_LE_GAP_ACT_DTM_RX_TEST_V3,                       /*!< Start a DTM receiver test, version 3. */
	RTK_BT_LE_GAP_ACT_DTM_TX_TEST_V1,                       /*!< Start a DTM transmitter test, version 1. */
	RTK_BT_LE_GAP_ACT_DTM_TX_TEST_V2,                       /*!< Start a DTM transmitter test, version 2. */
	RTK_BT_LE_GAP_ACT_DTM_TX_TEST_V3,                       /*!< Start a DTM transmitter test, version 3. */
	RTK_BT_LE_GAP_ACT_DTM_TX_TEST_V4,                       /*!< Start a DTM transmitter test, version 4. */
	RTK_BT_LE_GAP_ACT_DTM_END_TEST,                         /*!< End the DTM test. */
	RTK_BT_LE_GAP_ACT_MAX,                                  /*!< Reserved. */
} rtk_bt_le_gap_act_t;

/**
 * @typedef   rtk_bt_le_gap_evt_t
 * @brief     Bluetooth LE GAP event indication.
 * @note      By default, the event callback message is sent from the stack (API task) to the event task and executed in the event task.
 *            If the event is direct calling, the callback is executed directly in the API task. This is designed for events whose callback returns
 *            a result that affects the stack's behavior.
 *            BT APIs cannot be called in the callback of a direct calling event.
 */
typedef enum {
	RTK_BT_LE_GAP_EVT_ADV_START_IND = 1,             /*!< Indicate LE legacy advertising started, with msg @ref rtk_bt_le_adv_start_ind_t. */
	RTK_BT_LE_GAP_EVT_ADV_STOP_IND,                  /*!< Indicate LE legacy advertising stopped, with msg @ref rtk_bt_le_adv_stop_ind_t. */
	RTK_BT_LE_GAP_EVT_CONNECT_IND,                   /*!< Indicate LE connection completed, with msg @ref rtk_bt_le_conn_ind_t. */
	RTK_BT_LE_GAP_EVT_DISCONN_IND,                   /*!< Indicate LE disconnection completed, with msg @ref rtk_bt_le_disconn_ind_t. */
	RTK_BT_LE_GAP_EVT_SCAN_START_IND,                /*!< Indicate LE legacy scan started, with msg @ref rtk_bt_le_scan_start_ind_t. */
	RTK_BT_LE_GAP_EVT_SCAN_RES_IND,                  /*!< Indicate LE legacy scan result, with msg @ref rtk_bt_le_scan_res_ind_t. */
	RTK_BT_LE_GAP_EVT_SCAN_STOP_IND,                 /*!< Indicate LE legacy scan stopped, with msg @ref rtk_bt_le_scan_stop_ind_t. */
	RTK_BT_LE_GAP_EVT_CONN_UPDATE_IND,               /*!< Indicate LE connection parameters updated, with msg @ref rtk_bt_le_conn_update_ind_t. */
	RTK_BT_LE_GAP_EVT_REMOTE_CONN_UPDATE_REQ_IND,    /*!< Indicate LE connection parameter update request from the remote device, with msg
                                                      *   @ref rtk_bt_le_remote_conn_update_req_ind_t. Direct calling event, BT APIs cannot be called
                                                      *   in its callback. */
	RTK_BT_LE_GAP_EVT_DATA_LEN_CHANGE_IND,           /*!< Indicate LE data length changed, with msg @ref rtk_bt_le_data_len_change_ind_t. */
	RTK_BT_LE_GAP_EVT_PHY_UPDATE_IND,                /*!< Indicate LE PHY updated, with msg @ref rtk_bt_le_phy_update_ind_t. */
	RTK_BT_LE_GAP_EVT_READ_REMOTE_VERSION_IND,       /*!< Indicate LE remote version information read completed, with msg
                                                      *   @ref rtk_bt_le_read_remote_version_ind_t. */
	RTK_BT_LE_GAP_EVT_AUTH_PAIRING_CONFIRM_IND,      /*!< Indicate LE Just Works pairing needs confirmation, with msg @ref rtk_bt_le_auth_pair_cfm_ind_t. */
	RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_DISPLAY_IND,      /*!< Indicate LE pairing passkey needs to be displayed, with msg @ref rtk_bt_le_auth_key_display_ind_t. */
	RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_INPUT_IND,        /*!< Indicate LE pairing passkey needs to be input, with msg @ref rtk_bt_le_auth_key_input_ind_t. */
	RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_CONFIRM_IND,      /*!< Indicate LE pairing passkey needs to be confirmed, with msg @ref rtk_bt_le_auth_key_cfm_ind_t. */
	RTK_BT_LE_GAP_EVT_AUTH_OOB_KEY_INPUT_IND,        /*!< Indicate LE pairing OOB key needs to be input, with msg @ref rtk_bt_le_auth_oob_input_ind_t. */
	RTK_BT_LE_GAP_EVT_AUTH_COMPLETE_IND,             /*!< Indicate LE authentication completed, with msg @ref rtk_bt_le_auth_complete_ind_t. */
	RTK_BT_LE_GAP_EVT_BOND_MODIFY_IND,               /*!< Indicate LE bond information modified, with msg @ref rtk_bt_le_bond_modify_ind_t. */
	RTK_BT_LE_GAP_EVT_EXT_ADV_IND,                   /*!< Indicate LE extended advertising state changed, with msg @ref rtk_bt_le_ext_adv_ind_t. */
	RTK_BT_LE_GAP_EVT_EXT_SCAN_RES_IND,              /*!< Indicate LE extended scan result, with msg @ref rtk_bt_le_ext_scan_res_ind_t. */
	RTK_BT_LE_GAP_EVT_PA_IND,                        /*!< Indicate LE periodic advertising state changed, with msg @ref rtk_bt_le_pa_ind_t. */
	RTK_BT_LE_GAP_EVT_PA_SYNC_STATE_IND,             /*!< Indicate LE periodic advertising synchronization state changed, with msg
                                                      *   @ref rtk_bt_le_pa_sync_ind_t. */
	RTK_BT_LE_GAP_EVT_PA_ADV_REPORT_IND,             /*!< Indicate LE periodic advertising report received, with msg @ref rtk_bt_le_pa_adv_report_ind_t. */
	RTK_BT_LE_GAP_EVT_RESOLV_LIST_MODIFY_IND,        /*!< Indicate LE resolving list modified, with msg @ref rtk_bt_le_modify_resolv_list_ind_t. */
	RTK_BT_LE_GAP_EVT_RESOLV_LIST_PENDING_IND,       /*!< Indicate LE resolving list modification is pending, with msg
                                                      *   @ref rtk_bt_le_resolv_list_pending_ind_t. */
	RTK_BT_LE_GAP_EVT_TXPOWER_REPORT_IND,            /*!< Indicate LE TX power level report, with msg @ref rtk_bt_le_txpower_ind_t. */
	RTK_BT_LE_GAP_EVT_CONNLESS_CTE_TX_STATE_IND,     /*!< Indicate LE connectionless CTE transmit state, with msg @ref rtk_bt_le_gap_connless_cte_tx_ind_t. */
	RTK_BT_LE_GAP_EVT_CONNLESS_IQ_REPORT_IND,        /*!< Indicate LE connectionless IQ report, with msg @ref rtk_bt_le_gap_connless_iq_report_ind_t. */
	RTK_BT_LE_GAP_EVT_CONN_IQ_REPORT_IND,            /*!< Indicate LE connection IQ report, with msg @ref rtk_bt_le_gap_conn_iq_report_ind_t. */
	RTK_BT_LE_GAP_EVT_CONN_CTE_REQ_FAIL_IND,         /*!< Indicate LE CTE request failure, with msg @ref rtk_bt_le_gap_cte_req_fail_ind_t. */
	RTK_BT_LE_GAP_EVT_PAWR_DATA_REQ_IND,             /*!< Indicate LE periodic advertising subevent data request, with msg
                                                      *   @ref rtk_bt_le_gap_pawr_data_req_ind_t. */
	RTK_BT_LE_GAP_EVT_PAWR_RSP_IND,                  /*!< Indicate LE periodic advertising response from the synchronized device, with msg
                                                      *   @ref rtk_bt_le_gap_pawr_rsp_ind_t. */
	RTK_BT_LE_GAP_EVT_COC_CONNECT_IND,               /*!< Indicate LE COC connection completed, with msg @ref rtk_bt_le_coc_conn_state_ind_t. */
	RTK_BT_LE_GAP_EVT_COC_DISCONNECT_IND,            /*!< Indicate LE COC disconnection completed, with msg @ref rtk_bt_le_coc_conn_state_ind_t. */
	RTK_BT_LE_GAP_EVT_COC_SEND_DATA_RESULT_IND,      /*!< Indicate LE COC send data result, with msg @ref rtk_bt_le_coc_send_data_res_ind_t. */
	RTK_BT_LE_GAP_EVT_COC_RECEIVE_DATA_IND,          /*!< Indicate LE COC data received, with msg @ref rtk_bt_le_coc_receive_data_ind_t. */
	RTK_BT_LE_GAP_EVT_MAX,                           /*!< Reserved. */
} rtk_bt_le_gap_evt_t;

/**
 * @typedef   rtk_bt_gatts_act_t
 * @brief     Bluetooth GATT Server action.
 */
typedef enum {
	RTK_BT_GATTS_ACT_REGISTER_SERVICE = 1,        /*!< Register a service. */
	RTK_BT_GATTS_ACT_UNREGISTER_SERVICE,          /*!< Unregister a service. */
	RTK_BT_GATTS_ACT_NOTIFY,                      /*!< Send a notification to the client. */
	RTK_BT_GATTS_ACT_INDICATE,                    /*!< Send an indication to the client. */
	RTK_BT_GATTS_ACT_READ_RSP,                    /*!< Send a read response to the client. */
	RTK_BT_GATTS_ACT_WRITE_RSP,                   /*!< Send a write response to the client. */
	RTK_BT_GATTS_ACT_SERVICE_CHANGED_INDICATE,    /*!< Send a Service Changed indication to the client. */
	RTK_BT_GATTS_ACT_GET_ATTR_HANDLE,             /*!< Get the attribute handle by service and attribute index. */
	RTK_BT_GATTS_ACT_MAX,                         /*!< Reserved. */
} rtk_bt_gatts_act_t;

/**
 * @typedef   rtk_bt_gatts_evt_t
 * @brief     Bluetooth GATT Server event indication.
 */
typedef enum {
	RTK_BT_GATTS_EVT_REGISTER_SERVICE = 1,         /*!< Indicate the service registration completed, with msg @ref rtk_bt_gatts_reg_ind_t. */
	RTK_BT_GATTS_EVT_MTU_EXCHANGE,                 /*!< Indicate the MTU exchanged, with msg @ref rtk_bt_gatt_mtu_exchange_ind_t. */
	RTK_BT_GATTS_EVT_READ_IND,                     /*!< Indicate a read request from the remote client, with msg @ref rtk_bt_gatts_read_ind_t. */
	RTK_BT_GATTS_EVT_WRITE_IND,                    /*!< Indicate a write request from the remote client, with msg @ref rtk_bt_gatts_write_ind_t. */
	RTK_BT_GATTS_EVT_CCCD_IND,                     /*!< Indicate the remote client wrote a CCCD, with msg @ref rtk_bt_gatts_cccd_ind_t. */
	RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND,        /*!< Indicate the indication to the remote client completed, with msg @ref rtk_bt_gatts_ntf_and_ind_ind_t. */
	RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND,          /*!< Indicate the notification to the remote client completed, with msg
                                                    *   @ref rtk_bt_gatts_ntf_and_ind_ind_t. */
	RTK_BT_GATTS_EVT_CLIENT_SUPPORTED_FEATURES,    /*!< Indicate the remote client wrote the Client Supported Features characteristic, with msg
                                                    *   @ref rtk_bt_gatts_client_supported_features_ind_t. */
	RTK_BT_GATTS_EVT_SERVICE_CHANGED_CCCD_IND,     /*!< Indicate the remote client wrote the CCCD of the Service Changed characteristic in the built-in GATT
                                                    *   service, with msg @ref rtk_bt_gatts_service_changed_cccd_ind_t. */
	RTK_BT_GATTS_EVT_MAX,                          /*!< Reserved. */
} rtk_bt_gatts_evt_t;

/**
 * @typedef   rtk_bt_gattc_act_t
 * @brief     Bluetooth GATT Client action.
 */
typedef enum {
	RTK_BT_GATTC_ACT_REGISTER_PROFILE = 1,       /*!< Register a client profile. */
	RTK_BT_GATTC_ACT_EXCHANGE_MTU,               /*!< Request an ATT MTU exchange. */
	RTK_BT_GATTC_ACT_DISCOVER,                   /*!< Discover attributes of the remote server. */
	RTK_BT_GATTC_ACT_READ,                       /*!< Read characteristic value of the remote server. */
	RTK_BT_GATTC_ACT_WRITE,                      /*!< Write characteristic value of the remote server. */
	RTK_BT_GATTC_ACT_CCCD_ENABLE,                /*!< Enable notification or indication of remote server characteristics. */
	RTK_BT_GATTC_ACT_CCCD_DISABLE,               /*!< Disable notification or indication of remote server characteristics. */
	RTK_BT_GATTC_ACT_FIND,                       /*!< Find attributes of the remote server by type. */
	RTK_BT_GATTC_ACT_MAX,                        /*!< Reserved. */
} rtk_bt_gattc_act_t;

/**
 * @typedef   rtk_bt_gattc_evt_t
 * @brief     Bluetooth GATT Client event indication.
 */
typedef enum {
	RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND = 1,    /*!< Indicate the discovery result of the remote server, with msg @ref rtk_bt_gattc_discover_ind_t. */
	RTK_BT_GATTC_EVT_READ_RESULT_IND,            /*!< Indicate the read result of the remote server, with msg @ref rtk_bt_gattc_read_ind_t. */
	RTK_BT_GATTC_EVT_WRITE_RESULT_IND,           /*!< Indicate the write result of the remote server, with msg @ref rtk_bt_gattc_write_ind_t. */
	RTK_BT_GATTC_EVT_NOTIFY_IND,                 /*!< Indicate a notification received from the remote server, with msg @ref rtk_bt_gattc_cccd_value_ind_t. */
	RTK_BT_GATTC_EVT_INDICATE_IND,               /*!< Indicate an indication received from the remote server, with msg @ref rtk_bt_gattc_cccd_value_ind_t. */
	RTK_BT_GATTC_EVT_CCCD_ENABLE_IND,            /*!< Indicate enabling the CCCD of the remote server completed, with msg @ref rtk_bt_gattc_cccd_update_ind_t. */
	RTK_BT_GATTC_EVT_CCCD_DISABLE_IND,           /*!< Indicate disabling the CCCD of the remote server completed, with msg @ref rtk_bt_gattc_cccd_update_ind_t. */
	RTK_BT_GATTC_EVT_MTU_EXCHANGE,               /*!< Indicate the MTU exchanged, with msg @ref rtk_bt_gatt_mtu_exchange_ind_t. */
	RTK_BT_GATTC_EVT_DISCOVER_ALL_STATE_IND,     /*!< Indicate the discovery all attributes state of the remote server, with msg
                                                  *   @ref rtk_bt_gattc_discover_all_state_ind_t. */
	RTK_BT_GATTC_EVT_GATT_SERVICE_INFO_IND,      /*!< Indicate the GATT service information of the remote server, with msg @ref rtk_bt_gattc_gatt_service_info_ind_t. */
	RTK_BT_GATTC_EVT_MAX,                        /*!< Reserved. */
} rtk_bt_gattc_evt_t;

/**
 * @typedef   rtk_bt_le_iso_act_t
 * @brief     Bluetooth LE ISO management action.
 */
typedef enum {
	RTK_BT_LE_ISO_ACT_CIG_START_SETTING = 1,
	RTK_BT_LE_ISO_ACT_CIG_SETUP_PATH,
	RTK_BT_LE_ISO_ACT_CIG_REMOVE_PATH,
	RTK_BT_LE_ISO_ACT_ISO_DATA_SEND,
	RTK_BT_LE_ISO_ACT_CIG_GET_ISOCH_INFO,
	RTK_BT_LE_ISO_ACT_CIG_READ_ISO_TX_SYNC,
	RTK_BT_LE_ISO_ACT_CIG_READ_LINK_QUALITY,
	RTK_BT_LE_ISO_ACT_CIG_INITIATOR_SET_CIS_ACL_LINK,
	RTK_BT_LE_ISO_ACT_CIG_INITIATOR_CREATE_CIS_BY_CIG_ID,
	RTK_BT_LE_ISO_ACT_CIG_INITIATOR_CREATE_CIS_BY_CIG_CONN_HANDLE,
	RTK_BT_LE_ISO_ACT_CIG_DISCONNECT,
	RTK_BT_LE_ISO_ACT_CIG_INITIATOR_REMOVE_CIG,
	RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_ACCEPT_CIS,
	RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_REJECT_CIS,
	RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_CONFIG_CIS_REQ_ACTION,
	RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_REGISTER_CALLBACK,
	RTK_BT_LE_ISO_ACT_BIG_SETUP_PATH,
	RTK_BT_LE_ISO_ACT_BIG_REMOVE_PATH,
	RTK_BT_LE_ISO_ACT_BIG_BROADCASTER_CREATE,
	RTK_BT_LE_ISO_ACT_BIG_BROADCASTER_TERMINATE,
	RTK_BT_LE_ISO_ACT_BIG_BROADCASTER_READ_TX_SYNC,
	RTK_BT_LE_ISO_ACT_BIG_RECEIVER_CREATE_SYNC,
	RTK_BT_LE_ISO_ACT_BIG_RECEIVER_TERMINATE_SYNC,
	RTK_BT_LE_ISO_ACT_BIG_RECEIVER_READ_LINK_QUALITY,
	RTK_BT_LE_ISO_ACT_SYNC_INFO,
	RTK_BT_LE_ISO_ACT_GET_ISO_CONN_HANDLE,
	RTK_BT_LE_ISO_ACT_MAX
} rtk_bt_le_iso_act_t;

/**
 * @typedef   rtk_bt_le_iso_evt_code_t
 * @brief     Bluetooth LE ISO management event code definition.
 */
typedef enum {
	RTK_BT_LE_ISO_EVT_CIG_DISCONNECT_INFO = 1,                    /*!< Indicate a CIS disconnection, with msg @ref rtk_bt_le_iso_cig_disconnect_info_t. */
	RTK_BT_LE_ISO_EVT_CIG_CIS_ESTABLISHED_INFO,                   /*!< Indicate the CIS establishment result, with msg @ref rtk_bt_le_iso_cig_cis_established_info_t. */
	RTK_BT_LE_ISO_EVT_CIG_ACCEPTOR_REQUEST_CIS_IND,               /*!< Indicate a CIS establishment request received by the acceptor, with msg
                                                                   *   @ref rtk_bt_le_iso_cig_acceptor_request_cis_ind_t. */
	RTK_BT_LE_ISO_EVT_CIG_SETUP_DATA_PATH,                        /*!< Indicate the CIS data path setup completed, with msg @ref rtk_bt_le_iso_cig_setup_data_path_info_t. */
	RTK_BT_LE_ISO_EVT_CIG_REMOVE_DATA_PATH,                       /*!< Indicate the CIS data path removal completed, with msg @ref rtk_bt_le_iso_cig_remove_data_path_info_t. */
	RTK_BT_LE_ISO_EVT_CIG_START_SETTING_INFO,                     /*!< Indicate the CIG start setting completed, carrying the CIS connection handles, with msg
                                                                   *   @ref rtk_bt_le_iso_cig_start_setting_info_t. */
	RTK_BT_LE_ISO_EVT_BIG_BROADCASTER_CREATE_CMPL_INFO,           /*!< Indicate the BIG creation result of the broadcaster, with msg @ref rtk_bt_le_iso_big_broadcaster_create_cmpl_info_t. */
	RTK_BT_LE_ISO_EVT_BIG_BROADCASTER_SYNC_STATE_IND,             /*!< Indicate the BIG synchronization state change of the broadcaster, with msg
                                                                   *   @ref rtk_bt_le_iso_big_broadcaster_sync_state_ind_t. */
	RTK_BT_LE_ISO_EVT_BIG_RECEIVER_SYNC_ESTABLISHED_INFO,         /*!< Indicate the BIG synchronization establishment result of the receiver, with msg
                                                                   *   @ref rtk_bt_le_iso_big_receiver_sync_established_info_t. */
	RTK_BT_LE_ISO_EVT_BIG_RECEIVER_SYNC_STATE_IND,                /*!< Indicate the BIG synchronization state change of the receiver, with msg @ref rtk_bt_le_iso_big_receiver_sync_state_ind_t. */
	RTK_BT_LE_ISO_EVT_BIG_SETUP_DATA_PATH,                        /*!< Indicate the BIS data path setup completed, with msg @ref rtk_bt_le_iso_big_setup_data_path_info_t. */
	RTK_BT_LE_ISO_EVT_BIG_REMOVE_DATA_PATH,                       /*!< Indicate the BIS data path removal completed, with msg @ref rtk_bt_le_iso_big_remove_data_path_info_t. */
	RTK_BT_LE_ISO_EVT_DATA_RECEIVE_IND,                           /*!< Indicate ISO data received, with msg @ref rtk_bt_le_iso_direct_iso_data_ind_t.
                                                                   *   Direct calling, BT API shall not be called in this event. */
	RTK_BT_LE_ISO_EVT_MAX,                                        /*!< Reserved. */
} rtk_bt_le_iso_evt_code_t;

/**
 * @typedef   rtk_bt_le_audio_bap_act_t
 * @brief     Bluetooth LE audio bap action.
 */
typedef enum {
	RTK_BT_LE_AUDIO_ACT_ISO_DATA_SEND = 1,
	RTK_BT_LE_AUDIO_ACT_LINK_ALLOCATE,
	RTK_BT_LE_AUDIO_ACT_LINK_FREE,
	RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_CREATE,
	RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_START,
	RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_STOP,
	RTK_BT_LE_AUDIO_ACT_EXT_SCAN_ACT,
	RTK_BT_LE_AUDIO_ACT_BROADCAST_PA_SYNC_CREATE,
	RTK_BT_LE_AUDIO_ACT_BROADCAST_PA_SYNC_TERMINATE,
	RTK_BT_LE_AUDIO_ACT_BROADCAST_BIG_SYNC_CREATE,
	RTK_BT_LE_AUDIO_ACT_BROADCAST_BIG_SYNC_CREATE_BY_HANDLE,
	RTK_BT_LE_AUDIO_ACT_BROADCAST_BIG_SYNC_TERMINATE,
	RTK_BT_LE_AUDIO_ACT_BROADCAST_BIG_SYNC_TERMINATE_BY_HANDLE,
	RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_REMOTE_SCAN_ACT,
	RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_RECEPTION_START,
	RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_RECEPTION_STOP,
	RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_RECEPTION_REMOVE,
	RTK_BT_LE_AUDIO_ACT_GROUP_ALLOCATE,
	RTK_BT_LE_AUDIO_ACT_GROUP_RELEASE,
	RTK_BT_LE_AUDIO_ACT_CLIENT_START,
	RTK_BT_LE_AUDIO_ACT_CLIENT_STOP,
	RTK_BT_LE_AUDIO_ACT_CLIENT_RELEASE,
	RTK_BT_LE_AUDIO_ACT_BAP_PARAM_CONFIG,
	RTK_BT_LE_AUDIO_ACT_BAP_MAX,
} rtk_bt_le_audio_bap_act_t;

/**
 * @typedef   rtk_bt_le_audio_evt_t
 * @brief     Bluetooth LE audio event indication.
 */
typedef enum {
	RTK_BT_LE_AUDIO_EVT_ISO_DATA_RECEIVE_IND = 1,
	RTK_BT_LE_AUDIO_EVT_BAP_DISCOVERY_DONE_IND,
	RTK_BT_LE_AUDIO_EVT_ASCS_CIS_CONN_INFO,
	RTK_BT_LE_AUDIO_EVT_ASCS_SETUP_DATA_PATH_IND,
	RTK_BT_LE_AUDIO_EVT_ASCS_REMOVE_DATA_PATH_IND,
	RTK_BT_LE_AUDIO_EVT_ASCS_ASE_STATE_IND,
	RTK_BT_LE_AUDIO_EVT_ASCS_CP_CODEC_CFG_IND,
	RTK_BT_LE_AUDIO_EVT_ASCS_CP_QOS_CFG_IND,
	RTK_BT_LE_AUDIO_EVT_ASCS_CP_ENABLE_IND,
	RTK_BT_LE_AUDIO_EVT_ASCS_CP_DISABLE_IND,
	RTK_BT_LE_AUDIO_EVT_ASCS_GET_PREFER_QOS_IND,
	RTK_BT_LE_AUDIO_EVT_BASS_GET_PA_SYNC_PARAM_IND,
	RTK_BT_LE_AUDIO_EVT_BASS_GET_BIG_SYNC_PARAM_IND,
	RTK_BT_LE_AUDIO_EVT_PA_SYNC_STATE_IND,
	RTK_BT_LE_AUDIO_EVT_BIG_SYNC_STATE_IND,
	RTK_BT_LE_AUDIO_EVT_BIG_INFO_ADV_REPORT,
	RTK_BT_LE_AUDIO_EVT_BIG_SETUP_DATA_PATH,
	RTK_BT_LE_AUDIO_EVT_BIG_REMOVE_DATA_PATH,
	RTK_BT_LE_AUDIO_EVT_BROADCAST_SOURCE_STATE_IND,
	RTK_BT_LE_AUDIO_EVT_BROADCAST_SOURCE_SETUP_DATA_PATH_IND,
	RTK_BT_LE_AUDIO_EVT_BROADCAST_SOURCE_REMOVE_DATA_PATH_IND,
	RTK_BT_LE_AUDIO_EVT_GROUP_DEV_MSG_IND,
	RTK_BT_LE_AUDIO_EVT_BAP_SETUP_DATA_PATH_IND,
	RTK_BT_LE_AUDIO_EVT_BAP_REMOVE_DATA_PATH_IND,
	RTK_BT_LE_AUDIO_EVT_BAP_MAX,
} rtk_bt_le_audio_bap_evt_t;

/**
 * @typedef   rtk_bt_le_audio_cap_act_t
 * @brief     Bluetooth LE audio cap action.
 */
typedef enum {
	RTK_BT_LE_AUDIO_ACT_CSIS_GEN_RSI = 1,
	RTK_BT_LE_AUDIO_ACT_CSIS_UPDATE_SIRK,
	RTK_BT_LE_AUDIO_ACT_CSIS_UPDATE_SIZE,
	RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_CHECK_ADV_RSI,
	RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_CFG_DISCOVER,
	RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_ADD_GROUP,
	RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_ADD_DEVICE,
	RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_WRITE_LOCK,
	RTK_BT_LE_AUDIO_ACT_MCP_SERVER_SEND_DATA,
	RTK_BT_LE_AUDIO_ACT_MCP_SERVER_READ_CFM,
	RTK_BT_LE_AUDIO_ACT_MCP_CLIENT_WRITE_MEDIA_CP,
	RTK_BT_LE_AUDIO_ACT_MCP_CLIENT_READ_CHAR_VALUE,
	RTK_BT_LE_AUDIO_ACT_MCP_CLIENT_CFG_CCCD,
	RTK_BT_LE_AUDIO_ACT_VCS_SET_PARAM,
	RTK_BT_LE_AUDIO_ACT_VCS_GET_PARAM,
	RTK_BT_LE_AUDIO_ACT_VCS_WRITE_CP,
	RTK_BT_LE_AUDIO_ACT_VCS_GET_VOLUME_STATE,
	RTK_BT_LE_AUDIO_ACT_VCS_GET_VOLUME_FLAGS,
	RTK_BT_LE_AUDIO_ACT_VCS_CHANGE_MUTE,
	RTK_BT_LE_AUDIO_ACT_VCS_CHANGE_VOLUME,
	RTK_BT_LE_AUDIO_ACT_MICS_CHANGE_MUTE,
	RTK_BT_LE_AUDIO_ACT_MICS_SET_MUTE_VALUE,
	RTK_BT_LE_AUDIO_ACT_MICS_GET_MUTE_VALUE,
	RTK_BT_LE_AUDIO_ACT_MICS_SET_PARAM,
	RTK_BT_LE_AUDIO_ACT_MICS_GET_PARAM,
	RTK_BT_LE_AUDIO_ACT_VOCS_SET_PARAM,
	RTK_BT_LE_AUDIO_ACT_VOCS_GET_PARAM,
	RTK_BT_LE_AUDIO_ACT_VOCS_CFG_CCCD,
	RTK_BT_LE_AUDIO_ACT_VOCS_READ_CHAR_VALUE,
	RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_CP,
	RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_CP_BY_GROUP,
	RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_AUDIO_LOCATION,
	RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_OUTPUT_DES,
	RTK_BT_LE_AUDIO_ACT_VOCS_GET_SRV_DATA,
	RTK_BT_LE_AUDIO_ACT_AICS_SET_PARAM,
	RTK_BT_LE_AUDIO_ACT_AICS_GET_PARAM,
	RTK_BT_LE_AUDIO_ACT_AICS_CFG_CCCD,
	RTK_BT_LE_AUDIO_ACT_AICS_READ_CHAR_VALUE,
	RTK_BT_LE_AUDIO_ACT_AICS_WRITE_CP,
	RTK_BT_LE_AUDIO_ACT_AICS_WRITE_CP_BY_GROUP,
	RTK_BT_LE_AUDIO_ACT_AICS_WRITE_INPUT_DES,
	RTK_BT_LE_AUDIO_ACT_AICS_GET_SRV_DATA,
	RTK_BT_LE_AUDIO_ACT_CAP_PARAM_CONFIG,
	RTK_BT_LE_AUDIO_CAP_ACT_MAX,
} rtk_bt_le_audio_cap_act_t;

/**
 * @typedef   rtk_bt_le_audio_cap_evt_t
 * @brief     Bluetooth LE audio cap event indication.
 */
typedef enum {
	RTK_BT_LE_AUDIO_EVT_CAP_DISCOVERY_DONE_IND = 1,
	RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_DISCOVERY_DONE_IND,
	RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_READ_RESULT_IND,
	RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SEARCH_DONE_IND,
	RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SET_MEM_FOUND_IND,
	RTK_BT_LE_AUDIO_EVT_MCP_SERVER_WRITE_MEDIA_CP_IND,
	RTK_BT_LE_AUDIO_EVT_MCP_SERVER_READ_IND,
	RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_DISCOVERY_DONE_IND,
	RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_READ_RESULT_IND,
	RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_NOTIFY_IND,
	RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_MEDIA_CP_NOTIFY_IND,
	RTK_BT_LE_AUDIO_EVT_VCS_VOLUME_CP_IND,
	RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_DISCOVERY_DONE_IND,
	RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_STATE_IND,
	RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_FLAG_IND,
	RTK_BT_LE_AUDIO_EVT_MICS_WRITE_MUTE_IND, //0x30
	RTK_BT_LE_AUDIO_EVT_MICS_CLIENT_DISCOVERY_DONE_IND,
	RTK_BT_LE_AUDIO_EVT_MICS_CLIENT_NOTIFY_IND,
	RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_OFFSET_STATE_IND,
	RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_AUDIO_LOCATION_IND,
	RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_OUTPUT_DES_IND,
	RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_DISCOVERY_DONE_IND,
	RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_READ_RESULT_IND,
	RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_NOTIFY_IND,
	RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_CP_RESULT_IND,
	RTK_BT_LE_AUDIO_EVT_AICS_CP_IND,
	RTK_BT_LE_AUDIO_EVT_AICS_WRITE_INPUT_DES_IND,
	RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_DISCOVERY_DONE_IND,
	RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_READ_RESULT_IND,
	RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_NOTIFY_IND,
	RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_CP_RESULT_IND,
	RTK_BT_LE_AUDIO_CAP_EVT_MAX,
} rtk_bt_le_audio_cap_evt_t;

/**
 * @typedef   rtk_bt_le_audio_tmap_act_t
 * @brief     Bluetooth LE audio tmap action.
 */
typedef enum {
	RTK_BT_LE_AUDIO_ACT_TMAS_READ_ROLE = 1,
	RTK_BT_LE_AUDIO_TMAP_ACT_MAX,
} rtk_bt_le_audio_tmap_act_t;

/**
 * @typedef   rtk_bt_le_audio_tmap_evt_t
 * @brief     Bluetooth LE audio tmap event indication.
 */
typedef enum {
	RTK_BT_LE_AUDIO_EVT_TMAS_CLIENT_DISCOVERY_DONE_IND = 1,
	RTK_BT_LE_AUDIO_EVT_TMAS_CLIENT_READ_ROLE_RESULT,
	RTK_BT_LE_AUDIO_TMAP_EVT_MAX,
} rtk_bt_le_audio_tmap_evt_t;

/**
 * @typedef   rtk_bt_le_audio_gmap_act_t
 * @brief     Bluetooth LE audio gmap action.
 */
typedef enum {
	RTK_BT_LE_AUDIO_GMAS_ACT_READ_ROLE = 1,
	RTK_BT_LE_AUDIO_GMAS_ACT_READ_FEATURES,
	RTK_BT_LE_AUDIO_GMAP_ACT_MAX,
} rtk_bt_le_audio_gmap_act_t;

/**
 * @typedef   rtk_bt_le_audio_gmap_evt_t
 * @brief     Bluetooth LE audio gmap event indication.
 */
typedef enum {
	RTK_BT_LE_AUDIO_GMAS_EVT_READ_IND = 1,
	RTK_BT_LE_AUDIO_GMAS_EVT_CLIENT_DISCOVERY_DONE_IND,
	RTK_BT_LE_AUDIO_GMAS_EVT_CLIENT_READ_ROLE_RESULT,
	RTK_BT_LE_AUDIO_GMAS_EVT_CLIENT_READ_FEATURES_RESULT,
	RTK_BT_LE_AUDIO_GMAP_EVT_MAX,
} rtk_bt_le_audio_gmap_evt_t;
/************************************* BR/EDR group ************************************/

/**
 * @typedef   gap_uuid_type_t
 * @brief     Bluetooth GAP uuid type.
 */
typedef enum {
	BR_GAP_UUID16,    /*!< UUID type 16 bits. */
	BR_GAP_UUID32,    /*!< UUID type 32 bits. */
	BR_GAP_UUID128    /*!< UUID type 128 bits. */
} br_gap_uuid_type_t;

/**
 * @typedef   gap_uuid_data_t
 * @brief     Bluetooth GAP uuid data.
 */
typedef union {
	uint16_t    uuid_16;        /*!< UUID data in 16 bits. */
	uint32_t    uuid_32;        /*!< UUID data in 32 bits. */
	uint8_t     uuid_128[16];   /*!< UUID data in 128 bits. */
} br_gap_uuid_data_t;

/**
 * @typedef   rtk_bt_br_group_t
 * @brief     Bluetooth BR action group.
 */
typedef enum {
	RTK_BT_BR_GP_GAP = RTK_BT_API_BR_BASE,
	RTK_BT_BR_GP_AVRCP,                           /*!< BR/EDR avrcp group. */
	RTK_BT_BR_GP_A2DP,                            /*!< BR/EDR a2dp group. */
	RTK_BT_BR_GP_SPP,                             /*!< BR/EDR spp group. */
	RTK_BT_BR_GP_RFC,                             /*!< BR/EDR RFCOMM group. */
	RTK_BT_BR_GP_HFP,                             /*!< BR/EDR hfp group. */
	RTK_BT_BR_GP_SDP,                             /*!< BR/EDR sdp group. */
	RTK_BT_BR_GP_HID,                             /*!< BR/EDR hid group. */
	RTK_BT_BR_GP_PBAP,                            /*!< BR/EDR phone book access profile. */
	RTK_BT_BR_GP_MAX,
} rtk_bt_br_group_t;

/**
 * @typedef   rtk_bt_br_gap_act_t
 * @brief     Bluetooth BR/EDR GAP action.
 */
typedef enum {
	RTK_BT_BR_GAP_ACT_SET_DEFAULT_PARAM = 1,
	RTK_BT_BR_GAP_ACT_GET_DEVICE_ADDR,
	RTK_BT_BR_GAP_ACT_SET_DEVICE_NAME,
	RTK_BT_BR_GAP_ACT_SET_PAGE_PARAM,
	RTK_BT_BR_GAP_ACT_CFG_PAGE_PARAM,
	RTK_BT_BR_GAP_ACT_SET_INQUIRY_PARAM,
	RTK_BT_BR_GAP_ACT_CFG_INQUIRY_PARAM,
	RTK_BT_BR_GAP_ACT_SET_SEC_PARAM,
	RTK_BT_BR_GAP_ACT_DISCONNECT,
	RTK_BT_BR_GAP_ACT_START_INQUIRY,
	RTK_BT_BR_GAP_ACT_GET_REMOTE_NAME,
	RTK_BT_BR_GAP_ACT_MAX_BOND_NUM_GET,
	RTK_BT_BR_GAP_ACT_BOND_NUM_GET,
	RTK_BT_BR_GAP_ACT_BOND_ADDR_GET,
	RTK_BT_BR_GAP_ACT_BOND_INDEX_GET,
	RTK_BT_BR_GAP_ACT_BOND_KEY_GET,
	RTK_BT_BR_GAP_ACT_BOND_KEY_SET,
	RTK_BT_BR_GAP_ACT_BOND_PRIORITY_SET,
	RTK_BT_BR_GAP_ACT_BOND_DELETE,
	RTK_BT_BR_GAP_ACT_SET_PINCODE,
	RTK_BT_BR_GAP_ACT_SET_RADIO_MODE,
	RTK_BT_BR_GAP_ACT_SET_AUTO_SNIFF_MODE,
	RTK_BT_BR_GAP_ACT_SET_SNIFF_MODE,
	RTK_BT_BR_GAP_ACT_SET_LINK_QOS,
	RTK_BT_BR_GAP_ACT_SET_COD,
	RTK_BT_BR_GAP_ACT_SET_SUPV_TIMEOUT,
	RTK_BT_BR_GAP_ACT_BOND_CLEAR,
	RTK_BT_BR_GAP_ACT_READ_RSSI,
	RTK_BT_BR_GAP_ACT_MAX,
} rtk_bt_br_gap_act_t;

/**
 * @typedef   rtk_bt_br_gap_evt_t
 * @brief     Bluetooth BR/EDR GAP event.
 */
typedef enum {
	RTK_BT_BR_GAP_INQUIRY_RESULT = 1,
	RTK_BT_BR_GAP_INQUIRY_CMPL,
	RTK_BT_BR_GAP_REMOTE_NAME_RSP,
	RTK_BT_BR_GAP_ACL_CONN_IND,
	RTK_BT_BR_GAP_ACL_CONN_SUCCESS,
	RTK_BT_BR_GAP_ACL_CONN_FAIL,
	RTK_BT_BR_GAP_ACL_SNIFF,
	RTK_BT_BR_GAP_ACL_ACTIVE,
	RTK_BT_BR_GAP_ACL_DISCONN,
	RTK_BT_BR_GAP_LINK_KEY_REQ,
	RTK_BT_BR_GAP_LINK_KEY_INFO,
	RTK_BT_BR_GAP_LINK_ROLE_MASTER,
	RTK_BT_BR_GAP_LINK_ROLE_SLAVE,
	RTK_BT_BR_GAP_LINK_RSSI_INFO,
	RTK_BT_BR_GAP_EVT_MAX,
} rtk_bt_br_gap_evt_t;

/**
 * @typedef   rtk_bt_a2dp_act_t
 * @brief     Bluetooth A2DP action.
 */
typedef enum {
	RTK_BT_A2DP_ACT_CODEC_CFG = 1,
	RTK_BT_A2DP_ACT_CONNECT,
	RTK_BT_A2DP_ACT_DISCONNECT,
	RTK_BT_A2DP_ACT_START,
	RTK_BT_A2DP_ACT_SUSPEND,
	RTK_BT_A2DP_ACT_SEND_DATA,
	RTK_BT_A2DP_ACT_MAX,
} rtk_bt_a2dp_act_t;

/**
 * @typedef   rtk_bt_a2dp_evt_t
 * @brief     Bluetooth A2DP event indication.
 */
typedef enum {
	RTK_BT_A2DP_EVT_CONN_IND = 1,                  /*!< A2DP connection indication. */
	RTK_BT_A2DP_EVT_CONN_CMPL,                     /*!< A2DP connection complete. */
	RTK_BT_A2DP_EVT_DISCONN_CMPL,                  /*!< A2DP disconnection complete. */
	RTK_BT_A2DP_EVT_CONFIG_CMPL,                   /*!< A2DP configure complete. */
	RTK_BT_A2DP_EVT_STREAM_OPEN,                   /*!< A2DP stream open. */
	RTK_BT_A2DP_EVT_STREAM_START_IND,              /*!< A2DP stream start indication. */
	RTK_BT_A2DP_EVT_STREAM_START_RSP,              /*!< A2DP stream start response. */
	RTK_BT_A2DP_EVT_STREAM_STOP,                   /*!< A2DP stream stop. */
	RTK_BT_A2DP_EVT_STREAM_CLOSE,                  /*!< A2DP stream close. */
	RTK_BT_A2DP_EVT_STREAM_DATA_IND,               /*!< A2DP stream data indication. Direct calling, BT api shall not be called in this event case. */
	RTK_BT_A2DP_EVT_STREAM_DATA_RSP,               /*!< A2DP stream data receive rsp. */
	RTK_BT_A2DP_EVT_SDP_ATTR_INFO,                 /*!< A2DP SRC get sink sdp data info. */
	RTK_BT_A2DP_EVT_MAX,
} rtk_bt_a2dp_evt_t;

/**
 * @typedef   rtk_bt_avrcp_act_t
 * @brief     Bluetooth AVRCP action.
 */
typedef enum {
	RTK_BT_AVRCP_ACT_CONNECT = 1,
	RTK_BT_AVRCP_ACT_DISCONNECT,
	RTK_BT_AVRCP_ACT_PLAY_STATUS_CHANGE_REQ,
	RTK_BT_AVRCP_ACT_PLAY_STATUS_CHANGE_REG_RSP,
	RTK_BT_AVRCP_ACT_PLAY,
	RTK_BT_AVRCP_ACT_PAUSE,
	RTK_BT_AVRCP_ACT_STOP,
	RTK_BT_AVRCP_ACT_FORWARD,
	RTK_BT_AVRCP_ACT_BACKWARD,
	RTK_BT_AVRCP_ACT_REWIND_START,
	RTK_BT_AVRCP_ACT_REWIND_STOP,
	RTK_BT_AVRCP_ACT_FAST_FORWARD_START,
	RTK_BT_AVRCP_ACT_FAST_FORWARD_STOP,
	RTK_BT_AVRCP_ACT_ABSOLUTE_VOLUME_SET,
	RTK_BT_AVRCP_ACT_VOLUME_CHANGE_REQ,
	RTK_BT_AVRCP_ACT_GET_ELEMENT_ATTR,
	RTK_BT_AVRCP_ACT_CONN_COVER_ART,
	RTK_BT_AVRCP_ACT_APP_SETTING_ATTRS_LIST,
	RTK_BT_AVRCP_ACT_APP_SETTING_VALUES_LIST,
	RTK_BT_AVRCP_ACT_APP_SETTING_VALUE_GET,
	RTK_BT_AVRCP_ACT_APP_SETTING_VALUE_SET,
	RTK_BT_AVRCP_ACT_GET_PLAY_STATUS_REQ,
	RTK_BT_AVRCP_ACT_MAX,
} rtk_bt_avrcp_act_t;

/**
 * @typedef   rtk_bt_avrcp_evt_t
 * @brief     Bluetooth AVRCP event indication.
 */
typedef enum {
	RTK_BT_AVRCP_EVT_CONN_IND = 1,                  /*!< AVRCP connection indication. */
	RTK_BT_AVRCP_EVT_ABSOLUTE_VOLUME_SET,           /*!< AVRCP absolute volume set. */
	RTK_BT_AVRCP_EVT_VOLUME_CHANGED,                /*!< AVRCP volume changed. */
	RTK_BT_AVRCP_EVT_VOLUME_UP,                     /*!< AVRCP volume up. */
	RTK_BT_AVRCP_EVT_VOLUME_DOWN,                   /*!< AVRCP volume down. */
	RTK_BT_AVRCP_EVT_REG_VOLUME_CHANGED,            /*!< AVRCP volume reg changed. */
	RTK_BT_AVRCP_EVT_ELEMENT_ATTR_INFO,             /*!< AVRCP element attr information. */
	RTK_BT_AVRCP_EVT_APP_SETTING_ATTRS_LIST_RSP,    /*!< AVRCP app settings attrs list. */
	RTK_BT_AVRCP_EVT_APP_SETTING_VALUES_LIST_RSP,   /*!< AVRCP app values attrs list. */
	RTK_BT_AVRCP_EVT_APP_SETTING_GET_RSP,           /*!< AVRCP app setting get response. */
	RTK_BT_AVRCP_EVT_COVER_ART_DATA_IND,            /*!< AVRCP cover art data indication. */
	RTK_BT_AVRCP_EVT_CONN_CMPL,                     /*!< AVRCP conn completion. */
	RTK_BT_AVRCP_EVT_DISCONN_CMPL,                  /*!< AVRCP disconn completion. */
	RTK_BT_AVRCP_EVT_PLAY_STATUS_CHANGED_REG_REQ,   /*!< AVRCP play status changed. */
	RTK_BT_AVRCP_EVT_TRACK_CHANGED,                 /*!< AVRCP track changed. */
	RTK_BT_AVRCP_EVT_PLAY_REQ_EVENT,                /*!< AVRCP play request event. */
	RTK_BT_AVRCP_EVT_PAUSE_REQ_EVENT,               /*!< AVRCP pause request event. */
	RTK_BT_AVRCP_EVT_STOP_REQ_EVENT,                /*!< AVRCP stop request event. */
	RTK_BT_AVRCP_EVT_FORWARD_REQ_EVENT,             /*!< AVRCP forward request event. */
	RTK_BT_AVRCP_EVT_BACKWARD_REQ_EVENT,            /*!< AVRCP backward request event. */
	RTK_BT_AVRCP_EVT_FAST_FORWARD_START_REQ_EVENT,  /*!< AVRCP fast forward start request event. */
	RTK_BT_AVRCP_EVT_FAST_FORWARD_STOP_REQ_EVENT,   /*!< AVRCP fast forward stop request event. */
	RTK_BT_AVRCP_EVT_REWIND_START_REQ_EVENT,        /*!< AVRCP rewind start request event. */
	RTK_BT_AVRCP_EVT_REWIND_STOP_REQ_EVENT,         /*!< AVRCP rewind stop request event. */
	RTK_BT_AVRCP_EVT_GET_PLAY_STATUS_RSP_EVENT,     /*!< AVRCP get play status response event. */
	RTK_BT_AVRCP_EVT_MAX,
} rtk_bt_avrcp_evt_t;

/**
 * @typedef   rtk_bt_spp_act_t
 * @brief     Bluetooth SPP action.
 */

typedef enum {
	RTK_BT_SPP_ACT_INIT_CFG = 1,
	RTK_BT_SPP_ACT_SRV_CFG,
	RTK_BT_SPP_ACT_CONNECT,
	RTK_BT_SPP_ACT_DISCONNECT,
	RTK_BT_SPP_ACT_DISCONNECT_ALL,
	RTK_BT_SPP_ACT_SEND_DATA,
	RTK_BT_SPP_ACT_GIVE_CREDITS,
	RTK_BT_SPP_ACT_MAX,
} rtk_bt_spp_act_t;

/**
 * @typedef   rtk_bt_spp_evt_t
 * @brief     Bluetooth SPP event indication.
 */

typedef enum {
	RTK_BT_SPP_EVT_CONN_COMPL = 1,          /*!< SPP connection establishment completed. */
	RTK_BT_SPP_EVT_CREDIT_RCVD,             /*!< SPP link credits received. */
	RTK_BT_SPP_EVT_DATA_IND,                /*!< SPP data indication. */
	RTK_BT_SPP_EVT_DATA_RSP,                /*!< SPP data response. */
	RTK_BT_SPP_EVT_CONN_IND,                /*!< SPP connection indication. */
	RTK_BT_SPP_EVT_DISCONN_CMPL,            /*!< SPP disconnection completed. */
	RTK_BT_SPP_EVT_SDP_ATTR_INFO,           /*!< SPP get sdp attribute info. */
	RTK_BT_SPP_EVT_SDP_DISCOV_CMPL,         /*!< SPP sdp discovery completed. */
	RTK_BT_SPP_EVT_MAX,
} rtk_bt_spp_evt_t;

/**
 * @typedef   rtk_bt_rfc_act_t
 * @brief     Bluetooth RFC action.
 */

typedef enum {
	RTK_BT_RFC_ACT_CONNECT = 1,
	RTK_BT_RFC_ACT_DISCONNECT,
	RTK_BT_RFC_ACT_SEND_DATA,
	RTK_BT_RFC_ACT_CREDITS_GIVE,
	RTK_BT_RFC_ACT_MAX,
} rtk_bt_rfc_act_t;

/**
 * @typedef   rtk_bt_rfc_evt_t
 * @brief     Bluetooth RFC event indication.
 */

typedef enum {
	RTK_BT_RFC_EVT_RFC_ATTR_INFO = 1,        /*!< RFC get sdp attribute info. */
	RTK_BT_RFC_EVT_CONN_IND,                 /*!< RFC connection establishment completed. */
	RTK_BT_RFC_EVT_CONN_CMPL,                /*!< RFC link credits received. */
	RTK_BT_RFC_EVT_DISCONN_CMPL,             /*!< RFC data indication. */
	RTK_BT_RFC_EVT_DATA_IND,                 /*!< RFC data response. */
	RTK_BT_RFC_EVT_CREDITS_INFO,             /*!< RFC credits information. */
	RTK_BT_RFC_EVT_MAX,
} rtk_bt_rfc_evt_t;

/**
 * @typedef   rtk_bt_hid_act_t
 * @brief     Bluetooth HID action.
 */
typedef enum {
	RTK_BT_HID_ACT_DESCRIPTOR_ADD = 1,
	RTK_BT_HID_ACT_CONNECT,
	RTK_BT_HID_ACT_DISCONNECT,
	RTK_BT_HID_ACT_GET_REPORT_RSP,
	RTK_BT_HID_ACT_INPUT_DATA_SEND,
	RTK_BT_HID_ACT_MAX,
} rtk_bt_hid_act_t;

/**
 * @typedef   rtk_bt_hid_evt_t
 * @brief     Bluetooth HID event indication.
 */
typedef enum {
	RTK_BT_HID_EVT_CONN_IND = 1,                  /*!< HID connection indication. */
	RTK_BT_HID_EVT_CONN_CMPL,                     /*!< HID connection complete indication. */
	RTK_BT_HID_EVT_DISCONN_CMPL,                  /*!< HID disconnection complete indication. */
	RTK_BT_HID_EVT_MAX,
} rtk_bt_hid_evt_t;

/**
 * @typedef   rtk_bt_sdp_act_t
 * @brief     Bluetooth SDP action.
 */
typedef enum {
	RTK_BT_SDP_ACT_RECORD_ADD = 1,
	RTK_BT_SDP_ACT_DISCOV_START,
	RTK_BT_SDP_ACT_MAX,
} rtk_bt_sdp_act_t;

/**
 * @typedef   rtk_bt_sdp_evt_t
 * @brief     Bluetooth SDP event indication.
 */
typedef enum {
	RTK_BT_SDP_EVT_DID_ATTR_INFO = 1,
	RTK_BT_SDP_EVT_DISCOV_CMPL,
	RTK_BT_SDP_EVT_MAX,
} rtk_bt_sdp_evt_t;

/**
 * @typedef   rtk_bt_hfp_act_t
 * @brief     Bluetooth HFP action.
 */
typedef enum {
	RTK_BT_HFP_ACT_CONFIG_HF = 1,
	RTK_BT_HFP_ACT_CONFIG_AG,
	RTK_BT_HFP_ACT_CONNECT,
	RTK_BT_HFP_ACT_DISCONNECT,
	RTK_BT_HFP_ACT_SCO_CONNECT,
	RTK_BT_HFP_ACT_SCO_DISCONNECT,
	RTK_BT_HFP_ACT_CALL_INCOMING,
	RTK_BT_HFP_ACT_CALL_ANSWER,
	RTK_BT_HFP_ACT_CALL_TERMINATE,
	RTK_BT_HFP_ACT_DIAL_WITH_NUMBER,
	RTK_BT_HFP_ACT_DIAL_LAST_NUMBER,
	RTK_BT_HFP_ACT_SEND_SCO_DATA,
	RTK_BT_HFP_ACT_REPORT_BATT_LEVEL,
	RTK_BT_HFP_ACT_REPORT_SPEAKER_GAIN,
	RTK_BT_HFP_ACT_REPORT_MICROPHONE_GAIN,
	RTK_BT_HFP_ACT_VND_CMD_REQ,
	RTK_BT_HFP_ACT_MAX,
} rtk_bt_hfp_act_t;

/**
 * @typedef   rtk_bt_hfp_evt_t
 * @brief     Bluetooth HFP event indication.
 */
typedef enum {
	RTK_BT_HFP_EVT_SDP_ATTR_INFO = 1,                  /*!< HFP AG get hf sdp data info. */
	RTK_BT_HFP_EVT_SUPPORTED_FEATURES_IND,             /*!< HFP AG supported features indication. */
	RTK_BT_HFP_EVT_CONN_IND,                           /*!< HFP connection indication. */
	RTK_BT_HFP_EVT_CONN_CMPL,                          /*!< HFP connection completed. */
	RTK_BT_HFP_EVT_CALL_STATUS,                        /*!< HFP CALL status indication. */
	RTK_BT_HFP_EVT_CALLER_ID_IND,                      /*!< HFP CALLER number indication. */
	RTK_BT_HFP_EVT_DISCONN_CMPL,                       /*!< HFP disconnection completed. */
	RTK_BT_HFP_EVT_HF_BATTERY_IND,                     /*!< HFP HF battery indicator req. */
	RTK_BT_HFP_EVT_HF_SPEAKER_VOL_CHANGED_IND,         /*!< HFP HF speaker volume changed indicate. */
	RTK_BT_HFP_EVT_HF_MIC_VOL_CHANGED_IND,             /*!< HFP HF mic volume changed indicate. */
	RTK_BT_HFP_EVT_SCO_CONN_IND,                       /*!< HFP sco connection indication. */
	RTK_BT_HFP_EVT_SCO_CONN_CMPL,                      /*!< HFP sco connection completed. */
	RTK_BT_HFP_EVT_SCO_DATA_IND,                       /*!< HFP sco data indication. Direct calling, BT api shall not be called in this event case. */
	RTK_BT_HFP_EVT_SCO_DISCONNCTED_IND,                /*!< HFP sco disconnected indication. */
	RTK_BT_HFP_EVT_AG_CONN_IND,                        /*!< HFP ag conn indication. */
	RTK_BT_HFP_EVT_AG_CONN_CMPL,                       /*!< HFP ag conn complete. */
	RTK_BT_HFP_EVT_AG_DISCONN_CMPL,                    /*!< HFP ag disconnection complete. */
	RTK_BT_HFP_EVT_AG_INDICATORS_STATUS_REQ,           /*!< HFP ag disconnection complete. */
	RTK_BT_HFP_EVT_AG_CURR_CALLS_LIST_QUERY,           /*!< HFP ag current call list query. */
	RTK_BT_HFP_EVT_AG_DIAL_LAST_NUMBER,                /*!< HFP ag dial last number. */
	RTK_BT_HFP_EVT_AG_CALL_ANSWER_REQ,                 /*!< HFP ag call answer request. */
	RTK_BT_HFP_EVT_AG_CALL_TERMINATE_REQ,              /*!< HFP ag call terminate request. */
	RTK_BT_HFP_EVT_AG_CALL_STATUS,                     /*!< HFP ag call status incoming. */
	RTK_BT_HFP_EVT_UNKNOWN_EVENT_IND,                  /*!< HFP unknown event indication. */
	RTK_BT_HFP_EVT_MAX,
} rtk_bt_hfp_evt_t;

/**
 * @typedef   rtk_bt_pbap_act_t
 * @brief     Bluetooth PBAP action.
 */
typedef enum {
	RTK_BT_PBAP_ACT_CONNECT = 1,
	RTK_BT_PBAP_ACT_DISCONNECT,
	RTK_BT_PBAP_ACT_SET_PHONE_BOOK,
	RTK_BT_PBAP_ACT_PHONE_BOOK_PULL,
	RTK_BT_PBAP_ACT_PHONE_BOOK_PULL_CONTINUE,
	RTK_BT_PBAP_ACT_PHONE_BOOK_PULL_ABORT,
	RTK_BT_PBAP_ACT_PHONE_BOOK_SIZE_GET,
	RTK_BT_PBAP_ACT_PULL_CALLER_ID_NAME,
	RTK_BT_PBAP_ACT_MAX,
} rtk_bt_pbap_act_t;

/**
 * @typedef   rtk_bt_pbap_evt_t
 * @brief     Bluetooth PBAP event indication.
 */
typedef enum {
	RTK_BT_PBAP_EVT_CONN_CMPL = 1,                  /*!< PBAP connection completion. */
	RTK_BT_PBAP_EVT_CONN_FAIL,                      /*!< PBAP connection fail. */
	RTK_BT_PBAP_EVT_DISCONN_CMPL,                   /*!< PBAP disconnection completion. */
	RTK_BT_PBAP_EVT_SET_PHONE_BOOK_CMPL,            /*!< PBAP set phone book completion. */
	RTK_BT_PBAP_EVT_GET_PHONE_BOOK_CMPL,            /*!< PBAP get phone book completion. */
	RTK_BT_PBAP_EVT_CALLER_ID_NAME,                 /*!< PBAP caller id name indication. */
	RTK_BT_PBAP_EVT_GET_PHONE_BOOK_SIZE_CMPL,       /*!< PBAP get phone book size completion. */
	RTK_BT_PBAP_EVT_MAX,
} rtk_bt_pbap_evt_t;

/********************************* BT GAP common group *********************************/

/**
 * @typedef   rtk_bt_group_t
 * @brief     Bluetooth common action group.
 */
typedef enum {
	RTK_BT_COMMON_GP_GAP = RTK_BT_API_COMMON_BASE,    /*!< Common GAP group. */
	RTK_BT_COMMON_GP_MAX,                             /*!< Reserved. */
} rtk_bt_group_t;

/**
 * @typedef   rtk_bt_gap_act_t
 * @brief     Bluetooth common GAP action.
 */
typedef enum {
	RTK_BT_GAP_ACT_ECFC_SET_PARAM = 1,        /*!< Set the L2CAP ECFC parameters. */
	RTK_BT_GAP_ACT_ECFC_SEC_REG_REQ,          /*!< Register or unregister the security entry of an L2CAP ECFC protocol. */
	RTK_BT_GAP_ACT_ECFC_REGISTER,             /*!< Register a callback function to handle L2CAP ECFC related messages for a specific PSM. */
	RTK_BT_GAP_ACT_ECFC_CONN_REQ,             /*!< Send a request to create L2CAP ECFC connections. */
	RTK_BT_GAP_ACT_ECFC_CONN_CFM,             /*!< Confirm an L2CAP ECFC connection request. */
	RTK_BT_GAP_ACT_ECFC_DISCONN_REQ,          /*!< Request an L2CAP ECFC disconnection. */
	RTK_BT_GAP_ACT_ECFC_SEND_DATA,            /*!< Send data over an L2CAP ECFC channel. */
	RTK_BT_GAP_ACT_ECFC_RECONF_REQ,           /*!< Request an L2CAP ECFC channel reconfiguration. */
	RTK_BT_GAP_ACT_VENDOR_CMD_REQ,            /*!< Send a vendor command to the controller. */
	RTK_BT_GAP_ACT_MAX,                       /*!< Reserved. */
} rtk_bt_gap_act_t;

/**
 * @typedef   rtk_bt_gap_evt_t
 * @brief     Bluetooth common GAP event indication.
 */
typedef enum {
	RTK_BT_GAP_EVT_ECFC_CONN_CMPL_IND = 1,    /*!< Indicate the L2CAP ECFC connection establishment result, with msg @ref rtk_bt_ecfc_conn_cmpl_ind_t. */
	RTK_BT_GAP_EVT_ECFC_DATA_IND,             /*!< Indicate the L2CAP ECFC data received, with msg @ref rtk_bt_ecfc_data_ind_t. */
	RTK_BT_GAP_EVT_ECFC_CONN_REQ_IND,         /*!< Indicate an L2CAP ECFC connection request, with msg @ref rtk_bt_ecfc_conn_req_ind_t. */
	RTK_BT_GAP_EVT_ECFC_DISCONN_IND,          /*!< Indicate an L2CAP ECFC disconnection, with msg @ref rtk_bt_ecfc_disconn_ind_t. */
	RTK_BT_GAP_EVT_ECFC_RECONF_REQ_IND,       /*!< Indicate an L2CAP ECFC reconfiguration request, with msg @ref rtk_bt_ecfc_reconf_req_ind_t.
                                               *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_GAP_EVT_ECFC_RECONF_RSP_IND,       /*!< Indicate an L2CAP ECFC reconfiguration response, with msg @ref rtk_bt_ecfc_reconf_rsp_ind_t. */
	RTK_BT_GAP_EVT_FW_EXCEPTION_IND,          /*!< Indicate a BT firmware exception, with no msg. */
	RTK_BT_GAP_EVT_MAX,                       /*!< Reserved. */
} rtk_bt_gap_evt_t;

/**
 * @enum      rtk_bt_audio_err_common
 * @brief     Bluetooth audio API common error code.
 */
enum rtk_bt_audio_err_common {
	RTK_BT_AUDIO_OK                     =   0x00,   /*!< Success. */
	RTK_BT_AUDIO_FAIL                   =   0x01,   /*!< Common failure. */
	RTK_BT_AUDIO_GET_VALUE_FAIL         =   0xFF,   /*!< Failed to get the value. */
};

/**
 * @typedef   rtk_bt_audio_codec_t
 * @brief     Bluetooth audio codec type bit mask.
 */
typedef enum {
	RTK_BT_AUDIO_CODEC_SBC          =   BIT0,       /*!< SBC codec. */
	RTK_BT_AUDIO_CODEC_mSBC         =   BIT1,       /*!< mSBC codec. */
	RTK_BT_AUDIO_CODEC_AAC          =   BIT2,       /*!< AAC codec. */
	RTK_BT_AUDIO_CODEC_LC3          =   BIT3,       /*!< LC3 codec. */
	RTK_BT_AUDIO_CODEC_CVSD         =   BIT4,       /*!< CVSD codec. */
	RTK_BT_AUDIO_CODEC_MAX          =   BIT31,      /*!< Reserved. */
} rtk_bt_audio_codec_t;

/**
 * @typedef   rtk_bt_evt_cb_ret_t
 * @brief     Bluetooth event callback execution result.
 */
typedef enum {
	RTK_BT_EVT_CB_OK = 0,        /*!< The callback executed successfully. */
	RTK_BT_EVT_CB_ACCEPT = 1,    /*!< The callback accepted the request. */
	RTK_BT_EVT_CB_REJECT = 2,    /*!< The callback rejected the request. */
	RTK_BT_EVT_CB_FAIL = 3,      /*!< The callback failed to execute. */
} rtk_bt_evt_cb_ret_t;

/**
 * @typedef   rtk_bt_evt_cb_t
 * @brief     Bluetooth event callback function type definition.
 * @param[in] evt_code: Event code of the group the callback is registered to.
 * @param[in] data: Event data.
 * @param[in] data_len: Event data length.
 * @return
 *            - @ref rtk_bt_evt_cb_ret_t
 */
typedef rtk_bt_evt_cb_ret_t (*rtk_bt_evt_cb_t)(uint8_t evt_code, void *data, uint32_t data_len);

/************************* Data structures for API internal use ************************/

/**
 * @struct    rtk_bt_cmd_t
 * @brief     Bluetooth API command message.
 */
typedef struct {
	struct list_head list;    /*!< List node used to link the command in the pending command list. */
	uint8_t group;            /*!< Command group. */
	uint8_t act;              /*!< Command action within the group. */
	uint32_t user_data;       /*!< Stack private data. */
	void *param;              /*!< Command parameter. NULL if the action has no parameter. */
	uint32_t param_len;       /*!< Command parameter length. */
	uint16_t ret;             /*!< Command execution result. */
	void *psem;               /*!< Semaphore the API caller waits on until the command is executed. */
} rtk_bt_cmd_t;

/**
 * @struct    rtk_bt_evt_t
 * @brief     Bluetooth event message.
 */
typedef struct {
	uint8_t group;        /*!< Event group. */
	uint8_t evt;          /*!< Event code within the group. */
	uint32_t data_len;    /*!< Event data length. */
	void *data;           /*!< Event data. */
} rtk_bt_evt_t;

/********************************* Functions Declaration *******************************/

/**
 * @defgroup  bt_common BT Common APIs
 * @brief     BT common function APIs.
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Convert an LE device address to a string in the format "XX:XX:XX:XX:XX:XX(type)".
 *            The type is "public", "random", "public-id" or "random-id" for @ref RTK_BT_LE_ADDR_TYPE_PUBLIC, @ref RTK_BT_LE_ADDR_TYPE_RANDOM,
 *            @ref RTK_BT_LE_ADDR_TYPE_RPA_PUBLIC and @ref RTK_BT_LE_ADDR_TYPE_RPA_RANDOM respectively, otherwise it is the hexadecimal
 *            value of the address type, such as "0xff".
 * @param[in] paddr: Pointer to the 6-byte LE device address of type @ref rtk_bt_le_addr_t, whose address value is stored in little-endian order.
 *                   Must not be NULL.
 * @param[out] str: Buffer that receives the address string.
 *                  Must not be NULL.
 * @param[in] len: Length of the buffer in bytes. It should be at least 29 to hold the longest string, such as "XX:XX:XX:XX:XX:XX(random-id)",
 *                 and the terminating NUL.
 */
void rtk_bt_le_addr_to_str(void *paddr, char *str, uint32_t len);

/**
 * @brief     Convert a device address value to a string in the format "XX:XX:XX:XX:XX:XX".
 * @param[in] paddr: Pointer to the 6-byte device address value, stored in little-endian order
 *                   (paddr[0] is the least significant byte and is printed last).
 *                   Must not be NULL.
 * @param[out] str: Buffer that receives the address string.
 *                  Must not be NULL.
 * @param[in] len: Length of the buffer in bytes. It should be at least 18 to hold the string and the terminating NUL.
 */
void rtk_bt_addr_val_to_str(uint8_t *paddr, char *str, uint32_t len);

/**
 * @brief     Convert a BR/EDR device address value to a string in the format "XX:XX:XX:XX:XX:XX".
 * @param[in] paddr: Pointer to the 6-byte BR/EDR device address value, stored in little-endian order
 *                   (paddr[0] is the least significant byte and is printed last).
 *                   Must not be NULL.
 * @param[out] str: Buffer that receives the address string.
 *                  Must not be NULL.
 * @param[in] len: Length of the buffer in bytes. It should be at least 18 to hold the string and the terminating NUL.
 */
void rtk_bt_br_addr_to_str(uint8_t *paddr, char *str, uint32_t len);

/**
 * @brief     Convert a device address to a string in the format "XX:XX:XX:XX:XX:XX(type)".
 *            The type is "le public", "le random", "le public-id", "le random-id" or "classic" for @ref RTK_BT_ADDR_LE_PUBLIC,
 *            @ref RTK_BT_ADDR_LE_RANDOM, @ref RTK_BT_ADDR_LE_RPA_PUBLIC, @ref RTK_BT_ADDR_LE_RPA_RANDOM and @ref RTK_BT_ADDR_CLASSIC
 *            respectively, otherwise it is the hexadecimal value of the address type, such as "0xff".
 * @param[in] type: Device address type, see @ref rtk_bt_addr_type_t.
 * @param[in] paddr: Pointer to the 6-byte device address value, stored in little-endian order
 *                   (paddr[0] is the least significant byte and is printed last).
 *                   Must not be NULL.
 * @param[out] str: Buffer that receives the address string.
 *                  Must not be NULL.
 * @param[in] len: Length of the buffer in bytes. It should be at least 32 to hold the longest string, such as "XX:XX:XX:XX:XX:XX(le random-id)",
 *                 and the terminating NUL.
 */
void rtk_bt_addr_to_str(uint8_t type, uint8_t *paddr, char *str, uint32_t len);

/**
 * @brief     Register the event callback of an API command/event group. A callback registered to the same group previously is replaced.
 * @param[in] group: API command/event group.
 * @param[in] cb: Event callback to register to the group.
 * @return
 *            - @ref rtk_bt_err_common
 * @note      - The callback is executed in the BT event task, except for direct calling events, which are executed in the stack task.
 *            - If the callback is complex and consumes too much stack, enlarge the stack size of the BT event task @ref EVENT_TASK_STACK_SIZE.
 */
uint16_t rtk_bt_evt_register_callback(uint8_t group, rtk_bt_evt_cb_t cb);

/**
 * @brief     Unregister the event callback of an API command/event group.
 * @param[in] group: API command/event group.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_evt_unregister_callback(uint8_t group);

/**
 * @}
 */

/**************************** Functions for API internal use ***************************/

#if defined(RTK_BT_API_MEM_PRE_ALLOC) && RTK_BT_API_MEM_PRE_ALLOC
/**
 * @brief     Pre-allocate the semaphore pool used by API commands.
 * @return
 *            - true: pre-allocate success.
 *            - false: pre-allocate failed.
 */
bool bt_api_sem_pool_init(void);

/**
 * @brief     Wait until all semaphores are given back to the pool, then release the semaphore pool.
 */
void bt_api_sem_pool_deinit(void);

/**
 * @brief     Pre-allocate the memory pool used by event messages.
 * @return
 *            - true: pre-allocate success.
 *            - false: pre-allocate failed.
 */
bool bt_evt_mem_pool_init(void);

/**
 * @brief     Release the memory pool used by event messages.
 */
void bt_evt_mem_pool_deinit(void);
#endif

/**
 * @brief     Send an API command to the API task and wait until it is executed.
 * @param[in] group: Command group.
 * @param[in] act: Command action within the group.
 * @param[in,out] param: Command parameter. NULL if the action has no parameter.
 * @param[in] param_len: Length of the command parameter.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_send_cmd(uint8_t group, uint8_t act, void *param, uint32_t param_len);

/**
 * @brief     Wait until all API commands that are being sent to the API task have been sent.
 */
void bt_wait_cmd_send_complete(void);

/**
 * @brief     Create the BT event task and its message queue and semaphore.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_evt_init(void);

/**
 * @brief     Delete the BT event task and its message queue and semaphore, and reset all registered event callbacks.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_evt_deinit(void);

/**
 * @brief     Allocate and initialize an event message.
 * @param[in] group: Event group.
 * @param[in] evt: Event code within the group.
 * @param[in] param_len: Length of the event data area.
 * @return
 *            - Pointer to the event message: Success.
 *            - NULL: Memory allocation failed.
 */
rtk_bt_evt_t *rtk_bt_event_create(uint8_t group, uint8_t evt, uint32_t param_len);

/**
 * @brief     Indicate an event to the application. A direct calling event is executed immediately in the caller context.
 *            Other events are sent to the BT event task.
 * @param[in] evt: Event message created by @ref rtk_bt_event_create.
 *                 Must not be NULL.
 * @param[out] cb_ret: Receives the return value of the event callback for a direct calling event.
 *                     May be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_evt_indicate(void *evt, uint8_t *cb_ret);

/**
 * @brief     Release an event message created by @ref rtk_bt_event_create.
 * @param[in] pevt: Event message to release.
 */
void rtk_bt_event_free(rtk_bt_evt_t *pevt);

#define BT_STRUCT_TAIL(ptr, type)   ((void *)((uint8_t *)ptr + sizeof(type)))

#define LE_TO_U32(_a)                                   \
        (((uint32_t)(*((uint8_t *)(_a) + 0)) << 0)  |   \
         ((uint32_t)(*((uint8_t *)(_a) + 1)) << 8)  |   \
         ((uint32_t)(*((uint8_t *)(_a) + 2)) << 16) |   \
         ((uint32_t)(*((uint8_t *)(_a) + 3)) << 24))

#define LE_TO_U16(_a)                           \
        (((uint16_t)(*((uint8_t *)(_a) + 0)) << 0)  |   \
         ((uint16_t)(*((uint8_t *)(_a) + 1)) << 8))

#define U32_TO_LE(_a, _data) {                          \
        *((uint8_t *)(_a) + 0) = ((_data) >> 0) & 0xFF; \
        *((uint8_t *)(_a) + 1) = ((_data) >> 8) & 0xFF; \
        *((uint8_t *)(_a) + 2) = ((_data) >> 16) & 0xFF;\
        *((uint8_t *)(_a) + 3) = ((_data) >> 24) & 0xFF;\
    }

#define U16_TO_LE(_a, _data) {                          \
        *((uint8_t *)(_a) + 0) = ((_data) >> 0) & 0xFF; \
        *((uint8_t *)(_a) + 1) = ((_data) >> 8) & 0xFF; \
    }

#ifndef HI_WORD
#define HI_WORD(x)      ((uint8_t)((x & 0xFF00) >> 8))
#endif

#ifndef LO_WORD
#define LO_WORD(x)      ((uint8_t)(x))
#endif

#define BT_APP_PROCESS(func)                                \
    do {                                                    \
        uint16_t __func_ret = func;                         \
        if (RTK_BT_OK != __func_ret) {                      \
            BT_LOGE("[APP] %s failed! line: %d, err: 0x%x\r\n", __func__, __LINE__, __func_ret);   \
            return -1;                                      \
        }                                                   \
    } while (0)

#define BT_APP_EVT_CB_PROCESS(func)                                \
    do {                                                    \
        uint16_t __func_ret = func;                         \
        if (RTK_BT_OK != __func_ret) {                      \
            BT_LOGE("[APP] %s failed! line: %d, err: 0x%x\r\n", __func__, __LINE__, __func_ret);   \
            return RTK_BT_EVT_CB_FAIL;                                      \
        }                                                   \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_COMMON_H__ */
