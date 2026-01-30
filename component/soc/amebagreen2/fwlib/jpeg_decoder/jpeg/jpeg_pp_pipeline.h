/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef JPEG_PP_PIPELINE_H
#define JPEG_PP_PIPELINE_H

#include "decppif.h"

#ifdef __cplusplus
extern "C" {
#endif

i32 jpegRegisterPP(const void *decInst, const void *ppInst,
				   void (*PPRun)(const void *, const DecPpInterface *),
				   void (*PPEndCallback)(const void *),
				   void (*PPConfigQuery)(const void *, DecPpQuery *));

i32 jpegUnregisterPP(const void *decInst, const void *ppInst);

#ifdef __cplusplus
}
#endif

#endif /* #ifdef JPEG_PP_PIPELINE_H */
