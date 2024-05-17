/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SIC_BOOT_H__
#define __SIC_BOOT_H__

#include <stdint.h>

#include "boot_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief                  Initialise Secure I-Cache, and begin configuration.
 *
 * \return                 0 if operation completed successfully, another value
 *                         on error.
 */
int sic_boot_init(void);

/**
 * \brief                        Perform post-image-load steps to setup SIC,
 *                               for a given image.
 *
 * \param[in]  image_id          The image id to setup the SIC for.
 * \param[in]  image_load_offset The flash offset the image was loaded from.
 *                               This is used to detect which of the primary /
 *                               secondary images was loaded and determine which
 *                               code should be run through the SIC.
 *
 * \return                       0 if operation completed successfully, another
 *                               value on error.
 */
int sic_boot_post_load(uint32_t image_id, uint32_t image_load_offset);

/**
 * \brief                  Perform SIC configuration that needs to be run just
 *                         before the bootloader is exited to jump to the new
 *                         image.
 *
 * \param[out] vt_cpy      A pointer to the vector table pointer that is to be
 *                         jumped to. This is updated to the correct address
 *                         where the image can be executed via the SIC.
 *
 * \return                 0 if operation completed successfully, another value
 *                         on error.
 */
int sic_boot_pre_quit(struct boot_arm_vector_table **vt_cpy);

#ifdef __cplusplus
}
#endif

#endif /* __SIC_BOOT_H__ */
