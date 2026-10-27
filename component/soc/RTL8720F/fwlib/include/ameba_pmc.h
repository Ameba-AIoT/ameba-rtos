/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PMC_H_
#define _AMEBA_PMC_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Platform
  * @{
  */

/** @defgroup PMC
  * @brief PMC driver modules
  * @{
  */

/** @defgroup sleep_parameter np & ap sleep parameter
  * @{
  */
#ifndef CONFIG_BUILD_ROM

typedef struct {
	u32	sleep_type;
	u32	sleep_time;
	u32	dlps_enable;
	u32	rsvd[5];
} SLEEP_ParamDef;

#endif

enum SLEEP_TYPE {
	SLEEP_PG	= 0,
	SLEEP_CG	= 1,
	SLEEP_DSLP = 3,
};

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup PMC_Exported_Constants PMC Exported Constants
  * @{
  */


/** @defgroup MEM_Mode_definitions
  * @{
  */
#define MEM_ACT_MODE			((u32)0x00000000)
#define MEM_LS_MODE				((u32)0x00000001)
#define MEM_DS_MODE				((u32)0x00000002)
#define MEM_SD_MODE				((u32)0x00000004)
#define IS_MEM_MODE(MODE) (((MODE) == MEM_ACT_MODE) || \
									   ((MODE) == MEM_LS_MODE) || \
									   ((MODE) == MEM_DS_MODE) || \
									   ((MODE) == MEM_SD_MODE))
/**
  * @}
  */

/** @defgroup MEM_Type_definitions
  * @{
  */
#define APM_PSRAM_SlEEP_Mode	0
#define WB_PSRAM_SlEEP_Mode		1
#define DDR_SlEEP_Mode			2
#define FLASH_SLEEP_Mode		3

/** @defgroup NP_WAKE_AP by IPC list
  * @{
  */
#define FW_NPWAP_IPC			0
#define TIMER_NPWAP_IPC			1
/**
  * @}
  */
enum PMC_CORE_ROLE_TYPE {
	PMC_CORE_ROLE_AP2NP = 0,
	PMC_CORE_ROLE_SINGLE = 1,
};

/**
  * @}
  */
enum PMC_AP_WAKE_STATUS {
	PMC_WLAN_STATUS = 0,
	PMC_MAX = 7,
};

/**
  * @}
  */
int ap_suspend(void);
void ap_resume(void);
u32 ap_status_on(void);
void ap_clock_gate(void);
void ap_clock_on(void);

void ap_tickless_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum);
void SOCPS_NPWAP_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum);

/*If trustzone is enabled, the service entry is as follows*/
void SOCPS_SleepInitEntry(struct PSCFG_TypeDef *ps_config);
void SOCPS_PeriRestoreEntry(void);
u32 SOCPS_CPURoleGetEntry(void);
void SOCPS_HWSleepFilter(u32 enable);
void SOCPS_NVICBackupEntry(struct CPU_BackUp_TypeDef *bk, SysTick_Type *systick, NVIC_Type *nvic, SCB_Type *scb);
void SOCPS_NVICReFillEntry(struct CPU_BackUp_TypeDef *bk, SysTick_Type *systick, NVIC_Type *nvic, SCB_Type *scb);
void SOCPS_MPUBackupEntry(struct CPU_BackUp_TypeDef *bk, MPU_Type *mpu);
void SOCPS_MPUReFillEntry(struct CPU_BackUp_TypeDef *bk, MPU_Type *mpu);
void SOCPS_PeriPermissionEntry(uint32_t ip_mask, u32 enable);
void SOCPS_BitPermissionEntry(uint32_t ip_mask, u32 enable);

#ifdef __cplusplus
}
#endif

#endif //_AMEBA_PMC_H_