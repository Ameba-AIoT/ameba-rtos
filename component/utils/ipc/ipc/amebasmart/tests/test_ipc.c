/*
 * Copyright (c) 2022 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "utils/os.h"
#include "ipc.h"


#define IMQ_BUF_SIZE1 			256
#define IMQ_BUF_SIZE2 			512
#define IMQ_TEST_COUNT1			10000
#define IMQ_TEST_COUNT2			5000

#define QUEUE_AP_TO_NP			1
#define QUEUE_NP_TO_AP			0

void RPCServerTask(void *param)
{
	(void)param;
	printf("###########%s Enter.\n", __FUNCTION__);

	u8 buf[IMQ_BUF_SIZE1];
	uint32_t size;
	int count = 0;
	int total_size = 0;

	printf("%s, rx task start!\n", __FUNCTION__);

	while (count < IMQ_TEST_COUNT1) {

		memset(buf, 0x00, IMQ_BUF_SIZE1);
		size = IMQ_BUF_SIZE1;
		int32_t ret = IPC_Message_Queue_Recv(QUEUE_AP_TO_NP, buf, &size);

		count++;
		total_size += size;

		//if (count % 20 == 0)
		{
			printf("%x recv %d packets, %d bytes, ret=%d\n", QUEUE_AP_TO_NP, (int)count, (int)total_size, (int)ret);
		}
	}
	rtos_time_delay_ms(200);
	printf("%x recv %d packets, %d bytes\n", QUEUE_AP_TO_NP, (int)count, (int)total_size);

	rtos_task_delete(NULL);
}

void RPCClientTask(void *param)
{
	(void)param;
	printf("***************%s Enter.\n", __FUNCTION__);
	u8 buf[IMQ_BUF_SIZE2];
	int count = 0;
	int total_size = 0;

	printf("%s, tx task start!\n", __FUNCTION__);

	while (count < IMQ_TEST_COUNT2) {
		memset(buf, 0x00, IMQ_BUF_SIZE2);
		os_get_random_bytes(buf, IMQ_BUF_SIZE2);

		count++;
		total_size += IMQ_BUF_SIZE2;
		int32_t ret = IPC_Message_Queue_Send(QUEUE_AP_TO_NP, buf, IMQ_BUF_SIZE2, WAIT_FOREVER);
		//if (count % 11 == 0)
		{
			printf("%x send %d packets, %d bytes, ret=%d\n", QUEUE_AP_TO_NP, (int)count, (int)total_size, (int)ret);
		}
		rtos_time_delay_ms(2);
		//printf("%s, RPC_Data_Write = %d\n", __FUNCTION__, (int)ret);
	}
	rtos_time_delay_ms(200);
	printf("%x send %d packets, %d bytes\n", QUEUE_AP_TO_NP, (int)count, (int)total_size);

	rtos_task_delete(NULL);
}

void TestTask(void *param)
{
	(void)param;
	printf("TestTask\n");
	IPC_Message_Queue_Init();
	//rtos_time_delay_ms(2000);
#if defined(CONFIG_IPC_MSG_MASTER)
	printf("start RPCServerTask\n");
	rtos_task_create(NULL, ((const char *)"RPCServerTask"), RPCServerTask, NULL, 1024 * 4, 2);
#else
	printf("start RPCClientTask\n");
	rtos_task_create(NULL, ((const char *)"RPCClientTask"), RPCClientTask, NULL, 1024 * 4, 2);
#endif
	rtos_task_delete(NULL);
}

uint32_t test_ipc_message(uint16_t argc, uint8_t *argv[])
{
	(void) argc;
	(void) argv;
	rtos_task_create(NULL, ((const char *)"TestTask"), TestTask, NULL, 1024, 1);
	return 0;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE ipc_message_test_cmd_table[] = {
	{"test_ipc_message", test_ipc_message},
};