/**
 * @file      rtk_bt_mesh_time_model.h
 * @author    quinn_yang@realsil.com.cn
 * @brief     Bluetooth LE MESH time client and server model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_TIME_MODEL_H__
#define __RTK_BT_MESH_TIME_MODEL_H__

#include <rtk_bt_mesh_def.h>

/************************************** Time Model *************************************/
/**
 * @struct    rtk_bt_mesh_tai_time_t
 * @brief     BLE MESH tai time structure.
 */
typedef struct {
	uint8_t tai_seconds[5];        /*!< TAI Seconds state: current TAI time in seconds since the epoch 2000-01-01T00:00:00 TAI.
                                    *   Little endian, 40 bits.
                                    *   Range: 0x0000000000 to 0xFFFFFFFFFF.
                                    *   0x0000000000: The element cannot determine the time. */
	uint8_t subsecond;             /*!< Subsecond state: fractional part of the TAI time.
                                    *   Time = N / 256 s.
                                    *   Range: 0x00 to 0xFF. */
	uint8_t uncertainty;           /*!< Uncertainty state: estimated uncertainty of the time.
                                    *   Time = N * 10 ms.
                                    *   Range: 0x00 to 0xFF.
                                    *   0xFF: The uncertainty is 2.55 s or more. */
	uint16_t time_authority: 1;    /*!< Time Authority state.
                                    *   0: The element does not have a trusted OOB source of time, such as GPS or NTP.
                                    *   1: The element has a trusted OOB source of time, such as GPS or NTP or a battery-backed, properly initialized RTC. */
	uint16_t tai_utc_delta: 15;    /*!< TAI-UTC Delta Current state: current difference between TAI and UTC in seconds.
                                    *   Range: 0x0000 to 0x7FFF.
                                    *   Delta = N - 255 s.
                                    *   Delta Range: -255 s to +32512 s. */
	uint8_t time_zone_offset;      /*!< Time Zone Offset Current state: current local time zone offset in 15-minute increments from UTC, positive eastwards.
                                    *   Range: 0x00 to 0xFF.
                                    *   Offset = (N - 64) * 15 min.
                                    *   Offset Range: -16 h to +47.75 h. */
} _PACKED4_ rtk_bt_mesh_tai_time_t;

/**
 * @struct    rtk_bt_mesh_time_zone_t
 * @brief     BLE MESH time zone structure.
 */
typedef struct {
	uint8_t time_zone_offset_current;    /*!< Time Zone Offset Current state: current local time zone offset in 15-minute increments from UTC, positive eastwards.
                                          *   Range: 0x00 to 0xFF.
                                          *   Offset = (N - 64) * 15 min.
                                          *   Offset Range: -16 h to +47.75 h. */
	uint8_t time_zone_offset_new;        /*!< Time Zone Offset New state: upcoming local time zone offset in 15-minute increments from UTC, positive eastwards.
                                          *   Range: 0x00 to 0xFF.
                                          *   Offset = (N - 64) * 15 min.
                                          *   Offset Range: -16 h to +47.75 h. */
	uint8_t tai_of_zone_change[5];       /*!< TAI of Zone Change state: TAI time when the Time Zone Offset New is applied, in the TAI Seconds format.
                                          *   Little endian, 40 bits.
                                          *   Range: 0x0000000000 to 0xFFFFFFFFFF.
                                          *   0x0000000000: The TAI of Zone Change is unknown. */
} _PACKED4_ rtk_bt_mesh_time_zone_t;

/**
 * @struct    rtk_bt_mesh_tai_utc_delta_t
 * @brief     BLE MESH tai utc delta structure.
 */
typedef struct {
	uint16_t tai_utc_delta_current: 15;    /*!< TAI-UTC Delta Current state: current difference between TAI and UTC in seconds.
                                            *   Range: 0x0000 to 0x7FFF.
                                            *   Delta = N - 255 s.
                                            *   Delta Range: -255 s to +32512 s. */
	uint16_t padding1: 1;                  /*!< Padding, always 0.
                                            *   Other values are Prohibited. */
	uint16_t tai_utc_delta_new: 15;        /*!< TAI-UTC Delta New state: upcoming difference between TAI and UTC in seconds.
                                            *   Range: 0x0000 to 0x7FFF.
                                            *   Delta = N - 255 s.
                                            *   Delta Range: -255 s to +32512 s. */
	uint16_t padding2: 1;                  /*!< Padding, always 0.
                                            *   Other values are Prohibited. */
	uint8_t tai_of_delta_change[5];        /*!< TAI of Delta Change state: TAI time when the TAI-UTC Delta New is applied, in the TAI Seconds format.
                                            *   Little endian, 40 bits.
                                            *   Range: 0x0000000000 to 0xFFFFFFFFFF.
                                            *   0x0000000000: The TAI of Delta Change is unknown. */
} _PACKED4_ rtk_bt_mesh_tai_utc_delta_t;

/**
 * @enum      rtk_bt_mesh_time_role_t
 * @brief     BLE MESH time role definition.
 */
typedef enum {
	RTK_BT_MESH_TIME_ROLE_NONE,         /*!< The element does not participate in propagation of time information. */
	RTK_BT_MESH_TIME_ROLE_AUTHORITY,    /*!< The element publishes Time Status messages but does not process received Time Status messages. */
	RTK_BT_MESH_TIME_ROLE_RELAY,        /*!< The element processes received and publishes Time Status messages. */
	RTK_BT_MESH_TIME_ROLE_CLIENT        /*!< The element does not publish but processes received Time Status messages. */
} rtk_bt_mesh_time_role_t;

/********************************** Time Client Model **********************************/
/**
 * @enum      rtk_bt_mesh_time_client_model_act_t
 * @brief     BLE MESH time client model act definition.
 */
typedef enum {
	RTK_BT_MESH_TIME_CLIENT_MODEL_GET = 1,              /*!< Send a Time Get message to the server. */
	RTK_BT_MESH_TIME_CLIENT_MODEL_ZONE_GET,             /*!< Send a Time Zone Get message to the server. */
	RTK_BT_MESH_TIME_CLIENT_MODEL_TAI_UTC_DELTA_GET,    /*!< Send a TAI-UTC Delta Get message to the server. */
	RTK_BT_MESH_TIME_CLIENT_MODEL_ROLE_GET,             /*!< Send a Time Role Get message to the server. */
	RTK_BT_MESH_TIME_CLIENT_MODEL_SET,                  /*!< Send a Time Set message to the server. */
	RTK_BT_MESH_TIME_CLIENT_MODEL_ZONE_SET,             /*!< Send a Time Zone Set message to the server. */
	RTK_BT_MESH_TIME_CLIENT_MODEL_TAI_UTC_DELTA_SET,    /*!< Send a TAI-UTC Delta Set message to the server. */
	RTK_BT_MESH_TIME_CLIENT_MODEL_ROLE_SET,             /*!< Send a Time Role Set message to the server. */
	RTK_BT_MESH_TIME_CLIENT_MODEL_ACT_MAX,              /*!< Reserved. */
} rtk_bt_mesh_time_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_time_client_model_evt_t
 * @brief     BLE MESH time client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_TIME_CLIENT_MODEL_STATUS = 1,              /*!< Indicate Time Status message received from the server, with msg
                                                            *   @ref rtk_bt_mesh_time_client_status_t. */
	RTK_BT_MESH_TIME_CLIENT_MODEL_ROLE_STATUS,             /*!< Indicate Time Role Status message received from the server, with msg
                                                            *   @ref rtk_bt_mesh_time_client_status_role_t. */
	RTK_BT_MESH_TIME_CLIENT_MODEL_ZONE_STATUS,             /*!< Indicate Time Zone Status message received from the server, with msg
                                                            *   @ref rtk_bt_mesh_time_client_status_zone_t. */
	RTK_BT_MESH_TIME_CLIENT_MODEL_TAI_UTC_DELTA_STATUS,    /*!< Indicate TAI-UTC Delta Status message received from the server, with msg
                                                            *   @ref rtk_bt_mesh_time_client_status_tai_utc_delta_t. */
	RTK_BT_MESH_TIME_CLIENT_MODEL_EVT_MAX,                 /*!< Reserved. */
} rtk_bt_mesh_time_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_time_get_t
 * @brief     BLE MESH time get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_time_get_t;

/**
 * @struct    rtk_bt_mesh_time_zone_get_t
 * @brief     BLE MESH time zone get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_time_zone_get_t;

/**
 * @struct    rtk_bt_mesh_time_tai_utc_delta_get_t
 * @brief     BLE MESH time tai utc delta get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_time_tai_utc_delta_get_t;

/**
 * @struct    rtk_bt_mesh_time_role_get_t
 * @brief     BLE MESH time role get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_time_role_get_t;

/**
 * @struct    rtk_bt_mesh_time_set_t
 * @brief     BLE MESH time set act message structure.
 */
typedef struct {
	uint16_t dst;                   /*!< Destination address of the message. */
	uint16_t app_key_index;         /*!< Index of the application key used to encrypt the message.
                                     *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_tai_time_t time;    /*!< Time state to set. */
} _PACKED4_ rtk_bt_mesh_time_set_t;

/**
 * @struct    rtk_bt_mesh_time_zone_set_t
 * @brief     BLE MESH time zone set act message structure.
 */
typedef struct {
	uint16_t dst;                     /*!< Destination address of the message. */
	uint16_t app_key_index;           /*!< Index of the application key used to encrypt the message.
                                       *   Range: 0x0000 to 0x0FFF. */
	uint8_t time_zone_offset_new;     /*!< Time Zone Offset New state to set: upcoming local time zone offset in 15-minute increments from UTC, positive eastwards.
                                       *   Range: 0x00 to 0xFF.
                                       *   Offset = (N - 64) * 15 min.
                                       *   Offset Range: -16 h to +47.75 h. */
	uint8_t tai_of_zone_change[5];    /*!< TAI of Zone Change state to set: TAI time when the Time Zone Offset New is applied, in the TAI Seconds format.
                                       *   Little endian, 40 bits.
                                       *   Range: 0x0000000000 to 0xFFFFFFFFFF.
                                       *   0x0000000000: The TAI of Zone Change is unknown. */
} rtk_bt_mesh_time_zone_set_t;

/**
 * @struct    rtk_bt_mesh_time_tai_utc_delta_set_t
 * @brief     BLE MESH time tai utc delta set act message structure.
 */
typedef struct {
	uint16_t dst;                      /*!< Destination address of the message. */
	uint16_t app_key_index;            /*!< Index of the application key used to encrypt the message.
                                        *   Range: 0x0000 to 0x0FFF. */
	uint16_t tai_utc_delta_new;        /*!< TAI-UTC Delta New state to set: upcoming difference between TAI and UTC in seconds.
                                        *   Range: 0x0000 to 0x7FFF.
                                        *   Delta = N - 255 s.
                                        *   Delta Range: -255 s to +32512 s. */
	uint8_t tai_of_delta_change[5];    /*!< TAI of Delta Change state to set: TAI time when the TAI-UTC Delta New is applied, in the TAI Seconds format.
                                        *   Little endian, 40 bits.
                                        *   Range: 0x0000000000 to 0xFFFFFFFFFF.
                                        *   0x0000000000: The TAI of Delta Change is unknown. */
} rtk_bt_mesh_time_tai_utc_delta_set_t;

/**
 * @struct    rtk_bt_mesh_time_role_set_t
 * @brief     BLE MESH time role set act message structure.
 */
typedef struct {
	uint16_t dst;                    /*!< Destination address of the message. */
	uint16_t app_key_index;          /*!< Index of the application key used to encrypt the message.
                                      *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_time_role_t role;    /*!< Time Role state to set.
                                      *   0x04 to 0xFF: Prohibited. */
} _PACKED4_ rtk_bt_mesh_time_role_set_t;

/**
 * @struct    rtk_bt_mesh_time_client_status_t
 * @brief     BLE MESH time client model status message structure.
 */
typedef struct {
	uint16_t src;                       /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_tai_time_t tai_time;    /*!< Time state reported by the server.
                                         *   The fields other than tai_seconds are not carried when tai_seconds is 0x0000000000. */
} _PACKED4_ rtk_bt_mesh_time_client_status_t;

/**
 * @struct    rtk_bt_mesh_time_client_status_zone_t
 * @brief     BLE MESH time client model zone status message structure.
 */
typedef struct {
	uint16_t src;                        /*!< Unicast address of the server that sent the Status message. */
	uint8_t time_zone_offset_current;    /*!< Time Zone Offset Current state of the server.
                                          *   Range: 0x00 to 0xFF.
                                          *   Offset = (N - 64) * 15 min.
                                          *   Offset Range: -16 h to +47.75 h. */
	uint8_t time_zone_offset_new;        /*!< Time Zone Offset New state of the server.
                                          *   Range: 0x00 to 0xFF.
                                          *   Offset = (N - 64) * 15 min.
                                          *   Offset Range: -16 h to +47.75 h. */
	uint8_t tai_of_zone_change[5];       /*!< TAI of Zone Change state of the server.
                                          *   Little endian, 40 bits.
                                          *   Range: 0x0000000000 to 0xFFFFFFFFFF.
                                          *   0x0000000000: The TAI of Zone Change is unknown. */
} rtk_bt_mesh_time_client_status_zone_t;

/**
 * @struct    rtk_bt_mesh_time_client_status_tai_utc_delta_t
 * @brief     BLE MESH time client model tai utc delta status message structure.
 */
typedef struct {
	uint16_t src;                          /*!< Unicast address of the server that sent the Status message. */
	uint16_t tai_utc_delta_current: 15;    /*!< TAI-UTC Delta Current state of the server.
                                            *   Range: 0x0000 to 0x7FFF.
                                            *   Delta = N - 255 s.
                                            *   Delta Range: -255 s to +32512 s. */
	uint16_t padding1: 1;                  /*!< Padding, always 0. */
	uint16_t tai_utc_delta_new: 15;        /*!< TAI-UTC Delta New state of the server.
                                            *   Range: 0x0000 to 0x7FFF.
                                            *   Delta = N - 255 s.
                                            *   Delta Range: -255 s to +32512 s. */
	uint16_t padding2: 1;                  /*!< Padding, always 0. */
	uint8_t tai_of_delta_change[5];        /*!< TAI of Delta Change state of the server.
                                            *   Little endian, 40 bits.
                                            *   Range: 0x0000000000 to 0xFFFFFFFFFF.
                                            *   0x0000000000: The TAI of Delta Change is unknown. */
} rtk_bt_mesh_time_client_status_tai_utc_delta_t;

/**
 * @struct    rtk_bt_mesh_time_client_status_role_t
 * @brief     BLE MESH time client model role status message structure.
 */
typedef struct {
	rtk_bt_mesh_time_role_t role;    /*!< Time Role state of the server. */
} rtk_bt_mesh_time_client_status_role_t;


/********************************** Time Server Model **********************************/

/**
 * @enum      rtk_bt_mesh_time_server_model_evt_t
 * @brief     BLE MESH time server model event definition.
 */
typedef enum {
	RTK_BT_MESH_TIME_SERVER_MODEL_GET = 1,              /*!< Indicate a Time Get message is received or the server needs the current Time state, with msg
                                                         *   @ref rtk_bt_mesh_time_server_direct_get_t.
                                                         *   The application shall write the current Time state into the msg.
                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_TIME_SERVER_MODEL_ZONE_GET,             /*!< Indicate a Time Zone Get message received from a client, with msg
                                                         *   @ref rtk_bt_mesh_time_server_direct_get_zone_t.
                                                         *   The application shall write the current Time Zone state into the msg.
                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_TIME_SERVER_MODEL_TAI_UTC_DELTA_GET,    /*!< Indicate a TAI-UTC Delta Get message received from a client, with msg
                                                         *   @ref rtk_bt_mesh_time_server_direct_get_tai_utc_delta_t.
                                                         *   The application shall write the current TAI-UTC Delta state into the msg.
                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_TIME_SERVER_MODEL_STATUS_SET,           /*!< Indicate the Time state is to be updated from a received Time Status message, with msg
                                                         *   @ref rtk_bt_mesh_time_server_status_set_t.
                                                         *   It follows event @ref RTK_BT_MESH_TIME_SERVER_MODEL_ROLE_GET.
                                                         *   Raised only when the Time Role reported there is Time Relay or Time Client.
                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_TIME_SERVER_MODEL_ROLE_GET,             /*!< Indicate the server needs the current Time Role state to process a received Time Status message, with msg
                                                         *   @ref rtk_bt_mesh_time_server_get_role_t.
                                                         *   The application shall write the current Time Role state into the msg.
                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_TIME_SERVER_MODEL_EVT_MAX,              /*!< Reserved. */
} rtk_bt_mesh_time_server_model_evt_t;

/**
 * @typedef   rtk_bt_mesh_time_server_get_t
 * @brief     BLE MESH time get evt message structure.
 */
typedef rtk_bt_mesh_tai_time_t               rtk_bt_mesh_time_server_get_t;

/**
 * @struct    rtk_bt_mesh_time_server_direct_get_t
 * @brief     BLE MESH time direct get evt message structure.
 */
typedef struct {
	rtk_bt_mesh_time_server_get_t *value;    /*!< Output: the application writes the current Time state here.
                                              *   Must not be NULL. */
} rtk_bt_mesh_time_server_direct_get_t;

/**
 * @typedef   rtk_bt_mesh_time_server_get_zone_t
 * @brief     BLE MESH time zone get evt message structure.
 */
typedef rtk_bt_mesh_time_zone_t              rtk_bt_mesh_time_server_get_zone_t;

/**
 * @struct    rtk_bt_mesh_time_server_direct_get_zone_t
 * @brief     BLE MESH time zone direct get evt message structure.
 */
typedef struct {
	rtk_bt_mesh_time_server_get_zone_t *value;    /*!< Output: the application writes the current Time Zone Offset Current, Time Zone Offset New and TAI of Zone
                                                   *   Change states here.
                                                   *   Must not be NULL. */
} rtk_bt_mesh_time_server_direct_get_zone_t;

/**
 * @typedef   rtk_bt_mesh_time_server_get_tai_utc_delta_t
 * @brief     BLE MESH time tai utc delta get evt message structure.
 */
typedef rtk_bt_mesh_tai_utc_delta_t          rtk_bt_mesh_time_server_get_tai_utc_delta_t;

/**
 * @struct    rtk_bt_mesh_time_server_direct_get_tai_utc_delta_t
 * @brief     BLE MESH time tai utc delta direct get evt message structure.
 */
typedef struct {
	rtk_bt_mesh_time_server_get_tai_utc_delta_t *value;    /*!< Output: the application writes the current TAI-UTC Delta Current, TAI-UTC Delta New and TAI of Delta
                                                            *   Change states here.
                                                            *   Must not be NULL. */
} rtk_bt_mesh_time_server_direct_get_tai_utc_delta_t;

/**
 * @struct    rtk_bt_mesh_time_server_status_set_t
 * @brief     BLE MESH time server status set evt message structure.
 */
typedef struct {
	uint8_t tai_seconds[5];      /*!< TAI Seconds state to update.
                                  *   Little endian, 40 bits.
                                  *   Range: 0x0000000000 to 0xFFFFFFFFFF. */
	uint8_t subsecond;           /*!< Subsecond state to update.
                                  *   Time = N / 256 s.
                                  *   Range: 0x00 to 0xFF. */
	uint8_t uncertainty;         /*!< Uncertainty state to update.
                                  *   Time = N * 10 ms.
                                  *   Range: 0x00 to 0xFF.
                                  *   0xFF: The uncertainty is 2.55 s or more. */
	uint16_t tai_utc_delta;      /*!< TAI-UTC Delta Current state to update.
                                  *   Range: 0x0000 to 0x7FFF.
                                  *   Delta = N - 255 s.
                                  *   Delta Range: -255 s to +32512 s. */
	uint8_t time_zone_offset;    /*!< Time Zone Offset Current state to update.
                                  *   Range: 0x00 to 0xFF.
                                  *   Offset = (N - 64) * 15 min.
                                  *   Offset Range: -16 h to +47.75 h. */
} _PACKED4_ rtk_bt_mesh_time_server_status_set_t;

/******************************* Time Setup Server Model *******************************/
/**
 * @enum      rtk_bt_mesh_time_setup_server_model_evt_t
 * @brief     BLE MESH time setup server model event definition.
 */
typedef enum {
	RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_SET = 1,              /*!< Indicate a Time Set message received from a client, with msg
                                                               *   @ref rtk_bt_mesh_time_server_set_t.
                                                               *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_ZONE_SET,             /*!< Indicate a Time Zone Set message received from a client, with msg
                                                               *   @ref rtk_bt_mesh_time_server_set_zone_t.
                                                               *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_ROLE_GET,             /*!< Indicate a Time Role Get message is received or the server needs the current Time Role state, with msg
                                                               *   @ref rtk_bt_mesh_time_server_get_role_t.
                                                               *   The application shall write the current Time Role state into the msg.
                                                               *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_ROLE_SET,             /*!< Indicate a Time Role Set message received from a client, with msg
                                                               *   @ref rtk_bt_mesh_time_server_set_role_t.
                                                               *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_TAI_UTC_DELTA_SET,    /*!< Indicate a TAI-UTC Delta Set message received from a client, with msg
                                                               *   @ref rtk_bt_mesh_time_server_set_tai_utc_delta_t.
                                                               *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_ZONE_GET,             /*!< Indicate the server needs the current Time Zone state to reply to a Time Zone Set message, with msg
                                                               *   @ref rtk_bt_mesh_time_server_direct_get_zone_t.
                                                               *   It follows event @ref RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_ZONE_SET.
                                                               *   The application shall write the current Time Zone state into the msg.
                                                               *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_TAI_UTC_DELTA_GET,    /*!< Indicate the server needs the current TAI-UTC Delta state to reply to a TAI-UTC Delta Set message, with
                                                               *   msg @ref rtk_bt_mesh_time_server_direct_get_tai_utc_delta_t.
                                                               *   It follows event @ref RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_TAI_UTC_DELTA_SET.
                                                               *   The application shall write the current TAI-UTC Delta state into the msg.
                                                               *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_GET,                  /*!< Indicate the server needs the current Time state to reply to a Time Set message, with msg
                                                               *   @ref rtk_bt_mesh_time_server_direct_get_t.
                                                               *   It follows event @ref RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_SET.
                                                               *   The application shall write the current Time state into the msg.
                                                               *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_EVT_MAX,              /*!< Reserved. */
} rtk_bt_mesh_time_setup_server_model_evt_t;

/**
 * @typedef   rtk_bt_mesh_time_server_set_t
 * @brief     BLE MESH time set evt message structure.
 */
typedef rtk_bt_mesh_tai_time_t               rtk_bt_mesh_time_server_set_t;

/**
 * @struct    rtk_bt_mesh_time_server_get_role_t
 * @brief     BLE MESH time get role evt message structure.
 */
typedef struct {
	rtk_bt_mesh_time_role_t *role;    /*!< Output: the application writes the current Time Role state here.
                                       *   Must not be NULL. */
} rtk_bt_mesh_time_server_get_role_t;

/**
 * @struct    rtk_bt_mesh_time_server_set_role_t
 * @brief     BLE MESH time set role evt message structure.
 */
typedef struct {
	rtk_bt_mesh_time_role_t role;    /*!< Time Role state to set. */
} rtk_bt_mesh_time_server_set_role_t;

/**
 * @struct    rtk_bt_mesh_time_server_set_zone_t
 * @brief     BLE MESH time set zone evt message structure.
 */
typedef struct {
	uint8_t time_zone_offset_new;     /*!< Time Zone Offset New state requested by the client.
                                       *   Range: 0x00 to 0xFF.
                                       *   Offset = (N - 64) * 15 min.
                                       *   Offset Range: -16 h to +47.75 h. */
	uint8_t tai_of_zone_change[5];    /*!< TAI of Zone Change state requested by the client.
                                       *   Little endian, 40 bits.
                                       *   Range: 0x0000000000 to 0xFFFFFFFFFF.
                                       *   0x0000000000: The TAI of Zone Change is unknown. */
} rtk_bt_mesh_time_server_set_zone_t;

/**
 * @struct    rtk_bt_mesh_time_server_set_tai_utc_delta_t
 * @brief     BLE MESH time set tai utc delta evt message structure.
 */
typedef struct {
	uint16_t tai_utc_delta_new: 15;    /*!< TAI-UTC Delta New state requested by the client.
                                        *   Range: 0x0000 to 0x7FFF.
                                        *   Delta = N - 255 s.
                                        *   Delta Range: -255 s to +32512 s. */
	uint16_t padding: 1;               /*!< Padding, always 0. */
	uint8_t tai_of_delta_change[5];    /*!< TAI of Delta Change state requested by the client.
                                        *   Little endian, 40 bits.
                                        *   Range: 0x0000000000 to 0xFFFFFFFFFF.
                                        *   0x0000000000: The TAI of Delta Change is unknown. */
} rtk_bt_mesh_time_server_set_tai_utc_delta_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  ble_mesh_time_model BT LE Mesh Time Model APIs
 * @brief     BT LE Mesh Time Model related function APIs
 * @ingroup   ble_mesh_api_group
 * @{
 */
/**
 * @brief     Send a Time Get message to a time server to read its Time state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_TIME_CLIENT_MODEL_STATUS.
 * @param[in] time_client_get: Time Get message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_time_get(rtk_bt_mesh_time_get_t *time_client_get);

/**
 * @brief     Send a Time Zone Get message to a time server to read its Time Zone Offset Current, Time Zone Offset New and TAI of Zone Change states.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_TIME_CLIENT_MODEL_ZONE_STATUS.
 * @param[in] time_client_zone_get: Time Zone Get message parameters.
 *                                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_time_zone_get(rtk_bt_mesh_time_zone_get_t *time_client_zone_get);

/**
 * @brief     Send a TAI-UTC Delta Get message to a time server to read its TAI-UTC Delta Current, TAI-UTC Delta New and TAI of Delta Change states.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_TIME_CLIENT_MODEL_TAI_UTC_DELTA_STATUS.
 * @param[in] time_client_delta_get: TAI-UTC Delta Get message parameters.
 *                                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_time_tai_utc_delta_get(rtk_bt_mesh_time_tai_utc_delta_get_t *time_client_delta_get);

/**
 * @brief     Send a Time Role Get message to a time setup server to read its Time Role state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_TIME_CLIENT_MODEL_ROLE_STATUS.
 * @param[in] time_client_role_get: Time Role Get message parameters.
 *                                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_time_role_get(rtk_bt_mesh_time_role_get_t *time_client_role_get);

/**
 * @brief     Send a Time Set message to a time setup server to set its Time state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_TIME_CLIENT_MODEL_STATUS.
 * @param[in] time_client_set: Time Set message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_time_set(rtk_bt_mesh_time_set_t *time_client_set);

/**
 * @brief     Send a Time Zone Set message to a time setup server to set its Time Zone Offset New and TAI of Zone Change states.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_TIME_CLIENT_MODEL_ZONE_STATUS.
 * @param[in] time_client_zone_set: Time Zone Set message parameters.
 *                                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_time_zone_set(rtk_bt_mesh_time_zone_set_t *time_client_zone_set);

/**
 * @brief     Send a TAI-UTC Delta Set message to a time setup server to set its TAI-UTC Delta New and TAI of Delta Change states.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_TIME_CLIENT_MODEL_TAI_UTC_DELTA_STATUS.
 * @param[in] time_client_delta_set: TAI-UTC Delta Set message parameters.
 *                                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_time_tai_utc_delta_set(rtk_bt_mesh_time_tai_utc_delta_set_t *time_client_delta_set);

/**
 * @brief     Send a Time Role Set message to a time setup server to set its Time Role state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_TIME_CLIENT_MODEL_ROLE_STATUS.
 * @param[in] time_client_role_set: Time Role Set message parameters.
 *                                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_time_role_set(rtk_bt_mesh_time_role_set_t *time_client_role_set);

/**
 * @}
 */

#endif /* __RTK_BT_MESH_TIME_MODEL_H__ */
