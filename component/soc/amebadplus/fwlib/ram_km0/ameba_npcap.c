/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "NPCAP";
#if defined (CONFIG_FW_DRIVER_COEXIST) && CONFIG_FW_DRIVER_COEXIST
extern void wifi_hal_system_resume_wlan(void);
#endif
//KM0 is NP, KM0 control KM4

u32 km4_status_on(void)
{
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP0);

	if (Temp & APBPeriph_KM4_CLOCK) {
		return 1;
	} else {
		return 0;
	}
}

void km4_clock_gate(void)
{
	u32 timeout = 2000; //PMC_TODO: need further check to get a suitable value

	if (!km4_status_on()) {
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

	RTK_LOGD(TAG, "M4CG");

	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_KM4_CLOCK, DISABLE);

	SOCPS_AP_suspend_config(ENABLE);

	pmu_release_wakelock(PMU_KM4_RUN);
	pmu_release_wakelock(PMU_OS);
}

void km4_clock_on(void)
{
	if (km4_status_on()) {
		return;
	}

	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_KM4_CLOCK, ENABLE);

	RTK_LOGD(TAG, "M4CW");

	/* tell KM4 wake */
	asm volatile("sev");
}

u32 NPWAP_INTHandler(UNUSED_WARN_DIS void *Data)
{
	UNUSED(Data);

	RTK_LOGD(NOTAG, "NP WAKE AP HANDLER %08x \n",
			 HAL_READ32(PMC_BASE, WAK_STATUS0));
	InterruptDis(KM4_WAKE_IRQ);

	km4_resume();
	return TRUE;
}

u32 aontimer_int_wakeup_np(UNUSED_WARN_DIS void *Data)
{
	UNUSED(Data);

	AONTimer_ClearINT();
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "wakereason: 0x%x\n", SOCPS_AONWakeReason());
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, DISABLE);

	IPC_MSG_STRUCT ipc_msg_temp;
	ipc_msg_temp.msg_type = IPC_USER_POINT;
	ipc_msg_temp.msg = TIMER_NPWAP_IPC;
	ipc_msg_temp.msg_len = 1;
	ipc_msg_temp.rsvd = 0;
	ipc_send_message(IPC_KM0_TO_KM4, IPC_N2A_WAKE_AP, &ipc_msg_temp);

	return TRUE;
}


int km4_suspend(u32 type)
{
	UNUSED(type);

	int ret = RTK_SUCCESS;

	if (HAL_READ32(PMC_BASE, WAK_MASK0_AP)) {
		RTK_LOGD(TAG, "register KM4_wake_irq\n");
		/*clean km0 wake pending interrupt*/
		NVIC_ClearPendingIRQ(KM4_WAKE_IRQ);
		InterruptRegister(NPWAP_INTHandler, KM4_WAKE_IRQ, (u32)PMC_BASE, INT_PRI_LOWEST);
		InterruptEn(KM4_WAKE_IRQ, INT_PRI_LOWEST);
	}

	/*KM4 function reset will be off by PMC if enter PG*/
	km4_clock_gate();

	return ret;
}

void km4_resume(void)
{
	if (km4_status_on()) {
		return;
	}
	pmu_acquire_wakelock(PMU_KM4_RUN);

	SOCPS_AP_resume_config(ENABLE);

#if !defined(CONFIG_MP_SHRINK) && defined (CONFIG_FW_DRIVER_COEXIST) && CONFIG_FW_DRIVER_COEXIST
	extern void wifi_hal_system_resume_wlan(void);
	//request FW leave 32k and wakeup driver, by rpwm int
	wifi_hal_system_resume_wlan();
#endif

	/*KM4 rst will be released by PMC when PG wake, thus KM0 only need enable KM4 clk*/
	km4_clock_on();
}

void km4_tickless_ipc_int(UNUSED_WARN_DIS void *Data, UNUSED_WARN_DIS u32 IrqStatus, UNUSED_WARN_DIS u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);
	PIPC_MSG_STRUCT psleep_msg;
	SLEEP_ParamDef *psleep_param;

	RTK_LOGD(TAG, "km4_tickless_ipc_int\n");

	psleep_msg = ipc_get_message(IPC_KM4_TO_KM0, IPC_A2N_TICKLESS_INDICATION);
	psleep_param = (SLEEP_ParamDef *)psleep_msg->msg;
	DCache_Invalidate((u32)psleep_param, sizeof(SLEEP_ParamDef));

	//TODO: set dlps
	if (psleep_param->dlps_enable) {
		RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);
		if (psleep_param->sleep_time) {
			AONTimer_ClearINT();
			AONTimer_Setting(psleep_param->sleep_time);
			AONTimer_INT(ENABLE);
		}
		SOCPS_DeepSleep_RAM();
	}

	/* set KM0 sleep type same with KM4*/
	pmu_set_sleep_type(psleep_param->sleep_type);

	switch (psleep_param->sleep_type) {
	case SLEEP_PG:
	case SLEEP_CG:
		/* KM4 & KM0 share platform, KM4 enter CG first*/
		if (RTK_SUCCESS == km4_suspend(SLEEP_CG)) {
			//pmu_set_sysactive_time(2);
		}
		break;

	default:
		RTK_LOGW(TAG, "unknow sleep type\n");
	}

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
	.IPC_Direction = IPC_KM4_TO_KM0,
	.IPC_Channel = IPC_A2N_TICKLESS_INDICATION
};
