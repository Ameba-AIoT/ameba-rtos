/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSS_COMMS_ATU_HAL_H__
#define __RSS_COMMS_ATU_HAL_H__

#ifndef RSS_XIP
#define RSS_COMMS_ATU_REGION_MIN        0
#else
#define RSS_COMMS_ATU_REGION_MIN        2
#endif /* !RSS_XIP */

#define RSS_COMMS_ATU_REGION_MAX        15
/* There must be at least one region */
#define RSS_COMMS_ATU_REGION_AM         (RSS_COMMS_ATU_REGION_MAX - \
                                         RSS_COMMS_ATU_REGION_MIN + 1)
#define RSS_COMMS_ATU_PAGE_SIZE         0x2000
#define RSS_COMMS_ATU_REGION_SIZE       (RSS_COMMS_ATU_PAGE_SIZE * 8)

#endif /* __RSS_COMMS_HAL_H__ */
