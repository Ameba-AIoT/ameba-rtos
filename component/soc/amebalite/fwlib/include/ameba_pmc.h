/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PMC_H_
#define _AMEBA_PMC_H_

/** @addtogroup AmebaD_Platform
  * @{
  */

/** @defgroup PMC
  * @brief PMC driver modules
  * @{
  */

/** @addtogroup PMC
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * we support  following soc power save functions:
  *		- sleep clock gating
  *		- sleep power gating
  *		- deep standby
  *		- deep sleep
  *
  *****************************************************************************************
  * sleep power gating
  *****************************************************************************************
  * following functions can be used when power gating:
  *		-UART0/UART1
  *		-TIM4/TIM5
  *		-RTC
  *		-WIFI
  *		-SDIO
  *		-USB
  *		-I2C0/I2C1
  *		-ADC
  *		-GPIO
  *		-REGU timer
  *		-normal wakepin
  *		-ANA timer
  * following functions will be closed when power gating:
  *		-UART2 LOGUART
  *		-TIM0-TIM3
  *		-SPIC flash
  *
  *****************************************************************************************
  * deep standby
  *****************************************************************************************
  * following functions can be used when deep standby:
  *		-RTC
  *		-REGU timer
  *		-normal wakepin
  *		-ANA timer
  *
  *****************************************************************************************
  * deep sleep
  *****************************************************************************************
  * following functions can be used when deep standby:
  *		-REGU timer
  *		-REGU wakepin
  *
  *****************************************************************************************
  * wakepin (A18/A5/A22/A23: mux normal wakepin and REGU wakepin)
  *****************************************************************************************
  * normal wakepin:
  *		-SLP_CG
  *		-SLP_PG
  *		-STDBY
  * REGU wakepin:
  *		-just used in DSLP (1.2V closed)
  *		-just support high acive, so this pin should pull low on your board
  *
  *****************************************************************************************
  *****************************************************************************************
  * SLP & SNZ power option
  *****************************************************************************************
  * BIT_SYSON_PMOPT_SLP_EN_SWR & BIT_SYSON_PMOPT_SNZ_EN_SWR
  *		-we have two 1.2V LDO
  *			-BIG LDO: SWR mode or LDO mode (can config )
  *			-LITTLE LDO: a little 1.2v LDO
  *		-BIT_SYSON_PMOPT_SLP_EN_SWR
  *			-ENABLE/DISABLE BIG LDO when SLP
  *		BIT_SYSON_PMOPT_SNZ_EN_SWR
  *			-ENABLE/DISABLE BIG LDO when SNZ, WIFI & ADC need open BIG LDO when SNZ
  *
  * BIT_SYSON_PMOPT_SLP_EN_PWM & BIT_SYSON_PMOPT_SNZ_EN_PWM
  *		-BIT_SYSON_PMOPT_SLP_EN_PWM
  *			-ENABLE/DISABLE LDO heavy loading current mode when SLP
  *		-BIT_SYSON_PMOPT_SNZ_EN_PWM
  *			-ENABLE/DISABLE heavy loading current mode when SNZ, WIFI & ADC need heavy loading when SNZ
  *
  * BIT_SYSON_PMOPT_SLP_XTAL_EN & BIT_SYSON_PMOPT_SNZ_XTAL_EN
  *		-WIFI and SOC both need XTAL when work,
  *		-but WIFI have individual option to control XTAL, so BIT_SYSON_PMOPT_SNZ_XTAL_EN not needed
  *
  * BIT_SYSON_PMOPT_SLP_SYSPLL_EN & BIT_SYSON_PMOPT_SNZ_SYSPLL_EN
  *		-WIFI and SOC both have individual PLL, here is SOC 500M PLL
  *		-So BIT_SYSON_PMOPT_SNZ_SYSPLL_EN not needed
  *
  * BIT_SYSON_SNFEVT_WIFI_MSK = 1 & BIT_SYSON_BYPASS_SNZ_SLP = 1
  *		- after OS suspend, platform will enter SNZ and close CPU, then platform enter sleep mode when WIFI 32K
  *		- BIT_SYSON_PMOPT_SNZ_EN_SOC should never open, or CPU will not close when platform will enter SNZ
  *
  * BIT_SYSON_SNFEVT_WIFI_MSK = 1 & BIT_SYSON_BYPASS_SNZ_SLP = 0 (not use this config)
  *		- after OS suspend, platform will enter sleep mode & close CPU after WIFI 32K
  *****************************************************************************************
  * @endverbatim
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
	PMC_MAX = 31,
};

/**
  * @}
  */

typedef struct {
	u8 keep_OSC4M_on;
	u8 xtal_mode_in_sleep;
} PSCFG_TypeDef;

extern u32 psp_temp;
extern u32 psplim_temp;

void SOCPS_SleepCG_RAM(void);
void np_pm_init(void);
void np_boot_on(void);
int km4_suspend(SLEEP_ParamDef *psleep_param);
int kr4_suspend(SLEEP_ParamDef *psleep_param);
void km4_resume(void);
void kr4_resume(void);
u32 kr4_status_on(void);
u32 km4_status_on(void);
u32 dsp_status_on(void);

void km4_set_wake_event(u32 wevt);

void km4_tickless_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum);
void kr4_tickless_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum);
void dsp_tickless_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum);

void SOCPS_KR4WKM4_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum);
void SOCPS_KM4WKR4_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum);
void SOCPS_NPWDSP_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum);
void SOCPS_APWDSP_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum);

void ap_resume(void);
void ap_tickless_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum);

uint32_t pmu_get_km4sleeptime(void);
uint32_t pmu_get_kr4sleeptime(void);
void SOCPS_ResMemMode(u32 module);
void SOCPS_Kr4_StartTbl_Restore(void);
void SOCPS_Kr4_StartTbl_Backup(void);

#if defined (CONFIG_FW_DRIVER_COEXIST) && CONFIG_FW_DRIVER_COEXIST
extern void wifi_hal_system_resume_wlan(void);
#endif
#endif //_AMEBA_PMC_H_