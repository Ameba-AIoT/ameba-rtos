/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

//static const char *TAG = "PMC";

/**
  * @brief  KM4 wake KR4 by IPC
  * @param  None
  * @note  ipc_msg_temp.msg represents who wakes up KR4
  		* ipc_msg_temp.msg = 0: FW wakeup KR4
  * @retval None
  */
void SOCPS_KM4WKR4_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);

	PIPC_MSG_STRUCT	ipc_msg_temp = (PIPC_MSG_STRUCT)ipc_get_message(IPC_KM4_TO_KR4, IPC_M2R_WAKE_AP);

	u32 type = ipc_msg_temp->msg;

	if (type == FW_NPWAP_IPC) {
		//RTK_LOGD(TAG, "FW wakeup KR4 via IPC \n");
	} else if (type == TIMER_NPWAP_IPC) {
		//RTK_LOGD(TAG, "TIMER wakeup KR4 via IPC \n");
	}

}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_KM4WKR4_table = {
	.USER_MSG_TYPE = IPC_USER_DATA,
#ifndef CONFIG_MP_SHRINK
	.Rxfunc = SOCPS_KM4WKR4_ipc_int,
#else
	.Rxfunc = NULL,
#endif
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_KM4_TO_KR4,
	.IPC_Channel = IPC_M2R_WAKE_AP
};

