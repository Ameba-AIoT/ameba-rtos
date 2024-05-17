/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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

#include "tgu_armv8_m_drv.h"
#include "Driver_MPC.h"
#include "Driver_TGU_Common.h"
#include "cmsis_driver_config.h"
#include "RTE_Device.h"

/* Driver version */
#define ARM_TGU_ARMV8_M_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

#if (defined (RTE_ITCM_TGU_ARMV8_M) && (RTE_ITCM_TGU_ARMV8_M == 1)) || \
    (defined (RTE_DTCM_TGU_ARMV8_M) && (RTE_DTCM_TGU_ARMV8_M == 1))

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_MPC_API_VERSION,
    ARM_TGU_ARMV8_M_DRV_VERSION
};

static ARM_DRIVER_VERSION ARM_TGU_ARMV8_M_GetVersion(void)
{
    return DriverVersion;
}

/*
 * \brief Translates error codes from native API to CMSIS API.
 *
 * \param[in] err  Error code to translate (\ref tgu_armv8_m_error_t).
 *
 * \return Returns CMSIS error code.
 */
static int32_t error_trans(enum tgu_armv8_m_error_t err)
{
    switch(err) {
    case TGU_ERR_NONE:
        return ARM_DRIVER_OK;
    case TGU_INVALID_ARG:
        return ARM_DRIVER_ERROR_PARAMETER;
    case TGU_NOT_INIT:
        return ARM_MPC_ERR_NOT_INIT;
    case TGU_INTERN_ERR_NOT_IN_RANGE:
        return ARM_MPC_ERR_NOT_IN_RANGE;
    case TGU_INTERN_ERR_NOT_ALIGNED:
        return ARM_MPC_ERR_NOT_ALIGNED;
    case TGU_INTERN_ERR_INVALID_RANGE:
        return ARM_MPC_ERR_INVALID_RANGE;
    case TGU_ERR_RANGE_SEC_ATTR_NON_COMPATIBLE:
        return ARM_MPC_ERR_RANGE_SEC_ATTR_NON_COMPATIBLE;
    default:
        return ARM_MPC_ERR_UNSPECIFIED;
    }
}

#if (defined (RTE_ITCM_TGU_ARMV8_M) && (RTE_ITCM_TGU_ARMV8_M == 1))
ARM_DRIVER_TGU(ITCM_TGU_ARMV8_M, TGU_ARMV8_M_ITCM_DEV)
#endif /* RTE_ITCM_TGU_ARMV8_M */

#if (defined (RTE_DTCM_TGU_ARMV8_M) && (RTE_DTCM_TGU_ARMV8_M == 1))
ARM_DRIVER_TGU(DTCM_TGU_ARMV8_M, TGU_ARMV8_M_DTCM_DEV)
#endif /* RTE_DTCM_TGU_ARMV8_M */

#endif
