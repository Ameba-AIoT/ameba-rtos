/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SOCPS_H_
#define _AMEBA_SOCPS_H_

#ifdef __cplusplus
extern "C" {
#endif

enum wakeup_mask {
	WAKEUP_NULL	= 0,
	WAKEUP_KM0	= 1,
	WAKEUP_KM4	= 2,
};

struct WakeEvent_TypeDef {
	u32 Module;
	enum wakeup_mask Wakeup;
};

struct PSCFG_TypeDef {
	u8 keep_osc4m_on;			/* keep OSC4M on or off for sleep and dslp*/
	u8 xtal_mode_in_sleep;		/* set xtal mode during sleep mode*/
	u8 sleep_to_08V;			/* default sleep to 0.7V, set this option to TRUE will sleep to 0.8V */
};

extern void SOCPS_SetNPWakeEvent(u32 Option, u32 NewStatus);
extern void SOCPS_SetAPWakeEvent(u32 Option, u32 NewStatus);

extern int SOCPS_AONWakeReason(void);

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

#ifdef __cplusplus
}
#endif

#endif  //_AMEBA_SOCPS_H_
