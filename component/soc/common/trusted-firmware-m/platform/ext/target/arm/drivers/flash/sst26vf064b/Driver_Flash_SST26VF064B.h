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

#ifndef __DRIVER_FLASH_SST26VF064B_H__
#define __DRIVER_FLASH_SST26VF064B_H__

#include "cmsis_compiler.h"
#include "Driver_Flash_Common.h"
#include "spi_sst26vf064b_flash_lib.h"

/*
 * ARM FLASH device structure
 */
struct arm_flash_sst26vf064b_flash_dev_t {
    struct spi_sst26vf064b_dev_t* dev;   /*!< FLASH memory device structure */
    ARM_FLASH_INFO* data;                /*!< FLASH data */
    int8_t (*setup_qspi)(struct arm_flash_sst26vf064b_flash_dev_t* dev);
                                         /*!< Setup flash for QSPI access */
    int8_t (*release_qspi)(struct arm_flash_sst26vf064b_flash_dev_t* dev);
                                         /*!< Setup flash for XIP access */
    const uint32_t memory_base_s;        /*!< FLASH memory base address in
                                          *   XIP mode, secure alias */
    const uint32_t memory_base_ns;       /*!< FLASH memory base address in
                                          *   XIP mode, non-secure alias */
};
/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES SST26VF064BDriverCapabilities = {
    0, /* event_ready */
    0, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    1, /* erase_chip */
    0, /* reserved */
};

static ARM_FLASH_INFO SST26VF064B_DEV_DATA = {
    .sector_info    = NULL,     /* Uniform sector layout */
    .sector_count   = SST26VF064B_FLASH_TOTAL_SIZE /
                        SST26VF064B_FLASH_SECTOR_SIZE,
    .sector_size    = SST26VF064B_FLASH_SECTOR_SIZE,
    .page_size      = SST26VF064B_FLASH_PAGE_SIZE,
    .program_unit   = SST26VF064B_FLASH_PROGRAM_UNIT,
    .erased_value   = ARM_FLASH_DRV_ERASE_VALUE
};

__STATIC_INLINE ARM_FLASH_CAPABILITIES SST26VF064B_Driver_GetCapabilities(void)
{
    return SST26VF064BDriverCapabilities;
}

#define SETUP_QSPI(flash_dev)                                            \
do {                                                                     \
    if(flash_dev.setup_qspi != NULL) {                                   \
        if(flash_dev.setup_qspi(&flash_dev) != 0) {                      \
            ret = SST26VF064B_ERR_QSPI_SETUP;                            \
            goto cleanup;                                                \
        }                                                                \
    }                                                                    \
} while(0)

#define RELEASE_QSPI(flash_dev)                                          \
do {                                                                     \
    if(flash_dev.setup_qspi != NULL) {                                   \
        if(flash_dev.release_qspi(&flash_dev) != 0) {                    \
            /* Should never get there */                                 \
            __ASM("B .");                                                \
        }                                                                \
    }                                                                    \
} while(0)

/*
 * \brief Macro for SST26VF064B Flash Driver
 *
 * \param[in]  FLASH_DEV          Native driver device
 *                                \ref arm_flash_sst26vf064b_flash_dev_t
 * \param[out] FLASH_DRIVER_NAME  Resulting Driver name
 */
#define ARM_FLASH_SST26VF064B(FLASH_DEV, FLASH_DRIVER_NAME)                   \
static int32_t FLASH_DRIVER_NAME##_Initialize(                                \
                                            ARM_Flash_SignalEvent_t cb_event) \
{                                                                             \
    ARG_UNUSED(cb_event);                                                     \
    enum sst26vf064b_error_t ret = ARM_DRIVER_OK;                             \
    struct spi_sst26vf064b_dev_t* dev = FLASH_DEV.dev;                        \
    ARM_FLASH_INFO* data = FLASH_DEV.data;                                    \
                                                                              \
    dev->total_sector_cnt = data->sector_count;                               \
    dev->page_size = data->page_size;                                         \
    dev->sector_size = data->sector_size;                                     \
    dev->program_unit = data->program_unit;                                   \
                                                                              \
    SETUP_QSPI(FLASH_DEV);                                                    \
    ret = spi_sst26vf064b_initialize(FLASH_DEV.dev);                          \
    if (ret != SST26VF064B_ERR_NONE) {                                        \
        SPI_FLASH_LOG_MSG("%s: Initialization failed.\n\r", __func__);        \
        goto cleanup;                                                         \
    }                                                                         \
                                                                              \
cleanup:                                                                      \
    RELEASE_QSPI(FLASH_DEV);                                                  \
    if (ret != SST26VF064B_ERR_NONE) {                                        \
        return ARM_DRIVER_ERROR;                                              \
    }                                                                         \
    return ARM_DRIVER_OK;                                                     \
}                                                                             \
                                                                              \
static int32_t FLASH_DRIVER_NAME##_ReadData(uint32_t addr,                    \
                                            void *data,                       \
                                            uint32_t cnt)                     \
{                                                                             \
    enum sst26vf064b_error_t ret;                                             \
                                                                              \
    if (SST26VF064BDriverCapabilities.data_width > 2 ||                       \
        SST26VF064BDriverCapabilities.data_width < 0)                         \
    {                                                                         \
        SPI_FLASH_LOG_MSG("%s: Incorrect data width selected: addr=0x%x\n\r", \
                          __func__, addr);                                    \
        ret = SST26VF064B_ERR_WRONG_ARGUMENT;                                 \
        goto cleanup;                                                         \
    }                                                                         \
                                                                              \
    cnt *= data_width_byte[SST26VF064BDriverCapabilities.data_width];         \
                                                                              \
    SETUP_QSPI(FLASH_DEV);                                                    \
    ret = spi_sst26vf064b_read(FLASH_DEV.dev, addr, (uint8_t*) data, cnt);    \
    if (ret != SST26VF064B_ERR_NONE) {                                        \
        SPI_FLASH_LOG_MSG("%s: read failed: addr=0x%x, cnt=%u\n\r",           \
                          __func__, addr, cnt);                               \
        goto cleanup;                                                         \
    }                                                                         \
                                                                              \
    cnt /= data_width_byte[SST26VF064BDriverCapabilities.data_width];         \
                                                                              \
cleanup:                                                                      \
    RELEASE_QSPI(FLASH_DEV);                                                  \
    if (ret != SST26VF064B_ERR_NONE) {                                        \
        return ARM_DRIVER_ERROR;                                              \
    }                                                                         \
    return cnt;                                                               \
}                                                                             \
                                                                              \
static int32_t FLASH_DRIVER_NAME##_ProgramData(uint32_t addr,                 \
                                               const void *data,              \
                                               uint32_t cnt)                  \
{                                                                             \
    enum sst26vf064b_error_t ret;                                             \
                                                                              \
    if (SST26VF064BDriverCapabilities.data_width > 2 ||                       \
        SST26VF064BDriverCapabilities.data_width < 0)                         \
    {                                                                         \
        SPI_FLASH_LOG_MSG("%s: Incorrect data width selected: addr=0x%x\n\r", \
                          __func__, addr);                                    \
        ret = SST26VF064B_ERR_WRONG_ARGUMENT;                                 \
        goto cleanup;                                                         \
    }                                                                         \
                                                                              \
    cnt *= data_width_byte[SST26VF064BDriverCapabilities.data_width];         \
                                                                              \
    SETUP_QSPI(FLASH_DEV);                                                    \
    ret = spi_sst26vf064b_program(FLASH_DEV.dev, addr, (uint8_t*) data, cnt); \
    if (ret != SST26VF064B_ERR_NONE) {                                        \
        SPI_FLASH_LOG_MSG("%s: program failed: addr=0x%x, cnt=%u\n\r",        \
                          __func__, addr, cnt);                               \
        goto cleanup;                                                         \
    }                                                                         \
                                                                              \
    cnt /= data_width_byte[SST26VF064BDriverCapabilities.data_width];         \
                                                                              \
cleanup:                                                                      \
    RELEASE_QSPI(FLASH_DEV);                                                  \
    if (ret != SST26VF064B_ERR_NONE) {                                        \
        return ARM_DRIVER_ERROR;                                              \
    }                                                                         \
    return cnt;                                                               \
}                                                                             \
                                                                              \
static int32_t FLASH_DRIVER_NAME##_EraseSector(uint32_t addr)                 \
{                                                                             \
    enum sst26vf064b_error_t ret;                                             \
                                                                              \
    SETUP_QSPI(FLASH_DEV);                                                    \
    ret = spi_sst26vf064b_erase_sector(FLASH_DEV.dev, addr);                  \
    if (ret != SST26VF064B_ERR_NONE) {                                        \
        SPI_FLASH_LOG_MSG("%s: erase failed: addr=0x%x\n\r", __func__, addr); \
        goto cleanup;                                                         \
    }                                                                         \
                                                                              \
cleanup:                                                                      \
    RELEASE_QSPI(FLASH_DEV);                                                  \
    if (ret != SST26VF064B_ERR_NONE) {                                        \
        return ARM_DRIVER_ERROR;                                              \
    }                                                                         \
    return ARM_DRIVER_OK;                                                     \
}                                                                             \
                                                                              \
static int32_t FLASH_DRIVER_NAME##_EraseChip(void)                            \
{                                                                             \
    enum sst26vf064b_error_t ret;                                             \
                                                                              \
    SETUP_QSPI(FLASH_DEV);                                                    \
    ret = spi_sst26vf064b_erase_chip(FLASH_DEV.dev);                          \
    if (ret != SST26VF064B_ERR_NONE) {                                        \
        SPI_FLASH_LOG_MSG("%s: erase chip failed\n\r", __func__);             \
        goto cleanup;                                                         \
    }                                                                         \
                                                                              \
cleanup:                                                                      \
    RELEASE_QSPI(FLASH_DEV);                                                  \
    if (ret != SST26VF064B_ERR_NONE) {                                        \
        return ARM_DRIVER_ERROR;                                              \
    }                                                                         \
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
    SST26VF064B_Driver_GetCapabilities,                                       \
    FLASH_DRIVER_NAME##_Initialize,                                           \
    ARM_Flash_Uninitialize,                                                   \
    ARM_Flash_PowerControl,                                                   \
    FLASH_DRIVER_NAME##_ReadData,                                             \
    FLASH_DRIVER_NAME##_ProgramData,                                          \
    FLASH_DRIVER_NAME##_EraseSector,                                          \
    FLASH_DRIVER_NAME##_EraseChip,                                            \
    ARM_Flash_GetStatus,                                                      \
    FLASH_DRIVER_NAME##_GetInfo                                               \
}

#endif /* __DRIVER_FLASH_SST26VF064B_H__ */
