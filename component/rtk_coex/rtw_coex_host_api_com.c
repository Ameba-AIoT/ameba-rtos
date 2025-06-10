#include "wifi_api.h"
#include "rtw_coex_host_api.h"

/*
 * function extern
*/
extern int rtk_coex_ipc_h2c_info_handler(u16 type, u8 *pdata, u16 data_len);

/*
 * function define
*/
/////////////////////////////////////////////////////////////////////////////
/////// API for h2c (app side)
/////////////////////////////////////////////////////////////////////////////

__weak void rtk_coex_com_vendor_info_set(u8 vendor_id, u8 product_id)
{
	struct rtk_coex_vendor_info vendor_info = {
		.vendor_id = vendor_id,
		.product_id = product_id
	};
	rtk_coex_ipc_h2c_info_handler(COEX_COM_TYPE(COEX_H2C_COM_VENDOR_INFO_SET), (u8 *)&vendor_info, sizeof(struct rtk_coex_vendor_info));
}

__weak void rtk_coex_com_wl_slot_set(u8 wl_slot)
{
	rtk_coex_ipc_h2c_info_handler(COEX_COM_TYPE(COEX_H2C_COM_WL_SLOT_SET), &wl_slot, 1);
}

__weak void rtk_coex_com_state_get(void)
{
	rtk_coex_ipc_h2c_info_handler(COEX_COM_TYPE(COEX_H2C_COM_STATE_GET), NULL, 0);
}

/////////////////////////////////////////////////////////////////////////////
/////// API for c2h (app side)
/////////////////////////////////////////////////////////////////////////////
// ADD HERE
