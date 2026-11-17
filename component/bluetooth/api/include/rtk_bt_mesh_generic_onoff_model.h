/**
 * @file      rtk_bt_mesh_generic_onoff_model.h
 * @author    pico_zhai@realsil.com.cn
 * @brief     Bluetooth LE MESH generic on off client and server model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_GENERIC_ONOFF_MODEL_H__
#define __RTK_BT_MESH_GENERIC_ONOFF_MODEL_H__

#include <stdint.h>
#include <rtk_bt_mesh_def.h>

/**
 * @enum      rtk_bt_mesh_generic_onoff_client_model_act_t
 * @brief     BLE MESH generic on off client model act definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_ONOFF_CLIENT_MODEL_ACT_SET = 1,    /*!< Send a Generic OnOff Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_GENERIC_ONOFF_CLIENT_MODEL_ACT_GET,        /*!< Send a Generic OnOff Get message to the server. */
	RTK_BT_MESH_GENERIC_ONOFF_CLIENT_MODEL_ACT_MAX,        /*!< Reserved. */
} rtk_bt_mesh_generic_onoff_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_generic_onoff_client_model_evt_t
 * @brief     BLE MESH generic on off client model event definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_ONOFF_CLIENT_MODEL_EVT_STATUS = 1,    /*!< Indicate Generic OnOff Status message received from the server, with msg
                                                               *   @ref rtk_bt_mesh_generic_onoff_client_evt_status_t. */
	RTK_BT_MESH_GENERIC_ONOFF_CLIENT_MODEL_EVT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_generic_onoff_client_model_evt_t;

/**
 * @enum      rtk_bt_mesh_generic_onoff_server_model_evt_t
 * @brief     BLE MESH generic on off server model event definition.
 */
typedef enum {
	RTK_BT_MESH_GENERIC_ONOFF_SERVER_MODEL_EVT_SET = 1,    /*!< Indicate the Generic OnOff state is to change, with msg
                                                            *   @ref rtk_bt_mesh_generic_onoff_server_evt_set_t.
                                                            *   Raised when a Set or Set Unacknowledged message is received,
                                                            *   and again when the transition is updated or stopped.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_ONOFF_SERVER_MODEL_EVT_GET,        /*!< Indicate a Generic OnOff Get message is received or the server needs the current Generic OnOff state,
                                                            *   with msg @ref rtk_bt_mesh_generic_onoff_server_evt_get_t.
                                                            *   The application shall write the current Generic OnOff state into the msg.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_GENERIC_ONOFF_SERVER_MODEL_EVT_MAX,        /*!< Reserved. */
} rtk_bt_mesh_generic_onoff_server_model_evt_t;

/**
 * @enum      rtk_bt_mesh_generic_on_off_t
 * @brief     BLE MESH generic on off set data structure
 */
typedef enum {
	RTK_BT_MESH_GENERIC_OFF,    /*!< Generic OnOff state is Off. */
	RTK_BT_MESH_GENERIC_ON      /*!< Generic OnOff state is On. */
} rtk_bt_mesh_generic_on_off_t;

/**
 * @struct    rtk_bt_mesh_generic_transition_time_t
 * @brief     BLE MESH generic on off set transition time structure.
 */
typedef struct {
	uint8_t num_steps: 6;          /*!< Number of steps.
                                    *   0x00: Generic Transition Time is immediate.
                                    *   0x01 to 0x3E: Number of steps.
                                    *   0x3F: Interpretation of this value is context specific. */
	uint8_t step_resolution: 2;    /*!< Step resolution.
                                    *   Transition time = num_steps * step resolution.
                                    *   0x00: 100 milliseconds.
                                    *   0x01: 1 second.
                                    *   0x02: 10 seconds.
                                    *   0x03: 10 minutes. */
} _PACKED4_ rtk_bt_mesh_generic_transition_time_t;

/**
 * @struct    rtk_bt_mesh_generic_onoff_client_act_set_t
 * @brief     BLE MESH generic on off set message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_generic_on_off_t on_off;                 /*!< Target Generic OnOff state. */
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
                                                          *   - true: Send Generic OnOff Set and the server replies with a Generic OnOff Status message.
                                                          *   - false: Send Generic OnOff Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_generic_onoff_client_act_set_t;

/**
 * @struct    rtk_bt_mesh_generic_onoff_client_act_get_t
 * @brief     BLE MESH generic on off get message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_generic_onoff_client_act_get_t;

/**
 * @struct    rtk_bt_mesh_generic_onoff_server_evt_set_t
 * @brief     BLE MESH generic on off set evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_on_off_t on_off;                     /*!< Target Generic OnOff state requested by the client. */
	rtk_bt_mesh_generic_transition_time_t total_time;        /*!< Total time of the transition to the target state.
                                                              *   A zero value means the state changes immediately. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   A zero value means the state has reached the target. */
} rtk_bt_mesh_generic_onoff_server_evt_set_t;

/**
 * @struct    rtk_bt_mesh_generic_onoff_server_evt_get_t
 * @brief     BLE MESH generic on off get evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_on_off_t *on_off;    /*!< Output: the application writes the current Generic OnOff state here.
                                              *   Must not be NULL. */
} rtk_bt_mesh_generic_onoff_server_evt_get_t;

/**
 * @struct    rtk_bt_mesh_generic_onoff_client_evt_status_t
 * @brief     BLE MESH generic on off client model set evt message structure.
 */
typedef struct {
	uint16_t src;                                            /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_generic_on_off_t present_on_off;             /*!< Present Generic OnOff state of the server. */
	bool optional;                                           /*!< Whether the Target OnOff and Remaining Time fields are carried in the message.
                                                              *   - true: target_on_off and remaining_time are valid.
                                                              *     The server is changing the state or waiting for the delay to complete.
                                                              *   - false: target_on_off and remaining_time are not valid. */
	rtk_bt_mesh_generic_on_off_t target_on_off;              /*!< Target Generic OnOff state the server is changing to.
                                                              *   Only valid when optional is true. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   Only valid when optional is true. */
} rtk_bt_mesh_generic_onoff_client_evt_status_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  ble_mesh_generic_onoff_model BT LE Mesh Generic On Off Model APIs
 * @brief     BT LE Mesh Generic On Off Model related function APIs
 * @ingroup   ble_mesh_api_group
 * @{
 */

/**
 * @brief     Send a Generic OnOff Set or Set Unacknowledged message to a generic on off server to change its Generic OnOff state.
 *            Only when ack is true, the server replies with a Generic OnOff Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_ONOFF_CLIENT_MODEL_EVT_STATUS.
 * @param[in] onoff_set: Generic OnOff Set message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The tid member of onoff_set is overwritten by this API.
 */
uint16_t rtk_bt_mesh_generic_onoff_client_set(rtk_bt_mesh_generic_onoff_client_act_set_t *onoff_set);

/**
 * @brief     Send a Generic OnOff Get message to a generic on off server to read its Generic OnOff state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_GENERIC_ONOFF_CLIENT_MODEL_EVT_STATUS.
 * @param[in] onoff_get: Generic OnOff Get message parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_generic_onoff_client_get(rtk_bt_mesh_generic_onoff_client_act_get_t *onoff_get);

/**
 * @}
 */

#endif /* __RTK_BT_MESH_GENERIC_ONOFF_MODEL_H__ */
