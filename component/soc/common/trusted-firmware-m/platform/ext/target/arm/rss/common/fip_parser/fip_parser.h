/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __FIP_PARSER_H__
#define __FIP_PARSER_H__

#include "firmware_image_package.h"
#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief                    Parse a FIP and retrieve the offset and size of one
 *                           of the firmware images (specified by UUID).
 *
 * \param[in]  fip_base      The RSS address mapped to the FIP base address in
 *                           host flash.
 * \param[in]  atu_slot_size The size of the ATU region that was mapped for
 *                           access to this FIP. This is used to prevent reads
 *                           outside the mapped region.
 * \param[in]  uuid          The UUID of the firmware image to get the offset
 *                           and size of.
 *
 * \param[out] offset        The offset in host flash of the firmware image.
 * \param[out] size          The size of the firmware image.
 *
 * \return                   0 if operation completed successfully, another
 *                           value on error.
 */
int fip_get_entry_by_uuid(uint32_t fip_base, uint32_t atu_slot_size, uuid_t uuid,
                          uint64_t *offset, size_t *size);

#ifdef __cplusplus
}
#endif

#endif /* __FIP_PARSER_H__ */
