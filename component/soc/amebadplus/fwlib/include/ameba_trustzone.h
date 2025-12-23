/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_TRUSTZONE_H_
#define _AMEBA_TRUSTZONE_H_

#include "ameba_rxi300.h"

typedef struct {
	MPC_EntryTypeDef MPC_Entry[8];                        /*!< RXI300 IP have 8 ports,  Address offset: 0x200-0x23C */
	__IO uint32_t IDAU_CTRL;                              /*!< IDAU ENABLE CONTROL REGISTER,  Address offset: 0x240 */
	__IO uint32_t IDAU_LOCK;                              /*!< IDAU LOCK CONTROL REGISTER,  Address offset: 0x244 */
} MPC_TypeDef;

typedef struct {
	__IO uint32_t PPC1_REG;							  /*!< PPC1 CONTROL REGISTER,  Address offset: 0x400 */
	__IO uint32_t PPC2_REG;							  /*!< PPC2 CONTROL REGISTER,  Address offset: 0x404 */
	__IO uint32_t PPC3_REG;							  /*!< PPC3 CONTROL REGISTER,  Address offset: 0x408 */
	__IO uint32_t PPC_LOCK; 							  /*!< PPC LOCK CONFIGURE REGISTER,  Address offset: 0x40C */
} PPC_TypeDef;

typedef struct {
	u32 Start;
	u32 End;
	u32 CTRL;
} TZ_CFG_TypeDef;

typedef struct {
	u32 Start;
	u32 End;
	u32 NSC;
} SAU_CFG_TypeDef;

typedef enum {
	SECFG_WIFI_CFG = 0,
	SECFG_BT_CFG,
	SECFG_SECURE_ENGINE,	//including SECFG_AES/SECFG_SHA
	SECFG_GDMA0_CFG,
	SECFG_PPE_CFG,
	SECFG_SDIO_CFG,
	SECFG_SPI0,
	SECFG_SPI1,
	SECFG_PSRAM_PHY,
	SECFG_PSRAM_SPIC_USERMODE,
	SECFG_SPIC_USERMODE,
	SECFG_QSPI,
	SECFG_SPORT0_I2S,
	SECFG_SPORT1_I2S,

	SECFG_OTPC_CFG = 0x20,
	SECFG_SYSON,			//including SECFG_RTC/SECFG_WDG/SECFG_SDM32K
	SECFG_UART0,
	SECFG_UART1,
	SECFG_UART2_BT,
	SECFG_UART3_LOG,
	SECFG_GPIOA_B,
	SECFG_ADC_ADC_COMP,
	SECFG_CAP_TOUCH,
	SECFG_KEY_SCAN,
	SECFG_IPC,
	SECFG_DBG_TIMER,
	SECFG_PMC_TIMER_0_1,
	SECFG_TIMER0_7_BASIC,

	SECFG_TIMER8_9_PULSE_PWM_TIMER10_11 = 0x40,
	SECFG_TRNG_PORT1_PORT2,
	SECFG_RXI300,
	SECFG_RSIP,
	SECFG_LEDC,
	SECFG_PDM,
	SECFG_IR,
	SECFG_I2C0,
	SECFG_I2C1,
} PPC_PeripheralId;

#define SECURE				1
#define NON_SECURE			0

#define MPC_RW				RXI300_WRITE_CTRL(0)
#define MPC_RO				RXI300_WRITE_CTRL(1)
#define MPC_S				RXI300_SECURE_CTRL(0)
#define MPC_NS				RXI300_SECURE_CTRL(1)

/*
  *   <q> Enable SAU
  *   <i> Value for SAU->CTRL register bit ENABLE
  */
#define SAU_INIT_CTRL_ENABLE   1

/*
  *   <o> When SAU is disabled
  *     <0=> All Memory is Secure
  *     <1=> All Memory is Non-Secure
  *   <i> Value for SAU->CTRL register bit ALLNS
  *   <i> When all Memory is Non-Secure (ALLNS is 1), IDAU can override memory map configuration.
  */
#define SAU_INIT_CTRL_ALLNS  0

#define SAU_ENTRY_NUM			8
#define MPC_ENTRY_NUM			8

#define MPC_CNT					2


void BOOT_RAM_TZCfg(void);
void TZ_ConfigSlaveSecurity(PPC_PeripheralId Perip, u32 Status);


/* 	The TT instruction takes a memory address and returns the configuration of the Memory Protection Unit (MPU) at that address.
	The cmse_address_info_t is declared as following:
		typedef union {
			struct cmse_address_info {
				unsigned mpu_region:8;
				unsigned :8;
				unsigned mpu_region_valid:1;
				unsigned :1;
				unsigned read_ok:1;
				unsigned readwrite_ok:1;
				unsigned :12;
			} flags;
			unsigned value;
		} cmse_address_info_t;

	When executed in the secure state the result of TT instruction is extended to return the SAU and IDAU configurations at the specific address.
	The extended cmse_address_info_t is declared as following:
		typedef union {
			struct cmse_address_info {
				unsigned mpu_region:8;
				unsigned sau_region:8;
				unsigned mpu_region_valid:1;
				unsigned sau_region_valid:1;
				unsigned read_ok:1;
				unsigned readwrite_ok:1;
				unsigned nonsecure_read_ok:1;
				unsigned nonsecure_readwrite_ok:1;
				unsigned secure:1;
				unsigned idau_region_valid:1;
				unsigned idau_region:8;
			} flags;
			unsigned value;
		} cmse_address_info_t;

	As a result, these extended bits are only valid when executing in Secure state, and are UNDEFINED if used from Non-secure state.
*/
__STATIC_INLINE u32 TrustZone_IsSecure(void)
{
#if defined (CONFIG_ARM_CORE_CM4)
	cmse_address_info_t cmse_address_info = cmse_TT((void *)DiagPrintf);
	return cmse_address_info.flags.secure;
#else
	return 0;
#endif
}


#endif