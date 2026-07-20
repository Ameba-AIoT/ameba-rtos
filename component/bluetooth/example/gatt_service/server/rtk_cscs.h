/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __CSCS_SERVICE_H__
#define __CSCS_SERVICE_H__

#ifdef __cplusplus
extern "C"  {
#endif

#include <rtk_bt_def.h>

#define CSCS_MEASUREMENT_VALUE_MAX_LEN          11

/* CSC Measurement Flags (spec 3.1.1.1), 1 Byte */
typedef struct {
	uint8_t wheel_revolution_data_present: 1;  /* bit0 */
	uint8_t crank_revolution_data_present: 1;  /* bit1 */
	uint8_t rfu: 6;
} cscs_meas_flag_t;

typedef struct {
	cscs_meas_flag_t flags;
	uint32_t cumulative_wheel_revolutions;  /* present when flag.bit0 */
	uint16_t last_wheel_event_time;         /* 1/1024 s, present when flag.bit0 */
	uint16_t cumulative_crank_revolutions;  /* present when flag.bit1 */
	uint16_t last_crank_event_time;         /* 1/1024 s, present when flag.bit1 */
} cscs_meas_val_t;

/* CSC Feature (spec Table 3.2, uint16) */
#define CSCS_FEAT_WHEEL_REV_SUPPORTED       (1 << 0)
#define CSCS_FEAT_CRANK_REV_SUPPORTED       (1 << 1)
#define CSCS_FEAT_MULTI_SENSOR_LOC_SUPPORTED (1 << 2)

typedef enum {
	CSCS_PARAM_MEAS_FLAG = 0x01,
	CSCS_PARAM_MEAS_CUMULATIVE_WHEEL_REV,
	CSCS_PARAM_MEAS_LAST_WHEEL_EVENT_TIME,
	CSCS_PARAM_MEAS_CUMULATIVE_CRANK_REV,
	CSCS_PARAM_MEAS_LAST_CRANK_EVENT_TIME,
	CSCS_PARAM_FEATURE,
	CSCS_PARAM_SENSOR_LOCATION,
} cscs_param_type_t;

uint16_t cscs_srv_add(void);

void cscs_srv_callback(uint8_t event, void *data);

uint16_t cscs_srv_set_params(cscs_param_type_t type, uint8_t len, void *value);

void cscs_measurement_notify(uint16_t conn_handle);

void cscs_disconnect(uint16_t conn_handle);

void cscs_status_deinit(void);

#ifdef __cplusplus
}
#endif

#endif  /* __CSCS_SERVICE_H__ */
