/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"

#ifdef CONFIG_SUPPORT_ATCMD

#include "atcmd_service.h"

#ifdef __ZEPHYR__
char *strsep(char **stringp, const char *delim);
#endif

__attribute__((noinline)) void *atcmd_action(char *cmd)
{
	log_item_t *item = (log_item_t *)__cmd_table_start__;
	u32 cmd_mum = ((__cmd_table_end__ - __cmd_table_start__) / sizeof(log_item_t));
	u32	index ;
	void *act = NULL;

	for (index = 0; index < cmd_mum; index++) {
		if (strcmp(item->log_cmd, cmd) == 0) {
			act = (void *)item->at_act;
			break;
		}
		item++;
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
	char *argv[MAX_ARGC] = {0};
	int argc = 0;
	int i = 0;

	/* Validate input command */
	if (cmd == NULL) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[ATCMD] Invalid cmd input \r\n");
		return NULL;
	}

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
		/* Initialize argv array to NULL */
		for (i = 0; i < MAX_ARGC; i++) {
			argv[i] = NULL;
		}

		/* Parse parameters at top level before calling handler */
		/* Use parse_param_advance to support escaped characters like \, and \\ */
		argc = parse_param_advance(param, argv);

		/* Validate argc is within valid range */
		if (argc < 1 || argc >= MAX_ARGC) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[ATCMD] Invalid argc: %d\r\n", argc);
			return (void *)action;
		}

		action(argc, argv);
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
	const int most_size = CMD_BUFLEN - 1;
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
#endif /* CONFIG_SUPPORT_ATCMD */
