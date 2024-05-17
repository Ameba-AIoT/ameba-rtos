/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "dma350_ch_drv.h"
#include "dma350_lib.h"

#include <arm_cmse.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* Header for target specific MPU definitions */
#ifndef CMSIS_device_header
/* CMSIS pack default header, containing the CMSIS_device_header definition */
#include "RTE_Components.h"
#endif
#include CMSIS_device_header

/**********************************************/
/************** Static Functions **************/
/**********************************************/

static uint32_t dma350_remap(uint32_t addr)
{
    const struct dma350_remap_range_t* map;

    for(uint32_t i = 0; i < dma350_address_remap.size; ++i) {
        map = &dma350_address_remap.map[i];
        if(addr <= map->end && addr >= map->begin) {
            return addr + map->offset;
        }
    }
    return addr;
}

static enum dma350_lib_error_t dma350_runcmd(struct dma350_ch_dev_t* dev,
                                        enum dma350_lib_exec_type_t exec_type)
{
    union dma350_ch_status_t status;

    /* Extra setup based on execution type */
    switch(exec_type) {
        case DMA350_LIB_EXEC_IRQ:
            dma350_ch_enable_intr(dev, DMA350_CH_INTREN_DONE);
            break;
        case DMA350_LIB_EXEC_START_ONLY:
        case DMA350_LIB_EXEC_BLOCKING:
            dma350_ch_disable_intr(dev, DMA350_CH_INTREN_DONE);
            break;
        default:
            return DMA350_LIB_ERR_INVALID_EXEC_TYPE;
    }

    dma350_ch_cmd(dev, DMA350_CH_CMD_ENABLECMD);

    /* Return or check based on execution type */
    switch(exec_type) {
        case DMA350_LIB_EXEC_IRQ:
        case DMA350_LIB_EXEC_START_ONLY:
            if (dma350_ch_is_stat_set(dev, DMA350_CH_STAT_ERR)) {
                return DMA350_LIB_ERR_CMD_ERR;
            }
            break;
        case DMA350_LIB_EXEC_BLOCKING:
            status = dma350_ch_wait_status(dev);
            if (!status.b.STAT_DONE || status.b.STAT_ERR) {
                return DMA350_LIB_ERR_CMD_ERR;
            }
            break;
        /* default is handled above */
    }

    return DMA350_LIB_ERR_NONE;
}

static uint8_t get_default_memattr(uint32_t address)
{
    uint8_t mpu_attribute;
    switch ((address >> 29) & 0x7) /* Get top 3 bits */
    {
    case (0): // CODE region, WT-RA
        // Use same attribute for inner and outer
        mpu_attribute = ARM_MPU_ATTR((ARM_MPU_ATTR_MEMORY_(0, 0, 1, 0)), (ARM_MPU_ATTR_MEMORY_(0, 0, 1, 0))); // NT=0, WB=0, RA=1, WA=0
        break;
    case (1): // SRAM region, WB-WA-RA
        // Use same attribute for inner and outer
        mpu_attribute = ARM_MPU_ATTR((ARM_MPU_ATTR_MEMORY_(0, 1, 1, 1)), (ARM_MPU_ATTR_MEMORY_(0, 1, 1, 1))); // NT=0, WB=1, RA=1, WA=1
        break;
    case (2): // Peripheral region (Shareable)
        mpu_attribute = ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE, ARM_MPU_ATTR_DEVICE_nGnRE);
        break;
    case (3): // SRAM region, WB-WA-RA
        // Use same attribute for inner and outer
        mpu_attribute = ARM_MPU_ATTR((ARM_MPU_ATTR_MEMORY_(0, 1, 1, 1)), (ARM_MPU_ATTR_MEMORY_(0, 1, 1, 1))); // NT=0, WB=1, RA=1, WA=1
        break;
    case (4): // SRAM region, WT-RA
        // Use same attribute for inner and outer
        mpu_attribute = ARM_MPU_ATTR((ARM_MPU_ATTR_MEMORY_(0, 0, 1, 0)), (ARM_MPU_ATTR_MEMORY_(0, 0, 1, 0))); // NT=0, WB=0, RA=1, WA=0
        break;
    case (5): // Device region (Shareable)
        mpu_attribute = ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE, ARM_MPU_ATTR_DEVICE_nGnRE);
        break;
    case (6): // Device region (Shareable)
        mpu_attribute = ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE, ARM_MPU_ATTR_DEVICE_nGnRE);
        break;
    default: // System / Vendor specific
        if ((address < 0xE0100000UL))
        {
            mpu_attribute = ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE, ARM_MPU_ATTR_DEVICE_nGnRnE); // PPB
        } else {
            mpu_attribute = ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE, ARM_MPU_ATTR_DEVICE_nGnRE); // Vendor
        }
        break;
    } /* end switch */
    return mpu_attribute;
}

struct dma350_memattr {
    bool nonsecure;
    bool unprivileged;
    uint8_t mpu_attribute;
    uint8_t mpu_shareability;
};

static enum dma350_lib_error_t dma350_get_memattr(void* address,
                                                struct dma350_memattr* memattr,
                                                bool writable)
{
    cmse_address_info_t address_info;
    MPU_Type *Selected_MPU; /* Pointer to selected MPU (MPU / MPU_NS) */
    uint32_t mpu_attri_raw, saved_MPU_RNR;
    uint8_t mpu_attr_idx;
    memattr->mpu_attribute = 0;
    memattr->mpu_shareability = 0;
#if defined(__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    /* Check if address is readable by non-secure, then if by unprivileged */
    /* Secure state - alternate (NS) MPU alias availabe */
    /* Check if address is readable by privileged (NS) */
    address_info = cmse_TTA(address);
    if(writable ? address_info.flags.nonsecure_readwrite_ok
                : address_info.flags.nonsecure_read_ok) {
        memattr->nonsecure = true;
        Selected_MPU = MPU_NS; /* Use non-secure MPU for attr lookup */
        /* Check if address is readable by unprivileged (NS) */
        /* Updating address_info is OK, as MPU region and its validity is set
         * regardless of whether the address is accessible by unprivileged */
        address_info = cmse_TTAT(address);
        if(writable ? address_info.flags.nonsecure_readwrite_ok
                    : address_info.flags.nonsecure_read_ok)
        {
            memattr->unprivileged = true;
        } else {
            memattr->unprivileged = false;
        }
    } else {
        /* Target memory only readable by secure */
        memattr->nonsecure = false;
        Selected_MPU = MPU; /* Use secure MPU for attr lookup */
        /* Check if address is readable by unprivileged (S) */
        /* Update address_info, as Alternate lookup provided no valid region */
        /* Unprivileged lookup is OK as MPU region and its validity is set
         * regardless of whether the address is accessible by unprivileged */
        address_info = cmse_TTT(address);
        if(writable ? address_info.flags.readwrite_ok
                    : address_info.flags.read_ok) {
            memattr->unprivileged = true;
        } else {
            memattr->unprivileged = false;
        }
    }
#else
    /* Non-secure state */
    /* Check if address is readable by privileged (NS) */
    address_info = cmse_TT(address);
    if(writable ? address_info.flags.readwrite_ok
                : address_info.flags.read_ok) {
        memattr->nonsecure = true;
        Selected_MPU = MPU; /* Only non-aliased MPU available (== MPU_NS) */
        /* Check if address is readable by unprivileged (NS) */
        /* Updating address_info is OK, as MPU region and its validity is set
         * regardless of whether the address is accessible by unprivileged */
        address_info = cmse_TTT(address);
        if(writable ? address_info.flags.readwrite_ok
                    : address_info.flags.read_ok)
        {
            memattr->unprivileged = true;
        } else {
            memattr->unprivileged = false;
        }
    } else {
        /* Target memory not readable by non-secure */
        return DMA350_LIB_ERR_RANGE_NOT_ACCESSIBLE;
    }
#endif
    if ((Selected_MPU->CTRL & MPU_CTRL_ENABLE_Msk) &&
            address_info.flags.mpu_region_valid) {
        /* MPU is enabled, lookup attributes */
        saved_MPU_RNR = Selected_MPU->RNR;                 /* Save MPU_RNR */
        Selected_MPU->RNR = address_info.flags.mpu_region; /* Select Region */
        mpu_attr_idx = (Selected_MPU->RLAR & MPU_RLAR_AttrIndx_Msk)
                        >> MPU_RLAR_AttrIndx_Pos;
        memattr->mpu_shareability = (Selected_MPU->RBAR & MPU_RBAR_SH_Msk)
                        >> MPU_RBAR_SH_Pos;
        if (mpu_attr_idx > 3)
        {
            mpu_attri_raw = Selected_MPU->MAIR[1];         /* ATTR4 - ATTR7 */
        }
        else
        {
            mpu_attri_raw = Selected_MPU->MAIR[0];         /* ATTR0 - ATTR3 */
        }
        Selected_MPU->RNR = saved_MPU_RNR;                 /* Restore MPU_RNR */
        /* Extract 8-bit attribute */
        memattr->mpu_attribute = (mpu_attri_raw >> ((mpu_attr_idx & 0x3) << 3)) & 0xFFUL;
    } else {
        /* If MPU is not enabled, use privileged access */
        memattr->unprivileged = false;
        /* Default memory map lookup for attributes */
        memattr->mpu_attribute = get_default_memattr((uint32_t)address);
    }

    return DMA350_LIB_ERR_NONE;
}


/**********************************************/
/************** Public Functions **************/
/**********************************************/

enum dma350_lib_error_t dma350_lib_set_src(struct dma350_ch_dev_t* dev,
                                           const void* src)
{
    struct dma350_memattr memattr;
    enum dma350_lib_error_t lib_err;

    lib_err = verify_dma350_ch_dev_ready(dev);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }

    lib_err = dma350_get_memattr((void*)src, &memattr, false);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }

    if(memattr.nonsecure) {
        dma350_ch_set_src_trans_nonsecure(dev);
    } else {
        dma350_ch_set_src_trans_secure(dev);
    }
    if(memattr.unprivileged) {
        dma350_ch_set_src_trans_unprivileged(dev);
    } else {
        dma350_ch_set_src_trans_privileged(dev);
    }
    dma350_ch_set_srcmemattr(dev, memattr.mpu_attribute,
                             memattr.mpu_shareability);
    dma350_ch_set_src(dev, dma350_remap((uint32_t)src));

    return DMA350_LIB_ERR_NONE;
}

enum dma350_lib_error_t dma350_lib_set_des(struct dma350_ch_dev_t* dev,
                                           void* des)
{
    struct dma350_memattr memattr;
    enum dma350_lib_error_t lib_err;

    lib_err = verify_dma350_ch_dev_ready(dev);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }

    lib_err = dma350_get_memattr(des, &memattr, true);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }

    if(memattr.nonsecure) {
        dma350_ch_set_des_trans_nonsecure(dev);
    } else {
        dma350_ch_set_des_trans_secure(dev);
    }
    if(memattr.unprivileged) {
        dma350_ch_set_des_trans_unprivileged(dev);
    } else {
        dma350_ch_set_des_trans_privileged(dev);
    }
    dma350_ch_set_desmemattr(dev, memattr.mpu_attribute,
                             memattr.mpu_shareability);
    dma350_ch_set_des(dev, dma350_remap((uint32_t)des));

    return DMA350_LIB_ERR_NONE;
}

enum dma350_lib_error_t dma350_lib_set_src_des(struct dma350_ch_dev_t* dev,
                                               const void* src, void* des,
                                               uint32_t src_size,
                                               uint32_t des_size)
{
    enum dma350_lib_error_t lib_err;

    lib_err = verify_dma350_ch_dev_ready(dev);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }
    if(NULL == cmse_check_address_range((void*)src, src_size, CMSE_MPU_READ)) {
        return DMA350_LIB_ERR_RANGE_NOT_ACCESSIBLE;
    }
    if(NULL == cmse_check_address_range(des, des_size, CMSE_MPU_READWRITE)) {
        return DMA350_LIB_ERR_RANGE_NOT_ACCESSIBLE;
    }
    lib_err = dma350_lib_set_src(dev, src);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }
    lib_err = dma350_lib_set_des(dev, des);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }
    return DMA350_LIB_ERR_NONE;
}

enum dma350_lib_error_t dma350_memcpy(struct dma350_ch_dev_t* dev,
                                    const void* src, void* des, uint32_t size,
                                    enum dma350_lib_exec_type_t exec_type)
{
    enum dma350_lib_error_t lib_err;

    lib_err = verify_dma350_ch_dev_ready(dev);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }

    lib_err = dma350_lib_set_src_des(dev, src, des, size, size);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }
    dma350_ch_set_xaddr_inc(dev, 1, 1);

    if (size > 0xFFFF) {
        dma350_ch_set_xsize32(dev, size, size);
    }
    else {
        dma350_ch_set_xsize16(dev, (uint16_t)size, (uint16_t)size);
    }
    dma350_ch_set_transize(dev, DMA350_CH_TRANSIZE_8BITS);
    dma350_ch_set_xtype(dev, DMA350_CH_XTYPE_CONTINUE);
    dma350_ch_set_ytype(dev, DMA350_CH_YTYPE_DISABLE);

    return dma350_runcmd(dev, exec_type);
}

enum dma350_lib_error_t dma350_memmove(struct dma350_ch_dev_t* dev,
                                    const void* src, void* des, uint32_t size,
                                    enum dma350_lib_exec_type_t exec_type)
{
    enum dma350_lib_error_t lib_err;

    lib_err = verify_dma350_ch_dev_ready(dev);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }

    if (src < des && (((const uint8_t*)src) + size) > (uint8_t*)des) {
        /* Start from the end if the end of the source overlaps with
           the start of the destination */
        src = (const uint8_t*) (((const uint8_t*)src) + size - 1);
        des = (uint8_t*) (((uint8_t*)des) + size - 1);
        dma350_ch_set_xaddr_inc(dev, -1, -1);
    }
    else {
        dma350_ch_set_xaddr_inc(dev, 1, 1);
    }

    lib_err = dma350_lib_set_src(dev, src);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }
    lib_err = dma350_lib_set_des(dev, des);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }
    dma350_ch_set_xsize32(dev, size, size);
    dma350_ch_set_transize(dev, DMA350_CH_TRANSIZE_8BITS);
    dma350_ch_set_xtype(dev, DMA350_CH_XTYPE_CONTINUE);
    dma350_ch_set_ytype(dev, DMA350_CH_YTYPE_DISABLE);

    return dma350_runcmd(dev, exec_type);
}

enum dma350_lib_error_t dma350_endian_swap(struct dma350_ch_dev_t* dev,
                                        const void* src, void* des,
                                        uint8_t size, uint32_t count)
{
    uint32_t remaining = 0;
    enum dma350_lib_error_t lib_err;
    const uint8_t *ptr8 = (const uint8_t*) src;

    lib_err = verify_dma350_ch_dev_ready(dev);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }

    lib_err = dma350_lib_set_des(dev, des);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }
    /* First copy will always start at size - 1 offset, memory attributes are
     * expected to be constant whole the whole affected memory, so it is enough
     * to set the memory attributes once, then only update the src address. */
    lib_err = dma350_lib_set_src(dev, &ptr8[size - 1]);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }
    dma350_ch_set_xtype(dev, DMA350_CH_XTYPE_CONTINUE);
    dma350_ch_set_ytype(dev, DMA350_CH_YTYPE_CONTINUE);
    dma350_ch_set_transize(dev, DMA350_CH_TRANSIZE_8BITS);
    dma350_ch_set_xaddr_inc(dev, -1, 1);
    dma350_ch_set_yaddrstride(dev, size, 0);
    dma350_ch_set_donetype(dev, DMA350_CH_DONETYPE_END_OF_CMD);

    /* Split up the image into smaller parts to fit the ysize into 16 bits. */
    /* FIXME: command restart cannot be used, because of a bug: at the end of
     *        the command, srcaddr is not updated if yaddrstride is negative */
    remaining = count;
    while(remaining)
    {
        union dma350_ch_status_t status;
        uint16_t copy_count = remaining > UINT16_MAX ? UINT16_MAX :
                                                       (uint16_t) remaining;
        /* Start at last byte: size - 1,
         * then start at copy_count * size higher.
         * Total copied count = count - remaining */
        const uint8_t *ptr_start = &ptr8[(1 + count - remaining) * size - 1];
        dma350_ch_set_src(dev, (uint32_t) ptr_start);
        dma350_ch_set_ysize16(dev, copy_count, 1);
        dma350_ch_set_xsize32(dev, size, size * copy_count);
        remaining -= copy_count;

        dma350_ch_cmd(dev, DMA350_CH_CMD_ENABLECMD);

        /* Blocking until done as the whole operation is split into multiple
         * DMA commands.
         * Can be updated to non-blocking when FIXME above is fixed. */
        status = dma350_ch_wait_status(dev);
        if (!status.b.STAT_DONE || status.b.STAT_ERR) {
            return DMA350_LIB_ERR_CMD_ERR;
        }
    }

    return DMA350_LIB_ERR_NONE;
}

enum dma350_lib_error_t dma350_draw_from_canvas(struct dma350_ch_dev_t* dev,
                                    const void* src, void* des,
                                    uint32_t src_width, uint16_t src_height,
                                    uint16_t src_line_width,
                                    uint32_t des_width, uint16_t des_height,
                                    uint16_t des_line_width,
                                    enum dma350_ch_transize_t pixelsize,
                                    enum dma350_lib_transform_t transform,
                                    enum dma350_lib_exec_type_t exec_type)
{
    uint8_t *des_uint8_t;
    uint32_t des_offset, des_xsize;
    uint16_t des_ysize, des_yaddrstride;
    int16_t des_xaddrinc;
    enum dma350_lib_error_t lib_err;

    lib_err = verify_dma350_ch_dev_ready(dev);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }

    switch(transform) {
        case DMA350_LIB_TRANSFORM_NONE:
            des_offset = 0;
            des_xsize = des_width;
            des_ysize = des_height;
            des_xaddrinc = 1;
            des_yaddrstride = des_line_width;
            break;
        case DMA350_LIB_TRANSFORM_MIRROR_HOR:
            /* Top right */
            des_offset = des_width - 1;
            des_xsize = des_width;
            des_ysize = des_height;
            des_xaddrinc = -1;
            des_yaddrstride = des_line_width;
            break;
        case DMA350_LIB_TRANSFORM_MIRROR_VER:
            /* Bottom left */
            des_offset = (des_height - 1) * des_line_width;
            des_xsize = des_width;
            des_ysize = des_height;
            des_xaddrinc = 1;
            des_yaddrstride = -des_line_width;
            break;
        case DMA350_LIB_TRANSFORM_MIRROR_TLBR:
            if(des_width > UINT16_MAX) {
                return DMA350_LIB_ERR_CFG_ERR;
            }
            /* Bottom right */
            des_offset = (des_height-1) * des_line_width + des_width - 1;
            des_xsize = des_height;
            des_ysize = (uint16_t)des_width;
            des_xaddrinc = (int16_t)(-des_line_width);
            des_yaddrstride = (uint16_t)-1;
            break;
        case DMA350_LIB_TRANSFORM_MIRROR_TRBL:
            if(des_width > UINT16_MAX) {
                return DMA350_LIB_ERR_CFG_ERR;
            }
            des_offset = 0;
            des_xsize = des_height;
            des_ysize = (uint16_t)des_width;
            des_xaddrinc = (int16_t)des_line_width;
            des_yaddrstride = 1;
            break;
        case DMA350_LIB_TRANSFORM_ROTATE_90:
            if(des_width > UINT16_MAX) {
                return DMA350_LIB_ERR_CFG_ERR;
            }
            /* Top right */
            des_offset = des_width - 1;
            des_xsize = des_height;
            des_ysize = (uint16_t)des_width;
            des_xaddrinc = (int16_t)des_line_width;
            des_yaddrstride = (uint16_t)-1;
            break;
        case DMA350_LIB_TRANSFORM_ROTATE_180:
            /* Bottom right */
            des_offset = (des_height-1) * des_line_width + des_width - 1;
            des_xsize = des_width;
            des_ysize = des_height;
            des_xaddrinc = -1;
            des_yaddrstride = -des_line_width;
            break;
        case DMA350_LIB_TRANSFORM_ROTATE_270:
            if(des_width > UINT16_MAX) {
                return DMA350_LIB_ERR_CFG_ERR;
            }
            /* Bottom left */
            des_offset = (des_height - 1) * des_line_width;
            des_xsize = des_height;
            des_ysize = (uint16_t)des_width;
            des_xaddrinc = (int16_t)(-des_line_width);
            des_yaddrstride = 1;
            break;
        default:
            return DMA350_LIB_ERR_CFG_ERR;
    }

    /* Up until this point, offset was set as number of pixels. It needs to be
       multiplied by the size of the pixel to get the byte address offset.
       Pixel size is based on dma350_ch_transize_t which is calculated by
       2^transize, so the multiplication can be reduced to a bitshift. */
    des_offset <<= pixelsize;
    des_uint8_t = (uint8_t*) des;
    lib_err = dma350_lib_set_src(dev, src);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }
    lib_err = dma350_lib_set_des(dev, &des_uint8_t[des_offset]);
    if(lib_err != DMA350_LIB_ERR_NONE) {
        return lib_err;
    }

    dma350_ch_set_xaddr_inc(dev, 1, des_xaddrinc);
    dma350_ch_set_xsize32(dev, src_width, des_xsize);
    dma350_ch_set_ysize16(dev, src_height, des_ysize);
    dma350_ch_set_yaddrstride(dev, src_line_width, des_yaddrstride);

    dma350_ch_set_transize(dev, pixelsize);
    dma350_ch_set_xtype(dev, DMA350_CH_XTYPE_WRAP);
    dma350_ch_set_ytype(dev, DMA350_CH_YTYPE_WRAP);

    return dma350_runcmd(dev, exec_type);
}
