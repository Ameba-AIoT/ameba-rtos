/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_SYS_H__
#define __ATCMD_SYS_H__

#include "FreeRTOS.h"
#if (configGENERATE_RUN_TIME_STATS == 1)
#include "task.h"
#endif

#define ATCMD_VERSION          "V02"     /* ATCMD MAJOR VERSION, AT FORMAT CHANGED, OR PORTING NEW CHIPS */
#define ATCMD_SUBVERSION       "1"       /* ATCMD MINOR VERSION, NEW COMMAND ADDED OR DELETED */
#define ATCMD_REVISION         "1"       /* ATCMD FIX BUG REVISION */
#define SDK_VERSION            "v3.5"    /* SDK VERSION */

#ifndef CONFIG_INIC_NO_FLASH
#if (configGENERATE_RUN_TIME_STATS == 1)
typedef struct delta_status_s {
	int running_tick;
	const char *name;
	char bak_name[32];
	void *task;
	int task_id;
	int priority;
	eTaskState state;
} delta_status_t;

#define TASK_CNT 64
typedef struct task_status_s {
	TaskStatus_t status[2][TASK_CNT];
	TaskStatus_t *last;
	TaskStatus_t *curr;
	int last_cnt;
	int curr_cnt;
	delta_status_t delta[TASK_CNT];
} task_status_t;

typedef struct status_cmd_para_s {
	int time;
	int count;
} status_cmd_para_t;
#endif /* configGENERATE_RUN_TIME_STATS */
#endif /* CONFIG_INIC_NO_FLASH */

extern void print_system_at(void);

extern void sys_reset(void);

#endif
