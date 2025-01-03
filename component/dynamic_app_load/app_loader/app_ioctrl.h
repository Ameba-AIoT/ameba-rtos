/**
 ******************************************************************************
 * @file    app_ioctrl.h
 * @brief   Contains function for loading the app and setting up the freertos
 *          task
 ******************************************************************************
*/

#ifndef _APP_IOCTRL_H
#define _APP_IOCTRL_H

typedef enum {
	MOD_APP_CMD_TEST_PTHREAD = 0x0,
	MOD_APP_CMD_TEST_SOCKET = 0x1,
	MOD_APP_CMD_PASS_PARAMS = 0x2,
	MOD_APP_CMD_APP_MAIN = 0x3,
} app_mod_cmd_t;


typedef void (* func0_t)(void);
typedef void (* func1_t)(void *);

int app_ioctrl(void *mod, int cmd, void *args);

#endif
