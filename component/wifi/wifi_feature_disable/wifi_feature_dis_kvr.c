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

#include "rtw_wifi_constants.h"

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
void rtw_init_rm(void)
{

}

void rtw_deinit_rm(void)
{

}

void rtw_rm_enable(void)
{

}

void rtw_rm_disable(void)
{

}

void rtw_roam_nb_discover(void)
{

}

int rm_post_event(u8 a, u32 rmid, u8 evid)
{
	UNUSED(a);
	UNUSED(rmid);
	UNUSED(evid);
	return -1;
}

unsigned int onAction_rm(void *a, void *b)
{
	UNUSED(a);
	UNUSED(b);
	return -1;
}

void rm_dbg_cmd(char *s)
{
	UNUSED(s);
}
#endif

#ifndef CONFIG_WIFI_11V_ENABLE
void rtw_wnm_process_btm_req(char *buf, int buf_len, int flags, void *userdata)
{
	UNUSED(buf);
	UNUSED(buf_len);
	UNUSED(flags);
	UNUSED(userdata);
}

void rtw_wnm_set_btm_flags(u8 flags)
{
	UNUSED(flags);
}

void wnm_dbg_cmd(char *buf, int buf_len, int flags, void *userdata)
{
	UNUSED(buf);
	UNUSED(buf_len);
	UNUSED(flags);
	UNUSED(userdata);
}
#endif

#if !defined(CONFIG_WIFI_11K_ENABLE) && !defined(CONFIG_WIFI_11V_ENABLE)
void rtw_wnm_btm_candidates_survey(char *buf, int buf_len, int flags, void *userdata)
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
void rtw_set_to_roam(u8 to_roam)
{
	UNUSED(to_roam);
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

int rtw_ft_derive_ptk(u8 *sa, u8 *ssid)
{
	UNUSED(sa);
	UNUSED(ssid);
	return -1;
}

int rtw_ft_validate_ie_eapol_3(u8 *buf, u32 len)
{
	UNUSED(buf);
	UNUSED(len);
	return -1;
}

u8 rtw_ft_build_eapol_2_ies(void *a)
{
	UNUSED(a);
	return -1;
}
#endif

#endif

