/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "atcmd_service.h"

#ifdef CONFIG_SUPPORT_ATCMD

#ifndef CONFIG_MP_INCLUDED
#include "atcmd_mqtt.h"
#endif
#ifndef CONFIG_MP_SHRINK
#include "atcmd_wifi.h"
#endif

//======================================================
struct list_head log_hash[ATC_INDEX_NUM];

#include "at_intf_uart.h"
#include "lwip_netconf.h"

log_init_t log_init_table[] = {

#ifndef CONFIG_MP_SHRINK
#ifdef CONFIG_WLAN
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

#ifdef CONFIG_WLAN
#ifndef CONFIG_MP_INCLUDED
	at_mqtt_init,
#endif
#ifdef CONFIG_LWIP_LAYER
	at_tcpip_init,
#endif
#endif

#ifdef CONFIG_ATCMD_IO_UART
	atio_uart_init,
#endif
};


//======================================================
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

void atcmd_service_init(void)
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
void atcmd_service_add_table(log_item_t *tbl, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		log_add_new_command(&tbl[i]);
	}
}

void *atcmd_action(char *cmd)
{
	int index = hash_index(cmd) % ATC_INDEX_NUM;
	struct list_head *head = &log_hash[index];
	struct list_head *iterator;
	log_item_t *item;
	void *act = NULL;

	list_for_each(iterator, head) {
		item = list_entry(iterator, log_item_t, node);
		if (strcmp(item->log_cmd, cmd) == 0) {
			act = (void *)item->at_act;
			break;
		}
	}

	return act;
}

void *atcmd_handler(char *cmd)
{
	const char *atcmd_prefix = "AT";
	log_act_t action = NULL;
	char *copy = cmd;
	char *token = NULL;
	char *param = NULL;
	char tok[33] = {0};//'\0'
	char *tokSearch = NULL;
	int prefix_length = strlen(atcmd_prefix);

	if (strncmp(cmd, atcmd_prefix, prefix_length) != 0) {
		return NULL;
	}

	token = strsep(&copy, "=");
	param = strsep(&copy, "\0");

	/* The length of command header should be 4 bytes at least */
	if (token && (strlen(token) > 3)) {
		strncpy(tok, token, sizeof(tok) - 1);
		tokSearch = &tok[2];
	} else {
		return NULL;
	}
	action = (log_act_t)atcmd_action(tokSearch);

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
	/* The last charactor should be '\0'. */
	const int most_size = UART_LOG_CMD_BUFLEN - 1;
	int argc = 1, pos = 0, i = 0, j = 0, offset = 1;

	if (buf == NULL) {
		goto exit;
	}

	for (i = 0; most_size > i && '\0' != buf[i] && MAX_ARGC > argc; i += offset) {
		/* Escape charactor. */
		if ('\\' == buf[i]) {
			/* The next charactor should be comma or backslash. */
			if (',' != buf[i + 1] && '\\' != buf[i + 1]) {
				argc = -1;
				goto exit;
			}
			buf[j] = buf[i + 1];
			offset = 2;
		}
		/* Delimiter. */
		else if (',' == buf[i]) {
			buf[j] = '\0';
			argv[argc] = &buf[pos];
			argc++;
			pos = j + 1;
			offset = 1;
		}
		/* Other charactor. */
		else {
			buf[j] = buf[i];
			offset = 1;
		}
		j++;
	}

	/* The final arg. */
	if (MAX_ARGC > argc) {
		buf[j] = '\0';
		argv[argc] = &buf[pos];
		argc++;
	} else {
		argc = -1;
		goto exit;
	}

exit:
	return argc;
}

#ifdef CONFIG_WLAN
int mp_command_handler(char *cmd)
{
	char *start = "iwpriv ";
	int len = strlen(start);
	if (strncmp(cmd, start, len) == 0) {
#ifdef CONFIG_MP_INCLUDED
#if defined(CONFIG_AS_INIC_AP)
		inic_mp_command(cmd + len, strlen(cmd + len) + 1, 1);
#elif defined(CONFIG_SINGLE_CORE_WIFI)
		wext_private_command(cmd + len, 1, NULL);
#endif
#endif
		RTK_LOGS(NOTAG, "\n\r[MEM] After do cmd, available heap %d\n\r", rtos_mem_get_free_heap_size());
		RTK_LOGS(NOTAG, "\r\n\n#\r\n"); //"#" is needed for mp tool
		return TRUE;
	}

	return FALSE;
}
#endif

int atcmd_service(char *line_buf)
{
#ifdef CONFIG_LWIP_LAYER
	if (atcmd_lwip_tt_proc() == SUCCESS) {
		return TRUE;
	}
#endif

	if (atcmd_handler((char *)line_buf) == NULL) {
		return FALSE;
	}

	RTK_LOGS(NOTAG, "\n\r[MEM] After do cmd, available heap %d\n\r", rtos_mem_get_free_heap_size());
	RTK_LOGS(NOTAG, "\r\n\n#\r\n"); //"#" is needed for mp tool
	return TRUE;
}
#endif /* CONFIG_SUPPORT_ATCMD */
