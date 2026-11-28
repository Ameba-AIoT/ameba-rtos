/**
 * @file      rtk_bt_mesh_scene_model.h
 * @author    quinn_yang@realsil.com.cn
 * @brief     Bluetooth LE MESH scene client and server model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_SCENE_MODEL_H__
#define __RTK_BT_MESH_SCENE_MODEL_H__


#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_generic_onoff_model.h>

/** @addtogroup BT_Constants BT Constants
 * @{
 */
/** @addtogroup BT_Mesh_Constants BT Mesh Constants
 * @{
 */
/** @addtogroup BT_Mesh_Scene_Model_Constants BT Mesh Scene Model Constants
 * @{
 */
/**
 * @def       SCENE_REGISTER_ARRAY_DATA_MAX_LEN
 * @brief     Maximum number of Scene Numbers that can be reported for the Scene Register.
 */
#define SCENE_REGISTER_ARRAY_DATA_MAX_LEN    0x10

/**
 * @def       SCENE_DATA_MAX_LEN
 * @brief     Maximum length in bytes of the scene data of a scene.
 */
#define SCENE_DATA_MAX_LEN    0x10

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

/************************************* Scene Model *************************************/
/** @addtogroup BT_Types BT Types
 * @{
 */
/** @addtogroup BT_Mesh_Types BT Mesh Types
 * @{
 */
/** @addtogroup BT_Mesh_Scene_Model_Types BT Mesh Scene Model Types
 * @{
 */
/**
 * @enum      rtk_bt_mesh_scene_status_code_t
 * @brief     BLE MESH scene status code definition.
 */
typedef enum {
	RTK_BT_MESH_SCENE_STATUS_SUCCESS,          /*!< Success. */
	RTK_BT_MESH_SCENE_STATUS_REGISTER_FULL,    /*!< Scene Register is full. */
	RTK_BT_MESH_SCENE_STATUS_NOT_FOUND         /*!< Scene is not found. */
} rtk_bt_mesh_scene_status_code_t;

/********************************** Scene Client Model *********************************/
/**
 * @enum      rtk_bt_mesh_scene_client_model_act_t
 * @brief     BLE MESH scene client model act definition.
 */
typedef enum {
	RTK_BT_MESH_SCENE_CLIENT_MODEL_STORE = 1,       /*!< Send a Scene Store or Scene Store Unacknowledged message to the server. */
	RTK_BT_MESH_SCENE_CLIENT_MODEL_RECALL,          /*!< Send a Scene Recall or Scene Recall Unacknowledged message to the server. */
	RTK_BT_MESH_SCENE_CLIENT_MODEL_GET,             /*!< Send a Scene Get message to the server. */
	RTK_BT_MESH_SCENE_CLIENT_MODEL_REGISTER_GET,    /*!< Send a Scene Register Get message to the server. */
	RTK_BT_MESH_SCENE_CLIENT_MODEL_DELETE,          /*!< Send a Scene Delete or Scene Delete Unacknowledged message to the server. */
	RTK_BT_MESH_SCENE_CLIENT_MODEL_ACT_MAX,         /*!< Reserved. */
} rtk_bt_mesh_scene_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_scene_client_model_evt_t
 * @brief     BLE MESH scene client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_SCENE_CLIENT_MODEL_STATUS = 1,         /*!< Indicate a Scene Status message received from the server, with msg
                                                        *   @ref rtk_bt_mesh_scene_client_status_t. */
	RTK_BT_MESH_SCENE_CLIENT_MODEL_REGISTER_STATUS,    /*!< Indicate a Scene Register Status message received from the server, with msg
                                                        *   @ref rtk_bt_mesh_scene_client_register_status_t.
                                                        *   The msg is a byte stream filled in the order below, the structure only indicates this order and the sizes:
                                                        *   - src, 2 bytes.
                                                        *   - status, 1 byte.
                                                        *   - current_scene, 2 bytes.
                                                        *   - scene_array_len, 1 byte.
                                                        *   - scene_array, scene_array_len Scene Numbers of 2 bytes each. */
	RTK_BT_MESH_SCENE_CLIENT_MODEL_EVT_MAX,            /*!< Reserved. */
} rtk_bt_mesh_scene_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_scene_store_t
 * @brief     BLE MESH scene store act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t scene_number;     /*!< Scene Number of the scene to be stored.
                                *   Range: 0x0001 to 0xFFFF.
                                *   0x0000: Prohibited. */
	bool ack;                  /*!< Whether the server shall acknowledge the message.
                                *   - true: Send Scene Store and the server replies with a Scene Register Status message.
                                *   - false: Send Scene Store Unacknowledged and the server does not reply. */
} rtk_bt_mesh_scene_store_t;

/**
 * @struct    rtk_bt_mesh_scene_recall_t
 * @brief     BLE MESH scene recall act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	uint16_t scene_number;                               /*!< Scene Number of the scene to be recalled.
                                                          *   Range: 0x0001 to 0xFFFF.
                                                          *   0x0000: Prohibited. */
	uint8_t tid;                                         /*!< Transaction identifier of the message.
                                                          *   Assigned automatically by the API, the application does not need to set it. */
	bool optional;                                       /*!< Whether the Transition Time and Delay fields are carried in the message.
                                                          *   - true: Transition Time and Delay are carried.
                                                          *   - false: Transition Time and Delay are not carried. */
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Time the server takes to transition to the states of the recalled scene.
                                                          *   Only used when optional is true. */
	uint8_t delay;                                       /*!< Delay before the server starts the transition.
                                                          *   Time = N * 5 ms.
                                                          *   Time Range: 0 ms to 1275 ms.
                                                          *   Only used when optional is true. */
	bool ack;                                            /*!< Whether the server shall acknowledge the message.
                                                          *   - true: Send Scene Recall and the server replies with a Scene Status message.
                                                          *   - false: Send Scene Recall Unacknowledged and the server does not reply. */
} rtk_bt_mesh_scene_recall_t;

/**
 * @struct    rtk_bt_mesh_scene_get_t
 * @brief     BLE MESH scene get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_scene_get_t;

/**
 * @struct    rtk_bt_mesh_scene_register_get_t
 * @brief     BLE MESH scene register get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_scene_register_get_t;

/**
 * @struct    rtk_bt_mesh_scene_delete_t
 * @brief     BLE MESH scene delete act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t scene_number;     /*!< Scene Number of the scene to be deleted.
                                *   Range: 0x0001 to 0xFFFF.
                                *   0x0000: Prohibited. */
	bool ack;                  /*!< Whether the server shall acknowledge the message.
                                *   - true: Send Scene Delete and the server replies with a Scene Register Status message.
                                *   - false: Send Scene Delete Unacknowledged and the server does not reply. */
} rtk_bt_mesh_scene_delete_t;

/**
 * @struct    rtk_bt_mesh_scene_client_status_t
 * @brief     BLE MESH scene client model status message structure.
 */
typedef struct {
	uint16_t src;                                            /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_scene_status_code_t status;                  /*!< Status code of the last operation. */
	uint16_t current_scene;                                  /*!< Scene Number of the current scene.
                                                              *   0x0000: No scene is active. */
	bool optional;                                           /*!< Whether the Target Scene and Remaining Time fields are carried in the message.
                                                              *   - true: target_scene and remaining_time are valid.
                                                              *     The server is changing the state or waiting for the delay to complete.
                                                              *   - false: target_scene and remaining_time are not valid. */
	uint16_t target_scene;                                   /*!< Scene Number of the target scene the server is changing to.
                                                              *   Only valid when optional is true. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   Only valid when optional is true. */
} rtk_bt_mesh_scene_client_status_t;

/**
 * @struct    rtk_bt_mesh_scene_client_register_status_t
 * @brief     BLE MESH scene client model register status message structure.
 */
typedef struct {
	uint16_t src;                              /*!< Unicast address of the server that sent the Status message. */
	rtk_bt_mesh_scene_status_code_t status;    /*!< Status code of the previous operation. */
	uint16_t current_scene;                    /*!< Scene Number of the current scene.
                                                *   0x0000: No scene is active. */
	uint8_t scene_array_len;                   /*!< Number of Scene Numbers in scene_array. */
	uint16_t *scene_array;                     /*!< Scene Numbers stored in the Scene Register of the server, scene_array_len elements.
                                                *   This member only marks where the data starts,
                                                *   the Scene Numbers are located right after scene_array_len in the event msg. */
} rtk_bt_mesh_scene_client_register_status_t;


/********************************** Scene Server Model *********************************/
/**
 * @enum      rtk_bt_mesh_scene_server_model_evt_t
 * @brief     BLE MESH scene server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_SCENE_SERVER_MODEL_GET = 1,                /*!< Indicate a Scene Get message is received or the server needs the Current Scene state, with msg
                                                            *   @ref rtk_bt_mesh_scene_server_get_t.
                                                            *   The application shall write the Current Scene state into the msg.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCENE_SERVER_MODEL_RECALL,                 /*!< Indicate a scene is to be recalled, with msg @ref rtk_bt_mesh_scene_server_recall_t.
                                                            *   Raised when a Scene Recall or Scene Recall Unacknowledged message is received and its delay has elapsed,
                                                            *   and again when the transition is updated or stopped.
                                                            *   The msg is a byte stream:
                                                            *   - scene_number, 2 bytes.
                                                            *   - total_time, 1 byte.
                                                            *   - remaining_time, 1 byte.
                                                            *   - Scene data stored for the scene, @ref SCENE_DATA_MAX_LEN bytes.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCENE_SERVER_MODEL_GET_REGISTER_STATUS,    /*!< Indicate a Scene Register Get message received from a client, with msg
                                                            *   @ref rtk_bt_mesh_scene_server_get_register_status_t.
                                                            *   The application shall write the status code of the Scene Register into the msg.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCENE_SERVER_MODEL_GET_STATUS_RECALL,      /*!< Indicate the server needs the status code of the latest Scene Recall, with msg
                                                            *   @ref rtk_bt_mesh_scene_server_get_status_recall_t.
                                                            *   The application shall write the status code into the msg.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCENE_SERVER_MODEL_GET_SCENES,             /*!< Indicate the server needs the Scene Numbers stored in the Scene Register, with msg
                                                            *   @ref rtk_bt_mesh_scene_server_get_scenes_t.
                                                            *   It follows event @ref RTK_BT_MESH_SCENE_SERVER_MODEL_GET_SCENES_NUM, which gives num_scenes.
                                                            *   The application shall write the Scene Numbers into the msg.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCENE_SERVER_MODEL_GET_SCENES_NUM,         /*!< Indicate the server needs the number of scenes stored in the Scene Register, with msg
                                                            *   @ref rtk_bt_mesh_scene_server_get_scenes_num_t.
                                                            *   The application shall write the number of scenes into the msg.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCENE_SERVER_MODEL_EVT_MAX,                /*!< Reserved. */
} rtk_bt_mesh_scene_server_model_evt_t;

/**
 * @enum      rtk_bt_mesh_scene_server_model_act_t
 * @brief     BLE MESH scene server model act definition.
 */
typedef enum {
	RTK_BT_MESH_SCENE_SERVER_MODEL_ACT_DB_SET = 1,    /*!< Set the scene database of the server. */
	RTK_BT_MESH_SCENE_SERVER_MODEL_ACT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_scene_server_model_act_t;

/**
 * @struct    rtk_bt_mesh_scene_storage_memory_t
 * @brief     BLE MESH scene server database structure.
 */
typedef struct {
	uint16_t scene_number;    /*!< Scene Number of the scene stored in this entry.
                               *   0x0000: The entry is unused. */
	void *pmemory;            /*!< Memory that stores the scene data of this scene, @ref SCENE_DATA_MAX_LEN bytes. */
} rtk_bt_mesh_scene_storage_memory_t;

/**
 * @struct    rtk_bt_mesh_scene_server_db_set_t
 * @brief     BLE MESH scene server database set message structure.
 */
typedef struct {
	uint16_t num_scenes;    /*!< Number of entries in the scene database, which is the capacity of the Scene Register. */
	void *pdata_db;         /*!< Scene database, num_scenes entries of @ref rtk_bt_mesh_scene_storage_memory_t.
                             *   Must remain valid after the API returns. */
} rtk_bt_mesh_scene_server_db_set_t;

/**
 * @struct    rtk_bt_mesh_scene_server_get_t
 * @brief     BLE MESH scene get evt message structure.
 */
typedef struct {
	uint16_t *current_scene;    /*!< Output: the application writes the Current Scene state here.
                                 *   0x0000: No scene is active.
                                 *   Must not be NULL. */
} rtk_bt_mesh_scene_server_get_t;

/**
 * @struct    rtk_bt_mesh_scene_server_get_register_status_t
 * @brief     BLE MESH scene get register status evt message structure.
 */
typedef struct {
	rtk_bt_mesh_scene_status_code_t *status;    /*!< Output: the application writes the status code of the Scene Register here.
                                                 *   Must not be NULL. */
} rtk_bt_mesh_scene_server_get_register_status_t;

/**
 * @struct    rtk_bt_mesh_scene_server_get_default_transition_time_t
 * @brief     BLE MESH scene get default transition time evt message structure.
 */
typedef struct {
	rtk_bt_mesh_generic_transition_time_t trans_time;    /*!< Generic Default Transition Time of the server. */
} rtk_bt_mesh_scene_server_get_default_transition_time_t;

/**
 * @struct    rtk_bt_mesh_scene_server_store_t
 * @brief     BLE MESH scene store evt message structure.
 */
typedef struct {
	rtk_bt_mesh_scene_status_code_t status;    /*!< Status code of the Store operation. */
	uint16_t scene_number;                     /*!< Scene Number of the scene to be stored. */
	uint8_t *pmemory;                          /*!< Output: the application writes the scene data to be stored here, @ref SCENE_DATA_MAX_LEN bytes.
                                                *   Must not be NULL. */
} rtk_bt_mesh_scene_server_store_t;

/**
 * @struct    rtk_bt_mesh_scene_server_recall_t
 * @brief     BLE MESH scene recall evt message structure.
 */
typedef struct {
	uint16_t scene_number;                                   /*!< Scene Number of the scene to be recalled. */
	rtk_bt_mesh_generic_transition_time_t total_time;        /*!< Total time of the transition to the recalled scene.
                                                              *   A zero value means the states change immediately. */
	rtk_bt_mesh_generic_transition_time_t remaining_time;    /*!< Time remaining before the transition completes.
                                                              *   A zero value means the states have reached the recalled scene. */
	uint8_t *pmemory;                                        /*!< Scene data stored for the scene, @ref SCENE_DATA_MAX_LEN bytes.
                                                              *   This member only marks where the data starts,
                                                              *   the scene data is located right after remaining_time in the event msg. */
} rtk_bt_mesh_scene_server_recall_t;

/**
 * @struct    rtk_bt_mesh_scene_server_delete_t
 * @brief     BLE MESH scene delete evt message structure.
 */
typedef struct {
	uint16_t scene_number;    /*!< Scene Number of the scene that has been deleted. */
} rtk_bt_mesh_scene_server_delete_t;

/**
 * @struct    rtk_bt_mesh_scene_server_get_status_recall_t
 * @brief     BLE MESH scene get status recall evt message structure.
 */
typedef struct {
	uint16_t scene_number;                             /*!< Scene Number of the scene being recalled. */
	rtk_bt_mesh_scene_status_code_t *status_recall;    /*!< Output: the application writes the status code of the Recall here.
                                                        *   Must not be NULL. */
} rtk_bt_mesh_scene_server_get_status_recall_t;

/**
 * @struct    rtk_bt_mesh_scene_server_get_scenes_t
 * @brief     BLE MESH scene get scenes evt message structure.
 */
typedef struct {
	uint16_t num_scenes;    /*!< Number of Scene Numbers the application shall write, as provided by event
                             *   @ref RTK_BT_MESH_SCENE_SERVER_MODEL_GET_SCENES_NUM. */
	uint16_t *scenes;       /*!< Output: the application writes the Scene Numbers here, num_scenes Scene Numbers of 2 bytes each.
                             *   Capacity: @ref SCENE_DATA_MAX_LEN elements.
                             *   Must not be NULL. */
} rtk_bt_mesh_scene_server_get_scenes_t;

/**
 * @struct    rtk_bt_mesh_scene_server_get_scenes_num_t
 * @brief     BLE MESH scene get scenes number evt message structure.
 */
typedef struct {
	uint16_t *num_scenes;    /*!< Output: the application writes the number of scenes stored in the Scene Register here.
                              *   Must not be NULL. */
} rtk_bt_mesh_scene_server_get_scenes_num_t;

/******************************* Scene Setup Server Model ******************************/
/**
 * @enum      rtk_bt_mesh_scene_setup_server_model_evt_t
 * @brief     BLE MESH scene setup server model evt definition.
 */
typedef enum {
	RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_STORE = 1,              /*!< Indicate a Scene Store or Scene Store Unacknowledged message received from a client, with msg
                                                                  *   @ref rtk_bt_mesh_scene_server_store_t.
                                                                  *   The application shall write the scene data to be stored into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_DELETE,                 /*!< Indicate a Scene Delete or Scene Delete Unacknowledged message received from a client,
                                                                  *   and the scene has been deleted, with msg @ref rtk_bt_mesh_scene_server_delete_t.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_GET,                    /*!< Indicate the setup server needs the Current Scene state, with msg
                                                                  *   @ref rtk_bt_mesh_scene_server_get_t.
                                                                  *   The application shall write the Current Scene state into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_GET_STATUS_REGISTER,    /*!< Indicate the setup server needs the status code of the previous operation, with msg
                                                                  *   @ref rtk_bt_mesh_scene_setup_server_get_register_status_t.
                                                                  *   The application shall write the status code into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_GET_SCENES,             /*!< Indicate the setup server needs the Scene Numbers stored in the Scene Register, with msg
                                                                  *   @ref rtk_bt_mesh_scene_server_get_scenes_t.
                                                                  *   It follows event @ref RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_GET_SCENES_NUM, which gives num_scenes.
                                                                  *   The application shall write the Scene Numbers into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_GET_SCENES_NUM,         /*!< Indicate the setup server needs the number of scenes stored in the Scene Register, with msg
                                                                  *   @ref rtk_bt_mesh_scene_server_get_scenes_num_t.
                                                                  *   The application shall write the number of scenes into the msg.
                                                                  *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_EVT_MAX,                /*!< Reserved. */
} rtk_bt_mesh_scene_setup_server_model_evt_t;

/**
 * @enum      rtk_bt_mesh_scene_setup_server_model_act_t
 * @brief     BLE MESH scene setup server model act definition.
 */
typedef enum {
	RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_ACT_DB_SET = 1,    /*!< Set the scene database of the setup server. */
	RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_ACT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_scene_setup_server_model_act_t;

/**
 * @struct    rtk_bt_mesh_scene_setup_server_get_register_status_t
 * @brief     BLE MESH scene get register status evt message structure.
 */
typedef struct {
	uint16_t scene_number;                      /*!< Scene Number of the current scene. */
	rtk_bt_mesh_scene_status_code_t *status;    /*!< Output: the application writes the status code of the previous operation here.
                                                 *   Must not be NULL. */
} rtk_bt_mesh_scene_setup_server_get_register_status_t;

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
/** @addtogroup BT_Mesh_Scene_Model_Functions BT Mesh Scene Model Functions
 * @{
 */
/**
 * @brief     Send a Scene Store or Scene Store Unacknowledged message to a scene setup server to store its present states as a scene.
 *            Only when ack is true, the server replies with a Scene Register Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SCENE_CLIENT_MODEL_REGISTER_STATUS.
 * @param[in] scene_client_store: Scene Store or Scene Store Unacknowledged message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_scene_store(rtk_bt_mesh_scene_store_t *scene_client_store);

/**
 * @brief     Send a Scene Recall or Scene Recall Unacknowledged message to a scene server to recall a stored scene.
 *            Only when ack is true, the server replies with a Scene Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SCENE_CLIENT_MODEL_STATUS.
 * @param[in] scene_client_recall: Scene Recall or Scene Recall Unacknowledged message parameters.
 *                                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The tid member of scene_client_recall is overwritten by this API.
 */
uint16_t rtk_bt_mesh_scene_recall(rtk_bt_mesh_scene_recall_t *scene_client_recall);

/**
 * @brief     Send a Scene Get message to a scene server to read its current scene.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SCENE_CLIENT_MODEL_STATUS.
 * @param[in] scene_client_get: Scene Get message parameters.
 *                              Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_scene_get(rtk_bt_mesh_scene_get_t *scene_client_get);

/**
 * @brief     Send a Scene Register Get message to a scene server to read the Scene Numbers stored in its Scene Register.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SCENE_CLIENT_MODEL_REGISTER_STATUS.
 * @param[in] scene_register_get: Scene Register Get message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_scene_register_get(rtk_bt_mesh_scene_register_get_t *scene_register_get);

/**
 * @brief     Send a Scene Delete or Scene Delete Unacknowledged message to a scene setup server to delete a stored scene.
 *            Only when ack is true, the server replies with a Scene Register Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SCENE_CLIENT_MODEL_REGISTER_STATUS.
 * @param[in] scene_delete: Scene Delete or Scene Delete Unacknowledged message parameters.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_scene_delete(rtk_bt_mesh_scene_delete_t *scene_delete);

/**
 * @brief     Set the scene database of the local scene server.
 *            The database is stored locally and no mesh message is sent.
 * @param[in] db_set: Scene database parameters.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The server uses a default scene database until this API replaces it. Call this API to use the application's own scene storage.
 */
uint16_t rtk_bt_mesh_scene_server_db_set(rtk_bt_mesh_scene_server_db_set_t *db_set);

/**
 * @brief     Set the scene database of the local scene setup server.
 *            The database is stored locally and no mesh message is sent.
 * @param[in] db_set: Scene database parameters.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The setup server uses a default scene database until this API replaces it. Call this API to use the application's own scene storage.
 */
uint16_t rtk_bt_mesh_scene_setup_server_db_set(rtk_bt_mesh_scene_server_db_set_t *db_set);

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

#endif /* __RTK_BT_MESH_SCENE_MODEL_H__ */
