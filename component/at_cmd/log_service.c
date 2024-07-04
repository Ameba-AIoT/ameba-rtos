/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "log_service.h"

#ifdef SUPPORT_LOG_SERVICE

#ifndef CONFIG_NEW_ATCMD
#include "atcmd_sys.h"
#ifdef CONFIG_LWIP_LAYER
#include "atcmd_lwip.h"
#endif
#if defined(CONFIG_BT) && CONFIG_BT
#if defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED
#include "atcmd_bt_mp.h"
#endif
#endif
#endif /* !CONFIG_NEW_ATCMD */
#ifndef CONFIG_MP_INCLUDED
#include "atcmd_mqtt.h"
#endif
#ifndef CONFIG_MP_SHRINK
#include "atcmd_wifi.h"
#endif

//======================================================
struct list_head log_hash[ATC_INDEX_NUM];

#ifdef CONFIG_NEW_ATCMD
#include "at_intf_uart.h"
#include "lwip_netconf.h"
#else
/* apply old atcmd, which should be deleted after new version is ready */
extern void at_wifi_init(void);
extern void at_sys_init(void);
#endif /* CONFIG_NEW_ATCMD */

void at_log_init(void);

log_init_t log_init_table[] = {

#ifndef CONFIG_MP_SHRINK
#if CONFIG_WLAN
	at_wifi_init,
#endif
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

#ifdef CONFIG_ATCMD_IO_UART
	atio_uart_init,
#endif
#endif
};


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

	for (i = 0; i < ATC_INDEX_NUM; i++) {
		INIT_LIST_HEAD(&log_hash[i]);
	}

	for (i = 0; i < sizeof(log_init_table) / sizeof(log_init_t); i++) {
		log_init_table[i]();
	}
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
	const char *mp_start = "iwpriv ";
	log_act_t action = NULL;
	char *copy = cmd;
	char *token = NULL;
	char *param = NULL;
	char tok[33] = {0};//'\0'
#ifdef CONFIG_NEW_ATCMD
	char *tokSearch = NULL;
#endif
	int mp_length = strlen(mp_start);

	/* It is a mp_command, which will not be executed here. */
	if (strncmp(cmd, mp_start, mp_length) == 0) {
		return NULL;
	}

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

/****************************************************************
  Single parameter divided by comma.
  The argv[0] is reserved as NULL, argv[1] is the 1st paramerter, argv[2] is the 2nd
  parameter, and son on.
  If there are consecutive commas, it means there are parameter(s) ignored.
  So that, the mandatory parameters' length should be checked not equal to 0.
  The optional parameters' length can be 0, which means absent.
  e.g.
  AT+XXX=param1,param2		<-	There are 2 parameters.
  AT+XXX=param1,,param3		<-	The param2 has length == 0, but argv[2] is not NULL.
  If you need let the comma(s) be part(s) of parameter, use parse_param_advance instead.
****************************************************************/
int parse_param(char *buf, char **argv)
{
	int argc = 1;
	char *temp = NULL;

	if (buf == NULL) {
		goto exit;
	}
	temp = buf;

	for (; argc < MAX_ARGC; argc++) {
		argv[argc] = strsep(&temp, ",");
		if (argv[argc] == NULL) {
			goto exit;
		}
	}

exit:
	return argc;
}

/****************************************************************
  If the string parameter need include comma(s), use this one instead of parse_param.
  The argv[0] is reserved as NULL, argv[1] is the 1st paramerter, argv[2] is the 2nd
  parameter, and son on.
  If return value argc == -1, it means there are invalid datas inside.
  e.g.
    AT+XXX=param1,head\,tail,param3		<- The 2nd parameter is string "head,tail".
	AT+XXX=param1,head\\tail,param3		<- The 2nd parameter is string "head\tail".
  The single backslash is not allowed, in other words, you should use double backslashes
  such as "\\" to express single backslash.
****************************************************************/
int parse_param_advance(char *buf, char **argv)
{
	int argc = 1;
	int buf_pos = 0, temp_pos = 0, length = 0;
	static char temp_buf[LOG_SERVICE_BUFLEN];
	char *segment = temp_buf;

	if (buf == NULL) {
		goto exit;
	}
	length = strlen(buf);
	if (length == 0) {
		goto exit;
	}
	/* If the length is longer than LOG_SERVICE_BUFLEN - 1, the tail will be cut. */
	length = MIN(length, LOG_SERVICE_BUFLEN - 1);

	while (buf_pos < length) {
		/* Escape charactor. */
		if (buf[buf_pos] == '\\') {
			buf_pos++;
			/* There are 2 escape charactors supported right now. */
			if ((buf[buf_pos] == ',') || (buf[buf_pos] == '\\')) {
				temp_buf[temp_pos] = buf[buf_pos];
				temp_pos++;
			} else {
				argc = -1;
				goto exit;
			}
		}
		/* Comma is considered as segmentasion. */
		else if (buf[buf_pos] == ',') {
			temp_buf[temp_pos] = '\0';
			temp_pos++;
			argv[argc] = segment;
			argc++;
			segment = &temp_buf[temp_pos];
		}
		/* Other characters. */
		else {
			temp_buf[temp_pos] = buf[buf_pos];
			temp_pos++;
		}
		buf_pos++;
	}

	/* The last one. */
	temp_buf[temp_pos] = '\0';
	argv[argc] = segment;
	argc++;

exit:
	return argc;
}

#ifndef CONFIG_NEW_ATCMD
unsigned char  gDbgLevel = AT_DBG_ERROR;
unsigned int   gDbgFlag  = 0xFFFFFFFF;
#endif

#if CONFIG_WLAN
int mp_command_handler(char *cmd)
{
	char *start = "iwpriv ";
	int len = strlen(start);
	if (strncmp(cmd, start, len) == 0) {
#ifdef CONFIG_MP_INCLUDED
#if defined(CONFIG_AS_INIC_AP)
		inic_mp_command(cmd + len, strlen(cmd + len), 1);
#elif defined(CONFIG_SINGLE_CORE_WIFI)
		wext_private_command(cmd + len, 1, NULL);
#endif
#endif
		return 0;
	}
	return -1;
}
#endif

void log_service(char *line_buf)
{
#if ((defined CONFIG_NEW_ATCMD) && (defined CONFIG_LWIP_LAYER))
	if (atcmd_lwip_tt_proc() == SUCCESS) {
		return;
	}
#endif

	if (log_handler((char *)line_buf) == NULL) {
#ifdef CONFIG_MP_INCLUDED
		if (mp_command_handler((char *)line_buf) < 0)
#endif
		{
			RTK_LOGS(NOTAG, "\r\nunknown command '%s'", line_buf);
		}
	}

	RTK_LOGS(NOTAG, "\n\r[MEM] After do cmd, available heap %d\n\r", rtos_mem_get_free_heap_size());
	RTK_LOGS(NOTAG, "\r\n\n#\r\n"); //"#" is needed for mp tool
}
#endif /* SUPPORT_LOG_SERVICE */
