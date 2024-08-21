/**
  ******************************************************************************
  * @file    wifi_intf_drv_to_upper.h
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef __RTW_INTF_USER_H__
#define __RTW_INTF_USER_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include "wifi_conf.h"

#define		_1M_RATE_		0
#define		_2M_RATE_		1
#define		_5M_RATE_		2
#define		_11M_RATE_		3
#define		_6M_RATE_		4
#define		_9M_RATE_		5
#define		_12M_RATE_		6
#define		_18M_RATE_		7
#define		_24M_RATE_		8
#define		_36M_RATE_		9
#define		_48M_RATE_		10
#define		_54M_RATE_		11

enum WIFI_INDICATE_MODE {
	WIFI_INDICATE_DISABLE = 0,
	WIFI_INDICATE_NORMAL = 1,
	WIFI_INDICATE_WILD = 2,
	WIFI_INDICATE_MAX,
};
//----- ------------------------------------------------------------------
// Wlan Interface opened for upper layer
//----- ------------------------------------------------------------------
void wifi_if1_init(void);
int wifi_if2_init(void);
void wifi_if1_deinit(void);
void wifi_if2_deinit(void);
int wifi_if1_open(void);
void wifi_if2_open(void);
void wifi_stop_bss_network(void);
void wifi_set_platform_rom_func(void *(*calloc_func)(size_t, size_t),
								void (*free_func)(void *),
								int (*rand_func)(void *, unsigned char *, size_t));
void wifi_heap_statistics(u8 start);
void wifi_ram_statistic_print(u8 print_detail);
void wifi_set_rom2flash(void);
void wifi_set_rom2flash_user_config(void);

extern int rtw_disconnect(void);
extern int rtw_joinbss_start_api(struct _rtw_network_info_t *connect_param);
extern int rtw_ap_start_api(struct _rtw_softap_info_t *softAP_config, unsigned char value);
extern int rtw_scan_start_api(struct _rtw_scan_param_t *scan_param, unsigned char block);
extern void rtw_joinbss_by_roaming(struct _rtw_network_info_t *connect_param);

extern int wifi_hal_iwpriv_command(unsigned char wlan_idx, char *cmd, int show_msg);
#define rtw_iwpriv_command(wlan_idx, cmd,show_msg)	wifi_hal_iwpriv_command(wlan_idx, cmd,show_msg)

#ifdef CONFIG_NAN
int nan_intfs_init(void);
int nan_intfs_deinit(void);
int rtw_start_nan_api(u8 master_pref, u8 band_support);
void rtw_stop_nan_api(void);
void rtw_nan_func_set_parameter(void *func_param, void **nan_func_pointer);
int rtw_add_nan_func(void *func_param, void *nan_func_pointer);
int rtw_del_nan_func(u64 cookie);
void rtw_cfgvendor_cmd_process(u16 vendor_cmd, void *data, u32 len);
#endif
#ifdef CONFIG_P2P
void rtw_p2p_set_role(enum rtw_p2p_role role);
int rtw_p2p_remain_on_ch(u8 wlan_idx, u8 enable);
#endif
#ifdef INIC_SKIP_NP_MSG_TASK
void rtw_single_thread_wakeup(void);
#endif

#ifdef CONFIG_WIFI_TUNNEL
extern int wifi_tunnel_command(char *cmd);
#define rtw_wltunnel_command(cmd) wifi_tunnel_command(cmd)
#endif
#ifdef	__cplusplus
}
#endif

#endif //#ifndef __RTW_INTF_USER_H__
