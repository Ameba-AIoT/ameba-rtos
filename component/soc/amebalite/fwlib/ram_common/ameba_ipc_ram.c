/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "IPC";
IPC_IRQ_FUN IPC_IrqHandler[IPC_CHANNEL_NUM];
void *IPC_IrqData[IPC_CHANNEL_NUM];

static u8 PXID_Idx[4] = {1, 1, 1, 1};

void (*ipc_delay)(uint32_t);
void (*ipc_enter)(u32);
void (*ipc_exit)(u32);

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup IPC
  * @brief  IPC driver modules
  * @{
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup IPC_Exported_Functions IPC Exported Functions
  * @{
  */

/**
  * @brief  Enables or disables the specified IPC Channel interrupts.
  * @param  where IPCx can be IPCKR4_DEV for KR4, IPCKM4_DEV for CM4, IPCDSP_DEV for DSP.
  * @param  IPC_Shiftbit: 0 ~ 31.
  * @param  NewState: DISABLE/ENABLE.
  * @retval   None
  */
void IPC_INTConfig(IPC_TypeDef *IPCx, u8 IPC_Shiftbit, u32 NewState)
{
	if (ENABLE == NewState) {
		IPCx->IPC_IMR |= (BIT(IPC_Shiftbit));
	} else {
		IPCx->IPC_IMR &= ~(BIT(IPC_Shiftbit));
	}
}

/**
  * @brief  set IMR of specified IPC Channel interrupts.
  * @param  where IPCx can be IPCKR4_DEV for KR4, IPCKM4_DEV for CM4, IPCDSP_DEV for DSP.
  * @param  IPC_Chs: the Channels that want to be enable.
  * @retval   None
  */
void IPC_IERSet(IPC_TypeDef *IPCx, u32 IPC_Chs)
{
	IPCx->IPC_IMR = IPC_Chs;
}

/**
  * @brief  get IMR of specified IPC Channel interrupts.
  * @param  IPCx: where IPCx can be IPCKR4_DEV for KR4, IPCKM4_DEV for CM4, IPCDSP_DEV for DSP.
  * @retval  The IMR status of specified IPC
  */
u32 IPC_IERGet(IPC_TypeDef *IPCx)
{
	return IPCx->IPC_IMR;
}

/**
  * @brief  Request a core-to-core interrupt.
  * @param  IPC_Dir: Specifies core to core direction
  *          This parameter can be one of the following values:
  *		 		@arg IPC_KR4_TO_KM4: KR4 send request to KM4
  *		 		@arg IPC_KR4_TO_DSP: K4R send request to DSP
  *		 		@arg IPC_KM4_TO_KR4: KM4 send request to KR4
  *		 		@arg IPC_KM4_TO_DSP: KM4 send request to DSP
  *		 		@arg IPC_DSP_TO_KR4: DSP send request to KR4
  *		 		@arg IPC_DSP_TO_KM4: DSP send request to KM4
  * @param  IPC_ChNum: 0 ~ 7.
  * @retval   0/1
  */
u32 IPC_INTRequest(IPC_TypeDef *IPCx, u32 IPC_Dir, u8 IPC_ChNum)
{
	/* Check the parameters */
	assert_param(IS_IPC_DIR_MODE(IPC_Dir));
	u32 ipc_shift = 0;

	switch (IPC_Dir) {
	case IPC_KR4_TO_KM4:
	case IPC_KM4_TO_KR4:
	case IPC_DSP_TO_KR4:
		ipc_shift = 8;
		break;

	default:
		break;
	}

	if (IPCx->IPC_TX_DATA & (BIT(IPC_ChNum + ipc_shift))) {
		return 0;
	} else {
		IPCx->IPC_TX_DATA = (BIT(IPC_ChNum + ipc_shift));
		return 1;
	}
}

/**
  * @brief  Get core-to-core interrupt status.
  * @param  where IPCx can be IPCKR4_DEV for KR4, IPCKM4_DEV for CM4, IPCDSP_DEV for DSP.
  * @retval  tx_empty or rx_full interrupt status
  */
u32 IPC_INTGet(IPC_TypeDef *IPCx)
{
	return IPCx->IPC_ISR;
}

/**
  * @brief  Clear a core-to-core interrupt.
  * @param  where IPCx can be IPCKR4_DEV for KR4, IPCKM4_DEV for CM4, IPCDSP_DEV for DSP.
  * @param  IPC_Shiftbit: 0 ~ 31.
  * @retval   None
  */
void IPC_INTClear(IPC_TypeDef *IPCx, u8 IPC_Shiftbit)
{
	IPCx->IPC_ISR = BIT(IPC_Shiftbit);
}

/**
  * @brief  The common IPC interrupt handler
  * @param  Data: the data pointer to IPCx
  * @retval 0
  */
u32 IPC_INTHandler(void *Data)
{
	IPC_TypeDef *IPCx = (IPC_TypeDef *)Data;
	u32 IrqPending;
	u32 i;
	IrqPending = IPCx->IPC_ISR & IPCx->IPC_IMR;

	for (i = 0; i < 32; i++) {
		if (IrqPending & BIT(i)) {
			if (IPC_IrqHandler[i] != NULL) {
				IPC_IrqHandler[i](IPC_IrqData[i], IrqPending, i);
				IPCx->IPC_ISR = BIT(i);
			} else {
				IPCx->IPC_ISR = BIT(i);
			}
		}
	}

	return 0;
}

/**
  * @brief  To register a user interrupt handler for a specified IPC channel
  * @param  where IPCx can be IPCKR4_DEV for KR4, IPCKM4_DEV for CM4, IPCDSP_DEV for DSP.
  * @param  IPC_Shiftbit: 0 ~ 31.
  * @param  IrqHandler: The IRQ handler to be assigned to the specified IPC channel
  * @param  IrqData: The pointer will be pass the the IRQ handler
  * @retval None
  */
void IPC_INTUserHandler(IPC_TypeDef *IPCx, u8 IPC_Shiftbit, void *IrqHandler, void *IrqData)
{
	/* Check the parameters */
	if (IrqHandler == NULL) {
		return;
	}

	IPC_IrqHandler[IPC_Shiftbit] = (IPC_IRQ_FUN)IrqHandler;
	IPC_IrqData[IPC_Shiftbit] = IrqData;

	if (IS_IPC_RX_CHNUM(IPC_Shiftbit)) {
		IPC_INTConfig(IPCx, IPC_Shiftbit, ENABLE);
	}
}

/**
  * @brief  Set delay function for ipc sema.
  * @param  pfunc: delay function.
  * @retval   None
  */
void IPC_patch_function(void (*pfunc1)(u32), void (*pfunc2)(u32))
{
	ipc_enter = pfunc1;
	ipc_exit = pfunc2;
}
/**
  * @brief  Get core-to-core hardware semphone.
  * @param  SEM_Idx: 0~4.
  * @param  timeout: timeout to wait. 0 means never wait, 0xffffffff means waiting permanently.
  * @retval   TRUE/FALSE
  * @note  PXID_idx must not be 0
  */
u32 IPC_SEMTake(IPC_SEM_IDX SEM_Idx, u32 timeout)
{
	u32 CPUID_idx;
	u32 PXID_idx;
	u32 CPUID = SYS_CPUID();
	uint32_t IrqStatus;

	/* Check the parameters */
	assert_param(IS_IPC_VALID_PXID(PXID_Idx[SEM_Idx]));

	if (ipc_enter) {
		ipc_enter(RTOS_CRITICAL_SEMA);
	}

	do {
		CPUID_idx = IPC_GET_SEMx_CPUID(IPCKR4_DEV->IPC_SEMx[SEM_Idx]);

		if (CPUID_idx == 0x0) {
			break;
		} else {
			if (timeout == 0) {
				goto fail;
			}

			if (ipc_delay) {
				ipc_delay(1);
			}
		}
		timeout--;
	} while (1);

	IrqStatus = irq_disable_save();
	/* Set SET_CPUID and PXID, cpu ID will set automatically*/
	IPCKR4_DEV->IPC_SEMx[SEM_Idx] |= (IPC_SEMx_PXID(PXID_Idx[SEM_Idx]) | IPC_BIT_SEMx_SET_CPUID);

	CPUID_idx = IPC_GET_SEMx_CPUID(IPCKR4_DEV->IPC_SEMx[SEM_Idx]);
	PXID_idx = IPC_GET_SEMx_PXID(IPCKR4_DEV->IPC_SEMx[SEM_Idx]);

	/* check if cpu get this sema */
	if ((CPUID_idx != CPUID + 1) || (PXID_idx != PXID_Idx[SEM_Idx])) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "CPU %d take sema %d for pxid %d fail: cpuid %d pxid %d has taken the sema\n", CPUID, SEM_Idx, PXID_Idx[SEM_Idx], CPUID_idx - 1,
				 PXID_idx);

		irq_enable_restore(IrqStatus);
		goto fail;
	}

	if (PXID_Idx[SEM_Idx] == IPC_MAX_VALID_PXID) {
		PXID_Idx[SEM_Idx] = 1;
	} else {
		PXID_Idx[SEM_Idx]++;
	}

	irq_enable_restore(IrqStatus);
	return TRUE;

fail:
	if (ipc_exit) {
		ipc_exit(RTOS_CRITICAL_SEMA);
	}
	return FALSE;
}

/**
  * @brief  Free core-to-core hardware semphone.
  * @param  SEM_Idx: 0~4.
  * @retval   TRUE/FALSE
  * @note  PXID_idx must not be 0
  */
u32 IPC_SEMFree(IPC_SEM_IDX SEM_Idx)
{
	u32 CPUID_idx;
	u32 PXID_idx;
	u32 CPUID = SYS_CPUID();

	CPUID_idx = IPC_GET_SEMx_CPUID(IPCKR4_DEV->IPC_SEMx[SEM_Idx]);
	PXID_idx = IPC_GET_SEMx_PXID(IPCKR4_DEV->IPC_SEMx[SEM_Idx]);

	if (CPUID_idx != (CPUID + 1)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "CPU %d free sema %d for pxid %d fail: cpuid %d, pxid %d is taking the sema\n", CPUID, SEM_Idx, PXID_Idx[SEM_Idx], CPUID_idx - 1,
				 PXID_idx);
		return FALSE;
	}

	/* Set CLR to free it, cpu ID and PXID will clear automatically */
	IPCKR4_DEV->IPC_SEMx[SEM_Idx] |= IPC_BIT_SEMx_CLR;

	if (ipc_exit) {
		ipc_exit(RTOS_CRITICAL_SEMA);
	}

	return TRUE;
}

/**
  * @brief  Set delay stub function for ipc sema.
  * @param  pfunc: delay function.
  * @retval   None
  */
void IPC_SEMDelayStub(void (*pfunc)(uint32_t))
{
	ipc_delay = pfunc;
}


/**
  * @brief  Get IPC device.
  * @param  cpuid:
  *			0: CPU_KR4
  *		 	1: CPU_KM4
  *			2: CPU_DSP
  * @retval IPC device
  */
IPC_TypeDef *IPC_GetDevById(u32 cpu_id)
{
	/* Check the parameters */
	assert_param(IS_IPC_Valid_CPUID(cpu_id));

	if (cpu_id == 1) {
		return IPCKM4_DEV;
	} else if (cpu_id == 2) {
		return IPCDSP_DEV;
	} else {
		return IPCKR4_DEV;

	}
}

/** @} */

/** @} */
/** @} */