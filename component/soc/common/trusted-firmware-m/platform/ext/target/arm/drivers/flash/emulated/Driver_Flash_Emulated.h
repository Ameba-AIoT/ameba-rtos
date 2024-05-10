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

#ifndef __DRIVER_FLASH_EMULATED_H__
#define __DRIVER_FLASH_EMULATED_H__

#include "Driver_Flash_Common.h"
#include "emulated_flash_drv.h"

/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES DriverCapabilities = {
    0, /* event_ready */
    0, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    1, /* erase_chip */
    0  /* reserved */
};

static inline ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities(void)
{
    return DriverCapabilities;
}

/*
 * \brief Macro for Emulated Flash Driver
 *
 * \param[in]  FLASH_DEV          Native driver device \ref emulated_flash_dev_t
 * \param[out] FLASH_DRIVER_NAME  Resulting Driver name
 */
#define ARM_FLASH_EMULATED(FLASH_DEV, FLASH_DRIVER_NAME)                                        \
static int32_t FLASH_DRIVER_NAME##_Initialize(ARM_Flash_SignalEvent_t cb_event)                 \
{                                                                                               \
    ARG_UNUSED(cb_event);                                                                       \
                                                                                                \
    if (DriverCapabilities.data_width >= DATA_WIDTH_ENUM_SIZE) {                                \
        return ARM_DRIVER_ERROR;                                                                \
    }                                                                                           \
                                                                                                \
    /* Nothing to be done */                                                                    \
    return ARM_DRIVER_OK;                                                                       \
}                                                                                               \
                                                                                                \
static int32_t FLASH_DRIVER_NAME##_ReadData(uint32_t addr, void *data, uint32_t cnt)            \
{                                                                                               \
    /* Conversion between data items and bytes */                                               \
    enum emulated_flash_error_t rc =                                                            \
            emulated_flash_read_data(&FLASH_DEV, addr, data,                                    \
                                     cnt * data_width_byte[DriverCapabilities.data_width]);     \
                                                                                                \
    if (EMULATED_FLASH_ERR_NONE == rc) {                                                        \
        return (int32_t) cnt;                                                                   \
    } else if(EMULATED_FLASH_ERR_INVALID_PARAM == rc) {                                         \
        return ARM_DRIVER_ERROR_PARAMETER;                                                      \
    } else {                                                                                    \
        return ARM_DRIVER_ERROR;                                                                \
    }                                                                                           \
}                                                                                               \
                                                                                                \
static int32_t FLASH_DRIVER_NAME##_ProgramData(uint32_t addr,                                   \
                                               const void *data,                                \
                                               uint32_t cnt)                                    \
{                                                                                               \
    /* Conversion between data items and bytes */                                               \
    enum emulated_flash_error_t rc =                                                            \
            emulated_flash_program_data(&FLASH_DEV, addr, data,                                 \
                                        cnt * data_width_byte[DriverCapabilities.data_width]);  \
                                                                                                \
    if (EMULATED_FLASH_ERR_NONE == rc) {                                                        \
        return (int32_t) cnt;                                                                   \
    } else if(EMULATED_FLASH_ERR_INVALID_PARAM == rc) {                                         \
        return ARM_DRIVER_ERROR_PARAMETER;                                                      \
    } else if(EMULATED_FLASH_NOT_READY == rc) {                                                 \
        return ARM_DRIVER_ERROR;                                                                \
    } else {                                                                                    \
        return ARM_DRIVER_ERROR;                                                                \
    }                                                                                           \
}                                                                                               \
                                                                                                \
static int32_t FLASH_DRIVER_NAME##_EraseSector(uint32_t addr)                                   \
{                                                                                               \
    enum emulated_flash_error_t rc = emulated_flash_erase_sector(&FLASH_DEV, addr);             \
                                                                                                \
    if (EMULATED_FLASH_ERR_NONE == rc) {                                                        \
        return ARM_DRIVER_OK;                                                                   \
    } else if(EMULATED_FLASH_ERR_INVALID_PARAM == rc) {                                         \
        return ARM_DRIVER_ERROR_PARAMETER;                                                      \
    } else {                                                                                    \
        return ARM_DRIVER_ERROR;                                                                \
    }                                                                                           \
}                                                                                               \
                                                                                                \
static int32_t FLASH_DRIVER_NAME##_EraseChip(void)                                              \
{                                                                                               \
    emulated_flash_erase_chip(&FLASH_DEV);                                                      \
    return ARM_DRIVER_OK;                                                                       \
}                                                                                               \
                                                                                                \
static ARM_FLASH_INFO * FLASH_DRIVER_NAME##_GetInfo(void)                                       \
{                                                                                               \
    return FLASH_DEV.data;                                                                      \
}                                                                                               \
                                                                                                \
ARM_DRIVER_FLASH FLASH_DRIVER_NAME = {                                                          \
    ARM_Flash_GetVersion,                                                                       \
    ARM_Flash_GetCapabilities,                                                                  \
    FLASH_DRIVER_NAME##_Initialize,                                                             \
    ARM_Flash_Uninitialize,                                                                     \
    ARM_Flash_PowerControl,                                                                     \
    FLASH_DRIVER_NAME##_ReadData,                                                               \
    FLASH_DRIVER_NAME##_ProgramData,                                                            \
    FLASH_DRIVER_NAME##_EraseSector,                                                            \
    FLASH_DRIVER_NAME##_EraseChip,                                                              \
    ARM_Flash_GetStatus,                                                                        \
    FLASH_DRIVER_NAME##_GetInfo                                                                 \
}                                                                                               \

#endif /* __DRIVER_FLASH_EMULATED_H__ */
