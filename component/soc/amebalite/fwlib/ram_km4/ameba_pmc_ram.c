/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

//static const char *const TAG = "PMC";

/**
  * @brief  KR4 wake KM4 by IPC
  * @param  None
  * @note  ipc_msg_temp.msg represents who wakes up KM4
  		* ipc_msg_temp.msg = 0: FW wakeup KM4
  * @retval None
  */
void SOCPS_KR4WKM4_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);

	PIPC_MSG_STRUCT	ipc_msg_temp = (PIPC_MSG_STRUCT)ipc_get_message(IPC_KR4_TO_KM4, IPC_R2M_WAKE_AP);

	u32 type = ipc_msg_temp->msg;

	if (type == FW_NPWAP_IPC) {
		//RTK_LOGD(TAG, "FW wakeup KM4 via IPC \n");
	} else if (type == TIMER_NPWAP_IPC) {
		//RTK_LOGD(TAG, "TIMER wakeup KM4 via IPC \n");
	}

}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_KR4WKM4_table = {
	.USER_MSG_TYPE = IPC_USER_DATA,
#ifndef CONFIG_MP_SHRINK
	.Rxfunc = SOCPS_KR4WKM4_ipc_int,
#else
	.Rxfunc = NULL,
#endif
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_KR4_TO_KM4,
	.IPC_Channel = IPC_R2M_WAKE_AP
};

