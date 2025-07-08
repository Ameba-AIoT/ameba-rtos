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
};

struct WakeEvent_TypeDef {
	u32 Module;
	enum wakeup_mask Wakeup;
} ;

struct MPU_BackUp_TypeDef {
	u32 CTRL;
	u32 MAIR0;
	u32 MAIR1;
	u32 RBAR[MPU_MAX_REGION];
	u32 RLAR[MPU_MAX_REGION];
};

struct CPU_BackUp_TypeDef {
	u32 CPURegbackup_HP[25];
	u32 CPUPSP_HP;
	u32 PortSVC_Backup_HP;
	u32 NVICbackup_HP[9];
	u32 SYSTICKbackup_HP[4];
	u32 SCBbackup_HP[4];
	u32 BASEPRI_backup_HP;
	u32 PRIMASK_backup_HP;
	struct MPU_BackUp_TypeDef MPU_BK;
	u8 NVICIPbackup_HP[MAX_PERIPHERAL_IRQ_NUM];
};

extern struct CPU_BackUp_TypeDef PMC_BK;
extern u32 WakeEventFlag;
/*1. Sleep init*/
void SOCPS_SleepInit(void);
/*1.1 wake source settings.*/
void SOCPS_SetNPWakeEvent(u32 Option, u32 NewStatus);
void SOCPS_SetAPWakeEvent(u32 Option, u32 NewStatus);
void SOCPS_WakeSourceSet(struct WakeEvent_TypeDef *sleep_wevent_config);
/*1.2 Power setting*/
void SOCPS_ReguDelayAdjust(u8 delay_count);
void SOCPS_PowerManage(u8 regu_state);
/*1.3 Clock setting: XTAL and OSC4M. */
void SOCPS_ClockSourceConfig(u8 regu_state, u8 xtal_mode, u8 osc_option);
/*1.4 Peripheral clock settings. */
void SOCPS_AP_suspend_config(u32 Protection);
void SOCPS_AP_resume_config(u32 Protection);
void SOCPS_PsramCLockSwitch(bool is_xtal);
void SOCPS_FlashClockSwitch(bool is_xtal, u32 Protection);
/*2. Sleep entry/exit*/
void SOCPS_DeepSleep_RAM(void);
void SOCPS_SleepPG(void);
void SOCPS_SleepCG(void);
void SOCPS_SleepPG_RAM_FOR_WFE(struct CPU_BackUp_TypeDef *bk);
void SOCPS_SleepPG_RAM_FOR_WFI(struct CPU_BackUp_TypeDef *bk);
void SOCPS_SleepCG_RAM(void);
/*4. Reg backup/restore*/
void SOCPS_NVICBackup(struct CPU_BackUp_TypeDef *bk, SysTick_Type *systick, NVIC_Type *nvic, SCB_Type *scb);
void SOCPS_NVICReFill(struct CPU_BackUp_TypeDef *bk, SysTick_Type *systick, NVIC_Type *nvic, SCB_Type *scb);
void SOCPS_MPUBackup(struct CPU_BackUp_TypeDef *bk, MPU_Type *mpu);
void SOCPS_MPUReFill(struct CPU_BackUp_TypeDef *bk, MPU_Type *mpu);
void SOCPS_CPURegBackup(void);
void SOCPS_CPURegRestore(void);
/*5.Sleep trigger*/
void SOCPS_SleepModeRequest(u8 sleep_mode);
void SOCPS_SleepModeClear(void);
/*6.Fullmac mode only for cpu0*/
void SOCPS_Sleep_REQ(u32 mode);
void SOCPS_Sleep_FULLMAC(u32 type);
/*7.Others*/
void SOCPS_PowerGatingFlagSet(u32 flag);
void SOCPS_PowerGatingFlagClear(u32 flag);
int SOCPS_AONWakeReason(void);
void SOCPS_HWSleepFilter(u32 enable);
void SOCPS_WifiWakeupStatusPolling(void);
u32 SOCPS_CPURoleGet(void);
void SOCPS_HWSleepFilter_AP(u32 enable);
void SOCPS_HWSleepFilter_NP(u32 enable);
void SOCPS_PeriPermissionControl(uint32_t ip_mask, u32 enable);
void SOCPS_BitPermissionControl(uint32_t ip_mask, u32 enable);
void SOCPS_UartRxPinWakeSet(u32 status);
void SOCPS_FixSpicRetFailPatch(void);
void SOCPS_USBSuspendWakeControl(u8 status);
void SOCPS_PowerStateSetInSleep(u8 pwr_sts);
void SOCPS_PowerStateSetInNormal(u8 pwr_sts);
void SOCPS_SetMemMode(u32 module, u32 mem_mode);
#endif  //_AMEBA_SOCPS_H_