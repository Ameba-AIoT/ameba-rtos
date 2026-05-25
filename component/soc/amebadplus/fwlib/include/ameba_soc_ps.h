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
	u32 BASEPRI_backup_HP;
	u32 PRIMASK_backup_HP;
	u8 NVICIPbackup_HP[MAX_PERIPHERAL_IRQ_NUM];
	MPU_BackUp_TypeDef MPU_BK;
} CPU_BackUp_TypeDef;

#define PMC_BK		((CPU_BackUp_TypeDef *)(MSPLIM_RAM_HP_NS))
#else
typedef struct {
	u32 CPURegbackup[25];
	/* ISER read == ICER read, so we only save the enable mask once.
	 * On refill we use 0xFFFFFFFF on ICER to disable everything before
	 * re-applying the saved enable set.
	 */
	u32 NVICbackup_ISER[MAX_NVIC_ISER_NUM];
	u32 NVICbackup_ISPR[MAX_NVIC_ISER_NUM];
	u32 SYSTICKbackup[4];
	u32 SCBbackup[4];
	u32 CPUPSP;
	u32 PortSVC_Backup;
	u32 PRIMASK_Backup;
	u32 NVICIPbackup[MAX_NVIC_IPR_NUM];
	MPU_BackUp_TypeDef MPU_BK;
	MMU_Backup_TypeDef MMU_BK;
} CPU_BackUp_TypeDef;

#define PMC_BK		((CPU_BackUp_TypeDef *)(MSPLIM_RAM_LP))
#endif


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
