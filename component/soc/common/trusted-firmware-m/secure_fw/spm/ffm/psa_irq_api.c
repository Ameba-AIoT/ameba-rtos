/*
 * Copyright (c) 2019-2023, Arm Limited. All rights reserved.
 * Copyright (c) 2022-2023 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "critical_section.h"
#include "ffm/psa_api.h"
#include "interrupt.h"
#include "spm.h"
#include "tfm_hal_interrupt.h"

void tfm_spm_partition_psa_irq_enable(psa_signal_t irq_signal)
{
    struct partition_t *partition;
    const struct irq_load_info_t *irq_info;

    partition = GET_CURRENT_COMPONENT();

    irq_info = get_irq_info_for_signal(partition->p_ldinf, irq_signal);
    if (!irq_info) {
        tfm_core_panic();
    }

    tfm_hal_irq_enable(irq_info->source);
}

psa_irq_status_t tfm_spm_partition_psa_irq_disable(psa_signal_t irq_signal)
{
    struct partition_t *partition;
    const struct irq_load_info_t *irq_info;

    partition = GET_CURRENT_COMPONENT();

    irq_info = get_irq_info_for_signal(partition->p_ldinf, irq_signal);
    if (!irq_info) {
        tfm_core_panic();
    }

    tfm_hal_irq_disable(irq_info->source);

    return 1;
}

/* This API is only used for FLIH. */
#if CONFIG_TFM_FLIH_API == 1
void tfm_spm_partition_psa_reset_signal(psa_signal_t irq_signal)
{
    struct critical_section_t cs_assert = CRITICAL_SECTION_STATIC_INIT;
    const struct irq_load_info_t *irq_info;
    struct partition_t *partition;

    partition = GET_CURRENT_COMPONENT();

    irq_info = get_irq_info_for_signal(partition->p_ldinf, irq_signal);
    if (!irq_info) {
        tfm_core_panic();
    }

    if (!irq_info->flih_func) {
        /* This API is for FLIH IRQs only */
        tfm_core_panic();
    }

    if ((partition->signals_asserted & irq_signal) == 0) {
        /* The signal is not asserted */
        tfm_core_panic();
    }

    CRITICAL_SECTION_ENTER(cs_assert);
    partition->signals_asserted &= ~irq_signal;
    CRITICAL_SECTION_LEAVE(cs_assert);
}
#endif /* CONFIG_TFM_FLIH_API == 1 */

/* This API is only used for SLIH. */
#if CONFIG_TFM_SLIH_API == 1
void tfm_spm_partition_psa_eoi(psa_signal_t irq_signal)
{
    struct critical_section_t cs_assert = CRITICAL_SECTION_STATIC_INIT;
    const struct irq_load_info_t *irq_info = NULL;
    struct partition_t *partition = NULL;

    partition = GET_CURRENT_COMPONENT();

    irq_info = get_irq_info_for_signal(partition->p_ldinf, irq_signal);
    /* It is a fatal error if passed signal is not an interrupt signal. */
    if (!irq_info) {
        tfm_core_panic();
    }

    if (irq_info->flih_func) {
        /* This API is for SLIH IRQs only */
        tfm_core_panic();
    }

    /* It is a fatal error if passed signal is not currently asserted */
    if ((partition->signals_asserted & irq_signal) == 0) {
        tfm_core_panic();
    }

    CRITICAL_SECTION_ENTER(cs_assert);
    partition->signals_asserted &= ~irq_signal;
    CRITICAL_SECTION_LEAVE(cs_assert);

    tfm_hal_irq_clear_pending(irq_info->source);
    tfm_hal_irq_enable(irq_info->source);
}
#endif /* CONFIG_TFM_SLIH_API == 1 */
