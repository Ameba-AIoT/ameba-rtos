/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "atcmd_service.h"
#include "atcmd_sys_common.h"
#include "ameba_rtos_version.h"
#include "build_info.h"
#include "atcmd_sys.h"
#include "sys_api.h"
#include "stdlib.h"
//#include "FreeRTOS.h"
#include "os_wrapper_specific.h"
#if (configGENERATE_RUN_TIME_STATS == 1)
#include "task.h"
#endif

extern u32 cmd_dump_word(u16 argc, u8  *argv[]);
extern u32 cmd_write_word(u16 argc, u8  *argv[]);
static const char *const TAG = "AT-SYSCOMMON";

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

#ifdef CONFIG_MP_INCLUDED
#if defined(configUSE_TRACE_FACILITY) && (configUSE_TRACE_FACILITY == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS == 1)
extern void vTaskList(char *pcWriteBuffer);
#endif
#endif

#ifndef CONFIG_INIC_NO_FLASH
#if (configGENERATE_RUN_TIME_STATS == 1)
static task_status_t *task_status = NULL;
static void *top_exit_sema = NULL;
static u32 last_tick = 0;
static status_cmd_para_t para_in;

TaskStatus_t *find_status(UBaseType_t id, TaskStatus_t *status, int cnt)
{
	for (int i = 0; i < cnt; i++) {
		if (id == status[i].xTaskNumber) {
			return &status[i];
		}
	}

	return NULL;
}

delta_status_t *find_free_delta(delta_status_t *delta, int cnt)
{
	for (int i = 0; i < cnt; i++) {
		if (delta[i].task == NULL) {
			return &delta[i];
		}
	}

	return NULL;
}

static int update_status(void)
{
	delta_status_t *deltaone = NULL;
	/* init */
	if (task_status->last == NULL && task_status->curr == NULL) {
		task_status->last = task_status->status[0];
		task_status->curr = task_status->status[1];
	}

	if (uxTaskGetNumberOfTasks() > TASK_CNT) {
		RTK_LOGW(NOTAG, "number of tasks : %d(exceed TASK_CNT)! Please enlarge TASK_CNT\r\n", uxTaskGetNumberOfTasks());
		return -1;
	}

	/* update last */
	task_status->last_cnt = task_status->curr_cnt;
	TaskStatus_t *tmp = task_status->last;
	task_status->last = task_status->curr;
	task_status->curr = tmp;

	/* update curr */
	task_status->curr_cnt = uxTaskGetNumberOfTasks();
	task_status->curr_cnt = uxTaskGetSystemState(task_status->curr, task_status->curr_cnt, NULL);
	memset(task_status->delta, 0, TASK_CNT * sizeof(delta_status_t));

	/* update delta */
	if (task_status->last_cnt > 0) {
		for (int i = 0; i < task_status->curr_cnt; i++) {
			/* find in last */
			TaskStatus_t *lastone = find_status(task_status->curr[i].xTaskNumber, task_status->last, task_status->last_cnt);

			delta_status_t *deltanew = find_free_delta(task_status->delta, TASK_CNT);
			deltanew->task = task_status->curr[i].xHandle;
			deltanew->name = task_status->curr[i].pcTaskName;
			deltanew->task_id = task_status->curr[i].xTaskNumber;
			deltanew->running_tick = 0;
			deltanew->state = task_status->curr[i].eCurrentState;
			deltanew->priority = task_status->curr[i].uxCurrentPriority;
			strncpy(deltanew->bak_name, deltanew->name, 31);
			deltaone = deltanew;

			if (deltaone && lastone) {
				deltaone->running_tick = task_status->curr[i].ulRunTimeCounter - lastone->ulRunTimeCounter;
				deltaone->state = task_status->curr[i].eCurrentState;
			} else if (deltaone && !lastone) {
				deltaone->running_tick = task_status->curr[i].ulRunTimeCounter;
				deltaone->state = task_status->curr[i].eCurrentState;
			}
		}
	}

	return 0;
}

void print_delta(int delta_tick)
{
	char c_state[5] = {'R', 'r', 'b', 's', 'D'};
	char CPU_decimal_string[3] = {0};
	int CPU_1000, CPU_integer, CPU_decimal;

	at_printf("%s\t%s\t%s\t%s\n\r", "task name", "state", "prio", "CPU%");
	for (int i = 0; i < TASK_CNT; i++) {
		memset(CPU_decimal_string, 0, sizeof(CPU_decimal_string));
		CPU_1000 = (float)task_status->delta[i].running_tick * 100 / (float)delta_tick * 100;
		CPU_integer = CPU_1000 / 100;
		CPU_decimal = CPU_1000 % 100;

		for (int j = 0; j < 2; j++) {
			CPU_decimal *= 10;
			CPU_decimal_string[j] = CPU_decimal / 100 + 48; // number to ASCII
			CPU_decimal = CPU_decimal % 100;
		}

		if (task_status->delta[i].task != NULL) {
			at_printf("%s\t%c\t%d\t%d.%s\r\n", task_status->delta[i].bak_name, c_state[task_status->delta[i].state], task_status->delta[i].priority,
					  CPU_integer, CPU_decimal_string);
		}
	}
}

void cpu_stat_thread(void *dummy)
{
	status_cmd_para_t *ppara = rtos_mem_malloc(sizeof(status_cmd_para_t));
	if (NULL == ppara) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "%s malloc failed\r\n", __FUNCTION__);
		goto end;
	}
	memcpy(ppara, dummy, sizeof(status_cmd_para_t));
	last_tick = portGET_RUN_TIME_COUNTER_VALUE();
	while ((rtos_sema_take((rtos_sema_t)top_exit_sema, ppara->time * 1000) == RTK_FAIL)) {
		if (update_status()) {
			continue;
		}
		int delta_tick =  portGET_RUN_TIME_COUNTER_VALUE() - last_tick;
		last_tick = portGET_RUN_TIME_COUNTER_VALUE();
		print_delta(delta_tick);

		if (0 < ppara->count) {
			ppara->count--;
		} else if (0 == ppara->count) {
			break;
		}
	}
	rtos_mem_free(ppara);

end:
	rtos_sema_delete((rtos_sema_t)top_exit_sema);
	top_exit_sema = NULL;
	rtos_mem_free(task_status);
	task_status = NULL;
	rtos_task_delete(NULL);
}
#endif /* configGENERATE_RUN_TIME_STATS */


#if (configGENERATE_RUN_TIME_STATS == 1)
static void at_cpuload_help(void)
{
	RTK_LOGI(TAG, "\r\n");
	RTK_LOGI(TAG, "AT+CPULOAD=<mode>[,<time_intval>,<counter>]\r\n");
	RTK_LOGI(TAG, "\t<mode>:\t[0,2]\r\n");
	RTK_LOGI(TAG, "\t<time_intval>:\tIn sec\r\n");
	RTK_LOGI(TAG, "\t<counter>\t0 means infinit\r\n");
}

/****************************************************************
AT command process:
	AT+CPULOAD
	[+CPULOAD]: OK
****************************************************************/
void at_cpuload(void *arg)
{
	const int mode_idx = 1, time_idx = 2, count_idx = 3;
	int argc = 0;
	int error_no = 0;
	char *argv[MAX_ARGC] = {0};
	enum atcmd_cpuload_type_e top_mode = atcmd_cpuload_type_invalid;

	para_in.time = 1;
	para_in.count = -1;

	argc = parse_param(arg, argv);
	if (argc > 4 || argc < 2) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS,
				 "[top]Usage: top=mode,time,count\n\r mode: 0, start count cpu usage every [time] second.\r\n mode: 1, stop mode 0.\r\n mode: 2: start count cpu usage.\r\n mode: 3: stop mode 2.\r\n "
				 "time: CPU statistics interval.Default 1. unit(s) \r\n count: CPU statistics count, default until stop or 1,2,3...");
		error_no = 1;
		goto end;
	}

	if (strlen(argv[mode_idx]) != 0) {
		top_mode = atoi(argv[mode_idx]);
	}
	if (argc > time_idx && strlen(argv[time_idx]) != 0) {
		para_in.time = (atoi(argv[2])) > 0 ? (atoi(argv[2])) : 1;
	}
	if (argc > count_idx && strlen(argv[count_idx]) != 0) {
		para_in.count = (atoi(argv[3])) > 0 ? (atoi(argv[3])) - 1 : -1;
	}

	if (NULL == task_status) {
		task_status = (task_status_t *)rtos_mem_malloc(sizeof(task_status_t));
		if (NULL == task_status) {
			RTK_LOGI(NOTAG, "Memory failure for task_status\r\n");
			error_no = 2;
			goto end;
		}
	}

	at_printf("current mode: %d time: %d(s) count: %d (-1 for infinite)\r\n", top_mode, para_in.time, para_in.count);
	switch (top_mode) {
	case atcmd_cpuload_type_update:
		if (top_exit_sema)	{
			break;
		}
		memset(task_status, 0, sizeof(task_status_t));
		if (update_status()) {
			error_no = 4;
			break;
		}
		rtos_sema_create(&top_exit_sema, 0, 1);
		rtos_task_create(NULL, ((const char *)"cpu_stat_thread"), cpu_stat_thread, &para_in, 4096 * 4, configMAX_PRIORITIES - 1);
		break;
	case atcmd_cpuload_type_sema:
		if (top_exit_sema) {
			rtos_sema_give((rtos_sema_t) top_exit_sema);
		}
		break;
	case atcmd_cpuload_type_tick:
		if (top_exit_sema)	{
			break;
		}
		memset(task_status, 0, sizeof(task_status_t));
		last_tick = portGET_RUN_TIME_COUNTER_VALUE();
		if (update_status()) {
			error_no = 4;
		}
		break;
	case atcmd_cpuload_type_stop:
		if (top_exit_sema)	{
			break;
		}
		if (update_status()) {
			error_no = 4;
			break;
		}
		print_delta(portGET_RUN_TIME_COUNTER_VALUE() - last_tick);
		last_tick = portGET_RUN_TIME_COUNTER_VALUE();
		rtos_mem_free(task_status);
		task_status = NULL;
		break;
	default:
		error_no = 3;
		break;
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
		if (error_no == 1 || error_no == 3) {
			at_cpuload_help();
		}
	}
}
#endif
#endif /* CONFIG_INIC_NO_FLASH */

/****************************************************************
AT command process:
	AT+RREG
	[+RREG]: OK
	Read register value.
****************************************************************/
void at_rreg(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[RREG] Error parameters\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if ((argc < 2) || (argc > 4)) {
		RTK_LOGW(NOTAG, "[RREG] Error parameters\r\n");
		error_no = 1;
		goto end;
	}

	cmd_dump_word((u16)(argc - 1), (u8 **)&argv[1]);

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

/****************************************************************
AT command process:
	AT+WREG
	[+WREG]: OK
	Write register value.
****************************************************************/
void at_wreg(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[WREG] Error parameters\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 3) {
		RTK_LOGW(NOTAG, "[WREG] Error parameters\r\n");
		error_no = 1;
		goto end;
	}

	cmd_write_word((u16)(argc - 1), (u8 **)&argv[1]);

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

/****************************************************************
AT command process:
	AT+RST
	[+RST]: OK
	Then the board should re-start right now.
****************************************************************/
void at_rst(void *arg)
{
	UNUSED(arg);
	at_printf(ATCMD_OK_END_STR);
	sys_reset();
}

static void at_log_help(void)
{
	RTK_LOGI(TAG, "\r\n");
	RTK_LOGI(TAG, "AT+LOG=<get_set>,<module>[,<log_level>]\r\n");
	RTK_LOGI(TAG, "\t<get_set>:\t0-get, 1-set, 2-print all, 3-clear all\r\n");
	RTK_LOGI(TAG, "\t<module>:\t*-each module, others-specific module\r\n");
	RTK_LOGI(TAG, "\t<log_level>:\t[0,5], only applicable for set mode\r\n");
}

/****************************************************************
AT command process:
	AT+LOG
	[+LOG]: OK
	Set or get the log level of specific module / all modules.
****************************************************************/
void at_log(void *arg)
{
	int argc = 0, ret = 0, error_no = 0;
	enum atcmd_log_type_e mode = atcmd_log_type_invalid;
	char *argv[MAX_ARGC] = {0};
	rtk_log_level_t log_level;

	if (arg == NULL) {
		RTK_LOGA(NOTAG, "[LOG] ERROR arg: \r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if ((argc > 1) && (argv[1] != NULL)) {
		mode = (enum atcmd_log_type_e)atoi(argv[1]);
	}

	switch (mode) {
	/* Get. */
	case atcmd_log_type_get:
		if ((argc != 3) || (strlen(argv[2]) == 0)) {
			RTK_LOGA(NOTAG, "[LOG] Invalid get parameters.\r\n");
			error_no = 1;
			goto end;
		}
		log_level = rtk_log_level_get(argv[2]);
		at_printf("<%s> level is %d\r\n", argv[2], log_level);
		break;

	/* Set. */
	case atcmd_log_type_set:
		if ((argc != 4) || (strlen(argv[2]) == 0) || (strlen(argv[3]) == 0)) {
			RTK_LOGA(NOTAG, "[LOG] Invalid set parameters.\r\n");
			error_no = 1;
			goto end;
		}
		log_level = (rtk_log_level_t)atoi(argv[3]);
		ret = rtk_log_level_set(argv[2], log_level);
		if (ret != RTK_SUCCESS) {
			RTK_LOGA(NOTAG, "[LOG] Failed when set.\r\n");
			error_no = 2;
			goto end;
		}
		break;

	/* Print all. */
	case atcmd_log_type_print_all:
		if (argc != 2) {
			RTK_LOGA(NOTAG, "[LOG] Invalid parameter number.\r\n");
			error_no = 1;
			goto end;
		}
		rtk_log_array_print(rtk_log_tag_array);
		break;

	/* Clean all. */
	case atcmd_log_type_clear_all:
		if (argc != 2) {
			RTK_LOGA(NOTAG, "[LOG] Invalid parameter number.\r\n");
			error_no = 1;
			goto end;
		}
		rtk_log_array_clear();
		break;

	/* Invalid. */
	default:
		RTK_LOGA(NOTAG, "[LOG] Invalid mode %d.\r\n", mode);
		error_no = 1;
		goto end;
	}

	RTK_LOGA(NOTAG, "[LOG] Test always level\r\n");
	RTK_LOGE(NOTAG, "[LOG] Test error level\r\n");
	RTK_LOGW(NOTAG, "[LOG] Test warning level\r\n");
	RTK_LOGI(NOTAG, "[LOG] Test info level\r\n");
	RTK_LOGD(NOTAG, "[LOG] Test debug level\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[LOG] Test LOG_ITEMS\r\n");

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
		at_log_help();
	}
}

/****************************************************************
AT command process:
	AT+TICKPS
	R: release os wakelock
	A: acquire os wakelock
	TYPE: GC OR PG
****************************************************************/
void at_tickps(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[TICKPS] Error parameters\r\n");
		return;
	}
	argc = parse_param(arg, argv);
	if (argc < 2) {
		RTK_LOGW(NOTAG, "[TICKPS] Error parameters\r\n");
		return;
	}

	if (_strcmp((const char *)argv[1], "R") == 0) {
		pmu_release_wakelock(PMU_OS);
	}

	if (_strcmp((const char *)argv[1], "A") == 0) {
		pmu_acquire_wakelock(PMU_OS);
	}

	if (_strcmp((const char *)argv[1], "DSLP") == 0) {
		if (argc >= 3 && argc <= 4) {
			/*unit: ms*/
			u32 min_time = PMU_SLEEP_FOREVER, max_time = PMU_SLEEP_FOREVER;
			/*unit: ms*/
			if (argc >= 4) {
				min_time =  _strtoul((const char *)(argv[2]), (char **)NULL, 10);
				max_time =  _strtoul((const char *)(argv[3]), (char **)NULL, 10);
			} else {
				max_time =  _strtoul((const char *)(argv[2]), (char **)NULL, 10);
			}
			pmu_set_sleep_time_range(min_time, max_time);
		}

		pmu_release_wakelock(PMU_OS);
		pmu_release_deepwakelock(PMU_OS);
	}

	if (_strcmp((const char *)argv[1], "TYPE") == 0) {
		if (argc < 3) {
			RTK_LOGW(NOTAG, "[TICKPS] Error parameters\r\n");
			return;
		}

		if (_strcmp((const char *)argv[2], "PG") == 0) {
			pmu_set_sleep_type(SLEEP_PG);
		} else if (_strcmp((const char *)argv[2], "CG") == 0) {
			pmu_set_sleep_type(SLEEP_CG);
		} else {
			pmu_set_sleep_type(SLEEP_PG);
		}
	}

	if (_strcmp((char *)argv[1], "TIMER") == 0) {
		if (argc >= 3 && argc <= 4) {
			/*unit: ms*/
			u32 min_time = PMU_SLEEP_FOREVER, max_time = PMU_SLEEP_FOREVER;
			/*unit: ms*/
			if (argc >= 4) {
				min_time =  _strtoul((const char *)(argv[2]), (char **)NULL, 10);
				max_time =  _strtoul((const char *)(argv[3]), (char **)NULL, 10);
			} else {
				max_time =  _strtoul((const char *)(argv[2]), (char **)NULL, 10);
			}
			pmu_set_sleep_time_range(min_time, max_time);
		}

		pmu_release_wakelock(PMU_OS);
	}

	if (_strcmp((const char *)argv[1], "GET") == 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "lockbit:%x \r\n", pmu_get_wakelock_status());
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "dslp_lockbit:%x \r\n", pmu_get_deepwakelock_status());
	}
}

/****************************************************************
AT command process:
	AT+STATE
	[+STATE]: OK
	Show the task list, and heap state.
****************************************************************/
extern u32 total_heap_size;
void at_state(void *arg)
{
	UNUSED(arg);
#if defined(configUSE_TRACE_FACILITY) && (configUSE_TRACE_FACILITY == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS == 1) && (configGENERATE_RUN_TIME_STATS == 1)
	{
		char *pcWriteBuffer;
		int task_n = uxTaskGetNumberOfTasks();
		size_t buf_size = task_n * (configMAX_TASK_NAME_LEN + 16);
		pcWriteBuffer = (char *)rtos_mem_malloc(buf_size);
		_memset(pcWriteBuffer, 0, buf_size);
		vTaskList((char *)pcWriteBuffer);
		at_printf("Task List: \r\n%s\r\n", pcWriteBuffer);
		rtos_mem_free(pcWriteBuffer);
	}
#endif

	HeapStats_t pxHeapStats;
	vPortGetHeapStats(&pxHeapStats);
	at_printf("HeapStats: \r\n");
	at_printf("Total Heap:\t%u\r\n", total_heap_size);
	at_printf("Heap Free Now:\t%u\r\n", pxHeapStats.xAvailableHeapSpaceInBytes);
	at_printf("Heap Used Now:\t%u\r\n", total_heap_size - pxHeapStats.xAvailableHeapSpaceInBytes);
	at_printf("Heap Used Max:\t%u\r\n", total_heap_size - pxHeapStats.xMinimumEverFreeBytesRemaining);

	at_printf(ATCMD_OK_END_STR);
}

ATCMD_TABLE_DATA_SECTION
const log_item_t at_sys_common_items[] = {
	{"+RREG", at_rreg, {NULL, NULL}},
	{"+WREG", at_wreg, {NULL, NULL}},
	{"+RST", at_rst, {NULL, NULL}},
	{"+LOG", at_log, {NULL, NULL}},
	{"+TICKPS", at_tickps, {NULL, NULL}},
	{"+STATE", at_state, {NULL, NULL}},
#ifndef CONFIG_INIC_NO_FLASH
#if (configGENERATE_RUN_TIME_STATS == 1)
	{"+CPULOAD", at_cpuload, {NULL, NULL}},
#endif
#endif
};

void print_system_common_at(void)
{
	int index;
	int cmd_len = 0;

	cmd_len = sizeof(at_sys_common_items) / sizeof(at_sys_common_items[0]);
	for (index = 0; index < cmd_len; index++) {
		at_printf("AT%s\r\n", at_sys_common_items[index].log_cmd);
	}
}


void at_sys_init_common(void)
{
	atcmd_service_add_table((log_item_t *)at_sys_common_items, sizeof(at_sys_common_items) / sizeof(at_sys_common_items[0]));
}
