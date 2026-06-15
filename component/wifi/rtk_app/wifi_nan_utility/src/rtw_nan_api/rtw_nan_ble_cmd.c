#include <stdio.h>
#include <stdint.h>

enum rtw_ret_status {
	RTW_RET_STATUS_SUCCESS,
	RTW_RET_STATUS_ERROR,
};

enum ble_role {
	role_scanner = 0,
	role_advertiser = 1,
};

#define INFO_PRINT(...) do{ fprintf(stderr, __VA_ARGS__); } while(0)

enum rtw_ret_status rtw_nan_api_ble_init(enum ble_role role)
{
	INFO_PRINT("[rtw_ble] BLE stub: init (not supported)\n");
	return RTW_RET_STATUS_ERROR;
}

enum rtw_ret_status rtw_nan_api_ble_deinit()
{
	INFO_PRINT("[rtw_ble] BLE stub: deinit (not supported)\n");
	return RTW_RET_STATUS_ERROR;
}

enum rtw_ret_status rtw_nan_api_ble_start_ble_trigger(uint8_t trigger_nan)
{
	INFO_PRINT("[rtw_ble] BLE stub: start trigger (not supported)\n");
	return RTW_RET_STATUS_ERROR;
}

enum rtw_ret_status rtw_nan_api_ble_stop()
{
	INFO_PRINT("[rtw_ble] BLE stub: stop (not supported)\n");
	return RTW_RET_STATUS_ERROR;
}
