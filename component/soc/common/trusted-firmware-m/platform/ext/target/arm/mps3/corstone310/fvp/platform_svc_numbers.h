/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __DMA_SVC_HANDLER_H__
#define __DMA_SVC_HANDLER_H__

/* A big number, so the platform SVCs don't overlap the core SVCs */
#define TFM_SVC_PLATFORM_BASE               (20u)
#define TFM_SVC_PLATFORM_DMA350_CONFIG      (TFM_SVC_PLATFORM_BASE + 1)

#endif /* __DMA_SVC_HANDLER_H__ */
