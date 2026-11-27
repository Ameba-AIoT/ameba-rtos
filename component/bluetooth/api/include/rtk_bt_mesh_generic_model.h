/**
 * @file      rtk_bt_mesh_generic_model.h
 * @author    quinn_yang@realsil.com.cn
 * @brief     Bluetooth LE MESH generic client and server model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_GENERIC_MODEL_H__
#define __RTK_BT_MESH_GENERIC_MODEL_H__

#include <stdint.h>
#include <rtk_bt_mesh_def.h>

#include <rtk_bt_mesh_generic_onoff_model.h>

/** @addtogroup BT_Constants BT Constants
 * @{
 */
/** @addtogroup BT_Mesh_Constants BT Mesh Constants
 * @{
 */
/** @addtogroup BT_Mesh_Generic_Model_Constants BT Mesh Generic Model Constants
 * @{
 */
/**
 * @def       GENERIC_PROPERTY_DATA_MAX_LEN
 * @brief     Maximum length in bytes of the raw value of a Generic property, and maximum number of Generic property IDs that can be reported.
 */
#define GENERIC_PROPERTY_DATA_MAX_LEN    0x10

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

/** @addtogroup BT_Types BT Types
 * @{
 */
/** @addtogroup BT_Mesh_Types BT Mesh Types
 * @{
 */
/** @addtogroup BT_Mesh_Generic_Model_Types BT Mesh Generic Model Types
 * @{
 */
/************************************ Generic Level ************************************/
/**************************************** Client ***************************************/
/**
 * @enum      rtk_bt_mesh_generic_level_client_model_act_t
 * @brief     BLE MESH generic level client model act definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_GET = 1,      /*!< Send a Generic Level Get message to the server. */
	RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_SET,          /*!< Send a Generic Level Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_DELTA_SET,    /*!< Send a Generic Delta Set or Delta Set Unacknowledged message to the server. */
	RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_MOVE_SET,     /*!< Send a Generic Move Set or Move Set Unacknowledged message to the server. */
	RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_ACT_MAX,      /*!< Reserved. */
} rtk_bt_mesh_generic_level_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_generic_level_client_model_evt_t
 * @brief     BLE MESH generic level client model event definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_STATUS = 1,    /*!< Indicate Generic Level Status message received from the server, with msg
                                                           *   @ref rtk_bt_mesh_generic_level_client_status_t. */
	RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_EVT_MAX,       /*!< Reserved. */
} rtk_bt_mesh_generic_level_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_generic_level_get_t
 * @brief     BLE MESH generic level get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_generic_level_get_t;

/**
 * @struct    rtk_bt_mesh_generic_level_set_t
 * @brief     BLE MESH generic level set act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	int16_t level;                                       /*!< Target Generic Level state.
                                                          *   Range: -32768 to 32767. */
	uint8_t tid;                                         /*!< Transaction identifier of the message.
                                                          *   Assigned automatically by the API, the application does not need to set it. */
	bool optional;                                       /*!< Whether the Transition Time and Delay fields are carried in the message.
                                                          *   - true: Transition Time and Delay are carried.
                                                          *   - false: Transition Time and Delay are not carried. */
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Time the server takes to transition to the target state.
                                                          *   Only used when optional is true. */
	uint8_t delay;                                       /*!< Delay before the server starts the transition.
                                                          *   Time = N * 5 ms.
                                                          *   Time Range: 0 ms to 1275 ms.
                                                          *   Only used when optional is true. */
	bool ack;                                            /*!< Whether the server shall acknowledge the message.
                                                          *   - true: Send Generic Level Set and the server replies with a Generic Level Status message.
                                                          *   - false: Send Generic Level Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_generic_level_set_t;

/**
 * @struct    rtk_bt_mesh_generic_level_delta_set_t
 * @brief     BLE MESH generic level delta set act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	int32_t delta_level;                                 /*!< Delta change of the Generic Level state, increases the level when positive and decreases it when negative.
                                                          *   Range: -2147483648 to 2147483647. */
	uint8_t tid;                                         /*!< Transaction identifier of the message.
                                                          *   Assigned automatically by the API, the application does not need to set it. */
	bool optional;                                       /*!< Whether the Transition Time and Delay fields are carried in the message.
                                                          *   - true: Transition Time and Delay are carried.
                                                          *   - false: Transition Time and Delay are not carried. */
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Time the server takes to transition to the target state.
                                                          *   Only used when optional is true. */
	uint8_t delay;                                       /*!< Delay before the server starts the transition.
                                                          *   Time = N * 5 ms.
                                                          *   Time Range: 0 ms to 1275 ms.
                                                          *   Only used when optional is true. */
	bool ack;                                            /*!< Whether the server shall acknowledge the message.
                                                          *   - true: Send Generic Delta Set and the server replies with a Generic Level Status message.
                                                          *   - false: Send Generic Delta Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_generic_level_delta_set_t;

/**
 * @struct    rtk_bt_mesh_generic_level_move_set_t
 * @brief     BLE MESH generic level move set act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	int16_t delta_level;                                 /*!< Delta Level step used together with Transition Time to calculate the speed of the Generic Level change.
                                                          *   Range: -32768 to 32767. */
	uint8_t tid;                                         /*!< Transaction identifier of the message.
                                                          *   Assigned automatically by the API, the application does not need to set it. */
	bool optional;                                       /*!< Whether the Transition Time and Delay fields are carried in the message.
                                                          *   - true: Transition Time and Delay are carried.
                                                          *   - false: Transition Time and Delay are not carried. */
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Time used to calculate the speed of the Generic Level change.
                                                          *   If the resulting time is 0, no Generic Level change is initiated.
                                                          *   Only used when optional is true. */
	uint8_t delay;                                       /*!< Delay before the server starts the transition.
                                                          *   Time = N * 5 ms.
                                                          *   Time Range: 0 ms to 1275 ms.
                                                          *   Only used when optional is true. */
	bool ack;                                            /*!< Whether the server shall acknowledge the message.
                                                          *   - true: Send Generic Move Set and the server replies with a Generic Level Status message.
                                                          *   - false: Send Generic Move Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_generic_level_move_set_t;

/**
 * @struct    rtk_bt_mesh_generic_level_client_status_t
 * @brief     BLE MESH generic level client model status evt message structure.
 */
typedef struct {
	uint16_t src;                                            /*!< Unicast address of the server that sent the Status message. */
	int16_t present_level;                                   /*!< Present Generic Level state of the server.
                                                              *   Range: -32768 to 32767. */
	bool optional;                                           /*!< Whether the Target Level and Remaining Time fields are carried in the message.
                                                              *   - true: target_level and remaining_time are valid.
                                                              *     The server is changing the state or waiting for the delay to complete.
                                                              *   - false: target_level and remaining_time are not valid. */
	int16_t target_level;                                    /*!< Target Generic Level state the server is changing to.
                                                              *   Range: -32768 to 32767.
                                                              *   Only valid when optional is true. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   Only valid when optional is true. */
} rtk_bt_mesh_generic_level_client_status_t;


/**************************************** Server ***************************************/
/**
 * @enum      rtk_bt_mesh_generic_level_server_model_evt_t
 * @brief     BLE MESH generic level server model event definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_LEVEL_SERVER_MODEL_GET = 1,     /*!< Indicate a Generic Level Get message is received or the server needs the current Generic Level state, with
                                                         *   msg @ref rtk_bt_mesh_generic_level_server_get_t.
                                                         *   The application shall write the current Generic Level into the msg.
                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_LEVEL_SERVER_MODEL_SET,         /*!< Indicate the Generic Level state is to change, with msg @ref rtk_bt_mesh_generic_level_server_set_t.
                                                         *   Raised when a Set or Delta Set message is received and again when the transition is updated or stopped.
                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_LEVEL_SERVER_MODEL_SET_MOVE,    /*!< Indicate the Generic Level state is to move, with msg @ref rtk_bt_mesh_generic_level_server_set_move_t.
                                                         *   Raised when a Move Set message is received and again when the move is updated or stopped.
                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_LEVEL_SERVER_MODEL_EVT_MAX,     /*!< Reserved. */
} rtk_bt_mesh_generic_level_server_model_evt_t;

/**
 * @struct    rtk_bt_mesh_generic_level_server_get_t
 * @brief     BLE MESH generic level get evt message structure.
 */
typedef struct {
	int16_t *level;    /*!< Output: the application writes the current Generic Level state here.
                        *   Must not be NULL. */
} rtk_bt_mesh_generic_level_server_get_t;

/**
 * @struct    rtk_bt_mesh_generic_level_server_get_default_transition_time_t
 * @brief     BLE MESH generic level get default transition time evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Generic Default Transition Time of the server. */
} rtk_bt_mesh_generic_level_server_get_default_transition_time_t;

/**
 * @struct    rtk_bt_mesh_generic_level_server_set_t
 * @brief     BLE MESH generic level set evt message structure.
 */
typedef struct {
	int16_t level;                                           /*!< Target Generic Level state requested by the client.
                                                              *   Range: -32768 to 32767. */
	rtk_bt_mesh_generic_transition_time_t total_time;        /*!< Total time of the transition to the target state.
                                                              *   A zero value means the state changes immediately. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   A zero value means the state has reached the target. */
} rtk_bt_mesh_generic_level_server_set_t;

/**
 * @struct    rtk_bt_mesh_generic_level_server_set_move_t
 * @brief     BLE MESH generic level set move evt message structure.
 */
typedef struct {
	int16_t move_delta;                                      /*!< Delta Level step used together with the transition time to calculate the speed of the Generic Level
                                                              *   change.
                                                              *   Range: -32768 to 32767.
                                                              *   0: The Generic Level state shall stop changing.
                                                              *     The stack only reports this value, handle it in the application if needed. */
	int16_t target_level;                                    /*!< Target Generic Level state the move runs towards.
                                                              *   Range: -32768 to 32767. */
	rtk_bt_mesh_generic_transition_time_t total_time;        /*!< Total time of the transition to the target state.
                                                              *   A zero value means the state changes immediately. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   A zero value means the state has reached the target. */
} rtk_bt_mesh_generic_level_server_set_move_t;

/********************************* Generic Power On Off ********************************/
/**************************************** Client ***************************************/
/**
 * @enum      rtk_bt_mesh_generic_on_power_up_client_model_act_t
 * @brief     BLE MESH generic on power up client model act definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_ON_POWER_UP_CLIENT_MODEL_GET = 1,    /*!< Send a Generic OnPowerUp Get message to the server. */
	RTK_BT_MESH_GENERIC_ON_POWER_UP_CLIENT_MODEL_SET,        /*!< Send a Generic OnPowerUp Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_GENERIC_ON_POWER_UP_CLIENT_MODEL_ACT_MAX,    /*!< Reserved. */
} rtk_bt_mesh_generic_on_power_up_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_generic_on_power_up_client_model_evt_t
 * @brief     BLE MESH generic on power up client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_ON_POWER_UP_CLIENT_MODEL_STATUS = 1,    /*!< Indicate Generic OnPowerUp Status message received from the server, with msg
                                                                 *   @ref rtk_bt_mesh_generic_power_on_off_client_status_t. */
	RTK_BT_MESH_GENERIC_ON_POWER_UP_CLIENT_MODEL_EVT_MAX,       /*!< Reserved. */
} rtk_bt_mesh_generic_on_power_up_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_generic_on_power_up_get_t
 * @brief     BLE MESH generic on power up get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_generic_on_power_up_get_t;

/**
 * @enum      rtk_bt_mesh_generic_on_power_up_t
 * @brief     BLE MESH generic on power up set data structure
 */
typedef enum {
	RTK_BT_MESH_GENERIC_ON_POWER_UP_OFF,        /*!< After being powered up, the element is in an off state. */
	RTK_BT_MESH_GENERIC_ON_POWER_UP_DEFAULT,    /*!< After being powered up, the element is in an On state and uses default state values. */
	RTK_BT_MESH_GENERIC_ON_POWER_UP_RESTORE     /*!< If a transition was in progress when powered down, the element restores the target state when powered up.
                                                 *   Otherwise the element restores the state it was in when powered down. */
} rtk_bt_mesh_generic_on_power_up_t;

/**
 * @struct    rtk_bt_mesh_generic_on_power_up_set_t
 * @brief     BLE MESH generic on power up set act message structure.
 */
typedef struct {
	uint16_t dst;                                     /*!< Destination address of the message. */
	uint16_t app_key_index;                           /*!< Index of the application key used to encrypt the message.
                                                       *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_generic_on_power_up_t on_power_up;    /*!< Generic OnPowerUp state to set. */
	bool ack;                                         /*!< Whether the server shall acknowledge the message.
                                                       *   - true: Send Generic OnPowerUp Set and the server replies with a Generic OnPowerUp Status message.
                                                       *   - false: Send Generic OnPowerUp Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_generic_on_power_up_set_t;

/**
 * @struct    rtk_bt_mesh_generic_power_on_off_client_status_t
 * @brief     BLE MESH generic power on off client model status evt message structure.
 */
typedef struct {
	uint16_t src;                                     /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_generic_on_power_up_t on_power_up;    /*!< Generic OnPowerUp state reported by the server. */
} rtk_bt_mesh_generic_power_on_off_client_status_t;

/**************************************** Server ***************************************/

/**
 * @enum      rtk_bt_mesh_generic_power_on_off_server_model_evt_t
 * @brief     BLE MESH generic power on off server model event definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_POWER_ON_OFF_SERVER_MODEL_GET = 1,    /*!< Indicate a Generic OnPowerUp Get message is received or the server needs the current Generic OnPowerUp
                                                               *   state, with msg @ref rtk_bt_mesh_generic_power_on_off_server_get_t.
                                                               *   The application shall write the current Generic OnPowerUp state into the msg.
                                                               *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_POWER_ON_OFF_SERVER_MODEL_EVT_MAX,    /*!< Reserved. */
} rtk_bt_mesh_generic_power_on_off_server_model_evt_t;

/**
 * @enum      rtk_bt_mesh_generic_power_on_off_setup_server_model_evt_t
 * @brief     BLE MESH generic power on off setup server model event definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL_SET = 1,    /*!< Indicate Generic OnPowerUp Set or Set Unacknowledged message received from a client, with msg
                                                                     *   @ref rtk_bt_mesh_generic_power_on_off_server_set_t. */
	RTK_BT_MESH_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL_EVT_MAX,    /*!< Reserved. */
} rtk_bt_mesh_generic_power_on_off_setup_server_model_evt_t;

/**
 * @struct    rtk_bt_mesh_generic_power_on_off_server_get_t
 * @brief     BLE MESH generic power on off get evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_on_power_up_t *on_power_up;    /*!< Output: the application writes the current Generic OnPowerUp state here.
                                                        *   Must not be NULL. */
} rtk_bt_mesh_generic_power_on_off_server_get_t;

/**
 * @struct    rtk_bt_mesh_generic_power_on_off_server_set_t
 * @brief     BLE MESH generic power on off set evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_on_power_up_t on_power_up;    /*!< Generic OnPowerUp state requested by the client. */
} rtk_bt_mesh_generic_power_on_off_server_set_t;

/********************************* Generic Power Level *********************************/
/**************************************** Client ***************************************/

/**
 * @enum      rtk_bt_mesh_generic_power_level_client_model_act_t
 * @brief     BLE MESH generic power level client model act definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_GET = 1,        /*!< Send a Generic Power Level Get message to the server. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_SET,            /*!< Send a Generic Power Level Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_LAST_GET,       /*!< Send a Generic Power Last Get message to the server. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_DEFAULT_GET,    /*!< Send a Generic Power Default Get message to the server. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_DEFAULT_SET,    /*!< Send a Generic Power Default Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_RANGE_GET,      /*!< Send a Generic Power Range Get message to the server. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_RANGE_SET,      /*!< Send a Generic Power Range Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_ACT_MAX,        /*!< Reserved. */
} rtk_bt_mesh_generic_power_level_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_generic_power_level_client_model_evt_t
 * @brief     BLE MESH generic power level client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_STATUS = 1,        /*!< Indicate Generic Power Level Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_generic_power_level_client_status_t. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_LAST_STATUS,       /*!< Indicate Generic Power Last Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_generic_power_level_client_status_simple_t. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_DEFAULT_STATUS,    /*!< Indicate Generic Power Default Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_generic_power_level_client_status_simple_t. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_RANGE_STATUS,      /*!< Indicate Generic Power Range Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_generic_power_level_client_status_range_t. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_EVT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_generic_power_level_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_get_t
 * @brief     BLE MESH generic power level get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_generic_power_level_get_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_set_t
 * @brief     BLE MESH generic power level set act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	uint16_t power;                                      /*!< Target Generic Power Actual state.
                                                          *   Range: 0x0000 to 0xFFFF.
                                                          *   The value is the power level relative to the maximum power level. */
	uint8_t tid;                                         /*!< Transaction identifier of the message.
                                                          *   Assigned automatically by the API, the application does not need to set it. */
	bool optional;                                       /*!< Whether the Transition Time and Delay fields are carried in the message.
                                                          *   - true: Transition Time and Delay are carried.
                                                          *   - false: Transition Time and Delay are not carried. */
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Time the server takes to transition to the target state.
                                                          *   Only used when optional is true. */
	uint8_t delay;                                       /*!< Delay before the server starts the transition.
                                                          *   Time = N * 5 ms.
                                                          *   Time Range: 0 ms to 1275 ms.
                                                          *   Only used when optional is true. */
	bool ack;                                            /*!< Whether the server shall acknowledge the message.
                                                          *   - true: Send Generic Power Level Set and the server replies with a Generic Power Level Status message.
                                                          *   - false: Send Generic Power Level Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_generic_power_level_set_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_last_get_t
 * @brief     BLE MESH generic power level last get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_generic_power_level_last_get_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_default_get_t
 * @brief     BLE MESH generic power level default get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_generic_power_level_default_get_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_default_set_t
 * @brief     BLE MESH generic power level default set act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t power;            /*!< Generic Power Default state to set.
                                *   Range: 0x0000 to 0xFFFF.
                                *   0x0000: Use the Generic Power Last state as the default value.
                                *   0x0001 to 0xFFFF: Power level relative to the maximum power level. */
	bool ack;                  /*!< Whether the server shall acknowledge the message.
                                *   - true: Send Generic Power Default Set and the server replies with a Generic Power Default Status message.
                                *   - false: Send Generic Power Default Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_generic_power_level_default_set_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_range_get_t
 * @brief     BLE MESH generic power level range get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_generic_power_level_range_get_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_range_set_t
 * @brief     BLE MESH generic power level range set act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t range_min;        /*!< Generic Power Range Min to set.
                                *   Range: 0x0001 to 0xFFFF.
                                *   0x0000 is prohibited. */
	uint16_t range_max;        /*!< Generic Power Range Max to set.
                                *   Range: 0x0001 to 0xFFFF.
                                *   0x0000 is prohibited.
                                *   Must be greater than or equal to range_min. */
	bool ack;                  /*!< Whether the server shall acknowledge the message.
                                *   - true: Send Generic Power Range Set and the server replies with a Generic Power Range Status message.
                                *   - false: Send Generic Power Range Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_generic_power_level_range_set_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_client_status_t
 * @brief     BLE MESH generic power level client model status evt message structure.
 */
typedef struct {
	uint16_t src;                                            /*!< Unicast address of the server that sent the Status message. */
	uint16_t present_power;                                  /*!< Present Generic Power Actual state of the server. */
	bool optional;                                           /*!< Whether the Target Power and Remaining Time fields are carried in the message.
                                                              *   - true: target_power and remaining_time are valid.
                                                              *     The server is changing the state or waiting for the delay to complete.
                                                              *   - false: target_power and remaining_time are not valid. */
	uint16_t target_power;                                   /*!< Target Generic Power Actual state the server is changing to.
                                                              *   Only valid when optional is true. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   Only valid when optional is true. */
} rtk_bt_mesh_generic_power_level_client_status_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_client_status_simple_t
 * @brief     BLE MESH generic power level client model simple status evt message structure.
 */
typedef struct {
	uint16_t src;      /*!< Unicast address of the server that sent the Status message. */
	uint16_t power;    /*!< Generic Power Last or Generic Power Default state reported by the server.
                        *   Generic Power Last: 0x0000 is Prohibited, 0x0001 to 0xFFFF is the power level relative to the maximum power level.
                        *   Generic Power Default: 0x0000 means use the Generic Power Last value,
                        *   0x0001 to 0xFFFF is the power level relative to the maximum power level. */
} rtk_bt_mesh_generic_power_level_client_status_simple_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_client_status_range_t
 * @brief     BLE MESH generic power level client model range status evt message structure.
 */
typedef struct {
	uint16_t src;                       /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_generic_stat_t stat;    /*!< Status code of the last operation on the Generic Power Range state. */
	uint16_t range_min;                 /*!< Generic Power Range Min reported by the server. */
	uint16_t range_max;                 /*!< Generic Power Range Max reported by the server. */
} rtk_bt_mesh_generic_power_level_client_status_range_t;

/**************************************** Server ***************************************/

/**
 * @enum      rtk_bt_mesh_generic_power_level_server_model_evt_t
 * @brief     BLE MESH generic power level server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_GET = 1,        /*!< Indicate a Generic Power Level Get message is received or the server needs the current Generic Power
                                                                  *   Actual state, with msg @ref rtk_bt_mesh_generic_power_level_server_get_t.
                                                                  *   The application shall write the current Generic Power Actual state into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_SET,            /*!< Indicate the Generic Power Actual state is to change, with msg
                                                                  *   @ref rtk_bt_mesh_generic_power_level_server_set_t.
                                                                  *   Raised when a Set message is received and again when the transition is updated or stopped.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_LAST_GET,       /*!< Indicate Generic Power Last Get message received from a client, with msg
                                                                  *   @ref rtk_bt_mesh_generic_power_level_server_get_t.
                                                                  *   The application shall write the current Generic Power Last state into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_LAST_SET,       /*!< Indicate the Generic Power Last state needs to be updated, with msg
                                                                  *   @ref rtk_bt_mesh_generic_power_level_server_set_last_t.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_DEFAULT_GET,    /*!< Indicate Generic Power Default Get message received from a client, with msg
                                                                  *   @ref rtk_bt_mesh_generic_power_level_server_get_t.
                                                                  *   The application shall write the current Generic Power Default state into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_RANGE_GET,      /*!< Indicate a Generic Power Range Get message is received or the server needs the current Generic Power
                                                                  *   Range state, with msg @ref rtk_bt_mesh_generic_power_level_server_direct_get_range_t.
                                                                  *   The application shall write the current Generic Power Range state into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_EVT_MAX,        /*!< Reserved. */
} rtk_bt_mesh_generic_power_level_server_model_evt_t;

/**
 * @enum      rtk_bt_mesh_generic_power_level_setup_server_model_evt_t
 * @brief     BLE MESH generic power level setup server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL_DEFAULT_SET = 1,    /*!< Indicate Generic Power Default Set or Set Unacknowledged message received from a client,
                                                                            *   with msg @ref rtk_bt_mesh_generic_power_level_server_set_default_t. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL_RANGE_SET,          /*!< Indicate Generic Power Range Set or Set Unacknowledged message received from a client,
                                                                            *   with msg @ref rtk_bt_mesh_generic_power_level_server_set_range_t. */
	RTK_BT_MESH_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL_EVT_MAX,            /*!< Reserved. */
} rtk_bt_mesh_generic_power_level_setup_server_model_evt_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_server_get_t
 * @brief     BLE MESH generic power level server model get evt message structure.
 */
typedef struct {
	uint16_t *power;    /*!< Output: the application writes the current power value here.
                         *   Must not be NULL. */
} rtk_bt_mesh_generic_power_level_server_get_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_server_get_range_t
 * @brief     BLE MESH generic power level server model get range evt message structure.
 */
typedef struct {
	uint16_t range_min;    /*!< Generic Power Range Min. */
	uint16_t range_max;    /*!< Generic Power Range Max. */
} rtk_bt_mesh_generic_power_level_server_get_range_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_server_direct_get_range_t
 * @brief     BLE MESH generic power level server model get range for direct calling evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_power_level_server_get_range_t *value;    /*!< Output: the application writes the current Generic Power Range here.
                                                                   *   Must not be NULL. */
} rtk_bt_mesh_generic_power_level_server_direct_get_range_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_server_get_default_transition_time_t
 * @brief     BLE MESH generic power level server model get default transition time evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Generic Default Transition Time of the server. */
} rtk_bt_mesh_generic_power_level_server_get_default_transition_time_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_server_set_t
 * @brief     BLE MESH generic power level server model set evt message structure.
 */
typedef struct {
	uint16_t power;                                          /*!< Target Generic Power Actual state requested by the client.
                                                              *   Range: 0x0000 to 0xFFFF. */
	rtk_bt_mesh_generic_transition_time_t total_time;        /*!< Total time of the transition to the target state.
                                                              *   A zero value means the state changes immediately. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   A zero value means the state has reached the target. */
} rtk_bt_mesh_generic_power_level_server_set_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_server_set_last_t
 * @brief     BLE MESH generic power level server model set last evt message structure.
 */
typedef struct {
	uint16_t power;    /*!< Generic Power Last state to store.
                        *   Range: 0x0001 to 0xFFFF. */
} rtk_bt_mesh_generic_power_level_server_set_last_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_server_set_default_t
 * @brief     BLE MESH generic power level server model set default evt message structure.
 */
typedef struct {
	uint16_t power;    /*!< Generic Power Default state requested by the client.
                        *   0x0000: Use the Generic Power Last state as the default value.
                        *   The stack only reports this value, handle it in the application if needed. */
} rtk_bt_mesh_generic_power_level_server_set_default_t;

/**
 * @struct    rtk_bt_mesh_generic_power_level_server_set_range_t
 * @brief     BLE MESH generic power level server model set range evt message structure.
 */
typedef struct {
	uint16_t range_min;    /*!< Generic Power Range Min requested by the client.
                            *   Range: 0x0001 to 0xFFFF. */
	uint16_t range_max;    /*!< Generic Power Range Max requested by the client.
                            *   Range: 0x0001 to 0xFFFF. */
} rtk_bt_mesh_generic_power_level_server_set_range_t;

/*********************************** Generic Battery ***********************************/
/**
 * @enum      rtk_bt_mesh_generic_battery_client_model_act_t
 * @brief     BLE MESH generic battery client model act definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_BATTERY_CLIENT_MODEL_GET = 1,    /*!< Send a Generic Battery Get message to the server. */
	RTK_BT_MESH_GENERIC_BATTERY_CLIENT_MODEL_ACT_MAX,    /*!< Reserved. */
} rtk_bt_mesh_generic_battery_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_generic_battery_client_model_evt_t
 * @brief     BLE MESH generic battery client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_BATTERY_CLIENT_MODEL_STATUS = 1,    /*!< Indicate Generic Battery Status message received from the server, with msg
                                                             *   @ref rtk_bt_mesh_generic_battery_client_status_t. */
	RTK_BT_MESH_GENERIC_BATTERY_CLIENT_MODEL_EVT_MAX,       /*!< Reserved. */
} rtk_bt_mesh_generic_battery_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_generic_battery_get_t
 * @brief     BLE MESH generic battery get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_generic_battery_get_t;

/**
 * @struct    rtk_bt_mesh_generic_battery_flags_t
 * @brief     BLE MESH generic battery flags structure.
 */
typedef struct {
	uint8_t presence: 2;          /*!< Presence of the battery.
                                   *   0x00: The battery is not present.
                                   *   0x01: The battery is present and is removable.
                                   *   0x02: The battery is present and is non-removable.
                                   *   0x03: The battery presence is unknown. */
	uint8_t indicator: 2;         /*!< Charge level indicator of the battery.
                                   *   0x00: The battery charge is critically low level.
                                   *   0x01: The battery charge is low level.
                                   *   0x02: The battery charge is good level.
                                   *   0x03: The battery charge is unknown. */
	uint8_t charging: 2;          /*!< Charging state of the battery.
                                   *   0x00: The battery is not chargeable.
                                   *   0x01: The battery is chargeable and is not charging.
                                   *   0x02: The battery is chargeable and is charging.
                                   *   0x03: The battery charging state is unknown. */
	uint8_t serviceability: 2;    /*!< Serviceability of the battery.
                                   *   0x00: Reserved for future use.
                                   *   0x01: The battery does not require service.
                                   *   0x02: The battery requires service.
                                   *   0x03: The battery serviceability is unknown. */
} rtk_bt_mesh_generic_battery_flags_t;

/**
 * @struct    rtk_bt_mesh_generic_battery_client_status_t
 * @brief     BLE MESH generic battery client status act message structure.
 */
typedef struct {
	uint16_t src;                                 /*!< Unicast address of the server that sent the Status message. */
	uint8_t battery_level;                        /*!< Battery charge level in percent.
                                                   *   Range: 0x00 to 0x64.
                                                   *   0x65 to 0xFE: Prohibited.
                                                   *   0xFF: The percentage of the charge level is unknown. */
	uint32_t time_to_discharge;                   /*!< Remaining time of the discharging process.
                                                   *   Unit: minute.
                                                   *   Range: 0x000000 to 0xFFFFFE.
                                                   *   0xFFFFFF: The remaining time of the discharging process is unknown. */
	uint32_t time_to_charge;                      /*!< Remaining time of the charging process.
                                                   *   Unit: minute.
                                                   *   Range: 0x000000 to 0xFFFFFE.
                                                   *   0xFFFFFF: The remaining time of the charging process is unknown. */
	rtk_bt_mesh_generic_battery_flags_t flags;    /*!< Flags of the battery. */
} rtk_bt_mesh_generic_battery_client_status_t;

/**
 * @enum      rtk_bt_mesh_generic_battery_server_model_evt_t
 * @brief     BLE MESH generic battery server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_BATTERY_SERVER_MODEL_GET = 1,    /*!< Indicate a Generic Battery Get message is received or the server needs the current Generic Battery state,
                                                          *   with msg @ref rtk_bt_mesh_generic_battery_server_direct_get_t. The application shall write the current
                                                          *   Generic Battery state into the msg. Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_BATTERY_SERVER_MODEL_EVT_MAX,    /*!< Reserved. */
} rtk_bt_mesh_generic_battery_server_model_evt_t;

/**
 * @struct    rtk_bt_mesh_generic_battery_server_get_t
 * @brief     BLE MESH generic battery server get status message structure.
 */
typedef struct {
	uint8_t battery_level;                        /*!< Battery charge level in percent.
                                                   *   Range: 0x00 to 0x64.
                                                   *   0x65 to 0xFE: Prohibited.
                                                   *   0xFF: The percentage of the charge level is unknown. */
	uint32_t time_to_discharge;                   /*!< Remaining time of the discharging process.
                                                   *   Unit: minute.
                                                   *   Range: 0x000000 to 0xFFFFFE.
                                                   *   0xFFFFFF: The remaining time of the discharging process is unknown. */
	uint32_t time_to_charge;                      /*!< Remaining time of the charging process.
                                                   *   Unit: minute.
                                                   *   Range: 0x000000 to 0xFFFFFE.
                                                   *   0xFFFFFF: The remaining time of the charging process is unknown. */
	rtk_bt_mesh_generic_battery_flags_t flags;    /*!< Flags of the battery. */
} rtk_bt_mesh_generic_battery_server_get_t;

/**
 * @struct    rtk_bt_mesh_generic_battery_server_direct_get_t
 * @brief     BLE MESH generic battery server get status for direct calling message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_battery_server_get_t *value;    /*!< Output: the application writes the current Generic Battery state here.
                                                         *   Must not be NULL. */
} rtk_bt_mesh_generic_battery_server_direct_get_t;

/*********************************** Generic Location **********************************/
/**************************************** Client ***************************************/
/**
 * @enum      rtk_bt_mesh_generic_location_client_model_act_t
 * @brief     BLE MESH generic location client model act definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_GLOBAL_GET = 1,    /*!< Send a Generic Location Global Get message to the server. */
	RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_GLOBAL_SET,        /*!< Send a Generic Location Global Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_LOCAL_GET,         /*!< Send a Generic Location Local Get message to the server. */
	RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_LOCAL_SET,         /*!< Send a Generic Location Local Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_ACT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_generic_location_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_generic_location_client_model_evt_t
 * @brief     BLE MESH generic location client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_GLOBAL_STATUS = 1,    /*!< Indicate Generic Location Global Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_generic_location_client_status_global_t. */
	RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_LOCAL_STATUS,         /*!< Indicate Generic Location Local Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_generic_location_client_status_local_t. */
	RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_EVT_MAX,              /*!< Reserved. */
} rtk_bt_mesh_generic_location_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_generic_location_global_get_t
 * @brief     BLE MESH generic location global get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_generic_location_global_get_t;

/**
 * @struct    rtk_bt_mesh_generic_location_global_t
 * @brief     BLE MESH generic location global parameter structure.
 */
typedef struct {
	int32_t global_latitude;     /*!< Global WGS84 North coordinate of the element.
                                  *   The value X in the range [-90, 90] degrees is encoded as N = floor(X / 90 * (2^31 - 1)).
                                  *   Range: -2147483647 to 2147483647.
                                  *   0x80000000: Global Latitude is not configured. */
	int32_t global_longitude;    /*!< Global WGS84 East coordinate of the element.
                                  *   The value X in the range [-180, 180] degrees is encoded as N = floor(X / 180 * (2^31 - 1)).
                                  *   Range: -2147483647 to 2147483647.
                                  *   0x80000000: Global Longitude is not configured. */
	int16_t global_altitude;     /*!< Altitude of the element above the WGS84 datum.
                                  *   Unit: meter.
                                  *   0x7FFF: Global Altitude is not configured.
                                  *   0x7FFE: Global Altitude is greater than or equal to 32766 meters.
                                  *   0x8000 to 0x7FFD: Global Altitude is the field value, from -32768 meters to 32765 meters. */
} _PACKED4_ rtk_bt_mesh_generic_location_global_t;

/**
 * @struct    rtk_bt_mesh_generic_location_global_set_t
 * @brief     BLE MESH generic location global set act message structure.
 */
typedef struct {
	uint16_t dst;                                    /*!< Destination address of the message. */
	uint16_t app_key_index;                          /*!< Index of the application key used to encrypt the message.
                                                      *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_generic_location_global_t global;    /*!< Global location to set. */
	bool ack;                                        /*!< Whether the server shall acknowledge the message.
                                                      *   - true: Send Generic Location Global Set and the server replies with a Generic Location Global
                                                      *     Status message.
                                                      *   - false: Send Generic Location Global Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_generic_location_global_set_t;

/**
 * @struct    rtk_bt_mesh_generic_location_local_get_t
 * @brief     BLE MESH generic location local get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_generic_location_local_get_t;

/**
 * @union     rtk_bt_mesh_generic_location_uncertainty_t
 * @brief     BLE MESH generic location uncertainty parameter structure.
 */
typedef union {
	struct {
		uint16_t stationary: 1;     /*!< Whether the device is stationary or mobile.
                                     *   0x00: Stationary.
                                     *   0x01: Mobile. */
		uint16_t rfu: 7;            /*!< Reserved for future use. */
		uint16_t update_time: 4;    /*!< Time elapsed since the last update of the position of the device.
                                     *   Range: 0x0 to 0xF.
                                     *   Time = 2^(N-3) s.
                                     *   Time Range: 0.125 s to 4096 s.
                                     *   Can be ignored when stationary is 0x00. */
		uint16_t precision: 4;      /*!< Location precision.
                                     *   Range: 0x0 to 0xF.
                                     *   Precision = 2^(N-3) m.
                                     *   Precision Range: 0.125 m to 4096 m. */
	};
	uint16_t uncertainty;           /*!< Raw value of the Uncertainty field, shares the storage with the bit fields above. */
} _PACKED4_ rtk_bt_mesh_generic_location_uncertainty_t;

/**
 * @struct    rtk_bt_mesh_generic_location_local_t
 * @brief     BLE MESH generic location local parameter structure.
 */
typedef struct {
	int16_t local_north;                                       /*!< Local North coordinate of the device, relative to the north orientation on a predefined map.
                                                                *   Unit: decimeter.
                                                                *   Range: -32767 to 32767.
                                                                *   0x8000: Local North is not configured. */
	int16_t local_east;                                        /*!< Local East coordinate of the device, relative to the east orientation on a predefined map.
                                                                *   Unit: decimeter.
                                                                *   Range: -32767 to 32767.
                                                                *   0x8000: Local East is not configured. */
	int16_t local_altitude;                                    /*!< Altitude of the device relative to the Generic Location Global Altitude.
                                                                *   Unit: decimeter.
                                                                *   0x7FFF: Local Altitude is not configured.
                                                                *   0x7FFE: Local Altitude is greater than or equal to 32766 decimeters.
                                                                *   0x8000 to 0x7FFD: Local Altitude is the field value, from -32768 decimeters to 32765 decimeters. */
	uint8_t floor_num;                                         /*!< Floor number where the element is installed, encoded as N + 20 for floor number N.
                                                                *   0x00: Floor -20 or any floor below -20.
                                                                *   0x01 to 0xFB: Floor number N, encoded as N + 20.
                                                                *   0xFC: Floor 232 or any floor above 232.
                                                                *   0xFD: Ground floor, floor number 0 convention.
                                                                *   0xFE: Ground floor, floor number 1 convention.
                                                                *   0xFF: Floor number is not configured. */
	rtk_bt_mesh_generic_location_uncertainty_t uncertainty;    /*!< Uncertainty of the location information. */
} _PACKED4_ rtk_bt_mesh_generic_location_local_t;

/**
 * @struct    rtk_bt_mesh_generic_location_local_set_t
 * @brief     BLE MESH generic location local set act message structure.
 */
typedef struct {
	uint16_t dst;                                  /*!< Destination address of the message. */
	uint16_t app_key_index;                        /*!< Index of the application key used to encrypt the message.
                                                    *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_generic_location_local_t local;    /*!< Local location to set. */
	bool ack;                                      /*!< Whether the server shall acknowledge the message.
                                                    *   - true: Send Generic Location Local Set and the server replies with a Generic Location Local Status message.
                                                    *   - false: Send Generic Location Local Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_generic_location_local_set_t;

/**
 * @struct    rtk_bt_mesh_generic_location_client_status_global_t
 * @brief     BLE MESH generic location global status evt message structure.
 */
typedef struct {
	uint16_t src;                                    /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_generic_location_global_t global;    /*!< Global location reported by the server. */
} rtk_bt_mesh_generic_location_client_status_global_t;

/**
 * @struct    rtk_bt_mesh_generic_location_client_status_local_t
 * @brief     BLE MESH generic location local status evt message structure.
 */
typedef struct {
	uint16_t src;                                  /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_generic_location_local_t local;    /*!< Local location reported by the server. */
} rtk_bt_mesh_generic_location_client_status_local_t;

/**************************************** Server ***************************************/
/**
 * @enum      rtk_bt_mesh_generic_location_server_model_evt_t
 * @brief     BLE MESH generic location client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_LOCATION_SERVER_MODEL_GLOBAL_GET = 1,    /*!< Indicate a Generic Location Global Get message is received or the server needs the current Generic
                                                                  *   Location Global state, with msg
                                                                  *   @ref rtk_bt_mesh_generic_location_server_direct_get_global_t. The application shall write
                                                                  *   the current Generic Location global state into the msg. Direct calling event, BT APIs cannot
                                                                  *   be called in its callback. */
	RTK_BT_MESH_GENERIC_LOCATION_SERVER_MODEL_LOCAL_GET,         /*!< Indicate a Generic Location Local Get message is received or the server needs the current Generic
                                                                  *   Location Local state, with msg
                                                                  *   @ref rtk_bt_mesh_generic_location_server_direct_get_local_t. The application shall write
                                                                  *   the current Generic Location local state into the msg. Direct calling event, BT APIs cannot
                                                                  *   be called in its callback. */
	RTK_BT_MESH_GENERIC_LOCATION_SERVER_MODEL_EVT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_generic_location_server_model_evt_t;

/**
 * @enum      rtk_bt_mesh_generic_location_setup_server_model_evt_t
 * @brief     BLE MESH generic location setup server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL_GLOBAL_SET = 1,    /*!< Indicate Generic Location Global Set or Set Unacknowledged message received from a client,
                                                                        *   with msg @ref rtk_bt_mesh_generic_location_server_set_global_t. Direct calling event, BT
                                                                        *   APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL_LOCAL_SET,         /*!< Indicate Generic Location Local Set or Set Unacknowledged message received from a client,
                                                                        *   with msg @ref rtk_bt_mesh_generic_location_server_set_local_t. Direct calling event, BT
                                                                        *   APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL_GLOBAL_GET,        /*!< Indicate the setup server needs the current Generic Location Global state, e.g. before and
                                                                        *   after handling a Set message, with msg
                                                                        *   @ref rtk_bt_mesh_generic_location_server_direct_get_global_t. The application shall write
                                                                        *   the current Generic Location global state into the msg. Direct calling event, BT APIs
                                                                        *   cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL_LOCAL_GET,         /*!< Indicate the setup server needs the current Generic Location Local state, e.g. before and
                                                                        *   after handling a Set message, with msg
                                                                        *   @ref rtk_bt_mesh_generic_location_server_direct_get_local_t. The application shall write
                                                                        *   the current Generic Location local state into the msg. Direct calling event, BT APIs cannot
                                                                        *   be called in its callback. */
	RTK_BT_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL_EVT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_generic_location_setup_server_model_evt_t;

/**
 * @typedef   rtk_bt_mesh_generic_location_server_get_global_t
 * @brief     BLE MESH generic location global get evt message structure.
 */
typedef rtk_bt_mesh_generic_location_global_t rtk_bt_mesh_generic_location_server_get_global_t;

/**
 * @struct    rtk_bt_mesh_generic_location_server_direct_get_global_t
 * @brief     BLE MESH generic location global direct get evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_location_server_get_global_t *value;    /*!< Output: the application writes the current Generic Location global state here.
                                                                 *   Must not be NULL. */
} rtk_bt_mesh_generic_location_server_direct_get_global_t;

/**
 * @typedef   rtk_bt_mesh_generic_location_server_get_local_t
 * @brief     BLE MESH generic location local get evt message structure.
 */
typedef rtk_bt_mesh_generic_location_local_t rtk_bt_mesh_generic_location_server_get_local_t;

/**
 * @struct    rtk_bt_mesh_generic_location_server_direct_get_local_t
 * @brief     BLE MESH generic location local direct get evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_location_server_get_local_t *value;    /*!< Output: the application writes the current Generic Location local state here.
                                                                *   Must not be NULL. */
} rtk_bt_mesh_generic_location_server_direct_get_local_t;

/**
 * @typedef   rtk_bt_mesh_generic_location_server_set_global_t
 * @brief     BLE MESH generic location global set evt message structure.
 */
typedef rtk_bt_mesh_generic_location_global_t rtk_bt_mesh_generic_location_server_set_global_t;

/**
 * @typedef   rtk_bt_mesh_generic_location_server_set_local_t
 * @brief     BLE MESH generic location local set evt message structure.
 */
typedef rtk_bt_mesh_generic_location_local_t rtk_bt_mesh_generic_location_server_set_local_t;

/*********************************** Generic Property **********************************/
/**
 * @enum      rtk_bt_mesh_generic_property_client_model_act_t
 * @brief     BLE MESH generic property client model act definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTIES_GET = 1,    /*!< Send a Generic User Properties Get message to the server. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTY_GET,          /*!< Send a Generic User Property Get message to the server. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTY_SET,          /*!< Send a Generic User Property Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTIES_GET,       /*!< Send a Generic Admin Properties Get message to the server. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTY_GET,         /*!< Send a Generic Admin Property Get message to the server. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTY_SET,         /*!< Send a Generic Admin Property Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTIES_GET,        /*!< Send a Generic Manufacturer Properties Get message to the server. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTY_GET,          /*!< Send a Generic Manufacturer Property Get message to the server. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTY_SET,          /*!< Send a Generic Manufacturer Property Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_CLIENT_PROPERTY_GET,        /*!< Send a Generic Client Properties Get message to the server. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ACT_MAX,                    /*!< Reserved. */
} rtk_bt_mesh_generic_property_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_generic_property_client_model_evt_t
 * @brief     BLE MESH generic property client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTIES_STATUS = 1,    /*!< Indicate Generic User Properties Status message received from the server, with msg
                                                                              *   @ref rtk_bt_mesh_generic_properties_client_status_t. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTY_STATUS,          /*!< Indicate Generic User Property Status message received from the server, with msg
                                                                              *   @ref rtk_bt_mesh_generic_property_client_status_t. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTIES_STATUS,       /*!< Indicate Generic Admin Properties Status message received from the server, with msg
                                                                              *   @ref rtk_bt_mesh_generic_properties_client_status_t. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTY_STATUS,         /*!< Indicate Generic Admin Property Status message received from the server, with msg
                                                                              *   @ref rtk_bt_mesh_generic_property_client_status_t. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTIES_STATUS,        /*!< Indicate Generic Manufacturer Properties Status message received from the server, with
                                                                              *   msg @ref rtk_bt_mesh_generic_properties_client_status_t. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTY_STATUS,          /*!< Indicate Generic Manufacturer Property Status message received from the server, with msg
                                                                              *   @ref rtk_bt_mesh_generic_property_client_status_t. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_CLIENT_PROPERTY_STATUS,        /*!< Indicate Generic Client Properties Status message received from the server, with msg
                                                                              *   @ref rtk_bt_mesh_generic_properties_client_status_t. */
	RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_EVT_MAX,                       /*!< Reserved. */
} rtk_bt_mesh_generic_property_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_generic_user_properties_get_t
 * @brief     BLE MESH generic user properties get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_generic_user_properties_get_t;

/**
 * @struct    rtk_bt_mesh_generic_user_property_get_t
 * @brief     BLE MESH generic user property get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;      /*!< User Property ID of the property to get.
                                *   Range: 0x0001 to 0xFFFF. */
} rtk_bt_mesh_generic_user_property_get_t;

/**
 * @struct    rtk_bt_mesh_generic_user_property_set_t
 * @brief     BLE MESH generic user property set act message structure.
 */
typedef struct {
	uint16_t dst;                                     /*!< Destination address of the message. */
	uint16_t app_key_index;                           /*!< Index of the application key used to encrypt the message.
                                                       *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;                             /*!< User Property ID of the property to set.
                                                       *   Range: 0x0001 to 0xFFFF. */
	uint8_t pvalue[GENERIC_PROPERTY_DATA_MAX_LEN];    /*!< Raw value to write to the user property.
                                                       *   Capacity: @ref GENERIC_PROPERTY_DATA_MAX_LEN bytes. */
	uint8_t value_len;                                /*!< Length of the valid data in pvalue, in bytes.
                                                       *   Range: 1 to @ref GENERIC_PROPERTY_DATA_MAX_LEN. */
	bool ack;                                         /*!< Whether the server shall acknowledge the message.
                                                       *   - true: Send Generic User Property Set and the server replies with a Generic User Property Status message.
                                                       *   - false: Send Generic User Property Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_generic_user_property_set_t;

/**
 * @struct    rtk_bt_mesh_generic_admin_properties_get_t
 * @brief     BLE MESH generic admin properties get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_generic_admin_properties_get_t;

/**
 * @struct    rtk_bt_mesh_generic_admin_property_get_t
 * @brief     BLE MESH generic admin property get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;      /*!< Admin Property ID of the property to get.
                                *   Range: 0x0001 to 0xFFFF. */
} rtk_bt_mesh_generic_admin_property_get_t;

/**
 * @struct    rtk_bt_mesh_generic_admin_property_set_t
 * @brief     BLE MESH generic admin property set act message structure.
 */
typedef struct {
	uint16_t dst;                                     /*!< Destination address of the message. */
	uint16_t app_key_index;                           /*!< Index of the application key used to encrypt the message.
                                                       *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;                             /*!< Admin Property ID of the property to set.
                                                       *   Range: 0x0001 to 0xFFFF. */
	uint8_t property_access;                          /*!< Admin User Access of the property.
                                                       *   0x00: The device property is not a Generic User Property.
                                                       *   0x01: The device property is a Generic User Property and can be read.
                                                       *   0x02: The device property is a Generic User Property and can be written.
                                                       *   0x03: The device property is a Generic User Property and can be read and written.
                                                       *   0x04 to 0xFF: Prohibited. */
	uint8_t pvalue[GENERIC_PROPERTY_DATA_MAX_LEN];    /*!< Raw value to write to the admin property.
                                                       *   Capacity: @ref GENERIC_PROPERTY_DATA_MAX_LEN bytes. */
	uint8_t value_len;                                /*!< Length of the valid data in pvalue, in bytes.
                                                       *   Range: 1 to @ref GENERIC_PROPERTY_DATA_MAX_LEN. */
	bool ack;                                         /*!< Whether the server shall acknowledge the message.
                                                       *   - true: Send Generic Admin Property Set and the server replies with a Generic Admin Property Status message.
                                                       *   - false: Send Generic Admin Property Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_generic_admin_property_set_t;

/**
 * @struct    rtk_bt_mesh_generic_manu_properties_get_t
 * @brief     BLE MESH generic manufacturer properties get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_generic_manu_properties_get_t;

/**
 * @struct    rtk_bt_mesh_generic_manu_property_get_t
 * @brief     BLE MESH generic manufacturer property get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;      /*!< Manufacturer Property ID of the property to get.
                                *   Range: 0x0001 to 0xFFFF. */
} rtk_bt_mesh_generic_manu_property_get_t;

/**
 * @struct    rtk_bt_mesh_generic_manu_property_set_t
 * @brief     BLE MESH generic manufacturer property set act message structure.
 */
typedef struct {
	uint16_t dst;               /*!< Destination address of the message. */
	uint16_t app_key_index;     /*!< Index of the application key used to encrypt the message.
                                 *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;       /*!< Manufacturer Property ID of the property to set.
                                 *   Range: 0x0001 to 0xFFFF. */
	uint8_t property_access;    /*!< Manufacturer User Access of the property.
                                 *   0x00: The device property is not a Generic User Property.
                                 *   0x01: The device property is a Generic User Property and can be read.
                                 *   0x02 to 0xFF: Prohibited. */
	bool ack;                   /*!< Whether the server shall acknowledge the message.
                                 *   - true: Send Generic Manufacturer Property Set and the server replies with a Generic Manufacturer Property Status message.
                                 *   - false: Send Generic Manufacturer Property Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_generic_manu_property_set_t;

/**
 * @struct    rtk_bt_mesh_generic_client_properties_get_t
 * @brief     BLE MESH generic client properties get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;      /*!< Starting Client Property ID.
                                *   The server reports the Client Property IDs that follow this ID.
                                *   Range: 0x0001 to 0xFFFF. */
} rtk_bt_mesh_generic_client_properties_get_t;

/**
 * @struct    rtk_bt_mesh_generic_properties_client_status_t
 * @brief     BLE MESH generic client properties status evt message structure.
 */
typedef struct {
	uint16_t src;               /*!< Unicast address of the server that sent the Status message. */
	uint8_t num_ids;            /*!< Number of property IDs in pproperty_ids. */
	uint16_t *pproperty_ids;    /*!< Property IDs reported by the server, num_ids entries.
                                 *   Only valid during the event callback. */
} rtk_bt_mesh_generic_properties_client_status_t;

/**
 * @struct    rtk_bt_mesh_generic_property_client_status_t
 * @brief     BLE MESH generic client property status evt message structure.
 */
typedef struct {
	uint16_t src;                /*!< Unicast address of the server that sent the Status message. */
	uint16_t property_id;        /*!< Property ID reported by the server. */
	uint8_t property_access;     /*!< User Access of the property reported by the server, the valid values depend on the kind of property.
                                  *   User property: 0x01 read, 0x02 write, 0x03 read and write, other values are Prohibited.
                                  *   Admin property: 0x00 not a Generic User Property, 0x01 read, 0x02 write, 0x03 read and write, other values are Prohibited.
                                  *   Manufacturer property: 0x00 not a Generic User Property, 0x01 read, other values are Prohibited.
                                  *   0 together with value_len 0: The Status message carries neither User Access nor the property value. */
	uint16_t value_len;          /*!< Length of the data in pproperty_value, in bytes. */
	uint8_t *pproperty_value;    /*!< Raw property value reported by the server, value_len bytes.
                                  *   Only valid during the event callback. */
} rtk_bt_mesh_generic_property_client_status_t;

/**
 * @enum      rtk_bt_mesh_generic_user_property_server_model_evt_t
 * @brief     BLE MESH generic user property server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_PROPERTY_SET = 1,    /*!< Indicate a Generic User Property Set or Set Unacknowledged message is received and the
                                                                         *   property value has been updated.
                                                                         *   The msg is a byte stream:
                                                                         *   - Property ID, 2 bytes.
                                                                         *   - Value length, 2 bytes.
                                                                         *   - Property value, value length bytes.
                                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_PARAMETER_GET,       /*!< Indicate the server needs the access and length of a Generic User Property, with msg
                                                                         *   @ref rtk_bt_mesh_generic_property_server_para_get_t.
                                                                         *   The application shall write the parameters into the msg.
                                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_VALUE_GET,           /*!< Indicate the server needs the value of a Generic User Property, with msg
                                                                         *   @ref rtk_bt_mesh_generic_property_server_value_get_t.
                                                                         *   It follows event @ref RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_PARAMETER_GET.
                                                                         *   The value length is the one the application reported there.
                                                                         *   The application shall write the value into the msg.
                                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_NUM_IDS_GET,         /*!< Indicate the server needs the number of Generic User Property IDs, with msg
                                                                         *   @ref rtk_bt_mesh_generic_property_server_num_ids_get_t.
                                                                         *   The application shall write the number into the msg.
                                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_IDS_VALUE_GET,       /*!< Indicate the server needs the list of Generic User Property IDs, with msg
                                                                         *   @ref rtk_bt_mesh_generic_property_server_num_ids_value_get_t.
                                                                         *   It follows event @ref RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_NUM_IDS_GET.
                                                                         *   The number of IDs is the one the application reported there.
                                                                         *   The application shall write the IDs into the msg.
                                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_EVT_MAX,             /*!< Reserved. */
} rtk_bt_mesh_generic_user_property_server_model_evt_t;

/**
 * @enum      rtk_bt_mesh_generic_user_property_server_model_act_t
 * @brief     BLE MESH generic user property server model act definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_ACT_DB_SET = 1,    /*!< Set the Generic User Property database of the server. */
	RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_ACT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_generic_user_property_server_model_act_t;

/**
 * @struct    rtk_bt_mesh_generic_property_db_t
 * @brief     BLE MESH generic property server database structure.
 */
typedef struct {
	uint16_t property_id;        /*!< Property ID of this database entry.
                                  *   Range: 0x0001 to 0xFFFF. */
	uint8_t property_access;     /*!< User Access of this property, the valid values depend on the kind of property.
                                  *   User property: 0x01 read, 0x02 write, 0x03 read and write, other values are Prohibited.
                                  *   Admin property: 0x00 not a Generic User Property, 0x01 read, 0x02 write, 0x03 read and write, other values are Prohibited.
                                  *   Manufacturer property: 0x00 not a Generic User Property, 0x01 read, other values are Prohibited. */
	uint8_t *pproperty_value;    /*!< Raw property value, value_len bytes.
                                  *   The application owns the buffer and it must stay valid while the database is used. */
	uint16_t value_len;          /*!< Length of pproperty_value, in bytes. */
} rtk_bt_mesh_generic_property_db_t;

/**
 * @struct    rtk_bt_mesh_generic_user_property_server_set_t
 * @brief     BLE MESH generic user property server set evt message structure.
 */
typedef struct {
	uint16_t property_id;        /*!< Property ID of the property that has been updated. */
	uint8_t *pproperty_value;    /*!< New property value.
                                  *   The database has already been updated, this event only notifies the application. */
	uint16_t value_len;          /*!< Length of the new property value, in bytes. */
} rtk_bt_mesh_generic_user_property_server_set_t;

/**
 * @struct    rtk_bt_mesh_generic_property_server_para_t
 * @brief     BLE MESH generic property server parameter message structure.
 */
typedef struct {
	bool pdb;                   /*!< Whether the property database exists.
                                 *   - true: The property database exists.
                                 *   - false: The property database does not exist. */
	uint8_t property_access;    /*!< User Access of the property, the valid values depend on the kind of property.
                                 *   User property: 0x01 read, 0x02 write, 0x03 read and write, other values are Prohibited.
                                 *   Admin property: 0x00 not a Generic User Property, 0x01 read, 0x02 write, 0x03 read and write, other values are Prohibited.
                                 *   Manufacturer property: 0x00 not a Generic User Property, 0x01 read, other values are Prohibited. */
	uint16_t value_len;         /*!< Length of the property value, in bytes. */
} rtk_bt_mesh_generic_property_server_para_t;

/**
 * @struct    rtk_bt_mesh_generic_property_server_para_get_t
 * @brief     BLE MESH generic property server get parameter message structure.
 */
typedef struct {
	uint16_t property_id;                                  /*!< Property ID the server asks the parameters of. */
	rtk_bt_mesh_generic_property_server_para_t *pvalue;    /*!< Output: the application writes the parameters of the property here.
                                                            *   Must not be NULL. */
} rtk_bt_mesh_generic_property_server_para_get_t;

/**
 * @struct    rtk_bt_mesh_generic_property_server_value_get_t
 * @brief     BLE MESH generic property server get value message structure.
 */
typedef struct {
	uint16_t property_id;        /*!< Property ID the server asks the value of. */
	uint16_t value_len;          /*!< Length of the property value to provide, in bytes.
                                  *   It is the value_len the application reported in the preceding PARAMETER_GET event. */
	uint8_t *pproperty_value;    /*!< Output: the application writes the property value here, value_len bytes.
                                  *   Capacity: @ref GENERIC_PROPERTY_DATA_MAX_LEN bytes.
                                  *   Must not be NULL. */
} rtk_bt_mesh_generic_property_server_value_get_t;

/**
 * @struct    rtk_bt_mesh_generic_property_server_num_ids_t
 * @brief     BLE MESH generic property server num ids message structure.
 */
typedef struct {
	bool pdb;           /*!< Whether the property database exists.
                         *   - true: The property database exists.
                         *   - false: The property database does not exist. */
	uint8_t num_ids;    /*!< Number of property IDs the server reports. */
} rtk_bt_mesh_generic_property_server_num_ids_t;

/**
 * @struct    rtk_bt_mesh_generic_property_server_num_ids_get_t
 * @brief     BLE MESH generic property server num ids get message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_property_server_num_ids_t *pvalue;    /*!< Output: the application writes the number of property IDs here.
                                                               *   Must not be NULL. */
} rtk_bt_mesh_generic_property_server_num_ids_get_t;

/**
 * @struct    rtk_bt_mesh_generic_property_server_num_ids_value_get_t
 * @brief     BLE MESH generic user property server num ids value get message structure.
 */
typedef struct {
	uint8_t num_ids;           /*!< Number of property IDs to provide.
                                *   It is the num_ids the application reported in the preceding NUM_IDS_GET event. */
	uint16_t *property_ids;    /*!< Output: the application writes the property IDs here, num_ids entries.
                                *   Capacity: @ref GENERIC_PROPERTY_DATA_MAX_LEN entries.
                                *   Must not be NULL. */
} rtk_bt_mesh_generic_property_server_num_ids_value_get_t;

/**
 * @enum      rtk_bt_mesh_generic_admin_property_server_model_evt_t
 * @brief     BLE MESH generic admin property server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_PROPERTY_SET = 1,    /*!< Indicate a Generic Admin Property Set or Set Unacknowledged message is received and the
                                                                          *   property has been updated.
                                                                          *   The msg is a byte stream:
                                                                          *   - Property ID, 2 bytes.
                                                                          *   - User Access, 1 byte.
                                                                          *   - Value length, 2 bytes.
                                                                          *   - Property value, value length bytes.
                                                                          *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_PARAMETER_GET,       /*!< Indicate the server needs the access and length of a Generic Admin Property, with msg
                                                                          *   @ref rtk_bt_mesh_generic_property_server_para_get_t.
                                                                          *   The application shall write the parameters into the msg.
                                                                          *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_VALUE_GET,           /*!< Indicate the server needs the value of a Generic Admin Property, with msg
                                                                          *   @ref rtk_bt_mesh_generic_property_server_value_get_t.
                                                                          *   It follows event @ref RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_PARAMETER_GET.
                                                                          *   The value length is the one the application reported there.
                                                                          *   The application shall write the value into the msg.
                                                                          *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_NUM_IDS_GET,         /*!< Indicate the server needs the number of Generic Admin Property IDs, with msg
                                                                          *   @ref rtk_bt_mesh_generic_property_server_num_ids_get_t.
                                                                          *   The application shall write the number into the msg.
                                                                          *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_IDS_VALUE_GET,       /*!< Indicate the server needs the list of Generic Admin Property IDs, with msg
                                                                          *   @ref rtk_bt_mesh_generic_property_server_num_ids_value_get_t.
                                                                          *   It follows event @ref RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_NUM_IDS_GET.
                                                                          *   The number of IDs is the one the application reported there.
                                                                          *   The application shall write the IDs into the msg.
                                                                          *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_EVT_MAX,             /*!< Reserved. */
} rtk_bt_mesh_generic_admin_property_server_model_evt_t;

/**
 * @enum      rtk_bt_mesh_generic_admin_property_server_model_act_t
 * @brief     BLE MESH generic admin property server model act definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_ACT_DB_SET = 1,    /*!< Set the Generic Admin Property database of the server. */
	RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_ACT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_generic_admin_property_server_model_act_t;

/**
 * @enum      rtk_bt_mesh_generic_manu_property_server_model_evt_t
 * @brief     BLE MESH generic manu property server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_PROPERTY_SET = 1,    /*!< Indicate a Generic Manufacturer Property Set or Set Unacknowledged message is received and
                                                                         *   the User Access has been updated, with msg
                                                                         *   @ref rtk_bt_mesh_generic_manu_property_server_set_t.
                                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_PARAMETER_GET,       /*!< Indicate the server needs the access and length of a Generic Manufacturer Property, with msg
                                                                         *   @ref rtk_bt_mesh_generic_property_server_para_get_t.
                                                                         *   The application shall write the parameters into the msg.
                                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_VALUE_GET,           /*!< Indicate the server needs the value of a Generic Manufacturer Property, with msg
                                                                         *   @ref rtk_bt_mesh_generic_property_server_value_get_t.
                                                                         *   It follows event @ref RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_PARAMETER_GET.
                                                                         *   The value length is the one the application reported there.
                                                                         *   The application shall write the value into the msg.
                                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_NUM_IDS_GET,         /*!< Indicate the server needs the number of Generic Manufacturer Property IDs, with msg
                                                                         *   @ref rtk_bt_mesh_generic_property_server_num_ids_get_t.
                                                                         *   The application shall write the number into the msg.
                                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_IDS_VALUE_GET,       /*!< Indicate the server needs the list of Generic Manufacturer Property IDs, with msg
                                                                         *   @ref rtk_bt_mesh_generic_property_server_num_ids_value_get_t.
                                                                         *   It follows event @ref RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_NUM_IDS_GET.
                                                                         *   The number of IDs is the one the application reported there.
                                                                         *   The application shall write the IDs into the msg.
                                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_EVT_MAX,             /*!< Reserved. */
} rtk_bt_mesh_generic_manu_property_server_model_evt_t;

/**
 * @enum      rtk_bt_mesh_generic_manu_property_server_model_act_t
 * @brief     BLE MESH generic manu property server model act definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_ACT_DB_SET = 1,    /*!< Set the Generic Manufacturer Property database of the server. */
	RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_ACT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_generic_manu_property_server_model_act_t;

/**
 * @struct    rtk_bt_mesh_generic_manu_property_server_set_t
 * @brief     BLE MESH generic manufacturer property server set message structure.
 */
typedef struct {
	uint16_t property_id;       /*!< Property ID of the property whose User Access has been updated. */
	uint8_t property_access;    /*!< New User Access of the property.
                                 *   The database has already been updated, this event only notifies the application. */
} rtk_bt_mesh_generic_manu_property_server_set_t;

/**
 * @enum      rtk_bt_mesh_generic_client_property_server_model_evt_t
 * @brief     BLE MESH generic client property server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_CLIENT_PROPERTY_SERVER_MODEL_PROPERTY_GET = 1,    /*!< Indicate a client asked the Generic Client Properties of the server, with msg
                                                                           *   @ref rtk_bt_mesh_generic_client_property_server_direct_get_t.
                                                                           *   The application shall write the Client Property IDs into the msg.
                                                                           *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_CLIENT_PROPERTY_SERVER_MODEL_EVT_MAX,             /*!< Reserved. */
} rtk_bt_mesh_generic_client_property_server_model_evt_t;

/**
 * @struct    rtk_bt_mesh_generic_client_property_server_direct_get_t
 * @brief     BLE MESH generic client property server get for direct calling message structure.
 */
typedef struct {
	uint16_t property_id_start;    /*!< Starting Client Property ID asked by the client. */
	uint8_t *value;                /*!< Output: the application writes the Client Property IDs here.
                                    *   The first byte is the number of IDs, followed by the IDs with 2 bytes each.
                                    *   Capacity: @ref GENERIC_PROPERTY_DATA_MAX_LEN bytes.
                                    *   Must not be NULL. */
} rtk_bt_mesh_generic_client_property_server_direct_get_t;

/**
 * @struct    rtk_bt_mesh_generic_property_server_db_set_t
 * @brief     BLE MESH generic property server database set message structure.
 */
typedef struct {
	uint16_t num_properties;    /*!< Number of entries in the database. */
	void *pdata_db;             /*!< Property database, num_properties entries of @ref rtk_bt_mesh_generic_property_db_t.
                                 *   The application owns the buffer and it must stay valid while the database is used. */
} rtk_bt_mesh_generic_property_server_db_set_t;

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
/** @addtogroup BT_Mesh_Generic_Model_Functions BT Mesh Generic Model Functions
 * @{
 */

/**
 * @brief     Send a Generic Level Get message to a generic level server to read its Generic Level state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_STATUS.
 * @param[in] level_get: Generic Level Get message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_level_client_get(rtk_bt_mesh_generic_level_get_t *level_get);

/**
 * @brief     Send a Generic Level Set or Set Unacknowledged message to a generic level server to change its Generic Level state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_STATUS.
 * @param[in] level_set: Generic Level Set or Set Unacknowledged message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_level_client_set(rtk_bt_mesh_generic_level_set_t *level_set);

/**
 * @brief     Send a Generic Move Set or Move Set Unacknowledged message to a generic level server to start changing its Generic Level state continuously.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_STATUS.
 * @param[in] move_set: Generic Move Set or Move Set Unacknowledged message parameters.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_level_client_move_set(rtk_bt_mesh_generic_level_move_set_t *move_set);

/**
 * @brief     Send a Generic Delta Set or Delta Set Unacknowledged message to a generic level server to change its Generic Level state by a delta.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_STATUS.
 * @param[in] delta_set: Generic Delta Set or Delta Set Unacknowledged message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_level_client_delta_set(rtk_bt_mesh_generic_level_delta_set_t *delta_set);

/********************************* Generic Power On Off ********************************/

/**
 * @brief     Send a Generic OnPowerUp Get message to a generic power on off server to read its Generic OnPowerUp state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_ON_POWER_UP_CLIENT_MODEL_STATUS.
 * @param[in] power_get: Generic OnPowerUp Get message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_on_power_up_client_get(rtk_bt_mesh_generic_on_power_up_get_t *power_get);

/**
 * @brief     Send a Generic OnPowerUp Set or Set Unacknowledged message to a generic power on off setup server to change its Generic OnPowerUp state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_ON_POWER_UP_CLIENT_MODEL_STATUS.
 * @param[in] power_set: Generic OnPowerUp Set or Set Unacknowledged message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_on_power_up_client_set(rtk_bt_mesh_generic_on_power_up_set_t *power_set);

/********************************* Generic Power Level *********************************/

/**
 * @brief     Send a Generic Power Level Get message to a generic power level server to read its Generic Power Actual state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_STATUS.
 * @param[in] power_level_get: Generic Power Level Get message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_power_level_client_get(rtk_bt_mesh_generic_power_level_get_t *power_level_get);

/**
 * @brief     Send a Generic Power Level Set or Set Unacknowledged message to a generic power level server to change its Generic Power Actual state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_STATUS.
 * @param[in] power_level_set: Generic Power Level Set or Set Unacknowledged message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_power_level_client_set(rtk_bt_mesh_generic_power_level_set_t *power_level_set);

/**
 * @brief     Send a Generic Power Last Get message to a generic power level server to read its Generic Power Last state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_LAST_STATUS.
 * @param[in] power_level_last_get: Generic Power Last Get message parameters.
 *                                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_power_level_client_last_get(rtk_bt_mesh_generic_power_level_last_get_t *power_level_last_get);

/**
 * @brief     Send a Generic Power Default Get message to a generic power level server to read its Generic Power Default state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_DEFAULT_STATUS.
 * @param[in] power_level_default_get: Generic Power Default Get message parameters.
 *                                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_power_level_client_default_get(rtk_bt_mesh_generic_power_level_default_get_t *power_level_default_get);

/**
 * @brief     Send a Generic Power Default Set or Set Unacknowledged message to a generic power level setup server to change its Generic Power Default state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_DEFAULT_STATUS.
 * @param[in] power_level_default_set: Generic Power Default Set or Set Unacknowledged message parameters.
 *                                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_power_level_client_default_set(rtk_bt_mesh_generic_power_level_default_set_t *power_level_default_set);

/**
 * @brief     Send a Generic Power Range Get message to a generic power level server to read its Generic Power Range state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_RANGE_STATUS.
 * @param[in] power_level_range_get: Generic Power Range Get message parameters.
 *                                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_power_level_client_range_get(rtk_bt_mesh_generic_power_level_range_get_t *power_level_range_get);

/**
 * @brief     Send a Generic Power Range Set or Set Unacknowledged message to a generic power level setup server to change its Generic Power Range state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_RANGE_STATUS.
 * @param[in] power_level_range_set: Generic Power Range Set or Set Unacknowledged message parameters.
 *                                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_power_level_client_range_set(rtk_bt_mesh_generic_power_level_range_set_t *power_level_range_set);

/*********************************** Generic Battery ***********************************/
/**
 * @brief     Send a Generic Battery Get message to a generic battery server to read its Generic Battery state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_BATTERY_CLIENT_MODEL_STATUS.
 * @param[in] battery_get: Generic Battery Get message parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_battery_client_get(rtk_bt_mesh_generic_battery_get_t *battery_get);

/*********************************** Generic Location **********************************/
/**
 * @brief     Send a Generic Location Global Get message to a generic location server to read its global location.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_GLOBAL_STATUS.
 * @param[in] global_get: Generic Location Global Get message parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_location_client_global_get(rtk_bt_mesh_generic_location_global_get_t *global_get);

/**
 * @brief     Send a Generic Location Global Set or Set Unacknowledged message to a generic location setup server to change its global location.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_GLOBAL_STATUS.
 * @param[in] global_set: Generic Location Global Set or Set Unacknowledged message parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_location_client_global_set(rtk_bt_mesh_generic_location_global_set_t *global_set);

/**
 * @brief     Send a Generic Location Local Get message to a generic location server to read its local location.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_LOCAL_STATUS.
 * @param[in] local_get: Generic Location Local Get message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_location_client_local_get(rtk_bt_mesh_generic_location_local_get_t *local_get);

/**
 * @brief     Send a Generic Location Local Set or Set Unacknowledged message to a generic location setup server to change its local location.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_LOCAL_STATUS.
 * @param[in] local_set: Generic Location Local Set or Set Unacknowledged message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_location_client_local_set(rtk_bt_mesh_generic_location_local_set_t *local_set);

/*********************************** Generic Property **********************************/
/**
 * @brief     Send a Generic User Properties Get message to a generic user property server to read the Property IDs of its user properties.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTIES_STATUS.
 * @param[in] user_get: Generic User Properties Get message parameters.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_property_client_user_properties_get(rtk_bt_mesh_generic_user_properties_get_t *user_get);

/**
 * @brief     Send a Generic User Property Get message to a generic user property server to read one of its user properties.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTY_STATUS.
 * @param[in] user_property_get: Generic User Property Get message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_property_client_user_property_get(rtk_bt_mesh_generic_user_property_get_t *user_property_get);

/**
 * @brief     Send a Generic User Property Set or Set Unacknowledged message to a generic user property server to change one of its user properties.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTY_STATUS.
 * @param[in] user_property_set: Generic User Property Set or Set Unacknowledged message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_property_client_user_property_set(rtk_bt_mesh_generic_user_property_set_t *user_property_set);

/**
 * @brief     Send a Generic Admin Properties Get message to a generic admin property server to read the Property IDs of its admin properties.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTIES_STATUS.
 * @param[in] admin_get: Generic Admin Properties Get message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_property_client_admin_properties_get(rtk_bt_mesh_generic_admin_properties_get_t *admin_get);

/**
 * @brief     Send a Generic Admin Property Get message to a generic admin property server to read one of its admin properties.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTY_STATUS.
 * @param[in] admin_property_get: Generic Admin Property Get message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_property_client_admin_property_get(rtk_bt_mesh_generic_admin_property_get_t *admin_property_get);

/**
 * @brief     Send a Generic Admin Property Set or Set Unacknowledged message to a generic admin property server to change one of its admin properties.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTY_STATUS.
 * @param[in] admin_property_set: Generic Admin Property Set or Set Unacknowledged message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_property_client_admin_property_set(rtk_bt_mesh_generic_admin_property_set_t *admin_property_set);

/**
 * @brief     Send a Generic Manufacturer Properties Get message to a generic manufacturer property server to read the Property IDs of its manufacturer properties.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTIES_STATUS.
 * @param[in] manu_get: Generic Manufacturer Properties Get message parameters.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_property_client_manu_properties_get(rtk_bt_mesh_generic_manu_properties_get_t *manu_get);

/**
 * @brief     Send a Generic Manufacturer Property Get message to a generic manufacturer property server to read one of its manufacturer properties.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTY_STATUS.
 * @param[in] manu_property_get: Generic Manufacturer Property Get message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_property_client_manu_property_get(rtk_bt_mesh_generic_manu_property_get_t *manu_property_get);

/**
 * @brief     Send a Generic Manufacturer Property Set or Set Unacknowledged message to a generic manufacturer property server to change one of its manufacturer
 *            properties.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTY_STATUS.
 * @param[in] manu_property_set: Generic Manufacturer Property Set or Set Unacknowledged message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_property_client_manu_property_set(rtk_bt_mesh_generic_manu_property_set_t *manu_property_set);

/**
 * @brief     Send a Generic Client Properties Get message to a generic client property server to read the Property IDs of its client properties.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_CLIENT_PROPERTY_STATUS.
 * @param[in] client_get: Generic Client Properties Get message parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_property_client_client_properties_get(rtk_bt_mesh_generic_client_properties_get_t *client_get);

/**
 * @brief     Set the user property database of the local generic user property server.
 *            The database is stored locally and no mesh message is sent.
 * @param[in] db_set: Property database parameters.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The server uses a default property database until this API replaces it. Call this API to use the application's own properties.
 */
uint16_t rtk_bt_mesh_generic_property_client_user_db_set(rtk_bt_mesh_generic_property_server_db_set_t *db_set);

/**
 * @brief     Set the admin property database of the local generic admin property server.
 *            The database is stored locally and no mesh message is sent.
 * @param[in] db_set: Property database parameters.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The server uses a default property database until this API replaces it. Call this API to use the application's own properties.
 */
uint16_t rtk_bt_mesh_generic_property_client_admin_db_set(rtk_bt_mesh_generic_property_server_db_set_t *db_set);

/**
 * @brief     Set the manufacturer property database of the local generic manufacturer property server.
 *            The database is stored locally and no mesh message is sent.
 * @param[in] db_set: Property database parameters.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The server uses a default property database until this API replaces it. Call this API to use the application's own properties.
 */
uint16_t rtk_bt_mesh_generic_property_client_manu_db_set(rtk_bt_mesh_generic_property_server_db_set_t *db_set);

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

#endif /* __RTK_BT_MESH_GENERIC_MODEL_H__ */
