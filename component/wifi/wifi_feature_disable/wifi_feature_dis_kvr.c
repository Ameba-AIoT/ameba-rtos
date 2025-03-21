/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef __WIFI_FEATURE_DIS_KVR_C__
#define __WIFI_FEATURE_DIS_KVR_C__

#include "wifi_intf_drv_to_app_types.h"

#if !defined(CONFIG_WIFI_11K_ENABLE) && !defined(CONFIG_WIFI_11V_ENABLE) && !defined(CONFIG_WIFI_11R_ENABLE)
u8 rtw_check_peer_kvr_cap(void *a)
{
	UNUSED(a);
	return 0;
}

void rtw_roam_kvr_cap_update(char *buf, int buf_len, int flags, void *userdata)
{
	UNUSED(buf);
	UNUSED(buf_len);
	UNUSED(flags);
	UNUSED(userdata);
}
#endif

#ifndef CONFIG_WIFI_11K_ENABLE
void rtw_rrm_init_priv(void)
{

}

void rtw_rrm_disable(void)
{

}

void rtw_rrm_enable(void)
{

}


void rtw_rrmapi_nb_discover(void)
{

}

int rtw_rrmapi_event_post_cmd(u8 a, u32 rmid, u8 evid)
{
	UNUSED(a);
	UNUSED(rmid);
	UNUSED(evid);
	return -1;
}

int rtw_on_action_rrm(void *a, void *b)
{
	UNUSED(a);
	UNUSED(b);
	return FAIL;
}

void rtw_rrm_dbg_cmd(char *s)
{
	UNUSED(s);
}
#endif

#ifndef CONFIG_WIFI_11V_ENABLE
void rtw_wnm_btm_req_process(char *buf, int buf_len, int flags, void *userdata)
{
	UNUSED(buf);
	UNUSED(buf_len);
	UNUSED(flags);
	UNUSED(userdata);
}

void rtw_wnm_btm_flags_set(u8 flags)
{
	UNUSED(flags);
}

void rtw_wnm_dbg_cmd(char *buf, int buf_len, int flags, void *userdata)
{
	UNUSED(buf);
	UNUSED(buf_len);
	UNUSED(flags);
	UNUSED(userdata);
}
#endif

#if !defined(CONFIG_WIFI_11K_ENABLE) && !defined(CONFIG_WIFI_11V_ENABLE)
void rtw_roam_nb_rpt_elem_parsing(char *buf, int buf_len, int flags, void *userdata)
{
	UNUSED(buf);
	UNUSED(buf_len);
	UNUSED(flags);
	UNUSED(userdata);
}

void rtw_roam_nb_cap_update(u8 enable)
{
	UNUSED(enable);
}

u8 rtw_roam_nb_ch_num_get(void)
{
	return 0;
}

u16 rtw_roam_nb_ch_get(u8 ch)
{
	UNUSED(ch);
	return 0;
}
#endif

#if !defined(CONFIG_WIFI_11V_ENABLE) && !defined(CONFIG_WIFI_11R_ENABLE)
void rtw_set_to_roam(u8 roam_try_cnt)
{
	UNUSED(roam_try_cnt);
}
#endif

#ifndef CONFIG_WIFI_11R_ENABLE
u8 rtw_ft_check_en(void *a)
{
	UNUSED(a);
	return 0;
}

void rtw_ft_info_init(void)
{

}

void rtw_ft_info_deinit(void)
{

}

void rtw_ft_roam_reset(void)
{

}

void rtw_ft_cap_update(void *a)
{
	UNUSED(a);
}

void rtw_ft_rx_mgnt(char *buf, int buf_len, int flags, void *userdata)
{
	UNUSED(buf);
	UNUSED(buf_len);
	UNUSED(flags);
	UNUSED(userdata);
}

void rtw_ft_auth_start(char *buf, int buf_len, int flags, void *userdata)
{
	UNUSED(buf);
	UNUSED(buf_len);
	UNUSED(flags);
	UNUSED(userdata);
}

int rtw_ft_ptk_derive(u8 *sa, u8 *ssid)
{
	UNUSED(sa);
	UNUSED(ssid);
	return -1;
}

int rtw_ft_eapol3_ie_validate(u8 *buf, u32 len)
{
	UNUSED(buf);
	UNUSED(len);
	return -1;
}

int rtw_ft_eapol2_ies_build(void *a)
{
	UNUSED(a);
	return -1;
}
#endif

#endif

