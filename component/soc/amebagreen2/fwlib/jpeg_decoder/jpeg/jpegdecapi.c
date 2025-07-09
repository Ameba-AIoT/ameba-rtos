/*------------------------------------------------------------------------------
--         Copyright (c) 2015, VeriSilicon Inc. All rights reserved           --
--         Copyright (c) 2011-2014, Google Inc. All rights reserved.          --
--         Copyright (c) 2007-2010, Hantro OY. All rights reserved.           --
--                                                                            --
-- This software is confidential and proprietary and may be used only as      --
--   expressly authorized by VeriSilicon in a written licensing agreement.    --
--                                                                            --
--         This entire notice must be reproduced on all copies                --
--                       and may not be removed.                              --
--                                                                            --
--------------------------------------------------------------------------------
-- Redistribution and use in source and binary forms, with or without         --
-- modification, are permitted provided that the following conditions are met:--
--   * Redistributions of source code must retain the above copyright notice, --
--       this list of conditions and the following disclaimer.                --
--   * Redistributions in binary form must reproduce the above copyright      --
--       notice, this list of conditions and the following disclaimer in the  --
--       documentation and/or other materials provided with the distribution. --
--   * Neither the names of Google nor the names of its contributors may be   --
--       used to endorse or promote products derived from this software       --
--       without specific prior written permission.                           --
--------------------------------------------------------------------------------
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"--
-- AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE  --
-- IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE --
-- ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE  --
-- LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR        --
-- CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF       --
-- SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS   --
-- INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN    --
-- CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)    --
-- ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE --
-- POSSIBILITY OF SUCH DAMAGE.                                                --
--------------------------------------------------------------------------------
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------

    Table of contents

    1. Include headers
    2. External compiler flags
    3. Module defines
    4. Local function prototypes
    5. Functions
        - JpegDecInit
        - JpegDecRelease
        - JpegDecGetImageInfo
        - JpegDecDecode
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "vcnd_version.h"
#include "dwl.h"
#include "basetype.h"
#include "jpegdecapi.h"
#include "jpegdeccontainer.h"
#include "jpegdecmarkers.h"
#include "jpegdecinternal.h"
#include "jpegdecutils.h"
#include "jpegdechdrs.h"
#include "jpegdecscan.h"
#include "jpegregdrv.h"
#include "jpeg_pp_pipeline.h"
#include "commonconfig.h"
#include "sw_util.h"

#ifdef JPEGDEC_ASIC_TRACE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jpegasicdbgtrace.h"
#endif /* #ifdef JPEGDEC_ASIC_TRACE */

#ifdef JPEGDEC_PP_TRACE
#include "ppapi.h"
#endif /* #ifdef JPEGDEC_PP_TRACE */

static void JpegDecPreparePp(JpegDecContainer *pJpegDecCont);
static u32 JpegCycleCount(JpegDecContainer *dec_cont);
#define NEXT_MULTIPLE(value, n) (((value) + (n) - 1) & ~((n) - 1))

/*------------------------------------------------------------------------------
       Version Information - DO NOT CHANGE!
------------------------------------------------------------------------------*/

#define JPG_MAJOR_VERSION VCNDSW_VERSION_MAJOR
#define JPG_MINOR_VERSION VCNDSW_VERSION_MINOR

#define JPEGDEC_X170_MAX_STREAM (1<<29) - 1

/*------------------------------------------------------------------------------
    2. External compiler flags
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/
#ifdef JPEGDEC_TRACE
#define JPEGDEC_API_TRC(str)    JpegDecTrace((str))
#else
#define JPEGDEC_API_TRC(str)
#endif

#define JPEGDEC_CLEAR_IRQ  SetDecRegister(PTR_JPGC->jpegRegs, \
                                          HWIF_DEC_IRQ_STAT, 0); \
                           SetDecRegister(PTR_JPGC->jpegRegs, \
                                          HWIF_DEC_IRQ, 0);

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    5. Functions
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Function name: JpegDecInit

        Functional description:
            Init jpeg decoder

        Inputs:
            JpegDecInst * decInst     a reference to the jpeg decoder instance is
                                         stored here

        Outputs:
            JPEGDEC_OK
            JPEGDEC_INITFAIL
            JPEGDEC_PARAM_ERROR
            JPEGDEC_DWL_ERROR
            JPEGDEC_MEMFAIL

------------------------------------------------------------------------------*/
JpegDecRet JpegDecInit(JpegDecInst *pDecInst)
{
	JpegDecContainer *pJpegCont;
	const void *dwl;
	u32 i = 0;
	u32 asicID;
	u32 fuseStatus = 0;
	u32 extensionsSupported;
	u32 webpSupport;

	DWLInitParam_t dwlInit;

	JPEGDEC_API_TRC("JpegDecInit#");

	/* check that right shift on negative numbers is performed signed */
	/*lint -save -e* following check causes multiple lint messages */
	//if (((-1) >> 1) != (-1)) {
	//	JPEGDEC_API_TRC("JpegDecInit# ERROR: Right shift is not signed");
	//	return (JPEGDEC_INITFAIL);
	//}

	/*lint -restore */
	if (pDecInst == NULL) {
		JPEGDEC_API_TRC("JpegDecInit# ERROR: decInst == NULL");
		return (JPEGDEC_PARAM_ERROR);
	}
	*pDecInst = NULL;   /* return NULL instance for any error */

	/* check for proper hardware */
	asicID = DWLReadAsicID();

	{
		/* check that JPEG decoding supported in HW */
		DWLHwConfig_t hwCfg;

		DWLReadAsicConfig(&hwCfg);
		if (!hwCfg.jpegSupport) {
			JPEGDEC_API_TRC(("JpegDecInit# ERROR: JPEG not supported in HW\n"));
			return JPEGDEC_FORMAT_NOT_SUPPORTED;
		}

#ifndef VERIFY_32BIT_NANOD_IN_64BIT_ENV
		if (!hwCfg.addr64Support && sizeof(void *) == 8) {
			JPEGDEC_API_TRC("JpegDecInit# ERROR: HW not support 64bit address!\n");
			return (JPEGDEC_PARAM_ERROR);
		}
#endif
		/* check progressive support */
		if ((asicID >> 16) != 0x8170U) {
			/* progressive decoder */
			if (hwCfg.jpegSupport == JPEG_BASELINE) {
				fuseStatus = 1;
			}
		}

		extensionsSupported = hwCfg.jpegESupport;
		webpSupport = hwCfg.webpSupport;

	}

	dwlInit.clientType = DWL_CLIENT_TYPE_JPEG_DEC;

	/* Initialize Wrapper */
	dwl = DWLInit(&dwlInit);
	if (dwl == NULL) {
		JPEGDEC_API_TRC("JpegDecInit# ERROR: DWL Init failed");
		return (JPEGDEC_DWL_ERROR);
	}

	pJpegCont = (JpegDecContainer *) DWLmalloc(sizeof(JpegDecContainer));
	if (pJpegCont == NULL) {
		(void) DWLRelease(dwl);
		return (JPEGDEC_MEMFAIL);
	}

	(void)DWLmemset(pJpegCont, 0, sizeof(JpegDecContainer));

	pJpegCont->dwl = dwl;

	/* reset internal structures */
	JpegDecClearStructs(pJpegCont, 0);

	/* Reset shadow registers */
	for (i = 1; i < TOTAL_X170_REGISTERS; i++) {
		pJpegCont->jpegRegs[i] = 0;
	}

	SetCommonConfigRegs(pJpegCont->jpegRegs);

	/* save HW version so we dont need to check it all
	 * the time when deciding the control stuff */
	pJpegCont->is8190 = (asicID >> 16) != 0x8170U ? 1 : 0;
	/* set HW related config's */
	if (pJpegCont->is8190) {
		pJpegCont->fuseBurned = fuseStatus;
		/* max */
		if (webpSupport) { /* webp implicates 256Mpix support */
			pJpegCont->maxSupportedWidth = JPEGDEC_MAX_WIDTH_WEBP;
			pJpegCont->maxSupportedHeight = JPEGDEC_MAX_HEIGHT_WEBP;
			pJpegCont->maxSupportedPixelAmount = JPEGDEC_MAX_PIXEL_AMOUNT_WEBP;
			pJpegCont->maxSupportedSliceSize = JPEGDEC_MAX_SLICE_SIZE_WEBP;
		} else {
			pJpegCont->maxSupportedWidth = JPEGDEC_MAX_WIDTH_8190;
			pJpegCont->maxSupportedHeight = JPEGDEC_MAX_HEIGHT_8190;
			pJpegCont->maxSupportedPixelAmount = JPEGDEC_MAX_PIXEL_AMOUNT_8190;
			pJpegCont->maxSupportedSliceSize = JPEGDEC_MAX_SLICE_SIZE_8190;
		}
	} else {
		/* max */
		pJpegCont->maxSupportedWidth = JPEGDEC_MAX_WIDTH;
		pJpegCont->maxSupportedHeight = JPEGDEC_MAX_HEIGHT;
		pJpegCont->maxSupportedPixelAmount = JPEGDEC_MAX_PIXEL_AMOUNT;
		pJpegCont->maxSupportedSliceSize = JPEGDEC_MAX_SLICE_SIZE;
	}

	/* min */
	pJpegCont->minSupportedWidth = JPEGDEC_MIN_WIDTH;
	pJpegCont->minSupportedHeight = JPEGDEC_MIN_HEIGHT;

	pJpegCont->extensionsSupported = extensionsSupported;

	*pDecInst = (JpegDecContainer *) pJpegCont;

	JPEGDEC_API_TRC("JpegDecInit# OK\n");
	return (JPEGDEC_OK);
}

/*------------------------------------------------------------------------------

    Function name: JpegDecRelease

        Functional description:
            Release Jpeg decoder

        Inputs:
            JpegDecInst decInst    jpeg decoder instance

            void

------------------------------------------------------------------------------*/
void JpegDecRelease(JpegDecInst decInst)
{

#define PTR_JPGC ((JpegDecContainer *) decInst)

	const void *dwl;

	JPEGDEC_API_TRC("JpegDecRelease#");

	if (PTR_JPGC == NULL) {
		JPEGDEC_API_TRC("JpegDecRelease# ERROR: decInst == NULL");
		return;
	}

	dwl = PTR_JPGC->dwl;

	if (PTR_JPGC->asicRunning) {
		/* Release HW */
		DWLDisableHW(PTR_JPGC->dwl, PTR_JPGC->coreID, 4 * 1, 0);
		DWLReleaseHw(PTR_JPGC->dwl, PTR_JPGC->coreID);
	}

	if (PTR_JPGC->vlc.acTable0.vals) {
		DWLfree(PTR_JPGC->vlc.acTable0.vals);
	}
	if (PTR_JPGC->vlc.acTable1.vals) {
		DWLfree(PTR_JPGC->vlc.acTable1.vals);
	}
	if (PTR_JPGC->vlc.acTable2.vals) {
		DWLfree(PTR_JPGC->vlc.acTable2.vals);
	}
	if (PTR_JPGC->vlc.acTable3.vals) {
		DWLfree(PTR_JPGC->vlc.acTable3.vals);
	}
	if (PTR_JPGC->vlc.dcTable0.vals) {
		DWLfree(PTR_JPGC->vlc.dcTable0.vals);
	}
	if (PTR_JPGC->vlc.dcTable1.vals) {
		DWLfree(PTR_JPGC->vlc.dcTable1.vals);
	}
	if (PTR_JPGC->vlc.dcTable2.vals) {
		DWLfree(PTR_JPGC->vlc.dcTable2.vals);
	}
	if (PTR_JPGC->vlc.dcTable3.vals) {
		DWLfree(PTR_JPGC->vlc.dcTable3.vals);
	}
	if (PTR_JPGC->frame.pBuffer) {
		DWLfree(PTR_JPGC->frame.pBuffer);
	}
	/* progressive */
	if (PTR_JPGC->info.pCoeffBase.virtualAddress) {
		DWLFreeLinear(dwl, &(PTR_JPGC->info.pCoeffBase));
		PTR_JPGC->info.pCoeffBase.virtualAddress = NULL;
	}
	if (PTR_JPGC->info.tmpStrm.virtualAddress) {
		DWLFreeLinear(dwl, &(PTR_JPGC->info.tmpStrm));
		PTR_JPGC->info.tmpStrm.virtualAddress = NULL;
	}
	if (PTR_JPGC->frame.pTableBase.virtualAddress) {
		DWLFreeLinear(dwl, &(PTR_JPGC->frame.pTableBase));
		PTR_JPGC->frame.pTableBase.virtualAddress = NULL;
	}
	/* if not user allocated memories */
	if (!PTR_JPGC->info.userAllocMem) {
		if (PTR_JPGC->asicBuff.outLumaBuffer.virtualAddress != NULL) {
			DWLFreeRefFrm(dwl, &(PTR_JPGC->asicBuff.outLumaBuffer));
			PTR_JPGC->asicBuff.outLumaBuffer.virtualAddress = NULL;
		}
		if (PTR_JPGC->asicBuff.outChromaBuffer.virtualAddress != NULL) {
			DWLFreeRefFrm(dwl, &(PTR_JPGC->asicBuff.outChromaBuffer));
			PTR_JPGC->asicBuff.outChromaBuffer.virtualAddress = NULL;
		}
		if (PTR_JPGC->asicBuff.outChromaBuffer2.virtualAddress != NULL) {
			DWLFreeRefFrm(dwl, &(PTR_JPGC->asicBuff.outChromaBuffer2));
			PTR_JPGC->asicBuff.outChromaBuffer2.virtualAddress = NULL;
		}
	} else {
		PTR_JPGC->asicBuff.outLumaBuffer.virtualAddress = NULL;
		PTR_JPGC->asicBuff.outChromaBuffer.virtualAddress = NULL;
	}

	if (decInst) {
		DWLfree(PTR_JPGC);
	}
	(void) DWLRelease(dwl);

	JPEGDEC_API_TRC("JpegDecRelease# OK\n");

	return;

#undef PTR_JPGC
}

/*------------------------------------------------------------------------------

    Function name: JpegDecGetImageInfo

        Functional description:
            Get image information of the JFIF

        Inputs:
            JpegDecInst decInst     jpeg decoder instance
            JpegDecInput *pDecIn    input stream information
            JpegDecImageInfo *pImageInfo
                    structure where the image info is written

        Outputs:
            JPEGDEC_OK
            JPEGDEC_ERROR
            JPEGDEC_UNSUPPORTED
            JPEGDEC_PARAM_ERROR
            JPEGDEC_INCREASE_INPUT_BUFFER
            JPEGDEC_INVALID_STREAM_LENGTH
            JPEGDEC_INVALID_INPUT_BUFFER_SIZE

------------------------------------------------------------------------------*/

/* Get image information of the JFIF */
JpegDecRet JpegDecGetImageInfo(JpegDecInst decInst, JpegDecInput *pDecIn,
							   JpegDecImageInfo *pImageInfo)
{

#define PTR_JPGC ((JpegDecContainer *) decInst)

	u32 Nf = 0;
	u32 Ns = 0;
	u32 NsThumb = 0;
	u32 i, j = 0;
	u32 init = 0;
	u32 initThumb = 0;
	u32 H[MAX_NUMBER_OF_COMPONENTS];
	u32 V[MAX_NUMBER_OF_COMPONENTS];
	u32 Htn[MAX_NUMBER_OF_COMPONENTS];
	u32 Vtn[MAX_NUMBER_OF_COMPONENTS];
	u32 Hmax = 0;
	u32 Vmax = 0;
	u32 headerLength = 0;
	u32 currentByte = 0;
	u32 currentBytes = 0;
	u32 appLength = 0;
	u32 appBits = 0;
	u32 thumbnail = 0;
	u32 errorCode = 0;
	u32 newHeaderValue = 0;
	u32 markerByte = 0;

#ifdef JPEGDEC_ERROR_RESILIENCE
	u32 errorResilience = 0;
	u32 errorResilienceThumb = 0;
#endif /* JPEGDEC_ERROR_RESILIENCE */

	StreamStorage stream;

	JPEGDEC_API_TRC("JpegDecGetImageInfo#");

	/* check pointers & parameters */
	if (decInst == NULL || pDecIn == NULL || pImageInfo == NULL ||
		X170_CHECK_VIRTUAL_ADDRESS(pDecIn->streamBuffer.pVirtualAddress) ||
		X170_CHECK_BUS_ADDRESS(pDecIn->streamBuffer.busAddress)) {
		JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: NULL parameter");
		return (JPEGDEC_PARAM_ERROR);
	}

	/* Check the stream lenth */
	if (pDecIn->streamLength < 1) {
		JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: pDecIn->streamLength");
		return (JPEGDEC_INVALID_STREAM_LENGTH);
	}

	/* Check the stream lenth */
	if ((pDecIn->streamLength > JPEGDEC_X170_MAX_STREAM) &&
		(pDecIn->bufferSize < JPEGDEC_X170_MIN_BUFFER ||
		 pDecIn->bufferSize > JPEGDEC_X170_MAX_BUFFER)) {
		JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: pDecIn->bufferSize");
		return (JPEGDEC_INVALID_INPUT_BUFFER_SIZE);
	}

	/* Check the stream buffer size */
	if (pDecIn->bufferSize && (pDecIn->bufferSize < JPEGDEC_X170_MIN_BUFFER ||
							   pDecIn->bufferSize > JPEGDEC_X170_MAX_BUFFER)) {
		JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: pDecIn->bufferSize");
		return (JPEGDEC_INVALID_INPUT_BUFFER_SIZE);
	}

	/* Check the stream buffer size */
	if (pDecIn->bufferSize && ((pDecIn->bufferSize % 8) != 0)) {
		JPEGDEC_API_TRC
		("JpegDecGetImageInfo# ERROR: pDecIn->bufferSize % 8) != 0");
		return (JPEGDEC_INVALID_INPUT_BUFFER_SIZE);
	}

	/* reset sampling factors */
	for (i = 0; i < MAX_NUMBER_OF_COMPONENTS; i++) {
		H[i] = 0;
		V[i] = 0;
		Htn[i] = 0;
		Vtn[i] = 0;
	}

	/* imageInfo initialization */
	pImageInfo->displayWidth = 0;
	pImageInfo->displayHeight = 0;
	pImageInfo->outputWidth = 0;
	pImageInfo->outputHeight = 0;
	pImageInfo->version = 0;
	pImageInfo->units = 0;
	pImageInfo->xDensity = 0;
	pImageInfo->yDensity = 0;
	pImageInfo->outputFormat = 0;

	/* Default value to "Thumbnail" */
	pImageInfo->thumbnailType = JPEGDEC_NO_THUMBNAIL;
	pImageInfo->displayWidthThumb = 0;
	pImageInfo->displayHeightThumb = 0;
	pImageInfo->outputWidthThumb = 0;
	pImageInfo->outputHeightThumb = 0;
	pImageInfo->outputFormatThumb = 0;

	/* utils initialization */
	stream.bitPosInByte = 0;
	stream.pCurrPos = (u8 *) pDecIn->streamBuffer.pVirtualAddress;
	stream.pStartOfStream = (u8 *) pDecIn->streamBuffer.pVirtualAddress;
	stream.readBits = 0;
	stream.appnFlag = 0;

	/* stream length */
	if (!pDecIn->bufferSize) {
		stream.streamLength = pDecIn->streamLength;
	} else {
		stream.streamLength = pDecIn->bufferSize;
	}

	/* Read decoding parameters */
	for (stream.readBits = 0; (stream.readBits / 8) < stream.streamLength;) {
		/* Look for marker prefix byte from stream */
		markerByte = JpegDecGetByte(&(stream));
		if (markerByte == 0xFF) {
			currentByte = JpegDecGetByte(&(stream));
			while (currentByte == 0xFF) {
				currentByte = JpegDecGetByte(&(stream));
			}

			/* switch to certain header decoding */
			switch (currentByte) {
			/* baseline marker */
			case SOF0:
			/* progresive marker */
			case SOF2:
				if (currentByte == SOF0)
					pImageInfo->codingMode = PTR_JPGC->info.operationType =
												 JPEGDEC_BASELINE;
				else
					pImageInfo->codingMode = PTR_JPGC->info.operationType =
												 JPEGDEC_PROGRESSIVE;
				/* Frame header */
				i++;
				Hmax = 0;
				Vmax = 0;

				/* SOF0/SOF2 length */
				headerLength = JpegDecGet2Bytes(&(stream));
				if (headerLength == STRM_ERROR ||
					((stream.readBits + ((headerLength * 8) - 16)) >
					 (8 * stream.streamLength))) {
					errorCode = 1;
					break;
				}

				/* Sample precision (only 8 bits/sample supported) */
				currentByte = JpegDecGetByte(&(stream));
				if (currentByte != 8) {
					JPEGDEC_API_TRC
					("JpegDecGetImageInfo# ERROR: Sample precision");
					return (JPEGDEC_UNSUPPORTED);
				}

				/* Number of Lines */
				pImageInfo->outputHeight = JpegDecGet2Bytes(&(stream));
				pImageInfo->displayHeight = pImageInfo->outputHeight;

				if (pImageInfo->outputHeight < 1) {
					JPEGDEC_API_TRC
					("JpegDecGetImageInfo# ERROR: pImageInfo->outputHeight Unsupported");
					return (JPEGDEC_UNSUPPORTED);
				}

#ifdef JPEGDEC_ERROR_RESILIENCE
				if ((pImageInfo->outputHeight & 0xF) &&
					(pImageInfo->outputHeight & 0xF) <= 8) {
					errorResilience = 1;
				}
#endif /* JPEGDEC_ERROR_RESILIENCE */

				/* round up to next multiple-of-16 */
				pImageInfo->outputHeight += 0xf;
				pImageInfo->outputHeight &= ~(0xf);

				/* Number of Samples per Line */
				pImageInfo->outputWidth = JpegDecGet2Bytes(&(stream));
				pImageInfo->displayWidth = pImageInfo->outputWidth;
				if (pImageInfo->outputWidth < 1) {
					JPEGDEC_API_TRC
					("JpegDecGetImageInfo# ERROR: pImageInfo->outputWidth unsupported");
					return (JPEGDEC_UNSUPPORTED);
				}
				pImageInfo->outputWidth += 0xf;
				pImageInfo->outputWidth &= ~(0xf);

				/* check if height changes (MJPEG) */
				if (PTR_JPGC->frame.hwY != 0 &&
					(PTR_JPGC->frame.hwY != pImageInfo->outputHeight)) {
					JPEGDEC_API_TRC
					("JpegDecGetImageInfo# ERROR: pImageInfo->outputHeight changed (MJPEG)");
					newHeaderValue = 1;
				}

				/* check if width changes (MJPEG) */
				if (PTR_JPGC->frame.hwX != 0 &&
					(PTR_JPGC->frame.hwX != pImageInfo->outputWidth)) {
					JPEGDEC_API_TRC
					("JpegDecGetImageInfo# ERROR: pImageInfo->outputWidth changed (MJPEG)");
					newHeaderValue = 1;
				}

				/* check for minimum and maximum dimensions */
				if (pImageInfo->outputWidth < PTR_JPGC->minSupportedWidth ||
					pImageInfo->outputHeight < PTR_JPGC->minSupportedHeight ||
					pImageInfo->outputWidth > PTR_JPGC->maxSupportedWidth ||
					pImageInfo->outputHeight > PTR_JPGC->maxSupportedHeight ||
					(pImageInfo->outputWidth * pImageInfo->outputHeight) >
					PTR_JPGC->maxSupportedPixelAmount) {
					JPEGDEC_API_TRC
					("JpegDecGetImageInfo# ERROR: Unsupported size");
					return (JPEGDEC_UNSUPPORTED);
				}

				/* Number of Image Components per Frame */
				Nf = JpegDecGetByte(&(stream));
				if (Nf != 3 && Nf != 1) {
					JPEGDEC_API_TRC
					("JpegDecGetImageInfo# ERROR: Number of Image Components per Frame");
					return (JPEGDEC_UNSUPPORTED);
				}
				/* length 8 + 3 x Nf */
				if (headerLength != (8 + (3 * Nf))) {
					JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: Incorrect SOF0 header length");
					return (JPEGDEC_UNSUPPORTED);
				}
				for (j = 0; j < Nf; j++) {
					/* jump over component identifier */
					if (JpegDecFlushBits(&(stream), 8) == STRM_ERROR) {
						errorCode = 1;
						break;
					}

					/* Horizontal sampling factor */
					currentByte = JpegDecGetByte(&(stream));
					H[j] = (currentByte >> 4);

					/* Vertical sampling factor */
					V[j] = (currentByte & 0xF);

					/* jump over Tq */
					if (JpegDecFlushBits(&(stream), 8) == STRM_ERROR) {
						errorCode = 1;
						break;
					}

					if (H[j] > Hmax) {
						Hmax = H[j];
					}
					if (V[j] > Vmax) {
						Vmax = V[j];
					}
				}
				if (Hmax == 0 || Vmax == 0) {
					JPEGDEC_API_TRC
					("JpegDecGetImageInfo# ERROR: Hmax == 0 || Vmax == 0");
					return (JPEGDEC_UNSUPPORTED);
				}
#ifdef JPEGDEC_ERROR_RESILIENCE
				if (H[0] == 2 && V[0] == 2 &&
					H[1] == 1 && V[1] == 1 && H[2] == 1 && V[2] == 1) {
					pImageInfo->outputFormat = JPEGDEC_YCbCr420_SEMIPLANAR;
				} else {
					/* check if fill needed */
					if (errorResilience) {
						pImageInfo->outputHeight -= 16;
						pImageInfo->displayHeight = pImageInfo->outputHeight;
					}
				}
#endif /* JPEGDEC_ERROR_RESILIENCE */

				/* check format */
				if (H[0] == 2 && V[0] == 2 &&
					H[1] == 1 && V[1] == 1 && H[2] == 1 && V[2] == 1) {
					pImageInfo->outputFormat = JPEGDEC_YCbCr420_SEMIPLANAR;
					PTR_JPGC->frame.numMcuInRow = (pImageInfo->outputWidth / 16);
					PTR_JPGC->frame.numMcuInFrame = ((pImageInfo->outputWidth *
													  pImageInfo->outputHeight) /
													 256);
				} else if (H[0] == 2 && V[0] == 1 &&
						   H[1] == 1 && V[1] == 1 && H[2] == 1 && V[2] == 1) {
					pImageInfo->outputFormat = JPEGDEC_YCbCr422_SEMIPLANAR;
					PTR_JPGC->frame.numMcuInRow = (pImageInfo->outputWidth / 16);
					PTR_JPGC->frame.numMcuInFrame = ((pImageInfo->outputWidth *
													  pImageInfo->outputHeight) /
													 128);
				} else if (H[0] == 1 && V[0] == 2 &&
						   H[1] == 1 && V[1] == 1 && H[2] == 1 && V[2] == 1) {
					pImageInfo->outputFormat = JPEGDEC_YCbCr440;
					PTR_JPGC->frame.numMcuInRow = (pImageInfo->outputWidth / 8);
					PTR_JPGC->frame.numMcuInFrame = ((pImageInfo->outputWidth *
													  pImageInfo->outputHeight) /
													 128);
				} else if (H[0] == 1 && V[0] == 1 &&
						   H[1] == 0 && V[1] == 0 && H[2] == 0 && V[2] == 0) {
					pImageInfo->outputFormat = JPEGDEC_YCbCr400;
					PTR_JPGC->frame.numMcuInRow = (pImageInfo->outputWidth / 8);
					PTR_JPGC->frame.numMcuInFrame = ((pImageInfo->outputWidth *
													  pImageInfo->outputHeight) /
													 64);
				} else if (PTR_JPGC->extensionsSupported &&
						   H[0] == 4 && V[0] == 1 &&
						   H[1] == 1 && V[1] == 1 && H[2] == 1 && V[2] == 1) {
					/* YUV411 output has to be 32 pixel multiple */
					if (pImageInfo->outputWidth & 0x1F) {
						pImageInfo->outputWidth += 16;
					}

					/* check for maximum dimensions */
					if (pImageInfo->outputWidth > PTR_JPGC->maxSupportedWidth ||
						(pImageInfo->outputWidth * pImageInfo->outputHeight) >
						PTR_JPGC->maxSupportedPixelAmount) {
						JPEGDEC_API_TRC
						("JpegDecGetImageInfo# ERROR: Unsupported size");
						return (JPEGDEC_UNSUPPORTED);
					}

					pImageInfo->outputFormat = JPEGDEC_YCbCr411_SEMIPLANAR;
					PTR_JPGC->frame.numMcuInRow = (pImageInfo->outputWidth / 32);
					PTR_JPGC->frame.numMcuInFrame = ((pImageInfo->outputWidth *
													  pImageInfo->outputHeight) /
													 256);
				} else if (PTR_JPGC->extensionsSupported &&
						   H[0] == 1 && V[0] == 1 &&
						   H[1] == 1 && V[1] == 1 && H[2] == 1 && V[2] == 1) {
					pImageInfo->outputFormat = JPEGDEC_YCbCr444_SEMIPLANAR;
					PTR_JPGC->frame.numMcuInRow = (pImageInfo->outputWidth / 8);
					PTR_JPGC->frame.numMcuInFrame = ((pImageInfo->outputWidth *
													  pImageInfo->outputHeight) /
													 64);
				} else {
					JPEGDEC_API_TRC
					("JpegDecGetImageInfo# ERROR: Unsupported YCbCr format");
					return (JPEGDEC_UNSUPPORTED);
				}

				/* check if output format changes (MJPEG) */
				if (PTR_JPGC->info.getInfoYCbCrMode != 0 &&
					(PTR_JPGC->info.getInfoYCbCrMode != pImageInfo->outputFormat)) {
					JPEGDEC_API_TRC
					("JpegDecGetImageInfo# ERROR: YCbCr format changed (MJPEG)");
					newHeaderValue = 1;
				}
				break;
			case SOS:
				/* SOS length */
				headerLength = JpegDecGet2Bytes(&(stream));
				if (headerLength == STRM_ERROR ||
					((stream.readBits + ((headerLength * 8) - 16)) >
					 (8 * stream.streamLength))) {
					errorCode = 1;
					break;
				}

				/* check if interleaved or non-ibnterleaved */
				Ns = JpegDecGetByte(&(stream));
				if (Ns == MIN_NUMBER_OF_COMPONENTS &&
					pImageInfo->outputFormat != JPEGDEC_YCbCr400 &&
					pImageInfo->codingMode == JPEGDEC_BASELINE) {
					pImageInfo->codingMode = PTR_JPGC->info.operationType =
												 JPEGDEC_NONINTERLEAVED;
				}
				/* Number of Image Components */
				if (Ns != 3 && Ns != 1) {
					JPEGDEC_API_TRC
					("JpegDecGetImageInfo# ERROR: Number of Image Components");
					return (JPEGDEC_UNSUPPORTED);
				}
				/* length 6 + 2 x Ns */
				if (headerLength != (6 + (2 * Ns))) {
					JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: Incorrect SOS header length");
					return (JPEGDEC_UNSUPPORTED);
				}
				/* jump over SOS header */
				if (headerLength != 0) {
					stream.readBits += ((headerLength * 8) - 16);
					stream.pCurrPos += (((headerLength * 8) - 16) / 8);
				}

				if ((stream.readBits + 8) < (8 * stream.streamLength)) {
					PTR_JPGC->info.init = 1;
					init = 1;
				} else {
					JPEGDEC_API_TRC
					("JpegDecGetImageInfo# ERROR: Needs to increase input buffer");
					return (JPEGDEC_INCREASE_INPUT_BUFFER);
				}
				break;
			case DQT:
				/* DQT length */
				headerLength = JpegDecGet2Bytes(&(stream));
				if (headerLength == STRM_ERROR ||
					((stream.readBits + ((headerLength * 8) - 16)) >
					 (8 * stream.streamLength))) {
					errorCode = 1;
					break;
				}
				/* length >= (2 + 65) (baseline) */
				if (headerLength < (2 + 65)) {
					JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: Incorrect DQT header length");
					return (JPEGDEC_UNSUPPORTED);
				}
				/* jump over DQT header */
				if (headerLength != 0) {
					stream.readBits += ((headerLength * 8) - 16);
					stream.pCurrPos += (((headerLength * 8) - 16) / 8);
				}
				break;
			case DHT:
				/* DHT length */
				headerLength = JpegDecGet2Bytes(&(stream));
				if (headerLength == STRM_ERROR ||
					((stream.readBits + ((headerLength * 8) - 16)) >
					 (8 * stream.streamLength))) {
					errorCode = 1;
					break;
				}
				/* length >= 2 + 17 */
				if (headerLength < 19) {
					JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: Incorrect DHT header length");
					return (JPEGDEC_UNSUPPORTED);
				}
				/* jump over DHT header */
				if (headerLength != 0) {
					stream.readBits += ((headerLength * 8) - 16);
					stream.pCurrPos += (((headerLength * 8) - 16) / 8);
				}
				break;
			case DRI:
				/* DRI length */
				headerLength = JpegDecGet2Bytes(&(stream));
				if (headerLength == STRM_ERROR ||
					((stream.readBits + ((headerLength * 8) - 16)) >
					 (8 * stream.streamLength))) {
					errorCode = 1;
					break;
				}
				/* length == 4 */
				if (headerLength != 4) {
					JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: Incorrect DRI header length");
					return (JPEGDEC_UNSUPPORTED);
				}
#if 0
				/* jump over DRI header */
				if (headerLength != 0) {
					stream.readBits += ((headerLength * 8) - 16);
					stream.pCurrPos += (((headerLength * 8) - 16) / 8);
				}
#endif
				headerLength = JpegDecGet2Bytes(&(stream));
				if (headerLength == STRM_ERROR ||
					((stream.readBits + ((headerLength * 8) - 16)) >
					 (8 * stream.streamLength))) {
					errorCode = 1;
					break;
				}
				PTR_JPGC->frame.Ri = headerLength;
				break;
			/* application segments */
			case APP0:
				JPEGDEC_API_TRC("JpegDecGetImageInfo# APP0 in GetImageInfo");
				/* reset */
				appBits = 0;
				appLength = 0;
				stream.appnFlag = 0;

				/* APP0 length */
				headerLength = JpegDecGet2Bytes(&(stream));
				if (headerLength == STRM_ERROR ||
					((stream.readBits + ((headerLength * 8) - 16)) >
					 (8 * stream.streamLength))) {
					errorCode = 1;
					break;
				}
				/* length > 2 */
				if (headerLength < 2) {
					break;
				}
				appLength = headerLength;
				if (appLength < 16) {
					stream.appnFlag = 1;
					if (JpegDecFlushBits(&(stream), ((appLength * 8) - 16)) ==
						STRM_ERROR) {
						errorCode = 1;
						break;
					}
					break;
				}
				appBits += 16;

				/* check identifier */
				currentBytes = JpegDecGet2Bytes(&(stream));
				appBits += 16;
				if (currentBytes != 0x4A46) {
					stream.appnFlag = 1;
					if (JpegDecFlushBits(&(stream), ((appLength * 8) - appBits))
						== STRM_ERROR) {
						errorCode = 1;
						break;
					}
					break;
				}
				currentBytes = JpegDecGet2Bytes(&(stream));
				appBits += 16;
				if (currentBytes != 0x4946 && currentBytes != 0x5858) {
					stream.appnFlag = 1;
					if (JpegDecFlushBits(&(stream), ((appLength * 8) - appBits))
						== STRM_ERROR) {
						errorCode = 1;
						break;
					}
					break;
				}

				/* APP0 Extended */
				if (currentBytes == 0x5858) {
					thumbnail = 1;
				}
				currentByte = JpegDecGetByte(&(stream));
				appBits += 8;
				if (currentByte != 0x00) {
					stream.appnFlag = 1;
					if (JpegDecFlushBits(&(stream), ((appLength * 8) - appBits))
						== STRM_ERROR) {
						errorCode = 1;
						break;
					}
					stream.appnFlag = 0;
					break;
				}

				/* APP0 Extended thumb type */
				if (thumbnail) {
					/* extension code */
					currentByte = JpegDecGetByte(&(stream));
					if (currentByte == JPEGDEC_THUMBNAIL_JPEG) {
						pImageInfo->thumbnailType = JPEGDEC_THUMBNAIL_JPEG;
						appBits += 8;
						stream.appnFlag = 1;

						/* check thumbnail data */
						Hmax = 0;
						Vmax = 0;

						/* Read decoding parameters */
						for (; (stream.readBits / 8) < stream.streamLength;) {
							/* Look for marker prefix byte from stream */
							appBits += 8;
							markerByte = JpegDecGetByte(&(stream));
							/* check if APP0 decoded */
							if (((appBits + 8) / 8) >= appLength) {
								break;
							}
							if (markerByte == 0xFF) {
								/* switch to certain header decoding */
								appBits += 8;

								currentByte = JpegDecGetByte(&(stream));
								while (currentByte == 0xFF) {
									appBits += 8;
									currentByte = JpegDecGetByte(&(stream));
								}
								switch (currentByte) {
								/* baseline marker */
								case SOF0:
								/* progresive marker */
								case SOF2:
									if (currentByte == SOF0)
										pImageInfo->codingModeThumb =
											PTR_JPGC->info.operationTypeThumb =
												JPEGDEC_BASELINE;
									else
										pImageInfo->codingModeThumb =
											PTR_JPGC->info.operationTypeThumb =
												JPEGDEC_PROGRESSIVE;
									/* Frame header */
									i++;

									/* jump over Lf field */
									headerLength = JpegDecGet2Bytes(&(stream));
									if (headerLength == STRM_ERROR ||
										((stream.readBits + ((headerLength * 8) - 16)) >
										 (8 * stream.streamLength))) {
										errorCode = 1;
										break;
									}
									appBits += 16;

									/* Sample precision (only 8 bits/sample supported) */
									currentByte = JpegDecGetByte(&(stream));
									appBits += 8;
									if (currentByte != 8) {
										JPEGDEC_API_TRC
										("JpegDecGetImageInfo# ERROR: Thumbnail Sample precision");
										pImageInfo->thumbnailType = JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT;
									}

									/* Number of Lines */
									pImageInfo->outputHeightThumb =
										JpegDecGet2Bytes(&(stream));
									appBits += 16;
									pImageInfo->displayHeightThumb =
										pImageInfo->outputHeightThumb;
									if (pImageInfo->outputHeightThumb < 1) {
										JPEGDEC_API_TRC
										("JpegDecGetImageInfo# ERROR: pImageInfo->outputHeightThumb unsupported");
										pImageInfo->thumbnailType = JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT;
									}
#ifdef JPEGDEC_ERROR_RESILIENCE
									if ((pImageInfo->outputHeightThumb & 0xF) &&
										(pImageInfo->outputHeightThumb & 0xF) <=
										8) {
										errorResilienceThumb = 1;
									}
#endif /* JPEGDEC_ERROR_RESILIENCE */

									/* round up to next multiple-of-16 */
									pImageInfo->outputHeightThumb += 0xf;
									pImageInfo->outputHeightThumb &= ~(0xf);

									/* Number of Samples per Line */
									pImageInfo->outputWidthThumb =
										JpegDecGet2Bytes(&(stream));
									appBits += 16;
									pImageInfo->displayWidthThumb =
										pImageInfo->outputWidthThumb;
									if (pImageInfo->outputWidthThumb < 1) {
										JPEGDEC_API_TRC
										("JpegDecGetImageInfo# ERROR: pImageInfo->outputWidthThumb unsupported");
										pImageInfo->thumbnailType = JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT;
									}
									pImageInfo->outputWidthThumb += 0xf;
									pImageInfo->outputWidthThumb &= ~(0xf);

									/* check if height changes (MJPEG) */
									if (PTR_JPGC->frame.hwYTn != 0 &&
										(PTR_JPGC->frame.hwYTn != pImageInfo->outputHeightThumb)) {
										JPEGDEC_API_TRC
										("JpegDecGetImageInfo# ERROR: pImageInfo->outputHeightThumb changed (MJPEG)");
										newHeaderValue = 1;
									}

									/* check if width changes (MJPEG) */
									if (PTR_JPGC->frame.hwXTn != 0 &&
										(PTR_JPGC->frame.hwXTn != pImageInfo->outputWidthThumb)) {
										JPEGDEC_API_TRC
										("JpegDecGetImageInfo# ERROR: pImageInfo->outputWidthThumb changed (MJPEG)");
										newHeaderValue = 1;
									}

									if (pImageInfo->outputWidthThumb <
										PTR_JPGC->minSupportedWidth ||
										pImageInfo->outputHeightThumb <
										PTR_JPGC->minSupportedHeight ||
										pImageInfo->outputWidthThumb >
										JPEGDEC_MAX_WIDTH_TN ||
										pImageInfo->outputHeightThumb >
										JPEGDEC_MAX_HEIGHT_TN) {

										JPEGDEC_API_TRC
										("JpegDecGetImageInfo# ERROR: Thumbnail Unsupported size");
										pImageInfo->thumbnailType = JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT;
									}

									/* Number of Image Components per Frame */
									Nf = JpegDecGetByte(&(stream));
									appBits += 8;
									if (Nf != 3 && Nf != 1) {
										JPEGDEC_API_TRC
										("JpegDecGetImageInfo# ERROR: Thumbnail Number of Image Components per Frame");
										pImageInfo->thumbnailType = JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT;
									}
									/* length 8 + 3 x Nf */
									if (headerLength != (8 + (3 * Nf))) {
										JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: Thumbnail Incorrect SOF0 header length");
										pImageInfo->thumbnailType = JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT;
									}
									for (j = 0; j < Nf; j++) {

										/* jump over component identifier */
										if (JpegDecFlushBits(&(stream), 8) ==
											STRM_ERROR) {
											errorCode = 1;
											break;
										}
										appBits += 8;

										/* Horizontal sampling factor */
										currentByte = JpegDecGetByte(&(stream));
										appBits += 8;
										Htn[j] = (currentByte >> 4);

										/* Vertical sampling factor */
										Vtn[j] = (currentByte & 0xF);

										/* jump over Tq */
										if (JpegDecFlushBits(&(stream), 8) ==
											STRM_ERROR) {
											errorCode = 1;
											break;
										}
										appBits += 8;

										if (Htn[j] > Hmax) {
											Hmax = Htn[j];
										}
										if (Vtn[j] > Vmax) {
											Vmax = Vtn[j];
										}
									}
									if (Hmax == 0 || Vmax == 0) {
										JPEGDEC_API_TRC
										("JpegDecGetImageInfo# ERROR: Thumbnail Hmax == 0 || Vmax == 0");
										pImageInfo->thumbnailType = JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT;
									}
#ifdef JPEGDEC_ERROR_RESILIENCE
									if (Htn[0] == 2 && Vtn[0] == 2 &&
										Htn[1] == 1 && Vtn[1] == 1 &&
										Htn[2] == 1 && Vtn[2] == 1) {
										pImageInfo->outputFormatThumb =
											JPEGDEC_YCbCr420_SEMIPLANAR;
									} else {
										/* check if fill needed */
										if (errorResilienceThumb) {
											pImageInfo->outputHeightThumb -= 16;
											pImageInfo->displayHeightThumb =
												pImageInfo->outputHeightThumb;
										}
									}
#endif /* JPEGDEC_ERROR_RESILIENCE */

									/* check format */
									if (Htn[0] == 2 && Vtn[0] == 2 &&
										Htn[1] == 1 && Vtn[1] == 1 &&
										Htn[2] == 1 && Vtn[2] == 1) {
										pImageInfo->outputFormatThumb =
											JPEGDEC_YCbCr420_SEMIPLANAR;
									} else if (Htn[0] == 2 && Vtn[0] == 1 &&
											   Htn[1] == 1 && Vtn[1] == 1 &&
											   Htn[2] == 1 && Vtn[2] == 1) {
										pImageInfo->outputFormatThumb =
											JPEGDEC_YCbCr422_SEMIPLANAR;
									} else if (Htn[0] == 1 && Vtn[0] == 2 &&
											   Htn[1] == 1 && Vtn[1] == 1 &&
											   Htn[2] == 1 && Vtn[2] == 1) {
										pImageInfo->outputFormatThumb =
											JPEGDEC_YCbCr440;
									} else if (Htn[0] == 1 && Vtn[0] == 1 &&
											   Htn[1] == 0 && Vtn[1] == 0 &&
											   Htn[2] == 0 && Vtn[2] == 0) {
										pImageInfo->outputFormatThumb =
											JPEGDEC_YCbCr400;
									} else if (PTR_JPGC->is8190 &&
											   Htn[0] == 4 && Vtn[0] == 1 &&
											   Htn[1] == 1 && Vtn[1] == 1 &&
											   Htn[2] == 1 && Vtn[2] == 1) {
										pImageInfo->outputFormatThumb =
											JPEGDEC_YCbCr411_SEMIPLANAR;
									} else if (PTR_JPGC->is8190 &&
											   Htn[0] == 1 && Vtn[0] == 1 &&
											   Htn[1] == 1 && Vtn[1] == 1 &&
											   Htn[2] == 1 && Vtn[2] == 1) {
										pImageInfo->outputFormatThumb =
											JPEGDEC_YCbCr444_SEMIPLANAR;
									} else {
										JPEGDEC_API_TRC
										("JpegDecGetImageInfo# ERROR: Thumbnail Unsupported YCbCr format");
										pImageInfo->thumbnailType = JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT;
									}

									/* check if output format changes (MJPEG) */
									if (PTR_JPGC->info.getInfoYCbCrModeTn != 0 &&
										(PTR_JPGC->info.getInfoYCbCrModeTn != pImageInfo->outputFormatThumb)) {
										JPEGDEC_API_TRC
										("JpegDecGetImageInfo# ERROR: Thumbnail YCbCr format changed (MJPEG)");
										newHeaderValue = 1;
									}
									break;
								case SOS:
									/* SOS length */
									headerLength = JpegDecGet2Bytes(&(stream));
									if (headerLength == STRM_ERROR ||
										((stream.readBits +
										  ((headerLength * 8) - 16)) >
										 (8 * stream.streamLength))) {
										errorCode = 1;
										break;
									}

									/* check if interleaved or non-ibnterleaved */
									NsThumb = JpegDecGetByte(&(stream));
									if (NsThumb == MIN_NUMBER_OF_COMPONENTS &&
										pImageInfo->outputFormatThumb !=
										JPEGDEC_YCbCr400 &&
										pImageInfo->codingModeThumb ==
										JPEGDEC_BASELINE) {
										pImageInfo->codingModeThumb =
											PTR_JPGC->info.operationTypeThumb =
												JPEGDEC_NONINTERLEAVED;
									}
									if (NsThumb != 3 && NsThumb != 1) {
										JPEGDEC_API_TRC
										("JpegDecGetImageInfo# ERROR: Thumbnail Number of Image Components");
										pImageInfo->thumbnailType = JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT;
									}
									/* length 6 + 2 x NsThumb */
									if (headerLength != (6 + (2 * NsThumb))) {
										JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: Thumbnail Incorrect SOS header length");
										pImageInfo->thumbnailType = JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT;
									}
									/* jump over SOS header */
									if (headerLength != 0) {
										stream.readBits +=
											((headerLength * 8) - 16);
										stream.pCurrPos +=
											(((headerLength * 8) - 16) / 8);
									}

									if ((stream.readBits + 8) <
										(8 * stream.streamLength)) {
										PTR_JPGC->info.initThumb = 1;
										initThumb = 1;
									} else {
										JPEGDEC_API_TRC
										("JpegDecGetImageInfo# ERROR: Needs to increase input buffer");
										return (JPEGDEC_INCREASE_INPUT_BUFFER);
									}
									break;
								case DQT:
									/* DQT length */
									headerLength = JpegDecGet2Bytes(&(stream));
									if (headerLength == STRM_ERROR) {
										errorCode = 1;
										break;
									}
									/* length >= (2 + 65) (baseline) */
									if (headerLength < (2 + 65)) {
										JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: Thumbnail Incorrect DQT header length");
										pImageInfo->thumbnailType = JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT;
										break;
									}
									/* jump over DQT header */
									if (headerLength != 0) {
										stream.readBits +=
											((headerLength * 8) - 16);
										stream.pCurrPos +=
											(((headerLength * 8) - 16) / 8);
									}
									appBits += (headerLength * 8);
									break;
								case DHT:
									/* DHT length */
									headerLength = JpegDecGet2Bytes(&(stream));
									if (headerLength == STRM_ERROR) {
										errorCode = 1;
										break;
									}
									/* length >= 2 + 17 */
									if (headerLength < 19) {
										JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: Thumbnail Incorrect DHT header length");
										pImageInfo->thumbnailType = JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT;
										break;
									}
									/* jump over DHT header */
									if (headerLength != 0) {
										stream.readBits +=
											((headerLength * 8) - 16);
										stream.pCurrPos +=
											(((headerLength * 8) - 16) / 8);
									}
									appBits += (headerLength * 8);
									break;
								case DRI:
									/* DRI length */
									headerLength = JpegDecGet2Bytes(&(stream));
									if (headerLength == STRM_ERROR) {
										errorCode = 1;
										break;
									}
									/* length == 4 */
									if (headerLength != 4) {
										JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: Thumbnail Incorrect DRI header length");
										pImageInfo->thumbnailType = JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT;
										break;
									}
									/* jump over DRI header */
									if (headerLength != 0) {
										stream.readBits +=
											((headerLength * 8) - 16);
										stream.pCurrPos +=
											(((headerLength * 8) - 16) / 8);
									}
									appBits += (headerLength * 8);
									break;
								case APP0:
								case APP1:
								case APP2:
								case APP3:
								case APP4:
								case APP5:
								case APP6:
								case APP7:
								case APP8:
								case APP9:
								case APP10:
								case APP11:
								case APP12:
								case APP13:
								case APP14:
								case APP15:
									/* APPn length */
									headerLength = JpegDecGet2Bytes(&(stream));
									if (headerLength == STRM_ERROR) {
										errorCode = 1;
										break;
									}
									/* headerLength > 2 */
									if (headerLength < 2) {
										break;
									}
									/* jump over APPn header */
									if (headerLength != 0) {
										stream.readBits +=
											((headerLength * 8) - 16);
										stream.pCurrPos +=
											(((headerLength * 8) - 16) / 8);
									}
									appBits += (headerLength * 8);
									break;
								case DNL:
									/* DNL length */
									headerLength = JpegDecGet2Bytes(&(stream));
									if (headerLength == STRM_ERROR) {
										errorCode = 1;
										break;
									}
									/* length == 4 */
									if (headerLength != 4) {
										break;
									}
									/* jump over DNL header */
									if (headerLength != 0) {
										stream.readBits +=
											((headerLength * 8) - 16);
										stream.pCurrPos +=
											(((headerLength * 8) - 16) / 8);
									}
									appBits += (headerLength * 8);
									break;
								case COM:
									/* COM length */
									headerLength = JpegDecGet2Bytes(&(stream));
									if (headerLength == STRM_ERROR) {
										errorCode = 1;
										break;
									}
									/* length > 2 */
									if (headerLength < 2) {
										break;
									}
									/* jump over COM header */
									if (headerLength != 0) {
										stream.readBits +=
											((headerLength * 8) - 16);
										stream.pCurrPos +=
											(((headerLength * 8) - 16) / 8);
									}
									appBits += (headerLength * 8);
									break;
								/* unsupported coding styles */
								case SOF1:
								case SOF3:
								case SOF5:
								case SOF6:
								case SOF7:
								case SOF9:
								case SOF10:
								case SOF11:
								case SOF13:
								case SOF14:
								case SOF15:
								case DAC:
								case DHP:
									JPEGDEC_API_TRC
									("JpegDecGetImageInfo# ERROR: Unsupported coding styles");
									return (JPEGDEC_UNSUPPORTED);
								default:
									break;
								}
								if (PTR_JPGC->info.initThumb && initThumb) {
									/* flush the rest of thumbnail data */
									if (JpegDecFlushBits
										(&(stream),
										 ((appLength * 8) - appBits)) ==
										STRM_ERROR) {
										errorCode = 1;
										break;
									}
									stream.appnFlag = 0;
									break;
								}
							} else {
								if (!PTR_JPGC->info.initThumb &&
									((stream.readBits + 8) >= (stream.streamLength * 8)) &&
									pDecIn->bufferSize) {
									return (JPEGDEC_INCREASE_INPUT_BUFFER);
								}

								if (markerByte == STRM_ERROR) {
									return (JPEGDEC_STRM_ERROR);
								}
							}
						}
						if (!PTR_JPGC->info.initThumb && !initThumb) {
							JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: Thumbnail contains no data");
							pImageInfo->thumbnailType = JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT;
						}
						break;
					} else {
						appBits += 8;
						pImageInfo->thumbnailType =
							JPEGDEC_THUMBNAIL_NOT_SUPPORTED_FORMAT;
						stream.appnFlag = 1;
						if (JpegDecFlushBits
							(&(stream),
							 ((appLength * 8) - appBits)) == STRM_ERROR) {
							errorCode = 1;
							break;
						}
						stream.appnFlag = 0;
						break;
					}
				} else {
					/* version */
					pImageInfo->version = JpegDecGet2Bytes(&(stream));
					appBits += 16;

					/* units */
					currentByte = JpegDecGetByte(&(stream));
					if (currentByte == 0) {
						pImageInfo->units = JPEGDEC_NO_UNITS;
					} else if (currentByte == 1) {
						pImageInfo->units = JPEGDEC_DOTS_PER_INCH;
					} else if (currentByte == 2) {
						pImageInfo->units = JPEGDEC_DOTS_PER_CM;
					}
					appBits += 8;

					/* Xdensity */
					pImageInfo->xDensity = JpegDecGet2Bytes(&(stream));
					appBits += 16;

					/* Ydensity */
					pImageInfo->yDensity = JpegDecGet2Bytes(&(stream));
					appBits += 16;

					/* jump over rest of header data */
					stream.appnFlag = 1;
					if (JpegDecFlushBits(&(stream), ((appLength * 8) - appBits))
						== STRM_ERROR) {
						errorCode = 1;
						break;
					}
					stream.appnFlag = 0;
					break;
				}
			case APP1:
			case APP2:
			case APP3:
			case APP4:
			case APP5:
			case APP6:
			case APP7:
			case APP8:
			case APP9:
			case APP10:
			case APP11:
			case APP12:
			case APP13:
			case APP14:
			case APP15:
				/* APPn length */
				headerLength = JpegDecGet2Bytes(&(stream));
				if (headerLength == STRM_ERROR ||
					((stream.readBits + ((headerLength * 8) - 16)) >
					 (8 * stream.streamLength))) {
					errorCode = 1;
					break;
				}
				/* length > 2 */
				if (headerLength < 2) {
					break;
				}
				/* jump over APPn header */
				if (headerLength != 0) {
					stream.readBits += ((headerLength * 8) - 16);
					stream.pCurrPos += (((headerLength * 8) - 16) / 8);
				}
				break;
			case DNL:
				/* DNL length */
				headerLength = JpegDecGet2Bytes(&(stream));
				if (headerLength == STRM_ERROR ||
					((stream.readBits + ((headerLength * 8) - 16)) >
					 (8 * stream.streamLength))) {
					errorCode = 1;
					break;
				}
				/* length == 4 */
				if (headerLength != 4) {
					break;
				}
				/* jump over DNL header */
				if (headerLength != 0) {
					stream.readBits += ((headerLength * 8) - 16);
					stream.pCurrPos += (((headerLength * 8) - 16) / 8);
				}
				break;
			case COM:
				headerLength = JpegDecGet2Bytes(&(stream));
				if (headerLength == STRM_ERROR ||
					((stream.readBits + ((headerLength * 8) - 16)) >
					 (8 * stream.streamLength))) {
					errorCode = 1;
					break;
				}
				/* length > 2 */
				if (headerLength < 2) {
					break;
				}
				/* jump over COM header */
				if (headerLength != 0) {
					stream.readBits += ((headerLength * 8) - 16);
					stream.pCurrPos += (((headerLength * 8) - 16) / 8);
				}
				break;
			/* unsupported coding styles */
			case SOF1:
			case SOF3:
			case SOF5:
			case SOF6:
			case SOF7:
			case SOF9:
			case SOF10:
			case SOF11:
			case SOF13:
			case SOF14:
			case SOF15:
			case DAC:
			case DHP:
				JPEGDEC_API_TRC
				("JpegDecGetImageInfo# ERROR: Unsupported coding styles");
				return (JPEGDEC_UNSUPPORTED);
			default:
				break;
			}
			if (PTR_JPGC->info.init && init) {
				if (1/*!newHeaderValue*/) {
					PTR_JPGC->frame.hwY = PTR_JPGC->frame.fullY = pImageInfo->outputHeight;
					PTR_JPGC->frame.hwX = PTR_JPGC->frame.fullX = pImageInfo->outputWidth;
					/* restore output format */
					PTR_JPGC->info.yCbCrMode = PTR_JPGC->info.getInfoYCbCrMode =
												   pImageInfo->outputFormat;
					if (thumbnail) {
						PTR_JPGC->frame.hwYTn = pImageInfo->outputHeightThumb;
						PTR_JPGC->frame.hwXTn = pImageInfo->outputWidthThumb;
						/* restore output format for thumb */
						PTR_JPGC->info.getInfoYCbCrModeTn = pImageInfo->outputFormatThumb;
					}
					break;
				} else {
					return (JPEGDEC_UNSUPPORTED);
				}
			}

			if (errorCode) {
				if (pDecIn->bufferSize) {
					/* reset to ensure that big enough buffer will be allocated for decoding */
					if (newHeaderValue) {
						pImageInfo->outputHeight = PTR_JPGC->frame.hwY;
						pImageInfo->outputWidth = PTR_JPGC->frame.hwX;
						pImageInfo->outputFormat = PTR_JPGC->info.getInfoYCbCrMode;
						if (thumbnail) {
							pImageInfo->outputHeightThumb = PTR_JPGC->frame.hwYTn;
							pImageInfo->outputWidthThumb = PTR_JPGC->frame.hwXTn;
							pImageInfo->outputFormatThumb = PTR_JPGC->info.getInfoYCbCrModeTn;
						}
					}

					JPEGDEC_API_TRC
					("JpegDecGetImageInfo# ERROR: Image info failed, Needs to increase input buffer");
					return (JPEGDEC_INCREASE_INPUT_BUFFER);
				} else {
					JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR: Stream error");
					return (JPEGDEC_STRM_ERROR);
				}
			}
		} else {
			if (!PTR_JPGC->info.init && (stream.readBits + 8 >= (stream.streamLength * 8))) {
				return (JPEGDEC_INCREASE_INPUT_BUFFER);
			}

			if (markerByte == STRM_ERROR) {
				return (JPEGDEC_STRM_ERROR);
			}
		}
	}
	if (PTR_JPGC->info.init) {
		if (pDecIn->bufferSize) {
			PTR_JPGC->info.initBufferSize = pDecIn->bufferSize;
		}

		JPEGDEC_API_TRC("JpegDecGetImageInfo# OK\n");
		return (JPEGDEC_OK);
	} else {
		JPEGDEC_API_TRC("JpegDecGetImageInfo# ERROR\n");
		return (JPEGDEC_ERROR);
	}

#undef PTR_JPGC
}

/*------------------------------------------------------------------------------

    Function name: JpegDecDecode

        Functional description:
            Decode JFIF

        Inputs:
            JpegDecInst decInst     jpeg decoder instance
            JpegDecInput *pDecIn    pointer to structure where the decoder
                                    stores input information
            JpegDecOutput *pDecOut  pointer to structure where the decoder
                                    stores output frame information

        Outputs:
            JPEGDEC_FRAME_READY
            JPEGDEC_PARAM_ERROR
            JPEGDEC_INVALID_STREAM_LENGTH
            JPEGDEC_INVALID_INPUT_BUFFER_SIZE
            JPEGDEC_UNSUPPORTED
            JPEGDEC_ERROR
            JPEGDEC_STRM_ERROR
            JPEGDEC_HW_BUS_ERROR
            JPEGDEC_DWL_HW_TIMEOUT
            JPEGDEC_SYSTEM_ERROR
            JPEGDEC_HW_RESERVED
            JPEGDEC_STRM_PROCESSED

  ------------------------------------------------------------------------------*/
JpegDecRet JpegDecDecode(JpegDecInst decInst, JpegDecInput *pDecIn,
						 JpegDecOutput *pDecOut)
{

#define PTR_JPGC ((JpegDecContainer *) decInst)
#define JPG_FRM  PTR_JPGC->frame
#define JPG_INFO  PTR_JPGC->info

	StackStatInit();

	i32 dwlret = -1;
	u32 i = 0;
	u32 currentByte = 0;
	u32 currentBytes = 0;
	u32 appLength = 0;
	u32 appBits = 0;
	u32 asic_status = 0;
	u32 HINTdec = 0;
	u32 asicSliceBit = 0;
	u32 intDec = 0;
	g1_addr_t currentPos = 0;
	u32 endOfImage = 0;
	u32 nonInterleavedRdy = 0;
	JpegDecRet infoRet;
	JpegDecRet retCode; /* Returned code container */
	JpegDecImageInfo infoTmp;
	u32 mcuSizeDivider = 0;
	JPEGDEC_API_TRC("JpegDecDecode#");

	/* check null */
	if (decInst == NULL || pDecIn == NULL ||
		X170_CHECK_VIRTUAL_ADDRESS(pDecIn->streamBuffer.pVirtualAddress) ||
		X170_CHECK_BUS_ADDRESS(pDecIn->streamBuffer.busAddress) ||
		pDecOut == NULL) {
		JPEGDEC_API_TRC("JpegDecDecode# ERROR: NULL parameter");
		return (JPEGDEC_PARAM_ERROR);
	}

	/* check image decoding type */
	if (pDecIn->decImageType != 0 && pDecIn->decImageType != 1) {
		JPEGDEC_API_TRC("JpegDecDecode# ERROR: decImageType");
		return (JPEGDEC_PARAM_ERROR);
	}

	/* check user allocated null */
	if ((pDecIn->pictureBufferY.pVirtualAddress == NULL &&
		 pDecIn->pictureBufferY.busAddress != 0) ||
		(pDecIn->pictureBufferY.pVirtualAddress != NULL &&
		 pDecIn->pictureBufferY.busAddress == 0) ||
		(pDecIn->pictureBufferCbCr.pVirtualAddress == NULL &&
		 pDecIn->pictureBufferCbCr.busAddress != 0) ||
		(pDecIn->pictureBufferCbCr.pVirtualAddress != NULL &&
		 pDecIn->pictureBufferCbCr.busAddress == 0)) {
		JPEGDEC_API_TRC("JpegDecDecode# ERROR: NULL parameter");
		return (JPEGDEC_PARAM_ERROR);
	}

	/* Check the stream lenth */
	if (pDecIn->streamLength < 1) {
		JPEGDEC_API_TRC("JpegDecDecode# ERROR: pDecIn->streamLength");
		return (JPEGDEC_INVALID_STREAM_LENGTH);
	}

	/* check the input buffer settings ==>
	 * checks are discarded for last buffer */
	if (!PTR_JPGC->info.streamEndFlag) {
		/* Check the stream lenth */
		if (!PTR_JPGC->info.inputBufferEmpty &&
			(pDecIn->streamLength > JPEGDEC_X170_MAX_STREAM) &&
			(pDecIn->bufferSize < JPEGDEC_X170_MIN_BUFFER ||
			 pDecIn->bufferSize > JPEGDEC_X170_MAX_BUFFER)) {
			JPEGDEC_API_TRC("JpegDecDecode# ERROR: pDecIn->bufferSize");
			return (JPEGDEC_INVALID_INPUT_BUFFER_SIZE);
		}

		/* Check the stream buffer size */
		if (!PTR_JPGC->info.inputBufferEmpty &&
			pDecIn->bufferSize && (pDecIn->bufferSize < JPEGDEC_X170_MIN_BUFFER
								   || pDecIn->bufferSize >
								   JPEGDEC_X170_MAX_BUFFER)) {
			JPEGDEC_API_TRC("JpegDecDecode# ERROR: pDecIn->bufferSize");
			return (JPEGDEC_INVALID_INPUT_BUFFER_SIZE);
		}

		/* Check the stream buffer size */
		if (!PTR_JPGC->info.inputBufferEmpty &&
			pDecIn->bufferSize && ((pDecIn->bufferSize % 256) != 0)) {
			JPEGDEC_API_TRC
			("JpegDecDecode# ERROR: pDecIn->bufferSize % 256) != 0");
			return (JPEGDEC_INVALID_INPUT_BUFFER_SIZE);
		}

		if (!PTR_JPGC->info.inputBufferEmpty &&
			PTR_JPGC->info.init &&
			(pDecIn->bufferSize < PTR_JPGC->info.initBufferSize)) {
			JPEGDEC_API_TRC
			("JpegDecDecode# ERROR: Increase input buffer size!\n");
			return (JPEGDEC_INVALID_INPUT_BUFFER_SIZE);
		}
	}

	if (!PTR_JPGC->info.init && !PTR_JPGC->info.SliceReadyForPause &&
		!PTR_JPGC->info.inputBufferEmpty) {
		JPEGDEC_API_TRC(("JpegDecDecode#: Get Image Info!\n"));

		infoRet = JpegDecGetImageInfo(decInst, pDecIn, &infoTmp);

		if (infoRet != JPEGDEC_OK) {
			JPEGDEC_API_TRC(("JpegDecDecode# ERROR: Image info failed!"));
			return (infoRet);
		}
	}

	/* Store the stream parameters */
	if (PTR_JPGC->info.progressiveScanReady == 0 &&
		PTR_JPGC->info.nonInterleavedScanReady == 0) {
		PTR_JPGC->stream.bitPosInByte = 0;
		PTR_JPGC->stream.pCurrPos = (u8 *) pDecIn->streamBuffer.pVirtualAddress;
		PTR_JPGC->stream.streamBus = pDecIn->streamBuffer.busAddress;
		PTR_JPGC->stream.pStartOfStream =
			(u8 *) pDecIn->streamBuffer.pVirtualAddress;
		PTR_JPGC->stream.readBits = 0;
		PTR_JPGC->stream.streamLength = pDecIn->streamLength;
		PTR_JPGC->stream.appnFlag = 0;
		pDecOut->outputPictureY.pVirtualAddress = NULL;
		pDecOut->outputPictureY.busAddress = 0;
		pDecOut->outputPictureCbCr.pVirtualAddress = NULL;
		pDecOut->outputPictureCbCr.busAddress = 0;
		pDecOut->outputPictureCr.pVirtualAddress = NULL;
		pDecOut->outputPictureCr.busAddress = 0;
	} else {
		PTR_JPGC->image.headerReady = 0;
	}

	/* set mcu/slice value */
	PTR_JPGC->info.sliceMbSetValue = pDecIn->sliceMbSet;

	/* check HW supported features */
	if (!PTR_JPGC->is8190) {
		/* return if not valid HW and unsupported operation type */
		if (PTR_JPGC->info.operationType == JPEGDEC_NONINTERLEAVED ||
			PTR_JPGC->info.operationType == JPEGDEC_PROGRESSIVE) {
			JPEGDEC_API_TRC
			("JpegDecDecode# ERROR: Operation type not supported");
			return (JPEGDEC_UNSUPPORTED);
		}

		if (PTR_JPGC->info.getInfoYCbCrMode == JPEGDEC_YCbCr400 ||
			PTR_JPGC->info.getInfoYCbCrMode == JPEGDEC_YCbCr440 ||
			PTR_JPGC->info.getInfoYCbCrMode == JPEGDEC_YCbCr444_SEMIPLANAR) {
			mcuSizeDivider = 2;
		} else {
			mcuSizeDivider = 1;
		}

		/* check slice config */
		if ((pDecIn->sliceMbSet * (JPG_FRM.numMcuInRow / mcuSizeDivider)) >
			PTR_JPGC->maxSupportedSliceSize) {
			JPEGDEC_API_TRC
			("JpegDecDecode# ERROR: sliceMbSet  > JPEGDEC_MAX_SLICE_SIZE");
			return (JPEGDEC_PARAM_ERROR);
		}

		/* check frame size */
		if ((!pDecIn->sliceMbSet) &&
			JPG_FRM.numMcuInFrame > PTR_JPGC->maxSupportedSliceSize) {
			JPEGDEC_API_TRC
			("JpegDecDecode# ERROR: mcuInFrame > JPEGDEC_MAX_SLICE_SIZE");
			return (JPEGDEC_PARAM_ERROR);
		}
	} else {
		/* check if fuse was burned */
		if (PTR_JPGC->fuseBurned) {
			/* return if not valid HW and unsupported operation type */
			if (PTR_JPGC->info.operationType == JPEGDEC_PROGRESSIVE) {
				JPEGDEC_API_TRC
				("JpegDecDecode# ERROR: Operation type not supported");
				return (JPEGDEC_UNSUPPORTED);
			}
		}

		/* check slice config */
		if ((pDecIn->sliceMbSet && pDecIn->decImageType == JPEGDEC_IMAGE &&
			 PTR_JPGC->info.operationType != JPEGDEC_BASELINE) ||
			(pDecIn->sliceMbSet && pDecIn->decImageType == JPEGDEC_THUMBNAIL &&
			 PTR_JPGC->info.operationTypeThumb != JPEGDEC_BASELINE)) {
			JPEGDEC_API_TRC
			("JpegDecDecode# ERROR: Slice mode not supported for this operation type");
			return (JPEGDEC_SLICE_MODE_UNSUPPORTED);
		}

		/* check if frame size over 16M */
		if ((!pDecIn->sliceMbSet) &&
			((JPG_FRM.hwX * JPG_FRM.hwY) > JPEGDEC_MAX_PIXEL_AMOUNT)) {
			JPEGDEC_API_TRC
			("JpegDecDecode# ERROR: Resolution > 16M ==> use slice mode!");
			return (JPEGDEC_PARAM_ERROR);
		}

		if (PTR_JPGC->info.getInfoYCbCrMode == JPEGDEC_YCbCr400 ||
			PTR_JPGC->info.getInfoYCbCrMode == JPEGDEC_YCbCr440 ||
			PTR_JPGC->info.getInfoYCbCrMode == JPEGDEC_YCbCr444_SEMIPLANAR) {
			mcuSizeDivider = 2;
		} else {
			mcuSizeDivider = 1;
		}

		/* check slice config */
		if ((pDecIn->sliceMbSet * (JPG_FRM.numMcuInRow / mcuSizeDivider)) >
			PTR_JPGC->maxSupportedSliceSize) {
			JPEGDEC_API_TRC
			("JpegDecDecode# ERROR: sliceMbSet  > JPEGDEC_MAX_SLICE_SIZE");
			return (JPEGDEC_PARAM_ERROR);
		}
	}

	if (pDecIn->sliceMbSet > 255) {
		JPEGDEC_API_TRC
		("JpegDecDecode# ERROR: sliceMbSet  > Maximum slice size");
		return (JPEGDEC_PARAM_ERROR);
	}

	/* check slice size */
	if (pDecIn->sliceMbSet &&
		!PTR_JPGC->info.SliceReadyForPause && !PTR_JPGC->info.inputBufferEmpty) {
		if (PTR_JPGC->info.getInfoYCbCrMode == JPEGDEC_YCbCr400 ||
			PTR_JPGC->info.getInfoYCbCrMode == JPEGDEC_YCbCr440 ||
			PTR_JPGC->info.getInfoYCbCrMode == JPEGDEC_YCbCr444_SEMIPLANAR) {
			mcuSizeDivider = 2;
		} else {
			mcuSizeDivider = 1;
		}

		if ((pDecIn->sliceMbSet * (JPG_FRM.numMcuInRow / mcuSizeDivider)) >
			JPG_FRM.numMcuInFrame) {
			JPEGDEC_API_TRC
			("JpegDecDecode# ERROR: (sliceMbSet * Number of MCU's in row) > Number of MCU's in frame");
			return (JPEGDEC_PARAM_ERROR);
		}
	}

	/* Handle stream/hw parameters after buffer empty */
	if (PTR_JPGC->info.inputBufferEmpty) {
		/* Store the stream parameters */
		PTR_JPGC->stream.bitPosInByte = 0;
		PTR_JPGC->stream.pCurrPos = (u8 *) pDecIn->streamBuffer.pVirtualAddress;
		PTR_JPGC->stream.streamBus = pDecIn->streamBuffer.busAddress;
		PTR_JPGC->stream.pStartOfStream =
			(u8 *) pDecIn->streamBuffer.pVirtualAddress;

		/* update hw parameters */
		PTR_JPGC->info.inputStreaming = 1;
		if (pDecIn->bufferSize) {
			PTR_JPGC->info.inputBufferLen = pDecIn->bufferSize;
		} else {
			PTR_JPGC->info.inputBufferLen = pDecIn->streamLength;
		}

		/* decoded stream */
		PTR_JPGC->info.decodedStreamLen += PTR_JPGC->info.inputBufferLen;

		if (PTR_JPGC->info.decodedStreamLen > pDecIn->streamLength) {
			JPEGDEC_API_TRC
			("JpegDecDecode# Error: All input stream already processed");
			return JPEGDEC_STRM_ERROR;
		}
	}

	/* update user allocated output */
	PTR_JPGC->info.givenOutLuma.virtualAddress =
		pDecIn->pictureBufferY.pVirtualAddress;
	PTR_JPGC->info.givenOutLuma.busAddress = pDecIn->pictureBufferY.busAddress;
	PTR_JPGC->info.givenOutChroma.virtualAddress =
		pDecIn->pictureBufferCbCr.pVirtualAddress;
	PTR_JPGC->info.givenOutChroma.busAddress =
		pDecIn->pictureBufferCbCr.busAddress;
	PTR_JPGC->info.givenOutChroma2.virtualAddress =
		pDecIn->pictureBufferCr.pVirtualAddress;
	PTR_JPGC->info.givenOutChroma2.busAddress =
		pDecIn->pictureBufferCr.busAddress;

	if (PTR_JPGC->info.progressiveFinish) {
		/* output set */
		if (PTR_JPGC->ppInstance == NULL) {
			pDecOut->outputPictureY.pVirtualAddress =
				PTR_JPGC->info.outLuma.virtualAddress;
			ASSERT(pDecOut->outputPictureY.pVirtualAddress);

			/* output set */
			pDecOut->outputPictureY.busAddress =
				PTR_JPGC->info.outLuma.busAddress;
			ASSERT(pDecOut->outputPictureY.busAddress);

			/* if not grayscale */
			if (PTR_JPGC->image.sizeChroma) {
				pDecOut->outputPictureCbCr.pVirtualAddress =
					PTR_JPGC->info.outChroma.virtualAddress;
				ASSERT(pDecOut->outputPictureCbCr.pVirtualAddress);

				pDecOut->outputPictureCbCr.busAddress =
					PTR_JPGC->info.outChroma.busAddress;
				ASSERT(pDecOut->outputPictureCbCr.busAddress);

				pDecOut->outputPictureCr.pVirtualAddress =
					PTR_JPGC->info.outChroma2.virtualAddress;
				pDecOut->outputPictureCr.busAddress =
					PTR_JPGC->info.outChroma2.busAddress;
			}
		}
		JpegDecInitHWEmptyScan(PTR_JPGC, PTR_JPGC->info.pfCompId);
		dwlret = DWLWaitHwReady(PTR_JPGC->dwl, PTR_JPGC->coreID, PTR_JPGC->info.timeout);
		ASSERT(dwlret == DWL_HW_WAIT_OK);
		JpegRefreshRegs(PTR_JPGC);
		asic_status = GetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_IRQ_STAT);
		ASSERT(asic_status == 1);

		i = PTR_JPGC->info.pfCompId + 1;
		while (i < 3 && PTR_JPGC->info.pfNeeded[i] == 0) {
			i++;
		}
		if (i == 3) {
			PTR_JPGC->info.progressiveFinish = 0;
			return (JPEGDEC_FRAME_READY);
		} else {
			PTR_JPGC->info.pfCompId = i;
			return (JPEGDEC_SCAN_PROCESSED);
		}
	}

	/* check if input streaming used */
	if (!PTR_JPGC->info.SliceReadyForPause &&
		!PTR_JPGC->info.inputBufferEmpty && pDecIn->bufferSize) {
		PTR_JPGC->info.inputStreaming = 1;
		PTR_JPGC->info.inputBufferLen = pDecIn->bufferSize;
		PTR_JPGC->info.decodedStreamLen += PTR_JPGC->info.inputBufferLen;
	}

	/* find markers and go ! */
	do {
		/* if slice mode/slice done return to hw handling */
		if (PTR_JPGC->image.headerReady && PTR_JPGC->info.SliceReadyForPause) {
			break;
		}

		/* Look for marker prefix byte from stream */
		if (JpegDecGetByte(&(PTR_JPGC->stream)) == 0xFF) {
			currentByte = JpegDecGetByte(&(PTR_JPGC->stream));
			while (currentByte == 0xFF) {
				currentByte = JpegDecGetByte(&(PTR_JPGC->stream));
			}

			/* switch to certain header decoding */
			switch (currentByte) {
			case 0x00:
				break;
			case SOF0:
			case SOF2:
				JPEGDEC_API_TRC("JpegDecDecode# JpegDecDecodeFrameHdr decode");
				/* Baseline/Progressive */
				PTR_JPGC->frame.codingType = currentByte;
				/* Set operation type */
				if (PTR_JPGC->frame.codingType == SOF0) {
					PTR_JPGC->info.operationType = JPEGDEC_BASELINE;
				} else {
					PTR_JPGC->info.operationType = JPEGDEC_PROGRESSIVE;
				}

				retCode = JpegDecDecodeFrameHdr(PTR_JPGC);
				if (retCode != JPEGDEC_OK) {
					if (retCode == JPEGDEC_STRM_ERROR) {
						JPEGDEC_API_TRC("JpegDecDecode# ERROR: Stream error");
						return (retCode);
					} else {
						JPEGDEC_API_TRC
						("JpegDecDecode# ERROR: JpegDecDecodeFrameHdr err");
						return (retCode);
					}
				}
				break;
			/* Start of Scan */
			case SOS:
				/* reset image ready */
				PTR_JPGC->image.imageReady = 0;
				JPEGDEC_API_TRC("JpegDecDecode# JpegDecDecodeScan dec");

				retCode = JpegDecDecodeScan(PTR_JPGC);
				PTR_JPGC->image.headerReady = 1;
				if (retCode != JPEGDEC_OK) {
					if (retCode == JPEGDEC_STRM_ERROR) {
						JPEGDEC_API_TRC("JpegDecDecode# ERROR: Stream error");
						return (retCode);
					} else {
						JPEGDEC_API_TRC
						("JpegDecDecode# JpegDecDecodeScan err\n");
						return (retCode);
					}
				}

				if (PTR_JPGC->stream.bitPosInByte) {
					/* delete stuffing bits */
					currentByte = (8 - PTR_JPGC->stream.bitPosInByte);
					if (JpegDecFlushBits
						(&(PTR_JPGC->stream),
						 8 - PTR_JPGC->stream.bitPosInByte) == STRM_ERROR) {
						JPEGDEC_API_TRC("JpegDecDecode# ERROR: Stream error");
						return (JPEGDEC_STRM_ERROR);
					}
				}
				JPEGDEC_API_TRC("JpegDecDecode# Stuffing bits deleted\n");
				break;
			/* Start of Huffman tables */
			case DHT:
				JPEGDEC_API_TRC
				("JpegDecDecode# JpegDecDecodeHuffmanTables dec");
				retCode = JpegDecDecodeHuffmanTables(PTR_JPGC);
				JPEGDEC_API_TRC
				("JpegDecDecode# JpegDecDecodeHuffmanTables stops");
				if (retCode != JPEGDEC_OK) {
					if (retCode == JPEGDEC_STRM_ERROR) {
						JPEGDEC_API_TRC("JpegDecDecode# ERROR: Stream error");
						return (retCode);
					} else {
						JPEGDEC_API_TRC
						("JpegDecDecode# ERROR: JpegDecDecodeHuffmanTables err");
						return (retCode);
					}
				}
				/* mark DHT got from stream */
				PTR_JPGC->DHTfromStream = 1;
				break;
			/* start of Quantisation Tables */
			case DQT:
				JPEGDEC_API_TRC("JpegDecDecode# JpegDecDecodeQuantTables dec");
				retCode = JpegDecDecodeQuantTables(PTR_JPGC);
				if (retCode != JPEGDEC_OK) {
					if (retCode == JPEGDEC_STRM_ERROR) {
						JPEGDEC_API_TRC("JpegDecDecode# ERROR: Stream error");
						return (retCode);
					} else {
						JPEGDEC_API_TRC
						("JpegDecDecode# ERROR: JpegDecDecodeQuantTables err");
						return (retCode);
					}
				}
				break;
			/* Start of Image */
			case SOI:
				/* no actions needed, continue */
				break;
			/* End of Image */
			case EOI:
				if (PTR_JPGC->image.imageReady) {
					JPEGDEC_API_TRC("JpegDecDecode# EOI: OK\n");
					return (JPEGDEC_FRAME_READY);
				} else {
					JPEGDEC_API_TRC("JpegDecDecode# ERROR: EOI: NOK\n");
					return (JPEGDEC_ERROR);
				}
			/* Define Restart Interval */
			case DRI:
				JPEGDEC_API_TRC("JpegDecDecode# DRI");
				currentBytes = JpegDecGet2Bytes(&(PTR_JPGC->stream));
				if (currentBytes == STRM_ERROR) {
					JPEGDEC_API_TRC("JpegDecDecode# ERROR: Read bits ");
					return (JPEGDEC_STRM_ERROR);
				}
				PTR_JPGC->frame.Ri = JpegDecGet2Bytes(&(PTR_JPGC->stream));
				break;
			/* Restart with modulo 8 count m */
			case RST0:
			case RST1:
			case RST2:
			case RST3:
			case RST4:
			case RST5:
			case RST6:
			case RST7:
				/* initialisation of DC predictors to zero value !!! */
				for (i = 0; i < MAX_NUMBER_OF_COMPONENTS; i++) {
					PTR_JPGC->scan.pred[i] = 0;
				}
				JPEGDEC_API_TRC("JpegDecDecode# DC predictors init");
				break;
			/* unsupported features */
			case SOF1:
			case SOF3:
			case SOF5:
			case SOF6:
			case SOF7:
			case SOF9:
			case SOF10:
			case SOF11:
			case SOF13:
			case SOF14:
			case SOF15:
			case DAC:
			case DHP:
			case TEM:
				JPEGDEC_API_TRC("JpegDecDecode# ERROR: Unsupported Features");
				return (JPEGDEC_UNSUPPORTED);
			/* application data & comments */
			case APP0:
				JPEGDEC_API_TRC("JpegDecDecode# APP0 in Decode");
				/* APP0 Extended Thumbnail */
				if (pDecIn->decImageType == JPEGDEC_THUMBNAIL) {
					/* reset */
					appBits = 0;
					appLength = 0;

					/* length */
					appLength = JpegDecGet2Bytes(&(PTR_JPGC->stream));
					appBits += 16;

					/* length > 2 */
					if (appLength < 2) {
						break;
					}

					/* check identifier */
					currentBytes = JpegDecGet2Bytes(&(PTR_JPGC->stream));
					appBits += 16;
					if (currentBytes != 0x4A46) {
						PTR_JPGC->stream.appnFlag = 1;
						if (JpegDecFlushBits
							(&(PTR_JPGC->stream),
							 ((appLength * 8) - appBits)) == STRM_ERROR) {
							JPEGDEC_API_TRC
							("JpegDecDecode# ERROR: Stream error");
							return (JPEGDEC_STRM_ERROR);
						}
						PTR_JPGC->stream.appnFlag = 0;
						break;
					}
					currentBytes = JpegDecGet2Bytes(&(PTR_JPGC->stream));
					appBits += 16;
					if (currentBytes != 0x5858) {
						PTR_JPGC->stream.appnFlag = 1;
						if (JpegDecFlushBits
							(&(PTR_JPGC->stream),
							 ((appLength * 8) - appBits)) == STRM_ERROR) {
							JPEGDEC_API_TRC
							("JpegDecDecode# ERROR: Stream error");
							return (JPEGDEC_STRM_ERROR);
						}
						PTR_JPGC->stream.appnFlag = 0;
						break;
					}
					currentByte = JpegDecGetByte(&(PTR_JPGC->stream));
					appBits += 8;
					if (currentByte != 0x00) {
						PTR_JPGC->stream.appnFlag = 1;
						if (JpegDecFlushBits
							(&(PTR_JPGC->stream),
							 ((appLength * 8) - appBits)) == STRM_ERROR) {
							JPEGDEC_API_TRC
							("JpegDecDecode# ERROR: Stream error");
							return (JPEGDEC_STRM_ERROR);
						}
						PTR_JPGC->stream.appnFlag = 0;
						break;
					}
					/* extension code */
					currentByte = JpegDecGetByte(&(PTR_JPGC->stream));
					PTR_JPGC->stream.appnFlag = 0;
					if (currentByte != JPEGDEC_THUMBNAIL_JPEG) {
						JPEGDEC_API_TRC(("JpegDecDecode# ERROR: thumbnail unsupported"));
						return (JPEGDEC_UNSUPPORTED);
					}
					/* thumbnail mode */
					JPEGDEC_API_TRC("JpegDecDecode# Thumbnail data ok!");
					PTR_JPGC->stream.thumbnail = 1;
					break;
				} else {
					/* Flush unsupported thumbnail */
					currentBytes = JpegDecGet2Bytes(&(PTR_JPGC->stream));

					/* length > 2 */
					if (currentBytes < 2) {
						break;
					}

					PTR_JPGC->stream.appnFlag = 1;
					if (JpegDecFlushBits
						(&(PTR_JPGC->stream),
						 ((currentBytes - 2) * 8)) == STRM_ERROR) {
						JPEGDEC_API_TRC("JpegDecDecode# ERROR: Stream error");
						return (JPEGDEC_STRM_ERROR);
					}
					PTR_JPGC->stream.appnFlag = 0;
					break;
				}
			case DNL:
				JPEGDEC_API_TRC("JpegDecDecode# DNL ==> flush");
				break;
			case APP1:
			case APP2:
			case APP3:
			case APP4:
			case APP5:
			case APP6:
			case APP7:
			case APP8:
			case APP9:
			case APP10:
			case APP11:
			case APP12:
			case APP13:
			case APP14:
			case APP15:
			case COM:
				JPEGDEC_API_TRC("JpegDecDecode# COM");
				currentBytes = JpegDecGet2Bytes(&(PTR_JPGC->stream));
				if (currentBytes == STRM_ERROR) {
					JPEGDEC_API_TRC("JpegDecDecode# ERROR: Read bits ");
					return (JPEGDEC_STRM_ERROR);
				}
				/* length > 2 */
				if (currentBytes < 2) {
					break;
				}
				/* jump over not supported header */
				if (currentBytes != 0) {
					PTR_JPGC->stream.readBits += ((currentBytes * 8) - 16);
					PTR_JPGC->stream.pCurrPos +=
						(((currentBytes * 8) - 16) / 8);
				}
				break;
			default:
				break;
			}
		} else {
			if (currentByte == 0xFFFFFFFF) {
				break;
			}
		}

		if (PTR_JPGC->image.headerReady) {
			break;
		}
	} while ((PTR_JPGC->stream.readBits >> 3) <= PTR_JPGC->stream.streamLength);

	retCode = JPEGDEC_OK;

	/* check if no DHT in stream and if already loaded (MJPEG) */
	if (!PTR_JPGC->DHTfromStream && !PTR_JPGC->vlc.defaultTables) {
		JPEGDEC_API_TRC("JpegDecDecode# No DHT tables in stream, use tables defined in JPEG Standard\n");
		/* use default tables defined in standard */
		JpegDecDefaultHuffmanTables(PTR_JPGC);
	}

	/* Handle decoded image here */
	if (PTR_JPGC->image.headerReady) {
		/* loop until decoding control should return for user */
		do {
			/* if pp enabled ==> set pp control */
			if (PTR_JPGC->ppInstance != NULL) {
				PTR_JPGC->ppConfigQuery.tiledMode = 0;
				PTR_JPGC->PPConfigQuery(PTR_JPGC->ppInstance,
										&PTR_JPGC->ppConfigQuery);

				PTR_JPGC->ppControl.usePipeline =
					PTR_JPGC->ppConfigQuery.pipelineAccepted;

				/* set pp for combined mode */
				if (PTR_JPGC->ppControl.usePipeline) {
					JpegDecPreparePp(PTR_JPGC);
				}
			}

			/* check if we had to load imput buffer or not */
			if (!PTR_JPGC->info.inputBufferEmpty) {
				/* if slice mode ==> set slice height */
				if (PTR_JPGC->info.sliceMbSetValue &&
					PTR_JPGC->ppControl.usePipeline == 0) {
					JpegDecSliceSizeCalculation(PTR_JPGC);
				}

				/* Start HW or continue after pause */
				if (!PTR_JPGC->info.SliceReadyForPause) {
					if (!PTR_JPGC->info.progressiveScanReady ||
						PTR_JPGC->info.nonInterleavedScanReady) {
						JPEGDEC_API_TRC("JpegDecDecode# Start HW init\n");
						retCode = JpegDecInitHW(PTR_JPGC);
						PTR_JPGC->info.nonInterleavedScanReady = 0;
						if (retCode != JPEGDEC_OK) {
							/* return JPEGDEC_HW_RESERVED */
							return retCode;
						}

					} else {
						JPEGDEC_API_TRC
						("JpegDecDecode# Continue HW decoding after progressive scan ready\n");
						JpegDecInitHWProgressiveContinue(PTR_JPGC);
						PTR_JPGC->info.progressiveScanReady = 0;

					}
				} else {
					JPEGDEC_API_TRC
					("JpegDecDecode# Continue HW decoding after slice ready\n");
					JpegDecInitHWContinue(PTR_JPGC);
				}

				PTR_JPGC->info.SliceCount++;
			} else {
				JPEGDEC_API_TRC
				("JpegDecDecode# Continue HW decoding after input buffer has been loaded\n");
				JpegDecInitHWInputBuffLoad(PTR_JPGC);

				/* buffer loaded ==> reset flag */
				PTR_JPGC->info.inputBufferEmpty = 0;
			}

#ifdef JPEGDEC_PERFORMANCE
			dwlret = DWL_HW_WAIT_OK;
#else
			/* wait hw ready */
			dwlret = DWLWaitHwReady(PTR_JPGC->dwl, PTR_JPGC->coreID,
									PTR_JPGC->info.timeout);
#endif /* #ifdef JPEGDEC_PERFORMANCE */

			/* Refresh regs */
			JpegRefreshRegs(PTR_JPGC);

			if (dwlret == DWL_HW_WAIT_OK) {
				JPEGDEC_API_TRC("JpegDecDecode# DWL_HW_WAIT_OK");

#ifdef JPEGDEC_ASIC_TRACE
				{
					JPEGDEC_TRACE_INTERNAL(("\nJpegDecDecode# AFTER DWL_HW_WAIT_OK\n"));
					PrintJPEGReg(PTR_JPGC->jpegRegs);
				}
#endif /* #ifdef JPEGDEC_ASIC_TRACE */

				/* check && reset status */
				asic_status =
					GetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_IRQ_STAT);

				if (asic_status & JPEGDEC_X170_IRQ_BUS_ERROR) {
					/* check PP status... and go */
					if (PTR_JPGC->ppInstance != NULL) {
						/* End PP co-operation */
						if (PTR_JPGC->ppControl.ppStatus == DECPP_RUNNING) {
							JPEGDEC_API_TRC("JpegDecDecode# PP END CALL");
							PTR_JPGC->PPEndCallback(PTR_JPGC->ppInstance);
							PTR_JPGC->ppControl.ppStatus = DECPP_PIC_READY;
						}
					}

					JPEGDEC_API_TRC
					("JpegDecDecode# JPEGDEC_X170_IRQ_BUS_ERROR");
					/* clear interrupts */
					JPEGDEC_CLEAR_IRQ;

					SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_E, 0);
					DWLDisableHW(PTR_JPGC->dwl, PTR_JPGC->coreID, 4 * 1,
								 PTR_JPGC->jpegRegs[1]);

					/* Release HW */
					(void) DWLReleaseHw(PTR_JPGC->dwl, PTR_JPGC->coreID);

					/* update asicRunning */
					PTR_JPGC->asicRunning = 0;

					return JPEGDEC_HW_BUS_ERROR;
				} else if (asic_status & JPEGDEC_X170_IRQ_STREAM_ERROR ||
						   asic_status & JPEGDEC_X170_IRQ_TIMEOUT ||
						   asic_status & DEC_8190_IRQ_ABORT) {
					/* check PP status... and go */
					if (PTR_JPGC->ppInstance != NULL) {
						/* End PP co-operation */
						if (PTR_JPGC->ppControl.ppStatus == DECPP_RUNNING) {
							JPEGDEC_API_TRC("JpegDecDecode# PP END CALL");
							PTR_JPGC->PPEndCallback(PTR_JPGC->ppInstance);
						}

						PTR_JPGC->ppControl.ppStatus = DECPP_PIC_READY;
					}

					if (asic_status & JPEGDEC_X170_IRQ_STREAM_ERROR) {
						JPEGDEC_API_TRC
						("JpegDecDecode# JPEGDEC_X170_IRQ_STREAM_ERROR");
					} else if (asic_status & JPEGDEC_X170_IRQ_TIMEOUT) {
						JPEGDEC_API_TRC
						("JpegDecDecode# JPEGDEC_X170_IRQ_TIMEOUT");
					} else {
						JPEGDEC_API_TRC
						("JpegDecDecode# JPEGDEC_X170_IRQ_ABORT");
					}

					/* clear interrupts */
					JPEGDEC_CLEAR_IRQ;

					SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_E, 0);
					DWLDisableHW(PTR_JPGC->dwl, PTR_JPGC->coreID, 4 * 1,
								 PTR_JPGC->jpegRegs[1]);

					/* Release HW */
					(void) DWLReleaseHw(PTR_JPGC->dwl, PTR_JPGC->coreID);

					/* update asicRunning */
					PTR_JPGC->asicRunning = 0;

					/* output set */
					if (PTR_JPGC->ppInstance == NULL) {
						pDecOut->outputPictureY.pVirtualAddress =
							PTR_JPGC->info.outLuma.virtualAddress;
						ASSERT(pDecOut->outputPictureY.pVirtualAddress);

						/* output set */
						pDecOut->outputPictureY.busAddress =
							PTR_JPGC->info.outLuma.busAddress;
						ASSERT(pDecOut->outputPictureY.busAddress);

						/* if not grayscale */
						if (PTR_JPGC->image.sizeChroma) {
							pDecOut->outputPictureCbCr.pVirtualAddress =
								PTR_JPGC->info.outChroma.virtualAddress;
							ASSERT(pDecOut->outputPictureCbCr.pVirtualAddress);

							pDecOut->outputPictureCbCr.busAddress =
								PTR_JPGC->info.outChroma.busAddress;
							ASSERT(pDecOut->outputPictureCbCr.busAddress);

							pDecOut->outputPictureCr.pVirtualAddress =
								PTR_JPGC->info.outChroma2.virtualAddress;
							pDecOut->outputPictureCr.busAddress =
								PTR_JPGC->info.outChroma2.busAddress;
						}
						pDecOut->cycles_per_mb = JpegCycleCount(PTR_JPGC);
					}

					return JPEGDEC_STRM_ERROR;
				} else if (asic_status & JPEGDEC_X170_IRQ_BUFFER_EMPTY) {
					/* check if frame is ready */
					if (!(asic_status & JPEGDEC_X170_IRQ_DEC_RDY)) {
						JPEGDEC_API_TRC
						("JpegDecDecode# JPEGDEC_X170_IRQ_BUFFER_EMPTY/STREAM PROCESSED");

						/* clear interrupts */
						SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_BUFFER_INT,
									   0);

						/* flag to load buffer */
						PTR_JPGC->info.inputBufferEmpty = 1;

						/* check if all stream should be processed with the
						 * next buffer ==> may affect to some API checks */
						if ((PTR_JPGC->info.decodedStreamLen +
							 PTR_JPGC->info.inputBufferLen) >=
							PTR_JPGC->stream.streamLength) {
							PTR_JPGC->info.streamEndFlag = 1;
						}

						/* output set */
						if (PTR_JPGC->ppInstance == NULL) {
							pDecOut->outputPictureY.pVirtualAddress =
								PTR_JPGC->info.outLuma.virtualAddress;
							ASSERT(pDecOut->outputPictureY.pVirtualAddress);

							/* output set */
							pDecOut->outputPictureY.busAddress =
								PTR_JPGC->info.outLuma.busAddress;
							ASSERT(pDecOut->outputPictureY.busAddress);

							/* if not grayscale */
							if (PTR_JPGC->image.sizeChroma) {
								pDecOut->outputPictureCbCr.pVirtualAddress =
									PTR_JPGC->info.outChroma.virtualAddress;
								ASSERT(pDecOut->outputPictureCbCr.
									   pVirtualAddress);

								pDecOut->outputPictureCbCr.busAddress =
									PTR_JPGC->info.outChroma.busAddress;
								ASSERT(pDecOut->outputPictureCbCr.busAddress);

								pDecOut->outputPictureCr.pVirtualAddress =
									PTR_JPGC->info.outChroma2.virtualAddress;
								pDecOut->outputPictureCr.busAddress =
									PTR_JPGC->info.outChroma2.busAddress;
							}
							pDecOut->cycles_per_mb = JpegCycleCount(PTR_JPGC);
						}

						return JPEGDEC_STRM_PROCESSED;
					}
				}

				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_RDY_INT, 0);
				HINTdec = GetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_IRQ);
				if (HINTdec) {
					JPEGDEC_API_TRC("JpegDecDecode# CLEAR interrupt");
					SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_IRQ, 0);
				}

				/* check if slice ready */
				asicSliceBit = GetDecRegister(PTR_JPGC->jpegRegs,
											  HWIF_JPEG_SLICE_H);
				intDec = GetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_SLICE_INT);

				/* slice ready ==> reset interrupt */
				if (asicSliceBit && intDec) {
					/* if x170 pp not in use */
					if (PTR_JPGC->ppInstance == NULL) {
						PTR_JPGC->info.SliceReadyForPause = 1;
					} else {
						PTR_JPGC->info.SliceReadyForPause = 0;
					}

					if (PTR_JPGC->ppInstance != NULL &&
						!PTR_JPGC->ppControl.usePipeline) {
						PTR_JPGC->info.SliceReadyForPause = 1;
					}

					/* if user allocated memory return given base */
					if (PTR_JPGC->info.userAllocMem == 1 &&
						PTR_JPGC->info.SliceReadyForPause == 1) {
						/* output addresses */
						pDecOut->outputPictureY.pVirtualAddress =
							PTR_JPGC->info.givenOutLuma.virtualAddress;
						pDecOut->outputPictureY.busAddress =
							PTR_JPGC->info.givenOutLuma.busAddress;
						if (PTR_JPGC->image.sizeChroma) {
							pDecOut->outputPictureCbCr.pVirtualAddress =
								PTR_JPGC->info.givenOutChroma.virtualAddress;
							pDecOut->outputPictureCbCr.busAddress =
								PTR_JPGC->info.givenOutChroma.busAddress;
							pDecOut->outputPictureCr.pVirtualAddress =
								PTR_JPGC->info.givenOutChroma2.virtualAddress;
							pDecOut->outputPictureCr.busAddress =
								PTR_JPGC->info.givenOutChroma2.busAddress;
						}
					}

					/* if not user allocated memory return slice base */
					if (PTR_JPGC->info.userAllocMem == 0 &&
						PTR_JPGC->info.SliceReadyForPause == 1) {
						/* output addresses */
						pDecOut->outputPictureY.pVirtualAddress =
							PTR_JPGC->info.outLuma.virtualAddress;
						pDecOut->outputPictureY.busAddress =
							PTR_JPGC->info.outLuma.busAddress;
						if (PTR_JPGC->image.sizeChroma) {
							pDecOut->outputPictureCbCr.pVirtualAddress =
								PTR_JPGC->info.outChroma.virtualAddress;
							pDecOut->outputPictureCbCr.busAddress =
								PTR_JPGC->info.outChroma.busAddress;
							pDecOut->outputPictureCr.pVirtualAddress =
								PTR_JPGC->info.outChroma2.virtualAddress;
							pDecOut->outputPictureCr.busAddress =
								PTR_JPGC->info.outChroma2.busAddress;
						}
					}

					/* No slice output in case decoder + PP (no pipeline) */
					if (PTR_JPGC->ppInstance != NULL &&
						PTR_JPGC->ppControl.usePipeline == 0) {
						/* output addresses */
						pDecOut->outputPictureY.pVirtualAddress = NULL;
						pDecOut->outputPictureY.busAddress = 0;
						if (PTR_JPGC->image.sizeChroma) {
							pDecOut->outputPictureCbCr.pVirtualAddress = NULL;
							pDecOut->outputPictureCbCr.busAddress = 0;
							pDecOut->outputPictureCr.pVirtualAddress = NULL;
							pDecOut->outputPictureCr.busAddress = 0;
						}

						JPEGDEC_API_TRC(("JpegDecDecode# Decoder + PP (Rotation/Flip), Slice ready"));
						/* PP not in pipeline, continue do <==> while */
						PTR_JPGC->info.noSliceIrqForUser = 1;
					} else {
						JPEGDEC_API_TRC(("JpegDecDecode# Slice ready"));
						return JPEGDEC_SLICE_READY;
					}
				} else {
					if (PTR_JPGC->info.operationType == JPEGDEC_PROGRESSIVE ||
						PTR_JPGC->info.operationType == JPEGDEC_NONINTERLEAVED) {
						currentPos =
							GET_ADDR_REG(PTR_JPGC->jpegRegs,
										 HWIF_RLC_VLC_BASE);

						/* update input buffer address */
						PTR_JPGC->stream.pCurrPos = ((u8 *) currentPos - 10);
						PTR_JPGC->stream.bitPosInByte = 0;
						PTR_JPGC->stream.readBits =
							((PTR_JPGC->stream.pCurrPos -
							  PTR_JPGC->stream.pStartOfStream) * 8);

						/* default if data ends */
						endOfImage = 1;

						/* check if last scan is decoded */
						for (i = 0;
							 i <
							 ((PTR_JPGC->stream.streamLength -
							   (PTR_JPGC->stream.readBits / 8))); i++) {
							currentByte = PTR_JPGC->stream.pCurrPos[i];
							if (currentByte == 0xFF) {
								currentByte = PTR_JPGC->stream.pCurrPos[i + 1];
								if (currentByte == 0xD9) {
									endOfImage = 1;
									break;
								} else if (currentByte == 0xC4 ||
										   currentByte == 0xDA) {
									endOfImage = 0;
									break;
								}
							}
						}

						currentByte = 0;
						PTR_JPGC->info.SliceCount = 0;
						PTR_JPGC->info.SliceReadyForPause = 0;

						/* if not the last scan of the stream */
						if (endOfImage == 0) {
							/* output set */
							if (PTR_JPGC->ppInstance == NULL &&
								!PTR_JPGC->info.noSliceIrqForUser) {
								pDecOut->outputPictureY.pVirtualAddress =
									PTR_JPGC->info.outLuma.virtualAddress;
								ASSERT(pDecOut->outputPictureY.pVirtualAddress);

								/* output set */
								pDecOut->outputPictureY.busAddress =
									PTR_JPGC->info.outLuma.busAddress;
								ASSERT(pDecOut->outputPictureY.busAddress);

								/* if not grayscale */
								if (PTR_JPGC->image.sizeChroma) {
									pDecOut->outputPictureCbCr.pVirtualAddress =
										PTR_JPGC->info.outChroma.virtualAddress;
									ASSERT(pDecOut->outputPictureCbCr.
										   pVirtualAddress);

									pDecOut->outputPictureCbCr.busAddress =
										PTR_JPGC->info.outChroma.busAddress;
									ASSERT(pDecOut->outputPictureCbCr.
										   busAddress);

									pDecOut->outputPictureCr.pVirtualAddress =
										PTR_JPGC->info.outChroma2.
										virtualAddress;
									pDecOut->outputPictureCr.busAddress =
										PTR_JPGC->info.outChroma2.busAddress;
								}
								pDecOut->cycles_per_mb = JpegCycleCount(PTR_JPGC);
							}

							/* PP not in pipeline, continue do <==> while */
							PTR_JPGC->info.noSliceIrqForUser = 0;

							if (PTR_JPGC->info.operationType ==
								JPEGDEC_PROGRESSIVE) {
								PTR_JPGC->info.progressiveScanReady = 1;
							} else {
								PTR_JPGC->info.nonInterleavedScanReady = 1;
							}

							/* return control to application if progressive */
							if (PTR_JPGC->info.operationType !=
								JPEGDEC_NONINTERLEAVED) {
								/* non-interleaved scan ==> no output */
								if (PTR_JPGC->info.nonInterleaved == 0) {
									PTR_JPGC->info.noSliceIrqForUser = 1;
								} else {
									JPEGDEC_API_TRC
									("JpegDecDecode# SCAN PROCESSED");
									return (JPEGDEC_SCAN_PROCESSED);
								}
							} else {
								/* set decoded component */
								PTR_JPGC->info.components[PTR_JPGC->info.
														  componentId] = 1;

								/* check if we have decoded all components */
								if (PTR_JPGC->info.components[0] == 1 &&
									PTR_JPGC->info.components[1] == 1 &&
									PTR_JPGC->info.components[2] == 1) {
									/* continue decoding next scan */
									PTR_JPGC->info.noSliceIrqForUser = 0;
									nonInterleavedRdy = 0;
								} else {
									/* continue decoding next scan */
									PTR_JPGC->info.noSliceIrqForUser = 1;
									nonInterleavedRdy = 0;
								}
							}
						} else {
							if (PTR_JPGC->info.operationType ==
								JPEGDEC_NONINTERLEAVED) {
								/* set decoded component */
								PTR_JPGC->info.components[PTR_JPGC->info.
														  componentId] = 1;

								/* check if we have decoded all components */
								if (PTR_JPGC->info.components[0] == 1 &&
									PTR_JPGC->info.components[1] == 1 &&
									PTR_JPGC->info.components[2] == 1) {
									/* continue decoding next scan */
									PTR_JPGC->info.noSliceIrqForUser = 0;
									nonInterleavedRdy = 1;
								} else {
									/* continue decoding next scan */
									PTR_JPGC->info.noSliceIrqForUser = 1;
									nonInterleavedRdy = 0;
								}
							}
						}
					} else {
						/* PP not in pipeline, continue do <==> while */
						PTR_JPGC->info.noSliceIrqForUser = 0;
					}
				}

				if (PTR_JPGC->info.noSliceIrqForUser == 0) {
					/* Release HW */
					(void) DWLReleaseHw(PTR_JPGC->dwl, PTR_JPGC->coreID);
					/* update asicRunning */
					PTR_JPGC->asicRunning = 0;

					JPEGDEC_API_TRC("JpegDecDecode# FRAME READY");

					/* set image ready */
					PTR_JPGC->image.imageReady = 1;
				}

				/* check PP status... and go */
				if (PTR_JPGC->ppInstance != NULL &&
					!PTR_JPGC->info.noSliceIrqForUser) {
					/* set pp for stand alone */
					if (!PTR_JPGC->ppControl.usePipeline) {
						JpegDecPreparePp(PTR_JPGC);

						JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set output write disabled\n"));
						SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_OUT_DIS, 1);

						/* Enable pp */
						JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Enable pp\n"));
						PTR_JPGC->PPRun(PTR_JPGC->ppInstance,
										&PTR_JPGC->ppControl);

						PTR_JPGC->ppControl.ppStatus = DECPP_RUNNING;
						PTR_JPGC->info.pipeline = 1;

						/* Flush regs to hw register */
						JpegFlushRegs(PTR_JPGC);
					}

					/* End PP co-operation */
					if (PTR_JPGC->ppControl.ppStatus == DECPP_RUNNING) {
						JPEGDEC_API_TRC("JpegDecDecode# PP END CALL");
						PTR_JPGC->PPEndCallback(PTR_JPGC->ppInstance);
					}

					PTR_JPGC->ppControl.ppStatus = DECPP_PIC_READY;
				}

				/* output set */
				if (PTR_JPGC->ppInstance == NULL &&
					!PTR_JPGC->info.noSliceIrqForUser) {
					pDecOut->outputPictureY.pVirtualAddress =
						PTR_JPGC->info.outLuma.virtualAddress;
					ASSERT(pDecOut->outputPictureY.pVirtualAddress);

					/* output set */
					pDecOut->outputPictureY.busAddress =
						PTR_JPGC->info.outLuma.busAddress;
					ASSERT(pDecOut->outputPictureY.busAddress);

					/* if not grayscale */
					if (PTR_JPGC->image.sizeChroma) {
						pDecOut->outputPictureCbCr.pVirtualAddress =
							PTR_JPGC->info.outChroma.virtualAddress;
						ASSERT(pDecOut->outputPictureCbCr.pVirtualAddress);

						pDecOut->outputPictureCbCr.busAddress =
							PTR_JPGC->info.outChroma.busAddress;
						ASSERT(pDecOut->outputPictureCbCr.busAddress);

						pDecOut->outputPictureCr.pVirtualAddress =
							PTR_JPGC->info.outChroma2.virtualAddress;
						pDecOut->outputPictureCr.busAddress =
							PTR_JPGC->info.outChroma2.busAddress;
					}
					pDecOut->cycles_per_mb = JpegCycleCount(PTR_JPGC);
				}

#ifdef JPEGDEC_ASIC_TRACE
				{
					JPEGDEC_TRACE_INTERNAL(("\nJpegDecDecode# TEST\n"));
					PrintJPEGReg(PTR_JPGC->jpegRegs);
				}
#endif /* #ifdef JPEGDEC_ASIC_TRACE */

				/* get the current stream address  */
				if (PTR_JPGC->info.operationType == JPEGDEC_PROGRESSIVE ||
					(PTR_JPGC->info.operationType == JPEGDEC_NONINTERLEAVED &&
					 nonInterleavedRdy == 0)) {
					retCode = JpegDecNextScanHdrs(PTR_JPGC);
					if (retCode != JPEGDEC_OK && retCode != JPEGDEC_FRAME_READY) {
						/* return */
						return retCode;
					}
				}
			} else if (dwlret == DWL_HW_WAIT_TIMEOUT) {
				JPEGDEC_API_TRC("SCAN: DWL HW TIMEOUT\n");

				/* Release HW */
				(void) DWLReleaseHw(PTR_JPGC->dwl, PTR_JPGC->coreID);
				/* update asicRunning */
				PTR_JPGC->asicRunning = 0;

				return (JPEGDEC_DWL_HW_TIMEOUT);
			} else if (dwlret == DWL_HW_WAIT_ERROR) {
				JPEGDEC_API_TRC("SCAN: DWL HW ERROR\n");

				/* Release HW */
				(void) DWLReleaseHw(PTR_JPGC->dwl, PTR_JPGC->coreID);
				/* update asicRunning */
				PTR_JPGC->asicRunning = 0;

				return (JPEGDEC_SYSTEM_ERROR);
			}
		} while (!PTR_JPGC->image.imageReady);
	}

	if (PTR_JPGC->image.imageReady) {
		JPEGDEC_API_TRC("JpegDecDecode# IMAGE READY");
		JPEGDEC_API_TRC("JpegDecDecode# OK\n");

		/* reset image status */
		PTR_JPGC->image.imageReady = 0;
		PTR_JPGC->image.headerReady = 0;

		/* reset */
		JpegDecClearStructs(PTR_JPGC, 1);

		if (PTR_JPGC->info.operationType == JPEGDEC_BASELINE) {
			return (JPEGDEC_FRAME_READY);
		} else {
			if (endOfImage == 0) {
				return (JPEGDEC_SCAN_PROCESSED);
			} else {
				if (PTR_JPGC->frame.Nf != 1) {
					/* determine first component that needs to be cheated */
					i = 0;
					while (i < 3 && PTR_JPGC->info.pfNeeded[i] == 0) {
						i++;
					}
					if (i == 3) {
						return (JPEGDEC_FRAME_READY);
					}

					JpegDecInitHWEmptyScan(PTR_JPGC, i++);
					dwlret = DWLWaitHwReady(PTR_JPGC->dwl, PTR_JPGC->coreID,
											PTR_JPGC->info.timeout);
					ASSERT(dwlret == DWL_HW_WAIT_OK);
					JpegRefreshRegs(PTR_JPGC);
					asic_status =
						GetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_IRQ_STAT);
					ASSERT(asic_status == 1);

					while (i < 3 && PTR_JPGC->info.pfNeeded[i] == 0) {
						i++;
					}
					if (i == 3) {
						return (JPEGDEC_FRAME_READY);
					} else {
						PTR_JPGC->info.progressiveFinish = 1;
						PTR_JPGC->info.pfCompId = i;
						return (JPEGDEC_SCAN_PROCESSED);
					}
				} else {
					return (JPEGDEC_FRAME_READY);
				}
			}
		}
	} else {
		JPEGDEC_API_TRC("JpegDecDecode# ERROR\n");
		return (JPEGDEC_ERROR);
	}

#undef JPG_FRM
#undef PTR_JPGC
#undef PTR_INFO
}

/*------------------------------------------------------------------------------

    Function name: JpegDecPreparePp

    Functional description:
        Setup PP interface

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
static void JpegDecPreparePp(JpegDecContainer *pJpegDecCont)
{
	pJpegDecCont->ppControl.picStruct = 0;
	pJpegDecCont->ppControl.topField = 0;

	{
		u32 tmp = GetDecRegister(pJpegDecCont->jpegRegs, HWIF_DEC_OUT_ENDIAN);

		pJpegDecCont->ppControl.littleEndian =
			(tmp == DEC_X170_LITTLE_ENDIAN) ? 1 : 0;
	}
	pJpegDecCont->ppControl.wordSwap = GetDecRegister(pJpegDecCont->jpegRegs,
									   HWIF_DEC_OUTSWAP32_E) ? 1
									   : 0;

	/* pipeline */
	if (pJpegDecCont->ppControl.usePipeline) {
		/* luma */
		pJpegDecCont->ppControl.inputBusLuma = 0;
		/* chroma */
		pJpegDecCont->ppControl.inputBusChroma = 0;
	} else {
		/* luma */
		pJpegDecCont->ppControl.inputBusLuma =
			pJpegDecCont->asicBuff.outLumaBuffer.busAddress;
		/* chroma */
		pJpegDecCont->ppControl.inputBusChroma =
			pJpegDecCont->asicBuff.outChromaBuffer.busAddress;
	}

	/* dimensions */
	pJpegDecCont->ppControl.inwidth =
		pJpegDecCont->ppControl.croppedW = pJpegDecCont->info.X;

	pJpegDecCont->ppControl.inheight =
		pJpegDecCont->ppControl.croppedH = pJpegDecCont->info.Y;
}

/*------------------------------------------------------------------------------

    5.6. Function name: JpegDecGetAPIVersion

         Purpose:       Returns version information about this API

         Input:         void

         Output:        JpegDecApiVersion

------------------------------------------------------------------------------*/
JpegDecApiVersion JpegDecGetAPIVersion()
{
	JpegDecApiVersion ver;

	ver.major = JPG_MAJOR_VERSION;
	ver.minor = JPG_MINOR_VERSION;
	JPEGDEC_API_TRC("JpegDecGetAPIVersion# OK\n");
	return ver;
}

/*------------------------------------------------------------------------------

    5.7. Function name: JpegDecGetBuild

         Purpose:       Returns the SW and HW build information

         Input:         void

         Output:        JpegDecGetBuild

------------------------------------------------------------------------------*/
JpegDecBuild JpegDecGetBuild(void)
{
	JpegDecBuild buildInfo;

	(void)DWLmemset(&buildInfo, 0, sizeof(buildInfo));

	buildInfo.swBuild = HANTRO_DEC_SW_BUILD;
	buildInfo.hwBuild = DWLReadAsicID();

	DWLReadAsicConfig(buildInfo.hwConfig);

	JPEGDEC_API_TRC("JpegDecGetBuild# OK\n");

	return buildInfo;
}

/*------------------------------------------------------------------------------
    5.8. Function name   : jpegRegisterPP
         Description     : Called internally by PP to enable the pipeline
         Return type     : i32 - return 0 for success or a negative error code
         Argument        : const void * decInst - decoder instance
         Argument        : const void  *ppInst - post-processor instance
         Argument        : (*PPRun)(const void *) - decoder calls this to start PP
         Argument        : void (*PPEndCallback)(const void *) - decoder calls this
                           to notify PP that a picture was done.
------------------------------------------------------------------------------*/
i32 jpegRegisterPP(const void *decInst, const void *ppInst,
				   void (*PPRun)(const void *, const DecPpInterface *),
				   void (*PPEndCallback)(const void *),
				   void (*PPConfigQuery)(const void *, DecPpQuery *))
{
	JpegDecContainer *pDecCont;

	pDecCont = (JpegDecContainer *) decInst;

	if (decInst == NULL || pDecCont->ppInstance != NULL ||
		ppInst == NULL || PPRun == NULL || PPEndCallback == NULL) {
		return -1;
	}

	if (pDecCont->asicRunning) {
		return -2;
	}

	pDecCont->ppInstance = ppInst;
	pDecCont->PPEndCallback = PPEndCallback;
	pDecCont->PPRun = PPRun;
	pDecCont->PPConfigQuery = PPConfigQuery;

	return 0;
}

/*------------------------------------------------------------------------------
    5.9. Function name   : jpegUnregisterPP
         Description     : Called internally by PP to disable the pipeline
         Return type     : i32 - return 0 for success or a negative error code
         Argument        : const void * decInst - decoder instance
         Argument        : const void  *ppInst - post-processor instance
------------------------------------------------------------------------------*/
i32 jpegUnregisterPP(const void *decInst, const void *ppInst)
{
	JpegDecContainer *pDecCont;

	pDecCont = (JpegDecContainer *) decInst;

	ASSERT(decInst != NULL && ppInst == pDecCont->ppInstance);

	if (decInst == NULL || ppInst != pDecCont->ppInstance) {
		return -1;
	}

	if (pDecCont->asicRunning) {
		return -2;
	}

	pDecCont->ppInstance = NULL;
	pDecCont->PPEndCallback = NULL;
	pDecCont->PPRun = NULL;

	return 0;
}


JpegDecRet JpegDecAbortAfter(JpegDecInst decInst)
{

#define PTR_JPGC ((JpegDecContainer *) decInst)

	JPEGDEC_API_TRC("JpegDecAbortAfter#");

	/* check pointers */
	if (decInst == NULL) {
		JPEGDEC_API_TRC("JpegDecAbortAfter# ERROR: NULL parameter");
		return (JPEGDEC_PARAM_ERROR);
	}

	/* Stop and release HW */
	if (PTR_JPGC->asicRunning) {
		/* check PP status... and go */
		if (PTR_JPGC->ppInstance != NULL) {
			/* End PP co-operation */
			if (PTR_JPGC->ppControl.ppStatus == DECPP_RUNNING) {
				JPEGDEC_API_TRC("JpegDecAbortAfter# PP END CALL");
				PTR_JPGC->PPEndCallback(PTR_JPGC->ppInstance);
			}

			PTR_JPGC->ppControl.ppStatus = DECPP_PIC_READY;
		}

		/* clear interrupts */
		JPEGDEC_CLEAR_IRQ;

		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_E, 0);
		DWLDisableHW(PTR_JPGC->dwl, PTR_JPGC->coreID, 4 * 1,
					 PTR_JPGC->jpegRegs[1]);
		DWLReleaseHw(PTR_JPGC->dwl, PTR_JPGC->coreID);
		PTR_JPGC->asicRunning = 0;
	}

	/* reset image status */
	PTR_JPGC->image.imageReady = 0;
	PTR_JPGC->image.headerReady = 0;

	/* reset */
	JpegDecClearStructs(PTR_JPGC, 1);

	JPEGDEC_API_TRC("JpegDecAbortAfter# JPEGDEC_OK\n");
	return (JPEGDEC_OK);
}

static u32 JpegCycleCount(JpegDecContainer *dec_cont)
{
	u32 cycles = 0;
	u32 mbs = (NEXT_MULTIPLE(dec_cont->info.X, 16) *
			   NEXT_MULTIPLE(dec_cont->info.Y, 16)) >> 8;

	if (mbs) {
		cycles = GetDecRegister(dec_cont->jpegRegs, HWIF_PERF_CYCLE_COUNT) / mbs;
	}

	return cycles;
}
