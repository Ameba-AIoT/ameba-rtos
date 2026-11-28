/**
 * @file      rtk_bt_mesh_light_model.h
 * @author    quinn_yang@realsil.com.cn
 * @brief     Bluetooth LE MESH light client and server model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */


#ifndef __RTK_BT_MESH_LIGHT_LIGHTNESS_MODEL_H__
#define __RTK_BT_MESH_LIGHT_LIGHTNESS_MODEL_H__

#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_generic_onoff_model.h>

/** @addtogroup BT_Constants BT Constants
 * @{
 */
/** @addtogroup BT_Mesh_Constants BT Mesh Constants
 * @{
 */
/** @addtogroup BT_Mesh_Light_Model_Constants BT Mesh Light Model Constants
 * @{
 */

/**
 * @def       LIGHT_LC_PROPERTY_DATA_MAX_LEN
 * @brief     Maximum length in bytes of the raw value of a Light LC property.
 */
#define LIGHT_LC_PROPERTY_DATA_MAX_LEN    0x10

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

/************************************** Data Types *************************************/

/** @addtogroup BT_Types BT Types
 * @{
 */
/** @addtogroup BT_Mesh_Types BT Mesh Types
 * @{
 */
/** @addtogroup BT_Mesh_Light_Model_Types BT Mesh Light Model Types
 * @{
 */
/******************************** Light Lightness Model ********************************/
/***************************** Light Lightness Client Model ****************************/
/**
 * @enum      rtk_bt_mesh_light_lightness_client_model_act_t
 * @brief     BLE MESH light lightness client model act definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL_GET = 1,        /*!< Send a Light Lightness Get message to the server. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL_SET,            /*!< Send a Light Lightness Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_CLIENT_MODEL_GET,     /*!< Send a Light Lightness Linear Get message to the server. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_CLIENT_MODEL_SET,     /*!< Send a Light Lightness Linear Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_LAST_CLIENT_MODEL_GET,       /*!< Send a Light Lightness Last Get message to the server. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_CLIENT_MODEL_GET,    /*!< Send a Light Lightness Default Get message to the server. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_CLIENT_MODEL_SET,    /*!< Send a Light Lightness Default Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_CLIENT_MODEL_GET,      /*!< Send a Light Lightness Range Get message to the server. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_CLIENT_MODEL_SET,      /*!< Send a Light Lightness Range Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL_ACT_MAX,        /*!< Reserved. */
} rtk_bt_mesh_light_lightness_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_light_lightness_client_model_evt_t
 * @brief     BLE MESH light lightness client model event definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL_STATUS = 1,        /*!< Indicate Light Lightness Status message received from the server, with msg
                                                                 *   @ref rtk_bt_mesh_light_lightness_client_status_t. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_CLIENT_MODEL_STATUS,     /*!< Indicate Light Lightness Linear Status message received from the server, with msg
                                                                 *   @ref rtk_bt_mesh_light_lightness_client_status_t. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_LAST_CLIENT_MODEL_STATUS,       /*!< Indicate Light Lightness Last Status message received from the server, with msg
                                                                 *   @ref rtk_bt_mesh_light_lightness_client_status_last_t. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_CLIENT_MODEL_STATUS,    /*!< Indicate Light Lightness Default Status message received from the server, with msg
                                                                 *   @ref rtk_bt_mesh_light_lightness_client_status_default_t. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_CLIENT_MODEL_STATUS,      /*!< Indicate Light Lightness Range Status message received from the server, with msg
                                                                 *   @ref rtk_bt_mesh_light_lightness_client_status_range_t. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL_EVT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_light_lightness_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_get_t
 * @brief     BLE MESH light lightness get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_lightness_get_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_set_t
 * @brief     BLE MESH light lightness set act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	uint16_t lightness;                                  /*!< Target Light Lightness Actual state.
                                                          *   0x0000: Light is not emitted by the element.
                                                          *   0x0001 to 0xFFFE: Perceived lightness of the light emitted by the element.
                                                          *   0xFFFF: Highest perceived lightness of the light emitted by the element. */
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
                                                          *   - true: Send Light Lightness Set and the server replies with a Light Lightness Status message.
                                                          *   - false: Send Light Lightness Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_lightness_set_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_linear_get_t
 * @brief     BLE MESH light lightness linear get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_lightness_linear_get_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_linear_set_t
 * @brief     BLE MESH light lightness linear set act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	uint16_t lightness;                                  /*!< Target Light Lightness Linear state.
                                                          *   0x0000: Light is not emitted by the element.
                                                          *   0x0001 to 0xFFFE: Lightness of the light emitted by the element.
                                                          *   0xFFFF: Highest lightness of the light emitted by the element. */
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
                                                          *   - true: Send Light Lightness Linear Set and the server replies with a Light Lightness Linear Status message.
                                                          *   - false: Send Light Lightness Linear Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_lightness_linear_set_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_last_get_t
 * @brief     BLE MESH light lightness last get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_lightness_last_get_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_default_get_t
 * @brief     BLE MESH light lightness default get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_lightness_default_get_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_default_set_t
 * @brief     BLE MESH light lightness default set act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t lightness;        /*!< Light Lightness Default state to set.
                                *   0x0000: Use the Light Lightness Last state as the default value.
                                *   0x0001 to 0xFFFE: Perceived lightness of the light emitted by the element.
                                *   0xFFFF: Highest perceived lightness of the light emitted by the element. */
	bool ack;                  /*!< Whether the server shall acknowledge the message.
                                *   - true: Send Light Lightness Default Set and the server replies with a Light Lightness Default Status message.
                                *   - false: Send Light Lightness Default Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_lightness_default_set_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_range_get_t
 * @brief     BLE MESH light lightness range get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_lightness_range_get_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_range_set_t
 * @brief     BLE MESH light lightness range set act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t range_min;        /*!< Light Lightness Range Min to set.
                                *   0x0000: Prohibited.
                                *   0x0001 to 0xFFFF: Lightness of the element. */
	uint16_t range_max;        /*!< Light Lightness Range Max to set.
                                *   0x0000: Prohibited.
                                *   0x0001 to 0xFFFF: Lightness of the element.
                                *   Must be greater than or equal to range_min. */
	bool ack;                  /*!< Whether the server shall acknowledge the message.
                                *   - true: Send Light Lightness Range Set and the server replies with a Light Lightness Range Status message.
                                *   - false: Send Light Lightness Range Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_lightness_range_set_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_client_status_t
 * @brief     BLE MESH light lightness client model status message structure.
 */
typedef struct {
	uint16_t src;                                            /*!< Unicast address of the server that sent the Status message. */
	uint16_t present_lightness;                              /*!< Present Light Lightness Actual or Linear state of the server, depending on the event. */
	bool optional;                                           /*!< Whether the Target Lightness and Remaining Time fields are carried in the message.
                                                              *   - true: target_lightness and remaining_time are valid.
                                                              *     The server is changing the state or waiting for the delay to complete.
                                                              *   - false: target_lightness and remaining_time are not valid. */
	uint16_t target_lightness;                               /*!< Target Light Lightness Actual or Linear state the server is changing to.
                                                              *   Only valid when optional is true. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   Only valid when optional is true. */
} rtk_bt_mesh_light_lightness_client_status_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_client_status_last_t
 * @brief     BLE MESH light lightness client model last status message structure.
 */
typedef struct {
	uint16_t src;          /*!< Unicast address of the server that sent the Status message. */
	uint16_t lightness;    /*!< Light Lightness Last state reported by the server.
                            *   0x0000: Prohibited.
                            *   0x0001 to 0xFFFE: Perceived lightness of the light emitted by the element.
                            *   0xFFFF: Highest perceived lightness of the light emitted by the element. */
} rtk_bt_mesh_light_lightness_client_status_last_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_client_status_default_t
 * @brief     BLE MESH light lightness client model default status message structure.
 */
typedef struct {
	uint16_t src;          /*!< Unicast address of the server that sent the Status message. */
	uint16_t lightness;    /*!< Light Lightness Default state reported by the server.
                            *   0x0000: Use the Light Lightness Last state as the default value.
                            *   0x0001 to 0xFFFE: Perceived lightness of the light emitted by the element.
                            *   0xFFFF: Highest perceived lightness of the light emitted by the element. */
} rtk_bt_mesh_light_lightness_client_status_default_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_client_status_range_t
 * @brief     BLE MESH light lightness client model range status message structure.
 */
typedef struct {
	uint16_t src;                         /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_generic_stat_t status;    /*!< Status code of the last operation on the Light Lightness Range state. */
	uint16_t range_min;                   /*!< Light Lightness Range Min reported by the server. */
	uint16_t range_max;                   /*!< Light Lightness Range Max reported by the server. */
} rtk_bt_mesh_light_lightness_client_status_range_t;

/***************************** Light Lightness Server Model ****************************/
/**
 * @enum      rtk_bt_mesh_light_lightness_server_model_evt_t
 * @brief     BLE MESH light lightness server model event definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_LIGHTNESS_SERVER_MODEL_GET = 1,        /*!< Indicate a Light Lightness Get message is received or the server needs the current Light Lightness
                                                              *   Actual state, with msg @ref rtk_bt_mesh_light_lightness_server_direct_get_t.
                                                              *   The application shall write the current Light Lightness Actual state into the msg.
                                                              *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_SERVER_MODEL_GET,     /*!< Indicate a Light Lightness Linear Get message is received or the server needs the current Light
                                                              *   Lightness Linear state, with msg @ref rtk_bt_mesh_light_lightness_server_direct_get_t.
                                                              *   The application shall write the current Light Lightness Linear state into the msg.
                                                              *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_SERVER_MODEL_GET,    /*!< Indicate Light Lightness Default Get message received from a client, with msg
                                                              *   @ref rtk_bt_mesh_light_lightness_server_direct_get_t.
                                                              *   The application shall write the current Light Lightness Default state into the msg.
                                                              *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_LAST_SERVER_MODEL_GET,       /*!< Indicate Light Lightness Last Get message received from a client, with msg
                                                              *   @ref rtk_bt_mesh_light_lightness_server_direct_get_t.
                                                              *   The application shall write the current Light Lightness Last state into the msg.
                                                              *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_SERVER_MODEL_GET,      /*!< Indicate a Light Lightness Range Get message is received or the server needs the current Light Lightness
                                                              *   Range state, with msg @ref rtk_bt_mesh_light_lightness_server_direct_get_range_t.
                                                              *   The application shall write the current Light Lightness Range state into the msg.
                                                              *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_SERVER_MODEL_SET,            /*!< Indicate the Light Lightness Actual state is to change, with msg
                                                              *   @ref rtk_bt_mesh_light_lightness_server_set_t.
                                                              *   Raised when a Set message is received and again when the transition is updated or stopped.
                                                              *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_SERVER_MODEL_SET,     /*!< Indicate the Light Lightness Linear state is to change, with msg
                                                              *   @ref rtk_bt_mesh_light_lightness_server_set_t.
                                                              *   Raised when a Linear Set message is received and again when the transition is updated or stopped.
                                                              *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_LAST_SERVER_MODEL_SET,       /*!< Indicate the Light Lightness Last state needs to be updated, with msg
                                                              *   @ref rtk_bt_mesh_light_lightness_server_set_last_t.
                                                              *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_SERVER_MODEL_EVT_MAX,        /*!< Reserved. */
} rtk_bt_mesh_light_lightness_server_model_evt_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_server_get_t
 * @brief     BLE MESH light lightness get evt message structure.
 */
typedef struct {
	uint16_t lightness;    /*!< Output: the current Light Lightness value of the state indicated by the event, written by the application. */
} rtk_bt_mesh_light_lightness_server_get_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_server_direct_get_t
 * @brief     BLE MESH light lightness get direct evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_lightness_server_get_t *value;    /*!< Output: the application writes the current Light Lightness value here.
                                                         *   Must not be NULL. */
} rtk_bt_mesh_light_lightness_server_direct_get_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_server_get_range_t
 * @brief     BLE MESH light lightness get range evt message structure.
 */
typedef struct {
	uint16_t range_min;    /*!< Light Lightness Range Min. */
	uint16_t range_max;    /*!< Light Lightness Range Max. */
} rtk_bt_mesh_light_lightness_server_get_range_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_server_direct_get_range_t
 * @brief     BLE MESH light lightness get range direct evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_lightness_server_get_range_t *value;    /*!< Output: the application writes the current Light Lightness Range here.
                                                               *   Must not be NULL. */
} rtk_bt_mesh_light_lightness_server_direct_get_range_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_server_get_default_transition_time_t
 * @brief     BLE MESH light lightness get default transition time evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Generic Default Transition Time of the server. */
} rtk_bt_mesh_light_lightness_server_get_default_transition_time_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_server_set_t
 * @brief     BLE MESH light lightness set evt message structure.
 */
typedef struct {
	uint16_t lightness;                                      /*!< Target Light Lightness Actual or Linear state requested by the client, depending on the event.
                                                              *   Range: 0x0000 to 0xFFFF. */
	rtk_bt_mesh_generic_transition_time_t total_time;        /*!< Total time of the transition to the target state.
                                                              *   A zero value means the state changes immediately. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   A zero value means the state has reached the target. */
} rtk_bt_mesh_light_lightness_server_set_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_server_set_last_t
 * @brief     BLE MESH light lightness set last evt message structure.
 */
typedef struct {
	uint16_t lightness;    /*!< Light Lightness Last state to store.
                            *   Range: 0x0001 to 0xFFFF. */
} rtk_bt_mesh_light_lightness_server_set_last_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_server_set_default_t
 * @brief     BLE MESH light lightness set default evt message structure.
 */
typedef struct {
	uint16_t lightness;    /*!< Light Lightness Default state requested by the client.
                            *   0x0000: Use the Light Lightness Last state as the default value.
                            *   The stack only reports this value, handle it in the application if needed. */
} rtk_bt_mesh_light_lightness_server_set_default_t;

/**
 * @struct    rtk_bt_mesh_light_lightness_server_set_range_t
 * @brief     BLE MESH light lightness set range evt message structure.
 */
typedef struct {
	uint16_t range_min;    /*!< Light Lightness Range Min requested by the client.
                            *   Range: 0x0001 to 0xFFFF. */
	uint16_t range_max;    /*!< Light Lightness Range Max requested by the client.
                            *   Range: 0x0001 to 0xFFFF. */
} rtk_bt_mesh_light_lightness_server_set_range_t;

/************************** Light Lightness Setup Server Model *************************/
/**
 * @enum      rtk_bt_mesh_light_lightness_setup_server_model_evt_t
 * @brief     BLE MESH light lightness setup server model event definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_SERVER_MODEL_SET = 1,    /*!< Indicate Light Lightness Default Set or Set Unacknowledged message received from a client,
                                                                  *   with msg @ref rtk_bt_mesh_light_lightness_server_set_default_t. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_SERVER_MODEL_SET,          /*!< Indicate Light Lightness Range Set or Set Unacknowledged message received from a client,
                                                                  *   with msg @ref rtk_bt_mesh_light_lightness_server_set_range_t. */
	RTK_BT_MESH_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL_EVT_MAX,      /*!< Reserved. */
} rtk_bt_mesh_light_lightness_setup_server_model_evt_t;

/*********************************** Light CTL Model ***********************************/
/******************************** Light CTL Client Model *******************************/
/**
 * @enum      rtk_bt_mesh_light_ctl_client_model_act_t
 * @brief     BLE MESH light ctl client model act definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_GET = 1,                  /*!< Send a Light CTL Get message to the server. */
	RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_SET,                      /*!< Send a Light CTL Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_GET,          /*!< Send a Light CTL Temperature Get message to the server. */
	RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_SET,          /*!< Send a Light CTL Temperature Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_RANGE_GET,    /*!< Send a Light CTL Temperature Range Get message to the server. */
	RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_RANGE_SET,    /*!< Send a Light CTL Temperature Range Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_DEFAULT_GET,              /*!< Send a Light CTL Default Get message to the server. */
	RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_DEFAULT_SET,              /*!< Send a Light CTL Default Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_ACT_MAX,                  /*!< Reserved. */
} rtk_bt_mesh_light_ctl_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_light_ctl_client_model_evt_t
 * @brief     BLE MESH light ctl client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_STATUS = 1,                  /*!< Indicate Light CTL Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_light_ctl_client_status_t. */
	RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_STATUS,          /*!< Indicate Light CTL Temperature Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_light_ctl_client_status_temperature_t. */
	RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_RANGE_STATUS,    /*!< Indicate Light CTL Temperature Range Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_light_ctl_client_status_temperature_range_t. */
	RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_DEFAULT_STATUS,              /*!< Indicate Light CTL Default Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_light_ctl_client_status_default_t. */
	RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_EVT_MAX,                     /*!< Reserved. */
} rtk_bt_mesh_light_ctl_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_get_t
 * @brief     BLE MESH light ctl get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_ctl_get_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_set_t
 * @brief     BLE MESH light ctl set act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	uint16_t lightness;                                  /*!< Target Light CTL Lightness state.
                                                          *   0x0000: Light is not emitted by the element.
                                                          *   0x0001 to 0xFFFE: The perceived lightness of a light emitted by the element.
                                                          *   0xFFFF: The highest perceived lightness of a light emitted by the element. */
	uint16_t temperature;                                /*!< Target Light CTL Temperature state.
                                                          *   Unit: kelvin.
                                                          *   Range: 0x0320 to 0x4E20. */
	int16_t delta_uv;                                    /*!< Target Light CTL Delta UV state.
                                                          *   Range: -32768 to 32767.
                                                          *   Represented Delta UV = value / 32768. */
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
                                                          *   - true: Send Light CTL Set and the server replies with a Light CTL Status message.
                                                          *   - false: Send Light CTL Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_ctl_set_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_temperature_get_t
 * @brief     BLE MESH light ctl temperature get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_ctl_temperature_get_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_temperature_set_t
 * @brief     BLE MESH light ctl temperature set act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	uint16_t temperature;                                /*!< Target Light CTL Temperature state.
                                                          *   Unit: kelvin.
                                                          *   Range: 0x0320 to 0x4E20. */
	int16_t delta_uv;                                    /*!< Target Light CTL Delta UV state.
                                                          *   Range: -32768 to 32767.
                                                          *   Represented Delta UV = value / 32768. */
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
                                                          *   - true: Send Light CTL Temperature Set and the server replies with a Light CTL Temperature Status message.
                                                          *   - false: Send Light CTL Temperature Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_ctl_temperature_set_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_temperature_range_get_t
 * @brief     BLE MESH light ctl temperature range get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_ctl_temperature_range_get_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_temperature_range_set_t
 * @brief     BLE MESH light ctl temperature range set act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t range_min;        /*!< Light CTL Temperature Range Min to set.
                                *   Unit: kelvin.
                                *   0x0320 to 0x4E20: The color temperature of white light.
                                *   0xFFFF: The color temperature of white light is unknown.
                                *   Other values are Prohibited. */
	uint16_t range_max;        /*!< Light CTL Temperature Range Max to set.
                                *   Unit: kelvin.
                                *   0x0320 to 0x4E20: The color temperature of white light.
                                *   0xFFFF: The color temperature of white light is unknown.
                                *   Other values are Prohibited.
                                *   Must be greater than or equal to range_min. */
	bool ack;                  /*!< Whether the server shall acknowledge the message.
                                *   - true: Send Light CTL Temperature Range Set and the server replies with a Light CTL Temperature Range Status message.
                                *   - false: Send Light CTL Temperature Range Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_ctl_temperature_range_set_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_default_get_t
 * @brief     BLE MESH light ctl default get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_ctl_default_get_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_default_set_t
 * @brief     BLE MESH light ctl default set act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t lightness;        /*!< Light Lightness Default state to set.
                                *   0x0000: Use the Light Lightness Last state.
                                *   0x0001 to 0xFFFE: The perceived lightness of a light emitted by the element.
                                *   0xFFFF: The highest perceived lightness of a light emitted by the element. */
	uint16_t temperature;      /*!< Light CTL Temperature Default state to set.
                                *   Unit: kelvin.
                                *   Range: 0x0320 to 0x4E20. */
	int16_t delta_uv;          /*!< Light CTL Delta UV Default state to set.
                                *   Range: -32768 to 32767.
                                *   Represented Delta UV = value / 32768. */
	bool ack;                  /*!< Whether the server shall acknowledge the message.
                                *   - true: Send Light CTL Default Set and the server replies with a Light CTL Default Status message.
                                *   - false: Send Light CTL Default Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_ctl_default_set_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_client_status_t
 * @brief     BLE MESH light ctl client model status message structure.
 */
typedef struct {
	uint16_t src;                                            /*!< Unicast address of the server that sent the Status message. */
	uint16_t present_lightness;                              /*!< Present Light CTL Lightness state of the server.
                                                              *   0x0000: Light is not emitted by the element.
                                                              *   0x0001 to 0xFFFE: The perceived lightness of a light emitted by the element.
                                                              *   0xFFFF: The highest perceived lightness of a light emitted by the element. */
	uint16_t present_temperature;                            /*!< Present Light CTL Temperature state of the server.
                                                              *   Unit: kelvin.
                                                              *   Range: 0x0320 to 0x4E20. */
	bool optional;                                           /*!< Whether the Target CTL Lightness, Target CTL Temperature and Remaining Time are carried in the message.
                                                              *   - true: target_lightness, target_temperature and remaining_time are valid.
                                                              *     The server is changing the state or waiting for the delay to complete.
                                                              *   - false: target_lightness, target_temperature and remaining_time are not valid. */
	uint16_t target_lightness;                               /*!< Target Light CTL Lightness state the server is changing to.
                                                              *   Only valid when optional is true. */
	uint16_t target_temperature;                             /*!< Target Light CTL Temperature state the server is changing to.
                                                              *   Only valid when optional is true. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   Only valid when optional is true. */
} rtk_bt_mesh_light_ctl_client_status_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_client_status_temperature_t
 * @brief     BLE MESH light ctl client model status temperature message structure.
 */
typedef struct {
	uint16_t src;                                            /*!< Unicast address of the server that sent the Status message. */
	uint16_t present_temperature;                            /*!< Present Light CTL Temperature state of the server.
                                                              *   Unit: kelvin.
                                                              *   Range: 0x0320 to 0x4E20. */
	int16_t present_delta_uv;                                /*!< Present Light CTL Delta UV state of the server.
                                                              *   Range: -32768 to 32767.
                                                              *   Represented Delta UV = value / 32768. */
	bool optional;                                           /*!< Whether the Target CTL Temperature, Target CTL Delta UV and Remaining Time are carried in the message.
                                                              *   - true: target_temperature, target_delta_uv and remaining_time are valid.
                                                              *     The server is changing the state or waiting for the delay to complete.
                                                              *   - false: target_temperature, target_delta_uv and remaining_time are not valid. */
	uint16_t target_temperature;                             /*!< Target Light CTL Temperature state the server is changing to.
                                                              *   Only valid when optional is true. */
	int16_t target_delta_uv;                                 /*!< Target Light CTL Delta UV state the server is changing to.
                                                              *   Only valid when optional is true. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   Only valid when optional is true. */
} rtk_bt_mesh_light_ctl_client_status_temperature_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_client_status_temperature_range_t
 * @brief     BLE MESH light ctl client model status temperature range message structure.
 */
typedef struct {
	uint16_t src;                         /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_generic_stat_t status;    /*!< Status code of the last operation on the Light CTL Temperature Range state. */
	uint16_t range_min;                   /*!< Light CTL Temperature Range Min reported by the server.
                                           *   Unit: kelvin.
                                           *   0x0320 to 0x4E20: The color temperature of white light.
                                           *   0xFFFF: The color temperature of white light is unknown.
                                           *   Other values are Prohibited. */
	uint16_t range_max;                   /*!< Light CTL Temperature Range Max reported by the server.
                                           *   Unit: kelvin.
                                           *   0x0320 to 0x4E20: The color temperature of white light.
                                           *   0xFFFF: The color temperature of white light is unknown.
                                           *   Other values are Prohibited. */
} rtk_bt_mesh_light_ctl_client_status_temperature_range_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_client_status_default_t
 * @brief     BLE MESH light ctl client model status default message structure.
 */
typedef struct {
	uint16_t src;            /*!< Unicast address of the server that sent the Status message. */
	uint16_t lightness;      /*!< Light Lightness Default state of the server.
                              *   0x0000: Use the Light Lightness Last state.
                              *   0x0001 to 0xFFFE: The perceived lightness of a light emitted by the element.
                              *   0xFFFF: The highest perceived lightness of a light emitted by the element. */
	uint16_t temperature;    /*!< Light CTL Temperature Default state of the server.
                              *   Unit: kelvin.
                              *   Range: 0x0320 to 0x4E20. */
	int16_t delta_uv;        /*!< Light CTL Delta UV Default state of the server.
                              *   Range: -32768 to 32767.
                              *   Represented Delta UV = value / 32768. */
} rtk_bt_mesh_light_ctl_client_status_default_t;

/******************************** Light CTL Server Model *******************************/
/**
 * @enum      rtk_bt_mesh_light_ctl_server_model_evt_t
 * @brief     BLE MESH light ctl server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_GET = 1,                  /*!< Indicate a Light CTL Get message is received or the server needs the current CTL state, with msg
                                                                  *   @ref rtk_bt_mesh_light_ctl_server_direct_get_t.
                                                                  *   The application shall write the current Light CTL Lightness and Temperature states into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_DEFAULT_GET,              /*!< Indicate a Light CTL Default Get message received from a client, with msg
                                                                  *   @ref rtk_bt_mesh_light_ctl_server_direct_get_default_t.
                                                                  *   The application shall write the current Light CTL Default states into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_RANGE_GET,    /*!< Indicate a Light CTL Temperature Range Get message is received or the server needs the current Light
                                                                  *   CTL Temperature Range state, with msg
                                                                  *   @ref rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t.
                                                                  *   The application shall write the current Light CTL Temperature Range state into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_SET,                      /*!< Indicate the Light CTL Lightness, Temperature and Delta UV states are to change, with msg
                                                                  *   @ref rtk_bt_mesh_light_ctl_server_set_t.
                                                                  *   Raised when a Set message is received and again when the transition is updated or stopped.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_EVT_MAX,                  /*!< Reserved. */
} rtk_bt_mesh_light_ctl_server_model_evt_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_server_get_t
 * @brief     BLE MESH light ctl get evt message structure.
 */
typedef struct {
	uint16_t lightness;      /*!< Light CTL Lightness state.
                              *   0x0000: Light is not emitted by the element.
                              *   0x0001 to 0xFFFE: The perceived lightness of a light emitted by the element.
                              *   0xFFFF: The highest perceived lightness of a light emitted by the element. */
	uint16_t temperature;    /*!< Light CTL Temperature state.
                              *   Unit: kelvin.
                              *   Range: 0x0320 to 0x4E20. */
} rtk_bt_mesh_light_ctl_server_get_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_server_direct_get_t
 * @brief     BLE MESH light ctl get direct evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_ctl_server_get_t *value;    /*!< Output: the application writes the current Light CTL Lightness and Temperature states here.
                                                   *   Must not be NULL. */
} rtk_bt_mesh_light_ctl_server_direct_get_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_server_get_temperature_t
 * @brief     BLE MESH light ctl get temperature evt message structure.
 */
typedef struct {
	uint16_t temperature;    /*!< Light CTL Temperature state.
                              *   Unit: kelvin.
                              *   Range: 0x0320 to 0x4E20. */
	int16_t delta_uv;        /*!< Light CTL Delta UV state.
                              *   Range: -32768 to 32767.
                              *   Represented Delta UV = value / 32768. */
} rtk_bt_mesh_light_ctl_server_get_temperature_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_server_direct_get_temperature_t
 * @brief     BLE MESH light ctl get temperature direct evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_ctl_server_get_temperature_t *value;    /*!< Output: the application writes the current Light CTL Temperature and Delta UV states here.
                                                               *   Must not be NULL. */
} rtk_bt_mesh_light_ctl_server_direct_get_temperature_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_server_get_default_transition_time_t
 * @brief     BLE MESH light ctl get default transition time evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Generic Default Transition Time of the server. */
} rtk_bt_mesh_light_ctl_server_get_default_transition_time_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_server_get_default_t
 * @brief     BLE MESH light ctl get default evt message structure.
 */
typedef struct {
	uint16_t lightness;      /*!< Light Lightness Default state.
                              *   0x0000: Use the Light Lightness Last state.
                              *   0x0001 to 0xFFFE: The perceived lightness of a light emitted by the element.
                              *   0xFFFF: The highest perceived lightness of a light emitted by the element. */
	uint16_t temperature;    /*!< Light CTL Temperature Default state.
                              *   Unit: kelvin.
                              *   Range: 0x0320 to 0x4E20. */
	int16_t delta_uv;        /*!< Light CTL Delta UV Default state.
                              *   Range: -32768 to 32767.
                              *   Represented Delta UV = value / 32768. */
} rtk_bt_mesh_light_ctl_server_get_default_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_server_direct_get_default_t
 * @brief     BLE MESH light ctl get default direct evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_ctl_server_get_default_t *value;    /*!< Output: the application writes the current Light CTL Default states here.
                                                           *   Must not be NULL. */
} rtk_bt_mesh_light_ctl_server_direct_get_default_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_server_get_temperature_range_t
 * @brief     BLE MESH light ctl get temperature range evt message structure.
 */
typedef struct {
	uint16_t range_min;    /*!< Light CTL Temperature Range Min.
                            *   Unit: kelvin.
                            *   0x0320 to 0x4E20: The color temperature of white light.
                            *   0xFFFF: The color temperature of white light is unknown.
                            *   Other values are Prohibited. */
	uint16_t range_max;    /*!< Light CTL Temperature Range Max.
                            *   Unit: kelvin.
                            *   0x0320 to 0x4E20: The color temperature of white light.
                            *   0xFFFF: The color temperature of white light is unknown.
                            *   Other values are Prohibited. */
} rtk_bt_mesh_light_ctl_server_get_temperature_range_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t
 * @brief     BLE MESH light ctl get temperature range direct evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_ctl_server_get_temperature_range_t *value;    /*!< Output: the application writes the current Light CTL Temperature Range state here.
                                                                     *   Must not be NULL. */
} rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_server_set_t
 * @brief     BLE MESH light ctl set evt message structure.
 */
typedef struct {
	uint16_t lightness;                                      /*!< Target Light CTL Lightness state requested by the client.
                                                              *   0x0000: Light is not emitted by the element.
                                                              *   0x0001 to 0xFFFE: The perceived lightness of a light emitted by the element.
                                                              *   0xFFFF: The highest perceived lightness of a light emitted by the element. */
	uint16_t temperature;                                    /*!< Target Light CTL Temperature state requested by the client.
                                                              *   Unit: kelvin.
                                                              *   Range: 0x0320 to 0x4E20. */
	int16_t delta_uv;                                        /*!< Target Light CTL Delta UV state requested by the client.
                                                              *   Range: -32768 to 32767.
                                                              *   Represented Delta UV = value / 32768. */
	rtk_bt_mesh_generic_transition_time_t total_time;        /*!< Total time of the transition to the target state.
                                                              *   A zero value means the state changes immediately. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   A zero value means the state has reached the target. */
} rtk_bt_mesh_light_ctl_server_set_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_server_set_temperature_t
 * @brief     BLE MESH light ctl set temperature evt message structure.
 */
typedef struct {
	uint16_t temperature;                                    /*!< Target Light CTL Temperature state requested by the client.
                                                              *   Unit: kelvin.
                                                              *   Range: 0x0320 to 0x4E20. */
	int16_t delta_uv;                                        /*!< Target Light CTL Delta UV state requested by the client.
                                                              *   Range: -32768 to 32767.
                                                              *   Represented Delta UV = value / 32768. */
	rtk_bt_mesh_generic_transition_time_t total_time;        /*!< Total time of the transition to the target state.
                                                              *   A zero value means the state changes immediately. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   A zero value means the state has reached the target. */
} rtk_bt_mesh_light_ctl_server_set_temperature_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_server_set_default_t
 * @brief     BLE MESH light ctl set default evt message structure.
 */
typedef struct {
	uint16_t lightness;      /*!< Light Lightness Default state requested by the client.
                              *   0x0000: Use the Light Lightness Last state.
                              *   0x0001 to 0xFFFE: The perceived lightness of a light emitted by the element.
                              *   0xFFFF: The highest perceived lightness of a light emitted by the element.
                              *   The stack only reports this value, handle it in the application if needed. */
	uint16_t temperature;    /*!< Light CTL Temperature Default state requested by the client.
                              *   Unit: kelvin.
                              *   Range: 0x0320 to 0x4E20. */
	int16_t delta_uv;        /*!< Light CTL Delta UV Default state requested by the client.
                              *   Range: -32768 to 32767.
                              *   Represented Delta UV = value / 32768. */
} rtk_bt_mesh_light_ctl_server_set_default_t;

/**
 * @struct    rtk_bt_mesh_light_ctl_server_set_temperature_range_t
 * @brief     BLE MESH light ctl set temperature range evt message structure.
 */
typedef struct {
	uint16_t range_min;    /*!< Light CTL Temperature Range Min requested by the client.
                            *   Unit: kelvin.
                            *   0x0320 to 0x4E20: The color temperature of white light.
                            *   0xFFFF: The color temperature of white light is unknown.
                            *   Other values are Prohibited. */
	uint16_t range_max;    /*!< Light CTL Temperature Range Max requested by the client.
                            *   Unit: kelvin.
                            *   0x0320 to 0x4E20: The color temperature of white light.
                            *   0xFFFF: The color temperature of white light is unknown.
                            *   Other values are Prohibited. */
} rtk_bt_mesh_light_ctl_server_set_temperature_range_t;

/***************************** Light CTL Setup Server Model ****************************/
/**
 * @enum      rtk_bt_mesh_light_ctl_setup_server_model_evt_t
 * @brief     BLE MESH light ctl setup server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_RANGE_SET = 1,    /*!< Indicate a Light CTL Temperature Range Set or Set Unacknowledged message received from a client,
                                                                      *   with msg @ref rtk_bt_mesh_light_ctl_server_set_temperature_range_t. */
	RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_DEFAULT_SET,                  /*!< Indicate a Light CTL Default Set or Set Unacknowledged message received from a client, with msg
                                                                      *   @ref rtk_bt_mesh_light_ctl_server_set_default_t. */
	RTK_BT_MESH_LIGHT_CTL_SETUP_SERVER_MODEL_EVT_MAX,                /*!< Reserved. */
} rtk_bt_mesh_light_ctl_setup_server_model_evt_t;

/************************** Light CTL Temperature Server Model *************************/
/**
 * @enum      rtk_bt_mesh_light_ctl_temperature_server_model_evt_t
 * @brief     BLE MESH light ctl temperature server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_GET = 1,        /*!< Indicate a Light CTL Temperature Get message is received or the server needs the current Light CTL
                                                                    *   Temperature and Delta UV states, with msg
                                                                    *   @ref rtk_bt_mesh_light_ctl_server_direct_get_temperature_t.
                                                                    *   The application shall write the current Light CTL Temperature and Delta UV states into the msg.
                                                                    *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_SET,            /*!< Indicate the Light CTL Temperature and Delta UV states are to change, with msg
                                                                    *   @ref rtk_bt_mesh_light_ctl_server_set_temperature_t.
                                                                    *   Raised when a Set message is received and again when the transition is updated or stopped.
                                                                    *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_RANGE_GET_T,    /*!< Indicate the server needs the current Light CTL Temperature Range state to process a Light CTL
                                                                    *   Temperature Set or Set Unacknowledged message, with msg
                                                                    *   @ref rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t.
                                                                    *   The application shall write the current Light CTL Temperature Range state into the msg.
                                                                    *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_CTL_SERVER_TEMPERATURE_MODEL_EVT_MAX,        /*!< Reserved. */
} rtk_bt_mesh_light_ctl_temperature_server_model_evt_t;

/*********************************** Light HSL Model ***********************************/
/******************************** Light HSL Client Model *******************************/
/**
 * @enum      rtk_bt_mesh_light_hsl_client_model_act_t
 * @brief     BLE MESH light hsl client model act definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_HSL_CLIENT_MODEL_GET = 1,           /*!< Send a Light HSL Get message to the server. */
	RTK_BT_MESH_LIGHT_HSL_CLIENT_MODEL_SET,               /*!< Send a Light HSL Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_HSL_TARGET_CLIENT_MODEL_GET,        /*!< Send a Light HSL Target Get message to the server. */
	RTK_BT_MESH_LIGHT_HSL_HUE_CLIENT_MODEL_GET,           /*!< Send a Light HSL Hue Get message to the server. */
	RTK_BT_MESH_LIGHT_HSL_HUE_CLIENT_MODEL_SET,           /*!< Send a Light HSL Hue Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_HSL_SATURATION_CLIENT_MODEL_GET,    /*!< Send a Light HSL Saturation Get message to the server. */
	RTK_BT_MESH_LIGHT_HSL_SATURATION_CLIENT_MODEL_SET,    /*!< Send a Light HSL Saturation Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_HSL_DEFAULT_CLIENT_MODEL_GET,       /*!< Send a Light HSL Default Get message to the server. */
	RTK_BT_MESH_LIGHT_HSL_DEFAULT_CLIENT_MODEL_SET,       /*!< Send a Light HSL Default Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_HSL_RANGE_CLIENT_MODEL_GET,         /*!< Send a Light HSL Range Get message to the server. */
	RTK_BT_MESH_LIGHT_HSL_RANGE_CLIENT_MODEL_SET,         /*!< Send a Light HSL Range Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_HSL_CLIENT_MODEL_ACT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_light_hsl_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_light_hsl_client_model_evt_t
 * @brief     BLE MESH light hsl client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_HSL_CLIENT_MODEL_STATUS = 1,           /*!< Indicate Light HSL Status message received from the server, with msg
                                                              *   @ref rtk_bt_mesh_light_hsl_client_status_t. */
	RTK_BT_MESH_LIGHT_HSL_TARGET_CLIENT_MODEL_STATUS,        /*!< Indicate Light HSL Target Status message received from the server, with msg
                                                              *   @ref rtk_bt_mesh_light_hsl_client_status_t. */
	RTK_BT_MESH_LIGHT_HSL_HUE_CLIENT_MODEL_STATUS,           /*!< Indicate Light HSL Hue Status message received from the server, with msg
                                                              *   @ref rtk_bt_mesh_light_hsl_client_status_hue_t. */
	RTK_BT_MESH_LIGHT_HSL_SATURATION_CLIENT_MODEL_STATUS,    /*!< Indicate Light HSL Saturation Status message received from the server, with msg
                                                              *   @ref rtk_bt_mesh_light_hsl_client_status_saturation_t. */
	RTK_BT_MESH_LIGHT_HSL_DEFAULT_CLIENT_MODEL_STATUS,       /*!< Indicate Light HSL Default Status message received from the server, with msg
                                                              *   @ref rtk_bt_mesh_light_hsl_client_status_default_t. */
	RTK_BT_MESH_LIGHT_HSL_RANGE_CLIENT_MODEL_STATUS,         /*!< Indicate Light HSL Range Status message received from the server, with msg
                                                              *   @ref rtk_bt_mesh_light_hsl_client_status_range_t. */
	RTK_BT_MESH_LIGHT_HSL_CLIENT_MODEL_EVT_MAX,              /*!< Reserved. */
} rtk_bt_mesh_light_hsl_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_get_t
 * @brief     BLE MESH light hsl get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_hsl_get_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_set_t
 * @brief     BLE MESH light hsl set act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	uint16_t lightness;                                  /*!< Light HSL Lightness state to set.
                                                          *   0x0000: Light is not emitted by the element.
                                                          *   0x0001 to 0xFFFE: The perceived lightness of a light emitted by the element.
                                                          *   0xFFFF: The highest perceived lightness of a light emitted by the element. */
	uint16_t hue;                                        /*!< Light HSL Hue state to set.
                                                          *   Range: 0x0000 to 0xFFFF.
                                                          *   Hue in degrees = 360 * value / 65536. */
	uint16_t saturation;                                 /*!< Light HSL Saturation state to set.
                                                          *   0x0000: The lowest perceived saturation of a color light.
                                                          *   0x0001 to 0xFFFE: The saturation of a color light.
                                                          *   0xFFFF: The highest perceived saturation of a color light. */
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
                                                          *   - true: Send Light HSL Set and the server replies with a Light HSL Status message.
                                                          *   - false: Send Light HSL Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_hsl_set_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_target_get_t
 * @brief     BLE MESH light hsl target get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_hsl_target_get_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_hue_get_t
 * @brief     BLE MESH light hsl hue get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_hsl_hue_get_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_hue_set_t
 * @brief     BLE MESH light hsl hue set act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	uint16_t hue;                                        /*!< Light HSL Hue state to set.
                                                          *   Range: 0x0000 to 0xFFFF.
                                                          *   Hue in degrees = 360 * value / 65536. */
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
                                                          *   - true: Send Light HSL Hue Set and the server replies with a Light HSL Hue Status message.
                                                          *   - false: Send Light HSL Hue Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_hsl_hue_set_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_saturation_get_t
 * @brief     BLE MESH light hsl saturation get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_hsl_saturation_get_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_saturation_set_t
 * @brief     BLE MESH light hsl saturation set act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	uint16_t saturation;                                 /*!< Light HSL Saturation state to set.
                                                          *   0x0000: The lowest perceived saturation of a color light.
                                                          *   0x0001 to 0xFFFE: The saturation of a color light.
                                                          *   0xFFFF: The highest perceived saturation of a color light. */
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
                                                          *   - true: Send Light HSL Saturation Set and the server replies with a Light HSL Saturation Status message.
                                                          *   - false: Send Light HSL Saturation Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_hsl_saturation_set_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_default_get_t
 * @brief     BLE MESH light hsl default get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_hsl_default_get_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_default_set_t
 * @brief     BLE MESH light hsl default set act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t lightness;        /*!< Light Lightness Default state to set.
                                *   0x0000: Use the Light Lightness Last state as the default value.
                                *   0x0001 to 0xFFFE: The perceived lightness of a light emitted by the element.
                                *   0xFFFF: The highest perceived lightness of a light emitted by the element. */
	uint16_t hue;              /*!< Light HSL Hue Default state to set.
                                *   Range: 0x0000 to 0xFFFF.
                                *   Hue in degrees = 360 * value / 65536. */
	uint16_t saturation;       /*!< Light HSL Saturation Default state to set.
                                *   Range: 0x0000 to 0xFFFF. */
	bool ack;                  /*!< Whether the server shall acknowledge the message.
                                *   - true: Send Light HSL Default Set and the server replies with a Light HSL Default Status message.
                                *   - false: Send Light HSL Default Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_hsl_default_set_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_range_get_t
 * @brief     BLE MESH light hsl range get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_hsl_range_get_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_range_set_t
 * @brief     BLE MESH light hsl range set act message structure.
 */
typedef struct {
	uint16_t dst;                     /*!< Destination address of the message. */
	uint16_t app_key_index;           /*!< Index of the application key used to encrypt the message.
                                       *   Range: 0x0000 to 0x0FFF. */
	uint16_t hue_range_min;           /*!< Light HSL Hue Range Min to set.
                                       *   Range: 0x0000 to 0xFFFF. */
	uint16_t hue_range_max;           /*!< Light HSL Hue Range Max to set.
                                       *   Range: 0x0000 to 0xFFFF. */
	uint16_t saturation_range_min;    /*!< Light HSL Saturation Range Min to set.
                                       *   Range: 0x0000 to 0xFFFF. */
	uint16_t saturation_range_max;    /*!< Light HSL Saturation Range Max to set.
                                       *   Range: 0x0000 to 0xFFFF.
                                       *   Must be greater than or equal to saturation_range_min. */
	bool ack;                         /*!< Whether the server shall acknowledge the message.
                                       *   - true: Send Light HSL Range Set and the server replies with a Light HSL Range Status message.
                                       *   - false: Send Light HSL Range Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_hsl_range_set_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_client_status_t
 * @brief     BLE MESH light hsl client model status message structure.
 */
typedef struct {
	uint16_t src;                                            /*!< Unicast address of the server that sent the Status message. */
	uint16_t lightness;                                      /*!< Light HSL Lightness state reported by the server.
                                                              *   The present state in a Light HSL Status message, the target state in a Light HSL Target Status message.
                                                              *   0x0000: Light is not emitted by the element.
                                                              *   0x0001 to 0xFFFE: The perceived lightness of a light emitted by the element.
                                                              *   0xFFFF: The highest perceived lightness of a light emitted by the element. */
	uint16_t hue;                                            /*!< Light HSL Hue state reported by the server.
                                                              *   The present state in a Light HSL Status message, the target state in a Light HSL Target Status message.
                                                              *   Range: 0x0000 to 0xFFFF.
                                                              *   Hue in degrees = 360 * value / 65536. */
	uint16_t saturation;                                     /*!< Light HSL Saturation state reported by the server.
                                                              *   The present state in a Light HSL Status message, the target state in a Light HSL Target Status message.
                                                              *   0x0000: The lowest perceived saturation of a color light.
                                                              *   0x0001 to 0xFFFE: The saturation of a color light.
                                                              *   0xFFFF: The highest perceived saturation of a color light. */
	bool optional;                                           /*!< Whether the Remaining Time field is carried in the message.
                                                              *   - true: remaining_time is valid.
                                                              *   - false: remaining_time is not valid. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   Only valid when optional is true. */
} rtk_bt_mesh_light_hsl_client_status_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_client_status_hue_t
 * @brief     BLE MESH light hsl hue client model status message structure.
 */
typedef struct {
	uint16_t src;                                            /*!< Unicast address of the server that sent the Status message. */
	uint16_t present_hue;                                    /*!< Present Light HSL Hue state of the server.
                                                              *   Range: 0x0000 to 0xFFFF.
                                                              *   Hue in degrees = 360 * value / 65536. */
	bool optional;                                           /*!< Whether the Target Hue and Remaining Time fields are carried in the message.
                                                              *   - true: target_hue and remaining_time are valid.
                                                              *     The server is changing the state or waiting for the delay to complete.
                                                              *   - false: target_hue and remaining_time are not valid. */
	uint16_t target_hue;                                     /*!< Target Light HSL Hue state the server is changing to.
                                                              *   Range: 0x0000 to 0xFFFF.
                                                              *   Hue in degrees = 360 * value / 65536.
                                                              *   Only valid when optional is true. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   Only valid when optional is true. */
} rtk_bt_mesh_light_hsl_client_status_hue_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_client_status_saturation_t
 * @brief     BLE MESH light hsl saturation client model status message structure.
 */
typedef struct {
	uint16_t src;                                            /*!< Unicast address of the server that sent the Status message. */
	uint16_t present_saturation;                             /*!< Present Light HSL Saturation state of the server.
                                                              *   0x0000: The lowest perceived saturation of a color light.
                                                              *   0x0001 to 0xFFFE: The saturation of a color light.
                                                              *   0xFFFF: The highest perceived saturation of a color light. */
	bool optional;                                           /*!< Whether the Target Saturation and Remaining Time fields are carried in the message.
                                                              *   - true: target_saturation and remaining_time are valid.
                                                              *     The server is changing the state or waiting for the delay to complete.
                                                              *   - false: target_saturation and remaining_time are not valid. */
	uint16_t target_saturation;                              /*!< Target Light HSL Saturation state the server is changing to.
                                                              *   0x0000: The lowest perceived saturation of a color light.
                                                              *   0x0001 to 0xFFFE: The saturation of a color light.
                                                              *   0xFFFF: The highest perceived saturation of a color light.
                                                              *   Only valid when optional is true. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   Only valid when optional is true. */
} rtk_bt_mesh_light_hsl_client_status_saturation_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_client_status_default_t
 * @brief     BLE MESH light hsl client model default status message structure.
 */
typedef struct {
	uint16_t src;           /*!< Unicast address of the server that sent the Status message. */
	uint16_t lightness;     /*!< Light Lightness Default state reported by the server.
                             *   0x0000: Use the Light Lightness Last state as the default value.
                             *   0x0001 to 0xFFFE: The perceived lightness of a light emitted by the element.
                             *   0xFFFF: The highest perceived lightness of a light emitted by the element. */
	uint16_t hue;           /*!< Light HSL Hue Default state reported by the server.
                             *   Range: 0x0000 to 0xFFFF.
                             *   Hue in degrees = 360 * value / 65536. */
	uint16_t saturation;    /*!< Light HSL Saturation Default state reported by the server.
                             *   Range: 0x0000 to 0xFFFF. */
} rtk_bt_mesh_light_hsl_client_status_default_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_client_status_range_t
 * @brief     BLE MESH light hsl client model range status message structure.
 */
typedef struct {
	uint16_t src;                         /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_generic_stat_t status;    /*!< Status code of the last operation on the Light HSL Hue Range and Saturation Range states. */
	uint16_t hue_range_min;               /*!< Light HSL Hue Range Min reported by the server.
                                           *   Range: 0x0000 to 0xFFFF. */
	uint16_t hue_range_max;               /*!< Light HSL Hue Range Max reported by the server.
                                           *   Range: 0x0000 to 0xFFFF. */
	uint16_t saturation_range_min;        /*!< Light HSL Saturation Range Min reported by the server.
                                           *   Range: 0x0000 to 0xFFFF. */
	uint16_t saturation_range_max;        /*!< Light HSL Saturation Range Max reported by the server.
                                           *   Range: 0x0000 to 0xFFFF. */
} rtk_bt_mesh_light_hsl_client_status_range_t;

/******************************** Light HSL Server Model *******************************/
/**
 * @enum      rtk_bt_mesh_light_hsl_server_model_evt_t
 * @brief     BLE MESH light hsl server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_HSL_SERVER_MODEL_GET = 1,        /*!< Indicate a Light HSL Get message is received or the server needs the current Light HSL states, with msg
                                                        *   @ref rtk_bt_mesh_light_hsl_server_direct_get_t.
                                                        *   The application shall write the current Light HSL Lightness, Hue and Saturation states into the msg.
                                                        *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_HSL_DEFAULT_SERVER_MODEL_GET,    /*!< Indicate a Light HSL Default Get message received from a client, with msg
                                                        *   @ref rtk_bt_mesh_light_hsl_server_direct_get_default_t.
                                                        *   The application shall write the current Light HSL Default states into the msg.
                                                        *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_HSL_RANGE_SERVER_MODEL_GET,      /*!< Indicate a Light HSL Range Get message is received or the server needs the current Light HSL Range states,
                                                        *   with msg @ref rtk_bt_mesh_light_hsl_server_direct_get_range_t.
                                                        *   The application shall write the current Light HSL Hue Range and Saturation Range states into the msg.
                                                        *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_HSL_SERVER_MODEL_SET,            /*!< Indicate the Light HSL Lightness, Hue and Saturation states are to change, with msg
                                                        *   @ref rtk_bt_mesh_light_hsl_server_set_t.
                                                        *   Raised when a Set or Set Unacknowledged message is received.
                                                        *   Raised again when the transition is updated or stopped.
                                                        *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_HSL_SERVER_MODEL_EVT_MAX,        /*!< Reserved. */
} rtk_bt_mesh_light_hsl_server_model_evt_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_get_t
 * @brief     BLE MESH light hsl get evt message structure.
 */
typedef struct {
	uint16_t lightness;     /*!< Light HSL Lightness state.
                             *   0x0000: Light is not emitted by the element.
                             *   0x0001 to 0xFFFE: The perceived lightness of a light emitted by the element.
                             *   0xFFFF: The highest perceived lightness of a light emitted by the element. */
	uint16_t hue;           /*!< Light HSL Hue state.
                             *   Range: 0x0000 to 0xFFFF.
                             *   Hue in degrees = 360 * value / 65536. */
	uint16_t saturation;    /*!< Light HSL Saturation state.
                             *   0x0000: The lowest perceived saturation of a color light.
                             *   0x0001 to 0xFFFE: The saturation of a color light.
                             *   0xFFFF: The highest perceived saturation of a color light. */
} rtk_bt_mesh_light_hsl_server_get_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_direct_get_t
 * @brief     BLE MESH light hsl get direct evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_hsl_server_get_t *value;    /*!< Output: the application writes the current Light HSL Lightness, Hue and Saturation states here.
                                                   *   Must not be NULL. */
} rtk_bt_mesh_light_hsl_server_direct_get_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_get_hue_t
 * @brief     BLE MESH light hsl get hue evt message structure.
 */
typedef struct {
	uint16_t hue;    /*!< Light HSL Hue state.
                      *   Range: 0x0000 to 0xFFFF.
                      *   Hue in degrees = 360 * value / 65536. */
} rtk_bt_mesh_light_hsl_server_get_hue_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_direct_get_hue_t
 * @brief     BLE MESH light hsl get hue direct evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_hsl_server_get_hue_t *value;    /*!< Output: the application writes the current Light HSL Hue state here.
                                                       *   Must not be NULL. */
} rtk_bt_mesh_light_hsl_server_direct_get_hue_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_get_saturation_t
 * @brief     BLE MESH light hsl get saturation evt message structure.
 */
typedef struct {
	uint16_t saturation;    /*!< Light HSL Saturation state.
                             *   0x0000: The lowest perceived saturation of a color light.
                             *   0x0001 to 0xFFFE: The saturation of a color light.
                             *   0xFFFF: The highest perceived saturation of a color light. */
} rtk_bt_mesh_light_hsl_server_get_saturation_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_direct_get_saturation_t
 * @brief     BLE MESH light hsl get saturation direct evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_hsl_server_get_saturation_t *value;    /*!< Output: the application writes the current Light HSL Saturation state here.
                                                              *   Must not be NULL. */
} rtk_bt_mesh_light_hsl_server_direct_get_saturation_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_get_default_t
 * @brief     BLE MESH light hsl get default evt message structure.
 */
typedef struct {
	uint16_t lightness;     /*!< Light Lightness Default state.
                             *   0x0000: Use the Light Lightness Last state as the default value.
                             *   0x0001 to 0xFFFE: The perceived lightness of a light emitted by the element.
                             *   0xFFFF: The highest perceived lightness of a light emitted by the element. */
	uint16_t hue;           /*!< Light HSL Hue Default state.
                             *   Range: 0x0000 to 0xFFFF.
                             *   Hue in degrees = 360 * value / 65536. */
	uint16_t saturation;    /*!< Light HSL Saturation Default state.
                             *   Range: 0x0000 to 0xFFFF. */
} rtk_bt_mesh_light_hsl_server_get_default_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_direct_get_default_t
 * @brief     BLE MESH light hsl get default direct evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_hsl_server_get_default_t *value;    /*!< Output: the application writes the current Light HSL Default states here.
                                                           *   Must not be NULL. */
} rtk_bt_mesh_light_hsl_server_direct_get_default_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_get_range_t
 * @brief     BLE MESH light hsl get range evt message structure.
 */
typedef struct {
	uint16_t hue_range_min;           /*!< Light HSL Hue Range Min state.
                                       *   Range: 0x0000 to 0xFFFF. */
	uint16_t hue_range_max;           /*!< Light HSL Hue Range Max state.
                                       *   Range: 0x0000 to 0xFFFF. */
	uint16_t saturation_range_min;    /*!< Light HSL Saturation Range Min state.
                                       *   Range: 0x0000 to 0xFFFF. */
	uint16_t saturation_range_max;    /*!< Light HSL Saturation Range Max state.
                                       *   Range: 0x0000 to 0xFFFF. */
} rtk_bt_mesh_light_hsl_server_get_range_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_direct_get_range_t
 * @brief     BLE MESH light hsl get range direct evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_hsl_server_get_range_t *value;    /*!< Output: the application writes the current Light HSL Hue Range and Saturation Range states here.
                                                         *   Must not be NULL. */
} rtk_bt_mesh_light_hsl_server_direct_get_range_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_get_default_transition_time_t
 * @brief     BLE MESH light hsl get default transition time evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Generic Default Transition Time of the server. */
} rtk_bt_mesh_light_hsl_server_get_default_transition_time_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_set_t
 * @brief     BLE MESH light hsl set evt message structure.
 */
typedef struct {
	uint16_t lightness;                                      /*!< Target Light HSL Lightness state requested by the client.
                                                              *   0x0000: Light is not emitted by the element.
                                                              *   0x0001 to 0xFFFE: The perceived lightness of a light emitted by the element.
                                                              *   0xFFFF: The highest perceived lightness of a light emitted by the element. */
	uint16_t hue;                                            /*!< Target Light HSL Hue state requested by the client.
                                                              *   Range: 0x0000 to 0xFFFF.
                                                              *   Hue in degrees = 360 * value / 65536. */
	uint16_t saturation;                                     /*!< Target Light HSL Saturation state requested by the client.
                                                              *   0x0000: The lowest perceived saturation of a color light.
                                                              *   0x0001 to 0xFFFE: The saturation of a color light.
                                                              *   0xFFFF: The highest perceived saturation of a color light. */
	rtk_bt_mesh_generic_transition_time_t total_time;        /*!< Total time of the transition to the target state.
                                                              *   A zero value means the state changes immediately. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   A zero value means the state has reached the target. */
} rtk_bt_mesh_light_hsl_server_set_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_set_hue_t
 * @brief     BLE MESH light hsl set hue evt message structure.
 */
typedef struct {
	uint16_t hue;                                            /*!< Target Light HSL Hue state requested by the client.
                                                              *   Range: 0x0000 to 0xFFFF.
                                                              *   Hue in degrees = 360 * value / 65536. */
	rtk_bt_mesh_generic_transition_time_t total_time;        /*!< Total time of the transition to the target state.
                                                              *   A zero value means the state changes immediately. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   A zero value means the state has reached the target. */
} rtk_bt_mesh_light_hsl_server_set_hue_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_set_saturation_t
 * @brief     BLE MESH light hsl set saturation evt message structure.
 */
typedef struct {
	uint16_t saturation;                                     /*!< Target Light HSL Saturation state requested by the client.
                                                              *   0x0000: The lowest perceived saturation of a color light.
                                                              *   0x0001 to 0xFFFE: The saturation of a color light.
                                                              *   0xFFFF: The highest perceived saturation of a color light. */
	rtk_bt_mesh_generic_transition_time_t total_time;        /*!< Total time of the transition to the target state.
                                                              *   A zero value means the state changes immediately. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   A zero value means the state has reached the target. */
} rtk_bt_mesh_light_hsl_server_set_saturation_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_set_default_t
 * @brief     BLE MESH light hsl set default evt message structure.
 */
typedef struct {
	uint16_t lightness;     /*!< Light Lightness Default state requested by the client.
                             *   0x0000: Use the Light Lightness Last state as the default value.
                             *   The stack only reports this value, handle it in the application if needed. */
	uint16_t hue;           /*!< Light HSL Hue Default state requested by the client.
                             *   Range: 0x0000 to 0xFFFF.
                             *   Hue in degrees = 360 * value / 65536. */
	uint16_t saturation;    /*!< Light HSL Saturation Default state requested by the client.
                             *   Range: 0x0000 to 0xFFFF. */
} rtk_bt_mesh_light_hsl_server_set_default_t;

/**
 * @struct    rtk_bt_mesh_light_hsl_server_set_range_t
 * @brief     BLE MESH light hsl set range evt message structure.
 */
typedef struct {
	uint16_t hue_range_min;           /*!< Light HSL Hue Range Min state requested by the client.
                                       *   Range: 0x0000 to 0xFFFF. */
	uint16_t hue_range_max;           /*!< Light HSL Hue Range Max state requested by the client.
                                       *   Range: 0x0000 to 0xFFFF. */
	uint16_t saturation_range_min;    /*!< Light HSL Saturation Range Min state requested by the client.
                                       *   Range: 0x0000 to 0xFFFF. */
	uint16_t saturation_range_max;    /*!< Light HSL Saturation Range Max state requested by the client.
                                       *   Range: 0x0000 to 0xFFFF. */
} rtk_bt_mesh_light_hsl_server_set_range_t;

/****************************** Light HSL Hue Server Model *****************************/
/**
 * @enum      rtk_bt_mesh_light_hsl_hue_server_model_evt_t
 * @brief     BLE MESH light hsl hue server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_HSL_HUE_SERVER_MODEL_GET = 1,    /*!< Indicate a Light HSL Hue Get message is received or the server needs the current Light HSL Hue state, with msg
                                                        *   @ref rtk_bt_mesh_light_hsl_server_direct_get_hue_t.
                                                        *   The application shall write the current Light HSL Hue state into the msg.
                                                        *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_HSL_HUE_SERVER_MODEL_SET,        /*!< Indicate the Light HSL Hue state is to change, with msg @ref rtk_bt_mesh_light_hsl_server_set_hue_t.
                                                        *   Raised when a Set or Set Unacknowledged message is received.
                                                        *   Raised again when the transition is updated or stopped.
                                                        *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_HSL_RANGE_SERVER_MODEL_GET_H,    /*!< Indicate the server needs the current Light HSL Hue Range state to process a Light HSL Hue Set or Set
                                                        *   Unacknowledged message, with msg @ref rtk_bt_mesh_light_hsl_server_direct_get_range_t.
                                                        *   The application shall write the current Light HSL Hue Range state into the msg.
                                                        *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_HSL_HUE_SERVER_MODEL_EVT_MAX,    /*!< Reserved. */
} rtk_bt_mesh_light_hsl_hue_server_model_evt_t;

/************************** Light HSL Saturation Server Model **************************/
/**
 * @enum      rtk_bt_mesh_light_hsl_saturation_server_model_evt_t
 * @brief     BLE MESH light hsl saturation server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL_GET = 1,    /*!< Indicate a Light HSL Saturation Get message is received or the server needs the current Light HSL
                                                               *   Saturation state, with msg @ref rtk_bt_mesh_light_hsl_server_direct_get_saturation_t.
                                                               *   The application shall write the current Light HSL Saturation state into the msg.
                                                               *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL_SET,        /*!< Indicate the Light HSL Saturation state is to change, with msg
                                                               *   @ref rtk_bt_mesh_light_hsl_server_set_saturation_t.
                                                               *   Raised when a Set or Set Unacknowledged message is received.
                                                               *   Raised again when the transition is updated or stopped.
                                                               *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_HSL_RANGE_SERVER_MODEL_GET_S,           /*!< Indicate the server needs the current Light HSL Saturation Range state to process a Light HSL
                                                               *   Saturation Set or Set Unacknowledged message, with msg
                                                               *   @ref rtk_bt_mesh_light_hsl_server_direct_get_range_t.
                                                               *   The application shall write the current Light HSL Saturation Range state into the msg.
                                                               *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL_EVT_MAX,    /*!< Reserved. */
} rtk_bt_mesh_light_hsl_saturation_server_model_evt_t;

/***************************** Light HSL Setup Server Model ****************************/
/**
 * @enum      rtk_bt_mesh_light_hsl_setup_server_model_evt_t
 * @brief     BLE MESH light hsl setup server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_HSL_DEFAULT_SERVER_MODEL_SET = 1,    /*!< Indicate a Light HSL Default Set or Set Unacknowledged message received from a client, with msg
                                                            *   @ref rtk_bt_mesh_light_hsl_server_set_default_t. */
	RTK_BT_MESH_LIGHT_HSL_RANGE_SERVER_MODEL_SET,          /*!< Indicate a Light HSL Range Set or Set Unacknowledged message received from a client, with msg
                                                            *   @ref rtk_bt_mesh_light_hsl_server_set_range_t. */
	RTK_BT_MESH_LIGHT_HSL_SETUP_SERVER_MODEL_EVT_MAX,      /*!< Reserved. */
} rtk_bt_mesh_light_hsl_setup_server_model_evt_t;

/*********************************** Light xyL Model ***********************************/
/******************************** Light xyL Client Model *******************************/
/**
 * @enum      rtk_bt_mesh_light_xyl_client_model_act_t
 * @brief     BLE MESH light xyl client model act definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_XYL_CLIENT_MODEL_GET = 1,        /*!< Send a Light xyL Get message to the server. */
	RTK_BT_MESH_LIGHT_XYL_CLIENT_MODEL_SET,            /*!< Send a Light xyL Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_XYL_TARGET_CLIENT_MODEL_GET,     /*!< Send a Light xyL Target Get message to the server. */
	RTK_BT_MESH_LIGHT_XYL_DEFAULT_CLIENT_MODEL_GET,    /*!< Send a Light xyL Default Get message to the server. */
	RTK_BT_MESH_LIGHT_XYL_DEFAULT_CLIENT_MODEL_SET,    /*!< Send a Light xyL Default Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_XYL_RANGE_CLIENT_MODEL_GET,      /*!< Send a Light xyL Range Get message to the server. */
	RTK_BT_MESH_LIGHT_XYL_RANGE_CLIENT_MODEL_SET,      /*!< Send a Light xyL Range Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_XYL_CLIENT_MODEL_ACT_MAX,        /*!< Reserved. */
} rtk_bt_mesh_light_xyl_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_light_xyl_client_model_evt_t
 * @brief     BLE MESH light xyl client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_XYL_CLIENT_MODEL_STATUS = 1,        /*!< Indicate Light xyL Status message received from the server, with msg
                                                           *   @ref rtk_bt_mesh_light_xyl_client_status_t. */
	RTK_BT_MESH_LIGHT_XYL_TARGET_CLIENT_MODEL_STATUS,     /*!< Indicate Light xyL Target Status message received from the server, with msg
                                                           *   @ref rtk_bt_mesh_light_xyl_client_status_t.
                                                           *   The xyl member holds the target values. */
	RTK_BT_MESH_LIGHT_XYL_DEFAULT_CLIENT_MODEL_STATUS,    /*!< Indicate Light xyL Default Status message received from the server, with msg
                                                           *   @ref rtk_bt_mesh_light_xyl_client_status_default_t. */
	RTK_BT_MESH_LIGHT_XYL_RANGE_CLIENT_MODEL_STATUS,      /*!< Indicate Light xyL Range Status message received from the server, with msg
                                                           *   @ref rtk_bt_mesh_light_xyl_client_status_range_t. */
	RTK_BT_MESH_LIGHT_XYL_CLIENT_MODEL_EVT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_light_xyl_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_t
 * @brief     BLE MESH light xyl lightness, x and y state structure.
 */
typedef struct {
	uint16_t xyl_lightness;    /*!< Light xyL Lightness.
                                *   0x0000: Light is not emitted by the element.
                                *   0x0001 to 0xFFFE: Perceived lightness of the light emitted by the element.
                                *   0xFFFF: Highest perceived lightness of the light emitted by the element. */
	uint16_t xyl_x;            /*!< Light xyL x, the x coordinate on the CIE 1931 color space chart is xyl_x / 65535.
                                *   Range: 0x0000 to 0xFFFF. */
	uint16_t xyl_y;            /*!< Light xyL y, the y coordinate on the CIE 1931 color space chart is xyl_y / 65535.
                                *   Range: 0x0000 to 0xFFFF. */
} _PACKED4_ rtk_bt_mesh_light_xyl_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_get_t
 * @brief     BLE MESH light xyl get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_xyl_get_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_set_t
 * @brief     BLE MESH light xyl set act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_light_xyl_t xyl;                         /*!< Target Light xyL Lightness, x and y state. */
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
                                                          *   - true: Send Light xyL Set and the server replies with a Light xyL Status message.
                                                          *   - false: Send Light xyL Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_xyl_set_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_target_get_t
 * @brief     BLE MESH light xyl target get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_xyl_target_get_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_default_get_t
 * @brief     BLE MESH light xyl default get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_xyl_default_get_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_default_set_t
 * @brief     BLE MESH light xyl default set act message structure.
 */
typedef struct {
	uint16_t dst;                   /*!< Destination address of the message. */
	uint16_t app_key_index;         /*!< Index of the application key used to encrypt the message.
                                     *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_light_xyl_t xyl;    /*!< Light Lightness Default, Light xyL x Default and Light xyL y Default values to set. */
	bool ack;                       /*!< Whether the server shall acknowledge the message.
                                     *   - true: Send Light xyL Default Set and the server replies with a Light xyL Default Status message.
                                     *   - false: Send Light xyL Default Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_xyl_default_set_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_range_get_t
 * @brief     BLE MESH light xyl range get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_xyl_range_get_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_range_set_t
 * @brief     BLE MESH light xyl range set act message structure.
 */
typedef struct {
	uint16_t dst;                /*!< Destination address of the message. */
	uint16_t app_key_index;      /*!< Index of the application key used to encrypt the message.
                                  *   Range: 0x0000 to 0x0FFF. */
	uint16_t xyl_x_range_min;    /*!< Light xyL x Range Min to set.
                                  *   Range: 0x0000 to 0xFFFF. */
	uint16_t xyl_x_range_max;    /*!< Light xyL x Range Max to set.
                                  *   Range: 0x0000 to 0xFFFF, not less than xyl_x_range_min. */
	uint16_t xyl_y_range_min;    /*!< Light xyL y Range Min to set.
                                  *   Range: 0x0000 to 0xFFFF. */
	uint16_t xyl_y_range_max;    /*!< Light xyL y Range Max to set.
                                  *   Range: 0x0000 to 0xFFFF, not less than xyl_y_range_min. */
	bool ack;                    /*!< Whether the server shall acknowledge the message.
                                  *   - true: Send Light xyL Range Set and the server replies with a Light xyL Range Status message.
                                  *   - false: Send Light xyL Range Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_xyl_range_set_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_client_status_t
 * @brief     BLE MESH light xyl client model status message structure.
 */
typedef struct {
	uint16_t src;                                            /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_light_xyl_t xyl;                             /*!< Light xyL Lightness, x and y state reported by the server.
                                                              *   Present state in Status, target state in Target Status. */
	bool optional;                                           /*!< Whether the Remaining Time field is carried in the message.
                                                              *   - true: remaining_time is valid.
                                                              *   - false: remaining_time is not valid. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   Only valid when optional is true. */
} rtk_bt_mesh_light_xyl_client_status_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_client_status_default_t
 * @brief     BLE MESH light xyl client model default status message structure.
 */
typedef struct {
	uint16_t src;                   /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_light_xyl_t xyl;    /*!< Light Lightness Default, Light xyL x Default and Light xyL y Default values of the server. */
} rtk_bt_mesh_light_xyl_client_status_default_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_client_status_range_t
 * @brief     BLE MESH light xyl client model range status message structure.
 */
typedef struct {
	uint16_t src;                         /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_generic_stat_t status;    /*!< Status code of the last operation on the Light xyL x Range and Light xyL y Range states. */
	uint16_t xyl_x_range_min;             /*!< Light xyL x Range Min reported by the server. */
	uint16_t xyl_x_range_max;             /*!< Light xyL x Range Max reported by the server. */
	uint16_t xyl_y_range_min;             /*!< Light xyL y Range Min reported by the server. */
	uint16_t xyl_y_range_max;             /*!< Light xyL y Range Max reported by the server. */
} rtk_bt_mesh_light_xyl_client_status_range_t;

/******************************** Light xyL Server Model *******************************/
/**
 * @enum      rtk_bt_mesh_light_xyl_server_model_evt_t
 * @brief     BLE MESH light xyl server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_XYL_SERVER_MODEL_GET = 1,        /*!< Indicate a Light xyL Get message is received or the server needs the current Light xyL state, with msg
                                                        *   @ref rtk_bt_mesh_light_xyl_server_direct_get_t.
                                                        *   The application shall write the current Light xyL Lightness, x and y state into the msg.
                                                        *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_XYL_SERVER_MODEL_SET,            /*!< Indicate the Light xyL state is to change, with msg @ref rtk_bt_mesh_light_xyl_server_set_t.
                                                        *   Raised when a Set or Set Unacknowledged message is received,
                                                        *   and again when the transition is updated or stopped.
                                                        *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_XYL_DEFAULT_SERVER_MODEL_GET,    /*!< Indicate Light xyL Default Get message received from a client, with msg
                                                        *   @ref rtk_bt_mesh_light_xyl_server_direct_get_default_t.
                                                        *   The application shall write the current Light xyL Default state into the msg.
                                                        *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_XYL_RANGE_SERVER_MODEL_GET,      /*!< Indicate a Light xyL Range Get message is received or the server needs the current Light xyL Range state,
                                                        *   with msg @ref rtk_bt_mesh_light_xyl_server_direct_get_range_t.
                                                        *   The application shall write the current Light xyL x Range and y Range state into the msg.
                                                        *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_XYL_SERVER_MODEL_EVT_MAX,        /*!< Reserved. */
} rtk_bt_mesh_light_xyl_server_model_evt_t;

/**
 * @typedef   rtk_bt_mesh_light_xyl_server_get_t
 * @brief     BLE MESH light xyl get evt message structure.
 */
typedef rtk_bt_mesh_light_xyl_t rtk_bt_mesh_light_xyl_server_get_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_server_direct_get_t
 * @brief     BLE MESH light xyl get direct evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_xyl_server_get_t *value;    /*!< Output: the application writes the current Light xyL state here.
                                                   *   Must not be NULL. */
} rtk_bt_mesh_light_xyl_server_direct_get_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_server_get_range_t
 * @brief     BLE MESH light xyl get range evt message structure.
 */
typedef struct {
	uint16_t xyl_x_range_min;    /*!< Light xyL x Range Min. */
	uint16_t xyl_x_range_max;    /*!< Light xyL x Range Max. */
	uint16_t xyl_y_range_min;    /*!< Light xyL y Range Min. */
	uint16_t xyl_y_range_max;    /*!< Light xyL y Range Max. */
} rtk_bt_mesh_light_xyl_server_get_range_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_server_direct_get_range_t
 * @brief     BLE MESH light xyl get range direct evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_xyl_server_get_range_t *value;    /*!< Output: the application writes the current Light xyL x Range and y Range here.
                                                         *   Must not be NULL. */
} rtk_bt_mesh_light_xyl_server_direct_get_range_t;

/**
 * @typedef   rtk_bt_mesh_light_xyl_server_get_default_t
 * @brief     BLE MESH light xyl get default evt message structure.
 */
typedef rtk_bt_mesh_light_xyl_t rtk_bt_mesh_light_xyl_server_get_default_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_server_direct_get_default_t
 * @brief     BLE MESH light xyl get default direct evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_xyl_server_get_default_t *value;    /*!< Output: the application writes the current Light xyL Default state here.
                                                           *   Must not be NULL. */
} rtk_bt_mesh_light_xyl_server_direct_get_default_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_server_get_default_transition_time_t
 * @brief     BLE MESH light xyl get default transition time evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Generic Default Transition Time of the server. */
} rtk_bt_mesh_light_xyl_server_get_default_transition_time_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_server_set_t
 * @brief     BLE MESH light xyl set evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_xyl_t xyl;                             /*!< Target Light xyL Lightness, x and y state requested by the client. */
	rtk_bt_mesh_generic_transition_time_t total_time;        /*!< Total time of the transition to the target state.
                                                              *   A zero value means the state changes immediately. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   A zero value means the state has reached the target. */
} rtk_bt_mesh_light_xyl_server_set_t;

/**
 * @typedef   rtk_bt_mesh_light_xyl_server_set_default_t
 * @brief     BLE MESH light xyl set default evt message structure.
 */
typedef rtk_bt_mesh_light_xyl_t rtk_bt_mesh_light_xyl_server_set_default_t;

/**
 * @struct    rtk_bt_mesh_light_xyl_server_set_range_t
 * @brief     BLE MESH light xyl set range evt message structure.
 */
typedef struct {
	uint16_t xyl_x_range_min;    /*!< Light xyL x Range Min requested by the client. */
	uint16_t xyl_x_range_max;    /*!< Light xyL x Range Max requested by the client. */
	uint16_t xyl_y_range_min;    /*!< Light xyL y Range Min requested by the client. */
	uint16_t xyl_y_range_max;    /*!< Light xyL y Range Max requested by the client. */
} rtk_bt_mesh_light_xyl_server_set_range_t;

/***************************** Light xyL Setup Server Model ****************************/
/**
 * @enum      rtk_bt_mesh_light_xyl_setup_server_model_evt_t
 * @brief     BLE MESH light xyl setup server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_XYL_DEFAULT_SERVER_MODEL_SET = 1,    /*!< Indicate a Light xyL Default Set or Set Unacknowledged message received from a client, with msg
                                                            *   @ref rtk_bt_mesh_light_xyl_server_set_default_t. */
	RTK_BT_MESH_LIGHT_XYL_RANGE_SERVER_MODEL_SET,          /*!< Indicate a Light xyL Range Set or Set Unacknowledged message received from a client, with msg
                                                            *   @ref rtk_bt_mesh_light_xyl_server_set_range_t. */
	RTK_BT_MESH_LIGHT_XYL_SETUP_SERVER_MODEL_EVT_MAX,      /*!< Reserved. */
} rtk_bt_mesh_light_xyl_setup_server_model_evt_t;

/************************************ Light LC Model ***********************************/
/******************************** Light LC Client Model ********************************/
/**
 * @enum      rtk_bt_mesh_light_lc_client_model_act_t
 * @brief     BLE MESH light lc client model act definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_LC_MODE_CLIENT_MODEL_GET = 1,      /*!< Send a Light LC Mode Get message to the server. */
	RTK_BT_MESH_LIGHT_LC_MODE_CLIENT_MODEL_SET,          /*!< Send a Light LC Mode Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_LC_OM_CLIENT_MODEL_GET,            /*!< Send a Light LC Occupancy Mode Get message to the server. */
	RTK_BT_MESH_LIGHT_LC_OM_CLIENT_MODEL_SET,            /*!< Send a Light LC Occupancy Mode Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_LC_LIGHTONOFF_CLIENT_MODEL_GET,    /*!< Send a Light LC Light OnOff Get message to the server. */
	RTK_BT_MESH_LIGHT_LC_LIGHTONOFF_CLIENT_MODEL_SET,    /*!< Send a Light LC Light OnOff Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_LC_PROPERTY_CLIENT_MODEL_GET,      /*!< Send a Light LC Property Get message to the server. */
	RTK_BT_MESH_LIGHT_LC_PROPERTY_CLIENT_MODEL_SET,      /*!< Send a Light LC Property Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_LIGHT_LC_CLIENT_MODEL_ACT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_light_lc_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_light_lc_client_model_evt_t
 * @brief     BLE MESH light lc client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_LC_MODE_CLIENT_MODEL_STATUS = 1,      /*!< Indicate Light LC Mode Status message received from the server, with msg
                                                             *   @ref rtk_bt_mesh_light_lc_client_mode_status_t. */
	RTK_BT_MESH_LIGHT_LC_OM_CLIENT_MODEL_STATUS,            /*!< Indicate Light LC Occupancy Mode Status message received from the server, with msg
                                                             *   @ref rtk_bt_mesh_light_lc_client_om_status_t. */
	RTK_BT_MESH_LIGHT_LC_LIGHTONOFF_CLIENT_MODEL_STATUS,    /*!< Indicate Light LC Light OnOff Status message received from the server, with msg
                                                             *   @ref rtk_bt_mesh_light_lc_client_light_on_off_status_t. */
	RTK_BT_MESH_LIGHT_LC_PROPERTY_CLIENT_MODEL_STATUS,      /*!< Indicate Light LC Property Status message received from the server, with msg
                                                             *   @ref rtk_bt_mesh_light_lc_client_property_status_t. */
	RTK_BT_MESH_LIGHT_LC_CLIENT_MODEL_EVT_MAX,              /*!< Reserved. */
} rtk_bt_mesh_light_lc_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_light_lc_mode_get_t
 * @brief     BLE MESH light lc mode get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_lc_mode_get_t;

/**
 * @struct    rtk_bt_mesh_light_lc_mode_set_t
 * @brief     BLE MESH light lc mode set act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t mode;              /*!< Target Light LC Mode state.
                                *   0x00: The controller is turned off.
                                *   0x01: The controller is turned on. */
	bool ack;                  /*!< Whether the server shall acknowledge the message.
                                *   - true: Send Light LC Mode Set and the server replies with a Light LC Mode Status message.
                                *   - false: Send Light LC Mode Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_lc_mode_set_t;

/**
 * @struct    rtk_bt_mesh_light_lc_om_get_t
 * @brief     BLE MESH light lc om get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_lc_om_get_t;

/**
 * @struct    rtk_bt_mesh_light_lc_om_set_t
 * @brief     BLE MESH light lc om set act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t mode;              /*!< Target Light LC Occupancy Mode state.
                                *   0x00: The controller does not transition from a standby state when occupancy is reported.
                                *   0x01: The controller may transition from a standby state when occupancy is reported. */
	bool ack;                  /*!< Whether the server shall acknowledge the message.
                                *   - true: Send Light LC OM Set and the server replies with a Light LC OM Status message.
                                *   - false: Send Light LC OM Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_lc_om_set_t;

/**
 * @struct    rtk_bt_mesh_light_lc_lightonoff_get_t
 * @brief     BLE MESH light lc light on off get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_light_lc_lightonoff_get_t;

/**
 * @struct    rtk_bt_mesh_light_lc_lightonoff_set_t
 * @brief     BLE MESH light lc light on off set act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	uint8_t light_on_off;                                /*!< Target Light LC Light OnOff state.
                                                          *   0x00: Light Off.
                                                          *   0x01: Light On. */
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
                                                          *   - true: Send Light LC Light OnOff Set and the server replies with a Light LC Light OnOff Status message.
                                                          *   - false: Send Light LC Light OnOff Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_lc_lightonoff_set_t;

/**
 * @struct    rtk_bt_mesh_light_lc_property_get_t
 * @brief     BLE MESH light lc property get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;      /*!< Property ID of the Light LC property to get. */
} rtk_bt_mesh_light_lc_property_get_t;

/**
 * @struct    rtk_bt_mesh_light_lc_property_set_t
 * @brief     BLE MESH light lc property set act message structure.
 */
typedef struct {
	uint16_t dst;                                      /*!< Destination address of the message. */
	uint16_t app_key_index;                            /*!< Index of the application key used to encrypt the message.
                                                        *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;                              /*!< Property ID of the Light LC property to set. */
	uint8_t pvalue[LIGHT_LC_PROPERTY_DATA_MAX_LEN];    /*!< Raw value to write to the Light LC property.
                                                        *   The format is defined by the property referenced by property_id.
                                                        *   Capacity: @ref LIGHT_LC_PROPERTY_DATA_MAX_LEN bytes. */
	uint16_t value_len;                                /*!< Length of the valid data in pvalue, in bytes.
                                                        *   Range: 1 to @ref LIGHT_LC_PROPERTY_DATA_MAX_LEN. */
	bool ack;                                          /*!< Whether the server shall acknowledge the message.
                                                        *   - true: Send Light LC Property Set and the server replies with a Light LC Property Status message.
                                                        *   - false: Send Light LC Property Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_light_lc_property_set_t;

/**
 * @struct    rtk_bt_mesh_light_lc_client_mode_status_t
 * @brief     BLE MESH light lc client model mode status message structure.
 */
typedef struct {
	uint16_t src;    /*!< Unicast address of the server that sent the Status message. */
	uint8_t mode;    /*!< Present Light LC Mode state of the server.
                      *   0x00: The controller is turned off.
                      *   0x01: The controller is turned on. */
} rtk_bt_mesh_light_lc_client_mode_status_t;

/**
 * @struct    rtk_bt_mesh_light_lc_client_om_status_t
 * @brief     BLE MESH light lc client model om status message structure.
 */
typedef struct {
	uint16_t src;    /*!< Unicast address of the server that sent the Status message. */
	uint8_t mode;    /*!< Present Light LC Occupancy Mode state of the server.
                      *   0x00: The controller does not transition from a standby state when occupancy is reported.
                      *   0x01: The controller may transition from a standby state when occupancy is reported. */
} rtk_bt_mesh_light_lc_client_om_status_t;

/**
 * @struct    rtk_bt_mesh_light_lc_client_light_on_off_status_t
 * @brief     BLE MESH light lc client model light on off status message structure.
 */
typedef struct {
	uint16_t src;                                            /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_generic_on_off_t present_on_off;             /*!< Present Light LC State Machine Light OnOff state of the server. */
	bool optional;                                           /*!< Whether the Target Light OnOff and Remaining Time fields are carried in the message.
                                                              *   - true: target_on_off and remaining_time are valid.
                                                              *     The server is changing the state or waiting for the delay to complete.
                                                              *   - false: target_on_off and remaining_time are not valid. */
	rtk_bt_mesh_generic_on_off_t target_on_off;              /*!< Target Light LC State Machine Light OnOff state the server is changing to.
                                                              *   Only valid when optional is true. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   Only valid when optional is true. */
} rtk_bt_mesh_light_lc_client_light_on_off_status_t;

/**
 * @struct    rtk_bt_mesh_light_lc_client_property_status_t
 * @brief     BLE MESH light lc client model property status message structure.
 */
typedef struct {
	uint16_t src;               /*!< Unicast address of the server that sent the Status message. */
	uint16_t property_id;       /*!< Property ID reported by the server. */
	uint32_t property_value;    /*!< Property value reported by the server.
                                 *   Assembled from the raw value bytes of the message, the first byte is the least significant byte.
                                 *   The number of valid bytes depends on the property. */
} rtk_bt_mesh_light_lc_client_property_status_t;

/******************************** Light LC Server Model ********************************/
/**
 * @enum      rtk_bt_mesh_light_lc_server_model_evt_t
 * @brief     BLE MESH light lc server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_MODE_GET = 1,              /*!< Indicate a Light LC Mode Get message is received or the server needs the current
                                                                  *   Light LC Mode state, with msg @ref rtk_bt_mesh_light_lc_server_direct_get_mode_t.
                                                                  *   The application shall write the current Light LC Mode state into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_MODE_SET,                  /*!< Indicate a Light LC Mode Set or Set Unacknowledged message received from a client,
                                                                  *   with msg @ref rtk_bt_mesh_light_lc_server_set_mode_t.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_OM_GET,                    /*!< Indicate a Light LC Occupancy Mode Get message is received or the server needs the current
                                                                  *   Light LC Occupancy Mode state, with msg @ref rtk_bt_mesh_light_lc_server_direct_get_om_t.
                                                                  *   The application shall write the current Light LC Occupancy Mode state into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_OM_SET,                    /*!< Indicate a Light LC Occupancy Mode Set or Set Unacknowledged message received from a client,
                                                                  *   with msg @ref rtk_bt_mesh_light_lc_server_set_om_t.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_LIGHT_ON_OFF_GET,          /*!< Indicate a Light LC Light OnOff Get message is received or the server needs the current
                                                                  *   Light LC State Machine Light OnOff state, with msg
                                                                  *   @ref rtk_bt_mesh_light_lc_server_direct_get_light_on_off_t.
                                                                  *   The application shall write the current Light LC State Machine Light OnOff state into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_SM_TRANSITION_TIME_GET,    /*!< Indicate the server needs the transition time of the Light LC State Machine for a target
                                                                  *   Light LC Light OnOff state, with msg
                                                                  *   @ref rtk_bt_mesh_light_lc_server_direct_get_sm_transition_time_t.
                                                                  *   The application shall write the transition time into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_LIGHT_ON_OFF_SET,          /*!< Indicate the Light LC Light OnOff state is to change, with msg
                                                                  *   @ref rtk_bt_mesh_light_lc_server_set_light_on_off_t.
                                                                  *   Raised when a Light LC Light OnOff Set or Set Unacknowledged message is received,
                                                                  *   and again when the transition is updated or stopped.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_EVT_MAX,                   /*!< Reserved. */
} rtk_bt_mesh_light_lc_server_model_evt_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_get_mode_t
 * @brief     BLE MESH light lc get mode evt message structure.
 */
typedef struct {
	uint8_t mode;    /*!< Current Light LC Mode state.
                      *   0x00: The controller is turned off.
                      *   0x01: The controller is turned on. */
} rtk_bt_mesh_light_lc_server_get_mode_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_direct_get_mode_t
 * @brief     BLE MESH light lc direct get mode evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_lc_server_get_mode_t *value;    /*!< Output: the application writes the current Light LC Mode state here.
                                                       *   Must not be NULL. */
} rtk_bt_mesh_light_lc_server_direct_get_mode_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_get_om_t
 * @brief     BLE MESH light lc get om evt message structure.
 */
typedef struct {
	uint8_t mode;    /*!< Current Light LC Occupancy Mode state.
                      *   0x00: The controller does not transition from a standby state when occupancy is reported.
                      *   0x01: The controller may transition from a standby state when occupancy is reported. */
} rtk_bt_mesh_light_lc_server_get_om_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_direct_get_om_t
 * @brief     BLE MESH light lc direct get mode evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_lc_server_get_om_t *value;    /*!< Output: the application writes the current Light LC Occupancy Mode state here.
                                                     *   Must not be NULL. */
} rtk_bt_mesh_light_lc_server_direct_get_om_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_get_light_on_off_t
 * @brief     BLE MESH light lc get light on off evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_on_off_t on_off;    /*!< Current Light LC State Machine Light OnOff state. */
} rtk_bt_mesh_light_lc_server_get_light_on_off_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_direct_get_light_on_off_t
 * @brief     BLE MESH light lc direct get light on off evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_lc_server_get_light_on_off_t *value;    /*!< Output: the application writes the current Light LC State Machine Light OnOff state here.
                                                               *   Must not be NULL. */
} rtk_bt_mesh_light_lc_server_direct_get_light_on_off_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_get_property_t
 * @brief     BLE MESH light lc get property evt message structure.
 */
typedef struct {
	uint16_t property_id;       /*!< Property ID the server asks the value of. */
	uint32_t property_value;    /*!< Current value of the property.
                                 *   The first byte of the raw value is the least significant byte. */
	uint8_t value_len;          /*!< Length of the property value, in bytes. */
} rtk_bt_mesh_light_lc_server_get_property_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_direct_get_property_para_t
 * @brief     BLE MESH light lc direct get property parameter evt message structure.
 */
typedef struct {
	uint8_t value_len;          /*!< Length of the property value, in bytes. */
	uint32_t property_value;    /*!< Current value of the property.
                                 *   The first byte of the raw value is the least significant byte. */
} rtk_bt_mesh_light_lc_server_direct_get_property_para_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_direct_get_property_t
 * @brief     BLE MESH light lc direct get property evt message structure.
 */
typedef struct {
	uint16_t property_id;                                             /*!< Property ID the server asks the value of. */
	rtk_bt_mesh_light_lc_server_direct_get_property_para_t *value;    /*!< Output: the application writes the value and length of the property here.
                                                                       *   Must not be NULL. */
} rtk_bt_mesh_light_lc_server_direct_get_property_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_get_default_transition_time_t
 * @brief     BLE MESH light lc get default transition time evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Generic Default Transition Time of the server. */
} rtk_bt_mesh_light_lc_server_get_default_transition_time_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_get_sm_transition_time_t
 * @brief     BLE MESH light lc get sm transition time evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_on_off_t light_on_off;              /*!< Target Light LC Light OnOff state the transition time is requested for. */
	rtk_bt_mesh_generic_transition_time_t sm_trans_time;    /*!< Transition time of the Light LC State Machine for the transition to light_on_off.
                                                             *   The application writes it. */
} rtk_bt_mesh_light_lc_server_get_sm_transition_time_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_direct_get_sm_transition_time_t
 * @brief     BLE MESH light lc get sm transition time direct evt message structure.
 */
typedef struct {
	rtk_bt_mesh_light_lc_server_get_sm_transition_time_t *value;    /*!< Output: the application writes the transition time of the Light LC State Machine here.
                                                                     *   Must not be NULL. */
} rtk_bt_mesh_light_lc_server_direct_get_sm_transition_time_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_set_mode_t
 * @brief     BLE MESH light lc set mode evt message structure.
 */
typedef struct {
	uint8_t mode;    /*!< Target Light LC Mode state requested by the client.
                      *   0x00: The controller is turned off.
                      *   0x01: The controller is turned on. */
} rtk_bt_mesh_light_lc_server_set_mode_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_set_om_t
 * @brief     BLE MESH light lc set om evt message structure.
 */
typedef struct {
	uint8_t mode;    /*!< Target Light LC Occupancy Mode state requested by the client.
                      *   0x00: The controller does not transition from a standby state when occupancy is reported.
                      *   0x01: The controller may transition from a standby state when occupancy is reported. */
} rtk_bt_mesh_light_lc_server_set_om_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_set_light_on_off_t
 * @brief     BLE MESH light lc set light on off evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_on_off_t light_on_off;               /*!< Target Light LC Light OnOff state requested by the client. */
	rtk_bt_mesh_generic_transition_time_t total_time;        /*!< Total time of the transition to the target state.
                                                              *   A zero value means the state changes immediately. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   A zero value means the state has reached the target. */
} rtk_bt_mesh_light_lc_server_set_light_on_off_t;

/**
 * @struct    rtk_bt_mesh_light_lc_server_set_property_t
 * @brief     BLE MESH light lc set property evt message structure.
 */
typedef struct {
	uint16_t property_id;       /*!< Property ID of the property to set. */
	uint32_t property_value;    /*!< New value of the property.
                                 *   The first byte of the raw value is the least significant byte. */
} rtk_bt_mesh_light_lc_server_set_property_t;

/***************************** Light LC Setup Server Model *****************************/
/**
 * @enum      rtk_bt_mesh_light_lc_setup_server_model_evt_t
 * @brief     BLE MESH light lc setup server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_PROPERTY_GET = 1,    /*!< Indicate a Light LC Property Get message is received or the server needs the current property value,
                                                            *   with msg @ref rtk_bt_mesh_light_lc_server_direct_get_property_t.
                                                            *   The application shall write the value and length of the property into the msg.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_PROPERTY_SET,        /*!< Indicate a Light LC Property Set or Set Unacknowledged message received from a client,
                                                            *   with msg @ref rtk_bt_mesh_light_lc_server_set_property_t.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_LIGHT_LC_SETUP_SERVER_MODEL_EVT_MAX,       /*!< Reserved. */
} rtk_bt_mesh_light_lc_setup_server_model_evt_t;

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
/** @addtogroup BT_Mesh_Light_Model_Functions BT Mesh Light Model Functions
 * @{
 */

/**
 * @brief     Send a Light Lightness Get message to a light lightness server to read its Light Lightness Actual state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL_STATUS.
 * @param[in] lightness_get: Light Lightness Get message parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lightness_get(rtk_bt_mesh_light_lightness_get_t *lightness_get);

/**
 * @brief     Send a Light Lightness Set or Set Unacknowledged message to a light lightness server to change its Light Lightness Actual state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL_STATUS.
 * @param[in] lightness_set: Light Lightness Set message parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lightness_set(rtk_bt_mesh_light_lightness_set_t *lightness_set);

/**
 * @brief     Send a Light Lightness Linear Get message to a light lightness server to read its Light Lightness Linear state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_CLIENT_MODEL_STATUS.
 * @param[in] lightness_linear_get: Light Lightness Linear Get message parameters.
 *                                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lightness_linear_get(rtk_bt_mesh_light_lightness_linear_get_t *lightness_linear_get);

/**
 * @brief     Send a Light Lightness Linear Set or Set Unacknowledged message to a light lightness server to change its Light Lightness Linear state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_CLIENT_MODEL_STATUS.
 * @param[in] lightness_linear_set: Light Lightness Linear Set message parameters.
 *                                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lightness_linear_set(rtk_bt_mesh_light_lightness_linear_set_t *lightness_linear_set);

/**
 * @brief     Send a Light Lightness Last Get message to a light lightness server to read its Light Lightness Last state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LIGHTNESS_LAST_CLIENT_MODEL_STATUS.
 * @param[in] lightness_last_get: Light Lightness Last Get message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lightness_last_get(rtk_bt_mesh_light_lightness_last_get_t *lightness_last_get);

/**
 * @brief     Send a Light Lightness Default Get message to a light lightness server to read its Light Lightness Default state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_CLIENT_MODEL_STATUS.
 * @param[in] lightness_default_get: Light Lightness Default Get message parameters.
 *                                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lightness_default_get(rtk_bt_mesh_light_lightness_default_get_t *lightness_default_get);

/**
 * @brief     Send a Light Lightness Default Set or Set Unacknowledged message to a light lightness setup server to change its Light Lightness Default state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_CLIENT_MODEL_STATUS.
 * @param[in] lightness_default_set: Light Lightness Default Set message parameters.
 *                                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lightness_default_set(rtk_bt_mesh_light_lightness_default_set_t *lightness_default_set);

/**
 * @brief     Send a Light Lightness Range Get message to a light lightness server to read its Light Lightness Range state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_CLIENT_MODEL_STATUS.
 * @param[in] lightness_range_get: Light Lightness Range Get message parameters.
 *                                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lightness_range_get(rtk_bt_mesh_light_lightness_range_get_t *lightness_range_get);

/**
 * @brief     Send a Light Lightness Range Set or Set Unacknowledged message to a light lightness setup server to change its Light Lightness Range state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_CLIENT_MODEL_STATUS.
 * @param[in] lightness_range_set: Light Lightness Range Set message parameters.
 *                                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lightness_range_set(rtk_bt_mesh_light_lightness_range_set_t *lightness_range_set);

/**
 * @brief     Send a Light CTL Get message to a light CTL server to read its Light CTL state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_STATUS.
 * @param[in] ctl_get: Light CTL Get message parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_ctl_get(rtk_bt_mesh_light_ctl_get_t *ctl_get);

/**
 * @brief     Send a Light CTL Set or Set Unacknowledged message to a light CTL server to change its Light CTL state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_STATUS.
 * @param[in] ctl_set: Light CTL Set message parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_ctl_set(rtk_bt_mesh_light_ctl_set_t *ctl_set);

/**
 * @brief     Send a Light CTL Temperature Get message to a light CTL temperature server to read its Light CTL Temperature state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_STATUS.
 * @param[in] ctl_temperature_get: Light CTL Temperature Get message parameters.
 *                                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_ctl_temperature_get(rtk_bt_mesh_light_ctl_temperature_get_t *ctl_temperature_get);

/**
 * @brief     Send a Light CTL Temperature Set or Set Unacknowledged message to a light CTL temperature server to change its Light CTL Temperature state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_STATUS.
 * @param[in] ctl_temperature_set: Light CTL Temperature Set message parameters.
 *                                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_ctl_temperature_set(rtk_bt_mesh_light_ctl_temperature_set_t *ctl_temperature_set);

/**
 * @brief     Send a Light CTL Temperature Range Get message to a light CTL server to read its Light CTL Temperature Range state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_RANGE_STATUS.
 * @param[in] ctl_temperature_range_get: Light CTL Temperature Range Get message parameters.
 *                                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_ctl_temperature_range_get(rtk_bt_mesh_light_ctl_temperature_range_get_t *ctl_temperature_range_get);

/**
 * @brief     Send a Light CTL Temperature Range Set or Set Unacknowledged message to a light CTL setup server to change its Light CTL Temperature Range state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_RANGE_STATUS.
 * @param[in] ctl_temperature_range_set: Light CTL Temperature Range Set message parameters.
 *                                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_ctl_temperature_range_set(rtk_bt_mesh_light_ctl_temperature_range_set_t *ctl_temperature_range_set);

/**
 * @brief     Send a Light CTL Default Get message to a light CTL server to read its Light CTL Default state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_DEFAULT_STATUS.
 * @param[in] ctl_default_get: Light CTL Default Get message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_ctl_default_get(rtk_bt_mesh_light_ctl_default_get_t *ctl_default_get);

/**
 * @brief     Send a Light CTL Default Set or Set Unacknowledged message to a light CTL setup server to change its Light CTL Default state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_DEFAULT_STATUS.
 * @param[in] ctl_default_set: Light CTL Default Set message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_ctl_default_set(rtk_bt_mesh_light_ctl_default_set_t *ctl_default_set);

/**
 * @brief     Send a Light HSL Get message to a light HSL server to read its Light HSL state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_HSL_CLIENT_MODEL_STATUS.
 * @param[in] hsl_get: Light HSL Get message parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_hsl_get(rtk_bt_mesh_light_hsl_get_t *hsl_get);

/**
 * @brief     Send a Light HSL Set or Set Unacknowledged message to a light HSL server to change its Light HSL state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_HSL_CLIENT_MODEL_STATUS.
 * @param[in] hsl_set: Light HSL Set message parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_hsl_set(rtk_bt_mesh_light_hsl_set_t *hsl_set);

/**
 * @brief     Send a Light HSL Target Get message to a light HSL server to read its Light HSL Target state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_HSL_TARGET_CLIENT_MODEL_STATUS.
 * @param[in] hsl_target_get: Light HSL Target Get message parameters.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_hsl_target_get(rtk_bt_mesh_light_hsl_target_get_t *hsl_target_get);

/**
 * @brief     Send a Light HSL Hue Get message to a light HSL hue server to read its Light HSL Hue state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_HSL_HUE_CLIENT_MODEL_STATUS.
 * @param[in] hsl_hue_get: Light HSL Hue Get message parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_hsl_hue_get(rtk_bt_mesh_light_hsl_hue_get_t *hsl_hue_get);

/**
 * @brief     Send a Light HSL Hue Set or Set Unacknowledged message to a light HSL hue server to change its Light HSL Hue state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_HSL_HUE_CLIENT_MODEL_STATUS.
 * @param[in] hsl_hue_set: Light HSL Hue Set message parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_hsl_hue_set(rtk_bt_mesh_light_hsl_hue_set_t *hsl_hue_set);

/**
 * @brief     Send a Light HSL Saturation Get message to a light HSL saturation server to read its Light HSL Saturation state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_HSL_SATURATION_CLIENT_MODEL_STATUS.
 * @param[in] hsl_saturation_get: Light HSL Saturation Get message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_hsl_saturation_get(rtk_bt_mesh_light_hsl_saturation_get_t *hsl_saturation_get);

/**
 * @brief     Send a Light HSL Saturation Set or Set Unacknowledged message to a light HSL saturation server to change its Light HSL Saturation state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_HSL_SATURATION_CLIENT_MODEL_STATUS.
 * @param[in] hsl_saturation_set: Light HSL Saturation Set message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_hsl_saturation_set(rtk_bt_mesh_light_hsl_saturation_set_t *hsl_saturation_set);

/**
 * @brief     Send a Light HSL Default Get message to a light HSL server to read its Light HSL Default state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_HSL_DEFAULT_CLIENT_MODEL_STATUS.
 * @param[in] hsl_default_get: Light HSL Default Get message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_hsl_default_get(rtk_bt_mesh_light_hsl_default_get_t *hsl_default_get);

/**
 * @brief     Send a Light HSL Default Set or Set Unacknowledged message to a light HSL setup server to change its Light HSL Default state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_HSL_DEFAULT_CLIENT_MODEL_STATUS.
 * @param[in] hsl_default_set: Light HSL Default Set message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_hsl_default_set(rtk_bt_mesh_light_hsl_default_set_t *hsl_default_set);

/**
 * @brief     Send a Light HSL Range Get message to a light HSL server to read its Light HSL Range state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_HSL_RANGE_CLIENT_MODEL_STATUS.
 * @param[in] hsl_range_get: Light HSL Range Get message parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_hsl_range_get(rtk_bt_mesh_light_hsl_range_get_t *hsl_range_get);

/**
 * @brief     Send a Light HSL Range Set or Set Unacknowledged message to a light HSL setup server to change its Light HSL Range state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_HSL_RANGE_CLIENT_MODEL_STATUS.
 * @param[in] hsl_range_set: Light HSL Range Set message parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_hsl_range_set(rtk_bt_mesh_light_hsl_range_set_t *hsl_range_set);

/**
 * @brief     Send a Light xyL Get message to a light xyL server to read its Light xyL state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_XYL_CLIENT_MODEL_STATUS.
 * @param[in] xyl_get: Light xyL Get message parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_xyl_get(rtk_bt_mesh_light_xyl_get_t *xyl_get);

/**
 * @brief     Send a Light xyL Set or Set Unacknowledged message to a light xyL server to change its Light xyL state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_XYL_CLIENT_MODEL_STATUS.
 * @param[in] xyl_set: Light xyL Set message parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_xyl_set(rtk_bt_mesh_light_xyl_set_t *xyl_set);

/**
 * @brief     Send a Light xyL Target Get message to a light xyL server to read its target Light xyL state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_XYL_TARGET_CLIENT_MODEL_STATUS.
 * @param[in] xyl_target_get: Light xyL Target Get message parameters.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_xyl_target_get(rtk_bt_mesh_light_xyl_target_get_t *xyl_target_get);

/**
 * @brief     Send a Light xyL Default Get message to a light xyL server to read its Light xyL Default state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_XYL_DEFAULT_CLIENT_MODEL_STATUS.
 * @param[in] xyl_default_get: Light xyL Default Get message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_xyl_default_get(rtk_bt_mesh_light_xyl_default_get_t *xyl_default_get);

/**
 * @brief     Send a Light xyL Default Set or Set Unacknowledged message to a light xyL setup server to change its Light xyL Default state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_XYL_DEFAULT_CLIENT_MODEL_STATUS.
 * @param[in] xyl_default_set: Light xyL Default Set message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_xyl_default_set(rtk_bt_mesh_light_xyl_default_set_t *xyl_default_set);

/**
 * @brief     Send a Light xyL Range Get message to a light xyL server to read its Light xyL Range state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_XYL_RANGE_CLIENT_MODEL_STATUS.
 * @param[in] xyl_range_get: Light xyL Range Get message parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_xyl_range_get(rtk_bt_mesh_light_xyl_range_get_t *xyl_range_get);

/**
 * @brief     Send a Light xyL Range Set or Set Unacknowledged message to a light xyL setup server to change its Light xyL Range state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_XYL_RANGE_CLIENT_MODEL_STATUS.
 * @param[in] xyl_range_set: Light xyL Range Set message parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_xyl_range_set(rtk_bt_mesh_light_xyl_range_set_t *xyl_range_set);

/**
 * @brief     Send a Light LC Mode Get message to a light LC server to read its Light LC Mode state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LC_MODE_CLIENT_MODEL_STATUS.
 * @param[in] lc_mode_get: Light LC Mode Get message parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lc_mode_get(rtk_bt_mesh_light_lc_mode_get_t *lc_mode_get);

/**
 * @brief     Send a Light LC Mode Set or Set Unacknowledged message to a light LC server to change its Light LC Mode state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LC_MODE_CLIENT_MODEL_STATUS.
 * @param[in] lc_mode_set: Light LC Mode Set message parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lc_mode_set(rtk_bt_mesh_light_lc_mode_set_t *lc_mode_set);

/**
 * @brief     Send a Light LC OM Get message to a light LC server to read its Light LC Occupancy Mode state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LC_OM_CLIENT_MODEL_STATUS.
 * @param[in] lc_om_get: Light LC OM Get message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lc_om_get(rtk_bt_mesh_light_lc_om_get_t *lc_om_get);

/**
 * @brief     Send a Light LC OM Set or Set Unacknowledged message to a light LC server to change its Light LC Occupancy Mode state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LC_OM_CLIENT_MODEL_STATUS.
 * @param[in] lc_om_set: Light LC OM Set message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lc_om_set(rtk_bt_mesh_light_lc_om_set_t *lc_om_set);

/**
 * @brief     Send a Light LC Light OnOff Get message to a light LC server to read its Light LC Light OnOff state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LC_LIGHTONOFF_CLIENT_MODEL_STATUS.
 * @param[in] lc_lightonoff_get: Light LC Light OnOff Get message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lc_lightonoff_get(rtk_bt_mesh_light_lc_lightonoff_get_t *lc_lightonoff_get);

/**
 * @brief     Send a Light LC Light OnOff Set or Set Unacknowledged message to a light LC server to change its Light LC Light OnOff state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LC_LIGHTONOFF_CLIENT_MODEL_STATUS.
 * @param[in] lc_lightonoff_set: Light LC Light OnOff Set message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lc_lightonoff_set(rtk_bt_mesh_light_lc_lightonoff_set_t *lc_lightonoff_set);

/**
 * @brief     Send a Light LC Property Get message to a light LC setup server to read one of its Light LC properties.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LC_PROPERTY_CLIENT_MODEL_STATUS.
 * @param[in] lc_property_get: Light LC Property Get message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lc_property_get(rtk_bt_mesh_light_lc_property_get_t *lc_property_get);

/**
 * @brief     Send a Light LC Property Set or Set Unacknowledged message to a light LC setup server to change one of its Light LC properties.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_LIGHT_LC_PROPERTY_CLIENT_MODEL_STATUS.
 * @param[in] lc_property_set: Light LC Property Set message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_light_lc_property_set(rtk_bt_mesh_light_lc_property_set_t *lc_property_set);

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

#endif /* __RTK_BT_MESH_LIGHT_LIGHTNESS_MODEL_H__ */
