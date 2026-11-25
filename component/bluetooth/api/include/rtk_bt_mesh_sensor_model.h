/**
 * @file      rtk_bt_mesh_sensor_model.h
 * @author    quinn_yang@realsil.com.cn
 * @brief     Bluetooth LE MESH sensor client and server model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */


#ifndef __RTK_BT_MESH_SENSOR_MODEL_H__
#define __RTK_BT_MESH_SENSOR_MODEL_H__

#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_generic_onoff_model.h>

/** @addtogroup BT_Constants BT Constants
 * @{
 */
/** @addtogroup BT_Mesh_Constants BT Mesh Constants
 * @{
 */
/** @addtogroup BT_Mesh_Sensor_Model_Constants BT Mesh Sensor Model Constants
 * @{
 */

/**
 * @def       SENSOR_TRIGGER_DATA_MAX_LEN
 * @brief     Capacity in bytes of each of the Status Trigger Delta Down and Status Trigger Delta Up values of a sensor cadence.
 */
#define SENSOR_TRIGGER_DATA_MAX_LEN    0x10

/**
 * @def       SENSOR_CADENCE_DATA_MAX_LEN
 * @brief     Maximum length in bytes of the raw value of a sensor cadence, and capacity in bytes of each of the Fast Cadence Low and Fast Cadence High values.
 */
#define SENSOR_CADENCE_DATA_MAX_LEN    0x10

/**
 * @def       SENSOR_SETTING_DATA_MAX_LEN
 * @brief     Maximum length in bytes of the raw value of a sensor setting.
 */
#define SENSOR_SETTING_DATA_MAX_LEN    0x10

/**
 * @def       SENSOR_SETTINGS_DATA_MAX_LEN
 * @brief     Maximum number of Sensor Setting Property IDs of a sensor.
 */
#define SENSOR_SETTINGS_DATA_MAX_LEN   0x10

/**
 * @def       SENSOR_COLUMN_DATA_MAX_LEN
 * @brief     Maximum length in bytes of the raw value that identifies a column of a sensor series.
 */
#define SENSOR_COLUMN_DATA_MAX_LEN     0x10

/**
 * @def       SENSOR_GET_DATA_MAX_LEN
 * @brief     Maximum number of Sensor Descriptors that can be reported, and maximum length in bytes of the sensor data that can be reported.
 */
#define SENSOR_GET_DATA_MAX_LEN        0x10

/**
 * @def       SENSOR_GET_COLUMN_MAX_LEN
 * @brief     Maximum length in bytes of the Sensor Series Column data that can be reported.
 */
#define SENSOR_GET_COLUMN_MAX_LEN      0x10

/**
 * @def       SENSOR_GET_SERIES_MAX_LEN
 * @brief     Maximum length in bytes of the raw value that identifies a column of a sensor series.
 */
#define SENSOR_GET_SERIES_MAX_LEN      0x10

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
/** @addtogroup BT_Mesh_Sensor_Model_Types BT Mesh Sensor Model Types
 * @{
 */
/************************************* Sensor Model ************************************/
/**
 * @enum      rtk_bt_mesh_sensor_setting_access_t
 * @brief     BLE MESH sensor setting access definition.
 */
typedef enum {
	RTK_BT_MESH_SENSOR_SETTING_ACCESS_READ_ONLY = 0x01,     /*!< The setting can be read. */
	RTK_BT_MESH_SENSOR_SETTING_ACCESS_READ_WRITE = 0x03,    /*!< The setting can be read and written. */
} rtk_bt_mesh_sensor_setting_access_t;

/**
 * @struct    rtk_bt_mesh_sensor_cadence_t
 * @brief     BLE MESH sensor cadence state structure.
 */
typedef struct {
	uint8_t raw_value_len;                     /*!< Length in bytes of the raw value of the sensor, which is also the length of fast_cadence_low and fast_cadence_high. */
	uint8_t fast_cadence_period_divisor: 7;    /*!< Divisor of the publish period used while the measured value is inside the fast cadence range.
                                                *   The divisor is 2^n.
                                                *   Range: 0 to 15, other values are Prohibited. */
	uint8_t status_trigger_type: 1;            /*!< Unit and format of status_trigger_delta_down and status_trigger_delta_up.
                                                *   0: The format is defined by the Format Type of the characteristic that the Sensor Property ID references.
                                                *   1: Unitless, uint16, a percentage change with a resolution of 0.01 percent. */
	void *status_trigger_delta_down;           /*!< Negative change of the measured value that triggers a Sensor Status message.
                                                *   Its length is raw_value_len if status_trigger_type is 0, otherwise 2 bytes.
                                                *   If status_trigger_type is 0, the value is interpreted as defined by the Sensor Property ID.
                                                *   If status_trigger_type is 1, the change in percent is the value divided by 100.
                                                *   A zero value triggers a Sensor Status message for any measured change. */
	void *status_trigger_delta_up;             /*!< Positive change of the measured value that triggers a Sensor Status message.
                                                *   Its length is raw_value_len if status_trigger_type is 0, otherwise 2 bytes.
                                                *   If status_trigger_type is 0, the value is interpreted as defined by the Sensor Property ID.
                                                *   If status_trigger_type is 1, the change in percent is the value divided by 100.
                                                *   A zero value triggers a Sensor Status message for any measured change. */
	uint8_t status_min_interval;               /*!< Minimum interval between two consecutive Sensor Status messages.
                                                *   Time = 2^N ms.
                                                *   Range: 0 to 26, other values are Prohibited. */
	void *fast_cadence_low;                    /*!< Low boundary of the range of measured values where the publishing cadence is increased
                                                *   as defined by fast_cadence_period_divisor.
                                                *   The value is interpreted as defined by the Sensor Property ID, with raw_value_len bytes.
                                                *   It may be higher than fast_cadence_high,
                                                *   in which case the increased cadence applies outside the range between them. */
	void *fast_cadence_high;                   /*!< High boundary of the range of measured values where the publishing cadence is increased
                                                *   as defined by fast_cadence_period_divisor.
                                                *   The value is interpreted as defined by the Sensor Property ID, with raw_value_len bytes.
                                                *   It may be lower than fast_cadence_low,
                                                *   in which case the increased cadence applies outside the range between them. */
} _PACKED4_ rtk_bt_mesh_sensor_cadence_t;

/**
 * @struct    rtk_bt_mesh_sensor_setting_t
 * @brief     BLE MESH sensor setting state structure.
 */
typedef struct {
	uint16_t setting_property_id;                          /*!< Sensor Setting Property ID of the setting within the sensor.
                                                            *   Range: 0x0001 to 0xFFFF. */
	rtk_bt_mesh_sensor_setting_access_t setting_access;    /*!< Access rights of the setting. */
	uint8_t setting_raw_len;                               /*!< Length in bytes of the raw value of the setting. */
	void *setting_raw;                                     /*!< Raw value of the setting, with setting_raw_len bytes. */
} _PACKED4_ rtk_bt_mesh_sensor_setting_t;

/**
 * @enum      rtk_bt_mesh_sensor_sampling_function_t
 * @brief     BLE MESH sensor sampling function definition.
 */
typedef enum {
	RTK_BT_MESH_SENSOR_SAMPLING_UNSPECIFIED,        /*!< Unspecified. */
	RTK_BT_MESH_SENSOR_SAMPLING_INSTANTANEOUS,      /*!< Instantaneous. */
	RTK_BT_MESH_SENSOR_SAMPLING_ARITHMETIC_MEAN,    /*!< Arithmetic mean. */
	RTK_BT_MESH_SENSOR_SAMPLING_RMS,                /*!< Root mean square. */
	RTK_BT_MESH_SENSOR_SAMPLING_MAXIMUM,            /*!< Maximum. */
	RTK_BT_MESH_SENSOR_SAMPLING_MINIMUM,            /*!< Minimum. */
	RTK_BT_MESH_SENSOR_SAMPLING_ACCUMULATED,        /*!< Accumulated, a cumulative moving average. */
	RTK_BT_MESH_SENSOR_SAMPLING_COUNT,              /*!< Count of a discrete variable. */
} rtk_bt_mesh_sensor_sampling_function_t;

/**
 * @struct    rtk_bt_mesh_sensor_descriptor_t
 * @brief     BLE MESH sensor descriptor state structure.
 */
typedef struct {
	uint16_t property_id;               /*!< Sensor Property ID of the sensor.
                                         *   Range: 0x0001 to 0xFFFF. */
	uint32_t positive_tolerance: 12;    /*!< Magnitude of a possible positive error of the measurement.
                                         *   0x000: Unspecified.
                                         *   0x001 to 0xFFF: Possible positive error [%] = 100 * positive_tolerance / 4095. */
	uint32_t negative_tolerance: 12;    /*!< Magnitude of a possible negative error of the measurement.
                                         *   0x000: Unspecified.
                                         *   0x001 to 0xFFF: Possible negative error [%] = 100 * negative_tolerance / 4095. */
	uint32_t sampling_function: 8;      /*!< Averaging operation or type of sampling function applied to the measured value.
                                         *   0x00: Unspecified.
                                         *   0x01: Instantaneous.
                                         *   0x02: Arithmetic Mean.
                                         *   0x03: RMS.
                                         *   0x04: Maximum.
                                         *   0x05: Minimum.
                                         *   0x06: Accumulated.
                                         *   0x07: Count.
                                         *   0x08 to 0xFF: Reserved for future use. */
	uint8_t measurement_period;         /*!< Averaging time span, accumulation time or measurement period over which the measurement is taken.
                                         *   0x00: Not applicable.
                                         *   0x01 to 0xFF: Time period = 1.1^(N - 64) s. */
	uint8_t update_interval;            /*!< Interval between internal updates of the measurement.
                                         *   0x00: Not applicable.
                                         *   0x01 to 0xFF: Update interval = 1.1^(N - 64) s. */
} _PACKED4_ rtk_bt_mesh_sensor_descriptor_t;

/********************************* Sensor Client Model *********************************/
/**
 * @enum      rtk_bt_mesh_sensor_client_model_act_t
 * @brief     BLE MESH sensor client model act definition.
 */
typedef enum {
	RTK_BT_MESH_SENSOR_DESCRIPTOR_CLIENT_MODEL_GET = 1,    /*!< Send a Sensor Descriptor Get message to the server. */
	RTK_BT_MESH_SENSOR_CADENCE_CLIENT_MODEL_GET,           /*!< Send a Sensor Cadence Get message to the server. */
	RTK_BT_MESH_SENSOR_CADENCE_CLIENT_MODEL_SET,           /*!< Send a Sensor Cadence Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_SENSOR_SETTINGS_CLIENT_MODEL_GET,          /*!< Send a Sensor Settings Get message to the server. */
	RTK_BT_MESH_SENSOR_SETTING_CLIENT_MODEL_GET,           /*!< Send a Sensor Setting Get message to the server. */
	RTK_BT_MESH_SENSOR_SETTING_CLIENT_MODEL_SET,           /*!< Send a Sensor Setting Set or Set Unacknowledged message to the server. */
	RTK_BT_MESH_SENSOR_CLIENT_MODEL_GET,                   /*!< Send a Sensor Get message to the server. */
	RTK_BT_MESH_SENSOR_COLUMN_CLIENT_MODEL_GET,            /*!< Send a Sensor Column Get message to the server. */
	RTK_BT_MESH_SENSOR_SERIES_CLIENT_MODEL_GET,            /*!< Send a Sensor Series Get message to the server. */
	RTK_BT_MESH_SENSOR_CLIENT_MODEL_ACT_MAX,               /*!< Reserved. */
} rtk_bt_mesh_sensor_client_model_act_t;

/**
 * @enum      rtk_bt_mesh_sensor_client_model_evt_t
 * @brief     BLE MESH sensor client model event definition.
 */
typedef enum {
	RTK_BT_MESH_SENSOR_DESCRIPTOR_CLIENT_MODEL_STATUS = 1,    /*!< Indicate Sensor Descriptor Status message received from the server, with msg
                                                               *   @ref rtk_bt_mesh_sensor_client_status_descriptor_t. */
	RTK_BT_MESH_SENSOR_CADENCE_CLIENT_MODEL_STATUS,           /*!< Indicate Sensor Cadence Status message received from the server, with msg
                                                               *   @ref rtk_bt_mesh_sensor_client_status_cadence_t. */
	RTK_BT_MESH_SENSOR_SETTINGS_CLIENT_MODEL_STATUS,          /*!< Indicate Sensor Settings Status message received from the server, with msg
                                                               *   @ref rtk_bt_mesh_sensor_client_status_settings_t. */
	RTK_BT_MESH_SENSOR_SETTING_CLIENT_MODEL_STATUS,           /*!< Indicate Sensor Setting Status message received from the server, with msg
                                                               *   @ref rtk_bt_mesh_sensor_client_status_setting_t. */
	RTK_BT_MESH_SENSOR_CLIENT_MODEL_STATUS,                   /*!< Indicate Sensor Status message received from the server, with msg
                                                               *   @ref rtk_bt_mesh_sensor_client_status_t. */
	RTK_BT_MESH_SENSOR_COLUMN_CLIENT_MODEL_STATUS,            /*!< Indicate Sensor Column Status message received from the server, with msg
                                                               *   @ref rtk_bt_mesh_sensor_client_status_column_t. */
	RTK_BT_MESH_SENSOR_SERIES_CLIENT_MODEL_STATUS,            /*!< Indicate Sensor Series Status message received from the server, with msg
                                                               *   @ref rtk_bt_mesh_sensor_client_status_series_t. */
	RTK_BT_MESH_SENSOR_CLIENT_MODEL_EVT_MAX,                  /*!< Reserved. */
} rtk_bt_mesh_sensor_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_sensor_descriptor_get_t
 * @brief     BLE MESH sensor descriptor get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;      /*!< Sensor Property ID of the sensor whose Sensor Descriptor is requested.
                                *   Range: 0x0001 to 0xFFFF.
                                *   0x0000: Request the Sensor Descriptors of all sensors of the element. */
} rtk_bt_mesh_sensor_descriptor_get_t;

/**
 * @struct    rtk_bt_mesh_sensor_cadence_get_t
 * @brief     BLE MESH sensor cadence get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;      /*!< Sensor Property ID of the sensor.
                                *   Range: 0x0001 to 0xFFFF. */
} rtk_bt_mesh_sensor_cadence_get_t;

/**
 * @struct    rtk_bt_mesh_sensor_cadence_set_t
 * @brief     BLE MESH sensor cadence set act message structure.
 */
typedef struct {
	uint16_t dst;                                         /*!< Destination address of the message. */
	uint16_t app_key_index;                               /*!< Index of the application key used to encrypt the message.
                                                           *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;                                 /*!< Sensor Property ID of the sensor.
                                                           *   Range: 0x0001 to 0xFFFF. */
	bool ack;                                             /*!< Whether the server shall acknowledge the message.
                                                           *   - true: Send Sensor Cadence Set and the server replies with a Sensor Cadence Status message.
                                                           *   - false: Send Sensor Cadence Set Unacknowledged and the server does not reply. */
	uint8_t fast_cadence_period_divisor: 7;               /*!< Divisor of the publish period used while the measured value is inside the fast cadence range.
                                                           *   The divisor is 2^n.
                                                           *   Range: 0 to 15, other values are Prohibited. */
	uint8_t status_trigger_type: 1;                       /*!< Unit and format of trigger_down and trigger_up.
                                                           *   0: The format is defined by the Format Type of the characteristic that the Sensor Property ID references.
                                                           *   1: Unitless, uint16, a percentage change with a resolution of 0.01 percent. */
	uint8_t raw_value_len;                                /*!< Length in bytes of the raw value of the sensor, which is also the length of cadence_low and cadence_high.
                                                           *   Range: 1 to @ref SENSOR_CADENCE_DATA_MAX_LEN. */
	uint8_t trigger_down[SENSOR_TRIGGER_DATA_MAX_LEN];    /*!< Negative change of the measured value that triggers a Sensor Status message.
                                                           *   Its valid length is raw_value_len if status_trigger_type is 0, otherwise 2 bytes.
                                                           *   If status_trigger_type is 0, the value is interpreted as defined by the Sensor Property ID.
                                                           *   If status_trigger_type is 1, the change in percent is the value divided by 100.
                                                           *   A zero value triggers a Sensor Status message for any measured change. */
	uint8_t trigger_up[SENSOR_TRIGGER_DATA_MAX_LEN];      /*!< Positive change of the measured value that triggers a Sensor Status message.
                                                           *   Its valid length is raw_value_len if status_trigger_type is 0, otherwise 2 bytes.
                                                           *   If status_trigger_type is 0, the value is interpreted as defined by the Sensor Property ID.
                                                           *   If status_trigger_type is 1, the change in percent is the value divided by 100.
                                                           *   A zero value triggers a Sensor Status message for any measured change. */
	uint8_t status_min_interval;                          /*!< Minimum interval between two consecutive Sensor Status messages.
                                                           *   Time = 2^N ms.
                                                           *   Range: 0 to 26, other values are Prohibited. */
	uint8_t cadence_low[SENSOR_CADENCE_DATA_MAX_LEN];     /*!< Low boundary of the range of measured values where the publishing cadence is increased
                                                           *   as defined by fast_cadence_period_divisor.
                                                           *   The value is interpreted as defined by the Sensor Property ID, with raw_value_len valid bytes.
                                                           *   It may be higher than cadence_high,
                                                           *   in which case the increased cadence applies outside the range between them. */
	uint8_t cadence_high[SENSOR_CADENCE_DATA_MAX_LEN];    /*!< High boundary of the range of measured values where the publishing cadence is increased
                                                           *   as defined by fast_cadence_period_divisor.
                                                           *   The value is interpreted as defined by the Sensor Property ID, with raw_value_len valid bytes.
                                                           *   It may be lower than cadence_low,
                                                           *   in which case the increased cadence applies outside the range between them. */
} rtk_bt_mesh_sensor_cadence_set_t;

/**
 * @struct    rtk_bt_mesh_sensor_settings_get_t
 * @brief     BLE MESH sensor settings get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;      /*!< Sensor Property ID of the sensor.
                                *   Range: 0x0001 to 0xFFFF. */
} rtk_bt_mesh_sensor_settings_get_t;

/**
 * @struct    rtk_bt_mesh_sensor_setting_get_t
 * @brief     BLE MESH sensor setting get act message structure.
 */
typedef struct {
	uint16_t dst;                    /*!< Destination address of the message. */
	uint16_t app_key_index;          /*!< Index of the application key used to encrypt the message.
                                      *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;            /*!< Sensor Property ID of the sensor.
                                      *   Range: 0x0001 to 0xFFFF. */
	uint16_t setting_property_id;    /*!< Sensor Setting Property ID of the setting within the sensor.
                                      *   Range: 0x0001 to 0xFFFF. */
} rtk_bt_mesh_sensor_setting_get_t;

/**
 * @struct    rtk_bt_mesh_sensor_setting_set_t
 * @brief     BLE MESH sensor setting set act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;                                /*!< Sensor Property ID of the sensor.
                                                          *   Range: 0x0001 to 0xFFFF. */
	uint16_t setting_property_id;                        /*!< Sensor Setting Property ID of the setting within the sensor.
                                                          *   Range: 0x0001 to 0xFFFF. */
	bool ack;                                            /*!< Whether the server shall acknowledge the message.
                                                          *   - true: Send Sensor Setting Set and the server replies with a Sensor Setting Status message.
                                                          *   - false: Send Sensor Setting Set Unacknowledged and the server does not reply. */
	uint8_t setting_raw_len;                             /*!< Length in bytes of the raw value of the setting.
                                                          *   Range: 1 to @ref SENSOR_SETTING_DATA_MAX_LEN. */
	uint8_t setting_raw[SENSOR_SETTING_DATA_MAX_LEN];    /*!< Raw value of the setting, with setting_raw_len valid bytes. */
} rtk_bt_mesh_sensor_setting_set_t;

/**
 * @struct    rtk_bt_mesh_sensor_get_t
 * @brief     BLE MESH sensor get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Destination address of the message. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;      /*!< Sensor Property ID of the sensor whose Sensor Data is requested.
                                *   Range: 0x0001 to 0xFFFF.
                                *   0x0000: Request the Sensor Data of all sensors of the element. */
} rtk_bt_mesh_sensor_get_t;

/**
 * @struct    rtk_bt_mesh_sensor_column_get_t
 * @brief     BLE MESH sensor get column act message structure.
 */
typedef struct {
	uint16_t dst;                                       /*!< Destination address of the message. */
	uint16_t app_key_index;                             /*!< Index of the application key used to encrypt the message.
                                                         *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;                               /*!< Sensor Property ID of the sensor.
                                                         *   Range: 0x0001 to 0xFFFF. */
	uint8_t raw_value_x_len;                            /*!< Length in bytes of raw_value_x.
                                                         *   Range: 1 to @ref SENSOR_COLUMN_DATA_MAX_LEN. */
	uint8_t raw_value_x[SENSOR_COLUMN_DATA_MAX_LEN];    /*!< Raw value identifying the column, with raw_value_x_len valid bytes. */
} rtk_bt_mesh_sensor_column_get_t;

/**
 * @struct    rtk_bt_mesh_sensor_series_get_t
 * @brief     BLE MESH sensor get series act message structure.
 */
typedef struct {
	uint16_t dst;                                        /*!< Destination address of the message. */
	uint16_t app_key_index;                              /*!< Index of the application key used to encrypt the message.
                                                          *   Range: 0x0000 to 0x0FFF. */
	uint16_t property_id;                                /*!< Sensor Property ID of the sensor.
                                                          *   Range: 0x0001 to 0xFFFF. */
	uint8_t raw_value_x_len;                             /*!< Length in bytes of raw_value_x1 and raw_value_x2.
                                                          *   Range: 0 to @ref SENSOR_COLUMN_DATA_MAX_LEN.
                                                          *   0: Request the whole series, raw_value_x1 and raw_value_x2 are not used. */
	uint8_t raw_value_x1[SENSOR_COLUMN_DATA_MAX_LEN];    /*!< Raw value identifying the starting column, with raw_value_x_len valid bytes. */
	uint8_t raw_value_x2[SENSOR_COLUMN_DATA_MAX_LEN];    /*!< Raw value identifying the ending column, with raw_value_x_len valid bytes. */
} rtk_bt_mesh_sensor_series_get_t;

/**
 * @struct    rtk_bt_mesh_sensor_client_status_descriptor_t
 * @brief     BLE MESH sensor client model descriptor status message structure.
 */
typedef struct {
	uint16_t src;                                    /*!< Unicast address of the server that sent the Status message. */
	uint16_t property_id;                            /*!< Sensor Property ID carried in the message.
                                                      *   It is the requested Sensor Property ID when num_descriptors is 0, the server has no such sensor. */
	uint16_t num_descriptors;                        /*!< Number of Sensor Descriptors that follow the fixed fields of the event data. */
	rtk_bt_mesh_sensor_descriptor_t *descriptors;    /*!< Sensor Descriptors reported by the server.
                                                      *   This member only marks where the data starts,
                                                      *   the num_descriptors Sensor Descriptors are located right after the fixed fields of the event data. */
} rtk_bt_mesh_sensor_client_status_descriptor_t;

/**
 * @struct    rtk_bt_mesh_sensor_client_status_cadence_t
 * @brief     BLE MESH sensor client model cadence status message structure.
 */
typedef struct {
	uint16_t src;                             /*!< Unicast address of the server that sent the Status message. */
	uint16_t property_id;                     /*!< Sensor Property ID of the sensor. */
	bool flag;                                /*!< Whether the Sensor Cadence state is carried in the message.
                                               *   - true: The Sensor Cadence state follows the fixed fields of the event data.
                                               *   - false: The Sensor Cadence state is not carried. */
	rtk_bt_mesh_sensor_cadence_t *cadence;    /*!< Sensor Cadence state reported by the server.
                                               *   This member only marks where the data starts, the state is located right after the fixed fields of the event data.
                                               *   Layout: raw_value_len (1 byte), divisor and trigger type (1 byte, bit 0 to 6 divisor, bit 7 trigger type),
                                               *   delta down, delta up, status min interval (1 byte), fast cadence low, fast cadence high.
                                               *   The length of each delta is raw_value_len if the trigger type is 0, otherwise 2 bytes.
                                               *   Only valid when flag is true. */
} rtk_bt_mesh_sensor_client_status_cadence_t;

/**
 * @struct    rtk_bt_mesh_sensor_client_status_settings_t
 * @brief     BLE MESH sensor client model settings status message structure.
 */
typedef struct {
	uint16_t src;             /*!< Unicast address of the server that sent the Status message. */
	uint16_t property_id;     /*!< Sensor Property ID of the sensor. */
	uint16_t num_ids;         /*!< Number of Sensor Setting Property IDs that follow the fixed fields of the event data. */
	uint16_t *setting_ids;    /*!< Sensor Setting Property IDs reported by the server.
                               *   This member only marks where the data starts,
                               *   the num_ids Sensor Setting Property IDs (2 bytes each) are located right after the fixed fields of the event data. */
} rtk_bt_mesh_sensor_client_status_settings_t;

/**
 * @struct    rtk_bt_mesh_sensor_client_status_setting_t
 * @brief     BLE MESH sensor client model setting status message structure.
 */
typedef struct {
	uint16_t src;                             /*!< Unicast address of the server that sent the Status message. */
	uint16_t property_id;                     /*!< Sensor Property ID of the sensor. */
	uint16_t setting_property_id;             /*!< Sensor Setting Property ID of the setting within the sensor. */
	bool flag;                                /*!< Whether the Sensor Setting Access and Sensor Setting Raw are carried in the message.
                                               *   - true: The Sensor Setting state follows the fixed fields of the event data.
                                               *   - false: The Sensor Setting state is not carried. */
	rtk_bt_mesh_sensor_setting_t *setting;    /*!< Sensor Setting state reported by the server.
                                               *   This member only marks where the data starts, the state is located right after the fixed fields of the event data.
                                               *   Layout: setting property id (2 bytes), setting access (1 byte), setting raw length (1 byte), setting raw.
                                               *   Only valid when flag is true. */
} rtk_bt_mesh_sensor_client_status_setting_t;

/**
 * @struct    rtk_bt_mesh_sensor_client_status_t
 * @brief     BLE MESH sensor client model status message structure.
 */
typedef struct {
	uint16_t src;                           /*!< Unicast address of the server that sent the Status message. */
	uint16_t marshalled_sensor_data_len;    /*!< Length in bytes of the marshalled sensor data that follows the fixed fields of the event data. */
	void *marshalled_sensor_data;           /*!< Marshalled sensor data reported by the server.
                                             *   This member only marks where the data starts, the data is located right after the fixed fields of the event data.
                                             *   It is a list of Marshalled Sensor Data Items in ascending order of the Sensor Property ID. */
} rtk_bt_mesh_sensor_client_status_t;

/**
 * @struct    rtk_bt_mesh_sensor_client_status_column_t
 * @brief     BLE MESH sensor client model column status message structure.
 */
typedef struct {
	uint16_t src;                     /*!< Unicast address of the server that sent the Status message. */
	uint16_t property_id;             /*!< Sensor Property ID of the sensor. */
	uint16_t column_raw_value_len;    /*!< Length in bytes of the column raw value that follows the fixed fields of the event data. */
	void *column_raw_value;           /*!< Column raw value reported by the server.
                                       *   This member only marks where the data starts, the value is located right after the fixed fields of the event data.
                                       *   It contains the optional Raw Value A, followed by the optional Raw Value B and Raw Value C of the Sensor Series Column state.
                                       *   Raw Value B is only present if Raw Value A is present, and Raw Value C is present if and only if Raw Value B is present.
                                       *   All of them are absent when column_raw_value_len is 0. */
} rtk_bt_mesh_sensor_client_status_column_t;

/**
 * @struct    rtk_bt_mesh_sensor_client_status_series_t
 * @brief     BLE MESH sensor client model series status message structure.
 */
typedef struct {
	uint16_t src;                     /*!< Unicast address of the server that sent the Status message. */
	uint16_t property_id;             /*!< Sensor Property ID of the sensor. */
	uint16_t series_raw_value_len;    /*!< Length in bytes of the series raw value that follows the fixed fields of the event data. */
	void *series_raw_value;           /*!< Series raw value reported by the server.
                                       *   This member only marks where the data starts, the value is located right after the fixed fields of the event data.
                                       *   It is the Property Raw Value List, which has one or more entries.
                                       *   Each entry contains Raw Value A, optionally followed by Raw Value B and Raw Value C.
                                       *   Raw Value C is present if and only if Raw Value B is present.
                                       *   The list is absent when series_raw_value_len is 0. */
} rtk_bt_mesh_sensor_client_status_series_t;

/********************************* Sensor Server Model *********************************/
/**
 * @enum      rtk_bt_mesh_sensor_server_model_evt_t
 * @brief     BLE MESH sensor server model event definition.
 */
typedef enum {
	RTK_BT_MESH_SENSOR_SERVER_MODEL_GET = 1,               /*!< Indicate a Sensor Get message is received or the server needs the current sensor data, with msg
                                                            *   @ref rtk_bt_mesh_sensor_server_get_t.
                                                            *   The application shall write the current sensor data into the msg.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SENSOR_SERVER_MODEL_COLUMN_GET,            /*!< Indicate a Sensor Column Get message received from a client, with msg
                                                            *   @ref rtk_bt_mesh_sensor_server_direct_get_column_t.
                                                            *   The application shall write the Sensor Series Column state into the msg.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SENSOR_SERVER_MODEL_SERIES_GET,            /*!< Indicate a Sensor Series Get message received from a client, with msg
                                                            *   @ref rtk_bt_mesh_sensor_server_direct_get_series_t.
                                                            *   The application shall write the Sensor Series Column states into the msg.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SENSOR_SERVER_MODEL_COMPARE_CADENCE,       /*!< Indicate the server needs to know whether the fast cadence applies to the current measured value, with msg
                                                            *   @ref rtk_bt_mesh_sensor_server_compare_cadence_t.
                                                            *   The application shall compare the measured value with the Sensor Cadence state and write the result.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SENSOR_SERVER_MODEL_DESCRIPTOR_NUM_GET,    /*!< Indicate the server needs the number of Sensor Descriptors, with msg
                                                            *   @ref rtk_bt_mesh_sensor_server_get_descriptor_num_t.
                                                            *   The application shall write the number of Sensor Descriptors into the msg.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SENSOR_SERVER_MODEL_DESCRIPTOR_GET,        /*!< Indicate the server needs the Sensor Descriptors, with msg
                                                            *   @ref rtk_bt_mesh_sensor_server_get_descriptor_t.
                                                            *   It follows event @ref RTK_BT_MESH_SENSOR_SERVER_MODEL_DESCRIPTOR_NUM_GET, which gives the number.
                                                            *   The application shall write the Sensor Descriptors into the msg.
                                                            *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SENSOR_SERVER_MODEL_EVT_MAX,               /*!< Reserved. */
} rtk_bt_mesh_sensor_server_model_evt_t;

/**
 * @enum      rtk_bt_mesh_sensor_server_model_act_t
 * @brief     BLE MESH sensor server model act definition.
 */
typedef enum {
	RTK_BT_MESH_SENSOR_SERVER_MODEL_ACT_DB_SET = 1,    /*!< Set the sensor database of the sensor server. */
	RTK_BT_MESH_SENSOR_SERVER_MODEL_ACT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_sensor_server_model_act_t;

/**
 * @struct    rtk_bt_mesh_sensor_db_t
 * @brief     BLE MESH sensor server database structure.
 */
typedef struct {
	rtk_bt_mesh_sensor_descriptor_t descriptor;    /*!< Sensor Descriptor state of the sensor. */
	uint8_t sensor_raw_data_len;                   /*!< Length in bytes of the raw data of the sensor. */
	uint16_t num_settings;                         /*!< Number of Sensor Setting states in settings. */
	rtk_bt_mesh_sensor_setting_t *settings;        /*!< Sensor Setting states of the sensor, with num_settings elements. */
	rtk_bt_mesh_sensor_cadence_t *cadence;         /*!< Sensor Cadence state of the sensor. */
	int32_t pub_count;                             /*!< Publication counter used by the stack, initialize it to 0. */
} rtk_bt_mesh_sensor_db_t;

/**
 * @struct    rtk_bt_mesh_sensor_server_db_set_t
 * @brief     BLE MESH sensor server database set message structure.
 */
typedef struct {
	uint16_t num_sensors;    /*!< Number of sensors in the sensor database. */
	void *pdata_db;          /*!< Sensor database, an array of num_sensors @ref rtk_bt_mesh_sensor_db_t. */
} rtk_bt_mesh_sensor_server_db_set_t;

/**
 * @struct    rtk_bt_mesh_sensor_server_get_t
 * @brief     BLE MESH sensor server model get message structure.
 */
typedef struct {
	uint16_t property_id;    /*!< Sensor Property ID of the sensor whose sensor data is needed. */
	void *raw_data;          /*!< Output: the application writes the current sensor data here, in the form of a 2-byte data length followed by the data.
                              *   The data length is the length of the raw data of the sensor, a zero length means no data.
                              *   It is limited to @ref SENSOR_GET_DATA_MAX_LEN.
                              *   Must not be NULL. */
} rtk_bt_mesh_sensor_server_get_t;

/**
 * @struct    rtk_bt_mesh_sensor_server_get_column_t
 * @brief     BLE MESH sensor server model get column message structure.
 */
typedef struct {
	uint16_t property_id;       /*!< Sensor Property ID of the sensor whose Sensor Series Column state is needed. */
	uint8_t raw_value_x_len;    /*!< Length in bytes of raw_value_x. */
	const void *raw_value_x;    /*!< Raw value identifying the column, with raw_value_x_len bytes. */
	uint16_t column_len;        /*!< Length in bytes of the column. */
	void *column;               /*!< The column, which contains Raw Value A, optionally followed by Raw Value B and Raw Value C. */
} rtk_bt_mesh_sensor_server_get_column_t;

/**
 * @struct    rtk_bt_mesh_sensor_server_direct_get_column_t
 * @brief     BLE MESH sensor server model get column for direct calling message structure.
 */
typedef struct {
	uint16_t property_id;                               /*!< Sensor Property ID of the sensor whose Sensor Series Column state is needed. */
	uint8_t raw_value_x_len;                            /*!< Length in bytes of raw_value_x.
                                                         *   Range: 1 to @ref SENSOR_COLUMN_DATA_MAX_LEN. */
	uint8_t raw_value_x[SENSOR_COLUMN_DATA_MAX_LEN];    /*!< Raw value identifying the column, with raw_value_x_len valid bytes. */
	void *value;                                        /*!< Output: the application writes the column here, in the form of a 2-byte data length followed by the column.
                                                         *   The column contains Raw Value A, optionally followed by Raw Value B and Raw Value C.
                                                         *   Must not be NULL. */
} rtk_bt_mesh_sensor_server_direct_get_column_t;

/**
 * @struct    rtk_bt_mesh_sensor_server_get_series_t
 * @brief     BLE MESH sensor server model get series message structure.
 */
typedef struct {
	uint16_t property_id;        /*!< Sensor Property ID of the sensor whose Sensor Series Column states are needed. */
	uint8_t raw_value_x_len;     /*!< Length in bytes of raw_value_x1 and raw_value_x2. */
	const void *raw_value_x1;    /*!< Raw value identifying the starting column, with raw_value_x_len bytes. */
	const void *raw_value_x2;    /*!< Raw value identifying the ending column, with raw_value_x_len bytes. */
	uint16_t series_len;         /*!< Length in bytes of the series. */
	void *series;                /*!< The series, which is the Property Raw Value List,
                                  *   each entry containing Raw Value A, optionally followed by Raw Value B and Raw Value C. */
} rtk_bt_mesh_sensor_server_get_series_t;

/**
 * @struct    rtk_bt_mesh_sensor_server_direct_get_series_t
 * @brief     BLE MESH sensor server model get series for direct calling message structure.
 */
typedef struct {
	uint16_t property_id;                               /*!< Sensor Property ID of the sensor whose Sensor Series Column states are needed. */
	uint8_t raw_value_x_len;                            /*!< Length in bytes of raw_value_x1 and raw_value_x2.
                                                         *   Range: 0 to @ref SENSOR_GET_SERIES_MAX_LEN.
                                                         *   0: The whole series is requested, raw_value_x1 and raw_value_x2 are not used. */
	uint8_t raw_value_x1[SENSOR_GET_SERIES_MAX_LEN];    /*!< Raw value identifying the starting column, with raw_value_x_len valid bytes. */
	uint8_t raw_value_x2[SENSOR_GET_SERIES_MAX_LEN];    /*!< Raw value identifying the ending column, with raw_value_x_len valid bytes. */
	void *value;                                        /*!< Output: the application writes the series here, in the form of a 2-byte data length followed by the series.
                                                         *   The series is the Property Raw Value List,
                                                         *   each entry containing Raw Value A, optionally followed by Raw Value B and Raw Value C.
                                                         *   Must not be NULL. */
} rtk_bt_mesh_sensor_server_direct_get_series_t;

/**
 * @struct    rtk_bt_mesh_sensor_server_get_cadence_t
 * @brief     BLE MESH sensor server model get cadence message structure.
 */
typedef struct {
	uint16_t property_id;    /*!< Sensor Property ID of the sensor whose Sensor Cadence state is needed. */
	uint8_t *cadence;        /*!< Output: the application writes the Sensor Cadence state here, in the form of a 2-byte data length followed by the data.
                              *   The data is raw_value_len (1 byte), divisor and trigger type (1 byte, bit 0 to 6 divisor, bit 7 trigger type),
                              *   delta down, delta up, status min interval (1 byte), fast cadence low, fast cadence high.
                              *   A zero data length means the sensor has no Sensor Cadence state.
                              *   Must not be NULL. */
} rtk_bt_mesh_sensor_server_get_cadence_t;

/**
 * @struct    rtk_bt_mesh_sensor_server_set_cadence_t
 * @brief     BLE MESH sensor server model set cadence message structure.
 */
typedef struct {
	uint16_t property_id;                     /*!< Sensor Property ID of the sensor whose Sensor Cadence state is set. */
	rtk_bt_mesh_sensor_cadence_t *cadence;    /*!< New Sensor Cadence state requested by the client.
                                               *   This member only marks where the data starts, the state is located right after the fixed fields of the event data.
                                               *   Layout: raw_value_len (1 byte), divisor and trigger type (1 byte),
                                               *   delta down, delta up, status min interval (1 byte), fast cadence low, fast cadence high.
                                               *   Bit 0 to 6 of the divisor and trigger type byte is the divisor, bit 7 is the trigger type.
                                               *   The length of each delta is raw_value_len if the trigger type is 0, otherwise 2 bytes.
                                               *   The new value is already stored in the sensor database, the application only needs to use it if needed. */
} rtk_bt_mesh_sensor_server_set_cadence_t;

/**
 * @struct    rtk_bt_mesh_sensor_server_set_setting_t
 * @brief     BLE MESH sensor server model set setting message structure.
 */
typedef struct {
	uint16_t property_id;                     /*!< Sensor Property ID of the sensor whose Sensor Setting state is set. */
	rtk_bt_mesh_sensor_setting_t *setting;    /*!< New Sensor Setting state requested by the client.
                                               *   This member only marks where the data starts, the state is located right after the fixed fields of the event data.
                                               *   Layout: setting property id (2 bytes), setting access (1 byte), setting raw length (1 byte), setting raw.
                                               *   The new value is already stored in the sensor database, the application only needs to use it if needed. */
} rtk_bt_mesh_sensor_server_set_setting_t;

/**
 * @struct    rtk_bt_mesh_sensor_server_get_settings_t
 * @brief     BLE MESH sensor server model get settings message structure.
 */
typedef struct {
	uint16_t property_id;       /*!< Sensor Property ID of the sensor whose Sensor Setting Property IDs are needed. */
	uint16_t *settings_data;    /*!< Output: the application writes the Sensor Setting Property IDs here.
                                 *   The first uint16_t is the number of Sensor Setting Property IDs, followed by that many Sensor Setting Property IDs.
                                 *   Must not be NULL. */
} rtk_bt_mesh_sensor_server_get_settings_t;

/**
 * @struct    rtk_bt_mesh_sensor_server_get_setting_t
 * @brief     BLE MESH sensor server model get setting message structure.
 */
typedef struct {
	uint16_t property_id;            /*!< Sensor Property ID of the sensor whose Sensor Setting state is needed. */
	uint16_t setting_property_id;    /*!< Sensor Setting Property ID of the setting within the sensor. */
	uint8_t *setting_data;           /*!< Output: the application writes the Sensor Setting state here.
                                      *   The first byte is the setting access, the second byte is the length of the setting raw, followed by the setting raw.
                                      *   A zero setting access means the sensor has no such Sensor Setting state.
                                      *   Must not be NULL. */
} rtk_bt_mesh_sensor_server_get_setting_t;

/**
 * @struct    rtk_bt_mesh_sensor_server_compare_cadence_t
 * @brief     BLE MESH sensor server model compare cadence message structure.
 */
typedef struct {
	uint16_t property_id;       /*!< Sensor Property ID of the sensor whose measured value is compared. */
	bool *need_fast_divisor;    /*!< Output: the application writes the comparison result here.
                                 *   - true: The measured value is in the fast cadence range, the fast cadence period divisor applies.
                                 *   - false: The measured value is not in the fast cadence range, the fast cadence period divisor does not apply.
                                 *   Must not be NULL. */
} rtk_bt_mesh_sensor_server_compare_cadence_t;

/**
 * @struct    rtk_bt_mesh_sensor_server_get_descriptor_num_t
 * @brief     BLE MESH sensor server model get descriptor number message structure.
 */
typedef struct {
	uint16_t property_id;        /*!< Sensor Property ID of the sensor whose Sensor Descriptor is requested.
                                  *   0x0000: All Sensor Descriptors of the element are requested. */
	uint16_t *descriptor_num;    /*!< Output: the application writes the number of Sensor Descriptors here.
                                  *   0: There is no such Sensor Descriptor.
                                  *   Must not be NULL. */
} rtk_bt_mesh_sensor_server_get_descriptor_num_t;

/**
 * @struct    rtk_bt_mesh_sensor_server_get_descriptor_t
 * @brief     BLE MESH sensor server model get descriptor message structure.
 */
typedef struct {
	uint16_t property_id;                           /*!< Sensor Property ID of the sensor whose Sensor Descriptor is requested.
                                                     *   0x0000: All Sensor Descriptors of the element are requested. */
	uint16_t descriptor_num;                        /*!< Number of Sensor Descriptors to write, which is the number written for event
                                                     *   @ref RTK_BT_MESH_SENSOR_SERVER_MODEL_DESCRIPTOR_NUM_GET.
                                                     *   It is limited to @ref SENSOR_GET_DATA_MAX_LEN. */
	rtk_bt_mesh_sensor_descriptor_t *descriptor;    /*!< Output: the application writes the descriptor_num Sensor Descriptors here.
                                                     *   Must not be NULL. */
} rtk_bt_mesh_sensor_server_get_descriptor_t;

/****************************** Sensor Setup Server Model ******************************/
/**
 * @enum      rtk_bt_mesh_sensor_setup_server_model_evt_t
 * @brief     BLE MESH sensor setup server model event definition.
 */
typedef enum {
	RTK_BT_MESH_SENSOR_SERVER_MODEL_CADENCE_SET = 1,    /*!< Indicate a Sensor Cadence Set or Set Unacknowledged message received from a client, with msg in the layout
                                                         *   described by @ref rtk_bt_mesh_sensor_server_set_cadence_t.
                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SENSOR_SERVER_MODEL_SETTING_SET,        /*!< Indicate a Sensor Setting Set or Set Unacknowledged message received from a client, with msg in the layout
                                                         *   described by @ref rtk_bt_mesh_sensor_server_set_setting_t.
                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SENSOR_SERVER_MODEL_CADENCE_GET,        /*!< Indicate the server needs the Sensor Cadence state, with msg
                                                         *   @ref rtk_bt_mesh_sensor_server_get_cadence_t.
                                                         *   The application shall write the Sensor Cadence state into the msg.
                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SENSOR_SERVER_MODEL_SETTINGS_GET,       /*!< Indicate the server needs the Sensor Setting Property IDs, with msg
                                                         *   @ref rtk_bt_mesh_sensor_server_get_settings_t.
                                                         *   The application shall write the Sensor Setting Property IDs into the msg.
                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SENSOR_SERVER_MODEL_SETTING_GET,        /*!< Indicate the server needs the Sensor Setting state, with msg
                                                         *   @ref rtk_bt_mesh_sensor_server_get_setting_t.
                                                         *   The application shall write the Sensor Setting state into the msg.
                                                         *   Direct calling event, BT APIs cannot be called in its callback. */
	RTK_BT_MESH_SENSOR_SETUP_SERVER_MODEL_EVT_MAX,      /*!< Reserved. */
} rtk_bt_mesh_sensor_setup_server_model_evt_t;

/**
 * @enum      rtk_bt_mesh_sensor_setup_server_model_act_t
 * @brief     BLE MESH sensor setup server model act definition.
 */
typedef enum {
	RTK_BT_MESH_SENSOR_SETUP_SERVER_MODEL_ACT_DB_SET = 1,    /*!< Set the sensor database of the sensor setup server. */
	RTK_BT_MESH_SENSOR_SETUP_SERVER_MODEL_ACT_MAX,           /*!< Reserved. */
} rtk_bt_mesh_sensor_setup_server_model_act_t;

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
/** @addtogroup BT_Mesh_Sensor_Model_Functions BT Mesh Sensor Model Functions
 * @{
 */

/**
 * @brief     Send a Sensor Descriptor Get message to a sensor server to read its Sensor Descriptor states.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SENSOR_DESCRIPTOR_CLIENT_MODEL_STATUS.
 * @param[in] sensor_descriptor_get: Sensor Descriptor Get message parameters.
 *                                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_sensor_descriptor_get(rtk_bt_mesh_sensor_descriptor_get_t *sensor_descriptor_get);

/**
 * @brief     Send a Sensor Cadence Get message to a sensor setup server to read its Sensor Cadence state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SENSOR_CADENCE_CLIENT_MODEL_STATUS.
 * @param[in] sensor_cadence_get: Sensor Cadence Get message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_sensor_cadence_get(rtk_bt_mesh_sensor_cadence_get_t *sensor_cadence_get);

/**
 * @brief     Send a Sensor Cadence Set or Set Unacknowledged message to a sensor setup server to change its Sensor Cadence state.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SENSOR_CADENCE_CLIENT_MODEL_STATUS.
 * @param[in] sensor_cadence_set: Sensor Cadence Set message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_sensor_cadence_set(rtk_bt_mesh_sensor_cadence_set_t *sensor_cadence_set);

/**
 * @brief     Send a Sensor Settings Get message to a sensor setup server to read the list of its Sensor Setting states.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SENSOR_SETTINGS_CLIENT_MODEL_STATUS.
 * @param[in] sensor_settings_get: Sensor Settings Get message parameters.
 *                                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_sensor_settings_get(rtk_bt_mesh_sensor_settings_get_t *sensor_settings_get);

/**
 * @brief     Send a Sensor Setting Get message to a sensor setup server to read one of its Sensor Setting states.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SENSOR_SETTING_CLIENT_MODEL_STATUS.
 * @param[in] sensor_setting_get: Sensor Setting Get message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_sensor_setting_get(rtk_bt_mesh_sensor_setting_get_t *sensor_setting_get);

/**
 * @brief     Send a Sensor Setting Set or Set Unacknowledged message to a sensor setup server to change one of its Sensor Setting states.
 *            Only when ack is true, the server replies with a Status message.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SENSOR_SETTING_CLIENT_MODEL_STATUS.
 * @param[in] sensor_setting_set: Sensor Setting Set message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_sensor_setting_set(rtk_bt_mesh_sensor_setting_set_t *sensor_setting_set);

/**
 * @brief     Send a Sensor Get message to a sensor server to read its Sensor Data states.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SENSOR_CLIENT_MODEL_STATUS.
 * @param[in] sensor_get: Sensor Get message parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_sensor_get(rtk_bt_mesh_sensor_get_t *sensor_get);

/**
 * @brief     Send a Sensor Column Get message to a sensor server to read one of its Sensor Series Column states.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SENSOR_COLUMN_CLIENT_MODEL_STATUS.
 * @param[in] sensor_column_get: Sensor Column Get message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_sensor_column_get(rtk_bt_mesh_sensor_column_get_t *sensor_column_get);

/**
 * @brief     Send a Sensor Series Get message to a sensor server to read a sequence of its Sensor Series Column states.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SENSOR_SERIES_CLIENT_MODEL_STATUS.
 * @param[in] sensor_series_get: Sensor Series Get message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_sensor_series_get(rtk_bt_mesh_sensor_series_get_t *sensor_series_get);

/**
 * @brief     Set the sensor database of the sensor server.
 * @param[in] db_set: Sensor database parameters.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_sensor_server_db_set(rtk_bt_mesh_sensor_server_db_set_t *db_set);

/**
 * @brief     Set the sensor database of the sensor setup server.
 * @param[in] db_set: Sensor database parameters.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_sensor_setup_server_db_set(rtk_bt_mesh_sensor_server_db_set_t *db_set);

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

#endif /* __RTK_BT_MESH_SENSOR_MODEL_H__ */

