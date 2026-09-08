
#include "rtw_coex_host_api.h"
#include <string.h>

static u8 last_channel = 0;

struct extchip_para_t g_extchip_para_ap = {0};

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

__weak bool rtk_coex_extc_is_ready(void)
{
	int ret = rtk_coex_ipc_h2c_info_handler(COEX_EXT_TYPE(COEX_H2C_EXT_GET_READY), NULL, 0);

	return (ret == 0) ? true : false;
}

__weak void rtk_coex_extc_ntfy_wpan_channel(u8 channel)
{
	if (channel != last_channel) {
		last_channel = channel;
		RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "[COEX][HOST][EXT] enter rtk_coex_extc_ntfy_wpan_channel %d\r\n", channel);
		rtk_coex_ipc_h2c_info_handler(COEX_EXT_TYPE(COEX_H2C_EXT_WPAN_CHANNEL), (u8 *)&channel, sizeof(channel));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
//////////////// called by host side , for c2h
////////////////////////////////////////////////////////////////////////////////////////////

__weak s32 rtk_coex_extc_get_init_params(struct extchip_para_t *p_extchip_para)
{
	RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "[COEX][HOST][EXT] enter rtk_coex_extc_get_init_params.\r\n");

	memcpy(p_extchip_para, &g_extchip_para_ap, sizeof(struct extchip_para_t));

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
