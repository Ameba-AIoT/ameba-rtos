#ifndef __EXAMPLE_WIFI_ROAMING_H__
#define __EXAMPLE_WIFI_ROAMING_H__

#include "wifi_api.h"

//fast reconnect callback fun
extern int (*p_wifi_do_fast_connect)(void);
extern int (*p_store_fast_connect_info)(unsigned int data1, unsigned int data2);

void example_wifi_roaming_plus(void);
#endif