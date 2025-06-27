/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SOCPS_H_
#define _AMEBA_SOCPS_H_


enum wakeup_mask {
	WAKEUP_NULL	= 0,
	WAKEUP_NP	= 1,
	WAKEUP_AP	= 2,
	WAKEUP_DSP	= 3,
};

enum xtal_mode_sleep {
	XTAL_OFF = 0,
	XTAL_LPS = 1,
	XTAL_Normal = 2,
};


typedef struct {
	u32 Module;
	enum wakeup_mask wakeup;
} WakeEvent_TypeDef;

typedef struct {
	u32 Module;
	u32 MEM_SD;
	u32 MEM_DS;
	u32 MEM_LS;
} MEMMode_TypeDef;

extern void SOCPS_SleepPG(void);
extern void SOCPS_SleepCG(void);

extern void SOCPS_SleepInit(void);
extern void SOCPS_AP_suspend_config(u32 type, u32 Protection);
extern void SOCPS_AP_resume_config(u32 type, u32 Protection);
extern void SOCPS_NP_suspend(u32 type);
extern void SOCPS_NP_resume(u32 type);
extern void SOCPS_PSRAM_ClkSet(u8 Source);
extern void SOCPS_Voltage_Switch(u8 sleep);


extern int SOCPS_AONWakeReason(void);
extern void SOCPS_AONTimer(u32 SDuration);
extern void SOCPS_AONTimerINT_EN(u32 Status);
extern u32 SOCPS_AONTimerGet(void);
extern void SOCPS_AONTimerClearINT(void);

extern void SOCPS_SetNPWakeEvent_MSK0(u32 Option, u32 NewStatus);
extern void SOCPS_SetNPWakeEvent_MSK1(u32 Option, u32 NewStatus);
extern void SOCPS_SetNPWakeEvent(u32 Option, u32 Group, u32 NewStatus);
extern void SOCPS_SetAPWakeEvent_MSK0(u32 Option, u32 NewStatus);
extern void SOCPS_SetAPWakeEvent_MSK1(u32 Option, u32 NewStatus);
extern void SOCPS_SetAPWakeEvent(u32 Option, u32 Group, u32 NewStatus);
extern void SOCPS_SetDSPWakeEvent_MSK0(u32 Option, u32 NewStatus);
extern void SOCPS_SetDSPWakeEvent_MSK1(u32 Option, u32 NewStatus);
extern void SOCPS_SetDSPWakeEvent(u32 Option, u32 Group, u32 NewStatus);
extern void SOCPS_DeepSleep_RAM(void);
extern void SOCPS_Peripheral_Clk_Sleep(u8 state);
extern void SOCPS_UartRxPinWakeSet(u32 status);
#endif  //_AMEBA_SOCPS_H_
