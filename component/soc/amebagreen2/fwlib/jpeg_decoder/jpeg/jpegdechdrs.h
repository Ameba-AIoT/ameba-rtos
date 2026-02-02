/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*------------------------------------------------------------------------------

    Table of contents

    1. Include headers
    2. Module defines
    3. Data types
    4. Function prototypes

------------------------------------------------------------------------------*/

#ifndef JPEGDECHDRS_H
#define JPEGDECHDRS_H

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "basetype.h"
#include "jpegdeccontainer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
    2. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

JpegDecRet JpegDecDecodeFrameHdr(JpegDecContainer *pDecData);
JpegDecRet JpegDecDecodeQuantTables(JpegDecContainer *pDecData);
JpegDecRet JpegDecDecodeHuffmanTables(JpegDecContainer *pDecData);
void JpegDecDefaultHuffmanTables(JpegDecContainer *pDecData);
JpegDecRet JpegDecMode(JpegDecContainer *pDecData);
JpegDecRet JpegDecMode(JpegDecContainer *);

#ifdef __cplusplus
}
#endif

#endif /* #ifdef MODULE_H */
