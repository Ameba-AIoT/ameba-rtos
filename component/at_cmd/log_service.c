/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
#include "ameba_pmu.h"
#endif
#include "log_service.h"
#include "main.h"
#ifdef CONFIG_LWIP_LAYER
#include "atcmd_lwip.h"
#endif

#ifdef SUPPORT_LOG_SERVICE
//======================================================
struct list_head log_hash[ATC_INDEX_NUM];

extern void at_fs_init(void);
//extern void at_app_init(void);
extern void at_mp_init(void);
#if defined(CONFIG_BT) && CONFIG_BT
extern void at_bt_init(void);
#endif
#if defined(CONFIG_802154_ZIGBEE_EN)
extern void at_wpan_init(void);
#endif

#ifdef CONFIG_MP_INCLUDED
#ifdef CONFIG_AS_INIC_AP
extern void inic_mp_command(char *token, unsigned int cmd_len, int show_msg);
#else
extern int wext_private_command(char *cmd, int show_msg, char *user_buf);
#endif
#endif

#ifdef CONFIG_NEW_ATCMD
#include "at_intf_uart.h"
#include "lwip_netconf.h"

extern void at_sys_init(void);
#ifndef CONFIG_MP_SHRINK
extern void at_wifi_init(void);
#endif
#ifndef CONFIG_MP_INCLUDED
extern void at_mqtt_init(void);
#endif
#ifdef CONFIG_LWIP_LAYER
extern void at_tcpip_init(void);
#ifdef SUPPORT_LOG_SERVICE
extern char log_buf[UART_LOG_CMD_BUFLEN];
#endif
#endif
#if defined(CONFIG_BT) && CONFIG_BT
extern void at_bt_init(void);
#if defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED
extern void at_mp_init(void);
#endif
#endif

_WEAK
struct static_ip_config user_static_ip;

#else
/* apply old atcmd, which should be deleted after new version is ready */
#include "atcmd_wifi.h"
extern void at_wifi_init(void);
extern void at_sys_init(void);
#endif /* CONFIG_NEW_ATCMD */

void at_log_init(void);

#if defined (__ICCARM__) || defined ( __CC_ARM   ) || defined(__GNUC__)
//#pragma section=".data.log_init"
log_init_t *__log_init_begin__;
log_init_t *__log_init_end__;
log_init_t log_init_table[] = {

#ifndef CONFIG_MP_SHRINK
#if CONFIG_WLAN
	at_wifi_init,
#endif
#endif

#if 0 /* defined(CONFIG_802154_ZIGBEE_EN) */
	at_wpan_init,
#endif

#if defined(CONFIG_BT) && CONFIG_BT
	at_bt_init,
#if defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED
	at_mp_init,
#endif
#endif

	at_sys_init,

#ifdef CONFIG_NEW_ATCMD
#ifndef CONFIG_MP_INCLUDED
	at_mqtt_init,
#endif
#ifdef CONFIG_LWIP_LAYER
	at_tcpip_init,
#endif
#endif
};
#else
#error "not implement, add to linker script"
extern unsigned int __log_init_begin__;
extern unsigned int __log_init_end__;
#endif

#if defined(__GNUC__)
#if defined(CONFIG_AS_INIC_AP) || defined(CONFIG_SINGLE_CORE_WIFI)
#define _strsep strsep
#endif
#endif

//======================================================
#ifdef CONFIG_NEW_ATCMD
/* TODO */
#ifdef CONFIG_ATCMD_IO_UART
char global_buf[SMALL_BUF];
/* Out callback function */
at_write out_buffer;

/**
 * @brief Output format strings, like printf.
 * @param fmt: format string.
 * @return The length of the output string or error code.
 * @retval -1: encoding error or format string is tool long.
 * @retval others: The length of the output string.
 */
int at_printf(const char *fmt, ...)
{
	int ret = -1;
	int len_fmt = 0;
	char *buf;

	va_list ap;
	va_start(ap, fmt);
	len_fmt = vsnprintf(global_buf, SMALL_BUF, fmt, ap);
	va_end(ap);

	if (len_fmt < 0) {
		goto fail;
	}

	if (len_fmt < SMALL_BUF) {
		buf = global_buf;
	} else {
		buf = (char *)rtos_mem_malloc(BIG_BUF);
		va_start(ap, fmt);
		len_fmt = vsnprintf(buf, BIG_BUF, fmt, ap);
		va_end(ap);
	}

	if (len_fmt < 0 || len_fmt >= BIG_BUF) {
		goto fail;
	}

	ret = len_fmt;

	if (out_buffer) {
		out_buffer(buf, len_fmt);
	}

	if (len_fmt >= SMALL_BUF) {
		rtos_mem_free(buf);
	}

	return ret;

fail:
	return ret;
}
#endif
#endif

int hash_index(const char *str)
{
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;

	while (*str) {
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}

void log_add_new_command(log_item_t *new)
{
	int index = hash_index(new->log_cmd) % ATC_INDEX_NUM;

	list_add(&new->node, &log_hash[index]);
}

void log_service_init(void)
{
	unsigned int i;

#if defined (__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
	__log_init_begin__ = log_init_table;
	__log_init_end__ = __log_init_begin__ + sizeof(log_init_table) / sizeof(log_init_t);
#else
#error "not implement"
#endif

	for (i = 0; i < ATC_INDEX_NUM; i++) {
		INIT_LIST_HEAD(&log_hash[i]);
	}

	for (i = 0; i < (unsigned int)(__log_init_end__ - __log_init_begin__); i++) {
		log_init_table[i]();
	}

#if (defined(CONFIG_NEW_ATCMD) && defined(CONFIG_ATCMD_IO_UART))
	atio_uart_init();
#endif
}

//sizeof(log_items)/sizeof(log_items[0])
void log_service_add_table(log_item_t *tbl, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		log_add_new_command(&tbl[i]);
	}
}

void *log_action(char *cmd)
{
	int search_cnt = 0;
	int index = hash_index(cmd) % ATC_INDEX_NUM;
	struct list_head *head = &log_hash[index];
	struct list_head *iterator;
	log_item_t *item;
	void *act = NULL;

	list_for_each(iterator, head) {
		item = list_entry(iterator, log_item_t, node);
		search_cnt++;
		if (strcmp(item->log_cmd, cmd) == 0) {
			act = (void *)item->at_act;
			break;
		}
	}

	return act;
}

void *log_handler(char *cmd)
{
	log_act_t action = NULL;
	char buf[LOG_SERVICE_BUFLEN];
	memset(buf, 0, LOG_SERVICE_BUFLEN);
	char *copy = buf;
	char *token = NULL;
	char *param = NULL;
	char tok[33] = {0};//'\0'
#ifdef CONFIG_NEW_ATCMD
	char *tokSearch = NULL;
#endif
	strncpy(copy, cmd, LOG_SERVICE_BUFLEN - 1);

	token = strsep(&copy, "=");
	param = strsep(&copy, "\0");
#ifdef CONFIG_NEW_ATCMD
	/* The length of command header should be 4 bytes at least */
	if (token && (strlen(token) > 3)) {
		strncpy(tok, token, sizeof(tok) - 1);
		tokSearch = &tok[2];
		/* Check the first 2 letters. */
		if ('A' != tok[0] || 'T' != tok[1]) {
			return NULL;
		}
	} else {
		return NULL;
	}
	action = (log_act_t)log_action(tokSearch);
#else /* CONFIG_NEW_ATCMD */
	if (token && (strlen(token) > 0)) {
		strncpy(tok, token, sizeof(tok) - 1);
	} else {
		return NULL;
	}
	action = (log_act_t)log_action(tok);
#endif /* CONFIG_NEW_ATCMD */

	if (action) {
		action(param);
	}
	return (void *)action;

}

int parse_param(char *buf, char **argv)
{

	int argc = 1;
	char str_buf[LOG_SERVICE_BUFLEN];
	memset(str_buf, 0, LOG_SERVICE_BUFLEN);
	int str_count = 0;
	int buf_cnt = 0;
	static char temp_buf[LOG_SERVICE_BUFLEN];
	char *buf_pos = temp_buf;
	memset(temp_buf, 0, sizeof(temp_buf));

	if (buf == NULL) {
		goto exit;
	}
	strncpy(temp_buf, buf, LOG_SERVICE_BUFLEN - 1);

	while ((argc < MAX_ARGC) && (*buf_pos != '\0')) {
		while ((*buf_pos == ',') || (*buf_pos == '[') || (*buf_pos == ']')) {
			*buf_pos = '\0';
			buf_pos++;
		}

		if (*buf_pos == '\0') {
			break;
		} else if (*buf_pos == '"') {
			memset(str_buf, '\0', LOG_SERVICE_BUFLEN);
			str_count = 0;
			buf_cnt = 0;
			*buf_pos = '\0';
			buf_pos ++;
			if (*buf_pos == '\0') {
				break;
			}
			argv[argc] = buf_pos;
			while ((*buf_pos != '"') && (*buf_pos != '\0')) {
				if (*buf_pos == '\\') {
					buf_pos ++;
					buf_cnt++;
				}
				str_buf[str_count] = *buf_pos;
				str_count++;
				buf_cnt++;
				buf_pos ++;
			}
			*buf_pos = '\0';
			memcpy(buf_pos - buf_cnt, str_buf, buf_cnt);
		} else {
			argv[argc] = buf_pos;
		}
		argc++;
		buf_pos++;

		while ((*buf_pos != ',') && (*buf_pos != '\0') && (*buf_pos != '[') && (*buf_pos != ']')) {
			buf_pos++;
		}
	}
exit:
	return argc;
}

unsigned char  gDbgLevel = AT_DBG_ERROR;
unsigned int   gDbgFlag  = 0xFFFFFFFF;
void at_set_debug_level(unsigned char newDbgLevel)
{
	gDbgLevel = newDbgLevel;
}

void at_set_debug_mask(unsigned int newDbgFlag)
{
	gDbgFlag = newDbgFlag;
}

void print_help_msg(void)
{
#ifdef CONFIG_NEW_ATCMD /* TODO: Delete this later. */
	print_system_help();
#if !defined(CONFIG_MP_SHRINK) && CONFIG_WLAN
	print_wlan_help();
#endif
#ifndef CONFIG_MP_INCLUDED
	print_mqtt_help();
#endif
#if defined(CONFIG_BT) && CONFIG_BT
	print_bt_help();
#endif
#if 0
	print_tcpip_help();
#endif
#endif /* CONFIG_NEW_ATCMD */
}

#if CONFIG_WLAN
int mp_commnad_handler(char *cmd)
{
	char buf[128];
	char *pbuf = buf;
	char *token = NULL;
	memset(buf, 0, 128);

	//strncpy(buf, cmd, sizeof(buf));
	strncpy(buf, cmd, (128 - 1));
	token = strsep(&pbuf, " ");
	if (token && (strcmp(buf, "iwpriv") == 0)) {
		token = strsep(&pbuf, "");
#ifdef CONFIG_MP_INCLUDED
#ifdef CONFIG_AS_INIC_AP
		inic_mp_command(token, sizeof(buf), 1);
#else
		wext_private_command(token, 1, NULL);
#endif
#endif
		return 0;
	}
	return -1;
}
#endif

int print_help_handler(char *cmd)
{
	if (strcmp(cmd, "help") == 0) {
		print_help_msg();
		return 0;
	}
	return -1;
}

void log_service(char *line_buf)
{
#if ((defined CONFIG_NEW_ATCMD) && (defined CONFIG_LWIP_LAYER) && (defined SUPPORT_LOG_SERVICE))
	if (atcmd_lwip_tt_mode == TRUE) {
		atcmd_lwip_tt_datasize = strlen(log_buf);
		atcmd_lwip_tt_lasttickcnt = rtos_time_get_current_system_time_ms();
		if (atcmd_lwip_tt_sema != NULL) {
			rtos_sema_give(atcmd_lwip_tt_sema);
		}
		return;
	}
#endif
	if (log_handler((char *)line_buf) == NULL) {
#ifdef CONFIG_MP_INCLUDED
		if (mp_commnad_handler((char *)line_buf) < 0)
#endif
		{
			if (print_help_handler((char *)line_buf) < 0) {
				RTK_LOGS(NOTAG, "\r\nunknown command '%s'", line_buf);
			}
		}
	}

	RTK_LOGS(NOTAG, "\n\r[MEM] After do cmd, available heap %d\n\r", rtos_mem_get_free_heap_size());
	RTK_LOGS(NOTAG, "\r\n\n#\r\n"); //"#" is needed for mp tool
}

#endif
