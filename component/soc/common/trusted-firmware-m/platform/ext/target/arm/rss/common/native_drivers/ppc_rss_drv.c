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

#include "ppc_rss_drv.h"
#include <stdint.h>
#include <stdbool.h>
#include <platform_regs.h>

/* Default peripheral states */
#define SECURE_AS_DEFAULT_PERIPHERAL_STATE  true
#define PRIVILEGE_ONLY_AS_DEFAULT_PERIPHERAL_STATE  true

enum ppc_rss_error_t ppc_rss_init(struct ppc_rss_dev_t* dev)
{
    struct rss_sacfg_t* p_sacfg =
                         (struct rss_sacfg_t*)dev->cfg->sacfg_base;
    struct rss_nsacfg_t* p_nsacfg =
                       (struct rss_nsacfg_t*)dev->cfg->nsacfg_base;

    switch(dev->cfg->ppc_name) {
        /* Case for MAIN0 */
        case PPC_RSS_MAIN0:
            dev->data->sacfg_ns_ppc   = &p_sacfg->mainnsppc0;
            dev->data->sacfg_sp_ppc   = &p_sacfg->mainspppc0;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->mainnspppc0;
            dev->data->int_bit_mask    = MAIN_PPC0_INT_POS_MASK;
            break;

        /* Case for MAIN EXPX */
        case PPC_RSS_MAIN_EXP0:
            dev->data->sacfg_ns_ppc   = &p_sacfg-> mainnsppcexp0;
            dev->data->sacfg_sp_ppc   = &p_sacfg-> mainspppcexp0;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->mainnspppcexp0;
            dev->data->int_bit_mask    = MAIN_PPCEXP0_INT_POS_MASK;
            break;
        case PPC_RSS_MAIN_EXP1:
            dev->data->sacfg_ns_ppc   = &p_sacfg->mainnsppcexp1;
            dev->data->sacfg_sp_ppc   = &p_sacfg->mainspppcexp1;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->mainnspppcexp1;
            dev->data->int_bit_mask    = MAIN_PPCEXP1_INT_POS_MASK;
            break;
        case PPC_RSS_MAIN_EXP2:
            dev->data->sacfg_ns_ppc   = &p_sacfg->mainnsppcexp2;
            dev->data->sacfg_sp_ppc   = &p_sacfg->mainspppcexp2;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->mainnspppcexp2;
            dev->data->int_bit_mask    = MAIN_PPCEXP2_INT_POS_MASK;
            break;
        case PPC_RSS_MAIN_EXP3:
            dev->data->sacfg_ns_ppc   = &p_sacfg->mainnsppcexp3;
            dev->data->sacfg_sp_ppc   = &p_sacfg->mainspppcexp3;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->mainnspppcexp3;
            dev->data->int_bit_mask    = MAIN_PPCEXP3_INT_POS_MASK;
            break;

        /* Case for PERIPHX */
        case PPC_RSS_PERIPH0:
            dev->data->sacfg_ns_ppc   = &p_sacfg->periphnsppc0;
            dev->data->sacfg_sp_ppc   = &p_sacfg->periphspppc0;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->periphnspppc0;
            dev->data->int_bit_mask    = PERIPH_PPC0_INT_POS_MASK;
            break;
        case PPC_RSS_PERIPH1:
            dev->data->sacfg_ns_ppc   = &p_sacfg->periphnsppc1;
            dev->data->sacfg_sp_ppc   = &p_sacfg->periphspppc1;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->periphnspppc1;
            dev->data->int_bit_mask    = PERIPH_PPC1_INT_POS_MASK;
            break;

        /* Case for PERIPH EXPX */
        case PPC_RSS_PERIPH_EXP0:
            dev->data->sacfg_ns_ppc   = &p_sacfg->periphnsppcexp0;
            dev->data->sacfg_sp_ppc   = &p_sacfg->periphspppcexp0;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->periphnspppcexp0;
            dev->data->int_bit_mask    = PERIPH_PPCEXP0_INT_POS_MASK;
            break;
        case PPC_RSS_PERIPH_EXP1:
            dev->data->sacfg_ns_ppc   = &p_sacfg->periphnsppcexp1;
            dev->data->sacfg_sp_ppc   = &p_sacfg->periphspppcexp1;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->periphnspppcexp1;
            dev->data->int_bit_mask    = PERIPH_PPCEXP1_INT_POS_MASK;
            break;
        case PPC_RSS_PERIPH_EXP2:
            dev->data->sacfg_ns_ppc   = &p_sacfg->periphnsppcexp2;
            dev->data->sacfg_sp_ppc   = &p_sacfg->periphspppcexp2;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->periphnspppcexp2;
            dev->data->int_bit_mask    = PERIPH_PPCEXP2_INT_POS_MASK;
            break;
        case PPC_RSS_PERIPH_EXP3:
            dev->data->sacfg_ns_ppc   = &p_sacfg->periphnsppcexp3;
            dev->data->sacfg_sp_ppc   = &p_sacfg->periphspppcexp3;
            dev->data->nsacfg_nsp_ppc = &p_nsacfg->periphnspppcexp3;
            dev->data->int_bit_mask    = PERIPH_PPCEXP3_INT_POS_MASK;
            break;
        default:
            return PPC_RSS_ERR_INVALID_PARAM;
        }

    dev->data->is_initialized = true;

    return PPC_RSS_ERR_NONE;
}

enum ppc_rss_error_t
ppc_rss_config_privilege(struct ppc_rss_dev_t* dev, uint32_t mask,
                            enum ppc_rss_sec_attr_t sec_attr,
                            enum ppc_rss_priv_attr_t priv_attr)
{
    if(dev->data->is_initialized != true) {
        return PPC_RSS_ERR_NOT_INIT;
    }

    if(sec_attr == PPC_RSS_SECURE_ACCESS) {
#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))
        /* Uses secure unprivileged access address (SACFG) to set privilege
         * attribute
         */
        if(priv_attr == PPC_RSS_PRIV_ONLY_ACCESS) {
            *(dev->data->sacfg_sp_ppc) &= ~mask;
        } else {
            *(dev->data->sacfg_sp_ppc) |= mask;
        }
#else
        /* Configuring security from Non-Secure application is not permitted. */
        return PPC_RSS_ERR_NOT_PERMITTED;
#endif
    } else {
        /* Uses non-secure unprivileged access address (NSACFG) to set
         * privilege attribute */
        if(priv_attr == PPC_RSS_PRIV_ONLY_ACCESS) {
            *(dev->data->nsacfg_nsp_ppc) &= ~mask;
        } else {
            *(dev->data->nsacfg_nsp_ppc) |= mask;
        }
    }

    return PPC_RSS_ERR_NONE;
}

bool ppc_rss_is_periph_priv_only(struct ppc_rss_dev_t* dev,
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

enum ppc_rss_error_t
ppc_rss_config_security(struct ppc_rss_dev_t* dev, uint32_t mask,
                           enum ppc_rss_sec_attr_t sec_attr)
{
    if(dev->data->is_initialized != true) {
        return PPC_RSS_ERR_NOT_INIT;
    }

    if(sec_attr == PPC_RSS_SECURE_ACCESS) {
        *(dev->data->sacfg_ns_ppc) &= ~mask;
    } else {
        *(dev->data->sacfg_ns_ppc) |= mask;
    }

    return PPC_RSS_ERR_NONE;
}

bool ppc_rss_is_periph_secure(struct ppc_rss_dev_t* dev,
                                 uint32_t mask)
{
    if(dev->data->is_initialized != true) {
        /* Return true as the default configuration is secure */
        return true;
    }

    return ((*(dev->data->sacfg_ns_ppc) & mask) == 0);
}

enum ppc_rss_error_t ppc_rss_irq_enable(struct ppc_rss_dev_t* dev)
{
    struct rss_sacfg_t* p_sacfg =
                         (struct rss_sacfg_t*)dev->cfg->sacfg_base;

    if(dev->data->is_initialized != true) {
        return PPC_RSS_ERR_NOT_INIT;
    }

    p_sacfg->secppcinten |= dev->data->int_bit_mask;

    return PPC_RSS_ERR_NONE;
}

void ppc_rss_irq_disable(struct ppc_rss_dev_t* dev)
{
    struct rss_sacfg_t* p_sacfg =
                         (struct rss_sacfg_t*)dev->cfg->sacfg_base;

    if(dev->data->is_initialized == true) {
        p_sacfg->secppcinten &= ~(dev->data->int_bit_mask);
    }
}

void ppc_rss_clear_irq(struct ppc_rss_dev_t* dev)
{
    struct rss_sacfg_t* p_sacfg =
                         (struct rss_sacfg_t*)dev->cfg->sacfg_base;

    if(dev->data->is_initialized == true) {
        p_sacfg->secppcintclr = dev->data->int_bit_mask;
    }
}

bool ppc_rss_irq_state(struct ppc_rss_dev_t* dev)
{
    struct rss_sacfg_t* p_sacfg =
                         (struct rss_sacfg_t*)dev->cfg->sacfg_base;

    if(dev->data->is_initialized != true) {
        return false;
    }

    return ((p_sacfg->secppcintstat & dev->data->int_bit_mask) != 0);
}

#endif /* (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)) */
