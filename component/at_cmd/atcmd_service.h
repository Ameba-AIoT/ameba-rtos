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
#include "atcmd_fs.h"

#ifndef CONFIG_MP_INCLUDED
#if defined(CONFIG_ATCMD_SOCKET) && (CONFIG_ATCMD_SOCKET == 1)
#include "atcmd_sockets.h"
#endif
#endif

#if defined(CONFIG_BT) && CONFIG_BT
#if defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED
#include "atcmd_bt_mp.h"
#endif
#endif

#define ATC_INDEX_NUM 32

#ifndef MAX_ARGC
#define MAX_ARGC 23
#endif

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

int atcmd_tt_mode_start(u32 len);
u32 atcmd_tt_mode_get(u8 *buf, u32 len);
void atcmd_tt_mode_end(void);

#define MAX_TT_BUF_LEN 1024 * 10
#define ATCMD_TT_MODE_RING_BUF_SIZE 1024 * 20
#define ATCMD_MCU_CONTROL_INIT_STR "ATCMD READY\r\n"
#define ATCMD_OK_END_STR 		"\r\nOK\r\n"
#define ATCMD_ERROR_END_STR 	"\r\nERROR: %d\r\n"
#define ATCMD_ENTER_TT_MODE_STR	">>>\r\n"
#define ATCMD_EXIT_TT_MODE_STR	"<<<\r\n"
#define C_NUM_AT_CMD			4 //"ATxx", 4 characters
#define C_NUM_AT_CMD_DLT		1 //"=", 1 charater
#define STR_END_OF_ATCMD_RET	"\r\n\n# " //each AT command response will end with this string
#define STR_END_OF_ATDATA_RET	"\r\n\n> " //data transparent transmission indicator
#define SMALL_BUF               512
#define MAX_BUF_LEN             20000

#define PIN_VAL_TO_NAME_STR(val) \
	({ \
		char name[5]; \
		if (val < 0x20) { \
			snprintf(name, 5, "%s%d", "PA", val); \
		} else if (val < 0x40) { \
			snprintf(name, 5, "%s%d", "PB", val - 0x20); \
		} else if (val < 0x60) { \
			snprintf(name, 5, "%s%d", "PC", val - 0x40); \
		} \
		name;\
	})

typedef enum {
	CLIENT_CA = 0,
	CLIENT_CERT,
	CLIENT_KEY,
	SERVER_CA,
	SERVER_CERT,
	SERVER_KEY,
} CERT_TYPE;

enum {
	AT_MCU_CONTROL_UART = 1,
	AT_MCU_CONTROL_SPI,
	AT_MCU_CONTROL_SDIO,
};

int atcmd_get_ssl_certificate(char *buffer, CERT_TYPE cert_type, int index);
int atcmd_get_ssl_certificate_size(CERT_TYPE cert_type, int index);

/* TODO */
#ifdef CONFIG_ATCMD_MCU_CONTROL
typedef void (*at_write)(char *buf, int len);
extern uint16_t atcmd_switch;
extern char global_buf[SMALL_BUF];
extern at_write out_buffer;
int at_printf(const char *fmt, ...);
int at_printf_indicate(const char *fmt, ...);
#else
#define at_printf(fmt, args...)    RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, fmt, ##args)
#define at_printf_indicate(fmt, args...) \
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "$");\
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, fmt, ##args)
#endif

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
#if defined(CONFIG_CORE_AS_AP) || (!defined(CONFIG_CORE_AS_NP) && defined(CONFIG_FULLMAC_MENU))
extern void at_coex_init(void);
#endif

#endif /* ATCMD_SERVICE_H */