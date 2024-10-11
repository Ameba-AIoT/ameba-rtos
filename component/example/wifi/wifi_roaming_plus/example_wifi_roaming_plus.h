#ifndef __EXAMPLE_WIFI_ROAMING_H__
#define __EXAMPLE_WIFI_ROAMING_H__

#include "wifi_intf_drv_to_app_basic.h"

//fast reconnect callback fun
extern int (*p_wifi_do_fast_connect)(void);
extern int (*p_store_fast_connect_info)(unsigned int data1, unsigned int data2);

void example_wifi_roaming_plus(void);
void example_wifi_roaming_plus_init(void);
#endif