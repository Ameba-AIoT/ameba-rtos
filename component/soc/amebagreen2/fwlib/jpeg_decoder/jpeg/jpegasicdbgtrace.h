/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __JPEGDEC_TRACE__
#define __JPEGDEC_TRACE__
#include <stdio.h>

#include "basetype.h"
#include "jpegdeccontainer.h"

#ifdef __cplusplus
extern "C" {
#endif

void PrintJPEGReg(u32 *regBase);
void ppRegDump(const u32 *regBase);
void DumpJPEGCtrlReg(u32 *regBase, FILE *fd);
void HexDumpJPEGCtrlReg(u32 *regBase, FILE *fd);
void HexDumpJPEGTables(u32 *regBase, JpegDecContainer *pJpegDecCont, FILE *fd);
void HexDumpJPEGOutput(JpegDecContainer *pJpegDecCont, FILE *fd);
void HexDumpRegs(u32 *regBase, FILE *fd);
void ResetAsicBuffers(JpegDecContainer *pJpegDecCont, FILE *fd);

#ifdef __cplusplus
}
#endif

#endif /* __JPEGDEC_TRACE__ */
