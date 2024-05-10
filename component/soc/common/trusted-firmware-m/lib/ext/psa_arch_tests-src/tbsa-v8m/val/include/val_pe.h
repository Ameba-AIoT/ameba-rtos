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

#ifndef _VAL_PE_H_
#define _VAL_PE_H_

#include "val_common.h"

/* CPU Architecture values */
#define ARMV8M_BASELINE         0xC
#define ARMV8M_MAINLINE         0xF

/* CPU register bit masks */
#define MPU_RBAR_XN             0x1
#define MPU_RBAR_AP             0x6
#define MPU_RBAR_SH             0x18
#define MPU_RLAR_EN             0x1
#define MPU_RLAR_ATTR_IDX       0xE
#define SAU_RLAR_NSC            0x2
#define SAU_RLAR_EN             0x1

/* Memory-mapped CPU register offsets */
#define SYST_CSR                0xE000E010
#define NVIC_ISER(n)            0xE000E100 + 4*n  // n: 0 to 15
#define NVIC_ICER(n)            0xE000E180 + 4*n  // n: 0 to 15
#define NVIC_ISPR(n)            0xE000E200 + 4*n  // n: 0 to 15
#define NVIC_ICPR(n)            0xE000E280 + 4*n  // n: 0 to 15
#define NVIC_IABR(n)            0xE000E300 + 4*n  // n: 0 to 15
#define NVIC_ITNS(n)            0xE000E380 + 4*n  // n: 0 to 15
#define NVIC_IPR(n)             0xE000E400 + 4*n  // n: 0 to 123
#define CPUID                   0xE000ED00
#define ICSR                    0xE000ED04
#define VTOR                    0xE000ED08
#define AIRCR                   0xE000ED0C
#define SHCSR                   0xE000ED24
#define CFSR                    0xE000ED28
#define HFSR                    0xE000ED2C
#define MMFAR                   0xE000ED34
#define BFAR                    0xE000ED38
#define ID_PFR1                 0xE000ED44 /* Processor Feature Register 1 */
#define MPU_TYPE                0xE000ED90
#define MPU_CTRL                0xE000ED94
#define MPU_RNR                 0xE000ED98
#define MPU_RBAR                0xE000ED9C
#define MPU_RLAR                0xE000EDA0
#define MPU_MAIR0               0xE000EDC0
#define MPU_MAIR1               0xE000EDC4
#define SAU_TYPE                0xE000EDD4
#define SAU_RNR                 0xE000EDD8
#define SAU_RBAR                0xE000EDDC
#define SAU_RLAR                0xE000EDE0
#define SFSR                    0xE000EDE4
#define SFAR                    0xE000EDE8
/* Non-secure view of memory mapped register when accessed from secure */
#define VTOR_NS                 (VTOR     | SECURITY_OFFSET)
#define CPUID_NS                (CPUID    | SECURITY_OFFSET)
#define SYST_CSR_NS             (SYST_CSR | SECURITY_OFFSET)
#define MPU_TYPE_NS             (MPU_TYPE | SECURITY_OFFSET)

/* typedef's */
typedef enum {
  BYTE = 1,
  HALF_WORD,
  WORD
} mem_access_size_t;

typedef enum {
  PE_SECURE=100,
  PE_NONSECURE
} cpu_state_t;

typedef union
{
    struct
    {
        uint32_t  rev:4;
        uint32_t  par:12;
        uint32_t  arc:4;
        uint32_t  var:4;
        uint32_t  imp:8;
    }id_s;
    uint32_t id;
}cpuid_t;

typedef enum {
  CONTROL = 10,
  CONTROL_NS
}sys_reg_t;

tbsa_status_t val_mem_reg_read        (uint32_t reg_name, uint32_t *rd_data);
tbsa_status_t val_pe_reg_read         (sys_reg_t reg_name, uint32_t *rd_data);
tbsa_status_t val_mem_read            (uint32_t *address, mem_access_size_t access_size, uint32_t *rd_data);
tbsa_status_t val_mem_read_wide       (uint32_t *address, uint32_t *rd_data);
tbsa_status_t val_mem_write_wide      (uint32_t *address, uint32_t wr_data);
tbsa_status_t val_mem_reg_write       (uint32_t reg_name, uint32_t wr_data);
tbsa_status_t val_pe_reg_write        (sys_reg_t reg_name, uint32_t wr_data);
tbsa_status_t val_mem_write           (uint32_t *address, mem_access_size_t access_size, uint32_t wr_data);
bool_t        val_is_secure_address   (addr_t address);

/* Assembly function prototype */
uint32_t ldrb_asm(uint32_t *address);
uint32_t ldrh_asm(uint32_t *address);
uint32_t ldr_asm(uint32_t *address);
void     strb_asm(uint32_t *address, uint32_t wr_data);
void     strh_asm(uint32_t *address, uint32_t wr_data);
void     str_asm(uint32_t *address, uint32_t wr_data);
uint32_t tt_asm(uint32_t *address);
uint32_t ldr_wide_asm(uint32_t *address);
void     str_wide_asm(uint32_t *address, uint32_t wr_data);

void     dsb_asm(void);
void     isb_asm(void);
uint32_t sysread_control_asm(void);
uint32_t sysread_control_ns_asm(void);
void     syswrite_control_asm(uint32_t r0);
void     syswrite_control_ns_asm(uint32_t r0);

#endif /* _VAL_PE_H_ */
