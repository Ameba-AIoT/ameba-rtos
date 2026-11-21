/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "FreeRTOS.h"
#include "ameba_plic.h"

//static const char *const TAG = "PMC";
extern void vPortBackupPmp(BaseType_t *pxPmpBaseAddr);
extern void vPortRestorePmp(BaseType_t *pxPmpBaseAddr);

#if (portUSING_MPU_WRAPPERS == 1)
struct ctxPMPBackup {
	BaseType_t xPmpAddrs[ configMAX_PMPADDR_NUMBER ];
	BaseType_t xPmpCfgs[ configMAX_PMPCFG_NUMBER ];
};

struct ctxPMPBackup ctxPMP;
#endif

void SOCPS_vPortBackupPmp(void)
{
#if (portUSING_MPU_WRAPPERS == 1)
	vPortBackupPmp((BaseType_t *) &ctxPMP);
#endif
}

void SOCPS_vPortRestorePmp(void)
{
#if (portUSING_MPU_WRAPPERS == 1)
	vPortRestorePmp((BaseType_t *) &ctxPMP);
#endif
}

#if defined(CONFIG_FLASH_AVL)
#define KR4_FREEZE_PARKED	BIT9	/* BKUP_REG1: KR4 is parked in SRAM */
#define KR4_FREEZE_RELEASE	BIT10	/* BKUP_REG1: KM4 lets KR4 go */

/* KM4 changes the flash read mode / clock / QE and the SPIC may not serve XIP meanwhile, so no code or data of KR4
 * may come from flash: this is SRAM-resident, and only the backup register is touched. */
SRAMDRAM_ONLY_TEXT_SECTION __NO_INLINE
static void SOCPS_KR4_Freeze(void)
{
	u32 irq = irq_disable_save();

	BKUP_Set(BKUP_REG1, KR4_FREEZE_PARKED);
	while (!(BKUP_Read(BKUP_REG1) & KR4_FREEZE_RELEASE));
	BKUP_Clear(BKUP_REG1, KR4_FREEZE_PARKED);

	irq_enable_restore(irq);
}
#endif

/**
  * @brief  KM4 wake KR4 by IPC
  * @param  None
  * @note  ipc_msg_temp.msg represents who wakes up KR4
  		* ipc_msg_temp.msg = 0: FW wakeup KR4
  		* ipc_msg_temp.msg = FREEZE_NPWAP_IPC: park KR4 in SRAM until KM4 releases it (flash AVL only)
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

#if defined(CONFIG_FLASH_AVL)
	if (type == FREEZE_NPWAP_IPC) {
		SOCPS_KR4_Freeze();
		return;
	}
#endif

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

