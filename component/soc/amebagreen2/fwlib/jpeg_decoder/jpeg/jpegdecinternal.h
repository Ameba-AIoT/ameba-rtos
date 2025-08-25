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

#ifndef JPEGDECINTERNAL_H
#define JPEGDECINTERNAL_H

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "basetype.h"
#include "jpegdecapi.h"
#include "jpegdeccontainer.h"

#ifdef JPEGDEC_ASIC_TRACE
#include <stdio.h>
#endif

/*------------------------------------------------------------------------------
    2. Module defines
------------------------------------------------------------------------------*/
#ifdef JPEGDEC_ASIC_TRACE
#define JPEGDEC_TRACE_INTERNAL(args) printf args
#else
#define JPEGDEC_TRACE_INTERNAL(args)
#endif

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/
void JpegDecClearStructs(JpegDecContainer *pJpegDecCont, u32 mode);
JpegDecRet JpegDecInitHW(JpegDecContainer *pJpegDecCont);
void JpegDecInitHWContinue(JpegDecContainer *pJpegDecCont);
void JpegDecInitHWInputBuffLoad(JpegDecContainer *pJpegDecCont);
void JpegDecInitHWProgressiveContinue(JpegDecContainer *pJpegDecCont);
void JpegDecInitHWNonInterleaved(JpegDecContainer *pJpegDecCont);
JpegDecRet JpegDecAllocateResidual(JpegDecContainer *pJpegDecCont);
void JpegDecSliceSizeCalculation(JpegDecContainer *pJpegDecCont);
JpegDecRet JpegDecNextScanHdrs(JpegDecContainer *pJpegDecCont);
void JpegRefreshRegs(JpegDecContainer *pJpegDecCont);
void JpegFlushRegs(JpegDecContainer *pJpegDecCont);
void JpegDecInitHWEmptyScan(JpegDecContainer *pJpegDecCont, u32 componentId);

#endif /* #endif JPEGDECDATA_H */
