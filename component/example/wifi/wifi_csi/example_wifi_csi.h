#ifndef __EXAMPLE_WIFI_CSI_H__
#define __EXAMPLE_WIFI_CSI_H__

/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
void example_wifi_csi_report_cb(s8 *buf, s32 buf_len, s32 flags, void *userdata);
void wifi_csi_show(u8 *csi_buf);
void example_wifi_csi(void);

#endif //#ifndef __EXAMPLE_WIFI_CSI_H__
