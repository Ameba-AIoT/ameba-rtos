/*
 * Copyright (c) 2019-2023 Arm Limited. All rights reserved.
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

#ifndef __TARGET_CFG_H__
#define __TARGET_CFG_H__

#include <stdint.h>

#define TFM_DRIVER_STDIO    Driver_USART0
#define NS_DRIVER_STDIO     Driver_USART0

/**
 * \brief Defines the word offsets of Slave Peripheral Protection Controller
 *        Registers
 */
enum ppc_bank_e {
    PPC_SP_DO_NOT_CONFIGURE = -1,
    PPC_SP_AHB_PPC0     = 0,
    PPC_SP_AHB_PPC_EXP0 = 4,
    PPC_SP_APB_PPC0     = 8,
    PPC_SP_APB_PPC1     = 9,
    PPC_SP_APB_PPC_EXP0 = 12,
    PPC_SP_APB_PPC_EXP1 = 13,
    PPC_SP_APB_PPC_EXP2 = 14,
};

#endif /* __TARGET_CFG_H__ */
