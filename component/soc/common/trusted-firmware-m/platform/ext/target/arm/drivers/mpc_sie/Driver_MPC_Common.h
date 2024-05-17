/*
 * Copyright (c) 2023 Arm Limited
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

#ifndef __DRIVER_MPC_COMMON_H__
#define __DRIVER_MPC_COMMON_H__

#include "Driver_MPC.h"

/* Driver version */
#define ARM_MPC_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0)

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_MPC_API_VERSION,
    ARM_MPC_DRV_VERSION
};

static inline ARM_DRIVER_VERSION ARM_MPC_GetVersion(void)
{
    return DriverVersion;
}

#endif /* __DRIVER_MPC_COMMON_H__ */
