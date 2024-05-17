/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

typedef enum {
    PLATFORM_GPT_IMAGE = 0,
    PLATFORM_IMAGE_COUNT,
}platform_image_id_t;

#define FWU_METADATA_TYPE_UUID \
     ((uuid_t){{0xa0, 0x84, 0x7a, 0x8a}, {0x87, 0x83}, {0xf6, 0x40}, 0xab,  0x41, {0xa8, 0xb9, 0xa5, 0xa6, 0x0d, 0x23}})
#define PRIVATE_METADATA_TYPE_UUID \
     ((uuid_t){{0xc3, 0x5d, 0xb5, 0xec}, {0xb7, 0x8a}, {0x84, 0x4a}, 0xab,  0x56, {0xeb, 0x0a, 0x99, 0x74, 0xdb, 0x42}})

/* Initialize io storage of the platform */
int32_t plat_io_storage_init(void);

/* Return an IO device handle and specification which can be used to access
 * an image. This has to be implemented for the GPT parser. */
int32_t plat_get_image_source(unsigned int image_id, uintptr_t *dev_handle,
                              uintptr_t *image_spec);

#endif /*__PLATFORM_H__*/
