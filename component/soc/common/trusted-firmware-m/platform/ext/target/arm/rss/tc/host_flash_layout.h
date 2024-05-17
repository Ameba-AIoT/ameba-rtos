/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
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

#ifndef __HOST_FLASH_LAYOUT_H__
#define __HOST_FLASH_LAYOUT_H__


/* Sizes of a images */
#define FLASH_BL2_PARTITION_SIZE        (0x18000) /* BL2 partition: 96 KiB */
#define FLASH_S_PARTITION_SIZE          (0x60000) /* S   partition: 384 KiB */
#define FLASH_NS_PARTITION_SIZE         (0x60000) /* NS  partition: 384 KiB */
#define FLASH_AP_PARTITION_SIZE         (0x80000) /* AP  partition: 512 KiB */
#define FLASH_SCP_PARTITION_SIZE        (0x80000) /* SCP partition: 512 KiB */

#define HOST_IMAGE_HEADER_SIZE          (0x2000)

#ifdef RSS_XIP
/* Each table contains a bit less that 8KiB of HTR and 2KiB of mcuboot headers.
 * The spare space in the 8KiB is used for decryption IVs.
 */
#define FLASH_SIC_TABLE_SIZE            (0x2800) /* 10KiB */
#endif /* RSS_XIP */

/* Sector size of the flash hardware; same as FLASH0_SECTOR_SIZE */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (0x1000)    /* 512 KiB */
/* Same as FLASH0_SIZE */
#define FLASH_TOTAL_SIZE                (0xFC00000)  /* 252 MiB */

#ifndef RSS_GPT_SUPPORT
#define FLASH_FIP_MAX_SIZE         0x800000 /* 8MiB */
#define FLASH_FIP_A_OFFSET         0x0
#define FLASH_FIP_B_OFFSET         (FLASH_FIP_A_OFFSET + FLASH_FIP_MAX_SIZE)
#else
#define FLASH_LBA_SIZE             0x200
#endif /* !RSS_GPT_SUPPORT */

/* Flash device name used by BL1 and BL2
 * Name is defined in flash driver file: Driver_Flash.c
 */
#define FLASH_DEV_NAME Driver_FLASH0
/* Smallest flash programmable unit in bytes */
#define TFM_HAL_FLASH_PROGRAM_UNIT      (0x1)

#endif /* __HOST_FLASH_LAYOUT_H__ */
