/*
 * Copyright (c) 2022-2023 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * This file is derivative of CMSIS V5.9.0 startup_ARMCM33.c
 * Git SHA: 2b7495b8535bdcb306dac29b9ded4cfb679d7e5c
 */

#include "cmsis.h"
#include "core_cache.h"
#include "ameba_cache.h"
#include "build_config_check.h"
#include "fih.h"
#include "memory_symbols.h"
#include "spm.h"
#include "tfm_hal_isolation.h"
#include "tfm_hal_platform.h"
#include "runtime_defs.h"

#define MPU_MAX_REGION                  4
#define NVIC_REG_NUM	DIV_ROUND_UP(MAX_PERIPHERAL_IRQ_NUM, 32)

typedef struct {
	u32 CTRL;
	u32 MAIR0;
	u32 MAIR1;
	u32 RBAR[MPU_MAX_REGION];
	u32 RLAR[MPU_MAX_REGION];
} MPU_BackUp_TypeDef;

typedef struct {
	u32 NVICbackup_ITNS[NVIC_REG_NUM];
	u32 NVICbackup_ISER[NVIC_REG_NUM];
	u8  NVICbackup_IPR[MAX_PERIPHERAL_IRQ_NUM];

	u32 SCBbackup_AIRCR;
	u32 SCBbackup_VTOR;
	u8  SCBbackup_SHPR[12];

	u32 BASEPRI_backup_HP;
	u32 PRIMASK_backup_HP;
	MPU_BackUp_TypeDef MPU_BK;
} CPU_BackUp_TypeDef;

CPU_BackUp_TypeDef PMC_BK;

void SOCPS_NVICBackup_HP(void)
{
	int i = 0;
	NVIC_Type *pNVIC = NVIC;

	for (i = 0; i < NVIC_REG_NUM; i++) {
		PMC_BK.NVICbackup_ITNS[i] = pNVIC->ITNS[i]; /* 1 means interrupt targets Non-secure state. */
		PMC_BK.NVICbackup_ISER[i] = pNVIC->ISER[i] & ~PMC_BK.NVICbackup_ITNS[i];
	}

	for (i = 0; i < MAX_PERIPHERAL_IRQ_NUM; i++) {
		PMC_BK.NVICbackup_IPR[i] = pNVIC->IPR[i];
	}

#if defined(__VTOR_PRESENT) && (__VTOR_PRESENT == 1)
	PMC_BK.SCBbackup_VTOR = SCB->VTOR;
#endif
}

void SOCPS_MPUBackup_HP(void)
{
	MPU_Type *pMPU = MPU;
	int i = 0;
	PMC_BK.MPU_BK.CTRL = pMPU->CTRL;

	PMC_BK.MPU_BK.MAIR0 = pMPU->MAIR0;
	PMC_BK.MPU_BK.MAIR1 = pMPU->MAIR1;
	for (i = 0; i < MPU_MAX_REGION; i++) {
		pMPU->RNR = i;
		PMC_BK.MPU_BK.RBAR[i] = pMPU->RBAR;
		PMC_BK.MPU_BK.RLAR[i] = pMPU->RLAR;
	}
}

void SOCPS_NVICReFill_HP(void)
{
	int i = 0;
	NVIC_Type *pNVIC = NVIC;

#if defined(__VTOR_PRESENT) && (__VTOR_PRESENT == 1)
	SCB->VTOR = PMC_BK.SCBbackup_VTOR;
#endif

	for (i = 0; i < MAX_PERIPHERAL_IRQ_NUM; i++) {
		pNVIC->IPR[i] = PMC_BK.NVICbackup_IPR[i];
	}

	for (i = 0; i < NVIC_REG_NUM; i++) {
		pNVIC->ITNS[i] = PMC_BK.NVICbackup_ITNS[i];
		pNVIC->ISER[i] = PMC_BK.NVICbackup_ISER[i];
	}
}

void SOCPS_MPUReFill_HP(void)
{
	MPU_Type *pMPU = MPU;
	int i = 0;
	pMPU->CTRL = 0;
	pMPU->MAIR0 = PMC_BK.MPU_BK.MAIR0;
	pMPU->MAIR1 = PMC_BK.MPU_BK.MAIR1;
	for (i = 0; i < MPU_MAX_REGION; i++) {
		pMPU->RNR = i;
		pMPU->RBAR = PMC_BK.MPU_BK.RBAR[i];
		pMPU->RLAR = PMC_BK.MPU_BK.RLAR[i];
	}
	pMPU->CTRL = PMC_BK.MPU_BK.CTRL;
}

void SOCPS_Core_Backup(void)
{
	PMC_BK.BASEPRI_backup_HP = __get_BASEPRI();
	PMC_BK.PRIMASK_backup_HP = __get_PRIMASK();

	PMC_BK.SCBbackup_AIRCR = SCB->AIRCR;
	for (u32 i = 0; i < 12; i++) {
		PMC_BK.SCBbackup_SHPR[i] = SCB->SHPR[i];
	}

	/* backup registgers */
	SOCPS_NVICBackup_HP();
	SOCPS_MPUBackup_HP();

	DCache_Disable();
}

__attribute__((naked)) void ns_agent_tz_wakeup(uint32_t c_entry)
{
	__set_CONTROL_SPSEL(1); /*SPSEL=1 USE PSP*/

	__set_MSP(SPM_BOOT_STACK_BOTTOM);
    tfm_arch_set_msplim(SPM_BOOT_STACK_TOP);

    __asm volatile(
        "   b      ns_agent_tz_main       \n"
    );
}

void SOCPS_Core_ReFill(void)
{
    uint32_t VECTKEY;
    SCB_Type *scb = SCB;
    uint32_t AIRCR = PMC_BK.SCBbackup_AIRCR;

	DCache_Enable();

    /* Set PRIS flag in AIRCR */
    VECTKEY = (~AIRCR & SCB_AIRCR_VECTKEYSTAT_Msk);
    scb->AIRCR = SCB_AIRCR_PRIS_Msk |
                 VECTKEY |
                 (AIRCR & ~SCB_AIRCR_VECTKEY_Msk);

	for (u32 i = 0; i < 12; i++) {
		SCB->SHPR[i] = PMC_BK.SCBbackup_SHPR[i];
	}

	/* ReFill registers */
	__set_PRIMASK(PMC_BK.PRIMASK_backup_HP);
	__set_BASEPRI(PMC_BK.BASEPRI_backup_HP);

	SOCPS_NVICReFill_HP();
	SOCPS_MPUReFill_HP();

	struct partition_t *p_part_ns = GET_CURRENT_COMPONENT();
	/* common runtime metadata */
	uint32_t allocate_size = sizeof(struct runtime_metadata_t);
	if (!IS_IPC_MODEL(p_part_ns->p_ldinf)) {
		/* SFN specific metadata - SFN function table */
		allocate_size += sizeof(service_fn_t) * p_part_ns->p_ldinf->nservices;
	}

	/* arch_seal_thread_stack() */
	allocate_size= ((allocate_size + 7) & ~0x7) + TFM_STACK_SEALED_SIZE;

	tfm_arch_set_psplim(p_part_ns->ctx_ctrl.sp_limit);
	__set_PSP(p_part_ns->ctx_ctrl.sp_base - allocate_size);

	uint32_t c_entry = tfm_hal_get_ns_entry_point();
	ns_agent_tz_wakeup(c_entry);
}

