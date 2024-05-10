/*
 * Copyright (c) 2014-2022, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FIRMWARE_IMAGE_PACKAGE_H
#define FIRMWARE_IMAGE_PACKAGE_H

#include <stdint.h>

#include "uuid.h"

/* This is used as a signature to validate the blob header */
#define TOC_HEADER_NAME 0xAA640001

typedef struct fip_toc_header {
    uint32_t name;
    uint32_t serial_number;
    uint64_t flags;
} fip_toc_header_t;

typedef struct fip_toc_entry {
    uuid_t   uuid;
    uint64_t offset_address;
    uint64_t size;
    uint64_t flags;
} fip_toc_entry_t;

#endif /* FIRMWARE_IMAGE_PACKAGE_H */
