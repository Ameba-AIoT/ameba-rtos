
#include "ameba_soc.h"
#include "ameba_diag.h"
#include "ameba_diag_customer.h"
#include "os_wrapper.h"
#ifdef CONFIG_CORE_AS_NP



int test_rtk_diag_init(void)
{
	rtk_diag_init(RTK_DIAG_HEAP_SIZE);

	return 0;
}
int test_rtk_diag_exit(void)
{
	rtk_diag_deinit();

	return 0;
}
/*1. basic function test */
int test_rtk_diag_basic_add(void)
{
	u8 debugInfo1[] = {0x01, 0x02, 0x03};     // Small event
	u8 debugInfo2[] = {0x05, 0x06};           // Small event
	u8 debugInfo3[205] = {0};                 // Large event

	for (int i = 0; i < 205; i++) {
		debugInfo3[i] = i;
	}

	rtk_diag_new_event_add(0x01, sizeof(debugInfo1), RTK_EVENT_LEVEL_INFO, debugInfo1);
	rtk_diag_new_event_add(0x02, sizeof(debugInfo2), RTK_EVENT_LEVEL_WARNING, debugInfo2);
	rtk_diag_new_event_add(0x03, sizeof(debugInfo3), RTK_EVENT_LEVEL_DEBUG, debugInfo3);
	DiagPrintf("RTK System Heap Usage: %lu\n", rtk_sys_heap_usage_get());
	DiagPrintf("RTK Diag Heap Usage: %lu\n", rtk_diag_heap_usage_get());
	rtk_diag_all_events_read();
	return 0;
}

static void aontimer_dslp_handler(void)
{
	DiagPrintf("dslp wake from aontimer\n");

	SOCPS_AONTimerClearINT();
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, DISABLE);
	u8 debugInfo4[] = {0x07, 0x08, 0x09, 0x10};
	rtk_diag_new_event_add(0x07, sizeof(debugInfo4), RTK_EVENT_LEVEL_INFO, debugInfo4);
	DiagPrintf("dslp wake from aontimer over\n");
}
void test_rtk_diag_aontimer_isr(void)
{
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);
	SOCPS_AONTimer(100);
	InterruptRegister((IRQ_FUN)aontimer_dslp_handler, AON_TIM_IRQ, NULL, 3);
	InterruptEn(AON_TIM_IRQ, 3);
	SOCPS_AONTimerINT_EN(ENABLE);

}
// test case: Multi-task test case
void test_rtk_diag_multi_task1(void *pv_parameters)
{
	(void)pv_parameters;
	u8 debugInfo4[] = {0x07, 0x08, 0x09, 0x10};
	rtk_diag_new_event_add(0x07, sizeof(debugInfo4), RTK_EVENT_LEVEL_INFO, debugInfo4);

	rtos_task_delete(NULL);
}
#define TEST_LITTLE_EVT_NUM 17
#define TEST_LITTLE_EVT_LEN 100
void test_rtk_diag_multi_task2(void *pv_parameters)
{
	(void)pv_parameters;
	u8 debugInfo5[TEST_LITTLE_EVT_LEN];
	uint8_t k = 0;
	for (int i = 0; i < TEST_LITTLE_EVT_NUM; i++) {
		_memset(debugInfo5, 0, TEST_LITTLE_EVT_LEN);
		for (int j = 0; j < TEST_LITTLE_EVT_LEN; j++) {
			debugInfo5[j] = k++;
		}
		rtk_diag_new_event_add(i % 21, TEST_LITTLE_EVT_LEN, i % 4, debugInfo5);
	}
	DiagPrintf("test_rtk_diag_multi_task2 add ok\n");
	while (1) {
		rtos_time_delay_ms(100);
	}
}
u8 debugInfo6[2000] = {0};
void test_rtk_diag_multi_task3(void *pv_parameters)
{
	(void)pv_parameters;
	uint8_t test_cmd = 0xA;
	for (int i = 0; i < 2; i++) {
		for (int i = 0; i < 2000; i++) {
			debugInfo6[i] = test_cmd;
		}
		rtk_diag_new_event_add(0x0A, sizeof(debugInfo6), RTK_EVENT_LEVEL_WARNING, debugInfo6);
		test_cmd++;
	}

	rtos_task_delete(NULL);
}

void test_rtk_diag_multi_task4(void *pv_parameters)
{
	(void)pv_parameters;
	u8 debugInfo9[] = {0x01, 0x02, 0x03, 0x4};     // Small event
	rtk_diag_new_event_add(0x07, sizeof(debugInfo9), RTK_EVENT_LEVEL_INFO, debugInfo9);
	while (1) {
		rtos_time_delay_ms(100);
	}
}
void test_rtk_diag_multi_task_add(void)
{
	if (rtos_task_create(NULL, "basic_func2", (rtos_task_t)test_rtk_diag_multi_task1, NULL, (3072), (1)) != RTK_SUCCESS) {
		DiagPrintf("Cannot create power_ppe demo task\n\r");
	}
	if (rtos_task_create(NULL, "little_event_criticality_test", (rtos_task_t)test_rtk_diag_multi_task2, NULL, (3072), (1)) != RTK_SUCCESS) {
		DiagPrintf("Cannot create little_event_criticality_test demo task\n\r");
	}

	if (rtos_task_create(NULL, "big_event_criticality_test", (rtos_task_t)test_rtk_diag_multi_task3, NULL, (3072), (1)) != RTK_SUCCESS) {
		DiagPrintf("Cannot create big_event_criticality_test demo task\n\r");
	}

	if (rtos_task_create(NULL, "critial_region_test", (rtos_task_t)test_rtk_diag_multi_task4, NULL, (3072), (1)) != RTK_SUCCESS) {
		DiagPrintf("Cannot create critial_region_test demo task\n\r");
	}

}

void test_rtk_diag_register_func(u8 *data, u16 len, u8 ck_sum)
{
	int i = 0;
	DiagPrintf("len = %d\n", len);
	while (i < len) {
		DiagPrintf("%02x ", data[i++]);
	}
	DiagPrintf("%02x\n", ck_sum);
}

void test_rtk_diag_add_selfdef(void)
{
	// 使用结构体初始化列表来对testConfig进行赋值
	struct config testConfig = {
		.ip_address = {192, 168, 1, 100},
		.port = 8080,
		.settings = {
			.retry_count = 5,
			.timeout = 10
		},
		.status = 2, // Connected
		.bufferSize = 512,
		.flags = {
			.error = 0,
			.ready = 1,
			.mode = 1, // Auto
			.unused = 0
		}
	};

	// 添加事件
	int result = rtk_diag_new_event_add(RTK_EVENT_TYPE_WIFI_DS, sizeof(testConfig), RTK_EVENT_LEVEL_INFO, (u8 *)&testConfig);

	// RTK_EVENT_I(RTK_EVENT_TYPE_WIFI_DS, sizeof(testConfig), (u8*)&testConfig);
	// 输出结果
	if (result == 0) {
		printf("Event added successfully.\n");
	} else {
		printf("Failed to add event, error code: %d\n", result);
	}
}


u32 diagnose_Test(u16 argc, u8 *argv[])
{
	if (argc < 1) {
		DiagPrintf("Insufficient number of parameters!\n");
		return 1;
	}
	if (_strcmp((char *)argv[0], "init") == 0) {
		/*1.init.*/
		test_rtk_diag_init();
	} else if (_strcmp((char *)argv[0], "exit") == 0) {
		/*deinit test.*/
		test_rtk_diag_exit();
	} else if (_strcmp((char *)argv[0], "basic") == 0) {
		/*basic add func test.*/
		test_rtk_diag_basic_add();
	} else if (_strcmp((char *)argv[0], "task") == 0) {
		/*multi-task add test.*/
		test_rtk_diag_multi_task_add();
	} else if (_strcmp((char *)argv[0], "isr") == 0) {
		/*add event in interrupt test.*/
		test_rtk_diag_aontimer_isr();
	} else if (_strcmp((char *)argv[0], "self") == 0) {
		/*2.add self-def test.*/
		test_rtk_diag_aontimer_isr();
	} else if (_strcmp((char *)argv[0], "read") == 0) {
		rtk_diag_all_events_read();
	} else if (_strcmp((char *)argv[0], "diagheap") == 0) {
		DiagPrintf("diag heap index = %d\n", rtk_diag_heap_usage_get());
	} else if (_strcmp((char *)argv[0], "sysheap") == 0) {
		DiagPrintf("system heap index = %d\n", rtk_sys_heap_usage_get());
	} else if (_strcmp((char *)argv[0], "tl_read") == 0) {
		u32 time = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
		u32 offset = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		rtk_diag_outfunc_register((rtk_diag_frame_send)test_rtk_diag_register_func);
		rtk_diag_frame_packet(time, offset);
	} else if (_strcmp((char *)argv[0], "tl_time") == 0) {
		DiagPrintf("time = %d\n", rtos_time_get_current_system_time_ms());
	} else if (_strcmp((char *)argv[0], "tl_size") == 0) {
		u32 size = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
		rtk_diag_frame_size_set(size);
	}
	return 0;
}


CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   diagnose_test_cmd_table[] = {
	{
		(const u8 *)"diagnose",  4, diagnose_Test, (const  u8 *)"\tDiagnose \n"
		"\t\tdiagnose read/clear/time\n"
	},
};

#else

void test_rtk_diag_send_via_ipc(void *pv_parameters)
{
	(void)pv_parameters;
	u8 debugInfo[] = {0x0A, 0x0B, 0x0C, 0xD};     // Small event
	int result = rtk_diag_new_event_add(0x07, sizeof(debugInfo), RTK_EVENT_LEVEL_INFO, debugInfo);
	DiagPrintf("%s:%d result = %d\n", __func__, __LINE__, result);
}

// test case: Multi-task test case
void task5(void *pv_parameters)
{
	(void)pv_parameters;
	u8 debugInfo4[] = {0x07, 0x08, 0x09, 0x10};
	rtk_diag_new_event_add(0x07, sizeof(debugInfo4), RTK_EVENT_LEVEL_INFO, debugInfo4);

	// while(1);
	// rtos_task_delete(NULL);
}

void task6(void *pv_parameters)
{
	(void)pv_parameters;
	u8 debugInfo9[] = {0x01, 0x02, 0x03, 0x4};     // Small event
	rtk_diag_new_event_add(0x07, sizeof(debugInfo9), RTK_EVENT_LEVEL_INFO, debugInfo9);
	while (1) {
		rtos_time_delay_ms(10);
	}

	rtos_task_delete(NULL);
}

void test_rtk_diag_multi_task_add_via_ipc(void)
{
	if (rtos_task_create(NULL, "basic_func2", (rtos_task_t)task5, NULL, (3072), (1)) != RTK_SUCCESS) {
		DiagPrintf("Cannot create power_ppe demo task\n\r");
	}

	if (rtos_task_create(NULL, "critial_region_test", (rtos_task_t)task6, NULL, (3072), (2)) != RTK_SUCCESS) {
		DiagPrintf("Cannot create critial_region_test demo task\n\r");
	}

}

u32 diagnose_Test(u16 argc, u8 *argv[])
{
	if (argc < 1) {
		DiagPrintf("Insufficient number of parameters!\n");
		return 1;
	}

	if (_strcmp((char *)argv[0], "ipc") == 0) {
		/*3.AP send event via IPC*/
		test_rtk_diag_send_via_ipc(NULL);
	} else if (_strcmp((char *)argv[0], "ipc_task") == 0) {
		test_rtk_diag_multi_task_add_via_ipc();
	}
	return 0;
}


CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   diagnose_test_cmd_table[] = {
	{
		(const u8 *)"diagnose",  4, diagnose_Test, (const  u8 *)"\tDiagnose \n"
		"\t\tdiagnose read/clear/time\n"
	},
};


#endif