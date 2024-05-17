/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "log_service.h"

char zb_at_cmd_buf[LOG_SERVICE_BUFLEN] = {0};
char zb_change_channel = 0;

extern void zb_app_thread(void);

// ATZB
// ATZB=
static void fATZB(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};

	memset(zb_at_cmd_buf, 0, LOG_SERVICE_BUFLEN);

	if (arg) {
		strncpy(zb_at_cmd_buf, arg, sizeof(zb_at_cmd_buf));
		AT_PRINTK("fATZB:%s\r\n", zb_at_cmd_buf);
		argc = parse_param(zb_at_cmd_buf, argv);
	} else {
		zb_app_thread();
		return;
	}

	if (argc < 2 || argc > 8) {
		AT_PRINTK("[AT_PRINTK] ERROR: input parameter error!\n\r");
		goto exit;
	}

	// change channel
	if (strcmp(argv[1], "cc") == 0) {
		zb_change_channel = atoi(argv[2]);
		zb_change_channel |= 0x80;
		AT_PRINTK("fATZB: change channel %x\r\n", zb_change_channel);
	}

	return;
exit:
	AT_PRINTK("[ATBZ] run example: ATBZ\n\r");
	AT_PRINTK("[ATBZ] change channel: ATBZ=cc,chnl(d'11-d'26)\n\r");

}

log_item_t at_wpan_items[ ] = {
	{"ATZB", fATZB, {NULL, NULL}},
};

void at_wpan_init(void)
{
	log_service_add_table(at_wpan_items, sizeof(at_wpan_items) / sizeof(at_wpan_items[0]));
}

log_module_init(at_wpan_init);



