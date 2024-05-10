/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSS_COMMS_QUEUE_H__
#define __RSS_COMMS_QUEUE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t queue_enqueue(void *entry);

int32_t queue_dequeue(void **entry);

#ifdef __cplusplus
}
#endif

#endif /* __RSS_COMMS_QUEUE_H__ */
