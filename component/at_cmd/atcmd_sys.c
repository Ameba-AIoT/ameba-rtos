/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <platform_autoconf.h>

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
#ifdef CONFIG_NEW_ATCMD
#include "atcmd_mqtt.h"
#if defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED
#include "atcmd_bt_mp.h"
#endif
#ifdef CONFIG_LWIP_LAYER
#include "atcmd_lwip.h"
#endif
#endif

//#include "FreeRTOS.h"
//#include "task.h"

#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
#include "ameba_pmu.h"
#endif

extern u32 cmd_dump_word(u16 argc, u8  *argv[]);
extern u32 cmd_write_word(u16 argc, u8  *argv[]);

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

static void update_status(void)
{
	delta_status_t *deltaone = NULL;
	/* init */
	if (task_status->last == NULL && task_status->curr == NULL) {
		task_status->last = task_status->status[0];
		task_status->curr = task_status->status[1];
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
}

void print_delta(int delta_tick)
{
	char c_state[5] = {'R', 'r', 'b', 's', 'D'};
	char CPU_decimal_string[3] = {0};
	int CPU_1000, CPU_integer, CPU_decimal;

	at_printf("%-32s\t%6s\t%6s\t%s\n\r", "task name", "state", "prio", "CPU%");
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
			at_printf("%-32s\t%6c\t%6d\t%d.%s\r\n", task_status->delta[i].bak_name, c_state[task_status->delta[i].state], task_status->delta[i].priority,
					  CPU_integer, CPU_decimal_string);
		}
	}
}

void cpu_stat_thread(void *dummy)
{
	status_cmd_para_t *ppara = rtos_mem_malloc(sizeof(status_cmd_para_t));
	if (NULL == ppara) {
		RTK_LOGS(NOTAG, "%s malloc failed\r\n", __FUNCTION__);
		goto end;
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
	rtos_mem_free(ppara);

end:
	rtos_sema_delete((rtos_sema_t)top_exit_sema);
	top_exit_sema = NULL;
	rtos_mem_free(task_status);
	task_status = NULL;
	rtos_task_delete(NULL);
}
#endif /* configGENERATE_RUN_TIME_STATS */

/****************************************************************
AT command process:
	AT+OTACLEAR
	Clear OTA signature.
	[+OTACLEAR]: OK
****************************************************************/
void at_otaclear(void *arg)
{
	UNUSED(arg);
	at_printf("\r\n%sOK\r\n", "+OTACLEAR:");
	sys_clear_ota_signature();
}

/****************************************************************
AT command process:
	AT+OTARECOVER
	Recover OTA signature.
	[+OTARECOVER]: OK
****************************************************************/
void at_otarecover(void *arg)
{
	UNUSED(arg);
	at_printf("\r\n%sOK\r\n", "+OTARECOVER:");
	sys_recover_ota_signature();
}

#if (configGENERATE_RUN_TIME_STATS == 1)
static void at_cpuload_help(void)
{
	at_printf("\r\n");
	at_printf("AT+CPULOAD=<mode>[,<time_intval>,<counter>]\r\n");
	at_printf("\t<mode>:\t[0,2]\r\n");
	at_printf("\t<time_intval>:\tIn sec\r\n");
	at_printf("\t<counter>\t0 means infinit\r\n");
}

/****************************************************************
AT command process:
	AT+CPULOAD
	[+CPULOAD]: OK
****************************************************************/
void at_cpuload(void *arg)
{
	int argc = 0;
	int error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int top_mode = 0;

	para_in.time = 1;
	para_in.count = -1;

	argc = parse_param(arg, argv);
	if (argc > 4 || argc < 2) {
		RTK_LOGS(NOTAG,
				 "[top]Usage: top=mode,time,count\n\r mode: 0, start count cpu usage every [time] second.\r\n mode: 1, stop mode 0.\r\n mode: 2: start count cpu usage.\r\n mode: 3: stop mode 2.\r\n "
				 "time: CPU statistics interval.Default 1. unit(s) \r\n count: CPU statistics count, default until stop or 1,2,3...");
		error_no = 1;
		goto end;
	}
	if (argc == 3) {
		top_mode = atoi(argv[1]);
		para_in.time = (atoi(argv[2])) > 0 ? (atoi(argv[2])) : 1;
	} else if (argc == 2) {
		top_mode = atoi(argv[1]);
	} else {/* argc == 4 */
		top_mode = atoi(argv[1]);
		para_in.time = (atoi(argv[2])) > 0 ? (atoi(argv[2])) : 1;
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
		rtos_mem_free(task_status);
		task_status = NULL;
		break;
	default:
		error_no = 3;
		break;
	}

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+CPULOAD:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+CPULOAD:", error_no);
		if (error_no == 1 || error_no == 3) {
			at_cpuload_help();
		}
	}
}
#endif
#endif /* CONFIG_INIC_NO_FLASH */

/****************************************************************
AT command process:
	AT+TEST
	[+TEST]: OK
****************************************************************/
void at_test(void *arg)
{
	UNUSED(arg);
	at_printf("\r\n%sOK\r\n", "+TEST:");
}

/****************************************************************
AT command process:
	AT+LIST
	Common AT Command:
	Then pending all the AT common commands supported.
	Wi-Fi AT Command:
	Then pending all the Wi-Fi common commands supported.
	......
	[+LIST]:OK
****************************************************************/
void at_list(void *arg)
{
	UNUSED(arg);

	/* System commands. */
	at_printf("Common AT Command:\r\n");
	print_system_at();

#ifndef CONFIG_MP_INCLUDED
	/* Wifi commands. */
	at_printf("Wi-Fi AT Command:\r\n");
	print_wifi_at();
#endif

#ifdef CONFIG_LWIP_LAYER
	/* TCP/IP commands. */
	at_printf("TCP/IP AT Command:\r\n");
	print_lwip_at();
#endif

#if defined(CONFIG_BT) && CONFIG_BT
	/* Bluetooth commands. */
	at_printf("BT AT command:\r\n");
	print_bt_ext_at();
#if defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED
	print_bt_mp_at();
#endif
#endif

#ifndef CONFIG_MP_INCLUDED
	/* MQTT commands. */
	at_printf("MQTT AT command:\r\n");
	print_mqtt_at();
#endif

	at_printf("\r\n%sOK\r\n", "+LIST:");
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
	at_printf("\r\n%sOK\r\n", "+RST:");
	sys_reset();
}

/****************************************************************
AT command process:
	AT+RST
	[+RST]: OK
	Then the board should re-start right now.
****************************************************************/
extern u32 total_heap_size;
void at_state(void *arg)
{
	UNUSED(arg);
#if defined(configUSE_TRACE_FACILITY) && (configUSE_TRACE_FACILITY == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS == 1)
	{
		signed char pcWriteBuffer[1024];
		vTaskList((char *)pcWriteBuffer);
		at_printf("Task List: \n\r%s\n\r", pcWriteBuffer);
	}
#endif

	HeapStats_t pxHeapStats;
	vPortGetHeapStats(&pxHeapStats);
	at_printf("HeapStats: \n\r");
	at_printf("Total Heap:\t%u\n\r", total_heap_size);
	at_printf("Heap Free Now:\t%u\n\r", pxHeapStats.xAvailableHeapSpaceInBytes);
	at_printf("Heap Used Now:\t%u\n\r", total_heap_size - pxHeapStats.xAvailableHeapSpaceInBytes);
	at_printf("Heap Used Max:\t%u\n\r", total_heap_size - pxHeapStats.xMinimumEverFreeBytesRemaining);

	at_printf("\r\n%sOK\r\n", "+STATE:");
}


/****************************************************************
AT command process:
	AT+GMR
	+GMR:<at-version>,<sdk-version>(<compile_time>)
	[+GMR]: OK
****************************************************************/
void at_gmr(void *arg)
{
	char at_buf[32];
	char fw_buf[32];

	UNUSED(arg);

	ChipInfo_GetSocName();
	ChipInfo_GetLibVersion();

	strncpy(at_buf, ATCMD_VERSION"."ATCMD_SUBVERSION"."ATCMD_REVISION, sizeof(at_buf));
	strncpy(fw_buf, SDK_VERSION, sizeof(fw_buf));
	at_printf("\r\n%s%s,%s,%s\r\n", "+GMR:", at_buf, fw_buf, RTL_FW_COMPILE_TIME);
}

static void at_log_help(void)
{
	at_printf("\r\n");
	at_printf("AT+LOG=<get_set>,<module>[,<log_level>]\r\n");
	at_printf("\t<get_set>:\t0-get, 1-set, 2-print all, 3-clear all\r\n");
	at_printf("\t<module>:\t*-each module, others-specific module\r\n");
	at_printf("\t<log_level>:\t[0,5], only applicable for set mode\r\n");
}

/****************************************************************
AT command process:
	AT+LOG
	[+LOG]: OK
	Set or get the log level of specific module / all modules.
****************************************************************/
void at_log(void *arg)
{
	int argc = 0, temp = 0, mode = -1, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	rtk_log_level_t log_level;

	if (arg == NULL) {
		RTK_LOGA(NOTAG, "[LOG] ERROR arg: \r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if ((argc > 1) && (argv[1] != NULL)) {
		mode = atoi(argv[1]);
	}

	/* Get. */
	if (mode == 0) {
		if ((argc != 3) || (argv[2] == NULL)) {
			RTK_LOGA(NOTAG, "[LOG] Invalid get parameters.\r\n");
			error_no = 1;
			goto end;
		}
		log_level = rtk_log_level_get(argv[2]);
		at_printf("<%s> level is %d\r\n", argv[2], log_level);
	}

	/* Set. */
	else if (mode == 1) {
		if ((argc != 4) || (argv[2] == NULL) || (argv[3] == NULL)) {
			RTK_LOGA(NOTAG, "[LOG] Invalid set parameters.\r\n");
			error_no = 1;
			goto end;
		}
		temp = atoi(argv[3]);
		if ((temp < RTK_LOG_NONE) || (temp > RTK_LOG_DEBUG)) {
			RTK_LOGA(NOTAG, "[LOG] Invalid log level %d.\r\n", temp);
			error_no = 1;
			goto end;
		}
		log_level = (rtk_log_level_t)temp;
		rtk_log_level_set(argv[2], log_level);
	}

	/* Print all. */
	else if (mode == 2) {
		if (argc != 2) {
			RTK_LOGA(NOTAG, "[LOG] Invalid parameter number.\r\n");
			error_no = 1;
			goto end;
		}
		rtk_log_array_print(rtk_log_tag_array);
	}

	/* Clean all. */
	else if (mode == 3) {
		if (argc != 2) {
			RTK_LOGA(NOTAG, "[LOG] Invalid parameter number.\r\n");
			error_no = 1;
			goto end;
		}
		rtk_log_array_clear();
	}

	/* Invalid. */
	else {
		RTK_LOGA(NOTAG, "[LOG] Invalid get_set %d.\r\n", mode);
		error_no = 1;
		goto end;
	}

	RTK_LOGA(NOTAG, "[LOG] Test always level\r\n");
	RTK_LOGE(NOTAG, "[LOG] Test error level\r\n");
	RTK_LOGW(NOTAG, "[LOG] Test warning level\r\n");
	RTK_LOGI(NOTAG, "[LOG] Test info level\r\n");
	RTK_LOGD(NOTAG, "[LOG] Test debug level\r\n");
	RTK_LOGS(NOTAG, "[LOG] Test LOG_ITEMS\r\n");

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+LOG:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+LOG:", error_no);
		at_log_help();
	}
}

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
		at_printf("\r\n%sOK\r\n", "+RREG:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+RREG:", error_no);
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

	cmd_write_word((u16)(argc - 1), (u8 **)argv[1]);

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+WREG:");
	} else {
		at_printf("\r\n%sERROR: %d\r\n", "+WREG:", error_no);
	}
}

log_item_t at_sys_items[] = {
#ifndef CONFIG_INIC_NO_FLASH
	{"+OTACLEAR", at_otaclear, {NULL, NULL}},
	{"+OTARECOVER", at_otarecover, {NULL, NULL}},
#if (configGENERATE_RUN_TIME_STATS == 1)
	{"+CPULOAD", at_cpuload, {NULL, NULL}},
#endif
#endif
	{"+TEST", at_test, {NULL, NULL}},
	{"+LIST", at_list, {NULL, NULL}},
	{"+RST", at_rst, {NULL, NULL}},
	{"+STATE", at_state, {NULL, NULL}},
	{"+GMR", at_gmr, {NULL, NULL}},
	{"+LOG", at_log, {NULL, NULL}},
	{"+RREG", at_rreg, {NULL, NULL}},
	{"+WREG", at_wreg, {NULL, NULL}},
};

void print_system_at(void)
{
	int index;
	int cmd_len = 0;

	cmd_len = sizeof(at_sys_items) / sizeof(at_sys_items[0]);
	for (index = 0; index < cmd_len; index++) {
		at_printf("AT%s\r\n", at_sys_items[index].log_cmd);
	}
}

void print_system_help(void)
{
	at_printf("AT+OTACLEAR=\r\n");
	at_printf("AT+OTARECOVER\r\n");
	at_printf("AT+CPULOAD=<mode>,<time_inteval>,<count>\r\n");
	at_printf("AT+TEST\r\n");
	at_printf("AT+LIST\r\n");
	at_printf("AT+GMR\r\n");
	at_printf("AT+LOG=<set_or_get>[,<module_name>,<log_level>]\r\n");
	at_printf("AT+RREG=<flash_address>[,<length>,<in_byte>]\r\n");
	at_printf("AT+WREG=<flash_address>,<value>\r\n");
	at_printf("\r\n");
}

void at_sys_init(void)
{
	log_service_add_table(at_sys_items, sizeof(at_sys_items) / sizeof(at_sys_items[0]));
}

#ifdef SUPPORT_LOG_SERVICE
log_module_init(at_sys_init);
#endif
