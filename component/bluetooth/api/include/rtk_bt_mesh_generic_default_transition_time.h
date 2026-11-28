/**
 * @file      rtk_bt_mesh_generic_default_transition_time.h
 * @author    quinn_yang@realsil.com.cn
 * @brief     Bluetooth LE MESH generic client and server model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_MODEL_H__
#define __RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_MODEL_H__

#include <stdint.h>
#include <rtk_bt_mesh_def.h>

#include <rtk_bt_mesh_generic_onoff_model.h>

/** @addtogroup BT_Types BT Types
 * @{
 */
/** @addtogroup BT_Mesh_Types BT Mesh Types
 * @{
 */
/** @addtogroup BT_Mesh_Generic_Default_Transition_Time_Types BT Mesh Generic Default Transition Time Types
 * @{
 */
/********************* Generic Default Transition Time Client Model ********************/
/**
 * @enum      rtk_bt_mesh_generic_default_transition_time_client_model_act_t
 * @brief     BLE MESH generic default transition time client model act definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL_GET = 1,    /*!< Send a Generic Default Transition Time Get message to the server. */
	RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL_SET,        /*!< Send a Generic Default Transition Time Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL_ACT_MAX,    /*!< Reserved. */
} rtk_bt_mesh_generic_default_transition_time_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_generic_default_transition_time_client_model_evt_t
 * @brief     BLE MESH generic default transition time client model event definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL_STATUS = 1,    /*!< Indicate a Generic Default Transition Time Status message received from the server,
                                                                             *   with msg @ref rtk_bt_mesh_generic_transition_time_client_status_t. */
	RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL_EVT_MAX,       /*!< Reserved. */
} rtk_bt_mesh_generic_default_transition_time_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_generic_default_transition_time_get_t
 * @brief     BLE MESH generic default transition time get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_generic_default_transition_time_get_t;

/**
 * @struct    rtk_bt_mesh_generic_default_transition_time_set_t
 * @brief     BLE MESH generic default transition time set act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Generic Default Transition Time state to set.
                                                          *   The num_steps field shall not be 0x3F. */
	bool ack;                                            /*!< Whether the server shall acknowledge the message.
                                                          *   - true: Send Generic Default Transition Time Set and the server replies with a Status message.
                                                          *   - false: Send Generic Default Transition Time Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_generic_default_transition_time_set_t;

/**
 * @struct    rtk_bt_mesh_generic_transition_time_client_status_t
 * @brief     BLE MESH generic default transition time client model status evt message structure.
 */
typedef struct {
	uint16_t src;                                        /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Generic Default Transition Time state reported by the server. */
} rtk_bt_mesh_generic_transition_time_client_status_t;

/********************* Generic Default Transition Time Server Model ********************/
/**
 * @enum      rtk_bt_mesh_generic_default_transition_time_server_model_evt_t
 * @brief     BLE MESH generic default transition time server model event definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_SERVER_MODEL_GET = 1,    /*!< Indicate a Generic Default Transition Time Get message is received or the server needs
                                                                          *   the current Generic Default Transition Time state, with msg
                                                                          *   @ref rtk_bt_mesh_generic_default_transition_time_server_get_t.
                                                                          *   The application shall write the current Generic Default Transition Time state into the msg.
                                                                          *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_SERVER_MODEL_SET,        /*!< Indicate a Generic Default Transition Time Set or Set Unacknowledged message received from a
                                                                          *   client, with msg @ref rtk_bt_mesh_generic_default_transition_time_server_set_t.
                                                                          *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_SERVER_MODEL_EVT_MAX,    /*!< Reserved. */
} rtk_bt_mesh_generic_default_transition_time_server_model_evt_t;

/**
 * @struct    rtk_bt_mesh_generic_default_transition_time_server_set_t
 * @brief     BLE MESH generic default transition time set evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Generic Default Transition Time state requested by the client.
                                                          *   The num_steps field shall not be 0x3F. */
} rtk_bt_mesh_generic_default_transition_time_server_set_t;

/**
 * @struct    rtk_bt_mesh_generic_default_transition_time_server_get_t
 * @brief     BLE MESH generic default transition time get evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_transition_time_t *trans_time;    /*!< Output: the application writes the current Generic Default Transition Time state here.
                                                           *   Must not be NULL. */
} rtk_bt_mesh_generic_default_transition_time_server_get_t;

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
/** @addtogroup BT_Mesh_Generic_Default_Transition_Time_Functions BT Mesh Generic Default Transition Time Functions
 * @{
 */
/**
 * @brief     Send a Generic Default Transition Time Get message to a generic default transition time server to read its Generic Default Transition Time state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL_STATUS.
 * @param[in] time_get: Generic Default Transition Time Get message parameters.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_default_transition_time_client_get(rtk_bt_mesh_generic_default_transition_time_get_t *time_get);

/**
 * @brief     Send a Generic Default Transition Time Set or Set Unacknowledged message to a generic default transition time server
 *            to change its Generic Default Transition Time state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL_STATUS.
 * @param[in] time_set: Generic Default Transition Time Set message parameters.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_default_transition_time_client_set(rtk_bt_mesh_generic_default_transition_time_set_t *time_set);
/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

#endif /* __RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_MODEL_H__ */
