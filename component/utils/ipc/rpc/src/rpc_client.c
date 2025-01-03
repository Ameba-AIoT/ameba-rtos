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
#define LOG_TAG "RPCClient"
#include "rpc_utils.h"
#include <string.h>

#include "ameba_soc.h"

#include "rpc_common.h"
#include "rpc_client.h"
#include "rpc_server.h"

#define MAX_TASK 10
typedef struct LockInfo LockInfo;
struct LockInfo {
	long taskId;
	RPC_Sem sem;
};


static struct THREAD_STRUCT *g_ClientStruct = NULL;
static int32_t g_ClientThreadRunning = 0;
LockInfo g_Locks[MAX_TASK];
static int32_t g_lockCount = 0;

void SigInit(long taskId)
{
	if (g_lockCount < MAX_TASK) {
		for (int i = 0; i < MAX_TASK; i++) {
			if (g_Locks[i].taskId == taskId) {
				return;
			}
		}
		g_Locks[g_lockCount].taskId = taskId;
		RPC_SemInit(&(g_Locks[g_lockCount].sem), 0);
		g_lockCount++;
	} else {
		RPC_LOGE("<<%s task count is full.\n", __FUNCTION__);
	}
}

void SigWait(long taskId)
{
	for (int i = 0; i < MAX_TASK; i++) {
		if (g_Locks[i].taskId == taskId) {
			RPC_SemWait(&(g_Locks[i].sem), RPC_SEM_WAIT_FOREVER);
			break;
		}
	}
}

void SigDel(long taskId)
{
	for (int i = 0; i < MAX_TASK; i++) {
		if (g_Locks[i].taskId == taskId) {
			RPC_SemPost(&(g_Locks[i].sem));
			break;
		}
	}
}

/*
 * Registered Reply Handler,
 * only use in BLOCK_MODE,
 * responsible for wake up the client task, and copy the result parameter
 */
static void ReplyHandler(RPC_STRUCT *rpc, int32_t opt, RPC_Mutex *mutex, char *param_buf)
{
	XDR xdrs;
	char *buf;
	u_long result_taskID;
	RPC_LOGD("<<%s Enter.\n", __FUNCTION__);
	if (param_buf == NULL) {
		//RPC_LOGD("param_buf is NULL will read buffer.\n");
		buf = rpc_malloc(rpc->parameter_size);
		if (buf == NULL) {
			RPC_MutexUnlock(mutex);
			return;
		}

		int32_t channel_id = (opt & 0xF0) >> 4;
		int32_t result = GetRPCManager()->ReadBuffer(channel_id, opt, (uint8_t *)buf, rpc->parameter_size);
		if (result < 0 || (u_long)result != rpc->parameter_size) {
			RPC_LOGE("read ring buffer error \n");
			rpc_free(buf);
			RPC_MutexUnlock(mutex);
			return;
		}
	} else {
		buf = param_buf;
	}

	RPC_MutexUnlock(mutex);
	// decode the taskID.
	xdrmem_create(&xdrs, buf, sizeof(u_long), XDR_DECODE);
	if (!xdr_u_long(&xdrs, &result_taskID)) {
		RPC_LOGD("xdr_u_long false...\n");
		return;
	}
	buf += sizeof(u_long);

	// copy the result parameter and wake up the task by taskID.
	memcpy((char *)rpc->context, buf, (rpc->parameter_size - sizeof(u_long)));
	buf -= sizeof(u_long); //modified by ycyang
	if (param_buf == NULL) {
		rpc_free(buf);
	}

	SigDel((long)result_taskID);
}

int32_t RPC_ClientCall(RPC_STRUCT *rpc, u_long procedure_id, int32_t opt, xdrproc_t xdr_args, caddr_t args_ptr, long args_size)
{
	XDR xdrs;
	char *mem_ToShm;         // Memory start address for writing to Share-memory
	int32_t size_ToShm = 0;      // size of memory for writing to Share-memory
	int32_t cnt;                 // number of bytes writed to Share-memory
	RPC_LOGD(">>%s Enter.program_id=%d, procedure_id=%d\n", __FUNCTION__, (int)rpc->program_id, (int)procedure_id);
	// assign RPC_STRUCT's value
	rpc->procedure_id = procedure_id;
	/*
	    * prepare continuous memory for sending to share-memory
	    *    start  ---------------
	    *           |  RPC_STRUCT |
	    *           |-------------|
	    *           | user's args |
	    *           |-------------|
	    */
	// prepare XDR's memory location of ARGS
	// because we need args_size after XDR
	// so we do this first.
	mem_ToShm = rpc_malloc(sizeof(RPC_STRUCT) + args_size);
	mem_ToShm += sizeof(RPC_STRUCT);


	xdrmem_create(&xdrs, (char *)mem_ToShm, args_size, XDR_ENCODE);
	if ((*xdr_args)(&xdrs, args_ptr) == 0) {
		RPC_LOGE("XDR error \n");
		return RPC_ERROR;
	}

	rpc->parameter_size = xdr_getpos(&xdrs);
	size_ToShm += rpc->parameter_size;
	mem_ToShm -= sizeof(RPC_STRUCT);

	// prepare XDR's memory location of RPC_STRUCT
	xdrmem_create(&xdrs, (char *)mem_ToShm, sizeof(RPC_STRUCT), XDR_ENCODE);
	if (xdr_RPC_STRUCT(&xdrs, rpc) == 0) {
		RPC_LOGE(" XDR_RPC_STRUCT error \n");
		return RPC_ERROR;
	}
	size_ToShm += xdr_getpos(&xdrs);

	int32_t channel_id = (opt & 0x0E) >> 1;
#if RPC_DEBUG
	RPC_LOGD("RPC_ClientCall channel_id=%ld, opt=%x\n", channel_id, opt);
#endif
	cnt = GetRPCManager()->WriteBuffer(channel_id, opt, (uint8_t *)mem_ToShm, size_ToShm);
	rpc_free(mem_ToShm);

	// if writeRingBuf error,
	// return to the user send error,
	// we dont re-send it.
	if (cnt == size_ToShm) {
		return RPC_SUCCESS;
	} else {
		RPC_LOGD("cnt: %d, size_ToShm: %d\n", (int)cnt, (int)size_ToShm);
		return RPC_ERROR;
	}
}

RPC_STRUCT RPC_PrepareCall(CLNT_STRUCT *clnt, int32_t result)
{
	RPC_STRUCT rpc;

	// if NONBLOCK_MODE, Task ID must be 0
	if (clnt->send_mode & BLOCK_MODE) {
		long taskHandle = (long) rtos_task_handle_get();
		rpc.task_id = taskHandle;
		SigInit(taskHandle);
	} else {
		rpc.task_id = 0;
	}
	rpc.program_id = clnt->program_id;
	rpc.version_id = clnt->version_id;
	rpc.context = (u_int)result;
	rpc.parameter_size = 0;
	rpc.procedure_id = 0;
	return rpc;
}

/*
 * The function used to register ReplyHandler
 * the program ID and version ID are assignd as "REPLYID"
 */
struct REG_STRUCT *ReplyHandler_register(struct REG_STRUCT *reg)
{
	struct REG_STRUCT *newReg = (struct REG_STRUCT *)
								RPC_Register(reg, REPLYID, REPLYID, ReplyHandler);
	return newReg;
}

int32_t WaitReply(void)
{
	long taskHandle = (long) rtos_task_handle_get();
	SigWait(taskHandle);
	return RPC_SUCCESS;
}

void RPC_DeInitClient(void)
{
	RPC_LOGD("***RPC_DeInitClient...\n");
	if (g_ClientStruct) {
		rpc_free(g_ClientStruct);
		g_ClientStruct = NULL;
	}
	g_ClientThreadRunning = 0;
}