/**
 * @file      rtk_bt_mesh_datatrans_model.h
 * @author    pico_zhai@realsil.com.cn
 * @brief     Bluetooth LE MESH datatrans model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_API_DATATRANS_MODEL_H__
#define __RTK_BT_MESH_API_DATATRANS_MODEL_H__

#include <stdint.h>

/** @addtogroup BT_Constants BT Constants
 * @{
 */
/** @addtogroup BT_Mesh_Constants BT Mesh Constants
 * @{
 */
/** @addtogroup BT_Mesh_Datatrans_Model_Constants BT Mesh Datatrans Model Constants
 * @{
 */
/**
 * @def       DATA_TRANS_DATA_MAX_LEN
 * @brief     Maximum length in bytes of the data of a Datatrans message.
 * @note
 *            - The value shall not exceed 258, which is the maximum on AmebaLite for the single core case.
 */
#define DATA_TRANS_DATA_MAX_LEN    0xff
/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

/*********************************** Datatrans Model ***********************************/
/** @addtogroup BT_Types BT Types
 * @{
 */
/** @addtogroup BT_Mesh_Types BT Mesh Types
 * @{
 */
/** @addtogroup BT_Mesh_Datatrans_Model_Types BT Mesh Datatrans Model Types
 * @{
 */
/**
 * @enum      rtk_bt_mesh_datatrans_model_act_t
 * @brief     BLE MESH datatrans model act definition.
 */
typedef enum {
	RTK_BT_MESH_DATATRANS_MODEL_ACT_READ = 1,    /*!< Send a Datatrans Read message to the remote device. */
	RTK_BT_MESH_DATATRANS_MODEL_ACT_WRITE,       /*!< Send a Datatrans Write or Write Unacknowledged message to the remote device. */
	RTK_BT_MESH_DATATRANS_MODEL_ACT_MAX,         /*!< Reserved. */
} rtk_bt_mesh_datatrans_model_act_t;

/**
 * @enum      rtk_bt_mesh_datatrans_model_evt_t
 * @brief     BLE MESH datatrans model evt definition.
 */
typedef enum {
	RTK_BT_MESH_DATATRANS_MODEL_EVT_SERVER_WRITE = 1,       /*!< Indicate a Datatrans Write or Write Unacknowledged message received from a remote device, with msg
                                                             *   @ref rtk_bt_mesh_datatrans_server_write_event_t.
                                                             *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_DATATRANS_MODEL_EVT_SERVER_READ,            /*!< Indicate a Datatrans Read message received from a remote device, with msg
                                                             *   @ref rtk_bt_mesh_datatrans_server_read_event_t.
                                                             *   The application shall write the data to be read into the msg.
                                                             *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_DATATRANS_MODEL_EVT_CLIENT_WRITE_STATUS,    /*!< Indicate a Datatrans Status message received from a remote device, with msg
                                                             *   @ref rtk_bt_mesh_datatrans_client_write_event_t.
                                                             *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_DATATRANS_MODEL_EVT_CLIENT_READ_RESULT,     /*!< Indicate a Datatrans Data message received from a remote device, with msg
                                                             *   @ref rtk_bt_mesh_datatrans_client_read_event_t.
                                                             *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_DATATRANS_MODEL_EVT_MAX,                    /*!< Reserved. */
} rtk_bt_mesh_datatrans_model_evt_t;

/**
 * @typedef   rtk_bt_mesh_datatrans_stat_t
 * @brief     BLE MESH datatrans server model write status event data structure
 */
enum {
	RTK_BT_MESH_DATATRANS_SUCCESS,    /*!< The data was written successfully. */
	RTK_BT_MESH_DATATRANS_FAIL,       /*!< The data was not written. */
};
typedef uint8_t rtk_bt_mesh_datatrans_stat_t;

/**
 * @struct    rtk_bt_mesh_datatrans_client_write_api_t
 * @brief     BLE MESH datatrans client model write api data structure.
 */
typedef struct {
	uint16_t dst;                             /*!< Destination address of the message. */
	uint16_t app_key_index;                   /*!< Index of the application key used to encrypt the message.
                                               *   Range: 0x0000 to 0x0FFF. */
	uint16_t data_len;                        /*!< Length in bytes of the data to write.
                                               *   Range: 0 to @ref DATA_TRANS_DATA_MAX_LEN. */
	uint8_t data[DATA_TRANS_DATA_MAX_LEN];    /*!< Data to write, with data_len valid bytes. */
	bool ack;                                 /*!< Whether the remote device shall acknowledge the message.
                                               *   - true: Send Datatrans Write and the remote device replies with a Datatrans Status message.
                                               *   - false: Send Datatrans Write Unacknowledged and the remote device does not reply. */
} rtk_bt_mesh_datatrans_client_write_api_t;

/**
 * @struct    rtk_bt_mesh_datatrans_client_read_api_t
 * @brief     BLE MESH datatrans client model read api data structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t read_len;         /*!< Length in bytes of the data to read.
                                *   Range: 0 to @ref DATA_TRANS_DATA_MAX_LEN. */
} rtk_bt_mesh_datatrans_client_read_api_t;

/**
 * @struct    rtk_bt_mesh_datatrans_client_write_event_t
 * @brief     BLE MESH datatrans client model write result event data structure.
 */
typedef struct {
	uint16_t src;                           /*!< Unicast address of the remote device that sent the Status message. */
	rtk_bt_mesh_datatrans_stat_t status;    /*!< Status code of the write operation reported by the remote device. */
	uint16_t written_len;                   /*!< Length in bytes of the data written by the remote device. */
} rtk_bt_mesh_datatrans_client_write_event_t;

/**
 * @struct    rtk_bt_mesh_datatrans_client_read_event_t
 * @brief     BLE MESH datatrans client model read event data structure.
 */
typedef struct {
	uint16_t src;                             /*!< Unicast address of the remote device that sent the Data message. */
	uint8_t data_len;                         /*!< Length in bytes of the data reported by the remote device.
                                               *   Note: The remote device shall not send more than @ref DATA_TRANS_DATA_MAX_LEN bytes of data. */
	uint8_t data[DATA_TRANS_DATA_MAX_LEN];    /*!< Data reported by the remote device, with data_len valid bytes. */
} rtk_bt_mesh_datatrans_client_read_event_t;

/**
 * @struct    rtk_bt_mesh_datatrans_server_read_event_t
 * @brief     BLE MESH datatrans server model read event data structure.
 */
typedef struct {
	uint16_t data_len;    /*!< Length in bytes of the data requested by the remote device.
                           *   Range: 0 to @ref DATA_TRANS_DATA_MAX_LEN. */
	uint8_t *data;        /*!< Output: the application writes the data to read here, with data_len bytes.
                           *   Must not be NULL. */
} rtk_bt_mesh_datatrans_server_read_event_t;

/**
 * @struct    rtk_bt_mesh_datatrans_server_write_event_t
 * @brief     BLE MESH datatrans server model write event data structure.
 */
typedef struct {
	rtk_bt_mesh_datatrans_stat_t status;      /*!< Status code of the write operation. */
	uint8_t data_len;                         /*!< Length in bytes of the data written by the remote device.
                                               *   Range: 0 to @ref DATA_TRANS_DATA_MAX_LEN. */
	uint8_t data[DATA_TRANS_DATA_MAX_LEN];    /*!< Data written by the remote device, with data_len valid bytes. */
} rtk_bt_mesh_datatrans_server_write_event_t;

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
/** @addtogroup BT_Mesh_Datatrans_Model_Functions BT Mesh Datatrans Model Functions
 * @{
 */

/**
 * @brief     Send a Datatrans Write or Write Unacknowledged message to a remote device to write data.
 *            Only when ack is true, the remote device replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DATATRANS_MODEL_EVT_CLIENT_WRITE_STATUS.
 * @param[in] client_write: Datatrans Write message parameters.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_datatrans_model_write(rtk_bt_mesh_datatrans_client_write_api_t *client_write);

/**
 * @brief     Send a Datatrans Read message to a remote device to read data.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DATATRANS_MODEL_EVT_CLIENT_READ_RESULT.
 * @param[in] client_read: Datatrans Read message parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_datatrans_model_read(rtk_bt_mesh_datatrans_client_read_api_t *client_read);

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

#endif /* __RTK_BT_MESH_API_DATATRANS_MODEL_H__ */
