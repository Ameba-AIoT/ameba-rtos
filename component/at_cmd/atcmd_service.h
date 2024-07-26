/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ATCMD_SERVICE_H
#define ATCMD_SERVICE_H

#include "platform_autoconf.h"
#include "dlist.h"
/*
 * Include user defined options first. Anything not defined in these files
 * will be set to standard values. Override anything you dont like!
 */
#include "basic_types.h"
#include "diag.h"
#include "os_wrapper.h"

#include "atcmd_sys.h"
#include "atcmd_lwip.h"
#if defined(CONFIG_BT) && CONFIG_BT
#if defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED
#include "atcmd_bt_mp.h"
#endif
#endif

#define ATC_INDEX_NUM 32

#ifndef MAX_ARGC
#define MAX_ARGC 23
#endif

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

void atcmd_service_add_table(log_item_t *tbl, int len);
int parse_param(char *buf, char **argv);
int parse_param_advance(char *buf, char **argv);

#define C_NUM_AT_CMD			4 //"ATxx", 4 characters
#define C_NUM_AT_CMD_DLT		1 //"=", 1 charater
#define STR_END_OF_ATCMD_RET	"\r\n\n# " //each AT command response will end with this string
#define STR_END_OF_ATDATA_RET	"\r\n\n> " //data transparent transmission indicator
#define SMALL_BUF               128
#define BIG_BUF                 1024

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

/* TODO */
/* For compatibility with the old Bluetooth command, need cut down 4-letter command
  into two parts, e.g. Cut the top "AT" of "ATBE=peripheral,1", into "BE=peripheral,1"
  Because that, in function atcmd_handler ( ), the top 2-letter "AT" has been drawn already.
  This should be deleted once the new format commands are ready. */
#define ATCMD_BT_CUT_DOWN       1

#ifdef CONFIG_MP_INCLUDED
#ifdef CONFIG_AS_INIC_AP
extern void inic_mp_command(char *token, unsigned int cmd_len, int show_msg);
#else
extern int wext_private_command(char *cmd, int show_msg, char *user_buf);
#endif
#endif
#if defined(CONFIG_BT) && CONFIG_BT
extern void at_bt_init(void);
#endif

#endif /* ATCMD_SERVICE_H */