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

#ifndef __DRIVER_TGU_COMMON_H__
#define __DRIVER_TGU_COMMON_H__

#define ARM_DRIVER_TGU(TGU_NAME, TGU_DEV) \
static int32_t TGU_NAME##_Initialize() \
{ \
   return ARM_DRIVER_OK; \
} \
 \
static int32_t TGU_NAME##_Uninitialize(void) \
{ \
   /* Nothing to be done */ \
   return ARM_DRIVER_OK; \
} \
\
static int32_t TGU_NAME##_GetBlockSize(uint32_t *blk_size) \
{ \
   enum tgu_armv8_m_error_t ret; \
 \
   ret = tgu_armv8_m_get_block_size(&TGU_DEV, blk_size); \
 \
   return error_trans(ret); \
} \
 \
static int32_t TGU_NAME##_GetCtrlConfig(uint32_t *ctrl_val) \
{ \
   enum tgu_armv8_m_error_t ret; \
 \
   ret = tgu_armv8_m_get_ctrl(&TGU_DEV, ctrl_val); \
 \
   return error_trans(ret); \
} \
 \
static int32_t TGU_NAME##_SetCtrlConfig(uint32_t ctrl) \
{ \
   enum tgu_armv8_m_error_t ret; \
 \
   ret = tgu_armv8_m_set_ctrl(&TGU_DEV, ctrl); \
 \
   return error_trans(ret); \
} \
 \
static int32_t TGU_NAME##_GetRegionConfig(uintptr_t base, \
                                          uintptr_t limit, \
                                          ARM_MPC_SEC_ATTR *attr) \
{ \
   enum tgu_armv8_m_error_t ret; \
 \
   ret = tgu_armv8_m_get_region_config(&TGU_DEV, base, limit, \
                                    (enum tgu_armv8_m_sec_attr_t*)attr); \
 \
   return error_trans(ret); \
} \
 \
static int32_t TGU_NAME##_ConfigRegion(uintptr_t base, \
                                       uintptr_t limit, \
                                       ARM_MPC_SEC_ATTR attr) \
{ \
   enum tgu_armv8_m_error_t ret; \
 \
   ret = tgu_armv8_m_config_region(&TGU_DEV, base, limit, \
                                (enum tgu_armv8_m_sec_attr_t)attr); \
 \
   return error_trans(ret); \
} \
static int32_t TGU_NAME##_EnableInterrupt(void) \
{ \
   return ARM_DRIVER_ERROR_UNSUPPORTED; \
} \
 \
static void TGU_NAME##_DisableInterrupt(void) \
{ \
   /* Nothing to be done: Unsupported operation */ \
} \
 \
static void TGU_NAME##_ClearInterrupt(void) \
{ \
   /* Nothing to be done: Unsupported operation */ \
} \
 \
static uint32_t TGU_NAME##_InterruptState(void) \
{ \
   return ARM_DRIVER_ERROR_UNSUPPORTED; \
} \
 \
static int32_t TGU_NAME##_LockDown(void) \
{ \
   return ARM_DRIVER_ERROR_UNSUPPORTED; \
} \
 \
/* TGU_NAME Driver CMSIS access structure */ \
ARM_DRIVER_MPC Driver_##TGU_NAME = { \
    .GetVersion       = ARM_TGU_ARMV8_M_GetVersion, \
    .Initialize       = TGU_NAME##_Initialize, \
    .Uninitialize     = TGU_NAME##_Uninitialize, \
    .GetBlockSize     = TGU_NAME##_GetBlockSize, \
    .GetCtrlConfig    = TGU_NAME##_GetCtrlConfig, \
    .SetCtrlConfig    = TGU_NAME##_SetCtrlConfig, \
    .ConfigRegion     = TGU_NAME##_ConfigRegion, \
    .GetRegionConfig  = TGU_NAME##_GetRegionConfig, \
    .EnableInterrupt  = TGU_NAME##_EnableInterrupt, \
    .DisableInterrupt = TGU_NAME##_DisableInterrupt, \
    .ClearInterrupt   = TGU_NAME##_ClearInterrupt, \
    .InterruptState   = TGU_NAME##_InterruptState, \
    .LockDown         = TGU_NAME##_LockDown, \
};

#endif  /* __DRIVER_TGU_COMMON_H */
