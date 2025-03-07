#include "Test2RPC_Agent_data.h"
#include "Test2RPC_System.h"
#include "Test2RPC_Agent.h"

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "rpc_api.h"
#include "rpc_struct.h"


#ifdef CONFIG_ARM_CORE_CM4
#define TEST2_RPC_MODE (BLOCK_MODE | (RPC_NP_DSP << 1) | (RPC_DSP_NP << 4))
#else
#define TEST2_RPC_MODE (BLOCK_MODE | (RPC_AP_DSP << 1) | (RPC_DSP_AP << 4))
#endif

static int32_t TestInit(void)
{
	TEST2_RPC_MIC_DATA arg;
	arg.data_addr = 0;
	arg.length = 0;
	CLNT_STRUCT clnt = RPC_PrepareCLNT(TEST2_RPC_MODE, TEST2_SYSTEM, 0);
	TEST2_RPCRES_INIT *res = TEST2_RPC_ToAgent_Init_0(&arg, &clnt);
	free(res);
	return 0;
}


static bool SyncData(void)
{
	bool ret = false;
	TEST2_RPC_MIC_DATA arg;
	arg.data_addr = 0;
	arg.length = 0;
	printf("TEST2_RPC_ToAgent_Sync_Mic_Data_0 Start\n");
	CLNT_STRUCT clnt = RPC_PrepareCLNT(TEST2_RPC_MODE, TEST2_SYSTEM, 0);
	HRESULT *res = TEST2_RPC_ToAgent_Sync_Mic_Data_0(&arg, &clnt);
	printf("TEST2_RPC_ToAgent_Sync_Mic_Data_0 End\n");
	ret = (*res == 0);
	free(res);
	return ret;
}

static void TestTask2(void *param)
{
	(void)param;
	printf("TestTask2 Start\n");
	rtos_time_delay_ms(3000);
	int count = 1000;
	TestInit();
	while (count > 0) {
		SyncData();
		//count--;
		rtos_time_delay_ms(2);
	}
	rtos_task_delete(NULL);
}

void test_rpc2(void)
{
	printf("test_rpc2 Start\n");
	rtos_task_create(NULL, ((const char *)"TestTask2"), TestTask2, NULL, 1024 * 4, 1);
}