#ifndef __EXAMPLE_WIFI_ROAMING_H__
#define __EXAMPLE_WIFI_ROAMING_H__

#include "wifi_intf_drv_to_app_basic.h"

//fast reconnect callback fun
extern wifi_do_fast_connect_ptr p_wifi_do_fast_connect;
extern write_fast_connect_info_ptr p_store_fast_connect_info;

void example_wifi_roaming_plus(void);
void example_wifi_roaming_plus_init(void);
#endif