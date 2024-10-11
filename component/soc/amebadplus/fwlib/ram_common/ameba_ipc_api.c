/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"

static const char *TAG = "IPC";
rtos_sema_t ipc_Semaphore[IPC_TX_CHANNEL_NUM];

/**
  * @brief  init ipc interrupt handler table.
  * @param  where IPCx can be IPCKM0_DEV for KM0, IPCKM4_DEV for CM4.
  * @retval   None
  */
void ipc_table_init(IPC_TypeDef *IPCx)
{
	u32 i;
	u32 IPC_Dir;
	u32 IPC_ChNum;
	u32 IPC_GroupShift;
	u32 IPC_ChShift;
	u32 IPC_IMR;
	u32 IPC_IntMode;

#if defined ( __ICCARM__ )
	__ipc_table_start__ = (u8 *)__section_begin(".ipc.table.data");
	__ipc_table_end__ = (u8 *)__section_end(".ipc.table.data");
#endif
	IPC_INIT_TABLE *ipc_init_table = (IPC_INIT_TABLE *)__ipc_table_start__;
	u32 ipc_num = ((__ipc_table_end__ - __ipc_table_start__) / sizeof(IPC_INIT_TABLE));

	for (i = 0; i < ipc_num; i++) {
		IPC_Dir = ipc_init_table[i].IPC_Direction;
		IPC_ChNum = ipc_init_table[i].IPC_Channel;

		if (IPC_Dir == IPC_KM4_TO_KM0) {
			IPC_IntMode = (IPCx == IPCKM4_DEV) ? IPC_TX_EMPTY : IPC_RX_FULL;

		} else {
			IPC_IntMode = (IPCx == IPCKM4_DEV) ? IPC_RX_FULL : IPC_TX_EMPTY;

		}

		/* set ipc group shift according to intr mode */
		if (IPC_IntMode == IPC_RX_FULL) {
			IPC_GroupShift = 0;
		} else {
			IPC_GroupShift = IPC_TX_CHANNEL_SHIFT;
		}

		IPC_ChShift = IPC_ChNum + IPC_GroupShift;

		/* check if channel conflict */
		IPC_IMR = IPC_IERGet(IPCx);
		if (IPC_IMR & BIT(IPC_ChShift)) {
			/* overwrite the old call back func for cpu reset happens */
			RTK_LOGI(TAG, "Channel Conflict for CPU %lu Channel %lu ! Ignore If CPU Has Reset\r\n", SYS_CPUID(), IPC_ChShift);
			continue;
		}

		if (IPC_IntMode == IPC_RX_FULL) {
			/* enable rx intr and register call back func*/
			IPC_INTUserHandler(IPCx, IPC_ChShift, (void *)ipc_init_table[i].Rxfunc, ipc_init_table[i].RxIrqData);
		} else {
			/* enable tx intr and register call back func*/
			IPC_INTUserHandler(IPCx, IPC_ChShift, (void *)ipc_init_table[i].Txfunc, ipc_init_table[i].TxIrqData);
		}
	}
}

/**
  * @brief  The common IPC Tx interrupt handler
  * @param  Data: the data pointer to IPCx
  * @param  IrqStatus: Value of IPC_ISR
  * @param  ChanNum: ChanNum
  */
void IPC_TXHandler(void *Data, u32 IrqStatus, u32 ChanNum)
{
	UNUSED(Data);
	UNUSED(IrqStatus);

	u32 CPUID = SYS_CPUID();
	IPC_TypeDef *IPCx = IPC_GetDevById(CPUID);

	IPC_INTConfig(IPCx, ChanNum, DISABLE);

	if (ipc_Semaphore[ChanNum - IPC_TX_CHANNEL_SHIFT] != NULL) {
		rtos_sema_give(ipc_Semaphore[ChanNum - IPC_TX_CHANNEL_SHIFT]);
	}
}


/**
  * @brief  Processing functions when the IPC channel is occupied
  * @param  IPCx: where IPCx can be IPCKM0_DEV for KM0, IPCKM4_DEV for CM4.
  * @param  IPC_ChNum: IPC_ChNum
  * @retval IPC_REQ_TIMEOUT or IPC_SEMA_TIMEOUT or SUCCESS
  */
u32 IPC_wait_idle(IPC_TypeDef *IPCx, u32 IPC_ChNum)
{
	u32 timeout;

	timeout = 10000000;

	if (CPU_InInterrupt() || (IPC_IrqHandler[IPC_ChNum + IPC_TX_CHANNEL_SHIFT] == NULL)) {
		while (IPCx->IPC_DATA & (BIT(IPC_ChNum + IPC_TX_CHANNEL_SHIFT))) {
			timeout--;
			if (timeout == 0) {
				RTK_LOGS(TAG, " IPC Request Timeout\r\n");
				return IPC_REQ_TIMEOUT;
			}
		}
	} else {
		if (ipc_Semaphore[IPC_ChNum] == NULL) {
			rtos_sema_create_binary(&ipc_Semaphore[IPC_ChNum]);
		}

		IPC_INTConfig(IPCx, IPC_ChNum + IPC_TX_CHANNEL_SHIFT, ENABLE);

		if (rtos_sema_take(ipc_Semaphore[IPC_ChNum], IPC_SEMA_MAX_DELAY) != SUCCESS) {
			RTK_LOGS(TAG, " IPC Get Semaphore Timeout\r\n");
			IPC_INTConfig(IPCx, IPC_ChNum + IPC_TX_CHANNEL_SHIFT, DISABLE);
			return IPC_SEMA_TIMEOUT;
		}
	}
	return 0;
}

/**
  * @brief  exchange messages between KM0 and KM4.
  * @param  IPC_Dir: Specifies core to core direction
  *          This parameter can be one of the following values:
  *		 		@arg IPC_KM0_TO_KM4: KM0 send request to KM4
  *		 		@arg IPC_KM4_TO_KM0: KM4 send request to KM0
  * @param  IPC_ChNum: the IPC channel number.
  * @param  Message: pointer to the message to be exchanged,and should not stored in stack.
  * @retval   None
  */
u32 ipc_send_message(u32 IPC_Dir, u8 IPC_ChNum, PIPC_MSG_STRUCT IPC_Msg)
{
	/* Check the parameters */
	assert_param(IS_IPC_DIR_MODE(IPC_Dir));
	assert_param(IS_IPC_VALID_CHNUM(IPC_ChNum));

	PIPC_MSG_STRUCT IPC_MSG = (PIPC_MSG_STRUCT)__km0_ipc_memory_start__;
	u32 msg_idx = IPC_TX_CHANNEL_NUM * IPC_TX_CHANNEL_SWITCH(IPC_Dir) + IPC_ChNum;
	IPC_TypeDef *IPCx;

	IPCx = IPC_GetDev(IPC_Dir, 0);

	if (IPCx->IPC_DATA & (BIT(IPC_ChNum + IPC_TX_CHANNEL_SHIFT))) {
		if (IPC_wait_idle(IPCx, IPC_ChNum)) {
			return IPC_SEND_TIMEOUT;
		}
	}

	if (IPC_Msg) {
		_memcpy(&IPC_MSG[msg_idx], IPC_Msg, sizeof(IPC_MSG_STRUCT));
		DCache_Clean((u32)&IPC_MSG[msg_idx], sizeof(IPC_MSG_STRUCT));
	}
	IPCx->IPC_DATA = (BIT(IPC_ChNum + IPC_TX_CHANNEL_SHIFT));

	return IPC_SEND_SUCCESS;
}

/**
  * @brief  get ipc message.
  * @param  IPC_Dir: Specifies core to core direction
  *          This parameter can be one of the following values:
  *		 		@arg IPC_KM0_TO_KM4: KM0 send request to KM4
  *		 		@arg IPC_KM4_TO_KM0: KM4 send request to KM0
  * @param  IPC_ChNum: the IPC channel number.
  * @retval  : pointer to the message to be exchanged.
  * @note for data message, corresponding data cache should be invalidate before access.
  */
PIPC_MSG_STRUCT ipc_get_message(u32 IPC_Dir, u8 IPC_ChNum)
{
	PIPC_MSG_STRUCT IPC_MSG = NULL;
	u32 msg_idx = IPC_TX_CHANNEL_NUM * IPC_TX_CHANNEL_SWITCH(IPC_Dir) + IPC_ChNum;

	IPC_MSG = (PIPC_MSG_STRUCT)__km0_ipc_memory_start__;
	DCache_Invalidate((u32)&IPC_MSG[msg_idx], sizeof(IPC_MSG_STRUCT));

	return &IPC_MSG[msg_idx];
}