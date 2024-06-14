/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_SYS_H__
#define __ATCMD_SYS_H__

#define ATCMD_VERSION          "V02"     /* ATCMD MAJOR VERSION, AT FORMAT CHANGED, OR PORTING NEW CHIPS */
#define ATCMD_SUBVERSION       "1"       /* ATCMD MINOR VERSION, NEW COMMAND ADDED OR DELETED */
#define ATCMD_REVISION         "1"       /* ATCMD FIX BUG REVISION */
#define SDK_VERSION            "v3.5"    /* SDK VERSION */

typedef enum atcmd_log_type_e {
	atcmd_log_type_get = 0,
	atcmd_log_type_set = 1,
	atcmd_log_type_print_all = 2,
	atcmd_log_type_clear_all = 3,
	atcmd_log_type_invalid
} atcmd_log_type;

typedef enum atcmd_cpuload_type_e {
	atcmd_cpuload_type_update = 0,
	atcmd_cpuload_type_sema = 1,
	atcmd_cpuload_type_tick = 2,
	atcmd_cpuload_type_stop = 3,
	atcmd_cpuload_type_invalid
} atcmd_cpuload_type;

extern void print_bt_ext_at(void);
extern void sys_reset(void);

void at_sys_init(void);
void print_system_at(void);
#endif
