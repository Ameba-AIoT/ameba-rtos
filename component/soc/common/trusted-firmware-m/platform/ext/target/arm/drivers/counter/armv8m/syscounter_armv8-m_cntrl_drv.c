/*
 * Copyright (c) 2019 Arm Limited
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
 * \file syscounter_armv8-m_cntrl_drv.c
 *
 * \brief Driver for Armv8-M System Counter Control, covering CNTControlBase
 *        Frame
 *
 * This System Counter is a 64-bit up-counter, generating the physical
 * count for System Timer.
 *
 * Main features:
 *   - Enabled/disable and Set/Get the 64-bit upcounter
 *   - 2 scaling register for the 2 clock sources
 *       - These registers are used to pre-program the scaling values so
 *          that when hardware based clock switching is implemented there is no
 *          need to program the scaling increment value each time when clock is
 *          switched.
 *        - When counter scaling is enabled, ScaleVal is the amount added to the
 *          Counter Count Value for every period of the counter as determined
 *          by 1/Frequency from the current operating frequency of the system
 *          counter (the "counter tick").
 *        - ScaleVal is expressed as an unsigned fixed-point number with
 *          a 8 bit integer value and a 24-bit fractional value
 *   - Interrupt for error detection
 *       There are 2 possible reasons for error notification generation from
 *       the Counter:
 *         1. Security attribute mismatch between register access and security
 *            attribute of the CONTROL frame
 *         2. Address decode error within a given frame
 *
 */

#include "syscounter_armv8-m_cntrl_drv.h"
#include "syscounter_armv8-m_cntrl_reg_map.h"

/** Setter bit manipulation macro */
#define SET_BIT(WORD, BIT_INDEX) ((WORD) |= (1U << (BIT_INDEX)))
/** Clearing bit manipulation macro */
#define CLR_BIT(WORD, BIT_INDEX) ((WORD) &= ~(1U << (BIT_INDEX)))
/** Getter bit manipulation macro */
#define GET_BIT(WORD, BIT_INDEX) (bool)(((WORD) & (1U << (BIT_INDEX))))
/** Clear-and-Set bit manipulation macro */
#define ASSIGN_BIT(WORD, BIT_INDEX, VALUE) \
            (WORD = ((WORD & ~(1U << (BIT_INDEX))) | (VALUE << (BIT_INDEX))))
/** Getter bit-field manipulation macro */
#define GET_BIT_FIELD(WORD, BIT_MASK, BIT_OFFSET) \
            ((WORD & BIT_MASK) >> BIT_OFFSET)
/** Bit mask for given width bit-field manipulation macro */
#define BITMASK(width) ((1u<<(width))-1)

/**
 * \brief Counter Control Register bit fields
 */
#define SYSCOUNTER_ARMV8M_CNTCR_EN_OFF             0u
    /*!< Counter Control Register Enable Counter bit field offset */
#define SYSCOUNTER_ARMV8M_CNTCR_HDBG_OFF           1u
    /*!< Counter Control Register Halt On Debug bit field offset */
#define SYSCOUNTER_ARMV8M_CNTCR_SCEN_OFF           2u
    /*!< Counter Control Register Scale enable bit field offset */
#define SYSCOUNTER_ARMV8M_CNTCR_INTRMASK_OFF       3u
    /*!< Counter Control Register Interrupt mask bit field offset */
#define SYSCOUNTER_ARMV8M_CNTCR_PSLVERRDIS_OFF     4u
    /*!< Counter Control Register PSLVERR disable bit field offset */
#define SYSCOUNTER_ARMV8M_CNTCR_INTRCLR_OFF        5u
    /*!< Counter Control Register Interrupt Clear bit field offset */

/**
 * \brief Counter Status Register bit fields
 */
#define SYSCOUNTER_ARMV8M_CNTSR_DBGH_OFF           1u
    /*!< Counter Status Register Halt-on-Debug bit field offset */

/**
 * \brief Counter ID Register bit fields
 */
#define SYSCOUNTER_ARMV8M_CNTID_CNTSC_OFF                  0u
    /*!< Counter ID Register Counter Scaling is implemented bit field offset */
#define SYSCOUNTER_ARMV8M_CNTID_CNTCS_OFF                  16u
    /*!< Counter ID Register Clock switching is implemented bit field offset */

/*! Counter ID Register Clock source */
#define SYSCOUNTER_ARMV8M_CNTID_CNTSELCLK_OFF              17u
#define SYSCOUNTER_ARMV8M_CNTID_CNTSELCLK_WIDTH            2u
#define SYSCOUNTER_ARMV8M_CNTID_CNTSELCLK_MASK             \
            (BITMASK(SYSCOUNTER_ARMV8M_CNTID_CNTSELCLK_WIDTH) \
            << SYSCOUNTER_ARMV8M_CNTID_CNTSELCLK_OFF)

#define SYSCOUNTER_ARMV8M_CNTID_CNTSCR_OVR_OFF             19u
    /*!< Counter ID Register Override counter enable condition for
     *   writing to CNTSCR registers bit offset
     */

enum syscounter_armv8_m_cntrl_error_t syscounter_armv8_m_cntrl_init(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    enum syscounter_armv8_m_cntrl_error_t result = SYSCOUNTER_ARMV8_M_ERR_NONE;

    if (dev->data->is_initialized == false) {
        syscounter_armv8_m_cntrl_disable_counter(dev);
        if (syscounter_armv8_m_cntrl_is_counter_scaling_implemented(dev)) {
            result = syscounter_armv8_m_cntrl_set_counter_scale_value(
                     dev, SYSCOUNTER_ARMV8_M_SCALE_NR_0, dev->cfg->scale0);
            if (result != SYSCOUNTER_ARMV8_M_ERR_NONE) {
                return result;
            }
            result = syscounter_armv8_m_cntrl_set_counter_scale_value(
                     dev, SYSCOUNTER_ARMV8_M_SCALE_NR_1, dev->cfg->scale1);
            if (result != SYSCOUNTER_ARMV8_M_ERR_NONE) {
                return result;
            }
        }
        syscounter_armv8_m_cntrl_set_counter_value(dev,
            SYSCOUNTER_ARMV8_M_DEFAULT_INIT_CNT_VAL);
        syscounter_armv8_m_cntrl_disable_interrupt(dev);
        syscounter_armv8_m_cntrl_disable_scale(dev);

        syscounter_armv8_m_cntrl_enable_counter(dev);
        dev->data->is_initialized = true;
    }
    return SYSCOUNTER_ARMV8_M_ERR_NONE;
}

void syscounter_armv8_m_cntrl_uninit(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    if (dev->data->is_initialized == true) {
        syscounter_armv8_m_cntrl_disable_counter(dev);
        syscounter_armv8_m_cntrl_disable_interrupt(dev);
        syscounter_armv8_m_cntrl_set_counter_value(dev,
                SYSCOUNTER_ARMV8_M_DEFAULT_INIT_CNT_VAL);
        dev->data->is_initialized = false;
    }
}

void syscounter_armv8_m_cntrl_enable_counter(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    SET_BIT(p_cnt->cntcr, SYSCOUNTER_ARMV8M_CNTCR_EN_OFF);
}

void syscounter_armv8_m_cntrl_disable_counter(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    CLR_BIT(p_cnt->cntcr, SYSCOUNTER_ARMV8M_CNTCR_EN_OFF);
}

bool syscounter_armv8_m_cntrl_is_counter_enabled(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    return GET_BIT(p_cnt->cntcr,
                         SYSCOUNTER_ARMV8M_CNTCR_EN_OFF);
}

void syscounter_armv8_m_cntrl_enable_halt_on_debug(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    SET_BIT(p_cnt->cntcr, SYSCOUNTER_ARMV8M_CNTCR_HDBG_OFF);
}

void syscounter_armv8_m_cntrl_disable_halt_on_debug(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    CLR_BIT(p_cnt->cntcr, SYSCOUNTER_ARMV8M_CNTCR_HDBG_OFF);
}

bool syscounter_armv8_m_cntrl_is_halt_on_debug_enabled(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    return GET_BIT(p_cnt->cntcr,
                         SYSCOUNTER_ARMV8M_CNTCR_HDBG_OFF);
}

void syscounter_armv8_m_cntrl_enable_scale(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    SET_BIT(p_cnt->cntcr, SYSCOUNTER_ARMV8M_CNTCR_SCEN_OFF);
}

void syscounter_armv8_m_cntrl_disable_scale(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    CLR_BIT(p_cnt->cntcr, SYSCOUNTER_ARMV8M_CNTCR_SCEN_OFF);
}

bool syscounter_armv8_m_cntrl_is_scale_enabled(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    return GET_BIT(p_cnt->cntcr,
                         SYSCOUNTER_ARMV8M_CNTCR_SCEN_OFF);
}

void syscounter_armv8_m_cntrl_enable_interrupt(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    SET_BIT(p_cnt->cntcr, SYSCOUNTER_ARMV8M_CNTCR_INTRMASK_OFF);
}

void syscounter_armv8_m_cntrl_disable_interrupt(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    CLR_BIT(p_cnt->cntcr, SYSCOUNTER_ARMV8M_CNTCR_INTRMASK_OFF);
}

bool syscounter_armv8_m_cntrl_is_interrupt_enabled(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    return GET_BIT(p_cnt->cntcr,
                         SYSCOUNTER_ARMV8M_CNTCR_INTRMASK_OFF);
}

void syscounter_armv8_m_cntrl_enable_pslverr(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    SET_BIT(p_cnt->cntcr, SYSCOUNTER_ARMV8M_CNTCR_PSLVERRDIS_OFF);
}

void syscounter_armv8_m_cntrl_disable_pslverr(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    CLR_BIT(p_cnt->cntcr, SYSCOUNTER_ARMV8M_CNTCR_PSLVERRDIS_OFF);
}

bool syscounter_armv8_m_cntrl_is_pslverr_enabled(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    return GET_BIT(p_cnt->cntcr,
                         SYSCOUNTER_ARMV8M_CNTCR_PSLVERRDIS_OFF);
}

void syscounter_armv8_m_cntrl_clear_interrupt(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    CLR_BIT(p_cnt->cntcr, SYSCOUNTER_ARMV8M_CNTCR_INTRCLR_OFF);
}

bool syscounter_armv8_m_cntrl_is_counter_halted_on_debug(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    return GET_BIT(p_cnt->cntsr,
                         SYSCOUNTER_ARMV8M_CNTSR_DBGH_OFF);
}

uint64_t syscounter_armv8_m_cntrl_get_counter_value(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    uint32_t high = 0;
    uint32_t low = 0;
    uint32_t high_prev = 0;
    uint64_t value = 0;

    /* Make sure the 64-bit read will be atomic to avoid overflow between
     * the low and high registers read
     */
    high = p_cnt->cntcv_high;
    do {
        high_prev = high;
        low = p_cnt->cntcv_low;
        high = p_cnt->cntcv_high;
    }while(high != high_prev);

    value = low |
            (((uint64_t)high) << SYSCOUNTER_ARMV8_M_CNTRL_REGISTER_BIT_WIDTH);
    return value;
}

void syscounter_armv8_m_cntrl_set_counter_value(
        struct syscounter_armv8_m_cntrl_dev_t* dev,
        uint64_t value)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    p_cnt->cntcv_low = value & UINT32_MAX;
    p_cnt->cntcv_high = value >> SYSCOUNTER_ARMV8_M_CNTRL_REGISTER_BIT_WIDTH;
}

bool syscounter_armv8_m_cntrl_is_counter_scaling_implemented(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    return GET_BIT(p_cnt->cntid,
                         SYSCOUNTER_ARMV8M_CNTID_CNTSC_OFF);
}

bool syscounter_armv8_m_cntrl_is_clock_switching_implemented(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    return GET_BIT(p_cnt->cntid,
                         SYSCOUNTER_ARMV8M_CNTID_CNTCS_OFF);
}

enum syscounter_armv8_m_cntrl_selclk_t
syscounter_armv8_m_cntrl_get_clock_source(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    return (enum syscounter_armv8_m_cntrl_selclk_t)
            GET_BIT_FIELD(p_cnt->cntid,
                         SYSCOUNTER_ARMV8M_CNTID_CNTSELCLK_MASK,
                         SYSCOUNTER_ARMV8M_CNTID_CNTSELCLK_OFF);
}

enum syscounter_armv8_m_cntrl_cntscr_ovr_t
syscounter_armv8_m_cntrl_get_override_cntscr(
        struct syscounter_armv8_m_cntrl_dev_t* dev)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    return (enum syscounter_armv8_m_cntrl_cntscr_ovr_t)
            GET_BIT(p_cnt->cntid,
                    SYSCOUNTER_ARMV8M_CNTID_CNTSCR_OVR_OFF);
}

enum syscounter_armv8_m_cntrl_error_t
syscounter_armv8_m_cntrl_get_counter_scale_value(
        struct syscounter_armv8_m_cntrl_dev_t* dev,
        enum syscounter_armv8_m_cntrl_scale_nr_t nr,
        struct syscounter_armv8_m_cntrl_scale_val_t *val)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;

    switch (nr) {
    case SYSCOUNTER_ARMV8_M_SCALE_NR_0:
        val->integer = p_cnt->cntscr0 >> SYSCOUNTER_ARMV8_M_SCALE_VAL_INT_OFF;
        val->fixed_point_fraction = p_cnt->cntscr0 &
                                     SYSCOUNTER_ARMV8_M_SCALE_VAL_FRACT_MAX;
        break;
    case SYSCOUNTER_ARMV8_M_SCALE_NR_1:
        val->integer = p_cnt->cntscr1 >> SYSCOUNTER_ARMV8_M_SCALE_VAL_INT_OFF;
        val->fixed_point_fraction = p_cnt->cntscr1 &
                                     SYSCOUNTER_ARMV8_M_SCALE_VAL_FRACT_MAX;
        break;
    default:
        val->integer = 0;
        val->fixed_point_fraction = 0;
        return SYSCOUNTER_ARMV8_M_ERR_INVALID_ARG;
    }

    return SYSCOUNTER_ARMV8_M_ERR_NONE;
}

enum syscounter_armv8_m_cntrl_error_t
syscounter_armv8_m_cntrl_set_counter_scale_value(
        struct syscounter_armv8_m_cntrl_dev_t* dev,
        enum syscounter_armv8_m_cntrl_scale_nr_t nr,
        struct syscounter_armv8_m_cntrl_scale_val_t val)
{
    struct cnt_control_base_reg_map_t* p_cnt =
            (struct cnt_control_base_reg_map_t*)dev->cfg->base;
    uint32_t reg_val = 0;

    if ((syscounter_armv8_m_cntrl_get_override_cntscr(dev) ==
        SYSCOUNTER_ARMV8_M_CNTSCR_IF_DISABLED) &&
        syscounter_armv8_m_cntrl_is_counter_enabled(dev)) {
        return SYSCOUNTER_ARMV8_M_ERR_INVALID;
    }
    if (val.integer > SYSCOUNTER_ARMV8_M_SCALE_VAL_INT_MAX ||
        val.fixed_point_fraction > SYSCOUNTER_ARMV8_M_SCALE_VAL_FRACT_MAX) {
        return SYSCOUNTER_ARMV8_M_ERR_INVALID_ARG;
    }

    reg_val = val.integer << SYSCOUNTER_ARMV8_M_SCALE_VAL_INT_OFF;
    reg_val |= (val.fixed_point_fraction &
                SYSCOUNTER_ARMV8_M_SCALE_VAL_FRACT_MAX);

    switch (nr) {
    case SYSCOUNTER_ARMV8_M_SCALE_NR_0:
        p_cnt->cntscr0 = reg_val;
        break;
    case SYSCOUNTER_ARMV8_M_SCALE_NR_1:
        p_cnt->cntscr1 = reg_val;
        break;
    default:
        return SYSCOUNTER_ARMV8_M_ERR_INVALID_ARG;
    }

    return SYSCOUNTER_ARMV8_M_ERR_NONE;
}
