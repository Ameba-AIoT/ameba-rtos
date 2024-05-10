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

#ifndef __DRIVER_FLASH_N25Q256A_H__
#define __DRIVER_FLASH_N25Q256A_H__

#include "Driver_Flash_Common.h"
#include "spi_n25q256a_flash_lib.h"

/*
 * ARM FLASH device structure
 */
struct arm_n25q256a_flash_dev_t {
    struct spi_n25q256a_dev_t *dev;         /*!< FLASH memory device structure */
    ARM_FLASH_INFO *data;                   /*!< FLASH data */
};

/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES N25Q256ADriverCapabilities = {
    0, /* event_ready */
    0, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    1  /* erase_chip */
};

static ARM_FLASH_INFO N25Q256A_DEV_DATA = {
    .sector_info    = NULL,     /* Uniform sector layout */
    .sector_count   = PMOD_SF3_FLASH_TOTAL_SIZE / PMOD_SF3_FLASH_SECTOR_SIZE,
    .sector_size    = PMOD_SF3_FLASH_SECTOR_SIZE,
    .page_size      = PMOD_SF3_FLASH_PAGE_SIZE,
    .program_unit   = PMOD_SF3_FLASH_PROGRAM_UNIT,
    .erased_value   = ARM_FLASH_DRV_ERASE_VALUE
};

static inline ARM_FLASH_CAPABILITIES N25Q256A_Driver_GetCapabilities(void)
{
    return N25Q256ADriverCapabilities;
}

/*
 * \brief Macro for N25Q256A Flash Driver
 *
 * \param[in]  FLASH_DEV          Native driver device
 *                                \ref arm_n25q256a_flash_dev_t
 * \param[out] FLASH_DRIVER_NAME  Resulting Driver name
 */
#define ARM_FLASH_N25Q256A(FLASH_DEV, FLASH_DRIVER_NAME)                      \
static int32_t FLASH_DRIVER_NAME##_Initialize(                                \
                                            ARM_Flash_SignalEvent_t cb_event) \
{                                                                             \
    ARG_UNUSED(cb_event);                                                     \
    enum n25q256a_error_t ret;                                                \
    struct spi_n25q256a_dev_t* dev = FLASH_DEV.dev;                           \
    ARM_FLASH_INFO* data = FLASH_DEV.data;                                    \
                                                                              \
    dev->total_sector_cnt = data->sector_count;                               \
    dev->page_size = data->page_size;                                         \
    dev->sector_size = data->sector_size;                                     \
    dev->program_unit = data->program_unit;                                   \
                                                                              \
    ret = spi_n25q256a_initialize(FLASH_DEV.dev);                             \
    if (ret != N25Q256A_ERR_NONE) {                                           \
        SPI_FLASH_LOG_MSG("%s: Initialization failed.\n\r", __func__);        \
        return ARM_DRIVER_ERROR;                                              \
    }                                                                         \
                                                                              \
    return ARM_DRIVER_OK;                                                     \
}                                                                             \
                                                                              \
static int32_t FLASH_DRIVER_NAME##_Uninitialize(void)                         \
{                                                                             \
    spi_n25q256a_uninitialize(FLASH_DEV.dev);                                 \
    return ARM_DRIVER_OK;                                                     \
}                                                                             \
                                                                              \
static int32_t FLASH_DRIVER_NAME##_ReadData(uint32_t addr,                    \
                                            void *data,                       \
                                            uint32_t cnt)                     \
{                                                                             \
    enum n25q256a_error_t ret;                                                \
                                                                              \
    if (N25Q256ADriverCapabilities.data_width > 2 ||                          \
        N25Q256ADriverCapabilities.data_width < 0)                            \
    {                                                                         \
        SPI_FLASH_LOG_MSG("%s: Incorrect data width selected: addr=0x%x\n\r", \
                          __func__,                                           \
                          addr);                                              \
        return ARM_DRIVER_ERROR;                                              \
    }                                                                         \
                                                                              \
    cnt *= data_width_byte[N25Q256ADriverCapabilities.data_width];            \
                                                                              \
    ret = spi_n25q256a_read(FLASH_DEV.dev, addr, (uint8_t*) data, cnt);       \
    if (ret != N25Q256A_ERR_NONE) {                                           \
        SPI_FLASH_LOG_MSG("%s: read failed: addr=0x%x, cnt=%u\n\r",           \
                          __func__,                                           \
                          addr,                                               \
                          cnt);                                               \
        return ARM_DRIVER_ERROR;                                              \
    }                                                                         \
                                                                              \
    cnt /= data_width_byte[N25Q256ADriverCapabilities.data_width];            \
                                                                              \
    return cnt;                                                               \
}                                                                             \
                                                                              \
static int32_t FLASH_DRIVER_NAME##_ProgramData(uint32_t addr,                 \
                                               const void *data,              \
                                               uint32_t cnt)                  \
{                                                                             \
    enum n25q256a_error_t ret;                                                \
                                                                              \
    if (N25Q256ADriverCapabilities.data_width > 2 ||                          \
        N25Q256ADriverCapabilities.data_width < 0)                            \
    {                                                                         \
        SPI_FLASH_LOG_MSG("%s: Incorrect data width selected: addr=0x%x\n\r", \
                          __func__,                                           \
                          addr);                                              \
        return ARM_DRIVER_ERROR;                                              \
    }                                                                         \
                                                                              \
    cnt *= data_width_byte[N25Q256ADriverCapabilities.data_width];            \
                                                                              \
    ret = spi_n25q256a_program(FLASH_DEV.dev, addr, (uint8_t*) data, cnt);    \
    if (ret != N25Q256A_ERR_NONE) {                                           \
        SPI_FLASH_LOG_MSG("%s: program failed: addr=0x%x, cnt=%u\n\r",        \
                          __func__,                                           \
                          addr,                                               \
                          cnt);                                               \
        return ARM_DRIVER_ERROR;                                              \
    }                                                                         \
                                                                              \
    cnt /= data_width_byte[N25Q256ADriverCapabilities.data_width];            \
                                                                              \
    return cnt;                                                               \
}                                                                             \
                                                                              \
static int32_t FLASH_DRIVER_NAME##_EraseSector(uint32_t addr)                 \
{                                                                             \
    enum n25q256a_error_t ret;                                                \
                                                                              \
    ret = spi_n25q256a_erase(FLASH_DEV.dev, addr);                            \
    if (ret != N25Q256A_ERR_NONE) {                                           \
        SPI_FLASH_LOG_MSG("%s: erase failed: addr=0x%x\n\r", __func__, addr); \
        return ARM_DRIVER_ERROR;                                              \
    }                                                                         \
                                                                              \
    return ARM_DRIVER_OK;                                                     \
}                                                                             \
                                                                              \
static int32_t FLASH_DRIVER_NAME##_EraseChip(void)                            \
{                                                                             \
    enum n25q256a_error_t ret;                                                \
                                                                              \
    ret = spi_n25q256a_erase_chip(FLASH_DEV.dev);                             \
    if (ret != N25Q256A_ERR_NONE) {                                           \
        SPI_FLASH_LOG_MSG("%s: erase chip failed\n\r", __func__);             \
        return ARM_DRIVER_ERROR;                                              \
    }                                                                         \
                                                                              \
    return ARM_DRIVER_OK;                                                     \
}                                                                             \
                                                                              \
static ARM_FLASH_INFO * FLASH_DRIVER_NAME##_GetInfo(void)                     \
{                                                                             \
    return FLASH_DEV.data;                                                    \
}                                                                             \
                                                                              \
ARM_DRIVER_FLASH FLASH_DRIVER_NAME = {                                        \
    ARM_Flash_GetVersion,                                                     \
    N25Q256A_Driver_GetCapabilities,                                          \
    FLASH_DRIVER_NAME##_Initialize,                                           \
    FLASH_DRIVER_NAME##_Uninitialize,                                         \
    ARM_Flash_PowerControl,                                                   \
    FLASH_DRIVER_NAME##_ReadData,                                             \
    FLASH_DRIVER_NAME##_ProgramData,                                          \
    FLASH_DRIVER_NAME##_EraseSector,                                          \
    FLASH_DRIVER_NAME##_EraseChip,                                            \
    ARM_Flash_GetStatus,                                                      \
    FLASH_DRIVER_NAME##_GetInfo                                               \
}

#endif /* __DRIVER_FLASH_N25Q256A_H__ */
