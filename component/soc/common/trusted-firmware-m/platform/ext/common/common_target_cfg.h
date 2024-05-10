/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TARGET_CFG_COMMON_H__
#define __TARGET_CFG_COMMON_H__

#include <stdint.h>
#include <stddef.h>
#include "target_cfg.h"
#include "tfm_plat_defs.h"

/**
 * \brief Holds the data necessary to do isolation for a specific peripheral.
 */
struct platform_data_t
{
    uint32_t periph_start;
    uint32_t periph_limit;
    enum ppc_bank_e periph_ppc_bank;
    int16_t periph_ppc_mask;
};

/**
 * \brief Store the addresses of memory regions
 */
struct memory_region_limits {
    uint32_t non_secure_code_start;
    uint32_t non_secure_partition_base;
    uint32_t non_secure_partition_limit;
    uint32_t veneer_base;
    uint32_t veneer_limit;
#ifdef BL2
    uint32_t secondary_partition_base;
    uint32_t secondary_partition_limit;
#endif /* BL2 */
};

/**
 * \brief Clear MPC interrupt.
 */
void mpc_clear_irq(void);

/**
 * \brief Clears PPC interrupt.
 */
void ppc_clear_irq(void);

/**
 * \brief Configures SAU and IDAU.
 */
void sau_and_idau_cfg(void);

/**
 * \brief Configures the Memory Protection Controller.
 *
 * \return  Returns error code.
 */
enum tfm_plat_err_t mpc_init_cfg(void);

/**
 * \brief Configures the Peripheral Protection Controller.
 */
enum tfm_plat_err_t ppc_init_cfg(void);

/**
 * \brief Restict peripheral access to secure access only
 *
 * \note The function does not configure privilege
 */
void ppc_configure_to_secure(enum ppc_bank_e bank, uint32_t pos);

/**
 * \brief Allow non-secure access to peripheral
 *
 * \note The function does not configure privilege
 */
void ppc_configure_to_non_secure(enum ppc_bank_e bank, uint32_t pos);

/**
 * \brief Enable secure unprivileged access to peripheral
 */
void ppc_en_secure_unpriv(enum ppc_bank_e bank, uint32_t pos);

/**
 * \brief Clear secure unprivileged access to peripheral
 */
void ppc_clr_secure_unpriv(enum ppc_bank_e bank, uint32_t pos);

/**
 * \brief Enables the fault handlers BusFault, UsageFault,
 *        MemManageFault and SecureFault.
 */
enum tfm_plat_err_t enable_fault_handlers(void);

/**
 * \brief Configures the system reset request properties
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t system_reset_cfg(void);

/**
 * \brief Configures all external interrupts to target the
 *        NS state, apart for the ones associated to secure
 *        peripherals (plus MPC and PPC)
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t nvic_interrupt_target_state_cfg(void);

/**
 * \brief This function enable the interrupts associated
 *        to the secure peripherals (plus MPC and PPC)
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t nvic_interrupt_enable(void);

/**
 * \brief Configures the system debug properties.
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
enum tfm_plat_err_t init_debug(void);

/**
 * \brief Returns platform's MMIO list and length, defined in
 *        tfm_peripherals_def.c file.
 */
void get_partition_named_mmio_list(const uintptr_t** list, size_t* length);

#endif /* __TARGET_CFG_COMMON_H__ */
