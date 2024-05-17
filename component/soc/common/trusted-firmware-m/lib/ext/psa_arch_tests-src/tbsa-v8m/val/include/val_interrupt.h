/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#ifndef _VAL_INTERRUPT_H_
#define _VAL_INTERRUPT_H_

#include "val_common.h"
#include "val_pe.h"

/* Exception numbers */
#define   EXCP_NUM_NMI                   2
#define   EXCP_NUM_HF                    3
#define   EXCP_NUM_MMF                   4
#define   EXCP_NUM_BF                    5
#define   EXCP_NUM_UF                    6
#define   EXCP_NUM_SF                    7
#define   EXCP_NUM_SVC                   11
#define   EXCP_NUM_DME                   12
#define   EXCP_NUM_PENDSV                14
#define   EXCP_NUM_SYSTICK               15
#define   EXCP_NUM_EXT_INT(n)            16 + n

#define   NUM_OF_EXT_INT                 45
#define   MAX_EXCP_NUM                   (EXCP_NUM_EXT_INT(0) + NUM_OF_EXT_INT)

#define   INTR_NUM_INVALID               0xFFFF
#define   IS_INTR_NUM_VALID(intr_num)    ((intr_num - EXCP_NUM_EXT_INT(0)) >= INTR_NUM_INVALID ? 0 : 1)

/* typedef's */
typedef void (*tbsa_isr_vector) (void);

/* prototypes*/
void TBSA_S_NMI_Handler         (void);
void TBSA_S_HardFault_Handler   (void);
void TBSA_S_MemManage_Handler   (void);
void TBSA_S_BusFault_Handler    (void);
void TBSA_S_UsageFault_Handler  (void);
void TBSA_S_SecureFault_Handler (void);
void TBSA_S_SVC_Handler         (void);
void TBSA_S_DebugMon_Handler    (void);
void TBSA_S_PendSV_Handler      (void);
void TBSA_S_SysTick_Handler     (void);
void TBSA_S_default_sys_handler (void);
void TBSA_S_default_ext_int_handler (void);

void TBSA_NS_default_sys_handler (void);

tbsa_status_t val_interrupt_install_handler(uint32_t irq_num, tbsa_isr_vector handler);
tbsa_status_t val_interrupt_restore_handler(uint32_t irq_num);
tbsa_status_t val_interrupt_setup_handler(uint32_t excep_num, uint32_t priority, tbsa_isr_vector handler);
tbsa_status_t val_interrupt_enable(uint32_t intr_num);
tbsa_status_t val_interrupt_disable(uint32_t intr_num);
tbsa_status_t val_interrupt_route(uint32_t intr_num, cpu_state_t tgt_state);
tbsa_status_t val_interrupt_set_priority(uint32_t intr_num, uint32_t priority);
tbsa_status_t val_interrupt_get_pending_status(uint32_t intr_num, uint32_t *status);
tbsa_status_t val_interrupt_get_active_status(uint32_t intr_num, uint32_t *status);
tbsa_status_t val_interrupt_set_pending(uint32_t intr_num);
tbsa_status_t val_interrupt_clear_pending(uint32_t intr_num);

#endif /* _VAL_INTERRUPT_H_ */
