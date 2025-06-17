
#include "rtw_coex_host_api.h"

static u8 last_channel = 0;
static u8 last_wl_performance_req = EXT_PERF_INVALID;
static u32 last_ext_bt_profile = EXT_BT_UNDEF;

/*
 * function extern
*/
extern int rtk_coex_ipc_h2c_info_handler(u16 type, u8 *pdata, u16 data_len);

/*
 * function define
*/
////////////////////////////////////////////////////////////////////////////////////////////
//////////////// called by host side , for h2c
////////////////////////////////////////////////////////////////////////////////////////////

__weak void rtk_coex_extc_ntfy_wpan_channel(u8 channel)
{
	if (channel != last_channel) {
		last_channel = channel;
		RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "[EXT][HOST] enter rtk_coex_extc_ntfy_wpan_channel %d\r\n", channel);
		rtk_coex_ipc_h2c_info_handler(COEX_EXT_TYPE(COEX_H2C_EXT_WPAN_CHANNEL), (u8 *)&channel, sizeof(channel));
	}
}

__weak void rtk_coex_extc_ntfy_wl_performance_req(u8 performance_req)
{
	if (performance_req != last_wl_performance_req) {
		last_wl_performance_req = performance_req;
		RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "[EXT][HOST] enter rtk_coex_extc_ntfy_wl_performance_req %d\r\n", performance_req);
		rtk_coex_ipc_h2c_info_handler(COEX_EXT_TYPE(COEX_H2C_EXT_WL_PERFORMANCE_REQUEST), (u8 *)&performance_req, sizeof(performance_req));
	}
}

__weak void rtk_coex_extc_ntfy_bt_profile(u32 profile)
{
	if (profile != last_ext_bt_profile) {
		last_ext_bt_profile = profile;
		RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "[EXT][HOST] enter rtk_coex_extc_ntfy_bt_profile %d\r\n", profile);
		rtk_coex_ipc_h2c_info_handler(COEX_EXT_TYPE(COEX_H2C_EXT_BT_PROFILE), (u8 *)&profile, sizeof(profile));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
//////////////// called by host side , for c2h
////////////////////////////////////////////////////////////////////////////////////////////

// TODO

////////////////////////////////////////////////////////////////////////////////////////////
