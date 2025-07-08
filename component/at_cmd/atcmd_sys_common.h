/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_SYS_COMMON_H__
#define __ATCMD_SYS_COMMON_H__

#define ATCMD_VERSION          2       /* ATCMD MAJOR VERSION, AT FORMAT CHANGED, OR PORTING NEW CHIPS */
#define ATCMD_SUBVERSION       4       /* ATCMD MINOR VERSION, NEW COMMAND ADDED OR DELETED */
#define ATCMD_REVISION         2       /* ATCMD FIX BUG REVISION */

typedef enum atcmd_log_type_e {
	atcmd_log_type_get = 0,
	atcmd_log_type_set = 1,
	atcmd_log_type_print_all = 2,
	atcmd_log_type_clear_all = 3,
	atcmd_log_type_invalid
} atcmd_log_type;

void print_system_common_at(void);
void at_sys_init_common(void);
#endif
