/*
 * Copyright (c) 2019-2022 Arm Limited
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

#include "ppc_corstone310_drv.h"
#include "ppc_corstone310_reg_map.h"

#include <stdint.h>
#include <stdbool.h>

/* Default peripheral states */
#define SECURE_AS_DEFAULT_PERIPHERAL_STATE  true
#define PRIVILEGE_ONLY_AS_DEFAULT_PERIPHERAL_STATE  true

/* PPC interrupt position mask */
#define PERIPH_PPC0_INT_POS_MASK     (1UL << 0)
#define PERIPH_PPC1_INT_POS_MASK     (1UL << 1)
#define PERIPH_PPCEXP0_INT_POS_MASK  (1UL << 4)
#define PERIPH_PPCEXP1_INT_POS_MASK  (1UL << 5)
#define PERIPH_PPCEXP2_INT_POS_MASK  (1UL << 6)
#define PERIPH_PPCEXP3_INT_POS_MASK  (1UL << 7)
#define MAIN_PPC0_INT_POS_MASK       (1UL << 16)
#define MAIN_PPCEXP0_INT_POS_MASK    (1UL << 20)
#define MAIN_PPCEXP1_INT_POS_MASK    (1UL << 21)
#define MAIN_PPCEXP2_INT_POS_MASK    (1UL << 22)
#define MAIN_PPCEXP3_INT_POS_MASK    (1UL << 23)

/* Non-secure Access Configuration Register Block */
struct corstone310_nsacfg_block_t {
    volatile uint32_t reserved0[36];
    volatile uint32_t mainnspppc0;     /* 0x090 (R/W) Non-secure Unprivileged
                                        *             Access Peripheral
                                        *             Protection Control 0 on
                                        *             Main Interconnect */
    volatile uint32_t reserved1[3];

    volatile uint32_t mainnspppcexp0;  /* 0x0A0 (R/W) Expansion 0 Non-secure
                                        *             Unprivileged Access
                                        *             Peripheral Protection
                                        *             Control on Main
                                        *             Interconnect */
    volatile uint32_t mainnspppcexp1;  /* 0x0A4 (R/W) Expansion 1 Non-secure
                                        *             Unprivileged Access
                                        *             Peripheral Protection
                                        *             Control on Main
                                        *             Interconnect */
    volatile uint32_t mainnspppcexp2;  /* 0x0A8 (R/W) Expansion 2 Non-secure
                                        *             Unprivileged Access
                                        *             Peripheral Protection
                                        *             Control on Main
                                        *             Interconnect */
    volatile uint32_t mainnspppcexp3;  /* 0x0AC (R/W) Expansion 3 Non-secure
                                        *             Unprivileged Access
                                        *             Peripheral Protection
                                        *             Control on Main
                                        *             Interconnect */
    volatile uint32_t periphnspppc0;   /* 0x0B0 (R/W) Non-secure Unprivileged
                                        *             Access Peripheral
                                        *             Protection Control 0 on
                                        *             Peripheral Interconnect */
    volatile uint32_t periphnspppc1;   /* 0x0B4 (R/W) Non-secure Unprivileged
                                        *             Access Peripheral
                                        *             Protection Control 1 on
                                        *             Peripheral Interconnect */
    volatile uint32_t reserved2[2];
    volatile uint32_t periphnspppcexp0;/* 0x0C0 (R/W) Expansion 0 Non-secure
                                        *             Unprivileged Access
                                        *             Peripheral Protection
                                        *             Control on Peripheral
                                        *             Interconnect */
    volatile uint32_t periphnspppcexp1;/* 0x0C4 (R/W) Expansion 1 Non-secure
                                        *             Unprivileged Access
                                        *             Peripheral Protection
                                        *             Control on Peripheral
                                        *             Interconnect */
    volatile uint32_t periphnspppcexp2;/* 0x0C8 (R/W) Expansion 2 Non-secure
                                        *             Unprivileged Access
                                        *             Peripheral Protection
                                        *             Control on Peripheral
                                        *             Interconnect */
    volatile uint32_t periphnspppcexp3;/* 0x0CC (R/W) Expansion 3 Non-secure
                                        *             Unprivileged Access
                                        *             Peripheral Protection
                                        *             Control on Peripheral
                                        *             Interconnect */
    volatile uint32_t reserved3[960];
    volatile uint32_t pidr4;           /* 0xFD0 (R/ ) Peripheral ID 4 */
    volatile uint32_t reserved4[3];
    volatile uint32_t pidr0;           /* 0xFE0 (R/ ) Peripheral ID 0 */
    volatile uint32_t pidr1;           /* 0xFE4 (R/ ) Peripheral ID 1 */
    volatile uint32_t pidr2;           /* 0xFE8 (R/ ) Peripheral ID 2 */
    volatile uint32_t pidr3;           /* 0xFEC (R/ ) Peripheral ID 3 */
    volatile uint32_t cidr0;           /* 0xFF0 (R/ ) Component ID 0 */
    volatile uint32_t cidr1;           /* 0xFF4 (R/ ) Component ID 1 */
    volatile uint32_t cidr2;           /* 0xFF8 (R/ ) Component ID 2 */
    volatile uint32_t cidr3;           /* 0xFFC (R/ ) Component ID 3 */
};

enum ppc_corstone310_error_t ppc_corstone310_init(struct ppc_corstone310_dev_t* dev)
{
    struct corstone310_sacfg_block_t* p_sacfg =
                         (struct corstone310_sacfg_block_t*)dev->cfg->sacfg_base;
    struct corstone310_nsacfg_block_t* p_nsacfg =
                       (struct corstone310_nsacfg_block_t*)dev->cfg->nsacfg_base;

    switch(dev->cfg->ppc_name) {
        /* Case for MAIN0 */
        case PPC_CORSTONE310_MAIN0:
            dev->data->sacfg_ns_ppc   = &p_sacfg->mainnsppc0;
            dev->data->sacfg_sp_ppc   = &p_sacfg->mainspppc0;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->mainnspppc0;
            dev->data->int_bit_mask    = MAIN_PPC0_INT_POS_MASK;
            break;

        /* Case for MAIN EXPX */
        case PPC_CORSTONE310_MAIN_EXP0:
            dev->data->sacfg_ns_ppc   = &p_sacfg-> mainnsppcexp0;
            dev->data->sacfg_sp_ppc   = &p_sacfg-> mainspppcexp0;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->mainnspppcexp0;
            dev->data->int_bit_mask    = MAIN_PPCEXP0_INT_POS_MASK;
            break;
        case PPC_CORSTONE310_MAIN_EXP1:
            dev->data->sacfg_ns_ppc   = &p_sacfg->mainnsppcexp1;
            dev->data->sacfg_sp_ppc   = &p_sacfg->mainspppcexp1;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->mainnspppcexp1;
            dev->data->int_bit_mask    = MAIN_PPCEXP1_INT_POS_MASK;
            break;
        case PPC_CORSTONE310_MAIN_EXP2:
            dev->data->sacfg_ns_ppc   = &p_sacfg->mainnsppcexp2;
            dev->data->sacfg_sp_ppc   = &p_sacfg->mainspppcexp2;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->mainnspppcexp2;
            dev->data->int_bit_mask    = MAIN_PPCEXP2_INT_POS_MASK;
            break;
        case PPC_CORSTONE310_MAIN_EXP3:
            dev->data->sacfg_ns_ppc   = &p_sacfg->mainnsppcexp3;
            dev->data->sacfg_sp_ppc   = &p_sacfg->mainspppcexp3;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->mainnspppcexp3;
            dev->data->int_bit_mask    = MAIN_PPCEXP3_INT_POS_MASK;
            break;

        /* Case for PERIPHX */
        case PPC_CORSTONE310_PERIPH0:
            dev->data->sacfg_ns_ppc   = &p_sacfg->periphnsppc0;
            dev->data->sacfg_sp_ppc   = &p_sacfg->periphspppc0;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->periphnspppc0;
            dev->data->int_bit_mask    = PERIPH_PPC0_INT_POS_MASK;
            break;
        case PPC_CORSTONE310_PERIPH1:
            dev->data->sacfg_ns_ppc   = &p_sacfg->periphnsppc1;
            dev->data->sacfg_sp_ppc   = &p_sacfg->periphspppc1;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->periphnspppc1;
            dev->data->int_bit_mask    = PERIPH_PPC1_INT_POS_MASK;
            break;

        /* Case for PERIPH EXPX */
        case PPC_CORSTONE310_PERIPH_EXP0:
            dev->data->sacfg_ns_ppc   = &p_sacfg->periphnsppcexp0;
            dev->data->sacfg_sp_ppc   = &p_sacfg->periphspppcexp0;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->periphnspppcexp0;
            dev->data->int_bit_mask    = PERIPH_PPCEXP0_INT_POS_MASK;
            break;
        case PPC_CORSTONE310_PERIPH_EXP1:
            dev->data->sacfg_ns_ppc   = &p_sacfg->periphnsppcexp1;
            dev->data->sacfg_sp_ppc   = &p_sacfg->periphspppcexp1;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->periphnspppcexp1;
            dev->data->int_bit_mask    = PERIPH_PPCEXP1_INT_POS_MASK;
            break;
        case PPC_CORSTONE310_PERIPH_EXP2:
            dev->data->sacfg_ns_ppc   = &p_sacfg->periphnsppcexp2;
            dev->data->sacfg_sp_ppc   = &p_sacfg->periphspppcexp2;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->periphnspppcexp2;
            dev->data->int_bit_mask    = PERIPH_PPCEXP2_INT_POS_MASK;
            break;
        case PPC_CORSTONE310_PERIPH_EXP3:
            dev->data->sacfg_ns_ppc   = &p_sacfg->periphnsppcexp3;
            dev->data->sacfg_sp_ppc   = &p_sacfg->periphspppcexp3;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->periphnspppcexp3;
            dev->data->int_bit_mask    = PERIPH_PPCEXP3_INT_POS_MASK;
            break;
        case CORSTONE310_PPC_MAX_NUM:
        default:
            return PPC_CORSTONE310_ERR_INVALID_PARAM;
        }

    dev->data->is_initialized = true;

    return PPC_CORSTONE310_ERR_NONE;
}

enum ppc_corstone310_error_t
ppc_corstone310_config_privilege(struct ppc_corstone310_dev_t* dev, uint32_t mask,
                            enum ppc_corstone310_sec_attr_t sec_attr,
                            enum ppc_corstone310_priv_attr_t priv_attr)
{
    if(dev->data->is_initialized != true) {
        return PPC_CORSTONE310_ERR_NOT_INIT;
    }

    if(sec_attr == PPC_CORSTONE310_SECURE_ACCESS) {
#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))
        /* Uses secure unprivileged access address (SACFG) to set privilege
         * attribute
         */
        if(priv_attr == PPC_CORSTONE310_PRIV_ONLY_ACCESS) {
            *(dev->data->sacfg_sp_ppc) &= ~mask;
        } else {
            *(dev->data->sacfg_sp_ppc) |= mask;
        }
#else
        /* Configuring security from Non-Secure application is not permitted. */
        return PPC_CORSTONE310_ERR_NOT_PERMITTED;
#endif
    } else {
        /* Uses non-secure unprivileged access address (NSACFG) to set
         * privilege attribute */
        if(priv_attr == PPC_CORSTONE310_PRIV_ONLY_ACCESS) {
            *(dev->data->nsacfg_nsp_ppc) &= ~mask;
        } else {
            *(dev->data->nsacfg_nsp_ppc) |= mask;
        }
    }

    return PPC_CORSTONE310_ERR_NONE;
}

bool ppc_corstone310_is_periph_priv_only(struct ppc_corstone310_dev_t* dev,
                                    uint32_t mask)
{
    if(dev->data->is_initialized != true) {
        /* Return true as the default configuration is privilege only */
        return true;
    }

#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))
    /* In secure domain either secure or non-secure privilege access is returned
     * based on the configuration */
    if ((*(dev->data->sacfg_ns_ppc) & mask) == 0) {
        /* Returns secure unprivileged access (SACFG) */
        return ((*(dev->data->sacfg_sp_ppc) & mask) == 0);
    } else {
        /* Returns non-secure unprivileged access (NSACFG) */
        return ((*(dev->data->nsacfg_nsp_ppc) & mask) == 0);
    }
#else
    /* Returns non-secure unprivileged access (NSACFG) */
    return ((*(dev->data->nsacfg_nsp_ppc) & mask) == 0);
#endif
}

/* Secure only functions */
#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))

enum ppc_corstone310_error_t
ppc_corstone310_config_security(struct ppc_corstone310_dev_t* dev, uint32_t mask,
                           enum ppc_corstone310_sec_attr_t sec_attr)
{
    if(dev->data->is_initialized != true) {
        return PPC_CORSTONE310_ERR_NOT_INIT;
    }

    if(sec_attr == PPC_CORSTONE310_SECURE_ACCESS) {
        *(dev->data->sacfg_ns_ppc) &= ~mask;
    } else {
        *(dev->data->sacfg_ns_ppc) |= mask;
    }

    return PPC_CORSTONE310_ERR_NONE;
}

bool ppc_corstone310_is_periph_secure(struct ppc_corstone310_dev_t* dev,
                                 uint32_t mask)
{
    if(dev->data->is_initialized != true) {
        /* Return true as the default configuration is secure */
        return true;
    }

    return ((*(dev->data->sacfg_ns_ppc) & mask) == 0);
}

enum ppc_corstone310_error_t ppc_corstone310_irq_enable(struct ppc_corstone310_dev_t* dev)
{
    struct corstone310_sacfg_block_t* p_sacfg =
                         (struct corstone310_sacfg_block_t*)dev->cfg->sacfg_base;

    if(dev->data->is_initialized != true) {
        return PPC_CORSTONE310_ERR_NOT_INIT;
    }

    p_sacfg->secppcinten |= dev->data->int_bit_mask;

    return PPC_CORSTONE310_ERR_NONE;
}

void ppc_corstone310_irq_disable(struct ppc_corstone310_dev_t* dev)
{
    struct corstone310_sacfg_block_t* p_sacfg =
                         (struct corstone310_sacfg_block_t*)dev->cfg->sacfg_base;

    if(dev->data->is_initialized == true) {
        p_sacfg->secppcinten &= ~(dev->data->int_bit_mask);
    }
}

void ppc_corstone310_clear_irq(struct ppc_corstone310_dev_t* dev)
{
    struct corstone310_sacfg_block_t* p_sacfg =
                         (struct corstone310_sacfg_block_t*)dev->cfg->sacfg_base;

    if(dev->data->is_initialized == true) {
        p_sacfg->secppcintclr = dev->data->int_bit_mask;
    }
}

bool ppc_corstone310_irq_state(struct ppc_corstone310_dev_t* dev)
{
    struct corstone310_sacfg_block_t* p_sacfg =
                         (struct corstone310_sacfg_block_t*)dev->cfg->sacfg_base;

    if(dev->data->is_initialized != true) {
        return false;
    }

    return ((p_sacfg->secppcintstat & dev->data->int_bit_mask) != 0);
}

#endif /* (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)) */
