/*
 * Copyright (c) 2017-2023 Arm Limited. All rights reserved.
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

#ifndef __FLASH_LAYOUT_H__
#define __FLASH_LAYOUT_H__

/* This header file is included from linker scatter file as well, where only a
 * limited C constructs are allowed. Therefore it is not possible to include
 * here the platform_base_address.h to access flash related defines. To resolve
 * this some of the values are redefined here with different names, these are
 * marked with comment.
 */

#define MCUBOOT_MAX_IMG_SECTORS         (32)

/********************************/
/* Secure Enclave internal SRAM */
/********************************/
#define SRAM_BASE                       (0x30000000)
#define SRAM_SIZE                       (0x80000)     /* 512 KB */

#define BL2_DATA_GAP_SIZE               (0x09800)  /* 38 KB */


/*****************/
/***** Flash *****/
/*****************/

#define FLASH_BASE_ADDRESS              (0x68000000)

#if PLATFORM_IS_FVP
/* INTEL STRATA J3 NOR FLASH NVM */
#define STRATA_NVM_FLASH_TOTAL_SIZE     (0x04000000)  /* 32 MB Nor Flash (PMOD SF3) */
#define STRATA_NVM_FLASH_SECTOR_SIZE    (0x00001000)  /* 64 KB Sub sector size*/
#define STRATA_NVM_FLASH_PAGE_SIZE      (256U)        /* 64 KB */
#define STRATA_NVM_FLASH_PROGRAM_UNIT   (1U)          /* 4 B */

#define FLASH_DEV_NAME                  Driver_FLASH0
#define FLASH_TOTAL_SIZE                (STRATA_NVM_FLASH_TOTAL_SIZE)  /* 32 MB */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (STRATA_NVM_FLASH_SECTOR_SIZE)      /* 4 KiB */
#define FLASH_SECTOR_SIZE               (STRATA_NVM_FLASH_SECTOR_SIZE) /* 1 kB */
#define TFM_HAL_FLASH_PROGRAM_UNIT      (STRATA_NVM_FLASH_PROGRAM_UNIT)

#define STRATA_SE_FLASH_TOTAL_SIZE      (0x00800000)  /* 32 MB Nor Flash (PMOD SF3) */
#define STRATA_SE_FLASH_SECTOR_SIZE     (0x00001000)  /* 64 KB Sub sector size*/
#define STRATA_SE_FLASH_PAGE_SIZE       (256U)        /* 64 KB */
#define STRATA_SE_FLASH_PROGRAM_UNIT    (1U)          /* 4 B */

#define FLASH_DEV_NAME_SE_SECURE_FLASH  Driver_FLASH1
#define SECURE_FLASH_SECTOR_SIZE        STRATA_NVM_FLASH_SECTOR_SIZE

#else

/* PMOD SF3 NOR FLASH */
#define PMOD_SF3_FLASH_TOTAL_SIZE       (0x02000000)  /* 32 MB Nor Flash (PMOD SF3) */
#define PMOD_SF3_FLASH_SECTOR_SIZE      (0x00001000)  /* 4 KB Sub sector size*/
#define PMOD_SF3_FLASH_PAGE_SIZE        (256U)        /* 256 B */
#define PMOD_SF3_FLASH_PROGRAM_UNIT     (1U)          /* 1 B */

/* SST26VF064B NOR FLASH */
#define SST26VF064B_FLASH_TOTAL_SIZE    (0x00800000)  /* 8 MB Nor Flash (SST26VF064B) */
#define SST26VF064B_FLASH_SECTOR_SIZE   (0x00010000)  /* 64 KB Sub sector size*/
#define SST26VF064B_FLASH_PAGE_SIZE     (256U)        /* 256 B */
#define SST26VF064B_FLASH_PROGRAM_UNIT  (1U)          /* 1 B */

#define FLASH_DEV_NAME                  Driver_FLASH0

#define FLASH_TOTAL_SIZE                (PMOD_SF3_FLASH_TOTAL_SIZE)  /* 32 MB */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (PMOD_SF3_FLASH_SECTOR_SIZE) /* 4 KiB */
#define FLASH_SECTOR_SIZE               (PMOD_SF3_FLASH_SECTOR_SIZE) /* 1 kB */
#define TFM_HAL_FLASH_PROGRAM_UNIT      (PMOD_SF3_FLASH_PROGRAM_UNIT)

#define FLASH_DEV_NAME_SE_SECURE_FLASH  Driver_FLASH1
#define SECURE_FLASH_SECTOR_SIZE        SST26VF064B_FLASH_SECTOR_SIZE

#endif

/* Static Configurations of the Flash */
#define SE_BL2_PARTITION_SIZE           (0x18800)    /* 98 KB */
#define SE_BL2_BANK_0_OFFSET            (0x9000)  /* 72nd LBA */
#define SE_BL2_BANK_1_OFFSET            (0x1002000)  /* 32784th LBA */

/* Space in flash to store metadata and uefi variables */
#define FWU_METADATA_FLASH_DEV          (FLASH_DEV_NAME)
#define FWU_METADATA_FLASH_SECTOR_SIZE  (FLASH_SECTOR_SIZE)

#define FWU_METADATA_REPLICA_1_OFFSET   (0x5000)  /* 40th LBA */
#define FWU_METADATA_REPLICA_2_OFFSET   (FWU_METADATA_REPLICA_1_OFFSET + \
                                         FWU_METADATA_FLASH_SECTOR_SIZE)

#define FWU_PRIVATE_METADATA_REPLICA_1_OFFSET   (FWU_METADATA_REPLICA_2_OFFSET + \
                                                 FWU_METADATA_FLASH_SECTOR_SIZE)
#define FWU_PRIVATE_METADATA_REPLICA_2_OFFSET   (FWU_PRIVATE_METADATA_REPLICA_1_OFFSET + \
                                                 FWU_METADATA_FLASH_SECTOR_SIZE)

#define BANK_0_PARTITION_OFFSET         (SE_BL2_BANK_0_OFFSET + \
                                         SE_BL2_PARTITION_SIZE)
#define BANK_1_PARTITION_OFFSET         (SE_BL2_BANK_1_OFFSET + \
                                         SE_BL2_PARTITION_SIZE)

/* BL1: mcuboot flashmap configurations */
#define FLASH_AREA_8_ID                 (1)
#define FLASH_AREA_8_SIZE               (SE_BL2_PARTITION_SIZE)

#define FLASH_INVALID_ID                (0xFF)
#define FLASH_INVALID_OFFSET            (0xFFFFFFFF)
#define FLASH_INVALID_SIZE              (0xFFFFFFFF)

#define BL1_FLASH_AREA_IMAGE_PRIMARY(x)     (((x) == 0) ? FLASH_AREA_8_ID : \
                                                          255 )
#define BL1_FLASH_AREA_IMAGE_SECONDARY(x)   (((x) == 0) ? FLASH_INVALID_ID : \
                                                          255 )

#define BL1_FLASH_AREA_IMAGE_SCRATCH        255

/* FWU Configurations */
#define NR_OF_FW_BANKS                  (2)
#define NR_OF_IMAGES_IN_FW_BANK         (4) /* Secure Enclave: BL2 and TF-M \
                                             * Host: FIP and Kernel image
                                             */

/* Bank configurations */
#define BANK_PARTITION_SIZE             (0xFE0000)   /* 15.875 MB */
#define TFM_PARTITION_SIZE              (0x5E000)    /* 376 KB */

/************************************************************/
/* Bank : Images flash offsets are with respect to the bank */
/************************************************************/

/* Image 0: BL2 primary and secondary images */
#define FLASH_AREA_0_ID                 (1)
#define FLASH_AREA_0_OFFSET             (0) /* starting from 0th offset of the bank */
#define FLASH_AREA_0_SIZE               (SE_BL2_PARTITION_SIZE)

#define FLASH_AREA_1_ID                 (FLASH_AREA_0_ID + 1)
#define FLASH_AREA_1_OFFSET             (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE)
#define FLASH_AREA_1_SIZE               (SE_BL2_PARTITION_SIZE)

/* Image 1: TF-M primary and secondary images */
#define FLASH_AREA_2_ID                 (1)
#define FLASH_AREA_2_SIZE               (TFM_PARTITION_SIZE)
#define FLASH_AREA_3_ID                 (FLASH_AREA_2_ID + 1)
#define FLASH_AREA_3_SIZE               (TFM_PARTITION_SIZE)

/* Image 2: Host FIP */
#define FIP_SIGNATURE_AREA_SIZE         (0x1000)      /* 4 KB */

/* Host BL2 (TF-A) primary and secondary image. */
#define FLASH_AREA_4_ID                 (FLASH_AREA_3_ID + 1)
#define FLASH_AREA_5_ID                 (FLASH_AREA_4_ID + 1)

#define BL1_FLASH_AREA_IMAGE_PRIMARY(x)     (((x) == 0) ? FLASH_AREA_0_ID : \
                                                          255 )
#define BL1_FLASH_AREA_IMAGE_SECONDARY(x)   (((x) == 0) ? FLASH_AREA_1_ID : \
                                                          255 )

#define BL1_FLASH_AREA_IMAGE_SCRATCH        255

/* Macros needed to imgtool.py, used when creating TF-M signed image */
#define S_IMAGE_LOAD_ADDRESS            (SRAM_BASE)
#define SECURE_IMAGE_OFFSET             (0x0)
#define SECURE_IMAGE_MAX_SIZE           (TFM_PARTITION_SIZE)
#define NON_SECURE_IMAGE_OFFSET         (TFM_PARTITION_SIZE)
#define NON_SECURE_IMAGE_MAX_SIZE       (0x0)

#define FLASH_AREA_IMAGE_PRIMARY(x)     (((x) == 0) ? FLASH_AREA_2_ID : \
                                         ((x) == 1) ? FLASH_AREA_4_ID : \
                                                      255 )
#define FLASH_AREA_IMAGE_SECONDARY(x)   (((x) == 0) ? FLASH_AREA_3_ID : \
                                         ((x) == 1) ? FLASH_AREA_5_ID : \
                                                      255 )

#define FLASH_AREA_IMAGE_SCRATCH        255

/*******************************/
/*** ITS, PS and NV Counters ***/
/*******************************/

#define FLASH_ITS_AREA_OFFSET           (0x10000)  /* 64 KB */
#define FLASH_ITS_AREA_SIZE             (4 * SECURE_FLASH_SECTOR_SIZE)

#define FLASH_PS_AREA_OFFSET            (FLASH_ITS_AREA_OFFSET + \
                                         FLASH_ITS_AREA_SIZE)
#define FLASH_PS_AREA_SIZE              (16 * SECURE_FLASH_SECTOR_SIZE)

/* OTP_definitions */
#define FLASH_OTP_NV_COUNTERS_AREA_OFFSET (FLASH_PS_AREA_OFFSET + \
                                           FLASH_PS_AREA_SIZE)
#define FLASH_OTP_NV_COUNTERS_AREA_SIZE   (SECURE_FLASH_SECTOR_SIZE * 2)
#define FLASH_OTP_NV_COUNTERS_SECTOR_SIZE SECURE_FLASH_SECTOR_SIZE

/* Internal Trusted Storage (ITS) Service definitions
 * Note: Further documentation of these definitions can be found in the
 * TF-M ITS Integration Guide.
 */
#define TFM_HAL_ITS_FLASH_DRIVER FLASH_DEV_NAME_SE_SECURE_FLASH

/* Protected Storage (PS) Service definitions
 * Note: Further documentation of these definitions can be found in the
 * TF-M PS Integration Guide.
 */
#define TFM_HAL_PS_FLASH_DRIVER FLASH_DEV_NAME_SE_SECURE_FLASH

/* In this target the CMSIS driver requires only the offset from the base
 * address instead of the full memory address.
 */
/* Base address of dedicated flash area for ITS */
#define TFM_HAL_ITS_FLASH_AREA_ADDR     FLASH_ITS_AREA_OFFSET
/* Size of dedicated flash area for ITS */
#define TFM_HAL_ITS_FLASH_AREA_SIZE     FLASH_ITS_AREA_SIZE
/* Number of physical erase sectors per logical FS block */
#define TFM_HAL_ITS_SECTORS_PER_BLOCK   (1)
/* Smallest flash programmable unit in bytes */
#define TFM_HAL_ITS_PROGRAM_UNIT        (1)

/* In this target the CMSIS driver requires only the offset from the base
 * address instead of the full memory address.
 */
/* Base address of dedicated flash area for PS */
#define TFM_HAL_PS_FLASH_AREA_ADDR      FLASH_PS_AREA_OFFSET
/* Size of dedicated flash area for PS */
#define TFM_HAL_PS_FLASH_AREA_SIZE      FLASH_PS_AREA_SIZE
/* Number of physical erase sectors per logical FS block */
#define TFM_HAL_PS_SECTORS_PER_BLOCK    (4)
/* Smallest flash programmable unit in bytes */
#define TFM_HAL_PS_PROGRAM_UNIT         (1)

#define OTP_NV_COUNTERS_FLASH_DEV FLASH_DEV_NAME_SE_SECURE_FLASH

/* OTP / NV counter definitions */
#define TFM_OTP_NV_COUNTERS_AREA_SIZE   (FLASH_OTP_NV_COUNTERS_AREA_SIZE / 2)
#define TFM_OTP_NV_COUNTERS_AREA_ADDR   FLASH_OTP_NV_COUNTERS_AREA_OFFSET
#define TFM_OTP_NV_COUNTERS_SECTOR_SIZE FLASH_OTP_NV_COUNTERS_SECTOR_SIZE
#define TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR (TFM_OTP_NV_COUNTERS_AREA_ADDR + \
                                              TFM_OTP_NV_COUNTERS_AREA_SIZE)
#endif /* __FLASH_LAYOUT_H__ */
