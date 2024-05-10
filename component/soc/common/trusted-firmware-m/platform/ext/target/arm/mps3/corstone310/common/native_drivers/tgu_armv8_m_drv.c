/*
 * Copyright (c) 2016-2021 Arm Limited. All rights reserved.
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
#include "tgu_armv8_m_drv.h"

#include <stddef.h>
#include <stdbool.h>

#include "cmsis_compiler.h"

/* CTRL register bit indexes */
#define TGU_CTRL_DBFEN  (1UL)
#define TGU_CTRL_DEREN  (1UL << 1UL)
#define TGU_ARMV8_M_CFG_TGU_PRESENT (1UL << 31UL)

#define TGU_CFG_BLKSZ_MASK (15UL) /* Bits 0-3 */
#define TGU_CFG_NUMBLKS_MASK (15UL << 8UL) /* Bits 8-11 */

struct tgu_armv8_m_reg_map_t {
    volatile uint32_t ctrl;
    volatile uint32_t cfg;
    volatile uint32_t reserved[2];
    volatile uint32_t lutn[15];
};

/*
 * Checks if the address is controlled by the TGU and returns
 * the range index in which it is contained.
 *
 * \param[in]  dev         TGU device to initialize \ref tgu_armv8_m_dev_t
 * \param[in]  addr        Address to check if it is controlled by TGU.
 * \param[out] addr_range  Range index in which it is contained.
 *
 * \return True if the base is controller by the range list, false otherwise.
 */
static uint32_t is_ctrl_by_range_list(
                            struct tgu_armv8_m_dev_t* dev,
                            uint32_t addr,
                            const struct tgu_armv8_m_mem_range_t** addr_range)
{
    uint32_t i;
    const struct tgu_armv8_m_mem_range_t* range;

    for(i = 0; i < dev->data->nbr_of_ranges; i++) {
        range = dev->data->range_list[i];
        if(addr >= range->base && addr <= range->limit) {
            *addr_range = range;
            return 1;
        }
    }
    return 0;
}

static enum tgu_armv8_m_error_t get_lut_masks(
                                 struct tgu_armv8_m_dev_t* dev,
                                 const uint32_t base, const uint32_t limit,
                                 const struct tgu_armv8_m_mem_range_t** range,
                                 uint32_t *first_word_idx,
                                 uint32_t *nr_words,
                                 uint32_t *first_word_mask,
                                 uint32_t *limit_word_mask)
{
    const struct tgu_armv8_m_mem_range_t* base_range;
    uint32_t block_size;
    uint32_t base_block_idx;
    uint32_t base_word_idx;
    uint32_t blk_max;
    const struct tgu_armv8_m_mem_range_t* limit_range;
    uint32_t limit_block_idx;
    uint32_t limit_word_idx;
    uint32_t mask;
    uint32_t norm_base;
    uint32_t norm_limit;
    struct tgu_armv8_m_reg_map_t* p_tgu =
                                   (struct tgu_armv8_m_reg_map_t*)dev->cfg->base;

    /*
     * Check that the addresses are within the controlled regions
     * of this TGU
     */
    if(!is_ctrl_by_range_list(dev, base, &base_range) ||
       !is_ctrl_by_range_list(dev, limit, &limit_range)) {
        return TGU_INTERN_ERR_NOT_IN_RANGE;
    }

    /* Base and limit should be part of the same range */
    if(base_range != limit_range) {
        return TGU_INTERN_ERR_NOT_IN_RANGE;
    }
    *range = base_range;

    block_size = ( 1 << ((p_tgu->cfg & TGU_CFG_BLKSZ_MASK) + 5));

    /* Base and limit+1 addresses must be aligned on the TGU block size */
    if(base % block_size || (limit+1) % block_size) {
        return TGU_INTERN_ERR_NOT_ALIGNED;
    }

    /*
     * Get a normalized address that is an offset from the beginning
     * of the lowest range controlled by the TGU
     */
    norm_base  = (base - base_range->base) + base_range->range_offset;
    norm_limit = (limit - base_range->base) + base_range->range_offset;

    /*
     * Calculate block index and to which 32 bits word it belongs
     */
    limit_block_idx = norm_limit/block_size;
    limit_word_idx = limit_block_idx/32;

    base_block_idx = norm_base/block_size;
    base_word_idx = base_block_idx/32;

    if(base_block_idx > limit_block_idx) {
        return TGU_INTERN_ERR_INVALID_RANGE;
    }

    /* Transmit the information to the caller */
    *nr_words = limit_word_idx - base_word_idx + 1;
    *first_word_idx = base_word_idx;

    /* Limit to the highest block that can be configured */
    blk_max = 15;

    if((limit_word_idx > blk_max) || (base_word_idx > blk_max)) {
        return TGU_INTERN_ERR_BLK_IDX_TOO_HIGH;
    }

    /*
     * Create the mask for the first word to only select the limit N bits
     */
    *first_word_mask = ~((1 << (base_block_idx % 32)) - 1);

    /*
     * Create the mask for the limit word to select only the first M bits.
     */
    *limit_word_mask = (1 << ((limit_block_idx+1) % 32)) - 1;
    /*
     * If limit_word_mask is 0, it means that the limit touched block index is
     * the limit in its word, so the limit word mask has all its bits selected
     */
    if(*limit_word_mask == 0) {
        *limit_word_mask = 0xFFFFFFFF;
    }

    /*
     * If the blocks to configure are all packed in one word, only
     * touch this word.
     * Code using the computed masks should test if this mask
     * is non-zero, and if so, only use this one instead of the limit_word_mask
     * and first_word_mask.
     * As the only bits that are the same in both masks are the 1 that we want
     * to select, just use XOR to extract them.
     */
    if(base_word_idx == limit_word_idx) {
        mask = ~(*first_word_mask ^ *limit_word_mask);
        *first_word_mask = mask;
        *limit_word_mask = mask;
    }

    return TGU_ERR_NONE;
}

enum tgu_armv8_m_error_t tgu_armv8_m_init(struct tgu_armv8_m_dev_t* dev,
                                          const struct tgu_armv8_m_mem_range_t** range_list,
                                          uint8_t nbr_of_ranges)
{
    if((range_list == NULL) || (nbr_of_ranges == 0)) {
        return TGU_INVALID_ARG;
    }

    dev->data->range_list = range_list;
    dev->data->nbr_of_ranges = nbr_of_ranges;
    dev->data->is_initialized = true;

    return TGU_ERR_NONE;
}

enum tgu_armv8_m_error_t tgu_armv8_m_get_block_size(struct tgu_armv8_m_dev_t* dev,
                                            uint32_t* blk_size)
{
    struct tgu_armv8_m_reg_map_t* p_tgu =
                    (struct tgu_armv8_m_reg_map_t*)dev->cfg->base;

    if(dev->data->is_initialized != true) {
        return TGU_NOT_INIT;
    }
    /* Returns block size in bytes */
    *blk_size = ( 1 << ((p_tgu->cfg & TGU_CFG_BLKSZ_MASK) + 5));

    return TGU_ERR_NONE;
}

enum tgu_armv8_m_error_t tgu_armv8_m_get_number_of_prog_blocks(struct tgu_armv8_m_dev_t* dev,
                                                        uint32_t* num_blks)
{
    struct tgu_armv8_m_reg_map_t* p_tgu =
                    (struct tgu_armv8_m_reg_map_t*)dev->cfg->base;

    if(dev->data->is_initialized != true) {
        return TGU_NOT_INIT;
    }

    *num_blks = (1 << ((p_tgu->cfg & TGU_CFG_NUMBLKS_MASK) >> 8));

    return TGU_ERR_NONE;
}

enum tgu_armv8_m_error_t tgu_armv8_m_config_region(struct tgu_armv8_m_dev_t* dev,
                                           const uint32_t base,
                                           const uint32_t limit,
                                           enum tgu_armv8_m_sec_attr_t attr)
{
    enum tgu_armv8_m_error_t error;
    uint32_t first_word_idx;
    uint32_t first_word_mask;
    uint32_t i;
    uint32_t limit_word_mask;
    uint32_t limit_word_idx;
    uint32_t nr_words;
    const struct tgu_armv8_m_mem_range_t* range;
    uint32_t word_value;
    struct tgu_armv8_m_reg_map_t* p_tgu =
                                   (struct tgu_armv8_m_reg_map_t*)dev->cfg->base;

    if(dev->data->is_initialized != true) {
        return TGU_NOT_INIT;
    }

    /* Get the bitmasks used to select the bits in the LUT */
    error = get_lut_masks(dev, base, limit, &range, &first_word_idx, &nr_words,
                          &first_word_mask, &limit_word_mask);

    limit_word_idx = first_word_idx + nr_words - 1;

    if(error != TGU_ERR_NONE) {
        /* Map internal error code lower than 0 to a generic errpr */
        if(error < 0) {
            return TGU_INTERN_ERR_INVALID_RANGE;
        }
        return (enum tgu_armv8_m_error_t)error;
    }

    /*
     * The memory range should allow accesses in with the wanted security
     * attribute if it requires special attribute for successful accesses
     */
    if(range->attr != attr) {
        return TGU_ERR_RANGE_SEC_ATTR_NON_COMPATIBLE;
    }

    /*
     * Starts changing actual configuration so issue DMB to ensure every
     * transaction has completed by now
     */
    __DMB();

    /* If only one word needs to be touched in the LUT */
    if(nr_words == 1) {
        word_value = p_tgu->lutn[first_word_idx];
        if(attr == TGU_SEC_ATTR_NONSECURE) {
            word_value |= first_word_mask;
        } else {
            word_value &= ~first_word_mask;
        }

        p_tgu->lutn[first_word_idx] = word_value;

        /* Commit the configuration change */
        __DSB();
        __ISB();

        return TGU_ERR_NONE;
    }

    /* First word */
    word_value = p_tgu->lutn[first_word_idx];
    if(attr == TGU_SEC_ATTR_NONSECURE) {
        word_value |= first_word_mask;
    } else {
        word_value &= ~first_word_mask;
    }

    /* Partially configure the first word */
    p_tgu->lutn[first_word_idx] = word_value;

    /* Fully configure the intermediate words if there are any */
    for(i=first_word_idx+1; i<limit_word_idx; i++) {
        if(attr == TGU_SEC_ATTR_NONSECURE) {
            p_tgu->lutn[i] = 0xFFFFFFFF;
        } else {
            p_tgu->lutn[i] = 0x00000000;
        }
    }

    /* Partially configure the limit word */
    word_value = p_tgu->lutn[limit_word_idx];
    if(attr == TGU_SEC_ATTR_NONSECURE) {
        word_value |= limit_word_mask;
    } else {
        word_value &= ~limit_word_mask;
    }

    p_tgu->lutn[limit_word_idx] = word_value;

    /* Commit the configuration change */
    __DSB();
    __ISB();

    return TGU_ERR_NONE;
}

enum tgu_armv8_m_error_t tgu_armv8_m_get_region_config(struct tgu_armv8_m_dev_t* dev,
                                               uint32_t base,
                                               uint32_t limit,
                                               enum tgu_armv8_m_sec_attr_t* attr)
{
    enum tgu_armv8_m_sec_attr_t attr_prev;
    uint32_t block_size;
    uint32_t block_size_mask;
    enum tgu_armv8_m_error_t error;
    uint32_t first_word_idx;
    uint32_t first_word_mask;
    uint32_t i;
    uint32_t limit_word_idx;
    uint32_t limit_word_mask;
    uint32_t nr_words;
    struct tgu_armv8_m_reg_map_t* p_tgu =
                                   (struct tgu_armv8_m_reg_map_t*)dev->cfg->base;
    const struct tgu_armv8_m_mem_range_t* range;
    uint32_t word_value;

    if(dev->data->is_initialized != true) {
        return TGU_NOT_INIT;
    }

    if(attr == 0) {
        return TGU_INVALID_ARG;
    }

    /*
     * Initialize the security attribute to mixed in case of early
     * termination of this function. A caller that does not check the
     * returned error will act as if it does not know anything about the
     * region queried, which is the safest bet
     */
    *attr = TGU_SEC_ATTR_MIXED;

    /*
     * If the base and limit are not aligned, align them and make sure
     * that the resulting region fully includes the original region
     */
    block_size = ( 1 << ((p_tgu->cfg & TGU_CFG_BLKSZ_MASK) + 5));

    block_size_mask = block_size - 1;
    base &= ~(block_size_mask);
    limit &= ~(block_size_mask);
    limit += block_size - 1; /* Round to the upper block address,
                              * and then remove one to get the preceding
                              * address.
                              */

    /* Get the bitmasks used to select the bits in the LUT */
    error = get_lut_masks(dev, base, limit, &range, &first_word_idx, &nr_words,
                          &first_word_mask, &limit_word_mask);

    limit_word_idx = first_word_idx+nr_words - 1;

    if(error != TGU_ERR_NONE) {
        /* Map internal error code lower than 0 to generic error */
        if(error < 0) {
            return TGU_INTERN_ERR_INVALID_RANGE;
        }
        return (enum tgu_armv8_m_error_t)error;
    }

    /* If only one word needs to be touched in the LUT */
    if(nr_words == 1) {
        word_value =  p_tgu->lutn[first_word_idx];
        word_value &= first_word_mask;
        if(word_value == 0) {
            *attr = TGU_SEC_ATTR_SECURE;
        /*
         * If there are differences between the mask and the word value,
         * it means that the security attributes of blocks are mixed
         */
        } else if(word_value ^ first_word_mask) {
            *attr = TGU_SEC_ATTR_MIXED;
        } else {
            *attr = TGU_SEC_ATTR_NONSECURE;
        }
        return TGU_ERR_NONE;
    }

    /* Get the partial configuration of the first word */
    word_value =  p_tgu->lutn[first_word_idx] & first_word_mask;
    if(word_value == 0x00000000) {
        *attr = TGU_SEC_ATTR_SECURE;
    } else if(word_value ^ first_word_mask) {
        *attr = TGU_SEC_ATTR_MIXED;
        /*
         * Bail out as the security attribute will be the same regardless
         * of the configuration of other blocks
         */
        return TGU_ERR_NONE;
    } else {
        *attr = TGU_SEC_ATTR_NONSECURE;
    }
    /*
     * Store the current found attribute, to check that all the blocks indeed
     * have the same security attribute.
     */
    attr_prev = *attr;

    /* Get the configuration of the intermediate words if there are any */
    for(i=first_word_idx+1; i<limit_word_idx; i++) {
        word_value =  p_tgu->lutn[i];
        if(word_value == 0x00000000) {
            *attr = TGU_SEC_ATTR_SECURE;
        } else if(word_value == 0xFFFFFFFF) {
            *attr = TGU_SEC_ATTR_NONSECURE;
        } else {
            *attr = TGU_SEC_ATTR_MIXED;
            return TGU_ERR_NONE;
        }

        /* If the attribute is different than the one found before, bail out */
        if(*attr != attr_prev) {
            *attr = TGU_SEC_ATTR_MIXED;
            return TGU_ERR_NONE;
        }
        attr_prev = *attr;
    }

    /* Get the partial configuration of the limit word */
    word_value =  p_tgu->lutn[limit_word_idx] & limit_word_mask;
    if(word_value == 0x00000000) {
        *attr = TGU_SEC_ATTR_SECURE;
    } else if(word_value ^ first_word_mask) {
        *attr = TGU_SEC_ATTR_MIXED;
        return TGU_ERR_NONE;
    } else {
        *attr = TGU_SEC_ATTR_NONSECURE;
    }

    if(*attr != attr_prev) {
        *attr = TGU_SEC_ATTR_MIXED;
        return TGU_ERR_NONE;
    }

    return TGU_ERR_NONE;
}

enum tgu_armv8_m_error_t tgu_armv8_m_get_ctrl(struct tgu_armv8_m_dev_t* dev,
                                      uint32_t* ctrl_val)
{
    struct tgu_armv8_m_reg_map_t* p_tgu =
                                   (struct tgu_armv8_m_reg_map_t*)dev->cfg->base;

    if(dev->data->is_initialized != true) {
        return TGU_NOT_INIT;
    }

    if(ctrl_val == 0) {
        return TGU_INVALID_ARG;
    }

    *ctrl_val = p_tgu->ctrl;

    return TGU_ERR_NONE;
}

enum tgu_armv8_m_error_t tgu_armv8_m_set_ctrl(struct tgu_armv8_m_dev_t* dev,
                                      uint32_t tgu_ctrl)
{
    struct tgu_armv8_m_reg_map_t* p_tgu =
                                   (struct tgu_armv8_m_reg_map_t*)dev->cfg->base;

    if(dev->data->is_initialized != true) {
        return TGU_NOT_INIT;
    }

    p_tgu->ctrl = tgu_ctrl;

    return TGU_ERR_NONE;
}

enum tgu_armv8_m_error_t tgu_armv8_m_is_tgu_present(struct tgu_armv8_m_dev_t* dev,
                                               bool* tgu_present)
{
    struct tgu_armv8_m_reg_map_t* p_tgu =
                                   (struct tgu_armv8_m_reg_map_t*)dev->cfg->base;

    if(dev->data->is_initialized != true) {
        return TGU_NOT_INIT;
    }

    *tgu_present = (bool)(p_tgu->cfg & TGU_ARMV8_M_CFG_TGU_PRESENT);

    return TGU_ERR_NONE;
}
