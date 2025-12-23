/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_TRUSTZONE_H_
#define _AMEBA_TRUSTZONE_H_

#include "ameba_rxi300.h"

typedef struct {
	MPC_EntryTypeDef MPC_Entry[6]; /*!< RXI300 IP have 6 ports */
	__IO uint32_t RSVD4[4];
	__IO uint32_t IDAU_CTRL;       /*!< IDAU ENABLE CONTROL REGISTER */
	__IO uint32_t IDAU_LOCK;       /*!< IDAU LOCK CONTROL REGISTER */
} MPC_TypeDef;

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
	SECFG_PPC1 = 0,
	SECFG_PPC2,
	SECFG_PPC3,
} PPC_Id;

#define SECURE				1
#define NON_SECURE			0

#define MPC_RW				RXI300_IDAU_WRITE_CTRL(0)
#define MPC_RO				RXI300_IDAU_WRITE_CTRL(1)
#define MPC_S				RXI300_IDAU_SECURE_CTRL(0)
#define MPC_NS				RXI300_IDAU_SECURE_CTRL(1)

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

#define MPC_ENTRY_NUM			6
#define MPC_CNT					3

/**
  * @brief  Configure slave port as secure or non-secure attribute.
  * @param  PPC_PeripheralId: ppc_index
  * @param  msk_bit: it can be RXI300_BIT_WIFI_CFG/RXI300_BIT_OTPC_CFG...
  * @param  Status: it can be SECURE or NON_SECURE
  *     For Example: PPC->PPC1_REG &= RXI300_BIT_OTPC_CFG means OTPC is secure port
  * @retval None
  */
__STATIC_FORCEINLINE void TZ_ConfigSlaveSecurity(PPC_Id ppc_idx, u32 msk_bit, u32 Status)
{
	PPC_TypeDef *PPC = AP_PPC;
	volatile u32 *PPC_REG = &PPC->PPC1_REG;

	if (Status == SECURE) {
		PPC_REG[ppc_idx] &= ~msk_bit;
	} else if (Status == NON_SECURE) {
		PPC_REG[ppc_idx] |= msk_bit;
	}
}

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
__STATIC_FORCEINLINE u32 TrustZone_IsSecure(void)
{
#if defined (CONFIG_ARM_CORE_CM4)
	cmse_address_info_t cmse_address_info = cmse_TT((void *)DiagPrintf);
	return cmse_address_info.flags.secure;
#else
	return 0;
#endif
}
#endif
