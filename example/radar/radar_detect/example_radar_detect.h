#ifndef __EXAMPLE_RADAR_DETECT_H__
#define __EXAMPLE_RADAR_DETECT_H__

/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include "basic_types.h"
#include "wifi_api.h"
#include "wifi_api_event.h"

#define RADAR_REPORT_GROUP 6        /* = number of consecutive report types*/
#define RADAR_REPORT_BUF_DEPTH 1    /* maximum buffer num for per report type */
#define RADAR_REPORT_BUF_NUM (RADAR_REPORT_GROUP * RADAR_REPORT_BUF_DEPTH)
#define RADAR_ACS_ENABLE          0

struct radar_report_data {
	u8 *radar_buffer;
	u8 type;            /* indicate rpt type: init */
	u32 buf_size;       /* malloc size: init */
	u32 rpt_len;        /* actual radar header + raw data size: fill rpt */
	u8 rpt_ready : 2;   /* type match & valid raw data(with start-bit0/end-bit1)*/
};

struct radar_report_ring_priv {
	struct radar_report_data radar_rpt_pkt[RADAR_REPORT_BUF_NUM];
	rtos_mutex_t radar_rpt_mutex;  /* mutex */
	u8 radar_buf_write_idx[RADAR_REPORT_GROUP];      /*event rpt enq ++*/
	u8 radar_buf_read_idx;         /*task handle deq ++*/
	s8 curr_wbuf_idx;              /*indicate the buffer currently being written to(segments_to_report)*/
	u8 qlen;                       /*number of busy buf*/
	u8 per_radar_rpt_done : 1;
	u8 b_drop_group : 1;   /* drop type0~type5 group when buffers unavailable */
};

void example_radar_detect_report_cb(u8 *evt_info);
void example_radar_detect_show(struct radar_report_data *radar_rpt_pkt);
void example_radar_detect(void);

#endif //#ifndef __EXAMPLE_RADAR_DETECT_H__
