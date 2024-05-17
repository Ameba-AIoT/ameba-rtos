/*
 * Copyright (c) 2016-2018 ARM Limited
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file platform_retarget.h
 * \brief This file defines all the peripheral base addresses for MPS2/SSE-200 platform.
 */

#ifndef __ARM_LTD_AN521_RETARGET_H__
#define __ARM_LTD_AN521_RETARGET_H__

#include "basic_types.h"

extern u8 DerivedKey[16];
extern u8 DerivedKey_Bkup[16];

#define FLASH0_SIZE                    0x00800000  /* 8 MB */
#define FLASH0_SECTOR_SIZE             0x00001000  /* 4 kB */
#define FLASH0_PAGE_SIZE               0x00000100  /* 256 B */
#define FLASH0_PROGRAM_UNIT            0x1         /* Minimum write size */

#endif  /* __ARM_LTD_AN521_RETARGET_H__ */
