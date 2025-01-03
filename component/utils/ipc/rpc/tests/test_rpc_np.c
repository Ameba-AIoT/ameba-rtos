#include "Test1RPC_Agent.h"
#include "Test2RPC_Agent.h"

#include "ameba_soc.h"
#include "rpc_api.h"


extern void test_rpc1(void);
extern void test_rpc2(void);

void mcu_voice_task(void *param)
{
	(void) param;
	test_rpc1();
	//test_rpc2();

	rtos_task_delete(NULL);
}

void test_rpc0(void)
{
	rtos_task_create(NULL, ((const char *)"mcu_voice_task"), mcu_voice_task, NULL, 1024, 1);
}



