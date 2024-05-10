/*
 *
 * Copyright (c) 2021-2023, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef SPI_SST26VF064B_FLASH_LIB_H
#define SPI_SST26VF064B_FLASH_LIB_H

#include "xilinx_pg153_axi_qspi_controller_drv.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SST26VF064B NOR FLASH */
#define SST26VF064B_FLASH_TOTAL_SIZE    (0x00800000)  /* 8 MB Nor Flash (SST26VF064B) */
#define SST26VF064B_FLASH_SECTOR_SIZE   (0x00010000)  /* 64 KB Sector size*/
#define SST26VF064B_FLASH_PAGE_SIZE     (256U)        /* 256 B */
#define SST26VF064B_FLASH_PROGRAM_UNIT  (1U)          /* 1 B */

/**
 * \brief SPI Flash error enumeration types
 */
enum sst26vf064b_error_t {
    SST26VF064B_ERR_NONE = AXI_QSPI_ERR_NONE,
    SST26VF064B_ERR_WRONG_ARGUMENT = AXI_QSPI_ERR_WRONG_ARGUMENT,
    SST26VF064B_ERR_NOT_INITIALIZED = AXI_QSPI_ERR_NOT_INITIALIZED,
    SST26VF064B_ERR_WRONG_MEMORY,
    SST26VF064B_ERR_FLASH_CMD_FAILED,
    SST26VF064B_ERR_READ_IN_PROGRESS,
    SST26VF064B_ERR_WRITE_IN_PROGRESS,
    SST26VF064B_ERR_QSPI_SETUP
};

struct spi_sst26vf064b_dev_t {
    struct axi_qspi_dev_t *controller; /* QSPI Flash Controller */
    uint32_t total_sector_cnt;
    uint32_t page_size;
    uint32_t sector_size;
    uint32_t program_unit;
    bool is_initialized;
};

enum sst26vf064b_error_t spi_sst26vf064b_initialize(struct spi_sst26vf064b_dev_t* dev);

void spi_sst26vf064b_uninitialize(struct spi_sst26vf064b_dev_t* dev);

enum sst26vf064b_error_t spi_sst26vf064b_erase_chip(struct spi_sst26vf064b_dev_t* dev);

enum sst26vf064b_error_t spi_sst26vf064b_erase_sector(struct spi_sst26vf064b_dev_t* dev,
                                         uint32_t addr);

enum sst26vf064b_error_t spi_sst26vf064b_program(struct spi_sst26vf064b_dev_t* dev,
                                           uint32_t addr,
                                           const uint8_t *data, uint32_t cnt);

enum sst26vf064b_error_t spi_sst26vf064b_read(struct spi_sst26vf064b_dev_t* dev,
                                        uint32_t addr,
                                        uint8_t *data, uint32_t cnt);
#ifdef __cplusplus
}
#endif

#endif /* SPI_SST26VF064B_FLASH_LIB_H */
