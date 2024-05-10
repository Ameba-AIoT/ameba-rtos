/*
 * Copyright (c) 2021-2023 Arm Limited. All rights reserved.
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

#include "atu_rss_drv.h"

#include <stddef.h>

#define ATU_GET_ATUPS(atu_base) (                                     \
    (uint8_t)(((atu_base)->atubc & ATU_ATUBC_PS_MASK) >> ATU_ATUBC_PS_OFF))

#define ATU_ATUBC_PS_OFF                4u
    /*!< ATU Build Configuration Register Page Size bit field offset */
#define ATU_ATUBC_PS_MASK               (0xFu << ATU_ATUBC_PS_OFF)
    /*!< ATU Build Configuration Register Page Size bit field mask */
#define ATU_ATUBC_RC_OFF                0u
    /*!< ATU Build Configuration Register Region Count bit field offset */
#define ATU_ATUBC_RC_MASK               (0x7u << ATU_ATUBC_RC_OFF)
    /*!< ATU Build Configuration Register Region Count bit field mask */
#define ATU_ATUIS_ME_OFF                0u
    /*!< ATU Interrupt Status Register Mismatch Error bit field offset */
#define ATU_ATUIS_ME_MASK               (0x1u << ATU_ATUIS_ME_OFF)
    /*!< ATU Interrupt Status Register Mismatch Error bit field mask */
#define ATU_ATUIE_ME_OFF                0u
    /*!< ATU Interrupt Enable Register Mismatch Error bit field offset */
#define ATU_ATUIE_ME_MASK               (0x1u << ATU_ATUIE_ME_OFF)
    /*!< ATU Interrupt Enable Register Mismatch Error bit field mask */
#define ATU_ATUIC_ME_OFF                0u
    /*!< ATU Interrupt Clear Register Mismatch Error bit field offset */
#define ATU_ATUIC_ME_MASK               (0x1u << ATU_ATUIC_ME_OFF)
    /*!< ATU Interrupt Clear Register Mismatch Error bit field mask */
#define ATU_ATUROBA_AXNSE_OFF           14u
    /*!< ATU ROBA Register AxNSE bit field offset */
#define ATU_ATUROBA_AXNSE_MASK          (0x3u << ATU_ATUROBA_AXNSE_OFF)
    /*!< ATU ROBA Register AxNSE bit field mask */
#define ATU_ATUROBA_AXCACHE3_OFF        12u
    /*!< ATU ROBA Register AxCACHE3 bit field offset */
#define ATU_ATUROBA_AXCACHE3_MASK       (0x3u << ATU_ATUROBA_AXCACHE3_OFF)
    /*!< ATU ROBA Register AxCACHE3 bit field mask */
#define ATU_ATUROBA_AXCACHE2_OFF        10u
    /*!< ATU ROBA Register AxCACHE2 bit field offset */
#define ATU_ATUROBA_AXCACHE2_MASK       (0x3u << ATU_ATUROBA_AXCACHE2_OFF)
    /*!< ATU ROBA Register AxCACHE2 bit field mask */
#define ATU_ATUROBA_AXCACHE1_OFF        8u
    /*!< ATU ROBA Register AxCACHE1 bit field offset */
#define ATU_ATUROBA_AXCACHE1_MASK       (0x3u << ATU_ATUROBA_AXCACHE1_OFF)
    /*!< ATU ROBA Register AxCACHE1 bit field mask */
#define ATU_ATUROBA_AXCACHE0_OFF        6u
    /*!< ATU ROBA Register AxCACHE0 bit field offset */
#define ATU_ATUROBA_AXCACHE0_MASK       (0x3u << ATU_ATUROBA_AXCACHE0_OFF)
    /*!< ATU ROBA Register AxCACHE0 bit field mask */
#define ATU_ATUROBA_AXPROT2_OFF         4u
    /*!< ATU ROBA Register AxPROT2 bit field offset */
#define ATU_ATUROBA_AXPROT2_MASK        (0x3u << ATU_ATUROBA_AXPROT2_OFF)
    /*!< ATU ROBA Register AxPROT2 bit field mask */
#define ATU_ATUROBA_AXPROT1_OFF         2u
    /*!< ATU ROBA Register AxPROT1 bit field offset */
#define ATU_ATUROBA_AXPROT1_MASK        (0x3u << ATU_ATUROBA_AXPROT1_OFF)
    /*!< ATU ROBA Register AxPROT1 bit field mask */
#define ATU_ATUROBA_AXPROT0_OFF         0u
    /*!< ATU ROBA Register AxPROT0 bit field offset */
#define ATU_ATUROBA_AXPROT0_MASK        (0x3u << ATU_ATUROBA_AXPROT0_OFF)
    /*!< ATU ROBA Register AxPROT0 bit field mask */

/**
 * \brief ATU register map structure
 */
struct _atu_reg_map_t {
    volatile uint32_t atubc;
                /*!< Offset: 0x000 (R/ ) ATU Build Configuration Register */
    volatile uint32_t atuc;
                /*!< Offset: 0x004 (R/W) ATU Configuration Register */
    volatile uint32_t atuis;
                /*!< Offset: 0x008 (R/ ) ATU Interrupt Status Register */
    volatile uint32_t atuie;
                /*!< Offset: 0x00C (R/W) ATU Interrupt Enable Register */
    volatile uint32_t atuic;
                /*!< Offset: 0x010 (R/W) ATU Interrupt Clear Register */
    volatile uint32_t atuma;
                /*!< Offset: 0x014 (R/ ) ATU Mismatched Address Register */
    volatile uint32_t reserved_0[2];   /*!< Offset: 0x018-0x01C Reserved */
    volatile uint32_t atursla[32];
                /*!< Offset: 0x020 (R/W) ATU Region Start Logical Address n
                 *                       Register */
    volatile uint32_t aturela[32];
                /*!< Offset: 0x0A0 (R/W) ATU Region End Logical Address n
                 *                       Register */
    volatile uint32_t aturav_l[32];
                /*!< Offset: 0x120 (R/W) ATU Region Add Value Low n Register */
    volatile uint32_t aturav_m[32];
                /*!< Offset: 0x1A0 (R/W) ATU Region Add Value High n Register */
    volatile uint32_t aturoba[32];
                /*!< Offset: 0x220 (R/W) ATU Region Output Bus Attributes n
                 *                       Register */
    volatile uint32_t aturgp[32];
                /*!< Offset: 0x2A0 (R/W) ATU Region General Purpose n
                 *                       Register */
    volatile uint32_t reserved_1[811]; /*!< Offset: 0x320-0xFCC Reserved */
    volatile uint32_t pidr4;
                /*!< Offset: 0xFD0 (R/ ) Peripheral ID 4 */
    volatile uint32_t reserved_2[2];   /*!< Offset: 0xFD4-0xFDC Reserved */
    volatile uint32_t pidr0;
                /*!< Offset: 0xFE0 (R/ ) Peripheral ID 0 */
    volatile uint32_t pidr1;
                /*!< Offset: 0xFE4 (R/ ) Peripheral ID 1 */
    volatile uint32_t pidr2;
                /*!< Offset: 0xFE8 (R/ ) Peripheral ID 2 */
    volatile uint32_t pidr3;
                /*!< Offset: 0xFEC (R/ ) Peripheral ID 3 */
    volatile uint32_t cidr0;
                /*!< Offset: 0xFF0 (R/ ) Component ID 0 */
    volatile uint32_t cidr1;
                /*!< Offset: 0xFF4 (R/ ) Component ID 1 */
    volatile uint32_t cidr2;
                /*!< Offset: 0xFF8 (R/ ) Component ID 2 */
    volatile uint32_t cidr3;
                /*!< Offset: 0xFFC (R/ ) Component ID 3 */
};

static enum atu_error_t _set_bus_attribute(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region, uint8_t shift)
{
    struct _atu_reg_map_t* p_atu = (struct _atu_reg_map_t*)dev->cfg->base;

    if(region >= get_supported_region_count(dev))
        return ATU_ERR_INVALID_REGION;

    p_atu->aturoba[region] = ((p_atu->aturoba[region] & ~(0x3u << shift)) |
                              (val << shift));

    return ATU_ERR_NONE;
}

uint16_t get_page_size(struct atu_dev_t* dev)
{
    struct _atu_reg_map_t* p_atu = (struct _atu_reg_map_t*)dev->cfg->base;

    return (uint16_t)(0x1u << ATU_GET_ATUPS(p_atu));
}

uint8_t get_supported_region_count(struct atu_dev_t* dev)
{
    struct _atu_reg_map_t* p_atu = (struct _atu_reg_map_t*)dev->cfg->base;

    return (uint8_t)(0x1u << (p_atu->atubc & ATU_ATUBC_RC_MASK));
}

enum atu_error_t enable_atu_region(struct atu_dev_t* dev, uint8_t region)
{
    struct _atu_reg_map_t* p_atu = (struct _atu_reg_map_t*)dev->cfg->base;

    if(region >= get_supported_region_count(dev))
        return ATU_ERR_INVALID_REGION;

    p_atu->atuc |= (1u << region);

    return ATU_ERR_NONE;
}

enum atu_error_t disable_atu_region(struct atu_dev_t* dev, uint8_t region)
{
    struct _atu_reg_map_t* p_atu = (struct _atu_reg_map_t*)dev->cfg->base;

    if(region >= get_supported_region_count(dev))
        return ATU_ERR_INVALID_REGION;

    p_atu->atuc &= ~(1u << region);

    return ATU_ERR_NONE;
}

bool me_interrupt_is_waiting(struct atu_dev_t* dev)
{
    struct _atu_reg_map_t* p_atu = (struct _atu_reg_map_t*)dev->cfg->base;

    return (bool)(p_atu->atuis & ATU_ATUIS_ME_MASK);
}

void enable_me_interrupt(struct atu_dev_t* dev)
{
    struct _atu_reg_map_t* p_atu = (struct _atu_reg_map_t*)dev->cfg->base;

    p_atu->atuie |= ATU_ATUIE_ME_MASK;
}

void clear_me_interrupt(struct atu_dev_t* dev)
{
    struct _atu_reg_map_t* p_atu = (struct _atu_reg_map_t*)dev->cfg->base;

    p_atu->atuic |= ATU_ATUIC_ME_MASK;
}

uint32_t get_mismatch_address(struct atu_dev_t* dev)
{
    struct _atu_reg_map_t* p_atu = (struct _atu_reg_map_t*)dev->cfg->base;

    return p_atu->atuma;
}

enum atu_error_t set_start_logical_address(struct atu_dev_t* dev,
                    uint32_t address, uint8_t region)
{
    struct _atu_reg_map_t* p_atu = (struct _atu_reg_map_t*)dev->cfg->base;

    uint8_t ps = ATU_GET_ATUPS(p_atu);

    if(region >= get_supported_region_count(dev))
        return ATU_ERR_INVALID_REGION;

    /* The value stored in this field is the start logical address
     * right shifted by the value of the PS */
    p_atu->atursla[region] = (address >> ps);

    return ATU_ERR_NONE;
}

enum atu_error_t set_end_logical_address(struct atu_dev_t* dev,
                    uint32_t address, uint8_t region)
{
    struct _atu_reg_map_t* p_atu = (struct _atu_reg_map_t*)dev->cfg->base;

    uint8_t ps = ATU_GET_ATUPS(p_atu);

    if(region >= get_supported_region_count(dev))
        return ATU_ERR_INVALID_REGION;

    /* The end page should be greater than or equal to the start page */
    if((address >> ps) < p_atu->atursla[region])
        return ATU_ERR_INVALID_ADDRESS;

    /* The value stored in this field is the start logical address
     * right shifted by the value of the PS */
    p_atu->aturela[region] = (address >> ps);

    return ATU_ERR_NONE;
}

enum atu_error_t set_add_value(struct atu_dev_t* dev,
                    uint64_t offset_address, uint8_t region)
{
    struct _atu_reg_map_t* p_atu = (struct _atu_reg_map_t*)dev->cfg->base;

    if(region >= get_supported_region_count(dev))
        return ATU_ERR_INVALID_REGION;

    p_atu->aturav_l[region] = (uint32_t)(offset_address);

    p_atu->aturav_m[region] = (uint32_t)(offset_address >> 32);

    return ATU_ERR_NONE;
}

enum atu_error_t set_axnsc(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region)
{
    return _set_bus_attribute(dev, val, region, ATU_ATUROBA_AXNSE_OFF);
}

enum atu_error_t set_axcache3(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region)
{
    return _set_bus_attribute(dev, val, region, ATU_ATUROBA_AXCACHE3_OFF);
}

enum atu_error_t set_axcache2(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region)
{
    return _set_bus_attribute(dev, val, region, ATU_ATUROBA_AXCACHE2_OFF);
}

enum atu_error_t set_axcache1(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region)
{
    return _set_bus_attribute(dev, val, region, ATU_ATUROBA_AXCACHE1_OFF);
}

enum atu_error_t set_axcache0(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region)
{
    return _set_bus_attribute(dev, val, region, ATU_ATUROBA_AXCACHE0_OFF);
}

enum atu_error_t set_axprot2(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region)
{
    return _set_bus_attribute(dev, val, region, ATU_ATUROBA_AXPROT2_OFF);
}

enum atu_error_t set_axprot1(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region)
{
    return _set_bus_attribute(dev, val, region, ATU_ATUROBA_AXPROT1_OFF);
}

enum atu_error_t set_axprot0(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region)
{
    return _set_bus_attribute(dev, val, region, ATU_ATUROBA_AXPROT0_OFF);
}

enum atu_error_t set_gp_value(struct atu_dev_t* dev,
                    uint8_t val, uint8_t region)
{
    struct _atu_reg_map_t* p_atu = (struct _atu_reg_map_t*)dev->cfg->base;

    if(region >= get_supported_region_count(dev))
        return ATU_ERR_INVALID_REGION;

    p_atu->aturgp[region] = (uint32_t)val;

    return ATU_ERR_NONE;
}

uint8_t get_gp_value(struct atu_dev_t* dev, uint8_t region)
{
    struct _atu_reg_map_t* p_atu = (struct _atu_reg_map_t*)dev->cfg->base;

    return (uint8_t)(p_atu->aturgp[region] & 0xFF);
}

enum atu_error_t atu_initialize_region(struct atu_dev_t *dev, uint8_t region,
                                       uint32_t log_addr, uint64_t phys_addr,
                                       uint32_t size)
{
    enum atu_error_t err;
    struct _atu_reg_map_t* p_atu = (struct _atu_reg_map_t*)dev->cfg->base;

    uint32_t end_log_addr = log_addr + size - 1;
    uint8_t ps = ATU_GET_ATUPS(p_atu);
    uint64_t add_value = (phys_addr - log_addr) >> ps;

    if (dev == NULL) {
        /* Invalid parameters */
        return ATU_ERR_INVALID_ARG;
    }

    if ((phys_addr & ((1 << ps) - 1)) != 0 || (log_addr & ((1 << ps) - 1)) != 0
         || (size & ((1 << ps) - 1)) != 0) {
        return ATU_ERR_INVALID_ADDRESS;
    }

    err = set_start_logical_address(dev, log_addr, region);
    if (err != ATU_ERR_NONE) {
        return err;
    }

    err = set_end_logical_address(dev, end_log_addr, region);
    if (err != ATU_ERR_NONE) {
        return err;
    }

    err = set_add_value(dev, add_value, region);
    if (err != ATU_ERR_NONE) {
        return err;
    }

    err = enable_atu_region(dev, region);
    if (err != ATU_ERR_NONE) {
        return err;
    }

    return ATU_ERR_NONE;
}

enum atu_error_t atu_uninitialize_region(struct atu_dev_t *dev, uint8_t region)
{
    enum atu_error_t err;

    if (dev == NULL) {
        /* Invalid parameters */
        return ATU_ERR_INVALID_ARG;
    }

    err = disable_atu_region(dev, region);
    if (err != ATU_ERR_NONE) {
        return err;
    }

    return ATU_ERR_NONE;
}
