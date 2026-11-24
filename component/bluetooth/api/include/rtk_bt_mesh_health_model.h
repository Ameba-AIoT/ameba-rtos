/**
 * @file      rtk_bt_mesh_health_model.h
 * @author    quinn_yang@realsil.com.cn
 * @brief     Bluetooth LE MESH health client and server model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_HEALTH_MODEL_H__
#define __RTK_BT_MESH_HEALTH_MODEL_H__

#include <rtk_bt_mesh_def.h>

/************************************* Health Model ************************************/

/**
 * @def       HEALTH_FAULT_ARRAY_LEN
 * @brief     Number of fault values that the fault array of a Health Server event can hold.
 * @note
 *            - The number of fault values plus the Test ID is carried in a 1 byte length, so the value shall not exceed 254.
 */
#define HEALTH_FAULT_ARRAY_LEN 0x10

/********************************* Health Client Model *********************************/
/**
 * @enum      rtk_bt_mesh_health_client_model_act_t
 * @brief     BLE MESH health client model act definition.
 */
typedef enum {
	RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_GET = 1,    /*!< Send a Health Fault Get message to the server. */
	RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_CLEAR,      /*!< Send a Health Fault Clear or Health Fault Clear Unacknowledged message to the server. */
	RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_TEST,       /*!< Send a Health Fault Test or Health Fault Test Unacknowledged message to the server. */
	RTK_BT_MESH_HEALTH_CLIENT_MODEL_PERIOD_GET,       /*!< Send a Health Period Get message to the server. */
	RTK_BT_MESH_HEALTH_CLIENT_MODEL_PERIOD_SET,       /*!< Send a Health Period Set or Health Period Set Unacknowledged message to the server. */
	RTK_BT_MESH_HEALTH_CLIENT_MODEL_ATTN_GET,         /*!< Send a Health Attention Get message to the server. */
	RTK_BT_MESH_HEALTH_CLIENT_MODEL_ATTN_SET,         /*!< Send a Health Attention Set or Health Attention Set Unacknowledged message to the server. */
	RTK_BT_MESH_HEALTH_CLIENT_MODEL_ACT_MAX,          /*!< Reserved. */
} rtk_bt_mesh_health_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_health_client_model_evt_t
 * @brief     BLE MESH health client model event definition.
 */
typedef enum {
	RTK_BT_MESH_HEALTH_CLIENT_MODEL_ATTN_STATUS = 1,    /*!< Indicate a Health Attention Status message received from the server, with msg
                                                         *   @ref rtk_bt_mesh_health_client_status_attention_t. */
	RTK_BT_MESH_HEALTH_CLIENT_MODEL_CURT_STATUS,        /*!< Indicate a Health Current Status message received from the server, with msg
                                                         *   @ref rtk_bt_mesh_health_client_status_t. */
	RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_STATUS,       /*!< Indicate a Health Fault Status message received from the server, with msg
                                                         *   @ref rtk_bt_mesh_health_client_status_t. */
	RTK_BT_MESH_HEALTH_CLIENT_MODEL_PERIOD_STATUS,      /*!< Indicate a Health Period Status message received from the server, with msg
                                                         *   @ref rtk_bt_mesh_health_client_status_period_t. */
	RTK_BT_MESH_HEALTH_CLIENT_MODEL_EVT_MAX,            /*!< Reserved. */
} rtk_bt_mesh_health_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_health_fault_get_t
 * @brief     BLE MESH health fault get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t company_id;       /*!< Company ID that identifies the Health Fault state to get. */
} rtk_bt_mesh_health_fault_get_t;

/**
 * @struct    rtk_bt_mesh_health_fault_clear_t
 * @brief     BLE MESH health fault clear act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t company_id;       /*!< Company ID that identifies the Registered Fault state to clear. */
	bool ack;                  /*!< Whether the server shall acknowledge the message.
                                *   - true: Send Health Fault Clear and the server replies with a Health Fault Status message.
                                *   - false: Send Health Fault Clear Unacknowledged and the server does not reply. */
} rtk_bt_mesh_health_fault_clear_t;

/**
 * @struct    rtk_bt_mesh_health_fault_test_t
 * @brief     BLE MESH health fault test act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t test_id;           /*!< Identifier of the test to perform.
                                *   0x00: Standard test.
                                *   0x01 to 0xFF: Vendor-specific test. */
	uint16_t company_id;       /*!< Company ID that identifies the Health Fault state to test. */
	bool ack;                  /*!< Whether the server shall acknowledge the message.
                                *   - true: Send Health Fault Test and the server replies with a Health Fault Status message.
                                *   - false: Send Health Fault Test Unacknowledged and the server does not reply. */
} rtk_bt_mesh_health_fault_test_t;

/**
 * @struct    rtk_bt_mesh_health_period_get_t
 * @brief     BLE MESH health period get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_health_period_get_t;

/**
 * @struct    rtk_bt_mesh_health_period_set_t
 * @brief     BLE MESH health period set act message structure.
 */
typedef struct {
	uint16_t dst;                   /*!< Destination address of the message. */
	uint16_t app_key_index;         /*!< Index of the application key used to encrypt the message.
                                     *   Range: 0x0000 to 0x0FFF. */
	uint8_t fast_period_divisor;    /*!< Health Fast Period Divisor to set.
                                     *   Range: 0 to 15.
                                     *   The Publish Period of the Health Server model is divided by 2^n, n being this value, while faults are present. */
	bool ack;                       /*!< Whether the server shall acknowledge the message.
                                     *   - true: Send Health Period Set and the server replies with a Health Period Status message.
                                     *   - false: Send Health Period Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_health_period_set_t;

/**
 * @struct    rtk_bt_mesh_health_attn_get_t
 * @brief     BLE MESH health attn get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_health_attn_get_t;

/**
 * @struct    rtk_bt_mesh_health_attn_set_t
 * @brief     BLE MESH health attn set act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t attn;              /*!< Attention Timer to set, in seconds.
                                *   0x00: Off.
                                *   0x01 to 0xFF: On, remaining time in seconds. */
	bool ack;                  /*!< Whether the server shall acknowledge the message.
                                *   - true: Send Health Attention Set and the server replies with a Health Attention Status message.
                                *   - false: Send Health Attention Set Unacknowledged and the server does not reply. */
} rtk_bt_mesh_health_attn_set_t;

/**
 * @struct    rtk_bt_mesh_health_client_status_t
 * @brief     BLE MESH health client model status message structure.
 */
typedef struct {
	uint16_t src;               /*!< Unicast address of the server that sent the Status message. */
	uint8_t test_id;            /*!< Identifier of the most recently performed test.
                                 *   0x00: Standard test.
                                 *   0x01 to 0xFF: Vendor-specific test. */
	uint16_t company_id;        /*!< Company ID that identifies the Health Fault state reported by the server. */
	uint8_t fault_array_len;    /*!< Number of fault values in fault_array.
                                 *   A zero value means no fault is present. */
	uint8_t *fault_array;       /*!< Fault values reported by the server, fault_array_len entries of 1 byte each.
                                 *   This member only marks where the data starts,
                                 *   the fault values are located right after the structure in the event data. */
} rtk_bt_mesh_health_client_status_t;

/**
 * @struct    rtk_bt_mesh_health_client_status_period_t
 * @brief     BLE MESH health client model period status message structure.
 */
typedef struct {
	uint8_t fast_period_divisor;    /*!< Health Fast Period Divisor of the server.
                                     *   Range: 0 to 15. */
} rtk_bt_mesh_health_client_status_period_t;

/**
 * @struct    rtk_bt_mesh_health_client_status_attention_t
 * @brief     BLE MESH health client model attention status message structure.
 */
typedef struct {
	uint8_t attention;    /*!< Attention Timer of the server, in seconds.
                           *   0x00: Off.
                           *   0x01 to 0xFF: On, remaining time in seconds. */
} rtk_bt_mesh_health_client_status_attention_t;

/********************************* Health Server Model *********************************/
/**
 * @enum      rtk_bt_mesh_health_server_model_evt_t
 * @brief     BLE MESH health server model event definition.
 */
typedef enum {
	RTK_BT_MESH_HEALTH_SERVER_MODEL_FAULT_GET = 1,    /*!< Indicate a Health Fault Get message received from a client, with msg
                                                       *   @ref rtk_bt_mesh_health_server_fault_get_t.
                                                       *   The Health Fault Status reply is composed by the stack, the application is only notified.
                                                       *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_HEALTH_SERVER_MODEL_FAULT_CLEAR,      /*!< Indicate a Health Fault Clear message received from a client, with msg
                                                       *   @ref rtk_bt_mesh_health_server_fault_clear_t.
                                                       *   The Health Fault Status reply is composed by the stack, the application is only notified.
                                                       *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_HEALTH_SERVER_MODEL_FAULT_TEST,       /*!< Indicate a Health Fault Test or Health Fault Test Unacknowledged message received from a client, with msg
                                                       *   @ref rtk_bt_mesh_health_server_fault_test_t.
                                                       *   The Health Fault Status reply is composed by the stack, the application is only notified.
                                                       *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_HEALTH_SERVER_MODEL_PERIOD_GET,       /*!< Indicate a Health Period Get message received from a client, with msg
                                                       *   @ref rtk_bt_mesh_health_server_period_get_t.
                                                       *   The Health Period Status reply is composed by the stack, the application is only notified.
                                                       *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_HEALTH_SERVER_MODEL_PERIOD_SET,       /*!< Indicate a Health Period Set message received from a client, with msg
                                                       *   @ref rtk_bt_mesh_health_server_period_set_t.
                                                       *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_HEALTH_SERVER_MODEL_ATTN_GET,         /*!< Indicate a Health Attention Get message received from a client, with msg
                                                       *   @ref rtk_bt_mesh_health_server_attn_get_t.
                                                       *   The Health Attention Status reply is composed by the stack, the application is only notified.
                                                       *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_HEALTH_SERVER_MODEL_ATTN_SET,         /*!< Indicate a Health Attention Set message received from a client, with msg
                                                       *   @ref rtk_bt_mesh_health_server_attn_set_t.
                                                       *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_HEALTH_SERVER_MODEL_CURRENT_GET,      /*!< Indicate the server needs the Current Fault state to compose a Health Current Status message.
                                                       *   The msg is a byte stream laid out as below, at most @ref HEALTH_FAULT_ARRAY_LEN fault values:
                                                       *   - Length of the following data, 1 byte.
                                                       *     It equals the number of fault values plus the Test ID and the Company ID.
                                                       *   - Test ID, 1 byte.
                                                       *   - Company ID, 2 bytes.
                                                       *   - Fault values, 1 byte each.
                                                       *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_HEALTH_SERVER_MODEL_EVT_MAX,          /*!< Reserved. */
} rtk_bt_mesh_health_server_model_evt_t;

/**
 * @enum      rtk_bt_mesh_health_server_model_act_t
 * @brief     BLE MESH health server model act definition.
 */
typedef enum {
	RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_FAULT_REGISTER = 1,         /*!< Register a fault in the Registered Fault and Current Fault states of the Health Server. */
	RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_CURRENT_FAULT_CLEAR,        /*!< Clear a fault from the Current Fault state of the Health Server. */
	RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_CURRENT_FAULT_CLEAR_ALL,    /*!< Clear all faults from the Current Fault state of the Health Server. */
	RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_FAULT_COUNT,                /*!< Get the number of faults in the Current Fault state of the Health Server. */
	RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_CHECK_FAULT_IS_SET,         /*!< Check whether a fault is in the Current Fault state of the Health Server. */
	RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_TEST_SET,                   /*!< Set the self-tests that the Health Server can perform. */
	RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_COMPANY_ID_SET,             /*!< Set the Company ID of the Health Server. */
	RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_MAX,                        /*!< Reserved. */
} rtk_bt_mesh_health_server_model_act_t;

/**
 * @struct    rtk_bt_mesh_health_server_fault_register_act_t
 * @brief     BLE MESH health server fault register act message structure.
 */
typedef struct {
	uint8_t fault;    /*!< Fault value to register.
                       *   Range: 0x00 to 0xFF. */
} rtk_bt_mesh_health_server_fault_register_act_t;

/**
 * @struct    rtk_bt_mesh_health_server_fault_clear_act_t
 * @brief     BLE MESH health server fault clear act message structure.
 */
typedef struct {
	uint8_t fault;    /*!< Fault value to clear from the Current Fault state.
                       *   Range: 0x00 to 0xFF. */
} rtk_bt_mesh_health_server_fault_clear_act_t;

/**
 * @struct    rtk_bt_mesh_health_server_check_fault_act_t
 * @brief     BLE MESH health server fault check act message structure.
 */
typedef struct {
	uint8_t fault;    /*!< Fault value to check.
                       *   Range: 0x00 to 0xFF. */
	bool set;         /*!< Whether the fault is in the Current Fault state.
                       *   - true: The fault is in the Current Fault state.
                       *   - false: The fault is not in the Current Fault state. */
} rtk_bt_mesh_health_server_check_fault_act_t;

/**
 * @struct    rtk_bt_mesh_health_server_count_fault_act_t
 * @brief     BLE MESH health server fault count act message structure.
 */
typedef struct {
	uint8_t fault_count;    /*!< Number of faults in the Current Fault state. */
} rtk_bt_mesh_health_server_count_fault_act_t;

/**
 * @struct    rtk_bt_mesh_health_server_fault_get_t
 * @brief     BLE MESH health get fault evt message structure.
 */
typedef struct {
	uint16_t company_id;     /*!< Company ID that identifies the Health Fault state requested by the client. */
	uint8_t *fault_array;    /*!< Buffer of the fault array, at most @ref HEALTH_FAULT_ARRAY_LEN fault values.
                              *   The application is only notified, it does not need to write it.
                              *   The layout is: Length of the following data (1 byte, the number of fault values plus the Test ID),
                              *   Test ID (1 byte), then the fault values (1 byte each). */
} rtk_bt_mesh_health_server_fault_get_t;

/**
 * @struct    rtk_bt_mesh_health_server_fault_clear_t
 * @brief     BLE MESH health clear fault evt message structure.
 */
typedef struct {
	uint16_t company_id;     /*!< Company ID that identifies the Registered Fault state to clear requested by the client. */
	uint8_t *fault_array;    /*!< Buffer of the fault array, at most @ref HEALTH_FAULT_ARRAY_LEN fault values.
                              *   The application is only notified, it does not need to write it.
                              *   The layout is: Length of the following data (1 byte, the number of fault values plus the Test ID),
                              *   Test ID (1 byte), then the fault values (1 byte each). */
} rtk_bt_mesh_health_server_fault_clear_t;

/**
 * @struct    rtk_bt_mesh_health_server_fault_test_t
 * @brief     BLE MESH health test fault evt message structure.
 */
typedef struct {
	uint8_t test_id;         /*!< Identifier of the test requested by the client.
                              *   0x00: Standard test.
                              *   0x01 to 0xFF: Vendor-specific test. */
	uint16_t company_id;     /*!< Company ID that identifies the Health Fault state to test requested by the client. */
	uint8_t *fault_array;    /*!< Buffer of the fault array, at most @ref HEALTH_FAULT_ARRAY_LEN fault values.
                              *   The application is only notified, it does not need to write it.
                              *   The layout is: Length of the following data (1 byte, the number of fault values plus the Test ID),
                              *   Test ID (1 byte), then the fault values (1 byte each). */
} rtk_bt_mesh_health_server_fault_test_t;

/**
 * @struct    rtk_bt_mesh_health_server_period_get_t
 * @brief     BLE MESH health period evt get message structure.
 */
typedef struct {
	uint8_t *fast_period_divisor;    /*!< Buffer of the Health Fast Period Divisor.
                                      *   The application is only notified, it does not need to write it.
                                      *   Range: 0 to 15. */
} rtk_bt_mesh_health_server_period_get_t;

/**
 * @struct    rtk_bt_mesh_health_server_period_set_t
 * @brief     BLE MESH health period evt set message structure.
 */
typedef struct {
	uint8_t fast_period_divisor;    /*!< Health Fast Period Divisor requested by the client. */
} rtk_bt_mesh_health_server_period_set_t;

/**
 * @struct    rtk_bt_mesh_health_server_attn_get_t
 * @brief     BLE MESH health attention timer get evt message structure.
 */
typedef struct {
	uint8_t *attn;    /*!< Buffer of the Attention Timer, in seconds.
                       *   The application is only notified, it does not need to write it. */
} rtk_bt_mesh_health_server_attn_get_t;

/**
 * @struct    rtk_bt_mesh_health_server_attn_set_t
 * @brief     BLE MESH health attention timer set evt message structure.
 */
typedef struct {
	uint8_t attn;    /*!< Attention Timer requested by the client, in seconds.
                      *   0x00: Off.
                      *   0x01 to 0xFF: On, remaining time in seconds. */
} rtk_bt_mesh_health_server_attn_set_t;

/**
 * @typedef   rtk_bt_mesh_health_server_test_cb_t
 * @brief     BLE MESH health server test callback definition, called when the server is requested to perform the test.
 * @param[in] pmodel_info: Health Server model information.
 * @param[in] company_id: Company ID that identifies the Health Fault state to test.
 * @param[in] test_id: Identifier of the test to perform.
 */
typedef void (*rtk_bt_mesh_health_server_test_cb_t)(const void *pmodel_info, uint16_t company_id,
													uint8_t test_id);

/**
 * @struct    rtk_bt_mesh_health_server_test_t
 * @brief     BLE MESH health test setting message structure.
 */
typedef struct {
	uint8_t test_id;                                /*!< Identifier of the test. */
	rtk_bt_mesh_health_server_test_cb_t test_cb;    /*!< Callback called when the server is requested to perform this test. */
} rtk_bt_mesh_health_server_test_t;

/**
 * @struct    rtk_bt_mesh_health_server_test_set_t
 * @brief     BLE MESH health test set act message structure.
 */
typedef struct {
	uint8_t num_tests;    /*!< Number of tests in test. */
	void *test;           /*!< Array of the tests that the Health Server can perform, num_tests entries of rtk_bt_mesh_health_server_test_t.
                           *   The array is used by reference, it must stay valid while the Health Server is in use. */
} rtk_bt_mesh_health_server_test_set_t;

/**
 * @struct    rtk_bt_mesh_health_server_company_id_set_t
 * @brief     BLE MESH health company id set act message structure.
 */
typedef struct {
	uint16_t company_id;    /*!< Company ID of the Health Server, which identifies its Health Fault state. */
} rtk_bt_mesh_health_server_company_id_set_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  ble_mesh_health_model BT LE Mesh Health Model APIs
 * @brief     BT LE Mesh Health Model related function APIs
 * @ingroup   ble_mesh_api_group
 * @{
 */

/**
 * @brief     Send a Health Fault Get message to a Health Server to read its Registered Fault state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_STATUS.
 * @param[in] health_fault_get: Health Fault Get message parameters.
 *                              Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_health_fault_get(rtk_bt_mesh_health_fault_get_t *health_fault_get);

/**
 * @brief     Send a Health Fault Clear or Health Fault Clear Unacknowledged message to a Health Server to clear its Registered Fault state.
 *            Only when ack is true, the server replies with a Health Fault Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_STATUS.
 * @param[in] health_fault_clear: Health Fault Clear or Health Fault Clear Unacknowledged message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_health_fault_clear(rtk_bt_mesh_health_fault_clear_t *health_fault_clear);

/**
 * @brief     Send a Health Fault Test or Health Fault Test Unacknowledged message to a Health Server to invoke a self-test.
 *            Only when ack is true, the server replies with a Health Fault Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_STATUS.
 * @param[in] health_fault_test: Health Fault Test or Health Fault Test Unacknowledged message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_health_fault_test(rtk_bt_mesh_health_fault_test_t *health_fault_test);

/**
 * @brief     Send a Health Period Get message to a Health Server to read its Health Fast Period Divisor state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_HEALTH_CLIENT_MODEL_PERIOD_STATUS.
 * @param[in] health_period_get: Health Period Get message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_health_period_get(rtk_bt_mesh_health_period_get_t *health_period_get);

/**
 * @brief     Send a Health Period Set or Health Period Set Unacknowledged message to a Health Server to set its Health Fast Period Divisor state.
 *            Only when ack is true, the server replies with a Health Period Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_HEALTH_CLIENT_MODEL_PERIOD_STATUS.
 * @param[in] health_period_set: Health Period Set or Health Period Set Unacknowledged message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_health_period_set(rtk_bt_mesh_health_period_set_t *health_period_set);

/**
 * @brief     Send a Health Attention Get message to a Health Server to read its Attention Timer state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_HEALTH_CLIENT_MODEL_ATTN_STATUS.
 * @param[in] health_attn_get: Health Attention Get message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_health_attn_get(rtk_bt_mesh_health_attn_get_t *health_attn_get);

/**
 * @brief     Send a Health Attention Set or Health Attention Set Unacknowledged message to a Health Server to set its Attention Timer state.
 *            Only when ack is true, the server replies with a Health Attention Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_HEALTH_CLIENT_MODEL_ATTN_STATUS.
 * @param[in] health_attn_set: Health Attention Set or Health Attention Set Unacknowledged message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_health_attn_set(rtk_bt_mesh_health_attn_set_t *health_attn_set);

/**
 * @brief     Register a fault in the Registered Fault state and the Current Fault state of the Health Server on this node.
 *            The Health Server also publishes a Health Current Status message.
 * @param[in] health_fault_register: Fault to register.
 *                                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_health_server_fault_register(rtk_bt_mesh_health_server_fault_register_act_t *health_fault_register);

/**
 * @brief     Clear a fault from the Current Fault state of the Health Server on this node.
 * @param[in] health_fault_clear: Fault to clear.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_health_server_fault_clear(rtk_bt_mesh_health_server_fault_clear_act_t *health_fault_clear);

/**
 * @brief     Clear all faults from the Current Fault state of the Health Server on this node.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_health_server_fault_clear_all(void);

/**
 * @brief     Get the number of faults in the Current Fault state of the Health Server on this node.
 * @param[out] fault_count: Number of faults.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_health_server_fault_count(rtk_bt_mesh_health_server_count_fault_act_t *fault_count);

/**
 * @brief     Check whether a fault is in the Current Fault state of the Health Server on this node.
 * @param[in,out] health_fault_check: Fault to check and the check result.
 *                                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_health_server_fault_check(rtk_bt_mesh_health_server_check_fault_act_t *health_fault_check);

/**
 * @brief     Set the self-tests that the Health Server on this node can perform.
 *            The tests are used by reference, so the test array must stay valid while the Health Server is in use.
 * @param[in] test_set: Self-test parameters.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_health_server_set_tests(rtk_bt_mesh_health_server_test_set_t *test_set);

/**
 * @brief     Set the Company ID of the Health Server on this node.
 * @param[in] company_id_set: Company ID parameters.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_health_server_set_company_id(rtk_bt_mesh_health_server_company_id_set_t *company_id_set);

/**
 * @}
 */

#endif /* __RTK_BT_MESH_HEALTH_MODEL_H__ */
