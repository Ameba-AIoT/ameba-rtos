#ifndef __EXAMPLE_WIFI_MANAGER_H__
#define __EXAMPLE_WIFI_MANAGER_H__

/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/

#define WM_ASSOC_SEMA_TIMEOUT  300000

void example_wifi_manager(void);
void example_wifi_manager_connect_cb(char *buf, int buf_len, int flags, void *userdata);
void example_wifi_manager_disconnect_cb(char *buf, int buf_len, int flags, void *userdata);

#endif //#ifndef __EXAMPLE_WIFI_MANAGER_H__
