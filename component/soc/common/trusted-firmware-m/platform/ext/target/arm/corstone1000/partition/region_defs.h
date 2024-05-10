/*
 * Copyright (c) 2017-2022 Arm Limited. All rights reserved.
 * Copyright (c) 2021, Cypress Semiconductor Corporation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __REGION_DEFS_H__
#define __REGION_DEFS_H__

#include "flash_layout.h"

/* BL1_1 */
#define BL1_1_HEAP_SIZE         (0x0001000) /* 4KiB */
#define BL1_1_MSP_STACK_SIZE    (0x0001800) /* 6KiB */

/* BL1_2 */
#define BL1_2_HEAP_SIZE         (0x0001000) /* 4KiB */
#define BL1_2_MSP_STACK_SIZE    (0x0001800) /* 6KiB */

#define BL2_HEAP_SIZE           (0x0001000)
#define BL2_MSP_STACK_SIZE      (0x0001E00)

#ifdef ENABLE_HEAP
    #define S_HEAP_SIZE             (0x0000200)
#endif

#define S_MSP_STACK_SIZE        (0x0000800)

#define BOOT_TFM_SHARED_DATA_SIZE (0x400)

/* This size of buffer is big enough to store an attestation
 * token produced by initial attestation service
 */
#define PSA_INITIAL_ATTEST_TOKEN_MAX_SIZE   (0x250)

#define IMAGE_TFM_CODE_SIZE \
            (TFM_PARTITION_SIZE - BL2_HEADER_SIZE - BL2_TRAILER_SIZE)

#define IMAGE_BL2_CODE_SIZE \
            (SE_BL2_PARTITION_SIZE - BL1_HEADER_SIZE - BL1_TRAILER_SIZE)

/* Secure regions */
#define S_CODE_START            (SRAM_BASE + BL2_HEADER_SIZE)
#define S_CODE_SIZE             (IMAGE_TFM_CODE_SIZE)
#define S_CODE_LIMIT            (S_CODE_START + S_CODE_SIZE - 1)

#define S_DATA_START            (SRAM_BASE + TFM_PARTITION_SIZE)
#define S_DATA_SIZE             (SRAM_SIZE - TFM_PARTITION_SIZE)
#define S_UNPRIV_DATA_SIZE      (0x2160)
#define S_DATA_LIMIT            (S_DATA_START + S_DATA_SIZE - 1)
#define S_DATA_PRIV_START       (S_DATA_START + S_UNPRIV_DATA_SIZE)

/* OpenAMP shared memory region */
#define OPENAMP_SE_SHARED_MEMORY_START_ADDR 0xA8000000
#define OPENAMP_HOST_SHARED_MEMORY_START_ADDR 0x88000000
#define OPENAMP_SHARED_MEMORY_SIZE (1024 * 1024) /* 1MB */

#define NS_DATA_START OPENAMP_SE_SHARED_MEMORY_START_ADDR
#define NS_DATA_SIZE OPENAMP_SHARED_MEMORY_SIZE

#define S_CODE_VECTOR_TABLE_SIZE    (0xc0)

/* Stub NS macros needed for compilation */
#define NS_DATA_LIMIT   NS_DATA_START + NS_DATA_SIZE
#define NS_CODE_START   0x0
#define NS_CODE_SIZE    0x0
#define NS_CODE_LIMIT   0x0
#define NS_PARTITION_START 0
#define NS_PARTITION_SIZE (NS_DATA_SIZE)

/* Secondary partition for new images in case of firmware upgrade */
#define SECONDARY_PARTITION_START 0
#define SECONDARY_PARTITION_SIZE (TFM_PARTITION_SIZE)


/* SE BL2 regions */
#define BL2_IMAGE_START   (SRAM_BASE + SRAM_SIZE - SE_BL2_PARTITION_SIZE)
#define BL2_CODE_START    (BL2_IMAGE_START + BL1_HEADER_SIZE)
#define BL2_CODE_SIZE     (IMAGE_BL2_CODE_SIZE)
#define BL2_CODE_LIMIT    (BL2_CODE_START + BL2_CODE_SIZE - 1)

#define BL2_DATA_START    (BOOT_TFM_SHARED_DATA_BASE + \
                           BOOT_TFM_SHARED_DATA_SIZE)
#define BL2_DATA_SIZE     (BL2_CODE_START - BL2_HEADER_SIZE - BL2_DATA_START)
#define BL2_DATA_LIMIT    (BL2_DATA_START + BL2_DATA_SIZE - 1)

/* SE BL1 regions */
#define BL1_1_CODE_START    (0)
#define BL1_1_CODE_SIZE     (0x0000A000)     /* 40 KiB */
#define BL1_1_CODE_LIMIT    (BL1_1_CODE_START + BL1_1_CODE_SIZE - 1)

#define PROVISIONING_DATA_START (BL1_1_CODE_START + BL1_1_CODE_SIZE)
#define PROVISIONING_DATA_SIZE  (0x00002000)     /* 8 KiB */
#define PROVISIONING_DATA_LIMIT (PROVISIONING_DATA_START + PROVISIONING_DATA_SIZE - 1)

#define BL1_1_DATA_START    (SRAM_BASE)
#define BL1_1_DATA_SIZE     (0x8000)     /* 32 KiB*/
#define BL1_1_DATA_LIMIT    (BL1_1_DATA_START + BL1_1_DATA_SIZE - 1)

#define BL1_2_CODE_START    (BL1_1_DATA_START + BL1_1_DATA_SIZE)
#define BL1_2_CODE_SIZE     (0x00001000)     /* 4 KiB */
#define BL1_2_CODE_LIMIT    (BL1_2_CODE_START + BL1_2_CODE_SIZE - 1)

#define BL1_2_DATA_START    (BL1_2_CODE_START+BL1_2_CODE_SIZE)
#define BL1_2_DATA_SIZE     (0x8000)     /* 32 KiB*/
#define BL1_2_DATA_LIMIT    (BL1_2_DATA_START + BL1_2_DATA_SIZE - 1)

#define BOOT_TFM_SHARED_DATA_BASE (S_DATA_PRIV_START)

#define BOOT_TFM_SHARED_DATA_LIMIT (BOOT_TFM_SHARED_DATA_BASE + \
                                    BOOT_TFM_SHARED_DATA_SIZE - 1)

/* NSPE-to-SPE interrupt */
#define MAILBOX_IRQ HSE1_RECEIVER_COMBINED_IRQn

#endif /* __REGION_DEFS_H__ */

