/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ATCMD_SERVICE_H
#define ATCMD_SERVICE_H

/*
 * Include user defined options first. Anything not defined in these files
 * will be set to standard values. Override anything you dont like!
 */
#include "dlist.h"
#include "ameba_soc.h"
#include "os_wrapper.h"

#if (defined CONFIG_ATCMD_HOST_CONTROL && (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE))
#include "ringbuffer.h"
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
int atcmd_tt_mode_get(u8 *buf, u32 len);
void atcmd_tt_mode_end(void);

int at_printf_data(char *data, u32 len);
void at_printf_lock(void);
void at_printf_unlock(void);

#define MAX_TT_BUF_LEN 1024 * 10
#define MAX_TT_HEAP_SIZE 1024 * 80
#define TT_MODE_HIGH_WATERMARK 0.7
#define TT_MODE_LOW_WATERMARK 0.2
#define ATCMD_HOST_CONTROL_INIT_STR "ATCMD READY\r\n"
#define ATCMD_OK_END_STR 		"\r\nOK\r\n"
#define ATCMD_ERROR_END_STR 	"\r\nERROR: %d\r\n"
#define ATCMD_ENTER_TT_MODE_STR	">>>\r\n"
#define ATCMD_EXIT_TT_MODE_STR	"<<<\r\n"
#define ATCMD_TT_MODE_HIGH_WATERMARK_STR	"[$][TT]:High Watermark\r\n"
#define ATCMD_TT_MODE_LOW_WATERMARK_STR	"[$][TT]:Low Watermark\r\n"
#define ATCMD_DOWNSTREAM_TEST_START_STR "Downstream Test Start\r\n"
#define ATCMD_DOWNSTREAM_TEST_END_STR "Downstream Test End\r\n"
#define SMALL_BUF               512
#define MAX_BUF_LEN             20000

#if (defined CONFIG_ATCMD_HOST_CONTROL && (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE))
extern char g_host_control_mode;
extern char g_tt_mode;
extern char g_tt_mode_check_watermark;
extern char g_tt_mode_indicate_high_watermark;
extern char g_tt_mode_indicate_low_watermark;
extern RingBuffer *atcmd_tt_mode_rx_ring_buf;
extern rtos_sema_t atcmd_tt_mode_sema;
extern volatile char g_tt_mode_stop_flag;
extern volatile u8 g_tt_mode_stop_char_cnt;
extern rtos_timer_t xTimers_TT_Mode;
#endif

extern char pin_name[5];
#define PIN_VAL_TO_NAME_STR(val) \
	({ \
		if (val < 0x20 && val > 0) { \
			snprintf(pin_name, 5, "%s%d", "PA", val); \
		} else if (val < 0x40 && val >= 0x20) { \
			snprintf(pin_name, 5, "%s%d", "PB", val - 0x20); \
		} else if (val < 0x60 && val >= 0x40) { \
			snprintf(pin_name, 5, "%s%d", "PC", val - 0x40); \
		} \
		pin_name;\
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
	AT_HOST_CONTROL_UART = 1,
	AT_HOST_CONTROL_SPI,
	AT_HOST_CONTROL_SDIO,
	AT_HOST_CONTROL_USB,
};

int atcmd_get_ssl_certificate(char *buffer, CERT_TYPE cert_type, int index);
int atcmd_get_ssl_certificate_size(CERT_TYPE cert_type, int index);

/* TODO */
#if (defined CONFIG_ATCMD_HOST_CONTROL && (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE))
typedef void (*at_write)(char *buf, int len);
extern uint16_t atcmd_switch;
extern char global_buf[SMALL_BUF];
extern at_write out_buffer;
int at_printf(const char *fmt, ...);
int at_printf_indicate(const char *fmt, ...);
#else
#define at_printf(fmt, args...)    RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, fmt, ##args)
#define at_printf_indicate(fmt, args...) \
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[$]");\
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, fmt, ##args)
#endif

#ifdef CONFIG_MP_INCLUDED
#ifdef CONFIG_WHC_HOST
extern void whc_ipc_host_api_mp_command(char *token, unsigned int cmd_len, int show_msg);
#else
extern int wext_private_command(char *cmd, int show_msg, char *user_buf);
#endif
#endif
#if defined(CONFIG_BT) && CONFIG_BT
extern void at_bt_init(void);
#endif

#endif /* ATCMD_SERVICE_H */