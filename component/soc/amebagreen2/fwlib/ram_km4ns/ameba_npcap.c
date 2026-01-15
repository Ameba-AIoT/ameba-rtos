/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "NPCAP";

u32 APSleepTick = 0;

u32 ap_status_on(void)
{
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP0);

	return (Temp & APBPeriph_CPU0_CLOCK) ? 1 : 0;

}

void ap_clock_gate(void)
{
	u32 timeout = 2000; //PMC_TODO: need further check to get a suitable value

	if (!ap_status_on()) {
		return;
	}

	while (timeout > 0) {
		if (HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_STATUS) & LSYS_BIT_KM4TZ_SLP_MODE) {	/*get AP sleep status*/
			break;
		}
		timeout--;
		if (timeout == 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "CPU0 wake, no need to close CPU0\n");
			return;
		}
	}

	RTK_LOGD(TAG, "APCG\n");

	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_CPU0_CLOCK, DISABLE);

	pmu_release_wakelock(PMU_CPU0_RUN);
	pmu_release_wakelock(PMU_OS);
}
void ap_clock_on(void)
{
	if (ap_status_on()) {
		return;
	}

	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_CPU0_CLOCK, ENABLE);

	RTK_LOGD(TAG, "NPCW\n");

	/* tell AP wake */
	asm volatile("sev");

}

u32 NPWAP_INTHandler(UNUSED_WARN_DIS void *Data)
{
	UNUSED(Data);
	RTK_LOGD(TAG, "CPU1 WAKE CPU0 HANDLER %08x %08x\n",
			 HAL_READ32(PMC_BASE, WAK_STATUS0), HAL_READ32(PMC_BASE, WAK_STATUS1));

	InterruptDis(AP_WAKE_IRQ);

	ap_resume();
	return TRUE;
}

int ap_suspend(void)
{

	int ret = RTK_SUCCESS;

	if (HAL_READ32(PMC_BASE, WAK_MASK0_AP) || HAL_READ32(PMC_BASE, WAK_MASK1_AP)) {
		RTK_LOGD(TAG, "register ap_wake_irq\n");
		/*clean NP wake pending interrupt*/
		NVIC_ClearPendingIRQ(AP_WAKE_IRQ);
		InterruptRegister(NPWAP_INTHandler, AP_WAKE_IRQ, (u32)PMC_BASE, INT_PRI_LOWEST);
		InterruptEn(AP_WAKE_IRQ, INT_PRI_LOWEST);
	}

	/*AP function reset will be off by PMC if enter PG*/
	ap_clock_gate();

	return ret;
}

void ap_resume(void)
{
	if (ap_status_on()) {
		return;
	}

	pmu_acquire_wakelock(PMU_CPU0_RUN);

#if !defined(CONFIG_MP_SHRINK) && defined (CONFIG_FW_DRIVER_COEXIST) && CONFIG_FW_DRIVER_COEXIST
	extern void wifi_hal_system_resume_wlan(void);
	//request FW leave 32k and wakeup driver, by rpwm int
	wifi_hal_system_resume_wlan();
#endif

	/*AP rst will be released by PMC when PG wake, thus NP only need enable AP clk*/
	ap_clock_on();
}

void ap_tickless_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);
	PIPC_MSG_STRUCT psleep_msg;
	SLEEP_ParamDef *psleep_param;

	RTK_LOGD(TAG, "ap_tickless_ipc_int\n");

	psleep_msg = ipc_get_message(IPC_AP_TO_NP, IPC_A2N_TICKLESS_INDICATION);
	psleep_param = (SLEEP_ParamDef *)psleep_msg->msg;
	DCache_Invalidate((u32)psleep_param, sizeof(SLEEP_ParamDef));

	if (psleep_param->dlps_enable) {
		RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);
		if (psleep_param->sleep_time) {
			AONTimer_ClearINT();
			AONTimer_Setting(psleep_param->sleep_time);
			AONTimer_INT(ENABLE);
		}
		SOCPS_DeepSleep_RAM();
	}

	/* set np sleep type same with AP*/
	pmu_set_sleep_type(psleep_param->sleep_type);

	switch (psleep_param->sleep_type) {
	case SLEEP_PG:
	case SLEEP_CG:
		/* AP & NP share platform, AP enter CG first*/
		if (RTK_SUCCESS == ap_suspend()) {
			//pmu_set_sysactive_time(2);
		}
		break;

	default:
		RTK_LOGW(TAG, "unknow sleep type\n");
	}
	APSleepTick = SYSTIMER_TickGet();
	RTK_LOGD(TAG, "T:%lu, tms:%lu\r\n", APSleepTick, (((APSleepTick & 0xFFFF8000) / 32768) * 1000 + ((APSleepTick & 0x7FFF) * 1000) / 32768));
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_tickless_table = {
	.USER_MSG_TYPE = IPC_USER_DATA,
	.Rxfunc = ap_tickless_ipc_int,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_AP_TO_NP,
	.IPC_Channel = IPC_A2N_TICKLESS_INDICATION
};
