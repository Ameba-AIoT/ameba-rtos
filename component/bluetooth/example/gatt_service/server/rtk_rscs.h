/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __RSCS_SERVICE_H__
#define __RSCS_SERVICE_H__

#ifdef __cplusplus
extern "C"  {
#endif

#include <rtk_bt_def.h>

#define RSCS_MEASUREMENT_VALUE_MAX_LEN          10

/* RSC Measurement Flags (spec 3.1.1.1), 1 Byte */
typedef struct {
	uint8_t inst_stride_len_present: 1;  /* bit0 */
	uint8_t total_distance_present: 1;  /* bit1 */
	uint8_t walking_or_running: 1;  /* bit2: 0=walking, 1=running */
	uint8_t rfu: 5;
} rscs_meas_flag_t;

typedef struct {
	rscs_meas_flag_t flags;
	uint16_t inst_speed;        /* 1/256 m/s, mandatory */
	uint8_t  inst_cadence;      /* 1/min, mandatory */
	uint16_t inst_stride_len;   /* 1/100 m, present when flag.bit0 */
	uint32_t total_distance;    /* 1/10 m, present when flag.bit1 */
} rscs_meas_val_t;

/* RSC Feature (spec Table 3.2, uint16) */
#define RSCS_FEAT_INST_STRIDE_LEN   (1 << 0)
#define RSCS_FEAT_TOTAL_DISTANCE    (1 << 1)
#define RSCS_FEAT_WALK_OR_RUN       (1 << 2)
#define RSCS_FEAT_CALIBRATION       (1 << 3)
#define RSCS_FEAT_MULTI_SENSOR_LOC  (1 << 4)

typedef enum {
	RSCS_PARAM_MEAS_FLAG = 0x01,
	RSCS_PARAM_MEAS_INST_SPEED,
	RSCS_PARAM_MEAS_INST_CADENCE,
	RSCS_PARAM_MEAS_INST_STRIDE_LEN,
	RSCS_PARAM_MEAS_TOTAL_DISTANCE,
	RSCS_PARAM_FEATURE,
	RSCS_PARAM_SENSOR_LOCATION,
} rscs_param_type_t;

uint16_t rscs_srv_add(void);

void rscs_srv_callback(uint8_t event, void *data);

uint16_t rscs_srv_set_params(rscs_param_type_t type, uint8_t len, void *value);

/* Sensor calibration interface: report the calibration outcome as the SC
 * Control Point Response Value (0x01 = Success, 0x04 = Operation Failed). */
void rscs_set_calibration_result(uint8_t response_value);

void rscs_measurement_notify(uint16_t conn_handle);

void rscs_disconnect(uint16_t conn_handle);

void rscs_status_deinit(void);

#ifdef __cplusplus
}
#endif

#endif  /* __RSCS_SERVICE_H__ */
