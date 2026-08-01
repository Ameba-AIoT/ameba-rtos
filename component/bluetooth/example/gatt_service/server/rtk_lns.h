/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __LNS_SERVICE_H__
#define __LNS_SERVICE_H__

#ifdef __cplusplus
extern "C"  {
#endif

#include <rtk_bt_def.h>

/* Location and Speed value max: flags(2) + inst_speed(2) + total_distance(3) +
 * latitude(4) + longitude(4) + elevation(3) + heading(2) + rolling_time(1) + utc_time(7) */
#define LNS_LOC_SPEED_VALUE_MAX_LEN         28
/* Position Quality value max: flags(2) + beacons_in_solution(1) + beacons_in_view(1) +
 * time_to_first_fix(2) + ehpe(4) + evpe(4) + hdop(1) + vdop(1) */
#define LNS_POS_QUALITY_VALUE_MAX_LEN       16
/* Navigation value max: flags(2) + bearing(2) + heading(2) + remaining_distance(3) +
 * remaining_vertical_distance(3) + eta(7) */
#define LNS_NAVIGATION_VALUE_MAX_LEN        19

/* LN Feature (spec, uint32) */
#define LNS_FEAT_INSTANTANEOUS_SPEED            (1 << 0)
#define LNS_FEAT_TOTAL_DISTANCE                 (1 << 1)
#define LNS_FEAT_LOCATION                       (1 << 2)
#define LNS_FEAT_ELEVATION                      (1 << 3)
#define LNS_FEAT_HEADING                        (1 << 4)
#define LNS_FEAT_ROLLING_TIME                   (1 << 5)
#define LNS_FEAT_UTC_TIME                       (1 << 6)
#define LNS_FEAT_REMAINING_DISTANCE             (1 << 7)
#define LNS_FEAT_REMAINING_VERTICAL_DISTANCE    (1 << 8)
#define LNS_FEAT_ESTIMATED_TIME_OF_ARRIVAL      (1 << 9)
#define LNS_FEAT_NUMBER_OF_BEACONS_IN_SOLUTION  (1 << 10)
#define LNS_FEAT_NUMBER_OF_BEACONS_IN_VIEW      (1 << 11)
#define LNS_FEAT_TIME_TO_FIRST_FIX              (1 << 12)
#define LNS_FEAT_EHPE                           (1 << 13)
#define LNS_FEAT_EVPE                           (1 << 14)
#define LNS_FEAT_HDOP                           (1 << 15)
#define LNS_FEAT_VDOP                           (1 << 16)
#define LNS_FEAT_LOC_SPEED_CONTENT_MASKING      (1 << 17)
#define LNS_FEAT_FIX_RATE_SETTING               (1 << 18)
#define LNS_FEAT_ELEVATION_SETTING              (1 << 19)
#define LNS_FEAT_POSITION_STATUS                (1 << 20)

/* Position Status enumeration (2 bit) */
#define LNS_POSITION_STATUS_NO_POSITION         0
#define LNS_POSITION_STATUS_POSITION_OK         1
#define LNS_POSITION_STATUS_ESTIMATED_POSITION  2
#define LNS_POSITION_STATUS_LAST_KNOWN_POSITION 3

/* Elevation Source enumeration (2 bit) */
#define LNS_ELEVATION_SOURCE_POSITIONING_SYSTEM 0
#define LNS_ELEVATION_SOURCE_BAROMETRIC         1
#define LNS_ELEVATION_SOURCE_DATABASE_SERVICE   2
#define LNS_ELEVATION_SOURCE_OTHER              3

/* Location and Speed Flags (spec 3.2), uint16 little endian */
typedef struct {
	uint16_t inst_speed_present: 1;     /* bit0 */
	uint16_t total_distance_present: 1; /* bit1 */
	uint16_t location_present: 1;       /* bit2 (latitude + longitude) */
	uint16_t elevation_present: 1;      /* bit3 */
	uint16_t heading_present: 1;        /* bit4 */
	uint16_t rolling_time_present: 1;   /* bit5 */
	uint16_t utc_time_present: 1;       /* bit6 */
	uint16_t position_status: 2;        /* bit7-8 */
	uint16_t speed_distance_format: 1;  /* bit9  (0: 2D, 1: 3D) */
	uint16_t elevation_source: 2;       /* bit10-11 */
	uint16_t heading_source: 1;         /* bit12 (0: movement, 1: magnetic compass) */
	uint16_t rfu: 3;                    /* bit13-15 */
} lns_loc_speed_flag_t;

typedef struct {
	lns_loc_speed_flag_t flags;
	uint16_t inst_speed;        /* 1/100 m/s, present when flag.bit0 */
	uint32_t total_distance;    /* uint24, 1/10 m, present when flag.bit1 */
	int32_t latitude;           /* 1e-7 deg, present when flag.bit2 */
	int32_t longitude;          /* 1e-7 deg, present when flag.bit2 */
	int32_t elevation;          /* sint24, 1/100 m, present when flag.bit3 */
	uint16_t heading;           /* 1/100 deg, present when flag.bit4 */
	uint8_t rolling_time;       /* s, present when flag.bit5 */
	uint8_t utc_time[7];        /* Date Time, present when flag.bit6 */
} lns_loc_speed_val_t;

/* Position Quality Flags (spec 3.4), uint16 little endian */
typedef struct {
	uint16_t beacons_in_solution_present: 1;    /* bit0 */
	uint16_t beacons_in_view_present: 1;        /* bit1 */
	uint16_t time_to_first_fix_present: 1;      /* bit2 */
	uint16_t ehpe_present: 1;                   /* bit3 */
	uint16_t evpe_present: 1;                   /* bit4 */
	uint16_t hdop_present: 1;                   /* bit5 */
	uint16_t vdop_present: 1;                   /* bit6 */
	uint16_t position_status: 2;                /* bit7-8 */
	uint16_t rfu: 7;                            /* bit9-15 */
} lns_pos_quality_flag_t;

typedef struct {
	lns_pos_quality_flag_t flags;
	uint8_t beacons_in_solution;    /* present when flag.bit0 */
	uint8_t beacons_in_view;        /* present when flag.bit1 */
	uint16_t time_to_first_fix;     /* 1/10 s, present when flag.bit2 */
	uint32_t ehpe;                  /* 1/100 m, present when flag.bit3 */
	uint32_t evpe;                  /* 1/100 m, present when flag.bit4 */
	uint8_t hdop;                   /* x0.2, present when flag.bit5 */
	uint8_t vdop;                   /* x0.2, present when flag.bit6 */
} lns_pos_quality_val_t;

/* Navigation Flags (spec 3.3), uint16 little endian */
typedef struct {
	uint16_t remaining_distance_present: 1;             /* bit0 */
	uint16_t remaining_vertical_distance_present: 1;    /* bit1 */
	uint16_t eta_present: 1;                            /* bit2 */
	uint16_t position_status: 2;                        /* bit3-4 */
	uint16_t heading_source: 1;                         /* bit5 */
	uint16_t navigation_indicator_type: 1;              /* bit6 (0: To Waypoint, 1: To Destination) */
	uint16_t waypoint_reached: 1;                       /* bit7 */
	uint16_t destination_reached: 1;                    /* bit8 */
	uint16_t rfu: 7;                                    /* bit9-15 */
} lns_navigation_flag_t;

typedef struct {
	lns_navigation_flag_t flags;
	uint16_t bearing;                       /* 1/100 deg, Mandatory */
	uint16_t heading;                       /* 1/100 deg, Mandatory */
	uint32_t remaining_distance;            /* uint24, 1/10 m, present when flag.bit0 */
	int32_t remaining_vertical_distance;    /* sint24, 1/100 m, present when flag.bit1 */
	uint8_t eta[7];                         /* Date Time, present when flag.bit2 */
} lns_navigation_val_t;

typedef enum {
	/* LN Feature */
	LNS_PARAM_FEATURE = 0x01,
	/* Location and Speed */
	LNS_PARAM_LOC_SPEED_FLAG,
	LNS_PARAM_LOC_SPEED_INST_SPEED,
	LNS_PARAM_LOC_SPEED_TOTAL_DISTANCE,
	LNS_PARAM_LOC_SPEED_LATITUDE,
	LNS_PARAM_LOC_SPEED_LONGITUDE,
	LNS_PARAM_LOC_SPEED_ELEVATION,
	LNS_PARAM_LOC_SPEED_HEADING,
	LNS_PARAM_LOC_SPEED_ROLLING_TIME,
	LNS_PARAM_LOC_SPEED_UTC_TIME,
	/* Position Quality */
	LNS_PARAM_POS_QUALITY_FLAG,
	LNS_PARAM_POS_QUALITY_BEACONS_IN_SOLUTION,
	LNS_PARAM_POS_QUALITY_BEACONS_IN_VIEW,
	LNS_PARAM_POS_QUALITY_TIME_TO_FIRST_FIX,
	LNS_PARAM_POS_QUALITY_EHPE,
	LNS_PARAM_POS_QUALITY_EVPE,
	LNS_PARAM_POS_QUALITY_HDOP,
	LNS_PARAM_POS_QUALITY_VDOP,
	/* Navigation */
	LNS_PARAM_NAVIGATION_FLAG,
	LNS_PARAM_NAVIGATION_BEARING,
	LNS_PARAM_NAVIGATION_HEADING,
	LNS_PARAM_NAVIGATION_REMAINING_DISTANCE,
	LNS_PARAM_NAVIGATION_REMAINING_VERTICAL_DISTANCE,
	LNS_PARAM_NAVIGATION_ETA,
} lns_param_type_t;

uint16_t lns_srv_add(void);

uint16_t lns_srv_set_params(lns_param_type_t type, uint8_t len, void *value);

void lns_srv_callback(uint8_t event, void *data);

void lns_disconnect(uint16_t conn_handle);

void lns_status_deinit(void);

#ifdef __cplusplus
}
#endif

#endif  /* __LNS_SERVICE_H__ */
