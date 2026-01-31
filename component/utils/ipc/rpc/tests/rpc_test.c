/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"


extern void test_rpc(void);
extern void test_rpc0(void);
extern int32_t RPC_Init(void);

void rpc_test(void)
{
	RPC_Init();
	rtos_time_delay_ms(2000);
#if defined(CONFIG_ARM_CORE_CM4)
	test_rpc0();
#else
	test_rpc();
#endif
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE rpc_test_cmd_table[] = {
	{"rpc_test", rpc_test},
};