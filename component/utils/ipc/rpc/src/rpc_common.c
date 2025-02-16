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

#define LOG_TAG "RPCCommon"
#include "rpc_utils.h"

#include "xdr/xdr.h"
#include "xdr/xdr_prefix.h"

#include "rpc_struct.h"

#include "rpc_api.h"
#include "rpc_common.h"
#include "rpc_client.h"
#include "rpc_server.h"

#include "ameba_soc.h"

//#define VOICE_RPC_TEST
//#define CONFIG_RPC_TEST
#ifdef CONFIG_ARM_CORE_CM4
#define RPC_CHANNEL_AP2NP (RPC_AP_NP<<4) | (BLOCK_MODE | (RPC_NP_AP << 1))
#define RPC_CHANNEL_2DSP (RPC_DSP_NP<<4) | (BLOCK_MODE | (RPC_NP_DSP << 1))
#else
#define RPC_CHANNEL_AP2NP (RPC_NP_AP<<4) | (BLOCK_MODE | (RPC_AP_NP << 1))
#define RPC_CHANNEL_2DSP (RPC_DSP_AP<<4) | (BLOCK_MODE | (RPC_AP_DSP << 1))
#endif
#ifdef CONFIG_RPC_TEST
#if defined(CONFIG_ARM_CORE_CM4)
extern struct REG_STRUCT *TEST1_AGENT_0_register(struct REG_STRUCT *rnode);
extern struct REG_STRUCT *TEST2_AGENT_0_register(struct REG_STRUCT *rnode);
#else
extern struct REG_STRUCT *TEST1_SYSTEM_0_register(struct REG_STRUCT *rnode);
#endif
#endif

_WEAK struct REG_STRUCT *VOICE_AGENT_0_register(struct REG_STRUCT *rnode)
{
	(void) rnode;
	return NULL;
}

static RPCHwManager *g_rpc_manager = NULL;
static RPC_Mutex g_lock;
static bool g_inited = false;
static RPC_INIT_STRUCT g_init_param;

/*
 * XDR type of struct RPC_STRUCT
 */
bool_t xdr_RPC_STRUCT(XDR *xdrs, RPC_STRUCT *objp)
{
	if (!xdr_u_long(xdrs, &objp->program_id)) {
		return FALSE;
	}
	if (!xdr_u_long(xdrs, &objp->version_id)) {
		return FALSE;
	}
	if (!xdr_u_long(xdrs, &objp->procedure_id)) {
		return FALSE;
	}
	if (!xdr_u_long(xdrs, &objp->task_id)) {
		return FALSE;
	}
	if (!xdr_u_long(xdrs, &objp->parameter_size)) {
		return FALSE;
	}
	if (!xdr_u_int(xdrs, &objp->context)) {
		return FALSE;
	}

	return TRUE;
};

/*
 * assign user's setting value into the CLNT_STRUCT
 */
CLNT_STRUCT RPC_PrepareCLNT(int32_t opt, long program_id, long version_id)
{
	CLNT_STRUCT clnt;
	clnt.send_mode = opt;
	clnt.program_id = program_id;
	clnt.version_id = version_id;
	return clnt;
}

static void RPC_InitThread(void *param)
{
	RPC_INIT_STRUCT *init_param = (RPC_INIT_STRUCT *)param;
	if (g_rpc_manager) {
		RPC_LOGE("RPC_Init already DONE.\n");
		rtos_task_delete(NULL);
		return;
	}
	if (!g_rpc_manager) {
		g_rpc_manager = GetRPCManager();
	}
	if (g_rpc_manager) {
		RPC_LOGD("GetRPCManager Success.\n");
	} else {
		RPC_LOGE("GetRPCManager failed.\n");
	}

	struct REG_STRUCT *pReg = NULL;
	pReg = VOICE_AGENT_0_register(pReg);
	pReg = ReplyHandler_register(pReg);
	RPC_InitProxy(pReg, RPC_CHANNEL_2DSP, init_param);

#ifdef CONFIG_RPC_TEST
#if defined(CONFIG_ARM_CORE_CM4)
	struct REG_STRUCT *pReg1 = NULL;
	struct REG_STRUCT *pReg2 = NULL;
	pReg1 = TEST1_AGENT_0_register(pReg1);
	pReg1 = ReplyHandler_register(pReg1);
	RPC_InitProxy(pReg1, RPC_CHANNEL_AP2NP, init_param);

	pReg2 = TEST2_AGENT_0_register(pReg2);
	pReg2 = ReplyHandler_register(pReg2);
	RPC_InitProxy(pReg2, RPC_CHANNEL_2DSP, init_param);
#else
	struct REG_STRUCT *pReg1 = NULL;
	pReg1 = TEST1_SYSTEM_0_register(pReg1);
	pReg1 = ReplyHandler_register(pReg1);
	RPC_InitProxy(pReg1, RPC_CHANNEL_AP2NP, init_param);
#endif
#endif
	g_inited = true;
	rtos_task_delete(NULL);
}

void ipc_app_init(int32_t task_size, int32_t priority, int32_t log_enable)
{
	g_init_param.task_size = task_size;
	g_init_param.priority = priority;

	RPC_SetLog(log_enable);
	RPC_Init(&g_init_param);
}

/*
 * init RPC
 */
int32_t RPC_Init(RPC_INIT_STRUCT *init_param)
{
	RPC_MutexInit(&g_lock);
	if (rtos_task_create(NULL, ((const char *)"RPC_InitThread"), RPC_InitThread, init_param, 1024 * 4, 1) != SUCCESS) {
		RPC_LOGE("\n\r%s rtos_task_create(RPC_InitThread) failed", __FUNCTION__);
	}
	return 0;
}

void RPC_DeInit(void)
{
	RPC_DeInitClient();
	RPC_DeInitServer();
	RPC_MutexLock(&g_lock);
	if (g_rpc_manager) {
		DestoryRPCHwManager(g_rpc_manager);
		g_rpc_manager = NULL;
	}
	RPC_MutexUnlock(&g_lock);
	RPC_MutexDestroy(&g_lock);
}

RPCHwManager *GetRPCManager(void)
{
	RPC_MutexLock(&g_lock);
	if (!g_rpc_manager) {
		g_rpc_manager = GetRPCHwManager();
	}
	RPC_MutexUnlock(&g_lock);
	return g_rpc_manager;
}

int32_t RPC_IsInited(void)
{
	int32_t ret = false;
	RPC_MutexLock(&g_lock);
	ret = g_inited ? 1 : 0;
	RPC_MutexUnlock(&g_lock);
	return ret;
}
