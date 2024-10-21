/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *TAG = "NPCDSP";
u32 DSPSleepTick = 0;
static u32 dsp_sleep_type;


u32 dsp_status_on(void)
{
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP1);

	if (Temp & APBPeriph_HIFI_CLOCK) {
		return 1;
	} else {
		return 0;
	}
}

void dsp_power_off(void)
{
	if (!dsp_status_on()) {
		return;
	}

	/* poll DSP sleep status */
	u32 temp = 0;
	while (1) {
		temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_HIFI_STATUS);	/*get DSP sleep status*/
		if (temp & LSYS_BIT_HIFI_WAIT_MODE) {
			break;
		}
	}

	RTK_LOGD(TAG, "DSPPG\n");

	RCC_PeriphClockCmd(APBPeriph_HIFI, APBPeriph_HIFI_CLOCK, DISABLE);

	pmu_release_wakelock(PMU_DSP_RUN);
	pmu_release_wakelock(PMU_OS);
}

void dsp_power_on(void)
{
	if (dsp_status_on()) {
		return;
	}

	RCC_PeriphClockCmd(APBPeriph_HIFI, APBPeriph_HIFI_CLOCK, ENABLE);

	RTK_LOGD(TAG, "DSPPW\n");
}

void dsp_clock_gate(void)
{
	if (!dsp_status_on()) {
		return;
	}

	/* poll DSP sleep status */
	u32 temp = 0;

	while (1) {
		temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_HIFI_STATUS);	/*get DSP sleep status*/
		if (temp & LSYS_BIT_HIFI_WAIT_MODE) {
			break;
		}
	}

	RTK_LOGD(TAG, "DSPCG\n");

	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_HIFI_CLOCK, DISABLE);

	pmu_release_wakelock(PMU_DSP_RUN);
	pmu_release_wakelock(PMU_OS);
}


void dsp_clock_on(void)
{
	if (dsp_status_on()) {
		return;
	}

	/* enable DSP clock */
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_HIFI_CLOCK, ENABLE);

	RTK_LOGD(TAG, "DSPCW\n");
}

void dsp_resume(void)
{
	if (dsp_status_on()) {
		return;
	}

	pmu_acquire_wakelock(PMU_DSP_RUN);

	if (RRAM_DEV->PMC_CORE_ROLE_Flag == PMC_CORE_ROLE_AP2NP) {
		if (!kr4_status_on()) {
			SOCPS_AP_resume_config(dsp_sleep_type, ENABLE);
		}
	}

	if (dsp_sleep_type == SLEEP_PG) {
		/* check AP state, AP need be active when DSP PG Wakeup*/
		if (!kr4_status_on()) {
			RTK_LOGS(NOTAG, "wake KR4\n");
			kr4_resume();
		}

		/* Wait until KR4 start table bacukp done */
		while (RRAM_DEV->kr4_BackupTbl_Done == 0) {
			DelayUs(1);
		}

		dsp_power_on();
	} else {
		dsp_clock_on();
	}
}

u32 NPWDSP_INTHandler(UNUSED_WARN_DIS void *Data)
{
	UNUSED(Data);
	RTK_LOGD(TAG, "NP WAKE DSP HANDLER %lx %lx\n",
			 HAL_READ32(PMC_BASE, WAK_STATUS0), HAL_READ32(PMC_BASE, WAK_STATUS1));

	InterruptDis(DSP_WAKE_IRQ);
	dsp_resume();
	return TRUE;
}


u32 dsp_suspend(u32 type)
{
	UNUSED(type);

	u32 ret = _SUCCESS;
	u32 sleep_wevent_config_val[2] = {0};
	SLEEP_ParamDef *sleep_param;
	u32 duration = 0;

	sleep_param = (SLEEP_ParamDef *)ipc_get_message(IPC_DSP_TO_KM4, IPC_D2M_TICKLESS_INDICATION);

	if (sleep_param != NULL) {
		duration = sleep_param->sleep_time;
	}

	if (duration > 0) {
		/* used for resume delay */
	}

	/*clean dsp wake pending interrupt*/
	NVIC_ClearPendingIRQ(DSP_WAKE_IRQ);

	sleep_wevent_config_val[1] = HAL_READ32(PMC_BASE, WAK_MASK1_DSP);
	sleep_wevent_config_val[0] = HAL_READ32(PMC_BASE, WAK_MASK0_DSP);

	RTK_LOGS(NOTAG, "dsp_suspend %lx %lx\n", sleep_wevent_config_val[0], sleep_wevent_config_val[1]);
	if ((sleep_wevent_config_val[0] | sleep_wevent_config_val[1])) {
		InterruptRegister(NPWDSP_INTHandler, DSP_WAKE_IRQ, (u32)PMC_BASE, INT_PRI5);
		InterruptEn(DSP_WAKE_IRQ, INT_PRI5);
	}

	if (type == SLEEP_CG) {
		dsp_clock_gate();
	} else {
		dsp_power_off();
	}

	if (RRAM_DEV->PMC_CORE_ROLE_Flag == PMC_CORE_ROLE_AP2NP) {
		if (!kr4_status_on()) {
			SOCPS_AP_suspend_config(type, ENABLE);
		}
	}

	return ret;

}


void dsp_tickless_ipc_int(UNUSED_WARN_DIS void *Data, UNUSED_WARN_DIS u32 IrqStatus, UNUSED_WARN_DIS u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);
	SLEEP_ParamDef *psleep_param;

	RTK_LOGD(TAG, "dsp_tickless_ipc_int\n");

	psleep_param = (SLEEP_ParamDef *)ipc_get_message(IPC_DSP_TO_KM4, IPC_D2M_TICKLESS_INDICATION);
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

	dsp_sleep_type = psleep_param->sleep_type;

	switch (psleep_param->sleep_type) {
	case SLEEP_PG:
		if (_SUCCESS == dsp_suspend(SLEEP_PG)) {
		}
		break;
	case SLEEP_CG:
		if (_SUCCESS == dsp_suspend(SLEEP_CG)) {
			pmu_set_sysactive_time(2);
		}
		break;
	default:
		RTK_LOGW(TAG, "unknow sleep type\n");
	}

	DSPSleepTick = SYSTIMER_TickGet();
	RTK_LOGD(TAG, "T:%lu, tms:%lu\r\n", DSPSleepTick, (((DSPSleepTick & 0xFFFF8000) / 32768) * 1000 + ((DSPSleepTick & 0x7FFF) * 1000) / 32768));
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_km4cdsp_table = {
	.USER_MSG_TYPE = IPC_USER_DATA,
#ifndef CONFIG_MP_SHRINK
	.Rxfunc = dsp_tickless_ipc_int,
#else
	.Rxfunc = NULL,
#endif
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_DSP_TO_KM4,
	.IPC_Channel = IPC_D2M_TICKLESS_INDICATION
};
