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

void (*ipc_delay_func)(uint32_t);
void (*ipc_enter_func)(u32);
void (*ipc_exit_func)(u32);

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup IPC IPC
  * @brief  IPC driver modules
  * @{
  */

/**
  * @brief  Get core-to-core hardware semaphore.
  * @param  SEM_Idx 0~4.
  * @param  timeout Timeout to wait. 0 means never wait, 0xffffffff means waiting permanently.
  * @return The semaphore operation result:
  *         - TRUE: success
  *         - FALSE: failure
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

	if (ipc_enter_func) {
		ipc_enter_func(RTOS_CRITICAL_SEMA);
	}

	do {
		CPUID_idx = IPC_GET_SEMx_CPUID(IPCKR4_DEV->IPC_SEMx[SEM_Idx]);

		if (CPUID_idx == 0x0) {
			break;
		} else {
			if (timeout == 0) {
				goto fail;
			}

			if (ipc_delay_func) {
				ipc_delay_func(1);
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
	if (ipc_exit_func) {
		ipc_exit_func(RTOS_CRITICAL_SEMA);
	}
	return FALSE;
}

/**
  * @brief  Free core-to-core hardware semaphore.
  * @param  SEM_Idx 0~4.
  * @return The semaphore operation result:
  *         - TRUE: success
  *         - FALSE: failure
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

	if (ipc_exit_func) {
		ipc_exit_func(RTOS_CRITICAL_SEMA);
	}

	return TRUE;
}

/**
  * @brief  Get IPC device.
  * @param  cpu_id CPU ID. This parameter can be one of the following values:
  *            @arg 0: CPU_KR4
  *            @arg 1: CPU_KM4
  *            @arg 2: CPU_DSP
  * @return IPC device
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

/* Exported functions --------------------------------------------------------*/
/** @defgroup IPC_Exported_Functions IPC Exported Functions
  * @{
  */

/**
  * @brief  Enables or disables the specified IPC Channel interrupts.
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @param  IPC_Shiftbit 0 ~ 31.
  * @param  NewState DISABLE/ENABLE.
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
  * @brief  Set IMR of specified IPC Channel interrupts.
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @param  IPC_Chs The Channels that want to be enable.
  */
void IPC_IERSet(IPC_TypeDef *IPCx, u32 IPC_Chs)
{
	IPCx->IPC_IMR = IPC_Chs;
}

/**
  * @brief  Get IMR of specified IPC Channel interrupts.
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @return The IMR status of specified IPC
  */
u32 IPC_IERGet(IPC_TypeDef *IPCx)
{
	return IPCx->IPC_IMR;
}

/**
  * @brief  Request a core-to-core interrupt.
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @param  IPC_Dir Specifies core to core direction. This parameter can be a value of @ref IPC_Direction_Mode.
  * @param  IPC_ChNum 0 ~ 7.
  * @return The interrupt request result:
  *         - 1: the interrupt request is sent
  *         - 0: the channel is busy
  */
u32 IPC_INTRequest(IPC_TypeDef *IPCx, IPC_Direction_Mode IPC_Dir, u8 IPC_ChNum)
{
	/* Check the parameters */
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
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @return Tx_empty or rx_full interrupt status
  */
u32 IPC_INTGet(IPC_TypeDef *IPCx)
{
	return IPCx->IPC_ISR;
}

/**
  * @brief  Clear a core-to-core interrupt.
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @param  IPC_Shiftbit 0 ~ 31.
  */
void IPC_INTClear(IPC_TypeDef *IPCx, u8 IPC_Shiftbit)
{
	IPCx->IPC_ISR = BIT(IPC_Shiftbit);
}

/**
  * @brief  The common IPC interrupt handler
  * @param  Data The data pointer to IPCx
  * @return 0
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
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @param  IPC_Shiftbit 0 ~ 31.
  * @param  IrqHandler The IRQ handler to be assigned to the specified IPC channel
  * @param  IrqData The pointer will be passed to the IRQ handler
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
  * @param  pfunc1 Enter critical function.
  * @param  pfunc2 Exit critical function.
  */
void IPC_patch_function(void (*pfunc1)(u32), void (*pfunc2)(u32))
{
	ipc_enter_func = pfunc1;
	ipc_exit_func = pfunc2;
}

/**
  * @brief  Set delay stub function for ipc sema.
  * @param  pfunc Delay function.
  */
void IPC_SEMDelayStub(void (*pfunc)(uint32_t))
{
	ipc_delay_func = pfunc;
}

/** @} */
/** @} */
/** @} */
