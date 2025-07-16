#ifndef __EXAMPLE_WIFI_CSI_H__
#define __EXAMPLE_WIFI_CSI_H__

/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include "platform_autoconf.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "lwip_netconf.h"
#include "wifi_api.h"
#include "rtw_autoconf.h"
#include "rtw_queue.h"

#define CSI_REPORT_BUF_NUM 1
#define CSI_REPORT_BUF_SIZE 1024

struct csi_report_data {
	struct list_head list;
	u8 *csi_buffer;
	u32 length;
};

struct csi_report_q_priv {
	struct csi_report_data csi_rpt_pkt[CSI_REPORT_BUF_NUM];
	struct list_head idle_q;
	struct list_head busy_q;
	rtos_mutex_t csi_rpt_q_mutex;  /* mutex */
	u32 idle_q_cnt;
	u32 busy_q_cnt;
};

void example_wifi_csi_report_cb(u8 *buf, s32 buf_len);
void wifi_csi_show(struct csi_report_data *csi_rpt_pkt);
void example_wifi_csi(void);

#endif //#ifndef __EXAMPLE_WIFI_CSI_H__
