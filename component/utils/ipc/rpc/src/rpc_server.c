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
#define LOG_TAG "RPCServer"
#include "rpc_utils.h"

#include "ameba_soc.h"

#include "rpc_common.h"
#include "rpc_server.h"


#define NUM_HANDLER_THREADS 4
static int32_t g_ServerThreadRunning = 0;
rtos_task_t g_receiver_threads[NUM_HANDLER_THREADS];
static int32_t g_thread_count = 0;

/*
 * Dispatch Function
 * compare the REG Linking-List's value
 * according to program_id and version_id, dispatch to different handler
 */
void DispatchRPCRequest(struct REG_STRUCT *reg, char *buf, int32_t opt, RPC_Mutex *mutex, int32_t size)
{
	RPC_STRUCT rpc;
	XDR xdrs;
	xdrmem_create(&xdrs, buf, sizeof(RPC_STRUCT), XDR_DECODE);
	xdr_RPC_STRUCT(&xdrs, &rpc);

	// according to the program_id and version_id,
	// dispatch to the corresponding function
	for (; reg != NULL ; reg = reg->next) {
		RPC_LOGD("---Dispatch rpc.program_id=%d ,reg->program_id=%d\n", (int)rpc.program_id, (int) reg->program_id);
		if ((rpc.program_id == reg->program_id) &&
			(rpc.version_id == reg->version_id)) {
			if (size > (int32_t) sizeof(RPC_STRUCT)) {
				(*(reg->func))(&rpc, opt, mutex, buf + sizeof(RPC_STRUCT));
			} else {
				(*(reg->func))(&rpc, opt, mutex, NULL);
			}
			break;
		}
	}
}


/*
 *  The entry of Server Proxy
 *  responsible for reading from INTERRUPT & POLL ring-buffer
 *  then call dispatch
 */
void ReceiverLoop(void *param)
{
	struct THREAD_STRUCT *pData = (struct THREAD_STRUCT *) param;
	RPC_STRUCT buf;
	int32_t cnt;
	while (g_ServerThreadRunning) {

		RPC_MutexLock(pData->mutex);
		while (g_ServerThreadRunning) {
			int32_t channel_id = (pData->flag & 0xF0) >> 4;
			RPC_LOGD("ReceiverLoop read...programID=%d, receiver id=%d, pData->flag=%d\n", (int)pData->reg_ptr->program_id, (int)channel_id, (int)pData->flag);

			if ((cnt = GetRPCManager()->ReadBuffer(channel_id, BLOCK_MODE, (uint8_t *) &buf, sizeof(RPC_STRUCT))) > 0) {
				RPC_LOGD("read from intr ring buffer %d bytes\n", (int)cnt);
				DispatchRPCRequest(pData->reg_ptr, (char *) &buf, BLOCK_MODE | pData->flag, pData->mutex, cnt);
				break;
			}
			RPC_LOGD("***waiting timeout...\n");
		}
	}

	RPC_MutexUnlock(pData->mutex);
	RPC_MutexDestroy(pData->mutex);
	rpc_free(pData->mutex);
	rpc_free(pData);
	rtos_task_delete(NULL);
}


/*
 * read the IPC packet's body,
 * after XDR transformation, store the parameter
 */
int32_t RPC_GetArgs(RPC_STRUCT *rpc, xdrproc_t xdr_argument, caddr_t argument, int32_t opt, char *param_buf)
{
	XDR xdrs;
	char *buf;

	// according to the parameter size recording in RPC_STRUCT
	// read the packet's body from ring buffer.
	if (param_buf == NULL) {
		buf = rpc_malloc(rpc->parameter_size);
		int32_t channel_id = (opt & 0xF0) >> 4;
		RPC_LOGD("RPC_GetArgs will read buffer opt=%d, channel_id=%d\n", (int)opt, (int)channel_id);
		if (GetRPCManager()->ReadBuffer(channel_id, opt, (uint8_t *)buf, rpc->parameter_size) != (int32_t) rpc->parameter_size) {
			RPC_LOGE("read ring buffer error \n");
			rpc_free(buf);
			return 0;
		}
	} else {
		buf = param_buf;
	}

	xdrmem_create(&xdrs, buf, rpc->parameter_size, XDR_DECODE);

	// return the XDR transformation's result,
	// if success, must rpc_free the xdrs
	if ((*xdr_argument)(&xdrs, argument)) {
		xdrs.x_op = XDR_FREE;
		(*xdr_argument)(&xdrs, argument);
		if (param_buf == NULL) {
			rpc_free(buf);
		}
		return 1;
	} else {
		if (param_buf == NULL) {
			rpc_free(buf);
		}
		return 0;
	}
}


/*
 * assign register values into the REG linking list
 * ( program_id,
 *   version_id,
 *   dispatch function pointer)
 */
struct REG_STRUCT *RPC_Register(struct REG_STRUCT *reg, u_long program_id, u_long version_id,
								void (*func)(struct RPC_STRUCT *, int32_t, RPC_Mutex *, char *))
{
	if (reg == NULL) {
		reg = (struct REG_STRUCT *)rpc_malloc(sizeof(struct REG_STRUCT));
		if (reg == NULL) {
			RPC_LOGE("(register error! alloc memory fail\n");
			return 0;
		}
		reg->program_id = program_id;
		reg->version_id = version_id;
		reg->func = (void (*)(struct RPC_STRUCT *, int, RPC_Mutex *, char *))func;
		reg->next = NULL;
	} else if ((reg->program_id == program_id) && (reg->version_id == version_id)) {
		RPC_LOGD("(register error! program and version has been registered \n");
		return 0;
	} else {
		reg->next = RPC_Register(reg->next, program_id, version_id, func);
	}
	return reg;

}

int32_t RPC_Unregister(struct REG_STRUCT *reg)
{
	int32_t i, j, ptr[10];
	struct REG_STRUCT *tmp = reg;

	for (j = 0; j < 10; j++) {
		ptr[j] = 0;
	}

	i = 0;

	while (tmp->next) {
		ptr[i] = (int32_t)tmp;
		i++;
		tmp = tmp->next;
	}

	rpc_free(tmp);
	for (j = (i - 1); j >= 0; j--) {
		rpc_free((void *)ptr[j]);
	}

	return 1;
}


int32_t RPC_InitProxy(struct REG_STRUCT *reg, int32_t channel_id)
{
	if (g_ServerThreadRunning == 0) {
		g_ServerThreadRunning = 1;
	}

	RPC_LOGD("%s Enter, program_id=%lu, channel_id=%d, g_thread_count=%d\n", __FUNCTION__, reg->program_id, (int)channel_id, (int)g_thread_count);
	if (g_thread_count >= NUM_HANDLER_THREADS) {
		return -1;
	}
	RPC_Mutex *pMutex = rpc_malloc(sizeof(RPC_Mutex));
	RPC_MutexInit(pMutex);
	for (uint8_t i = 0; i < 2; i++) {
		THREAD_STRUCT *serverStruct = rpc_malloc(sizeof(THREAD_STRUCT));
		serverStruct->reg_ptr = reg;
		serverStruct->mutex = pMutex;
		serverStruct->flag = channel_id;
		g_receiver_threads[g_thread_count] = NULL;
		if (rtos_task_create(&g_receiver_threads[g_thread_count], ((const char *)"ReceiverLoop"), ReceiverLoop, (void *)serverStruct, 1024 * 4,
							 3) != SUCCESS) {
			RPC_LOGE("\n\r%s rtos_task_create(ReceiverLoop) failed", __FUNCTION__);
		}
		g_thread_count++;
	}
	return RPC_SUCCESS;
}


/*
 * The function apply to server proxy
 * used to send the reply back
 */
void RPC_SendReply(u_long req_taskID, int32_t req_context, char *reply_param, u_long param_size, xdrproc_t xdr_result, int32_t opt)
{
	RPC_STRUCT rpc;
	XDR xdrs;
	char *mem_ToShm;        // mem start addr for writing to share memory
	int32_t size_ToShm = 0 ;    // total mem size for writing to share memory

	// when reply, RPC's context must be the parameter addr we want to reply.
	RPC_LOGD(">>>%s Enter. opt=%d\n", __FUNCTION__, (int)opt);
	rpc.program_id = REPLYID;
	rpc.version_id = REPLYID;
	rpc.procedure_id = 0;
	rpc.context = (u_int) req_context;  // fill in req's para addr
	rpc.task_id = 0;

	////////////////////////////////////////////////////////
	//               _______________
	//               |  RPC_STRUCT  |
	//               ----------------
	//               | req's TaskID |   <- use to wake up request func
	//               ----------------
	//               |  Reply para. |
	//               ----------------
	mem_ToShm = rpc_malloc(sizeof(RPC_STRUCT) + sizeof(u_long) + param_size);
	mem_ToShm += sizeof(RPC_STRUCT);

	// TaskID
	xdrmem_create(&xdrs, mem_ToShm, sizeof(u_long), XDR_ENCODE);
	if (!xdr_u_long(&xdrs, &req_taskID)) {
		RPC_LOGD("get xdr_u_long error...\n");
		return;
	}
	size_ToShm += sizeof(u_long);
	mem_ToShm += size_ToShm;

	// Reply Parameter
	xdrmem_create(&xdrs, mem_ToShm, param_size, XDR_ENCODE);
	(*xdr_result)(&xdrs, reply_param);
	size_ToShm += xdr_getpos(&xdrs);
	rpc.parameter_size = size_ToShm;
	mem_ToShm -= (sizeof(u_long) + sizeof(RPC_STRUCT));

	// RPC_STRUCT
	xdrmem_create(&xdrs, mem_ToShm, sizeof(RPC_STRUCT), XDR_ENCODE);
	xdr_RPC_STRUCT(&xdrs, &rpc);
	size_ToShm += xdr_getpos(&xdrs);
	int32_t channel_id = (opt & 0x0E) >> 1;
	RPC_LOGD("RPC_SendReply channel_id=%d, opt=%d\n", (int)channel_id, (int)opt);
	GetRPCManager()->WriteBuffer(channel_id, opt, (uint8_t *)mem_ToShm, size_ToShm);
	rpc_free(mem_ToShm);
}

void RPC_DeInitServer(void)
{
	RPC_LOGD("***RPC_DeInitServer...\n");
	g_ServerThreadRunning = 0;
	for (uint8_t i = 0; i < NUM_HANDLER_THREADS; i++) {
		if (g_receiver_threads[i]) {
			rtos_task_delete(g_receiver_threads[i]);
		}
	}
}