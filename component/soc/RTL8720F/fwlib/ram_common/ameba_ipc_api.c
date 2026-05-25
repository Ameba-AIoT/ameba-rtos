/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "IPC";
rtos_sema_t ipc_Semaphore[IPC_TX_CHANNEL_NUM];

/**
  * @brief  init ipc interrupt handler table.
  * @param  where IPCx can be IPCNP_DEV for NP, IPCAP_DEV for CM4.
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

	IPC_INIT_TABLE *ipc_init_table = (IPC_INIT_TABLE *)__ipc_table_start__;
	u32 ipc_num = ((__ipc_table_end__ - __ipc_table_start__) / sizeof(IPC_INIT_TABLE));
#ifndef CONFIG_IPC_UNSHARE_MODE
	IPC_SelectMode(IPCx, IPC_SHARE_MODE);
#else
	IPC_SelectMode(IPCx, IPC_UNSHARE_MODE);
	IPC_SetTxThreshold(IPCx, IPC_TX_LEVEVL);
	IPC_SetTxtTimeout(IPCx, IPC_TX_TIMEOUT);
#endif

	for (i = 0; i < ipc_num; i++) {
		IPC_Dir = ipc_init_table[i].IPC_Direction;
		IPC_ChNum = ipc_init_table[i].IPC_Channel;

		if (IPC_Dir == IPC_AP_TO_NP) {
			IPC_IntMode = (IPCx == IPCAP_DEV) ? IPC_TX_EMPTY : IPC_RX_FULL;

		} else {
			IPC_IntMode = (IPCx == IPCAP_DEV) ? IPC_RX_FULL : IPC_TX_EMPTY;

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
			RTK_LOGW(TAG, "Channel Conflict for CPU %lu Channel %lu ! Ignore If CPU Has Reset\r\n", SYS_CPUID(), IPC_ChShift);
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
  * @param  IPCx: where IPCx can be IPCNP_DEV for NP, IPCAP_DEV for CM4.
  * @param  IPC_ChNum: IPC_ChNum
  * @retval IPC_REQ_TIMEOUT or IPC_SEMA_TIMEOUT or RTK_SUCCESS
  */
u32 IPC_wait_idle(IPC_TypeDef *IPCx, u32 IPC_ChNum)
{
	u32 timeout;

	timeout = 10000000;

	if (CPU_InInterrupt() || (IPC_IrqHandler[IPC_ChNum + IPC_TX_CHANNEL_SHIFT] == NULL)) {
		while (IPCx->IPC_TX_CH_ST & (BIT(IPC_ChNum))) {
			timeout--;
			if (timeout == 0) {
				RTK_LOGS(TAG,  RTK_LOG_WARN, " IPC Request Timeout\r\n");
				return IPC_REQ_TIMEOUT;
			}
		}
	} else {
		if (ipc_Semaphore[IPC_ChNum] == NULL) {
			rtos_sema_create_binary(&ipc_Semaphore[IPC_ChNum]);
		}

		/* clear pending interrupt status */
		IPC_INTClear(IPCx, IPC_ChNum + IPC_TX_CHANNEL_SHIFT);

		/* if TX channel cleared during waiting then break waiting */
		if (IPCx->IPC_TX_CH_ST & (BIT(IPC_ChNum))) {
			IPC_INTConfig(IPCx, IPC_ChNum + IPC_TX_CHANNEL_SHIFT, ENABLE);
			while (rtos_sema_take(ipc_Semaphore[IPC_ChNum], IPC_SEMA_MAX_DELAY) != RTK_SUCCESS) {
				RTK_LOGS(TAG,  RTK_LOG_WARN, " IPC Get Semaphore Timeout\r\n");
			}
			IPC_INTConfig(IPCx, IPC_ChNum + IPC_TX_CHANNEL_SHIFT, DISABLE);
		}
	}
	return 0;
}


/**
  * @brief  exchange messages between AP and NP.
  * @param  IPC_Dir: Specifies core to core direction
  *          This parameter can be one of the following values:
  *		 		@arg IPC_KM0_TO_KM4: KM0 send request to KM4
  *		 		@arg IPC_KM4_TO_KM0: KM4 send request to KM0
  * @param  IPC_ChNum: the IPC channel number.
  * @param  IPC_Msg: pointer to the message to be exchanged,and should not stored in stack.
  * @retval   None
  */
u32 ipc_send_message(u32 IPC_Dir, u8 IPC_ChNum, PIPC_MSG_STRUCT IPC_Msg)
{
#ifndef CONFIG_IPC_UNSHARE_MODE
	/* Check the parameters */
	assert_param(IS_IPC_DIR_MODE(IPC_Dir));
	assert_param(IS_IPC_VALID_CHNUM(IPC_ChNum));

	PIPC_MSG_STRUCT IPC_MSG = (PIPC_MSG_STRUCT)__ipc_memory_start__;
	u32 msg_idx = IPC_TX_CHANNEL_NUM * IPC_TX_CHANNEL_SWITCH(IPC_Dir) + IPC_ChNum;
	IPC_TypeDef *IPCx;

	IPCx = IPC_GetDev(IPC_Dir, 0);

	if (IPCx->IPC_TX_CH_ST & (BIT(IPC_ChNum))) {
		if (IPC_wait_idle(IPCx, IPC_ChNum)) {
			return IPC_SEND_TIMEOUT;
		}
	}

	if (IPC_Msg) {
		_memcpy(&IPC_MSG[msg_idx], IPC_Msg, sizeof(IPC_MSG_STRUCT));
		DCache_Clean((u32)&IPC_MSG[msg_idx], sizeof(IPC_MSG_STRUCT));
	}

	IPCx->IPC_TX_CH_ST |= BIT(IPC_ChNum);
#else
	(void)IPC_Dir;
	(void)IPC_ChNum;
	(void)IPC_Msg;
#endif

	return IPC_SEND_SUCCESS;
}

PIPC_MSG_STRUCT ipc_get_message(u32 IPC_Dir, u8 IPC_ChNum)
{
#ifndef CONFIG_IPC_UNSHARE_MODE
	PIPC_MSG_STRUCT IPC_MSG = (PIPC_MSG_STRUCT)__ipc_memory_start__;
	u32 msg_idx = IPC_TX_CHANNEL_NUM * IPC_TX_CHANNEL_SWITCH(IPC_Dir) + IPC_ChNum;

	DCache_Invalidate((u32)&IPC_MSG[msg_idx], sizeof(IPC_MSG_STRUCT));

	return &IPC_MSG[msg_idx];
#else
	(void)IPC_Dir;
	(void)IPC_ChNum;
#endif
	return NULL;
}


/**
  * @brief  exchange messages between AP and NP.
  * @param  IPC_Dir: Specifies core to core direction
  *          This parameter can be one of the following values:
  *		 		@arg IPC_NP_TO_AP: NP send request to AP
  *		 		@arg IPC_AP_TO_NP: AP send request to NP
  * @param  IPC_ChNum: the IPC channel number.
  * @param  IPC_Data: pointer to the message to be exchanged,and should not stored in stack.
  * @retval   None
  */
u32 ipc_send_message_unshare(u32 IPC_Dir, u8 IPC_ChNum, PIPC_DATA_STRUCT IPC_Data)
{
	/* Check the parameters */
	assert_param(IS_IPC_DIR_MODE(IPC_Dir));
	assert_param(IS_IPC_VALID_CHNUM(IPC_ChNum));

	IPC_TypeDef *IPCx = IPC_GetDev(IPC_Dir, 0);
	u32 timeout = 0xFFFF;
	u32 len, SemaId;

	u32 CPUID = SYS_CPUID();

	if (CPUID == KM4TZ_CPU_ID) {
		SemaId = IPC_SEM_IPC1;
	} else {
		SemaId = IPC_SEM_IPC0;
	}

	/*get sema*/
	if (IPC_SEMTake(SemaId, timeout) == FALSE) {
		return IPC_SEMA_TIMEOUT;
	}
	/*polling check channel busy*/
	if (IPCx->IPC_TX_CH_ST & (BIT(IPC_ChNum))) {
		if (IPC_wait_idle(IPCx, IPC_ChNum)) {
			return IPC_SEND_TIMEOUT;
		}
	}

	for (len = 0; len < IPC_Data->data_len; len++) {
		/*polling check writeable*/
		while (IPCx->IPC_TX_ST & IPC_BIT_TX0_FIFO_WFULL);
		/*write tx data 32bit*/
		IPCx->IPC_TX_DATA_CHx[IPC_ChNum] = IPC_Data->data[len];

		if (len == IPC_Data->data_len - 1) {
			/*if last data, set TX_STATUS, when channelx transfer done, hw will clear TX_STATUS to 0*/
			IPCx->IPC_TX_CH_ST |= BIT(IPC_ChNum);
		}
	}

	IPC_SEMFree(SemaId);

	return IPC_SEND_SUCCESS;
}


/**
  * @brief  get ipc message.
  * @param  IPC_Dir: Specifies core to core direction
  *          This parameter can be one of the following values:
  *		 		@arg IPC_NP_TO_AP: NP send request to AP
  *		 		@arg IPC_AP_TO_NP: AP send request to NP
  * @param  IPC_ChNum: the IPC channel number.
  * @param  maxlen: The maximum value for receiving data
  * @param  IPC_Data: pointer to the message to be exchanged
  * @retval  IPC receive status
  * @note for data message, corresponding data cache should be invalidate before access.
  */
u32 ipc_get_message_unshare(u32 IPC_Dir, u8 IPC_ChNum, PIPC_DATA_STRUCT IPC_Data)
{
	assert_param(IS_IPC_VALID_CHNUM(IPC_ChNum));
	assert_param(IS_IPC_DIR_MODE(IPC_Dir));
	assert_param(IPC_Data->data != NULL);
	IPC_TypeDef *IPCx = IPC_GetDev(IPC_Dir, TRUE);

	IPC_Data->data_len = 0;

	do {
		/*read data*/
		IPC_Data->data[IPC_Data->data_len] = IPCx->IPC_RX_DATA;
		IPC_Data->data_len++;

		/*Check if the receive FIFO is empty*/
		if (IPCx->IPC_RX_ST & IPC_BIT_RX0_FIFO_EMPTY) {
			break;
		}

		/*Get the current channel number*/
	} while (BIT(IPC_ChNum) == ((IPCx->IPC_ISR) & 0xFFFF));

	/* IPC_RX_DATA is u32 type, which is 4byte */
	DCache_CleanInvalidate((u32)IPC_Data->data, IPC_Data->data_len * 4);
	return IPC_RECEIVE_SUCCESS;
}

