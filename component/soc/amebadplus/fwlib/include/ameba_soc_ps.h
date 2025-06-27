/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SOCPS_H_
#define _AMEBA_SOCPS_H_

enum wakeup_mask {
	WAKEUP_NULL	= 0,
	WAKEUP_KM0	= 1,
	WAKEUP_KM4	= 2,
};

enum xtal_mode_sleep {
	XTAL_OFF = 0,
	XTAL_LPS_Without_40M = 1,
	XTAL_LPS_With_40M = 2,
	XTAL_Normal = 3,
	XTAL_HP = 4,
};

typedef struct {
	u32 Module;
	enum wakeup_mask Wakeup;
} WakeEvent_TypeDef;


typedef struct {
	u8 keep_OSC4M_on;			/* keep OSC4M on or off for sleep and dslp*/
	u8 xtal_mode_in_sleep;	/* see enum xtal_mode_sleep for detail */
	u8 sleep_to_08V;			/* default sleep to 0.7V, set this option to TRUE will sleep to 0.8V */
} PSCFG_TypeDef;

extern void SOCPS_SetNPWakeEvent(u32 Option, u32 NewStatus);
extern void SOCPS_SetAPWakeEvent(u32 Option, u32 NewStatus);

extern int SOCPS_AONWakeReason(void);

/* AON timer related */
extern void SOCPS_AONTimer(u32 SDuration);
extern u32 SOCPS_AONTimerGet(void);
extern void SOCPS_AONTimerINT_EN(u32 Status);
extern void SOCPS_AONTimerClearINT(void);

extern void SOCPS_SleepInit(void);
extern void SOCPS_DeepSleep_RAM(void);
extern void SOCPS_SleepPG(void);
extern void SOCPS_SleepCG(void);
extern void SOCPS_AP_suspend_config(u32 Protection);
extern void SOCPS_AP_resume_config(u32 Protection);
extern void SOCPS_PSRAM_ClkSet(u8 Source);
extern void SOCPS_Peripheral_Clk_Sleep(u8 state);
extern void SOCPS_HWSleepFilter_KM0(u32 enable);

extern void SOCPS_MDLO_Disable(void);
extern void SOCPS_UartRxPinWakeSet(u32 status);

#endif  //_AMEBA_SOCPS_H_
