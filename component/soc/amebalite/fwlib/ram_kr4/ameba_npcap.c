/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

//KR4 is NP, KR4 control KM4
static const char *const TAG = "NPCAP";
u32 KM4SleepTick = 0;
static u32 km4_sleep_type;

u32 km4_status_on(void)
{
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP0);

	if (Temp & APBPeriph_KM4_CLOCK) {
		return 1;
	} else {
		return 0;
	}
}

void km4_power_gate(void)
{
	u32 temp = 0;
	u32 timeout = 20;

	if (!km4_status_on()) {
		return;
	}

	if (HAL_READ32(PMC_BASE, SYSPMC_CTRL) & PMC_BIT_KM4_IRQ_MASK) {
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "KM4 exit WFI\n");
		return;
	}

	while (timeout > 0) {
		if (HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_STATUS) & LSYS_BIT_KM4_SLP_MODE) {	/*get KM4 sleep status*/
			break;
		}
		timeout--;
		if (timeout == 0) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "KM4 wake, no need to close KM4\n");
			return;
		}
	}

	RTK_LOGD(NOTAG, "M4PG\n");

	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_KM4_CLOCK, DISABLE);

	/*unmask km4 irqs*/
	temp = HAL_READ32(PMC_BASE, SYSPMC_CTRL);
	temp &= (~PMC_BIT_KM4_IRQ_MASK);
	HAL_WRITE32(PMC_BASE, SYSPMC_CTRL, temp);

	pmu_release_wakelock(PMU_KM4_RUN);
	pmu_release_wakelock(PMU_OS);
}

void km4_power_wake(void)
{
	if (km4_status_on()) {
		return;
	}

	RCC_PeriphClockCmd(APBPeriph_KM4, APBPeriph_KM4_CLOCK, ENABLE);

	RTK_LOGD(TAG, "M4PW\n");
}

void km4_clock_gate(void)
{
	u32 temp = 0;
	u32 timeout = 20;

	if (!km4_status_on()) {
		return;
	}

	if (HAL_READ32(PMC_BASE, SYSPMC_CTRL) & PMC_BIT_KM4_IRQ_MASK) {
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "KM4 exit WFI\n");
		return;
	}

	while (timeout > 0) {
		if (HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_STATUS) & LSYS_BIT_KM4_SLP_MODE) {	/*get KM4 sleep status*/
			break;
		}
		timeout--;
		if (timeout == 0) {
			RTK_LOGS(NOTAG, RTK_LOG_WARN, "KM4 wake, no need to close KM4\n");
			return;
		}
	}

	RTK_LOGD(TAG, "M4CG\n");

	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_KM4_CLOCK, DISABLE);

	/*unmask km4 irqs*/
	temp = HAL_READ32(PMC_BASE, SYSPMC_CTRL);
	temp &= (~PMC_BIT_KM4_IRQ_MASK);
	HAL_WRITE32(PMC_BASE, SYSPMC_CTRL, temp);

	pmu_release_wakelock(PMU_KM4_RUN);
	pmu_release_wakelock(PMU_OS);
}

void km4_clock_on(void)
{
	if (km4_status_on()) {
		return;
	}

	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_KM4_CLOCK, ENABLE);

	RTK_LOGD(TAG, "M4CW\n");
}

u32 NPWAP_INTHandler(UNUSED_WARN_DIS void *Data)
{
	UNUSED(Data);
	RTK_LOGD(TAG, "NP WAKE AP HANDLER %lx %lx\n",
			 HAL_READ32(PMC_BASE, WAK_STATUS0), HAL_READ32(PMC_BASE, WAK_STATUS1));

	InterruptDis(NP_WAKE_IRQ);

	km4_resume();
	return TRUE;
}

u32 aontimer_int_wakeup_ap(UNUSED_WARN_DIS void *Data)
{
	UNUSED(Data);

	SOCPS_AONTimerClearINT();
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "wakereason: 0x%x\n", SOCPS_AONWakeReason());
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, DISABLE);

	return TRUE;
}

int km4_suspend(u32 type)
{
	int ret = SUCCESS;
	SLEEP_ParamDef *sleep_param;
	u32 duration = 0;
	RRAM_TypeDef *rram = RRAM_DEV;

	sleep_param = (SLEEP_ParamDef *)ipc_get_message(IPC_KM4_TO_KR4, IPC_M2R_TICKLESS_INDICATION);

	if (sleep_param != NULL) {
		duration = sleep_param->sleep_time;
	}

	if (duration > 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "duration: %lu\n", duration);
		RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);
		SOCPS_AONTimer(duration);
		SOCPS_SetAPWakeEvent_MSK1(WAKE_SRC_AON_TIM, ENABLE);
		SOCPS_AONTimerINT_EN(ENABLE);
		InterruptRegister(aontimer_int_wakeup_ap, AON_TIM_IRQ, (u32)PMC_BASE, INT_PRI3);
		InterruptEn(AON_TIM_IRQ, INT_PRI3);
	}

	/*clean kr4 wake pending interrupt*/
	NVIC_ClearPendingIRQ(NP_WAKE_IRQ);

	RTK_LOGD(TAG, "register np_wake_irq_kr4\n");
	InterruptRegister(NPWAP_INTHandler, NP_WAKE_IRQ, (u32)PMC_BASE, INT_PRI3);
	InterruptEn(NP_WAKE_IRQ, INT_PRI3);

	if (type == SLEEP_CG) {
		km4_clock_gate();
	} else {
		km4_power_gate();
	}

	if (rram->PMC_CORE_ROLE_Flag == PMC_CORE_ROLE_AP2NP) {
		if (!dsp_status_on()) {
			SOCPS_AP_suspend_config(type, ENABLE);
		}
	}

	pmu_set_sleep_type(type);

	return ret;
}

void km4_resume(void)
{
	RRAM_TypeDef *rram = RRAM_DEV;

	if (km4_status_on()) {
		return;
	}

	pmu_acquire_wakelock(PMU_KM4_RUN);
	pmu_acquire_wakelock(PMU_OS);

	if (rram->PMC_CORE_ROLE_Flag == PMC_CORE_ROLE_AP2NP) {
		if (!dsp_status_on()) {
			SOCPS_AP_resume_config(km4_sleep_type, ENABLE);
		}
#if !defined(CONFIG_MP_SHRINK) && defined (CONFIG_FW_DRIVER_COEXIST) && CONFIG_FW_DRIVER_COEXIST
		//request FW leave 32k and wakeup driver, by rpwm int
		wifi_hal_system_resume_wlan();
#endif
	}

	if (km4_sleep_type == SLEEP_CG) {
		km4_clock_on();
	} else {
		km4_power_wake();
	}
}

void km4_tickless_ipc_int(UNUSED_WARN_DIS void *Data, UNUSED_WARN_DIS u32 IrqStatus, UNUSED_WARN_DIS u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);
	SLEEP_ParamDef *psleep_param;

	RTK_LOGD(TAG, "km4_tickless_ipc_int\n");

	psleep_param = (SLEEP_ParamDef *)ipc_get_message(IPC_KM4_TO_KR4, IPC_M2R_TICKLESS_INDICATION);
	DCache_Invalidate((u32)psleep_param, sizeof(SLEEP_ParamDef));

	//TODO: set dlps
	if (psleep_param->dlps_enable) {
		RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);
		if (psleep_param->sleep_time) {
			SOCPS_AONTimerClearINT();
			SOCPS_AONTimer(psleep_param->sleep_time);
			SOCPS_AONTimerINT_EN(ENABLE);
		}
		SOCPS_DeepSleep_RAM();
	}

	km4_sleep_type = psleep_param->sleep_type;

	if ((psleep_param->sleep_type == SLEEP_PG) || (psleep_param->sleep_type == SLEEP_CG)) {
		km4_suspend(psleep_param->sleep_type);
	} else {
		RTK_LOGW(TAG, "unknow sleep type\n");
	}

	KM4SleepTick = SYSTIMER_TickGet();
	RTK_LOGD(TAG, "T:%lu, tms:%lu\r\n", KM4SleepTick, (((KM4SleepTick & 0xFFFF8000) / 32768) * 1000 + ((KM4SleepTick & 0x7FFF) * 1000) / 32768));
}

/**
  * @brief  Get km4 sleep time, km4 is np.
  * @param  null.
  * @retval km4 sleep time in ms.
  */
uint32_t pmu_get_km4sleeptime(void)
{
	u32 current_tick = 0, km4sleeptime = 0;

	current_tick = SYSTIMER_TickGet();

	if (current_tick >= KM4SleepTick) {
		km4sleeptime = current_tick - KM4SleepTick;
	} else {
		km4sleeptime = (0xFFFFFFFF - KM4SleepTick) + current_tick;
	}

	km4sleeptime = ((km4sleeptime & 0xFFFF8000) / 32768) * 1000 + ((km4sleeptime & 0x7FFF) * 1000) / 32768;

	return km4sleeptime;
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_tickless_table = {
	.USER_MSG_TYPE = IPC_USER_DATA,
#ifndef CONFIG_MP_SHRINK
	.Rxfunc = km4_tickless_ipc_int,
#else
	.Rxfunc = NULL,
#endif
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_KM4_TO_KR4,
	.IPC_Channel = IPC_M2R_TICKLESS_INDICATION
};
