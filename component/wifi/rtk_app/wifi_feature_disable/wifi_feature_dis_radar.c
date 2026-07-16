/**
  ******************************************************************************
  * @file    wifi_feature_dis_radar.c
  * @author
  * @version
  * @date
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
#define __WIFI_FEATURE_DIS_CSI_C__

#include "wifi_api_types.h"

#ifdef CONFIG_RADAR
void wifi_hal_radar_support_set(void)
{

}

void wifi_hal_radar_en_by_user(u8 *radar_en_by_user)
{
	*radar_en_by_user = 0;
}

s32 wifi_hal_radar_recv_data(u16 frame_num, u8 frame_type, u8 *data)
{
	UNUSED(frame_num);
	UNUSED(frame_type);
	UNUSED(data);
	return -1;
}

void wifi_hal_radar_deinit(void *a)
{
	UNUSED(a);
}

s32 wifi_hal_radar_cfg(void *a)
{
	UNUSED(a);
	return -1;
}

s32 wifi_hal_radar_en(void *a)
{
	UNUSED(a);
	return -1;
}

void wifi_hal_radar_interrupt_handle(void)
{

}

void wifi_hal_radar_rpt_hdl(u8 *buf, void *a)
{
	UNUSED(buf);
	UNUSED(a);
}

s32 wifi_hal_radar_check_status(u8 *radar_active)
{
	UNUSED(radar_active);
	return 0;
}

void wifi_hal_radar_query(void)
{

}

void wifi_hal_dbg_radar_edcca_th(char *cmd_ptr, u32 extra_arg)
{
	(void)cmd_ptr;
	(void)extra_arg;
}

void wifi_hal_dbg_radar_intf_det_th(char *cmd_ptr, u32 extra_arg)
{
	(void)cmd_ptr;
	(void)extra_arg;
}

void wifi_hal_dbg_radar_timeout_th(char *cmd_ptr, u32 extra_arg)
{
	(void)cmd_ptr;
	(void)extra_arg;
}

void wifi_hal_dbg_radar_rfk_en(char *cmd_ptr, u32 extra_arg)
{
	(void)cmd_ptr;
	(void)extra_arg;
}

void wifi_hal_dbg_radar_rf_srk_dis(char *cmd_ptr, u32 extra_arg)
{
	(void)cmd_ptr;
	(void)extra_arg;
}

#ifdef CONFIG_MP_INCLUDED
void wifi_hal_radar_start(void)
{

}

void wifi_hal_radar_stop(void)
{

}

void wifi_hal_radar_bw(u8 bw_idx)
{
	UNUSED(bw_idx);
}

void wifi_hal_radar_chrip(u8 chrip_width, u8 chirp_num)
{
	UNUSED(chrip_width);
	UNUSED(chirp_num);
}

void wifi_hal_radar_frame(u16 frame_intev, u16 frame_num)
{
	UNUSED(frame_intev);
	UNUSED(frame_num);
}

void wifi_hal_radar_trx(u8 start)
{
	UNUSED(start);
}

s16 wifi_hal_radar_get_iso(void)
{
	return 0;
}

s16 wifi_hal_radar_get_rssi(void)
{
	return 0;
}
#endif

#endif /* CONFIG_RADAR */
