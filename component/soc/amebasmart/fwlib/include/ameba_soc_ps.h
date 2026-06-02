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

typedef struct {
	u32 CTRL;
	u32 MAIR0;
	u32 MAIR1;
	u32 RBAR[MPU_MAX_REGION];
	u32 RLAR[MPU_MAX_REGION];
} MPU_BackUp_TypeDef;

typedef struct {
	u32 MMU_ENTRY0_CTRL;			/*!< flash MMU entry_x control */
	u32 MMU_ENTRY0_STRADDR;		/*!< flash MMU entry_x virtual address start */
	u32 MMU_ENTRY0_ENDADDR;		/*!< flash MMU entry_x virtual address end */
	u32 MMU_ENTRY0_OFFSET;
	u32 MMU_ENTRY1_CTRL;			/*!< flash MMU entry_x control */
	u32 MMU_ENTRY1_STRADDR;		/*!< flash MMU entry_x virtual address start */
	u32 MMU_ENTRY1_ENDADDR;		/*!< flash MMU entry_x virtual address end */
	u32 MMU_ENTRY1_OFFSET;
} MMU_Backup_TypeDef;

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
#else
typedef struct {
	u32 CPURegbackup[25];
	u32 NVICbackup[4];
	u32 SYSTICKbackup[4];
	u32 SCBbackup[4];
	u32 CPUPSP;
	u32 IPCbackup;
	u32 PortSVC_Backup;
	u32 NVICIPbackup[MAX_NVIC_IPR_NUM];
	MPU_BackUp_TypeDef MPU_BK;
	MMU_Backup_TypeDef MMU_BK;
} CPU_BackUp_TypeDef;

#define PMC_BK		((CPU_BackUp_TypeDef *)(MSPLIM_RAM_LP))
#endif


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
extern void SOCPS_Backup_CPU1(void);
extern void SOCPS_Restore_CPU1(void);

#ifdef __cplusplus
}
#endif

#endif  //_AMEBA_SOCPS_H_
