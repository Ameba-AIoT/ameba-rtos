/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SOCPS_H_

#ifdef __cplusplus
extern "C" {
#endif

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


struct WakeEvent_TypeDef {
	u32 Module;
	enum wakeup_mask wakeup;
};

struct PSCFG_TypeDef {
	u8 keep_osc4m_on;			/* keep OSC4M on or off for sleep and dslp*/
	u8 xtal_mode_in_sleep;		/* set xtal mode during sleep mode*/
};

typedef struct {
	u32 Module;
	u32 MEM_SD;
	u32 MEM_DS;
	u32 MEM_LS;
} MEMMode_TypeDef;

typedef struct {
	u32 CTRL;
	u32 MAIR0;
	u32 MAIR1;
	u32 RBAR[MPU_MAX_REGION];
	u32 RLAR[MPU_MAX_REGION];
} MPU_BackUp_TypeDef;

#if defined (CONFIG_ARM_CORE_CM4)
typedef struct {
	u32 CPURegbackup_HP[25];
	u32 CPUPSP_HP;
	u32 PortSVC_Backup_HP;
	u32 NVICbackup_HP[6];
	u32 SYSTICKbackup_HP[4];
	u32 SCBbackup_HP[4];
	u32 IPCbackup_HP;
	u32 BASEPRI_backup_HP;
	u32 PRIMASK_backup_HP;
	u8 NVICIPbackup_HP[MAX_PERIPHERAL_IRQ_NUM];
	MPU_BackUp_TypeDef MPU_BK;
} CPU_BackUp_TypeDef;

#define PMC_BK		((CPU_BackUp_TypeDef *)(MSPLIM_RAM_HP_NS))
#endif

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

extern void SOCPS_SetNPWakeEvent(u32 Option, u32 NewStatus);
extern void SOCPS_SetAPWakeEvent(u32 Option, u32 NewStatus);
extern void SOCPS_SetDSPWakeEvent(u32 Option, u32 NewStatus);

extern void SOCPS_DeepSleep_RAM(void);
extern void SOCPS_Peripheral_Clk_Sleep(u8 state);
extern void SOCPS_UartRxPinWakeSet(u32 status);
extern void SOCPS_SetReguOCP(u8 state);

#ifdef __cplusplus
}
#endif

#endif  //_AMEBA_SOCPS_H_
