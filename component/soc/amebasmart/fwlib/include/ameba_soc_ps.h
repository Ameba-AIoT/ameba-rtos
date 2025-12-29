/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SOCPS_H_
#define _AMEBA_SOCPS_H_

enum wakeup_mask {
	WAKEUP_NULL	= 0,
	WAKEUP_LP	= 1,
	WAKEUP_NP	= 2,
	WAKEUP_AP	= 3,
};

enum xtal_mode_sleep {
	XTAL_OFF = 0,
	XTAL_LPS = 1,
	XTAL_Normal = 2,
};

typedef struct {
	u32 Module;
	u32 MEM_SD;
	u32 MEM_DS;
	u32 MEM_LS;
} MEMMode_TypeDef;

typedef struct {
	u32 Module;
	u32 Status;
} PWRCFG_TypeDef;

struct PSCFG_TypeDef {
	u8 km0_audio_vad_on;
	u8 keep_osc4m_on; 		/* keep OSC4M on or off for sleep and dslp*/
	u8 xtal_mode_in_sleep;  /* set xtal mode during sleep mode*/
};

struct WakeEvent_TypeDef {
	u32 Module;
	enum wakeup_mask wakeup;
};

extern PWRCFG_TypeDef sleep_sram_config[];

extern void SOCPS_SleepPG(void);
extern void SOCPS_SleepCG(void);

extern void SOCPS_SleepInit(void);
extern void SOCPS_Hsram_Setting(void);

extern u32 SOCPS_DsleepWakeStatusGet(void);
extern int SOCPS_AONWakeReason(void);

extern void SOCPS_MMUReFill(void);

extern void SOCPS_SetLPWakeEvent(u32 Option, u32 NewStatus);
extern void SOCPS_SetNPWakeEvent(u32 Option, u32 NewStatus);
extern void SOCPS_SetAPWakeEvent(u32 Option, u32 NewStatus);

extern void SOCPS_SWRLDO_Suspend(u32 new_status);
extern void SOCPS_SleepCG_RAM(void);
extern void SOCPS_SleepPG_RAM(void);
extern void SOCPS_DeepSleep_RAM(void);
extern void SOCPS_LPWHP_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum);

extern void SOCPS_WakeEvent_Init(void);
extern void SOCPS_SetPowerCut_Time(u32 time);
extern void SOCPS_SetReguOCP(u8 state);
#endif  //_AMEBA_SOCPS_H_
