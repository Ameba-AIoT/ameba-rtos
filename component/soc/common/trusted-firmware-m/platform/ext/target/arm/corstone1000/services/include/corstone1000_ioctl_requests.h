/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CORSTONE1000_IOCTL_REQUESTS_H
#define CORSTONE1000_IOCTL_REQUESTS_H

#include<stdint.h>


enum corstone1000_ioctl_id_t {
   IOCTL_CORSTONE1000_FWU_FLASH_IMAGES = 0,
   IOCTL_CORSTONE1000_FWU_HOST_ACK,
   IOCTL_CORSTONE1000_FMP_GET_IMAGE_INFO,
};

#endif /* CORSTONE1000_IOCTL_REQUESTS_H */
