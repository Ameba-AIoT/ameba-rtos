/**
 * @file      rtk_bt_mesh_scheduler_model.h
 * @author    quinn_yang@realsil.com.cn
 * @brief     Bluetooth LE MESH scheduler client and server model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */


#ifndef __RTK_BT_MESH_SCHEDULER_MODEL_H__
#define __RTK_BT_MESH_SCHEDULER_MODEL_H__


#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_generic_onoff_model.h>

/*********************************** Scheduler Model ***********************************/
/**
 * @struct    rtk_bt_mesh_scheduler_register_t
 * @brief     BLE MESH scheduler register state structure.
 */
typedef struct {
	uint64_t index: 4;              /*!< Index of the Schedule Register entry.
                                     *   Range: 0x0 to 0xF. */
	uint64_t year: 7;               /*!< Scheduled year for the action.
                                     *   0x00 to 0x63: 2 least significant digits of the year.
                                     *   0x64: Any year.
                                     *   Other values: Prohibited. */
	uint64_t month: 12;             /*!< Scheduled months for the action, the action is triggered in the months whose bit is set to 1.
                                     *   Bit 0: Scheduled in January.
                                     *   Bit 1: Scheduled in February.
                                     *   Bit 2: Scheduled in March.
                                     *   Bit 3: Scheduled in April.
                                     *   Bit 4: Scheduled in May.
                                     *   Bit 5: Scheduled in June.
                                     *   Bit 6: Scheduled in July.
                                     *   Bit 7: Scheduled in August.
                                     *   Bit 8: Scheduled in September.
                                     *   Bit 9: Scheduled in October.
                                     *   Bit 10: Scheduled in November.
                                     *   Bit 11: Scheduled in December.
                                     *   If all bits are 0, the action is not triggered. */
	uint64_t day: 5;                /*!< Scheduled day of the month for the action.
                                     *   0x00: Any day.
                                     *   0x01 to 0x1F: Day of the month.
                                     *   A day larger than the number of days in the month means the last day of the month. */
	uint64_t hour: 5;               /*!< Scheduled hour for the action.
                                     *   0x00 to 0x17: Hour of the day (00 to 23 hours).
                                     *   0x18: Any hour of the day.
                                     *   0x19: Once a day (at a random hour).
                                     *   Other values: Prohibited. */
	uint64_t minute: 6;             /*!< Scheduled minute for the action.
                                     *   0x00 to 0x3B: Minute of the hour (00 to 59).
                                     *   0x3C: Any minute of the hour.
                                     *   0x3D: Every 15 minutes (minute modulo 15 is 0).
                                     *   0x3E: Every 20 minutes (minute modulo 20 is 0).
                                     *   0x3F: Once an hour (at a random minute). */
	uint64_t second: 6;             /*!< Scheduled second for the action.
                                     *   0x00 to 0x3B: Second of the minute (00 to 59).
                                     *   0x3C: Any second of the minute.
                                     *   0x3D: Every 15 seconds (second modulo 15 is 0).
                                     *   0x3E: Every 20 seconds (second modulo 20 is 0).
                                     *   0x3F: Once a minute (at a random second). */
	uint64_t day_of_week: 7;        /*!< Scheduled days of the week for the action, the action is triggered on the days whose bit is set to 1.
                                     *   Bit 0: Scheduled on Mondays.
                                     *   Bit 1: Scheduled on Tuesdays.
                                     *   Bit 2: Scheduled on Wednesdays.
                                     *   Bit 3: Scheduled on Thursdays.
                                     *   Bit 4: Scheduled on Fridays.
                                     *   Bit 5: Scheduled on Saturdays.
                                     *   Bit 6: Scheduled on Sundays.
                                     *   If all bits are 0, the action is not triggered. */
	uint64_t action: 4;             /*!< Action to be performed at the scheduled time.
                                     *   0x0: Turn Off.
                                     *   0x1: Turn On.
                                     *   0x2: Scene Recall.
                                     *   0xF: Inactive, the entry is not defined and no action is performed.
                                     *   Other values: Reserved for future use. */
	uint64_t num_steps: 6;          /*!< Number of steps of the Transition Time used when the action is performed.
                                     *   0x00: Transition is immediate.
                                     *   0x01 to 0x3E: Number of steps.
                                     *   0x3F: Interpretation of this value is context specific. */
	uint64_t step_resolution: 2;    /*!< Step resolution of the Transition Time used when the action is performed.
                                     *   Transition time = num_steps * step resolution.
                                     *   0x00: 100 milliseconds.
                                     *   0x01: 1 second.
                                     *   0x02: 10 seconds.
                                     *   0x03: 10 minutes. */
	uint16_t scene_number;          /*!< Scene Number to be recalled when the action is Scene Recall.
                                     *   0x0000: No scene. */
} _PACKED4_ rtk_bt_mesh_scheduler_register_t;

/******************************** Scheduler Client Model *******************************/
/**
 * @enum      rtk_bt_mesh_scheduler_client_model_act_t
 * @brief     BLE MESH scheduler client model act definition.
 */
typedef enum {
	RTK_BT_MESH_SCHEDULER_CLIENT_MODEL_GET = 1,       /*!< Send a Scheduler Get message to the server. */
	RTK_BT_MESH_SCHEDULER_ACTION_CLIENT_MODEL_GET,    /*!< Send a Scheduler Action Get message to the server. */
	RTK_BT_MESH_SCHEDULER_ACTION_CLIENT_MODEL_SET,    /*!< Send a Scheduler Action Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_SCHEDULER_CLIENT_MODEL_ACT_MAX,       /*!< Reserved. */
} rtk_bt_mesh_scheduler_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_scheduler_client_model_evt_t
 * @brief     BLE MESH scheduler client model event definition.
 */
typedef enum {
	RTK_BT_MESH_SCHEDULER_CLIENT_MODEL_STATUS = 1,       /*!< Indicate a Scheduler Status message received from the server, with msg
                                                          *   @ref rtk_bt_mesh_scheduler_client_status_t. */
	RTK_BT_MESH_SCHEDULER_ACTION_CLIENT_MODEL_STATUS,    /*!< Indicate a Scheduler Action Status message received from the server, with msg
                                                          *   @ref rtk_bt_mesh_scheduler_client_status_action_t. */
	RTK_BT_MESH_SCHEDULER_CLIENT_MODEL_EVT_MAX,          /*!< Reserved. */
} rtk_bt_mesh_scheduler_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_scheduler_get_t
 * @brief     BLE MESH scheduler get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_scheduler_get_t;

/**
 * @struct    rtk_bt_mesh_scheduler_action_get_t
 * @brief     BLE MESH scheduler action get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t index;             /*!< Index of the Schedule Register entry to get.
                                *   Range: 0x00 to 0x0F, other values are Prohibited. */
} rtk_bt_mesh_scheduler_action_get_t;

/**
 * @struct    rtk_bt_mesh_scheduler_action_set_t
 * @brief     BLE MESH scheduler action set act message structure.
 */
typedef struct {
	uint16_t dst;                                  /*!< Destination address of the message. */
	uint16_t app_key_index;                        /*!< Index of the application key used to encrypt the message.
                                                    *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_scheduler_register_t scheduler;    /*!< Schedule Register entry to set, the entry is identified by its index. */
	bool ack;                                      /*!< Whether the server shall acknowledge the message.
                                                    *   - true: Send Scheduler Action Set and the server replies with a Scheduler Action Status message.
                                                    *   - false: Send Scheduler Action Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_scheduler_action_set_t;

/**
 * @struct    rtk_bt_mesh_scheduler_client_status_t
 * @brief     BLE MESH scheduler client model status message structure.
 */
typedef struct {
	uint16_t src;           /*!< Unicast address of the server that sent the Status message. */
	uint16_t schedulers;    /*!< Bit field indicating which entries in the Schedule Register are defined.
                             *   Bit n corresponds to the entry whose index is n.
                             *   1: The action in the entry is defined.
                             *   0: The action in the entry is not defined. */
} rtk_bt_mesh_scheduler_client_status_t;

/**
 * @struct    rtk_bt_mesh_scheduler_client_status_action_t
 * @brief     BLE MESH scheduler client model action status message structure.
 */
typedef struct {
	uint16_t src;                                  /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_scheduler_register_t scheduler;    /*!< Schedule Register entry reported by the server, the entry is identified by its index. */
} rtk_bt_mesh_scheduler_client_status_action_t;

/******************************** Scheduler Server Model *******************************/
/**
 * @enum      rtk_bt_mesh_scheduler_server_model_evt_t
 * @brief     BLE MESH scheduler server model event definition.
 */
typedef enum {
	RTK_BT_MESH_SCHEDULER_SERVER_MODEL_GET = 1,       /*!< Indicate a Scheduler Get message received from a client, with msg
                                                       *   @ref rtk_bt_mesh_scheduler_server_get_t.
                                                       *   The application shall write the Schedules bit field into the msg.
                                                       *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCHEDULER_ACTION_SERVER_MODEL_GET,    /*!< Indicate a Scheduler Action Get message is received or the server needs a Schedule Register entry, with msg
                                                       *   @ref rtk_bt_mesh_scheduler_server_get_action_t.
                                                       *   The application shall write the entry identified by index into the msg.
                                                       *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCHEDULER_SERVER_MODEL_EVT_MAX,       /*!< Reserved. */
} rtk_bt_mesh_scheduler_server_model_evt_t;

/**
 * @struct    rtk_bt_mesh_scheduler_server_get_t
 * @brief     BLE MESH scheduler get evt message structure.
 */
typedef struct {
	uint16_t *schedulers;    /*!< Output: the application writes the Schedules bit field here.
                              *   Bit n corresponds to the Schedule Register entry whose index is n.
                              *   1: The action in the entry is defined.
                              *   0: The action in the entry is not defined.
                              *   Must not be NULL. */
} rtk_bt_mesh_scheduler_server_get_t;

/**
 * @struct    rtk_bt_mesh_scheduler_server_get_action_t
 * @brief     BLE MESH scheduler action get evt message structure.
 */
typedef struct {
	uint8_t index;                                  /*!< Index of the Schedule Register entry that is needed.
                                                     *   Range: 0x00 to 0x0F. */
	rtk_bt_mesh_scheduler_register_t *scheduler;    /*!< Output: the application writes the Schedule Register entry identified by index here.
                                                     *   Must not be NULL. */
} rtk_bt_mesh_scheduler_server_get_action_t;

/**
 * @typedef   rtk_bt_mesh_scheduler_server_set_action_t
 * @brief     BLE MESH scheduler action set evt message structure.
 */
typedef rtk_bt_mesh_scheduler_register_t rtk_bt_mesh_scheduler_server_set_action_t;

/***************************** Scheduler Setup Server Model ****************************/
/**
 * @enum      rtk_bt_mesh_scheduler_setup_server_model_evt_t
 * @brief     BLE MESH scheduler setup server model event definition.
 */
typedef enum {
	RTK_BT_MESH_SCHEDULER_ACTION_SERVER_MODEL_SET = 1,      /*!< Indicate a Scheduler Action Set or Set Unacknowledged message received from a client, with msg
                                                             *   @ref rtk_bt_mesh_scheduler_server_set_action_t.
                                                             *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCHEDULER_SETUP_SERVER_MODEL_GET_ACTION,    /*!< Indicate the setup server needs the current Schedule Register entry to process a Scheduler Action Set
                                                             *   or Set Unacknowledged message, with msg @ref rtk_bt_mesh_scheduler_server_get_action_t.
                                                             *   The application shall write the entry identified by index into the msg.
                                                             *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCHEDULER_SETUP_SERVER_MODEL_EVT_MAX,       /*!< Reserved. */
} rtk_bt_mesh_scheduler_setup_server_model_evt_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  ble_mesh_scheduler_model BT LE Mesh Scheduler Model APIs
 * @brief     BT LE Mesh Scheduler Model related function APIs
 * @ingroup   ble_mesh_api_group
 * @{
 */

/**
 * @brief     Send a Scheduler Get message to a scheduler server to read which entries of its Schedule Register are defined.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SCHEDULER_CLIENT_MODEL_STATUS.
 * @param[in] scheduler_get: Scheduler Get message parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_scheduler_get(rtk_bt_mesh_scheduler_get_t *scheduler_get);

/**
 * @brief     Send a Scheduler Action Get message to a scheduler server to read an entry of its Schedule Register.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SCHEDULER_ACTION_CLIENT_MODEL_STATUS.
 * @param[in] scheduler_action_get: Scheduler Action Get message parameters.
 *                                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_scheduler_action_get(rtk_bt_mesh_scheduler_action_get_t *scheduler_action_get);

/**
 * @brief     Send a Scheduler Action Set or Set Unacknowledged message to a scheduler setup server to set an entry of its Schedule Register.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SCHEDULER_ACTION_CLIENT_MODEL_STATUS.
 * @param[in] scheduler_action_set: Scheduler Action Set message parameters.
 *                                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_scheduler_action_set(rtk_bt_mesh_scheduler_action_set_t *scheduler_action_set);

/**
 * @}
 */

#endif /* __RTK_BT_MESH_SCHEDULER_MODEL_H__ */
