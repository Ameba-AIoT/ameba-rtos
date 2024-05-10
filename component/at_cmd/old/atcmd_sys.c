/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <platform_autoconf.h>
#include "os_wrapper.h"

#include <gpio_api.h>   // mbed
#include <sys_api.h>
#include <flash_api.h>

#if !defined(CONFIG_AMEBASMART) && !defined(CONFIG_AMEBALITE) && !defined(CONFIG_AMEBADPLUS) && !defined(CONFIG_AMEBAGREEN2)
#include <rtl_lib.h>
#endif
#include <build_info.h>
#include "log_service.h"
#include "atcmd_sys.h"
#include "main.h"
#include "atcmd_wifi.h"

#include "FreeRTOS.h"
#include "task.h"

#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
#include "ameba_pmu.h"
#endif


//-------- AT SYS commands ---------------------------------------------------------------
void fATSC(void *arg)
{
	/* To avoid gcc warnings */
	(void) arg;

	AT_DBG_MSG(AT_FLAG_OTA, AT_DBG_ALWAYS, "[ATSC]: _AT_SYSTEM_CLEAR_OTA_SIGNATURE_");
	sys_clear_ota_signature();
}

void fATSR(void *arg)
{
	/* To avoid gcc warnings */
	(void) arg;

	AT_DBG_MSG(AT_FLAG_OTA, AT_DBG_ALWAYS, "[ATSR]: _AT_SYSTEM_RECOVER_OTA_SIGNATURE_");
	sys_recover_ota_signature();
}

#if (configGENERATE_RUN_TIME_STATS == 1)
void fATSS(void *arg)	// Show CPU stats
{
	/* To avoid gcc warnings */
	(void) arg;

	int buf_size = 512;
	char *cBuffer = (char *)malloc(buf_size);

	if (NULL == cBuffer) {
		RTK_LOGI(NOTAG, "%s malloc failed\r\n", __FUNCTION__);
		return;
	}
	memset(cBuffer, 0, buf_size);

	AT_PRINTK("[ATSS]: _AT_SYSTEM_CPU_STATS_");
	vTaskGetRunTimeStats((char *)cBuffer);
	AT_PRINTK("%s", cBuffer);

	free(cBuffer);
}

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
	//int delta_cnt;
} task_status_t;

typedef struct status_cmd_para_s {
	int time;
	int count;
} status_cmd_para_t;

static task_status_t *task_status = NULL;

TaskStatus_t *find_status(UBaseType_t id, TaskStatus_t *status, int cnt)
{
	for (int i = 0; i < cnt; i++) {
		if (id == status[i].xTaskNumber) {
			return &status[i];
		}
	}

	return NULL;
}

delta_status_t *find_delta(int id, delta_status_t *delta, int cnt)
{
	for (int i = 0; i < cnt; i++) {
		if (id == delta[i].task_id) {
			return &delta[i];
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

static void update_status(void)
{
	delta_status_t *deltaone = NULL;
	// init
	if (task_status->last == NULL && task_status->curr == NULL) {
		task_status->last = task_status->status[0];
		task_status->curr = task_status->status[1];
	}

	// update last
	task_status->last_cnt = task_status->curr_cnt;
	TaskStatus_t *tmp = task_status->last;
	task_status->last = task_status->curr;
	task_status->curr = tmp;

	// update curr
	task_status->curr_cnt = uxTaskGetNumberOfTasks();
	task_status->curr_cnt = uxTaskGetSystemState(task_status->curr, task_status->curr_cnt, NULL);
	memset(task_status->delta, 0, TASK_CNT * sizeof(delta_status_t));
	// update delta
	if (task_status->last_cnt > 0) {
		for (int i = 0; i < task_status->curr_cnt; i++) {

			// find in last
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
}

void print_delta(int delta_tick)
{
	char c_state[5] = {'R', 'r', 'b', 's', 'D'};
	char CPU_decimal_string[3] = {0};
	int CPU_1000, CPU_integer, CPU_decimal;

	RTK_LOGI(NOTAG, "%-32s\t%6s\t%6s\t%s\n\r", "task name", "state", "prio", "CPU%");
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
			RTK_LOGI(NOTAG, "%-32s\t%6c\t%6d\t%d.%s\r\n", task_status->delta[i].bak_name, c_state[task_status->delta[i].state], task_status->delta[i].priority,
					 CPU_integer, CPU_decimal_string);
		}
	}
}

static void *top_exit_sema;
static u32 last_tick;
status_cmd_para_t para_in;

void cpu_stat_thread(void *dummy)
{
	status_cmd_para_t *ppara = malloc(sizeof(status_cmd_para_t));
	if (NULL == ppara) {
		RTK_LOGI(NOTAG, "%s malloc failed\r\n", __FUNCTION__);
		goto exit;
	}
	memcpy(ppara, dummy, sizeof(status_cmd_para_t));
	last_tick = portGET_RUN_TIME_COUNTER_VALUE();
	while ((rtos_sema_take((rtos_sema_t)top_exit_sema, ppara->time * 1000) == FAIL)) {
		update_status();
		int delta_tick =  portGET_RUN_TIME_COUNTER_VALUE() - last_tick;
		last_tick = portGET_RUN_TIME_COUNTER_VALUE();
		print_delta(delta_tick);

		if (0 < ppara->count) {
			ppara->count--;
		} else if (0 == ppara->count) {
			break;
		}
	}
	free(ppara);

exit:
	rtos_sema_delete((rtos_sema_t)top_exit_sema);
	top_exit_sema = NULL;
	free(task_status);
	task_status = NULL;
	rtos_task_delete(NULL);
}

void fATSP(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	int top_mode = 0;

	para_in.time = 1;
	para_in.count = -1;
	argc = parse_param(arg, argv);

	if (argc > 4 || argc < 2)	{
		RTK_LOGS(NOTAG,
				 "[top]Usage: top=mode,time,count\n\r mode: 0, start count cpu usage every [time] second.\r\n mode: 1, stop mode 0.\r\n mode: 2: start count cpu usage.\r\n mode: 3: stop mode 2.\r\n "
				 "time: CPU statistics interval.Default 1. unit(s) \r\n count: CPU statistics count, default until stop or 1,2,3...");
		return;
	}
	if (argc == 3) {
		top_mode = atoi(argv[1]);
		para_in.time = (atoi(argv[2])) > 0 ? (atoi(argv[2])) : 1;
	} else if (argc == 2) {
		top_mode = atoi(argv[1]);
	} else if (argc == 4) {
		top_mode = atoi(argv[1]);
		para_in.time = (atoi(argv[2])) > 0 ? (atoi(argv[2])) : 1;
		para_in.count = (atoi(argv[3])) > 0 ? (atoi(argv[3])) - 1 : -1;
	}

	if (NULL == task_status) {
		task_status = (task_status_t *)malloc(sizeof(task_status_t));
		if (NULL == task_status) {
			RTK_LOGI(NOTAG, "%s malloc failed\r\n", __FUNCTION__);
			return;
		}
	}

	RTK_LOGS(NOTAG, "current mode: %d time: %d(s) count: %d (-1 for infinite)\r\n", top_mode, para_in.time, para_in.count);
	switch (top_mode) {
	case 0:
		if (top_exit_sema)	{
			break;
		}
		memset(task_status, 0, sizeof(task_status_t));
		update_status();
		rtos_sema_create(&top_exit_sema, 0, 1);
		rtos_task_create(NULL, ((const char *)"cpu_stat_thread"), cpu_stat_thread, &para_in, 4096 * 4, configMAX_PRIORITIES - 1);
		break;
	case 1:
		if (top_exit_sema) {
			rtos_sema_give((rtos_sema_t) top_exit_sema);
		}
		break;
	case 2:
		if (top_exit_sema)	{
			break;
		}
		memset(task_status, 0, sizeof(task_status_t));
		last_tick = portGET_RUN_TIME_COUNTER_VALUE();
		update_status();
		break;
	case 3:
		if (top_exit_sema)	{
			break;
		}
		update_status();
		print_delta(portGET_RUN_TIME_COUNTER_VALUE() - last_tick);
		last_tick = portGET_RUN_TIME_COUNTER_VALUE();
		free(task_status);
		task_status = NULL;
		break;
	}
}

#endif

#if defined(WIFI_LOGO_CERTIFICATION_CONFIG) && WIFI_LOGO_CERTIFICATION_CONFIG

#define FLASH_ADDR_SW_VERSION 	FAST_RECONNECT_DATA+0x900
#define SW_VERSION_LENGTH 	32

void fATSV(void *arg)
{
	unsigned char sw_version[SW_VERSION_LENGTH + 1];
	flash_t flash;

	if (!arg) {
		RTK_LOGI(NOTAG, "[ATSV]Usage: ATSV=[SW_VERSION]\n\r");
		return;
	}

	if (strlen((char *)arg) > SW_VERSION_LENGTH) {
		RTK_LOGI(NOTAG, "[ATSV] ERROR : SW_VERSION length can't exceed %d\n\r", SW_VERSION_LENGTH);
		return;
	}

	memset(sw_version, 0, SW_VERSION_LENGTH + 1);
	strncpy(sw_version, (char *)arg, strlen((char *)arg));

	flash_erase_sector(&flash, FAST_RECONNECT_DATA);
	flash_stream_write(&flash, FLASH_ADDR_SW_VERSION, strlen((char *)arg), (uint8_t *) sw_version);

	RTK_LOGI(NOTAG, "[ATSV] Write SW_VERSION to flash : %s\n\r", sw_version);

}
#endif

void fATSx(void *arg)
{
	/* To avoid gcc warnings */
	(void) arg;

	AT_PRINTK("[ATS?]: _AT_SYSTEM_HELP_");

	ChipInfo_GetSocName();
	ChipInfo_GetLibVersion();
}

log_item_t at_sys_items[] = {
#ifndef CONFIG_INIC_NO_FLASH
	{"ATSC", fATSC, {NULL, NULL}},	// Clear OTA signature
	{"ATSR", fATSR, {NULL, NULL}},	// Recover OTA signature
#if (configGENERATE_RUN_TIME_STATS == 1)
	{"ATSP", fATSP, {NULL, NULL}},
//	{"ATSS", fATSS, {NULL, NULL}},	// Show CPU stats==》改成开始和停止输出信息，以及秒级间隔。
#endif
	{"ATS?", fATSx, {NULL, NULL}},	// Help
#if defined(WIFI_LOGO_CERTIFICATION_CONFIG) && WIFI_LOGO_CERTIFICATION_CONFIG
	{"ATSV", fATSV},				// Write SW version for wifi logo test
#endif

#endif
};

void at_sys_init(void)
{
	log_service_add_table(at_sys_items, sizeof(at_sys_items) / sizeof(at_sys_items[0]));
}

#ifdef SUPPORT_LOG_SERVICE
log_module_init(at_sys_init);
#endif
