/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __HTS_SERVICE_H__
#define __HTS_SERVICE_H__

#ifdef __cplusplus
extern "C"  {
#endif

#include <rtk_bt_def.h>

/* Temperature Measurement value: flags(1) + temperature(4) + time stamp(7) + temperature type(1) */
#define HTS_TEMPERATURE_MEASUREMENT_VALUE_MAX_LEN   13

#define HEALTH_THERMOMETER_SRV_UUID                 0x1809
#define TEMPERATURE_MEASUREMENT_CHAR_UUID           0x2A1C
#define TEMPERATURE_TYPE_CHAR_UUID                  0x2A1D
#define INTERMEDIATE_TEMPERATURE_CHAR_UUID          0x2A1E
#define MEASUREMENT_INTERVAL_CHAR_UUID              0x2A21
#define VALID_RANGE_DESC_UUID                       0x2906

#define RTK_BT_UUID_HEALTH_THERMOMETER_SRV          BT_UUID_DECLARE_16(HEALTH_THERMOMETER_SRV_UUID)
#define RTK_BT_UUID_TEMPERATURE_MEASUREMENT_CHAR    BT_UUID_DECLARE_16(TEMPERATURE_MEASUREMENT_CHAR_UUID)
#define RTK_BT_UUID_TEMPERATURE_TYPE_CHAR           BT_UUID_DECLARE_16(TEMPERATURE_TYPE_CHAR_UUID)
#define RTK_BT_UUID_INTERMEDIATE_TEMPERATURE_CHAR   BT_UUID_DECLARE_16(INTERMEDIATE_TEMPERATURE_CHAR_UUID)
#define RTK_BT_UUID_MEASUREMENT_INTERVAL_CHAR       BT_UUID_DECLARE_16(MEASUREMENT_INTERVAL_CHAR_UUID)

#define HTS_TEMPERATURE_MEASUREMENT_CHAR_VAL_INDEX      (2)    /* indicate */
#define HTS_TEMPERATURE_MEASUREMENT_CHAR_CCCD_INDEX     (3)    /* cccd indicate */
#define HTS_TEMPERATURE_TYPE_CHAR_VAL_INDEX             (5)    /* read */
#define HTS_INTERMEDIATE_TEMPERATURE_CHAR_VAL_INDEX     (7)    /* notify */
#define HTS_INTERMEDIATE_TEMPERATURE_CHAR_CCCD_INDEX    (8)    /* cccd notify */
#define HTS_MEASUREMENT_INTERVAL_CHAR_VAL_INDEX         (10)   /* read, write, indicate */
#define HTS_MEASUREMENT_INTERVAL_CHAR_CCCD_INDEX        (11)   /* cccd indicate */
#define HTS_VALID_RANGE_DESC_INDEX                      (12)   /* read (handled by stack) */

typedef struct {
	uint8_t temperature_unit_bit: 1;    /* 0: Celsius, 1: Fahrenheit */
	uint8_t time_stamp_bit: 1;          /* time stamp field present */
	uint8_t temperature_type_bit: 1;    /* temperature type field present */
	uint8_t rfu: 5;
} temperature_measurement_flag_t;  /* flag bit_field, 1 Byte */

typedef enum {
	HTS_TEMPERATURE_MEASUREMENT_PARAM_FLAG = 0x01,
	HTS_TEMPERATURE_MEASUREMENT_PARAM_TEMPERATURE_VALUE,
	HTS_TEMPERATURE_TYPE_PARAM_VALUE,
	HTS_MEASUREMENT_INTERVAL_PARAM_VALUE,
	HTS_TEMPERATURE_MEASUREMENT_PARAM_TIMESTAMP,   /* 7-byte Date Time: Year(2 LE) Month Day Hours Minutes Seconds */
} hts_param_type_t;

uint16_t health_thermometer_srv_add(void);

void health_thermometer_srv_callback(uint8_t event, void *data);

uint16_t health_thermometer_srv_set_params(hts_param_type_t param_type, uint8_t len, void *p_value);

uint16_t temperature_measurement_indicate(uint16_t conn_handle);

uint16_t intermediate_temperature_notify(uint16_t conn_handle);

uint16_t measurement_interval_indicate(uint16_t conn_handle);

void hts_disconnect(uint16_t conn_handle);

void hts_status_deinit(void);

#ifdef __cplusplus
}
#endif

#endif  /* __HTS_SERVICE_H__ */
