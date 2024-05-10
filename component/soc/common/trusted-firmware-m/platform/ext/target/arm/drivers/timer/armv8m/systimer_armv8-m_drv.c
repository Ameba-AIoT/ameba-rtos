/*
 * Copyright (c) 2019-2021 Arm Limited
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

/**
 * \file systimer_armv8-m_drv.c
 *
 * \brief Driver for Armv8-M System Timer
 */

#include "systimer_armv8-m_drv.h"
#include "systimer_armv8-m_reg_map.h"

/** Setter bit manipulation macro */
#define SET_BIT(WORD, BIT_INDEX) ((WORD) |= (1u << (BIT_INDEX)))
/** Clearing bit manipulation macro */
#define CLR_BIT(WORD, BIT_INDEX) ((WORD) &= ~(1u << (BIT_INDEX)))
/** Getter bit manipulation macro */
#define GET_BIT(WORD, BIT_INDEX) (bool)(((WORD) & (1u << (BIT_INDEX))))
/** Getter bit-field manipulation macro */
#define GET_BIT_FIELD(WORD, BIT_MASK, BIT_OFFSET) \
            ((WORD & BIT_MASK) >> BIT_OFFSET)
/** Bit mask for given width bit-field manipulation macro */
#define BITMASK(width) ((1u<<(width))-1)

/**
 * \brief Timer Control Register bit fields
 */
#define SYSCTIMER_ARMV8M_CNTP_CTL_EN_OFF           0u
    /*!< Timer Control Register Enable Counter bit field offset */
#define SYSCTIMER_ARMV8M_CNTP_CTL_IMASK_OFF        1u
/*!< Timer Control Register Interrupt Mask bit field offset */
#define SYSCTIMER_ARMV8M_CNTP_CTL_ISTATUS_OFF      2u
/*!< Timer Control Register Interrupt Status bit field offset */

/**
 * \brief Timer AutoInc Control Register bit fields
 */
#define SYSCTIMER_ARMV8M_CNTP_AIVAL_CTL_EN_OFF           0u
    /*!< Timer Control Register Enable Counter bit field offset */
#define SYSCTIMER_ARMV8M_CNTP_AIVAL_CTL_IRQ_CLR_OFF      1u
/*!< Timer Control Register Interrupt clear bit field offset */

/**
 * \brief Timer AutoInc Config Register bit fields
 */
#define SYSCTIMER_ARMV8M_CNTP_CFG_CTL_AUTOINC_OFF        0u
    /*!< Timer Control Register AutoInc is implemented bit field offset */


void systimer_armv8_m_init(struct systimer_armv8_m_dev_t* dev)
{
    if (dev->data->is_initialized == false) {
        systimer_armv8_m_disable_interrupt(dev);
        systimer_armv8_m_disable_autoinc(dev);
        systimer_armv8_m_set_counter_freq(dev, dev->cfg->default_freq_hz);
        systimer_armv8_m_enable_timer(dev);
        dev->data->is_initialized = true;
    }
}

void systimer_armv8_m_uninit(struct systimer_armv8_m_dev_t* dev)
{
    if (dev->data->is_initialized == true) {
        systimer_armv8_m_disable_interrupt(dev);
        systimer_armv8_m_disable_autoinc(dev);
        systimer_armv8_m_disable_timer(dev);
        dev->data->is_initialized = false;
    }
}

uint64_t systimer_armv8_m_get_counter_value(
           struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    uint32_t high = 0;
    uint32_t low = 0;
    uint32_t high_prev = 0;
    uint64_t value = 0;

    /* Make sure the 64-bit read will be atomic to avoid overflow between
     * the low and high registers read
     */
    high = p_cnt->cntpct_high;
    do {
        high_prev = high;
        low = p_cnt->cntpct_low;
        high = p_cnt->cntpct_high;
    }while(high != high_prev);

    value = low | (((uint64_t)high) << SYSTIMER_ARMV8_M_REGISTER_BIT_WIDTH);
    return value;
}

void systimer_armv8_m_set_compare_value(
        struct systimer_armv8_m_dev_t* dev,
        uint64_t value)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    p_cnt->cntp_cval_low = value & UINT32_MAX;
    p_cnt->cntp_cval_high = value >> SYSTIMER_ARMV8_M_REGISTER_BIT_WIDTH;
}

uint64_t systimer_armv8_m_get_compare_value(
        struct systimer_armv8_m_dev_t* dev)
{
    uint64_t value = 0;
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    value = p_cnt->cntp_cval_low |
            (((uint64_t)p_cnt->cntp_cval_high) <<
                    SYSTIMER_ARMV8_M_REGISTER_BIT_WIDTH);
    return value;
}

void systimer_armv8_m_set_counter_freq(struct systimer_armv8_m_dev_t* dev,
                                       uint32_t value)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    p_cnt->cntfrq = value;
}

uint32_t systimer_armv8_m_get_counter_freq(
        struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    return p_cnt->cntfrq;
}

void systimer_armv8_m_set_timer_value(struct systimer_armv8_m_dev_t* dev,
                                      uint32_t value)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    p_cnt->cntp_tval = value;
}

uint32_t systimer_armv8_m_get_timer_value(
        struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    return p_cnt->cntp_tval;
}

void systimer_armv8_m_enable_timer(struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    SET_BIT(p_cnt->cntp_ctl, SYSCTIMER_ARMV8M_CNTP_CTL_EN_OFF);
}

void systimer_armv8_m_disable_timer(struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    CLR_BIT(p_cnt->cntp_ctl, SYSCTIMER_ARMV8M_CNTP_CTL_EN_OFF);
}

bool systimer_armv8_m_is_timer_enabled(
        struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    return GET_BIT(p_cnt->cntp_ctl, SYSCTIMER_ARMV8M_CNTP_CTL_EN_OFF);
}

void systimer_armv8_m_enable_interrupt(
        struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    /* The bit is masking interrupt, so it should be inverted. */
    CLR_BIT(p_cnt->cntp_ctl, SYSCTIMER_ARMV8M_CNTP_CTL_IMASK_OFF);
}

void systimer_armv8_m_disable_interrupt(
        struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    /* The bit is masking interrupt, so it should be inverted. */
    SET_BIT(p_cnt->cntp_ctl, SYSCTIMER_ARMV8M_CNTP_CTL_IMASK_OFF);
}

bool systimer_armv8_m_is_interrupt_enabled(
        struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    /* The bit is masking interrupt, so it should be inverted. */
    return !GET_BIT(p_cnt->cntp_ctl, SYSCTIMER_ARMV8M_CNTP_CTL_IMASK_OFF);
}

bool systimer_armv8_m_is_interrupt_asserted(
        struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    return GET_BIT(p_cnt->cntp_ctl, SYSCTIMER_ARMV8M_CNTP_CTL_ISTATUS_OFF);
}

uint64_t systimer_armv8_m_get_autoinc_value(
        struct systimer_armv8_m_dev_t* dev)
{
    uint64_t value = 0;
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    value = p_cnt->cntp_aival_low |
            (((uint64_t)p_cnt->cntp_aival_high) <<
             SYSTIMER_ARMV8_M_REGISTER_BIT_WIDTH);
    return value;
}

void systimer_armv8_m_set_autoinc_reload(
        struct systimer_armv8_m_dev_t* dev,
        uint32_t value)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    p_cnt->cntp_aival_reload = value;
}

uint32_t systimer_armv8_m_get_autoinc_reload(
        struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    return p_cnt->cntp_aival_reload;
}

void systimer_armv8_m_enable_autoinc(struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    SET_BIT(p_cnt->cntp_aival_ctl, SYSCTIMER_ARMV8M_CNTP_AIVAL_CTL_EN_OFF);
}

void systimer_armv8_m_disable_autoinc(struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    CLR_BIT(p_cnt->cntp_aival_ctl, SYSCTIMER_ARMV8M_CNTP_AIVAL_CTL_EN_OFF);
}

bool systimer_armv8_m_is_autoinc_enabled(
        struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    return GET_BIT(p_cnt->cntp_aival_ctl,
                   SYSCTIMER_ARMV8M_CNTP_AIVAL_CTL_EN_OFF);
}

void systimer_armv8_m_clear_autoinc_interrupt(
        struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    CLR_BIT(p_cnt->cntp_aival_ctl, SYSCTIMER_ARMV8M_CNTP_AIVAL_CTL_IRQ_CLR_OFF);
}

bool systimer_armv8_m_is_autoinc_implemented(
        struct systimer_armv8_m_dev_t* dev)
{
    struct cnt_base_reg_map_t* p_cnt =
            (struct cnt_base_reg_map_t*)dev->cfg->base;
    return GET_BIT(p_cnt->cntp_cfg,
                   SYSCTIMER_ARMV8M_CNTP_CFG_CTL_AUTOINC_OFF);
}
