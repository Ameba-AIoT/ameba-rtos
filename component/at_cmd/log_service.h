/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LOG_SERVICE_H
#define LOG_SERVICE_H

#include "platform_autoconf.h"
#include "dlist.h"
/*
 * Include user defined options first. Anything not defined in these files
 * will be set to standard values. Override anything you dont like!
 */
#include "platform_stdlib.h"
#include "basic_types.h"
#include "diag.h"
#include "os_wrapper.h"

#define log_module_init(fn)

#define ATC_INDEX_NUM 32

//LOG_SERVICE_BUFLEN: default, only 63 bytes could be used for keeping input
//                                     cmd, the last byte is for string end ('\0').
#define LOG_SERVICE_BUFLEN 100

#ifndef MAX_ARGC
#define MAX_ARGC 23
#endif

#define AT_BIT(n)           (1<<n)
#define AT_FLAG_DUMP        AT_BIT(0)
#define AT_FLAG_EDIT        AT_BIT(1)
#define AT_FLAG_ADC         AT_BIT(2)
#define AT_FLAG_GPIO        AT_BIT(3)
#define AT_FLAG_OTA         AT_BIT(4)
#define AT_FLAG_NFC         AT_BIT(5)
#define AT_FLAG_OS          AT_BIT(6)
#define AT_FLAG_LWIP        AT_BIT(7)
#define AT_FLAG_COMMON      AT_BIT(8)
#define AT_FLAG_WIFI        AT_BIT(9)
#define AT_FLAG_RDP         AT_BIT(10)

enum {
	AT_DBG_OFF = 0,
	AT_DBG_ALWAYS,
	AT_DBG_ERROR,
	AT_DBG_WARNING,
	AT_DBG_INFO
};

extern unsigned char  gDbgLevel;
extern unsigned int   gDbgFlag;

#define AT_PRINTK(...)			\
		do{							\
			DiagPrintf(__VA_ARGS__); 	\
			DiagPrintf("\r\n");			\
		}while(0)
#define _AT_PRINTK(...)	DiagPrintf(__VA_ARGS__)
#define AT_DBG_MSG(flag, level, ...)					\
		do{														\
			if(((flag) & gDbgFlag) && (level <= gDbgLevel)){	\
				AT_PRINTK(__VA_ARGS__);							\
			}													\
		}while(0)
#define _AT_DBG_MSG(flag, level, ...)					\
		do{														\
			if(((flag) & gDbgFlag) && (level <= gDbgLevel)){	\
				_AT_PRINTK(__VA_ARGS__);						\
			}													\
		}while(0)

typedef void (*log_init_t)(void);
typedef void (*log_act_t)(void *);
typedef struct _at_command_item_ {
	const char *log_cmd;
	log_act_t at_act;
	struct list_head node;
} log_item_t;

void log_service_add_table(log_item_t *tbl, int len);
int parse_param(char *buf, char **argv);

#define C_NUM_AT_CMD			4 //"ATxx", 4 characters
#define C_NUM_AT_CMD_DLT		1 //"=", 1 charater
#define STR_END_OF_ATCMD_RET	"\r\n\n# " //each AT command response will end with this string
#define STR_END_OF_ATDATA_RET	"\r\n\n> " //data transparent transmission indicator
#define SMALL_BUF               128
#define BIG_BUF                 1024

#ifdef CONFIG_NEW_ATCMD
/* TODO */
#ifdef CONFIG_ATCMD_IO_UART
typedef void (*at_write)(char *buf, int len);
extern uint16_t atcmd_switch;
extern char global_buf[SMALL_BUF];
extern at_write out_buffer;
int at_printf(const char *fmt, ...);
#else
#define at_printf(fmt, args...)    RTK_LOGS(NOTAG, fmt, ##args)
#endif
#endif

/* TODO */
/* For compatibility with the old Bluetooth command, need cut down 4-letter command
  into two parts, e.g. Cut the top "AT" of "ATBE=peripheral,1", into "BE=peripheral,1"
  Because that, in function log_handler ( ), the top 2-letter "AT" has been drawn already.
  This should be deleted once the new format commands are ready. */
#define ATCMD_BT_CUT_DOWN       1

#ifndef CONFIG_MP_INCLUDED
extern void print_mqtt_help(void);
extern void print_wlan_help(void);
#endif
extern void print_system_help(void);
#if defined(CONFIG_BT) && CONFIG_BT
extern void print_bt_help(void);
#endif

#endif
