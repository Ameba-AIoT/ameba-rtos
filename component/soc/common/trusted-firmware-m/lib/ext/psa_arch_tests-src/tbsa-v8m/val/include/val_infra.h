/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
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

#ifndef _VAL_INFRA_H_
#define _VAL_INFRA_H_

#include "val_common.h"

#define TBSA_REGION_SIZE(p1, p2) ((p1 >= p2) ? 0 : \
                                             ((uint32_t) (p2) - (uint32_t) (p1)))

#define TBSA_ERROR(status)              (status?1:0)

#define TBSA_NVRAM_BLOCK_SIZE           4
#define TBSA_NVRAM_OFFSET(nvram_idx)    (nvram_idx * TBSA_NVRAM_BLOCK_SIZE)

#define TBSA_TEST_MAJOR_GROUP_MASK      0xFF000000UL
#define TBSA_TEST_MINOR_GROUP_MASK      0x00FF0000UL
#define TBSA_TEST_CFG_INSTANCE_MASK     0x0000FFFFUL
#define TBSA_TEST_INVALID_CFG_ID        0xFFFFFFFFUL
#define TBSA_S_TEST_ACTIVE              0xAAAAAAAAUL
#define TBSA_NS_TEST_ACTIVE             0x55555555UL

/* typedef's */
typedef struct
{
    uint32_t wb:8;
    uint32_t cb:8;
    uint32_t wdogb:8;
    uint32_t res2:8;
}boot_t;

typedef struct
{
    uint32_t pass_cnt:8;
    uint32_t skip_cnt:8;
    uint32_t fail_cnt:8;
    uint32_t res0:8;
}test_count_t;

/* enums */
typedef enum
{
    WARM_BOOT_UNKNOWN               = 0x1,
    WARM_BOOT_SUPPORT_AVAILABLE     = 0x2,
    WARM_BOOT_REQUESTED             = 0x3,
    WARM_BOOT_COMPLETED             = 0x4,
    WARM_BOOT_SUPPORT_NOT_AVAILABLE = 0x5,
    COLD_BOOT_UNKNOWN               = 0x6,
    COLD_BOOT_SUPPORT_AVAILABLE     = 0x7,
    COLD_BOOT_REQUESTED             = 0x8,
    COLD_BOOT_COMPLETED             = 0x9,
    COLD_BOOT_SUPPORT_NOT_AVAILABLE = 0xA,
    WDOG_BOOT_UNKNOWN               = 0xB,
    WDOG_BOOT_SUPPORT_AVAILABLE     = 0xC,
    WDOG_BOOT_REQUESTED             = 0xD,
    WDOG_BOOT_COMPLETED             = 0xE,
    WDOG_BOOT_SUPPORT_NOT_AVAILABLE = 0xF,
    BOOT_UNKNOWN                    = 0x10
}boot_state_t;

typedef enum
{
    WARM_RESET    = 0x1,
    COLD_RESET    = 0x2,
    WDOG_RESET    = 0x3,
    UNKNOWN_RESET = 0x4
}system_reset_t;

typedef enum
{
    NV_TEST     = 0x0,
    NV_TEST_CNT = 0x1,
    NV_BOOT     = 0x2,
    NV_DPM1     = 0x3,
    NV_DPM2     = 0x4,
    NV_SHCSR    = 0x5,
    NV_SPAD1    = 0x6,
    NV_SPAD2    = 0x7,
    NV_ACT_TST  = 0x8,
}nvram_index_t;

/* prototypes */
void           tbsa_dispatcher(test_id_t test_id_prev);
tbsa_status_t  val_print(print_verbosity_t verbosity, char *string, uint32_t data);
tbsa_status_t  val_infra_init(test_id_t *test_id);
tbsa_status_t  val_infra_exit(void);
tbsa_status_t  val_target_get_config(cfg_id_t cfg_id, uint8_t **data, uint32_t *size);

tbsa_status_t  val_err_check_set(uint32_t checkpoint, tbsa_status_t status);
tbsa_status_t  val_report_status (test_id_t test_id);
tbsa_status_t  val_set_status    (uint32_t status);
uint32_t       val_get_status    (void);

void           val_system_reset      (system_reset_t reset_type);
bool_t         val_system_reset_type (system_reset_t reset_type);

char           *val_get_comp_name(test_id_t test_id);

tbsa_status_t  val_get_test_binary_info(addr_t *test_binary_src_addr, uint32_t *test_binary_in_ram);

void           val_memcpy                  (void *dst, void *src, uint32_t size);
void           val_memset                  (void *dst, uint32_t str,  uint32_t size);
uint32_t       val_execute_in_trusted_mode (addr_t address);

bool_t         val_is_vtor_relocated_from_rom(void);

bool_t is_sec_ext_enabled(void);
#endif /* _VAL_INFRA_H_ */
